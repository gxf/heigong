#ifndef MAY_TWELFTH_H
#define MAY_TWELFTH_H

#include "Context.h"

class May12th{
    public:
        May12th(Logger* log, const char* filename, bool convert = true);
        ~May12th();

    public:
        // Interface for test.

    public:
        inline void SetEncoding(ENCODING_MODE em) { encoding = em; }
        inline ENCODING_MODE GetEncoding() { return encoding; }

    public:
        void MainLoop();
//        void PerCharDisplay(int page_num);
        void* Display(int page_num);

        inline Context* GetContext(){ return ctx; }

    public:
        void RenderAll();
        bool StartBackGround();
        bool GetPage(uint32 page_num, uint32 * width, uint32 * height, 
                     uint32 * depth, void** img);
        bool FreePage(void* img);
        bool Term();

    private:
        void Init(uint32 fontSize = DEFAULT_FONT_SIZE);
//        bool RenderChar(Char &);

    private:
        const static int screen_width;
        const static int screen_height;

    private:
        bool            inited;     // Status to show if engine is inited
        bool            convert;    // Need html conversion
        bool            bgMode;     // Background Mode
        const char*     filename;
        ENCODING_MODE   encoding;

    private:
        Context*        ctx;

    private:
        Logger*         logger;
};

#endif

