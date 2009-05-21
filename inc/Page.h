#ifndef PAGE_H
#define PAGE_H

#include "Common.h"

// Page Item for caching
class PageItem{
    public:
        PageItem(void* fb = NULL, uint32 w = 0, uint32 h = 0):
        pFb(fb), fbWidth(w), fbHeight(h),
        valid(true)
        {}

        ~PageItem(){}

    public:
        void*   pFb;        // The content of framebuffer
        uint32  fbWidth;    // The width of framebuffer
        uint32  fbHeight;   // The height of framebuffer

    public:
        bool    valid;
//        bool    
};

// Page class
class Page{
    public:
        Page(int n = 0, uint32 o = 0, PageItem* pi = NULL):
            num(n), offset(o), pItem(pi)
        {}

        ~Page(){}

        inline uint32 GetOffset() { return offset; }
        inline int GetNum() { return num; }

    private:
        int         num;    // Page Number
        uint32      offset; // Offset of 1st charator in file
        PageItem *  pItem;
};

#endif
