/////////////////////////////////////////////////////////////////////////////
// Name:        dialog.cpp
// Purpose:     wxDialog class
// Author:      David Webster
// Modified by:
// Created:     10/14/99
// RCS-ID:      $Id$
// Copyright:   (c) David Webster
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/dialog.h"
#include "wx/utils.h"
#include "wx/frame.h"
#include "wx/app.h"
#include "wx/settings.h"
#include "wx/intl.h"
#include "wx/log.h"
#endif

#include "wx/os2/private.h"
#include "wx/log.h"

#define wxDIALOG_DEFAULT_X 300
#define wxDIALOG_DEFAULT_Y 300

#define wxDIALOG_DEFAULT_WIDTH 500
#define wxDIALOG_DEFAULT_HEIGHT 500

// Lists to keep track of windows, so we can disable/enable them
// for modal dialogs
wxWindowList wxModalDialogs;
wxWindowList wxModelessWindows;  // Frames and modeless dialogs
extern wxList WXDLLEXPORT wxPendingDelete;

   IMPLEMENT_DYNAMIC_CLASS(wxDialog, wxPanel)

   BEGIN_EVENT_TABLE(wxDialog, wxPanel)
      EVT_SIZE(wxDialog::OnSize)
      EVT_BUTTON(wxID_OK, wxDialog::OnOK)
      EVT_BUTTON(wxID_APPLY, wxDialog::OnApply)
      EVT_BUTTON(wxID_CANCEL, wxDialog::OnCancel)
      EVT_CHAR_HOOK(wxDialog::OnCharHook)
      EVT_SYS_COLOUR_CHANGED(wxDialog::OnSysColourChanged)
      EVT_CLOSE(wxDialog::OnCloseWindow)
   END_EVENT_TABLE()

void wxDialog::Init()
{
    m_pOldFocus = (wxWindow *)NULL;
    m_isShown = FALSE;
    m_pWindowDisabler = (wxWindowDisabler *)NULL;
    SetBackgroundColour(wxSystemSettings::GetSystemColour(wxSYS_COLOUR_3DFACE));
} // end of wxDialog::Init

bool wxDialog::Create(
  wxWindow*                         pParent
, wxWindowID                        vId
, const wxString&                   rsTitle
, const wxPoint&                    rPos
, const wxSize&                     rSize
, long                              lStyle
, const wxString&                   rsName
)
{
    long                            lX = rPos.x;
    long                            lY = rPos.y;
    long                            lWidth = rSize.x;
    long                            lHeight = rSize.y;
    const char*                     zDlg;
    WXDWORD                         dwExtendedStyle = 0L;
    HWND                            hWnd;

    Init();
    m_pOldFocus = (wxWindow*)FindFocus();
    SetName(rsName);
    wxTopLevelWindows.Append(this);
    if (pParent)
        pParent->AddChild(this);
    if (vId == -1)
        m_windowId = NewControlId();
    else
        m_windowId = vId;
    if (lX < 0)
        lX = wxDIALOG_DEFAULT_X;
    if (lY < 0)
        lY = wxDIALOG_DEFAULT_Y;
    m_windowStyle = lStyle;
    if (lWidth < 0)
        lWidth = wxDIALOG_DEFAULT_WIDTH;
    if (lHeight < 0)
        lHeight = wxDIALOG_DEFAULT_HEIGHT;

    //
    // All dialogs should really have this style
    //
    m_windowStyle |= wxTAB_TRAVERSAL;

    //
    // Allows creation of dialogs with & without captions under MSWindows,
    // resizeable or not (but a resizeable dialog always has caption -
    // otherwise it would look too strange)
    //
    if (lStyle & wxRESIZE_BORDER )
        zDlg = "wxResizeableDialog";
    else if (lStyle & wxCAPTION )
        zDlg = "wxCaptionDialog";
    else
        zDlg = "wxNoCaptionDialog";
    OS2Create( GetWinHwnd(pParent)
              ,NULL
              ,rsTitle.c_str()
              ,0L
              ,lX
              ,lY
              ,lWidth
              ,lHeight
              ,GetWinHwnd(pParent)
              ,HWND_TOP
              ,(long)m_windowId
              ,NULL
              ,NULL
             );
    hWnd = (HWND)GetHWND();
    if (!hWnd)
    {
        return FALSE;
    }
    SubclassWin(GetHWND());
    ::WinSetWindowText( hWnd
                       ,(PSZ)rsTitle.c_str()
                      );
    SetFont(wxSystemSettings::GetSystemFont(wxSYS_DEFAULT_GUI_FONT));
    return TRUE;
} // end of wxDialog::Create

