#include "Table.h"
#include "RenderMan.h"

Table_Data_Cell::Table_Data_Cell(Logger * log, uint32 w):
    Glyph(log), width(w)
{
}

Table_Row::Table_Row(Logger * log, uint32 w):
    Glyph(log), width(w)
{
}

Table::Table(Logger* log):
    Glyph(log), width(0)
{}

bool Table_Data_Cell::Setup(Context * ctx){
    return true;
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

