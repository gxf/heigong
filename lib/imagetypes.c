#include "image.h"
#include "imagetypes.h"
#include "image_file.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

extern int errno;
/* some of these are order-dependent */

static struct {
     STATUS (*loader) (char *, ImageOptions *);
     const char *name;
} ImageTypes[] = {
     {jpegLoad,	"JFIF style jpeg Image"},
     {gifLoad,	"GIF Image"},
//     {bmpLoad,	"Windows, OS/2 RLE Image"},
     {NULL,		NULL}
};


Image *global_image;  /**< �ڲ�ͼƬ��ŵĿռ� */
UINT8 *row_buffer,*rgb_row_buffer,*rgb_row_buffer2; /**< �ֱ����л���, ת�����rgb����(��jpeg_convert_irgb_to_rgb���������ŵ�row_buffer������ƽ���õ��Ļ��� */
UINT16 *width_index,*height_index;  /**< ����ϵ����ŵĿռ� */

/** 
 * ���ļ��ж�ȡһ��ͼ��
 * 
 * @param image_ops �μ��ṹ��˵�����ú�����Բ������ϸ�ļ��
 * 
 * @return �������ERROR������Ҫ�����Ĳ���<br>
 * �������OK, �����ʹ��imagefillϵ����䣬�����ҪfreeImage�ͷ��ڴ�!
 */
STATUS loadImage(ImageOptions * image_ops)
{
     UINT8 * fullname = image_ops->fullname;
     int a,fd;

     if(fullname==NULL) 
         return ERROR;
#if 0
     if(image_ops->zoom == 0 || image_ops->zoom > 3) 
         return ERROR;
#endif
     if(image_ops->exactly!=TRUE && image_ops->exactly!=FALSE)
         return ERROR;
     if(image_ops->smooth!=TRUE && image_ops->smooth != FALSE)
         return ERROR;

     fd = image_file_open(fullname,O_RDONLY);
     if(fd<0)
         return ERROR;
     image_file_close(fd);
     
     global_image   = NULL;
     row_buffer     = NULL;
     width_index    = NULL;
     height_index   = NULL;

     width_index = (UINT16*)malloc(FINAL_WIDTH_LENGTH * 2); // at most 640, alloc 650
     if(width_index == NULL)
         goto err;
     height_index = (UINT16*)malloc(FINAL_HEIGHT_LENGTH * 2); // at most 480, alloc 490
     if(height_index == NULL)
         goto err;
     row_buffer = (UINT8*)malloc(BYTE_LENGTH_LIMIT * 4); // pixlen���Ϊ4,length limit 4000
     if(row_buffer == NULL)
         goto err;
     rgb_row_buffer = (UINT8*)malloc(BYTE_LENGTH_LIMIT * 2); // pixlen=2, length limit 4000
     if(rgb_row_buffer == NULL)
         goto err;
     rgb_row_buffer2 = (UINT8*)malloc(BYTE_LENGTH_LIMIT * 2); // �㹻��..
     if(rgb_row_buffer2 == NULL)
         goto err;
     global_image = (Image*)malloc(sizeof(Image));

     if(global_image==NULL)
         goto err;

     for (a = 0; ImageTypes[a].loader; a++) {
	    if(ImageTypes[a].loader((char*)fullname, image_ops)==OK)
	       return OK;
     }

err:
     if(global_image) {free(global_image);global_image=NULL;}
     if(rgb_row_buffer2) {free(rgb_row_buffer2);rgb_row_buffer2=NULL;}     
     if(rgb_row_buffer) {free(rgb_row_buffer);rgb_row_buffer=NULL;}
     if(row_buffer) {free(row_buffer);row_buffer=NULL;}
     if(height_index) {free(height_index);height_index=NULL;}
     if(width_index) {free(width_index);width_index=NULL;}
     return ERROR;
}
/** 
 * �ͷ���Ӧ���ڴ�
 */
STATUS freeImage()
{
     if(global_image) {free(global_image);global_image=NULL;}
     if(rgb_row_buffer2) {free(rgb_row_buffer2);rgb_row_buffer2=NULL;}
     if(rgb_row_buffer) {free(rgb_row_buffer);rgb_row_buffer=NULL;}
     if(row_buffer) {free(row_buffer);row_buffer=NULL;}
     if(height_index) {free(height_index);height_index=NULL;}
     if(width_index) {free(width_index);width_index=NULL;}
     return OK;
}

