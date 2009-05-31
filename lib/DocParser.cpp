#include "Common.h"
#include "Logger.h"
#include "DocParser.h"
#include "Glyph.h"
#include <cstring>
#include <cstdlib>
#include <fstream>
#include <string>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>

DocParser::DocParser(Logger* log):
    docStream(log), 
    logger(log)
{}

DocParser::~DocParser(){}

bool DocParser::Init(const char* filen){
    return docStream.OpenFile(filen);
}
void DocParser::SetCurParseOffset(long int offset){
    ClearGlyphStream();
    docStream.SetOffset(offset);
}

void DocParser::ClearGlyphStream(){
    while(!(glyphBuffer.empty())){
        delete glyphBuffer.front();
        glyphBuffer.pop_front();
    }
}

DocParser::DP_RET_T DocParser::GetNextGlyph(Glyph* glyph){
    // Retrieve first when glyphBuffer is not empty
    if (!(glyphBuffer.empty())){
        glyph = glyphBuffer.front();
        glyphBuffer.pop_front();
        return DP_OK;
    }

    // Else, fill glyph stream with new content
    if (!docStream){
        glyph = NULL;
        return DP_INVALID;  // Defensive code: this condition should never be met
    }

    try{
        fillGlyphStream();
    }
    catch(Except_EOF &){
        if (!(glyphBuffer.empty())){
            glyph = glyphBuffer.front();
            glyphBuffer.pop_front();
            return DP_OK;
        }
        else{
            glyph = NULL;
            return DP_EOF;
        }
    }

    if (glyphBuffer.empty()){
        glyph = NULL;
        return DP_ERROR;    // Defensive code: this condition should never be met
    }
    else{
        glyph = glyphBuffer.front();
        glyphBuffer.pop_front();
        return DP_OK;
    }
}

DocParser::HDocState DocParser::ShadowDocState(){
    // Shadow docStream offset and glyphBuffer
    return (HDocState)NULL;
}

bool DocParser::RestoreDocState(DocParser::HDocState hState){
    // Restore the docStream offset and glyphBuffer

    return true;
}

void DocParser::fillGlyphStream(){
    int ch;
    docStream >> ch;
    skipBlanks(ch);

    while(ch == '<'){
        procLabel(ch);
        skipBlanks(ch);     // EOF exception may throw
        docStream >> ch;
        skipBlanks(ch);
    }

    // Else, proc the word content. 
    procWord(ch);
}

void DocParser::procLabel(int & ch){
    // Search label for attribute, image, hyperlink
    docStream >> ch;
    // Only process certain labels
    switch(ch){
        case 'i':
            if(match("mg")){
                Image* pImg = new Image(logger);
                getImageAttrib(ch, *pImg);
                glyphBuffer.push_back(pImg);
            }
            else{
                // ignore all left labels start with 'i'
                while('>' != ch){ docStream >> ch; }
            }
            break;
        case 'b':
            if(match("r")){
                // interpret <br> to new line
                Char* c = new Char(logger);
                c->SetVal('\n');
                glyphBuffer.push_back(c);
            }
            // ignore all left labels start with 'b'
            while('>' != ch){ docStream >> ch; }
            break;
        case 'p':
            if(match(">")){
                Char* c = new Char(logger);
                c->SetVal('\n');
                glyphBuffer.push_back(c);
            }
            else{
                // TODO: <p ...>
            }
            // ignore all left labels start with 'p'
            while('>' != ch){ docStream >> ch; }
            break;
        case 'd':
            if(match("iv")){
                // TODO: <div ...>
            }
            // ignore all left labels start with 'd'
            while('>' != ch){ docStream >> ch; }
            break;
        default:
            break;
    }
}

void DocParser::procWord(int & ch){ 
    Char* c = new Char(logger);
    // But translate special tokens
    // <    == &lt
    // >    == &gt  
    // &    == &amp
    // "    == &quot
    // blank == &nbsp
    if (ch == '&'){
        if (match("lt")){ 
            c->SetVal('<'); 
        }
        else if (match("gt")){ 
            c->SetVal('>'); 
        }
        else if (match("amp")){ 
            c->SetVal('&'); 
        } 
        else if (match("quot")){ 
            c->SetVal('\"'); 
        }
        else if (match("nbsp")){ 
            c->SetVal(' '); 
        } 
        else{ 
            docStream << ch; 
            docStream >> *c;
        }
        glyphBuffer.push_back(c);
    }
    else{
        docStream << ch; 
        docStream >> *c;
        glyphBuffer.push_back(c);
    }
}

void DocParser::getImageAttrib(int & ch, Image & img){
    skipBlanks(ch);

    long int width   = 0;
    long int height  = 0;
    char* src;
    
    while('>' != ch){
        docStream >> ch;
        switch(ch){
            case 'w':
                if (match("idth")){
                    skipBlanks(ch);
                    if(!match("=")) throw Except_Parse_Err();
                    skipBlanks(ch);
                    width = getInteger();
                }
                // Skip other labels
                break;
            case 'h':
                if (match("eight")){
                    skipBlanks(ch);
                    if(!match("=")) throw Except_Parse_Err();
                    skipBlanks(ch);
                    height = getInteger();
                }
                // Skip other labels
                break;
            case 's':
                if (match("rc")){
                    skipBlanks(ch);
                    if(!match("=")) throw Except_Parse_Err();
                    skipBlanks(ch);
                    src = getString();  // Note: User's resposibility to release string
                }
                // Skip other labels
                break;
            default:
                break;
        }
    }
}

bool DocParser::match(char ch){
    int c;
    docStream >> c;
    if (c == ch){
        return true;
    }
    docStream << c;
    return false;
}

bool DocParser::match(const char* ch){
    int c;
    int i = 0;

    while ('\0' == ch[i]){
        docStream >> c;
        if (c != (int)ch[i]){
            docStream << c;
            while(--i >= 0){
                int tmp = (int)ch[i];
                docStream << tmp;
            }
            return false;
        }
        i++;
    }
    return true;
}

bool DocParser::match_b(const char* ch){
    int c;
    int i = 0;
    // Skip blank spaces
    while(' ' == ch[i] || '\t' == ch[i] || '\n' == ch[i]){
        i++;
    }

    int beg = i;
    while ('\0' == ch[i]){
        docStream >> c;
        if (c != ch[i]){
            docStream << c;
            while(--i >= beg){
                int tmp = (int)ch[i];
                docStream << tmp;
            }
            return false;
        }
        i++;
    }
    return true;
}

void DocParser::skipBlanks(int & ch){
    // Skip blank spaces
    while(' ' == ch || '\t' == ch || '\n' == ch){
        docStream >> ch;
    }
}

long int DocParser::getInteger(){
    std::string str;
    int ch;
    int term = ' ';
    docStream >> ch;
    if ('\"' == ch){ 
        term = '\"';
        docStream >> ch;
    }
    do{
        str += (char)ch;
        docStream >> ch;
    }
    while(term != ch);

    return std::strtol(str.c_str(), '\0', 10);
}

char* DocParser::getString(){
    std::string str;
    int ch;
    int term = ' ';
    docStream >> ch;
    if ('\"' == ch){ 
        term = '\"';
        docStream >> ch;
    }
    do{
        str += (char)ch;
        docStream >> ch;
    }
    while(term != ch);

    // Note: User's resposibility to release string
    char* cstr = new char[str.size() + 1];
    std::strcpy(cstr, str.c_str());
    return cstr;
}

