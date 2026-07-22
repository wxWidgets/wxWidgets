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

#include "wx/winui/private/tlwhost.h"

#ifndef WX_PRECOMP
    #include "wx/log.h"
    #include "wx/string.h"
    #include "wx/window.h"
    #include "wx/bitmap.h"
    #include "wx/image.h"
    #include "wx/cursor.h"
    #include "wx/font.h"
#endif

#include "wx/app.h"
#include "wx/evtloop.h"
#include "wx/settings.h"
#include "wx/panel.h"
#include "wx/sizer.h"
#include "wx/splitter.h"
#include "wx/toplevel.h"
#include "wx/utils.h"
#include "wx/weakref.h"
#include "wx/msw/private.h"

#include <dwmapi.h>
#include <winrt/Windows.Storage.Streams.h>
#include <winrt/Microsoft.UI.Input.h>

#include <algorithm>
#include <cmath>
#include <functional>
#include <limits>
#include <cstdarg>
#include <cstring>
#include <map>
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
#ifndef DWMSBT_TRANSIENTWINDOW
    #define DWMSBT_TRANSIENTWINDOW 3  // Mica Alt, for popups/dialogs
#endif

void wxWinUILogException(const char *what, const winrt::hresult_error& e);

namespace
{

const wchar_t wxWinUIBackdropTransparentProp[] =
    L"wxWinUIBackdropTransparent";

// Map a wxCursor to the equivalent WinUI input cursor (null == default arrow).
winrt::Microsoft::UI::Input::InputCursor
wxWinUIInputCursorFromWxCursor(const wxCursor& cursor)
{
    namespace MUI = winrt::Microsoft::UI::Input;
    if ( !cursor.IsOk() )
        return nullptr;

    const HCURSOR h = reinterpret_cast<HCURSOR>(cursor.GetHCURSOR());
    if ( !h )
        return nullptr;

    static const struct { const wchar_t *id; MUI::InputSystemCursorShape shape; }
    s_map[] =
    {
        { IDC_WAIT,       MUI::InputSystemCursorShape::Wait },
        { IDC_APPSTARTING,MUI::InputSystemCursorShape::AppStarting },
        { IDC_ARROW,      MUI::InputSystemCursorShape::Arrow },
        { IDC_HAND,       MUI::InputSystemCursorShape::Hand },
        { IDC_IBEAM,      MUI::InputSystemCursorShape::IBeam },
        { IDC_CROSS,      MUI::InputSystemCursorShape::Cross },
        { IDC_SIZEALL,    MUI::InputSystemCursorShape::SizeAll },
        { IDC_SIZENWSE,   MUI::InputSystemCursorShape::SizeNorthwestSoutheast },
        { IDC_SIZENESW,   MUI::InputSystemCursorShape::SizeNortheastSouthwest },
        { IDC_SIZEWE,     MUI::InputSystemCursorShape::SizeWestEast },
        { IDC_SIZENS,     MUI::InputSystemCursorShape::SizeNorthSouth },
        { IDC_NO,         MUI::InputSystemCursorShape::UniversalNo },
        { IDC_HELP,       MUI::InputSystemCursorShape::Help },
        { IDC_UPARROW,    MUI::InputSystemCursorShape::UpArrow },
    };

    for ( const auto& e : s_map )
    {
        if ( h == ::LoadCursorW(nullptr, e.id) )
            return MUI::InputSystemCursor::Create(e.shape);
    }

    // Unknown (e.g. a custom cursor): fall back to the default arrow.
    return nullptr;
}

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

