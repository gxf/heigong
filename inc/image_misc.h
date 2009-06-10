#include <string.h>
#include <stdlib.h>

#define bcopy(S,D,N) memcpy((char *)(D),(char *)(S),(N))
#define bzero(P,N) memset((P),'\0',(N))
#define bfill(P,N,C) memset((P),(C),(N))

#include "image.h"
void convert_irgb_to_rgb565(Image *image,UINT16 depth,UINT8* row_buffer,UINT32 current_heigth,UINT8* dest);
void gif_convert_irgb_to_rgb565(Image *image,UINT16 depth,UINT8* row_buffer,UINT32 current_height,UINT8* data_source);
void jpg_convert_irgb_to_rgb565(Image *image,UINT16 depth,UINT8* row_buffer,UINT32 current_height,UINT8* data_source,BOOL smooth);
