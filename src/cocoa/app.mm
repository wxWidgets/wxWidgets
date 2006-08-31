/////////////////////////////////////////////////////////////////////////////
// Name:        src/cocoa/app.mm
// Purpose:     wxApp
// Author:      David Elliott
// Modified by:
// Created:     2002/11/27
// RCS-ID:      $Id$
// Copyright:   (c) David Elliott
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

#include "wx/cocoa/ObjcPose.h"
#include "wx/cocoa/autorelease.h"
#include "wx/cocoa/mbarman.h"
#include "wx/cocoa/NSApplication.h"

#if wxUSE_WX_RESOURCES
#  include "wx/resource.h"
#endif

#import <AppKit/NSApplication.h>
#import <Foundation/NSRunLoop.h>
#import <Foundation/NSThread.h>
#import <AppKit/NSEvent.h>
#import <Foundation/NSString.h>
#import <Foundation/NSNotification.h>
#import <AppKit/NSCell.h>

// ========================================================================
// wxPoseAsInitializer
// ========================================================================
wxPoseAsInitializer *wxPoseAsInitializer::sm_first = NULL;

static bool sg_needIdle = true;

// ========================================================================
// wxPoserNSApplication
// ========================================================================
@interface wxPoserNSApplication : NSApplication
{
}

- (NSEvent *)nextEventMatchingMask:(unsigned int)mask untilDate:(NSDate *)expiration inMode:(NSString *)mode dequeue:(BOOL)flag;
- (void)sendEvent: (NSEvent*)anEvent;
@end // wxPoserNSApplication

WX_IMPLEMENT_POSER(wxPoserNSApplication);

@implementation wxPoserNSApplication : NSApplication

/* NOTE: The old method of idle event handling added the handler using the
    [NSRunLoop -performSelector:target:argument:order:modes] which caused
    the invocation to occur at the begining of [NSApplication
    -nextEventMatchingMask:untilDate:expiration:inMode:dequeue:].  However,
    the code would be scheduled for invocation with every iteration of
    the event loop.  This new method simply overrides the method.  The
    same caveats apply.  In particular, by the time the event loop has
    called this method, it usually expects to receive an event.  If you
    plan on stopping the event loop, it is wise to send an event through
    the queue to ensure this method will return.
    See wxEventLoop::Exit() for more information.

    This overridden method calls the superclass method with an untilDate
    parameter that indicates nil should be returned if there are no pending
    events.  That is, nextEventMatchingMask: should not wait for an event.
    If nil is returned then idle event processing occurs until the user
    does not request anymore idle events or until a real event comes through.

    RN: Even though Apple documentation states that nil can be passed in place
    of [NSDate distantPast] in the untilDate parameter, this causes Jaguar (10.2)
    to get stuck in some kind of loop deep within nextEventMatchingMask:, thus we
    need to explicitly pass [NSDate distantPast] instead.
*/

- (NSEvent *)nextEventMatchingMask:(unsigned int)mask untilDate:(NSDate *)expiration inMode:(NSString *)mode dequeue:(BOOL)flag
{
    // Get the same events except don't block
    NSEvent *event = [super nextEventMatchingMask:mask untilDate:[NSDate distantPast] inMode:mode dequeue:flag];
    // If we got one, simply return it
    if(event)
        return event;
    // No events, try doing some idle stuff
    if(sg_needIdle
#ifdef __WXDEBUG__
        && !wxTheApp->IsInAssert()
#endif
        && ([NSDefaultRunLoopMode isEqualToString:mode] || [NSModalPanelRunLoopMode isEqualToString:mode]))
    {
        sg_needIdle = false;
        wxLogTrace(wxTRACE_COCOA,wxT("Processing idle events"));
        while(wxTheApp->ProcessIdle())
        {
            // Get the same events except don't block
            NSEvent *event = [super nextEventMatchingMask:mask untilDate:[NSDate distantPast] inMode:mode dequeue:flag];
            // If we got one, simply return it
            if(event)
                return event;
            // we didn't get one, do some idle work
            wxLogTrace(wxTRACE_COCOA,wxT("Looping idle events"));
        }
        // No more idle work requested, block
        wxLogTrace(wxTRACE_COCOA,wxT("Finished idle processing"));
    }
    else
        wxLogTrace(wxTRACE_COCOA,wxT("Avoiding idle processing sg_needIdle=%d"),sg_needIdle);
    return [super nextEventMatchingMask:mask untilDate:expiration inMode:mode dequeue:flag];
}

- (void)sendEvent: (NSEvent*)anEvent
{
    wxLogTrace(wxTRACE_COCOA,wxT("SendEvent"));
    sg_needIdle = true;
    [super sendEvent: anEvent];
}

@end // wxPoserNSApplication

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

- (void)controlTintChanged:(NSNotification *)notification
{
    wxLogDebug(wxT("TODO: send EVT_SYS_COLOUR_CHANGED as appropriate"));
}

@end // implementation wxNSApplicationDelegate : NSObject

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
    [[NSNotificationCenter defaultCenter] removeObserver:m_cocoaAppDelegate
        name:NSControlTintDidChangeNotification object:nil];
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
    m_cocoaAppDelegate = [[wxNSApplicationDelegate alloc] init];
    [m_cocoaApp setDelegate:m_cocoaAppDelegate];
    [[NSNotificationCenter defaultCenter] addObserver:m_cocoaAppDelegate
        selector:@selector(controlTintChanged:)
        name:NSControlTintDidChangeNotification object:nil];

    wxMenuBarManager::CreateInstance();

    wxDC::CocoaInitializeTextSystem();
    return true;
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

#ifdef __WXDEBUG__
void wxApp::OnAssert(const wxChar *file, int line, const wxChar* cond, const wxChar *msg)
{
    m_isInAssert = true;
    wxAppBase::OnAssert(file, line, cond, msg);
    m_isInAssert = false;
}
#endif // __WXDEBUG__
