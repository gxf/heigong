#include "image_file.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

INT32 image_file_open(UINT8 * pathname,INT32 flags)
{
     return open((const char*)pathname,flags);
}

INT32 image_file_read(INT32 fd,UINT8* buf,UINT32 count)
{
     return read(fd,buf,count);
}

INT32 image_file_close(INT32 fd)
{
     return close(fd);
}
INT16  image_file_getc(INT32 fd)
{
     UINT8 c;
     if(read(fd,&c,1)==0)
	  return -1;
     return (INT16)(c);
}
UINT32 image_file_tell(INT32 fd)
{
     return lseek(fd,0,SEEK_CUR);
}

void image_file_rewind(INT32 fd)
{
     lseek(fd,0,SEEK_SET);
}

void image_file_unread(INT32 fd,INT32 len)
{
     lseek(fd,-len,SEEK_CUR);
}

