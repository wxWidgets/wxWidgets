/////////////////////////////////////////////////////////////////////////////
// Name:        msw/dialup.cpp
// Purpose:     MSW implementation of network/dialup classes and functions
// Author:      Vadim Zeitlin
// Modified by:
// Created:     07.07.99
// RCS-ID:      $Id$
// Copyright:   (c) Vadim Zeitlin
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// for compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// these functions require Win32
#if defined(__WIN16__) && wxUSE_DIALUP_MANAGER
    #undef wxUSE_DIALUP_MANAGER
    #define wxUSE_DIALUP_MANAGER 0
#endif // wxUSE_DIALUP_MANAGER && Win16

#if wxUSE_DIALUP_MANAGER

#ifndef WX_PRECOMP
    #include "wx/log.h"
    #include "wx/intl.h"
    #include "wx/event.h"
#endif

#include "wx/timer.h"
#include "wx/app.h"
#include "wx/generic/choicdgg.h"

#include "wx/msw/private.h"  // must be before #include "dynlib.h"

#if !wxUSE_DYNLIB_CLASS
    #error You need wxUSE_DYNLIB_CLASS to be 1 to compile dialup.cpp.
#endif

#include "wx/dynlib.h"

#include "wx/dialup.h"

// Doesn't yet compile under VC++ 4, BC++, mingw, Watcom C++: no wininet.h
#if !defined(__BORLANDC__) && !defined(__GNUWIN32_OLD__) && !defined(__GNUWIN32__) && !defined(__WATCOMC__) && ! (defined(__VISUALC__) && (__VISUALC__ < 1020))

#include <ras.h>
#include <raserror.h>

#include <wininet.h>

// Not in VC++ 5
#ifndef INTERNET_CONNECTION_LAN
#define INTERNET_CONNECTION_LAN 2
#endif
#ifndef INTERNET_CONNECTION_PROXY
#define INTERNET_CONNECTION_PROXY 4
#endif

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// this message is sent by the secondary thread when RAS status changes
#define wxWM_RAS_STATUS_CHANGED (WM_USER + 10010)
#define wxWM_RAS_DIALING_PROGRESS (WM_USER + 10011)

// ----------------------------------------------------------------------------
// types
// ----------------------------------------------------------------------------

// the signatures of RAS functions: all this is quite heavy, but we must do it
// to allow running wxWin programs on machine which don't have RAS installed
// (this does exist) - if we link with rasapi32.lib, the program will fail on
// startup because of the missing DLL...

#ifndef UNICODE
    typedef DWORD (APIENTRY * RASDIAL)( LPRASDIALEXTENSIONS, LPCSTR, LPRASDIALPARAMSA, DWORD, LPVOID, LPHRASCONN );
    typedef DWORD (APIENTRY * RASENUMCONNECTIONS)( LPRASCONNA, LPDWORD, LPDWORD );
    typedef DWORD (APIENTRY * RASENUMENTRIES)( LPCSTR, LPCSTR, LPRASENTRYNAMEA, LPDWORD, LPDWORD );
    typedef DWORD (APIENTRY * RASGETCONNECTSTATUS)( HRASCONN, LPRASCONNSTATUSA );
    typedef DWORD (APIENTRY * RASGETERRORSTRING)( UINT, LPSTR, DWORD );
    typedef DWORD (APIENTRY * RASHANGUP)( HRASCONN );
    typedef DWORD (APIENTRY * RASGETPROJECTIONINFO)( HRASCONN, RASPROJECTION, LPVOID, LPDWORD );
    typedef DWORD (APIENTRY * RASCREATEPHONEBOOKENTRY)( HWND, LPCSTR );
    typedef DWORD (APIENTRY * RASEDITPHONEBOOKENTRY)( HWND, LPCSTR, LPCSTR );
    typedef DWORD (APIENTRY * RASSETENTRYDIALPARAMS)( LPCSTR, LPRASDIALPARAMSA, BOOL );
    typedef DWORD (APIENTRY * RASGETENTRYDIALPARAMS)( LPCSTR, LPRASDIALPARAMSA, LPBOOL );
    typedef DWORD (APIENTRY * RASENUMDEVICES)( LPRASDEVINFOA, LPDWORD, LPDWORD );
    typedef DWORD (APIENTRY * RASGETCOUNTRYINFO)( LPRASCTRYINFOA, LPDWORD );
    typedef DWORD (APIENTRY * RASGETENTRYPROPERTIES)( LPCSTR, LPCSTR, LPRASENTRYA, LPDWORD, LPBYTE, LPDWORD );
    typedef DWORD (APIENTRY * RASSETENTRYPROPERTIES)( LPCSTR, LPCSTR, LPRASENTRYA, DWORD, LPBYTE, DWORD );
    typedef DWORD (APIENTRY * RASRENAMEENTRY)( LPCSTR, LPCSTR, LPCSTR );
    typedef DWORD (APIENTRY * RASDELETEENTRY)( LPCSTR, LPCSTR );
    typedef DWORD (APIENTRY * RASVALIDATEENTRYNAME)( LPCSTR, LPCSTR );
    typedef DWORD (APIENTRY * RASCONNECTIONNOTIFICATION)( HRASCONN, HANDLE, DWORD );

    static const wxChar gs_funcSuffix = _T('A');
