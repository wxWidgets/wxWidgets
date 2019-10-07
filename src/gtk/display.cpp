///////////////////////////////////////////////////////////////////////////
// Name:        src/gtk/display.cpp
// Author:      Paul Cornett
// Created:     2014-04-17
// Copyright:   (c) 2014 Paul Cornett
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#include "wx/private/display.h"
#if wxUSE_DISPLAY
    #include "wx/window.h"
#endif

#include "wx/gtk/private/wrapgtk.h"
#ifdef GDK_WINDOWING_X11
    #ifndef __WXGTK4__
        #include "wx/unix/private/displayx11.h"

        #define wxGTK_HAVE_X11_DISPLAY
    #endif

    #include <gdk/gdkx.h>
#endif

// This file is not used at all when using Win32.
#if !defined(GDK_WINDOWING_WIN32)

GdkWindow* wxGetTopLevelGDK();

// There are 2 quite different implementations here: one for GTK+ 4 and the
// other one for the previous versions.

#ifdef __WXGTK4__

static inline GdkDisplay* GetDisplay()
{
    return gdk_window_get_display(wxGetTopLevelGDK());
}

// This class is always defined as it's used for the main display even when
// wxUSE_DISPLAY == 0.
class wxDisplayImplGTK : public wxDisplayImpl
{
    typedef wxDisplayImpl base_type;
public:
    wxDisplayImplGTK(unsigned i);
    virtual wxRect GetGeometry() const wxOVERRIDE;
    virtual wxRect GetClientArea() const wxOVERRIDE;
    virtual int GetDepth() const wxOVERRIDE;
    virtual double GetScaleFactor() const wxOVERRIDE;
    virtual wxSize GetSizeMM() const wxOVERRIDE;

#if wxUSE_DISPLAY
    virtual bool IsPrimary() const wxOVERRIDE;
    virtual wxArrayVideoModes GetModes(const wxVideoMode& mode) const wxOVERRIDE;
    virtual wxVideoMode GetCurrentMode() const wxOVERRIDE;
    virtual bool ChangeMode(const wxVideoMode& mode) wxOVERRIDE;
#endif // wxUSE_DISPLAY

    GdkMonitor* const m_monitor;
};

// This class is only defined when we're built with full display support.
#if wxUSE_DISPLAY
class wxDisplayFactoryGTK: public wxDisplayFactory
{
public:
    virtual wxDisplayImpl* CreateDisplay(unsigned n) wxOVERRIDE;
    virtual unsigned GetCount() wxOVERRIDE;
    virtual int GetFromPoint(const wxPoint& pt) wxOVERRIDE;
    virtual int GetFromWindow(const wxWindow* win) wxOVERRIDE;
};

wxDisplayImpl* wxDisplayFactoryGTK::CreateDisplay(unsigned n)
{
    return new wxDisplayImplGTK(n);
}

unsigned wxDisplayFactoryGTK::GetCount()
{
    return gdk_display_get_n_monitors(::GetDisplay());
}

int wxDisplayFactoryGTK::GetFromPoint(const wxPoint& pt)
{
    GdkRectangle rect;
    GdkDisplay* display = ::GetDisplay();
    GdkMonitor* monitor = gdk_display_get_monitor_at_point(display, pt.x, pt.y);
    gdk_monitor_get_geometry(monitor, &rect);
    if (wxRect(rect.x, rect.y, rect.width, rect.height).Contains(pt))
    {
        for (unsigned i = gdk_display_get_n_monitors(display); i--;)
        {
            if (gdk_display_get_monitor(display, i) == monitor)
                return i;
        }
    }
    return wxNOT_FOUND;
}

int wxDisplayFactoryGTK::GetFromWindow(const wxWindow* win)
{
    if (win && win->m_widget)
    {
        GdkDisplay* display = gtk_widget_get_display(win->m_widget);
        GdkMonitor* monitor;
        if (GdkWindow* window = gtk_widget_get_window(win->m_widget))
            monitor = gdk_display_get_monitor_at_window(display, window);
        else
            monitor = gdk_display_get_primary_monitor(display);

        for (unsigned i = gdk_display_get_n_monitors(display); i--;)
        {
            if (gdk_display_get_monitor(display, i) == monitor)
                return i;
        }
    }
    return wxNOT_FOUND;
}
#endif // wxUSE_DISPLAY

