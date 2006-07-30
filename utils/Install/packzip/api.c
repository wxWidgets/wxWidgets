/*---------------------------------------------------------------------------

  api.c

  This module supplies an UnZip engine for use directly from C/C++
  programs.  The functions are:

    UzpVer *UzpVersion(void);
    void UzpVersion2(UzpVer2 *version)
    int UzpMain(int argc, char *argv[]);
    int UzpAltMain(int argc, char *argv[], UzpInit *init);
    int UzpValidate(char *archive, int AllCodes);
    void UzpFreeMemBuffer(UzpBuffer *retstr);
    int UzpUnzipToMemory(char *zip, char *file, UzpOpts *optflgs,
                         UzpCB *UsrFuncts, UzpBuffer *retstr);

  non-WINDLL only (a special WINDLL variant is defined in windll/windll.c):
    int UzpGrep(char *archive, char *file, char *pattern, int cmd, int SkipBin,
                UzpCB *UsrFuncts);

  OS/2 only (for now):
    int UzpFileTree(char *name, cbList(callBack), char *cpInclude[],
          char *cpExclude[]);

  You must define `DLL' in order to include the API extensions.

  ---------------------------------------------------------------------------*/


#ifdef OS2
#  define  INCL_DOSMEMMGR
#  include <os2.h>
#endif
#include <setjmp.h>

#define UNZIP_INTERNAL
#include "unzip.h"
#include "version.h"
#ifdef WINDLL
#  include "windll/windll.h"
#endif

#ifdef DLL      /* This source file supplies DLL-only interface code. */

jmp_buf dll_error_return;

/*---------------------------------------------------------------------------
    Documented API entry points
  ---------------------------------------------------------------------------*/


UzpVer * UZ_EXP UzpVersion()   /* should be pointer to const struct */
{
    static UzpVer version;     /* doesn't change between calls */


    version.structlen = UZPVER_LEN;

#ifdef BETA
    version.flag = 1;
#else
    version.flag = 0;
#endif
    version.betalevel = BETALEVEL;
    version.date = VERSION_DATE;

#ifdef ZLIB_VERSION
    version.zlib_version = ZLIB_VERSION;
    version.flag |= 2;
#else
    version.zlib_version = NULL;
#endif

    /* someday each of these may have a separate patchlevel: */
    version.unzip.major = UZ_MAJORVER;
    version.unzip.minor = UZ_MINORVER;
    version.unzip.patchlevel = PATCHLEVEL;

    version.zipinfo.major = ZI_MAJORVER;
    version.zipinfo.minor = ZI_MINORVER;
    version.zipinfo.patchlevel = PATCHLEVEL;

    /* these are retained for backward compatibility only: */
    version.os2dll.major = UZ_MAJORVER;
    version.os2dll.minor = UZ_MINORVER;
    version.os2dll.patchlevel = PATCHLEVEL;

    version.windll.major = UZ_MAJORVER;
    version.windll.minor = UZ_MINORVER;
    version.windll.patchlevel = PATCHLEVEL;

    return &version;
}

void UZ_EXP UzpVersion2(UzpVer2 *version)
{

    version->structlen = UZPVER_LEN;

#ifdef BETA
    version->flag = 1;
#else
    version->flag = 0;
#endif
    strcpy(version->betalevel, BETALEVEL);
    strcpy(version->date, VERSION_DATE);

#ifdef ZLIB_VERSION
    strcpy(version->zlib_version, ZLIB_VERSION);
    version->flag |= 2;
#else
    version->zlib_version[0] = '\0';
#endif

    /* someday each of these may have a separate patchlevel: */
    version->unzip.major = UZ_MAJORVER;
    version->unzip.minor = UZ_MINORVER;
    version->unzip.patchlevel = PATCHLEVEL;

    version->zipinfo.major = ZI_MAJORVER;
    version->zipinfo.minor = ZI_MINORVER;
    version->zipinfo.patchlevel = PATCHLEVEL;

    /* these are retained for backward compatibility only: */
    version->os2dll.major = UZ_MAJORVER;
    version->os2dll.minor = UZ_MINORVER;
    version->os2dll.patchlevel = PATCHLEVEL;

    version->windll.major = UZ_MAJORVER;
    version->windll.minor = UZ_MINORVER;
    version->windll.patchlevel = PATCHLEVEL;
}





#ifndef WINDLL

