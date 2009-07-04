#include "image_misc.h"
/** 
 * 转换各种格式到rgb565的格式
 * @param map 调色板
 * @param depth 色深
 * @param row_buffer row buffer 
 * @param width 图片宽度
 * @param dest rgb565目标缓存区
 * $Log: image_misc.c,v $
 * Revision 1.12  2005/03/28 11:44:13  fra
 * jpeg使用了领域平滑算法，先缩放，然后convert to 565
 * 之前每行中的缩放，先转化成565，再平滑的
 *
 * Revision 1.10  2005/03/27 16:00:36  fra
 * 使用malloc替代静态数组
 *
 * Revision 1.9  2005/03/27 11:41:50  fra
 * bug: value = memToVal(source,2), 当width_index[0]==0时出现
 *
 * Revision 1.8  2005/03/23 18:13:57  fra
 * 增加了缩放的函数
 *
 * Revision 1.7  2005/03/23 07:04:22  fra
 * 增加了图片压缩的算法
 *
 * Revision 1.6  2005/03/22 02:59:07  fra
 * 增加了jpeg的转换函数
 *
 * Revision 1.5  2005/03/21 13:05:09  fra
 * 增加了针对gif的irgb->rgb565的转换函数
 *
 */
#define RGB_TO_565(R,G,B) \
  ((((UINT16)((R) & 0xF8)) << 8) | ((UINT16)((G) & 0xFC) << 3) | ((B) >> 3))
extern UINT16 *width_index;
extern UINT16 *height_index;
extern UINT8 *rgb_row_buffer,*rgb_row_buffer2;
static UINT8 need_next_line;
static UINT32 last_found_index;
static void __convert_irgb_to_rgb565(Image *image,UINT16 depth,UINT8* row_buffer,UINT8 *output_row_buffer)
{
     INT32 i, j, index, width;
     UINT8 r,g,b,tmp;
     RGBMap *map = &image->rgb;
     UINT8 * dest = output_row_buffer;
     width = image->width;
     switch(depth){
     case 1:
     {
	  int c,d;
	  d = width / 8;
	  c = width % 8;
	  for(i=0;i<d;i++){
	       tmp = row_buffer[i];
	       for(j=0;j<8;j++){
		    index = (tmp & 0x80)>>7;
		    tmp = tmp<<1;
		    *(UINT16*)(dest+j*2) = RGB_TO_565(map->red[index],map->green[index],map->blue[index]);
	       }
	       dest += 16;
	  }
	  if(c){
	       tmp = row_buffer[i];
	       for(j=0;j<c;j++){
		    index = (tmp & 0x80)>>7;
		    tmp = tmp<<1;
		    *(UINT16*)(dest+j*2) = RGB_TO_565(map->red[index],map->green[index],map->blue[index]);	       	       		    
	       }
	       dest += 2*c;
	  }
	  break;
     }
     case 4:
     {
	  int c,d;
	  d = width / 2;
	  c = width % 2;
	  for(i=0;i<d;i++){
	       tmp = row_buffer[i];
	       index = tmp >> 4;
	       r = map->red[index]>>3;
	       g = map->green[index]>>2;
	       b = map->blue[index]>>3;
	       *(UINT16*)dest = (g<<5)+(r<<11)+b;
	       dest += 2;
	       
	       index = tmp & 0xF;
	       *(UINT16*)dest = RGB_TO_565(map->red[index],map->green[index],map->blue[index]);	       	       
	       dest += 2;
	  }
	  if(c){
	       tmp = row_buffer[i];
	       index = tmp >> 4;
	       *(UINT16*)dest = RGB_TO_565(map->red[index],map->green[index],map->blue[index]);	       
	       dest += 2;
	  }
	  break;
     }
     case 5: // 4 bit rle，由于读取时编码不同，所以采用5来单独处理这个case
     	  for(i=0;i<width;i++){
	       index = row_buffer[i]&0xf;
	       *(UINT16*)dest = RGB_TO_565(map->red[index],map->green[index],map->blue[index]);
	       dest += 2;
	  }
	  break;
     case 8:
	  for(i=0;i<width;i++){
	       index = row_buffer[i];
	       *(UINT16*)dest = RGB_TO_565(map->red[index],map->green[index],map->blue[index]);
	       dest += 2;
	  }
	  break;
     case 16:
	  for(i=0;i<width;i++){
	       index = 2*i;
	       *dest = *(row_buffer+2*i);
	       *(dest+1) = *(row_buffer+2*i+1);
	       dest +=2;
	  }
	  break;
     case 24:
	  for(i=0;i<width;i++){
	       index = 3*i;
	       *(UINT16*)dest = RGB_TO_565(row_buffer[index+2],row_buffer[index+1],row_buffer[index]);
	       dest += 2;
	  }
	  break;
     default:
	  return;
     }
}
static inline UINT32 two_value(UINT8 *source)
{
     UINT8 a[3];
     UINT32 value;
     a[0] = (*source)/2+(*(source+3))/2;
     a[1] = (*(source+1))/2+(*(source+4))/2;
     a[2] = (*(source+2))/2+(*(source+5))/2;
     value = memToVal(a,3);
     return value;
}

