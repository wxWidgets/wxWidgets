///////////////////////////////////////////////////////////////////////////////
// Name:        src/gauge/gaugecmn.cpp
// Purpose:     wxGauge for wxUniversal
// Author:      Vadim Zeitlin
// Modified by:
// Created:     20.02.01
// RCS-ID:      $Id$
// Copyright:   (c) 2001 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// License:     wxWindows license
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "gaugeuniv.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
#endif //WX_PRECOMP

#include "wx/gauge.h"

#if wxUSE_GAUGE

#include "wx/univ/renderer.h"

IMPLEMENT_DYNAMIC_CLASS(wxGauge, wxControl)

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxGauge creation
// ----------------------------------------------------------------------------

void wxGauge::Init()
{
    m_gaugePos =
    m_rangeMax = 0;
}

bool wxGauge::Create(wxWindow *parent,
                     wxWindowID id,
                     int range,
                     const wxPoint& pos,
                     const wxSize& size,
                     long style,
                     const wxValidator& validator,
                     const wxString& name)
{
    if ( !wxGaugeBase::Create(parent, id, range, pos, size, style,
                              validator, name) )
    {
        return FALSE;
    }

    return TRUE;
}

// ----------------------------------------------------------------------------
// wxGauge range/position
// ----------------------------------------------------------------------------

void wxGauge::SetRange(int range)
{
    wxGaugeBase::SetRange(range);

    Refresh();
}

void wxGauge::SetValue(int pos)
{
    wxGaugeBase::SetValue(pos);

    Refresh();
}

// ----------------------------------------------------------------------------
// wxGauge drawing
// ----------------------------------------------------------------------------

wxBorder wxGauge::GetDefaultBorder() const
{
    return wxBORDER_STATIC;
}

void wxGauge::DoDraw(wxControlRenderer *renderer)
{
    int pos = GetValue();
    if ( IsSmooth() )
    {
        // make it a multiple of the step
        int step = GetStep();
        if ( step )
        {
            pos /= step;
            pos *= step;
        }
    }

    renderer->DrawProgressBar(this);
}

#endif // wxUSE_GAUGE
