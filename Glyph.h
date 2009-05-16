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
        virtual bool Adjust(int baseline) = 0;
};

class Char: public Glyph{
    public:
        Char(Position p, int bl, int bm_w, int bm_h, void* bm);
        ~Char();

    public:
        inline void* GetBitmap()   { return bitmap; }
        bool Draw(RenderMan*);
        bool Adjust(int);
        void SetPos(const Position & p);

    public:
        Position pos;
        int baseline;
        int bitmap_w;   // Bitmap width
        int bitmap_h;   // Bitmap height
        void* bitmap;
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

