/////////////////////////////////////////////////////////////////////////////
// Name:        app.cpp
// Purpose:     wxApp
// Author:      Julian Smart
// Modified by:
// Created:     17/09/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "app.h"
#endif

#include "wx/frame.h"
#include "wx/app.h"
#include "wx/utils.h"
#include "wx/gdicmn.h"
#include "wx/pen.h"
#include "wx/brush.h"
#include "wx/cursor.h"
#include "wx/icon.h"
#include "wx/palette.h"
#include "wx/dc.h"
#include "wx/dialog.h"
#include "wx/msgdlg.h"
#include "wx/log.h"
#include "wx/module.h"
#include "wx/memory.h"

#if wxUSE_THREADS
#include "wx/thread.h"
#endif

#if wxUSE_WX_RESOURCES
#include "wx/resource.h"
#endif

#include <Xm/Xm.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xresource.h>
#include <X11/Xatom.h>

#include "wx/motif/private.h"

#include <string.h>

extern char *wxBuffer;
extern wxList wxPendingDelete;

wxApp *wxTheApp = NULL;

wxHashTable *wxWidgetHashTable = NULL;

#if !USE_SHARED_LIBRARY
IMPLEMENT_DYNAMIC_CLASS(wxApp, wxEvtHandler)
BEGIN_EVENT_TABLE(wxApp, wxEvtHandler)
EVT_IDLE(wxApp::OnIdle)
END_EVENT_TABLE()
#endif

long wxApp::sm_lastMessageTime = 0;

bool wxApp::Initialize()
{
#ifdef __WXMSW__
    wxBuffer = new char[1500];
#else
    wxBuffer = new char[BUFSIZ + 512];
#endif

    wxClassInfo::InitializeClasses();

    wxTheColourDatabase = new wxColourDatabase(wxKEY_STRING);
    wxTheColourDatabase->Initialize();

    wxInitializeStockLists();
    wxInitializeStockObjects();

#if wxUSE_WX_RESOURCES
    wxInitializeResourceSystem();
#endif

    // For PostScript printing
#if wxUSE_POSTSCRIPT
    /* Done using wxModule now
    wxInitializePrintSetupData();
    wxThePrintPaperDatabase = new wxPrintPaperDatabase;
    wxThePrintPaperDatabase->CreateDatabase();
    */
#endif

    wxBitmap::InitStandardHandlers();

    wxWidgetHashTable = new wxHashTable(wxKEY_INTEGER);

    wxModule::RegisterModules();
    if (!wxModule::InitializeModules()) return FALSE;

    return TRUE;
}

void wxApp::CleanUp()
{
    delete wxWidgetHashTable;
    wxWidgetHashTable = NULL;

    wxModule::CleanUpModules();

#if wxUSE_WX_RESOURCES
    wxCleanUpResourceSystem();
#endif

    wxDeleteStockObjects() ;

    // Destroy all GDI lists, etc.

    delete wxTheBrushList;
    wxTheBrushList = NULL;

    delete wxThePenList;
    wxThePenList = NULL;

    delete wxTheFontList;
    wxTheFontList = NULL;

    delete wxTheBitmapList;
    wxTheBitmapList = NULL;

    delete wxTheColourDatabase;
    wxTheColourDatabase = NULL;

#if wxUSE_POSTSCRIPT
    /* Done using wxModule now
    wxInitializePrintSetupData(FALSE);
    delete wxThePrintPaperDatabase;
    wxThePrintPaperDatabase = NULL;
    */
#endif

    wxBitmap::CleanUpHandlers();

    delete[] wxBuffer;
    wxBuffer = NULL;

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
        wxLogDebug("There were memory leaks.\n");
        wxDebugContext::Dump();
        wxDebugContext::PrintStatistics();
    }
#endif

    // do it as the very last thing because everything else can log messages
    wxLog::DontCreateOnDemand();
    // do it as the very last thing because everything else can log messages
    delete wxLog::SetActiveTarget(NULL);
}

