/////////////////////////////////////////////////////////////////////////////
// Name:        cocoa/button.mm
// Purpose:     wxButton
// Author:      David Elliott
// Modified by:
// Created:     2002/12/30
// RCS-ID:      $Id: 
// Copyright:   (c) 2002 David Elliott
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
    #include "wx/defs.h"
    #include "wx/button.h"
    #include "wx/log.h"
#endif

#include "wx/cocoa/autorelease.h"

#import <AppKit/NSButton.h>
#include "wx/cocoa/string.h"

IMPLEMENT_DYNAMIC_CLASS(wxButton, wxControl)
BEGIN_EVENT_TABLE(wxButton, wxButtonBase)
END_EVENT_TABLE()
WX_IMPLEMENT_COCOA_OWNER(wxButton,NSButton,NSControl,NSView)

bool wxButton::Create(wxWindow *parent, wxWindowID winid,
            const wxString& label, const wxPoint& pos,
            const wxSize& size, long style,
            const wxValidator& validator, const wxString& name)
{
    wxAutoNSAutoreleasePool pool;
    wxLogDebug("Creating control with id=%d",winid);
    if(!CreateControl(parent,winid,pos,size,style,validator,name))
        return false;
    wxLogDebug("Created control with id=%d",GetId());
    NSRect cocoaRect = NSMakeRect(10,10,20,20);
    m_cocoaNSView = NULL;
    SetNSButton([[NSButton alloc] initWithFrame: cocoaRect]);
    // NOTE: YES we want to release this (to match the alloc).
    // DoAddChild(this) will retain us again since addSubView doesn't.
    [m_cocoaNSView release];

    [GetNSButton() setBezelStyle:NSRoundedBezelStyle];
    [GetNSButton() setTitle:wxNSStringWithWxString(label)];
    [GetNSControl() sizeToFit];

    if(m_parent)
        m_parent->CocoaAddChild(this);

    return true;
}

wxButton::~wxButton()
{
    DisassociateNSButton(m_cocoaNSView);
}

void wxButton::Cocoa_wxNSButtonAction(void)
{
    wxLogDebug("YAY!");
    wxCommandEvent event(wxEVT_COMMAND_BUTTON_CLICKED, GetId());
    InitCommandEvent(event); //    event.SetEventObject(this);
    Command(event);
}

wxString wxButton::GetLabel() const
{
    return wxString([[GetNSButton() title] lossyCString]);
}

void wxButton::SetLabel(const wxString& label)
{
    [GetNSButton() setTitle:wxNSStringWithWxString(label)];
}

wxSize wxButtonBase::GetDefaultSize()
{
    // FIXME: stub
    return wxDefaultSize;
}

