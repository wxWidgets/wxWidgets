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

#include "wx/setup.h"
#if wxUSE_GAUGE

#include "wx/app.h"
#include "wx/gauge.h"

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
    SetNSView([[NSProgressIndicator alloc] initWithFrame: NSMakeRect(10,10,20,20)]);
    [m_cocoaNSView release];
    if(m_parent)
        m_parent->CocoaAddChild(this);
    return true;
}

wxGauge::~wxGauge()
{
}

#endif // wxUSE_GAUGE
