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
#include "wx/memory.h"

#if wxUSE_WX_RESOURCES
#include "wx/resource.h"
#endif

#include <string.h>

extern char *wxBuffer;
extern wxList wxPendingDelete;

wxApp *wxTheApp = NULL;

#if !USE_SHARED_LIBRARY
IMPLEMENT_DYNAMIC_CLASS(wxApp, wxEvtHandler)
BEGIN_EVENT_TABLE(wxApp, wxEvtHandler)
    EVT_IDLE(wxApp::OnIdle)
END_EVENT_TABLE()
#endif

// platform layer

typedef void * PLEventHandle ;

int PLTestMinimalRequirements() ;
void PLErrorMessage( int errorCode ) ;
int PLStartupPhase1() ;
int PLStartupPhase2() ;
void PLCleanup() ;

bool PLDoOneEvent() ;
bool PLHandleOneEvent( PLEventHandle event ) ; // true if really event
bool PLCallbackIdle() ;
bool PLCallbackRepeat() ;

long wxApp::sm_lastMessageTime = 0;

bool wxApp::Initialize()
{
  int error = 0 ;
	
  error = PLStartupPhase1() ;
  if ( !error )
  {
    error = PLTestMinimalRequirements() ;
    if ( !error )
      error = PLStartupPhase2() ;
  }
	
  if ( error )
  {
    PLErrorMessage( error ) ;
    return FALSE ;
  }  
  
#ifdef __WXMSW__
  wxBuffer = new char[1500];
#else
  wxBuffer = new char[BUFSIZ + 512];
#endif

/* No longer used
#if (defined(__WXDEBUG__) && wxUSE_MEMORY_TRACING) || wxUSE_DEBUG_CONTEXT

  streambuf* sBuf = new wxDebugStreamBuf;
  ostream* oStr = new ostream(sBuf) ;
  wxDebugContext::SetStream(oStr, sBuf);
#endif
*/
  
  wxClassInfo::InitializeClasses();

  wxTheColourDatabase = new wxColourDatabase(wxKEY_STRING);
  wxTheColourDatabase->Initialize();

  wxInitializeStockLists();
  wxInitializeStockObjects();

#if wxUSE_WX_RESOURCES
  wxInitializeResourceSystem();
#endif

  wxBitmap::InitStandardHandlers();

  wxModule::RegisterModules();
  wxASSERT( wxModule::InitializeModules() == TRUE );

  return TRUE;
}

void wxApp::CleanUp()
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

  wxBitmap::CleanUpHandlers();

  delete[] wxBuffer;
  wxBuffer = NULL;

  wxClassInfo::CleanUpClasses();

  delete wxTheApp;
  wxTheApp = NULL;
  
#if (defined(__WXDEBUG__) && wxUSE_MEMORY_TRACING) || wxUSE_DEBUG_CONTEXT
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
//  wxDebugContext::SetStream(NULL, NULL);
#endif
  
  // do it as the very last thing because everything else can log messages
  wxLog::DontCreateOnDemand();
  // do it as the very last thing because everything else can log messages
  delete wxLog::SetActiveTarget(NULL);

	PLCleanup() ;
}

int wxEntry( int argc, char *argv[] )
{
  if (!wxApp::Initialize())
    return FALSE;
  if (!wxTheApp)
  {
    if (!wxApp::GetInitializerFunction())
    {
      printf( "wxWindows error: No initializer - use IMPLEMENT_APP macro.\n" );
      return 0;
    };
    
  	wxTheApp = (wxApp*) (* wxApp::GetInitializerFunction()) ();
  };
  
  if (!wxTheApp) 
  {
    printf( "wxWindows error: wxTheApp == NULL\n" );
    return 0;
  };

  wxTheApp->argc = argc;
  wxTheApp->argv = argv;

  // GUI-specific initialization, such as creating an app context.
  wxTheApp->OnInitGui();

  // Here frames insert themselves automatically
  // into wxTopLevelWindows by getting created
  // in OnInit().
  
  if (!wxTheApp->OnInit()) return 0;

  int retValue = 0;
  
  if (wxTheApp->Initialized()) retValue = wxTheApp->OnRun();

  if (wxTheApp->GetTopWindow())
  {
    delete wxTheApp->GetTopWindow();
    wxTheApp->SetTopWindow(NULL);
  }
  
  wxTheApp->DeletePendingObjects();  
  
  wxTheApp->OnExit();
  
  wxApp::CleanUp();

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
    return TRUE;
//  if (GetTopWindow())
//    return TRUE;
//  else
//    return FALSE;
}