wxDisplayImplGTK::wxDisplayImplGTK(unsigned i)
    : base_type(i)
    , m_monitor(gdk_display_get_monitor(GetDisplay(), i))
{
}

wxRect wxDisplayImplGTK::GetGeometry() const
{
    GdkRectangle rect;
    gdk_monitor_get_geometry(m_monitor, &rect);
    return wxRect(rect.x, rect.y, rect.width, rect.height);
}

wxRect wxDisplayImplGTK::GetClientArea() const
{
    GdkRectangle rect;
    gdk_monitor_get_workarea(m_monitor, &rect);
    return wxRect(rect.x, rect.y, rect.width, rect.height);
}

int wxDisplayImplGTK::GetDepth() const
{
    return 24;
}

double wxDisplayImplGTK::GetScaleFactor() const
{
    return gdk_monitor_get_scale_factor(m_monitor);
}

wxSize wxDisplayImplGTK::GetSizeMM() const
{
    return wxSize
           (
                gdk_monitor_get_width_mm(m_monitor),
                gdk_monitor_get_height_mm(m_monitor)
           );
}

#if wxUSE_DISPLAY
bool wxDisplayImplGTK::IsPrimary() const
{
    return gdk_monitor_is_primary(m_monitor) != 0;
}

wxArrayVideoModes
wxDisplayImplGTK::GetModes(const wxVideoMode& WXUNUSED(mode)) const
{
    return wxArrayVideoModes();
}

wxVideoMode wxDisplayImplGTK::GetCurrentMode() const
{
    return wxVideoMode();
}

bool wxDisplayImplGTK::ChangeMode(const wxVideoMode& WXUNUSED(mode))
{
    return false;
}
#endif // wxUSE_DISPLAY

#else // !__WXGTK4__

#ifdef __WXGTK3__

static inline bool wxIsX11GDKScreen(GdkScreen* screen)
{
    return GDK_IS_X11_SCREEN(screen);
}

#else // !__WXGTK3__

static inline int wx_gdk_screen_get_primary_monitor(GdkScreen* screen)
{
    int monitor = 0;
#if GTK_CHECK_VERSION(2,20,0)
    if (wx_is_at_least_gtk2(20))
        monitor = gdk_screen_get_primary_monitor(screen);
#endif
    return monitor;
}
#define gdk_screen_get_primary_monitor wx_gdk_screen_get_primary_monitor

static inline bool wxIsX11GDKScreen(GdkScreen* WXUNUSED(screen))
{
    return true;
}

#endif // __WXGTK3__/!__WXGTK3__

static inline void
wx_gdk_screen_get_monitor_workarea(GdkScreen* screen, int monitor, GdkRectangle* dest)
{
    wxGCC_WARNING_SUPPRESS(deprecated-declarations)
#if GTK_CHECK_VERSION(3,4,0)
    if (gtk_check_version(3,4,0) == NULL)
        gdk_screen_get_monitor_workarea(screen, monitor, dest);
    else
#endif
    {
        gdk_screen_get_monitor_geometry(screen, monitor, dest);
#ifdef wxGTK_HAVE_X11_DISPLAY
        if ( wxIsX11GDKScreen(screen) )
        {
            GdkRectangle rect = { 0, 0, 0, 0 };
            wxGetWorkAreaX11(GDK_SCREEN_XSCREEN(screen),
                rect.x, rect.y, rect.width, rect.height);
            // in case _NET_WORKAREA result is too large
            if (rect.width && rect.height)
                gdk_rectangle_intersect(dest, &rect, dest);
        }
#endif // wxGTK_HAVE_X11_DISPLAY
    }
    wxGCC_WARNING_RESTORE()
}
#define gdk_screen_get_monitor_workarea wx_gdk_screen_get_monitor_workarea

static inline GdkScreen* GetScreen()
{
    return gdk_window_get_screen(wxGetTopLevelGDK());
}

