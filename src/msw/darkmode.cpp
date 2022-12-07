///////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/darkmode.cpp
// Purpose:     Support for dark mode in wxMSW
// Author:      Vadim Zeitlin
// Created:     2022-06-24
// Copyright:   (c) 2022 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

/*
    The code in this file is based on the following sources:

    - win32-darkmode by Richard Yu (https://github.com/ysc3839/win32-darkmode)
 */

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// for compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

// Allow predefining this as 0 to disable dark mode support completely.
#ifndef wxUSE_DARK_MODE
    // Otherwise enable it by default.
    #define wxUSE_DARK_MODE 1
#endif

#if wxUSE_DARK_MODE

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/bitmap.h"
    #include "wx/dcmemory.h"
    #include "wx/image.h"
    #include "wx/log.h"
#endif // WX_PRECOMP

#include "wx/dynlib.h"
#include "wx/module.h"

#include "wx/msw/dc.h"
#include "wx/msw/uxtheme.h"

static const char* TRACE_DARKMODE = "msw-darkmode";

#define DWMWA_USE_IMMERSIVE_DARK_MODE 20

namespace
{

// Constants for use with SetPreferredAppMode().
enum PreferredAppMode
{
    AppMode_Default,
    AppMode_AllowDark,
    AppMode_ForceDark,
    AppMode_ForceLight
};

PreferredAppMode gs_appMode = AppMode_Default;

template <typename T>
bool TryLoadByOrd(T& func, const wxDynamicLibrary& lib, int ordinal)
{
    func = (T)::GetProcAddress(lib.GetLibHandle(), MAKEINTRESOURCEA(ordinal));
    if ( !func )
    {
        wxLogTrace(TRACE_DARKMODE,
                   "Required function with ordinal %d not found", ordinal);
        return false;
    }

    return true;
}

} // anonymous namespace

// ============================================================================
// implementation
// ============================================================================

namespace wxMSWImpl
{

// Global pointers of the functions we use: they're not only undocumented, but
// don't appear in the SDK headers at all.
BOOL (WINAPI *ShouldAppsUseDarkMode)() = nullptr;
BOOL (WINAPI *AllowDarkModeForWindow)(HWND hwnd, BOOL allow) = nullptr;
DWORD (WINAPI *SetPreferredAppMode)(DWORD) = nullptr;

bool InitDarkMode()
{
    // Note: for simplicity, we support dark mode only in Windows 10 v2004
    // ("20H1", build number 19041) and later, even if, in principle, it could
    // be supported as far back as v1809 (build 17763) -- but very few people
    // must still use it by now and so it just doesn't seem to be worth it.
    if ( !wxCheckOsVersion(10, 0, 19041) )
    {
        wxLogTrace(TRACE_DARKMODE, "Unsupported due to OS version");
        return false;
    }

    wxLoadedDLL dllUxTheme(wxS("uxtheme.dll"));

    // These functions are not only undocumented but are not even exported by
    // name, and have to be resolved using their ordinals.
    return TryLoadByOrd(ShouldAppsUseDarkMode, dllUxTheme, 132) &&
           TryLoadByOrd(AllowDarkModeForWindow, dllUxTheme, 133) &&
           TryLoadByOrd(SetPreferredAppMode, dllUxTheme, 135);
}

// This function is only used in this file as it's more clear than using
// IsActive() without the namespace name -- but in the rest of our code, it's
// IsActive() which is more clear.
bool ShouldUseDarkMode()
{
    switch ( gs_appMode )
    {
        case AppMode_Default:
            // Dark mode support not enabled, don't try using dark mode.
            return false;

        case AppMode_AllowDark:
            // Follow the global setting.
            return wxMSWImpl::ShouldAppsUseDarkMode();

        case AppMode_ForceDark:
            return true;

        case AppMode_ForceLight:
            return false;
    }

    wxFAIL_MSG( "unreachable" );

    return false;
}

} // namespace wxMSWImpl

// ----------------------------------------------------------------------------
// Module keeping dark mode-related data
// ----------------------------------------------------------------------------

namespace
{

// This function is documented, but we still load it dynamically to avoid
// having to link with dwmapi.lib.
typedef HRESULT
(WINAPI *DwmSetWindowAttribute_t)(HWND, DWORD, const void*, DWORD);

} // anonymous namespace

class wxDarkModeModule : public wxModule
{
public:
    virtual bool OnInit() override { return true; }
    virtual void OnExit() override
    {
        ms_pfnDwmSetWindowAttribute = (DwmSetWindowAttribute_t)-1;
        ms_dllDWM.Unload();
    }

