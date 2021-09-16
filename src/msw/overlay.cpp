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
    #include "wx/log.h"
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
    static wxWindow*  Create()
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

        HWND hwnd = ::CreateWindowEx
                      (
                        WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_NOACTIVATE | WS_EX_TOPMOST,
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

wxOverlayMSWImpl::wxOverlayMSWImpl()
{
    m_window = NULL;
    m_overlayWindow = NULL;
}

wxOverlayMSWImpl::~wxOverlayMSWImpl()
{
    Reset();

    m_overlayWindow->Destroy();
}

bool wxOverlayMSWImpl::IsOk()
{
    return GetBitmap().IsOk();
}

void wxOverlayMSWImpl::InitFromWindow(wxWindow* win, bool fullscreen)
{
    wxASSERT_MSG( !IsOk() , "You cannot Init an overlay twice" );

    m_window = win;
    m_fullscreen = fullscreen;

    if ( fullscreen )
    {
        m_rect = wxDisplay(win).GetGeometry();
    }
    else
    {
        m_rect.SetSize(win->GetClientSize());
        m_rect.SetPosition(win->GetScreenPosition());
    }

    wxBitmap& bitmap = GetBitmap();
    bitmap.Create(m_rect.GetWidth(), m_rect.GetHeight());

    if ( !m_overlayWindow )
    {
        m_overlayWindow = wxPrivate::wxOverlayWindow::Create();
        m_overlayWindow->SetBackgroundColour(*wxBLUE);
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

    winDC.Blit(m_rect.x, m_rect.y, m_rect.width, m_rect.height, dc, m_rect.x, m_rect.y);

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
    GetBitmap() = wxBitmap();

    if ( m_overlayWindow )
        m_overlayWindow->Hide();
}

wxOverlayImpl* wxOverlayImpl::Create() { return new wxOverlayMSWImpl(); }
