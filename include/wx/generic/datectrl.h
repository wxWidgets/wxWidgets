/////////////////////////////////////////////////////////////////////////////
// Name:        generic/datectrl.h
// Purpose:     generic wxDatePickerCtrl implementation
// Author:      Andreas Pflug
// Modified by:
// Created:     2005-01-19
// RCS-ID:      $Id$
// Copyright:   (c) 2005 Andreas Pflug <pgadmin@pse-consulting.de>
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GENERIC_DATECTRL_H_
#define _WX_GENERIC_DATECTRL_H_

class WXDLLIMPEXP_ADV wxCalendarDateAttr;
class WXDLLIMPEXP_ADV wxCalendarCtrl;
class WXDLLIMPEXP_ADV wxCalendarEvent;

class WXDLLIMPEXP_ADV wxDatePickerCtrl : public wxDatePickerCtrlBase
{
public:
    wxDatePickerCtrl() { Init(); }
    wxDatePickerCtrl(wxWindow *parent,
                   wxWindowID id,
                   const wxDateTime& date = wxDefaultDateTime,
                   const wxPoint& pos = wxDefaultPosition,
                   const wxSize& size = wxDefaultSize,
                   long style = wxCAL_SHOW_HOLIDAYS | wxWANTS_CHARS, const wxString& name=wxDatePickerCtrlNameStr);

    bool Create(wxWindow *parent,
                            wxWindowID id,
                            const wxDateTime& date,
                            const wxPoint& pos,
                            const wxSize& size,
                            long style,
                            const wxString& name=wxDatePickerCtrlNameStr);

    void SetValue(const wxDateTime& date);
    wxDateTime GetValue() const;
    bool GetRange(wxDateTime *dt1, wxDateTime *dt2) const;
    void SetRange(const wxDateTime &dt1, const wxDateTime &dt2);

    bool Destroy();

    bool SetLowerDateLimit(const wxDateTime& date = wxDefaultDateTime) { return m_cal->SetLowerDateLimit(date); }
    const wxDateTime& GetLowerDateLimit() const { return m_cal->GetLowerDateLimit(); }
    bool SetUpperDateLimit(const wxDateTime& date = wxDefaultDateTime) { return m_cal->SetUpperDateLimit(date); }
    const wxDateTime& GetUpperDateLimit() const { return m_cal->GetUpperDateLimit(); }

    bool SetDateRange(const wxDateTime& lowerdate = wxDefaultDateTime, const wxDateTime& upperdate = wxDefaultDateTime)
    { return m_cal->SetDateRange(lowerdate, upperdate); }

    wxCalendarDateAttr *GetAttr(size_t day) const { return m_cal->GetAttr(day); }
    void SetAttr(size_t day, wxCalendarDateAttr *attr) { m_cal->SetAttr(day, attr); }
    void SetHoliday(size_t day) { m_cal->SetHoliday(day); }
    void ResetAttr(size_t day) { m_cal->ResetAttr(day); }
    bool SetFormat(const wxChar *fmt);

    virtual bool Enable(bool enable = TRUE);
    virtual bool Show(bool show = TRUE);

    virtual wxSize DoGetBestSize() const;
    virtual void DoMoveWindow(int x, int y, int width, int height);

private:
    wxDialog *m_dlg;
    wxTextCtrl *m_txt;
    wxCalendarCtrl *m_cal;
    wxButton *m_btn;
    wxString m_format;

    bool m_dropped, m_ignoreDrop;

    void Init();
    void DropDown(bool down=true);

    void OnText(wxCommandEvent &ev);
    void OnEditKey(wxKeyEvent & event);
    void OnCalKey(wxKeyEvent & event);
    void OnClick(wxCommandEvent &ev);
    void OnSelChange(wxCalendarEvent &ev);
    void OnSetFocus(wxFocusEvent &ev);
    void OnKillFocus(wxFocusEvent &ev);
    void OnChildSetFocus(wxChildFocusEvent &ev);

    DECLARE_DYNAMIC_CLASS(wxDatePickerCtrl)
    DECLARE_EVENT_TABLE()
    DECLARE_NO_COPY_CLASS(wxDatePickerCtrl)
};

#endif // _WX_GENERIC_DATECTRL_H_

