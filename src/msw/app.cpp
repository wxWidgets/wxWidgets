/////////////////////////////////////////////////////////////////////////////
// Name:        app.cpp
// Purpose:     wxApp
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ===========================================================================
// declarations
// ===========================================================================

// ---------------------------------------------------------------------------
// headers
// ---------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "app.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if defined(__BORLANDC__)
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
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
    #include "wx/intl.h"
    #include "wx/dynarray.h"
    #include "wx/wxchar.h"
    #include "wx/icon.h"
    #include "wx/log.h"
#endif

#include "wx/apptrait.h"
#include "wx/filename.h"
#include "wx/module.h"

#include "wx/msw/private.h"

#if wxUSE_THREADS
    #include "wx/thread.h"

    // define the array of MSG strutures
    WX_DECLARE_OBJARRAY(MSG, wxMsgArray);

    #include "wx/arrimpl.cpp"

    WX_DEFINE_OBJARRAY(wxMsgArray);
#endif // wxUSE_THREADS

#if wxUSE_TOOLTIPS
    #include "wx/tooltip.h"
#endif // wxUSE_TOOLTIPS

// OLE is used for drag-and-drop, clipboard, OLE Automation..., but some
// compilers don't support it (missing headers, libs, ...)
#if defined(__GNUWIN32_OLD__) || defined(__SYMANTEC__) || defined(__SALFORDC__)
    #undef wxUSE_OLE

    #define  wxUSE_OLE 0
#endif // broken compilers

#if wxUSE_OLE
    #include <ole2.h>
#endif

#include <string.h>
#include <ctype.h>

#if defined(__WIN95__) && !((defined(__GNUWIN32_OLD__) || defined(__WXMICROWIN__)) && !defined(__CYGWIN10__))
    #include <commctrl.h>
#endif

// ----------------------------------------------------------------------------
// conditional compilation
// ----------------------------------------------------------------------------

// The macro _WIN32_IE is defined by commctrl.h (unless it had already been
// defined before) and shows us what common control features are available
// during the compile time (it doesn't mean that they will be available during
// the run-time, use GetComCtl32Version() to test for them!). The possible
// values are:
//
// 0x0200     for comctl32.dll 4.00 shipped with Win95/NT 4.0
// 0x0300                      4.70              IE 3.x
// 0x0400                      4.71              IE 4.0
// 0x0401                      4.72              IE 4.01 and Win98
// 0x0500                      5.00              IE 5.x and NT 5.0 (Win2000)

#ifndef _WIN32_IE
    // minimal set of features by default
    #define _WIN32_IE 0x0200
#endif

#if _WIN32_IE >= 0x0300 && \
    (!defined(__MINGW32__) || wxCHECK_W32API_VERSION( 2, 0 )) && \
    !defined(__CYGWIN__)
    #include <shlwapi.h>
#endif

// ---------------------------------------------------------------------------
// global variables
// ---------------------------------------------------------------------------

extern wxList WXDLLEXPORT wxPendingDelete;
#ifndef __WXMICROWIN__
extern void wxSetKeyboardHook(bool doIt);
#endif

MSG s_currentMsg;

// NB: all "NoRedraw" classes must have the same names as the "normal" classes
//     with NR suffix - wxWindow::MSWCreate() supposes this
const wxChar *wxCanvasClassName        = wxT("wxWindowClass");
const wxChar *wxCanvasClassNameNR      = wxT("wxWindowClassNR");
const wxChar *wxMDIFrameClassName      = wxT("wxMDIFrameClass");
const wxChar *wxMDIFrameClassNameNoRedraw = wxT("wxMDIFrameClassNR");
const wxChar *wxMDIChildFrameClassName = wxT("wxMDIChildFrameClass");
const wxChar *wxMDIChildFrameClassNameNoRedraw = wxT("wxMDIChildFrameClassNR");

HBRUSH wxDisableButtonBrush = (HBRUSH) 0;

// ----------------------------------------------------------------------------
// private functions
// ----------------------------------------------------------------------------

