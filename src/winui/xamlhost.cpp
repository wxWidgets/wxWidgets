/////////////////////////////////////////////////////////////////////////////
// Name:        src/winui/xamlhost.cpp
// Purpose:     wxWinUI XAML island host
// Author:      wxWidgets development team
// Created:     2026-05-31
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#include "wx/winui/xamlhost.h"

#if wxUSE_WINUI3

#ifndef WX_PRECOMP
    #include "wx/log.h"
    #include "wx/string.h"
#endif

#include "wx/winui/winui.h"

#include "wx/msw/wrapwin.h"

#ifdef GetCurrentTime
    #undef GetCurrentTime
#endif

#include <winrt/base.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Graphics.h>
#include <winrt/Microsoft.UI.h>
#include <winrt/Microsoft.UI.Content.h>
#include <winrt/Microsoft.UI.Interop.h>
#include <winrt/Microsoft.UI.Xaml.h>
#include <winrt/Microsoft.UI.Xaml.Hosting.h>
#include <winrt/Microsoft.UI.Xaml.Markup.h>

#include <string>

namespace
{

void wxWinUILogException(const char *what, const winrt::hresult_error& e)
{
    wxLogWarning("%s failed with HRESULT 0x%08lx: %s",
                 what,
                 static_cast<unsigned long>(e.code()),
                 wxString(e.message().c_str()));
}

} // namespace

class wxWinUIXamlHostImpl
{
public:
    winrt::Microsoft::UI::Xaml::Hosting::DesktopWindowXamlSource source{ nullptr };
    winrt::Microsoft::UI::Xaml::UIElement content{ nullptr };
};

wxIMPLEMENT_DYNAMIC_CLASS(wxWinUIXamlHost, wxWindow);

wxWinUIXamlHost::wxWinUIXamlHost()
{
}

wxWinUIXamlHost::wxWinUIXamlHost(wxWindow *parent,
                                 wxWindowID id,
                                 const wxPoint& pos,
                                 const wxSize& size,
                                 long style,
                                 const wxString& name)
{
    Create(parent, id, pos, size, style, name);
}

wxWinUIXamlHost::~wxWinUIXamlHost()
{
    ClearContent();

    if ( m_impl && m_impl->source )
    {
        try
        {
            m_impl->source.Close();
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException("DesktopWindowXamlSource::Close", e);
        }
    }
}

bool wxWinUIXamlHost::Create(wxWindow *parent,
                             wxWindowID id,
                             const wxPoint& pos,
                             const wxSize& size,
                             long style,
                             const wxString& name)
{
    if ( !wxWindow::Create(parent, id, pos, size, style, name) )
        return false;

    m_impl.reset(new wxWinUIXamlHostImpl);
    if ( !InitializeXamlSource() )
        return false;

    Bind(wxEVT_SIZE, &wxWinUIXamlHost::OnSize, this);
    return true;
}

bool wxWinUIXamlHost::InitializeXamlSource()
{
    if ( m_impl && m_impl->source )
        return true;

    if ( !wxWinUI3Initialize() )
        return false;

    if ( !m_impl )
        m_impl.reset(new wxWinUIXamlHostImpl);

    try
    {
        using namespace winrt::Microsoft::UI::Content;
        using namespace winrt::Microsoft::UI::Xaml::Hosting;

        m_impl->source = DesktopWindowXamlSource();

        HWND hwnd = static_cast<HWND>(GetHWND());
        ::SetWindowLongPtr
        (
            hwnd,
            GWL_STYLE,
            ::GetWindowLongPtr(hwnd, GWL_STYLE) |
                WS_CLIPCHILDREN | WS_CLIPSIBLINGS
        );

        const auto windowId = winrt::Microsoft::UI::GetWindowIdFromWindow(hwnd);

        m_impl->source.Initialize(windowId);
        m_impl->source.SiteBridge().ResizePolicy(ContentSizePolicy::None);

        HWND bridgeHwnd = winrt::Microsoft::UI::GetWindowFromWindowId(
            m_impl->source.SiteBridge().WindowId());
        ::SetWindowLongPtr
        (
            bridgeHwnd,
            GWL_STYLE,
            ::GetWindowLongPtr(bridgeHwnd, GWL_STYLE) |
                WS_CHILD | WS_VISIBLE | WS_TABSTOP
        );

        MoveAndResizeXamlSource();
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("DesktopWindowXamlSource initialization", e);
        m_impl->source = nullptr;
        return false;
    }

    return true;
}

bool wxWinUIXamlHost::SetContentFromXaml(const wxString& xaml)
{
    if ( !InitializeXamlSource() )
        return false;

    try
    {
        using namespace winrt::Microsoft::UI::Xaml;
        using namespace winrt::Microsoft::UI::Xaml::Markup;

        const std::wstring xamlText = xaml.ToStdWstring();
        const auto loaded = XamlReader::Load(winrt::hstring(xamlText));
        m_impl->content = loaded.as<UIElement>();
        m_impl->source.Content(m_impl->content);
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("XamlReader::Load", e);
        return false;
    }

    return true;
}

void wxWinUIXamlHost::ClearContent()
{
    if ( !m_impl || !m_impl->source )
        return;

    try
    {
        m_impl->source.Content(winrt::Microsoft::UI::Xaml::UIElement{ nullptr });
        m_impl->content = nullptr;
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("DesktopWindowXamlSource::Content", e);
    }
}

void wxWinUIXamlHost::MoveAndResizeXamlSource()
{
    if ( !m_impl || !m_impl->source )
        return;

    const wxSize size = GetClientSize();
    if ( size.x <= 0 || size.y <= 0 )
        return;

    try
    {
        m_impl->source.SiteBridge().MoveAndResize({ 0, 0, size.x, size.y });
        m_impl->source.SiteBridge().Show();
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("DesktopWindowXamlSource::MoveAndResize", e);
    }
}

void wxWinUIXamlHost::OnSize(wxSizeEvent& event)
{
    MoveAndResizeXamlSource();
    event.Skip();
}

#endif // wxUSE_WINUI3
