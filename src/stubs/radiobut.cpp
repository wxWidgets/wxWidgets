/////////////////////////////////////////////////////////////////////////////
// Name:        radiobut.cpp
// Purpose:     wxRadioButton
// Author:      AUTHOR
// Modified by:
// Created:     ??/??/98
// RCS-ID:      $Id$
// Copyright:   (c) AUTHOR
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "radiobut.h"
#endif

#include "wx/radiobut.h"

#if !USE_SHARED_LIBRARY
IMPLEMENT_DYNAMIC_CLASS(wxRadioButton, wxControl)
#endif

bool wxRadioButton::Create(wxWindow *parent, wxWindowID id,
		   const wxString& label,
           const wxPoint& pos,
           const wxSize& size, long style,
           const wxValidator& validator,
           const wxString& name)
{
    SetName(name);
    SetValidator(validator);

    if (parent) parent->AddChild(this);

    if ( id == -1 )
  	    m_windowId = (int)NewControlId();
    else
	    m_windowId = id;

    m_windowStyle = style ;

    // TODO create radiobutton
    return FALSE;
}

void wxRadioButton::SetLabel(const wxString& label)
{
    // TODO
}

void wxRadioButton::SetValue(bool value)
{
    // TODO
}

// Get single selection, for single choice list items
bool wxRadioButton::GetValue() const
{
    // TODO
    return FALSE;
}

void wxRadioButton::Command (wxCommandEvent & event)
{
  SetValue ( (event.m_commandInt != 0) );
  ProcessCommand (event);
}


