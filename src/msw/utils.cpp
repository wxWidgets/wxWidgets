/////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/utils.cpp
// Purpose:     Various utilities
// Author:      Julian Smart
// Created:     04/01/98
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#ifndef WX_PRECOMP
    #include "wx/utils.h"
    #include "wx/app.h"
    #include "wx/intl.h"
    #include "wx/log.h"
#endif  //WX_PRECOMP

#include "wx/msw/registry.h"
#include "wx/apptrait.h"
#include "wx/dynlib.h"
#include "wx/dynload.h"
#include "wx/scopeguard.h"
#include "wx/filename.h"
#include "wx/fontenc.h"

#include "wx/confbase.h"        // for wxExpandEnvVars()

#include "wx/msw/private.h"     // includes <windows.h>
#include "wx/msw/private/hiddenwin.h"
#include "wx/msw/missing.h"     // for CHARSET_HANGUL

#if defined(__CYGWIN__)
    //CYGWIN gives annoying warning about runtime stuff if we don't do this
#   define USE_SYS_TYPES_FD_SET
#   include <sys/types.h>
#endif

#include <wx/dynlib.h>
#include <lm.h>

// Doesn't work with Cygwin at present
#if wxUSE_SOCKETS && (defined(__CYGWIN32__))
    // apparently we need to include winsock.h to get WSADATA and other stuff
    // used in wxGetFullHostName() with the old mingw32 versions
    #include <winsock.h>
#endif

#if !defined(__GNUWIN32__)
    #include <direct.h>

    #include <dos.h>
#endif  //GNUWIN32

#if defined(__CYGWIN__)
    #include <sys/unistd.h>
    #include <sys/stat.h>
    #include <sys/cygwin.h> // for cygwin_conv_path()
    // and cygwin_conv_to_full_win32_path()
    #include <cygwin/version.h>
#endif  //GNUWIN32

#ifndef __UNIX__
    #include <io.h>
#endif

#ifndef __GNUWIN32__
    #include <shellapi.h>
#endif

#ifndef PROCESSOR_ARCHITECTURE_ARM64
#define PROCESSOR_ARCHITECTURE_ARM64 12
#endif

#ifndef IMAGE_FILE_MACHINE_ARM64
#define IMAGE_FILE_MACHINE_ARM64 0xAA64
#endif

#ifndef IMAGE_FILE_MACHINE_ARMNT
#define IMAGE_FILE_MACHINE_ARMNT 0x01c4
#endif

#include <errno.h>

// For wxKillAllChildren
#include <tlhelp32.h>

// For wxCmpNatural()
#include <shlwapi.h>

// In some distributions of MinGW32, this function is exported in the library,
// but not declared in shlwapi.h. Therefore we declare it here.
#if defined( __MINGW32_TOOLCHAIN__ )
    extern "C" __declspec(dllimport) int WINAPI StrCmpLogicalW(LPCWSTR psz1, LPCWSTR psz2);
#endif

// For MSVC we can also link the library containing StrCmpLogicalW()
// directly from here, for the other compilers this needs to be done at
// makefiles level.
#ifdef __VISUALC__
    #pragma comment(lib, "shlwapi")
#endif

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

WXDLLIMPEXP_DATA_BASE(const wxChar *) wxUserResourceStr = wxT("TEXT");

// ============================================================================
// implementation
// ============================================================================

using NetGetAnyDCName_t = NET_API_STATUS(NET_API_FUNCTION*)(
    LPCWSTR ServerName,
    LPCWSTR DomainName,
    LPBYTE* Buffer);
using NetUserGetInfo_t = NET_API_STATUS(NET_API_FUNCTION*)(
    LPCWSTR servername,
    LPCWSTR username,
    DWORD   level,
    LPBYTE* bufptr);
using NetApiBufferFree_t = NET_API_STATUS(NET_API_FUNCTION*)(
    LPVOID Buffer);

// ----------------------------------------------------------------------------
// get host name and related
// ----------------------------------------------------------------------------

// Get hostname only (without domain name)
bool wxGetHostName(wxChar *buf, int maxSize)
{
    DWORD nSize = maxSize;
    if ( !::GetComputerName(buf, &nSize) )
    {
        wxLogLastError(wxT("GetComputerName"));

        return false;
    }

    return true;
}

// get full hostname (with domain name if possible)
bool wxGetFullHostName(wxChar *buf, int maxSize)
{
#if wxUSE_SOCKETS
    // TODO should use GetComputerNameEx() when available

    // we don't want to always link with Winsock DLL as we might not use it at
    // all, so load it dynamically here if needed (and don't complain if it is
    // missing, we handle this)
    wxLogNull noLog;

    wxDynamicLibrary dllWinsock(wxT("ws2_32.dll"), wxDL_VERBATIM);
    if ( dllWinsock.IsLoaded() )
    {
        typedef int (PASCAL *WSAStartup_t)(WORD, WSADATA *);
        typedef int (PASCAL *gethostname_t)(char *, int);
        typedef hostent* (PASCAL *gethostbyname_t)(const char *);
        typedef hostent* (PASCAL *gethostbyaddr_t)(const char *, int , int);
        typedef int (PASCAL *WSACleanup_t)(void);

        #define LOAD_WINSOCK_FUNC(func)                                       \
            func ## _t                                                        \
                pfn ## func = (func ## _t)dllWinsock.GetSymbol(wxT(#func))

        LOAD_WINSOCK_FUNC(WSAStartup);

        WSADATA wsa;
        if ( pfnWSAStartup && pfnWSAStartup(MAKEWORD(1, 1), &wsa) == 0 )
        {
            LOAD_WINSOCK_FUNC(gethostname);

            wxString host;
            if ( pfngethostname )
            {
                char bufA[256];
                if ( pfngethostname(bufA, WXSIZEOF(bufA)) == 0 )
                {
                    // gethostname() won't usually include the DNS domain name,
                    // for this we need to work a bit more
                    if ( !strchr(bufA, '.') )
                    {
                        LOAD_WINSOCK_FUNC(gethostbyname);

                        struct hostent *pHostEnt = pfngethostbyname
                                                    ? pfngethostbyname(bufA)
                                                    : nullptr;

                        if ( pHostEnt )
                        {
                            // Windows will use DNS internally now
                            LOAD_WINSOCK_FUNC(gethostbyaddr);

                            pHostEnt = pfngethostbyaddr
                                        ? pfngethostbyaddr(pHostEnt->h_addr,
                                                           4, AF_INET)
                                        : nullptr;
                        }

                        if ( pHostEnt )
                        {
                            host = pHostEnt->h_name;
                        }
                    }
                }
            }

            LOAD_WINSOCK_FUNC(WSACleanup);
            if ( pfnWSACleanup )
                pfnWSACleanup();


            if ( !host.empty() )
            {
                wxStrlcpy(buf, host.c_str(), maxSize);

                return true;
            }
        }
    }
#endif // wxUSE_SOCKETS

    return wxGetHostName(buf, maxSize);
}

