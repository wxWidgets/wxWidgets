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
#include "wx/icon.h"
#include "wx/dialog.h"
#include "wx/log.h"
#include "wx/module.h"
#include "wx/memory.h"
#include "wx/log.h"
#include "wx/intl.h"
#include "wx/evtloop.h"
#include "wx/timer.h"
#include "wx/filename.h"
#include "wx/hash.h"

#include "wx/univ/theme.h"
#include "wx/univ/renderer.h"

#if wxUSE_THREADS
    #include "wx/thread.h"
#endif

#include "wx/x11/private.h"

#include <string.h>

//------------------------------------------------------------------------
//   global data
//------------------------------------------------------------------------

extern wxList wxPendingDelete;

wxHashTable *wxWidgetHashTable = NULL;
wxHashTable *wxClientWidgetHashTable = NULL;

// This is set within wxEntryStart -- too early on
// to put these in wxTheApp
static bool g_showIconic = FALSE;
static wxSize g_initialSize = wxDefaultSize;

// This is required for wxFocusEvent::SetWindow(). It will only
// work for focus events which we provoke ourselves (by calling
// SetFocus()). It will not work for those events, which X11
// generates itself.
static wxWindow *g_nextFocus = NULL;
static wxWindow *g_prevFocus = NULL;

//------------------------------------------------------------------------
//   X11 error handling
//------------------------------------------------------------------------

#ifdef __WXDEBUG__
typedef int (*XErrorHandlerFunc)(Display *, XErrorEvent *);

XErrorHandlerFunc gs_pfnXErrorHandler = 0;

static int wxXErrorHandler(Display *dpy, XErrorEvent *xevent)
{
    // just forward to the default handler for now
    if (gs_pfnXErrorHandler)
        return gs_pfnXErrorHandler(dpy, xevent);
    else
        return 0;
}
#endif // __WXDEBUG__

//------------------------------------------------------------------------
//   wxApp
//------------------------------------------------------------------------

long wxApp::sm_lastMessageTime = 0;
WXDisplay *wxApp::ms_display = NULL;

IMPLEMENT_DYNAMIC_CLASS(wxApp, wxEvtHandler)

BEGIN_EVENT_TABLE(wxApp, wxEvtHandler)
    EVT_IDLE(wxApp::OnIdle)
END_EVENT_TABLE()

bool wxApp::Initialize()
{
    wxClassInfo::InitializeClasses();

#if wxUSE_INTL
    wxFont::SetDefaultEncoding(wxLocale::GetSystemEncoding());
#endif

    // GL: I'm annoyed ... I don't know where to put this and I don't want to
    // create a module for that as it's part of the core.
#if wxUSE_THREADS
    wxPendingEventsLocker = new wxCriticalSection();
#endif

    wxTheColourDatabase = new wxColourDatabase(wxKEY_STRING);
    wxTheColourDatabase->Initialize();

    wxInitializeStockLists();
    wxInitializeStockObjects();

    wxWidgetHashTable = new wxHashTable(wxKEY_INTEGER);
    wxClientWidgetHashTable = new wxHashTable(wxKEY_INTEGER);

    wxModule::RegisterModules();
    if (!wxModule::InitializeModules()) return FALSE;

    return TRUE;
}

void wxApp::CleanUp()
{
    delete wxWidgetHashTable;
    wxWidgetHashTable = NULL;
    delete wxClientWidgetHashTable;
    wxClientWidgetHashTable = NULL;

    wxModule::CleanUpModules();

    delete wxTheColourDatabase;
    wxTheColourDatabase = NULL;

    wxDeleteStockObjects();

    wxDeleteStockLists();

    delete wxTheApp;
    wxTheApp = NULL;

    wxClassInfo::CleanUpClasses();

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
        wxLogDebug("There were memory leaks.");
        wxDebugContext::Dump();
        wxDebugContext::PrintStatistics();
    }
