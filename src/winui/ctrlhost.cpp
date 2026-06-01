/////////////////////////////////////////////////////////////////////////////
// Name:        src/winui/ctrlhost.cpp
// Purpose:     private wxWinUI host implementation
// Author:      wxWidgets development team
// Created:     2026-05-31
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#include "wx/winui/winui.h"

#if wxUSE_WINUI3

#include "private.h"

#ifndef WX_PRECOMP
    #include "wx/log.h"
    #include "wx/string.h"
    #include "wx/window.h"
    #include "wx/bitmap.h"
    #include "wx/image.h"
#endif

#include "wx/app.h"
#include "wx/settings.h"
#include "wx/toplevel.h"

#include <dwmapi.h>
#include <winrt/Windows.Storage.Streams.h>

#include <algorithm>
#include <set>
#include <vector>

#ifndef DWMWA_USE_IMMERSIVE_DARK_MODE
    #define DWMWA_USE_IMMERSIVE_DARK_MODE 20
#endif
#ifndef DWMWA_SYSTEMBACKDROP_TYPE
    #define DWMWA_SYSTEMBACKDROP_TYPE 38
#endif
#ifndef DWMSBT_MAINWINDOW
    #define DWMSBT_MAINWINDOW 2  // Mica
#endif

void wxWinUILogException(const char *what, const winrt::hresult_error& e);

namespace
{

const wchar_t wxWinUIBackdropTransparentProp[] =
    L"wxWinUIBackdropTransparent";

// All live control hosts, so the theme can be switched on the fly.
std::vector<wxWinUIControlHost *> gs_winuiHosts;

// Monitors whose DWM backdrop surface has already been primed (see
// wxWinUIPrimeBackdrop): each monitor needs a one-time size nudge.
std::set<HMONITOR> gs_winuiPrimedMonitors;

wxWinUIAppTheme gs_winuiAppTheme = wxWinUIAppTheme::System;
winrt::Microsoft::UI::Xaml::ElementTheme gs_winuiElementTheme =
    winrt::Microsoft::UI::Xaml::ElementTheme::Default;

// Read the system "apps use light theme" preference directly from the registry,
// which works regardless of whether the classic MSW dark mode support has been
// enabled (the WinUI port bypasses it).
bool wxWinUISystemUsesDarkMode()
{
    DWORD value = 1;
    DWORD size = sizeof(value);
    if ( ::RegGetValueW(
             HKEY_CURRENT_USER,
             L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
             L"AppsUseLightTheme",
             RRF_RT_REG_DWORD,
             nullptr,
             &value,
             &size) == ERROR_SUCCESS )
    {
        return value == 0;
    }

    return false;
}

// Whether the effective theme (taking System into account) is dark.
bool wxWinUIEffectiveDark()
{
    switch ( gs_winuiAppTheme )
    {
        case wxWinUIAppTheme::Light:
            return false;
        case wxWinUIAppTheme::Dark:
            return true;
        case wxWinUIAppTheme::System:
            break;
    }

    return wxWinUISystemUsesDarkMode();
}

// WinUI-like solid background colours used only when the DWM Mica backdrop is
// not available.
wxColour wxWinUIBackgroundColour()
{
    return wxWinUIEffectiveDark() ? wxColour(32, 32, 32)
                                  : wxColour(243, 243, 243);
}

wxWinUIControlHost *wxWinUIFindHostForTabHWND(HWND hwnd)
{
    if ( !hwnd )
        return nullptr;

    for ( wxWinUIControlHost *host : gs_winuiHosts )
    {
        if ( host->GetHostHWND() == hwnd || host->GetBridgeHWND() == hwnd )
            return host;
    }

    return nullptr;
}

wxWinUIControlHost *wxWinUIFindHostContainingFocus(HWND hwnd)
{
    if ( !hwnd )
        return nullptr;

    for ( wxWinUIControlHost *host : gs_winuiHosts )
    {
        if ( host->ContainsFocus(hwnd) )
            return host;
    }

    return nullptr;
}

// Theme a window's native scrollbars (and other common controls) to match the
// light/dark app theme.  SetWindowTheme is loaded dynamically to avoid pulling
// in <uxtheme.h> here (which conflicts with the C++/WinRT headers).
void wxWinUIApplyScrollbarTheme(HWND hwnd, bool dark)
{
    typedef HRESULT (WINAPI *SetWindowTheme_t)(HWND, LPCWSTR, LPCWSTR);
    static SetWindowTheme_t s_setWindowTheme = []() -> SetWindowTheme_t
    {
        HMODULE module = ::GetModuleHandleW(L"uxtheme.dll");
        if ( !module )
            module = ::LoadLibraryW(L"uxtheme.dll");
        return module
            ? reinterpret_cast<SetWindowTheme_t>(
                  ::GetProcAddress(module, "SetWindowTheme"))
            : nullptr;
    }();

    if ( s_setWindowTheme )
        s_setWindowTheme(hwnd, dark ? L"DarkMode_Explorer" : L"Explorer", nullptr);
}

void wxWinUIApplyMicaBackground(wxWindow *win, bool micaEnabled)
{
    if ( !win )
        return;

    HWND hwnd = static_cast<HWND>(win->GetHWND());
    if ( hwnd )
    {
        // The prop drives the WM_ERASEBKGND handler in src/msw/window.cpp: when
        // present, the window is erased with black so the DWM Mica backdrop
        // shows through.  WS_EX_TRANSPARENT is deliberately NOT used -- it does
        // not make the HWND alpha-transparent to DWM and only reorders child
        // painting, which produced inconsistent (opaque) results on SDR
        // monitors.
        if ( micaEnabled )
            ::SetPropW(hwnd, wxWinUIBackdropTransparentProp, reinterpret_cast<HANDLE>(1));
        else
            ::RemovePropW(hwnd, wxWinUIBackdropTransparentProp);

        // Match native scrollbars (e.g. wxScrolledWindow) to the app theme.
        wxWinUIApplyScrollbarTheme(hwnd, wxWinUIEffectiveDark());
    }

    if ( !micaEnabled )
        win->SetBackgroundColour(wxWinUIBackgroundColour());

    for ( wxWindowList::const_iterator i = win->GetChildren().begin();
          i != win->GetChildren().end();
          ++i )
    {
        wxWinUIApplyMicaBackground(*i, micaEnabled);
    }
}

} // namespace

