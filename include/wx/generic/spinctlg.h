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

#ifdef __GNUG__
    #pragma interface "spinctlg.h"
#endif

#include "wx/textctrl.h"

// ----------------------------------------------------------------------------
// generic wxSpinCtrl is just a text control
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxSpinCtrl : public wxTextCtrl
{
public:
    wxSpinCtrl() { Init(); }

    wxSpinCtrl(wxWindow *parent,
               wxWindowID id = -1,
               const wxString& value = wxEmptyString,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               long style = wxSP_ARROW_KEYS,
               int min = 0, int max = 100, int initial = 0,
               const wxString& name = _T("wxSpinCtrl"))
    {
        Create(parent, id, value, pos, size, style, min, max, initial, name);
    }

    bool Create(wxWindow *parent,
                wxWindowID id = -1,
                const wxString& value = wxEmptyString,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxSP_ARROW_KEYS,
                int min = 0, int max = 100, int initial = 0,
                const wxString& name = _T("wxSpinCtrl"))
    {
        SetRange(min, max);

        bool ok = wxTextCtrl::Create(parent, id, value, pos, size, style,
                                     wxDefaultValidator, name);
        SetValue(initial);

        return ok;
    }

    // accessors
    int GetValue(int WXUNUSED(dummy) = 1) const
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

private:
    DECLARE_DYNAMIC_CLASS(wxSpinCtrl)
};

#endif // _WX_GENERIC_SPINCTRL_H_

