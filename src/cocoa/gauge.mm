/////////////////////////////////////////////////////////////////////////////
// Name:        cocoa/gauge.mm
// Purpose:     wxGauge
// Author:      David Elliott
// Modified by:
// Created:     2003/07/15
// RCS-ID:      $Id$
// Copyright:   (c) 2003 David Elliott
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"
#if wxUSE_GAUGE

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/gauge.h"
#endif //WX_PRECOMP

#import <AppKit/NSProgressIndicator.h>

IMPLEMENT_DYNAMIC_CLASS(wxGauge, wxControl)
BEGIN_EVENT_TABLE(wxGauge, wxGaugeBase)
END_EVENT_TABLE()
// WX_IMPLEMENT_COCOA_OWNER(wxGauge,NSProgressIndicator,NSView,NSView)

bool wxGauge::Create(wxWindow *parent, wxWindowID winid, int range,
            const wxPoint& pos, const wxSize& size, long style,
            const wxValidator& validator, const wxString& name)
{
    if(!CreateControl(parent,winid,pos,size,style,validator,name))
        return false;
    SetNSView([[NSProgressIndicator alloc] initWithFrame: MakeDefaultNSRect(size)]);
    [m_cocoaNSView release];
    if(m_parent)
        m_parent->CocoaAddChild(this);
    SetInitialFrameRect(pos,size);

    return true;
}

wxGauge::~wxGauge()
{
}

#endif // wxUSE_GAUGE
