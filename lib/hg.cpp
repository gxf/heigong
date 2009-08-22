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

// Global variable
char* work_dir      = DEFAULT_WORK_DIR;
uint32 scr_width    = SCREEN_WIDTH;
uint32 scr_height   = SCREEN_HEIGHT;
static Logger* logger;

static bool r_only  = false;
static bool serd    = false; // Serialized
static bool async   = false;

hHgMaster HG_Init(const char* file, const char* path, bool asynchronize, bool render_only, bool serialized, uint32 screen_width, uint32 screen_height){
    // Negtive number protector
    if (screen_width > 10000 || screen_height > 10000){
        return NULL;
    }
    scr_width  = screen_width;
    scr_height = screen_height;
    logger = new Logger;
    if (!logger){
        return NULL;
    }
    r_only  = render_only;
    serd    = serialized;
    async   = asynchronize;

    work_dir = new char8[std::strlen(path) + 1];
    std::memcpy(work_dir, path, std::strlen(path) + 1);

    May12th * engine = new May12th(logger, file, !serialized);
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
    if (false == ret){
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
    char cmd[] = "killall -9 lt-wvWare";
    system(cmd);

    return true;
}
