#ifndef HG_TABLE_H
#define HG_TABLE_H

#include "Logger.h"
#include "Glyph.h"
#include "TableLayout.h"
#include <vector>
#include <queue>
#include <deque>

typedef class Table_Data_Cell: public Glyph{
    public:
        Table_Data_Cell(Logger* log, uint32 w, uint32 o);
        ~Table_Data_Cell();

    public:
        bool Draw(RenderMan&);
        bool DrawBorder(RenderMan&);
        bool Relocate(int, int);
        bool Adjust2Baseline(int baseline){ return true; };
        bool Setup(LayoutManager& lo);
        Glyph* Dup();
        Glyph* UngetSet();

    public:
        inline void AddChar(Char* ch){ glyphBuffer.push_back(ch); }
        inline void AddDelayedChar(Char* label){ delayedToken.push(label); }
        inline void PushDelayedLabel(){ 
            while(!delayedToken.empty()){
                glyphBuffer.push_back(delayedToken.front());
                delayedToken.pop();
            }
        }

    public:
        inline uint32 GetHeight(){ 
            return cellLayout.GetMaxHeight(); 
        }
        inline void SetBorderHeight(uint32 bh){
            borderHeight = bh;
        }

    public:
        uint32 width;
        uint32 xoff;
        Position borderPos;
        uint32 borderSize;
        uint32 borderHeight;

    public:
        Attrib_Glyph        glyphAttrib;
        Attrib_Line         lineAttrib;

    public:
        TableLayout cellLayout;

    public:
        std::queue<Glyph*>  delayedToken;
        std::deque<Glyph*>  glyphBuffer;

}Table_DC;

typedef class Table_Row: public Glyph{
    public:
        Table_Row(Logger* log, uint32 w, uint32 o);
        ~Table_Row();

    public:
        bool Draw(RenderMan&);
        bool DrawBorder(RenderMan&);
        bool Relocate(int, int);
        bool Adjust2Baseline(int baseline){ return true; };
        bool Setup(LayoutManager& lo);
        Glyph* Dup();
        Glyph* UngetSet();

    public:
        inline uint32 GetHeight(){ return height; }
        inline uint32 GetWidth(){ return width; }
        inline void AddTD(Table_DC * td){ dataCells.push_back(td); }

    public:
        uint32 xoff;

    public:
        uint32 width;
        uint32 height;
        Position borderPos;
        uint32 borderSize;

    public:
        std::vector<Table_DC*> dataCells;

}Table_R;

class Table: public Glyph{
    public:
        Table(Logger* log);
        ~Table();

    public:
        bool Draw(RenderMan&);
        bool Relocate(int, int);
        bool Adjust2Baseline(int baseline){ return true; };
        bool Setup(LayoutManager& lo);
        Glyph* Dup();
        Glyph* UngetSet();

    public:
        inline void SetOffset(uint32 o){ xoff = o; }
        inline uint32 GetOffset(){ return xoff; }
        inline uint32 GetWidth(){ return width; }
        inline void SetWidth(uint32 w){ width = w; }
        inline void SetCol(uint32 c){ col = c; }
        inline void SetRow(uint32 r){ row = r; }
        inline void SetBorder(uint32 b){ border = b; }
        inline void AddTR(Table_R * tr){ rows.push_back(tr); }

    public:
        uint32 xoff;
        uint32 width;
        uint32 col;
        uint32 row;
        uint32 border;

    public:
        uint32 height;
        uint32 rowSplit;

    public:
        std::vector<Table_R*> rows;
};

#endif