class wxDisplayImplGTK : public wxDisplayImpl
{
    typedef wxDisplayImpl base_type;
public:
    wxDisplayImplGTK(unsigned i);
    virtual wxRect GetGeometry() const wxOVERRIDE;
    virtual wxRect GetClientArea() const wxOVERRIDE;
    virtual int GetDepth() const wxOVERRIDE;
#if GTK_CHECK_VERSION(3,10,0)
    virtual double GetScaleFactor() const wxOVERRIDE;
#endif // GTK+ 3.10
    virtual wxSize GetPPI() const wxOVERRIDE;
    virtual wxSize GetSizeMM() const wxOVERRIDE;

#if wxUSE_DISPLAY
    virtual bool IsPrimary() const wxOVERRIDE;
    virtual wxArrayVideoModes GetModes(const wxVideoMode& mode) const wxOVERRIDE;
    virtual wxVideoMode GetCurrentMode() const wxOVERRIDE;
    virtual bool ChangeMode(const wxVideoMode& mode) wxOVERRIDE;
#endif // wxUSE_DISPLAY

    GdkScreen* const m_screen;
};

#if wxUSE_DISPLAY
class wxDisplayFactoryGTK: public wxDisplayFactory
{
public:
    virtual wxDisplayImpl* CreateDisplay(unsigned n) wxOVERRIDE;
    virtual unsigned GetCount() wxOVERRIDE;
    virtual int GetFromPoint(const wxPoint& pt) wxOVERRIDE;
    virtual int GetFromWindow(const wxWindow* win) wxOVERRIDE;
};

wxGCC_WARNING_SUPPRESS(deprecated-declarations)

wxDisplayImpl* wxDisplayFactoryGTK::CreateDisplay(unsigned n)
{
    return new wxDisplayImplGTK(n);
}

unsigned wxDisplayFactoryGTK::GetCount()
{
    return gdk_screen_get_n_monitors(GetScreen());
}

int wxDisplayFactoryGTK::GetFromPoint(const wxPoint& pt)
{
    GdkRectangle rect;
    GdkScreen* screen = GetScreen();
    int monitor = gdk_screen_get_monitor_at_point(screen, pt.x, pt.y);
    gdk_screen_get_monitor_geometry(screen, monitor, &rect);
    if (!wxRect(rect.x, rect.y, rect.width, rect.height).Contains(pt))
        monitor = wxNOT_FOUND;
    return monitor;
}

int wxDisplayFactoryGTK::GetFromWindow(const wxWindow* win)
{
    int monitor = wxNOT_FOUND;
    if (win && win->m_widget)
    {
        GdkScreen* screen = gtk_widget_get_screen(win->m_widget);
        if (GdkWindow* window = gtk_widget_get_window(win->m_widget))
            monitor = gdk_screen_get_monitor_at_window(screen, window);
        else
            monitor = gdk_screen_get_primary_monitor(screen);
    }
    return monitor;
}
#endif // wxUSE_DISPLAY

wxDisplayImplGTK::wxDisplayImplGTK(unsigned i)
    : base_type(i)
    , m_screen(GetScreen())
{
}

wxRect wxDisplayImplGTK::GetGeometry() const
{
    GdkRectangle rect;
    gdk_screen_get_monitor_geometry(m_screen, m_index, &rect);
    return wxRect(rect.x, rect.y, rect.width, rect.height);
}

wxRect wxDisplayImplGTK::GetClientArea() const
{
    GdkRectangle rect;
    gdk_screen_get_monitor_workarea(m_screen, m_index, &rect);
    return wxRect(rect.x, rect.y, rect.width, rect.height);
}

int wxDisplayImplGTK::GetDepth() const
{
    // TODO: How to get the depth of the specific display?
    return gdk_visual_get_depth(gdk_window_get_visual(wxGetTopLevelGDK()));
}

#if GTK_CHECK_VERSION(3,10,0)
double wxDisplayImplGTK::GetScaleFactor() const
{
    if ( gtk_check_version(3,10,0) == NULL )
        return gdk_screen_get_monitor_scale_factor(m_screen, m_index);

    return 1.0;
}
#endif // GTK+ 3.10

wxSize wxDisplayImplGTK::GetPPI() const
{
    // Try the base class version which uses our GetSizeMM() and returns
    // per-display PPI value if it works.
    wxSize ppi = wxDisplayImpl::GetPPI();

    if ( !ppi.x || !ppi.y )
    {
        // But if it didn't work, fall back to the global DPI value common to
        // all displays -- this is still better than nothing and more
        // compatible with the previous wxWidgets versions.
        ppi = ComputePPI(gdk_screen_width(), gdk_screen_height(),
                         gdk_screen_width_mm(), gdk_screen_height_mm());
    }

    return ppi;
}

