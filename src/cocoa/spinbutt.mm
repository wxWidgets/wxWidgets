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

#include "wx/wxprec.h"
#if wxUSE_SPINBTN

#ifndef WX_PRECOMP
    #include "wx/app.h"
#endif //WX_PRECOMP
#include "wx/spinbutt.h"

#import <AppKit/NSStepper.h>

IMPLEMENT_DYNAMIC_CLASS(wxSpinEvent, wxNotifyEvent)

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
    SetNSControl([[NSStepper alloc] initWithFrame: MakeDefaultNSRect(size)]);
    [m_cocoaNSView release];
    if(m_parent)
        m_parent->CocoaAddChild(this);
    SetInitialFrameRect(pos,size);

    return true;
}

wxSpinButton::~wxSpinButton()
{
}

#endif // wxUSE_SPINBTN
