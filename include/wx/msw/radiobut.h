/////////////////////////////////////////////////////////////////////////////
// Name:        radiobut.h
// Purpose:     wxRadioButton class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_RADIOBUT_H_
#define _WX_RADIOBUT_H_

#ifdef __GNUG__
    #pragma interface "radiobut.h"
#endif

class WXDLLEXPORT wxRadioButton: public wxControl
{
public:
    // ctors and creation functions
    wxRadioButton() { Init(); }

    wxRadioButton(wxWindow *parent,
                  wxWindowID id,
                  const wxString& label,
                  const wxPoint& pos = wxDefaultPosition,
                  const wxSize& size = wxDefaultSize,
                  long style = 0,
                  const wxValidator& validator = wxDefaultValidator,
                  const wxString& name = wxRadioButtonNameStr)
    {
        Init();

        Create(parent, id, label, pos, size, style, validator, name);
    }

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxString& label,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxRadioButtonNameStr);

    // implement the radio button interface
    virtual void SetValue(bool value);
    virtual bool GetValue() const;

    // implementation only from now on
    virtual bool MSWCommand(WXUINT param, WXWORD id);
    virtual void Command(wxCommandEvent& event);
    virtual long MSWWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam);

    virtual void SetFocus();

protected:
    virtual wxSize DoGetBestSize() const;

private:
    // common part of all ctors
    void Init();

    // see the comments in SetFocus()
    bool m_focusJustSet;

    DECLARE_DYNAMIC_CLASS(wxRadioButton)
};

#endif
    // _WX_RADIOBUT_H_
