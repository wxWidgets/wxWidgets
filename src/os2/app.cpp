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

#ifdef __GNUG__
    #pragma implementation "app.h"
#endif

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

#ifdef __EMX__

#include <sys/ioctl.h>
#include <sys/select.h>

#else

#include <nerrno.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <sys/time.h>

#endif //

#ifndef __EMX__

#define select(a,b,c,d,e) bsdselect(a,b,c,d,e)
extern "C" int _System bsdselect(int,
                                 struct fd_set *,
                                 struct fd_set *,
                                 struct fd_set *,
                                 struct timeval *);
#endif

#if wxUSE_THREADS
    #include "wx/thread.h"

    // define the array of QMSG strutures
    WX_DECLARE_OBJARRAY(QMSG, wxMsgArray);

    #include "wx/arrimpl.cpp"

    WX_DEFINE_OBJARRAY(wxMsgArray);
#endif // wxUSE_THREADS

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
// helper struct and functions for socket handling
// ---------------------------------------------------------------------------

struct GsocketCallbackInfo{
    void (*proc)(void *);
    int type;
    int handle;
    void* gsock;
};

// These defines and wrapper functions are used here and in gsockpm.c
#define wxSockReadMask  0x01
#define wxSockWriteMask 0x02

#ifdef __EMX__
extern "C"
int wxAppAddSocketHandler(int handle, int mask,
                           void (*callback)(void*), void * gsock)
{
    return wxTheApp->AddSocketHandler(handle, mask, callback, gsock);
}
extern "C"
void wxAppRemoveSocketHandler(int handle)
{
    wxTheApp->RemoveSocketHandler(handle);
}
#else
//  Linkage mode problems using callbacks with extern C in a .cpp module
int wxAppAddSocketHandler(int handle, int mask,
                           void (*callback)(void*), void * gsock)
{
    return wxTheApp->AddSocketHandler(handle, mask, callback, gsock);
}
void wxAppRemoveSocketHandler(int handle)
{
    wxTheApp->RemoveSocketHandler(handle);
}
#endif