    static DwmSetWindowAttribute_t GetDwmSetWindowAttribute()
    {
        if ( ms_pfnDwmSetWindowAttribute == (DwmSetWindowAttribute_t)-1 )
        {
            ms_dllDWM.Load(wxS("dwmapi.dll"), wxDL_VERBATIM | wxDL_QUIET);
            wxDL_INIT_FUNC(ms_pfn, DwmSetWindowAttribute, ms_dllDWM);
        }

        return ms_pfnDwmSetWindowAttribute;
    }

private:
    static wxDynamicLibrary ms_dllDWM;
    static DwmSetWindowAttribute_t ms_pfnDwmSetWindowAttribute;

    wxDECLARE_DYNAMIC_CLASS(wxDarkModeModule);
};

wxIMPLEMENT_DYNAMIC_CLASS(wxDarkModeModule, wxModule);

wxDynamicLibrary wxDarkModeModule::ms_dllDWM;

DwmSetWindowAttribute_t
wxDarkModeModule::ms_pfnDwmSetWindowAttribute = (DwmSetWindowAttribute_t)-1;

// ----------------------------------------------------------------------------
// Public API
// ----------------------------------------------------------------------------

bool wxApp::MSWEnableDarkMode(int flags)
{
    if ( !wxMSWImpl::InitDarkMode() )
        return false;

    const PreferredAppMode mode = flags & DarkMode_Always ? AppMode_ForceDark
                                                          : AppMode_AllowDark;
    const DWORD rc = wxMSWImpl::SetPreferredAppMode(mode);

    // It's supposed to return the old mode normally.
    if ( rc != static_cast<DWORD>(gs_appMode) )
    {
        wxLogTrace(TRACE_DARKMODE,
                   "SetPreferredAppMode(%d) unexpectedly returned %d",
                   mode, rc);
    }

    gs_appMode = mode;

    return true;
}

// ----------------------------------------------------------------------------
// Supporting functions for the rest of wxMSW code
// ----------------------------------------------------------------------------

