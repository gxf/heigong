#include "Glyph.h"
#include "FontsCache.h"
#include <cstdlib>
#include <cstring>

uint32 FontsCache::cacheSize = DEFAULT_FONTS_CACHE_SIZE;

FontsCache::FontsCache():
    curSize(0)
{}

FontsCache::~FontsCache(){
    ForceEvict();
}

void FontsCache::AdjustBitmap(int width, int height, void* bitmap){
    // This function rotate the bitmap up side down
    // for GL & freetype defines the bitmap starting
    // point differently. 
    // (GL - bottom-left; freetype - top-left);

    height--;
    int i, j;
    unsigned char tmprow[width];
    unsigned char* p = (unsigned char*)bitmap;

    // Also change the color from light to dark pattern
    for (i = 0; i < height - i; i++){
        std::memcpy(tmprow, p + i * width, width);
        for (j = 0; j < width; j++){
            tmprow[j] = 0xff - tmprow[j];
        }
        std::memcpy(p + i * width, p + (height - i) * width, width);
        unsigned char* pp = p + i * width;
        for (j = 0; j < width; j++){
            *(pp + j) = 0xff - *(pp + j);
        }
        std::memcpy(p + (height - i) * width, tmprow, width);
    }
    if ( i == height - i){
        unsigned char* pp = p + i * width;
        for (j = 0; j < width; j++){
            *(pp + j) = 0xff - *(pp + j);
        }
    }
}

void FontsCache::CacheFont(Char* ch, int width, int height, void* bitmap){
    // Cache & convert bitmap for local storage
    char* p = (char*)AquireMem(width * height);
    std::memcpy(p, bitmap, width * height);
    AdjustBitmap(width, height, p);

    ch->SetBitmap(width, height,p);

    // TODO: Manage memory
    elems.push_back(ch);
}

void FontsCache::CacheFont2(Char* ch, int width, int height, void* bitmap){
    char* p = (char*)AquireMem(width * height);
    std::memcpy(p, bitmap, width * height);
    ch->SetBitmap(width, height,p);

    // TODO: Manage memory
    elems.push_back(ch);
}

void FontsCache::DelChar(Char * ch){
    if (NULL == ch)
        return;
    ReleaseMem((char*)ch->GetBitmap());
    delete ch;
    ch = NULL;
}

void * FontsCache::AquireMem(int size){
    return new char[size];
}

void FontsCache::ReleaseMem(char mem[]){
    delete [] mem;
}

void FontsCache::ForceEvict(){
    Evict();
}

void FontsCache::Evict(){
    std::vector<Char*>::iterator itr = elems.begin();
    while(itr != elems.end()){
        if(false == (*itr)->valid){
            DelChar(*itr);
            itr = elems.erase(itr);
        }
        else{
            ++itr;
        }
    }
}
