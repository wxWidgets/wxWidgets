/////////////////////////////////////////////////////////////////////////////
// Name:        cocoa/app.mm
// Purpose:     wxApp
// Author:      David Elliott
// Modified by:
// Created:     2002/11/27
// RCS-ID:      $Id$
// Copyright:   (c) David Elliott
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
    #include "wx/defs.h"
    #include "wx/app.h"
    #include "wx/frame.h"
    #include "wx/dialog.h"
    #include "wx/dc.h"
    #include "wx/intl.h"
    #include "wx/log.h"
#endif

#include "wx/module.h"

#include "wx/cocoa/ObjcPose.h"
#include "wx/cocoa/autorelease.h"

#if wxUSE_WX_RESOURCES
#  include "wx/resource.h"
#endif

#import <AppKit/NSApplication.h>
#import <Foundation/NSRunLoop.h>
#import <Foundation/NSArray.h>
#import <Foundation/NSAutoreleasePool.h>

// ----------------------------------------------------------------------------
// globals
// ----------------------------------------------------------------------------

wxPoseAsInitializer *wxPoseAsInitializer::sm_first = NULL;

@interface wxPoserNSApplication : NSApplication
{
}

- (void)doIdle: (id)data;
- (void)sendEvent: (NSEvent*)anEvent;
- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)theApplication;
@end // wxPoserNSApplication

@implementation wxPoserNSApplication : NSApplication

- (void)doIdle: (id)data
{
    wxASSERT(wxTheApp);
    wxLogDebug("doIdle called");
    NSRunLoop *rl = [NSRunLoop currentRunLoop];
    // runMode: beforeDate returns YES if something was done
    while(wxTheApp->ProcessIdle()) // FIXME: AND NO EVENTS ARE PENDING
    {
        wxLogDebug("Looping for idle events");
        #if 1
        if( [rl runMode:[rl currentMode] beforeDate:[NSDate distantPast]])
        {
            wxLogDebug("Found actual work to do");
            break;
        }
        #endif
    }
    wxLogDebug("Idle processing complete, requesting next idle event");
    // Add ourself back into the run loop (on next event) if necessary
    wxTheApp->CocoaRequestIdle();
}

- (void)sendEvent: (NSEvent*)anEvent
{
    wxLogDebug("SendEvent");
    wxTheApp->CocoaInstallRequestedIdleHandler();
    [super sendEvent: anEvent];
}

- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)theApplication
{
    BOOL ret = wxTheApp->GetExitOnFrameDelete();
    wxLogDebug("applicationShouldTermintaeAfterLastWindowClosed=%d",ret);
    return ret;
}

@end // wxPoserNSApplication
WX_IMPLEMENT_POSER(wxPoserNSApplication);

// ============================================================================
// functions
// ============================================================================

void wxApp::Exit()
{
    wxApp::CleanUp();

    wxAppConsole::Exit();
}

// ============================================================================
// wxApp implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxApp Static member initialization
// ----------------------------------------------------------------------------

#if !USE_SHARED_LIBRARY
IMPLEMENT_DYNAMIC_CLASS(wxApp, wxEvtHandler)
BEGIN_EVENT_TABLE(wxApp, wxEvtHandler)
    EVT_IDLE(wxAppBase::OnIdle)
//    EVT_END_SESSION(wxApp::OnEndSession)
//    EVT_QUERY_END_SESSION(wxApp::OnQueryEndSession)
END_EVENT_TABLE()
#endif

// ----------------------------------------------------------------------------
// wxApp initialization/cleanup
// ----------------------------------------------------------------------------

bool wxApp::Initialize(int& argc, wxChar **argv)
{
    wxAutoNSAutoreleasePool pool;
    // Mac OS X passes a process serial number command line argument when
    // the application is launched from the Finder. This argument must be
    // removed from the command line arguments before being handled by the
    // application (otherwise applications would need to handle it)
    if ( argc > 1 )
    {
        static const wxChar *ARG_PSN = _T("-psn_");
        if ( wxStrncmp(argv[1], ARG_PSN, sizeof(ARG_PSN) - 1) == 0 )
        {
            // remove this argument
            memmove(argv, argv + 1, argc--);
        }
    }

    // Posing must be completed before any instances of the Objective-C
    // classes being posed as are created.
    wxPoseAsInitializer::InitializePosers();

    return wxAppBase::Initialize(argc, argv);
}