#else // Unicode
    typedef DWORD (APIENTRY * RASDIAL)( LPRASDIALEXTENSIONS, LPCWSTR, LPRASDIALPARAMSW, DWORD, LPVOID, LPHRASCONN );
    typedef DWORD (APIENTRY * RASENUMCONNECTIONS)( LPRASCONNW, LPDWORD, LPDWORD );
    typedef DWORD (APIENTRY * RASENUMENTRIES)( LPCWSTR, LPCWSTR, LPRASENTRYNAMEW, LPDWORD, LPDWORD );
    typedef DWORD (APIENTRY * RASGETCONNECTSTATUS)( HRASCONN, LPRASCONNSTATUSW );
    typedef DWORD (APIENTRY * RASGETERRORSTRING)( UINT, LPWSTR, DWORD );
    typedef DWORD (APIENTRY * RASHANGUP)( HRASCONN );
    typedef DWORD (APIENTRY * RASGETPROJECTIONINFO)( HRASCONN, RASPROJECTION, LPVOID, LPDWORD );
    typedef DWORD (APIENTRY * RASCREATEPHONEBOOKENTRY)( HWND, LPCWSTR );
    typedef DWORD (APIENTRY * RASEDITPHONEBOOKENTRY)( HWND, LPCWSTR, LPCWSTR );
    typedef DWORD (APIENTRY * RASSETENTRYDIALPARAMS)( LPCWSTR, LPRASDIALPARAMSW, BOOL );
    typedef DWORD (APIENTRY * RASGETENTRYDIALPARAMS)( LPCWSTR, LPRASDIALPARAMSW, LPBOOL );
    typedef DWORD (APIENTRY * RASENUMDEVICES)( LPRASDEVINFOW, LPDWORD, LPDWORD );
    typedef DWORD (APIENTRY * RASGETCOUNTRYINFO)( LPRASCTRYINFOW, LPDWORD );
    typedef DWORD (APIENTRY * RASGETENTRYPROPERTIES)( LPCWSTR, LPCWSTR, LPRASENTRYW, LPDWORD, LPBYTE, LPDWORD );
    typedef DWORD (APIENTRY * RASSETENTRYPROPERTIES)( LPCWSTR, LPCWSTR, LPRASENTRYW, DWORD, LPBYTE, DWORD );
    typedef DWORD (APIENTRY * RASRENAMEENTRY)( LPCWSTR, LPCWSTR, LPCWSTR );
    typedef DWORD (APIENTRY * RASDELETEENTRY)( LPCWSTR, LPCWSTR );
    typedef DWORD (APIENTRY * RASVALIDATEENTRYNAME)( LPCWSTR, LPCWSTR );
    typedef DWORD (APIENTRY * RASCONNECTIONNOTIFICATION)( HRASCONN, HANDLE, DWORD );

    static const wxChar gs_funcSuffix = _T('W');
#endif // ASCII/Unicode

// structure passed to the secondary thread
struct WXDLLEXPORT wxRasThreadData
{
    wxRasThreadData()
    {
        hWnd = 0;
        hEventRas = hEventQuit = INVALID_HANDLE_VALUE;
        dialUpManager = NULL;
    }

    HWND    hWnd;       // window to send notifications to
    HANDLE  hEventRas,  // event which RAS signals when status changes
            hEventQuit; // event which we signal when we terminate

    class WXDLLEXPORT wxDialUpManagerMSW *dialUpManager;  // the owner
};

// ----------------------------------------------------------------------------
// wxDialUpManager class for MSW
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxDialUpManagerMSW : public wxDialUpManager
{
public:
    // ctor & dtor
    wxDialUpManagerMSW();
    virtual ~wxDialUpManagerMSW();

    // implement base class pure virtuals
    virtual bool IsOk() const;
    virtual size_t GetISPNames(wxArrayString& names) const;
    virtual bool Dial(const wxString& nameOfISP,
                      const wxString& username,
                      const wxString& password,
                      bool async);
    virtual bool IsDialing() const;
    virtual bool CancelDialing();
    virtual bool HangUp();
    virtual bool IsAlwaysOnline() const;
    virtual bool IsOnline() const;
    virtual void SetOnlineStatus(bool isOnline = TRUE);
    virtual bool EnableAutoCheckOnlineStatus(size_t nSeconds);
    virtual void DisableAutoCheckOnlineStatus();
    virtual void SetWellKnownHost(const wxString& hostname, int port);
    virtual void SetConnectCommand(const wxString& commandDial,
                                   const wxString& commandHangup);

    // for RasTimer
    void CheckRasStatus();

    // for wxRasStatusWindowProc
    void OnConnectStatusChange();
    void OnDialProgress(RASCONNSTATE rasconnstate, DWORD dwError);

    // for wxRasDialFunc
    static HWND GetRasWindow() { return ms_hwndRas; }
    static wxDialUpManagerMSW *GetDialer() { return ms_dialer; }

private:
    // return the error string for the given RAS error code
    static wxString GetErrorString(DWORD error);

    // find the (first) handle of the active connection
    static HRASCONN FindActiveConnection();

    // notify the application about status change
    void NotifyApp(bool connected, bool fromOurselves = FALSE) const;

    // destroy the thread data and the thread itself
    void CleanUpThreadData();

    // timer used for polling RAS status
    class WXDLLEXPORT RasTimer : public wxTimer
    {
    public:
        RasTimer(wxDialUpManagerMSW *dialUpManager)
            { m_dialUpManager = dialUpManager; }

        virtual void Notify() { m_dialUpManager->CheckRasStatus(); }

    private:
        wxDialUpManagerMSW *m_dialUpManager;
    } m_timerStatusPolling;

    // thread handle for the thread sitting on connection change event
    HANDLE m_hThread;

    // data used by this thread and our hidden window to send messages between
    // each other
    wxRasThreadData m_data;

    // the hidden window we use for passing messages between threads
    static HWND ms_hwndRas;

    // the handle of the connection we initiated or 0 if none
    static HRASCONN ms_hRasConnection;

    // the use count of rasapi32.dll
    static int ms_nDllCount;

    // the handle of rasapi32.dll when it's loaded
    static wxDllType ms_dllRas;

    // the pointers to RAS functions
    static RASDIAL ms_pfnRasDial;
    static RASENUMCONNECTIONS ms_pfnRasEnumConnections;
    static RASENUMENTRIES ms_pfnRasEnumEntries;
    static RASGETCONNECTSTATUS ms_pfnRasGetConnectStatus;
    static RASGETERRORSTRING ms_pfnRasGetErrorString;
    static RASHANGUP ms_pfnRasHangUp;
    static RASGETPROJECTIONINFO ms_pfnRasGetProjectionInfo;
    static RASCREATEPHONEBOOKENTRY ms_pfnRasCreatePhonebookEntry;
    static RASEDITPHONEBOOKENTRY ms_pfnRasEditPhonebookEntry;
    static RASSETENTRYDIALPARAMS ms_pfnRasSetEntryDialParams;
    static RASGETENTRYDIALPARAMS ms_pfnRasGetEntryDialParams;
    static RASENUMDEVICES ms_pfnRasEnumDevices;
    static RASGETCOUNTRYINFO ms_pfnRasGetCountryInfo;
    static RASGETENTRYPROPERTIES ms_pfnRasGetEntryProperties;
    static RASSETENTRYPROPERTIES ms_pfnRasSetEntryProperties;
    static RASRENAMEENTRY ms_pfnRasRenameEntry;
    static RASDELETEENTRY ms_pfnRasDeleteEntry;
    static RASVALIDATEENTRYNAME ms_pfnRasValidateEntryName;

    // this function is not supported by Win95
    static RASCONNECTIONNOTIFICATION ms_pfnRasConnectionNotification;

    // if this flag is different from -1, it overrides IsOnline()
    static int ms_userSpecifiedOnlineStatus;

    // this flag tells us if we're online
    static int ms_isConnected;

    // this flag is the result of the call to IsAlwaysOnline() (-1 if not
    // called yet)
    static int ms_isAlwaysOnline;

    // this flag tells us whether a call to RasDial() is in progress
    static wxDialUpManagerMSW *ms_dialer;
};

