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
#endif

#include "wx/msw/private.h"
#include "wx/log.h"
#include "wx/module.h"

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

// To UG: there's no point in putting this #if here
// if you don't do the same for the Ole calls further down.
// Also, OLE is used not just for drag and drop (it's used by automatn.cpp).
// #if wxUSE_DRAG_AND_DROP
#if !defined(__GNUWIN32__) && !defined(__SC__) && !defined(__SALFORDC__)
    #include <ole2.h>
#endif

#include <string.h>
#include <ctype.h>

#if (defined(__WIN95__) && !defined(__GNUWIN32__)) || defined(__TWIN32__)
  #include <commctrl.h>
#endif

#include "wx/msw/msvcrt.h"

extern char *wxBuffer;
extern char *wxOsVersion;
extern wxList *wxWinHandleList;
extern wxList WXDLLEXPORT wxPendingDelete;
extern void wxSetKeyboardHook(bool doIt);
extern wxCursor *g_globalCursor;

HINSTANCE wxhInstance = 0;
static MSG s_currentMsg;
wxApp *wxTheApp = NULL;

// @@ why not const? and not static?
char wxFrameClassName[]         = "wxFrameClass";
char wxMDIFrameClassName[]      = "wxMDIFrameClass";
char wxMDIChildFrameClassName[] = "wxMDIChildFrameClass";
char wxPanelClassName[]         = "wxPanelClass";
char wxCanvasClassName[]        = "wxCanvasClass";

HICON wxSTD_FRAME_ICON = (HICON) NULL;
HICON wxSTD_MDICHILDFRAME_ICON = (HICON) NULL;
HICON wxSTD_MDIPARENTFRAME_ICON = (HICON) NULL;

HICON wxDEFAULT_FRAME_ICON = (HICON) NULL;
HICON wxDEFAULT_MDICHILDFRAME_ICON = (HICON) NULL;
HICON wxDEFAULT_MDIPARENTFRAME_ICON = (HICON) NULL;

HBRUSH wxDisableButtonBrush = (HBRUSH) 0;

LRESULT APIENTRY wxWndProc(HWND, UINT, WPARAM, LPARAM);

#if !USE_SHARED_LIBRARY
  IMPLEMENT_DYNAMIC_CLASS(wxApp, wxEvtHandler)

  BEGIN_EVENT_TABLE(wxApp, wxEvtHandler)
      EVT_IDLE(wxApp::OnIdle)
      EVT_END_SESSION(wxApp::OnEndSession)
      EVT_QUERY_END_SESSION(wxApp::OnQueryEndSession)
  END_EVENT_TABLE()
#endif

long wxApp::sm_lastMessageTime = 0;

#if defined(__WIN95__) && !defined(__TWIN32__)
#define wxUSE_RICHEDIT 1
#else
#define wxUSE_RICHEDIT 0
#endif

#if wxUSE_RICHEDIT
  static HINSTANCE gs_hRichEdit = (HINSTANCE) NULL;
#endif

//// Initialize

