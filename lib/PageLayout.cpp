#include "Common.h"
#include "Line.h"
#include "Logger.h"
#include "PageLayout.h"
#include "FontsManager.h"
#include "RenderMan.h"

PageLayout::PageLayout(int w, int h, int mv, int mh, 
                       Logger* log, RenderMan * r, int ls, int ws):
    LayoutManager(w, h, mv, mh, log, ls, ws), imageConp(0), render(r)
{
    curLine = new Line(log, this, w, mv);
}

PageLayout::~PageLayout(){
    delete curLine;
}

LAYOUT_RET PageLayout::GetCharPos(Position & pos, int width, int height, int bearingY){
    // Setting up baseline and maxheight
    curBaseline =  (curBaseline > bearingY) ? curBaseline : bearingY;
    int delta = height - bearingY;
    curMaxHeight = (curMaxHeight > curBaseline + delta) ? curMaxHeight : curBaseline + delta;

    int Xoff;
    if (firstLine){
        Xoff = curLine->GetIndent() + g_word_spacing + width;
    }
    else{
        Xoff = g_word_spacing + width;
    }
    int Yoff = g_line_spacing + curMaxHeight;
    Yoff = (Yoff > curLine->GetHeight()) ? Yoff : curLine->GetHeight();

    if (curPos.x + Xoff >= p_width - h_m_width){
        // Current line is over for use
        char buf[100];
        sprintf(buf, "Current x: %d, y: %d, max height: %d", curPos.x + Xoff, curPos.y + Yoff, p_height - v_m_width);
        LOG_EVENT(buf);
        if (curPos.y + Yoff + curMaxHeight >= p_height - v_m_width){
            // & current page is over for use
            // Return invalid position
            curPos.x        = v_m_width;
            curPos.y        = h_m_width;
            lastBaseline    = curBaseline;
            curBaseline     = 0;
            curMaxHeight    = 0;
            pos.x           = -1;
            pos.y           = -1;
            imageConp       = 0;

            curLine->DrawFlush(render);
            Line* lastLine = curLine;
            curLine = lastLine->Dup();
            delete lastLine;

            return LO_NEW_PAGE;
        }
        else{
            // Return position of new line head
            curPos.x        = h_m_width;
            curPos.y        += Yoff;
            lastBaseline    = curBaseline - imageConp;
            curBaseline     = bearingY;
            lastMaxHeight   = curMaxHeight;
            curMaxHeight    = height;
            pos             = curPos;
            pos.y           += height;
            curPos.x        += width + g_word_spacing;
            imageConp       = 0;

            firstLine= false;

            curLine->DrawFlush(render);
            Line* lastLine = curLine;
            curLine = lastLine->Dup();
            delete lastLine;

            return LO_NEW_LINE;
        }
    }
    else
    {
        // Current line still have space, return curPos
        pos      = curPos;
        if (firstLine){
            // Append indent offset
            pos.x += curLine->GetIndent();
        }
        pos.y    += height;
        curPos.x += width + g_word_spacing;
        return LO_OK;
    }
}

LAYOUT_RET PageLayout::GetGraphPos(Position & pos, int width, int height){
    if (curMaxHeight < height){
        imageConp = height - curMaxHeight;
        curMaxHeight = height;
    }
    int Xoff;
    if (firstLine){
        Xoff = curLine->GetIndent() + g_word_spacing + width;
    }
    else{
        Xoff = g_word_spacing + width;
    }
    int Yoff = g_line_spacing + curMaxHeight;
    Yoff = (Yoff > curLine->GetHeight()) ? Yoff : curLine->GetHeight();

    if (curPos.y + Yoff + curMaxHeight >= p_height - v_m_width){
        // & current page is over for use
        // Return invalid position
        char buf[100];
        sprintf(buf, "Current x: %d, y: %d, max height: %d", curPos.x + Xoff, curPos.y + Yoff, p_height - v_m_width);
        LOG_EVENT(buf);

        curPos.x        = v_m_width;
        curPos.y        = h_m_width;
        lastBaseline    = curBaseline;
        curBaseline     = 0;
        curMaxHeight    = 0;
        pos.x           = -1;
        pos.y           = -1;
        imageConp       = 0;
//        pos             = curPos;

        curLine->DrawFlush(render);
        Line* lastLine = curLine;
        curLine = lastLine->Dup();
        delete lastLine;

        return LO_NEW_PAGE;
    }
    else if (curPos.x + Xoff >= p_width - h_m_width){
        // Current line is over for use
        char buf[100];
        sprintf(buf, "Current x: %d, y: %d, max height: %d", curPos.x + Xoff, curPos.y + Yoff, p_height - v_m_width);
        LOG_EVENT(buf);
        
        // Return position of new line head
        curPos.x        = h_m_width;
        curPos.y        += Yoff;
        lastBaseline    = curBaseline;
        curBaseline     = 0;
        lastMaxHeight   = curMaxHeight - imageConp;
        curMaxHeight    = height;
        pos             = curPos;
        pos.y           += height;
        curPos.x        += width + g_word_spacing;
        imageConp       = 0;

        firstLine= false;

        curLine->DrawFlush(render);
        Line* lastLine = curLine;
        curLine = lastLine->Dup();
        delete lastLine;

        return LO_NEW_LINE;
    }
    else
    {
        // Current line still have space, return curPos
        pos      = curPos;
        if (firstLine){
            // Append indent offset
            pos.x += curLine->GetIndent();
        }
        pos.y    += height;
        curPos.x += width + g_word_spacing;
        return LO_OK;
    }
}

