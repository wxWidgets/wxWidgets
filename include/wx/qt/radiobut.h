/////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/radiobut.h
// Author:      Peter Most
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_QT_RADIOBUT_H_
#define _WX_QT_RADIOBUT_H_

class QRadioButton;

class WXDLLIMPEXP_CORE wxRadioButton : public wxRadioButtonBase
{
public:
    wxRadioButton();
    wxRadioButton( wxWindow *parent,
                   wxWindowID id,
                   const wxString& label,
                   const wxPoint& pos = wxDefaultPosition,
                   const wxSize& size = wxDefaultSize,
                   long style = 0,
                   const wxValidator& validator = wxDefaultValidator,
                   const wxString& name = wxASCII_STR(wxRadioButtonNameStr) );

    bool Create( wxWindow *parent,
                 wxWindowID id,
                 const wxString& label,
                 const wxPoint& pos = wxDefaultPosition,
                 const wxSize& size = wxDefaultSize,
                 long style = 0,
                 const wxValidator& validator = wxDefaultValidator,
                 const wxString& name = wxASCII_STR(wxRadioButtonNameStr) );

    virtual void SetValue(bool value) override;
    virtual bool GetValue() const override;

    virtual void SetLabel(const wxString &label) override;
    virtual wxString GetLabel() const override;

    virtual QWidget *GetHandle() const override;

private:
    QRadioButton *m_qtRadioButton;

    wxDECLARE_DYNAMIC_CLASS( wxRadioButton );
};

#endif // _WX_QT_RADIOBUT_H_
