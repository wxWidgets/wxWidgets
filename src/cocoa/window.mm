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

#import <Appkit/NSView.h>

// normally the base classes aren't included, but wxWindow is special
#ifdef __WXUNIVERSAL__
IMPLEMENT_ABSTRACT_CLASS(wxWindowCocoa, wxWindowBase)
#else
IMPLEMENT_DYNAMIC_CLASS(wxWindow, wxWindowBase)
#endif

BEGIN_EVENT_TABLE(wxWindowCocoa, wxWindowBase)
END_EVENT_TABLE()

// Constructor
void wxWindowCocoa::Init()
{
    InitBase();

    m_cocoaNSView = NULL;
    m_dummyNSView = NULL;
    m_isBeingDeleted = FALSE;
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
    NSRect cocoaRect = NSMakeRect(10,10,20,20);
    m_cocoaNSView = NULL;
    SetNSView([[NSView alloc] initWithFrame: cocoaRect]);
    [m_cocoaNSView release];

    if (m_parent)
    {
        m_parent->AddChild(this);
        m_parent->CocoaAddChild(this);
    }

    return TRUE;
}

// Destructor
wxWindow::~wxWindow()
{
    DestroyChildren();

    if(m_parent)
        m_parent->RemoveChild(this);

    CocoaRemoveFromParent();
    SetNSView(NULL);
}

void wxWindowCocoa::CocoaAddChild(wxWindowCocoa *child)
{
    [child->m_cocoaNSView retain];
    // NOTE: addSubView takes ownership of, but does not retain the subview
    // Upon a removeFromView or closing the super view, the child WILL be
    // released!!!  I think the idea here is that normally you would alloc
    // the subview and add it to the superview and this way you don't have
    // to release what you just alloced.  Unfortunately, that doesn't
    // make sense for wxCocoa, so we do this instead.
    [m_cocoaNSView addSubview: child->m_cocoaNSView];
    wxASSERT(!child->m_dummyNSView);
    child->m_isShown = true;
}

void wxWindowCocoa::CocoaRemoveFromParent(void)
{
    if(m_dummyNSView)
    {
        wxASSERT(m_cocoaNSView);
        // balances the alloc
        [m_dummyNSView removeFromSuperview];
        // But since we also retained it ourselves
        [m_dummyNSView release];
        m_dummyNSView = nil;
        // m_cocoaNSView has of course already been removed by virtue of
        // replaceSubview: m_cocoaNSView with: m_dummyNSView
    }
    else
        [m_cocoaNSView removeFromSuperview];
}

void wxWindowCocoa::SetNSView(WX_NSView cocoaNSView)
{
    bool need_debug = cocoaNSView || m_cocoaNSView;
    if(need_debug) wxLogDebug("wxWindowCocoa=%p::SetNSView [m_cocoaNSView=%p retainCount]=%d",this,m_cocoaNSView,[m_cocoaNSView retainCount]);
    if(m_cocoaNSView)
        DisassociateNSView(m_cocoaNSView);
    [cocoaNSView retain];
    [m_cocoaNSView release];
    m_cocoaNSView = cocoaNSView;
    if(m_cocoaNSView)
        AssociateNSView(m_cocoaNSView);
    if(need_debug) wxLogDebug("wxWindowCocoa=%p::SetNSView [cocoaNSView=%p retainCount]=%d",this,cocoaNSView,[cocoaNSView retainCount]);
}

