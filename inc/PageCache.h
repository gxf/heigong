#ifndef PAGE_CACHE_H
#define PAGE_CACHE_H

#include <vector>
#include "Common.h"
#include "Page.h"

class PageCache{
    public:
        PageCache(int pr = DEFAULT_RESERVED_PAGE):
            res_page(pr), cur_size(0)
        {}

        ~PageCache(){
            std::vector<Page*>::iterator itr = pages.begin();
            
            while(itr != pages.end()){ 
                if (!((*itr) -> pFb) && (*itr) -> valid){
                    delete [] (Page*)((*itr) -> pFb);
                    (*itr) -> valid = false;
                }
                ++itr;
            }
        }

    public:
        void GetMem(Page* pg){
            // TODO: Cache management
            pg->pFb = new char[pg->fbWidth * pg->fbHeight];
            pg->valid = true;
            pages.push_back(pg);
            cur_size += pg->fbWidth * pg->fbHeight; // in Bytes
            // Eviction
            if (cur_size > max_size){
                EvictPages(pg->num - (res_page / 2));
            }
        }

        void EvictPages(int page_num){
            std::vector<Page*>::iterator itr = pages.begin();
            
            while(itr != pages.end()){ 
                if((*itr) -> num < page_num - (res_page / 2) ||
                   (*itr) -> num > page_num + (res_page / 2))
                {
                    delete [] (Page*)((*itr) -> pFb);
                    (*itr) -> valid = false;
                }
                ++itr;
            }
        }

    private:
        const static uint32 max_size = DEFAULT_MAX_PAGE_SIZE;

    private:
        std::vector<Page*>  pages;

    private:
        int     res_page;       // Num of pages should be reserved
        uint32  cur_size;       // Current size of cached pages
};

#endif
