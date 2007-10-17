/////////////////////////////////////////////////////////////////////////////
// Name:        src/cocoa/window.mm
// Purpose:     wxWindowCocoa
// Author:      David Elliott
// Modified by:
// Created:     2002/12/26
// RCS-ID:      $Id$
// Copyright:   (c) 2002 David Elliott
// Licence:     wxWidgets licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
    #include "wx/log.h"
    #include "wx/window.h"
    #include "wx/dc.h"
    #include "wx/utils.h"
#endif //WX_PRECOMP

#include "wx/tooltip.h"

#include "wx/cocoa/autorelease.h"
#include "wx/cocoa/string.h"
#include "wx/cocoa/trackingrectmanager.h"
#include "wx/mac/corefoundation/cfref.h"

#import <Foundation/NSArray.h>
#import <Foundation/NSRunLoop.h>
#include "wx/cocoa/objc/NSView.h"
#import <AppKit/NSEvent.h>
#import <AppKit/NSScrollView.h>
#import <AppKit/NSColor.h>
#import <AppKit/NSClipView.h>
#import <Foundation/NSException.h>
#import <AppKit/NSApplication.h>
#import <AppKit/NSWindow.h>
#import <AppKit/NSScreen.h>

// Turn this on to paint green over the dummy views for debugging
#undef WXCOCOA_FILL_DUMMY_VIEW

#ifdef WXCOCOA_FILL_DUMMY_VIEW
#import <AppKit/NSBezierPath.h>
#endif //def WXCOCOA_FILL_DUMMY_VIEW

// STL list used by wxCocoaMouseMovedEventSynthesizer
#include <list>

/* NSComparisonResult is typedef'd as an enum pre-Leopard but typedef'd as
 * NSInteger post-Leopard.  Pre-Leopard the Cocoa toolkit expects a function
 * returning int and not NSComparisonResult.  Post-Leopard the Cocoa toolkit
 * expects a function returning the new non-enum NSComparsionResult.
 * Hence we create a typedef named CocoaWindowCompareFunctionResult.
 */
#if defined(NSINTEGER_DEFINED)
typedef NSComparisonResult CocoaWindowCompareFunctionResult;
#else
typedef int CocoaWindowCompareFunctionResult;
#endif

// A category for methods that are only present in Panther's SDK
@interface NSView(wxNSViewPrePantherCompatibility)
- (void)getRectsBeingDrawn:(const NSRect **)rects count:(int *)count;
@end

// ========================================================================
// Helper functions for converting to/from wxWidgets coordinates and a
// specified NSView's coordinate system.
// ========================================================================
NSPoint CocoaTransformNSViewBoundsToWx(NSView *nsview, NSPoint pointBounds)
{
    wxCHECK_MSG(nsview, pointBounds, wxT("Need to have a Cocoa view to do translation"));
    if([nsview isFlipped])
        return pointBounds;
    NSRect ourBounds = [nsview bounds];
    return NSMakePoint
    (   pointBounds.x
    ,   ourBounds.size.height - pointBounds.y
    );
}

NSRect CocoaTransformNSViewBoundsToWx(NSView *nsview, NSRect rectBounds)
{
    wxCHECK_MSG(nsview, rectBounds, wxT("Need to have a Cocoa view to do translation"));
    if([nsview isFlipped])
        return rectBounds;
    NSRect ourBounds = [nsview bounds];
    return NSMakeRect
    (   rectBounds.origin.x
    ,   ourBounds.size.height - (rectBounds.origin.y + rectBounds.size.height)
    ,   rectBounds.size.width
    ,   rectBounds.size.height
    );
}

NSPoint CocoaTransformNSViewWxToBounds(NSView *nsview, NSPoint pointWx)
{
    wxCHECK_MSG(nsview, pointWx, wxT("Need to have a Cocoa view to do translation"));
    if([nsview isFlipped])
        return pointWx;
    NSRect ourBounds = [nsview bounds];
    return NSMakePoint
    (   pointWx.x
    ,   ourBounds.size.height - pointWx.y
    );
}

NSRect CocoaTransformNSViewWxToBounds(NSView *nsview, NSRect rectWx)
{
    wxCHECK_MSG(nsview, rectWx, wxT("Need to have a Cocoa view to do translation"));
    if([nsview isFlipped])
        return rectWx;
    NSRect ourBounds = [nsview bounds];
    return NSMakeRect
    (   rectWx.origin.x
    ,   ourBounds.size.height - (rectWx.origin.y + rectWx.size.height)
    ,   rectWx.size.width
    ,   rectWx.size.height
    );
}

// ============================================================================
// Screen coordinate helpers
// ============================================================================

/*
General observation about Cocoa screen coordinates:
It is documented that the first object of the [NSScreen screens] array is the screen with the menubar.

It is not documented (but true as far as I can tell) that (0,0) in Cocoa screen coordinates is always
the BOTTOM-right corner of this screen.  Recall that Cocoa uses cartesian coordinates so y-increase is up.

It isn't clearly documented but visibleFrame returns a rectangle in screen coordinates, not a rectangle
relative to that screen's frame.  The only real way to test this is to configure two screens one atop
the other such that the menubar screen is on top.  The Dock at the bottom of the screen will then
eat into the visibleFrame of screen 1 by incrementing it's y-origin.  Thus if you arrange two
1920x1200 screens top/bottom then screen 1 (the bottom screen) will have frame origin (0,-1200) and
visibleFrame origin (0,-1149) which is exactly 51 pixels higher than the full frame origin.

In wxCocoa, we somewhat arbitrarily declare that wx (0,0) is the TOP-left of screen 0's frame (the entire screen).
However, this isn't entirely arbitrary because the Quartz Display Services (CGDisplay) uses this same scheme.
This works out nicely because wxCocoa's wxDisplay is implemented using Quartz Display Services instead of NSScreen.
*/

namespace { // file namespace

class wxCocoaPrivateScreenCoordinateTransformer
{
    DECLARE_NO_COPY_CLASS(wxCocoaPrivateScreenCoordinateTransformer)
public:
    wxCocoaPrivateScreenCoordinateTransformer();
    ~wxCocoaPrivateScreenCoordinateTransformer();
    wxPoint OriginInWxDisplayCoordinatesForRectInCocoaScreenCoordinates(NSRect windowFrame);
    NSPoint OriginInCocoaScreenCoordinatesForRectInWxDisplayCoordinates(wxCoord x, wxCoord y, wxCoord width, wxCoord height, bool keepOriginVisible);

protected:
    NSScreen *m_screenZero;
    NSRect m_screenZeroFrame;
};

// NOTE: This is intended to be a short-lived object.  A future enhancment might
// make it a global and reconfigure it upon some notification that the screen layout
// has changed.
inline wxCocoaPrivateScreenCoordinateTransformer::wxCocoaPrivateScreenCoordinateTransformer()
{
    NSArray *screens = [NSScreen screens];

    [screens retain];

    m_screenZero = nil;
    if(screens != nil && [screens count] > 0)
        m_screenZero = [[screens objectAtIndex:0] retain];

    [screens release];

    if(m_screenZero != nil)
        m_screenZeroFrame = [m_screenZero frame];
    else
    {
        wxLogWarning(wxT("Can't translate to/from wx screen coordinates and Cocoa screen coordinates"));
        // Just blindly assume 1024x768 so that at least we can sort of flip things around into
        // Cocoa coordinates.
        // NOTE: Theoretically this case should never happen anyway.
        m_screenZeroFrame = NSMakeRect(0,0,1024,768);
    }
}

inline wxCocoaPrivateScreenCoordinateTransformer::~wxCocoaPrivateScreenCoordinateTransformer()
{
    [m_screenZero release];
    m_screenZero = nil;
}

inline wxPoint wxCocoaPrivateScreenCoordinateTransformer::OriginInWxDisplayCoordinatesForRectInCocoaScreenCoordinates(NSRect windowFrame)
{
    // x and y are in wx screen coordinates which we're going to arbitrarily define such that
    // (0,0) is the TOP-left of screen 0 (the one with the menubar)
    // NOTE WELL: This means that (0,0) is _NOT_ an appropriate position for a window.

    wxPoint theWxOrigin;

    // Working in Cocoa's screen coordinates we must realize that the x coordinate we want is
    // the distance between the left side (origin.x) of the window's frame and the left side of
    // screen zero's frame.
    theWxOrigin.x = windowFrame.origin.x - m_screenZeroFrame.origin.x;

    // Working in Cocoa's screen coordinates we must realize that the y coordinate we want is
    // actually the distance between the top-left of the screen zero frame and the top-left
    // of the window's frame.

    theWxOrigin.y = (m_screenZeroFrame.origin.y + m_screenZeroFrame.size.height) - (windowFrame.origin.y + windowFrame.size.height);

    return theWxOrigin;
}

inline NSPoint wxCocoaPrivateScreenCoordinateTransformer::OriginInCocoaScreenCoordinatesForRectInWxDisplayCoordinates(wxCoord x, wxCoord y, wxCoord width, wxCoord height, bool keepOriginVisible)
{
    NSPoint theCocoaOrigin;

    // The position is in wx screen coordinates which we're going to arbitrarily define such that
    // (0,0) is the TOP-left of screen 0 (the one with the menubar)

    // NOTE: The usable rectangle is smaller and hence we have the keepOriginVisible flag
    // which will move the origin downward and/or left as necessary if the origin is
    // inside the screen0 rectangle (i.e. x/y >= 0 in wx coordinates) and outside the
    // visible frame (i.e. x/y < the top/left of the screen0 visible frame in wx coordinates)
    // We don't munge origin coordinates < 0 because it actually is possible that the menubar is on
    // the top of the bottom screen and thus that origin is completely valid!
    if(keepOriginVisible && (m_screenZero != nil))
    {
        // Do al of this in wx coordinates because it's far simpler since we're dealing with top/left points
        wxPoint visibleOrigin = OriginInWxDisplayCoordinatesForRectInCocoaScreenCoordinates([m_screenZero visibleFrame]);
        if(x >= 0 && x < visibleOrigin.x)
            x = visibleOrigin.x;
        if(y >= 0 && y < visibleOrigin.y)
            y = visibleOrigin.y;
    }

    // The x coordinate is simple as it's just relative to screen zero's frame
    theCocoaOrigin.x = m_screenZeroFrame.origin.x + x;
    // Working in Cocoa's coordinates think to start at the bottom of screen zero's frame and add
    // the height of that rect which gives us the coordinate for the top of the visible rect.  Now realize that
    // the wx coordinates are flipped so if y is say 10 then we want to be 10 pixels down from that and thus
    // we subtract y.  But then we still need to take into account the size of the window which is h and subtract
    // that to get the bottom-left origin of the rectangle.
    theCocoaOrigin.y = m_screenZeroFrame.origin.y + m_screenZeroFrame.size.height - y - height;

    return theCocoaOrigin;
}

} // namespace

