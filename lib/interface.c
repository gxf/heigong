/**
 * @file   interface.c
 * @author Zhou Qiang
 * @date   Thu Mar 24 11:21:30 2005
 * 
 * @brief  ͼ����ʾ�Ľӿں���
 */
#include "image.h"

extern Image *global_image;
/** 
 *
 * (j,i) <==> (src_x+j*zoom,src_y+i*zoom)
 * (j,i) ==> dest + i*stride+j*2
 * (src_x+j*zoom,src_y+i*zoom) ==> (m,n) ==> image->data+n*image->width*2+m*2;
 * @param dest �����ԭ��ĵ�ַ
 * @param width ���Ŀ��
 * @param heigt ���ĸ߶�
 * @param stride Ŀ��ͼ���stride,��λbyte
 * @param src_x Դͼ���x����
 * @param src_y Դͼ���y����
 * @return �Ƿ�ɹ�
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
