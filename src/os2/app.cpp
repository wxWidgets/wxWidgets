/////////////////////////////////////////////////////////////////////////////
// Name:        app.cpp
// Purpose:     wxApp
// Author:      David Webster
// Modified by:
// Created:     10/13/99
// RCS-ID:      $Id$
// Copyright:   (c) David Webster
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

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
#   include "wx/wxchar.h"
#   include "wx/icon.h"
#endif

#include "wx/log.h"
#include "wx/module.h"

#include "wx/os2/private.h"

#if wxUSE_THREADS
    #include "wx/thread.h"

    // define the array of MSG strutures
// TODO:    WX_DECLARE_OBJARRAY(MSG, wxMsgArray);

    #include "wx/arrimpl.cpp"

// TODO:    WX_DEFINE_OBJARRAY(wxMsgArray);
#endif // wxUSE_THREADS

#if wxUSE_WX_RESOURCES
    #include "wx/resource.h"
#endif

#include <string.h>
#include <ctype.h>

// ---------------------------------------------------------------------------
// global variables
// ---------------------------------------------------------------------------

extern wxChar *wxBuffer;
extern wxChar *wxOsVersion;
extern wxList *wxWinHandleList;
extern wxList WXDLLEXPORT wxPendingDelete;
#if wxUSE_THREADS
extern wxList *wxPendingEvents;
extern wxCriticalSection *wxPendingEventsLocker;
#endif
extern void wxSetKeyboardHook(bool doIt);
extern wxCursor *g_globalCursor;

HINSTANCE wxhInstance = 0;
// TODO: MSG s_currentMsg;
wxApp *wxTheApp = NULL;

// FIXME why not const? and not static?

// NB: all "NoRedraw" classes must have the same names as the "normal" classes
//     with NR suffix - wxWindow::MSWCreate() supposes this
wxChar wxFrameClassName[]         = wxT("wxFrameClass");
wxChar wxFrameClassNameNoRedraw[] = wxT("wxFrameClassNR");
wxChar wxMDIFrameClassName[]      = wxT("wxMDIFrameClass");
wxChar wxMDIFrameClassNameNoRedraw[] = wxT("wxMDIFrameClassNR");
wxChar wxMDIChildFrameClassName[] = wxT("wxMDIChildFrameClass");
wxChar wxMDIChildFrameClassNameNoRedraw[] = wxT("wxMDIChildFrameClassNR");
wxChar wxPanelClassName[]         = wxT("wxPanelClass");
wxChar wxCanvasClassName[]        = wxT("wxCanvasClass");

HICON wxSTD_FRAME_ICON = (HICON) NULL;
HICON wxSTD_MDICHILDFRAME_ICON = (HICON) NULL;
HICON wxSTD_MDIPARENTFRAME_ICON = (HICON) NULL;

HICON wxDEFAULT_FRAME_ICON = (HICON) NULL;
HICON wxDEFAULT_MDICHILDFRAME_ICON = (HICON) NULL;
HICON wxDEFAULT_MDIPARENTFRAME_ICON = (HICON) NULL;

HBRUSH wxDisableButtonBrush = (HBRUSH) 0;

MRESULT wxWndProc(HWND, UINT, MPARAM, MPARAM);

// ===========================================================================
// implementation
// ===========================================================================

// ---------------------------------------------------------------------------
// wxApp
// ---------------------------------------------------------------------------

#if !USE_SHARED_LIBRARY
    IMPLEMENT_DYNAMIC_CLASS(wxApp, wxEvtHandler)

    BEGIN_EVENT_TABLE(wxApp, wxEvtHandler)
        EVT_IDLE(wxApp::OnIdle)
        EVT_END_SESSION(wxApp::OnEndSession)
        EVT_QUERY_END_SESSION(wxApp::OnQueryEndSession)
    END_EVENT_TABLE()
#endif

