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
    #include "wx/wxchar.h"
    #include "wx/icon.h"
    #include "wx/timer.h"
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

#if wxUSE_TOOLTIPS
    #include "wx/tooltip.h"
#endif // wxUSE_TOOLTIPS

#include <string.h>
#include <ctype.h>

// ---------------------------------------------------------------------------
// global variables
// ---------------------------------------------------------------------------

extern wxChar*                      wxBuffer;
extern wxList*                      wxWinHandleList;
extern wxList WXDLLEXPORT           wxPendingDelete;
extern wxCursor*                    g_globalCursor;

HAB                                 vHabmain = NULLHANDLE;
QMSG                                svCurrentMsg;
wxApp*                              wxTheApp = NULL;

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

MRESULT EXPENTRY wxWndProc( HWND hWnd,ULONG message,MPARAM mp1,MPARAM mp2);
MRESULT EXPENTRY wxFrameWndProc( HWND hWnd,ULONG message,MPARAM mp1,MPARAM mp2);

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

//
// Initialize
//
bool wxApp::Initialize(
  HAB                               vHab
)
{
#if defined(wxUSE_CONSOLEDEBUG)
  #if wxUSE_CONSOLEDEBUG
/***********************************************/
/* Code for using stdout debug                 */
/* To use it you mast link app as "Window" - EK*/
/***********************************************/
  {
     PPIB pib;
     PTIB tib;

    printf("In console\n");

  DosGetInfoBlocks(&tib, &pib);
/* Try morphing into a PM application. */
//  if(pib->pib_ultype == 2)    /* VIO */
    pib->pib_ultype = 3;
   }
/**********************************************/
/**********************************************/
  #endif //wxUSE_CONSOLEDEBUG
#endif

    //
    // OS2 has to have an anchorblock
    //
    vHab = WinInitialize(0);

    if (!vHab)
        return FALSE;
    else
        vHabmain = vHab;

    // Some people may wish to use this, but
    // probably it shouldn't be here by default.
#ifdef __WXDEBUG__
    //    wxRedirectIOToConsole();
#endif

    wxBuffer = new wxChar[1500]; // FIXME; why?

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

    RegisterWindowClasses(vHab);
    wxWinHandleList = new wxList(wxKEY_INTEGER);

    // This is to foil optimizations in Visual C++ that throw out dummy.obj.
    // PLEASE DO NOT ALTER THIS.
#if !defined(WXMAKINGDLL) && defined(__VISAGECPP__)
    extern char wxDummyChar;
    if (wxDummyChar) wxDummyChar++;
#endif

    // wxSetKeyboardHook(TRUE);

    wxModule::RegisterModules();
    if (!wxModule::InitializeModules())
        return FALSE;
    return TRUE;
} // end of wxApp::Initialize

const char*                         CANTREGISTERCLASS = " Can't register Class ";
// ---------------------------------------------------------------------------
// RegisterWindowClasses
// ---------------------------------------------------------------------------

