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
#include "wx/dialog.h"
#include "wx/msgdlg.h"
#include "wx/log.h"
#include "wx/module.h"
#include "wx/memory.h"
#include "wx/log.h"
#include "wx/intl.h"
#include "wx/evtloop.h"

#if wxUSE_THREADS
    #include "wx/thread.h"
#endif

#if wxUSE_WX_RESOURCES
    #include "wx/resource.h"
#endif

#ifdef __VMS__
#pragma message disable nosimpint
#endif
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xresource.h>
#include <X11/Xatom.h>
#ifdef __VMS__
#pragma message enable nosimpint
#endif

#include "wx/x11/private.h"

#include <string.h>

extern char *wxBuffer;
extern wxList wxPendingDelete;

wxApp *wxTheApp = NULL;

wxHashTable *wxWidgetHashTable = NULL;

IMPLEMENT_DYNAMIC_CLASS(wxApp, wxEvtHandler)

BEGIN_EVENT_TABLE(wxApp, wxEvtHandler)
    EVT_IDLE(wxApp::OnIdle)
END_EVENT_TABLE()

#ifdef __WXDEBUG__
typedef int (*XErrorHandlerFunc)(Display *, XErrorEvent *);

XErrorHandlerFunc gs_pfnXErrorHandler = 0;

static int wxXErrorHandler(Display *dpy, XErrorEvent *xevent)
{
    // just forward to the default handler for now
    return gs_pfnXErrorHandler(dpy, xevent);
}
#endif // __WXDEBUG__

long wxApp::sm_lastMessageTime = 0;

