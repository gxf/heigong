/* 
 * $Log: gif.c,v $
 * Revision 1.11  2005/03/28 11:39:45  fra
 * ��ȼ���
 *
 * Revision 1.10  2005/03/27 17:46:50  fra
 * �޸���***Load�Ľӿ�
 *
 * Revision 1.9  2005/03/27 17:12:19  fra
 * ȥ����Identϵ�к�����֧��
 *
 * Revision 1.8  2005/03/27 17:08:13  fra
 * ʹ��malloc�����̬����
 * ʹ��TRACE���printf
 *
 * Revision 1.7  2005/03/27 11:42:00  fra
 * *** empty log message ***
 *
 * Revision 1.6  2005/03/27 09:18:24  fra
 * �޸���buildindex�����Ľӿ�
 *
 * Revision 1.5  2005/03/23 18:12:32  fra
 * ���������ŵ�֧��
 *
 * Revision 1.4  2005/03/21 13:11:34  fra
 * bug 1: interlace ��ͼƬ��bug���, y+=interlace_rate[pass]��λ�÷Ŵ���
 *
 * Revision 1.3  2005/03/21 13:06:25  fra
 * ����ʵ��gif, ��֪��bug:
 * 1. interlace ��ͼƬ������
 * 2. ��֧�ֶ���
 * 3. ��֧��͸������
 */

#define GIF_C
#include "image.h"
#include "imagetypes.h"
#include "image_file.h"
#include "image_misc.h"
#include "gif.h"
#include "stdio.h"
#include "zoom.h"

#ifdef DEBUG
#define TRACE(string,args...) printf("<GIF>:"string,##args)
#else
#define TRACE(string,args...)
#endif

extern Image *global_image;
extern UINT8 *row_buffer;
#define PUSH_PIXEL(p)                                       \
{                                                           \
  if (pstk_idx == PSTK_SIZE)                                \
    return GIFIN_ERR_PSO;                                   \
  else                                                      \
    pstk[pstk_idx++] = (p);                                 \
}

/*
 * push a string (denoted by a code) onto the pixel stack
 * (returns the code of the first pixel in the string in ps_rslt)
 */

int ps_rslt;	/* return result */

#define GIFIN_PUSH_STRING(code)                             \
{                                                           \
  int ps_code;                                              \
  ps_code = code;                                           \
                                                            \
  while (((unsigned int)ps_code) < STAB_SIZE && prefix[ps_code] != NULL_CODE) \
  {                                                         \
    PUSH_PIXEL(extnsn[ps_code]);                            \
    ps_code = prefix[ps_code];                              \
  }                                                         \
                                                            \
  if (((unsigned int)ps_code) >= STAB_SIZE)                                 \
    return GIFIN_ERR_TAO;                                   \
  PUSH_PIXEL(extnsn[ps_code]);                              \
  ps_rslt = extnsn[ps_code];                                \
  if (((unsigned int)ps_rslt) >= STAB_SIZE)                                 \
    return GIFIN_ERR_TAO;                                   \
}

/*
 * Look up the ascii message coresponding to
 * the error number.
 */
static const char *
get_err_string(int errno)
{
     int i;
     for (i=0;gif_err_strings[i].err_no != 0;i++)
     {
         if (gif_err_strings[i].err_no == errno)
             return gif_err_strings[i].name;
     }
     return "";
}

/****
 **
 ** local variables
 **
 ****/
static int interlace_start[4]= { /* start line for interlacing */
     0, 4, 2, 1
};

static int interlace_rate[4]= { /* rate at which we accelerate vertically */
     8, 8, 4, 2
};

static UINT8 file_open  = 0;     /* status flags */
static UINT8 image_open = 0;

static INT32 ins;              /* input stream */

static int  root_size;          /* root code size */
static int  clr_code;           /* clear code */
static int  eoi_code;           /* end of information code */
static int  code_size;          /* current code size */
static int  code_mask;          /* current code mask */
static int  prev_code;          /* previous code */

/*
 * NOTE: a long is assumed to be at least 32 bits wide
 */
static long work_data;          /* working bit buffer */
static int  work_bits;          /* working bit count */

static UINT8 buf[256];           /* byte buffer */
static int  buf_cnt;            /* byte count */
static int  buf_idx;            /* buffer index */