int UZ_EXP UzpAltMain(int argc, char *argv[], UzpInit *init)
{
    int r, (*dummyfn)();


    CONSTRUCTGLOBALS();

    if (init->structlen >= (sizeof(ulg) + sizeof(dummyfn)) && init->msgfn)
        G.message = init->msgfn;

    if (init->structlen >= (sizeof(ulg) + 2*sizeof(dummyfn)) && init->inputfn)
        G.input = init->inputfn;

    if (init->structlen >= (sizeof(ulg) + 3*sizeof(dummyfn)) && init->pausefn)
        G.mpause = init->pausefn;

    if (init->structlen >= (sizeof(ulg) + 4*sizeof(dummyfn)) && init->userfn)
        (*init->userfn)();    /* allow void* arg? */

    r = unzip(__G__ argc, argv);
    DESTROYGLOBALS()
    RETURN(r);
}

#endif /* !WINDLL */




#ifndef __16BIT__

void UZ_EXP UzpFreeMemBuffer(UzpBuffer *retstr)
{
    if (retstr->strptr != NULL) {
        free(retstr->strptr);
        retstr->strptr = NULL;
    }
}




#ifndef WINDLL

static int UzpDLL_Init OF((zvoid *pG, UzpCB *UsrFuncts));

static int UzpDLL_Init(pG, UsrFuncts)
zvoid *pG;
UzpCB *UsrFuncts;
{
    int (*dummyfn)();

    if (UsrFuncts->structlen >= (sizeof(ulg) + sizeof(dummyfn)) &&
        UsrFuncts->msgfn)
        ((Uz_Globs *)pG)->message = UsrFuncts->msgfn;
    else
        return FALSE;

    if (UsrFuncts->structlen >= (sizeof(ulg) + 2*sizeof(dummyfn)) &&
        UsrFuncts->inputfn)
        ((Uz_Globs *)pG)->input = UsrFuncts->inputfn;

    if (UsrFuncts->structlen >= (sizeof(ulg) + 3*sizeof(dummyfn)) &&
        UsrFuncts->pausefn)
        ((Uz_Globs *)pG)->mpause = UsrFuncts->pausefn;

    if (UsrFuncts->structlen >= (sizeof(ulg) + 4*sizeof(dummyfn)) &&
        UsrFuncts->passwdfn)
        ((Uz_Globs *)pG)->decr_passwd = UsrFuncts->passwdfn;

    if (UsrFuncts->structlen >= (sizeof(ulg) + 5*sizeof(dummyfn)) &&
        UsrFuncts->statrepfn)
        ((Uz_Globs *)pG)->statreportcb = UsrFuncts->statrepfn;

    return TRUE;
}


int UZ_EXP UzpUnzipToMemory(char *zip, char *file, UzpOpts *optflgs,
    UzpCB *UsrFuncts, UzpBuffer *retstr)
{
    int r;
#if (defined(WINDLL) && !defined(CRTL_CP_IS_ISO))
    char *intern_zip, *intern_file;
#endif

    CONSTRUCTGLOBALS();
#if (defined(WINDLL) && !defined(CRTL_CP_IS_ISO))
    intern_zip = (char *)malloc(strlen(zip)+1);
    if (intern_zip == NULL) {
       DESTROYGLOBALS()
       return PK_MEM;
    }
    intern_file = (char *)malloc(strlen(file)+1);
    if (intern_file == NULL) {
       DESTROYGLOBALS()
       free(intern_zip);
       return PK_MEM;
    }
    ISO_TO_INTERN(zip, intern_zip);
    ISO_TO_INTERN(file, intern_file);
#   define zip intern_zip
#   define file intern_file
#endif
    /* Copy those options that are meaningful for UzpUnzipToMemory, instead of
     * a simple "memcpy(G.UzO, optflgs, sizeof(UzpOpts));"
     */
    uO.pwdarg = optflgs->pwdarg;
    uO.aflag = optflgs->aflag;
    uO.C_flag = optflgs->C_flag;
    uO.qflag = optflgs->qflag;  /* currently,  overridden in unzipToMemory */

    if (!UzpDLL_Init((zvoid *)&G, UsrFuncts)) {
       DESTROYGLOBALS();
       return PK_BADERR;
    }
    G.redirect_data = 1;

    r = (unzipToMemory(__G__ zip, file, retstr) <= PK_WARN);

    DESTROYGLOBALS()
#if (defined(WINDLL) && !defined(CRTL_CP_IS_ISO))
#  undef file
#  undef zip
    free(intern_file);
    free(intern_zip);
#endif
    if (!r && retstr->strlength) {
       free(retstr->strptr);
       retstr->strptr = NULL;
    }
    return r;
}
#endif /* !WINDLL */
#endif /* !__16BIT__ */