bool wxApp::Initialize()
{
    // Some people may wish to use this, but
    // probably it shouldn't be here by default.
#ifdef __WXDEBUG__
//    wxRedirectIOToConsole();
#endif

    wxBuffer = new char[1500];

    wxClassInfo::InitializeClasses();

#if wxUSE_RESOURCES
    wxGetResource("wxWindows", "OsVersion", &wxOsVersion);
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

#if wxUSE_RICHEDIT
    gs_hRichEdit = LoadLibrary("RICHED32.DLL");

    if (gs_hRichEdit == (HINSTANCE) NULL)
    {
      wxMessageBox("Could not initialise Rich Edit DLL");
    }
#endif

#endif

    int iMsg = 96;

   // for OLE, enlarge message queue to be as large as possible
   while (!SetMessageQueue(iMsg) && (iMsg -= 8));

/*
    DWORD dwOleVer;
    dwOleVer = CoBuildVersion();

    // check the OLE library version
    if (rmm != HIWORD(dwOleVer))
    {
        wxMessageBox("Incorrect version of OLE libraries.");
        return FALSE;
    }
*/

#if !defined(__GNUWIN32__) && !defined(__SC__) && !defined(__SALFORDC__)
    // we need to initialize OLE library
    if ( FAILED(::OleInitialize(NULL)) )
      wxFatalError(_("Cannot initialize OLE"));
#endif

#if CTL3D
    if (!Ctl3dRegister(wxhInstance))
      wxFatalError("Cannot register CTL3D");

    Ctl3dAutoSubclass(wxhInstance);
#endif

    g_globalCursor = new wxCursor;

    wxSTD_FRAME_ICON = LoadIcon(wxhInstance, "wxSTD_FRAME");
    wxSTD_MDIPARENTFRAME_ICON = LoadIcon(wxhInstance, "wxSTD_MDIPARENTFRAME");
    wxSTD_MDICHILDFRAME_ICON = LoadIcon(wxhInstance, "wxSTD_MDICHILDFRAME");

    wxDEFAULT_FRAME_ICON = LoadIcon(wxhInstance, "wxDEFAULT_FRAME");
    wxDEFAULT_MDIPARENTFRAME_ICON = LoadIcon(wxhInstance, "wxDEFAULT_MDIPARENTFRAME");
    wxDEFAULT_MDICHILDFRAME_ICON = LoadIcon(wxhInstance, "wxDEFAULT_MDICHILDFRAME");

    RegisterWindowClasses();

    // Create the brush for disabling bitmap buttons

    LOGBRUSH lb ;
    lb.lbStyle = BS_PATTERN;
    lb.lbHatch = (int)LoadBitmap( wxhInstance, "wxDISABLE_BUTTON_BITMAP" ) ;
    wxDisableButtonBrush = ::CreateBrushIndirect( & lb ) ;
    ::DeleteObject( (HGDIOBJ)lb.lbHatch ) ;

#if wxUSE_PENWINDOWS
    wxRegisterPenWin();
#endif

    wxWinHandleList = new wxList(wxKEY_INTEGER);

    // This is to foil optimizations in Visual C++ that throw out dummy.obj.
    // PLEASE DO NOT ALTER THIS.
#if defined(_MSC_VER) && !defined(WXMAKINGDLL)
    extern char wxDummyChar;
    if (wxDummyChar) wxDummyChar++;
#endif

    wxSetKeyboardHook(TRUE);

    wxModule::RegisterModules();
    if (!wxModule::InitializeModules())
        return FALSE;
    return TRUE;
}

//// RegisterWindowClasses

