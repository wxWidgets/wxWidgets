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

#include "wx/app.h"
#include "wx/gdicmn.h"
#include "wx/utils.h"
#include "wx/intl.h"
#include "wx/log.h"
#include "wx/memory.h"
#include "wx/font.h"
#include "wx/settings.h"
#include "wx/dialog.h"

#if wxUSE_WX_RESOURCES
    #include "wx/resource.h"
#endif

#include "wx/module.h"
#include "wx/image.h"

#if wxUSE_THREADS
#include "wx/thread.h"
#endif

#include "unistd.h"

#include "glib.h"
#include "gdk/gdk.h"
#include "gtk/gtk.h"

#include "wx/gtk/win_gtk.h"

//-----------------------------------------------------------------------------
// global data
//-----------------------------------------------------------------------------

wxApp *wxTheApp = (wxApp *)  NULL;
wxAppInitializerFunction wxApp::m_appInitFn = (wxAppInitializerFunction) NULL;

#if wxUSE_THREADS
extern wxList *wxPendingEvents;
extern wxCriticalSection *wxPendingEventsLocker;
#endif
extern wxResourceCache *wxTheResourceCache;
extern bool g_isIdle;

unsigned char g_palette[64*3] =
{
  0x0,  0x0,  0x0,
  0xff, 0xff, 0xff,
  0xff, 0x0,  0x0,
  0xff, 0xff, 0x0,
  0x0,  0xff, 0x0,
  0x0,  0x0,  0xff,
  0x0,  0xff, 0xff,
  0x99, 0x99, 0x99,
  0xff, 0x88, 0x0,
  0x88, 0x0,  0x0,
  0x0,  0x88, 0x88,
  0x88, 0x88, 0x0,
  0xff, 0xcc, 0x97,
  0xbb, 0xbb, 0xbb,
  0x9f, 0x6b, 0x42,
  0x55, 0x55, 0x55,
  0xdd, 0xdd, 0xdd,
  0x77, 0x77, 0x77,
  0x33, 0x33, 0x33,
  0xcc, 0x0,  0x0,
  0xff, 0x44, 0x0,
  0xff, 0xcc, 0x0,
  0xcc, 0xcc, 0x0,
  0x60, 0x60, 0x0,
  0x0,  0x43, 0x0,
  0x0,  0x7f, 0x0,
  0x0,  0xcc, 0x0,
  0x0,  0x44, 0x44,
  0x0,  0x0,  0x44,
  0x0,  0x0,  0x88,
  0xef, 0xb1, 0x7b,
  0xdf, 0x98, 0x5f,
  0xbf, 0x87, 0x56,
  0x7f, 0x57, 0x26,
  0x5f, 0x39, 0xc,
  0x3f, 0x1c, 0x0,
  0x21, 0x0,  0x0,
  0x0,  0x43, 0x87,
  0x2d, 0x70, 0xaf,
  0x5a, 0x9e, 0xd7,
  0x87, 0xcc, 0xff,
  0xff, 0xe0, 0xba,
  0x21, 0x43, 0xf,
  0x3d, 0x5d, 0x25,
  0x59, 0x78, 0x3a,
  0x75, 0x93, 0x4f,
  0x91, 0xae, 0x64,
  0xad, 0xc8, 0x7a,
  0xf0, 0xa8, 0xef,
  0xd0, 0x88, 0xd0,
  0xaf, 0x66, 0xaf,
  0x8e, 0x44, 0x8e,
  0x6d, 0x22, 0x6d,
  0x4b, 0x0,  0x4b,
  0xff, 0xc0, 0xbc,
  0xff, 0x93, 0x91,
  0xff, 0x66, 0x67,
  0xd8, 0xf2, 0xbf,
  0xff, 0xc9, 0x68,
  0xff, 0x96, 0x67,
  0xa5, 0x60, 0xff,
  0x51, 0xff, 0x99,
  0x3f, 0xa5, 0x63,
  0x98, 0x90, 0x67
};

//-----------------------------------------------------------------------------
// local functions
//-----------------------------------------------------------------------------

extern void wxFlushResources(void);

//-----------------------------------------------------------------------------
// global functions
//-----------------------------------------------------------------------------

void wxExit()
{
    gtk_main_quit();
}

/* forward declaration */
gint wxapp_idle_callback( gpointer WXUNUSED(data) );

