#include "Common.h"
#include "Logger.h"
#include "RenderMan.h"
#include "FontsManager.h"
#include "FontsCache.h"
#include "LayoutManager.h"
#include "MayTwelfth.h"
#include <cstdlib>
#include <cstring>

const int May12th::screen_width = SCREEN_WIDTH;
const int May12th::screen_height= SCREEN_HEIGHT;

May12th::May12th(Logger* log):
    logger(log), fonts(log), 
    layout(screen_width, screen_height, 30, 30, log), 
    render(log, screen_width, screen_height) 
{
    render.Init();
}

May12th::~May12th(){
    render.Quit();
}

bool May12th::RenderString(const char* str){
    const char* ch = str;
    if (NULL == ch)
        return true;

    while('\0' != *ch){
/*        char buf[100];
        sprintf(buf, "Current char: %c", *ch);
        LOG_EVENT(buf);
        */

        FT_Bitmap* bitmap; 
        FT_Glyph_Metrics* metrics;
        Position topLeft, advance;
        fonts.GetBitmap((FT_ULong)*ch, &bitmap, &metrics, &topLeft, &advance);

        Position pos = 
            layout.GetProperPos(LayoutManager::GT_CHAR, advance.x >> 6, bitmap->rows, metrics->horiBearingY >> 6);

//        sprintf(buf, "Got pos form LayoutManager @ (%d, %d)", pos.x, pos.y);
//        LOG_EVENT(buf);

        char p[bitmap->pitch * bitmap->rows];
        std::memcpy(p, bitmap->buffer, bitmap->pitch * bitmap->rows);
        fontsCache.AdjustBitmap(bitmap->pitch, bitmap->rows, p);

        render.RenderGrayMap(pos.x + (metrics->horiBearingX >> 6), pos.y, bitmap->pitch, bitmap->rows, p);
        ch++;
    }
    return true;
}

void May12th::MainLoop(){
    int done = false;
    SDL_Event event;

    while (!done){ 
        while (SDL_PollEvent(&event))
        {
            switch(event.type){
                case SDL_ACTIVEEVENT: 
                    render.Clear();
                    layout.NewPage();
                    fonts.SetFontSize(32);
                    layout.SetLineSpacing(32/4);
                    RenderString("Hello, world!");
                    layout.NewLine();
                    fonts.SetFontSize(40);
                    layout.SetLineSpacing(40/4);
                    RenderString("This is Garfiled's X Flavor!");
                    layout.NewLine();
                    layout.NewLine();
                    fonts.SetFontSize(15);
                    layout.SetLineSpacing(15/4);
                    RenderString("There is a place, in your heart. And everyone knows its name is love. Please do remember the people left us on May. 12th, 2008. And please do remember taking care of the still alives!");
                    render.Flush();
                    break;              
                case SDL_VIDEORESIZE:
                    break;
                case SDL_KEYDOWN: 
                    // handle key presses
                    switch (event.key.keysym.sym)
                    {
                        case SDLK_ESCAPE:
                            done = true;
                            break;
                        default:
                            break;
                    }
                    break;
                case SDL_QUIT: // handle quit requests
                    done = true;
                    break;
                default:
                    break;
            }
        }
    } 
}
