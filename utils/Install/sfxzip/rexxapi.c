/**********************************************************************
*   REXXAPI.C                                                         *
*                                                                     *
*   This program adds a ZIP engine directly to the REXX language.     *
*   The functions are:                                                *
*       UZDropFuncs         -- Makes all functions in this package    *
*                              unknown to REXX.                       *
*       UZLoadFuncs         -- Makes all functions in this package    *
*                              known to REXX so REXX programs may     *
*                              call them.                             *
*       UZFileTree          -- Searches for files matching a given    *
*                              filespec, including files in           *
*                              subdirectories.                        *
*       UZUnZip             -- Unzip command-line entry point.        *
*                              This is functionally equivalent to     *
*                              using Unzip as an external program.    *
*       UZUnZipToVar            -- Unzip one file to a variable       *
*       UZUnZipToStem       -- Unzip files to a variable array        *
*       UZVer               -- Returns the Unzip version number       *
*                                                                     *
**********************************************************************/
/* Include files */

#ifdef OS2DLL

#define  INCL_DOS
#define  INCL_DOSMEMMGR
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#define UNZIP_INTERNAL
#include "../unzip.h"
#include "../version.h"


/*********************************************************************/
/*  Various definitions used by various functions.                   */
/*********************************************************************/

RexxFunctionHandler UZDropFuncs;
RexxFunctionHandler UZLoadFuncs;
RexxFunctionHandler UZFileTree;
RexxFunctionHandler UZUnZip;
RexxFunctionHandler UZUnZipToVar;
RexxFunctionHandler UZUnZipToStem;
RexxFunctionHandler UZVer;
RexxFunctionHandler UZAPIVer;


int SetOutputVar(__GPRO__ const char *name);
int SetOutputVarStem(__GPRO__ const char *name);
int SetOutputVarLength(__GPRO);
int WriteToVariable(__GPRO__ const char *name, char *buffer, int len);
int PrintToSubVariable(__GPRO__ int idx, const char *format,...);
int PrintToVariable(__GPRO__ const char *name, const char *format,...);
int _PrintToVariable(__GPRO__ const char *name, const char *format, va_list arg_ptr);
int TextSetNext(__GPRO__ char *format, int len, int all);

#define EZRXSTRING(r,p)  {(r).strptr=(PCH)p;(r).strlength=(ULONG)strlen((r).strptr);}


/*********************************************************************/
/* RxFncTable                                                        */
/*   Array of names of the UNZIPAPI functions.                       */
/*   This list is used for registration and deregistration.          */
/*********************************************************************/

static PSZ  RxFncTable[] =
   {
      "UZDropFuncs",
      "UZLoadFuncs",
      "UZFileSearch",
      "UZFileTree",
      "UZUnZip",
      "UZUnZipToVar",
      "UZUnZipToStem",
      "UZVer",
   };

/*********************************************************************/
/* Numeric Error Return Strings                                      */
/*********************************************************************/

#define  NO_UTIL_ERROR    "0"          /* No error whatsoever        */
#define  ERROR_NOMEM      "2"          /* Insufficient memory        */

/*********************************************************************/
/* Numeric Return calls                                              */
/*********************************************************************/

#define  INVALID_ROUTINE 40            /* Raise Rexx error           */
#define  VALID_ROUTINE    0            /* Successful completion      */

/*********************************************************************/
/* Some useful macros                                                */
/*********************************************************************/

#define BUILDRXSTRING(t, s) { \
  strcpy((t)->strptr,(s));\
  (t)->strlength = strlen((s)); \
}


/*********************************************************************/
/****************  UNZIPAPI Supporting Functions  ********************/
/****************  UNZIPAPI Supporting Functions  ********************/
/****************  UNZIPAPI Supporting Functions  ********************/
/*********************************************************************/


