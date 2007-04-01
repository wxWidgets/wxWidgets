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

    m_widget = gtk_label_new(NULL);

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

#ifdef __WXGTK26__
    if (!gtk_check_version(2,6,0))
    {
        // set ellipsize mode
        PangoEllipsizeMode ellipsizeMode = PANGO_ELLIPSIZE_NONE;
        if ( style & wxST_ELLIPSIZE_START )
            ellipsizeMode = PANGO_ELLIPSIZE_START;
        else if ( style & wxST_ELLIPSIZE_MIDDLE )
            ellipsizeMode = PANGO_ELLIPSIZE_MIDDLE;
        else if ( style & wxST_ELLIPSIZE_END )
            ellipsizeMode = PANGO_ELLIPSIZE_END;

        gtk_label_set_ellipsize( GTK_LABEL(m_widget), ellipsizeMode );
    }
#endif // __WXGTK26__

    SetLabel(label);

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
    // we need to return the label just like it was passed to the last call
    // to SetLabel(): i.e. with wx-style mnemonics and with markup
    return wxControl::GetLabel();
}

void wxStaticText::SetLabel( const wxString& str )
{
    wxCHECK_RET( m_widget != NULL, wxT("invalid static text") );

    // save the label inside m_labelOrig in case user calls GetLabel() later
    m_labelOrig = str;

    wxString label(str);
    if (gtk_check_version(2,6,0) &&
        IsEllipsized())
    {
        // GTK+ < 2.6 does not support ellipsization:
        // since we need to use our generic code for ellipsization (which does not
        // behaves well in conjunction with markup; i.e. it may break the markup
        // validity erasing portions of the string), we also need to strip out
        // the markup (if present) from the label.

        label = GetEllipsizedLabelWithoutMarkup();
    }

    if ( HasFlag(wxST_MARKUP) )
        GTKSetLabelWithMarkupForLabel(GTK_LABEL(m_widget), label);
    else
        GTKSetLabelForLabel(GTK_LABEL(m_widget), label);

    // adjust the label size to the new label unless disabled
    if ( !HasFlag(wxST_NO_AUTORESIZE) && 
         !IsEllipsized() )  // if ellipsize is ON, then we don't want to get resized!
        SetSize( GetBestSize() );
}

bool wxStaticText::SetFont( const wxFont &font )
{
    const bool wasUnderlined = GetFont().GetUnderlined();

    bool ret = wxControl::SetFont(font);

    if ( font.GetUnderlined() != wasUnderlined )
    {
        // the underlines for mnemonics are incompatible with using attributes
        // so turn them off when setting underlined font and restore them when
        // unsetting it
        gtk_label_set_use_underline(GTK_LABEL(m_widget), wasUnderlined);

        if ( wasUnderlined )
        {
            // it's not underlined any more, remove the attributes we set
            gtk_label_set_attributes(GTK_LABEL(m_widget), NULL);
        }
        else // the text is underlined now
        {
            PangoAttrList *attrs = pango_attr_list_new();
            PangoAttribute *a = pango_attr_underline_new(PANGO_UNDERLINE_SINGLE);
            a->start_index = 0;
            a->end_index = (guint)-1;
            pango_attr_list_insert(attrs, a);
            gtk_label_set_attributes(GTK_LABEL(m_widget), attrs);
            pango_attr_list_unref(attrs);
        }
    }

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

    if (gtk_check_version(2,6,0))
    {
        // GTK+ < 2.6 does not support ellipsization - we need to run our
        // generic code (actually it will be run only if IsEllipsized() == true)
        UpdateLabel();
    }
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


// These functions should be used only when GTK+ < 2.6 by wxStaticTextBase::UpdateLabel()

wxString wxStaticText::DoGetLabel() const
{
    GtkLabel *label = GTK_LABEL(m_widget);
    return wxGTK_CONV_BACK( gtk_label_get_text( label ) );
}

void wxStaticText::DoSetLabel(const wxString& str)
{
    GTKSetLabelForLabel(GTK_LABEL(m_widget), str);
}

// static
wxVisualAttributes
wxStaticText::GetClassDefaultAttributes(wxWindowVariant WXUNUSED(variant))
{
    return GetDefaultAttributesFromGTKWidget(gtk_label_new);
}

#endif // wxUSE_STATTEXT
