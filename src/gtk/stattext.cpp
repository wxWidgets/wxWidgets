/////////////////////////////////////////////////////////////////////////////
// Name:        stattext.cpp
// Purpose:
// Author:      Robert Roebling
// Created:     01/02/97
// Id:
// Copyright:   (c) 1998 Robert Roebling, Julian Smart and Markus Holzem
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////


#ifdef __GNUG__
#pragma implementation "stattext.h"
#endif

#include "wx/stattext.h"

//-----------------------------------------------------------------------------
// wxStaticText
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxStaticText,wxControl)

wxStaticText::wxStaticText(void)
{
};

wxStaticText::wxStaticText( wxWindow *parent, wxWindowID id, const wxString &label, 
      const wxPoint &pos, const wxSize &size, 
      const long style, const wxString &name )
{
  Create( parent, id, label, pos, size, style, name );
};

bool wxStaticText::Create( wxWindow *parent, wxWindowID id, const wxString &label, 
      const wxPoint &pos, const wxSize &size, 
      const long style, const wxString &name )
{
  m_needParent = TRUE;
  
  wxSize newSize = size;
  
  PreCreation( parent, id, pos, size, style, name );
  
  m_widget = gtk_label_new( label );
  
  if (newSize.x == -1) newSize.x = gdk_string_measure( m_widget->style->font, label );
  if (newSize.y == -1) newSize.y = 26;
  SetSize( newSize.x, newSize.y );
  
  PostCreation();
  
  Show( TRUE );
    
  return TRUE;
};

wxString wxStaticText::GetLabel(void) const
{
  char *str = NULL;
  gtk_label_get( GTK_LABEL(m_widget), &str );
  wxString tmp( str );
  return tmp;
};

void wxStaticText::SetLabel( const wxString &label )
{
  gtk_label_set( GTK_LABEL(m_widget), label );
};