int RexxReturn(__GPRO__ int nodefault, RXSTRING *retstr)
{
  int ret = G.os2.rexx_error;
  if (G.filenotfound)
    G.os2.rexx_mes = "file not found";
  if (*G.os2.rexx_mes != '0') {
    if (retstr->strlength > 255) {
      DosFreeMem(retstr->strptr);
      retstr->strptr = NULL;
    }
  } else if (nodefault)
    goto noBuild;
  BUILDRXSTRING(retstr, G.os2.rexx_mes);
 noBuild:
  DESTROYGLOBALS();
  return ret;
}

/* Get a variable from REXX, return 0 if OK */
int GetVariable(__GPRO__ const char *name)
{
  G.os2.request.shvnext = NULL;
  EZRXSTRING(G.os2.request.shvname, name);
  G.os2.request.shvnamelen = G.os2.request.shvname.strlength;
  G.os2.request.shvvalue.strptr = G.os2.buffer;
  G.os2.request.shvvalue.strlength = IBUF_LEN;
  G.os2.request.shvvaluelen = IBUF_LEN;
  G.os2.request.shvcode = RXSHV_SYFET;
  G.os2.request.shvret = 0;
  switch (RexxVariablePool(&G.os2.request)) {
  case RXSHV_MEMFL:
    G.os2.rexx_mes = ERROR_NOMEM;
    break;
  case RXSHV_BADN:
  case RXSHV_NEWV:
    G.os2.request.shvvaluelen = 0;
  case RXSHV_OK:
    *(G.os2.buffer+G.os2.request.shvvaluelen) = 0;
    return G.os2.request.shvvaluelen;
  }
  return 0;
}


/* Get REXX compound variable */
/* Stem must exist in G.os2.getvar_buf w/ length in G.os2.getvar_len */
int GetVariableIndex(__GPRO__ int index)
{
  sprintf(G.os2.getvar_buf+G.os2.getvar_len,"%d",index);
  return GetVariable(__G__ G.os2.getvar_buf);
}


/* Transfer REXX array to standard C string array */
/* Returns number of elements */
/* User is responsible for calling KillStringArray */

int CompoundToStringArray(__GPRO__ char ***pointer, const char *name)
{
  int count;
  int total;
  char **trav;

  G.os2.getvar_len = strlen(name);
  memcpy(G.os2.getvar_buf,name,G.os2.getvar_len+1);
  if (*(G.os2.getvar_buf+G.os2.getvar_len-1) != '.')
    *(G.os2.getvar_buf+G.os2.getvar_len++) = '.', *(G.os2.getvar_buf+G.os2.getvar_len) = 0;

  if (GetVariableIndex(__G__ 0))
    return 0;

  total = atoi(G.os2.buffer);
  *pointer = (char **)malloc((total+1)<<2);
  trav = *pointer;
  for (count = 1; count <= total; count++) {
    GetVariableIndex(__G__ count);
    trav[count-1] = (char *)malloc(strlen(G.os2.buffer)+1);
    strcpy(trav[count-1],G.os2.buffer);
  }
  trav[count-1] = NULL;
  return total;
}


/* Kill string array created by CompoundToStringArray */

void KillStringArray(char **pointer)
{
  char **trav=pointer;
  while (*trav != NULL) {
    free(*trav);
    trav++;
  }
  free(pointer);
}


/*************************************************************************
* Function:  UZDropFuncs                                                 *
*                                                                        *
* Syntax:    call UZDropFuncs                                            *
*                                                                        *
* Return:    NO_UTIL_ERROR - Successful.                                 *
*************************************************************************/

ULONG UZDropFuncs(CHAR *name, ULONG numargs, RXSTRING args[],
                  CHAR *queuename, RXSTRING *retstr)
{
  INT     entries;                     /* Num of entries             */
  INT     j;                           /* Counter                    */

  if (numargs != 0)                    /* no arguments for this      */
    return INVALID_ROUTINE;            /* raise an error             */

  retstr->strlength = 0;               /* return a null string result*/

  entries = sizeof(RxFncTable)/sizeof(PSZ);

  for (j = 0; j < entries; j++)
    RexxDeregisterFunction(RxFncTable[j]);

  return VALID_ROUTINE;                /* no error on call           */
}


