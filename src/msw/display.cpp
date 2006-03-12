/////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/display.cpp
// Purpose:     MSW Implementation of wxDisplay class
// Author:      Royce Mitchell III
// Modified by: VZ (resolutions enumeration/change support, DirectDraw, ...)
//              Ryan Norton (IsPrimary override)
// Created:     06/21/02
// RCS-ID:      $Id$
// Copyright:   (c) wxWidgets team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ===========================================================================
// declarations
// ===========================================================================

// ---------------------------------------------------------------------------
// headers
// ---------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_DISPLAY

#ifndef WX_PRECOMP
   #include "wx/app.h"
   #include "wx/dynarray.h"
   #include "wx/frame.h"
#endif

#include "wx/dynload.h"

#include "wx/display.h"

// Mingw's w32api headers don't include ddraw.h, though the user may have
// installed it.  If using configure, we actually probe for ddraw.h there
// and set HAVE_DDRAW_H.  Otherwise, assume we don't have it if using
// the w32api headers, and that we do otherwise.
#if !defined HAVE_W32API_H && !defined HAVE_DDRAW_H
#define HAVE_DDRAW_H 1
#endif

#ifndef __WXWINCE__
    // Older versions of windef.h don't define HMONITOR.  Unfortunately, we
    // can't directly test whether HMONITOR is defined or not in windef.h as
    // it's not a macro but a typedef, so we test for an unrelated symbol which
    // is only defined in winuser.h if WINVER >= 0x0500
    #if !defined(HMONITOR_DECLARED) && !defined(MNS_NOCHECK)
        DECLARE_HANDLE(HMONITOR);
        #define HMONITOR_DECLARED
    #endif
#endif // !__WXWINCE__

#ifdef HAVE_DDRAW_H
#include <ddraw.h>

// we don't want to link with ddraw.lib which contains the real
// IID_IDirectDraw2 definition
const GUID wxIID_IDirectDraw2 =
    { 0xB3A6F3E0, 0x2B43, 0x11CF, { 0xA2,0xDE,0x00,0xAA,0x00,0xB9,0x33,0x56 } };
#endif

// ----------------------------------------------------------------------------
// macros
// ----------------------------------------------------------------------------

#ifdef _UNICODE
    #define WINFUNC(x)  _T(#x) L"W"
#else
    #define WINFUNC(x) #x "A"
#endif

// ----------------------------------------------------------------------------
// typedefs for dynamically loaded Windows functions
// ----------------------------------------------------------------------------

typedef LONG (WINAPI *ChangeDisplaySettingsEx_t)(LPCTSTR lpszDeviceName,
                                                 LPDEVMODE lpDevMode,
                                                 HWND hwnd,
                                                 DWORD dwFlags,
                                                 LPVOID lParam);

#ifdef HAVE_DDRAW_H
typedef BOOL (PASCAL *DDEnumExCallback_t)(GUID *pGuid,
                                          LPTSTR driverDescription,
                                          LPTSTR driverName,
                                          LPVOID lpContext,
                                          HMONITOR hmon);

typedef HRESULT (WINAPI *DirectDrawEnumerateEx_t)(DDEnumExCallback_t lpCallback,
                                                  LPVOID lpContext,
                                                  DWORD dwFlags);

typedef HRESULT (WINAPI *DirectDrawCreate_t)(GUID *lpGUID,
                                             LPDIRECTDRAW *lplpDD,
                                             IUnknown *pUnkOuter);
#endif

typedef BOOL (WINAPI *EnumDisplayMonitors_t)(HDC,LPCRECT,MONITORENUMPROC,LPARAM);
typedef HMONITOR (WINAPI *MonitorFromPoint_t)(POINT,DWORD);
typedef HMONITOR (WINAPI *MonitorFromWindow_t)(HWND,DWORD);
typedef BOOL (WINAPI *GetMonitorInfo_t)(HMONITOR,LPMONITORINFO);

static EnumDisplayMonitors_t gs_EnumDisplayMonitors = NULL;
static MonitorFromPoint_t gs_MonitorFromPoint = NULL;
static MonitorFromWindow_t gs_MonitorFromWindow = NULL;
static GetMonitorInfo_t gs_GetMonitorInfo = NULL;

// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------

class wxDisplayInfo
{
public:
    // handle of this monitor used by MonitorXXX() functions, never NULL
    HMONITOR m_hmon;