static int table_size;          /* string table size */
static int prefix[STAB_SIZE];   /* string table : prefixes */
static int extnsn[STAB_SIZE];   /* string table : extensions */

static UINT8 pstk[PSTK_SIZE];    /* pixel stack */
static int  pstk_idx;           /* pixel stack pointer */


/****
 **
 ** global variables
 **
 ****/

static int  gifin_rast_width;          /* raster width */
static int  gifin_rast_height;         /* raster height */
static UINT8 gifin_g_cmap_flag;         /* global colormap flag */
static int  gifin_g_pixel_bits;        /* bits per pixel, global colormap */
static int  gifin_g_ncolors;           /* number of colors, global colormap */
static UINT8 gifin_g_cmap[3][256];      /* global colormap */
static UINT8 gifin_g_cmap_sorted;       /* global colormap sorted (GIF89a only) */
static int  gifin_bg_color;            /* background color index */
static int  gifin_color_bits;          /* bits of color resolution */
static double  gifin_aspect;           /* pixel aspect ratio (width/height) */
static int  gifin_version;             /* gif file version */

static int  gifin_img_left;            /* image position on raster */
static int  gifin_img_top;             /* image position on raster */
static int  gifin_img_width;           /* image width */
static int  gifin_img_height;          /* image height */
static UINT8 gifin_l_cmap_flag;         /* local colormap flag */
static int  gifin_l_pixel_bits;        /* bits per pixel, local colormap */
static int  gifin_l_ncolors;           /* number of colors, local colormap */
static UINT8 gifin_l_cmap[3][256];      /* local colormap */
static UINT8 gifin_interlace_flag;      /* interlace image format flag */

/*
 * load a colormap from the input stream
 */
static INT32 gifin_load_cmap(UINT8 (*cmap)[256], INT32 ncolors)
{
     INT32 i;

     for (i=0; i<ncolors; i++)
     {
	  if (image_file_read(ins, buf, 3) != 3)
	       return GIFIN_ERR_EOF;
    
	  cmap[GIF_RED][i] = buf[GIF_RED];
	  cmap[GIF_GRN][i] = buf[GIF_GRN];
	  cmap[GIF_BLU][i] = buf[GIF_BLU];
     }

     /* done! */
     return GIFIN_SUCCESS;
}


/*
 * open a GIF file, using s as the input stream
 */

static int gifin_open_file(INT32 s)
{
     int errno;
     /* make sure there isn't already a file open */
     if (file_open)
         return GIFIN_ERR_FAO;

     /* remember that we've got this file open */
     file_open = 1;
     ins       = s;

     /* check GIF signature */
     if (image_file_read(ins, buf, GIF_SIG_LEN) != GIF_SIG_LEN)
         return GIFIN_ERR_EOF;

     buf[GIF_SIG_LEN] = '\0';
     if (strcmp((char *) buf, GIF_SIG) == 0)
         gifin_version = GIF87a;
     else if(strcmp((char *) buf, GIF_SIG_89) == 0)
         gifin_version = GIF89a;
     else
         return GIFIN_ERR_BAD_SIG;

     /* read screen descriptor */
     if (image_file_read(ins, buf, GIF_SD_SIZE) != GIF_SD_SIZE)
         return GIFIN_ERR_EOF;

     /* decode screen descriptor */
     gifin_rast_width   = (buf[1] << 8) + buf[0];
     gifin_rast_height  = (buf[3] << 8) + buf[2];
     gifin_g_cmap_flag  = (buf[4] & 0x80) ? 1 : 0;
     gifin_color_bits   = (((int)(buf[4] & 0x70)) >> 4) + 1;
     gifin_g_pixel_bits = (buf[4] & 0x07) + 1;
     gifin_bg_color     = buf[5];
     gifin_aspect = 1.0;

     if (gifin_version == GIF87a) {
         if (buf[4] & 0x08 || buf[6] != 0)
             return GIFIN_ERR_BAD_SD;
     } 
     else {
         gifin_g_cmap_sorted = ((buf[4] & 0x08) != 0);
         if (buf[6] != 0)
             gifin_aspect = ((double)buf[6] + 15.0) / 64.0;
     }

     /* load global colormap */
     if (gifin_g_cmap_flag) {
         gifin_g_ncolors = (1 << gifin_g_pixel_bits);

         if ((errno = gifin_load_cmap(gifin_g_cmap, gifin_g_ncolors)) != GIFIN_SUCCESS)
             return errno;
     }
     else{
         gifin_g_ncolors = 0;
     }

     /* done! */
     return GIFIN_SUCCESS;
}


