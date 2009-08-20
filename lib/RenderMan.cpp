
#include <cstdio>

#ifndef RENDER2FILE
#include <GL/glew.h>
#endif

#include "Common.h"
#include "Logger.h"
#include "Page.h"
#include "RenderMan.h"
#include "Color.h"
#include "BufferManager.h"

using namespace std;

RenderMan::RenderMan(Logger* log, int w, int h, int bpp):
    screen_width(w), screen_height(h), screen_bpp(bpp),
    fb(w, h), logger(log)
{
}

RenderMan::~RenderMan(){
}

void RenderMan::Init(){
#ifndef RENDER2FILE
    /* initialize SDL */
    if (SDL_Init(SDL_INIT_VIDEO) < 0){
        fprintf(stderr, "Video initialization failed: %s\n", SDL_GetError());
        Quit(); 
    } 
    /* Fetch the video info */
    videoInfo = SDL_GetVideoInfo();

    if (!videoInfo){
        fprintf(stderr, "Video query failed: %s\n", SDL_GetError());
        Quit();
    }
    /* the flags to pass to SDL_SetVideoMode */
    videoFlags  = SDL_OPENGL;          /* Enable OpenGL in SDL */
    videoFlags |= SDL_GL_DOUBLEBUFFER; /* Enable double buffering */
    videoFlags |= SDL_HWPALETTE;       /* Store the palette in hardware */
    videoFlags |= SDL_RESIZABLE;       /* Enable window resizing */
    
    /* This checks to see if surfaces can be stored in memory */
    if ( videoInfo->hw_available )
        videoFlags |= SDL_HWSURFACE;
    else
        videoFlags |= SDL_SWSURFACE;

    /* This checks if hardware blits can be done */
    if ( videoInfo->blit_hw )
        videoFlags |= SDL_HWACCEL;

    /* Sets up OpenGL double buffering */
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    /* get a SDL surface */
    surface = SDL_SetVideoMode(screen_width, screen_height, screen_bpp, videoFlags);

    /* Verify there is a surface */
    if (!surface){
        fprintf( stderr,  "Video mode set failed: %s\n", SDL_GetError( ) );
        Quit();
    }
    
    InitGL();
#endif
}

#ifndef RENDER2FILE
void RenderMan::InitGL()     // Create Some Everyday Functions
{ 
    LOG_EVENT("Initializing GL.");
    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
        LOG_ERROR("GLew init fails.");
        Quit();
        exit(0);
    }
    char buf[100];
    sprintf(buf, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
    LOG_EVENT(buf);

    glShadeModel(GL_SMOOTH);                // Enable Smooth Shading
    glClearColor(1.0f, 1.0f, 1.0f, 0.5f);   // Black Background 
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glClear(GL_COLOR_BUFFER_BIT);
    glFlush();
}
#endif

void RenderMan::Quit(){
    LOG_EVENT("Render terminated.");

#ifndef RENDER2FILE
    SDL_Quit();
#endif
}

void RenderMan::Clear(){
    fb.Clear();
}

bool RenderMan::RenderPoint(int x, int y, uint32 size, Color & col){
    char buf[100];
    sprintf(buf, "render point to (%d , %d), size %d", x, y, size);
    LOG_EVENT(buf);

    return true;
}

bool RenderMan::RenderHorizLine(int x, int y, uint32 width, uint32 length, Color & col){
#if 0
    char buf[100];
    sprintf(buf, "render horizontal line to (%d , %d), width: %d, length: %d", x, y, width, length);
    LOG_EVENT(buf);
#endif

    uint8 line[width * length];
    memset(line, 0x0, sizeof(line));

#ifndef NOGL
    Position pos(x, scr_height - y);
#else
    Position pos(x, y);
#endif
    fb.Write(pos, length, width, line);
    return true;
}

