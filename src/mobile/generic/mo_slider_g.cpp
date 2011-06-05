/////////////////////////////////////////////////////////////////////////////
// Name:        src/mobile/generic/mo_slider_g.cpp
// Purpose:     wxMoSlider class
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
#include "wx/dcbuffer.h"

#include "wx/mobile/generic/slider.h"
#include "wx/mobile/generic/utils.h"

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
    style &= ~wxBORDER_MASK;
    style |= wxBORDER_NONE;

    if ( !wxSliderBase::Create(parent, id, pos, size, style, validator, name) )
        return false;

    m_minValue = minValue;
    m_maxValue = maxValue;
    m_value = value;
    
    if (m_value < m_minValue)
        m_value = m_minValue;
    if (m_value > m_maxValue)
        m_value = m_maxValue;

    SetBackgroundStyle(wxBG_STYLE_CUSTOM);
    SetBackgroundColour(wxMoSystemSettings::GetColour(wxMO_COLOUR_SLIDER_BG));
    SetForegroundColour(*wxWHITE);

    SetInitialSize(size);

    return true;
}

wxMoSlider::~wxMoSlider()
{
}

void wxMoSlider::Init()
{
    m_dragStatus = DragNone;
}

wxSize wxMoSlider::DoGetBestSize() const
{
    int width = 100;
    int height = 24;
    
    wxSize best(width, height);
    CacheBestSize(best);
    return best;
}

bool wxMoSlider::SetBackgroundColour(const wxColour &colour)
{
    if ( !wxControl::SetBackgroundColour(colour) )
    {
        // nothing to do
        return false;
    }

    Refresh();

    return true;
}

bool wxMoSlider::SetForegroundColour(const wxColour &colour)
{
    if ( !wxControl::SetForegroundColour(colour) )
    {
        // nothing to do
        return false;
    }

    Refresh();

    return true;
}

bool wxMoSlider::Enable(bool enable)
{
    wxControl::Enable(enable);
    Refresh();
    return true;
}

bool wxMoSlider::SetFont(const wxFont& font)
{
    wxControl::SetFont(font);

    Refresh();

    return true;
}

// get/set the current slider value (should be in range)
int wxMoSlider::GetValue() const
{
    return m_value;
}

void wxMoSlider::SetValue(int value)
{
    m_value = value;
    Refresh();
}

// retrieve/change the range
void wxMoSlider::SetRange(int minValue, int maxValue)
{
    m_minValue = minValue;
    m_maxValue = maxValue;

    if (m_value < m_minValue)
        m_value = m_minValue;
    if (m_value > m_maxValue)
        m_value = m_maxValue;

    Refresh();
}

int wxMoSlider::GetMin() const
{
    return m_minValue;
}

int wxMoSlider::GetMax() const
{
    return m_maxValue;
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
    m_minValueBitmap = bitmap;
}

// Set the maximum value bitmap (drawn on the right side of the slider)
void wxMoSlider::SetMaxValueBitmap(const wxBitmap& bitmap)
{
    m_maxValueBitmap = bitmap;
}
    
void wxMoSlider::OnPaint(wxPaintEvent& WXUNUSED(event))
{
    wxColour fillColour(wxMoSystemSettings::GetColour(wxMO_COLOUR_SLIDER_FILL));
    wxPen outlinePen(wxMoSystemSettings::GetColour(wxMO_COLOUR_NORMAL_BUTTON_BG));

#ifdef __WXMSW__
    wxBufferedPaintDC dc(this);
#else
    wxPaintDC dc(this);
#endif

    dc.SetBackground(wxBrush(GetParent()->GetBackgroundColour()));
    dc.Clear();

    int trackHeight = 8;
    int thumbDiameter = 14;

    // Must be enough room for thumb on each side
    int itemSpacing = thumbDiameter/2 + 2;
    //int circleRadius = trackHeight/2;

    wxSize windowSize = GetClientSize();

    wxRect minImageRect(0, 0, 0, 0);
    wxRect maxImageRect(0, 0, 0, 0);
    if (m_minValueBitmap.IsOk())
        minImageRect = wxRect(0,
                             (windowSize.y - m_minValueBitmap.GetHeight())/2,
                             m_minValueBitmap.GetWidth(), m_minValueBitmap.GetHeight());
    if (m_maxValueBitmap.IsOk())
        maxImageRect = wxRect(windowSize.x - m_maxValueBitmap.GetWidth(),
                             (windowSize.y - m_maxValueBitmap.GetHeight())/2,
                              m_maxValueBitmap.GetWidth(), m_maxValueBitmap.GetHeight());
    wxRect trackRect;
    trackRect.y = (windowSize.y - trackHeight)/2;
    if (m_minValueBitmap.IsOk())
        trackRect.x = minImageRect.x + minImageRect.width;
    else
        trackRect.x = 0;
    trackRect.x += itemSpacing;

    trackRect.height = trackHeight;
    trackRect.width = windowSize.x - trackRect.x - itemSpacing;

    if (m_maxValueBitmap.IsOk())
        trackRect.width -= m_maxValueBitmap.GetWidth();

    // Area without ellipses at the ends
    // Forget rounded ends for now, trick to get right
#if 1
    wxRect actualTrackArea(trackRect.x, trackRect.y,
        trackRect.width, trackRect.height);
#else
    wxRect actualTrackArea(trackRect.x + circleRadius, trackRect.y,
        trackRect.width - 2*circleRadius, trackRect.height);
#endif

    dc.SetPen(outlinePen);
    dc.SetBrush(wxBrush(GetBackgroundColour()));
    dc.DrawRoundedRectangle(actualTrackArea, 4);

    int filledWidth = (double(trackRect.width) * (double(GetValue())/double(GetMax()) - double(GetMin())));
    if (filledWidth > 0)
    {
        wxRect filledRect(actualTrackArea.x, actualTrackArea.y, filledWidth, actualTrackArea.height);
        dc.SetPen(wxPen(fillColour));
        dc.SetBrush(wxBrush(fillColour));
        dc.DrawRoundedRectangle(filledRect, 4);
    }

    if (m_minValueBitmap.IsOk())
        dc.DrawBitmap(m_minValueBitmap, minImageRect.x, minImageRect.y, true);
    if (m_maxValueBitmap.IsOk())
        dc.DrawBitmap(m_maxValueBitmap, maxImageRect.x, maxImageRect.y, true);

    dc.SetPen(outlinePen);
    dc.SetBrush(wxBrush(wxMoSystemSettings::GetColour(wxMO_COLOUR_SLIDER_THUMB)));
    dc.DrawCircle(actualTrackArea.x+filledWidth, windowSize.y/2, thumbDiameter/2);
}