LRESULT WXDLLEXPORT APIENTRY wxWndProc(HWND, UINT, WPARAM, LPARAM);

// ===========================================================================
// wxGUIAppTraits implementation
// ===========================================================================

// private class which we use to pass parameters from BeforeChildWaitLoop() to
// AfterChildWaitLoop()
struct ChildWaitLoopData
{
    ChildWaitLoopData(wxWindowDisabler *wd_, wxWindow *winActive_)
    {
        wd = wd_;
        winActive = winActive_;
    }

    wxWindowDisabler *wd;
    wxWindow *winActive;
};

void *wxGUIAppTraits::BeforeChildWaitLoop()
{
    /*
       We use a dirty hack here to disable all application windows (which we
       must do because otherwise the calls to wxYield() could lead to some very
       unexpected reentrancies in the users code) but to avoid losing
       focus/activation entirely when the child process terminates which would
       happen if we simply disabled everything using wxWindowDisabler. Indeed,
       remember that Windows will never activate a disabled window and when the
       last childs window is closed and Windows looks for a window to activate
       all our windows are still disabled. There is no way to enable them in
       time because we don't know when the childs windows are going to be
       closed, so the solution we use here is to keep one special tiny frame
       enabled all the time. Then when the child terminates it will get
       activated and when we close it below -- after reenabling all the other
       windows! -- the previously active window becomes activated again and
       everything is ok.
     */
    wxBeginBusyCursor();

    // first disable all existing windows
    wxWindowDisabler *wd = new wxWindowDisabler;

    // then create an "invisible" frame: it has minimal size, is positioned
    // (hopefully) outside the screen and doesn't appear on the taskbar
    wxWindow *winActive = new wxFrame
                    (
                        wxTheApp->GetTopWindow(),
                        -1,
                        _T(""),
                        wxPoint(32600, 32600),
                        wxSize(1, 1),
                        wxDEFAULT_FRAME_STYLE | wxFRAME_NO_TASKBAR
                    );
    winActive->Show();

    return new ChildWaitLoopData(wd, winActive);
}

void wxGUIAppTraits::AlwaysYield()
{
    wxYield();
}

void wxGUIAppTraits::AfterChildWaitLoop(void *dataOrig)
{
    wxEndBusyCursor();

    const ChildWaitLoopData * const data = (ChildWaitLoopData *)dataOrig;

    delete data->wd;

    // finally delete the dummy frame and, as wd has been already destroyed and
    // the other windows reenabled, the activation is going to return to the
    // window which had had it before
    data->winActive->Destroy();
}

bool wxGUIAppTraits::DoMessageFromThreadWait()
{
    return !wxTheApp || wxTheApp->DoMessage();
}

// ===========================================================================
// wxApp implementation
// ===========================================================================

int wxApp::m_nCmdShow = SW_SHOWNORMAL;

// ---------------------------------------------------------------------------
// wxWin macros
// ---------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxApp, wxEvtHandler)

BEGIN_EVENT_TABLE(wxApp, wxEvtHandler)
    EVT_IDLE(wxApp::OnIdle)
    EVT_END_SESSION(wxApp::OnEndSession)
    EVT_QUERY_END_SESSION(wxApp::OnQueryEndSession)
END_EVENT_TABLE()

// class to ensure that wxAppBase::CleanUp() is called if our Initialize()
// fails
class wxCallBaseCleanup
{
public:
    wxCallBaseCleanup(wxApp *app) : m_app(app) { }
    ~wxCallBaseCleanup() { if ( m_app ) m_app->wxAppBase::CleanUp(); }

    void Dismiss() { m_app = NULL; }

private:
    wxApp *m_app;
};

