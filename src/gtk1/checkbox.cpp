/////////////////////////////////////////////////////////////////////////////
// Name:        checkbox.cpp
// Purpose:
// Author:      Robert Roebling
// Created:     01/02/97
// Id:
// Copyright:   (c) 1998 Robert Roebling, Julian Smart and Markus Holzem
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////


#ifdef __GNUG__
#pragma implementation "checkbox.h"
#endif

#include "wx/checkbox.h"

//-----------------------------------------------------------------------------
// data
//-----------------------------------------------------------------------------

extern bool   g_blockEventsOnDrag;

//-----------------------------------------------------------------------------
// wxCheckBox
//-----------------------------------------------------------------------------

static void gtk_checkbox_clicked_callback( GtkWidget *WXUNUSED(widget), wxCheckBox *cb )
{
  if (!cb->HasVMT()) return;
  if (g_blockEventsOnDrag) return;
  
  wxCommandEvent event(wxEVT_COMMAND_CHECKBOX_CLICKED, cb->GetId());
  event.SetInt( cb->GetValue() );
  event.SetEventObject(cb);
  cb->GetEventHandler()->ProcessEvent(event);
};

//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxCheckBox,wxControl)

wxCheckBox::wxCheckBox(void)
{
};

wxCheckBox::wxCheckBox( wxWindow *parent, wxWindowID id, const wxString &label,
      const wxPoint &pos, const wxSize &size, 
      long style, const wxString &name )
{
  Create( parent, id, label, pos, size, style, name );
};

bool wxCheckBox::Create(  wxWindow *parent, wxWindowID id, const wxString &label,
      const wxPoint &pos, const wxSize &size, 
      long style, const wxString &name )
{
  m_needParent = TRUE;
  
  PreCreation( parent, id, pos, size, style, name );

  SetLabel( label );

  m_widget = gtk_check_button_new_with_label( label );
 
  wxSize newSize = size;
  if (newSize.x == -1) newSize.x = 25+gdk_string_measure( m_widget->style->font, label );
  if (newSize.y == -1) newSize.y = 26;
  SetSize( newSize.x, newSize.y );
   
  gtk_signal_connect( GTK_OBJECT(m_widget), "clicked", 
    GTK_SIGNAL_FUNC(gtk_checkbox_clicked_callback), (gpointer*)this );
    
  PostCreation();
  
  Show( TRUE );
    
  return TRUE;
};

void wxCheckBox::SetValue( bool state )
{
  if (state)
    gtk_toggle_button_set_state( GTK_TOGGLE_BUTTON(m_widget), GTK_STATE_ACTIVE );
  else
    gtk_toggle_button_set_state( GTK_TOGGLE_BUTTON(m_widget), GTK_STATE_NORMAL );
};

bool wxCheckBox::GetValue(void) const
{
  GtkToggleButton *tb = GTK_TOGGLE_BUTTON(m_widget);
  return tb->active;
};

