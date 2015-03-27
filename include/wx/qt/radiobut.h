/////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/radiobut.h
// Author:      Peter Most
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef WX_QT_RADIOBUT_H_
#define WX_QT_RADIOBUT_H_

#include <QtWidgets/QRadioButton>

class WXDLLIMPEXP_CORE wxRadioButton : public wxControl
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
                   const wxString& name = wxRadioButtonNameStr );

    bool Create( wxWindow *parent,
                 wxWindowID id,
                 const wxString& label,
                 const wxPoint& pos = wxDefaultPosition,
                 const wxSize& size = wxDefaultSize,
                 long style = 0,
                 const wxValidator& validator = wxDefaultValidator,
                 const wxString& name = wxRadioButtonNameStr );

    virtual void SetValue(bool value);
    virtual bool GetValue() const;

    virtual QRadioButton *GetHandle() const;

protected:

private:
    QRadioButton *m_qtRadioButton;

    wxDECLARE_DYNAMIC_CLASS( wxRadioButton );
};

#endif // _WX_QT_RADIOBUT_H_
