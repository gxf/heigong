#include <png.h>
#include "Color.h"
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
    if (bitmap != NULL){
//        delete [] (char*)bitmap;
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
        char info[100];
        sprintf(info, "Fail to open file %s", file.c_str());
        LOG_ERROR(info);
        return false;
    } 

    IF_T type = DetectFormat(file.c_str(), fp);

    switch(type){
        case IF_NONE:
            return false;
        case IF_PNG:
            LOG_EVENT("Png file is detected.");
            return SetupPNG(ctx, fp);
        case IF_JPG:
            LOG_EVENT("JPG file is detected.");
            return SetupJPG();
            break;
        case IF_EMF:
            break;
        default:
            LOG_ERROR("Unsupported image file type.");
            break;
    }
    return true;
}

Image::IF_T Image::DetectFormat(const char * str, FILE * fp){
    // Detect if it is PNG file
    size_t number = 8;
    char header[number];
    fread(header, 1, number, fp);
    bool is_png = !png_sig_cmp((png_byte*)header, 0, (png_size_t)number);
    if (is_png){
        rewind(fp);
        return IF_PNG;
    }
    rewind(fp);

    // Detect if it is JPG file
    // Notice: Only check 1st byte here
    uchar8  ch;
    if (fread(&ch, sizeof(ch), 1, fp) == 0){
        throw Except_Fail_To_Read_file();
    }
    if (0xFF == ch || 0xD8 == ch){
        return IF_JPG;
    }
    rewind(fp);

    // Else
    LOG_ERROR("Currently other files than png are not supported!");

    return IF_NONE;
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

    png_init_io(png_ptr, fp);
//    png_set_sig_bytes(png_ptr, 0);

/*    
    png_read_png(png_ptr, info_ptr, 
                 PNG_TRANSFORM_STRIP_16 | PNG_TRANSFORM_STRIP_ALPHA , 
                 NULL);
*/
    png_read_info(png_ptr, info_ptr);

    uchar8 col_type;
    uchar8 bit_depth;
//    bitmap = png_get_rows(png_ptr, info_ptr);

    bitmap_w = png_get_image_width(png_ptr, info_ptr);
    bitmap_h = png_get_image_height(png_ptr, info_ptr);

    col_type = png_get_color_type(png_ptr, info_ptr);
    bit_depth = png_get_bit_depth(png_ptr, info_ptr);

    if (col_type == PNG_COLOR_TYPE_PALETTE){
        png_set_palette_to_rgb(png_ptr);
    }
    if (bit_depth == 16){
        png_set_strip_16(png_ptr);
    }
    if (col_type & PNG_COLOR_MASK_ALPHA){
        png_set_strip_alpha(png_ptr);
    }
    png_color_16 my_background;
    png_color_16p image_background;

    if (png_get_bKGD(png_ptr, info_ptr, &image_background))
        png_set_background(png_ptr, image_background, 
                           PNG_BACKGROUND_GAMMA_FILE, 0, 1.0);
    else
        png_set_background(png_ptr, &my_background, 
                           PNG_BACKGROUND_GAMMA_SCREEN, 1, 1.0);
                
    png_read_update_info(png_ptr, info_ptr);

//    bitmap = new char[bitmap_w * bitmap_h * 4];
    bitmap = new char[800 * 600 * 4];   // Enough to hold one page
    png_read_image(png_ptr, (png_byte**)bitmap);
    png_read_end(png_ptr, end_info);

    col_type = png_get_color_type(png_ptr, info_ptr);
    bit_depth = png_get_bit_depth(png_ptr, info_ptr);
//    bitmap_w = png_get_image_width(png_ptr, info_ptr);
//    bitmap_h = png_get_image_height(png_ptr, info_ptr);
//    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);

    Convert(&bitmap, bitmap_w, bitmap_h, col_type, bit_depth);

    LAYOUT_RET ret;
    ret = ctx->layout.GetImagePos(pos, bitmap_w, bitmap_h);
//    ret = ctx->layout.GetImagePos(pos, req_width, req_height);

    switch(ret){
        case LO_OK:
            ctx->line.AddGlyph(this);
            break;
        case LO_NEW_LINE:
            ctx->line.DrawFlush(ctx);
            ctx->line.AddGlyph(this);
            break;
        case LO_NEW_PAGE:
            ctx->line.DrawFlush(ctx);
            ctx->layout.Reset();
            return false;
        default:
            LOG_ERROR("Unsupported Layout return.");
            break;
    }

    return true;
}

void Image::Convert(void** bmap, int w, int h, uchar8 col_t, uchar8 bit_depth){
    // Convert image from RGB to Greyscale
    printf("Bit depth: %d\n", (int)bit_depth);
    uchar8 * nbmap = NULL;
    Color* p = (Color*)*bmap;
    Color_A * pa = (Color_A*)*bmap;
    int i = 0, j = 0;
    switch(col_t){ 
        case PNG_COLOR_TYPE_GRAY:
            LOG_EVENT("Color need not to convert.");
            break;
        case PNG_COLOR_TYPE_PALETTE:
            LOG_ERROR("Unsupported png color format conversion: PNG_COLOR_TYPE_PALETTE");
            break;
        case PNG_COLOR_TYPE_RGB:
            nbmap = new uchar8[800 * 600 * 4];
            for(;i < h; i++){
                for(;j < w; j += 3){
                    *nbmap = (6969 * (int)p->R + 23434 * (int)p->G + 2365 * (int)p->B)/32768;
                    p++;
                }
            }
            delete [] (char*)*bmap;
            *bmap = nbmap;
            LOG_EVENT("Color converted from RGB to Greyscale.");
            break;
        case PNG_COLOR_TYPE_RGB_ALPHA:
            nbmap = new uchar8[800 * 600 * 4];
            for(;i < h; i++){
                for(;j < w; j += 4){
                    *nbmap = (6969 * (int)pa->R + 23434 * (int)pa->G + 2365 * (int)pa->B)/32768;
                    pa++;
                }
            }
            delete [] (char*)*bmap;
            *bmap = nbmap;
            LOG_EVENT("Color converted from RGBA to Greyscale.");
//            LOG_EVENT("Unsupported png color format conversion: PNG_COLOR_TYPE_RGB_ALPHA");
            break;
        case PNG_COLOR_TYPE_GRAY_ALPHA:
            LOG_ERROR("Unsupported png color format conversion: PNG_COLOR_TYPE_GRAY_ALPHA");
            break;
        default:
            LOG_ERROR("Unsupported png color format.");
            break;
    }
}

bool Image::SetupJPG(){
    return true;
}

bool Image::Draw(RenderMan* render){
//    return render->RenderGrayMap(pos.x, pos.y, req_width, req_height, bitmap);
    if (bitmap != NULL){
        return render->RenderGrayMap(pos.x, pos.y, bitmap_w, bitmap_h, bitmap);
//        return render->RenderPixMap(pos.x, pos.y, bitmap_w, bitmap_h, bitmap);
    }
    else
        return false;
}

bool Image::AdjustPos(int x, int y){
    return true;
}

Glyph* Image::Dup(){
    Image* img = new Image(logger);

    img->pos        = this->pos;
    img->bitmap_w   = this->bitmap_w;
    img->bitmap_h   = this->bitmap_h;
    img->bitmap     = new char[bitmap_w * bitmap_h];

    img->file_name  = new char[std::strlen(file_name) + 1];
    std::memcpy(img->file_name, file_name, std::strlen(file_name) + 1);
    img->file_path  = this->file_path;
    return NULL;
}