void wxDialog::SetModal(
  bool                              bFlag
)
{
    if (bFlag)
        m_windowStyle |= wxDIALOG_MODAL ;
    else if ( m_windowStyle & wxDIALOG_MODAL )
        m_windowStyle -= wxDIALOG_MODAL ;

      wxModelessWindows.DeleteObject(this);
      if (!bFlag)
          wxModelessWindows.Append(this);
} // end of wxDialog::SetModal

wxDialog::~wxDialog()
{
    m_isBeingDeleted = TRUE;
    wxTopLevelWindows.DeleteObject(this);
    Show(FALSE);
    if (!IsModal)
        wxModelessWindows.DeleteObject(this);

    //
    // If this is the last top-level window, exit.
    //
    if (wxTheApp && (wxTopLevelWindows.Number() == 0))
    {
        wxTheApp->SetTopWindow(NULL);

        if (wxTheApp->GetExitOnFrameDelete())
        {
            ::WinPostMsg(GetHwnd(), WM_QUIT, 0, 0);
        }
    }
} // end of wxDialog::~wxDialog

//
// By default, pressing escape cancels the dialog
//
void wxDialog::OnCharHook(
  wxKeyEvent&                       rEvent
)
{
    if (GetHWND())
    {
        if (rEvent.m_keyCode == WXK_ESCAPE)
        {
            //
            // Behaviour changed in 2.0: we'll send a Cancel message
            // to the dialog instead of Close.
            //
            wxCommandEvent          vCancelEvent( wxEVT_COMMAND_BUTTON_CLICKED
                                                 ,wxID_CANCEL
                                                );

            vCancelEvent.SetEventObject( this );
            GetEventHandler()->ProcessEvent(vCancelEvent);

            //
            // Ensure that there is another message for this window so the
            // ShowModal loop will exit and won't get stuck in GetMessage().
            //
            ::WinPostMsg(GetHwnd(), WM_NULL, 0, 0);
            return;
        }
    }
    // We didn't process this event.
    rEvent.Skip();
}

void wxDialog::Iconize(
  bool                              WXUNUSED(bIconize)
)
{
} // end of wxDialog::Iconize

bool wxDialog::IsIconized() const
{
    return FALSE;
} // end of wxDialog::IsIconized

void wxDialog::DoSetClientSize(
  int                               nWidth
, int                               nHeight
)
{
    HWND                            hWnd = (HWND) GetHWND();
    RECTL                           vRect;
    RECTL                           vRect2;

    ::WinQueryWindowRect(hWnd, &vRect);
    ::WinQueryWindowRect(hWnd, &vRect2);

    LONG                            lActualWidth = vRect2.xRight - vRect2.xLeft - vRect.xRight + nWidth;
    LONG                            lActualHeight = vRect2.yTop + vRect2.yTop - vRect.yTop + nHeight;

    ::WinSetWindowPos( GetHwnd()
                      ,HWND_TOP
                      ,(LONG)vRect2.xLeft
                      ,(LONG)vRect2.yTop
                      ,(LONG)lActualWidth
                      ,(LONG)lActualHeight
                      ,SWP_SIZE | SWP_MOVE
                     );

    wxSizeEvent                     vEvent( wxSize( lActualWidth
                                                   ,lActualHeight
                                                  )
                                           ,m_windowId
                                          );

    vEvent.SetEventObject( this );
    GetEventHandler()->ProcessEvent(vEvent);
} // end of wxDialog::DoSetClientSize

