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
#ifdef wxUSE_WX_RESOURCES
#include "wx/resource.h"
#endif
#include "wx/module.h"
#include "wx/image.h"
#ifdef wxUSE_THREADS
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

extern wxList wxPendingDelete;
extern wxResourceCache *wxTheResourceCache;

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

void wxExit(void)
{
    gtk_main_quit();
}

bool wxYield(void)
{
    while (gtk_events_pending() > 0) gtk_main_iteration();
    return TRUE;
}

//-----------------------------------------------------------------------------
// wxApp
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxApp,wxEvtHandler)

BEGIN_EVENT_TABLE(wxApp, wxEvtHandler)
    EVT_IDLE(wxApp::OnIdle)
END_EVENT_TABLE()

gint wxapp_idle_callback( gpointer WXUNUSED(data) )
{
    if (wxTheApp) while (wxTheApp->ProcessIdle()) {}
#ifdef wxUSE_THREADS
    wxMutexGuiLeave();
#endif
    usleep(10000);
#ifdef wxUSE_THREADS
    wxMutexGuiEnter();
#endif
    return TRUE;
}

wxApp::wxApp()
{
    m_idleTag = 0;
    m_topWindow = (wxWindow *) NULL;
    m_exitOnFrameDelete = TRUE;
    wxTheApp = this;
}

wxApp::~wxApp(void)
{
    gtk_idle_remove( m_idleTag );
}

bool wxApp::OnInit(void)
{
    return TRUE;
}

bool wxApp::OnInitGui(void)
{
    m_idleTag = gtk_idle_add( wxapp_idle_callback, NULL );
    return TRUE;
}

int wxApp::OnRun(void)
{
    return MainLoop();
}

bool wxApp::ProcessIdle(void)
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

    /* 'Garbage' collection of windows deleted with Close(). */
    DeletePendingObjects();

    /* flush the logged messages if any */
    wxLog *log = wxLog::GetActiveTarget();
    if (log != NULL && log->HasPendingMessages())
        log->Flush();

    /* Send OnIdle events to all windows */
    bool needMore = SendIdleEvents();

    if (needMore)
        event.RequestMore(TRUE);

    inOnIdle = FALSE;
}

bool wxApp::SendIdleEvents(void)
{
    bool needMore = FALSE;

    wxNode* node = wxTopLevelWindows.First();
    while (node)
    {
        wxWindow* win = (wxWindow*) node->Data();
        if (SendIdleEvents(win))
            needMore = TRUE;
        node = node->Next();
    }
    return needMore;
}