wxPoint wxWindowCocoa::OriginInWxDisplayCoordinatesForRectInCocoaScreenCoordinates(NSRect windowFrame)
{
    wxCocoaPrivateScreenCoordinateTransformer transformer;
    return transformer.OriginInWxDisplayCoordinatesForRectInCocoaScreenCoordinates(windowFrame);
}

NSPoint wxWindowCocoa::OriginInCocoaScreenCoordinatesForRectInWxDisplayCoordinates(wxCoord x, wxCoord y, wxCoord width, wxCoord height, bool keepOriginVisible)
{
    wxCocoaPrivateScreenCoordinateTransformer transformer;
    return transformer.OriginInCocoaScreenCoordinatesForRectInWxDisplayCoordinates(x,y,width,height,keepOriginVisible);
}

// ========================================================================
// wxWindowCocoaHider
// ========================================================================
class wxWindowCocoaHider: protected wxCocoaNSView
{
    DECLARE_NO_COPY_CLASS(wxWindowCocoaHider)
public:
    wxWindowCocoaHider(wxWindow *owner);
    virtual ~wxWindowCocoaHider();
    inline WX_NSView GetNSView() { return m_dummyNSView; }
protected:
    wxWindowCocoa *m_owner;
    WX_NSView m_dummyNSView;
    virtual void Cocoa_FrameChanged(void);
#if wxUSE_ABI_INCOMPATIBLE_FEATURES
    virtual void Cocoa_synthesizeMouseMoved(void) {}
#endif
#ifdef WXCOCOA_FILL_DUMMY_VIEW
    virtual bool Cocoa_drawRect(const NSRect& rect);
#endif //def WXCOCOA_FILL_DUMMY_VIEW
private:
    wxWindowCocoaHider();
};

// ========================================================================
// wxWindowCocoaScrollView
// ========================================================================
class wxWindowCocoaScrollView: protected wxCocoaNSView
{
    DECLARE_NO_COPY_CLASS(wxWindowCocoaScrollView)
public:
    wxWindowCocoaScrollView(wxWindow *owner);
    virtual ~wxWindowCocoaScrollView();
    inline WX_NSScrollView GetNSScrollView() { return m_cocoaNSScrollView; }
    void ClientSizeToSize(int &width, int &height);
    void DoGetClientSize(int *x, int *y) const;
    void Encapsulate();
    void Unencapsulate();
protected:
    wxWindowCocoa *m_owner;
    WX_NSScrollView m_cocoaNSScrollView;
    virtual void Cocoa_FrameChanged(void);
#if wxUSE_ABI_INCOMPATIBLE_FEATURES
    virtual void Cocoa_synthesizeMouseMoved(void) {}
#endif
private:
    wxWindowCocoaScrollView();
};

// ========================================================================
// wxDummyNSView
// ========================================================================
@interface wxDummyNSView : NSView
- (NSView *)hitTest:(NSPoint)aPoint;
@end
WX_DECLARE_GET_OBJC_CLASS(wxDummyNSView,NSView)

@implementation wxDummyNSView : NSView
- (NSView *)hitTest:(NSPoint)aPoint
{
    return nil;
}

@end
WX_IMPLEMENT_GET_OBJC_CLASS(wxDummyNSView,NSView)

// ========================================================================
// wxWindowCocoaHider
// ========================================================================
wxWindowCocoaHider::wxWindowCocoaHider(wxWindow *owner)
:   m_owner(owner)
{
    wxASSERT(owner);
    wxASSERT(owner->GetNSViewForHiding());
    m_dummyNSView = [[WX_GET_OBJC_CLASS(wxDummyNSView) alloc]
        initWithFrame:[owner->GetNSViewForHiding() frame]];
    [m_dummyNSView setAutoresizingMask: [owner->GetNSViewForHiding() autoresizingMask]];
    AssociateNSView(m_dummyNSView);
}

wxWindowCocoaHider::~wxWindowCocoaHider()
{
    DisassociateNSView(m_dummyNSView);
    [m_dummyNSView release];
}

void wxWindowCocoaHider::Cocoa_FrameChanged(void)
{
    // Keep the real window in synch with the dummy
    wxASSERT(m_dummyNSView);
    [m_owner->GetNSViewForHiding() setFrame:[m_dummyNSView frame]];
}


#ifdef WXCOCOA_FILL_DUMMY_VIEW
bool wxWindowCocoaHider::Cocoa_drawRect(const NSRect& rect)
{
    NSBezierPath *bezpath = [NSBezierPath bezierPathWithRect:rect];
    [[NSColor greenColor] set];
    [bezpath stroke];
    [bezpath fill];
    return true;
}
#endif //def WXCOCOA_FILL_DUMMY_VIEW

// ========================================================================
// wxFlippedNSClipView
// ========================================================================
@interface wxFlippedNSClipView : NSClipView
- (BOOL)isFlipped;
@end
WX_DECLARE_GET_OBJC_CLASS(wxFlippedNSClipView,NSClipView)

@implementation wxFlippedNSClipView : NSClipView
- (BOOL)isFlipped
{
    return YES;
}

@end
WX_IMPLEMENT_GET_OBJC_CLASS(wxFlippedNSClipView,NSClipView)

// ========================================================================
// wxWindowCocoaScrollView
// ========================================================================
wxWindowCocoaScrollView::wxWindowCocoaScrollView(wxWindow *owner)
:   m_owner(owner)
{
    wxAutoNSAutoreleasePool pool;
    wxASSERT(owner);
    wxASSERT(owner->GetNSView());
    m_cocoaNSScrollView = [[NSScrollView alloc]
        initWithFrame:[owner->GetNSView() frame]];
    AssociateNSView(m_cocoaNSScrollView);

    /* Replace the default NSClipView with a flipped one.  This ensures
       scrolling is "pinned" to the top-left instead of bottom-right. */
    NSClipView *flippedClip = [[WX_GET_OBJC_CLASS(wxFlippedNSClipView) alloc]
        initWithFrame: [[m_cocoaNSScrollView contentView] frame]];
    [m_cocoaNSScrollView setContentView:flippedClip];
    [flippedClip release];

    [m_cocoaNSScrollView setBackgroundColor: [NSColor windowBackgroundColor]];
    [m_cocoaNSScrollView setHasHorizontalScroller: YES];
    [m_cocoaNSScrollView setHasVerticalScroller: YES];
    Encapsulate();
}

