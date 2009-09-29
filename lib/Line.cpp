#include "Logger.h"
#include "PageLayout.h"
#include "RenderMan.h"

void Line::AddGlyph(Glyph* glyph){
    glyphs.push_back(glyph);
    uint32 maxX = glyph->pos.x + glyph->bitmap_w;
    curWidth = (curWidth > maxX) ? curWidth : maxX;
}

void Line::Clear(){
    std::vector<Glyph*>::iterator itr = glyphs.begin();
#if 0
    while (itr != glyphs.end()){
        Char* pchr = dynamic_cast<Char*>(*itr);
        if(pchr && '\n' == pchr->GetVal()){
            // Characters except '\n' is handled by FontCache Manager
            delete pchr;
        }
        else{
            delete *itr;
        }
        ++itr;
    }
#endif
#if 0
    while (itr != glyphs.end()){
        Char* pchr = dynamic_cast<Char*>(*itr);
        if(pchr){ 
            pchr->valid = false; 
        }
        ++itr;
    }
    glyphs.clear();
#endif
//    attrib.Reset();
//    Char::ClearCache();
    curWidth = margin;
}

void Line::RelocLine(){
    int baseline = layout->GetLastBaseLine();
//    LOG_EVENT_STR2("[LINE] Reloc: Current baseline", baseline);
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
        (*itr)->Adjust2Baseline(baseline);
        (*itr)->Relocate(xShift, 0);
//        (*itr)->Relocate(xShift, baseline);
        ++itr;
    }
}

void Line::DrawFlush(RenderMan* render){
    RelocLine();
    std::vector<Glyph*>::iterator itr = glyphs.begin();
    while (itr != glyphs.end()){
        (*itr)->Draw(*render);
        ++itr;
    }
    Clear();
}

