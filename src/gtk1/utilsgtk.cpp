/////////////////////////////////////////////////////////////////////////////
// Name:        src/gtk1/utilsgtk.cpp
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
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

#include "wx/process.h"

#include "wx/unix/execute.h"

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
#ifndef __EMX__
// on OS/2, we use the wxBell from wxBase library

void wxBell()
{
    gdk_beep();
}
#endif

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

void wxDisplaySize( int *width, int *height )
{
    if (width) *width = gdk_screen_width();
    if (height) *height = gdk_screen_height();
}

void wxDisplaySizeMM( int *width, int *height )
{
    if (width) *width = gdk_screen_width_mm();
    if (height) *height = gdk_screen_height_mm();
}

void wxClientDisplayRect(int *x, int *y, int *width, int *height)
{
    // This is supposed to return desktop dimensions minus any window
    // manager panels, menus, taskbars, etc.  If there is a way to do that
    // for this platform please fix this function, otherwise it defaults
    // to the entire desktop.
    if (x) *x = 0;
    if (y) *y = 0;
    wxDisplaySize(width, height);
}

void wxGetMousePosition( int* x, int* y )
{
    gdk_window_get_pointer( (GdkWindow*) NULL, x, y, (GdkModifierType*) NULL );
}

bool wxColourDisplay()
{
    return true;
}

int wxDisplayDepth()
{
    return gdk_window_get_visual( wxGetRootWindow()->window )->depth;
}

wxWindow* wxFindWindowAtPoint(const wxPoint& pt)
{
    return wxGenericFindWindowAtPoint(pt);
}


// ----------------------------------------------------------------------------
// subprocess routines
// ----------------------------------------------------------------------------

extern "C" {
static
void GTK_EndProcessDetector(gpointer data, gint source,
                            GdkInputCondition WXUNUSED(condition) )
{
   wxEndProcessData *proc_data = (wxEndProcessData *)data;

   // has the process really terminated? unfortunately GDK (or GLib) seem to
   // generate G_IO_HUP notification even when it simply tries to read from a
   // closed fd and hasn't terminated at all
   int pid = (proc_data->pid > 0) ? proc_data->pid : -(proc_data->pid);
   int status = 0;
   int rc = waitpid(pid, &status, WNOHANG);

   if ( rc == 0 )
   {
       // no, it didn't exit yet, continue waiting
       return;
   }

   // set exit code to -1 if something bad happened
   proc_data->exitcode = rc != -1 && WIFEXITED(status) ? WEXITSTATUS(status)
                                                      : -1;

   // child exited, end waiting
   close(source);

   // don't call us again!
   gdk_input_remove(proc_data->tag);

   wxHandleProcessTermination(proc_data);
}
}

int wxAddProcessCallback(wxEndProcessData *proc_data, int fd)
{
    int tag = gdk_input_add(fd,
                            GDK_INPUT_READ,
                            GTK_EndProcessDetector,
                            (gpointer)proc_data);

    return tag;
}

// ----------------------------------------------------------------------------
// wxPlatformInfo-related
// ----------------------------------------------------------------------------

wxPortId wxGUIAppTraits::GetToolkitVersion(int *verMaj, int *verMin) const
{
    if ( verMaj )
        *verMaj = gtk_major_version;
    if ( verMin )
        *verMin = gtk_minor_version;

    return wxPORT_GTK;
}

wxString wxGUIAppTraits::GetDesktopEnvironment() const
{
    return wxEmptyString;
}

#if wxUSE_INTL
void wxGUIAppTraits::SetLocale()
{
    gtk_set_locale();
}
#endif

