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

wxPoint wxFrame::GetClientAreaOrigin() const
{
    return wxPoint(0,0);
}

