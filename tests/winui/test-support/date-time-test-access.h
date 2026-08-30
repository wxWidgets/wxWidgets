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
    static bool GetSpinFields(
        const wxDatePickerCtrl& control,
        int *year,
        int *month,
        int *day);
    static bool SetSpinPart(
        wxDatePickerCtrl& control,
        unsigned part, int value);
    static bool GetSpinPart(
        const wxDatePickerCtrl& control,
        unsigned part,
        int *value,
        bool *blank);
    static wxString GetSpinYearText(const wxDatePickerCtrl& control);
    static wxString GetLocaleDatePattern(const wxDatePickerCtrl& control);
    static wxString GetPeerNullText(const wxDatePickerCtrl& control);
    static wxString SetLanguage(const wxString& language);
};
#endif

#if wxUSE_TIMEPICKCTRL
class WXDLLIMPEXP_CORE wxWinUITimePickerTestAccess final
{
public:
    using HourLoadedHook =
        void (*)(wxTimePickerCtrl *, void *);

    static bool SetPeerTime(
        wxTimePickerCtrl& control,
        const wxDateTime& dt);
    static bool GetPeerTime(
        const wxTimePickerCtrl& control,
        wxDateTime *value);
    static wxString GetLocaleTimePattern(const wxTimePickerCtrl& control);
    static bool GetTimeFieldOrder(
        const wxTimePickerCtrl& control,
        int *hour,
        int *minute,
        int *second,
        int *period);
    static bool GetHourSpinBinding(
        const wxTimePickerCtrl& control,
        std::uintptr_t *incrementIdentity,
        std::uintptr_t *decrementIdentity,
        std::uint64_t *generation = nullptr);
    static void SetHourLoadedHook(HourLoadedHook hook,
        void *data = nullptr);
    static wxString SetLanguage(const wxString& language);
};
#endif

#endif
