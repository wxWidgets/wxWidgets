/////////////////////////////////////////////////////////////////////////////
// Name:        msw/utils.cpp
// Purpose:     Various utilities
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
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
    #include "wx/intl.h"
    #include "wx/log.h"
#if wxUSE_GUI
    #include "wx/cursor.h"
#endif
#endif  //WX_PRECOMP

#include "wx/msw/private.h"     // includes <windows.h>

#ifdef __GNUWIN32_OLD__
    // apparently we need to include winsock.h to get WSADATA and other stuff
    // used in wxGetFullHostName() with the old mingw32 versions
    #include <winsock.h>
#endif

#include "wx/timer.h"

#if !defined(__GNUWIN32__) && !defined(__SALFORDC__) && !defined(__WXMICROWIN__)
    #include <direct.h>

    #ifndef __MWERKS__
        #include <dos.h>
    #endif
#endif  //GNUWIN32

#if defined(__CYGWIN__) && !defined(__TWIN32__)
    #include <sys/unistd.h>
    #include <sys/stat.h>
    #include <sys/cygwin.h> // for cygwin_conv_to_full_win32_path()
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

#if defined(__WIN32__) && !defined(__WXMICROWIN__)
    #ifndef __UNIX__
        #include <io.h>
    #endif

    #ifndef __GNUWIN32__
        #include <shellapi.h>
    #endif
#endif

#ifndef __WATCOMC__
    #if !(defined(_MSC_VER) && (_MSC_VER > 800))
        #include <errno.h>
    #endif
#endif

//// BEGIN for console support: VC++ only
#ifdef __VISUALC__

#include "wx/msw/msvcrt.h"

#include <fcntl.h>

#include "wx/ioswrap.h"

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
#if !defined(__WIN32__) && !defined(__WXMICROWIN__)
static const wxChar eHOSTNAME[]  = wxT("HostName");
static const wxChar eUSERID[]    = wxT("UserId");
#endif // !Win32

#ifndef __WXMICROWIN__

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// get host name and related
// ----------------------------------------------------------------------------

// Get hostname only (without domain name)
bool wxGetHostName(wxChar *buf, int maxSize)
{
#if defined(__WIN32__) && !defined(__TWIN32__) && !defined(__WXMICROWIN__)
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
#if defined(__WIN32__) && !defined(__TWIN32__) && !defined(__WXMICROWIN__) && ! (defined(__GNUWIN32__) && !defined(__MINGW32__))
    // TODO should use GetComputerNameEx() when available

    // the idea is that if someone had set wxUSE_SOCKETS to 0 the code
    // shouldn't use winsock.dll (a.k.a. ws2_32.dll) at all so only use this
    // code if we link with it anyhow
#if wxUSE_SOCKETS
    //FIXME __DIGITALMARS__ why is using sockets not properly turned off in setup.h ??
    // find which library should contain sockets for Digital Mars CE April 2003
#ifndef __DIGITALMARS__
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
                    pHostEnt = gethostbyaddr(pHostEnt->h_addr, 4, AF_INET);
                }

                if ( pHostEnt )
                {
                    host = wxString::FromAscii(pHostEnt->h_name);
                }
            }
        }

        WSACleanup();

        if ( !host.empty() )
        {
            wxStrncpy(buf, host, maxSize);

            return TRUE;
        }
    }
#endif //__DIGITALMARS__

#endif // wxUSE_SOCKETS

#endif // Win32

    return wxGetHostName(buf, maxSize);
}

