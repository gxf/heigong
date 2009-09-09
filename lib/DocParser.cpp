#include "Table.h"
#include "PageLayout.h"
#include "TableLayout.h"
#include "DocParser.h"
#include <stdlib.h>
#include <string>
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
    docStream(log, (std::string(work_dir)+std::string(DEFAULT_TMP_FILE_NAME)).c_str()), 
    logger(log)
{}

DocParser::~DocParser(){
    ClearGlyphStream();
}

bool DocParser::Init(const char* filen, bool convert, bool background){
    if (true == convert){
        return docStream.OpenFile(filen, background);
    }
    else{
        return docStream.OpenFileDirect(filen);
    }
}

void DocParser::SetCurParseOffset(long int offset){
    ClearGlyphStream();
    docStream.SetOffset(offset);
}

void DocParser::ClearGlyphStream(){
//#if 0
    while(!(glyphBuffer.empty())){
        delete glyphBuffer.front();
        glyphBuffer.pop_front();
    }
//#endif
    while(!delayedToken.empty()){
        delete delayedToken.front();
        delayedToken.pop();
    }
//    glyphBuffer.clear();
}

DocParser::DP_RET_T 
DocParser::GetNextGlyph(Glyph** glyph, LayoutManager * layout){
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
        glyphBuffer.push_back(new Eof(logger));
        *glyph = glyphBuffer.front();
        glyphBuffer.pop_front();
        return DP_OK;
//        return DP_EOF;
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

HDocState DocParser::ShadowDocState(){
    // Shadow docStream offset and glyphBuffer
    HDocState shadowState = new DocState(logger);
    std::deque<Glyph*>::iterator itr= glyphBuffer.begin();
    while(itr != glyphBuffer.end()){
        shadowState->buffer.push_back((*itr)->Dup());
        ++itr;
    }
    shadowState->offset = docStream.GetCurOffset();
    shadowState->glyphAttrib = glyphAttrib;
    shadowState->lineAttrib  = lineAttrib;
    return shadowState;
}

bool DocParser::RestoreDocState(HDocState hState){
    // Restore the docStream offset and glyphBuffer
    ClearGlyphStream();
    std::deque<Glyph*>::iterator itr= hState->buffer.begin();
    while(itr != hState->buffer.end()){
        glyphBuffer.push_back((*itr)->Dup());
        ++itr;
    }
    docStream.SetOffset(hState->offset);
    glyphAttrib = hState->glyphAttrib;
    lineAttrib  = hState->lineAttrib;

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
    PageLayout* pl = dynamic_cast<PageLayout *>(layout);
    if (NULL == pl){
        // This should not happen
        exit(0);
    }
    pl->curLine->SetAttrib(lineAttrib);

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
    // Search label for attribute, image, table, hyperlink
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
                while('>' != ch){ docStream >> ch; }
                return false;   // Return immediately for line processing
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
                getMyFont(ch, glyphAttrib);
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
                getStyle(ch, lineAttrib);
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
                return false;
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
    // .    == &hellip
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
        else if (match("hellip;")){
            c->SetVal('.');
        }
        else{ 
            docStream << ch; 
            docStream >> *c;
        }
    }
    else if (match("\r")){
        c->SetVal(' ');
    }
    else if (match("\n")){
        c->SetVal('\n');
    }
    else{
        docStream << ch; 
        docStream >> *c;
    }
    if(!headerMode){
        glyphBuffer.push_back(c);
    }
}

void DocParser::getMyFont(int &ch, Attrib_Glyph & glyphAttr){
    skipBlanks(ch);

    while('>' != ch){
        docStream >> ch;
        switch(ch){
            case 'n':
                if (match("ame") && match_b("=")){
                    // TODO: FONT translate
//                    glyphAttr.font = "";
                }
                break;
            case 's':
                if (match("ize") && match_b("=")){
                    glyphAttr.size = getInteger() / 2;
//                    LOG_EVENT_STR2("Got size: ", glyphAttrib.size);
                }
                break;
            default:
                break;
        }
    }
}

