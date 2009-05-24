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
    offset(0), fd(NULL), fileEnds(true),
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
    if(!(fd = fopen(tmpfile, "r"))){
        LOG_ERROR("fail to open doc file.");
        return false;
    }
    fileEnds = false;
    return true;
}

bool DocParser::ReOpenFile(){
    CloseFile();
    if(!(fd = fopen(tmpfile, "r"))){
        LOG_ERROR("fail to open doc file.");
        return false;
    }
    fileEnds = false;
    return true;
}


void DocParser::CloseFile()
{
    fclose(fd);
    fd = NULL;
}

DocParser & DocParser::operator>>(uchar8 & ch){
    // deprecated
    return *this;
}

DocParser & DocParser::operator>>(Char & ch){
    union VALUE{
        unsigned int all;
        struct{
            uchar8 byte1;
            uchar8 byte2;
            uchar8 byte3;
            uchar8 byte4;
        }f;
    }val;
    val.all = 0;
    int c_val = 0;

    switch(ch.GetEncoding()){
        case EM_ASCII:
            c_val = getc(fd);
            if(EOF == c_val){
                fileEnds = true;
            }
            ch.SetVal(c_val);
            offset++;
            break;
        case EM_UTF_8:
            c_val = getc(fd);
            if(EOF == c_val){
                fileEnds = true;
            }
//            val.f.byte1 = reinterpret_cast<uchar8>(c_val);
            val.f.byte1 = (uchar8)c_val;
            if (/*val.f.byte1 >= 0x00 &&*/ val.f.byte1 <= 0x7F){
                ch.SetCharLength(1);
                offset++;
            }
            else if (val.f.byte1 >= 0xC0 && val.f.byte1 <= 0xDF){
                val.f.byte2 = getc(fd);
                ch.SetCharLength(2);
                offset += 2;
            }
            else if (val.f.byte1 >= 0xE0 && val.f.byte1 <= 0xEF){
                val.f.byte2 = getc(fd);
                val.f.byte3 = getc(fd);
                ch.SetCharLength(3);
                offset += 3;
            }
            else if (val.f.byte1 >= 0xF0 && val.f.byte1 <= 0xF4){
                val.f.byte2 = getc(fd);
                val.f.byte3 = getc(fd);
                val.f.byte4 = getc(fd);
                ch.SetCharLength(4);
                offset += 4;
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

DocParser & DocParser::operator<<(Char & ch){
    union VALUE{
        unsigned int all;
        struct{
            uchar8 byte1;
            uchar8 byte2;
            uchar8 byte3;
            uchar8 byte4;
        }f;
    }val;
    val.all = ch.GetVal(ch.GetEncoding());

    switch(ch.GetEncoding()){
        case EM_ASCII:
            ungetc(val.f.byte1, fd);
            offset--;
            fileEnds = false;
            break;
        case EM_UTF_8:
            if (val.f.byte1 <= 0x7F){
                ungetc(val.f.byte1, fd);
                offset--;
            }
            else if (val.f.byte1 >= 0xC0 && val.f.byte1 <= 0xDF){
                ungetc(val.f.byte2, fd);
                ungetc(val.f.byte1, fd);
                offset -= 2;
            }
            else if (val.f.byte1 >= 0xE0 && val.f.byte1 <= 0xEF){
                ungetc(val.f.byte3, fd);
                ungetc(val.f.byte2, fd);
                ungetc(val.f.byte1, fd);
                offset -= 3;
            }
            else if (val.f.byte1 >= 0xF0 && val.f.byte1 <= 0xF4){
                ungetc(val.f.byte4, fd);
                ungetc(val.f.byte3, fd);
                ungetc(val.f.byte2, fd);
                ungetc(val.f.byte1, fd);
                offset -= 4;
            }
            else{
                LOG_ERROR("Unsupported UTF-8 encoding!");
            }
            fileEnds = false;
            break;
        default:
            LOG_ERROR("Unsupported Encoding format!");
            break;
    }
    
    return *this;
}

void DocParser::SetOffset(long int offset){
    if (fd){
        fseek(fd, offset, SEEK_SET);
    }
    else{
        LOG_ERROR("File descriptor is invalid.");
    }
}

DocParser & DocParser::operator>>(Glyph & glyph){
    return *this;
}

bool DocParser::operator!(){
    return fileEnds;
}
