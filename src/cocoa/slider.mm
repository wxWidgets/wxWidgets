/////////////////////////////////////////////////////////////////////////////
// Name:        cocoa/slider.mm
// Purpose:     wxSlider
// Author:      David Elliott
// Modified by:
// Created:     2003/06/19
// RCS-ID:      $Id$
// Copyright:   (c) 2003 David Elliott
// Licence:   	wxWidgets licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"
#if wxUSE_SLIDER

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/slider.h"
#endif //WX_PRECOMP

#import <AppKit/NSSlider.h>

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
    SetNSView([[NSSlider alloc] initWithFrame: MakeDefaultNSRect(size)]);
    [m_cocoaNSView release];
    if(m_parent)
        m_parent->CocoaAddChild(this);
    SetInitialFrameRect(pos,size);

    return true;
}

wxSlider::~wxSlider()
{
}

#endif // wxUSE_SLIDER
