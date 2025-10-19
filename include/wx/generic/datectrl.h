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

        virtual bool Validate(wxWindow* parent) override
        {
            // We can only be used with wxComboCtrl, so just a static_cast<> would
            // be safe, but use checked cast to notice any problems in debug build.
            const wxString
                s = wxStaticCast(m_validatorWindow, wxComboCtrl)->GetValue();
            if (s.empty())
            {
                // There is no need to tell the user that an empty string is
                // invalid, this shouldn't be a surprise for them.
                return true;
            }

            wxDateTime dt;
            if (!dt.ParseFormat(s, m_format))
            {
                wxMessageBox
                (
                    wxString::Format
                    (
                        _("\"%s\" is not in the expected date format, "
                            "please enter it as e.g. \"%s\"."),
                        s, wxDateTime::Today().Format(m_format)
                    ),
                    _("Invalid date"),
                    wxOK | wxICON_WARNING,
                    parent
                );

                return false;
            }

            return true;
        }

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
    virtual bool Create(wxWindow* parent) override
    {
        if (!wxCalendarCtrl::Create(parent, wxID_ANY, wxDefaultDateTime,
            wxPoint(0, 0), wxDefaultSize,
            wxCAL_SEQUENTIAL_MONTH_SELECTION
            | wxCAL_SHOW_HOLIDAYS | wxBORDER_SUNKEN))
            return false;

        SetFormat(GetLocaleDateFormat());

        m_useSize = wxCalendarCtrl::GetBestSize();

        wxWindow* tx = m_combo->GetTextCtrl();
        if (!tx)
            tx = m_combo;

        tx->Bind(wxEVT_KILL_FOCUS, &wxCalendarComboPopup::OnKillTextFocus, this);

        return true;
    }

    virtual wxSize GetAdjustedSize(int WXUNUSED(minWidth),
        int WXUNUSED(prefHeight),
        int WXUNUSED(maxHeight)) override
    {
        return m_useSize;
    }

    virtual wxWindow* GetControl() override { return this; }

    void SetDateValue(const wxDateTime& date)
    {
        if (date.IsValid())
        {
            m_combo->SetText(date.Format(m_format));
            SetDate(date);
        }
        else // invalid date
        {
            wxASSERT_MSG(HasDPFlag(wxDP_ALLOWNONE),
                wxT("this control must have a valid date"));

            m_combo->SetText(wxEmptyString);
        }
    }

    bool IsTextEmpty() const
    {
        return m_combo->GetTextCtrl()->IsEmpty();
    }

    // This is public because it is used by wxDatePickerCtrlGeneric itself to
    // change the date when the text control field changes. The reason it's
    // done there and not in this class itself is mostly historic.
    void ChangeDateAndNotifyIfValid()
    {
        wxDateTime dt;
        if (!ParseDateTime(m_combo->GetValue(), &dt))
        {
            // The user must be in the process of updating the date, don't do
            // anything -- we'll take care of ensuring it's valid on focus loss
            // later.
            return;
        }

        if (dt == GetDate())
        {
            // No need to send event if the date hasn't changed.
            return;
        }

        // We change the date immediately, as it's more consistent with the
        // native MSW version and avoids another event on focus loss.
        SetDate(dt);

        SendDateEvent(dt);
    }

    bool SetFormat(const wxString& fmt)
    {
        m_format = fmt;

        if (m_combo)
        {
#if wxUSE_VALIDATORS
            m_combo->SetValidator(DateValidator(m_format));
#endif // wxUSE_VALIDATORS

            if (GetDate().IsValid())
                m_combo->SetText(GetDate().Format(m_format));
        }

        return true;
    }


