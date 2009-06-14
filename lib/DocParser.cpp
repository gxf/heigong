#include "Common.h"
#include "Logger.h"
#include "DocParser.h"
#include "Glyph.h"
//#include "Line.h"
#include "LayoutManager.h"
#include <stdlib.h>
#include <cstring>
#include <cstdlib>
#include <fstream>
#include <string>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>

DocParser::DocParser(Logger* log):
    listMode(LM_NONE), headerMode(false),
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
#if 0
    while(!(glyphBuffer.empty())){
        delete glyphBuffer.front();
        glyphBuffer.pop_front();
    }
#endif
    glyphBuffer.clear();
}

DocParser::DP_RET_T DocParser::GetNextGlyph(Glyph** glyph, LayoutManager * layout){
    // Retrieve first when glyphBuffer is not empty
    if (!(glyphBuffer.empty())){
        *glyph = glyphBuffer.front();
        glyphBuffer.pop_front();
        return DP_OK;
    }

    // Else, fill glyph stream with new content
    if (!docStream){
        *glyph = NULL;
        return DP_INVALID;  // Defensive code: this condition should never be met
    }

    try{
        fillGlyphStream(layout);
    }
    catch(Except_EOF &){
        if (!(glyphBuffer.empty())){
            *glyph = glyphBuffer.front();
            glyphBuffer.pop_front();
            return DP_OK;
        }
        else{
            *glyph = NULL;
            return DP_EOF;
        }
    }

    if (glyphBuffer.empty()){
        GetNextGlyph(glyph, layout);  // Header parsing may lead to this
        return DP_OK;
    }
    else{
        *glyph = glyphBuffer.front();
        glyphBuffer.pop_front();
        return DP_OK;
    }
}

DocParser::HDocState DocParser::ShadowDocState(){
    // Shadow docStream offset and glyphBuffer
    HDocState shadowState = new DocState;
    std::deque<Glyph*>::iterator itr= glyphBuffer.begin();
    while(itr != glyphBuffer.end()){
        shadowState->buffer.push_back((*itr)->Dup());
        ++itr;
    }
    shadowState->offset = docStream.GetCurOffset();
    return shadowState;
}

bool DocParser::RestoreDocState(DocParser::HDocState hState){
    // Restore the docStream offset and glyphBuffer
    ClearGlyphStream();
    std::deque<Glyph*>::iterator itr= hState->buffer.begin();
    while(itr != hState->buffer.end()){
        glyphBuffer.push_back((*itr)->Dup());
        ++itr;
    }
    docStream.SetOffset(hState->offset);

    return true;
}

void DocParser::fillGlyphStream(LayoutManager* layout){
    int ch;
    docStream >> ch;
    skipBlanks(ch);

    while(ch == '<'){
        if(false == procLabel(ch)){
            return;
        }
        docStream >> ch;    // EOF exception may throw
        skipBlanks(ch);
    }
    layout->curLine->SetAttrib(lineAttrib);

    // proc the word content. 
    while(!delayedToken.empty()){
        glyphBuffer.push_back(delayedToken.front());
        delayedToken.pop();
    }
    while(ch != '<'){
        procWord(ch);
        docStream >> ch;
    }
    if (ch == '<'){
        docStream << ch;
    }
}

