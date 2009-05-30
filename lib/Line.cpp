#include "Context.h"

void Line::AddGlyph(Glyph* glyph){
    glyphs.push_back(glyph);
}

void Line::DrawCurrent(Context* ctx, int baseline){
    std::vector<Glyph*>::iterator itr = glyphs.begin();
    while (itr != glyphs.end()){
        (*itr)->AdjustPos(0, baseline);
        (*itr)->Draw(&(ctx->render));
        ++itr;
    }
}

void Line::Clear(){
    glyphs.clear();
}

void Line::DrawFlush(Context* ctx){
    int baseline = ctx->layout.GetLastBaseLine();
    std::vector<Glyph*>::iterator itr = glyphs.begin();
    while (itr != glyphs.end()){
        (*itr)->AdjustPos(0, baseline);
        (*itr)->Draw(&(ctx->render));
        ++itr;
    }
    glyphs.clear();
}
