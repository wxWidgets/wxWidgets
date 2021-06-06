/////////////////////////////////////////////////////////////////////////////
// Name:        wx/gtk/private/overlay.h
// Purpose:     wxOverlayImpl declaration
// Author:      Kettab Ali
// Created:     2021-06-01
// Copyright:   (c) wxWidgets team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GTK_PRIVATE_OVERLAY_H_
#define _WX_GTK_PRIVATE_OVERLAY_H_

#include "wx/gdicmn.h"
#include <cairo.h>


class WXDLLIMPEXP_FWD_CORE wxWindow;
class WXDLLIMPEXP_FWD_CORE wxDC;
class WXDLLIMPEXP_FWD_CORE wxSizeEvent;

class wxOverlayImpl
{
public:
    wxOverlayImpl();
    ~wxOverlayImpl();

    void Reset(bool dispose);
    bool IsOk() const;
    void Init(wxDC* dc, int x , int y , int width , int height);
    void BeginDrawing(wxDC* dc);
    void EndDrawing(wxDC* dc);
    void Clear(wxDC* dc);
    void SetUpdateRectangle(const wxRect& rect);

    // Getters:
    wxWindow* GetWindow() const { return m_window; }

    cairo_surface_t* GetCairoSurface() const { return m_cairoSurface; }

public:
    // m_surface is not automatically resized when m_window get resized.
    // So we need this handler to do it ourselves.
    void OnSize(wxSizeEvent& event);

    // Rectangle covered by the overlay, in m_window's client coordinates
    wxRect m_rect;
    wxRect m_oldRect;
    // Window the overlay is associated with
    wxWindow* m_window;

    // Draw the content of m_cairoSurface on this surface.
    // has always the same size as m_window's client area.
    GtkWidget* m_surface;

    cairo_t*         m_cairoContext;
    cairo_surface_t* m_cairoSurface;

    wxDECLARE_NO_COPY_CLASS(wxOverlayImpl);
};

#endif // _WX_GTK_PRIVATE_OVERLAY_H_