void wxWindowCocoaScrollView::Encapsulate()
{
    // Set the scroll view autoresizingMask to match the current NSView
    [m_cocoaNSScrollView setAutoresizingMask: [m_owner->GetNSView() autoresizingMask]];
    [m_owner->GetNSView() setAutoresizingMask: NSViewNotSizable];
    // NOTE: replaceSubView will cause m_cocaNSView to be released
    // except when it hasn't been added into an NSView hierarchy in which
    // case it doesn't need to be and this should work out to a no-op
    m_owner->CocoaReplaceView(m_owner->GetNSView(), m_cocoaNSScrollView);
    // The NSView is still retained by owner
    [m_cocoaNSScrollView setDocumentView: m_owner->GetNSView()];
    // Now it's also retained by the NSScrollView
}

void wxWindowCocoaScrollView::Unencapsulate()
{
    [m_cocoaNSScrollView setDocumentView: nil];
    m_owner->CocoaReplaceView(m_cocoaNSScrollView, m_owner->GetNSView());
    if(![[m_owner->GetNSView() superview] isFlipped])
        [m_owner->GetNSView() setAutoresizingMask: NSViewMinYMargin];
}

wxWindowCocoaScrollView::~wxWindowCocoaScrollView()
{
    DisassociateNSView(m_cocoaNSScrollView);
    [m_cocoaNSScrollView release];
}

void wxWindowCocoaScrollView::ClientSizeToSize(int &width, int &height)
{
    NSSize frameSize = [NSScrollView
        frameSizeForContentSize: NSMakeSize(width,height)
        hasHorizontalScroller: [m_cocoaNSScrollView hasHorizontalScroller]
        hasVerticalScroller: [m_cocoaNSScrollView hasVerticalScroller]
        borderType: [m_cocoaNSScrollView borderType]];
    width = (int)frameSize.width;
    height = (int)frameSize.height;
}

void wxWindowCocoaScrollView::DoGetClientSize(int *x, int *y) const
{
    NSSize nssize = [m_cocoaNSScrollView contentSize];
    if(x)
        *x = (int)nssize.width;
    if(y)
        *y = (int)nssize.height;
}

void wxWindowCocoaScrollView::Cocoa_FrameChanged(void)
{
    wxLogTrace(wxTRACE_COCOA,wxT("Cocoa_FrameChanged"));
    wxSizeEvent event(m_owner->GetSize(), m_owner->GetId());
    event.SetEventObject(m_owner);
    m_owner->GetEventHandler()->ProcessEvent(event);
}

// ========================================================================
// wxWindowCocoa
// ========================================================================
// normally the base classes aren't included, but wxWindow is special
#ifdef __WXUNIVERSAL__
IMPLEMENT_ABSTRACT_CLASS(wxWindowCocoa, wxWindowBase)
#else
IMPLEMENT_DYNAMIC_CLASS(wxWindow, wxWindowBase)
#endif

BEGIN_EVENT_TABLE(wxWindowCocoa, wxWindowBase)
END_EVENT_TABLE()

wxWindow *wxWindowCocoa::sm_capturedWindow = NULL;

// Constructor
void wxWindowCocoa::Init()
{
    m_cocoaNSView = NULL;
    m_cocoaHider = NULL;
    m_wxCocoaScrollView = NULL;
    m_isBeingDeleted = false;
    m_isInPaint = false;
    m_shouldBeEnabled = true;
#if wxUSE_ABI_INCOMPATIBLE_FEATURES
    m_visibleTrackingRectManager = NULL;
#endif
}

// Constructor
bool wxWindow::Create(wxWindow *parent, wxWindowID winid,
           const wxPoint& pos,
           const wxSize& size,
           long style,
           const wxString& name)
{
    if(!CreateBase(parent,winid,pos,size,style,wxDefaultValidator,name))
        return false;

    // TODO: create the window
    m_cocoaNSView = NULL;
    SetNSView([[WX_GET_OBJC_CLASS(WXNSView) alloc] initWithFrame: MakeDefaultNSRect(size)]);
    [m_cocoaNSView release];

    if (m_parent)
    {
        m_parent->AddChild(this);
        m_parent->CocoaAddChild(this);
        SetInitialFrameRect(pos,size);
    }

    return true;
}

// Destructor
wxWindow::~wxWindow()
{
    wxAutoNSAutoreleasePool pool;
    DestroyChildren();

    // Make sure our parent (in the wxWidgets sense) is our superview
    // before we go removing from it.
    if(m_parent && m_parent->GetNSView()==[GetNSViewForSuperview() superview])
        CocoaRemoveFromParent();
    delete m_cocoaHider;
    delete m_wxCocoaScrollView;
    if(m_cocoaNSView)
        SendDestroyEvent();
    SetNSView(NULL);
}

void wxWindowCocoa::CocoaAddChild(wxWindowCocoa *child)
{
    // Pool here due to lack of one during wx init phase
    wxAutoNSAutoreleasePool pool;

    NSView *childView = child->GetNSViewForSuperview();

    wxASSERT(childView);
    [m_cocoaNSView addSubview: childView];
}

void wxWindowCocoa::CocoaRemoveFromParent(void)
{
    [GetNSViewForSuperview() removeFromSuperview];
}

void wxWindowCocoa::SetNSView(WX_NSView cocoaNSView)
{
#if wxUSE_ABI_INCOMPATIBLE_FEATURES
    // Clear the visible area tracking rect if we have one.
    delete m_visibleTrackingRectManager;
    m_visibleTrackingRectManager = NULL;
#endif

    bool need_debug = cocoaNSView || m_cocoaNSView;
    if(need_debug) wxLogTrace(wxTRACE_COCOA_RetainRelease,wxT("wxWindowCocoa=%p::SetNSView [m_cocoaNSView=%p retainCount]=%d"),this,m_cocoaNSView,[m_cocoaNSView retainCount]);
    DisassociateNSView(m_cocoaNSView);
    [cocoaNSView retain];
    [m_cocoaNSView release];
    m_cocoaNSView = cocoaNSView;
    AssociateNSView(m_cocoaNSView);
    if(need_debug) wxLogTrace(wxTRACE_COCOA_RetainRelease,wxT("wxWindowCocoa=%p::SetNSView [cocoaNSView=%p retainCount]=%d"),this,cocoaNSView,[cocoaNSView retainCount]);
}

WX_NSView wxWindowCocoa::GetNSViewForSuperview() const
{
    return m_cocoaHider
        ?   m_cocoaHider->GetNSView()
        :   m_wxCocoaScrollView
            ?   m_wxCocoaScrollView->GetNSScrollView()
            :   m_cocoaNSView;
}

WX_NSView wxWindowCocoa::GetNSViewForHiding() const
{
    return m_wxCocoaScrollView
        ?   m_wxCocoaScrollView->GetNSScrollView()
        :   m_cocoaNSView;
}

NSPoint wxWindowCocoa::CocoaTransformBoundsToWx(NSPoint pointBounds)
{
    // TODO: Handle scrolling offset
    return CocoaTransformNSViewBoundsToWx(GetNSView(), pointBounds);
}

NSRect wxWindowCocoa::CocoaTransformBoundsToWx(NSRect rectBounds)
{
    // TODO: Handle scrolling offset
    return CocoaTransformNSViewBoundsToWx(GetNSView(), rectBounds);
}

NSPoint wxWindowCocoa::CocoaTransformWxToBounds(NSPoint pointWx)
{
    // TODO: Handle scrolling offset
    return CocoaTransformNSViewWxToBounds(GetNSView(), pointWx);
}

NSRect wxWindowCocoa::CocoaTransformWxToBounds(NSRect rectWx)
{
    // TODO: Handle scrolling offset
    return CocoaTransformNSViewWxToBounds(GetNSView(), rectWx);
}

WX_NSAffineTransform wxWindowCocoa::CocoaGetWxToBoundsTransform()
{
    // TODO: Handle scrolling offset
    NSAffineTransform *transform = wxDC::CocoaGetWxToBoundsTransform([GetNSView() isFlipped], [GetNSView() bounds].size.height);
    return transform;
}

bool wxWindowCocoa::Cocoa_drawRect(const NSRect &rect)
{
    wxLogTrace(wxTRACE_COCOA,wxT("Cocoa_drawRect"));
    // Recursion can happen if the event loop runs from within the paint
    // handler.  For instance, if an assertion dialog is shown.
    // FIXME: This seems less than ideal.
    if(m_isInPaint)
    {
        wxLogDebug(wxT("Paint event recursion!"));
        return false;
    }
    m_isInPaint = true;

    // Set m_updateRegion
    const NSRect *rects = &rect; // The bounding box of the region
    NSInteger countRects = 1;
    // Try replacing the larger rectangle with a list of smaller ones:
    if ([GetNSView() respondsToSelector:@selector(getRectsBeingDrawn:count:)])
        [GetNSView() getRectsBeingDrawn:&rects count:&countRects];

    NSRect *transformedRects = (NSRect*)malloc(sizeof(NSRect)*countRects);
    for(int i=0; i<countRects; i++)
    {
        transformedRects[i] = CocoaTransformBoundsToWx(rects[i]);
    }
    m_updateRegion = wxRegion(transformedRects,countRects);
    free(transformedRects);

    wxPaintEvent event(m_windowId);
    event.SetEventObject(this);
    bool ret = GetEventHandler()->ProcessEvent(event);
    m_isInPaint = false;
    return ret;
}

