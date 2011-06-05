/////////////////////////////////////////////////////////////////////////////
// Name:        src/mobile/generic/mp_gauge_g.cpp
// Purpose:     wxMoGauge class
// Author:      Julian Smart
// Modified by:
// Created:     16/05/09
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/dcclient.h"
#include "wx/dcscreen.h"
#include "wx/settings.h"
#include "wx/dcbuffer.h"

#include "wx/mobile/generic/utils.h"
#include "wx/mobile/generic/gauge.h"

IMPLEMENT_DYNAMIC_CLASS(wxMoGauge, wxControl)

BEGIN_EVENT_TABLE(wxMoGauge, wxControl)
    EVT_PAINT(wxMoGauge::OnPaint)
    EVT_ERASE_BACKGROUND(wxMoGauge::OnEraseBackground)    
    EVT_SIZE(wxMoGauge::OnSize)
END_EVENT_TABLE()

bool wxMoGauge::Create(wxWindow *parent,
                       wxWindowID id,
                       int range,
                       const wxPoint& pos,
                       const wxSize& size,
                       long style,
                       const wxValidator& validator,
                       const wxString& name)
{
    style &= ~wxBORDER_MASK;
    style |= wxBORDER_NONE;

    if ( !wxControl::Create(parent, id, pos, size, style, validator, name) )
        return false;

    SetForegroundColour(wxMoSystemSettings::GetColour(wxMO_COLOUR_GAUGE_FILL));
    SetBackgroundStyle(wxBG_STYLE_CUSTOM);
    SetRange(range);

    SetInitialSize(size);

    return true;
}

void wxMoGauge::Init()
{
}

wxSize wxMoGauge::DoGetBestSize() const
{
    wxSize sz(100, 20);
    CacheBestSize(sz);
    return sz;
}

// ----------------------------------------------------------------------------
// wxMoGauge setters
// ----------------------------------------------------------------------------

void wxMoGauge::SetRange(int r)
{
    // switch to determinate mode if required
    SetDeterminateMode();

    m_rangeMax = r;

    Refresh();
}

void wxMoGauge::SetValue(int pos)
{
    // switch to determinate mode if required
    SetDeterminateMode();

    m_gaugePos = pos;

    Refresh();
}

bool wxMoGauge::SetForegroundColour(const wxColour& col)
{
    if ( !wxControl::SetForegroundColour(col) )
        return false;

    Refresh();

    return true;
}

bool wxMoGauge::SetBackgroundColour(const wxColour& col)
{
    if ( !wxControl::SetBackgroundColour(col) )
        return false;

    Refresh();

    return true;
}

void wxMoGauge::SetIndeterminateMode()
{
    // TODO
}

void wxMoGauge::SetDeterminateMode()
{
    // TODO
}

void wxMoGauge::Pulse()
{
    // TODO
    // emulate indeterminate mode
    wxGaugeBase::Pulse();
}

void wxMoGauge::OnPaint(wxPaintEvent& WXUNUSED(event))
{
#ifdef __WXMSW__
    wxBufferedPaintDC dc(this);
#else
    wxPaintDC dc(this);
#endif

    wxSize clientSize(GetClientSize());

    wxColour bgColour = GetParent()->GetBackgroundColour();

    dc.SetBackground(wxBrush(bgColour));
    dc.Clear();

    wxPen outlinePen(wxMoSystemSettings::GetColour(wxMO_COLOUR_NORMAL_BUTTON_BG));
    wxBrush filledBrush(GetForegroundColour());
    wxBrush unfilledBrush(wxMoSystemSettings::GetColour(wxMO_COLOUR_GAUGE_BG));

    dc.SetPen(outlinePen);

    if (GetValue() > 0)
        dc.SetBrush(filledBrush);
    else
        dc.SetBrush(unfilledBrush);

    int circleRadius = (clientSize.y/2)-2;
    int xStart = circleRadius+1;
    int yStart = clientSize.y/2;
    dc.DrawArc(xStart, yStart-circleRadius, xStart, yStart+circleRadius, xStart, yStart);
    
    if (GetValue() >= GetRange()-1)
        dc.SetBrush(filledBrush);
    else
        dc.SetBrush(unfilledBrush);

    int xEnd = clientSize.x - (circleRadius+1);
    int yEnd = clientSize.y/2;
    dc.DrawArc(xEnd, yEnd+circleRadius, xEnd, yEnd-circleRadius, xEnd, yEnd);

    dc.SetPen(*wxTRANSPARENT_PEN);
    // Overwrite vertical arc edge at left and right
    dc.DrawRectangle(xStart, yStart-circleRadius, xEnd-xStart+1, 2*circleRadius);

    int barWidth = int((clientSize.x - 2*circleRadius) * (double(GetValue())/double(GetRange())));
    if (barWidth > 0)
    {
        dc.SetBrush(filledBrush);
        dc.DrawRectangle(xStart, yStart-circleRadius, barWidth, 2*circleRadius);
    }

    dc.SetPen(outlinePen);
    dc.DrawLine(xStart, yStart-circleRadius, xEnd+1, yStart-circleRadius);
    dc.DrawLine(xStart, yStart+circleRadius, xEnd+1, yStart+circleRadius);
}

void wxMoGauge::OnEraseBackground(wxEraseEvent& WXUNUSED(event))
{
}

void wxMoGauge::OnSize(wxSizeEvent& WXUNUSED(event))
{
    Refresh();
}
