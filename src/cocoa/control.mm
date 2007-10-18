/////////////////////////////////////////////////////////////////////////////
// Name:        src/cocoa/control.mm
// Purpose:     wxControl class
// Author:      David Elliiott
// Modified by:
// Created:     2003/02/15
// RCS-ID:      $Id$
// Copyright:   (c) 2003 David Elliott
// Licence:     wxWidgets licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#include "wx/control.h"

#ifndef WX_PRECOMP
    #include "wx/log.h"
#endif

#include "wx/cocoa/string.h"
#include "wx/cocoa/autorelease.h"
#include "wx/cocoa/string.h"
#include "wx/cocoa/trackingrectmanager.h"
#include "wx/cocoa/objc/objc_uniquifying.h"

#import <AppKit/NSControl.h>
#import <AppKit/NSCell.h>
#import <Foundation/NSException.h>

#include <math.h>

@interface wxNonControlNSControl : NSControl
{
}

- (id)initWithFrame: (NSRect)frameRect;
- (void)drawRect: (NSRect)rect;
- (void)mouseDown:(NSEvent *)theEvent;
- (void)mouseDragged:(NSEvent *)theEvent;
- (void)mouseUp:(NSEvent *)theEvent;
- (void)mouseMoved:(NSEvent *)theEvent;
- (void)mouseEntered:(NSEvent *)theEvent;
- (void)mouseExited:(NSEvent *)theEvent;
- (void)rightMouseDown:(NSEvent *)theEvent;
- (void)rightMouseDragged:(NSEvent *)theEvent;
- (void)rightMouseUp:(NSEvent *)theEvent;
- (void)otherMouseDown:(NSEvent *)theEvent;
- (void)otherMouseDragged:(NSEvent *)theEvent;
- (void)otherMouseUp:(NSEvent *)theEvent;
- (void)resetCursorRects;
- (void)viewDidMoveToWindow;
- (void)viewWillMoveToWindow:(NSWindow *)newWindow;
@end // wxNonControlNSControl
WX_DECLARE_GET_OBJC_CLASS(wxNonControlNSControl,NSControl)

@implementation wxNonControlNSControl : NSControl

- (id)initWithFrame: (NSRect)frameRect
{
    if( (self = [super initWithFrame:frameRect]) == nil)
        return nil;
    // NSControl by default has no cell as it is semi-abstract.
    // Provide a normal NSCell for the sole purpose of making
    // setStringValue/stringValue work for labels in 2.8.
    NSCell *newCell = [[NSCell alloc] initTextCell:@""];
    [self setCell:newCell];
    [newCell release];
    return self;
}

- (BOOL)acceptsFirstMouse:(NSEvent *)theEvent
{
    bool acceptsFirstMouse = false;
    wxCocoaNSView *win = wxCocoaNSView::GetFromCocoa(self);
    if(!win || !win->Cocoa_acceptsFirstMouse(acceptsFirstMouse, theEvent))
        acceptsFirstMouse = [super acceptsFirstMouse:theEvent];
    return acceptsFirstMouse;
}

- (void)drawRect: (NSRect)rect
{
    wxCocoaNSView *win = wxCocoaNSView::GetFromCocoa(self);
    if( !win || !win->Cocoa_drawRect(rect) )
        [super drawRect:rect];
}

- (void)mouseDown:(NSEvent *)theEvent
{
    wxCocoaNSView *win = wxCocoaNSView::GetFromCocoa(self);
    if( !win || !win->Cocoa_mouseDown(theEvent) )
        [super mouseDown:theEvent];
}

- (void)mouseDragged:(NSEvent *)theEvent
{
    wxCocoaNSView *win = wxCocoaNSView::GetFromCocoa(self);
    if( !win || !win->Cocoa_mouseDragged(theEvent) )
        [super mouseDragged:theEvent];
}

- (void)mouseUp:(NSEvent *)theEvent
{
    wxCocoaNSView *win = wxCocoaNSView::GetFromCocoa(self);
    if( !win || !win->Cocoa_mouseUp(theEvent) )
        [super mouseUp:theEvent];
}

- (void)mouseMoved:(NSEvent *)theEvent
{
    wxCocoaNSView *win = wxCocoaNSView::GetFromCocoa(self);
    if( !win || !win->Cocoa_mouseMoved(theEvent) )
        [super mouseMoved:theEvent];
}

- (void)mouseEntered:(NSEvent *)theEvent
{
    wxCocoaNSView *win = wxCocoaNSView::GetFromCocoa(self);
    if( !win || !win->Cocoa_mouseEntered(theEvent) )
        [super mouseEntered:theEvent];
}

- (void)mouseExited:(NSEvent *)theEvent
{
    wxCocoaNSView *win = wxCocoaNSView::GetFromCocoa(self);
    if( !win || !win->Cocoa_mouseExited(theEvent) )
        [super mouseExited:theEvent];
}

- (void)rightMouseDown:(NSEvent *)theEvent
{
    wxCocoaNSView *win = wxCocoaNSView::GetFromCocoa(self);
    if( !win || !win->Cocoa_rightMouseDown(theEvent) )
        [super rightMouseDown:theEvent];
}

