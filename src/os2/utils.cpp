/////////////////////////////////////////////////////////////////////////////
// Name:        utils.cpp
// Purpose:     Various utilities
// Author:      David Webster
// Modified by:
// Created:     09/17/99
// RCS-ID:      $Id$
// Copyright:   (c) David Webster
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
    #include "wx/setup.h"
    #include "wx/utils.h"
    #include "wx/app.h"
#endif  //WX_PRECOMP

#include "wx/os2/private.h"
#include "wx/intl.h"
#include "wx/apptrait.h"

#include <ctype.h>
#ifdef __EMX__
#include <dirent.h>
#endif

#include "wx/log.h"

#include <io.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>

#define PURE_32

#ifndef __EMX__
#include <upm.h>
#include <netcons.h>
#include <netbios.h>
#endif

static const wxChar WX_SECTION[] = _T("wxWidgets");
static const wxChar eHOSTNAME[]  = _T("HostName");
static const wxChar eUSERID[]    = _T("UserId");
static const wxChar eUSERNAME[]  = _T("UserName");

// For the following functions we SHOULD fill in support
// for Windows-NT (which I don't know) as I assume it begin
// a POSIX Unix (so claims MS) that it has some special
// functions beyond those provided by WinSock

// Get full hostname (eg. DoDo.BSn-Germany.crg.de)
bool wxGetHostName(
  wxChar*                           zBuf
, int                               nMaxSize
)
{
#if wxUSE_NET_API
    char                            zServer[256];
    char                            zComputer[256];
    unsigned long                   ulLevel = 0;
    unsigned char*                  zBuffer = NULL;
    unsigned long                   ulBuffer = 256;
    unsigned long*                  pulTotalAvail = NULL;

    NetBios32GetInfo( (const unsigned char*)zServer
                     ,(const unsigned char*)zComputer
                     ,ulLevel
                     ,zBuffer
                     ,ulBuffer
                     ,pulTotalAvail
                    );
    strcpy(zBuf, zServer);
#else
    wxChar*                         zSysname;
    const wxChar*                   zDefaultHost = _T("noname");

    if ((zSysname = wxGetenv(_T("SYSTEM_NAME"))) == NULL)
    {
        ::PrfQueryProfileString( HINI_PROFILE
                                ,(PSZ)WX_SECTION
                                ,(PSZ)eHOSTNAME
                                ,(PSZ)zDefaultHost
                                ,(void*)zBuf
                                ,(ULONG)nMaxSize - 1
                               );
    }
    else
        wxStrncpy(zBuf, zSysname, nMaxSize - 1);
    zBuf[nMaxSize] = _T('\0');
#endif
    return *zBuf ? true : false;
}

// Get user ID e.g. jacs
bool wxGetUserId(
  wxChar*                           zBuf
, int                               nType
)
{
#if defined(__VISAGECPP__)
    long                            lrc;
    // UPM procs return 0 on success
    lrc = U32ELOCU((unsigned char*)zBuf, (unsigned long *)&nType);
    if (lrc == 0) return true;
#endif
    return false;
}

bool wxGetUserName(
  wxChar*                           zBuf
, int                               nMaxSize
)
{
#ifdef USE_NET_API
    wxGetUserId( zBuf
                ,nMaxSize
               );
#else
    wxStrncpy(zBuf, _T("Unknown User"), nMaxSize);
#endif
    return true;
}

int wxKill(
  long                              lPid
, wxSignal                          eSig
, wxKillError*                      peError
, int                               flags
)
{
    return((int)::DosKillProcess(0, (PID)lPid));
}

//
// Execute a program in an Interactive Shell
//
bool wxShell(
  const wxString&                   rCommand
)
{
    wxChar*                         zShell = _T("CMD.EXE");
    wxString                        sInputs;
    STARTDATA                       SData = {0};
    PSZ                             PgmTitle = "Command Shell";
    APIRET                          rc;
    PID                             vPid = 0;
    ULONG                           ulSessID = 0;
    UCHAR                           achObjBuf[256] = {0}; //error data if DosStart fails
    RESULTCODES                     vResult;

    SData.Length   = sizeof(STARTDATA);
    SData.Related  = SSF_RELATED_INDEPENDENT;
    SData.FgBg     = SSF_FGBG_FORE;
    SData.TraceOpt = SSF_TRACEOPT_NONE;
    SData.PgmTitle = PgmTitle;
    SData.PgmName  = zShell;

    sInputs = "/C " + rCommand;
    SData.PgmInputs     = (BYTE*)sInputs.c_str();
    SData.TermQ         = 0;
    SData.Environment   = 0;
    SData.InheritOpt    = SSF_INHERTOPT_SHELL;
    SData.SessionType   = SSF_TYPE_WINDOWABLEVIO;
    SData.IconFile      = 0;
    SData.PgmHandle     = 0;
    SData.PgmControl    = SSF_CONTROL_VISIBLE | SSF_CONTROL_MAXIMIZE;
    SData.InitXPos      = 30;
    SData.InitYPos      = 40;
    SData.InitXSize     = 200;
    SData.InitYSize     = 140;
    SData.Reserved      = 0;
    SData.ObjectBuffer  = (char*)achObjBuf;
    SData.ObjectBuffLen = (ULONG)sizeof(achObjBuf);

    rc = ::DosStartSession(&SData, &ulSessID, &vPid);
    if (rc == 0 || rc == 457) // NO_ERROR or SMG_START_IN_BACKGROUND
    {
        PTIB                            ptib;
        PPIB                            ppib;

        ::DosGetInfoBlocks(&ptib, &ppib);

        ::DosWaitChild( DCWA_PROCESS
                       ,DCWW_WAIT
                       ,&vResult
                       ,&ppib->pib_ulpid
                       ,vPid
                      );
    }
    return (rc != 0);
}

