/////////////////////////////////////////////////////////////////////////////
// Name:        gauge.mm
// Purpose:     wxGauge class
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id: gauge.cpp 54820 2008-07-29 20:04:11Z SC $
// Copyright:   (c) Stefan Csomor
// Licence:       wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_GAUGE

#include "wx/gauge.h"

#include "wx/osx/private.h"

@interface wxNSProgressIndicator : NSProgressIndicator
{
    WXCOCOAIMPL_COMMON_MEMBERS
}

WXCOCOAIMPL_COMMON_INTERFACE

@end

@implementation wxNSProgressIndicator

- (id)initWithFrame:(NSRect)frame
{
    [super initWithFrame:frame];
    impl = NULL;
    return self;
}

WXCOCOAIMPL_COMMON_IMPLEMENTATION

@end

class wxOSXGaugeCocoaImpl : public wxWidgetCocoaImpl
{
public :
    wxOSXGaugeCocoaImpl( wxWindowMac* peer, WXWidget w) : wxWidgetCocoaImpl( peer, w )
    {
    }
    
    void SetMaximum(wxInt32 v)
    {
        SetDeterminateMode();
        wxWidgetCocoaImpl::SetMaximum( v ) ;
    }
    
    void SetValue(wxInt32 v)
    {
        SetDeterminateMode();
        wxWidgetCocoaImpl::SetValue( v ) ;
    }
    
    void PulseGauge()
    {
        if ( ![(wxNSProgressIndicator*)m_osxView isIndeterminate] )
        {
            [(wxNSProgressIndicator*)m_osxView setIndeterminate:YES];
            [(wxNSProgressIndicator*)m_osxView startAnimation:nil];
        }
    }
protected:
    void SetDeterminateMode()
    {
       // switch back to determinate mode if necessary
        if ( [(wxNSProgressIndicator*)m_osxView isIndeterminate]  )
        {
            [(wxNSProgressIndicator*)m_osxView stopAnimation:nil];
            [(wxNSProgressIndicator*)m_osxView setIndeterminate:NO];
        }
    }
};


wxWidgetImplType* wxWidgetImpl::CreateGauge( wxWindowMac* wxpeer, 
                                    wxWindowMac* parent, 
                                    wxWindowID id, 
                                    wxInt32 value,
                                    wxInt32 minimum,
                                    wxInt32 maximum,
                                    const wxPoint& pos, 
                                    const wxSize& size,
                                    long style, 
                                    long extraStyle)
{
    NSRect r = wxOSXGetFrameForControl( wxpeer, pos , size ) ;
    wxNSProgressIndicator* v = [[wxNSProgressIndicator alloc] initWithFrame:r];

    [v setMinValue: minimum];
    [v setMaxValue: maximum];
    [v setIndeterminate:FALSE];
    [v setDoubleValue: (double) value];
    wxWidgetCocoaImpl* c = new wxOSXGaugeCocoaImpl( wxpeer, v );
    [v setImplementation:c];
    return c;
}

#endif // wxUSE_GAUGE

