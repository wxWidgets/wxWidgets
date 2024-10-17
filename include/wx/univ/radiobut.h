///////////////////////////////////////////////////////////////////////////////
// Name:        wx/univ/radiobut.h
// Purpose:     wxRadioButton declaration
// Author:      Vadim Zeitlin
// Created:     10.09.00
// Copyright:   (c) 2000 SciTech Software, Inc. (www.scitechsoft.com)
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_UNIV_RADIOBUT_H_
#define _WX_UNIV_RADIOBUT_H_

// ----------------------------------------------------------------------------
// wxRadioButton
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxRadioButton : public wxRadioButtonBase
{
public:
    // constructors
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

    // (re)implement pure virtuals from wxRadioButtonBase
    virtual void SetValue(bool value) override { return wxCheckBox::SetValue(value); }
    virtual bool GetValue() const override { return wxCheckBox::GetValue(); }

    // override some base class methods
    virtual void ChangeValue(bool value) override;

protected:
    virtual wxBorder GetDefaultBorder() const override { return wxBORDER_NONE; }

    // implement our own drawing
    virtual void DoDraw(wxControlRenderer *renderer) override;

    // we use the radio button bitmaps for size calculation
    virtual wxSize GetBitmapSize() const override;

    // the radio button can only be cleared using this method, not
    // ChangeValue() above - and it is protected as it can only be called by
    // another radiobutton
    void ClearValue();

    // called when the radio button becomes checked: we clear all the buttons
    // in the same group with us here
    virtual void OnCheck() override;

    // send event about radio button selection
    virtual void SendEvent() override;

private:
    wxDECLARE_DYNAMIC_CLASS(wxRadioButton);
};

#endif // _WX_UNIV_RADIOBUT_H_
