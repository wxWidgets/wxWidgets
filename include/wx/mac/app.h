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

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "app.h"
#endif

#include "wx/defs.h"
#include "wx/object.h"
#include "wx/gdicmn.h"
#include "wx/event.h"

class WXDLLEXPORT wxFrame;
class WXDLLEXPORT wxWindowMac;
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
    virtual bool Yield(bool onlyIfNeeded = FALSE);
    virtual bool ProcessIdle();
    
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
    bool SendIdleEvents(wxWindowMac* win);
    
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
    
    virtual bool OnInit();
    void DeletePendingObjects();
    bool IsExiting() { return !m_keepGoing ; }
    
public:
    static long           sm_lastMessageTime;
    static wxWindow*      s_captureWindow ;
    static int            s_lastMouseDown ; // 0 = none , 1 = left , 2 = right
    static WXHRGN         s_macCursorRgn ;
    static long           s_lastModifiers ;
    WXEVENTREF            m_macCurrentEvent ;
    
    int                   m_nCmdShow;
    
protected:
    bool                  m_keepGoing ;

    // mac specifics

public:
    static bool           s_macDefaultEncodingIsPC ;
    static bool           s_macSupportPCMenuShortcuts ;
    static long           s_macAboutMenuItemId ;
    static long           s_macPreferencesMenuItemId ;
    static long           s_macExitMenuItemId ;
    static wxString       s_macHelpMenuTitleName ;

    static bool           s_macHasAppearance ;
    static long           s_macAppearanceVersion ;
    static bool           s_macHasNavigation ;
    static bool           s_macNavigationVersion ;
    static bool           s_macHasWindowManager ;
    static long           s_macWindowManagerVersion ;
    static bool           s_macHasMenuManager ;
    static long           s_macMenuManagerVersion ;
    static bool           s_macHasDialogManager ;
    static long           s_macDialogManagerVersion ;

    WXHRGN                m_macCursorRgn ;
    WXHRGN                m_macSleepRgn ;
    WXHRGN                m_macHelpRgn ;
    
    virtual void          MacSuspend( bool convertClipboard ) ;
    virtual void          MacResume( bool convertClipboard ) ;

    virtual void          MacConvertPrivateToPublicScrap() ;
    virtual void          MacConvertPublicToPrivateScrap() ;

    // event main methods

    void                  MacDoOneEvent() ;
    void                  MacHandleOneEvent( WXEVENTREF ev ) ;
    void                  MacHandleModifierEvents( WXEVENTREF ev ) ;
    WXEVENTREF            MacGetCurrentEvent() { return m_macCurrentEvent ; }
  
    // primary events
	
    virtual void          MacHandleMouseDownEvent( WXEVENTREF ev ) ;
    virtual void          MacHandleMouseUpEvent( WXEVENTREF ev ) ;
    virtual void          MacHandleActivateEvent( WXEVENTREF ev ) ;
    virtual void          MacHandleUpdateEvent( WXEVENTREF ev ) ;
    virtual void          MacHandleKeyDownEvent( WXEVENTREF ev ) ;
    virtual void          MacHandleKeyUpEvent( WXEVENTREF ev ) ;
    virtual void          MacHandleDiskEvent( WXEVENTREF ev ) ;
    virtual void          MacHandleOSEvent( WXEVENTREF ev ) ;
    virtual void          MacHandleHighLevelEvent( WXEVENTREF ev ) ;
    virtual void          MacHandleMenuSelect( int menuid , int menuitem ) ;
    
    virtual bool          MacSendKeyDownEvent( wxWindow* focus , long keyval , long modifiers , long when , short wherex , short wherey ) ;
    virtual bool          MacSendKeyUpEvent( wxWindow* focus , long keyval , long modifiers , long when , short wherex , short wherey ) ;
    
    virtual short         MacHandleAEODoc(const WXAPPLEEVENTREF event , WXAPPLEEVENTREF reply) ;
    virtual short         MacHandleAEPDoc(const WXAPPLEEVENTREF event , WXAPPLEEVENTREF reply) ;
    virtual short         MacHandleAEOApp(const WXAPPLEEVENTREF event , WXAPPLEEVENTREF reply) ;
    virtual short         MacHandleAEQuit(const WXAPPLEEVENTREF event , WXAPPLEEVENTREF reply) ;
    
    // in response of an open-document apple event
    virtual void   	  MacOpenFile(const wxString &fileName) ;
    // in response of a print-document apple event
    virtual void   	  MacPrintFile(const wxString &fileName) ;
    // in response of a open-application apple event
    virtual void   	  MacNewFile() ;

    DECLARE_EVENT_TABLE()
};

class WXDLLEXPORT wxStAppResource
{
public:
    wxStAppResource() ;
    ~wxStAppResource() ;

    // opaque pointer for CFragInitBlock
    static void OpenSharedLibraryResource(const void *) ;
    static void CloseSharedLibraryResource() ;
    
private:    
    short       m_currentRefNum ;
} ;

// TODO: add platform-specific arguments
#ifndef __DARWIN__
int WXDLLEXPORT wxEntry( int argc, char *argv[] , bool enterLoop = TRUE);
#endif

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

// converts this string into a pascal with optional pc 2 mac encoding
void wxMacStringToPascal( const char * from , unsigned char* to , bool pc2macEncoding ) ;

// converts this string into a pascal with pc 2 mac encoding if s_macDefaultEncodingIsPC
inline void wxMacStringToPascal( const char * from , unsigned char*  to ) 
  { wxMacStringToPascal( from , to , wxApp::s_macDefaultEncodingIsPC ) ; }

// converts this string into a pascal with optional mac 2 pc encoding
wxString wxMacMakeStringFromPascal( unsigned char*  from , bool mac2pcEncoding ) ;

// converts this pascal string into a wxString with pc 2 mac encoding if s_macDefaultEncodingIsPC
inline wxString wxMacMakeStringFromPascal( unsigned char*  from  ) 
  { return wxMacMakeStringFromPascal( from , wxApp::s_macDefaultEncodingIsPC ) ; }

// converts this c string into a wxString with optional mac 2 pc encoding
wxString wxMacMakeStringFromMacString( const char* from , bool mac2pcEncoding ) ;

// converts this c string into a wxString with pc 2 mac encoding if s_macDefaultEncodingIsPC
inline wxString wxMacMakeStringFromMacString( const char* from  ) 
  { return wxMacMakeStringFromMacString( from , wxApp::s_macDefaultEncodingIsPC ) ; }

#endif
    // _WX_APP_H_

