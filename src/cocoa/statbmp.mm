/////////////////////////////////////////////////////////////////////////////
// Name:        cocoa/statbmp.mm
// Purpose:     wxStaticBitmap
// Author:      David Elliott
// Modified by:
// Created:     2003/02/15
// RCS-ID:      $Id: 
// Copyright:   (c) 2003 David Elliott
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#include "wx/app.h"
#include "wx/statbmp.h"

#import <AppKit/NSView.h>

IMPLEMENT_DYNAMIC_CLASS(wxStaticBitmap, wxControl)
BEGIN_EVENT_TABLE(wxStaticBitmap, wxControl)
END_EVENT_TABLE()
// WX_IMPLEMENT_COCOA_OWNER(wxStaticBitmap,NSTextField,NSControl,NSView)

bool wxStaticBitmap::Create(wxWindow *parent, wxWindowID winid,
           const wxBitmap& bitmap,
           const wxPoint& pos,
           const wxSize& size,
           long style,
           const wxString& name)
{
    if(!CreateControl(parent,winid,pos,size,style,wxDefaultValidator,name))
        return false;
    m_cocoaNSView = NULL;
    SetNSView([[NSView alloc] initWithFrame: NSMakeRect(10,10,20,20)]);
    [m_cocoaNSView release];
    if(m_parent)
        m_parent->CocoaAddChild(this);
    return true;
}

wxStaticBitmap::~wxStaticBitmap()
{
}

void wxStaticBitmap::SetIcon(const wxIcon& icon)
{
}

void wxStaticBitmap::SetBitmap(const wxBitmap& bitmap)
{
}

wxBitmap wxStaticBitmap::GetBitmap() const
{
    return wxNullBitmap;
}

