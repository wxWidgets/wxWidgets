/////////////////////////////////////////////////////////////////////////////
// Name:        dialog.cpp
// Purpose:
// Author:      Robert Roebling
// Created:     01/02/97
// Id:
// Copyright:   (c) 1998 Robert Roebling, Julian Smart and Markus Holzem
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "dialog.h"
#endif

#include "wx/dialog.h"
#include "wx/frame.h"
#include "wx/app.h"

//-----------------------------------------------------------------------------

extern wxList wxPendingDelete;

//-----------------------------------------------------------------------------
// wxDialog
//-----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(wxDialog,wxWindow)
  EVT_BUTTON  (wxID_OK,       wxDialog::OnOk)
  EVT_BUTTON  (wxID_CANCEL,   wxDialog::OnCancel)
  EVT_BUTTON  (wxID_APPLY,    wxDialog::OnApply)
  EVT_CLOSE   (wxDialog::OnCloseWindow)
END_EVENT_TABLE()

IMPLEMENT_DYNAMIC_CLASS(wxDialog,wxWindow)

wxDialog::wxDialog(void)
{
  m_title = "";
  m_modalShowing = FALSE;
  wxTopLevelWindows.Insert( this );
};

wxDialog::wxDialog( wxWindow *parent, 
      wxWindowID id, const wxString &title,
      const wxPoint &pos, const wxSize &size, 
      long style, const wxString &name )
{
  m_modalShowing = FALSE;
  wxTopLevelWindows.Insert( this );
  Create( parent, id, title, pos, size, style, name );
};

bool wxDialog::Create( wxWindow *parent,
      wxWindowID id, const wxString &title,
      const wxPoint &pos, const wxSize &size, 
      long style, const wxString &name )
{
  return TRUE;
};

wxDialog::~wxDialog(void)
{
  wxTopLevelWindows.DeleteObject( this );
  if (wxTopLevelWindows.Number() == 0) wxTheApp->ExitMainLoop();
};

void wxDialog::SetTitle(const wxString& title )
{
  m_title = title;
};

wxString wxDialog::GetTitle(void) const
{
  return (wxString&)m_title;
};

void wxDialog::OnApply( wxCommandEvent &WXUNUSED(event) )
{
  if (Validate()) TransferDataFromWindow();
};

void wxDialog::OnCancel( wxCommandEvent &WXUNUSED(event) )
{
  if (IsModal())
  {
    EndModal(wxID_CANCEL);
  }
  else
  {
    SetReturnCode(wxID_CANCEL);
    this->Show(FALSE);
  };
};

void wxDialog::OnOk( wxCommandEvent &WXUNUSED(event) )
{
  if ( Validate() && TransferDataFromWindow())
  {
    if (IsModal()) 
    {
      EndModal(wxID_OK);
    }
    else
    {
      SetReturnCode(wxID_OK);
      this->Show(FALSE);
    };
  };
};

void wxDialog::OnPaint( wxPaintEvent& WXUNUSED(event) )
{
  // yes
};

bool wxDialog::OnClose(void)
{
  static wxList closing;

  if (closing.Member(this)) return FALSE;   // no loops
  
  closing.Append(this);

  wxCommandEvent cancelEvent(wxEVT_COMMAND_BUTTON_CLICKED, wxID_CANCEL);
  cancelEvent.SetEventObject( this );
  GetEventHandler()->ProcessEvent(cancelEvent);
  closing.DeleteObject(this);
  
  return FALSE;
}

bool wxDialog::Destroy(void)
{
  if (!wxPendingDelete.Member(this))
    wxPendingDelete.Append(this);

  return TRUE;
}

void wxDialog::OnCloseWindow(wxCloseEvent& event)
{
  if (GetEventHandler()->OnClose() || event.GetForce())
  {
    this->Destroy();
  };
};

bool wxDialog::Show( bool show )
{
  if (!show && IsModal() && m_modalShowing)
  {
    EndModal( wxID_CANCEL );
  };

  wxWindow::Show( show );
  
  if (show) InitDialog();
  
  return TRUE;
};

int wxDialog::ShowModal(void)
{
  if (m_modalShowing) return GetReturnCode();

  Show( TRUE );
  
  m_modalShowing = TRUE;
  
  // grab here
  // main here
  // release here
  
  return GetReturnCode();
};

void wxDialog::EndModal( int retCode )
{
  SetReturnCode( retCode );
  
  if (!m_modalShowing) return;
  m_modalShowing = FALSE;
  
  // quit main 
};

void wxDialog::InitDialog(void)
{
  wxWindow::InitDialog();
};

