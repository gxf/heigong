#include "FontsManager.h"
#include "Logger.h"

FontsManager::FontsManager(Logger* log):
    logger(log)
{
    int error = FT_Init_FreeType(&library); 
    if (!error){
        LOG_ERROR("Failed to init font library.");
    }
    else{
        LOG_EVENT("Font library initialization succeeds.");
    }
}

FontsManager::~FontsManager(){
    std::vector<FT_Face *>::iterator itr = faces.begin();
    while(itr != faces.end()){
        delete *itr;
        ++itr;
    }
}

FontsManager::Handle FontsManager::OpenFont(const char* path){
    FT_Face* pFace = new FT_Face;

    int error = FT_New_Face(library, path, 0, pFace ); 
    switch(error){
        case 0:
            char buf[200];
            sprintf(buf, "Opened new font face at %s", path); 
            LOG_EVENT(buf);
            break;
        case FT_Err_Unknown_File_Format:
            LOG_ERROR("Unknow file format to open.");
            return (Handle)0;
        default:
            LOG_ERROR("Open new font face fails.");
            return (Handle)0;
    }
    faces.push_back(pFace);

    return reinterpret_cast<Handle>(pFace);
}

bool FontsManager::DelFont(Handle hFont){
    std::vector<FT_Face *>::iterator itr = faces.begin();

    while(itr != faces.end()){
        if (*itr == reinterpret_cast<FT_Face *>(hFont)){
            delete *itr;
            faces.erase(itr);
            LOG_EVENT("Font is deleted");
            return true;
        }
        ++itr;
    }

    LOG_ERROR("Fail to delete font.");
    return false;
}

FT_GlyphSlot FontsManager::GetGlyph(FT_ULong ch, Handle hFont){
    FT_UInt glyph_index;
    FT_Face* pFace = reinterpret_cast<FT_Face*>(hFont);

    glyph_index = FT_Get_Char_Index(*pFace, ch); 

    /* load glyph image into the slot (erase previous one) */  
    int error = FT_Load_Glyph(*pFace, glyph_index, FT_LOAD_DEFAULT); 
    if (error){
        LOG_WARNING("Fail to load glyph image");
    }

    /* convert to an anti-aliased bitmap */  
    error = FT_Render_Glyph((*pFace)->glyph, FT_RENDER_MODE_NORMAL); 
    if (error){
        LOG_WARNING("Fail to convert glyph image to anti-aliased bitmap.");
    }
    return (*pFace)->glyph;
}

