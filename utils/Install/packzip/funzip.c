/* funzip.c -- put in the public domain by Mark Adler */

#define VERSION "3.93 of 21 November 1998"


/* You can do whatever you like with this source file, though I would
   prefer that if you modify it and redistribute it that you include
   comments to that effect with your name and the date.  Thank you.

   History:
   vers     date          who           what
   ----   ---------  --------------  ------------------------------------
   1.0    13 Aug 92  M. Adler        really simple unzip filter.
   1.1    13 Aug 92  M. Adler        cleaned up somewhat, give help if
                                     stdin not redirected, warn if more
                                     zip file entries after the first.
   1.2    15 Aug 92  M. Adler        added check of lengths for stored
                                     entries, added more help.
   1.3    16 Aug 92  M. Adler        removed redundant #define's, added
                                     decryption.
   1.4    27 Aug 92  G. Roelofs      added exit(0).
   1.5     1 Sep 92  K. U. Rommel    changed read/write modes for OS/2.
   1.6     6 Sep 92  G. Roelofs      modified to use dummy crypt.c and
                                     crypt.h instead of -DCRYPT.
   1.7    23 Sep 92  G. Roelofs      changed to use DOS_OS2; included
                                     crypt.c under MS-DOS.
   1.8     9 Oct 92  M. Adler        improved inflation error msgs.
   1.9    17 Oct 92  G. Roelofs      changed ULONG/UWORD/byte to ulg/ush/uch;
                                     renamed inflate_entry() to inflate();
                                     adapted to use new, in-place zdecode.
   2.0    22 Oct 92  M. Adler        allow filename argument, prompt for
                                     passwords and don't echo, still allow
                                     command-line password entry, but as an
                                     option.
   2.1    23 Oct 92  J-l. Gailly     fixed crypt/store bug,
                     G. Roelofs      removed crypt.c under MS-DOS, fixed
                                     decryption check to compare single byte.
   2.2    28 Oct 92  G. Roelofs      removed declaration of key.
   2.3    14 Dec 92  M. Adler        replaced fseek (fails on stdin for SCO
                                     Unix V.3.2.4).  added quietflg for
                                     inflate.c.
   3.0    11 May 93  M. Adler        added gzip support
   3.1     9 Jul 93  K. U. Rommel    fixed OS/2 pipe bug (PIPE_ERROR)
   3.2     4 Sep 93  G. Roelofs      moved crc_32_tab[] to tables.h; used FOPx
                                     from unzip.h; nuked OUTB macro and outbuf;
                                     replaced flush(); inlined FlushOutput();
                                     renamed decrypt to encrypted
   3.3    29 Sep 93  G. Roelofs      replaced ReadByte() with NEXTBYTE macro;
                                     revised (restored?) flush(); added FUNZIP
   3.4    21 Oct 93  G. Roelofs      renamed quietflg to qflag; changed outcnt,
                     H. Gessau       second updcrc() arg and flush() arg to ulg;
                                     added inflate_free(); added "g =" to null
                                     getc(in) to avoid compiler warnings
   3.5    31 Oct 93  H. Gessau       changed DOS_OS2 to DOS_NT_OS2
   3.6     6 Dec 93  H. Gessau       added "near" to mask_bits[]
   3.7     9 Dec 93  G. Roelofs      added extent typecasts to fwrite() checks
   3.8    28 Jan 94  GRR/JlG         initialized g variable in main() for gcc
   3.81   22 Feb 94  M. Hanning-Lee  corrected usage message
   3.82   27 Feb 94  G. Roelofs      added some typecasts to avoid warnings
   3.83   22 Jul 94  G. Roelofs      changed fprintf to macro for DLLs
    -      2 Aug 94  -               public release with UnZip 5.11
    -     28 Aug 94  -               public release with UnZip 5.12
   3.84    1 Oct 94  K. U. Rommel    changes for Metaware High C
   3.85   29 Oct 94  G. Roelofs      changed fprintf macro to Info
   3.86    7 May 95  K. Davis        RISCOS patches;
                     P. Kienitz      Amiga patches
   3.87   12 Aug 95  G. Roelofs      inflate_free(), DESTROYGLOBALS fixes
   3.88    4 Sep 95  C. Spieler      reordered macro to work around MSC 5.1 bug
   3.89   22 Nov 95  PK/CS           ifdef'd out updcrc() for ASM_CRC
   3.9    17 Dec 95  G. Roelofs      modified for USE_ZLIB (new fillinbuf())
    -     30 Apr 96  -               public release with UnZip 5.2
   3.91   17 Aug 96  G. Roelofs      main() -> return int (Peter Seebach)
   3.92   13 Apr 97  G. Roelofs      minor cosmetic fixes to messages
    -     22 Apr 97  -               public release with UnZip 5.3
    -     31 May 97  -               public release with UnZip 5.31
   3.93   20 Sep 97  G. Roelofs      minor cosmetic fixes to messages
    -      3 Nov 97  -               public release with UnZip 5.32
    -     28 Nov 98  -               public release with UnZip 5.4
 */


