#include "Common.h"
#include "Glyph.h"
#include "RenderMan.h"

//using namespace heigong;

Char::Char(Position p, int bl, int bm_w, int bm_h, void* bm, ID cid):
    pos(p), baseline(bl), 
    bitmap_w(bm_w), bitmap_h(bm_h), bitmap(bm),
    encodeMode(EM_UTF_8), val(0),
    id(cid)
{
}

Char::~Char(){
}

bool Char::AdjustPos(int bl){
    int delta = bl - baseline;
    if (delta < 0)
        return false;
    pos.y += delta;
    return true;
}

bool Char::Draw(RenderMan * render){
    return render->RenderGrayMap(pos.x, pos.y, bitmap_w, bitmap_h, bitmap);
}