int wxEntry( int argc, char *argv[] )
{
#if (defined(__WXDEBUG__) && wxUSE_MEMORY_TRACING) || wxUSE_DEBUG_CONTEXT
    // This seems to be necessary since there are 'rogue'
    // objects present at this point (perhaps global objects?)
    // Setting a checkpoint will ignore them as far as the
    // memory checking facility is concerned.
    // Of course you may argue that memory allocated in globals should be
    // checked, but this is a reasonable compromise.
    wxDebugContext::SetCheckpoint();
#endif

    if (!wxApp::Initialize())
        return FALSE;

    if (!wxTheApp)
    {
        if (!wxApp::GetInitializerFunction())
        {
            printf( "wxWindows error: No initializer - use IMPLEMENT_APP macro.\n" );
            return 0;
        };

        wxTheApp = (wxApp*) (* wxApp::GetInitializerFunction()) ();
    };

    if (!wxTheApp)
    {
        printf( "wxWindows error: wxTheApp == NULL\n" );
        return 0;
    };

    wxTheApp->SetClassName(wxFileNameFromPath(argv[0]));
    wxTheApp->SetAppName(wxFileNameFromPath(argv[0]));

    wxTheApp->argc = argc;
    wxTheApp->argv = argv;

    // GUI-specific initialization, such as creating an app context.
    wxTheApp->OnInitGui();

    // Here frames insert themselves automatically
    // into wxTopLevelWindows by getting created
    // in OnInit().

    int retValue = 0;
    if (wxTheApp->OnInit())
    {
        if (wxTheApp->Initialized()) retValue = wxTheApp->OnRun();
    }

    // flush the logged messages if any
    wxLog *pLog = wxLog::GetActiveTarget();
    if ( pLog != NULL && pLog->HasPendingMessages() )
        pLog->Flush();

    delete wxLog::SetActiveTarget(new wxLogStderr); // So dialog boxes aren't used
    // for further messages

    if (wxTheApp->GetTopWindow())
    {
        delete wxTheApp->GetTopWindow();
        wxTheApp->SetTopWindow(NULL);
    }

    wxTheApp->DeletePendingObjects();

    wxTheApp->OnExit();

    wxApp::CleanUp();

    return retValue;
};

// Static member initialization
wxAppInitializerFunction wxApp::m_appInitFn = (wxAppInitializerFunction) NULL;

wxApp::wxApp()
{
    m_topWindow = NULL;
    wxTheApp = this;
    m_className = "";
    m_wantDebugOutput = TRUE ;
    m_appName = "";
    argc = 0;
    argv = NULL;
    m_printMode = wxPRINT_POSTSCRIPT;
    m_exitOnFrameDelete = TRUE;
    m_auto3D = TRUE;

    m_mainColormap = (WXColormap) NULL;
    m_appContext = (WXAppContext) NULL;
    m_topLevelWidget = (WXWidget) NULL;
    m_maxRequestSize = 0;
    m_initialDisplay = (WXDisplay*) 0;
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
    m_keepGoing = TRUE;

    /*
    * Sit around forever waiting to process X-events. Property Change
    * event are handled special, because they have to refer to
    * the root window rather than to a widget. therefore we can't
    * use an Xt-eventhandler.
    */

    XSelectInput(XtDisplay((Widget) wxTheApp->GetTopLevelWidget()),
        XDefaultRootWindow(XtDisplay((Widget) wxTheApp->GetTopLevelWidget())),
        PropertyChangeMask);

    XEvent event;

    // Use this flag to allow breaking the loop via wxApp::ExitMainLoop()
    while (m_keepGoing)
    {
        XtAppNextEvent( (XtAppContext) wxTheApp->GetAppContext(), &event);

        ProcessXEvent((WXEvent*) & event);

        if (XtAppPending( (XtAppContext) wxTheApp->GetAppContext() ) == 0)
        {
            if (!ProcessIdle())
            {
                // leave the main loop to give other threads a chance to
                // perform their GUI work
                wxMutexGuiLeave();
                wxUsleep(20);
                wxMutexGuiEnter();
            }
        }

    }

    return 0;
}

