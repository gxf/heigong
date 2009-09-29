#include "Common.h"
#include "Logger.h"
#include "FontsManager.h"

FontsTab::fTabEntry FontsTab::fontsTab[] = {
    { "宋体", "./fonts/simsun.ttc"},
    { "仿宋", "./fonts/simfang.ttf"},
    { "黑体", "./fonts/simhei.ttf"},
    { "楷体", "./fonts/simkai.ttf"},
    { "DroidSansFallBack", ""},
};

const char* FontsManager::dftFontPath = DEFAULT_FONT;

FontsManager::FontsManager(Logger* log):
    curFont(NULL), dpi(g_dpi),
    widTab(NULL), horiBearingTab(NULL), inited(false),
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
    inited = false;
}

FontsManager::~FontsManager(){
    if (widTab == NULL){
        delete [] widTab;
    }
    if (horiBearingTab == NULL){
        delete [] horiBearingTab;
    }
    inited = false;
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

    SetFontSize(DEFAULT_FONT_SIZE);
    return true;
}

void FontsManager::PreBuildWidTab(){
    if(false == inited){
        if (curFont == NULL){
            LOG_EVENT("Font is not opened.");
            exit(0);
        }
        int width = 46 * DEFAULT_FONT_SIZE * g_dpi * 100 / (64 * 254 * 20);
        std::cout << "***********************************"
            << "g_dpi = " << g_dpi << std::endl;
        int error = FT_Set_Pixel_Sizes(curFont, width, width);
        if (error){
            LOG_EVENT("Fail to set char size.");
            exit(0);
        }

        FT_GlyphSlot glyphSlot;

        widTab = new int[256];
        horiBearingTab = new int[256];

        int i;
        for (i = 0; i < 256; i++){
            GetGlyphSlot((FT_ULong)i, &glyphSlot);
            widTab[i] = ((glyphSlot->advance.x) >> 6);//* EXP_RATIO / glyphSlot->bitmap.rows; // width * EXP_RATIO / height
            horiBearingTab[i] = ((glyphSlot->metrics.horiBearingX) >> 6);
        }
        inited = true;
    }
}

bool FontsManager::SetFontSize(int pt){
    if (curFont == NULL)
        return false;
//    int error = FT_Set_Char_Size(curFont, pt * 64, pt * 64, g_dpi, g_dpi); 
    // 2.3 * (pt * dpi / 2.54), 2.3 is got by experiment
    int width = 46 * pt * g_dpi * 100 / (64 * 254 * 20);
    int error = FT_Set_Pixel_Sizes(curFont, width, width);
    if (error){
        LOG_EVENT("Fail to set char size.");
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
}

void FontsManager::GetGlyphSlotNoRender(FT_ULong ch, FT_GlyphSlot* slot){
    FT_UInt glyph_index; /* retrieve glyph index from character code */  

    glyph_index = FT_Get_Char_Index(curFont, ch); /* load glyph image into the slot (erase previous one) */  
    int error = FT_Load_Glyph(curFont, glyph_index, FT_LOAD_DEFAULT ); 

    if (error){
        LOG_WARNING("Fail to convert glyph image to anti-aliased bitmap.");
    }

    *slot = curFont->glyph;
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

