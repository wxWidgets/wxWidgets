/////////////////////////////////////////////////////////////////////////////
// Name:        cocoa/scrolbar.mm
// Purpose:     wxScrollBar
// Author:      David Elliott
// Modified by:
// Created:     2004/04/25
// RCS-ID:      $Id$
// Copyright:   (c) 2004 David Elliott
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"
#if wxUSE_SCROLLBAR

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/scrolbar.h"
#endif //WX_PRECOMP

#import <AppKit/NSScroller.h>

IMPLEMENT_DYNAMIC_CLASS(wxScrollBar, wxControl)
BEGIN_EVENT_TABLE(wxScrollBar, wxScrollBarBase)
END_EVENT_TABLE()
WX_IMPLEMENT_COCOA_OWNER(wxScrollBar,NSScroller,NSControl,NSView)

bool wxScrollBar::Create(wxWindow *parent, wxWindowID winid,
            const wxPoint& pos, const wxSize& size, long style,
            const wxValidator& validator, const wxString& name)
{
    if(!CreateControl(parent,winid,pos,size,style,validator,name))
        return false;
    SetNSScroller([[NSScroller alloc] initWithFrame: MakeDefaultNSRect(size)]);
    [m_cocoaNSView release];
    if(m_parent)
        m_parent->CocoaAddChild(this);
    SetInitialFrameRect(pos,size);

    return true;
}

wxScrollBar::~wxScrollBar()
{
    DisassociateNSScroller(GetNSScroller());
}

/* A word about NSScroller vs. wxScrollbar:

NSScroller uses two float values to represent the state of the scroller.
The floatValue indicates where the knob is positioned on a scale from
0.0 to 1.0.  A value of 0.0 indicates the scroller is at the top or left,
a value of 1.0 indicates the scroller is at the bottom or right.  A value
of 0.5 indicates the scroller is dead center.

wxScrollBar uses three values.  The position indicates the number of
scroll units where 0 is at the top or left.  The range indicates how
many scroll units there are in the entire bar and the thumb size indicates
how many scroll units the thumb takes.  The scrollbar is at the bottom
or right when position == range - thumbSize.

It may be easier to consider wx position to be the top or left of the thumb.
In Cocoa, floatValue can be considered as if it were the center of the
thumb and the range is ALWAYS (no matter what the knobProportion is) the
distance between the center point of the knob from one extreme to the other.
*/

int wxScrollBar::GetThumbPosition() const
{
    return (int)((m_range-m_thumbSize)*[GetNSScroller() floatValue]);
}

void wxScrollBar::SetThumbPosition(int position)
{
    [GetNSScroller() setFloatValue:((float)position)/(m_range-m_thumbSize)];
}

void wxScrollBar::SetScrollbar(int position, int thumbSize,
            int range, int pageSize, bool refresh)
{
    m_range = range;
    m_thumbSize = thumbSize;
    m_pageSize = pageSize;
    [GetNSScroller() setFloatValue:((float)position)/(m_range-m_thumbSize)
            knobProportion:((float)m_thumbSize)/m_range];
}

void wxScrollBar::Cocoa_wxNSScrollerAction()
{
    NSScrollerPart hitPart = [GetNSScroller() hitPart];
    wxEventType command;
    // Note: the comments about the part that is hit are for OS X, the
    // constants are sort of a leftover from NeXT.  It makes more sense if
    // you remember that in NeXT clicking the knob slot used to do what
    // option-clicking does now.
    switch(hitPart)
    {
    // User dragged knob
    case NSScrollerKnob:
        command = wxEVT_SCROLL_THUMBTRACK;
        break;
    // User option-clicked slot
    case NSScrollerKnobSlot:
        command = wxEVT_SCROLL_THUMBTRACK;
        break;
    // User clicked Up/Left button
    case NSScrollerDecrementLine:
        command = wxEVT_SCROLL_LINEUP;
        break;
    // User option-clicked Up/left or clicked in slot
    case NSScrollerDecrementPage:
        command = wxEVT_SCROLL_PAGEUP;
        break;
    // User clicked Down/Right button
    case NSScrollerIncrementLine:
        command = wxEVT_SCROLL_LINEDOWN;
        break;
    // User option-clicked Down/Right or clicked in slot
    case NSScrollerIncrementPage:
        command = wxEVT_SCROLL_PAGEDOWN;
        break;
    // No-op
    case NSScrollerNoPart:
    default:
        return;
    }
    // TODO: When scrolling by pages, readjust the floatValue using the
    // pageSize (which may be different from thumbSize).
    wxScrollEvent event(command, GetId(), GetThumbPosition(),
        HasFlag(wxSB_VERTICAL)?wxVERTICAL:wxHORIZONTAL);
    event.SetEventObject(this);
    GetEventHandler()->ProcessEvent(event);
}

#endif // wxUSE_SCROLLBAR