- (void)rightMouseDragged:(NSEvent *)theEvent
{
    wxCocoaNSView *win = wxCocoaNSView::GetFromCocoa(self);
    if( !win || !win->Cocoa_rightMouseDragged(theEvent) )
        [super rightMouseDragged:theEvent];
}

- (void)rightMouseUp:(NSEvent *)theEvent
{
    wxCocoaNSView *win = wxCocoaNSView::GetFromCocoa(self);
    if( !win || !win->Cocoa_rightMouseUp(theEvent) )
        [super rightMouseUp:theEvent];
}

- (void)otherMouseDown:(NSEvent *)theEvent
{
    wxCocoaNSView *win = wxCocoaNSView::GetFromCocoa(self);
    if( !win || !win->Cocoa_otherMouseDown(theEvent) )
        [super otherMouseDown:theEvent];
}

- (void)otherMouseDragged:(NSEvent *)theEvent
{
    wxCocoaNSView *win = wxCocoaNSView::GetFromCocoa(self);
    if( !win || !win->Cocoa_otherMouseDragged(theEvent) )
        [super otherMouseDragged:theEvent];
}

- (void)otherMouseUp:(NSEvent *)theEvent
{
    wxCocoaNSView *win = wxCocoaNSView::GetFromCocoa(self);
    if( !win || !win->Cocoa_otherMouseUp(theEvent) )
        [super otherMouseUp:theEvent];
}

- (void)resetCursorRects
{
    wxCocoaNSView *win = wxCocoaNSView::GetFromCocoa(self);
    if( !win || !win->Cocoa_resetCursorRects() )
        [super resetCursorRects];
}

- (void)viewDidMoveToWindow
{
#if wxUSE_ABI_INCOMPATIBLE_FEATURES
    wxCocoaNSView *win = wxCocoaNSView::GetFromCocoa(self);
    if( !win || !win->Cocoa_viewDidMoveToWindow() )
#endif
        [super viewDidMoveToWindow];
}

- (void)viewWillMoveToWindow:(NSWindow *)newWindow
{
#if wxUSE_ABI_INCOMPATIBLE_FEATURES
    wxCocoaNSView *win = wxCocoaNSView::GetFromCocoa(self);
    if( !win || !win->Cocoa_viewWillMoveToWindow(newWindow) )
#endif
        [super viewWillMoveToWindow:newWindow];
}

@end // wxNonControlNSControl
WX_IMPLEMENT_GET_OBJC_CLASS(wxNonControlNSControl,NSControl)

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
    SetNSControl([[WX_GET_OBJC_CLASS(wxNonControlNSControl) alloc] initWithFrame: MakeDefaultNSRect(size)]);
    // NOTE: YES we want to release this (to match the alloc).
    // DoAddChild(this) will retain us again since addSubView doesn't.
    [m_cocoaNSView release];

    if(m_parent)
        m_parent->CocoaAddChild(this);
    SetInitialFrameRect(pos,size);

#if wxUSE_ABI_INCOMPATIBLE_FEATURES
    // Controls should have a viewable-area tracking rect by default
    m_visibleTrackingRectManager = new wxCocoaTrackingRectManager(this);
#endif

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
    if([GetNSControl() respondsToSelector:@selector(cell)])
        cell = [GetNSControl() cell];
    if(cell)
    {
        NSSize cellSize = [cell cellSize];
        wxSize size((int)ceil(cellSize.width),(int)ceil(cellSize.height));
        wxLogTrace(wxTRACE_COCOA_Window_Size,wxT("wxControl=%p::DoGetBestSize()==(%d,%d) from NSCell"),this,size.x,size.y);
        return size;
    }

    /* multi-celled control? size to fit, get the size, then set it back */
    if([GetNSControl() respondsToSelector:@selector(sizeToFit)])
    {
        NSRect storedRect = [m_cocoaNSView frame];
        [GetNSControl() sizeToFit];
        NSRect cocoaRect = [m_cocoaNSView frame];
        wxSize size((int)ceil(cocoaRect.size.width),(int)ceil(cocoaRect.size.height));
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
    if([GetNSControl() respondsToSelector:@selector(setEnabled:)])
        [GetNSControl() setEnabled: enable];
}

/*static*/ void wxControl::CocoaSetLabelForObject(const wxString& label, struct objc_object *aView)
{
    [aView setTitle:wxNSStringWithWxString(GetLabelText(label))];
}

wxString wxControl::GetLabel() const
{
    if([GetNSControl() isKindOfClass:[WX_GET_OBJC_CLASS(wxNonControlNSControl) class]])
    {
        return wxStringWithNSString([GetNSControl() stringValue]);
    }
    else
        return wxControlBase::GetLabel();
}

void wxControl::SetLabel(const wxString& label)
{
    wxControlBase::SetLabel(label);
    // wx 2.8 hack: we need somewhere to stuff the label for
    // platform-independent subclasses of wxControl.
    if([GetNSControl() isKindOfClass:[WX_GET_OBJC_CLASS(wxNonControlNSControl) class]])
    {
        [GetNSControl() setStringValue:wxNSStringWithWxString(label)];
    }
}