/*************************************************************************
* Function:  UZFileTree                                                  *
*                                                                        *
* Syntax:    call UZFileTree zipfile, stem[, include-filespec]           *
*                                [, exclude-filespec][, options]         *
*                                                                        *
* Params:    zipfile  - Name of zip file to search.                      *
*            stem     - Name of stem var to store results in.            *
*            include  - Filespec to search for (may include * and ?).    *
*            exclude  - Filespec to exclude (may include * and ?).       *
*            options  - Either of the following:                         *
*                       'F' - Give file statistics.                      *
*                          Length Date Time Name                         *
*                       'Z' - Give zip statistics, too.                  *
*                          Length Method Size Ratio Date Time CRC-32 Name*
*                       Default is to return only filenames              *
*                                                                        *
* Return:    NO_UTIL_ERROR   - Successful.                               *
*            ERROR_NOMEM     - Out of memory.                            *
*************************************************************************/

ULONG UZFileTree(CHAR *name, ULONG numargs, RXSTRING args[],
                 CHAR *queuename, RXSTRING *retstr)
{
                                       /* validate arguments         */
  char *incname[2];
  char *excname[2];
  CONSTRUCTGLOBALS();

  if (numargs < 2 || numargs > 5 ||
      !RXVALIDSTRING(args[0]) ||
      !RXVALIDSTRING(args[1]) ||
      args[0].strlength > 255) {
    DESTROYGLOBALS();
    return INVALID_ROUTINE;            /* Invalid call to routine    */
  }
                                       /* initialize data area       */
  SetOutputVarStem(__G__ args[1].strptr);
  G.wildzipfn = args[0].strptr;
  G.process_all_files = TRUE;

  uO.lflag = 1;
  uO.zipinfo_mode = TRUE;
  uO.C_flag = 1;
  G.extract_flag = FALSE;
  uO.qflag = 2;

  if (numargs >= 3 &&                  /* check third option         */
      !RXNULLSTRING(args[2]) &&
      args[2].strlength > 0) {            /* a zero length string isn't */
    if (!(G.filespecs = CompoundToStringArray(__G__ &G.pfnames,args[2].strptr))) {
      G.pfnames = incname;
      incname[0] = args[2].strptr;
      incname[1] = NULL;
      G.filespecs = 1;
    }
    G.process_all_files = FALSE;
  }

  if (numargs >= 4 &&                  /* check third option         */
      !RXNULLSTRING(args[3]) &&
      args[3].strlength > 0) {            /* a zero length string isn't */
    if (!(G.xfilespecs = CompoundToStringArray(__G__ &G.pxnames,args[3].strptr))) {
      G.pxnames = excname;
      excname[0] = args[3].strptr;
      excname[1] = NULL;
      G.xfilespecs = 1;
    }
    G.process_all_files = FALSE;
  }

  if (numargs == 5 &&                  /* check third option         */
      !RXNULLSTRING(args[4]) &&
      args[4].strlength > 0) {            /* a zero length string isn't */
    int first = *args[4].strptr & 0x5f;

    if (first == 'Z')
      uO.vflag = 2, uO.lflag = 0, uO.zipinfo_mode = FALSE;
    else if (first == 'F')
      uO.vflag = 1, uO.lflag = 0, uO.zipinfo_mode = FALSE;
  }

  process_zipfiles(__G);
  SetOutputVarLength(__G);
  if (G.filespecs > 0 && G.pfnames != incname)
    KillStringArray(G.pfnames);
  if (G.xfilespecs > 0 && G.pxnames != excname)
    KillStringArray(G.pxnames);
  return RexxReturn(__G__ 0,retstr);        /* no error on call           */
}


/*************************************************************************
* Function:  UZUnZipToVar                                                *
*                                                                        *
* Syntax:    call UZUnZipToVar zipfile, filespec [, stem]                *
*                                                                        *
* Params:    zipfile  - Name of zip file to search.                      *
*            filespec - File to extract                                  *
*            stem     - If you specify a stem variable, the file will be *
*                       extracted to the variable, one line per index    *
*                       In this case, 0 will be returned                 *
*                                                                        *
* Return:    Extracted file                                              *
*            ERROR_NOMEM     - Out of memory.                            *
*************************************************************************/