#ifdef OS2DLL

int UZ_EXP UzpFileTree(char *name, cbList(callBack), char *cpInclude[],
                char *cpExclude[])
{
    int r;

    CONSTRUCTGLOBALS();
    uO.qflag = 2;
    uO.vflag = 1;
    uO.C_flag = 1;
    G.wildzipfn = name;
    G.process_all_files = TRUE;
    if (cpInclude) {
        char **ptr = cpInclude;

        while (*ptr != NULL) ptr++;
        G.filespecs = ptr - cpInclude;
        G.pfnames = cpInclude, G.process_all_files = FALSE;
    }
    if (cpExclude) {
        char **ptr = cpExclude;

        while (*ptr != NULL) ptr++;
        G.xfilespecs = ptr - cpExclude;
        G.pxnames = cpExclude, G.process_all_files = FALSE;
    }

    G.processExternally = callBack;
    r = process_zipfiles(__G)==0;
    DESTROYGLOBALS()
    return r;
}

#endif /* OS2DLL */




/*---------------------------------------------------------------------------
    Helper functions
  ---------------------------------------------------------------------------*/


void setFileNotFound(__G)
    __GDEF
{
    G.filenotfound++;
}



int unzipToMemory(__GPRO__ char *zip, char *file, UzpBuffer *retstr)
{
    int r;
    char *incname[2];

    G.process_all_files = FALSE;
    G.extract_flag = TRUE;
    uO.qflag = 2;
    G.wildzipfn = zip;

    G.pfnames = incname;
    incname[0] = file;
    incname[1] = NULL;
    G.filespecs = 1;

    r = process_zipfiles(__G);
    if (retstr) {
        retstr->strptr = (char *)G.redirect_buffer;
        retstr->strlength = G.redirect_size;
    }
    return r;                   /* returns `PK_???' error values */
}



int redirect_outfile(__G)
     __GDEF
{
    if (G.redirect_size != 0 || G.redirect_buffer != NULL)
        return FALSE;

#ifndef NO_SLIDE_REDIR
    G.redirect_slide = !G.pInfo->textmode;
#endif
    G.redirect_size = (G.pInfo->textmode ?
                       G.lrec.ucsize * lenEOL : G.lrec.ucsize);
#ifdef OS2
    DosAllocMem((void **)&G.redirect_buffer, G.redirect_size+1,
      PAG_READ|PAG_WRITE|PAG_COMMIT);
    G.redirect_pointer = G.redirect_buffer;
#else
#ifdef __16BIT__
    if ((ulg)((extent)G.redirect_size) != G.redirect_size)
        return FALSE;
#endif
    G.redirect_pointer = G.redirect_buffer = malloc(G.redirect_size+1);
#endif
    if (!G.redirect_buffer)
        return FALSE;
    G.redirect_pointer[G.redirect_size] = '\0';
    return TRUE;
}



int writeToMemory(__GPRO__ uch *rawbuf, ulg size)
{
    if (rawbuf != G.redirect_pointer)
        memcpy(G.redirect_pointer,rawbuf,size);
    G.redirect_pointer += size;
    return 0;
}




int close_redirect(__G)
     __GDEF
{
    if (G.pInfo->textmode) {
        *G.redirect_pointer = '\0';
        G.redirect_size = G.redirect_pointer - G.redirect_buffer;
        if ((G.redirect_buffer =
             realloc(G.redirect_buffer, G.redirect_size + 1)) == NULL) {
            G.redirect_size = 0;
            return EOF;
        }
    }
    return 0;
}




#ifndef __16BIT__
#ifndef WINDLL

/* Purpose: Determine if file in archive contains the string szSearch

   Parameters: archive  = archive name
               file     = file contained in the archive. This cannot be
                          a wild card to be meaningful
               pattern  = string to search for
               cmd      = 0 - case-insensitive search
                          1 - case-sensitve search
                          2 - case-insensitive, whole words only
                          3 - case-sensitive, whole words only
               SkipBin  = if true, skip any files that have control
                          characters other than CR, LF, or tab in the first
                          100 characters.

   Returns:    TRUE if a match is found
               FALSE if no match is found
               -1 on error

   Comments: This does not pretend to be as useful as the standard
             Unix grep, which returns the strings associated with a
             particular pattern, nor does it search past the first
             matching occurrence of the pattern.
 */