// Get user ID e.g. jacs
bool wxGetUserId(wxChar *buf,
                 int maxSize)
{
    DWORD nSize = maxSize;
    if ( ::GetUserName(buf, &nSize) == 0 )
    {
        // actually, it does happen if the user didn't log on
        DWORD res = ::GetEnvironmentVariable(wxT("username"), buf, maxSize);
        if ( res == 0 )
        {
            // not found
            return false;
        }
    }

    return true;
}

// Get user name (e.g., Julian Smart)
bool wxGetUserName(wxChar* buf, int maxSize)
{
    wxCHECK_MSG(buf && (maxSize > 0), false,
        "Empty buffer in wxGetUserName");

    if ( !wxGetUserId(buf, maxSize) )
        return false;

    /* This code is based on Microsoft Learn's ::NetUserGetInfo example code.
       Attempt to get the full user name; if any of this fails, we can still
       return true with the buffer at least filled with the login name
       from wxGetUserId().

       Note that there is a ::GetUserNameEx function, but that requires
       defining SECURITY_WIN32, which may have other side effects.
       Instead, use the NetAPI functions.*/

    const static wxDynamicLibrary netapi32("netapi32", wxDL_VERBATIM | wxDL_QUIET);
    if ( !netapi32.IsLoaded() )
    {
        wxLogTrace("utils", "Failed to load netapi32.dll");
        return true;
    }

    const static NetGetAnyDCName_t netGetAnyDCName =
        reinterpret_cast<NetGetAnyDCName_t>(netapi32.GetSymbol("NetGetAnyDCName"));
    const static NetUserGetInfo_t netUserGetInfo =
        reinterpret_cast<NetUserGetInfo_t>(netapi32.GetSymbol("NetUserGetInfo"));
    const static NetApiBufferFree_t netApiBufferFree =
        reinterpret_cast<NetApiBufferFree_t>(netapi32.GetSymbol("NetApiBufferFree"));

    if ( netGetAnyDCName == nullptr ||
         netUserGetInfo == nullptr ||
         netApiBufferFree == nullptr )
    {
        return true;
    }

    LPBYTE computerName{ nullptr };
    USER_INFO_2* ui2{ nullptr };

    // Get the domain controller for any domain.
    if ( netGetAnyDCName(nullptr, nullptr, &computerName) != NERR_Success )
    {
        // May not be networked, so use the local machine.
        computerName = nullptr;
    }
    // ::NetGetAnyDCName calls ::NetApiBufferAlloc to fill the
    // domain controller name, so need to free that upon exit.
    auto dcBufferFree = std::unique_ptr<BYTE, void(*)(LPBYTE)>
        {
        computerName,
        [](LPBYTE computerName)
            {
                if ( computerName != nullptr )
                    netApiBufferFree(computerName);
            }
        };

    // Look up the user on the DC.
    const NET_API_STATUS status = netUserGetInfo((LPWSTR)computerName,
        (LPWSTR)buf,
        2, // level - we want USER_INFO_2
        (LPBYTE*)&ui2);
    // ::NetUserGetInfo calls ::NetApiBufferAlloc to create the
    // USER_INFO_2 structure, so need to free that upon exit.
    auto uiBufferFree = std::unique_ptr<USER_INFO_2, void(*)(USER_INFO_2*)>
        {
        ui2,
        [](USER_INFO_2* ui2)
            {
                if ( ui2 != nullptr )
                    netApiBufferFree(ui2);
            }
        };

    if ( status != NERR_Success )
    {
        wxLogTrace("utils", "Failed to retrieve full user information.");
        return true;
    }

    if ( ui2 != nullptr &&
         ui2->usri2_full_name != nullptr )
    {
        wxString fullUserName(ui2->usri2_full_name);
        if ( fullUserName.empty() )
        {
            return true;
        }
        // In the case of full name being in the format of "[LAST_NAME], [FIRST_NAME]",
        // reformat it to a more readable "[FIRST_NAME] [LAST_NAME]".
        const size_t commaPosition = fullUserName.find(", ");
        if ( commaPosition != wxString::npos )
            {
            const wxString firstName = fullUserName.substr(commaPosition + 2);
            fullUserName.erase(commaPosition);
            fullUserName.insert(0, firstName + ' ');
            }
        wxStrlcpy(buf, fullUserName.t_str(), maxSize);
    }

    return true;
}

