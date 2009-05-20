#ifndef COMMON_H
#define COMMON_H

class Position{
    public:
        int x;
        int y;
    public:
        Position(int u = 0, int v = 0): x(u), y(v)
        {}
        Position & operator=(const Position & p){
            if (&p == this)
                return *this;
            x = p.x;
            y = p.y;
            return *this;
        }
};

enum ENCODING_MODE{
    EM_ASCII,
    EM_UTF_8,
    EM_UTF_16,
    EM_UTF_32,
};

#define SCREEN_WIDTH    640
#define SCREEN_HEIGHT   480

//#define DEFAULT_FONT    "/usr/share/fonts/truetype/wqy/wqy-zenhei.ttf"
#define DEFAULT_FONT    "/usr/share/fonts/truetype/ttf-droid/DroidSansFallback.ttf"
//#define DEFAULT_FONT    "/usr/share/fonts/truetype/freefont/FreeSans.ttf"
//#define DEFAULT_FONT    "/usr/share/fonts/truetype/ttf-dejavu/DejaVuSansCondensed.ttf"
#define DEFAULT_FONT_SIZE   12
#endif


