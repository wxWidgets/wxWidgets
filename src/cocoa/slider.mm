/////////////////////////////////////////////////////////////////////////////
// Name:        cocoa/slider.mm
// Purpose:     wxSlider
// Author:      David Elliott
// Modified by:
// Created:     2003/06/19
// RCS-ID:      $Id$
// Copyright:   (c) 2003 David Elliott
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/setup.h"
#if wxUSE_SLIDER

#include "wx/app.h"
#include "wx/slider.h"

#import <AppKit/NSView.h>

IMPLEMENT_DYNAMIC_CLASS(wxSlider, wxControl)
BEGIN_EVENT_TABLE(wxSlider, wxSliderBase)
END_EVENT_TABLE()
// WX_IMPLEMENT_COCOA_OWNER(wxSlider,NSSlider,NSControl,NSView)

bool wxSlider::Create(wxWindow *parent, wxWindowID winid,
            int value, int minValue, int maxValue,
            const wxPoint& pos, const wxSize& size, long style,
            const wxValidator& validator, const wxString& name)
{
    if(!CreateControl(parent,winid,pos,size,style,validator,name))
        return false;
    SetNSView([[NSView alloc] initWithFrame: NSMakeRect(10,10,20,20)]);
    [m_cocoaNSView release];
    if(m_parent)
        m_parent->CocoaAddChild(this);
    return true;
}

wxSlider::~wxSlider()
{
}

#endif // wxUSE_SLIDER
