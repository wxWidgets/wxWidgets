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
#include "wx/evtloop.h"
#include "wx/ptr_scpd.h"

#define wxDIALOG_DEFAULT_X 300
#define wxDIALOG_DEFAULT_Y 300

#define wxDIALOG_DEFAULT_WIDTH 500
#define wxDIALOG_DEFAULT_HEIGHT 500

IMPLEMENT_DYNAMIC_CLASS(wxDialog, wxTopLevelWindow)

BEGIN_EVENT_TABLE(wxDialog, wxDialogBase)
    EVT_BUTTON(wxID_OK, wxDialog::OnOK)
    EVT_BUTTON(wxID_APPLY, wxDialog::OnApply)
    EVT_BUTTON(wxID_CANCEL, wxDialog::OnCancel)
    EVT_CHAR_HOOK(wxDialog::OnCharHook)
    EVT_SYS_COLOUR_CHANGED(wxDialog::OnSysColourChanged)
    EVT_CLOSE(wxDialog::OnCloseWindow)
END_EVENT_TABLE()

// ----------------------------------------------------------------------------
// wxDialogModalData
// ----------------------------------------------------------------------------

// this is simply a container for any data we need to implement modality which
// allows us to avoid changing wxDialog each time the implementation changes
class wxDialogModalData
{
public:
    wxDialogModalData(wxDialog *dialog) : m_evtLoop(dialog) { }

    void RunLoop()
    {
        m_evtLoop.Run();
    }

    void ExitLoop()
    {
        m_evtLoop.Exit();
    }

private:
    wxModalEventLoop m_evtLoop;
};

wxDEFINE_TIED_SCOPED_PTR_TYPE(wxDialogModalData);

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxDialog construction
// ----------------------------------------------------------------------------

void wxDialog::Init()
{
    m_pOldFocus = (wxWindow *)NULL;
    m_isShown = FALSE;
    m_pWindowDisabler = (wxWindowDisabler *)NULL;
    m_modalData = NULL;
    SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE));
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
    Init();
    SetExtraStyle(GetExtraStyle() | wxTOPLEVEL_EX_DIALOG);

    //
    // Save focus before doing anything which can potentially change it
    //
    m_pOldFocus = FindFocus();

    //
    // All dialogs should really have this style
    //
    lStyle |= wxTAB_TRAVERSAL;

    if (!wxTopLevelWindow::Create( pParent
                                  ,vId
                                  ,rsTitle
                                  ,rPos
                                  ,rSize
                                  ,lStyle
                                  ,rsName
                                 ))
        return FALSE;
    SetFont(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT));

    //
    // Must defer setting the title until after dialog is created and sized
    //
    if (!rsTitle.IsNull())
        SetTitle(rsTitle);
    return TRUE;
} // end of wxDialog::Create

void wxDialog::SetModal(
  bool                              bFlag
)
{
    if (bFlag)
    {
        m_windowStyle |= wxDIALOG_MODAL ;
        wxModelessWindows.DeleteObject(this);
    }
    else
    {
        m_windowStyle &= ~wxDIALOG_MODAL ;
        wxModelessWindows.Append(this);
    }
} // end of wxDialog::SetModal

wxDialog::~wxDialog()
{
    m_isBeingDeleted = TRUE;
    Show(FALSE);
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

// ----------------------------------------------------------------------------
// showing the dialogs
// ----------------------------------------------------------------------------

bool wxDialog::IsModal() const
{
    return (GetWindowStyleFlag() & wxDIALOG_MODAL) != 0;
} // end of wxDialog::IsModal

bool wxDialog::IsModalShowing() const
{
    return m_modalData != NULL; // const_cast
} // end of wxDialog::IsModalShowing

void wxDialog::DoShowModal()
{
    wxWindow*                       pParent = GetParent();
    wxWindow*                       pOldFocus = m_pOldFocus;
    HWND                            hWndOldFocus = 0;

    wxCHECK_RET( !IsModalShowing(), _T("DoShowModal() called twice") );
    wxCHECK_RET( IsModal(), _T("can't DoShowModal() modeless dialog") );

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

    //
    // Before entering the modal loop, reset the "is in OnIdle()" flag (see
    // comment in app.cpp)
    //
    extern bool                     gbInOnIdle;
    bool                            bWasInOnIdle = gbInOnIdle;

    gbInOnIdle = FALSE;

    // enter the modal loop
    {
        wxDialogModalDataTiedPtr modalData(&m_modalData,
                                           new wxDialogModalData(this));
        modalData->RunLoop();
    }
    gbInOnIdle = bWasInOnIdle;

    //
    // and restore focus
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
        // which is enabled, anyhow) and we will lose activation.  We really don't
        // do this in OS/2 since PM does this for us.
        //
        if (m_pWindowDisabler)
        {
            delete m_pWindowDisabler;
            m_pWindowDisabler = NULL;
        }
        if ( m_modalData )
            m_modalData->ExitLoop();
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
        // dialogs don't get WM_SIZE message after creation unlike most (all?)
        // other windows and so could start their life non laid out correctly
        // if we didn't call Layout() from here
        //
        // NB: normally we should call it just the first time but doing it
        //     every time is simpler than keeping a flag
//         Layout();

        //
        // Usually will result in TransferDataToWindow() being called
        //
        InitDialog();
    }

    if (GetTitle().c_str())
        ::WinSetWindowText((HWND)GetHwnd(), GetTitle().c_str());
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
    }
    return TRUE;
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

// ----------------------------------------------------------------------------
// wxWin event handlers
// ----------------------------------------------------------------------------

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

void wxDialog::OnSysColourChanged(
  wxSysColourChangedEvent&          rEvent
)
{
    SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE));
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