const wxChar* wxGetHomeDir(wxString *pstr)
{
    wxString& strDir = *pstr;

    // first branch is for Cygwin
#if defined(__UNIX__) && !defined(__WINE__)
    const wxChar *szHome = wxGetenv(wxT("HOME"));
    if ( szHome == nullptr ) {
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
        #if CYGWIN_VERSION_DLL_MAJOR >= 1007
            cygwin_conv_path(CCP_POSIX_TO_WIN_W, strDir.c_str(), windowsPath, MAX_PATH);
        #else
            cygwin_conv_to_full_win32_path(strDir.c_str(), windowsPath);
        #endif
        strDir = windowsPath;
    #endif
#else
    strDir.clear();

    // If we have a valid HOME directory, as is used on many machines that
    // have unix utilities on them, we should use that.
    const wxChar *szHome = wxGetenv(wxT("HOME"));

    if ( szHome != nullptr )
    {
        strDir = szHome;
    }
    else // no HOME, try HOMEDRIVE/PATH
    {
        szHome = wxGetenv(wxT("HOMEDRIVE"));
        if ( szHome != nullptr )
            strDir << szHome;
        szHome = wxGetenv(wxT("HOMEPATH"));

        if ( szHome != nullptr )
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

        if ( szHome != nullptr )
            strDir = szHome;
    }

    if ( !strDir.empty() )
    {
        // sometimes the value of HOME may be "%USERPROFILE%", so reexpand the
        // value once again, it shouldn't hurt anyhow
        strDir = wxExpandEnvVars(strDir);
    }
    else // fall back to the program directory
    {
        // extract the directory component of the program file name
        wxFileName::SplitPath(wxGetFullModuleName(), &strDir, nullptr, nullptr);
    }
#endif  // UNIX/Win

    return strDir.c_str();
}

wxString wxGetUserHome(const wxString& user)
{
    wxString home;

    if ( user.empty() || user == wxGetUserId() )
        wxGetHomeDir(&home);

    return home;
}

bool wxGetDiskSpace(const wxString& path,
                    wxDiskspaceSize_t *pTotal,
                    wxDiskspaceSize_t *pFree)
{
    if ( path.empty() )
        return false;

    ULARGE_INTEGER bytesFree, bytesTotal;

    // may pass the path as is, GetDiskFreeSpaceEx() is smart enough
    if ( !::GetDiskFreeSpaceEx(path.t_str(),
                               &bytesFree,
                               &bytesTotal,
                               nullptr) )
    {
        wxLogLastError(wxT("GetDiskFreeSpaceEx"));

        return false;
    }

    if ( pTotal )
    {
        *pTotal = wxDiskspaceSize_t(bytesTotal.HighPart, bytesTotal.LowPart);
    }

    if ( pFree )
    {
        *pFree = wxLongLong(bytesFree.HighPart, bytesFree.LowPart);
    }

    return true;
}

// ----------------------------------------------------------------------------
// env vars
// ----------------------------------------------------------------------------

bool wxGetEnv(const wxString& var,
              wxString *value)
{
    // first get the size of the buffer
    DWORD dwRet = ::GetEnvironmentVariable(var.t_str(), nullptr, 0);
    if ( !dwRet )
    {
        // this means that there is no such variable
        return false;
    }

    if ( value )
    {
        (void)::GetEnvironmentVariable(var.t_str(),
                                       wxStringBuffer(*value, dwRet),
                                       dwRet);
    }

    return true;
}

bool wxDoSetEnv(const wxString& var, const wxChar *value)
{
    // update the CRT environment if possible as people expect getenv() to also
    // work and it is not affected by Win32 SetEnvironmentVariable() call (OTOH
    // the CRT does use Win32 call to update the process environment block so
    // there is no need to call it)
    //
    // TODO: add checks for the other compilers (and update wxSetEnv()
    //       documentation in interface/wx/utils.h accordingly)
#if defined(__VISUALC__) || defined(__MINGW32__)
    // notice that Microsoft _putenv() has different semantics from POSIX
    // function with almost the same name: in particular it makes a copy of the
    // string instead of using it as part of environment so we can safely call
    // it here without going through all the troubles with wxSetEnvModule as in
    // src/unix/utilsunx.cpp
    wxString envstr = var;
    envstr += '=';
    if ( value )
        envstr += value;
    if ( _tputenv(envstr.t_str()) != 0 )
        return false;
#else // other compiler
    if ( !::SetEnvironmentVariable(var.t_str(), value) )
    {
        wxLogLastError(wxT("SetEnvironmentVariable"));

        return false;
    }
#endif // compiler

    return true;
}

bool wxSetEnv(const wxString& variable, const wxString& value)
{
    return wxDoSetEnv(variable, value.t_str());
}

bool wxUnsetEnv(const wxString& variable)
{
    return wxDoSetEnv(variable, nullptr);
}

// ----------------------------------------------------------------------------
// security
// ----------------------------------------------------------------------------

void wxSecureZeroMemory(void* v, size_t n)
{
#if defined(__MINGW32__)
    // A generic implementation based on the example at:
    // http://www.open-std.org/jtc1/sc22/wg14/www/docs/n1381.pdf
    int c = 0;
    volatile unsigned char *p = reinterpret_cast<unsigned char *>(v);
    while ( n-- )
        *p++ = c;
#else
    RtlSecureZeroMemory(v, n);
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

    wxDECLARE_NO_COPY_CLASS(wxFindByPidParams);
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

int wxKillAllChildren(long pid, wxSignal sig, wxKillError *krc);

int wxKill(long pid, wxSignal sig, wxKillError *krc, int flags)
{
    if (flags & wxKILL_CHILDREN)
        wxKillAllChildren(pid, sig, krc);

    // get the process handle to operate on
    DWORD dwAccess = PROCESS_QUERY_INFORMATION | SYNCHRONIZE;
    if ( sig == wxSIGKILL )
        dwAccess |= PROCESS_TERMINATE;

    HANDLE hProcess = ::OpenProcess(dwAccess, FALSE, (DWORD)pid);
    if ( hProcess == nullptr )
    {
        if ( krc )
        {
            // recognize wxKILL_ACCESS_DENIED as special because this doesn't
            // mean that the process doesn't exist and this is important for
            // wxProcess::Exists()
            *krc = ::GetLastError() == ERROR_ACCESS_DENIED
                        ? wxKILL_ACCESS_DENIED
                        : wxKILL_NO_PROCESS;
        }

        return -1;
    }

    wxON_BLOCK_EXIT1(::CloseHandle, hProcess);

    // Default timeout for waiting for the process termination after killing
    // it. It should be long enough to allow the process to terminate even on a
    // busy system but short enough to avoid blocking the main thread for too
    // long.
    DWORD waitTimeout = 500; // ms

    bool ok = true;
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

                ok = false;
            }
            break;

        case wxSIGNONE:
            // Opening the process handle may succeed for a process even if it
            // doesn't run any more (typically because open handles to it still
            // exist elsewhere, possibly in this process itself if we're
            // killing a child process) so we still need check if it hasn't
            // terminated yet but, unlike when killing it, we don't need to
            // wait for any time at all.
            waitTimeout = 0;
            break;

        default:
            // any other signal means "terminate"
            {
                wxFindByPidParams params;
                params.pid = (DWORD)pid;

                // EnumWindows() has nice semantics: it returns 0 if it found
                // something or if an error occurred and non zero if it
                // enumerated all the window
                if ( !::EnumWindows(wxEnumFindByPidProc, (LPARAM)&params) )
                {
                    // did we find any window?
                    if ( params.hwnd )
                    {
                        // tell the app to close
                        //
                        // NB: this is the harshest way, the app won't have an
                        //     opportunity to save any files, for example, but
                        //     this is probably what we want here. If not we
                        //     can also use SendMesageTimeout(WM_CLOSE)
                        if ( !::PostMessage(params.hwnd, WM_QUIT, 0, 0) )
                        {
                            wxLogLastError(wxT("PostMessage(WM_QUIT)"));
                        }
                    }
                    else // it was an error then
                    {
                        wxLogLastError(wxT("EnumWindows"));

                        ok = false;
                    }
                }
                else // no windows for this PID
                {
                    if ( krc )
                        *krc = wxKILL_ERROR;

                    ok = false;
                }
            }
    }

    // the return code
    if ( ok )
    {
        // as we wait for a short time, we can use just WaitForSingleObject()
        // and not MsgWaitForMultipleObjects()
        switch ( ::WaitForSingleObject(hProcess, waitTimeout) )
        {
            case WAIT_OBJECT_0:
                // Process terminated: normally this indicates that we
                // successfully killed it but when testing for the process
                // existence, this means failure.
                if ( sig == wxSIGNONE )
                {
                    if ( krc )
                        *krc = wxKILL_NO_PROCESS;

                    ok = false;
                }
                break;

            default:
                wxFAIL_MSG( wxT("unexpected WaitForSingleObject() return") );
                wxFALLTHROUGH;

            case WAIT_FAILED:
                wxLogLastError(wxT("WaitForSingleObject"));
                wxFALLTHROUGH;

            case WAIT_TIMEOUT:
                // Process didn't terminate: normally this is a failure but not
                // when we're just testing for its existence.
                if ( sig != wxSIGNONE )
                {
                    if ( krc )
                        *krc = wxKILL_ERROR;

                    ok = false;
                }
                break;
        }
    }


    // the return code is the same as from Unix kill(): 0 if killed
    // successfully or -1 on error
    if ( !ok )
        return -1;

    if ( krc )
        *krc = wxKILL_OK;

    return 0;
}

