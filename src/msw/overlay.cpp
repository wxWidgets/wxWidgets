///////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/overlay.cpp
// Author:      Ali Kettab
// Copyright:   (c) 2022 Ali Kettab
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#include "wx/private/overlay.h"
#include "wx/nativewin.h"
#include "wx/dcclient.h"
#include "wx/dcmemory.h"
#include "wx/msw/dc.h"
#include "wx/msw/private.h"

namespace // anonymous
{
class wxOverlayWindow : public wxNativeContainerWindow
{
public:
    static wxWindow* New(const wxRect& rect)
    {
        WNDCLASS wndclass;
        wxZeroMemory(wndclass);

        wndclass.lpfnWndProc   = ::DefWindowProc;
        wndclass.hInstance     = wxGetInstance();
        wndclass.lpszClassName = GetOverlayWindowClass();

        if ( !::RegisterClass(&wndclass) )
        {
            wxLogLastError(wxS("RegisterClass() in wxOverlayWindow::Create()"));
            return nullptr;
        }

        const wxPoint pos = rect.GetPosition();
        const wxSize size = rect.GetSize();
        WXDWORD exStyle = WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_NOACTIVATE;

        HWND hwnd = ::CreateWindowEx
                      (
                        exStyle,
                        GetOverlayWindowClass(),
                        nullptr,
                        WS_POPUP,
                        pos.x, pos.y,
                        size.x, size.y,
                        (HWND)nullptr,
                        (HMENU)nullptr,
                        wxGetInstance(),
                        (LPVOID)nullptr
                      );

        if ( !hwnd )
        {
            wxLogLastError(wxS("CreateWindowEx() in wxOverlayWindow::Create()"));
            return nullptr;
        }

        if ( !::SetLayeredWindowAttributes(hwnd, 0, 128, LWA_COLORKEY|LWA_ALPHA) )
        {
            wxLogLastError(wxS("SetLayeredWindowAttributes() in wxOverlayWindow::Create()"));
        }

        auto overlayWin = new wxOverlayWindow(hwnd);
        overlayWin->Show(true);
        return overlayWin;
    }

    static const wxChar* GetOverlayWindowClass()
    {
        return wxS("wxOverlayWindow");
    }

    virtual bool Show(bool show) override
    {
        if ( !GetHandle() || !wxWindowBase::Show(show) )
            return false;

        if ( show )
        {
            ::SetWindowPos(GetHandle(), nullptr, 0, 0, 0, 0,
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

    virtual bool Destroy() override
    {
        HWND hwnd = GetHandle();

        if ( hwnd && !::DestroyWindow(hwnd) )
        {
            wxLogLastError(wxS("DestroyWindow() in wxOverlayWindow::Destroy()"));
            return false;
        }

        if ( !::UnregisterClass(GetOverlayWindowClass(), wxGetInstance()) )
        {
            wxLogLastError(wxS("UnregisterClass() in wxOverlayWindow::Destroy()"));
        }

        return true;
    }

private:
    wxOverlayWindow(HWND hwnd) : wxNativeContainerWindow(hwnd) {}
    ~wxOverlayWindow() = default;
};
} // anonymous namespace

class wxOverlayImpl : public wxOverlay::Impl
{
public:
    wxOverlayImpl() = default;
    ~wxOverlayImpl() = default;

    virtual bool IsNative() const override { return true; }
    virtual void Reset() override;
    virtual bool IsOk() override;
    virtual void Init(wxDC* dc, int x , int y , int width , int height) override;
    virtual void BeginDrawing(wxDC* dc) override;
    virtual void EndDrawing(wxDC* dc) override;
    virtual void Clear(wxDC* dc) override;

public:
    // window the overlay is associated with
    wxWindow* m_window = nullptr;
    // the overlay window itself. doesn't have to be child of any window
    // and is totally managed by this class
    wxWindow* m_overlayWindow = nullptr;

    wxRect      m_rect;

    // Drawing on the overlay window is achieved by hijacking the existing wxDC.
    // i.e. any drawing done through wxDC should go to the offscreen bitmap m_bitmap
    // which will eventually be drawn on the overlay window.
    wxBitmap    m_bitmap;
    wxMemoryDC  m_memDC;    // for memory dc construction only.
    WXHDC       m_hdc = 0;  // the original HDC

    wxDECLARE_NO_COPY_CLASS(wxOverlayImpl);
};

bool wxOverlayImpl::IsOk()
{
    return m_overlayWindow != nullptr;
}

void wxOverlayImpl::Init(wxDC* dc, int , int , int , int )
{
    if ( IsOk() )
        return;

    m_window = dc->GetWindow();

    m_rect.SetSize(m_window->GetClientSize());
    m_rect.SetPosition(m_window->GetScreenPosition());

    m_bitmap.CreateWithDIPSize(m_rect.GetSize(), m_window->GetDPIScaleFactor());

    m_overlayWindow = wxOverlayWindow::New(m_rect);
}

void wxOverlayImpl::BeginDrawing(wxDC* dc)
{
    wxCHECK_RET( IsOk(), wxS("overlay not initialized") );

    m_memDC.SelectObject(m_bitmap);

    auto impl = dc->GetImpl();
    auto msw_impl = static_cast<wxMSWDCImpl*>(impl);

    m_hdc = GetHdcOf(*msw_impl); // save the original hdc

    msw_impl->SetHDC(GetHdcOf(m_memDC));
    msw_impl->UpdateClipBox();
}

void wxOverlayImpl::EndDrawing(wxDC* dc)
{
    wxCHECK_RET( IsOk(), wxS("overlay not initialized") );

    m_memDC.SelectObject(wxNullBitmap);

    auto impl = dc->GetImpl();
    auto msw_impl = static_cast<wxMSWDCImpl*>(impl);

    msw_impl->SetHDC(m_hdc); // restore the original hdc
    msw_impl->UpdateClipBox();

    wxWindowDC winDC(m_overlayWindow);
    winDC.DrawBitmap(m_bitmap, wxPoint(0, 0));
}

void wxOverlayImpl::Clear(wxDC* WXUNUSED(dc))
{
    wxCHECK_RET( IsOk(), wxS("overlay not initialized") );

    // doesn't work with non double buffered m_window
    // m_memDC.SetBackground(*wxTRANSPARENT_BRUSH);

    m_memDC.SetBackground(wxBrush(wxTransparentColour));
    m_memDC.Clear();
}

void wxOverlayImpl::Reset()
{
    if ( IsOk() )
    {
        m_overlayWindow->Destroy();
        m_overlayWindow = nullptr;
    }
}

wxOverlay::Impl* wxOverlay::Create()
{
    return new wxOverlayImpl;
}
