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

bool DocStream::OpenFile(const char* filename, bool background){
    bgMode = background;

    char filen[std::strlen(filename) + 200];
    std::memset(filen, '\0', std::strlen(filename) + 200);
    std::memcpy(filen, filename, std::strlen(filename));
    AdjustCmd(filen, std::strlen(filename));

    int strlength   = std::strlen(filename) + 1;
    strlength       += std::strlen(tmpFile);
    strlength       += 256; 

    char cmd[strlength];
    std::memset(cmd, 0x0, strlength);
    if (false == bgMode){
        sprintf(cmd, "./wvWare -x ./wvHtml.xml -d ./wvTmp -b wvImage %s > %s", filen, tmpFile);
        LOG_EVENT(cmd); 
        system(cmd); 
        return OpenFileDirect(tmpFile);
    }
    else{
        sprintf(cmd, "./wvWare -x ./wvHtml.xml -d ./wvTmp -b wvImage %s | tee %s", filen, tmpFile);
        LOG_EVENT(cmd);

        // Open pipe for processing
        if (!(bg_pipe_fd = popen(cmd, "r"))){
            LOG_ERROR("Fail to open pipe.");
            return false;
        }
        if (!(bg_file_fd = fopen(tmpFile, "r"))){
            LOG_ERROR("Fail to open tmp file.");
            return false;
        }
        fd = bg_pipe_fd;

        fileEnds = false;

        return true;
    }
}

// Deprecated
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


bool DocStream::CloseFile()
{
    if (true == bgMode){
        if(-1 == pclose(bg_pipe_fd)){
            LOG_ERROR("Fail to close pipe.");
            return false;
        }
        if(-1 == fclose(bg_file_fd)){
            LOG_ERROR("Fail to close file.");
            return false;
        }
        bg_pipe_fd = NULL;
        bg_file_fd = NULL;
    }
    else{
        fclose(fd);
    }

    fd = NULL;
    char cmd[100];
    sprintf(cmd, "rm -f %s", tmpFile);
    LOG_EVENT(cmd);

//    system(cmd);
    sprintf(cmd, "rm -f *.png *.jpg *.emf *.wmf *.pg");
    LOG_EVENT(cmd);
    system(cmd);
    return true;
}

DocStream & DocStream::operator>>(int & ch){
    ch = GetChar();

    if (EOF == ch){
        fileEnds = true;
        // TODO: recover stream
        throw Except_EOF();
    }
    return *this;
}

DocStream& DocStream::operator<<(int &ch){
    UnGetChar(ch);
    return *this;
}

DocStream& DocStream::operator<<(const char *str){
    int length = std::strlen(str);
#if 0
    int i = 0;
    while(i < length){
        UnGetChar((int)str[i]);
        i++;
    }
#endif
    while(--length >= 0){
        UnGetChar((int)str[length]);
    }
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
            c_val = GetChar();
            if(EOF == c_val){
                fileEnds = true;
                throw Except_EOF();
            }
            ch.SetVal(c_val);
            break;
        case EM_UTF_8:
            c_val = GetChar();
            if(EOF == c_val){
                fileEnds = true;
                // TODO: recover stream
                throw Except_EOF();
            }
            val.f.byte1 = (uchar8)c_val;
            if (/*val.f.byte1 >= 0x00 &&*/ val.f.byte1 <= 0x7F){
                ch.SetCharLength(1);
            }
            else if (val.f.byte1 >= 0xC0 && val.f.byte1 <= 0xDF){
                val.f.byte2 = GetChar();
                ch.SetCharLength(2);
            }
            else if (val.f.byte1 >= 0xE0 && val.f.byte1 <= 0xEF){
                val.f.byte2 = GetChar();
                val.f.byte3 = GetChar();
                ch.SetCharLength(3);
            }
            else if (val.f.byte1 >= 0xF0 && val.f.byte1 <= 0xF4){
                val.f.byte2 = GetChar();
                val.f.byte3 = GetChar();
                val.f.byte4 = GetChar();
                ch.SetCharLength(4);
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
            UnGetChar(val.f.byte1);
            fileEnds = false;
            break;
        case EM_UTF_8:
            if (val.f.byte1 <= 0x7F){
                UnGetChar(val.f.byte1);
            }
            else if (val.f.byte1 >= 0xC0 && val.f.byte1 <= 0xDF){
                UnGetChar(val.f.byte2);
                UnGetChar(val.f.byte1);
            }
            else if (val.f.byte1 >= 0xE0 && val.f.byte1 <= 0xEF){
                UnGetChar(val.f.byte3);
                UnGetChar(val.f.byte2);
                UnGetChar(val.f.byte1);
            }
            else if (val.f.byte1 >= 0xF0 && val.f.byte1 <= 0xF4){
                UnGetChar(val.f.byte4);
                UnGetChar(val.f.byte3);
                UnGetChar(val.f.byte2);
                UnGetChar(val.f.byte1);
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
    if (false == bgMode){
        if (fd){
            if (-1 == fseek(fd, off, SEEK_SET)){
                LOG_ERROR("fail to seek offset in file.");
                printf("offset: %ld\n", off);
            }
        }
        else{
            LOG_ERROR("File descriptor is invalid.");
        }
    }
    else if (true == bgMode && off > file_off){
        // Background parsing: forward searching synchronization is 
        // dealt by pipe mechanism, this really should not happen
        LOG_ERROR("Fail to set file offset.");
//        assert(false);
        return;
    }
    else if (true == bgMode && off <= file_off){
        // Switch to file descriptor
        fd = bg_file_fd;
        if (-1 == fseek(fd, off, SEEK_SET)){
            LOG_ERROR("fail to seek offset in file.");
            printf("offset: %ld\n", off);
        }
        // Record back ground parsing offset here
        bg_off = file_off;
    }
    // Empty the UnGetChar buffer
    while(false == ch_buf.empty()){
        ch_buf.pop();
    }
    file_off = off;
}

long int DocStream::GetCurOffset(){ 
    // return the stream position but ingore unputc(s) in stream
//    printf("ftell: %ld, file_off: %ld", ftell(fd), file_off);
//    return ftell(fd) - ch_buf.size();

    return file_off - ch_buf.size();// - comps;
}

uint8 DocStream::GetChar(){
    uint8 ret;
    if (false == ch_buf.empty()){
        ret = ch_buf.top();
        ch_buf.pop();
        return ret;
    }
    else{
        if ((true == bgMode) && (fd == bg_file_fd) && (file_off >= bg_off)){
            // Switch descriptor to pipe to avoid reading ahead of generating
            fd = bg_pipe_fd;
        }
        int32 val = getc(fd);
        if (EOF == val || val < 0){
            fileEnds = true;
            if (true == bgMode){
                bgMode = false;
            }
            throw Except_EOF();
        }
        else{
            ret = (uint8)val;
            ++file_off;
            return ret;
        }
    }
}

void DocStream::UnGetChar(uint8 ch){
    ch_buf.push(ch);
}

