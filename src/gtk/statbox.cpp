/////////////////////////////////////////////////////////////////////////////
// Name:        statbox.cpp
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "statbox.h"
#endif

#include "wx/statbox.h"

#include "gdk/gdk.h"
#include "gtk/gtk.h"

//-----------------------------------------------------------------------------
// wxStaticBox
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxStaticBox,wxControl)

wxStaticBox::wxStaticBox(void)
{
}

wxStaticBox::wxStaticBox( wxWindow *parent, wxWindowID id, const wxString &label,
      const wxPoint &pos, const wxSize &size,
      long style, const wxString &name )
{
    Create( parent, id, label, pos, size, style, name );
}

bool wxStaticBox::Create( wxWindow *parent, wxWindowID id, const wxString &label,
      const wxPoint &pos, const wxSize &size,
      long style, const wxString &name )
{
    m_needParent = TRUE;

    PreCreation( parent, id, pos, size, style, name );

    m_isStaticBox = TRUE;
    
    m_widget = gtk_frame_new(m_label);

    m_parent->AddChild( this );

    (m_parent->m_insertCallback)( m_parent, this );
  
    PostCreation();

    SetLabel(label);
  
    SetBackgroundColour( parent->GetBackgroundColour() );
    SetForegroundColour( parent->GetForegroundColour() );
    SetFont( parent->GetFont() );

    Show( TRUE );

    return TRUE;
}

void wxStaticBox::SetLabel( const wxString &label )
{
    wxControl::SetLabel( label );
    GtkFrame *frame = GTK_FRAME( m_widget );
    gtk_frame_set_label( frame, GetLabel() );
}

void wxStaticBox::ApplyWidgetStyle()
{
    SetWidgetStyle();
    gtk_widget_set_style( m_widget, m_widgetStyle );
}

