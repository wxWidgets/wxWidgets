///////////////////////////////////////////////////////////////////////////////
// Name:        wx/univ/radiobut.h
// Purpose:     wxRadioButton declaration
// Author:      Vadim Zeitlin
// Modified by:
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
    virtual void SetValue(bool value) wxOVERRIDE { return wxCheckBox::SetValue(value); }
    virtual bool GetValue() const wxOVERRIDE { return wxCheckBox::GetValue(); }

    // override some base class methods
    virtual void ChangeValue(bool value) wxOVERRIDE;

protected:
    virtual wxBorder GetDefaultBorder() const wxOVERRIDE { return wxBORDER_NONE; }

    // implement our own drawing
    virtual void DoDraw(wxControlRenderer *renderer) wxOVERRIDE;

    // we use the radio button bitmaps for size calculation
    virtual wxSize GetBitmapSize() const wxOVERRIDE;

    // the radio button can only be cleared using this method, not
    // ChangeValue() above - and it is protected as it can only be called by
    // another radiobutton
    void ClearValue();

    // called when the radio button becomes checked: we clear all the buttons
    // in the same group with us here
    virtual void OnCheck() wxOVERRIDE;

    // send event about radio button selection
    virtual void SendEvent() wxOVERRIDE;

private:
    wxDECLARE_DYNAMIC_CLASS(wxRadioButton);
};

#endif // _WX_UNIV_RADIOBUT_H_
