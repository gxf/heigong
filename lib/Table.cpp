#include "Logger.h"
#include "Color.h"
#include "Table.h"
#include "TableLayout.h"
#include "PageLayout.h"
#include "RenderMan.h"
#include <algorithm>

uint32 Table_Data_Cell::magic_num = 't' + 'r';
uint32 Table_Row::magic_num = 't' + 'd' + 'c';
uint32 Table::magic_num = 't' + 'a' + 'b' + 'l' + 'e';

Table_Data_Cell::Table_Data_Cell(Logger * log, uint32 w, uint32 o):
    Glyph(log), width(w), xoff(o),
    borderPos(0, 0), borderSize(1),
    cellLayout(w, 0, TABLE_MARGIN_VERTICAL, TABLE_MARGIN_HORIZONTAL, log){
}

Table_Data_Cell::~Table_Data_Cell(){
}

Table_Row::Table_Row(Logger * log, uint32 w, uint32 o):
    Glyph(log), xoff(o), width(w), height(0),
    borderPos(0, 0), borderSize(1)
{
}

Table_Row::~Table_Row(){
    std::vector<Table_DC*>::iterator itr = dataCells.begin();
    while(itr != dataCells.end()){
//        delete (*itr);
        ++itr;
    }
}

Table::Table(Logger* log):
    Glyph(log), xoff(0),
    width(0), col(0), row(0), border(1),
    height(0), rowSplit(0)
{}

Table::~Table(){
    std::vector<Table_Row *>::iterator itr = rows.begin();
    while(itr != rows.end()){
        delete (*itr);
        ++itr;
    }
    rows.clear();
}

/*************************************/
// Table data cell 
/*************************************/

uint32 Table_Data_Cell::term_magic_delay = 't' + 'e' + 'r' + 'm' + 't' + 'd' + 'c' + 'd' + 'e' + 'l' + 'a' + 'y';
uint32 Table_Data_Cell::term_magic = 't' + 'e' + 'r' + 'm' + 't' + 'd' + 'c';

Glyph::GY_ST_RET Table_Data_Cell::Setup(LayoutManager& lo){
    std::deque<Glyph*>::iterator itr = glyphBuffer.begin();
    while(itr != glyphBuffer.end()){
        (*itr) -> Setup(lo);
        ++itr;
    }
    TableLayout & tlo = dynamic_cast<TableLayout &>(lo);
    tlo.curLine->RelocLine();
    return GY_OK;
}

bool Table_Data_Cell::Draw(RenderMan& render){
    std::deque<Glyph*>::iterator itr = glyphBuffer.begin();
    while(itr != glyphBuffer.end()){
        (*itr)->Draw(render);
        ++itr;
    }
    return DrawBorder(render);
}

bool Table_Data_Cell::DrawBorder(RenderMan& render){
    // Render column seperator
    Color col(255, 255, 255);
#ifndef NOGL
    render.RenderVerticLine(borderPos.x, borderPos.y, borderSize, borderHeight, col);
    render.RenderVerticLine(borderPos.x + width, borderPos.y, borderSize, borderHeight, col);
#else
    render.RenderVerticLine(borderPos.x, borderPos.y - borderHeight, borderSize, borderHeight, col);
    render.RenderVerticLine(borderPos.x + width, borderPos.y - borderHeight, borderSize, borderHeight, col);
#endif
    return true;
}

