/////////////////////////////////////////////////////////////////////////////
// Name:        app.cpp
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling, Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
    #pragma implementation "app.h"
#endif

#ifdef __VMS
#include <vms_jackets.h>
#undef ConnectionNumber
#endif

#include "wx/app.h"
#include "wx/gdicmn.h"
#include "wx/utils.h"
#include "wx/intl.h"
#include "wx/log.h"
#include "wx/memory.h"
#include "wx/font.h"
#include "wx/settings.h"
#include "wx/dialog.h"
#include "wx/msgdlg.h"
#include "wx/file.h"
#include "wx/filename.h"
#include "wx/module.h"
#include "wx/image.h"

#ifdef __WXUNIVERSAL__
    #include "wx/univ/theme.h"
    #include "wx/univ/renderer.h"
#endif

#if wxUSE_THREADS
    #include "wx/thread.h"
#endif

#include <unistd.h>

#ifdef HAVE_POLL
    #if defined(__VMS)
        #include <poll.h>
    #else
        // bug in the OpenBSD headers: at least in 3.1 there is no extern "C"
        // in neither poll.h nor sys/poll.h which results in link errors later
        #ifdef __OPENBSD__
            extern "C"
            {
        #endif

        #include <sys/poll.h>

        #ifdef __OPENBSD__
            };
        #endif
    #endif // platform
#else // !HAVE_POLL
    // we implement poll() ourselves using select() which is supposed exist in
    // all modern Unices
    #include <sys/types.h>
    #include <sys/time.h>
    #include <unistd.h>
#endif // HAVE_POLL/!HAVE_POLL

#include "wx/gtk/win_gtk.h"

#include <gtk/gtk.h>


//-----------------------------------------------------------------------------
// global data
//-----------------------------------------------------------------------------

wxApp *wxTheApp = (wxApp *)  NULL;
wxAppInitializerFunction wxAppBase::m_appInitFn = (wxAppInitializerFunction) NULL;

bool   g_mainThreadLocked = FALSE;
gint   g_pendingTag = 0;

static GtkWidget *gs_RootWindow = (GtkWidget*) NULL;

//-----------------------------------------------------------------------------
// idle system
//-----------------------------------------------------------------------------

extern bool g_isIdle;

void wxapp_install_idle_handler();

//-----------------------------------------------------------------------------
// wxExit
//-----------------------------------------------------------------------------

void wxExit()
{
    gtk_main_quit();
}

//-----------------------------------------------------------------------------
// wxYield
//-----------------------------------------------------------------------------

// not static because used by textctrl.cpp
//
// MT-FIXME
bool wxIsInsideYield = FALSE;

bool wxApp::Yield(bool onlyIfNeeded)
{
    if ( wxIsInsideYield )
    {
        if ( !onlyIfNeeded )
        {
            wxFAIL_MSG( wxT("wxYield called recursively" ) );
        }

        return FALSE;
    }

#if wxUSE_THREADS
    if ( !wxThread::IsMain() )
    {
        // can't call gtk_main_iteration() from other threads like this
        return TRUE;
    }
#endif // wxUSE_THREADS

    wxIsInsideYield = TRUE;

    if (!g_isIdle)
    {
        // We need to remove idle callbacks or the loop will
        // never finish.
        gtk_idle_remove( m_idleTag );
        m_idleTag = 0;
        g_isIdle = TRUE;
    }

    // disable log flushing from here because a call to wxYield() shouldn't
    // normally result in message boxes popping up &c
    wxLog::Suspend();

    while (gtk_events_pending())
        gtk_main_iteration();

    // It's necessary to call ProcessIdle() to update the frames sizes which
    // might have been changed (it also will update other things set from
    // OnUpdateUI() which is a nice (and desired) side effect). But we
    // call ProcessIdle() only once since this is not meant for longish
    // background jobs (controlled by wxIdleEvent::RequestMore() and the
    // return value of Processidle().
    ProcessIdle();

    // let the logs be flashed again
    wxLog::Resume();

    wxIsInsideYield = FALSE;

    return TRUE;
}

//-----------------------------------------------------------------------------
// wxWakeUpIdle
//-----------------------------------------------------------------------------

