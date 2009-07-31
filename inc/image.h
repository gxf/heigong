/* image.h:
 * portable image type declarations
 *
 */
#ifndef __IMAGE_H__
#define __IMAGE_H__
#include "IpadTypes.h"

#define MAX_INDEX_COLOR 256
#define BYTE_LENGTH_LIMIT 0x4000 /**< 图片一行最多允许的象素 */
#define FINAL_WIDTH_LENGTH 650
#define FINAL_HEIGHT_LENGTH 490
/// 定义图片类型
#define IBITMAP 0		/* image is a bitmap */
#define IRGB    1		/* image is RGB */
#define ITRUE   2		/* image is true color */

/// 设定图片格式的宏
#define BITMAPP(IMAGE) ((IMAGE)->type == IBITMAP)
#define RGBP(IMAGE)    ((IMAGE)->type == IRGB)
#define TRUEP(IMAGE)   ((IMAGE)->type == ITRUE)

/// TRUE <--> RGB 转化
#define TRUE_RED(PIXVAL)   (((UINT32)((PIXVAL) & 0xff0000)) >> 16)
#define TRUE_GREEN(PIXVAL) (((UINT32)((PIXVAL) & 0xff00)) >> 8)
#define TRUE_BLUE(PIXVAL)   ((UINT32)((PIXVAL) & 0xff))
#define RGB_TO_TRUE(R,G,B) \
  ((((UINT32)((R) & 0xff00)) << 8) | ((G) & 0xff00) | (((unsigned short)(B)) >> 8))

#define memToVal(PTR,LEN) (\
	(LEN) == 1 ? (unsigned long)(*((UINT8 *)(PTR))) : \
	(LEN) == 2 ? (unsigned long)(((unsigned long)(*((UINT8 *)(PTR))))<< 8) \
		+ (*(((UINT8 *)(PTR))+1)) : \
	(LEN) == 3 ? (unsigned long)(((unsigned long)(*((UINT8 *)(PTR))))<<16) \
		+ (((unsigned long)(*(((UINT8 *)(PTR))+1)))<< 8) \
		+ (*(((UINT8 *)(PTR))+2)) : \
	(unsigned long)(((unsigned long)(*((UINT8 *)(PTR))))<<24) \
		+ (((unsigned long)(*(((UINT8 *)(PTR))+1)))<<16) \
		+ (((unsigned long)(*(((UINT8 *)(PTR))+2)))<< 8) \
		+ (*(((UINT8 *)(PTR))+3)))

#define valToMem(VAL,PTR,LEN)  ( \
(LEN) == 1 ? (*((UINT8 *)(PTR)) = (VAL)) : \
(LEN) == 2 ? (*((UINT8 *)(PTR)) = (((UINT32)(VAL))>> 8), \
	*(((UINT8 *)(PTR))+1) = (VAL)) : \
(LEN) == 3 ? (*((UINT8 *)(PTR)) = (((UINT32)(VAL))>>16), \
	*(((UINT8 *)(PTR))+1) = (((UINT32)(VAL))>> 8), \
	*(((UINT8 *)(PTR))+2) = (VAL)) : \
	(*((UINT8 *)(PTR)) = (((UINT32)(VAL))>>24), \
	*(((UINT8 *)(PTR))+1) = (((UINT32)(VAL))>>16), \
	*(((UINT8 *)(PTR))+2) = (((UINT32)(VAL))>> 8), \
	*(((UINT8 *)(PTR))+3) = (VAL)))

typedef struct {
     UINT32 size;	/* size of RGB map */
     UINT32 used;	/* number of colors used in RGB map */
//   BOOL compressed;	/* image uses colormap fully */
     UINT8 red[MAX_INDEX_COLOR];		/* color values in X style */
     UINT8 green[MAX_INDEX_COLOR];
     UINT8 blue[MAX_INDEX_COLOR];
} RGBMap;

/* image structure */

typedef struct {
     UINT8 type;    /* type of image */
     RGBMap rgb;    /* RGB map of image if IRGB type */
     UINT32 width;  /* width of image in pixels */
     UINT32 height; /* height of image in pixels */
     UINT16 depth;  /* depth of image in bits if IRGB type */
     UINT8  data[1200 * 800];    /* data rounded to full byte for each row */
     UINT32 pixlen; /* length of pixel if IRGB type, gif 使用该属性 */
     UINT32 new_width;
     UINT32 new_height;
} Image;


/* image name and option structure used when processing arguments */
typedef struct {
     UINT8 fullname[300];	/**< 文件名，绝对路径 */
     UINT32 zoom;		/**< zoom只能取值1,2或者3,相应的大小1: 160*128, 2: 320*240, 3: 640*480 */
     BOOL exactly;		/**< TRUE 表示缩放到zoom对应的大小， FALSE表示等比例缩放 */
     BOOL smooth;		/**< 平滑，TRUE表示平滑， FALSE不平滑 */
     UINT32 width;	// zoom to certain width
     UINT32 height;	// zoom to certain height 
} ImageOptions;

/* imagetypes.c */
STATUS loadImage(ImageOptions *image_ops);
STATUS freeImage();
STATUS fillImage(UINT8 *dest,UINT32 width,UINT32 heigt, UINT32 stride,INT32 src_x,INT32 src_y);
#endif // __IMAGE_H__