ULONG UZUnZipToVar(CHAR *name, ULONG numargs, RXSTRING args[],
                          CHAR *queuename, RXSTRING *retstr)
{
  CONSTRUCTGLOBALS();
  UzpBuffer *ub = (UzpBuffer *)retstr;
                                       /* validate arguments         */
  if (numargs < 2 || numargs > 3 ||
      !RXVALIDSTRING(args[0]) ||
      !RXVALIDSTRING(args[1]) ||
      args[0].strlength == 0 ||
      args[1].strlength == 0) {
    DESTROYGLOBALS();
    return INVALID_ROUTINE;            /* Invalid call to routine    */
  }

  uO.C_flag = 1;
  G.redirect_data=1;
  if (numargs == 3) {
    if (!RXVALIDSTRING(args[2]) ||
        RXNULLSTRING(args[1]) ||
        args[2].strlength == 0) {
      DESTROYGLOBALS();
      return INVALID_ROUTINE;            /* Invalid call to routine    */
    }
    SetOutputVarStem(__G__ args[2].strptr);
    G.redirect_text = 0;
    G.redirect_data++;
  }
  unzipToMemory(__G__ args[0].strptr, args[1].strptr,
                G.redirect_data==1 ? ub : NULL);
  return RexxReturn(__G__ G.redirect_data==1,retstr);
}


/*************************************************************************
* Function:  UZUnZipToStem                                               *
*                                                                        *
* Syntax:    call UZUnZipToStem zipfile, stem[, include-filespec]        *
*                                [, exclude-filespec][, mode]            *
*                                                                        *
* Params:    zipfile  - Name of zip file to search.                      *
*            stem     - Name of stem var to store files in.              *
*            include  - Filespec to search for (may include * and ?).    *
*            exclude  - Filespec to exclude (may include * and ?).       *
*            mode     - Specifies 'F'lat or 'T'ree mode.  Umm, this is   *
*                        hard to explain so I'll give an example, too.   *
*                       Assuming a file unzip.zip containing:            *
*                               unzip.c                                  *
*                               unshrink.c                               *
*                               extract.c                                *
*                               os2/makefile.os2                         *
*                               os2/os2.c                                *
*                               os2/dll/dll.def                          *
*                               os2/dll/unzipapi.c                       *
*                                                                        *
*                       -- In flat mode, each file is stored in          *
*                          stem.fullname i.e. stem."os2/dll/unzipapi.c"  *
*                          A list of files is created in stem.<index>    *
*                                                                        *
*                       Flat mode returns:                               *
*                               stem.0 = 7                               *
*                               stem.1 = unzip.c                         *
*                               stem.2 = unshrink.c                      *
*                               stem.3 = extract.c                       *
*                               stem.4 = os2/makefile.os2                *
*                               stem.5 = os2/os2.c                       *
*                               stem.6 = os2/dll/dll.def                 *
*                               stem.7 = os2/dll/unzipapi.c              *
*                                                                        *
*                       And the following contain the contents of the    *
*                       various programs:                                *
*                               stem.unzip.c                             *
*                               stem.unshrink.c                          *
*                               stem.extract.c                           *
*                               stem.os2/makefile.os2                    *
*                               stem.os2/os2.c                           *
*                               stem.os2/dll/dll.def                     *
*                               stem.os2/dll/unzipapi.c                  *
*                                                                        *
*                       -- In tree mode, slashes are converted to periods*
*                          in the pathname thus the above file would have*
*                          been stored in stem.os2.dll.unzipapi.c        *
*                          The index would then be stored in stem.OS2.   *
*                          DLL.<index>.                                  *
*                                                                        *
*                       NOTE: All path names are converted to uppercase  *
*                                                                        *
*                       Tree mode returns:                               *
*                               stem.0 = 4                               *
*                               stem.1 = unzip.c                         *
*                               stem.2 = unshrink.c                      *
*                               stem.3 = extract.c                       *
*                               stem.4 = OS2/                            *
*                                                                        *
*                               stem.OS2.0 = 3                           *
*                               stem.OS2.1 = makefile.os2                *
*                               stem.OS2.2 = os2.c                       *
*                               stem.OS2.3 = DLL/                        *
*                                                                        *
*                               stem.OS2.DLL.0 = 2                       *
*                               stem.OS2.DLL.1 = def                     *
*                               stem.OS2.DLL.2 = unzipapi.c              *
*                                                                        *
*                       And the following contain the contents of the    *
*                       various programs:                                *
*                               stem.unzip.c                             *
*                               stem.unshrink.c                          *
*                               stem.extract.c                           *
*                               stem.OS2.makefile.os2                    *
*                               stem.OS2.os2.c                           *
*                               stem.OS2.DLL.dll.def                     *
*                               stem.OS2.DLL.unzipapi.c                  *
*                                                                        *
*                                                                        *
* Return:    NO_UTIL_ERROR   - Successful.                               *
*            ERROR_NOMEM     - Out of memory.                            *
*************************************************************************/

