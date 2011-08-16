/////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/iphone/button.mm
// Purpose:     wxButton
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id$
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
    wxSize sz;
    
    wxRect r;
    GetPeer()->GetBestRect(&r);

    sz.x = r.GetWidth();
    sz.y = r.GetHeight();

    if ( GetWindowStyle() & ( wxBU_DISCLOSURE | wxBU_INFO_LIGHT | wxBU_INFO_DARK |wxBU_CONTACT_ADD ) )
    {
        // never adjust for these
    }
    else 
    {
        int wBtn = GetDefaultSize().x;
        if ((wBtn > sz.x) || ( GetWindowStyle() & wxBU_EXACTFIT))
            sz.x = wBtn;
    }
    
    return sz ;
}

wxSize wxButton::GetDefaultSize()
{
    int wBtn = 72 ;
    int hBtn = 35 ;

    return wxSize(wBtn, hBtn);
}

@implementation wxUIButton

+ (void)initialize {
    static BOOL initialized = NO;
    if (!initialized) {
        initialized = YES;
        wxOSXIPhoneClassAddWXMethods( self );
    }
}

- (int)intValue {
    return 0;
}

- (void)setIntValue:(int)v
{
    
}

- (NSString *)description {
    
    NSString *strButtonType = nil;
    switch (self.buttonType) {
        case UIButtonTypeCustom:    strButtonType = @"UIButtonTypeCustom";  break;
        case UIButtonTypeRoundedRect:   strButtonType = @"UIButtonTypeRoundedRect"; break;
        case UIButtonTypeDetailDisclosure:  strButtonType = @"UIButtonTypeDetailDisclosure";    break;
        case UIButtonTypeInfoLight: strButtonType = @"UIButtonTypeInfoLight";   break;
        case UIButtonTypeInfoDark:  strButtonType = @"UIButtonTypeInfoDark";    break;
        case UIButtonTypeContactAdd:    strButtonType = @"UIButtonTypeContactAdd";  break;
        default:    strButtonType = @"UNKNOWN";  break;
    };
    
    NSMutableString *description = [NSMutableString stringWithString:@""];
    [description appendFormat:@"<%@: 0x%08x; superClass = %@; frame = %@; hidden = %@; opaque = %@; alpha = %f; backgroundColor = %@, buttonType = %@>",
     NSStringFromClass([self class]),
     self,
     NSStringFromClass([self superclass]),
     NSStringFromCGRect(self.frame),
     (self.hidden ? @"YES" : @"NO"),
     (self.opaque ? @"YES" : @"NO"),
     self.alpha,
     [self.backgroundColor description],
     strButtonType];
    
    return [NSString stringWithString:description];

    
}

@end

#pragma mark -
#pragma mark Peer implementation

class wxButtonIPhoneImpl : public wxWidgetIPhoneImpl
{
public:
    wxButtonIPhoneImpl( wxWindowMac* peer , WXWidget w, UIButtonType buttonType ) : wxWidgetIPhoneImpl(peer, w)
    {
        m_buttonType = buttonType;
    }
        
    void SetLabel(const wxString& title, wxFontEncoding encoding)
    {
        if (AcceptsLabel() and !title.empty()) {
            wxCFStringRef cf( title , encoding );
            [m_osxView setTitle:cf.AsNSString()
                         forState:UIControlStateNormal];
        } else {
            [m_osxView setTitle:nil
                       forState:UIControlStateNormal];            
        }
    }
    
    bool AcceptsLabel() const
    {
        return (m_buttonType == UIButtonTypeCustom || m_buttonType == UIButtonTypeRoundedRect);
    }
        
    bool IsEnabled() const
    {
        [m_osxView enabled];
    }
    
    void Enable( bool enable )
    {
        [m_osxView setEnabled:enable];
    }

    
private:
    // Save a copy of button's type because [m_osxView buttonType]
    // doesn't work with anything but round rect. or custom
    // (maybe has something to do with UIButton being a class cluster)
    UIButtonType m_buttonType;
};


// "Private" button creation function
wxWidgetImplType* CreateIPhoneButton(wxWindowMac* wxpeer,
                                     wxWindowMac* parent,
                                     wxWindowID id,
                                     const wxString& label,
                                     const wxBitmap& bitmap,
                                     const wxPoint& pos,
                                     const wxSize& size,
                                     long style,
                                     long extraStyle)
{
    // Type
    UIButtonType buttonType;
    if (style & wxBU_ROUNDED_RECTANGLE) {
        buttonType = UIButtonTypeRoundedRect;
    } else if (style & wxBU_DISCLOSURE) {
        buttonType = UIButtonTypeDetailDisclosure;
    } else if (style & wxBU_INFO_LIGHT) {
        buttonType = UIButtonTypeInfoLight;
    } else if (style & wxBU_INFO_DARK) {
        buttonType = UIButtonTypeInfoDark;
    } else if (style & wxBU_CONTACT_ADD) {
        buttonType = UIButtonTypeContactAdd;
    } else {
        buttonType = UIButtonTypeRoundedRect;
    }
    
    UIButton* v = [[UIButton buttonWithType:buttonType] retain];
    v.frame = wxOSXGetFrameForControl( wxpeer, pos , size ) ;
    
    if ( buttonType == UIButtonTypeRoundedRect )
    {
        // Bitmap

        if (bitmap.IsOk()) 
        {
            [v setImage:[bitmap.GetUIImage() retain]
               forState:UIControlStateNormal];
        } else 
        {
            [v setImage:nil
               forState:UIControlStateNormal];
        }
    }
    
    // Label is being set elsewhere (in SetPeer())
    
    return new wxButtonIPhoneImpl( wxpeer, v, buttonType );
}


wxWidgetImplType* wxWidgetImpl::CreateButton(wxWindowMac* wxpeer,
                                             wxWindowMac* parent,
                                             wxWindowID id,
                                             const wxString& label,
                                             const wxPoint& pos,
                                             const wxSize& size,
                                             long style,
                                             long extraStyle)
{
    return CreateIPhoneButton(wxpeer, parent, id, label, wxNullBitmap, pos, size, style, extraStyle);
}

wxWidgetImplType* wxWidgetImpl::CreateBitmapButton(wxWindowMac* wxpeer,
                                                   wxWindowMac* parent,
                                                   wxWindowID id,
                                                   const wxBitmap& bitmap,
                                                   const wxPoint& pos,
                                                   const wxSize& size,
                                                   long style,
                                                   long extraStyle)
{    
    return CreateIPhoneButton(wxpeer, parent, id, wxEmptyString, bitmap, pos, size, style, extraStyle);
}

wxWidgetImplType* wxWidgetImpl::CreateDisclosureTriangle(wxWindowMac* wxpeer,
                                                         wxWindowMac* parent,
                                                         wxWindowID id,
                                                         const wxString& WXUNUSED(label),
                                                         const wxPoint& pos,
                                                         const wxSize& size,
                                                         long style,
                                                         long extraStyle)
{
    return CreateIPhoneButton(wxpeer, parent, id, wxEmptyString, wxNullBitmap, pos, size, style, extraStyle);
}
