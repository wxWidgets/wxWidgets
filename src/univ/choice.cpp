/////////////////////////////////////////////////////////////////////////////
// Name:        univ/choice.cpp
// Purpose:     wxChoice implementation
// Author:      Vadim Zeitlin
// Modified by:
// Created:     15.12.00
// RCS-ID:      $Id$
// Copyright:   (c) 2000 SciTech Software, Inc. (www.scitechsoft.com)
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "univchoice.h"
#endif

#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_CHOICE

#ifndef WX_PRECOMP
    #include "wx/choice.h"
#endif

IMPLEMENT_DYNAMIC_CLASS(wxChoice, wxControl)

BEGIN_EVENT_TABLE(wxChoice, wxComboBox)
    EVT_COMBOBOX(-1, wxChoice::OnComboBox)
END_EVENT_TABLE()

bool wxChoice::Create(wxWindow *parent, wxWindowID id,
                      const wxPoint& pos,
                      const wxSize& size,
                      int n, const wxString choices[],
                      long style,
                      const wxValidator& validator,
                      const wxString& name)
{
    wxString value;
    if ( n )
        value = choices[0];
    return wxComboBox::Create(parent, id, value,
                                 pos, size, n, choices,
                                 wxCB_READONLY, validator, name);
}


void wxChoice::OnComboBox(wxCommandEvent& event)
{
    if ( event.GetId() == GetId() )
    {
        event.SetEventType(wxEVT_COMMAND_CHOICE_SELECTED);
        event.Skip();
        GetEventHandler()->ProcessEvent(event);
    }
    else
        event.Skip();
}

#endif // wxUSE_CHOICE
