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
class WXDLLEXPORT wxApp;
class WXDLLEXPORT wxKeyEvent;
class WXDLLEXPORT wxLog;
class WXDLLEXPORT wxEventLoop;

// ----------------------------------------------------------------------------
// the wxApp class for wxX11 - see wxAppBase for more details
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
    virtual bool Pending();
    virtual void Dispatch();
    virtual bool Yield(bool onlyIfNeeded = FALSE);
    
    virtual bool OnInitGui();
    
    // implementation from now on
    // --------------------------
    
    void OnIdle(wxIdleEvent& event);
    
    // Send idle event to all top-level windows.
    // Returns TRUE if more idle time is requested.
    bool SendIdleEvents();
    
    // Send idle event to window and all subwindows
    // Returns TRUE if more idle time is requested.
    bool SendIdleEvents(wxWindow* win);
    
    // Processes an X event.
    virtual bool ProcessXEvent(WXEvent* event);
    
    virtual void OnAssert(const wxChar *file, int line, const wxChar *msg);
    
protected:
    bool                  m_showOnInit;
    
public:
    // Implementation
    static bool Initialize();
    static void CleanUp();
    
    void DeletePendingObjects();
    bool ProcessIdle();
    
    WXWindow       GetTopLevelWidget() const { return m_topLevelWidget; }
    WXColormap     GetMainColormap(WXDisplay* display);
    long           GetMaxRequestSize() const { return m_maxRequestSize; }
    
    // This handler is called when a property change event occurs
    virtual bool HandlePropertyChange(WXEvent *event);
    
    // We need this before create the app
    static   WXDisplay* GetDisplay() { return ms_display; }
    static   WXDisplay* ms_display;

    // Values that can be passed on the command line.
    // Returns -1, -1 if none specified.
    const wxSize& GetInitialSize() const { return m_initialSize; }
    bool GetShowIconic() const { return m_showIconic; }
    
public:
    static long           sm_lastMessageTime;
    bool                  m_showIconic;    
    wxSize                m_initialSize;
protected:
    bool                  m_keepGoing;
    
    WXWindow              m_topLevelWidget;
    WXColormap            m_mainColormap;
    long                  m_maxRequestSize;
    wxEventLoop*          m_mainLoop;
    
    DECLARE_EVENT_TABLE()
};

int WXDLLEXPORT wxEntry( int argc, char *argv[] );

#endif
// _WX_APP_H_

