/////////////////////////////////////////////////////////////////////////////
// Name:        app.h
// Purpose:     wxApp class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_APP_H_
#define _WX_APP_H_

#ifdef __GNUG__
#pragma interface "app.h"
#endif

#include "wx/event.h"
#include "wx/icon.h"

class WXDLLEXPORT wxFrame;
class WXDLLEXPORT wxWindow;
class WXDLLEXPORT wxApp ;
class WXDLLEXPORT wxKeyEvent;
class WXDLLEXPORT wxLog;

// Represents the application. Derive OnInit and declare
// a new App object to start application
class WXDLLEXPORT wxApp : public wxAppBase
{
    DECLARE_DYNAMIC_CLASS(wxApp)

public:
    wxApp();
    virtual ~wxApp();

    // override base class (pure) virtuals
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

    void SetAuto3D(bool flag) { m_auto3D = flag; }
    bool GetAuto3D() const { return m_auto3D; }

protected:
    bool              m_showOnInit;
    int               m_printMode; // wxPRINT_WINDOWS, wxPRINT_POSTSCRIPT
    bool              m_auto3D ;   // Always use 3D controls, except where overriden

    /* Windows-specific wxApp definitions */

public:

    // Implementation
    static bool Initialize();
    static void CleanUp();

    static bool RegisterWindowClasses();
    // Convert Windows to argc, argv style
    void ConvertToStandardCommandArgs(char* p);
    virtual bool DoMessage();
    virtual bool ProcessMessage(WXMSG* pMsg);
    void DeletePendingObjects();
    bool ProcessIdle();
#if wxUSE_THREADS
    void ProcessPendingEvents();
#endif
    int GetComCtl32Version() const;

public:
    int               m_nCmdShow;

protected:
    bool              m_keepGoing ;

    DECLARE_EVENT_TABLE()
};

#if !defined(_WINDLL) || (defined(_WINDLL) && defined(WXMAKINGDLL))
int WXDLLEXPORT wxEntry(WXHINSTANCE hInstance, WXHINSTANCE hPrevInstance, char *lpszCmdLine,
                    int nCmdShow, bool enterLoop = TRUE);
#else
int WXDLLEXPORT wxEntry(WXHINSTANCE hInstance);
#endif

#endif
    // _WX_APP_H_

