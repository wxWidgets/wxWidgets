/////////////////////////////////////////////////////////////////////////////
// Name:        app.cpp
// Purpose:     wxApp
// Author:      AUTHOR
// Modified by:
// Created:     ??/??/98
// RCS-ID:      $Id$
// Copyright:   (c) AUTHOR
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "app.h"
#endif

#include "wx/frame.h"
#include "wx/app.h"
#include "wx/utils.h"
#include "wx/gdicmn.h"
#include "wx/pen.h"
#include "wx/brush.h"
#include "wx/cursor.h"
#include "wx/icon.h"
#include "wx/palette.h"
#include "wx/dc.h"
#include "wx/dialog.h"
#include "wx/msgdlg.h"
#include "wx/log.h"
#include "wx/module.h"

#if wxUSE_WX_RESOURCES
#include "wx/resource.h"
#endif

#include <string.h>

#if defined(__WIN95__) && !defined(__GNUWIN32__)
extern char *wxBuffer;
extern wxList wxPendingDelete;

wxApp *wxTheApp = NULL;

IMPLEMENT_DYNAMIC_CLASS(wxApp, wxEvtHandler)
BEGIN_EVENT_TABLE(wxApp, wxEvtHandler)
    EVT_IDLE(wxApp::OnIdle)
END_EVENT_TABLE()

long wxApp::sm_lastMessageTime = 0;

void wxApp::CommonInit()
{
#ifdef __WXMSW__
  wxBuffer = new char[1500];
#else
  wxBuffer = new char[BUFSIZ + 512];
#endif

  wxClassInfo::InitializeClasses();

  wxTheColourDatabase = new wxColourDatabase(wxKEY_STRING);
  wxTheColourDatabase->Initialize();
  wxInitializeStockObjects();

#if wxUSE_WX_RESOURCES
  wxInitializeResourceSystem();
#endif

  // For PostScript printing
#if wxUSE_POSTSCRIPT
  wxInitializePrintSetupData();
  wxThePrintPaperDatabase = new wxPrintPaperDatabase;
  wxThePrintPaperDatabase->CreateDatabase();
#endif

  wxBitmap::InitStandardHandlers();

  wxModule::RegisterModules();
  wxASSERT( wxModule::InitializeModules() == TRUE );
}

void wxApp::CommonCleanUp()
{
  wxModule::CleanUpModules();

#if wxUSE_WX_RESOURCES
  wxCleanUpResourceSystem();
#endif

  wxDeleteStockObjects() ;

  // Destroy all GDI lists, etc.
  delete wxTheBrushList;
  wxTheBrushList = NULL;

  delete wxThePenList;
  wxThePenList = NULL;

  delete wxTheFontList;
  wxTheFontList = NULL;

  delete wxTheBitmapList;
  wxTheBitmapList = NULL;

  delete wxTheColourDatabase;
  wxTheColourDatabase = NULL;

#if wxUSE_POSTSCRIPT
  wxInitializePrintSetupData(FALSE);
  delete wxThePrintPaperDatabase;
  wxThePrintPaperDatabase = NULL;
#endif

  wxBitmap::CleanUpHandlers();

  delete[] wxBuffer;
  wxBuffer = NULL;

  // do it as the very last thing because everything else can log messages
  delete wxLog::SetActiveTarget(NULL);
}

int wxEntry( int argc, char *argv[] )
{
  wxClassInfo::InitializeClasses();
  
#if (WXDEBUG && wxUSE_MEMORY_TRACING) || wxUSE_DEBUG_CONTEXT

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
    
  	wxTheApp = (* wxApp::GetInitializerFunction()) ();
  };
  
  if (!wxTheApp) 
  {
    printf( "wxWindows error: wxTheApp == NULL\n" );
    return 0;
  };

  wxTheApp->argc = argc;
  wxTheApp->argv = argv;

  // TODO: your platform-specific initialization.

  wxApp::CommonInit();

  // GUI-specific initialization, such as creating an app context.
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
  
#if (WXDEBUG && wxUSE_MEMORY_TRACING) || wxUSE_DEBUG_CONTEXT
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

// Static member initialization
wxAppInitializerFunction wxApp::m_appInitFn = (wxAppInitializerFunction) NULL;

wxApp::wxApp()
{
  m_topWindow = NULL;
  wxTheApp = this;
  m_className = "";
  m_wantDebugOutput = TRUE ;
  m_appName = "";
  argc = 0;
  argv = NULL;
#ifdef __WXMSW__
  m_printMode = wxPRINT_WINDOWS;
#else
  m_printMode = wxPRINT_POSTSCRIPT;
#endif
  m_exitOnFrameDelete = TRUE;
  m_auto3D = TRUE;
}

bool wxApp::Initialized()
{
  if (GetTopWindow())
    return TRUE;
  else
    return FALSE;
}

int wxApp::MainLoop()
{
  m_keepGoing = TRUE;

/* TODO: implement your main loop here, calling ProcessIdle in idle time.
  while (m_keepGoing)
  {
    while (!::PeekMessage(&s_currentMsg, 0, 0, 0, PM_NOREMOVE) &&
           ProcessIdle()) {}
    if (!DoMessage())
      m_keepGoing = FALSE;
  }
*/

  return 0;
}

// Returns TRUE if more time is needed.
bool wxApp::ProcessIdle()
{
    wxIdleEvent event;
    event.SetEventObject(this);
    ProcessEvent(event);

    return event.MoreRequested();
}

void wxApp::ExitMainLoop()
{
  m_keepGoing = FALSE;
}

// Is a message/event pending?
bool wxApp::Pending()
{
/* TODO.
 */
  return FALSE;
}

// Dispatch a message.
void wxApp::Dispatch()
{
/* TODO.
 */
}

void wxApp::OnIdle(wxIdleEvent& event)
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
}

// Send idle event to all top-level windows
bool wxApp::SendIdleEvents()
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

// Send idle event to window and all subwindows
bool wxApp::SendIdleEvents(wxWindow* win)
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
}

void wxApp::DeletePendingObjects()
{
  wxNode *node = wxPendingDelete.First();
  while (node)
  {
    wxObject *obj = (wxObject *)node->Data();
    
    delete obj;

    if (wxPendingDelete.Member(obj))
      delete node;

    // Deleting one object may have deleted other pending
    // objects, so start from beginning of list again.
    node = wxPendingDelete.First();
  }
}

wxLog* wxApp::CreateLogTarget()
{
    return new wxLogGui;
}

wxWindow* wxApp::GetTopWindow() const
{
    if (m_topWindow)
        return m_topWindow;
    else if (wxTopLevelWindows.Number() > 0)
        return (wxWindow*) wxTopLevelWindows.First()->Data();
    else
        return NULL;
}

void wxExit()
{
  wxApp::CommonCleanUp();
/*
 * TODO: Exit in some platform-specific way. Not recommended that the app calls this:
 * only for emergencies.
 */
}

// Yield to other processes
bool wxYield()
{
  /*
   * TODO
   */
  return TRUE;
}

