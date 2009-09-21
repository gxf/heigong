#ifndef PAGE_MANAGER_H
#define PAGE_MANAGER_H

#include <vector>
#include "Logger.h"
#include "Page.h"
#include "PageCache.h"
#include "DocState.h"

class DocParser;
class RenderMan;

class PageManager{
    public:
        PageManager(Logger* log, DocParser & parse);
        ~PageManager();

    public:
        int PrevPage();
        int NextPage();
        int RepeatPage();
        
        HDocState StartPage(int pg_num = -1);
        void EndPage(HDocState dState, RenderMan* render, int page_num, int sub_pg_num = -1);

        bool RestorePage(int page_num, int sub_pg_num = -1);

        bool CachedRender(int page_num, RenderMan* render);

    public:
        inline void SetMaxPageNum(int num){ maxPageNum = num;}
        inline int GetMaxPageNum(){ return maxPageNum; }
        inline int GetToWorkPageNum(){ return numToRender; }
        inline int GetLastPageNum(){ return numLastRendered; }
        inline void Reset(){
            curPageNum = 0;
            maxPageNum = MAX_INT32;
            numToRender = 0;
            numLastRendered = 0;
        }

    private:
        int     curPageNum;
        int     maxPageNum;

        int     numToRender;
        int     numLastRendered;

        Page*   curPage;

    private:
        std::vector<Page*> pages;
        PageCache          pageCache;

    private:
        DocParser &        docParser;
        Logger *           logger;
};

#endif

