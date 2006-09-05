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

class WXDLLIMPEXP_ADV wxButton;
class WXDLLIMPEXP_ADV wxCalendarDateAttr;
class WXDLLIMPEXP_ADV wxCalendarCtrl;
class WXDLLIMPEXP_ADV wxCalendarEvent;
class WXDLLIMPEXP_ADV wxDatePopup;
class WXDLLIMPEXP_ADV wxTextCtrl;

class WXDLLIMPEXP_ADV wxDatePopupInternal;

class WXDLLIMPEXP_ADV wxDatePickerCtrlGeneric : public wxDatePickerCtrlBase
{
public:
    // creating the control
    wxDatePickerCtrlGeneric() { Init(); }
    virtual ~wxDatePickerCtrlGeneric() ;
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
    void SetValue(const wxDateTime& date);
    wxDateTime GetValue() const;

    bool GetRange(wxDateTime *dt1, wxDateTime *dt2) const;
    void SetRange(const wxDateTime &dt1, const wxDateTime &dt2);

    bool SetDateRange(const wxDateTime& lowerdate = wxDefaultDateTime,
                      const wxDateTime& upperdate = wxDefaultDateTime);

    // extra methods available only in this (generic) implementation
    bool SetFormat(const wxChar *fmt);
    wxCalendarCtrl *GetCalendar() const { return m_cal; }


    // implementation only from now on
    // -------------------------------

    // overridden base class methods
    virtual bool Destroy();

    virtual bool Enable(bool enable = true);
    virtual bool Show(bool show = true);

protected:
    virtual wxSize DoGetBestSize() const;
    virtual void DoMoveWindow(int x, int y, int width, int height);

private:
    void Init();
    void DropDown(bool down = true);

    void OnText(wxCommandEvent &event);
    void OnEditKey(wxKeyEvent & event);
    void OnCalKey(wxKeyEvent & event);
    void OnClick(wxCommandEvent &event);
    void OnSelChange(wxCalendarEvent &event);
    void OnSetFocus(wxFocusEvent &event);
    void OnKillFocus(wxFocusEvent &event);
    void OnChildSetFocus(wxChildFocusEvent &event);
    void OnSize(wxSizeEvent& event);


    wxDatePopupInternal *m_popup;
    wxTextCtrl *m_txt;
    wxCalendarCtrl *m_cal;
    wxButton *m_btn;
    wxString m_format;
    wxDateTime m_currentDate;

    bool m_dropped,
         m_ignoreDrop;


    DECLARE_EVENT_TABLE()
    DECLARE_NO_COPY_CLASS(wxDatePickerCtrlGeneric)
};

#endif // _WX_GENERIC_DATECTRL_H_

