#ifndef FONTS_CACHE_H
#define FONTS_CACHE_H

class Char;

class FontsCache{
    public:
        FontsCache();
        ~FontsCache();

    public:
        void AdjustBitmap(int width, int height, void* bitmap);
        void CacheFont(Char* ch, int width, int height, void* bitmap);
        void DelChar(Char* ch);

    private:
        void* AquireMem(int size);
        void ReleaseMem(char mem[]);
};

#endif
