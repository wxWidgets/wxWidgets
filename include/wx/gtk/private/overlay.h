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

#ifdef GDK_WINDOWING_X11
class wxOverlayX11Helper;
#endif

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

    void SetShownOnScreen();
    bool IsShownOnScreen() const;

private:
    // create and initialize m_surface
    void CreateSurface(wxOverlay::Target target);

    GtkWidget* GetSurface() const { return m_surface; }

    // The input shape applies only to event handling.
    // i.e. mouse clicks should be passed on to m_window.
    void SetInputShape();

#ifdef GDK_WINDOWING_X11
    friend class wxOverlayX11Helper;
    wxOverlayX11Helper* m_x11Helper;
#else
    void* m_x11Helper; // never used in this case
#endif // GDK_WINDOWING_X11

    // Rectangle covered by the overlay, in m_window's client coordinates
    wxRect m_rect;
    wxRect m_oldRect;

    // Window the overlay is associated with
    wxWindow* m_window;

    // Draw the content of m_cairoSurface on this surface.
    GtkWidget* m_surface;

    cairo_surface_t* m_cairoSurface;

    wxDECLARE_NO_COPY_CLASS(wxOverlayGTKImpl);
};

#ifdef GDK_WINDOWING_X11
#include "wx/timer.h"

// In short, this class helps moving/resizing the overlay window
// when the target window moves/resizes.
//
class wxOverlayX11Helper
{
public:
    wxOverlayX11Helper(wxOverlayGTKImpl* owner);
    ~wxOverlayX11Helper();

    wxWindow* GetWindow() const { return m_owner->GetWindow(); }

    bool IsShownOnScreen() const { return m_isShownOnScreen; }
    void SetShownOnScreen();

private:
    // handlers connected to the app window.
    void OnMove(wxMoveEvent& event);
    void OnSize(wxSizeEvent& event);

    wxOverlayGTKImpl* const m_owner;

    class OverlayTimer : public wxTimer
    {
    public:
        OverlayTimer(wxOverlayX11Helper* helper) : m_helper(helper) { }

        virtual void Notify() wxOVERRIDE;

    private:
        wxOverlayX11Helper* const m_helper;

    } m_timer;

    // indicates that the overlay window is really shown on screen
    // and hence we can blit m_cairoSurface on it.
    bool m_isShownOnScreen;
};
#endif // GDK_WINDOWING_X11

#endif // _WX_GTK_PRIVATE_OVERLAY_H_
