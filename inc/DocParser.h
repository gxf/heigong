
#ifndef DOC_PARSER_H
#define DOC_PARSER_H

#include "Common.h"

class Logger;
class Glyph;
class Char;

class DocParser{
    public:
        DocParser(Logger* log);
        ~DocParser();

    public:
        bool OpenFile(const char* filename);
        void CloseFile();
        bool ReOpenFile();

        DocParser & operator>>(uchar8 & ch);
        DocParser & operator>>(Char & ch);
        DocParser & operator<<(Char & ch);
        DocParser & operator>>(Glyph & glyph);

        bool operator!();

    public:
        inline uint32 GetCurOffset(){ return offset; }
        void SetOffset(long int offset);

    private:
        long int            offset;     // Current file offset

    private:
        static const char*  tmpfile;
        FILE*               fd;
        bool                fileEnds;

    private:
        Logger* logger;
        
};

#endif
