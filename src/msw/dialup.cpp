/////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/dialup.cpp
// Purpose:     MSW implementation of network/dialup classes and functions
// Author:      Vadim Zeitlin
// Modified by:
// Created:     07.07.99
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


#if wxUSE_DIALUP_MANAGER

#include "wx/dialup.h"

#ifndef WX_PRECOMP
    #include "wx/log.h"
    #include "wx/intl.h"
    #include "wx/event.h"
    #include "wx/app.h"
    #include "wx/timer.h"
    #include "wx/module.h"
#endif

#include "wx/generic/choicdgg.h"

#include "wx/msw/private.h"
#include "wx/msw/private/hiddenwin.h"
#include "wx/msw/private/event.h"
#include "wx/dynlib.h"

wxDEFINE_EVENT( wxEVT_DIALUP_CONNECTED, wxDialUpEvent );
wxDEFINE_EVENT( wxEVT_DIALUP_DISCONNECTED, wxDialUpEvent );

// Wine: no wininet.h
#if !defined(__WINE__)

#include <ras.h>
#include <raserror.h>

#include <wininet.h>

static const wxChar *
    wxMSWDIALUP_WNDCLASSNAME = wxT("_wxDialUpManager_Internal_Class");
static const wxChar *gs_classForDialUpWindow = nullptr;

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

    static const wxChar gs_funcSuffix = wxT('A');
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

    static const wxChar gs_funcSuffix = wxT('W');
#endif // ASCII/Unicode

// structure passed to the secondary thread
struct WXDLLEXPORT wxRasThreadData
{
    wxRasThreadData()
    {
        hWnd = 0;
        dialUpManager = nullptr;
    }

    ~wxRasThreadData()
    {
        if ( hWnd )
            DestroyWindow(hWnd);
    }

    HWND    hWnd;       // window to send notifications to
    wxWinAPI::Event hEventRas,  // automatic event which RAS signals when status changes
                    hEventQuit; // manual event which we signal when we terminate

    class WXDLLIMPEXP_FWD_CORE wxDialUpManagerMSW *dialUpManager;  // the owner
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
    virtual bool IsOk() const override;
    virtual size_t GetISPNames(wxArrayString& names) const override;
    virtual bool Dial(const wxString& nameOfISP,
                      const wxString& username,
                      const wxString& password,
                      bool async) override;
    virtual bool IsDialing() const override;
    virtual bool CancelDialing() override;
    virtual bool HangUp() override;
    virtual bool IsAlwaysOnline() const override;
    virtual bool IsOnline() const override;
    virtual void SetOnlineStatus(bool isOnline = true) override;
    virtual bool EnableAutoCheckOnlineStatus(size_t nSeconds) override;
    virtual void DisableAutoCheckOnlineStatus() override;
    virtual void SetWellKnownHost(const wxString& hostname, int port) override;
    virtual void SetConnectCommand(const wxString& commandDial,
                                   const wxString& commandHangup) override;

    // for RasTimer
    void CheckRasStatus();

    // for wxRasStatusWindowProc
    void OnConnectStatusChange();
    void OnDialProgress(RASCONNSTATE rasconnstate, DWORD dwError);

    // for wxRasDialFunc
    static HWND GetRasWindow() { return ms_hwndRas; }
    static void ResetRasWindow() { ms_hwndRas = nullptr; }
    static wxDialUpManagerMSW *GetDialer() { return ms_dialer; }

private:
    // return the error string for the given RAS error code
    static wxString GetErrorString(DWORD error);

    // find the (first) handle of the active connection
    static HRASCONN FindActiveConnection();

    // notify the application about status change
    void NotifyApp(bool connected, bool fromOurselves = false) const;

    // destroy the thread data and the thread itself
    void CleanUpThreadData();

    // number of times EnableAutoCheckOnlineStatus() had been called minus the
    // number of times DisableAutoCheckOnlineStatus() had been called
    int m_autoCheckLevel;

    // timer used for polling RAS status
    class WXDLLEXPORT RasTimer : public wxTimer
    {
    public:
        RasTimer(wxDialUpManagerMSW *dialUpManager)
            { m_dialUpManager = dialUpManager; }

        virtual void Notify() override { m_dialUpManager->CheckRasStatus(); }

    private:
        wxDialUpManagerMSW *m_dialUpManager;

        wxDECLARE_NO_COPY_CLASS(RasTimer);
    } m_timerStatusPolling;

    // thread handle for the thread sitting on connection change event
    HANDLE m_hThread;

    // data used by this thread and our hidden window to send messages between
    // each other
    wxRasThreadData *m_data;

