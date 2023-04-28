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
    - UAH menu by adzm (https://github.com/adzm/win32-custom-menubar-aero-theme)
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

#include "wx/msw/darkmode.h"
#include "wx/msw/dc.h"
#include "wx/msw/uxtheme.h"

#include "wx/msw/private/darkmode.h"

#include <memory>

#if wxUSE_LOG_TRACE
static const char* TRACE_DARKMODE = "msw-darkmode";
#endif // wxUSE_LOG_TRACE

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
//
// Note that, not being public, they use C++ bool type and not Win32 BOOL.
bool (WINAPI *ShouldAppsUseDarkMode)() = nullptr;
bool (WINAPI *AllowDarkModeForWindow)(HWND hwnd, bool allow) = nullptr;
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
        ms_settings.reset();

        ms_pfnDwmSetWindowAttribute = (DwmSetWindowAttribute_t)-1;
        ms_dllDWM.Unload();
    }

    // Takes ownership of the provided pointer.
    static void SetSettings(wxDarkModeSettings* settings)
    {
        ms_settings.reset(settings);
    }

    // Returns the currently used settings: may only be called when the dark
    // mode is on.
    static wxDarkModeSettings& GetSettings()
    {
        return *ms_settings;
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

    static std::unique_ptr<wxDarkModeSettings> ms_settings;

    wxDECLARE_DYNAMIC_CLASS(wxDarkModeModule);
};

wxIMPLEMENT_DYNAMIC_CLASS(wxDarkModeModule, wxModule);

wxDynamicLibrary wxDarkModeModule::ms_dllDWM;
std::unique_ptr<wxDarkModeSettings> wxDarkModeModule::ms_settings;

DwmSetWindowAttribute_t
wxDarkModeModule::ms_pfnDwmSetWindowAttribute = (DwmSetWindowAttribute_t)-1;

// ----------------------------------------------------------------------------
// Public API
// ----------------------------------------------------------------------------

bool wxApp::MSWEnableDarkMode(int flags, wxDarkModeSettings* settings)
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

    // Set up the settings to use, allocating a default one if none specified.
    if ( !settings )
        settings = new wxDarkModeSettings();

    wxDarkModeModule::SetSettings(settings);

    return true;
}

// ----------------------------------------------------------------------------
// Default wxDarkModeSettings implementation
// ----------------------------------------------------------------------------

// Implemented here to ensure that it's generated inside the DLL.
wxDarkModeSettings::~wxDarkModeSettings() = default;

wxColour wxDarkModeSettings::GetColour(wxSystemColour index)
{
    // This is not great at all, but better than using light mode colours that
    // are not appropriate for the dark mode.
    //
    // There is also an undocumented GetImmersiveColorFromColorSetEx(), but it
    // doesn't seem to return any colours with the values that are actually
    // used in e.g. Explorer in the dark mode, such as 0x202020 background.
    switch ( index )
    {
        case wxSYS_COLOUR_BTNSHADOW:
            return *wxBLACK;

        case wxSYS_COLOUR_ACTIVECAPTION:
        case wxSYS_COLOUR_APPWORKSPACE:
        case wxSYS_COLOUR_INFOBK:
        case wxSYS_COLOUR_LISTBOX:
        case wxSYS_COLOUR_WINDOW:
        case wxSYS_COLOUR_BTNFACE:
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

        case wxSYS_COLOUR_HOTLIGHT:
            return wxColour(0x474747);

        case wxSYS_COLOUR_SCROLLBAR:
            return wxColour(0x4d4d4d);

        case wxSYS_COLOUR_INACTIVECAPTION:
        case wxSYS_COLOUR_MENU:
            return wxColour(0x2b2b2b);

        case wxSYS_COLOUR_MENUBAR:
            return wxColour(0x626262);

        case wxSYS_COLOUR_MENUHILIGHT:
            return wxColour(0x353535);

        case wxSYS_COLOUR_BTNHIGHLIGHT:
        case wxSYS_COLOUR_HIGHLIGHT:
            return wxColour(0x777777);

        case wxSYS_COLOUR_INACTIVECAPTIONTEXT:
            return wxColour(0xaaaaaa);

        case wxSYS_COLOUR_3DDKSHADOW:
        case wxSYS_COLOUR_3DLIGHT:
        case wxSYS_COLOUR_ACTIVEBORDER:
        case wxSYS_COLOUR_DESKTOP:
        case wxSYS_COLOUR_GRADIENTACTIVECAPTION:
        case wxSYS_COLOUR_GRADIENTINACTIVECAPTION:
        case wxSYS_COLOUR_GRAYTEXT:
        case wxSYS_COLOUR_INACTIVEBORDER:
        case wxSYS_COLOUR_WINDOWFRAME:
            return wxColour();

        case wxSYS_COLOUR_MAX:
            break;
    }

    wxFAIL_MSG( "unreachable" );
    return wxColour();
}

