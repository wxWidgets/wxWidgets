/////////////////////////////////////////////////////////////////////////////
// Name:        radiobut.cpp
// Purpose:
// Author:      Robert Roebling
// Created:     01/02/97
// Id:
// Copyright:   (c) 1998 Robert Roebling, Julian Smart and Markus Holzem
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////


#ifdef __GNUG__
#pragma implementation "radiobut.h"
#endif

#include "wx/radiobut.h"

//-----------------------------------------------------------------------------
// data
//-----------------------------------------------------------------------------

extern bool g_blockEventsOnDrag;

//-----------------------------------------------------------------------------
// wxRadioButton
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxRadioButton,wxControl)
  
static void gtk_radiobutton_clicked_callback( GtkWidget *WXUNUSED(widget), wxRadioButton *rb )
{
  if (!rb->HasVMT()) return;
  if (g_blockEventsOnDrag) return;
  
  wxCommandEvent event( wxEVT_COMMAND_RADIOBUTTON_SELECTED, rb->GetId());
  event.SetInt( rb->GetValue() );
  event.SetEventObject( rb );
  rb->GetEventHandler()->ProcessEvent( event );
}

bool wxRadioButton::Create( wxWindow *parent, wxWindowID id, const wxString& label,
  const wxPoint& pos,  const wxSize& size, long style,
  const wxValidator& validator, const wxString& name )
{
  m_needParent = TRUE;

  wxSize newSize = size;

  PreCreation( parent, id, pos, newSize, style, name );
  
  SetValidator( validator );

  m_widget = gtk_radio_button_new_with_label( (GSList *) NULL, label );
      
  SetLabel(label);

  if (newSize.x == -1) newSize.x = 22+gdk_string_measure( m_widget->style->font, label );
  if (newSize.y == -1) newSize.y = 26;
  SetSize( newSize.x, newSize.y );

  gtk_signal_connect( GTK_OBJECT(m_widget), "clicked", 
    GTK_SIGNAL_FUNC(gtk_radiobutton_clicked_callback), (gpointer*)this );
       
  PostCreation();

  Show( TRUE );

  return TRUE;
}

void wxRadioButton::SetLabel( const wxString& label )
{
  wxControl::SetLabel( label );
  GtkButton *bin = GTK_BUTTON( m_widget );
  GtkLabel *g_label = GTK_LABEL( bin->child );
  gtk_label_set( g_label, GetLabel() );
}

void wxRadioButton::SetValue( bool val )
{
  gtk_toggle_button_set_state( GTK_TOGGLE_BUTTON(m_widget), val );
}

bool wxRadioButton::GetValue(void) const
{
  return GTK_TOGGLE_BUTTON(m_widget)->active;
}


