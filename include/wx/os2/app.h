/////////////////////////////////////////////////////////////////////////////
// Name:        app.h
// Purpose:     wxApp class
// Author:      David Webster
// Modified by:
// Created:     10/13/99
// RCS-ID:      $Id$
// Copyright:   (c) David Webster
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_APP_H_
#define _WX_APP_H_

#include "wx/event.h"
#include "wx/icon.h"

class WXDLLEXPORT wxFrame;
class WXDLLEXPORT wxWindow;
class WXDLLEXPORT wxApp ;
class WXDLLEXPORT wxKeyEvent;
class WXDLLEXPORT wxLog;

WXDLLEXPORT_DATA(extern wxApp*) wxTheApp;
WXDLLEXPORT_DATA(extern HAB)    vHabmain;

// Force an exit from main loop
void WXDLLEXPORT wxExit(void);

// Yield to other apps/messages
bool WXDLLEXPORT wxYield(void);

extern MRESULT EXPENTRY wxWndProc( HWND
                                  ,ULONG
                                  ,MPARAM
                                  ,MPARAM
                                 );

// Represents the application. Derive OnInit and declare
// a new App object to start application
class WXDLLEXPORT wxApp : public wxAppBase
{
    DECLARE_DYNAMIC_CLASS(wxApp)

public:
    wxApp();
    virtual ~wxApp();

    virtual bool OnInitGui(void);

    // override base class (pure) virtuals
    virtual int  MainLoop(void);
    virtual void ExitMainLoop(void);
    virtual bool Initialized(void);
    virtual bool Pending(void) ;
    virtual void Dispatch(void);

    virtual wxIcon GetStdIcon(int which) const;

    virtual void SetPrintMode(int mode) { m_nPrintMode = mode; }
    virtual int  GetPrintMode(void) const { return m_nPrintMode; }

    // implementation only
    void OnIdle(wxIdleEvent& rEvent);
    void OnEndSession(wxCloseEvent& rEvent);
    void OnQueryEndSession(wxCloseEvent& rEvent);

    // Send idle event to all top-level windows.
    // Returns TRUE if more idle time is requested.
    bool SendIdleEvents(void);

    // Send idle event to window and all subwindows
    // Returns TRUE if more idle time is requested.
    bool SendIdleEvents(wxWindow* pWin);

    void SetAuto3D(bool bFlag) { m_bAuto3D = bFlag; }
    bool GetAuto3D(void) const { return m_bAuto3D; }

protected:
    bool                            m_bShowOnInit;
    int                             m_nPrintMode; // wxPRINT_WINDOWS, wxPRINT_POSTSCRIPT
    bool                            m_bAuto3D ;   // Always use 3D controls, except where overriden

    //
    // PM-specific wxApp definitions */
    //
public:

    // Implementation
    static bool  Initialize(HAB vHab);
    static void  CleanUp(void);

    static bool  RegisterWindowClasses(HAB vHab);
    virtual bool DoMessage(void);
    virtual bool ProcessMessage(WXMSG* pMsg);
    void         DeletePendingObjects(void);
    bool         ProcessIdle(void);

public:
    int                             m_nCmdShow;

protected:
    bool                            m_bKeepGoing ;

    DECLARE_EVENT_TABLE()
private:
    HMQ                             m_hMq;
};

int WXDLLEXPORT wxEntry( int argc, char *argv[] );
#endif
    // _WX_APP_H_