wxColour wxDarkModeSettings::GetMenuColour(wxMenuColour which)
{
    switch ( which )
    {
        case wxMenuColour::StandardFg:
            return wxColour(0xffffff);

        case wxMenuColour::StandardBg:
            return wxColour(0x6d6d6d);

        case wxMenuColour::DisabledFg:
            return wxColour(0x414141);

        case wxMenuColour::HotBg:
            return wxColour(0x2b2b2b);
    }

    wxFAIL_MSG( "unreachable" );
    return wxColour();
}

wxPen wxDarkModeSettings::GetBorderPen()
{
    // Use a darker pen than the default white one by default. There doesn't
    // seem to be any standard colour to use for it, Windows itself uses both
    // 0x666666 and 0x797979 for the borders in the "Colours" control panel
    // window, so it doesn't seem like anybody cares about consistency here.
    return *wxGREY_PEN;
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

    wxMSWImpl::AllowDarkModeForWindow(hwnd, true);
}

void AllowForWindow(HWND hwnd, const wchar_t* themeName, const wchar_t* themeId)
{
    if ( !wxMSWImpl::ShouldUseDarkMode() )
        return;

    if ( wxMSWImpl::AllowDarkModeForWindow(hwnd, true) )
        wxLogTrace(TRACE_DARKMODE, "Allow dark mode for %p failed", hwnd);

    if ( themeName || themeId )
    {
        HRESULT hr = ::SetWindowTheme(hwnd, themeName, themeId);
        if ( FAILED(hr) )
        {
            wxLogApiError(wxString::Format("SetWindowTheme(%p, %s, %s)",
                                           hwnd, themeName, themeId), hr);
        }
    }
}

wxColour GetColour(wxSystemColour index)
{
    return wxDarkModeModule::GetSettings().GetColour(index);
}

wxPen GetBorderPen()
{
    return wxDarkModeModule::GetSettings().GetBorderPen();
}

HBRUSH GetBackgroundBrush()
{
    wxBrush* const brush =
        wxTheBrushList->FindOrCreateBrush(GetColour(wxSYS_COLOUR_WINDOW));

    return brush ? GetHbrushOf(*brush) : 0;
}

wxBitmap InvertBitmap(const wxBitmap& bmp)
{
#if wxUSE_IMAGE
    wxImage image = bmp.ConvertToImage();

    unsigned char *data = image.GetData();
    const int len = image.GetWidth()*image.GetHeight();
    for ( int i = 0; i < len; ++i, data += 3 )
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

    return wxBitmap(image);
#else // !wxUSE_IMAGE
    return wxBitmap();
#endif // wxUSE_IMAGE/!wxUSE_IMAGE
}

bool PaintIfNecessary(HWND hwnd, WXWNDPROC defWndProc)
{
#if wxUSE_IMAGE
    if ( !wxMSWImpl::ShouldUseDarkMode() )
        return false;

    const RECT rc = wxGetClientRect(hwnd);
    const wxSize size{rc.right - rc.left, rc.bottom - rc.top};

    // Don't bother doing anything with the empty windows.
    if ( size == wxSize() )
        return false;

    // Ask the control to paint itself on the given bitmap.
    wxBitmap bmp(size);
    {
        wxMemoryDC mdc(bmp);

        WPARAM wparam = (WPARAM)GetHdcOf(mdc);
        if ( defWndProc )
            ::CallWindowProc(defWndProc, hwnd, WM_PAINT, wparam, 0);
        else
            ::DefWindowProc(hwnd, WM_PAINT, wparam, 0);
    }

    PAINTSTRUCT ps;
    wxDCTemp dc(::BeginPaint(hwnd, &ps), size);
    dc.DrawBitmap(InvertBitmap(bmp), 0, 0);
    ::EndPaint(hwnd, &ps);

    return true;
#else // !wxUSE_IMAGE
    wxUnusedVar(hwnd);
    wxUnusedVar(defWndProc);

    return false;
#endif
}

