/////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/dcclient.cpp
// Purpose:     wxClientDC class
// Author:      Julian Smart
// Created:     01/02/97
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ===========================================================================
// declarations
// ===========================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#include "wx/dcclient.h"
#include "wx/msw/dcclient.h"

#ifndef WX_PRECOMP
    #include "wx/string.h"
    #include "wx/log.h"
    #include "wx/window.h"
#endif

#include "wx/stack.h"

#include "wx/msw/private.h"
#include "wx/msw/private/paint.h"

#if defined(__WXWINUI__) && wxUSE_WINUI3
#include "wx/winui/private/tlwhostmsw.h"
#include <uxtheme.h>
#ifdef WXWINUI_TEST_SUPPORT
#include "opaque-paint-test-access.h"
#endif
#endif

#include <unordered_map>

// ----------------------------------------------------------------------------
// local data structures
// ----------------------------------------------------------------------------

// This is a base class for two concrete subclasses below and contains HDC
// cached for the duration of the WM_PAINT processing together with some
// bookkeeping information.
class wxPaintDCInfo
{
public:
    wxPaintDCInfo(HDC hdc)
        : m_hdc(hdc)
    {
    }

    // The derived class must perform some cleanup.
    virtual ~wxPaintDCInfo() = 0;

    WXHDC GetHDC() const { return (WXHDC)m_hdc; }

protected:
#if defined(__WXWINUI__) && wxUSE_WINUI3
    HDC m_hdc;
#else
    const HDC m_hdc;
#endif

    wxDECLARE_NO_COPY_CLASS(wxPaintDCInfo);
};

namespace
{

#if defined(__WXWINUI__) && wxUSE_WINUI3
// UxTheme's buffered-paint cache belongs to the calling thread, not the app.
// Acquire it lazily, without a process-global UI-thread assumption.
struct wxWinUIBufferedPaintThread
{
    wxWinUIBufferedPaintThread() : ready(SUCCEEDED(::BufferedPaintInit())) {}
    ~wxWinUIBufferedPaintThread() { if ( ready ) ::BufferedPaintUnInit(); }
    const bool ready;
};
#ifdef WXWINUI_TEST_SUPPORT
wxWinUIOpaquePaintTestAccess::Observer gs_opaquePaintObserver = nullptr;
void* gs_opaquePaintObserverContext = nullptr;
#endif
#endif

// This subclass contains information for the HDCs we create ourselves, i.e.
// those for which we call BeginPaint() -- and hence need to call EndPaint().
class wxPaintDCInfoOur : public wxPaintDCInfo
{
public:
    wxPaintDCInfoOur(wxWindow* win)
        : wxPaintDCInfo(::BeginPaint(GetHwndOf(win), GetPaintStructPtr(m_ps))),
          m_hwnd(GetHwndOf(win))
    {
#if defined(__WXWINUI__) && wxUSE_WINUI3
        if ( m_hdc && wxWinUIShouldUseOpaquePaintBuffer(win) )
            BeginOpaqueBuffer();
#endif
    }

    virtual ~wxPaintDCInfoOur()
    {
#if defined(__WXWINUI__) && wxUSE_WINUI3
        EndOpaqueBuffer();
#endif
        ::EndPaint(m_hwnd, &m_ps);
    }

private:
    // This helper is only needed in order to call it from the ctor initializer
    // list.
    static PAINTSTRUCT* GetPaintStructPtr(PAINTSTRUCT& ps)
    {
        wxZeroMemory(ps);
        return &ps;
    }