// Processes an X event.
void wxApp::ProcessXEvent(WXEvent* _event)
{
    XEvent* event = (XEvent*) _event;

    if ((event->type == KeyPress) && CheckForAccelerator(_event))
    {
        // Do nothing! We intercepted and processed the event as an accelerator.
        return;
    }
    else if (event->type == PropertyNotify)
    {
        HandlePropertyChange(_event);
        return;
    }
    else if (event->type == ResizeRequest)
    {
    /* Terry Gitnick <terryg@scientech.com> - 1/21/98
    * If resize event, don't resize until the last resize event for this
    * window is recieved. Prevents flicker as windows are resized.
        */

        Display *disp = XtDisplay((Widget) wxTheApp->GetTopLevelWidget());
        Window win = event->xany.window;
        XEvent report;

        //  to avoid flicker
        report = * event;
        while( XCheckTypedWindowEvent (disp, win, ResizeRequest, &report));

        // TODO: when implementing refresh optimization, we can use
        // XtAddExposureToRegion to expand the window's paint region.

        XtDispatchEvent(event);
    }
    else
    {
        XtDispatchEvent(event);
    }
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
    m_keepGoing = FALSE;
}

// Is a message/event pending?
bool wxApp::Pending()
{
    XFlush(XtDisplay( (Widget) wxTheApp->GetTopLevelWidget() ));

    // Fix by Doug from STI, to prevent a stall if non-X event
    // is found.
    return ((XtAppPending( (XtAppContext) GetAppContext() ) & XtIMXEvent) != 0) ;
}

// Dispatch a message.
void wxApp::Dispatch()
{
    //    XtAppProcessEvent( (XtAppContext) wxTheApp->GetAppContext(), XtIMAll);

    XEvent event;
    XtAppNextEvent((XtAppContext) GetAppContext(), &event);
    ProcessXEvent((WXEvent*) & event);
}

// This should be redefined in a derived class for
// handling property change events for XAtom IPC.
void wxApp::HandlePropertyChange(WXEvent *event)
{
    // by default do nothing special
    XtDispatchEvent((XEvent*) event); /* let Motif do the work */
}

void wxApp::OnIdle(wxIdleEvent& event)
{
    static bool inOnIdle = FALSE;

    // Avoid recursion (via ProcessEvent default case)
    if (inOnIdle)
        return;

    inOnIdle = TRUE;

    // 'Garbage' collection of windows deleted with Close().
    DeletePendingObjects();

    // flush the logged messages if any
    wxLog *pLog = wxLog::GetActiveTarget();
    if ( pLog != NULL && pLog->HasPendingMessages() )
        pLog->Flush();

    // Send OnIdle events to all windows
    bool needMore = SendIdleEvents();

    if (needMore)
        event.RequestMore(TRUE);

    inOnIdle = FALSE;
}

// Send idle event to all top-level windows
bool wxApp::SendIdleEvents()
{
    bool needMore = FALSE;
    wxNode* node = wxTopLevelWindows.First();
    while (node)
    {
        wxWindow* win = (wxWindow*) node->Data();
        if (SendIdleEvents(win))
            needMore = TRUE;

        node = node->Next();
    }
    return needMore;
}

// Send idle event to window and all subwindows
bool wxApp::SendIdleEvents(wxWindow* win)
{
    bool needMore = FALSE;

    wxIdleEvent event;
    event.SetEventObject(win);
    win->ProcessEvent(event);

    if (event.MoreRequested())
        needMore = TRUE;

    wxNode* node = win->GetChildren().First();
    while (node)
    {
        wxWindow* win = (wxWindow*) node->Data();
        if (SendIdleEvents(win))
            needMore = TRUE;

        node = node->Next();
    }
    return needMore ;
}

void wxApp::DeletePendingObjects()
{
    wxNode *node = wxPendingDelete.First();
    while (node)
    {
        wxObject *obj = (wxObject *)node->Data();

        delete obj;

        if (wxPendingDelete.Member(obj))
            delete node;

        // Deleting one object may have deleted other pending
        // objects, so start from beginning of list again.
        node = wxPendingDelete.First();
    }
}