bool wxYield()
{
    /* it's necessary to call ProcessIdle() to update the frames sizes which
       might have been changed (it also will update other things set from
       OnUpdateUI() which is a nice (and desired) side effect) */
    while (wxTheApp->ProcessIdle()) { }
       
#if 0
    for ( wxWindowList::Node *node = wxTopLevelWindows.GetFirst();
          node;
          node = node->GetNext() )
    {
        wxWindow *win = node->GetData();
        win->OnInternalIdle();
    }
#endif

    if (wxTheApp->m_idleTag)
    {
        /* We need to temporarily remove idle callbacks or the loop will
           never finish. */
        gtk_idle_remove( wxTheApp->m_idleTag );
        wxTheApp->m_idleTag = 0;

        while (gtk_events_pending())
            gtk_main_iteration();

        /* re-add idle handler */
        wxTheApp->m_idleTag = gtk_idle_add( wxapp_idle_callback, (gpointer) NULL );
    }
    else
    {
        while (gtk_events_pending())
            gtk_main_iteration();
    }
    
    return TRUE;
}

gint wxapp_idle_callback( gpointer WXUNUSED(data) )
{
    if (!wxTheApp) return TRUE;
    
#if (GTK_MINOR_VERSION > 0)
    /* when getting called from GDK's idle handler we
       are no longer within GDK's grab on the GUI
       thread so we must lock it here ourselves */
    GDK_THREADS_ENTER ();
#endif
    
    /* sent idle event to all who request them */
    while (wxTheApp->ProcessIdle()) { }
    
    /* we don't want any more idle events until the next event is
       sent to wxGTK */
    gtk_idle_remove( wxTheApp->m_idleTag );
    wxTheApp->m_idleTag = 0;

    /* indicate that we are now in idle mode - even so deeply
       in idle mode that we don't get any idle events anymore.
       this is like wxMSW where an idle event is sent only
       once each time after the event queue has been completely
       emptied */
    g_isIdle = TRUE;
    
#if (GTK_MINOR_VERSION > 0)
    /* release lock again */
    GDK_THREADS_LEAVE ();
#endif

    return TRUE;
}

void wxapp_install_idle_handler()
{
    wxASSERT_MSG( wxTheApp->m_idleTag == 0, _T("attempt to install idle handler twice") );

    /* this routine gets called by all event handlers
       indicating that the idle is over. */

    wxTheApp->m_idleTag = gtk_idle_add( wxapp_idle_callback, (gpointer) NULL );
    
    g_isIdle = FALSE;
}

#if wxUSE_THREADS
static gint wxapp_wakeup_timerout_callback( gpointer WXUNUSED(data) )
{
    gtk_timeout_remove( wxTheApp->m_wakeUpTimerTag );
    wxTheApp->m_wakeUpTimerTag = 0;
    
#if (GTK_MINOR_VERSION > 0)
    /* when getting called from GDK's time-out handler 
       we are no longer within GDK's grab on the GUI
       thread so we must lock it here ourselves */
    GDK_THREADS_ENTER ();
#endif
    
    /* unblock other threads wishing to do some GUI things */
    wxMutexGuiLeave();
    
    /* wake up other threads */
    wxUsleep( 1 );
    
    /* block other thread again  */
    wxMutexGuiEnter();
    
#if (GTK_MINOR_VERSION > 0)
    /* release lock again */
    GDK_THREADS_LEAVE ();
#endif
    
    wxTheApp->m_wakeUpTimerTag = gtk_timeout_add( 20, wxapp_wakeup_timerout_callback, (gpointer) NULL );
    
    return TRUE;
}
#endif

//-----------------------------------------------------------------------------
// wxApp
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxApp,wxEvtHandler)

BEGIN_EVENT_TABLE(wxApp, wxEvtHandler)
    EVT_IDLE(wxApp::OnIdle)
END_EVENT_TABLE()

wxApp::wxApp()
{
    wxTheApp = this;

    m_topWindow = (wxWindow *) NULL;
    m_exitOnFrameDelete = TRUE;

    m_idleTag = gtk_idle_add( wxapp_idle_callback, (gpointer) NULL );
    
#if wxUSE_THREADS
    m_wakeUpTimerTag = gtk_timeout_add( 20, wxapp_wakeup_timerout_callback, (gpointer) NULL );
#endif

    m_colorCube = (unsigned char*) NULL;
}

wxApp::~wxApp()
{
    if (m_idleTag) gtk_idle_remove( m_idleTag );

#if wxUSE_THREADS
    if (m_wakeUpTimerTag) gtk_timeout_remove( m_wakeUpTimerTag );
#endif
    
    if (m_colorCube) free(m_colorCube);
}

