///////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/overlay.cpp
// Author:      Ali Kettab
// Copyright:   (c) 2022 Ali Kettab
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#include "wx/private/overlay.h"
#include "wx/dcclient.h"
#include "wx/dcmemory.h"
#include "wx/msw/dc.h"
#include "wx/msw/private.h"

// A note about updating the overlay window :
// ------------------------------------------
// Although both SetLayeredWindowAttributes and UpdateLayeredWindow can be used
// to update the overlay window, the latter is more flexible because it can perform
// per-pixel alpha blending like the other ports do. But this requires the DC to
// support alpha drawing, which can only be done via graphics contexts. That's why
// it's not used by default and is only enabled by SetOpacity(-1) call before the
// overlay window is initialized. Like this for example:
//      ...
//      m_overlay.SetOpacity(-1);
//      wxClientDC dc(win);
//      wxDCOverlay overlaydc(m_overlay, &dc);
//      wxGCDC gdc(dc);
//      "use gdc for drawing"
//      ...
//
// Notice that the opacity of the overlay window is opaque by default, assuming that
// SetOpacity(-1) wasn't called. Call SetOpacity(alpha) at any time to change that.
// E.g.: SetOpacity(128) will make the overlay window semi-transparent.
//
// Notice also that SetOpacity(-1) has no effect if called after the overlay window
// is initialized. Likewise, calling SetOpacity(alpha) has no effect if SetOpacity(-1)
// was effectively called before the overlay window is initialized.
//
//    From MSDN remarks:
//    ------------------
//        Note that once SetLayeredWindowAttributes has been called for a
//    layered window, subsequent UpdateLayeredWindow calls will fail until
//    the layering style bit is cleared and set again.
//

namespace // anonymous
{

inline COLORREF GetTransparentColor()
{
    // Use some unlikely colour as the transparent one. Notably do not use
    // black, as we don't want everything drawn with black on the overlay to be
    // transparent.
    return RGB(0, 1, 2);
}

wxWindow* wxCreateOverlayWindow(const wxRect& rect, int alpha, HWND hWndInsertAfter)
{
    auto overlayWin = new wxWindow();

    overlayWin->MSWCreate
                (
                    overlayWin->GetMSWClassName(0),
                    nullptr, // No title
                    rect.GetPosition(),
                    rect.GetSize(),
                    WS_POPUP,
                    WS_EX_LAYERED |
                    WS_EX_TRANSPARENT |
                    WS_EX_NOACTIVATE
                );

    overlayWin->SetBackgroundStyle(wxBG_STYLE_PAINT);

    if ( alpha >= 0 )
    {
        if ( !::SetLayeredWindowAttributes(GetHwndOf(overlayWin),
                                           GetTransparentColor(),
                                           alpha,
                                           LWA_COLORKEY | LWA_ALPHA) )
        {
            wxLogLastError(wxS("SetLayeredWindowAttributes()"));
        }
    }
    //else: UpdateLayeredWindow will be used to update the overlay window

    // We intentionally don't use WS_VISIBLE when creating this window to avoid
    // stealing activation from the parent, so show it using SWP_NOACTIVATE now.
    ::SetWindowPos(GetHwndOf(overlayWin), hWndInsertAfter, 0, 0, 0, 0,
                   SWP_NOSIZE |
                   SWP_NOMOVE |
                   SWP_NOREDRAW |
                   SWP_NOOWNERZORDER |
                   SWP_NOACTIVATE |
                   SWP_SHOWWINDOW);

    return overlayWin;
}

} // anonymous namespace

class wxOverlayImpl : public wxOverlay::Impl
{
public:
    wxOverlayImpl() = default;
    ~wxOverlayImpl() { Reset(); }

    virtual void Reset() override;
    virtual bool IsOk() override;
    virtual void Init(wxDC* dc, int x , int y , int width , int height) override;
    virtual void BeginDrawing(wxDC* dc) override;
    virtual void EndDrawing(wxDC* dc) override;
    virtual void Clear(wxDC* dc) override;

    // To use per-pixel alpha blending, call this function with -1 before the
    // overlay is initialized. Or with another value to change the opacity of
    // the overlay window if you like a constant opacity.
    virtual void SetOpacity(int alpha) override;

    bool IsUsingConstantOpacity() const { return m_alpha >= 0; }

public:
    // window the overlay is associated with
    wxWindow* m_window = nullptr;
    // the overlay window itself. doesn't have to be child of any window
    // and is totally managed by this class
    wxWindow* m_overlayWindow = nullptr;

