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
        void* SerializedDisplay(int page_num);

        inline Context* GetContext(){ return ctx; }

    public:
        void RenderAll();
        bool StartForeGround();
        bool StartForeGroundSerialized();
        bool StartForeGroundSerializedNoConv();
        bool StartBackGround();
        bool GetPage(uint32 page_num, uint32 * width, uint32 * height, 
                     uint32 * depth, void** img);
        uint32 GetCurMaxPage();
        uint32 GetMaxPage();
        bool FreePage(void* img);
        bool Term();
        inline Logger* GetLogger(){ return logger; }

        void PB_Set2Page(int page_num);
        void* PB_Display(int page_num);

    private:
        void Init(uint32 fontSize = DEFAULT_FONT_SIZE);
//        bool RenderChar(Char &);

    private:
        bool            inited;     // Status to show if engine is inited
        bool            convert;    // Need html conversion
        bool            bgMode;     // Background Mode
        bool            slMode;     // Serialized Mode
        const char*     filename;
        ENCODING_MODE   encoding;

    private:
        Context*        ctx;

    private:
        Logger*         logger;
};

#endif

