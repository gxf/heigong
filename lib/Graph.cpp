#include <png.h>
#include <jpeglib.h>
#include <jerror.h>
#include "Color.h"
#include "Context.h"
#include "image.h"
#include <string>

ImageOptions global_IO;
Image output_image;

Graph::Graph(Logger* log):
    Glyph(log), file_name(NULL), file_path(NULL)
{
    file_path = (char*)"./";
}

Graph::~Graph(){
    if (file_name != NULL){
        delete [] file_name;
    }
    if (bitmap != NULL){
        delete [] (char*)bitmap;
    }
}

void Graph::SetSrcFile(const char* src) { 
    char buf[100];
    sprintf(buf, "Graph file: %s", src);
    LOG_EVENT(buf);
    file_name = new char[std::strlen(src) + 1];
    std::memcpy(file_name, src, std::strlen(src) + 1);
}

bool Graph::Setup(LayoutManager& layout){
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
            return SetupPNG(layout, fp);
        case IF_JPG:
            LOG_EVENT("JPG file is detected.");
            return SetupJPG(layout, fp);
            break;
        case IF_EMF:
            break;
        default:
            LOG_ERROR("Unsupported image file type.");
            break;
    }
    return true;
}

Graph::IF_T Graph::DetectFormat(const char * str, FILE * fp){
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
        rewind(fp);
        return IF_JPG;
    }
    rewind(fp);

    // Else
    LOG_ERROR("Currently other files than png are not supported!");

    return IF_NONE;
}

bool Graph::SetupPNG(LayoutManager& layout, FILE* fp){
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

    png_read_info(png_ptr, info_ptr);

    png_uint_32 width, height;
    int bit_depth, col_type, channel;
    png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &col_type,
                 NULL, NULL, NULL);

    if (col_type == PNG_COLOR_TYPE_PALETTE){
        png_set_expand(png_ptr);
    }
    if (col_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8){
        png_set_expand(png_ptr);
    }
    if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS)){
        png_set_expand(png_ptr);
    }
    if (bit_depth == 16){
        png_set_strip_16(png_ptr);
    }
    if (col_type & PNG_COLOR_MASK_ALPHA){
        png_set_strip_alpha(png_ptr);
    }

    png_read_update_info(png_ptr, info_ptr);

    png_bytep row_pointers[height];

    uint32 row;

    char* bmap = new char[png_get_rowbytes(png_ptr, info_ptr) * height];

    for (row = 0; row < height; row++){
        row_pointers[row] = 
            (png_bytep)bmap + row * png_get_rowbytes(png_ptr, info_ptr);
    }

    png_read_image(png_ptr, row_pointers);
    png_read_end(png_ptr, end_info);

    col_type = png_get_color_type(png_ptr, info_ptr);
    bit_depth = png_get_bit_depth(png_ptr, info_ptr);
    channel = png_get_channels(png_ptr, info_ptr);

    Convert((void**)row_pointers, png_get_rowbytes(png_ptr, info_ptr), height, col_type, bit_depth, channel);
    delete [] bmap;

    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);

    LAYOUT_RET ret;
    ret = layout.GetGraphPos(pos, bitmap_w, bitmap_h);

    printf("graph pos: x=%d, y=%d\n", pos.x, pos.y);
    switch(ret){
        case LO_OK:
            layout.AddGlyph(this);
            break;
        case LO_NEW_LINE:
            layout.AddGlyph(this);
            break;
        case LO_NEW_PAGE:
            layout.Reset();
            return false;
        default:
            LOG_ERROR("Unsupported Layout return.");
            break;
    }

    return true;
}

void Graph::Convert(void** bmap, int w, int h, uchar8 col_t, uchar8 bit_depth, int channel){
    // Convert image from RGB to Grayscale

    bitmap = NULL;
    Color* p = NULL;
    Color_A * pa = NULL;
    int i = 0, j = 0;
#if 0
    switch(col_t){ 
        case PNG_COLOR_TYPE_GRAY:
            LOG_EVENT("Color need not to convert.");
            break;
        case PNG_COLOR_TYPE_PALETTE:
            LOG_ERROR("Unsupported png color format conversion: PNG_COLOR_TYPE_PALETTE");
            break;
        case PNG_COLOR_TYPE_RGB:
            bitmap = new uchar8[w * h];
            for(;i < h ; i++){
                p = (Color *)bmap[i];
                for(;j < w; j += 3){
                    *(uchar8*)bitmap = (6969 * (long int)p->R + 23434 * (long int)p->G + 2365 * (long int)p->B)/32768;
                    printf("p: %x, R: %x, G: %x, B: %x, Gray: %x\n", (int)p, (int)p->R, (int)p->G, (int)p->B, *(uchar8*)bitmap);
                    bitmap = (uchar8*)bitmap + 1;
                    p++;
                }
            }
            bitmap_w = w / 3;
            bitmap_h = h;
            LOG_EVENT("Color converted from RGB to Grayscale.");
            break;
        case PNG_COLOR_TYPE_RGB_ALPHA:
            bitmap = new uchar8[w * h];
            for(;i < h ; i++){
                pa = (Color_A*)bmap[i];
                for(;j < w; j += 4){
                    *(uchar8*)bitmap = (6969 * (long int)pa->R + 23434 * (long int)pa->G + 2365 * (long int)pa->B)/32768;
                    printf("pa: %x, R: %x, G: %x, B: %x, Gray: %x\n", (int)pa, (int)pa->R, (int)pa->G, (int)pa->B, *(uchar8*)bitmap);
                    bitmap = (uchar8*)bitmap + 1;
                    pa++;
                }
            }
            bitmap_w = w / 4;
            bitmap_h = h;
            LOG_EVENT("Color converted from RGBA to Grayscale.");
            break;
        case PNG_COLOR_TYPE_GRAY_ALPHA:
            LOG_ERROR("Unsupported png color format conversion: PNG_COLOR_TYPE_GRAY_ALPHA");
            break;
        default:
            LOG_ERROR("Unsupported png color format.");
            break;
    }
#endif
    if (channel == 3){
        bitmap = new uchar8[w * h / channel];
        uchar8* nbmap = (uchar8 *)bitmap;
        for(i = h - 1;i >= 0 ; i--){
            p = (Color *)bmap[i];
            for(j = 0;j < w; j += channel){
                *(uchar8*)nbmap = (6969 * (long int)p->R + 23434 * (long int)p->G + 2365 * (long int)p->B)/32768;
                nbmap++;
                p++;
            }
        }
        bitmap_w = w / channel;
        bitmap_h = h;
        LOG_EVENT("Color converted from RGB to Grayscale.");
    }
    else if (channel == 4){
        bitmap = new uchar8[w * h / channel];
        uchar8* nbmap = (uchar8 *)bitmap;
        for(i = h - 1; i >= 0; i--){
            pa = (Color_A*)bmap[i];
            for(j = 0; j < w; j += channel){
                *(uchar8*)nbmap = (6969 * (long int)pa->R + 23434 * (long int)pa->G + 2365 * (long int)pa->B)/32768;
                nbmap++;
                pa++;
            }
        }
        bitmap_w = w / channel;
        bitmap_h = h;
        LOG_EVENT("Color converted from RGBA to Grayscale.");
    }
}

