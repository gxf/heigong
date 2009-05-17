
#ifndef DOC_PARSER_H
#define DOC_PARSER_H

class Logger;
class Glyph;

class DocParser{
    public:
        DocParser(Logger* log);
        ~DocParser();

    public:
        bool OpenFile(const char* filename);
        void CloseFile();
        bool ReOpenFile();

        DocParser & operator>>(char & ch);
        DocParser & operator>>(Glyph & glyph);

        bool operator!();

    private:
        static const char*  tmpfile;
        std::fstream        file;

    private:
        Logger* logger;
        
};

#endif
