/*---------------------------------------------------------------------------

  unzip.h (new)

  This header file contains the public macros and typedefs required by
  both the UnZip sources and by any application using the UnZip API.  If
  UNZIP_INTERNAL is defined, it includes unzpriv.h (containing includes,
  prototypes and extern variables used by the actual UnZip sources).

  ---------------------------------------------------------------------------*/


#ifndef __unzip_h   /* prevent multiple inclusions */
#define __unzip_h

/*---------------------------------------------------------------------------
    Predefined, machine-specific macros.
  ---------------------------------------------------------------------------*/

#ifdef __GO32__                 /* MS-DOS extender:  NOT Unix */
#  ifdef unix
#    undef unix
#  endif
#  ifdef __unix
#    undef __unix
#  endif
#  ifdef __unix__
#    undef __unix__
#  endif
#endif

#if ((defined(__convex__) || defined(__convexc__)) && !defined(CONVEX))
#  define CONVEX
#endif

#if (defined(unix) || defined(__unix) || defined(__unix__))
#  ifndef UNIX
#    define UNIX
#  endif
#endif /* unix || __unix || __unix__ */
#if (defined(M_XENIX) || defined(COHERENT) || defined(__hpux))
#  ifndef UNIX
#    define UNIX
#  endif
#endif /* M_XENIX || COHERENT || __hpux */
#if (defined(CONVEX) || defined(MINIX) || defined(_AIX) || defined(__QNX__))
#  ifndef UNIX
#    define UNIX
#  endif
#endif /* CONVEX || MINIX || _AIX || __QNX__ */

#if (defined(VM_CMS) || defined(MVS))
#  define CMS_MVS
#endif

#if (defined(__OS2__) && !defined(OS2))
#  define OS2
#endif

#if (defined(__TANDEM) && !defined(TANDEM))
#  define TANDEM
#endif

#if (defined(__VMS) && !defined(VMS))
#  define VMS
#endif

#if ((defined(__WIN32__) || defined(_WIN32)) && !defined(WIN32))
#  define WIN32
#endif
#if ((defined(__WINNT__) || defined(__WINNT)) && !defined(WIN32))
#  define WIN32
#endif

#ifdef __COMPILER_KCC__
#  include <c-env.h>
#  ifdef SYS_T20
#    define TOPS20
#  endif
#endif /* __COMPILER_KCC__ */

/* Borland C does not define __TURBOC__ if compiling for a 32-bit platform */
#ifdef __BORLANDC__
#  ifndef __TURBOC__
#    define __TURBOC__
#  endif
#  if (!defined(__MSDOS__) && !defined(OS2) && !defined(WIN32))
#    define __MSDOS__
#  endif
#endif

/* define MSDOS for Turbo C (unless OS/2) and Power C as well as Microsoft C */
#ifdef __POWERC
#  define __TURBOC__
#  define MSDOS
#endif /* __POWERC */

#if (defined(__MSDOS__) && !defined(MSDOS))   /* just to make sure */
#  define MSDOS
#endif

/* RSXNTDJ (at least up to v1.3) compiles for WIN32 (RSXNT) using a derivate
   of the EMX environment, but defines MSDOS and __GO32__. ARG !!! */
#if (defined(MSDOS) && defined(WIN32))
#  undef MSDOS                  /* WIN32 is >>>not<<< MSDOS */
#endif
#if (defined(__GO32__) && defined(__EMX__) && defined(__RSXNT__))
#  undef __GO32__
#endif

#if (defined(linux) && !defined(LINUX))
#  define LINUX
#endif

#ifdef __riscos
#  define RISCOS
#endif

#if (defined(THINK_C) || defined(MPW))
#  define MACOS
#endif
#if (defined(__MWERKS__) && defined(macintosh))
#  define MACOS
#endif

/* use prototypes and ANSI libraries if __STDC__, or Microsoft or Borland C, or
 * Silicon Graphics, or Convex?, or IBM C Set/2, or GNU gcc/emx, or Watcom C,
 * or Macintosh, or Windows NT, or Sequent, or Atari or IBM RS/6000.
 */
