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

// mac

#if __option(profile)
	#include <profiler.h>
#endif

#include "apprsrc.h"

#include <wx/mac/uma.h>

extern char *wxBuffer;
extern wxList wxPendingDelete;
extern wxList *wxWinMacWindowList;
extern wxList *wxWinMacControlList;

wxApp *wxTheApp = NULL;

IMPLEMENT_DYNAMIC_CLASS(wxApp, wxEvtHandler)
BEGIN_EVENT_TABLE(wxApp, wxEvtHandler)
    EVT_IDLE(wxApp::OnIdle)
END_EVENT_TABLE()


const short	kMacMinHeap = (29 * 1024) ;
// platform specific static variables

const short kwxMacMenuBarResource = 1 ;
const short kwxMacAppleMenuId = 1 ;

RgnHandle			wxApp::s_macCursorRgn = NULL;
wxWindow*			wxApp::s_captureWindow = NULL ;
int					wxApp::s_lastMouseDown = 0 ;
long 					wxApp::sm_lastMessageTime = 0;

#ifdef __WXMAC__

bool	wxApp::s_macDefaultEncodingIsPC = true ;
bool wxApp::s_macSupportPCMenuShortcuts = true ;
long wxApp::s_macAboutMenuItemId = wxID_ABOUT ;
wxString wxApp::s_macHelpMenuTitleName = "&Help" ;

OSErr AEHandleODoc( AppleEvent *event , AppleEvent *reply , long refcon )
{
	wxApp* app = (wxApp*) refcon ;
	return wxTheApp->MacHandleAEODoc( event , reply) ;
}

OSErr AEHandleOApp( AppleEvent *event , AppleEvent *reply , long refcon )
{
	wxApp* app = (wxApp*) refcon ;
	return wxTheApp->MacHandleAEOApp( event , reply ) ;
}

OSErr AEHandlePDoc( AppleEvent *event , AppleEvent *reply , long refcon )
{
	wxApp* app = (wxApp*) refcon ;
	return wxTheApp->MacHandleAEPDoc( event , reply ) ;
}

OSErr AEHandleQuit( AppleEvent *event , AppleEvent *reply , long refcon )
{
	wxApp* app = (wxApp*) refcon ;
	return wxTheApp->MacHandleAEQuit( event , reply) ;
}

OSErr wxApp::MacHandleAEODoc(AppleEvent *event , AppleEvent *reply)
{
	ProcessSerialNumber PSN ;
	PSN.highLongOfPSN = 0 ;
	PSN.lowLongOfPSN = kCurrentProcess ;
	SetFrontProcess( &PSN ) ;
	return noErr ;
}

OSErr wxApp::MacHandleAEPDoc(AppleEvent *event , AppleEvent *reply)
{
	return noErr ;
}

OSErr wxApp::MacHandleAEOApp(AppleEvent *event , AppleEvent *reply)
{
	return noErr ;
}

OSErr wxApp::MacHandleAEQuit(AppleEvent *event , AppleEvent *reply)
{
	wxWindow* win = GetTopWindow() ;
	if ( win )
	{
		win->Close(TRUE ) ;
	}
	else
	{
		ExitMainLoop() ;
	}
	return noErr ;
}

char StringMac[] = 	"\x80\x81\x82\x83\x84\x85\x86\x87\x88\x89\x8a\x8b\x8c\x8d\x8e\x8f"
					"\x90\x91\x92\x93\x94\x95\x96\x97\x98\x99\x9a\x9b\x9c\x9d\x9e\x9f"
					"\xa0\xa1\xa2\xa3\xa4\xa5\xa6\xa7\xa8\xa9\xaa\xab\xac\xae\xaf"
					"\xb1\xb4\xb5\xb6\xbb\xbc\xbe\xbf"
					"\xc0\xc1\xc2\xc4\xc7\xc8\xc9\xcb\xcc\xcd\xce\xcf"
					"\xd0\xd1\xd2\xd3\xd4\xd5\xd6\xd8\xca\xdb" ;