//// Initialize
bool wxApp::Initialize(int& argc, wxChar **argv)
{
    if ( !wxAppBase::Initialize(argc, argv) )
        return false;

    // ensure that base cleanup is done if we return too early
    wxCallBaseCleanup callBaseCleanup(this);

    // the first thing to do is to check if we're trying to run an Unicode
    // program under Win9x w/o MSLU emulation layer - if so, abort right now
    // as it has no chance to work
#if wxUSE_UNICODE && !wxUSE_UNICODE_MSLU
    if ( wxGetOsVersion() != wxWINDOWS_NT )
    {
        // note that we can use MessageBoxW() as it's implemented even under
        // Win9x - OTOH, we can't use wxGetTranslation() because the file APIs
        // used by wxLocale are not
        ::MessageBox
        (
         NULL,
         _T("This program uses Unicode and requires Windows NT/2000/XP.\nProgram aborted."),
         _T("wxWindows Fatal Error"),
         MB_ICONERROR | MB_OK
        );

        return FALSE;
    }
#endif // wxUSE_UNICODE && !wxUSE_UNICODE_MSLU

#if defined(__WIN95__) && !defined(__WXMICROWIN__)
    InitCommonControls();
#endif // __WIN95__

#if wxUSE_OLE || wxUSE_DRAG_AND_DROP

#ifdef __WIN16__
    // for OLE, enlarge message queue to be as large as possible
    int iMsg = 96;
    while (!SetMessageQueue(iMsg) && (iMsg -= 8))
        ;
#endif // Win16

#if wxUSE_OLE
    // we need to initialize OLE library
    if ( FAILED(::OleInitialize(NULL)) )
        wxLogError(_("Cannot initialize OLE"));
#endif

#endif // wxUSE_OLE

#if wxUSE_CTL3D
    if (!Ctl3dRegister(wxhInstance))
        wxLogError(wxT("Cannot register CTL3D"));

    Ctl3dAutoSubclass(wxhInstance);
#endif // wxUSE_CTL3D

    RegisterWindowClasses();

#ifndef __WXMICROWIN__
    // Create the brush for disabling bitmap buttons

    LOGBRUSH lb;
    lb.lbStyle = BS_PATTERN;
    lb.lbColor = 0;
    lb.lbHatch = (int)LoadBitmap( wxhInstance, wxT("wxDISABLE_BUTTON_BITMAP") );
    if ( lb.lbHatch )
    {
        wxDisableButtonBrush = ::CreateBrushIndirect( & lb );
        ::DeleteObject( (HGDIOBJ)lb.lbHatch );
    }
    //else: wxWindows resources are probably not linked in
#endif

#if wxUSE_PENWINDOWS
    wxRegisterPenWin();
#endif

    wxWinHandleHash = new wxWinHashTable(wxKEY_INTEGER, 100);

    // This is to foil optimizations in Visual C++ that throw out dummy.obj.
    // PLEASE DO NOT ALTER THIS.
#if defined(__VISUALC__) && defined(__WIN16__) && !defined(WXMAKINGDLL)
    extern char wxDummyChar;
    if (wxDummyChar) wxDummyChar++;
#endif

#ifndef __WXMICROWIN__
    wxSetKeyboardHook(TRUE);
#endif

    callBaseCleanup.Dismiss();

    return true;
}

// ---------------------------------------------------------------------------
// RegisterWindowClasses
// ---------------------------------------------------------------------------