    // IDirectDraw object used to control this display, may be NULL
#ifdef HAVE_DDRAW_H
    IDirectDraw2 *m_pDD2;
#else
    void *m_pDD2;
#endif

    // DirectDraw GUID for this display, only valid when using DirectDraw
    GUID m_guid;

    // the entire area of this monitor in virtual screen coordinates
    wxRect m_rect;

    // the display device name for this monitor, empty initially and retrieved
    // on demand by DoGetName()
    wxString m_devName;

    wxDisplayInfo() { m_hmon = NULL; m_pDD2 = NULL; }
    ~wxDisplayInfo() {
#ifdef HAVE_DDRAW_H
        if ( m_pDD2 ) m_pDD2->Release();
#endif
    }
};

WX_DECLARE_OBJARRAY(wxDisplayInfo, wxDisplayInfoArray);
#include "wx/arrimpl.cpp"
WX_DEFINE_OBJARRAY(wxDisplayInfoArray);

// this module is used to cleanup gs_displays array
class wxDisplayModule : public wxModule
{
public:
    virtual bool OnInit() { return true; }
    virtual void OnExit();

    DECLARE_DYNAMIC_CLASS(wxDisplayModule)
};

IMPLEMENT_DYNAMIC_CLASS(wxDisplayModule, wxModule)

// ----------------------------------------------------------------------------
// globals
// ----------------------------------------------------------------------------

#ifdef HAVE_DDRAW_H
// do we use DirectX?
static bool gs_useDirectX = false;
#endif

// Try to look up the functions needed for supporting multiple monitors.  If
// they aren't available (probably because we're running on Win95 or NT4 which
// predate this API), set a flag which makes wxDisplay return results for a
// single screen.
static bool OsSupportsMultipleMonitors()
{
    static int isNewEnough = -1;
    if ( isNewEnough == -1 )
    {
        isNewEnough = 0;
        wxDynamicLibrary dllUser32(_T("user32.dll"));
        // Check for one of the symbols to avoid logging errors just because
        // we happen to be running on Win95 or NT4.
        if ( dllUser32.IsLoaded() &&
             dllUser32.HasSymbol(wxT("EnumDisplayMonitors")) )
        {
            // GetMonitorInfo has Unicode/ANSI variants, the others don't.
            gs_EnumDisplayMonitors = (EnumDisplayMonitors_t)
                dllUser32.GetSymbol(wxT("EnumDisplayMonitors"));
            gs_MonitorFromPoint = (MonitorFromPoint_t)
                dllUser32.GetSymbol(wxT("MonitorFromPoint"));
            gs_MonitorFromWindow = (MonitorFromWindow_t)
                dllUser32.GetSymbol(wxT("MonitorFromWindow"));
            gs_GetMonitorInfo = (GetMonitorInfo_t)
                dllUser32.GetSymbol(WINFUNC(GetMonitorInfo));
            if ( gs_EnumDisplayMonitors != NULL &&
                 gs_MonitorFromPoint != NULL &&
                 gs_MonitorFromWindow != NULL &&
                 gs_GetMonitorInfo != NULL )
            {
                isNewEnough = 1;
            }
        }
    }
    return (isNewEnough != 0);
}

#ifdef HAVE_DDRAW_H
// dynamically resolved DirectDrawCreate()
static DirectDrawCreate_t gs_DirectDrawCreate = NULL;
#endif

// this is not really MT-unsafe as wxDisplay is only going to be used from the
// main thread, i.e. we consider that it's a GUI class and so don't protect it
static wxDisplayInfoArray *gs_displays = NULL;

// ===========================================================================
// implementation
// ===========================================================================

// ----------------------------------------------------------------------------
// callbacks for monitor/modes enumeration stuff
// ----------------------------------------------------------------------------

