/////////////////////////////////////////////////////////////////////////////
// Name:        utils.cpp
// Purpose:     Various utilities
// Author:      David Webster
// Modified by:
// Created:     09/17/99
// RCS-ID:      $Id$
// Copyright:   (c) David Webster
// Licence:     wxWindows license
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
#else
#include <direct.h>
#endif

#include "wx/log.h"

#include <io.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>

#define INCL_DOS
#define INCL_PM
#define INCL_GPI
#include <os2.h>
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
#ifndef __EMX__
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
, int                               nSig
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
    ::DosSleep(ulMilliseconds);
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
            Unix2DosFilename(wxBuffer);
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
    HPS                             hpsScreen;
    HDC                             hdcScreen;
    LONG                            lColors;

    hpsScreen = ::WinGetScreenPS(HWND_DESKTOP);
    hdcScreen = ::GpiQueryDevice(hpsScreen);
    ::DevQueryCaps(hdcScreen, CAPS_COLORS, 1L, &lColors);
    return(lColors > 1L);
}

// Returns depth of screen
int wxDisplayDepth()
{
    HPS                             hpsScreen;
    HDC                             hdcScreen;
    LONG                            lPlanes;
    LONG                            lBitsPerPixel;
    LONG                            nDepth;

    hpsScreen = ::WinGetScreenPS(HWND_DESKTOP);
    hdcScreen = ::GpiQueryDevice(hpsScreen);
    ::DevQueryCaps(hdcScreen, CAPS_COLOR_PLANES, 1L, &lPlanes);
    ::DevQueryCaps(hdcScreen, CAPS_COLOR_BITCOUNT, 1L, &lBitsPerPixel);

    nDepth = (int)(lPlanes * lBitsPerPixel);
    DevCloseDC(hdcScreen);
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
    LONG                            lWidth;
    LONG                            lHeight;

    hpsScreen = ::WinGetScreenPS(HWND_DESKTOP);
    hdcScreen = ::GpiQueryDevice(hpsScreen);
    ::DevQueryCaps(hdcScreen, CAPS_WIDTH, 1L, &lWidth);
    ::DevQueryCaps(hdcScreen, CAPS_HEIGHT, 1L, &lHeight);
    DevCloseDC(hdcScreen);
    *pWidth = (int)lWidth;
    *pHeight = (int)lHeight;
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