#if (defined(__STDC__) || defined(MSDOS) || defined(WIN32) || defined(__EMX__))
#  ifndef PROTO
#    define PROTO
#  endif
#  ifndef MODERN
#    define MODERN
#  endif
#endif
#if (defined(__IBMC__) || defined(__BORLANDC__) || defined(__WATCOMC__))
#  ifndef PROTO
#    define PROTO
#  endif
#  ifndef MODERN
#    define MODERN
#  endif
#endif
#if (defined(MACOS) || defined(ATARI_ST) || defined(RISCOS))
#  ifndef PROTO
#    define PROTO
#  endif
#  ifndef MODERN
#    define MODERN
#  endif
#endif
/* Sequent running Dynix/ptx:  non-modern compiler */
#if (defined(_AIX) || defined(sgi) || (defined(_SEQUENT_) && !defined(PTX)))
#  ifndef PROTO
#    define PROTO
#  endif
#  ifndef MODERN
#    define MODERN
#  endif
#endif
#if (defined(CMS_MVS) || defined(__BEOS__))  /* || defined(CONVEX) */
#  ifndef PROTO
#    define PROTO
#  endif
#  ifndef MODERN
#    define MODERN
#  endif
#endif

/* turn off prototypes if requested */
#if (defined(NOPROTO) && defined(PROTO))
#  undef PROTO
#endif

/* used to remove arguments in function prototypes for non-ANSI C */
#ifdef PROTO
#  define OF(a) a
#else
#  define OF(a) ()
#endif

/* enable the "const" keyword only if MODERN and if not otherwise instructed */
#ifdef MODERN
#  if (!defined(ZCONST) && (defined(USE_CONST) || !defined(NO_CONST)))
#    define ZCONST const
#  endif
#endif

#ifndef ZCONST
#  define ZCONST
#endif


/*---------------------------------------------------------------------------
    Grab system-specific public include headers.
  ---------------------------------------------------------------------------*/

#ifdef POCKET_UNZIP             /* WinCE port */
#  include "wince/punzip.h"     /* must appear before windows.h */
#endif

#ifdef WINDLL
#  include <windows.h>
#  include "windll/structs.h"
#endif

/*---------------------------------------------------------------------------
    Grab system-dependent definition of EXPENTRY for prototypes below.
  ---------------------------------------------------------------------------*/

#if 0
#if (defined(OS2) && !defined(FUNZIP))
#  ifdef UNZIP_INTERNAL
#    define INCL_NOPM
#    define INCL_DOSNLS
#    define INCL_DOSPROCESS
#    define INCL_DOSDEVICES
#    define INCL_DOSDEVIOCTL
#    define INCL_DOSERRORS
#    define INCL_DOSMISC
#    ifdef OS2DLL
#      define INCL_REXXSAA
#      include <rexxsaa.h>
#    endif
#  endif /* UNZIP_INTERNAL */
#  include <os2.h>
#  define UZ_EXP EXPENTRY
#endif /* OS2 && !FUNZIP */
#endif /* 0 */

#if (defined(OS2) && !defined(FUNZIP))
#  if (defined(__IBMC__) || defined(__WATCOMC__))
#    define UZ_EXP  _System    /* compiler keyword */
#  else
#    define UZ_EXP
#  endif
#endif /* OS2 && !FUNZIP */

#if (defined(WINDLL) || defined(USE_UNZIP_LIB))
#  ifndef EXPENTRY
#    define UZ_EXP WINAPI
#  else
#    define UZ_EXP EXPENTRY
#  endif
#endif

#ifndef UZ_EXP
#  define UZ_EXP
#endif


/*---------------------------------------------------------------------------
    Public typedefs.
  ---------------------------------------------------------------------------*/

#ifndef _IZ_TYPES_DEFINED
#ifdef MODERN
   typedef void zvoid;
#else /* !MODERN */
#  ifndef AOS_VS         /* mostly modern? */
#    ifndef VAXC         /* not fully modern, but has knows 'void' */
#      define void int
#    endif /* !VAXC */
#  endif /* !AOS_VS */
   typedef char zvoid;
#endif /* ?MODERN */
typedef unsigned char   uch;    /* code assumes unsigned bytes; these type-  */
typedef unsigned short  ush;    /*  defs replace byte/UWORD/ULONG (which are */
typedef unsigned long   ulg;    /*  predefined on some systems) & match zip  */
#define _IZ_TYPES_DEFINED
#endif /* !_IZ_TYPES_DEFINED */