static BOOL CALLBACK wxmswMonitorEnumProc (
  HMONITOR hMonitor,        // handle to display monitor
  HDC WXUNUSED(hdcMonitor), // handle to monitor-appropriate device context
  LPRECT lprcMonitor,       // pointer to monitor intersection rectangle
  LPARAM WXUNUSED(dwData)   // data passed from EnumDisplayMonitors (unused)
)
{
    wxDisplayInfo *info = new wxDisplayInfo();

    // we need hMonitor to be able to map display id to it which is needed for
    // MonitorXXX() functions, in particular MonitorFromPoint()
    info->m_hmon = hMonitor;

    // we also store the display geometry
    info->m_rect.SetX ( lprcMonitor->left );
    info->m_rect.SetY ( lprcMonitor->top );
    info->m_rect.SetWidth ( lprcMonitor->right - lprcMonitor->left );
    info->m_rect.SetHeight ( lprcMonitor->bottom - lprcMonitor->top );

    // now add this monitor to the array
    gs_displays->Add(info);

    // continue the enumeration
    return true;
}

#ifdef HAVE_DDRAW_H
BOOL PASCAL
wxDDEnumExCallback(GUID *pGuid,
                   LPTSTR WXUNUSED(driverDescription),
                   LPTSTR driverName,
                   LPVOID WXUNUSED(lpContext),
                   HMONITOR hmon)
{
    if ( pGuid )
    {
        wxDisplayInfo *info = new wxDisplayInfo();

        info->m_hmon = hmon;
        info->m_guid = *pGuid;
        info->m_devName = driverName;

        gs_displays->Add(info);
    }
    //else: we're called for the primary monitor, skip it

    // continue the enumeration
    return true;
}

HRESULT WINAPI wxDDEnumModesCallback(LPDDSURFACEDESC lpDDSurfaceDesc,
                                     LPVOID lpContext)
{
    // we need at least the mode size
    static const DWORD FLAGS_REQUIRED = DDSD_HEIGHT | DDSD_WIDTH;
    if ( (lpDDSurfaceDesc->dwFlags & FLAGS_REQUIRED) == FLAGS_REQUIRED )
    {
        wxArrayVideoModes * const modes = (wxArrayVideoModes *)lpContext;

        modes->Add(wxVideoMode(lpDDSurfaceDesc->dwWidth,
                               lpDDSurfaceDesc->dwHeight,
                               lpDDSurfaceDesc->ddpfPixelFormat.dwRGBBitCount,
                               lpDDSurfaceDesc->dwRefreshRate));
    }

    // continue the enumeration
    return DDENUMRET_OK;
}
#endif

// ----------------------------------------------------------------------------
// local functions
// ----------------------------------------------------------------------------

#ifdef HAVE_DDRAW_H
// initialize gs_displays using DirectX functions
static bool DoInitDirectX()
{
#if wxUSE_LOG
    // suppress the errors if ddraw.dll is not found
    wxLog::EnableLogging(false);
#endif

    wxDynamicLibrary dllDX(_T("ddraw.dll"));

#if wxUSE_LOG
    wxLog::EnableLogging();
#endif

    if ( !dllDX.IsLoaded() )
        return false;

    DirectDrawEnumerateEx_t pDDEnumEx = (DirectDrawEnumerateEx_t)
        dllDX.GetSymbol(WINFUNC(DirectDrawEnumerateEx));
    if ( !pDDEnumEx )
        return false;

    // we'll also need DirectDrawCreate() later, resolve it right now
    gs_DirectDrawCreate = (DirectDrawCreate_t)
        dllDX.GetSymbol(_T("DirectDrawCreate"));
    if ( !gs_DirectDrawCreate )
        return false;

    if ( (*pDDEnumEx)(wxDDEnumExCallback,
                      NULL,
                      DDENUM_ATTACHEDSECONDARYDEVICES) != DD_OK )
    {
        return false;
    }

    // ok, it seems like we're going to use DirectDraw and so we're going to
    // need ddraw.dll all the time, don't unload it
    dllDX.Detach();

    return true;
}
#endif

// initialize gs_displays using the standard Windows functions
static void DoInitStdWindows()
{
    // enumerate all displays
    if ( !gs_EnumDisplayMonitors(NULL, NULL, wxmswMonitorEnumProc, 0) )
    {
        wxLogLastError(wxT("EnumDisplayMonitors"));

        // TODO: still create at least one (valid) entry in gs_displays for the
        //       primary display!
    }
}

// this function must be called before accessing gs_displays array as it
// creates and initializes it
static void InitDisplays()
{
    if ( gs_displays )
        return;

    gs_displays = new wxDisplayInfoArray();

#ifdef HAVE_DDRAW_H
    if ( !gs_useDirectX || !DoInitDirectX() )
    {
        // either we were told not to try to use DirectX or fall back to std
        // functions if DirectX method failed
        gs_useDirectX = false;

        DoInitStdWindows();
    }
#else
    DoInitStdWindows();
#endif
}

