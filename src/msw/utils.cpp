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
// Note: this is done in utilscmn.cpp now.
// #pragma implementation
// #pragma implementation "utils.h"
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

#include <ctype.h>

#ifndef __GNUWIN32__
#include <direct.h>
#include <dos.h>
#endif  //GNUWIN32

#ifdef __GNUWIN32__
#include <sys/unistd.h>
#include <sys/stat.h>
#ifndef __MINGW32__
#include <std.h>
#endif  //MINGW32

#endif  //GNUWIN32

#include "wx/log.h"

#ifdef __BORLANDC__ // Please someone tell me which version of Borland needs
                    // this (3.1 I believe) and how to test for it.
                    // If this works for Borland 4.0 as well, then no worries.
#include <dir.h>
#endif

#ifdef __WIN32__
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

// In the WIN.INI file
static const char WX_SECTION[] = "wxWindows";
static const char eHOSTNAME[]  = "HostName";
static const char eUSERID[]    = "UserId";
static const char eUSERNAME[]  = "UserName";

// For the following functions we SHOULD fill in support
// for Windows-NT (which I don't know) as I assume it begin
// a POSIX Unix (so claims MS) that it has some special
// functions beyond those provided by WinSock

// Get full hostname (eg. DoDo.BSn-Germany.crg.de)
bool wxGetHostName(char *buf, int maxSize)
{
#ifdef __WIN32__
  DWORD nSize = maxSize;
  return (::GetComputerName(buf, &nSize) != 0);
#else
  char *sysname;
  const char *default_host = "noname";

  if ((sysname = getenv("SYSTEM_NAME")) == NULL) {
     GetProfileString(WX_SECTION, eHOSTNAME, default_host, buf, maxSize - 1);
  } else
    strncpy(buf, sysname, maxSize - 1);
  buf[maxSize] = '\0';
  return *buf ? TRUE : FALSE;
#endif
}

// Get user ID e.g. jacs
bool wxGetUserId(char *buf, int maxSize)
{
#if defined(__WIN32__) && !defined(__win32s__) && 0
  // Gets the current user's full name according to the MS article PSS ID
  // Number: Q119670
  // Seems to be the same as the login name for me?
  char *UserName = new char[256];
  char *Domain = new char[256];
  DWORD maxCharacters = 255;
  GetUserName( UserName, &maxCharacters );
  GetComputerName( Domain, &maxCharacters );

  WCHAR  wszUserName[256];           // Unicode user name
  WCHAR  wszDomain[256];
  LPBYTE ComputerName;
 
  struct _SERVER_INFO_100 *si100;   // Server structure
  struct _USER_INFO_2 *ui;          // User structure
 
  // Convert ASCII user name and domain to Unicode.
 
  MultiByteToWideChar( CP_ACP, 0, UserName,
     strlen(UserName)+1, wszUserName, sizeof(wszUserName) );
  MultiByteToWideChar( CP_ACP, 0, Domain,
     strlen(Domain)+1, wszDomain, sizeof(wszDomain) );

  // Get the computer name of a DC for the specified domain.
  // >If you get a link error on this, include netapi32.lib<
 
  NetGetDCName( NULL, wszDomain, &ComputerName );
 
  // Look up the user on the DC.
 
  if(NetUserGetInfo( (LPWSTR) ComputerName,
     (LPWSTR) &wszUserName, 2, (LPBYTE *) &ui))
  {
     printf( "Error getting user information.\n" );
     return( FALSE );
  }
 
  // Convert the Unicode full name to ASCII.
 
  WideCharToMultiByte( CP_ACP, 0, ui->usri2_full_name,
     -1, buf, 256, NULL, NULL );
  }
  return( TRUE );
