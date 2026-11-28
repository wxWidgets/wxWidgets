/////////////////////////////////////////////////////////////////////////////
// Private, non-installed access to WinUI peer test operations.
// Available only in an explicitly test-enabled WinUI library build.
/////////////////////////////////////////////////////////////////////////////

#ifndef WX_WINUI_DATE_TIME_TEST_ACCESS_H
#define WX_WINUI_DATE_TIME_TEST_ACCESS_H

#ifndef WXWINUI_TEST_SUPPORT
    #error Link the private wx_winui_test_support target to use this header
#endif

#include "wx/datectrl.h"
#include "wx/timectrl.h"

#if wxUSE_DATEPICKCTRL
class WXDLLIMPEXP_CORE wxWinUIDatePickerTestAccess final
{
public:
    static bool SetPeerDate(
        wxDatePickerCtrl& control,
        const wxDateTime& dt);
    static bool ClearPeerDate(wxDatePickerCtrl& control);
    static bool GetPeerState(
        const wxDatePickerCtrl& control,
        wxDateTime *value,
        wxDateTime *minimum,
        wxDateTime *maximum);
    static bool GetDefaultPeerRange(
        const wxDatePickerCtrl& control,
        wxDateTime *minimum,
        wxDateTime *maximum);
    static wxString GetPeerDateFormat(const wxDatePickerCtrl& control);
    static bool UsesDropdown(const wxDatePickerCtrl& control);
    // Without wxDP_DROPDOWN: the DatePicker of WinUI is what the control
    // shows, the year, the month and the day it shows in its calendar, and
    // the format of its year.
    static bool UsesNativePicker(const wxDatePickerCtrl& control);
    static bool GetSpinFields(
        const wxDatePickerCtrl& control,
        int *year,
        int *month,
        int *day);
    static wxString GetPeerYearFormat(const wxDatePickerCtrl& control);
    static wxString GetLocaleDatePattern(const wxDatePickerCtrl& control);
    static wxString GetPeerNullText(const wxDatePickerCtrl& control);
    static wxString SetLanguage(const wxString& language);
};
#endif

#if wxUSE_TIMEPICKCTRL
class WXDLLIMPEXP_CORE wxWinUITimePickerTestAccess final
{
public:
    // Changes the time as the user does, in the TimePicker and in the
    // field of the seconds.
    static bool SetPeerTime(
        wxTimePickerCtrl& control,
        const wxDateTime& dt);
    static bool GetPeerTime(
        const wxTimePickerCtrl& control,
        wxDateTime *value);
    // The TimePicker of WinUI is what the control shows.
    static bool UsesNativePicker(const wxTimePickerCtrl& control);
    static wxString SetLanguage(const wxString& language);
};
#endif

#endif