    wxRect      m_rect;

    // This variable defines how our overlay window is updated:
    // - If set to -1, UpdateLayeredWindow will be used, which means the overlay
    //   window will use per-pixel alpha blending with the values in the source
    //   bitmap.
    // - If set to a value between (0..255) a constant opacity will be applied
    //   to the entire overlay window. SetLayeredWindowAttributes will be used
    //   to update the overlay window in this case.
    int m_alpha = wxALPHA_OPAQUE;

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

    // The rectangle must be in screen coordinates
    m_rect = m_window->GetClientSize();
    m_window->ClientToScreen(&m_rect.x, &m_rect.y);

    // Because wxClientDC adjusts the origin of the device context to the
    // origin of the client area of the window, we need to compensate for it
    // here, to make sure that (0, 0) of the rectangle corresponds to the point
    // (0, 0) of the window client area.
    m_rect.Offset(-m_window->GetClientAreaOrigin());

    if ( IsUsingConstantOpacity() )
    {
        m_bitmap.CreateWithDIPSize(m_rect.GetSize(), m_window->GetDPIScaleFactor());
    }
    else
    {
        m_bitmap.CreateWithDIPSize(m_rect.GetSize(), m_window->GetDPIScaleFactor(), 32);
        m_bitmap.UseAlpha();
    }

    // We want the overlay window precede the associated window in Z-order, but
    // under any floating window i.e. be positioned between them. And to achieve
    // this, we must insert it after the _previous_ window in the same order, as
    // then it will be just before this one.
    // GW_HWNDPREV: Returns a handle to the window above the given window. (MSDN)
    const auto win = wxGetTopLevelParent(m_window);
    HWND hWndInsertAfter = ::GetNextWindow(GetHwndOf(win), GW_HWNDPREV);
    m_overlayWindow = wxCreateOverlayWindow(m_rect, m_alpha, hWndInsertAfter);
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

    if ( !IsUsingConstantOpacity() )
    {
        POINT ptSrc{0, 0};
        POINT ptDst{m_rect.x, m_rect.y};
        SIZE size{m_rect.width, m_rect.height};
        BLENDFUNCTION blendFunc{AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};

        if ( !::UpdateLayeredWindow(GetHwndOf(m_overlayWindow), nullptr, &ptDst, &size,
                                    GetHdcOf(m_memDC), &ptSrc, 0, &blendFunc, ULW_ALPHA) )
        {
            wxLogLastError(wxS("UpdateLayeredWindow()"));
        }
    }

    m_memDC.SelectObject(wxNullBitmap);

    auto impl = dc->GetImpl();
    auto msw_impl = static_cast<wxMSWDCImpl*>(impl);

    msw_impl->SetHDC(m_hdc); // restore the original hdc
    msw_impl->UpdateClipBox();

    if ( IsUsingConstantOpacity() )
    {
        wxWindowDC winDC(m_overlayWindow);
        winDC.DrawBitmap(m_bitmap, wxPoint(0, 0));
    }
}

void wxOverlayImpl::Clear(wxDC* WXUNUSED(dc))
{
    wxCHECK_RET( IsOk(), wxS("overlay not initialized") );

    m_memDC.SetBackground(wxColour(GetTransparentColor()));
    m_memDC.Clear();
}

void wxOverlayImpl::Reset()
{
    if ( IsOk() )
    {
        m_overlayWindow->Destroy();
        m_overlayWindow = nullptr;

        m_alpha = wxALPHA_OPAQUE;
    }
}

void wxOverlayImpl::SetOpacity(int alpha)
{
#if wxUSE_GRAPHICS_CONTEXT
    if ( !IsOk() )
    {
        m_alpha = wxClip(alpha, -1, 255);
    }
    else if ( IsUsingConstantOpacity() )
#endif // wxUSE_GRAPHICS_CONTEXT
    {
        m_alpha = wxClip(alpha, 0, 255);

        if ( !::SetLayeredWindowAttributes(GetHwndOf(m_overlayWindow),
                                           GetTransparentColor(),
                                           m_alpha,
                                           LWA_COLORKEY | LWA_ALPHA) )
        {
            wxLogLastError(wxS("SetLayeredWindowAttributes()"));
        }
    }
}

wxOverlay::Impl* wxOverlay::Create()
{
    return new wxOverlayImpl;
}
