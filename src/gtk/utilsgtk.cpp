/////////////////////////////////////////////////////////////////////////////
// Name:        src/gtk/utilsgtk.cpp
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
#include "wx/process.h"
#include "wx/sysopt.h"
#include "wx/vector.h"

#include "wx/gtk/private/timer.h"
#include "wx/evtloop.h"

#include "wx/gtk/private/wrapgtk.h"
#ifdef GDK_WINDOWING_WIN32
#include <gdk/gdkwin32.h>
#endif
#ifdef GDK_WINDOWING_X11
#include <gdk/gdkx.h>
#endif

#if wxDEBUG_LEVEL
    #include "wx/gtk/assertdlg_gtk.h"
    #if wxUSE_STACKWALKER
        #include "wx/stackwalk.h"
    #endif // wxUSE_STACKWALKER
#endif // wxDEBUG_LEVEL

#include <stdarg.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#ifdef __UNIX__
#include <unistd.h>
#endif

#if wxUSE_DETECT_SM
    #include <X11/SM/SMlib.h>

    #include "wx/unix/utilsx11.h"
#endif

GdkWindow* wxGetTopLevelGDK();

//----------------------------------------------------------------------------
// misc.
//----------------------------------------------------------------------------

void wxBell()
{
    gdk_beep();
}

// ----------------------------------------------------------------------------
// display characteristics
// ----------------------------------------------------------------------------

#ifdef GDK_WINDOWING_X11
void *wxGetDisplay()
{
    return GDK_DISPLAY_XDISPLAY(gdk_window_get_display(wxGetTopLevelGDK()));
}
#endif

wxWindow* wxFindWindowAtPoint(const wxPoint& pt)
{
    return wxGenericFindWindowAtPoint(pt);
}

#if !wxUSE_UNICODE

WXDLLIMPEXP_CORE wxCharBuffer
wxConvertToGTK(const wxString& s, wxFontEncoding enc)
{
    wxWCharBuffer wbuf;
    if ( enc == wxFONTENCODING_SYSTEM || enc == wxFONTENCODING_DEFAULT )
    {
        wbuf = wxConvUI->cMB2WC(s.c_str());
    }
    else // another encoding, use generic conversion class
    {
        wbuf = wxCSConv(enc).cMB2WC(s.c_str());
    }

    if ( !wbuf && !s.empty() )
    {
        // conversion failed, but we still want to show something to the user
        // even if it's going to be wrong it is better than nothing
        //
        // we choose ISO8859-1 here arbitrarily, it's just the most common
        // encoding probably and, also importantly here, conversion from it
        // never fails as it's done internally by wxCSConv
        wbuf = wxCSConv(wxFONTENCODING_ISO8859_1).cMB2WC(s.c_str());
    }

    return wxConvUTF8.cWC2MB(wbuf);
}

WXDLLIMPEXP_CORE wxCharBuffer
wxConvertFromGTK(const wxString& s, wxFontEncoding enc)
{
    // this conversion should never fail as GTK+ always uses UTF-8 internally
    // so there are no complications here
    const wxWCharBuffer wbuf(wxConvUTF8.cMB2WC(s.c_str()));
    if ( enc == wxFONTENCODING_SYSTEM )
        return wxConvUI->cWC2MB(wbuf);

    return wxCSConv(enc).cWC2MB(wbuf);
}

#endif // !wxUSE_UNICODE

// Returns NULL if version is certainly greater or equal than major.minor.micro
// Returns string describing the error if version is lower than
// major.minor.micro OR it cannot be determined and one should not rely on the
// availability of pango version major.minor.micro, nor the non-availability
const gchar *wx_pango_version_check (int major, int minor, int micro)
{
    // NOTE: you don't need to use this macro to check for Pango features
    //       added in pango-1.4 or earlier since GTK 2.4 (our minimum requirement
    //       for GTK lib) required pango 1.4...

#ifdef __WXGTK3__
    return pango_version_check(major, minor, micro);
#elif defined(PANGO_VERSION_MAJOR)
    if (!gtk_check_version (2,11,0))
    {
        // GTK+ 2.11 requires Pango >= 1.15.3 and pango_version_check
        // was added in Pango 1.15.2 thus we know for sure the pango lib we're
        // using has the pango_version_check function:
        return pango_version_check (major, minor, micro);
    }

    return "can't check";
#else // !PANGO_VERSION_MAJOR
    wxUnusedVar(major);
    wxUnusedVar(minor);
    wxUnusedVar(micro);

    return "too old headers";
#endif
}

