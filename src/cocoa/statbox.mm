/////////////////////////////////////////////////////////////////////////////
// Name:        cocoa/statbox.mm
// Purpose:     wxStaticBox
// Author:      David Elliott
// Modified by:
// Created:     2003/02/15
// RCS-ID:      $Id: 
// Copyright:   (c) 2003 David Elliott
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#include "wx/app.h"
#include "wx/statbox.h"

#import <AppKit/NSBox.h>
#import <Foundation/NSString.h>

IMPLEMENT_DYNAMIC_CLASS(wxStaticBox, wxControl)
BEGIN_EVENT_TABLE(wxStaticBox, wxStaticBoxBase)
END_EVENT_TABLE()
WX_IMPLEMENT_COCOA_OWNER(wxStaticBox,NSBox,NSView,NSView)

bool wxStaticBox::Create(wxWindow *parent, wxWindowID winid,
           const wxString& title,
           const wxPoint& pos,
           const wxSize& size,
           long style, const wxValidator& validator,
           const wxString& name)
{
    if(!CreateControl(parent,winid,pos,size,style,validator,name))
        return false;
    m_cocoaNSView = NULL;
    SetNSBox([[NSBox alloc] initWithFrame:NSMakeRect(0,0,30,30)]);
    [GetNSBox() setTitle:[NSString stringWithCString:title.c_str()]];
    if(m_parent)
        m_parent->CocoaAddChild(this);
    return true;
}

wxStaticBox::~wxStaticBox()
{
    CocoaRemoveFromParent();
    SetNSBox(NULL);
}

void wxStaticBox::GetBordersForSizer(int *borderTop, int *borderOther) const
{
    NSRect contentRect = [[GetNSBox() contentView] frame];
    NSRect thisRect = [m_cocoaNSView frame];
    *borderTop = (int)(thisRect.size.height - (contentRect.origin.y+contentRect.size.height));
    *borderOther = (int)(thisRect.size.width - (contentRect.origin.x+contentRect.size.width));
    int nextBorder = (int)contentRect.origin.y;
    if(nextBorder > *borderOther)
        *borderOther = nextBorder;
    nextBorder = (int)contentRect.origin.x;
    if(nextBorder > *borderOther)
        *borderOther = nextBorder;
}