char StringANSI[] = "\xC4\xC5\xC7\xC9\xD1\xD6\xDC\xE1\xE0\xE2\xE4\xE3\xE5\xE7\xE9\xE8"
					"\xEA\xEB\xED\xEC\xEE\xEF\xF1\xF3\xF2\xF4\xF6\xF5\xFA\xF9\xFB\xFC"
					"\x86\xBA\xA2\xA3\xA7\x95\xB6\xDF\xAE\xA9\x99\xB4\xA8\xC6\xD8"
					"\xB1\xA5\xB5\xF0\xAA\xBA\xE6\xF8"
					"\xBF\xA1\xAC\x83\xAB\xBB\x85\xC0\xC3\xD5\x8C\x9C"
					"\x96\x97\x93\x94\x91\x92\xF7\xFF\xA0\x80" ;

void wxMacConvertFromPC( const char *from , char *to , int len )
{
	char *c ;
	if ( from == to )
	{
		for( int i = 0 ; i < len ; ++ i )
		{
			c = strchr( StringANSI , *from ) ;
			if ( c != NULL )
			{
				*to = StringMac[ c - StringANSI] ;
			}
			++to ;
			++from ;
		}
	}
	else
	{
		for( int i = 0 ; i < len ; ++ i )
		{
			c = strchr( StringANSI , *from ) ;
			if ( c != NULL )
			{
				*to = StringMac[ c - StringANSI] ;
			}
			else
			{
				*to = *from ;
			}
			++to ;
			++from ;
		}
	}
}

void wxMacConvertToPC( const char *from , char *to , int len )
{
	char *c ;
	if ( from == to )
	{
		for( int i = 0 ; i < len ; ++ i )
		{
			c = strchr( StringMac , *from ) ;
			if ( c != NULL )
			{
				*to = StringANSI[ c - StringMac] ;
			}
			++to ;
			++from ;
		}
	}
	else
	{
		for( int i = 0 ; i < len ; ++ i )
		{
			c = strchr( StringMac , *from ) ;
			if ( c != NULL )
			{
				*to = StringANSI[ c - StringMac] ;
			}
			else
			{
				*to = *from ;
			}
			++to ;
			++from ;
		}
	}
}

void wxMacConvertFromPC( char * p )
{
	char *ptr = p ;
	int len = strlen ( p ) ;

	wxMacConvertFromPC( ptr , ptr , len ) ;
}

void wxMacConvertFromPCForControls( char * p )
{
	char *ptr = p ;
	int len = strlen ( p ) ;

	wxMacConvertFromPC( ptr , ptr , len ) ;
	for ( int i = 0 ; i < strlen ( ptr ) ; i++ )
	{
		if ( ptr[i] == '&' && ptr[i]+1 != ' ' )
		{
			memmove( &ptr[i] , &ptr[i+1] , strlen( &ptr[i+1] ) + 1) ;
		}
	}
}

void wxMacConvertFromPC( unsigned char *p )
{
	char *ptr = (char*) p + 1 ;
	int len = p[0] ;

	wxMacConvertFromPC( ptr , ptr , len ) ;
}

extern char *wxBuffer ;

wxString wxMacMakeMacStringFromPC( const char * p )
{
	const char *ptr = p ;
	int len = strlen ( p ) ;
	char *buf = wxBuffer ;

	if ( len >= BUFSIZ + 512 )
	{
		buf = new char [len+1] ;
	}

	wxMacConvertFromPC( ptr , buf , len ) ;
	buf[len] = 0 ;
	wxString result( buf ) ;
	if ( buf != wxBuffer )
		delete buf ;
	return result ;
}


void wxMacConvertToPC( char * p )
{
	char *ptr = p ;
	int len = strlen ( p ) ;

	wxMacConvertToPC( ptr , ptr , len ) ;
}

void wxMacConvertToPC( unsigned char *p )
{
	char *ptr = (char*) p + 1 ;
	int len = p[0] ;

	wxMacConvertToPC( ptr , ptr , len ) ;
}

wxString wxMacMakePCStringFromMac( const char * p )
{
	const char *ptr = p ;
	int len = strlen ( p ) ;
	char *buf = wxBuffer ;

	if ( len >= BUFSIZ + 512 )
	{
		buf = new char [len+1] ;
	}

	wxMacConvertToPC( ptr , buf , len ) ;
	buf[len] = 0 ;

	wxString result( buf ) ;
	if ( buf != wxBuffer )
		delete buf ;
	return result ;
}

#endif

