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

    // define the array of QMSG strutures
    WX_DECLARE_OBJARRAY(QMSG, wxMsgArray);

    #include "wx/arrimpl.cpp"

    WX_DEFINE_OBJARRAY(wxMsgArray);
#endif // wxUSE_THREADS

#if wxUSE_WX_RESOURCES
    #include "wx/resource.h"
#endif

#include <string.h>
#include <ctype.h>

// ---------------------------------------------------------------------------
// global variables
// ---------------------------------------------------------------------------

extern wxChar*                      wxBuffer;
extern wxChar*                      wxOsVersion;
extern wxList*                      wxWinHandleList;
extern wxList WXDLLEXPORT           wxPendingDelete;
extern void wxSetKeyboardHook(bool doIt);
extern wxCursor*                    g_globalCursor;

HINSTANCE                           wxhInstance = 0;
QMSG                                svCurrentMsg;
wxApp*                              wxTheApp = NULL;

// FIXME why not const? and not static?

// NB: all "NoRedraw" classes must have the same names as the "normal" classes
//     with NR suffix - wxWindow::OS2Create() supposes this
wxChar wxFrameClassName[]                 = wxT("wxFrameClass");
wxChar wxFrameClassNameNoRedraw[]         = wxT("wxFrameClassNR");
wxChar wxMDIFrameClassName[]              = wxT("wxMDIFrameClass");
wxChar wxMDIFrameClassNameNoRedraw[]      = wxT("wxMDIFrameClassNR");
wxChar wxMDIChildFrameClassName[]         = wxT("wxMDIChildFrameClass");
wxChar wxMDIChildFrameClassNameNoRedraw[] = wxT("wxMDIChildFrameClassNR");
wxChar wxPanelClassName[]                 = wxT("wxPanelClass");
wxChar wxCanvasClassName[]                = wxT("wxCanvasClass");

HICON wxSTD_FRAME_ICON          = (HICON) NULL;
HICON wxSTD_MDICHILDFRAME_ICON  = (HICON) NULL;
HICON wxSTD_MDIPARENTFRAME_ICON = (HICON) NULL;

