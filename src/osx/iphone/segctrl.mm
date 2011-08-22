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
    
    // wxEVT_COMMAND_MENU_SELECTED
    wxCommandEvent changedEvent(wxEVT_COMMAND_MENU_SELECTED, moSegmentedCtrl->GetId());
    changedEvent.SetEventObject(moSegmentedCtrl);
    changedEvent.SetEventType(wxEVT_COMMAND_MENU_SELECTED);
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
    
    wxUISegmentedControl* v = [[wxUISegmentedControl alloc] initWithWxSegmentedCtrl:(wxSegmentedCtrl *)wxpeer];
    v.frame = r;

    wxWidgetIPhoneImpl* c = new wxSegmentedCtrlIPhoneImpl( wxpeer, v );
    return c;
}


#pragma mark -
#pragma mark wxSegmentedCtrl implementation

IMPLEMENT_DYNAMIC_CLASS(wxSegmentedCtrl, wxSegmentedCtrlBase)


BEGIN_EVENT_TABLE(wxSegmentedCtrl, wxSegmentedCtrlBase)
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
    
    if (! wxControl::Create(parent, id, pos, size, style)) {
        return false;
    }
    
    SetPeer(wxWidgetImpl::CreateSegmentedCtrl( this, parent, id, pos, size, style, GetExtraStyle() ));
    
    MacPostControlCreate( pos, size );
    
    return true;
}

/// Insert an item, passing an optional index into the image list.
bool wxSegmentedCtrl::InsertItem(int item, const wxString& text, int WXUNUSED(imageId))
{
    wxUISegmentedControl *segmentedControl = (wxUISegmentedControl *)(GetPeer()->GetWXWidget());
    if (! segmentedControl) {
        return false;
    }
    
    NSString *segmTitle = [NSString stringWithString:wxCFStringRef(text).AsNSString()];
    if (! segmTitle) {
        return false;
    }
    [segmentedControl insertSegmentWithTitle:segmTitle
                                     atIndex:item
                                    animated:NO];
    
    return true;
}

/// Insert an item, passing a bitmap.
bool wxSegmentedCtrl::InsertItem(int item, const wxString& text, const wxBitmap& bitmap)
{
    wxUISegmentedControl *segmentedControl = (wxUISegmentedControl *)(GetPeer()->GetWXWidget());
    if (! segmentedControl) {
        return false;
    }
    
    UIImage *segmImage = [bitmap.GetUIImage() retain];
    if (! segmImage) {
        return false;
    }
    [segmentedControl insertSegmentWithImage:segmImage
                                     atIndex:item
                                    animated:NO];
    
    return true;
}

/// Get the number of items
int wxSegmentedCtrl::GetItemCount() const
{
    wxUISegmentedControl *segmentedControl = (wxUISegmentedControl *)(GetPeer()->GetWXWidget());
    if (! segmentedControl) {
        return -1;
    }
    
    return [segmentedControl numberOfSegments];
}

/// Get the selection
int wxSegmentedCtrl::GetSelection() const
{
    wxUISegmentedControl *segmentedControl = (wxUISegmentedControl *)(GetPeer()->GetWXWidget());
    if (! segmentedControl) {
        return -1;
    }
    
    return [segmentedControl selectedSegmentIndex];
}

// Set the selection
int wxSegmentedCtrl::SetSelection(int item)
{
    int previousSelection = ChangeSelection(item);
    
    // wxEVT_COMMAND_MENU_SELECTED
    wxCommandEvent changedEvent(wxEVT_COMMAND_MENU_SELECTED, GetId());
    changedEvent.SetEventObject(this);
    changedEvent.SetEventType(wxEVT_COMMAND_MENU_SELECTED);
    GetEventHandler()->ProcessEvent(changedEvent);    
    
    return previousSelection;
}

/// Set the selection, without generating events
int wxSegmentedCtrl::ChangeSelection(int item)
{
    wxUISegmentedControl *segmentedControl = (wxUISegmentedControl *)(GetPeer()->GetWXWidget());
    if (! segmentedControl) {
        return -1;
    }
    
    NSInteger previousSelection = [segmentedControl selectedSegmentIndex];
    [segmentedControl setSelectedSegmentIndex:item];
        
    return previousSelection;
}

/// Get the item text
wxString wxSegmentedCtrl::GetItemText(int item) const
{
    wxUISegmentedControl *segmentedControl = (wxUISegmentedControl *)(GetPeer()->GetWXWidget());
    if (! segmentedControl) {
        return wxEmptyString;
    }
    
    NSString *itemText = [segmentedControl titleForSegmentAtIndex:item];
    if ( !itemText ) {
        return wxEmptyString;
    }
    
    return wxString([itemText cStringUsingEncoding:[NSString defaultCStringEncoding]]);
}

