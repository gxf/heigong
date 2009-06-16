#include "Logger.h"
#include "PageLayout.h"
#include "RenderMan.h"

void Line::AddGlyph(Glyph* glyph){
    glyphs.push_back(glyph);
    uint32 maxX = glyph->pos.x + glyph->bitmap_w;
    curWidth = (curWidth > maxX) ? curWidth : maxX;
}

void Line::Clear(){
    glyphs.clear();
//    attrib.Reset();
    curWidth = margin;
}

void Line::DrawFlush(RenderMan* render){
    int baseline = layout->GetLastBaseLine();
    int xShift = 0;
    switch(attrib.align){
        case A_LEFT:
//            xShift = attrib.indent;
            break;
        case A_CENTRAL:
            xShift = (pageWidth - margin - curWidth) / 2;
            break;
        case A_RIGHT:
            xShift = pageWidth - margin - curWidth;
            break;
        default:
            LOG_ERROR("Unsupported alignment type.");
            break;
    }
    std::vector<Glyph*>::iterator itr = glyphs.begin();
    while (itr != glyphs.end()){
        (*itr)->Relocate(xShift, baseline);
        (*itr)->Draw(render);
        ++itr;
    }
    Clear();
}

