#include "Logger.h"
#include "DocState.h"
#include "Logger.h"
#include "Glyph.h"
#include "Table.h"
#include <iomanip>


uint32 DocState::offset_magic = 'o' + 'f' + 'f' + 's' + 'e' + 't';

DocState::DocState(Logger* log):
    offset(0), term_off(0), logger(log)
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
    uint32 fComplete = 0;
    SER_OBJ(fComplete);
    SER_OBJ(offset);
    SER_OBJ(term_off);   // Place holder
    Serialize(ofs);
    ofs.close();
}

void DocState::AppendState(const char * filename){
    std::ofstream ofs(filename, std::ios_base::in | std::ios_base::binary | std::ios_base::ate);
    uint32 fComplete;
    ofs.seekp((sizeof(fComplete) + sizeof(offset)), 
              std::ios_base::beg);  // Skip completion flag & offset
    SER_OBJ(term_off);      // Append info about offset of term
    fComplete = (~0);
    ofs.seekp(0, std::ios_base::beg);
    SER_OBJ(fComplete);     // Write completion flag
    ofs.flush();
    ofs.close();
}

bool DocState::RecoverState(const char * filename){
    std::ifstream ifs(filename);
    uint32 fComplete = 0x0;
    DESER_OBJ(fComplete);
    if (fComplete != ~0){
        LOG_EVENT("Recover state fails.");
        ifs.close();
        return false;
    }
    DESER_OBJ(offset);
    DESER_OBJ(term_off);
    Deserialize(ifs);
    ifs.close();
    return true;
}

void DocState::Serialize(std::ofstream & ofs){
    std::deque<Glyph*>::iterator itr = buffer.begin();
    while (itr != buffer.end()){
        (*itr)->Serialize(ofs);
        ++itr;
    }
    SER_OBJ(offset_magic);
    SER_OBJ(glyphAttrib);
    SER_OBJ(lineAttrib);
}

void DocState::Deserialize(std::ifstream & ifs){
    // Dummy objects
    Char    ch(logger);
    Graph   g(logger);
    Table   t(logger);
    Eof     e(logger);

    uint32 magic;
    while(!ifs.eof()){
        DESER_OBJ(magic);
        if(magic == offset_magic){
            // offset is placed at the end of the serialized record
            DESER_OBJ(glyphAttrib);
            DESER_OBJ(lineAttrib);
            return;
        }
        else if (magic == e.GetMagic()){
            Eof * pe = new Eof(logger);
            buffer.push_back(pe);
            pe->Deserialize(ifs);
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

