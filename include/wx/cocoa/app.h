/////////////////////////////////////////////////////////////////////////////
// Name:        cocoa/app.h
// Purpose:     wxApp class
// Author:      David Elliott
// Modified by:
// Created:     2002/11/27
// RCS-ID:      $Id:
// Copyright:   (c) 2002 David Elliott
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_COCOA_APP_H_
#define _WX_COCOA_APP_H_

// Represents the application. Derive OnInit and declare
// a new App object to start application
class WXDLLEXPORT wxApp: public wxAppBase
{
    DECLARE_DYNAMIC_CLASS(wxApp)
    DECLARE_EVENT_TABLE()
// ------------------------------------------------------------------------
// initialization
// ------------------------------------------------------------------------
public:
    wxApp();
    virtual ~wxApp() {}

// ------------------------------------------------------------------------
// Cocoa specifics
// ------------------------------------------------------------------------
public:
    inline WX_NSApplication GetNSApplication() { return m_cocoaApp; }
    void CocoaInstallRequestedIdleHandler() { if(m_isIdle) CocoaInstallIdleHandler(); }
    inline void CocoaRequestIdle() { m_isIdle = true; }
protected:
    WX_NSApplication m_cocoaApp;
    void CocoaInstallIdleHandler();
    bool m_isIdle;

// ------------------------------------------------------------------------
// Implementation
// ------------------------------------------------------------------------
public:
    // Implement wxAppBase pure virtuals
    virtual int MainLoop();
    virtual void ExitMainLoop();
    virtual bool Initialized();
    virtual bool Pending();
    virtual void Dispatch();

    virtual void Exit();

    virtual bool Yield(bool onlyIfNeeded = FALSE);
    virtual bool ProcessIdle();
    virtual void WakeUpIdle() { CocoaRequestIdle(); }
    
    /* Idle Processing */
    void OnIdle(wxIdleEvent& event);
    // Send idle event to all top-level windows.
    // Returns TRUE if more idle time is requested.
    bool SendIdleEvents();
    // Send idle event to window and all subwindows
    // Returns TRUE if more idle time is requested.
    bool SendIdleEvents(wxWindowCocoa* win);
    
    virtual bool Initialize(int& argc, wxChar **argv);
    virtual void CleanUp();
    virtual bool CallOnInit();

    
    virtual bool OnInit();
    virtual bool OnInitGui();
};

#endif // _WX_COCOA_APP_H_
