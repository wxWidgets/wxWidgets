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

wxDialog::wxDialog()
{
    m_isShown = FALSE;
    m_modalShowing = FALSE;

    SetBackgroundColour(wxSystemSettings::GetSystemColour(wxSYS_COLOUR_3DFACE));
}

bool wxDialog::Create(wxWindow *parent, wxWindowID id,
           const wxString& title,
           const wxPoint& pos,
           const wxSize& size,
           long style,
           const wxString& name)
{
#if wxUSE_TOOLTIPS
    m_hwndToolTip = 0;
#endif

    SetBackgroundColour(wxSystemSettings::GetSystemColour(wxSYS_COLOUR_3DFACE));
    SetName(name);

    if (!parent)
        wxTopLevelWindows.Append(this);

    if (parent) parent->AddChild(this);

    if ( id == -1 )
        m_windowId = (int)NewControlId();
    else
        m_windowId = id;

    int x = pos.x;
    int y = pos.y;
    int width = size.x;
    int height = size.y;

    if (x < 0) x = wxDIALOG_DEFAULT_X;
    if (y < 0) y = wxDIALOG_DEFAULT_Y;

    m_windowStyle = style;

    m_isShown = FALSE;
    m_modalShowing = FALSE;

    if (width < 0)
        width = 500;
    if (height < 0)
        height = 500;

    // TODO: convert below to OS/2 PM code

    // All dialogs should really have this style
//    m_windowStyle |= wxTAB_TRAVERSAL;
//
//    WXDWORD extendedStyle = MakeExtendedStyle(m_windowStyle);
//    if (m_windowStyle & wxSTAY_ON_TOP)
//        extendedStyle |= WS_EX_TOPMOST;
//
    // Allows creation of dialogs with & without captions under MSWindows,
    // resizeable or not (but a resizeable dialog always has caption -
    // otherwise it would look too strange)
//    const wxChar *dlg;
//    if ( style & wxRESIZE_BORDER )
//        dlg = wxT("wxResizeableDialog");
//    else if ( style & wxCAPTION )
//        dlg = wxT("wxCaptionDialog");
//    else
//        dlg = wxT("wxNoCaptionDialog");
//    MSWCreate(m_windowId, parent, NULL, this, NULL,
//              x, y, width, height,
//              0, // style is not used if we have dlg template
//              dlg,
//              extendedStyle);
//
//    HWND hwnd = (HWND)GetHWND();
//
//    if ( !hwnd )
//    {
//        wxLogError(wxT("Failed to create dialog."));
//
//        return FALSE;
//    }
//
//    SubclassWin(GetHWND());
//
//    SetWindowText(hwnd, title);
//    SetFont(wxSystemSettings::GetSystemFont(wxSYS_DEFAULT_GUI_FONT));
//
    return TRUE;
}

void wxDialog::SetModal(bool flag)
{
    if ( flag )
        m_windowStyle |= wxDIALOG_MODAL ;
    else if ( m_windowStyle & wxDIALOG_MODAL )
        m_windowStyle -= wxDIALOG_MODAL ;

      wxModelessWindows.DeleteObject(this);
      if (!flag)
          wxModelessWindows.Append(this);
}

wxDialog::~wxDialog()
{
    m_isBeingDeleted = TRUE;

    wxTopLevelWindows.DeleteObject(this);

    Show(FALSE);

    if (m_modalShowing)
    {
        if (GetParent() && GetParent()->GetHWND())
           // TODO: bring the parent to the top
           return;
    }

    m_modalShowing = FALSE;
    if ( (GetWindowStyleFlag() & wxDIALOG_MODAL) != wxDIALOG_MODAL )
        wxModelessWindows.DeleteObject(this);


    // If this is the last top-level window, exit.
    if (wxTheApp && (wxTopLevelWindows.Number() == 0))
    {
      wxTheApp->SetTopWindow(NULL);

      if (wxTheApp->GetExitOnFrameDelete())
      {
         // TODO: exit
      }
    }
}

// By default, pressing escape cancels the dialog
void wxDialog::OnCharHook(wxKeyEvent& event)
{
    if (GetHWND())
    {
        if (event.m_keyCode == WXK_ESCAPE)
        {
            // Behaviour changed in 2.0: we'll send a Cancel message
            // to the dialog instead of Close.
            wxCommandEvent cancelEvent(wxEVT_COMMAND_BUTTON_CLICKED, wxID_CANCEL);
            cancelEvent.SetEventObject( this );
            GetEventHandler()->ProcessEvent(cancelEvent);

            return;
        }
    }
    // We didn't process this event.
    event.Skip();
}

void wxDialog::OnPaint(wxPaintEvent& event)
{
  // No: if you call the default procedure, it makes
  // the following painting code not work.
//  wxWindow::OnPaint(event);
}

void wxDialog::Fit()
{
    wxWindow::Fit();
}

void wxDialog::Iconize(bool WXUNUSED(iconize))
{
    // Windows dialog boxes can't be iconized
}

bool wxDialog::IsIconized() const
{
    return FALSE;
}

