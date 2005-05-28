/////////////////////////////////////////////////////////////////////////////
// Name:        gauge.cpp
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "gauge.h"
#endif

#include "wx/gauge.h"

#if wxUSE_GAUGE

#include <gtk/gtk.h>

//-----------------------------------------------------------------------------
// wxGauge
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxGauge, wxControl)

bool wxGauge::Create( wxWindow *parent,
                      wxWindowID id,
                      int range,
                      const wxPoint& pos,
                      const wxSize& size,
                      long style,
                      const wxValidator& validator,
                      const wxString& name )
{
    m_needParent = TRUE;

    if (!PreCreation( parent, pos, size ) ||
        !CreateBase( parent, id, pos, size, style, validator, name ))
    {
        wxFAIL_MSG( wxT("wxGauge creation failed") );
        return FALSE;
    }

    m_rangeMax = range;

    m_widget = gtk_progress_bar_new();
    if ( style & wxGA_VERTICAL )
    {
        gtk_progress_bar_set_orientation( GTK_PROGRESS_BAR(m_widget),
                                          GTK_PROGRESS_BOTTOM_TO_TOP );
    }

    m_parent->DoAddChild( this );

    PostCreation();

    Show( TRUE );

    return TRUE;
}

void wxGauge::DoSetGauge()
{
    wxASSERT_MSG( 0 <= m_gaugePos && m_gaugePos <= m_rangeMax,
                  _T("invalid gauge position in DoSetGauge()") );

    gtk_progress_bar_update( GTK_PROGRESS_BAR(m_widget),
                             m_rangeMax ? ((float)m_gaugePos)/m_rangeMax : 0.);
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
    wxCHECK_RET( pos <= m_rangeMax, _T("invalid value in wxGauge::SetValue()") );

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

void wxGauge::ApplyWidgetStyle()
{
    SetWidgetStyle();
    gtk_widget_set_style( m_widget, m_widgetStyle );
}

#endif // wxUSE_GAUGE

