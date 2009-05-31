#ifndef MAY_TWELFTH_H
#define MAY_TWELFTH_H

#include "Context.h"

class May12th{
    public:
        May12th(Logger* log, const char* filename);
        ~May12th();

    public:
        // Interface for test.

    public:
        inline void SetEncoding(ENCODING_MODE em) { encoding = em; }
        inline ENCODING_MODE GetEncoding() { return encoding; }

    public:
        void MainLoop();
//        void PerCharDisplay(int page_num);
        void Display(int page_num);

        inline Context* GetContext(){ return ctx; }

    private:
        void Init(uint32 fontSize);
//        bool RenderChar(Char &);

    private:
        const static int screen_width;
        const static int screen_height;

    private:
        ENCODING_MODE   encoding;

    private:
        Context*        ctx;

    private:
        Logger*         logger;
};

#endif

