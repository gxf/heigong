#include "TableLayout.h"

TableLayout::TableLayout(int w, int h, int mv, int mh, 
                        Logger* log, int ls, int ws):
    LayoutManager(w, h, mv, mh, log, ls, ws)
{
}

TableLayout::~TableLayout(){
}

LAYOUT_RET TableLayout::GetCharPos(Position & pos, int width, int height, int bearingY){
    // TODO
    return LO_OK;
}

LAYOUT_RET TableLayout::NewLine(){
    // TODO
    return LO_OK;
}

void TableLayout::Reset(){
}
