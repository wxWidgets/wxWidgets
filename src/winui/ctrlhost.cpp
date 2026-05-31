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
#endif

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

        wxWindow *hostWindow = wxGetTopLevelParent(window);
        if ( !hostWindow )
            hostWindow = window;

        HWND hwnd = static_cast<HWND>(window->GetHWND());
        ::SetWindowLongPtr
        (
            hwnd,
            GWL_STYLE,
            ::GetWindowLongPtr(hwnd, GWL_STYLE) |
                WS_CLIPCHILDREN | WS_CLIPSIBLINGS
        );

        m_hostHwnd = static_cast<HWND>(hostWindow->GetHWND());
        const auto windowId = winrt::Microsoft::UI::GetWindowIdFromWindow(m_hostHwnd);

        m_source.Initialize(windowId);
        m_source.SiteBridge().ResizePolicy(ContentSizePolicy::None);

        m_bridgeHwnd = winrt::Microsoft::UI::GetWindowFromWindowId(
            m_source.SiteBridge().WindowId());
        ::SetWindowLongPtr
        (
            m_bridgeHwnd,
            GWL_STYLE,
            ::GetWindowLongPtr(m_bridgeHwnd, GWL_STYLE) |
                WS_CHILD | WS_VISIBLE | WS_TABSTOP
        );
        ::SetWindowPos
        (
            m_bridgeHwnd,
            HWND_TOP,
            0,
            0,
            0,
            0,
            SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_SHOWWINDOW
        );

        m_window = window;
        m_window->Bind(wxEVT_MOVE, &wxWinUIControlHost::OnWindowMove, this);
        m_window->Bind(wxEVT_SIZE, &wxWinUIControlHost::OnWindowSize, this);
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
    if ( m_window )
    {
        m_window->Unbind(wxEVT_MOVE, &wxWinUIControlHost::OnWindowMove, this);
        m_window->Unbind(wxEVT_SIZE, &wxWinUIControlHost::OnWindowSize, this);
        m_window = nullptr;
    }
    m_hostHwnd = nullptr;
    m_bridgeHwnd = nullptr;

    if ( !m_source )
        return;

    try
    {
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

    m_source.Content(element);
    MoveAndResize();
}

void wxWinUIControlHost::ClearContent()
{
    if ( !m_source )
        return;

    m_source.Content(nullptr);
}

void wxWinUIControlHost::MoveAndResize()
{
    if ( !m_source || !m_window )
        return;

    HWND hwnd = static_cast<HWND>(m_window->GetHWND());
    RECT rect;
    if ( !::GetClientRect(hwnd, &rect) )
        return;

    POINT points[2] = { { rect.left, rect.top }, { rect.right, rect.bottom } };
    if ( !::MapWindowPoints(hwnd, m_hostHwnd, points, WXSIZEOF(points)) )
        return;

    const int width = points[1].x - points[0].x;
    const int height = points[1].y - points[0].y;
    if ( width <= 0 || height <= 0 )
        return;

    try
    {
        m_source.SiteBridge().MoveAndResize(
            { points[0].x, points[0].y, width, height });
        m_source.SiteBridge().Show();
        if ( m_bridgeHwnd )
        {
            ::SetWindowPos
            (
                m_bridgeHwnd,
                HWND_TOP,
                points[0].x,
                points[0].y,
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

void wxWinUIControlHost::OnWindowMove(wxMoveEvent& event)
{
    MoveAndResize();
    event.Skip();
}

void wxWinUIControlHost::OnWindowSize(wxSizeEvent& event)
{
    MoveAndResize();
    event.Skip();
}

#endif // wxUSE_WINUI3