// ----------------------------------------------------------------------------
// Menu bar custom drawing
// ----------------------------------------------------------------------------

namespace wxMSWMenuImpl
{

// Definitions for undocumented messages and structs used in this code.
constexpr int WM_MENUBAR_DRAWMENU = 0x91;
constexpr int WM_MENUBAR_DRAWMENUITEM = 0x92;

// This is passed via LPARAM of WM_MENUBAR_DRAWMENU.
struct MenuBarDrawMenu
{
    HMENU hmenu;
    HDC hdc;
    DWORD dwReserved;
};

struct MenuBarMenuItem
{
    int iPosition;

    // There are more fields in this (undocumented) struct but we don't
    // currently need them, so don't bother with declaring them.
};

struct MenuBarDrawMenuItem
{
    DRAWITEMSTRUCT dis;
    MenuBarDrawMenu mbdm;
    MenuBarMenuItem mbmi;
};

wxColour GetMenuColour(wxMenuColour which)
{
    return wxDarkModeModule::GetSettings().GetMenuColour(which);
}

HBRUSH GetMenuBrush(wxMenuColour which = wxMenuColour::StandardBg)
{
    wxBrush* const brush =
        wxTheBrushList->FindOrCreateBrush(GetMenuColour(which));

    return brush ? GetHbrushOf(*brush) : 0;
}

} // namespace wxMSWMenuImpl