bool wxApp::RegisterWindowClasses()
{
///////////////////////////////////////////////////////////////////////
// Register the frame window class.
  WNDCLASS wndclass;   // Structure used to register Windows class.

  wndclass.style         = CS_HREDRAW | CS_VREDRAW;
  wndclass.lpfnWndProc   = (WNDPROC)wxWndProc;
  wndclass.cbClsExtra    = 0;
  wndclass.cbWndExtra    = sizeof( DWORD ); // was 4
  wndclass.hInstance     = wxhInstance;
  wndclass.hIcon         = (HICON) NULL;        // wxSTD_FRAME_ICON;
  wndclass.hCursor       = LoadCursor( (HINSTANCE) NULL, IDC_ARROW );
  wndclass.hbrBackground =  (HBRUSH)(COLOR_APPWORKSPACE+1) ;
//  wndclass.hbrBackground = GetStockObject( WHITE_BRUSH );
  wndclass.lpszMenuName  = NULL;
#ifdef _MULTIPLE_INSTANCES
  sprintf( wxFrameClassName,"wxFrameClass%d", wxhInstance );
#endif
  wndclass.lpszClassName = wxFrameClassName;

  if (!RegisterClass( &wndclass ))
  {
    // wxFatalError("Can't register Frame Window class");
  }

///////////////////////////////////////////////////////////////////////
// Register the MDI frame window class.
  WNDCLASS wndclass1;   // Structure used to register Windows class.

  wndclass1.style         = CS_HREDRAW | CS_VREDRAW;
  wndclass1.lpfnWndProc   = (WNDPROC)wxWndProc;
  wndclass1.cbClsExtra    = 0;
  wndclass1.cbWndExtra    = sizeof( DWORD ); // was 4
  wndclass1.hInstance     = wxhInstance;
  wndclass1.hIcon         = (HICON) NULL; // wxSTD_MDIPARENTFRAME_ICON;
  wndclass1.hCursor       = LoadCursor( (HINSTANCE) NULL, IDC_ARROW );
//  wndclass1.hbrBackground =  (HBRUSH)(COLOR_APPWORKSPACE+1) ;
  wndclass1.hbrBackground = (HBRUSH) NULL;
  wndclass1.lpszMenuName  = NULL;

  wndclass1.lpszClassName = wxMDIFrameClassName;
  if (!RegisterClass( &wndclass1 ))
  {
//    wxFatalError("Can't register MDI Frame window class");
//  return FALSE;
  }

///////////////////////////////////////////////////////////////////////
// Register the MDI child frame window class.
  WNDCLASS wndclass4;   // Structure used to register Windows class.

  wndclass4.style         = CS_HREDRAW | CS_VREDRAW;
  wndclass4.lpfnWndProc   = (WNDPROC)wxWndProc;
  wndclass4.cbClsExtra    = 0;
  wndclass4.cbWndExtra    = sizeof( DWORD ); // was 4
  wndclass4.hInstance     = wxhInstance;
  wndclass4.hIcon         = (HICON) NULL;       // wxSTD_MDICHILDFRAME_ICON;
  wndclass4.hCursor       = LoadCursor( (HINSTANCE) NULL, IDC_ARROW );
  // TODO: perhaps this should be NULL so that Windows doesn't
  // paint the background itself (would OnEraseBackground duplicate
  // this?)
  wndclass4.hbrBackground =  (HBRUSH)(COLOR_WINDOW+1) ;
//  wndclass4.hbrBackground = NULL;
  wndclass4.lpszMenuName  = NULL;
  wndclass4.lpszClassName = wxMDIChildFrameClassName;

  if (!RegisterClass( &wndclass4 ))
  {
//   wxFatalError("Can't register MDI child frame window class");
//   return FALSE;
  }

///////////////////////////////////////////////////////////////////////
// Register the panel window class.
  WNDCLASS wndclass2;   // Structure used to register Windows class.
  memset(&wndclass2, 0, sizeof(WNDCLASS));   // start with NULL defaults
  // Use CS_OWNDC to avoid messing about restoring the context
  // for every graphic operation.
  wndclass2.style         = CS_HREDRAW | CS_VREDRAW;
  wndclass2.lpfnWndProc   = (WNDPROC)wxWndProc;
  wndclass2.cbClsExtra    = 0;
  wndclass2.cbWndExtra    = sizeof( DWORD ); // was 4
  wndclass2.hInstance     = wxhInstance;
  wndclass2.hIcon         = (HICON) NULL;
  wndclass2.hCursor       = (HCURSOR) NULL;
//  wndclass2.hbrBackground = (HBRUSH)(COLOR_BTNFACE+1) ;
  wndclass2.hbrBackground = (HBRUSH) GetStockObject( LTGRAY_BRUSH );
  wndclass2.lpszMenuName  = NULL;
  wndclass2.lpszClassName = wxPanelClassName;
  if (!RegisterClass( &wndclass2 ))
  {
//   wxFatalError("Can't register Panel Window class");
//   return FALSE;
  }

///////////////////////////////////////////////////////////////////////
// Register the canvas and textsubwindow class name
  WNDCLASS wndclass3;   // Structure used to register Windows class.
  memset(&wndclass3, 0, sizeof(WNDCLASS));   // start with NULL defaults
  // Use CS_OWNDC to avoid messing about restoring the context
  // for every graphic operation.
//  wndclass3.style         = CS_HREDRAW | CS_VREDRAW | CS_OWNDC | CS_DBLCLKS ;
  // wxWin 2.0
  wndclass3.style         = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS ;
  wndclass3.lpfnWndProc   = (WNDPROC)wxWndProc;
  wndclass3.cbClsExtra    = 0;
  wndclass3.cbWndExtra    = sizeof( DWORD ); // was 4
  wndclass3.hInstance     = wxhInstance;
  wndclass3.hIcon         = (HICON) NULL;
  wndclass3.hCursor       = (HCURSOR) NULL;
//  wndclass3.hbrBackground = (HBRUSH)(COLOR_WINDOW+1) ;
  wndclass3.hbrBackground = (HBRUSH) NULL;
  wndclass3.lpszMenuName  = NULL;
  wndclass3.lpszClassName = wxCanvasClassName;
  if (!RegisterClass( &wndclass3))
  {
//   wxFatalError("Can't register Canvas class");
//   return FALSE;
  }

  return TRUE;
}

//// Convert Windows to argc, argv style