    const HWND m_hwnd;
    PAINTSTRUCT m_ps;

#if defined(__WXWINUI__) && wxUSE_WINUI3
    void BeginOpaqueBuffer()
    {
        RECT clip;
        const int clipType = ::GetClipBox(m_hdc, &clip);
        if ( clipType == ERROR || clipType == NULLREGION ||
             ::IsRectEmpty(&m_ps.rcPaint) )
            return;

        static thread_local wxWinUIBufferedPaintThread thread;
        if ( !thread.ready ) return;

        const DWORD layout = ::GetLayout(m_hdc);
        POINT targetOrigin{};
        if ( layout == GDI_ERROR || !::LPtoDP(m_hdc, &targetOrigin, 1) )
            return;
        m_physicalPaint = m_ps.rcPaint;
        if ( layout & LAYOUT_RTL )
        {
            // BeginPaint's update rectangle follows the mirrored window's
            // client coordinates. Buffer pixels still run left to right.
            m_physicalPaint.left = targetOrigin.x + 1 - m_ps.rcPaint.right;
            m_physicalPaint.right = targetOrigin.x + 1 - m_ps.rcPaint.left;
        }
        BP_PAINTPARAMS params{};
        params.cbSize = sizeof(params);
        params.dwFlags = BPPF_ERASE;
        params.pBlendFunction = &m_blend;
        HDC bufferDC = nullptr;
        HPAINTBUFFER buffer = ::BeginBufferedPaint(
            m_hdc, &m_ps.rcPaint, BPBF_TOPDOWNDIB, &params, &bufferDC);
        if ( !buffer || !bufferDC )
        {
            if ( buffer ) ::EndBufferedPaint(buffer, FALSE);
            return;
        }

        // Seed and transport pixels in physical coordinates. The cached bitmap
        // can be larger than this update, so mirroring must not use its width.
        const RECT& rect = m_ps.rcPaint;
        if ( !::BitBlt(bufferDC, rect.left, rect.top,
                       rect.right - rect.left, rect.bottom - rect.top,
                       m_hdc, rect.left, rect.top, SRCCOPY) ||
             FAILED(::BufferedPaintSetAlpha(buffer, nullptr, 0)) )
        {
            ::EndBufferedPaint(buffer, FALSE);
            return;
        }

        const int transportState = ::SaveDC(bufferDC);
        POINT bufferOrigin{};
        if ( !transportState || ::SetLayout(bufferDC, layout) == GDI_ERROR ||
             !::LPtoDP(bufferDC, &bufferOrigin, 1) ||
             !::OffsetViewportOrgEx(bufferDC,
                 (layout & LAYOUT_RTL ? -1 : 1) *
                    (targetOrigin.x - m_physicalPaint.left - bufferOrigin.x),
                 targetOrigin.y - m_physicalPaint.top - bufferOrigin.y, nullptr) )
        {
            ::EndBufferedPaint(buffer, FALSE);
            return;
        }

        // The copied pixels seed handlers that only repaint part of their
        // update region, but must not make an otherwise empty paint opaque.
        if ( !::SetBoundsRect(bufferDC, nullptr, DCB_ENABLE | DCB_RESET) )
        {
            ::EndBufferedPaint(buffer, FALSE);
            return;
        }
        m_buffer = buffer;
        m_transportState = transportState;
        m_hdc = bufferDC;
    }

    void EndOpaqueBuffer()
    {
        if ( !m_buffer ) return;

        // GDI batches writes to DIBs. Finish them before UxTheme reads/modifies
        // alpha bytes, otherwise a later batch can zero alpha again.
        ::GdiFlush();
        RECT drawn{};
        const UINT bounds = ::GetBoundsRect(m_hdc, &drawn, 0);
        bool update = (bounds & DCB_SET) == DCB_SET;
        POINT points[2] = {{drawn.left, drawn.top}, {drawn.right, drawn.bottom}};
        // Bounds are in the final wxDC logical coordinates and may be inverted
        // for RTL. UxTheme consumes physical target coordinates, so normalize
        // only after converting to buffer pixels (whose origin is rcPaint).
        if ( update && !::LPtoDP(m_hdc, points, 2) ) update = false;
        if ( !::RestoreDC(m_hdc, m_transportState) ) update = false;
        if ( update )
        {
            drawn = {m_physicalPaint.left + wxMin(points[0].x, points[1].x),
                     m_physicalPaint.top + wxMin(points[0].y, points[1].y),
                     m_physicalPaint.left + wxMax(points[0].x, points[1].x),
                     m_physicalPaint.top + wxMax(points[0].y, points[1].y)};
            RECT clipped;
            update = ::IntersectRect(&clipped, &drawn, &m_physicalPaint) != FALSE;
            if ( update )
            {
                // AlphaBlend expects premultiplied pixels. Work in physical
                // bitmap coordinates: UxTheme's rectangle-based alpha helpers
                // inherit the target's RTL mapping, unlike these top-down bits.
                // Clear unused pixels to RGBA zero and preserve all painted RGB,
                // including black, while making the painted bounds opaque.
                const RECT& target = m_physicalPaint;
                RGBQUAD* bits = nullptr;
                int stride = 0;
                const bool prepared =
                    SUCCEEDED(::GetBufferedPaintBits(m_buffer, &bits, &stride)) &&
                    bits && stride >= target.right - target.left;
                if ( prepared )
                {
                    for ( LONG y = 0; y < target.bottom - target.top; ++y )
                    {
                        RGBQUAD* row = bits + static_cast<size_t>(y) * stride;
                        for ( LONG x = 0; x < target.right - target.left; ++x )
                        {
                            if ( x >= clipped.left - target.left &&
                                 x < clipped.right - target.left &&
                                 y >= clipped.top - target.top &&
                                 y < clipped.bottom - target.top )
                                row[x].rgbReserved = 255;
                            else
                                row[x] = {};
                        }
                    }
                }
                else
                {
                    // Preserve the pre-existing direct GDI result if the
                    // compositor-specific alpha operation is unavailable.
                    ::BitBlt(m_ps.hdc, clipped.left, clipped.top,
                             clipped.right - clipped.left, clipped.bottom - clipped.top,
                             m_hdc, clipped.left, clipped.top, SRCCOPY);
                    update = false;
                }
#ifdef WXWINUI_TEST_SUPPORT
                if ( prepared && gs_opaquePaintObserver )
                {
                    gs_opaquePaintObserver(
                        (WXHWND)m_hwnd, (WXHDC)m_hdc, m_buffer,
                        wxRect(clipped.left, clipped.top,
                               clipped.right - clipped.left, clipped.bottom - clipped.top),
                        gs_opaquePaintObserverContext);
                }
#endif
            }
        }
        // Source alpha remains zero outside the actual drawing bounds. An
        // early-returning paint handler therefore leaves its target intact.
        ::EndBufferedPaint(m_buffer, update ? TRUE : FALSE);
    }

