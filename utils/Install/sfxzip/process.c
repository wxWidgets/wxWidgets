/*---------------------------------------------------------------------------

  process.c

  This file contains the top-level routines for processing multiple zipfiles.

  Contains:  process_zipfiles()
             free_G_buffers()
             do_seekable()
             find_ecrec()
             uz_end_central()
             process_cdir_file_hdr()
             get_cdir_ent()
             process_local_file_hdr()
             ef_scan_for_izux()

  ---------------------------------------------------------------------------*/


#define UNZIP_INTERNAL
#include "unzip.h"
#ifdef WINDLL
#  ifdef POCKET_UNZIP
#    include "wince/intrface.h"
#  else
#    include "windll/windll.h"
#  endif
#endif

#include "install.h"
#include "instsup.h"
extern int installstate;

off_t acelseek(off_t offset, int whence);
int aceread(void *buf, size_t count);
int aceopen(const char *path, int flags);
int aceclose(int fd);
int acesize(void);
int acetell(int fd);
char *replacestr(char *str1, char *str2, char *str3);

static int    do_seekable        OF((__GPRO__ int lastchance));
static int    find_ecrec         OF((__GPRO__ long searchlen));

static ZCONST char Far CannotAllocateBuffers[] =
  "error:  cannot allocate unzip buffers\n";

#ifdef SFX
   static ZCONST char Far CannotFindMyself[] =
     "unzipsfx:  cannot find myself! [%s]\n";

#else /* !SFX */
   /* process_zipfiles() strings */
# if (defined(IZ_CHECK_TZ) && defined(USE_EF_UT_TIME))
     static ZCONST char Far WarnInvalidTZ[] =
       "Warning: TZ environment variable not found, cannot use UTC times!!\n";
# endif
   static ZCONST char Far FilesProcessOK[] =
     "%d archive%s successfully processed.\n";
   static ZCONST char Far ArchiveWarning[] =
     "%d archive%s had warnings but no fatal errors.\n";
   static ZCONST char Far ArchiveFatalError[] =
     "%d archive%s had fatal errors.\n";
   static ZCONST char Far FileHadNoZipfileDir[] =
     "%d file%s had no zipfile directory.\n";
   static ZCONST char Far ZipfileWasDir[] = "1 \"zipfile\" was a directory.\n";
   static ZCONST char Far ManyZipfilesWereDir[] =
     "%d \"zipfiles\" were directories.\n";
   static ZCONST char Far NoZipfileFound[] = "No zipfiles found.\n";

   /* do_seekable() strings */
# ifdef UNIX
   static ZCONST char Far CannotFindZipfileDirMsg[] =
     "%s:  cannot find zipfile directory in one of %s or\n\
        %s%s.zip, and cannot find %s, period.\n";
   static ZCONST char Far CannotFindEitherZipfile[] =
     "%s:  cannot find %s, %s.zip or %s.\n";   /* ", so there" removed 970918 */
# else /* !UNIX */
# ifndef AMIGA
   static ZCONST char Far CannotFindWildcardMatch[] =
     "%s:  cannot find any matches for wildcard specification \"%s\".\n";
# endif /* !AMIGA */
   static ZCONST char Far CannotFindZipfileDirMsg[] =
     "%s:  cannot find zipfile directory in %s,\n\
        %sand cannot find %s, period.\n";
   static ZCONST char Far CannotFindEitherZipfile[] =
     "%s:  cannot find either %s or %s.\n";    /* ", so there" removed 970918 */
# endif /* ?UNIX */
   extern ZCONST char Far Zipnfo[];       /* in unzip.c */
#ifndef WINDLL
   static ZCONST char Far Unzip[] = "unzip";
#else
   static ZCONST char Far Unzip[] = "UnZip DLL";
#endif
   static ZCONST char Far MaybeExe[] =
     "note:  %s may be a plain executable, not an archive\n";
   static ZCONST char Far CentDirNotInZipMsg[] = "\n\
   [%s]:\n\
     Zipfile is disk %u of a multi-disk archive, and this is not the disk on\n\
     which the central zipfile directory begins (disk %u).\n";
   static ZCONST char Far EndCentDirBogus[] =
     "\nwarning [%s]:  end-of-central-directory record claims this\n\
  is disk %u but that the central directory starts on disk %u; this is a\n\
  contradiction.  Attempting to process anyway.\n";
# ifdef NO_MULTIPART
   static ZCONST char Far NoMultiDiskArcSupport[] =
     "\nerror [%s]:  zipfile is part of multi-disk archive\n\
  (sorry, not yet supported).\n";
   static ZCONST char Far MaybePakBug[] = "warning [%s]:\
  zipfile claims to be 2nd disk of a 2-part archive;\n\
  attempting to process anyway.  If no further errors occur, this archive\n\
  was probably created by PAK v2.51 or earlier.  This bug was reported to\n\
  NoGate in March 1991 and was supposed to have been fixed by mid-1991; as\n\
  of mid-1992 it still hadn't been.  (If further errors do occur, archive\n\
  was probably created by PKZIP 2.04c or later; UnZip does not yet support\n\
  multi-part archives.)\n";
# else
   static ZCONST char Far MaybePakBug[] = "warning [%s]:\
  zipfile claims to be last disk of a multi-part archive;\n\
  attempting to process anyway, assuming all parts have been concatenated\n\
  together in order.  Expect \"errors\" and warnings...true multi-part support\
\n  doesn't exist yet (coming soon).\n";
# endif
   static ZCONST char Far ExtraBytesAtStart[] =
     "warning [%s]:  %ld extra byte%s at beginning or within zipfile\n\
  (attempting to process anyway)\n";
#endif /* ?SFX */

static ZCONST char Far MissingBytes[] =
  "error [%s]:  missing %ld bytes in zipfile\n\
  (attempting to process anyway)\n";
static ZCONST char Far NullCentDirOffset[] =
  "error [%s]:  NULL central directory offset\n\
  (attempting to process anyway)\n";
static ZCONST char Far ZipfileEmpty[] = "warning [%s]:  zipfile is empty\n";
static ZCONST char Far CentDirStartNotFound[] =
  "error [%s]:  start of central directory not found;\n\
  zipfile corrupt.\n%s";
#ifndef SFX
   static ZCONST char Far CentDirTooLong[] =
     "error [%s]:  reported length of central directory is\n\
  %ld bytes too long (Atari STZip zipfile?  J.H.Holm ZIPSPLIT 1.1\n\
  zipfile?).  Compensating...\n";
   static ZCONST char Far CentDirEndSigNotFound[] = "\
  End-of-central-directory signature not found.  Either this file is not\n\
  a zipfile, or it constitutes one disk of a multi-part archive.  In the\n\
  latter case the central directory and zipfile comment will be found on\n\
  the last disk(s) of this archive.\n";
#else /* SFX */
   static ZCONST char Far CentDirEndSigNotFound[] =
     "  End-of-central-directory signature not found.\n";
#endif /* ?SFX */
static ZCONST char Far ZipfileCommTrunc1[] =
  "\ncaution:  zipfile comment truncated\n";




/*******************************/
/* Function process_zipfiles() */
/*******************************/

