#ifndef DOC_PARSER_H
#define DOC_PARSER_H

#include "Common.h"
#include "DocStream.h"
#include <deque>
#include <stdio.h>

class Logger;
class Glyph;
class Char;
class Line;
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

        typedef struct DocState_Rec{
            std::deque<Glyph*>  buffer;
            long int            offset;
        }DocState, *HDocState;

    public:
        bool Init(const char* filen);
        DP_RET_T GetNextGlyph(Glyph** g, Line* line);
        void SetCurParseOffset(long int offset);

        HDocState ShadowDocState();
        bool RestoreDocState(HDocState hState);

    private:
        void ClearGlyphStream();
        
    private:
        bool match(char ch);
        bool match(const char* chs);
        bool match_b(const char* chs);

    private:
        void fillGlyphStream(Line * line);
        void getStyle(int & ch);
        void getImageAttrib(int & ch, Image & img);
        void skipBlanks(int & ch);
        void procLabel(int & ch);
        void procWord(int & ch);

    private:
        long int getInteger();
        double getFloat(int term);
        char* getString(int term);

    private:
        std::deque<Glyph*>  glyphBuffer;
        DocStream           docStream;

    private:
        Attrib_Glyph        glyphAttrib;
        Attrib_Line         lineAttrib;

    private:
        Logger*             logger;
};

#endif