/** 
 * read a new data block from the input stream
 * buf_cntΪ���ݿ�ĳ��ȣ� buf_idx ��Ϊ0
 * 
 * @return GIFIN_SUCCESS or GIFIN_ERR_EOF
 */
static int gifin_read_data_block(void)
{
     /* read the data block header */
     buf_cnt = image_file_getc(ins);

     /* read the data block body */
     if (image_file_read(ins, buf, buf_cnt) != buf_cnt)
         return GIFIN_ERR_EOF;

     buf_idx = 0;

     /* done! */
     return GIFIN_SUCCESS;
}


/*
 * skip an extension block in the input stream
 */

static int gifin_skip_extension(void)
{
     int errno;

     /* get the extension function byte */
     image_file_getc(ins);

     /* skip any remaining raster data */
     do
     {
         if ((errno = gifin_read_data_block()) != GIFIN_SUCCESS)
             return errno;
     }
     while (buf_cnt > 0);

     /* done! */
     return GIFIN_SUCCESS;
}


/*
 * open next GIF image in the input stream; returns GIFIN_SUCCESS if
 * successful. if there are no more images, returns GIFIN_DONE. (might
 * also return various GIFIN_ERR codes.)
 */

static int gifin_open_image(void)
{
     int i;
     int separator;
     int errno;

     /* make sure there's a file open */
     if (!file_open)
         return GIFIN_ERR_NFO;

     /* make sure there isn't already an image open */
     if (image_open)
         return GIFIN_ERR_IAO;

     /* remember that we've got this image open */
     image_open = 1;

     /* skip over any extension blocks */
     do
     {
         separator = image_file_getc(ins);
         if (separator == GIF_EXTENSION)
         {
             if ((errno = gifin_skip_extension()) != GIFIN_SUCCESS)
                 return errno;
         }
     }
     while (separator == GIF_EXTENSION);

     /* check for end of file marker */
     if (separator == GIF_TERMINATOR)
         return GIFIN_DONE;

     /* make sure we've got an image separator */
     if (separator != GIF_SEPARATOR)
         return GIFIN_ERR_BAD_SEP;

     /* read image descriptor */
     if (image_file_read(ins, buf, GIF_ID_SIZE) != GIF_ID_SIZE)
         return GIFIN_ERR_EOF;

     /* decode image descriptor */
     gifin_img_left       = (buf[1] << 8) + buf[0];
     gifin_img_top        = (buf[3] << 8) + buf[2];
     gifin_img_width      = (buf[5] << 8) + buf[4];
     gifin_img_height     = (buf[7] << 8) + buf[6];
     gifin_l_cmap_flag    = (buf[8] & 0x80) ? 1 : 0;
     gifin_interlace_flag = (buf[8] & 0x40) ? 1 : 0;
     gifin_l_pixel_bits   = (buf[8] & 0x07) + 1;

     /* load local colormap */
     if (gifin_l_cmap_flag)
     {
         gifin_l_ncolors = (1 << gifin_l_pixel_bits); 
         
         if ((errno = gifin_load_cmap(gifin_l_cmap, gifin_l_ncolors)) != GIFIN_SUCCESS)
             return errno;
     }
     else{
         gifin_l_ncolors = 0;
     }

     /* initialize raster data stream decoder */
     root_size = image_file_getc(ins);
     clr_code  = 1 << root_size;
     eoi_code  = clr_code + 1;
     code_size = root_size + 1;
     code_mask = (1 << code_size) - 1;
     work_bits = 0;
     work_data = 0;
     buf_cnt   = 0;
     buf_idx   = 0;

     /* initialize string table */
     for (i = 0; i < STAB_SIZE; i++) {
         prefix[i] = NULL_CODE;
         extnsn[i] = i;
     }

     /* initialize pixel stack */
     pstk_idx = 0;

     /* done! */
     return GIFIN_SUCCESS;
}

