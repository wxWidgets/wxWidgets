/////////////////////////////////////////////////////////////////////////////
// Name:        app.cpp
// Purpose:     wxApp
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:     wxWindows license
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
#endif

#include "wx/log.h"
#include "wx/module.h"

#include "wx/msw/private.h"

#if wxUSE_THREADS
    #include "wx/thread.h"

    // define the array of MSG strutures
    WX_DECLARE_OBJARRAY(MSG, wxMsgArray);

    #include "wx/arrimpl.cpp"

    WX_DEFINE_OBJARRAY(wxMsgArray);
#endif // wxUSE_THREADS

#if wxUSE_WX_RESOURCES
    #include "wx/resource.h"
#endif

// OLE is used for drag-and-drop, clipboard, OLE Automation...
#ifndef wxUSE_NORLANDER_HEADERS
#if defined(__GNUWIN32__) || defined(__SC__) || defined(__SALFORDC__)
    #undef wxUSE_OLE

    #define  wxUSE_OLE 0
#endif // broken compilers
#endif

#if wxUSE_OLE
    #include <ole2.h>
#endif

#include <string.h>
#include <ctype.h>

#if (defined(__WIN95__) && !defined(__GNUWIN32__)) || defined(__TWIN32__) || defined(wxUSE_NORLANDER_HEADERS)
    #include <commctrl.h>
#endif

#include "wx/msw/msvcrt.h"

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

#if _WIN32_IE >= 0x0300
    #include <shlwapi.h>
#endif

// ---------------------------------------------------------------------------
// global variables
// ---------------------------------------------------------------------------

extern wxChar *wxBuffer;
extern wxChar *wxOsVersion;
extern wxList *wxWinHandleList;
extern wxList WXDLLEXPORT wxPendingDelete;
extern void wxSetKeyboardHook(bool doIt);
extern wxCursor *g_globalCursor;

MSG s_currentMsg;
wxApp *wxTheApp = NULL;

// NB: all "NoRedraw" classes must have the same names as the "normal" classes
//     with NR suffix - wxWindow::MSWCreate() supposes this
const wxChar *wxFrameClassName         = wxT("wxFrameClass");
const wxChar *wxFrameClassNameNoRedraw = wxT("wxFrameClassNR");
const wxChar *wxMDIFrameClassName      = wxT("wxMDIFrameClass");
const wxChar *wxMDIFrameClassNameNoRedraw = wxT("wxMDIFrameClassNR");
const wxChar *wxMDIChildFrameClassName = wxT("wxMDIChildFrameClass");
const wxChar *wxMDIChildFrameClassNameNoRedraw = wxT("wxMDIChildFrameClassNR");
const wxChar *wxPanelClassName         = wxT("wxPanelClass");
const wxChar *wxCanvasClassName        = wxT("wxCanvasClass");

HICON wxSTD_FRAME_ICON = (HICON) NULL;
HICON wxSTD_MDICHILDFRAME_ICON = (HICON) NULL;
HICON wxSTD_MDIPARENTFRAME_ICON = (HICON) NULL;

HICON wxDEFAULT_FRAME_ICON = (HICON) NULL;
HICON wxDEFAULT_MDICHILDFRAME_ICON = (HICON) NULL;
HICON wxDEFAULT_MDIPARENTFRAME_ICON = (HICON) NULL;

HBRUSH wxDisableButtonBrush = (HBRUSH) 0;

LRESULT WXDLLEXPORT APIENTRY wxWndProc(HWND, UINT, WPARAM, LPARAM);

#if wxUSE_RICHEDIT
    // the handle to richedit DLL and the version of the DLL loaded
    static HINSTANCE gs_hRichEdit = (HINSTANCE)NULL;
    static int gs_verRichEdit = -1;
#endif

// ===========================================================================
// implementation
// ===========================================================================

// ---------------------------------------------------------------------------
// wxApp
// ---------------------------------------------------------------------------

    IMPLEMENT_DYNAMIC_CLASS(wxApp, wxEvtHandler)

    BEGIN_EVENT_TABLE(wxApp, wxEvtHandler)
        EVT_IDLE(wxApp::OnIdle)
        EVT_END_SESSION(wxApp::OnEndSession)
        EVT_QUERY_END_SESSION(wxApp::OnQueryEndSession)
    END_EVENT_TABLE()

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