bool wxApp::RegisterWindowClasses(
  HAB                               vHab
)
{
    APIRET                          rc;
    ERRORID                         vError = 0L;
    wxString                        sError;

    if (!::WinRegisterClass( vHab
                            ,wxFrameClassName
                            ,wxFrameWndProc
                            ,CS_SIZEREDRAW | CS_MOVENOTIFY | CS_SYNCPAINT  | CS_CLIPCHILDREN
                            ,sizeof(ULONG)
                           ))
    {
        vError = ::WinGetLastError(vHab);
        sError = wxPMErrorToStr(vError);
        wxLogLastError(sError);
        return FALSE;
    }

    if (!::WinRegisterClass( vHab
                            ,wxFrameClassNameNoRedraw
                            ,wxWndProc
                            ,0
                            ,sizeof(ULONG)
                           ))
    {
        vError = ::WinGetLastError(vHab);
        sError = wxPMErrorToStr(vError);
        wxLogLastError(sError);
        return FALSE;
    }

    if (!::WinRegisterClass( vHab
                            ,wxMDIFrameClassName
                            ,wxWndProc
                            ,CS_SIZEREDRAW | CS_MOVENOTIFY | CS_SYNCPAINT
                            ,sizeof(ULONG)
                           ))
    {
        vError = ::WinGetLastError(vHab);
        sError = wxPMErrorToStr(vError);
        wxLogLastError(sError);
        return FALSE;
    }

    if (!::WinRegisterClass( vHab
                            ,wxMDIFrameClassNameNoRedraw
                            ,wxWndProc
                            ,0
                            ,sizeof(ULONG)
                           ))
    {
        vError = ::WinGetLastError(vHab);
        sError = wxPMErrorToStr(vError);
        wxLogLastError(sError);
        return FALSE;
    }

    if (!::WinRegisterClass( vHab
                            ,wxMDIChildFrameClassName
                            ,wxWndProc
                            ,CS_MOVENOTIFY | CS_SIZEREDRAW | CS_SYNCPAINT | CS_HITTEST
                            ,sizeof(ULONG)
                           ))
    {
        vError = ::WinGetLastError(vHab);
        sError = wxPMErrorToStr(vError);
        wxLogLastError(sError);
        return FALSE;
    }

    if (!::WinRegisterClass( vHab
                            ,wxMDIChildFrameClassNameNoRedraw
                            ,wxWndProc
                            ,CS_HITTEST
                            ,sizeof(ULONG)
                           ))
    {
        vError = ::WinGetLastError(vHab);
        sError = wxPMErrorToStr(vError);
        wxLogLastError(sError);
        return FALSE;
    }

    if (!::WinRegisterClass( vHab
                            ,wxPanelClassName
                            ,wxWndProc
                            ,CS_MOVENOTIFY | CS_SIZEREDRAW | CS_HITTEST | CS_SAVEBITS | CS_SYNCPAINT
                            ,sizeof(ULONG)
                           ))
    {
        vError = ::WinGetLastError(vHab);
        sError = wxPMErrorToStr(vError);
        wxLogLastError(sError);
        return FALSE;
    }

    if (!::WinRegisterClass( vHab
                            ,wxCanvasClassName
                            ,wxWndProc
                            ,CS_MOVENOTIFY | CS_SIZEREDRAW | CS_HITTEST | CS_SAVEBITS | CS_SYNCPAINT | CS_CLIPCHILDREN
                            ,sizeof(ULONG)
                           ))
    {
        vError = ::WinGetLastError(vHab);
        sError = wxPMErrorToStr(vError);
        wxLogLastError(sError);
        return FALSE;
    }
    return TRUE;
} // end of wxApp::RegisterWindowClasses

//
// Cleans up any wxWindows internal structures left lying around
//
void wxApp::CleanUp()
{
    //
    // COMMON CLEANUP
    //

#if wxUSE_LOG

    //
    // Flush the logged messages if any and install a 'safer' log target: the
    // default one (wxLogGui) can't be used after the resources are freed just
    // below and the user suppliedo ne might be even more unsafe (using any
    // wxWindows GUI function is unsafe starting from now)
    //
    wxLog::DontCreateOnDemand();

    //
    // This will flush the old messages if any
    //
    delete wxLog::SetActiveTarget(new wxLogStderr);
#endif // wxUSE_LOG

    //
    // One last chance for pending objects to be cleaned up
    //
    wxTheApp->DeletePendingObjects();

    wxModule::CleanUpModules();

#if wxUSE_WX_RESOURCES
    wxCleanUpResourceSystem();
#endif

    wxDeleteStockObjects();

    //
    // Destroy all GDI lists, etc.
    //
    wxDeleteStockLists();

    delete wxTheColourDatabase;
    wxTheColourDatabase = NULL;

    wxBitmap::CleanUpHandlers();

    delete[] wxBuffer;
    wxBuffer = NULL;

    //
    // PM-SPECIFIC CLEANUP
    //

    // wxSetKeyboardHook(FALSE);

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

    delete wxPendingEvents;
#if wxUSE_THREADS
    delete wxPendingEventsLocker;
    // If we don't do the following, we get an apparent memory leak.
    ((wxEvtHandler&) wxDefaultValidator).ClearEventLocker();
#endif

    wxClassInfo::CleanUpClasses();

    // Delete Message queue
    if (wxTheApp->m_hMq)
        ::WinDestroyMsgQueue(wxTheApp->m_hMq);

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
} // end of wxApp::CleanUp

//----------------------------------------------------------------------
// Main wxWindows entry point
//----------------------------------------------------------------------
int wxEntry(
  int                               argc
, char*                             argv[]
)
{
    HAB                             vHab = 0;

    if (!wxApp::Initialize(vHab))
        return 0;

    //
    // create the application object or ensure that one already exists
    //
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
            nRetValue = wxTheApp->OnRun();
        }
        // Normal exit
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
    else // app initialization failed
    {
        wxLogLastError(" Gui initialization failed, exitting");
    }
#if wxUSE_CONSOLEDEBUG
    printf("wxTheApp->OnExit ");
    fflush(stdout);
#endif
    wxTheApp->OnExit();
#if wxUSE_CONSOLEDEBUG
    printf("wxApp::CleanUp ");
    fflush(stdout);
#endif
    wxApp::CleanUp();
#if wxUSE_CONSOLEDEBUG
    printf("return %i ", nRetValue);
    fflush(stdout);
#endif
    return(nRetValue);
} // end of wxEntry

