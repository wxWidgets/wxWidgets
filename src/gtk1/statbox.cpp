/////////////////////////////////////////////////////////////////////////////
// Name:        gtk/statbox.cpp
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "statbox.h"
#endif

#include "wx/defs.h"

#if wxUSE_STATBOX

#include "wx/statbox.h"
#include "wx/gtk/private.h"

#include "gdk/gdk.h"
#include "gtk/gtk.h"

//-----------------------------------------------------------------------------
// wxStaticBox
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxStaticBox, wxControl)

wxStaticBox::wxStaticBox()
{
}

wxStaticBox::wxStaticBox( wxWindow *parent,
                          wxWindowID id,
                          const wxString &label,
                          const wxPoint& pos,
                          const wxSize& size,
                          long style,
                          const wxString& name )
{
    Create( parent, id, label, pos, size, style, name );
}

bool wxStaticBox::Create( wxWindow *parent,
                          wxWindowID id,
                          const wxString& label,
                          const wxPoint& pos,
                          const wxSize& size,
                          long style,
                          const wxString& name )
{
    m_needParent = TRUE;

    if (!PreCreation( parent, pos, size ) ||
        !CreateBase( parent, id, pos, size, style, wxDefaultValidator, name ))
    {
        wxFAIL_MSG( wxT("wxStaticBox creation failed") );
        return FALSE;
    }

    wxControl::SetLabel(label);

    m_widget = gtk_frame_new(m_label.empty() ? (char *)NULL : (const char*) wxGTK_CONV( m_label ) );

    m_parent->DoAddChild( this );

    PostCreation();

    InheritAttributes();

    // need to set non default alignment?
    gfloat xalign;
    if ( style & wxALIGN_CENTER )
        xalign = 0.5;
    else if ( style & wxALIGN_RIGHT )
        xalign = 1.0;
    else // wxALIGN_LEFT
        xalign = 0.0;

    if ( xalign )
        gtk_frame_set_label_align(GTK_FRAME( m_widget ), xalign, 0.0);

    Show( TRUE );

    return TRUE;
}

void wxStaticBox::SetLabel( const wxString &label )
{
    wxControl::SetLabel( label );

    gtk_frame_set_label( GTK_FRAME( m_widget ),
                         m_label.empty() ? (char *)NULL : (const char*) wxGTK_CONV( m_label ) );
}

void wxStaticBox::ApplyWidgetStyle()
{
    SetWidgetStyle();
    gtk_widget_set_style( m_widget, m_widgetStyle );
}

#endif // wxUSE_STATBOX
