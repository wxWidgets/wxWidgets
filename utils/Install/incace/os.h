#ifndef __os_h
#define __os_h


#if defined(AMIGA)
  #define DIRSEP '/'
  #define HI_LO_BYTE_ORDER
#endif

#if defined(DOS) || defined(WINNT) || defined(WIN16)
  #define DIRSEP '\\'
  #define LO_HI_BYTE_ORDER
#endif

#if defined(OS2) || defined(__EMX__)
  #define DIRSEP '\\'
  #define LO_HI_BYTE_ORDER
#endif

#if defined(UNIX)
  #define DIRSEP '/'
  #define HI_LO_BYTE_ORDER
#endif


#endif /* __os_h */
