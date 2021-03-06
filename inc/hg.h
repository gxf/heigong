#ifndef HG_H
#define HG_H

typedef unsigned int uint32;

typedef uint32 hHgMaster;   // Handle of the engine

// Note: client should never try to deallocate memory of page_info
// directly. Nor does its inner "img" buffer.
typedef struct{
    uint32 width;
    uint32 height;
    uint32 depth;
    void*  img;
}page_info, *p_page_info;

// BRIEF:
//     This function setup the necessary engineer structures with 
// given DOC file.
//
// PARAMS:
//     file - file name in char string.
//     path - working directory for *.pg, tmp.hg files
//     html_path - html root path
//     asynchronize - async mode on/off
//     render_only - render with/without serialize information generation
//     serialized - serialized info is/isnot ready
//     screen_width - the width of the screen
//     screen_height - the height of the screen
//     dpi - as the name suggest
//     fps - fast calculate page sum
// RETURN:
//     Handler to engine context - if success
//     NULL - if fails
hHgMaster HG_Init(const char* file, const char* path, const char * html_path, 
                  bool asynchronize, bool render_only, bool serialized, bool fps,
                  uint32 screen_width, uint32 screen_height, uint32 dpi);

// BRIEF:
//     This function start the engineer to parse the DOC file set.
//
// PARAMS:
//     hHG - Handler fo engine context
// RETURN:
//     true - if sucessful
//     fasle - if error happens.
bool HG_StartParse(hHgMaster hHG);

// BRIEF:
//     This function get the certain page of the DOC file.
//
// PARAMS:
//     hHG - Handler fo engine context
//     pg_num - page number to render
// RETURN:
//     p_page_info - return page info structure if it is sucessful
//     NULL - if the page exceed the max page.
// Notes:
//     1. The function will not return unless the background parser has
//     reached to this page.
//     2. The memory of page_info will be valid in memory unless the
//     function FreePage is called.
p_page_info HG_GetPage(hHgMaster hHG, uint32 pg_num);

// BRIEF:
//     This function get the total page processed by engine
//
// PARAMS:
//     hHG - Handler fo engine context
// RETURN:
//     Total page already processed by engine
uint32 HG_GetCurMaxPage(hHgMaster hHG);

// BRIEF:
//     This function get the total page, fast page sum mode 
//     only. This is a sync function.
//
// PARAMS:
//     hHG - Handler fo engine context
// RETURN:
//     Total page number
uint32 HG_GetMaxPage(hHgMaster hHG);

// BRIEF:
//     This function free the memory page_info structure holds.
//
// PARAMS:
//     hHG - Handler fo engine context
//     hPG - Handler(pointer) of page_info structure
// RETURN:
//     true - if sucessful
//     fasle - if error happens.
// Notes:
//     Memory of rendered framebuffer is managed by the client
bool HG_FreePage(hHgMaster hHG, p_page_info hPG);

// BRIEF:
//     This function terminate the engineer to parse the DOC file set.
//
// PARAMS:
//     hHG - Handler fo engine context
// RETURN:
//     true - if sucessful
//     fasle - if error happens.
// Notes:
//     If the background parser still works, it will terminate the application then.
bool HG_Term(hHgMaster hHG);

/*************************************************************************/
//
// PAGE_BASED API
//
// HG_PB_...
hHgMaster HG_PB_Init(const char * html_file_name, const char * pg_file_path, const char * html_file_path, 
                     uint32 screen_width, uint32 screen_height, uint32 dpi);

bool HG_PB_StartParse(hHgMaster hHG);

bool HG_PB_SetPage(hHgMaster hHG, uint32 pg_num);

p_page_info HG_PB_GetReRenderedPage(hHgMaster hHG, uint32 pgre_render_num);

bool HG_PB_FreePage(hHgMaster hHG, p_page_info hPG);

bool HG_PB_Term(hHgMaster hHG);

#endif
