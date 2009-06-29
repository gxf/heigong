#ifndef DOC_STATE_H
#define DOC_STATE_H

#include <fstream>
#include <map>
#include <deque>
#include "Common.h"
#include "Serializer.h"

class Logger;
class Glyph;

class DocState : public Serializer{
    public:
        DocState(Logger* log);
        ~DocState();

    public:
        void StoreState(const char * filename);
        void RecoverState(const char * filename);

    public:
        void Serialize(std::ofstream & ofs);
        void Deserialize(std::ifstream & ifs);

    protected:
        void Init();
        
    public:
        std::deque<Glyph*>  buffer;
        long int            offset;

    public:
        std::map<uint32, const char*> serialTable;
        Logger * logger;

    public:
        static uint32 offset_magic;
};

typedef DocState* HDocState;

#endif