//// Initialize
bool wxApp::Initialize()
{
    // Some people may wish to use this, but
    // probably it shouldn't be here by default.
#ifdef __WXDEBUG__
    //    wxRedirectIOToConsole();
#endif

    wxBuffer = new wxChar[1500]; // FIXME

    wxClassInfo::InitializeClasses();

#if wxUSE_RESOURCES
    wxGetResource(wxT("wxWindows"), wxT("OsVersion"), &wxOsVersion);
#endif

    // I'm annoyed ... I don't know where to put this and I don't want to
    // create a module for that as it's part of the core.
#if wxUSE_THREADS
    wxPendingEvents = new wxList();
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

    g_globalCursor = new wxCursor;

// TODO:
/*
    wxSTD_FRAME_ICON = LoadIcon(wxhInstance, wxT("wxSTD_FRAME"));
    wxSTD_MDIPARENTFRAME_ICON = LoadIcon(wxhInstance, wxT("wxSTD_MDIPARENTFRAME"));
    wxSTD_MDICHILDFRAME_ICON = LoadIcon(wxhInstance, wxT("wxSTD_MDICHILDFRAME"));

    wxDEFAULT_FRAME_ICON = LoadIcon(wxhInstance, wxT("wxDEFAULT_FRAME"));
    wxDEFAULT_MDIPARENTFRAME_ICON = LoadIcon(wxhInstance, wxT("wxDEFAULT_MDIPARENTFRAME"));
    wxDEFAULT_MDICHILDFRAME_ICON = LoadIcon(wxhInstance, wxT("wxDEFAULT_MDICHILDFRAME"));
*/
    RegisterWindowClasses();

    // Create the brush for disabling bitmap buttons
// TODO:
/*
    LOGBRUSH lb;
    lb.lbStyle = BS_PATTERN;
    lb.lbHatch = (int)LoadBitmap( wxhInstance, wxT("wxDISABLE_BUTTON_BITMAP") );
    if ( lb.lbHatch )
    {
        wxDisableButtonBrush = ::CreateBrushIndirect( & lb );
        ::DeleteObject( (HGDIOBJ)lb.lbHatch );
    }
 */
    //else: wxWindows resources are probably not linked in

    wxWinHandleList = new wxList(wxKEY_INTEGER);

    // This is to foil optimizations in Visual C++ that throw out dummy.obj.
    // PLEASE DO NOT ALTER THIS.
#if !defined(WXMAKINGDLL)
    extern char wxDummyChar;
    if (wxDummyChar) wxDummyChar++;
#endif

    wxSetKeyboardHook(TRUE);

    wxModule::RegisterModules();
    if (!wxModule::InitializeModules())
        return FALSE;
    return TRUE;
}

// ---------------------------------------------------------------------------
// RegisterWindowClasses
// ---------------------------------------------------------------------------