static void __jpeg_resize_image(Image * image,UINT8 * in_rgb_row_buffer,UINT8* out_rgb_row_buffer,BOOL smooth)
{
     UINT32 i;
     UINT8 *dest,*source;
     UINT32 value,src=0;

     source = in_rgb_row_buffer;
     dest   = out_rgb_row_buffer;

     if(smooth == TRUE){
         if(width_index[0] == 0 && width_index[1] > 1)
             value = two_value(source);
         else		    
             value = memToVal(source,3);

         for(i = 0;i < image->new_width;i++){
             if(src != width_index[i]){
                 do{
                     src++;
                     source += 3;
                 }
                 while(src != width_index[i]);

                 if(i < image->new_width - 1 && width_index[i+1] > src + 1)
                     value = two_value(source);
                 else
                     value= memToVal(source, 3);
             }
             valToMem(value, dest, 3);
             dest += 3;
         }
     }
     else{
         value = memToVal(source,3);
         for(i=0;i<image->new_width;i++){
             if(src!=width_index[i]){
                 do{
                     src ++;
                     source +=3;
                 }
                 while(src!=width_index[i]);
                 value= memToVal(source, 3);
             }
             valToMem(value,dest,3);
             dest += 3;
         }
     }
}

static void __resize_image(Image *image,UINT32 current_height,UINT32 found_index,UINT8* in_rgb_row_buffer,UINT8* data_source)
{
     UINT32 i,j;
     UINT8 *dest,*source;
     for(i=found_index;i<image->new_height;i++){
	  UINT16 src=0,value;
	  if(height_index[i]!=current_height)
	       return;
	  source = rgb_row_buffer;
	  dest = data_source+image->new_width*2*i;

	  value = memToVal(source,2);
	  for(j=0;j<image->new_width;j++){
	       if(src!=width_index[j]){
		    do{
			 src ++;
			 source +=2;
		    }
		    while(src!=width_index[j]);
		    value= memToVal(source, 2);
	       }
	       valToMem(value,dest,2);
	       dest += 2;
	  }
     }
}

void convert_irgb_to_rgb565(Image *image,UINT16 depth,UINT8* row_buffer,UINT32 current_height,UINT8* data_source)
{
     UINT32 i,found_index;

     for(i=0;i<image->new_height;i++){
	  if(current_height == height_index[i]){ //找到则这行是有用的
	       found_index = i;
	       break;
	  }
     }
     if(i==image->new_height)
	  return; // 这行在新行中没被映射到
     
     __convert_irgb_to_rgb565(image,depth,row_buffer,rgb_row_buffer);
     
     __resize_image(image,current_height,found_index,rgb_row_buffer,data_source);
}
/** 
 * gif的转换，gif有1-8bits,所以独立出来
 * @param map 调色板
 * @param depth 色深
 * @param row_buffer row buffer 
 * @param width 图片宽度
 * @param dest rgb565目标缓存区
  */
