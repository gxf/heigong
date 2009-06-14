#ifndef HG_LINE_H
#define HG_LINE_H

#include "Common.h"
#include "Glyph.h"
#include "FontsCache.h"
#include <vector>

class RenderMan;
class Logger;
class LayoutManager;

// Note: Line is just a abstract concept which does not
// get any Glypth Object memory management involved
class Line{
    public:
        Line(Logger* log, LayoutManager* lo, uint32 pw, uint32 m, ALIGNMENT a = A_LEFT): 
            curWidth(0),
            pageWidth(pw), margin(m),
            layout(lo), logger(log)
        {}
        ~Line(){}

    public:
        Line* Dup(){
            return new Line(logger, layout, pageWidth, margin, attrib.align); 
        }

        void AddGlyph(Glyph* glyph);

        void Clear();
        void DrawFlush(RenderMan* render);

    public:
        inline void SetLeftAligned(){ attrib.align = A_LEFT; }
        inline void SetRightAligned(){ attrib.align = A_RIGHT; }
        inline void SetCentralAligned(){ attrib.align = A_CENTRAL; }
        inline void SetAttrib(const Attrib_Line & att){ attrib = att; }

        inline double GetHeight() { return attrib.height; }
        inline double GetIndent() { return attrib.indent; }

    private:
        std::vector<Glyph*> glyphs;

    private:
        Attrib_Line attrib;
        uint32      curWidth;
        uint32      pageWidth;
        uint32      margin;

    private:
        LayoutManager* layout;
        Logger*     logger;
};

#endif
