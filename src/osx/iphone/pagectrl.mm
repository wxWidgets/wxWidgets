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
    wxPageCtrl *moPageCtrl;
}

- (id)initWithWxPageCtrl:(wxPageCtrl *)initWxPageCtrl;
- (void)pageHasBeenChanged:(id)sender;

@end

@implementation wxUIPageControl

- (id)init {
    if ((self = [self init])) {
        moPageCtrl = NULL;
        
        [self addTarget:self
                 action:@selector(pageHasBeenChanged:)
       forControlEvents:UIControlEventValueChanged];        
    }
    
    return self;
}

- (id)initWithWxPageCtrl:(wxPageCtrl *)initWxPageCtrl {
    if ((self = [super init])) {
        moPageCtrl = initWxPageCtrl;
    }
    
    return self;
}

- (void)pageHasBeenChanged:(id)sender {
    if ( !moPageCtrl ) {
        return;
    }
    
    wxCommandEvent event(wxEVT_COMMAND_BUTTON_CLICKED, moPageCtrl->GetId());
    event.SetEventObject(moPageCtrl);
    event.SetInt(moPageCtrl->GetCurrentPage());
    event.SetExtraLong(moPageCtrl->GetCurrentPage());
    
    moPageCtrl->GetEventHandler()->ProcessEvent(event);
}

@end


#pragma mark -
#pragma mark Peer implementation

class wxPageCtrlIPhoneImpl : public wxWidgetIPhoneImpl
{
public:
    wxPageCtrlIPhoneImpl( wxWindowMac* peer , WXWidget w ) : wxWidgetIPhoneImpl(peer, w)
    {
        m_pageControl = w;
    }
    
    void SetPageCount(int pageCount)
    {
        [m_pageControl setNumberOfPages:pageCount];
    }
    
    void SetCurrentPage(int page)
    {
        [m_pageControl setCurrentPage:page];
    }
    
    
    
private:
    wxUIPageControl* m_pageControl;
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
    
    BOOL hidesForSinglePage = NO;
    if (style & wxPC_HIDE_FOR_SINGLE_PAGE) {
        hidesForSinglePage = YES;
    }

    wxUIPageControl* v = [[wxUIPageControl alloc] initWithWxPageCtrl:(wxPageCtrl *)wxpeer];
    [v setNumberOfPages:0];
    [v setHidesForSinglePage:hidesForSinglePage];
    
    wxWidgetIPhoneImpl* c = new wxPageCtrlIPhoneImpl( wxpeer, v );
    return c;
}


#pragma mark -
#pragma mark wxPageCtrl implementation

IMPLEMENT_DYNAMIC_CLASS(wxPageCtrl, wxPageCtrlBase)

BEGIN_EVENT_TABLE(wxPageCtrl, wxPageCtrlBase)
END_EVENT_TABLE()


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

/// Sets the page count.
void wxPageCtrl::SetPageCount(int pageCount)
{
    wxPageCtrlIPhoneImpl* peer = (wxPageCtrlIPhoneImpl *)GetPeer();
    wxASSERT_MSG(peer, "No peer");
    if ( !peer ) {
        return;
    }
    
    peer->SetPageCount(pageCount);
    m_pageCount = pageCount;
}

/// Sets the current page.
void wxPageCtrl::SetCurrentPage(int page)
{
    wxPageCtrlIPhoneImpl* peer = (wxPageCtrlIPhoneImpl *)GetPeer();
    wxASSERT_MSG(peer, "No peer");
    if ( !peer ) {
        return;
    }
    
    peer->SetCurrentPage(page);
    
    m_currentPage = page;
}