// ----------------------------------------------------------------------------
// private functions
// ----------------------------------------------------------------------------

static LRESULT WINAPI wxRasStatusWindowProc(HWND hWnd, UINT message,
                                            WPARAM wParam, LPARAM lParam);

static DWORD wxRasMonitorThread(wxRasThreadData *data);

static void WINAPI wxRasDialFunc(UINT unMsg,
                                 RASCONNSTATE rasconnstate,
                                 DWORD dwError);

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// init the static variables
// ----------------------------------------------------------------------------

HRASCONN wxDialUpManagerMSW::ms_hRasConnection = 0;

HWND wxDialUpManagerMSW::ms_hwndRas = 0;

int wxDialUpManagerMSW::ms_nDllCount = 0;
wxDllType wxDialUpManagerMSW::ms_dllRas = 0;

RASDIAL wxDialUpManagerMSW::ms_pfnRasDial = 0;
RASENUMCONNECTIONS wxDialUpManagerMSW::ms_pfnRasEnumConnections = 0;
RASENUMENTRIES wxDialUpManagerMSW::ms_pfnRasEnumEntries = 0;
RASGETCONNECTSTATUS wxDialUpManagerMSW::ms_pfnRasGetConnectStatus = 0;
RASGETERRORSTRING wxDialUpManagerMSW::ms_pfnRasGetErrorString = 0;
RASHANGUP wxDialUpManagerMSW::ms_pfnRasHangUp = 0;
RASGETPROJECTIONINFO wxDialUpManagerMSW::ms_pfnRasGetProjectionInfo = 0;
RASCREATEPHONEBOOKENTRY wxDialUpManagerMSW::ms_pfnRasCreatePhonebookEntry = 0;
RASEDITPHONEBOOKENTRY wxDialUpManagerMSW::ms_pfnRasEditPhonebookEntry = 0;
RASSETENTRYDIALPARAMS wxDialUpManagerMSW::ms_pfnRasSetEntryDialParams = 0;
RASGETENTRYDIALPARAMS wxDialUpManagerMSW::ms_pfnRasGetEntryDialParams = 0;
RASENUMDEVICES wxDialUpManagerMSW::ms_pfnRasEnumDevices = 0;
RASGETCOUNTRYINFO wxDialUpManagerMSW::ms_pfnRasGetCountryInfo = 0;
RASGETENTRYPROPERTIES wxDialUpManagerMSW::ms_pfnRasGetEntryProperties = 0;
RASSETENTRYPROPERTIES wxDialUpManagerMSW::ms_pfnRasSetEntryProperties = 0;
RASRENAMEENTRY wxDialUpManagerMSW::ms_pfnRasRenameEntry = 0;
RASDELETEENTRY wxDialUpManagerMSW::ms_pfnRasDeleteEntry = 0;
RASVALIDATEENTRYNAME wxDialUpManagerMSW::ms_pfnRasValidateEntryName = 0;
RASCONNECTIONNOTIFICATION wxDialUpManagerMSW::ms_pfnRasConnectionNotification = 0;

int wxDialUpManagerMSW::ms_userSpecifiedOnlineStatus = -1;
int wxDialUpManagerMSW::ms_isConnected = -1;
int wxDialUpManagerMSW::ms_isAlwaysOnline = -1;
wxDialUpManagerMSW *wxDialUpManagerMSW::ms_dialer = NULL;

// ----------------------------------------------------------------------------
// ctor and dtor: the dynamic linking happens here
// ----------------------------------------------------------------------------