void wxWinUILogException(const char *what, const winrt::hresult_error& e)
{
    wxLogWarning("%s failed with HRESULT 0x%08lx: %s",
                 what,
                 static_cast<unsigned long>(e.code()),
                 wxString(e.message().c_str()));
}

winrt::hstring wxWinUIToHString(const wxString& str)
{
    return winrt::hstring(str.ToStdWstring());
}

wxString wxWinUIFromHString(const winrt::hstring& str)
{
    return wxString(str.c_str());
}

namespace
{

// COM interface giving direct access to an IBuffer's bytes.
struct __declspec(uuid("905a0fef-bc53-11df-8c49-001e4fc686da"))
IWxHostBufferByteAccess : ::IUnknown
{
    virtual HRESULT __stdcall Buffer(uint8_t** value) = 0;
};

} // namespace

winrt::Microsoft::UI::Xaml::Media::Imaging::WriteableBitmap
wxWinUIWriteableBitmapFromBitmap(const wxBitmap& bitmap)
{
    if ( !bitmap.IsOk() )
        return nullptr;

    wxImage image = bitmap.ConvertToImage();
    if ( !image.IsOk() )
        return nullptr;

    const int w = image.GetWidth();
    const int h = image.GetHeight();
    const unsigned char *rgb = image.GetData();
    const unsigned char *alpha = image.HasAlpha() ? image.GetAlpha() : nullptr;

    winrt::Microsoft::UI::Xaml::Media::Imaging::WriteableBitmap wb(w, h);

    uint8_t *dst = nullptr;
    auto access = wb.PixelBuffer().as<IWxHostBufferByteAccess>();
    if ( FAILED(access->Buffer(&dst)) || !dst )
        return nullptr;

    for ( int i = 0; i < w * h; ++i )
    {
        const unsigned char r = rgb[i * 3 + 0];
        const unsigned char g = rgb[i * 3 + 1];
        const unsigned char b = rgb[i * 3 + 2];
        const unsigned char a = alpha ? alpha[i] : 255;

        // WriteableBitmap expects premultiplied BGRA.
        dst[i * 4 + 0] = static_cast<uint8_t>(b * a / 255);
        dst[i * 4 + 1] = static_cast<uint8_t>(g * a / 255);
        dst[i * 4 + 2] = static_cast<uint8_t>(r * a / 255);
        dst[i * 4 + 3] = a;
    }

    wb.Invalidate();
    return wb;
}

