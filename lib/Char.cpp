#include "Context.h"
#include "utf8.h"
#include <vector>

//using namespace heigong;
FontsCache Char::ftCache;

Char::Char(Logger* log, int bl, ID cid):
    Glyph(log), baseline(bl), 
    encodeMode(EM_UTF_8), val(0), charLen(1),
    id(cid), valid(true)
{
}

Char::~Char(){
    valid = false;
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

bool Char::AdjustPos(int align, int bl){
    int delta = bl - baseline;
    if (delta < 0)
        return false;
    pos.y += delta;
    pos.x += align;
    return true;
}

bool Char::Draw(RenderMan * render){
    return render->RenderGrayMap(pos.x, pos.y, bitmap_w, bitmap_h, bitmap);
}

bool Char::Setup(Context* ctx){
    if ('\n' == GetVal()){
        switch(ctx->layout.NewLine()){
            case LO_OK:
                ctx->line.DrawFlush(ctx);
                return true;
            case LO_NEW_PAGE:
                ctx->line.DrawFlush(ctx);
                ctx->layout.Reset();
                return false;
            default:
                LOG_ERROR("Unsupported Layout Newline return.");
                return false;
        }
    }
    FT_GlyphSlot glyphSlot;

    ctx->fonts.GetGlyphSlot((FT_ULong)GetVal(EM_UTF_32), &glyphSlot);
    baseline = ((glyphSlot->metrics.horiBearingY) >> 6);
    ftCache.CacheFont(this, 
            glyphSlot->bitmap.pitch, glyphSlot->bitmap.rows, 
            glyphSlot->bitmap.buffer);
    LAYOUT_RET ret = 
        ctx->layout.GetCharPos(pos, (glyphSlot->advance.x) >> 6, 
                               glyphSlot->bitmap.rows, baseline);
    pos.x += ((glyphSlot->metrics.horiBearingX) >> 6);
    switch(ret){
        case LO_OK:
            ctx->line.AddGlyph(this);
            break;
        case LO_NEW_LINE:
            ctx->line.DrawFlush(ctx);
            ctx->line.AddGlyph(this);
            break;
        case LO_NEW_PAGE:
            ctx->line.DrawFlush(ctx);
            ctx->layout.Reset();
            return false;
        default:
            LOG_ERROR("Unsupported Layout return.");
            break;
    }

    return true;
}

Glyph* Char::Dup(){
    Char* ch = new Char(logger);

    ch->pos         = this->pos;
    ch->bitmap_w    = this->bitmap_w;
    ch->bitmap_h    = this->bitmap_h;
    // Always let the cache assign memory
    ftCache.CacheFont(ch, bitmap_w , bitmap_h, bitmap);
    ch->baseline    = this->baseline;
    ch->encodeMode  = this->encodeMode;
    ch->val         = this->val;
    ch->charLen     = this->charLen;
    ch->id          = this->id;
    ch->valid       = this->valid;

    return ch;
}

