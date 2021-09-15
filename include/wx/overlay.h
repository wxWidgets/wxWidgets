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
#else
    // don't define wxHAS_NATIVE_OVERLAY
#endif

#ifdef wxOVERLAY_NO_EXTERNAL_DC
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

    // use generic implementation even if the native one is available
    void UseGeneric();

    // returns true if we are using a native implementation
    bool IsNative() const;

    // clears the overlay without restoring the former state
    // to be done eg when the window content has been changed and repainted
    void Reset();

    // returns (port-specific) implementation of the overlay
    wxOverlayImpl *GetImpl() { return m_impl; }

private:
    friend class WXDLLIMPEXP_FWD_CORE wxDCOverlay;

    // returns true if it has been setup
    bool IsOk();

    void Init(wxDC* dc, int x , int y , int width , int height);

    void Init(wxWindow* win, bool fullscreen);

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
    wxDCOverlay(wxOverlay &overlay, wxWindow *win, bool fullscreen = false);

    // removes the connection between the overlay and the dc
    virtual ~wxDCOverlay();

    // implicit conversion to wxDC
    operator wxDC& () const
    {
        wxASSERT_MSG( m_dc, "Invalid device context" );
        return *m_dc;
    }

    // clears the layer, restoring the state at the last init
    void Clear();

    // sets the rectangle to be refreshed/updated within the overlay.
    void SetUpdateRectangle(const wxRect& rect);

private:
    void Init(wxDC *dc);
    void Init(wxDC *dc, int x , int y , int width , int height);
    void Init(wxWindow *win, bool fullscreen, const wxRect& rect);

    wxOverlay& m_overlay;

    wxDC*      m_dc;
    bool       m_ownsDC;

#ifdef wxOVERLAY_NO_EXTERNAL_DC
    wxMemoryDC m_memDC;
#endif // wxOVERLAY_NO_EXTERNAL_DC

    wxDECLARE_NO_COPY_CLASS(wxDCOverlay);
};

#endif // _WX_OVERLAY_H_
