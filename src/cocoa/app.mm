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

wxApp *wxTheApp = NULL;
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

// ----------------------------------------------------------------------------
// other functions
// ----------------------------------------------------------------------------
void wxWakeUpIdle()
{
    wxTheApp->CocoaRequestIdle();
}

void wxExit()
{
    wxLogError(_("Fatal error: exiting"));

    wxApp::CleanUp();
    exit(1);
}

// ============================================================================
// wxApp implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxApp Static member initialization
// ----------------------------------------------------------------------------
wxAppInitializerFunction wxAppBase::m_appInitFn = (wxAppInitializerFunction) NULL;

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
/*static*/ bool wxApp::Initialize()
{
    wxPoseAsInitializer::InitializePosers();
    wxClassInfo::InitializeClasses();

#if wxUSE_THREADS
    wxPendingEventsLocker = new wxCriticalSection;
#endif

    wxTheColourDatabase = new wxColourDatabase(wxKEY_STRING);
    wxTheColourDatabase->Initialize();

    wxInitializeStockLists();
    wxInitializeStockObjects();

#if wxUSE_WX_RESOURCES
    wxInitializeResourceSystem();
#endif

    wxBitmap::InitStandardHandlers();

    wxModule::RegisterModules();
    if (!wxModule::InitializeModules()) {
        return FALSE;
    }
    return TRUE;
}

/*static*/ void wxApp::CleanUp()
{
    wxModule::CleanUpModules();

#if wxUSE_WX_RESOURCES
    wxCleanUpResourceSystem();
#endif

    wxDeleteStockObjects() ;

    // Destroy all GDI lists, etc.
    wxDeleteStockLists();

    delete wxTheColourDatabase;
    wxTheColourDatabase = NULL;

    wxBitmap::CleanUpHandlers();

    delete wxPendingEvents;

#if wxUSE_THREADS
    delete wxPendingEventsLocker;
    // If we don't do the following, we get an apparent memory leak.
    ((wxEvtHandler&) wxDefaultValidator).ClearEventLocker();
#endif

    wxClassInfo::CleanUpClasses();

    delete wxTheApp;
    wxTheApp = NULL;

#if (defined(__WXDEBUG__) && wxUSE_MEMORY_TRACING) || wxUSE_DEBUG_CONTEXT
    // At this point we want to check if there are any memory
    // blocks that aren't part of the wxDebugContext itself,
    // as a special case. Then when dumping we need to ignore
    // wxDebugContext, too.
    if (wxDebugContext::CountObjectsLeft(TRUE) > 0)
    {
        wxLogDebug(wxT("There were memory leaks."));
        wxDebugContext::Dump();
        wxDebugContext::PrintStatistics();
    }
    //  wxDebugContext::SetStream(NULL, NULL);
#endif

    wxDC::CocoaShutdownTextSystem();
#if wxUSE_LOG
    // do it as the very last thing because everything else can log messages
    delete wxLog::SetActiveTarget(NULL);
#endif // wxUSE_LOG
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

void wxApp::DeletePendingObjects()
{
    wxNode *node = wxPendingDelete.GetFirst();
    while (node)
    {
        wxObject *obj = (wxObject *)node->GetData();

        delete obj;

        if (wxPendingDelete.Find(obj))
            delete node;

        node = wxPendingDelete.GetFirst();
    }
}

// platform specifics

