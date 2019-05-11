/////////////////////////////////////////////////////////////////////////////
// Name:        src/gtk/clrpicker.cpp
// Purpose:     implementation of wxColourButton
// Author:      Francesco Montorsi
// Modified By:
// Created:     15/04/2006
// Copyright:   (c) Francesco Montorsi
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////


// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_COLOURPICKERCTRL

#include "wx/clrpicker.h"

#include "wx/gtk/private/wrapgtk.h"

// ============================================================================
// implementation
// ============================================================================

//-----------------------------------------------------------------------------
// "color-set"
//-----------------------------------------------------------------------------

extern "C" {
static void gtk_clrbutton_setcolor_callback(GtkColorButton *widget,
                                            wxColourButton *p)
{
    // update the m_colour member of the wxColourButton
    wxASSERT(p);
#ifdef __WXGTK4__
    GdkRGBA gdkColor;
    gtk_color_chooser_get_rgba(GTK_COLOR_CHOOSER(widget), &gdkColor);
#elif defined(__WXGTK3__)
    wxGCC_WARNING_SUPPRESS(deprecated-declarations)
    GdkRGBA gdkColor;
    gtk_color_button_get_rgba(widget, &gdkColor);
    wxGCC_WARNING_RESTORE()
#else
    GdkColor gdkColor;
    gtk_color_button_get_color(widget, &gdkColor);
#endif
    p->GTKSetColour(gdkColor);

    // Fire the corresponding event: note that we want it to appear as
    // originating from our parent, which is the user-visible window, and not
    // this button itself, which is just an implementation detail.
    wxWindow* const parent = p->GetParent();
    wxColourPickerEvent event(parent, parent->GetId(), p->GetColour());
    p->HandleWindowEvent(event);
}
}

//-----------------------------------------------------------------------------
// wxColourButton
//-----------------------------------------------------------------------------

wxIMPLEMENT_DYNAMIC_CLASS(wxColourButton, wxButton);

bool wxColourButton::Create( wxWindow *parent, wxWindowID id,
                        const wxColour &col,
                        const wxPoint &pos, const wxSize &size,
                        long style, const wxValidator& validator,
                        const wxString &name )
{
    if (!PreCreation( parent, pos, size ) ||
        !wxControl::CreateBase(parent, id, pos, size, style, validator, name))
    {
        wxFAIL_MSG( wxT("wxColourButton creation failed") );
        return false;
    }

    m_colour = col;
#ifdef __WXGTK3__
    m_widget = gtk_color_button_new_with_rgba(m_colour);
#else
    m_widget = gtk_color_button_new_with_color( m_colour.GetColor() );
#endif
    g_object_ref(m_widget);

    // Display opacity slider
    g_object_set(G_OBJECT(m_widget), "use-alpha",
                 static_cast<bool>(style & wxCLRP_SHOW_ALPHA), NULL);
    // GtkColourButton signals
    g_signal_connect(m_widget, "color-set",
                    G_CALLBACK(gtk_clrbutton_setcolor_callback), this);


    m_parent->DoAddChild( this );

    PostCreation(size);
    SetInitialSize(size);

    return true;
}

wxColourButton::~wxColourButton()
{
}

void wxColourButton::UpdateColour()
{
#ifdef __WXGTK4__
    gtk_color_chooser_set_rgba(GTK_COLOR_CHOOSER(m_widget), m_colour);
#elif defined(__WXGTK3__)
    wxGCC_WARNING_SUPPRESS(deprecated-declarations)
    gtk_color_button_set_rgba(GTK_COLOR_BUTTON(m_widget), m_colour);
    wxGCC_WARNING_RESTORE()
#else
    gtk_color_button_set_color(GTK_COLOR_BUTTON(m_widget), m_colour.GetColor());
#endif
}

#endif // wxUSE_COLOURPICKERCTRL
