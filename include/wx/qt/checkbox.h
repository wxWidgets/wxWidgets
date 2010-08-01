/////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/checkbox.h
// Author:      Peter Most
// Id:          $Id$
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_QT_CHECKBOX_H_
#define _WX_QT_CHECKBOX_H_

#include <QtCore/QPointer>
#include <QtGui/QCheckBox>

class WXDLLIMPEXP_CORE wxCheckBox : public wxCheckBoxBase
{
public:
    wxCheckBox();
    wxCheckBox( wxWindow *parent, wxWindowID id, const wxString& label,
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize, long style = 0,
            const wxValidator& validator = wxDefaultValidator,
            const wxString& name = wxCheckBoxNameStr);

    ~wxCheckBox();

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

private:
    QPointer< QCheckBox > m_qtCheckBox;
};

#endif // _WX_QT_CHECKBOX_H_