void wxDialog::DoSetClientSize(int width, int height)
{
    // TODO: Convert the below to OS/2 PM code

//    HWND hWnd = (HWND) GetHWND();
//    RECT rect;
//    ::GetClientRect(hWnd, &rect);
//
//    RECT rect2;
//    GetWindowRect(hWnd, &rect2);
//
    // Find the difference between the entire window (title bar and all)
    // and the client area; add this to the new client size to move the
    // window
//    int actual_width = rect2.right - rect2.left - rect.right + width;
//    int actual_height = rect2.bottom - rect2.top - rect.bottom + height;

//    MoveWindow(hWnd, rect2.left, rect2.top, actual_width, actual_height, TRUE);
//
//    wxSizeEvent event(wxSize(actual_width, actual_height), m_windowId);
//    event.SetEventObject( this );
//    GetEventHandler()->ProcessEvent(event);
}
void wxDialog::GetPosition(int *x, int *y) const
{
    // TODO: Convert
//  HWND hWnd = (HWND) GetHWND();
//  RECT rect;
//  GetWindowRect(hWnd, &rect);

//  *x = rect.left;
//  *y = rect.top;
}

bool wxDialog::IsShown() const
{
  return m_isShown;
}

bool wxDialog::IsModal() const
{
    return wxModalDialogs.Find((wxDialog *)this) != 0; // const_cast
}

bool wxDialog::Show(bool show)
{
    // TODO: This is involved code, look at msw port for details
    return FALSE;
}

void wxDialog::SetTitle(const wxString& title)
{
    ::WinSetWindowText((HWND) GetHWND(), title.c_str());
}

wxString wxDialog::GetTitle() const
{
    ::WinQueryWindowText((HWND) GetHWND(), 1000, wxBuffer);
    return wxString(wxBuffer);
}

void wxDialog::Centre(int direction)
{
  int x_offset,y_offset ;
  int display_width, display_height;
  int  width, height, x, y;
  wxWindow *parent = GetParent();
  if ((direction & wxCENTER_FRAME) && parent)
  {
      parent->GetPosition(&x_offset,&y_offset) ;
      parent->GetSize(&display_width,&display_height) ;
  }
  else
  {
    wxDisplaySize(&display_width, &display_height);
    x_offset = 0 ;
    y_offset = 0 ;
  }

  GetSize(&width, &height);
  GetPosition(&x, &y);

  if (direction & wxHORIZONTAL)
    x = (int)((display_width - width)/2);
  if (direction & wxVERTICAL)
    y = (int)((display_height - height)/2);

  SetSize(x+x_offset, y+y_offset, width, height);
}

// Replacement for Show(TRUE) for modal dialogs - returns return code
int wxDialog::ShowModal()
{
    m_windowStyle |= wxDIALOG_MODAL;
    Show(TRUE);
    return GetReturnCode();
}

void wxDialog::EndModal(int retCode)
{
    SetReturnCode(retCode);
    // TODO modal un-showing
    Show(FALSE);
}

// Define for each class of dialog and control
WXHBRUSH wxDialog::OnCtlColor(WXHDC pDC, WXHWND pWnd, WXUINT nCtlColor,
      WXUINT message, WXWPARAM wParam, WXLPARAM lParam)
{
    return 0;
}
// Standard buttons
void wxDialog::OnOK(wxCommandEvent& event)
{
    if ( Validate() && TransferDataFromWindow() )
    {
        if ( IsModal() )
            EndModal(wxID_OK);
        else
        {
            SetReturnCode(wxID_OK);
            this->Show(FALSE);
        }
    }
}

void wxDialog::OnApply(wxCommandEvent& event)
{
    if (Validate())
        TransferDataFromWindow();
    // TODO probably need to disable the Apply button until things change again
}

void wxDialog::OnCancel(wxCommandEvent& event)
{
    if ( IsModal() )
        EndModal(wxID_CANCEL);
    else
    {
        SetReturnCode(wxID_CANCEL);
        this->Show(FALSE);
    }
}

void wxDialog::OnCloseWindow(wxCloseEvent& event)
{
    // We'll send a Cancel message by default,
    // which may close the dialog.
    // Check for looping if the Cancel event handler calls Close().

    // Note that if a cancel button and handler aren't present in the dialog,
    // nothing will happen when you close the dialog via the window manager, or
    // via Close().
    // We wouldn't want to destroy the dialog by default, since the dialog may have been
    // created on the stack.
    // However, this does mean that calling dialog->Close() won't delete the dialog
    // unless the handler for wxID_CANCEL does so. So use Destroy() if you want to be
    // sure to destroy the dialog.
    // The default OnCancel (above) simply ends a modal dialog, and hides a modeless dialog.

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
    if (!wxPendingDelete.Member(this))
        wxPendingDelete.Append(this);
    return TRUE;
}

void wxDialog::OnSize(wxSizeEvent& WXUNUSED(event))
{
  // if we're using constraints - do use them
  #if wxUSE_CONSTRAINTS
    if ( GetAutoLayout() )
    {
      Layout();
    }
  #endif
}

void wxDialog::OnSysColourChanged(wxSysColourChangedEvent& event)
{
  SetBackgroundColour(wxSystemSettings::GetSystemColour(wxSYS_COLOUR_3DFACE));
  Refresh();
}

MRESULT wxDialog::OS2WindowProc(WXUINT message, WXWPARAM wParam, WXLPARAM lParam)
{
    MRESULT rc = 0;
    bool    processed = FALSE;

    switch ( message )
    {
        case WM_CLOSE:
            // if we can't close, tell the system that we processed the
            // message - otherwise it would close us
            processed = !Close();
            break;
    }

    if ( !processed )
        rc = wxWindow::OS2WindowProc(message, wParam, lParam);

    return rc;
}

