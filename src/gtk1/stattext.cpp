/////////////////////////////////////////////////////////////////////////////
// Name:        stattext.cpp
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
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
}

wxStaticText::wxStaticText( wxWindow *parent, wxWindowID id, const wxString &label, 
      const wxPoint &pos, const wxSize &size, 
      long style, const wxString &name )
{
  Create( parent, id, label, pos, size, style, name );
}

bool wxStaticText::Create( wxWindow *parent, wxWindowID id, const wxString &label, 
      const wxPoint &pos, const wxSize &size, 
      long style, const wxString &name )
{
  m_needParent = TRUE;
  
  wxSize newSize = size;
  
  PreCreation( parent, id, pos, size, style, name );
  
  wxControl::SetLabel(label);
  m_widget = gtk_label_new( m_label );

  GtkJustification justify;
  if ( style & wxALIGN_CENTER )
    justify = GTK_JUSTIFY_CENTER;
  else if ( style & wxALIGN_RIGHT )
    justify = GTK_JUSTIFY_RIGHT;
  else // wxALIGN_LEFT is 0
    justify = GTK_JUSTIFY_LEFT;
  gtk_label_set_justify(GTK_LABEL(m_widget), justify);
  
   int y = 1;
   if (newSize.x == -1) 
   {
     char *s = WXSTRINGCAST m_label;
     char *nl = strchr(s, '\n');
     if (nl) 
     {
       do 
       {
         *nl = 0;
         int x = gdk_string_measure( m_widget->style->font, s ) + 4;
         if (x > newSize.x) newSize.x = x;
         *nl++ = '\n';
         ++y;
         if (! (nl = strchr(s = nl, '\n'))) 
         {
	 
           int x = gdk_string_measure( m_widget->style->font, s ) + 4;
           if (x > newSize.x) newSize.x = x;
         }
       } while (nl);
     } 
     else 
     {
       newSize.x = gdk_string_measure( m_widget->style->font, label ) + 4;
     }
   }
   if (newSize.y == -1) 
   {
     if (y == 1) 
       newSize.y = 26;
     else
       newSize.y = 4 + y * (m_widget->style->font->ascent + m_widget->style->font->descent + 2);
   }
  
  SetSize( newSize.x, newSize.y );
  
  m_parent->AddChild( this );

  (m_parent->m_insertCallback)( m_parent, this );
  
  PostCreation();
  
  SetBackgroundColour( parent->GetBackgroundColour() );
  SetForegroundColour( parent->GetForegroundColour() );

  Show( TRUE );
    
  return TRUE;
}

wxString wxStaticText::GetLabel(void) const
{
  char *str = (char *) NULL;
  gtk_label_get( GTK_LABEL(m_widget), &str );
  wxString tmp( str );
  return tmp;
}

void wxStaticText::SetLabel( const wxString &label )
{
  wxControl::SetLabel(label);

  gtk_label_set( GTK_LABEL(m_widget), m_label );
}

void wxStaticText::ApplyWidgetStyle()
{
  SetWidgetStyle();
  gtk_widget_set_style( m_widget, m_widgetStyle );
}
  