void wxDialog::DoGetPosition(
  int*                              pnX
, int*                              pnY
) const
{
    RECTL                           vRect;

    ::WinQueryWindowRect(GetHwnd(), &vRect);
    if (pnX)
        *pnX = vRect.xLeft;
    if (pnY)
        *pnY = vRect.yBottom; // OS/2's bottom is windows' top???
} // end of wxDialog::DoGetPosition

bool wxDialog::IsModal() const
{
    return (GetWindowStyleFlag() & wxDIALOG_MODAL) != 0;
} // end of wxDialog::IsModal

bool wxDialog::IsModalShowing() const
{
    return wxModalDialogs.Find((wxDialog *)this) != NULL; // const_cast
} // end of wxDialog::IsModalShowing

void wxDialog::DoShowModal()
{
    wxWindow*                       pParent = GetParent();
    wxWindow*                       pOldFocus = m_pOldFocus;
    HWND                            hWndOldFocus = 0;

    wxCHECK_RET( !IsModalShowing(), _T("DoShowModal() called twice") );
    wxCHECK_RET( IsModal(), _T("can't DoShowModal() modeless dialog") );

    wxModalDialogs.Append(this);
    if (pOldFocus)
        hWndOldFocus = (HWND)pOldFocus->GetHWND();

    //
    // Remember where the focus was
    //
    if (!pOldFocus)
    {
        pOldFocus = pParent;
        if (pParent)
            hWndOldFocus = GetHwndOf(pParent);
    }

    //
    // Disable all other app windows
    //
    wxASSERT_MSG(!m_pWindowDisabler, _T("disabling windows twice?"));

    m_pWindowDisabler = new wxWindowDisabler(this);

    //
    // Enter the modal loop
    //
    while ( IsModalShowing() )
    {
#if wxUSE_THREADS
        wxMutexGuiLeaveOrEnter();
#endif // wxUSE_THREADS

        while ( !wxTheApp->Pending() && wxTheApp->ProcessIdle() )
            ;

        // a message came or no more idle processing to do
        wxTheApp->DoMessage();
    }

    //
    // Snd restore focus
    // Note that this code MUST NOT access the dialog object's data
    // in case the object has been deleted (which will be the case
    // for a modal dialog that has been destroyed before calling EndModal).
    //
    if (pOldFocus && (pOldFocus != this) && ::WinIsWindow(vHabmain, hWndOldFocus))
    {
        //
        // This is likely to prove that the object still exists
        //
        if (wxFindWinFromHandle((WXHWND) hWndOldFocus) == pOldFocus)
            pOldFocus->SetFocus();
    }
} // end of wxDialog::DoShowModal

bool wxDialog::Show(
  bool                              bShow
)
{
    if (!bShow)
    {
        //
        // If we had disabled other app windows, reenable them back now because
        // if they stay disabled Windows will activate another window (one
        // which is enabled, anyhow) and we will lose activation
        //
        if (m_pWindowDisabler)
        {
            delete m_pWindowDisabler;
            m_pWindowDisabler = NULL;
        }
    }

    //
    // ShowModal() may be called for already shown dialog
    //
    if (!wxDialogBase::Show(bShow) && !(bShow && IsModal()))
    {
        //
        // Nothing to do
        //
        return FALSE;
    }

    if (bShow)
    {
        //
        // Usually will result in TransferDataToWindow() being called
        //
        InitDialog();
    }

    if (IsModal())
    {
        if (bShow)
        {
            //
            // Modal dialog needs a parent window, so try to find one
            //
            if (!GetParent())
            {
                wxWindow*           pParent = wxTheApp->GetTopWindow();

                if ( pParent && pParent != this && pParent->IsShown() )
                {
                    //
                    // Use it
                    //
                    m_parent = pParent;

                }
            }
            DoShowModal();
        }
        else // end of modal dialog
        {
            //
            // This will cause IsModalShowing() return FALSE and our local
            // message loop will terminate
            //
            wxModalDialogs.DeleteObject(this);
        }
    }
    return FALSE;
} // end of wxDialog::Show

