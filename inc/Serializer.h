#ifndef SERIALIZER_H
#define SERIALIZER_H

#include <fstream>

class Serializer{
    public:
        virtual void Serialize(std::ofstream & ofs) = 0;
        virtual void Deserialize(std::ifstream & ifs) = 0;
};

#define SER_OBJ(obj) ofs.write((char*)&obj, sizeof(obj));
#define DESER_OBJ(obj) ifs.read((char*)&obj, sizeof(obj));

#endif
