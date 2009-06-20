#include "Logger.h"
#include "Table.h"
#include "TableLayout.h"
#include "PageLayout.h"
#include "RenderMan.h"
#include <algorithm>

Table_Data_Cell::Table_Data_Cell(Logger * log, uint32 w, uint32 o):
    Glyph(log), width(w), xoff(o),
    cellLayout(w, 0, TABLE_MARGIN_VERTICAL, TABLE_MARGIN_HORIZONTAL, log){
}

Table_Data_Cell::~Table_Data_Cell(){
}

Table_Row::Table_Row(Logger * log, uint32 w):
    Glyph(log), width(w), height(0)
{
}

Table_Row::~Table_Row(){
}

Table::Table(Logger* log):
    Glyph(log), 
    width(0), col(0), row(0), border(1),
    height(0), rowSplit(0)
{}

Table::~Table(){
}

/*************************************/
// Table data cell 
/*************************************/

bool Table_Data_Cell::Setup(LayoutManager& lo){
    std::deque<Glyph*>::iterator itr = glyphBuffer.begin();
    while(itr != glyphBuffer.end()){
        (*itr) -> Setup(lo);
        ++itr;
    }
    return true;
}

bool Table_Data_Cell::Draw(RenderMan& render){
    std::deque<Glyph*>::iterator itr = glyphBuffer.begin();
    while(itr != glyphBuffer.end()){
        (*itr)->Draw(render);
        ++itr;
    }
    return true;
}

bool Table_Data_Cell::Relocate(int x, int y){
    std::deque<Glyph*>::iterator itr = glyphBuffer.begin();
    while(itr != glyphBuffer.end()){
        (*itr)->Relocate(x + xoff, y);
        ++itr;
    }
    return true;
}

Glyph* Table_Data_Cell::UngetSet(){
//    cellLayout.Reset();
    return this;
}

Glyph* Table_Data_Cell::Dup(){
#if 0
    Table_DC * tdc = new Table_DC(logger, width, xoff);
    tdc->glyphAttrib = this->glyphAttrib;
    tdc->lineAttrib  = this->lineAttrib;
//    tdc->cellLayout  = this->cellLayout;
    while(!(this->delayedToken.empty())){
        tdc->delayedToken.push(this->delayedToken.front()->UngetSet());
        this->delayedToken.pop();
    }
    std::copy(this->glyphBuffer.begin(), 
              this->glyphBuffer.end(), 
              tdc->glyphBuffer.begin());
    while(!(glyphBuffer.empty())){
        tdc->glyphBuffer.push_back(this->glyphBuffer.front()->UngetSet());
        this->glyphBuffer.pop_front();
    }
    return tdc;
#endif
    cellLayout.Reset();
    return this;
}

/*************************************/
// Table rows
/*************************************/
bool Table_Row::Setup(LayoutManager& lo){
    std::vector<Table_DC*>::iterator itr = dataCells.begin();
    while(itr != dataCells.end()){
        (*itr) -> Setup((*itr)->cellLayout);
        height = (height > (*itr)->GetHeight()) ? 
                  height : (*itr)->GetHeight();
        ++itr;
    }
    LOG_EVENT_STR2("[TABLE_ROW] Row height: ", height);
    Position pos;
    PageLayout & plo = dynamic_cast<PageLayout &>(lo);
    if (LO_NEW_PAGE == plo.GetTablePos(pos, height)){
        LOG_EVENT_STR3("Get table row at position: ", pos.x, pos.y);
        return false;
    }
    else{
        LOG_EVENT_STR3("Get table row at position: ", pos.x, pos.y);
        Relocate(pos.x, pos.y);
        return true;
    }
}

bool Table_Row::Draw(RenderMan& render){
    std::vector<Table_DC*>::iterator itr = dataCells.begin();
    while(itr != dataCells.end()){
        (*itr)->Draw(render);
        ++itr;
    }
    return true;
}

bool Table_Row::Relocate(int x, int y){
    std::vector<Table_DC*>::iterator itr = dataCells.begin();
    while(itr != dataCells.end()){
        (*itr)->Relocate(x, y);
        ++itr;
    }
    return true;
}

Glyph* Table_Row::UngetSet(){
    std::vector<Table_DC*>::iterator itr= dataCells.begin();
    while(dataCells.end() != itr){
        (*itr)->UngetSet();
        ++itr;
    }
    return this;
}

Glyph* Table_Row::Dup(){
    Table_R * tr = new Table_R(logger, width);
//    tr->height   = this -> height;
    tr->height   = 0;

    std::vector<Table_DC*>::iterator itr= dataCells.begin();
    while(dataCells.end() != itr){
        tr->AddTD(dynamic_cast<Table_DC*>((*itr)->Dup()));
//        tr->AddTD(*itr);
        ++itr;
    }

    return tr;
}

/*************************************/
// Table
/*************************************/
bool Table::Draw(RenderMan& render){
    uint32 r = 0;
    std::vector<Table_Row *>::iterator itr = rows.begin();
    while(++r <= rowSplit && itr != rows.end()){
        if (false == (*itr) -> Draw(render)){
            return false;
        }
        LOG_EVENT("[TABLE] Draw called.");
        ++itr;
    }
    return true;
}

bool Table::Relocate(int x, int y){
    uint32 r = 0;
    std::vector<Table_Row *>::iterator itr = rows.begin();
    while(++r <= rowSplit && itr != rows.end()){
        (*itr) -> Relocate(x, y);
        ++itr;
    }
    return true;
}

bool Table::Setup(LayoutManager& lo){
    // Break rows into two pages if it is too long
    std::vector<Table_Row *>::iterator itr = rows.begin();
    while(itr != rows.end()){
        if (false == (*itr) -> Setup(lo)){
            --rowSplit;
            LOG_EVENT_STR2("[TABLE] Page ends.Row is splitted @ ", rowSplit);
            return false;
        }
        height = (height > (*itr)->height) ? height : (*itr)->height;
        ++itr;
        ++rowSplit;
    }
    return true;
}

Glyph* Table::UngetSet(){
    uint32 r = 0;
    std::vector<Table_Row *>::iterator itr = rows.begin();
    while(r++ < rowSplit){
        ++itr;
    }
    Table *t = new Table(logger);

    std::vector<Table_Row *>::iterator itra(itr);
    while(itr != rows.end()){
        t -> AddTR(*itr);
        (*itr) -> UngetSet();
        ++itr;
    }
    rows.erase(itra, rows.end());

    return t;
}

Glyph* Table::Dup(){
    Table *t = new Table(logger);

    t->width    = this->width;
    t->col      = this->col;
    t->row      = this->row;
    t->border   = this->border;
    t->height   = this->height;
    t->rowSplit = this->rowSplit;

    std::vector<Table_Row *>::iterator itr = rows.begin();
    while(itr != rows.end()){
        t -> AddTR(dynamic_cast<Table_R *>((*itr)->Dup()));
        ++itr;
    }
    return t;
}

