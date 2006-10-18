/////////////////////////////////////////////////////////////////////////////
// Name:        src/gtk/utilsgtk.cpp
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
#include "gdk/gdkx.h"

#ifdef HAVE_X11_XKBLIB_H
    /* under HP-UX and Solaris 2.6, at least, XKBlib.h defines structures with
     * field named "explicit" - which is, of course, an error for a C++
     * compiler. To be on the safe side, just redefine it everywhere. */
    #define explicit __wx_explicit

    #include "X11/XKBlib.h"

    #undef explicit
#endif // HAVE_X11_XKBLIB_H


#if wxUSE_DETECT_SM
    #include "X11/Xlib.h"
    #include "X11/SM/SMlib.h"
#endif

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

// Escapes string so that it is valid Pango markup XML string:
wxString wxEscapeStringForPangoMarkup(const wxString& str)
{
    size_t len = str.length();
    wxString out;
    out.Alloc(len);
    for (size_t i = 0; i < len; i++)
    {
        wxChar c = str[i];
        switch (c)
        {
            case _T('&'):
                out << _T("&amp;");
                break;
            case _T('<'):
                out << _T("&lt;");
                break;
            case _T('>'):
                out << _T("&gt;");
                break;
            case _T('\''):
                out << _T("&apos;");
                break;
            case _T('"'):
                out << _T("&quot;");
                break;
            default:
                out << c;
                break;
        }
    }
    return out;
}


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
    return gdk_drawable_get_visual( wxGetRootWindow()->window )->depth;
}

wxWindow* wxFindWindowAtPoint(const wxPoint& pt)
{
    return wxGenericFindWindowAtPoint(pt);
}

#if !wxUSE_UNICODE

wxCharBuffer wxConvertToGTK(const wxString& s, wxFontEncoding enc)
{
    wxCharBuffer buf;
    if ( enc == wxFONTENCODING_UTF8 )
    {
        // no need for conversion at all, but do check that we have a valid
        // UTF-8 string because passing invalid UTF-8 to GTK+ is going to
        // result in a GTK+ error message and, especially, loss of data which
        // was supposed to be shown in the GUI
        if ( wxConvUTF8.ToWChar(NULL, 0, s, s.length()) == wxCONV_FAILED )
        {
            // warn the programmer that something is probably wrong in his code
            //
            // NB: don't include the string in output because chances are that
            //     this invalid UTF-8 string could result in more errors itself
            //     if the application shows logs in the GUI and so we get into
            //     an infinite loop
            wxLogDebug(_T("Invalid UTF-8 string in wxConvertToGTK()"));

            // but still try to show at least something on the screen
            wxMBConvUTF8 utf8permissive(wxMBConvUTF8::MAP_INVALID_UTF8_TO_OCTAL);
            wxWCharBuffer wbuf(utf8permissive.cMB2WC(s));
            buf = wxConvUTF8.cWC2MB(wbuf);
        }
        else // valid UTF-8 string, no need to convert
        {
            buf = wxCharBuffer(s);
        }
    }
    else // !UTF-8
    {
        wxWCharBuffer wbuf;
        if ( enc == wxFONTENCODING_SYSTEM || enc == wxFONTENCODING_DEFAULT )
        {
            wbuf = wxConvUI->cMB2WC(s);
        }
        else // another encoding, use generic conversion class
        {
            wbuf = wxCSConv(enc).cMB2WC(s);
        }

        if ( wbuf )
            buf = wxConvUTF8.cWC2MB(wbuf);
    }

    return buf;
}

#endif // !wxUSE_UNICODE

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

#if wxUSE_DETECT_SM
static wxString GetSM()
{
	Display     *dpy;
	SmcConn     smc_conn;
	char        *vendor;
	char        *client_id_ret;
	dpy = XOpenDisplay(NULL);

	smc_conn = SmcOpenConnection(NULL, NULL,
	                             999, 999,
	                             0 /* mask */, NULL /* callbacks */,
	                             NULL, &client_id_ret, 0, NULL);

	vendor = SmcVendor(smc_conn);
    wxString ret = wxString::FromAscii( vendor );
	free(vendor);

	SmcCloseConnection(smc_conn, 0, NULL);
	free(client_id_ret);

	XCloseDisplay(dpy);
    
    return ret;
}
#endif

wxString wxGUIAppTraits::GetDesktopEnvironment() const
{
#if wxUSE_DETECT_SM
    wxString SM = GetSM();
    
    if (SM == wxT("GnomeSM"))
        return wxT("GNOME");
        
    if (SM == wxT("KDE"))
        return wxT("KDE");
#endif

    return wxEmptyString;
}



