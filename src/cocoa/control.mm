/////////////////////////////////////////////////////////////////////////////
// Name:        cocoa/control.mm
// Purpose:     wxControl class
// Author:      David Elliiott
// Modified by:
// Created:     2003/02/15
// RCS-ID:      $Id: 
// Copyright:   (c) 2003 David Elliott
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
    #include "wx/defs.h"
    #include "wx/control.h"
    #include "wx/log.h"
#endif

#import <AppKit/NSControl.h>

IMPLEMENT_ABSTRACT_CLASS(wxControl, wxWindow)
BEGIN_EVENT_TABLE(wxControl, wxControlBase)
END_EVENT_TABLE()
WX_IMPLEMENT_COCOA_OWNER(wxControl,NSControl,NSView,NSView)

bool wxControl::Create(wxWindow *parent, wxWindowID winid,
            const wxPoint& pos, const wxSize& size, long style,
            const wxValidator& validator, const wxString& name)
{
    wxLogDebug("Creating control with id=%d",winid);
    if(!CreateControl(parent,winid,pos,size,style,validator,name))
        return false;
    wxLogDebug("Created control with id=%d",GetId());
    NSRect cocoaRect = NSMakeRect(10,10,20,20);
    m_cocoaNSView = NULL;
    SetNSControl([[NSControl alloc] initWithFrame: cocoaRect]);
    // NOTE: YES we want to release this (to match the alloc).
    // DoAddChild(this) will retain us again since addSubView doesn't.
    [m_cocoaNSView release];

    [GetNSControl() sizeToFit];

    if(m_parent)
        m_parent->CocoaAddChild(this);

    return true;
}

wxControl::~wxControl()
{
    CocoaRemoveFromParent();
    SetNSControl(NULL);
}

wxSize wxControl::DoGetBestSize() const
{
    wxASSERT(m_cocoaNSView);
    NSRect storedRect = [m_cocoaNSView frame];
    [GetNSControl() sizeToFit];
    NSRect cocoaRect = [m_cocoaNSView frame];
    wxSize size((int)cocoaRect.size.width+10,(int)cocoaRect.size.height);
    [m_cocoaNSView setFrame: storedRect];
    wxLogDebug("wxControl=%p::DoGetBestSize()==(%d,%d)",this,size.x,size.y);
    return size;
}

bool wxControl::ProcessCommand(wxCommandEvent& event)
{
#if WXWIN_COMPATIBILITY
    if ( m_callback )
    {
        (void)(*m_callback)(*this, event);

        return TRUE;
    }
    else
#endif // WXWIN_COMPATIBILITY

    return GetEventHandler()->ProcessEvent(event);
}

