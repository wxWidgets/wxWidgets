/*
   crypt.h (dummy version) by Info-ZIP.      Last revised: 15 Aug 98

   This is a non-functional version of Info-ZIP's crypt.h encryption/
   decryption header file for Zip, ZipCloak, UnZip and fUnZip.  This
   file is not copyrighted and may be distributed without restriction.
   See the "WHERE" file for sites from which to obtain the full crypt
   sources (zcrypt28.zip or later).
 */

#ifndef __crypt_h   /* don't include more than once */
#define __crypt_h

#ifdef CRYPT
#  undef CRYPT
#endif
#define CRYPT  0    /* dummy version */

#define zencode
#define zdecode

#define zfwrite  fwrite

#endif /* !__crypt_h */
