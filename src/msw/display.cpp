/////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/display.cpp
// Purpose:     MSW Implementation of wxDisplay class
// Author:      Royce Mitchell III, Vadim Zeitlin
// Modified by: Ryan Norton (IsPrimary override)
// Created:     06/21/02
// Copyright:   (c) wxWidgets team
// Copyright:   (c) 2002-2006 wxWidgets team
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

#include "wx/msw/wrapwin.h"

#if wxUSE_DISPLAY

#include "wx/display.h"

#ifndef WX_PRECOMP
    #include "wx/dynarray.h"
    #include "wx/app.h"
    #include "wx/frame.h"
#endif

#include "wx/dynlib.h"
#include "wx/sysopt.h"

#include "wx/display_impl.h"
#include "wx/msw/missing.h"
#include "wx/msw/private.h"
#include "wx/msw/private/hiddenwin.h"

static const wxChar displayDllName[] = wxT("user32.dll");

// ----------------------------------------------------------------------------
// wxDisplayMSW declaration
// ----------------------------------------------------------------------------

class wxDisplayMSW : public wxDisplayImpl
{
public:
    wxDisplayMSW(unsigned n, HMONITOR hmon)
        : wxDisplayImpl(n),
          m_hmon(hmon)
    {
    }

    virtual wxRect GetGeometry() const wxOVERRIDE;
    virtual wxRect GetClientArea() const wxOVERRIDE;
    virtual wxString GetName() const wxOVERRIDE;
    virtual bool IsPrimary() const wxOVERRIDE;

    virtual wxVideoMode GetCurrentMode() const wxOVERRIDE;
    virtual wxArrayVideoModes GetModes(const wxVideoMode& mode) const wxOVERRIDE;
    virtual bool ChangeMode(const wxVideoMode& mode) wxOVERRIDE;

protected:
    // convert a DEVMODE to our wxVideoMode
    static wxVideoMode ConvertToVideoMode(const DEVMODE& dm)
    {
        // note that dmDisplayFrequency may be 0 or 1 meaning "standard one"
        // and although 0 is ok for us we don't want to return modes with 1hz
        // refresh
        return wxVideoMode(dm.dmPelsWidth,
                           dm.dmPelsHeight,
                           dm.dmBitsPerPel,
                           dm.dmDisplayFrequency > 1 ? dm.dmDisplayFrequency : 0);
    }

    // Call GetMonitorInfo() and fill in the provided struct and return true if
    // it succeeded, otherwise return false.
    bool GetMonInfo(MONITORINFOEX& monInfo) const;

    HMONITOR m_hmon;

private:
    wxDECLARE_NO_COPY_CLASS(wxDisplayMSW);
};


// ----------------------------------------------------------------------------
// wxDisplayFactoryMSW declaration
// ----------------------------------------------------------------------------

WX_DEFINE_ARRAY(HMONITOR, wxMonitorHandleArray);

class wxDisplayFactoryMSW : public wxDisplayFactory
{
public:
    // ctor checks if the current system supports multimon API and dynamically
    // bind the functions we need if this is the case and fills the
    // m_displays array if they're available
    wxDisplayFactoryMSW();

    // Dtor destroys the hidden window we use for getting WM_SETTINGCHANGE.
    virtual ~wxDisplayFactoryMSW();

    bool IsOk() const { return !m_displays.empty(); }

    virtual wxDisplayImpl *CreateDisplay(unsigned n) wxOVERRIDE;
    virtual unsigned GetCount() wxOVERRIDE { return unsigned(m_displays.size()); }
    virtual int GetFromPoint(const wxPoint& pt) wxOVERRIDE;
    virtual int GetFromWindow(const wxWindow *window) wxOVERRIDE;

    // Called when we receive WM_SETTINGCHANGE to refresh the list of monitor
    // handles.
    static void RefreshMonitors() { ms_factory->DoRefreshMonitors(); }


private:
    // EnumDisplayMonitors() callback
    static BOOL CALLBACK MultimonEnumProc(HMONITOR hMonitor,
                                          HDC hdcMonitor,
                                          LPRECT lprcMonitor,
                                          LPARAM dwData);

    // find the monitor corresponding to the given handle,
    // return wxNOT_FOUND if not found
    int FindDisplayFromHMONITOR(HMONITOR hmon) const;

    // Update m_displays array, used by RefreshMonitors().
    void DoRefreshMonitors();


    // The unique factory being used (as we don't have direct access to the
    // global factory pointer in the common code so we just duplicate this
    // variable (also making it of correct type for us) here).
    static wxDisplayFactoryMSW* ms_factory;


    // the array containing information about all available displays, filled by
    // MultimonEnumProc()
    wxMonitorHandleArray m_displays;

    // The hidden window we use for receiving WM_SETTINGCHANGE and its class
    // name.
    HWND m_hiddenHwnd;
    const wxChar* m_hiddenClass;

    wxDECLARE_NO_COPY_CLASS(wxDisplayFactoryMSW);
};

wxDisplayFactoryMSW* wxDisplayFactoryMSW::ms_factory = NULL;

// ----------------------------------------------------------------------------
// wxDisplay implementation
// ----------------------------------------------------------------------------

