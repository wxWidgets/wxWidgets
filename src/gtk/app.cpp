/////////////////////////////////////////////////////////////////////////////
// Name:        src/gtk/app.cpp
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling, Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __VMS
// vms_jackets.h should for proper working be included before anything else
# include <vms_jackets.h>
#undef ConnectionNumber
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/app.h"

#ifndef WX_PRECOMP
    #include "wx/intl.h"
    #include "wx/log.h"
    #include "wx/utils.h"
    #include "wx/memory.h"
    #include "wx/font.h"
#endif

#include "wx/thread.h"

#ifdef __WXGPE__
    #include <gpe/init.h>
#endif

#include "wx/gtk/win_gtk.h"
#include "wx/gtk/private.h"

#include <gdk/gdkx.h>

//-----------------------------------------------------------------------------
// link GnomeVFS
//-----------------------------------------------------------------------------

#if wxUSE_MIMETYPE && wxUSE_LIBGNOMEVFS
    #include "wx/link.h"
    wxFORCE_LINK_MODULE(gnome_vfs)
#endif

//-----------------------------------------------------------------------------
// global data
//-----------------------------------------------------------------------------

bool   g_mainThreadLocked = false;

static GtkWidget *gs_RootWindow = (GtkWidget*) NULL;

//-----------------------------------------------------------------------------
// idle system
//-----------------------------------------------------------------------------

void wxapp_install_idle_handler();

#if wxUSE_THREADS
static wxMutex gs_idleTagsMutex;
#endif

//-----------------------------------------------------------------------------
// wxYield
//-----------------------------------------------------------------------------

// not static because used by textctrl.cpp
//
// MT-FIXME
bool wxIsInsideYield = false;

bool wxApp::Yield(bool onlyIfNeeded)
{
    if ( wxIsInsideYield )
    {
        if ( !onlyIfNeeded )
        {
            wxFAIL_MSG( wxT("wxYield called recursively" ) );
        }

        return false;
    }

#if wxUSE_THREADS
    if ( !wxThread::IsMain() )
    {
        // can't call gtk_main_iteration() from other threads like this
        return true;
    }
#endif // wxUSE_THREADS

    wxIsInsideYield = true;

    // We need to remove idle callbacks or the loop will
    // never finish.
    SuspendIdleCallback();

#if wxUSE_LOG
    // disable log flushing from here because a call to wxYield() shouldn't
    // normally result in message boxes popping up &c
    wxLog::Suspend();
#endif

    while (gtk_events_pending())
        gtk_main_iteration();

    // It's necessary to call ProcessIdle() to update the frames sizes which
    // might have been changed (it also will update other things set from
    // OnUpdateUI() which is a nice (and desired) side effect). But we
    // call ProcessIdle() only once since this is not meant for longish
    // background jobs (controlled by wxIdleEvent::RequestMore() and the
    // return value of Processidle().
    ProcessIdle();

#if wxUSE_LOG
    // let the logs be flashed again
    wxLog::Resume();
#endif

    wxIsInsideYield = false;

    return true;
}

//-----------------------------------------------------------------------------
// wxWakeUpIdle
//-----------------------------------------------------------------------------

// RR/KH: No wxMutexGui calls are needed here according to the GTK faq,
// http://www.gtk.org/faq/#AEN500 - this caused problems for wxPostEvent.

void wxApp::WakeUpIdle()
{
    wxapp_install_idle_handler();
}

//-----------------------------------------------------------------------------
// local functions
//-----------------------------------------------------------------------------

