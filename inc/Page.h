#ifndef PAGE_H
#define PAGE_H

#include "Common.h"


class PageCache;

// Page class
class Page{
    public:
        Page(int n = 0, long int o = 0):
            num(n), offset(o),
            pFb(NULL), fbWidth(0), fbHeight(0),
            valid(false)
        {}

        ~Page(){}

        inline long int GetOffset() { return offset; }
        inline int GetNum() { return num; }
        inline int CachedFB(){ return valid; }
        inline void SetSize(int w, int h){
            fbWidth   = w;
            fbHeight  = h;
        }
        inline void* GetFB(){ return pFb; }
        inline uint32 GetFBWidth(){ return fbWidth; }
        inline uint32 GetFBHeight(){ return fbHeight; }

    public:
        friend class PageCache;
        
    private:
        int         num;        // Page Number
        long int    offset;     // Offset of 1st charator in file

    private:
        void*       pFb;        // The content of framebuffer
        uint32      fbWidth;    // The width of framebuffer
        uint32      fbHeight;   // The height of framebuffer
        bool        valid;      // flag to show if pFb is valid

};

#endif
