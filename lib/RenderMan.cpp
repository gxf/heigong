
#include <cstdio>
#include <GL/glew.h>

#include "Common.h"
#include "Logger.h"
#include "Page.h"
#include "RenderMan.h"
#include "Color.h"

using namespace std;

RenderMan::RenderMan(Logger* log, int w, int h, int bpp):
    screen_width(w), screen_height(h), screen_bpp(bpp),
    fb(w, h), logger(log)
{
}

RenderMan::~RenderMan(){
}

void RenderMan::Init(){
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
}

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

void RenderMan::Quit(){
    LOG_EVENT("Render terminated.");

    SDL_Quit();
//    exit(0);
}

void RenderMan::Clear(){
//    glClear(GL_COLOR_BUFFER_BIT);
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

    Position pos(x, SCREEN_HEIGHT - y);
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

    Position pos(x, SCREEN_HEIGHT - (y + length));
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

    Position pos(x, SCREEN_HEIGHT - y);
    fb.Write(pos, width, height, pixmap);
    return true;
}

bool RenderMan::RenderPixMap(int x, int y, int width, int height, void* pixmap){
    char buf[100];
    sprintf(buf, "render pixmap with left-top @ (%d , %d), width: %d, height: %d", x, y, width, height);
    LOG_EVENT(buf);
#if 0
    glColor3f(1.0f, 1.0f, 0.0f);
    glWindowPos2i(x, SCREEN_HEIGHT - y);
    glDrawPixels(width, height, GL_RGB, GL_UNSIGNED_BYTE, (GLubyte*)pixmap);
#endif

    return true;
}

static void RenderToFile(void* pFb, uint32 width, uint32 height, const char* filen){
    std::ofstream of(filen);
    uint32 i, j;
//    uint32 start = 0;
    uint8* p = (uint8*)pFb;
    for(i = 0; i < height; i++){
        for(j = 0; j < width; j += 4){
            uint32 buf = (uint32)(*p & 0xf0) >> 4       | 
                         (uint32)(*(p + 1) & 0xf0)      | 
                         (uint32)(*(p + 2) & 0xf0) << 4 | 
                         (uint32)(*(p + 3) & 0xf0) << 8;
            of << buf;
            p += 4;
        }
        // Not necessary
#if 0
        j = start;
        for(j = 0; j < width; j += 2){
            uint8 buf = (uint32)(*p & 0xf0) >> 4  | 
                         (uint32)(*(p + 1) & 0xf0);
            of << buf;
            p += 2;
        }
        if ( j == width){
            start = 1;
        }
        else{
            start = 0;
        }
#endif
    }
    of.close();
}

void RenderMan::Flush(){
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
    RenderToFile(pFb, width, height, "framebuffer.fb");
#endif
}

void RenderMan::GetFBSize(Page* pg){
    pg -> SetSize(SCREEN_WIDTH, SCREEN_HEIGHT);
}

void RenderMan::GetFrameBuffer(Page* pg){
    glFlush();
//    glReadBuffer(GL_FRONT);
    glReadPixels(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, GL_LUMINANCE, GL_UNSIGNED_BYTE, pg -> GetFB());
}

