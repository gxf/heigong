#ifndef PAGE_MANAGER_H
#define PAGE_MANAGER_H

#include <vector>
#include "Logger.h"
#include "Page.h"
#include "PageCache.h"

class PageManager{
    public:
        PageManager(Logger* log):
            numToRender(0), numLastRendered(0),
            logger(log)
        {
        }
        ~PageManager(){
            // De-allocate these not managed by PageCache
            std::vector<Page*>::iterator itr = pages.begin();
            while(itr != pages.end()){
                ++itr;
            }
        }

    public:
        const static int PAGE_ERROR = 0xffffffff;

    public:
        inline void StartPage(fpos_t* pos, long int offset){
            curPage = new Page(numToRender, pos, offset);
            pages.push_back(curPage);
            char buf[100];
            sprintf(buf, "Start page %d. Offset: %ld. CurPage: %d", numToRender, offset, (uint32)curPage);
            LOG_EVENT(buf);
        }

        inline void EndPage(int page_num, RenderMan* render){
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

        inline uint32 GetPageOffset(const int page_num){
            // Notice: Can only get the page already exists.
            std::vector<Page*>::iterator itr = pages.begin();
            while(itr != pages.end()){
                if (page_num == (*itr) -> GetNum()){
                    char buf[100];
                    sprintf(buf, "Retrieved offset: %ld.", (*itr) -> GetOffset());
                    LOG_EVENT(buf);
                    return (*itr) -> GetOffset();
                }
                ++itr;
            }
            LOG_ERROR("Fail to look for page.");
            return PAGE_ERROR;
        }

        inline fpos_t* GetPagePos(const int page_num){
            // Notice: Can only get the page already exists.
            std::vector<Page*>::iterator itr = pages.begin();
            while(itr != pages.end()){
                if (page_num == (*itr) -> GetNum()){
/*                    char buf[100];
                    sprintf(buf, "Retrieved offset: %ld.", (*itr) -> GetOffset());
                    LOG_EVENT(buf);
*/                    
                    return (*itr) -> GetStreamPos();
                }
                ++itr;
            }
            LOG_ERROR("Fail to look for page.");
            return NULL;
        }

        // Get the page to do the work
        inline int GetToWorkPageNum(){
            return numToRender;
        }

        // Get the page have done the work
        inline int GetLastPageNum(){
            return numLastRendered; 
        }

        inline bool CachedRender(int page_num, RenderMan* render){
            std::vector<Page*>::iterator itr = pages.begin();
            while(itr != pages.end()){
                if (page_num == (*itr) -> GetNum() && 
                    (*itr) -> CachedFB()){
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

    private:
        int     numToRender;
        int     numLastRendered;
        Page*   curPage;

    private:
        std::vector<Page*> pages;
        PageCache          pageCache;

    private:
        Logger *           logger;
};

#endif