HICON wxDEFAULT_FRAME_ICON           = (HICON) NULL;
HICON wxDEFAULT_MDICHILDFRAME_ICON   = (HICON) NULL;
HICON wxDEFAULT_MDIPARENTFRAME_ICON  = (HICON) NULL;

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
bool wxApp::Initialize(
  HAB                               vHab
)
{
    // Some people may wish to use this, but
    // probably it shouldn't be here by default.
#ifdef __WXDEBUG__
    //    wxRedirectIOToConsole();
#endif

    wxBuffer = new wxChar[1500]; // FIXME; why?

    wxClassInfo::InitializeClasses();

#if wxUSE_RESOURCES
    wxGetResource(wxT("wxWindows"), wxT("OsVersion"), &wxOsVersion);
#endif

    // I'm annoyed ... I don't know where to put this and I don't want to
    // create a module for that as it's part of the core.
    wxTheColourDatabase = new wxColourDatabase(wxKEY_STRING);
    wxTheColourDatabase->Initialize();

    wxInitializeStockLists();
    wxInitializeStockObjects();

#if wxUSE_WX_RESOURCES
    wxInitializeResourceSystem();
#endif

    wxBitmap::InitStandardHandlers();

    g_globalCursor = new wxCursor;

#if 0
    wxSTD_FRAME_ICON = ::WinLoadFileIcon(wxT("wxSTD_FRAME"), TRUE);
    wxSTD_MDIPARENTFRAME_ICON = ::WinLoadFileIcon(wxT("wxSTD_MDIPARENTFRAME"), TRUE);
    wxSTD_MDICHILDFRAME_ICON = ::WinLoadFileIcon(wxT("wxSTD_MDICHILDFRAME"), TRUE);

    wxDEFAULT_FRAME_ICON = ::WinLoadFileIcon(wxT("wxDEFAULT_FRAME"), TRUE);
    wxDEFAULT_MDIPARENTFRAME_ICON = ::WinLoadFileIcon(wxT("wxDEFAULT_MDIPARENTFRAME"), TRUE);
    wxDEFAULT_MDICHILDFRAME_ICON = ::WinLoadFileIcon(wxT("wxDEFAULT_MDICHILDFRAME"), TRUE);
#endif
    RegisterWindowClasses(vHab);
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
bool wxApp::RegisterWindowClasses(
  HAB                               vHab
)
{

    if (!::WinRegisterClass( vHab
                            ,wxFrameClassName
                            ,(PFNWP)wxWndProc
                            ,CS_SIZEREDRAW | CS_SYNCPAINT | CS_HITTEST | CS_CLIPCHILDREN | CS_FRAME
                            ,0
                           ))
    {
        wxLogLastError("RegisterClass(frame)");

        return FALSE;
    }

    if (!::WinRegisterClass( vHab
                            ,wxFrameClassNameNoRedraw
                            ,(PFNWP)wxWndProc
                            ,CS_HITTEST | CS_CLIPCHILDREN | CS_FRAME
                            ,0
                           ))
    {
        wxLogLastError("RegisterClass(no redraw frame)");

        return FALSE;
    }

    if (!::WinRegisterClass( vHab
                            ,wxMDIFrameClassName
                            ,(PFNWP)wxWndProc
                            ,CS_SIZEREDRAW | CS_SYNCPAINT | CS_HITTEST | CS_CLIPCHILDREN | CS_FRAME
                            ,0
                           ))
    {
        wxLogLastError("RegisterClass(MDI parent)");

        return FALSE;
    }

    if (!::WinRegisterClass( vHab
                            ,wxMDIFrameClassNameNoRedraw
                            ,(PFNWP)wxWndProc
                            ,CS_HITTEST | CS_CLIPCHILDREN | CS_FRAME
                            ,0
                           ))
    {
        wxLogLastError("RegisterClass(no redraw MDI parent)");

        return FALSE;
    }

    if (!::WinRegisterClass( vHab
                            ,wxMDIChildFrameClassName
                            ,(PFNWP)wxWndProc
                            ,CS_MOVENOTIFY | CS_SIZEREDRAW | CS_SYNCPAINT | CS_HITTEST | CS_CLIPSIBLINGS | CS_FRAME
                            ,0
                           ))
    {
        wxLogLastError("RegisterClass(MDI child)");

        return FALSE;
    }

    if (!::WinRegisterClass( vHab
                            ,wxMDIChildFrameClassNameNoRedraw
                            ,(PFNWP)wxWndProc
                            ,CS_HITTEST | CS_CLIPSIBLINGS | CS_FRAME
                            ,0
                           ))
    {
        wxLogLastError("RegisterClass(no redraw MDI child)");

        return FALSE;
    }

    if (!::WinRegisterClass( vHab
                            ,wxPanelClassName
                            ,(PFNWP)wxWndProc
                            ,CS_MOVENOTIFY | CS_SIZEREDRAW | CS_HITTEST | CS_CLIPSIBLINGS | CS_SAVEBITS | CS_SYNCPAINT
                            ,0
                           ))
    {
        wxLogLastError("RegisterClass(Panel)");

        return FALSE;
    }

    if (!::WinRegisterClass( vHab
                            ,wxCanvasClassName
                            ,(PFNWP)wxWndProc
                            ,CS_MOVENOTIFY | CS_SIZEREDRAW | CS_HITTEST | CS_CLIPSIBLINGS | CS_SAVEBITS | CS_SYNCPAINT
                            ,0
                           ))
    {
        wxLogLastError("RegisterClass(Canvas)");

        return FALSE;
    }
    return TRUE;
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

    if (wxSTD_FRAME_ICON)
        ::WinFreeFileIcon(wxSTD_FRAME_ICON);
    if (wxSTD_MDICHILDFRAME_ICON)
        ::WinFreeFileIcon(wxSTD_MDICHILDFRAME_ICON);
    if (wxSTD_MDIPARENTFRAME_ICON)
        ::WinFreeFileIcon(wxSTD_MDIPARENTFRAME_ICON);

    if (wxDEFAULT_FRAME_ICON)
        ::WinFreeFileIcon(wxDEFAULT_FRAME_ICON);
    if (wxDEFAULT_MDICHILDFRAME_ICON)
        ::WinFreeFileIcon(wxDEFAULT_MDICHILDFRAME_ICON);
    if (wxDEFAULT_MDIPARENTFRAME_ICON)
        ::WinFreeFileIcon(wxDEFAULT_MDIPARENTFRAME_ICON);

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

int wxEntry(
  int                               argc
, char*                             argv[]
)
{
    HAB                             vHab;

    if (!wxApp::Initialize(vHab))
        return 0;

    //
    // create the application object or ensure that one already exists
    //
    if (!wxTheApp)
    {
        wxCHECK_MSG( wxApp::GetInitializerFunction()
                    ,-1
                    ,wxT("wxWindows error: No initializer - use IMPLEMENT_APP macro.\n")
                   );

        wxAppInitializerFunction    fnAppIni = wxApp::GetInitializerFunction();
        wxObject*                   pTest_app = fnAppIni();

        wxTheApp = (wxApp*)pTest_app;
    }
    wxCHECK_MSG( wxTheApp, -1, wxT("wxWindows error: no application object") );
    wxTheApp->argc = argc;

#if wxUSE_UNICODE
    wxTheApp->argv = new wxChar*[argc+1];

    int                             nArgc = 0;

    while (nArgc < argc)
    {
          wxTheApp->argv[nArgc] = wxStrdup(wxConvLibc.cMB2WX(argv[nArgc]));
          nArgc++;
    }
    wxTheApp->argv[nArgc] = (wxChar *)NULL;
#else
    wxTheApp->argv = argv;
#endif

    wxString                        sName(wxFileNameFromPath(argv[0]));

    wxStripExtension(sName);
    wxTheApp->SetAppName(sName);

    int                             nRetValue = 0;

    if (!wxTheApp->OnInitGui())
        nRetValue = -1;

    if (nRetValue == 0)
    {
        if (wxTheApp->OnInit())
        {
            nRetValue = -1;
        }
    }

    if (nRetValue == 0)
    {
        wxWindow*                   pTopWindow = wxTheApp->GetTopWindow();

        if (pTopWindow)
        {
            // Forcibly delete the window.
            if (pTopWindow->IsKindOf(CLASSINFO(wxFrame)) ||
                pTopWindow->IsKindOf(CLASSINFO(wxDialog)) )
            {
                pTopWindow->Close(TRUE);
                wxTheApp->DeletePendingObjects();
            }
            else
            {
                delete pTopWindow;
                wxTheApp->SetTopWindow(NULL);
            }
        }
    }
    wxTheApp->OnExit();
    wxApp::CleanUp();
    return(nRetValue);
}

bool wxApp::OnInitGui()
{
    m_vHab = WinInitialize(0);
    m_hMq  = WinCreateMsgQueue(m_vHab, 0);

    return TRUE;
}

//
// Static member initialization
//
wxAppInitializerFunction wxAppBase::m_appInitFn = (wxAppInitializerFunction) NULL;

wxApp::wxApp()
{
    m_topWindow = NULL;
    wxTheApp = this;
    m_wantDebugOutput = TRUE;

    argc = 0;
    argv = NULL;
    m_nPrintMode = wxPRINT_WINDOWS;
    m_exitOnFrameDelete = TRUE;
    m_bAuto3D = TRUE;
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
    if (GetTopWindow())
        return TRUE;
    else
        return FALSE;
}

//
// Get and process a message, returning FALSE if WM_QUIT
// received (and also set the flag telling the app to exit the main loop)
//
bool wxApp::DoMessage()
{
    BOOL                            bRc = ::WinGetMsg(m_vHab, &m_vMsg, HWND(NULL), 0, 0);

    if (bRc == 0)
    {
        // got WM_QUIT
        m_bKeepGoing = FALSE;
        return FALSE;
    }
    else if (bRc == -1)
    {
        // should never happen, but let's test for it nevertheless
        wxLogLastError("GetMessage");
    }
    else
    {
#if wxUSE_THREADS
        wxASSERT_MSG( wxThread::IsMain()
                     ,wxT("only the main thread can process Windows messages")
                    );

        static bool                 sbHadGuiLock = TRUE;
        static wxMsgArray           svSavedMessages;

        //
        // if a secondary thread owns is doing GUI calls, save all messages for
        // later processing - we can't process them right now because it will
        // lead to recursive library calls (and we're not reentrant)
        //
        if (!wxGuiOwnedByMainThread())
        {
            sbHadGuiLock = FALSE;

            // leave out WM_COMMAND messages: too dangerous, sometimes
            // the message will be processed twice
            if ( !wxIsWaitingForThread() ||
                    svCurrentMsg.msg != WM_COMMAND )
            {
                svSavedMessages.Add(svCurrentMsg);
            }
            return TRUE;
        }
        else
        {
            //
            // have we just regained the GUI lock? if so, post all of the saved
            // messages
            //
            // FIXME of course, it's not _exactly_ the same as processing the
            //       messages normally - expect some things to break...
            //
            if (!sbHadGuiLock )
            {
                sbHadGuiLock = TRUE;

                size_t             nCount = svSavedMessages.Count();

                for (size_t n = 0; n < nCount; n++)
                {
                    QMSG            vMsg = svSavedMessages[n];

                    if ( !ProcessMessage((WXMSG *)&vMsg) )
                    {
                        ::WinDispatchMsg(m_vHab, &vMsg);
                    }
                }
                svSavedMessages.Empty();
            }
        }
#endif // wxUSE_THREADS

        // Process the message
        if (!ProcessMessage((WXMSG *)&svCurrentMsg) )
        {
            ::WinDispatchMsg(m_vHab, (PQMSG)&svCurrentMsg);
        }
    }
    return TRUE;
}

//////////////////////////////////////////////////////////////////////////////
//
// Keep trying to process messages until WM_QUIT
// received.
//
// If there are messages to be processed, they will all be
// processed and OnIdle will not be called.
// When there are no more messages, OnIdle is called.
// If OnIdle requests more time,
// it will be repeatedly called so long as there are no pending messages.
// A 'feature' of this is that once OnIdle has decided that no more processing
// is required, then it won't get processing time until further messages
// are processed (it'll sit in DoMessage).
//
//////////////////////////////////////////////////////////////////////////////
int wxApp::MainLoop()
{
    m_bKeepGoing = TRUE;

    while (m_bKeepGoing)
    {
#if wxUSE_THREADS
        wxMutexGuiLeaveOrEnter();
#endif // wxUSE_THREADS
        while (!::WinPeekMsg(m_vHab, &svCurrentMsg, (HWND)NULL, 0, 0, PM_NOREMOVE) &&
                ProcessIdle() )
        {
        }
        DoMessage();
    }
    return (int)svCurrentMsg.mp1;
}

//
// Returns TRUE if more time is needed.
//
bool wxApp::ProcessIdle()
{
    wxIdleEvent                     vEvent;

    vEvent.SetEventObject(this);
    ProcessEvent(vEvent);
    return vEvent.MoreRequested();
}

#if wxUSE_THREADS
void wxApp::ProcessPendingEvents()
{
    wxNode*                         pNode = wxPendingEvents->First();
    wxCriticalSectionLocker         vLocker(*wxPendingEventsLocker);

    while (pNode)
    {
        wxEvtHandler*               pHandler = (wxEvtHandler *)pNode->Data();
        pHandler->ProcessPendingEvents();

        delete pNode;
        pNode = wxPendingEvents->First();
    }
}
#endif

void wxApp::ExitMainLoop()
{
    m_bKeepGoing = FALSE;
}

bool wxApp::Pending()
{
    return (::WinPeekMsg(m_vHab, (PQMSG)&svCurrentMsg, (HWND)NULL, 0, 0, PM_NOREMOVE) != 0);
}

void wxApp::Dispatch()
{
    DoMessage();
}

//////////////////////////////////////////////////////////////////////////////
//
// Give all windows a chance to preprocess
// the message. Some may have accelerator tables, or have
// MDI complications.
//
//////////////////////////////////////////////////////////////////////////////
bool wxApp::ProcessMessage(
  WXMSG*                            pWxmsg
)
{
    QMSG*                           vMsg = (PQMSG)pWxmsg;
    HWND                            hWnd = vMsg->hwnd;
    wxWindow*                       pWndThis = wxFindWinFromHandle((WXHWND)hWnd);
    wxWindow*                       pWnd;

    //
    // for some composite controls (like a combobox), wndThis might be NULL
    // because the subcontrol is not a wxWindow, but only the control itself
    // is - try to catch this case
    //
    while (hWnd && !pWndThis)
    {
        hWnd = ::WinQueryWindow(hWnd, QW_PARENT);
        pWndThis = wxFindWinFromHandle((WXHWND)hWnd);
    }

    // Anyone for a non-translation message? Try youngest descendants first.
    for (pWnd = pWndThis; pWnd; pWnd = pWnd->GetParent())
    {
        if (pWnd->OS2ProcessMessage(pWxmsg))
            return TRUE;
    }
    return FALSE;
}

void wxApp::OnIdle(
  wxIdleEvent&                      rEvent
)
{
    static bool                     sbInOnIdle = FALSE;

    //
    // Avoid recursion (via ProcessEvent default case)
    //
    if (sbInOnIdle )
        return;

    sbInOnIdle = TRUE;

    //
    // 'Garbage' collection of windows deleted with Close().
    //
    DeletePendingObjects();

#if wxUSE_LOG
    // flush the logged messages if any
    wxLog*                          pLog = wxLog::GetActiveTarget();

    if (pLog != NULL && pLog->HasPendingMessages())
        pLog->Flush();
#endif // wxUSE_LOG

    // Send OnIdle events to all windows
    if (SendIdleEvents())
    {
        //
        // SendIdleEvents() returns TRUE if at least one window requested more
        // idle events
        //
        rEvent.RequestMore(TRUE);
    }

    //
    // If they are pending events, we must process them.
    //
#if wxUSE_THREADS
    ProcessPendingEvents();
#endif
    sbInOnIdle = FALSE;
}

// Send idle event to all top-level windows
bool wxApp::SendIdleEvents()
{
    bool                            bNeedMore = FALSE;
    wxWindowList::Node*             pNode = wxTopLevelWindows.GetFirst();

    while (pNode)
    {
        wxWindow*                   pWin = pNode->GetData();

        if (SendIdleEvents(pWin))
            bNeedMore = TRUE;
        pNode = pNode->GetNext();
    }
    return bNeedMore;
}

//
// Send idle event to window and all subwindows
//
bool wxApp::SendIdleEvents(
  wxWindow*                         pWin
)
{
    bool                            bNeedMore = FALSE;
    wxIdleEvent                     vEvent;

    vEvent.SetEventObject(pWin);
    pWin->GetEventHandler()->ProcessEvent(vEvent);

    if (vEvent.MoreRequested())
        bNeedMore = TRUE;

    wxNode*                         pNode = pWin->GetChildren().First();

    while (pNode)
    {
        wxWindow*                   pWin = (wxWindow*) pNode->Data();

        if (SendIdleEvents(pWin))
            bNeedMore = TRUE;
        pNode = pNode->Next();
    }
    return bNeedMore;
}

void wxApp::DeletePendingObjects()
{
    wxNode*                         pNode = wxPendingDelete.First();

    while (pNode)
    {
        wxObject*                   pObj = (wxObject *)pNode->Data();

        delete pObj;

        if (wxPendingDelete.Member(pObj))
            delete pNode;

        //
        // Deleting one object may have deleted other pending
        // objects, so start from beginning of list again.
        //
        pNode = wxPendingDelete.First();
    }
}

void wxApp::OnEndSession(
  wxCloseEvent&                     WXUNUSED(rEvent))
{
    if (GetTopWindow())
        GetTopWindow()->Close(TRUE);
}

//
// Default behaviour: close the application with prompts. The
// user can veto the close, and therefore the end session.
//
void wxApp::OnQueryEndSession(
  wxCloseEvent&                     rEvent
)
{
    if (GetTopWindow())
    {
        if (!GetTopWindow()->Close(!rEvent.CanVeto()))
            rEvent.Veto(TRUE);
    }
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

wxIcon wxApp::GetStdIcon(
  int                               nWhich
) const
{
    switch(nWhich)
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


