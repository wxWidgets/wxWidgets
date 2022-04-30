/////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/cocoa/gauge.mm
// Purpose:     wxGauge class
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// Copyright:   (c) Stefan Csomor
// Licence:       wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_GAUGE

#include "wx/gauge.h"

#include "wx/osx/private.h"

@interface wxNSProgressIndicator : NSProgressIndicator
{
}

@end

@implementation wxNSProgressIndicator

+ (void)initialize
{
    static BOOL initialized = NO;
    if (!initialized)
    {
        initialized = YES;
        wxOSXCocoaClassAddWXMethods( self );
    }
}

@end

@interface NSView(PossibleSizeMethods)
- (NSControlSize)controlSize;
@end

namespace
{

class wxOSXGaugeCocoaImpl : public wxWidgetCocoaImpl
{
public :
    wxOSXGaugeCocoaImpl( wxWindowMac* peer, WXWidget w) : wxWidgetCocoaImpl( peer, w )
    {
    }

    void SetMaximum(wxInt32 v) wxOVERRIDE
    {
        SetDeterminateMode();
        wxWidgetCocoaImpl::SetMaximum( v ) ;
    }

    void SetValue(wxInt32 v) wxOVERRIDE
    {
        SetDeterminateMode();
        wxWidgetCocoaImpl::SetValue( v ) ;
    }

    void PulseGauge() wxOVERRIDE
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
    
} // anonymous namespace

wxWidgetImplType* wxWidgetImpl::CreateGauge( wxWindowMac* wxpeer,
                                    wxWindowMac* WXUNUSED(parent),
                                    wxWindowID WXUNUSED(id),
                                    wxInt32 value,
                                    wxInt32 minimum,
                                    wxInt32 maximum,
                                    const wxPoint& pos,
                                    const wxSize& size,
                                    long style,
                                    long WXUNUSED(extraStyle))
{
    NSRect r = wxOSXGetFrameForControl( wxpeer, pos , size ) ;
    wxNSProgressIndicator* v = [[wxNSProgressIndicator alloc] initWithFrame:r];

    [v setMinValue: minimum];
    [v setMaxValue: maximum];
    [v setIndeterminate:FALSE];
    [v setDoubleValue: (double) value];
    if (style & wxGA_VERTICAL)
    {
        [v setBoundsRotation:-90.0];
    }
    wxWidgetCocoaImpl* c = new wxOSXGaugeCocoaImpl( wxpeer, v );
    return c;
}

#endif // wxUSE_GAUGE

