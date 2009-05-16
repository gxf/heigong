#ifndef MAY_TWELFTH_H
#define MAY_TWELFTH_H

class Logger;
class FontsManager;
class LayoutManager;
class RenderMan;
class FontsCache;
class DocParser;

class May12th{
    public:
        May12th(Logger* log, const char* filename);
        ~May12th();

    public:
        // Interface for test.
        bool RenderString(const char* str);
        bool RenderWord(const char*, int size);

    public:
        void MainLoop();
        void PerCharDisplay();

    private:
        bool RenderChar(const char);

    private:
        Logger*         logger;

    private:
        FontsManager    fonts;
        LayoutManager   layout;
        RenderMan       render;
        FontsCache      fontsCache;
        DocParser       docParse;

    private:
        const static int screen_width;
        const static int screen_height;
};

#endif

