/*---------------------------------------------------------------------------

  unzipstb.c

  Simple stub function for UnZip DLL (or shared library, whatever); does
  exactly the same thing as normal UnZip, except for additional printf()s
  of various version numbers, solely as a demonstration of what can/should
  be checked when using the DLL.  (If major version numbers ever differ,
  assume program is incompatible with DLL--especially if DLL version is
  older.  This is not likely to be a problem with *this* simple program,
  but most user programs will be much more complex.)

  ---------------------------------------------------------------------------*/

#include <stdio.h>
#include "unzip.h"
#include "version.h"

int main(int argc, char *argv[])
{
    static UzpVer *pVersion;   /* no pervert jokes, please... */

    pVersion = UzpVersion();

    printf("UnZip stub:  checking version numbers (DLL is dated %s)\n",
      pVersion->date);
    printf("   UnZip versions:    expecting %d.%d%d, using %d.%d%d%s\n",
      UZ_MAJORVER, UZ_MINORVER, PATCHLEVEL, pVersion->unzip.major,
      pVersion->unzip.minor, pVersion->unzip.patchlevel, pVersion->betalevel);
    printf("   ZipInfo versions:  expecting %d.%d%d, using %d.%d%d\n",
      ZI_MAJORVER, ZI_MINORVER, PATCHLEVEL, pVersion->zipinfo.major,
      pVersion->zipinfo.minor, pVersion->zipinfo.patchlevel);

/*
    D2_M*VER and os2dll.* are obsolete, though retained for compatibility:

    printf("   OS2 DLL versions:  expecting %d.%d%d, using %d.%d%d\n",
      D2_MAJORVER, D2_MINORVER, PATCHLEVEL, pVersion->os2dll.major,
      pVersion->os2dll.minor, pVersion->os2dll.patchlevel);
 */

    if (pVersion->flag & 2)
        printf("   using zlib version %s\n", pVersion->zlib_version);
    printf("\n");

    /* call the actual UnZip routine (string-arguments version) */
    return UzpMain(argc, argv);
}