// TODO we should only register classes really used by the app. For this it
//      would be enough to just delay the class registration until an attempt
//      to create a window of this class is made.
bool wxApp::RegisterWindowClasses()
{
// TODO:
/*
    WNDCLASS wndclass;

    // for each class we register one with CS_(V|H)REDRAW style and one
    // without for windows created with wxNO_FULL_REDRAW_ON_REPAINT flag
    static const long styleNormal = 0; // TODO: CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
    static const long styleNoRedraw = 0; // TODO: CS_DBLCLKS;

    // the fields which are common to all classes
    wndclass.lpfnWndProc   = (WNDPROC)wxWndProc;
    wndclass.cbClsExtra    = 0;
    wndclass.cbWndExtra    = sizeof( DWORD ); // VZ: what is this DWORD used for?
    wndclass.hInstance     = wxhInstance;
    wndclass.hIcon         = (HICON) NULL;
    wndclass.hCursor       = 0; // TODO: ::LoadCursor((HINSTANCE)NULL, IDC_ARROW);
    wndclass.lpszMenuName  = NULL;

    // Register the frame window class.
    wndclass.hbrBackground = (HBRUSH)(COLOR_APPWORKSPACE + 1);
    wndclass.lpszClassName = wxFrameClassName;
    wndclass.style         = styleNormal;

    if ( !RegisterClass(&wndclass) )
    {
        wxLogLastError("RegisterClass(frame)");

        return FALSE;
    }

    // "no redraw" frame
    wndclass.lpszClassName = wxFrameClassNameNoRedraw;
    wndclass.style         = styleNoRedraw;

    if ( !RegisterClass(&wndclass) )
    {
        wxLogLastError("RegisterClass(no redraw frame)");

        return FALSE;
    }

    // Register the MDI frame window class.
    wndclass.hbrBackground = (HBRUSH)NULL; // paint MDI frame ourselves
    wndclass.lpszClassName = wxMDIFrameClassName;
    wndclass.style         = styleNormal;

    if ( !RegisterClass(&wndclass) )
    {
        wxLogLastError("RegisterClass(MDI parent)");

        return FALSE;
    }

    // "no redraw" MDI frame
    wndclass.lpszClassName = wxMDIFrameClassNameNoRedraw;
    wndclass.style         = styleNoRedraw;

    if ( !RegisterClass(&wndclass) )
    {
        wxLogLastError("RegisterClass(no redraw MDI parent frame)");

        return FALSE;
    }

    // Register the MDI child frame window class.
    wndclass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wndclass.lpszClassName = wxMDIChildFrameClassName;
    wndclass.style         = styleNormal;

    if ( !RegisterClass(&wndclass) )
    {
        wxLogLastError("RegisterClass(MDI child)");

        return FALSE;
    }

    // "no redraw" MDI child frame
    wndclass.lpszClassName = wxMDIChildFrameClassNameNoRedraw;
    wndclass.style         = styleNoRedraw;

    if ( !RegisterClass(&wndclass) )
    {
        wxLogLastError("RegisterClass(no redraw MDI child)");

        return FALSE;
    }

    // Register the panel window class.
    wndclass.hbrBackground = (HBRUSH) GetStockObject( LTGRAY_BRUSH );
    wndclass.lpszClassName = wxPanelClassName;
    wndclass.style         = styleNormal;

    if ( !RegisterClass(&wndclass) )
    {
        wxLogLastError("RegisterClass(panel)");

        return FALSE;
    }

    // Register the canvas and textsubwindow class name
    wndclass.hbrBackground = (HBRUSH)NULL;
    wndclass.lpszClassName = wxCanvasClassName;

    if ( !RegisterClass(&wndclass) )
    {
        wxLogLastError("RegisterClass(canvas)");

        return FALSE;
    }
*/
    return TRUE;
}

// ---------------------------------------------------------------------------
// Convert Windows to argc, argv style
// ---------------------------------------------------------------------------

void wxApp::ConvertToStandardCommandArgs(char* lpCmdLine)
{
    wxStringList args;

    wxString cmdLine(lpCmdLine);
    int count = 0;

    // Get application name
    wxChar name[260]; // 260 is MAX_PATH value from windef.h
// TODO:     ::GetModuleFileName(wxhInstance, name, WXSIZEOF(name));

    args.Add(name);
    count++;

    wxStrcpy(name, wxFileNameFromPath(name));
    wxStripExtension(name);
    wxTheApp->SetAppName(name);

    // Break up string
    // Treat strings enclosed in double-quotes as single arguments
    int i = 0;
    int len = cmdLine.Length();
    while (i < len)
    {
        // Skip whitespace
        while ((i < len) && wxIsspace(cmdLine.GetChar(i)))
            i ++;

        if (i < len)
        {
            if (cmdLine.GetChar(i) == wxT('"')) // We found the start of a string
            {
                i ++;
                int first = i;
                while ((i < len) && (cmdLine.GetChar(i) != wxT('"')))
                    i ++;

                wxString arg(cmdLine.Mid(first, (i - first)));

                args.Add(arg);
                count ++;

                if (i < len)
                    i ++; // Skip past 2nd quote
            }
            else // Unquoted argument
            {
                int first = i;
                while ((i < len) && !wxIsspace(cmdLine.GetChar(i)))
                    i ++;

                wxString arg(cmdLine.Mid(first, (i - first)));

                args.Add(arg);
                count ++;
            }
        }
    }

    wxTheApp->argv = new wxChar*[count + 1];
    for (i = 0; i < count; i++)
    {
        wxString arg(args[i]);
        wxTheApp->argv[i] = copystring((const wxChar*)arg);
    }
    wxTheApp->argv[count] = NULL; // argv[] is a NULL-terminated list
    wxTheApp->argc = count;
}