/* static */ wxDisplayFactory *wxDisplay::CreateFactory()
{
    wxDisplayFactoryMSW *factoryMM = new wxDisplayFactoryMSW;

    if ( factoryMM->IsOk() )
        return factoryMM;

    delete factoryMM;

    // fall back to a stub implementation if no multimon support (Win95?)
    return new wxDisplayFactorySingle;
}


// ----------------------------------------------------------------------------
// wxDisplayMSW implementation
// ----------------------------------------------------------------------------

bool wxDisplayMSW::GetMonInfo(MONITORINFOEX& monInfo) const
{
    if ( !::GetMonitorInfo(m_hmon, &monInfo) )
    {
        wxLogLastError(wxT("GetMonitorInfo"));
        return false;
    }

    return true;
}

wxRect wxDisplayMSW::GetGeometry() const
{
    WinStruct<MONITORINFOEX> monInfo;

    wxRect rect;
    if ( GetMonInfo(monInfo) )
        wxCopyRECTToRect(monInfo.rcMonitor, rect);

    return rect;
}

wxRect wxDisplayMSW::GetClientArea() const
{
    WinStruct<MONITORINFOEX> monInfo;

    wxRect rectClient;
    if ( GetMonInfo(monInfo) )
        wxCopyRECTToRect(monInfo.rcWork, rectClient);

    return rectClient;
}

wxString wxDisplayMSW::GetName() const
{
    WinStruct<MONITORINFOEX> monInfo;

    wxString name;
    if ( GetMonInfo(monInfo) )
        name = monInfo.szDevice;

    return name;
}

bool wxDisplayMSW::IsPrimary() const
{
    WinStruct<MONITORINFOEX> monInfo;

    if ( !GetMonInfo(monInfo) )
        return false;

    return (monInfo.dwFlags & MONITORINFOF_PRIMARY) != 0;
}

wxVideoMode wxDisplayMSW::GetCurrentMode() const
{
    wxVideoMode mode;

    // The first parameter of EnumDisplaySettings() must be NULL according
    // to MSDN, in order to specify the current display on the computer
    // on which the calling thread is running.
    const wxString name = GetName();
    const wxChar * const deviceName = name.empty()
                                          ? (const wxChar*)NULL
                                          : (const wxChar*)name.c_str();

    DEVMODE dm;
    dm.dmSize = sizeof(dm);
    dm.dmDriverExtra = 0;

    if ( !::EnumDisplaySettings(deviceName, ENUM_CURRENT_SETTINGS, &dm) )
    {
        wxLogLastError(wxT("EnumDisplaySettings(ENUM_CURRENT_SETTINGS)"));
    }
    else
    {
        mode = ConvertToVideoMode(dm);
    }

    return mode;
}

wxArrayVideoModes wxDisplayMSW::GetModes(const wxVideoMode& modeMatch) const
{
    wxArrayVideoModes modes;

    // The first parameter of EnumDisplaySettings() must be NULL according
    // to MSDN, in order to specify the current display on the computer
    // on which the calling thread is running.
    const wxString name = GetName();
    const wxChar * const deviceName = name.empty()
                                            ? (const wxChar*)NULL
                                            : (const wxChar*)name.c_str();

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

bool wxDisplayMSW::ChangeMode(const wxVideoMode& mode)
{
    // prepare ChangeDisplaySettingsEx() parameters
    DEVMODE dm;
    DEVMODE *pDevMode;

    int flags;

    if ( mode == wxDefaultVideoMode )
    {
        // reset the video mode to default
        pDevMode = NULL;
        flags = 0;
    }
    else // change to the given mode
    {
        wxCHECK_MSG( mode.GetWidth() && mode.GetHeight(), false,
                        wxT("at least the width and height must be specified") );

        wxZeroMemory(dm);
        dm.dmSize = sizeof(dm);
        dm.dmDriverExtra = 0;
        dm.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT;
        dm.dmPelsWidth = mode.GetWidth();
        dm.dmPelsHeight = mode.GetHeight();

        if ( mode.GetDepth() )
        {
            dm.dmFields |= DM_BITSPERPEL;
            dm.dmBitsPerPel = mode.GetDepth();
        }

        if ( mode.GetRefresh() )
        {
            dm.dmFields |= DM_DISPLAYFREQUENCY;
            dm.dmDisplayFrequency = mode.GetRefresh();
        }

        pDevMode = &dm;

        flags = CDS_FULLSCREEN;
    }


    // do change the mode
    switch ( ::ChangeDisplaySettingsEx
             (
                GetName().t_str(),  // display name
                pDevMode,           // dev mode or NULL to reset
                NULL,               // reserved
                flags,
                NULL                // pointer to video parameters (not used)
             ) )
    {
        case DISP_CHANGE_SUCCESSFUL:
            // ok
            {
                // If we have a top-level, full-screen frame, emulate
                // the DirectX behaviour and resize it.  This makes this
                // API quite a bit easier to use.
                wxWindow *winTop = wxTheApp->GetTopWindow();
                wxFrame *frameTop = wxDynamicCast(winTop, wxFrame);
                if (frameTop && frameTop->IsFullScreen())
                {
                    wxVideoMode current = GetCurrentMode();
                    frameTop->SetClientSize(current.GetWidth(), current.GetHeight());
                }
            }
            return true;

        case DISP_CHANGE_BADMODE:
            // don't complain about this, this is the only "expected" error
            break;

        default:
            wxFAIL_MSG( wxT("unexpected ChangeDisplaySettingsEx() return value") );
    }

    return false;
}


// ----------------------------------------------------------------------------
// wxDisplayFactoryMSW implementation
// ----------------------------------------------------------------------------

LRESULT APIENTRY
wxDisplayWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if ( msg == WM_SETTINGCHANGE )
    {
        wxDisplayFactoryMSW::RefreshMonitors();

        return 0;
    }

    return ::DefWindowProc(hwnd, msg, wParam, lParam);
}