bool
HandleMenuMessage(WXLRESULT* result,
                  wxWindow* w,
                  WXUINT nMsg,
                  WXWPARAM wParam,
                  WXLPARAM lParam)
{
    if ( !wxMSWImpl::ShouldUseDarkMode() )
        return false;

    using namespace wxMSWMenuImpl;

    switch ( nMsg )
    {
        case WM_MENUBAR_DRAWMENU:
            // Erase the menu bar background using custom brush.
            if ( auto* const drawMenu = (MenuBarDrawMenu*)lParam )
            {
                HWND hwnd = GetHwndOf(w);

                WinStruct<MENUBARINFO> mbi;
                if ( !::GetMenuBarInfo(hwnd, OBJID_MENU, 0, &mbi) )
                {
                    wxLogLastError("GetMenuBarInfo");
                    break;
                }

                const RECT rcWindow = wxGetWindowRect(hwnd);

                // rcBar is expressed in screen coordinates.
                ::OffsetRect(&mbi.rcBar, -rcWindow.left, -rcWindow.top);

                ::FillRect(drawMenu->hdc, &mbi.rcBar, GetMenuBrush());
            }

            *result = 0;
            return true;

        case WM_NCPAINT:
        case WM_NCACTIVATE:
            // Drawing the menu bar background in WM_MENUBAR_DRAWMENU somehow
            // leaves a single pixel line unpainted (and increasing the size of
            // the rectangle doesn't help, i.e. drawing is clipped to an area
            // which is one pixel too small), so we have to draw over it here
            // to get rid of it.
            {
                *result = w->MSWDefWindowProc(nMsg, wParam, lParam);

                HWND hwnd = GetHwndOf(w);
                WindowHDC hdc(hwnd);

                // Create a RECT one pixel above the client area: note that we
                // have to use window (and not client) coordinates for this as
                // this is outside of the client area of the window.
                const RECT rcWindow = wxGetWindowRect(hwnd);
                RECT rc = wxGetClientRect(hwnd);

                // Convert client coordinates to window ones.
                wxMapWindowPoints(hwnd, HWND_DESKTOP, &rc);
                ::OffsetRect(&rc, -rcWindow.left, -rcWindow.top);

                rc.bottom = rc.top;
                rc.top--;

                ::FillRect(hdc, &rc, GetMenuBrush());
            }
            return true;

        case WM_MENUBAR_DRAWMENUITEM:
            if ( auto* const drawMenuItem = (MenuBarDrawMenuItem*)lParam )
            {
                const DRAWITEMSTRUCT& dis = drawMenuItem->dis;

                // Just a sanity check.
                if ( dis.CtlType != ODT_MENU )
                    break;

                wchar_t buf[256];
                WinStruct<MENUITEMINFO> mii;
                mii.fMask = MIIM_STRING;
                mii.dwTypeData = buf;
                mii.cch = sizeof(buf) - 1;

                // Note that we need to use the iPosition field of the
                // undocumented struct here because DRAWITEMSTRUCT::itemID is
                // not initialized in the struct passed to us here, so this is
                // the only way to identify the item we're dealing with.
                if ( !::GetMenuItemInfo((HMENU)dis.hwndItem,
                                        drawMenuItem->mbmi.iPosition,
                                        TRUE,
                                        &mii) )
                    break;

                const UINT itemState = dis.itemState;

                HBRUSH hbr = 0;
                int partState = 0;
                wxMenuColour colText = wxMenuColour::StandardFg;
                if ( itemState & ODS_INACTIVE )
                {
                    partState = MBI_DISABLED;
                    colText = wxMenuColour::DisabledFg;
                }
                else if ( (itemState & ODS_GRAYED) && (itemState & ODS_HOTLIGHT) )
                {
                    partState = MBI_DISABLEDHOT;
                }
                else if ( itemState & ODS_GRAYED )
                {
                    partState = MBI_DISABLED;
                    colText = wxMenuColour::DisabledFg;
                }
                else if ( itemState & (ODS_HOTLIGHT | ODS_SELECTED) )
                {
                    partState = MBI_HOT;

                    hbr = GetMenuBrush(wxMenuColour::HotBg);
                }
                else
                {
                    partState = MBI_NORMAL;
                }

                RECT* const rcItem = const_cast<RECT*>(&dis.rcItem);

                // Don't use DrawThemeBackground() here, as it doesn't use the
                // correct colours in the dark mode, at least not when using
                // the "Menu" theme.
                ::FillRect(dis.hDC, &dis.rcItem, hbr ? hbr : GetMenuBrush());

                // We have to specify the text colour explicitly as by default
                // black would be used, making the menu label unreadable on the
                // (almost) black background.
                DTTOPTS textOpts;
                textOpts.dwSize = sizeof(textOpts);
                textOpts.dwFlags = DTT_TEXTCOLOR;
                textOpts.crText = wxColourToRGB(GetMenuColour(colText));

                DWORD drawTextFlags = DT_CENTER | DT_SINGLELINE | DT_VCENTER;
                if ( itemState & ODS_NOACCEL)
                    drawTextFlags |= DT_HIDEPREFIX;

                wxUxThemeHandle menuTheme(GetHwndOf(w), L"Menu");
                ::DrawThemeTextEx(menuTheme, dis.hDC, MENU_BARITEM, partState,
                                  buf, mii.cch, drawTextFlags, rcItem,
                                  &textOpts);
            }
            return true;
    }

    return false;
}

} // namespace wxMSWDarkMode

#else // !wxUSE_DARK_MODE

bool
wxApp::MSWEnableDarkMode(int WXUNUSED(flags),
                         wxDarkModeSettings* WXUNUSED(settings))
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

wxPen GetBorderPen()
{
    return wxPen{};
}

HBRUSH GetBackgroundBrush()
{
    return 0;
}

wxBitmap InvertBitmap(const wxBitmap& WXUNUSED(bmp))
{
    return wxBitmap();
}

bool PaintIfNecessary(HWND WXUNUSED(hwnd), WXWNDPROC WXUNUSED(defWndProc))
{
    return false;
}

bool
HandleMenuMessage(WXLRESULT* WXUNUSED(result),
                  wxWindow* WXUNUSED(w),
                  WXUINT WXUNUSED(nMsg),
                  WXWPARAM WXUNUSED(wParam),
                  WXLPARAM WXUNUSED(lParam))
{
    return false;
}

} // namespace wxMSWDarkMode

#endif // wxUSE_DARK_MODE/!wxUSE_DARK_MODE
