/////////////////////////////////////////////////////////////////////////////
// Name:        spinbutt.h
// Purpose:     wxSpinButton class
// Author:      Julian Smart
// Modified by:
// Created:     17/09/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_SPINBUTT_H_
#define _WX_SPINBUTT_H_

#ifdef __GNUG__
    #pragma interface "spinbutt.h"
#endif

class WXDLLEXPORT wxSpinButton : public wxSpinButtonBase
{
    DECLARE_DYNAMIC_CLASS(wxSpinButton)

public:
    wxSpinButton() { }

    wxSpinButton(wxWindow *parent,
                 wxWindowID id = -1,
                 const wxPoint& pos = wxDefaultPosition,
                 const wxSize& size = wxDefaultSize,
                 long style = wxSP_VERTICAL,
                 const wxString& name = "wxSpinButton")
    {
        Create(parent, id, pos, size, style, name);
    }
    ~wxSpinButton();

    bool Create(wxWindow *parent,
                wxWindowID id = -1,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxSP_VERTICAL,
                const wxString& name = "wxSpinButton");


    // accessors
    int GetValue() const;
    int GetMin() const { return m_min; }
    int GetMax() const { return m_max; }

    // operations
    void SetValue(int val);
    void SetRange(int minVal, int maxVal);

    // Implementation
    virtual void Command(wxCommandEvent& event) { (void)ProcessCommand(event); };
    virtual void ChangeFont(bool keepOriginalSize = TRUE);
    virtual void ChangeBackgroundColour();
    virtual void ChangeForegroundColour();

protected:
    int   m_min;
    int   m_max;
};

#endif
    // _WX_SPINBUTT_H_