wxDisplayFactoryMSW::wxDisplayFactoryMSW()
{
    // This is not supposed to happen with the current code, the factory is
    // implicitly a singleton.
    wxASSERT_MSG( !ms_factory, wxS("Using more than one factory?") );

    ms_factory = this;

    m_hiddenHwnd = NULL;
    m_hiddenClass = NULL;

    DoRefreshMonitors();

    // Also create a hidden window to listen for WM_SETTINGCHANGE that we
    // receive when a monitor is added to or removed from the system as we must
    // refresh our monitor handles information then.
    m_hiddenHwnd = wxCreateHiddenWindow
                   (
                    &m_hiddenClass,
                    wxT("wxDisplayHiddenWindow"),
                    wxDisplayWndProc
                   );
}

wxDisplayFactoryMSW::~wxDisplayFactoryMSW()
{
    if ( m_hiddenHwnd )
    {
        if ( !::DestroyWindow(m_hiddenHwnd) )
        {
            wxLogLastError(wxT("DestroyWindow(wxDisplayHiddenWindow)"));
        }

        if ( m_hiddenClass )
        {
            if ( !::UnregisterClass(m_hiddenClass, wxGetInstance()) )
            {
                wxLogLastError(wxT("UnregisterClass(wxDisplayHiddenWindow)"));
            }
        }
    }

    ms_factory = NULL;
}

void wxDisplayFactoryMSW::DoRefreshMonitors()
{
    m_displays.Clear();

    if ( !::EnumDisplayMonitors(NULL, NULL, MultimonEnumProc, (LPARAM)this) )
    {
        wxLogLastError(wxT("EnumDisplayMonitors"));
    }
}

/* static */
BOOL CALLBACK
wxDisplayFactoryMSW::MultimonEnumProc(
    HMONITOR hMonitor,              // handle to display monitor
    HDC WXUNUSED(hdcMonitor),       // handle to monitor-appropriate device context
    LPRECT WXUNUSED(lprcMonitor),   // pointer to monitor intersection rectangle
    LPARAM dwData)                  // data passed from EnumDisplayMonitors (this)
{
    wxDisplayFactoryMSW *const self = (wxDisplayFactoryMSW *)dwData;

    self->m_displays.Add(hMonitor);

    // continue the enumeration
    return TRUE;
}

wxDisplayImpl *wxDisplayFactoryMSW::CreateDisplay(unsigned n)
{
    wxCHECK_MSG( n < m_displays.size(), NULL, wxT("An invalid index was passed to wxDisplay") );

    return new wxDisplayMSW(n, m_displays[n]);
}

// helper for GetFromPoint() and GetFromWindow()
int wxDisplayFactoryMSW::FindDisplayFromHMONITOR(HMONITOR hmon) const
{
    if ( hmon )
    {
        const size_t count = m_displays.size();
        for ( size_t n = 0; n < count; n++ )
        {
            if ( hmon == m_displays[n] )
                return n;
        }
    }

    return wxNOT_FOUND;
}

int wxDisplayFactoryMSW::GetFromPoint(const wxPoint& pt)
{
    POINT pt2;
    pt2.x = pt.x;
    pt2.y = pt.y;

    return FindDisplayFromHMONITOR(::MonitorFromPoint(pt2,
                                                       MONITOR_DEFAULTTONULL));
}

int wxDisplayFactoryMSW::GetFromWindow(const wxWindow *window)
{
#ifdef __WXMSW__
    return FindDisplayFromHMONITOR(::MonitorFromWindow(GetHwndOf(window),
                                                        MONITOR_DEFAULTTONULL));
#else
    const wxSize halfsize = window->GetSize() / 2;
    wxPoint pt = window->GetScreenPosition();
    pt.x += halfsize.x;
    pt.y += halfsize.y;
    return GetFromPoint(pt);
#endif
}

#endif // wxUSE_DISPLAY

void wxClientDisplayRect(int *x, int *y, int *width, int *height)
{
    // Determine the desktop dimensions minus the taskbar and any other
    // special decorations...
    RECT r;

    SystemParametersInfo(SPI_GETWORKAREA, 0, &r, 0);
    if (x)      *x = r.left;
    if (y)      *y = r.top;
    if (width)  *width = r.right - r.left;
    if (height) *height = r.bottom - r.top;
}
