/////////////////////////////////////////////////////////////////////////////
// Name:        cocoa/radiobut.mm
// Purpose:     wxRadioButton
// Author:      David Elliott
// Modified by:
// Created:     2003/03/16
// RCS-ID:      $Id: 
// Copyright:   (c) 2003 David Elliott
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#include "wx/app.h"
#include "wx/radiobut.h"
#include "wx/log.h"

#import <AppKit/NSButton.h>
#include "wx/cocoa/string.h"

IMPLEMENT_DYNAMIC_CLASS(wxRadioButton, wxControl)
// wxRadioButtonBase == wxControl
BEGIN_EVENT_TABLE(wxRadioButton, wxControl)
END_EVENT_TABLE()
WX_IMPLEMENT_COCOA_OWNER(wxRadioButton,NSButton,NSControl,NSView)

bool wxRadioButton::Create(wxWindow *parent, wxWindowID winid,
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
    [GetNSButton() setButtonType: NSRadioButton];
    [GetNSButton() setTitle:wxNSStringWithWxString(label)];
    [GetNSControl() sizeToFit];

    if(m_parent)
        m_parent->CocoaAddChild(this);
    return true;
}

wxRadioButton::~wxRadioButton()
{
    CocoaRemoveFromParent();
    SetNSButton(NULL);
}

void wxRadioButton::SetValue(bool)
{
}

bool wxRadioButton::GetValue() const
{
    return false;
}

void wxRadioButton::Cocoa_wxNSButtonAction(void)
{
    wxLogDebug("wxRadioButton");
}