bool wxApp::OnInitGui()
{
    /* on some SGIs, the default visual is just 256 colours, so we
       make sure we get the best. this can sometimes be wasteful,
       of course, but what do these guys pay $30.000 for? */
    GdkVisual* visual = gdk_visual_get_best();
    gtk_widget_set_default_visual( visual );

    /* Nothing to do for 15, 16, 24, 32 bit displays */
    if (visual->depth > 8) return TRUE;

        /* this initiates the standard palette as defined by GdkImlib
           in the GNOME libraries. it ensures that all GNOME applications
           use the same 64 colormap entries on 8-bit displays so you
           can use several rather graphics-heavy applications at the
           same time.
           NOTE: this doesn't really seem to work this way... */

        /*
        GdkColormap *cmap = gdk_colormap_new( gdk_visual_get_system(), TRUE );

        for (int i = 0; i < 64; i++)
        {
            GdkColor col;
            col.red    = g_palette[i*3 + 0] << 8;
            col.green  = g_palette[i*3 + 1] << 8;
            col.blue   = g_palette[i*3 + 2] << 8;
            col.pixel  = 0;

            gdk_color_alloc( cmap, &col );
        }

        gtk_widget_set_default_colormap( cmap );
        */

    /* initialize color cube for 8-bit color reduction dithering */

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
		    /* assume 8-bit true or static colors. this really
		       exists. */
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

bool wxApp::ProcessIdle()
{
    wxIdleEvent event;
    event.SetEventObject( this );
    ProcessEvent( event );

    return event.MoreRequested();
}

void wxApp::OnIdle( wxIdleEvent &event )
{
    static bool inOnIdle = FALSE;

    /* Avoid recursion (via ProcessEvent default case) */
    if (inOnIdle)
        return;

    inOnIdle = TRUE;

#if wxUSE_THREADS
    /* Resend in the main thread events which have been prepared in other
       threads */
    ProcessPendingEvents();
#endif

    /* 'Garbage' collection of windows deleted with Close(). */
    DeletePendingObjects();

    /* flush the logged messages if any */
#if wxUSE_LOG
    wxLog *log = wxLog::GetActiveTarget();
    if (log != NULL && log->HasPendingMessages())
        log->Flush();
#endif // wxUSE_LOG

    /* Send OnIdle events to all windows */
    bool needMore = SendIdleEvents();

    if (needMore)
        event.RequestMore(TRUE);

    inOnIdle = FALSE;
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

bool wxApp::SendIdleEvents( wxWindow* win )
{
    bool needMore = FALSE;

    wxIdleEvent event;
    event.SetEventObject(win);

    win->OnInternalIdle();

    win->ProcessEvent(event);

    if (event.MoreRequested())
        needMore = TRUE;

    wxNode* node = win->GetChildren().First();
    while (node)
    {
        wxWindow* win = (wxWindow*) node->Data();
        if (SendIdleEvents(win))
            needMore = TRUE;

        node = node->Next();
    }
    return needMore ;
}

int wxApp::MainLoop()
{
    gtk_main();
    return 0;
}

void wxApp::ExitMainLoop()
{
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

#if wxUSE_THREADS
void wxApp::ProcessPendingEvents()
{
    wxNode *node = wxPendingEvents->First();
    wxCriticalSectionLocker locker(*wxPendingEventsLocker);

    while (node)
    {
        wxEvtHandler *handler = (wxEvtHandler *)node->Data();

        handler->ProcessPendingEvents();

        delete node;

        node = wxPendingEvents->First();
    }
}
#endif // wxUSE_THREADS

void wxApp::DeletePendingObjects()
{
    wxNode *node = wxPendingDelete.First();
    while (node)
    {
        wxObject *obj = (wxObject *)node->Data();

        delete obj;

        if (wxPendingDelete.Find(obj))
            delete node;

        node = wxPendingDelete.First();
    }
}

wxWindow *wxApp::GetTopWindow()
{
    if (m_topWindow)
        return m_topWindow;
    else if (wxTopLevelWindows.GetCount() > 0)
        return wxTopLevelWindows.GetFirst()->GetData();
    else
        return NULL;
}

void wxApp::SetTopWindow( wxWindow *win )
{
    m_topWindow = win;
}

bool wxApp::Initialize()
{
    wxBuffer = new wxChar[BUFSIZ + 512];

    wxClassInfo::InitializeClasses();

    wxSystemSettings::Init();

    // GL: I'm annoyed ... I don't know where to put this and I don't want to
    // create a module for that as it's part of the core.
#if wxUSE_THREADS
    wxPendingEvents = new wxList();
    wxPendingEventsLocker = new wxCriticalSection();
#endif

/*
    wxTheFontNameDirectory =  new wxFontNameDirectory;
    wxTheFontNameDirectory->Initialize();
*/

    wxTheColourDatabase = new wxColourDatabase( wxKEY_STRING );
    wxTheColourDatabase->Initialize();

    wxInitializeStockLists();
    wxInitializeStockObjects();

#if wxUSE_WX_RESOURCES
    wxTheResourceCache = new wxResourceCache( wxKEY_STRING );

    wxInitializeResourceSystem();
#endif

    wxImage::InitStandardHandlers();

    wxModule::RegisterModules();
    if (!wxModule::InitializeModules()) return FALSE;

    return TRUE;
}

void wxApp::CleanUp()
{
    wxModule::CleanUpModules();

#if wxUSE_WX_RESOURCES
    wxFlushResources();

    if (wxTheResourceCache)
        delete wxTheResourceCache;
    wxTheResourceCache = (wxResourceCache*) NULL;

    wxCleanUpResourceSystem();
#endif

    if (wxTheColourDatabase)
        delete wxTheColourDatabase;
    wxTheColourDatabase = (wxColourDatabase*) NULL;

/*
    if (wxTheFontNameDirectory) delete wxTheFontNameDirectory;
    wxTheFontNameDirectory = (wxFontNameDirectory*) NULL;
*/

    wxDeleteStockObjects();

    wxDeleteStockLists();

    wxImage::CleanUpHandlers();

    delete wxTheApp;
    wxTheApp = (wxApp*) NULL;

    // GL: I'm annoyed ... I don't know where to put this and I don't want to
    // create a module for that as it's part of the core.
#if wxUSE_THREADS
    delete wxPendingEvents;
    delete wxPendingEventsLocker;
#endif

    wxSystemSettings::Done();

    delete[] wxBuffer;

    wxClassInfo::CleanUpClasses();

    // check for memory leaks
#if (defined(__WXDEBUG__) && wxUSE_MEMORY_TRACING) || wxUSE_DEBUG_CONTEXT
    if (wxDebugContext::CountObjectsLeft() > 0)
    {
        wxLogDebug(_T("There were memory leaks.\n"));
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
}

wxLog *wxApp::CreateLogTarget()
{
    return new wxLogGui;
}
#endif // wxUSE_LOG

//-----------------------------------------------------------------------------
// wxEntry
//-----------------------------------------------------------------------------

int wxEntry( int argc, char *argv[] )
{
    gtk_set_locale();

    if (!wxOKlibc()) wxConvCurrent = &wxConvLocal;

    gtk_init( &argc, &argv );

    wxSetDetectableAutoRepeat( TRUE );
    
    if (!wxApp::Initialize())
        return -1;

    if (!wxTheApp)
    {
        wxCHECK_MSG( wxApp::GetInitializerFunction(), -1,
                     _T("wxWindows error: No initializer - use IMPLEMENT_APP macro.\n") );

        wxAppInitializerFunction app_ini = wxApp::GetInitializerFunction();

        wxObject *test_app = app_ini();

        wxTheApp = (wxApp*) test_app;
    }

    wxCHECK_MSG( wxTheApp, -1, _T("wxWindows error: no application object") );

    wxTheApp->argc = argc;
    wxTheApp->argv = argv;

    wxString name(wxFileNameFromPath(argv[0]));
    wxStripExtension( name );
    wxTheApp->SetAppName( name );

    int retValue = 0;

    if ( !wxTheApp->OnInitGui() )
        retValue = -1;

    // Here frames insert themselves automatically into wxTopLevelWindows by
    // getting created in OnInit().
    if ( retValue == 0 )
    {
        if ( !wxTheApp->OnInit() )
            retValue = -1;
    }

    if ( retValue == 0 )
    {
        /* delete pending toplevel windows (typically a single
	   dialog) so that, if there isn't any left, we don't
	   call OnRun() */
        wxTheApp->DeletePendingObjects();
	
        wxTheApp->m_initialized = wxTopLevelWindows.GetCount() != 0;

        if (wxTheApp->Initialized())
	{
            retValue = wxTheApp->OnRun();

            wxWindow *topWindow = wxTheApp->GetTopWindow();
            if (topWindow)
            {
                /* Forcibly delete the window. */
                if (topWindow->IsKindOf(CLASSINFO(wxFrame)) ||
                    topWindow->IsKindOf(CLASSINFO(wxDialog)) )
                {
                    topWindow->Close( TRUE );
                    wxTheApp->DeletePendingObjects();
                }
                else
                {
                    delete topWindow;
                    wxTheApp->SetTopWindow( (wxWindow*) NULL );
                }
	    }
            wxTheApp->OnExit();
        }
    }

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

    return retValue;
}

