#include "Glyph.h"
#include "RenderMan.h"

Table::Table(Logger* log):
    Glyph(log), width(0), 
{}

bool Table::Draw(RenderMan* render){
    return true;
}

bool Table::Relocate(int x, int y){
    return true;
}

bool Table::Setup(Context * ctx){
    return true;
}

Glyph* Table::Dup(){
    return NULL;
}
