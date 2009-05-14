#include "FontsCache.h"
#include <cstdlib>
#include <cstring>

FontsCache::FontsCache(){
}

FontsCache::~FontsCache(){
}

void FontsCache::AdjustBitmap(int width, int height, void* bitmap){
    // This function rotate the bitmap up side down
    // for GL & freetype defines the bitmap starting
    // point differently. 
    // (GL - bottom-left; freetype - top-left);

    int i;
    unsigned char tmprow[width];
    unsigned char* p = (unsigned char*)bitmap;

    for (i = 0; i <= (height - 1) / 2; i++){
        std::memcpy(tmprow, p + i * width, width);
        std::memcpy(p + i * width, p + (height - i) * width, width);
        std::memcpy(p + (height - i) * width, tmprow, width);
    }
}