// the static creator function is implemented here
wxDialUpManager *wxDialUpManager::Create()
{
    return new wxDialUpManagerMSW;
}

#ifdef __VISUALC__
    // warning about "'this' : used in base member initializer list" - so what?
    #pragma warning(disable:4355)
#endif // VC++

wxDialUpManagerMSW::wxDialUpManagerMSW()
                  : m_timerStatusPolling(this)
{
    // initialize our data
    m_hThread = 0;

    if ( !ms_nDllCount++ )
    {
        // load the RAS library
        ms_dllRas = wxDllLoader::LoadLibrary("RASAPI32");
        if ( !ms_dllRas )
        {
            wxLogError(_("Dial up functions are unavailable because the remote access service (RAS) is not installed on this machine. Please install it."));
        }
        else
        {
            // resolve the functions we need

            // this will contain the name of the function we failed to resolve
            // if any at the end
            const char *funcName = NULL;

            // get the function from rasapi32.dll and abort if it's not found
            #define RESOLVE_RAS_FUNCTION(type, name)                    \
                ms_pfn##name = (type)wxDllLoader::GetSymbol(ms_dllRas,  \
                               wxString(_T(#name)) + gs_funcSuffix);    \
                if ( !ms_pfn##name )                                    \
                {                                                       \
                    funcName = #name;                                   \
                    goto exit;                                          \
                }

            // a variant of above macro which doesn't abort if the function is
            // not found in the DLL
            #define RESOLVE_OPTIONAL_RAS_FUNCTION(type, name)           \
                ms_pfn##name = (type)wxDllLoader::GetSymbol(ms_dllRas,  \
                               wxString(_T(#name)) + gs_funcSuffix);

            RESOLVE_RAS_FUNCTION(RASDIAL, RasDial);
            RESOLVE_RAS_FUNCTION(RASENUMCONNECTIONS, RasEnumConnections);
            RESOLVE_RAS_FUNCTION(RASENUMENTRIES, RasEnumEntries);
            RESOLVE_RAS_FUNCTION(RASGETCONNECTSTATUS, RasGetConnectStatus);
            RESOLVE_RAS_FUNCTION(RASGETERRORSTRING, RasGetErrorString);
            RESOLVE_RAS_FUNCTION(RASHANGUP, RasHangUp);
            RESOLVE_RAS_FUNCTION(RASGETENTRYDIALPARAMS, RasGetEntryDialParams);

            // suppress wxDllLoader messages about missing (non essential)
            // functions
            {
                wxLogNull noLog;

                RESOLVE_OPTIONAL_RAS_FUNCTION(RASGETPROJECTIONINFO, RasGetProjectionInfo);
                RESOLVE_OPTIONAL_RAS_FUNCTION(RASCREATEPHONEBOOKENTRY, RasCreatePhonebookEntry);
                RESOLVE_OPTIONAL_RAS_FUNCTION(RASEDITPHONEBOOKENTRY, RasEditPhonebookEntry);
                RESOLVE_OPTIONAL_RAS_FUNCTION(RASSETENTRYDIALPARAMS, RasSetEntryDialParams);
                RESOLVE_OPTIONAL_RAS_FUNCTION(RASGETENTRYPROPERTIES, RasGetEntryProperties);
                RESOLVE_OPTIONAL_RAS_FUNCTION(RASSETENTRYPROPERTIES, RasSetEntryProperties);
                RESOLVE_OPTIONAL_RAS_FUNCTION(RASRENAMEENTRY, RasRenameEntry);
                RESOLVE_OPTIONAL_RAS_FUNCTION(RASDELETEENTRY, RasDeleteEntry);
                RESOLVE_OPTIONAL_RAS_FUNCTION(RASVALIDATEENTRYNAME, RasValidateEntryName);
                RESOLVE_OPTIONAL_RAS_FUNCTION(RASGETCOUNTRYINFO, RasGetCountryInfo);
                RESOLVE_OPTIONAL_RAS_FUNCTION(RASENUMDEVICES, RasEnumDevices);
                RESOLVE_OPTIONAL_RAS_FUNCTION(RASCONNECTIONNOTIFICATION, RasConnectionNotification);
            }

            // keep your preprocessor name space clean
            #undef RESOLVE_RAS_FUNCTION
            #undef RESOLVE_OPTIONAL_RAS_FUNCTION

exit:
            if ( funcName )
            {
                static const wxChar *msg = wxTRANSLATE(
"The version of remote access service (RAS) installed on this machine is too\
old, please upgrade (the following required function is missing: %s)."
                                                       );

                wxLogError(wxGetTranslation(msg), funcName);

                wxDllLoader::UnloadLibrary(ms_dllRas);
                ms_dllRas = 0;
                ms_nDllCount = 0;

                return;
            }
        }
    }

    // enable auto check by default
    EnableAutoCheckOnlineStatus(0);
}

wxDialUpManagerMSW::~wxDialUpManagerMSW()
{
    CleanUpThreadData();

    if ( !--ms_nDllCount )
    {
        // unload the RAS library
        wxDllLoader::UnloadLibrary(ms_dllRas);
        ms_dllRas = 0;
    }
}

// ----------------------------------------------------------------------------
// helper functions
// ----------------------------------------------------------------------------

wxString wxDialUpManagerMSW::GetErrorString(DWORD error)
{
    wxChar buffer[512]; // this should be more than enough according to MS docs
    DWORD dwRet = ms_pfnRasGetErrorString(error, buffer, WXSIZEOF(buffer));
    switch ( dwRet )
    {
        case ERROR_INVALID_PARAMETER:
            // this was a standard Win32 error probably
            return wxString(wxSysErrorMsg(error));

        default:
            {
                wxLogSysError(dwRet,
                      _("Failed to retrieve text of RAS error message"));

                wxString msg;
                msg.Printf(_("unknown error (error code %08x)."), error);
                return msg;
            }

        case 0:
            // we want the error message to start from a lower case letter
            buffer[0] = wxTolower(buffer[0]);

            return wxString(buffer);
    }
}

HRASCONN wxDialUpManagerMSW::FindActiveConnection()
{
    // enumerate connections
    DWORD cbBuf = sizeof(RASCONN);
    LPRASCONN lpRasConn = (LPRASCONN)malloc(cbBuf);
    if ( !lpRasConn )
    {
        // out of memory
        return 0;
    }

    lpRasConn->dwSize = sizeof(RASCONN);

    DWORD nConnections = 0;
    DWORD dwRet = ERROR_BUFFER_TOO_SMALL;

    while ( dwRet == ERROR_BUFFER_TOO_SMALL )
    {
        dwRet = ms_pfnRasEnumConnections(lpRasConn, &cbBuf, &nConnections);

        if ( dwRet == ERROR_BUFFER_TOO_SMALL )
        {
            LPRASCONN lpRasConnOld = lpRasConn;
            lpRasConn = (LPRASCONN)realloc(lpRasConn, cbBuf);
            if ( !lpRasConn )
            {
                // out of memory
                free(lpRasConnOld);

                return 0;
            }
        }
        else if ( dwRet == 0 )
        {
            // ok, success
            break;
        }
        else
        {
            // an error occured
            wxLogError(_("Cannot find active dialup connection: %s"),
                       GetErrorString(dwRet));
            return 0;
        }
    }

    HRASCONN hrasconn;

    switch ( nConnections )
    {
        case 0:
            // no connections
            hrasconn = 0;
            break;

        default:
            // more than 1 connection - we don't know what to do with this
            // case, so give a warning but continue (taking the first
            // connection) - the warning is really needed because this function
            // is used, for example, to select the connection to hang up and so
            // we may hang up the wrong connection here...
            wxLogWarning(_("Several active dialup connections found, choosing one randomly."));
            // fall through

        case 1:
            // exactly 1 connection, great
            hrasconn = lpRasConn->hrasconn;
    }

    free(lpRasConn);

    return hrasconn;
}

void wxDialUpManagerMSW::CleanUpThreadData()
{
    if ( m_hThread )
    {
        if ( !SetEvent(m_data.hEventQuit) )
        {
            wxLogLastError(_T("SetEvent(RasThreadQuit)"));
        }

        CloseHandle(m_hThread);

        m_hThread = 0;
    }

    if ( m_data.hWnd )
    {
        DestroyWindow(m_data.hWnd);

        m_data.hWnd = 0;
    }

    if ( m_data.hEventQuit )
    {
        CloseHandle(m_data.hEventQuit);

        m_data.hEventQuit = 0;
    }

    if ( m_data.hEventRas )
    {
        CloseHandle(m_data.hEventRas);

        m_data.hEventRas = 0;
    }
}

// ----------------------------------------------------------------------------
// connection status
// ----------------------------------------------------------------------------

void wxDialUpManagerMSW::CheckRasStatus()
{
    // use int, not bool to compare with -1
    int isConnected = FindActiveConnection() != 0;
    if ( isConnected != ms_isConnected )
    {
        if ( ms_isConnected != -1 )
        {
            // notify the program
            NotifyApp(isConnected != 0);
        }
        // else: it's the first time we're called, just update the flag

        ms_isConnected = isConnected;
    }
}

void wxDialUpManagerMSW::NotifyApp(bool connected, bool fromOurselves) const
{
    wxDialUpEvent event(connected, fromOurselves);
    (void)wxTheApp->ProcessEvent(event);
}

// this function is called whenever the status of any RAS connection on this
// machine changes by RAS itself
void wxDialUpManagerMSW::OnConnectStatusChange()
{
    // we know that status changed, but we don't know whether we're connected
    // or not - so find it out
    CheckRasStatus();
}

// this function is called by our callback which we give to RasDial() when
// calling it asynchronously
void wxDialUpManagerMSW::OnDialProgress(RASCONNSTATE rasconnstate,
                                        DWORD dwError)
{
    if ( !GetDialer() )
    {
        // this probably means that CancelDialing() was called and we get
        // "disconnected" notification
        return;
    }

    // we're only interested in 2 events: connected and disconnected
    if ( dwError )
    {
        wxLogError(_("Failed to establish dialup connection: %s"),
                   GetErrorString(dwError));

        // we should still call RasHangUp() if we got a non 0 connection
        if ( ms_hRasConnection )
        {
            ms_pfnRasHangUp(ms_hRasConnection);
            ms_hRasConnection = 0;
        }

        ms_dialer = NULL;

        NotifyApp(FALSE /* !connected */, TRUE /* we dialed ourselves */);
    }
    else if ( rasconnstate == RASCS_Connected )
    {
        ms_isConnected = TRUE;
        ms_dialer = NULL;

        NotifyApp(TRUE /* connected */, TRUE /* we dialed ourselves */);
    }
}

// ----------------------------------------------------------------------------
// implementation of wxDialUpManager functions
// ----------------------------------------------------------------------------

bool wxDialUpManagerMSW::IsOk() const
{
    return ms_dllRas != 0;
}

size_t wxDialUpManagerMSW::GetISPNames(wxArrayString& names) const
{
    // fetch the entries
    DWORD size = sizeof(RASENTRYNAME);
    RASENTRYNAME *rasEntries = (RASENTRYNAME *)malloc(size);
    rasEntries->dwSize = sizeof(RASENTRYNAME);

    DWORD nEntries;
    DWORD dwRet;
    do
    {
        dwRet = ms_pfnRasEnumEntries
                  (
                   NULL,                // reserved
                   NULL,                // default phone book (or all)
                   rasEntries,          // [out] buffer for the entries
                   &size,               // [in/out] size of the buffer
                   &nEntries            // [out] number of entries fetched
                  );

        if ( dwRet == ERROR_BUFFER_TOO_SMALL )
        {
            // reallocate the buffer
            rasEntries = (RASENTRYNAME *)realloc(rasEntries, size);
        }
        else if ( dwRet != 0 )
        {
            // some other error - abort
            wxLogError(_("Failed to get ISP names: %s"), GetErrorString(dwRet));

            free(rasEntries);

            return 0u;
        }
    }
    while ( dwRet != 0 );

    // process them
    names.Empty();
    for ( size_t n = 0; n < (size_t)nEntries; n++ )
    {
        names.Add(rasEntries[n].szEntryName);
    }

    free(rasEntries);

    // return the number of entries
    return names.GetCount();
}

bool wxDialUpManagerMSW::Dial(const wxString& nameOfISP,
                              const wxString& username,
                              const wxString& password,
                              bool async)
{
    // check preconditions
    wxCHECK_MSG( IsOk(), FALSE, wxT("using uninitialized wxDialUpManager") );

    if ( ms_hRasConnection )
    {
        wxFAIL_MSG(wxT("there is already an active connection"));

        return TRUE;
    }

    // get the default ISP if none given
    wxString entryName(nameOfISP);
    if ( !entryName )
    {
        wxArrayString names;
        size_t count = GetISPNames(names);
        switch ( count )
        {
            case 0:
                // no known ISPs, abort
                wxLogError(_("Failed to connect: no ISP to dial."));

                return FALSE;

            case 1:
                // only one ISP, choose it
                entryName = names[0u];
                break;

            default:
                // several ISPs, let the user choose
                {
                    wxString *strings = new wxString[count];
                    for ( size_t i = 0; i < count; i++ )
                    {
                        strings[i] = names[i];
                    }

                    entryName = wxGetSingleChoice
                                (
                                 _("Choose ISP to dial"),
                                 _("Please choose which ISP do you want to connect to"),
                                 count,
                                 strings
                                );

                    delete [] strings;

                    if ( !entryName )
                    {
                        // cancelled by user
                        return FALSE;
                    }
                }
        }
    }

    RASDIALPARAMS rasDialParams;
    rasDialParams.dwSize = sizeof(rasDialParams);
    wxStrncpy(rasDialParams.szEntryName, entryName, RAS_MaxEntryName);

    // do we have the username and password?
    if ( !username || !password )
    {
        BOOL gotPassword;
        DWORD dwRet = ms_pfnRasGetEntryDialParams
                      (
                       NULL,            // default phonebook
                       &rasDialParams,  // [in/out] the params of this entry
                       &gotPassword     // [out] did we get password?
                      );

        if ( dwRet != 0 )
        {
            wxLogError(_("Failed to connect: missing username/password."));

            return FALSE;
        }
    }
	else
	{
		wxStrncpy(rasDialParams.szUserName, username, UNLEN);
		wxStrncpy(rasDialParams.szPassword, password, PWLEN);
	}

	// default values for other fields
    rasDialParams.szPhoneNumber[0] = '\0';
    rasDialParams.szCallbackNumber[0] = '\0';
    rasDialParams.szCallbackNumber[0] = '\0';

    rasDialParams.szDomain[0] = '*';
    rasDialParams.szDomain[1] = '\0';

    // apparently, this is not really necessary - passing NULL instead of the
    // phone book has the same effect
#if 0
    wxString phoneBook;
    if ( wxGetOsVersion() == wxWINDOWS_NT )
    {
        // first get the length
        UINT nLen = ::GetSystemDirectory(NULL, 0);
        nLen++;

        if ( !::GetSystemDirectory(phoneBook.GetWriteBuf(nLen), nLen) )
        {
            wxLogSysError(_("Cannot find the location of address book file"));
        }

        phoneBook.UngetWriteBuf();

        // this is the default phone book
        phoneBook << "\\ras\\rasphone.pbk";
    }
#endif // 0

    // TODO may be we should disable auto check while async dialing is in
    //      progress?

    ms_dialer = this;

    DWORD dwRet = ms_pfnRasDial
                  (
                   (LPRASDIALEXTENSIONS)NULL,   // no extended features
                   NULL,                  // default phone book file (NT only)
                   &rasDialParams,
                   0,                     // use callback for notifications
                   async ? wxRasDialFunc  // the callback
                         : 0,             // no notifications - sync operation
                   &ms_hRasConnection
                  );

    if ( dwRet != 0 )
    {
        wxLogError(_("Failed to %s dialup connection: %s"),
                   async ? _("initiate") : _("establish"),
                   GetErrorString(dwRet));

        // we should still call RasHangUp() if we got a non 0 connection
        if ( ms_hRasConnection )
        {
            ms_pfnRasHangUp(ms_hRasConnection);
            ms_hRasConnection = 0;
        }

        ms_dialer = NULL;

        return FALSE;
    }

    // for async dialing, we're not yet connected
    if ( !async )
    {
        ms_isConnected = TRUE;
    }

    return TRUE;
}

bool wxDialUpManagerMSW::IsDialing() const
{
    return GetDialer() != NULL;
}

bool wxDialUpManagerMSW::CancelDialing()
{
    if ( !GetDialer() )
    {
        // silently ignore
        return FALSE;
    }

    wxASSERT_MSG( ms_hRasConnection, wxT("dialing but no connection?") );

    ms_dialer = NULL;

    return HangUp();
}

bool wxDialUpManagerMSW::HangUp()
{
    wxCHECK_MSG( IsOk(), FALSE, wxT("using uninitialized wxDialUpManager") );

    // we may terminate either the connection we initiated or another one which
    // is active now
    HRASCONN hRasConn;
    if ( ms_hRasConnection )
    {
        hRasConn = ms_hRasConnection;

        ms_hRasConnection = 0;
    }
    else
    {
        hRasConn = FindActiveConnection();
    }

    if ( !hRasConn )
    {
        wxLogError(_("Cannot hang up - no active dialup connection."));

        return FALSE;
    }

    DWORD dwRet = ms_pfnRasHangUp(hRasConn);
    if ( dwRet != 0 )
    {
        wxLogError(_("Failed to terminate the dialup connection: %s"),
                   GetErrorString(dwRet));
    }

    ms_isConnected = FALSE;

    return TRUE;
}

bool wxDialUpManagerMSW::IsAlwaysOnline() const
{
    // we cache the result (presumably this won't change while the program is
    // running!)
    if ( ms_isAlwaysOnline != -1 )
    {
        return ms_isAlwaysOnline != 0;
    }

    // try to use WinInet function first
    bool ok;
    wxDllType hDll = wxDllLoader::LoadLibrary(_T("WININET"), &ok);
    if ( ok )
    {
        typedef BOOL (*INTERNETGETCONNECTEDSTATE)(LPDWORD, DWORD);
        INTERNETGETCONNECTEDSTATE pfnInternetGetConnectedState;

        #define RESOLVE_FUNCTION(type, name) \
            pfn##name = (type)wxDllLoader::GetSymbol(hDll, _T(#name))

        RESOLVE_FUNCTION(INTERNETGETCONNECTEDSTATE, InternetGetConnectedState);

        if ( pfnInternetGetConnectedState )
        {
            DWORD flags = 0;
            if ( pfnInternetGetConnectedState(&flags, 0 /* reserved */) )
            {
                // there is some connection to the net, see of which type
                ms_isAlwaysOnline = (flags & INTERNET_CONNECTION_LAN != 0) ||
                                    (flags & INTERNET_CONNECTION_PROXY != 0);
            }
            else
            {
                // no Internet connection at all
                ms_isAlwaysOnline = FALSE;
            }
        }

        wxDllLoader::UnloadLibrary(hDll);
    }

    // did we succeed with WinInet? if not, try something else
    if ( ms_isAlwaysOnline == -1 )
    {
        if ( !IsOnline() )
        {
            // definitely no permanent connection because we are not connected
            // now
            ms_isAlwaysOnline = FALSE;
        }
        else
        {
            // of course, having a modem doesn't prevent us from having a
            // permanent connection as well, but we have to guess somehow and
            // it's probably more common that a system connected via a modem
            // doesn't have any other net access, so:
            ms_isAlwaysOnline = FALSE;
        }
    }

    wxASSERT_MSG( ms_isAlwaysOnline != -1, wxT("logic error") );

    return ms_isAlwaysOnline != 0;
}

bool wxDialUpManagerMSW::IsOnline() const
{
    wxCHECK_MSG( IsOk(), FALSE, wxT("using uninitialized wxDialUpManager") );

    if ( ms_userSpecifiedOnlineStatus != -1 )
    {
        // user specified flag overrides our logic
        return ms_userSpecifiedOnlineStatus != 0;
    }
    else
    {
        // return TRUE if there is at least one active connection
        return FindActiveConnection() != 0;
    }
}

void wxDialUpManagerMSW::SetOnlineStatus(bool isOnline)
{
    wxCHECK_RET( IsOk(), wxT("using uninitialized wxDialUpManager") );

    ms_userSpecifiedOnlineStatus = isOnline;
}

bool wxDialUpManagerMSW::EnableAutoCheckOnlineStatus(size_t nSeconds)
{
    wxCHECK_MSG( IsOk(), FALSE, wxT("using uninitialized wxDialUpManager") );

    bool ok = ms_pfnRasConnectionNotification != 0;

    if ( ok )
    {
        // we're running under NT 4.0, Windows 98 or later and can use
        // RasConnectionNotification() to be notified by a secondary thread

        // first, see if we don't have this thread already running
        if ( m_hThread != 0 )
        {
            DWORD dwSuspendCount = 2;
            while ( dwSuspendCount > 1 )
            {
                dwSuspendCount = ResumeThread(m_hThread);
                if ( dwSuspendCount == (DWORD)-1 )
                {
                    wxLogLastError(wxT("ResumeThread(RasThread)"));

                    ok = FALSE;
                }
            }

            if ( ok )
            {
                return TRUE;
            }
        }
    }

    // create all the stuff we need to be notified about RAS connection
    // status change

    if ( ok )
    {
        // first create an event to wait on
        m_data.hEventRas = CreateEvent
                           (
                             NULL,      // security attribute (default)
                             FALSE,     // manual reset (not)
                             FALSE,     // initial state (not signaled)
                             NULL       // name (no)
                           );
        if ( !m_data.hEventRas )
        {
            wxLogLastError(wxT("CreateEvent(RasStatus)"));

            ok = FALSE;
        }
    }

    if ( ok )
    {
        // create the event we use to quit the thread
        m_data.hEventQuit = CreateEvent(NULL, FALSE, FALSE, NULL);
        if ( !m_data.hEventQuit )
        {
            wxLogLastError(wxT("CreateEvent(RasThreadQuit)"));

            CleanUpThreadData();

            ok = FALSE;
        }
    }

    if ( ok && !ms_hwndRas )
    {
        // create a hidden window to receive notification about connections
        // status change
        extern const wxChar *wxPanelClassName;
        ms_hwndRas = ::CreateWindow(wxPanelClassName, NULL,
                                    0, 0, 0, 0,
                                    0, NULL,
                                    (HMENU)NULL, wxGetInstance(), 0);
        if ( !ms_hwndRas )
        {
            wxLogLastError(wxT("CreateWindow(RasHiddenWindow)"));

            CleanUpThreadData();

            ok = FALSE;
        }

        // and subclass it
        FARPROC windowProc = MakeProcInstance
                             (
                              (FARPROC)wxRasStatusWindowProc,
                              wxGetInstance()
                             );

        ::SetWindowLong(ms_hwndRas, GWL_WNDPROC, (LONG) windowProc);
    }

    m_data.hWnd = ms_hwndRas;

    if ( ok )
    {
        // start the secondary thread
        m_data.dialUpManager = this;

        DWORD tid;
        m_hThread = CreateThread
                    (
                     NULL,
                     0,
                     (LPTHREAD_START_ROUTINE)wxRasMonitorThread,
                     (void *)&m_data,
                     0,
                     &tid
                    );

        if ( !m_hThread )
        {
            wxLogLastError(wxT("CreateThread(RasStatusThread)"));

            CleanUpThreadData();
        }
    }

    if ( ok )
    {
        // start receiving RAS notifications
        DWORD dwRet = ms_pfnRasConnectionNotification
                      (
                        (HRASCONN)INVALID_HANDLE_VALUE,
                        m_data.hEventRas,
                        3 /* RASCN_Connection | RASCN_Disconnection */
                      );

        if ( dwRet != 0 )
        {
            wxLogDebug(wxT("RasConnectionNotification() failed: %s"),
                       GetErrorString(dwRet));

            CleanUpThreadData();
        }
        else
        {
            return TRUE;
        }
    }

    // we're running under Windows 95 and have to poll ourselves
    // (or, alternatively, the code above for NT/98 failed)
    m_timerStatusPolling.Stop();
    if ( nSeconds == 0 )
    {
        // default value
        nSeconds = 60;
    }
    m_timerStatusPolling.Start(nSeconds * 1000);

    return TRUE;
}

void wxDialUpManagerMSW::DisableAutoCheckOnlineStatus()
{
    wxCHECK_RET( IsOk(), wxT("using uninitialized wxDialUpManager") );

    if ( m_hThread )
    {
        // we have running secondary thread, it's just enough to suspend it
        if ( SuspendThread(m_hThread) == (DWORD)-1 )
        {
            wxLogLastError(wxT("SuspendThread(RasThread)"));
        }
    }
    else
    {
        // even simpler - just stop the timer
        m_timerStatusPolling.Stop();
    }
}

// ----------------------------------------------------------------------------
// stubs which don't do anything in MSW version
// ----------------------------------------------------------------------------

void wxDialUpManagerMSW::SetWellKnownHost(const wxString& WXUNUSED(hostname),
                                          int WXUNUSED(port))
{
    wxCHECK_RET( IsOk(), wxT("using uninitialized wxDialUpManager") );

    // nothing to do - we don't use this
}

void wxDialUpManagerMSW::SetConnectCommand(const wxString& WXUNUSED(dial),
                                           const wxString& WXUNUSED(hangup))
{
    wxCHECK_RET( IsOk(), wxT("using uninitialized wxDialUpManager") );

    // nothing to do - we don't use this
}

// ----------------------------------------------------------------------------
// callbacks
// ----------------------------------------------------------------------------

static DWORD wxRasMonitorThread(wxRasThreadData *data)
{
    HANDLE handles[2];
    handles[0] = data->hEventRas;
    handles[1] = data->hEventQuit;

    bool cont = TRUE;
    while ( cont )
    {
        DWORD dwRet = WaitForMultipleObjects(2, handles, FALSE, INFINITE);

        switch ( dwRet )
        {
            case WAIT_OBJECT_0:
                // RAS connection status changed
                SendMessage(data->hWnd, wxWM_RAS_STATUS_CHANGED,
                            0, (LPARAM)data);
                break;

            case WAIT_OBJECT_0 + 1:
                cont = FALSE;
                break;

            case WAIT_FAILED:
                wxLogLastError(wxT("WaitForMultipleObjects(RasMonitor)"));
                break;
        }
    }

    return 0;
}

static LRESULT APIENTRY wxRasStatusWindowProc(HWND hWnd, UINT message,
                                              WPARAM wParam, LPARAM lParam)
{
    if ( message == wxWM_RAS_STATUS_CHANGED )
    {
        wxRasThreadData *data = (wxRasThreadData *)lParam;
        data->dialUpManager->OnConnectStatusChange();
    }
    else if ( message == wxWM_RAS_DIALING_PROGRESS )
    {
        wxDialUpManagerMSW *dialUpManager = wxDialUpManagerMSW::GetDialer();

        dialUpManager->OnDialProgress((RASCONNSTATE)wParam, lParam);
    }

    return 0;
}

static void WINAPI wxRasDialFunc(UINT unMsg,
                                 RASCONNSTATE rasconnstate,
                                 DWORD dwError)
{
    wxDialUpManagerMSW *dialUpManager = wxDialUpManagerMSW::GetDialer();

    wxCHECK_RET( dialUpManager, wxT("who started to dial then?") );

    SendMessage(dialUpManager->GetRasWindow(), wxWM_RAS_DIALING_PROGRESS,
                rasconnstate, dwError);
}

#endif
  // __BORLANDC__
#endif // wxUSE_DIALUP_MANAGER
