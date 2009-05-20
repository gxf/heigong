#include "Common.h"
#include "Logger.h"
#include "FontsManager.h"

const char* FontsManager::dftFontPath = DEFAULT_FONT;

FontsManager::FontsManager(Logger* log):
    curFont(NULL), dpi(96),
    logger(log)
{
    int error = FT_Init_FreeType(&library); 
    if (error){
        LOG_ERROR("Failed to init font library.");
    }
    else{
        LOG_EVENT("Font library initialization succeeds.");
        Init();
    }
}

FontsManager::~FontsManager(){
}

void FontsManager::Init(){
    OpenFont(dftFontPath);
}

bool FontsManager::OpenFont(const char* path){
    if ((curFont = FindFont(path)) != NULL){
        return true;
    }

    FT_Face face;

    int error = FT_New_Face(library, path, 0, &face ); 
    switch(error){
        case 0:
            char buf[200];
            sprintf(buf, "Opened new font face at %s", path); 
            LOG_EVENT(buf);
            break;
        case FT_Err_Unknown_File_Format:
            LOG_ERROR("Unknow file format to open.");
            return false;
        default:
            LOG_ERROR("Open new font face fails.");
            return false;
    }
//    error = FT_Select_Charmap(face, FT_ENCODING_GB2312);
    FT_CharMap charmap; 
    int n; 
    for ( n = 0; n < face->num_charmaps; n++ ) { 
        charmap = face->charmaps[n]; 
        switch(charmap->encoding){
            case FT_ENCODING_MS_SYMBOL:
                LOG_EVENT("Supported Encoding: FT_ENCODING_MS_SYMBOL");
                break;
            case FT_ENCODING_UNICODE: 
                LOG_EVENT("Supported Encoding: FT_ENCODING_UNICODE");
                break;
            case FT_ENCODING_SJIS:
                LOG_EVENT("Supported Encoding: FT_ENCODING_SJIS");
                break;
            case FT_ENCODING_GB2312:
                LOG_EVENT("Supported Encoding: FT_ENCODING_GB2312");
                break;
            case FT_ENCODING_BIG5:
                LOG_EVENT("Supported Encoding: FT_ENCODING_BIG5");
                break;
            case FT_ENCODING_WANSUNG:
                LOG_EVENT("Supported Encoding: FT_ENCODING_WANSUNG");
                break;
            case FT_ENCODING_JOHAB: 
                LOG_EVENT("Supported Encoding: FT_ENCODING_JOHAB");
                break;
            default:
                LOG_ERROR("Unknown Encoding");
                break;
        }
    }
    error = FT_Select_Charmap(face, FT_ENCODING_UNICODE);
    if (error){
        LOG_ERROR("Fail to select charmap encoding FT_ENCODING_GB2312.");
    }
    fonts.push_back(FontEntry(path, face));
    curFont = face;

    SetFontSize(25);
    return true;
}

bool FontsManager::SetFontSize(int pt){
    if (curFont == NULL)
        return false;
    int error = FT_Set_Char_Size(curFont, pt * 64, 0, dpi, 0 ); 
    if (error){
        LOG_ERROR("Fail to set char size.");
        return false;
    }
    return true;
}


bool FontsManager::DelFont(FT_Face face){
    std::vector<FontEntry>::iterator itr = fonts.begin();

    while(itr != fonts.end()){
        if (itr->face == face){
            fonts.erase(itr);
            LOG_EVENT("Font is deleted");
            return true;
        }
        ++itr;
    }

    LOG_ERROR("Fail to delete font.");
    return false;
}

void FontsManager::GetGlyphSlot(FT_ULong ch, FT_GlyphSlot* slot){

    int error = FT_Load_Char(curFont, ch, FT_LOAD_RENDER); 
    if (error){
        LOG_WARNING("Fail to convert glyph image to anti-aliased bitmap.");
    }

    switch(curFont->glyph->bitmap.pixel_mode){
        case FT_PIXEL_MODE_MONO:
            // A monochrome bitmap, using 1 bit per pixel.
            LOG_WARNING("Pixel Mode MONO. Not supported.");
            break;
        case FT_PIXEL_MODE_GRAY:
            // An 8-bit bitmap, generally used to represent anti-aliased glyph images. 
            // Each pixel is stored in one byte. 
            break;
        case FT_PIXEL_MODE_GRAY2:
            // A 2-bit per pixel bitmap
            LOG_WARNING("Pixel Mode GRAY 2 bits per pixel. Not supported.");
            break;
        case FT_PIXEL_MODE_GRAY4:
            // A 4-bit per pixel bitmap
            LOG_WARNING("Pixel Mode GRAY 4 bits per pixel. Not supported.");
            break;
        case FT_PIXEL_MODE_LCD:
            // An 8-bit bitmap, representing RGB or BGR decimated glyph images 
            // used for display on LCD displays;
            LOG_WARNING("Pixel Mode LCD. Not supported.");
            break;
        case FT_PIXEL_MODE_LCD_V:
            // An 8-bit bitmap, representing RGB or BGR decimated glyph images 
            // used for display on rotated LCD displays. 
            LOG_WARNING("Pixel Mode LCD_V. Not supported.");
            break;
        default:
            LOG_ERROR("Unsupported pixel format.");
            exit(0);
    }

    *slot = curFont->glyph;
/*    *bitmap     = &(curFont->glyph->bitmap);
    *metrics    = &(curFont->glyph->metrics);
    topLeft->x  = curFont->glyph->bitmap_left;
    topLeft->y  = curFont->glyph->bitmap_top;
    advance->x  = curFont->glyph->advance.x;
    advance->y  = curFont->glyph->advance.y;
    */
}

FT_Face FontsManager::FindFont(const char* path){
    std::vector<FontEntry>::iterator itr = fonts.begin();

    while(itr != fonts.end()){
        if (strcmp(path, itr->path) == 0){
            return itr->face;
        }
    }
    return NULL;
}