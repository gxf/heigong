#ifndef PAGE_H
#define PAGE_H

#include "Common.h"
#include "DocParser.h"
#include <stdio.h>

class PageCache;

// Page class
class Page{
    public:
        Page(int n = 0, HDocState hState = (HDocState)0):
            num(n), hPState(hState),
            pFb(NULL), fbWidth(0), fbHeight(0),
            valid(false)
        {}

        ~Page(){}

        inline HDocState GetParserState(){ return hPState; }
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
        HDocState   hPState;    // Parser state at page start

    private:
        void*       pFb;        // The content of framebuffer
        uint32      fbWidth;    // The width of framebuffer
        uint32      fbHeight;   // The height of framebuffer
        bool        valid;      // flag to show if pFb is valid

};

#endif
