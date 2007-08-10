/////////////////////////////////////////////////////////////////////////////
// Name:        src/cocoa/slider.mm
// Purpose:     wxSlider
// Author:      David Elliott
//              Mark Oxenham
// Modified by:
// Created:     2003/06/19
// RCS-ID:      $Id$
// Copyright:   (c) 2003 David Elliott
//              (c) 2007 Software 2000 Ltd.
// Licence:     wxWidgets licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_SLIDER

#include "wx/slider.h"

#ifndef WX_PRECOMP
    #include "wx/app.h"
#endif //WX_PRECOMP

#import <AppKit/NSSlider.h>

IMPLEMENT_DYNAMIC_CLASS(wxSlider, wxControl)
    BEGIN_EVENT_TABLE(wxSlider, wxSliderBase)
END_EVENT_TABLE()
WX_IMPLEMENT_COCOA_OWNER(wxSlider,NSSlider,NSControl,NSView)


inline void AdjustDimension(
                bool            isTicksStyle,
                int             &pos,
                wxSize          &size,
                int             (wxSize::*GetDimension)() const,
                void            (wxSize::*SetDimension)(int))
{
    const int dimension = (size.*GetDimension)();
    const int minSize = (isTicksStyle) ? 23 : 20;

    if (dimension < minSize)
    {
        (size.*SetDimension)(minSize);
    }

    pos += (dimension - (size.*GetDimension)() + 1) / 2;
}

bool wxSlider::Create(wxWindow *parent, wxWindowID winid,
            int value, int minValue, int maxValue,
            const wxPoint& pos, const wxSize& size, long style,
            const wxValidator& validator, const wxString& name)
{
    wxSize adjustedSize(size);
    wxPoint adjustedPos(pos);
    const bool isTicksStyle = (style & wxSL_TICKS) != 0;

    if ((style & wxSL_HORIZONTAL) && (size.GetHeight() != wxDefaultCoord))
    {
        AdjustDimension(isTicksStyle, adjustedPos.y, adjustedSize, &wxSize::GetHeight, &wxSize::SetHeight);
    }
    else if ((style & wxSL_VERTICAL) && (size.GetWidth() != wxDefaultCoord))
    {
        AdjustDimension(isTicksStyle, adjustedPos.x, adjustedSize, &wxSize::GetWidth, &wxSize::SetWidth);
    }
    
    if(!CreateControl(parent,winid,pos,size,style,validator,name))
        return false;
    SetNSView([[NSSlider alloc] initWithFrame: MakeDefaultNSRect(size)]);
    [m_cocoaNSView release];
    
    if(m_parent)
        m_parent->CocoaAddChild(this);
    SetInitialFrameRect(pos,size);
    
    SetRange(minValue, maxValue);
    SetValue(value);

    return true;
}

wxSlider::~wxSlider()
{
    DisassociateNSSlider(GetNSSlider());
}

void wxSlider::ProcessEventType(wxEventType commandType)
{
    wxScrollEvent event(commandType, GetId(), GetValue(), HasFlag(wxSL_VERTICAL)?wxVERTICAL:wxHORIZONTAL);
    event.SetEventObject(this);
    GetEventHandler()->ProcessEvent(event);
}

void wxSlider::CocoaNotification_startTracking(WX_NSNotification notification)
{
    CocoaNotification_continueTracking(notification);
}

void wxSlider::CocoaNotification_continueTracking(WX_NSNotification notification)
{
    const double realValue = [GetNSSlider() doubleValue];

    if (realValue != [GetNSSlider() intValue])
    {
        SetValue(rint(realValue));
    }

    ProcessEventType(wxEVT_SCROLL_THUMBTRACK);
}

void wxSlider::CocoaNotification_stopTracking(WX_NSNotification notification)
{
    ProcessEventType(wxEVT_SCROLL_THUMBRELEASE);
}

int wxSlider::GetValue() const
{
    return [GetNSSlider() intValue];
}

void wxSlider::SetValue(int value)
{
    [GetNSSlider() setIntValue:value];
}

void wxSlider::SetRange(int minValue, int maxValue)
{
    [GetNSSlider() setMinValue:minValue];
    [GetNSSlider() setMaxValue:maxValue];
}

int wxSlider::GetMin() const
{
    return [GetNSSlider() minValue];
}

int wxSlider::GetMax() const
{
    return [GetNSSlider() maxValue];
}

void wxSlider::SetTickFreq(int n, int pos)
{
    const int numTicks = (n > 0) ? ((GetMax() - GetMin()) / n) + 1 : 0;
    [GetNSSlider() setNumberOfTickMarks:numTicks];
}

int wxSlider::GetTickFreq() const
{
    const int numTicks = [GetNSSlider() numberOfTickMarks];
    return ((numTicks != 0) ? (GetMax() - GetMin()) / (numTicks - 1) : 0);
}

void wxSlider::SetTickPos(int pos)
{
    NSTickMarkPosition thePos = NSTickMarkBelow;
    wxSize size = GetSize();

    if (size.GetWidth() < size.GetHeight()) // NSSlider isVertical method can return -1 if it has not been displayed.
    {
        thePos = (pos != 1) ? NSTickMarkLeft : NSTickMarkRight;
    }
    else
    {
        thePos = (pos != 1) ? NSTickMarkBelow : NSTickMarkAbove;
    }

    [GetNSSlider() setTickMarkPosition:thePos];
}

void wxSlider::SetLineSize(int lineSize)
{
    // to do
}

void wxSlider::SetPageSize(int pageSize)
{
    // to do
}

int wxSlider::GetLineSize() const
{
    return 1;
}

int wxSlider::GetPageSize() const
{
    return 1;
}

int wxSlider::GetThumbLength() const
{
    return 1;
}

void wxSlider::SetThumbLength(int lenPixels)
{
    // to do
}

#endif // wxUSE_SLIDER