// the callback functions must be extern "C" to comply with GTK+ declarations
extern "C"
{

// One-shot emission hook for "event" signal, to install idle handler.
// This will be called when the "event" signal is issued on any GtkWidget object.
static gboolean
event_emission_hook(GSignalInvocationHint*, guint, const GValue*, gpointer)
{
    wxapp_install_idle_handler();
    // remove hook
    return false;
}

// add emission hook for "event" signal, to re-install idle handler when needed
static inline void wxAddEmissionHook()
{
    GType widgetType = GTK_TYPE_WIDGET;
    // if GtkWidget type is loaded
    if (g_type_class_peek(widgetType) != NULL)
    {
        guint sig_id = g_signal_lookup("event", widgetType);
        g_signal_add_emission_hook(sig_id, 0, event_emission_hook, NULL, NULL);
    }
}

static gint wxapp_idle_callback( gpointer WXUNUSED(data) )
{
    // this does not look possible, but just in case...
    if (!wxTheApp)
        return false;

    bool moreIdles = false;

#ifdef __WXDEBUG__
    // don't generate the idle events while the assert modal dialog is shown,
    // this matches the behavior of wxMSW
    if (!wxTheApp->IsInAssert())
#endif // __WXDEBUG__
    {
        guint idleID_save;
        {
            // Allow another idle source to be added while this one is busy.
            // Needed if an idle event handler runs a new event loop,
            // for example by showing a dialog.
#if wxUSE_THREADS
            wxMutexLocker lock(gs_idleTagsMutex);
#endif
            idleID_save = wxTheApp->m_idleTag;
            wxTheApp->m_idleTag = 0;
            g_isIdle = true;
            wxAddEmissionHook();
        }

        // When getting called from GDK's time-out handler
        // we are no longer within GDK's grab on the GUI
        // thread so we must lock it here ourselves.
        gdk_threads_enter();

        // Send idle event to all who request them as long as
        // no events have popped up in the event queue.
        do {
            moreIdles = wxTheApp->ProcessIdle();
        } while (moreIdles && gtk_events_pending() == 0);

        // Release lock again
        gdk_threads_leave();
        
        {
            // If another idle source was added, remove it
#if wxUSE_THREADS
            wxMutexLocker lock(gs_idleTagsMutex);
#endif
            if (wxTheApp->m_idleTag != 0)
                g_source_remove(wxTheApp->m_idleTag);
            wxTheApp->m_idleTag = idleID_save;
            g_isIdle = false;
        }
    }

    if (!moreIdles)
    {
#if wxUSE_THREADS
        wxMutexLocker lock(gs_idleTagsMutex);
#endif
        // Indicate that we are now in idle mode and event handlers
        // will have to reinstall the idle handler again.
        g_isIdle = true;
        wxTheApp->m_idleTag = 0;

        wxAddEmissionHook();
    }

    // Return FALSE if no more idle events are to be sent
    return moreIdles;
}
} // extern "C"

#if wxUSE_THREADS

static GPollFunc wxgs_poll_func;

extern "C" {
static gint wxapp_poll_func( GPollFD *ufds, guint nfds, gint timeout )
{
    gdk_threads_enter();

    wxMutexGuiLeave();
    g_mainThreadLocked = true;

    gint res = (*wxgs_poll_func)(ufds, nfds, timeout);

    wxMutexGuiEnter();
    g_mainThreadLocked = false;

    gdk_threads_leave();

    return res;
}
}

#endif // wxUSE_THREADS

void wxapp_install_idle_handler()
{
    if (wxTheApp == NULL)
        return;

#if wxUSE_THREADS
    wxMutexLocker lock(gs_idleTagsMutex);
#endif

    // Don't install the handler if it's already installed. This test *MUST*
    // be done when gs_idleTagsMutex is locked!
    if (!g_isIdle)
        return;

    // GD: this assert is raised when using the thread sample (which works)
    //     so the test is probably not so easy. Can widget callbacks be
    //     triggered from child threads and, if so, for which widgets?
    // wxASSERT_MSG( wxThread::IsMain() || gs_WakeUpIdle, wxT("attempt to install idle handler from widget callback in child thread (should be exclusively from wxWakeUpIdle)") );

    wxASSERT_MSG( wxTheApp->m_idleTag == 0, wxT("attempt to install idle handler twice") );

    g_isIdle = false;

    // This routine gets called by all event handlers
    // indicating that the idle is over. It may also
    // get called from other thread for sending events
    // to the main thread (and processing these in
    // idle time). Very low priority.
    wxTheApp->m_idleTag = g_idle_add_full(G_PRIORITY_LOW, wxapp_idle_callback, NULL, NULL);
}

