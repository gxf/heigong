#ifndef GLYPH_H
#define GLYPH_H

//namespace heigong{
#include "Common.h"
#include "Serializer.h"
#include <cstdio>
#include <fstream>
#include <string>

class Position;
class RenderMan;
class Line;
class LayoutManager;
class PageManager;
class Logger;
class FontsCache;
class FontsManager;

class Glyph : public Serializer{
    public:
        // Setup return enum of glyph
        enum GY_ST_RET{
            GY_OK = 0,
            GY_NEW_PAGE,
            GY_EOF,
            GY_ERROR,
        };
    public:
        Glyph(Logger* log): 
            pos(0, 0), 
            bitmap_w(0), bitmap_h(0), bitmap(NULL),
            logger(log)
        {};
        virtual ~Glyph(){}

    public:
        virtual bool Draw(RenderMan&) = 0;
        virtual bool Relocate(int x, int y) = 0;
        virtual bool Adjust2Baseline(int baseline) = 0;
        virtual GY_ST_RET Setup(LayoutManager& lo) = 0;
        virtual Glyph * Dup() = 0;
        virtual Glyph * UngetSet() = 0;

    public:
        virtual uint32 GetMagic() = 0;
        void Serialize(std::ofstream & ofs);
        void Deserialize(std::ifstream & ifs);

    public:
        Position    pos;        // Left-bottom position
        int32       bitmap_w;   // Bitmap width
        int32       bitmap_h;   // Bitmap height
        void *      bitmap;

    protected:
        Logger* logger;

};

class Eof : public Glyph{
    public:
        Eof(Logger* log): Glyph(log){}

    public:
        bool Draw(RenderMan&){ return true; }
        bool Relocate(int x, int y){ return true; }
        bool Adjust2Baseline(int baseline){ return true; }
        GY_ST_RET Setup(LayoutManager& lo) { return GY_EOF; }
        Glyph * Dup() { return this; }
        Glyph * UngetSet() { return this; }

    public:
        // Serialize support only involves the magic number
        uint32 GetMagic(){ return magic_num; }
        void Serialize(std::ofstream & ofs){ SER_OBJ(magic_num);}
        void Deserialize(std::ifstream & ifs){}

    private:
        static uint32 magic_num;
};

class Char: public Glyph{
    public:
        class ID{
            public:
                ID(const char * n, int size):
                    pt(size)
                {
                    if (NULL != n){
                        name = std::string(n);
                    }
                }
            public:
                ID(ID & id){
                    name    = id.name;
                    pt      = id.pt;
                }

                ID & operator=(const ID & id){
                    if (this != &id){
                        name    = id.name;
                        pt      = id.pt;
                    }
                    return *this;
                }
                    
                friend std::ifstream & operator>>(std::ifstream &ifs, ID & id);
                
                friend std::ofstream & operator<<(std::ofstream &ofs, ID & id);

            public:
                std::string name;   // key to name
                int         pt;

            private:
                

        };

    public:
        Char(Logger* log);
        Char(Logger* log, uint32 v);
        ~Char();

    public:
        inline void SetVal(uint32 v) { val = v; }
        inline void SetCharLength(uint32 len) { charLen = len; }
        inline void SetPos(const Position & p){ pos = p; }
        inline void SetBitmap(int bw, int bh, void* b){
            bitmap_w = bw;
            bitmap_h = bh;
            bitmap   = b;
        }
        inline void SetBaseline(int b){ baseline = b; }
        inline void SetID(ID cid){ id = cid; }
        inline void SetSize(int s){ id.pt = s; }
        inline void SetFont(const char* n) { id.name = std::string(n); }
        inline void SetAttrib(Attrib_Glyph & attr){ attrib = attr; }

        inline void* GetBitmap() { return bitmap; }
        inline Attrib_Glyph GetAttrib(Attrib_Glyph & attr){ return attrib; }
        inline ENCODING_MODE GetEncoding(){ return encodeMode; }
        inline bool operator==(char ch){ return (char)val == ch; }
        inline bool operator!=(char ch){ return (char)val != ch; }
        inline bool operator==(Char& ch){ return val == ch.val; }

    public:
        unsigned int GetVal(ENCODING_MODE em = EM_UTF_8);
        bool Draw(RenderMan&);
        bool Relocate(int, int);
        bool Adjust2Baseline(int baseline);
        GY_ST_RET Setup(LayoutManager& lo);
        Glyph* UngetSet();
        Glyph* Dup();

    public:
        void Serialize(std::ofstream & ofs);
        void Deserialize(std::ifstream & ifs);
        uint32 GetMagic(){ return magic_num; }

    public:
        int             baseline;
        ENCODING_MODE   encodeMode;
        uint32          val;
        uint32          charLen;
        ID              id;
        bool            valid;      // If the char still need to exist in mem
        Attrib_Glyph    attrib;

    private:
        static FontsCache ftCache;
        static FontsManager ftMgr;

    private:
        static uint32 magic_num;
};

class Graph: public Glyph{
    public:
        Graph(Logger* log);
        ~Graph();

    public:
        typedef enum IMAGE_FILE_TYPE{
            IF_NONE,
            IF_PNG,
            IF_JPG,
            IF_GIF,
            IF_EMF,
        }IF_T;

    public:
        bool Draw(RenderMan&);
        bool Relocate(int, int);
        bool Adjust2Baseline(int baseline){  return true; }
        GY_ST_RET Setup(LayoutManager& lo);
        Glyph* Dup();
        Glyph* UngetSet();

    public:
        inline void SetReqWidth(uint32 w) { req_width = w; }
        inline void SetReqHeight(uint32 h) { req_height = h; }
        void SetSrcFile(const char* src);

    public:
        void Serialize(std::ofstream & ofs);
        void Deserialize(std::ifstream & ifs);
        uint32 GetMagic(){ return magic_num; }

    protected:
        GY_ST_RET SetupPNG(LayoutManager& lo, FILE* fp);
        GY_ST_RET SetupJPG(LayoutManager& lo, FILE* fp);
        GY_ST_RET SetupGIF(LayoutManager& lo, FILE* fp);
        IF_T DetectFormat(const char*, FILE * fp);
        void Convert(void** bmap, int w, int h, uchar8 col_t, uchar8 b_depth, int channel);
        void ConvertJPG(void* bmap, int w, int h);
        void* Resize(void* bmap, int32 w_old, int32 h_old, int32 & w_new, int32 & h_new);
        void* ResizeImpl(void* bmap, int32 w_old, int32 h_old, int32 w_new, int32 h_new);

    public:
        uint32  req_width;
        uint32  req_height;

    public:
        char* file_name;
        char* file_path;

    private:
        static uint32 magic_num;
};


//} // namespace heigong

#endif

