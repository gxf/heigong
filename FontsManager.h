
#ifndef FONTS_MANAGER_H
#define FONTS_MANAGER_H

#include <vector>

#include <ft2build.h>  
#include FT_FREETYPE_H 


class Logger;

class FontsManager{
    public:
        FontsManager(Logger *log);
        ~FontsManager();

    public:
        typedef unsigned int Handle;    // Porting notice: 32bit only

    public:
        Handle OpenFont(const char* path);
        bool DelFont(Handle hFont);

        bool SetFontParam(){return true;}
//        bool Set
        FT_GlyphSlot GetGlyph(FT_ULong c, Handle hFont);

    private:
        FT_Library library; /* handle to library */  
        std::vector<FT_Face *> faces;       /* handle to face object */ 

        Logger* logger;
};

#endif
