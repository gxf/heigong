
#ifndef DOC_PARSER_H
#define DOC_PARSER_H

class Logger;

class DocParser{
    public:
        DocParser(Logger* log);
        ~DocParser();

    public:
        bool OpenFile(const char* filename);
        void CloseFile();

        DocParser & operator>>(char & ch);
        bool operator!();

    private:
        static const char*  tmpfile;
        std::fstream        file;

    private:
        Logger* logger;
        
};

#endif
