/////////////////////////////////////////////////////////////////////////////
// Name:        src/unix/utilsx11.cpp
// Purpose:     Miscellaneous X11 functions
// Author:      Mattia Barbon, Vaclav Slavik, Robert Roebling
// Modified by:
// Created:     25.03.02
// RCS-ID:      $Id$
// Copyright:   (c) wxWindows team
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#if defined(__WXX11__) || defined(__WXGTK__) || defined(__WXMOTIF__)

#include "wx/unix/utilsx11.h"
#include "wx/iconbndl.h"
#include "wx/image.h"
#include "wx/icon.h"
#include "wx/log.h"

#ifdef __VMS
#pragma message disable nosimpint
#endif
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#ifdef __VMS
#pragma message enable nosimpint
#endif

#ifdef __WXGTK20__
#include <gdk/gdk.h>
#include <gdk/gdkx.h>
#endif

// Various X11 Atoms used in this file:
static Atom _NET_WM_ICON = 0;
static Atom _NET_WM_STATE = 0;
static Atom _NET_WM_STATE_FULLSCREEN = 0;
static Atom _NET_WM_STATE_STAYS_ON_TOP = 0;
static Atom _NET_WM_WINDOW_TYPE = 0;
static Atom _NET_WM_WINDOW_TYPE_NORMAL = 0;
static Atom _KDE_NET_WM_WINDOW_TYPE_OVERRIDE = 0;
static Atom _WIN_LAYER = 0;
static Atom KWIN_RUNNING = 0;
#ifndef __WXGTK20__
static Atom _NET_SUPPORTING_WM_CHECK = 0;
static Atom _NET_SUPPORTED = 0;
#endif

