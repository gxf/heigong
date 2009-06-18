#include "Line.h"
#include "Logger.h"
#include "TableLayout.h"

TableLayout::TableLayout(int w, int h, int mv, int mh, 
                         Logger* log, int ls, int ws):
    LayoutManager(w, h, mv, mh, log, ls, ws)
{
    curLine = new Line(log, this, w, mv);
}

TableLayout::~TableLayout(){
    delete curLine;
}

LAYOUT_RET TableLayout::GetCharPos(Position & pos, int width, int height, int bearingY){
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
        // Return position of new line head
        curPos.x        = h_m_width;
        curPos.y        += Yoff;
        lastBaseline    = curBaseline;
        curBaseline     = bearingY;
        lastMaxHeight   = curMaxHeight;
        curMaxHeight    = height;
        pos             = curPos;
        pos.y           += height;
        curPos.x        += width + g_word_spacing;

        firstLine= false;

        curLine->RelocLine();
        curLine->Clear();

        Line* lastLine = curLine;
        curLine = lastLine->Dup();

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

LAYOUT_RET TableLayout::NewLine(){
    // TODO
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

    curPos.x = h_m_width;
    lastMaxHeight   = curMaxHeight;
    curMaxHeight    = 0;
    lastBaseline    = curBaseline;
    curBaseline     = 0;
       
    return LO_OK;
}

void TableLayout::Reset(){
    curPos.x = v_m_width;
    curPos.y = h_m_width;

    curMaxHeight = 0;
    lastMaxHeight= 0;
    curBaseline  = 0;
    lastBaseline = 0;
}
