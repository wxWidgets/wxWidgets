/////////////////////////////////////////////////////////////////////////////
// Name:        wx/generic/spinctlg.h
// Purpose:     generic wxSpinCtrl class
// Author:      Vadim Zeitlin
// Modified by:
// Created:     28.10.99
// RCS-ID:      $Id$
// Copyright:   (c) Vadim Zeitlin
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GENERIC_SPINCTRL_H_
#define _WX_GENERIC_SPINCTRL_H_

#include "wx/textctrl.h"

// ----------------------------------------------------------------------------
// generic wxSpinCtrl is just a text control
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxSpinCtrl : public wxTextCtrl
{
public:
    wxSpinCtrlBase() { Init(); }

    // accessors
    int GetValue() const
    {
        int n;
        if ( (wxSscanf(wxTextCtrl::GetValue(), wxT("%d"), &n) != 1) )
            n = INT_MIN;

        return n;
    }

    int GetMin() const { return m_min; }
    int GetMax() const { return m_max; }

    // operations
    void SetValue(const wxString& value) { wxTextCtrl::SetValue(value); }
    void SetValue(int val) { wxString s; s << val; wxTextCtrl::SetValue(s); }
    void SetRange(int min, int max) { m_min = min; m_max = max; }

protected:
    // initialize m_min/max with the default values
    void Init() { SetRange(0, 100); }

    int   m_min;
    int   m_max;
};

#endif // _WX_GENERIC_SPINCTRL_H_

