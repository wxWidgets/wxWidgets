/////////////////////////////////////////////////////////////////////////////
// Name:        wx/generic/datectrl.h
// Purpose:     generic wxDatePickerCtrl implementation
// Author:      Andreas Pflug
// Created:     2005-01-19
// Copyright:   (c) 2005 Andreas Pflug <pgadmin@pse-consulting.de>
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GENERIC_DATECTRL_H_
#define _WX_GENERIC_DATECTRL_H_

#include "wx/compositewin.h"
#include "wx/containr.h"
#include "wx/combo.h"
#include "wx/uilocale.h"
#include "wx/calctrl.h"
#include "wx/dateevt.h"

class WXDLLIMPEXP_FWD_CORE wxComboCtrl;

class WXDLLIMPEXP_FWD_CORE wxCalendarCtrl;

#if wxUSE_VALIDATORS

    class DateValidator : public wxValidator
    {
    public:
        explicit DateValidator(const wxString& format) : m_format(format) {}
        DateValidator(const DateValidator& val) = default;

        virtual wxObject* Clone() const override
        {
            return new DateValidator(*this);
        }

        virtual bool Validate(wxWindow* parent) override;
        
        // We don't transfer any data, this validator is used only for validation.
        virtual bool TransferFromWindow() override { return true; }
        virtual bool TransferToWindow() override { return true; }

    private:
        const wxString m_format;
    };

#endif // wxUSE_VALIDATORS

class wxCalendarComboPopup : public wxCalendarCtrl,
    public wxComboPopup
{
public:

    wxCalendarComboPopup() : wxCalendarCtrl(),
        wxComboPopup()
    {
    }

    virtual void Init() override
    {
    }

    // NB: Don't create lazily since it didn't work that way before
    //     wxComboCtrl was used, and changing behaviour would almost
    //     certainly introduce new bugs.
    virtual bool Create(wxWindow* parent) override;
    virtual wxSize GetAdjustedSize(int WXUNUSED(minWidth),
        int WXUNUSED(prefHeight),
        int WXUNUSED(maxHeight)) override
    {
        return m_useSize;
    }

    virtual wxWindow* GetControl() override { return this; }

    void SetDateValue(const wxDateTime& date);
  
    bool IsTextEmpty() const
    {
        return m_combo->GetTextCtrl()->IsEmpty();
    }

    // This is public because it is used by wxDatePickerCtrlGeneric itself to
    // change the date when the text control field changes. The reason it's
    // done there and not in this class itself is mostly historic.
    void ChangeDateAndNotifyIfValid();
   
    bool SetFormat(const wxString& fmt);
 

private:
    bool ParseDateTime(const wxString& s, wxDateTime* pDt) const;
  
    void SendDateEvent(const wxDateTime& dt);

    void OnCalKey(wxKeyEvent& ev);
    
    void OnSelChange(wxCalendarEvent& ev);
   
    void OnKillTextFocus(wxFocusEvent& ev);
   
    bool HasDPFlag(int flag) const
    {
        return m_combo->GetParent()->HasFlag(flag);
    }

    // Return the format to be used for the dates shown by the control. This
    // functions honours wxDP_SHOWCENTURY flag.
    wxString GetLocaleDateFormat() const;
 
    virtual void SetStringValue(const wxString& s) override;

    virtual wxString GetStringValue() const override;
  
private:
    // returns either the given date representation using the current format or
    // an empty string if it's invalid
    wxString GetStringValueFor(const wxDateTime& dt) const;
   
    wxSize          m_useSize;
    wxString        m_format;

    wxDECLARE_EVENT_TABLE();
};



typedef wxDatePickerCtrlCommonBase<wxDateTimePickerCtrlBase> wxDatePickerCtrlGenericBase;

class WXDLLIMPEXP_CORE wxDatePickerCtrlGeneric
    : public wxCompositeWindow< wxNavigationEnabled<wxDatePickerCtrlGenericBase> >
{
public:
    // creating the control
    wxDatePickerCtrlGeneric() { Init(); }
    virtual ~wxDatePickerCtrlGeneric();
    wxDatePickerCtrlGeneric(wxWindow *parent,
                            wxWindowID id,
                            const wxDateTime& date = wxDefaultDateTime,
                            const wxPoint& pos = wxDefaultPosition,
                            const wxSize& size = wxDefaultSize,
                            long style = wxDP_DEFAULT | wxDP_SHOWCENTURY,
                            const wxValidator& validator = wxDefaultValidator,
                            const wxString& name = wxDatePickerCtrlNameStr)
    {
        Init();

        (void)Create(parent, id, date, pos, size, style, validator, name);
    }

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxDateTime& date = wxDefaultDateTime,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxDP_DEFAULT | wxDP_SHOWCENTURY,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxDatePickerCtrlNameStr);

    // wxDatePickerCtrl methods
    void SetFormat(const wxString& format);
    void SetValue(const wxDateTime& date) override;
    wxDateTime GetValue() const override;

    bool GetRange(wxDateTime *dt1, wxDateTime *dt2) const override;
    void SetRange(const wxDateTime &dt1, const wxDateTime &dt2) override;

    bool SetDateRange(const wxDateTime& lowerdate = wxDefaultDateTime,
                      const wxDateTime& upperdate = wxDefaultDateTime);

    // extra methods available only in this (generic) implementation
    wxCalendarCtrl *GetCalendar() const;


    // implementation only from now on
    // -------------------------------

    // overridden base class methods
    virtual bool Destroy() override;

protected:
    virtual wxSize DoGetBestSize() const override;

private:
    void Init();

    // return the list of the windows composing this one
    virtual wxWindowList GetCompositeWindowParts() const override;

    void OnText(wxCommandEvent &event);
    void OnSize(wxSizeEvent& event);

    wxComboCtrl* m_combo;
    wxCalendarComboPopup* m_popup;

    wxDECLARE_EVENT_TABLE();
    wxDECLARE_NO_COPY_CLASS(wxDatePickerCtrlGeneric);
};

#endif // _WX_GENERIC_DATECTRL_H_