winrt::Microsoft::UI::Xaml::ElementTheme wxWinUIGetCurrentElementTheme()
{
    return gs_winuiElementTheme;
}

wxWinUIControlHost::~wxWinUIControlHost()
{
    Close();
}

bool wxWinUIControlHost::Initialize(wxWindow *window)
{
    if ( m_source )
        return true;

    if ( !window || !wxWinUI3Initialize() )
        return false;

    try
    {
        using namespace winrt::Microsoft::UI::Content;
        using namespace winrt::Microsoft::UI::Xaml::Hosting;

        m_source = DesktopWindowXamlSource();

        // Parent the XAML island bridge directly to the control's own window
        // and let it fill the client area.  Because the bridge is a child of
        // the control, it is moved together with it by the OS, which avoids the
        // flicker that results from repositioning a top-level-parented island
        // manually on every resize.
        HWND hwnd = static_cast<HWND>(window->GetHWND());
        ::SetWindowLongPtr
        (
            hwnd,
            GWL_STYLE,
            ::GetWindowLongPtr(hwnd, GWL_STYLE) |
                WS_CLIPCHILDREN | WS_CLIPSIBLINGS
        );

        m_hostHwnd = hwnd;
        const auto windowId = winrt::Microsoft::UI::GetWindowIdFromWindow(m_hostHwnd);

        m_source.Initialize(windowId);
        m_source.SiteBridge().ResizePolicy(
            ContentSizePolicy::ResizeContentToParentWindow);
        m_takeFocusRequestedToken = m_source.TakeFocusRequested(
            [this](
                winrt::Microsoft::UI::Xaml::Hosting::DesktopWindowXamlSource const&,
                winrt::Microsoft::UI::Xaml::Hosting::DesktopWindowXamlSourceTakeFocusRequestedEventArgs const& event)
            {
                OnTakeFocusRequested(event);
            });

        m_bridgeHwnd = winrt::Microsoft::UI::GetWindowFromWindowId(
            m_source.SiteBridge().WindowId());
        ::SetWindowLongPtr
        (
            m_bridgeHwnd,
            GWL_STYLE,
            ::GetWindowLongPtr(m_bridgeHwnd, GWL_STYLE) |
                WS_CHILD | WS_VISIBLE | WS_TABSTOP
        );

        m_window = window;
        m_window->Bind(wxEVT_SIZE, &wxWinUIControlHost::OnWindowSize, this);
        m_window->Bind(wxEVT_SET_FOCUS, &wxWinUIControlHost::OnSetFocus, this);
        gs_winuiHosts.push_back(this);
        MoveAndResize();
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("DesktopWindowXamlSource initialization", e);
        m_window = nullptr;
        m_hostHwnd = nullptr;
        m_source = nullptr;
        return false;
    }

    return true;
}

void wxWinUIControlHost::Close()
{
    gs_winuiHosts.erase(
        std::remove(gs_winuiHosts.begin(), gs_winuiHosts.end(), this),
        gs_winuiHosts.end());

    if ( m_window )
    {
        m_window->Unbind(wxEVT_SIZE, &wxWinUIControlHost::OnWindowSize, this);
        m_window->Unbind(wxEVT_SET_FOCUS, &wxWinUIControlHost::OnSetFocus, this);
        m_window = nullptr;
    }
    m_hostHwnd = nullptr;
    m_bridgeHwnd = nullptr;
    m_content = nullptr;

    if ( !m_source )
        return;

    try
    {
        if ( m_takeFocusRequestedToken.value )
        {
            m_source.TakeFocusRequested(m_takeFocusRequestedToken);
            m_takeFocusRequestedToken = {};
        }

        m_source.Content(nullptr);
        m_source.Close();
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("DesktopWindowXamlSource::Close", e);
    }

    m_source = nullptr;
}

void wxWinUIControlHost::SetContent(const winrt::Microsoft::UI::Xaml::UIElement& element)
{
    if ( !m_source )
        return;

    m_content = element;
    m_source.Content(element);
    ApplyTheme(gs_winuiElementTheme);
    MoveAndResize();

    // Apply the island's own Mica backdrop, but only once the message loop is
    // running: setting it synchronously during control creation deadlocks
    // because the backdrop controller waits on the dispatcher queue, which is
    // not being pumped yet.
    if ( m_window && !m_backdropApplied )
    {
        m_backdropApplied = true;
        m_window->CallAfter([this]() { ApplyBackdropMaterial(); });
    }
}

