/*
    SDL - Simple DirectMedia Layer
    Copyright (C) 1997-2006 Sam Lantinga

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

    Sam Lantinga
    slouken@libsdl.org
*/
#include "SDL_config.h"

/* PGM SDL video driver implementation: dump PGM files upon SDL_UpdateRect(s) calls */

#include "SDL_video.h"
#include "SDL_mouse.h"
#include "../SDL_sysvideo.h"
#include "../SDL_pixels_c.h"
#include "../../events/SDL_events_c.h"

#include "SDL_pgmvideo.h"
#include "SDL_nullevents_c.h"
#include "SDL_nullmouse_c.h"

#define PGMVID_DRIVER_NAME "pgm"

/* Initialization/Query functions */
static int PGM_VideoInit(_THIS, SDL_PixelFormat *vformat);
static SDL_Rect **PGM_ListModes(_THIS, SDL_PixelFormat *format, Uint32 flags);
static SDL_Surface *PGM_SetVideoMode(_THIS, SDL_Surface *current, int width, int height, int bpp, Uint32 flags);
static int PGM_SetColors(_THIS, int firstcolor, int ncolors, SDL_Color *colors);
static void PGM_VideoQuit(_THIS);

/* Hardware surface functions */
static int PGM_AllocHWSurface(_THIS, SDL_Surface *surface);
static int PGM_LockHWSurface(_THIS, SDL_Surface *surface);
static void PGM_UnlockHWSurface(_THIS, SDL_Surface *surface);
static void PGM_FreeHWSurface(_THIS, SDL_Surface *surface);

/* etc. */
static void PGM_UpdateRects(_THIS, int numrects, SDL_Rect *rects);

/* PGM driver bootstrap functions */

static int PGM_Available(void)
{
	const char *envr = SDL_getenv("SDL_VIDEODRIVER");
	if ((envr) && (SDL_strcmp(envr, PGMVID_DRIVER_NAME) == 0)) {
		return(1);
	}

	return(0);
}

static void PGM_DeleteDevice(SDL_VideoDevice *device)
{
	SDL_free(device->hidden);
	SDL_free(device);
}

static SDL_VideoDevice *PGM_CreateDevice(int devindex)
{
	SDL_VideoDevice *device;

	/* Initialize all variables that we clean on shutdown */
	device = (SDL_VideoDevice *)SDL_malloc(sizeof(SDL_VideoDevice));
	if ( device ) {
		SDL_memset(device, 0, (sizeof *device));
		device->hidden = (struct SDL_PrivateVideoData *)
				SDL_malloc((sizeof *device->hidden));
	}
	if ( (device == NULL) || (device->hidden == NULL) ) {
		SDL_OutOfMemory();
		if ( device ) {
			SDL_free(device);
		}
		return(0);
	}
	SDL_memset(device->hidden, 0, (sizeof *device->hidden));

	/* Set the function pointers */
	device->VideoInit = PGM_VideoInit;
	device->ListModes = PGM_ListModes;
	device->SetVideoMode = PGM_SetVideoMode;
	device->CreateYUVOverlay = NULL;
	device->SetColors = PGM_SetColors;
	device->UpdateRects = PGM_UpdateRects;
	device->VideoQuit = PGM_VideoQuit;
	device->AllocHWSurface = PGM_AllocHWSurface;
	device->CheckHWBlit = NULL;
	device->FillHWRect = NULL;
	device->SetHWColorKey = NULL;
	device->SetHWAlpha = NULL;
	device->LockHWSurface = PGM_LockHWSurface;
	device->UnlockHWSurface = PGM_UnlockHWSurface;
	device->FlipHWSurface = NULL;
	device->FreeHWSurface = PGM_FreeHWSurface;
	device->SetCaption = NULL;
	device->SetIcon = NULL;
	device->IconifyWindow = NULL;
	device->GrabInput = NULL;
	device->GetWMInfo = NULL;
	device->InitOSKeymap = DUMMY_InitOSKeymap;
	device->PumpEvents = DUMMY_PumpEvents;

	device->free = PGM_DeleteDevice;

	return device;
}

VideoBootStrap PGM_bootstrap = {
	PGMVID_DRIVER_NAME, "SDL pgm video driver",
	PGM_Available, PGM_CreateDevice
};


int PGM_VideoInit(_THIS, SDL_PixelFormat *vformat)
{
	/* Determine the screen depth (use default 8-bit depth) */
	/* we change this during the SDL_SetVideoMode implementation... */
	vformat->BitsPerPixel = 8;
	vformat->BytesPerPixel = 1;

	/* We're done! */
	return(0);
}

SDL_Rect **PGM_ListModes(_THIS, SDL_PixelFormat *format, Uint32 flags)
{
   	 return (SDL_Rect **) -1;
}

SDL_Surface *PGM_SetVideoMode(_THIS, SDL_Surface *current,
				int width, int height, int bpp, Uint32 flags)
{
	if ( this->hidden->buffer ) {
		SDL_free( this->hidden->buffer );
	}

	this->hidden->buffer = SDL_malloc(width * height * (bpp / 8));
	if ( ! this->hidden->buffer ) {
		SDL_SetError("Couldn't allocate buffer for requested mode");
		return(NULL);
	}

/* 	printf("Setting mode %dx%d\n", width, height); */

	SDL_memset(this->hidden->buffer, 0, width * height * (bpp / 8));

	/* Allocate the new pixel format for the screen */
	if ( ! SDL_ReallocFormat(current, bpp, 0, 0, 0, 0) ) {
		SDL_free(this->hidden->buffer);
		this->hidden->buffer = NULL;
		SDL_SetError("Couldn't allocate new pixel format for requested mode");
		return(NULL);
	}

	/* Set up the new mode framebuffer */
	current->flags = flags & SDL_FULLSCREEN;
	this->hidden->w = current->w = width;
	this->hidden->h = current->h = height;
	current->pitch = current->w * (bpp / 8);
	current->pixels = this->hidden->buffer;

	/* We're done */
	return(current);
}

/* We don't actually allow hardware surfaces other than the main one */
static int PGM_AllocHWSurface(_THIS, SDL_Surface *surface)
{
	return(-1);
}
static void PGM_FreeHWSurface(_THIS, SDL_Surface *surface)
{
	return;
}

/* We need to wait for vertical retrace on page flipped displays */
static int PGM_LockHWSurface(_THIS, SDL_Surface *surface)
{
	return(0);
}

static void PGM_UnlockHWSurface(_THIS, SDL_Surface *surface)
{
	return;
}

static void PGM_UpdateRects(_THIS, int numrects, SDL_Rect *rects)
{
	/* do nothing. */
}

int PGM_SetColors(_THIS, int firstcolor, int ncolors, SDL_Color *colors)
{
	/* do nothing of note. */
	return(1);
}

/* Note:  If we are terminated, this could be called in the middle of
   another SDL video routine -- notably UpdateRects.
*/
void PGM_VideoQuit(_THIS)
{
	if (this->screen->pixels != NULL)
	{
		SDL_free(this->screen->pixels);
		this->screen->pixels = NULL;
	}
}
