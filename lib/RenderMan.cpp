
#include <cstdio>
#include <GL/glew.h>

#include "RenderMan.h"
#include "Color.h"
#include "Common.h"
#include "Logger.h"

using namespace std;

RenderMan::RenderMan(Logger* log, int w, int h, int bpp):
    screen_width(w), screen_height(h), screen_bpp(bpp),
    logger(log)
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
    glClear(GL_COLOR_BUFFER_BIT);
}

bool RenderMan::RenderPoint(int x, int y, int size, Color & col){
    char buf[100];
    sprintf(buf, "render point to (%d , %d), size %d", x, y, size);
    LOG_EVENT(buf);

    char point[size * size];
    memset(point, 0xffffffff, sizeof(point));

    glColor3b(col.R, col.G, col.B);
    glWindowPos2i(x, y);
    glBitmap(size, size, 0, 0, size, 0, (GLubyte*)point);
    glFinish();

    return true;
}

bool RenderMan::RenderLine(int x, int y, int width, int length, Color & col){
    char buf[100];
    sprintf(buf, "render line to (%d , %d), width: %d, length: %d", x, y, width, length);
    LOG_EVENT(buf);
    //

    char line[width * length];
    memset(line, 0xffffffff, sizeof(line));

    glColor3b(col.R, col.G, col.B);
    glWindowPos2i(x, y);
    glBitmap(width, length, 0, 0, width, 0, (GLubyte*)line);
    glFinish();

    return true;
}

bool RenderMan::RenderLine2(int start_x, int start_y, int end_x, int end_y, 
                            int width, Color & col)
{
    char buf[100];
    sprintf(buf, "render line from (%d, %d) to (%d , %d), width: %d", start_x, start_y, end_x, end_y, width);

    LOG_EVENT(buf);
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

    glWindowPos2i(x, y);
    glBitmap(width, height, 0, 0, width, 0, (GLubyte*)bitmap);
    glFinish();
    return true;
}

bool RenderMan::RenderGrayMap(int x, int y, int width, int height, void* pixmap){
    /*
    char buf[100];
    sprintf(buf, "render graymap with left-bottom @ (%d , %d), width: %d, height: %d", x, y, width, height);
    LOG_EVENT(buf);
    */

    glColor3f(1.0f, 1.0f, 0.0f);
    glWindowPos2i(x, SCREEN_HEIGHT - y);
    glDrawPixels(width, height, GL_LUMINANCE, GL_UNSIGNED_BYTE, (GLubyte*)pixmap);
    return true;
}

bool RenderMan::RenderPixMap(int x, int y, int width, int height, void* pixmap){
    char buf[100];
    sprintf(buf, "render pixmap with left-top @ (%d , %d), width: %d, height: %d", x, y, width, height);

    return true;
}

void RenderMan::Flush(){
//    LOG_EVENT("Flush to buffer");
    glFlush();
    SDL_GL_SwapBuffers();
}

