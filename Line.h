#ifndef HG_LINE_H
#define HG_LINE_H

#include "Glyph.h"
#include "FontsCache.h"
#include <vector>

class RenderMan;

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
                (*itr)->Adjust(baseline);
                (*itr)->Draw(render);
//                cache->DelChar(*itr);
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
