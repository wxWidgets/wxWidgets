/////////////////////////////////////////////////////////////////////////////
// Name:        src/gtk/stattext.cpp
// Purpose:
// Author:      Robert Roebling
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_STATTEXT

#include "wx/stattext.h"

#ifndef WX_PRECOMP
    #include "wx/dcclient.h"
#endif

#if wxUSE_MARKUP
    #include "wx/generic/private/markuptext.h"
#endif // wxUSE_MARKUP

#include "wx/gtk/private.h"

//-----------------------------------------------------------------------------
// wxStaticText
//-----------------------------------------------------------------------------

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
    if (!PreCreation( parent, pos, size ) ||
        !CreateBase( parent, id, pos, size, style, wxDefaultValidator, name ))
    {
        wxFAIL_MSG( wxT("wxStaticText creation failed") );
        return false;
    }

    m_widget = gtk_label_new(nullptr);
    g_object_ref(m_widget);

    GtkJustification justify;
    if ( style & wxALIGN_CENTER_HORIZONTAL )
    {
#ifndef __WXGTK3__
        // This looks like a bug in GTK+ and seems to be fixed in GTK+3, but
        // using non-default justification with default ellipsize mode doesn't
        // work: the justification is just ignored. In practice, alignment is
        // more important, so turn on ellipsize mode even if it was not
        // specified to make it work if necessary.
        if ( !(style & wxST_ELLIPSIZE_MASK) )
            style |= wxST_ELLIPSIZE_MIDDLE;
#endif // GTK+ 2

        justify = GTK_JUSTIFY_CENTER;
    }
    else if ( style & wxALIGN_RIGHT )
    {
#ifndef __WXGTK3__
        // As above, we need to use a non-default ellipsize mode for the
        // alignment to have any effect.
        if ( !(style & wxST_ELLIPSIZE_MASK) )
            style |= wxST_ELLIPSIZE_START;
#endif // GTK+ 2

        justify = GTK_JUSTIFY_RIGHT;
    }
    else // must be wxALIGN_LEFT which is 0
    {
        // No need to play games with wxST_ELLIPSIZE_XXX.
        justify = GTK_JUSTIFY_LEFT;
    }

    if (GetLayoutDirection() == wxLayout_RightToLeft)
    {
        if (justify == GTK_JUSTIFY_RIGHT)
            justify = GTK_JUSTIFY_LEFT;
        else if (justify == GTK_JUSTIFY_LEFT)
            justify = GTK_JUSTIFY_RIGHT;
    }

    gtk_label_set_justify(GTK_LABEL(m_widget), justify);

    // set ellipsize mode
    PangoEllipsizeMode ellipsizeMode = PANGO_ELLIPSIZE_NONE;
    if ( style & wxST_ELLIPSIZE_START )
        ellipsizeMode = PANGO_ELLIPSIZE_START;
    else if ( style & wxST_ELLIPSIZE_MIDDLE )
        ellipsizeMode = PANGO_ELLIPSIZE_MIDDLE;
    else if ( style & wxST_ELLIPSIZE_END )
        ellipsizeMode = PANGO_ELLIPSIZE_END;

    gtk_label_set_ellipsize( GTK_LABEL(m_widget), ellipsizeMode );

    // GTK_JUSTIFY_LEFT is 0, RIGHT 1 and CENTER 2
    static const float labelAlignments[] = { 0.0, 1.0, 0.5 };
#ifdef __WXGTK4__
    g_object_set(m_widget, "xalign", labelAlignments[justify], nullptr);
#else
    wxGCC_WARNING_SUPPRESS(deprecated-declarations)
    gtk_misc_set_alignment(GTK_MISC(m_widget), labelAlignments[justify], 0.0);
    wxGCC_WARNING_RESTORE()
#endif

    gtk_label_set_line_wrap( GTK_LABEL(m_widget), TRUE );

    SetLabel(label);

    m_parent->DoAddChild( this );

    PostCreation(size);

#ifndef __WXGTK3__
    // GtkLabel does its layout based on its size-request, rather than its
    // actual size. The size-request may not always get set, specifically if
    // the initial size is fully specified. So make sure it's set here.
    gtk_widget_set_size_request(m_widget, m_width, m_height);
#endif

    return true;
}

void wxStaticText::GTKDoSetLabel(GTKLabelSetter setter, const wxString& label)
{
    wxCHECK_RET( m_widget != nullptr, wxT("invalid static text") );

    (this->*setter)(GTK_LABEL(m_widget), label);

    AutoResizeIfNecessary();
}

void wxStaticText::SetLabel(const wxString& label)
{
    if ( label == m_labelOrig )
        return;

    m_labelOrig = label;

    GTKDoSetLabel(&wxStaticText::GTKSetLabelForLabel, label);
}

#if wxUSE_MARKUP

bool wxStaticText::DoSetLabelMarkup(const wxString& markup)
{
    const wxString stripped = RemoveMarkup(markup);
    if ( stripped.empty() && !markup.empty() )
        return false;

    m_labelOrig = stripped;

    GTKDoSetLabel(&wxStaticText::GTKSetLabelWithMarkupForLabel, markup);

    return true;
}

#endif // wxUSE_MARKUP

