/////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/overlay.cpp
// Purpose:     wxOverlay implementation for wxMSW
// Author:      Kettab Ali
// Created:     2021-07-12
// Copyright:   (c) wxWidgets team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#ifndef WX_PRECOMP
    #include "wx/toplevel.h"
    #include "wx/window.h"
    #include "wx/dcclient.h"
#endif

#include "wx/display.h"
#include "wx/nativewin.h"
#include "wx/scrolwin.h"
#include "wx/msw/private.h"
#include "wx/msw/private/overlay.h"


namespace wxPrivate
{
static const wxChar* gs_overlayClassName = NULL;

class wxOverlayWindow : public wxNativeContainerWindow
{
public:
    static wxWindow*  Create(wxOverlay::Target target)
    {
        static const wxChar* OVERLAY_WINDOW_CLASS = wxS("wxOverlayWindow");

        if ( !gs_overlayClassName )
        {
            WNDCLASS wndclass;
            wxZeroMemory(wndclass);

            wndclass.lpfnWndProc   = ::DefWindowProc;
            wndclass.hInstance     = wxGetInstance();
            wndclass.lpszClassName = OVERLAY_WINDOW_CLASS;

            if ( !::RegisterClass(&wndclass) )
            {
                wxLogLastError(wxS("RegisterClass() in wxOverlayWindow::Create()"));
                return NULL;
            }

            gs_overlayClassName = OVERLAY_WINDOW_CLASS;
        }

        DWORD exStyle = WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_NOACTIVATE;

        if ( target == wxOverlay::Overlay_Screen )
            exStyle |= WS_EX_TOPMOST;

        HWND hwnd = ::CreateWindowEx
                      (
                        exStyle,
                        OVERLAY_WINDOW_CLASS,
                        NULL,
                        WS_POPUP, 0, 0, 0,
                        0,
                        (HWND) NULL,
                        (HMENU)NULL,
                        wxGetInstance(),
                        (LPVOID) NULL
                      );

        if ( !hwnd )
        {
            wxLogLastError(wxS("CreateWindowEx() in wxOverlayWindow::Create()"));
            return NULL;
        }

        if ( !::SetLayeredWindowAttributes(hwnd, 0, 128, LWA_COLORKEY|LWA_ALPHA) )
        {
            wxLogLastError(wxS("SetLayeredWindowAttributes() in wxOverlayWindow::Create()"));
        }

        return new wxOverlayWindow(hwnd);
    }

    virtual bool Destroy() wxOVERRIDE
    {
        HWND hwnd = GetHandle();

        if ( hwnd && !::DestroyWindow(hwnd) )
        {
            wxLogLastError(wxS("DestroyWindow in wxOverlayWindow::Destroy()"));
            return false;
        }

        if ( gs_overlayClassName )
        {
            if ( !::UnregisterClass(gs_overlayClassName, wxGetInstance()) )
            {
                wxLogLastError(wxS("UnregisterClass in wxOverlayWindow::Destroy()"));
            }

            gs_overlayClassName = NULL;
        }

        return true;
    }

    virtual bool Show(bool show) wxOVERRIDE
    {
        if ( !wxWindowBase::Show(show) || !GetHandle() )
            return false;

        if ( show )
        {
            ::SetWindowPos(GetHandle(), NULL, 0, 0, 0, 0,
                           SWP_NOSIZE |
                           SWP_NOMOVE |
                           SWP_NOREDRAW |
                           SWP_NOOWNERZORDER |
                           SWP_NOACTIVATE |
                           SWP_SHOWWINDOW);
        }
        else
        {
            ::ShowWindow(GetHandle(), SW_HIDE);
        }

        return true;
    }

private:
    wxOverlayWindow(HWND hwnd) : wxNativeContainerWindow(hwnd) {}
    ~wxOverlayWindow() {}
};
} // wxPrivate namespace

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxOverlay
// ----------------------------------------------------------------------------

wxOverlayMSWImpl::wxOverlayMSWImpl() : m_timer(this)
{
    m_window = NULL;
    m_overlayWindow = NULL;
}

wxOverlayMSWImpl::~wxOverlayMSWImpl()
{
    Reset();
}

bool wxOverlayMSWImpl::IsOk()
{
    return m_overlayWindow != NULL && GetBitmap().IsOk();
}

