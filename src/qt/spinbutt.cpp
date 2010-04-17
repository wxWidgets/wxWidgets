/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/spinbutt.cpp
// Author:      Peter Most
// Id:          $Id$
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/spinbutt.h"

IMPLEMENT_DYNAMIC_CLASS(wxSpinEvent, wxNotifyEvent)

wxSpinButton::wxSpinButton()
{
}

wxSpinButton::wxSpinButton(wxWindow *parent,
             wxWindowID id,
             const wxPoint& pos,
             const wxSize& size,
             long style,
             const wxString& name)
{
}

bool wxSpinButton::Create(wxWindow *parent,
            wxWindowID id,
            const wxPoint& pos,
            const wxSize& size,
            long style,
            const wxString& name)
{
    return false;
}

int wxSpinButton::GetValue() const
{
    return 0;
}

void wxSpinButton::SetValue(int val)
{
}