// convert a DEVMODE to our wxVideoMode
wxVideoMode ConvertToVideoMode(const DEVMODE& dm)
{
    // note that dmDisplayFrequency may be 0 or 1 meaning "standard one" and
    // although 0 is ok for us we don't want to return modes with 1hz refresh
    return wxVideoMode(dm.dmPelsWidth,
                       dm.dmPelsHeight,
                       dm.dmBitsPerPel,
                       dm.dmDisplayFrequency > 1 ? dm.dmDisplayFrequency : 0);
}

#ifndef __WXWINCE__
// emulation of ChangeDisplaySettingsEx() for Win95
LONG WINAPI ChangeDisplaySettingsExForWin95(LPCTSTR WXUNUSED(lpszDeviceName),
                                            LPDEVMODE lpDevMode,
                                            HWND WXUNUSED(hwnd),
                                            DWORD dwFlags,
                                            LPVOID WXUNUSED(lParam))
{
    return ::ChangeDisplaySettings(lpDevMode, dwFlags);
}
#endif // !__WXWINCE__

// ----------------------------------------------------------------------------
// wxDisplayModule
// ----------------------------------------------------------------------------

void wxDisplayModule::OnExit()
{
    delete gs_displays;
}

// ---------------------------------------------------------------------------
// wxDisplay
// ---------------------------------------------------------------------------

/* static */
void wxDisplay::UseDirectX(bool useDX)
{
    wxCHECK_RET( !gs_displays, _T("it is too late to call UseDirectX") );

#ifdef HAVE_DDRAW_H
    // DirectDrawEnumerateEx requires Win98 or Win2k anyway.
    if ( OsSupportsMultipleMonitors() ) gs_useDirectX = useDX;
#endif
}

// helper for GetFromPoint() and GetFromWindow()
static int DisplayFromHMONITOR(HMONITOR hmon)
{
    if ( hmon )
    {
        const size_t count = wxDisplay::GetCount();

        for ( size_t n = 0; n < count; n++ )
        {
            if ( hmon == (*gs_displays)[n].m_hmon )
                return n;
        }
    }

    return wxNOT_FOUND;
}

/* static */
size_t wxDisplayBase::GetCount()
{
    if ( !OsSupportsMultipleMonitors() ) return 1;

    InitDisplays();

    //RN: FIXME:  This is wrong - the display info array should reload after every call
    //to GetCount() - otherwise it will not be accurate.
    //The user can change the number of displays in display properties/settings
    //after GetCount or similar is called and really mess this up...
    //wxASSERT_MSG( gs_displays->GetCount() == (size_t)::GetSystemMetrics(SM_CMONITORS),
    //                _T("So how many displays does this system have?") );

    return gs_displays->GetCount();
}

/* static */
int wxDisplayBase::GetFromPoint ( const wxPoint& pt )
{
    if ( !OsSupportsMultipleMonitors() )
    {
        const wxSize size = wxGetDisplaySize();
        if (pt.x >= 0 && pt.x < size.GetWidth() &&
            pt.y >= 0 && pt.y < size.GetHeight())
        {
            return 0;
        }
        return wxNOT_FOUND;
    }

    POINT pt2;
    pt2.x = pt.x;
    pt2.y = pt.y;

    return DisplayFromHMONITOR(gs_MonitorFromPoint(pt2, MONITOR_DEFAULTTONULL));
}

/* static */
int wxDisplayBase::GetFromWindow(wxWindow *window)
{
    if ( !OsSupportsMultipleMonitors() )
    {
        const wxRect r(window->GetRect());
        const wxSize size = wxGetDisplaySize();
        if (r.x < size.GetWidth() && r.x + r.width >= 0 &&
            r.y < size.GetHeight() && r.y + r.height >= 0)
        {
            return 0;
        }
        return wxNOT_FOUND;
    }

    return DisplayFromHMONITOR
           (
            gs_MonitorFromWindow(GetHwndOf(window), MONITOR_DEFAULTTONULL)
           );
}

// ----------------------------------------------------------------------------
// wxDisplay ctor/dtor
// ----------------------------------------------------------------------------