#endif

    // do it as the very last thing because everything else can log messages
    wxLog::DontCreateOnDemand();
    // do it as the very last thing because everything else can log messages
    delete wxLog::SetActiveTarget(NULL);
}

// NB: argc and argv may be changed here, pass by reference!
int wxEntryStart( int& argc, char *argv[] )
{
#ifdef __WXDEBUG__
#if !wxUSE_NANOX
    // install the X error handler
    gs_pfnXErrorHandler = XSetErrorHandler( wxXErrorHandler );
#endif
#endif // __WXDEBUG__

    char *displayName = NULL;
    bool syncDisplay = FALSE;

    int i;
    for (i = 0; i < argc; i++)
    {
        if (strcmp( argv[i], "-display") == 0)
        {
            if (i < (argc - 1))
            {
                i ++;
                displayName = argv[i];
                continue;
            }
        }
        else if (strcmp( argv[i], "-geometry") == 0)
        {
            if (i < (argc - 1))
            {
                i ++;
                int w, h;
                if (sscanf(argv[i], "%dx%d", &w, &h) != 2)
                {
                    wxLogError( _("Invalid geometry specification '%s'"), wxString::FromAscii(argv[i]).c_str() );
                }
                else
                {
                    g_initialSize = wxSize(w, h);
                }
                continue;
            }
        }
        else if (strcmp( argv[i], "-sync") == 0)
        {
            syncDisplay = TRUE;
            continue;
        }
        else if (strcmp( argv[i], "-iconic") == 0)
        {
            g_showIconic = TRUE;

            continue;
        }

    }

    // X11 display stuff
    Display* xdisplay = XOpenDisplay( displayName );
    if (!xdisplay)
    {
        wxLogError( _("wxWindows could not open display. Exiting.") );
        return -1;
    }

    if (syncDisplay)
        XSynchronize(xdisplay, True);

    wxApp::ms_display = (WXDisplay*) xdisplay;

    XSelectInput( xdisplay, XDefaultRootWindow(xdisplay), PropertyChangeMask);

    // Misc.
    wxSetDetectableAutoRepeat( TRUE );

#if wxUSE_UNICODE
    // Glib's type system required by Pango
    g_type_init();
#endif

    if (!wxApp::Initialize())
        return -1;

    return 0;
}

