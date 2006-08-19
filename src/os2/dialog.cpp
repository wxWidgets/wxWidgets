/////////////////////////////////////////////////////////////////////////////
// Name:        src/os2/dialog.cpp
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

#include "wx/dialog.h"

#ifndef WX_PRECOMP
    #include "wx/utils.h"
    #include "wx/frame.h"
    #include "wx/app.h"
    #include "wx/settings.h"
    #include "wx/intl.h"
    #include "wx/log.h"
#endif

#include "wx/os2/private.h"
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
    m_isShown = false;
    m_pWindowDisabler = (wxWindowDisabler *)NULL;
    m_modalData = NULL;
    SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE));
} // end of wxDialog::Init

bool wxDialog::Create( wxWindow*       pParent,
                       wxWindowID      vId,
                       const wxString& rsTitle,
                       const wxPoint&  rPos,
                       const wxSize&   rSize,
                       long            lStyle,
                       const wxString& rsName )
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
        return false;

    SetFont(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT));

    //
    // Must defer setting the title until after dialog is created and sized
    //
    if (!rsTitle.IsNull())
        SetTitle(rsTitle);
    return true;
} // end of wxDialog::Create

#if WXWIN_COMPATIBILITY_2_6

// deprecated ctor
wxDialog::wxDialog(wxWindow *parent,
                   const wxString& title,
                   bool WXUNUSED(modal),
                   int x,
                   int y,
                   int w,
                   int h,
                   long style,
                   const wxString& name)
{
    Init();

    Create(parent, wxID_ANY, title, wxPoint(x, y), wxSize(w, h), style, name);
}

void wxDialog::SetModal(bool WXUNUSED(bFlag))
{
    // nothing to do, obsolete method
} // end of wxDialog::SetModal

#endif // WXWIN_COMPATIBILITY_2_6

wxDialog::~wxDialog()
{
    m_isBeingDeleted = true;

    // this will also reenable all the other windows for a modal dialog
    Show(false);
} // end of wxDialog::~wxDialog

// ----------------------------------------------------------------------------
// showing the dialogs
// ----------------------------------------------------------------------------

#if WXWIN_COMPATIBILITY_2_6

bool wxDialog::IsModalShowing() const
{
    return IsModal();
} // end of wxDialog::IsModalShowing

#endif // WXWIN_COMPATIBILITY_2_6

wxWindow *wxDialog::FindSuitableParent() const
{
    // first try to use the currently active window
    HWND hwndFg = ::WinQueryActiveWindow(HWND_DESKTOP);
    wxWindow *parent = hwndFg ? wxFindWinFromHandle((WXHWND)hwndFg)
                              : NULL;
    if ( !parent )
    {
        // next try the main app window
        parent = wxTheApp->GetTopWindow();
    }

    // finally, check if the parent we found is really suitable
    if ( !parent || parent == (wxWindow *)this || !parent->IsShown() )
    {
        // don't use this one
        parent = NULL;
    }

    return parent;
}

bool wxDialog::Show( bool bShow )
{
    if ( bShow == IsShown() )
        return false;

    if (!bShow && m_modalData )
    {
        // we need to do this before calling wxDialogBase version because if we
        // had disabled other app windows, they must be reenabled right now as
        // if they stay disabled Windows will activate another window (one
        // which is enabled, anyhow) when we're hidden in the base class Show()
        // and we will lose activation
        m_modalData->ExitLoop();
#if 0
        if (m_pWindowDisabler)
        {
            delete m_pWindowDisabler;
            m_pWindowDisabler = NULL;
        }
#endif
    }

    if (bShow)
    {
        // this usually will result in TransferDataToWindow() being called
        // which will change the controls values so do it before showing as
        // otherwise we could have some flicker
        InitDialog();
    }

    wxDialogBase::Show(bShow);

    wxString title = GetTitle();
    if (!title.empty())
        ::WinSetWindowText((HWND)GetHwnd(), (PSZ)title.c_str());

    if ( bShow )
    {
        // dialogs don't get WM_SIZE message after creation unlike most (all?)
        // other windows and so could start their life not laid out correctly
        // if we didn't call Layout() from here
        //
        // NB: normally we should call it just the first time but doing it
        //     every time is simpler than keeping a flag
        Layout();
    }

    return true;
} // end of wxDialog::Show

