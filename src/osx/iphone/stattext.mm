/////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/iphone/stattext.mm
// Purpose:     wxStaticText
// Author:      Stefan Csomor
// Modified by:
// Created:     04/01/98
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

@interface wxUILabel : UILabel
{
}
@end

@implementation wxUILabel

+ (void)initialize
{
    static BOOL initialized = NO;
    if (!initialized)
    {
        initialized = YES;
        wxOSXIPhoneClassAddWXMethods( self );
    }
}

@end

class wxStaticTextIPhoneImpl : public wxWidgetIPhoneImpl
{
public:
    wxStaticTextIPhoneImpl( wxWindowMac* peer , WXWidget w ) : wxWidgetIPhoneImpl(peer, w)
    {
    }

    virtual void SetLabel(const wxString& title, wxFontEncoding encoding)
    {
        wxUILabel* v = (wxUILabel*)GetWXWidget();
        wxCFStringRef text( title , encoding );
        
        [v setText:text.AsNSString()];
    }
private :
};

wxSize wxStaticText::DoGetBestSize() const
{
    return wxWindowMac::DoGetBestSize() ;
}

wxWidgetImplType* wxWidgetImpl::CreateStaticText( wxWindowMac* wxpeer,
                                    wxWindowMac* WXUNUSED(parent),
                                    wxWindowID WXUNUSED(id),
                                    const wxString& WXUNUSED(label),
                                    const wxPoint& pos,
                                    const wxSize& size,
                                    long style,
                                    long WXUNUSED(extraStyle))
{
    CGRect r = wxOSXGetFrameForControl( wxpeer, pos , size ) ;
    wxUILabel* v = [[wxUILabel alloc] initWithFrame:r];
    v.backgroundColor = [UIColor clearColor];

    NSLineBreakMode linebreak = NSLineBreakByWordWrapping;
    if ( style & wxST_ELLIPSIZE_MASK )
    {
        if ( style & wxST_ELLIPSIZE_MIDDLE )
            linebreak = NSLineBreakByTruncatingMiddle;
        else if (style & wxST_ELLIPSIZE_END )
            linebreak = NSLineBreakByTruncatingTail;
        else if (style & wxST_ELLIPSIZE_START )
            linebreak = NSLineBreakByTruncatingHead;
    }
    [v setLineBreakMode:linebreak];

    if (style & wxALIGN_CENTER)
        [v setTextAlignment: NSTextAlignmentCenter];
    else if (style & wxALIGN_RIGHT)
        [v setTextAlignment: NSTextAlignmentRight];
    
    wxWidgetIPhoneImpl* c = new wxStaticTextIPhoneImpl( wxpeer, v );
    return c;
}

#endif //if wxUSE_STATTEXT
