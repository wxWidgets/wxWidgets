/////////////////////////////////////////////////////////////////////////////
// Name:        src/gtk/radiobut.cpp
// Purpose:
// Author:      Robert Roebling
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_RADIOBTN

#include "wx/radiobut.h"

#include "wx/gtk/private.h"

//-----------------------------------------------------------------------------
// data
//-----------------------------------------------------------------------------

extern bool           g_blockEventsOnDrag;

//-----------------------------------------------------------------------------
// "clicked"
//-----------------------------------------------------------------------------

extern "C" {
static
void gtk_radiobutton_clicked_callback( GtkToggleButton *button, wxRadioButton *rb )
{
    if (g_blockEventsOnDrag) return;

    if (!gtk_toggle_button_get_active(button)) return;

    wxCommandEvent event( wxEVT_RADIOBUTTON, rb->GetId());
    event.SetInt( rb->GetValue() );
    event.SetEventObject( rb );
    rb->HandleWindowEvent( event );
}
}

//-----------------------------------------------------------------------------
// wxRadioButton
//-----------------------------------------------------------------------------

bool wxRadioButton::Create( wxWindow *parent,
                            wxWindowID id,
                            const wxString& label,
                            const wxPoint& pos,
                            const wxSize& size,
                            long style,
                            const wxValidator& validator,
                            const wxString& name )
{
    if (!PreCreation( parent, pos, size ) ||
        !CreateBase( parent, id, pos, size, style, validator, name ))
    {
        wxFAIL_MSG( wxT("wxRadioButton creation failed") );
        return false;
    }

    // Check if this radio button should be put into an existing group. This
    // shouldn't be done if it's given a style to explicitly start a new group
    // or if it's not meant to be a part of a group at all.
    GSList* radioButtonGroup = nullptr;
    if (!HasFlag(wxRB_GROUP) && !HasFlag(wxRB_SINGLE))
    {
        // search backward for last group start
        wxWindowList::compatibility_iterator node = parent->GetChildren().GetLast();
        for (; node; node = node->GetPrevious())
        {
            wxWindow *child = node->GetData();

            // We stop at the first previous radio button in any case as it
            // wouldn't make sense to put this button in a group with another
            // one if there is a radio button that is not part of the same
            // group between them.
            if (wxIsKindOf(child, wxRadioButton))
            {
                // Any preceding radio button can be used to get its group, not
                // necessarily one with wxRB_GROUP style, but exclude
                // wxRB_SINGLE ones as their group should never be shared.
                if (!child->HasFlag(wxRB_SINGLE))
                {
                    radioButtonGroup = gtk_radio_button_get_group(
                        GTK_RADIO_BUTTON(child->m_widget));
                }

                break;
            }
        }
    }

    // GTK does not allow a radio button to be inactive if it is the only radio
    // button in its group, so we need to work around this by creating a second
    // hidden radio button.
    if (HasFlag(wxRB_SINGLE))
    {
        m_hiddenButton = gtk_radio_button_new( nullptr );
        m_widget = gtk_radio_button_new_with_label_from_widget( GTK_RADIO_BUTTON(m_hiddenButton), label.utf8_str() );
        // Since this is the second button in the group, we need to ensure it
        // is active by default and not the first hidden one.
        gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON(m_widget), TRUE );
    }
    else
    {
        m_widget = gtk_radio_button_new_with_label( radioButtonGroup, label.utf8_str() );
    }

    g_object_ref(m_widget);

    SetLabel(label);

    g_signal_connect_after (m_widget, "clicked",
                            G_CALLBACK (gtk_radiobutton_clicked_callback), this);

    m_parent->DoAddChild( this );

    PostCreation(size);

    return true;
}

void wxRadioButton::SetLabel( const wxString& label )
{
    wxCHECK_RET( m_widget != nullptr, wxT("invalid radiobutton") );

    // save the original label
    wxControlBase::SetLabel(label);

    GTKSetLabelForLabel(GTK_LABEL(gtk_bin_get_child(GTK_BIN(m_widget))), label);
}

void wxRadioButton::SetValue( bool val )
{
    wxCHECK_RET( m_widget != nullptr, wxT("invalid radiobutton") );

    if (val == GetValue())
        return;

    g_signal_handlers_block_by_func(
        m_widget, (void*)gtk_radiobutton_clicked_callback, this);

    if (val)
    {
        gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON(m_widget), TRUE );
    }
    else
    {
        // Normal radio buttons can only be turned off by turning on another
        // button in the same group, but the single ones can be turned off
        // manually, which is implemented by turning a hidden button on, as
        // it's the only way to do it with GTK.
        if (HasFlag(wxRB_SINGLE))
            gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON(m_hiddenButton), TRUE );
    }

    g_signal_handlers_unblock_by_func(
        m_widget, (void*)gtk_radiobutton_clicked_callback, this);
}

bool wxRadioButton::GetValue() const
{
    wxCHECK_MSG( m_widget != nullptr, false, wxT("invalid radiobutton") );

    return gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_widget)) != 0;
}

void wxRadioButton::DoEnable(bool enable)
{
    if ( !m_widget )
        return;

    base_type::DoEnable(enable);

    gtk_widget_set_sensitive(gtk_bin_get_child(GTK_BIN(m_widget)), enable);

    if (enable)
        GTKFixSensitivity();
}

void wxRadioButton::DoApplyWidgetStyle(GtkRcStyle *style)
{
    GTKApplyStyle(m_widget, style);
    GTKApplyStyle(gtk_bin_get_child(GTK_BIN(m_widget)), style);
}

GdkWindow *
wxRadioButton::GTKGetWindow(wxArrayGdkWindows& WXUNUSED(windows)) const
{
    return gtk_button_get_event_window(GTK_BUTTON(m_widget));
}

// static
wxVisualAttributes
wxRadioButton::GetClassDefaultAttributes(wxWindowVariant WXUNUSED(variant))
{
    return GetDefaultAttributesFromGTKWidget(gtk_radio_button_new_with_label(nullptr, ""));
}


#endif