void wxApp::CleanUp()
{
    wxDC::CocoaShutdownTextSystem();

    wxAppBase::CleanUp();
}

// ----------------------------------------------------------------------------
// wxApp creation
// ----------------------------------------------------------------------------

wxApp::wxApp()
{
    m_topWindow = NULL;
    wxTheApp = this;

    m_isIdle = true;
#if WXWIN_COMPATIBILITY_2_2
    m_wantDebugOutput = TRUE;
#endif

    argc = 0;
    argv = NULL;
    m_cocoaApp = NULL;
}

void wxApp::CocoaInstallIdleHandler()
{
    // If we're supposed to be stopping, don't add more idle events
    if(![m_cocoaApp isRunning])
        return;
    wxLogDebug("wxApp::CocoaInstallIdleHandler");
    m_isIdle = false;
    // Call doIdle for EVERYTHING dammit
// We'd need Foundation/NSConnection.h for this next constant, do we need it?
    [[ NSRunLoop currentRunLoop ] performSelector:@selector(doIdle:) target:m_cocoaApp argument:NULL order:0 modes:[NSArray arrayWithObjects:NSDefaultRunLoopMode, /* NSConnectionReplyRunLoopMode,*/ NSModalPanelRunLoopMode, /**/NSEventTrackingRunLoopMode,/**/ nil] ];
}

bool wxApp::OnInitGui()
{
    wxAutoNSAutoreleasePool pool;
    if(!wxAppBase::OnInitGui())
        return FALSE;

    // Create the app using the sharedApplication method
    m_cocoaApp = [NSApplication sharedApplication];
    wxDC::CocoaInitializeTextSystem();
//    [ m_cocoaApp setDelegate:m_cocoaApp ];
    #if 0
    wxLogDebug("Just for kicks");
    [ m_cocoaApp performSelector:@selector(doIdle:) withObject:NULL ];
    wxLogDebug("okay.. done now");
    #endif
    return TRUE;
}

bool wxApp::CallOnInit()
{
//    wxAutoNSAutoreleasePool pool;
    return OnInit();
}

bool wxApp::OnInit()
{
    if(!wxAppBase::OnInit())
        return FALSE;

    return TRUE;
}

bool wxApp::Initialized()
{
  if (GetTopWindow())
    return TRUE;
  else
    return FALSE;
}

int wxApp::MainLoop()
{
    [m_cocoaApp run];
    return 0;
}

void wxApp::ExitMainLoop()
{
    wxLogDebug("wxApp::ExitMailLoop m_isIdle=%d, isRunning=%d",(int)m_isIdle,(int)[m_cocoaApp isRunning]);
//    CocoaInstallRequestedIdleHandler();
//    if(m_isIdle)
//        [[ NSRunLoop currentRunLoop ] performSelector:@selector(doIdle:) target:m_cocoaApp argument:NULL order:0 modes:[NSArray arrayWithObjects:NSDefaultRunLoopMode, /* NSConnectionReplyRunLoopMode, NSModalPanelRunLoopMode, NSEventTrackingRunLoopMode,*/ nil] ];
// actually.. we WANT the idle event
// or not
#if 0
    if(!m_isIdle)
        [[ NSRunLoop currentRunLoop ] cancelPerformSelector:@selector(doIdle:) target:m_cocoaApp argument:NULL];
#endif
    [m_cocoaApp stop: m_cocoaApp];
}

// Is a message/event pending?
bool wxApp::Pending()
{
    return 0;
}

// Dispatch a message.
void wxApp::Dispatch()
{
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

    wxLogDebug("WARNING: SUPPOSED to have yielded!");
    // FIXME: Do something!

#if wxUSE_LOG
    // let the logs be flashed again
    wxLog::Resume();
#endif // wxUSE_LOG

    s_inYield = false;

    return true;
}