int process_zipfiles(__G)    /* return PK-type error code */
    __GDEF
{
#ifndef SFX
    char *lastzipfn = (char *)NULL;
    int NumWinFiles, NumLoseFiles, NumWarnFiles;
    int NumMissDirs, NumMissFiles;
#endif
    int error=0, error_in_archive=0;
	extern unsigned current_file;
	extern char installdir2[];


/*---------------------------------------------------------------------------
    Start by allocating buffers and (re)constructing the various PK signature
    strings.
  ---------------------------------------------------------------------------*/

    G.inbuf = (uch *)malloc(INBUFSIZ + 4);    /* 4 extra for hold[] (below) */
    G.outbuf = (uch *)malloc(OUTBUFSIZ + 1);  /* 1 extra for string term. */

    if ((G.inbuf == (uch *)NULL) || (G.outbuf == (uch *)NULL)) {
        Info(slide, 0x401, ((char *)slide,
          LoadFarString(CannotAllocateBuffers)));
        return(PK_MEM);
    }
    G.hold = G.inbuf + INBUFSIZ;     /* to check for boundary-spanning sigs */
#ifndef VMS     /* VMS uses its own buffer scheme for textmode flush(). */
#ifdef SMALL_MEM
    G.outbuf2 = G.outbuf+RAWBUFSIZ;  /* never changes */
#endif
#endif /* !VMS */

#if 0 /* CRC_32_TAB has been NULLified by CONSTRUCTGLOBALS !!!! */
    /* allocate the CRC table only later when we know we have a zipfile */
    CRC_32_TAB = NULL;
#endif /* 0 */

    /* finish up initialization of magic signature strings */
    local_hdr_sig[0]  /* = extd_local_sig[0] */  = 0x50;   /* ASCII 'P', */
    central_hdr_sig[0] = end_central_sig[0] = 0x50;     /* not EBCDIC */

    local_hdr_sig[1]  /* = extd_local_sig[1] */  = 0x4B;   /* ASCII 'K', */
    central_hdr_sig[1] = end_central_sig[1] = 0x4B;     /* not EBCDIC */

/*---------------------------------------------------------------------------
    Make sure timezone info is set correctly; localtime() returns GMT on
    some OSes (e.g., Solaris 2.x) if this isn't done first.  The ifdefs were
    initially copied from dos_to_unix_time() in fileio.c. probably, they are
    still too strict; any listed OS that supplies tzset(), regardless of
    whether the function does anything, should be removed from the ifdefs.
  ---------------------------------------------------------------------------*/

#if (defined(IZ_CHECK_TZ) && defined(USE_EF_UT_TIME))
#  ifndef VALID_TIMEZONE
#     define VALID_TIMEZONE(tmp) \
             (((tmp = getenv("TZ")) != NULL) && (*tmp != '\0'))
#  endif
    {
        char *p;
        G.tz_is_valid = VALID_TIMEZONE(p);
#  ifndef SFX
        if (!G.tz_is_valid) {
            Info(slide, 0x401, ((char *)slide, LoadFarString(WarnInvalidTZ)));
            error_in_archive = error = PK_WARN;
        }
#  endif /* !SFX */
    }
#endif /* IZ_CHECK_TZ && USE_EF_UT_TIME */

/* For systems that do not have tzset() but supply this function using another
   name (_tzset() or something similar), an appropiate "#define tzset ..."
   should be added to the system specifc configuration section.  */
#if (!defined(T20_VMS) && !defined(MACOS) && !defined(RISCOS) && !defined(QDOS))
#if (!defined(BSD) && !defined(MTS) && !defined(CMS_MVS) && !defined(TANDEM))
    tzset();
#endif
#endif

/*---------------------------------------------------------------------------
    Match (possible) wildcard zipfile specification with existing files and
    attempt to process each.  If no hits, try again after appending ".zip"
    suffix.  If still no luck, give up.
  ---------------------------------------------------------------------------*/

#ifdef SFX
    if ((error = do_seekable(__G__ 0)) == PK_NOZIP) {
#ifdef EXE_EXTENSION
        int len=strlen(G.argv0);

        /* append .exe if appropriate; also .sfx? */
        if ( (G.zipfn = (char *)malloc(len+sizeof(EXE_EXTENSION))) !=
             (char *)NULL ) {
            strcpy(G.zipfn, G.argv0);
            strcpy(G.zipfn+len, EXE_EXTENSION);
            error = do_seekable(__G__ 0);
            free(G.zipfn);
            G.zipfn = G.argv0;  /* for "cannot find myself" message only */
        }
#endif /* EXE_EXTENSION */
#ifdef WIN32
        G.zipfn = G.argv0;  /* for "cannot find myself" message only */
#endif
    }
    if (error) {
        if (error == IZ_DIR)
            error_in_archive = PK_NOZIP;
        else
            error_in_archive = error;
        if (error == PK_NOZIP)
            Info(slide, 1, ((char *)slide, LoadFarString(CannotFindMyself),
              G.zipfn));
    }

#else /* !SFX */
    NumWinFiles = NumLoseFiles = NumWarnFiles = 0;
    current_file = NumMissDirs = NumMissFiles = 0;

    while ((G.zipfn = do_wild(__G__ G.wildzipfn)) != (char *)NULL) {
        Trace((stderr, "do_wild( %s ) returns %s\n", G.wildzipfn, G.zipfn));

        lastzipfn = G.zipfn;

        /* print a blank line between the output of different zipfiles */
        if (!uO.qflag  &&  error != PK_NOZIP  &&  error != IZ_DIR
#ifdef TIMESTAMP
            && (!uO.T_flag || uO.zipinfo_mode)
#endif
            && (NumWinFiles+NumLoseFiles+NumWarnFiles+NumMissFiles) > 0)
            (*G.message)((zvoid *)&G, (uch *)"\n", 1L, 0);

        if ((error = do_seekable(__G__ 0)) == PK_WARN)
            ++NumWarnFiles;
        else if (error == IZ_DIR)
            ++NumMissDirs;
        else if (error == PK_NOZIP)
            ++NumMissFiles;
        else if (error)
            ++NumLoseFiles;
        else
			++NumWinFiles;

		current_file = NumWinFiles;

		if(installstate == INSTALLING)
			DoGUI();

        if (error != IZ_DIR && error > error_in_archive)
            error_in_archive = error;
        Trace((stderr, "do_seekable(0) returns %d\n", error));
#ifdef WINDLL
        if (error == IZ_CTRLC) {
            free_G_buffers(__G);
            return error;
        }
#endif

    } /* end while-loop (wildcard zipfiles) */

    if ((NumWinFiles + NumWarnFiles + NumLoseFiles) == 0  &&
        (NumMissDirs + NumMissFiles) == 1  &&  lastzipfn != (char *)NULL)
    {
        NumMissDirs = NumMissFiles = 0;
        if (error_in_archive == PK_NOZIP)
            error_in_archive = PK_COOL;

#if (!defined(UNIX) && !defined(AMIGA)) /* filenames with wildcard characters */
        if (iswild(G.wildzipfn))
            Info(slide, 0x401, ((char *)slide,
              LoadFarString(CannotFindWildcardMatch), uO.zipinfo_mode?
              LoadFarStringSmall(Zipnfo) : LoadFarStringSmall(Unzip),
              G.wildzipfn));
        else
#endif
        {
            char *p = lastzipfn + strlen(lastzipfn);

            G.zipfn = lastzipfn;
            strcpy(p, ZSUFX);

#if defined(UNIX) || defined(QDOS)
   /* only Unix has case-sensitive filesystems */
   /* Well FlexOS (sometimes) also has them,  but support is per media */
   /* and a pig to code for,  so treat as case insensitive for now */
   /* we do this under QDOS to check for .zip as well as _zip */
            if ((error = do_seekable(__G__ 0)) == PK_NOZIP || error == IZ_DIR) {
                if (error == IZ_DIR)
                    ++NumMissDirs;
                strcpy(p, ALT_ZSUFX);
                error = do_seekable(__G__ 1);
            }
#else
            error = do_seekable(__G__ 1);
#endif
            if (error == PK_WARN)   /* GRR: make this a switch/case stmt ... */
                ++NumWarnFiles;
            else if (error == IZ_DIR)
                ++NumMissDirs;
            else if (error == PK_NOZIP)
                /* increment again => bug: "1 file had no zipfile directory." */
                /* ++NumMissFiles */ ;
            else if (error)
                ++NumLoseFiles;
            else
                ++NumWinFiles;

            if (error > error_in_archive)
                error_in_archive = error;
            Trace((stderr, "do_seekable(1) returns %d\n", error));
#ifdef WINDLL
            if (error == IZ_CTRLC) {
                free_G_buffers(__G);
                return error;
            }
#endif
        }
    }
#endif /* ?SFX */

/*---------------------------------------------------------------------------
    Print summary of all zipfiles, assuming zipfile spec was a wildcard (no
    need for a summary if just one zipfile).
  ---------------------------------------------------------------------------*/

#ifndef SFX
    if (iswild(G.wildzipfn) && uO.qflag < 3
#ifdef TIMESTAMP
        && !(uO.T_flag && uO.qflag && !uO.zipinfo_mode)
#endif
                                                    )
    {
        if ((NumMissFiles + NumLoseFiles + NumWarnFiles > 0 || NumWinFiles != 1)
#ifdef TIMESTAMP
            && !(uO.T_flag && !uO.zipinfo_mode)
#endif
            && !(uO.tflag && uO.qflag > 1))
            (*G.message)((zvoid *)&G, (uch *)"\n", 1L, 0x401);
        if ((NumWinFiles > 1) || (NumWinFiles == 1 &&
            NumMissDirs + NumMissFiles + NumLoseFiles + NumWarnFiles > 0))
            Info(slide, 0x401, ((char *)slide, LoadFarString(FilesProcessOK),
              NumWinFiles, (NumWinFiles == 1)? " was" : "s were"));
        if (NumWarnFiles > 0)
            Info(slide, 0x401, ((char *)slide, LoadFarString(ArchiveWarning),
              NumWarnFiles, (NumWarnFiles == 1)? "" : "s"));
        if (NumLoseFiles > 0)
            Info(slide, 0x401, ((char *)slide, LoadFarString(ArchiveFatalError),
              NumLoseFiles, (NumLoseFiles == 1)? "" : "s"));
        if (NumMissFiles > 0)
            Info(slide, 0x401, ((char *)slide,
              LoadFarString(FileHadNoZipfileDir), NumMissFiles,
              (NumMissFiles == 1)? "" : "s"));
        if (NumMissDirs == 1)
            Info(slide, 0x401, ((char *)slide, LoadFarString(ZipfileWasDir)));
        else if (NumMissDirs > 0)
            Info(slide, 0x401, ((char *)slide,
              LoadFarString(ManyZipfilesWereDir), NumMissDirs));
        if (NumWinFiles + NumLoseFiles + NumWarnFiles == 0)
            Info(slide, 0x401, ((char *)slide, LoadFarString(NoZipfileFound)));
    }
#endif /* !SFX */

    /* free allocated memory */
    free_G_buffers(__G);

    return error_in_archive;

} /* end function process_zipfiles() */





