/////////////////////////////////////////////////////////////////////////////
// Name:        app.cpp
// Purpose:
// Author:      Robert Roebling
// Created:     01/02/97
// Id:
// Copyright:   (c) 1998 Robert Roebling, Julian Smart and Markus Holzem
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "app.h"
#endif

#include "wx/app.h"
#include "wx/gdicmn.h"
#include "wx/utils.h"
#include "wx/postscrp.h"
#include "wx/intl.h"
#include "wx/log.h"

#include "unistd.h"

#ifdef USE_GDK_IMLIB
#include "gdk_imlib.h"
#endif

//-----------------------------------------------------------------------------
// global data
//-----------------------------------------------------------------------------

wxApp *wxTheApp = NULL;
wxAppInitializerFunction wxApp::m_appInitFn = (wxAppInitializerFunction) NULL;

extern wxList wxPendingDelete;

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
};

bool wxYield(void)
{
  while (gtk_events_pending() > 0) gtk_main_iteration();
  return TRUE;
};

//-----------------------------------------------------------------------------
// wxApp
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxApp,wxEvtHandler)

gint wxapp_idle_callback( gpointer WXUNUSED(data) )
{
  if (wxTheApp) wxTheApp->OnIdle();
  usleep( 10000 );
  return TRUE;
};

wxApp::wxApp()
{
  m_idleTag = 0;
  m_topWindow = NULL;
  m_exitOnFrameDelete = TRUE;
};

wxApp::~wxApp(void)
{
  gtk_idle_remove( m_idleTag );
};

bool wxApp::OnInit(void)
{
  return TRUE;
};

bool wxApp::OnInitGui(void)
{ 
  m_idleTag = gtk_idle_add( wxapp_idle_callback, NULL );
  return TRUE; 
};

int wxApp::OnRun(void) 
{ 
  return MainLoop(); 
};

bool wxApp::OnIdle(void)
{
  DeletePendingObjects();
  return FALSE;
};

int wxApp::OnExit(void)
{
  return 0;
};

int wxApp::MainLoop(void)
{
  gtk_main();
  return 0;
};

void wxApp::ExitMainLoop(void)
{
  gtk_main_quit();
};

bool wxApp::Initialized(void)
{
  return m_initialized;
};

bool wxApp::Pending(void) 
{
  return FALSE;
};

void wxApp::Dispatch(void) 
{
};

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
  };
};

wxWindow *wxApp::GetTopWindow(void)
{
  if (m_topWindow) return m_topWindow;
  wxNode *node = wxTopLevelWindows.First();
  if (!node) return NULL;
  return (wxWindow*)node->Data();
};

void wxApp::SetTopWindow( wxWindow *win )
{
  m_topWindow = win;
};

void wxApp::CommonInit(void)
{

/*
#if USE_RESOURCES
  (void) wxGetResource("wxWindows", "OsVersion", &wxOsVersion);
#endif
*/

  wxTheColourDatabase = new wxColourDatabase(wxKEY_STRING);
  wxTheColourDatabase->Initialize();
  wxInitializeStockObjects();

  // For PostScript printing
#if USE_POSTSCRIPT
  wxInitializePrintSetupData();
  wxThePrintPaperDatabase = new wxPrintPaperDatabase;
  wxThePrintPaperDatabase->CreateDatabase();
#endif


/*
  wxBitmap::InitStandardHandlers();

  g_globalCursor = new wxCursor;
*/

  wxInitializeStockObjects();
};

void wxApp::CommonCleanUp(void)
{
  wxDeleteStockObjects();
  
  wxFlushResources();
};
    
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
    };
    
    wxAppInitializerFunction app_ini = wxApp::GetInitializerFunction();
    
    wxObject *test_app = app_ini();
    
    wxTheApp = (wxApp*) test_app;
    
//    wxTheApp = (wxApp*)( app_ini() );
  };
  
  if (!wxTheApp) 
  {
    printf( "wxWindows error: wxTheApp == NULL\n" );
    return 0;
  };

//  printf( "Programmstart.\n" );
  
  wxTheApp->argc = argc;
  wxTheApp->argv = argv;
  
  gtk_init( &argc, &argv );

#ifdef USE_GDK_IMLIB

  gdk_imlib_init();
  
  gtk_widget_push_visual(gdk_imlib_get_visual());
  
  gtk_widget_push_colormap(gdk_imlib_get_colormap());
  
#endif  
    
  wxApp::CommonInit();

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
  
  wxApp::CommonCleanUp();
  
  return retValue;
};
