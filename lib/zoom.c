/**
 * @file   zoom.c
 * @author Zhou Qiang
 * @date   Wed Mar 23 10:58:34 2005
 * 
 * @brief  zoom an image
 */

#include "image.h"
#include "zoom.h"
#include <stdio.h>
#include <stdlib.h>
typedef unsigned long Pixel;	/* what X thinks a pixel is */
extern UINT16 *width_index;
extern UINT16 *height_index;

/** 
 * �ڻ�ȡimage�ļ��ĳߴ�����buildIndex, ��image_misc��irgb->true565ת��ʱ�������
 * �����Ҫ���°�image�µ�new_height��new_width���µ�height��width
 * @param width ������ļ�ֵ
 * @param zoom ���ŵı���, 100������
 * @param rwidth ���صĴ�С
 * 
 * @return ���ɵ������ڵ�ĵ�ַ
 */
STATUS buildIndex(Image * image,ImageOptions *image_ops)
{
     UINT32 xzoom,yzoom,i;
//     UINT16 demand_size[3][2]={{160,128},{320,240},{640,480}};

     xzoom=yzoom=100000;
     
#if 0
     if(image_ops->zoom>=1 && image_ops->zoom<=3){
	if(image_ops->width != 0 && image_ops->height != 0){
	  image->new_width = image_ops->width;
	  image->new_height= image_ops->height;
	}
	else{
	  image->new_width = demand_size[image_ops->zoom-1][0];
	  image->new_height = demand_size[image_ops->zoom-1][1];
	}
     }
     else return ERROR;
     
     // ����zoomֵ
     if(image_ops->exactly){
         xzoom = 100000l*image->new_width/image->width;
         yzoom = 100000l*image->new_height/image->height;
     }
     else { // �ȱ�������
         if(image->width<=image->new_width && image->height<=image->new_height)
             xzoom = yzoom = 100000;
         else{
             int t;
             if(image->width>image->new_width)
                 xzoom = 100000l*image->new_width/image->width;
             if(image->height>image->new_height)
                 yzoom = 100000l*image->new_height/image->height;
             t = xzoom<yzoom?xzoom:yzoom;
             xzoom = t;
             yzoom = t;
         }
     }

     // �������ź�ĳߴ�
     if(!image_ops->exactly){
         image->new_width = xzoom * image->width / 100000;
         image->new_height = yzoom * image->height / 100000;
     }
#endif
     // hgMaster: Do not need zoom here

     if (FINAL_WIDTH_LENGTH < image->new_width){
         width_index = (UINT16*)realloc(width_index, (image->new_width + 10) * sizeof(*width_index));
     }
     if (FINAL_HEIGHT_LENGTH < image->new_height){
         height_index = (UINT16*)realloc(height_index, (image->new_height + 10) * sizeof(*height_index));
     }
     for (i= 0; i < image->new_width; i++){
#if 0
         if (xzoom!=100000)
             width_index[i] = (i*100000+50000)/xzoom;
         else
#endif
             width_index[i] = i;
     }
     for (i= 0; i < image->new_height; i++){
#if 0
         if (yzoom!=100000)
             height_index[i] = (i*100000+50000)/yzoom;
         else
#endif
             height_index[i] = i;
     }
     return OK;
}