    // the handle of rasapi32.dll when it's loaded
    wxDynamicLibrary m_dllRas;

    // the hidden window we use for passing messages between threads
    static HWND ms_hwndRas;

    // the handle of the connection we initiated or 0 if none
    static HRASCONN ms_hRasConnection;

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
    static RASCONNECTIONNOTIFICATION ms_pfnRasConnectionNotification;

    // if this flag is different from -1, it overrides IsOnline()
    static int ms_userSpecifiedOnlineStatus;

    // this flag tells us if we're online
    static int ms_isConnected;

    // this flag tells us whether a call to RasDial() is in progress
    static wxDialUpManagerMSW *ms_dialer;

    wxDECLARE_NO_COPY_CLASS(wxDialUpManagerMSW);
};

// module to destroy helper window created by wxDialUpManagerMSW
class wxDialUpManagerModule : public wxModule
{
public:
    bool OnInit() override { return true; }
    void OnExit() override
    {
        HWND hwnd = wxDialUpManagerMSW::GetRasWindow();
        if ( hwnd )
        {
            ::DestroyWindow(hwnd);
            wxDialUpManagerMSW::ResetRasWindow();
        }

        if ( gs_classForDialUpWindow )
        {
            ::UnregisterClass(wxMSWDIALUP_WNDCLASSNAME, wxGetInstance());
            gs_classForDialUpWindow = nullptr;
        }
    }

private:
    wxDECLARE_DYNAMIC_CLASS(wxDialUpManagerModule);
};

wxIMPLEMENT_DYNAMIC_CLASS(wxDialUpManagerModule, wxModule);

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
wxDialUpManagerMSW *wxDialUpManagerMSW::ms_dialer = nullptr;

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
                  : m_timerStatusPolling(this),
                    m_dllRas(wxT("RASAPI32"))
{
    // initialize our data
    m_autoCheckLevel = 0;
    m_hThread = 0;
    m_data = new wxRasThreadData;

    if ( !m_dllRas.IsLoaded() )
    {
        wxLogError(_("Dial up functions are unavailable because the remote access service (RAS) is not installed on this machine. Please install it."));
    }
    else if ( !ms_pfnRasDial )
    {
        // resolve the functions we need

        // this will contain the name of the function we failed to resolve
        // if any at the end
        const char *funcName = nullptr;

        // get the function from rasapi32.dll and abort if it's not found
        #define RESOLVE_RAS_FUNCTION(type, name)                          \
            ms_pfn##name = (type)m_dllRas.GetSymbol( wxString(wxT(#name))  \
                                                     + gs_funcSuffix);    \
            if ( !ms_pfn##name )                                          \
            {                                                             \
                funcName = #name;                                         \
                goto exit;                                                \
            }

        // a variant of above macro which doesn't abort if the function is
        // not found in the DLL
        #define RESOLVE_OPTIONAL_RAS_FUNCTION(type, name)                 \
            ms_pfn##name = (type)m_dllRas.GetSymbol( wxString(wxT(#name))  \
                                                     + gs_funcSuffix);

        RESOLVE_RAS_FUNCTION(RASDIAL, RasDial);
        RESOLVE_RAS_FUNCTION(RASENUMCONNECTIONS, RasEnumConnections);
        RESOLVE_RAS_FUNCTION(RASENUMENTRIES, RasEnumEntries);
        RESOLVE_RAS_FUNCTION(RASGETCONNECTSTATUS, RasGetConnectStatus);
        RESOLVE_RAS_FUNCTION(RASGETERRORSTRING, RasGetErrorString);
        RESOLVE_RAS_FUNCTION(RASHANGUP, RasHangUp);
        RESOLVE_RAS_FUNCTION(RASGETENTRYDIALPARAMS, RasGetEntryDialParams);

        // suppress error messages about missing (non essential) functions
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
            wxLogError(_("The version of remote access service (RAS) installed "
                          "on this machine is too old, please upgrade (the "
                          "following required function is missing: %s)."),
                       funcName);
            m_dllRas.Unload();
            return;
        }
    }

    // enable auto check by default
    EnableAutoCheckOnlineStatus(0);
}

