/////////////////////////////////////////////////////////////////////////////
// Name:        button.cpp
// Purpose:
// Author:      Robert Roebling
// Created:     01/02/97
// Id:
// Copyright:   (c) 1998 Robert Roebling, Julian Smart and Markus Holzem
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "button.h"
#endif

#include "wx/button.h"

//-----------------------------------------------------------------------------
// classes
//-----------------------------------------------------------------------------

class wxButton;

//-----------------------------------------------------------------------------
// wxButton
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxButton,wxControl)

void gtk_button_clicked_callback( GtkWidget *WXUNUSED(widget), gpointer data )
{
  wxButton *button = (wxButton*)data;
  wxCommandEvent event(wxEVT_COMMAND_BUTTON_CLICKED, button->GetId());
  event.SetEventObject(button);
  button->ProcessEvent(event);
};

//-----------------------------------------------------------------------------

wxButton::wxButton(void)
{
};

wxButton::wxButton( wxWindow *parent, wxWindowID id, const wxString &label,
      const wxPoint &pos, const wxSize &size, 
      long style, const wxString &name )
{
  Create( parent, id, label, pos, size, style, name );
};

bool wxButton::Create(  wxWindow *parent, wxWindowID id, const wxString &label,
      const wxPoint &pos, const wxSize &size, 
      long style, const wxString &name )
{
  m_needParent = TRUE;
  
  wxSize newSize = size;

  PreCreation( parent, id, pos, newSize, style, name );
  
  m_label = label;
  m_widget = gtk_button_new_with_label( label );
  
  if (newSize.x == -1) newSize.x = 15+gdk_string_measure( m_widget->style->font, label );
  if (newSize.y == -1) newSize.y = 26;
  SetSize( newSize.x, newSize.y );
  
  gtk_signal_connect( GTK_OBJECT(m_widget), "clicked", 
    GTK_SIGNAL_FUNC(gtk_button_clicked_callback), (gpointer*)this );

  PostCreation();
  
  Show( TRUE );
    
  return TRUE;
};
      
void wxButton::SetDefault(void)
{
};

void wxButton::SetLabel( const wxString &label )
{
  wxControl::SetLabel( label );
};

wxString wxButton::GetLabel(void) const
{
  return wxControl::GetLabel();
};
