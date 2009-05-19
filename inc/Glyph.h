#ifndef GLYPH_H
#define GLYPH_H

//namespace heigong{

class Position;
class RenderMan;

class Glyph{
    public:
        Glyph(){};
        virtual ~Glyph(){}

    public:
//        virtual bool Gen() = 0;
        virtual bool Draw(RenderMan*) = 0;
        virtual bool AdjustPos(int baseline) = 0;
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
        Char(Position p = Position(0, 0), int bl = 0, int bm_w = 0, int bm_h = 0, void* bm = 0, ID id = ID(0, 0));
        ~Char();

    public:
        enum ENCODING_MODE{
            EM_ASCII,
            EM_UTF_8,
        };

    public:
        inline void SetVal(unsigned int v) { val = v; }
        inline unsigned int GetVal() { return val; }
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

    public:
        ID              id;

};

class Graph: public Glyph{
    public:
        Graph(){}
        ~Graph(){}

    public:
};

class Table: public Glyph{
    public:
};

//} // namespace heigong

#endif

