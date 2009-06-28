#ifndef PAGE_MANAGER_H
#define PAGE_MANAGER_H

#include <vector>
#include "Logger.h"
#include "Page.h"
#include "PageCache.h"

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
        
        void StartPage();
        void EndPage(int page_num, RenderMan* render);

        bool RestorePage(int page_num);

        bool CachedRender(int page_num, RenderMan* render);

    public:
        inline void SetMaxPageNum(int num){ maxPageNum = num;}
        inline int GetMaxPageNum(){ return maxPageNum; }
        inline int GetToWorkPageNum(){ return numToRender; }
        inline int GetLastPageNum(){ return numLastRendered; }


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