/*
  DWORD nSize = maxSize;
  return ::GetUserName(buf, &nSize);
*/
#else
  char *user;
  const char *default_id = "anonymous";

  // Can't assume we have NIS (PC-NFS) or some other ID daemon
  // So we ...
  if (  (user = getenv("USER")) == NULL &&
  (user = getenv("LOGNAME")) == NULL ) {
     // Use wxWindows configuration data (comming soon)
     GetProfileString(WX_SECTION, eUSERID, default_id, buf, maxSize - 1);
  } else
    strncpy(buf, user, maxSize - 1);
  return *buf ? TRUE : FALSE;
#endif
}

// Get user name e.g. Julian Smart
bool wxGetUserName(char *buf, int maxSize)
{
  const char *default_name = "Unknown User"; 
#if defined(__WIN32__)
/*
  DWORD nSize = maxSize;
  In VC++ 4.0, results in unresolved symbol __imp__GetUserNameA
  if (GetUserName(buf, &nSize))
    return TRUE;
  else
*/
    // Could use NIS, MS-Mail or other site specific programs
    // Use wxWindows configuration data 
    GetProfileString(WX_SECTION, eUSERNAME, default_name, buf, maxSize - 1);
    return *buf ? TRUE : FALSE;
//  }
#else
#if !defined(__WATCOMC__) && !defined(__GNUWIN32__) && USE_PENWINDOWS
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
    // Could use NIS, MS-Mail or other site specific programs
    // Use wxWindows configuration data 
    GetProfileString(WX_SECTION, eUSERNAME, default_name, buf, maxSize - 1);
  }
  return *buf ? TRUE : FALSE;
#endif
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
  char *shell;
  if ((shell = getenv("COMSPEC")) == NULL)
    shell = "\\COMMAND.COM";

  char tmp[255];
  if (command != "")
    sprintf(tmp, "%s /c %s", shell, WXSTRINGCAST command);
  else
    strcpy(tmp, shell);

  return (wxExecute((char *)tmp, FALSE) != 0);
}

// Get free memory in bytes, or -1 if cannot determine amount (e.g. on UNIX)
long wxGetFreeMemory(void)
{
#if defined(__WIN32__) && !defined(__BORLANDC__)
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
  inline void Notify(void)
  {
    inTimer = FALSE;
    Stop();
  }
};

static wxTimer *wxTheSleepTimer = NULL;

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
void wxFlushEvents(void)
{
//  wxYield();
}

// Output a debug mess., in a system dependent fashion.
void wxDebugMsg(const char *fmt ...)
{
  va_list ap;
  static char buffer[512];

  if (!wxTheApp->GetWantDebugOutput())
    return ;

  va_start(ap, fmt);

  wvsprintf(buffer,fmt,ap) ;
  OutputDebugString((LPCSTR)buffer) ;

  va_end(ap);
}

// Non-fatal error: pop up message box and (possibly) continue
void wxError(const wxString& msg, const wxString& title)
{
  sprintf(wxBuffer, "%s\nContinue?", WXSTRINGCAST msg);
  if (MessageBox(NULL, (LPCSTR)wxBuffer, (LPCSTR)WXSTRINGCAST title,
             MB_ICONSTOP | MB_YESNO) == IDNO)
    wxExit();
}

// Fatal error: pop up message box and abort
void wxFatalError(const wxString& msg, const wxString& title)
{
  sprintf(wxBuffer, "%s: %s", WXSTRINGCAST title, WXSTRINGCAST msg);
  FatalAppExit(0, (LPCSTR)wxBuffer);
}

// Emit a beeeeeep
void wxBell(void)
{
#ifdef __WIN32__
  Beep(1000,1000) ; // 1kHz during 1 sec.
#else
  MessageBeep(-1) ;
#endif
}

