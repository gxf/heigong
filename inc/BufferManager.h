#ifndef BUFFER_MANAGER
#define BUFFER_MANAGER

#include "Common.h"
#include <map>
#include <cstring>

class BufferManager{
    public:
        BufferManager(){}
        ~BufferManager(){}

    public:
        class BufAttr{
            public:
                BufAttr(uint32 w, uint32 h, uint32 d):
                    width(w), height(h), depth(d)
                {}

            public:
                uint32  width;
                uint32  height;
                uint32  depth;
        };

    public:
        inline void* Insert(void* p, uint32 width, uint32 height, uint32 depth){
            char* pp = new char[width * height * depth / 8];
            std::memcpy(pp, p, width * height * depth / 8);

            BufAttr attr(width, height, depth);
            buffers.insert(std::make_pair(pp, attr));
            return pp;
        }

        inline bool GetAttr(void* p, uint32* width, uint32* height, uint32* depth){
            std::map<void*, BufAttr>::iterator itr;
            if(buffers.end() == (itr = buffers.find(p))){
                *width  = 0;
                *height = 0;
                *depth  = 0;
                return false;
            }
            *width  = itr->second.width;
            *height = itr->second.height;
            *depth  = itr->second.depth;
            return true;
        }

        inline bool Delete(void* p){
            std::map<void*, BufAttr>::iterator itr;
            if(buffers.end() == (itr = buffers.find(p))){
                return false;
            }
            buffers.erase(itr);
            delete [] (char*)p;
            return true;
        }

    private:
        std::map<void*, BufAttr> buffers;
};

#endif
