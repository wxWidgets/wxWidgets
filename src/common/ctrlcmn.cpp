/////////////////////////////////////////////////////////////////////////////
// Name:        ctrlcmn.cpp
// Purpose:     wxControl common interface
// Author:      Vadim Zeitlin
// Modified by:
// Created:     26.07.99
// RCS-ID:      $Id$
// Copyright:   (c) wxWindows team
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "controlbase.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/control.h"
    #include "wx/log.h"
#endif

// ============================================================================
// implementation
// ============================================================================

bool wxControlBase::CreateControl(wxWindowBase *parent,
                                  wxWindowID id,
                                  const wxPoint& pos,
                                  const wxSize& size,
                                  long style,
#if wxUSE_VALIDATORS
                                  const wxValidator& validator,
#endif
                                  const wxString& name)
{
    // even if it's possible to create controls without parents in some port,
    // it should surely be discouraged because it doesn't work at all under
    // Windows
    wxCHECK_MSG( parent, FALSE, wxT("all controls must have parents") );

    if ( !CreateBase(parent, id, pos, size, style, validator, name) )
        return FALSE;

    parent->AddChild(this);

    return TRUE;
}

// inherit colour and font settings from the parent window
void wxControlBase::InheritAttributes()
{
    SetBackgroundColour(GetParent()->GetBackgroundColour());
    SetForegroundColour(GetParent()->GetForegroundColour());
    SetFont(GetParent()->GetFont());
}

void wxControlBase::Command(wxCommandEvent& event)
{
    (void)ProcessEvent(event);
}