void wxWindowCocoa::InitMouseEvent(wxMouseEvent& event, WX_NSEvent cocoaEvent)
{
    wxASSERT_MSG([m_cocoaNSView window]==[cocoaEvent window],wxT("Mouse event for different NSWindow"));
    // Mouse events happen at the NSWindow level so we need to convert
    // into our bounds coordinates then convert to wx coordinates.
    NSPoint cocoaPoint = [m_cocoaNSView convertPoint:[(NSEvent*)cocoaEvent locationInWindow] fromView:nil];
    NSPoint pointWx = CocoaTransformBoundsToWx(cocoaPoint);
    // FIXME: Should we be adjusting for client area origin?
    const wxPoint &clientorigin = GetClientAreaOrigin();
    event.m_x = (wxCoord)pointWx.x - clientorigin.x;
    event.m_y = (wxCoord)pointWx.y - clientorigin.y;

    event.m_shiftDown = [cocoaEvent modifierFlags] & NSShiftKeyMask;
    event.m_controlDown = [cocoaEvent modifierFlags] & NSControlKeyMask;
    event.m_altDown = [cocoaEvent modifierFlags] & NSAlternateKeyMask;
    event.m_metaDown = [cocoaEvent modifierFlags] & NSCommandKeyMask;

    // TODO: set timestamp?
    event.SetEventObject(this);
    event.SetId(GetId());
}

bool wxWindowCocoa::Cocoa_mouseMoved(WX_NSEvent theEvent)
{
    wxMouseEvent event(wxEVT_MOTION);
    InitMouseEvent(event,theEvent);
    wxLogTrace(wxTRACE_COCOA,wxT("wxWindow=%p::Cocoa_mouseMoved @%d,%d"),this,event.m_x,event.m_y);
    return GetEventHandler()->ProcessEvent(event);
}

void wxWindowCocoa::Cocoa_synthesizeMouseMoved()
{
    wxMouseEvent event(wxEVT_MOTION);
    NSWindow *window = [GetNSView() window];
    NSPoint locationInWindow = [window mouseLocationOutsideOfEventStream];
    NSPoint cocoaPoint = [m_cocoaNSView convertPoint:locationInWindow fromView:nil];

    NSPoint pointWx = CocoaTransformBoundsToWx(cocoaPoint);
    // FIXME: Should we be adjusting for client area origin?
    const wxPoint &clientorigin = GetClientAreaOrigin();
    event.m_x = (wxCoord)pointWx.x - clientorigin.x;
    event.m_y = (wxCoord)pointWx.y - clientorigin.y;

    // TODO: Handle shift, control, alt, meta flags
    event.SetEventObject(this);
    event.SetId(GetId());

    wxLogTrace(wxTRACE_COCOA,wxT("wxwin=%p Synthesized Mouse Moved @%d,%d"),this,event.m_x,event.m_y);
    GetEventHandler()->ProcessEvent(event);
}

bool wxWindowCocoa::Cocoa_mouseEntered(WX_NSEvent theEvent)
{
#if wxUSE_ABI_INCOMPATIBLE_FEATURES
    if(m_visibleTrackingRectManager != NULL && m_visibleTrackingRectManager->IsOwnerOfEvent(theEvent))
    {
        m_visibleTrackingRectManager->BeginSynthesizingEvents();

        // Although we synthesize the mouse moved events we don't poll for them but rather send them only when
        // some other event comes in.  That other event is (guess what) mouse moved events that will be sent
        // to the NSWindow which will forward them on to the first responder.  We are not likely to be the
        // first responder, so the mouseMoved: events are effectively discarded.
        [[GetNSView() window] setAcceptsMouseMovedEvents:YES];

        wxMouseEvent event(wxEVT_ENTER_WINDOW);
        InitMouseEvent(event,theEvent);
        wxLogTrace(wxTRACE_COCOA_TrackingRect,wxT("wxwin=%p Mouse Entered TR#%d @%d,%d"),this,[theEvent trackingNumber], event.m_x,event.m_y);
        return GetEventHandler()->ProcessEvent(event);
    }
    else
#endif
        return false;
}

bool wxWindowCocoa::Cocoa_mouseExited(WX_NSEvent theEvent)
{
#if wxUSE_ABI_INCOMPATIBLE_FEATURES
    if(m_visibleTrackingRectManager != NULL && m_visibleTrackingRectManager->IsOwnerOfEvent(theEvent))
    {
        m_visibleTrackingRectManager->StopSynthesizingEvents();

        wxMouseEvent event(wxEVT_LEAVE_WINDOW);
        InitMouseEvent(event,theEvent);
        wxLogTrace(wxTRACE_COCOA_TrackingRect,wxT("wxwin=%p Mouse Exited TR#%d @%d,%d"),this,[theEvent trackingNumber],event.m_x,event.m_y);
        return GetEventHandler()->ProcessEvent(event);
    }
    else
#endif
        return false;
}

bool wxWindowCocoa::Cocoa_mouseDown(WX_NSEvent theEvent)
{
    wxMouseEvent event([theEvent clickCount]<2?wxEVT_LEFT_DOWN:wxEVT_LEFT_DCLICK);
    InitMouseEvent(event,theEvent);
    wxLogTrace(wxTRACE_COCOA,wxT("Mouse Down @%d,%d num clicks=%d"),event.m_x,event.m_y,[theEvent clickCount]);
    return GetEventHandler()->ProcessEvent(event);
}

bool wxWindowCocoa::Cocoa_mouseDragged(WX_NSEvent theEvent)
{
    wxMouseEvent event(wxEVT_MOTION);
    InitMouseEvent(event,theEvent);
    event.m_leftDown = true;
    wxLogTrace(wxTRACE_COCOA,wxT("Mouse Drag @%d,%d"),event.m_x,event.m_y);
    return GetEventHandler()->ProcessEvent(event);
}

bool wxWindowCocoa::Cocoa_mouseUp(WX_NSEvent theEvent)
{
    wxMouseEvent event(wxEVT_LEFT_UP);
    InitMouseEvent(event,theEvent);
    wxLogTrace(wxTRACE_COCOA,wxT("Mouse Up @%d,%d"),event.m_x,event.m_y);
    return GetEventHandler()->ProcessEvent(event);
}

bool wxWindowCocoa::Cocoa_rightMouseDown(WX_NSEvent theEvent)
{
    wxMouseEvent event([theEvent clickCount]<2?wxEVT_RIGHT_DOWN:wxEVT_RIGHT_DCLICK);
    InitMouseEvent(event,theEvent);
    wxLogDebug(wxT("Mouse Down @%d,%d num clicks=%d"),event.m_x,event.m_y,[theEvent clickCount]);
    return GetEventHandler()->ProcessEvent(event);
}

bool wxWindowCocoa::Cocoa_rightMouseDragged(WX_NSEvent theEvent)
{
    wxMouseEvent event(wxEVT_MOTION);
    InitMouseEvent(event,theEvent);
    event.m_rightDown = true;
    wxLogDebug(wxT("Mouse Drag @%d,%d"),event.m_x,event.m_y);
    return GetEventHandler()->ProcessEvent(event);
}

bool wxWindowCocoa::Cocoa_rightMouseUp(WX_NSEvent theEvent)
{
    wxMouseEvent event(wxEVT_RIGHT_UP);
    InitMouseEvent(event,theEvent);
    wxLogDebug(wxT("Mouse Up @%d,%d"),event.m_x,event.m_y);
    return GetEventHandler()->ProcessEvent(event);
}

bool wxWindowCocoa::Cocoa_otherMouseDown(WX_NSEvent theEvent)
{
    return false;
}

bool wxWindowCocoa::Cocoa_otherMouseDragged(WX_NSEvent theEvent)
{
    return false;
}

bool wxWindowCocoa::Cocoa_otherMouseUp(WX_NSEvent theEvent)
{
    return false;
}

void wxWindowCocoa::Cocoa_FrameChanged(void)
{
    wxLogTrace(wxTRACE_COCOA,wxT("wxWindow=%p::Cocoa_FrameChanged"),this);
#if wxUSE_ABI_INCOMPATIBLE_FEATURES
    if(m_visibleTrackingRectManager != NULL)
        m_visibleTrackingRectManager->RebuildTrackingRect();
#endif
    wxSizeEvent event(GetSize(), m_windowId);
    event.SetEventObject(this);
    GetEventHandler()->ProcessEvent(event);
}