int UZ_EXP UzpGrep(char *archive, char *file, char *pattern, int cmd,
                   int SkipBin, UzpCB *UsrFuncts)
{
    int retcode = FALSE, compare;
    ulg i, j, patternLen, buflen;
    char * sz, *p;
    UzpOpts flgopts;
    UzpBuffer retstr;

    memzero(&flgopts, sizeof(UzpOpts));         /* no special options */

    if (!UzpUnzipToMemory(archive, file, &flgopts, UsrFuncts, &retstr)) {
       return -1;   /* not enough memory, file not found, or other error */
    }

    if (SkipBin) {
        if (retstr.strlength < 100)
            buflen = retstr.strlength;
        else
            buflen = 100;
        for (i = 0; i < buflen; i++) {
            if (iscntrl(retstr.strptr[i])) {
                if ((retstr.strptr[i] != 0x0A) &&
                    (retstr.strptr[i] != 0x0D) &&
                    (retstr.strptr[i] != 0x09))
                {
                    /* OK, we now think we have a binary file of some sort */
                    free(retstr.strptr);
                    return FALSE;
                }
            }
        }
    }

    patternLen = strlen(pattern);

    if (retstr.strlength < patternLen) {
        free(retstr.strptr);
        return FALSE;
    }

    sz = malloc(patternLen + 3); /* add two in case doing whole words only */
    if (cmd > 1) {
        strcpy(sz, " ");
        strcat(sz, pattern);
        strcat(sz, " ");
    } else
        strcpy(sz, pattern);

    if ((cmd == 0) || (cmd == 2)) {
        for (i = 0; i < strlen(sz); i++)
            sz[i] = toupper(sz[i]);
        for (i = 0; i < retstr.strlength; i++)
            retstr.strptr[i] = toupper(retstr.strptr[i]);
    }

    for (i = 0; i < (retstr.strlength - patternLen); i++) {
        p = &retstr.strptr[i];
        compare = TRUE;
        for (j = 0; j < patternLen; j++) {
            /* We cannot do strncmp here, as we may be dealing with a
             * "binary" file, such as a word processing file, or perhaps
             * even a true executable of some sort. */
            if (p[j] != sz[j]) {
                compare = FALSE;
                break;
            }
        }
        if (compare == TRUE) {
            retcode = TRUE;
            break;
        }
    }

    free(sz);
    free(retstr.strptr);

    return retcode;
}
#endif /* !WINDLL */
#endif /* !__16BIT__ */




int UZ_EXP UzpValidate(char *archive, int AllCodes)
{
    int retcode;
    CONSTRUCTGLOBALS();

    uO.jflag = 1;
    uO.tflag = 1;
    uO.overwrite_none = 0;
    G.extract_flag = (!uO.zipinfo_mode &&
                      !uO.cflag && !uO.tflag && !uO.vflag && !uO.zflag
#ifdef TIMESTAMP
                      && !uO.T_flag
#endif
                     );

    uO.qflag = 2;                        /* turn off all messages */
    G.fValidate = TRUE;
    G.pfnames = (char **)&fnames[0];    /* assign default filename vector */
#ifdef WINDLL
    Wiz_NoPrinting(TRUE);
#endif

    if (archive == NULL) {      /* something is screwed up:  no filename */
        DESTROYGLOBALS();
        return PK_NOZIP;
    }

    G.wildzipfn = (char *)malloc(FILNAMSIZ + 1);
    strcpy(G.wildzipfn, archive);
#if (defined(WINDLL) && !defined(CRTL_CP_IS_ISO))
    _ISO_INTERN(G.wildzipfn);
#endif

    G.process_all_files = TRUE;         /* for speed */

    retcode = setjmp(dll_error_return);

    if (retcode) {
#ifdef WINDLL
        Wiz_NoPrinting(FALSE);
#endif
        free(G.wildzipfn);
        DESTROYGLOBALS();
        return PK_BADERR;
    }

    retcode = process_zipfiles(__G);

    free(G.wildzipfn);
#ifdef WINDLL
    Wiz_NoPrinting(FALSE);
#endif
    DESTROYGLOBALS();

    /* PK_WARN == 1 and PK_FIND == 11. When we are just looking at an
       archive, we should still be able to see the files inside it,
       even if we can't decode them for some reason.

       We also still want to be able to get at files even if there is
       something odd about the zip archive, hence allow PK_WARN,
       PK_FIND, IZ_UNSUP as well as PK_ERR
     */

    if (AllCodes)
        return retcode;

    if ((retcode == PK_OK) || (retcode == PK_WARN) || (retcode == PK_ERR) ||
        (retcode == IZ_UNSUP) || (retcode == PK_FIND))
        return TRUE;
    else
        return FALSE;
}

#endif /* DLL */
