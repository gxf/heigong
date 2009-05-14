#include "Common.h"
#include "Logger.h"
#include "LayoutManager.h"
#include "FontsManager.h"
#include "RenderMan.h"

LayoutManager::LayoutManager(int w, int h, int mv, int mh, 
                             Logger* log, int ls, int ws):
    p_width(w), p_height(h), 
    v_m_width(mv), h_m_width(mh), 
    g_line_spacing(ls),g_word_spacing(ws),
    curPos(mv, mh), curMaxHeight(0),
    logger(log)
{
}

LayoutManager::~LayoutManager(){}

const Position LayoutManager::GetProperPos(GLYTH_TYPE tp, int width, int height){
    Position pos(0, 0);

    switch(tp){
        case GT_CHAR:
            GetCharPos(pos, width, height);
            break;
        case GT_BITMAP:
            GetImagePos(pos, width, height);
            break;
        default:
            LOG_ERROR("Unsupported glyth type.");
            break;
    }

    return pos;
}

void LayoutManager::GetCharPos(Position & pos, int width, int height){
    curMaxHeight = (curMaxHeight > height) ? curMaxHeight : height;
    if (curPos.x + g_word_spacing + width >= p_width - h_m_width){
        // Current line is over for use
        if (curPos.y + g_line_spacing + curMaxHeight >= p_height - v_m_width){
            // & current page is over for use
            // Return invalid position
            curPos.x = v_m_width;
            curPos.y = h_m_width;
            pos.x = -1;
            pos.y = -1;
            return;
        }
        else{
            // Return position of new line head
            curPos.x = h_m_width;
            curPos.y += g_line_spacing + curMaxHeight;
            pos = curPos;
            curPos.x += width + g_word_spacing;
            curMaxHeight = height;
        }
    }
    else
    {
        // Current line still have space, return curPos
        pos = curPos;
        curPos.x += width + g_word_spacing;
    }
    pos.y += curMaxHeight;
}

void LayoutManager::GetImagePos(Position & pos, int width, int height){
}

void LayoutManager::NewLine(){
    curPos.x = h_m_width;
    curPos.y += curMaxHeight + g_line_spacing;
}

void LayoutManager::NewPage(){
    curPos.x = v_m_width;
    curPos.y = h_m_width;
}
