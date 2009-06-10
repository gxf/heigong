/* imagetypes.h:
 *
 * supported image types and the imagetypes array declaration.  when you
 * add a new image type, only the makefile and this header need to be
 * changed.
 *
 * jim frost 10.15.89
 */
#ifndef __IMAGETYPES_H_
#define __IMAGETYPES_H_
STATUS gifLoad(char *fullname, ImageOptions * image_ops);
STATUS jpegLoad(char *fullname, ImageOptions * image_ops);
STATUS bmpLoad(char *fullname, ImageOptions * image_ops);

#endif // __IMAGETYPES_H_