void DocParser::getStyle(int &ch, Attrib_Line & lineAttr){
    skipBlanks(ch);

    while('\"' != ch){
        docStream >> ch;
        switch(ch){
            case 't':
                if (match("ext-indent:")){
                    if (LM_NONE == listMode){
                        lineAttr.indent = getFloat('m') * (g_dpi) / 25.4;
                    }
                }
                else if (match("ext-align:")){
                    skipBlanks(ch);
                    char* align = getString(';');
                    if(strcmp(align, "center") == 0){
                        lineAttr.align = A_CENTRAL;
                    }
                    else if(strcmp(align, "left") == 0){
                        lineAttr.align = A_LEFT;
                    }
                    else if(strcmp(align, "right") == 0){
                        lineAttr.align = A_RIGHT;
                    }
                    delete [] align;
                }
                break;
            case 'l':
                if(match("ine-height:")){
                    lineAttr.height = getFloat('m') * (g_dpi) / 25.4;
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
                    delete [] pfile;
                }
                // Skip other labels
                break;
            default:
                break;
        }
    }
}

// Table related
void DocParser::ParseTable(int & ch){
    skipBlanks(ch);

    Table* tab = new Table(logger);

    // Parse table attributes
    while('>' != ch){
        if (match_b("width=")){
            uint32 tab_width = (uint32)(getFloat('%') * (scr_width - 2 * MARGIN_VERTICAL) / 100);
            tab->SetWidth(tab_width); 
            tab->SetOffset((scr_width - 2 * MARGIN_VERTICAL -tab_width) / 2);
        }
        else if (match_b("border=")){
            tab->SetBorder(getInteger());
        }
        else if (match_b("cols=")){
            tab->SetCol(getInteger());
        }
        else if (match_b("rows=")){
            tab->SetRow(getInteger());
        }
        else{
            docStream >> ch;
        }
    }

    // Parse row by row
    while(!match_b("</table>")){
        getTR(ch, tab);
        const char * tok[] = {"<tr>", "</table>"};
        skipTill(tok, 2);
    }

    glyphBuffer.push_back(tab);
}

void DocParser::getTR(int & ch, Table* tab){
    uint32 xoffset = 0;
    if (match_b("<tr>")){
        Table_R * tr = new Table_R(logger, tab->GetWidth(), tab->GetOffset());
        while(!match_b("</tr>")){
            getTD(ch, tr, xoffset);
            // Work around
#if 0
            match_b("<myfont>");
            match_b("</myfont>");
#endif
            const char* wa[] = {"</tr>", "<td"};
            skipTill(wa, 2);
        }
        tab->AddTR(tr);
    }
}

void DocParser::getTD(int &ch, Table_R* tab_r, uint32 &off){
    if (match("<td")){
        uint32 width = tab_r->GetWidth();
        uint32 rowspan = 1;
        uint32 colspan = 1;

        docStream >> ch;
        // Get td attribute
        while('>' != ch){
            if (match_b("width=")){
                double ratio = getFloat('%') / 100;
                width *= ratio;
            }
            else if (match_b("rowspan=")){
                rowspan = getInteger();
            }
            else if (match_b("colspan=")){
                colspan = getInteger();
            }
            else{
                docStream >> ch;
            }
        }
        Table_DC * td = new Table_DC(logger, width, off);
        off += width;

        // Parse all data segments
        while(true){
            docStream  >>  ch;
            while(ch == '<'){
                if(false == procTableLabel(ch, td)){ 
                    tab_r->AddTD(td);
                    if (ch == '<'){
                        docStream << ch;
                    }
                    return;
                }
                // If wvWare works fine, the EOF will not throw
                docStream >> ch;
                skipBlanks(ch);
            }
    
            // set line attrib
            // For convinience, only the last set affect the total cell
            td->cellLayout.curLine->SetAttrib(lineAttrib);

            td->PushDelayedLabel();

            // proc the word content. 
            while(ch != '<'){
                procTableWord(ch, td);
                docStream >> ch;
            }
            if (ch == '<'){
                docStream << ch;
            }
//            td->Setup(td->cellLayout);
        }
    }
}

