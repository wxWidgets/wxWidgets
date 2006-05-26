///////////////////////////////////////////////////////////////////////////////
// Name:        src/common/gaugecmn.cpp
// Purpose:     wxGaugeBase: common to all ports methods of wxGauge
// Author:      Vadim Zeitlin
// Modified by:
// Created:     20.02.01
// RCS-ID:      $Id$
// Copyright:   (c) 2001 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// License:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
#endif //WX_PRECOMP

#if wxUSE_GAUGE

#include "wx/gauge.h"

WXDLLIMPEXP_DATA_CORE(const wxChar) wxGaugeNameStr[] = wxT("gauge");

// ============================================================================
// implementation
// ============================================================================

wxGaugeBase::~wxGaugeBase()
{
    // this destructor is required for Darwin
}

// ----------------------------------------------------------------------------
// wxGauge creation
// ----------------------------------------------------------------------------

bool wxGaugeBase::Create(wxWindow *parent,
                         wxWindowID id,
                         int range,
                         const wxPoint& pos,
                         const wxSize& size,
                         long style,
                         const wxValidator& validator,
                         const wxString& name)
{
    if ( !wxControl::Create(parent, id, pos, size, style, validator, name) )
        return false;

    SetName(name);

#if wxUSE_VALIDATORS
    SetValidator(validator);
#endif // wxUSE_VALIDATORS

    SetRange(range);
    SetValue(0);

    return true;
}

// ----------------------------------------------------------------------------
// wxGauge range/position
// ----------------------------------------------------------------------------

void wxGaugeBase::SetRange(int range)
{
    m_rangeMax = range;
}

int wxGaugeBase::GetRange() const
{
    return m_rangeMax;
}

void wxGaugeBase::SetValue(int pos)
{
    m_gaugePos = pos;
}

int wxGaugeBase::GetValue() const
{
    return m_gaugePos;
}

// ----------------------------------------------------------------------------
// wxGauge appearance params
// ----------------------------------------------------------------------------

void wxGaugeBase::SetShadowWidth(int WXUNUSED(w))
{
}

int wxGaugeBase::GetShadowWidth() const
{
    return 0;
}


void wxGaugeBase::SetBezelFace(int WXUNUSED(w))
{
}

int wxGaugeBase::GetBezelFace() const
{
    return 0;
}

#endif // wxUSE_GAUGE