bool wxApp::Initialize()
{
  int error = 0 ;

  // Mac-specific

  UMAInitToolbox( 4 ) ;
	UMAShowWatchCursor() ;

	AEInstallEventHandler( kCoreEventClass , kAEOpenDocuments , NewAEEventHandlerProc(AEHandleODoc) , (long) wxTheApp , FALSE ) ;
	AEInstallEventHandler( kCoreEventClass , kAEOpenApplication , NewAEEventHandlerProc(AEHandleOApp) , (long) wxTheApp , FALSE ) ;
	AEInstallEventHandler( kCoreEventClass , kAEPrintDocuments , NewAEEventHandlerProc(AEHandlePDoc) , (long) wxTheApp , FALSE ) ;
	AEInstallEventHandler( kCoreEventClass , kAEQuitApplication , NewAEEventHandlerProc(AEHandleQuit) , (long) wxTheApp  , FALSE ) ;
#if 0
	GUSISetup(GUSIwithInternetSockets);
#endif


  // test the minimal configuration necessary

	long theSystem ;
	long theMachine;

	if (Gestalt(gestaltMachineType, &theMachine) != noErr)
	{
		error = kMacSTRWrongMachine;
	}
	else if (theMachine < gestaltMacPlus)
	{
		error = kMacSTRWrongMachine;
	}
	else if (Gestalt(gestaltSystemVersion, &theSystem) != noErr )
	{
		error = kMacSTROldSystem  ;
	}
	else if ( theSystem < 0x0750 )
	{
		error = kMacSTROldSystem  ;
	}
	#if !TARGET_CARBON
	else if ((long)GetApplLimit() - (long)ApplicationZone() < kMacMinHeap)
	{
		error = kMacSTRSmallSize;
	}
	#endif
	/*
	else
	{
		if ( !UMAHasAppearance() )
		{
			error = kMacSTRNoPre8Yet ;
		}
	}
	*/

	// if we encountered any problems so far, give the error code and exit immediately

  if ( error )
  {
		short itemHit;
		Str255 message;

		GetIndString(message, 128, error);
		UMAShowArrowCursor() ;
		ParamText("\pFatal Error", message, (ConstStr255Param)"\p", (ConstStr255Param)"\p");
		itemHit = Alert(128, nil);
	  return FALSE ;
  }

#if __option(profile)
	ProfilerInit( collectDetailed, bestTimeBase , 20000 , 30 ) ;
#endif

  // now avoid exceptions thrown for new (bad_alloc)

  std::__throws_bad_alloc = FALSE ;

	s_macCursorRgn = ::NewRgn() ;

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
  if (!wxModule::InitializeModules())
     return FALSE;

  wxWinMacWindowList = new wxList(wxKEY_INTEGER);
  wxWinMacControlList = new wxList(wxKEY_INTEGER);

	UMAShowArrowCursor() ;

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

  if (wxWinMacWindowList)
    delete wxWinMacWindowList ;

  wxClassInfo::CleanUpClasses();

#if __option(profile)
	ProfilerDump( "\papp.prof" ) ;
	ProfilerTerm() ;
#endif

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

	::PrClose() ;
	if (s_macCursorRgn)
		::DisposeRgn(s_macCursorRgn);
	#if 0
		TerminateAE() ;
	#endif
}