wxSize wxDisplayImplGTK::GetSizeMM() const
{
    wxSize sizeMM;
#if GTK_CHECK_VERSION(2,14,0)
    if ( wx_is_at_least_gtk2(14) )
    {
        // Take care not to return (-1, -1) from here, the caller expects us to
        // return (0, 0) if we can't retrieve this information.
        int rc = gdk_screen_get_monitor_width_mm(m_screen, m_index);
        if ( rc != -1 )
            sizeMM.x = rc;

        rc = gdk_screen_get_monitor_height_mm(m_screen, m_index);
        if ( rc != -1 )
            sizeMM.y = rc;
    }
#endif // GTK+ 2.14

    // When we have only a single display, we can use global GTK+ functions.
    // Note that at least in some configurations, these functions return valid
    // values when gdk_screen_get_monitor_xxx_mm() only return -1, so it's
    // always worth fallng back on them, but we can't do it when using
    // multiple displays because they combine the sizes of all displays in this
    // case, which would result in a completely wrong value for GetPPI().
    if ( !(sizeMM.x && sizeMM.y) && gdk_screen_get_n_monitors(m_screen) == 1 )
    {
        sizeMM.x = gdk_screen_width_mm();
        sizeMM.y = gdk_screen_height_mm();
    }

    return sizeMM;
}

#if wxUSE_DISPLAY
bool wxDisplayImplGTK::IsPrimary() const
{
    return gdk_screen_get_primary_monitor(m_screen) == int(m_index);
}

wxArrayVideoModes wxDisplayImplGTK::GetModes(const wxVideoMode& mode) const
{
    wxArrayVideoModes modes;
#ifdef wxGTK_HAVE_X11_DISPLAY
    if ( wxIsX11GDKScreen(m_screen) )
    {
        Display* display = GDK_DISPLAY_XDISPLAY(gdk_screen_get_display(m_screen));
#ifdef HAVE_X11_EXTENSIONS_XF86VMODE_H
        int nScreen = gdk_x11_screen_get_screen_number(m_screen);
        modes = wxXF86VidMode_GetModes(mode, display, nScreen);
#else
        modes = wxX11_GetModes(this, mode, display);
#endif
    }
#else
    wxUnusedVar(mode);
#endif
    return modes;
}

wxVideoMode wxDisplayImplGTK::GetCurrentMode() const
{
    wxVideoMode mode;
#if defined(wxGTK_HAVE_X11_DISPLAY) && defined(HAVE_X11_EXTENSIONS_XF86VMODE_H)
    if ( wxIsX11GDKScreen(m_screen) )
    {
        Display* display = GDK_DISPLAY_XDISPLAY(gdk_screen_get_display(m_screen));
        int nScreen = gdk_x11_screen_get_screen_number(m_screen);
        mode = wxXF86VidMode_GetCurrentMode(display, nScreen);
    }
#endif
    return mode;
}

bool wxDisplayImplGTK::ChangeMode(const wxVideoMode& mode)
{
    bool success = false;
#if defined(wxGTK_HAVE_X11_DISPLAY) && defined(HAVE_X11_EXTENSIONS_XF86VMODE_H)
    if ( wxIsX11GDKScreen(m_screen) )
    {
        Display* display = GDK_DISPLAY_XDISPLAY(gdk_screen_get_display(m_screen));
        int nScreen = gdk_x11_screen_get_screen_number(m_screen);
        success = wxXF86VidMode_ChangeMode(mode, display, nScreen);
    }
#else
    wxUnusedVar(mode);
#endif
    return success;
}

wxGCC_WARNING_RESTORE()

#endif // wxUSE_DISPLAY

#endif // __WXGTK4__/!__WXGTK4__

#if wxUSE_DISPLAY

wxDisplayFactory* wxDisplay::CreateFactory()
{
    return new wxDisplayFactoryGTK;
}

#else // !wxUSE_DISPLAY

class wxDisplayFactorySingleGTK : public wxDisplayFactorySingle
{
protected:
    virtual wxDisplayImpl *CreateSingleDisplay()
    {
        return new wxDisplayImplGTK(0);
    }
};

wxDisplayFactory* wxDisplay::CreateFactory()
{
    return new wxDisplayFactorySingleGTK;
}

#endif // wxUSE_DISPLAY/!wxUSE_DISPLAY

#endif // !defined(GDK_WINDOWING_WIN32)
