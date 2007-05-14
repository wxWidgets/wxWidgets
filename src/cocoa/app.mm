/////////////////////////////////////////////////////////////////////////////
// Name:        src/cocoa/app.mm
// Purpose:     wxApp
// Author:      David Elliott
// Modified by:
// Created:     2002/11/27
// RCS-ID:      $Id$
// Copyright:   (c) David Elliott
//              Software 2000 Ltd.
// Licence:     wxWidgets licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#include "wx/app.h"

#ifndef WX_PRECOMP
    #include "wx/dc.h"
    #include "wx/intl.h"
    #include "wx/log.h"
    #include "wx/module.h"
#endif

#include "wx/cocoa/ObjcRef.h"
#include "wx/cocoa/ObjcPose.h"
#include "wx/cocoa/autorelease.h"
#include "wx/cocoa/mbarman.h"
#include "wx/cocoa/NSApplication.h"

#import <AppKit/NSApplication.h>
#import <Foundation/NSRunLoop.h>
#import <Foundation/NSThread.h>
#import <AppKit/NSEvent.h>
#import <Foundation/NSString.h>
#import <Foundation/NSNotification.h>
#import <AppKit/NSCell.h>

// wxNSApplicationObserver singleton.
static wxObjcAutoRefFromAlloc<wxNSApplicationObserver*> sg_cocoaAppObserver = [[wxNSApplicationObserver alloc] init];

// ========================================================================
// wxPoseAsInitializer
// ========================================================================
wxPoseAsInitializer *wxPoseAsInitializer::sm_first = NULL;

// ========================================================================
// wxNSApplicationDelegate
// ========================================================================
@implementation wxNSApplicationDelegate : NSObject

// NOTE: Terminate means that the event loop does NOT return and thus
// cleanup code doesn't properly execute.  Furthermore, wxWidgets has its
// own exit on frame delete mechanism.
- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)theApplication
{
    return NO;
}

@end // implementation wxNSApplicationDelegate : NSObject

// ========================================================================
// wxNSApplicationObserver
// ========================================================================
@implementation wxNSApplicationObserver : NSObject

- (void)applicationWillBecomeActive:(NSNotification *)notification
{
    wxTheApp->CocoaDelegate_applicationWillBecomeActive();
}

- (void)applicationDidBecomeActive:(NSNotification *)notification
{
    wxTheApp->CocoaDelegate_applicationDidBecomeActive();
}

- (void)applicationWillResignActive:(NSNotification *)notification
{
    wxTheApp->CocoaDelegate_applicationWillResignActive();
}

- (void)applicationDidResignActive:(NSNotification *)notification
{
    wxTheApp->CocoaDelegate_applicationDidResignActive();
}

- (void)applicationWillUpdate:(NSNotification *)notification;
{
    wxTheApp->CocoaDelegate_applicationWillUpdate();
}

- (void)controlTintChanged:(NSNotification *)notification
{
    wxLogDebug(wxT("TODO: send EVT_SYS_COLOUR_CHANGED as appropriate"));
}

@end // implementation wxNSApplicationObserver : NSObject

// ========================================================================
// wxApp
// ========================================================================

// ----------------------------------------------------------------------------
// wxApp Static member initialization
// ----------------------------------------------------------------------------
IMPLEMENT_DYNAMIC_CLASS(wxApp, wxEvtHandler)
BEGIN_EVENT_TABLE(wxApp, wxEvtHandler)
    EVT_IDLE(wxAppBase::OnIdle)
//    EVT_END_SESSION(wxApp::OnEndSession)
//    EVT_QUERY_END_SESSION(wxApp::OnQueryEndSession)
END_EVENT_TABLE()

// ----------------------------------------------------------------------------
// wxApp initialization/cleanup
// ----------------------------------------------------------------------------
bool wxApp::Initialize(int& argc, wxChar **argv)
{
    wxAutoNSAutoreleasePool pool;
    m_cocoaMainThread = [NSThread currentThread];
    // Mac OS X passes a process serial number command line argument when
    // the application is launched from the Finder. This argument must be
    // removed from the command line arguments before being handled by the
    // application (otherwise applications would need to handle it)
    if ( argc > 1 )
    {
        static const wxChar *ARG_PSN = _T("-psn_");
        if ( wxStrncmp(argv[1], ARG_PSN, wxStrlen(ARG_PSN)) == 0 )
        {
            // remove this argument
            --argc;
            memmove(argv + 1, argv + 2, argc * sizeof(wxChar *));
        }
    }

    // Posing must be completed before any instances of the Objective-C
    // classes being posed as are created.
    wxPoseAsInitializer::InitializePosers();

    return wxAppBase::Initialize(argc, argv);
}

