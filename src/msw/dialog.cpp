/////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/dialog.cpp
// Purpose:     wxDialog class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "dialog.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/dialog.h"
    #include "wx/utils.h"
    #include "wx/frame.h"
    #include "wx/app.h"
    #include "wx/settings.h"
    #include "wx/intl.h"
    #include "wx/log.h"
#endif

#include "wx/msw/private.h"
#include "wx/log.h"

#if wxUSE_COMMON_DIALOGS && !defined(__WXMICROWIN__)
    #include <commdlg.h>
#endif

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// default dialog pos and size

#define wxDIALOG_DEFAULT_X 300
#define wxDIALOG_DEFAULT_Y 300

#define wxDIALOG_DEFAULT_WIDTH 500
#define wxDIALOG_DEFAULT_HEIGHT 500

// ----------------------------------------------------------------------------
// globals
// ----------------------------------------------------------------------------

// all modal dialogs currently shown
static wxWindowList wxModalDialogs;

// ----------------------------------------------------------------------------
// wxWin macros
// ----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxDialog, wxTopLevelWindow)

BEGIN_EVENT_TABLE(wxDialog, wxDialogBase)
    EVT_BUTTON(wxID_OK, wxDialog::OnOK)
    EVT_BUTTON(wxID_APPLY, wxDialog::OnApply)
    EVT_BUTTON(wxID_CANCEL, wxDialog::OnCancel)

    EVT_CHAR_HOOK(wxDialog::OnCharHook)

    EVT_SYS_COLOUR_CHANGED(wxDialog::OnSysColourChanged)

    EVT_CLOSE(wxDialog::OnCloseWindow)
END_EVENT_TABLE()

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxDialog construction
// ----------------------------------------------------------------------------

void wxDialog::Init()
{
    m_oldFocus = (wxWindow *)NULL;

    m_isShown = FALSE;

    m_windowDisabler = (wxWindowDisabler *)NULL;

    SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE));
}

bool wxDialog::Create(wxWindow *parent,
                      wxWindowID id,
                      const wxString& title,
                      const wxPoint& pos,
                      const wxSize& size,
                      long style,
                      const wxString& name)
{
    Init();

    SetExtraStyle(GetExtraStyle() | wxTOPLEVEL_EX_DIALOG);

    // save focus before doing anything which can potentially change it
    m_oldFocus = FindFocus();

    // All dialogs should really have this style
    style |= wxTAB_TRAVERSAL;

    if ( !wxTopLevelWindow::Create(parent, id, title, pos, size, style, name) )
        return FALSE;

    SetFont(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT));

    return TRUE;
}

void wxDialog::SetModal(bool flag)
{
    if ( flag )
    {
        m_windowStyle |= wxDIALOG_MODAL;

        wxModelessWindows.DeleteObject(this);
    }
    else
    {
        m_windowStyle &= ~wxDIALOG_MODAL;

        wxModelessWindows.Append(this);
    }
}

wxDialog::~wxDialog()
{
    m_isBeingDeleted = TRUE;

    // this will also reenable all the other windows for a modal dialog
    Show(FALSE);
}

// ----------------------------------------------------------------------------
// kbd handling
// ----------------------------------------------------------------------------

// By default, pressing escape cancels the dialog
void wxDialog::OnCharHook(wxKeyEvent& event)
{
    if (GetHWND())
    {
        // "Esc" works as an accelerator for the "Cancel" button, but it
        // shouldn't close the dialog which doesn't have any cancel button
        if ( (event.m_keyCode == WXK_ESCAPE) && FindWindow(wxID_CANCEL) )
        {
            wxCommandEvent cancelEvent(wxEVT_COMMAND_BUTTON_CLICKED, wxID_CANCEL);
            cancelEvent.SetEventObject( this );
            GetEventHandler()->ProcessEvent(cancelEvent);

            // ensure that there is another message for this window so the
            // ShowModal loop will exit and won't get stuck in GetMessage().
            ::PostMessage(GetHwnd(), WM_NULL, 0, 0);

            return;
        }
    }

    // We didn't process this event.
    event.Skip();
}

// ----------------------------------------------------------------------------
// showing the dialogs
// ----------------------------------------------------------------------------

bool wxDialog::IsModal() const
{
    return (GetWindowStyleFlag() & wxDIALOG_MODAL) != 0;
}

bool wxDialog::IsModalShowing() const
{
    return wxModalDialogs.Find(wxConstCast(this, wxDialog)) != NULL;
}

