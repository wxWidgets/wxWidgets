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
    m_isBeingDeleted = FALSE;
    m_isInPaint = FALSE;
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
        :   m_cocoaNSView;
}

WX_NSView wxWindowCocoa::GetNSViewForHiding() const
{
    return m_cocoaNSView;
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
    return false;
}

void wxWindow::CocoaReplaceView(WX_NSView oldView, WX_NSView newView)
{
    [[oldView superview] replaceSubview:oldView with:newView];
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
    NSRect parentRect = [superview frame];

    NSRect cocoaRect = NSMakeRect(x,parentRect.size.height-(y+height),width,height);
    [nsview setFrame: cocoaRect];
}

void wxWindowCocoa::SetInitialFrameRect(const wxPoint& pos, const wxSize& size)
{
    NSView *nsview = GetNSViewForSuperview();
    NSView *superview = [nsview superview];
    wxCHECK_RET(superview,"NSView does not have a superview");
    NSRect parentRect = [superview frame];
    NSRect frameRect = [nsview frame];
    if(size.x!=-1)
        frameRect.size.width = size.x;
    if(size.y!=-1)
        frameRect.size.height = size.y;
    frameRect.origin.x = pos.x;
    frameRect.origin.y = parentRect.size.height-(pos.y+size.y);
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
//    wxLogDebug("wxWindow=%p::DoGetSize = (%d,%d)",this,(int)cocoaRect.size.width,(int)cocoaRect.size.height);
}

void wxWindow::DoGetPosition(int *x, int *y) const
{
    NSView *nsview = GetNSViewForSuperview();
    NSView *superview = [nsview superview];
    wxCHECK_RET(superview,"NSView does not have a superview");
    NSRect parentRect = [superview frame];

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
    wxWindowCocoa::DoGetSize(x,y);
    // TODO: Actually account for menubar, borders, etc...
}

void wxWindow::DoSetClientSize(int width, int height)
{
    wxLogDebug("DoSetClientSize=(%d,%d)",width,height);
    // TODO
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

// New function that will replace some of the above.
void wxWindow::SetScrollbar(int orient, int pos, int thumbVisible,
    int range, bool refresh)
{
    // TODO
}

// Does a physical scroll
void wxWindow::ScrollWindow(int dx, int dy, const wxRect *rect)
{
    // TODO
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