ULONG UZUnZipToStem(CHAR *name, ULONG numargs, RXSTRING args[],
                          CHAR *queuename, RXSTRING *retstr)
{
  char *incname[2];
  char *excname[2];
  CONSTRUCTGLOBALS();
                                       /* validate arguments         */
  if (numargs < 2 || numargs > 5 ||
      !RXVALIDSTRING(args[0]) ||
      !RXVALIDSTRING(args[1]) ||
      args[0].strlength > 255) {
    DESTROYGLOBALS();
    return INVALID_ROUTINE;            /* Invalid call to routine    */
  }
                                       /* initialize data area       */
  G.wildzipfn = args[0].strptr;
  G.process_all_files = TRUE;

  uO.C_flag = 1;
  G.extract_flag = TRUE;
  SetOutputVarStem(__G__ args[1].strptr);
  G.redirect_data = 3;
  G.redirect_text = 0;

  if (numargs >= 3 &&                  /* check third option         */
      !RXNULLSTRING(args[2]) &&
      args[2].strlength > 0) {            /* a zero length string isn't */
    if (!(G.filespecs = CompoundToStringArray(__G__ &G.pfnames,args[2].strptr))) {
      G.pfnames = incname;
      incname[0] = args[2].strptr;
      incname[1] = NULL;
      G.filespecs = 1;
    }
    G.process_all_files = FALSE;
  }

  if (numargs >= 4 &&                  /* check third option         */
      !RXNULLSTRING(args[3]) &&
      args[3].strlength > 0) {            /* a zero length string isn't */
    if (!(G.xfilespecs = CompoundToStringArray(__G__ &G.pxnames,args[3].strptr))) {
      G.pxnames = excname;
      excname[0] = args[3].strptr;
      excname[1] = NULL;
      G.xfilespecs = 1;
    }
    G.process_all_files = FALSE;
  }

  if (numargs == 5 &&                  /* check third option         */
      !RXNULLSTRING(args[4]) &&
      (*args[4].strptr & 0x5f) == 'T') {
    G.redirect_data++;
    G.os2.request.shvnext = NULL;
    EZRXSTRING(G.os2.request.shvname, args[4].strptr);
    G.os2.request.shvnamelen = G.os2.request.shvname.strlength;
    G.os2.request.shvcode = RXSHV_SYDRO;
    G.os2.request.shvret = 0;
    RexxVariablePool(&G.os2.request);
  }


  uO.qflag = 2;

  process_zipfiles(__G);
  if (G.filespecs > 0 && G.pfnames != incname)
    KillStringArray(G.pfnames);
  if (G.xfilespecs > 0 && G.pxnames != excname)
    KillStringArray(G.pxnames);
  if (G.redirect_data == 3)
    SetOutputVarLength(__G);
  return RexxReturn(__G__ 0,retstr);            /* no error on call           */
}