// ----------------------------------------------------------------------------
// wxPlatformInfo-related
// ----------------------------------------------------------------------------

wxPortId wxGUIAppTraits::GetToolkitVersion(int *verMaj,
                                           int *verMin,
                                           int *verMicro) const
{
#ifdef __WXGTK3__
    if (verMaj)
        *verMaj = gtk_get_major_version();
    if (verMin)
        *verMin = gtk_get_minor_version();
    if (verMicro)
        *verMicro = gtk_get_micro_version();
#else
    if ( verMaj )
        *verMaj = gtk_major_version;
    if ( verMin )
        *verMin = gtk_minor_version;
    if ( verMicro )
        *verMicro = gtk_micro_version;
#endif

    return wxPORT_GTK;
}

#if wxUSE_TIMER

wxTimerImpl *wxGUIAppTraits::CreateTimerImpl(wxTimer *timer)
{
    return new wxGTKTimerImpl(timer);
}

#endif // wxUSE_TIMER

#if wxUSE_DETECT_SM
static wxString GetSM()
{
    wxX11Display dpy;
    if ( !dpy )
        return wxEmptyString;

    char smerr[256];
    char *client_id;
    SmcConn smc_conn = SmcOpenConnection(NULL, NULL,
                                         999, 999,
                                         0 /* mask */, NULL /* callbacks */,
                                         NULL, &client_id,
                                         WXSIZEOF(smerr), smerr);

    if ( !smc_conn )
    {
        // Don't report error if there is no session manager at all
        if (getenv("SESSION_MANAGER"))
        {
            wxLogDebug("Failed to connect to session manager: %s", smerr);
        }
        return wxEmptyString;
    }

    char *vendor = SmcVendor(smc_conn);
    wxString ret = wxString::FromAscii( vendor );
    free(vendor);

    SmcCloseConnection(smc_conn, 0, NULL);
    free(client_id);

    return ret;
}
#endif // wxUSE_DETECT_SM


//-----------------------------------------------------------------------------
// wxGUIAppTraits
//-----------------------------------------------------------------------------

wxEventLoopBase *wxGUIAppTraits::CreateEventLoop()
{
    return new wxEventLoop();
}


#ifdef __UNIX__

#if wxDEBUG_LEVEL && wxUSE_STACKWALKER

// private helper class
class StackDump : public wxStackWalker
{
public:
    StackDump(GtkAssertDialog *dlg) { m_dlg=dlg; }

    void ShowStackInDialog()
    {
        ProcessFrames(0);

        for ( wxVector<Frame>::const_iterator it = m_frames.begin();
              it != m_frames.end();
              ++it )
        {
            gtk_assert_dialog_append_stack_frame(m_dlg,
                                                 it->name.utf8_str(),
                                                 it->file.utf8_str(),
                                                 it->line);
        }

        m_frames.clear();
    }

protected:
    virtual void OnStackFrame(const wxStackFrame& frame) wxOVERRIDE
    {
        const wxString name = frame.GetName();
        if ( name.StartsWith("wxOnAssert") )
        {
            // Ignore all frames until the wxOnAssert() one, just as we do in
            // wxAppTraitsBase::GetAssertStackTrace().
            m_frames.clear();
            return;
        }

        // Also ignore frames which don't have neither the function name nor
        // the file name, showing them in the dialog wouldn't provide any
        // useful information.
        if ( name.empty() && frame.GetFileName().empty() )
            return;

        m_frames.push_back(Frame(frame));
    }

private:
    GtkAssertDialog *m_dlg;

