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
#include "wx/apptrait.h"

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

#if wxUSE_LOG
    // disable log flushing from here because a call to wxYield() shouldn't
    // normally result in message boxes popping up &c
    wxLog::Suspend();
#endif

    while (EventsPending())
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
// local functions
//-----------------------------------------------------------------------------

// One-shot signal emission hook, to install idle handler.
extern "C" {
static gboolean
wx_emission_hook(GSignalInvocationHint*, guint, const GValue*, gpointer data)
{
    wxApp* app = wxTheApp;
    if (app != NULL)
        app->WakeUpIdle();
    gulong* hook_id = (gulong*)data;
    // record that hook is not installed
    *hook_id = 0;
    // remove hook
    return false;
}
}

// Add signal emission hooks, to re-install idle handler when needed.
static void wx_add_idle_hooks()
{
    // "event" hook
    {
        static gulong hook_id = 0;
        if (hook_id == 0)
        {
            static guint sig_id = 0;
            if (sig_id == 0)
                sig_id = g_signal_lookup("event", GTK_TYPE_WIDGET);
            hook_id = g_signal_add_emission_hook(
                sig_id, 0, wx_emission_hook, &hook_id, NULL);
        }
    }
    // "size_allocate" hook
    // Needed to match the behavior of the old idle system,
    // but probably not necessary.
    {
        static gulong hook_id = 0;
        if (hook_id == 0)
        {
            static guint sig_id = 0;
            if (sig_id == 0)
                sig_id = g_signal_lookup("size_allocate", GTK_TYPE_WIDGET);
            hook_id = g_signal_add_emission_hook(
                sig_id, 0, wx_emission_hook, &hook_id, NULL);
        }
    }
}

extern "C" {
static gboolean wxapp_idle_callback(gpointer)
{
    return wxTheApp->DoIdle();
}
}

bool wxApp::DoIdle()
{
    guint id_save;
    {
        // Allow another idle source to be added while this one is busy.
        // Needed if an idle event handler runs a new event loop,
        // for example by showing a dialog.
#if wxUSE_THREADS
        wxMutexLocker lock(*m_idleMutex);
#endif
        id_save = m_idleSourceId;
        m_idleSourceId = 0;
        wx_add_idle_hooks();
#ifdef __WXDEBUG__
        // don't generate the idle events while the assert modal dialog is shown,
        // this matches the behavior of wxMSW
        if (m_isInAssert)
            return false;
#endif
    }

    gdk_threads_enter();
    bool needMore;
    do {
        needMore = ProcessIdle();
    } while (needMore && gtk_events_pending() == 0);
    gdk_threads_leave();

#if wxUSE_THREADS
    wxMutexLocker lock(*m_idleMutex);
#endif
    // if a new idle source was added during ProcessIdle
    if (m_idleSourceId != 0)
    {
        // remove it
        g_source_remove(m_idleSourceId);
        m_idleSourceId = 0;
    }
    // if more idle processing requested
    if (needMore)
    {
        // keep this source installed
        m_idleSourceId = id_save;
        return true;
    }
    // add hooks and remove this source
    wx_add_idle_hooks();
    return false;
}

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
#if wxUSE_THREADS
    m_idleMutex = NULL;
#endif
    m_idleSourceId = 0;
}

wxApp::~wxApp()
{
}

bool wxApp::OnInitGui()
{
    if ( !wxAppBase::OnInitGui() )
        return false;

    // if this is a wxGLApp (derived from wxApp), and we've already
    // chosen a specific visual, then derive the GdkVisual from that
    if ( GetXVisualInfo() )
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

    XVisualInfo *xvi = (XVisualInfo *)GetXVisualInfo();
    if ( xvi )
        visual = gdkx_visual_get( xvi->visualid );
    else
        visual = gdk_drawable_get_visual( wxGetRootWindow()->window );

    wxASSERT( visual );

    return visual;
}

