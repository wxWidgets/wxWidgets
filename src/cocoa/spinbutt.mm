/////////////////////////////////////////////////////////////////////////////
// Name:        cocoa/spinbutt.mm
// Purpose:     wxSpinButton
// Author:      David Elliott
// Modified by:
// Created:     2003/07/14
// RCS-ID:      $Id$
// Copyright:   (c) 2003 David Elliott
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/setup.h"
#if wxUSE_SPINBTN

#include "wx/app.h"
#include "wx/spinbutt.h"

#import <AppKit/NSStepper.h>

IMPLEMENT_DYNAMIC_CLASS(wxSpinButton, wxControl)
BEGIN_EVENT_TABLE(wxSpinButton, wxSpinButtonBase)
END_EVENT_TABLE()
// WX_IMPLEMENT_COCOA_OWNER(wxSpinButton,NSStepper,NSControl,NSView)

bool wxSpinButton::Create(wxWindow *parent, wxWindowID winid,
            const wxPoint& pos, const wxSize& size, long style,
            const wxString& name)
{
    if(!CreateControl(parent,winid,pos,size,style,wxDefaultValidator,name))
        return false;
    SetNSControl([[NSStepper alloc] initWithFrame: NSMakeRect(10,10,20,20)]);
    [m_cocoaNSView release];
    if(m_parent)
        m_parent->CocoaAddChild(this);
    return true;
}

wxSpinButton::~wxSpinButton()
{
}

#endif // wxUSE_SPINBTN