void wxApp::CleanUp()
{
    wxAutoNSAutoreleasePool pool;

    wxDC::CocoaShutdownTextSystem();
    wxMenuBarManager::DestroyInstance();

    [m_cocoaApp setDelegate:nil];
    [[NSNotificationCenter defaultCenter] removeObserver:m_cocoaAppDelegate];
    [m_cocoaAppDelegate release];
    m_cocoaAppDelegate = NULL;

    wxAppBase::CleanUp();
}

// ----------------------------------------------------------------------------
// wxApp creation
// ----------------------------------------------------------------------------
wxApp::wxApp()
{
    m_topWindow = NULL;

#ifdef __WXDEBUG__
    m_isInAssert = false;
#endif // __WXDEBUG__

    argc = 0;
    argv = NULL;
    m_cocoaApp = NULL;
    m_cocoaAppDelegate = NULL;
}

void wxApp::CocoaDelegate_applicationWillBecomeActive()
{
}

void wxApp::CocoaDelegate_applicationDidBecomeActive()
{
}

void wxApp::CocoaDelegate_applicationWillResignActive()
{
    wxTopLevelWindowCocoa::DeactivatePendingWindow();
}

void wxApp::CocoaDelegate_applicationDidResignActive()
{
}

bool wxApp::OnInitGui()
{
    wxAutoNSAutoreleasePool pool;
    if(!wxAppBase::OnInitGui())
        return false;

    // Create the app using the sharedApplication method
    m_cocoaApp = [NSApplication sharedApplication];

    // Enable response to application delegate messages
    m_cocoaAppDelegate = [[wxNSApplicationDelegate alloc] init];
    [m_cocoaApp setDelegate:m_cocoaAppDelegate];

    // Enable response to "delegate" messages on the notification observer
    [[NSNotificationCenter defaultCenter] addObserver:sg_cocoaAppObserver
        selector:@selector(applicationWillBecomeActive:)
        name:NSApplicationWillBecomeActiveNotification object:nil];

    [[NSNotificationCenter defaultCenter] addObserver:sg_cocoaAppObserver
        selector:@selector(applicationDidBecomeActive:)
        name:NSApplicationDidBecomeActiveNotification object:nil];

    [[NSNotificationCenter defaultCenter] addObserver:sg_cocoaAppObserver
        selector:@selector(applicationWillResignActive:)
        name:NSApplicationWillResignActiveNotification object:nil];

    [[NSNotificationCenter defaultCenter] addObserver:sg_cocoaAppObserver
        selector:@selector(applicationDidResignActive:)
        name:NSApplicationDidResignActiveNotification object:nil];

    [[NSNotificationCenter defaultCenter] addObserver:sg_cocoaAppObserver
        selector:@selector(applicationWillUpdate:)
        name:NSApplicationWillUpdateNotification object:nil];

    // Enable response to system notifications
    [[NSNotificationCenter defaultCenter] addObserver:sg_cocoaAppObserver
        selector:@selector(controlTintChanged:)
        name:NSControlTintDidChangeNotification object:nil];

    wxMenuBarManager::CreateInstance();

    wxDC::CocoaInitializeTextSystem();
    return true;
}

wxApp::~wxApp()
{
    if(m_cfRunLoopIdleObserver != NULL)
    {
        // Invalidate the observer which also removes it from the run loop.
        CFRunLoopObserverInvalidate(m_cfRunLoopIdleObserver);
        // Release the ref as we don't need it anymore.
        m_cfRunLoopIdleObserver.reset();
    }
}

bool wxApp::CallOnInit()
{
//    wxAutoNSAutoreleasePool pool;
    return OnInit();
}

bool wxApp::OnInit()
{
    if(!wxAppBase::OnInit())
        return false;

    return true;
}

void wxApp::Exit()
{
    wxApp::CleanUp();

    wxAppConsole::Exit();
}

