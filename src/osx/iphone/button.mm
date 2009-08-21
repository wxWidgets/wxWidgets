/////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/iphone/button.mm
// Purpose:     wxButton
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id: button.cpp 54845 2008-07-30 14:52:41Z SC $
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#include "wx/button.h"

#ifndef WX_PRECOMP
    #include "wx/panel.h"
    #include "wx/toplevel.h"
    #include "wx/dcclient.h"
#endif

#include "wx/stockitem.h"

#include "wx/osx/private.h"

wxSize wxButton::DoGetBestSize() const
{
    if ( GetId() == wxID_HELP )
        return wxSize( 18 , 18 ) ;

    wxSize sz = GetDefaultSize() ;

    wxRect r ;

    m_peer->GetBestRect(&r);

    if ( r.GetWidth() == 0 && r.GetHeight() == 0 )
    {
    }
    sz.x = r.GetWidth();
    sz.y = r.GetHeight();

    int wBtn = 72;

    if ((wBtn > sz.x) || ( GetWindowStyle() & wxBU_EXACTFIT))
        sz.x = wBtn;

    return sz ;
}

wxSize wxButton::GetDefaultSize()
{
    int wBtn = 72 ;
    int hBtn = 35 ;

    return wxSize(wBtn, hBtn);
}

@implementation wxUIButton

+ (void)initialize
{
    static BOOL initialized = NO;
    if (!initialized)
    {
        initialized = YES;
        wxOSXIPhoneClassAddWXMethods( self );
    }
}

- (int) intValue
{
    return 0;
}

- (void) setIntValue: (int) v
{
}

@end


wxWidgetImplType* wxWidgetImpl::CreateButton( wxWindowMac* wxpeer,
                                    wxWindowMac* WXUNUSED(parent),
                                    wxWindowID id,
                                    const wxString& WXUNUSED(label),
                                    const wxPoint& pos,
                                    const wxSize& size,
                                    long WXUNUSED(style),
                                    long WXUNUSED(extraStyle))
{
    CGRect r = wxOSXGetFrameForControl( wxpeer, pos , size ) ;
    UIButtonType buttonType = UIButtonTypeRoundedRect;

    if ( id == wxID_HELP )
    {
        buttonType = UIButtonTypeInfoDark;
    }

    UIButton* v = [[UIButton buttonWithType:buttonType] retain];
    v.frame = r;
    wxWidgetIPhoneImpl* c = new wxWidgetIPhoneImpl( wxpeer, v );
    return c;
/*
    OSStatus err;
    Rect bounds = wxMacGetBoundsForControl( wxpeer , pos , size ) ;
    wxMacControl* peer = new wxMacControl(wxpeer) ;
    if ( id == wxID_HELP )
    {
        ControlButtonContentInfo info ;
        info.contentType = kControlContentIconRef ;
        GetIconRef(kOnSystemDisk, kSystemIconsCreator, kHelpIcon, &info.u.iconRef);
        err = CreateRoundButtonControl(
            MAC_WXHWND(parent->MacGetTopLevelWindowRef()),
            &bounds, kControlRoundButtonNormalSize,
            &info, peer->GetControlRefAddr() );
    }
    else if ( label.Find('\n' ) == wxNOT_FOUND && label.Find('\r' ) == wxNOT_FOUND)
    {
        // Button height is static in Mac, can't be changed, so we need to force it here
        int maxHeight;
        switch (wxpeer->GetWindowVariant() )
        {
            case wxWINDOW_VARIANT_NORMAL:
            case wxWINDOW_VARIANT_LARGE:
                maxHeight = 20 ;
                break;
            case wxWINDOW_VARIANT_SMALL:
                maxHeight = 17;
            case wxWINDOW_VARIANT_MINI:
                maxHeight = 15;
            default:
                break;
        }
        bounds.bottom = bounds.top + maxHeight ;
        wxpeer->SetMaxSize( wxSize( wxpeer->GetMaxWidth() , maxHeight ));
        err = CreatePushButtonControl(
            MAC_WXHWND(parent->MacGetTopLevelWindowRef()),
            &bounds, CFSTR(""), peer->GetControlRefAddr() );
    }
    else
    {
        ControlButtonContentInfo info ;
        info.contentType = kControlNoContent ;
        err = CreateBevelButtonControl(
            MAC_WXHWND(parent->MacGetTopLevelWindowRef()) , &bounds, CFSTR(""),
            kControlBevelButtonLargeBevel, kControlBehaviorPushbutton,
            &info, 0, 0, 0, peer->GetControlRefAddr() );
    }
    verify_noerr( err );
    return peer;
    */
}

void wxWidgetIPhoneImpl::SetDefaultButton( bool isDefault )
{
}

void wxWidgetIPhoneImpl::PerformClick()
{
}

wxWidgetImplType* wxWidgetImpl::CreateDisclosureTriangle( wxWindowMac* wxpeer,
                                    wxWindowMac* WXUNUSED(parent),
                                    wxWindowID WXUNUSED(id),
                                    const wxString& label,
                                    const wxPoint& pos,
                                    const wxSize& size,
                                    long WXUNUSED(style),
                                    long WXUNUSED(extraStyle))
{
    CGRect r = wxOSXGetFrameForControl( wxpeer, pos , size ) ;
    wxUIButton* v = [[wxUIButton alloc] initWithFrame:r];
    [v setTitle:wxCFStringRef( label).AsNSString() forState:UIControlStateNormal];
    wxWidgetIPhoneImpl* c = new wxWidgetIPhoneImpl( wxpeer, v );
    return c;
}