// This is used by wxProcess::Activate().
extern
bool wxMSWActivatePID(long pid)
{
    wxFindByPidParams params;
    params.pid = (DWORD)pid;

    if ( ::EnumWindows(wxEnumFindByPidProc, (LPARAM)&params) != 0 )
    {
        // No windows corresponding to this PID were found.
        return false;
    }

    if ( !::BringWindowToTop(params.hwnd) )
    {
        wxLogLastError(wxS("BringWindowToTop"));
        return false;
    }

    return true;
}

// By John Skiff
int wxKillAllChildren(long pid, wxSignal sig, wxKillError *krc)
{
    if (krc)
        *krc = wxKILL_OK;

    // Take a snapshot of all processes in the system.
    HANDLE hProcessSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hProcessSnap == INVALID_HANDLE_VALUE) {
        if (krc)
            *krc = wxKILL_ERROR;
        return -1;
    }

    //Fill in the size of the structure before using it.
    PROCESSENTRY32 pe;
    wxZeroMemory(pe);
    pe.dwSize = sizeof(PROCESSENTRY32);

    // Walk the snapshot of the processes, and for each process,
    // kill it if its parent is pid.
    if (!::Process32First(hProcessSnap, &pe)) {
        // Can't get first process.
        if (krc)
            *krc = wxKILL_ERROR;
        CloseHandle (hProcessSnap);
        return -1;
    }

    do {
        if (pe.th32ParentProcessID == (DWORD) pid) {
            if (wxKill(pe.th32ProcessID, sig, krc))
                return -1;
        }
    } while (::Process32Next (hProcessSnap, &pe));


    return 0;
}

