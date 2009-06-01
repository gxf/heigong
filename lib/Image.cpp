#include "Context.h"

bool Image::Setup(Context* ctx){
    return true;
}

bool Image::Draw(RenderMan* render){
    return true;
}

bool Image::AdjustPos(int x, int y){
    return true;
}

Glyph* Image::Dup(){
    return NULL;
}
