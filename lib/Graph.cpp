#include <png.h>
#include <jpeglib.h>
#include <jerror.h>
#include "Color.h"
#include "Context.h"
#include "image.h"
#include "gif.h"
#include <string>
#include <cassert>

ImageOptions global_IO;

uint32 Graph::magic_num = 'g' + 'r' + 'a' + 'p' + 'h';

static void ReviseFileName(std::string & fname){
    int i;
    int len = fname.length();
    for (i = 0; i < len; i++){
        switch(fname[i]){
            case ' ':
            case '(':
            case ')':
                fname.insert(i, "\\");
                i++;
                break;
            default:
                break;
        }
    }
}

Graph::Graph(Logger* log):
    Glyph(log), file_name(NULL), file_path(NULL)
{
    if(NULL == html_dir){
        file_path = new int8[std::strlen(work_dir) + 1];
        std::strcpy(file_path, work_dir);
    }
    else{
        file_path = new int8[std::strlen(html_dir) + 1];
        std::strcpy(file_path, html_dir);
    }
}

Graph::~Graph(){
    if (file_name != NULL){
        delete [] file_name;
    }
    if (bitmap != NULL){
        delete [] (char*)bitmap;
    }
    if (file_path != NULL){
        delete [] (int8*)file_path;
        file_path = NULL;
    }
}

void Graph::SetSrcFile(const char* src) { 
    char buf[256 + std::strlen(src)];
    sprintf(buf, "Graph file: %s", src);
    LOG_EVENT(buf);
    file_name = new char[std::strlen(src) + 256];
    std::strcpy(file_name, src);
}

Glyph::GY_ST_RET Graph::Setup(LayoutManager& layout){
    if (true == fast_page_sum){
        if (req_width > PAGE_WIDTH || req_height > PAGE_HEIGHT){
            Resize(NULL, req_width, req_height, bitmap_w, bitmap_h);
        }
        LAYOUT_RET ret = layout.GetGraphPos(pos, bitmap_w, bitmap_h);

#ifdef NOGL
        pos.y -= bitmap_h;
#endif

        LOG_EVENT_STR3("JPG position", pos.x, pos.y);
        switch(ret){
            case LO_OK:
                layout.AddGlyph(this);
                break;
            case LO_NEW_LINE:
                layout.AddGlyph(this);
                break;
            case LO_NEW_PAGE:
                layout.Reset();
                return GY_NEW_PAGE;
            default:
                LOG_ERROR("Unsupported Layout return.");
                break;
        } 
        return GY_OK;
    }
    std::string file(file_path);
    file += file_name;
    ReviseFileName(file);

    FILE *fp = fopen(file.c_str(), "rb");
    if (!fp) {
        char info[256 + file.size()];
        sprintf(info, "Fail to open file %s", file.c_str());
        LOG_ERROR(info);
        return GY_ERROR;
    }

    IF_T type = DetectFormat(file.c_str(), fp);

    switch(type){
        case IF_NONE:
            LOG_WARNING("Unknow image format.");
            break;
        case IF_PNG:
            LOG_EVENT("Png file is detected.");
            return SetupPNG(layout, fp);
        case IF_JPG:
//            if (req_width == 0) assert(0);
            LOG_EVENT("JPG file is detected.");
            return SetupJPG(layout, fp);
            break;
        case IF_GIF:
            LOG_EVENT("GIF file is detected.");
            return SetupGIF(layout, fp);
            break;
        case IF_EMF:
            break;
        default:
            LOG_ERROR("Unsupported image file type.");
            break;
    }
    return GY_OK;
}

Graph::IF_T Graph::DetectFormat(const char * str, FILE * fp){
    // Detect if it is PNG file
    size_t png_hdr_num = 8;
    char png_hdr[png_hdr_num];
    fread(png_hdr, 1, png_hdr_num, fp);
    bool is_png = !png_sig_cmp((png_byte*)png_hdr, 0, (png_size_t)png_hdr_num);
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

    // Detect if it is GIF file
    char gif_hdr[GIF_SIG_LEN];
    if (fread(gif_hdr, 1, GIF_SIG_LEN, fp) != GIF_SIG_LEN){
        throw Except_Fail_To_Read_file();
    }
    gif_hdr[GIF_SIG_LEN] = '\0';
    if (strcmp((char *)gif_hdr, GIF_SIG) == 0 ||
        strcmp((char *)gif_hdr, GIF_SIG_89) == 0)
    { 
        rewind(fp);
        return IF_GIF;
    }
    rewind(fp);

    // Else
    LOG_ERROR("Currently other files than png are not supported!");

    return IF_NONE;
}

