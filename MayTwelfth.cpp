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
//        char buf[100];
//        sprintf(buf, "Current char: %c", *ch);
//        LOG_EVENT(buf);

        FT_Bitmap* bitmap;
        Position topLeft, advance;
        fonts.GetBitmap((FT_ULong)*ch, &bitmap, &topLeft, &advance);

        Position pos = 
            layout.GetProperPos(LayoutManager::GT_CHAR, bitmap->width, bitmap->rows);
        char p[bitmap->pitch * bitmap->rows];
        std::memcpy(p, bitmap->buffer, bitmap->pitch * bitmap->rows);
        fontsCache.AdjustBitmap(bitmap->pitch, bitmap->rows, p);

        render.RenderGrayMap(pos.x, pos.y, bitmap->pitch, bitmap->rows, p);
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
                    fonts.SetFontSize(32);
                    layout.NewPage();
                    RenderString("Hello, world!");
                    layout.NewLine();
                    fonts.SetFontSize(40);
                    RenderString("This is Garfiled's X Flavor!");
                    layout.NewLine();
                    layout.NewLine();
                    fonts.SetFontSize(15);
                    RenderString("There is a place, in your heart. And every one knows its name is love. Please do remember the people left us on May. 12th, 2008. And please do remember take care of the still alives!");
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
