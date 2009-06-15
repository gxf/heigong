#ifndef DOC_PARSER_H
#define DOC_PARSER_H

#include "Common.h"
#include "DocStream.h"
#include <deque>
#include <queue>
#include <stdio.h>

class Logger;
class Glyph;
class Char;
class Graph;
class Table_Row;
class Table;
class LayoutManager;

class DocParser{
    public:
        DocParser(Logger* log);
        ~DocParser();

    public:
        typedef enum DOC_PARSER_RETURN_TYPE{
            DP_OK,
            DP_EOF,
            DP_INVALID,     // Current stream is invalid
            DP_ERROR,       // Internal parse error
        }DP_RET_T;

        typedef struct DocState_Rec{
            std::deque<Glyph*>  buffer;
            long int            offset;
        }DocState, *HDocState;

    public:
        bool Init(const char* filen);
        DP_RET_T GetNextGlyph(Glyph** g, LayoutManager* line);
        void SetCurParseOffset(long int offset);

        HDocState ShadowDocState();
        bool RestoreDocState(HDocState hState);

    private:
        void ClearGlyphStream();
        
    private:
        bool match(char ch);
        bool match(const char* chs);
        bool match_b(const char* chs);

    private:
        void fillGlyphStream(LayoutManager* layout);
        void getStyle(int & ch);
        void getGraphAttrib(int & ch, Graph & img);
       	void getMyFont(int &ch);
        void skipBlanks(int & ch);
        bool procLabel(int & ch);
        void procWord(int & ch);
        void ParseTable(int & ch);
        void getTR(int &ch, Table* tab);
        void getTD(int &ch, Table_Row* tr);

    private:
        long int getInteger();
        double getFloat(int term);
        char* getString(int term);

    private:
        enum LIST_MODE{
            LM_NONE,
            LM_DIR,      // <dir>, directory list
            LM_MEMU,     // <menu>, menu list
            LM_ORDER,    // <ol>, ordered list
            LM_UNORDER,  // <ul>, un-ordered list
            LM_DEFIN,    // <dl>, definition list
        };
        LIST_MODE listMode;  
        bool      headerMode;

    private:
        std::queue<Glyph*>  delayedToken;
        std::deque<Glyph*>  glyphBuffer;
        DocStream           docStream;

    private:
        Attrib_Glyph        glyphAttrib;
        Attrib_Line         lineAttrib;

    private:
        Logger*             logger;
};

#endif

