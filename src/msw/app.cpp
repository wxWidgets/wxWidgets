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

#include "wx/cmdline.h"
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

#ifndef __WXMICROWIN__
#include "wx/msw/msvcrt.h"
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

extern wxChar *wxBuffer;
extern wxList WXDLLEXPORT wxPendingDelete;
#ifndef __WXMICROWIN__
extern void wxSetKeyboardHook(bool doIt);
#endif

MSG s_currentMsg;
wxApp *wxTheApp = NULL;

// NB: all "NoRedraw" classes must have the same names as the "normal" classes
//     with NR suffix - wxWindow::MSWCreate() supposes this
const wxChar *wxCanvasClassName        = wxT("wxWindowClass");
const wxChar *wxCanvasClassNameNR      = wxT("wxWindowClassNR");
const wxChar *wxMDIFrameClassName      = wxT("wxMDIFrameClass");
const wxChar *wxMDIFrameClassNameNoRedraw = wxT("wxMDIFrameClassNR");
const wxChar *wxMDIChildFrameClassName = wxT("wxMDIChildFrameClass");
const wxChar *wxMDIChildFrameClassNameNoRedraw = wxT("wxMDIChildFrameClassNR");

HICON wxSTD_FRAME_ICON = (HICON) NULL;
HICON wxSTD_MDICHILDFRAME_ICON = (HICON) NULL;
HICON wxSTD_MDIPARENTFRAME_ICON = (HICON) NULL;

HICON wxDEFAULT_FRAME_ICON = (HICON) NULL;
HICON wxDEFAULT_MDICHILDFRAME_ICON = (HICON) NULL;
HICON wxDEFAULT_MDIPARENTFRAME_ICON = (HICON) NULL;

HBRUSH wxDisableButtonBrush = (HBRUSH) 0;

#ifdef __DIGITALMARS__
extern "C" LRESULT WXDLLEXPORT APIENTRY wxWndProc(HWND, UINT, WPARAM, LPARAM);
#else
LRESULT WXDLLEXPORT APIENTRY wxWndProc(HWND, UINT, WPARAM, LPARAM);
#endif

// FIXME wxUSE_ON_FATAL_EXCEPTION is only supported for VC++ now because it
//       needs compiler support for Win32 SEH. Others (especially Borland)
//       probably have it too, but I'm not sure about how it works
// JACS: get 'Cannot use __try in functions that require unwinding
// in Unicode mode, so disabling.
#if !defined(__VISUALC__) || defined(__WIN16__) || defined(UNICODE)
    #undef wxUSE_ON_FATAL_EXCEPTION
    #define wxUSE_ON_FATAL_EXCEPTION 0
#endif // VC++

#if wxUSE_ON_FATAL_EXCEPTION
    static bool gs_handleExceptions = FALSE;
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

    wxBuffer = new wxChar[1500]; // FIXME

    wxClassInfo::InitializeClasses();

#if wxUSE_THREADS
    wxPendingEventsLocker = new wxCriticalSection;
#endif

    wxTheColourDatabase = new wxColourDatabase(wxKEY_STRING);
    wxTheColourDatabase->Initialize();

    wxInitializeStockLists();
    wxInitializeStockObjects();

    wxBitmap::InitStandardHandlers();

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

// ---------------------------------------------------------------------------
// Convert Windows to argc, argv style
// ---------------------------------------------------------------------------

