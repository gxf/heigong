#include "Common.h"
#include "Line.h"
#include "Logger.h"
#include "LayoutManager.h"
#include "FontsManager.h"
#include "RenderMan.h"

LayoutManager::LayoutManager(int w, int h, int mv, int mh, 
                             Line* l, Logger* log, int ls, int ws):
    p_width(w), p_height(h), 
    v_m_width(mv), h_m_width(mh), 
    g_line_spacing(ls),g_word_spacing(ws),
    curPos(mv, mh), 
    curMaxHeight(0), lastMaxHeight(0),
    curBaseline(0), lastBaseline(0),
    firstLine(true),
    line(l), logger(log)
{
}

LayoutManager::~LayoutManager(){}

const Position LayoutManager::GetProperPos(GLYTH_TYPE tp, int width, int height, int bearingY){
    Position pos(0, 0);

    switch(tp){
        case GT_CHAR:
            GetCharPos(pos, width, height, bearingY);
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

LAYOUT_RET LayoutManager::GetCharPos(Position & pos, int width, int height, int bearingY){
    // Setting up baseline and maxheight
    curBaseline =  (curBaseline > bearingY) ? curBaseline : bearingY;
    int delta = height - bearingY;
    curMaxHeight = (curMaxHeight > curBaseline + delta) ? curMaxHeight : curBaseline + delta;

    int Xoff;
    if (firstLine){
        Xoff = line->GetIndent() + g_word_spacing + width;
    }
    else{
        Xoff = g_word_spacing + width;
    }
    if (curPos.x + Xoff >= p_width - h_m_width){
        // Current line is over for use
        char buf[100];
        sprintf(buf, "Current x: %d, y: %d, max height: %d", curPos.x + Xoff, curPos.y + g_line_spacing + curMaxHeight, p_height - v_m_width);
        LOG_EVENT(buf);
        if (curPos.y + g_line_spacing + curMaxHeight >= p_height - v_m_width){
            // & current page is over for use
            // Return invalid position
            char buf[100];
            sprintf(buf, "Current y: %d, max height: %d", curPos.y + g_line_spacing + curMaxHeight, p_height - v_m_width);
            LOG_EVENT(buf);
            curPos.x        = v_m_width;
            curPos.y        = h_m_width;
            lastBaseline    = curBaseline;
            curBaseline     = 0;
            curMaxHeight    = 0;
            pos.x           = -1;
            pos.y           = -1;
            return LO_NEW_PAGE;
        }
        else{
            uint32 lineH = g_line_spacing + curMaxHeight;
            lineH = (lineH > line->GetHeight()) ? lineH : line->GetHeight();
            // Return position of new line head
            curPos.x        = h_m_width;
            curPos.y        += lineH;
            lastBaseline    = curBaseline;
            curBaseline     = bearingY;
            lastMaxHeight   = curMaxHeight;
            curMaxHeight    = height;
            pos             = curPos;
//            if (firstLine){
//                pos.x += line->GetIndent();
//            }
            pos.y           += height;
            curPos.x        += width + g_word_spacing;
            firstLine= false;
            return LO_NEW_LINE;
        }
    }
    else
    {
        // Current line still have space, return curPos
        pos      = curPos;
        if (firstLine){
            pos.x += line->GetIndent();
        }
        pos.y    += height;
        curPos.x += width + g_word_spacing;
        return LO_OK;
    }
}

void LayoutManager::GetImagePos(Position & pos, int width, int height){
}

LAYOUT_RET LayoutManager::NewLine(){
    firstLine = true;
    if (0 == curMaxHeight){
        curPos.y += lastMaxHeight + g_line_spacing;
    }
    else{
        curPos.y += curMaxHeight + g_line_spacing;
    }

    if (curPos.y >= p_height - v_m_width){
        // & current page is over for use
        // Return invalid position
        char buf[100];
        sprintf(buf, "Current y: %d, max height: %d", curPos.y + g_line_spacing + curMaxHeight, p_height - v_m_width);
        LOG_EVENT(buf);
        curPos.x        = v_m_width;
        curPos.y        = h_m_width;
        lastBaseline    = curBaseline;
        curBaseline     = 0;
        curMaxHeight    = 0;
        return LO_NEW_PAGE;
    }
    else{
        curPos.x = h_m_width;
        lastMaxHeight   = curMaxHeight;
        curMaxHeight    = 0;
        lastBaseline    = curBaseline;
        curBaseline     = 0;
        return LO_OK;
    }
}

void LayoutManager::NewPage(){
    curPos.x = v_m_width;
    curPos.y = h_m_width;
    if (0 == curMaxHeight){
        uint32 lineH = g_line_spacing + curMaxHeight;
        lineH = (lineH > line->GetHeight()) ? lineH : line->GetHeight();
        curPos.y += lineH;
    }
    else{
        curPos.y += curMaxHeight + g_line_spacing;
        lastMaxHeight   = curMaxHeight;
        curMaxHeight    = 0;
    }
    lastBaseline    = curBaseline;
    curBaseline     = 0;
}

void LayoutManager::Reset(){
    curPos.x = v_m_width;
    curPos.y = h_m_width;

    curMaxHeight = 0;
    lastMaxHeight= 0;
    curBaseline  = 0;
    lastBaseline = 0;
}