/** 
 * add a new string to the string table
 * @param p 
 * @param e 
 * 
 * @return 
 */static int gifin_add_string(int p, int e)
{
     prefix[table_size] = p;
     extnsn[table_size] = e;

     if ((table_size == code_mask) && (code_size < 12))
     {
	  code_size += 1;
	  code_mask  = (1 << code_size) - 1;
     }

     table_size += 1;
     if (table_size > STAB_SIZE)
	  return GIFIN_ERR_TAO;
     return GIFIN_SUCCESS;
}

/*
 * try to read next pixel from the raster, return result in *pel
 */
static int gifin_get_pixel(int *pel)
{
     int  code;
     int  first;
     int  place;
     int  errno;

     /* decode until there are some pixels on the pixel stack */
     while (pstk_idx == 0)
     {
	  /* load bytes until we have enough bits for another code, code_size�ǻ�仯��,�12 bits */
	  while (work_bits < code_size)
	  {
	       if (buf_idx == buf_cnt)
	       {
		    /* read a new data block, ���������256��bytes */
		    if ((errno = gifin_read_data_block()) != GIFIN_SUCCESS)
			 return errno;

		    if (buf_cnt == 0)
			 return GIFIN_ERR_EOD;
	       }

	       work_data |= ((long) buf[buf_idx++]) << work_bits;
	       work_bits += 8;
	  }

	  /* get the next code */
	  code        = work_data & code_mask; // ȡ����Ҫ��bits
	  work_data >>= code_size; // ȥ���Ѿ���ȡ�õ�code
	  work_bits  -= code_size; // ����work bits

	  /* interpret the code, reset ����ı�*/
	  if (code == clr_code)
	  {
	       /* reset decoder stream */
	       code_size  = root_size + 1; // ����code size Ϊԭʼ��root_size +1
	       code_mask  = (1 << code_size) - 1; // ��Ӧ���޸�code_mask
	       prev_code  = NULL_CODE; 
	       table_size = eoi_code + 1; // (1<<root_size)+1+1
	  }
	  else if (code == eoi_code)
	  {
	       /* Ooops! no more pixels */
	       return GIFIN_ERR_EOF;
	  }
	  else if (prev_code == NULL_CODE)
	  {
	       GIFIN_PUSH_STRING(code);
	       prev_code = code;
	  }
	  else
	  {
	       if (code < table_size)
	       {
		    GIFIN_PUSH_STRING(code);
		    first = ps_rslt;
	       }
	       else
	       {
		    place = pstk_idx;
		    PUSH_PIXEL(NULL_CODE);
		    GIFIN_PUSH_STRING(prev_code);
		    first = ps_rslt;
		    pstk[place] = first;
	       }

	       if((errno = gifin_add_string(prev_code, first)) != GIFIN_SUCCESS)
		    return errno;
	       prev_code = code;
	  }
     }

     /* pop a pixel off the pixel stack */
     *pel = (int) pstk[--pstk_idx];

     /* done! */
     return GIFIN_SUCCESS;
}


/*
 * close an open GIF file
 */

static int gifin_close_file(void)
{
     /* make sure there's a file open */
     if (!file_open)
	  return GIFIN_ERR_NFO;

     /* mark file (and image) as closed */
     file_open  = 0;
     image_open = 0;

     /* done! */
     return GIFIN_SUCCESS;
}


/* these are the routines added for interfacing to xli
 */

/* tell someone what the image we're loading is.  this could be a little more
 * descriptive but I don't care
 */

static void tellAboutImage(char *name)
{
     TRACE("%s is a %dx%d %s%s image with %d colors\n",
	    name, gifin_img_width, gifin_img_height,
	    (gifin_interlace_flag ? "interlaced " : ""),
	    gif_version_name[gifin_version],
	    (gifin_l_cmap_flag ? gifin_l_ncolors : gifin_g_ncolors));
}


