/////////////////////////////////////////////////////////////////////////////
// Name:        cocoa/control.mm
// Purpose:     wxControl class
// Author:      David Elliiott
// Modified by:
// Created:     2003/02/15
// RCS-ID:      $Id: 
// Copyright:   (c) 2003 David Elliott
// Licence:   	wxWidgets licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
    #include "wx/defs.h"
    #include "wx/control.h"
    #include "wx/log.h"
#endif

#include "wx/cocoa/autorelease.h"

#import <AppKit/NSControl.h>
#import <AppKit/NSCell.h>
#import <Foundation/NSException.h>

#include <math.h>

@interface wxNonControlNSControl : NSControl
{
}

- (void)drawRect: (NSRect)rect;
@end // wxNonControlNSControl

@implementation wxNonControlNSControl : NSControl
- (void)drawRect: (NSRect)rect
{
    wxCocoaNSView *win = wxCocoaNSView::GetFromCocoa(self);
    if( !win || !win->Cocoa_drawRect(rect) )
        [super drawRect:rect];
}
@end // wxNonControlNSControl

IMPLEMENT_ABSTRACT_CLASS(wxControl, wxWindow)
BEGIN_EVENT_TABLE(wxControl, wxControlBase)
END_EVENT_TABLE()
WX_IMPLEMENT_COCOA_OWNER(wxControl,NSControl,NSView,NSView)

bool wxControl::Create(wxWindow *parent, wxWindowID winid,
            const wxPoint& pos, const wxSize& size, long style,
            const wxValidator& validator, const wxString& name)
{
    wxLogTrace(wxTRACE_COCOA,wxT("Creating control with id=%d"),winid);
    if(!CreateControl(parent,winid,pos,size,style,validator,name))
        return false;
    wxLogTrace(wxTRACE_COCOA,wxT("Created control with id=%d"),GetId());
    m_cocoaNSView = NULL;
    SetNSControl([[wxNonControlNSControl alloc] initWithFrame: MakeDefaultNSRect(size)]);
    // NOTE: YES we want to release this (to match the alloc).
    // DoAddChild(this) will retain us again since addSubView doesn't.
    [m_cocoaNSView release];

    [GetNSControl() sizeToFit];

    if(m_parent)
        m_parent->CocoaAddChild(this);
    SetInitialFrameRect(pos,size);

    return true;
}

wxControl::~wxControl()
{
    DisassociateNSControl(GetNSControl());
}

wxSize wxControl::DoGetBestSize() const
{
    wxAutoNSAutoreleasePool pool;
    wxASSERT(GetNSControl());
    /* We can ask single-celled controls for their cell and get its size */
    NSCell *cell = nil;
NS_DURING
    cell = [GetNSControl() cell];
NS_HANDLER
    // TODO: if anything other than method not implemented, re-raise
NS_ENDHANDLER
    if(cell)
    {
        NSSize cellSize = [cell cellSize];
        wxSize size((int)ceilf(cellSize.width),(int)ceilf(cellSize.height));
        wxLogTrace(wxTRACE_COCOA_Window_Size,wxT("wxControl=%p::DoGetBestSize()==(%d,%d) from NSCell"),this,size.x,size.y);
        return size;
    }

    /* multi-celled control? size to fit, get the size, then set it back */
    NSRect storedRect = [m_cocoaNSView frame];
    bool didFit = false;
NS_DURING
    [GetNSControl() sizeToFit];
    didFit = true;
NS_HANDLER
    // TODO: if anything other than method not implemented, re-raise
NS_ENDHANDLER
    if(didFit)
    {
        NSRect cocoaRect = [m_cocoaNSView frame];
        wxSize size((int)ceilf(cocoaRect.size.width),(int)ceilf(cocoaRect.size.height));
        [m_cocoaNSView setFrame: storedRect];
        wxLogTrace(wxTRACE_COCOA_Window_Size,wxT("wxControl=%p::DoGetBestSize()==(%d,%d) from sizeToFit"),this,size.x,size.y);
        return size;
    }
    // Cocoa can't tell us the size, probably not an NSControl.
    wxLogDebug(wxT("Class %s (or superclass still below wxControl) should implement DoGetBestSize()"),GetClassInfo()->GetClassName());
    return wxControlBase::DoGetBestSize();
}

bool wxControl::ProcessCommand(wxCommandEvent& event)
{
    return GetEventHandler()->ProcessEvent(event);
}

void wxControl::CocoaSetEnabled(bool enable)
{
    [GetNSControl() setEnabled: enable];
}

