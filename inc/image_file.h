#include "IpadTypes.h"

#define O_ACCESS   003          /* (&) O_ACCESS get the access mode */
#define O_RDONLY   00           /* +1 == FREAD */
#define O_WRONLY   01           /* +1 == FWRITE */
#define O_RDWR	   02           /* +1 == FREAD|FWRITE */

INT32 image_file_open(UINT8 * pathname,INT32 flags);
INT32 image_file_read(INT32 fd,UINT8* buf,UINT32 count);
INT32 image_file_close(INT32 fd);
INT16  image_file_getc(INT32 fd);
UINT32 image_file_tell(INT32 fd);
void image_file_rewind(INT32 fd);
void image_file_unread(INT32 fd,INT32 len);
