/////////////////////////////////////////////////////////////////////////////
// Private, non-installed access to WinUI peer test operations.
// Available only in an explicitly test-enabled WinUI library build.
/////////////////////////////////////////////////////////////////////////////

#ifndef WX_WINUI_CALENDAR_TEST_ACCESS_H
#define WX_WINUI_CALENDAR_TEST_ACCESS_H

#ifndef WXWINUI_TEST_SUPPORT
    #error Link the private wx_winui_test_support target to use this header
#endif

#include "wx/calctrl.h"
#include <vector>

#if wxUSE_CALENDARCTRL
class WXDLLIMPEXP_CORE wxWinUICalendarTestAccess final
{
public:
    struct WeekRefreshRecovery
    {
        std::uint64_t generation = 0;
        std::uint64_t requestRevision = 0;
        std::uint64_t completedRevision = 0;
        unsigned automaticRetriesRemaining = 0;
        unsigned injectedFailuresRemaining = 0;
        unsigned injectedPendingPassesRemaining = 0;
        unsigned injectedPeerMutationPassesRemaining = 0;
        unsigned injectedDayLayoutFailuresRemaining = 0;
        unsigned pendingCause = 0;
        std::uint64_t peerMutationDeferrals = 0;
        std::uint64_t boundedPendingRetries = 0;
        std::uint64_t invalidTicketDeferrals = 0;
        std::uint64_t navigationGeneration = 0;
        std::uint64_t navigationResolveFailures = 0;
        std::uint64_t navigationRetryExhaustions = 0;
        std::uint64_t dayItemIdentityChanges = 0;
        std::uint64_t dayItemRepeatNotifications = 0;
        std::uint64_t weekLayoutEchoAbsorptions = 0;
        std::uint64_t dayLayoutFailureRecoveries = 0;
        unsigned projectedMonthDays = 0;
        unsigned projectedWeekdayColumns = 0;
        unsigned navigationSnapshotButtons = 0;
        unsigned navigationCandidates = 0;
        unsigned navigationFailure = 0;
        bool scheduled = false;
        bool degraded = false;
        bool waitingForMutationEnd = false;
        bool navigationScheduled = false;
        bool navigationBindingRequired = false;
        bool navigationBound = false;
        bool dayLayoutValidationPending = false;
        bool dayLayoutValidationScheduled = false;
    };

    enum class DayLayoutValidationFailure : unsigned
    {
        Exception = 1,
        TransientRead = 2,
        QueueRefusal = 3
    };

    struct MonthTopologyDay
    {
        int month = 0;
        int calendarDay = 0;
        int daysInMonth = 0;
        double x = 0.0;
        double y = 0.0;
        double width = 0.0;
        double height = 0.0;
        std::uintptr_t identity = 0;
        bool authoritative = false;
    };

    struct MonthTopologyResult
    {
        int month = 0;
        unsigned dayCount = 0;
        unsigned columnCount = 0;
        unsigned rowCount = 0;
        double firstDayTop = 0.0;
    };

    struct LayoutTicket
    {
        std::uint64_t layoutRevision = 0;
        std::uint64_t realizedDaysRevision = 0;
        std::uint64_t weekRefreshRevision = 0;
        std::uint64_t navigationGeneration = 0;
        std::uintptr_t decrementIdentity = 0;
        std::uintptr_t incrementIdentity = 0;
    };

    static bool SetPeerDate(
        wxCalendarCtrl& control,
        const wxDateTime& date);
    static bool GetPeerState(
        const wxCalendarCtrl& control,
        wxDateTime *date,
        wxDateTime *minimum,
        wxDateTime *maximum);
    static bool GetDefaultPeerRange(
        const wxCalendarCtrl& control,
        wxDateTime *minimum,
        wxDateTime *maximum);
    static bool DoubleTapDate(
        wxCalendarCtrl& control,
        const wxDateTime& date);
    static bool DoubleTapNonDay(wxCalendarCtrl& control);
    static bool IsMarked(
        const wxCalendarCtrl& control,
        size_t day);
    static bool IsHoliday(
        const wxCalendarCtrl& control,
        size_t day);
    static bool GetAppliedDensityColour(
        wxCalendarCtrl& control,
        size_t day,
        wxColour *colour,
        unsigned *colourCount = nullptr,
        unsigned long long *writeRevision = nullptr);
    static bool SetTodayForeground(
        wxCalendarCtrl& control,
        const wxColour& colour);
    static bool UseSystemMarkColourFallback(wxCalendarCtrl& control);
    static bool DeliverThemeChanged(wxCalendarCtrl& control);
    static bool HasThemeChangedHandler(const wxCalendarCtrl& control);
    static bool GetDateClientPoint(
        wxCalendarCtrl& control,
        const wxDateTime& date,
        wxPoint *point);
    static bool GetPeerRightToLeft(
        const wxCalendarCtrl& control,
        bool *rightToLeft);
    static bool GetPeerCalendarStyle(
        const wxCalendarCtrl& control,
        bool *mondayFirst,
        bool *surroundingWeeks,
        bool *weekNumbers);
    static bool GetWeekNumber(
        wxCalendarCtrl& control,
        const wxDateTime& date,
        wxDateTime *weekStart,
        int *weekNumber);
    static unsigned long long GetWeekRefreshRunCount(const wxCalendarCtrl& control);
    static bool RequestWeekRefresh(wxCalendarCtrl& control);
    static bool SetWeekRefreshFailures(
        wxCalendarCtrl& control,
        unsigned count);
    static bool SetWeekRefreshPendingPasses(
        wxCalendarCtrl& control,
        unsigned count);
    static bool SetWeekRefreshPeerMutationPasses(
        wxCalendarCtrl& control,
        unsigned count);
    static bool RequestDayLayoutValidation(wxCalendarCtrl& control);
    static bool RequestDayLayoutValidationFailure(
        wxCalendarCtrl& control,
        DayLayoutValidationFailure failure =
        DayLayoutValidationFailure::Exception);
    static bool GetWeekRefreshRecovery(
        const wxCalendarCtrl& control,
        WeekRefreshRecovery *recovery);
    static bool ResolveActiveMonthTopology(const std::vector<MonthTopologyDay>& days,
        MonthTopologyResult *result,
        int truncatedMonth = 0,
        int truncatedFirstDay = 0,
        int truncatedLastDay = 0);
    static bool GetStableLayoutTicket(
        const wxCalendarCtrl& control,
        LayoutTicket *ticket);
    static wxString SetLanguage(const wxString& language);
};
#endif

#endif