void wxWakeUpIdle()
{
#if wxUSE_THREADS
    if (!wxThread::IsMain())
        wxMutexGuiEnter();
#endif

    if (g_isIdle)
        wxapp_install_idle_handler();

#if wxUSE_THREADS
    if (!wxThread::IsMain())
        wxMutexGuiLeave();
#endif
}

//-----------------------------------------------------------------------------
// local functions
//-----------------------------------------------------------------------------

// the callback functions must be extern "C" to comply with GTK+ declarations
extern "C"
{

static gint wxapp_pending_callback( gpointer WXUNUSED(data) )
{
    if (!wxTheApp) return TRUE;

    // When getting called from GDK's time-out handler
    // we are no longer within GDK's grab on the GUI
    // thread so we must lock it here ourselves.
    gdk_threads_enter();

    // Sent idle event to all who request them.
    wxTheApp->ProcessPendingEvents();

    g_pendingTag = 0;

    // Flush the logged messages if any.
#if wxUSE_LOG
    wxLog::FlushActive();
#endif // wxUSE_LOG

    // Release lock again
    gdk_threads_leave();

    // Return FALSE to indicate that no more idle events are
    // to be sent (single shot instead of continuous stream)
    return FALSE;
}

static gint wxapp_idle_callback( gpointer WXUNUSED(data) )
{
    if (!wxTheApp)
        return TRUE;

#ifdef __WXDEBUG__
    // don't generate the idle events while the assert modal dialog is shown,
    // this completely confuses the apps which don't expect to be reentered
    // from some safely-looking functions
    if ( wxTheApp->IsInAssert() )
    {
        // But repaint the assertion message if necessary
        if (wxTopLevelWindows.GetCount() > 0)
        {
            wxWindow* win = (wxWindow*) wxTopLevelWindows.GetLast()->GetData();
#ifdef __WXGTK20__
            if (win->IsKindOf(CLASSINFO(wxMessageDialog)))
#else
            if (win->IsKindOf(CLASSINFO(wxGenericMessageDialog)))
#endif
                win->OnInternalIdle();
        }
        return TRUE;
    }
#endif // __WXDEBUG__

    // When getting called from GDK's time-out handler
    // we are no longer within GDK's grab on the GUI
    // thread so we must lock it here ourselves.
    gdk_threads_enter();

    // Indicate that we are now in idle mode and event handlers
    // will have to reinstall the idle handler again.
    g_isIdle = TRUE;
    wxTheApp->m_idleTag = 0;

    // Send idle event to all who request them as long as
    // no events have popped up in the event queue.
    while (wxTheApp->ProcessIdle() && (gtk_events_pending() == 0))
        ;

    // Release lock again
    gdk_threads_leave();

    // Return FALSE to indicate that no more idle events are
    // to be sent (single shot instead of continuous stream).
    return FALSE;
}

#if wxUSE_THREADS

#ifdef HAVE_POLL
    #define wxPoll poll
    #define wxPollFd pollfd
#else // !HAVE_POLL

typedef GPollFD wxPollFd;

int wxPoll(wxPollFd *ufds, unsigned int nfds, int timeout)
{
    // convert timeout from ms to struct timeval (s/us)
    timeval tv_timeout;
    tv_timeout.tv_sec = timeout/1000;
    tv_timeout.tv_usec = (timeout%1000)*1000;

    // remember the highest fd used here
    int fdMax = -1;

    // and fill the sets for select()
    fd_set readfds;
    fd_set writefds;
    fd_set exceptfds;
    FD_ZERO(&readfds);
    FD_ZERO(&writefds);
    FD_ZERO(&exceptfds);

    unsigned int i;
    for ( i = 0; i < nfds; i++ )
    {
        wxASSERT_MSG( ufds[i].fd < FD_SETSIZE, _T("fd out of range") );

        if ( ufds[i].events & G_IO_IN )
            FD_SET(ufds[i].fd, &readfds);

        if ( ufds[i].events & G_IO_PRI )
            FD_SET(ufds[i].fd, &exceptfds);

        if ( ufds[i].events & G_IO_OUT )
            FD_SET(ufds[i].fd, &writefds);

        if ( ufds[i].fd > fdMax )
            fdMax = ufds[i].fd;
    }

    fdMax++;
    int res = select(fdMax, &readfds, &writefds, &exceptfds, &tv_timeout);

    // translate the results back
    for ( i = 0; i < nfds; i++ )
    {
        ufds[i].revents = 0;

        if ( FD_ISSET(ufds[i].fd, &readfds ) )
            ufds[i].revents |= G_IO_IN;

        if ( FD_ISSET(ufds[i].fd, &exceptfds ) )
            ufds[i].revents |= G_IO_PRI;

        if ( FD_ISSET(ufds[i].fd, &writefds ) )
            ufds[i].revents |= G_IO_OUT;
    }

    return res;
}

#endif // HAVE_POLL/!HAVE_POLL

static gint wxapp_poll_func( GPollFD *ufds, guint nfds, gint timeout )
{
    gdk_threads_enter();

    wxMutexGuiLeave();
    g_mainThreadLocked = TRUE;

    // we rely on the fact that glib GPollFD struct is really just pollfd but
    // I wonder how wise is this in the long term (VZ)
    gint res = wxPoll( (wxPollFd *) ufds, nfds, timeout );

    wxMutexGuiEnter();
    g_mainThreadLocked = FALSE;

    gdk_threads_leave();

    return res;
}

#endif // wxUSE_THREADS

} // extern "C"