/*****************************/
/* Function free_G_buffers() */
/*****************************/

void free_G_buffers(__G)     /* releases all memory allocated in global vars */
    __GDEF
{
    inflate_free(__G);
    checkdir(__G__ (char *)NULL, END);

#ifdef DYNALLOC_CRCTAB
    if (CRC_32_TAB) {
        free_crc_table();
        CRC_32_TAB = NULL;
    }
#endif

   if (G.key != (char *)NULL) {
        free(G.key);
        G.key = (char *)NULL;
   }

#if (!defined(VMS) && !defined(SMALL_MEM))
    /* VMS uses its own buffer scheme for textmode flush() */
    if (G.outbuf2) {
        free(G.outbuf2);   /* malloc'd ONLY if unshrink and -a */
        G.outbuf2 = (uch *)NULL;
    }
#endif

    if (G.outbuf)
        free(G.outbuf);
    if (G.inbuf)
        free(G.inbuf);
    G.inbuf = G.outbuf = (uch *)NULL;

#ifdef MALLOC_WORK
    if (G.area.Slide) {
        free(G.area.Slide);
        G.area.Slide = (uch *)NULL;
    }
#endif

} /* end function free_G_buffers() */





/**************************/
/* Function do_seekable() */
/**************************/

static int do_seekable(__G__ lastchance)        /* return PK-type error code */
    __GDEF
    int lastchance;
{
#ifndef SFX
    /* static int no_ecrec = FALSE;  SKM: moved to globals.h */
    int maybe_exe=FALSE;
    int too_weird_to_continue=FALSE;
#ifdef TIMESTAMP
    time_t uxstamp;
    unsigned nmember = 0;
#endif
#endif
    int error=0, error_in_archive;


/*---------------------------------------------------------------------------
    Open the zipfile for reading in BINARY mode to prevent CR/LF translation,
    which would corrupt the bit streams.
  ---------------------------------------------------------------------------*/

#if 0
    if (SSTAT(G.zipfn, &G.statbuf) ||
        (error = S_ISDIR(G.statbuf.st_mode)) != 0)
    {
#ifndef SFX
        if (lastchance) {
#if defined(UNIX) || defined(QDOS)
            if (G.no_ecrec)
                Info(slide, 1, ((char *)slide,
                  LoadFarString(CannotFindZipfileDirMsg), uO.zipinfo_mode?
                  LoadFarStringSmall(Zipnfo) : LoadFarStringSmall(Unzip),
                  G.wildzipfn, uO.zipinfo_mode? "  " : "", G.wildzipfn,
                  G.zipfn));
            else
                Info(slide, 1, ((char *)slide,
                  LoadFarString(CannotFindEitherZipfile), uO.zipinfo_mode?
                  LoadFarStringSmall(Zipnfo) : LoadFarStringSmall(Unzip),
                  G.wildzipfn, G.wildzipfn, G.zipfn));
#else /* !UNIX */
            if (G.no_ecrec)
                Info(slide, 0x401, ((char *)slide,
                  LoadFarString(CannotFindZipfileDirMsg), uO.zipinfo_mode?
                  LoadFarStringSmall(Zipnfo) : LoadFarStringSmall(Unzip),
                  G.wildzipfn, uO.zipinfo_mode? "  " : "", G.zipfn));
            else
                Info(slide, 0x401, ((char *)slide,
                  LoadFarString(CannotFindEitherZipfile), uO.zipinfo_mode?
                  LoadFarStringSmall(Zipnfo) : LoadFarStringSmall(Unzip),
                  G.wildzipfn, G.zipfn));
#endif /* ?UNIX */
        }
#endif /* !SFX */
        return error? IZ_DIR : PK_NOZIP;
	}
    G.ziplen = G.statbuf.st_size;
#endif
	G.ziplen = acesize();

#ifndef SFX
#if defined(UNIX) || defined(DOS_OS2_W32)
    if (G.statbuf.st_mode & S_IEXEC)   /* no extension on Unix exes:  might */
        maybe_exe = TRUE;               /*  find unzip, not unzip.zip; etc. */
#endif
#endif /* !SFX */

#ifdef VMS
    if (check_format(__G))              /* check for variable-length format */
        return PK_ERR;
#endif

    if (open_input_file(__G))   /* this should never happen, given */
        return PK_NOZIP;        /*  the stat() test above, but... */

/*---------------------------------------------------------------------------
    Find and process the end-of-central-directory header.  UnZip need only
    check last 65557 bytes of zipfile:  comment may be up to 65535, end-of-
    central-directory record is 18 bytes, and signature itself is 4 bytes;
    add some to allow for appended garbage.  Since ZipInfo is often used as
    a debugging tool, search the whole zipfile if zipinfo_mode is true.
  ---------------------------------------------------------------------------*/

    /* initialize the CRC table pointer (once) */
    if (CRC_32_TAB == NULL) {
        if ((CRC_32_TAB = get_crc_table()) == NULL)
            return PK_MEM;
    }

#if (!defined(SFX) || defined(SFX_EXDIR))
    /* check out if specified extraction root directory exists */
    if (uO.exdir != (char *)NULL && G.extract_flag) {
        G.create_dirs = !uO.fflag;
        if ((error = checkdir(__G__ uO.exdir, ROOT)) > 2)
            return error;   /* out of memory, or file in way */
    }
#endif /* !SFX || SFX_EXDIR */

    G.cur_zipfile_bufstart = 0;
    G.inptr = G.inbuf;

#if (!defined(WINDLL) && !defined(SFX))
#ifdef TIMESTAMP
    if (!uO.zipinfo_mode && !uO.qflag && !uO.T_flag)
#else
    if (!uO.zipinfo_mode && !uO.qflag)
#endif
#ifdef WIN32    /* Win32 console may require codepage conversion for G.zipfn */
        Info(slide, 0, ((char *)slide, "Archive:  %s\n", FnFilter1(G.zipfn)));
#else
        Info(slide, 0, ((char *)slide, "Archive:  %s\n", G.zipfn));
#endif
#endif /* !WINDLL && !SFX */

    if ((
#ifndef NO_ZIPINFO
         uO.zipinfo_mode &&
          ((error_in_archive = find_ecrec(__G__ G.ziplen)) != 0 ||
          (error_in_archive = zi_end_central(__G)) > PK_WARN))
        || (!uO.zipinfo_mode &&
#endif
          ((error_in_archive = find_ecrec(__G__ MIN(G.ziplen,66000L))) != 0 ||
          (error_in_archive = uz_end_central(__G)) > PK_WARN)))
    {
        CLOSE_INFILE();

#ifdef SFX
        ++lastchance;   /* avoid picky compiler warnings */
        return error_in_archive;
#else
        if (maybe_exe)
            Info(slide, 0x401, ((char *)slide, LoadFarString(MaybeExe),
            G.zipfn));
        if (lastchance)
            return error_in_archive;
        else {
            G.no_ecrec = TRUE;    /* assume we found wrong file:  e.g., */
            return PK_NOZIP;       /*  unzip instead of unzip.zip */
        }
#endif /* ?SFX */
    }

    if ((uO.zflag > 0) && !uO.zipinfo_mode) { /* unzip: zflag = comment ONLY */
        CLOSE_INFILE();
        return error_in_archive;
    }

/*---------------------------------------------------------------------------
    Test the end-of-central-directory info for incompatibilities (multi-disk
    archives) or inconsistencies (missing or extra bytes in zipfile).
  ---------------------------------------------------------------------------*/

#ifdef NO_MULTIPART
    error = !uO.zipinfo_mode && (G.ecrec.number_this_disk == 1) &&
            (G.ecrec.num_disk_start_cdir == 1);
#else
    error = !uO.zipinfo_mode && (G.ecrec.number_this_disk != 0);
#endif

#ifndef SFX
    if (uO.zipinfo_mode &&
        G.ecrec.number_this_disk != G.ecrec.num_disk_start_cdir)
    {
        if (G.ecrec.number_this_disk > G.ecrec.num_disk_start_cdir) {
            Info(slide, 0x401, ((char *)slide,
              LoadFarString(CentDirNotInZipMsg), G.zipfn,
              G.ecrec.number_this_disk, G.ecrec.num_disk_start_cdir));
            error_in_archive = PK_FIND;
            too_weird_to_continue = TRUE;
        } else {
            Info(slide, 0x401, ((char *)slide,
              LoadFarString(EndCentDirBogus), G.zipfn,
              G.ecrec.number_this_disk, G.ecrec.num_disk_start_cdir));
            error_in_archive = PK_WARN;
        }
#ifdef NO_MULTIPART   /* concatenation of multiple parts works in some cases */
    } else if (!uO.zipinfo_mode && !error && G.ecrec.number_this_disk != 0) {
        Info(slide, 0x401, ((char *)slide, LoadFarString(NoMultiDiskArcSupport),
          G.zipfn));
        error_in_archive = PK_FIND;
        too_weird_to_continue = TRUE;
#endif
    }

    if (!too_weird_to_continue) {  /* (relatively) normal zipfile:  go for it */
        if (error) {
            Info(slide, 0x401, ((char *)slide, LoadFarString(MaybePakBug),
              G.zipfn));
            error_in_archive = PK_WARN;
        }
#endif /* !SFX */
        if ((G.extra_bytes = G.real_ecrec_offset-G.expect_ecrec_offset) <
            (LONGINT)0)
        {
            Info(slide, 0x401, ((char *)slide, LoadFarString(MissingBytes),
              G.zipfn, (long)(-G.extra_bytes)));
            error_in_archive = PK_ERR;
        } else if (G.extra_bytes > 0) {
            if ((G.ecrec.offset_start_central_directory == 0) &&
                (G.ecrec.size_central_directory != 0))   /* zip 1.5 -go bug */
            {
                Info(slide, 0x401, ((char *)slide,
                  LoadFarString(NullCentDirOffset), G.zipfn));
                G.ecrec.offset_start_central_directory = G.extra_bytes;
                G.extra_bytes = 0;
                error_in_archive = PK_ERR;
            }
#ifndef SFX
            else {
                Info(slide, 0x401, ((char *)slide,
                  LoadFarString(ExtraBytesAtStart), G.zipfn,
                  (long)G.extra_bytes, (G.extra_bytes == 1)? "":"s"));
                error_in_archive = PK_WARN;
            }
#endif /* !SFX */
        }

    /*-----------------------------------------------------------------------
        Check for empty zipfile and exit now if so.
      -----------------------------------------------------------------------*/

        if (G.expect_ecrec_offset==0L && G.ecrec.size_central_directory==0) {
            if (uO.zipinfo_mode)
                Info(slide, 0, ((char *)slide, "%sEmpty zipfile.\n",
                  uO.lflag>9? "\n  " : ""));
            else
                Info(slide, 0x401, ((char *)slide, LoadFarString(ZipfileEmpty),
                                    G.zipfn));
            CLOSE_INFILE();
            return (error_in_archive > PK_WARN)? error_in_archive : PK_WARN;
        }

    /*-----------------------------------------------------------------------
        Compensate for missing or extra bytes, and seek to where the start
        of central directory should be.  If header not found, uncompensate
        and try again (necessary for at least some Atari archives created
        with STZip, as well as archives created by J.H. Holm's ZIPSPLIT 1.1).
      -----------------------------------------------------------------------*/

        ZLSEEK( G.ecrec.offset_start_central_directory )
#ifdef OLD_SEEK_TEST
        if (readbuf(G.sig, 4) == 0) {
            CLOSE_INFILE();
            return PK_ERR;  /* file may be locked, or possibly disk error(?) */
        }
        if (strncmp(G.sig, central_hdr_sig, 4))
#else
        if ((readbuf(__G__ G.sig, 4) == 0) ||
             strncmp(G.sig, central_hdr_sig, 4))
#endif
        {
#ifndef SFX
            long tmp = G.extra_bytes;
#endif

            G.extra_bytes = 0;
            ZLSEEK( G.ecrec.offset_start_central_directory )
            if ((readbuf(__G__ G.sig, 4) == 0) ||
                strncmp(G.sig, central_hdr_sig, 4))
            {
                Info(slide, 0x401, ((char *)slide,
                  LoadFarString(CentDirStartNotFound), G.zipfn,
                  LoadFarStringSmall(ReportMsg)));
                CLOSE_INFILE();
                return PK_BADERR;
            }
#ifndef SFX
            Info(slide, 0x401, ((char *)slide, LoadFarString(CentDirTooLong),
              G.zipfn, -tmp));
#endif
            error_in_archive = PK_ERR;
        }

    /*-----------------------------------------------------------------------
        Seek to the start of the central directory one last time, since we
        have just read the first entry's signature bytes; then list, extract
        or test member files as instructed, and close the zipfile.
      -----------------------------------------------------------------------*/

        Trace((stderr, "about to extract/list files (error = %d)\n",
          error_in_archive));

        ZLSEEK( G.ecrec.offset_start_central_directory )

#ifdef DLL
        /* G.fValidate is used only to look at an archive to see if
           it appears to be a valid archive.  There is no interest
           in what the archive contains, nor in validating that the
           entries in the archive are in good condition.  This is
           currently used only in the Windows DLLs for purposes of
           checking archives within an archive to determine whether
           or not to display the inner archives.
         */
        if (!G.fValidate)
#endif
        {
#ifndef NO_ZIPINFO
            if (uO.zipinfo_mode)
                error = zipinfo(__G);                     /* ZIPINFO 'EM */
            else
#endif
#ifndef SFX
#ifdef TIMESTAMP
            if (uO.T_flag)
                error = get_time_stamp(__G__ &uxstamp, &nmember);
            else
#endif
            if (uO.vflag && !uO.tflag && !uO.cflag)
                error = list_files(__G);              /* LIST 'EM */
            else
#endif /* !SFX */
                error = extract_or_test_files(__G);   /* EXTRACT OR TEST 'EM */

            Trace((stderr, "done with extract/list files (error = %d)\n",
                   error));
        }

        if (error > error_in_archive)   /* don't overwrite stronger error */
            error_in_archive = error;   /*  with (for example) a warning */
#ifndef SFX
    } /* end if (!too_weird_to_continue) */
#endif

    CLOSE_INFILE();

#ifdef TIMESTAMP
    if (uO.T_flag && !uO.zipinfo_mode && (nmember > 0)) {
# ifdef WIN32
        if (stamp_file(__G__ G.zipfn, uxstamp)) {       /* TIME-STAMP 'EM */
# else
        if (stamp_file(G.zipfn, uxstamp)) {             /* TIME-STAMP 'EM */
# endif
            Info(slide, 0x201, ((char *)slide,
              "warning:  cannot set time for %s\n", G.zipfn));
            if (error_in_archive < PK_WARN)
                error_in_archive = PK_WARN;
        }
    }
#endif
    return error_in_archive;

} /* end function do_seekable() */





/*************************/
/* Function find_ecrec() */
/*************************/

static int find_ecrec(__G__ searchlen)          /* return PK-class error */
    __GDEF
    long searchlen;
{
    int i, numblks, found=FALSE;
    LONGINT tail_len;
    ec_byte_rec byterec;


/*---------------------------------------------------------------------------
    Treat case of short zipfile separately.
  ---------------------------------------------------------------------------*/

    if (G.ziplen <= INBUFSIZ) {
        acelseek(0L, SEEK_SET);
        if ((G.incnt = aceread((char *)G.inbuf,(unsigned int)G.ziplen))
            == (int)G.ziplen)

            /* 'P' must be at least 22 bytes from end of zipfile */
            for (G.inptr = G.inbuf+(int)G.ziplen-22;  G.inptr >= G.inbuf;
                 --G.inptr)
                if ((native(*G.inptr) == 'P')  &&
                     !strncmp((char *)G.inptr, end_central_sig, 4)) {
                    G.incnt -= (int)(G.inptr - G.inbuf);
                    found = TRUE;
                    break;
                }

/*---------------------------------------------------------------------------
    Zipfile is longer than INBUFSIZ:  may need to loop.  Start with short
    block at end of zipfile (if not TOO short).
  ---------------------------------------------------------------------------*/

    } else {
        if ((tail_len = G.ziplen % INBUFSIZ) > ECREC_SIZE) {
#ifdef USE_STRM_INPUT
            fseek((FILE *)G.zipfd, G.ziplen-tail_len, SEEK_SET);
            G.cur_zipfile_bufstart = ftell((FILE *)G.zipfd);
#else /* !USE_STRM_INPUT */
            G.cur_zipfile_bufstart = acelseek(G.ziplen-tail_len,
              SEEK_SET);
#endif /* ?USE_STRM_INPUT */
            if ((G.incnt = aceread((char *)G.inbuf,
                (unsigned int)tail_len)) != (int)tail_len)
                goto fail;      /* it's expedient... */

            /* 'P' must be at least 22 bytes from end of zipfile */
            for (G.inptr = G.inbuf+(int)tail_len-22;  G.inptr >= G.inbuf;
                 --G.inptr)
                if ((native(*G.inptr) == 'P')  &&
                     !strncmp((char *)G.inptr, end_central_sig, 4)) {
                    G.incnt -= (int)(G.inptr - G.inbuf);
                    found = TRUE;
                    break;
                }
            /* sig may span block boundary: */
            strncpy((char *)G.hold, (char *)G.inbuf, 3);
        } else
            G.cur_zipfile_bufstart = G.ziplen - tail_len;

    /*-----------------------------------------------------------------------
        Loop through blocks of zipfile data, starting at the end and going
        toward the beginning.  In general, need not check whole zipfile for
        signature, but may want to do so if testing.
      -----------------------------------------------------------------------*/

        numblks = (int)((searchlen - tail_len + (INBUFSIZ-1)) / INBUFSIZ);
        /*               ==amount=   ==done==   ==rounding==    =blksiz=  */

        for (i = 1;  !found && (i <= numblks);  ++i) {
            G.cur_zipfile_bufstart -= INBUFSIZ;
            acelseek(G.cur_zipfile_bufstart, SEEK_SET);
            if ((G.incnt = aceread((char *)G.inbuf,INBUFSIZ))
                != INBUFSIZ)
                break;          /* fall through and fail */

            for (G.inptr = G.inbuf+INBUFSIZ-1;  G.inptr >= G.inbuf;
                 --G.inptr)
                if ((native(*G.inptr) == 'P')  &&
                     !strncmp((char *)G.inptr, end_central_sig, 4)) {
                    G.incnt -= (int)(G.inptr - G.inbuf);
                    found = TRUE;
                    break;
                }
            /* sig may span block boundary: */
            strncpy((char *)G.hold, (char *)G.inbuf, 3);
        }
    } /* end if (ziplen > INBUFSIZ) */

/*---------------------------------------------------------------------------
    Searched through whole region where signature should be without finding
    it.  Print informational message and die a horrible death.
  ---------------------------------------------------------------------------*/

fail:
    if (!found) {
        if (uO.qflag || uO.zipinfo_mode)
            Info(slide, 0x401, ((char *)slide, "[%s]\n", G.zipfn));
        Info(slide, 0x401, ((char *)slide,
          LoadFarString(CentDirEndSigNotFound)));
        return PK_ERR;   /* failed */
    }

/*---------------------------------------------------------------------------
    Found the signature, so get the end-central data before returning.  Do
    any necessary machine-type conversions (byte ordering, structure padding
    compensation) by reading data into character array and copying to struct.
  ---------------------------------------------------------------------------*/

    G.real_ecrec_offset = G.cur_zipfile_bufstart + (G.inptr-G.inbuf);
#ifdef TEST
    pipeit("\n  found end-of-central-dir signature at offset %ld (%.8lXh)\n",
      G.real_ecrec_offset, G.real_ecrec_offset);
    pipeit("    from beginning of file; offset %d (%.4Xh) within block\n",
      G.inptr-G.inbuf, G.inptr-G.inbuf);
#endif

    if (readbuf(__G__ (char *)byterec, ECREC_SIZE+4) == 0)
        return PK_EOF;

    G.ecrec.number_this_disk =
      makeword(&byterec[NUMBER_THIS_DISK]);
    G.ecrec.num_disk_start_cdir =
      makeword(&byterec[NUM_DISK_WITH_START_CENTRAL_DIR]);
    G.ecrec.num_entries_centrl_dir_ths_disk =
      makeword(&byterec[NUM_ENTRIES_CENTRL_DIR_THS_DISK]);
    G.ecrec.total_entries_central_dir =
      makeword(&byterec[TOTAL_ENTRIES_CENTRAL_DIR]);
    G.ecrec.size_central_directory =
      makelong(&byterec[SIZE_CENTRAL_DIRECTORY]);
    G.ecrec.offset_start_central_directory =
      makelong(&byterec[OFFSET_START_CENTRAL_DIRECTORY]);
    G.ecrec.zipfile_comment_length =
      makeword(&byterec[ZIPFILE_COMMENT_LENGTH]);

    G.expect_ecrec_offset = G.ecrec.offset_start_central_directory +
                          G.ecrec.size_central_directory;
    return PK_COOL;

} /* end function find_ecrec() */





/*****************************/
/* Function uz_end_central() */
/*****************************/

int uz_end_central(__G)    /* return PK-type error code */
    __GDEF
{
    int error = PK_COOL;


/*---------------------------------------------------------------------------
    Get the zipfile comment (up to 64KB long), if any, and print it out.
    Then position the file pointer to the beginning of the central directory
    and fill buffer.
  ---------------------------------------------------------------------------*/

#ifdef WINDLL
    /* for comment button: */
    if ((!G.fValidate) && (G.lpUserFunctions != NULL))
       G.lpUserFunctions->cchComment = G.ecrec.zipfile_comment_length;
    if (G.ecrec.zipfile_comment_length && (uO.zflag > 0))
#else /* !WINDLL */
    if (G.ecrec.zipfile_comment_length && (uO.zflag > 0 ||
        (uO.zflag == 0 &&
#ifdef TIMESTAMP
                          !uO.T_flag &&
#endif
                                        !uO.qflag)))
#endif /* ?WINDLL */
    {
        if (do_string(__G__ G.ecrec.zipfile_comment_length, DISPLAY)) {
            Info(slide, 0x401, ((char *)slide,
              LoadFarString(ZipfileCommTrunc1)));
            error = PK_WARN;
        }
    }
    return error;

} /* end function uz_end_central() */





/************************************/
/* Function process_cdir_file_hdr() */
/************************************/

int process_cdir_file_hdr(__G)    /* return PK-type error code */
    __GDEF
{
    int error;


/*---------------------------------------------------------------------------
    Get central directory info, save host and method numbers, and set flag
    for lowercase conversion of filename, depending on the OS from which the
    file is coming.
  ---------------------------------------------------------------------------*/

    if ((error = get_cdir_ent(__G)) != 0)
        return error;

    G.pInfo->hostnum = MIN(G.crec.version_made_by[1], NUM_HOSTS);
/*  extnum = MIN(crec.version_needed_to_extract[1], NUM_HOSTS); */

    G.pInfo->lcflag = 0;
    if (uO.L_flag)            /* user specified case-conversion */
        switch (G.pInfo->hostnum) {
            case FS_FAT_:     /* PKZIP and zip -k store in uppercase */
            case CPM_:        /* like MS-DOS, right? */
            case VM_CMS_:     /* all caps? */
            case MVS_:        /* all caps? */
            case TANDEM_:
            case TOPS20_:
            case VMS_:        /* our Zip uses lowercase, but ASi's doesn't */
        /*  case Z_SYSTEM_:   ? */
        /*  case QDOS_:       ? */
                G.pInfo->lcflag = 1;   /* convert filename to lowercase */
                break;

            default:     /* AMIGA_, FS_HPFS_, FS_NTFS_, MAC_, UNIX_, ATARI_, */
                break;   /*  FS_VFAT_, BEOS_ (Z_SYSTEM_):  no conversion */
        }

    /* do Amigas (AMIGA_) also have volume labels? */
    if (IS_VOLID(G.crec.external_file_attributes) &&
        (G.pInfo->hostnum == FS_FAT_ || G.pInfo->hostnum == FS_HPFS_ ||
         G.pInfo->hostnum == FS_NTFS_ || G.pInfo->hostnum == ATARI_))
    {
        G.pInfo->vollabel = TRUE;
        G.pInfo->lcflag = 0;        /* preserve case of volume labels */
    } else
        G.pInfo->vollabel = FALSE;

    return PK_COOL;

} /* end function process_cdir_file_hdr() */





/***************************/
/* Function get_cdir_ent() */
/***************************/

int get_cdir_ent(__G)   /* return PK-type error code */
    __GDEF
{
    cdir_byte_hdr byterec;


/*---------------------------------------------------------------------------
    Read the next central directory entry and do any necessary machine-type
    conversions (byte ordering, structure padding compensation--do so by
    copying the data from the array into which it was read (byterec) to the
    usable struct (crec)).
  ---------------------------------------------------------------------------*/

    if (readbuf(__G__ (char *)byterec, CREC_SIZE) == 0)
        return PK_EOF;

    G.crec.version_made_by[0] = byterec[C_VERSION_MADE_BY_0];
    G.crec.version_made_by[1] = byterec[C_VERSION_MADE_BY_1];
    G.crec.version_needed_to_extract[0] =
      byterec[C_VERSION_NEEDED_TO_EXTRACT_0];
    G.crec.version_needed_to_extract[1] =
      byterec[C_VERSION_NEEDED_TO_EXTRACT_1];

    G.crec.general_purpose_bit_flag =
      makeword(&byterec[C_GENERAL_PURPOSE_BIT_FLAG]);
    G.crec.compression_method =
      makeword(&byterec[C_COMPRESSION_METHOD]);
    G.crec.last_mod_dos_datetime =
      makelong(&byterec[C_LAST_MOD_DOS_DATETIME]);
    G.crec.crc32 =
      makelong(&byterec[C_CRC32]);
    G.crec.csize =
      makelong(&byterec[C_COMPRESSED_SIZE]);
    G.crec.ucsize =
      makelong(&byterec[C_UNCOMPRESSED_SIZE]);
    G.crec.filename_length =
      makeword(&byterec[C_FILENAME_LENGTH]);
    G.crec.extra_field_length =
      makeword(&byterec[C_EXTRA_FIELD_LENGTH]);
    G.crec.file_comment_length =
      makeword(&byterec[C_FILE_COMMENT_LENGTH]);
    G.crec.disk_number_start =
      makeword(&byterec[C_DISK_NUMBER_START]);
    G.crec.internal_file_attributes =
      makeword(&byterec[C_INTERNAL_FILE_ATTRIBUTES]);
    G.crec.external_file_attributes =
      makelong(&byterec[C_EXTERNAL_FILE_ATTRIBUTES]);  /* LONG, not word! */
    G.crec.relative_offset_local_header =
      makelong(&byterec[C_RELATIVE_OFFSET_LOCAL_HEADER]);

    return PK_COOL;

} /* end function get_cdir_ent() */





/*************************************/
/* Function process_local_file_hdr() */
/*************************************/

int process_local_file_hdr(__G)    /* return PK-type error code */
    __GDEF
{
    local_byte_hdr byterec;


/*---------------------------------------------------------------------------
    Read the next local file header and do any necessary machine-type con-
    versions (byte ordering, structure padding compensation--do so by copy-
    ing the data from the array into which it was read (byterec) to the
    usable struct (lrec)).
  ---------------------------------------------------------------------------*/

    if (readbuf(__G__ (char *)byterec, LREC_SIZE) == 0)
        return PK_EOF;

    G.lrec.version_needed_to_extract[0] =
      byterec[L_VERSION_NEEDED_TO_EXTRACT_0];
    G.lrec.version_needed_to_extract[1] =
      byterec[L_VERSION_NEEDED_TO_EXTRACT_1];

    G.lrec.general_purpose_bit_flag =
      makeword(&byterec[L_GENERAL_PURPOSE_BIT_FLAG]);
    G.lrec.compression_method = makeword(&byterec[L_COMPRESSION_METHOD]);
    G.lrec.last_mod_dos_datetime = makelong(&byterec[L_LAST_MOD_DOS_DATETIME]);
    G.lrec.crc32 = makelong(&byterec[L_CRC32]);
    G.lrec.csize = makelong(&byterec[L_COMPRESSED_SIZE]);
    G.lrec.ucsize = makelong(&byterec[L_UNCOMPRESSED_SIZE]);
    G.lrec.filename_length = makeword(&byterec[L_FILENAME_LENGTH]);
    G.lrec.extra_field_length = makeword(&byterec[L_EXTRA_FIELD_LENGTH]);

    G.csize = (long) G.lrec.csize;
    G.ucsize = (long) G.lrec.ucsize;

    if ((G.lrec.general_purpose_bit_flag & 8) != 0) {
        /* can't trust local header, use central directory: */
        G.lrec.crc32 = G.pInfo->crc;
        G.csize = (long)(G.lrec.csize = G.pInfo->compr_size);
        G.ucsize = (long)(G.lrec.ucsize = G.pInfo->uncompr_size);
    }

    return PK_COOL;

} /* end function process_local_file_hdr() */


#ifdef USE_EF_UT_TIME

/*******************************/
/* Function ef_scan_for_izux() */
/*******************************/

unsigned ef_scan_for_izux(ef_buf, ef_len, ef_is_c, dos_mdatetime,
                          z_utim, z_uidgid)
    uch *ef_buf;        /* buffer containing extra field */
    unsigned ef_len;    /* total length of extra field */
    int ef_is_c;        /* flag indicating "is central extra field" */
    ulg dos_mdatetime;  /* last_mod_file_date_time in DOS format */
    iztimes *z_utim;    /* return storage: atime, mtime, ctime */
    ush *z_uidgid;      /* return storage: uid and gid */
{
    unsigned flags = 0;
    unsigned eb_id;
    unsigned eb_len;
    int have_new_type_eb = FALSE;
    int ut_zip_unzip_compatible = FALSE;

/*---------------------------------------------------------------------------
    This function scans the extra field for EF_TIME, EF_IZUNIX2, EF_IZUNIX, or
    EF_PKUNIX blocks containing Unix-style time_t (GMT) values for the entry's
    access, creation, and modification time.
    If a valid block is found, the time stamps are copied to the iztimes
    structure (provided the z_utim pointer is not NULL).
    If a IZUNIX2 block is found or the IZUNIX block contains UID/GID fields,
    and the z_uidgid array pointer is valid (!= NULL), the owner info is
    transfered as well.
    The presence of an EF_TIME or EF_IZUNIX2 block results in ignoring all
    data from probably present obsolete EF_IZUNIX blocks.
    If multiple blocks of the same type are found, only the information from
    the last block is used.
    The return value is a combination of the EF_TIME Flags field with an
    additional flag bit indicating the presence of valid UID/GID info,
    or 0 in case of failure.
  ---------------------------------------------------------------------------*/

    if (ef_len == 0 || ef_buf == NULL || (z_utim == 0 && z_uidgid == NULL))
        return 0;

    TTrace((stderr,"\nef_scan_for_izux: scanning extra field of length %u\n",
      ef_len));

    while (ef_len >= EB_HEADSIZE) {
        eb_id = makeword(EB_ID + ef_buf);
        eb_len = makeword(EB_LEN + ef_buf);

        if (eb_len > (ef_len - EB_HEADSIZE)) {
            /* discovered some extra field inconsistency! */
            TTrace((stderr,
              "ef_scan_for_izux: block length %u > rest ef_size %u\n", eb_len,
              ef_len - EB_HEADSIZE));
            break;
        }

        switch (eb_id) {
          case EF_TIME:
            flags &= ~0x0ff;    /* ignore previous IZUNIX or EF_TIME fields */
            have_new_type_eb = TRUE;
            if ( eb_len >= EB_UT_MINLEN && z_utim != NULL) {
                unsigned eb_idx = EB_UT_TIME1;
                TTrace((stderr,"ef_scan_for_izux: found TIME extra field\n"));
                flags |= (ef_buf[EB_HEADSIZE+EB_UT_FLAGS] & 0x0ff);
                if ((flags & EB_UT_FL_MTIME)) {
                    if ((eb_idx+4) <= eb_len) {
                        z_utim->mtime = makelong((EB_HEADSIZE+eb_idx) + ef_buf);
                        eb_idx += 4;
                        TTrace((stderr,"  UT e.f. modification time = %ld\n",
                                z_utim->mtime));

                        if ((ulg)(z_utim->mtime) & (ulg)(0x80000000L)) {
                            ut_zip_unzip_compatible =
                              ((time_t)0x80000000L < (time_t)0L)
                              ? (dos_mdatetime == DOSTIME_MINIMUM)
                              : (dos_mdatetime >= DOSTIME_2038_01_18);
                            if (!ut_zip_unzip_compatible) {
                            /* UnZip interpretes mtime differently than Zip;
                               without modtime: ignore complete UT field */
                              flags &= ~0x0ff;  /* no time_t times available */
                              TTrace((stderr,
                                "  UT modtime range error; ignore e.f.!\n"));
                              break;            /* stop scanning this field */
                            }
                        } else {
                            /* cannot determine, safe assumption is FALSE */
                            ut_zip_unzip_compatible = FALSE;
                        }
                    } else {
                        flags &= ~EB_UT_FL_MTIME;
                        TTrace((stderr,"  UT e.f. truncated; no modtime\n"));
                    }
                }
                if (ef_is_c) {
                    break;      /* central version of TIME field ends here */
                }

                if (flags & EB_UT_FL_ATIME) {
                    if ((eb_idx+4) <= eb_len) {
                        z_utim->atime = makelong((EB_HEADSIZE+eb_idx) + ef_buf);
                        eb_idx += 4;
                        TTrace((stderr,"  UT e.f. access time = %ld\n",
                                z_utim->atime));
                        if (((ulg)(z_utim->atime) & (ulg)(0x80000000L)) &&
                            !ut_zip_unzip_compatible) {
                            flags &= ~EB_UT_FL_ATIME;
                            TTrace((stderr,
                              "  UT access time range error: skip time!\n"));
                        }
                    } else {
                        flags &= ~EB_UT_FL_ATIME;
                    }
                }
                if (flags & EB_UT_FL_CTIME) {
                    if ((eb_idx+4) <= eb_len) {
                        z_utim->ctime = makelong((EB_HEADSIZE+eb_idx) + ef_buf);
                        TTrace((stderr,"  UT e.f. creation time = %ld\n",
                                z_utim->ctime));
                        if (((ulg)(z_utim->ctime) & (ulg)(0x80000000L)) &&
                            !ut_zip_unzip_compatible) {
                            flags &= ~EB_UT_FL_CTIME;
                            TTrace((stderr,
                              "  UT creation time range error: skip time!\n"));
                        }
                    } else {
                        flags &= ~EB_UT_FL_CTIME;
                    }
                }
            }
            break;

          case EF_IZUNIX2:
            if (!have_new_type_eb) {
                flags &= ~0x0ff;        /* ignore any previous IZUNIX field */
                have_new_type_eb = TRUE;
            }
            if (eb_len >= EB_UX2_MINLEN && z_uidgid != NULL) {
                z_uidgid[0] = makeword((EB_HEADSIZE+EB_UX2_UID) + ef_buf);
                z_uidgid[1] = makeword((EB_HEADSIZE+EB_UX2_GID) + ef_buf);
                flags |= EB_UX2_VALID;   /* signal success */
            }
            break;

          case EF_IZUNIX:
          case EF_PKUNIX:       /* PKUNIX e.f. layout is identical to IZUNIX */
            if (eb_len >= EB_UX_MINLEN) {
                TTrace((stderr,"ef_scan_for_izux: found %s extra field\n",
                        (eb_id == EF_IZUNIX ? "IZUNIX" : "PKUNIX")));
                if (have_new_type_eb) {
                    break;      /* Ignore IZUNIX extra field block ! */
                }
                if (z_utim != NULL) {
                    z_utim->atime = makelong((EB_HEADSIZE+EB_UX_ATIME)+ef_buf);
                    z_utim->mtime = makelong((EB_HEADSIZE+EB_UX_MTIME)+ef_buf);
                    TTrace((stderr,"  Unix EF actime = %ld\n", z_utim->atime));
                    TTrace((stderr,"  Unix EF modtime = %ld\n", z_utim->mtime));
                    flags |= (EB_UT_FL_MTIME | EB_UT_FL_ATIME);
                    if ((ulg)(z_utim->mtime) & (ulg)(0x80000000L)) {
                        ut_zip_unzip_compatible =
                          ((time_t)0x80000000L < (time_t)0L)
                          ? (dos_mdatetime == DOSTIME_MINIMUM)
                          : (dos_mdatetime >= DOSTIME_2038_01_18);
                        if (!ut_zip_unzip_compatible) {
                            /* UnZip interpretes mtime differently than Zip;
                               without modtime: ignore complete UT field */
                            flags &= ~0x0ff;    /* no time_t times available */
                            TTrace((stderr,
                                  "  UX modtime range error: ignore e.f.!\n"));
                        }
                    } else {
                        /* cannot determine, safe assumption is FALSE */
                        ut_zip_unzip_compatible = FALSE;
                    }
                    if ((ulg)(z_utim->atime) & (ulg)(0x80000000L) &&
                        !ut_zip_unzip_compatible && (flags & 0x0ff)) {
                        /* atime not in range of UnZip's time_t */
                        flags &= ~EB_UT_FL_ATIME;
                        TTrace((stderr,
                                "  UX access time range error: skip time!\n"));
                    }
                }
                if (eb_len >= EB_UX_FULLSIZE && z_uidgid != NULL) {
                    z_uidgid[0] = makeword((EB_HEADSIZE+EB_UX_UID) + ef_buf);
                    z_uidgid[1] = makeword((EB_HEADSIZE+EB_UX_GID) + ef_buf);
                    flags |= EB_UX2_VALID;
                }
            }
            break;

          default:
            break;
        }

        /* Skip this extra field block */
        ef_buf += (eb_len + EB_HEADSIZE);
        ef_len -= (eb_len + EB_HEADSIZE);
    }

    return flags;
}

#endif /* USE_EF_UT_TIME */
