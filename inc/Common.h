#ifndef COMMON_H
#define COMMON_H

typedef unsigned int    uint32;
typedef unsigned char   uchar8; 

class Except_EOF{};         // EOF is met
class Except_Parse_Err{};   // parse error

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

#define SCREEN_WIDTH    800
#define SCREEN_HEIGHT   600

#define DEFAULT_FONT    "/usr/share/fonts/truetype/ttf-droid/DroidSansFallback.ttf"
#define DEFAULT_FONT_SIZE   12

#define DEFAULT_RESERVED_PAGE   8
#define DEFAULT_MAX_PAGE_SIZE   0x800000
#define DEFAULT_FONTS_CACHE_SIZE    1024*1024

#define DEFAULT_TMP_FILE_NAME   "tmp.hg"
#endif


