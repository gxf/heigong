#include "Common.h"
#include "MayTwelfth.h"
#include "Table.h"
#include <cstdlib>
#include <cstring>
#include <cassert>

static int pb_cur_page = 0;

May12th::May12th(Logger* log, const char* fn, bool conv):
    inited(false), convert(conv), bgMode(false), slMode(false),
    filename(fn), encoding(EM_UTF_8), ctx(NULL), logger(log)
{
    ctx = new Context(log, scr_width, scr_height);
}

May12th::~May12th(){
    ctx->render.Quit();
    delete ctx;
}

void May12th::Init(uint32 fontSize){
    if (false == inited){
        ctx->render.Init();
        // If it is serialized mode, choose the tmp file as input
        if (true == slMode && (true == convert)){
            if (false == ctx->docParse.Init((std::string(work_dir) + std::string(DEFAULT_TMP_FILE_NAME)).c_str(), false, bgMode)){
                exit(0);
            }
        }
        else{
            if (false == ctx->docParse.Init(filename, convert, bgMode)){
                exit(0);
            }
        }
        ctx->layout.NewPage();
        ctx->layout.SetLineSpacing(fontSize/4);
        inited = true;
    }
}

void May12th::RenderAll(){
    Init(DEFAULT_FONT_SIZE);
    int i = 0;
    for (i = 0; i < ctx->pgMgr.GetMaxPageNum(); i++){
        Display(i);
    }
}

bool May12th::StartForeGroundSerialized(){
    bgMode = false;
    slMode = true;
    Init(DEFAULT_FONT_SIZE);
    return true;
}

bool May12th::StartForeGroundSerializedNoConv(){
    bgMode = false;
    slMode = true;
    convert= false;
    Init(DEFAULT_FONT_SIZE);
    return true;
}

bool May12th::StartForeGround(){
    bgMode = false;
    slMode = false;
    convert= false;
    Init(DEFAULT_FONT_SIZE);
    return true;
}

bool May12th::StartBackGround(){
    bgMode = true;
    slMode = false;
    Init(DEFAULT_FONT_SIZE);
    return true;
}

bool May12th::PB_GetPage(uint32 page_num, uint32 * width, uint32 * height, 
                      uint32 * depth, void** img)
{
    if((int)page_num > ctx->pgMgr.GetMaxPageNum()){
        *width  = 0;
        *height = 0;
        *depth  = 0;
        *img    = NULL;
        return false;
    }

    if (NULL == (*img = PB_Display(page_num))){
        return false;
    }
    bool ret = ctx->bufMgr.GetAttr(*img, width, height, depth);
    assert( ret == true);
    return ret;
}

bool May12th::GetPage(uint32 page_num, uint32 * width, uint32 * height, 
                      uint32 * depth, void** img){
    if (true == slMode){
        if (NULL == (*img = SerializedDisplay(page_num))){
            return false;
        }
        bool ret = ctx->bufMgr.GetAttr(*img, width, height, depth);
        assert( ret == true);
        return ret;
    }
    if((int)page_num > ctx->pgMgr.GetMaxPageNum()){
        *width  = 0;
        *height = 0;
        *depth  = 0;
        *img    = NULL;
        return false;
    }

    *img = Display(page_num);
    if ((int)page_num > ctx->pgMgr.GetMaxPageNum()){
        *width  = 0;
        *height = 0;
        *depth  = 0;
        *img    = NULL;
        return false;
    }
    else if ((int)page_num == ctx->pgMgr.GetMaxPageNum()){
        bool ret = ctx->bufMgr.GetAttr(*img, width, height, depth);
        assert( ret == true);
        return false;
    }
    else{
        bool ret = ctx->bufMgr.GetAttr(*img, width, height, depth);
        assert( ret == true);
        return true;
    }
}

bool May12th::FreePage(void* img){
    if (!ctx->bufMgr.Delete(img)){
        return false;
    }
    return true;
}

uint32 May12th::GetCurMaxPage(){
    return ctx->pgMgr.GetLastPageNum();
}

uint32 May12th::GetMaxPage(){
    RenderAll();
    return ctx->pgMgr.GetMaxPageNum();
}

bool May12th::Term(){
    return true;
}

