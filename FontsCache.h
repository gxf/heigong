#ifndef FONTS_CACHE_H
#define FONTS_CACHE_H

class FontsCache{
    public:
        FontsCache();
        ~FontsCache();

    public:
        void AdjustBitmap(int width, int height, void* bitmap);
};

#endif