bool wxApp::OnInitGui()
{
    ERRORID                         vError;
    wxString                        sError;

    m_hMq = ::WinCreateMsgQueue(vHabmain, 0);
    if (!m_hMq)
    {
        vError = ::WinGetLastError(vHabmain);
        sError = wxPMErrorToStr(vError);
        wxLogDebug(sError);
        return FALSE;
    }
    return TRUE;
} // end of wxApp::OnInitGui

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
    m_hMq = 0;
} // end of wxApp::wxApp

wxApp::~wxApp()
{
    //
    // Delete command-line args
    //
#if wxUSE_UNICODE
    int                             i;

    for (i = 0; i < argc; i++)
    {
        delete[] argv[i];
    }
    delete[] argv;
#endif
} // end of wxApp::~wxApp

bool wxApp::Initialized()
{
    if (GetTopWindow())
        return TRUE;
    else
        return FALSE;
} // end of wxApp::Initialized

//
// Get and process a message, returning FALSE if WM_QUIT
// received (and also set the flag telling the app to exit the main loop)
//
bool wxApp::DoMessage()
{
    BOOL                            bRc = ::WinGetMsg(vHabmain, &svCurrentMsg, HWND(NULL), 0, 0);

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
        // If a secondary thread owns is doing GUI calls, save all messages for
        // later processing - we can't process them right now because it will
        // lead to recursive library calls (and we're not reentrant)
        //
        if (!wxGuiOwnedByMainThread())
        {
            sbHadGuiLock = FALSE;

            //
            // Leave out WM_COMMAND messages: too dangerous, sometimes
            // the message will be processed twice
            //
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
            // Have we just regained the GUI lock? if so, post all of the saved
            // messages
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
                        ::WinDispatchMsg(vHabmain, &vMsg);
                    }
                }
                svSavedMessages.Empty();
            }
        }
#endif // wxUSE_THREADS

        // Process the message
        if (!ProcessMessage((WXMSG *)&svCurrentMsg))
        {
            ::WinDispatchMsg(vHabmain, (PQMSG)&svCurrentMsg);
        }
    }
    return TRUE;
} // end of wxApp::DoMessage

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
        while (/*Pending() &&*/ ProcessIdle())
        {
//          wxUsleep(10000);
        }
        DoMessage();
    }
    return (int)svCurrentMsg.mp1;
} // end of wxApp::MainLoop

//
// Returns TRUE if more time is needed.
//
bool wxApp::ProcessIdle()
{
    wxIdleEvent                     vEvent;

    vEvent.SetEventObject(this);
    ProcessEvent(vEvent);
    return vEvent.MoreRequested();
} // end of wxApp::ProcessIdle

void wxApp::ExitMainLoop()
{
    m_bKeepGoing = FALSE;
}

bool wxApp::Pending()
{
    return (::WinPeekMsg(vHabmain, (PQMSG)&svCurrentMsg, (HWND)NULL, 0, 0, PM_NOREMOVE) != 0);
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
    QMSG*                           pMsg = (PQMSG)pWxmsg;
    HWND                            hWnd = pMsg->hwnd;
    wxWindow*                       pWndThis = wxFindWinFromHandle((WXHWND)hWnd);
    wxWindow*                       pWnd;

#if wxUSE_TOOLTIPS
    //
    // We must relay WM_MOUSEMOVE events to the tooltip ctrl if we want it to
    // popup the tooltip bubbles
    //
    if (pWndThis && (pMsg->msg == WM_MOUSEMOVE))
    {
        wxToolTip*                  pToolTip = pWndThis->GetToolTip();
        if (pToolTip)
        {
            pToolTip->RelayEvent(pWxmsg);
        }
    }
#endif // wxUSE_TOOLTIPS

    //
    // We must relay Timer events to wxTimer's processing function
    //
    if (pMsg->msg == WM_TIMER)
        wxTimerProc(NULL, 0, (int)pMsg->mp1, 0);

    //
    // For some composite controls (like a combobox), wndThis might be NULL
    // because the subcontrol is not a wxWindow, but only the control itself
    // is - try to catch this case
    //
    while (hWnd && !pWndThis)
    {
        hWnd = ::WinQueryWindow(hWnd, QW_PARENT);
        pWndThis = wxFindWinFromHandle((WXHWND)hWnd);
    }

    //
    // Try translations first; find the youngest window with
    // a translation table. OS/2 has case sensative accels, so
    // this block, coded by BK, removes that and helps make them
    // case insensative.
    //
    if(pMsg->msg == WM_CHAR)
    {
       PBYTE                        pChmsg = (PBYTE)&(pMsg->msg);
       USHORT                       uSch  = CHARMSG(pChmsg)->chr;
       bool                         bRc;

       //
       // Do not process keyup events
       //
       if(!(CHARMSG(pChmsg)->fs & KC_KEYUP))
       {
           if((CHARMSG(pChmsg)->fs & (KC_ALT | KC_CTRL)) && CHARMSG(pChmsg)->chr != 0)
                CHARMSG(pChmsg)->chr = (USHORT)wxToupper((UCHAR)uSch);


           for(pWnd = pWndThis; pWnd; pWnd = pWnd->GetParent() )
           {
               if((bRc = pWnd->OS2TranslateMessage(pWxmsg)) == TRUE)
                   break;
           }

            if(!bRc)    // untranslated, should restore original value
                CHARMSG(pChmsg)->chr = uSch;
        }
    }
    //
    // Anyone for a non-translation message? Try youngest descendants first.
    //
//  for (pWnd = pWndThis; pWnd; pWnd = pWnd->GetParent())
//  {
//      if (pWnd->OS2ProcessMessage(pWxmsg))
//          return TRUE;
//  }
    return FALSE;
} // end of wxApp::ProcessMessage

