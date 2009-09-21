// File: hg.cpp
//
// Brief:
//     Implement of the interface.
//

#include "hg.h"
#include "MayTwelfth.h"
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <unistd.h>

// Thread local variable
char8* work_dir     = (char*)(DEFAULT_WORK_DIR);
char8* html_dir     = NULL;
uint32 scr_width    = SCREEN_WIDTH;
uint32 scr_height   = SCREEN_HEIGHT;
uint32 g_dpi        = DPI_DFT;
bool fast_page_sum  = false;
bool pg_based_render= false;

static Logger* logger;

static bool r_only  = false;
static bool serd    = false; // Serialized
static bool async   = false;

hHgMaster HG_Init(const char* file, const char* path, const char* html_path, 
                  bool asynchronize, bool render_only, bool serialized, bool fps,
                  uint32 screen_width, uint32 screen_height, uint32 dpi)
{
    fast_page_sum = fps;

    // Negtive number protector
    if (screen_width > 10000 || screen_height > 10000 || dpi > 1000){
        return NULL;
    }
    scr_width  = screen_width;
    scr_height = screen_height;
    g_dpi      = dpi;
    logger = new Logger;
    if (!logger){
        return NULL;
    }
    r_only  = render_only;
    serd    = serialized;
    async   = asynchronize;

    work_dir = new char8[std::strlen(path) + 1];
    std::strcpy(work_dir, path);
    if (NULL != html_path){
        html_dir = new char8[std::strlen(html_path) + 1];
        std::strcpy(html_dir, html_path);
    }
    else{
        html_dir = NULL;
    }

    May12th * engine = new May12th(logger, file, !r_only);
    if (!engine){
        delete logger;
        return NULL;
    }

    return reinterpret_cast<hHgMaster>(engine);
}

bool HG_StartParse(hHgMaster hHG){
    May12th * engine = reinterpret_cast<May12th*>(hHG);
    if (true == serd){
        if (true == r_only){ 
            return engine->StartForeGroundSerializedNoConv();
        }
        else{
            return engine->StartForeGroundSerialized();
        }
    }
    else{
        if (true == async){
            return engine->StartBackGround();
        }
        else{
            return engine->StartForeGround();
        }
    }
}

p_page_info HG_GetPage(hHgMaster hHG, uint32 pg_num){
    May12th * engine = reinterpret_cast<May12th*>(hHG);

    p_page_info pg_info = new page_info;
    pg_info->width   = 0;
    pg_info->height  = 0;
    pg_info->depth   = 0;
    pg_info->img     = NULL;

    bool ret = engine->GetPage(pg_num, &pg_info->width, &pg_info->height, 
                               &pg_info->depth, &pg_info->img);
    if (false == ret && NULL == pg_info->img){
        delete pg_info;
        return NULL;
    }
    else{
        return pg_info;
    }
}

uint32 HG_GetCurMaxPage(hHgMaster hHG){
    May12th * engine = reinterpret_cast<May12th*>(hHG);

    return engine->GetCurMaxPage();
}

uint32 HG_GetMaxPage(hHgMaster hHG){
    May12th * engine = reinterpret_cast<May12th*>(hHG);

    return engine->GetMaxPage();
}

bool HG_FreePage(hHgMaster hHG, p_page_info hPG){
    May12th * engine = reinterpret_cast<May12th*>(hHG);
    bool ret = engine->FreePage(hPG->img);
    delete hPG;
    return ret;
}

bool HG_Term(hHgMaster hHG){
    May12th * engine = reinterpret_cast<May12th*>(hHG);
    if (NULL == engine){
        return false;
    }
    engine->Term();
    delete logger;
    delete engine;
    if (0 != strcmp(work_dir, DEFAULT_WORK_DIR)){
        delete [] work_dir;
    }
    char cmd[] = "killall -9 lt-wvWare";
    system(cmd);

    return true;
}

/*************************************************************************/
//
// PAGE_BASED API
//
// HG_PB_...
hHgMaster HG_PB_Init(const char *html_file_name, const char* pg_file_path, const char * html_file_path, 
                     uint32 screen_width, uint32 screen_height, uint32 dpi)
{
    pg_based_render = true;

    // Negtive number protector
    if (screen_width > 10000 || screen_height > 10000 || dpi > 1000){
        return NULL;
    }
    scr_width  = screen_width;
    scr_height = screen_height;
    g_dpi      = dpi;
    logger = new Logger;
    if (!logger){
        return NULL;
    }
    work_dir = new char8[std::strlen(pg_file_path) + 1];
    std::strcpy(work_dir, pg_file_path);
    if (NULL != html_file_path){
        html_dir = new char8[std::strlen(html_file_path) + 1];
        std::strcpy(html_dir, html_file_path);
    }
    else{
        html_dir = NULL;
    }

    May12th * engine = new May12th(logger, html_file_name, false); // No convert
    if (!engine){
        delete logger;
        return NULL;
    }

    return reinterpret_cast<hHgMaster>(engine);
}

bool HG_PB_StartParse(hHgMaster hHG){
    May12th * engine = reinterpret_cast<May12th*>(hHG);
    // Only one mode get involved.
    return engine->StartForeGroundSerializedNoConv();
}

bool HG_PB_SetPage(hHgMaster hHG, uint32 pg_num){
    May12th * engine = reinterpret_cast<May12th*>(hHG);

    engine->PB_Set2Page(pg_num);
    return true;
}

p_page_info HG_PB_GetReRenderedPage(hHgMaster hHG, uint32 pg_num){
    May12th * engine = reinterpret_cast<May12th*>(hHG);

    p_page_info pg_info = new page_info;
    pg_info->width   = 0;
    pg_info->height  = 0;
    pg_info->depth   = 0;
    pg_info->img     = NULL;

    bool ret = engine->PB_GetPage(pg_num, &pg_info->width, &pg_info->height, 
                               &pg_info->depth, &pg_info->img);
    if (false == ret && NULL == pg_info->img){
        delete pg_info;
        return NULL;
    }
    else{
        return pg_info;
    }
    return NULL;
}

bool HG_PB_FreePage(hHgMaster hHG, p_page_info hPG){
    May12th * engine = reinterpret_cast<May12th*>(hHG);
    bool ret = engine->FreePage(hPG->img);
    delete hPG;
    return ret;
}

bool HG_PB_Term(hHgMaster hHG){
    May12th * engine = reinterpret_cast<May12th*>(hHG);
    if (NULL == engine){
        return false;
    }
    engine->Term();
    delete logger;
    delete engine;
    if (0 != strcmp(work_dir, DEFAULT_WORK_DIR)){
        delete [] work_dir;
    }
    return true;
}