/// Set the text for an item
bool wxSegmentedCtrl::SetItemText(int item, const wxString& text)
{
    wxUISegmentedControl *segmentedControl = (wxUISegmentedControl *)(GetPeer()->GetWXWidget());
    if (! segmentedControl) {
        return -1;
    }

    [segmentedControl setTitle:[NSString stringWithString:wxCFStringRef(text).AsNSString()]
             forSegmentAtIndex:item];
    
    return true;
}

/// Get the item image
wxBitmap wxSegmentedCtrl::GetItemImage(int item) const
{
    wxUISegmentedControl *segmentedControl = (wxUISegmentedControl *)(GetPeer()->GetWXWidget());
    if (! segmentedControl) {
        return wxNullBitmap;
    }

    UIImage *itemImage = [segmentedControl imageForSegmentAtIndex:item];
    if ( !itemImage ) {
        return wxNullBitmap;
    }
    
    return wxBitmap([itemImage CGImage]);
}

/// Set the image for an item
bool wxSegmentedCtrl::SetItemImage(int item, wxBitmap& image)
{
    wxUISegmentedControl *segmentedControl = (wxUISegmentedControl *)(GetPeer()->GetWXWidget());
    if (! segmentedControl) {
        return false;
    }
    
    UIImage *itemImage = image.GetUIImage();
    if ( !itemImage ) {
        return false;
    }
    
    [segmentedControl setImage:itemImage
             forSegmentAtIndex:item];
    
    return true;
}

/// Get the content offset
wxSize wxSegmentedCtrl::GetContentOffset(int item) const
{
    wxUISegmentedControl *segmentedControl = (wxUISegmentedControl *)(GetPeer()->GetWXWidget());
    if (! segmentedControl) {
        return wxSize(0, 0);
    }

    return wxFromNSSize(NULL, [segmentedControl contentOffsetForSegmentAtIndex:item]);
}

/// Set the content offset
void wxSegmentedCtrl::SetContentOffset(int item, const wxSize& size)
{
    wxUISegmentedControl *segmentedControl = (wxUISegmentedControl *)(GetPeer()->GetWXWidget());
    if (! segmentedControl) {
        return;
    }
    
    [segmentedControl setContentOffset:wxToNSSize(NULL, size)
                     forSegmentAtIndex:item];
}

/// Get the segment width
float wxSegmentedCtrl::GetWidth(int item) const
{
    wxUISegmentedControl *segmentedControl = (wxUISegmentedControl *)(GetPeer()->GetWXWidget());
    if (! segmentedControl) {
        return 0.0f;
    }

    return [segmentedControl widthForSegmentAtIndex:item];
}

/// Set the segment width
void wxSegmentedCtrl::SetWidth(int item, const float width)
{
    wxUISegmentedControl *segmentedControl = (wxUISegmentedControl *)(GetPeer()->GetWXWidget());
    if (! segmentedControl) {
        return;
    }

    [segmentedControl setWidth:width
             forSegmentAtIndex:item];
}

/// Set button background ("tint") colour
void wxSegmentedCtrl::SetButtonBackgroundColour(const wxColour& colour)
{
    wxUISegmentedControl *segmentedControl = (wxUISegmentedControl *)(GetPeer()->GetWXWidget());
    if (! segmentedControl) {
        return;
    }

    UIColor *uiColor = [[[UIColor alloc] initWithCGColor:colour.GetCGColor()] autorelease];
    [segmentedControl setTintColor:uiColor];
}

/// Get button background ("tint") colour
const wxColour& wxSegmentedCtrl::GetButtonBackgroundColour() const
{
    wxUISegmentedControl *segmentedControl = (wxUISegmentedControl *)(GetPeer()->GetWXWidget());
    if (! segmentedControl) {
        return wxNullColour;
    }
    
    UIColor *color = [segmentedControl tintColor];
    if ( !color ) {
        return wxNullColour;
    }

    return wxColour([color CGColor]);
}

/// Delete an item
bool wxSegmentedCtrl::DeleteItem(int item)
{
    wxUISegmentedControl *segmentedControl = (wxUISegmentedControl *)(GetPeer()->GetWXWidget());
    if (! segmentedControl) {
        return -1;
    }
    
    [segmentedControl removeSegmentAtIndex:item
                                  animated:NO];
    
    return true;
}

/// Delete all items
bool wxSegmentedCtrl::DeleteAllItems()
{
    wxUISegmentedControl *segmentedControl = (wxUISegmentedControl *)(GetPeer()->GetWXWidget());
    if (! segmentedControl) {
        return -1;
    }
    
    [segmentedControl removeAllSegments];
    
    return true;
}