bool wxStaticText::SetFont( const wxFont &font )
{
    const bool wasUnderlined = GetFont().GetUnderlined();
    const bool wasStrickenThrough = GetFont().GetStrikethrough();

    if ( !wxControl::SetFont(font) )
        return false;

    if ( !IsShownOnScreen() )
    {
        // Setting the font of a hidden window doesn't update GTK style cache,
        // see #16088, and the size computed by GTK will be wrong, so we will
        // need to compute it ourselves.
        m_computeOurOwnBestSize = true;
    }

    const bool isUnderlined = GetFont().GetUnderlined();
    const bool isStrickenThrough = GetFont().GetStrikethrough();

    if ( (isUnderlined != wasUnderlined) ||
            (isStrickenThrough != wasStrickenThrough) )
    {
        // We need to update the Pango attributes used for the text.
        if ( isUnderlined || isStrickenThrough )
        {
            PangoAttrList* const attrs = pango_attr_list_new();
            if ( isUnderlined )
            {
                PangoAttribute *a = pango_attr_underline_new(PANGO_UNDERLINE_SINGLE);
                a->start_index = 0;
                a->end_index = (guint)-1;
                pango_attr_list_insert(attrs, a);
            }

            if ( isStrickenThrough )
            {
                PangoAttribute *a = pango_attr_strikethrough_new( TRUE );
                a->start_index = 0;
                a->end_index = (guint) -1;
                pango_attr_list_insert(attrs, a);
            }

            gtk_label_set_attributes(GTK_LABEL(m_widget), attrs);
            pango_attr_list_unref(attrs);
        }
        else // No special attributes any more.
        {
            // Just remove any attributes we had set.
            gtk_label_set_attributes(GTK_LABEL(m_widget), nullptr);
        }

        // The underlines for mnemonics are incompatible with using attributes
        // so turn them off when setting underlined font.
        gtk_label_set_use_underline(GTK_LABEL(m_widget), !isUnderlined);
    }

    AutoResizeIfNecessary();

    return true;
}

wxSize wxStaticText::DoGetBestSize() const
{
    // Do not return any arbitrary default value...
    wxASSERT_MSG( m_widget, wxT("wxStaticText::DoGetBestSize called before creation") );

    wxSize size;
    if ( m_computeOurOwnBestSize )
    {
        // GTK style cache may not be up to date, so we can't trust the results
        // of wxControl::DoGetBestSize() and need to compute the best size
        // ourselves here.
        wxInfoDC dc(wxConstCast(this, wxStaticText));

        const wxString
            label = wxString::FromUTF8(gtk_label_get_label(GTK_LABEL(m_widget)));

#if wxUSE_MARKUP
        if ( gtk_label_get_use_markup(GTK_LABEL(m_widget)) )
        {
            wxMarkupText markupText(label);
            size = markupText.Measure(dc);
        }
        else
#endif // wxUSE_MARKUP
        {
            size = dc.GetMultiLineTextExtent(label);
        }
    }
    else
    {
        // GetBestSize is supposed to return unwrapped size but calling
        // gtk_label_set_line_wrap() from here is a bad idea as it queues another
        // size request by calling gtk_widget_queue_resize() and we end up in
        // infinite loop sometimes (notably when the control is in a toolbar)
        // With GTK3 however, there is no simple alternative, and the sizing loop
        // no longer seems to occur.
#ifdef __WXGTK3__
        gtk_label_set_line_wrap(GTK_LABEL(m_widget), false);
#else
        GTK_LABEL(m_widget)->wrap = FALSE;

        // Reset the ellipsize mode while computing the best size, otherwise it's
        // going to be too small as the control knows that it can be shrunk to the
        // bare minimum and just hide most of the text replacing it with ellipsis.
        // This is especially important because we can enable ellipsization
        // implicitly for GTK+ 2, see the code dealing with alignment in the ctor.
        const PangoEllipsizeMode ellipsizeMode = gtk_label_get_ellipsize(GTK_LABEL(m_widget));
        gtk_label_set_ellipsize(GTK_LABEL(m_widget), PANGO_ELLIPSIZE_NONE);
#endif
        size = wxStaticTextBase::DoGetBestSize();
#ifdef __WXGTK3__
        gtk_label_set_line_wrap(GTK_LABEL(m_widget), true);
#else
        gtk_label_set_ellipsize(GTK_LABEL(m_widget), ellipsizeMode);
        GTK_LABEL(m_widget)->wrap = TRUE; // restore old value
#endif
    }

    // Adding 1 to width to workaround GTK sometimes wrapping the text needlessly
    size.x++;
    return size;
}

bool wxStaticText::GTKWidgetNeedsMnemonic() const
{
    return true;
}

void wxStaticText::GTKWidgetDoSetMnemonic(GtkWidget* w)
{
    gtk_label_set_mnemonic_widget(GTK_LABEL(m_widget), w);
}


// These functions are not used as GTK supports ellipsization natively and we
// never call the base class UpdateText() which uses them.
//
// Note that, unfortunately, we still need to define them because they still
// exist, as pure virtuals, in the base class even in wxGTK to allow
// wxGenericStaticText to override them.

wxString wxStaticText::WXGetVisibleLabel() const
{
    wxFAIL_MSG(wxS("Unreachable"));

    return wxString();
}

void wxStaticText::WXSetVisibleLabel(const wxString& WXUNUSED(str))
{
    wxFAIL_MSG(wxS("Unreachable"));
}

// static
wxVisualAttributes
wxStaticText::GetClassDefaultAttributes(wxWindowVariant WXUNUSED(variant))
{
    return GetDefaultAttributesFromGTKWidget(gtk_label_new(""));
}

#endif // wxUSE_STATTEXT