void wxOverlayMSWImpl::InitFromWindow(wxWindow* win, wxOverlay::Target target)
{
    wxASSERT_MSG( !IsOk() , "You cannot Init an overlay twice" );
    wxASSERT_MSG( !m_window || m_window == win,
        "wxOverlay re-initialized with a different window");

    if ( target != wxOverlay::Overlay_Screen )
    {
        wxTopLevelWindow * const
            appWin = wxDynamicCast(wxGetTopLevelParent(win), wxTopLevelWindow);

        appWin->Bind(wxEVT_MOVE_START, &wxOverlayMSWImpl::OnMoveStart, this);
        appWin->Bind(wxEVT_MOVE_END,   &wxOverlayMSWImpl::OnMoveEnd,   this);
        appWin->Bind(wxEVT_SIZE,       &wxOverlayMSWImpl::OnResize,    this);
        appWin->Bind(wxEVT_ICONIZE,    &wxOverlayMSWImpl::OnIconize,   this);
    }

    m_window = win;

    wxRect rect;

    if ( target == wxOverlay::Overlay_Screen )
    {
        rect = wxDisplay(win).GetGeometry();
    }
    else
    {
        rect.SetSize(win->GetClientSize());
        rect.SetPosition(win->GetScreenPosition());
    }

    if ( m_rect.GetWidth() < rect.GetWidth() ||
         m_rect.GetHeight() < rect.GetHeight() )
    {
        m_rect = rect;
    }

    wxBitmap& bitmap = GetBitmap();
    bitmap.Create(m_rect.GetWidth(), m_rect.GetHeight());

    if ( !m_overlayWindow )
    {
        m_overlayWindow = wxPrivate::wxOverlayWindow::Create(target);
    }

    if ( m_window->GetLayoutDirection() == wxLayout_RightToLeft )
    {
        m_rect.x -= m_rect.GetWidth();
        m_overlayWindow->SetLayoutDirection(wxLayout_RightToLeft);
    }

    m_overlayWindow->Move(m_rect.GetPosition());
    m_overlayWindow->SetSize(m_rect.GetSize());
    m_overlayWindow->Show();
}

void wxOverlayMSWImpl::InitFromDC(wxDC* , int, int, int, int)
{
    wxFAIL_MSG("wxOverlay initialized from wxDC not implemented under wxMSW");
}

void wxOverlayMSWImpl::SetUpdateRectangle(const wxRect& rect)
{
    m_rect = rect;
}

void wxOverlayMSWImpl::BeginDrawing(wxDC* dc)
{
    wxScrolledWindow* const win = wxDynamicCast(m_window, wxScrolledWindow);
    if ( win )
        win->PrepareDC(*dc);
}

void wxOverlayMSWImpl::EndDrawing(wxDC* dc)
{
    m_oldRect = m_rect;

    dc->GetClippingBox(&m_rect.x, &m_rect.y, &m_rect.width, &m_rect.height);

    wxWindowDC winDC(m_overlayWindow);
    const wxPoint pt = dc->GetDeviceOrigin();
    winDC.SetDeviceOrigin(pt.x, pt.y);

    // It looks like Blit() is still having a problem with RTL!
    int rect_x = m_rect.x;
    if ( m_overlayWindow->GetLayoutDirection() == wxLayout_RightToLeft )
        rect_x += 1;

    winDC.Blit(rect_x, m_rect.y, m_rect.width, m_rect.height, dc, m_rect.x, m_rect.y);

    dc->DestroyClippingRegion();
}

void wxOverlayMSWImpl::Clear(wxDC* dc)
{
    wxASSERT_MSG( IsOk(),
                  "You cannot Clear an overlay that is not initialized" );

    wxRect rect = m_rect;
    rect.Union(m_oldRect);

    dc->SetClippingRegion(rect);

    dc->SetBackground(wxBrush(wxTransparentColour));
    dc->Clear();
}

void wxOverlayMSWImpl::Reset()
{
    if ( IsOk() )
    {
        DWORD dwExStyle = ::GetWindowLong(m_overlayWindow->GetHandle(), GWL_EXSTYLE);
        if ( (dwExStyle & WS_EX_TOPMOST) == 0 )
        {
            wxTopLevelWindow * const
                appWin = wxDynamicCast(wxGetTopLevelParent(m_window), wxTopLevelWindow);

            if ( appWin )
            {
                appWin->Unbind(wxEVT_MOVE_START, &wxOverlayMSWImpl::OnMoveStart, this);
                appWin->Unbind(wxEVT_MOVE_END,   &wxOverlayMSWImpl::OnMoveEnd,   this);
                appWin->Unbind(wxEVT_SIZE,       &wxOverlayMSWImpl::OnResize,    this);
                appWin->Unbind(wxEVT_ICONIZE,    &wxOverlayMSWImpl::OnIconize,   this);
            }
        }
    }

    DoReset();
}

void wxOverlayMSWImpl::DoReset()
{
    if ( !IsOk() )
        return;

    m_oldRect = wxRect();

    GetBitmap() = wxBitmap();

    if ( m_overlayWindow )
    {
        m_overlayWindow->Destroy();
        m_overlayWindow = NULL;
    }
}

void wxOverlayMSWImpl::OnMoveStart(wxMoveEvent& event)
{
    DoReset();
    event.Skip();
}

void wxOverlayMSWImpl::OnMoveEnd(wxMoveEvent& event)
{
    m_window->GetMainWindowOfCompositeControl()->Refresh(false);
    event.Skip();
}

void wxOverlayMSWImpl::OnResize(wxSizeEvent& event)
{
    DoReset();
    m_timer.StartOnce(100);
    event.Skip();
}

void wxOverlayMSWImpl::OnIconize(wxIconizeEvent& event)
{
    if ( event.IsIconized() )
    {
        DoReset();
    }
    else
    {
        m_window->GetMainWindowOfCompositeControl()->Refresh(false);
    }
}

void wxOverlayMSWImpl::OverlayTimer::Notify()
{
    m_owner->GetWindow()->GetMainWindowOfCompositeControl()->Refresh(false);
}

wxOverlayImpl* wxOverlayImpl::Create() { return new wxOverlayMSWImpl(); }