    // Every window is marked, as it originally was: no class has to be singled
    // out here.  The transparency comes from the WM_ERASEBKGND black fill in
    // src/msw/window.cpp, which does not stop a window from painting its own
    // content on top of it -- WM_PAINT is no longer discarded for these
    // windows, which is what used to blank every wx-drawn control.
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

void wxWinUIDebugLog(const char *format, ...)
{
#if !wxUSE_WINUI3_DEBUG_LOG
    wxUnusedVar(format);
#else
    va_list argptr;
    va_start(argptr, format);
    const wxString message = wxString::FormatV(wxString::FromAscii(format),
                                               argptr);
    va_end(argptr);

    const wxString line = wxString::Format(
        "[%llu pid=%lu tid=%lu] %s\r\n",
        static_cast<unsigned long long>(::GetTickCount64()),
        static_cast<unsigned long>(::GetCurrentProcessId()),
        static_cast<unsigned long>(::GetCurrentThreadId()),
        message.c_str());

    ::OutputDebugString(line.t_str());

    wchar_t tempPath[MAX_PATH];
    const DWORD len = ::GetTempPathW(WXSIZEOF(tempPath), tempPath);
    if ( !len || len >= WXSIZEOF(tempPath) )
        return;

    wxString filename(tempPath);
    filename += wxS("wxwinui-tooltip.log");

    HANDLE file = ::CreateFile(filename.t_str(),
                               FILE_APPEND_DATA,
                               FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                               nullptr,
                               OPEN_ALWAYS,
                               FILE_ATTRIBUTE_NORMAL,
                               nullptr);
    if ( file == INVALID_HANDLE_VALUE )
        return;

    const wxCharBuffer utf8 = line.utf8_str();
    const char *data = utf8.data();
    if ( data )
    {
        DWORD written = 0;
        ::WriteFile(file,
                    data,
                    static_cast<DWORD>(std::strlen(data)),
                    &written,
                    nullptr);
        ::FlushFileBuffers(file);
    }

    ::CloseHandle(file);
#endif // wxUSE_WINUI3_DEBUG_LOG
}

winrt::hstring wxWinUIToHString(const wxString& str)
{
    return winrt::hstring(str.ToStdWstring());
}

wxString wxWinUIFromHString(const winrt::hstring& str)
{
    return wxString(str.c_str());
}

wxString wxWinUIRemoveMnemonics(const wxString& label)
{
    wxString stripped;
    stripped.reserve(label.length());

    for ( size_t i = 0; i < label.length(); ++i )
    {
        if ( label[i] == '&' )
        {
            if ( i + 1 < label.length() && label[i + 1] == '&' )
            {
                stripped += '&';
                ++i;
            }
        }
        else
        {
            stripped += label[i];
        }
    }

    return stripped;
}

void wxWinUISetDialogText(winrt::Microsoft::UI::Xaml::Controls::TextBlock const& text,
                          const wxString& value)
{
    using namespace winrt::Microsoft::UI::Xaml;

    text.Text(wxWinUIToHString(value));
    text.TextWrapping(TextWrapping::Wrap);
    text.IsTextSelectionEnabled(true);
}

bool wxWinUIIsHostWindow(wxWindow *win)
{
    if ( !win )
        return false;

    wxWinUITopLevelHost * const host =
        wxWinUITopLevelHost::ForWindow(win, false);
    return host && host->FindSlot(win) != nullptr;
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

    // Many icons (e.g. from wxArtProvider) carry a colour mask rather than a
    // real alpha channel; without this the masked (transparent) areas would
    // render as opaque black.  Convert the mask to alpha so they composite
    // transparently.
    if ( !image.HasAlpha() && image.HasMask() )
        image.InitAlpha();

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

void wxWinUISetToolTip(const winrt::Microsoft::UI::Xaml::UIElement& element,
                       const wxString& tip)
{
    if ( !element )
        return;

    try
    {
        namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;
        if ( tip.empty() )
            MUXC::ToolTipService::SetToolTip(element, nullptr);
        else
            MUXC::ToolTipService::SetToolTip(
                element, winrt::box_value(wxWinUIToHString(tip)));
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI tooltip", e);
    }
}

winrt::Microsoft::UI::Xaml::ElementTheme wxWinUIGetCurrentElementTheme()
{
    return gs_winuiElementTheme;
}

bool wxWinUIIsDarkTheme()
{
    return wxWinUIEffectiveDark();
}

wxWinUIControlHost::~wxWinUIControlHost()
{
    Close();
}

bool wxWinUIControlHost::Initialize(wxWindow *window)
{
    if ( m_window )
        return true;

    if ( !window || !wxWinUI3Initialize() )
        return false;

    // Find or create the shared island of this window's top-level parent;
    // the slot itself is only registered by SetContent().
    if ( !wxWinUITopLevelHost::ForWindow(window, true) )
        return false;

    m_window = window;
    return true;
}

bool wxWinUIControlHost::IsOk() const
{
    return m_window &&
           wxWinUITopLevelHost::ForWindow(m_window, false) != nullptr;
}

HWND wxWinUIControlHost::GetHostHWND() const
{
    return m_window ? static_cast<HWND>(m_window->GetHWND()) : nullptr;
}

HWND wxWinUIControlHost::GetBridgeHWND() const
{
    wxWinUITopLevelHost * const host =
        m_window ? wxWinUITopLevelHost::ForWindow(m_window, false) : nullptr;
    return host ? host->GetBridgeHwnd() : nullptr;
}

void wxWinUIControlHost::Close()
{
    if ( !m_window )
        return;

    // Revoke the Loaded hook first: the lambda captures "this".
    if ( m_loadedToken.value && m_content )
    {
        try
        {
            if ( auto framework = m_content
                     .try_as<winrt::Microsoft::UI::Xaml::FrameworkElement>() )
            {
                framework.Loaded(m_loadedToken);
            }
        }
        catch ( const winrt::hresult_error& )
        {
        }
    }
    m_loadedToken = {};

    // The TLW host may already be gone (it dies with its top-level window,
    // taking every slot with it), hence the tolerant lookup.
    wxWinUITopLevelHost * const host =
        wxWinUITopLevelHost::ForWindow(m_window, false);
    if ( host )
        host->UnregisterSlot(m_window);

    m_content = nullptr;
    m_window = nullptr;
}

void wxWinUIControlHost::SetContent(const winrt::Microsoft::UI::Xaml::UIElement& element)
{
    if ( !m_window )
        return;

    wxWinUITopLevelHost * const host =
        wxWinUITopLevelHost::ForWindow(m_window, true);
    if ( !host )
        return;

    // Re-setting the content drops the previous Loaded hook.
    if ( m_loadedToken.value && m_content )
    {
        try
        {
            if ( auto framework = m_content
                     .try_as<winrt::Microsoft::UI::Xaml::FrameworkElement>() )
            {
                framework.Loaded(m_loadedToken);
            }
        }
        catch ( const winrt::hresult_error& )
        {
        }
        m_loadedToken = {};
    }

    m_content = element;
    m_contentLoaded = false;

    // The natural size of a XAML element is only known once its control
    // template has been applied, which happens when it is loaded into a live
    // visual tree -- i.e. some time after this call.  Until then Measure()
    // under-reports (typically it returns the bare glyph of a check box,
    // without its label), so the control would be laid out far too small and
    // its content clipped away.  Re-query the best size when the content is
    // actually loaded and let the layout catch up.
    if ( auto framework =
             element.try_as<winrt::Microsoft::UI::Xaml::FrameworkElement>() )
    {
        m_loadedToken = framework.Loaded(
            [this](winrt::Windows::Foundation::IInspectable const&,
                   winrt::Microsoft::UI::Xaml::RoutedEventArgs const&)
            {
                OnContentLoaded();
            });
    }

    host->RegisterSlot(m_window, element);
}

void wxWinUIControlHost::ApplyBackdropMaterial()
{
    // Nothing to do: the per-island Mica backdrop is gone, the DWM window
    // backdrop shows through the shared island's transparent pixels.
}

void wxWinUIControlHost::ClearContent()
{
    if ( !m_window )
        return;

    wxWinUITopLevelHost * const host =
        wxWinUITopLevelHost::ForWindow(m_window, false);
    if ( host )
    {
        if ( wxWinUISlot * const slot = host->FindSlot(m_window) )
            slot->SetContent(nullptr);
    }

    m_content = nullptr;
}

void wxWinUIControlHost::ApplyWxCursor(const wxCursor& cursor)
{
    if ( !m_window )
        return;

    wxWinUITopLevelHost * const host =
        wxWinUITopLevelHost::ForWindow(m_window, false);
    if ( host )
        host->SetSlotCursor(m_window, wxWinUIInputCursorFromWxCursor(cursor));
}

void wxWinUISetWindowCursor(wxWindow *win, const wxCursor& cursor)
{
    if ( !win )
        return;

    wxWinUITopLevelHost * const host =
        wxWinUITopLevelHost::ForWindow(win, false);
    if ( host && host->FindSlot(win) )
        host->SetSlotCursor(win, wxWinUIInputCursorFromWxCursor(cursor));
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
}

void wxWinUIControlHost::SetBridgeHeightLimit(int physicalHeight)
{
    if ( !m_window )
        return;

    wxWinUITopLevelHost * const host =
        wxWinUITopLevelHost::ForWindow(m_window, false);
    if ( host )
        host->SetSlotClipHeight(m_window, physicalHeight);
}

wxSize wxWinUIControlHost::MeasureContent() const
{
    if ( !m_content || !m_window )
        return wxDefaultSize;

    auto element =
        m_content.try_as<winrt::Microsoft::UI::Xaml::FrameworkElement>();
    if ( !element )
        return wxDefaultSize;

    try
    {
        // The element itself is no longer size-pinned (its slot container
        // is), so an unconstrained Measure() reports the natural size
        // directly.
        const float inf = std::numeric_limits<float>::infinity();
        element.Measure({ inf, inf });
        const auto desired = element.DesiredSize();

        if ( desired.Width > 0 && desired.Height > 0 )
        {
            // DesiredSize is expressed in DIPs.
            return m_window->FromDIP(
                wxSize(static_cast<int>(std::ceil(desired.Width)),
                       static_cast<int>(std::ceil(desired.Height))));
        }
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("wxWinUIControlHost::MeasureContent", e);
    }

    return wxDefaultSize;
}

namespace
{

// Pending relayout pass, coalesced across every control whose content loads
// in the same burst (at startup they all load together: one pass, not one
// per control).
bool gs_winuiRelayoutPending = false;
std::vector<wxWeakRef<wxWindow>> gs_winuiRelayoutTops;

void wxWinUIScheduleContentRelayout(wxWindow *top)
{
    for ( const auto& weak : gs_winuiRelayoutTops )
    {
        if ( weak.get() == top )
        {
            top = nullptr;
            break;
        }
    }
    if ( top )
        gs_winuiRelayoutTops.push_back(wxWeakRef<wxWindow>(top));

    if ( gs_winuiRelayoutPending )
        return;
    gs_winuiRelayoutPending = true;

    wxTheApp->CallAfter([]()
    {
        gs_winuiRelayoutPending = false;

        std::vector<wxWeakRef<wxWindow>> tops;
        tops.swap(gs_winuiRelayoutTops);

        for ( const auto& weak : tops )
        {
            wxWindow * const top = weak.get();
            if ( !top )
                continue;

            if ( top->GetSizer() )
                top->Layout();

            // A single top-level Layout() is NOT enough: when it hands an
            // intermediate panel the very size it already had, that panel's
            // own sizer never reruns and keeps the geometry computed with
            // the pre-loading best sizes (cropped labels in static boxes...)
            // until something resizes the window.  Relayout DEEP, parents
            // first, and let every scrolling window recompute its virtual
            // size too -- otherwise the bottom of a page that grew during
            // loading stays out of reach.
            std::function<void (wxWindow *)> walk = [&](wxWindow *win)
            {
                for ( wxWindow *child : win->GetChildren() )
                {
                    if ( child->GetSizer() )
                        child->Layout();

                    const HWND hwnd = static_cast<HWND>(child->GetHWND());
                    if ( hwnd )
                    {
                        const LONG_PTR style =
                            ::GetWindowLongPtr(hwnd, GWL_STYLE);
                        if ( style & (WS_VSCROLL | WS_HSCROLL) )
                            child->SendSizeEvent();
                    }
                    walk(child);
                }
            };
            walk(top);
        }
    });
}

} // anonymous namespace

void wxWinUIControlHost::OnContentLoaded()
{
    if ( m_contentLoaded || !m_window )
        return;

    m_contentLoaded = true;

    // Clearing the cached best size is safe from inside the XAML callback;
    // the actual relayout is deferred (and coalesced globally).
    m_window->InvalidateBestSize();

    wxWindow * const top = wxGetTopLevelParent(m_window);
    if ( top )
        wxWinUIScheduleContentRelayout(top);
    else if ( wxWindow * const parent = m_window->GetParent() )
        wxWinUIScheduleContentRelayout(parent);
}

void wxWinUIControlHost::ForceRender()
{
    // Property changes invalidate the shared XAML tree by themselves; a slot
    // layout refresh is kept as belt and braces for the historical call
    // sites.
    if ( !m_window )
        return;

    wxWinUITopLevelHost * const host =
        wxWinUITopLevelHost::ForWindow(m_window, false);
    wxWinUISlot * const slot = host ? host->FindSlot(m_window) : nullptr;
    if ( !slot )
        return;

    try
    {
        const auto container = slot->GetContainer();
        container.InvalidateMeasure();
        container.InvalidateArrange();
        container.UpdateLayout();
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("wxWinUIControlHost::ForceRender", e);
    }
}

bool wxWinUIControlHost::ContainsFocus(HWND hwnd) const
{
    if ( !hwnd || !m_window )
        return false;

    wxWinUITopLevelHost * const host =
        wxWinUITopLevelHost::ForWindow(m_window, false);
    if ( !host || host->GetFocusOwner() != m_window )
        return false;

    const HWND bridge = host->GetBridgeHwnd();
    return hwnd == bridge || (bridge && ::IsChild(bridge, hwnd));
}

bool wxWinUIControlHost::NavigateFocus(bool WXUNUSED(forward))
{
    if ( !m_window )
        return false;

    wxWinUITopLevelHost * const host =
        wxWinUITopLevelHost::ForWindow(m_window, false);
    if ( !host || !host->FindSlot(m_window) )
        return false;

    host->FocusSlot(m_window);
    return true;
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

    // Resolve the logical current control: with the focus inside an island,
    // it is the slot owner recorded by the arbiter.
    wxWindow *current = wxWinUITopLevelHost::ResolveFocusHwnd((WXHWND)focus);
    const bool focusInIsland = current != nullptr;
    if ( !current )
        current = wxGetWindowFromHWND((WXHWND)focus);
    if ( !current )
        return false;

    wxWinUITopLevelHost * const host =
        wxWinUITopLevelHost::ForWindow(current, false);
    if ( !host )
        return false;   // no island in this TLW: default wx processing

    const HWND hwndCurrent = static_cast<HWND>(current->GetHWND());
    const HWND hwndTLW = static_cast<HWND>(host->GetTLW()->GetHWND());
    if ( !hwndCurrent || !hwndTLW )
        return false;

    const bool previous = (::GetKeyState(VK_SHIFT) & 0x8000) != 0;
    HWND next = ::GetNextDlgTabItem(hwndTLW, hwndCurrent, previous);
    if ( !next || next == hwndCurrent )
        return false;

    wxWindow * const nextWin = wxGetWindowFromHWND((WXHWND)next);
    if ( nextWin && host->FindSlot(nextWin) )
    {
        host->FocusSlot(nextWin);
        return true;
    }

    // Only take over the Tab when leaving an island: between two plain wx
    // windows the standard wx navigation must keep running.
    if ( focusInIsland )
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
    if ( !wxWinUITopLevelHost::ResolveFocusHwnd((WXHWND)::GetFocus()) )
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
    //
    // OWNED top-level windows (dialogs) get it too: an earlier conclusion
    // that DWM never composes the backdrop on them was wrong (re-verified
    // on the spike's modal dialog: with the marking + per-window priming
    // below the material composes fine, and the mismatched black control
    // patches disappear).
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
    // resized; before that it can show as an opaque rectangle (most visibly on
    // SDR monitors).  Nudge the window size by one pixel and back once PER
    // WINDOW to force its backdrop to initialise, without requiring the user
    // to resize manually.  (This used to be done once per monitor, which left
    // every dialog opened after the main frame with a flat, Mica-less
    // background.)
    // Tracked with a window prop (not a static set) so the flag dies with
    // the window and a recycled handle value cannot skip a new window.
    static const wchar_t s_primedProp[] = L"wxWinUIBackdropPrimed";
    if ( ::GetPropW(hwnd, s_primedProp) )
        return;

    RECT rect;
    if ( !::GetWindowRect(hwnd, &rect) )
        return;

    const int width = rect.right - rect.left;
    const int height = rect.bottom - rect.top;
    if ( width <= 1 || height <= 1 )
        return;  // not yet laid out; prime on a later call

    ::SetPropW(hwnd, s_primedProp, reinterpret_cast<HANDLE>(1));

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

    wxWinUITopLevelHost::ApplyThemeToAll(gs_winuiElementTheme);

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
