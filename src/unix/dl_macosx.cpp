/* dl_macosx.c
 * 
 * Platform:	Mac OS X
 * Author:	Gilles Depeyrot (Gilles.Depeyrot@wanadoo.fr)
 * Based on:    dl_next.xs by Anno Siegel (siegel@zrz.TU-Berlin.DE)
 * Based on:	dl_dlopen.xs by Paul Marquess
 * Created:	Aug 15th, 1994
 *
 */

/* Porting notes:
 *   dl_macosx.c is itself a port from dl_next.xs by Anno Siegel.
 *   dl_next.xs is itself a port from dl_dlopen.xs by Paul Marquess.
 *   The method used here is just to supply the sun style dlopen etc.
 *   functions in terms of NeXTs rld_*.
 */

#include <stdio.h>

static char dl_last_error[1024];

char *dlerror()
{
    return dl_last_error;
}

int dlclose(void *handle) /* stub only */
{
    return 0;
}

#import <mach-o/dyld.h>

enum dyldErrorSource
{
    OFImage,
};

static
void TranslateError(const char *path, enum dyldErrorSource type, int number)
{
  unsigned int index;
  static char *OFIErrorStrings[] =
  {
    "%s(%d): Object Image Load Failure\n",
    "%s(%d): Object Image Load Success\n",
    "%s(%d): Not an recognisable object file\n",
    "%s(%d): No valid architecture\n",
    "%s(%d): Object image has an invalid format\n",
    "%s(%d): Invalid access (permissions?)\n",
    "%s(%d): Unknown error code from NSCreateObjectFileImageFromFile\n",
  };
#define NUM_OFI_ERRORS (sizeof(OFIErrorStrings) / sizeof(OFIErrorStrings[0]))
  
  switch (type)
    {
    case OFImage:
      index = number;
      if (index > NUM_OFI_ERRORS - 1)
	index = NUM_OFI_ERRORS - 1;
      sprintf(dl_last_error, OFIErrorStrings[index], path, number);
      break;
      
    default:
      sprintf(dl_last_error, "%s(%d): Totally unknown error type %d\n",
	      path, number, type);
      break;
    }
}

void *dlopen(const char *path, int mode /* mode is ignored */)
{
  int dyld_result;
  NSObjectFileImage ofile;
  NSModule handle = NULL;
  
  dyld_result = NSCreateObjectFileImageFromFile(path, &ofile);
  if (dyld_result != NSObjectFileImageSuccess)
    {
      TranslateError(path, OFImage, dyld_result);
    }
  else
    {
      // NSLinkModule will cause the run to abort on any link error's
      // not very friendly but the error recovery functionality is limited.
      handle = NSLinkModule(ofile, path, TRUE);
    }
  
  return handle;
}

void *dlsym(void *handle, const char *symbol)
{
  void *addr;
  
  if (NSIsSymbolNameDefined(symbol))
    addr = NSAddressOfSymbol(NSLookupAndBindSymbol(symbol));
  else
    addr = NULL;
  
  return addr;
}
