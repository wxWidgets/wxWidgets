/////////////////////////////////////////////////////////////////////////////
// Name:        display.cpp
// Purpose:     MSW Implementation of wxDisplay class
// Author:      Royce Mitchell III
// Modified by:
// Created:     06/21/02
// RCS-ID:      $Id$
// Copyright:   (c) wxWindows team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ===========================================================================
// declarations
// ===========================================================================

// ---------------------------------------------------------------------------
// headers
// ---------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "display.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_DISPLAY

#ifndef WX_PRECOMP
   #include "wx/dynarray.h"
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
    // as (m)any standard header, this one doesn't compile without warnings
    // with VC++ 6 <sigh>
    #pragma warning(disable:4706)
#endif

#include <multimon.h>

#ifdef _MSC_VER
    #pragma warning(default:4706)
#endif

#ifdef _UNICODE
    #define MAKE_WFUNC(x) #x "W"
    #define WINFUNC(x)  L ## MAKE_WFUNC(x)
#else
    #define WINFUNC(x) #x "A"
#endif

// ----------------------------------------------------------------------------
// typedefs
// ----------------------------------------------------------------------------

typedef LONG (WINAPI *ChangeDisplaySettingsEx_t)(LPCTSTR lpszDeviceName,
                                                 LPDEVMODE lpDevMode,
                                                 HWND hwnd,
                                                 DWORD dwFlags,
                                                 LPVOID lParam);

// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------

class wxDisplayInfo
{
public:
    // handle of this monitor used by MonitorXXX() functions, never NULL
    HMONITOR m_hmon;

    // the entire area of this monitor in virtual screen coordinates
    wxRect m_rect;

    // the display device name for this monitor, empty initially and retrieved
    // on demand by DoGetName()
    wxString m_devName;

    wxDisplayInfo() { m_hmon = NULL; }
};

WX_DECLARE_OBJARRAY(wxDisplayInfo, wxDisplayInfoArray);
#include "wx/arrimpl.cpp"
WX_DEFINE_OBJARRAY(wxDisplayInfoArray);

// this is not really MT-unsafe as wxDisplay is only going to be used from the
// main thread, i.e. we consider that it's a GUI class and so don't protect it
static wxDisplayInfoArray *g_displays = NULL;


// this module is used to cleanup g_displays array
class wxDisplayModule : public wxModule
{
public:
    virtual bool OnInit() { return TRUE; }
    virtual void OnExit();

    DECLARE_DYNAMIC_CLASS(wxDisplayModule)
};

IMPLEMENT_DYNAMIC_CLASS(wxDisplayModule, wxModule)

// ===========================================================================
// implementation
// ===========================================================================

// ----------------------------------------------------------------------------
// local functions
// ----------------------------------------------------------------------------

static BOOL CALLBACK wxmswMonitorEnumProc (
  HMONITOR hMonitor,  // handle to display monitor
  HDC hdcMonitor,     // handle to monitor-appropriate device context (NULL)
  LPRECT lprcMonitor, // pointer to monitor intersection rectangle
  LPARAM dwData       // data passed from EnumDisplayMonitors (unused)
)
{
    wxDisplayInfo* info = new wxDisplayInfo();

    // we need hMonitor to be able to map display id to it which is needed for
    // MonitorXXX() functions, in particular MonitorFromPoint()
    info->m_hmon = hMonitor;

    // we also store the display geometry
    info->m_rect.SetX ( lprcMonitor->left );
    info->m_rect.SetY ( lprcMonitor->top );
    info->m_rect.SetWidth ( lprcMonitor->right - lprcMonitor->left );
    info->m_rect.SetHeight ( lprcMonitor->bottom - lprcMonitor->top );

    // now add this monitor to the array
    g_displays->Add(info);

    // continue the enumeration
    return TRUE;
}

// this function must be called before accessing g_displays array as it
// creates and initializes it
static void InitDisplays()
{
    if ( g_displays )
        return;

    g_displays = new wxDisplayInfoArray();

    // enumerate all displays
    if ( !::EnumDisplayMonitors(NULL, NULL, wxmswMonitorEnumProc, 0) )
    {
        wxLogLastError(wxT("EnumDisplayMonitors"));

        // TODO: still create at least one (valid) entry in g_displays for the
        //       primary display!
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
    delete g_displays;
}

// ---------------------------------------------------------------------------
// wxDisplay
// ---------------------------------------------------------------------------

// helper of GetFromPoint() and GetFromWindow()
static int DisplayFromHMONITOR(HMONITOR hmon)
{
    if ( hmon )
    {
        const size_t count = wxDisplay::GetCount();

        for ( size_t n = 0; n < count; n++ )
        {
            if ( hmon == (*g_displays)[n].m_hmon )
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
    wxASSERT_MSG( g_displays->GetCount() == (size_t)::GetSystemMetrics(SM_CMONITORS),
                    _T("So how many displays does this system have?") );

    return g_displays->GetCount();
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

wxDisplay::wxDisplay ( size_t n )
         : wxDisplayBase ( n )
{
    // if we do this in ctor we won't have to call it from all the member
    // functions
    InitDisplays();
}

wxRect wxDisplay::GetGeometry() const
{
    return (*g_displays)[m_index].m_rect;
}

wxString wxDisplay::GetName() const
{
    wxDisplayInfo& dpyInfo = (*g_displays)[m_index];
    if ( dpyInfo.m_devName.empty() )
    {
        MONITORINFOEX monInfo;
        wxZeroMemory(monInfo);
        monInfo.cbSize = sizeof(monInfo);

        if ( !::GetMonitorInfo(dpyInfo.m_hmon, &monInfo) )
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

wxArrayVideoModes wxDisplay::GetModes(const wxVideoMode& modeMatch) const
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

bool wxDisplay::ChangeMode(const wxVideoMode& mode)
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
        wxCHECK_MSG( mode.w && mode.h, FALSE,
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
            return TRUE;

        case DISP_CHANGE_BADMODE:
            // don't complain about this, this is the only "expected" error
            break;

        default:
            wxFAIL_MSG( _T("unexpected ChangeDisplaySettingsEx() return value") );
    }

    return FALSE;
}

#endif // wxUSE_DISPLAY

