#ifndef TABLE_LAYOUT_H
#define TABLE_LAYOUT_H

#include "Line.h"
#include "LayoutManager.h"

class Line;

class TableLayout : public LayoutManager{
    public:
        TableLayout(int w, int h, int m_v, int m_h, 
                    Logger* log, int ls = TAB_MARGIN, int ws = TAB_MARGIN);
        ~TableLayout();

    public:
        // Interfaces
        void AddGlyph(Glyph* g){ curLine->AddGlyph(g); }
        LAYOUT_RET GetCharPos(Position & pos, int width, int height, int bearingY);
        LAYOUT_RET GetGraphPos(Position & pos, int width, int height){
            // Not supported yet
            return LO_OK;
        }
        LAYOUT_RET NewLine();
        void NewPage(){}
        void Reset();

    public:
        int GetMaxHeight();

        void ReSetup(int w, int h, int mv, int mh, 
                     Logger* log, int ls = TAB_MARGIN, int ws = TAB_MARGIN);

        TableLayout & operator=(TableLayout & tlo);

    public:
        Line* curLine;

};

#endif
