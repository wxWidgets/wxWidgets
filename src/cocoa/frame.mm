/////////////////////////////////////////////////////////////////////////////
// Name:        cocoa/frame.mm
// Purpose:     wxFrame
// Author:      David Elliott
// Modified by:
// Created:     2003/03/16
// RCS-ID:      $Id:
// Copyright:   (c) 2003 David Elliott
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
    #include "wx/log.h"
    #include "wx/app.h"
    #include "wx/frame.h"
    #include "wx/menu.h"
    #include "wx/toolbar.h"
    #include "wx/statusbr.h"
#endif // WX_PRECOMP

#include "wx/cocoa/autorelease.h"

#import <AppKit/NSWindow.h>
#import <AppKit/NSApplication.h>
#import <AppKit/NSView.h>

// wxFrame

BEGIN_EVENT_TABLE(wxFrame, wxFrameBase)
END_EVENT_TABLE()

IMPLEMENT_DYNAMIC_CLASS(wxFrame, wxTopLevelWindow)

void wxFrame::Init()
{
    m_frameNSView = nil;
}

bool wxFrame::Create(wxWindow *parent,
           wxWindowID winid,
           const wxString& title,
           const wxPoint& pos,
           const wxSize& size,
           long style,
           const wxString& name)
{
    bool rt = wxTopLevelWindow::Create(parent,winid,title,pos,size,style,name);

    return rt;
}

wxFrame::~wxFrame()
{
    [m_frameNSView release];
}

void wxFrame::AttachMenuBar(wxMenuBar *mbar)
{
    wxFrameBase::AttachMenuBar(mbar);
    if(m_frameMenuBar)
    {
        wxLogDebug("Attached menu");
        [m_cocoaNSWindow setMenu:m_frameMenuBar->GetNSMenu()];
    }
}

void wxFrame::DetachMenuBar()
{
    if(m_frameMenuBar)
    {
        [m_cocoaNSWindow setMenu:nil];
    }
    wxFrameBase::DetachMenuBar();
}

bool wxFrame::Show(bool show)
{
    wxAutoNSAutoreleasePool pool;
    bool ret = wxFrameBase::Show(show);
    if(show && GetMenuBar())
        [wxTheApp->GetNSApplication() setMenu:GetMenuBar()->GetNSMenu() ];
    return ret;
}

wxPoint wxFrame::GetClientAreaOrigin() const
{
    return wxPoint(0,0);
}

void wxFrame::CocoaSetWxWindowSize(int width, int height)
{
    if(m_frameStatusBar)
        height += m_frameStatusBar->GetSize().y;
#if wxUSE_TOOLBAR
    if(m_frameToolBar)
        height += m_frameToolBar->GetSize().y;
#endif //wxUSE_TOOLBAR
    wxTopLevelWindow::CocoaSetWxWindowSize(width,height);
}

void wxFrame::CocoaReplaceView(WX_NSView oldView, WX_NSView newView)
{
    // If we have the additional toolbar/statbar view, then the
    // default replaceSubview will work. Otherwise, the old view
    // should be the content view and should be replaced that way
    if(m_frameNSView)
        wxWindow::CocoaReplaceView(oldView, newView);
    else
        wxTopLevelWindow::CocoaReplaceView(oldView, newView);
}

