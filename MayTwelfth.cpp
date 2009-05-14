#include "Common.h"
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

    render.Clear();
    while('\0' != *ch){
        char buf[100];
        sprintf(buf, "Current char: %c", *ch);
        LOG_EVENT(buf);

        FT_Bitmap* bitmap;
        Position topLeft, advance;
        fonts.GetBitmap((FT_ULong)*ch, &bitmap, &topLeft, &advance);
        Position pos = 
            layout.GetProperPos(LayoutManager::GT_CHAR, topLeft.x, topLeft.y);
        render.RenderBitMap(pos.x, pos.y, bitmap->width, bitmap->rows, bitmap->buffer);
        ch++;
    }
    render.Flush();

    return true;
}

