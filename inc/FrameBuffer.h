#ifndef HG_FRAMEBUFFER_H
#define HG_FRAMEBUFFER_H

#include "Common.h"
#include <cstring>

class FrameBuffer{
    public:
        FrameBuffer(uint32 w, uint32 h, uint32 elem_l = 1) :
            width(w), height(h), elem_len(elem_l)
        {
            pFB = new uint8[width * height * elem_len + FB_GUARD_SIZE];
            std::memset(pFB, 0xff, width * height * elem_len + FB_GUARD_SIZE);
        }

        ~FrameBuffer(){
            delete [] pFB;
        }

    public:
        void Write(Position pos, uint32 w, uint32 h, void* data){
            uint8* p = pFB + (pos.y * width + pos.x) * elem_len;
            uint8* q = (uint8*)data;
            uint32 i, j;
            uint32 pitch = width - w;
            for(i = 0; i < h; i++){
                for(j = 0; j < w; j++){
                    std::memcpy(p, q, elem_len);
                    p += elem_len;
                    q += elem_len;
                }
                p += pitch * elem_len;
            }
        }

        void GetFB(uint8** p, uint32* w, uint32* h){
            *p = pFB;
            *w = width;
            *h = height;
        }

        void Clear(){
            std::memset(pFB, 0xff, width * height * elem_len + FB_GUARD_SIZE);
        }

    private:
        uint32  width;
        uint32  height;
        uint32  elem_len;
        uint8 * pFB;
};

#endif
