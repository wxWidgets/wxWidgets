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
    #include "wx/cursor.h"
#endif  //WX_PRECOMP

#include "wx/os2/private.h"
#include "wx/timer.h"
#include "wx/intl.h"

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

static const wxChar WX_SECTION[] = _T("wxWindows");
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
        ULONG n = ::PrfQueryProfileString( HINI_PROFILE
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
    return *zBuf ? TRUE : FALSE;
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
    if (lrc == 0) return TRUE;
#endif
    return FALSE;
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
    return TRUE;
}

int wxKill(
  long                              lPid
, wxSignal                          eSig
, wxKillError*                      peError
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
    wxChar                          zTmp[255];
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
    return FALSE;
}

// Get free memory in bytes, or -1 if cannot determine amount (e.g. on UNIX)
long wxGetFreeMemory()
{
    void*                           pMemptr = NULL;
    ULONG                           lSize;
    ULONG                           lMemFlags;
    APIRET                          rc;

    lMemFlags = PAG_FREE;
    rc = ::DosQueryMem(pMemptr, &lSize, &lMemFlags);
    if (rc != 0)
        return -1L;
    return (long)lSize;
}

// ----------------------------------------------------------------------------
// env vars
// ----------------------------------------------------------------------------

bool wxGetEnv(const wxString& var, wxString *value)
{
    // wxGetenv is defined as getenv()
    wxChar *p = wxGetenv(var);
    if ( !p )
        return FALSE;

    if ( value )
    {
        *value = p;
    }

    return TRUE;
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
    return FALSE;
#endif
}


// Sleep for nSecs seconds. Attempt a Windows implementation using timers.
static bool inTimer = FALSE;

class wxSleepTimer: public wxTimer
{
public:
    inline void Notify()
    {
        inTimer = FALSE;
        Stop();
    }
};

static wxTimer*                     wxTheSleepTimer = NULL;

