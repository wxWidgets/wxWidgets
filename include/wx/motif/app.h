/////////////////////////////////////////////////////////////////////////////
// Name:        app.h
// Purpose:     wxApp class
// Author:      Julian Smart
// Modified by:
// Created:     17/09/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_APP_H_
#define _WX_APP_H_

#ifdef __GNUG__
    #pragma interface "app.h"
#endif

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "wx/gdicmn.h"
#include "wx/event.h"

// ----------------------------------------------------------------------------
// forward declarations
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxFrame;
class WXDLLEXPORT wxWindow;
class WXDLLEXPORT wxApp ;
class WXDLLEXPORT wxKeyEvent;
class WXDLLEXPORT wxLog;

// ----------------------------------------------------------------------------
// the wxApp class for Motif - see wxAppBase for more details
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxApp : public wxAppBase
{
DECLARE_DYNAMIC_CLASS(wxApp)

public:
    wxApp();
    ~wxApp() {}

    // override base class (pure) virtuals
    // -----------------------------------

    virtual int MainLoop();
    virtual void ExitMainLoop();
    virtual bool Initialized();
    virtual bool Pending() ;
    virtual void Dispatch() ;

    virtual bool OnInitGui();

    virtual wxIcon GetStdIcon(int which) const;

    // implementation from now on
    // --------------------------

    void OnIdle(wxIdleEvent& event);

    // Send idle event to all top-level windows.
    // Returns TRUE if more idle time is requested.
    bool SendIdleEvents();

    // Send idle event to window and all subwindows
    // Returns TRUE if more idle time is requested.
    bool SendIdleEvents(wxWindow* win);

    // Motif implementation.

    // Processes an X event.
    virtual void ProcessXEvent(WXEvent* event);

    // Returns TRUE if an accelerator has been processed
    virtual bool CheckForAccelerator(WXEvent* event);

    // Returns TRUE if a key down event has been processed
    virtual bool CheckForKeyDown(WXEvent* event);

    // Returns TRUE if a key up event has been processed
    virtual bool CheckForKeyUp(WXEvent* event);

protected:
    bool                  m_showOnInit;

public:
    // Implementation
    static bool Initialize();
    static void CleanUp();

    void DeletePendingObjects();
    bool ProcessIdle();
#if wxUSE_THREADS
    void ProcessPendingEvents();
#endif

    // Motif-specific
    WXAppContext   GetAppContext() const { return m_appContext; }
    WXWidget       GetTopLevelWidget() const { return m_topLevelWidget; }
    WXColormap     GetMainColormap(WXDisplay* display) ;
    WXDisplay*     GetInitialDisplay() const { return m_initialDisplay; }
    long           GetMaxRequestSize() const { return m_maxRequestSize; }

    // This handler is called when a property change event occurs
    virtual void   HandlePropertyChange(WXEvent *event);

public:
    static long    sm_lastMessageTime;
    int            m_nCmdShow;

protected:
    bool                  m_keepGoing ;

    // Motif-specific
    WXAppContext          m_appContext;
    WXWidget              m_topLevelWidget;
    WXColormap            m_mainColormap;
    WXDisplay*            m_initialDisplay;
    long                  m_maxRequestSize;

    DECLARE_EVENT_TABLE()
};

int WXDLLEXPORT wxEntry( int argc, char *argv[] );

#endif
    // _WX_APP_H_