bool wxWindowCocoa::Cocoa_drawRect(const NSRect &rect)
{
    wxLogDebug("Cocoa_drawRect");
    //FIXME: should probably turn that rect into the update region
    wxPaintEvent event(m_windowId);
    event.SetEventObject(this);
    return GetEventHandler()->ProcessEvent(event);
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

bool wxWindow::Show(bool show)
{
    // If the window is marked as visible, then it shouldn't have a dummy view
    // If the window is marked hidden, then it should have a dummy view
    wxASSERT_MSG( (m_isShown && !m_dummyNSView) || (!m_isShown && m_dummyNSView),"wxWindow: m_isShown does not agree with m_dummyNSView");
    // Return false if there isn't a window to show or hide
    if(!m_cocoaNSView)
        return false;
    // Return false if the state isn't changing
    if( show == m_isShown )
        return false;
    if(show)
    {
        // replaceSubView releases m_dummyNSView, balancing the alloc
        [m_cocoaNSView retain];
        [[m_dummyNSView superview] replaceSubview:m_dummyNSView with:m_cocoaNSView];
        // But since we also retained it ourselves
        [m_dummyNSView release];
        m_dummyNSView = nil;
    }
    else
    {
        m_dummyNSView = [[NSView alloc] initWithFrame: [m_cocoaNSView frame]];
        [m_dummyNSView retain];
        // NOTE: replaceSubView will cause m_cocaNSView to be released
        [[m_cocoaNSView superview] replaceSubview:m_cocoaNSView with:m_dummyNSView];
        // m_coocaNSView is now only retained by us
    }
    m_isShown = show;
    return true;
}

void wxWindowCocoa::DoSetSize(int x, int y, int width, int height, int sizeFlags)
{
    wxLogDebug("wxWindow=%p::DoSetSizeWindow(%d,%d,%d,%d,Auto: %s%s)",this,x,y,width,height,(sizeFlags&wxSIZE_AUTO_WIDTH)?"W":".",sizeFlags&wxSIZE_AUTO_HEIGHT?"H":".");
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
    wxLogDebug("wxWindow=%p::DoMoveWindow(%d,%d,%d,%d)",this,x,y,width,height);

    NSView *nsview = m_dummyNSView?m_dummyNSView:m_cocoaNSView;
    NSView *superview = [nsview superview];
    wxCHECK_RET(superview,"NSView does not have a superview");
    NSRect parentRect = [superview frame];

    NSRect cocoaRect = NSMakeRect(x,parentRect.size.height-(y+height),width,height);
    [m_cocoaNSView setFrame: cocoaRect];
    // Also change the dummy's size
    if(m_dummyNSView)
        [m_dummyNSView setFrame: cocoaRect];
}

// Get total size
void wxWindow::DoGetSize(int *w, int *h) const
{
    NSRect cocoaRect = [m_cocoaNSView frame];
    if(w)
        *w=(int)cocoaRect.size.width;
    if(h)
        *h=(int)cocoaRect.size.height;
    wxLogDebug("wxWindow=%p::DoGetSize = (%d,%d)",this,(int)cocoaRect.size.width,(int)cocoaRect.size.height);
}

void wxWindow::DoGetPosition(int *x, int *y) const
{
    NSView *nsview = m_dummyNSView?m_dummyNSView:m_cocoaNSView;
    NSView *superview = [nsview superview];
    wxCHECK_RET(superview,"NSView does not have a superview");
    NSRect parentRect = [superview frame];

    NSRect cocoaRect = [nsview frame];
    if(x)
        *x=(int)cocoaRect.origin.x;
    if(y)
        *y=(int)(parentRect.size.height-(cocoaRect.origin.y+cocoaRect.size.height));
    wxLogDebug("wxWindow=%p::DoGetPosition = (%d,%d)",this,(int)cocoaRect.origin.x,(int)cocoaRect.origin.y);
}

WXWidget wxWindow::GetHandle() const
{
    return m_cocoaNSView;
}

void wxWindow::SetFocus()
{
    // TODO
}

void wxWindow::DoCaptureMouse()
{
    // TODO
}

void wxWindow::DoReleaseMouse()
{
    // TODO
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

void wxWindow::Refresh(bool eraseBack, const wxRect *rect)
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

void wxWindow::Clear()
{
    // TODO
}

// Raise the window to the top of the Z order
void wxWindow::Raise()
{
    // TODO
}

// Lower the window to the bottom of the Z order
void wxWindow::Lower()
{
    // TODO
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
    return NULL;
}

wxWindow *wxGetActiveWindow()
{
    // TODO
    return NULL;
}