wxDialUpManagerMSW::~wxDialUpManagerMSW()
{
    CleanUpThreadData();
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
            return wxSysErrorMsgStr(error);

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
            buffer[0] = (wxChar)wxTolower(buffer[0]);

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
            // an error occurred
            wxLogError(_("Cannot find active dialup connection: %s"),
                       GetErrorString(dwRet).c_str());
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
            wxFALLTHROUGH;

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
        if ( m_data->hEventQuit.Set() )
        {
            // sent quit request to the background thread

            // the thread still needs m_data so we can't free it here, rather
            // let the thread do it itself
            m_data = nullptr;
        }

        CloseHandle(m_hThread);

        m_hThread = 0;
    }

    wxDELETE(m_data);
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
                   GetErrorString(dwError).c_str());

        // we should still call RasHangUp() if we got a non 0 connection
        if ( ms_hRasConnection )
        {
            ms_pfnRasHangUp(ms_hRasConnection);
            ms_hRasConnection = 0;
        }

        ms_dialer = nullptr;

        NotifyApp(false /* !connected */, true /* we dialed ourselves */);
    }
    else if ( rasconnstate == RASCS_Connected )
    {
        ms_isConnected = true;
        ms_dialer = nullptr;

        NotifyApp(true /* connected */, true /* we dialed ourselves */);
    }
}

// ----------------------------------------------------------------------------
// implementation of wxDialUpManager functions
// ----------------------------------------------------------------------------

bool wxDialUpManagerMSW::IsOk() const
{
    return m_dllRas.IsLoaded();
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
                   wxRESERVED_PARAM,
                   nullptr,             // default phone book (or all)
                   rasEntries,          // [out] buffer for the entries
                   &size,               // [in/out] size of the buffer
                   &nEntries            // [out] number of entries fetched
                  );

        if ( dwRet == ERROR_BUFFER_TOO_SMALL )
        {
            // reallocate the buffer
            void *n  = realloc(rasEntries, size);
            if (n == nullptr)
            {
                free(rasEntries);
                return 0;
            }
            rasEntries = (RASENTRYNAME *)n;
        }
        else if ( dwRet != 0 )
        {
            // some other error - abort
            wxLogError(_("Failed to get ISP names: %s"),
                       GetErrorString(dwRet).c_str());

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
    wxCHECK_MSG( IsOk(), false, wxT("using uninitialized wxDialUpManager") );

    if ( ms_hRasConnection )
    {
        wxFAIL_MSG(wxT("there is already an active connection"));

        return true;
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

                return false;

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
                        return false;
                    }
                }
        }
    }

    RASDIALPARAMS rasDialParams;
    rasDialParams.dwSize = sizeof(rasDialParams);
    wxStrlcpy(rasDialParams.szEntryName, entryName.c_str(), RAS_MaxEntryName);

    // do we have the username and password?
    if ( !username || !password )
    {
        BOOL gotPassword;
        DWORD dwRet = ms_pfnRasGetEntryDialParams
                      (
                       nullptr,         // default phonebook
                       &rasDialParams,  // [in/out] the params of this entry
                       &gotPassword     // [out] did we get password?
                      );

        if ( dwRet != 0 )
        {
            wxLogError(_("Failed to connect: missing username/password."));

            return false;
        }
    }
    else
    {
        wxStrlcpy(rasDialParams.szUserName, username.c_str(), UNLEN);
        wxStrlcpy(rasDialParams.szPassword, password.c_str(), PWLEN);
    }

    // default values for other fields
    rasDialParams.szPhoneNumber[0] = '\0';
    rasDialParams.szCallbackNumber[0] = '\0';

    rasDialParams.szDomain[0] = '*';
    rasDialParams.szDomain[1] = '\0';

    // apparently, this is not really necessary - passing nullptr instead of the
    // phone book has the same effect
#if 0
    wxString phoneBook;
    if ( wxGetOsVersion() == wxWINDOWS_NT )
    {
        // first get the length
        UINT nLen = ::GetSystemDirectory(nullptr, 0);
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
                   nullptr,                 // no extended features
                   nullptr,                 // default phone book file (NT only)
                   &rasDialParams,
                   0,                       // use callback for notifications
                   async ? (void *)wxRasDialFunc  // cast needed for gcc 3.1
                         : 0,               // no notifications, sync operation
                   &ms_hRasConnection
                  );

    if ( dwRet != 0 )
    {
        // can't pass a wxWCharBuffer through ( ... )
        if ( async )
        {
            wxLogError(_("Failed to initiate dialup connection: %s"),
                       GetErrorString(dwRet).c_str());
        }
        else
        {
            wxLogError(_("Failed to establish dialup connection: %s"),
                       GetErrorString(dwRet).c_str());
        }

        // we should still call RasHangUp() if we got a non 0 connection
        if ( ms_hRasConnection )
        {
            ms_pfnRasHangUp(ms_hRasConnection);
            ms_hRasConnection = 0;
        }

        ms_dialer = nullptr;

        return false;
    }

    // for async dialing, we're not yet connected
    if ( !async )
    {
        ms_isConnected = true;
    }

    return true;
}

