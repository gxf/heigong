#ifndef HG_LINE_H
#define HG_LINE_H

#include "Glyph.h"
#include "FontsCache.h"
#include <vector>

class RenderMan;

// Note: Line is just a abstract concept which does not
// get any Glypth Object memory management involved
class Line{
    public:
        Line(){}
        ~Line(){}

    public:
        void AddGlyph(Glyph* glyph){
            glyphs.push_back(glyph);
        }

        void Flush(RenderMan * render, FontsCache * cache, int baseline){
            std::vector<Glyph*>::iterator itr = glyphs.begin();
            while (itr != glyphs.end()){
                (*itr)->AdjustPos(baseline);
                (*itr)->Draw(render);
                ++itr;
            }
        }

        void Clear(){
            glyphs.clear();
        }

    private:
        std::vector<Glyph*> glyphs;
};

#endif
