/*
 *  $Log: jpeg.c,v $
 *  Revision 1.10  2005/03/28 11:40:35  fra
 *  宽度检验, 增加smooth参数
 *
 *  Revision 1.9  2005/03/27 17:46:50  fra
 *  修改了***Load的接口
 *
 *  Revision 1.8  2005/03/27 17:12:19  fra
 *  去掉了Ident系列函数的支持
 *
 *  Revision 1.7  2005/03/27 17:08:13  fra
 *  使用malloc替代静态数组
 *  使用TRACE替代printf
 *
 *  Revision 1.6  2005/03/27 11:39:49  fra
 *  实现先使用jpeg内置1/2,1/4,1/8缩放，再缩放的算法
 *
 *  Revision 1.5  2005/03/27 09:18:24  fra
 *  修改了buildindex函数的接口
 *
 *  Revision 1.4  2005/03/27 01:26:19  fra
 *  *** empty log message ***
 *
 *  Revision 1.3  2005/03/25 11:51:02  fra
 *  纠正从第二行填充的问题
 *
 *  Revision 1.2  2005/03/23 18:13:03  fra
 *  增加了缩放的支持
 *
 */
#include "image.h"
#include "imagetypes.h"
#include "image_file.h"
#include "image_misc.h"
#include "zoom.h"
#include <stdio.h>
#include <jpeglib.h>
#include <jerror.h>
#include <setjmp.h>
#include <assert.h>
#include <ctype.h>

#ifdef DEBUG
#define TRACE(string,args...) printf("<JPEG>:"string,##args)
#define _TRACE(string,args...) printf(string,##args)
#else
#define TRACE(string,args...)
#define _TRACE(string,args...)
#endif

#define RETURN_GAMMA DEFAULT_IRGB_GAMMA
#define INPUT_BUF_SIZE 4096

extern Image *global_image;
extern UINT8 *row_buffer;

typedef struct {
     struct jpeg_error_mgr pub;
     jmp_buf setjmp_buffer;
} fra_jpg_err;

typedef struct {
     struct jpeg_source_mgr pub;
     INT32 fd;
     JOCTET buf[INPUT_BUF_SIZE];
} fra_jpg_src;


static void fra_jpg_error_exit(j_common_ptr cinfo)
{
	fra_jpg_err *err = (fra_jpg_err *) cinfo->err;

	(*cinfo->err->output_message) (cinfo);
	longjmp(err->setjmp_buffer, 1);
}


static void fra_jpg_init_src(j_decompress_ptr cinfo)
{
}


static boolean fra_jpg_fill_buf(j_decompress_ptr cinfo)
{
	fra_jpg_src *src = (fra_jpg_src *) cinfo->src;
	size_t n;

	n = image_file_read(src->fd, src->buf, INPUT_BUF_SIZE);

	if (n <= 0) {
		WARNMS(cinfo, JWRN_JPEG_EOF);
		/* Insert a fake EOI marker */
		src->buf[0] = (JOCTET) 0xFF;
		src->buf[1] = (JOCTET) JPEG_EOI;
		n = 2;
	}
	src->pub.next_input_byte = src->buf;
	src->pub.bytes_in_buffer = n;

	return TRUE;
}


static void fra_jpg_skip(j_decompress_ptr cinfo, long int n)
{
	fra_jpg_src *src = (fra_jpg_src *) cinfo->src;

	if (n > 0) {
		while (n > (long) src->pub.bytes_in_buffer) {
			n -= (long) src->pub.bytes_in_buffer;
			fra_jpg_fill_buf(cinfo);
		}
		src->pub.next_input_byte += (size_t) n;
		src->pub.bytes_in_buffer -= (size_t) n;
	}
}


/*
maybe in future we can exploit zio's rewind capability to
provide a resync_to_restart function here?
*/


static void fra_jpg_term(j_decompress_ptr cinfo)
{
	/* return unread input to the zstream */
	fra_jpg_src *src = (fra_jpg_src *) cinfo->src;

	if (src->fd>=0 && src->pub.bytes_in_buffer > 0) {
		image_file_unread(src->fd,src->pub.bytes_in_buffer);
	}
}


static void fra_jpg_zio_src(j_decompress_ptr cinfo, INT32 fd)
{
	fra_jpg_src *src;

	if (!cinfo->src) {
		cinfo->src = (struct jpeg_source_mgr *)
		    (*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_PERMANENT,
						sizeof(fra_jpg_src));
	}
	src = (fra_jpg_src *) cinfo->src;
	src->pub.init_source = fra_jpg_init_src;
	src->pub.fill_input_buffer = fra_jpg_fill_buf;
	src->pub.skip_input_data = fra_jpg_skip;
	src->pub.resync_to_restart = jpeg_resync_to_restart;	/* use default */
	src->pub.term_source = fra_jpg_term;
	src->fd = fd;
	src->pub.bytes_in_buffer = 0;	/* forces fill_input_buffer on first read */
	src->pub.next_input_byte = NULL;	/* until buffer loaded */
}