/* InputFn is not yet used and is likely to change: */
#ifdef PROTO
   typedef int   (UZ_EXP MsgFn)     (zvoid *pG, uch *buf, ulg size, int flag);
   typedef int   (UZ_EXP InputFn)   (zvoid *pG, uch *buf, int *size, int flag);
   typedef void  (UZ_EXP PauseFn)   (zvoid *pG, ZCONST char *prompt, int flag);
   typedef int   (UZ_EXP PasswdFn)  (zvoid *pG, int *rcnt, char *pwbuf,
                                     int size, ZCONST char *zfn,
                                     ZCONST char *efn);
   typedef int   (UZ_EXP StatCBFn)  (zvoid *pG, int fnflag, ZCONST char *zfn,
                                     ZCONST char *efn, ZCONST zvoid *details);
   typedef void  (UZ_EXP UsrIniFn)  (void);
#else /* !PROTO */
   typedef int   (UZ_EXP MsgFn)     ();
   typedef int   (UZ_EXP InputFn)   ();
   typedef void  (UZ_EXP PauseFn)   ();
   typedef int   (UZ_EXP PasswdFn)  ();
   typedef int   (UZ_EXP StatCBFn)  ();
   typedef void  (UZ_EXP UsrIniFn)  ();
#endif /* ?PROTO */

typedef struct _UzpBuffer {    /* rxstr */
    ulg   strlength;           /* length of string */
    char  *strptr;             /* pointer to string */
} UzpBuffer;

typedef struct _UzpInit {
    ulg structlen;             /* length of the struct being passed */

    /* GRR: can we assume that each of these is a 32-bit pointer?  if not,
     * does it matter? add "far" keyword to make sure? */
    MsgFn *msgfn;
    InputFn *inputfn;
    PauseFn *pausefn;
    UsrIniFn *userfn;          /* user init function to be called after */
                               /*  globals constructed and initialized */

    /* pointer to program's environment area or something? */
    /* hooks for performance testing? */
    /* hooks for extra unzip -v output? (detect CPU or other hardware?) */
    /* anything else?  let me (Greg) know... */
} UzpInit;

typedef struct _UzpCB {
    ulg structlen;             /* length of the struct being passed */
    /* GRR: can we assume that each of these is a 32-bit pointer?  if not,
     * does it matter? add "far" keyword to make sure? */
    MsgFn *msgfn;
    InputFn *inputfn;
    PauseFn *pausefn;
    PasswdFn *passwdfn;
    StatCBFn *statrepfn;
} UzpCB;

