/////////////////////////////////////////////////////////////////////////////
// Name:        app.h
// Purpose:     wxApp class
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_APP_H_
#define _WX_APP_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "app.h"
#endif

#include "wx/defs.h"
#include "wx/object.h"
#include "wx/gdicmn.h"
#include "wx/event.h"

#ifdef __WXMAC_OSX__
typedef struct __CFRunLoopSource * CFRunLoopSourceRef;
#endif

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
    virtual bool Pending() ;
    virtual bool Dispatch() ;

    virtual void Exit();

    virtual bool Yield(bool onlyIfNeeded = FALSE);
    virtual void WakeUpIdle();

    virtual void SetPrintMode(int mode) { m_printMode = mode; }
    virtual int GetPrintMode() const { return m_printMode; }

#if wxUSE_GUI
    // setting up all MacOS Specific Event-Handlers etc
    virtual bool OnInitGui();
#endif // wxUSE_GUI
    // implementation only
    void OnIdle(wxIdleEvent& event);
    void OnEndSession(wxCloseEvent& event);
    void OnQueryEndSession(wxCloseEvent& event);

protected:
    bool                  m_showOnInit;
    int                   m_printMode; // wxPRINT_WINDOWS, wxPRINT_POSTSCRIPT

public:

    static bool           sm_isEmbedded;
    // Implementation
    virtual bool Initialize(int& argc, wxChar **argv);
    virtual void CleanUp();

    bool IsExiting() { return !m_keepGoing ; }

    // the installed application event handler
    WXEVENTHANDLERREF    MacGetEventHandler() { return m_macEventHandler ; }
    WXEVENTHANDLERREF    MacGetCurrentEventHandlerCallRef() { return m_macCurrentEventHandlerCallRef ; }
    void MacSetCurrentEvent( WXEVENTREF event , WXEVENTHANDLERCALLREF handler )
    { m_macCurrentEvent = event ; m_macCurrentEventHandlerCallRef = handler ; }

public:
    static long           sm_lastMessageTime;
    static wxWindow*      s_captureWindow ;
    static int            s_lastMouseDown ; // 0 = none , 1 = left , 2 = right
    static WXHRGN         s_macCursorRgn ;
    static long           s_lastModifiers ;

    int                   m_nCmdShow;

private:
    bool                  m_keepGoing ;

    // mac specifics

    WXEVENTHANDLERREF     m_macEventHandler ;
    WXEVENTHANDLERCALLREF m_macCurrentEventHandlerCallRef ;
    WXEVENTREF            m_macCurrentEvent ;
#ifdef __WXMAC_OSX__
    CFRunLoopSourceRef    m_macEventPosted ;
#endif

public:
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

    void                  MacDoOneEvent() ;
    WXEVENTREF            MacGetCurrentEvent() { return m_macCurrentEvent ; }
    void                  MacHandleOneEvent( WXEVENTREF ev ) ;

    // For embedded use. By default does nothing.
    virtual void          MacHandleUnhandledEvent( WXEVENTREF ev );

    bool    MacSendKeyDownEvent( wxWindow* focus , long keyval , long modifiers , long when , short wherex , short wherey , wxChar uniChar ) ;
    bool    MacSendKeyUpEvent( wxWindow* focus , long keyval , long modifiers , long when , short wherex , short wherey , wxChar uniChar ) ;

    virtual short         MacHandleAEODoc(const WXAPPLEEVENTREF event , WXAPPLEEVENTREF reply) ;
    virtual short         MacHandleAEPDoc(const WXAPPLEEVENTREF event , WXAPPLEEVENTREF reply) ;
    virtual short         MacHandleAEOApp(const WXAPPLEEVENTREF event , WXAPPLEEVENTREF reply) ;
    virtual short         MacHandleAEQuit(const WXAPPLEEVENTREF event , WXAPPLEEVENTREF reply) ;
    virtual short         MacHandleAERApp(const WXAPPLEEVENTREF event , WXAPPLEEVENTREF reply) ;

    // in response of an open-document apple event
    virtual void         MacOpenFile(const wxString &fileName) ;
    // in response of a print-document apple event
    virtual void         MacPrintFile(const wxString &fileName) ;
    // in response of a open-application apple event
    virtual void         MacNewFile() ;
    // in response of a reopen-application apple event
    virtual void         MacReopenApp() ;

    DECLARE_EVENT_TABLE()
};

#endif
    // _WX_APP_H_

