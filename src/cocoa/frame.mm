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

#include "wx/frame.h"
#include "wx/menu.h"
#include "wx/menuitem.h"
#include "wx/app.h"
#include "wx/log.h"
#include "wx/statusbr.h"

#import <AppKit/NSWindow.h>
#import <AppKit/NSApplication.h>

// wxFrame

BEGIN_EVENT_TABLE(wxFrame, wxFrameBase)
END_EVENT_TABLE()

IMPLEMENT_DYNAMIC_CLASS(wxFrame, wxTopLevelWindow)

void wxFrame::Init()
{
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
}

void wxFrame::Cocoa_wxMenuItemAction(wxMenuItem& item)
{
    Command(item.GetId());
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
    bool ret = wxFrameBase::Show(show);
    if(show && GetMenuBar())
        [wxTheApp->GetNSApplication() setMenu:GetMenuBar()->GetNSMenu() ];
    return ret;
}

void wxFrame::Cocoa_FrameChanged(void)
{
    PositionStatusBar();
    wxFrameBase::Cocoa_FrameChanged();
}

wxPoint wxFrame::GetClientAreaOrigin() const
{
    return wxPoint(0,0);
}

void wxFrame::DoGetClientSize(int *width, int *height) const
{
    wxFrameBase::DoGetClientSize(width,height);
    if(height)
    {
        if(m_frameStatusBar && m_frameStatusBar->IsShown())
            *height -= m_frameStatusBar->GetSize().y;
    }
}

void wxFrame::DoSetClientSize(int width, int height)
{
    if(m_frameStatusBar && m_frameStatusBar->IsShown())
        height += m_frameStatusBar->GetSize().y;
    wxFrameBase::DoSetClientSize(width,height);
}

void wxFrame::PositionStatusBar()
{
    if( !m_frameStatusBar || !m_frameStatusBar->IsShown() )
        return;

    // Get the client size.  Since it excludes the StatusBar area we want
    // the top of the status bar to be directly under it (thus located at h)
    // The width of the statusbar should then match the client width
    int w, h;
    GetClientSize(&w, &h);

    int sh;
    m_frameStatusBar->GetSize(NULL, &sh);

    m_frameStatusBar->SetSize(0, h, w, sh);
}

