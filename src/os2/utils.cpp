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
#include <direct.h>

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
#include<netdb.h>
#define PURE_32
#include <upm.h>
#include <netcons.h>
#include <netbios.h>

// In the WIN.INI file
static const wxChar WX_SECTION[] = _T("wxWindows");
static const wxChar eHOSTNAME[]  = _T("HostName");
static const wxChar eUSERID[]    = _T("UserId");
static const wxChar eUSERNAME[]  = _T("UserName");

// For the following functions we SHOULD fill in support
// for Windows-NT (which I don't know) as I assume it begin
// a POSIX Unix (so claims MS) that it has some special
// functions beyond those provided by WinSock

// Get full hostname (eg. DoDo.BSn-Germany.crg.de)
bool wxGetHostName(wxChar *buf, int maxSize)
{
#if wxUSE_NET_API
  char                               server[256];
  char                               computer[256];
  unsigned long                      ulLevel;
  unsigned char*                     pbBuffer;
  unsigned long                      ulBuffer;
  unsigned long*                     pulTotalAvail;

  NetBios32GetInfo( (const unsigned char*)server
                   ,(const unsigned char*)computer
                   ,ulLevel
                   ,pbBuffer
                   ,ulBuffer
                   ,pulTotalAvail
                  );
  strcpy(buf, server);
#else
  wxChar *sysname;
  const wxChar *default_host = _T("noname");

  if ((sysname = wxGetenv(_T("SYSTEM_NAME"))) == NULL)
  {
      // GetProfileString(WX_SECTION, eHOSTNAME, default_host, buf, maxSize - 1);
  }
  else
      wxStrncpy(buf, sysname, maxSize - 1);
  buf[maxSize] = _T('\0');
#endif
  return *buf ? TRUE : FALSE;
}

// Get user ID e.g. jacs
bool wxGetUserId(wxChar *buf, int maxSize)
{
  return(U32ELOCL((unsigned char*)buf, (unsigned long *)&maxSize));
}

bool wxGetUserName(wxChar *buf, int maxSize)
{
#ifdef USE_NET_API
    wxGetUserId(buf, maxSize);
#else
    wxStrncpy(buf, _T("Unknown User"), maxSize);
#endif
  return TRUE;
}

int wxKill(long pid, int sig)
{
  return 0;
}

//
// Execute a program in an Interactive Shell
//
bool wxShell(const wxString& command)
{
  wxChar *shell;
  if ((shell = wxGetenv(_T("COMSPEC"))) == NULL)
    shell = _T("\\CMD.EXE");

  wxChar tmp[255];
  if (command != "")
    wxSprintf(tmp, "%s /c %s", shell, WXSTRINGCAST command);
  else
    wxStrcpy(tmp, shell);

  return (wxExecute((wxChar *)tmp, FALSE) != 0);
}

