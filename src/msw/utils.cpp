/////////////////////////////////////////////////////////////////////////////
// Name:        msw/utils.cpp
// Purpose:     Various utilities
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
// #pragma implementation "utils.h"   // Note: this is done in utilscmn.cpp now.
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/utils.h"
    #include "wx/app.h"
    #include "wx/cursor.h"
    #include "wx/intl.h"
    #include "wx/log.h"
#endif  //WX_PRECOMP

// In some mingws there is a missing extern "C" int the winsock header,
// so we put it here just to be safe. Note that this must appear _before_
// #include "wx/msw/private.h" which itself includes <windows.h>, as this 
// one in turn includes <winsock.h> unless we define WIN32_LEAN_AND_MEAN.
//
#if defined(__WIN32__) && !defined(__TWIN32__) && ! (defined(__GNUWIN32__) && !defined(__MINGW32__))
extern "C" {
    #include <winsock.h>    // we use socket functions in wxGetFullHostName()
}
#endif

#include "wx/msw/private.h"     // includes <windows.h>

#include "wx/timer.h"

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
#  define new new(__TFILE__,__LINE__)
#  endif

#endif
  // __VISUALC__
/// END for console support

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// In the WIN.INI file
static const wxChar WX_SECTION[] = wxT("wxWindows");
static const wxChar eUSERNAME[]  = wxT("UserName");

// these are only used under Win16
#ifndef __WIN32__
static const wxChar eHOSTNAME[]  = wxT("HostName");
static const wxChar eUSERID[]    = wxT("UserId");
#endif // !Win32

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// get host name and related
// ----------------------------------------------------------------------------

