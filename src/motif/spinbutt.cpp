/////////////////////////////////////////////////////////////////////////////
// Name:        spinbutt.cpp
// Purpose:     wxSpinButton
// Author:      Julian Smart
// Modified by:
// Created:     17/09/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "spinbutt.h"
#endif

#include "wx/spinbutt.h"

IMPLEMENT_DYNAMIC_CLASS(wxSpinButton, wxControl)
IMPLEMENT_DYNAMIC_CLASS(wxSpinEvent, wxNotifyEvent)

bool wxSpinButton::Create(wxWindow *parent, wxWindowID id, const wxPoint& WXUNUSED(pos), const wxSize& WXUNUSED(size),
            long style, const wxString& name)
{
    SetName(name);

    m_windowStyle = style;

    if (parent) parent->AddChild(this);

    InitBase();

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

void wxSpinButton::SetValue(int WXUNUSED(val))
{
    // TODO
}

void wxSpinButton::SetRange(int minVal, int maxVal)
{
    // TODO
    wxSpinButtonBase::SetRange(minVal, maxVal);
}

void wxSpinButton::ChangeFont(bool WXUNUSED(keepOriginalSize))
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
