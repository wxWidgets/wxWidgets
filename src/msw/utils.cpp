/////////////////////////////////////////////////////////////////////////////
// Name:        utils.cpp
// Purpose:     Various utilities
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
// #pragma implementation "utils.h"   // Note: this is done in utilscmn.cpp now.
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/setup.h"
    #include "wx/utils.h"
    #include "wx/app.h"
    #include "wx/cursor.h"
#endif  //WX_PRECOMP

#include "wx/msw/private.h"
#include "wx/timer.h"
#include "wx/intl.h"

#include <windows.h>

#include <ctype.h>

#if !defined(__GNUWIN32__) && !defined(__WXWINE__) && !defined(__SALFORDC__)
    #include <direct.h>

    #ifndef __MWERKS__
        #include <dos.h>
    #endif
#endif  //GNUWIN32

#if defined(__GNUWIN32__) && !defined(__TWIN32__)
    #include <sys/unistd.h>
    #include <sys/stat.h>
#endif  //GNUWIN32

#include "wx/log.h"

#ifdef __BORLANDC__ // Please someone tell me which version of Borland needs
                    // this (3.1 I believe) and how to test for it.
                    // If this works for Borland 4.0 as well, then no worries.
    #include <dir.h>
#endif

// VZ: there is some code using NetXXX() functions to get the full user name:
//     I don't think it's a good idea because they don't work under Win95 and
//     seem to return the same as wxGetUserId() under NT. If you really want
//     to use them, just #define USE_NET_API
#undef USE_NET_API

#ifdef USE_NET_API
    #include <lm.h>
#endif // USE_NET_API

#if defined(__WIN32__) && !defined(__WXWINE__)
    #include <io.h>

    #ifndef __GNUWIN32__
        #include <shellapi.h>
    #endif
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef __WATCOMC__
    #if !(defined(_MSC_VER) && (_MSC_VER > 800))
        #include <errno.h>
    #endif
#endif
#include <stdarg.h>

//// BEGIN for console support: VC++ only
#ifdef __VISUALC__

#include "wx/msw/msvcrt.h"

#include <fcntl.h>

#include "wx/ioswrap.h"

#if wxUSE_IOSTREAMH
// N.B. BC++ doesn't have istream.h, ostream.h
#  include <io.h>
#  include <fstream.h>
#else
#  include <fstream>
#endif

/* Need to undef new if including crtdbg.h */
#  ifdef new
#  undef new
#  endif

#ifndef __WIN16__
#  include <crtdbg.h>
#endif

#  if defined(__WXDEBUG__) && wxUSE_GLOBAL_MEMORY_OPERATORS && wxUSE_DEBUG_NEW_ALWAYS
#  define new new(__FILE__,__LINE__)
#  endif

#endif
  // __VISUALC__
/// END for console support

// In the WIN.INI file
static const wxChar WX_SECTION[] = wxT("wxWindows");
static const wxChar eHOSTNAME[]  = wxT("HostName");
static const wxChar eUSERID[]    = wxT("UserId");
static const wxChar eUSERNAME[]  = wxT("UserName");

// For the following functions we SHOULD fill in support
// for Windows-NT (which I don't know) as I assume it begin
// a POSIX Unix (so claims MS) that it has some special
// functions beyond those provided by WinSock

// Get full hostname (eg. DoDo.BSn-Germany.crg.de)
bool wxGetHostName(wxChar *buf, int maxSize)
{
#if defined(__WIN32__) && !defined(__TWIN32__)
  DWORD nSize = maxSize;
  return (::GetComputerName(buf, &nSize) != 0);
#else
  wxChar *sysname;
  const wxChar *default_host = wxT("noname");

  if ((sysname = wxGetenv(wxT("SYSTEM_NAME"))) == NULL) {
     GetProfileString(WX_SECTION, eHOSTNAME, default_host, buf, maxSize - 1);
  } else
    wxStrncpy(buf, sysname, maxSize - 1);
  buf[maxSize] = wxT('\0');
  return *buf ? TRUE : FALSE;
#endif
}

