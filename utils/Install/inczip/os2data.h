/*---------------------------------------------------------------------------

  os2data.h

  OS/2-specific structures and data to be included in the global data struc-
  ture.

  ---------------------------------------------------------------------------*/


#define MAXNAMLEN  256
#define MAXPATHLEN 256
#define MAXLEN     256    /* temporary buffer length */
#define IBUF_LEN   4096   /* input buffer length */

#define INCL_NOPM
#define INCL_DOSNLS
#define INCL_DOSPROCESS
#define INCL_DOSDEVICES
#define INCL_DOSDEVIOCTL
#define INCL_DOSERRORS
#define INCL_DOSMISC

#if (defined(OS2DLL) && !defined(DLL))
#  undef OS2DLL
#endif

#ifdef OS2DLL
#  define INCL_REXXSAA
#  include <rexxsaa.h>
#endif

#include <os2.h>


struct direct
{
  ino_t    d_ino;                   /* a bit of a farce */
  int      d_reclen;                /* more farce */
  int      d_namlen;                /* length of d_name */
  char     d_name[MAXNAMLEN + 1];   /* null terminated */
  /* nonstandard fields */
  long     d_size;                  /* size in bytes */
  unsigned d_mode;                  /* MS-DOS or OS/2 file attributes */
  unsigned d_time;
  unsigned d_date;
};

/* The fields d_size and d_mode are extensions by me (Kai Uwe Rommel).  The
 * find_first and find_next calls deliver these data without any extra cost.
 * If these data are needed, the fields save a lot of extra calls to stat()
 * (each stat() again performs a find_first call !).
 */

struct _dircontents
{
  char *_d_entry;
  long _d_size;
  unsigned _d_mode, _d_time, _d_date;
  struct _dircontents *_d_next;
};

typedef struct _dirdesc
{
  int  dd_id;                   /* uniquely identify each open directory */
  long dd_loc;                  /* where we are in directory entry is this */
  struct _dircontents *dd_contents;   /* pointer to contents of dir */
  struct _dircontents *dd_cp;         /* pointer to current position */
}
DIR;


struct os2Global {

#ifndef SFX
   HDIR hdir;
#ifdef __32BIT__
   ULONG count;
   FILEFINDBUF3 find;
#else
   USHORT count;
   FILEFINDBUF find;
#endif
#endif /* !SFX */
  
   int created_dir;        /* used by mapname(), checkdir() */
   int renamed_fullpath;   /* ditto */
   int fnlen;              /* ditto */
#ifdef __32BIT__
   ULONG nLabelDrive;      /* ditto */
#else
   USHORT nLabelDrive;
#endif
   int longnameEA;         /* checkdir(), close_outfile() */
   char *lastpathcomp;     /* ditto */
  
   struct direct dp;
   int lower;
   USHORT nLastDrive, nResult;
  
   DIR *dir;
   char *dirname, *wildname, matchname[FILNAMSIZ];
   int firstcall, have_dirname, dirnamelen;
  
   int rootlen;          /* length of rootpath */
   char *rootpath;       /* user's "extract-to" directory */
   char *buildpathHPFS;  /* full path (so far) to extracted file, */
   char *buildpathFAT;   /*  both HPFS/EA (main) and FAT versions */
   char *endHPFS;        /* corresponding pointers to end of */
   char *endFAT;         /*  buildpath ('\0') */

#ifdef OS2DLL
   char buffer[IBUF_LEN];
   char output_var[MAXLEN];
   char getvar_buf[MAXLEN];
   int getvar_len;
  
   int output_idx;
   int stem_len;
   int putchar_idx;
   int rexx_error;
   char *rexx_mes;

   SHVBLOCK request;
#endif
};


#define SYSTEM_SPECIFIC_GLOBALS struct os2Global os2;
#define SYSTEM_SPECIFIC_CTOR    os2GlobalsCtor

#ifdef OS2DLL
#  ifdef API_DOC
#    define SYSTEM_API_BRIEF    REXXBrief
#    define SYSTEM_API_DETAILS  REXXDetails
     extern char *REXXBrief;
     extern APIDocStruct REXXDetails[];
#  endif
#endif