//// Cleans up any wxWindows internal structures left lying around

void wxApp::CleanUp()
{
    //// COMMON CLEANUP

#if wxUSE_LOG
    // flush the logged messages if any and install a 'safer' log target: the
    // default one (wxLogGui) can't be used after the resources are freed just
    // below and the user suppliedo ne might be even more unsafe (using any
    // wxWindows GUI function is unsafe starting from now)
    wxLog::DontCreateOnDemand();

    // this will flush the old messages if any
    delete wxLog::SetActiveTarget(new wxLogStderr);
#endif // wxUSE_LOG

    // One last chance for pending objects to be cleaned up
    wxTheApp->DeletePendingObjects();

    wxModule::CleanUpModules();

#if wxUSE_WX_RESOURCES
    wxCleanUpResourceSystem();

    //  wxDefaultResourceTable->ClearTable();
#endif

    // Indicate that the cursor can be freed, so that cursor won't be deleted
    // by deleting the bitmap list before g_globalCursor goes out of scope
    // (double deletion of the cursor).
    wxSetCursor(wxNullCursor);
    delete g_globalCursor;
    g_globalCursor = NULL;

    wxDeleteStockObjects();

    // Destroy all GDI lists, etc.
    wxDeleteStockLists();

    delete wxTheColourDatabase;
    wxTheColourDatabase = NULL;

    wxBitmap::CleanUpHandlers();

    delete[] wxBuffer;
    wxBuffer = NULL;

    //// WINDOWS-SPECIFIC CLEANUP

    wxSetKeyboardHook(FALSE);
// TODO:
/*
    if (wxSTD_FRAME_ICON)
        DestroyIcon(wxSTD_FRAME_ICON);
    if (wxSTD_MDICHILDFRAME_ICON)
        DestroyIcon(wxSTD_MDICHILDFRAME_ICON);
    if (wxSTD_MDIPARENTFRAME_ICON)
        DestroyIcon(wxSTD_MDIPARENTFRAME_ICON);

    if (wxDEFAULT_FRAME_ICON)
        DestroyIcon(wxDEFAULT_FRAME_ICON);
    if (wxDEFAULT_MDICHILDFRAME_ICON)
        DestroyIcon(wxDEFAULT_MDICHILDFRAME_ICON);
    if (wxDEFAULT_MDIPARENTFRAME_ICON)
        DestroyIcon(wxDEFAULT_MDIPARENTFRAME_ICON);
*/
    if ( wxDisableButtonBrush )
    {
// TODO:        ::DeleteObject( wxDisableButtonBrush );
    }

    if (wxWinHandleList)
        delete wxWinHandleList;

    // GL: I'm annoyed ... I don't know where to put this and I don't want to
    // create a module for that as it's part of the core.
#if wxUSE_THREADS
    delete wxPendingEvents;
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

#if wxUSE_LOG
    // do it as the very last thing because everything else can log messages
    delete wxLog::SetActiveTarget(NULL);
#endif // wxUSE_LOG
}

#if !defined(_WINDLL) || (defined(_WINDLL) && defined(WXMAKINGDLL))

