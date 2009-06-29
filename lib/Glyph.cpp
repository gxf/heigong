#include "Glyph.h"

void Glyph::Serialize(std::ofstream & ofs){
    SER_OBJ(pos);
    SER_OBJ(bitmap_w);
    SER_OBJ(bitmap_h);
    // Do not deal with bitmap memory directly
}

void Glyph::Deserialize(std::ifstream & ifs){
    DESER_OBJ(pos);
    DESER_OBJ(bitmap_w);
    DESER_OBJ(bitmap_h);
    // Do not deal with bitmap memory directly
}
