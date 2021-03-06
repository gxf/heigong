#include "PageManager.h"
#include "DocState.h"
#include "DocStream.h"
#include "RenderMan.h"
#include <cstring>


PageManager::PageManager(Logger* log, DocParser & parse):
    curPageNum(0), maxPageNum(MAX_INT32),
    numToRender(0), numLastRendered(0),
    docParser(parse), logger(log)
{
}

PageManager::~PageManager(){
    // De-allocate these not managed by PageCache
    std::vector<Page*>::iterator itr = pages.begin();
    while(itr != pages.end()){
        ++itr;
    }
}

int PageManager::PrevPage(){
    if (curPageNum - 1 >= 0){
        return --curPageNum;
    }
    else{
        return curPageNum;
    }
}

int PageManager::NextPage(){
    if (curPageNum + 1 <= maxPageNum){
        return ++curPageNum;
    }
    else{
        return curPageNum;
    }
}

int PageManager::RepeatPage(){
    return curPageNum;
}

HDocState PageManager::StartPage(int pg_num ){
//    if (false == fast_page_sum){
    // Always Gen .pg
//    if(true){
        HDocState docState = docParser.ShadowDocState();
        static char dbuf[100];
        if (-1 == pg_num){
            sprintf(dbuf, "%s%d\0", work_dir, numToRender);
        }
        else{
            sprintf(dbuf, "%s%d-%d\0", work_dir, pg_num, numToRender);
        }
        std::string pers_page(dbuf);
        pers_page += ".pg";
        docState->StoreState(pers_page.c_str());
        docParser.PostStoreState(docState);
    
//        delete docState;
#if 0
        curPage = new Page(numToRender, docState);
        pages.push_back(curPage);
#endif
        char buf[100];
        sprintf(buf, "Start page %d. CurPage: %d", numToRender, curPageNum);
        LOG_EVENT(buf);
        return docState;
//    }
}

void PageManager::EndPage(HDocState hdoc, RenderMan* render, int page_num, int pg_num){
    if (page_num == numToRender){
        numLastRendered = numToRender++;
//        if (false == fast_page_sum){
        if (true){
            char buf[100];
            sprintf(buf, "End page %d. CurPage: %d", numLastRendered, (uint32)curPageNum);
            LOG_EVENT(buf);
        }

        docParser.FinalStoreState(hdoc);
        static char dbuf[100];
        if(-1 == pg_num){
            sprintf(dbuf, "%s%d\0", work_dir, numLastRendered);
        }
        else{ 
            sprintf(dbuf, "%s%d-%d\0", work_dir, pg_num, numLastRendered);
        }
        std::string pers_page(dbuf);
        pers_page += ".pg";
        hdoc->AppendState(pers_page.c_str());

#ifdef PAGE_CACHED_RENDER
        // Put the page into page cache
        render->GetFBSize(curPage);
        pageCache.GetMem(curPage);
       
        render->GetFrameBuffer(curPage);
#endif
    }
}

bool PageManager::RestorePage(int page_num, int sub_pg_num){
    HDocState hds = new DocState(logger);
    static char dbuf[100];
    if(-1 == sub_pg_num){
        sprintf(dbuf, "%s%d\0", work_dir, page_num);
    }
    else{ 
        sprintf(dbuf, "%s%d-%d\0", work_dir, page_num, sub_pg_num);
    }
    std::string pers_page(dbuf);
    pers_page +=  ".pg";
    hds ->RecoverState(pers_page.c_str());
    docParser.RestoreDocState(hds);
    docParser.PostStoreState(hds);
    delete hds;
    return true;
#if 0
    // Notice: Can only get the page already exists.
    std::vector<Page*>::iterator itr = pages.begin();
    while(itr != pages.end()){
        if (page_num == (*itr) -> GetNum()){
            break;
        }
        ++itr;
    }
    if (itr != pages.end()){
//        docParser.RestoreDocState((*itr)->GetParserState());
        return true;
    }
    LOG_ERROR("Fail to look for page.");
    return false;
#endif
}

bool PageManager::CachedRender(int page_num, RenderMan* render){
    std::vector<Page*>::iterator itr = pages.begin();
    while(itr != pages.end()){
        if (page_num == (*itr) -> GetNum() && (*itr) -> CachedFB()){
            render -> RenderGrayMap(0, 0, (*itr)->GetFBWidth(), (*itr)->GetFBHeight(), (*itr)->GetFB());
            char buf[100];
            sprintf(buf, "Cached Rendering! Page: %d, fb ptr: %d", page_num, (int)(*itr)->GetFB());
            LOG_EVENT(buf);
            return true;
        }
        ++itr;
    }
    return false;
}

