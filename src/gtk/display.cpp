///////////////////////////////////////////////////////////////////////////
// Name:        src/gtk/display.cpp
// Author:      Paul Cornett
// Created:     2014-04-17
// Copyright:   (c) 2014 Paul Cornett
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_DISPLAY
    #include "wx/display.h"
    #include "wx/display_impl.h"
#endif
#include "wx/utils.h" // wxClientDisplayRect

#include <gtk/gtk.h>
#ifdef GDK_WINDOWING_X11
    #include <gdk/gdkx.h>
#endif
#include "wx/gtk/private/gtk2-compat.h"

#if !defined(GDK_WINDOWING_WIN32)

GdkWindow* wxGetTopLevelGDK();

//-----------------------------------------------------------------------------

#ifndef __WXGTK4__

#ifdef GDK_WINDOWING_X11
void wxGetWorkAreaX11(Screen* screen, int& x, int& y, int& width, int& height);
#endif

#ifndef __WXGTK3__
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
#endif // !__WXGTK3__

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
#ifdef GDK_WINDOWING_X11
#ifdef __WXGTK3__
        if (GDK_IS_X11_SCREEN(screen))
#endif
        {
            GdkRectangle rect = { 0, 0, 0, 0 };
            wxGetWorkAreaX11(GDK_SCREEN_XSCREEN(screen),
                rect.x, rect.y, rect.width, rect.height);
            // in case _NET_WORKAREA result is too large
            if (rect.width && rect.height)
                gdk_rectangle_intersect(dest, &rect, dest);
        }
#endif // GDK_WINDOWING_X11
    }
    wxGCC_WARNING_RESTORE()
}
#define gdk_screen_get_monitor_workarea wx_gdk_screen_get_monitor_workarea

#endif // !__WXGTK4__

void wxClientDisplayRect(int* x, int* y, int* width, int* height)
{
    GdkRectangle rect;
    GdkWindow* window = wxGetTopLevelGDK();
#ifdef __WXGTK4__
    GdkMonitor* monitor =
        gdk_display_get_monitor_at_window(gdk_window_get_display(window), window);
    gdk_monitor_get_workarea(monitor, &rect);
#else
    GdkScreen* screen = gdk_window_get_screen(window);
    wxGCC_WARNING_SUPPRESS(deprecated-declarations)
    int monitor = gdk_screen_get_monitor_at_window(screen, window);
    gdk_screen_get_monitor_workarea(screen, monitor, &rect);
    wxGCC_WARNING_RESTORE()
#endif
    if (x)
        *x = rect.x;
    if (y)
        *y = rect.y;
    if (width)
        *width = rect.width;
    if (height)
        *height = rect.height;
}
//-----------------------------------------------------------------------------

#if wxUSE_DISPLAY
class wxDisplayFactoryGTK: public wxDisplayFactory
{
public:
    virtual wxDisplayImpl* CreateDisplay(unsigned n) wxOVERRIDE;
    virtual unsigned GetCount() wxOVERRIDE;
    virtual int GetFromPoint(const wxPoint& pt) wxOVERRIDE;
};

class wxDisplayImplGTK: public wxDisplayImpl
{
    typedef wxDisplayImpl base_type;
public:
    wxDisplayImplGTK(unsigned i);
    virtual wxRect GetGeometry() const wxOVERRIDE;
    virtual wxRect GetClientArea() const wxOVERRIDE;
    virtual wxString GetName() const wxOVERRIDE;
    virtual bool IsPrimary() const wxOVERRIDE;
    virtual wxArrayVideoModes GetModes(const wxVideoMode& mode) const wxOVERRIDE;
    virtual wxVideoMode GetCurrentMode() const wxOVERRIDE;
    virtual bool ChangeMode(const wxVideoMode& mode) wxOVERRIDE;
#ifdef __WXGTK4__
    GdkMonitor* const m_monitor;
#else
    GdkScreen* const m_screen;
#endif
};
//-----------------------------------------------------------------------------

#ifdef __WXGTK4__
static inline GdkDisplay* GetDisplay()
{
    return gdk_window_get_display(wxGetTopLevelGDK());
}
#else
static inline GdkScreen* GetScreen()
{
    return gdk_window_get_screen(wxGetTopLevelGDK());
}
#endif

wxDisplayImpl* wxDisplayFactoryGTK::CreateDisplay(unsigned n)
{
    return new wxDisplayImplGTK(n);
}

