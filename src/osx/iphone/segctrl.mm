/////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/iphone/segctrl.mm
// Purpose:     wxSegmentedCtrl
// Author:      Linas Valiukas
// Modified by:
// Created:     2011-07-04
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

#include "wx/segctrl.h"
#include "wx/osx/private.h"
#include "wx/osx/iphone/private/segctrlimpl.h"


#pragma mark -
#pragma mark Cocoa class

@implementation wxUISegmentedControl

- (id)init {
    if ((self = [super init])) {
        moSegmentedCtrl = NULL;
        
        [self addTarget:self
                 action:@selector(segmentHasBeenChanged:)
       forControlEvents:UIControlEventValueChanged];
    }
    
    return self;
}

- (id)initWithWxSegmentedCtrl:(wxSegmentedCtrl *)initWxSegmentedCtrl {
    if ((self = [self init])) {
        moSegmentedCtrl = initWxSegmentedCtrl;
    }
    
    return self;
}

- (void)segmentHasBeenChanged:(id)sender {
    if ( !moSegmentedCtrl ) {
        return;
    }
    
    int selection = [self selectedSegmentIndex];
        
    // wxEVT_COMMAND_TAB_SEL_CHANGING
    // FIXME should be able to veto the change
    wxTabEvent changingEvent(wxEVT_COMMAND_TAB_SEL_CHANGING, moSegmentedCtrl->GetId());
    changingEvent.SetEventObject(moSegmentedCtrl);
    changingEvent.SetEventType(wxEVT_COMMAND_TAB_SEL_CHANGING);
    changingEvent.SetSelection(selection);
    moSegmentedCtrl->GetEventHandler()->ProcessEvent(changingEvent);    
    
    // wxEVT_COMMAND_TAB_SEL_CHANGED
    wxTabEvent changedEvent(wxEVT_COMMAND_TAB_SEL_CHANGED, moSegmentedCtrl->GetId());
    changedEvent.SetEventObject(moSegmentedCtrl);
    changedEvent.SetEventType(wxEVT_COMMAND_TAB_SEL_CHANGED);
    changedEvent.SetSelection(selection);
    moSegmentedCtrl->GetEventHandler()->ProcessEvent(changedEvent);    
}

@end


#pragma mark -
#pragma mark Peer implementation

wxSegmentedCtrlIPhoneImpl::wxSegmentedCtrlIPhoneImpl( wxWindowMac* peer , WXWidget w ) : wxWidgetIPhoneImpl(peer, w)
{
    
}    


wxWidgetImplType* wxWidgetImpl::CreateSegmentedCtrl(wxWindowMac* wxpeer,
                                                    wxWindowMac* WXUNUSED(parent),
                                                    wxWindowID WXUNUSED(id),
                                                    const wxPoint& pos,
                                                    const wxSize& size,
                                                    long style,
                                                    long WXUNUSED(extraStyle))
{
    CGRect r = wxOSXGetFrameForControl( wxpeer, pos , size ) ;
    
    wxUISegmentedControl* v = [[wxUISegmentedControl alloc] init];
    v.frame = r;

    wxWidgetIPhoneImpl* c = new wxSegmentedCtrlIPhoneImpl( wxpeer, v );
    return c;
}


#pragma mark -
#pragma mark wxSegmentedCtrl implementation

IMPLEMENT_DYNAMIC_CLASS(wxSegmentedCtrl, wxSegmentedCtrlBase)


BEGIN_EVENT_TABLE(wxSegmentedCtrl, wxSegmentedCtrlBase)
    EVT_PAINT(wxSegmentedCtrl::OnPaint)
END_EVENT_TABLE()

wxSegmentedCtrl::wxSegmentedCtrl()
{
    Init();
}

wxSegmentedCtrl::wxSegmentedCtrl(wxWindow *parent,
                                     wxWindowID id,
                                     const wxPoint& pos,
                                     const wxSize& size,
                                     long style,
                                     const wxString& name)
{
    Init();
    
    Create(parent, id, pos, size, style, name);
}


bool wxSegmentedCtrl::Create(wxWindow *parent,
                               wxWindowID id,
                               const wxPoint& pos,
                               const wxSize& size,
                               long style,
                               const wxString& name)
{
    DontCreatePeer();
    
    if (! wxSegmentedCtrlBase::Create(parent, id, pos, size, style, name)) {
        return false;
    }
    
    SetPeer(wxWidgetImpl::CreateSegmentedCtrl( this, parent, id, pos, size, style, GetExtraStyle() ));
    
    MacPostControlCreate( pos, size );
    
    return true;
}

void wxSegmentedCtrl::Init()
{
    // FIXME stub
}

#pragma mark wxTabCtrl overrides

bool wxSegmentedCtrl::AddItem(const wxString& text, int imageId)
{
    wxUISegmentedControl *segmentedControl = (wxUISegmentedControl *)(GetPeer()->GetWXWidget());
    if (! segmentedControl) {
        return false;
    }
    
    NSString *segmTitle = [NSString stringWithString:wxCFStringRef(text).AsNSString()];
    if (! segmTitle) {
        return false;
    }
    NSUInteger segmIndex = [segmentedControl numberOfSegments];
    
    [segmentedControl insertSegmentWithTitle:segmTitle
                                     atIndex:segmIndex
                                    animated:NO];
    
    return true;
}

// Add an item, passing a bitmap.
bool wxSegmentedCtrl::AddItem(const wxString& text, const wxBitmap& bitmap)
{
    wxUISegmentedControl *segmentedControl = (wxUISegmentedControl *)(GetPeer()->GetWXWidget());
    if (! segmentedControl) {
        return false;
    }
    
    UIImage *segmImage = [bitmap.GetUIImage() retain];
    if (! segmImage) {
        return false;
    }
    NSUInteger segmIndex = [segmentedControl numberOfSegments];
    
    [segmentedControl insertSegmentWithImage:segmImage
                                     atIndex:segmIndex
                                    animated:NO];
    
    return true;
}

// Set the selection
int wxSegmentedCtrl::SetSelection(int item)
{
    wxUISegmentedControl *segmentedControl = (wxUISegmentedControl *)(GetPeer()->GetWXWidget());
    if (! segmentedControl) {
        return -1;
    }
    
    NSInteger previousSelection = [segmentedControl selectedSegmentIndex];
    [segmentedControl setSelectedSegmentIndex:item];
    
    return previousSelection;
}
