///////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/datectrl.h
// Purpose:     wxDatePickerCtrl for Windows
// Author:      Vadim Zeitlin
// Modified by:
// Created:     2005-01-09
// RCS-ID:      $Id$
// Copyright:   (c) 2005 Vadim Zeitlin <vadim@wxwindows.org>
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
    wxDatePickerCtrl() { }

    wxDatePickerCtrl(wxWindow *parent,
                     wxWindowID id,
                     const wxDateTime& dt = wxDefaultDateTime,
                     const wxPoint& pos = wxDefaultPosition,
                     const wxSize& size = wxDefaultSize,
                     long style = 0,
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
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxDatePickerCtrlNameStr);

    // set/get the date
    virtual void SetValue(const wxDateTime& dt);
    virtual wxDateTime GetValue() const;

    // set/get the allowed valid range for the dates, if either/both of them
    // are invalid, there is no corresponding limit and if neither is set
    // GetRange() returns false
    virtual void SetRange(const wxDateTime& dt1, const wxDateTime& dt2);
    virtual bool GetRange(wxDateTime *dt1, wxDateTime *dt2) const;

protected:
    virtual WXDWORD MSWGetStyle(long style, WXDWORD *exstyle) const;

    virtual bool MSWOnNotify(int idCtrl, WXLPARAM lParam, WXLPARAM *result);

    virtual wxSize DoGetBestSize() const;


    DECLARE_NO_COPY_CLASS(wxDatePickerCtrl)
};

#endif // _WX_MSW_DATECTRL_H_