//// Main wxWindows entry point
int wxEntry(WXHINSTANCE hInstance,
            WXHINSTANCE WXUNUSED(hPrevInstance),
            char *lpCmdLine,
            int nCmdShow,
            bool enterLoop)
{
    // do check for memory leaks on program exit
    // (another useful flag is _CRTDBG_DELAY_FREE_MEM_DF which doesn't free
    //  deallocated memory which may be used to simulate low-memory condition)
// TODO:    wxCrtSetDbgFlag(_CRTDBG_LEAK_CHECK_DF);

    // take everything into a try-except block in release build
    // FIXME other compilers must support Win32 SEH (structured exception
    //       handling) too, just find the appropriate keyword in their docs!
    //       Please note that it's _not_ the same as C++ exceptions!

        wxhInstance = (HINSTANCE) hInstance;

        if (!wxApp::Initialize())
            return 0;

        // create the application object or ensure that one already exists
        if (!wxTheApp)
        {
            // The app may have declared a global application object, but we recommend
            // the IMPLEMENT_APP macro is used instead, which sets an initializer
            // function for delayed, dynamic app object construction.
            wxCHECK_MSG( wxApp::GetInitializerFunction(), 0,
                         wxT("No initializer - use IMPLEMENT_APP macro.") );

            wxTheApp = (wxApp*)(*wxApp::GetInitializerFunction()) ();
        }

        wxCHECK_MSG( wxTheApp, 0, wxT("You have to define an instance of wxApp!") );

        // save the WinMain() parameters
        wxTheApp->ConvertToStandardCommandArgs(lpCmdLine);
        wxTheApp->m_nCmdShow = nCmdShow;

        // GUI-specific initialisation. In fact on Windows we don't have any,
        // but this call is provided for compatibility across platforms.
        wxTheApp->OnInitGui();

        int retValue = 0;

        if ( wxTheApp->OnInit() )
        {
            if ( enterLoop )
            {
                retValue = wxTheApp->OnRun();
            }
            else
                // We want to initialize, but not run or exit immediately.
                return 1;
        }
        //else: app initialization failed, so we skipped OnRun()

        wxWindow *topWindow = wxTheApp->GetTopWindow();
        if ( topWindow )
        {
            // Forcibly delete the window.
            if ( topWindow->IsKindOf(CLASSINFO(wxFrame)) ||
                    topWindow->IsKindOf(CLASSINFO(wxDialog)) )
            {
                topWindow->Close(TRUE);
                wxTheApp->DeletePendingObjects();
            }
            else
            {
                delete topWindow;
                wxTheApp->SetTopWindow(NULL);
            }
        }

        wxTheApp->OnExit();

        wxApp::CleanUp();

        return retValue;
}

#else /*  _WINDLL  */

//// Entry point for DLLs

int wxEntry(WXHINSTANCE hInstance)
{
    wxhInstance = (HINSTANCE) hInstance;
    wxApp::Initialize();

    // The app may have declared a global application object, but we recommend
    // the IMPLEMENT_APP macro is used instead, which sets an initializer function
    // for delayed, dynamic app object construction.
    if (!wxTheApp)
    {
        wxCHECK_MSG( wxApp::GetInitializerFunction(), 0,
                     "No initializer - use IMPLEMENT_APP macro." );

        wxTheApp = (* wxApp::GetInitializerFunction()) ();
    }

    wxCHECK_MSG( wxTheApp, 0, "You have to define an instance of wxApp!" );

    wxTheApp->argc = 0;
    wxTheApp->argv = NULL;

    wxTheApp->OnInitGui();

    wxTheApp->OnInit();

    wxWindow *topWindow = wxTheApp->GetTopWindow();
    if ( topWindow && topWindow->GetHWND())
    {
        topWindow->Show(TRUE);
    }

    return 1;
}
#endif // _WINDLL

//// Static member initialization

wxAppInitializerFunction wxAppBase::m_appInitFn = (wxAppInitializerFunction) NULL;

wxApp::wxApp()
{
    m_topWindow = NULL;
    wxTheApp = this;
    m_wantDebugOutput = TRUE;

    argc = 0;
    argv = NULL;
    m_printMode = wxPRINT_WINDOWS;
    m_exitOnFrameDelete = TRUE;
    m_auto3D = TRUE;
}

wxApp::~wxApp()
{
    // Delete command-line args
    int i;
    for (i = 0; i < argc; i++)
    {
        delete[] argv[i];
    }
    delete[] argv;
}

