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

#if wxUSE_STATBOX

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

    if (!PreCreation( parent, pos, size ) ||
        !CreateBase( parent, id, pos, size, style, wxDefaultValidator, name ))
    {
        wxFAIL_MSG( wxT("wxStaticBox creation failed") );
	return FALSE;
    }

    m_isStaticBox = TRUE;
    
    if (label.IsEmpty())
        m_widget = gtk_frame_new( (char*) NULL );
    else
        m_widget = gtk_frame_new( m_label.mbc_str() );

    m_parent->DoAddChild( this );
  
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
    gtk_frame_set_label( frame, GetLabel().mbc_str() );
}

void wxStaticBox::ApplyWidgetStyle()
{
    SetWidgetStyle();
    gtk_widget_set_style( m_widget, m_widgetStyle );
}

#endif
