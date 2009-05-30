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
        Context(Logger* log, int screen_width, int screen_height):
            fonts(log), 
            layout(screen_width, screen_height, 30, 30, log), 
            render(log, screen_width, screen_height), 
            docParse(log), line(log), pgMgr(log)
        {}
        ~Context(){}

};

#endif