void wxApp::ConvertToStandardCommandArgs(char* lpCmdLine)
{
    wxStringList args;

    wxString cmdLine(lpCmdLine);
    int count = 0;

    // Get application name
    char name[260]; // 260 is MAX_PATH value from windef.h
    ::GetModuleFileName(wxhInstance, name, WXSIZEOF(name));

    // GNUWIN32 already fills in the first arg with the application name.
#if !defined(__GNUWIN32__)
    args.Add(name);
    count ++;
#endif

    strcpy(name, wxFileNameFromPath(name));
    wxStripExtension(name);
    wxTheApp->SetAppName(name);

    // Break up string
    // Treat strings enclosed in double-quotes as single arguments
    int i = 0;
    int len = cmdLine.Length();
    while (i < len)
    {
        // Skip whitespace
        while ((i < len) && isspace(cmdLine.GetChar(i)))
            i ++;

        if (i < len)
        {
            if (cmdLine.GetChar(i) == '"') // We found the start of a string
            {
                i ++;
                int first = i;
                while ((i < len) && (cmdLine.GetChar(i) != '"'))
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
                while ((i < len) && !isspace(cmdLine.GetChar(i)))
                    i ++;

                wxString arg(cmdLine.Mid(first, (i - first)));

                args.Add(arg);
                count ++;
            }
        }
    }

    wxTheApp->argv = new char*[count + 1];
    for (i = 0; i < count; i++)
    {
        wxString arg(args[i]);
        wxTheApp->argv[i] = copystring((const char*)arg);
    }
    wxTheApp->argv[count] = NULL; // argv[] is a NULL-terminated list
    wxTheApp->argc = count;
}

//// Cleans up any wxWindows internal structures left lying around

void wxApp::CleanUp()
{
  //// COMMON CLEANUP
  wxModule::CleanUpModules();

#if wxUSE_WX_RESOURCES
  wxCleanUpResourceSystem();

//  wxDefaultResourceTable->ClearTable();
#endif

  // Indicate that the cursor can be freed,
  // so that cursor won't be deleted by deleting
  // the bitmap list before g_globalCursor goes out
  // of scope (double deletion of the cursor).
  wxSetCursor(wxNullCursor);
  delete g_globalCursor;

  wxDeleteStockObjects() ;

  // Destroy all GDI lists, etc.
  wxDeleteStockLists();

  delete wxTheColourDatabase;
  wxTheColourDatabase = NULL;

  wxBitmap::CleanUpHandlers();

  delete[] wxBuffer;
  wxBuffer = NULL;

  //// WINDOWS-SPECIFIC CLEANUP

  wxSetKeyboardHook(FALSE);

#ifdef __WIN95__

#if wxUSE_RICHEDIT
  if (gs_hRichEdit != (HINSTANCE) NULL)
    FreeLibrary(gs_hRichEdit);
#endif

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
    ::DeleteObject( wxDisableButtonBrush ) ;

#if !defined(__GNUWIN32__) && !defined(__SC__) && !defined(__SALFORDC__)
  ::OleUninitialize();
#endif

#if CTL3D
  Ctl3dUnregister(wxhInstance);
#endif

  if (wxWinHandleList)
    delete wxWinHandleList ;

  wxClassInfo::CleanUpClasses();

  delete wxTheApp;
  wxTheApp = NULL;

#if (defined(__WXDEBUG__) && wxUSE_MEMORY_TRACING) || wxUSE_DEBUG_CONTEXT
  // At this point we want to check if there are any memory
  // blocks that aren't part of the wxDebugContext itself,
  // as a special case. Then when dumping we need to ignore
  // wxDebugContext, too.
  if (wxDebugContext::CountObjectsLeft() > 0)
  {
      wxLogDebug("There were memory leaks.");
      wxDebugContext::Dump();
      wxDebugContext::PrintStatistics();
  }
//  wxDebugContext::SetStream(NULL, NULL);
#endif

  // do it as the very last thing because everything else can log messages
  wxLog::DontCreateOnDemand();
  delete wxLog::SetActiveTarget(NULL);
}

#if !defined(_WINDLL) || (defined(_WINDLL) && defined(WXMAKINGDLL))

// temporarily disable this warning
#ifdef _MSC_VER
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

  // take everything into a try-except block in release build
  // FIXME other compilers must support Win32 SEH (structured exception
  //       handling) too, just find the appropriate keyword in their docs!
  //       Please note that it's _not_ the same as C++ exceptions!
#if !defined(__WXDEBUG__) && defined(_MSC_VER) && !defined(__MWERKS__)
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
                 "No initializer - use IMPLEMENT_APP macro." );

    wxTheApp = (*wxApp::GetInitializerFunction()) ();
  }

  wxCHECK_MSG( wxTheApp, 0, "You have to define an instance of wxApp!" );

  // save the WinMain() parameters
  wxTheApp->ConvertToStandardCommandArgs(lpCmdLine);
  wxTheApp->m_nCmdShow = nCmdShow;

  // GUI-specific initialisation. In fact on Windows we don't have any,
  // but this call is provided for compatibility across platforms.
  wxTheApp->OnInitGui() ;

  int retValue = 0;

  if ( wxTheApp->OnInit() )
  {
      if ( enterLoop )
      {
          retValue = wxTheApp->OnRun();
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

  wxTheApp->OnExit();

  // flush the logged messages if any
  wxLog *pLog = wxLog::GetActiveTarget();
  if ( pLog != NULL && pLog->HasPendingMessages() )
    pLog->Flush();


  wxApp::CleanUp();

  return retValue;

#ifdef CATCH_PROGRAM_EXCEPTIONS
  }
  __except ( EXCEPTION_EXECUTE_HANDLER ) {
    /*
    if ( wxTheApp )
      wxTheApp->OnFatalException();
    */

    ::ExitProcess(3); // the same exit code as abort()

    // NOTREACHED
  }
#endif // CATCH_PROGRAM_EXCEPTIONS
}

