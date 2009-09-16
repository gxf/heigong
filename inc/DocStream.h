#ifndef DOC_STREAM_H
#define DOC_STREAM_H

#include "Common.h"
#include <stdio.h>
#include <cstring>
#include <stack>

class Char;
class Logger;

class DocStream{
    public:
        DocStream(Logger * log, const char * tmpfilen = DEFAULT_WORK_DIR DEFAULT_TMP_FILE_NAME):
            tmpFile(NULL), fd(NULL), fileEnds(false), 
            bg_pipe_fd(NULL), bg_file_fd(NULL), bgMode(false),
            file_off(0), bg_off(0), logger(log)
        {
            if (false == fast_page_sum){
                tmpFile = new char8[std::strlen(tmpfilen) + 1];
                std::strcpy(tmpFile, tmpfilen); 
            }
            else{
                size_t sz = std::strlen(work_dir) + std::strlen(DEFAULT_FAST_PAGE_TMP_FILE) + 1;
                tmpFile = new char8[sz];
                std::memset(tmpFile, 0x0, sz);
                std::strcpy(tmpFile, work_dir);
                std::strcat(tmpFile, DEFAULT_FAST_PAGE_TMP_FILE);
            }
        }
        ~DocStream(){
            if (tmpFile){
                delete [] tmpFile;
                tmpFile = NULL;
            }
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
        DocStream & operator<<(const char* str);

        inline bool operator!(){ return (fd) ? false : true; }

    public:
        long int GetCurOffset();
        void SetOffset(long int offset);
        void SetTermOff(long int offset){ term_off = offset;}

    private:
        uint8 GetChar();
        void UnGetChar(uint8 ch);
        void AdjustCmd(char*cmd, int length);

    private:
        char*   tmpFile;
        FILE*   fd;
        bool                fileEnds;

        // Background mode related
    private:
        // pipe file descriptor for backgound mode
        FILE*               bg_pipe_fd;      
        // tmp file descriptor for backgound mode
        FILE*               bg_file_fd;      
        // Background parsing mode
        bool                bgMode;     


    private:
        long int            file_off;   // Current file offset
        long int            term_off;   // Terminate pos, pg based render only
        long int            bg_off;     // Current background parsing offset

    private:
        std::stack<uint8>   ch_buf;
        Logger*             logger;
};

#endif
