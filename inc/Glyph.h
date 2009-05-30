#ifndef GLYPH_H
#define GLYPH_H

//namespace heigong{
#include "Common.h"
#include <stdio.h>

class Position;
class RenderMan;
class Line;
class LayoutManager;
class PageManager;
class Logger;
class Context;

class Glyph{
    public:
        Glyph(Logger* log): 
            pos(0, 0), 
            bitmap_w(0), bitmap_h(0), bitmap(NULL),
            logger(log)
        {};
        virtual ~Glyph(){}

    public:
        virtual bool Draw(RenderMan*) = 0;
        virtual bool AdjustPos(int x, int y) = 0;
        virtual bool Setup(Context* ctx) = 0;

    public:
        Position        pos;        // Left-bottom position
        int             bitmap_w;   // Bitmap width
        int             bitmap_h;   // Bitmap height
        void *          bitmap;

    protected:
        Logger* logger;

};

class Char: public Glyph{
    public:
        class ID{
            public:
                ID(const char * n, int size):
                    name(n), pt(size)
                {}
                // Trivial copy constructor & assigment operator
            public:
                const char* name;
                int         pt;
        };

    public:
        Char(Logger* log, int bl = 0, ID id = ID(0, 0));
        ~Char();

    public:
        inline void SetVal(unsigned int v) { val = v; }
        inline void SetCharLength(unsigned int len) { charLen = len; }
        inline void SetPos(const Position & p){ pos = p; }
        inline void SetBitmap(int bw, int bh, void* b){
            bitmap_w = bw;
            bitmap_h = bh;
            bitmap   = b;
        }
        inline void SetBaseline(int b){ baseline = b; }
        inline void SetID(ID cid){
            id = cid;
        }
        inline void* GetBitmap() { return bitmap; }
        inline ENCODING_MODE GetEncoding(){ return encodeMode; }
        inline bool operator==(char ch){ return (char)val == ch; }
        inline bool operator!=(char ch){ return (char)val != ch; }
        inline bool operator==(Char& ch){ return val == ch.val; }

    public:
        unsigned int GetVal(ENCODING_MODE em = EM_UTF_8);
        bool Draw(RenderMan*);
        bool AdjustPos(int, int);
        bool Setup(Context* ctx);

    public:
        int             baseline;

        ENCODING_MODE   encodeMode;
        unsigned int    val;
        unsigned int    charLen;

    public:
        ID              id;

};

class Image: public Glyph{
    public:
        Image(Logger* log):Glyph(log){}
        ~Image(){}

    public:
        bool Draw(RenderMan*);
        bool AdjustPos(int, int);
        bool Setup(Context* ctx);
};

class Table: public Glyph{
    public:
};

//} // namespace heigong

#endif