// Get free memory in bytes, or -1 if cannot determine amount (e.g. on UNIX)
long wxGetFreeMemory(void *memptr)
{
    ULONG                           lSize;
    ULONG                           lMemFlags;
    APIRET                          rc;

    lMemFlags = PAG_FREE;
    rc = ::DosQueryMem(memptr, &lSize, &lMemFlags);
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

static wxTimer *wxTheSleepTimer = NULL;

void wxUsleep(unsigned long milliseconds)
{
    ::DosSleep(milliseconds);
}

void wxSleep(int nSecs)
{
    ::DosSleep( 1000*nSecs );
}

// Consume all events until no more left
void wxFlushEvents()
{
//  wxYield();
}

// Output a debug mess., in a system dependent fashion.
void wxDebugMsg(const wxChar *fmt ...)
{
  va_list ap;
  static wxChar buffer[512];

  if (!wxTheApp->GetWantDebugOutput())
    return ;

  va_start(ap, fmt);

   sprintf(buffer,fmt,ap) ;

  va_end(ap);
}

// Non-fatal error: pop up message box and (possibly) continue
void wxError(const wxString& msg, const wxString& title)
{
  wxSprintf(wxBuffer, "%s\nContinue?", WXSTRINGCAST msg);
  if (::WinMessageBox( HWND_DESKTOP
                      ,NULL
                      ,(PSZ)wxBuffer
                      ,(PSZ)WXSTRINGCAST title
                      ,0
                      ,MB_ICONEXCLAMATION | MB_YESNO
                     ) == MBID_YES)
    wxExit();
}

// Fatal error: pop up message box and abort
void wxFatalError(const wxString& rMsg, const wxString& rTitle)
{
    unsigned long                   rc;

    rc = ::WinMessageBox( HWND_DESKTOP
                         ,NULL
                         ,WXSTRINGCAST rMsg
                         ,WXSTRINGCAST rTitle
                         ,0
                         ,MB_NOICON | MB_OK
                        );
    DosExit(EXIT_PROCESS, rc);
}

// Emit a beeeeeep
void wxBell()
{
    DosBeep(1000,1000); // 1kHz during 1 sec.
}

// Chris Breeze 27/5/98: revised WIN32 code to
// detect WindowsNT correctly
int wxGetOsVersion(int *majorVsn, int *minorVsn)
{
  ULONG                 aulSysInfo[QSV_MAX] = {0};

  if (DosQuerySysInfo( 1L
                      ,QSV_MAX
                      ,(PVOID)aulSysInfo
                      ,sizeof(ULONG) * QSV_MAX
                     ))
  {
     *majorVsn = aulSysInfo[QSV_VERSION_MAJOR];
     *minorVsn = aulSysInfo[QSV_VERSION_MINOR];
     return wxWINDOWS_OS2;
  }
  return wxWINDOWS; // error if we get here, return generic value
}

// Reading and writing resources (eg WIN.INI, .Xdefaults)
// TODO: Ability to read and write to an INI file

#if wxUSE_RESOURCES
bool wxWriteResource(const wxString& section, const wxString& entry, const wxString& value, const wxString& file)
{
    HAB                             hab;
    HINI                            hIni;

    if (file != "")
    {
        hIni = ::PrfOpenProfile(hab, (PSZ)WXSTRINGCAST file);
        if (hIni != 0L)
        {
            return (::PrfWriteProfileString( hIni
                                            ,(PSZ)WXSTRINGCAST section
                                            ,(PSZ)WXSTRINGCAST entry
                                            ,(PSZ)WXSTRINGCAST value
                                           ));
        }
    }
    else
        return (::PrfWriteProfileString( HINI_PROFILE
                                        ,(PSZ)WXSTRINGCAST section
                                        ,(PSZ)WXSTRINGCAST entry
                                        ,(PSZ)WXSTRINGCAST value
                                       ));
    return FALSE;
}

bool wxWriteResource(const wxString& section, const wxString& entry, float value, const wxString& file)
{
    wxChar buf[50];
    wxSprintf(buf, "%.4f", value);
    return wxWriteResource(section, entry, buf, file);
}

bool wxWriteResource(const wxString& section, const wxString& entry, long value, const wxString& file)
{
    wxChar buf[50];
    wxSprintf(buf, "%ld", value);
    return wxWriteResource(section, entry, buf, file);
}

bool wxWriteResource(const wxString& section, const wxString& entry, int value, const wxString& file)
{
    wxChar buf[50];
    wxSprintf(buf, "%d", value);
    return wxWriteResource(section, entry, buf, file);
}

bool wxGetResource(const wxString& section, const wxString& entry, wxChar **value, const wxString& file)
{
    HAB                             hab;
    HINI                            hIni;
    static const wxChar             defunkt[] = _T("$$default");

    if (file != "")
    {
        hIni = ::PrfOpenProfile(hab, (PSZ)WXSTRINGCAST file);
        if (hIni != 0L)
        {
            ULONG n = ::PrfQueryProfileString( hIni
                                              ,(PSZ)WXSTRINGCAST section
                                              ,(PSZ)WXSTRINGCAST entry
                                              ,(PSZ)defunkt
                                              ,(void*)wxBuffer
                                              ,1000
                                             );
            if (n == 0L || wxStrcmp(wxBuffer, defunkt) == 0)
                return FALSE;
        }
        else
            return FALSE;
    }
    else
    {
        ULONG n = ::PrfQueryProfileString( HINI_PROFILE
                                          ,(PSZ)WXSTRINGCAST section
                                          ,(PSZ)WXSTRINGCAST entry
                                          ,(PSZ)defunkt
                                          ,(void*)wxBuffer
                                          ,1000
                                         );
        if (n == 0L || wxStrcmp(wxBuffer, defunkt) == 0)
            return FALSE;
    }
    if (*value)
        delete[] (*value);
    *value = copystring(wxBuffer);
    return TRUE;
}

bool wxGetResource(const wxString& section, const wxString& entry, float *value, const wxString& file)
{
  wxChar *s = NULL;
  bool succ = wxGetResource(section, entry, (wxChar **)&s, file);
  if (succ)
  {
    *value = (float)wxStrtod(s, NULL);
    delete[] s;
    return TRUE;
  }
  else return FALSE;
}

bool wxGetResource(const wxString& section, const wxString& entry, long *value, const wxString& file)
{
  wxChar *s = NULL;
  bool succ = wxGetResource(section, entry, (wxChar **)&s, file);
  if (succ)
  {
    *value = wxStrtol(s, NULL, 10);
    delete[] s;
    return TRUE;
  }
  else return FALSE;
}

bool wxGetResource(const wxString& section, const wxString& entry, int *value, const wxString& file)
{
  wxChar *s = NULL;
  bool succ = wxGetResource(section, entry, (wxChar **)&s, file);
  if (succ)
  {
    *value = (int)wxStrtol(s, NULL, 10);
    delete[] s;
    return TRUE;
  }
  else return FALSE;
}
#endif // wxUSE_RESOURCES

// ---------------------------------------------------------------------------
// helper functions for showing a "busy" cursor
// ---------------------------------------------------------------------------

HCURSOR gs_wxBusyCursor = 0;     // new, busy cursor
HCURSOR gs_wxBusyCursorOld = 0;  // old cursor
static int gs_wxBusyCursorCount = 0;

// Set the cursor to the busy cursor for all windows
void wxBeginBusyCursor(wxCursor *cursor)
{
    if ( gs_wxBusyCursorCount++ == 0 )
    {
        gs_wxBusyCursor = (HCURSOR)cursor->GetHCURSOR();
        ::WinSetPointer(HWND_DESKTOP, (HPOINTER)gs_wxBusyCursor);
    }
    //else: nothing to do, already set
}

// Restore cursor to normal
void wxEndBusyCursor()
{
    wxCHECK_RET( gs_wxBusyCursorCount > 0,
                 _T("no matching wxBeginBusyCursor() for wxEndBusyCursor()"));

    if ( --gs_wxBusyCursorCount == 0 )
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
const wxChar* wxGetHomeDir(wxString *pstr)
{
  wxString& strDir = *pstr;

  // OS/2 has no idea about home,
  // so use the working directory instead?

  // 256 was taken from os2def.h
#ifndef MAX_PATH
#  define MAX_PATH  256
#endif


    char     DirName[256];
    ULONG    DirLen;

    ::DosQueryCurrentDir( 0, DirName, &DirLen);
    strDir = DirName;
  return strDir.c_str();
}

// Hack for MS-DOS
wxChar *wxGetUserHome (const wxString& user)
{
  wxChar *home;
  wxString user1(user);

  if (user1 != _T("")) {
    wxChar tmp[64];
    if (wxGetUserId(tmp, sizeof(tmp)/sizeof(char))) {
      // Guests belong in the temp dir
      if (wxStricmp(tmp, _T("annonymous")) == 0) {
        if ((home = wxGetenv(_T("TMP"))) != NULL ||
            (home = wxGetenv(_T("TMPDIR"))) != NULL ||
            (home = wxGetenv(_T("TEMP"))) != NULL)
            return *home ? home : (wxChar*)_T("\\");
      }
      if (wxStricmp(tmp, WXSTRINGCAST user1) == 0)
        user1 = _T("");
    }
  }
  if (user1 == _T(""))
    if ((home = wxGetenv(_T("HOME"))) != NULL)
    {
      wxStrcpy(wxBuffer, home);
      Unix2DosFilename(wxBuffer);
      return wxBuffer;
    }
  return NULL; // No home known!
}

// Check whether this window wants to process messages, e.g. Stop button
// in long calculations.
bool wxCheckForInterrupt(wxWindow *wnd)
{
  if(wnd){
    QMSG            msg;
    HAB             hab;
    HWND            hwndFilter;

    HWND win= (HWND) wnd->GetHWND();
    while(::WinPeekMsg(hab,&msg,hwndFilter,0,0,PM_REMOVE))
    {
      ::WinDispatchMsg( hab, &msg );
    }
    return TRUE;//*** temporary?
  }
  else{
    wxFAIL_MSG(_T("wnd==NULL !!!"));

    return FALSE;//*** temporary?
  }
}

wxChar *wxLoadUserResource(const wxString& resourceName, const wxString& resourceType)
{
  wxChar *s = NULL;

/*
*  How to in PM?
*
* #if !defined(__WIN32__) || defined(__TWIN32__)
*   HRSRC hResource = ::FindResource(wxGetInstance(), WXSTRINGCAST resourceName, WXSTRINGCAST resourceType);
* #else
* #ifdef UNICODE
*   HRSRC hResource = ::FindResourceW(wxGetInstance(), WXSTRINGCAST resourceName, WXSTRINGCAST resourceType);
* #else
*   HRSRC hResource = ::FindResourceA(wxGetInstance(), WXSTRINGCAST resourceName, WXSTRINGCAST resourceType);
* #endif
* #endif
*
*   if (hResource == 0)
*     return NULL;
*   HGLOBAL hData = ::LoadResource(wxGetInstance(), hResource);
*   if (hData == 0)
*     return NULL;
*   wxChar *theText = (wxChar *)LockResource(hData);
*   if (!theText)
*     return NULL;
*
* s = copystring(theText);
*/
  return s;
}

void wxGetMousePosition( int* x, int* y )
{
    POINTL pt;
    ::WinQueryPointerPos( HWND_DESKTOP, & pt );
    *x = pt.x;
    *y = pt.y;
};

// Return TRUE if we have a colour display
bool wxColourDisplay()
{
    bool flag;
    // TODO:  use DosQueryDevCaps to figure it out
    return flag;
}

// Returns depth of screen
int wxDisplayDepth()
{
    HDC                             hDc = ::WinOpenWindowDC((HWND)NULL);
    long                            lArray[CAPS_COLOR_BITCOUNT];
    int                             nPlanes;
    int                             nBitsPerPixel;
    int                             nDepth;

    if(DevQueryCaps( hDc
                    ,CAPS_FAMILY
                    ,CAPS_COLOR_BITCOUNT
                    ,lArray
                   ))
    {
        nPlanes = (int)lArray[CAPS_COLOR_PLANES];
        nBitsPerPixel = (int)lArray[CAPS_COLOR_BITCOUNT];
        nDepth = nPlanes * nBitsPerPixel;
    }
    DevCloseDC(hDc);
    return (nDepth);
}

// Get size of display
void wxDisplaySize(int *width, int *height)
{
    HDC                             hDc = ::WinOpenWindowDC((HWND)NULL);
    long                            lArray[CAPS_HEIGHT];

    if(DevQueryCaps( hDc
                    ,CAPS_FAMILY
                    ,CAPS_HEIGHT
                    ,lArray
                   ))
    {
        *width  = (int)lArray[CAPS_WIDTH];
        *height = (int)lArray[CAPS_HEIGHT];
    }
    DevCloseDC(hDc);
}

bool wxDirExists(const wxString& dir)
{
   // TODO:  Control program file stuff
   return TRUE;
}

// ---------------------------------------------------------------------------
// window information functions
// ---------------------------------------------------------------------------

wxString WXDLLEXPORT wxGetWindowText(WXHWND hWnd)
{
    wxString str;
    long len = ::WinQueryWindowTextLength((HWND)hWnd) + 1;
    ::WinQueryWindowText((HWND)hWnd, len, str.GetWriteBuf((int)len));
    str.UngetWriteBuf();

    return str;
}

wxString WXDLLEXPORT wxGetWindowClass(WXHWND hWnd)
{
    wxString str;

    int len = 256; // some starting value

    for ( ;; )
    {
        int count = ::WinQueryClassName((HWND)hWnd, len, str.GetWriteBuf(len));

        str.UngetWriteBuf();
        if ( count == len )
        {
            // the class name might have been truncated, retry with larger
            // buffer
            len *= 2;
        }
        else
        {
            break;
        }
    }
    return str;
}

WXWORD WXDLLEXPORT wxGetWindowId(WXHWND hWnd)
{
    return ::WinQueryWindowUShort((HWND)hWnd, QWS_ID);
}