namespace wxMSWDarkMode
{

bool IsActive()
{
    return wxMSWImpl::ShouldUseDarkMode();
}

void EnableForTLW(HWND hwnd)
{
    // Nothing to do, dark mode support not enabled or dark mode is not used.
    if ( !wxMSWImpl::ShouldUseDarkMode() )
        return;

    BOOL useDarkMode = TRUE;
    HRESULT hr = wxDarkModeModule::GetDwmSetWindowAttribute()
                 (
                    hwnd,
                    DWMWA_USE_IMMERSIVE_DARK_MODE,
                    &useDarkMode,
                    sizeof(useDarkMode)
                 );
    if ( FAILED(hr) )
        wxLogApiError("DwmSetWindowAttribute(USE_IMMERSIVE_DARK_MODE)", hr);

    wxMSWImpl::AllowDarkModeForWindow(hwnd, TRUE);
}

void AllowForWindow(HWND hwnd, const wchar_t* themeClass)
{
    if ( wxMSWImpl::ShouldUseDarkMode() )
    {
        wxMSWImpl::AllowDarkModeForWindow(hwnd, TRUE);

        // For some reason using a theme class is incompatible with using
        // "Explorer" as the app name, even though it looks like it ought to
        // be, but passing ("Explorer", "ExplorerStatusBar") here, for example,
        // does _not_ work, while omitting the app name in this case does work.
        ::SetWindowTheme(hwnd, themeClass ? nullptr : L"Explorer", themeClass);
    }
}

wxColour GetColour(wxSystemColour index)
{
    // This is not great at all, but better than using light mode colours that
    // are not appropriate for the dark mode.
    switch ( index )
    {
        case wxSYS_COLOUR_ACTIVECAPTION:
        case wxSYS_COLOUR_APPWORKSPACE:
        case wxSYS_COLOUR_INFOBK:
        case wxSYS_COLOUR_LISTBOX:
        case wxSYS_COLOUR_WINDOW:
            return wxColour(0x202020);

        case wxSYS_COLOUR_BTNTEXT:
        case wxSYS_COLOUR_CAPTIONTEXT:
        case wxSYS_COLOUR_HIGHLIGHTTEXT:
        case wxSYS_COLOUR_INFOTEXT:
        case wxSYS_COLOUR_LISTBOXHIGHLIGHTTEXT:
        case wxSYS_COLOUR_LISTBOXTEXT:
        case wxSYS_COLOUR_MENUTEXT:
        case wxSYS_COLOUR_WINDOWTEXT:
            return wxColour(0xe0e0e0);

        case wxSYS_COLOUR_SCROLLBAR:
            return wxColour(0x4d4d4d);

        case wxSYS_COLOUR_INACTIVECAPTION:
        case wxSYS_COLOUR_MENU:
            return wxColour(0x2b2b2b);

        case wxSYS_COLOUR_BTNFACE:
            return wxColour(0x333333);

        case wxSYS_COLOUR_MENUBAR:
            return wxColour(0x626262);

        case wxSYS_COLOUR_MENUHILIGHT:
            return wxColour(0x353535);

        case wxSYS_COLOUR_HIGHLIGHT:
            return wxColour(0x777777);

        case wxSYS_COLOUR_INACTIVECAPTIONTEXT:
            return wxColour(0xaaaaaa);

        case wxSYS_COLOUR_3DDKSHADOW:
        case wxSYS_COLOUR_3DLIGHT:
        case wxSYS_COLOUR_ACTIVEBORDER:
        case wxSYS_COLOUR_BTNHIGHLIGHT:
        case wxSYS_COLOUR_BTNSHADOW:
        case wxSYS_COLOUR_DESKTOP:
        case wxSYS_COLOUR_GRADIENTACTIVECAPTION:
        case wxSYS_COLOUR_GRADIENTINACTIVECAPTION:
        case wxSYS_COLOUR_GRAYTEXT:
        case wxSYS_COLOUR_HOTLIGHT:
        case wxSYS_COLOUR_INACTIVEBORDER:
        case wxSYS_COLOUR_WINDOWFRAME:
            return wxColour();

        case wxSYS_COLOUR_MAX:
            break;
    }

    wxFAIL_MSG( "unreachable" );
    return wxColour();
}

HBRUSH GetBackgroundBrush()
{
    wxBrush* const brush =
        wxTheBrushList->FindOrCreateBrush(GetColour(wxSYS_COLOUR_WINDOW));

    return brush ? GetHbrushOf(*brush) : 0;
}

bool PaintIfNecessary(wxWindow* w)
{
#if wxUSE_IMAGE
    if ( !wxMSWImpl::ShouldUseDarkMode() )
        return false;

    const wxSize size = w->GetClientSize();

    // Don't bother doing anything with the empty windows.
    if ( size == wxSize() )
        return false;

    // Ask the control to paint itself on the given bitmap.
    wxBitmap bmp(size);
    {
        wxMemoryDC mdc(bmp);
        w->MSWDefWindowProc(WM_PAINT, (WPARAM)GetHdcOf(mdc), 0);
    }

    wxImage image = bmp.ConvertToImage();

    unsigned char *data = image.GetData();
    for ( int i = 0; i < size.x*size.y; ++i, data += 3 )
    {
        wxImage::RGBValue rgb(data[0], data[1], data[2]);
        wxImage::HSVValue hsv = wxImage::RGBtoHSV(rgb);

        // There is no really good way to convert normal colours to dark mode,
        // but try to do a bit better than just inverting the value because
        // this results in colours which are much too dark.
        hsv.value = sqrt(1.0 - hsv.value*hsv.value);

        rgb = wxImage::HSVtoRGB(hsv);
        data[0] = rgb.red;
        data[1] = rgb.green;
        data[2] = rgb.blue;
    }

    PAINTSTRUCT ps;
    wxDCTemp dc(::BeginPaint(GetHwndOf(w), &ps), size);
    dc.DrawBitmap(wxBitmap(image), 0, 0);
    ::EndPaint(GetHwndOf(w), &ps);

    return true;
#else // !wxUSE_IMAGE
    wxUnusedVar(w);

    return false;
#endif
}

} // namespace wxMSWDarkMode

#else // !wxUSE_DARK_MODE

bool wxApp::MSWEnableDarkMode(int WXUNUSED(flags))
{
    return false;
}

namespace wxMSWDarkMode
{

bool IsActive()
{
    return false;
}

void EnableForTLW(HWND WXUNUSED(hwnd))
{
}

void AllowForWindow(HWND WXUNUSED(hwnd), const wchar_t* WXUNUSED(themeClass))
{
}

wxColour GetColour(wxSystemColour WXUNUSED(index))
{
    return wxColour();
}

HBRUSH GetBackgroundBrush()
{
    return 0;
}

bool PaintIfNecessary(wxWindow* WXUNUSED(w))
{
    return false;
}

} // namespace wxMSWDarkMode

#endif // wxUSE_DARK_MODE/!wxUSE_DARK_MODE
