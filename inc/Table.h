#ifndef HG_TABLE_H
#define HG_TABLE_H

#include "Glyph.h"
#include <vector>
#include <queue>
#include <deque>

typedef class Table_Data_Cell: public Glyph{
    public:
        Table_Data_Cell(Logger* log, uint32 w);

    public:
        bool Draw(RenderMan*);
        bool Relocate(int, int);
        bool Setup(Context* ctx);
        Glyph* Dup();

    public:
        inline uint32 GetHeight(){ return height; }

    public:
        uint32 width;
        uint32 height;

    private:
        std::queue<Glyph*>  delayedToken;
        std::deque<Glyph*>  glyphBuffer;

    private:
        Attrib_Glyph        glyphAttrib;
        Attrib_Line         lineAttrib;

}Table_DC;

typedef class Table_Row: public Glyph{
    public:
        Table_Row(Logger* log, uint32 w);

    public:
        bool Draw(RenderMan*);
        bool Relocate(int, int);
        bool Setup(Context* ctx);
        Glyph* Dup();

    public:
        inline uint32 GetHeight(){ return height; }
        inline uint32 GetWidth(){ return width; }
        inline void AddTD(Table_DC * td){ dataCells.push_back(td); }

    public:
        uint32 width;
        uint32 height;

    public:
        std::vector<Table_DC*> dataCells;

}Table_R;

class Table: public Glyph{
    public:
        Table(Logger* log);

    public:
        bool Draw(RenderMan*);
        bool Relocate(int, int);
        bool Setup(Context* ctx);
        Glyph* Dup();

    public:
        inline uint32 GetWidth(){ return width; }
        inline void SetWidth(uint32 w){ width = w; }
        inline void SetCol(uint32 c){ col = c; }
        inline void SetRow(uint32 r){ row = w; }
        inline void SetBorder(uint32 b){ border = b; }
        inline void AddTR(Table_R * tr){ rows.push_back(tr); }

    public:
        uint32 width;
        uint32 col;
        uint32 row;
        uint32 border;

    public:
        uint32 height;

    public:
        std::vector<Table_R*> rows;
};

#endif

