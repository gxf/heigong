#ifndef COMMON_H
#define COMMON_H

#include <fstream>
#include <string>

typedef long long int64;
typedef int    int32;
typedef short  int16;
typedef char   char8; 
typedef char8  int8; 

typedef unsigned long long uint64;
typedef unsigned int    uint32;
typedef unsigned short  uint16;
typedef unsigned char   uchar8; 
typedef uchar8          uint8; 

#define MAX_UINT32   ((uint32)-1) 
#define MAX_INT32   ((1 << (sizeof(int32) << 3) - 1) - 1) 

class Except_EOF{};         // EOF is met
class Except_Parse_Err{};   // parse error
class Except_Fail_To_Open_File{};
class Except_Fail_To_Read_file{};

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
    public:
        friend std::ifstream & operator>>(std::ifstream & ifs, Position & pos);
        friend std::ofstream & operator<<(std::ofstream & ifs, Position & pos);

};

#if 0
class hgFloat{
    public:
        hgFloat(int64 v = 0, uint32 b = 0): val(v), base(b){}

    public:
        int32 operator int32(){
            return (int32)(val / base);
        }

        inline hgFloat operator+(hgFloat& f){
            hgFloat ret;
            if (f.base > base){
                ret.base = base;
                ret.val = f.base * f.val / base + val;
            }
            else if (f.base == base){
                ret.base = base;
                ret.val = f.val + val;
            }
            else{
                ret.base = f.base;
                ret.val = base * val / f.base + f.val;
            }
            return ret;
        }

        inline hgFloat operator-(hgFloat& f){
            hgFloat ret;
            if (f.base > base){
                ret.base = base;
                ret.val = val - f.base * f.val / base;
            }
            else if (f.base == base){
                ret.base = base;
                ret.val = val - f.val;
            }
            else{
                ret.base = f.base;
                ret.val = base * val / f.base - f.val;
            }
            return ret;
        }

        inline hgFloat operator*(int32& d){
        }

        hg
    public:
        int64   val;
        uint32  base;
};
#endif

// Attributes for line
enum ALIGNMENT{
    A_LEFT = 0,
    A_RIGHT,
    A_CENTRAL,
};

class Attrib_Glyph{
    public:
        bool bold;
        bool italic;
        int  size;
//        std::string font;

    public:
        Attrib_Glyph():
          bold(false), italic(false), size(0)//, font("")
        {}

        // Trivial consturctor & copy constructor;
    
    public:
        void Reset(){ 
            bold    = false; 
            italic  = false;
            size    = 0;
//            font.clear();
        }

    public:
        friend std::ifstream & operator>>(std::ifstream & ifs, Attrib_Glyph & ag);
        friend std::ofstream & operator<<(std::ofstream & ifs, Attrib_Glyph & ag);
};

class Attrib_Line{
    // Common attribute for glyph
    public:
        ALIGNMENT   align;
        double      indent;
        double      height;

        // Trivial consturctor & copy constructor;
    public:
        Attrib_Line(ALIGNMENT a = A_LEFT, double i = 0, double h = 0):
            align(a), indent(i), height(h)
        {}
        void Reset(){ align = A_LEFT; indent = 0; height = 0;}

    public:
        friend std::ifstream & operator>>(std::ifstream & ifs, Attrib_Line & al);
        friend std::ofstream & operator<<(std::ofstream & ifs, Attrib_Line & al);
};

enum ENCODING_MODE{
    EM_ASCII = 0,
    EM_UTF_8,
    EM_UTF_16,
    EM_UTF_32,
};


#define DPI_DFT         96
#define SCREEN_WIDTH    600
#define SCREEN_HEIGHT   800

#define MARGIN_VERTICAL     30
#define MARGIN_HORIZONTAL   30

#define PAGE_WIDTH  (scr_width - 2 * MARGIN_HORIZONTAL)
#define PAGE_HEIGHT (scr_height - 2 * MARGIN_VERTICAL)

#define TABLE_MARGIN_VERTICAL     0
#define TABLE_MARGIN_HORIZONTAL   2

#define IMAGE_GUARD_SIZE    40
#define FB_GUARD_SIZE       256

#define TAB_MARGIN  2

#define DEFAULT_FONT    "/usr/share/fonts/truetype/ttf-droid/DroidSansFallback.ttf"
#define DEFAULT_FONT_SIZE   12

#define DEFAULT_RESERVED_PAGE       8
#define DEFAULT_MAX_PAGE_SIZE       0x800000
#define DEFAULT_FONTS_CACHE_SIZE    1024*1024

#define DEFAULT_TMP_FILE_NAME   "tmp.hg"
#define DEFAULT_WORK_DIR        "./"

#define TIME_WAIT   1000
#define TIMES_RETRY 100

#define PROC_WORD_MAX 10

extern char* work_dir;
extern char* html_dir;
extern uint32 scr_width;
extern uint32 scr_height;
extern uint32 g_dpi;

#endif


