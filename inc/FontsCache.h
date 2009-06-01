#ifndef FONTS_CACHE_H
#define FONTS_CACHE_H

#include "Common.h"
#include <vector>

class Char;

class FontsCache{
    public:
        FontsCache();
        ~FontsCache();

    public:
        void AdjustBitmap(int width, int height, void* bitmap);
        void CacheFont(Char* ch, int width, int height, void* bitmap);
        void CacheFont2(Char* ch, int width, int height, void* bitmap); // Still cache font bitmap, but without bitmap adjust
        void DelChar(Char* ch);
        void ForceEvict();

    private:
        static uint32 cacheSize; // Cache Size in Bytes

    private:
        void Evict();
        void* AquireMem(int size);
        void ReleaseMem(char mem[]);

    private:
        uint32  curSize;
        std::vector<Char*> elems;
};

#endif