#if defined(__WIN95__)
    InitCommonControls();

#endif // __WIN95__

#if wxUSE_OLE

#ifdef __WIN16__
    // for OLE, enlarge message queue to be as large as possible
    int iMsg = 96;
    while (!SetMessageQueue(iMsg) && (iMsg -= 8))
        ;
#endif // Win16
    // we need to initialize OLE library
    if ( FAILED(::OleInitialize(NULL)) )
        wxLogError(_("Cannot initialize OLE"));
#endif // wxUSE_OLE

#if wxUSE_CTL3D
    if (!Ctl3dRegister(wxhInstance))
        wxLogError(wxT("Cannot register CTL3D"));

    Ctl3dAutoSubclass(wxhInstance);
#endif

    g_globalCursor = new wxCursor;

    // VZ: these icons are not in wx.rc anyhow (but should they?)!
#if 0
    wxSTD_FRAME_ICON = LoadIcon(wxhInstance, wxT("wxSTD_FRAME"));
    wxSTD_MDIPARENTFRAME_ICON = LoadIcon(wxhInstance, wxT("wxSTD_MDIPARENTFRAME"));
    wxSTD_MDICHILDFRAME_ICON = LoadIcon(wxhInstance, wxT("wxSTD_MDICHILDFRAME"));

    wxDEFAULT_FRAME_ICON = LoadIcon(wxhInstance, wxT("wxDEFAULT_FRAME"));
    wxDEFAULT_MDIPARENTFRAME_ICON = LoadIcon(wxhInstance, wxT("wxDEFAULT_MDIPARENTFRAME"));
    wxDEFAULT_MDICHILDFRAME_ICON = LoadIcon(wxhInstance, wxT("wxDEFAULT_MDICHILDFRAME"));
#endif // 0

    RegisterWindowClasses();

    // Create the brush for disabling bitmap buttons

    LOGBRUSH lb;
    lb.lbStyle = BS_PATTERN;
    lb.lbHatch = (int)LoadBitmap( wxhInstance, wxT("wxDISABLE_BUTTON_BITMAP") );
    if ( lb.lbHatch )
    {
        wxDisableButtonBrush = ::CreateBrushIndirect( & lb );
        ::DeleteObject( (HGDIOBJ)lb.lbHatch );
    }
    //else: wxWindows resources are probably not linked in

#if wxUSE_PENWINDOWS
    wxRegisterPenWin();
#endif

    wxWinHandleList = new wxList(wxKEY_INTEGER);

    // This is to foil optimizations in Visual C++ that throw out dummy.obj.
    // PLEASE DO NOT ALTER THIS.
#if defined(__VISUALC__) && !defined(WXMAKINGDLL)
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
    WNDCLASS wndclass;

    // for each class we register one with CS_(V|H)REDRAW style and one
    // without for windows created with wxNO_FULL_REDRAW_ON_REPAINT flag
    static const long styleNormal = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
    static const long styleNoRedraw = CS_DBLCLKS;

    // the fields which are common to all classes
    wndclass.lpfnWndProc   = (WNDPROC)wxWndProc;
    wndclass.cbClsExtra    = 0;
    wndclass.cbWndExtra    = sizeof( DWORD ); // VZ: what is this DWORD used for?
    wndclass.hInstance     = wxhInstance;
    wndclass.hIcon         = (HICON) NULL;
    wndclass.hCursor       = ::LoadCursor((HINSTANCE)NULL, IDC_ARROW);
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
    ::GetModuleFileName(wxhInstance, name, WXSIZEOF(name));

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

#if wxUSE_RICHEDIT
    if (gs_hRichEdit != (HINSTANCE) NULL)
        FreeLibrary(gs_hRichEdit);
#endif

#if wxUSE_PENWINDOWS
    wxCleanUpPenWin();
#endif

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

    if ( wxDisableButtonBrush )
        ::DeleteObject( wxDisableButtonBrush );

#if wxUSE_OLE
    ::OleUninitialize();
#endif

#if wxUSE_CTL3D
    Ctl3dUnregister(wxhInstance);
#endif

    if (wxWinHandleList)
        delete wxWinHandleList;

    // GL: I'm annoyed ... I don't know where to put this and I don't want to
    // create a module for that as it's part of the core.
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