bool DocParser::procLabel(int & ch){
    // Search label for attribute, image, hyperlink
    docStream >> ch;
    // Only process certain labels
    switch(ch){
        case 'b':
            if(match("r>")){
                // interpret <br> to new line
                Char* c = new Char(logger);
                c->SetVal('\n');
                glyphBuffer.push_back(c);
                return false;   // Return immediately for line processing
            }
            // ignore all left labels start with 'b'
            while('>' != ch){ docStream >> ch; }
            break;
        case 'd':
            if(match("iv")){
                // TODO: <div ...>
                Char* c = new Char(logger);
                c->SetVal('\n');
                glyphBuffer.push_back(c);
            }
            // ignore all left labels start with 'd'
            while('>' != ch){ docStream >> ch; }
            break;
        case 'i':
            if(match("mg")){
                Graph* pImg = new Graph(logger);
                getGraphAttrib(ch, *pImg);
                glyphBuffer.push_back(pImg);
            }
            else{
                // ignore all left labels start with 'i'
                while('>' != ch){ docStream >> ch; }
            }
            break;
        case 'l':
            if(match("i")){
                Char* label;
                switch(listMode){
                    case LM_ORDER:
                        if(match_b("value=")){
                            label = new Char(logger);
                            label->SetVal('0' + getInteger());
                            delayedToken.push(label);
                            label = new Char(logger);
                            label->SetVal('.');
                            delayedToken.push(label);
                            label = new Char(logger);
                            label->SetVal(' ');
                            delayedToken.push(label);
                        }
                        break;
                    case LM_UNORDER:
                        label = new Char(logger);
                        label->SetVal(' ');
                        delayedToken.push(label);
                        label = new Char(logger);
                        label->SetVal('*');
                        delayedToken.push(label);
                        label = new Char(logger);
                        label->SetVal(' ');
                        delayedToken.push(label);
                        break;
                    default:
                        break;
                }
            }
            while('>' != ch){ docStream >> ch; }
            break;
	case 'm':
	    if (match("yfont")){
                getMyFont(ch);
                break;
	    }
            while('>' != ch){ docStream >> ch; }
	    break;
        case 'o':
            if(match("l") && match_b(">")){
                listMode = LM_ORDER;
                break;
            }
            while('>' != ch){ docStream >> ch; }
            break;
        case 'p':
            if(match(">")){
                Char* c = new Char(logger);
                c->SetVal('\n');
                glyphBuffer.push_back(c);
                return false;   // Return immediately for line processing
            }
            else if (match_b("style") && match_b("=") && match("\"")){
                // <p style="..."> 
                getStyle(ch);
            }
            // ignore all left labels start with 'p'
            while('>' != ch){ docStream >> ch; }
            break;
        case 'u':
            if(match("l") && match_b(">")){
                listMode = LM_UNORDER;
                break;
            }
            while('>' != ch){ docStream >> ch; }
            break;
        case 't':
            if(match("itle>")){
                lineAttrib.align = A_CENTRAL;
                glyphAttrib.bold = true;
                headerMode       = true;
                break;
            }
            else if (match("able")){
                ParseTable(ch);
                break;
            }
            while('>' != ch){ docStream >> ch; }
            break;
        case '/':
            if(match("div>")){
                Char* c = new Char(logger);
                c->SetVal('\n');
                glyphBuffer.push_back(c);
                break;
            }
            if(match("title>")){
                lineAttrib.Reset();
                glyphAttrib.Reset();
                headerMode = false;
                break;
            }
            else if(match("ol>")){
                listMode = LM_NONE;
                break;
            }
            else if(match("ul>")){
                listMode = LM_NONE;
                break;
            }
            else if(match("myfont>")){
                glyphAttrib.Reset();
                break;
            }
            while('>' != ch){ docStream >> ch; }
            break;
        case '!':
            if(match("--")){
                while(!match_b("-->")){
                    docStream >> ch;
                }
                break;
            }
            // ignore all left labels start with '!'
            while('>' != ch){ docStream >> ch; }
            break;
        default:
            // ignore all other labels
            while('>' != ch){ docStream >> ch; }
            break;
    }
    return true;
}

void DocParser::procWord(int & ch){ 
    Char* c = new Char(logger);
    c->SetAttrib(glyphAttrib);
    // But translate special tokens
    // <    == &lt
    // >    == &gt  
    // &    == &amp
    // "    == &quot
    // blank == &nbsp
    // “    == &ldquo
    // ”    == &rdquo
    // －   == &mdash
#define FourBytes(a, b, c, d) (d << 24 | c << 16 | b << 8 | a)
    if (ch == '&'){
        if (match("lt;")){ 
            c->SetVal('<'); 
        }
        else if (match("gt;")){ 
            c->SetVal('>'); 
        }
        else if (match("amp;")){ 
            c->SetVal('&'); 
        } 
        else if (match("quot;")){ 
            c->SetVal('\"'); 
        }
        else if (match("nbsp;")){ 
            c->SetVal(' '); 
        } 
        else if (match("ldquo;")){ 
//            c->SetVal(FourBytes(0xe2, 0x80, 0x9c, 0x00)); 
            c->SetVal('\"');    // Work around
        } 
        else if (match("rdquo;")){ 
//            c->SetVal(FourBytes(0xe2, 0x80, 0x9d, 0x00)); 
            c->SetVal('\"');    // Work around
        } 

        else if (match("mdash;")){ 
//            c->SetVal(FourBytes(0xef, 0xbc, 0x8d, 0x00)); 
            c->SetVal('-');    // Work around
        } 
        else{ 
            docStream << ch; 
            docStream >> *c;
        }
        if(!headerMode){
            glyphBuffer.push_back(c);
        }
    }
    else{
        docStream << ch; 
        docStream >> *c;
        if(!headerMode){
            glyphBuffer.push_back(c);
        }
    }
}
void DocParser::getMyFont(int &ch){
    skipBlanks(ch);

    while('>' != ch){
        docStream >> ch;
        switch(ch){
            case 'n':
                if (match("ame") && match_b("=")){
                    // TODO: FONT translate
                    glyphAttrib.font = NULL;
                }
                break;
            case 's':
                if (match("ize") && match_b("=")){
                    glyphAttrib.size = getInteger() / 2;
//                    LOG_EVENT_STR2("Got size: ", glyphAttrib.size);
                }
                break;
            default:
                break;
        }
    }
}

