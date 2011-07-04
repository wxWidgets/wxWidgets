/////////////////////////////////////////////////////////////////////////////
// Name:        src/mobile/generic/mo_segctrl_g.h
// Purpose:     wxMoSegmentedCtrl class
// Author:      Julian Smart, Linas Valiukas
// Modified by:
// Created:     2011-06-14
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart, Linas Valiukas
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/brush.h"
    #include "wx/settings.h"
    #include "wx/dcscreen.h"
    #include "wx/dcclient.h"
    #include "wx/toplevel.h"
#endif

#include "wx/mobile/native/segctrl.h"
#include "wx/osx/private.h"
#include "wx/osx/iphone/private/segctrlimpl.h"

#include "wx/imaglist.h"
#include "wx/dcbuffer.h"

IMPLEMENT_DYNAMIC_CLASS(wxMoSegmentedCtrl, wxMoTabCtrl)

BEGIN_EVENT_TABLE(wxMoSegmentedCtrl, wxMoTabCtrl)
    EVT_PAINT(wxMoSegmentedCtrl::OnPaint)
END_EVENT_TABLE()

wxMoSegmentedCtrl::wxMoSegmentedCtrl()
{
    Init();
}

wxMoSegmentedCtrl::wxMoSegmentedCtrl(wxWindow *parent,
                                     wxWindowID id,
                                     const wxPoint& pos,
                                     const wxSize& size,
                                     long style,
                                     const wxString& name)
{
    Init();
    
    Create(parent, id, pos, size, style, name);
}


bool wxMoSegmentedCtrl::Create(wxWindow *parent,
                               wxWindowID id,
                               const wxPoint& pos,
                               const wxSize& size,
                               long style,
                               const wxString& name)
{
    DontCreatePeer();
    
    if (! wxMoTabCtrl::Create(parent, id, pos, size, style, name)) {
        return false;
    }
    
    SetPeer(wxWidgetImpl::CreateSegmentedCtrl( this, parent, id, pos, size, style, GetExtraStyle() ));
    
    MacPostControlCreate( pos, size );
    
    return true;
}

void wxMoSegmentedCtrl::Init()
{
    // FIXME stub
}

// wxEVT_COMMAND_TAB_SEL_CHANGED;
// wxEVT_COMMAND_TAB_SEL_CHANGING;

bool wxMoSegmentedCtrl::SendCommand(wxEventType eventType, int selection)
{
    // FIXME stub
    
    return true;
}

void wxMoSegmentedCtrl::OnPaint(wxPaintEvent& WXUNUSED(event))
{
    // FIXME stub
}

void wxMoSegmentedCtrl::OnInsertItem(wxMoBarButton* button)
{
    // FIXME stub
}

wxSize wxMoSegmentedCtrl::DoGetBestSize() const
{
    // FIXME stub

    wxSize empty(1, 1);
    return empty;
}

#pragma mark wxMoTabCtrl overrides

bool wxMoSegmentedCtrl::AddItem(const wxString& text, int imageId)
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
bool wxMoSegmentedCtrl::AddItem(const wxString& text, const wxBitmap& bitmap)
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
int wxMoSegmentedCtrl::SetSelection(int item)
{
    wxUISegmentedControl *segmentedControl = (wxUISegmentedControl *)(GetPeer()->GetWXWidget());
    if (! segmentedControl) {
        return -1;
    }
    
    NSInteger previousSelection = [segmentedControl selectedSegmentIndex];
    [segmentedControl setSelectedSegmentIndex:item];
    return previousSelection;
}