// Get user ID e.g. jacs
bool wxGetUserId(wxChar *buf, int maxSize)
{
#if defined(__WIN32__) && !defined(__win32s__) && !defined(__TWIN32__)
    DWORD nSize = maxSize;
    if ( ::GetUserName(buf, &nSize) == 0 )
    {
        // actually, it does happen on Win9x if the user didn't log on
        DWORD res = ::GetEnvironmentVariable(wxT("username"), buf, maxSize);
        if ( res == 0 )
        {
            // not found
            return FALSE;
        }
    }

    return TRUE;
#else   // Win16 or Win32s
  wxChar *user;
  const wxChar *default_id = wxT("anonymous");

  // Can't assume we have NIS (PC-NFS) or some other ID daemon
  // So we ...
  if (  (user = wxGetenv(wxT("USER"))) == NULL &&
        (user = wxGetenv(wxT("LOGNAME"))) == NULL )
  {
     // Use wxWindows configuration data (comming soon)
     GetProfileString(WX_SECTION, eUSERID, default_id, buf, maxSize - 1);
  }
  else
  {
    wxStrncpy(buf, user, maxSize - 1);
  }

  return *buf ? TRUE : FALSE;
#endif
}

// Get user name e.g. Julian Smart
bool wxGetUserName(wxChar *buf, int maxSize)
{
#if wxUSE_PENWINDOWS && !defined(__WATCOMC__) && !defined(__GNUWIN32__)
  extern HANDLE g_hPenWin; // PenWindows Running?
  if (g_hPenWin)
  {
    // PenWindows Does have a user concept!
    // Get the current owner of the recognizer
    GetPrivateProfileString("Current", "User", default_name, wxBuffer, maxSize - 1, "PENWIN.INI");
    strncpy(buf, wxBuffer, maxSize - 1);
  }
  else
#endif
  {
#ifdef USE_NET_API
    CHAR szUserName[256];
    if ( !wxGetUserId(szUserName, WXSIZEOF(szUserName)) )
        return FALSE;

    // TODO how to get the domain name?
    CHAR *szDomain = "";

    // the code is based on the MSDN example (also see KB article Q119670)
    WCHAR wszUserName[256];          // Unicode user name
    WCHAR wszDomain[256];
    LPBYTE ComputerName;

    USER_INFO_2 *ui2;         // User structure

    // Convert ANSI user name and domain to Unicode
    MultiByteToWideChar( CP_ACP, 0, szUserName, strlen(szUserName)+1,
                          wszUserName, WXSIZEOF(wszUserName) );
    MultiByteToWideChar( CP_ACP, 0, szDomain, strlen(szDomain)+1,
                         wszDomain, WXSIZEOF(wszDomain) );

    // Get the computer name of a DC for the domain.
    if ( NetGetDCName( NULL, wszDomain, &ComputerName ) != NERR_Success )
    {
        wxLogError(wxT("Can not find domain controller"));

        goto error;
    }

    // Look up the user on the DC
    NET_API_STATUS status = NetUserGetInfo( (LPWSTR)ComputerName,
                                            (LPWSTR)&wszUserName,
                                            2, // level - we want USER_INFO_2
                                            (LPBYTE *) &ui2 );
    switch ( status )
    {
        case NERR_Success:
            // ok
            break;

        case NERR_InvalidComputer:
            wxLogError(wxT("Invalid domain controller name."));

            goto error;

        case NERR_UserNotFound:
            wxLogError(wxT("Invalid user name '%s'."), szUserName);

            goto error;

        default:
            wxLogSysError(wxT("Can't get information about user"));

            goto error;
    }

    // Convert the Unicode full name to ANSI
    WideCharToMultiByte( CP_ACP, 0, ui2->usri2_full_name, -1,
                         buf, maxSize, NULL, NULL );

    return TRUE;

error:
  wxLogError(wxT("Couldn't look up full user name."));

  return FALSE;
#else  // !USE_NET_API
    // Could use NIS, MS-Mail or other site specific programs
    // Use wxWindows configuration data
    bool ok = GetProfileString(WX_SECTION, eUSERNAME, wxT(""), buf, maxSize - 1) != 0;
    if ( !ok )
    {
        ok = wxGetUserId(buf, maxSize);
    }

    if ( !ok )
    {
        wxStrncpy(buf, wxT("Unknown User"), maxSize);
    }
#endif // Win32/16
  }

  return TRUE;
}

int wxKill(long pid, int sig)
{
  return 0;
}

//
// Execute a program in an Interactive Shell
//
bool
wxShell(const wxString& command)
{
  wxChar *shell;
  if ((shell = wxGetenv(wxT("COMSPEC"))) == NULL)
    shell = wxT("\\COMMAND.COM");

  wxChar tmp[255];
  if (command != wxT(""))
    wxSprintf(tmp, wxT("%s /c %s"), shell, WXSTRINGCAST command);
  else
    wxStrcpy(tmp, shell);

  return (wxExecute((wxChar *)tmp, FALSE) != 0);
}

