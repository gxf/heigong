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
            // Init first page
//            curPage = new Page(0, 0);
//            pages.push_back(curPage);
        }
        ~PageManager(){
            std::vector<Page*>::iterator itr = pages.begin();
            while(itr != pages.end()){
                ++itr;
            }
        }

    public:
        const static int PAGE_ERROR = 0xffffffff;

    public:
        inline void StartPage(uint32 offset){
            char buf[100];
            sprintf(buf, "Start page %d.", numToRender);
            LOG_EVENT(buf);
            curPage = new Page(numToRender, offset);
            pages.push_back(curPage);
        }

        inline void EndPage(int page_num){
            if (page_num == numToRender){
                numLastRendered = numToRender++;
            char buf[100];
            sprintf(buf, "End page %d.", numLastRendered);
            LOG_EVENT(buf);
            }
            // TODO: Put the page into page cache
        }

        inline uint32 GetPageOffset(const int page_num){
            // Notice: Can only get the page already exists.
            std::vector<Page*>::iterator itr = pages.begin();
            while(itr != pages.end()){
                if (page_num == (*itr) -> GetNum()){
                    return (*itr) -> GetOffset();
                }
                ++itr;
            }
            LOG_ERROR("Fail to look for page.");
            return PAGE_ERROR;
        }

        // Get the page to do the work
        inline int GetToWorkPageNum() { 
/*            char buf[100];
            sprintf(buf, "Cur page num: %d.", numToRender);
            LOG_EVENT(buf);
*/            return numToRender;
        }

        // Get the page have done the work
        inline int GetLastPageNum() { 
/*            char buf[100];
            sprintf(buf, "Last rendered page num: %d.", numLastRendered);
            LOG_EVENT(buf);
*/            return numLastRendered; 
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