void wxApp::ConvertToStandardCommandArgs(const char* lpCmdLine)
{
    // break the command line in words
    wxArrayString args =
        wxCmdLineParser::ConvertStringToArgs(wxConvertMB2WX(lpCmdLine));

    // +1 here for the program name
    argc = args.GetCount() + 1;

    // and +1 here for the terminating NULL
    argv = new wxChar *[argc + 1];

    argv[0] = new wxChar[260]; // 260 is MAX_PATH value from windef.h
    ::GetModuleFileName(wxhInstance, argv[0], 260);

    // also set the app name from argv[0]
    wxString name;
    wxFileName::SplitPath(argv[0], NULL, &name, NULL);

    // but don't override the name already set by the user code, if any
    if ( GetAppName().empty() )
        SetAppName(name);

    // copy all the other arguments to wxApp::argv[]
    for ( int i = 1; i < argc; i++ )
    {
        argv[i] = copystring(args[i - 1]);
    }

    // argv[] must be NULL-terminated
    argv[argc] = NULL;
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

    wxDeleteStockObjects();

    // Destroy all GDI lists, etc.
    wxDeleteStockLists();

    delete wxTheColourDatabase;
    wxTheColourDatabase = NULL;

    wxBitmap::CleanUpHandlers();

    delete[] wxBuffer;
    wxBuffer = NULL;

    //// WINDOWS-SPECIFIC CLEANUP

#ifndef __WXMICROWIN__
    wxSetKeyboardHook(FALSE);
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

#ifdef WXMAKINGDLL
    // for an EXE the classes are unregistered when it terminates but DLL may
    // be loaded several times (load/unload/load) into the same process in
    // which case the registration will fail after the first time if we don't
    // unregister the classes now
    UnregisterWindowClasses();
#endif // WXMAKINGDLL

#if wxUSE_CTL3D
    Ctl3dUnregister(wxhInstance);
#endif

    delete wxWinHandleHash;
    wxWinHandleHash = NULL; // Set to null in case anything later tries to ref it.

    delete wxPendingEvents;
    wxPendingEvents = NULL; // Set to null because wxAppBase::wxEvtHandler is destroyed later.

#if wxUSE_THREADS
    delete wxPendingEventsLocker;
    wxPendingEventsLocker = NULL; // Set to null because wxAppBase::wxEvtHandler is destroyed later.
    // If we don't do the following, we get an apparent memory leak
#if wxUSE_VALIDATORS
    ((wxEvtHandler&) wxDefaultValidator).ClearEventLocker();
#endif // wxUSE_VALIDATORS
#endif // wxUSE_THREADS

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
        wxLogMessage(wxT("There were memory leaks."));
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

//----------------------------------------------------------------------
// Entry point helpers, used by wxPython
//----------------------------------------------------------------------

int WXDLLEXPORT wxEntryStart( int WXUNUSED(argc), char** WXUNUSED(argv) )
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


#if !defined(_WINDLL) || (defined(_WINDLL) && defined(WXMAKINGDLL))

// temporarily disable this warning which would be generated in release builds
// because of __try
#ifdef __VISUALC__
    #pragma warning(disable: 4715) // not all control paths return a value
#endif // Visual C++

//----------------------------------------------------------------------
// Main wxWindows entry point
//----------------------------------------------------------------------
int wxEntry(WXHINSTANCE hInstance,
            WXHINSTANCE WXUNUSED(hPrevInstance),
            char *lpCmdLine,
            int nCmdShow,
            bool enterLoop)
{
    // do check for memory leaks on program exit
    // (another useful flag is _CRTDBG_DELAY_FREE_MEM_DF which doesn't free
    //  deallocated memory which may be used to simulate low-memory condition)
#ifndef __WXMICROWIN__
    wxCrtSetDbgFlag(_CRTDBG_LEAK_CHECK_DF);
#endif

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

    // take everything into a try-except block to be able to call
    // OnFatalException() if necessary
#if wxUSE_ON_FATAL_EXCEPTION
    __try {
#endif
        wxhInstance = (HINSTANCE) hInstance;

        if (!wxEntryStart(0,0))
            return 0;

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

        // save the WinMain() parameters
        if (lpCmdLine) // MicroWindows passes NULL
            wxTheApp->ConvertToStandardCommandArgs(lpCmdLine);
        wxTheApp->m_nCmdShow = nCmdShow;

        // We really don't want timestamps by default, because it means
        // we can't simply double-click on the error message and get to that
        // line in the source. So VC++ at least, let's have a sensible default.
#ifdef __VISUALC__
#if wxUSE_LOG
        wxLog::SetTimestamp(NULL);
#endif // wxUSE_LOG
#endif // __VISUALC__

        // init the app
        int retValue = wxEntryInitGui() && wxTheApp->OnInit() ? 0 : -1;

        if ( retValue == 0 )
        {
            if ( enterLoop )
            {
                // run the main loop
                wxTheApp->OnRun();
            }
            else
            {
                // we want to initialize, but not run or exit immediately.
                return 1;
            }
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

        retValue = wxTheApp->OnExit();

        wxEntryCleanup();

        return retValue;

#if wxUSE_ON_FATAL_EXCEPTION
    }
    __except ( gs_handleExceptions ? EXCEPTION_EXECUTE_HANDLER
                                   : EXCEPTION_CONTINUE_SEARCH ) {
        if ( wxTheApp )
        {
           // give the user a chance to do something special about this
           wxTheApp->OnFatalException();
        }

        ::ExitProcess(3); // the same exit code as abort()

        // NOTREACHED
    }
#endif // wxUSE_ON_FATAL_EXCEPTION
}

// restore warning state
#ifdef __VISUALC__
    #pragma warning(default: 4715) // not all control paths return a value
#endif // Visual C++

#else /*  _WINDLL  */

//----------------------------------------------------------------------
// Entry point for wxWindows + the App in a DLL
//----------------------------------------------------------------------

int wxEntry(WXHINSTANCE hInstance)
{
    wxhInstance = (HINSTANCE) hInstance;
    wxEntryStart(0, 0);

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

    wxEntryInitGui();

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
    argc = 0;
    argv = NULL;
    m_printMode = wxPRINT_WINDOWS;
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

void wxApp::DeletePendingObjects()
{
    wxNode *node = wxPendingDelete.GetFirst();
    while (node)
    {
        wxObject *obj = node->GetData();

        delete obj;

        if (wxPendingDelete.Member(obj))
            delete node;

        // Deleting one object may have deleted other pending
        // objects, so start from beginning of list again.
        node = wxPendingDelete.GetFirst();
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

void wxExit()
{
    if ( wxTheApp )
    {
        wxTheApp->ExitMainLoop();
    }
    else
    {
        // what else can we do?
        exit(-1);
    }
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

bool wxHandleFatalExceptions(bool doit)
{
#if wxUSE_ON_FATAL_EXCEPTION
    // assume this can only be called from the main thread
    gs_handleExceptions = doit;

    return TRUE;
#else
    wxFAIL_MSG(_T("set wxUSE_ON_FATAL_EXCEPTION to 1 to use this function"));

    (void)doit;
    return FALSE;
#endif
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
            wxLogLastError(wxT("PostMessage(WM_NULL)"));
        }
    }
}

//-----------------------------------------------------------------------------

// For some reason, with MSVC++ 1.5, WinMain isn't linked in properly
// if in a separate file. So include it here to ensure it's linked.
#if (defined(__VISUALC__) && !defined(__WIN32__)) || (defined(__GNUWIN32__) && !defined(__WINE__) && !defined(WXMAKINGDLL))
#include "main.cpp"
#endif