// Get free memory in bytes, or -1 if cannot determine amount (e.g. on UNIX)
long wxGetFreeMemory()
{
#if defined(__WIN32__) && !defined(__BORLANDC__) && !defined(__TWIN32__)
  MEMORYSTATUS memStatus;
  memStatus.dwLength = sizeof(MEMORYSTATUS);
  GlobalMemoryStatus(&memStatus);
  return memStatus.dwAvailPhys;
#else
  return (long)GetFreeSpace(0);
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

static wxTimer *wxTheSleepTimer = NULL;

void wxUsleep(unsigned long milliseconds)
{
#ifdef __WIN32__
    ::Sleep(milliseconds);
#else
  if (inTimer)
    return;

  wxTheSleepTimer = new wxSleepTimer;
  inTimer = TRUE;
  wxTheSleepTimer->Start(milliseconds);
  while (inTimer)
  {
    if (wxTheApp->Pending())
      wxTheApp->Dispatch();
  }
  delete wxTheSleepTimer;
  wxTheSleepTimer = NULL;
#endif
}

void wxSleep(int nSecs)
{
#if 0 // WIN32 hangs app
  Sleep( 1000*nSecs );
#else
  if (inTimer)
    return;

  wxTheSleepTimer = new wxSleepTimer;
  inTimer = TRUE;
  wxTheSleepTimer->Start(nSecs*1000);
  while (inTimer)
  {
    if (wxTheApp->Pending())
      wxTheApp->Dispatch();
  }
  delete wxTheSleepTimer;
  wxTheSleepTimer = NULL;
#endif
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

  wvsprintf(buffer,fmt,ap) ;
  OutputDebugString((LPCTSTR)buffer) ;

  va_end(ap);
}

// Non-fatal error: pop up message box and (possibly) continue
void wxError(const wxString& msg, const wxString& title)
{
  wxSprintf(wxBuffer, wxT("%s\nContinue?"), WXSTRINGCAST msg);
  if (MessageBox(NULL, (LPCTSTR)wxBuffer, (LPCTSTR)WXSTRINGCAST title,
             MB_ICONSTOP | MB_YESNO) == IDNO)
    wxExit();
}

// Fatal error: pop up message box and abort
void wxFatalError(const wxString& msg, const wxString& title)
{
  wxSprintf(wxBuffer, wxT("%s: %s"), WXSTRINGCAST title, WXSTRINGCAST msg);
  FatalAppExit(0, (LPCTSTR)wxBuffer);
}

// Emit a beeeeeep
void wxBell()
{
    // Removed by RD because IHMO syncronous sound is a Bad Thing.  MessageBeep
    // will do a similar thing anyway if there is no sound card...
//#ifdef __WIN32__
//  Beep(1000,1000) ; // 1kHz during 1 sec.
//#else
  MessageBeep((UINT)-1) ;
//#endif
}

// Chris Breeze 27/5/98: revised WIN32 code to
// detect WindowsNT correctly
int wxGetOsVersion(int *majorVsn, int *minorVsn)
{
  if (majorVsn) *majorVsn = 0;
  if (minorVsn) *minorVsn = 0;

#if defined(__WIN32__) && !defined(__SC__)
  OSVERSIONINFO info;
  memset(&info, 0, sizeof(OSVERSIONINFO));
  info.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
  if (GetVersionEx(&info))
  {
    if (majorVsn) *majorVsn = info.dwMajorVersion;
    if (minorVsn) *minorVsn = info.dwMinorVersion;
    switch (info.dwPlatformId)
  {
  case VER_PLATFORM_WIN32s:
    return wxWIN32S;
    break;
  case VER_PLATFORM_WIN32_WINDOWS:
    return wxWIN95;
    break;
  case VER_PLATFORM_WIN32_NT:
    return wxWINDOWS_NT;
    break;
  }
  }
  return wxWINDOWS; // error if we get here, return generic value
#else
  // Win16 code...
  int retValue = 0;
#  ifdef __WINDOWS_386__
  retValue = wxWIN386;
#  else
#    if !defined(__WATCOMC__) && !defined(GNUWIN32) && wxUSE_PENWINDOWS
  extern HANDLE g_hPenWin;
  retValue = g_hPenWin ? wxPENWINDOWS : wxWINDOWS ;
#    endif
#  endif
  // @@@@ To be completed. I don't have the manual here...
  if (majorVsn) *majorVsn = 3 ;
  if (minorVsn) *minorVsn = 1 ;
  return retValue ;
#endif
}

// Reading and writing resources (eg WIN.INI, .Xdefaults)
#if wxUSE_RESOURCES
bool wxWriteResource(const wxString& section, const wxString& entry, const wxString& value, const wxString& file)
{
  if (file != wxT(""))
    return (WritePrivateProfileString((LPCTSTR)WXSTRINGCAST section, (LPCTSTR)WXSTRINGCAST entry, (LPCTSTR)value, (LPCTSTR)WXSTRINGCAST file) != 0);
  else
    return (WriteProfileString((LPCTSTR)WXSTRINGCAST section, (LPCTSTR)WXSTRINGCAST entry, (LPCTSTR)WXSTRINGCAST value) != 0);
}

bool wxWriteResource(const wxString& section, const wxString& entry, float value, const wxString& file)
{
  wxChar buf[50];
  wxSprintf(buf, wxT("%.4f"), value);
  return wxWriteResource(section, entry, buf, file);
}

bool wxWriteResource(const wxString& section, const wxString& entry, long value, const wxString& file)
{
  wxChar buf[50];
  wxSprintf(buf, wxT("%ld"), value);
  return wxWriteResource(section, entry, buf, file);
}

bool wxWriteResource(const wxString& section, const wxString& entry, int value, const wxString& file)
{
  wxChar buf[50];
  wxSprintf(buf, wxT("%d"), value);
  return wxWriteResource(section, entry, buf, file);
}

bool wxGetResource(const wxString& section, const wxString& entry, wxChar **value, const wxString& file)
{
  static const wxChar defunkt[] = wxT("$$default");
  if (file != wxT(""))
  {
    int n = GetPrivateProfileString((LPCTSTR)WXSTRINGCAST section, (LPCTSTR)WXSTRINGCAST entry, (LPCTSTR)defunkt,
                                    (LPTSTR)wxBuffer, 1000, (LPCTSTR)WXSTRINGCAST file);
    if (n == 0 || wxStrcmp(wxBuffer, defunkt) == 0)
     return FALSE;
  }
  else
  {
    int n = GetProfileString((LPCTSTR)WXSTRINGCAST section, (LPCTSTR)WXSTRINGCAST entry, (LPCTSTR)defunkt,
                                    (LPTSTR)wxBuffer, 1000);
    if (n == 0 || wxStrcmp(wxBuffer, defunkt) == 0)
      return FALSE;
  }
  if (*value) delete[] (*value);
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
        gs_wxBusyCursorOld = ::SetCursor(gs_wxBusyCursor);
    }
    //else: nothing to do, already set
}

