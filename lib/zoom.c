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
 * 在获取image文件的尺寸后调用buildIndex, 在image_misc的irgb->true565转化时完成缩放
 * 最后还需要更新把image下的new_height和new_width更新到height和width
 * @param width 传入的文件值
 * @param zoom 缩放的倍数, 100则不缩放
 * @param rwidth 返回的大小
 * 
 * @return 生成的索引节点的地址
 */
STATUS buildIndex(Image * image,ImageOptions *image_ops)
{
     UINT32 xzoom,yzoom,i;
     UINT16 demand_size[3][2]={{160,128},{320,240},{640,480}};

     xzoom=yzoom=100000;
     
     if(image_ops->zoom>=1 && image_ops->zoom<=3){
	  image->new_width = demand_size[image_ops->zoom-1][0];
	  image->new_height = demand_size[image_ops->zoom-1][1];
     }
     else return ERROR;
     
     // 计算zoom值
     if(image_ops->exactly){
	  xzoom = 100000l*image->new_width/image->width;
	  yzoom = 100000l*image->new_height/image->height;
     }
     else { // 等比例缩放
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

     // 调整缩放后的尺寸
     if(!image_ops->exactly){
	  image->new_width = xzoom * image->width / 100000;
	  image->new_height = yzoom * image->height / 100000;
     }

     for (i= 0; i < image->new_width; i++){
	  if (xzoom!=100000)
	       width_index[i] = (i*100000+50000)/xzoom;
	  else
	       width_index[i] = i;
     }
     for (i= 0; i < image->new_height; i++){
	  if (yzoom!=100000)
	       height_index[i] = (i*100000+50000)/yzoom;
	  else
	       height_index[i] = i;
     }
     return OK;
}
