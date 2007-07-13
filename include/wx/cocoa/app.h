/////////////////////////////////////////////////////////////////////////////
// Name:        cocoa/app.h
// Purpose:     wxApp class
// Author:      David Elliott
// Modified by:
// Created:     2002/11/27
// RCS-ID:      $Id$
// Copyright:   (c) 2002 David Elliott
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_COCOA_APP_H_
#define _WX_COCOA_APP_H_

typedef struct __CFRunLoopObserver * CFRunLoopObserverRef;
typedef const struct __CFString * CFStringRef;

#include "wx/mac/corefoundation/cfref.h"

// ========================================================================
// wxApp
// ========================================================================
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
// ABI compatibility warning: This was implemented inline.  wxCocoa apps
// compiled against < 2.8.5 won't call the new implementation which cleans up
// the CFRunLoop idle observer.  Fortunately, Cleanup does that in 2.8.
    virtual ~wxApp();

// ------------------------------------------------------------------------
// Cocoa specifics
// ------------------------------------------------------------------------
public:
    inline WX_NSApplication GetNSApplication() { return m_cocoaApp; }
    virtual void CocoaDelegate_applicationWillBecomeActive();
    virtual void CocoaDelegate_applicationDidBecomeActive();
    virtual void CocoaDelegate_applicationWillResignActive();
    virtual void CocoaDelegate_applicationDidResignActive();
    /* The following two cannot be virtual due to 2.8 ABI compatibility */
    /*virtual*/ void CocoaDelegate_applicationWillUpdate();
    /*virtual*/ void CF_ObserveMainRunLoopBeforeWaiting(CFRunLoopObserverRef observer, int activity);
protected:
    WX_NSApplication m_cocoaApp;
    struct objc_object *m_cocoaAppDelegate;
    WX_NSThread m_cocoaMainThread;

// ------------------------------------------------------------------------
// Implementation
// ------------------------------------------------------------------------
public:
    // Implement wxAppBase pure virtuals
    virtual void Exit();

    virtual bool Yield(bool onlyIfNeeded = FALSE);
    virtual void WakeUpIdle();
    
    virtual bool Initialize(int& argc, wxChar **argv);
    virtual void CleanUp();
    virtual bool CallOnInit();

    
    virtual bool OnInit();
    virtual bool OnInitGui();

#ifdef __WXDEBUG__
    virtual void OnAssert(const wxChar *file, int line, const wxChar *cond, const wxChar *msg);
    bool IsInAssert() const { return m_isInAssert; }
#endif // __WXDEBUG__

    // Set true _before_ initializing wx to force embedded mode (no app delegate, etc.)
    static bool sm_isEmbedded;
private:
#ifdef __WXDEBUG__
    bool m_isInAssert;
#endif // __WXDEBUG__
};

#endif // _WX_COCOA_APP_H_