bool DocParser::procTableLabel(int & ch, Table_DC* tdc){
    // Search label for attribute, image, hyperlink
    docStream >> ch;
    // Only process certain labels
    switch(ch){
        case 'b':
            if(match("r>")){
                // interpret <br> to new line
                tdc->AddChar(new Char(logger, '\n'));
                break;
            }
            // ignore all left labels start with 'b'
            while('>' != ch){ docStream >> ch; }
            break;
        case 'd':
            if(match("iv")){
                // TODO: <div ...>
                tdc->AddChar(new Char(logger, '\n'));
            }
            // ignore all left labels start with 'd'
            while('>' != ch){ docStream >> ch; }
            break;
        case 'i':
            // Image inside of table is not supported 
            while('>' != ch){ docStream >> ch; }
            break;
        case 'l':
            if(match("i")){
                switch(listMode){
                    case LM_ORDER:
                        if(match_b("value=")){
                            // Notice: not larger than 10
                            tdc->AddDelayedChar(new Char(logger, '0' + getInteger()));
                            tdc->AddDelayedChar(new Char(logger, '.'));
                            tdc->AddDelayedChar(new Char(logger, ' '));
                        }
                        break;
                    case LM_UNORDER:
                        tdc->AddDelayedChar(new Char(logger, ' '));
                        tdc->AddDelayedChar(new Char(logger, '*'));
                        tdc->AddDelayedChar(new Char(logger, ' '));
                        break;
                    default:
                        break;
                }
            }
            while('>' != ch){ docStream >> ch; }
            break;
        case 'm':
            if (match("yfont")){
                getMyFont(ch, tdc->glyphAttrib);
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
                tdc->AddChar(new Char(logger, '\n'));
                break;
            }
            else if (match_b("style") && match_b("=") && match("\"")){
                // <p style="..."> 
                getStyle(ch, tdc->lineAttrib);
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
            // Table inside of table is not supported
            while('>' != ch){ docStream >> ch; }
            break;
        case '/':
            if(match("div>")){
                tdc->AddChar(new Char(logger, '\n'));
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
                tdc->glyphAttrib.Reset();
                break;
            }
            else if (match("td>")){
                return false;
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

void DocParser::procTableWord(int & ch, Table_DC * tdc){ 
    Char* c = new Char(logger);
    c->SetAttrib(tdc->glyphAttrib);
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
        else if (match("hellip;")){
            c->SetVal('.');
        }
    }
    docStream << ch; 
    docStream >> *c;
    tdc->AddChar(c);
}

/*****************************************************/
/*              Auxilary functions                  */
/*****************************************************/

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

void DocParser::skipTill(const char* tok[], int len){
    int ch;
    while(true){
        int i;
        for (i = 0;i < len; i++){
            if(true == match(tok[i])){
                docStream << tok[i];
                return;
            }
        }
        // Skip this if not match
        docStream >> ch;
    }
}

double DocParser::getFloat(int term){
    // ARM notice: strtod is not portable to ARM
    std::string str;
    int ch;
    uint32 base = 1;
    int term_e = 0;
    bool negative   = false;
    bool dot_add    = false;

    docStream >> ch;
    skipBlanks(ch);

    if ('\"' == ch){ 
        term_e = '\"';
        docStream >> ch;
    }

    do{
        if ((char)ch == '-'){ negative = true; }
        else if ((char)ch == '.'){ dot_add = true; }
        else{
            str += (char)ch;
        }
        docStream >> ch;
        if (true == dot_add){ base *= 10; }
    }
    while(term != ch && term_e != ch);

    // Set right stream position
    if (term_e != term && '\"' == term_e){
        while (ch != term_e)
            docStream >> ch;
    }

    long val = std::strtol(str.c_str(), '\0', 10);
    if (true == negative){
        val = 0 - val;
    }
//    return std::strtod(str.c_str(), NULL);
    return (double)(val * 10) / base;
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


DocParser & DocParser::operator<<(Glyph * g){
    glyphBuffer.push_front(g);
    return *this;
}