STATUS gifLoad(char *fullname, ImageOptions *image_ops)
{
     INT32  zf;
     Image *image;
     int    x, y, pixel, pass;
     UINT8  *pixptr;
     int    errno;

     if ((zf = image_file_open((UINT8*)fullname, O_RDONLY)) < 0) {
         perror("gifLoad");
         return ERROR;
     }
     if ((gifin_open_file(zf) != GIFIN_SUCCESS) || /* read GIF header */
         (gifin_open_image()  != GIFIN_SUCCESS)) {  /* read image header */
         gifin_close_file();
         image_file_close(zf);
         return ERROR;
     }

     image = global_image;
     image->width   = gifin_img_width;
     image->height  = gifin_img_height;
     image->depth   = gifin_l_cmap_flag ? gifin_l_pixel_bits:gifin_g_pixel_bits;
     image->pixlen  = (image->depth+7)/8;

     image->new_width   = gifin_img_width;
     image->new_height  = gifin_img_height;

     if(image->width > BYTE_LENGTH_LIMIT){
         gifin_close_file();
         image_file_close(zf);
         return ERROR;
     }
     /* ʹ��local colormap ���� global colormap */
     if (gifin_l_cmap_flag) {
         for (x= 0; x < gifin_l_ncolors; x++) {
             image->rgb.red[x]  = gifin_l_cmap[GIF_RED][x];
             image->rgb.green[x]= gifin_l_cmap[GIF_GRN][x];
             image->rgb.blue[x] = gifin_l_cmap[GIF_BLU][x];
         }
         image->rgb.used = gifin_l_ncolors;
     } 
     else{
         for (x= 0; x < gifin_g_ncolors; x++) {
             image->rgb.red[x]  = gifin_g_cmap[GIF_RED][x];
             image->rgb.green[x]= gifin_g_cmap[GIF_GRN][x];
             image->rgb.blue[x] = gifin_g_cmap[GIF_BLU][x];
         }
         image->rgb.used= gifin_g_ncolors;
     }

     tellAboutImage(fullname);
/* interlaced image -- futz with the vertical trace.  i wish i knew what
 * kind of drugs the GIF people were on when they decided that they
 * needed to support interlacing.
 */
     buildIndex(image, image_ops);
     if (gifin_interlace_flag) {
	  // scanlen= image->height * image->pixlen;
	  /* interlacing takes four passes to read, each starting at a different vertical point. */
         for (pass= 0; pass < 4; pass++) {
             y= interlace_start[pass];
             // scanlen = image->width * image->pixlen * interlace_rate[pass];
             // pixline = image->data + (y * image->width * image->pixlen);
             while (y < gifin_img_height) {
                 //pixptr= pixline;
                pixptr = row_buffer;
                for (x= 0; x < gifin_img_width; x++) {
                    if ((errno = gifin_get_pixel(&pixel)) != GIFIN_SUCCESS) {
                        TRACE("gifLoad: Short read within image data, '%s'\n", get_err_string(errno));
                        y = gifin_img_height;
                        x = gifin_img_width;
                    }
                    valToMem(pixel, pixptr, image->pixlen);
                    pixptr += image->pixlen;
                }
                //pixline += scanlen;
                gif_convert_irgb_to_rgb565(image,image->depth,row_buffer,y,image->data);
                y += interlace_rate[pass];
             }
         }
     }

/* not an interlaced image, just read in sequentially
 */
     else {
         if(image->pixlen == 1) {	/* the usual case */
             for (y= 0; y < gifin_img_height; y++){
                 pixptr= row_buffer;
                 for (x= 0; x < gifin_img_width; x++) {
                     if ((errno = gifin_get_pixel(&pixel)) != GIFIN_SUCCESS) {
                         TRACE("gifLoad: Short read within image data, '%s'\n", get_err_string(errno));
                         y = gifin_img_height; x = gifin_img_width;
                     }
                     valToMem(pixel, pixptr, 1);
                     pixptr += 1;
                 }
                 gif_convert_irgb_to_rgb565(image,image->depth,row_buffer,y,image->data);
             }
         }
         else {	/* less ususal case */
             for (y= 0; y < gifin_img_height; y++){
                 pixptr= row_buffer;
                 for (x= 0; x < gifin_img_width; x++) {
                     if ((errno = gifin_get_pixel(&pixel)) != GIFIN_SUCCESS) {
                         TRACE("gifLoad: Short read within image data, '%s'\n", get_err_string(errno));
                         y = gifin_img_height; 
                         x = gifin_img_width;
                     }
                     valToMem(pixel, pixptr, image->pixlen);
                     pixptr += image->pixlen;
                 }
                 gif_convert_irgb_to_rgb565(image,image->depth,row_buffer,y,image->data);
             }
         }
     }

     image_ops->width   = image->width;
     image_ops->height  = image->height;
	
     gifin_close_file();
     image_file_close(zf);

     return OK;
}

