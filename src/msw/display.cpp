/////////////////////////////////////////////////////////////////////////////
// Name:        display.cpp
// Purpose:     MSW Implementation of wxDisplay class
// Author:      Royce Mitchell III
// Modified by: VZ (resolutions enumeration/change support, DirectDraw, ...)
//		    Ryan Norton (IsPrimary override)
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

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma implementation "display.h"
#endif

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

// the following define is necessary to access the multi-monitor function
// declarations in a manner safe to use w/ Windows 95
#define COMPILE_MULTIMON_STUBS

// if you don't have multimon.h you can download the file from:
//
//  http://www.microsoft.com/msj/0697/monitor/monitortextfigs.htm#fig4
//

#ifdef _MSC_VER
    // as (m)any standard header(s), this one doesn't compile without warnings
    // with VC++ 6 <sigh>
    #pragma warning(disable:4706)
#endif

#include <multimon.h>

#ifdef _MSC_VER
    #pragma warning(default:4706)
#endif

#include <ddraw.h>

// we don't want to link with ddraw.lib which contains the real
// IID_IDirectDraw2 definition
const GUID wxIID_IDirectDraw2 =
    { 0xB3A6F3E0, 0x2B43, 0x11CF, { 0xA2,0xDE,0x00,0xAA,0x00,0xB9,0x33,0x56 } };

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

// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------

class wxDisplayInfo
{
public:
    // handle of this monitor used by MonitorXXX() functions, never NULL
    HMONITOR m_hmon;

    // IDirectDraw object used to control this display, may be NULL
    IDirectDraw2 *m_pDD2;

    // DirectDraw GUID for this display, only valid when using DirectDraw
    GUID m_guid;

    // the entire area of this monitor in virtual screen coordinates
    wxRect m_rect;

    // the display device name for this monitor, empty initially and retrieved
    // on demand by DoGetName()
    wxString m_devName;

    wxDisplayInfo() { m_hmon = NULL; m_pDD2 = NULL; }
    ~wxDisplayInfo() { if ( m_pDD2 ) m_pDD2->Release(); }
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

// do we use DirectX?
static bool gs_useDirectX = false;

// dynamically resolved DirectDrawCreate()
static DirectDrawCreate_t gs_DirectDrawCreate = NULL;

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

// ----------------------------------------------------------------------------
// local functions
// ----------------------------------------------------------------------------

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

// initialize gs_displays using the standard Windows functions
static void DoInitStdWindows()
{
    // enumerate all displays
    if ( !::EnumDisplayMonitors(NULL, NULL, wxmswMonitorEnumProc, 0) )
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

    if ( !gs_useDirectX || !DoInitDirectX() )
    {
        // either we were told not to try to use DirectX or fall back to std
        // functions if DirectX method failed
        gs_useDirectX = false;

        DoInitStdWindows();
    }
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

// emulation of ChangeDisplaySettingsEx() for Win95
LONG WINAPI ChangeDisplaySettingsExForWin95(LPCTSTR WXUNUSED(lpszDeviceName),
                                            LPDEVMODE lpDevMode,
                                            HWND WXUNUSED(hwnd),
                                            DWORD dwFlags,
                                            LPVOID WXUNUSED(lParam))
{
    return ::ChangeDisplaySettings(lpDevMode, dwFlags);
}

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

    gs_useDirectX = useDX;
}

// helper of GetFromPoint() and GetFromWindow()
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
    InitDisplays();

    // I'm not sure if they really always return the same thing and if this is
    // not true I'd like to know in which situation does it happen
    wxASSERT_MSG( gs_displays->GetCount() == (size_t)::GetSystemMetrics(SM_CMONITORS),
                    _T("So how many displays does this system have?") );

    return gs_displays->GetCount();
}

/* static */
int wxDisplayBase::GetFromPoint ( const wxPoint& pt )
{
    POINT pt2;
    pt2.x = pt.x;
    pt2.y = pt.y;

    return DisplayFromHMONITOR(::MonitorFromPoint(pt2, MONITOR_DEFAULTTONULL));
}

/* static */
int wxDisplayBase::GetFromWindow(wxWindow *window)
{
    return DisplayFromHMONITOR
           (
            ::MonitorFromWindow(GetHwndOf(window), MONITOR_DEFAULTTONULL)
           );
}

