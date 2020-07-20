/////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/radiobut.h
// Purpose:     wxRadioButton class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_RADIOBUT_H_
#define _WX_RADIOBUT_H_

#include "wx/msw/ownerdrawnbutton.h"

class WXDLLIMPEXP_CORE wxRadioButton : public wxMSWOwnerDrawnButton<wxControl>
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
                  const wxString& name = wxASCII_STR(wxRadioButtonNameStr))
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
                const wxString& name = wxASCII_STR(wxRadioButtonNameStr));

    // implement the radio button interface
    virtual void SetValue(bool value);
    virtual bool GetValue() const;

    // implementation only from now on
    virtual bool MSWCommand(WXUINT param, WXWORD id) wxOVERRIDE;
    virtual void Command(wxCommandEvent& event) wxOVERRIDE;

    virtual bool HasTransparentBackground() wxOVERRIDE { return true; }

    virtual WXDWORD MSWGetStyle(long style, WXDWORD *exstyle) const wxOVERRIDE;

protected:
    virtual wxBorder GetDefaultBorder() const wxOVERRIDE { return wxBORDER_NONE; }
    virtual wxSize DoGetBestSize() const wxOVERRIDE;

    // Implement wxMSWOwnerDrawnButtonBase methods.
    virtual int MSWGetButtonStyle() const wxOVERRIDE;
    virtual void MSWOnButtonResetOwnerDrawn() wxOVERRIDE;
    virtual int MSWGetButtonCheckedFlag() const wxOVERRIDE;
    virtual void
        MSWDrawButtonBitmap(wxDC& dc, const wxRect& rect, int flags) wxOVERRIDE;


private:
    // common part of all ctors
    void Init();


    // we need to store the state internally as the result of GetValue()
    // sometimes gets out of sync in WM_COMMAND handler
    bool m_isChecked;

    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxRadioButton);
};

#endif // _WX_RADIOBUT_H_