// Restore cursor to normal
void wxEndBusyCursor()
{
    wxCHECK_RET( gs_wxBusyCursorCount > 0,
                 wxT("no matching wxBeginBusyCursor() for wxEndBusyCursor()") );

    if ( --gs_wxBusyCursorCount == 0 )
    {
        ::SetCursor(gs_wxBusyCursorOld);

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

  #if defined(__UNIX__) && !defined(__TWIN32__)
    const wxChar *szHome = wxGetenv("HOME");
    if ( szHome == NULL ) {
      // we're homeless...
      wxLogWarning(_("can't find user's HOME, using current directory."));
      strDir = wxT(".");
    }
    else
       strDir = szHome;

    // add a trailing slash if needed
    if ( strDir.Last() != wxT('/') )
      strDir << wxT('/');
  #else   // Windows
    #ifdef  __WIN32__
      const wxChar *szHome = wxGetenv(wxT("HOMEDRIVE"));
      if ( szHome != NULL )
        strDir << szHome;
      szHome = wxGetenv(wxT("HOMEPATH"));
      if ( szHome != NULL ) {
        strDir << szHome;

        // the idea is that under NT these variables have default values
        // of "%systemdrive%:" and "\\". As we don't want to create our
        // config files in the root directory of the system drive, we will
        // create it in our program's dir. However, if the user took care
        // to set HOMEPATH to something other than "\\", we suppose that he
        // knows what he is doing and use the supplied value.
        if ( wxStrcmp(szHome, wxT("\\")) != 0 )
          return strDir.c_str();
      }

    #else   // Win16
      // Win16 has no idea about home, so use the working directory instead
    #endif  // WIN16/32

    // 260 was taken from windef.h
    #ifndef MAX_PATH
      #define MAX_PATH  260
    #endif

    wxString strPath;
    ::GetModuleFileName(::GetModuleHandle(NULL),
                        strPath.GetWriteBuf(MAX_PATH), MAX_PATH);
    strPath.UngetWriteBuf();

    // extract the dir name
    wxSplitPath(strPath, &strDir, NULL, NULL);

  #endif  // UNIX/Win

  return strDir.c_str();
}

// Hack for MS-DOS
wxChar *wxGetUserHome (const wxString& user)
{
  wxChar *home;
  wxString user1(user);

  if (user1 != wxT("")) {
    wxChar tmp[64];
    if (wxGetUserId(tmp, sizeof(tmp)/sizeof(char))) {
      // Guests belong in the temp dir
      if (wxStricmp(tmp, wxT("annonymous")) == 0) {
        if ((home = wxGetenv(wxT("TMP"))) != NULL ||
            (home = wxGetenv(wxT("TMPDIR"))) != NULL ||
            (home = wxGetenv(wxT("TEMP"))) != NULL)
            return *home ? home : (wxChar*)wxT("\\");
      }
      if (wxStricmp(tmp, WXSTRINGCAST user1) == 0)
        user1 = wxT("");
    }
  }
  if (user1 == wxT(""))
    if ((home = wxGetenv(wxT("HOME"))) != NULL)
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
    MSG msg;
    HWND win= (HWND) wnd->GetHWND();
    while(PeekMessage(&msg,win,0,0,PM_REMOVE)){
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
    return TRUE;//*** temporary?
  }
  else{
    wxFAIL_MSG(wxT("wnd==NULL !!!"));

    return FALSE;//*** temporary?
  }
}

// MSW only: get user-defined resource from the .res file.
// Returns NULL or newly-allocated memory, so use delete[] to clean up.

#ifdef __WXMSW__
wxChar *wxLoadUserResource(const wxString& resourceName, const wxString& resourceType)
{
  wxChar *s = NULL;
#if !defined(__WIN32__) || defined(__TWIN32__)
  HRSRC hResource = ::FindResource(wxGetInstance(), WXSTRINGCAST resourceName, WXSTRINGCAST resourceType);
#else
#ifdef UNICODE
  HRSRC hResource = ::FindResourceW(wxGetInstance(), WXSTRINGCAST resourceName, WXSTRINGCAST resourceType);
#else
  HRSRC hResource = ::FindResourceA(wxGetInstance(), WXSTRINGCAST resourceName, WXSTRINGCAST resourceType);
#endif
#endif

  if (hResource == 0)
    return NULL;
  HGLOBAL hData = ::LoadResource(wxGetInstance(), hResource);
  if (hData == 0)
    return NULL;
  wxChar *theText = (wxChar *)LockResource(hData);
  if (!theText)
    return NULL;

  s = copystring(theText);

  // Obsolete in WIN32
#ifndef __WIN32__
  UnlockResource(hData);
#endif

  // No need??
//  GlobalFree(hData);

  return s;
}
#endif

void wxGetMousePosition( int* x, int* y )
{
  POINT pt;
  GetCursorPos( & pt );
  *x = pt.x;
  *y = pt.y;
};

// Return TRUE if we have a colour display
bool wxColourDisplay()
{
  HDC dc = ::GetDC((HWND) NULL);
  bool flag;
  int noCols = GetDeviceCaps(dc, NUMCOLORS);
  if ((noCols == -1) || (noCols > 2))
    flag = TRUE;
  else
    flag = FALSE;
  ReleaseDC((HWND) NULL, dc);
  return flag;
}

// Returns depth of screen
int wxDisplayDepth()
{
  HDC dc = ::GetDC((HWND) NULL);
  int planes = GetDeviceCaps(dc, PLANES);
  int bitsPerPixel = GetDeviceCaps(dc, BITSPIXEL);
  int depth = planes*bitsPerPixel;
  ReleaseDC((HWND) NULL, dc);
  return depth;
}

// Get size of display
void wxDisplaySize(int *width, int *height)
{
  HDC dc = ::GetDC((HWND) NULL);
  *width = GetDeviceCaps(dc, HORZRES); *height = GetDeviceCaps(dc, VERTRES);
  ReleaseDC((HWND) NULL, dc);
}

bool wxDirExists(const wxString& dir)
{
  /* MATTHEW: [6] Always use same code for Win32, call FindClose */
#if defined(__WIN32__)
  WIN32_FIND_DATA fileInfo;
#else
#ifdef __BORLANDC__
  struct ffblk fileInfo;
#else
  struct find_t fileInfo;
#endif
#endif

#if defined(__WIN32__)
  HANDLE h = FindFirstFile((LPTSTR) WXSTRINGCAST dir,(LPWIN32_FIND_DATA)&fileInfo);

  if (h==INVALID_HANDLE_VALUE)
   return FALSE;
  else {
   FindClose(h);
   return ((fileInfo.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY);
  }
#else
  // In Borland findfirst has a different argument
  // ordering from _dos_findfirst. But _dos_findfirst
  // _should_ be ok in both MS and Borland... why not?
#ifdef __BORLANDC__
  return ((findfirst(WXSTRINGCAST dir, &fileInfo, _A_SUBDIR) == 0  && (fileInfo.ff_attrib & _A_SUBDIR) != 0));
#else
  return (((_dos_findfirst(WXSTRINGCAST dir, _A_SUBDIR, &fileInfo) == 0) && (fileInfo.attrib & _A_SUBDIR)) != 0);
#endif
#endif
}

// ---------------------------------------------------------------------------
// window information functions
// ---------------------------------------------------------------------------

wxString WXDLLEXPORT wxGetWindowText(WXHWND hWnd)
{
    wxString str;
    int len = GetWindowTextLength((HWND)hWnd) + 1;
    GetWindowText((HWND)hWnd, str.GetWriteBuf(len), len);
    str.UngetWriteBuf();

    return str;
}

wxString WXDLLEXPORT wxGetWindowClass(WXHWND hWnd)
{
    wxString str;

    int len = 256; // some starting value

    for ( ;; )
    {
        // as we've #undefined GetClassName we must now manually choose the
        // right function to call
        int count =

        #ifndef __WIN32__
            GetClassName
        #else // Win32
            #ifdef UNICODE
                GetClassNameW
            #else // !Unicode
                #ifdef __TWIN32__
                    GetClassName
                #else // !Twin32
                    GetClassNameA
                #endif // Twin32/!Twin32
            #endif // Unicode/ANSI
        #endif // Win16/32
                                    ((HWND)hWnd, str.GetWriteBuf(len), len);

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
#ifndef __WIN32__
    return GetWindowWord((HWND)hWnd, GWW_ID);
#else // Win32
    return GetWindowLong((HWND)hWnd, GWL_ID);
#endif // Win16/32
}

#if 0
//------------------------------------------------------------------------
// wild character routines
//------------------------------------------------------------------------

bool wxIsWild( const wxString& pattern )
{
  wxString tmp = pattern;
  char *pat = WXSTRINGCAST(tmp);
    while (*pat) {
  switch (*pat++) {
  case '?': case '*': case '[': case '{':
      return TRUE;
  case '\\':
      if (!*pat++)
    return FALSE;
  }
    }
    return FALSE;
};


bool wxMatchWild( const wxString& pat, const wxString& text, bool dot_special )
{
  wxString tmp1 = pat;
  char *pattern = WXSTRINGCAST(tmp1);
  wxString tmp2 = text;
  char *str = WXSTRINGCAST(tmp2);
    char c;
    char *cp;
    bool done = FALSE, ret_code, ok;
    // Below is for vi fans
    const char OB = '{', CB = '}';

    // dot_special means '.' only matches '.'
    if (dot_special && *str == '.' && *pattern != *str)
  return FALSE;

    while ((*pattern != '\0') && (!done)
    && (((*str=='\0')&&((*pattern==OB)||(*pattern=='*')))||(*str!='\0'))) {
  switch (*pattern) {
  case '\\':
      pattern++;
      if (*pattern != '\0')
    pattern++;
      break;
  case '*':
      pattern++;
      ret_code = FALSE;
      while ((*str!='\0')
      && (!(ret_code=wxMatchWild(pattern, str++, FALSE))))
    /*loop*/;
      if (ret_code) {
    while (*str != '\0')
        str++;
    while (*pattern != '\0')
        pattern++;
      }
      break;
  case '[':
      pattern++;
    repeat:
      if ((*pattern == '\0') || (*pattern == ']')) {
    done = TRUE;
    break;
      }
      if (*pattern == '\\') {
    pattern++;
    if (*pattern == '\0') {
        done = TRUE;
        break;
    }
      }
      if (*(pattern + 1) == '-') {
    c = *pattern;
    pattern += 2;
    if (*pattern == ']') {
        done = TRUE;
        break;
    }
    if (*pattern == '\\') {
        pattern++;
        if (*pattern == '\0') {
      done = TRUE;
      break;
        }
    }
    if ((*str < c) || (*str > *pattern)) {
        pattern++;
        goto repeat;
    }
      } else if (*pattern != *str) {
    pattern++;
    goto repeat;
      }
      pattern++;
      while ((*pattern != ']') && (*pattern != '\0')) {
    if ((*pattern == '\\') && (*(pattern + 1) != '\0'))
        pattern++;
    pattern++;
      }
      if (*pattern != '\0') {
    pattern++, str++;
      }
      break;
  case '?':
      pattern++;
      str++;
      break;
  case OB:
      pattern++;
      while ((*pattern != CB) && (*pattern != '\0')) {
    cp = str;
    ok = TRUE;
    while (ok && (*cp != '\0') && (*pattern != '\0')
    &&  (*pattern != ',') && (*pattern != CB)) {
        if (*pattern == '\\')
      pattern++;
        ok = (*pattern++ == *cp++);
    }
    if (*pattern == '\0') {
        ok = FALSE;
        done = TRUE;
        break;
    } else if (ok) {
        str = cp;
        while ((*pattern != CB) && (*pattern != '\0')) {
      if (*++pattern == '\\') {
          if (*++pattern == CB)
        pattern++;
      }
        }
    } else {
        while (*pattern!=CB && *pattern!=',' && *pattern!='\0') {
      if (*++pattern == '\\') {
                            if (*++pattern == CB || *pattern == ',')
        pattern++;
      }
        }
    }
    if (*pattern != '\0')
        pattern++;
      }
      break;
  default:
      if (*str == *pattern) {
    str++, pattern++;
      } else {
    done = TRUE;
      }
  }
    }
    while (*pattern == '*')
  pattern++;
    return ((*str == '\0') && (*pattern == '\0'));
};

#endif

#if defined(__WIN95__) && defined(__WXDEBUG__) && wxUSE_DBWIN32

/*
When I started programming with Visual C++ v4.0, I missed one of my favorite
tools -- DBWIN.  Finding the code for a simple debug trace utility, DBMON,
on MSDN was a step in the right direction, but it is a console application
and thus has limited features and extensibility.  DBWIN32 is my creation
to solve this problem.

The code is essentially a merging of a stripped down version of the DBWIN code
from VC 1.5 and DBMON.C with a few 32 bit changes.

As of version 1.2B, DBWIN32 supports both Win95 and NT.  The NT support is
built into the operating system and works just by running DBWIN32.  The Win95
team decided not to support this hook, so I have provided code that will do
this for you.  See the file WIN95.TXT for instructions on installing this.

If you have questions, problems or suggestions about DBWIN32, I welcome your
feedback and plan to actively maintain the code.

Andrew Tucker
ast@halcyon.com

To download dbwin32, see e.g.:

http://ftp.digital.com/pub/micro/NT/WinSite/programr/dbwin32.zip
*/

#if !defined(__MWERKS__) && !defined(__SALFORDC__) && !defined(__TWIN32__)
#include <process.h>
#endif

void OutputDebugStringW95(const wxChar* lpOutputString, ...)
{
    HANDLE heventDBWIN;  /* DBWIN32 synchronization object */
    HANDLE heventData;   /* data passing synch object */
    HANDLE hSharedFile;  /* memory mapped file shared data */
    LPSTR lpszSharedMem;
    wxChar achBuffer[500];

    /* create the output buffer */
    va_list args;
    va_start(args, lpOutputString);
    wxVsprintf(achBuffer, lpOutputString, args);
    va_end(args);

    /*
        Do a regular OutputDebugString so that the output is
        still seen in the debugger window if it exists.

        This ifdef is necessary to avoid infinite recursion
        from the inclusion of W95TRACE.H
    */
#ifdef _UNICODE
    ::OutputDebugStringW(achBuffer);
#else
#ifdef __TWIN32__
    ::OutputDebugString(achBuffer);
#else
    ::OutputDebugStringA(achBuffer);
#endif
#endif

    /* bail if it's not Win95 */
    {
        OSVERSIONINFO VerInfo;
        VerInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
        GetVersionEx(&VerInfo);
        if ( VerInfo.dwPlatformId != VER_PLATFORM_WIN32_WINDOWS )
            return;
    }

    /* make sure DBWIN is open and waiting */
    heventDBWIN = OpenEvent(EVENT_MODIFY_STATE, FALSE, wxT("DBWIN_BUFFER_READY"));
    if ( !heventDBWIN )
    {
        //MessageBox(NULL, wxT("DBWIN_BUFFER_READY nonexistent"), NULL, MB_OK);
        return;
    }

    /* get a handle to the data synch object */
    heventData = OpenEvent(EVENT_MODIFY_STATE, FALSE, wxT("DBWIN_DATA_READY"));
    if ( !heventData )
    {
        // MessageBox(NULL, wxT("DBWIN_DATA_READY nonexistent"), NULL, MB_OK);
        CloseHandle(heventDBWIN);
        return;
    }

    hSharedFile = CreateFileMapping((HANDLE)-1, NULL, PAGE_READWRITE, 0, 4096, wxT("DBWIN_BUFFER"));
    if (!hSharedFile)
    {
        //MessageBox(NULL, wxT("DebugTrace: Unable to create file mapping object DBWIN_BUFFER"), wxT("Error"), MB_OK);
        CloseHandle(heventDBWIN);
        CloseHandle(heventData);
        return;
    }

    lpszSharedMem = (LPSTR)MapViewOfFile(hSharedFile, FILE_MAP_WRITE, 0, 0, 512);
    if (!lpszSharedMem)
    {
        //MessageBox(NULL, wxT("DebugTrace: Unable to map shared memory"), wxT("Error"), MB_OK);
        CloseHandle(heventDBWIN);
        CloseHandle(heventData);
        return;
    }

    /* wait for buffer event */
    WaitForSingleObject(heventDBWIN, INFINITE);

    /* write it to the shared memory */
#if defined( __BORLANDC__ ) || defined( __MWERKS__ ) || defined(__SALFORDC__)
    *((LPDWORD)lpszSharedMem) = getpid();
#else
    *((LPDWORD)lpszSharedMem) = _getpid();
#endif

    wsprintf((LPTSTR)(lpszSharedMem + sizeof(DWORD)), wxT("%s"), achBuffer);

    /* signal data ready event */
    SetEvent(heventData);

    /* clean up handles */
    CloseHandle(hSharedFile);
    CloseHandle(heventData);
    CloseHandle(heventDBWIN);

    return;
}


#endif


#if 0

// maximum mumber of lines the output console should have
static const WORD MAX_CONSOLE_LINES = 500;

BOOL WINAPI MyConsoleHandler( DWORD dwCtrlType ) {   //  control signal type
  FreeConsole();
  return TRUE;
}

void wxRedirectIOToConsole()
{
    int                        hConHandle;
    long                       lStdHandle;
    CONSOLE_SCREEN_BUFFER_INFO coninfo;
    FILE                       *fp;

    // allocate a console for this app
    AllocConsole();

    // set the screen buffer to be big enough to let us scroll text
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE),
                               &coninfo);
    coninfo.dwSize.Y = MAX_CONSOLE_LINES;
    SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE),
                               coninfo.dwSize);

    // redirect unbuffered STDOUT to the console
    lStdHandle = (long)GetStdHandle(STD_OUTPUT_HANDLE);
    hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
    if(hConHandle <= 0) return;
    fp = _fdopen( hConHandle, "w" );
    *stdout = *fp;
    setvbuf( stdout, NULL, _IONBF, 0 );

    // redirect unbuffered STDIN to the console
    lStdHandle = (long)GetStdHandle(STD_INPUT_HANDLE);
    hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
    if(hConHandle <= 0) return;
    fp = _fdopen( hConHandle, "r" );
    *stdin = *fp;
    setvbuf( stdin, NULL, _IONBF, 0 );

    // redirect unbuffered STDERR to the console
    lStdHandle = (long)GetStdHandle(STD_ERROR_HANDLE);
    hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
    if(hConHandle <= 0) return;
    fp = _fdopen( hConHandle, "w" );
    *stderr = *fp;
    setvbuf( stderr, NULL, _IONBF, 0 );

    // make cout, wcout, cin, wcin, wcerr, cerr, wclog and clog
    // point to console as well
    ios::sync_with_stdio();

    SetConsoleCtrlHandler(MyConsoleHandler, TRUE);
}
#else
// Not supported
void wxRedirectIOToConsole()
{
}
#endif