bool wxApp::SendIdleEvents( wxWindow* win )
{
    bool needMore = FALSE;

    wxIdleEvent event;
    event.SetEventObject(win);
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

int wxApp::OnExit(void)
{
    return 0;
}

int wxApp::MainLoop(void)
{
    gtk_main();
    return 0;
}

void wxApp::ExitMainLoop(void)
{
    gtk_main_quit();
}

bool wxApp::Initialized(void)
{
    return m_initialized;
}

bool wxApp::Pending(void)
{
    return FALSE;
}

void wxApp::Dispatch(void)
{
}

void wxApp::DeletePendingObjects(void)
{
    wxNode *node = wxPendingDelete.First();
    while (node)
    {
        wxObject *obj = (wxObject *)node->Data();

        delete obj;

        if (wxPendingDelete.Member(obj))
        delete node;

        node = wxPendingDelete.First();
    }
}

wxWindow *wxApp::GetTopWindow(void)
{
    if (m_topWindow) return m_topWindow;
    wxNode *node = wxTopLevelWindows.First();
    if (!node) return (wxWindow *) NULL;
    return (wxWindow*)node->Data();
}

void wxApp::SetTopWindow( wxWindow *win )
{
    m_topWindow = win;
}

void wxApp::CommonInit(void)
{

/*
#if wxUSE_RESOURCES
  (void) wxGetResource("wxWindows", "OsVersion", &wxOsVersion);
#endif
*/
  wxSystemSettings::Init();
  
  wxTheFontNameDirectory =  new wxFontNameDirectory;
  wxTheFontNameDirectory->Initialize();

  wxTheColourDatabase = new wxColourDatabase(wxKEY_STRING);
  wxTheColourDatabase->Initialize();

  wxInitializeStockLists();
  wxInitializeStockObjects();

#ifdef wxUSE_WX_RESOURCES
  wxTheResourceCache = new wxResourceCache(wxKEY_STRING);
  
  wxInitializeResourceSystem();
#endif

  wxImage::InitStandardHandlers();

//  g_globalCursor = new wxCursor;
}

void wxApp::CommonCleanUp(void)
{
    wxDELETE(wxTheColourDatabase);
    wxDELETE(wxTheFontNameDirectory);
    wxDeleteStockObjects();

#ifdef wxUSE_WX_RESOURCES
    wxFlushResources();

    wxDELETE(wxTheResourceCache);
    
    wxCleanUpResourceSystem();
#endif

    wxDeleteStockLists();

    wxImage::CleanUpHandlers();

    wxSystemSettings::Done();
}

wxLog *wxApp::CreateLogTarget()
{
  return new wxLogGui;
}

//-----------------------------------------------------------------------------
// wxEntry
//-----------------------------------------------------------------------------

int wxEntry( int argc, char *argv[] )
{
    wxBuffer = new char[BUFSIZ + 512];

    wxClassInfo::InitializeClasses();


    if (!wxTheApp)
    {
        if (!wxApp::GetInitializerFunction())
        {
            printf( "wxWindows error: No initializer - use IMPLEMENT_APP macro.\n" );
            return 0;
        }

        wxAppInitializerFunction app_ini = wxApp::GetInitializerFunction();

        wxObject *test_app = app_ini();

        wxTheApp = (wxApp*) test_app;
    }

    if (!wxTheApp)
    {
        printf( "wxWindows error: wxTheApp == NULL\n" );
        return 0;
    }

    wxTheApp->argc = argc;
    wxTheApp->argv = argv;

    char name[200];
    strcpy( name, argv[0] );
    strcpy( name, wxFileNameFromPath(name) );
    wxStripExtension( name );
    wxTheApp->SetAppName( name );

    gtk_set_locale();

    gtk_init( &argc, &argv );

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

    gtk_widget_push_colormap( cmap );

    gtk_widget_set_default_colormap( cmap );

    wxApp::CommonInit();

    wxModule::RegisterModules();
    if (!wxModule::InitializeModules()) return FALSE;

    wxTheApp->OnInitGui();

    // Here frames insert themselves automatically
    // into wxTopLevelWindows by getting created
    // in OnInit().

    if (!wxTheApp->OnInit()) return 0;

    wxTheApp->m_initialized = (wxTopLevelWindows.Number() > 0);

    int retValue = 0;

    if (wxTheApp->Initialized()) retValue = wxTheApp->OnRun();

    wxTheApp->DeletePendingObjects();

    wxTheApp->OnExit();

    wxModule::CleanUpModules();

    wxApp::CommonCleanUp();

    delete wxTheApp;
    wxTheApp = (wxApp*) NULL;

    wxClassInfo::CleanUpClasses();

    delete[] wxBuffer;

#if (defined(__WXDEBUG__) && wxUSE_MEMORY_TRACING) || wxUSE_DEBUG_CONTEXT

    if (wxDebugContext::CountObjectsLeft() > 0)
    {
        wxLogDebug("There were memory leaks.\n");
        wxDebugContext::Dump();
        wxDebugContext::PrintStatistics();
    }

#endif

    wxLog *oldLog = wxLog::SetActiveTarget( NULL );
    if (oldLog) delete oldLog;

    return retValue;
}

//-----------------------------------------------------------------------------

