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

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
    #include "wx/log.h"
    #include "wx/app.h"
    #include "wx/checkbox.h"
#endif //WX_PRECOMP

#include "wx/cocoa/autorelease.h"

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
    wxAutoNSAutoreleasePool pool;
    if(!CreateControl(parent,winid,pos,size,style,validator,name))
        return false;
    m_cocoaNSView = NULL;
    SetNSButton([[NSButton alloc] initWithFrame: MakeDefaultNSRect(size)]);
    [m_cocoaNSView release];
    [GetNSButton() setButtonType: NSSwitchButton];
    [GetNSButton() setTitle:[NSString stringWithCString: label.c_str()]];
    [GetNSControl() sizeToFit];

    if(m_parent)
        m_parent->CocoaAddChild(this);
    SetInitialFrameRect(pos,size);

    return true;
}

wxCheckBox::~wxCheckBox()
{
    DisassociateNSButton(m_cocoaNSView);
}

void wxCheckBox::SetValue(bool value)
{
    if(value)
        [GetNSButton() setState: NSOnState];
    else
        [GetNSButton() setState: NSOffState];
}

bool wxCheckBox::GetValue() const
{
    int state = [GetNSButton() state];
    wxASSERT(state!=NSMixedState);
    return state==NSOnState;
}

void wxCheckBox::Cocoa_wxNSButtonAction(void)
{
    wxLogDebug("Checkbox");
    wxCommandEvent event(wxEVT_COMMAND_CHECKBOX_CLICKED, GetId());
    InitCommandEvent(event); //    event.SetEventObject(this);
    event.SetInt(GetValue());
    Command(event);
}