wxWindow *wxDialog::FindSuitableParent() const
{
    // first try to use the currently active window
    HWND hwndFg = ::GetForegroundWindow();
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

void wxDialog::DoShowModal()
{
    wxCHECK_RET( !IsModalShowing(), _T("DoShowModal() called twice") );
    wxCHECK_RET( IsModal(), _T("can't DoShowModal() modeless dialog") );

    wxModalDialogs.Append(this);

    wxWindow *parent = GetParent();

    wxWindow* oldFocus = m_oldFocus;

    // We have to remember the HWND because we need to check
    // the HWND still exists (oldFocus can be garbage when the dialog
    // exits, if it has been destroyed)
    HWND hwndOldFocus = 0;
    if (oldFocus)
        hwndOldFocus = (HWND) oldFocus->GetHWND();

    // remember where the focus was
    if ( !oldFocus )
    {
        oldFocus = parent;
        if ( parent )
            hwndOldFocus = GetHwndOf(parent);
    }

    // disable all other app windows
    wxASSERT_MSG( !m_windowDisabler, _T("disabling windows twice?") );

    m_windowDisabler = new wxWindowDisabler(this);

    // before entering the modal loop, reset the "is in OnIdle()" flag (see
    // comment in app.cpp)
    extern bool wxIsInOnIdleFlag;
    bool wasInOnIdle = wxIsInOnIdleFlag;
    wxIsInOnIdleFlag = FALSE;

    // enter the modal loop
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

    wxIsInOnIdleFlag = wasInOnIdle;

    // and restore focus
    // Note that this code MUST NOT access the dialog object's data
    // in case the object has been deleted (which will be the case
    // for a modal dialog that has been destroyed before calling EndModal).
    if ( oldFocus && (oldFocus != this) && ::IsWindow(hwndOldFocus))
    {
        // This is likely to prove that the object still exists
        if (wxFindWinFromHandle((WXHWND) hwndOldFocus) == oldFocus)
            oldFocus->SetFocus();
    }
}

bool wxDialog::Show(bool show)
{
    if ( !show )
    {
        // if we had disabled other app windows, reenable them back now because
        // if they stay disabled Windows will activate another window (one
        // which is enabled, anyhow) and we will lose activation
        if ( m_windowDisabler )
        {
            delete m_windowDisabler;
            m_windowDisabler = NULL;
        }
    }

    // ShowModal() may be called for already shown dialog
    if ( !wxDialogBase::Show(show) && !(show && IsModal()) )
    {
        // nothing to do
        return FALSE;
    }

    if ( show )
    {
        // dialogs don't get WM_SIZE message after creation unlike most (all?)
        // other windows and so could start their life non laid out correctly
        // if we didn't call Layout() from here
        //
        // NB: normally we should call it just the first time but doing it
        //     every time is simpler than keeping a flag
        Layout();

        // usually will result in TransferDataToWindow() being called
        InitDialog();
    }

    if ( IsModal() )
    {
        if ( show )
        {
            // modal dialog needs a parent window, so try to find one
            if ( !GetParent() )
            {
                m_parent = FindSuitableParent();
            }

            DoShowModal();
        }
        else // end of modal dialog
        {
            // this will cause IsModalShowing() return FALSE and our local
            // message loop will terminate
            wxModalDialogs.DeleteObject(this);

            // ensure that there is another message for this window so the
            // ShowModal loop will exit and won't get stuck in GetMessage().
            ::PostMessage(GetHwnd(), WM_NULL, 0, 0);
        }
    }

    return TRUE;
}

void wxDialog::Raise()
{
    ::SetForegroundWindow(GetHwnd());
}

// a special version for Show(TRUE) for modal dialogs which returns return code
int wxDialog::ShowModal()
{
    if ( !IsModal() )
    {
        SetModal(TRUE);
    }

    Show(TRUE);

    return GetReturnCode();
}

// NB: this function (surprizingly) may be called for both modal and modeless
//     dialogs and should work for both of them
void wxDialog::EndModal(int retCode)
{
    SetReturnCode(retCode);

    Show(FALSE);
}

// ----------------------------------------------------------------------------
// wxWin event handlers
// ----------------------------------------------------------------------------

// Standard buttons
void wxDialog::OnOK(wxCommandEvent& WXUNUSED(event))
{
  if ( Validate() && TransferDataFromWindow() )
  {
      EndModal(wxID_OK);
  }
}

void wxDialog::OnApply(wxCommandEvent& WXUNUSED(event))
{
    if ( Validate() )
        TransferDataFromWindow();

    // TODO probably need to disable the Apply button until things change again
}

void wxDialog::OnCancel(wxCommandEvent& WXUNUSED(event))
{
    EndModal(wxID_CANCEL);
}

void wxDialog::OnCloseWindow(wxCloseEvent& WXUNUSED(event))
{
    // We'll send a Cancel message by default, which may close the dialog.
    // Check for looping if the Cancel event handler calls Close().

    // Note that if a cancel button and handler aren't present in the dialog,
    // nothing will happen when you close the dialog via the window manager, or
    // via Close(). We wouldn't want to destroy the dialog by default, since
    // the dialog may have been created on the stack. However, this does mean
    // that calling dialog->Close() won't delete the dialog unless the handler
    // for wxID_CANCEL does so. So use Destroy() if you want to be sure to
    // destroy the dialog. The default OnCancel (above) simply ends a modal
    // dialog, and hides a modeless dialog.

    // VZ: this is horrible and MT-unsafe. Can't we reuse some of these global
    //     lists here? don't dare to change it now, but should be done later!
    static wxList closing;

    if ( closing.Member(this) )
        return;

    closing.Append(this);

    wxCommandEvent cancelEvent(wxEVT_COMMAND_BUTTON_CLICKED, wxID_CANCEL);
    cancelEvent.SetEventObject( this );
    GetEventHandler()->ProcessEvent(cancelEvent); // This may close the dialog

    closing.DeleteObject(this);
}

void wxDialog::OnSysColourChanged(wxSysColourChangedEvent& WXUNUSED(event))
{
#if wxUSE_CTL3D
    Ctl3dColorChange();
#else
    SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE));
    Refresh();