// Shutdown or reboot the PC
bool wxShutdown(wxShutdownFlags wFlags)
{
    // TODO
    return false;
}

// Get free memory in bytes, or -1 if cannot determine amount (e.g. on UNIX)
wxMemorySize wxGetFreeMemory()
{
    void*                           pMemptr = NULL;
    ULONG                           lSize;
    ULONG                           lMemFlags;
    APIRET                          rc;

    lMemFlags = PAG_FREE;
    rc = ::DosQueryMem(pMemptr, &lSize, &lMemFlags);
    if (rc != 0)
        lSize = -1L;
    return (wxMemorySize)lSize;
}

// ----------------------------------------------------------------------------
// env vars
// ----------------------------------------------------------------------------

bool wxGetEnv(const wxString& var, wxString *value)
{
    // wxGetenv is defined as getenv()
    wxChar *p = wxGetenv(var);
    if ( !p )
        return false;

    if ( value )
    {
        *value = p;
    }

    return true;
}

bool wxSetEnv(const wxString& variable, const wxChar *value)
{
#if defined(HAVE_SETENV)
    return setenv(variable.mb_str(), value ? wxString(value).mb_str().data()
                                           : NULL, 1 /* overwrite */) == 0;
#elif defined(HAVE_PUTENV)
    wxString s = variable;
    if ( value )
        s << _T('=') << value;

    // transform to ANSI
    const char *p = s.mb_str();

    // the string will be free()d by libc
    char *buf = (char *)malloc(strlen(p) + 1);
    strcpy(buf, p);

    return putenv(buf) == 0;
#else // no way to set an env var
    return false;
#endif
}

void wxMilliSleep(
  unsigned long                     ulMilliseconds
)
{
    ::DosSleep(ulMilliseconds);
}

void wxMicroSleep(
  unsigned long                     ulMicroseconds
)
{
    ::DosSleep(ulMicroseconds/1000);
}

void wxSleep(
  int                               nSecs
)
{
    ::DosSleep(1000 * nSecs);
}

// Consume all events until no more left
void wxFlushEvents()
{
//  wxYield();
}

#if WXWIN_COMPATIBILITY_2_2

// Output a debug mess., in a system dependent fashion.
void wxDebugMsg(
  const wxChar*                     zFmt ...
)
{
    va_list                         vAp;
    static wxChar                   zBuffer[512];

    if (!wxTheApp->GetWantDebugOutput())
        return ;
    va_start(vAp, zFmt);
    sprintf(zBuffer, zFmt, vAp) ;
    va_end(vAp);
}

// Non-fatal error: pop up message box and (possibly) continue
void wxError(
  const wxString&                   rMsg
, const wxString&                   rTitle
)
{
    wxChar *wxBuffer = new wxChar[256];
    wxSprintf(wxBuffer, "%s\nContinue?", WXSTRINGCAST rMsg);
    if (::WinMessageBox( HWND_DESKTOP
                        ,NULL
                        ,(PSZ)wxBuffer
                        ,(PSZ)WXSTRINGCAST rTitle
                        ,0
                        ,MB_ICONEXCLAMATION | MB_YESNO
                       ) == MBID_YES)
    delete[] wxBuffer;
    wxExit();
}

// Fatal error: pop up message box and abort
void wxFatalError(
  const wxString&                   rMsg
, const wxString&                   rTitle
)
{
    unsigned long                   ulRc;

    ulRc = ::WinMessageBox( HWND_DESKTOP
                           ,NULL
                           ,WXSTRINGCAST rMsg
                           ,WXSTRINGCAST rTitle
                           ,0
                           ,MB_NOICON | MB_OK
                          );
    DosExit(EXIT_PROCESS, ulRc);
}

#endif // WXWIN_COMPATIBILITY_2_2

// Emit a beeeeeep
void wxBell()
{
    DosBeep(1000,1000); // 1kHz during 1 sec.
}


void wxAppTraits::InitializeGui(unsigned long &ulHab)
{
}