Glyph::GY_ST_RET Graph::SetupPNG(LayoutManager& layout, FILE* fp){
    png_structp png_ptr = 
        png_create_read_struct(PNG_LIBPNG_VER_STRING, (png_voidp)NULL, NULL, NULL);
    if (!png_ptr){
        LOG_ERROR("Fail to create png struct!");
        return GY_ERROR; 
    }

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr){
        png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
        LOG_ERROR("Fail to create png info!");
        return GY_ERROR; 
    } 

    png_infop end_info = png_create_info_struct(png_ptr);
    if (!end_info) {
        png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
        LOG_ERROR("Fail to create png info!");
        return GY_ERROR; 
    }

    if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
        fclose(fp);
        LOG_ERROR("Setjump callback.");
        return GY_ERROR; 
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

    uint8* bmap = new uint8[png_get_rowbytes(png_ptr, info_ptr) * height];

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

    int n_w, n_h;
    if (bitmap_w > PAGE_WIDTH || bitmap_h > PAGE_HEIGHT){
        bmap = (uint8*)Resize(bitmap, bitmap_w, bitmap_h, n_w, n_h);
        if (NULL != bmap){
            delete [] (uint8*)bitmap;
            bitmap = bmap;
            bitmap_w = n_w;
            bitmap_h = n_h;
        }
    }

    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);

    LAYOUT_RET ret;
    ret = layout.GetGraphPos(pos, bitmap_w, bitmap_h);

#ifdef NOGL
    pos.y -= bitmap_h;
#endif

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
            return GY_NEW_PAGE;
        default:
            LOG_ERROR("Unsupported Layout return.");
            break;
    }

    return GY_OK;
}

