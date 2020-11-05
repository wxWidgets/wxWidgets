///////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/datetimectrl.h
// Purpose:     wxDateTimePickerCtrl for Windows.
// Author:      Vadim Zeitlin
// Created:     2011-09-22 (extracted from wx/msw/datectrl.h).
// Copyright:   (c) 2005-2011 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MSW_DATETIMECTRL_H_
#define _WX_MSW_DATETIMECTRL_H_

#include "wx/intl.h"

// Forward declare a struct from Platform SDK.
struct tagNMDATETIMECHANGE;

// ----------------------------------------------------------------------------
// wxDateTimePickerCtrl
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_ADV wxDateTimePickerCtrl : public wxDateTimePickerCtrlBase
{
public:
    // set/get the date
    virtual void SetValue(const wxDateTime& dt) wxOVERRIDE;
    virtual wxDateTime GetValue() const wxOVERRIDE;

    virtual void SetNullText(const wxString& text) wxOVERRIDE;

    // returns true if the platform should explicitly apply a theme border
    virtual bool CanApplyThemeBorder() const wxOVERRIDE { return false; }

    virtual bool MSWOnNotify(int idCtrl, WXLPARAM lParam, WXLPARAM *result) wxOVERRIDE;

protected:
    virtual wxBorder GetDefaultBorder() const wxOVERRIDE { return wxBORDER_NONE; }
    virtual wxSize DoGetBestSize() const wxOVERRIDE;

    // Helper for the derived classes Create(): creates a native control with
    // the specified attributes.
    bool MSWCreateDateTimePicker(wxWindow *parent,
                                 wxWindowID id,
                                 const wxDateTime& dt,
                                 const wxPoint& pos,
                                 const wxSize& size,
                                 long style,
                                 const wxValidator& validator,
                                 const wxString& name);

#if wxUSE_INTL
    // Override to return the date/time format used by this control.
    virtual wxLocaleInfo MSWGetFormat() const = 0;
#endif // wxUSE_INTL

    // Override to indicate whether we can have no date at all.
    virtual bool MSWAllowsNone() const = 0;

    // Override to update m_date and send the event when the control contents
    // changes, return true if the event was handled.
    virtual bool MSWOnDateTimeChange(const tagNMDATETIMECHANGE& dtch) = 0;


    // the date currently shown by the control, may be invalid
    wxDateTime m_date;

private:
    // Helper setting the appropriate format depending on the passed in state.
    void MSWUpdateFormat(bool valid);

    // Same thing, but only doing if the validity differs from the date
    // validity, i.e. avoiding useless work if nothing needs to be done.
    void MSWUpdateFormatIfNeeded(bool valid);


    // shown when there is no valid value (so only used with wxDP_ALLOWNONE),
    // always non-empty if SetNullText() was called, see the comments there
    wxString m_nullText;
};

#endif // _WX_MSW_DATETIMECTRL_H_