// TODO we should only register classes really used by the app. For this it
//      would be enough to just delay the class registration until an attempt
//      to create a window of this class is made.
bool wxApp::RegisterWindowClasses()
{
    WNDCLASS wndclass;
    wxZeroMemory(wndclass);

    // for each class we register one with CS_(V|H)REDRAW style and one
    // without for windows created with wxNO_FULL_REDRAW_ON_REPAINT flag
    static const long styleNormal = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
    static const long styleNoRedraw = CS_DBLCLKS;

    // the fields which are common to all classes
    wndclass.lpfnWndProc   = (WNDPROC)wxWndProc;
    wndclass.hInstance     = wxhInstance;
    wndclass.hCursor       = ::LoadCursor((HINSTANCE)NULL, IDC_ARROW);

    // Register the frame window class.
    wndclass.hbrBackground = (HBRUSH)(COLOR_APPWORKSPACE + 1);
    wndclass.lpszClassName = wxCanvasClassName;
    wndclass.style         = styleNormal;

    if ( !RegisterClass(&wndclass) )
    {
        wxLogLastError(wxT("RegisterClass(frame)"));
    }

    // "no redraw" frame
    wndclass.lpszClassName = wxCanvasClassNameNR;
    wndclass.style         = styleNoRedraw;

    if ( !RegisterClass(&wndclass) )
    {
        wxLogLastError(wxT("RegisterClass(no redraw frame)"));
    }

    // Register the MDI frame window class.
    wndclass.hbrBackground = (HBRUSH)NULL; // paint MDI frame ourselves
    wndclass.lpszClassName = wxMDIFrameClassName;
    wndclass.style         = styleNormal;

    if ( !RegisterClass(&wndclass) )
    {
        wxLogLastError(wxT("RegisterClass(MDI parent)"));
    }

    // "no redraw" MDI frame
    wndclass.lpszClassName = wxMDIFrameClassNameNoRedraw;
    wndclass.style         = styleNoRedraw;

    if ( !RegisterClass(&wndclass) )
    {
        wxLogLastError(wxT("RegisterClass(no redraw MDI parent frame)"));
    }

    // Register the MDI child frame window class.
    wndclass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wndclass.lpszClassName = wxMDIChildFrameClassName;
    wndclass.style         = styleNormal;

    if ( !RegisterClass(&wndclass) )
    {
        wxLogLastError(wxT("RegisterClass(MDI child)"));
    }

    // "no redraw" MDI child frame
    wndclass.lpszClassName = wxMDIChildFrameClassNameNoRedraw;
    wndclass.style         = styleNoRedraw;

    if ( !RegisterClass(&wndclass) )
    {
        wxLogLastError(wxT("RegisterClass(no redraw MDI child)"));
    }

    return TRUE;
}

// ---------------------------------------------------------------------------
// UnregisterWindowClasses
// ---------------------------------------------------------------------------

bool wxApp::UnregisterWindowClasses()
{
    bool retval = TRUE;

#ifndef __WXMICROWIN__
    // MDI frame window class.
    if ( !::UnregisterClass(wxMDIFrameClassName, wxhInstance) )
    {
        wxLogLastError(wxT("UnregisterClass(MDI parent)"));

        retval = FALSE;
    }

    // "no redraw" MDI frame
    if ( !::UnregisterClass(wxMDIFrameClassNameNoRedraw, wxhInstance) )
    {
        wxLogLastError(wxT("UnregisterClass(no redraw MDI parent frame)"));

        retval = FALSE;
    }

    // MDI child frame window class.
    if ( !::UnregisterClass(wxMDIChildFrameClassName, wxhInstance) )
    {
        wxLogLastError(wxT("UnregisterClass(MDI child)"));

        retval = FALSE;
    }

    // "no redraw" MDI child frame
    if ( !::UnregisterClass(wxMDIChildFrameClassNameNoRedraw, wxhInstance) )
    {
        wxLogLastError(wxT("UnregisterClass(no redraw MDI child)"));

        retval = FALSE;
    }

    // canvas class name
    if ( !::UnregisterClass(wxCanvasClassName, wxhInstance) )
    {
        wxLogLastError(wxT("UnregisterClass(canvas)"));

        retval = FALSE;
    }

    if ( !::UnregisterClass(wxCanvasClassNameNR, wxhInstance) )
    {
        wxLogLastError(wxT("UnregisterClass(no redraw canvas)"));

        retval = FALSE;
    }
#endif // __WXMICROWIN__

    return retval;
}