bool RenderMan::RenderVerticLine(int x, int y, uint32 width, uint32 length, Color & col){
#if 0
    char buf[100];
    sprintf(buf, "render vertical line from (%d, %d) width: %d, length: %d.", x, y, width, length);
    LOG_EVENT(buf);
#endif

    uint8 line[width * length];
    memset(line, 0x0, sizeof(line));

#ifndef NOGL
    Position pos(x, scr_height - (y + length));
#else
    Position pos(x, y + length);
#endif
    fb.Write(pos, width, length, line);

    return true;
}

bool RenderMan::RenderRectangle(int x, int y, int width, int length, Color & col){
    char buf[100];
    sprintf(buf, "render rectangle with left-top @ (%d , %d), width: %d, length: %d", x, y, width, length);

    LOG_EVENT(buf);
    return true;
}

bool RenderMan::RenderBitMap(int x, int y, int width, int height, void* bitmap){
    char buf[100];
    sprintf(buf, "render bitmap with left-top @ (%d , %d), width: %d, height: %d", x, y, width, height);

    LOG_EVENT(buf);

#if 0
    glWindowPos2i(x, y);
    glBitmap(width, height, 0, 0, width, 0, (GLubyte*)bitmap);
    glFinish();
#endif

    return true;
}

bool RenderMan::RenderGrayMap(int x, int y, int width, int height, void* pixmap){
#if 0
    char buf[100];
    sprintf(buf, "render graymap with left-bottom @ (%d , %d), width: %d, height: %d", x, y, width, height);
    LOG_EVENT(buf);
#endif

#ifndef NOGL
    Position pos(x, scr_height - y);
#else
    Position pos(x, y);
#endif
    fb.Write(pos, width, height, pixmap);
    return true;
}

bool RenderMan::RenderPixMap(int x, int y, int width, int height, void* pixmap){
    char buf[100];
    sprintf(buf, "render pixmap with left-top @ (%d , %d), width: %d, height: %d", x, y, width, height);
    LOG_EVENT(buf);
#if 0
    glColor3f(1.0f, 1.0f, 0.0f);
    glWindowPos2i(x, scr_height - y);
    glDrawPixels(width, height, GL_RGB, GL_UNSIGNED_BYTE, (GLubyte*)pixmap);
#endif

    return true;
}

#ifdef RENDER2FILE
static void RenderToFile(void* pFb, uint32 width, uint32 height, const char* filen){
    std::ofstream of(filen);
    char header[100];
    sprintf(header, "P5 %d %d 255 \n", width, height);
    of << header;
    of.write((char*)pFb, width * height);
    of.close();
}
#endif

void* RenderMan::Flush(BufferManager * bufMgr){
//    LOG_EVENT("Flush to buffer");

    uint32 width, height;
    uint8* pFb;
    fb.GetFB(&pFb, &width, &height);

#ifndef RENDER2FILE
//    glWindowPos2i(0, 0);
    glRasterPos2i(-1, -1);
    glColor3f(1.0f, 1.0f, 0.0f);

    char buf[100];
    sprintf(buf, "render pixmap with left-top @ (%d , %d), width: %d, height: %d", 0, 0, width, height);
    LOG_EVENT(buf);

    glDrawPixels(width, height, GL_LUMINANCE, GL_UNSIGNED_BYTE, (GLubyte*)pFb);
    glFlush();
    SDL_GL_SwapBuffers();
#else
//    RenderToFile(pFb, width, height, "framebuffer.pgm");
#endif
#ifdef API_BASED
    if (bufMgr){
        return bufMgr->Insert(pFb, width, height, 8);
    }
    else
        return NULL;
#else
    return NULL;
#endif
}

void RenderMan::GetFBSize(Page* pg){
    pg -> SetSize(scr_width, scr_height);
}

void RenderMan::GetFrameBuffer(Page* pg){
#ifndef RENDER2FILE
    glFlush();
//    glReadBuffer(GL_FRONT);
    glReadPixels(0, 0, scr_width, scr_height, GL_LUMINANCE, GL_UNSIGNED_BYTE, pg -> GetFB());
#endif
}

