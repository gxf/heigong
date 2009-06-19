#include "Table.h"
#include "TableLayout.h"
#include "RenderMan.h"

Table_Data_Cell::Table_Data_Cell(Logger * log, uint32 w, uint32 o):
    Glyph(log), width(w), xoff(o),
    cellLayout(w, 0, TABLE_MARGIN_VERTICAL, TABLE_MARGIN_HORIZONTAL, log){
}

Table_Data_Cell::~Table_Data_Cell(){
}

Table_Row::Table_Row(Logger * log, uint32 w):
    Glyph(log), width(w)
{
}

Table_Row::~Table_Row(){
}

Table::Table(Logger* log):
    Glyph(log), width(0)
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

Glyph* Table_Data_Cell::Dup(){
    return NULL;
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
    return true;
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

Glyph* Table_Row::Dup(){
    return NULL;
}

/*************************************/
// Table
/*************************************/
bool Table::Draw(RenderMan& render){
    std::vector<Table_Row *>::iterator itr = rows.begin();
    while(itr != rows.end()){
        if (false == (*itr) -> Draw(render)){
            return false;
        }
        ++itr;
    }
    return true;
}

bool Table::Relocate(int x, int y){
    std::vector<Table_Row *>::iterator itr = rows.begin();
    while(itr != rows.end()){
        if (false == (*itr) -> Relocate(x, y)){
            return false;
        }
        ++itr;
    }
    return true;
}

bool Table::Setup(LayoutManager& lo){
    std::vector<Table_Row *>::iterator itr = rows.begin();
    while(itr != rows.end()){
        if (false == (*itr) -> Setup(lo)){
            return false;
        }
        height = (height > (*itr)->height) ? 
                  height : (*itr)->height;
        ++itr;
    }
    return true;
}

Glyph* Table::Dup(){
    return NULL;
}