bool wxWindowCocoa::SetCursor(const wxCursor &cursor)
{
    if(!wxWindowBase::SetCursor(cursor))
        return false;

    // Set up the cursor rect so that invalidateCursorRectsForView: will destroy it.
    // If we don't do this then Cocoa thinks (rightly) that we don't have any cursor
    // rects and thus won't ever call resetCursorRects.
    [GetNSView() addCursorRect: [GetNSView() visibleRect]  cursor: m_cursor.GetNSCursor()];

    // Invalidate the cursor rects so the cursor will change
    // Note that it is not enough to remove the old one (if any) and add the new one.
    // For the rects to work properly, Cocoa itself must call resetCursorRects.
    [[GetNSView() window] invalidateCursorRectsForView:GetNSView()];
    return true;
}

bool wxWindowCocoa::Cocoa_resetCursorRects()
{
    wxLogTrace(wxTRACE_COCOA,wxT("wxWindow=%p::Cocoa_resetCursorRects"),this);
#if wxUSE_ABI_INCOMPATIBLE_FEATURES

    // When we are called there may be a queued tracking rect event (mouse entered or exited) and
    // we won't know it.  A specific example is wxGenericHyperlinkCtrl changing the cursor from its
    // mouse exited event.  If the control happens to share the edge with its parent window which is
    // also tracking mouse events then Cocoa receives two mouse exited events from the window server.
    // The first one will cause wxGenericHyperlinkCtrl to call wxWindow::SetCursor which will
    // invaildate the cursor rect causing Cocoa to schedule cursor rect reset with the run loop
    // which willl in turn call us before exiting for the next user event.

    // If we are the parent window then rebuilding our tracking rectangle will cause us to miss
    // our mouse exited event because the already queued event will have the old tracking rect
    // tag.  The simple solution is to only rebuild our tracking rect if we need to.

    if(m_visibleTrackingRectManager != NULL)
        m_visibleTrackingRectManager->RebuildTrackingRectIfNeeded();
#endif

    if(!m_cursor.GetNSCursor())
        return false;

    [GetNSView() addCursorRect: [GetNSView() visibleRect]  cursor: m_cursor.GetNSCursor()];

    return true;
}

#if wxUSE_ABI_INCOMPATIBLE_FEATURES
bool wxWindowCocoa::Cocoa_viewDidMoveToWindow()
{
    wxLogTrace(wxTRACE_COCOA,wxT("wxWindow=%p::viewDidMoveToWindow"),this);
    // Set up new tracking rects.  I am reasonably sure the new window must be set before doing this.
    if(m_visibleTrackingRectManager != NULL)
        m_visibleTrackingRectManager->BuildTrackingRect();
    return false;
}

bool wxWindowCocoa::Cocoa_viewWillMoveToWindow(WX_NSWindow newWindow)
{
    wxLogTrace(wxTRACE_COCOA,wxT("wxWindow=%p::viewWillMoveToWindow:%p"),this, newWindow);
    // Clear tracking rects.  It is imperative this be done before the new window is set.
    if(m_visibleTrackingRectManager != NULL)
        m_visibleTrackingRectManager->ClearTrackingRect();
    return false;
}
#endif

bool wxWindow::Close(bool force)
{
    // The only reason this function exists is that it is virtual and
    // wxTopLevelWindowCocoa will override it.
    return wxWindowBase::Close(force);
}

void wxWindow::CocoaReplaceView(WX_NSView oldView, WX_NSView newView)
{
    [[oldView superview] replaceSubview:oldView with:newView];
}

bool wxWindow::EnableSelfAndChildren(bool enable)
{
    // If the state isn't changing, don't do anything
    if(!wxWindowBase::Enable(enable && m_shouldBeEnabled))
        return false;
    // Set the state of the Cocoa window
    CocoaSetEnabled(m_isEnabled);
    // Disable all children or (if enabling) return them to their proper state
    for(wxWindowList::compatibility_iterator node = GetChildren().GetFirst();
        node; node = node->GetNext())
    {
        node->GetData()->EnableSelfAndChildren(enable);
    }
    return true;
}

bool wxWindow::Enable(bool enable)
{
    // Keep track of what the window SHOULD be doing
    m_shouldBeEnabled = enable;
    // If the parent is disabled for any reason, then this window will be too.
    if(!IsTopLevel() && GetParent())
    {
        enable = enable && GetParent()->IsEnabled();
    }
    return EnableSelfAndChildren(enable);
}

bool wxWindow::Show(bool show)
{
    wxAutoNSAutoreleasePool pool;
    // If the window is marked as visible, then it shouldn't have a dummy view
    // If the window is marked hidden, then it should have a dummy view
    // wxSpinCtrl (generic) abuses m_isShown, don't use it for any logic
//    wxASSERT_MSG( (m_isShown && !m_dummyNSView) || (!m_isShown && m_dummyNSView),wxT("wxWindow: m_isShown does not agree with m_dummyNSView"));
    // Return false if there isn't a window to show or hide
    NSView *cocoaView = GetNSViewForHiding();
    if(!cocoaView)
        return false;
    if(show)
    {
        // If state isn't changing, return false
        if(!m_cocoaHider)
            return false;
        CocoaReplaceView(m_cocoaHider->GetNSView(), cocoaView);
        wxASSERT(![m_cocoaHider->GetNSView() superview]);
        delete m_cocoaHider;
        m_cocoaHider = NULL;
        wxASSERT([cocoaView superview]);
    }
    else
    {
        // If state isn't changing, return false
        if(m_cocoaHider)
            return false;
        m_cocoaHider = new wxWindowCocoaHider(this);
        // NOTE: replaceSubview:with will cause m_cocaNSView to be
        // (auto)released which balances out addSubview
        CocoaReplaceView(cocoaView, m_cocoaHider->GetNSView());
        // m_coocaNSView is now only retained by us
        wxASSERT([m_cocoaHider->GetNSView() superview]);
        wxASSERT(![cocoaView superview]);
    }
    m_isShown = show;
    return true;
}

void wxWindowCocoa::DoSetSize(int x, int y, int width, int height, int sizeFlags)
{
    wxLogTrace(wxTRACE_COCOA_Window_Size,wxT("wxWindow=%p::DoSetSizeWindow(%d,%d,%d,%d,Auto: %s%s)"),this,x,y,width,height,(sizeFlags&wxSIZE_AUTO_WIDTH)?"W":".",sizeFlags&wxSIZE_AUTO_HEIGHT?"H":".");
    int currentX, currentY;
    int currentW, currentH;
    DoGetPosition(&currentX, &currentY);
    DoGetSize(&currentW, &currentH);
    if((x==-1) && !(sizeFlags&wxSIZE_ALLOW_MINUS_ONE))
        x=currentX;
    if((y==-1) && !(sizeFlags&wxSIZE_ALLOW_MINUS_ONE))
        y=currentY;

    AdjustForParentClientOrigin(x,y,sizeFlags);

    wxSize size(wxDefaultSize);

    if((width==-1)&&!(sizeFlags&wxSIZE_ALLOW_MINUS_ONE))
    {
        if(sizeFlags&wxSIZE_AUTO_WIDTH)
        {
            size=DoGetBestSize();
            width=size.x;
        }
        else
            width=currentW;
    }
    if((height==-1)&&!(sizeFlags&wxSIZE_ALLOW_MINUS_ONE))
    {
        if(sizeFlags&wxSIZE_AUTO_HEIGHT)
        {
            if(size.x==-1)
                size=DoGetBestSize();
            height=size.y;
        }
        else
            height=currentH;
    }
    DoMoveWindow(x,y,width,height);
}

#if wxUSE_TOOLTIPS

void wxWindowCocoa::DoSetToolTip( wxToolTip *tip )
{
    wxWindowBase::DoSetToolTip(tip);

    if ( m_tooltip )
    {
        m_tooltip->SetWindow((wxWindow *)this);
    }
}

#endif

void wxWindowCocoa::DoMoveWindow(int x, int y, int width, int height)
{
    wxAutoNSAutoreleasePool pool;
    wxLogTrace(wxTRACE_COCOA_Window_Size,wxT("wxWindow=%p::DoMoveWindow(%d,%d,%d,%d)"),this,x,y,width,height);

    NSView *nsview = GetNSViewForSuperview();
    NSView *superview = [nsview superview];

    wxCHECK_RET(GetParent(), wxT("Window can only be placed correctly when it has a parent"));

    NSRect oldFrameRect = [nsview frame];
    NSRect newFrameRect = GetParent()->CocoaTransformWxToBounds(NSMakeRect(x,y,width,height));
    [nsview setFrame:newFrameRect];
    // Be sure to redraw the parent to reflect the changed position
    [superview setNeedsDisplayInRect:oldFrameRect];
    [superview setNeedsDisplayInRect:newFrameRect];
}