// Execute a program in an Interactive Shell
bool wxShell(const wxString& command)
{
    wxString cmd;

    const wxChar* shell = wxGetenv(wxT("COMSPEC"));
    if ( !shell )
        shell = wxT("\\COMMAND.COM");

    if ( command.empty() )
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
bool wxShutdown(int flags)
{
    bool bOK = true;

    // Get a token for this process.
    HANDLE hToken;
    bOK = ::OpenProcessToken(GetCurrentProcess(),
                                TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY,
                                &hToken) != 0;
    if ( bOK )
    {
        TOKEN_PRIVILEGES tkp;

        // Get the LUID for the shutdown privilege.
        bOK = ::LookupPrivilegeValue(nullptr, SE_SHUTDOWN_NAME,
                                        &tkp.Privileges[0].Luid) != 0;

        if ( bOK )
        {
            tkp.PrivilegeCount = 1;  // one privilege to set
            tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

            // Get the shutdown privilege for this process.
            ::AdjustTokenPrivileges(hToken, FALSE, &tkp, 0,
                                    nullptr, 0);

            // Cannot test the return value of AdjustTokenPrivileges.
            bOK = ::GetLastError() == ERROR_SUCCESS;
        }

        ::CloseHandle(hToken);
    }

    if ( bOK )
    {
        UINT wFlags = 0;
        if ( flags & wxSHUTDOWN_FORCE )
        {
            wFlags = EWX_FORCE;
            flags &= ~wxSHUTDOWN_FORCE;
        }

        switch ( flags )
        {
            case wxSHUTDOWN_POWEROFF:
                wFlags |= EWX_POWEROFF;
                break;

            case wxSHUTDOWN_REBOOT:
                wFlags |= EWX_REBOOT;
                break;

            case wxSHUTDOWN_LOGOFF:
                wFlags |= EWX_LOGOFF;
                break;

            default:
                wxFAIL_MSG( wxT("unknown wxShutdown() flag") );
                return false;
        }

        bOK = ::ExitWindowsEx(wFlags, 0) != 0;
    }

    return bOK;
}

// ----------------------------------------------------------------------------
// misc
// ----------------------------------------------------------------------------

// Get free memory in bytes, or -1 if cannot determine amount (e.g. on UNIX)
wxMemorySize wxGetFreeMemory()
{
#if defined(__WIN64__)
    MEMORYSTATUSEX memStatex;
    memStatex.dwLength = sizeof (memStatex);
    ::GlobalMemoryStatusEx (&memStatex);
    return (wxMemorySize)memStatex.ullAvailPhys;
#else /* if defined(__WIN32__) */
    MEMORYSTATUS memStatus;
    memStatus.dwLength = sizeof(MEMORYSTATUS);
    ::GlobalMemoryStatus(&memStatus);
    return (wxMemorySize)memStatus.dwAvailPhys;
#endif
}

unsigned long wxGetProcessId()
{
    return ::GetCurrentProcessId();
}

bool wxIsDebuggerRunning()
{
    return ::IsDebuggerPresent() != 0;
}

// ----------------------------------------------------------------------------
// working with MSW resources
// ----------------------------------------------------------------------------

bool
wxLoadUserResource(const void **outData,
                   size_t *outLen,
                   const wxString& resourceName,
                   const wxChar* resourceType,
                   WXHINSTANCE instance)
{
    wxCHECK_MSG( outData && outLen, false, "output pointers can't be null" );

    HRSRC hResource = ::FindResource(instance,
                                     resourceName.t_str(),
                                     resourceType);
    if ( !hResource )
        return false;

    HGLOBAL hData = ::LoadResource(instance, hResource);
    if ( !hData )
    {
        wxLogSysError(_("Failed to load resource \"%s\"."), resourceName);
        return false;
    }

    *outData = ::LockResource(hData);
    if ( !*outData )
    {
        wxLogSysError(_("Failed to lock resource \"%s\"."), resourceName);
        return false;
    }

    *outLen = ::SizeofResource(instance, hResource);

    // Notice that we need to call neither UnlockResource() (which is
    // obsolete in Win32) nor GlobalFree() (resources are freed on process
    // termination only)

    return true;
}

char *
wxLoadUserResource(const wxString& resourceName,
                   const wxChar* resourceType,
                   int* pLen,
                   WXHINSTANCE instance)
{
    const void *data;
    size_t len;
    if ( !wxLoadUserResource(&data, &len, resourceName, resourceType, instance) )
        return nullptr;

    char *s = new char[len + 1];
    memcpy(s, data, len);
    s[len] = '\0'; // NUL-terminate in case the resource itself wasn't

    if (pLen)
      *pLen = len;

    return s;
}

// ----------------------------------------------------------------------------
// OS version
// ----------------------------------------------------------------------------

namespace
{

// Helper trying to get the real Windows version which is needed because
// GetVersionEx() doesn't return it any more since Windows 8.
OSVERSIONINFOEXW wxGetWindowsVersionInfo()
{
    OSVERSIONINFOEXW info;
    wxZeroMemory(info);
    info.dwOSVersionInfoSize = sizeof(info);

    // The simplest way to get the version is to call the kernel
    // RtlGetVersion() directly, if it is available.
    wxDynamicLibrary dllNtDll;
    if ( dllNtDll.Load(wxS("ntdll.dll"), wxDL_VERBATIM | wxDL_QUIET) )
    {
        typedef LONG /* NTSTATUS */ (WINAPI *RtlGetVersion_t)(OSVERSIONINFOEXW*);

        RtlGetVersion_t wxDL_INIT_FUNC(pfn, RtlGetVersion, dllNtDll);
        if ( pfnRtlGetVersion &&
                (pfnRtlGetVersion(&info) == 0 /* STATUS_SUCCESS */) )
        {
            return info;
        }
    }

#ifdef __VISUALC__
    #pragma warning(push)
    #pragma warning(disable:4996) // 'xxx': was declared deprecated
#endif

    if ( !::GetVersionExW(reinterpret_cast<OSVERSIONINFOW *>(&info)) )
    {
        // This really shouldn't ever happen.
        wxFAIL_MSG( "GetVersionEx() unexpectedly failed" );
    }

#ifdef __VISUALC__
    #pragma warning(pop)
#endif

    return info;
}

// check if we're running under a server or workstation Windows system: it
// returns true or false with obvious meaning as well as -1 if the system type
// couldn't be determined
//
// this function is currently private but we may want to expose it later if
// it's really useful

int wxIsWindowsServer()
{
#ifdef VER_NT_WORKSTATION
    switch ( wxGetWindowsVersionInfo().wProductType )
    {
        case VER_NT_WORKSTATION:
            return false;

        case VER_NT_SERVER:
        case VER_NT_DOMAIN_CONTROLLER:
            return true;
    }
#endif // VER_NT_WORKSTATION

    return -1;
}

static const int WINDOWS_SERVER2016_BUILD = 14393;
static const int WINDOWS_SERVER2019_BUILD = 17763;
static const int WINDOWS_SERVER2022_BUILD = 20348;

// Windows 11 uses the same version as Windows 10 but its build numbers start
// from 22000, which provides a way to test for it.
static const int FIRST_WINDOWS11_BUILD = 22000;

} // anonymous namespace

// When adding a new version, update also the table in wxGetOsVersion() docs.
wxString wxGetOsDescription()
{
    wxString str;

    const OSVERSIONINFOEXW info = wxGetWindowsVersionInfo();
    switch ( info.dwPlatformId )
    {
        case VER_PLATFORM_WIN32_NT:
            switch ( info.dwMajorVersion )
            {
                case 5:
                    switch ( info.dwMinorVersion )
                    {
                        case 2:
                            // we can't distinguish between XP 64 and 2003
                            // as they both are 5.2, so examine the product
                            // type to resolve this ambiguity
                            if ( wxIsWindowsServer() == 1 )
                            {
                                str = "Windows Server 2003";
                                break;
                            }
                            //else: must be XP, fall through
                            wxFALLTHROUGH;

                        case 1:
                            str = "Windows XP";
                            break;
                    }
                    break;

                case 6:
                    switch ( info.dwMinorVersion )
                    {
                        case 0:
                            str = wxIsWindowsServer() == 1
                                    ? "Windows Server 2008"
                                    : "Windows Vista";
                            break;

                        case 1:
                            str = wxIsWindowsServer() == 1
                                    ? "Windows Server 2008 R2"
                                    : "Windows 7";
                            break;

                        case 2:
                            str = wxIsWindowsServer() == 1
                                    ? "Windows Server 2012"
                                    : "Windows 8";
                            break;

                        case 3:
                            str = wxIsWindowsServer() == 1
                                    ? "Windows Server 2012 R2"
                                    : "Windows 8.1";
                            break;
                    }
                    break;

                case 10:
                    if ( wxIsWindowsServer() == 1 )
                    {
                        switch ( info.dwBuildNumber )
                        {
                            case WINDOWS_SERVER2016_BUILD:
                                str = "Windows Server 2016";
                                break;
                            case WINDOWS_SERVER2019_BUILD:
                                str = "Windows Server 2019";
                                break;
                            case WINDOWS_SERVER2022_BUILD:
                                str = "Windows Server 2022";
                                break;
                        }
                    }
                    else
                    {
                        str = info.dwBuildNumber >= FIRST_WINDOWS11_BUILD
                            ? "Windows 11"
                            : "Windows 10";
                    }
                    break;
            }

            if ( str.empty() )
            {
                str.Printf("Windows %s%lu.%lu",
                           wxIsWindowsServer() == 1 ? "Server " : "",
                           info.dwMajorVersion,
                           info.dwMinorVersion);
            }

            str << wxT(" (")
                << wxString::Format(_("build %lu"), info.dwBuildNumber);
            if ( !wxIsEmpty(info.szCSDVersion) )
            {
                str << wxT(", ") << info.szCSDVersion;
            }
            str << wxT(')');

            if ( wxIsPlatform64Bit() )
                str << _(", 64-bit edition");
            break;
    }

    return str;
}

bool wxIsPlatform64Bit()
{
#if defined(__WIN64__)
    return true;  // 64-bit programs run only on Win64
#else // Win32
    // 32-bit programs run on both 32-bit and 64-bit Windows so check
    typedef BOOL (WINAPI *IsWow64Process_t)(HANDLE, BOOL *);

    wxDynamicLibrary dllKernel32(wxT("kernel32.dll"));
    IsWow64Process_t pfnIsWow64Process =
        (IsWow64Process_t)dllKernel32.RawGetSymbol(wxT("IsWow64Process"));

    BOOL wow64 = FALSE;
    if ( pfnIsWow64Process )
    {
        pfnIsWow64Process(::GetCurrentProcess(), &wow64);
    }
    //else: running under a system without Win64 support

    return wow64 != FALSE;
#endif // Win64/Win32
}

wxOperatingSystemId wxGetOsVersion(int *verMaj, int *verMin, int *verMicro)
{
    static struct
    {
        bool initialized;

        wxOperatingSystemId os;

        int verMaj,
            verMin,
            verMicro;
    } s_version;

    // query the OS info only once as it's not supposed to change
    if ( !s_version.initialized )
    {
        const OSVERSIONINFOEXW info = wxGetWindowsVersionInfo();

        s_version.initialized = true;

        switch ( info.dwPlatformId )
        {
            case VER_PLATFORM_WIN32_NT:
                s_version.os = wxOS_WINDOWS_NT;
        }

        s_version.verMaj = info.dwMajorVersion;
        s_version.verMin = info.dwMinorVersion;
        s_version.verMicro = info.dwBuildNumber;
    }

    if ( verMaj )
        *verMaj = s_version.verMaj;
    if ( verMin )
        *verMin = s_version.verMin;
    if ( verMicro )
        *verMicro = s_version.verMicro;

    return s_version.os;
}

bool wxCheckOsVersion(int majorVsn, int minorVsn, int microVsn)
{
    int majorCur, minorCur, microCur;
    wxGetOsVersion(&majorCur, &minorCur, &microCur);

    return majorCur > majorVsn
        || (majorCur == majorVsn && minorCur > minorVsn)
        || (majorCur == majorVsn && minorCur == minorVsn && microCur >= microVsn);
}

wxWinVersion wxGetWinVersion()
{
    int verMaj,
        verMin,
        build;
    switch ( wxGetOsVersion(&verMaj, &verMin, &build) )
    {
        case wxOS_WINDOWS_NT:
            switch ( verMaj )
            {
                case 5:
                    switch ( verMin )
                    {
                        case 1:
                            return wxWinVersion_XP;

                        case 2:
                            return wxWinVersion_2003;
                    }
                    break;

                case 6:
                    switch ( verMin )
                    {
                        case 0:
                            return wxWinVersion_Vista;

                        case 1:
                            return wxWinVersion_7;

                        case 2:
                            return wxWinVersion_8;

                        case 3:
                            return wxWinVersion_8_1;

                    }
                    break;

                case 10:
                    return build >= FIRST_WINDOWS11_BUILD ? wxWinVersion_11
                                                          : wxWinVersion_10;
            }
            break;
        default:
            // Do nothing just to silence GCC warning
            break;
    }

    return wxWinVersion_Unknown;
}

wxString wxGetCpuArchitecureNameFromImageType(USHORT imageType)
{
    switch (imageType)
    {
    case IMAGE_FILE_MACHINE_I386:
        return "x86";
    case IMAGE_FILE_MACHINE_AMD64:
        return "x64";
    case IMAGE_FILE_MACHINE_IA64:
        return "Itanium";
    case IMAGE_FILE_MACHINE_ARMNT:
        return "arm";
    case IMAGE_FILE_MACHINE_ARM64:
        return "arm64";
    default:
        return wxString();
    }
}

// Wrap IsWow64Process2 API (Available since Win10 1511)
BOOL wxIsWow64Process2(HANDLE hProcess, USHORT* pProcessMachine, USHORT* pNativeMachine)
{
    typedef BOOL(WINAPI *IsWow64Process2_t)(HANDLE, USHORT *, USHORT *);

    wxDynamicLibrary dllKernel32("kernel32.dll");
    IsWow64Process2_t pfnIsWow64Process2 =
        (IsWow64Process2_t)dllKernel32.RawGetSymbol("IsWow64Process2");

    if (pfnIsWow64Process2)
        return pfnIsWow64Process2(hProcess, pProcessMachine, pNativeMachine);

    return FALSE;
}

wxString wxGetCpuArchitectureName()
{
    // Try to get the current active CPU architecture via IsWow64Process2()
    // first, fallback to GetNativeSystemInfo() otherwise
    USHORT machine;
    if (wxIsWow64Process2(::GetCurrentProcess(), &machine, nullptr) &&
        machine != IMAGE_FILE_MACHINE_UNKNOWN)
        return wxGetCpuArchitecureNameFromImageType(machine);

    SYSTEM_INFO si;
    GetNativeSystemInfo(&si);

    switch (si.wProcessorArchitecture)
    {
    case PROCESSOR_ARCHITECTURE_AMD64:
        return "x64";

    case PROCESSOR_ARCHITECTURE_ARM:
        return "ARM";

    case PROCESSOR_ARCHITECTURE_ARM64:
        return "ARM64";

    case PROCESSOR_ARCHITECTURE_IA64:
        return "Itanium";

    case PROCESSOR_ARCHITECTURE_INTEL:
        return "x86";

    case PROCESSOR_ARCHITECTURE_UNKNOWN:
    default:
        return wxString();
    }
}

wxString wxGetNativeCpuArchitectureName()
{
    USHORT machine;
    USHORT nativeMachine;
    if (wxIsWow64Process2(::GetCurrentProcess(), &machine, &nativeMachine))
        return wxGetCpuArchitecureNameFromImageType(nativeMachine);
    else
        return wxGetCpuArchitectureName();
}



// ----------------------------------------------------------------------------
// sleep functions
// ----------------------------------------------------------------------------

void wxMilliSleep(unsigned long milliseconds)
{
    ::Sleep(milliseconds);
}

void wxMicroSleep(unsigned long microseconds)
{
    wxMilliSleep(microseconds/1000);
}

void wxSleep(int nSecs)
{
    wxMilliSleep(1000*nSecs);
}

// ----------------------------------------------------------------------------
// font encoding <-> Win32 codepage conversion functions
// ----------------------------------------------------------------------------

extern WXDLLIMPEXP_BASE long wxEncodingToCharset(wxFontEncoding encoding)
{
    switch ( encoding )
    {
        // although this function is supposed to return an exact match, do do
        // some mappings here for the most common case of "standard" encoding
        case wxFONTENCODING_SYSTEM:
            return DEFAULT_CHARSET;

        case wxFONTENCODING_ISO8859_1:
        case wxFONTENCODING_ISO8859_15:
        case wxFONTENCODING_CP1252:
            return ANSI_CHARSET;

        // The following four fonts are multi-byte charsets
        case wxFONTENCODING_CP932:
            return SHIFTJIS_CHARSET;

        case wxFONTENCODING_CP936:
            return GB2312_CHARSET;

        case wxFONTENCODING_CP949:
            return HANGUL_CHARSET;

        case wxFONTENCODING_CP950:
            return CHINESEBIG5_CHARSET;

        // The rest are single byte encodings
        case wxFONTENCODING_CP1250:
            return EASTEUROPE_CHARSET;

        case wxFONTENCODING_CP1251:
            return RUSSIAN_CHARSET;

        case wxFONTENCODING_CP1253:
            return GREEK_CHARSET;

        case wxFONTENCODING_CP1254:
            return TURKISH_CHARSET;

        case wxFONTENCODING_CP1255:
            return HEBREW_CHARSET;

        case wxFONTENCODING_CP1256:
            return ARABIC_CHARSET;

        case wxFONTENCODING_CP1257:
            return BALTIC_CHARSET;

        case wxFONTENCODING_CP874:
            return THAI_CHARSET;

        case wxFONTENCODING_CP437:
            return OEM_CHARSET;

        default:
            // no way to translate this encoding into a Windows charset
            return -1;
    }
}

// we have 2 versions of wxCharsetToCodepage(): the old one which directly
// looks up the vlaues in the registry and the new one which is more
// politically correct and has more chances to work on other Windows versions
// as well but the old version is still needed for !wxUSE_FONTMAP case

extern WXDLLIMPEXP_BASE long wxEncodingToCodepage(wxFontEncoding encoding)
{
    // There don't seem to be symbolic names for
    // these under Windows so I just copied the
    // values from MSDN.

    unsigned int ret;

    switch (encoding)
    {
        case wxFONTENCODING_ISO8859_1:      ret = 28591; break;
        case wxFONTENCODING_ISO8859_2:      ret = 28592; break;
        case wxFONTENCODING_ISO8859_3:      ret = 28593; break;
        case wxFONTENCODING_ISO8859_4:      ret = 28594; break;
        case wxFONTENCODING_ISO8859_5:      ret = 28595; break;
        case wxFONTENCODING_ISO8859_6:      ret = 28596; break;
        case wxFONTENCODING_ISO8859_7:      ret = 28597; break;
        case wxFONTENCODING_ISO8859_8:      ret = 28598; break;
        case wxFONTENCODING_ISO8859_9:      ret = 28599; break;
        case wxFONTENCODING_ISO8859_10:     ret = 28600; break;
        case wxFONTENCODING_ISO8859_11:     ret = 874; break;
        // case wxFONTENCODING_ISO8859_12,      // doesn't exist currently, but put it
        case wxFONTENCODING_ISO8859_13:     ret = 28603; break;
        // case wxFONTENCODING_ISO8859_14:     ret = 28604; break; // no correspondence on Windows
        case wxFONTENCODING_ISO8859_15:     ret = 28605; break;

        case wxFONTENCODING_KOI8:           ret = 20866; break;
        case wxFONTENCODING_KOI8_U:         ret = 21866; break;

        case wxFONTENCODING_CP437:          ret = 437; break;
        case wxFONTENCODING_CP850:          ret = 850; break;
        case wxFONTENCODING_CP852:          ret = 852; break;
        case wxFONTENCODING_CP855:          ret = 855; break;
        case wxFONTENCODING_CP866:          ret = 866; break;
        case wxFONTENCODING_CP874:          ret = 874; break;
        case wxFONTENCODING_CP932:          ret = 932; break;
        case wxFONTENCODING_CP936:          ret = 936; break;
        case wxFONTENCODING_CP949:          ret = 949; break;
        case wxFONTENCODING_CP950:          ret = 950; break;
        case wxFONTENCODING_CP1250:         ret = 1250; break;
        case wxFONTENCODING_CP1251:         ret = 1251; break;
        case wxFONTENCODING_CP1252:         ret = 1252; break;
        case wxFONTENCODING_CP1253:         ret = 1253; break;
        case wxFONTENCODING_CP1254:         ret = 1254; break;
        case wxFONTENCODING_CP1255:         ret = 1255; break;
        case wxFONTENCODING_CP1256:         ret = 1256; break;
        case wxFONTENCODING_CP1257:         ret = 1257; break;
        case wxFONTENCODING_CP1258:         ret = 1258; break;

        case wxFONTENCODING_EUC_JP:         ret = 20932; break;

        case wxFONTENCODING_MACROMAN:       ret = 10000; break;
        case wxFONTENCODING_MACJAPANESE:    ret = 10001; break;
        case wxFONTENCODING_MACCHINESETRAD: ret = 10002; break;
        case wxFONTENCODING_MACKOREAN:      ret = 10003; break;
        case wxFONTENCODING_MACARABIC:      ret = 10004; break;
        case wxFONTENCODING_MACHEBREW:      ret = 10005; break;
        case wxFONTENCODING_MACGREEK:       ret = 10006; break;
        case wxFONTENCODING_MACCYRILLIC:    ret = 10007; break;
        case wxFONTENCODING_MACTHAI:        ret = 10021; break;
        case wxFONTENCODING_MACCHINESESIMP: ret = 10008; break;
        case wxFONTENCODING_MACCENTRALEUR:  ret = 10029; break;
        case wxFONTENCODING_MACCROATIAN:    ret = 10082; break;
        case wxFONTENCODING_MACICELANDIC:   ret = 10079; break;
        case wxFONTENCODING_MACROMANIAN:    ret = 10010; break;

        case wxFONTENCODING_ISO2022_JP:     ret = 50222; break;

        case wxFONTENCODING_UTF7:           ret = 65000; break;
        case wxFONTENCODING_UTF8:           ret = 65001; break;

        default:                            return -1;
    }

    if (::IsValidCodePage(ret) == 0)
        return -1;

    CPINFO info;
    if (::GetCPInfo(ret, &info) == 0)
        return -1;

    return (long) ret;
}

#if wxUSE_FONTMAP

#include "wx/fontmap.h"

extern long wxCharsetToCodepage(const char *name)
{
    // first get the font encoding for this charset
    if ( !name )
        return -1;

    wxFontEncoding enc = wxFontMapperBase::Get()->CharsetToEncoding(name, false);
    if ( enc == wxFONTENCODING_SYSTEM )
        return -1;

    // the use the helper function
    return wxEncodingToCodepage(enc);
}

#else // !wxUSE_FONTMAP

#include "wx/msw/registry.h"

// this should work if Internet Exploiter is installed
extern long wxCharsetToCodepage(const char *name)
{
    if (!name)
        return GetACP();

    long CP = -1;

#if wxUSE_REGKEY
    wxString path(wxT("MIME\\Database\\Charset\\"));
    wxString cn(name);

    // follow the alias loop
    for ( ;; )
    {
        wxRegKey key(wxRegKey::HKCR, path + cn);

        if (!key.Exists())
            break;

        // two cases: either there's an AliasForCharset string,
        // or there are Codepage and InternetEncoding dwords.
        // The InternetEncoding gives us the actual encoding,
        // the Codepage just says which Windows character set to
        // use when displaying the data.
        if (key.HasValue(wxT("InternetEncoding")) &&
            key.QueryValue(wxT("InternetEncoding"), &CP))
            break;

        // no encoding, see if it's an alias
        if (!key.HasValue(wxT("AliasForCharset")) ||
            !key.QueryValue(wxT("AliasForCharset"), cn))
            break;
    }
#endif // wxUSE_REGKEY

    return CP;
}

#endif // wxUSE_FONTMAP/!wxUSE_FONTMAP

extern "C" WXDLLIMPEXP_BASE HWND
wxCreateHiddenWindow(LPCTSTR *pclassname, LPCTSTR classname, WNDPROC wndproc)
{
    wxCHECK_MSG( classname && pclassname && wndproc, nullptr,
                    wxT("null parameter in wxCreateHiddenWindow") );

    // register the class fi we need to first
    if ( *pclassname == nullptr )
    {
        WNDCLASS wndclass;
        wxZeroMemory(wndclass);

        wndclass.lpfnWndProc   = wndproc;
        wndclass.hInstance     = wxGetInstance();
        wndclass.lpszClassName = classname;

        if ( !::RegisterClass(&wndclass) )
        {
            wxLogLastError(wxT("RegisterClass() in wxCreateHiddenWindow"));

            return nullptr;
        }

        *pclassname = classname;
    }

    // next create the window
    HWND hwnd = ::CreateWindow
                  (
                    *pclassname,
                    nullptr,
                    0, 0, 0, 0,
                    0,
                    nullptr,
                    nullptr,
                    wxGetInstance(),
                    nullptr
                  );

    if ( !hwnd )
    {
        wxLogLastError(wxT("CreateWindow() in wxCreateHiddenWindow"));
    }

    return hwnd;
}

int wxCMPFUNC_CONV wxCmpNatural(const wxString& s1, const wxString& s2)
{
    return StrCmpLogicalW(s1.wc_str(), s2.wc_str());
}