bool wxDialUpManagerMSW::IsDialing() const
{
    return GetDialer() != nullptr;
}

bool wxDialUpManagerMSW::CancelDialing()
{
    if ( !GetDialer() )
    {
        // silently ignore
        return false;
    }

    wxASSERT_MSG( ms_hRasConnection, wxT("dialing but no connection?") );

    ms_dialer = nullptr;

    return HangUp();
}

bool wxDialUpManagerMSW::HangUp()
{
    wxCHECK_MSG( IsOk(), false, wxT("using uninitialized wxDialUpManager") );

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

        return false;
    }

    // note that it's not an error if the connection had been already
    // terminated
    const DWORD dwRet = ms_pfnRasHangUp(hRasConn);
    if ( dwRet != 0 && dwRet != ERROR_NO_CONNECTION )
    {
        wxLogError(_("Failed to terminate the dialup connection: %s"),
                   GetErrorString(dwRet).c_str());
    }

    ms_isConnected = false;

    return true;
}

bool wxDialUpManagerMSW::IsAlwaysOnline() const
{
    // assume no permanent connection by default
    bool isAlwaysOnline = false;

    // try to use WinInet functions

    // NB: we could probably use wxDynamicLibrary here just as well,
    //     but we allow multiple instances of wxDialUpManagerMSW so
    //     we might as well use the ref counted version here too.

    wxDynamicLibrary hDll(wxT("WININET"));
    if ( hDll.IsLoaded() )
    {
        typedef BOOL (WINAPI *INTERNETGETCONNECTEDSTATE)(LPDWORD, DWORD);
        INTERNETGETCONNECTEDSTATE pfnInternetGetConnectedState;

        #define RESOLVE_FUNCTION(type, name) \
            pfn##name = (type)hDll.GetSymbol(wxT(#name))

        RESOLVE_FUNCTION(INTERNETGETCONNECTEDSTATE, InternetGetConnectedState);

        if ( pfnInternetGetConnectedState )
        {
            DWORD flags = 0;
            if ( pfnInternetGetConnectedState(&flags, wxRESERVED_PARAM) )
            {
                // there is some connection to the net, see of which type
                isAlwaysOnline = (flags & (INTERNET_CONNECTION_LAN |
                                           INTERNET_CONNECTION_PROXY)) != 0;
            }
            //else: no Internet connection at all
        }
    }

    return isAlwaysOnline;
}