bool wxApp::Initialized()
{
#ifndef _WINDLL
    if (GetTopWindow())
        return TRUE;
    else
        return FALSE;
#endif
#ifdef _WINDLL // Assume initialized if DLL (no way of telling)
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
    BOOL rc = FALSE; // TODO: ::GetMessage(&s_currentMsg, (HWND) NULL, 0, 0);
    if ( rc == 0 )
    {
        // got WM_QUIT
        m_keepGoing = FALSE;

        return FALSE;
    }
    else if ( rc == -1 )
    {
        // should never happen, but let's test for it nevertheless
        wxLogLastError("GetMessage");
    }
    else
    {
#if wxUSE_THREADS
        wxASSERT_MSG( wxThread::IsMain(),
                      wxT("only the main thread can process Windows messages") );

        static bool s_hadGuiLock = TRUE;
// TODO:
/*
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

                size_t count = s_aSavedMessages.Count();
                for ( size_t n = 0; n < count; n++ )
                {
                    MSG& msg = s_aSavedMessages[n];

                    if ( !ProcessMessage((WXMSG *)&msg) )
                    {
                        ::TranslateMessage(&msg);
                        ::DispatchMessage(&msg);
                    }
                }
                s_aSavedMessages.Empty();
            }
        }
*/
#endif // wxUSE_THREADS

        // Process the message
// TODO:
/*
        if ( !ProcessMessage((WXMSG *)&s_currentMsg) )
        {
            ::TranslateMessage(&s_currentMsg);
            ::DispatchMessage(&s_currentMsg);
        }
*/
    }

    return TRUE;
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
// TODO:
/*
        while ( !::PeekMessage(&s_currentMsg, 0, 0, 0, PM_NOREMOVE) &&
                ProcessIdle() )
        {
        }
*/

        DoMessage();
    }

// TODO:    return s_currentMsg.wParam;
    return FALSE;
}

// Returns TRUE if more time is needed.
bool wxApp::ProcessIdle()
{
    wxIdleEvent event;
    event.SetEventObject(this);
    ProcessEvent(event);

    return event.MoreRequested();
}

#if wxUSE_THREADS
void wxApp::ProcessPendingEvents()
{
    wxNode *node = wxPendingEvents->First();
    wxCriticalSectionLocker locker(*wxPendingEventsLocker);

    while (node)
    {
        wxEvtHandler *handler = (wxEvtHandler *)node->Data();

        handler->ProcessPendingEvents();

        delete node;
        node = wxPendingEvents->First();
    }
}
#endif


void wxApp::ExitMainLoop()
{
    m_keepGoing = FALSE;
}

bool wxApp::Pending()
{
// TODO:    return (::PeekMessage(&s_currentMsg, 0, 0, 0, PM_NOREMOVE) != 0);
    return FALSE;
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
// TODO:
/*
    MSG *msg = (MSG *)wxmsg;
    HWND hWnd = msg->hwnd;
    wxWindow *wndThis = wxFindWinFromHandle((WXHWND)hWnd), *wnd;

    // for some composite controls (like a combobox), wndThis might be NULL
    // because the subcontrol is not a wxWindow, but only the control itself
    // is - try to catch this case
    while ( hWnd && !wndThis )
    {
        hWnd = 0; // TODO: ::GetParent(hWnd);
        wndThis = wxFindWinFromHandle((WXHWND)hWnd);
    }

    // Try translations first; find the youngest window with
    // a translation table.
    for ( wnd = wndThis; wnd; wnd = wnd->GetParent() )
    {
        if ( wnd->OS2TranslateMessage(wxmsg) )
            return TRUE;
    }

    // Anyone for a non-translation message? Try youngest descendants first.
    for ( wnd = wndThis; wnd; wnd = wnd->GetParent() )
    {
        if ( wnd->OS2ProcessMessage(wxmsg) )
            return TRUE;
    }
*/
    return FALSE;
}

