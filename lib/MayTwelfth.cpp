#include "Common.h"
#include "MayTwelfth.h"
#include "Table.h"
#include <cstdlib>
#include <cstring>

const int May12th::screen_width = SCREEN_WIDTH;
const int May12th::screen_height= SCREEN_HEIGHT;

May12th::May12th(Logger* log, const char* filename):
    encoding(EM_UTF_8), ctx(NULL), logger(log)
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
    ctx->layout.SetLineSpacing(fontSize/4);
}

void May12th::Display(int page_num){
    bool newPage;
    if (page_num > ctx->pgMgr.GetToWorkPageNum()){
        // TODO: forward search
        LOG_ERROR("Forward search is not supported yet.");
        return;
    }
    else if (page_num == ctx->pgMgr.GetToWorkPageNum()){
        char buf[100];
        sprintf(buf, "Render a new page: %d", page_num);
        LOG_EVENT(buf);
        ctx->pgMgr.StartPage();
        newPage = true;
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
        newPage = false;
    }

    ctx->render.Clear();

    Glyph* glyph;
    DocParser::DP_RET_T dp_ret = DocParser::DP_OK;

    while(DocParser::DP_OK == dp_ret){
        dp_ret = ctx->docParse.GetNextGlyph(&glyph, &ctx->layout);

        switch(dp_ret){
            case DocParser::DP_OK:
                if(false == glyph->Setup(ctx->layout)){
                    Table* tab = dynamic_cast<Table *>(glyph);
                    LOG_EVENT_STR2("tab = ", (int)tab);
                    if (tab){
                        tab->Draw(ctx->render);
                    }
                    if(true == newPage){
                        ctx->docParse << glyph->UngetSet();
                    }
                    ctx->render.Flush();
                    ctx->pgMgr.EndPage(page_num, &ctx->render);
                    return;
                }
                else{
                    Table* tab = dynamic_cast<Table *>(glyph);
                    if (tab){
                        tab->Draw(ctx->render);
                    }
                }
                break;
            case DocParser::DP_EOF:
                ctx->pgMgr.SetMaxPageNum(page_num);
                ctx->layout.curLine->DrawFlush(&ctx->render);
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