#if wxUSE_LOG
    // do it as the very last thing because everything else can log messages
    delete wxLog::SetActiveTarget(NULL);
#endif // wxUSE_LOG
}

#if !defined(_WINDLL) || (defined(_WINDLL) && defined(WXMAKINGDLL))

// temporarily disable this warning which would be generated in release builds
// because of __try
#ifdef __VISUALC__
    #pragma warning(disable: 4715) // not all control paths return a value
#endif // Visual C++

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
    wxCrtSetDbgFlag(_CRTDBG_LEAK_CHECK_DF);
#ifdef __MWERKS__
#if (defined(__WXDEBUG__) && wxUSE_MEMORY_TRACING) || wxUSE_DEBUG_CONTEXT
    // This seems to be necessary since there are 'rogue'
    // objects present at this point (perhaps global objects?)
    // Setting a checkpoint will ignore them as far as the
    // memory checking facility is concerned.
    // Of course you may argue that memory allocated in globals should be
    // checked, but this is a reasonable compromise.
    wxDebugContext::SetCheckpoint();
#endif
#endif
    // take everything into a try-except block in release build
    // FIXME other compilers must support Win32 SEH (structured exception
    //       handling) too, just find the appropriate keyword in their docs!
    //       Please note that it's _not_ the same as C++ exceptions!
#if !defined(__WXDEBUG__) && defined(__VISUALC__)
    #define CATCH_PROGRAM_EXCEPTIONS

    __try {
#else
    #undef  CATCH_PROGRAM_EXCEPTIONS
#endif
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

            wxTheApp = (*wxApp::GetInitializerFunction()) ();
        }

        wxCHECK_MSG( wxTheApp, 0, wxT("You have to define an instance of wxApp!") );

        // save the WinMain() parameters
        wxTheApp->ConvertToStandardCommandArgs(lpCmdLine);
        wxTheApp->m_nCmdShow = nCmdShow;

        // GUI-specific initialisation. In fact on Windows we don't have any,
        // but this call is provided for compatibility across platforms.
        wxTheApp->OnInitGui();

        // We really don't want timestamps by default, because it means
        // we can't simply double-click on the error message and get to that
        // line in the source. So VC++ at least, let's have a sensible default.
#ifdef __VISUALC__
        wxLog::SetTimestamp(NULL);
#endif

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

#ifdef CATCH_PROGRAM_EXCEPTIONS
    }
    __except ( EXCEPTION_EXECUTE_HANDLER ) {
        /*
           if ( wxTheApp )
           wxTheApp->OnFatalException();
         */

        // using wxLog would be unsafe here
        ::MessageBox(NULL,
                     _("Unrecoverable program error detected: "
                     " the application will terminate."),
                     _("Fatal Error"),
                     MB_APPLMODAL | MB_ICONSTOP | MB_OK);

        ::ExitProcess(3); // the same exit code as abort()

        // NOTREACHED
    }
#endif // CATCH_PROGRAM_EXCEPTIONS
}

// restore warning state
#ifdef __VISUALC__
    #pragma warning(default: 4715) // not all control paths return a value
#endif // Visual C++

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
        wxLogLastError("GetMessage");
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
#endif // wxUSE_THREADS

        // Process the message
        if ( !ProcessMessage((WXMSG *)&s_currentMsg) )
        {
            ::TranslateMessage(&s_currentMsg);
            ::DispatchMessage(&s_currentMsg);
        }
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

        while ( !::PeekMessage(&s_currentMsg, 0, 0, 0, PM_NOREMOVE) &&
                ProcessIdle() )
        {
        }


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

    return event.MoreRequested();
}

void wxApp::ExitMainLoop()
{
    m_keepGoing = FALSE;
}

