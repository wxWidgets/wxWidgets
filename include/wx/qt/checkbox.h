/////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/checkbox.h
// Author:      Peter Most, Sean D'Epagnier, Mariano Reingart
// Copyright:   (c) 2010 wxWidgets dev team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_QT_CHECKBOX_H_
#define _WX_QT_CHECKBOX_H_

class QCheckBox;

class WXDLLIMPEXP_CORE wxCheckBox : public wxCheckBoxBase
{
public:
    wxCheckBox() = default;

    wxCheckBox( wxWindow *parent, wxWindowID id, const wxString& label,
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize, long style = 0,
            const wxValidator& validator = wxDefaultValidator,
            const wxString& name = wxASCII_STR(wxCheckBoxNameStr));

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxString& label,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxASCII_STR(wxCheckBoxNameStr) );

    virtual void SetValue(bool value) override;
    virtual bool GetValue() const override;

    virtual void SetLabel(const wxString& label) override;
    virtual wxString GetLabel() const override;

    QCheckBox* GetQCheckBox() const;

protected:
    virtual void DoSet3StateValue(wxCheckBoxState state) override;
    virtual wxCheckBoxState DoGet3StateValue() const override;

    wxDECLARE_DYNAMIC_CLASS(wxCheckBox);
};

#endif // _WX_QT_CHECKBOX_H_