#endif
}

// ---------------------------------------------------------------------------
// dialog window proc
// ---------------------------------------------------------------------------

long wxDialog::MSWWindowProc(WXUINT message, WXWPARAM wParam, WXLPARAM lParam)
{
    long rc = 0;
    bool processed = FALSE;

    switch ( message )
    {
        case WM_CLOSE:
            // if we can't close, tell the system that we processed the
            // message - otherwise it would close us
            processed = !Close();
            break;

        case WM_SIZE:
            // the Windows dialogs unfortunately are not meant to be resizeable
            // at all and their standard class doesn't include CS_[VH]REDRAW
            // styles which means that the window is not refreshed properly
            // after the resize and no amount of WS_CLIPCHILDREN/SIBLINGS can
            // help with it - so we have to refresh it manually which certainly
            // creates flicker but at least doesn't show garbage on the screen
            rc = wxWindow::MSWWindowProc(message, wParam, lParam);
            processed = TRUE;
            if ( !HasFlag(wxNO_FULL_REPAINT_ON_RESIZE) )
            {
                ::InvalidateRect(GetHwnd(), NULL, FALSE /* erase bg */);
            }
            break;

#ifndef __WXMICROWIN__
        case WM_SETCURSOR:
            // we want to override the busy cursor for modal dialogs:
            // typically, wxBeginBusyCursor() is called and then a modal dialog
            // is shown, but the modal dialog shouldn't have hourglass cursor
            if ( IsModalShowing() && wxIsBusy() )
            {
                // set our cursor for all windows (but see below)
                wxCursor cursor = m_cursor;
                if ( !cursor.Ok() )
                    cursor = wxCURSOR_ARROW;

                ::SetCursor(GetHcursorOf(cursor));

                // in any case, stop here and don't let wxWindow process this
                // message (it would set the busy cursor)
                processed = TRUE;

                // but return FALSE to tell the child window (if the event
                // comes from one of them and not from ourselves) that it can
                // set its own cursor if it has one: thus, standard controls
                // (e.g. text ctrl) still have correct cursors in a dialog
                // invoked while wxIsBusy()
                rc = FALSE;
            }
            break;
#endif // __WXMICROWIN__
    }

    if ( !processed )
        rc = wxWindow::MSWWindowProc(message, wParam, lParam);

    return rc;
}

#if wxUSE_CTL3D

// Define for each class of dialog and control
WXHBRUSH wxDialog::OnCtlColor(WXHDC WXUNUSED(pDC),
                              WXHWND WXUNUSED(pWnd),
                              WXUINT WXUNUSED(nCtlColor),
                              WXUINT message,
                              WXWPARAM wParam,
                              WXLPARAM lParam)
{
    return (WXHBRUSH)Ctl3dCtlColorEx(message, wParam, lParam);
}

#endif // wxUSE_CTL3D