void wxWindowCocoa::SetInitialFrameRect(const wxPoint& pos, const wxSize& size)
{
    NSView *nsview = GetNSViewForSuperview();
    NSView *superview = [nsview superview];
    wxCHECK_RET(superview,wxT("NSView does not have a superview"));
    wxCHECK_RET(GetParent(), wxT("Window can only be placed correctly when it has a parent"));
    NSRect frameRect = [nsview frame];
    if(size.x!=-1)
        frameRect.size.width = size.x;
    if(size.y!=-1)
        frameRect.size.height = size.y;
    frameRect.origin.x = pos.x;
    frameRect.origin.y = pos.y;
    // Tell Cocoa to change the margin between the bottom of the superview
    // and the bottom of the control.  Keeps the control pinned to the top
    // of its superview so that its position in the wxWidgets coordinate
    // system doesn't change.
    if(![superview isFlipped])
        [nsview setAutoresizingMask: NSViewMinYMargin];
    // MUST set the mask before setFrame: which can generate a size event
    // and cause a scroller to be added!
    frameRect = GetParent()->CocoaTransformWxToBounds(frameRect);
    [nsview setFrame: frameRect];
}

// Get total size
void wxWindow::DoGetSize(int *w, int *h) const
{
    NSRect cocoaRect = [GetNSViewForSuperview() frame];
    if(w)
        *w=(int)cocoaRect.size.width;
    if(h)
        *h=(int)cocoaRect.size.height;
    wxLogTrace(wxTRACE_COCOA_Window_Size,wxT("wxWindow=%p::DoGetSize = (%d,%d)"),this,(int)cocoaRect.size.width,(int)cocoaRect.size.height);
}

void wxWindow::DoGetPosition(int *x, int *y) const
{
    NSView *nsview = GetNSViewForSuperview();

    NSRect cocoaRect = [nsview frame];
    NSRect rectWx = GetParent()->CocoaTransformBoundsToWx(cocoaRect);
    if(x)
        *x=(int)rectWx.origin.x;
    if(y)
        *y=(int)rectWx.origin.y;
    wxLogTrace(wxTRACE_COCOA_Window_Size,wxT("wxWindow=%p::DoGetPosition = (%d,%d)"),this,(int)cocoaRect.origin.x,(int)cocoaRect.origin.y);
}

WXWidget wxWindow::GetHandle() const
{
    return m_cocoaNSView;
}

wxWindow* wxWindow::GetWxWindow() const
{
    return (wxWindow*) this;
}

void wxWindow::Refresh(bool eraseBack, const wxRect *rect)
{
    [m_cocoaNSView setNeedsDisplay:YES];
}

void wxWindow::SetFocus()
{
    if([GetNSView() acceptsFirstResponder])
        [[GetNSView() window] makeFirstResponder: GetNSView()];
}

void wxWindow::DoCaptureMouse()
{
    // TODO
    sm_capturedWindow = this;
}

void wxWindow::DoReleaseMouse()
{
    // TODO
    sm_capturedWindow = NULL;
}

void wxWindow::DoScreenToClient(int *x, int *y) const
{
    // Point in cocoa screen coordinates:
    NSPoint cocoaScreenPoint = OriginInCocoaScreenCoordinatesForRectInWxDisplayCoordinates(x!=NULL?*x:0, y!=NULL?*y:0, 0, 0, false);
    NSView *clientView = const_cast<wxWindow*>(this)->GetNSView();
    NSWindow *theWindow = [clientView window];

    // Point in window's base coordinate system:
    NSPoint windowPoint = [theWindow convertScreenToBase:cocoaScreenPoint];
    // Point in view's bounds coordinate system
    NSPoint boundsPoint = [clientView convertPoint:windowPoint fromView:nil];
    // Point in wx client coordinates:
    NSPoint theWxClientPoint = CocoaTransformNSViewBoundsToWx(clientView, boundsPoint);
    if(x!=NULL)
        *x = theWxClientPoint.x;
    if(y!=NULL)
        *y = theWxClientPoint.y;
}

void wxWindow::DoClientToScreen(int *x, int *y) const
{
    // Point in wx client coordinates
    NSPoint theWxClientPoint = NSMakePoint(x!=NULL?*x:0, y!=NULL?*y:0);

    NSView *clientView = const_cast<wxWindow*>(this)->GetNSView();

    // Point in the view's bounds coordinate system
    NSPoint boundsPoint = CocoaTransformNSViewWxToBounds(clientView, theWxClientPoint);

    // Point in the window's base coordinate system
    NSPoint windowPoint = [clientView convertPoint:boundsPoint toView:nil];

    NSWindow *theWindow = [clientView window];
    // Point in Cocoa's screen coordinates
    NSPoint screenPoint = [theWindow convertBaseToScreen:windowPoint];

    // Act as though this was the origin of a 0x0 rectangle
    NSRect screenPointRect = NSMakeRect(screenPoint.x, screenPoint.y, 0, 0);

    // Convert that rectangle to wx coordinates
    wxPoint theWxScreenPoint = OriginInWxDisplayCoordinatesForRectInCocoaScreenCoordinates(screenPointRect);
    if(*x)
        *x = theWxScreenPoint.x;
    if(*y)
        *y = theWxScreenPoint.y;
}

// Get size *available for subwindows* i.e. excluding menu bar etc.
void wxWindow::DoGetClientSize(int *x, int *y) const
{
    wxLogTrace(wxTRACE_COCOA,wxT("DoGetClientSize:"));
    if(m_wxCocoaScrollView)
        m_wxCocoaScrollView->DoGetClientSize(x,y);
    else
        wxWindowCocoa::DoGetSize(x,y);
}

void wxWindow::DoSetClientSize(int width, int height)
{
    wxLogTrace(wxTRACE_COCOA_Window_Size,wxT("DoSetClientSize=(%d,%d)"),width,height);
    if(m_wxCocoaScrollView)
        m_wxCocoaScrollView->ClientSizeToSize(width,height);
    CocoaSetWxWindowSize(width,height);
}

void wxWindow::CocoaSetWxWindowSize(int width, int height)
{
    wxWindowCocoa::DoSetSize(wxDefaultCoord,wxDefaultCoord,width,height,wxSIZE_USE_EXISTING);
}

void wxWindow::SetLabel(const wxString& WXUNUSED(label))
{
    // Intentional no-op.
}

wxString wxWindow::GetLabel() const
{
    // General Get/Set of labels is NOT implemented in wxControlBase until trunk (2.9).
    wxLogDebug(wxT("wxWindow::GetLabel: Should be overridden if needed."));
    return wxEmptyString;
}

int wxWindow::GetCharHeight() const
{
    // TODO
    return 0;
}

int wxWindow::GetCharWidth() const
{
    // TODO
    return 0;
}

void wxWindow::GetTextExtent(const wxString& string, int *outX, int *outY,
        int *outDescent, int *outExternalLeading, const wxFont *inFont) const
{
    // FIXME: This obviously ignores the window's font (if any) along with any size
    // transformations.  However, it's better than nothing.
    // We don't create a wxClientDC because we don't want to accidently be able to use
    // it for drawing.
    wxDC tmpdc;
    // NOTE: We must use const_cast here on 2.8 because we must take a const wxFont but the wxDC method
    // must not.  In trunk, the wxDC method has been corrected to take const wxFont.
    return tmpdc.GetTextExtent(string, outX, outY, outDescent, outExternalLeading, const_cast<wxFont*>(inFont));
}

// Coordinates relative to the window
void wxWindow::WarpPointer (int x_pos, int y_pos)
{
    // TODO
}

int wxWindow::GetScrollPos(int orient) const
{
    // TODO
    return 0;
}

// This now returns the whole range, not just the number
// of positions that we can scroll.
int wxWindow::GetScrollRange(int orient) const
{
    // TODO
    return 0;
}

int wxWindow::GetScrollThumb(int orient) const
{
    // TODO
    return 0;
}

void wxWindow::SetScrollPos(int orient, int pos, bool refresh)
{
    // TODO
}

void wxWindow::CocoaCreateNSScrollView()
{
    if(!m_wxCocoaScrollView)
    {
        m_wxCocoaScrollView = new wxWindowCocoaScrollView(this);
    }
}

// New function that will replace some of the above.
void wxWindow::SetScrollbar(int orient, int pos, int thumbVisible,
    int range, bool refresh)
{
    CocoaCreateNSScrollView();
    // TODO
}

// Does a physical scroll
void wxWindow::ScrollWindow(int dx, int dy, const wxRect *rect)
{
    // TODO
}