void DocParser::getStyle(int &ch){
    skipBlanks(ch);

    while('\"' != ch){
        docStream >> ch;
        switch(ch){
            case 't':
                if (match("ext-indent:")){
                    if (LM_NONE == listMode){
                        lineAttrib.indent = getFloat('m') * (DPI) / 25.4;
                    }
                }
                else if (match("ext-align:")){
                    skipBlanks(ch);
                    char* align = getString(';');
                    if(strcmp(align, "center") == 0){
                        lineAttrib.align = A_CENTRAL;
                    }
                    else if(strcmp(align, "left") == 0){
                        lineAttrib.align = A_LEFT;
                    }
                    else if(strcmp(align, "right") == 0){
                        lineAttrib.align = A_RIGHT;
                    }
                    delete align;
                }
                break;
            case 'l':
                if(match("ine-height:")){
                    lineAttrib.height = getFloat('m') * (DPI) / 25.4;
                }
                break;
            default:
                break;
        }
    }

}
void DocParser::getGraphAttrib(int & ch, Graph & img){
    skipBlanks(ch);

    while('>' != ch){
        docStream >> ch;
        switch(ch){
            case 'w':
                if (match("idth")){
                    skipBlanks(ch);
                    if(!match("=")) throw Except_Parse_Err();
                    skipBlanks(ch);
                    img.SetReqWidth(getInteger());
                }
                // Skip other labels
                break;
            case 'h':
                if (match("eight")){
                    skipBlanks(ch);
                    if(!match("=")) throw Except_Parse_Err();
                    skipBlanks(ch);
                    img.SetReqHeight(getInteger());
                }
                // Skip other labels
                break;
            case 's':
                if (match("rc")){
                    skipBlanks(ch);
                    if(!match("=")) throw Except_Parse_Err();
                    skipBlanks(ch);
                    // Note: User's resposibility to release string
                    char* pfile = getString(' ');  
                    img.SetSrcFile(pfile);
                    delete pfile;
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

    while ('\0' != ch[i]){
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

    int beg = i;
    while ('\0' != ch[i]){
        // Skip blank spaces
        do{
            docStream >> c;
        }
        while(' ' == c || '\t' == c || '\n' == c);
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

void ParseTable(int & ch){
    skipBlanks(ch);

    Table* tab = new Table(logger);

    // Parse table attributes
    while('>' != ch){
        docStream >> ch;
        if (match_b("width=")){
            tab->width = getFloat('\"') * (SCREEN_WIDTH - 2 * MARGIN_VERTICAL); 
        }
        else if (match_b("border=")){
            tab->border = getInteger();
        }
        else if (match_b("cols")){
            tab->col = getInteger();
        }
        else if (match_b("rows")){
            tab->row = getInteger();
        }
    }

    // Parse row by row
    DocStream >> ch;

    while(match_b("<\\table>")){
        getTR(ch, tab);
    }
}

void getTR(int & ch, Table* tab){
    if (match("<tr>")){
        while(!match_b("<\\tr>")){
            docStream >> ch;
            getTD(ch, Table);
        }
    }
}

void getTD(int &ch, Table* tab){
    if (match("<td")){
        // Get td attribute
        docStream >> ch;
        while('>' != ch){
            if(match_b("width")){
            }
        }
    }
}
// Auxilary functions
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

double DocParser::getFloat(int term){
    std::string str;
    int ch;
    docStream >> ch;
    skipBlanks(ch);
    if ('\"' == ch){ 
        term = '\"';
        docStream >> ch;
    }
    do{
        str += (char)ch;
        docStream >> ch;
    }
    while(term != ch);

    return std::strtod(str.c_str(), NULL);
}

char* DocParser::getString(int term){
    std::string str;
    int ch;
    docStream >> ch;
    skipBlanks(ch);
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