private:
    bool ParseDateTime(const wxString& s, wxDateTime* pDt) const
    {
        pDt->ParseFormat(s, m_format);
        if (!pDt->IsValid())
            return false;

        return true;
    }

    void SendDateEvent(const wxDateTime& dt)
    {
        // Sends both wxCalendarEvent and wxDateEvent
        wxWindow* datePicker = m_combo->GetParent();

        wxCalendarEvent cev(datePicker, dt, wxEVT_CALENDAR_SEL_CHANGED);
        datePicker->GetEventHandler()->ProcessEvent(cev);

        wxDateEvent event(datePicker, dt, wxEVT_DATE_CHANGED);
        datePicker->GetEventHandler()->ProcessEvent(event);
    }

    void OnCalKey(wxKeyEvent& ev)
    {
        if (ev.GetKeyCode() == WXK_ESCAPE && !ev.HasModifiers())
            Dismiss();
        else
            ev.Skip();
    }

    void OnSelChange(wxCalendarEvent& ev)
    {
        m_combo->SetText(GetDate().Format(m_format));

        if (ev.GetEventType() == wxEVT_CALENDAR_DOUBLECLICKED)
        {
            Dismiss();
        }

        SendDateEvent(GetDate());
    }

    void OnKillTextFocus(wxFocusEvent& ev)
    {
        ev.Skip();

        const wxDateTime& dtOld = GetDate();

        wxDateTime dt;
        wxString value = m_combo->GetValue();
        if (!ParseDateTime(value, &dt))
        {
            if (!HasDPFlag(wxDP_ALLOWNONE))
                dt = dtOld;
        }

        if (dt.IsValid())
        {
            // Set it at wxCalendarCtrl level.
            SetDate(dt);

            // And show it in the text field.
            m_combo->SetText(GetStringValue());

            // And if the date has really changed, send an event about it.
            if (dt != dtOld)
                SendDateEvent(dt);
        }
        else // Invalid date currently entered.
        {
            if (HasDPFlag(wxDP_ALLOWNONE))
            {
                // Clear the text part to indicate that the date is invalid.
                // Would it be better to indicate this in some more clear way,
                // e.g. maybe by using "[none]" or something like this?
                m_combo->SetText(wxString());
            }
            else
            {
                // Restore the original value, as we can't have invalid value
                // in this control.
                m_combo->SetText(GetStringValue());
            }
        }
    }

    bool HasDPFlag(int flag) const
    {
        return m_combo->GetParent()->HasFlag(flag);
    }

    // Return the format to be used for the dates shown by the control. This
    // functions honours wxDP_SHOWCENTURY flag.
    wxString GetLocaleDateFormat() const
    {
#if wxUSE_INTL
        wxString fmt = wxUILocale::GetCurrent().GetInfo(wxLOCALE_SHORT_DATE_FMT);
        if (HasDPFlag(wxDP_SHOWCENTURY))
            fmt.Replace("%y", "%Y");
#else // !wxUSE_INTL
        wxString fmt = wxS("%x");
#endif // wxUSE_INTL/!wxUSE_INTL

        // Also check if we can actually parse dates in this format because we
        // had several problems with unsupported format specifiers being used
        // in some locales date format strings in the past, and in this case
        // we'd just annoy the user with senseless messages about invalid dates
        // being entered when it's actually just our own bug.
        wxDateTime dt;
        if (!dt.ParseFormat(wxDateTime::Now().Format(fmt), fmt))
        {
            // If we can't parse the date in the format we're going to use, we
            // can't use it and have to fallback to something else -- this is
            // not ideal, but better than not allowing the user to enter any
            // dates at all.
            wxLogTrace("datectrl",
                "Can't parse dates in format \"%s\", "
                "using ISO 8601 as fallback",
                fmt);

            fmt = HasDPFlag(wxDP_SHOWCENTURY) ? wxS("%Y-%m-%d")
                : wxS("%y-%m-%d");
        }

        return fmt;
    }

    virtual void SetStringValue(const wxString& s) override
    {
        wxDateTime dt;
        if (ParseDateTime(s, &dt))
            SetDate(dt);
        //else: keep the old value
    }

    virtual wxString GetStringValue() const override
    {
        return GetStringValueFor(GetDate());
    }

private:
    // returns either the given date representation using the current format or
    // an empty string if it's invalid
    wxString GetStringValueFor(const wxDateTime& dt) const
    {
        wxString val;
        if (dt.IsValid())
            val = dt.Format(m_format);

        return val;
    }

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

