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

#import <AppKit/NSView.h>
#import <AppKit/NSEvent.h>
#import <AppKit/NSScrollView.h>
#import <AppKit/NSColor.h>
#import <AppKit/NSClipView.h>
#import <Foundation/NSException.h>
#import <AppKit/NSApplication.h>
#import <AppKit/NSWindow.h>

// Turn this on to paint green over the dummy views for debugging
#undef WXCOCOA_FILL_DUMMY_VIEW

#ifdef WXCOCOA_FILL_DUMMY_VIEW
#import <AppKit/NSBezierPath.h>
#endif //def WXCOCOA_FILL_DUMMY_VIEW

// A category for methods that are only present in Panther's SDK
@interface NSView(wxNSViewPrePantherCompatibility)
- (void)getRectsBeingDrawn:(const NSRect **)rects count:(int *)count;
@end

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
private:
    wxWindowCocoaScrollView();
};

// ========================================================================
// wxDummyNSView
// ========================================================================
@interface wxDummyNSView : NSView
- (NSView *)hitTest:(NSPoint)aPoint;
@end

@implementation wxDummyNSView : NSView
- (NSView *)hitTest:(NSPoint)aPoint
{
    return nil;
}

@end

// ========================================================================
// wxWindowCocoaHider
// ========================================================================
wxWindowCocoaHider::wxWindowCocoaHider(wxWindow *owner)
:   m_owner(owner)
{
    wxASSERT(owner);
    wxASSERT(owner->GetNSViewForHiding());
    m_dummyNSView = [[wxDummyNSView alloc]
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

@implementation wxFlippedNSClipView : NSClipView
- (BOOL)isFlipped
{
    return YES;
}

@end

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
    NSClipView *flippedClip = [[wxFlippedNSClipView alloc]
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
    SetNSView([[NSView alloc] initWithFrame: MakeDefaultNSRect(size)]);
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
    NSView *childView = child->GetNSViewForSuperview();

    wxASSERT(childView);
    [m_cocoaNSView addSubview: childView];
    child->m_isShown = !m_cocoaHider;
}

void wxWindowCocoa::CocoaRemoveFromParent(void)
{
    [GetNSViewForSuperview() removeFromSuperview];
}

void wxWindowCocoa::SetNSView(WX_NSView cocoaNSView)
{
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
    int countRects = 1;
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
    wxLogTrace(wxTRACE_COCOA,wxT("Mouse Drag @%d,%d"),event.m_x,event.m_y);
    return GetEventHandler()->ProcessEvent(event);
}

bool wxWindowCocoa::Cocoa_mouseEntered(WX_NSEvent theEvent)
{
    return false;
}

bool wxWindowCocoa::Cocoa_mouseExited(WX_NSEvent theEvent)
{
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
    wxLogTrace(wxTRACE_COCOA,wxT("Cocoa_FrameChanged"));
    wxSizeEvent event(GetSize(), m_windowId);
    event.SetEventObject(this);
    GetEventHandler()->ProcessEvent(event);
}

bool wxWindowCocoa::Cocoa_resetCursorRects()
{
    if(!m_cursor.GetNSCursor())
        return false;

    [GetNSView() addCursorRect: [GetNSView() visibleRect]  cursor: m_cursor.GetNSCursor()];

    return true;
}

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

void wxWindow::DoEnable(bool enable)
{
	CocoaSetEnabled(enable);
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
    // TODO
}

void wxWindow::DoClientToScreen(int *x, int *y) const
{
    // TODO
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
    // TODO
}

wxString wxWindow::GetLabel() const
{
    // TODO
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

void wxWindow::GetTextExtent(const wxString& string, int *x, int *y,
        int *descent, int *externalLeading, const wxFont *theFont) const
{
    // TODO
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

void wxWindow::DoSetVirtualSize( int x, int y )
{
    wxWindowBase::DoSetVirtualSize(x,y);
    CocoaCreateNSScrollView();
    [m_cocoaNSView setFrameSize:NSMakeSize(m_virtualSize.x,m_virtualSize.y)];
}

bool wxWindow::SetFont(const wxFont& font)
{
    // TODO
    return true;
}

static int CocoaRaiseWindowCompareFunction(id first, id second, void *target)
{
    // first should be ordered higher
    if(first==target)
        return NSOrderedDescending;
    // second should be ordered higher
    if(second==target)
        return NSOrderedAscending;
    return NSOrderedSame;
}

// Raise the window to the top of the Z order
void wxWindow::Raise()
{
//    wxAutoNSAutoreleasePool pool;
    NSView *nsview = GetNSViewForSuperview();
    [[nsview superview] sortSubviewsUsingFunction:
            CocoaRaiseWindowCompareFunction
        context: nsview];
}

static int CocoaLowerWindowCompareFunction(id first, id second, void *target)
{
    // first should be ordered lower
    if(first==target)
        return NSOrderedAscending;
    // second should be ordered lower
    if(second==target)
        return NSOrderedDescending;
    return NSOrderedSame;
}

// Lower the window to the bottom of the Z order
void wxWindow::Lower()
{
    NSView *nsview = GetNSViewForSuperview();
    [[nsview superview] sortSubviewsUsingFunction:
            CocoaLowerWindowCompareFunction
        context: nsview];
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