void wxApp::HandleSockets()
{
    bool pendingEvent = FALSE;

    // Check whether it's time for Gsocket operation
    if (m_maxSocketHandles > 0 && m_maxSocketNr > 0)
    {
        fd_set readfds = m_readfds;
        fd_set writefds = m_writefds;
        struct timeval timeout;
        int i;
        struct GsocketCallbackInfo
          *CallbackInfo = (struct GsocketCallbackInfo *)m_sockCallbackInfo;
        int r = 0;
        timeout.tv_sec = 0;
        timeout.tv_usec = 0;
        if ( select(m_maxSocketNr, &readfds, &writefds, 0, &timeout) > 0)
        {
            for (i = m_lastUsedHandle + 1; i != m_lastUsedHandle; i++)
            {
                if (i == m_maxSocketNr)
                    i = 0;
                if (FD_ISSET(i, &readfds))
                {
                    int r;
                    for (r = 0; r < m_maxSocketHandles; r++){
                        if(CallbackInfo[r].handle == i &&
                           CallbackInfo[r].type == wxSockReadMask)
                            break;
                    }
                    if (r < m_maxSocketHandles)
                    {
                        CallbackInfo[r].proc(CallbackInfo[r].gsock);
                        pendingEvent = TRUE;
                        wxYield();
                    }
                }
                if (FD_ISSET(i, &writefds))
                {
                    int r;
                    for (r = 0; r < m_maxSocketHandles; r++)
                        if(CallbackInfo[r].handle == i &&
                           CallbackInfo[r].type == wxSockWriteMask)
                            break;
                    if (r < m_maxSocketHandles)
                    {
                        CallbackInfo[r].proc(CallbackInfo[r].gsock);
                        pendingEvent = TRUE;
                        wxYield();
                    }
                }
            }
            m_lastUsedHandle = i;
        }
        if (pendingEvent)
            wxYield();
    }
}
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
bool wxApp::Initialize(int& argc, wxChar **argv)
{
    if ( !wxAppBase::Initialize(argc, argv) )
        return false;

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
    vHabmain = WinInitialize(0);

    if (!vHabmain)
    {
        // TODO: at least give some error message here...
        wxAppBase::CleanUp();

        return FALSE;
    }

    wxBuffer = new wxChar[1500]; // FIXME; why?

    // Some people may wish to use this, but
    // probably it shouldn't be here by default.
#ifdef __WXDEBUG__
    //    wxRedirectIOToConsole();
#endif

    wxWinHandleList = new wxList(wxKEY_INTEGER);

    // This is to foil optimizations in Visual C++ that throw out dummy.obj.
    // PLEASE DO NOT ALTER THIS.
#if !defined(WXMAKINGDLL) && defined(__VISAGECPP__)
    extern char wxDummyChar;
    if (wxDummyChar) wxDummyChar++;
#endif

    // wxSetKeyboardHook(TRUE);

    RegisterWindowClasses(vHab);

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
    ERRORID                         vError = 0L;
    wxString                        sError;

    if (!::WinRegisterClass( vHab
                            ,wxFrameClassName
                            ,wxFrameWndProc
                            ,CS_SIZEREDRAW | CS_SYNCPAINT
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
                            ,CS_SIZEREDRAW | CS_HITTEST | CS_SYNCPAINT
                            ,sizeof(ULONG)
                           ))
    {
        vError = ::WinGetLastError(vHab);
        sError = wxPMErrorToStr(vError);
        wxLogLastError(sError);
        return FALSE;
    }
    if (!::WinRegisterClass( vHab
                            ,wxCanvasClassNameNR
                            ,wxWndProc
                            ,CS_HITTEST | CS_SYNCPAINT
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

    // Delete Message queue
    if (wxTheApp->m_hMq)
        ::WinDestroyMsgQueue(wxTheApp->m_hMq);

    wxAppBase::CleanUp();
} // end of wxApp::CleanUp

bool wxApp::OnInitGui()
{
    ERRORID                         vError;
    wxString                        sError;

    if (!wxAppBase::OnInitGui())
        return FALSE;

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

wxApp::wxApp()
{
    m_topWindow = NULL;
    wxTheApp = this;

    argc = 0;
    argv = NULL;
    m_nPrintMode = wxPRINT_WINDOWS;
    m_bAuto3D = TRUE;
    m_hMq = 0;
    m_maxSocketHandles = 0;
    m_maxSocketNr = 0;
    m_sockCallbackInfo = 0;
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

                    DoMessage((WXMSG*)&vMsg);
                }
                svSavedMessages.Empty();
            }
        }
#endif // wxUSE_THREADS

        //
        // Process the message
        //
        DoMessage((WXMSG *)&svCurrentMsg);
    }
    return TRUE;
} // end of wxApp::DoMessage

void wxApp::DoMessage(
  WXMSG*                            pMsg
)
{
    if (!ProcessMessage((WXMSG *)&svCurrentMsg))
    {
        ::WinDispatchMsg(vHabmain, (PQMSG)&svCurrentMsg);
    }
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
        while (!Pending() && ProcessIdle())
        {
            HandleSockets();
            wxUsleep(10000);
        }
        HandleSockets();
        if (Pending())
            DoMessage();
        else
            wxUsleep(10000);

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
    ::WinPostMsg(NULL, WM_QUIT, 0, 0);
} // end of wxApp::ExitMainLoop

bool wxApp::Pending()
{
    return (::WinPeekMsg(vHabmain, (PQMSG)&svCurrentMsg, (HWND)NULL, 0, 0, PM_NOREMOVE) != 0);
} // end of wxApp::Pending

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

    //
    // Pass non-system timer messages to the wxTimerProc
    //
    if (pMsg->msg == WM_TIMER &&
        (SHORT1FROMMP(pMsg->mp1) != TID_CURSOR &&
         SHORT1FROMMP(pMsg->mp1) != TID_FLASHWINDOW &&
         SHORT1FROMMP(pMsg->mp1) != TID_SCROLL &&
         SHORT1FROMMP(pMsg->mp1) != 0x0000
        ))
        wxTimerProc(NULL, 0, (int)pMsg->mp1, 0);

    //
    // Allow the window to prevent certain messages from being
    // translated/processed (this is currently used by wxTextCtrl to always
    // grab Ctrl-C/V/X, even if they are also accelerators in some parent)
    //
    if (pWndThis && !pWndThis->OS2ShouldPreProcessMessage(pWxmsg))
    {
        return FALSE;
    }

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