// restore warning state
#ifdef _MSC_VER
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
    if (!wxApp::GetInitializerFunction())
    {
      MessageBox(NULL, "No initializer - use IMPLEMENT_APP macro.", "wxWindows Error", MB_APPLMODAL | MB_ICONSTOP | MB_OK);
    return 0;
    }

    wxTheApp = (* wxApp::GetInitializerFunction()) ();
  }

  if (!wxTheApp) {
    MessageBox(NULL, "You have to define an instance of wxApp!", "wxWindows Error", MB_APPLMODAL | MB_ICONSTOP | MB_OK);
    return 0;
  }

  wxTheApp->argc = 0;
  wxTheApp->argv = NULL;

  wxTheApp->OnInitGui();

  wxTheApp->OnInit();

  if (wxTheApp->GetTopWindow() && wxTheApp->GetTopWindow()->GetHWND()) {
    wxTheApp->GetTopWindow()->Show(TRUE);
  }

  return 1;
}
#endif // _WINDLL

//// Static member initialization

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
#ifdef __WXMSW__
  m_printMode = wxPRINT_WINDOWS;
#else
  m_printMode = wxPRINT_POSTSCRIPT;
#endif
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
                      "only the main thread can process Windows messages" );

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
            wxApp::sm_lastMessageTime = s_currentMsg.time; /* MATTHEW: timeStamp impl. */
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
  return (::PeekMessage(&s_currentMsg, 0, 0, 0, PM_NOREMOVE) != 0) ;
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
bool wxApp::ProcessMessage(WXMSG *Msg)
{
  MSG *msg = (MSG *)Msg;

  HWND hWnd;

  // Try translations first; find the youngest window with
  // a translation table.
  for (hWnd = msg->hwnd; hWnd != (HWND) NULL; hWnd = ::GetParent(hWnd))
  {
    wxWindow *wnd = wxFindWinFromHandle((WXHWND) hWnd);
    if (wnd)
    {
       if (wnd->MSWTranslateMessage(Msg))
         return TRUE;
    }
  }

  // Anyone for a non-translation message? Try youngest descendants first.
  for (hWnd = msg->hwnd; hWnd != (HWND) NULL; hWnd = ::GetParent(hWnd))
  {
    wxWindow *wnd = wxFindWinFromHandle((WXHWND) hWnd);
    if (wnd)
    {
       if (wnd->MSWProcessMessage(Msg))
         return TRUE;
    }
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

    // flush the logged messages if any
    wxLog *pLog = wxLog::GetActiveTarget();
    if ( pLog != NULL && pLog->HasPendingMessages() )
        pLog->Flush();

    // Send OnIdle events to all windows
    if ( SendIdleEvents() )
    {
        // SendIdleEvents() returns TRUE if at least one window requested more
        // idle events
        event.RequestMore(TRUE);
    }

    s_inOnIdle = FALSE;
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

int wxApp::GetComCtl32Version() const
{
    // have we loaded COMCTL32 yet?
    HMODULE theModule = ::GetModuleHandle("COMCTL32");
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
}

void wxExit()
{
  wxApp::CleanUp();
  FatalAppExit(0, "Fatal error: exiting");
}

// Yield to incoming messages
bool wxYield()
{
  MSG msg;
  // We want to go back to the main message loop
  // if we see a WM_QUIT. (?)
  while (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE) && msg.message != WM_QUIT)
  {
    if ( !wxTheApp->DoMessage() )
      break;
  }

  return TRUE;
}

HINSTANCE wxGetInstance()
{
  return wxhInstance;
}

// For some reason, with MSVC++ 1.5, WinMain isn't linked in properly
// if in a separate file. So include it here to ensure it's linked.
#if (defined(_MSC_VER) && !defined(__WIN32__)) || (defined(__GNUWIN32__) && !defined(__TWIN32__))
  #include "main.cpp"
#endif