wxDisplay::wxDisplay ( size_t n )
         : wxDisplayBase ( n )
{
    if ( !OsSupportsMultipleMonitors() ) return;

    // if we do this in ctor we won't have to call it from all the member
    // functions
    InitDisplays();

#ifdef HAVE_DDRAW_H
    if ( gs_useDirectX )
    {
        wxDisplayInfo& dpyInfo = (*gs_displays)[n];

        LPDIRECTDRAW2& pDD2 = dpyInfo.m_pDD2;
        if ( !pDD2 )
        {
            if ( !gs_DirectDrawCreate )
            {
                // what to do??
                return;
            }

            IDirectDraw *pDD;
            HRESULT hr = (*gs_DirectDrawCreate)(&dpyInfo.m_guid, &pDD, NULL);

            if ( FAILED(hr) || !pDD )
            {
                // what to do??
                wxLogApiError(_T("DirectDrawCreate"), hr);
            }
            else // got IDirectDraw, we want IDirectDraw2
            {
                hr = pDD->QueryInterface(wxIID_IDirectDraw2, (void **)&pDD2);
                if ( FAILED(hr) || !pDD2 )
                {
                    wxLogApiError(_T("IDirectDraw::QueryInterface(IDD2)"), hr);
                }

                pDD->Release();
            }
        }
        //else: DirectDraw object corresponding to our display already exists

        // increment its ref count to account for Release() in dtor
        //
        // NB: pDD2 will be only really Release()d when gs_displays is
        //     destroyed which is ok as we don't want to recreate DD objects
        //     all the time
        pDD2->AddRef();
    }
#endif
}

wxDisplay::~wxDisplay()
{
#ifdef HAVE_DDRAW_H
    if ( !OsSupportsMultipleMonitors() ) return;

    wxDisplayInfo& dpyInfo = (*gs_displays)[m_index];

    LPDIRECTDRAW2& pDD2 = dpyInfo.m_pDD2;
    if ( pDD2 )
    {
        pDD2->Release();
    }
#endif
}

// ----------------------------------------------------------------------------
// wxDisplay simple accessors
// ----------------------------------------------------------------------------

bool wxDisplay::IsOk() const
{
#ifdef HAVE_DDRAW_H
    return m_index < GetCount() &&
                (!gs_useDirectX || (*gs_displays)[m_index].m_pDD2);
#else
    return m_index < GetCount();
#endif
}

wxRect wxDisplay::GetGeometry() const
{
    if ( !OsSupportsMultipleMonitors() )
    {
        wxSize size = wxGetDisplaySize();
        return wxRect(0, 0, size.GetWidth(), size.GetHeight());
    }

    wxDisplayInfo& dpyInfo = (*gs_displays)[m_index];
    wxRect& rect = dpyInfo.m_rect;
    if ( !rect.width )
    {
        MONITORINFO monInfo;
        wxZeroMemory(monInfo);
        monInfo.cbSize = sizeof(monInfo);

        if ( !gs_GetMonitorInfo(dpyInfo.m_hmon, &monInfo) )
        {
            wxLogLastError(_T("GetMonitorInfo"));
        }
        else
        {
            wxCopyRECTToRect(monInfo.rcMonitor, rect);
        }
    }

    return rect;
}

wxString wxDisplay::GetName() const
{
    if ( !OsSupportsMultipleMonitors() ) return wxT("");

    wxDisplayInfo& dpyInfo = (*gs_displays)[m_index];
    if ( dpyInfo.m_devName.empty() )
    {
        MONITORINFOEX monInfo;
        wxZeroMemory(monInfo);
        monInfo.cbSize = sizeof(monInfo);

        // NB: Cast from MONITORINFOEX* to MONITORINFO* is done because
        //     Mingw headers - unlike the ones from Microsoft's Platform SDK -
        //     don't derive the former from the latter in C++ mode and so
        //     the pointer's type is not converted implicitly.
        if ( !gs_GetMonitorInfo(dpyInfo.m_hmon, (LPMONITORINFO)&monInfo) )
        {
            wxLogLastError(_T("GetMonitorInfo"));
        }
        else
        {
            dpyInfo.m_devName = monInfo.szDevice;
        }
    }

    return dpyInfo.m_devName;
}

// ----------------------------------------------------------------------------
// determine if this is the primary display
// ----------------------------------------------------------------------------

