/////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/checkbox.h
// Author:      Peter Most
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_QT_CHECKBOX_H_
#define _WX_QT_CHECKBOX_H_

#include <QtWidgets/QCheckBox>

class WXDLLIMPEXP_CORE wxCheckBox : public wxCheckBoxBase
{
public:
    wxCheckBox();
    wxCheckBox( wxWindow *parent, wxWindowID id, const wxString& label,
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize, long style = 0,
            const wxValidator& validator = wxDefaultValidator,
            const wxString& name = wxCheckBoxNameStr);

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxString& label,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxCheckBoxNameStr );

    virtual void SetValue(bool value);
    virtual bool GetValue() const;

    virtual QCheckBox *GetHandle() const;

protected:
    virtual void DoSet3StateValue(wxCheckBoxState state);
    virtual wxCheckBoxState DoGet3StateValue() const;

private:
    QCheckBox *m_qtCheckBox;

    wxDECLARE_DYNAMIC_CLASS(wxCheckBox);
};

#endif // _WX_QT_CHECKBOX_H_