// Yield to other processes
bool wxApp::Yield(bool onlyIfNeeded)
{
    // MT-FIXME
    static bool s_inYield = false;

#if wxUSE_LOG
    // disable log flushing from here because a call to wxYield() shouldn't
    // normally result in message boxes popping up &c
    wxLog::Suspend();
#endif // wxUSE_LOG

    if (s_inYield)
    {
        if ( !onlyIfNeeded )
        {
            wxFAIL_MSG( wxT("wxYield called recursively" ) );
        }

        return false;
    }

    s_inYield = true;

    // Run the event loop until it is out of events
    while(1)
    {
        wxAutoNSAutoreleasePool pool;
        NSEvent *event = [GetNSApplication()
                nextEventMatchingMask:NSAnyEventMask
                untilDate:[NSDate distantPast]
                inMode:NSDefaultRunLoopMode
                dequeue: YES];
        if(!event)
            break;
        [GetNSApplication() sendEvent: event];
    }

#if wxUSE_LOG
    // let the logs be flashed again
    wxLog::Resume();
#endif // wxUSE_LOG

    s_inYield = false;

    return true;
}

void wxApp::WakeUpIdle()
{
    [m_cocoaApp postEvent:[NSEvent otherEventWithType:NSApplicationDefined
            location:NSZeroPoint modifierFlags:NSAnyEventMask
            timestamp:0 windowNumber:0 context:nil
            subtype:0 data1:0 data2:0] atStart:NO];
}

extern "C" static void ObserveMainRunLoopBeforeWaiting(CFRunLoopObserverRef observer, CFRunLoopActivity activity, void *info);
extern "C" static void ObserveMainRunLoopBeforeWaiting(CFRunLoopObserverRef observer, CFRunLoopActivity activity, void *info)
{
    static_cast<wxApp*>(info)->CF_ObserveMainRunLoopBeforeWaiting(observer, activity);
}

#if 0
static int sg_cApplicationWillUpdate = 0;
#endif

void wxApp::CocoaDelegate_applicationWillUpdate()
{
    wxLogTrace(wxTRACE_COCOA,wxT("applicationWillUpdate"));

//    CFRunLoopRef cfRunLoop = [[NSRunLoop currentRunLoop] getCFRunLoop];
    CFRunLoopRef cfRunLoop = CFRunLoopGetCurrent();
    wxCFRef<CFStringRef> cfRunLoopMode(CFRunLoopCopyCurrentMode(cfRunLoop));

    if(m_cfRunLoopIdleObserver != NULL && m_cfObservedRunLoopMode != cfRunLoopMode)
    {
        CFRunLoopObserverInvalidate(m_cfRunLoopIdleObserver);
        m_cfRunLoopIdleObserver.reset();
    }
#if 0
    ++sg_cApplicationWillUpdate;
#endif
    if(m_cfRunLoopIdleObserver == NULL)
    {
        // Enable idle event handling
        CFRunLoopObserverContext observerContext =
        {   0
        ,   this
        ,   NULL
        ,   NULL
        ,   NULL
        };
        m_cfRunLoopIdleObserver.reset(CFRunLoopObserverCreate(kCFAllocatorDefault, kCFRunLoopBeforeWaiting, /*repeats*/FALSE, /*priority*/0, ObserveMainRunLoopBeforeWaiting, &observerContext));
        m_cfObservedRunLoopMode = cfRunLoopMode;
        CFRunLoopAddObserver(cfRunLoop, m_cfRunLoopIdleObserver, m_cfObservedRunLoopMode);
    }
}

static inline bool FakeNeedMoreIdle()
{
#if 0
// Return true on every 10th call.
    static int idleCount = 0;
    return ++idleCount % 10;
#else
    return false;
#endif
}

void wxApp::CF_ObserveMainRunLoopBeforeWaiting(CFRunLoopObserverRef observer, int activity)
{
    // Ensure that the app knows we've been invalidated
    m_cfRunLoopIdleObserver.reset();
#if 0
    wxLogTrace(wxTRACE_COCOA,wxT("Idle BEGIN (%d)"), sg_cApplicationWillUpdate);
    sg_cApplicationWillUpdate = 0;
#else
    wxLogTrace(wxTRACE_COCOA,wxT("Idle BEGIN"));
#endif
    if( ProcessIdle() || FakeNeedMoreIdle() )
    {
        wxLogTrace(wxTRACE_COCOA, wxT("Idle REQUEST MORE"));
        [NSApp setWindowsNeedUpdate:YES];
    }
    else
    {
        wxLogTrace(wxTRACE_COCOA, wxT("Idle END"));
    }
}

#ifdef __WXDEBUG__
void wxApp::OnAssert(const wxChar *file, int line, const wxChar* cond, const wxChar *msg)
{
    m_isInAssert = true;
    wxAppBase::OnAssert(file, line, cond, msg);
    m_isInAssert = false;
}
#endif // __WXDEBUG__