int wxApp::MainLoop()
{
  m_keepGoing = TRUE;

  while (m_keepGoing)
  {
		PLDoOneEvent() ;
  }

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
  wxApp::CleanUp();
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

// -------------------------------------------------------------------
// Portability Layer PL
// -------------------------------------------------------------------
// this is the c-api part, the only part of this file that needs to be
// adapted for supporting a new platform
// there are two flavours of PL... functions, Callbacks and normal functions
// Callbacks are called by other PLxxx functions and allow to trigger idle 
// processing etc. the callbacks don't have to be adapted for every platform
// but only in case of changes to the underlying wx framework

// callbacks

bool PLCallbackIdle() 
{
	return wxTheApp->ProcessIdle() ;
}

bool PLCallbackRepeat() 
{
	// wxMacProcessSocketEvents() ;  
	return false ;
}

// platform specific static variables

bool					gMacHasAppearance = false ;
long					gMacAppearanceVersion = 0 ;
RgnHandle			gMacCursorRgn = NULL ;

#define	kMinHeap				 (29 * 1024)
#define	kMinSpace				(20 * 1024)
#define	eWrongMachine			1
#define	eSmallSize				2
#define	eNoMemory				3
#define	eOldSystem				4
#define	eGenericAbout			5

// platform specific prototypes

void DoMacNullEvent( EventRecord *ev ) ;
void DoMacHighLevelEvent( EventRecord *ev ) ;
void DoMacMouseDownEvent( EventRecord *ev ) ;
void DoMacMouseUpEvent( EventRecord *ev ) ;
void DoMacKeyDownEvent( EventRecord *ev ) ;
void DoMacKeyUpEvent( EventRecord *ev ) ;
void DoMacAutoKeyEvent( EventRecord *ev ) ;
void DoMacActivateEvent( EventRecord *ev ) ;
void DoMacUpdateEvent( EventRecord *ev ) ;
void DoMacDiskEvent( EventRecord *ev ) ;
void DoMacOSEvent( EventRecord *ev ) ;

// platform specific functions

// -------------------------------------------------------------------
// PLStartupPhase1
// -------------------------------------------------------------------
// Initializes the system so that at least the requirements can be tested
// and that error messages will shop up at all ;-)
// 
// parameters   : none
// return value : non zero for a implementation specific error code

int PLStartupPhase1()
{
	::InitGraf(&qd.thePort);
	::InitFonts();
	::InitWindows();
	::InitMenus();
	::TEInit();
	::InitDialogs(0L);
	::InitCursor();
	CursHandle aCursHandle = ::GetCursor(watchCursor);	// Watch should be in system
	if (aCursHandle)
		::SetCursor(*aCursHandle);						// Change cursor to watch 
	::FlushEvents(everyEvent, 0);
	
	gMacCursorRgn = ::NewRgn() ;

	return 0 ;
}

// -------------------------------------------------------------------
// PLStartupPhase2
// -------------------------------------------------------------------
// booting the system further until all subsystems are running
// 
// parameters   : none
// return value : non zero for a implementation specific error code

int PLStartupPhase2()
{
	long total,contig;

	::MaxApplZone();
	for (long i = 1; i <= 4; i++)
		::MoreMasters();
	PurgeSpace(&total, &contig);
	::SetCursor( &qd.arrow ) ; 

#if 0
	InitAEHandlers();
	InitializeAECore() ;
	GUSISetup(GUSIwithInternetSockets);
#endif

	return 0 ;
}

// -------------------------------------------------------------------
// PLErrorMessage
// -------------------------------------------------------------------
// notifies the user of a implementation specific error
// is useful for messages before the wx System is up and running
// 
// parameters   : int error = error code (implementation specific)
// return value : none

void PLErrorMessage( int error )
{
	short itemHit;
	Str255 message;

	SetCursor(&qd.arrow);
	GetIndString(message, 128, error);
	ParamText(message, (ConstStr255Param)"\p", (ConstStr255Param)"\p", (ConstStr255Param)"\p");
	itemHit = Alert(129, nil);
}

// -------------------------------------------------------------------
// PLCleanup
// -------------------------------------------------------------------
// notifies the user of a implementation specific error
// is useful for messages before the wx System is up and running
// 
// parameters   : int error = error code (implementation specific)
// return value : none

void PLCleanup()
{
	::PrClose() ;
	if (gMacCursorRgn)
		::DisposeRgn(gMacCursorRgn);
#if 0
	TerminateAE() ;
#endif
}

// -------------------------------------------------------------------
// PLTestMinimalRequirements
// -------------------------------------------------------------------
// test whether we are on the correct runnable system and read out any
// useful informations from the system
// 
// parameters	: none
// return value : non zero for a implementation specific error code

int PLTestMinimalRequirements()
{
	long theSystem ;
	long theMachine;
	long theAppearance ;

	if (Gestalt(gestaltMachineType, &theMachine) != noErr)
	{
		return(eWrongMachine);
	}

	if (theMachine < gestaltMacPlus)
	{
		return(eWrongMachine);
	}

	if (Gestalt(gestaltSystemVersion, &theSystem) != noErr )
	{
		return( eOldSystem ) ;
	}
		
	if ( theSystem < 0x0700 )
	{
		return( eOldSystem ) ;
	}

	if ((long)GetApplLimit() - (long)ApplicationZone() < kMinHeap)
	{
		return(eSmallSize);
	}

	if ( Gestalt( gestaltAppearanceAttr, &theAppearance ) == noErr )
	{
		gMacHasAppearance = true ;
		RegisterAppearanceClient();
		if ( Gestalt( gestaltAppearanceVersion, &theAppearance ) == noErr )
		{
			gMacAppearanceVersion = theAppearance ;
		}
		else
		{
			gMacAppearanceVersion = 0x0100 ;
		}
	}

	return 0 ;
}

// -------------------------------------------------------------------
// PLDoOneEvent
// -------------------------------------------------------------------
// 
// parameters	: none
// return value : returns true if a real event occured (no null or timeout event)

bool PLDoOneEvent() 
{
  EventRecord event ;

	long sleepTime = 60;

	bool gotEvent = false ;
  

	if (WaitNextEvent(everyEvent, &event,sleepTime, gMacCursorRgn))
	{
    gotEvent = PLHandleOneEvent( &event );
	}
	else
	{
    PLCallbackIdle();
	}
		
	PLCallbackRepeat() ;
  
  return gotEvent ;
}

// -------------------------------------------------------------------
// PLHandleOneEvent
// -------------------------------------------------------------------
// 
// parameters	: event = event handle of the platform specific event to be handled
// return value : returns true if a real event occured (no null or timeout event)

bool PLHandleOneEvent( PLEventHandle event ) 
{
	bool realEvent = true ;

	EventRecord* ev = (EventRecord*) event ;

	switch (ev->what)
	{
		case nullEvent:
			DoMacNullEvent( ev ) ;
			realEvent = false ;
			break ;
		case kHighLevelEvent:
			DoMacHighLevelEvent( ev ) ;
			break;
		case mouseDown:
			DoMacMouseDownEvent( ev ) ;
			wxTheApp->ExitMainLoop() ;
			break;
		case mouseUp:
			DoMacMouseUpEvent( ev ) ;
			break;
		case keyDown:
			DoMacKeyDownEvent( ev ) ;
			break;
		case autoKey:
			DoMacAutoKeyEvent( ev ) ;
			break;
		case keyUp:
			DoMacKeyUpEvent( ev ) ;
			break;
		case activateEvt:
			DoMacActivateEvent( ev ) ;
			break;
		case updateEvt:
			DoMacUpdateEvent( ev ) ;
			break;
		case diskEvt:
			DoMacDiskEvent( ev ) ;
			break;
		case osEvt:
			DoMacOSEvent( ev ) ;
			break;
		default:
			break;
	}
  return realEvent ;
}

// platform specific functions (non PLxxx functions)

void DoMacNullEvent( EventRecord *ev ) 
{
}

void DoMacHighLevelEvent( EventRecord *ev )
{
}

void DoMacMouseDownEvent( EventRecord *ev )
{
}

void DoMacMouseUpEvent( EventRecord *ev )
{
}

void DoMacKeyDownEvent( EventRecord *ev )
{
}

void DoMacKeyUpEvent( EventRecord *ev )
{
}

void DoMacAutoKeyEvent( EventRecord *ev )
{
}

void DoMacActivateEvent( EventRecord *ev )
{
}

void DoMacUpdateEvent( EventRecord *ev )
{
}

void DoMacDiskEvent( EventRecord *ev )
{
}

void DoMacOSEvent( EventRecord *ev )
{
}



/*

void wxApp::doMacMouseDown(void)
{
	WindowPtr window;
	short windowPart = ::FindWindow(m_event.where, &window);
	if ( windowPart != inMenuBar )
	{
		WindowPtr frontWindow = FrontWindow();
		if (WindowIsModal(frontWindow) && (window != frontWindow))
			SysBeep(1);
		else
		{	
			switch (windowPart)
			{
				case inMenuBar:
					break;
				case inContent:
					doMacInContent(window); break;
				case inDrag:
					doMacInDrag(window); break;
				case inGrow:
					doMacInGrow(window); break;
				case inGoAway:
					doMacInGoAway(window); break;
				case inZoomIn:
				case inZoomOut:
					doMacInZoom(window, windowPart); break;
				default:
					break;
			}
	
		}
	}
	else
	{
		doMacInMenuBar(::MenuSelect(m_event.where));
	}
}

void wxApp::doMacMouseUp(void)
{
	if (m_mouseWindow)
	{
#if 0
		int hitX = m_event.where.h; 				// screen window c.s.
		int hitY = m_event.where.v; 				// screen window c.s.
		m_mouseWindow->ScreenToClient(&hitX, &hitY);		// mouseWindow client c.s.
		m_mouseWindow->ClientToLogical(&hitX, &hitY); 	// mouseWindow logical c.s.
#endif
		
		wxMouseEvent event(wxEVT_LEFT_UP);
		event.m_shiftDown = m_event.modifiers & shiftKey;
		event.m_controlDown = m_event.modifiers & controlKey;
		event.m_altDown = m_event.modifiers & optionKey;
		event.m_metaDown = m_event.modifiers & cmdKey;
		event.m_leftDown = FALSE;
		event.m_middleDown = FALSE;
		event.m_rightDown = FALSE;
		event.m_x = m_event.where.h;
		event.m_y = m_event.where.v;
		event.m_timeStamp = m_event.when;
		event.SetEventObject(m_mouseWindow);

		m_mouseWindow->ProcessEvent(event);
	}
	else
 	{
 		//??? Can't we just throw away mouse up events without matching mouse down
		wxWindow* theMacWxFrame = wxFrame::MacFindFrameOrDialog(::FrontWindow());
 		if (theMacWxFrame)
 		{
#if 0
 			int hitX = cCurrentEvent.where.h; // screen window c.s.
 			int hitY = cCurrentEvent.where.v; // screen window c.s.
			theMacWxFrame->ScreenToWindow(&hitX, &hitY);
#endif

			wxMouseEvent event(wxEVT_LEFT_UP);
			event.m_shiftDown = m_event.modifiers & shiftKey;
			event.m_controlDown = m_event.modifiers & controlKey;
			event.m_altDown = m_event.modifiers & optionKey;
			event.m_metaDown = m_event.modifiers & cmdKey;
			event.m_leftDown = FALSE;
			event.m_middleDown = FALSE;
			event.m_rightDown = FALSE;
			event.m_x = m_event.where.h;
			event.m_y = m_event.where.v;
			event.m_timeStamp = m_event.when;
			event.SetEventObject(m_mouseWindow);

			theMacWxFrame->ProcessEvent(event);
 		}
 	}
}

void wxApp::doMacMouseMotion(void)
{
	if (m_mouseWindow) {
		wxMouseEvent event(wxEVT_MOTION);
		event.m_shiftDown = m_event.modifiers & shiftKey;
		event.m_controlDown = m_event.modifiers & controlKey;
		event.m_altDown = m_event.modifiers & optionKey;
		event.m_metaDown = m_event.modifiers & cmdKey;
		event.m_leftDown = !(m_event.modifiers & btnState);
		event.m_middleDown = FALSE;
		event.m_rightDown = FALSE;
		event.m_x = m_event.where.h;
		event.m_y = m_event.where.v;
		event.m_timeStamp = m_event.when;
		event.SetEventObject(m_mouseWindow);

		m_mouseWindow->ProcessEvent(event);
	}
	else
 	{
		wxWindow* theMacWxFrame = wxFrame::MacFindFrameOrDialog(::FrontWindow());
 		if (theMacWxFrame)
 		{
			wxMouseEvent event(wxEVT_MOTION);
			event.m_shiftDown = m_event.modifiers & shiftKey;
			event.m_controlDown = m_event.modifiers & controlKey;
			event.m_altDown = m_event.modifiers & optionKey;
			event.m_metaDown = m_event.modifiers & cmdKey;
			event.m_leftDown = !(m_event.modifiers & btnState);
			event.m_middleDown = FALSE;
			event.m_rightDown = FALSE;
			event.m_x = m_event.where.h;
			event.m_y = m_event.where.v;
			event.m_timeStamp = m_event.when;
			event.SetEventObject(m_mouseWindow);

			m_mouseWindow->ProcessEvent(event);
		}
	}
	
	//??? Need to work with floating windows... isn't there a toolbox call to find the
	// top window intersecting a point is screen coordinates??
#if 0
	else	// will only work for one floating window at the moment... ?
	{	
		WindowPtr frontDocPtr = findFrontNonFloatingWindow();
		WindowPtr frontFloatingPtr = ::FrontWindow();
		
		int hitX = cCurrentEvent.where.h;
		int hitY = cCurrentEvent.where.v;
		
		wxFrame* macWxFrame = findMacWxFrame(frontDocPtr);
		
		if ((frontFloatingPtr != frontDocPtr) & (frontFloatingPtr != NULL))
		{
			RgnHandle frontFloatStrRgn = getStructureRegion(frontFloatingPtr);
			Rect 	  frontFloatRect = (**frontFloatStrRgn).rgnBBox;
	
			if 	   ((hitX >= frontFloatRect.left) & 
					(hitX <= frontFloatRect.right) & 
					(hitY >= frontFloatRect.top) &
					(hitY <= frontFloatRect.bottom))
			{
				macWxFrame = findMacWxFrame(frontFloatingPtr);
			}
		}
	}
#endif
}

void wxApp::doMacKeyDown(void)
{
	long menuResult = 0 ;
	short keycode ;
	short keychar ;
	keychar = short(m_event.message & charCodeMask);
	keycode = short(m_event.message & keyCodeMask) >> 8 ;

	// Handle menu accelerators
	if ( gSFMacHasAppearance )
	{
		menuResult = MenuEvent( &m_event ) ;
		if ( HiWord( menuResult ) )
		{
			doMacInMenuBar( menuResult ) ;
		}
		else
		{
			ControlHandle control ;
			
			GetKeyboardFocus( FrontNonFloatingWindow() , &control ) ;
			if ( control && keychar != 0x07 )
				HandleControlKey( control , keycode , keychar , m_event.modifiers ) ;
			else
			{
				wxWindow* theMacWxFrame = wxFrame::MacFindFrameOrDialog(::FrontWindow());
				if (theMacWxFrame)
				{
					wxKeyEvent event(wxEVT_CHAR);
					event.m_shiftDown = m_event.modifiers & shiftKey;
					event.m_controlDown = m_event.modifiers & controlKey;
					event.m_altDown = m_event.modifiers & optionKey;
					event.m_metaDown = m_event.modifiers & cmdKey;
					event.m_keyCode = macTranslateKey(keychar, m_event.modifiers & (shiftKey|optionKey));
					event.m_x = m_event.where.h;
					event.m_y = m_event.where.v;
					event.m_timeStamp = m_event.when;
					event.SetEventObject(theMacWxFrame);
			
					theMacWxFrame->ProcessEvent(event);
				}
			}
		}
	}
	else
	{
		if (GetMenuHandle( kwxMacAppleMenuId ) )
		{
//			menuResult = MDEF_MenuKey(m_event.message, m_event.modifiers , GetMenuHandle( kwxMacAppleMenuId ) );
		}
		else
		{
			if (m_event.modifiers & cmdKey)
			{
				menuResult = MenuKey( keychar ) ;
			}
		}

		if ( HiWord( menuResult ) )
		{
			doMacInMenuBar( menuResult ) ;
		}
		else
		{
			wxWindow* theMacWxFrame = wxFrame::MacFindFrameOrDialog(::FrontWindow());
			if (theMacWxFrame)
			{
				wxKeyEvent event(wxEVT_CHAR);
				event.m_shiftDown = m_event.modifiers & shiftKey;
				event.m_controlDown = m_event.modifiers & controlKey;
				event.m_altDown = m_event.modifiers & optionKey;
				event.m_metaDown = m_event.modifiers & cmdKey;
				event.m_keyCode = macTranslateKey(keychar, m_event.modifiers & (shiftKey|optionKey));
				event.m_x = m_event.where.h;
				event.m_y = m_event.where.v;
				event.m_timeStamp = m_event.when;
				event.SetEventObject(theMacWxFrame);
		
				theMacWxFrame->ProcessEvent(event);
			}
		}
	}
}

void wxApp::doMacAutoKey(void)
{
	doMacKeyDown();
}

void wxApp::doMacKeyUp(void)
{
}

void wxApp::doMacActivateEvt(void)
{
	HighlightAndActivateWindow( (WindowPtr) m_event.message , m_event.modifiers & activeFlag ) ;
}

void wxApp::doMacUpdateEvt(void)
{
	WindowPtr theMacWindow = (WindowPtr)(m_event.message);
	::BeginUpdate(theMacWindow);
	
	wxWindow* theMacWxFrame = wxFrame::MacFindFrameOrDialog(theMacWindow);
	if (theMacWxFrame)
	{
//		if (!::EmptyRgn(theMacWindow->visRgn))	// this doesn't work with windowshade
//		{
			if ( theMacWxFrame->MacSetupPort() )
			{
				// Erase update region
				// we must do this, because controls add their former rect to the inval
				// rgn and the background might not have been correct at that moment
				::EraseRect(&theMacWindow->portRect); 
	
	 			// Can't use UpdateControls since each control has it's own coordinate system
	 			//		::UpdateControls(theMacWindow, theMacWindow->visRgn);
	
				::UpdateControls( theMacWindow , theMacWindow->visRgn ) ;
	#if 0
	 			// Draw the grow box
	 			if (cStyle & wxRESIZE_BORDER)
	 				MacDrawGrowIcon();
	#endif
	
				wxPaintEvent event;
				event.m_timeStamp = m_event.when;
				event.SetEventObject(theMacWxFrame);
	
				theMacWxFrame->ProcessEvent(event);
	//			::SetThemeWindowBackground(  theMacWindow , kThemeActiveDialogBackgroundBrush ,  false ) ;
				::ClipRect( &theMacWindow->portRect ) ;		
				::SetOrigin(  0 , 0 );	
			}
			else
			{
				wxASSERT_MSG( false , "unabled to setup window mac port") ;
			}
			
//		}
	}

	::EndUpdate(theMacWindow);
}

void wxApp::doMacDiskEvt(void)
{ // based on "Programming for System 7" by Gary Little and Tim Swihart
	if ((m_event.message >> 16) != noErr)
	{
		const int kDILeft = 0x0050; // top coord for disk init dialog
		const int kDITop = 0x0070; // left coord for disk init dialog
		Point mountPoint;
		mountPoint.h = kDILeft;
		mountPoint.v = kDITop;
		int myError = DIBadMount(mountPoint, m_event.message);
	}
}

void wxApp::doMacOsEvt(void)
{ // based on "Programming for System 7" by Gary Little and Tim Swihart
	switch ((m_event.message >> 24) & 0x0ff)
	{
		case suspendResumeMessage:
			if (m_event.message & resumeFlag)
				doMacResumeEvent();
			else
				doMacSuspendEvent();
			break;
		case mouseMovedMessage:
			doMacMouseMovedMessage();
			break;
	}
}

void wxApp::doMacHighLevelEvent(void)
{
	::AEProcessAppleEvent(&m_event); // System 7 or higher
}

void wxApp::doMacResumeEvent(void)
{
	wxWindow* theMacWxFrame = wxFrame::MacFindFrameOrDialog(::FrontWindow());
	if (theMacWxFrame)
	{
		if (m_event.message & convertClipboardFlag)
			::TEFromScrap();

		wxActivateEvent event(wxEVT_ACTIVATE, TRUE);
		event.m_timeStamp = m_event.when;
		event.SetEventObject(theMacWxFrame);

		theMacWxFrame->ProcessEvent(event);
	}
}

void wxApp::doMacSuspendEvent(void)
{
	wxWindow* theMacWxFrame = wxFrame::MacFindFrameOrDialog(::FrontWindow());
	if (theMacWxFrame)
	{
		::ZeroScrap();
		::TEToScrap();

		wxActivateEvent event(wxEVT_ACTIVATE, FALSE);
		event.m_timeStamp = m_event.when;
		event.SetEventObject(theMacWxFrame);

		theMacWxFrame->ProcessEvent(event);
	}
}

void wxApp::doMacMouseMovedMessage(void)
{ // based on "Programming for System 7" by Gary Little and Tim Swihart
	if (m_cursorRgn)
		::DisposeRgn(m_cursorRgn);
	m_cursorRgn = ::NewRgn();
	::SetRectRgn(m_cursorRgn, -32768, -32768, 32766, 32766);
}

void wxApp::doMacInMenuBar(long menuResult)
{
	int macMenuId = HiWord(menuResult);
	int macMenuItemNum = LoWord(menuResult); // counting from 1

	if (macMenuId == 0) 					// no menu item selected;
		 return;
	if (macMenuId == 128) 
	{
		if (macMenuItemNum != 1) 
		{			// if not the "About" entry (or the separator)
			Str255		daName;
			::GetMenuItemText(GetMenuHandle(128), macMenuItemNum, daName);
			(void)::OpenDeskAcc(daName);
			::HiliteMenu(0);								
			return;
		}
	}

	wxWindow* theMacWxFrame = wxFrame::MacFindFrameOrDialog(::FrontWindow());
	if (theMacWxFrame)
	{
		if ( theMacWxFrame->IsKindOf( CLASSINFO( wxDialog ) ) )
			(( wxDialog *) theMacWxFrame)->MacMenuSelect(m_event, macMenuId, macMenuItemNum);
		else if ( theMacWxFrame->IsKindOf( CLASSINFO( wxFrame ) ) )
			(( wxFrame *) theMacWxFrame)->MacMenuSelect(m_event, macMenuId, macMenuItemNum);
	}
	::HiliteMenu(0);								
}

void wxApp::doMacInContent(WindowPtr window)
{
	WindowPtr frontWindow = FrontWindow();
	if (window != frontWindow )
	{
//		SFSelectWindow( window ) ;
	}
	else
	{
		ControlHandle	control ;
		Point		localwhere = m_event.where ;
		GrafPtr		port ;
		SInt16		controlpart ;
		
		::GetPort( &port ) ;
		::SetPort( window ) ;
		::GlobalToLocal( &localwhere ) ;

		::SetPort( port ) ;
		
		if ( !gSFMacHasAppearance )
		{
			controlpart = FindControl( localwhere , window , &control ) ;
		}
		else
		{
			control = FindControlUnderMouse( localwhere , window , &controlpart ) ;
		}
		
		if ( control && IsControlActive( control ) )
		{
			wxControl* wxc = (wxControl*) GetControlReference( control ) ;
					
			if ( wxWindow::FindFocus() != wxc &&  wxc->AcceptsFocus() )
			{
				wxc->SetFocus() ;	
				if ( wxWindow::FindFocus() != wxc )
					control = NULL ; // we were not able to change focus
			}		
								
			if ( control )
			{
				if ( !gSFMacHasAppearance)
				{
					controlpart = TrackControl( control , localwhere , NULL ) ;
				}
				else
				{
					controlpart = HandleControlClick( control , localwhere , m_event.modifiers , (ControlActionUPP) -1 ) ;
				}
				
				if ( controlpart )
				{
					wxControl*	wx = (wxControl*) GetControlReference( control ) ;
					if ( wx )
						wx->MacHandleControlClick( control , controlpart ) ;
				}
			}
		}
		else
		{
			wxWindow* theMacWxFrame = wxFrame::MacFindFrameOrDialog(window);
			if (theMacWxFrame)
			{
				doMacContentClick((wxFrame*)theMacWxFrame); // todo : this cast is wrong
			}
		}
	}
	
#if 0
	wxFrame* theMacWxFrame = findMacWxFrame(window);
	if (theMacWxFrame)
	{
		WindowPtr MacWindow = findFrontNonFloatingWindow();
		if (window != MacWindow)
		{		
			wxFrame* frontFrame = findMacWxFrame(MacWindow);
			if (!frontFrame) wxFatalError("No wxFrame for frontnonfloatingWindow.");
			if (!frontFrame->IsModal())
			{
				frontFrame->SetFocus();
				doMacContentClick(theMacWxFrame);	// jonto - to deal with doc windows behind floaters ?
				::newSelectWindow(window); 			// WCH : should I be calling some wxMethod?
				if (!IsFloating(MacWindow))
				{
					KeyMap keyMap;
					GetKeys(keyMap);
					if (!(keyMap[1] & 0x8000)) theMacWxFrame->ShowAsActive(true);	// temporary measure...
				}
			}										// jonto : not sure yet, but let's try this ...
			else ::SysBeep(3);
		}
		else
		{
			doMacContentClick(theMacWxFrame);
		}
	}
#endif
}

void wxApp::doMacContentClick(wxWindow* frame)
{
	m_mouseWindow = frame;

	wxMouseEvent event(wxEVT_LEFT_DOWN);
	event.m_shiftDown = m_event.modifiers & shiftKey;
	event.m_controlDown = m_event.modifiers & controlKey;
	event.m_altDown = m_event.modifiers & optionKey;
	event.m_metaDown = m_event.modifiers & cmdKey;
	event.m_leftDown = FALSE;
	event.m_middleDown = FALSE;
	event.m_rightDown = FALSE;
	if ( m_event.modifiers & controlKey )
	{
		event.m_rightDown = TRUE;
	}
	else
	{
		event.m_leftDown = TRUE;
	}
#if 0
  event.m_leftDown = !(m_event.modifiers & btnState);
	event.m_middleDown = FALSE;
	event.m_rightDown = FALSE;
#endif
	event.m_x = m_event.where.h;
	event.m_y = m_event.where.v;
	event.m_timeStamp = m_event.when;
	event.SetEventObject(m_mouseWindow);

//	m_mouseWindow->ProcessEvent(event);
	m_mouseWindow->MacDispatchMouseEvent(event);
	
#if 0
	// RightButton is cmdKey click  on the mac platform for one-button mouse
	Bool rightButton = cCurrentEvent.modifiers & cmdKey;
	// altKey is optionKey on the mac platform:
	Bool isAltKey = cCurrentEvent.modifiers & optionKey;

	WXTYPE mouseEventType = rightButton ? wxEVENT_TYPE_RIGHT_DOWN
										: wxEVENT_TYPE_LEFT_DOWN;
	wxMouseEvent theMouseEvent(mouseEventType);
	theMouseEvent.leftDown = !rightButton;
	theMouseEvent.middleDown = FALSE;
	theMouseEvent.rightDown = rightButton;
	theMouseEvent.shiftDown = cCurrentEvent.modifiers & shiftKey;
	theMouseEvent.controlDown = cCurrentEvent.modifiers & controlKey;
	theMouseEvent.altDown = isAltKey;
	theMouseEvent.metaDown = FALSE;  // mflatt
	theMouseEvent.timeStamp = cCurrentEvent.when; // mflatt

	int hitX = cCurrentEvent.where.h; // screen window c.s.
	int hitY = cCurrentEvent.where.v; // screen window c.s.

	frame->ScreenToWindow(&hitX, &hitY);
//	frameParentArea->ScreenToArea(&hitX, &hitY);			// tx coords ?
	theMouseEvent.x = hitX; // frame parent area c.s.
	theMouseEvent.y = hitY; // frame parent area c.s.

	frame->SeekMouseEventArea(theMouseEvent);
#endif
}

void wxApp::doMacInDrag(WindowPtr window)
{
	wxWindow* theMacWxFrame = wxFrame::MacFindFrameOrDialog(window);
	if (theMacWxFrame)
	{
		// should be desktop, not screen
		Rect dragBoundsRect = qd.screenBits.bounds;	// can't move to a different screen
//xxx	::InsetRect(&dragBoundsRect, 4, ::GetMBarHeight() + 4); // This is not really necessary
		Rect oldPos = (**(((WindowPeek)window)->strucRgn)).rgnBBox;
		::DragReferencedWindow(window, m_event.where, &dragBoundsRect);	// jonto
		#if 0
		theMacWxFrame->m_x += (**(((WindowPeek)window)->strucRgn)).rgnBBox.left - oldPos.left;
		theMacWxFrame->m_y += (**(((WindowPeek)window)->strucRgn)).rgnBBox.top - oldPos.top;
		#endif
		Move( (**(((WindowPeek)window)->strucRgn)).rgnBBox.left , (**(((WindowPeek)window)->strucRgn)).rgnBBox.top ) ;
#if 0
		theMacWxFrame->wxMacRecalcNewSize(); // Actually, recalc new position only
#endif
	}
	
#if 0
//	if (window != ::FrontWindow())
	if (window != findFrontNonFloatingWindow())
	{
//		wxFrame* frontFrame = findMacWxFrame(::FrontWindow());
		wxFrame* frontFrame = findMacWxFrame(findFrontNonFloatingWindow());
		if (!frontFrame) wxFatalError("No wxFrame for frontWindow.");
		if (frontFrame->IsModal())
		{
			::SysBeep(3);
			return;
		}
	}

	wxFrame* theMacWxFrame = findMacWxFrame(window);
	if (theMacWxFrame)
	{
		Rect dragBoundsRect = qd.screenBits.bounds;	// can't move to a different screen
		::InsetRect(&dragBoundsRect, 4, ::GetMBarHeight() + 4); // This is not really necessary
		newDragWindow(window, cCurrentEvent.where, &dragBoundsRect);	// jonto
		theMacWxFrame->wxMacRecalcNewSize(); // Actually, recalc new position only
		if (!IsFloating(window))
		{
			theMacWxFrame->ShowAsActive(true);	// temporary measure...
		}
	}
#endif
}

void wxApp::doMacInGrow(WindowPtr window)
{
	wxWindow* theMacWxFrame = wxFrame::MacFindFrameOrDialog(window);
	if (theMacWxFrame)
	{
		Rect growSizeRect; // WCH: growSizeRect should be a member of wxFrame class
		growSizeRect.top = 1; // minimum window height
		growSizeRect.left = 1; // minimum window width
		growSizeRect.bottom = qd.screenBits.bounds.bottom - qd.screenBits.bounds.top;
		growSizeRect.right = qd.screenBits.bounds.right - qd.screenBits.bounds.left;
		long windSize = ::GrowWindow(window, m_event.where, &growSizeRect);
		if (windSize != 0)
		{
			int nWidth = LoWord(windSize);
			int nHeight = HiWord(windSize);
			int oWidth, oHeight;
			theMacWxFrame->GetSize(&oWidth, &oHeight);
			if (nWidth == 0) nWidth = oWidth;
			if (nHeight == 0) nHeight = oHeight;
			theMacWxFrame->SetSize( -1, -1, nWidth, nHeight, wxSIZE_USE_EXISTING);
		}
	}
}

void wxApp::doMacInGoAway(WindowPtr window)
{
	wxWindow* theMacWxFrame = wxFrame::MacFindFrameOrDialog(window);
	if (theMacWxFrame)
	{
		if (TrackGoAway(window, m_event.where))
		{
            // TODO: Stefan, I think you need to send a wxCloseEvent to the window
            // here. The OnCloseWindow handler will take care of delete the frame
            // if it wishes to (there should be a default wxFrame::OnCloseWindow
            // that destroys the frame).
			if (theMacWxFrame->OnClose()) {
#if WXGARBAGE_COLLECTION_ON
				theMacWxFrame->Show(FALSE);
#else
				delete theMacWxFrame;
#endif
			}
		}
	}
}

void wxApp::doMacInZoom(WindowPtr window, short windowPart)
{
	wxWindow* theMacWxFrame = wxFrame::MacFindFrameOrDialog(window);
	if (theMacWxFrame)
	{
		if (TrackBox(window, m_event.where, windowPart))
		{
#if 0
			theMacWxFrame->Maximize(windowPart == inZoomOut);
#endif
		}
	}
}

long wxApp::macTranslateKey(char key, int mods)
{
    static Handle transH = NULL;
    static unsigned long transState = 0;
    static Handle ScriptH = NULL;
    static short region_code = 1;

	if (!ScriptH) { // tom: don't guess the regioncode!!!!
      struct ItlbRecord * r;
      ScriptH = GetResource('itlb',0);
      if (ScriptH) {
      	HLock(ScriptH);
      	r = (ItlbRecord*)*ScriptH;
     	region_code = r->itlbKeys;  	
      	HUnlock(ScriptH);
      }	
    }
 
    switch (key) {
    case 0x7e:
    case 0x3e:
      key = WXK_UP;
      break;
    case 0x7d:
    case 0x3d:
      key = WXK_DOWN;
      break;
    case 0x7b:
    case 0x3b:
      key = WXK_LEFT;
      break;
    case 0x7c:
    case 0x3c:
      key = WXK_RIGHT;
      break;
    case 0x24:
    case 0x4c:
      key = WXK_RETURN;
      break;
    case 0x30:
      key = WXK_TAB;
      break;
    case 0x33:
      key = WXK_BACK;
      break;
    case 0x75:
      key = WXK_DELETE;
      break;
    case 0x73:
	  key = WXK_HOME;
	  break;
	case 0x77:
	  key = WXK_END;
	  break;   
	case 0x74:
	  key = WXK_PAGEUP;
	  break;     
	case 0x79:
	  key = WXK_PAGEDOWN;
	  break;     
    default:
      if (!transH) {
		transH = GetIndResource('KCHR', 1);
		HNoPurge(transH);
      }
#if 0 //Tom replaces
      if (transH) {
		// Only let shift & option modify the key: 
		HLock(transH);
		key = KeyTranslate(*transH, (key & 0x7F) | mods, &transState) & charCodeMask;
		HUnlock(transH);
#else
       if (0) { // tom fettig@dfki.uni-sb.de
 		// why if(0):
 		// code is not correct, see inside Macintosh: Text 1-87
 		// and 'itlk'-resource!!
 		// and it is not necessary, as the translated char is in
 		// cCurrrentEvent.message!!
  		// Only let shift & option modify the key: 
  		HLock(transH);
		key = KeyTranslate(*transH, (key & 0x7F) | mods, &transState) & charCodeMask;
		HUnlock(transH);
#endif
	  }
 	} // end switch
	
	return key;
}

void
wxApp::macAdjustCursor()
{
  if (m_event.what != kHighLevelEvent)
  {
	wxWindow* theMacWxFrame = wxFrame::MacFindFrameOrDialog(::FrontWindow());
	if (theMacWxFrame)
	{
  	  if (!theMacWxFrame->MacAdjustCursor(m_event.where))
		::SetCursor(&(qd.arrow));
  	}
  }
}
*/