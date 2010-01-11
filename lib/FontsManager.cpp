#include "Common.h"
#include "Logger.h"
#include "FontsManager.h"

static int PreBuildFontPt[] ={
    4, 5, 6, 7, 8, 9, 10, 11, 12,
    13, 14, 15, 16, 17, 18, 19, 20,
    22, 24, 26, 28, 30, 32, 36, 40,
    44, 48, 56, 64, 72, 80, 96, 120
};

FontsTab::fTabEntry FontsTab::fontsTab[] = {
    { "宋体", "./fonts/simsun.ttc"},
    { "仿宋", "./fonts/simfang.ttf"},
    { "黑体", "./fonts/simhei.ttf"},
    { "楷体", "./fonts/simkai.ttf"},
    { "DroidSansFallBack", ""},
};

const char* FontsManager::dftFontPath = DEFAULT_FONT;

FontsManager::FontsManager(Logger* log):
    curFont(NULL), 
//    dpi(g_dpi), 
    inited(false), logger(log)
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
    std::map<int32, int32*>::iterator itr = widTab.begin();
    while(itr != widTab.end()){
        delete [] (itr->second);
        ++itr;
    }
    itr = horiBearingTab.begin();
    while(itr != horiBearingTab.end()){
        delete [] (itr->second);
        ++itr;
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

        int w_num = sizeof(PreBuildFontPt) / sizeof(PreBuildFontPt[0]);
        int cnt;
//        std::cout << "w_num = " << w_num << std::endl;
        for(cnt = 0; cnt < w_num; cnt++){ 
//            int width = 46 * DEFAULT_FONT_SIZE * PreBuildFontPt[cnt] * 100 / (64 * 254 * 20);
            int width = 46 * PreBuildFontPt[cnt] * g_dpi * 100 / (64 * 254 * 20);
//    int width = 46 * pt * g_dpi * 100 / (64 * 254 * 20);
//                << "g_dpi = " << g_dpi << std::endl;
//            std::cout << "Width = " << width << std::endl;
            int error = FT_Set_Pixel_Sizes(curFont, width, width);
            if (error){
                LOG_EVENT("Fail to set char size.");
                exit(0);
            }

            FT_GlyphSlot glyphSlot;

            int32 *curwidTab = new int32[256];
            int32 *curhBTab = new int32[256];

            int i;
            for (i = 0; i < 256; i++){
                GetGlyphSlot((FT_ULong)i, &glyphSlot);
                curwidTab[i] = ((glyphSlot->advance.x) >> 6);//* EXP_RATIO / glyphSlot->bitmap.rows; // width * EXP_RATIO / height
                curhBTab[i] = ((glyphSlot->metrics.horiBearingX) >> 6);
//                std::cout << "width = " << curwidTab[i] << ", hb = " << curhBTab[i] << std::endl;
            }
            widTab.insert(std::make_pair(PreBuildFontPt[cnt], curwidTab));
            horiBearingTab.insert(std::make_pair(PreBuildFontPt[cnt], curhBTab));
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

int32 FontsManager::GetWidRatio(int sz, int32 idx){ 
    if (idx < 0 || idx > 256){
        return 0;
    }
    if (sz < 4) {
        return widTab[4][idx];
    }
    else if (sz > 120) {
        return widTab[120][idx];
    }
    else if (sz > 33 && sz < 35) {
        return widTab[32][idx];
    }
    else if (sz > 36 && sz < 40) {
        return widTab[36][idx];
    }
    else if (sz > 40 && sz < 44) {
        return widTab[40][idx];
    }
    else if (sz > 44 && sz < 48) {
        return widTab[44][idx];
    }
    else if (sz > 48 && sz < 56) {
        return widTab[48][idx];
    }
    else if (sz > 56 && sz < 64) {
        return widTab[56][idx];
    }
    else if (sz > 64 && sz < 72) {
        return widTab[64][idx];
    }
    else if (sz > 72 && sz < 80) {
        return widTab[72][idx];
    }
    else if (sz > 80 && sz < 96) {
        return widTab[80][idx];
    }
    else if (sz > 96 && sz < 120) {
        return widTab[96][idx];
    }
    return widTab[sz][idx];
}

int32 FontsManager::GetHoriBearing(int sz, int32 idx){ 
    if (idx < 0 || idx > 256){
        return 0;
    }
    if (sz < 4) {
        return horiBearingTab[4][idx];
    }
    else if (sz > 120) {
        return horiBearingTab[120][idx];
    }
    else if (sz > 33 && sz < 35) {
        return horiBearingTab[32][idx];
    }
    else if (sz > 36 && sz < 40) {
        return horiBearingTab[36][idx];
    }
    else if (sz > 40 && sz < 44) {
        return horiBearingTab[40][idx];
    }
    else if (sz > 44 && sz < 48) {
        return horiBearingTab[44][idx];
    }
    else if (sz > 48 && sz < 56) {
        return horiBearingTab[48][idx];
    }
    else if (sz > 56 && sz < 64) {
        return horiBearingTab[56][idx];
    }
    else if (sz > 64 && sz < 72) {
        return horiBearingTab[64][idx];
    }
    else if (sz > 72 && sz < 80) {
        return horiBearingTab[72][idx];
    }
    else if (sz > 80 && sz < 96) {
        return horiBearingTab[80][idx];
    }
    else if (sz > 96 && sz < 120) {
        return horiBearingTab[96][idx];
    }
    return horiBearingTab[sz][idx];

}
