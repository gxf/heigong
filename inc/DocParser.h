#ifndef DOC_PARSER_H
#define DOC_PARSER_H

#include "Common.h"
#include "DocStream.h"
#include "DocState.h"
#include <deque>
#include <queue>
#include <stdio.h>

class Logger;
class Glyph;
class Char;
class Graph;
class Table_Data_Cell;
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

    public:
        bool Init(const char* filen, bool convert = true, bool backgroud = false);
        DP_RET_T GetNextGlyph(Glyph** g, LayoutManager* line);
        void SetCurParseOffset(long int offset);

        HDocState ShadowDocState();
        bool PostStoreState(HDocState hState);
        bool RestoreDocState(HDocState hState);

    public:
        DocParser & operator<<(Glyph * g);

    private:
        void ClearGlyphStream();
        
    private:
        bool match(char ch);
        bool match(const char* chs);
        bool match_b(const char* chs);

    private:
        void fillGlyphStream(LayoutManager* layout);
        void getStyle(int & ch, Attrib_Line & la);
        void getGraphAttrib(int & ch, Graph & img);
       	void getMyFont(int &ch, Attrib_Glyph & ga);
        void skipBlanks(int & ch);
        void skipTill(const char* tok[], int len);
        bool procLabel(int & ch);
        void procWord(int & ch);

        // Table related
        void ParseTable(int & ch);
        void getTR(int &ch, Table* tab);
        void getTD(int &ch, Table_Row* tr, uint32 & off);
        bool procTableLabel(int & ch, Table_Data_Cell * tdc);
        void procTableWord(int & ch, Table_Data_Cell * tdc);

    protected:
        long int getInteger();
        double getFloat(int term);
        char* getString(int term);

    protected:
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

