#ifndef CONTEXT_H
#define CONTEXT_H

#include "Common.h"
#include "Logger.h"
#include "RenderMan.h"
#include "FontsManager.h"
#include "FontsCache.h"
#include "LayoutManager.h"
#include "DocParser.h"
#include "Glyph.h"
#include "Line.h"
#include "PageManager.h"

class Context{
    public:
        FontsManager    fonts;
        LayoutManager   layout;
        RenderMan       render;
        FontsCache      ftCache;
        DocParser       docParse;
        Line            line;
        PageManager     pgMgr;

    public:
        Context(Logger* log, int screen_width, int screen_height, uint32 margin_v = MARGIN_VERTICAL, uint32 margin_h = MARGIN_HORIZONTAL):
            fonts(log), 
            layout(screen_width, screen_height, margin_v, margin_h, log), 
            render(log, screen_width, screen_height), 
            docParse(log), line(log, screen_width, margin_v), pgMgr(log, docParse)
        {}
        ~Context(){}

};

#endif