void wxAppTraits::TerminateGui(unsigned long ulHab)
{
}

wxToolkitInfo & wxConsoleAppTraits::GetToolkitInfo()
{
    static wxToolkitInfo  vInfo;
    ULONG                 ulSysInfo[QSV_MAX] = {0};
    APIRET                ulrc;

    vInfo.name = _T("wxBase");
    ulrc = ::DosQuerySysInfo( 1L
                             ,QSV_MAX
                             ,(PVOID)ulSysInfo
                             ,sizeof(ULONG) * QSV_MAX
                            );
    if (ulrc == 0L)
    {
        vInfo.versionMajor = ulSysInfo[QSV_VERSION_MAJOR] / 10;
        vInfo.versionMinor = ulSysInfo[QSV_VERSION_MINOR];
    }
    vInfo.os = wxOS2_PM;
    return vInfo;
}

// ---------------------------------------------------------------------------
const wxChar* wxGetHomeDir(
  wxString*                         pStr
)
{
    wxString&                       rStrDir = *pStr;

    // OS/2 has no idea about home,
    // so use the working directory instead.
    // However, we might have a valid HOME directory,
    // as is used on many machines that have unix utilities
    // on them, so we should use that, if available.

    // 256 was taken from os2def.h
#ifndef MAX_PATH
#  define MAX_PATH  256
#endif

    const wxChar *szHome = wxGetenv("HOME");
    if ( szHome == NULL ) {
      // we're homeless, use current directory.
      rStrDir = wxT(".");
    }
    else
       rStrDir = szHome;

    return rStrDir.c_str();
}

// Hack for OS/2
wxChar* wxGetUserHome (
  const wxString&                   rUser
)
{
    wxChar*                         zHome;
    wxString                        sUser1(rUser);

    char *wxBuffer = new wxChar[256];
#ifndef __EMX__
    if (sUser1 != _T(""))
    {
        wxChar                      zTmp[64];

        if (wxGetUserId( zTmp
                        ,sizeof(zTmp)/sizeof(char)
                       ))
        {
            // Guests belong in the temp dir
            if (wxStricmp(zTmp, _T("annonymous")) == 0)
            {
                if ((zHome = wxGetenv(_T("TMP"))) != NULL    ||
                    (zHome = wxGetenv(_T("TMPDIR"))) != NULL ||
                    (zHome = wxGetenv(_T("TEMP"))) != NULL)
                    delete[] wxBuffer;
                    return *zHome ? zHome : (wxChar*)_T("\\");
            }
            if (wxStricmp(zTmp, WXSTRINGCAST sUser1) == 0)
                sUser1 = _T("");
        }
    }
#endif
    if (sUser1 == _T(""))
    {
        if ((zHome = wxGetenv(_T("HOME"))) != NULL)
        {
            wxStrcpy(wxBuffer, zHome);
            wxUnix2DosFilename(wxBuffer);
            wxStrcpy(zHome, wxBuffer);
            delete[] wxBuffer;
            return zHome;
        }
    }
    delete[] wxBuffer;
    return NULL; // No home known!
}

bool wxDirExists(
  const wxString&                   rDir
)
{
    return (::DosSetCurrentDir(WXSTRINGCAST rDir));
}

wxString WXDLLEXPORT wxPMErrorToStr(
  ERRORID                           vError
)
{
    wxString                        sError;

    //
    // Remove the high order byte -- it is useless
    //
    vError &= 0x0000ffff;
    switch(vError)
    {
        case PMERR_INVALID_HWND:
            sError = wxT("Invalid window handle specified");
            break;

        case PMERR_INVALID_FLAG:
            sError = wxT("Invalid flag bit set");
            break;

        case PMERR_NO_MSG_QUEUE:
            sError = wxT("No message queue available");
            break;

        case PMERR_INVALID_PARM:
            sError = wxT("Parameter contained invalid data");
            break;

        case PMERR_INVALID_PARAMETERS:
            sError = wxT("Parameter value is out of range");
            break;

        case PMERR_PARAMETER_OUT_OF_RANGE:
            sError = wxT("Parameter value is out of range");
            break;

        case PMERR_INVALID_INTEGER_ATOM:
            sError = wxT("Not a valid atom");
            break;

        case PMERR_INVALID_HATOMTBL:
            sError = wxT("Atom table handle is invalid");
            break;

        case PMERR_INVALID_ATOM_NAME:
            sError = wxT("Not a valid atom name");
            break;

        case PMERR_ATOM_NAME_NOT_FOUND:
            sError = wxT("Valid name format, but cannot find name in atom table");
            break;

        default:
            sError = wxT("Unknown error");
    }
    return(sError);
} // end of wxPMErrorToStr

// replacement for implementation in unix/utilsunx.cpp,
// to be used by all X11 based ports.
struct wxEndProcessData;

void wxHandleProcessTermination(wxEndProcessData *proc_data)
{
    // For now, just do nothing. To be filled in as needed.
}
