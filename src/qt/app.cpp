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
#include "wx/memory.h"

#include "unistd.h"

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
};

bool wxYield(void)
{
  return TRUE;
};

//-----------------------------------------------------------------------------
// wxApp
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxApp,wxEvtHandler)

BEGIN_EVENT_TABLE(wxApp, wxEvtHandler)
    EVT_IDLE(wxApp::OnIdle)
END_EVENT_TABLE()

wxApp::wxApp()
{
  m_topWindow = NULL;
  m_exitOnFrameDelete = TRUE;
};

wxApp::~wxApp(void)
{
};

bool wxApp::OnInit(void)
{
  return TRUE;
};

bool wxApp::OnInitGui(void)
{ 
  return TRUE; 
};

int wxApp::OnRun(void) 
{ 
  return MainLoop(); 
};

bool wxApp::ProcessIdle(void)
{
  wxIdleEvent event;
  event.SetEventObject( this );
  ProcessEvent( event );
  
  return event.MoreRequested();
};

void wxApp::OnIdle( wxIdleEvent &event )
{
  static bool inOnIdle = FALSE;

  // Avoid recursion (via ProcessEvent default case)
  if (inOnIdle)
    return;

  inOnIdle = TRUE;

  // 'Garbage' collection of windows deleted with Close().
  DeletePendingObjects();

  // flush the logged messages if any
  wxLog *pLog = wxLog::GetActiveTarget();
  if ( pLog != NULL && pLog->HasPendingMessages() )
    pLog->Flush();

  // Send OnIdle events to all windows
  bool needMore = SendIdleEvents();

  if (needMore)
    event.RequestMore(TRUE);

  inOnIdle = FALSE;
};

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
};

bool wxApp::SendIdleEvents( wxWindow* win )
{
    bool needMore = FALSE;

	wxIdleEvent event;
	event.SetEventObject(win);
	win->ProcessEvent(event);

    if (event.MoreRequested())
        needMore = TRUE;

	wxNode* node = win->GetChildren()->First();
	while (node)
	{
		wxWindow* win = (wxWindow*) node->Data();
		if (SendIdleEvents(win))
            needMore = TRUE;

		node = node->Next();
	}
    return needMore ;
};

int wxApp::OnExit(void)
{
  return 0;
};

int wxApp::MainLoop(void)
{
  return 0;
};

void wxApp::ExitMainLoop(void)
{
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
  
#if (WXDEBUG && USE_MEMORY_TRACING) || USE_DEBUG_CONTEXT

#if !defined(_WINDLL)
  streambuf* sBuf = new wxDebugStreamBuf;
#else
  streambuf* sBuf = NULL;
#endif
  ostream* oStr = new ostream(sBuf) ;
  wxDebugContext::SetStream(oStr, sBuf);

#endif
  
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

// Your init here  !!!!
    
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
  
#if (WXDEBUG && USE_MEMORY_TRACING) || USE_DEBUG_CONTEXT
  // At this point we want to check if there are any memory
  // blocks that aren't part of the wxDebugContext itself,
  // as a special case. Then when dumping we need to ignore
  // wxDebugContext, too.
  if (wxDebugContext::CountObjectsLeft() > 0)
  {
    wxTrace("There were memory leaks.\n");
    wxDebugContext::Dump();
    wxDebugContext::PrintStatistics();
  }
  wxDebugContext::SetStream(NULL, NULL);
#endif
  
  return retValue;
};

//-----------------------------------------------------------------------------
// main()
//-----------------------------------------------------------------------------

#if defined(AIX) || defined(AIX4) || defined(____HPUX__)

 // main in IMPLEMENT_WX_MAIN in IMPLEMENT_APP in app.h

#else

  int main(int argc, char *argv[]) { return wxEntry(argc, argv); }

#endif