bool wxDisplay::IsPrimary() const
{
    if ( !OsSupportsMultipleMonitors() ) return true;

    wxDisplayInfo& dpyInfo = (*gs_displays)[m_index];

    MONITORINFOEX monInfo;
    wxZeroMemory(monInfo);
    monInfo.cbSize = sizeof(monInfo);

    // NB: Cast from MONITORINFOEX* to MONITORINFO* is done because
    //     Mingw headers - unlike the ones from Microsoft's Platform SDK -
    //     don't derive the former from the latter in C++ mode and so
    //     the pointer's type is not converted implicitly.
    if ( !gs_GetMonitorInfo(dpyInfo.m_hmon, (LPMONITORINFO)&monInfo) )
    {
        wxLogLastError(_T("GetMonitorInfo"));
    }

    return (monInfo.dwFlags & MONITORINFOF_PRIMARY) == MONITORINFOF_PRIMARY;
}

// ----------------------------------------------------------------------------
// video modes enumeration
// ----------------------------------------------------------------------------

#ifdef HAVE_DDRAW_H
wxArrayVideoModes
wxDisplay::DoGetModesDirectX(const wxVideoMode& WXUNUSED(modeMatch)) const
{
    wxArrayVideoModes modes;

    IDirectDraw2 *pDD = (*gs_displays)[m_index].m_pDD2;

    if ( pDD )
    {
        HRESULT hr = pDD->EnumDisplayModes
                          (
                            DDEDM_REFRESHRATES,
                            NULL,   // all modes (TODO: use modeMatch!)
                            &modes,   // callback parameter
                            wxDDEnumModesCallback
                          );

        if ( FAILED(hr) )
        {
            wxLogApiError(_T("IDirectDraw::EnumDisplayModes"), hr);
        }
    }

    return modes;
}
#endif

wxArrayVideoModes
wxDisplay::DoGetModesWindows(const wxVideoMode& modeMatch) const
{
    wxArrayVideoModes modes;

    // The first parameter of EnumDisplaySettings() must be NULL under Win95
    // according to MSDN.  The version of GetName() we implement for Win95
    // returns an empty string.
    const wxString name = GetName();
    const wxChar * const deviceName = name.empty() ? NULL : name.c_str();

    DEVMODE dm;
    dm.dmSize = sizeof(dm);
    dm.dmDriverExtra = 0;
    for ( int iModeNum = 0;
          ::EnumDisplaySettings(deviceName, iModeNum, &dm);
          iModeNum++ )
    {
        const wxVideoMode mode = ConvertToVideoMode(dm);
        if ( mode.Matches(modeMatch) )
        {
            modes.Add(mode);
        }
    }

    return modes;
}

wxArrayVideoModes wxDisplay::GetModes(const wxVideoMode& modeMatch) const
{
#ifdef HAVE_DDRAW_H
    return gs_useDirectX ? DoGetModesDirectX(modeMatch)
                         : DoGetModesWindows(modeMatch);
#else
    return DoGetModesWindows(modeMatch);
#endif
}

wxVideoMode wxDisplay::GetCurrentMode() const
{
    wxVideoMode mode;

    // The first parameter of EnumDisplaySettings() must be NULL under Win95
    // according to MSDN.  The version of GetName() we implement for Win95
    // returns an empty string.
    const wxString name = GetName();
    const wxChar * const deviceName = name.empty() ? NULL : name.c_str();

    DEVMODE dm;
    dm.dmSize = sizeof(dm);
    dm.dmDriverExtra = 0;
    if ( !::EnumDisplaySettings(deviceName, ENUM_CURRENT_SETTINGS, &dm) )
    {
        wxLogLastError(_T("EnumDisplaySettings(ENUM_CURRENT_SETTINGS)"));
    }
    else
    {
        mode = ConvertToVideoMode(dm);
    }

    return mode;
}

// ----------------------------------------------------------------------------
// video mode switching
// ----------------------------------------------------------------------------