void wxApp::CleanUp()
{
#ifndef __WXMICROWIN__
    wxSetKeyboardHook(FALSE);
#endif

#if wxUSE_PENWINDOWS
    wxCleanUpPenWin();
#endif

    if ( wxDisableButtonBrush )
        ::DeleteObject( wxDisableButtonBrush );

#if wxUSE_OLE
    ::OleUninitialize();
#endif

    // for an EXE the classes are unregistered when it terminates but DLL may
    // be loaded several times (load/unload/load) into the same process in
    // which case the registration will fail after the first time if we don't
    // unregister the classes now
    UnregisterWindowClasses();

#if wxUSE_CTL3D
    Ctl3dUnregister(wxhInstance);
#endif

    delete wxWinHandleHash;
    wxWinHandleHash = NULL;

    wxAppBase::CleanUp();
}

// ----------------------------------------------------------------------------
// wxApp ctor/dtor
// ----------------------------------------------------------------------------

wxApp::wxApp()
{
    m_printMode = wxPRINT_WINDOWS;
}

wxApp::~wxApp()
{
    // our cmd line arguments are allocated inside wxEntry(HINSTANCE), they
    // don't come from main(), so we have to free them

    while ( argc )
    {
        // m_argv elements were allocated by wxStrdup()
        free(argv[--argc]);
    }

    // but m_argv itself -- using new[]
    delete [] argv;
}

bool wxApp::Initialized()
{
#ifndef _WINDLL
    if (GetTopWindow())
        return TRUE;
    else
        return FALSE;
#else // Assume initialized if DLL (no way of telling)
    return TRUE;
#endif
}

/*
 * Get and process a message, returning FALSE if WM_QUIT
 * received (and also set the flag telling the app to exit the main loop)
 *
 */
bool wxApp::DoMessage()
{
    BOOL rc = ::GetMessage(&s_currentMsg, (HWND) NULL, 0, 0);
    if ( rc == 0 )
    {
        // got WM_QUIT
        m_keepGoing = FALSE;

        return FALSE;
    }
    else if ( rc == -1 )
    {
        // should never happen, but let's test for it nevertheless
        wxLogLastError(wxT("GetMessage"));
    }
    else
    {
#if wxUSE_THREADS
        wxASSERT_MSG( wxThread::IsMain(),
                      wxT("only the main thread can process Windows messages") );

        static bool s_hadGuiLock = TRUE;
        static wxMsgArray s_aSavedMessages;

        // if a secondary thread owns is doing GUI calls, save all messages for
        // later processing - we can't process them right now because it will
        // lead to recursive library calls (and we're not reentrant)
        if ( !wxGuiOwnedByMainThread() )
        {
            s_hadGuiLock = FALSE;

            // leave out WM_COMMAND messages: too dangerous, sometimes
            // the message will be processed twice
            if ( !wxIsWaitingForThread() ||
                    s_currentMsg.message != WM_COMMAND )
            {
                s_aSavedMessages.Add(s_currentMsg);
            }

            return TRUE;
        }
        else
        {
            // have we just regained the GUI lock? if so, post all of the saved
            // messages
            //
            // FIXME of course, it's not _exactly_ the same as processing the
            //       messages normally - expect some things to break...
            if ( !s_hadGuiLock )
            {
                s_hadGuiLock = TRUE;

                size_t count = s_aSavedMessages.GetCount();
                for ( size_t n = 0; n < count; n++ )
                {
                    MSG& msg = s_aSavedMessages[n];

                    DoMessage((WXMSG *)&msg);
                }

                s_aSavedMessages.Empty();
            }
        }
#endif // wxUSE_THREADS

        // Process the message
        DoMessage((WXMSG *)&s_currentMsg);
    }

    return TRUE;
}

void wxApp::DoMessage(WXMSG *pMsg)
{
    if ( !ProcessMessage(pMsg) )
    {
        ::TranslateMessage((MSG *)pMsg);
        ::DispatchMessage((MSG *)pMsg);
    }
}

/*
 * Keep trying to process messages until WM_QUIT
 * received.
 *
 * If there are messages to be processed, they will all be
 * processed and OnIdle will not be called.
 * When there are no more messages, OnIdle is called.
 * If OnIdle requests more time,
 * it will be repeatedly called so long as there are no pending messages.
 * A 'feature' of this is that once OnIdle has decided that no more processing
 * is required, then it won't get processing time until further messages
 * are processed (it'll sit in DoMessage).
 */