bool wxApp::Pending()
{
    return (::PeekMessage(&s_currentMsg, 0, 0, 0, PM_NOREMOVE) != 0);
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
    HWND hWnd = msg->hwnd;
    wxWindow *wndThis = wxFindWinFromHandle((WXHWND)hWnd), *wnd;

    // for some composite controls (like a combobox), wndThis might be NULL
    // because the subcontrol is not a wxWindow, but only the control itself
    // is - try to catch this case
    while ( hWnd && !wndThis )
    {
        hWnd = ::GetParent(hWnd);
        wndThis = wxFindWinFromHandle((WXHWND)hWnd);
    }

    // Try translations first; find the youngest window with
    // a translation table.
    for ( wnd = wndThis; wnd; wnd = wnd->GetParent() )
    {
        if ( wnd->MSWTranslateMessage(wxmsg) )
            return TRUE;
    }

    // Anyone for a non-translation message? Try youngest descendants first.
    for ( wnd = wndThis; wnd; wnd = wnd->GetParent() )
    {
        if ( wnd->MSWProcessMessage(wxmsg) )
            return TRUE;
    }

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

    // If they are pending events, we must process them: pending events are
    // either events to the threads other than main or events posted with
    // wxPostEvent() functions
    ProcessPendingEvents();

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

#if wxUSE_RICHEDIT

/* static */
bool wxApp::InitRichEdit(int version)
{
    wxCHECK_MSG( version >= 1 && version <= 3, FALSE,
                 _T("incorrect richedit control version requested") );

    if ( version <= gs_verRichEdit )
    {
        // we've already got this or better
        return TRUE;
    }

    if ( gs_hRichEdit )
    {
        ::FreeLibrary(gs_hRichEdit);
    }

    // always try load riched20.dll first - like this we won't have to reload
    // it later if we're first asked for RE 1 and then for RE 2 or 3
    wxString dllname = _T("riched20.dll");
    gs_hRichEdit = ::LoadLibrary(dllname);
    if ( !gs_hRichEdit && (version == 1) )
    {
        // fall back to RE 1
        dllname = _T("riched32.dll");
        gs_hRichEdit = ::LoadLibrary(dllname);
    }

    if ( !gs_hRichEdit )
    {
        wxLogSysError(_("Could not load Rich Edit DLL '%s'"), dllname.c_str());

        gs_verRichEdit = -1;

        return FALSE;
    }

    gs_verRichEdit = version;

    return TRUE;
}

#endif // wxUSE_RICHEDIT

/* static */
int wxApp::GetComCtl32Version()
{
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

        // if so, then we can check for the version
        if ( hModuleComCtl32 )
        {
            // try to use DllGetVersion() if available in _headers_
            #ifdef DLLVER_PLATFORM_WINDOWS // defined in shlwapi.h
                DLLGETVERSIONPROC pfnDllGetVersion = (DLLGETVERSIONPROC)
                    ::GetProcAddress(hModuleComCtl32, _T("DllGetVersion"));
                if ( pfnDllGetVersion )
                {
                    DLLVERSIONINFO dvi;
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
            #endif
                // DllGetVersion() unavailable either during compile or
                // run-time, try to guess the version otherwise
                if ( !s_verComCtl32 )
                {
                    // InitCommonControlsEx is unique to 4.70 and later
                    FARPROC theProc = ::GetProcAddress
                                        (
                                         hModuleComCtl32,
                                         _T("InitCommonControlsEx")
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
                                     _T("InitializeFlatSB")
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
    }

    return s_verComCtl32;
}

void wxExit()
{
    wxLogError(_("Fatal error: exiting"));

    wxApp::CleanUp();
}

// Yield to incoming messages
bool wxYield()
{
    // we don't want to process WM_QUIT from here - it should be processed in
    // the main event loop in order to stop it

    MSG msg;
    while ( PeekMessage(&msg, (HWND)0, 0, 0, PM_NOREMOVE) &&
            msg.message != WM_QUIT )
    {
        if ( !wxTheApp->DoMessage() )
            break;
    }

    // If they are pending events, we must process them.
    wxTheApp->ProcessPendingEvents();

    return TRUE;
}

//-----------------------------------------------------------------------------
// wxWakeUpIdle
//-----------------------------------------------------------------------------

void wxWakeUpIdle()
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
            wxLogLastError("PostMessage(WM_NULL)");
        }
    }
}

//-----------------------------------------------------------------------------

wxIcon
wxApp::GetStdIcon(int which) const
{
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
}

// For some reason, with MSVC++ 1.5, WinMain isn't linked in properly
// if in a separate file. So include it here to ensure it's linked.
#if (defined(__VISUALC__) && !defined(__WIN32__)) || (defined(__GNUWIN32__) && !defined(__TWIN32__))
#include "main.cpp"
#endif
