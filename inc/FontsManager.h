
#ifndef FONTS_MANAGER_H
#define FONTS_MANAGER_H

#include <vector>
#include <cstring>

#include <ft2build.h>  
#include FT_FREETYPE_H 

class Logger;
class Position;

class FontsManager{
    public:
        FontsManager(Logger *log);
        ~FontsManager();

    public:
        class FontEntry{
        public:
            const char* path;
            FT_Face     face;
        public:
            FontEntry(const char* p, FT_Face f):
                path(p), face(f)
            {}
            FontEntry(const FontEntry & fe){
                path = fe.path;
                face = fe.face;
            }
            bool operator==(FontEntry & fe){
                if (std::strcmp(fe.path, path) == 0)
                    return true;
                return false;
            }
        };

    public:
        // Interfaces
        bool OpenFont(const char* path);
        bool DelFont(FT_Face face);

        bool SetFontSize(int pt);

        void GetGlyphSlot(FT_ULong c, FT_GlyphSlot* slot); 

    private:
        void Init();
        FT_Face FindFont(const char* path);

    private:
        static const char* dftFontPath;

    private:
        FT_Library library;             /* handle to library */  
        std::vector<FontEntry> fonts;     /* handle to face object */ 

        FT_Face curFont;

        int     dpi;

    private:
        Logger* logger;
};

#endif