void wxMoSlider::OnEraseBackground(wxEraseEvent& WXUNUSED(event))
{
    // Do nothing
}

void wxMoSlider::OnMouseEvent(wxMouseEvent& event)
{
    if (!IsEnabled())
        return;

    wxRect winRect(GetRect());

    wxRect rect(wxPoint(0, 0), GetSize());

    wxRect trackArea, thumbRect;
    GetTrackDimensions(trackArea, thumbRect);

    if (event.LeftDown())
    {
        if (thumbRect.Contains(event.GetPosition()))
        {
            m_dragStatus = DragStarting;
            m_startPosition = event.GetPosition();
            CaptureMouse();
        }
    }
    else if (event.Dragging())
    {
        if (m_dragStatus == DragStarting)
        {
            if (abs(event.GetPosition().x - m_startPosition.x) > 2 || 
                abs(event.GetPosition().y - m_startPosition.y) > 2)
            {
                m_dragStatus = DragDragging;
            }
        }
        if (m_dragStatus == DragDragging)
        {
            int offsetX = wxMax(0, event.GetPosition().x - trackArea.x);
            offsetX = wxMin(offsetX, trackArea.width);
            int newValue = double(m_maxValue-m_minValue) * double(offsetX)/double(trackArea.width);
            SetValue(newValue);

            wxScrollEvent scrollEvent(wxEVT_SCROLL_THUMBTRACK, GetId());
            scrollEvent.SetPosition(newValue);
            scrollEvent.SetEventObject( this );
            GetEventHandler()->ProcessEvent(scrollEvent);
            
            wxCommandEvent cevent( wxEVT_COMMAND_SLIDER_UPDATED, GetId() );
            cevent.SetInt(newValue);
            cevent.SetEventObject( this );
            GetEventHandler()->ProcessEvent(cevent);            
        }
    }
    else if (event.LeftUp())
    {
        if (GetCapture() == this)
            ReleaseMouse();
        m_dragStatus = DragNone;
    }
#if 0
    else if (event.Leaving())
    {
        if (GetCapture() == this)
            ReleaseMouse();
        m_dragStatus = DragNone;
    }
#endif

#if 0
    if (m_pressDetector.ProcessMouseEvent(this, rect, event))
    {
        SetValue(!GetValue());
        SendClickEvent();
        Refresh();
    }
    else if ((event.LeftUp() || event.Leaving()) && (wxWindow::GetCapture() == this))
    {
        m_pressDetector.SetButtonState(0);
        ReleaseMouse();
    }
#endif
}

// Gets the track area rectangle
bool wxMoSlider::GetTrackDimensions(wxRect& retTrackRect, wxRect& thumbRect) const
{
    int trackHeight = 8;
    int thumbDiameter = 14;

    // Must be enough room for thumb on each side
    int itemSpacing = thumbDiameter/2 + 2;

    wxSize windowSize = GetClientSize();

    wxRect minImageRect(0, 0, 0, 0);
    wxRect maxImageRect(0, 0, 0, 0);
    if (m_minValueBitmap.IsOk())
        minImageRect = wxRect(0,
                             (windowSize.y - m_minValueBitmap.GetHeight())/2,
                             m_minValueBitmap.GetWidth(), m_minValueBitmap.GetHeight());
    if (m_maxValueBitmap.IsOk())
        maxImageRect = wxRect(windowSize.x - m_maxValueBitmap.GetWidth(),
                             (windowSize.y - m_maxValueBitmap.GetHeight())/2,
                              m_maxValueBitmap.GetWidth(), m_maxValueBitmap.GetHeight());
    wxRect trackRect;
    trackRect.y = (windowSize.y - trackHeight)/2;
    if (m_minValueBitmap.IsOk())
        trackRect.x = minImageRect.x + minImageRect.width;
    else
        trackRect.x = 0;
    trackRect.x += itemSpacing;

    trackRect.height = trackHeight;
    trackRect.width = windowSize.x - trackRect.x - itemSpacing;

    if (m_maxValueBitmap.IsOk())
        trackRect.width -= m_maxValueBitmap.GetWidth();

    wxRect actualTrackArea(trackRect.x, trackRect.y,
        trackRect.width, trackRect.height);

    retTrackRect = actualTrackArea;

    int filledWidth = (double(trackRect.width) * (double(GetValue())/double(GetMax()) - double(GetMin())));

    thumbRect.x = actualTrackArea.x + filledWidth - thumbDiameter/2;
    thumbRect.y = windowSize.y/2 - thumbDiameter/2;
    thumbRect.width = thumbDiameter;
    thumbRect.height = thumbDiameter;

    return true;

}

