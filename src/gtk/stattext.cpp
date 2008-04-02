/////////////////////////////////////////////////////////////////////////////
// Name:        stattext.cpp
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_STATTEXT

#include "wx/stattext.h"
#include "wx/gtk/private.h"

#include "gdk/gdk.h"
#include "gtk/gtk.h"

extern "C"
void wxgtk_window_size_request_callback(GtkWidget *widget,
                                        GtkRequisition *requisition,
                                        wxWindow *win);

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
        wxFAIL_MSG( wxT("wxStaticText creation failed") );
        return FALSE;
    }

    const wxString labelGTK = GTKConvertMnemonics(label);
    m_label = label;
    m_widget = gtk_label_new_with_mnemonic(wxGTK_CONV(labelGTK));

    GtkJustification justify;
    if ( style & wxALIGN_CENTER )
      justify = GTK_JUSTIFY_CENTER;
    else if ( style & wxALIGN_RIGHT )
      justify = GTK_JUSTIFY_RIGHT;
    else // wxALIGN_LEFT is 0
      justify = GTK_JUSTIFY_LEFT;
      
    if (GetLayoutDirection() == wxLayout_RightToLeft)
    {  
         if (justify == GTK_JUSTIFY_RIGHT)
            justify = GTK_JUSTIFY_LEFT;
         if (justify == GTK_JUSTIFY_LEFT)
            justify = GTK_JUSTIFY_RIGHT;
    }
    
    gtk_label_set_justify(GTK_LABEL(m_widget), justify);

    // GTK_JUSTIFY_LEFT is 0, RIGHT 1 and CENTER 2
    static const float labelAlignments[] = { 0.0, 1.0, 0.5 };
    gtk_misc_set_alignment(GTK_MISC(m_widget), labelAlignments[justify], 0.0);

    gtk_label_set_line_wrap( GTK_LABEL(m_widget), TRUE );

    m_parent->DoAddChild( this );

    PostCreation(size);

    // the bug below only happens with GTK 2
    if ( justify != GTK_JUSTIFY_LEFT )
    {
        // if we let GTK call wxgtk_window_size_request_callback the label
        // always shrinks to its minimal size for some reason and so no
        // alignment except the default left doesn't work (in fact it does,
        // but you don't see it)
        g_signal_handlers_disconnect_by_func (m_widget,
                                              (gpointer) wxgtk_window_size_request_callback,
                                              this);
    }

    return TRUE;
}

wxString wxStaticText::GetLabel() const
{
    GtkLabel *label = GTK_LABEL(m_widget);
    wxString str = wxGTK_CONV_BACK( gtk_label_get_text( label ) );

    return wxString(str);
}

void wxStaticText::SetLabel( const wxString &label )
{
    wxCHECK_RET( m_widget != NULL, wxT("invalid static text") );

    GTKSetLabelForLabel(GTK_LABEL(m_widget), label);

    // adjust the label size to the new label unless disabled
    if ( !HasFlag(wxST_NO_AUTORESIZE) )
    {
        InvalidateBestSize();
        SetSize( GetBestSize() );
    }
}

bool wxStaticText::SetFont( const wxFont &font )
{
    bool ret = wxControl::SetFont(font);

    // adjust the label size to the new label unless disabled
    if (!HasFlag(wxST_NO_AUTORESIZE))
    {
        InvalidateBestSize();
        SetSize( GetBestSize() );
    }
    return ret;
}

void wxStaticText::DoSetSize(int x, int y,
                           int width, int height,
                           int sizeFlags )
{
    wxControl::DoSetSize( x, y, width, height, sizeFlags );
}

wxSize wxStaticText::DoGetBestSize() const
{
    // Do not return any arbitrary default value...
    wxASSERT_MSG( m_widget, wxT("wxStaticText::DoGetBestSize called before creation") );

    // GetBestSize is supposed to return unwrapped size but calling
    // gtk_label_set_line_wrap() from here is a bad idea as it queues another
    // size request by calling gtk_widget_queue_resize() and we end up in
    // infinite loop sometimes (notably when the control is in a toolbar)
    GTK_LABEL(m_widget)->wrap = FALSE;

    GtkRequisition req;
    req.width = -1;
    req.height = -1;
    (* GTK_WIDGET_CLASS( GTK_OBJECT_GET_CLASS(m_widget) )->size_request )
        (m_widget, &req );

    GTK_LABEL(m_widget)->wrap = TRUE; // restore old value

    // Adding 1 to width to workaround GTK sometimes wrapping the text needlessly
    return wxSize (req.width+1, req.height);
}

bool wxStaticText::SetForegroundColour(const wxColour& colour)
{
    // First, we call the base class member
    wxControl::SetForegroundColour(colour);
    // Then, to force the color change, we set the label with the current label
    SetLabel(GetLabel());
    return true;
}

bool wxStaticText::GTKWidgetNeedsMnemonic() const
{
    return true;
}

void wxStaticText::GTKWidgetDoSetMnemonic(GtkWidget* w)
{
    gtk_label_set_mnemonic_widget(GTK_LABEL(m_widget), w);
}

// static
wxVisualAttributes
wxStaticText::GetClassDefaultAttributes(wxWindowVariant WXUNUSED(variant))
{
    return GetDefaultAttributesFromGTKWidget(gtk_label_new);
}

#endif // wxUSE_STATTEXT
