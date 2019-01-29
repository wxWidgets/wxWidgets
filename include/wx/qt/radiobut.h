/////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/radiobut.h
// Author:      Peter Most
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_QT_RADIOBUT_H_
#define _WX_QT_RADIOBUT_H_

class QRadioButton;
class QButtonGroup;

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

    virtual ~wxRadioButton();

    virtual void SetValue(bool value);
    virtual bool GetValue() const;

    virtual QWidget *GetHandle() const;

private:
    void createAndJoinNewGroup( wxWindow *parent );
    void searchForPreviousGroupToJoin( wxWindow *parent );

    QRadioButton *m_qtRadioButton;
    QButtonGroup *m_qtButtonGroup;

    wxDECLARE_DYNAMIC_CLASS( wxRadioButton );
};

#endif // _WX_QT_RADIOBUT_H_
