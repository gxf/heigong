#include "Common.h"
#include "Logger.h"
#include "DocParser.h"
#include "Glyph.h"
#include <cstring>
#include <fstream>

const char* DocParser::tmpfile = "doc.tmp";

DocParser::DocParser(Logger* log):
    logger(log)
{
}

DocParser::~DocParser(){
}

bool DocParser::OpenFile(const char* filename){
    int strlength   = std::strlen(filename) + 1;
    strlength       += std::strlen(tmpfile);

    char cmd[strlength + 200];
//    sprintf(cmd, "./catdoc -w %s >%s", filename, tmpfile);
    sprintf(cmd, "./catdoc -m0 %s >%s", filename, tmpfile);
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

DocParser & DocParser::operator>>(char & ch){
    file >> ch;
    return *this;
}

DocParser & DocParser::operator>>(Glyph & glyph){
    return *this;
}

bool DocParser::operator!(){
    return !file;
}
