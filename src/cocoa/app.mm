/////////////////////////////////////////////////////////////////////////////
// Name:        cocoa/app.mm
// Purpose:     wxApp
// Author:      David Elliott
// Modified by:
// Created:     2002/11/27
// RCS-ID:      $Id:
// Copyright:   (c) David Elliott
// Licence:     wxWindows license
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

#if wxUSE_WX_RESOURCES
#  include "wx/resource.h"
#endif

#import <AppKit/NSApplication.h>
#import <Foundation/NSRunLoop.h>
#import <Foundation/NSArray.h>

// ----------------------------------------------------------------------------
// globals
// ----------------------------------------------------------------------------

wxPoseAsInitializer *wxPoseAsInitializer::sm_first = NULL;

@interface wxPoserNSApplication : NSApplication
{
}

- (void)doIdle: (id)data;
- (void)finishLaunching;
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

- (void)finishLaunching
{
    wxLogDebug("finishLaunching");
    bool initsuccess = wxTheApp->OnInit();
    if(!initsuccess)
        [super stop: NULL];

    [super finishLaunching];
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

//----------------------------------------------------------------------
// wxEntry
//----------------------------------------------------------------------

int WXDLLEXPORT wxEntryStart( int WXUNUSED(argc), char *WXUNUSED(argv)[] )
{
    return wxApp::Initialize();
}

int WXDLLEXPORT wxEntryInitGui()
{
    return wxTheApp->OnInitGui();
}

void WXDLLEXPORT wxEntryCleanup()
{
    wxApp::CleanUp();
}

int wxEntry( int argc, char *argv[])
{
    if (!wxEntryStart(argc, argv)) {
        return 0;
    }
    wxLogDebug("Creating application");
   // create the application object or ensure that one already exists
    if (!wxTheApp)
    {
        // The app may have declared a global application object, but we recommend
        // the IMPLEMENT_APP macro is used instead, which sets an initializer
        // function for delayed, dynamic app object construction.
        wxCHECK_MSG( wxApp::GetInitializerFunction(), 0,
                     wxT("No initializer - use IMPLEMENT_APP macro.") );

        wxTheApp = (wxApp*) (*wxApp::GetInitializerFunction()) ();
    }

    wxCHECK_MSG( wxTheApp, 0, wxT("You have to define an instance of wxApp!") );

    // Mac OS X passes a process serial number command line argument when
    // the application is launched from the Finder. This argument must be
    // removed from the command line arguments before being handled by the
    // application (otherwise applications would need to handle it)

    if (argc > 1) {
        char theArg[6] = "";
        strncpy(theArg, argv[1], 5);

        if (strcmp(theArg, "-psn_") == 0) {
            // assume the argument is always the only one and remove it
            --argc;
        }
    }

    wxTheApp->argc = argc;
    wxTheApp->argv = argv;

    wxLogDebug("initializing gui");
    // GUI-specific initialization, such as creating an app context.
    wxEntryInitGui();

    // Here frames insert themselves automatically
    // into wxTopLevelWindows by getting created
    // in OnInit().

    int retValue = 0;

    wxLogDebug("Time to run");
    retValue = wxTheApp->OnRun();

    wxWindow *topWindow = wxTheApp->GetTopWindow();
    if ( topWindow )
    {
        // Forcibly delete the window.
        if ( topWindow->IsKindOf(CLASSINFO(wxFrame)) ||
                topWindow->IsKindOf(CLASSINFO(wxDialog)) )
        {
            topWindow->Close(TRUE);
        }
        else
        {
            delete topWindow;
            wxTheApp->SetTopWindow(NULL);
        }
    }

    wxTheApp->OnExit();

    wxEntryCleanup();

    return retValue;
}

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
    EVT_IDLE(wxApp::OnIdle)
//    EVT_END_SESSION(wxApp::OnEndSession)
//    EVT_QUERY_END_SESSION(wxApp::OnQueryEndSession)
END_EVENT_TABLE()
#endif

// ----------------------------------------------------------------------------
// wxApp static functions
// ----------------------------------------------------------------------------

bool wxApp::Initialize(int argc, wxChar **argv)
{
    // VZ: apparently this needs to be done a.s.a.p., right? it is done after
    //     wxClassInfo::InitializeClasses() now but usd to be done before, I
    //     hope it's not a problem -- if it is, please let me know, David (if
    //     it isn't, just remove this comment :-)
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
    wxLogDebug("wxApp::CocoaInstallIdleHandler");
    m_isIdle = false;
    // Call doIdle for EVERYTHING dammit
// We'd need Foundation/NSConnection.h for this next constant, do we need it?
    [[ NSRunLoop currentRunLoop ] performSelector:@selector(doIdle:) target:m_cocoaApp argument:NULL order:0 modes:[NSArray arrayWithObjects:NSDefaultRunLoopMode, /* NSConnectionReplyRunLoopMode,*/ NSModalPanelRunLoopMode, /**/NSEventTrackingRunLoopMode,/**/ nil] ];
}

bool wxApp::OnInitGui()
{
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

// Returns TRUE if more time is needed.
bool wxApp::ProcessIdle()
{
    wxIdleEvent event;
    event.SetEventObject(this);
    ProcessEvent(event);

    return event.MoreRequested();
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
    [m_cocoaApp terminate: m_cocoaApp];
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

void wxApp::OnIdle(wxIdleEvent& event)
{
    wxLogDebug("wxApp::OnIdle");
   static bool s_inOnIdle = FALSE;

    // Avoid recursion (via ProcessEvent default case)
    if ( s_inOnIdle )
        return;
    s_inOnIdle = TRUE;


    DeletePendingObjects();

    // flush the logged messages if any
    wxLog *pLog = wxLog::GetActiveTarget();
    if ( pLog != NULL && pLog->HasPendingMessages() )
        pLog->Flush();

    // Send OnIdle events to all windows
    bool needMore = SendIdleEvents();

    if (needMore)
      event.RequestMore(TRUE);

    s_inOnIdle = FALSE;
}

// Send idle event to all top-level windows
bool wxApp::SendIdleEvents()
{
    bool needMore = FALSE;
    wxWindowList::Node* node = wxTopLevelWindows.GetFirst();
    while (node)
    {
        wxWindow* win = node->GetData();
        if (SendIdleEvents(win))
            needMore = TRUE;

        node = node->GetNext();
    }
    return needMore;
}

// Send idle event to window and all subwindows
bool wxApp::SendIdleEvents(wxWindow* win)
{
//    wxLogDebug("SendIdleEvents win=%p",win);
    bool needMore = FALSE;

    wxIdleEvent event;
    event.SetEventObject(win);
    win->ProcessEvent(event);

    if (event.MoreRequested())
        needMore = TRUE;

    wxWindowList::Node* node = win->GetChildren().GetFirst();
    while (node)
    {
//        wxLogDebug("child=%p",node->Data());
        wxWindow* win = node->GetData();
        if (SendIdleEvents(win))
            needMore = TRUE;

        node = node->GetNext();
    }
    return needMore;
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

