#include "Common.h"
#include "MayTwelfth.h"
#include <cstdlib>
#include <cstring>

const int May12th::screen_width = SCREEN_WIDTH;
const int May12th::screen_height= SCREEN_HEIGHT;

May12th::May12th(Logger* log, const char* filename):
    logger(log), fonts(log), 
    layout(screen_width, screen_height, 30, 30, log), 
    render(log, screen_width, screen_height), 
    docParse(log)
{
    render.Init();
    if (false == docParse.OpenFile(filename)){
        exit(0);
    }
}

May12th::~May12th(){
    render.Quit();
}

void May12th::PerCharDisplay(){
    render.Clear();
    layout.NewPage();
    line.Clear();
    layout.SetLineSpacing(DEFAULT_FONT_SIZE/4);
    fonts.SetFontSize(DEFAULT_FONT_SIZE);

    Char* cur = new Char;
    cur->SetID(Char::ID(DEFAULT_FONT, DEFAULT_FONT_SIZE));
    docParse >> *cur;

    while (EOF != cur->GetVal()){
/*        char buf[100];
        sprintf(buf, "Current char: %c", cur);
        LOG_EVENT(buf);
*/
        if ('\n' == cur->GetVal()){
            layout.NewLine();
        }
        else if(false == RenderChar(*cur)){
            render.Flush();
            docParse.ReOpenFile();
            layout.Reset();
            return;
        }
        cur = new Char;
        cur->SetID(Char::ID(DEFAULT_FONT, DEFAULT_FONT_SIZE));
        if (!(docParse >> *cur))
            break;
    }
    line.Flush(&render, &fontsCache, layout.GetLastBaseLine());
    line.Clear();
    render.Flush();
    docParse.ReOpenFile();
}

bool May12th::RenderChar(Char& ch){
    if (EOF == ch.GetVal())
        return false;

    Position     pos(0, 0);
    FT_GlyphSlot glyphSlot;

    fonts.GetGlyphSlot((FT_ULong)ch.GetVal(), &glyphSlot);
    int baseline = (glyphSlot->metrics.horiBearingY) >> 6;
    ch.SetBaseline(baseline);
    fontsCache.CacheFont(&ch, glyphSlot->bitmap.pitch, glyphSlot->bitmap.rows, 
                         glyphSlot->bitmap.buffer);
    LAYOUT_RET ret = 
        layout.GetCharPos(pos, (glyphSlot->advance.x) >> 6, 
                          glyphSlot->bitmap.rows, baseline);
    pos.x += ((glyphSlot->metrics.horiBearingX) >> 6);
    ch.SetPos(pos);
    switch(ret){
        case LO_OK:
            line.AddGlyph(&ch);
            break;
        case LO_NEW_LINE:
            line.Flush(&render, &fontsCache, layout.GetLastBaseLine());
            line.Clear();
            line.AddGlyph(&ch);
            break;
        case LO_NEW_PAGE:
            line.Flush(&render, &fontsCache, layout.GetLastBaseLine());
//            line.Clear();
//            line.AddGlyph(pch);
            return false;
        default:
            LOG_ERROR("Unsupported Layout return.");
            break;
    }

//  sprintf(buf, "Got pos form LayoutManager @ (%d, %d)", pos.x, pos.y);
//  LOG_EVENT(buf);

//    pch -> Draw(&render);
//    fontsCache.DelChar(pch);
    return true;
}

bool May12th::RenderWord(const char* str, int size){
    const char* ch = str;
    if (NULL == ch || 0 == size)
        return true;
    return true;
}

bool May12th::RenderString(const char* str){
/*    const char* ch = str;
    if (NULL == ch)
        return true;

    while('\0' != *ch){
        char buf[100];
        sprintf(buf, "Current char: %c", *ch);
        LOG_EVENT(buf);

        FT_GlyphSlot glyphSlot;
        fonts.GetGlyphSlot((FT_ULong)*ch, &glyphSlot);

        Position pos = 
            layout.GetProperPos(LayoutManager::GT_CHAR, (glyphSlot->advance.x) >> 6, glyphSlot->bitmap.rows, (glyphSlot->metrics.horiBearingY) >> 6);

//        sprintf(buf, "Got pos form LayoutManager @ (%d, %d)", pos.x, pos.y);
//        LOG_EVENT(buf);

        char p[glyphSlot->bitmap.pitch * glyphSlot->bitmap.rows];
        std::memcpy(p, glyphSlot->bitmap.buffer, glyphSlot->bitmap.pitch * glyphSlot->bitmap.rows);
        fontsCache.AdjustBitmap(glyphSlot->bitmap.pitch, glyphSlot->bitmap.rows, p);

        pos.x += ((glyphSlot->metrics.horiBearingX) >> 6);
        render.RenderGrayMap(pos.x, pos.y, glyphSlot->bitmap.pitch, glyphSlot->bitmap.rows, p);
        ch++;
    }
*/    return true;
}

void May12th::MainLoop(){
    int done = false;
    SDL_Event event;

    while (!done){ 
        while (SDL_PollEvent(&event))
        {
            switch(event.type){
                case SDL_ACTIVEEVENT: 
/*                    render.Clear();
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
                    */
                    PerCharDisplay();
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
