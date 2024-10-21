/////////////////////////////////////////////////////////////////////////////
// Name:        src/gtk/spinbutt.cpp
// Purpose:     wxSpinButton
// Author:      Robert
// Copyright:   (c) Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_SPINBTN

#include "wx/spinbutt.h"

#include "wx/gtk/private/wrapgtk.h"

//-----------------------------------------------------------------------------
// data
//-----------------------------------------------------------------------------

extern bool   g_blockEventsOnDrag;

//-----------------------------------------------------------------------------
// "value_changed"
//-----------------------------------------------------------------------------

extern "C" {
static void
gtk_value_changed(GtkSpinButton* spinbutton, wxSpinButton* win)
{
    const int pos = gtk_spin_button_get_value_as_int(spinbutton);
    const int oldPos = win->m_pos;
    if (g_blockEventsOnDrag || pos == oldPos)
    {
        win->m_pos = pos;
        return;
    }

    // Normally we can determine which way we're going by just comparing the
    // old and the new values.
    bool up = pos > oldPos;

    // However we need to account for the possibility of wrapping around.
    if ( win->HasFlag(wxSP_WRAP) )
    {
        // We have no way of distinguishing between wraparound and normal
        // change when the range is just 1, as pressing either arrow results in
        // the same change, so don't even try doing it in this case.
        const int spinMin = win->GetMin();
        const int spinMax = win->GetMax();

        if ( spinMax - spinMin > 1 )
        {
            if ( up )
            {
                if ( oldPos == spinMin && pos == spinMax )
                    up = false;
            }
            else // down
            {
                if ( oldPos == spinMax && pos == spinMin )
                    up = true;
            }
        }
    }

    wxSpinEvent event(up ? wxEVT_SCROLL_LINEUP : wxEVT_SCROLL_LINEDOWN, win->GetId());
    event.SetPosition(pos);
    event.SetEventObject(win);

    if ((win->HandleWindowEvent( event )) &&
        !event.IsAllowed() )
    {
        /* program has vetoed */
        // this will cause another "value_changed" signal,
        // but because pos == oldPos nothing will happen
        gtk_spin_button_set_value(spinbutton, oldPos);
        return;
    }

    win->m_pos = pos;

    /* always send a thumbtrack event */
    wxSpinEvent event2(wxEVT_SCROLL_THUMBTRACK, win->GetId());
    event2.SetPosition(pos);
    event2.SetEventObject(win);
    win->HandleWindowEvent(event2);
}
}

//-----------------------------------------------------------------------------
// wxSpinButton
//-----------------------------------------------------------------------------

wxSpinButton::wxSpinButton()
{
    m_pos = 0;
}

bool wxSpinButton::Create(wxWindow *parent,
                          wxWindowID id,
                          const wxPoint& pos,
                          const wxSize& size,
                          long style,
                          const wxString& name)
{
    if (!PreCreation(parent, pos, size) ||
        !CreateBase(parent, id, pos, size, style, wxDefaultValidator, name))
    {
        wxFAIL_MSG( wxT("wxSpinButton creation failed") );
        return false;
    }

    m_pos = 0;

    m_widget = gtk_spin_button_new_with_range(0, 100, 1);
    g_object_ref(m_widget);

    gtk_entry_set_width_chars(GTK_ENTRY(m_widget), 0);
#if GTK_CHECK_VERSION(3,12,0)
    if (gtk_check_version(3,12,0) == nullptr)
        gtk_entry_set_max_width_chars(GTK_ENTRY(m_widget), 0);
#endif
#ifdef __WXGTK3__
    if (gtk_check_version(3,20,0) == nullptr)
    {
        GTKApplyCssStyle(
            "entry { min-width:0; padding-left:0; padding-right:0 }"
            "button.down { border-style:none }");
    }
#endif
    gtk_spin_button_set_wrap( GTK_SPIN_BUTTON(m_widget),
                              (int)(m_windowStyle & wxSP_WRAP) );

    g_signal_connect_after(
        m_widget, "value_changed", G_CALLBACK(gtk_value_changed), this);

    m_parent->DoAddChild( this );

    PostCreation(size);

    return true;
}