int wxApp::MainLoop()
{
    m_keepGoing = TRUE;

    while ( m_keepGoing )
    {
#if wxUSE_THREADS
        wxMutexGuiLeaveOrEnter();
#endif // wxUSE_THREADS

        while ( !Pending() && ProcessIdle() )
            ;

        // a message came or no more idle processing to do
        DoMessage();
    }

    return s_currentMsg.wParam;
}

// Returns TRUE if more time is needed.
bool wxApp::ProcessIdle()
{
    wxIdleEvent event;
    event.SetEventObject(this);
    ProcessEvent(event);

    wxUpdateUIEvent::ResetUpdateTime();
    
    return event.MoreRequested();
}

void wxApp::ExitMainLoop()
{
    // this will set m_keepGoing to FALSE a bit later
    ::PostQuitMessage(0);
}

bool wxApp::Pending()
{
    return ::PeekMessage(&s_currentMsg, 0, 0, 0, PM_NOREMOVE) != 0;
}

void wxApp::Dispatch()
{
    DoMessage();
}

/*
 * Give all windows a chance to preprocess
 * the message. Some may have accelerator tables, or have
 * MDI complications.
 */

bool wxApp::ProcessMessage(WXMSG *wxmsg)
{
    MSG *msg = (MSG *)wxmsg;
    HWND hwnd = msg->hwnd;
    wxWindow *wndThis = wxGetWindowFromHWND((WXHWND)hwnd);

    // this may happen if the event occured in a standard modeless dialog (the
    // only example of which I know of is the find/replace dialog) - then call
    // IsDialogMessage() to make TAB navigation in it work
    if ( !wndThis )
    {
        // we need to find the dialog containing this control as
        // IsDialogMessage() just eats all the messages (i.e. returns TRUE for
        // them) if we call it for the control itself
        while ( hwnd && ::GetWindowLong(hwnd, GWL_STYLE) & WS_CHILD )
        {
            hwnd = ::GetParent(hwnd);
        }

        return hwnd && ::IsDialogMessage(hwnd, msg) != 0;
    }

#if wxUSE_TOOLTIPS
    // we must relay WM_MOUSEMOVE events to the tooltip ctrl if we want it to
    // popup the tooltip bubbles
    if ( (msg->message == WM_MOUSEMOVE) )
    {
        wxToolTip *tt = wndThis->GetToolTip();
        if ( tt )
        {
            tt->RelayEvent(wxmsg);
        }
    }
#endif // wxUSE_TOOLTIPS

    // allow the window to prevent certain messages from being
    // translated/processed (this is currently used by wxTextCtrl to always
    // grab Ctrl-C/V/X, even if they are also accelerators in some parent)
    if ( !wndThis->MSWShouldPreProcessMessage(wxmsg) )
    {
        return FALSE;
    }

    // try translations first: the accelerators override everything
    wxWindow *wnd;

    for ( wnd = wndThis; wnd; wnd = wnd->GetParent() )
    {
        if ( wnd->MSWTranslateMessage(wxmsg))
            return TRUE;

        // stop at first top level window, i.e. don't try to process the key
        // strokes originating in a dialog using the accelerators of the parent
        // frame - this doesn't make much sense
        if ( wnd->IsTopLevel() )
            break;
    }

    // now try the other hooks (kbd navigation is handled here): we start from
    // wndThis->GetParent() because wndThis->MSWProcessMessage() was already
    // called above
    for ( wnd = wndThis->GetParent(); wnd; wnd = wnd->GetParent() )
    {
        if ( wnd->MSWProcessMessage(wxmsg) )
            return TRUE;
    }

    // no special preprocessing for this message, dispatch it normally
    return FALSE;
}

// this is a temporary hack and will be replaced by using wxEventLoop in the
// future
//
// it is needed to allow other event loops (currently only one: the modal
// dialog one) to reset the OnIdle() semaphore because otherwise OnIdle()
// wouldn't do anything while a modal dialog shown from OnIdle() call is shown.
bool wxIsInOnIdleFlag = FALSE;