void wxApp::OnIdle(
  wxIdleEvent&                      rEvent
)
{
    static bool                     sbInOnIdle = FALSE;

    //
    // Avoid recursion (via ProcessEvent default case)
    //
    if (sbInOnIdle)
        return;

    sbInOnIdle = TRUE;

    //
    // If there are pending events, we must process them: pending events
    // are either events to the threads other than main or events posted
    // with wxPostEvent() functions
    //
    ProcessPendingEvents();

    //
    // 'Garbage' collection of windows deleted with Close().
    //
    DeletePendingObjects();

#if wxUSE_LOG
    //
    // Flush the logged messages if any
    //
    wxLog::FlushActive();
#endif // wxUSE_LOG

    //
    // Send OnIdle events to all windows
    //
    if (SendIdleEvents())
    {
        //
        // SendIdleEvents() returns TRUE if at least one window requested more
        // idle events
        //
        rEvent.RequestMore(TRUE);
    }
    sbInOnIdle = FALSE;
} // end of wxApp::OnIdle

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
} // end of wxApp::SendIdleEvents

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
} // end of wxApp::SendIdleEvents

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
} // end of wxApp::DeletePendingObjects

void wxApp::OnEndSession(
  wxCloseEvent&                     WXUNUSED(rEvent))
{
    if (GetTopWindow())
        GetTopWindow()->Close(TRUE);
} // end of wxApp::OnEndSession

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
} // end of wxApp::OnQueryEndSession

void wxExit()
{
    wxLogError(_("Fatal error: exiting"));

    wxApp::CleanUp();
} // end of wxExit

static bool gs_inYield = FALSE;

//
// Yield to incoming messages
//
bool wxYield()
{
    HAB                             vHab = 0;
    QMSG                            vMsg;

    //
    // Disable log flushing from here because a call to wxYield() shouldn't
    // normally result in message boxes popping up &c
    //
    wxLog::Suspend();

    gs_inYield = TRUE;

    //
    // We want to go back to the main message loop
    // if we see a WM_QUIT. (?)
    //
    while (::WinPeekMsg(vHab, &vMsg, (HWND)NULL, 0, 0, PM_NOREMOVE) && vMsg.msg != WM_QUIT)
    {
#if wxUSE_THREADS
        wxMutexGuiLeaveOrEnter();
#endif // wxUSE_THREADS
        if (!wxTheApp->DoMessage())
            break;
    }
    //
    // If they are pending events, we must process them.
    //
    if (wxTheApp)
        wxTheApp->ProcessPendingEvents();

    //
    // Let the logs be flashed again
    //
    wxLog::Resume();
    gs_inYield = FALSE;
    return TRUE;
} // end of wxYield

// Yield to incoming messages; but fail silently if recursion is detected.
bool wxYieldIfNeeded()
{
    if (gs_inYield)
        return FALSE;
        
    return wxYield();
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
} // end of wxApp::GetStdIcon

//-----------------------------------------------------------------------------
// wxWakeUpIdle
//-----------------------------------------------------------------------------

void wxWakeUpIdle()
{
    //
    // Send the top window a dummy message so idle handler processing will
    // start up again.  Doing it this way ensures that the idle handler
    // wakes up in the right thread (see also wxWakeUpMainThread() which does
    // the same for the main app thread only)
    //
    wxWindow*                       pTopWindow = wxTheApp->GetTopWindow();

    if (pTopWindow)
    {
        if ( !::WinPostMsg(GetHwndOf(pTopWindow), WM_NULL, (MPARAM)0, (MPARAM)0))
        {
            //
            // Should never happen
            //
            wxLogLastError("PostMessage(WM_NULL)");
        }
    }
} // end of wxWakeUpIdle

HAB wxGetInstance()
{
    return vHabmain;
}

void wxSetInstance(
  HAB                               vHab
)
{
    vHabmain = vHab;
}