/*************************************************************************
* Function:  UZLoadFuncs                                                 *
*                                                                        *
* Syntax:    call UZLoadFuncs [option]                                   *
*                                                                        *
* Params:    none                                                        *
*                                                                        *
* Return:    null string                                                 *
*************************************************************************/

ULONG UZLoadFuncs(CHAR *name, ULONG numargs, RXSTRING args[],
                           CHAR *queuename, RXSTRING *retstr)
{
  INT    entries;                      /* Num of entries             */
  INT    j;                            /* Counter                    */

  retstr->strlength = 0;               /* set return value           */
                                       /* check arguments            */
  if (numargs > 0)
    return INVALID_ROUTINE;

  entries = sizeof(RxFncTable)/sizeof(PSZ);

  for (j = 0; j < entries; j++) {
    RexxRegisterFunctionDll(RxFncTable[j],
          "UNZIP32", RxFncTable[j]);
  }
  return VALID_ROUTINE;
}



/*************************************************************************
* Function:  UZVer                                                       *
*                                                                        *
* Syntax:    call UZVer                                                  *
*                                                                        *
* Return:    Version of Unzip                                            *
*************************************************************************/

ULONG UZVer(CHAR *name, ULONG numargs, RXSTRING args[],
                        CHAR *queuename, RXSTRING *retstr)
{
  if (numargs > 1)                    /* validate arg count         */
    return INVALID_ROUTINE;

  if (numargs == 0 || (*args[0].strptr & 0x5f) != 'L')
    /* strcpy( retstr->strptr, UZ_VERNUM );    "5.13a BETA" */
    sprintf( retstr->strptr, "%d.%d%d%s", UZ_MAJORVER, UZ_MINORVER,
      PATCHLEVEL, BETALEVEL );
  else
    /* strcpy( retstr->strptr, UZ_VERSION );   UZ_VERNUM" of 26 Sep 94" */
    sprintf( retstr->strptr, "%d.%d%d%s of %s", UZ_MAJORVER, UZ_MINORVER,
      PATCHLEVEL, BETALEVEL, VERSION_DATE );
  retstr->strlength = strlen(retstr->strptr);
  return VALID_ROUTINE;
}


/*************************************************************************
* Function:  UZUnZip                                                     *
*                                                                        *
* Syntax:    call UZUnZip                                                *
*                                                                        *
* Return:    Unzip return code                                           *
*************************************************************************/

ULONG UZUnZip(CHAR *name, ULONG numargs, RXSTRING args[],
                        CHAR *queuename, RXSTRING *retstr)
{
  char *argv[30];
  char *scan;
  int argc=0;
  int idx;
  CONSTRUCTGLOBALS();

  if (numargs < 1 || numargs > 2 ||
      args[0].strlength > 255) {
    DESTROYGLOBALS();
    return INVALID_ROUTINE;            /* Invalid call to routine    */
  }
                                       /* initialize data area       */
  if (numargs == 2)
    SetOutputVarStem(__G__ args[1].strptr);

  scan = args[0].strptr;
  argv[argc++] = "";         /* D:\\SOURCECODE\\UNZIP51S\\UNZIP.COM"; */
  while (*scan == ' ')
    scan++;
  argv[argc++] = scan;
  while ( (scan = strchr(scan,' ')) != NULL) {
    *scan++ = 0;
    while (*scan == ' ')
      scan++;
    argv[argc++] = scan;
  }
  if (*argv[argc-1] == 0)
    argc--;
  argv[argc] = 0;

         /* GRR:  should resetMainFlags() be called in here somewhere? */

  sprintf(retstr->strptr, "%d", unzip(__G__ argc, argv));   /* a.k.a. MAIN() */
  if (numargs == 2)
    SetOutputVarLength(__G);
  retstr->strlength = strlen(retstr->strptr);
  return RexxReturn(__G__ 1,retstr);
}

