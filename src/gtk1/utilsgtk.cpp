/////////////////////////////////////////////////////////////////////////////
// Name:        src/gtk1/utilsgtk.cpp
// Purpose:
// Author:      Robert Roebling
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/utils.h"

#ifndef WX_PRECOMP
    #include "wx/string.h"
    #include "wx/intl.h"
    #include "wx/log.h"
#endif

#include "wx/apptrait.h"
#include "wx/gtk1/private/timer.h"
#include "wx/evtloop.h"
#include "wx/process.h"

#include <stdarg.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>   // for WNOHANG
#include <unistd.h>

#include "glib.h"
#include "gdk/gdk.h"
#include "gtk/gtk.h"
#include "gtk/gtkfeatures.h"
#include "gdk/gdkx.h"

#ifdef HAVE_X11_XKBLIB_H
    /* under HP-UX and Solaris 2.6, at least, XKBlib.h defines structures with
     * field named "explicit" - which is, of course, an error for a C++
     * compiler. To be on the safe side, just redefine it everywhere. */
    #define explicit __wx_explicit

    #include "X11/XKBlib.h"

    #undef explicit
#endif // HAVE_X11_XKBLIB_H

//-----------------------------------------------------------------------------
// data
//-----------------------------------------------------------------------------

extern GtkWidget *wxGetRootWindow();

//----------------------------------------------------------------------------
// misc.
//----------------------------------------------------------------------------

void wxBell()
{
    gdk_beep();
}

/* Don't synthesize KeyUp events holding down a key and producing
   KeyDown events with autorepeat. */
#ifdef HAVE_X11_XKBLIB_H
bool wxSetDetectableAutoRepeat( bool flag )
{
    Bool result;
    XkbSetDetectableAutoRepeat( GDK_DISPLAY(), flag, &result );
    return result;       /* true if keyboard hardware supports this mode */
}
#else
bool wxSetDetectableAutoRepeat( bool WXUNUSED(flag) )
{
    return false;
}
#endif

// ----------------------------------------------------------------------------
// display characterstics
// ----------------------------------------------------------------------------

void *wxGetDisplay()
{
    return GDK_DISPLAY();
}

void wxGetMousePosition( int* x, int* y )
{
    gdk_window_get_pointer( NULL, x, y, NULL );
}

wxWindow* wxFindWindowAtPoint(const wxPoint& pt)
{
    return wxGenericFindWindowAtPoint(pt);
}


// ----------------------------------------------------------------------------
// subprocess routines
// ----------------------------------------------------------------------------

#if wxUSE_TIMER

wxTimerImpl* wxGUIAppTraits::CreateTimerImpl(wxTimer *timer)
{
    return new wxGTKTimerImpl(timer);
}

#endif // wxUSE_TIMER

// ----------------------------------------------------------------------------
// wxPlatformInfo-related
// ----------------------------------------------------------------------------

wxPortId wxGUIAppTraits::GetToolkitVersion(int *verMaj,
                                           int *verMin,
                                           int *verMicro) const
{
    if ( verMaj )
        *verMaj = gtk_major_version;
    if ( verMin )
        *verMin = gtk_minor_version;
    if ( verMicro )
        *verMicro = gtk_micro_version;

    return wxPORT_GTK;
}

wxEventLoopBase* wxGUIAppTraits::CreateEventLoop()
{
    return new wxEventLoop;
}

