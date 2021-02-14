/////////////////////////////////////////////////////////////////////////////
// Name:        src/univ/choice.cpp
// Purpose:     wxChoice implementation
// Author:      Vadim Zeitlin
// Modified by:
// Created:     15.12.00
// Copyright:   (c) 2000 SciTech Software, Inc. (www.scitechsoft.com)
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "wx/wxprec.h"


#if wxUSE_CHOICE

#include "wx/choice.h"

#ifndef WX_PRECOMP
    #include "wx/arrstr.h"
#endif

wxBEGIN_EVENT_TABLE(wxChoice, wxComboBox)
    EVT_COMBOBOX(wxID_ANY, wxChoice::OnComboBox)
wxEND_EVENT_TABLE()

wxChoice::wxChoice(wxWindow *parent, wxWindowID id,
                   const wxPoint& pos,
                   const wxSize& size,
                   const wxArrayString& choices,
                   long style,
                   const wxValidator& validator,
                   const wxString& name)
{
    Create(parent, id, pos, size, choices, style, validator, name);
}

bool wxChoice::Create(wxWindow *parent, wxWindowID id,
                      const wxPoint& pos,
                      const wxSize& size,
                      const wxArrayString& choices,
                      long style,
                      const wxValidator& validator,
                      const wxString& name)
{
    wxCArrayString chs(choices);

    return Create(parent, id, pos, size, chs.GetCount(), chs.GetStrings(),
                  style, validator, name);
}

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
                                 wxCB_READONLY | style, validator, name);
}


void wxChoice::OnComboBox(wxCommandEvent& event)
{
    if ( event.GetId() == GetId() )
    {
        event.SetEventType(wxEVT_CHOICE);
        event.Skip();
        GetEventHandler()->ProcessEvent(event);
    }
    else
        event.Skip();
}

#endif // wxUSE_CHOICE
