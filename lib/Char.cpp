#include "Common.h"
#include "Logger.h"
#include "Glyph.h"
#include "RenderMan.h"
#include "utf8.h"
#include <vector>

//using namespace heigong;

Char::Char(Logger* log, Position p, int bl, int bm_w, int bm_h, void* bm, ID cid):
    Glyph(log), pos(p), baseline(bl), 
    bitmap_w(bm_w), bitmap_h(bm_h), bitmap(bm),
    encodeMode(EM_UTF_8), val(0), charLen(1),
    id(cid)
{
}

Char::~Char(){
}

unsigned int Char::GetVal(ENCODING_MODE em){
    if (em == encodeMode){
        return val;
    }
    else if (EM_UTF_8 == encodeMode){
        if (1 == charLen){
            return val;
        }
        else if (EM_UTF_32 == em){
            std::vector<unsigned int> utf32val; 
//            utf8::utf8to32((char*)&val, (char*)(&val) + charLen - 1, std::back_inserter(utf32val));
            utf8::utf8to32((char*)&val, (char*)(&val) + charLen, std::back_inserter(utf32val));
            return utf32val[0];
        }
        else{
            LOG_WARNING("Convertion from utf8. Unsupported code conversion.");
            return 0;
        }
    }
    else{
        LOG_WARNING("Unsupported code conversion.");
        return 0;
    }
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

