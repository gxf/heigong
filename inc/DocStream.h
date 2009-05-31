#ifndef DOC_STREAM_H
#define DOC_STREAM_H

#include "Common.h"

class Char;
class Logger;

class DocStream{
    public:
        DocStream(Logger * log, const char * tmpfilen = DEFAULT_TMP_FILE_NAME):
            tmpFile(tmpfilen), fd(NULL), fileEnds(false),
            offset(0), logger(log)
        {}
        ~DocStream(){
            if(fd){ CloseFile(); }
        }

    public:
        bool OpenFile(const char* filename);
        void CloseFile();
        bool ReOpenFile();
    
    public:
        DocStream & operator>>(int & ch);
        DocStream & operator<<(int & ch);

        DocStream & operator>>(Char & ch);
        DocStream & operator<<(Char & ch);

        inline bool operator!(){ return (fd) ? false : true; }

    public:
        inline uint32 GetCurOffset(){ return offset; }
        void SetOffset(long int offset);

    private:
        void AdjustCmd(char*cmd, int length);

    private:
        const char*         tmpFile;
        FILE*               fd;
        bool                fileEnds;

    private:
        long int            offset;     // Current file offset

    private:
        Logger*             logger;
};

#endif
