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

    // do not move this call elsewhere
    gtk_label_set_line_wrap( GTK_LABEL(m_widget), FALSE );

    m_parent->DoAddChild( this );

    PostCreation();
    
    ApplyWidgetStyle();

    wxControl::SetFont( parent->GetFont() );
    
    wxSize size_best( DoGetBestSize() );
    wxSize new_size( size );
    if (new_size.x == -1)
        new_size.x = size_best.x;
    if (new_size.y == -1)
        new_size.y = size_best.y;
    if ((new_size.x != size.x) || (new_size.y != size.y))
        SetSize( new_size.x, new_size.y );

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
    if (!HasFlag(wxST_NO_AUTORESIZE))
        SetSize( GetBestSize() );
}

bool wxStaticText::SetFont( const wxFont &font )
{
    bool ret = wxControl::SetFont(font);

    // adjust the label size to the new label unless disabled
    if (!HasFlag(wxST_NO_AUTORESIZE))
        SetSize( GetBestSize() );
    
    return ret;
}

void wxStaticText::ApplyWidgetStyle()
{
    SetWidgetStyle();
    gtk_widget_set_style( m_widget, m_widgetStyle );
}

wxSize wxStaticText::DoGetBestSize() const
{
    // Do not return any arbitrary default value...
    wxASSERT_MSG( m_widget, wxT("wxStaticText::DoGetBestSize called before creation") );

    // this invalidates the size request
    gtk_label_set_line_wrap( GTK_LABEL(m_widget), TRUE );
    gtk_label_set_line_wrap( GTK_LABEL(m_widget), FALSE );

    GtkRequisition req;
    req.width = 2;
    req.height = 2;
    (* GTK_WIDGET_CLASS( GTK_OBJECT(m_widget)->klass )->size_request )
        (m_widget, &req );

    return wxSize(req.width, req.height);
}