// Get hostname only (without domain name)
bool wxGetHostName(wxChar *buf, int maxSize)
{
#if defined(__WIN32__) && !defined(__TWIN32__)
    DWORD nSize = maxSize;
    if ( !::GetComputerName(buf, &nSize) )
    {
        wxLogLastError(wxT("GetComputerName"));

        return FALSE;
    }

    return TRUE;
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

// get full hostname (with domain name if possible)
bool wxGetFullHostName(wxChar *buf, int maxSize)
{
#if defined(__WIN32__) && !defined(__TWIN32__) && ! (defined(__GNUWIN32__) && !defined(__MINGW32__))
    // TODO should use GetComputerNameEx() when available
    WSADATA wsa;
    if ( WSAStartup(MAKEWORD(1, 1), &wsa) == 0 )
    {
        wxString host;
        char bufA[256];
        if ( gethostname(bufA, WXSIZEOF(bufA)) == 0 )
        {
            // gethostname() won't usually include the DNS domain name, for
            // this we need to work a bit more
            if ( !strchr(bufA, '.') )
            {
                struct hostent *pHostEnt =  gethostbyname(bufA);

                if ( pHostEnt )
                {
                    // Windows will use DNS internally now
                    pHostEnt = gethostbyaddr(pHostEnt->h_addr, 4, PF_INET);
                }

                if ( pHostEnt )
                {
                    host = pHostEnt->h_name;
                }
            }
        }

        WSACleanup();

        if ( !!host )
        {
            wxStrncpy(buf, host, maxSize);

            return TRUE;
        }
    }
#endif // Win32

    return wxGetHostName(buf, maxSize);
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

wxChar *wxGetUserHome(const wxString& user)
{
    // VZ: the old code here never worked for user != "" anyhow! Moreover, it
    //     returned sometimes a malloc()'d pointer, sometimes a pointer to a
    //     static buffer and sometimes I don't even know what.
    static wxString s_home;

    return (wxChar *)wxGetHomeDir(&s_home);
}

bool wxDirExists(const wxString& dir)
{
#if defined(__WIN32__)
    DWORD attribs = GetFileAttributes(dir);
    return ((attribs != -1) && (attribs & FILE_ATTRIBUTE_DIRECTORY));
#else // Win16
    #ifdef __BORLANDC__
        struct ffblk fileInfo;
    #else
        struct find_t fileInfo;
    #endif
    // In Borland findfirst has a different argument
    // ordering from _dos_findfirst. But _dos_findfirst
    // _should_ be ok in both MS and Borland... why not?
    #ifdef __BORLANDC__
        return (findfirst(dir, &fileInfo, _A_SUBDIR) == 0 &&
               (fileInfo.ff_attrib & _A_SUBDIR) != 0);
    #else
        return (_dos_findfirst(dir, _A_SUBDIR, &fileInfo) == 0) &&
               ((fileInfo.attrib & _A_SUBDIR) != 0);
    #endif
#endif // Win32/16
}

// ----------------------------------------------------------------------------
// env vars
// ----------------------------------------------------------------------------

bool wxGetEnv(const wxString& var, wxString *value)
{
    // first get the size of the buffer
    DWORD dwRet = ::GetEnvironmentVariable(var, NULL, 0);
    if ( !dwRet )
    {
        // this means that there is no such variable
        return FALSE;
    }

    if ( value )
    {
        (void)::GetEnvironmentVariable(var, value->GetWriteBuf(dwRet), dwRet);
        value->UngetWriteBuf();
    }

    return TRUE;
}

bool wxSetEnv(const wxString& var, const wxChar *value)
{
    // some compilers have putenv() or _putenv() or _wputenv() but it's better
    // to always use Win32 function directly instead of dealing with them
#if defined(__WIN32__)
    if ( !::SetEnvironmentVariable(var, value) )
    {
        wxLogLastError(_T("SetEnvironmentVariable"));

        return FALSE;
    }

    return TRUE;
#else // no way to set env vars
    return FALSE;
#endif
}

// ----------------------------------------------------------------------------
// process management
// ----------------------------------------------------------------------------

int wxKill(long pid, int sig)
{
    // TODO use SendMessage(WM_QUIT) and TerminateProcess() if needed

    return 0;
}

// Execute a program in an Interactive Shell
bool wxShell(const wxString& command)
{
    wxChar *shell = wxGetenv(wxT("COMSPEC"));
    if ( !shell )
        shell = wxT("\\COMMAND.COM");

    wxString cmd;
    if ( !command )
    {
        // just the shell
        cmd = shell;
    }
    else
    {
        // pass the command to execute to the command processor
        cmd.Printf(wxT("%s /c %s"), shell, command.c_str());
    }

    return wxExecute(cmd, TRUE /* sync */) != 0;
}

// ----------------------------------------------------------------------------
// misc
// ----------------------------------------------------------------------------

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

// Emit a beeeeeep
void wxBell()
{
    ::MessageBeep((UINT)-1);        // default sound
}

wxString wxGetOsDescription()
{
#ifdef __WIN32__
    wxString str;

    OSVERSIONINFO info;
    wxZeroMemory(info);

    info.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
    if ( ::GetVersionEx(&info) )
    {
        switch ( info.dwPlatformId )
        {
            case VER_PLATFORM_WIN32s:
                str = _("Win32s on Windows 3.1");
                break;

            case VER_PLATFORM_WIN32_WINDOWS:
                str.Printf(_("Windows 9%c"),
                           info.dwMinorVersion == 0 ? _T('5') : _T('8'));
                if ( !wxIsEmpty(info.szCSDVersion) )
                {
                    str << _T(" (") << info.szCSDVersion << _T(')');
                }
                break;

            case VER_PLATFORM_WIN32_NT:
                str.Printf(_T("Windows NT %lu.%lu (build %lu"),
                           info.dwMajorVersion,
                           info.dwMinorVersion,
                           info.dwBuildNumber);
                if ( !wxIsEmpty(info.szCSDVersion) )
                {
                    str << _T(", ") << info.szCSDVersion;
                }
                str << _T(')');
                break;
        }
    }
    else
    {
        wxFAIL_MSG( _T("GetVersionEx() failed") ); // should never happen
    }

    return str;
#else // Win16
    return _("Windows 3.1");
#endif // Win32/16
}

int wxGetOsVersion(int *majorVsn, int *minorVsn)
{
#if defined(__WIN32__) && !defined(__SC__)
    OSVERSIONINFO info;
    wxZeroMemory(info);

    info.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
    if ( ::GetVersionEx(&info) )
    {
        if (majorVsn)
            *majorVsn = info.dwMajorVersion;
        if (minorVsn)
            *minorVsn = info.dwMinorVersion;
        
        switch ( info.dwPlatformId )
        {
            case VER_PLATFORM_WIN32s:
                return wxWIN32S;

            case VER_PLATFORM_WIN32_WINDOWS:
                return wxWIN95;

            case VER_PLATFORM_WIN32_NT:
                return wxWINDOWS_NT;
        }
    }

    return wxWINDOWS; // error if we get here, return generic value
#else // Win16
    int retValue = wxWINDOWS;
    #ifdef __WINDOWS_386__
        retValue = wxWIN386;
    #else
        #if !defined(__WATCOMC__) && !defined(GNUWIN32) && wxUSE_PENWINDOWS
            extern HANDLE g_hPenWin;
            retValue = g_hPenWin ? wxPENWINDOWS : wxWINDOWS;
        #endif
    #endif

    if (majorVsn)
        *majorVsn = 3;
    if (minorVsn)
        *minorVsn = 1;

    return retValue;
#endif
}

// ----------------------------------------------------------------------------
// sleep functions
// ----------------------------------------------------------------------------

#if wxUSE_GUI

// Sleep for nSecs seconds. Attempt a Windows implementation using timers.
static bool gs_inTimer = FALSE;

class wxSleepTimer: public wxTimer
{
public:
    virtual void Notify()
    {
        gs_inTimer = FALSE;
        Stop();
    }
};

static wxTimer *wxTheSleepTimer = NULL;

void wxUsleep(unsigned long milliseconds)
{
#ifdef __WIN32__
    ::Sleep(milliseconds);
#else
    if (gs_inTimer)
        return;

    wxTheSleepTimer = new wxSleepTimer;
    gs_inTimer = TRUE;
    wxTheSleepTimer->Start(milliseconds);
    while (gs_inTimer)
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
    if (gs_inTimer)
        return;

    wxTheSleepTimer = new wxSleepTimer;
    gs_inTimer = TRUE;
    wxTheSleepTimer->Start(nSecs*1000);
    while (gs_inTimer)
    {
        if (wxTheApp->Pending())
            wxTheApp->Dispatch();
    }
    delete wxTheSleepTimer;
    wxTheSleepTimer = NULL;
}

// Consume all events until no more left
void wxFlushEvents()
{
//  wxYield();
}

#elif defined(__WIN32__) // wxUSE_GUI

void wxUsleep(unsigned long milliseconds)
{
    ::Sleep(milliseconds);
}

void wxSleep(int nSecs)
{
    wxUsleep(1000*nSecs);
}

#endif // wxUSE_GUI/!wxUSE_GUI

// ----------------------------------------------------------------------------
// deprecated (in favour of wxLog) log functions
// ----------------------------------------------------------------------------

#if wxUSE_GUI

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

// ----------------------------------------------------------------------------
// functions to work with .INI files
// ----------------------------------------------------------------------------

// Reading and writing resources (eg WIN.INI, .Xdefaults)
#if wxUSE_RESOURCES
bool wxWriteResource(const wxString& section, const wxString& entry, const wxString& value, const wxString& file)
{
  if (file != wxT(""))
// Don't know what the correct cast should be, but it doesn't
// compile in BC++/16-bit without this cast.
#if !defined(__WIN32__)
    return (WritePrivateProfileString((const char*) section, (const char*) entry, (const char*) value, (const char*) file) != 0);
#else
    return (WritePrivateProfileString((LPCTSTR)WXSTRINGCAST section, (LPCTSTR)WXSTRINGCAST entry, (LPCTSTR)value, (LPCTSTR)WXSTRINGCAST file) != 0);
#endif
  else
    return (WriteProfileString((LPCTSTR)WXSTRINGCAST section, (LPCTSTR)WXSTRINGCAST entry, (LPCTSTR)WXSTRINGCAST value) != 0);
}

bool wxWriteResource(const wxString& section, const wxString& entry, float value, const wxString& file)
{
    wxString buf;
    buf.Printf(wxT("%.4f"), value);

    return wxWriteResource(section, entry, buf, file);
}

bool wxWriteResource(const wxString& section, const wxString& entry, long value, const wxString& file)
{
    wxString buf;
    buf.Printf(wxT("%ld"), value);

    return wxWriteResource(section, entry, buf, file);
}

bool wxWriteResource(const wxString& section, const wxString& entry, int value, const wxString& file)
{
    wxString buf;
    buf.Printf(wxT("%d"), value);

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

static HCURSOR gs_wxBusyCursor = 0;     // new, busy cursor
static HCURSOR gs_wxBusyCursorOld = 0;  // old cursor
static int gs_wxBusyCursorCount = 0;

extern HCURSOR wxGetCurrentBusyCursor()
{
    return gs_wxBusyCursor;
}

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

// Check whether this window wants to process messages, e.g. Stop button
// in long calculations.
bool wxCheckForInterrupt(wxWindow *wnd)
{
    wxCHECK( wnd, FALSE );

    MSG msg;
    while ( ::PeekMessage(&msg, GetHwndOf(wnd), 0, 0, PM_REMOVE) )
    {
        ::TranslateMessage(&msg);
        ::DispatchMessage(&msg);
    }

    return TRUE;
}

// MSW only: get user-defined resource from the .res file.
// Returns NULL or newly-allocated memory, so use delete[] to clean up.

wxChar *wxLoadUserResource(const wxString& resourceName, const wxString& resourceType)
{
    HRSRC hResource = ::FindResource(wxGetInstance(), resourceName, resourceType);
    if ( hResource == 0 )
        return NULL;

    HGLOBAL hData = ::LoadResource(wxGetInstance(), hResource);
    if ( hData == 0 )
        return NULL;

    wxChar *theText = (wxChar *)::LockResource(hData);
    if ( !theText )
        return NULL;

    // Not all compilers put a zero at the end of the resource (e.g. BC++ doesn't).
    // so we need to find the length of the resource.
    int len = ::SizeofResource(wxGetInstance(), hResource);
    wxChar  *s = new wxChar[len+1];
    wxStrncpy(s,theText,len);
    s[len]=0;

    // wxChar *s = copystring(theText);

    // Obsolete in WIN32
#ifndef __WIN32__
    UnlockResource(hData);
#endif

    // No need??
    //  GlobalFree(hData);

    return s;
}

// ----------------------------------------------------------------------------
// get display info
// ----------------------------------------------------------------------------

// See also the wxGetMousePosition in window.cpp
// Deprecated: use wxPoint wxGetMousePosition() instead
void wxGetMousePosition( int* x, int* y )
{
    POINT pt;
    GetCursorPos( & pt );
    if ( x ) *x = pt.x;
    if ( y ) *y = pt.y;
};

// Return TRUE if we have a colour display
bool wxColourDisplay()
{
    // this function is called from wxDC ctor so it is called a *lot* of times
    // hence we optimize it a bit but doign the check only once
    //
    // this should be MT safe as only the GUI thread (holding the GUI mutex)
    // can call us
    static int s_isColour = -1;

    if ( s_isColour == -1 )
    {
        ScreenHDC dc;
        int noCols = ::GetDeviceCaps(dc, NUMCOLORS);

        s_isColour = (noCols == -1) || (noCols > 2);
    }

    return s_isColour != 0;
}

// Returns depth of screen
int wxDisplayDepth()
{
    ScreenHDC dc;
    return GetDeviceCaps(dc, PLANES) * GetDeviceCaps(dc, BITSPIXEL);
}

// Get size of display
void wxDisplaySize(int *width, int *height)
{
    ScreenHDC dc;

    if ( width ) *width = GetDeviceCaps(dc, HORZRES);
    if ( height ) *height = GetDeviceCaps(dc, VERTRES);
}

void wxDisplaySizeMM(int *width, int *height)
{
    ScreenHDC dc;

    if ( width ) *width = GetDeviceCaps(dc, HORZSIZE);
    if ( height ) *height = GetDeviceCaps(dc, VERTSIZE);
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

#endif // wxUSE_GUI

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

#endif // 0

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


