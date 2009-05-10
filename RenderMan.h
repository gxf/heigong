
#ifndef RENDER_MAN_H
#define RENDER_MAN_H

#include "SDL/SDL.h"

class Logger;
class Color;

class RenderMan{
    public:
        RenderMan(Logger* log);
        ~RenderMan();

    public:
        void Init();

        bool RenderPoint(int x, int y, int size, const Color& col);
        bool RenderLine(int x, int y, int width, int length, const Color& col);
        bool RenderLine2(int start_x, int start_y, int end_x, int end_y, 
                         int width, const Color& col);
        bool RenderRectangle(int x, int y, int width, int length, const Color& col);

        void Flush();

        void Quit();
    private:
        void InitGL();

    private:
        int                     videoFlags;// Flags to pass to SDL_SetVideoMode
        const SDL_VideoInfo*    videoInfo; // holds some info about our display
        SDL_Surface *           surface;   // SDL surface

        Logger* logger;
};

#endif

