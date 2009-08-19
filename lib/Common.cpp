#include "Common.h"


std::ifstream & operator>>(std::ifstream & ifs, Position & pos){
    ifs >> pos.x;
    ifs >> pos.y;
    return ifs;
}

std::ofstream & operator<<(std::ofstream & ofs, Position & pos){
    ofs << pos.x;
    ofs << pos.y;
    return ofs;
}

std::ifstream & operator>>(std::ifstream & ifs, Attrib_Glyph & ag){
    ifs >> ag.bold;
    ifs >> ag.italic;
    ifs >> ag.size;
//    ifs >> ag.font;
    return ifs;
}

std::ofstream & operator<<(std::ofstream & ofs, Attrib_Glyph & ag){
    ofs << ag.bold;
    ofs << ag.italic;
    ofs << ag.size;
//    ofs << ag.font;
    return ofs;
}

std::ifstream & operator>>(std::ifstream & ifs, Attrib_Line & al){
    uint32 tmp;
    ifs >> tmp;
    al.align = (ALIGNMENT)tmp;
    ifs >> al.indent;
    ifs >> al.height;
    return ifs;
}

std::ofstream & operator<<(std::ofstream & ofs, Attrib_Line & al){
    ofs << (uint32)al.align;
    ofs << al.indent;
    ofs << al.height;
    return ofs;
}
