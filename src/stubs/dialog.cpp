/////////////////////////////////////////////////////////////////////////////
// Name:        dialog.cpp
// Purpose:     wxDialog class
// Author:      AUTHOR
// Modified by:
// Created:     ??/??/98
// RCS-ID:      $Id$
// Copyright:   (c) AUTHOR
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "dialog.h"
#endif

#include "wx/dialog.h"
#include "wx/utils.h"
#include "wx/frame.h"
#include "wx/app.h"
#include "wx/settings.h"

// Lists to keep track of windows, so we can disable/enable them
// for modal dialogs
wxList wxModalDialogs;
wxList wxModelessWindows;  // Frames and modeless dialogs
extern wxList wxPendingDelete;

#if !USE_SHARED_LIBRARY
IMPLEMENT_DYNAMIC_CLASS(wxDialog, wxPanel)

BEGIN_EVENT_TABLE(wxDialog, wxPanel)
  EVT_BUTTON(wxID_OK, wxDialog::OnOK)
  EVT_BUTTON(wxID_APPLY, wxDialog::OnApply)
  EVT_BUTTON(wxID_CANCEL, wxDialog::OnCancel)
  EVT_CHAR_HOOK(wxDialog::OnCharHook)
  EVT_SYS_COLOUR_CHANGED(wxDialog::OnSysColourChanged)
  EVT_CLOSE(wxDialog::OnCloseWindow)
END_EVENT_TABLE()

#endif

wxDialog::wxDialog()
{
    SetBackgroundColour(wxSystemSettings::GetSystemColour(wxSYS_COLOUR_3DFACE));
}

bool wxDialog::Create(wxWindow *parent, wxWindowID id,
           const wxString& title,
           const wxPoint& pos,
           const wxSize& size,
           long style,
           const wxString& name)
{
  m_windowStyle = style;

  SetBackgroundColour(wxSystemSettings::GetSystemColour(wxSYS_COLOUR_3DFACE));
  SetName(name);
  
  if (!parent)
    wxTopLevelWindows.Append(this);

  if (parent) parent->AddChild(this);

  if ( id == -1 )
  	m_windowId = (int)NewControlId();
  else
	m_windowId = id;

  // TODO: create dialog

  return FALSE;
}

void wxDialog::SetModal(bool flag)
{
	if ( flag )
		m_windowStyle |= wxDIALOG_MODAL ;
	else
		if ( m_windowStyle & wxDIALOG_MODAL )
  			m_windowStyle -= wxDIALOG_MODAL ;
  
  wxModelessWindows.DeleteObject(this);
  if (!flag)
    wxModelessWindows.Append(this);
}

wxDialog::~wxDialog()
{
    // TODO
    wxTopLevelWindows.DeleteObject(this);

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
  if (event.m_keyCode == WXK_ESCAPE)
  {
		// Behaviour changed in 2.0: we'll send a Cancel message
		// to the dialog instead of Close.
		wxCommandEvent cancelEvent(wxEVT_COMMAND_BUTTON_CLICKED, wxID_CANCEL);
		cancelEvent.SetEventObject( this );
		GetEventHandler()->ProcessEvent(cancelEvent);

		return;
  }
  // We didn't process this event.
  event.Skip();
}

void wxDialog::Iconize(bool WXUNUSED(iconize))
{
    // TODO
}

bool wxDialog::IsIconized() const
{
    // TODO
    return FALSE;
}

void wxDialog::SetClientSize(int width, int height)
{
    // TODO
}

void wxDialog::GetPosition(int *x, int *y) const
{
    // TODO
}

bool wxDialog::Show(bool show)
{
    // TODO
    return FALSE;
}

void wxDialog::SetTitle(const wxString& title)
{
    // TODO
}

wxString wxDialog::GetTitle() const
{
    // TODO
    return wxString("");
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
    // TODO: modal showing
	Show(TRUE);
	return GetReturnCode();
}

void wxDialog::EndModal(int retCode)
{
	SetReturnCode(retCode);
    // TODO modal un-showing
	Show(FALSE);
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

bool wxDialog::OnClose()
{
	// Behaviour changed in 2.0: we'll send a Cancel message by default,
    // which may close the dialog.
    // Check for looping if the Cancel event handler calls Close()

    static wxList closing;

    if ( closing.Member(this) )
        return FALSE;

    closing.Append(this);

	wxCommandEvent cancelEvent(wxEVT_COMMAND_BUTTON_CLICKED, wxID_CANCEL);
	cancelEvent.SetEventObject( this );
	GetEventHandler()->ProcessEvent(cancelEvent);

    closing.DeleteObject(this);

	return FALSE;
}

void wxDialog::OnCloseWindow(wxCloseEvent& event)
{
    // Compatibility
    if ( GetEventHandler()->OnClose() || event.GetForce())
    {
        this->Destroy();
    }
}

// Destroy the window (delayed, if a managed window)
bool wxDialog::Destroy()
{
  if (!wxPendingDelete.Member(this))
    wxPendingDelete.Append(this);
  return TRUE;
}

void wxDialog::OnSysColourChanged(wxSysColourChangedEvent& event)
{
  SetBackgroundColour(wxSystemSettings::GetSystemColour(wxSYS_COLOUR_3DFACE));
  Refresh();
}

void wxDialog::Fit()
{
}
