/////////////////////////////////////////////////////////////////////////////
// Name:        wx/overlay.h
// Purpose:     wxOverlay class
// Author:      Stefan Csomor
// Modified by:
// Created:     2006-10-20
// Copyright:   (c) wxWidgets team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_OVERLAY_H_
#define _WX_OVERLAY_H_

#include "wx/defs.h"

#if defined(__WXDFB__)
    #define wxHAS_NATIVE_OVERLAY 1
#elif defined(__WXOSX__) && wxOSX_USE_COCOA
    #define wxHAS_NATIVE_OVERLAY 1
#elif defined(__WXMSW__) || defined(__WXGTK3__)
    #define wxHAS_NATIVE_OVERLAY 1
    #define wxOVERLAY_USE_INTERNAL_BUFFER 1
#else
    // don't define wxHAS_NATIVE_OVERLAY
#endif

// Under MSW and GTK3 we can create and use a wxDCOverlay (like a regular wxDC)
// independently of any external wxDC. i.e. a wxMemoryDC/wxGCDC are used for the
// actual drawings and the result is blitted to the destination (overlay) window
// eventually.
// For the other platforms, they can define this symbol after overriding the base
// class wxOverlayImpl::InitFromWindow() function.
#ifdef wxOVERLAY_USE_INTERNAL_BUFFER
    #include "wx/dcmemory.h"
#endif

// ----------------------------------------------------------------------------
// creates an overlay over an existing window, allowing for manipulations like
// rubberbanding etc. This API is not stable yet, not to be used outside wx
// internal code
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_FWD_CORE wxOverlayImpl;
class WXDLLIMPEXP_FWD_CORE wxDC;
class WXDLLIMPEXP_FWD_CORE wxWindow;
class WXDLLIMPEXP_FWD_CORE wxRect;

class WXDLLIMPEXP_CORE wxOverlay
{
public:
    wxOverlay();
    ~wxOverlay();

    // clears the overlay without restoring the former state
    // to be done eg when the window content has been changed and repainted
    void Reset();

    // We need to informe/hint wxOverlay to set up a mechanism to detect the
    // geometry change of the overlaid window, because under wxMSW and wxGTK3
    // (X11) there is no direct/implicit connection between the overlay and the
    // wxWindow it overlays when the latter get moved or resized. so this function
    // is only needed for a specific kind of overlays (a manual reset overlays)
    // which by convention are overlays that need a different action to be reset
    // other than the mouse-up event (e.g. wxCaret overlay, wxGrid selection overlay).
    void SetIsManualReset();

    // implementation only
    // -------------------

    // returns (port-specific) implementation of the overlay
    wxOverlayImpl *GetImpl() { return m_impl; }

    // use generic implementation even if the native one is available
    void UseGeneric();

    // returns true if we are using a native implementation
    bool IsNative() const;

    enum Target
    {
        Overlay_Window,
        Overlay_Screen
    };

private:
    friend class WXDLLIMPEXP_FWD_CORE wxDCOverlay;

    // returns true if it has been setup
    bool IsOk();

    void InitFromDC(wxDC* dc, int x , int y , int width , int height);

    void InitFromWindow(wxWindow* win, Target target);

    void BeginDrawing(wxDC* dc);

    void EndDrawing(wxDC* dc);

    void Clear(wxDC* dc);

    wxOverlayImpl* m_impl;

    bool m_inDrawing;


    wxDECLARE_NO_COPY_CLASS(wxOverlay);
};


class WXDLLIMPEXP_CORE wxDCOverlay
{
public:
    // connects this overlay to the corresponding drawing dc, if the overlay is
    // not initialized yet this call will do so
    wxDCOverlay(wxOverlay &overlay, wxDC *dc, int x , int y , int width , int height);

    // convenience wrapper that behaves the same using the entire area of the dc
    wxDCOverlay(wxOverlay &overlay, wxDC *dc);

    // alternatively, if these ctors are used instead, the drawing should be done
    // via this class directly (just cast the object to wxDC, i.e.:  wxDC& dc = overlaydc)
    wxDCOverlay(wxOverlay &overlay, wxWindow *win, int x, int y, int width, int height);
    wxDCOverlay(wxOverlay &overlay, wxWindow *win,
                wxOverlay::Target target = wxOverlay::Overlay_Window);

    // removes the connection between the overlay and the dc
    virtual ~wxDCOverlay();

    // implicit conversion to wxDC
    operator wxDC& () const { return *m_dc; }

    // clears the layer, restoring the state at the last init
    void Clear();

    // sets the rectangle to be refreshed/updated within the overlay.
    void SetUpdateRectangle(const wxRect& rect);

private:
    void InitFromDC(wxDC *dc);
    void InitFromDC(wxDC *dc, int x , int y , int width , int height);
    void InitFromWindow(wxWindow *win, wxOverlay::Target target, const wxRect& rect);

    wxOverlay& m_overlay;

    wxDC*      m_dc;
    bool       m_ownsDC;

#ifdef wxOVERLAY_USE_INTERNAL_BUFFER
    wxMemoryDC m_memDC;
#endif // wxOVERLAY_USE_INTERNAL_BUFFER

    wxDECLARE_NO_COPY_CLASS(wxDCOverlay);
};

#endif // _WX_OVERLAY_H_
