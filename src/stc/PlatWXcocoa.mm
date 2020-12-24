/////////////////////////////////////////////////////////////////////////////
// Name:        src/stc/PlatWXcocoa.mm
// Purpose:     Implementation of utility functions for wxSTC with cocoa
// Author:      New Pagodi
// Created:     2019-03-10
// Copyright:   (c) 2019 wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_STC

#include "wx/osx/private.h"
#include "PlatWXcocoa.h"

// A simple view used for popup windows.

@interface wxSTCPopupBaseView : NSView
{
@private
    NSTrackingArea * m_trackingArea;
    wxWindow*        m_wxWin;
}

- (id)initWithwxWin:(wxWindow*) wxWin;

@end

@implementation wxSTCPopupBaseView

- (id)initWithwxWin:(wxWindow*) wxWin
{
    m_trackingArea = nil;

    self = [super init];
    if ( self )
        m_wxWin = wxWin;

    return self;
}

- (void)updateTrackingAreas
{
    if( m_trackingArea != nil )
    {
        [self removeTrackingArea:m_trackingArea];
        [m_trackingArea release];
    }

    int options = NSTrackingMouseEnteredAndExited | NSTrackingActiveAlways;
    m_trackingArea = [[NSTrackingArea alloc] initWithRect: [self bounds]
                                                  options: options
                                                    owner: self
                                                 userInfo: nil];
    [self addTrackingArea:m_trackingArea];
}

- (void)mouseEntered:(NSEvent *)evt
{
    wxUnusedVar(evt);
    wxMouseEvent wxevent(wxEVT_ENTER_WINDOW);
    wxevent.SetEventObject(m_wxWin);
    m_wxWin->ProcessWindowEvent(wxevent);
}

- (void)mouseExited:(NSEvent *)evt
{
    wxUnusedVar(evt);
    wxMouseEvent wxevent(wxEVT_LEAVE_WINDOW);
    wxevent.SetEventObject(m_wxWin);
    m_wxWin->ProcessWindowEvent(wxevent);
}

- (void)mouseDown:(NSEvent *)evt
{
    NSRect locationInWindow = NSZeroRect;
    locationInWindow.origin = [evt locationInWindow];
    NSPoint locationInView = [self convertPoint: locationInWindow.origin
                                       fromView: nil];
    wxPoint locationInViewWX = wxFromNSPoint(self, locationInView);

    wxMouseEvent wxevent(wxEVT_LEFT_DOWN);
    wxevent.SetEventObject(m_wxWin);
    wxevent.SetX(locationInViewWX.x);
    wxevent.SetY(locationInViewWX.y);
    m_wxWin->ProcessWindowEvent(wxevent);
}

- (void)drawRect:(NSRect)dirtyRect
{
    static_cast<wxWidgetCocoaImpl*>(m_wxWin->GetPeer())->
        drawRect(&dirtyRect, self, NULL);
}

@end


// Utility functions.

WX_NSWindow CreateFloatingWindow(wxWindow* wxWin)
{
    NSWindow* w = [[NSWindow alloc] initWithContentRect: NSZeroRect
                                              styleMask: NSBorderlessWindowMask
                                                backing: NSBackingStoreBuffered
                                                  defer: NO];
    [w setLevel:NSFloatingWindowLevel];
    [w setHasShadow:YES];
    [w setContentView:[[wxSTCPopupBaseView alloc] initWithwxWin:wxWin]];

    return w;
}

void CloseFloatingWindow(WX_NSWindow nsWin)
{
    [nsWin setReleasedWhenClosed:YES];
    [nsWin close];
}

void ShowFloatingWindow(WX_NSWindow nsWin)
{
    [nsWin orderFront:NSApp];
}

void HideFloatingWindow(WX_NSWindow nsWin)
{
    [nsWin orderOut:NSApp];
}

wxColour GetListHighlightColour()
{
    return wxColour([NSColor alternateSelectedControlColor]);
}

#endif // wxUSE_STC
