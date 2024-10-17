///////////////////////////////////////////////////////////////////////////////
// Name:        src/gauge/gauge.cpp
// Purpose:     wxGauge for wxUniversal
// Author:      Vadim Zeitlin
// Created:     20.02.01
// Copyright:   (c) 2001 SciTech Software, Inc. (www.scitechsoft.com)
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#if wxUSE_GAUGE

#include "wx/gauge.h"

#ifndef WX_PRECOMP
#endif //WX_PRECOMP

#include "wx/univ/renderer.h"

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
        return false;
    }

    SetInitialSize(size);

    return true;
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
// wxGauge geometry
// ----------------------------------------------------------------------------

wxSize wxGauge::DoGetBestClientSize() const
{
    wxSize size = GetRenderer()->GetProgressBarStep();

    // these adjustments are really ridiculous - they are just done to find the
    // "correct" result under Windows (FIXME)
    if ( IsVertical() )
    {
        size.x = (3*size.y) / 2 + 2;
        size.y = wxDefaultCoord;
    }
    else
    {
        size.y = (3*size.x) / 2 + 2;
        size.x = wxDefaultCoord;
    }

    return size;
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
    renderer->DrawProgressBar(this);
}

#endif // wxUSE_GAUGE
