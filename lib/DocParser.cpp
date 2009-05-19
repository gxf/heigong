#include "Common.h"
#include "Logger.h"
#include "DocParser.h"
#include "Glyph.h"
#include <cstring>
#include <fstream>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>

const char* DocParser::tmpfile = "doc.tmp";

DocParser::DocParser(Logger* log):
    logger(log)
{
}

DocParser::~DocParser(){
}

static void AdjustCmd(char* cmd, int length){
    char* p = cmd;
    char* q = NULL;

    int i;
    for (i = 0; i < length; i++, p++){
        switch(*p){
            case ' ':
            case '(':
            case ')':
                for (q = p + (length + 1 - i); q > p; q--){
                    *q = *(q - 1) ;
                }
                length++;
                *p = '\\';
                p++;
                break;
            default:
                break;
        }
    }
}

bool DocParser::OpenFile(const char* filename){
#if 0
    pid_t pid;

    if ((pid = fork()) < 0){
        LOG_ERROR("fork error.");
        exit(0);
    }
    else if (0 == pid){
        int strlength   = std::strlen(filename) + 1;
        strlength       += std::strlen(tmpfile);
        strlength		+= 256; 

        char cmd[strlength];
        std::memset(cmd, 0x0, strlength);
//      sprintf(cmd, "./catdoc -w %s >%s", filename, tmpfile);
        sprintf(cmd, "./catdoc -m0 %s >%s", filename, tmpfile);
//        int fd = open(filename, O_RDWR | O_TRUNC);
//        dup2(1, fd);
        LOG_EVENT(cmd);
        if (execlp("catdoc", "-w", filename, (char*)0) < 0){
            LOG_ERROR("fail to run catdoc.");
        }
        exit(0);
//        system(cmd);
    }
    if (waitpid(pid, NULL, 0) < 0){
        LOG_ERROR("waitpid fails.");
    }
#endif

    char filen[std::strlen(filename) + 200];
    std::memset(filen, '\0', std::strlen(filename) + 200);
    std::memcpy(filen, filename, std::strlen(filename));
    AdjustCmd(filen, std::strlen(filename));

    int strlength   = std::strlen(filename) + 1;
    strlength       += std::strlen(tmpfile);
    strlength		+= 256; 

    char cmd[strlength];
    std::memset(cmd, 0x0, strlength);
    sprintf(cmd, "./catdoc -w %s >%s", filen, tmpfile);
    LOG_EVENT(cmd);

    system(cmd);

    file.open(tmpfile);
    if(!file){
        LOG_ERROR("fail to open doc file.");
        return false;
    }
    file.unsetf(std::ios::skipws);
    return true;
}

bool DocParser::ReOpenFile(){
    CloseFile();

    file.open(tmpfile);
    if(!file){
        LOG_ERROR("fail to open doc file.");
        return false;
    }
    file.unsetf(std::ios::skipws);
    return true;
}

void DocParser::CloseFile()
{
    file.close();
}

DocParser & DocParser::operator>>(unsigned char & ch){
    file >> ch;
    return *this;
}

DocParser & DocParser::operator>>(Char & ch){
    union VALUE{
        unsigned int all;
        struct{
            unsigned char byte1;
            unsigned char byte2;
            unsigned char byte3;
            unsigned char byte4;
        }f;
    }val;
    val.all = 0;
    unsigned char c_val = 0;

    switch(ch.GetEncoding()){
        case Char::EM_ASCII:
            file >> c_val;
            ch.SetVal(c_val);
            break;
        case Char::EM_UTF_8:
            file >> c_val;
            val.f.byte1 = c_val;
            if (c_val >= 0x00 && c_val <= 0x7F){
            }
//            else if (c_val >= 0xC2 && c_val =< 0xDF){
            else if (c_val >= 0xC0 && c_val <= 0xDF){
                file >> val.f.byte2;
            }
            else if (c_val >= 0xE0 && c_val <= 0xEF){
                file >> val.f.byte2;
                file >> val.f.byte3;
            }
            else if (c_val >= 0xF0 && c_val <= 0xF4){
                file >> val.f.byte2;
                file >> val.f.byte3;
                file >> val.f.byte4;
            }
            else{
                LOG_ERROR("Unsupported UTF-8 encoding!");
            }
            ch.SetVal(val.all);
            break;
        default:
            LOG_ERROR("Unsupported Encoding format!");
            break;
    }
    
    return *this;
}

DocParser & DocParser::operator>>(Glyph & glyph){
    return *this;
}

bool DocParser::operator!(){
    return !file;
}