void wxWinUIControlHost::ApplyBackdropMaterial()
{
    if ( !m_source )
        return;

    try
    {
        m_source.SystemBackdrop(
            winrt::Microsoft::UI::Xaml::Media::MicaBackdrop{});
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("DesktopWindowXamlSource::SystemBackdrop", e);
    }
}

void wxWinUIControlHost::ClearContent()
{
    if ( !m_source )
        return;

    m_content = nullptr;
    m_source.Content(nullptr);
}

void wxWinUIControlHost::ApplyTheme(winrt::Microsoft::UI::Xaml::ElementTheme theme)
{
    if ( !m_content )
        return;

    if ( auto element =
             m_content.try_as<winrt::Microsoft::UI::Xaml::FrameworkElement>() )
    {
        try
        {
            element.RequestedTheme(theme);
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException("FrameworkElement::RequestedTheme", e);
        }
    }

    ForceRender();
}

void wxWinUIControlHost::SetBridgeHeightLimit(int physicalHeight)
{
    if ( m_bridgeHeightLimit == physicalHeight )
        return;

    m_bridgeHeightLimit = physicalHeight;
    MoveAndResize();
}

void wxWinUIControlHost::UpdateContentSize(int width, int height)
{
    if ( !m_content )
        return;

    if ( auto element =
             m_content.try_as<winrt::Microsoft::UI::Xaml::FrameworkElement>() )
    {
        element.Width(width);
        element.Height(height);
        element.UpdateLayout();
    }
}

void wxWinUIControlHost::ForceRender()
{
    if ( !m_source || !m_window )
        return;

    RECT rect;
    if ( !::GetClientRect(m_hostHwnd, &rect) )
        return;

    const int width = rect.right - rect.left;
    int height = rect.bottom - rect.top;
    if ( width <= 0 || height <= 0 )
        return;

    if ( m_bridgeHeightLimit > 0 && m_bridgeHeightLimit < height )
        height = m_bridgeHeightLimit;

    try
    {
        UpdateContentSize(width, height);
        m_source.SiteBridge().MoveAndResize({ 0, 0, width, height });
        m_source.SiteBridge().Show();
        if ( m_bridgeHwnd )
        {
            ::InvalidateRect(m_bridgeHwnd, nullptr, FALSE);
            ::UpdateWindow(m_bridgeHwnd);
        }
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("wxWinUIControlHost::ForceRender", e);
    }
}

bool wxWinUIControlHost::ContainsFocus(HWND hwnd) const
{
    if ( !hwnd )
        return false;

    return hwnd == m_hostHwnd ||
           hwnd == m_bridgeHwnd ||
           (m_hostHwnd && ::IsChild(m_hostHwnd, hwnd)) ||
           (m_bridgeHwnd && ::IsChild(m_bridgeHwnd, hwnd));
}

bool wxWinUIControlHost::NavigateFocus(bool forward)
{
    if ( !m_source )
        return false;

    try
    {
        using namespace winrt::Microsoft::UI::Xaml::Hosting;

        XamlSourceFocusNavigationRequest request(
            forward ? XamlSourceFocusNavigationReason::First
                    : XamlSourceFocusNavigationReason::Last);
        const XamlSourceFocusNavigationResult result =
            m_source.NavigateFocus(request);
        return result.WasFocusMoved();
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("DesktopWindowXamlSource::NavigateFocus", e);
        return false;
    }
}

void wxWinUIControlHost::MoveAndResize()
{
    if ( !m_source || !m_window )
        return;

    RECT rect;
    if ( !::GetClientRect(m_hostHwnd, &rect) )
        return;

    const int width = rect.right - rect.left;
    int height = rect.bottom - rect.top;
    if ( width <= 0 || height <= 0 )
        return;

    if ( m_bridgeHeightLimit > 0 && m_bridgeHeightLimit < height )
        height = m_bridgeHeightLimit;

    try
    {
        UpdateContentSize(width, height);

        // The bridge is a child of the control window, so it always fills the
        // client area at the origin and follows the control automatically.
        m_source.SiteBridge().MoveAndResize({ 0, 0, width, height });
        m_source.SiteBridge().Show();
        if ( m_bridgeHwnd )
        {
            // Keep the island at the bottom of the sibling z-order.  This is
            // irrelevant for normal (non-overlapping) controls but essential
            // for container controls such as wxNotebook, whose page windows are
            // siblings of this bridge and must render on top of it.
            ::SetWindowPos
            (
                m_bridgeHwnd,
                HWND_BOTTOM,
                0,
                0,
                width,
                height,
                SWP_NOACTIVATE | SWP_SHOWWINDOW
            );
        }
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("DesktopWindowXamlSource::MoveAndResize", e);
    }
}