void wxFrame::UpdateFrameNSView()
{
    if(!m_frameNSView)
    {
        m_frameNSView = [[NSView alloc] initWithFrame:[[m_cocoaNSWindow contentView] frame]];
        [m_cocoaNSWindow setContentView: m_frameNSView];
        [m_frameNSView addSubview:m_cocoaNSView];
    }
    NSRect frameRect = [m_frameNSView frame];
    float tbarheight = 0.0;
#if wxUSE_TOOLBAR
    if(m_frameToolBar)
    {
        NSView *tbarNSView = m_frameToolBar->GetNSViewForSuperview();
        if(![tbarNSView superview])
            [m_frameNSView addSubview: tbarNSView];
        NSRect tbarRect = [tbarNSView frame];
        tbarRect.size.width = frameRect.size.width;
        tbarRect.origin.x = 0.0;
        tbarRect.origin.y = frameRect.size.height - tbarRect.size.height;
        [tbarNSView setFrame:tbarRect];
        // width expands, bottom margin expands
        [tbarNSView setAutoresizingMask: NSViewWidthSizable|NSViewMinYMargin];
        tbarheight = tbarRect.size.height;
    }
#endif //wxUSE_TOOLBAR
    float sbarheight = 0.0;
    if(m_frameStatusBar)
    {
        NSView *sbarNSView = m_frameStatusBar->GetNSViewForSuperview();
        if(![sbarNSView superview])
            [m_frameNSView addSubview: sbarNSView];
        NSRect sbarRect = [sbarNSView frame];
        sbarRect.size.width = frameRect.size.width;
        sbarRect.origin.x = 0.0;
        sbarRect.origin.y = 0.0;
        [sbarNSView setFrame:sbarRect];
        // width expands, top margin expands
        [sbarNSView setAutoresizingMask: NSViewWidthSizable|NSViewMaxYMargin];
        sbarheight = sbarRect.size.height;
    }
    wxLogDebug("frame height=%f, tbar=%f, sbar=%f",frameRect.size.height,tbarheight,sbarheight);
    NSRect innerRect = [m_cocoaNSView frame];
    innerRect.size.height = frameRect.size.height - tbarheight - sbarheight;
    innerRect.origin.y = sbarheight;
    [m_cocoaNSView setFrame:innerRect];
    [m_cocoaNSView setAutoresizingMask: NSViewWidthSizable|NSViewHeightSizable];
    // Don't let the frame get smaller than the toolbar+statusbar height
    NSRect frameMinRect = [NSWindow frameRectForContentRect:
            NSMakeRect(0.0,0.0,0.0,tbarheight+sbarheight)
        styleMask: [m_cocoaNSWindow styleMask]];
    [m_cocoaNSWindow setMinSize:frameMinRect.size];
}

void wxFrame::SetStatusBar(wxStatusBar *statusbar)
{
    if(m_frameStatusBar)
    {
        [m_frameStatusBar->GetNSViewForSuperview() removeFromSuperview];
        [m_frameStatusBar->GetNSViewForSuperview() setAutoresizingMask: NSViewMinYMargin];
        if(m_frameStatusBar->GetParent())
            m_frameStatusBar->GetParent()->CocoaAddChild(m_frameStatusBar);
    }
    m_frameStatusBar = statusbar;
    if(m_frameStatusBar)
    {
        m_frameStatusBar->CocoaRemoveFromParent();
    }
    UpdateFrameNSView();
}

wxStatusBar* wxFrame::CreateStatusBar(int number,
                                          long style,
                                          wxWindowID winid,
                                          const wxString& name)
{
    wxFrameBase::CreateStatusBar(number,style,winid,name);
    if(m_frameStatusBar)
    {
        m_frameStatusBar->CocoaRemoveFromParent();
    }
    UpdateFrameNSView();
    return m_frameStatusBar;
}

#if wxUSE_TOOLBAR
void wxFrame::SetToolBar(wxToolBar *toolbar)
{
    if(m_frameToolBar)
    {
        [m_frameToolBar->GetNSViewForSuperview() removeFromSuperview];
        [m_frameToolBar->GetNSViewForSuperview() setAutoresizingMask: NSViewMinYMargin];
        if(m_frameToolBar->GetParent())
            m_frameToolBar->GetParent()->CocoaAddChild(m_frameToolBar);
    }
    m_frameToolBar = toolbar;
    if(m_frameToolBar)
    {
        m_frameToolBar->CocoaRemoveFromParent();
    }
    UpdateFrameNSView();
}

wxToolBar* wxFrame::CreateToolBar(long style,
                                      wxWindowID winid,
                                      const wxString& name)
{
    wxFrameBase::CreateToolBar(style,winid,name);
    if(m_frameToolBar)
    {
        m_frameToolBar->CocoaRemoveFromParent();
    }
    UpdateFrameNSView();
    return m_frameToolBar;
}
#endif // wxUSE_TOOLBAR

void wxFrame::PositionStatusBar()
{
}

