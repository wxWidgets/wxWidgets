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
#include <winrt/Microsoft.UI.Xaml.Markup.h>
#include <winrt/Microsoft.UI.Xaml.Media.h>

class wxWinUIControlHost
{
public:
    ~wxWinUIControlHost();

    bool Initialize(wxWindow *window);
    void Close();

    bool IsOk() const { return static_cast<bool>(m_source); }

    void SetContent(const winrt::Microsoft::UI::Xaml::UIElement& element);
    void ClearContent();

private:
    void MoveAndResize();
    void OnWindowMove(wxMoveEvent& event);
    void OnWindowSize(wxSizeEvent& event);

    wxWindow *m_window = nullptr;
    HWND m_hostHwnd = nullptr;
    HWND m_bridgeHwnd = nullptr;
    winrt::Microsoft::UI::Xaml::Hosting::DesktopWindowXamlSource m_source{ nullptr };
};

void wxWinUILogException(const char *what, const winrt::hresult_error& e);
winrt::hstring wxWinUIToHString(const wxString& str);
wxString wxWinUIFromHString(const winrt::hstring& str);

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