int wxEntry( int argc, char *argv[] )
{
#ifdef __MWERKS__
#if (defined(__WXDEBUG__) && wxUSE_MEMORY_TRACING) || wxUSE_DEBUG_CONTEXT
    // This seems to be necessary since there are 'rogue'
    // objects present at this point (perhaps global objects?)
    // Setting a checkpoint will ignore them as far as the
    // memory checking facility is concerned.
    // Of course you may argue that memory allocated in globals should be
    // checked, but this is a reasonable compromise.
    wxDebugContext::SetCheckpoint();
#endif
#endif
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

#ifdef __WXMAC__
  argc = 1 ; // currently we don't support files as parameters
#endif

  wxTheApp->argc = argc;
  wxTheApp->argv = argv;

  // GUI-specific initialization, such as creating an app context.
  wxTheApp->OnInitGui();

  // we could try to get the open apple events here to adjust argc and argv better


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

  m_printMode = wxPRINT_WINDOWS;

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

  while (m_keepGoing)
  {
		MacDoOneEvent() ;
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
	EventRecord event ;

  return EventAvail( everyEvent , &event ) ;
}

// Dispatch a message.
void wxApp::Dispatch()
{
	MacDoOneEvent() ;
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

void wxWakeUpIdle()
{
    // **** please implement me! ****
    // Wake up the idle handler processor, even if it is in another thread...
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
	::ExitToShell() ;
}

// Yield to other processes
static bool gs_inYield = FALSE;

bool wxYield()
{
#ifdef __WXDEBUG__
    if (gs_inYield)
        wxFAIL_MSG( wxT("wxYield called recursively" ) );
#endif

    gs_inYield = TRUE;
    
//	YieldToAnyThread() ;
	SystemTask() ;

    gs_inYield = FALSE;
    
  return TRUE;
}

// Yield to incoming messages; but fail silently if recursion is detected.
bool wxYieldIfNeeded()
{
    if (gs_inYield)
        return FALSE;
        
    return wxYield();
}

// platform specifics

void wxApp::MacSuspend( bool convertClipboard )
{
		s_lastMouseDown = 0 ;
		if( convertClipboard )
		{
			MacConvertPrivateToPublicScrap() ;
		}

		UMAHideFloatingWindows() ;
}

void wxApp::MacResume( bool convertClipboard )
{
		s_lastMouseDown = 0 ;
		if( convertClipboard )
		{
			MacConvertPublicToPrivateScrap() ;
		}

		UMAShowFloatingWindows() ;
}

void wxApp::MacConvertPrivateToPublicScrap()
{
	::ZeroScrap();
	::TEToScrap();
}

void wxApp::MacConvertPublicToPrivateScrap()
{
	::TEFromScrap() ;
}

void wxApp::MacDoOneEvent()
{
  EventRecord event ;

	long sleepTime = ::GetCaretTime();

	if (WaitNextEvent(everyEvent, &event,sleepTime, s_macCursorRgn))
	{
    MacHandleOneEvent( &event );
	}
	else
	{
		// idlers
		WindowPtr window = UMAFrontWindow() ;
		if ( window )
			UMAIdleControls( window ) ;

		wxTheApp->ProcessIdle() ;
	}
	if ( event.what != kHighLevelEvent )
		SetRectRgn( s_macCursorRgn , event.where.h - 1 , event.where.v - 1,  event.where.h + 1 , event.where.v + 1 ) ;

	// repeaters

#if 0
	wxMacProcessSocketEvents() ;
#endif
}

void wxApp::MacHandleOneEvent( EventRecord *ev )
{
	m_macCurrentEvent = ev ;

	wxApp::sm_lastMessageTime = ev->when ;

	switch (ev->what)
	{
		case mouseDown:
			MacHandleMouseDownEvent( ev ) ;
			if ( ev->modifiers & controlKey )
				s_lastMouseDown = 2;
			else
				s_lastMouseDown = 1;
			break;
		case mouseUp:
			if ( s_lastMouseDown == 2 )
			{
				ev->modifiers |= controlKey ;
			}
			else
			{
				ev->modifiers &= ~controlKey ;
			}
			MacHandleMouseUpEvent( ev ) ;
			s_lastMouseDown = 0;
			break;
		case activateEvt:
			MacHandleActivateEvent( ev ) ;
			break;
		case updateEvt:
			MacHandleUpdateEvent( ev ) ;
			break;
		case keyDown:
		case autoKey:
			MacHandleKeyDownEvent( ev ) ;
			break;
		case keyUp:
			MacHandleKeyUpEvent( ev ) ;
			break;
		case diskEvt:
			MacHandleDiskEvent( ev ) ;
			break;
		case osEvt:
			MacHandleOSEvent( ev ) ;
			break;
		case kHighLevelEvent:
			MacHandleHighLevelEvent( ev ) ;
			break;
		default:
			break;
	}
}

void wxApp::MacHandleHighLevelEvent( EventRecord *ev )
{
	::AEProcessAppleEvent( ev ) ;
}

bool s_macIsInModalLoop = false ;

void wxApp::MacHandleMouseDownEvent( EventRecord *ev )
{
	WindowRef window;
	WindowRef frontWindow = UMAFrontNonFloatingWindow() ;
	WindowAttributes frontWindowAttributes = NULL ;
	if ( frontWindow )
		UMAGetWindowAttributes( frontWindow , &frontWindowAttributes ) ;

	short windowPart = ::FindWindow(ev->where, &window);
	wxWindow* win = wxFindWinFromMacWindow( window ) ;

	switch (windowPart)
	{
		case inMenuBar :
			if ( s_macIsInModalLoop )
			{
				SysBeep ( 30 ) ;
			}
			else
			{
				UInt32 menuresult = MenuSelect(ev->where) ;
				MacHandleMenuSelect( HiWord( menuresult ) , LoWord( menuresult ) );
				s_lastMouseDown = 0;
			}
			break ;
		case inSysWindow :
			SystemClick( ev , window ) ;
			s_lastMouseDown = 0;
			break ;
		case inDrag :
			if ( window != frontWindow && s_macIsInModalLoop && !(ev->modifiers & cmdKey ) )
			{
				SysBeep ( 30 ) ;
			}
			else
			{
				DragWindow(window, ev->where, &qd.screenBits.bounds);
				if (win)
				{
					GrafPtr port ;
					GetPort( &port ) ;
					Point pt = { 0, 0 } ;
					SetPort( window ) ;
					SetOrigin( 0 , 0 ) ;
					LocalToGlobal( &pt ) ;
					SetPort( port ) ;
						win->SetSize( pt.h , pt.v , -1 ,
							-1 , wxSIZE_USE_EXISTING);
				}
				s_lastMouseDown = 0;
			}
			break ;
		case inGoAway:
			if (TrackGoAway(window, ev->where))
			{
				if ( win )
					win->Close() ;
			}
			s_lastMouseDown = 0;
			break;
		case inGrow:
				int growResult = GrowWindow(window , ev->where, &qd.screenBits.bounds);
				if (growResult != 0)
				{
					int newWidth = LoWord(growResult);
					int newHeight = HiWord(growResult);
					int oldWidth, oldHeight;

					win->GetSize(&oldWidth, &oldHeight);
					if (newWidth == 0)
						newWidth = oldWidth;
					if (newHeight == 0)
						newHeight = oldHeight;

					if (win)
						win->SetSize( -1, -1, newWidth, newHeight, wxSIZE_USE_EXISTING);
				}
				s_lastMouseDown = 0;
			break;
		case inZoomIn:
		case inZoomOut:
				if (TrackBox(window, ev->where, windowPart))
				{
					// TODO setup size event
					ZoomWindow( window , windowPart , false ) ;
					if (win)
						win->SetSize( -1, -1, window->portRect.right-window->portRect.left ,
							window->portRect.bottom-window->portRect.top, wxSIZE_USE_EXISTING);
				}
			s_lastMouseDown = 0;
			break;
		case inCollapseBox :
				// TODO setup size event
			s_lastMouseDown = 0;
			break ;

		case inContent :
				if ( window != frontWindow )
				{
					if ( s_macIsInModalLoop )
					{
						SysBeep ( 30 ) ;
					}
					else if ( UMAIsWindowFloating( window ) )
					{
						if ( win )
							win->MacMouseDown( ev , windowPart ) ;
					}
					else
					{
						UMASelectWindow( window ) ;
					}
				}
				else
				{
					if ( win )
						win->MacMouseDown( ev , windowPart ) ;
				}
			break ;

		default:
			break;
	}
}

void wxApp::MacHandleMouseUpEvent( EventRecord *ev )
{
	WindowRef window;

	short windowPart = ::FindWindow(ev->where, &window);

	switch (windowPart)
	{
		case inMenuBar :
			break ;
		case inSysWindow :
			break ;
		default:
			{
				wxWindow* win = wxFindWinFromMacWindow( window ) ;
				if ( win )
					win->MacMouseUp( ev , windowPart ) ;
			}
			break;
	}
}

long wxMacTranslateKey(unsigned char key, unsigned char code)
{
	long retval = key ;
    switch (key)
    {
    	case 0x01 :
		 		retval = WXK_HOME;
		  break;
    	case 0x03 :
		 		retval = WXK_RETURN;
		  break;
    	case 0x04 :
		 		retval = WXK_END;
		  break;
    	case 0x05 :
		 		retval = WXK_HELP;
		  break;
    	case 0x08 :
		 		retval = WXK_BACK;
		  break;
    	case 0x09 :
		 		retval = WXK_TAB;
		  break;
    	case 0x0b :
		 		retval = WXK_PAGEUP;
		  break;
    	case 0x0c :
		 		retval = WXK_PAGEDOWN;
		  break;
    	case 0x0d :
		 		retval = WXK_RETURN;
		  break;
			case 0x10 :
			{
				switch( code )
				{
					case 0x7a :
						retval = WXK_F1 ;
						break;
					case 0x78 :
						retval = WXK_F2 ;
						break;
					case 0x63 :
						retval = WXK_F3 ;
						break;
					case 0x76 :
						retval = WXK_F4 ;
						break;
					case 0x60 :
						retval = WXK_F5 ;
						break;
					case 0x61 :
						retval = WXK_F6 ;
						break;
					case 0x62:
						retval = WXK_F7 ;
						break;
					case 0x64 :
						retval = WXK_F8 ;
						break;
					case 0x65 :
						retval = WXK_F9 ;
						break;
					case 0x6D :
						retval = WXK_F10 ;
						break;
					case 0x67 :
						retval = WXK_F11 ;
						break;
					case 0x6F :
						retval = WXK_F12 ;
						break;
					case 0x69 :
						retval = WXK_F13 ;
						break;
					case 0x6B :
						retval = WXK_F14 ;
						break;
					case 0x71 :
						retval = WXK_F15 ;
						break;
				}
			}
			break ;
			case 0x1b :
				retval = WXK_ESCAPE ;
			break ;
			case 0x1c :
				retval = WXK_LEFT ;
			break ;
			case 0x1d :
				retval = WXK_RIGHT ;
			break ;
			case 0x1e :
				retval = WXK_UP ;
			break ;
			case 0x1f :
				retval = WXK_DOWN ;
			break ;
			case 0x7F :
				retval = WXK_DELETE ;
	 		default:
			break ;
 	} // end switch

	return retval;
}

void wxApp::MacHandleKeyDownEvent( EventRecord *ev )
{
	UInt32 menuresult = UMAMenuEvent(ev) ;
	if ( HiWord( menuresult ) )
		MacHandleMenuSelect( HiWord( menuresult ) , LoWord( menuresult ) ) ;
	else
	{
		short keycode ;
		short keychar ;
		keychar = short(ev->message & charCodeMask);
		keycode = short(ev->message & keyCodeMask) >> 8 ;

		wxWindow* focus = wxWindow::FindFocus() ;
		if ( focus )
		{
			long keyval = wxMacTranslateKey(keychar, keycode) ;
			
			wxKeyEvent event(wxEVT_KEY_DOWN);
			event.m_shiftDown = ev->modifiers & shiftKey;
			event.m_controlDown = ev->modifiers & controlKey;
			event.m_altDown = ev->modifiers & optionKey;
			event.m_metaDown = ev->modifiers & cmdKey;
			event.m_keyCode = keyval;
			event.m_x = ev->where.h;
			event.m_y = ev->where.v;
			event.m_timeStamp = ev->when;
			event.SetEventObject(focus);
			bool handled = focus->GetEventHandler()->ProcessEvent( event ) ;
			if ( !handled )
			{
				#if wxUSE_ACCEL
			    if (!handled)
			    {
			        wxWindow *ancestor = focus;
			        /*
			        while (ancestor)
			        {
			            int command = ancestor->GetAcceleratorTable()->GetCommand( event );
			            if (command != -1)
			            {
			                wxCommandEvent command_event( wxEVT_COMMAND_MENU_SELECTED, command );
			                handled = ancestor->GetEventHandler()->ProcessEvent( command_event );
			                break;
			            }
			            if (ancestor->m_isFrame)
			                break;
			            ancestor = ancestor->GetParent();
			        }
			        */
			    }
				#endif // wxUSE_ACCEL
			}
			if (!handled)
			{
				wxKeyEvent event(wxEVT_CHAR);
				event.m_shiftDown = ev->modifiers & shiftKey;
				event.m_controlDown = ev->modifiers & controlKey;
				event.m_altDown = ev->modifiers & optionKey;
				event.m_metaDown = ev->modifiers & cmdKey;
				event.m_keyCode = keyval;
				event.m_x = ev->where.h;
				event.m_y = ev->where.v;
				event.m_timeStamp = ev->when;
				event.SetEventObject(focus);
				handled = focus->GetEventHandler()->ProcessEvent( event ) ;
			}
			if ( !handled &&
		         (keyval == WXK_TAB) &&
		         (!focus->HasFlag(wxTE_PROCESS_TAB)) &&
		         (focus->GetParent()) &&
		         (focus->GetParent()->HasFlag( wxTAB_TRAVERSAL)) )
		    {
		        wxNavigationKeyEvent new_event;
		        new_event.SetEventObject( focus );
		        new_event.SetDirection( !event.ShiftDown() );
		        /* CTRL-TAB changes the (parent) window, i.e. switch notebook page */
		        new_event.SetWindowChange( event.ControlDown() );
		        new_event.SetCurrentFocus( focus );
		        handled = focus->GetEventHandler()->ProcessEvent( new_event );
		    }
		    /* generate wxID_CANCEL if command-. or <esc> has been pressed (typically in dialogs) */
		    if ( (!handled) &&
		         (keyval == '.' && event.ControlDown() ) )
		    {
		        wxCommandEvent new_event(wxEVT_COMMAND_BUTTON_CLICKED,wxID_CANCEL);
		        new_event.SetEventObject( focus );
		        handled = focus->GetEventHandler()->ProcessEvent( new_event );
		    }
		}
	}
}

void wxApp::MacHandleKeyUpEvent( EventRecord *ev )
{
	// nothing to do
}

void wxApp::MacHandleActivateEvent( EventRecord *ev )
{
	WindowRef window = (WindowRef) ev->message ;
	if ( window )
	{
		bool activate = (ev->modifiers & activeFlag ) ;
		WindowClass wclass ;
		UMAGetWindowClass ( window , &wclass ) ;
		if ( wclass == kFloatingWindowClass )
		{
			// if it is a floater we activate/deactivate the front non-floating window instead
			window = UMAFrontNonFloatingWindow() ;
		}
		wxWindow* win = wxFindWinFromMacWindow( window ) ;
		if ( win )
			win->MacActivate( ev , activate ) ;
	}
}

void wxApp::MacHandleUpdateEvent( EventRecord *ev )
{
	WindowRef window = (WindowRef) ev->message ;
	wxWindow * win = wxFindWinFromMacWindow( window ) ;
	if ( win )
	{
		win->MacUpdate( ev ) ;
	}
}

void wxApp::MacHandleDiskEvent( EventRecord *ev )
{
	if ( HiWord( ev->message ) != noErr )
  {
		OSErr err ;
		Point point ;
 		SetPt( &point , 100 , 100 ) ;

  	err = DIBadMount( point , ev->message ) ;
		wxASSERT( err == noErr ) ;
	}
}

void wxApp::MacHandleOSEvent( EventRecord *ev )
{
	switch( ( ev->message & osEvtMessageMask ) >> 24 )
	{
		case suspendResumeMessage :
			{
				bool isResuming = ev->message & resumeFlag ;
				bool convertClipboard = ev->message & convertClipboardFlag ;
				bool doesActivate = UMAGetProcessModeDoesActivateOnFGSwitch() ;
				if ( isResuming )
				{
					WindowRef oldFrontWindow = NULL ;
					WindowRef newFrontWindow = NULL ;

					// in case we don't take care of activating ourselves, we have to synchronize
					// our idea of the active window with the process manager's - which it already activated

					if ( !doesActivate )
						oldFrontWindow = UMAFrontNonFloatingWindow() ;

					MacResume( convertClipboard ) ;

					newFrontWindow = UMAFrontNonFloatingWindow() ;

					if ( oldFrontWindow )
					{
						wxWindow* win = wxFindWinFromMacWindow( oldFrontWindow ) ;
						if ( win )
							win->MacActivate( ev , false ) ;
					}
					if ( newFrontWindow )
					{
						wxWindow* win = wxFindWinFromMacWindow( newFrontWindow ) ;
						if ( win )
							win->MacActivate( ev , true ) ;
					}
				}
				else
				{
					MacSuspend( convertClipboard ) ;

					// in case this suspending did close an active window, another one might
					// have surfaced -> lets deactivate that one

					WindowRef newActiveWindow = UMAGetActiveNonFloatingWindow() ;
					if ( newActiveWindow )
					{
						wxWindow* win = wxFindWinFromMacWindow( newActiveWindow ) ;
						if ( win )
							win->MacActivate( ev , false ) ;
					}
				}
			}
			break ;
		case mouseMovedMessage :
			{
				WindowRef window;

				wxWindow* currentMouseWindow = NULL ;

				MacGetWindowFromPoint( wxPoint( ev->where.h , ev->where.v ) , &currentMouseWindow ) ;

				if ( currentMouseWindow != wxWindow::s_lastMouseWindow )
				{
					wxMouseEvent event ;

					bool isDown = !(ev->modifiers & btnState) ; // 1 is for up
					bool controlDown = ev->modifiers & controlKey ; // for simulating right mouse

					event.m_leftDown = isDown && !controlDown;
					event.m_middleDown = FALSE;
					event.m_rightDown = isDown && controlDown;
					event.m_shiftDown = ev->modifiers & shiftKey;
					event.m_controlDown = ev->modifiers & controlKey;
					event.m_altDown = ev->modifiers & optionKey;
					event.m_metaDown = ev->modifiers & cmdKey;
					event.m_x = ev->where.h;
					event.m_y = ev->where.v;
					event.m_timeStamp = ev->when;
					event.SetEventObject(this);

					if ( wxWindow::s_lastMouseWindow )
					{
						wxMouseEvent eventleave(event ) ;
						eventleave.SetEventType( wxEVT_LEAVE_WINDOW ) ;
						wxWindow::s_lastMouseWindow->GetEventHandler()->ProcessEvent(eventleave);
					}
					if ( currentMouseWindow )
					{
						wxMouseEvent evententer(event ) ;
						evententer.SetEventType( wxEVT_ENTER_WINDOW ) ;
						currentMouseWindow->GetEventHandler()->ProcessEvent(evententer);
					}
					wxWindow::s_lastMouseWindow = currentMouseWindow ;
				}

				short windowPart = ::FindWindow(ev->where, &window);

				switch (windowPart)
				{
					case inMenuBar :
						break ;
					case inSysWindow :
						break ;
					default:
						{
							if ( s_lastMouseDown == 0 )
								ev->modifiers |= btnState ;

							wxWindow* win = wxFindWinFromMacWindow( window ) ;
							if ( win )
								win->MacMouseMoved( ev , windowPart ) ;
						}
						break;
				}
			}
			break ;

	}
}

void wxApp::MacHandleMenuSelect( int macMenuId , int macMenuItemNum )
{
	if (macMenuId == 0)
		 return; // no menu item selected

	if (macMenuId == kwxMacAppleMenuId && macMenuItemNum > 1)
	{
		#if ! TARGET_CARBON
		Str255		deskAccessoryName ;
		GrafPtr		savedPort ;

		GetMenuItemText(GetMenuHandle(kwxMacAppleMenuId), macMenuItemNum, deskAccessoryName);
		GetPort(&savedPort);
		OpenDeskAcc(deskAccessoryName);
		SetPort(savedPort);
		#endif
	}
	else
	{
		wxWindow* frontwindow = wxFindWinFromMacWindow( ::FrontWindow() )  ;
		if ( frontwindow && wxMenuBar::MacGetInstalledMenuBar() )
			wxMenuBar::MacGetInstalledMenuBar()->MacMenuSelect( frontwindow->GetEventHandler() , 0 , macMenuId , macMenuItemNum ) ;
	}
	HiliteMenu(0);
}

/*
long wxApp::MacTranslateKey(char key, int mods)
{
}

void wxApp::MacAdjustCursor()
{
}

*/
/*
void
wxApp::macAdjustCursor()
{
  if (ev->what != kHighLevelEvent)
  {
	wxWindow* theMacWxFrame = wxFrame::MacFindFrameOrDialog(::FrontWindow());
	if (theMacWxFrame)
	{
  	  if (!theMacWxFrame->MacAdjustCursor(ev->where))
		::SetCursor(&(qd.arrow));
  	}
  }
}
*/