//-----------------------------------------------------------------------------
// Access to the root window global
//-----------------------------------------------------------------------------

GtkWidget* wxGetRootWindow()
{
    if (gs_RootWindow == NULL)
    {
        gs_RootWindow = gtk_window_new( GTK_WINDOW_TOPLEVEL );
        gtk_widget_realize( gs_RootWindow );
    }
    return gs_RootWindow;
}

//-----------------------------------------------------------------------------
// wxApp
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxApp,wxEvtHandler)

BEGIN_EVENT_TABLE(wxApp, wxEvtHandler)
    EVT_IDLE(wxAppBase::OnIdle)
END_EVENT_TABLE()

wxApp::wxApp()
{
#ifdef __WXDEBUG__
    m_isInAssert = false;
#endif // __WXDEBUG__

    m_idleTag = 0;
    g_isIdle = true;
    wxapp_install_idle_handler();

    // this is NULL for a "regular" wxApp, but is set (and freed) by a wxGLApp
    m_glVisualInfo = (void *) NULL;
    m_glFBCInfo = (void *) NULL;
}

wxApp::~wxApp()
{
    if (m_idleTag)
        g_source_remove( m_idleTag );
}

bool wxApp::OnInitGui()
{
    if ( !wxAppBase::OnInitGui() )
        return false;

    // if this is a wxGLApp (derived from wxApp), and we've already
    // chosen a specific visual, then derive the GdkVisual from that
    if (m_glVisualInfo != NULL)
    {
        GdkVisual* vis = gtk_widget_get_default_visual();

        GdkColormap *colormap = gdk_colormap_new( vis, FALSE );
        gtk_widget_set_default_colormap( colormap );
    }
    else
    {
        // On some machines, the default visual is just 256 colours, so
        // we make sure we get the best. This can sometimes be wasteful.
        if (m_useBestVisual)
        {
            if (m_forceTrueColour)
            {
                GdkVisual* visual = gdk_visual_get_best_with_both( 24, GDK_VISUAL_TRUE_COLOR );
                if (!visual)
                {
                    wxLogError(wxT("Unable to initialize TrueColor visual."));
                    return false;
                }
                GdkColormap *colormap = gdk_colormap_new( visual, FALSE );
                gtk_widget_set_default_colormap( colormap );
            }
            else
            {
                if (gdk_visual_get_best() != gdk_visual_get_system())
                {
                    GdkVisual* visual = gdk_visual_get_best();
                    GdkColormap *colormap = gdk_colormap_new( visual, FALSE );
                    gtk_widget_set_default_colormap( colormap );
                }
            }
        }
    }

    return true;
}

GdkVisual *wxApp::GetGdkVisual()
{
    GdkVisual *visual = NULL;

    if (m_glVisualInfo)
        visual = gdkx_visual_get( ((XVisualInfo *) m_glVisualInfo)->visualid );
    else
        visual = gdk_drawable_get_visual( wxGetRootWindow()->window );

    wxASSERT( visual );

    return visual;
}

