#ifndef __attribs_h
#define __attribs_h


#ifndef _A_SUBDIR
  #define _A_SUBDIR 0x10        // MS-DOS directory constant
#endif

#ifndef S_IEXEC
  #ifdef S_IEXECUTE
    #define S_IEXEC S_IEXECUTE
  #else
    #define S_IEXEC 0
  #endif
#endif

#ifndef S_IDELETE
  #define S_IDELETE 0
#endif

#ifndef S_IRGRP 
  #define S_IRGRP 0
  #define S_IWGRP 0
#endif

#ifndef S_IROTH 
  #define S_IROTH 0
  #define S_IWOTH 0
#endif


#endif /* __attribs_h */

