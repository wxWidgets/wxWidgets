/////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/iphone/notebook.mm
// Purpose:     wxNotebook
// Author:      Stefan Csomor
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_NOTEBOOK

#include "wx/notebook.h"

#ifndef WX_PRECOMP
#include "wx/app.h"
#include "wx/utils.h"
#include "wx/dc.h"
#include "wx/dcclient.h"
#include "wx/settings.h"
#endif // WX_PRECOMP

#include "wx/osx/private.h"

#include <stdio.h>

@interface wxUITabBar : UITabBar
{
}
@end

@implementation wxUITabBar

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

class wxNotebookIPhoneImpl : public wxWidgetIPhoneImpl
{
public:
    wxNotebookIPhoneImpl( wxWindowMac* peer , WXWidget w ) : wxWidgetIPhoneImpl(peer, w)
    {
    }
    
    /*
    virtual void SetLabel(const wxString& title, wxFontEncoding encoding)
    {
        wxUILabel* v = (wxUILabel*)GetWXWidget();
        wxCFStringRef text( title , encoding );
        
        [v setText:text.AsNSString()];
    }
    */
    
    private :
};

wxWidgetImplType* wxWidgetImpl::CreateTabView( wxWindowMac* wxpeer,
                                                 wxWindowMac* WXUNUSED(parent),
                                                 wxWindowID WXUNUSED(id),
                                                 const wxPoint& pos,
                                                 const wxSize& size,
                                                 long style,
                                                 long WXUNUSED(extraStyle))
{
    CGRect r = wxOSXGetFrameForControl( wxpeer, pos , size ) ;
    wxUITabBar* v = [[wxUITabBar alloc] initWithFrame:r];
    v.backgroundColor = [UIColor clearColor];
    
    /*
    UILineBreakMode linebreak = UILineBreakModeWordWrap;
    if ( ((wxStaticText*)wxpeer)->IsEllipsized() )
    {
        if ( style & wxST_ELLIPSIZE_MIDDLE )
            linebreak = UILineBreakModeMiddleTruncation;
        else if (style & wxST_ELLIPSIZE_END )
            linebreak = UILineBreakModeTailTruncation;
        else if (style & wxST_ELLIPSIZE_START )
            linebreak = UILineBreakModeHeadTruncation;
    }
    [v setLineBreakMode:linebreak];
    
    if (style & wxALIGN_CENTER)
        [v setTextAlignment: UITextAlignmentCenter];
    else if (style & wxALIGN_RIGHT)
        [v setTextAlignment: UITextAlignmentRight];
    */
    
    wxWidgetIPhoneImpl* c = new wxNotebookIPhoneImpl( wxpeer, v );
    return c;
}

#endif //if wxUSE_NOTEBOOK
