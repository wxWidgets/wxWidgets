/////////////////////////////////////////////////////////////////////////////
// Name:        cocoa/checkbox.mm
// Purpose:     wxCheckBox
// Author:      David Elliott
// Modified by:
// Created:     2003/03/16
// RCS-ID:      $Id: 
// Copyright:   (c) 2003 David Elliott
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#include "wx/app.h"
#include "wx/checkbox.h"
#include "wx/log.h"

#import <AppKit/NSButton.h>
#import <Foundation/NSString.h>

IMPLEMENT_DYNAMIC_CLASS(wxCheckBox, wxControl)
BEGIN_EVENT_TABLE(wxCheckBox, wxCheckBoxBase)
END_EVENT_TABLE()
WX_IMPLEMENT_COCOA_OWNER(wxCheckBox,NSButton,NSControl,NSView)

bool wxCheckBox::Create(wxWindow *parent, wxWindowID winid,
           const wxString& label,
           const wxPoint& pos,
           const wxSize& size,
           long style,
           const wxValidator& validator,
           const wxString& name)
{
    if(!CreateControl(parent,winid,pos,size,style,validator,name))
        return false;
    m_cocoaNSView = NULL;
    NSRect cocoaRect = NSMakeRect(10,10,20,20);
    SetNSButton([[NSButton alloc] initWithFrame: cocoaRect]);
    [m_cocoaNSView release];
    [GetNSButton() setButtonType: NSSwitchButton];
    [GetNSButton() setTitle:[NSString stringWithCString: label.c_str()]];
    [GetNSControl() sizeToFit];

    if(m_parent)
        m_parent->CocoaAddChild(this);
    return true;
}

wxCheckBox::~wxCheckBox()
{
    DisassociateNSButton(m_cocoaNSView);
}

void wxCheckBox::SetValue(bool)
{
}

bool wxCheckBox::GetValue() const
{
    return false;
}

void wxCheckBox::Cocoa_wxNSButtonAction(void)
{
    wxLogDebug("Checkbox");
}