/*

   All funzip does is take a zipfile from stdin and decompress the
   first entry to stdout.  The entry has to be either deflated or
   stored.  If the entry is encrypted, then the decryption password
   must be supplied on the command line as the first argument.

   funzip needs to be linked with inflate.o and crypt.o compiled from
   the unzip source.  If decryption is desired, the full version of
   crypt.c (and crypt.h) from zcrypt21.zip or later must be used.

 */

#define FUNZIP
#define UNZIP_INTERNAL
#include "unzip.h"
#include "crypt.h"
#include "ttyio.h"

#ifdef EBCDIC
#  undef EBCDIC                 /* don't need ebcdic[] */
#endif
#include "tables.h"             /* crc_32_tab[] */

#ifndef USE_ZLIB  /* zlib's function is called inflate(), too */
#  define UZinflate inflate
#endif

/* PKZIP header definitions */
#define ZIPMAG 0x4b50           /* two-byte zip lead-in */
#define LOCREM 0x0403           /* remaining two bytes in zip signature */
#define LOCSIG 0x04034b50L      /* full signature */
#define LOCFLG 4                /* offset of bit flag */
#define  CRPFLG 1               /*  bit for encrypted entry */
#define  EXTFLG 8               /*  bit for extended local header */
#define LOCHOW 6                /* offset of compression method */
#define LOCTIM 8                /* file mod time (for decryption) */
#define LOCCRC 12               /* offset of crc */
#define LOCSIZ 16               /* offset of compressed size */
#define LOCLEN 20               /* offset of uncompressed length */
#define LOCFIL 24               /* offset of file name field length */
#define LOCEXT 26               /* offset of extra field length */
#define LOCHDR 28               /* size of local header, including LOCREM */
#define EXTHDR 16               /* size of extended local header, inc sig */

/* GZIP header definitions */
#define GZPMAG 0x8b1f           /* two-byte gzip lead-in */
#define GZPHOW 0                /* offset of method number */
#define GZPFLG 1                /* offset of gzip flags */
#define  GZPMUL 2               /* bit for multiple-part gzip file */
#define  GZPISX 4               /* bit for extra field present */
#define  GZPISF 8               /* bit for filename present */
#define  GZPISC 16              /* bit for comment present */
#define  GZPISE 32              /* bit for encryption */
#define GZPTIM 2                /* offset of Unix file modification time */
#define GZPEXF 6                /* offset of extra flags */
#define GZPCOS 7                /* offset of operating system compressed on */
#define GZPHDR 8                /* length of minimal gzip header */

/* Macros for getting two-byte and four-byte header values */
#define SH(p) ((ush)(uch)((p)[0]) | ((ush)(uch)((p)[1]) << 8))
#define LG(p) ((ulg)(SH(p)) | ((ulg)(SH((p)+2)) << 16))

