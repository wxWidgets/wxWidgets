/////////////////////////////////////////////////////////////////////////////
// Name:        utils.cpp
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling, Julian Smart and Markus Holzem
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/utils.h"
#include "wx/string.h"

#include "wx/intl.h"
#include "wx/log.h"

#include "wx/process.h"

#include "wx/unix/execute.h"

#include <stdarg.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
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

// ----------------------------------------------------------------------------
// misc.
// ----------------------------------------------------------------------------

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
    return result;       /* TRUE if keyboard hardware supports this mode */
}
#else
bool wxSetDetectableAutoRepeat( bool WXUNUSED(flag) )
{
    return FALSE;
}
#endif

// ----------------------------------------------------------------------------
// display characterstics
// ----------------------------------------------------------------------------

void *wxGetDisplay()
{
    return gdk_display;
}

void wxDisplaySize( int *width, int *height )
{
    if (width) *width = gdk_screen_width();
    if (height) *height = gdk_screen_height();
}

void wxGetMousePosition( int* x, int* y )
{
    gdk_window_get_pointer( (GdkWindow*) NULL, x, y, (GdkModifierType*) NULL );
}

bool wxColourDisplay()
{
    return TRUE;
}

int wxDisplayDepth()
{
    return gdk_window_get_visual( (GdkWindow*) &gdk_root_parent )->depth;
}

int wxGetOsVersion(int *majorVsn, int *minorVsn)
{
  if (majorVsn) *majorVsn = GTK_MAJOR_VERSION;
  if (minorVsn) *minorVsn = GTK_MINOR_VERSION;

  return wxGTK;
}

// ----------------------------------------------------------------------------
// subprocess routines
// ----------------------------------------------------------------------------

static void GTK_EndProcessDetector(gpointer data, gint source,
                                   GdkInputCondition WXUNUSED(condition) )
{
    wxEndProcessData *proc_data = (wxEndProcessData *)data;

    wxHandleProcessTermination(proc_data);

    close(source);
    gdk_input_remove(proc_data->tag);
}

int wxAddProcessCallback(wxEndProcessData *proc_data, int fd)
{
    int tag = gdk_input_add(fd,
                            GDK_INPUT_READ,
                            GTK_EndProcessDetector,
                            (gpointer)proc_data);

    return tag;
}