//
// Replacement for Show(TRUE) for modal dialogs - returns return code
//
int wxDialog::ShowModal()
{
    if (!IsModal())
    {
        SetModal(TRUE);
    }
    Show(TRUE);
    return GetReturnCode();
} // end of wxDialog::ShowModal

void wxDialog::EndModal(
  int                               nRetCode
)
{
    SetReturnCode(nRetCode);
    Show(FALSE);
} // end of wxDialog::EndModal

void wxDialog::OnApply(
  wxCommandEvent&                   rEvent
)
{
    if (Validate())
        TransferDataFromWindow();
} // end of wxDialog::OnApply

// Standard buttons
void wxDialog::OnOK(
  wxCommandEvent&                   rEvent
)
{
    if ( Validate() && TransferDataFromWindow() )
    {
        EndModal(wxID_OK);
    }
} // end of wxDialog::OnOK

void wxDialog::OnCancel(
  wxCommandEvent&                   rEvent
)
{
    EndModal(wxID_CANCEL);
} // end of wxDialog::OnCancel

void wxDialog::OnCloseWindow(
  wxCloseEvent&                     rEvent
)
{
    //
    // We'll send a Cancel message by default,
    // which may close the dialog.
    // Check for looping if the Cancel event handler calls Close().
    //
    // Note that if a cancel button and handler aren't present in the dialog,
    // nothing will happen when you close the dialog via the window manager, or
    // via Close().
    // We wouldn't want to destroy the dialog by default, since the dialog may have been
    // created on the stack.
    // However, this does mean that calling dialog->Close() won't delete the dialog
    // unless the handler for wxID_CANCEL does so. So use Destroy() if you want to be
    // sure to destroy the dialog.
    // The default OnCancel (above) simply ends a modal dialog, and hides a modeless dialog.
    //

    //
    // Ugh???  This is not good but until I figure out a global list it'll have to do
    //
    static wxList closing;

    if ( closing.Member(this) )
        return;

    closing.Append(this);

    wxCommandEvent                  vCancelEvent(wxEVT_COMMAND_BUTTON_CLICKED, wxID_CANCEL);

    vCancelEvent.SetEventObject( this );
    GetEventHandler()->ProcessEvent(vCancelEvent); // This may close the dialog

    closing.DeleteObject(this);
} // end of wxDialog::OnCloseWindow

//
// Destroy the window (delayed, if a managed window)
//
bool wxDialog::Destroy()
{
    wxCHECK_MSG( !wxPendingDelete.Member(this), FALSE,
                 _T("wxDialog destroyed twice") );
    wxPendingDelete.Append(this);
    return TRUE;
} // end of wxDialog::Destroy

void wxDialog::OnSysColourChanged(
  wxSysColourChangedEvent&          rEvent
)
{
    SetBackgroundColour(wxSystemSettings::GetSystemColour(wxSYS_COLOUR_3DFACE));
    Refresh();
} // end of wxDialog::OnSysColourChanged

MRESULT wxDialog::OS2WindowProc(
  WXUINT                            uMessage
, WXWPARAM                          wParam
, WXLPARAM                          lParam
)
{
    MRESULT                         rc = 0;
    bool                            bProcessed = FALSE;

    switch (uMessage)
    {
        case WM_CLOSE:
            //
            // If we can't close, tell the system that we processed the
            // message - otherwise it would close us
            //
            bProcessed = !Close();
            break;
    }

    if (!bProcessed)
        rc = wxWindow::OS2WindowProc( uMessage
                                     ,wParam
                                     ,lParam
                                    );
    return rc;
} // end of wxDialog::OS2WindowProc