void wxWinUIControlHost::OnWindowSize(wxSizeEvent& event)
{
    MoveAndResize();
    event.Skip();
}

void wxWinUIControlHost::OnSetFocus(wxFocusEvent& event)
{
    // When wx gives the keyboard focus to the host control, hand it to the
    // island so that character input (WM_CHAR) is delivered to the XAML control
    // instead of being consumed by wx's dialog navigation.
    if ( m_bridgeHwnd )
        ::SetFocus(m_bridgeHwnd);

    if ( auto control =
             m_content.try_as<winrt::Microsoft::UI::Xaml::Controls::Control>() )
    {
        try
        {
            control.Focus(winrt::Microsoft::UI::Xaml::FocusState::Programmatic);
        }
        catch ( const winrt::hresult_error& )
        {
        }
    }

    event.Skip();
}

void wxWinUIControlHost::OnTakeFocusRequested(
    winrt::Microsoft::UI::Xaml::Hosting::DesktopWindowXamlSourceTakeFocusRequestedEventArgs const& event)
{
    if ( !m_hostHwnd )
        return;

    using namespace winrt::Microsoft::UI::Xaml::Hosting;

    const bool previous =
        event.Request().Reason() == XamlSourceFocusNavigationReason::Last;
    HWND parent = ::GetParent(m_hostHwnd);
    if ( !parent )
        parent = ::GetAncestor(m_hostHwnd, GA_ROOT);
    if ( !parent )
        return;

    HWND next = ::GetNextDlgTabItem(parent, m_hostHwnd, previous);
    if ( !next || next == m_hostHwnd )
        return;

    if ( wxWinUIControlHost *host = wxWinUIFindHostForTabHWND(next) )
    {
        host->NavigateFocus(!previous);
        return;
    }

    ::SetFocus(next);
}

bool wxWinUI3ProcessTabNavigation(WXMSG *msg)
{
    if ( !msg || msg->message != WM_KEYDOWN || msg->wParam != VK_TAB )
        return false;

    HWND focus = ::GetFocus();
    if ( !focus )
        focus = msg->hwnd;
    if ( !focus )
        return false;

    wxWinUIControlHost *currentHost = wxWinUIFindHostContainingFocus(focus);
    HWND current = currentHost ? currentHost->GetHostHWND() : focus;
    if ( !current )
        return false;

    HWND parent = ::GetParent(current);
    if ( !parent )
        parent = ::GetAncestor(current, GA_ROOT);
    if ( !parent )
        return false;

    const bool previous = (::GetKeyState(VK_SHIFT) & 0x8000) != 0;
    HWND next = ::GetNextDlgTabItem(parent, current, previous);
    if ( (!next || next == current) && parent != ::GetAncestor(current, GA_ROOT) )
        next = ::GetNextDlgTabItem(::GetAncestor(current, GA_ROOT), current, previous);
    if ( !next || next == current )
        return false;

    if ( wxWinUIControlHost *host = wxWinUIFindHostForTabHWND(next) )
        return host->NavigateFocus(!previous);

    if ( currentHost )
    {
        ::SetFocus(next);
        return true;
    }

    return false;
}

bool wxWinUI3DispatchIslandKeyboard(WXMSG *msg)
{
    if ( !msg )
        return false;

    switch ( msg->message )
    {
        case WM_KEYDOWN:
        case WM_KEYUP:
        case WM_CHAR:
        case WM_DEADCHAR:
        case WM_SYSKEYDOWN:
        case WM_SYSKEYUP:
        case WM_SYSCHAR:
        case WM_SYSDEADCHAR:
        case WM_UNICHAR:
            break;
        default:
            return false;
    }

    // Only take over keyboard input while a WinUI island actually has focus.
    if ( !wxWinUIFindHostContainingFocus(::GetFocus()) )
        return false;

    // ContentPreTranslateMessage() (called earlier from
    // wxWinUI3PreTranslateMessage) has already had its chance to consume the
    // message.  Since it did not, dispatch it straight to the focused island,
    // exactly like the canonical XAML-island message loop does.  Crucially we
    // skip wxWidgets' own PreProcessMessage(): its dialog navigation treats
    // character keys as mnemonic candidates, swallows them and plays the error
    // sound instead of letting the TextBox insert the text.
    ::TranslateMessage(reinterpret_cast<MSG *>(msg));
    ::DispatchMessage(reinterpret_cast<MSG *>(msg));
    return true;
}