/* the collection of general UnZip option flags and option arguments */
typedef struct _UzpOpts {
#ifndef FUNZIP
    char *exdir;        /* pointer to extraction root directory (-d option) */
    char *pwdarg;       /* pointer to command-line password (-P option) */
    int zipinfo_mode;   /* behave like ZipInfo or like normal UnZip? */
    int aflag;          /* -a: do ASCII-EBCDIC and/or end-of-line translation */
#ifdef VMS
    int bflag;          /* -b: force fixed record format for binary files */
#endif
#ifdef UNIXBACKUP
    int B_flag;         /* -B: back up existing files by renaming to *~ first */
#endif
    int cflag;          /* -c: output to stdout */
    int C_flag;         /* -C: match filenames case-insensitively */
#ifdef MACOS
    int E_flag;         /* -E: [MacOS] show Mac extra field during restoring */
#endif
    int fflag;          /* -f: "freshen" (extract only newer files) */
#if (defined(RISCOS) || defined(ACORN_FTYPE_NFS))
    int acorn_nfs_ext;  /* -F: RISC OS types & NFS filetype extensions */
#endif
    int hflag;          /* -h: header line (zipinfo) */
#ifdef MACOS
    int i_flag;         /* -i: [MacOS] ignore filenames stored in Mac e.f. */
#endif
#ifdef RISCOS
    int scanimage;      /* -I: scan image files */
#endif
    int jflag;          /* -j: junk pathnames (unzip) */
#if (defined(__BEOS__) || defined(MACOS))
    int J_flag;         /* -J: ignore BeOS/MacOS extra field info (unzip) */
#endif
    int lflag;          /* -12slmv: listing format (zipinfo) */
    int L_flag;         /* -L: convert filenames from some OSes to lowercase */
    int overwrite_none; /* -n: never overwrite files (no prompting) */
#ifdef AMIGA
    int N_flag;         /* -N: restore comments as AmigaDOS filenotes */
#endif
    int overwrite_all;  /* -o: OK to overwrite files without prompting */
#endif /* !FUNZIP */
    int qflag;          /* -q: produce a lot less output */
#ifndef FUNZIP
#if (defined(MSDOS) || defined(FLEXOS) || defined(OS2) || defined(WIN32))
    int sflag;          /* -s: convert spaces in filenames to underscores */
#endif
#if (defined(MSDOS) || defined(OS2) || defined(WIN32))
    int volflag;        /* -$: extract volume labels */
#endif
    int tflag;          /* -t: test (unzip) or totals line (zipinfo) */
    int T_flag;         /* -T: timestamps (unzip) or dec. time fmt (zipinfo) */
    int uflag;          /* -u: "update" (extract only newer/brand-new files) */
    int vflag;          /* -v: (verbosely) list directory */
    int V_flag;         /* -V: don't strip VMS version numbers */
#if (defined(TANDEM) || defined(UNIX) || defined(VMS) || defined(__BEOS__))
    int X_flag;         /* -X: restore owner/protection or UID/GID or ACLs */
#endif
#if (defined(OS2) || defined(WIN32))
    int X_flag;         /* -X: restore owner/protection or UID/GID or ACLs */
#endif
    int zflag;          /* -z: display the zipfile comment (only, for unzip) */
#endif /* !FUNZIP */
} UzpOpts;

/* intended to be a private struct: */
typedef struct _ver {
    uch major;              /* e.g., integer 5 */
    uch minor;              /* e.g., 2 */
    uch patchlevel;         /* e.g., 0 */
    uch not_used;
} _version_type;

typedef struct _UzpVer {
    ulg structlen;          /* length of the struct being passed */
    ulg flag;               /* bit 0: is_beta   bit 1: uses_zlib */
    char *betalevel;        /* e.g., "g BETA" or "" */
    char *date;             /* e.g., "4 Sep 95" (beta) or "4 September 1995" */
    char *zlib_version;     /* e.g., "0.95" or NULL */
    _version_type unzip;
    _version_type zipinfo;
    _version_type os2dll;
    _version_type windll;
} UzpVer;

/* for Visual BASIC access to Windows DLLs: */
typedef struct _UzpVer2 {
    ulg structlen;          /* length of the struct being passed */
    ulg flag;               /* bit 0: is_beta   bit 1: uses_zlib */
    char betalevel[10];     /* e.g., "g BETA" or "" */
    char date[20];          /* e.g., "4 Sep 95" (beta) or "4 September 1995" */
    char zlib_version[10];  /* e.g., "0.95" or NULL */
    _version_type unzip;
    _version_type zipinfo;
    _version_type os2dll;
    _version_type windll;
} UzpVer2;

typedef struct central_directory_file_header { /* CENTRAL */
    uch version_made_by[2];
    uch version_needed_to_extract[2];
    ush general_purpose_bit_flag;
    ush compression_method;
    ulg last_mod_dos_datetime;
    ulg crc32;
    ulg csize;
    ulg ucsize;
    ush filename_length;
    ush extra_field_length;
    ush file_comment_length;
    ush disk_number_start;
    ush internal_file_attributes;
    ulg external_file_attributes;
    ulg relative_offset_local_header;
} cdir_file_hdr;


#define UZPINIT_LEN   sizeof(UzpInit)
#define UZPVER_LEN    sizeof(UzpVer)
#define cbList(func)  int (* UZ_EXP func)(char *filename, cdir_file_hdr *crec)


/*---------------------------------------------------------------------------
    Return (and exit) values of the public UnZip API functions.
  ---------------------------------------------------------------------------*/

