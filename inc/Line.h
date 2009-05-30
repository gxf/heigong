#ifndef HG_LINE_H
#define HG_LINE_H

#include "Glyph.h"
#include "FontsCache.h"
#include <vector>

class RenderMan;
class Logger;
class Context;

// Attributes for line
enum ALIGNMENT{
    A_LEFT,
    A_RIGHT,
    A_CENTER,
};

// Note: Line is just a abstract concept which does not
// get any Glypth Object memory management involved
class Line{
    public:
        Line(Logger* log, ALIGNMENT a = A_LEFT): 
            align(a), curWidth(0),
            logger(log)
        {}
        ~Line(){}

    public:
        void AddGlyph(Glyph* glyph);

        void DrawCurrent(Context* ctx, int baseline);

        void Clear();

        void DrawFlush(Context* ctx);

    private:
        std::vector<Glyph*> glyphs;

    private:
        ALIGNMENT   align;
        uint32      curWidth;

    private:
        Logger*     logger;
};

#endif
