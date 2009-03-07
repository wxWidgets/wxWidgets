/////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/cocoa/stattext.mm
// Purpose:     wxStaticText
// Author:      Stefan Csomor
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id: stattext.cpp 54845 2008-07-30 14:52:41Z SC $
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_STATTEXT

#include "wx/stattext.h"

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/utils.h"
    #include "wx/dc.h"
    #include "wx/dcclient.h"
    #include "wx/settings.h"
#endif // WX_PRECOMP

#include "wx/osx/private.h"

#include <stdio.h>

@interface wxNSStaticTextView : NSTextView
{
    wxWidgetCocoaImpl* impl;
}

- (void) setImplementation:(wxWidgetCocoaImpl*) item;
- (wxWidgetCocoaImpl*) implementation;
@end
@implementation wxNSStaticTextView

+ (void)initialize
{
    static BOOL initialized = NO;
    if (!initialized) 
    {    
        initialized = YES;
        wxOSXCocoaClassAddWXMethods( self );
    }
}

- (wxWidgetCocoaImpl*) implementation
{
    return impl;
}

- (void) setImplementation:(wxWidgetCocoaImpl*) item
{
    impl = item;
}
@end


wxSize wxStaticText::DoGetBestSize() const
{
Point bounds;

#if wxOSX_USE_ATSU_TEXT
    OSStatus err = noErr;
    wxCFStringRef str( m_label,  GetFont().GetEncoding() );

    SInt16 baseline;
    if ( m_font.MacGetThemeFontID() != kThemeCurrentPortFont )
    {
        err = GetThemeTextDimensions(
            (!m_label.empty() ? (CFStringRef)str : CFSTR(" ")),
            m_font.MacGetThemeFontID(), kThemeStateActive, false, &bounds, &baseline );
        verify_noerr( err );
    }
    else
#endif
    {
        wxClientDC dc(const_cast<wxStaticText*>(this));
        wxCoord width, height ;
        dc.GetMultiLineTextExtent( m_label , &width, &height);
        // FIXME: Some labels seem to have their last characters
        // stripped out.  Adding 12 pixels seems to be enough to fix this.
        // Perhaps m_label is not being synced properly...
        bounds.h = width+12;
        bounds.v = height;
    }

    if ( m_label.empty() )
        bounds.h = 0;

    bounds.h += MacGetLeftBorderSize() + MacGetRightBorderSize();
    bounds.v += MacGetTopBorderSize() + MacGetBottomBorderSize();

    return wxSize( bounds.h, bounds.v );
}

// for wxST_ELLIPSIZE_* support:

/*
   FIXME: UpdateLabel() should be called on size events when wxST_ELLIPSIZE_START is set
          to allow correct dynamic ellipsizing of the label
*/

wxWidgetImplType* wxWidgetImpl::CreateStaticText( wxWindowMac* wxpeer,
                                    wxWindowMac* parent,
                                    wxWindowID id,
                                    const wxString& label,
                                    const wxPoint& pos,
                                    const wxSize& size,
                                    long style,
                                    long extraStyle)
{
    NSRect r = wxOSXGetFrameForControl( wxpeer, pos , size ) ;
    wxNSStaticTextView* v = [[wxNSStaticTextView alloc] initWithFrame:r];

    [v setEditable:NO];
    [v setDrawsBackground:NO];
    [v setString: wxCFStringRef( label , wxpeer->GetFont().GetEncoding() ).AsNSString()];
    
    NSRange allText = NSMakeRange(0, label.length());
    if (style & wxALIGN_CENTER)
        [v setAlignment: NSCenterTextAlignment range: allText];
    else if (style & wxALIGN_RIGHT)
        [v setAlignment: NSRightTextAlignment range: allText];

    wxWidgetCocoaImpl* c = new wxWidgetCocoaImpl( wxpeer, v );
    return c;
/*
    Rect bounds = wxMacGetBoundsForControl( wxpeer, pos, size );

    wxMacControl* peer = new wxMacControl( wxpeer );
    OSStatus err = CreateStaticTextControl(
        MAC_WXHWND(parent->MacGetTopLevelWindowRef()),
        &bounds, NULL, NULL, peer->GetControlRefAddr() );
    verify_noerr( err );

    if ( ( style & wxST_ELLIPSIZE_END ) || ( style & wxST_ELLIPSIZE_MIDDLE ) )
    {
        TruncCode tCode = truncEnd;
        if ( style & wxST_ELLIPSIZE_MIDDLE )
            tCode = truncMiddle;

        err = peer->SetData( kControlStaticTextTruncTag, tCode );
        err = peer->SetData( kControlStaticTextIsMultilineTag, (Boolean)0 );
    }
    return peer;
    */
}

#endif //if wxUSE_STATTEXT