void* May12th::Display(int page_num){
    bool newPage = false;
    HDocState docState = NULL;
    if (page_num > ctx->pgMgr.GetToWorkPageNum()){
        // Forward Display
        int i = ctx->pgMgr.GetToWorkPageNum(); 
        Display(i);
        if(i++ >= ctx->pgMgr.GetMaxPageNum())
            return NULL;
        while(i < page_num){
            Display(ctx->pgMgr.NextPage());
            if(i++ >= ctx->pgMgr.GetMaxPageNum())
                return NULL;
        }
        return Display(i);
    }
    else if (page_num == ctx->pgMgr.GetToWorkPageNum()){
        char buf[100];
        sprintf(buf, "Render a new page: %d", page_num);
        LOG_EVENT(buf);
        docState = ctx->pgMgr.StartPage();
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

    bool finished = false;
    void* img = NULL;
    while(!finished){
        dp_ret = ctx->docParse.GetNextGlyph(&glyph, &ctx->layout);
        Glyph::GY_ST_RET gy_ret = Glyph::GY_OK; 
        Table* tab = NULL;
        switch(dp_ret){
            case DocParser::DP_OK:
                gy_ret = glyph->Setup(ctx->layout);
                tab = dynamic_cast<Table *>(glyph);
                switch(gy_ret){
                    case Glyph::GY_OK:
                        // Table render is trigered here
                        if (tab){ tab->Draw(ctx->render); }
                        break;
                    case Glyph::GY_NEW_PAGE:
                        // Table render is trigered here
                        if (tab){ tab->Draw(ctx->render); }
                        if(true == newPage){
                            ctx->docParse << glyph->UngetSet();
                        }
                        if (bgMode || (!bgMode && !convert))
                            img = ctx->render.Flush(&ctx->bufMgr);
                        else
                            img = ctx->render.Flush(NULL);
                        ctx->pgMgr.EndPage(docState, &ctx->render, page_num);
                        Char::ClearCache();
                        finished = true;
                        break;
                    case Glyph::GY_EOF:
                        ctx->pgMgr.SetMaxPageNum(page_num);
                        ctx->layout.curLine->DrawFlush(&ctx->render);
                        if (bgMode || (!bgMode && !convert))
                            img = ctx->render.Flush(&ctx->bufMgr);
                        else
                            img = ctx->render.Flush(NULL);
                        ctx->pgMgr.EndPage(docState, &ctx->render, page_num);
                        Char::ClearCache();
                        finished = true;
                        break;
                    case Glyph::GY_ERROR:
                        LOG_ERROR("Internal error.");
//                        finished = true;
                        break;
                    default:
                        LOG_ERROR("Unsupported setting up return value of glyph.");
                        finished = true;
                        break;
                }
            case DocParser::DP_EOF:
//                finished = true;
                break;
            case DocParser::DP_INVALID:
                LOG_ERROR("DocParser return invalid stream!");
//                finished = true;
                break;
            case DocParser::DP_ERROR:
                LOG_ERROR("DocParser return parse error!");
//                finished = true;
                break;
            default:
                LOG_ERROR("Unsupported DocParser return type!");
//                finished = true;
                break;
        }
    }
    return img;
}

void* May12th::SerializedDisplay(int page_num){
    ctx->layout.Reset();
    ctx->pgMgr.RestorePage(page_num);

    ctx->render.Clear();

    Glyph* glyph;
    DocParser::DP_RET_T dp_ret = DocParser::DP_OK;

    bool finished = false;
    void* img = NULL;
    while(!finished){
        dp_ret = ctx->docParse.GetNextGlyph(&glyph, &ctx->layout);
        Glyph::GY_ST_RET gy_ret = Glyph::GY_OK; 
        Table* tab = NULL;
        switch(dp_ret){
            case DocParser::DP_OK:
                gy_ret = glyph->Setup(ctx->layout);
                tab = dynamic_cast<Table *>(glyph);
                switch(gy_ret){
                    case Glyph::GY_OK:
                        // Table render is trigered here
                        if (tab){ tab->Draw(ctx->render); }
                        break;
                    case Glyph::GY_NEW_PAGE:
                        // Table render is trigered here
                        if (tab){ tab->Draw(ctx->render); }
                        img = ctx->render.Flush(&ctx->bufMgr);
    //                    ctx->pgMgr.EndPage(page_num, &ctx->render);
                        Char::ClearCache();
                        finished = true;
                        break;
                    case Glyph::GY_EOF:
     //                   ctx->pgMgr.SetMaxPageNum(page_num);
                        ctx->layout.curLine->DrawFlush(&ctx->render);
                        img = ctx->render.Flush(&ctx->bufMgr);
//                        ctx->pgMgr.EndPage(page_num, &ctx->render);
                        Char::ClearCache();
                        finished = true;
                        break;
                    case Glyph::GY_ERROR:
                        LOG_ERROR("Internal error.");
//                        finished = true;
                        break;
                    default:
                        LOG_ERROR("Unsupported setting up return value of glyph.");
                        finished = true;
                        break;
                }
            case DocParser::DP_EOF:
//                finished = true;
                break;
            case DocParser::DP_INVALID:
                LOG_ERROR("DocParser return invalid stream!");
//                finished = true;
                break;
            case DocParser::DP_ERROR:
                LOG_ERROR("DocParser return parse error!");
//                finished = true;
                break;
            default:
                LOG_ERROR("Unsupported DocParser return type!");
//                finished = true;
                break;
        }
    }
    return img;
}
// Note: .pg based render only
void May12th::PB_Set2Page(int page_num){
    ctx->layout.Reset();
    ctx->pgMgr.RestorePage(page_num);

    ctx->render.Clear();
    pb_cur_page = page_num;
}

// Note: .pg based render only
void* May12th::PB_Display(int sub_pg_num){
    bool newPage = false;
    HDocState docState = NULL;

    if (sub_pg_num > ctx->pgMgr.GetToWorkPageNum()){
        // Forward Display
        int i = ctx->pgMgr.GetToWorkPageNum(); 
        PB_Display(i);
        if(i++ >= ctx->pgMgr.GetMaxPageNum())
            return NULL;
        while(i < sub_pg_num){
            PB_Display(ctx->pgMgr.NextPage());
            if(i++ >= ctx->pgMgr.GetMaxPageNum())
                return NULL;
        }
        return PB_Display(i);
    }
    else if (sub_pg_num == ctx->pgMgr.GetToWorkPageNum()){
        char buf[100];
        sprintf(buf, "Render a new page: %d-%d", pb_cur_page, sub_pg_num);
        LOG_EVENT(buf);
        docState = ctx->pgMgr.StartPage(pb_cur_page);
        newPage = true;
    }
    else if (sub_pg_num < ctx->pgMgr.GetToWorkPageNum()){
        char buf[100];
        sprintf(buf,"Render the rendered page: %d-%d", pb_cur_page, sub_pg_num);
        LOG_EVENT(buf);
        ctx->layout.Reset();
        ctx->pgMgr.RestorePage(pb_cur_page, sub_pg_num);
        newPage = false;
    }

    ctx->render.Clear();

    Glyph* glyph;
    DocParser::DP_RET_T dp_ret = DocParser::DP_OK;

    bool finished = false;
    void* img = NULL;
    while(!finished){
        dp_ret = ctx->docParse.GetNextGlyph(&glyph, &ctx->layout);
        Glyph::GY_ST_RET gy_ret = Glyph::GY_OK; 
        Table* tab = NULL;
        switch(dp_ret){
            case DocParser::DP_OK:
                gy_ret = glyph->Setup(ctx->layout);
                tab = dynamic_cast<Table *>(glyph);
                switch(gy_ret){
                    case Glyph::GY_OK:
                        // Table render is trigered here
                        if (tab){ tab->Draw(ctx->render); }
                        break;
                    case Glyph::GY_NEW_PAGE:
                        // Table render is trigered here
                        if (tab){ tab->Draw(ctx->render); }
                        if(true == newPage){
                            ctx->docParse << glyph->UngetSet();
                        }
                        if (bgMode || (!bgMode && !convert))
                            img = ctx->render.Flush(&ctx->bufMgr);
                        else
                            img = ctx->render.Flush(NULL);
                        ctx->pgMgr.EndPage(docState, &ctx->render, sub_pg_num, pb_cur_page);
                        Char::ClearCache();
                        finished = true;
                        break;
                    case Glyph::GY_EOF:
                        ctx->pgMgr.SetMaxPageNum(sub_pg_num);
                        ctx->layout.curLine->DrawFlush(&ctx->render);
                        if (bgMode || (!bgMode && !convert))
                            img = ctx->render.Flush(&ctx->bufMgr);
                        else
                            img = ctx->render.Flush(NULL);
                        ctx->pgMgr.EndPage(docState, &ctx->render, sub_pg_num, pb_cur_page);
                        Char::ClearCache();
                        finished = true;
                        break;
                    case Glyph::GY_ERROR:
                        LOG_ERROR("Internal error.");
//                        finished = true;
                        break;
                    default:
                        LOG_ERROR("Unsupported setting up return value of glyph.");
                        finished = true;
                        break;
                }
            case DocParser::DP_EOF:
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
    return img;
}

void May12th::MainLoop(){
#ifndef RENDER2FILE
    Init();
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
#endif
}

