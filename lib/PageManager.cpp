#include "PageManager.h"
#include "DocStream.h"
#include "RenderMan.h"


PageManager::PageManager(Logger* log, DocParser & parse):
    curPageNum(0), maxPageNum(0xffffff),
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
    return numLastRendered;
}

void PageManager::StartPage(){
    DocParser::HDocState docState = docParser.ShadowDocState();
    curPage = new Page(numToRender, docState);
    pages.push_back(curPage);
    char buf[100];
    sprintf(buf, "Start page %d. CurPage: %d", numToRender, (uint32)curPage);
    LOG_EVENT(buf);
}

void PageManager::EndPage(int page_num, RenderMan* render){
    if (page_num == numToRender){
        numLastRendered = numToRender++;
        char buf[100];
        sprintf(buf, "End page %d. CurPage: %d", numLastRendered, (uint32)curPage);
        LOG_EVENT(buf);

#ifdef PAGE_CACHED_RENDER
        // Put the page into page cache
        render->GetFBSize(curPage);
        pageCache.GetMem(curPage);
       
        render->GetFrameBuffer(curPage);
#endif
    }
}

bool PageManager::RestorePage(int page_num){
    // Notice: Can only get the page already exists.
    std::vector<Page*>::iterator itr = pages.begin();
    while(itr != pages.end()){
        if (page_num == (*itr) -> GetNum()){
            break;
        }
        ++itr;
    }
    if (itr != pages.end()){
        docParser.RestoreDocState((*itr)->GetParserState());
        return true;
    }
    LOG_ERROR("Fail to look for page.");
    return false;
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