void Graph::Convert(void** bmap, int w, int h, uchar8 col_t, uchar8 bit_depth, int channel){
    // Convert image from RGB to Grayscale

    bitmap = NULL;
//    Color* p = NULL;
    Color_A * pa = NULL;
    int i = 0, j = 0;

    if (channel == 3){
        bitmap = new uchar8[w * h / channel];
        uchar8* nbmap = (uchar8 *)bitmap;
#ifndef NOGL
        for(i = h - 1;i >= 0; i--){
#else
        for(i = 0;i < h; i++){
#endif
#if 0
            p = (Color *)bmap[i];
            for(j = 0;j < w; j += channel){
                *(uchar8*)nbmap = (uchar8)((6969 * (long int)p->R + 23434 * (long int)p->G + 2365 * (long int)p->B)/32768);
                nbmap++;
                p++;
            }
#endif
            // ARM fix: Arm doesn't support addressing 3 bytes offset from the base
            uint8 * p = (uint8 *)bmap[i];
            for(j = 0;j < w; j += channel){
                *(uchar8*)nbmap = (uchar8)((6969 * (long int)p[0] + 23434 * (long int)p[1] + 2365 * (long int)p[2])/32768);
                nbmap++;
                p += 3;
            }
        }
        bitmap_w = w / channel;
        bitmap_h = h;
        LOG_EVENT("Color converted from RGB to Grayscale.");
    }
    else if (channel == 4){
        bitmap = new uchar8[w * h / channel];
        uchar8* nbmap = (uchar8 *)bitmap;
#ifndef NOGL
        for(i = h - 1;i >= 0; i--){
#else
        for(i = 0;i < h; i++){
#endif
            pa = (Color_A*)bmap[i];
            for(j = 0; j < w; j += channel){
                *(uchar8*)nbmap = (uchar8)((6969 * (long int)pa->R + 23434 * (long int)pa->G + 2365 * (long int)pa->B)/32768);
                nbmap++;
                pa++;
            }
        }
        bitmap_w = w / channel;
        bitmap_h = h;
        LOG_EVENT("Color converted from RGBA to Grayscale.");
    }
}

Glyph::GY_ST_RET Graph::SetupJPG(LayoutManager& layout, FILE* fp){
    fclose(fp);

    std::string file(file_path);
    file += file_name;

    strcpy((char*)global_IO.fullname, file.c_str());

    global_IO.exactly =FALSE;
    global_IO.zoom    = 1;
#if 0
    global_IO.smooth  = FALSE;
    global_IO.width   = req_width; 
    global_IO.height  = req_height; 
#endif
    global_IO.smooth  = TRUE;
    global_IO.width   = 1; 
    global_IO.height  = 1; 

    LOG_EVENT("load image.");
    if(loadImage(&global_IO) == ERROR){
        LOG_ERROR("Loading jpg file fails");
        return GY_ERROR;
    }

    req_width   = global_IO.width;
    req_height  = global_IO.height;

    ConvertJPG((void*)getImage(), req_width, req_height);

    freeImage();

    uint8* bmap;
    int n_w, n_h;
    if (req_width > PAGE_WIDTH || req_height > PAGE_HEIGHT){
        bmap = (uint8*)Resize(bitmap, req_width, req_height, n_w, n_h);
        if (NULL != bmap){
            delete [] (uint8*)bitmap;
            bitmap = bmap;
            bitmap_w = n_w;
            bitmap_h = n_h;
        }
    }


    LAYOUT_RET ret;
    ret = layout.GetGraphPos(pos, bitmap_w, bitmap_h);

#ifdef NOGL
    pos.y -= bitmap_h;
#endif

    LOG_EVENT_STR3("JPG position", pos.x, pos.y);
    switch(ret){
        case LO_OK:
            layout.AddGlyph(this);
            break;
        case LO_NEW_LINE:
            layout.AddGlyph(this);
            break;
        case LO_NEW_PAGE:
            layout.Reset();
            return GY_NEW_PAGE;
        default:
            LOG_ERROR("Unsupported Layout return.");
            break;
    }

    return GY_OK;
}

Glyph::GY_ST_RET Graph::SetupGIF(LayoutManager& layout, FILE* fp){
    fclose(fp);

    std::string file(file_path);
    file += file_name;

    strcpy((char*)global_IO.fullname, file.c_str());

    global_IO.exactly =FALSE;
    global_IO.zoom    = 1;
#if 0
    global_IO.smooth  = FALSE;
    global_IO.width   = req_width; 
    global_IO.height  = req_height; 
#endif
    global_IO.smooth  = TRUE;
    global_IO.width   = 1; 
    global_IO.height  = 1; 

    LOG_EVENT("load image.");
    if(loadImage(&global_IO) == ERROR){
        LOG_ERROR("Loading gif file fails");
        return GY_ERROR;
    }

    req_width   = global_IO.width;
    req_height  = global_IO.height;

//    LOG_EVENT("fill image.");
//    fillImage(output_image.data, req_width, req_height, req_width * 2, 0, 0);
//    LOG_EVENT("convert image.");
    ConvertJPG((void*)getImage(), req_width, req_height);

    LOG_EVENT("resize image.");
    uint8* bmap;
    int n_w, n_h;
    if (req_width > PAGE_WIDTH || req_height > PAGE_HEIGHT){
        bmap = (uint8*)Resize(bitmap, req_width, req_height, n_w, n_h);
        if (NULL != bmap){
            delete [] (uint8*)bitmap;
            bitmap = bmap;
            bitmap_w = n_w;
            bitmap_h = n_h;
        }
    }

    freeImage();

    LOG_EVENT("setup image.");
    LAYOUT_RET ret;
    ret = layout.GetGraphPos(pos, bitmap_w, bitmap_h);

#ifdef NOGL
    pos.y -= bitmap_h;
#endif

    LOG_EVENT_STR3("GIF position", pos.x, pos.y);
    switch(ret){
        case LO_OK:
            layout.AddGlyph(this);
            break;
        case LO_NEW_LINE:
            layout.AddGlyph(this);
            break;
        case LO_NEW_PAGE:
            layout.Reset();
            return GY_NEW_PAGE;
        default:
            LOG_ERROR("Unsupported Layout return.");
            break;
    }

    return GY_OK;
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
    
#ifndef NOGL
    for(j = h - 1; j >= 0; j--){
#else
    for(j = 0; j < h; j++){
#endif
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

void* Graph::Resize(void* bmap, int32 w_old, int32 h_old, int32 & w_new, int32 & h_new){
//    assert(w_old >= 0);
//    assert(h_old >= 0);

    int32 newWidth;
    int32 newHeight;

    if (w_old > PAGE_WIDTH){
        newWidth = PAGE_WIDTH - IMAGE_GUARD_SIZE;
        newHeight = h_old * newWidth / w_old;
        if (newHeight > PAGE_HEIGHT){
            newHeight = PAGE_HEIGHT - IMAGE_GUARD_SIZE;
            newWidth = w_old * newHeight / h_old;
        }
    }
    else if (h_old > PAGE_HEIGHT){
        newHeight = PAGE_HEIGHT - IMAGE_GUARD_SIZE;
        newWidth = w_old * newHeight / h_old;
        if (newWidth > PAGE_WIDTH){
            newWidth = PAGE_WIDTH - IMAGE_GUARD_SIZE;
            newHeight = h_old * newWidth / w_old;
        }
    }
    // Else, don't change anything
    else{
        w_new = 0;
        h_new = 0;
        return NULL;
    }
    w_new = newWidth;
    h_new = newHeight;
    if (true == fast_page_sum){
        return NULL;
    }
    else{
        return ResizeImpl(bmap, w_old, h_old, newWidth, newHeight);
    }
}

void* Graph::ResizeImpl(void* bmap, int32 w_old, int32 h_old, int32 w_new, int32 h_new){
    int32 row, col;
    uint8 * p = (uint8*)bmap;
    uint8 * nbmap = new uint8[w_new * h_new * sizeof(uint8)];
    uint8 * q = nbmap;

    // Nearest point sampling
    for(row = 0; row < h_new; row++){
        int32 sample_row = row * h_old / h_new;
        for (col = 0; col < w_new; col++){
            int32 sample_col = col * w_old / w_new;
            *(q++) = *(p + (sample_row * w_old) + sample_col);
        }
    }

    return nbmap;
}

bool Graph::Draw(RenderMan& render){
    if (true == fast_page_sum){
        return true;
    }
    if (bitmap != NULL){
        bool ret = render.RenderGrayMap(pos.x, pos.y, bitmap_w, bitmap_h, bitmap);
        delete [] (uint8*)bitmap;
        return ret;
    }
    else
        return false;
}

bool Graph::Relocate(int x, int y){
    pos.x += x;
    return true;
}

Glyph* Graph::UngetSet(){
    delete [] (uint8 *)bitmap;
    bitmap = NULL;
    bitmap_w = 0;
    bitmap_h = 0;
    return this;
}

Glyph* Graph::Dup(){
    Graph* img = new Graph(logger);

    img->pos        = this->pos;
    img->bitmap_w   = this->bitmap_w;
    img->bitmap_h   = this->bitmap_h;
    img->bitmap     = new char[bitmap_w * bitmap_h];
    img->req_width  = this->req_width;
    img->req_height = this->req_height;

    img->file_name  = new char[std::strlen(file_name) + 1];
    std::memcpy(img->file_name, file_name, std::strlen(file_name) + 1);
    img->file_path  = this->file_path;

    return img;
}

void Graph::Serialize(std::ofstream & ofs){

    SER_OBJ(magic_num);
    SER_OBJ(req_width);
    SER_OBJ(req_height);
    uint32 len = std::strlen(file_name) + 1;
    SER_OBJ(len);
    ofs.write((char*)file_name, len);
}

void Graph::Deserialize(std::ifstream & ifs){
    DESER_OBJ(req_width);
    DESER_OBJ(req_height);
    uint32 len;
    DESER_OBJ(len);
    if (file_name){
        delete [] file_name;
        file_name = NULL;
    }
    file_name = new char[len];
    ifs.read((char*)file_name, len);
}