void wxApp::OnIdle(wxIdleEvent& event)
{
    static bool s_inOnIdle = FALSE;

    // Avoid recursion (via ProcessEvent default case)
    if ( s_inOnIdle )
        return;

    s_inOnIdle = TRUE;

    // 'Garbage' collection of windows deleted with Close().
    DeletePendingObjects();

#if wxUSE_LOG
    // flush the logged messages if any
    wxLog *pLog = wxLog::GetActiveTarget();
    if ( pLog != NULL && pLog->HasPendingMessages() )
        pLog->Flush();
#endif // wxUSE_LOG

    // Send OnIdle events to all windows
    if ( SendIdleEvents() )
    {
        // SendIdleEvents() returns TRUE if at least one window requested more
        // idle events
        event.RequestMore(TRUE);
    }

    // If they are pending events, we must process them.
#if wxUSE_THREADS
    ProcessPendingEvents();
#endif
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
    bool needMore = FALSE;

    wxIdleEvent event;
    event.SetEventObject(win);
    win->GetEventHandler()->ProcessEvent(event);

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
    return needMore;
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

int wxApp::GetComCtl32Version() const
{
   // TODO:  Does OS/2 even need this method?
   /*
    // have we loaded COMCTL32 yet?
    HMODULE theModule = ::GetModuleHandle(wxT("COMCTL32"));
    int version = 0;

    // if so, then we can check for the version
    if (theModule)
    {
        // InitCommonControlsEx is unique to 4.7 and later
        FARPROC theProc = ::GetProcAddress(theModule, "InitCommonControlsEx");

        if (! theProc)
        {                    // not found, must be 4.00
            version = 400;
        }
        else
        {
            // The following symbol are unique to 4.71
            //   DllInstall
            //   FlatSB_EnableScrollBar FlatSB_GetScrollInfo FlatSB_GetScrollPos
            //   FlatSB_GetScrollProp FlatSB_GetScrollRange FlatSB_SetScrollInfo
            //   FlatSB_SetScrollPos FlatSB_SetScrollProp FlatSB_SetScrollRange
            //   FlatSB_ShowScrollBar
            //   _DrawIndirectImageList _DuplicateImageList
            //   InitializeFlatSB
            //   UninitializeFlatSB
            // we could check for any of these - I chose DllInstall
            FARPROC theProc = ::GetProcAddress(theModule, "DllInstall");
            if (! theProc)
            {
                // not found, must be 4.70
                version = 470;
            }
            else
            {                         // found, must be 4.71
                version = 471;
            }
        }
    }
    return version;
*/
    return 0;
}

void wxExit()
{
    wxLogError(_("Fatal error: exiting"));

    wxApp::CleanUp();
}

// Yield to incoming messages
bool wxYield()
{
    HAB                             vHab;
    QMSG                            vMsg;
    // We want to go back to the main message loop
    // if we see a WM_QUIT. (?)
    while (::WinPeekMsg(vHab, &vMsg, (HWND)NULL, 0, 0, PM_NOREMOVE) && vMsg.msg != WM_QUIT)
    {
        if (!wxTheApp->DoMessage())
            break;
    }
    // If they are pending events, we must process them.
#if wxUSE_THREADS
    wxTheApp->ProcessPendingEvents();
#endif
    return TRUE;
}

wxIcon wxApp::GetStdIcon(int which) const
{
    // TODO:
    /*
    switch(which)
    {
        case wxICON_INFORMATION:
            return wxIcon("wxICON_INFO");

        case wxICON_QUESTION:
            return wxIcon("wxICON_QUESTION");

        case wxICON_EXCLAMATION:
            return wxIcon("wxICON_WARNING");

        default:
            wxFAIL_MSG(wxT("requested non existent standard icon"));
            // still fall through

        case wxICON_HAND:
            return wxIcon("wxICON_ERROR");
    }
*/
    return wxIcon("wxICON_ERROR");
}


HINSTANCE wxGetInstance()
{
    return wxhInstance;
}

void wxSetInstance(HINSTANCE hInst)
{
    wxhInstance = hInst;
}