// ----------------------------------------------------------------------------
// wxDisplay ctor/dtor
// ----------------------------------------------------------------------------

wxDisplay::wxDisplay ( size_t n )
         : wxDisplayBase ( n )
{
    // if we do this in ctor we won't have to call it from all the member
    // functions
    InitDisplays();

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
}

wxDisplay::~wxDisplay()
{
    wxDisplayInfo& dpyInfo = (*gs_displays)[m_index];

    LPDIRECTDRAW2& pDD2 = dpyInfo.m_pDD2;
    if ( pDD2 )
    {
        pDD2->Release();
    }
}

// ----------------------------------------------------------------------------
// wxDisplay simple accessors
// ----------------------------------------------------------------------------

bool wxDisplay::IsOk() const
{
    return m_index < GetCount() &&
                (!gs_useDirectX || (*gs_displays)[m_index].m_pDD2);
}

wxRect wxDisplay::GetGeometry() const
{
    wxDisplayInfo& dpyInfo = (*gs_displays)[m_index];
    wxRect& rect = dpyInfo.m_rect;
    if ( !rect.width )
    {
        MONITORINFO monInfo;
        wxZeroMemory(monInfo);
        monInfo.cbSize = sizeof(monInfo);

        if ( !::GetMonitorInfo(dpyInfo.m_hmon, &monInfo) )
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
        if ( !::GetMonitorInfo(dpyInfo.m_hmon, (LPMONITORINFO)&monInfo) )
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

wxString wxDisplay::GetNameForEnumSettings() const
{
    int major, minor;
    const bool isWin95 = wxGetOsVersion(&major, &minor) == wxWIN95 &&
                            major == 4 && minor == 0;

    // the first parameter of EnumDisplaySettings() must be NULL under Win95
    // according to MSDN but GetMonitorInfo() stub in multimon.h still returns
    // something even in this case, so we have to correct this manually
    wxString name;
    if ( !isWin95 )
        name = GetName();

    return name;
}

// ----------------------------------------------------------------------------
// determine if this is the primary display
// ----------------------------------------------------------------------------

bool wxDisplay::IsPrimary() const
{
    wxDisplayInfo& dpyInfo = (*gs_displays)[m_index];

    MONITORINFOEX monInfo;
    wxZeroMemory(monInfo);
    monInfo.cbSize = sizeof(monInfo);

    // NB: Cast from MONITORINFOEX* to MONITORINFO* is done because
    //     Mingw headers - unlike the ones from Microsoft's Platform SDK -
    //     don't derive the former from the latter in C++ mode and so
    //     the pointer's type is not converted implicitly.
    if ( !::GetMonitorInfo(dpyInfo.m_hmon, (LPMONITORINFO)&monInfo) )
    {
        wxLogLastError(_T("GetMonitorInfo"));
    }

    return (monInfo.dwFlags & MONITORINFOF_PRIMARY) == MONITORINFOF_PRIMARY;
}

// ----------------------------------------------------------------------------
// video modes enumeration
// ----------------------------------------------------------------------------

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

wxArrayVideoModes
wxDisplay::DoGetModesWindows(const wxVideoMode& modeMatch) const
{
    wxArrayVideoModes modes;

    const wxString name = GetNameForEnumSettings();

    const wxChar * const deviceName = name.empty() ? NULL : name.c_str();

    DEVMODE dm;
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
    return gs_useDirectX ? DoGetModesDirectX(modeMatch)
                         : DoGetModesWindows(modeMatch);
}

wxVideoMode wxDisplay::GetCurrentMode() const
{
    wxVideoMode mode;

    const wxString name = GetNameForEnumSettings();

    DEVMODE dm;
    if ( !::EnumDisplaySettings(name.empty() ? NULL : name.c_str(),
                                ENUM_CURRENT_SETTINGS,
                                &dm) )
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

        flags = CDS_FULLSCREEN;
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

        if ( !pfnChangeDisplaySettingsEx )
        {
            // we must be under Win95 and so there is no multiple monitors
            // support anyhow
            pfnChangeDisplaySettingsEx = ChangeDisplaySettingsExForWin95;
        }
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
    return gs_useDirectX ? DoChangeModeDirectX(mode)
                         : DoChangeModeWindows(mode);
}

#endif // wxUSE_DISPLAY

