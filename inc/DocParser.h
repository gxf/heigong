#ifndef DOC_PARSER_H
#define DOC_PARSER_H

#include "Common.h"
#include "DocStream.h"
#include <queue>
#include <stdio.h>

class Logger;
class Glyph;
class Char;
class Image;

class DocParser{
    public:
        DocParser(Logger* log);
        ~DocParser();

    public:
        typedef enum DOC_PARSER_RETURN_TYPE{
            DP_OK,
            DP_EOF,
            DP_INVALID,     // Current stream is invalid
            DP_ERROR,       // Internal parse error
        }DP_RET_T;

    public:
        bool Init(const char* filen);
        DP_RET_T GetNextGlyph(Glyph* g);
        void SetCurParseOffset(long int offset);

    private:
        void ClearGlyphStream();
        
    private:
        bool match(char ch);
        bool match(const char* chs);
        bool match_b(const char* chs);

    private:
        void fillGlyphStream();
        void getImageAttrib(int & ch, Image & img);
        void skipBlanks(int & ch);
        void procLabel(int & ch);
        void procWord(int & ch);

    private:
        long int getInteger();
        char* getString();

    private:
        static const char*  tmpfile;

    private:
        std::queue<Glyph*>  glyphStream;
        DocStream           docStream;
        Logger*             logger;
};

#endif