/* Function prototypes */
void err OF((int, char *));
int main OF((int, char **));

/* Globals */
FILE *out;                      /* output file (*in moved to G struct) */
ulg outsiz;                     /* total bytes written to out */
int encrypted;                  /* flag to turn on decryption */

/* Masks for inflate.c */
ZCONST ush near mask_bits[] = {
    0x0000,
    0x0001, 0x0003, 0x0007, 0x000f, 0x001f, 0x003f, 0x007f, 0x00ff,
    0x01ff, 0x03ff, 0x07ff, 0x0fff, 0x1fff, 0x3fff, 0x7fff, 0xffff
};


#ifdef USE_ZLIB

int fillinbuf(__G)
__GDEF
/* Fill input buffer for pull-model inflate() in zlib.  Return the number of
 * bytes in inbuf. */
{
/*   GRR: check return value from fread(): same as read()?  check errno? */
  if ((G.incnt = fread((char *)G.inbuf, 1, INBUFSIZ, G.in)) <= 0)
    return 0;
  G.inptr = G.inbuf;

#if CRYPT
  if (encrypted) {
    uch *p;
    int n;

    for (n = G.incnt, p = G.inptr;  n--;  p++)
      zdecode(*p);
  }
#endif /* CRYPT */

  return G.incnt;

}

#endif /* USE_ZLIB */


#if (!defined(USE_ZLIB) || defined(USE_OWN_CRCTAB))
#ifdef USE_ZLIB
ZCONST uLongf *get_crc_table()
{
  return (ZCONST uLongf *)crc_32_tab;
}
#else /* !USE_ZLIB */
ZCONST ulg near *get_crc_table()
{
  return crc_32_tab;
}
#endif /* ?USE_ZLIB */
#endif /* !USE_ZLIB || USE_OWN_CRCTAB */


void err(n, m)
int n;
char *m;
/* Exit on error with a message and a code */
{
  Info(slide, 1, ((char *)slide, "funzip error: %s\n", m));
  DESTROYGLOBALS()
  EXIT(n);
}


int flush(w)    /* used by inflate.c (FLUSH macro) */
ulg w;          /* number of bytes to flush */
{
  G.crc32val = crc32(G.crc32val, slide, (extent)w);
  if (fwrite((char *)slide,1,(extent)w,out) != (extent)w && !PIPE_ERROR)
    err(9, "out of space on stdout");
  outsiz += w;
  return 0;
}


