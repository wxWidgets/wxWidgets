///////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/datectrl.h
// Purpose:     wxDatePickerCtrl for Windows
// Author:      Vadim Zeitlin
// Created:     2005-01-09
// Copyright:   (c) 2005 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MSW_DATECTRL_H_
#define _WX_MSW_DATECTRL_H_

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

    // Override MSW-specific functions used during control creation.
    virtual WXDWORD MSWGetStyle(long style, WXDWORD *exstyle) const override;

protected:
#if wxUSE_INTL
    virtual wxLocaleInfo MSWGetFormat() const override;
#endif // wxUSE_INTL
    virtual bool MSWAllowsNone() const override { return HasFlag(wxDP_ALLOWNONE); }
    virtual bool MSWOnDateTimeChange(const tagNMDATETIMECHANGE& dtch) override;

private:
    wxDateTime MSWGetControlValue() const;

    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxDatePickerCtrl);
};

#endif // _WX_MSW_DATECTRL_H_