bool wxApp::Initialize(int& argc, wxChar **argv)
{
    if ( !wxAppBase::Initialize(argc, argv) )
        return false;

#if wxUSE_THREADS
    if (!g_thread_supported())
        g_thread_init(NULL);

    wxgs_poll_func = g_main_context_get_poll_func(NULL);
    g_main_context_set_poll_func(NULL, wxapp_poll_func);
#endif // wxUSE_THREADS

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

    // if wxUSE_INTL==0 it probably indicates that only "C" locale is supported
    // by the program anyhow so prevent GTK+ from calling setlocale(LC_ALL, "")
    // from gtk_init_check() as it does by default
    gtk_disable_setlocale();

#endif // wxUSE_INTL
    static wxConvBrokenFileNames fileconv(encName);
    wxConvFileName = &fileconv;


    bool init_result;

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
    wxUpdateLocaleIsUtf8();

    if ( argcGTK != argc )
    {
        // we have to drop the parameters which were consumed by GTK+
        for ( i = 0; i < argcGTK; i++ )
        {
            while ( strcmp(wxConvUTF8.cWX2MB(argv[i]), argvGTK[i]) != 0 )
            {
                memmove(argv + i, argv + i + 1, (argc - i)*sizeof(*argv));
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

    // update internal arg[cv] as GTK+ may have removed processed options:
    this->argc = argc;
    this->argv = argv;

    if ( m_traits )
    {
        // if there are still GTK+ standard options unparsed in the command
        // line, it means that they were not syntactically correct and GTK+
        // already printed a warning on the command line and we should now
        // exit:
        wxArrayString opt, desc;
        m_traits->GetStandardCmdLineOptions(opt, desc);

        for ( int i = 0; i < argc; i++ )
        {
            // leave just the names of the options with values
            const wxString str = wxString(argv[i]).BeforeFirst('=');

            for ( size_t j = 0; j < opt.size(); j++ )
            {
                // remove the leading spaces from the option string as it does
                // have them
                if ( opt[j].Trim(false).BeforeFirst('=') == str )
                {
                    // a GTK+ option can be left on the command line only if
                    // there was an error in (or before, in another standard
                    // options) it, so abort, just as we do if incorrect
                    // program option is given
                    wxLogError(_("Invalid GTK+ command line option, use \"%s --help\""),
                               argv[0]);
                    return false;
                }
            }
        }
    }

    if ( !init_result )
    {
        wxLogError(_("Unable to initialize GTK+, is DISPLAY set properly?"));
        return false;
    }

    // we can not enter threads before gtk_init is done
    gdk_threads_enter();

    wxSetDetectableAutoRepeat( true );

#if wxUSE_INTL
    wxFont::SetDefaultEncoding(wxLocale::GetSystemEncoding());
#endif

#if wxUSE_THREADS
    m_idleMutex = new wxMutex;
#endif
    // make sure GtkWidget type is loaded, idle hooks need it
    g_type_class_ref(GTK_TYPE_WIDGET);
    WakeUpIdle();

    return true;
}

void wxApp::CleanUp()
{
    if (m_idleSourceId != 0)
        g_source_remove(m_idleSourceId);
#if wxUSE_THREADS
    delete m_idleMutex;
    m_idleMutex = NULL;
#endif
    // release reference acquired by Initialize()
    g_type_class_unref(g_type_class_peek(GTK_TYPE_WIDGET));

    gdk_threads_leave();

    wxAppBase::CleanUp();
}

void wxApp::WakeUpIdle()
{
#if wxUSE_THREADS
    wxMutexLocker lock(*m_idleMutex);
#endif
    if (m_idleSourceId == 0)
        m_idleSourceId = g_idle_add_full(G_PRIORITY_LOW, wxapp_idle_callback, NULL, NULL);
}

// Checking for pending events requires first removing our idle source,
// otherwise it will cause the check to always return true.
bool wxApp::EventsPending()
{
#if wxUSE_THREADS
    wxMutexLocker lock(*m_idleMutex);
#endif
    if (m_idleSourceId != 0)
    {
        g_source_remove(m_idleSourceId);
        m_idleSourceId = 0;
        wx_add_idle_hooks();
    }
    return gtk_events_pending() != 0;
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
