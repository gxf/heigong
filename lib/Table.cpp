#include "Table.h"
#include "TableLayout.h"
#include "RenderMan.h"

Table_Data_Cell::Table_Data_Cell(Logger * log, uint32 w):
    Glyph(log), width(w), rowLayout(w, 0, TABLE_MARGIN_VERTICAL, TABLE_MARGIN_HORIZONTAL, log)
{
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

bool Table_Data_Cell::Setup(Context * ctx){
    return true;
}

bool Table_Data_Cell::Draw(RenderMan* render){
    return true;
}

bool Table_Data_Cell::Relocate(int x, int y){
    return true;
}

Glyph* Table_Data_Cell::Dup(){
    return NULL;
}

bool Table_Row::Setup(Context * ctx){
    return true;
}

bool Table_Row::Draw(RenderMan* render){
    return true;
}

bool Table_Row::Relocate(int x, int y){
    return true;
}

Glyph* Table_Row::Dup(){
    return NULL;
}

bool Table::Draw(RenderMan* render){
    std::vector<Table_Row *>::iterator itr = rows.begin();
    while(itr != rows.end()){
        if (false == (*itr) -> Draw(render)){
            return false;
        }
    }
    return true;
}

bool Table::Relocate(int x, int y){
    std::vector<Table_Row *>::iterator itr = rows.begin();
    while(itr != rows.end()){
        if (false == (*itr) -> Relocate(x, y)){
            return false;
        }
    }
    return true;
}

bool Table::Setup(Context * ctx){
    return true;
}

Glyph* Table::Dup(){
    return NULL;
}

