#ifndef HG_LINE_H
#define HG_LINE_H

#include "Common.h"
#include "Glyph.h"
#include "FontsCache.h"
#include <vector>

class RenderMan;
class Logger;
class Context;

// Note: Line is just a abstract concept which does not
// get any Glypth Object memory management involved
class Line{
    public:
        Line(Logger* log, uint32 pw, uint32 m, ALIGNMENT a = A_LEFT): 
            curWidth(0),
            pageWidth(pw), margin(m),
            logger(log)
        {}
        ~Line(){}

    public:
        void AddGlyph(Glyph* glyph);

        void DrawCurrent(Context* ctx, int baseline);
        void Clear();
        void DrawFlush(Context* ctx);

    public:
        inline void SetLeftAligned(){ attrib.align = A_LEFT; }
        inline void SetRightAligned(){ attrib.align = A_RIGHT; }
        inline void SetCentralAligned(){ attrib.align = A_CENTRAL; }
        inline void SetAttrib(const Attrib_Line & att){ attrib = att; }

    private:
        std::vector<Glyph*> glyphs;

    private:
        Attrib_Line attrib;
        uint32      curWidth;
        uint32      pageWidth;
        uint32      margin;

    private:
        Logger*     logger;
};

#endif