void wxApp::OnIdle(wxIdleEvent& event)
{
    // Avoid recursion (via ProcessEvent default case)
    if ( wxIsInOnIdleFlag )
        return;

    wxIsInOnIdleFlag = TRUE;

    // If there are pending events, we must process them: pending events
    // are either events to the threads other than main or events posted
    // with wxPostEvent() functions
    // GRG: I have moved this here so that all pending events are processed
    //   before starting to delete any objects. This behaves better (in
    //   particular, wrt wxPostEvent) and is coherent with wxGTK's current
    //   behaviour. Changed Feb/2000 before 2.1.14
    ProcessPendingEvents();

    // 'Garbage' collection of windows deleted with Close().
    DeletePendingObjects();

#if wxUSE_LOG
    // flush the logged messages if any
    wxLog::FlushActive();
#endif // wxUSE_LOG

#if wxUSE_DC_CACHEING
    // automated DC cache management: clear the cached DCs and bitmap
    // if it's likely that the app has finished with them, that is, we
    // get an idle event and we're not dragging anything.
    if (!::GetKeyState(MK_LBUTTON) && !::GetKeyState(MK_MBUTTON) && !::GetKeyState(MK_RBUTTON))
        wxDC::ClearCache();
#endif // wxUSE_DC_CACHEING

    // Send OnIdle events to all windows
    if ( SendIdleEvents() )
    {
        // SendIdleEvents() returns TRUE if at least one window requested more
        // idle events
        event.RequestMore(TRUE);
    }

    wxIsInOnIdleFlag = FALSE;
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
    wxIdleEvent event;
    event.SetEventObject(win);
    win->GetEventHandler()->ProcessEvent(event);

    bool needMore = event.MoreRequested();

    wxWindowList::Node *node = win->GetChildren().GetFirst();
    while ( node )
    {
        wxWindow *win = node->GetData();
        if (SendIdleEvents(win))
            needMore = TRUE;

        node = node->GetNext();
    }

    return needMore;
}

void wxApp::WakeUpIdle()
{
    // Send the top window a dummy message so idle handler processing will
    // start up again.  Doing it this way ensures that the idle handler
    // wakes up in the right thread (see also wxWakeUpMainThread() which does
    // the same for the main app thread only)
    wxWindow *topWindow = wxTheApp->GetTopWindow();
    if ( topWindow )
    {
        if ( !::PostMessage(GetHwndOf(topWindow), WM_NULL, 0, 0) )
        {
            // should never happen
            wxLogLastError(wxT("PostMessage(WM_NULL)"));
        }
    }
}

void wxApp::OnEndSession(wxCloseEvent& WXUNUSED(event))
{
    if (GetTopWindow())
        GetTopWindow()->Close(TRUE);
}

// Default behaviour: close the application with prompts. The
// user can veto the close, and therefore the end session.
void wxApp::OnQueryEndSession(wxCloseEvent& event)
{
    if (GetTopWindow())
    {
        if (!GetTopWindow()->Close(!event.CanVeto()))
            event.Veto(TRUE);
    }
}

typedef struct _WXADllVersionInfo
{
        DWORD cbSize;
        DWORD dwMajorVersion;                   // Major version
        DWORD dwMinorVersion;                   // Minor version
        DWORD dwBuildNumber;                    // Build number
        DWORD dwPlatformID;                     // DLLVER_PLATFORM_*
} WXADLLVERSIONINFO;

typedef HRESULT (CALLBACK* WXADLLGETVERSIONPROC)(WXADLLVERSIONINFO *);