static void describe_jpeg(j_decompress_ptr cinfo, char *filename)
{
     TRACE("%s is a %dx%d JPEG image, color space ", filename,
	    cinfo->image_width, cinfo->image_height);

     switch (cinfo->jpeg_color_space) {
     case JCS_UNKNOWN:
	  _TRACE("Unknown");
	  break;
     case JCS_GRAYSCALE:
	  _TRACE("Grayscale");
	  break;
     case JCS_RGB:
	  _TRACE("RGB");
	  break;
     case JCS_YCbCr:
	  _TRACE("YCbCr");
	  break;
     case JCS_CMYK:
	  _TRACE("CMYK");
	  break;
     case JCS_YCCK:
	  _TRACE("YCCK");
	  break;
     default:
	  _TRACE("Totally Weird");
	  break;
     }

     _TRACE(", %d comp%s, %s\n", cinfo->num_components,
	    cinfo->num_components ? "s." : ".",
	    cinfo->arith_code ? "Arithmetic coding" : "Huffman coding");
}

STATUS jpegLoad(char *fullname, ImageOptions *image_ops)
{
     INT32 fd,rowbytes;
     struct jpeg_decompress_struct cinfo;
     fra_jpg_err jerr;
     Image *image = 0;;
     UINT8 *rows = 0;
     UINT16 demand_size[3][2]={{160,128},{320,240},{640,480}};
     
     
     image= global_image;
     fd = image_file_open((UINT8 *)fullname,O_RDONLY);
     if (fd<0)
	  return ERROR;

     /* Quick check to see if file starts with JPEG SOI marker */
     if (image_file_getc(fd) != 0xFF || image_file_getc(fd) != 0xD8) {
	  image_file_close(fd);
	  return ERROR;
     }
     image_file_rewind(fd);

     /* set default error handlers and override error_exit */
     cinfo.err = jpeg_std_error(&jerr.pub);
     jerr.pub.error_exit = fra_jpg_error_exit;

     if (setjmp(jerr.setjmp_buffer)) {
	  jpeg_destroy_decompress(&cinfo);
	  image_file_close(fd);
	  return ERROR;
     }
     jpeg_create_decompress(&cinfo);
     fra_jpg_zio_src(&cinfo, fd);

     jpeg_read_header(&cinfo, TRUE);
     describe_jpeg(&cinfo, fullname);

     cinfo.scale_num = 1;
     cinfo.scale_denom = 1;
     // 如果太宽就出错
     if(cinfo.image_width> BYTE_LENGTH_LIMIT){
	  jpeg_destroy_decompress(&cinfo);
	  image_file_close(fd);
	  return ERROR;
     }
	  
     if(image_ops->zoom>=1 && image_ops->zoom<=3){
	  INT16 i;
	  image->new_width = demand_size[image_ops->zoom-1][0];
	  image->new_height = demand_size[image_ops->zoom-1][1];
	  for(i=3;i>=0;i--){
	       if((cinfo.image_width>>i)>=image->new_width &&(cinfo.image_height>>i)>=image->new_height){
		    cinfo.scale_denom = (1<<i);
		    break;
	       }
	  }
     }
     else{
	  jpeg_destroy_decompress(&cinfo);
	  image_file_close(fd);
	  return ERROR;
     }
     
     jpeg_start_decompress(&cinfo);

     if (JCS_GRAYSCALE == cinfo.out_color_space) {
	  int i;
	  image->width = cinfo.output_width;
	  image->height = cinfo.output_height;
	  image->depth = 8;
	  image->pixlen = 1;
	  for (i = 0; i < 256; i++) {
	       image->rgb.red[i] = image->rgb.green[i] =
		    image->rgb.blue[i] = i;
	  }
	  image->rgb.used = 256;
     } else if (JCS_RGB == cinfo.out_color_space) {
	  image->width = cinfo.output_width;
	  image->height = cinfo.output_height;
	  image->depth = 24;
	  image->pixlen = 3;
     } else {
	  jpeg_destroy_decompress(&cinfo);
	  image_file_close(fd);
	  return ERROR;
     }

     rowbytes = image->width * 2;
     rows = row_buffer;

     if(buildIndex(image,image_ops)==ERROR){
          jpeg_destroy_decompress(&cinfo);
	  image_file_close(fd);
	  return ERROR;
     }

     while (cinfo.output_scanline < cinfo.output_height) {
	  jpeg_read_scanlines(&cinfo, &rows ,1);//+ cinfo.output_scanline,1);
	  jpg_convert_irgb_to_rgb565(image,image->depth,rows,cinfo.output_scanline-1,image->data,image_ops->smooth);
     }

     jpeg_finish_decompress(&cinfo);
     jpeg_destroy_decompress(&cinfo);
     image_file_close(fd);

     image->width = image->new_width;
     image->height = image->new_height;
	
     return OK;
}


