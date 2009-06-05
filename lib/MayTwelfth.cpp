#include "Common.h"
#include "MayTwelfth.h"
#include <cstdlib>
#include <cstring>

const int May12th::screen_width = SCREEN_WIDTH;
const int May12th::screen_height= SCREEN_HEIGHT;

May12th::May12th(Logger* log, const char* filename):
    encoding(EM_UTF_8), 
    ctx(NULL), logger(log)
{
    ctx = new Context(log, screen_width, screen_height);
    ctx->render.Init();
    if (false == ctx->docParse.Init(filename)){
        exit(0);
    }
    Init(DEFAULT_FONT_SIZE);
}

May12th::~May12th(){
    ctx->render.Quit();
    delete ctx;
}

void May12th::Init(uint32 fontSize){
    ctx->layout.NewPage();
    ctx->line.Clear();
    ctx->layout.SetLineSpacing(fontSize/4);
    ctx->fonts.SetFontSize(fontSize);
}

void May12th::Display(int page_num){
    if (page_num > ctx->pgMgr.GetToWorkPageNum()){
        // TODO: forward search
        LOG_ERROR("Forward search is not supported yet.");
        return;
    }
    else if (page_num == ctx->pgMgr.GetToWorkPageNum()){
        char buf[100];
        sprintf(buf,"Render a new page: %d", page_num);
        LOG_EVENT(buf);
        ctx->pgMgr.StartPage();
    }
    else if (page_num < ctx->pgMgr.GetToWorkPageNum()){
        char buf[100];
        sprintf(buf,"Render the rendered page: %d", page_num);
        LOG_EVENT(buf);

#ifdef PAGE_CACHED_RENDER
        ctx->render.Clear();
        if (true == ctx->pgMgr.CachedRender(page_num, &(ctx->render))){
            ctx->render.Flush();
            return;
        }
        else{
            ctx->pgMgr.RestorePage(page_num);
        }
#else
//        ctx->layout.NewPage();
        ctx->layout.Reset();
        ctx->pgMgr.RestorePage(page_num);
#endif
    }

    ctx->render.Clear();

    Glyph* glyph;
    DocParser::DP_RET_T dp_ret = DocParser::DP_OK;

    while(DocParser::DP_OK == dp_ret){
        dp_ret = ctx->docParse.GetNextGlyph(&glyph, &ctx->line);

        switch(dp_ret){
            case DocParser::DP_OK:
                if(false == glyph->Setup(ctx)){
                    ctx->render.Flush();
                    ctx->pgMgr.EndPage(page_num, &ctx->render);
                    return;
                }
                break;
            case DocParser::DP_EOF:
                ctx->pgMgr.SetMaxPageNum(page_num);
                ctx->line.DrawFlush(ctx);
                ctx->render.Flush();
                ctx->pgMgr.EndPage(page_num, &ctx->render);
                break;
            case DocParser::DP_INVALID:
                LOG_ERROR("DocParser return invalid stream!");
                break;
            case DocParser::DP_ERROR:
                LOG_ERROR("DocParser return parse error!");
                break;
            default:
                LOG_ERROR("Unsupported DocParser return type!");
                break;
        }
    }
}

void May12th::MainLoop(){
    int done = false;
    SDL_Event event;

    while (!done){ 
        while (SDL_PollEvent(&event))
        {
            switch(event.type){
                case SDL_ACTIVEEVENT: 
                    Display(ctx->pgMgr.RepeatPage());
                    break;              
                case SDL_VIDEORESIZE:
                    break;
                case SDL_KEYDOWN: 
                    // handle key presses
                    switch (event.key.keysym.sym)
                    {
                        case SDLK_UP:
                            Display(ctx->pgMgr.PrevPage());
                            break;
                        case SDLK_DOWN:
                            Display(ctx->pgMgr.NextPage());
                            break;
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

#if 0
void May12th::PerCharDisplay(int page_num){
    if (page_num > pgMgr.GetToWorkPageNum()){
        // TODO: forward search
        LOG_ERROR("Forward search is not supported yet.");
        return;
    }
    else if (page_num == pgMgr.GetToWorkPageNum()){
        char buf[100];
        sprintf(buf,"Render a new page: %d", page_num);
        LOG_EVENT(buf);
        pgMgr.StartPage(docParse.GetStreamPos(), docParse.GetCurOffset());
    }
    else if (page_num < pgMgr.GetToWorkPageNum()){
        char buf[100];
        sprintf(buf,"Render the rendered page: %d", page_num);
        LOG_EVENT(buf);

#ifdef PAGE_CACHED_RENDER
        render.Clear();
        if (true == pgMgr.CachedRender(page_num, &render)){
            render.Flush();
            return;
        }
        else{
            docParse.SetOffset(pgMgr.GetPageOffset(page_num));
//            docParse.SetStreamPos(pgMgr.GetPagePos(page_num));
        }
#else
        docParse.SetOffset(pgMgr.GetPageOffset(page_num));
//        docParse.SetStreamPos(pgMgr.GetPagePos(page_num));
#endif
    }

    Char* cur = new Char(logger);
    cur -> SetID(Char::ID(DEFAULT_FONT, DEFAULT_FONT_SIZE));

    render.Clear();

    do{
        if(!(docParse >> *cur)){
            maxPageNum = page_num;
            line.DrawCurrent(&render, &ftCache, layout.GetLastBaseLine());
            line.Clear();
            render.Flush();
            pgMgr.EndPage(page_num, &render);
            docParse << *cur;
            docParse.ReOpenFile();
            break;
        }

        if(false == RenderChar(*cur)){
            render.Flush();
            pgMgr.EndPage(page_num, &render);
            docParse << *cur;
            return;
        }
        cur = new Char(logger);
        cur -> SetID(Char::ID(DEFAULT_FONT, DEFAULT_FONT_SIZE));
    }
    while(true);

//    docParse.ReOpenFile();
}
#endif


#if 0
bool May12th::RenderChar(Char& ch){
    if ('\n' == ch.GetVal()){
        switch(layout.NewLine()){
            case LO_OK:
                line.DrawCurrent(&render, &ftCache, layout.GetLastBaseLine());
                line.Clear();
                return true;
            case LO_NEW_PAGE:
                line.DrawCurrent(&render, &ftCache, layout.GetLastBaseLine());
                line.Clear();
                layout.Reset();
                return false;
            default:
                LOG_ERROR("Unsupported Layout Newline return.");
                return false;
        }
    }
    Position     pos(0, 0);
    FT_GlyphSlot glyphSlot;

    fonts.GetGlyphSlot((FT_ULong)ch.GetVal(EM_UTF_32), &glyphSlot);
    int baseline = (glyphSlot->metrics.horiBearingY) >> 6;
    ch.SetBaseline(baseline);
    ftCache.CacheFont(&ch, glyphSlot->bitmap.pitch, glyphSlot->bitmap.rows, 
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
            line.DrawCurrent(&render, &ftCache, layout.GetLastBaseLine());
            line.Clear();
            line.AddGlyph(&ch);
            break;
        case LO_NEW_PAGE:
            line.DrawCurrent(&render, &ftCache, layout.GetLastBaseLine());
            line.Clear();
            layout.Reset();
            return false;
        default:
            LOG_ERROR("Unsupported Layout return.");
            break;
    }

    return true;
}
#endif