    HPAINTBUFFER m_buffer = nullptr;
    int m_transportState = 0;
    RECT m_physicalPaint{};
    BLENDFUNCTION m_blend{AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
#endif

    wxDECLARE_NO_COPY_CLASS(wxPaintDCInfoOur);
};

// The global map containing HDC to use for the given window. The entries in
// this map only exist during WM_PAINT processing and are destroyed when it is
// over.
//
// It is needed because in some circumstances it can happen that more than one
// wxPaintDC is created for the same window during its WM_PAINT handling (and
// as this can happen implicitly, e.g. by calling a function in some library,
// this can be quite difficult to find) but we need to reuse the same HDC for
// all of them because we can't call BeginPaint() more than once. So we cache
// the first HDC created for the window in this map and then reuse it later if
// needed. And, of course, remove it from the map when the painting is done.
using PaintDCInfos = std::unordered_map<wxWindow*, wxPaintDCInfo*>;

PaintDCInfos gs_PaintDCInfos;

} // anonymous namespace

#if defined(__WXWINUI__) && wxUSE_WINUI3 && defined(WXWINUI_TEST_SUPPORT)
void wxWinUIOpaquePaintTestAccess::SetObserver(Observer observer, void* context)
{
    gs_opaquePaintObserver = observer;
    gs_opaquePaintObserverContext = context;
}
#endif

// ===========================================================================
// implementation
// ===========================================================================

// ----------------------------------------------------------------------------
// wxMSWWindowDCImpl
// ----------------------------------------------------------------------------

wxIMPLEMENT_ABSTRACT_CLASS(wxWindowDCImpl, wxMSWDCImpl);

wxWindowDCImpl::wxWindowDCImpl( wxDC *owner ) :
   wxMSWDCImpl( owner )
{
}

wxWindowDCImpl::wxWindowDCImpl( wxDC *owner, wxWindow *window ) :
   wxMSWDCImpl( owner )
{
    wxCHECK_RET( window, wxT("invalid window in wxWindowDCImpl") );

    InitWindow(window);
    m_hDC = (WXHDC) ::GetWindowDC(GetHwndOf(m_window));

    // m_bOwnsDC was already set to false in the base class ctor, so the DC
    // will be released (and not deleted) in ~wxDC
    InitDC();
}

void wxWindowDCImpl::InitDC()
{
    // the background mode is only used for text background and is set in
    // DrawText() to OPAQUE as required, otherwise always TRANSPARENT,
    ::SetBkMode(GetHdc(), TRANSPARENT);

    // since we are a window dc we need to grab the palette from the window
#if wxUSE_PALETTE
    InitializePalette();
#endif
}

void wxWindowDCImpl::DoGetSize(int *width, int *height) const
{
    wxCHECK_RET( m_window, wxT("wxWindowDCImpl without a window?") );

    m_window->GetSize(width, height);
}

// ----------------------------------------------------------------------------
// wxClientDCImpl
// ----------------------------------------------------------------------------

wxIMPLEMENT_ABSTRACT_CLASS(wxClientDCImpl, wxWindowDCImpl);

wxClientDCImpl::wxClientDCImpl( wxDC *owner ) :
   wxWindowDCImpl( owner )
{
}

wxClientDCImpl::wxClientDCImpl( wxDC *owner, wxWindow *window ) :
   wxWindowDCImpl( owner )
{
    wxCHECK_RET( window, wxT("invalid window in wxClientDCImpl") );

    InitWindow(window);
    m_hDC = (WXHDC)::GetDC(GetHwndOf(window));

    // m_bOwnsDC was already set to false in the base class ctor, so the DC
    // will be released (and not deleted) in ~wxDC

    InitDC();
}

void wxClientDCImpl::InitDC()
{
    wxWindowDCImpl::InitDC();

    // Account for the origin of the client area which is non-zero only for
    // TLWs with (left or top) toolbar: we shouldn't draw over the toolbar.
    wxPoint ptOrigin = m_window->GetClientAreaOrigin();
    if ( ptOrigin.x || ptOrigin.y )
    {
        // no need to shift DC origin if shift is null
        SetDeviceOrigin(ptOrigin.x, ptOrigin.y);
    }

    // in wxUniv build we must manually do some DC adjustments usually
    // performed by Windows for us
#if defined(__WXUNIVERSAL__)
    // clip the DC to avoid overwriting the non client area
    wxSize size = m_window->GetClientSize();
    DoSetClippingRegion(0, 0, size.x, size.y);
#endif // __WXUNIVERSAL__
}

wxClientDCImpl::~wxClientDCImpl()
{
}

void wxClientDCImpl::DoGetSize(int *width, int *height) const
{
    wxCHECK_RET( m_window, wxT("wxClientDCImpl without a window?") );

    m_window->GetClientSize(width, height);
}

// ----------------------------------------------------------------------------
// wxPaintDCImpl
// ----------------------------------------------------------------------------

wxIMPLEMENT_ABSTRACT_CLASS(wxPaintDCImpl, wxClientDCImpl);

wxPaintDCImpl::wxPaintDCImpl( wxDC *owner ) :
   wxClientDCImpl( owner )
{
}

wxPaintDCImpl::wxPaintDCImpl( wxDC *owner, wxWindow *window ) :
   wxClientDCImpl( owner )
{
    wxCHECK_RET( window, wxT("null canvas in wxPaintDCImpl ctor") );

    using namespace wxMSWImpl;
    wxCHECK_RET( !paintStack.empty(),
                 "wxPaintDC can't be created outside wxEVT_PAINT handler" );
    wxCHECK_RET( paintStack.top().window == window,
                 "wxPaintDC must be associated with the window being repainted" );

    paintStack.top().createdPaintDC = true;

    InitWindow(window);

    // do we have a DC for this window in the cache?
    m_hDC = FindDCInCache(m_window);
    if ( !m_hDC )
    {
        // not in cache, create a new one
        wxPaintDCInfoOur* const info = new wxPaintDCInfoOur(m_window);
        gs_PaintDCInfos[m_window] = info;
        m_hDC = info->GetHDC();
    }

    // Note: at this point m_hDC can be null under MicroWindows, when dragging.
    if (!GetHDC())
        return;

    // (re)set the DC parameters.
    InitDC();
}

wxPaintDCImpl::~wxPaintDCImpl()
{
    if ( m_hDC )
    {
        SelectOldObjects(m_hDC);
        m_hDC = 0;
    }
}


/* static */
wxPaintDCInfo *wxPaintDCImpl::FindInCache(wxWindow *win)
{
    PaintDCInfos::const_iterator it = gs_PaintDCInfos.find( win );

    return it != gs_PaintDCInfos.end() ? it->second : nullptr;
}

/* static */
WXHDC wxPaintDCImpl::FindDCInCache(wxWindow* win)
{
    wxPaintDCInfo* const info = FindInCache(win);

    return info ? info->GetHDC() : 0;
}

/* static */
void wxPaintDCImpl::EndPaint(wxWindow *win)
{
    wxPaintDCInfo *info = FindInCache(win);
    if ( info )
    {
        gs_PaintDCInfos.erase(win);
        delete info;
    }
}

wxPaintDCInfo::~wxPaintDCInfo()
{
}

/*
 * wxPaintDCEx
 */

class wxPaintDCExImpl: public wxPaintDCImpl
{
public:
    wxPaintDCExImpl( wxDC *owner, wxWindow *window, WXHDC dc );
    ~wxPaintDCExImpl();
};


wxIMPLEMENT_ABSTRACT_CLASS(wxPaintDCEx, wxPaintDC);

wxPaintDCEx::wxPaintDCEx(wxWindow *window, WXHDC dc)
           : wxPaintDC(new wxPaintDCExImpl(this, window, dc))
{
}

wxPaintDCExImpl::wxPaintDCExImpl(wxDC *owner, wxWindow *window, WXHDC dc)
               : wxPaintDCImpl( owner )
{
    wxCHECK_RET( dc, wxT("wxPaintDCEx requires an existing device context") );

    InitWindow(window);
    m_hDC = dc;
}

wxPaintDCExImpl::~wxPaintDCExImpl()
{
    // prevent the base class dtor from ReleaseDC()ing it again
    m_hDC = 0;
}