bool wxApp::Initialize(int& argc, wxChar **argv)
{
    bool init_result;

#if wxUSE_THREADS
    if (!g_thread_supported())
        g_thread_init(NULL);

    wxgs_poll_func = g_main_context_get_poll_func(NULL);
    g_main_context_set_poll_func(NULL, wxapp_poll_func);
#endif // wxUSE_THREADS

    gtk_set_locale();

    // We should have the wxUSE_WCHAR_T test on the _outside_
#if wxUSE_WCHAR_T
    // gtk+ 2.0 supports Unicode through UTF-8 strings
    wxConvCurrent = &wxConvUTF8;
#else // !wxUSE_WCHAR_T
    if (!wxOKlibc())
        wxConvCurrent = (wxMBConv*) NULL;
#endif // wxUSE_WCHAR_T/!wxUSE_WCHAR_T

    // decide which conversion to use for the file names

    // (1) this variable exists for the sole purpose of specifying the encoding
    //     of the filenames for GTK+ programs, so use it if it is set
    wxString encName(wxGetenv(_T("G_FILENAME_ENCODING")));
    encName = encName.BeforeFirst(_T(','));
    if (encName.CmpNoCase(_T("@locale")) == 0)
        encName.clear();
    encName.MakeUpper();
#if wxUSE_INTL
    if (encName.empty())
    {
        // (2) if a non default locale is set, assume that the user wants his
        //     filenames in this locale too
        encName = wxLocale::GetSystemEncodingName().Upper();
        // (3) finally use UTF-8 by default
        if (encName.empty() || encName == _T("US-ASCII"))
            encName = _T("UTF-8");
        wxSetEnv(_T("G_FILENAME_ENCODING"), encName);
    }
#else
    if (encName.empty())
        encName = _T("UTF-8");
#endif // wxUSE_INTL
    static wxConvBrokenFileNames fileconv(encName);
    wxConvFileName = &fileconv;

#if wxUSE_UNICODE
    // gtk_init() wants UTF-8, not wchar_t, so convert
    int i;
    char **argvGTK = new char *[argc + 1];
    for ( i = 0; i < argc; i++ )
    {
        argvGTK[i] = wxStrdupA(wxConvUTF8.cWX2MB(argv[i]));
    }

    argvGTK[argc] = NULL;

    int argcGTK = argc;

#ifdef __WXGPE__
    init_result = true;  // is there a _check() version of this?
    gpe_application_init( &argcGTK, &argvGTK );
#else
    init_result = gtk_init_check( &argcGTK, &argvGTK );
#endif

    if ( argcGTK != argc )
    {
        // we have to drop the parameters which were consumed by GTK+
        for ( i = 0; i < argcGTK; i++ )
        {
            while ( strcmp(wxConvUTF8.cWX2MB(argv[i]), argvGTK[i]) != 0 )
            {
                memmove(argv + i, argv + i + 1, argc - i);
            }
        }

        argc = argcGTK;
    }
    //else: gtk_init() didn't modify our parameters

    // free our copy
    for ( i = 0; i < argcGTK; i++ )
    {
        free(argvGTK[i]);
    }

    delete [] argvGTK;
#else // !wxUSE_UNICODE
    // gtk_init() shouldn't actually change argv itself (just its contents) so
    // it's ok to pass pointer to it
    init_result = gtk_init_check( &argc, &argv );
#endif // wxUSE_UNICODE/!wxUSE_UNICODE

    if (!init_result) {
        wxLogError(wxT("Unable to initialize gtk, is DISPLAY set properly?"));
        return false;
    }

    // we can not enter threads before gtk_init is done
    gdk_threads_enter();

    if ( !wxAppBase::Initialize(argc, argv) )
    {
        gdk_threads_leave();

        return false;
    }

    wxSetDetectableAutoRepeat( true );

#if wxUSE_INTL
    wxFont::SetDefaultEncoding(wxLocale::GetSystemEncoding());
#endif

    return true;
}

void wxApp::CleanUp()
{
    gdk_threads_leave();

    wxAppBase::CleanUp();
}

#ifdef __WXDEBUG__

void wxApp::OnAssertFailure(const wxChar *file,
                            int line,
                            const wxChar* func,
                            const wxChar* cond,
                            const wxChar *msg)
{

    // block wx idle events while assert dialog is showing
    m_isInAssert = true;

    wxAppBase::OnAssertFailure(file, line, func, cond, msg);

    m_isInAssert = false;
}

#endif // __WXDEBUG__

void wxApp::SuspendIdleCallback()
{
#if wxUSE_THREADS
    wxMutexLocker lock(gs_idleTagsMutex);
#endif
    if (m_idleTag != 0)
    {
        g_source_remove(m_idleTag);
        m_idleTag = 0;
        g_isIdle = true;
        wxAddEmissionHook();
    }
}
