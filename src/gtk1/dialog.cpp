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
#include "wx/gtk/win_gtk.h"

//-----------------------------------------------------------------------------

extern wxList wxPendingDelete;

//-----------------------------------------------------------------------------
// "delete_event"
//-----------------------------------------------------------------------------

bool gtk_dialog_delete_callback( GtkWidget *WXUNUSED(widget), GdkEvent *WXUNUSED(event), wxDialog *win )
{ 
/*
  printf( "OnDelete from " );
  if (win->GetClassInfo() && win->GetClassInfo()->GetClassName())
    printf( win->GetClassInfo()->GetClassName() );
  printf( ".\n" );
*/
  
  win->Close();

  return TRUE;
}

//-----------------------------------------------------------------------------
// wxDialog
//-----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(wxDialog,wxPanel)
  EVT_BUTTON  (wxID_OK,       wxDialog::OnOK)
  EVT_BUTTON  (wxID_CANCEL,   wxDialog::OnCancel)
  EVT_BUTTON  (wxID_APPLY,    wxDialog::OnApply)
  EVT_CLOSE   (wxDialog::OnCloseWindow)
END_EVENT_TABLE()

IMPLEMENT_DYNAMIC_CLASS(wxDialog,wxPanel)

wxDialog::wxDialog(void)
{
  m_title = "";
  m_modalShowing = FALSE;
  wxTopLevelWindows.Insert( this );
}

wxDialog::wxDialog( wxWindow *parent, 
      wxWindowID id, const wxString &title,
      const wxPoint &pos, const wxSize &size, 
      long style, const wxString &name )
{
  m_modalShowing = FALSE;
  wxTopLevelWindows.Insert( this );
  Create( parent, id, title, pos, size, style, name );
}

bool wxDialog::Create( wxWindow *parent,
      wxWindowID id, const wxString &title,
      const wxPoint &pos, const wxSize &size, 
      long style, const wxString &name )
{
  m_needParent = FALSE;
  
  PreCreation( parent, id, pos, size, style, name );
  
  m_widget = gtk_window_new( GTK_WINDOW_TOPLEVEL );
  GTK_WIDGET_UNSET_FLAGS( m_widget, GTK_CAN_FOCUS );
 
  gtk_widget_set( m_widget, "GtkWindow::allow_shrink", TRUE, NULL);
  
  gtk_signal_connect( GTK_OBJECT(m_widget), "delete_event", 
    GTK_SIGNAL_FUNC(gtk_dialog_delete_callback), (gpointer)this );
    
  m_wxwindow = gtk_myfixed_new();
  gtk_widget_show( m_wxwindow );
  GTK_WIDGET_UNSET_FLAGS( m_wxwindow, GTK_CAN_FOCUS );
  
  gtk_container_add( GTK_CONTAINER(m_widget), m_wxwindow );
  
  SetTitle( title );
  
  if ((m_x != -1) || (m_y != -1))
     gtk_widget_set_uposition( m_widget, m_x, m_y );
     
  gtk_widget_set_usize( m_widget, m_width, m_height );
     
  PostCreation();
  
  return TRUE;
}

wxDialog::~wxDialog(void)
{
  wxTopLevelWindows.DeleteObject( this );
  if (wxTopLevelWindows.Number() == 0) wxTheApp->ExitMainLoop();
}

void wxDialog::SetTitle(const wxString& title )
{
  m_title = title;
  if (m_title.IsNull()) m_title = "";
  gtk_window_set_title( GTK_WINDOW(m_widget), m_title );
}

wxString wxDialog::GetTitle(void) const
{
  return (wxString&)m_title;
}

void wxDialog::OnApply( wxCommandEvent &WXUNUSED(event) )
{
  if (Validate()) TransferDataFromWindow();
}

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
  }
}

void wxDialog::OnOK( wxCommandEvent &WXUNUSED(event) )
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
    }
  }
}

void wxDialog::OnPaint( wxPaintEvent& WXUNUSED(event) )
{
  // yes
}

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
  }
}

void wxDialog::ImplementSetPosition(void)
{
  if ((m_x != -1) || (m_y != -1))
     gtk_widget_set_uposition( m_widget, m_x, m_y );
}

void wxDialog::Centre( int direction )
{
  if (direction & wxHORIZONTAL == wxHORIZONTAL) m_x = (gdk_screen_width () - m_width) / 2;
  if (direction & wxVERTICAL == wxVERTICAL) m_y = (gdk_screen_height () - m_height) / 2;
  ImplementSetPosition();
}

bool wxDialog::Show( bool show )
{
  if (!show && IsModal())
  {
    EndModal( wxID_CANCEL );
  }

  wxWindow::Show( show );
  
  if (show) InitDialog();
  
  return TRUE;
}

bool wxDialog::IsModal(void) const
{
  return m_modalShowing;
}

void wxDialog::SetModal( bool WXUNUSED(flag) )
{
/*
  if (flag)
    m_windowStyle |= wxDIALOG_MODAL;
  else
    if (m_windowStyle & wxDIALOG_MODAL) m_windowStyle -= wxDIALOG_MODAL;
*/
  wxFAIL_MSG( "wxDialog:SetModal obsolete now" );
}

int wxDialog::ShowModal(void)
{
  if (IsModal())
  {
    wxFAIL_MSG( "wxDialog:ShowModal called twice" );
    return GetReturnCode();
  }

  Show( TRUE );
  
  m_modalShowing = TRUE;
  
  gtk_grab_add( m_widget );
  gtk_main();
  gtk_grab_remove( m_widget );
  
  return GetReturnCode();
}

void wxDialog::EndModal( int retCode )
{
  SetReturnCode( retCode );
  
  if (!IsModal())
  {
     wxFAIL_MSG( "wxDialog:EndModal called twice" );
     return;
  }
  
  m_modalShowing = FALSE;
  
  gtk_main_quit();
  
  Show( FALSE );
}

void wxDialog::InitDialog(void)
{
  wxWindow::InitDialog();
}

void wxDialog::SetIcon( const wxIcon &icon )
{
  m_icon = icon;
  if (!icon.Ok()) return;
  
  wxMask *mask = icon.GetMask();
  GdkBitmap *bm = (GdkBitmap *) NULL;
  if (mask) bm = mask->GetBitmap();
  
  gdk_window_set_icon( m_widget->window, (GdkWindow *) NULL, icon.GetPixmap(), bm );
}