int main(argc, argv)
int argc;
char **argv;
/* Given a zipfile on stdin, decompress the first entry to stdout. */
{
  ush n;
  uch h[LOCHDR];                /* first local header (GZPHDR < LOCHDR) */
  int g = 0;                    /* true if gzip format */
#if CRYPT
  char *s = " [-password]";
  char *p;                      /* password */
#else /* !CRYPT */
  char *s = "";
#endif /* ?CRYPT */
  CONSTRUCTGLOBALS();

  /* skip executable name */
  argc--;
  argv++;

#if CRYPT
  /* get the command line password, if any */
  p = (char *)NULL;
  if (argc && **argv == '-')
  {
    argc--;
    p = 1 + *argv++;
  }
#endif /* CRYPT */

#ifdef MALLOC_WORK
  G.area.Slide = (uch *)calloc(8193, sizeof(short)+sizeof(char)+sizeof(char));
#endif

  /* if no file argument and stdin not redirected, give the user help */
  if (argc == 0 && isatty(0))
  {
    Info(slide, 1, ((char *)slide, "fUnZip (filter UnZip), version %s\n",
      VERSION));
    Info(slide, 1, ((char *)slide, "usage: ... | funzip%s | ...\n", s));
    Info(slide, 1, ((char *)slide, "       ... | funzip%s > outfile\n", s));
    Info(slide, 1, ((char *)slide, "       funzip%s infile.zip > outfile\n",s));
    Info(slide, 1, ((char *)slide, "       funzip%s infile.gz > outfile\n", s));
    Info(slide, 1, ((char *)slide, "Extracts to stdout the gzip file or first\
 zip entry of stdin or the given file.\n"));
    DESTROYGLOBALS()
    EXIT(3);
  }

  /* prepare to be a binary filter */
  if (argc)
  {
    if ((G.in = fopen(*argv, FOPR)) == (FILE *)NULL)
      err(2, "cannot find input file");
  }
  else
  {
#ifdef DOS_FLX_H68_OS2_W32
#if (defined(__HIGHC__) && !defined(FLEXOS))
    setmode(stdin, _BINARY);
#else
    setmode(0, O_BINARY);  /* some buggy C libraries require BOTH setmode() */
#endif                     /*  call AND the fdopen() in binary mode :-( */
#endif /* DOS_FLX_H68_OS2_W32 */

#ifdef RISCOS
    G.in = stdin;
#else
    if ((G.in = fdopen(0, FOPR)) == (FILE *)NULL)
      err(2, "cannot find stdin");
#endif
  }

#ifdef DOS_FLX_H68_OS2_W32
#if (defined(__HIGHC__) && !defined(FLEXOS))
  setmode(stdout, _BINARY);
#else
  setmode(1, O_BINARY);
#endif
#endif /* DOS_FLX_H68_OS2_W32 */

#ifdef RISCOS
  out = stdout;
#else
  if ((out = fdopen(1, FOPW)) == (FILE *)NULL)
    err(2, "cannot write to stdout");
#endif

  /* read local header, check validity, and skip name and extra fields */
  n = getc(G.in);  n |= getc(G.in) << 8;
  if (n == ZIPMAG)
  {
    if (fread((char *)h, 1, LOCHDR, G.in) != LOCHDR || SH(h) != LOCREM)
      err(3, "invalid zipfile");
    if (SH(h + LOCHOW) != STORED && SH(h + LOCHOW) != DEFLATED)
      err(3, "first entry not deflated or stored--cannot unpack");
    for (n = SH(h + LOCFIL); n--; ) g = getc(G.in);
    for (n = SH(h + LOCEXT); n--; ) g = getc(G.in);
    g = 0;
    encrypted = h[LOCFLG] & CRPFLG;
  }
  else if (n == GZPMAG)
  {
    if (fread((char *)h, 1, GZPHDR, G.in) != GZPHDR)
      err(3, "invalid gzip file");
    if (h[GZPHOW] != DEFLATED)
      err(3, "gzip file not deflated");
    if (h[GZPFLG] & GZPMUL)
      err(3, "cannot handle multi-part gzip files");
    if (h[GZPFLG] & GZPISX)
    {
      n = getc(G.in);  n |= getc(G.in) << 8;
      while (n--) g = getc(G.in);
    }
    if (h[GZPFLG] & GZPISF)
      while ((g = getc(G.in)) != 0 && g != EOF) ;
    if (h[GZPFLG] & GZPISC)
      while ((g = getc(G.in)) != 0 && g != EOF) ;
    g = 1;
    encrypted = h[GZPFLG] & GZPISE;
  }
  else
    err(3, "input not a zip or gzip file");

  /* if entry encrypted, decrypt and validate encryption header */
  if (encrypted)
#if CRYPT
    {
      ush i, e;

      if (p == (char *)NULL) {
        if ((p = (char *)malloc(IZ_PWLEN+1)) == (char *)NULL)
          err(1, "out of memory");
        else if ((p = getp("Enter password: ", p, IZ_PWLEN+1)) == (char *)NULL)
          err(1, "no tty to prompt for password");
      }
#if (defined(USE_ZLIB) && !defined(USE_OWN_CRCTAB))
      /* initialize crc_32_tab pointer for decryption */
      CRC_32_TAB = (ZCONST ulg Far *)get_crc_table();
#endif
      init_keys(p);
      for (i = 0; i < RAND_HEAD_LEN; i++)
        e = NEXTBYTE;
      if (e != (ush)(h[LOCFLG] & EXTFLG ? h[LOCTIM + 1] : h[LOCCRC + 3]))
        err(3, "incorrect password for first entry");
    }
#else /* !CRYPT */
    err(3, "cannot decrypt entry (need to recompile with full crypt.c)");
#endif /* ?CRYPT */

  /* prepare output buffer and crc */
  G.outptr = slide;
  G.outcnt = 0L;
  outsiz = 0L;
  G.crc32val = CRCVAL_INITIAL;

  /* decompress */
  if (g || h[LOCHOW])
  {                             /* deflated entry */
    int r;

#ifdef USE_ZLIB
    /* need to allocate and prepare input buffer */
    if ((G.inbuf = (uch *)malloc(INBUFSIZ)) == (uch *)NULL)
       err(1, "out of memory");
#endif /* USE_ZLIB */
    if ((r = UZinflate(__G)) != 0) {
      if (r == 3)
        err(1, "out of memory");
      else
        err(4, "invalid compressed data--format violated");
    }
    inflate_free(__G);
  }
  else
  {                             /* stored entry */
    register ulg n;

    n = LG(h + LOCLEN);
#if CRYPT
    if (n != LG(h + LOCSIZ) - (encrypted ? RAND_HEAD_LEN : 0)) {
#else
    if (n != LG(h + LOCSIZ)) {
#endif
      Info(slide, 1, ((char *)slide, "len %ld, siz %ld\n", n, LG(h + LOCSIZ)));
      err(4, "invalid compressed data--length mismatch");
    }
    while (n--) {
      ush c = getc(G.in);
#if CRYPT
      if (encrypted)
        zdecode(c);
#endif
      *G.outptr++ = (uch)c;
      if (++G.outcnt == WSIZE)    /* do FlushOutput() */
      {
        G.crc32val = crc32(G.crc32val, slide, (extent)G.outcnt);
        if (fwrite((char *)slide, 1,(extent)G.outcnt,out) != (extent)G.outcnt
            && !PIPE_ERROR)
          err(9, "out of space on stdout");
        outsiz += G.outcnt;
        G.outptr = slide;
        G.outcnt = 0L;
      }
    }
  }
  if (G.outcnt)   /* flush one last time; no need to reset G.outptr/outcnt */
  {
    G.crc32val = crc32(G.crc32val, slide, (extent)G.outcnt);
    if (fwrite((char *)slide, 1,(extent)G.outcnt,out) != (extent)G.outcnt
        && !PIPE_ERROR)
      err(9, "out of space on stdout");
    outsiz += G.outcnt;
  }
  fflush(out);

  /* if extended header, get it */
  if (g)
  {
    if (fread((char *)h + LOCCRC, 1, 8, G.in) != 8)
      err(3, "gzip file ended prematurely");
  }
  else
    if ((h[LOCFLG] & EXTFLG) &&
        fread((char *)h + LOCCRC - 4, 1, EXTHDR, G.in) != EXTHDR)
      err(3, "zipfile ended prematurely");

  /* validate decompression */
  if (LG(h + LOCCRC) != G.crc32val)
    err(4, "invalid compressed data--crc error");
  if (LG((g ? (h + LOCSIZ) : (h + LOCLEN))) != outsiz)
    err(4, "invalid compressed data--length error");

  /* check if there are more entries */
  if (!g && fread((char *)h, 1, 4, G.in) == 4 && LG(h) == LOCSIG)
    Info(slide, 1, ((char *)slide,
      "funzip warning: zipfile has more than one entry--rest ignored\n"));

  DESTROYGLOBALS()
  RETURN (0);
}
