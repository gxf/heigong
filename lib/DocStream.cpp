#include "Logger.h"
#include "DocStream.h"
#include "Glyph.h"
#include <cstring>
#include <fstream>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>

void DocStream::AdjustCmd(char* cmd, int length){
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

bool DocStream::OpenFileDirect(const char* filename){
    if(!(fd = fopen(filename, "r"))){
        LOG_ERROR("fail to open doc file.");
        return false;
    }
    fileEnds = false;
    return true;
}

bool DocStream::OpenFile(const char* filename){

    char filen[std::strlen(filename) + 200];
    std::memset(filen, '\0', std::strlen(filename) + 200);
    std::memcpy(filen, filename, std::strlen(filename));
    AdjustCmd(filen, std::strlen(filename));

    int strlength   = std::strlen(filename) + 1;
    strlength       += std::strlen(tmpFile);
    strlength		+= 256; 

    char cmd[strlength];
    std::memset(cmd, 0x0, strlength);
    sprintf(cmd, "./wvWare -x ./wvHtml.xml -d ./wvTmp -b wvImage %s > %s", filen, tmpFile);
    LOG_EVENT(cmd);

    system(cmd);

    return OpenFileDirect(tmpFile);
}

bool DocStream::ReOpenFile(){
    LOG_ERROR("ReOpenFile NOT PROPERLY IMPLEMENTED!!!");

    return false;

    CloseFile();
    if(!(fd = fopen(tmpFile, "r"))){
        LOG_ERROR("fail to open doc file.");
        return false;
    }
    fileEnds = false;
    return true;
}


void DocStream::CloseFile()
{
    fclose(fd);
    fd = NULL;
    char cmd[100];
    sprintf(cmd, "rm -f %s", tmpFile);
    LOG_EVENT(cmd);

//    system(cmd);
    sprintf(cmd, "rm -f *.png *.jpg *.emf *.wmf");
    LOG_EVENT(cmd);
    system(cmd);
}

DocStream & DocStream::operator>>(int & ch){
    ch = getc(fd);
    if (EOF == ch){
        fileEnds = true;
        // TODO: recover stream
        throw Except_EOF();
    }
    else{
        if (offset > 0){
            --offset;
        }
    }
    return *this;
}

DocStream& DocStream::operator<<(int &ch){
    ungetc(ch, fd);
    offset++;
    return *this;
}

DocStream & DocStream::operator>>(Char & ch){
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
                throw Except_EOF();
            }
            ch.SetVal(c_val);
            offset++;
            break;
        case EM_UTF_8:
            c_val = getc(fd);
            if(EOF == c_val){
                fileEnds = true;
                // TODO: recover stream
                throw Except_EOF();
            }
            val.f.byte1 = (uchar8)c_val;
            if (/*val.f.byte1 >= 0x00 &&*/ val.f.byte1 <= 0x7F){
                ch.SetCharLength(1);
                if (offset > 0){ --offset; }
            }
            else if (val.f.byte1 >= 0xC0 && val.f.byte1 <= 0xDF){
                val.f.byte2 = getc(fd);
                ch.SetCharLength(2);
                if (offset > 0){ offset = (offset - 2) ? (offset - 2) : 0; }
            }
            else if (val.f.byte1 >= 0xE0 && val.f.byte1 <= 0xEF){
                val.f.byte2 = getc(fd);
                val.f.byte3 = getc(fd);
                ch.SetCharLength(3);
                if (offset > 0){ offset = (offset - 3) ? (offset - 3) : 0; }
            }
            else if (val.f.byte1 >= 0xF0 && val.f.byte1 <= 0xF4){
                val.f.byte2 = getc(fd);
                val.f.byte3 = getc(fd);
                val.f.byte4 = getc(fd);
                ch.SetCharLength(4);
                if (offset > 0){ offset = (offset - 4) ? (offset - 4) : 0; }
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

DocStream & DocStream::operator<<(Char & ch){
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
            offset++;
            fileEnds = false;
            break;
        case EM_UTF_8:
            if (val.f.byte1 <= 0x7F){
                ungetc(val.f.byte1, fd);
                offset++;
            }
            else if (val.f.byte1 >= 0xC0 && val.f.byte1 <= 0xDF){
                ungetc(val.f.byte2, fd);
                ungetc(val.f.byte1, fd);
                offset += 2;
            }
            else if (val.f.byte1 >= 0xE0 && val.f.byte1 <= 0xEF){
                ungetc(val.f.byte3, fd);
                ungetc(val.f.byte2, fd);
                ungetc(val.f.byte1, fd);
                offset += 3;
            }
            else if (val.f.byte1 >= 0xF0 && val.f.byte1 <= 0xF4){
                ungetc(val.f.byte4, fd);
                ungetc(val.f.byte3, fd);
                ungetc(val.f.byte2, fd);
                ungetc(val.f.byte1, fd);
                offset += 3;
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

void DocStream::SetOffset(long int off){
    if (fd){
        if (-1 == fseek(fd, off, SEEK_SET)){
            LOG_ERROR("fail to seek offset in file");
        }
    }
    else{
        LOG_ERROR("File descriptor is invalid.");
    }
    offset = 0;
}

long int DocStream::GetCurOffset(){ 
    // return the stream position but ingore unputc(s) in stream
    return ftell(fd) - offset; 
}
