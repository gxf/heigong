#ifndef PAGE_LAYOUT_H
#define PAGE_LAYOUT_H

#include "LayoutManager.h"
#include "Line.h"

class PageLayout : public LayoutManager{
    public:
        PageLayout(int w, int h, int m_v, int m_h, 
                      Logger* log, RenderMan* render,
                      int ls = 2, int ws = 2);
        ~PageLayout();

    public:
        LAYOUT_RET NewLine();
        void NewPage();

    public:
        void AddGlyph(Glyph* g){ curLine->AddGlyph(g); }

    public:
        LAYOUT_RET GetCharPos(Position & pos, int width, int height, int bearingY);
        LAYOUT_RET GetGraphPos(Position & pos, int width, int height);
        LAYOUT_RET GetTablePos(Position & pos, int height);
            
        void Reset();

    public:
        int      imageConp;

    public:
        RenderMan*  render;
        Line *      curLine;
};

#endif