void wxWinUIApplyWindowBackdrop(wxWindow *tlw)
{
    if ( !tlw )
        return;

    HWND hwnd = static_cast<HWND>(tlw->GetHWND());
    if ( !hwnd )
        return;

    // Match the title bar (and other non-client areas) to the active theme.
    BOOL dark = wxWinUIEffectiveDark();
    ::DwmSetWindowAttribute(hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE,
                            &dark, sizeof(dark));

    // Let DWM provide the same Mica backdrop used by normal Win32 apps.  The
    // client background is made transparent below so this is visible behind
    // wx panels without requiring a WinUI MicaController/DispatcherQueue.
    int backdrop = DWMSBT_MAINWINDOW;
    const bool micaEnabled =
        SUCCEEDED(::DwmSetWindowAttribute(hwnd, DWMWA_SYSTEMBACKDROP_TYPE,
                                          &backdrop, sizeof(backdrop)));
    if ( micaEnabled )
    {
        MARGINS margins = { -1, -1, -1, -1 };
        ::DwmExtendFrameIntoClientArea(hwnd, &margins);
    }

    wxWinUIApplyMicaBackground(tlw, micaEnabled);
    tlw->Refresh();

    if ( micaEnabled )
        wxWinUIPrimeBackdrop(hwnd);
}

void wxWinUIPrimeBackdrop(WXHWND hwndArg)
{
    HWND hwnd = static_cast<HWND>(hwndArg);
    if ( !hwnd )
        return;

    // The DWM backdrop surface only becomes "live" once the window is actually
    // resized on a given monitor; before that it can show as an opaque
    // rectangle (most visibly on SDR monitors).  Nudge the window size by one
    // pixel and back the first time it appears on each monitor to force the
    // backdrop to initialise, without requiring the user to resize manually.
    HMONITOR monitor = ::MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
    if ( !monitor || gs_winuiPrimedMonitors.count(monitor) )
        return;

    RECT rect;
    if ( !::GetWindowRect(hwnd, &rect) )
        return;

    const int width = rect.right - rect.left;
    const int height = rect.bottom - rect.top;
    if ( width <= 1 || height <= 1 )
        return;  // not yet laid out; prime on a later call

    gs_winuiPrimedMonitors.insert(monitor);

    const UINT flags = SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE |
                       SWP_NOOWNERZORDER;

    // Shrink the window by one pixel now, then restore it on the NEXT event
    // loop iteration (not immediately).  A manual resize works because the
    // message loop runs between size steps, letting DWM commit each one; two
    // back-to-back SetWindowPos calls are coalesced into "no change" and do not
    // initialise the backdrop.  Deferring the restore reproduces the manual
    // resize and primes the window backdrop as well as every child island.
    ::SetWindowPos(hwnd, nullptr, 0, 0, width, height - 1, flags);
    wxTheApp->CallAfter(
        [hwnd, width, height, flags]()
        {
            ::SetWindowPos(hwnd, nullptr, 0, 0, width, height, flags);
        });
}

void wxWinUISetAppTheme(wxWinUIAppTheme theme)
{
    using winrt::Microsoft::UI::Xaml::ElementTheme;

    gs_winuiAppTheme = theme;
    switch ( theme )
    {
        case wxWinUIAppTheme::Light:
            gs_winuiElementTheme = ElementTheme::Light;
            break;
        case wxWinUIAppTheme::Dark:
            gs_winuiElementTheme = ElementTheme::Dark;
            break;
        case wxWinUIAppTheme::System:
            gs_winuiElementTheme = ElementTheme::Default;
            break;
    }

    for ( wxWinUIControlHost *host : gs_winuiHosts )
        host->ApplyTheme(gs_winuiElementTheme);

    // Refresh the backdrop/title-bar of all top-level windows.
    for ( wxWindowList::const_iterator i = wxTopLevelWindows.begin();
          i != wxTopLevelWindows.end();
          ++i )
    {
        wxWinUIApplyWindowBackdrop(*i);
    }
}

wxWinUIAppTheme wxWinUIGetAppTheme()
{
    return gs_winuiAppTheme;
}

#endif // wxUSE_WINUI3
