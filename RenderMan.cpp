
#include <cstdio>
#include <GL/gl.h>
#include <GL/glu.h>

#include "RenderMan.h"
#include "Logger.h"

using namespace std;

#define SCREEN_WIDTH  640
#define SCREEN_HEIGHT 480
#define SCREEN_BPP     16

RenderMan::RenderMan(Logger* log):
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
    surface = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, videoFlags);

    /* Verify there is a surface */
    if (!surface){
        fprintf( stderr,  "Video mode set failed: %s\n", SDL_GetError( ) );
        Quit();
    }
    
    InitGL();
}

void RenderMan::InitGL()     // Create Some Everyday Functions
{ 
    glShadeModel(GL_SMOOTH);                // Enable Smooth Shading
    glClearColor(0.0f, 0.0f, 0.0f, 0.5f);   // Black Background 
    glClearDepth(1.0f);                     // Depth Buffer Setup 
    glEnable(GL_DEPTH_TEST);                // Enables Depth Testing 
    glDepthFunc(GL_LEQUAL);                 // The Type Of Depth Testing To Do 
    glEnable ( GL_COLOR_MATERIAL ); 
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
}

void RenderMan::Quit(){
    SDL_Quit();
    exit(0);
}

bool RenderMan::RenderPoint(int x, int y, int size, const Color& col){
    char buf[100];
    sprintf(buf, "render point to (%d , %d), size %d", x, y, size);

    LOG_EVENT(buf);
    
}

bool RenderMan::RenderLine(int x, int y, int width, int length, const Color& col){
    char buf[100];
    sprintf(buf, "render line to (%d , %d), width: %d, length: %d", x, y, width, length);

    LOG_EVENT(buf);
}

bool RenderMan::RenderLine2(int start_x, int start_y, int end_x, int end_y, 
                            int width, const Color& col)
{
    char buf[100];
    sprintf(buf, "render line from (%d, %d) to (%d , %d), width: %d", start_x, start_y, end_x, end_y, width);

    LOG_EVENT(buf);
}

bool RenderMan::RenderRectangle(int x, int y, int width, int length, const Color& col){
    char buf[100];
    sprintf(buf, "render rectangle with left-top @ (%d , %d), width: %d, length: %d", x, y, width, length);

    LOG_EVENT(buf);
}

void RenderMan::Flush(){
    LOG_EVENT("Flush to buffer");
    glFlush();
}
