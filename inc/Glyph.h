#ifndef GLYPH_H
#define GLYPH_H

//namespace heigong{

class Position;
class RenderMan;
class Logger;

class Glyph{
    public:
        Glyph(Logger* log): logger(log)
        {};
        virtual ~Glyph(){}

    public:
//        virtual bool Gen() = 0;
        virtual bool Draw(RenderMan*) = 0;
        virtual bool AdjustPos(int baseline) = 0;

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
        Char(Logger* log, Position p = Position(0, 0), int bl = 0, 
             int bm_w = 0, int bm_h = 0, void* bm = 0, 
             ID id = ID(0, 0));
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
        bool AdjustPos(int);

    public:
        Position        pos;
        int             baseline;
        int             bitmap_w;   // Bitmap width
        int             bitmap_h;   // Bitmap height
        void *          bitmap;

        ENCODING_MODE   encodeMode;
        unsigned int    val;
        unsigned int    charLen;

    public:
        ID              id;

};

class Graph: public Glyph{
    public:
        Graph(Logger* log):Glyph(log){}
        ~Graph(){}

    public:
};

class Table: public Glyph{
    public:
};

//} // namespace heigong

#endif

