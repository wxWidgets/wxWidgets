/////////////////////////////////////////////////////////////////////////////
// Name:        spinbutt.cpp
// Purpose:     wxSpinButton
// Author:      Julian Smart
// Modified by:
// Created:     17/09/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "spinbutt.h"
#endif

#include "wx/spinbutt.h"

#if !USE_SHARED_LIBRARY
IMPLEMENT_DYNAMIC_CLASS(wxSpinButton, wxControl)
#endif

wxSpinButton::wxSpinButton()
{
	m_min = 0;
	m_max = 100;
}

bool wxSpinButton::Create(wxWindow *parent, wxWindowID id, const wxPoint& pos, const wxSize& size,
            long style, const wxString& name)
{
    SetName(name);

    m_windowStyle = style;

    SetParent(parent);

    m_min = 0;
    m_max = 100;

    m_windowId = (id == -1) ? NewControlId() : id;

    // TODO create spin button
    return FALSE;
}

wxSpinButton::~wxSpinButton()
{
}

// Attributes
////////////////////////////////////////////////////////////////////////////

int wxSpinButton::GetValue() const
{
	// TODO
    return 0;
}

void wxSpinButton::SetValue(int val)
{
	// TODO
}

void wxSpinButton::SetRange(int minVal, int maxVal)
{
	m_min = minVal;
	m_max = maxVal;
	// TODO
}

void wxSpinButton::ChangeFont(bool keepOriginalSize)
{
    // TODO
}

void wxSpinButton::ChangeBackgroundColour()
{
    // TODO
}

void wxSpinButton::ChangeForegroundColour()
{
    // TODO
}

// Spin event
IMPLEMENT_DYNAMIC_CLASS(wxSpinEvent, wxScrollEvent)

wxSpinEvent::wxSpinEvent(wxEventType commandType, int id):
  wxScrollEvent(commandType, id)
{
}