// Get user ID e.g. jacs
bool wxGetUserId(wxChar *buf, int maxSize)
{
#if defined(__WIN32__) && !defined(__win32s__) && !defined(__TWIN32__) && !defined(__WXMICROWIN__)
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

    #ifdef __CYGWIN__
      // Cygwin returns unix type path but that does not work well
      static wxChar windowsPath[MAX_PATH];
      cygwin_conv_to_full_win32_path(strDir, windowsPath);
      strDir = windowsPath;
    #endif
  #else   // Windows
    #ifdef  __WIN32__
      strDir.clear();

      // If we have a valid HOME directory, as is used on many machines that
      // have unix utilities on them, we should use that.
      const wxChar *szHome = wxGetenv(wxT("HOME"));

      if ( szHome != NULL )
      {
        strDir = szHome;
      }
      else // no HOME, try HOMEDRIVE/PATH
      {
          szHome = wxGetenv(wxT("HOMEDRIVE"));
          if ( szHome != NULL )
            strDir << szHome;
          szHome = wxGetenv(wxT("HOMEPATH"));

          if ( szHome != NULL )
          {
            strDir << szHome;

            // the idea is that under NT these variables have default values
            // of "%systemdrive%:" and "\\". As we don't want to create our
            // config files in the root directory of the system drive, we will
            // create it in our program's dir. However, if the user took care
            // to set HOMEPATH to something other than "\\", we suppose that he
            // knows what he is doing and use the supplied value.
            if ( wxStrcmp(szHome, wxT("\\")) == 0 )
              strDir.clear();
          }
      }

      if ( strDir.empty() )
      {
          // If we have a valid USERPROFILE directory, as is the case in
          // Windows NT, 2000 and XP, we should use that as our home directory.
          szHome = wxGetenv(wxT("USERPROFILE"));

          if ( szHome != NULL )
            strDir = szHome;
      }

      if ( !strDir.empty() )
      {
          return strDir.c_str();
      }
      //else: fall back to the prograrm directory
    #else   // Win16
      // Win16 has no idea about home, so use the executable directory instead
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

wxChar *wxGetUserHome(const wxString& WXUNUSED(user))
{
    // VZ: the old code here never worked for user != "" anyhow! Moreover, it
    //     returned sometimes a malloc()'d pointer, sometimes a pointer to a
    //     static buffer and sometimes I don't even know what.
    static wxString s_home;

    return (wxChar *)wxGetHomeDir(&s_home);
}

bool wxDirExists(const wxString& dir)
{
#ifdef __WXMICROWIN__
    return wxPathExist(dir);
#elif defined(__WIN32__)
    DWORD attribs = GetFileAttributes(dir);
    return ((attribs != (DWORD)-1) && (attribs & FILE_ATTRIBUTE_DIRECTORY));
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

bool wxGetDiskSpace(const wxString& path, wxLongLong *pTotal, wxLongLong *pFree)
{
    if ( path.empty() )
        return FALSE;

// old w32api don't have ULARGE_INTEGER
#if defined(__WIN32__) && \
    (!defined(__GNUWIN32__) || wxCHECK_W32API_VERSION( 0, 3 ))
    // GetDiskFreeSpaceEx() is not available under original Win95, check for
    // it
    typedef BOOL (WINAPI *GetDiskFreeSpaceEx_t)(LPCTSTR,
                                                PULARGE_INTEGER,
                                                PULARGE_INTEGER,
                                                PULARGE_INTEGER);

    GetDiskFreeSpaceEx_t
        pGetDiskFreeSpaceEx = (GetDiskFreeSpaceEx_t)::GetProcAddress
                              (
                                ::GetModuleHandle(_T("kernel32.dll")),
#if wxUSE_UNICODE
                                "GetDiskFreeSpaceExW"
#else
                                "GetDiskFreeSpaceExA"
#endif
                              );

    if ( pGetDiskFreeSpaceEx )
    {
        ULARGE_INTEGER bytesFree, bytesTotal;

        // may pass the path as is, GetDiskFreeSpaceEx() is smart enough
        if ( !pGetDiskFreeSpaceEx(path,
                                  &bytesFree,
                                  &bytesTotal,
                                  NULL) )
        {
            wxLogLastError(_T("GetDiskFreeSpaceEx"));

            return FALSE;
        }

        // ULARGE_INTEGER is a union of a 64 bit value and a struct containing
        // two 32 bit fields which may be or may be not named - try to make it
        // compile in all cases
#if defined(__BORLANDC__) && !defined(_ANONYMOUS_STRUCT)
        #define UL(ul) ul.u
#else // anon union
        #define UL(ul) ul
#endif
        if ( pTotal )
        {
            *pTotal = wxLongLong(UL(bytesTotal).HighPart, UL(bytesTotal).LowPart);
        }

        if ( pFree )
        {
            *pFree = wxLongLong(UL(bytesFree).HighPart, UL(bytesFree).LowPart);
        }
    }
    else
#endif // Win32
    {
        // there's a problem with drives larger than 2GB, GetDiskFreeSpaceEx()
        // should be used instead - but if it's not available, fall back on
        // GetDiskFreeSpace() nevertheless...

        DWORD lSectorsPerCluster,
              lBytesPerSector,
              lNumberOfFreeClusters,
              lTotalNumberOfClusters;

        // FIXME: this is wrong, we should extract the root drive from path
        //        instead, but this is the job for wxFileName...
        if ( !::GetDiskFreeSpace(path,
                                 &lSectorsPerCluster,
                                 &lBytesPerSector,
                                 &lNumberOfFreeClusters,
                                 &lTotalNumberOfClusters) )
        {
            wxLogLastError(_T("GetDiskFreeSpace"));

            return FALSE;
        }

        wxLongLong lBytesPerCluster = lSectorsPerCluster;
        lBytesPerCluster *= lBytesPerSector;

        if ( pTotal )
        {
            *pTotal = lBytesPerCluster;
            *pTotal *= lTotalNumberOfClusters;
        }

        if ( pFree )
        {
            *pFree = lBytesPerCluster;
            *pFree *= lNumberOfFreeClusters;
        }
    }

    return TRUE;
}

// ----------------------------------------------------------------------------
// env vars
// ----------------------------------------------------------------------------

bool wxGetEnv(const wxString& var, wxString *value)
{
#ifdef __WIN16__
    const wxChar* ret = wxGetenv(var);
    if ( !ret )
        return FALSE;

    if ( value )
    {
        *value = ret;
    }

    return TRUE;
#else // Win32
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
#endif // Win16/32
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

// structure used to pass parameters from wxKill() to wxEnumFindByPidProc()
struct wxFindByPidParams
{
    wxFindByPidParams() { hwnd = 0; pid = 0; }

    // the HWND used to return the result
    HWND hwnd;

    // the PID we're looking from
    DWORD pid;

    DECLARE_NO_COPY_CLASS(wxFindByPidParams)
};

// wxKill helper: EnumWindows() callback which is used to find the first (top
// level) window belonging to the given process
BOOL CALLBACK wxEnumFindByPidProc(HWND hwnd, LPARAM lParam)
{
    DWORD pid;
    (void)::GetWindowThreadProcessId(hwnd, &pid);

    wxFindByPidParams *params = (wxFindByPidParams *)lParam;
    if ( pid == params->pid )
    {
        // remember the window we found
        params->hwnd = hwnd;

        // return FALSE to stop the enumeration
        return FALSE;
    }

    // continue enumeration
    return TRUE;
}

int wxKill(long pid, wxSignal sig, wxKillError *krc)
{
    // get the process handle to operate on
    HANDLE hProcess = ::OpenProcess(SYNCHRONIZE |
                                    PROCESS_TERMINATE |
                                    PROCESS_QUERY_INFORMATION,
                                    FALSE, // not inheritable
                                    (DWORD)pid);
    if ( hProcess == NULL )
    {
        if ( krc )
        {
            if ( ::GetLastError() == ERROR_ACCESS_DENIED )
            {
                *krc = wxKILL_ACCESS_DENIED;
            }
            else
            {
                *krc = wxKILL_NO_PROCESS;
            }
        }

        return -1;
    }

    bool ok = TRUE;
    switch ( sig )
    {
        case wxSIGKILL:
            // kill the process forcefully returning -1 as error code
            if ( !::TerminateProcess(hProcess, (UINT)-1) )
            {
                wxLogSysError(_("Failed to kill process %d"), pid);

                if ( krc )
                {
                    // this is not supposed to happen if we could open the
                    // process
                    *krc = wxKILL_ERROR;
                }

                ok = FALSE;
            }
            break;

        case wxSIGNONE:
            // do nothing, we just want to test for process existence
            break;

        default:
            // any other signal means "terminate"
            {
                wxFindByPidParams params;
                params.pid = (DWORD)pid;

                // EnumWindows() has nice semantics: it returns 0 if it found
                // something or if an error occured and non zero if it
                // enumerated all the window
                if ( !::EnumWindows(wxEnumFindByPidProc, (LPARAM)&params) )
                {
                    // did we find any window?
                    if ( params.hwnd )
                    {
                        // tell the app to close
                        //
                        // NB: this is the harshest way, the app won't have
                        //     opportunity to save any files, for example, but
                        //     this is probably what we want here. If not we
                        //     can also use SendMesageTimeout(WM_CLOSE)
                        if ( !::PostMessage(params.hwnd, WM_QUIT, 0, 0) )
                        {
                            wxLogLastError(_T("PostMessage(WM_QUIT)"));
                        }
                    }
                    else // it was an error then
                    {
                        wxLogLastError(_T("EnumWindows"));

                        ok = FALSE;
                    }
                }
                else // no windows for this PID
                {
                    if ( krc )
                    {
                        *krc = wxKILL_ERROR;
                    }

                    ok = FALSE;
                }
            }
    }

    // the return code
    DWORD rc;

    if ( ok )
    {
        // as we wait for a short time, we can use just WaitForSingleObject()
        // and not MsgWaitForMultipleObjects()
        switch ( ::WaitForSingleObject(hProcess, 500 /* msec */) )
        {
            case WAIT_OBJECT_0:
                // process terminated
                if ( !::GetExitCodeProcess(hProcess, &rc) )
                {
                    wxLogLastError(_T("GetExitCodeProcess"));
                }
                break;

            default:
                wxFAIL_MSG( _T("unexpected WaitForSingleObject() return") );
                // fall through

            case WAIT_FAILED:
                wxLogLastError(_T("WaitForSingleObject"));
                // fall through

            case WAIT_TIMEOUT:
                if ( krc )
                {
                    *krc = wxKILL_ERROR;
                }

                rc = STILL_ACTIVE;
                break;
        }
    }
    else // !ok
    {
        // just to suppress the warnings about uninitialized variable
        rc = 0;
    }

    ::CloseHandle(hProcess);

    // the return code is the same as from Unix kill(): 0 if killed
    // successfully or -1 on error
    //
    // be careful to interpret rc correctly: for wxSIGNONE we return success if
    // the process exists, for all the other sig values -- if it doesn't
    if ( ok &&
            ((sig == wxSIGNONE) == (rc == STILL_ACTIVE)) )
    {
        if ( krc )
        {
            *krc = wxKILL_OK;
        }

        return 0;
    }

    // error
    return -1;
}

// Execute a program in an Interactive Shell
bool wxShell(const wxString& command)
{
    wxChar *shell = wxGetenv(wxT("COMSPEC"));
    if ( !shell )
        shell = (wxChar*) wxT("\\COMMAND.COM");

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

    return wxExecute(cmd, wxEXEC_SYNC) == 0;
}

// Shutdown or reboot the PC
bool wxShutdown(wxShutdownFlags wFlags)
{
#ifdef __WIN32__
    bool bOK = TRUE;

    if ( wxGetOsVersion(NULL, NULL) == wxWINDOWS_NT ) // if is NT or 2K
    {
        // Get a token for this process.
        HANDLE hToken;
        bOK = ::OpenProcessToken(GetCurrentProcess(),
                                 TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY,
                                 &hToken) != 0;
        if ( bOK )
        {
            TOKEN_PRIVILEGES tkp;

            // Get the LUID for the shutdown privilege.
            ::LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME,
                                   &tkp.Privileges[0].Luid);

            tkp.PrivilegeCount = 1;  // one privilege to set
            tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

            // Get the shutdown privilege for this process.
            ::AdjustTokenPrivileges(hToken, FALSE, &tkp, 0,
                                    (PTOKEN_PRIVILEGES)NULL, 0);

            // Cannot test the return value of AdjustTokenPrivileges.
            bOK = ::GetLastError() == ERROR_SUCCESS;
        }
    }

    if ( bOK )
    {
        UINT flags = EWX_SHUTDOWN | EWX_FORCE;
        switch ( wFlags )
        {
            case wxSHUTDOWN_POWEROFF:
                flags |= EWX_POWEROFF;
                break;

            case wxSHUTDOWN_REBOOT:
                flags |= EWX_REBOOT;
                break;

            default:
                wxFAIL_MSG( _T("unknown wxShutdown() flag") );
                return FALSE;
        }

        bOK = ::ExitWindowsEx(EWX_SHUTDOWN | EWX_FORCE | EWX_REBOOT, 0) != 0;
    }

    return bOK;
#else // Win16
    return FALSE;
#endif // Win32/16
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

unsigned long wxGetProcessId()
{
#ifdef __WIN32__
    return ::GetCurrentProcessId();
#else
    return 0;
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
#if defined(__WIN32__) 
    static int ver = -1, major = -1, minor = -1;

    if ( ver == -1 )
    {
        OSVERSIONINFO info;
        wxZeroMemory(info);

        ver = wxWINDOWS;
        info.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
        if ( ::GetVersionEx(&info) )
        {
            major = info.dwMajorVersion;
            minor = info.dwMinorVersion;

            switch ( info.dwPlatformId )
            {
                case VER_PLATFORM_WIN32s:
                    ver = wxWIN32S;
                    break;

                case VER_PLATFORM_WIN32_WINDOWS:
                    ver = wxWIN95;
                    break;

                case VER_PLATFORM_WIN32_NT:
                    ver = wxWINDOWS_NT;
                    break;
            }
        }
    }

    if (majorVsn && major != -1)
        *majorVsn = major;
    if (minorVsn && minor != -1)
        *minorVsn = minor;

    return ver;
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

#if wxUSE_TIMER

// Sleep for nSecs seconds. Attempt a Windows implementation using timers.
static bool gs_inTimer = FALSE;

class wxSleepTimer : public wxTimer
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
#else // !Win32
    if (gs_inTimer)
        return;
    if (miliseconds <= 0)
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
#endif // Win32/!Win32
}

void wxSleep(int nSecs)
{
    if (gs_inTimer)
        return;
    if (nSecs <= 0)
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

#endif // wxUSE_TIMER

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
#endif // __WXMICROWIN__

// ----------------------------------------------------------------------------
// deprecated (in favour of wxLog) log functions
// ----------------------------------------------------------------------------

#if WXWIN_COMPATIBILITY_2_2

// Output a debug mess., in a system dependent fashion.
#ifndef __WXMICROWIN__
void wxDebugMsg(const wxChar *fmt ...)
{
  va_list ap;
  static wxChar buffer[512];

  if (!wxTheApp->GetWantDebugOutput())
    return;

  va_start(ap, fmt);

  wvsprintf(buffer,fmt,ap);
  OutputDebugString((LPCTSTR)buffer);

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
#endif // __WXMICROWIN__

#endif // WXWIN_COMPATIBILITY_2_2

#if wxUSE_GUI

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

#ifdef __DIGITALMARS__
extern "C" HCURSOR wxGetCurrentBusyCursor()
#else
extern HCURSOR wxGetCurrentBusyCursor()
#endif
{
    return gs_wxBusyCursor;
}

// Set the cursor to the busy cursor for all windows
void wxBeginBusyCursor(wxCursor *cursor)
{
    if ( gs_wxBusyCursorCount++ == 0 )
    {
        gs_wxBusyCursor = (HCURSOR)cursor->GetHCURSOR();
#ifndef __WXMICROWIN__
        gs_wxBusyCursorOld = ::SetCursor(gs_wxBusyCursor);
#endif
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
#ifndef __WXMICROWIN__
        ::SetCursor(gs_wxBusyCursorOld);
#endif
        gs_wxBusyCursorOld = 0;
    }
}

// TRUE if we're between the above two calls
bool wxIsBusy()
{
  return gs_wxBusyCursorCount > 0;
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

#ifndef __WXMICROWIN__
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
#endif // __WXMICROWIN__

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
#ifdef __WXMICROWIN__
    // MICROWIN_TODO
    return TRUE;
#else
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
#endif
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
#ifdef __WXMICROWIN__
    RECT rect;
    HWND hWnd = GetDesktopWindow();
    ::GetWindowRect(hWnd, & rect);

    if ( width )
        *width = rect.right - rect.left;
    if ( height )
        *height = rect.bottom - rect.top;
#else // !__WXMICROWIN__
    ScreenHDC dc;

    if ( width )
        *width = ::GetDeviceCaps(dc, HORZRES);
    if ( height )
        *height = ::GetDeviceCaps(dc, VERTRES);
#endif // __WXMICROWIN__/!__WXMICROWIN__
}

void wxDisplaySizeMM(int *width, int *height)
{
#ifdef __WXMICROWIN__
    // MICROWIN_TODO
    if ( width )
        *width = 0;
    if ( height )
        *height = 0;
#else
    ScreenHDC dc;

    if ( width )
        *width = ::GetDeviceCaps(dc, HORZSIZE);
    if ( height )
        *height = ::GetDeviceCaps(dc, VERTSIZE);
#endif
}

void wxClientDisplayRect(int *x, int *y, int *width, int *height)
{
#if defined(__WIN16__) || defined(__WXMICROWIN__)
    *x = 0; *y = 0;
    wxDisplaySize(width, height);
#else
    // Determine the desktop dimensions minus the taskbar and any other
    // special decorations...
    RECT r;

    SystemParametersInfo(SPI_GETWORKAREA, 0, &r, 0);
    if (x)      *x = r.left;
    if (y)      *y = r.top;
    if (width)  *width = r.right - r.left;
    if (height) *height = r.bottom - r.top;
#endif
}

// ---------------------------------------------------------------------------
// window information functions
// ---------------------------------------------------------------------------

wxString WXDLLEXPORT wxGetWindowText(WXHWND hWnd)
{
    wxString str;

    if ( hWnd )
    {
        int len = GetWindowTextLength((HWND)hWnd) + 1;
        ::GetWindowText((HWND)hWnd, str.GetWriteBuf(len), len);
        str.UngetWriteBuf();
    }

    return str;
}

wxString WXDLLEXPORT wxGetWindowClass(WXHWND hWnd)
{
    wxString str;

    // MICROWIN_TODO
#ifndef __WXMICROWIN__
    if ( hWnd )
    {
        int len = 256; // some starting value

        for ( ;; )
        {
            int count = ::GetClassName((HWND)hWnd, str.GetWriteBuf(len), len);

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
    }
#endif // !__WXMICROWIN__

    return str;
}

WXWORD WXDLLEXPORT wxGetWindowId(WXHWND hWnd)
{
#ifndef __WIN32__
    return (WXWORD)GetWindowWord((HWND)hWnd, GWW_ID);
#else // Win32
    return (WXWORD)GetWindowLong((HWND)hWnd, GWL_ID);
#endif // Win16/32
}

// ----------------------------------------------------------------------------
// Metafile helpers
// ----------------------------------------------------------------------------

extern void PixelToHIMETRIC(LONG *x, LONG *y)
{
    ScreenHDC hdcRef;

    int iWidthMM = GetDeviceCaps(hdcRef, HORZSIZE),
        iHeightMM = GetDeviceCaps(hdcRef, VERTSIZE),
        iWidthPels = GetDeviceCaps(hdcRef, HORZRES),
        iHeightPels = GetDeviceCaps(hdcRef, VERTRES);

    *x *= (iWidthMM * 100);
    *x /= iWidthPels;
    *y *= (iHeightMM * 100);
    *y /= iHeightPels;
}

extern void HIMETRICToPixel(LONG *x, LONG *y)
{
    ScreenHDC hdcRef;

    int iWidthMM = GetDeviceCaps(hdcRef, HORZSIZE),
        iHeightMM = GetDeviceCaps(hdcRef, VERTSIZE),
        iWidthPels = GetDeviceCaps(hdcRef, HORZRES),
        iHeightPels = GetDeviceCaps(hdcRef, VERTRES);

    *x *= iWidthPels;
    *x /= (iWidthMM * 100);
    *y *= iHeightPels;
    *y /= (iHeightMM * 100);
}

#endif // wxUSE_GUI

#ifdef __WXMICROWIN__
int wxGetOsVersion(int *majorVsn, int *minorVsn)
{
    // MICROWIN_TODO
    if (majorVsn) *majorVsn = 0;
    if (minorVsn) *minorVsn = 0;
    return wxUNIX;
}
#endif // __WXMICROWIN__

// ----------------------------------------------------------------------------
// Win32 codepage conversion functions
// ----------------------------------------------------------------------------

#if defined(__WIN32__) && !defined(__WXMICROWIN__)

// wxGetNativeFontEncoding() doesn't exist neither in wxBase nor in wxUniv
#if wxUSE_GUI && !defined(__WXUNIVERSAL__)

#include "wx/fontmap.h"

// VZ: the new version of wxCharsetToCodepage() is more politically correct
//     and should work on other Windows versions as well but the old version is
//     still needed for !wxUSE_FONTMAP || !wxUSE_GUI case

extern long wxEncodingToCodepage(wxFontEncoding encoding)
{
    // translate encoding into the Windows CHARSET
    wxNativeEncodingInfo natveEncInfo;
    if ( !wxGetNativeFontEncoding(encoding, &natveEncInfo) )
        return -1;

    // translate CHARSET to code page
    CHARSETINFO csetInfo;
    if ( !::TranslateCharsetInfo((DWORD *)(DWORD)natveEncInfo.charset,
                                 &csetInfo,
                                 TCI_SRCCHARSET) )
    {
        wxLogLastError(_T("TranslateCharsetInfo(TCI_SRCCHARSET)"));

        return -1;
    }

    return csetInfo.ciACP;
}

#if wxUSE_FONTMAP

extern long wxCharsetToCodepage(const wxChar *name)
{
    // first get the font encoding for this charset
    if ( !name )
        return -1;

    wxFontEncoding enc = wxFontMapper::Get()->CharsetToEncoding(name, FALSE);
    if ( enc == wxFONTENCODING_SYSTEM )
        return -1;

    // the use the helper function
    return wxEncodingToCodepage(enc);
}

#endif // wxUSE_FONTMAP

#endif // wxUSE_GUI

// include old wxCharsetToCodepage() by OK if needed
#if !wxUSE_GUI || !wxUSE_FONTMAP

#include "wx/msw/registry.h"

// this should work if Internet Exploiter is installed
extern long wxCharsetToCodepage(const wxChar *name)
{
    if (!name)
        return GetACP();

    long CP=-1;

    wxString cn(name);
    do {
        wxString path(wxT("MIME\\Database\\Charset\\"));
        path += cn;
        wxRegKey key(wxRegKey::HKCR, path);

        if (!key.Exists()) break;

        // two cases: either there's an AliasForCharset string,
        // or there are Codepage and InternetEncoding dwords.
        // The InternetEncoding gives us the actual encoding,
        // the Codepage just says which Windows character set to
        // use when displaying the data.
        if (key.HasValue(wxT("InternetEncoding")) &&
            key.QueryValue(wxT("InternetEncoding"), &CP)) break;

        // no encoding, see if it's an alias
        if (!key.HasValue(wxT("AliasForCharset")) ||
            !key.QueryValue(wxT("AliasForCharset"), cn)) break;
    } while (1);

    return CP;
}

#endif // !wxUSE_GUI || !wxUSE_FONTMAP

#endif // Win32

