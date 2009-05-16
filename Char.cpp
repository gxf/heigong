#include "Common.h"
#include "Glyph.h"
#include "RenderMan.h"

//using namespace heigong;

Char::Char(Position p, int bl, int bm_w, int bm_h, void* bm):
    pos(p), baseline(bl), 
    bitmap_w(bm_w), bitmap_h(bm_h), bitmap(bm)
{
}

Char::~Char(){
}

void Char::SetPos(const Position & p){
    pos = p;
}

bool Char::Adjust(int bl){
    int delta = bl - baseline;
    if (delta < 0)
        return false;
    pos.y += delta;
    return true;
}

bool Char::Draw(RenderMan * render){
    return render->RenderGrayMap(pos.x, pos.y, bitmap_w, bitmap_h, bitmap);
}

