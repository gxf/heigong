#ifndef MAY_TWELFTH_H
#define MAY_TWELFTH_H

class Logger;
class FontsManager;
class LayoutManager;
class RenderMan;

class May12th{
    public:
        May12th(Logger* log);
        ~May12th();

    public:
        // Interface for test.
        bool RenderString(const char* str);

    public:
//        bool RenderWord(){return true;}

    private:
        Logger*         logger;

    private:
        FontsManager    fonts;
        LayoutManager   layout;
        RenderMan       render;
    private:
        const static int screen_width;
        const static int screen_height;
};

#endif