int varmessage(__GPRO__ uch *buf, ulg size)
{
  if (size > 0)
    memcpy(G.os2.buffer+G.os2.putchar_idx,buf,size);
    G.os2.putchar_idx = TextSetNext(__G__ G.os2.buffer, size+G.os2.putchar_idx,0);
  return 0;
}

int varputchar(__GPRO__ int c)
{
  G.os2.buffer[G.os2.putchar_idx++] = c;
  if (c == '\n') {
    G.os2.buffer[G.os2.putchar_idx] = 0;
    if (G.os2.output_var[0])
      G.os2.putchar_idx = TextSetNext(__G__ G.os2.buffer, G.os2.putchar_idx,0);
    else {
      G.os2.buffer[--G.os2.putchar_idx] = 0;
      puts(G.os2.buffer);
      G.os2.putchar_idx = 0;
    }
  }
  return 1;
}



int SetOutputVarStem(__GPRO__ const char *name)
{
  int len=strlen(name);
  G.redirect_text=1;
  G.os2.output_idx = 0;
  strcpy(G.os2.output_var, name);
  if (len) {
    strupr(G.os2.output_var);                 /* uppercase the name         */
    if (*(G.os2.output_var+len-1) != '.') {
      *(G.os2.output_var+len) = '.';
      len++;
      *(G.os2.output_var+len) = 0;
    }
    WriteToVariable(__G__ G.os2.output_var,"",0);
  }
  G.os2.stem_len = len;
  return G.os2.stem_len;
}

int SetOutputVar(__GPRO__ const char *name)
{
  int len=strlen(name);
  G.redirect_text=1;
  G.os2.output_idx = 0;
  strcpy(G.os2.output_var, name);
  strupr(G.os2.output_var);                 /* uppercase the name         */
  if (*(name+len-1) == '.')
    G.os2.stem_len = len;
  else
    G.os2.stem_len = 0;
  return G.os2.stem_len;
}

int SetOutputVarLength(__GPRO)
{
  if (G.os2.stem_len > 0) {
    if (G.os2.putchar_idx)
      TextSetNext(__G__ G.os2.buffer,G.os2.putchar_idx,1);
    return PrintToSubVariable(__G__ 0,"%d",G.os2.output_idx);
  }
  return 0;
}

int PrintToVariable(__GPRO__ const char *name, const char *format,...)
{
  va_list arg_ptr;
  int ret;

  va_start(arg_ptr, format);
  ret = _PrintToVariable(__G__ name, format, arg_ptr);
  va_end(arg_ptr);
  return ret;
}

int WriteToVariable(__GPRO__ const char *name, char *buffer, int len)
{
  G.os2.request.shvnext = NULL;
  EZRXSTRING(G.os2.request.shvname, name);
  G.os2.request.shvnamelen = G.os2.request.shvname.strlength;
  G.os2.request.shvvalue.strptr = buffer;
  G.os2.request.shvvalue.strlength = len;
  G.os2.request.shvvaluelen = len;
  G.os2.request.shvcode = RXSHV_SET;
  G.os2.request.shvret = 0;
  switch (RexxVariablePool(&G.os2.request)) {
  case RXSHV_BADN:
    G.os2.rexx_error = INVALID_ROUTINE;
    break;
  case RXSHV_MEMFL:
    G.os2.rexx_mes = ERROR_NOMEM;
    break;
  case RXSHV_OK:
    return 0;
  }
  return INVALID_ROUTINE;      /* error on non-zero          */
}

int _PrintToVariable(__GPRO__ const char *name, const char *format, va_list arg_ptr)
{
  int ret = vsprintf(G.os2.buffer, format, arg_ptr);
  WriteToVariable(__G__ name, G.os2.buffer, strlen(G.os2.buffer));
  return ret;
}

int PrintToSubVariable(__GPRO__ int idx, const char *format, ...)
{
  va_list arg_ptr;
  int ret;

  if (G.os2.stem_len == 0)
    return INVALID_ROUTINE;      /* error on non-zero          */
  sprintf(G.os2.output_var+G.os2.stem_len,"%d",idx);

  va_start(arg_ptr, format);
  ret = _PrintToVariable(__G__ G.os2.output_var, format, arg_ptr);
  va_end(arg_ptr);
  return ret;
}