void gif_convert_irgb_to_rgb565(Image *image,UINT16 depth,UINT8* row_buffer,UINT32 current_height,UINT8* data_source)
{
     UINT32 i,index;
     UINT8* dest;
     UINT32 width=image->width,found_index;
     RGBMap * map = &image->rgb;

     for(i=0;i<image->new_height;i++){
	  if(current_height == height_index[i]){ //找到则这行是有用的
	       found_index = i;
	       break;
	  }
     }
     if(i==image->new_height)
	  return; // 这行在新行中没被映射到

     dest = rgb_row_buffer; //转换成rgb row buffer

     if(depth>=1 && depth<=8){
	  for(i=0;i<width;i++){
	       index = row_buffer[i];
	       *(UINT16*)dest = RGB_TO_565(map->red[index],map->green[index],map->blue[index]);
	       dest += 2;
	  }
     }
     
     __resize_image(image,current_height,found_index,rgb_row_buffer,data_source);

     return;
}
/** 
 * jpg的转换，jpeg只有8bit和24bit两种，因为24bit是RGB ,而BMP是BGR所以要分开。
 * @param map 调色板
 * @param depth 色深
 * @param row_buffer row buffer 
 * @param width 图片宽度
 * @param dest rgb565目标缓存区
 */
void jpg_convert_irgb_to_rgb565(Image *image, UINT16 depth, UINT8* row_buffer, UINT32 current_height, UINT8* data_source, BOOL smooth)
{
     UINT32 i,index;
     UINT8* dest;
     UINT32 width = image->width,found_index;
     RGBMap * map = &image->rgb;

     if(depth == 24 && smooth == TRUE){
         if(need_next_line == 0){
             for(i = 0;i < image->new_height;i++){
                 if(current_height == height_index[i]){ //找到则这行是有用的
                     found_index = i;
                     if((i < image->new_height - 1) && 
                        (height_index[i+1] > current_height+1)){
                         __jpeg_resize_image(image,row_buffer,rgb_row_buffer2,smooth);
                         need_next_line = 1;
                         last_found_index = i;
                         return;
                     }
                     break;
                 }
             }
             if(i == image->new_height)
                 return; // 这行在新行中没被映射到
         }
         __jpeg_resize_image(image, row_buffer, rgb_row_buffer, smooth);

         if(need_next_line)
             dest = data_source + image->new_width * 2 * last_found_index;
         else
             dest = data_source + image->new_width * 2 * found_index;

         for(i = 0;i < image->new_width;i++){
             index = 3 * i;
             if(need_next_line){
                 *(UINT16*)dest = 
                     RGB_TO_565((rgb_row_buffer2[index]/2 + rgb_row_buffer[index]/2),
                                (rgb_row_buffer2[index+1]/2 + rgb_row_buffer[index+1]/2),
                                (rgb_row_buffer2[index+2]/2 + rgb_row_buffer[index+2]/2));
             }
             else{
                 *(UINT16*)dest = 
                     RGB_TO_565(rgb_row_buffer[index], 
                                rgb_row_buffer[index + 1], 
                                rgb_row_buffer[index + 2]);
             }
             dest += 2;
         }
         if(need_next_line) need_next_line=0;
         return;
     }
     else if(depth == 24 && smooth == FALSE){
         for(i = 0;i < image->new_height; i++){
             if(current_height == height_index[i]){ //找到则这行是有用的
                 found_index = i;
                 break;
             }
         }
         if(i==image->new_height)
             return; // 这行在新行中没被映射到

         __jpeg_resize_image(image,row_buffer,rgb_row_buffer,smooth);
//         memcpy(rgb_row_buffer, row_buffer, image->new_width * 2);

         dest = data_source + image->new_width * 2 * found_index;

         for(i = 0;i < image->new_width;i++){
             index = 3 * i;
             *(UINT16*)dest = 
                 RGB_TO_565(rgb_row_buffer[index],rgb_row_buffer[index+1],rgb_row_buffer[index+2]);
             dest += 2;
         }
         return;
     }
     else if(depth == 8){
         for(i=0;i<image->new_height;i++){
             if(current_height == height_index[i]){ //找到则这行是有用的
                 found_index = i;
                 break;
             }
         }
         if(i==image->new_height)
             return; // 这行在新行中没被映射到

         dest = rgb_row_buffer; //转换成rgb row buffer

         for(i=0;i<width;i++){
             index = row_buffer[i];
             *(UINT16*)dest = RGB_TO_565(map->red[index],map->green[index],map->blue[index]);
             dest += 2;
         }
         __resize_image(image,current_height,found_index,rgb_row_buffer,data_source);
         return;
     }
}