#define wxMAKE_ATOM(name, display) \
    if (name == 0) name = XInternAtom((display), #name, False)


// Is the window mapped?
static bool IsMapped(Display *display, Window window)
{
    XWindowAttributes attr;
    XGetWindowAttributes(display, window, &attr);
    return (attr.map_state != IsUnmapped);
}



// Suspends X11 errors. Used when we expect errors but they are not fatal
// for us.
class wxX11ErrorsSuspender
{
public:
    wxX11ErrorsSuspender(Display *d) : m_display(d)
    {
        m_old = XSetErrorHandler(handler);
    }
    ~wxX11ErrorsSuspender()
    {
        XFlush(m_display);
        XSetErrorHandler(m_old);
    }
private:
    Display *m_display;
    int (*m_old)(Display*, XErrorEvent *);
    static int handler(Display *, XErrorEvent *) { return 0; }
};



// ----------------------------------------------------------------------------
// Setting icons for window manager:
// ----------------------------------------------------------------------------

void wxSetIconsX11( WXDisplay* display, WXWindow window,
                    const wxIconBundle& ib )
{
#if !wxUSE_NANOX
    size_t size = 0;
    size_t i, max = ib.m_icons.GetCount();

    for( i = 0; i < max; ++i )
        if( ib.m_icons[i].Ok() )
            size += 2 + ib.m_icons[i].GetWidth() * ib.m_icons[i].GetHeight();

    wxMAKE_ATOM(_NET_WM_ICON, (Display*)display);

    if( size > 0 )
    {
        wxUint32* data = new wxUint32[size];
        wxUint32* ptr = data;

        for( i = 0; i < max; ++i )
        {
            const wxImage image = ib.m_icons[i].ConvertToImage();
            int width = image.GetWidth(), height = image.GetHeight();
            unsigned char* imageData = image.GetData();
            unsigned char* imageDataEnd = imageData + ( width * height * 3 );
            bool hasMask = image.HasMask();
            unsigned char rMask, gMask, bMask;
            unsigned char r, g, b, a;

            if( hasMask )
            {
                rMask = image.GetMaskRed();
                gMask = image.GetMaskGreen();
                bMask = image.GetMaskBlue();
            }
            else // no mask, but still init the variables to avoid warnings
            {
                rMask =
                gMask =
                bMask = 0;
            }

            *ptr++ = width;
            *ptr++ = height;

            while( imageData < imageDataEnd ) {
                r = imageData[0];
                g = imageData[1];
                b = imageData[2];
                if( hasMask && r == rMask && g == gMask && b == bMask )
                    a = 0;
                else
                    a = 255;

                *ptr++ = ( a << 24 ) | ( r << 16 ) | ( g << 8 ) | b;

                imageData += 3;
            }
        }

        XChangeProperty( (Display*)display,
                         (Window)window,
                         _NET_WM_ICON,
                         XA_CARDINAL, 32,
                         PropModeReplace,
                         (unsigned char*)data, size );
        delete[] data;
    }
    else
    {
        XDeleteProperty( (Display*)display,
                         (Window)window,
                         _NET_WM_ICON );
    }
#endif
}


// ----------------------------------------------------------------------------
// Fullscreen mode:
// ----------------------------------------------------------------------------

// NB: Setting fullscreen mode under X11 is a complicated matter. There was
//     no standard way of doing it until recently. ICCCM doesn't know the
//     concept of fullscreen windows and the only way to make a window
//     fullscreen is to remove decorations, resize it to cover entire screen
//     and set WIN_LAYER_ABOVE_DOCK.
//
//     This doesn't always work, though. Specifically, at least kwin from 
//     KDE 3 ignores the hint. The only way to make kwin accept our request
//     is to emulate the way Qt does it. That is, unmap the window, set
//     _NET_WM_WINDOW_TYPE to _KDE_NET_WM_WINDOW_TYPE_OVERRIDE (KDE extension),
//     add _NET_WM_STATE_STAYS_ON_TOP (ditto) to _NET_WM_STATE and map
//     the window again.
//
//     Version 1.2 of Window Manager Specification (aka wm-spec aka
//     Extended Window Manager Hints) introduced _NET_WM_STATE_FULLSCREEN
//     window state which provides cleanest and simplest possible way of
//     making a window fullscreen. WM-spec is a de-facto standard adopted
//     by GNOME and KDE folks, but _NET_WM_STATE_FULLSCREEN isn't yet widely
//     supported. As of January 2003, only GNOME 2's default WM Metacity 
//     implements, KDE will support it from version 3.2. At toolkits level,
//     GTK+ >= 2.1.2 uses it as the only method of making windows fullscreen
//     (that's why wxGTK will *not* switch to using gtk_window_fullscreen
//     unless it has better compatiblity with older WMs).
//
//     
//     This is what wxWindows does in wxSetFullScreenStateX11:
//       1) if _NET_WM_STATE_FULLSCREEN is supported, use it
//       2) otherwise try WM-specific hacks (KDE, IceWM)
//       3) use _WIN_LAYER and hope that the WM will recognize it
//     The code was tested with:
//     twm, IceWM, WindowMaker, Metacity, kwin, sawfish, lesstif-mwm


#define  WIN_LAYER_NORMAL       4
#define  WIN_LAYER_ABOVE_DOCK  10

static void wxWinHintsSetLayer(Display *display, Window rootWnd,
                               Window window, int layer)
{
    wxX11ErrorsSuspender noerrors(display);
    
    XEvent xev;

    wxMAKE_ATOM( _WIN_LAYER, display );

    if (IsMapped(display, window))
    {
        xev.type = ClientMessage;
        xev.xclient.type = ClientMessage;
        xev.xclient.window = window;
        xev.xclient.message_type = _WIN_LAYER;
        xev.xclient.format = 32;
        xev.xclient.data.l[0] = (long)layer;
        xev.xclient.data.l[1] = CurrentTime;

        XSendEvent(display, rootWnd, False,
                   SubstructureNotifyMask, (XEvent*) &xev);
    }
    else
    {
        long data[1];

        data[0] = layer;
        XChangeProperty(display, window,
                        _WIN_LAYER, XA_CARDINAL, 32,
                        PropModeReplace, (unsigned char *)data, 1);
    }
}



#ifdef __WXGTK20__
static bool wxQueryWMspecSupport(Display* WXUNUSED(display),
                                 Window WXUNUSED(rootWnd),
                                 Atom (feature))
{
    GdkAtom gatom = gdk_x11_xatom_to_atom(feature);
    return gdk_net_wm_supports(gatom);
}
#else
static bool wxQueryWMspecSupport(Display *display, Window rootWnd, Atom feature)
{
    wxMAKE_ATOM(_NET_SUPPORTING_WM_CHECK, display);
    wxMAKE_ATOM(_NET_SUPPORTED, display);
      
    // FIXME: We may want to cache these checks. Note that we can't simply
    //        remember the results in global variable because the WM may go
    //        away and be replaced by another one! One possible approach
    //        would be invalidate the case every 15 seconds or so. Since this
    //        code is currently only used by wxTopLevelWindow::ShowFullScreen,
    //        it is not important that it is not optimized.
    //
    //        If the WM supports ICCCM (i.e. the root window has
    //        _NET_SUPPORTING_WM_CHECK property that points to a WM-owned
    //        window), we could watch for DestroyNotify event on the window
    //        and invalidate our cache when the windows goes away (= WM
    //        is replaced by another one). This is what GTK+ 2 does.
    //        Let's do it only if it is needed, it requires changes to
    //        the event loop.
  
    Atom type;
    Window *wins;
    Atom *atoms;
    int format;
    unsigned long after;
    unsigned long nwins, natoms;
    
    // Is the WM ICCCM supporting?
    XGetWindowProperty(display, rootWnd,
                       _NET_SUPPORTING_WM_CHECK, 0, UINT_MAX,
                       False, XA_WINDOW, &type, &format, &nwins,
                       &after, (unsigned char **)&wins);
    if ( type != XA_WINDOW || nwins <= 0 || wins[0] == None )
       return FALSE;
    XFree(wins);

    // Query for supported features:
    XGetWindowProperty(display, rootWnd,
                       _NET_SUPPORTED, 0, UINT_MAX,
                       False, XA_ATOM, &type, &format, &natoms,
                       &after, (unsigned char **)&atoms);
    if ( type != XA_ATOM || atoms == NULL )
        return FALSE;

    // Lookup the feature we want:
    for (unsigned i = 0; i < natoms; i++)
    {
        if ( atoms[i] == feature )
        {
            XFree(atoms);
            return TRUE;
        }
    }
    XFree(atoms);
    return FALSE;
}
#endif


#define _NET_WM_STATE_REMOVE        0
#define _NET_WM_STATE_ADD           1

static void wxWMspecSetState(Display *display, Window rootWnd,
                             Window window, int operation, Atom state)
{
    wxMAKE_ATOM(_NET_WM_STATE, display);
  
    if ( IsMapped(display, window) )
    {
        XEvent xev;
        xev.type = ClientMessage;
        xev.xclient.type = ClientMessage;
        xev.xclient.serial = 0;
        xev.xclient.send_event = True;
        xev.xclient.display = display;
        xev.xclient.window = window;
        xev.xclient.message_type = _NET_WM_STATE;
        xev.xclient.format = 32;
        xev.xclient.data.l[0] = operation;
        xev.xclient.data.l[1] = state;
        xev.xclient.data.l[2] = None;
  
        XSendEvent(display, rootWnd,
                   False,
                   SubstructureRedirectMask | SubstructureNotifyMask,
                   &xev);
    }
    // FIXME - must modify _NET_WM_STATE property list if the window
    //         wasn't mapped!
}

static void wxWMspecSetFullscreen(Display *display, Window rootWnd,
                                  Window window, bool fullscreen)
{
    wxMAKE_ATOM(_NET_WM_STATE_FULLSCREEN, display);
    wxWMspecSetState(display, rootWnd,
                     window,
                     fullscreen ? _NET_WM_STATE_ADD : _NET_WM_STATE_REMOVE,
                      _NET_WM_STATE_FULLSCREEN);
}


// Is the user running KDE's kwin window manager? At least kwin from KDE 3
// sets KWIN_RUNNING property on the root window.
static bool wxKwinRunning(Display *display, Window rootWnd)
{
    wxMAKE_ATOM(KWIN_RUNNING, display);
    
    long *data;
    Atom type;
    int format;
    unsigned long nitems, after;
    if (XGetWindowProperty(display, rootWnd,
                           KWIN_RUNNING, 0, 1, False, KWIN_RUNNING,
                           &type, &format, &nitems, &after,
                           (unsigned char**)&data) != Success)
    {
        return FALSE;
    }

    bool retval = (type == KWIN_RUNNING &&
                   nitems == 1 && data && data[0] == 1);
    XFree(data);
    return retval;
}

// KDE's kwin is Qt-centric so much than no normal method of fullscreen 
// mode will work with it. We have to carefully emulate the Qt way.
static void wxSetKDEFullscreen(Display *display, Window rootWnd,
                               Window w, bool fullscreen, wxRect *origRect)
{
    long data[2];
    unsigned lng;

    wxMAKE_ATOM(_NET_WM_WINDOW_TYPE, display);
    wxMAKE_ATOM(_NET_WM_WINDOW_TYPE_NORMAL, display);
    wxMAKE_ATOM(_KDE_NET_WM_WINDOW_TYPE_OVERRIDE, display);
    wxMAKE_ATOM(_NET_WM_STATE_STAYS_ON_TOP, display);

    if (fullscreen)
    {
        data[0] = _KDE_NET_WM_WINDOW_TYPE_OVERRIDE;
        data[1] = _NET_WM_WINDOW_TYPE_NORMAL;
        lng = 2;
    }
    else
    {
        data[0] = _NET_WM_WINDOW_TYPE_NORMAL;
        data[1] = None;
        lng = 1;
    }

    // it is neccessary to unmap the window, otherwise kwin will ignore us:
    XSync(display, False);
    
    bool wasMapped = IsMapped(display, w);
    if (wasMapped)
    {
        XUnmapWindow(display, w);
        XSync(display, False);
    }

    XChangeProperty(display, w, _NET_WM_WINDOW_TYPE, XA_ATOM, 32,
	                PropModeReplace, (unsigned char *) &data, lng);
    XSync(display, False);

    if (wasMapped)
    {
        XMapRaised(display, w);
        XSync(display, False);
    }
    
    wxWMspecSetState(display, rootWnd, w, 
                     fullscreen ? _NET_WM_STATE_ADD : _NET_WM_STATE_REMOVE,
                     _NET_WM_STATE_STAYS_ON_TOP);
    XSync(display, False);

    if (!fullscreen)
    {
        // NB: like many other WMs, kwin ignores first request for window
        //     position change after the window was mapped. This additional
        //     move+resize event will ensure that the window is restored in
        //     exactly same position as before it was made fullscreen (because
        //     wxTopLevelWindow::ShowFullScreen will call SetSize, thus
        //     setting the position for second time).
        XMoveResizeWindow(display, w,
                          origRect->x, origRect->y,
                          origRect->width, origRect->height);
        XSync(display, False);
    }
}


wxX11FullScreenMethod wxGetFullScreenMethodX11(WXDisplay* display,
                                               WXWindow rootWindow)
{
    Window root = (Window)rootWindow;
    Display *disp = (Display*)display;

    // if WM supports _NET_WM_STATE_FULLSCREEN from wm-spec 1.2, use it:
    wxMAKE_ATOM(_NET_WM_STATE_FULLSCREEN, disp);
    if (wxQueryWMspecSupport(disp, root, _NET_WM_STATE_FULLSCREEN))
    {
        wxLogTrace(_T("fullscreen"),
                   _T("detected _NET_WM_STATE_FULLSCREEN support"));
        return wxX11_FS_WMSPEC;
    }
    
    // if the user is running KDE's kwin WM, use a legacy hack because
    // kwin doesn't understand any other method:
    if (wxKwinRunning(disp, root))
    {
        wxLogTrace(_T("fullscreen"), _T("detected kwin"));
        return wxX11_FS_KDE;
    }
    
    // finally, fall back to ICCCM heuristic method:
    wxLogTrace(_T("fullscreen"), _T("unknown WM, using _WIN_LAYER"));
    return wxX11_FS_GENERIC;
}


void wxSetFullScreenStateX11(WXDisplay* display, WXWindow rootWindow,
                             WXWindow window, bool show,
                             wxRect *origRect,
                             wxX11FullScreenMethod method)
{
    // NB: please see the comment under "Fullscreen mode:" title above
    //     for implications of changing this code.

    Window wnd = (Window)window;
    Window root = (Window)rootWindow;
    Display *disp = (Display*)display;

    if (method == wxX11_FS_AUTODETECT)
        method = wxGetFullScreenMethodX11(display, rootWindow);

    switch (method)
    {
        case wxX11_FS_WMSPEC:
            wxWMspecSetFullscreen(disp, root, wnd, show);
            break;
        case wxX11_FS_KDE:
            wxSetKDEFullscreen(disp, root, wnd, show, origRect);
            break;
        default:
            wxWinHintsSetLayer(disp, root, wnd,
                               show ? WIN_LAYER_ABOVE_DOCK : WIN_LAYER_NORMAL);
            break;
    }
}

#endif