void wxapp_install_idle_handler()
{
    wxASSERT_MSG( wxTheApp->m_idleTag == 0, wxT("attempt to install idle handler twice") );

    g_isIdle = FALSE;

    if (g_pendingTag == 0)
        g_pendingTag = gtk_idle_add_priority( 900, wxapp_pending_callback, (gpointer) NULL );

    // This routine gets called by all event handlers
    // indicating that the idle is over. It may also
    // get called from other thread for sending events
    // to the main thread (and processing these in
    // idle time). Very low priority.
    wxTheApp->m_idleTag = gtk_idle_add_priority( 1000, wxapp_idle_callback, (gpointer) NULL );
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
    EVT_IDLE(wxApp::OnIdle)
END_EVENT_TABLE()

wxApp::wxApp()
{
    m_initialized = FALSE;
#ifdef __WXDEBUG__
    m_isInAssert = FALSE;
#endif // __WXDEBUG__

    m_idleTag = 0;
    wxapp_install_idle_handler();

#if wxUSE_THREADS
    g_main_set_poll_func( wxapp_poll_func );
#endif

    m_colorCube = (unsigned char*) NULL;

    // this is NULL for a "regular" wxApp, but is set (and freed) by a wxGLApp
    m_glVisualInfo = (void *) NULL;
}

wxApp::~wxApp()
{
    if (m_idleTag) gtk_idle_remove( m_idleTag );

    if (m_colorCube) free(m_colorCube);
}

bool wxApp::OnInitGui()
{
    if ( !wxAppBase::OnInitGui() )
        return FALSE;

    GdkVisual *visual = gdk_visual_get_system();

    // if this is a wxGLApp (derived from wxApp), and we've already
    // chosen a specific visual, then derive the GdkVisual from that
    if (m_glVisualInfo != NULL)
    {
#ifdef __WXGTK20__
        // seems gtk_widget_set_default_visual no longer exists?
        GdkVisual* vis = gtk_widget_get_default_visual();
#else
        GdkVisual* vis = gdkx_visual_get(
            ((XVisualInfo *) m_glVisualInfo) ->visualid );
        gtk_widget_set_default_visual( vis );
#endif

        GdkColormap *colormap = gdk_colormap_new( vis, FALSE );
        gtk_widget_set_default_colormap( colormap );

        visual = vis;
    }

    // On some machines, the default visual is just 256 colours, so
    // we make sure we get the best. This can sometimes be wasteful.

    else
    if ((gdk_visual_get_best() != gdk_visual_get_system()) && (m_useBestVisual))
    {
#ifdef __WXGTK20__
        /* seems gtk_widget_set_default_visual no longer exists? */
        GdkVisual* vis = gtk_widget_get_default_visual();
#else
        GdkVisual* vis = gdk_visual_get_best();
        gtk_widget_set_default_visual( vis );
#endif

        GdkColormap *colormap = gdk_colormap_new( vis, FALSE );
        gtk_widget_set_default_colormap( colormap );

        visual = vis;
    }

    // Nothing to do for 15, 16, 24, 32 bit displays
    if (visual->depth > 8) return TRUE;

    // initialize color cube for 8-bit color reduction dithering

    GdkColormap *cmap = gtk_widget_get_default_colormap();

    m_colorCube = (unsigned char*)malloc(32 * 32 * 32);

    for (int r = 0; r < 32; r++)
    {
        for (int g = 0; g < 32; g++)
        {
            for (int b = 0; b < 32; b++)
            {
                int rr = (r << 3) | (r >> 2);
                int gg = (g << 3) | (g >> 2);
                int bb = (b << 3) | (b >> 2);

                int index = -1;

                GdkColor *colors = cmap->colors;
                if (colors)
                {
                    int max = 3 * 65536;

                    for (int i = 0; i < cmap->size; i++)
                    {
                        int rdiff = ((rr << 8) - colors[i].red);
                        int gdiff = ((gg << 8) - colors[i].green);
                        int bdiff = ((bb << 8) - colors[i].blue);
                        int sum = ABS (rdiff) + ABS (gdiff) + ABS (bdiff);
                        if (sum < max)
                        {
                            index = i; max = sum;
                        }
                    }
                }
                else
                {
                    // assume 8-bit true or static colors. this really exists
                    GdkVisual* vis = gdk_colormap_get_visual( cmap );
                    index = (r >> (5 - vis->red_prec)) << vis->red_shift;
                    index |= (g >> (5 - vis->green_prec)) << vis->green_shift;
                    index |= (b >> (5 - vis->blue_prec)) << vis->blue_shift;
                }
                m_colorCube[ (r*1024) + (g*32) + b ] = index;
            }
        }
    }

    return TRUE;
}

GdkVisual *wxApp::GetGdkVisual()
{
    GdkVisual *visual = NULL;

    if (m_glVisualInfo)
        visual = gdkx_visual_get( ((XVisualInfo *) m_glVisualInfo)->visualid );
    else
        visual = gdk_window_get_visual( wxGetRootWindow()->window );

    wxASSERT( visual );

    return visual;
}

bool wxApp::ProcessIdle()
{
    wxWindowList::Node* node = wxTopLevelWindows.GetFirst();
    node = wxTopLevelWindows.GetFirst();
    while (node)
    {
        wxWindow* win = node->GetData();
        CallInternalIdle( win );

        node = node->GetNext();
    }

    wxIdleEvent event;
    event.SetEventObject( this );
    ProcessEvent( event );

    return event.MoreRequested();
}

void wxApp::OnIdle( wxIdleEvent &event )
{
    static bool s_inOnIdle = FALSE;

    // Avoid recursion (via ProcessEvent default case)
    if (s_inOnIdle)
        return;

    s_inOnIdle = TRUE;

    // Resend in the main thread events which have been prepared in other
    // threads
    ProcessPendingEvents();

    // 'Garbage' collection of windows deleted with Close()
    DeletePendingObjects();

    // Send OnIdle events to all windows
    bool needMore = SendIdleEvents();

    if (needMore)
        event.RequestMore(TRUE);

    s_inOnIdle = FALSE;
}

bool wxApp::SendIdleEvents()
{
    bool needMore = FALSE;

    wxWindowList::Node* node = wxTopLevelWindows.GetFirst();
    while (node)
    {
        wxWindow* win = node->GetData();
        if (SendIdleEvents(win))
            needMore = TRUE;

        node = node->GetNext();
    }

    return needMore;
}

bool wxApp::CallInternalIdle( wxWindow* win )
{
    win->OnInternalIdle();

    wxWindowList::Node  *node = win->GetChildren().GetFirst();
    while (node)
    {
        wxWindow    *win = node->GetData();

        CallInternalIdle( win );
        node = node->GetNext();
    }

    return TRUE;
}

bool wxApp::SendIdleEvents( wxWindow* win )
{
    bool needMore = FALSE;

    wxIdleEvent event;
    event.SetEventObject(win);

    win->GetEventHandler()->ProcessEvent(event);

    if (event.MoreRequested())
        needMore = TRUE;

    wxWindowList::Node  *node = win->GetChildren().GetFirst();
    while (node)
    {
        wxWindow    *win = node->GetData();

        if (SendIdleEvents(win))
            needMore = TRUE;
        node = node->GetNext();
    }

    return needMore;
}

int wxApp::MainLoop()
{
    gtk_main();
    return 0;
}

void wxApp::ExitMainLoop()
{
    if (gtk_main_level() > 0)
        gtk_main_quit();
}

bool wxApp::Initialized()
{
    return m_initialized;
}

bool wxApp::Pending()
{
    return (gtk_events_pending() > 0);
}

void wxApp::Dispatch()
{
    gtk_main_iteration();
}

void wxApp::DeletePendingObjects()
{
    wxNode *node = wxPendingDelete.GetFirst();
    while (node)
    {
        wxObject *obj = (wxObject *)node->GetData();

        delete obj;

        if (wxPendingDelete.Find(obj))
            delete node;

        node = wxPendingDelete.GetFirst();
    }
}

bool wxApp::Initialize()
{
    wxClassInfo::InitializeClasses();

    // GL: I'm annoyed ... I don't know where to put this and I don't want to
    // create a module for that as it's part of the core.
#if wxUSE_THREADS
    wxPendingEvents = new wxList();
    wxPendingEventsLocker = new wxCriticalSection();
#endif

    wxTheColourDatabase = new wxColourDatabase( wxKEY_STRING );
    wxTheColourDatabase->Initialize();

    wxInitializeStockLists();
    wxInitializeStockObjects();

    wxModule::RegisterModules();
    if (!wxModule::InitializeModules())
        return FALSE;

#if wxUSE_INTL
    wxFont::SetDefaultEncoding(wxLocale::GetSystemEncoding());
#endif

    return TRUE;
}

void wxApp::CleanUp()
{
    wxModule::CleanUpModules();

    delete wxTheColourDatabase;
    wxTheColourDatabase = (wxColourDatabase*) NULL;

    wxDeleteStockObjects();

    wxDeleteStockLists();

    delete wxTheApp;
    wxTheApp = (wxApp*) NULL;

    wxClassInfo::CleanUpClasses();

#if wxUSE_THREADS
    delete wxPendingEvents;
    wxPendingEvents = NULL;
    delete wxPendingEventsLocker;
    wxPendingEventsLocker = NULL;
#endif

    // check for memory leaks
#if (defined(__WXDEBUG__) && wxUSE_MEMORY_TRACING) || wxUSE_DEBUG_CONTEXT
    if (wxDebugContext::CountObjectsLeft(TRUE) > 0)
    {
        wxLogDebug(wxT("There were memory leaks.\n"));
        wxDebugContext::Dump();
        wxDebugContext::PrintStatistics();
    }
#endif // Debug

#if wxUSE_LOG
    // do this as the very last thing because everything else can log messages
    wxLog::DontCreateOnDemand();

    wxLog *oldLog = wxLog::SetActiveTarget( (wxLog*) NULL );
    if (oldLog)
        delete oldLog;
#endif // wxUSE_LOG
}

//-----------------------------------------------------------------------------
// wxEntry
//-----------------------------------------------------------------------------

// NB: argc and argv may be changed here, pass by reference!
int wxEntryStart( int& argc, char *argv[] )
{
#if wxUSE_THREADS
    // GTK 1.2 up to version 1.2.3 has broken threads
    if ((gtk_major_version == 1) &&
        (gtk_minor_version == 2) &&
        (gtk_micro_version < 4))
    {
        printf( "wxWindows warning: GUI threading disabled due to outdated GTK version\n" );
    }
    else
    {
        g_thread_init(NULL);
    }
#endif

    gtk_set_locale();

    // We should have the wxUSE_WCHAR_T test on the _outside_
#if wxUSE_WCHAR_T
#if defined(__WXGTK20__)
    // gtk+ 2.0 supports Unicode through UTF-8 strings
    wxConvCurrent = &wxConvUTF8;
#else
    if (!wxOKlibc()) wxConvCurrent = &wxConvLocal;
#endif
#else
    if (!wxOKlibc()) wxConvCurrent = (wxMBConv*) NULL;
#endif

    gdk_threads_enter();

    gtk_init( &argc, &argv );

    wxSetDetectableAutoRepeat( TRUE );

    if (!wxApp::Initialize())
    {
        gdk_threads_leave();
        return -1;
    }

    return 0;
}


int wxEntryInitGui()
{
    int retValue = 0;

    if ( !wxTheApp->OnInitGui() )
        retValue = -1;

    wxGetRootWindow();

    return retValue;
}


void wxEntryCleanup()
{
#if wxUSE_LOG
    // flush the logged messages if any
    wxLog *log = wxLog::GetActiveTarget();
    if (log != NULL && log->HasPendingMessages())
        log->Flush();

    // continuing to use user defined log target is unsafe from now on because
    // some resources may be already unavailable, so replace it by something
    // more safe
    wxLog *oldlog = wxLog::SetActiveTarget(new wxLogStderr);
    if ( oldlog )
        delete oldlog;
#endif // wxUSE_LOG

    wxApp::CleanUp();

    gdk_threads_leave();
}


int wxEntry( int argc, char *argv[] )
{
#if (defined(__WXDEBUG__) && wxUSE_MEMORY_TRACING) || wxUSE_DEBUG_CONTEXT
    // This seems to be necessary since there are 'rogue'
    // objects present at this point (perhaps global objects?)
    // Setting a checkpoint will ignore them as far as the
    // memory checking facility is concerned.
    // Of course you may argue that memory allocated in globals should be
    // checked, but this is a reasonable compromise.
    wxDebugContext::SetCheckpoint();
#endif
    int err = wxEntryStart(argc, argv);
    if (err)
        return err;

    if (!wxTheApp)
    {
        wxCHECK_MSG( wxApp::GetInitializerFunction(), -1,
                     wxT("wxWindows error: No initializer - use IMPLEMENT_APP macro.\n") );

        wxAppInitializerFunction app_ini = wxApp::GetInitializerFunction();

        wxObject *test_app = app_ini();

        wxTheApp = (wxApp*) test_app;
    }

    wxCHECK_MSG( wxTheApp, -1, wxT("wxWindows error: no application object") );

    wxTheApp->argc = argc;
#if wxUSE_UNICODE
    wxTheApp->argv = new wxChar*[argc+1];
    int mb_argc = 0;
    while (mb_argc < argc)
    {
        wxTheApp->argv[mb_argc] = wxStrdup(wxConvLibc.cMB2WX(argv[mb_argc]));
        mb_argc++;
    }
    wxTheApp->argv[mb_argc] = (wxChar *)NULL;
#else
    wxTheApp->argv = argv;
#endif

    if (wxTheApp->argc > 0)
    {
        wxFileName fname( wxTheApp->argv[0] );
        wxTheApp->SetAppName( fname.GetName() );
    }

    int retValue;
    retValue = wxEntryInitGui();

    // Here frames insert themselves automatically into wxTopLevelWindows by
    // getting created in OnInit().
    if ( retValue == 0 )
    {
        if ( !wxTheApp->OnInit() )
            retValue = -1;
    }

    if ( retValue == 0 )
    {
        // Delete pending toplevel windows
        wxTheApp->DeletePendingObjects();

        // When is the app not initialized ?
        wxTheApp->m_initialized = TRUE;

        if (wxTheApp->Initialized())
        {
            wxTheApp->OnRun();

            wxWindow *topWindow = wxTheApp->GetTopWindow();

            // Delete all pending windows if any
            wxTheApp->DeletePendingObjects();

            // Reset top window
            if (topWindow)
                wxTheApp->SetTopWindow( (wxWindow*) NULL );

            retValue = wxTheApp->OnExit();
        }
    }

    wxEntryCleanup();

    return retValue;
}

#ifdef __WXDEBUG__

void wxApp::OnAssert(const wxChar *file, int line, const wxChar* cond, const wxChar *msg)
{
    m_isInAssert = TRUE;

    wxAppBase::OnAssert(file, line, cond, msg);

    m_isInAssert = FALSE;
}

#endif // __WXDEBUG__

