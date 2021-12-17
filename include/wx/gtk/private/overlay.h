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
#include <gdk/gdkx.h>

// In short, this class helps moving/resizing the overlay window
// when the target window moves/resizes.
//
// We need to handle things at X11 level because under some systems (e.g. KDE)
// the wxMoveEvent event is only caught when the user stops moving the window.
// as a consequence, the overlay window will appear disassociated from the window
// it overlays which is an unpleasant user experience.
class wxOverlayX11Helper
{
public:
    wxOverlayX11Helper(wxOverlayGTKImpl* owner);
    ~wxOverlayX11Helper();

    Window XGetAppWindow() const;

    bool IsShownOnScreen() const { return m_isShownOnScreen; }
    void SetShownOnScreen();

    void ResetIsShownOnScreen() { m_isShownOnScreen = false; }

    void Thaw();
    void Freeze();

    bool IsFrozen() const { return m_isFrozen; }

private:
    // handlers connected to the app window.
    void OnMove(wxMoveEvent& event);
    void OnSize(wxSizeEvent& event);
    void OnEnter(wxMouseEvent& event);
    void OnLeave(wxMouseEvent& event);

    wxOverlayGTKImpl* const m_owner;

    class wxOverlayTimer : public wxTimer
    {
    public:
        wxOverlayTimer(wxOverlayX11Helper* helper) : m_helper(helper) { }

        virtual void Notify() wxOVERRIDE
        {
            m_helper->Thaw();
        }

    private:
        wxOverlayX11Helper* const m_helper;

    } m_timer;

    // indicates that the overlay window is really shown on screen
    // and hence we can blit m_cairoSurface on it.
    bool m_isShownOnScreen;

    // true if/when the appwin starts moving/resizing, false otherwise.
    // the overlay window cannot be shown while this is true.
    bool m_isFrozen;

    // remember the mouse pointer state (i.e. inside or outside the appwin)
    //
    // the trick is to unset the current transient window of the overlay if
    // the pointer is outside the appwin, which will result in a ConfigureNotify
    // event being sent (due to windows restack operation) if the user starts
    // moving or resizing the appwin.
    bool m_isPointerInsideWindow;
};
#endif // GDK_WINDOWING_X11

#endif // _WX_GTK_PRIVATE_OVERLAY_H_