bool                                gbInOnIdle = FALSE;

void wxApp::OnIdle(
  wxIdleEvent&                      rEvent
)
{

    //
    // Avoid recursion (via ProcessEvent default case)
    //
    if (gbInOnIdle)
        return;

    gbInOnIdle = TRUE;

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

#if wxUSE_DC_CACHEING
    // automated DC cache management: clear the cached DCs and bitmap
    // if it's likely that the app has finished with them, that is, we
    // get an idle event and we're not dragging anything.
    if (!::WinGetKeyState(HWND_DESKTOP, VK_BUTTON1) &&
        !::WinGetKeyState(HWND_DESKTOP, VK_BUTTON3) &&
        !::WinGetKeyState(HWND_DESKTOP, VK_BUTTON2))
        wxDC::ClearCache();
#endif // wxUSE_DC_CACHEING

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
    gbInOnIdle = FALSE;
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

void wxApp::Exit()
{
    wxApp::CleanUp();

    // VZ: must really exit somehow, insert appropriate OS/2 syscall (FIXME)
    wxAppConsole::Exit();
} // end of wxExit

//
// Yield to incoming messages
//
bool wxApp::Yield(bool onlyIfNeeded)
{
    static bool s_inYield = FALSE;

    if ( s_inYield )
    {
        if ( !onlyIfNeeded )
        {
            wxFAIL_MSG( _T("wxYield() called recursively") );
        }

        return FALSE;
    }

    HAB                             vHab = 0;
    QMSG                            vMsg;

    //
    // Disable log flushing from here because a call to wxYield() shouldn't
    // normally result in message boxes popping up &c
    //
    wxLog::Suspend();

    s_inYield = TRUE;

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
    s_inYield = FALSE;
    return TRUE;
} // end of wxYield

int wxApp::AddSocketHandler(int handle, int mask,
                            void (*callback)(void*), void * gsock)
{
    int find;
    struct GsocketCallbackInfo
        *CallbackInfo = (struct GsocketCallbackInfo *)m_sockCallbackInfo;

    for (find = 0; find < m_maxSocketHandles; find++)
        if (CallbackInfo[find].handle == -1)
            break;
    if (find == m_maxSocketHandles)
    {
        // Allocate new memory
        m_sockCallbackInfo = realloc(m_sockCallbackInfo,
                                     (m_maxSocketHandles+=10)*
                                     sizeof(struct GsocketCallbackInfo));
        CallbackInfo = (struct GsocketCallbackInfo *)m_sockCallbackInfo;
        for (find = m_maxSocketHandles - 10; find < m_maxSocketHandles; find++)
            CallbackInfo[find].handle = -1;
        find = m_maxSocketHandles - 10;
    }
    CallbackInfo[find].proc = callback;
    CallbackInfo[find].type = mask;
    CallbackInfo[find].handle = handle;
    CallbackInfo[find].gsock = gsock;
    if (mask & wxSockReadMask)
        FD_SET(handle, &m_readfds);
    if (mask & wxSockWriteMask)
        FD_SET(handle, &m_writefds);
    if (handle >= m_maxSocketNr)
        m_maxSocketNr = handle + 1;
    return find;
}

void wxApp::RemoveSocketHandler(int handle)
{
    struct GsocketCallbackInfo
        *CallbackInfo = (struct GsocketCallbackInfo *)m_sockCallbackInfo;
    if (handle < m_maxSocketHandles)
    {
        if (CallbackInfo[handle].type & wxSockReadMask)
            FD_CLR(CallbackInfo[handle].handle, &m_readfds);
        if (CallbackInfo[handle].type & wxSockWriteMask)
            FD_CLR(CallbackInfo[handle].handle, &m_writefds);
        CallbackInfo[handle].handle = -1;
    }
}

//-----------------------------------------------------------------------------
// wxWakeUpIdle
//-----------------------------------------------------------------------------

void wxApp::WakeUpIdle()
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

