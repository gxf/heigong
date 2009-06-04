#include <png.h>
#include "Context.h"
#include <string>

Image::Image(Logger* log):
    Glyph(log), file_name(NULL), file_path(NULL)
{
    file_path = (char*)"./";
}

Image::~Image(){
    if (file_name != NULL){
        delete [] file_name;
    }
}

void Image::SetSrcFile(const char* src) { 
    char buf[100];
    sprintf(buf, "Image file: %s", src);
    LOG_EVENT(buf);
    file_name = new char[std::strlen(src) + 1];
    std::memcpy(file_name, src, std::strlen(src) + 1);
}

bool Image::Setup(Context* ctx){
    std::string file(file_path);
    file += file_name;

    FILE *fp = fopen(file.c_str(), "rb");
    if (!fp) {
//        throw Except_Fail_To_OpenFile();
        char info[100];
        sprintf(info, "Fail to open file %s", file.c_str());
        LOG_ERROR(info);
        return false;
    } 

    size_t number = 8;
    char header[number];
    fread(header, 1, number, fp);
    bool is_png = !png_sig_cmp((png_byte*)header, 0, (png_size_t)number);
    if (!is_png){
        LOG_ERROR("Currently other files than png are not supported!");
        return false; 
    }

//    return SetupPNG(ctx, fp);
    return true;
}

bool Image::SetupPNG(Context* ctx, FILE* fp){
    png_structp png_ptr = 
        png_create_read_struct(PNG_LIBPNG_VER_STRING, (png_voidp)NULL, NULL, NULL);
    if (!png_ptr){
        LOG_ERROR("Fail to create png struct!");
        return false; 
    }

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr){
        png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
        LOG_ERROR("Fail to create png info!");
        return false; 
    } 

    png_infop end_info = png_create_info_struct(png_ptr);
    if (!end_info) {
        png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
        LOG_ERROR("Fail to create png info!");
        return false; 
    }

    if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
        fclose(fp);
        LOG_ERROR("Setjump callback.");
        return false;
    }

    // TODO: Read png from file
    png_read_png(png_ptr, info_ptr, 
                 PNG_TRANSFORM_STRIP_16 | PNG_TRANSFORM_STRIP_ALPHA , 
                 NULL);


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