static inline int _DoFixupDistance(int vDistance, int cDistance)
{
    // If the virtual distance is wxDefaultCoord, set it to the client distance
    // This definitely has to be done or else we literally get views with a -1 size component!
    if(vDistance == wxDefaultCoord)
        vDistance = cDistance;
    // NOTE: Since cDistance should always be >= 0 and since wxDefaultCoord is -1, the above
    // test is more or less useless because it gets covered by the next one.  However, just in
    // case anyone decides that the next test is not correct, I want them to be aware that
    // the above test would still be needed.

    // I am not entirely sure about this next one but I believe it makes sense because
    // otherwise the virtual view (which is the m_cocoaNSView that got wrapped by the scrolling
    // machinery) can be smaller than the NSClipView (the client area) which
    // means that, for instance, mouse clicks inside the client area as wx sees it but outside
    // the virtual area as wx sees it won't be seen by the m_cocoaNSView.
    // We make the assumption that if a virtual distance is less than the client distance that
    // the real view must already be or will soon be positioned at coordinate 0  within the
    // NSClipView that represents the client area.  This way, when we increase the distance to
    // be the client distance, the real view will exactly fit in the clip view.
    else if(vDistance < cDistance)
        vDistance = cDistance;
    return vDistance;
}

void wxWindow::DoSetVirtualSize( int x, int y )
{
    // Call wxWindowBase method which will set m_virtualSize to the appropriate value,
    // possibly not what the caller passed in.  For example, the current implementation
    // clamps the width and height to within the min/max virtual ranges.
    // wxDefaultCoord is passed through unchanged which means we need to handle it ourselves
    // which we do by using the _DoFixupDistance helper method.
    wxWindowBase::DoSetVirtualSize(x,y);
    // Create the scroll view if it hasn't been already.
    CocoaCreateNSScrollView();
    // Now use fixed-up distances when setting the frame size
    wxSize clientSize = GetClientSize();
    [m_cocoaNSView setFrameSize:NSMakeSize(_DoFixupDistance(m_virtualSize.x, clientSize.x), _DoFixupDistance(m_virtualSize.y, clientSize.y))];
}

bool wxWindow::SetFont(const wxFont& font)
{
    // FIXME: We may need to handle wx font inheritance.
    return wxWindowBase::SetFont(font);
}

#if 0 // these are used when debugging the algorithm.
static char const * const comparisonresultStrings[] =
{   "<"
,   "=="
,   ">"
};
#endif

class CocoaWindowCompareContext
{
    DECLARE_NO_COPY_CLASS(CocoaWindowCompareContext)
public:
    CocoaWindowCompareContext(); // Not implemented
    CocoaWindowCompareContext(NSView *target, NSArray *subviews)
    {
        m_target = target;
        // Cocoa sorts subviews in-place.. make a copy
        m_subviews = [subviews copy];
    }
    ~CocoaWindowCompareContext()
    {   // release the copy
        [m_subviews release];
    }
    NSView* target()
    {   return m_target; }
    NSArray* subviews()
    {   return m_subviews; }
    /* Helper function that returns the comparison based off of the original ordering */
    CocoaWindowCompareFunctionResult CompareUsingOriginalOrdering(id first, id second)
    {
        NSUInteger firstI = [m_subviews indexOfObjectIdenticalTo:first];
        NSUInteger secondI = [m_subviews indexOfObjectIdenticalTo:second];
        // NOTE: If either firstI or secondI is NSNotFound then it will be NSIntegerMax and thus will
        // likely compare higher than the other view which is reasonable considering the only way that
        // can happen is if the subview was added after our call to subviews but before the call to
        // sortSubviewsUsingFunction:context:.  Thus we don't bother checking.  Particularly because
        // that case should never occur anyway because that would imply a multi-threaded GUI call
        // which is a big no-no with Cocoa.

        // Subviews are ordered from back to front meaning one that is already lower will have an lower index.
        NSComparisonResult result = (firstI < secondI)
            ?   NSOrderedAscending /* -1 */
            :   (firstI > secondI)
                ?   NSOrderedDescending /* 1 */
                :   NSOrderedSame /* 0 */;

#if 0 // Enable this if you need to debug the algorithm.
        NSLog(@"%@ [%d] %s %@ [%d]\n", first, firstI, comparisonresultStrings[result+1], second, secondI);
#endif
        return result;
    }
private:
    /* The subview we are trying to Raise or Lower */
    NSView *m_target;
    /* A copy of the original array of subviews */
    NSArray *m_subviews;
};

/* Causes Cocoa to raise the target view to the top of the Z-Order by telling the sort function that
 * the target view is always higher than every other view.  When comparing two views neither of
 * which is the target, it returns the correct response based on the original ordering
 */
static CocoaWindowCompareFunctionResult CocoaRaiseWindowCompareFunction(id first, id second, void *ctx)
{
    CocoaWindowCompareContext *compareContext = (CocoaWindowCompareContext*)ctx;
    // first should be ordered higher
    if(first==compareContext->target())
        return NSOrderedDescending;
    // second should be ordered higher
    if(second==compareContext->target())
        return NSOrderedAscending;
    return compareContext->CompareUsingOriginalOrdering(first,second);
}

// Raise the window to the top of the Z order
void wxWindow::Raise()
{
//    wxAutoNSAutoreleasePool pool;
    NSView *nsview = GetNSViewForSuperview();
    NSView *superview = [nsview superview];
    CocoaWindowCompareContext compareContext(nsview, [superview subviews]);

    [superview sortSubviewsUsingFunction:
            CocoaRaiseWindowCompareFunction
        context: &compareContext];
}

/* Causes Cocoa to lower the target view to the bottom of the Z-Order by telling the sort function that
 * the target view is always lower than every other view.  When comparing two views neither of
 * which is the target, it returns the correct response based on the original ordering
 */
static CocoaWindowCompareFunctionResult CocoaLowerWindowCompareFunction(id first, id second, void *ctx)
{
    CocoaWindowCompareContext *compareContext = (CocoaWindowCompareContext*)ctx;
    // first should be ordered lower
    if(first==compareContext->target())
        return NSOrderedAscending;
    // second should be ordered lower
    if(second==compareContext->target())
        return NSOrderedDescending;
    return compareContext->CompareUsingOriginalOrdering(first,second);
}

// Lower the window to the bottom of the Z order
void wxWindow::Lower()
{
    NSView *nsview = GetNSViewForSuperview();
    NSView *superview = [nsview superview];
    CocoaWindowCompareContext compareContext(nsview, [superview subviews]);

#if 0
    NSLog(@"Target:\n%@\n", nsview);
    NSLog(@"Before:\n%@\n", compareContext.subviews());
#endif
    [superview sortSubviewsUsingFunction:
            CocoaLowerWindowCompareFunction
        context: &compareContext];
#if 0
    NSLog(@"After:\n%@\n", [superview subviews]);
#endif
}

bool wxWindow::DoPopupMenu(wxMenu *menu, int x, int y)
{
    return false;
}

// Get the window with the focus
wxWindow *wxWindowBase::DoFindFocus()
{
    // Basically we are somewhat emulating the responder chain here except
    // we are only loking for the first responder in the key window or
    // upon failing to find one if the main window is different we look
    // for the first responder in the main window.

    // Note that the firstResponder doesn't necessarily have to be an
    // NSView but wxCocoaNSView::GetFromCocoa() will simply return
    // NULL unless it finds its argument in its hash map.

    wxCocoaNSView *win;

    NSWindow *keyWindow = [[NSApplication sharedApplication] keyWindow];
    win = wxCocoaNSView::GetFromCocoa(static_cast<NSView*>([keyWindow firstResponder]));
    if(win)
        return win->GetWxWindow();

    NSWindow *mainWindow = [[NSApplication sharedApplication] keyWindow];
    if(mainWindow == keyWindow)
        return NULL;
    win = wxCocoaNSView::GetFromCocoa(static_cast<NSView*>([mainWindow firstResponder]));
    if(win)
        return win->GetWxWindow();

    return NULL;
}

/* static */ wxWindow *wxWindowBase::GetCapture()
{
    // TODO
    return wxWindowCocoa::sm_capturedWindow;
}

wxWindow *wxGetActiveWindow()
{
    // TODO
    return NULL;
}

wxPoint wxGetMousePosition()
{
    // TODO
    return wxDefaultPosition;
}

wxMouseState wxGetMouseState()
{
    wxMouseState ms;
    // TODO
    return ms;
}

wxWindow* wxFindWindowAtPointer(wxPoint& pt)
{
    pt = wxGetMousePosition();
    return NULL;
}

// ========================================================================
// wxCocoaMouseMovedEventSynthesizer
// ========================================================================

#define wxTRACE_COCOA_MouseMovedSynthesizer wxT("COCOA_MouseMovedSynthesizer")