/* static */
int wxApp::GetComCtl32Version()
{
#ifdef __WXMICROWIN__
    return 0;
#else
    // cache the result
    static int s_verComCtl32 = -1;

    wxCRIT_SECT_DECLARE(csComCtl32);
    wxCRIT_SECT_LOCKER(lock, csComCtl32);

    if ( s_verComCtl32 == -1 )
    {
        // initally assume no comctl32.dll at all
        s_verComCtl32 = 0;

        // do we have it?
        HMODULE hModuleComCtl32 = ::GetModuleHandle(wxT("COMCTL32"));
        BOOL bFreeComCtl32 = FALSE ;
        if(!hModuleComCtl32)
        {
            hModuleComCtl32 = ::LoadLibrary(wxT("COMCTL32.DLL")) ;
            if(hModuleComCtl32)
            {
                bFreeComCtl32 = TRUE ;
            }
        }

        // if so, then we can check for the version
        if ( hModuleComCtl32 )
        {
            // try to use DllGetVersion() if available in _headers_
                WXADLLGETVERSIONPROC pfnDllGetVersion = (WXADLLGETVERSIONPROC)
                    ::GetProcAddress(hModuleComCtl32, "DllGetVersion");
                if ( pfnDllGetVersion )
                {
                    WXADLLVERSIONINFO dvi;
                    dvi.cbSize = sizeof(dvi);

                    HRESULT hr = (*pfnDllGetVersion)(&dvi);
                    if ( FAILED(hr) )
                    {
                        wxLogApiError(_T("DllGetVersion"), hr);
                    }
                    else
                    {
                        // this is incompatible with _WIN32_IE values, but
                        // compatible with the other values returned by
                        // GetComCtl32Version()
                        s_verComCtl32 = 100*dvi.dwMajorVersion +
                                            dvi.dwMinorVersion;
                    }
                }
                // DllGetVersion() unavailable either during compile or
                // run-time, try to guess the version otherwise
                if ( !s_verComCtl32 )
                {
                    // InitCommonControlsEx is unique to 4.70 and later
                    FARPROC theProc = ::GetProcAddress
                                        (
                                         hModuleComCtl32,
                                         "InitCommonControlsEx"
                                        );

                    if ( !theProc )
                    {
                        // not found, must be 4.00
                        s_verComCtl32 = 400;
                    }
                    else
                    {
                        // many symbols appeared in comctl32 4.71, could use
                        // any of them except may be DllInstall
                        theProc = ::GetProcAddress
                                    (
                                     hModuleComCtl32,
                                     "InitializeFlatSB"
                                    );
                        if ( !theProc )
                        {
                            // not found, must be 4.70
                            s_verComCtl32 = 470;
                        }
                        else
                        {
                            // found, must be 4.71
                            s_verComCtl32 = 471;
                        }
                    }
                }
        }

        if(bFreeComCtl32)
        {
            ::FreeLibrary(hModuleComCtl32) ;
        }
    }

    return s_verComCtl32;
#endif
}

// Yield to incoming messages

bool wxApp::Yield(bool onlyIfNeeded)
{
    // MT-FIXME
    static bool s_inYield = FALSE;

#if wxUSE_LOG
    // disable log flushing from here because a call to wxYield() shouldn't
    // normally result in message boxes popping up &c
    wxLog::Suspend();
#endif // wxUSE_LOG

    if ( s_inYield )
    {
        if ( !onlyIfNeeded )
        {
            wxFAIL_MSG( wxT("wxYield called recursively" ) );
        }

        return FALSE;
    }

    s_inYield = TRUE;

    // we don't want to process WM_QUIT from here - it should be processed in
    // the main event loop in order to stop it
    MSG msg;
    while ( PeekMessage(&msg, (HWND)0, 0, 0, PM_NOREMOVE) &&
            msg.message != WM_QUIT )
    {
#if wxUSE_THREADS
        wxMutexGuiLeaveOrEnter();
#endif // wxUSE_THREADS

        if ( !wxTheApp->DoMessage() )
            break;
    }

    // if there are pending events, we must process them.
    ProcessPendingEvents();

#if wxUSE_LOG
    // let the logs be flashed again
    wxLog::Resume();
#endif // wxUSE_LOG

    s_inYield = FALSE;

    return TRUE;
}