bool Graph::SetupJPG(LayoutManager& layout, FILE* fp){
    fclose(fp);

    // Adjust image size in order not to render out side of frame buffer
    double ratio = (double)req_width / req_height;

    if (req_width > SCREEN_WIDTH - 2 * MARGIN_VERTICAL){
	req_width = SCREEN_WIDTH - 2 * MARGIN_VERTICAL - IMAGE_GAURD_SIZE;
	req_height = req_width / ratio;
	if (req_height > SCREEN_HEIGHT - 2 * MARGIN_HORIZONTAL){
	    req_height = SCREEN_HEIGHT - 2 * MARGIN_HORIZONTAL - IMAGE_GAURD_SIZE;
	    req_width = req_height * ratio;
	}
    }

    std::string file(file_path);
    file += file_name;

    strcpy((char*)global_IO.fullname, file.c_str());

    global_IO.exactly =FALSE;
    global_IO.zoom    = 1;
    global_IO.smooth  = TRUE;
    global_IO.width   = req_width; 
    global_IO.height  = req_height; 

    if(loadImage(&global_IO) == ERROR){
        LOG_ERROR("Loading jpg file fails");
        return false;
    }

    fillImage(output_image.data, req_width, req_height, req_width * 2, 0, 0);
//    fillImage(output_image.data, 160, 128, 160* 2, 0, 0);
    ConvertJPG((void**)output_image.data, req_width, req_height);
//    ConvertJPG((void**)output_image.data, 160, 128);

    freeImage();

    LAYOUT_RET ret;
    ret = layout.GetGraphPos(pos, bitmap_w, bitmap_h);

    switch(ret){
        case LO_OK:
            layout.AddGlyph(this);
            break;
        case LO_NEW_LINE:
            layout.AddGlyph(this);
            break;
        case LO_NEW_PAGE:
            layout.Reset();
            return false;
        default:
            LOG_ERROR("Unsupported Layout return.");
            break;
    }

    return true;
}

void Graph::ConvertJPG(void* bmap, int w, int h){
    // Convert image from RGB to Grayscale

    bitmap = NULL;
    int i = 0, j = 0;
    int channel = 3;
    int ind;
    UINT16 color;
    UINT8  r, g, b;
    
    bitmap = new uchar8[w * h * channel];
    uchar8* nbmap = (uchar8 *)bitmap;
    
    for(j = h - 1; j >= 0; j--){
	for(i = 0; i < w; i ++){
	    ind = j * w + i;
	    color = *((uint16*)((uchar8*)bmap + ind * 2));
	    r = (color & 0x1F)<<3;
	    g = (color & 0x7E0)>>3;
	    b = (color & 0xF800)>>8;
            *(uchar8*)nbmap = (6969 * (long int)r + 23434 * (long int)g + 2365 * (long int)b)/32768;
            nbmap++;
        }
    }
    bitmap_w = w;
    bitmap_h = h;
    LOG_EVENT("Color converted from RGBA to Grayscale.");
}


bool Graph::Draw(RenderMan& render){
    if (bitmap != NULL){
        return render.RenderGrayMap(pos.x, pos.y, bitmap_w, bitmap_h, bitmap);
    }
    else
        return false;
}

bool Graph::Relocate(int x, int y){
    return true;
}

Glyph* Graph::Dup(){
    Graph* img = new Graph(logger);

    img->pos        = this->pos;
    img->bitmap_w   = this->bitmap_w;
    img->bitmap_h   = this->bitmap_h;
    img->bitmap     = new char[bitmap_w * bitmap_h];

    img->file_name  = new char[std::strlen(file_name) + 1];
    std::memcpy(img->file_name, file_name, std::strlen(file_name) + 1);
    img->file_path  = this->file_path;

    return NULL;
}
