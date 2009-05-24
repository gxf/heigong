
#ifndef RENDER_MAN_H
#define RENDER_MAN_H

#include "SDL/SDL.h"

class Logger;
class Color;
class Page;

class RenderMan{
    public:
        RenderMan(Logger* log, int w, int h, int bpp = 16);
        ~RenderMan();

    public:
        void Init();

        bool RenderPoint(int x, int y, int size, Color& col);
        bool RenderLine(int x, int y, int width, int length, Color& col);
        bool RenderLine2(int start_x, int start_y, int end_x, int end_y, 
                         int width, Color & col);
        bool RenderRectangle(int x, int y, int width, int height, Color & col);
        bool RenderBitMap(int x, int y, int width, int height, void* ptr);
        bool RenderGrayMap(int x, int y, int width, int height, void* ptr);
        bool RenderPixMap(int x, int y, int width, int height, void* ptr);

        void GetFBSize(Page*);
        void GetFrameBuffer(Page*); 
        void Clear();
        void Flush();

        void Quit();
    private:
        void InitGL();

    private:
        int                     videoFlags;// Flags to pass to SDL_SetVideoMode
        const SDL_VideoInfo*    videoInfo; // holds some info about our display
        SDL_Surface *           surface;   // SDL surface

        int screen_width;
        int screen_height;
        int screen_bpp;

    private:
        Logger* logger;
};

#endif