bool Table_Data_Cell::Relocate(int x, int y){
    borderPos.x = x + xoff;
    borderPos.y = y;

    std::deque<Glyph*>::iterator itr = glyphBuffer.begin();
//    LOG_EVENT_STR3("[TDC] Relocate to pos: ", x + xoff, y);
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

void Table_Data_Cell::Invalidate(){
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
    std::deque<Glyph*>::iterator itr = glyphBuffer.begin();
    while(itr != glyphBuffer.end()){
        Char * pch = dynamic_cast<Char *>(*itr);
        if (pch)
            pch->valid = true;
        ++itr;
    }
    cellLayout.Reset();
    return this;
}

void Table_Data_Cell::Serialize(std::ofstream & ofs){
    SER_OBJ(magic_num);
    SER_OBJ(width);
    SER_OBJ(xoff);
    SER_OBJ(borderPos);
    SER_OBJ(borderSize);
    SER_OBJ(borderHeight);
    SER_OBJ(glyphAttrib);
    SER_OBJ(lineAttrib);

    std::queue<Glyph*> tmpQueue(delayedToken);
    while(!tmpQueue.empty()){
        tmpQueue.front()->Serialize(ofs);
        tmpQueue.pop();
    }
    
    SER_OBJ(term_magic_delay);

    std::deque<Glyph*>::iterator itr = glyphBuffer.begin();
    while (itr != glyphBuffer.end()){
        (*itr) -> Serialize(ofs);
        ++itr;
    }
    SER_OBJ(term_magic);
}

void Table_Data_Cell::Deserialize(std::ifstream & ifs){
    DESER_OBJ(width);
    DESER_OBJ(xoff);
    DESER_OBJ(borderPos);
    DESER_OBJ(borderSize);
    DESER_OBJ(borderHeight);
    DESER_OBJ(glyphAttrib);
    DESER_OBJ(lineAttrib);

    cellLayout.ReSetup(width, 0, TABLE_MARGIN_VERTICAL, TABLE_MARGIN_HORIZONTAL, logger);

    // Dummy objects
    Char    ch(logger);
    Graph   g(logger);
    Eof     e(logger);

    bool finished = false;

    uint32 magic;
    while(!finished){
        DESER_OBJ(magic);
        if (magic == term_magic_delay){
            finished = true;
        }
        else if (magic == e.GetMagic()){
            Eof * pe = new Eof(logger);
            delayedToken.push(pe);
            pe->Deserialize(ifs);
        }
        else if (magic == ch.GetMagic()){
            Char* pch = new Char(logger);
            delayedToken.push(pch);
            pch->Deserialize(ifs);
        }
        else if (magic == g.GetMagic()){
            Graph* pg = new Graph(logger);
            delayedToken.push(pg);
            pg->Deserialize(ifs);
        }
        else{
            LOG_ERROR("Unsupported class magic.");
        }
    }

    finished = false;
    while(!finished){
        DESER_OBJ(magic);
        if (magic == term_magic){
            finished = true;
        }
        else if (magic == e.GetMagic()){
            Eof * pe = new Eof(logger);
            glyphBuffer.push_back(pe);
            pe->Deserialize(ifs);
        }
        else if (magic == ch.GetMagic()){
            Char* pch = new Char(logger);
            glyphBuffer.push_back(pch);
            pch->Deserialize(ifs);
        }
        else if (magic == g.GetMagic()){
            Graph* pg = new Graph(logger);
            glyphBuffer.push_back(pg);
            pg->Deserialize(ifs);
        }
        else{
            LOG_ERROR("Unsupported class magic.");
        }
    }
}

/*************************************/
// Table rows
/*************************************/
uint32 Table_Row::term_magic = 't' + 'e' + 'r' + 'm' + 't' + 'a' + 'b' + 'l' + 'e' + 'r' + 'o' + 'w';

Glyph::GY_ST_RET Table_Row::Setup(LayoutManager& lo){
    std::vector<Table_DC*>::iterator itr = dataCells.begin();
    while(itr != dataCells.end()){
        (*itr) -> Setup((*itr)->cellLayout);
        height = (height > (*itr)->GetHeight()) ? 
                  height : (*itr)->GetHeight();
        ++itr;
    }
    for(itr = dataCells.begin(); itr != dataCells.end();++itr){
        (*itr) -> SetBorderHeight(height);
    }

//    LOG_EVENT_STR2("[TABLE_ROW] Row height: ", height);
    Position pos;
    PageLayout & plo = dynamic_cast<PageLayout &>(lo);
    if (LO_NEW_PAGE == plo.GetTablePos(pos, height)){
        return GY_NEW_PAGE;
    }
    else{
//        LOG_EVENT_STR3("[TABLE_ROW] Get table row at position: ", pos.x, pos.y);
        Relocate(pos.x + xoff, pos.y);
        borderPos.x = pos.x + xoff;
        borderPos.y = pos.y;
        return GY_OK;
    }
}

bool Table_Row::Draw(RenderMan& render){
    std::vector<Table_DC*>::iterator itr = dataCells.begin();
    while(itr != dataCells.end()){
        (*itr)->Draw(render);
        ++itr;
    }
    return DrawBorder(render);
}

bool Table_Row::DrawBorder(RenderMan& render){
    // Render row seperator
    Color col(255, 255, 255);
    render.RenderHorizLine(borderPos.x, borderPos.y, borderSize, width, col);
    render.RenderHorizLine(borderPos.x, borderPos.y + (int)height, borderSize, width, col);
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

void Table_Row::Invalidate(){
    std::vector<Table_Data_Cell*>::iterator itr = dataCells.begin();
    while(itr != dataCells.end()){
        (*itr)->Invalidate();
        ++itr;
    }
}

Glyph* Table_Row::Dup(){
    Table_R * tr = new Table_R(logger, xoff, width);
//    tr->height   = this -> height;
    tr->xoff    = this->xoff;
    tr->width   = this->width;
    tr->height  = 0;

    std::vector<Table_DC*>::iterator itr= dataCells.begin();
    while(dataCells.end() != itr){
        tr->AddTD(dynamic_cast<Table_DC*>((*itr)->Dup()));
//        tr->AddTD(*itr);
        ++itr;
    }

    return tr;
}

void Table_Row::Serialize(std::ofstream & ofs){
    SER_OBJ(magic_num);
    SER_OBJ(xoff);
    SER_OBJ(width);
    SER_OBJ(height);
    SER_OBJ(borderPos);
    SER_OBJ(borderSize);

    Table_R tdc(logger, width, xoff);
    std::vector<Table_DC*>::iterator itr= dataCells.begin();
    while(dataCells.end() != itr){
        (*itr)->Serialize(ofs);
        ++itr;
    }
    SER_OBJ(term_magic);
}

void Table_Row::Deserialize(std::ifstream & ifs){
    DESER_OBJ(xoff);
    DESER_OBJ(width);
    DESER_OBJ(height);
    DESER_OBJ(borderPos);
    DESER_OBJ(borderSize);

    uint32 magic;

    Table_DC tdc(logger, width, xoff);

    bool finished = false;

    while(!finished){
        DESER_OBJ(magic);
        if (magic == term_magic){
            finished = true;
        }
        else if(magic == tdc.GetMagic()){
            Table_DC * ptdc = new Table_DC(logger, width, xoff);
            ptdc -> Deserialize(ifs);
            AddTD(ptdc);
        }
        else{
            LOG_ERROR("Unsupported table magic value.");
        }
    }
}

/*************************************/
// Table
/*************************************/
uint32 Table::term_magic = 't' + 'e' + 'r' + 'm' + 't' + 'a' + 'b' + 'l' + 'e';

bool Table::Draw(RenderMan& render){
    uint32 r = 0;
    std::vector<Table_Row *>::iterator itr = rows.begin();
    while(++r <= rowSplit && itr != rows.end()){
        if (false == (*itr) -> Draw(render)){
            return false;
        }
//        LOG_EVENT("[TABLE] Draw called.");
        ++itr;
    }
    return true;
}

bool Table::Relocate(int x, int y){
#if 0
    uint32 r = 0;
    std::vector<Table_Row *>::iterator itr = rows.begin();
    while(++r <= rowSplit && itr != rows.end()){
        (*itr) -> Relocate(x, y);
        ++itr;
    }
#endif
    return true;
}

Glyph::GY_ST_RET Table::Setup(LayoutManager& lo){
    // Break rows into two pages if it is too long
    std::vector<Table_Row *>::iterator itr = rows.begin();
    while(itr != rows.end()){
        if (GY_NEW_PAGE == (*itr) -> Setup(lo)){
            if (rowSplit > 0)
                --rowSplit;
//            LOG_EVENT_STR2("[TABLE] Page ends.Row is splitted @ ", rowSplit);
            return GY_NEW_PAGE;
        }
        height = (height > (*itr)->height) ? height : (*itr)->height;
        ++itr;
        ++rowSplit;
    }
    lo.AddGlyph(this);
    return GY_OK;
}

Glyph* Table::UngetSet(){
    uint32 r = 0;
    std::vector<Table_Row *>::iterator itr = rows.begin();
    while(r++ < rowSplit && itr != rows.end()){
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

void Table::Invalidate(){
    std::vector<Table_Row *>::iterator itr = rows.begin();
    while(itr != rows.end()){
        (*itr)->Invalidate();
        ++itr;
    }
}

Glyph* Table::Dup(){
    Table *t = new Table(logger);

    t->xoff     = this->xoff;
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

void Table::Serialize(std::ofstream & ofs){
    SER_OBJ(magic_num);
    SER_OBJ(xoff);
    SER_OBJ(width);
    SER_OBJ(col);
    SER_OBJ(row);
    SER_OBJ(border);
    SER_OBJ(height);
    SER_OBJ(rowSplit);

    std::vector<Table_Row *>::iterator itr = rows.begin();
    while(itr != rows.end()){
        (*itr) -> Serialize(ofs);
        ++itr;
    }

    SER_OBJ(term_magic);
}

void Table::Deserialize(std::ifstream & ifs){
    DESER_OBJ(xoff);
    DESER_OBJ(width);
    DESER_OBJ(col);
    DESER_OBJ(row);
    DESER_OBJ(border);
    DESER_OBJ(height);
    DESER_OBJ(rowSplit);

    Table_R tr(logger, width, xoff);

    uint32 magic;
    bool finished = false;

    while(!finished){
        DESER_OBJ(magic);
        if (magic == term_magic){
            finished = true;
        }
        else if (magic == tr.GetMagic()){
            Table_R * ptr = new Table_R(logger, width, xoff);
            ptr -> Deserialize(ifs);
            AddTR(ptr);
        }
        else{
            LOG_ERROR("Unsupported table magic value.");
        }
    }
}

