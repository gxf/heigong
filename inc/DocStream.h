#ifndef DOC_STREAM_H
#define DOC_STREAM_H

#include "Common.h"
#include <stdio.h>

class Char;
class Logger;

class DocStream{
    public:
        DocStream(Logger * log, const char * tmpfilen = DEFAULT_TMP_FILE_NAME):
            tmpFile(tmpfilen), fd(NULL), bgMode(false),
            fileEnds(false), offset(0), logger(log)
        {}
        ~DocStream(){
            if(fd){ CloseFile(); }
        }

    public:
        bool OpenFile(const char* filename, bool bkgrd);
        bool OpenFileDirect(const char* filename);
        bool CloseFile();

        bool ReOpenFile();  // Deprecated

    public:
        DocStream & operator>>(int & ch);
        DocStream & operator<<(int & ch);

        DocStream & operator>>(Char & ch);
        DocStream & operator<<(Char & ch);

        inline bool operator!(){ return (fd) ? false : true; }

    public:
        long int GetCurOffset();
        void SetOffset(long int offset);

    private:
        void AdjustCmd(char*cmd, int length);

    private:
        const char*         tmpFile;
        FILE*               fd;
        bool                bgMode;     // Background parsing mode
        bool                fileEnds;

    private:
        long int            offset;     // Current file offset

    private:
        Logger*             logger;
};

#endif