unsigned wxDisplayFactoryGTK::GetCount()
{
#ifdef __WXGTK4__
    return gdk_display_get_n_monitors(GetDisplay());
#else
    wxGCC_WARNING_SUPPRESS(deprecated-declarations)
    return gdk_screen_get_n_monitors(GetScreen());
    wxGCC_WARNING_RESTORE()
#endif
}

int wxDisplayFactoryGTK::GetFromPoint(const wxPoint& pt)
{
    GdkRectangle rect;
#ifdef __WXGTK4__
    GdkDisplay* display = GetDisplay();
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
#else
    GdkScreen* screen = GetScreen();
    wxGCC_WARNING_SUPPRESS(deprecated-declarations)
    int monitor = gdk_screen_get_monitor_at_point(screen, pt.x, pt.y);
    gdk_screen_get_monitor_geometry(screen, monitor, &rect);
    wxGCC_WARNING_RESTORE()
    if (!wxRect(rect.x, rect.y, rect.width, rect.height).Contains(pt))
        monitor = wxNOT_FOUND;
    return monitor;
#endif
}
//-----------------------------------------------------------------------------

wxDisplayImplGTK::wxDisplayImplGTK(unsigned i)
    : base_type(i)
#ifdef __WXGTK4__
    , m_monitor(gdk_display_get_monitor(GetDisplay(), i))
#else
    , m_screen(GetScreen())
#endif
{
}

wxRect wxDisplayImplGTK::GetGeometry() const
{
    GdkRectangle rect;
#ifdef __WXGTK4__
    gdk_monitor_get_geometry(m_monitor, &rect);
#else
    wxGCC_WARNING_SUPPRESS(deprecated-declarations)
    gdk_screen_get_monitor_geometry(m_screen, m_index, &rect);
    wxGCC_WARNING_RESTORE()
#endif
    return wxRect(rect.x, rect.y, rect.width, rect.height);
}

wxRect wxDisplayImplGTK::GetClientArea() const
{
    GdkRectangle rect;
#ifdef __WXGTK4__
    gdk_monitor_get_workarea(m_monitor, &rect);
#else
    wxGCC_WARNING_SUPPRESS(deprecated-declarations)
    gdk_screen_get_monitor_workarea(m_screen, m_index, &rect);
    wxGCC_WARNING_RESTORE()
#endif
    return wxRect(rect.x, rect.y, rect.width, rect.height);
}

wxString wxDisplayImplGTK::GetName() const
{
    return wxString();
}

bool wxDisplayImplGTK::IsPrimary() const
{
#ifdef __WXGTK4__
    return gdk_monitor_is_primary(m_monitor) != 0;
#else
    wxGCC_WARNING_SUPPRESS(deprecated-declarations)
    return gdk_screen_get_primary_monitor(m_screen) == int(m_index);
    wxGCC_WARNING_RESTORE()
#endif
}

#if defined(GDK_WINDOWING_X11) && !defined(__WXGTK4__)
wxArrayVideoModes wxXF86VidMode_GetModes(const wxVideoMode& mode, Display* pDisplay, int nScreen);
wxVideoMode wxXF86VidMode_GetCurrentMode(Display* display, int nScreen);
bool wxXF86VidMode_ChangeMode(const wxVideoMode& mode, Display* display, int nScreen);
wxArrayVideoModes wxX11_GetModes(const wxDisplayImpl* impl, const wxVideoMode& modeMatch, Display* display);
#endif

wxArrayVideoModes wxDisplayImplGTK::GetModes(const wxVideoMode& mode) const
{
    wxArrayVideoModes modes;
#if defined(GDK_WINDOWING_X11) && !defined(__WXGTK4__)
#ifdef __WXGTK3__
    if (GDK_IS_X11_SCREEN(m_screen))
#endif
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
#if defined(GDK_WINDOWING_X11) && defined(HAVE_X11_EXTENSIONS_XF86VMODE_H) && !defined(__WXGTK4__)
#ifdef __WXGTK3__
    if (GDK_IS_X11_SCREEN(m_screen))
#endif
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
#if defined(GDK_WINDOWING_X11) && defined(HAVE_X11_EXTENSIONS_XF86VMODE_H) && !defined(__WXGTK4__)
#ifdef __WXGTK3__
    if (GDK_IS_X11_SCREEN(m_screen))
#endif
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
//-----------------------------------------------------------------------------

wxDisplayFactory* wxDisplay::CreateFactory()
{
    return new wxDisplayFactoryGTK;
}
#endif // wxUSE_DISPLAY

#endif // !defined(GDK_WINDOWING_WIN32)
