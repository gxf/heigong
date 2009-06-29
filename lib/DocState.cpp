#include "Logger.h"
#include "DocState.h"
#include "Logger.h"
#include "Glyph.h"
#include "Table.h"
#include <iomanip>


uint32 DocState::offset_magic = 'o' + 'f' + 'f' + 's' + 'e' + 't';

DocState::DocState(Logger* log):
    offset(0), logger(log)
{
    Init();
}

void DocState::Init(){
    // Dummy objects for creating serialize table
#if 0
    Char ch(logger);
    Graph g(logger);
    Table table(logger);

    serialTable.insert(std::make_pair(ch.GetMagic(), "char"));
    serialTable.insert(std::make_pair(g.GetMagic(), "graph"));
    serialTable.insert(std::make_pair(table.GetMagic(), "table"));
    serialTable.insert(std::make_pair(offset_magic, "offset"));
#endif
}

void DocState::StoreState(const char * filename){
    std::ofstream ofs(filename);
    Serialize(ofs);
}

void DocState::RecoverState(const char * filename){
    std::ifstream ifs(filename);
    Deserialize(ifs);
}

void DocState::Serialize(std::ofstream & ofs){
    std::deque<Glyph*>::iterator itr = buffer.begin();
    while (itr != buffer.end()){
        (*itr)->Serialize(ofs);
        ++itr;
    }
    SER_OBJ(offset_magic);
    SER_OBJ(offset);
}

void DocState::Deserialize(std::ifstream & ifs){
    // Dummy objects
    Char ch(logger);
    Graph g(logger);
    Table t(logger);

    uint32 magic;
    while(!ifs.eof()){
        DESER_OBJ(magic);
        if(magic == offset_magic){
            // offset is placed at the end of the serialized record
            DESER_OBJ(offset);
            return;
        }
        else if (magic == ch.GetMagic()){
            Char* pch = new Char(logger);
            buffer.push_back(pch);
            pch->Deserialize(ifs);
        }
        else if (magic == g.GetMagic()){
            Graph* pg = new Graph(logger);
            buffer.push_back(pg);
            pg->Deserialize(ifs);
        }
        else if (magic == t.GetMagic()){
            Table* pt = new Table(logger);
            buffer.push_back(pt);
            pt->Deserialize(ifs);
        }
        else{
            LOG_ERROR("Unsupported class magic.");
        }
    }
}

