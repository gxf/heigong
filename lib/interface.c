/**
 * @file   interface.c
 * @author Zhou Qiang
 * @date   Thu Mar 24 11:21:30 2005
 * 
 * @brief  图像显示的接口函数
 */
#include "image.h"

extern Image *global_image;
/** 
 *
 * (j,i) <==> (src_x+j*zoom,src_y+i*zoom)
 * (j,i) ==> dest + i*stride+j*2
 * (src_x+j*zoom,src_y+i*zoom) ==> (m,n) ==> image->data+n*image->width*2+m*2;
 * @param dest 被填充原点的地址
 * @param width 填充的宽度
 * @param heigt 填充的高度
 * @param stride 目标图像的stride,单位byte
 * @param src_x 源图像的x坐标
 * @param src_y 源图像的y坐标
 * @return 是否成功
 */
STATUS fillImage(UINT8 *data_source,UINT32 width,UINT32 height, UINT32 stride,INT32 src_x,INT32 src_y)
{
     
     UINT32 i,j,x,y;
     UINT8 * source,*dest;
     Image * image = global_image;

     for(i=0;i<height;i++){
	  dest = data_source + i*stride;
	  source = image->data + (src_y+i)*image->width*2;
	  for(j=0;j<width;j++){
	       x = j + src_x;
	       y = i + src_y;
	       if(x>=image->width || y>=image->height)
		    *(UINT16*)dest = 0;
	       else
		    *(UINT16*)dest = *(UINT16*)(source + x*2);
	       dest+=2;
	  }
     }
     return OK;
}
