
#ifndef RENDER_MAN_H
#define RENDER_MAN_H

#ifndef RENDER2FILE
#include "SDL/SDL.h"
#endif

#include "Common.h"
#include "FrameBuffer.h"

class Logger;
class Color;
class Page;
class BufferManager;

class RenderMan{
    public:
        RenderMan(Logger* log, int w, int h, int bpp = 16);
        ~RenderMan();

    public:
        void Init();

        bool RenderPoint(int x, int y, uint32 size, Color& col);
        bool RenderHorizLine(int x, int y, uint32 width, uint32 length, Color & col);
        bool RenderVerticLine(int x, int y, uint32 width, uint32 length, Color& col);
        bool RenderRectangle(int x, int y, int width, int height, Color & col);
        bool RenderBitMap(int x, int y, int width, int height, void* ptr);
        bool RenderGrayMap(int x, int y, int width, int height, void* ptr);
        bool RenderPixMap(int x, int y, int width, int height, void* ptr);

        void GetFBSize(Page*);
        void GetFrameBuffer(Page*); 
        void Clear();
        void* Flush(BufferManager *);

        void Quit();

#ifndef RENDER2FILE
    private:
        void InitGL();
#endif

    private:
#ifndef RENDER2FILE
        int                     videoFlags;// Flags to pass to SDL_SetVideoMode
        const SDL_VideoInfo*    videoInfo; // holds some info about our display
        SDL_Surface *           surface;   // SDL surface
#endif

        int screen_width;
        int screen_height;
        int screen_bpp;

    private:
        FrameBuffer fb;
        Logger* logger;
};

#endif

