/////////////////////////////////////////////////////////////////////////////
// Name:        wx/gtk/private/overlay.h
// Purpose:     wxOverlayGTKImpl declaration
// Author:      Kettab Ali
// Created:     2021-06-01
// Copyright:   (c) wxWidgets team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GTK_PRIVATE_OVERLAY_H_
#define _WX_GTK_PRIVATE_OVERLAY_H_

#include "wx/private/overlay.h"
#include <cairo.h>


class WXDLLIMPEXP_FWD_CORE wxWindow;
class WXDLLIMPEXP_FWD_CORE wxDC;

class wxOverlayGTKImpl : public wxOverlayImpl
{
public:
    wxOverlayGTKImpl();
    ~wxOverlayGTKImpl();

    virtual bool IsGenericSupported() const wxOVERRIDE;

    virtual void Reset() wxOVERRIDE;
    virtual bool IsOk() wxOVERRIDE;
    virtual void InitFromDC(wxDC* dc, int x , int y , int width , int height) wxOVERRIDE;
    virtual void InitFromWindow(wxWindow* win, wxOverlay::Target target) wxOVERRIDE;
    virtual void BeginDrawing(wxDC* dc) wxOVERRIDE;
    virtual void EndDrawing(wxDC* dc) wxOVERRIDE;
    virtual void Clear(wxDC* dc) wxOVERRIDE;

    virtual void SetUpdateRectangle(const wxRect& rect) wxOVERRIDE;

    // Getters:
    wxWindow* GetWindow() const { return m_window; }

    cairo_surface_t* GetCairoSurface() const { return m_cairoSurface; }

private:
    // create and initialize m_surface
    void CreateSurface();

    // The input shape applies only to event handling.
    // i.e. mouse clicks should be passed on to m_window.
    void SetInputShape();

    // Rectangle covered by the overlay, in m_window's client coordinates
    wxRect m_rect;
    wxRect m_oldRect;

    // Window the overlay is associated with
    wxWindow* m_window;

    // Draw the content of m_cairoSurface on this surface.
    // has always the same size as m_window's client area.
    GtkWidget* m_surface;

    cairo_surface_t* m_cairoSurface;

    wxDECLARE_NO_COPY_CLASS(wxOverlayGTKImpl);
};

#endif // _WX_GTK_PRIVATE_OVERLAY_H_