/* This class registers one run loop observer to cover all windows registered with it.
 *  It will register the observer when the first view is registerd and unregister the
 * observer when the last view is unregistered.
 * It is instantiated as a static s_mouseMovedSynthesizer in this file although there
 * is no reason it couldn't be instantiated multiple times.
 */
class wxCocoaMouseMovedEventSynthesizer
{
    DECLARE_NO_COPY_CLASS(wxCocoaMouseMovedEventSynthesizer)
public:
    wxCocoaMouseMovedEventSynthesizer()
    {   m_lastScreenMouseLocation = NSZeroPoint;
    }
    ~wxCocoaMouseMovedEventSynthesizer();
    void RegisterWxCocoaView(wxCocoaNSView *aView);
    void UnregisterWxCocoaView(wxCocoaNSView *aView);
    void SynthesizeMouseMovedEvent();
    
protected:
    void AddRunLoopObserver();
    void RemoveRunLoopObserver();
    wxCFRef<CFRunLoopObserverRef> m_runLoopObserver;
    std::list<wxCocoaNSView*> m_registeredViews;
    NSPoint m_lastScreenMouseLocation;
    static void SynthesizeMouseMovedEvent(CFRunLoopObserverRef observer, CFRunLoopActivity activity, void *info);
};

void wxCocoaMouseMovedEventSynthesizer::RegisterWxCocoaView(wxCocoaNSView *aView)
{
    m_registeredViews.push_back(aView);
    wxLogTrace(wxTRACE_COCOA_MouseMovedSynthesizer, wxT("Registered wxCocoaNSView=%p"), aView);

    if(!m_registeredViews.empty() && m_runLoopObserver == NULL)
    {
        AddRunLoopObserver();
    }
}

void wxCocoaMouseMovedEventSynthesizer::UnregisterWxCocoaView(wxCocoaNSView *aView)
{
    m_registeredViews.remove(aView);
    wxLogTrace(wxTRACE_COCOA_MouseMovedSynthesizer, wxT("Unregistered wxCocoaNSView=%p"), aView);
    if(m_registeredViews.empty() && m_runLoopObserver != NULL)
    {
        RemoveRunLoopObserver();
    }
}

wxCocoaMouseMovedEventSynthesizer::~wxCocoaMouseMovedEventSynthesizer()
{
    if(!m_registeredViews.empty())
    {
        // This means failure to clean up so we report on it as a debug message.
        wxLogDebug(wxT("There are still %d wxCocoaNSView registered to receive mouse moved events at static destruction time"), m_registeredViews.size());
        m_registeredViews.clear();
    }
    if(m_runLoopObserver != NULL)
    {
        // This should not occur unless m_registeredViews was not empty since the last object unregistered should have done this.
        wxLogDebug(wxT("Removing run loop observer during static destruction time."));
        RemoveRunLoopObserver();
    }
}

void wxCocoaMouseMovedEventSynthesizer::SynthesizeMouseMovedEvent(CFRunLoopObserverRef observer, CFRunLoopActivity activity, void *info)
{
    reinterpret_cast<wxCocoaMouseMovedEventSynthesizer*>(info)->SynthesizeMouseMovedEvent();
}

void wxCocoaMouseMovedEventSynthesizer::AddRunLoopObserver()
{
    CFRunLoopObserverContext observerContext =
    {   0
    ,   this
    ,   NULL
    ,   NULL
    ,   NULL
    };
    m_runLoopObserver.reset(CFRunLoopObserverCreate(kCFAllocatorDefault, kCFRunLoopBeforeWaiting, TRUE, 0, SynthesizeMouseMovedEvent, &observerContext));
    CFRunLoopAddObserver([[NSRunLoop currentRunLoop] getCFRunLoop], m_runLoopObserver, kCFRunLoopCommonModes);
    wxLogTrace(wxTRACE_COCOA_TrackingRect, wxT("Added tracking rect run loop observer"));
}

void wxCocoaMouseMovedEventSynthesizer::RemoveRunLoopObserver()
{
    CFRunLoopRemoveObserver([[NSRunLoop currentRunLoop] getCFRunLoop], m_runLoopObserver, kCFRunLoopCommonModes);
    m_runLoopObserver.reset();
    wxLogTrace(wxTRACE_COCOA_TrackingRect, wxT("Removed tracking rect run loop observer"));
}

void wxCocoaMouseMovedEventSynthesizer::SynthesizeMouseMovedEvent()
{
#if wxUSE_ABI_INCOMPATIBLE_FEATURES
    NSPoint screenMouseLocation = [NSEvent mouseLocation];
    // Checking the last mouse location is done for a few reasons:
    // 1. We are observing every iteration of the event loop so we'd be sending out a lot of extraneous events
    //    telling the app the mouse moved when the user hit a key for instance.
    // 2. When handling the mouse moved event, user code can do something to the view which will cause Cocoa to
    //    call resetCursorRects.  Cocoa does this by using a delayed notification which means the event loop gets
    //    pumped once which would mean that if we didn't check the mouse location we'd get into a never-ending
    //    loop causing the tracking rectangles to constantly be reset.
    if(screenMouseLocation.x != m_lastScreenMouseLocation.x || screenMouseLocation.y != m_lastScreenMouseLocation.y)
    {
        m_lastScreenMouseLocation = screenMouseLocation;
        wxLogTrace(wxTRACE_COCOA_TrackingRect, wxT("Synthesizing mouse moved at screen (%f,%f)"), screenMouseLocation.x, screenMouseLocation.y);
        for(std::list<wxCocoaNSView*>::iterator i = m_registeredViews.begin(); i != m_registeredViews.end(); ++i)
        {
            (*i)->Cocoa_synthesizeMouseMoved();
        }
    }
#endif
}

// Singleton used for all views:
static wxCocoaMouseMovedEventSynthesizer s_mouseMovedSynthesizer;

// ========================================================================
// wxCocoaTrackingRectManager
// ========================================================================

wxCocoaTrackingRectManager::wxCocoaTrackingRectManager(wxWindow *window)
:   m_window(window)
{
    m_isTrackingRectActive = false;
    BuildTrackingRect();
}

void wxCocoaTrackingRectManager::ClearTrackingRect()
{
    if(m_isTrackingRectActive)
    {
        [m_window->GetNSView() removeTrackingRect:m_trackingRectTag];
        m_isTrackingRectActive = false;
        wxLogTrace(wxTRACE_COCOA_TrackingRect, wxT("%s@%p: Removed tracking rect #%d"), m_window->GetClassInfo()->GetClassName(), m_window, m_trackingRectTag);
    }
    // If we were doing periodic events we need to clear those too
    StopSynthesizingEvents();
}

void wxCocoaTrackingRectManager::StopSynthesizingEvents()
{
    s_mouseMovedSynthesizer.UnregisterWxCocoaView(m_window);
}

void wxCocoaTrackingRectManager::BuildTrackingRect()
{
    // Pool here due to lack of one during wx init phase
    wxAutoNSAutoreleasePool pool;

    wxASSERT_MSG(!m_isTrackingRectActive, wxT("Tracking rect was not cleared"));

    NSView *theView = m_window->GetNSView();

    if([theView window] != nil)
    {
        NSRect visibleRect = [theView visibleRect];

        m_trackingRectTag = [theView addTrackingRect:visibleRect owner:theView userData:NULL assumeInside:NO];
        m_trackingRectInWindowCoordinates = [theView convertRect:visibleRect toView:nil];
        m_isTrackingRectActive = true;

        wxLogTrace(wxTRACE_COCOA_TrackingRect, wxT("%s@%p: Added tracking rect #%d"), m_window->GetClassInfo()->GetClassName(), m_window, m_trackingRectTag);
    }
}

void wxCocoaTrackingRectManager::BeginSynthesizingEvents()
{
    s_mouseMovedSynthesizer.RegisterWxCocoaView(m_window);
}

void wxCocoaTrackingRectManager::RebuildTrackingRectIfNeeded()
{
    if(m_isTrackingRectActive)
    {
        NSView *theView = m_window->GetNSView();
        NSRect currentRect = [theView convertRect:[theView visibleRect] toView:nil];
        if(NSEqualRects(m_trackingRectInWindowCoordinates,currentRect))
        {
            wxLogTrace(wxTRACE_COCOA_TrackingRect, wxT("Ignored request to rebuild TR#%d"), m_trackingRectTag);
            return;
        }
    }
    RebuildTrackingRect();
}

void wxCocoaTrackingRectManager::RebuildTrackingRect()
{
    ClearTrackingRect();
    BuildTrackingRect();
}

wxCocoaTrackingRectManager::~wxCocoaTrackingRectManager()
{
    ClearTrackingRect();
}

bool wxCocoaTrackingRectManager::IsOwnerOfEvent(NSEvent *anEvent)
{
    return m_isTrackingRectActive && (m_trackingRectTag == [anEvent trackingNumber]);
}

