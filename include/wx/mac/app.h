/////////////////////////////////////////////////////////////////////////////
// Name:        app.h
// Purpose:     wxApp class
// Author:      AUTHOR
// Modified by:
// Created:     ??/??/98
// RCS-ID:      $Id$
// Copyright:   (c) AUTHOR
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_APP_H_
#define _WX_APP_H_

#ifdef __GNUG__
#pragma interface "app.h"
#endif

#include "wx/defs.h"
#include "wx/object.h"
#include "wx/gdicmn.h"
#include "wx/event.h"

class WXDLLEXPORT wxFrame;
class WXDLLEXPORT wxWindow;
class WXDLLEXPORT wxApp ;
class WXDLLEXPORT wxKeyEvent;
class WXDLLEXPORT wxLog;

#define wxPRINT_WINDOWS         1
#define wxPRINT_POSTSCRIPT      2

WXDLLEXPORT_DATA(extern wxApp*) wxTheApp;

// Force an exit from main loop
void WXDLLEXPORT wxExit();

// Yield to other apps/messages
bool WXDLLEXPORT wxYield();

// Represents the application. Derive OnInit and declare
// a new App object to start application
class WXDLLEXPORT wxApp: public wxAppBase
{
  DECLARE_DYNAMIC_CLASS(wxApp)
  wxApp();
  virtual ~wxApp() {}

  virtual int MainLoop();
  virtual void ExitMainLoop();
  virtual bool Initialized();
  virtual bool Pending() ;
  virtual void Dispatch() ;

  virtual wxIcon GetStdIcon(int which) const;
  virtual void SetPrintMode(int mode) { m_printMode = mode; }
  virtual int GetPrintMode() const { return m_printMode; }

  // implementation only
  void OnIdle(wxIdleEvent& event);
  void OnEndSession(wxCloseEvent& event);
  void OnQueryEndSession(wxCloseEvent& event);

  // Send idle event to all top-level windows.
  // Returns TRUE if more idle time is requested.
  bool SendIdleEvents();

  // Send idle event to window and all subwindows
  // Returns TRUE if more idle time is requested.
  bool SendIdleEvents(wxWindow* win);

  // Windows only, but for compatibility...
  inline void SetAuto3D(bool flag) { m_auto3D = flag; }
  inline bool GetAuto3D() const { return m_auto3D; }

protected:
  bool                  m_showOnInit;
  int                   m_printMode; // wxPRINT_WINDOWS, wxPRINT_POSTSCRIPT
  bool                  m_auto3D ;   // Always use 3D controls, except
                                 // where overriden
public:

  // Implementation
  static bool Initialize();
  static void CleanUp();

  void DeletePendingObjects();
  bool ProcessIdle();
  bool IsExiting() { return !m_keepGoing ; }

public:
  static long           sm_lastMessageTime;
  static wxWindow*			s_captureWindow ;
  static int						s_lastMouseDown ; // 0 = none , 1 = left , 2 = right
  static RgnHandle			s_macCursorRgn ;
	EventRecord*					m_macCurrentEvent ;
  
  int                   m_nCmdShow;

protected:
  bool                  m_keepGoing ;

// mac specifics

public :
	static bool						s_macDefaultEncodingIsPC ;
	static bool						s_macSupportPCMenuShortcuts ;
	static long						s_macAboutMenuItemId ;
	static wxString				s_macHelpMenuTitleName ;

  static bool						s_macHasAppearance ;
  static long						s_macAppearanceVersion ;
  static bool						s_macHasNavigation ;
  static bool						s_macNavigationVersion ;
  static bool						s_macHasWindowManager ;
  static long						s_macWindowManagerVersion ;
  static bool						s_macHasMenuManager ;
  static long						s_macMenuManagerVersion ;
  static bool						s_macHasDialogManager ;
  static long						s_macDialogManagerVersion ;

	RgnHandle							m_macCursorRgn ;
	RgnHandle							m_macSleepRgn ;
	RgnHandle							m_macHelpRgn ;
	
  virtual void          MacSuspend( bool convertClipboard ) ;
  virtual void          MacResume( bool convertClipboard ) ;

	virtual void					MacConvertPrivateToPublicScrap() ;
	virtual void					MacConvertPublicToPrivateScrap() ;

	// event main methods

	void									MacDoOneEvent() ;	
	void									MacHandleOneEvent( EventRecord *ev ) ;
	EventRecord*					MacGetCurrentEvent() { return m_macCurrentEvent ; }
	// primary events
	
  virtual void          MacHandleMouseDownEvent( EventRecord *ev ) ;
  virtual void          MacHandleMouseUpEvent( EventRecord *ev ) ;
 	virtual void          MacHandleActivateEvent( EventRecord *ev ) ;
  virtual void          MacHandleUpdateEvent( EventRecord *ev ) ;
  virtual void          MacHandleKeyDownEvent( EventRecord *ev ) ;
  virtual void          MacHandleKeyUpEvent( EventRecord *ev ) ;
  virtual void          MacHandleDiskEvent( EventRecord *ev ) ;
  virtual void          MacHandleOSEvent( EventRecord *ev ) ;
  virtual void   				MacHandleHighLevelEvent( EventRecord *ev ) ;
 	virtual void					MacHandleMenuSelect( int menuid , int menuitem ) ;

	virtual OSErr					MacHandleAEODoc(AppleEvent *event , AppleEvent *reply) ;
	virtual OSErr					MacHandleAEPDoc(AppleEvent *event , AppleEvent *reply) ;
	virtual OSErr					MacHandleAEOApp(AppleEvent *event , AppleEvent *reply) ;
	virtual OSErr					MacHandleAEQuit(AppleEvent *event , AppleEvent *reply) ;

DECLARE_EVENT_TABLE()
};

// TODO: add platform-specific arguments
int WXDLLEXPORT wxEntry( int argc, char *argv[] , bool enterLoop = TRUE);

void wxMacConvertFromPCForControls( char * p ) ;

void wxMacConvertToPC( const char *from , char *to , int len ) ;
void wxMacConvertFromPC( const char *from , char *to , int len ) ;
void wxMacConvertToPC( const char *from , char *to , int len ) ;
void wxMacConvertFromPC( char * p ) ;
void wxMacConvertFromPC( unsigned char *p ) ;
wxString wxMacMakeMacStringFromPC( const char * p ) ;
void wxMacConvertToPC( char * p ) ;
void wxMacConvertToPC( unsigned char *p ) ;
wxString wxMacMakePCStringFromMac( const char * p ) ;

#endif
    // _WX_APP_H_

