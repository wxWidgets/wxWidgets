/////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/dialog.cpp
// Purpose:     wxDialog class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
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

#if wxUSE_COMMON_DIALOGS
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

// all objects to be deleted during next idle processing - from window.cpp
extern wxList WXDLLEXPORT wxPendingDelete;

// all frames and modeless dialogs - not static, used in frame.cpp, mdi.cpp &c
wxWindowList wxModelessWindows;

// all modal dialogs currently shown
static wxWindowList wxModalDialogs;

// ----------------------------------------------------------------------------
// wxWin macros
// ----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxDialog, wxPanel)

BEGIN_EVENT_TABLE(wxDialog, wxPanel)
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

wxDialog::wxDialog()
{
    m_oldFocus = (wxWindow *)NULL;
    m_isShown = FALSE;

    SetBackgroundColour(wxSystemSettings::GetSystemColour(wxSYS_COLOUR_3DFACE));
}

bool wxDialog::Create(wxWindow *parent, wxWindowID id,
                      const wxString& title,
                      const wxPoint& pos,
                      const wxSize& size,
                      long style,
                      const wxString& name)
{
    m_oldFocus = FindFocus();

    SetBackgroundColour(wxSystemSettings::GetSystemColour(wxSYS_COLOUR_3DFACE));
    SetName(name);

    wxTopLevelWindows.Append(this);

    //  windowFont = wxTheFontList->FindOrCreateFont(11, wxSWISS, wxNORMAL, wxNORMAL);

    if (parent) parent->AddChild(this);

    if ( id == -1 )
        m_windowId = (int)NewControlId();
    else
        m_windowId = id;

    int x = pos.x;
    int y = pos.y;
    int width = size.x;
    int height = size.y;

    if (x < 0)
        x = wxDIALOG_DEFAULT_X;
    if (y < 0)
        y = wxDIALOG_DEFAULT_Y;

    m_windowStyle = style;

    m_isShown = FALSE;

    if (width < 0)
        width = wxDIALOG_DEFAULT_WIDTH;
    if (height < 0)
        height = wxDIALOG_DEFAULT_HEIGHT;

    // All dialogs should really have this style
    m_windowStyle |= wxTAB_TRAVERSAL;

    WXDWORD extendedStyle = MakeExtendedStyle(m_windowStyle);
    if (m_windowStyle & wxSTAY_ON_TOP)
        extendedStyle |= WS_EX_TOPMOST;

    // Allows creation of dialogs with & without captions under MSWindows,
    // resizeable or not (but a resizeable dialog always has caption -
    // otherwise it would look too strange)
    const wxChar *dlg;
    if ( style & wxRESIZE_BORDER )
        dlg = wxT("wxResizeableDialog");
    else if ( style & wxCAPTION )
        dlg = wxT("wxCaptionDialog");
    else
        dlg = wxT("wxNoCaptionDialog");
    MSWCreate(m_windowId, parent, NULL, this, NULL,
              x, y, width, height,
              0, // style is not used if we have dlg template
              dlg,
              extendedStyle);

    HWND hwnd = (HWND)GetHWND();

    if ( !hwnd )
    {
        wxFAIL_MSG(_("Failed to create dialog. You probably forgot to include wx/msw/wx.rc in your resources."));

        return FALSE;
    }

    SubclassWin(GetHWND());

    SetWindowText(hwnd, title);
    SetFont(wxSystemSettings::GetSystemFont(wxSYS_DEFAULT_GUI_FONT));

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

    wxTopLevelWindows.DeleteObject(this);

    // this will call BringWindowToTop() if necessary to bring back our parent
    // window to top
    Show(FALSE);

    if ( !IsModal() )
        wxModelessWindows.DeleteObject(this);

    // If this is the last top-level window, exit.
    if ( wxTheApp && (wxTopLevelWindows.Number() == 0) )
    {
        wxTheApp->SetTopWindow(NULL);

        if ( wxTheApp->GetExitOnFrameDelete() )
        {
            ::PostQuitMessage(0);
        }
    }
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
// Windows dialog boxes can't be iconized
// ----------------------------------------------------------------------------

void wxDialog::Iconize(bool WXUNUSED(iconize))
{
}

bool wxDialog::IsIconized() const
{
    return FALSE;
}

// ----------------------------------------------------------------------------
// size/position handling
// ----------------------------------------------------------------------------

void wxDialog::DoSetClientSize(int width, int height)
{
    HWND hWnd = (HWND) GetHWND();
    RECT rect;
    ::GetClientRect(hWnd, &rect);

    RECT rect2;
    GetWindowRect(hWnd, &rect2);

    // Find the difference between the entire window (title bar and all)
    // and the client area; add this to the new client size to move the
    // window
    int actual_width = rect2.right - rect2.left - rect.right + width;
    int actual_height = rect2.bottom - rect2.top - rect.bottom + height;

    MoveWindow(hWnd, rect2.left, rect2.top, actual_width, actual_height, TRUE);

    wxSizeEvent event(wxSize(actual_width, actual_height), m_windowId);
    event.SetEventObject( this );
    GetEventHandler()->ProcessEvent(event);
}

void wxDialog::DoGetPosition(int *x, int *y) const
{
    RECT rect;
    GetWindowRect(GetHwnd(), &rect);

    if ( x )
        *x = rect.left;
    if ( y )
        *y = rect.top;
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
    return wxModalDialogs.Find((wxDialog *)this) != NULL; // const_cast
}

void wxDialog::DoShowModal()
{
    wxCHECK_RET( !IsModalShowing(), _T("DoShowModal() called twice") );
    wxCHECK_RET( IsModal(), _T("can't DoShowModal() modeless dialog") );

    wxModalDialogs.Append(this);

    wxWindow *parent = GetParent();

    // inside this block, all app windows are disabled
    {
        wxWindowDisabler wd(this);

        // remember where the focus was
        if ( !m_oldFocus )
        {
            m_oldFocus = parent;
        }
        if ( !m_oldFocus )
        {
            m_oldFocus = wxTheApp->GetTopWindow();
        }

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
    }

#ifdef __WIN32__
    if ( parent )
        ::SetActiveWindow(GetHwndOf(parent));
#endif // __WIN32__

    // and restore focus
    if ( m_oldFocus && (m_oldFocus != this) )
    {
        m_oldFocus->SetFocus();
    }
}

bool wxDialog::Show(bool show)
{
    // The following is required when the parent has been disabled, (modal
    // dialogs, or modeless dialogs with disabling such as wxProgressDialog).
    // Otherwise the parent disappears behind other windows when the dialog is
    // hidden.
    if ( !show )
    {
        wxWindow *parent = GetParent();
        if ( parent )
        {
            ::BringWindowToTop(GetHwndOf(parent));
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
                wxWindow *parent = wxTheApp->GetTopWindow();
                if ( parent && parent != this )
                {
                    // use it
                    m_parent = parent;
                }
            }

            DoShowModal();
        }
        else // end of modal dialog
        {
            // this will cause IsModalShowing() return FALSE and our local
            // message loop will terminate
            wxModalDialogs.DeleteObject(this);
        }
    }

    return TRUE;
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
void wxDialog::OnOK(wxCommandEvent& event)
{
  if ( Validate() && TransferDataFromWindow() )
  {
      EndModal(wxID_OK);
  }
}

void wxDialog::OnApply(wxCommandEvent& event)
{
    if ( Validate() )
        TransferDataFromWindow();

    // TODO probably need to disable the Apply button until things change again
}

void wxDialog::OnCancel(wxCommandEvent& event)
{
    EndModal(wxID_CANCEL);
}

void wxDialog::OnCloseWindow(wxCloseEvent& event)
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

// Destroy the window (delayed, if a managed window)
bool wxDialog::Destroy()
{
    wxCHECK_MSG( !wxPendingDelete.Member(this), FALSE,
                 _T("wxDialog destroyed twice") );

    wxPendingDelete.Append(this);

    return TRUE;
}

void wxDialog::OnSysColourChanged(wxSysColourChangedEvent& event)
{
#if wxUSE_CTL3D
    Ctl3dColorChange();
#else
    SetBackgroundColour(wxSystemSettings::GetSystemColour(wxSYS_COLOUR_3DFACE));
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
        case WM_ACTIVATE:
            switch ( LOWORD(wParam) )
            {
                case WA_ACTIVE:
                case WA_CLICKACTIVE:
                    if ( IsModalShowing() && GetParent() )
                    {
                        // bring the owner window to top as the standard dialog
                        // boxes do
                        if ( !::SetWindowPos
                                (
                                    GetHwndOf(GetParent()),
                                    GetHwnd(),
                                    0, 0,
                                    0, 0,
                                    SWP_NOACTIVATE |
                                    SWP_NOMOVE |
                                    SWP_NOSIZE
                                ) )
                        {
                            wxLogLastError(wxT("SetWindowPos(SWP_NOACTIVATE)"));
                        }
                    }
                    // fall through to process it normally as well
            }
            break;

        case WM_CLOSE:
            // if we can't close, tell the system that we processed the
            // message - otherwise it would close us
            processed = !Close();
            break;

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

