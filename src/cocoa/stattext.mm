/////////////////////////////////////////////////////////////////////////////
// Name:        cocoa/stattext.mm
// Purpose:     wxStaticText
// Author:      David Elliott
// Modified by:
// Created:     2003/02/15
// RCS-ID:      $Id: 
// Copyright:   (c) 2003 David Elliott
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#include "wx/app.h"
#include "wx/stattext.h"

#import <Foundation/NSString.h>
#import <AppKit/NSTextField.h>

IMPLEMENT_DYNAMIC_CLASS(wxStaticText, wxControl)
BEGIN_EVENT_TABLE(wxStaticText, wxControl)
END_EVENT_TABLE()
WX_IMPLEMENT_COCOA_OWNER(wxStaticText,NSTextField,NSControl,NSView)

bool wxStaticText::Create(wxWindow *parent, wxWindowID winid,
           const wxString& label,
           const wxPoint& pos,
           const wxSize& size,
           long style,
           const wxString& name)
{
    if(!CreateControl(parent,winid,pos,size,style,wxDefaultValidator,name))
        return false;
    m_cocoaNSView = NULL;
    SetNSTextField([[NSTextField alloc] initWithFrame:NSMakeRect(0,0,30,30)]);
    [m_cocoaNSView release];
    [GetNSTextField() setStringValue:[NSString stringWithCString:label.c_str()]];
//    [GetNSTextField() setBordered: NO];
    [GetNSTextField() setBezeled: NO];
    [GetNSTextField() setEditable: NO];
    [GetNSTextField() setDrawsBackground: NO];
    [GetNSControl() sizeToFit];
    if(m_parent)
        m_parent->CocoaAddChild(this);
    return true;
}

wxStaticText::~wxStaticText()
{
    CocoaRemoveFromParent();
    SetNSTextField(NULL);
}

void wxStaticText::SetLabel(const wxString& label)
{
    // TODO
}

void wxStaticText::Cocoa_didChangeText(void)
{
}