/* external return codes */
#define PK_OK              0   /* no error */
#define PK_COOL            0   /* no error */
#define PK_GNARLY          0   /* no error */
#define PK_WARN            1   /* warning error */
#define PK_ERR             2   /* error in zipfile */
#define PK_BADERR          3   /* severe error in zipfile */
#define PK_MEM             4   /* insufficient memory (during initialization) */
#define PK_MEM2            5   /* insufficient memory (password failure) */
#define PK_MEM3            6   /* insufficient memory (file decompression) */
#define PK_MEM4            7   /* insufficient memory (memory decompression) */
#define PK_MEM5            8   /* insufficient memory (not yet used) */
#define PK_NOZIP           9   /* zipfile not found */
#define PK_PARAM          10   /* bad or illegal parameters specified */
#define PK_FIND           11   /* no files found */
#define PK_DISK           50   /* disk full */
#define PK_EOF            51   /* unexpected EOF */

#define IZ_CTRLC          80   /* user hit ^C to terminate */
#define IZ_UNSUP          81   /* no files found: all unsup. compr/encrypt. */
#define IZ_BADPWD         82   /* no files found: all had bad password */

/* internal and DLL-only return codes */
#define IZ_DIR            76   /* potential zipfile is a directory */
#define IZ_CREATED_DIR    77   /* directory created: set time and permissions */
#define IZ_VOL_LABEL      78   /* volume label, but can't set on hard disk */
#define IZ_EF_TRUNC       79   /* local extra field truncated (PKZIP'd) */

/* return codes of password fetches (negative = user abort; positive = error) */
#define IZ_PW_ENTERED      0   /* got some password string; use/try it */
#define IZ_PW_CANCEL      -1   /* no password available (for this entry) */
#define IZ_PW_CANCELALL   -2   /* no password, skip any further pwd. request */
#define IZ_PW_ERROR        5   /* = PK_MEM2 : failure (no mem, no tty, ...) */

/* flag values for status callback function */
#define UZ_ST_START_EXTRACT     1
#define UZ_ST_IN_PROGRESS       2
#define UZ_ST_FINISH_MEMBER     3

/* return values of status callback function */
#define UZ_ST_CONTINUE          0
#define UZ_ST_BREAK             1


/*---------------------------------------------------------------------------
    Prototypes for public UnZip API (DLL) functions.
  ---------------------------------------------------------------------------*/

#define  UzpMatch match

int      UZ_EXP UzpMain            OF((int argc, char **argv));
int      UZ_EXP UzpAltMain         OF((int argc, char **argv, UzpInit *init));
UzpVer * UZ_EXP UzpVersion         OF((void));
void     UZ_EXP UzpFreeMemBuffer   OF((UzpBuffer *retstr));
#ifndef WINDLL
int      UZ_EXP UzpUnzipToMemory   OF((char *zip, char *file, UzpOpts *optflgs,
                                       UzpCB *UsrFunc, UzpBuffer *retstr));
#endif
#ifndef WINDLL
   int   UZ_EXP UzpGrep            OF((char *archive, char *file,
                                       char *pattern, int cmd, int SkipBin,
                                       UzpCB *UsrFunc));
#endif
#ifdef OS2
int      UZ_EXP UzpFileTree        OF((char *name, cbList(callBack),
                                       char *cpInclude[], char *cpExclude[]));
#endif

void     UZ_EXP UzpVersion2        OF((UzpVer2 *version));
int      UZ_EXP UzpValidate        OF((char *archive, int AllCodes));


/* default I/O functions (can be swapped out via UzpAltMain() entry point): */

int      UZ_EXP UzpMessagePrnt   OF((zvoid *pG, uch *buf, ulg size, int flag));
int      UZ_EXP UzpMessageNull   OF((zvoid *pG, uch *buf, ulg size, int flag));
int      UZ_EXP UzpInput         OF((zvoid *pG, uch *buf, int *size, int flag));
void     UZ_EXP UzpMorePause     OF((zvoid *pG, ZCONST char *prompt, int flag));
int      UZ_EXP UzpPassword      OF((zvoid *pG, int *rcnt, char *pwbuf,
                                     int size, ZCONST char *zfn,
                                     ZCONST char *efn));


/*---------------------------------------------------------------------------
    Remaining private stuff for UnZip compilation.
  ---------------------------------------------------------------------------*/

#ifdef UNZIP_INTERNAL
#  include "unzpriv.h"
#endif

int pipeit(char *format, ...);
int pipeit2(char *format, int flag, ...);
int confirm(char *format, ...);

#endif /* !__unzip_h */
