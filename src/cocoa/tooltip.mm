/////////////////////////////////////////////////////////////////////////////
// Name:        src/cocoa/tooltip.mm
// Purpose:     Cocoa tooltips
// Author:      Ryan Norton
// Modified by: 
// Created:     2004-10-03
// RCS-ID:      $Id$
// Copyright:   (c) Ryan Norton
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ===========================================================================
// declarations
// ===========================================================================

// ---------------------------------------------------------------------------
// headers
// ---------------------------------------------------------------------------

#include "wx/defs.h"

#if wxUSE_TOOLTIPS

#include "wx/window.h"
#include "wx/tooltip.h"

#include "wx/cocoa/autorelease.h"
#include "wx/cocoa/string.h"

#import <AppKit/NSView.h>

//-----------------------------------------------------------------------------
// wxToolTip
//-----------------------------------------------------------------------------

IMPLEMENT_ABSTRACT_CLASS(wxToolTip, wxObject)

wxToolTip::wxToolTip(const wxString &tip) : 
    m_text(tip), m_window(0) 
{
}

wxToolTip::~wxToolTip() 
{
}

void wxToolTip::SetTip(const wxString& tip)
{ 
    m_text = tip; 
}

const wxString& wxToolTip::GetTip() const 
{ 
    return m_text; 
}

// the window we're associated with
wxWindow *wxToolTip::GetWindow() const 
{ 
    return m_window; 
}

// enable or disable the tooltips globally
//static 
    void wxToolTip::Enable(bool flag) 
{
    //TODO
    wxFAIL_MSG(wxT("Not implemented"));
}

// set the delay after which the tooltip appears
//static
    void SetDelay(long milliseconds) 
{
    //TODO
    wxFAIL_MSG(wxT("Not implemented"));
}

void wxToolTip::SetWindow(wxWindow* window) 
{
    wxAutoNSAutoreleasePool pool;

    m_window = window;
    
    //set the tooltip - empty string means remove
    if (m_text.IsEmpty())
        [m_window->GetNSView() setToolTip:nil];        
    else
        [m_window->GetNSView() setToolTip:wxNSStringWithWxString(m_text)];    
}

#endif //wxUSE_TOOLTIPS
