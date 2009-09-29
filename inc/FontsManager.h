
#ifndef FONTS_MANAGER_H
#define FONTS_MANAGER_H

#include <vector>
#include <cstring>

#include <ft2build.h>  
//#include FT_FREETYPE_H 
#include <freetype/freetype.h>

class Logger;
class Position;

class FontsTab{
    public:
        inline static 
        const char* GetFontPath(const char* fontName){
            uint32 i;
            uint32 size = sizeof(fontsTab)/sizeof(fTabEntry);
            for(i = 0; i < size; i++){
                if(!(std::strcmp(fontsTab[i].f_name, fontName))){
                    return fontsTab[i].f_path;
                }
            }
            // Fallback to first 
            return fontsTab[i].f_name;
        }

    public:
        typedef struct fontsTable_Rec{
            const char* f_name;
            const char* f_path;
        }fTabEntry;

    private: 
        static fTabEntry fontsTab[5];
};

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
        void PreBuildWidTab();

        bool DelFont(FT_Face face);

        bool SetFontSize(int pt);

        void GetGlyphSlot(FT_ULong c, FT_GlyphSlot* slot); 
        void GetGlyphSlotNoRender(FT_ULong c, FT_GlyphSlot* slot); 

        inline int32 GetWidRatio(int32 idx){ 
            return (widTab && idx >=0 && idx < 256) ? widTab[idx]: 1; }

        inline int32 GetHoriBearing(int32 idx){ 
            return (horiBearingTab && idx >=0 && idx < 256) ?  horiBearingTab[idx] : 0; 
        }

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
        FontsTab    fontsTab;
        int32*      widTab;
        int32*      horiBearingTab; 
        bool        inited;

    private:
        Logger* logger;
};

#endif
