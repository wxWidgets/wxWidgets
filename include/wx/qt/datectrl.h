///////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/datectrl.h
// Purpose:     wxDatePickerCtrl for Qt
// Author:      Ali Kettab
// Created:     2023-10-12
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_QT_DATECTRL_H_
#define _WX_QT_DATECTRL_H_

class QDateEdit;

// ----------------------------------------------------------------------------
// wxDatePickerCtrl
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_ADV wxDatePickerCtrl : public wxDatePickerCtrlBase
{
public:
    // ctors
    wxDatePickerCtrl() = default;

    wxDatePickerCtrl(wxWindow *parent,
                     wxWindowID id,
                     const wxDateTime& dt = wxDefaultDateTime,
                     const wxPoint& pos = wxDefaultPosition,
                     const wxSize& size = wxDefaultSize,
                     long style = wxDP_DEFAULT | wxDP_SHOWCENTURY,
                     const wxValidator& validator = wxDefaultValidator,
                     const wxString& name = wxDatePickerCtrlNameStr)
    {
        Create(parent, id, dt, pos, size, style, validator, name);
    }

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxDateTime& dt = wxDefaultDateTime,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxDP_DEFAULT | wxDP_SHOWCENTURY,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxDatePickerCtrlNameStr);

    // Override this one to add date-specific (and time-ignoring) checks.
    virtual void SetValue(const wxDateTime& dt) override;
    virtual wxDateTime GetValue() const override;

    // Implement the base class pure virtuals.
    virtual void SetRange(const wxDateTime& dt1, const wxDateTime& dt2) override;
    virtual bool GetRange(wxDateTime *dt1, wxDateTime *dt2) const override;

    QDateEdit* GetQDateEdit() const;

    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxDatePickerCtrl);
};

#endif // _WX_QT_DATECTRL_H_
