/* inflate.h for UnZip -- put in the public domain by Mark Adler
   version c14f, 23 November 1995 */


/* You can do whatever you like with this source file, though I would
   prefer that if you modify it and redistribute it that you include
   comments to that effect with your name and the date.  Thank you.

   History:
   vers    date          who           what
   ----  ---------  --------------  ------------------------------------
    c14  12 Mar 93  M. Adler        made inflate.c standalone with the
                                    introduction of inflate.h.
    c14d 28 Aug 93  G. Roelofs      replaced flush/FlushOutput with new version
    c14e 29 Sep 93  G. Roelofs      moved everything into unzip.h; added crypt.h
    c14f 23 Nov 95  G. Roelofs      added UNZIP_INTERNAL to accommodate newly
                                    split unzip.h
 */

#define UNZIP_INTERNAL
#include "unzip.h"     /* provides slide[], typedefs and macros */
#ifdef FUNZIP
#  include "crypt.h"   /* provides NEXTBYTE macro for crypt version of funzip */
#endif
