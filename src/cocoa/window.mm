/////////////////////////////////////////////////////////////////////////////
// Name:        src/cocoa/window.mm
// Purpose:     wxWindowCocoa
// Author:      David Elliott
// Modified by:
// Created:     2002/12/26
// RCS-ID:      $Id:
// Copyright:   (c) 2002 David Elliott
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#include "wx/window.h"
#include "wx/log.h"

#include "wx/cocoa/autorelease.h"

#import <Appkit/NSView.h>
#import <AppKit/NSEvent.h>
#import <AppKit/NSScrollView.h>
#import <AppKit/NSColor.h>
#import <AppKit/NSClipView.h>

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
private:
    wxWindowCocoaHider();
};

// ========================================================================
// wxWindowCocoaScroller
// ========================================================================
class wxWindowCocoaScroller: protected wxCocoaNSView
{
    DECLARE_NO_COPY_CLASS(wxWindowCocoaScroller)
public:
    wxWindowCocoaScroller(wxWindow *owner);
    virtual ~wxWindowCocoaScroller();
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
    wxWindowCocoaScroller();
};

// ========================================================================
// wxWindowCocoaHider
// ========================================================================
wxWindowCocoaHider::wxWindowCocoaHider(wxWindow *owner)
:   m_owner(owner)
{
    wxASSERT(owner);
    wxASSERT(owner->GetNSViewForHiding());
    m_dummyNSView = [[NSView alloc]
        initWithFrame:[owner->GetNSViewForHiding() frame]];
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
// wxWindowCocoaScroller
// ========================================================================
wxWindowCocoaScroller::wxWindowCocoaScroller(wxWindow *owner)
:   m_owner(owner)
{
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

void wxWindowCocoaScroller::Encapsulate()
{
    // NOTE: replaceSubView will cause m_cocaNSView to be released
    // except when it hasn't been added into an NSView hierarchy in which
    // case it doesn't need to be and this should work out to a no-op
    m_owner->CocoaReplaceView(m_owner->GetNSView(), m_cocoaNSScrollView);
    // The NSView is still retained by owner
    [m_cocoaNSScrollView setDocumentView: m_owner->GetNSView()];
    // Now it's also retained by the NSScrollView
}

void wxWindowCocoaScroller::Unencapsulate()
{
    [m_cocoaNSScrollView setDocumentView: nil];
    m_owner->CocoaReplaceView(m_cocoaNSScrollView, m_owner->GetNSView());
}

wxWindowCocoaScroller::~wxWindowCocoaScroller()
{
    DisassociateNSView(m_cocoaNSScrollView);
    [m_cocoaNSScrollView release];
}

void wxWindowCocoaScroller::ClientSizeToSize(int &width, int &height)
{
    NSSize frameSize = [NSScrollView
        frameSizeForContentSize: NSMakeSize(width,height)
        hasHorizontalScroller: [m_cocoaNSScrollView hasHorizontalScroller]
        hasVerticalScroller: [m_cocoaNSScrollView hasVerticalScroller]
        borderType: [m_cocoaNSScrollView borderType]];
    width = frameSize.width;
    height = frameSize.height;
}

void wxWindowCocoaScroller::DoGetClientSize(int *x, int *y) const
{
    NSSize nssize = [m_cocoaNSScrollView contentSize];
    if(x)
        *x = nssize.width;
    if(y)
        *y = nssize.height;
}

void wxWindowCocoaScroller::Cocoa_FrameChanged(void)
{
    wxLogDebug("Cocoa_FrameChanged");
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
    InitBase();

    m_cocoaNSView = NULL;
    m_cocoaHider = NULL;
    m_cocoaScroller = NULL;
    m_isBeingDeleted = FALSE;
    m_isInPaint = FALSE;
    m_shouldBeEnabled = true;
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
    }
    SetInitialFrameRect(pos,size);

    return TRUE;
}

// Destructor
wxWindow::~wxWindow()
{
    wxAutoNSAutoreleasePool pool;
    DestroyChildren();

    if(m_parent)
        m_parent->RemoveChild(this);

    CocoaRemoveFromParent();
    delete m_cocoaHider;
    delete m_cocoaScroller;
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
    if(need_debug) wxLogDebug("wxWindowCocoa=%p::SetNSView [m_cocoaNSView=%p retainCount]=%d",this,m_cocoaNSView,[m_cocoaNSView retainCount]);
    DisassociateNSView(m_cocoaNSView);
    [cocoaNSView retain];
    [m_cocoaNSView release];
    m_cocoaNSView = cocoaNSView;
    AssociateNSView(m_cocoaNSView);
    if(need_debug) wxLogDebug("wxWindowCocoa=%p::SetNSView [cocoaNSView=%p retainCount]=%d",this,cocoaNSView,[cocoaNSView retainCount]);
}

WX_NSView wxWindowCocoa::GetNSViewForSuperview() const
{
    return m_cocoaHider
        ?   m_cocoaHider->GetNSView()
        :   m_cocoaScroller
            ?   m_cocoaScroller->GetNSScrollView()
            :   m_cocoaNSView;
}

WX_NSView wxWindowCocoa::GetNSViewForHiding() const
{
    return m_cocoaScroller
        ?   m_cocoaScroller->GetNSScrollView()
        :   m_cocoaNSView;
}

bool wxWindowCocoa::Cocoa_drawRect(const NSRect &rect)
{
    wxLogDebug("Cocoa_drawRect");
    // Recursion can happen if the event loop runs from within the paint
    // handler.  For instance, if an assertion dialog is shown.
    // FIXME: This seems less than ideal.
    if(m_isInPaint)
    {
        wxLogDebug("Paint event recursion!");
        return false;
    }
    //FIXME: should probably turn that rect into the update region
    m_isInPaint = TRUE;
    wxPaintEvent event(m_windowId);
    event.SetEventObject(this);
    bool ret = GetEventHandler()->ProcessEvent(event);
    m_isInPaint = FALSE;
    return ret;
}

void wxWindowCocoa::InitMouseEvent(wxMouseEvent& event, WX_NSEvent cocoaEvent)
{
    wxASSERT_MSG([m_cocoaNSView window]==[cocoaEvent window],"Mouse event for different NSWindow");
    NSPoint cocoaPoint = [m_cocoaNSView convertPoint:[(NSEvent*)cocoaEvent locationInWindow] fromView:nil];
    NSRect cocoaRect = [m_cocoaNSView frame];
    const wxPoint &clientorigin = GetClientAreaOrigin();
    event.m_x = (wxCoord)cocoaPoint.x - clientorigin.x;
    event.m_y = (wxCoord)(cocoaRect.size.height - cocoaPoint.y) - clientorigin.y;

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
    wxLogDebug("Mouse Drag @%d,%d",event.m_x,event.m_y);
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
    wxLogDebug("Mouse Down @%d,%d num clicks=%d",event.m_x,event.m_y,[theEvent clickCount]);
    return GetEventHandler()->ProcessEvent(event);
}

bool wxWindowCocoa::Cocoa_mouseDragged(WX_NSEvent theEvent)
{
    wxMouseEvent event(wxEVT_MOTION);
    InitMouseEvent(event,theEvent);
    event.m_leftDown = true;
    wxLogDebug("Mouse Drag @%d,%d",event.m_x,event.m_y);
    return GetEventHandler()->ProcessEvent(event);
}

bool wxWindowCocoa::Cocoa_mouseUp(WX_NSEvent theEvent)
{
    wxMouseEvent event(wxEVT_LEFT_UP);
    InitMouseEvent(event,theEvent);
    wxLogDebug("Mouse Up @%d,%d",event.m_x,event.m_y);
    return GetEventHandler()->ProcessEvent(event);
}

bool wxWindowCocoa::Cocoa_rightMouseDown(WX_NSEvent theEvent)
{
    return false;
}

bool wxWindowCocoa::Cocoa_rightMouseDragged(WX_NSEvent theEvent)
{
    return false;
}

bool wxWindowCocoa::Cocoa_rightMouseUp(WX_NSEvent theEvent)
{
    return false;
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
    wxLogDebug("Cocoa_FrameChanged");
    wxSizeEvent event(GetSize(), m_windowId);
    event.SetEventObject(this);
    GetEventHandler()->ProcessEvent(event);
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
//    wxASSERT_MSG( (m_isShown && !m_dummyNSView) || (!m_isShown && m_dummyNSView),"wxWindow: m_isShown does not agree with m_dummyNSView");
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
//    wxLogDebug("wxWindow=%p::DoSetSizeWindow(%d,%d,%d,%d,Auto: %s%s)",this,x,y,width,height,(sizeFlags&wxSIZE_AUTO_WIDTH)?"W":".",sizeFlags&wxSIZE_AUTO_HEIGHT?"H":".");
    int currentX, currentY;
    int currentW, currentH;
    DoGetPosition(&currentX, &currentY);
    DoGetSize(&currentW, &currentH);
    if((x==-1) && !(sizeFlags&wxSIZE_ALLOW_MINUS_ONE))
        x=currentX;
    if((y==-1) && !(sizeFlags&wxSIZE_ALLOW_MINUS_ONE))
        y=currentY;

    AdjustForParentClientOrigin(x,y,sizeFlags);

    wxSize size(-1,-1);

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

void wxWindowCocoa::DoMoveWindow(int x, int y, int width, int height)
{
//    wxLogDebug("wxWindow=%p::DoMoveWindow(%d,%d,%d,%d)",this,x,y,width,height);

    NSView *nsview = GetNSViewForSuperview();
    NSView *superview = [nsview superview];
    wxCHECK_RET(superview,"NSView does not have a superview");
    NSRect parentRect = [superview bounds];

    NSRect cocoaRect = NSMakeRect(x,parentRect.size.height-(y+height),width,height);
    [nsview setFrame: cocoaRect];
    // Be sure to redraw the parent to reflect the changed position
    [superview setNeedsDisplay:YES];
}

void wxWindowCocoa::SetInitialFrameRect(const wxPoint& pos, const wxSize& size)
{
    NSView *nsview = GetNSViewForSuperview();
    NSView *superview = [nsview superview];
    wxCHECK_RET(superview,"NSView does not have a superview");
    NSRect parentRect = [superview bounds];
    NSRect frameRect = [nsview frame];
    if(size.x!=-1)
        frameRect.size.width = size.x;
    if(size.y!=-1)
        frameRect.size.height = size.y;
    frameRect.origin.x = pos.x;
    frameRect.origin.y = parentRect.size.height-(pos.y+frameRect.size.height);
    [nsview setFrame: frameRect];
    // Tell Cocoa to change the margin between the bottom of the superview
    // and the bottom of the control.  Keeps the control pinned to the top
    // of its superview so that its position in the wxWindows coordinate
    // system doesn't change.
    if(![superview isFlipped])
        [nsview setAutoresizingMask: NSViewMinYMargin];
}

// Get total size
void wxWindow::DoGetSize(int *w, int *h) const
{
    NSRect cocoaRect = [GetNSViewForSuperview() frame];
    if(w)
        *w=(int)cocoaRect.size.width;
    if(h)
        *h=(int)cocoaRect.size.height;
//    wxLogDebug("wxWindow=%p::DoGetSize = (%d,%d)",this,(int)cocoaRect.size.width,(int)cocoaRect.size.height);
}

void wxWindow::DoGetPosition(int *x, int *y) const
{
    NSView *nsview = GetNSViewForSuperview();
    NSView *superview = [nsview superview];
    wxCHECK_RET(superview,"NSView does not have a superview");
    NSRect parentRect = [superview bounds];

    NSRect cocoaRect = [nsview frame];
    if(x)
        *x=(int)cocoaRect.origin.x;
    if(y)
        *y=(int)(parentRect.size.height-(cocoaRect.origin.y+cocoaRect.size.height));
//    wxLogDebug("wxWindow=%p::DoGetPosition = (%d,%d)",this,(int)cocoaRect.origin.x,(int)cocoaRect.origin.y);
}

WXWidget wxWindow::GetHandle() const
{
    return m_cocoaNSView;
}

void wxWindow::Refresh(bool eraseBack, const wxRect *rect)
{
    [m_cocoaNSView setNeedsDisplay:YES];
}

void wxWindow::SetFocus()
{
    // TODO
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
    wxLogDebug("DoGetClientSize:");
    if(m_cocoaScroller)
        m_cocoaScroller->DoGetClientSize(x,y);
    else
        wxWindowCocoa::DoGetSize(x,y);
}

void wxWindow::DoSetClientSize(int width, int height)
{
    wxLogDebug("DoSetClientSize=(%d,%d)",width,height);
    if(m_cocoaScroller)
        m_cocoaScroller->ClientSizeToSize(width,height);
    CocoaSetWxWindowSize(width,height);
}

void wxWindow::CocoaSetWxWindowSize(int width, int height)
{
    wxWindowCocoa::DoSetSize(-1,-1,width,height,wxSIZE_USE_EXISTING);
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
    if(!m_cocoaScroller)
    {
        m_cocoaScroller = new wxWindowCocoaScroller(this);
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
    return TRUE;
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
    return FALSE;
}

// Get the window with the focus
wxWindow *wxWindowBase::FindFocus()
{
    // TODO
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

wxWindow* wxFindWindowAtPointer(wxPoint& pt)
{
    pt = wxGetMousePosition();
    return NULL;
}

