#include "Logger.h"
#include "RenderMan.h"
#include "FontsManager.h"
#include "LayoutManager.h"
#include "MayTwelfth.h"

const int May12th::screen_width = 640;
const int May12th::screen_height= 480;

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
        char buf[100];
        sprintf(buf, "Current char: %c", *ch);
        LOG_EVENT(buf);

        FT_Bitmap* bitmap = fonts.GetBitmap((FT_ULong)*ch);
        Position pos = 
            layout.GetProperPos(LayoutManager::GT_CHAR, bitmap->width,bitmap->rows);
        render.RenderBitMap(pos.x, pos.y, bitmap->width, bitmap->rows, bitmap->buffer);
        ch++;
    }
    return true;
}

