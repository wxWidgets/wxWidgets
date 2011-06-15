/////////////////////////////////////////////////////////////////////////////
// Name:        src/mobile/generic/mo_slider_n.mm
// Purpose:     wxMoSlider class
// Author:      Julian Smart, Linas Valiukas
// Modified by:
// Created:     2011-06-14
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart, Linas Valiukas
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/dcclient.h"
#include "wx/dcbuffer.h"

#include "wx/mobile/native/slider.h"
#include "wx/mobile/native/utils.h"

IMPLEMENT_DYNAMIC_CLASS(wxMoSlider, wxControl)

BEGIN_EVENT_TABLE(wxMoSlider, wxControl)
    EVT_PAINT(wxMoSlider::OnPaint)
    EVT_MOUSE_EVENTS(wxMoSlider::OnMouseEvent)
    EVT_ERASE_BACKGROUND(wxMoSlider::OnEraseBackground)
END_EVENT_TABLE()

bool wxMoSlider::Create(wxWindow *parent,
                      wxWindowID id,
                      int value, int minValue, int maxValue,
                      const wxPoint& pos,
                      const wxSize& size,
                      long style,
                      const wxValidator& validator,
                      const wxString& name)
{
    // FIXME stub

    return true;
}

wxMoSlider::~wxMoSlider()
{
}

void wxMoSlider::Init()
{
    // FIXME stub
}

wxSize wxMoSlider::DoGetBestSize() const
{
    // FIXME stub

    wxSize empty(1, 1);
    return empty;
}

bool wxMoSlider::SetBackgroundColour(const wxColour &colour)
{
    // FIXME stub

    return true;
}

bool wxMoSlider::SetForegroundColour(const wxColour &colour)
{
    // FIXME stub

    return true;
}

bool wxMoSlider::Enable(bool enable)
{
    // FIXME stub

    return true;
}

bool wxMoSlider::SetFont(const wxFont& font)
{
    // FIXME stub

    return true;
}

// get/set the current slider value (should be in range)
int wxMoSlider::GetValue() const
{
    // FIXME stub

    return 0;
}

void wxMoSlider::SetValue(int value)
{
    // FIXME stub
}

// retrieve/change the range
void wxMoSlider::SetRange(int minValue, int maxValue)
{
    // FIXME stub
}

int wxMoSlider::GetMin() const
{
    // FIXME stub

    return 0;
}

int wxMoSlider::GetMax() const
{
    // FIXME stub

    return 0;
}

// the line/page size is the increment by which the slider moves when
// cursor arrow key/page up or down are pressed (clicking the mouse is like
// pressing PageUp/Down) and are by default set to 1 and 1/10 of the range
void wxMoSlider::SetLineSize(int WXUNUSED(lineSize))
{
}

void wxMoSlider::SetPageSize(int WXUNUSED(pageSize))
{
}

int wxMoSlider::GetLineSize() const
{
    return 1;
}

int wxMoSlider::GetPageSize() const
{
    return 1;
}

// these methods get/set the length of the slider pointer in pixels
void wxMoSlider::SetThumbLength(int WXUNUSED(lenPixels))
{
}

int wxMoSlider::GetThumbLength() const
{
    return 1;
}

// Set the minimum value bitmap (drawn on the left side of the slider)
void wxMoSlider::SetMinValueBitmap(const wxBitmap& bitmap)
{
    // FIXME stub
}

// Set the maximum value bitmap (drawn on the right side of the slider)
void wxMoSlider::SetMaxValueBitmap(const wxBitmap& bitmap)
{
    // FIXME stub
}
    
void wxMoSlider::OnPaint(wxPaintEvent& WXUNUSED(event))
{
    // FIXME stub
}

void wxMoSlider::OnEraseBackground(wxEraseEvent& WXUNUSED(event))
{
    // Do nothing
}

void wxMoSlider::OnMouseEvent(wxMouseEvent& event)
{
    // FIXME stub
}

// Gets the track area rectangle
bool wxMoSlider::GetTrackDimensions(wxRect& retTrackRect, wxRect& thumbRect) const
{
    // FIXME stub

    return true;
}
