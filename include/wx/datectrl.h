///////////////////////////////////////////////////////////////////////////////
// Name:        wx/datectrl.h
// Purpose:     implements wxDatePickerCtrl
// Author:      Vadim Zeitlin
// Modified by:
// Created:     2005-01-09
// RCS-ID:      $Id$
// Copyright:   (c) 2005 Vadim Zeitlin <vadim@wxwindows.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_DATECTRL_H_
#define _WX_DATECTRL_H_

#include "wx/control.h"         // the base class
#include "wx/datetime.h"

#define wxDatePickerCtrlNameStr _T("datectrl")

// ----------------------------------------------------------------------------
// wxDatePickerCtrl: allow the user to enter the date
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_ADV wxDatePickerCtrlBase : public wxControl
{
public:
    /*
       The derived classes should implement ctor and Create() method with the
       following signature:

        bool Create(wxWindow *parent,
                    wxWindowID id,
                    const wxDateTime& dt = wxDefaultDateTime,
                    const wxPoint& pos = wxDefaultPosition,
                    const wxSize& size = wxDefaultSize,
                    long style = 0,
                    const wxValidator& validator = wxDefaultValidator,
                    const wxString& name = wxDatePickerCtrlNameStr);
     */

    // set/get the date
    virtual void SetValue(const wxDateTime& dt) = 0;
    virtual wxDateTime GetValue() const = 0;

    // set/get the allowed valid range for the dates, if either/both of them
    // are invalid, there is no corresponding limit and if neither is set
    // GetRange() returns false
    virtual void SetRange(const wxDateTime& dt1, const wxDateTime& dt2) = 0;
    virtual bool GetRange(wxDateTime *dt1, wxDateTime *dt2) const = 0;
};

#if defined(__WXMSW__) && !defined(__WXUNIVERSAL__)
    #include "wx/msw/datectrl.h"
#else
    #include "wx/generic/datectrl.h"
#endif

#endif // _WX_DATECTRL_H_