int wxEntryInitGui()
{
    int retValue = 0;

    if ( !wxTheApp->OnInitGui() )
        retValue = -1;

    return retValue;
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
    int err = wxEntryStart(argc, argv);
    if (err)
        return err;

    if (!wxTheApp)
    {
        if (!wxApp::GetInitializerFunction())
        {
            printf( "wxWindows error: No initializer - use IMPLEMENT_APP macro.\n" );
            return 0;
        }

        wxTheApp = (wxApp*) (* wxApp::GetInitializerFunction()) ();
    }

    if (!wxTheApp)
    {
        printf( "wxWindows error: wxTheApp == NULL\n" );
        return 0;
    }

    // Command line argument stuff
    wxTheApp->argc = argc;
#if wxUSE_UNICODE
    wxTheApp->argv = new wxChar*[argc+1];
    int mb_argc = 0;
    while (mb_argc < argc)
    {
        wxString tmp = wxString::FromAscii( argv[mb_argc] );
        wxTheApp->argv[mb_argc] = wxStrdup( tmp.c_str() );
        mb_argc++;
    }
    wxTheApp->argv[mb_argc] = (wxChar *)NULL;
#else
    wxTheApp->argv = argv;
#endif

    if (wxTheApp->argc > 0)
    {
        wxFileName fname( wxTheApp->argv[0] );
        wxTheApp->SetAppName( fname.GetName() );
    }

    wxTheApp->m_showIconic = g_showIconic;
    wxTheApp->m_initialSize = g_initialSize;

    int retValue;
    retValue = wxEntryInitGui();

    // Here frames insert themselves automatically into wxTopLevelWindows by
    // getting created in OnInit().
    if ( retValue == 0 )
    {
        if ( !wxTheApp->OnInit() )
            retValue = -1;
    }

    if ( retValue == 0 )
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

wxApp::wxApp()
{
    // TODO: parse the command line
    argc = 0;
    argv = NULL;

    m_mainColormap = (WXColormap) NULL;
    m_topLevelWidget = (WXWindow) NULL;
    m_maxRequestSize = 0;
    m_mainLoop = NULL;
    m_showIconic = FALSE;
    m_initialSize = wxDefaultSize;

#if !wxUSE_NANOX
    m_visualInfo = NULL;
#endif
}

wxApp::~wxApp()
{
#if !wxUSE_NANOX
    delete m_visualInfo;
#endif
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

#if !wxUSE_NANOX
//-----------------------------------------------------------------------
// X11 predicate function for exposure compression
//-----------------------------------------------------------------------

struct wxExposeInfo
{
    Window window;
    Bool found_non_matching;
};

static Bool expose_predicate (Display *display, XEvent *xevent, XPointer arg)
{
    wxExposeInfo *info = (wxExposeInfo*) arg;

    if (info->found_non_matching)
       return FALSE;

    if (xevent->xany.type != Expose)
    {
        info->found_non_matching = TRUE;
        return FALSE;
    }

    if (xevent->xexpose.window != info->window)
    {
        info->found_non_matching = TRUE;
        return FALSE;
    }

    return TRUE;
}
#endif
    // wxUSE_NANOX

//-----------------------------------------------------------------------
// Processes an X event, returning TRUE if the event was processed.
//-----------------------------------------------------------------------

bool wxApp::ProcessXEvent(WXEvent* _event)
{
    XEvent* event = (XEvent*) _event;

    wxWindow* win = NULL;
    Window window = XEventGetWindow(event);
#if 0
    Window actualWindow = window;
#endif

    // Find the first wxWindow that corresponds to this event window
    // Because we're receiving events after a window
    // has been destroyed, assume a 1:1 match between
    // Window and wxWindow, so if it's not in the table,
    // it must have been destroyed.

    win = wxGetWindowFromTable(window);
    if (!win)
    {
#if wxUSE_TWO_WINDOWS
        win = wxGetClientWindowFromTable(window);
        if (!win)
#endif
            return FALSE;
    }

#ifdef __WXDEBUG__
    wxString windowClass = win->GetClassInfo()->GetClassName();
#endif

    switch (event->type)
    {
        case Expose:
        {
#if wxUSE_TWO_WINDOWS && !wxUSE_NANOX
            if (event->xexpose.window != (Window)win->GetClientAreaWindow())
            {
                XEvent tmp_event;
                wxExposeInfo info;
                info.window = event->xexpose.window;
                info.found_non_matching = FALSE;
                while (XCheckIfEvent( wxGlobalDisplay(), &tmp_event, expose_predicate, (XPointer) &info ))
                {
                    // Don't worry about optimizing redrawing the border etc.
                }
                win->NeedUpdateNcAreaInIdle();
            }
            else
#endif
            {
                win->GetUpdateRegion().Union( XExposeEventGetX(event), XExposeEventGetY(event),
                                              XExposeEventGetWidth(event), XExposeEventGetHeight(event));
                win->GetClearRegion().Union( XExposeEventGetX(event), XExposeEventGetY(event),
                                         XExposeEventGetWidth(event), XExposeEventGetHeight(event));

#if !wxUSE_NANOX
                XEvent tmp_event;
                wxExposeInfo info;
                info.window = event->xexpose.window;
                info.found_non_matching = FALSE;
                while (XCheckIfEvent( wxGlobalDisplay(), &tmp_event, expose_predicate, (XPointer) &info ))
                {
                    win->GetUpdateRegion().Union( tmp_event.xexpose.x, tmp_event.xexpose.y,
                                                  tmp_event.xexpose.width, tmp_event.xexpose.height );

                    win->GetClearRegion().Union( tmp_event.xexpose.x, tmp_event.xexpose.y,
                                                 tmp_event.xexpose.width, tmp_event.xexpose.height );
                }
#endif

                // This simplifies the expose and clear areas to simple
                // rectangles.
                win->GetUpdateRegion() = win->GetUpdateRegion().GetBox();
                win->GetClearRegion() = win->GetClearRegion().GetBox();

                // If we only have one X11 window, always indicate
                // that borders might have to be redrawn.
                if (win->GetMainWindow() == win->GetClientAreaWindow())
                    win->NeedUpdateNcAreaInIdle();

                // Only erase background, paint in idle time.
                win->SendEraseEvents();

                // EXPERIMENT
                //win->Update();
            }

            return TRUE;
        }

#if !wxUSE_NANOX
        case GraphicsExpose:
        {
            printf( "GraphicExpose event\n" );

            wxLogTrace( _T("expose"), _T("GraphicsExpose from %s"), win->GetName().c_str());

            win->GetUpdateRegion().Union( event->xgraphicsexpose.x, event->xgraphicsexpose.y,
                                          event->xgraphicsexpose.width, event->xgraphicsexpose.height);

            win->GetClearRegion().Union( event->xgraphicsexpose.x, event->xgraphicsexpose.y,
                                         event->xgraphicsexpose.width, event->xgraphicsexpose.height);

            if (event->xgraphicsexpose.count == 0)
            {
                // Only erase background, paint in idle time.
                win->SendEraseEvents();
                // win->Update();
            }

            return TRUE;
        }
#endif

        case KeyPress:
        {
            if (!win->IsEnabled())
                return FALSE;

            wxKeyEvent keyEvent(wxEVT_KEY_DOWN);
            wxTranslateKeyEvent(keyEvent, win, window, event);

            // wxLogDebug( "OnKey from %s", win->GetName().c_str() );

            // We didn't process wxEVT_KEY_DOWN, so send wxEVT_CHAR
            if (win->GetEventHandler()->ProcessEvent( keyEvent ))
                return TRUE;

            keyEvent.SetEventType(wxEVT_CHAR);
            // Do the translation again, retaining the ASCII
            // code.
            wxTranslateKeyEvent(keyEvent, win, window, event, TRUE);
            if (win->GetEventHandler()->ProcessEvent( keyEvent ))
                return TRUE;

            if ( (keyEvent.m_keyCode == WXK_TAB) &&
                 win->GetParent() && (win->GetParent()->HasFlag( wxTAB_TRAVERSAL)) )
            {
                wxNavigationKeyEvent new_event;
                new_event.SetEventObject( win->GetParent() );
                /* GDK reports GDK_ISO_Left_Tab for SHIFT-TAB */
                new_event.SetDirection( (keyEvent.m_keyCode == WXK_TAB) );
                /* CTRL-TAB changes the (parent) window, i.e. switch notebook page */
                new_event.SetWindowChange( keyEvent.ControlDown() );
                new_event.SetCurrentFocus( win );
                return win->GetParent()->GetEventHandler()->ProcessEvent( new_event );
            }

            return FALSE;
        }
        case KeyRelease:
        {
            if (!win->IsEnabled())
                return FALSE;

            wxKeyEvent keyEvent(wxEVT_KEY_UP);
            wxTranslateKeyEvent(keyEvent, win, window, event);

            return win->GetEventHandler()->ProcessEvent( keyEvent );
        }
        case ConfigureNotify:
        {
#if wxUSE_NANOX
            if (event->update.utype == GR_UPDATE_SIZE)
#endif
            {
                if (win->IsTopLevel())
                {
                    wxTopLevelWindow *tlw = (wxTopLevelWindow*) win;
                    tlw->SetConfigureGeometry( XConfigureEventGetX(event), XConfigureEventGetY(event),
                        XConfigureEventGetWidth(event), XConfigureEventGetHeight(event) );
                }

                if (win->IsTopLevel() && win->IsShown())
                {
                    wxTopLevelWindowX11 *tlw = (wxTopLevelWindowX11 *) win;
                    tlw->SetNeedResizeInIdle();
                }
                else
                {
                    wxSizeEvent sizeEvent( wxSize(XConfigureEventGetWidth(event), XConfigureEventGetHeight(event)), win->GetId() );
                    sizeEvent.SetEventObject( win );

                    return win->GetEventHandler()->ProcessEvent( sizeEvent );
                }
            }
            return FALSE;
            break;
        }
#if !wxUSE_NANOX
        case PropertyNotify:
        {
            //wxLogDebug("PropertyNotify: %s", windowClass.c_str());
            return HandlePropertyChange(_event);
        }
        case ClientMessage:
        {
            if (!win->IsEnabled())
                return FALSE;

            Atom wm_delete_window = XInternAtom(wxGlobalDisplay(), "WM_DELETE_WINDOW", True);
            Atom wm_protocols = XInternAtom(wxGlobalDisplay(), "WM_PROTOCOLS", True);

            if (event->xclient.message_type == wm_protocols)
            {
                if ((Atom) (event->xclient.data.l[0]) == wm_delete_window)
                {
                    win->Close(FALSE);
                    return TRUE;
                }
            }
            return FALSE;
        }
#if 0
        case DestroyNotify:
        {
            printf( "destroy from %s\n", win->GetName().c_str() );
            break;
        }
        case CreateNotify:
        {
            printf( "create from %s\n", win->GetName().c_str() );
            break;
        }
        case MapRequest:
        {
            printf( "map request from %s\n", win->GetName().c_str() );
            break;
        }
        case ResizeRequest:
        {
            printf( "resize request from %s\n", win->GetName().c_str() );

            Display *disp = (Display*) wxGetDisplay();
            XEvent report;

            //  to avoid flicker
            report = * event;
            while( XCheckTypedWindowEvent (disp, actualWindow, ResizeRequest, &report));

            wxSize sz = win->GetSize();
            wxSizeEvent sizeEvent(sz, win->GetId());
            sizeEvent.SetEventObject(win);

            return win->GetEventHandler()->ProcessEvent( sizeEvent );
        }
#endif
#endif
#if wxUSE_NANOX
        case GR_EVENT_TYPE_CLOSE_REQ:
        {
            if (win)
            {
                win->Close(FALSE);
                return TRUE;
            }
            return FALSE;
            break;
        }
#endif
        case EnterNotify:
        case LeaveNotify:
        case ButtonPress:
        case ButtonRelease:
        case MotionNotify:
        {
            if (!win->IsEnabled())
                return FALSE;

            // Here we check if the top level window is
            // disabled, which is one aspect of modality.
            wxWindow *tlw = win;
            while (tlw && !tlw->IsTopLevel())
                tlw = tlw->GetParent();
            if (tlw && !tlw->IsEnabled())
                return FALSE;

            if (event->type == ButtonPress)
            {
                if ((win != wxWindow::FindFocus()) && win->AcceptsFocus())
                {
                    // This might actually be done in wxWindow::SetFocus()
                    // and not here. TODO.
                    g_prevFocus = wxWindow::FindFocus();
                    g_nextFocus = win;

                    wxLogTrace( _T("focus"), _T("About to call SetFocus on %s of type %s due to button press"), win->GetName().c_str(), win->GetClassInfo()->GetClassName() );

                    // Record the fact that this window is
                    // getting the focus, because we'll need to
                    // check if its parent is getting a bogus
                    // focus and duly ignore it.
                    // TODO: may need to have this code in SetFocus, too.
                    extern wxWindow* g_GettingFocus;
                    g_GettingFocus = win;
                    win->SetFocus();
                }
            }

#if !wxUSE_NANOX
            if (event->type == LeaveNotify || event->type == EnterNotify)
            {
                // Throw out NotifyGrab and NotifyUngrab
                if (event->xcrossing.mode != NotifyNormal)
                    return FALSE;
            }
#endif
            wxMouseEvent wxevent;
            wxTranslateMouseEvent(wxevent, win, window, event);
            return win->GetEventHandler()->ProcessEvent( wxevent );
        }
        case FocusIn:
            {
#if !wxUSE_NANOX
                if ((event->xfocus.detail != NotifyPointer) &&
                    (event->xfocus.mode == NotifyNormal))
#endif
                {
                    wxLogTrace( _T("focus"), _T("FocusIn from %s of type %s"), win->GetName().c_str(), win->GetClassInfo()->GetClassName() );

                    extern wxWindow* g_GettingFocus;
                    if (g_GettingFocus && g_GettingFocus->GetParent() == win)
                    {
                        // Ignore this, this can be a spurious FocusIn
                        // caused by a child having its focus set.
                        g_GettingFocus = NULL;
                        wxLogTrace( _T("focus"), _T("FocusIn from %s of type %s being deliberately ignored"), win->GetName().c_str(), win->GetClassInfo()->GetClassName() );
                        return TRUE;
                    }
                    else
                    {
                        wxFocusEvent focusEvent(wxEVT_SET_FOCUS, win->GetId());
                        focusEvent.SetEventObject(win);
                        focusEvent.SetWindow( g_prevFocus );
                        g_prevFocus = NULL;

                        return win->GetEventHandler()->ProcessEvent(focusEvent);
                    }
                }
                return FALSE;
                break;
            }
        case FocusOut:
            {
#if !wxUSE_NANOX
                if ((event->xfocus.detail != NotifyPointer) &&
                    (event->xfocus.mode == NotifyNormal))
#endif
                {
                    wxLogTrace( _T("focus"), _T("FocusOut from %s of type %s"), win->GetName().c_str(), win->GetClassInfo()->GetClassName() );

                    wxFocusEvent focusEvent(wxEVT_KILL_FOCUS, win->GetId());
                    focusEvent.SetEventObject(win);
                    focusEvent.SetWindow( g_nextFocus );
                    g_nextFocus = NULL;
                    return win->GetEventHandler()->ProcessEvent(focusEvent);
                }
                return FALSE;
                break;
            }
        default:
        {
#ifdef __WXDEBUG__
            //wxString eventName = wxGetXEventName(XEvent& event);
            //wxLogDebug(wxT("Event %s not handled"), eventName.c_str());
#endif
            return FALSE;
            break;
        }
    }
    return FALSE;
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
bool wxApp::HandlePropertyChange(WXEvent *event)
{
    // by default do nothing special
    // TODO: what to do for X11
    // XtDispatchEvent((XEvent*) event);
    return FALSE;
}

void wxApp::OnIdle(wxIdleEvent& event)
{
    static bool s_inOnIdle = FALSE;

    // Avoid recursion (via ProcessEvent default case)
    if (s_inOnIdle)
        return;

    s_inOnIdle = TRUE;

    // Resend in the main thread events which have been prepared in other
    // threads
    ProcessPendingEvents();

    // 'Garbage' collection of windows deleted with Close()
    DeletePendingObjects();

    // Send OnIdle events to all windows
    bool needMore = SendIdleEvents();

    if (needMore)
        event.RequestMore(TRUE);

    s_inOnIdle = FALSE;
}

void wxApp::WakeUpIdle()
{
    // TODO: use wxMotif implementation?

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

    win->GetEventHandler()->ProcessEvent(event);

    if (event.MoreRequested())
        needMore = TRUE;

    wxWindowListNode* node = win->GetChildren().GetFirst();
    while (node)
    {
        wxWindow* win = (wxWindow*) node->GetData();
        if (SendIdleEvents(win))
            needMore = TRUE;

        node = node->GetNext();
    }

    win->OnInternalIdle();

    return needMore;
}

void wxApp::DeletePendingObjects()
{
    wxNode *node = wxPendingDelete.GetFirst();
    while (node)
    {
        wxObject *obj = (wxObject *)node->GetData();

        delete obj;

        if (wxPendingDelete.Member(obj))
            delete node;

        // Deleting one object may have deleted other pending
        // objects, so start from beginning of list again.
        node = wxPendingDelete.GetFirst();
    }
}

// Create display, and other initialization
bool wxApp::OnInitGui()
{
    // Eventually this line will be removed, but for
    // now we don't want to try popping up a dialog
    // for error messages.
    delete wxLog::SetActiveTarget(new wxLogStderr);

    if (!wxAppBase::OnInitGui())
        return FALSE;

    GetMainColormap( wxApp::GetDisplay() );

    m_maxRequestSize = XMaxRequestSize( (Display*) wxApp::GetDisplay() );

#if !wxUSE_NANOX
    m_visualInfo = new wxXVisualInfo;
    wxFillXVisualInfo( m_visualInfo, (Display*) wxApp::GetDisplay() );
#endif

    return TRUE;
}

#if wxUSE_UNICODE

#include <pango/pango.h>
#include <pango/pangox.h>
#include <pango/pangoxft.h>

PangoContext* wxApp::GetPangoContext()
{
    static PangoContext *ret = NULL;
    if (ret)
        return ret;

    Display *xdisplay = (Display*) wxApp::GetDisplay();

#if 1
    int xscreen = DefaultScreen(xdisplay);
    static int use_xft = -1;
    if (use_xft == -1)
    {
        wxString val = wxGetenv( L"GDK_USE_XFT" );
        use_xft = (val == L"1");
    }

    if (use_xft)
        ret = pango_xft_get_context( xdisplay, xscreen );
    else
#endif
        ret = pango_x_get_context( xdisplay );

    if (!PANGO_IS_CONTEXT(ret))
        wxLogError( wxT("No pango context.") );

    return ret;
}
#endif

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
    wxASSERT_MSG( window, "invalid window" );

    return (Window) 0;

    Window parent, root = 0;
#if wxUSE_NANOX
    int noChildren = 0;
#else
    unsigned int noChildren = 0;
#endif
    Window* children = NULL;

    // #define XQueryTree(d,w,r,p,c,nc)     GrQueryTree(w,p,c,nc)
    int res = 1;
#if !wxUSE_NANOX
    res =
#endif
        XQueryTree((Display*) wxGetDisplay(), window, & root, & parent,
             & children, & noChildren);
    if (children)
        XFree(children);
    if (res)
        return parent;
    else
        return (Window) 0;
}

void wxApp::Exit()
{
    wxApp::CleanUp();

    wxAppConsole::Exit();
}

// Yield to other processes

bool wxApp::Yield(bool onlyIfNeeded)
{
    // Sometimes only 2 yields seem
    // to do the trick, e.g. in the
    // progress dialog
    int i;
    for (i = 0; i < 2; i++)
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

        // Make sure we have an event loop object,
        // or Pending/Dispatch will fail
        wxEventLoop* eventLoop = wxEventLoop::GetActive();
        wxEventLoop* newEventLoop = NULL;
        if (!eventLoop)
        {
            newEventLoop = new wxEventLoop;
            wxEventLoop::SetActive(newEventLoop);
        }

        // Call dispatch at least once so that sockets
        // can be tested
        wxTheApp->Dispatch();

        while (wxTheApp && wxTheApp->Pending())
            wxTheApp->Dispatch();

#if wxUSE_TIMER
        wxTimer::NotifyTimers();
#endif
        ProcessIdle();

        if (newEventLoop)
        {
            wxEventLoop::SetActive(NULL);
            delete newEventLoop;
        }

        s_inYield = FALSE;
    }

    return TRUE;
}

#ifdef __WXDEBUG__

void wxApp::OnAssert(const wxChar *file, int line, const wxChar* cond, const wxChar *msg)
{
    // While the GUI isn't working that well, just print out the
    // message.
#if 1
    wxAppBase::OnAssert(file, line, cond, msg);
#else
    wxString msg2;
    msg2.Printf("At file %s:%d: %s", file, line, msg);
    wxLogDebug(msg2);
#endif
}

#endif // __WXDEBUG__

