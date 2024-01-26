/////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/radiobut.h
// Purpose:     wxRadioButton class
// Author:      Julian Smart
// Created:     01/02/97
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_RADIOBUT_H_
#define _WX_RADIOBUT_H_

#include "wx/msw/ownerdrawnbutton.h"

class WXDLLIMPEXP_CORE wxRadioButton : public wxMSWOwnerDrawnButton<wxRadioButtonBase>
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
    virtual void SetValue(bool value) override;
    virtual bool GetValue() const override;

    // implementation only from now on
    virtual bool MSWCommand(WXUINT param, WXWORD id) override;
    virtual void Command(wxCommandEvent& event) override;

    virtual bool HasTransparentBackground() override { return true; }

    virtual WXDWORD MSWGetStyle(long style, WXDWORD *exstyle) const override;

protected:
    virtual wxBorder GetDefaultBorder() const override { return wxBORDER_NONE; }
    virtual wxSize DoGetBestSize() const override;

    virtual bool MSWGetDarkModeSupport(MSWDarkModeSupport& support) const override;

    // Implement wxMSWOwnerDrawnButtonBase methods.
    virtual int MSWGetButtonStyle() const override;
    virtual void MSWOnButtonResetOwnerDrawn() override;
    virtual int MSWGetButtonCheckedFlag() const override;
    virtual void
        MSWDrawButtonBitmap(wxDC& dc, const wxRect& rect, int flags) override;


private:
    // common part of all ctors
    void Init();


    // we need to store the state internally as the result of GetValue()
    // sometimes gets out of sync in WM_COMMAND handler
    bool m_isChecked;

    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxRadioButton);
};

#endif // _WX_RADIOBUT_H_