int wxSpinButton::GetMin() const
{
    wxCHECK_MSG( (m_widget != nullptr), 0, wxT("invalid spin button") );

    double min;
    gtk_spin_button_get_range((GtkSpinButton*)m_widget, &min, nullptr);
    return int(min);
}

int wxSpinButton::GetMax() const
{
    wxCHECK_MSG( (m_widget != nullptr), 0, wxT("invalid spin button") );

    double max;
    gtk_spin_button_get_range((GtkSpinButton*)m_widget, nullptr, &max);
    return int(max);
}

int wxSpinButton::GetValue() const
{
    wxCHECK_MSG( (m_widget != nullptr), 0, wxT("invalid spin button") );

    return m_pos;
}

void wxSpinButton::SetValue( int value )
{
    wxCHECK_RET( (m_widget != nullptr), wxT("invalid spin button") );

    GtkDisableEvents();
    gtk_spin_button_set_value((GtkSpinButton*)m_widget, value);
    m_pos = int(gtk_spin_button_get_value((GtkSpinButton*)m_widget));
    GtkEnableEvents();
}

void wxSpinButton::SetRange(int minVal, int maxVal)
{
    wxCHECK_RET( (m_widget != nullptr), wxT("invalid spin button") );

    GtkDisableEvents();
    gtk_spin_button_set_range((GtkSpinButton*)m_widget, minVal, maxVal);
    m_pos = int(gtk_spin_button_get_value((GtkSpinButton*)m_widget));

    // Use smaller page increment in the case of a narrow range for convenience
    // and to limit possible up/down ambiguity in gtk_value_changed() when
    // wrapping is on (The maximal page increment of 10 is consistent with the
    // default page increment set by gtk_spin_button_new_with_range(0, 100, 1)
    // in wxSpinButton::Create().)
    int pageInc;
    if ( maxVal < minVal + 10 )
        pageInc = 1;
    else if ( maxVal < minVal + 20 )
        pageInc = 2;
    else if ( maxVal < minVal + 50 )
        pageInc = 5;
    else
        pageInc = 10;

    GtkAdjustment* adj = gtk_spin_button_get_adjustment((GtkSpinButton*)m_widget);
    gtk_adjustment_set_page_increment(adj, pageInc);

    GtkEnableEvents();
}

void wxSpinButton::DoEnable(bool enable)
{
    if ( !m_widget )
        return;

    base_type::DoEnable(enable);

    // Work around lack of visual update when enabling
    if (enable)
        GTKFixSensitivity(false /* fix even if not under mouse */);
}

void wxSpinButton::GtkDisableEvents() const
{
    g_signal_handlers_block_by_func(m_widget,
        (void*)gtk_value_changed, const_cast<wxSpinButton*>(this));
}

void wxSpinButton::GtkEnableEvents() const
{
    g_signal_handlers_unblock_by_func(m_widget,
        (void*)gtk_value_changed, const_cast<wxSpinButton*>(this));
}

GdkWindow *wxSpinButton::GTKGetWindow(wxArrayGdkWindows& WXUNUSED_IN_GTK2(windows)) const
{
#ifdef __WXGTK3__
    GTKFindWindow(m_widget, windows);
    return nullptr;
#else
    return GTK_SPIN_BUTTON(m_widget)->panel;
#endif
}

wxSize wxSpinButton::DoGetBestSize() const
{
    wxSize best = base_type::DoGetBestSize();
#ifdef __WXGTK3__
    GtkStyleContext* sc = gtk_widget_get_style_context(m_widget);
    GtkBorder pad = { 0, 0, 0, 0 };
    gtk_style_context_get_padding(sc, gtk_style_context_get_state(sc), &pad);
    best.x -= pad.left + pad.right;
#else
    gtk_widget_ensure_style(m_widget);
    int w = PANGO_PIXELS(pango_font_description_get_size(m_widget->style->font_desc));
    w &= ~1;
    if (w < 6)
        w = 6;
    best.x = w + 2 * m_widget->style->xthickness;
#endif
    return best;
}

// static
wxVisualAttributes
wxSpinButton::GetClassDefaultAttributes(wxWindowVariant WXUNUSED(variant))
{
    return GetDefaultAttributesFromGTKWidget(gtk_spin_button_new_with_range(0, 100, 1));
}

#endif // wxUSE_SPINBTN