bool wxDialUpManagerMSW::IsOnline() const
{
    wxCHECK_MSG( IsOk(), false, wxT("using uninitialized wxDialUpManager") );

    if ( IsAlwaysOnline() )
    {
        // always => now
        return true;
    }

    if ( ms_userSpecifiedOnlineStatus != -1 )
    {
        // user specified flag overrides our logic
        return ms_userSpecifiedOnlineStatus != 0;
    }
    else
    {
        // return true if there is at least one active connection
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
    wxCHECK_MSG( IsOk(), false, wxT("using uninitialized wxDialUpManager") );

    if ( m_autoCheckLevel++ )
    {
        // already checking
        return true;
    }

    bool ok = ms_pfnRasConnectionNotification != 0;

    if ( ok )
    {
        // first, see if we don't have this thread already running
        if ( m_hThread != 0 )
        {
            if ( ::ResumeThread(m_hThread) != (DWORD)-1 )
                return true;

            // we're leaving a zombie thread... but what else can we do?
            wxLogLastError(wxT("ResumeThread(RasThread)"));

            ok = false;
        }
    }

    // create all the stuff we need to be notified about RAS connection
    // status change

    if ( ok )
    {
        // first create an event to wait on
        if ( !m_data->hEventRas.Create
                            (
                             wxWinAPI::Event::AutomaticReset,
                             wxWinAPI::Event::Nonsignaled
                            ) )
        {
            wxLogLastError(wxT("CreateEvent(RasStatus)"));

            ok = false;
        }
    }

    if ( ok )
    {
        // create the event we use to quit the thread: using a manual event
        // here avoids problems with missing the event if wxDialUpManagerMSW
        // is created and destroyed immediately, before wxRasStatusWindowProc
        // starts waiting on the event
        if ( !m_data->hEventQuit.Create
                             (
                                wxWinAPI::Event::ManualReset,
                                wxWinAPI::Event::Nonsignaled
                             ) )
        {
            wxLogLastError(wxT("CreateEvent(RasThreadQuit)"));

            CleanUpThreadData();

            ok = false;
        }
    }

    if ( ok && !ms_hwndRas )
    {
        // create a hidden window to receive notification about connections
        // status change
        ms_hwndRas = wxCreateHiddenWindow
                     (
                        &gs_classForDialUpWindow,
                        wxMSWDIALUP_WNDCLASSNAME,
                        wxRasStatusWindowProc
                     );
        if ( !ms_hwndRas )
        {
            wxLogLastError(wxT("CreateWindow(RasHiddenWindow)"));

            CleanUpThreadData();

            ok = false;
        }
    }

    m_data->hWnd = ms_hwndRas;

    if ( ok )
    {
        // start the secondary thread
        m_data->dialUpManager = this;

        DWORD tid;
        m_hThread = CreateThread
                    (
                     nullptr,
                     0,
                     (LPTHREAD_START_ROUTINE)wxRasMonitorThread,
                     (void *)m_data,
                     0,
                     &tid
                    );

        if ( !m_hThread )
        {
            wxLogLastError(wxT("CreateThread(RasStatusThread)"));

            CleanUpThreadData();

            ok = false;
        }
    }

    if ( ok )
    {
        // start receiving RAS notifications
        DWORD dwRet = ms_pfnRasConnectionNotification
                      (
                        (HRASCONN)INVALID_HANDLE_VALUE,
                        m_data->hEventRas,
                        3 /* RASCN_Connection | RASCN_Disconnection */
                      );

        if ( dwRet != 0 )
        {
            wxLogDebug(wxT("RasConnectionNotification() failed: %s"),
                       GetErrorString(dwRet).c_str());

            CleanUpThreadData();
        }
        else
        {
            return true;
        }
    }

    m_timerStatusPolling.Stop();
    if ( nSeconds == 0 )
    {
        // default value
        nSeconds = 60;
    }
    m_timerStatusPolling.Start(nSeconds * 1000);

    return true;
}

void wxDialUpManagerMSW::DisableAutoCheckOnlineStatus()
{
    wxCHECK_RET( IsOk(), wxT("using uninitialized wxDialUpManager") );

    if ( --m_autoCheckLevel != 0 )
    {
        // still checking
        return;
    }

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

    bool cont = true;
    while ( cont )
    {
        DWORD dwRet = ::WaitForMultipleObjects(2, handles, FALSE, INFINITE);

        switch ( dwRet )
        {
            case WAIT_OBJECT_0:
                // RAS connection status changed
                SendMessage(data->hWnd, wxWM_RAS_STATUS_CHANGED,
                            0, (LPARAM)data);
                break;

            case WAIT_OBJECT_0 + 1:
                cont = false;
                break;

            default:
                wxFAIL_MSG( wxT("unexpected return of WaitForMultipleObjects()") );
                wxFALLTHROUGH;

            case WAIT_FAILED:
                // using wxLogLastError() from here is dangerous: we risk to
                // deadlock the main thread if wxLog sends output to GUI
                DWORD err = GetLastError();
                wxMessageOutputDebug dbg;
                dbg.Printf
                (
                    wxT("WaitForMultipleObjects(RasMonitor) failed: 0x%08lx (%s)"),
                    err,
                    wxSysErrorMsgStr(err)
                );

                // no sense in continuing, who knows if the handles we're
                // waiting for even exist yet...
                return (DWORD)-1;
        }
    }

    // we don't need it any more now and if this thread ran, it is our
    // responsibility to free the data
    delete data;

    return 0;
}

static LRESULT APIENTRY wxRasStatusWindowProc(HWND hWnd, UINT message,
                                              WPARAM wParam, LPARAM lParam)
{
    switch ( message )
    {
        case wxWM_RAS_STATUS_CHANGED:
            {
                wxRasThreadData *data = (wxRasThreadData *)lParam;
                data->dialUpManager->OnConnectStatusChange();
            }
            break;

        case wxWM_RAS_DIALING_PROGRESS:
            {
                wxDialUpManagerMSW *dialMan = wxDialUpManagerMSW::GetDialer();

                dialMan->OnDialProgress((RASCONNSTATE)wParam, lParam);
            }
            break;

        default:
            return ::DefWindowProc(hWnd, message, wParam, lParam);
    }

    return 0;
}

static void WINAPI wxRasDialFunc(UINT WXUNUSED(unMsg),
                                 RASCONNSTATE rasconnstate,
                                 DWORD dwError)
{
    wxDialUpManagerMSW *dialUpManager = wxDialUpManagerMSW::GetDialer();

    wxCHECK_RET( dialUpManager, wxT("who started to dial then?") );

    SendMessage(wxDialUpManagerMSW::GetRasWindow(), wxWM_RAS_DIALING_PROGRESS,
                rasconnstate, dwError);
}

#endif // #if !defined(__WINE__)

#endif // wxUSE_DIALUP_MANAGER
