#ifndef MAY_TWELFTH_H
#define MAY_TWELFTH_H

#include "Common.h"
#include "Logger.h"
#include "RenderMan.h"
#include "FontsManager.h"
#include "FontsCache.h"
#include "LayoutManager.h"
#include "DocParser.h"
#include "MayTwelfth.h"
#include "Glyph.h"
#include "Line.h"

class May12th{
    public:
        May12th(Logger* log, const char* filename);
        ~May12th();

    public:
        // Interface for test.
        bool RenderString(const char* str);
        bool RenderWord(const char*, int size);

    public:
        inline void SetEncoding(ENCODING_MODE em) { encoding = em; }
        inline ENCODING_MODE GetEncoding() { return encoding; }

    public:
        void MainLoop();
        void PerCharDisplay();

    private:
        bool RenderChar(Char &);

    private:
        Logger*         logger;

    private:
        FontsManager    fonts;
        LayoutManager   layout;
        RenderMan       render;
        FontsCache      fontsCache;
        DocParser       docParse;
        Line            line;

    private:
        ENCODING_MODE   encoding;

    private:
        const static int screen_width;
        const static int screen_height;
};

#endif

