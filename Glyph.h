#ifndef GLYPH_H
#define GLYPH_H

//namespace heigong{

class Glyph{
    public:
        Glyph(){};
        virtual ~Glyph(){}

    public:
        virtual bool Gen() = 0;
        virtual bool Draw() = 0;
};

class Char: public Glyph{
    public:
        Char();
        ~Char();

    public:
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

