#ifndef PAGE_CACHE_H
#define PAGE_CACHE_H

#include <vector>
#include "Page.h"

class PageCache{
    public:
        PageCache(){}
        ~PageCache(){}

    public:
        void* GetCacheMem(uint32 size){
            // TODO: Cache management
            return (void*) new char[size];
        }


    private:
        std::vector<PageItem*>  pPage;
};

#endif
