/////////////////////////////////////////////////////////////////////////////
// Name:        src/gtk/gauge.cpp
// Purpose:
// Author:      Robert Roebling
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_GAUGE

#include "wx/gauge.h"

#include "wx/gtk/private/wrapgtk.h"

//-----------------------------------------------------------------------------
// wxGauge
//-----------------------------------------------------------------------------

bool wxGauge::Create( wxWindow *parent,
                      wxWindowID id,
                      int range,
                      const wxPoint& pos,
                      const wxSize& size,
                      long style,
                      const wxValidator& validator,
                      const wxString& name )
{
    if (!PreCreation( parent, pos, size ) ||
        !CreateBase( parent, id, pos, size, style, validator, name ))
    {
        wxFAIL_MSG( wxT("wxGauge creation failed") );
        return false;
    }

    m_rangeMax = range;

    m_widget = gtk_progress_bar_new();
    g_object_ref(m_widget);
    const bool isVertical = (style & wxGA_VERTICAL) != 0;
    if (isVertical)
    {
#ifdef __WXGTK3__
        gtk_orientable_set_orientation(GTK_ORIENTABLE(m_widget), GTK_ORIENTATION_VERTICAL);
        gtk_progress_bar_set_inverted(GTK_PROGRESS_BAR(m_widget), true);
#else
        gtk_progress_bar_set_orientation( GTK_PROGRESS_BAR(m_widget),
                                          GTK_PROGRESS_BOTTOM_TO_TOP );
#endif
    }

    // when using the gauge in indeterminate mode, we need this:
    gtk_progress_bar_set_pulse_step(GTK_PROGRESS_BAR (m_widget), 0.05);

    m_parent->DoAddChild( this );

    PostCreation(size);
#ifdef __WXGTK3__
    int wh = isVertical ? size.x : size.y;
    if (wh > 0 && gtk_check_version(3,20,0) == nullptr)
    {
        GtkCssProvider* provider = gtk_css_provider_new();
        const char* whStr = isVertical ? "width" : "height";
        char buf[40];
        snprintf(buf, sizeof(buf), "*{min-%s:%dpx}", whStr, wh);
        GTKApplyCssStyle(provider, buf);

        int min;
        if (isVertical)
            gtk_widget_get_preferred_width(m_widget, &min, nullptr);
        else
            gtk_widget_get_preferred_height(m_widget, &min, nullptr);

        // Adjust the min{width,height} to get the right overall size
        wh -= min - wh;
        snprintf(buf, sizeof(buf), "*{min-%s:%dpx}", whStr, wxMax(wh, 1));
        GTKApplyCssStyle(provider, buf);
        g_object_unref(provider);
    }
#endif
    SetInitialSize(size);

    return true;
}

void wxGauge::DoSetGauge()
{
    wxASSERT_MSG( 0 <= m_gaugePos && m_gaugePos <= m_rangeMax,
                  wxT("invalid gauge position in DoSetGauge()") );

    gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (m_widget),
                                   m_rangeMax ? ((double)m_gaugePos)/m_rangeMax : 0.0);
}

void wxGauge::SetRange( int range )
{
    m_rangeMax = range;
    if (m_gaugePos > m_rangeMax)
        m_gaugePos = m_rangeMax;

    DoSetGauge();
}

void wxGauge::SetValue( int pos )
{
    wxCHECK_RET( pos <= m_rangeMax, wxT("invalid value in wxGauge::SetValue()") );

    m_gaugePos = pos;

    DoSetGauge();
}

int wxGauge::GetRange() const
{
    return m_rangeMax;
}

int wxGauge::GetValue() const
{
    return m_gaugePos;
}

void wxGauge::Pulse()
{
    gtk_progress_bar_pulse(GTK_PROGRESS_BAR (m_widget));
}

wxVisualAttributes wxGauge::GetDefaultAttributes() const
{
    // Visible gauge colours use a different colour state
    return GetDefaultAttributesFromGTKWidget(m_widget,
                                             UseGTKStyleBase(),
                                             GTK_STATE_ACTIVE);

}

// static
wxVisualAttributes
wxGauge::GetClassDefaultAttributes(wxWindowVariant WXUNUSED(variant))
{
    return GetDefaultAttributesFromGTKWidget(gtk_progress_bar_new(),
                                             false, GTK_STATE_ACTIVE);
}

#endif // wxUSE_GAUGE