int WriteToNextVariable(__GPRO__ char *buffer, int len)
{
  if (G.os2.stem_len > 0) {
    G.os2.output_idx++;
    sprintf(G.os2.output_var+G.os2.stem_len,"%d",G.os2.output_idx);
  }
  return WriteToVariable(__G__ G.os2.output_var, buffer, len);
}


int TextSetNext(__GPRO__ char *buffer, int len, int all)
{
  char *scan = buffer, *next, *base=buffer;
  int remaining=len;
  int ret;

  while ((next = strchr(scan,'\n')) != NULL && remaining > 0) {
    if (next > scan && *(next-1) == 0xd)
      *(next-1) = 0;
    else
      *next = 0;
    if (WriteToNextVariable(__G__ scan,strlen(scan)))
      return 0;
    next++;
    remaining -= (next-scan);
    scan = next;
  }
  if (remaining > 0)
    if (all) {
      *(scan+remaining) = 0;
      WriteToNextVariable(__G__ scan,remaining);
    } else {
      memcpy(buffer,scan,remaining);
      return remaining;
    }

  return 0;
}

int finish_REXX_redirect(__GPRO)
{
  char *scan, *ptr;
  int idx=0, first=1, offset;

  if (!G.redirect_size)
    return 0;
  switch(G.redirect_data) {
  case 1:
    break;
  case 2:
    TextSetNext(__G__ G.redirect_buffer, G.redirect_size, 1);
    SetOutputVarLength(__G);
    DosFreeMem(G.redirect_buffer);
    G.redirect_buffer = NULL;
    G.redirect_size = 0;
    break;
  case 3:
    WriteToNextVariable(__G__ G.filename,strlen(G.filename));
    sprintf(G.os2.output_var+G.os2.stem_len,G.filename);
    WriteToVariable(__G__ G.os2.output_var, G.redirect_buffer, G.redirect_size);
    DosFreeMem(G.redirect_buffer);
    G.redirect_buffer = NULL;
    G.redirect_size = 0;
    break;
  case 4:
    if ((scan = strrchr(G.filename,'/')) != NULL) {
      idx = *scan;
      *scan = 0;
      strupr(G.filename);
      *scan = idx;
    }
    scan = G.os2.output_var+G.os2.stem_len;
    strcpy(scan,G.filename);
    while ((scan = strchr(scan,'/')) != NULL)
      *scan = '.';
    WriteToVariable(__G__ G.os2.output_var, G.redirect_buffer, G.redirect_size);
    DosFreeMem(G.redirect_buffer);
    G.redirect_buffer = NULL;
    G.redirect_size = 0;
    strcpy(G.os2.getvar_buf, G.os2.output_var);
    do {
      if ((scan = strrchr(G.filename,'/')) == NULL)
        offset = 0;
      else
        offset = scan-G.filename+1;
      if (first || !GetVariable(__G__ G.os2.output_var)) {
        ptr = G.os2.getvar_buf+offset+G.os2.stem_len;
        *ptr = '0';
        *(ptr+1) = 0;
        if (!GetVariable(__G__ G.os2.getvar_buf))
          idx = 1;
        else
          idx = atoi(G.os2.buffer)+1;
        PrintToVariable(__G__ G.os2.getvar_buf,"%d",idx);
        sprintf(ptr,"%d",idx);
        if (!first) {
          PrintToVariable(__G__ G.os2.output_var,"%d",idx);
          idx = strlen(G.filename);
          *(G.filename+idx)   = '/';
          *(G.filename+idx+1) = 0;
        }
        WriteToVariable(__G__ G.os2.getvar_buf,G.filename+offset,strlen(G.filename+offset));
        first=0;
      }
      if (offset) {
        *(G.os2.output_var+G.os2.stem_len+offset-1)   = 0;
        *scan = 0;
      }
    } while (offset);
    break;
  }
  return 0;
}

#endif /* OS2DLL */