bool wxApp::Initialize()
{
    wxBuffer = new char[BUFSIZ + 512];

    wxClassInfo::InitializeClasses();

    // GL: I'm annoyed ... I don't know where to put this and I don't want to
    // create a module for that as it's part of the core.
#if wxUSE_THREADS
    wxPendingEventsLocker = new wxCriticalSection();
#endif

    wxTheColourDatabase = new wxColourDatabase(wxKEY_STRING);
    wxTheColourDatabase->Initialize();

    wxInitializeStockLists();
    wxInitializeStockObjects();

#if wxUSE_WX_RESOURCES
    wxInitializeResourceSystem();
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

    wxDeleteStockLists();

    delete wxTheColourDatabase;
    wxTheColourDatabase = NULL;

    wxBitmap::CleanUpHandlers();

    delete[] wxBuffer;
    wxBuffer = NULL;

    wxClassInfo::CleanUpClasses();

    delete wxTheApp;
    wxTheApp = NULL;

    // GL: I'm annoyed ... I don't know where to put this and I don't want to
    // create a module for that as it's part of the core.
#if wxUSE_THREADS
    delete wxPendingEvents;
    delete wxPendingEventsLocker;
#endif

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

    // Here frames insert themselves automatically into wxTopLevelWindows by
    // getting created in OnInit().

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
wxAppInitializerFunction wxAppBase::m_appInitFn = (wxAppInitializerFunction) NULL;

wxApp::wxApp()
{
    m_topWindow = NULL;
    wxTheApp = this;
    m_className = "";
    m_wantDebugOutput = TRUE ;
    m_appName = "";
    argc = 0;
    argv = NULL;
    m_exitOnFrameDelete = TRUE;
    m_mainColormap = (WXColormap) NULL;
    m_topLevelWidget = (WXWindow) NULL;
    m_maxRequestSize = 0;
    m_initialDisplay = (WXDisplay*) 0;
    m_mainLoop = NULL;
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
     int rt;
    m_mainLoop = new wxEventLoop;

    rt = m_mainLoop->Run();

    delete m_mainLoop;
    m_mainLoop = NULL;
    return rt;
}

// Processes an X event.
void wxApp::ProcessXEvent(WXEvent* _event)
{
    XEvent* event = (XEvent*) _event;

    wxWindow* win = NULL;
    Window window = event->xany.window;
    Window actualWindow = window;

    // Find the first wxWindow that corresponds to this event window
    // TODO: may need to translate coordinates from actualWindow
    // to window, if the receiving window != wxWindow window
    while (window && !(win = wxGetWindowFromTable(window)))
        window = wxGetWindowParent(window);

    // TODO: shouldn't all the ProcessEvents below
    // be win->GetEventHandler()->ProcessEvent?
    switch (event->type)
    {
        case KeyPress:
        {
            if (win && !win->IsEnabled())
                return;

            {
                if (win)
                {
                    wxKeyEvent keyEvent(wxEVT_KEY_DOWN);
                    wxTranslateKeyEvent(keyEvent, win, window, event);
        
                    // We didn't process wxEVT_KEY_DOWN, so send
                    // wxEVT_CHAR
                    if (!win->GetEventHandler()->ProcessEvent( keyEvent ))
                    {
                        keyEvent.SetEventType(wxEVT_CHAR);
                        win->GetEventHandler()->ProcessEvent( keyEvent );
                    }

                    // We intercepted and processed the key down event
                    return;
                }
            }
            return;
        }
        case KeyRelease:
        {
            if (win && !win->IsEnabled())
                return;

            if (win)
            {
                wxKeyEvent keyEvent(wxEVT_KEY_UP);
                wxTranslateKeyEvent(keyEvent, win, window, event);
        
                win->GetEventHandler()->ProcessEvent( keyEvent );
            }
            return;
        }
        case PropertyNotify:
        {
            HandlePropertyChange(_event);
            return;
        }
        case ClientMessage:
        {           
            Atom wm_delete_window = XInternAtom(wxGlobalDisplay(), "WM_DELETE_WINDOW", True);;
            Atom wm_protocols = XInternAtom(wxGlobalDisplay(), "WM_PROTOCOLS", True);;

            if (event->xclient.message_type == wm_protocols)
            {
                if (event->xclient.data.l[0] == wm_delete_window)
                {
                    if (win)
                    {
                        win->Close(FALSE);
                    }
                }
            }
            return;
        }
        case ResizeRequest:
        {
            /* Terry Gitnick <terryg@scientech.com> - 1/21/98
            * If resize event, don't resize until the last resize event for this
            * window is recieved. Prevents flicker as windows are resized.
            */
        
            Display *disp = (Display*) wxGetDisplay();
            XEvent report;
            
            //  to avoid flicker
            report = * event;
            while( XCheckTypedWindowEvent (disp, actualWindow, ResizeRequest, &report));
            
            // TODO: when implementing refresh optimization, we can use
            // XtAddExposureToRegion to expand the window's paint region.

            if (win)
            {
                wxSize sz = win->GetSize();
                wxSizeEvent sizeEvent(sz, win->GetId());
                sizeEvent.SetEventObject(win);

                win->GetEventHandler()->ProcessEvent( sizeEvent );
            }

            return;
        }
        case Expose:
        {
            if (win)
            {
                win->GetUpdateRegion().Union( event->xexpose.x, event->xexpose.y,
                                              event->xexpose.width, event->xexpose.height);
                if (event->xexpose.count == 0)
                {
                    win->X11SendPaintEvents();  // TODO let an idle handler do that
                }
            }

            return;
        }
        case EnterNotify:
        case LeaveNotify:
        case ButtonPress:
        case ButtonRelease:
        case MotionNotify:
        {
            if (win && !win->IsEnabled())
                return;

            if (win)
            {
                wxMouseEvent wxevent;
                wxTranslateMouseEvent(wxevent, win, window, event);
                win->GetEventHandler()->ProcessEvent( wxevent );
            }
            return;
        }
        case FocusIn:
            {
                if (win && event->xfocus.detail != NotifyPointer)
                {
                    wxFocusEvent focusEvent(wxEVT_SET_FOCUS, win->GetId());
                    focusEvent.SetEventObject(win);
                    win->GetEventHandler()->ProcessEvent(focusEvent);
                }
                break;
            }
        case FocusOut:
            {
                if (win && event->xfocus.detail != NotifyPointer)
                {
                    wxFocusEvent focusEvent(wxEVT_KILL_FOCUS, win->GetId());
                    focusEvent.SetEventObject(win);
                    win->GetEventHandler()->ProcessEvent(focusEvent);
                }
                break;
            }
        case DestroyNotify:
            {
                // Do we want to process this (for top-level windows)?
                // But we want to be able to veto closes, anyway
                break;
            }
        default:
        {
            break;
        }
    }
}

// Returns TRUE if more time is needed.
// Note that this duplicates wxEventLoopImpl::SendIdleEvent
// but ProcessIdle may be needed by apps, so is kept.
bool wxApp::ProcessIdle()
{
    wxIdleEvent event;
    event.SetEventObject(this);
    ProcessEvent(event);

    return event.MoreRequested();
}

void wxApp::ExitMainLoop()
{
    if (m_mainLoop)
        m_mainLoop->Exit(0);
}

// Is a message/event pending?
bool wxApp::Pending()
{
    return wxEventLoop::GetActive()->Pending();
}

// Dispatch a message.
void wxApp::Dispatch()
{
    wxEventLoop::GetActive()->Dispatch();
}

// This should be redefined in a derived class for
// handling property change events for XAtom IPC.
void wxApp::HandlePropertyChange(WXEvent *event)
{
    // by default do nothing special
    // TODO: what to do for X11
    // XtDispatchEvent((XEvent*) event); /* let Motif do the work */
}

void wxApp::OnIdle(wxIdleEvent& event)
{
    static bool inOnIdle = FALSE;

    // Avoid recursion (via ProcessEvent default case)
    if (inOnIdle)
        return;

    inOnIdle = TRUE;

    // If there are pending events, we must process them: pending events
    // are either events to the threads other than main or events posted
    // with wxPostEvent() functions
    // GRG: I have moved this here so that all pending events are processed
    //   before starting to delete any objects. This behaves better (in
    //   particular, wrt wxPostEvent) and is coherent with wxGTK's current
    //   behaviour. Also removed the '#if wxUSE_THREADS' around it.
    //  Changed Mar/2000 before 2.1.14

    // Flush pending events.
    ProcessPendingEvents();

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

void wxWakeUpIdle()
{
    // **** please implement me! ****
    // Wake up the idle handler processor, even if it is in another thread...
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

// Create an application context
bool wxApp::OnInitGui()
{
    // TODO: parse argv and get display to pass to XOpenDisplay
    Display* dpy = XOpenDisplay(NULL);
    m_initialDisplay = (WXDisplay*) dpy;

    if (!dpy) {
        wxString className(wxTheApp->GetClassName());
        wxLogError(_("wxWindows could not open display for '%s': exiting."),
            (const char*) className);
        exit(-1);
    }
    XSelectInput((Display*) m_initialDisplay,
        XDefaultRootWindow((Display*) m_initialDisplay),
        PropertyChangeMask);

#ifdef __WXDEBUG__
    // install the X error handler
    gs_pfnXErrorHandler = XSetErrorHandler(wxXErrorHandler);
#endif // __WXDEBUG__

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

Window wxGetWindowParent(Window window)
{
    Window parent, root = 0;
    unsigned int noChildren = 0;
    if (XQueryTree((Display*) wxGetDisplay(), window, & root, & parent,
        NULL, & noChildren))
        return parent;
    else
        return (Window) 0;
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

bool wxApp::Yield(bool onlyIfNeeded)
{
    bool s_inYield = FALSE;

    if ( s_inYield )
    {
        if ( !onlyIfNeeded )
        {
            wxFAIL_MSG( wxT("wxYield called recursively" ) );
        }

        return FALSE;
    }

    s_inYield = TRUE;

    while (wxTheApp && wxTheApp->Pending())
        wxTheApp->Dispatch();

    s_inYield = FALSE;

    return TRUE;
}

// TODO use XmGetPixmap (?) to get the really standard icons!

// XPM hack: make the arrays const
#define static static const

#include "wx/generic/info.xpm"
#include "wx/generic/error.xpm"
#include "wx/generic/question.xpm"
#include "wx/generic/warning.xpm"

#undef static

wxIcon
wxApp::GetStdIcon(int which) const
{
    switch(which)
    {
        case wxICON_INFORMATION:
            return wxIcon(info_xpm);

        case wxICON_QUESTION:
            return wxIcon(question_xpm);

        case wxICON_EXCLAMATION:
            return wxIcon(warning_xpm);

        default:
            wxFAIL_MSG("requested non existent standard icon");
            // still fall through

        case wxICON_HAND:
            return wxIcon(error_xpm);
    }
}

// ----------------------------------------------------------------------------
// accessors for C modules
// ----------------------------------------------------------------------------

#if 0
extern "C" XtAppContext wxGetAppContext()
{
    return (XtAppContext)wxTheApp->GetAppContext();
}
#endif
