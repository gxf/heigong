#include "Common.h"
#include "Logger.h"
#include "Glyph.h"
#include "FontsCache.h"
#include "FontsManager.h"
#include "LayoutManager.h"
#include "RenderMan.h"
#include "utf8.h"
#include <vector>

//using namespace heigong;
static Logger ftlogger;
FontsCache Char::ftCache;
FontsManager Char::ftMgr(&ftlogger);

uint32 Char::magic_num = 'c' + 'h' + 'a' + 'r';

std::ifstream & operator>>(std::ifstream &ifs, Char::ID & id){
    ifs >> id.name;
    ifs >> id.pt;
    return ifs;
}

std::ofstream & operator<<(std::ofstream &ofs, Char::ID & id){ 
    ofs << id.name; 
    ofs << id.pt;
    return ofs;
}

Char::Char(Logger* log):
    Glyph(log), baseline(0), 
    encodeMode(EM_UTF_8), val(0), charLen(1),
    id(NULL, 0), valid(true)
{}

Char::Char(Logger* log, uint32 v):
    Glyph(log), baseline(0), 
    encodeMode(EM_UTF_8), val(v), charLen(1),
    id(NULL, 0), valid(true)
{}

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

bool Char::Adjust2Baseline(int bl){
    int delta = bl - baseline;
    if (delta < 0)
        return false;
    pos.y += delta;
    return true;
}

bool Char::Relocate(int x, int y){
    pos.x += x;
    pos.y += y;
    return true;
}

bool Char::Draw(RenderMan & render){
    return render.RenderGrayMap(pos.x, pos.y, bitmap_w, bitmap_h, bitmap);
}

Glyph::GY_ST_RET Char::Setup(LayoutManager& layout){
    if ('\n' == GetVal()){
        switch(layout.NewLine()){
            case LO_OK:
                return GY_OK;
            case LO_NEW_PAGE:
                layout.Reset();
                return GY_NEW_PAGE;
            default:
                LOG_ERROR("Unsupported Layout Newline return.");
                return GY_ERROR;
        }
    }
    FT_GlyphSlot glyphSlot;

    if (0 == attrib.size || 
        false == ftMgr.SetFontSize(attrib.size)){
        ftMgr.SetFontSize(DEFAULT_FONT_SIZE);
    }
    ftMgr.GetGlyphSlot((FT_ULong)GetVal(EM_UTF_32), &glyphSlot);
    baseline = ((glyphSlot->metrics.horiBearingY) >> 6);
    ftCache.CacheFont(this, 
            glyphSlot->bitmap.pitch, glyphSlot->bitmap.rows, 
            glyphSlot->bitmap.buffer);
    ftMgr.SetFontSize(DEFAULT_FONT_SIZE);
    LAYOUT_RET ret = 
        layout.GetCharPos(pos, (glyphSlot->advance.x) >> 6, 
                          glyphSlot->bitmap.rows, baseline);
    pos.x += ((glyphSlot->metrics.horiBearingX) >> 6);
    switch(ret){
        case LO_OK:
            layout.AddGlyph(this);
            break;
        case LO_NEW_LINE:
            layout.AddGlyph(this);
            break;
        case LO_NEW_PAGE:
            layout.Reset();
            return GY_NEW_PAGE;
        default:
            LOG_ERROR("Unsupported Layout return.");
            break;
    }

    return GY_OK;
}

Glyph * Char::UngetSet(){
    // Mark for deleting
    valid = false;
    Char* ch = new Char(logger);
    ch->encodeMode  = this->encodeMode;
    ch->val         = this->val;
    ch->charLen     = this->charLen;
    ch->id          = this->id;
    return ch;
}

Glyph* Char::Dup(){
    Char* ch = new Char(logger);

    ch->pos         = this->pos;
    ch->bitmap_w    = this->bitmap_w;
    ch->bitmap_h    = this->bitmap_h;
    // Always let the cache assign memory
    if (bitmap != NULL){
        ftCache.CacheFont(ch, bitmap_w , bitmap_h, bitmap);
    }
    else{
        ch->bitmap = NULL;
    }
    ch->baseline    = this->baseline;
    ch->encodeMode  = this->encodeMode;
    ch->val         = this->val;
    ch->charLen     = this->charLen;
    ch->id          = this->id;
    ch->valid       = this->valid;
    ch->attrib      = this->attrib;

    return ch;
}

void Char::Serialize(std::ofstream & ofs){

    SER_OBJ(magic_num);
    SER_OBJ(encodeMode);
    SER_OBJ(val);
    SER_OBJ(charLen);
    SER_OBJ(id);
    SER_OBJ(attrib);
//    ofs << id;
}

void Char::Deserialize(std::ifstream & ifs){
//    DESER_OBJ(valid);
    DESER_OBJ(encodeMode);
    DESER_OBJ(val);
    DESER_OBJ(charLen);
    DESER_OBJ(id);
    DESER_OBJ(attrib);
//    ifs >> id;
}