LAYOUT_RET PageLayout::GetTablePos(Position & pos, int width, int height){
    if (curMaxHeight < height){
        imageConp = height - curMaxHeight;
        curMaxHeight = height;
    }
    int Xoff;
    if (firstLine){
        Xoff = curLine->GetIndent() + g_word_spacing + width;
    }
    else{
        Xoff = g_word_spacing + width;
    }
    int Yoff = g_line_spacing + curMaxHeight;
    Yoff = (Yoff > curLine->GetHeight()) ? Yoff : curLine->GetHeight();

    if (curPos.y + Yoff + curMaxHeight >= p_height - v_m_width){
        // & current page is over for use
        // Return invalid position
        char buf[100];
        sprintf(buf, "Current x: %d, y: %d, max height: %d", curPos.x + Xoff, curPos.y + Yoff, p_height - v_m_width);
        LOG_EVENT(buf);

        curPos.x        = v_m_width;
        curPos.y        = h_m_width;
        lastBaseline    = curBaseline;
        curBaseline     = 0;
        curMaxHeight    = 0;
        pos.x           = -1;
        pos.y           = -1;
        imageConp       = 0;
//        pos             = curPos;

        curLine->DrawFlush(render);
        Line* lastLine = curLine;
        curLine = lastLine->Dup();
        delete lastLine;

        return LO_NEW_PAGE;
    }
    else if (curPos.x + Xoff >= p_width - h_m_width){
        // Current line is over for use
        char buf[100];
        sprintf(buf, "Current x: %d, y: %d, max height: %d", curPos.x + Xoff, curPos.y + Yoff, p_height - v_m_width);
        LOG_EVENT(buf);
        
        // Return position of new line head
        curPos.x        = h_m_width;
        curPos.y        += Yoff;
        lastBaseline    = curBaseline;
        curBaseline     = 0;
        lastMaxHeight   = curMaxHeight - imageConp;
        curMaxHeight    = height;
        pos             = curPos;
        pos.y           += height;
        curPos.x        += width + g_word_spacing;
        imageConp       = 0;

        firstLine= false;

        curLine->DrawFlush(render);
        Line* lastLine = curLine;
        curLine = lastLine->Dup();
        delete lastLine;

        return LO_NEW_LINE;
    }
    else
    {
        // Current line still have space, return curPos
        pos      = curPos;
        if (firstLine){
            // Append indent offset
            pos.x += curLine->GetIndent();
        }
        pos.y    += height;
        curPos.x += width + g_word_spacing;
        return LO_OK;
    }
}
LAYOUT_RET PageLayout::NewLine(){
    firstLine = true;

    int Yoff = g_line_spacing;
    if (0 == curMaxHeight){ 
        Yoff += lastMaxHeight; 
    }
    else{ 
        Yoff += curMaxHeight; 
        Yoff = (Yoff > curLine->GetHeight()) ? Yoff : curLine->GetHeight();
    }
    curPos.y += Yoff;

    LAYOUT_RET ret = LO_OK;

    if (curPos.y >= p_height - v_m_width){
        // & current page is over for use
        // Return invalid position
        char buf[100];
        sprintf(buf, "Current y: %d, max height: %d", curPos.y, p_height - v_m_width);
        LOG_EVENT(buf);
        curPos.x        = v_m_width;
        curPos.y        = h_m_width;
        lastBaseline    = curBaseline;
        curBaseline     = 0;
        curMaxHeight    = 0;
        imageConp       = 0;
       
        ret = LO_NEW_PAGE;
    }
    else{
        curPos.x = h_m_width;
        lastMaxHeight   = curMaxHeight - imageConp;
        curMaxHeight    = 0;
        lastBaseline    = curBaseline;
        curBaseline     = 0;
        imageConp       = 0;
       
        ret = LO_OK;
    }
    curLine->DrawFlush(render);
    Line* lastLine = curLine;
    curLine = lastLine->Dup();
    delete lastLine;

    return ret;
}

void PageLayout::NewPage(){
    curPos.x = v_m_width;
    curPos.y = h_m_width;
    uint32 Yoff = g_line_spacing + curMaxHeight - imageConp;
    Yoff = (Yoff > curLine->GetHeight()) ? Yoff : curLine ->GetHeight();
    if (0 == curMaxHeight){
        curPos.y += Yoff;
    }
    else{
        curPos.y += Yoff;
        lastMaxHeight   = curMaxHeight - imageConp;
        curMaxHeight    = 0;
        imageConp       = 0;
    }
    lastBaseline    = curBaseline;
    curBaseline     = 0;
}

void PageLayout::Reset(){
    imageConp= 0;
    curPos.x = v_m_width;
    curPos.y = h_m_width;

    curMaxHeight = 0;
    lastMaxHeight= 0;
    curBaseline  = 0;
    lastBaseline = 0;
}
