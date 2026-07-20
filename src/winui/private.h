/////////////////////////////////////////////////////////////////////////////
// Name:        src/winui/private.h
// Purpose:     private wxWinUI helpers
// Author:      wxWidgets development team
// Created:     2026-05-31
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WINUI_PRIVATE_H_
#define _WX_WINUI_PRIVATE_H_

#include "wx/window.h"

#include "wx/msw/wrapwin.h"

#ifdef GetCurrentTime
    #undef GetCurrentTime
#endif

// C++/WinRT's base.h uses std::array in constexpr code which the min/max macros
// from <windows.h> break (they may be pulled in, without NOMINMAX, by wx headers
// included before this one).  Undefine them before including any winrt header.
#ifdef min
    #undef min
#endif
#ifdef max
    #undef max
#endif

#include <winrt/base.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Graphics.h>
#include <winrt/Windows.UI.h>
#include <winrt/Microsoft.UI.h>
#include <winrt/Microsoft.UI.Content.h>
#include <winrt/Microsoft.UI.Interop.h>
#include <winrt/Microsoft.UI.Xaml.h>
#include <winrt/Microsoft.UI.Xaml.Controls.h>
#include <winrt/Microsoft.UI.Xaml.Controls.Primitives.h>
#include <winrt/Microsoft.UI.Xaml.Hosting.h>
#include <winrt/Microsoft.UI.Xaml.Input.h>
#include <winrt/Microsoft.UI.Xaml.Markup.h>
#include <winrt/Microsoft.UI.Xaml.Media.h>
#include <winrt/Microsoft.UI.Xaml.Media.Imaging.h>
#include <winrt/Windows.System.h>

class wxWinUIControlHost
{
public:
    ~wxWinUIControlHost();

    bool Initialize(wxWindow *window);
    void Close();

    bool IsOk() const { return static_cast<bool>(m_source); }

    void SetContent(const winrt::Microsoft::UI::Xaml::UIElement& element);
    void ClearContent();

    // Apply an element theme to the hosted content, if it is a FrameworkElement.
    void ApplyTheme(winrt::Microsoft::UI::Xaml::ElementTheme theme);

    // Force the island to recompose after a programmatic change.  Hosted XAML
    // islands only commit a new composition frame when the site bridge is
    // resized, so property changes made outside of user input would otherwise
    // not become visible until the next resize.
    void ForceRender();

    // Apply the island's native Mica backdrop.  Must run while the message loop
    // is pumping (deferred via CallAfter), otherwise the backdrop controller
    // deadlocks on the dispatcher queue.
    void ApplyBackdropMaterial();

    HWND GetHostHWND() const { return m_hostHwnd; }
    HWND GetBridgeHWND() const { return m_bridgeHwnd; }
    bool ContainsFocus(HWND hwnd) const;
    bool NavigateFocus(bool forward);

    // Limit the island bridge to the top portion of the control's client area
    // (in physical pixels), leaving the area below free of any XAML island.
    // Used by wxNotebook so that its page windows -- and crucially their native
    // scrollbars -- are not occluded by the TabView's composition surface.
    // Pass 0 to fill the whole client area (the default).
    void SetBridgeHeightLimit(int physicalHeight);

    // Reflect a wxWindow::SetCursor() on this island (per-window cursor, e.g.
    // setting a wait cursor on a single control).  Mapped to ProtectedCursor.
    void ApplyWxCursor(const wxCursor& cursor);

    // The wx control whose HWND hosts this island.
    wxWindow *HostedWindow() const { return m_window; }

private:
    void UpdateContentSize(int width, int height);
    void MoveAndResize();
    void OnWindowSize(wxSizeEvent& event);
    void OnSetFocus(wxFocusEvent& event);
    void OnTakeFocusRequested(
        winrt::Microsoft::UI::Xaml::Hosting::DesktopWindowXamlSourceTakeFocusRequestedEventArgs const& event);

    wxWindow *m_window = nullptr;
    HWND m_hostHwnd = nullptr;
    HWND m_bridgeHwnd = nullptr;
    winrt::Microsoft::UI::Xaml::Hosting::DesktopWindowXamlSource m_source{ nullptr };
    winrt::Microsoft::UI::Xaml::UIElement m_content{ nullptr };
    winrt::event_token m_takeFocusRequestedToken{};
    bool m_backdropApplied = false;
    int m_bridgeHeightLimit = 0;
};

// Current element theme requested by the application (Default == follow system).
winrt::Microsoft::UI::Xaml::ElementTheme wxWinUIGetCurrentElementTheme();
bool wxWinUIIsDarkTheme();

void wxWinUILogException(const char *what, const winrt::hresult_error& e);
winrt::hstring wxWinUIToHString(const wxString& str);
wxString wxWinUIFromHString(const winrt::hstring& str);

// Convert a wxBitmap to a WinUI WriteableBitmap (premultiplied BGRA).  Returns
// nullptr for an invalid bitmap.
class wxBitmap;
winrt::Microsoft::UI::Xaml::Media::Imaging::WriteableBitmap
wxWinUIWriteableBitmapFromBitmap(const wxBitmap& bitmap);

// Peer state shared by wxChoice and wxComboBox (which reconfigures the same
// WinUI ComboBox to be editable).
class wxWinUIChoiceImpl
{
public:
    wxWinUIControlHost host;
    winrt::Microsoft::UI::Xaml::Controls::ComboBox comboBox{ nullptr };
    winrt::event_token selectionChangedToken{};

    // wxComboBox only: the inner TextBox of an editable ComboBox (resolved
    // lazily from the control template once it is loaded), and its hooks.
    winrt::Microsoft::UI::Xaml::Controls::TextBox editBox{ nullptr };
    winrt::event_token textSubmittedToken{};
    long long textChangedCallbackToken = 0;
};

// Strip '&' mnemonics ("&&" -> "&"): WinUI labels don't support them.
wxString wxWinUIRemoveMnemonics(const wxString& label);

// Set wrapped, selectable text on a dialog TextBlock.
void wxWinUISetDialogText(winrt::Microsoft::UI::Xaml::Controls::TextBlock const& text,
                          const wxString& value);

// Set (or clear, when tip is empty) a WinUI tooltip on a hosted XAML element.
// Shared by the wxWinUI controls so that wxWindow::SetToolTip() actually shows
// a tooltip over the XAML island instead of being silently dropped.
void wxWinUISetToolTip(const winrt::Microsoft::UI::Xaml::UIElement& element,
                       const wxString& tip);

inline winrt::Microsoft::UI::Xaml::Media::SolidColorBrush
wxWinUIBrush(unsigned char red,
             unsigned char green,
             unsigned char blue,
             unsigned char alpha = 255)
{
    winrt::Windows::UI::Color color{};
    color.A = alpha;
    color.R = red;
    color.G = green;
    color.B = blue;
    return winrt::Microsoft::UI::Xaml::Media::SolidColorBrush(color);
}

#endif // _WX_WINUI_PRIVATE_H_
