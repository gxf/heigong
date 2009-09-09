#include "Glyph.h"
#include "FontsCache.h"
#include <cstdlib>
#include <cstring>
#include <iostream>

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
    uint8 tmprow[width];
    uint8 * p = (uint8 *)bitmap;

#ifndef NOGL
    // Also change the color from light to dark pattern
    for (i = 0; i < height - i; i++){
        std::memcpy(tmprow, p + i * width, width);
        for (j = 0; j < width; j++){
            tmprow[j] = 0xff - tmprow[j];
        }
        std::memcpy(p + i * width, p + (height - i) * width, width);
        uint8* pp = p + i * width;
        for (j = 0; j < width; j++){
            *(pp + j) = 0xff - *(pp + j);
        }
        std::memcpy(p + (height - i) * width, tmprow, width);
    }
    // Setup the middle line
    if ( i == height - i){
        uint8* pp = p + i * width;
        for (j = 0; j < width; j++){
            *(pp + j) = 0xff - *(pp + j);
        }
    }
#else
    // Just change the background to white when it is not GL coordinate
    for (i = 0; i <= height; i++){
        uint8* pp = p + i * width;
        for (j = 0; j < width; j++){
            *(pp + j) = 0xff - *(pp + j);
        }
    }
#endif
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
    if (mem) delete [] mem;
}

void FontsCache::ForceEvict(){
    Evict();
}

void FontsCache::Evict(){
//    elems.erase(std::unique(elems.begin(),elems.end()), elems.end());
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
