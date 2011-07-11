/////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/iphone/pagectrl.mm
// Purpose:     wxMoPageCtrl
// Author:      Linas Valiukas
// Modified by:
// Created:     2011-07-02
// RCS-ID:      $Id$
// Copyright:   (c) Linas Valiukas
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/app.h"
#include "wx/utils.h"
#include "wx/dc.h"
#include "wx/dcclient.h"
#include "wx/settings.h"
#endif // WX_PRECOMP

#include "wx/osx/private.h"
#include "wx/pagectrl.h"


#pragma mark -
#pragma mark Cocoa class

@interface wxUIPageControl : UIPageControl
{
}

@end

@implementation wxUIPageControl


@end


#pragma mark -
#pragma mark Peer implementation

class wxPageCtrlIPhoneImpl : public wxWidgetIPhoneImpl
{
public:
    wxPageCtrlIPhoneImpl( wxWindowMac* peer , WXWidget w ) : wxWidgetIPhoneImpl(peer, w)
    {
        
    }    
};

wxWidgetImplType* wxWidgetImpl::CreatePageCtrl(wxWindowMac* wxpeer,
                                               wxWindowMac* WXUNUSED(parent),
                                               wxWindowID WXUNUSED(id),
                                               const wxPoint& pos,
                                               const wxSize& size,
                                               long style,
                                               long WXUNUSED(extraStyle))
{
    CGRect r = wxOSXGetFrameForControl( wxpeer, pos , size ) ;

    wxUIPageControl* v = [[wxUIPageControl alloc] init];
    [v setNumberOfPages:4];
    [v setCurrentPage:2];
    [v setHidesForSinglePage:NO];
    
    wxWidgetIPhoneImpl* c = new wxPageCtrlIPhoneImpl( wxpeer, v );
    return c;
}


#pragma mark -
#pragma mark wxPageCtrl implementation

bool wxPageCtrl::Create(wxWindow *parent,
                          wxWindowID id,
                          const wxPoint& pos,
                          const wxSize& size,
                          long style,
                          const wxValidator& validator,
                          const wxString& name)
{
    DontCreatePeer();
    
    if ( !wxControl::Create( parent, id, pos, size, style, validator, name )) {
        return false;
    }
    
    SetPeer(wxWidgetImpl::CreatePageCtrl( this, parent, id, pos, size, style, GetExtraStyle() ));
    
    MacPostControlCreate( pos, size );
    
    return true;
}

wxPageCtrl::~wxPageCtrl()
{
}

void wxPageCtrl::Init()
{
    
}