//
// Replacement for Show(true) for modal dialogs - returns return code
//
int wxDialog::ShowModal()
{
    wxASSERT_MSG( !IsModal(), _T("wxDialog::ShowModal() reentered?") );

    m_endModalCalled = false;

    Show();

    // EndModal may have been called from InitDialog handler (called from
    // inside Show()), which would cause an infinite loop if we didn't take it
    // into account
    if ( !m_endModalCalled )
    {
        // modal dialog needs a parent window, so try to find one
        wxWindow *parent = GetParent();
        if ( !parent )
        {
            parent = FindSuitableParent();
        }

        // remember where the focus was
        wxWindow *oldFocus = m_pOldFocus;
        if ( !oldFocus )
        {
            // VZ: do we really want to do this?
            oldFocus = parent;
        }

        // We have to remember the HWND because we need to check
        // the HWND still exists (oldFocus can be garbage when the dialog
        // exits, if it has been destroyed)
        HWND hwndOldFocus = oldFocus ? GetHwndOf(oldFocus) : NULL;


        //
        // Before entering the modal loop, reset the "is in OnIdle()" flag (see
        // comment in app.cpp)
        //
        extern bool                     gbInOnIdle;
        bool                            bWasInOnIdle = gbInOnIdle;

        gbInOnIdle = false;

        // enter and run the modal loop
        {
            wxDialogModalDataTiedPtr modalData(&m_modalData,
                                               new wxDialogModalData(this));
            modalData->RunLoop();
        }
        gbInOnIdle = bWasInOnIdle;

        // and restore focus
        // Note that this code MUST NOT access the dialog object's data
        // in case the object has been deleted (which will be the case
        // for a modal dialog that has been destroyed before calling EndModal).
        if ( oldFocus && (oldFocus != this) && ::WinIsWindow(vHabmain, hwndOldFocus))
        {
            // This is likely to prove that the object still exists
            if (wxFindWinFromHandle((WXHWND) hwndOldFocus) == oldFocus)
                oldFocus->SetFocus();
        }
    }

    return GetReturnCode();
} // end of wxDialog::ShowModal

void wxDialog::EndModal(
  int                               nRetCode
)
{
    wxASSERT_MSG( IsModal(), _T("EndModal() called for non modal dialog") );

    m_endModalCalled = true;
    SetReturnCode(nRetCode);

    Hide();
} // end of wxDialog::EndModal

void wxDialog::EndDialog(int rc)
{
    if ( IsModal() )
        EndModal(rc);
    else
        Hide();
}

// ----------------------------------------------------------------------------
// wxWin event handlers
// ----------------------------------------------------------------------------

void wxDialog::OnApply( wxCommandEvent& WXUNUSED(rEvent) )
{
    if (Validate())
        TransferDataFromWindow();
} // end of wxDialog::OnApply

// Standard buttons
void wxDialog::OnOK( wxCommandEvent& WXUNUSED(rEvent) )
{
    if ( Validate() && TransferDataFromWindow() )
    {
        EndDialog(wxID_OK);
    }
} // end of wxDialog::OnOK

void wxDialog::OnCancel( wxCommandEvent& WXUNUSED(rEvent) )
{
    EndDialog(wxID_CANCEL);
} // end of wxDialog::OnCancel

void wxDialog::OnCloseWindow( wxCloseEvent& WXUNUSED(rEvent) )
{
    //
    // We'll send a Cancel message by default, which may close the dialog.
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

    wxCommandEvent vCancelEvent(wxEVT_COMMAND_BUTTON_CLICKED, wxID_CANCEL);

    vCancelEvent.SetEventObject( this );
    GetEventHandler()->ProcessEvent(vCancelEvent); // This may close the dialog

    closing.DeleteObject(this);
} // end of wxDialog::OnCloseWindow

void wxDialog::OnSysColourChanged( wxSysColourChangedEvent& WXUNUSED(rEvent) )
{
    SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE));
    Refresh();
} // end of wxDialog::OnSysColourChanged

MRESULT wxDialog::OS2WindowProc( WXUINT uMessage, WXWPARAM wParam, WXLPARAM lParam )
{
    MRESULT  rc = 0;
    bool     bProcessed = false;

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
