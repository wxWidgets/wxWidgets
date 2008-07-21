/////////////////////////////////////////////////////////////////////////////
// Name:        wx/dfb/private/overlay.h
// Purpose:     wxOverlayImpl declaration
// Author:      Vaclav Slavik
// Created:     2006-10-20
// RCS-ID:      $Id$
// Copyright:   (c) wxWidgets team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_DFB_PRIVATE_OVERLAY_H_
#define _WX_DFB_PRIVATE_OVERLAY_H_

#include "wx/gdicmn.h"
#include "wx/dfb/dfbptr.h"

wxDFB_DECLARE_INTERFACE(IDirectFBSurface);

class WXDLLEXPORT wxWindow;


class wxOverlayImpl
{
public:
    wxOverlayImpl();
    ~wxOverlayImpl();

    void Reset();
    bool IsOk();
    void Init(wxWindowDC* dc, int x , int y , int width , int height);
    void BeginDrawing(wxWindowDC* dc);
    void EndDrawing(wxWindowDC* dc);
    void Clear(wxWindowDC* dc);

    // wxDFB specific methods:
    bool IsEmpty() const { return m_isEmpty; }
    wxRect GetRect() const { return m_rect; }
    wxIDirectFBSurfacePtr GetDirectFBSurface() const { return m_surface; }

public:
    // window the overlay is associated with
    wxWindow *m_window;
    // rectangle covered by the overlay, in m_window's window coordinates
    wxRect m_rect;
    // surface of the overlay, same size as m_rect
    wxIDirectFBSurfacePtr m_surface;
    // this flag is set to true if nothing was drawn on the overlay (either
    // initially or Clear() was called)
    bool m_isEmpty;
};

#endif // _WX_DFB_PRIVATE_OVERLAY_H_
