/* This is a dummy zip.h to allow the source files shared with Zip (crypt.c,
   crc32.c, crctab.c, ttyio.c) to compile for UnZip.  */

#ifndef __zip_h   /* don't include more than once */
#define __zip_h

#define UNZIP_INTERNAL
#include "unzip.h"

#define local static

#define ZE_MEM         PK_MEM
#define ziperr(c, h)   return

#endif /* !__zip_h */