wxLog* wxApp::CreateLogTarget()
{
    return new wxLogGui;
}

wxWindow* wxApp::GetTopWindow() const
{
    if (m_topWindow)
        return m_topWindow;
    else if (wxTopLevelWindows.Number() > 0)
        return (wxWindow*) wxTopLevelWindows.First()->Data();
    else
        return NULL;
}

// Create an application context
bool wxApp::OnInitGui()
{
    XtToolkitInitialize() ;
    wxTheApp->m_appContext = (WXAppContext) XtCreateApplicationContext() ;
    Display *dpy = XtOpenDisplay((XtAppContext) wxTheApp->m_appContext,(String)NULL,NULL,
        (const char*) wxTheApp->GetClassName(), NULL,
# if XtSpecificationRelease < 5
        0,(Cardinal*) &argc,argv) ;
# else
    0,&argc,argv) ;
# endif
    if (!dpy) {
        cerr << "wxWindows could not open display for " << wxTheApp->GetClassName() << ": exiting.\n";
        exit(-1);
    }
    m_initialDisplay = (WXDisplay*) dpy;

    wxTheApp->m_topLevelWidget = (WXWidget) XtAppCreateShell((String)NULL, (const char*) wxTheApp->GetClassName(),
        applicationShellWidgetClass,dpy,
        NULL,0) ;

    // Add general resize proc
    XtActionsRec rec;
    rec.string = "resize";
    rec.proc = (XtActionProc)wxWidgetResizeProc;
    XtAppAddActions((XtAppContext) wxTheApp->m_appContext, &rec, 1);

    GetMainColormap(dpy);
    m_maxRequestSize = XMaxRequestSize((Display*) dpy);

    return TRUE;
}

WXColormap wxApp::GetMainColormap(WXDisplay* display)
{
    if (!display) /* Must be called first with non-NULL display */
        return m_mainColormap;

    int defaultScreen = DefaultScreen((Display*) display);
    Screen* screen = XScreenOfDisplay((Display*) display, defaultScreen);

    Colormap c = DefaultColormapOfScreen(screen);

    if (!m_mainColormap)
        m_mainColormap = (WXColormap) c;

    return (WXColormap) c;
}

// Returns TRUE if an accelerator has been processed
bool wxApp::CheckForAccelerator(WXEvent* event)
{
    XEvent* xEvent = (XEvent*) event;
    if (xEvent->xany.type == KeyPress)
    {
        // Find a wxWindow for this window
        // TODO: should get display for the window, not the current display
        Widget widget = XtWindowToWidget((Display*) wxGetDisplay(), xEvent->xany.window);
        wxWindow* win = NULL;

        // Find the first wxWindow that corresponds to this event window
        while (widget && !(win = wxGetWindowFromTable(widget)))
            widget = XtParent(widget);

        if (!widget || !win)
            return FALSE;

        wxKeyEvent keyEvent(wxEVT_CHAR);
        wxTranslateKeyEvent(keyEvent, win, (Widget) 0, xEvent);

        // Now we have a wxKeyEvent and we have a wxWindow.
        // Go up the hierarchy until we find a matching accelerator,
        // or we get to the top.
        while (win)
        {
            if (win->ProcessAccelerator(keyEvent))
                return TRUE;
            win = win->GetParent();
        }
        return FALSE;
    }
    return FALSE;
}

void wxExit()
{
    int retValue = 0;
    if (wxTheApp)
        retValue = wxTheApp->OnExit();

    wxApp::CleanUp();
    /*
    * Exit in some platform-specific way. Not recommended that the app calls this:
    * only for emergencies.
    */
    exit(retValue);
}

// Yield to other processes
bool wxYield()
{
    while (wxTheApp && wxTheApp->Pending())
        wxTheApp->Dispatch();

    // VZ: is it the same as this (taken from old wxExecute)?
#if 0
    XtAppProcessEvent((XtAppContext) wxTheApp->GetAppContext(), XtIMAll);
#endif

    return TRUE;
}