// Chris Breeze 27/5/98: revised WIN32 code to
// detect WindowsNT correctly
int wxGetOsVersion(int *majorVsn, int *minorVsn)
{
  extern char *wxOsVersion;
  if (majorVsn) *majorVsn = 0;
  if (minorVsn) *minorVsn = 0;

#ifdef WIN32
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
  int retValue ;
#  ifdef __WINDOWS_386__
  retValue = wxWIN386;
#  else
#    if !defined(__WATCOMC__) && !defined(GNUWIN32) && USE_PENWINDOWS
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
#if USE_RESOURCES
bool wxWriteResource(const wxString& section, const wxString& entry, const wxString& value, const wxString& file)
{
  if (file != "")
    return (WritePrivateProfileString((LPCSTR)WXSTRINGCAST section, (LPCSTR)WXSTRINGCAST entry, (LPCSTR)value, (LPCSTR)WXSTRINGCAST file) != 0);
  else
    return (WriteProfileString((LPCSTR)WXSTRINGCAST section, (LPCSTR)WXSTRINGCAST entry, (LPCSTR)WXSTRINGCAST value) != 0);
}

bool wxWriteResource(const wxString& section, const wxString& entry, float value, const wxString& file)
{
  char buf[50];
  sprintf(buf, "%.4f", value);
  return wxWriteResource(section, entry, buf, file);
}

bool wxWriteResource(const wxString& section, const wxString& entry, long value, const wxString& file)
{
  char buf[50];
  sprintf(buf, "%ld", value);
  return wxWriteResource(section, entry, buf, file);
}

bool wxWriteResource(const wxString& section, const wxString& entry, int value, const wxString& file)
{
  char buf[50];
  sprintf(buf, "%d", value);
  return wxWriteResource(section, entry, buf, file);
}

bool wxGetResource(const wxString& section, const wxString& entry, char **value, const wxString& file)
{
  static const char defunkt[] = "$$default";
  if (file != "")
  {
    int n = GetPrivateProfileString((LPCSTR)WXSTRINGCAST section, (LPCSTR)WXSTRINGCAST entry, (LPCSTR)defunkt,
                                    (LPSTR)wxBuffer, 1000, (LPCSTR)WXSTRINGCAST file);
    if (n == 0 || strcmp(wxBuffer, defunkt) == 0)
     return FALSE;
  }
  else
  {
    int n = GetProfileString((LPCSTR)WXSTRINGCAST section, (LPCSTR)WXSTRINGCAST entry, (LPCSTR)defunkt,
                                    (LPSTR)wxBuffer, 1000);
    if (n == 0 || strcmp(wxBuffer, defunkt) == 0)
      return FALSE;
  }
  if (*value) delete[] (*value);
      *value = copystring(wxBuffer);
      return TRUE;
    }

bool wxGetResource(const wxString& section, const wxString& entry, float *value, const wxString& file)
{
  char *s = NULL;
  bool succ = wxGetResource(section, entry, (char **)&s, file);
  if (succ)
  {
    *value = (float)strtod(s, NULL);
    delete[] s;
    return TRUE;
  }
  else return FALSE;
}

bool wxGetResource(const wxString& section, const wxString& entry, long *value, const wxString& file)
{
  char *s = NULL;
  bool succ = wxGetResource(section, entry, (char **)&s, file);
  if (succ)
  {
    *value = strtol(s, NULL, 10);
    delete[] s;
    return TRUE;
  }
  else return FALSE;
}

bool wxGetResource(const wxString& section, const wxString& entry, int *value, const wxString& file)
{
  char *s = NULL;
  bool succ = wxGetResource(section, entry, (char **)&s, file);
  if (succ)
  {
    *value = (int)strtol(s, NULL, 10);
    delete[] s; 
    return TRUE;
  }
  else return FALSE;
}
#endif // USE_RESOURCES

// Old cursor
static HCURSOR wxBusyCursorOld = 0;
static int wxBusyCursorCount = 0;

// Set the cursor to the busy cursor for all windows
void wxBeginBusyCursor(wxCursor *cursor)
{
  wxBusyCursorCount ++;
  if (wxBusyCursorCount == 1)
  {
    wxBusyCursorOld = ::SetCursor((HCURSOR) cursor->GetHCURSOR());
  }
  else
  {
    (void)::SetCursor((HCURSOR) cursor->GetHCURSOR());
  }
}

// Restore cursor to normal
void wxEndBusyCursor(void)
{
  if (wxBusyCursorCount == 0)
    return;
    
  wxBusyCursorCount --;
  if (wxBusyCursorCount == 0)
  {
    ::SetCursor(wxBusyCursorOld);
    wxBusyCursorOld = 0;
  }
}

// TRUE if we're between the above two calls
bool wxIsBusy(void)
{
  return (wxBusyCursorCount > 0);
}    

const char* WXDLLEXPORT wxGetHomeDir(wxString *pstr)
{
  wxString& strDir = *pstr;

  #ifdef __UNIX__
    const char *szHome = getenv("HOME");
    if ( szHome == NULL ) {
      // we're homeless...
      wxLogWarning(_("can't find user's HOME, using current directory."));
      strDir = ".";
    }
    else
       strDir = szHome;

    // add a trailing slash if needed
    if ( strDir.Last() != '/' )
      strDir << '/';
  #else   // Windows
    #ifdef  __WIN32__
      const char *szHome = getenv("HOMEDRIVE");
      if ( szHome != NULL )
        strDir << szHome;
      szHome = getenv("HOMEPATH");
      if ( szHome != NULL ) {
        strDir << szHome;

        // the idea is that under NT these variables have default values
        // of "%systemdrive%:" and "\\". As we don't want to create our
        // config files in the root directory of the system drive, we will
        // create it in our program's dir. However, if the user took care
        // to set HOMEPATH to something other than "\\", we suppose that he
        // knows what he is doing and use the supplied value.
        if ( strcmp(szHome, "\\") != 0 )
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
char *wxGetUserHome (const wxString& user)
{
  char *home;
  wxString user1(user);

  if (user1 != "") {
    char tmp[64];
    if (wxGetUserId(tmp, sizeof(tmp)/sizeof(char))) {
      // Guests belong in the temp dir
      if (Stricmp(tmp, "annonymous") == 0) {
        if ((home = getenv("TMP")) != NULL ||
            (home = getenv("TMPDIR")) != NULL ||
            (home = getenv("TEMP")) != NULL)
            return *home ? home : "\\";
      }
      if (Stricmp(tmp, WXSTRINGCAST user1) == 0)
        user1 = "";
    }
  }
  if (user1 == "")
    if ((home = getenv("HOME")) != NULL)
    {
      strcpy(wxBuffer, home);
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
    wxError("wnd==NULL !!!");
    return FALSE;//*** temporary?
  }
}

// MSW only: get user-defined resource from the .res file.
// Returns NULL or newly-allocated memory, so use delete[] to clean up.

#ifdef __WXMSW__
char *wxLoadUserResource(const wxString& resourceName, const wxString& resourceType)
{
  char *s = NULL;
#ifndef __WIN32__
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
  char *theText = (char *)LockResource(hData);
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
bool wxColourDisplay(void)
{
  HDC dc = ::GetDC(NULL);
  bool flag;
  int noCols = GetDeviceCaps(dc, NUMCOLORS);
  if ((noCols == -1) || (noCols > 2))
    flag = TRUE;
  else
    flag = FALSE;
  ReleaseDC(NULL, dc);
  return flag;
}

// Returns depth of screen
int wxDisplayDepth(void)
{
  HDC dc = ::GetDC(NULL);
  int planes = GetDeviceCaps(dc, PLANES);
  int bitsPerPixel = GetDeviceCaps(dc, BITSPIXEL);
  int depth = planes*bitsPerPixel;
  ReleaseDC(NULL, dc);
  return depth;
}

// Get size of display
void wxDisplaySize(int *width, int *height)
{
  HDC dc = ::GetDC(NULL);
  *width = GetDeviceCaps(dc, HORZRES); *height = GetDeviceCaps(dc, VERTRES);
  ReleaseDC(NULL, dc);
}