#ifdef HAVE_DDRAW_H
bool wxDisplay::DoChangeModeDirectX(const wxVideoMode& mode)
{
    IDirectDraw2 *pDD = (*gs_displays)[m_index].m_pDD2;
    if ( !pDD )
        return false;

    wxWindow *winTop = wxTheApp->GetTopWindow();
    wxCHECK_MSG( winTop, false, _T("top level window required for DirectX") );

    HRESULT hr = pDD->SetCooperativeLevel
                      (
                        GetHwndOf(winTop),
                        DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN
                      );
    if ( FAILED(hr) )
    {
        wxLogApiError(_T("IDirectDraw::SetCooperativeLevel"), hr);

        return false;
    }

    hr = pDD->SetDisplayMode(mode.w, mode.h, mode.bpp, mode.refresh, 0);
    if ( FAILED(hr) )
    {
        wxLogApiError(_T("IDirectDraw::SetDisplayMode"), hr);

        return false;
    }


    return true;
}
#endif

bool wxDisplay::DoChangeModeWindows(const wxVideoMode& mode)
{
    // prepare ChangeDisplaySettingsEx() parameters
    DEVMODE dm,
           *pDevMode;
    int flags;

    if ( mode == wxDefaultVideoMode )
    {
        // reset the video mode to default
        pDevMode = NULL;
        flags = 0;
    }
    else // change to the given mode
    {
        wxCHECK_MSG( mode.w && mode.h, false,
                        _T("at least the width and height must be specified") );

        wxZeroMemory(dm);
        dm.dmSize = sizeof(dm);
        dm.dmDriverExtra = 0;
        dm.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT;
        dm.dmPelsWidth = mode.w;
        dm.dmPelsHeight = mode.h;

        if ( mode.bpp )
        {
            dm.dmFields |= DM_BITSPERPEL;
            dm.dmBitsPerPel = mode.bpp;
        }

        if ( mode.refresh )
        {
            dm.dmFields |= DM_DISPLAYFREQUENCY;
            dm.dmDisplayFrequency = mode.refresh;
        }

        pDevMode = &dm;

#ifdef __WXWINCE__
        flags = 0;
#else // !__WXWINCE__
        flags = CDS_FULLSCREEN;
#endif // __WXWINCE__/!__WXWINCE__
    }


    // get pointer to the function dynamically
    //
    // we're only called from the main thread, so it's ok to use static
    // variable
    static ChangeDisplaySettingsEx_t pfnChangeDisplaySettingsEx = NULL;
    if ( !pfnChangeDisplaySettingsEx )
    {
        wxDynamicLibrary dllUser32(_T("user32.dll"));
        if ( dllUser32.IsLoaded() )
        {
            pfnChangeDisplaySettingsEx = (ChangeDisplaySettingsEx_t)
                dllUser32.GetSymbol(WINFUNC(ChangeDisplaySettingsEx));
        }
        //else: huh, no user32.dll??

#ifndef __WXWINCE__
        if ( !pfnChangeDisplaySettingsEx )
        {
            // we must be under Win95 and so there is no multiple monitors
            // support anyhow
            pfnChangeDisplaySettingsEx = ChangeDisplaySettingsExForWin95;
        }
#endif // !__WXWINCE__
    }

    // do change the mode
    switch ( pfnChangeDisplaySettingsEx
             (
                GetName(),      // display name
                pDevMode,       // dev mode or NULL to reset
                NULL,           // reserved
                flags,
                NULL            // pointer to video parameters (not used)
             ) )
    {
        case DISP_CHANGE_SUCCESSFUL:
            // ok
            {
                // If we have a top-level, full-screen frame, emulate
                // the DirectX behavior and resize it.  This makes this
                // API quite a bit easier to use.
                wxWindow *winTop = wxTheApp->GetTopWindow();
                wxFrame *frameTop = wxDynamicCast(winTop, wxFrame);
                if (frameTop && frameTop->IsFullScreen())
                {
                    wxVideoMode current = GetCurrentMode();
                    frameTop->SetClientSize(current.w, current.h);
                }
            }
            return true;

        case DISP_CHANGE_BADMODE:
            // don't complain about this, this is the only "expected" error
            break;

        default:
            wxFAIL_MSG( _T("unexpected ChangeDisplaySettingsEx() return value") );
    }

    return false;
}

bool wxDisplay::ChangeMode(const wxVideoMode& mode)
{
#ifdef HAVE_DDRAW_H
    return gs_useDirectX ? DoChangeModeDirectX(mode)
                         : DoChangeModeWindows(mode);
#else
    return DoChangeModeWindows(mode);
#endif
}

#endif // wxUSE_DISPLAY

