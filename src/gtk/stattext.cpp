/////////////////////////////////////////////////////////////////////////////
// Name:        stattext.cpp
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////


#ifdef __GNUG__
    #pragma implementation "stattext.h"
#endif

#include "wx/stattext.h"

#include "gdk/gdk.h"
#include "gtk/gtk.h"

//-----------------------------------------------------------------------------
// wxStaticText
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxStaticText,wxControl)

wxStaticText::wxStaticText()
{
}

wxStaticText::wxStaticText(wxWindow *parent,
                           wxWindowID id,
                           const wxString &label,
                           const wxPoint &pos,
                           const wxSize &size,
                           long style,
                           const wxString &name)
{
  Create( parent, id, label, pos, size, style, name );
}

bool wxStaticText::Create(wxWindow *parent,
                          wxWindowID id,
                          const wxString &label,
                          const wxPoint &pos,
                          const wxSize &size,
                          long style,
                          const wxString &name )
{
    m_needParent = TRUE;

    if (!PreCreation( parent, pos, size ) ||
        !CreateBase( parent, id, pos, size, style, wxDefaultValidator, name ))
    {
        wxFAIL_MSG( wxT("wxXX creation failed") );
        return FALSE;
    }

    // notice that we call the base class version which will just remove the
    // '&' characters from the string, but not set the label's text to it
    // because the label is not yet created and because SetLabel() has a side
    // effect of changing the control size which might not be desirable
    wxControl::SetLabel(label);
    m_widget = gtk_label_new( m_label.mbc_str() );
    
    gtk_label_set_line_wrap( GTK_LABEL(m_widget), FALSE );

    SetFont( parent->GetFont() );

    GtkJustification justify;
    if ( style & wxALIGN_CENTER )
      justify = GTK_JUSTIFY_CENTER;
    else if ( style & wxALIGN_RIGHT )
      justify = GTK_JUSTIFY_RIGHT;
    else // wxALIGN_LEFT is 0
      justify = GTK_JUSTIFY_LEFT;
    gtk_label_set_justify(GTK_LABEL(m_widget), justify);

    // GTK_JUSTIFY_LEFT is 0, RIGHT 1 and CENTER 2
    static const float labelAlignments[] = { 0.0, 1.0, 0.5 };
    gtk_misc_set_alignment(GTK_MISC(m_widget), labelAlignments[justify], 0.0);

    GtkRequisition req;
    (* GTK_WIDGET_CLASS( GTK_OBJECT(m_widget)->klass )->size_request ) (m_widget, &req );
    
    wxSize newSize = size;
    if (newSize.x == -1) newSize.x = req.width;
    if (newSize.y == -1) newSize.y = req.height;
    SetSize( newSize.x, newSize.y );

    m_parent->DoAddChild( this );

    PostCreation();

    SetBackgroundColour( parent->GetBackgroundColour() );
    SetForegroundColour( parent->GetForegroundColour() );
    Show( TRUE );

    return TRUE;
}

wxString wxStaticText::GetLabel() const
{
    char *str = (char *) NULL;
    gtk_label_get( GTK_LABEL(m_widget), &str );

    return wxString(str);
}

void wxStaticText::SetLabel( const wxString &label )
{
    wxControl::SetLabel(label);

    gtk_label_set( GTK_LABEL(m_widget), m_label.mbc_str() );

    // adjust the label size to the new label unless disabled
    if ( !(GetWindowStyle() & wxST_NO_AUTORESIZE) )
    {
        GtkRequisition req;
        (* GTK_WIDGET_CLASS( GTK_OBJECT(m_widget)->klass )->size_request )
            (m_widget, &req );

        SetSize( req.width, req.height );
    }
}

void wxStaticText::ApplyWidgetStyle()
{
    SetWidgetStyle();
    gtk_widget_set_style( m_widget, m_widgetStyle );
}