void wxUsleep(
  unsigned long                     ulMilliseconds
)
{
    ::DosSleep(ulMilliseconds/1000l);
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
    wxBuffer = new wxChar[256];
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

// Chris Breeze 27/5/98: revised WIN32 code to
// detect WindowsNT correctly
int wxGetOsVersion(
  int*                              pMajorVsn
, int*                              pMinorVsn
)
{
    ULONG                           ulSysInfo[QSV_MAX] = {0};
    APIRET                          ulrc;

    ulrc = ::DosQuerySysInfo( 1L
                             ,QSV_MAX
                             ,(PVOID)ulSysInfo
                             ,sizeof(ULONG) * QSV_MAX
                            );
    if (ulrc == 0L)
    {
        *pMajorVsn = ulSysInfo[QSV_VERSION_MAJOR];
        *pMajorVsn = *pMajorVsn/10;
        *pMinorVsn = ulSysInfo[QSV_VERSION_MINOR];
        return wxWINDOWS_OS2;
    }
    return wxWINDOWS; // error if we get here, return generic value
}

// Reading and writing resources (eg WIN.INI, .Xdefaults)
#if wxUSE_RESOURCES
bool wxWriteResource(
  const wxString&                   rSection
, const wxString&                   rEntry
, const wxString&                   rValue
, const wxString&                   rFile
)
{
    HAB                             hab = 0;
    HINI                            hIni = 0;

    if (rFile != "")
    {
        hIni = ::PrfOpenProfile(hab, (PSZ)WXSTRINGCAST rFile);
        if (hIni != 0L)
        {
            return (::PrfWriteProfileString( hIni
                                            ,(PSZ)WXSTRINGCAST rSection
                                            ,(PSZ)WXSTRINGCAST rEntry
                                            ,(PSZ)WXSTRINGCAST rValue
                                           ));
        }
    }
    else
        return (::PrfWriteProfileString( HINI_PROFILE
                                        ,(PSZ)WXSTRINGCAST rSection
                                        ,(PSZ)WXSTRINGCAST rEntry
                                        ,(PSZ)WXSTRINGCAST rValue
                                       ));
    return FALSE;
}

bool wxWriteResource(
  const wxString&                   rSection
, const wxString&                   rEntry
, float                             fValue
, const wxString&                   rFile
)
{
    wxChar                          zBuf[50];

    wxSprintf(zBuf, "%.4f", fValue);
    return wxWriteResource( rSection
                           ,rEntry
                           ,zBuf
                           ,rFile
                          );
}

bool wxWriteResource(
  const wxString&                   rSection
, const wxString&                   rEntry
, long                              lValue
, const wxString&                   rFile
)
{
    wxChar                          zBuf[50];

    wxSprintf(zBuf, "%ld", lValue);
    return wxWriteResource( rSection
                           ,rEntry
                           ,zBuf
                           ,rFile
                          );
}

bool wxWriteResource(
  const wxString&                   rSection
, const wxString&                   rEntry
, int                               lValue
, const wxString&                   rFile
)
{
    wxChar                          zBuf[50];

    wxSprintf(zBuf, "%d", lValue);
    return wxWriteResource( rSection
                           ,rEntry
                           ,zBuf
                           ,rFile
                          );
}

bool wxGetResource(
  const wxString&                   rSection
, const wxString&                   rEntry
, wxChar**                          ppValue
, const wxString&                   rFile
)
{
    HAB                             hab = 0;
    HINI                            hIni = 0;
    wxChar                          zDefunkt[] = _T("$$default");
    char                            zBuf[1000];

    if (rFile != "")
    {
        hIni = ::PrfOpenProfile(hab, (PSZ)WXSTRINGCAST rFile);
        if (hIni != 0L)
        {
            ULONG n = ::PrfQueryProfileString( hIni
                                              ,(PSZ)WXSTRINGCAST rSection
                                              ,(PSZ)WXSTRINGCAST rEntry
                                              ,(PSZ)zDefunkt
                                              ,(PVOID)zBuf
                                              ,1000
                                             );
            if (zBuf == NULL)
                return FALSE;
            if (n == 0L || wxStrcmp(zBuf, zDefunkt) == 0)
                return FALSE;
            zBuf[n-1] = '\0';
        }
        else
            return FALSE;
    }
    else
    {
        ULONG n = ::PrfQueryProfileString( HINI_PROFILE
                                          ,(PSZ)WXSTRINGCAST rSection
                                          ,(PSZ)WXSTRINGCAST rEntry
                                          ,(PSZ)zDefunkt
                                          ,(PVOID)zBuf
                                          ,1000
                                         );
        if (zBuf == NULL)
            return FALSE;
        if (n == 0L || wxStrcmp(zBuf, zDefunkt) == 0)
            return FALSE;
        zBuf[n-1] = '\0';
    }
    strcpy((char*)*ppValue, zBuf);
    return TRUE;
}

bool wxGetResource(
  const wxString&                   rSection
, const wxString&                   rEntry
, float*                            pValue
, const wxString&                   rFile
)
{
    wxChar*                         zStr = NULL;

    zStr = new wxChar[1000];
    bool                            bSucc = wxGetResource( rSection
                                                          ,rEntry
                                                          ,(wxChar **)&zStr
                                                          ,rFile
                                                         );

    if (bSucc)
    {
        *pValue = (float)wxStrtod(zStr, NULL);
        delete[] zStr;
        return TRUE;
    }
    else
    {
        delete[] zStr;
        return FALSE;
    }
}

bool wxGetResource(
  const wxString&                   rSection
, const wxString&                   rEntry
, long*                             pValue
, const wxString&                   rFile
)
{
    wxChar*                           zStr = NULL;

    zStr = new wxChar[1000];
    bool                              bSucc = wxGetResource( rSection
                                                            ,rEntry
                                                            ,(wxChar **)&zStr
                                                            ,rFile
                                                           );

    if (bSucc)
    {
        *pValue = wxStrtol(zStr, NULL, 10);
        delete[] zStr;
        return TRUE;
    }
    else
    {
        delete[] zStr;
        return FALSE;
    }
}

bool wxGetResource(
  const wxString&                   rSection
, const wxString&                   rEntry
, int*                              pValue
, const wxString&                   rFile
)
{
    wxChar*                         zStr = NULL;

    zStr = new wxChar[1000];
    bool                            bSucc = wxGetResource( rSection
                                                          ,rEntry
                                                          ,(wxChar **)&zStr
                                                          ,rFile
                                                         );

    if (bSucc)
    {
        *pValue = (int)wxStrtol(zStr, NULL, 10);
        delete[] zStr;
        return TRUE;
    }
    else
    {
        delete[] zStr;
        return FALSE;
    }
}
#endif // wxUSE_RESOURCES

// ---------------------------------------------------------------------------
// helper functions for showing a "busy" cursor
// ---------------------------------------------------------------------------

HCURSOR gs_wxBusyCursor = 0;     // new, busy cursor
HCURSOR gs_wxBusyCursorOld = 0;  // old cursor
static int gs_wxBusyCursorCount = 0;

// Set the cursor to the busy cursor for all windows
void wxBeginBusyCursor(
  wxCursor*                         pCursor
)
{
    if ( gs_wxBusyCursorCount++ == 0 )
    {
        gs_wxBusyCursor = (HCURSOR)pCursor->GetHCURSOR();
        ::WinSetPointer(HWND_DESKTOP, (HPOINTER)gs_wxBusyCursor);
    }
    //else: nothing to do, already set
}

// Restore cursor to normal
void wxEndBusyCursor()
{
    wxCHECK_RET( gs_wxBusyCursorCount > 0
                ,_T("no matching wxBeginBusyCursor() for wxEndBusyCursor()")
               );

    if (--gs_wxBusyCursorCount == 0)
    {
        ::WinSetPointer(HWND_DESKTOP, (HPOINTER)gs_wxBusyCursorOld);
        gs_wxBusyCursorOld = 0;
    }
}

// TRUE if we're between the above two calls
bool wxIsBusy()
{
    return (gs_wxBusyCursorCount > 0);
}

// ---------------------------------------------------------------------------
const wxChar* wxGetHomeDir(
  wxString*                         pStr
)
{
    wxString&                       rStrDir = *pStr;

    // OS/2 has no idea about home,
    // so use the working directory instead?

    // 256 was taken from os2def.h
#ifndef MAX_PATH
#  define MAX_PATH  256
#endif

    char                            zDirName[256];
    ULONG                           ulDirLen;

    ::DosQueryCurrentDir(0, zDirName, &ulDirLen);
    rStrDir = zDirName;
    return rStrDir.c_str();
}

// Hack for OS/2
wxChar* wxGetUserHome (
  const wxString&                   rUser
)
{
    wxChar*                         zHome;
    wxString                        sUser1(rUser);

    wxBuffer = new wxChar[256];
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

// Check whether this window wants to process messages, e.g. Stop button
// in long calculations.
bool wxCheckForInterrupt(
  wxWindow*                         pWnd
)
{
    if(pWnd)
    {
        QMSG                        vMsg;
        HAB                         hab = 0;
        HWND                        hwndFilter = NULLHANDLE;
        HWND                        hwndWin= (HWND) pWnd->GetHWND();

        while(::WinPeekMsg(hab, &vMsg, hwndFilter, 0, 0, PM_REMOVE))
        {
            ::WinDispatchMsg(hab, &vMsg);
        }
        return TRUE;//*** temporary?
    }
    else
    {
        wxFAIL_MSG(_T("pWnd==NULL !!!"));
        return FALSE;//*** temporary?
    }
}

void wxGetMousePosition(
  int*                              pX
, int*                              pY
)
{
    POINTL                          vPt;

    ::WinQueryPointerPos(HWND_DESKTOP, &vPt);
    *pX = vPt.x;
    *pY = vPt.y;
};

// Return TRUE if we have a colour display
bool wxColourDisplay()
{
#if 0
    HPS                             hpsScreen;
    HDC                             hdcScreen;
    LONG                            lColors;

    hpsScreen = ::WinGetScreenPS(HWND_DESKTOP);
    hdcScreen = ::GpiQueryDevice(hpsScreen);
    ::DevQueryCaps(hdcScreen, CAPS_COLORS, 1L, &lColors);
    return(lColors > 1L);
#else
    // I don't see how the PM display could not be color. Besides, this
    // was leaking DCs and PSs!!!  MN
    return TRUE;
#endif
}

// Returns depth of screen
int wxDisplayDepth()
{
    HPS                             hpsScreen;
    HDC                             hdcScreen;
    LONG                            lPlanes;
    LONG                            lBitsPerPixel;
    static LONG                     nDepth = 0;

    // The screen colordepth ain't gonna change. No reason to query
    // it over and over!
    if (!nDepth) {
        hpsScreen = ::WinGetScreenPS(HWND_DESKTOP);
        hdcScreen = ::GpiQueryDevice(hpsScreen);
        ::DevQueryCaps(hdcScreen, CAPS_COLOR_PLANES, 1L, &lPlanes);
        ::DevQueryCaps(hdcScreen, CAPS_COLOR_BITCOUNT, 1L, &lBitsPerPixel);

        nDepth = (int)(lPlanes * lBitsPerPixel);
        ::DevCloseDC(hdcScreen);
        ::WinReleasePS(hpsScreen);
    }
    return (nDepth);
}

// Get size of display
void wxDisplaySize(
  int*                              pWidth
, int*                              pHeight
)
{
    HPS                             hpsScreen;
    HDC                             hdcScreen;
    static LONG                     lWidth  = 0;
    static LONG                     lHeight = 0;

    // The screen size ain't gonna change either so just cache the values
    if (!lWidth) {
        hpsScreen = ::WinGetScreenPS(HWND_DESKTOP);
        hdcScreen = ::GpiQueryDevice(hpsScreen);
        ::DevQueryCaps(hdcScreen, CAPS_WIDTH, 1L, &lWidth);
        ::DevQueryCaps(hdcScreen, CAPS_HEIGHT, 1L, &lHeight);
        ::DevCloseDC(hdcScreen);
        ::WinReleasePS(hpsScreen);
    }
    *pWidth = (int)lWidth;
    *pHeight = (int)lHeight;
}

void wxDisplaySizeMM(
  int*                              pWidth
, int*                              pHeight
)
{
    HPS                             hpsScreen;
    HDC                             hdcScreen;

    hpsScreen = ::WinGetScreenPS(HWND_DESKTOP);
    hdcScreen = ::GpiQueryDevice(hpsScreen);

    if (pWidth)
        ::DevQueryCaps( hdcScreen
                       ,CAPS_HORIZONTAL_RESOLUTION
                       ,1L
                       ,(PLONG)pWidth
                      );
    if (pHeight)
        ::DevQueryCaps( hdcScreen
                       ,CAPS_VERTICAL_RESOLUTION
                       ,1L
                       ,(PLONG)pHeight
                      );
    ::DevCloseDC(hdcScreen);
    ::WinReleasePS(hpsScreen);
}

void wxClientDisplayRect(int *x, int *y, int *width, int *height)
{
    // This is supposed to return desktop dimensions minus any window
    // manager panels, menus, taskbars, etc.  If there is a way to do that
    // for this platform please fix this function, otherwise it defaults
    // to the entire desktop.
    if (x) *x = 0;
    if (y) *y = 0;
    wxDisplaySize(width, height);
}


bool wxDirExists(
  const wxString&                   rDir
)
{
    return (::DosSetCurrentDir(WXSTRINGCAST rDir));
}

// ---------------------------------------------------------------------------
// window information functions
// ---------------------------------------------------------------------------

wxString WXDLLEXPORT wxGetWindowText(
  WXHWND                            hWnd
)
{
    wxString                        vStr;
    long                            lLen = ::WinQueryWindowTextLength((HWND)hWnd) + 1;

    ::WinQueryWindowText((HWND)hWnd, lLen, vStr.GetWriteBuf((int)lLen));
    vStr.UngetWriteBuf();

    return vStr;
}

wxString WXDLLEXPORT wxGetWindowClass(
  WXHWND                            hWnd
)
{
    wxString                        vStr;
    int                             nLen = 256; // some starting value

    for ( ;; )
    {
        int                         nCount = ::WinQueryClassName((HWND)hWnd, nLen, vStr.GetWriteBuf(nLen));

        vStr.UngetWriteBuf();
        if (nCount == nLen )
        {
            // the class name might have been truncated, retry with larger
            // buffer
            nLen *= 2;
        }
        else
        {
            break;
        }
    }
    return vStr;
}

WXWORD WXDLLEXPORT wxGetWindowId(
  WXHWND                            hWnd
)
{
    return ::WinQueryWindowUShort((HWND)hWnd, QWS_ID);
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

void wxDrawBorder(
  HPS                               hPS
, RECTL&                            rRect
, WXDWORD                           dwStyle
)
{
    POINTL                          vPoint[2];

    vPoint[0].x = rRect.xLeft;
    vPoint[0].y = rRect.yBottom;
    ::GpiMove(hPS, &vPoint[0]);
    if (dwStyle & wxSIMPLE_BORDER ||
        dwStyle & wxSTATIC_BORDER)
    {
        vPoint[1].x = rRect.xRight - 1;
        vPoint[1].y = rRect.yTop - 1;
        ::GpiBox( hPS
                 ,DRO_OUTLINE
                 ,&vPoint[1]
                 ,0L
                 ,0L
                );
    }
    if (dwStyle & wxSUNKEN_BORDER)
    {
        LINEBUNDLE                      vLineBundle;

        vLineBundle.lColor     = 0x00FFFFFF; // WHITE
        vLineBundle.usMixMode  = FM_OVERPAINT;
        vLineBundle.fxWidth    = 2;
        vLineBundle.lGeomWidth = 2;
        vLineBundle.usType     = LINETYPE_SOLID;
        vLineBundle.usEnd      = 0;
        vLineBundle.usJoin     = 0;
        ::GpiSetAttrs( hPS
                      ,PRIM_LINE
                      ,LBB_COLOR | LBB_MIX_MODE | LBB_WIDTH | LBB_GEOM_WIDTH | LBB_TYPE
                      ,0L
                      ,&vLineBundle
                     );
        vPoint[1].x = rRect.xRight - 1;
        vPoint[1].y = rRect.yTop - 1;
        ::GpiBox( hPS
                 ,DRO_OUTLINE
                 ,&vPoint[1]
                 ,0L
                 ,0L
                );
       vPoint[0].x = rRect.xLeft + 1;
       vPoint[0].y = rRect.yBottom + 1;
       ::GpiMove(hPS, &vPoint[0]);
        vPoint[1].x = rRect.xRight - 2;
        vPoint[1].y = rRect.yTop - 2;
        ::GpiBox( hPS
                 ,DRO_OUTLINE
                 ,&vPoint[1]
                 ,0L
                 ,0L
                );

        vLineBundle.lColor     = 0x00000000; // BLACK
        vLineBundle.usMixMode  = FM_OVERPAINT;
        vLineBundle.fxWidth    = 2;
        vLineBundle.lGeomWidth = 2;
        vLineBundle.usType     = LINETYPE_SOLID;
        vLineBundle.usEnd      = 0;
        vLineBundle.usJoin     = 0;
        ::GpiSetAttrs( hPS
                      ,PRIM_LINE
                      ,LBB_COLOR | LBB_MIX_MODE | LBB_WIDTH | LBB_GEOM_WIDTH | LBB_TYPE
                      ,0L
                      ,&vLineBundle
                     );
        vPoint[0].x = rRect.xLeft + 2;
        vPoint[0].y = rRect.yBottom + 2;
        ::GpiMove(hPS, &vPoint[0]);
        vPoint[1].x = rRect.xLeft + 2;
        vPoint[1].y = rRect.yTop - 3;
        ::GpiLine(hPS, &vPoint[1]);
        vPoint[1].x = rRect.xRight - 3;
        vPoint[1].y = rRect.yTop - 3;
        ::GpiLine(hPS, &vPoint[1]);

        vPoint[0].x = rRect.xLeft + 3;
        vPoint[0].y = rRect.yBottom + 3;
        ::GpiMove(hPS, &vPoint[0]);
        vPoint[1].x = rRect.xLeft + 3;
        vPoint[1].y = rRect.yTop - 4;
        ::GpiLine(hPS, &vPoint[1]);
        vPoint[1].x = rRect.xRight - 4;
        vPoint[1].y = rRect.yTop - 4;
        ::GpiLine(hPS, &vPoint[1]);
    }
    if (dwStyle & wxDOUBLE_BORDER)
    {
        LINEBUNDLE                      vLineBundle;

        vLineBundle.lColor     = 0x00FFFFFF; // WHITE
        vLineBundle.usMixMode  = FM_OVERPAINT;
        vLineBundle.fxWidth    = 2;
        vLineBundle.lGeomWidth = 2;
        vLineBundle.usType     = LINETYPE_SOLID;
        vLineBundle.usEnd      = 0;
        vLineBundle.usJoin     = 0;
        ::GpiSetAttrs( hPS
                      ,PRIM_LINE
                      ,LBB_COLOR | LBB_MIX_MODE | LBB_WIDTH | LBB_GEOM_WIDTH | LBB_TYPE
                      ,0L
                      ,&vLineBundle
                     );
        vPoint[1].x = rRect.xRight - 1;
        vPoint[1].y = rRect.yTop - 1;
        ::GpiBox( hPS
                 ,DRO_OUTLINE
                 ,&vPoint[1]
                 ,0L
                 ,0L
                );
        vLineBundle.lColor     = 0x00000000; // WHITE
        vLineBundle.usMixMode  = FM_OVERPAINT;
        vLineBundle.fxWidth    = 2;
        vLineBundle.lGeomWidth = 2;
        vLineBundle.usType     = LINETYPE_SOLID;
        vLineBundle.usEnd      = 0;
        vLineBundle.usJoin     = 0;
        ::GpiSetAttrs( hPS
                      ,PRIM_LINE
                      ,LBB_COLOR | LBB_MIX_MODE | LBB_WIDTH | LBB_GEOM_WIDTH | LBB_TYPE
                      ,0L
                      ,&vLineBundle
                     );
        vPoint[0].x = rRect.xLeft + 2;
        vPoint[0].y = rRect.yBottom + 2;
        ::GpiMove(hPS, &vPoint[0]);
        vPoint[1].x = rRect.xRight - 2;
        vPoint[1].y = rRect.yTop - 2;
        ::GpiBox( hPS
                 ,DRO_OUTLINE
                 ,&vPoint[1]
                 ,0L
                 ,0L
                );
        vLineBundle.lColor     = 0x00FFFFFF; // BLACK
        vLineBundle.usMixMode  = FM_OVERPAINT;
        vLineBundle.fxWidth    = 2;
        vLineBundle.lGeomWidth = 2;
        vLineBundle.usType     = LINETYPE_SOLID;
        vLineBundle.usEnd      = 0;
        vLineBundle.usJoin     = 0;
        ::GpiSetAttrs( hPS
                      ,PRIM_LINE
                      ,LBB_COLOR | LBB_MIX_MODE | LBB_WIDTH | LBB_GEOM_WIDTH | LBB_TYPE
                      ,0L
                      ,&vLineBundle
                     );
        vPoint[0].x = rRect.xLeft + 3;
        vPoint[0].y = rRect.yBottom + 3;
        ::GpiMove(hPS, &vPoint[0]);
        vPoint[1].x = rRect.xRight - 3;
        vPoint[1].y = rRect.yTop - 3;
        ::GpiBox( hPS
                 ,DRO_OUTLINE
                 ,&vPoint[1]
                 ,0L
                 ,0L
                );
    }
    if (dwStyle & wxRAISED_BORDER)
    {
        LINEBUNDLE                      vLineBundle;

        vLineBundle.lColor     = 0x00000000; // BLACK
        vLineBundle.usMixMode  = FM_OVERPAINT;
        vLineBundle.fxWidth    = 2;
        vLineBundle.lGeomWidth = 2;
        vLineBundle.usType     = LINETYPE_SOLID;
        vLineBundle.usEnd      = 0;
        vLineBundle.usJoin     = 0;
        ::GpiSetAttrs( hPS
                      ,PRIM_LINE
                      ,LBB_COLOR | LBB_MIX_MODE | LBB_WIDTH | LBB_GEOM_WIDTH | LBB_TYPE
                      ,0L
                      ,&vLineBundle
                     );
        vPoint[1].x = rRect.xRight - 1;
        vPoint[1].y = rRect.yTop - 1;
        ::GpiBox( hPS
                 ,DRO_OUTLINE
                 ,&vPoint[1]
                 ,0L
                 ,0L
                );
       vPoint[0].x = rRect.xLeft + 1;
       vPoint[0].y = rRect.yBottom + 1;
       ::GpiMove(hPS, &vPoint[0]);
        vPoint[1].x = rRect.xRight - 2;
        vPoint[1].y = rRect.yTop - 2;
        ::GpiBox( hPS
                 ,DRO_OUTLINE
                 ,&vPoint[1]
                 ,0L
                 ,0L
                );

        vLineBundle.lColor     = 0x00FFFFFF; // WHITE
        vLineBundle.usMixMode  = FM_OVERPAINT;
        vLineBundle.fxWidth    = 2;
        vLineBundle.lGeomWidth = 2;
        vLineBundle.usType     = LINETYPE_SOLID;
        vLineBundle.usEnd      = 0;
        vLineBundle.usJoin     = 0;
        ::GpiSetAttrs( hPS
                      ,PRIM_LINE
                      ,LBB_COLOR | LBB_MIX_MODE | LBB_WIDTH | LBB_GEOM_WIDTH | LBB_TYPE
                      ,0L
                      ,&vLineBundle
                     );
        vPoint[0].x = rRect.xLeft + 2;
        vPoint[0].y = rRect.yBottom + 2;
        ::GpiMove(hPS, &vPoint[0]);
        vPoint[1].x = rRect.xLeft + 2;
        vPoint[1].y = rRect.yTop - 3;
        ::GpiLine(hPS, &vPoint[1]);
        vPoint[1].x = rRect.xRight - 3;
        vPoint[1].y = rRect.yTop - 3;
        ::GpiLine(hPS, &vPoint[1]);

        vPoint[0].x = rRect.xLeft + 3;
        vPoint[0].y = rRect.yBottom + 3;
        ::GpiMove(hPS, &vPoint[0]);
        vPoint[1].x = rRect.xLeft + 3;
        vPoint[1].y = rRect.yTop - 4;
        ::GpiLine(hPS, &vPoint[1]);
        vPoint[1].x = rRect.xRight - 4;
        vPoint[1].y = rRect.yTop - 4;
        ::GpiLine(hPS, &vPoint[1]);
    }
} // end of wxDrawBorder

void wxOS2SetFont(
  HWND                              hWnd
, const wxFont&                     rFont
)
{
    char                            zFont[128];
    char                            zFacename[30];
    char                            zWeight[30];
    char                            zStyle[30];

    if (hWnd == NULLHANDLE)
        return;

    //
    // The fonts available for Presentation Params are just a few
    // outline fonts, the rest are available to the GPI, so we must
    // map the families to one of these three
    //
    switch(rFont.GetFamily())
    {
        case wxSCRIPT:
            strcpy(zFacename, "Script");
            break;

        case wxDECORATIVE:
            strcpy(zFacename, "WarpSans");
            break;

        case wxROMAN:
            strcpy(zFacename,"Times New Roman");
            break;

        case wxTELETYPE:
            strcpy(zFacename, "Courier New");
            break;

        case wxMODERN:
            strcpy(zFacename, "Courier New");
            break;

        case wxDEFAULT:
        default:
        case wxSWISS:
            strcpy(zFacename, "Helvetica");
            break;
    }

    switch(rFont.GetWeight())
    {
        default:
        case wxNORMAL:
        case wxLIGHT:
            zWeight[0] = '\0';
            break;

        case wxBOLD:
        case wxFONTWEIGHT_MAX:
            strcpy(zWeight, "Bold");
            break;
    }

    switch(rFont.GetStyle())
    {
        case wxITALIC:
        case wxSLANT:
            strcpy(zStyle, "Italic");
            break;

        default:
            zStyle[0] = '\0';
            break;
    }
    sprintf(zFont, "%d.%s", rFont.GetPointSize(), zFacename);
    if (zWeight[0] != '\0')
    {
        strcat(zFont, " ");
        strcat(zFont, zWeight);
    }
    if (zStyle[0] != '\0')
    {
        strcat(zFont, " ");
        strcat(zFont, zStyle);
    }
    ::WinSetPresParam(hWnd, PP_FONTNAMESIZE, strlen(zFont) + 1, (PVOID)zFont);
} // end of wxOS2SetFont

// ---------------------------------------------------------------------------
// Helper for taking a regular bitmap and giving it a disabled look
// ---------------------------------------------------------------------------
wxBitmap wxDisableBitmap(
  const wxBitmap&                   rBmp
, long                              lColor
)
{
    wxMask*                         pMask = rBmp.GetMask();

    if (!pMask)
        return(wxNullBitmap);

    DEVOPENSTRUC                    vDop  = {0L, "DISPLAY", NULL, 0L, 0L, 0L, 0L, 0L, 0L};
    SIZEL                           vSize = {0, 0};
    HDC                             hDC   = ::DevOpenDC(vHabmain, OD_MEMORY, "*", 5L, (PDEVOPENDATA)&vDop, NULLHANDLE);
    HPS                             hPS   = ::GpiCreatePS(vHabmain, hDC, &vSize, PU_PELS | GPIA_ASSOC);
    BITMAPINFOHEADER2               vHeader;
    BITMAPINFO2                     vInfo;
    ERRORID                         vError;
    wxString                        sError;
    HBITMAP                         hBitmap =  (HBITMAP)rBmp.GetHBITMAP();
    HBITMAP                         hOldBitmap = NULLHANDLE;
    HBITMAP                         hOldMask   = NULLHANDLE;
    HBITMAP                         hMask = (HBITMAP)rBmp.GetMask()->GetMaskBitmap();
    unsigned char*                  pucBits;     // buffer that will contain the bitmap data
    unsigned char*                  pucData;     // pointer to use to traverse bitmap data
    unsigned char*                  pucBitsMask; // buffer that will contain the mask data
    unsigned char*                  pucDataMask; // pointer to use to traverse mask data
    LONG                            lScans = 0L;
    LONG                            lScansSet = 0L;
    bool                            bpp16 = (wxDisplayDepth() == 16);

    memset(&vHeader, '\0', 16);
    vHeader.cbFix           = 16;

    memset(&vInfo, '\0', 16);
    vInfo.cbFix           = 16;
    vInfo.cx              = (ULONG)rBmp.GetWidth();
    vInfo.cy              = (ULONG)rBmp.GetHeight();
    vInfo.cPlanes         = 1;
    vInfo.cBitCount       = 24; // Set to desired count going in

    //
    // Create the buffers for data....all wxBitmaps are 24 bit internally
    //
    int                             nBytesPerLine = rBmp.GetWidth() * 3;
    int                             nSizeDWORD    = sizeof(DWORD);
    int                             nLineBoundary = nBytesPerLine % nSizeDWORD;
    int                             nPadding = 0;
    int                             i;
    int                             j;

    //
    // Bitmap must be ina double-word alligned address so we may
    // have some padding to worry about
    //
    if (nLineBoundary > 0)
    {
        nPadding     = nSizeDWORD - nLineBoundary;
        nBytesPerLine += nPadding;
    }
    pucBits = (unsigned char *)malloc(nBytesPerLine * rBmp.GetHeight());
    memset(pucBits, '\0', (nBytesPerLine * rBmp.GetHeight()));
    pucBitsMask = (unsigned char *)malloc(nBytesPerLine * rBmp.GetHeight());
    memset(pucBitsMask, '\0', (nBytesPerLine * rBmp.GetHeight()));

    //
    // Extract the bitmap and mask data
    //
    if ((hOldBitmap = ::GpiSetBitmap(hPS, hBitmap)) == HBM_ERROR)
    {
        vError = ::WinGetLastError(vHabmain);
        sError = wxPMErrorToStr(vError);
    }
    ::GpiQueryBitmapInfoHeader(hBitmap, &vHeader);
    vInfo.cBitCount = 24;
    if ((lScans = ::GpiQueryBitmapBits( hPS
                                       ,0L
                                       ,(LONG)rBmp.GetHeight()
                                       ,(PBYTE)pucBits
                                       ,&vInfo
                                      )) == GPI_ALTERROR)
    {
        vError = ::WinGetLastError(vHabmain);
        sError = wxPMErrorToStr(vError);
    }
    if ((hOldMask = ::GpiSetBitmap(hPS, hMask)) == HBM_ERROR)
    {
        vError = ::WinGetLastError(vHabmain);
        sError = wxPMErrorToStr(vError);
    }
    ::GpiQueryBitmapInfoHeader(hMask, &vHeader);
    vInfo.cBitCount = 24;
    if ((lScans = ::GpiQueryBitmapBits( hPS
                                       ,0L
                                       ,(LONG)rBmp.GetHeight()
                                       ,(PBYTE)pucBitsMask
                                       ,&vInfo
                                      )) == GPI_ALTERROR)
    {
        vError = ::WinGetLastError(vHabmain);
        sError = wxPMErrorToStr(vError);
    }
    if (( hMask = ::GpiSetBitmap(hPS, hOldMask)) == HBM_ERROR)
    {
        vError = ::WinGetLastError(vHabmain);
        sError = wxPMErrorToStr(vError);
    }
    pucData     = pucBits;
    pucDataMask = pucBitsMask;

    //
    // Get the mask value
    //
    for (i = 0; i < rBmp.GetHeight(); i++)
    {
        for (j = 0; j < rBmp.GetWidth(); j++)
        {
            // Byte 1
            if (bpp16 && *pucDataMask == 0xF8) // 16 bit display gobblygook
            {
                *pucData = 0x7F;
                pucData++;
            }
            else if (*pucDataMask == 0xFF) // set to grey
            {
                *pucData = 0x7F;
                pucData++;
            }
            else
            {
                *pucData = ((unsigned char)(lColor >> 16));
                pucData++;
            }

            // Byte 2
            if (bpp16 && *(pucDataMask + 1) == 0xFC) // 16 bit display gobblygook
            {
                *pucData = 0x7F;
                pucData++;
            }
            else if (*(pucDataMask + 1) == 0xFF) // set to grey
            {
                *pucData = 0x7F;
                pucData++;
            }
            else
            {
                *pucData = ((unsigned char)(lColor >> 8));
                pucData++;
            }

            // Byte 3
            if (bpp16 && *(pucDataMask + 2) == 0xF8) // 16 bit display gobblygook
            {
                *pucData = 0x7F;
                pucData++;
            }
            else if (*(pucDataMask + 2) == 0xFF) // set to grey
            {
                *pucData = 0x7F;
                pucData++;
            }
            else
            {
                *pucData = ((unsigned char)lColor);
                pucData++;
            }
            pucDataMask += 3;
        }
        for (j = 0; j < nPadding; j++)
        {
            pucData++;
            pucDataMask++;
        }
    }

    //
    // Create a new bitmap and set the modified bits
    //
    wxBitmap                        vNewBmp( rBmp.GetWidth()
                                            ,rBmp.GetHeight()
                                            ,24
                                           );
    HBITMAP                         hNewBmp = (HBITMAP)vNewBmp.GetHBITMAP();

    if ((hOldBitmap = ::GpiSetBitmap(hPS, hNewBmp)) == HBM_ERROR)
    {
        vError = ::WinGetLastError(vHabmain);
        sError = wxPMErrorToStr(vError);
    }
    if ((lScansSet = ::GpiSetBitmapBits( hPS
                                        ,0L
                                        ,(LONG)rBmp.GetHeight()
                                        ,(PBYTE)pucBits
                                        ,&vInfo
                                       )) == GPI_ALTERROR)

    {
        vError = ::WinGetLastError(vHabmain);
        sError = wxPMErrorToStr(vError);
    }
    wxMask*                         pNewMask;

    pNewMask = new wxMask(pMask->GetMaskBitmap());
    vNewBmp.SetMask(pNewMask);
    free(pucBits);
    ::GpiSetBitmap(hPS, NULLHANDLE);
    ::GpiDestroyPS(hPS);
    ::DevCloseDC(hDC);
    if (vNewBmp.Ok())
        return(vNewBmp);
    return(wxNullBitmap);
} // end of wxDisableBitmap

COLORREF wxColourToRGB(
  const wxColour&                   rColor
)
{
    return(OS2RGB(rColor.Red(), rColor.Green(), rColor.Blue()));
} // end of wxColourToRGB