    struct Frame
    {
        explicit Frame(const wxStackFrame& f)
            : name(f.GetName()),
              file(f.GetFileName()),
              line(f.GetLine())
        {
        }

        wxString name;
        wxString file;
        int line;
    };

    wxVector<Frame> m_frames;
};

static void get_stackframe_callback(void* p)
{
    StackDump* dump = static_cast<StackDump*>(p);
    dump->ShowStackInDialog();
}

#endif // wxDEBUG_LEVEL && wxUSE_STACKWALKER

bool wxGUIAppTraits::ShowAssertDialog(const wxString& msg)
{
#if wxDEBUG_LEVEL
    // we can't show the dialog from another thread
    if ( wxIsMainThread() )
    {
        // under GTK2 we prefer to use a dialog widget written using directly
        // in GTK+ as use a dialog written using wxWidgets would need the
        // wxWidgets idle processing to work correctly which might not be the
        // case when assert happens
        GtkWidget *dialog = gtk_assert_dialog_new();
        gtk_assert_dialog_set_message(GTK_ASSERT_DIALOG(dialog), msg.mb_str());

        GdkDisplay* display = gtk_widget_get_display(dialog);
#ifdef __WXGTK4__
        gdk_seat_ungrab(gdk_display_get_default_seat(display));
#elif defined(__WXGTK3__)
        wxGCC_WARNING_SUPPRESS(deprecated-declarations)
        GdkDeviceManager* manager = gdk_display_get_device_manager(display);
        GdkDevice* device = gdk_device_manager_get_client_pointer(manager);
        gdk_device_ungrab(device, unsigned(GDK_CURRENT_TIME));
        wxGCC_WARNING_RESTORE()
#else
        gdk_display_pointer_ungrab(display, unsigned(GDK_CURRENT_TIME));
#endif

#if wxUSE_STACKWALKER
        // save the current stack ow...
        StackDump dump(GTK_ASSERT_DIALOG(dialog));
        dump.SaveStack(100); // showing more than 100 frames is not very useful

        // ...but process it only if the user needs it
        gtk_assert_dialog_set_backtrace_callback
        (
            GTK_ASSERT_DIALOG(dialog),
            get_stackframe_callback,
            &dump
        );
#endif // wxUSE_STACKWALKER

        gint result = gtk_dialog_run(GTK_DIALOG (dialog));
        bool returnCode = false;
        switch (result)
        {
            case GTK_ASSERT_DIALOG_STOP:
                // Don't call wxTrap() directly from here to avoid having the
                // functions between the occurrence of the assert in the code
                // and this function in the call stack. Instead, just set a
                // flag so that inline expansion of the assert macro we are
                // called from calls wxTrap() itself, like this the debugger
                // would break exactly at the assert position.
                wxTrapInAssert = true;
                break;
            case GTK_ASSERT_DIALOG_CONTINUE:
                // nothing to do
                break;
            case GTK_ASSERT_DIALOG_CONTINUE_SUPPRESSING:
                // no more asserts
                returnCode = true;
                break;

            default:
                wxFAIL_MSG( wxT("unexpected return code from GtkAssertDialog") );
        }

        gtk_widget_destroy(dialog);
        return returnCode;
    }
#endif // wxDEBUG_LEVEL

    return wxAppTraitsBase::ShowAssertDialog(msg);
}

#endif // __UNIX__

#if defined(__UNIX__)

wxString wxGUIAppTraits::GetDesktopEnvironment() const
{
    wxString de = wxSystemOptions::GetOption(wxT("gtk.desktop"));
#if wxUSE_DETECT_SM
    if ( de.empty() )
    {
        static const wxString s_SM = GetSM();

        if (s_SM == wxT("GnomeSM"))
            de = wxT("GNOME");
        else if (s_SM == wxT("KDE"))
            de = wxT("KDE");
    }
#endif // wxUSE_DETECT_SM

    return de;
}

#endif // __UNIX__
