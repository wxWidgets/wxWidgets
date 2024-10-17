///////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/timectrl.h
// Purpose:     wxTimePickerCtrl for Qt.
// Author:      Ali Kettab
// Created:     2023-10-13
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_QT_TIMECTRL_H_
#define _WX_QT_TIMECTRL_H_

class QTimeEdit;

// ----------------------------------------------------------------------------
// wxTimePickerCtrl
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_ADV wxTimePickerCtrl : public wxTimePickerCtrlBase
{
public:
    // ctors
    wxTimePickerCtrl() = default;

    wxTimePickerCtrl(wxWindow *parent,
                     wxWindowID id,
                     const wxDateTime& dt = wxDefaultDateTime,
                     const wxPoint& pos = wxDefaultPosition,
                     const wxSize& size = wxDefaultSize,
                     long style = wxTP_DEFAULT,
                     const wxValidator& validator = wxDefaultValidator,
                     const wxString& name = wxTimePickerCtrlNameStr)
    {
        Create(parent, id, dt, pos, size, style, validator, name);
    }

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxDateTime& dt = wxDefaultDateTime,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxTP_DEFAULT,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxTimePickerCtrlNameStr);

    // Override this one to add time-specific (and date-ignoring) checks.
    virtual void SetValue(const wxDateTime& dt) override;
    virtual wxDateTime GetValue() const override;

    QTimeEdit* GetQTimeEdit() const;

    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxTimePickerCtrl);
};

#endif // _WX_QT_TIMECTRL_H_
