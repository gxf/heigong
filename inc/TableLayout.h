#ifndef TABLE_LAYOUT_H
#define TABLE_LAYOUT_H

#include "LayoutManager.h"

class TableLayout : public LayoutManager{
    public:
        TableLayout(int w, int h, int m_v, int m_h, 
                    Logger* log, int ls = 2, int ws = 2);
        ~TableLayout();

    public:
        void AddGlyph(Glyph* g){}
        LAYOUT_RET GetCharPos(Position & pos, int width, int height, int bearingY);
        LAYOUT_RET GetGraphPos(Position & pos, int width, int height){
            return LO_OK;
        } // Not supported yet
        LAYOUT_RET NewLine();
        void NewPage(){}
        void Reset();

};

#endif
