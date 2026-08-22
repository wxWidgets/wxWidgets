/////////////////////////////////////////////////////////////////////////////
// Name:        wx/winui/calctrl.h
// Purpose:     wxWinUI wxCalendarCtrl declaration (WinUI CalendarView)
// Author:      wxWidgets development team
// Created:     2026-06-01
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WINUI_CALCTRL_H_
#define _WX_WINUI_CALCTRL_H_

#include <cstdint>
#include <memory>
#include <vector>

class wxWinUICalendarImpl;

class WXDLLIMPEXP_CORE wxCalendarCtrl : public wxCalendarCtrlBase
{
public:
    wxCalendarCtrl();
    wxCalendarCtrl(wxWindow *parent,
                   wxWindowID id,
                   const wxDateTime& date = wxDefaultDateTime,
                   const wxPoint& pos = wxDefaultPosition,
                   const wxSize& size = wxDefaultSize,
                   long style = wxCAL_SHOW_HOLIDAYS,
                   const wxString& name = wxASCII_STR(wxCalendarNameStr));
    ~wxCalendarCtrl() override;

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxDateTime& date = wxDefaultDateTime,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxCAL_SHOW_HOLIDAYS,
                const wxString& name = wxASCII_STR(wxCalendarNameStr));

    bool SetDate(const wxDateTime& date) override;
    wxDateTime GetDate() const override;

    bool SetDateRange(const wxDateTime& lowerdate = wxDefaultDateTime,
                      const wxDateTime& upperdate = wxDefaultDateTime) override;
    bool GetDateRange(wxDateTime *lowerdate, wxDateTime *upperdate) const override;

    wxCalendarHitTestResult
    HitTest(const wxPoint& pos,
            wxDateTime *date = nullptr,
            wxDateTime::WeekDay *wd = nullptr) override;

    bool EnableMonthChange(bool enable = true) override;
    void EnableHolidayDisplay(bool display = true) override;
    void Mark(size_t day, bool mark) override;
    void SetHoliday(size_t day) override;
    void SetWindowStyleFlag(long style) override;
    void SetMinSize(const wxSize& minSize) override;

    // Deterministic seams exercising the real CalendarView peer and the same
    // day-item resolver used by the DoubleTapped delegate.
    bool WinUISetPeerDateForTesting(const wxDateTime& date);
    bool WinUIGetPeerStateForTesting(wxDateTime *date,
                                     wxDateTime *minimum,
                                     wxDateTime *maximum) const;
    bool WinUIGetDefaultPeerRangeForTesting(wxDateTime *minimum,
                                            wxDateTime *maximum) const;
    bool WinUIDoubleTapDateForTesting(const wxDateTime& date);
    bool WinUIDoubleTapNonDayForTesting();
    bool WinUIIsMarkedForTesting(size_t day) const;
    bool WinUIIsHolidayForTesting(size_t day) const;
    bool WinUIGetAppliedDensityColourForTesting(size_t day,
                                                wxColour *colour,
                                                unsigned *colourCount = nullptr,
                                                unsigned long long *writeRevision = nullptr);
    bool WinUISetTodayForegroundForTesting(const wxColour& colour);
    bool WinUIUseSystemMarkColourFallbackForTesting();
    bool WinUIDeliverThemeChangedForTesting();
    bool WinUIHasThemeChangedHandlerForTesting() const;
    bool WinUIGetDateClientPointForTesting(const wxDateTime& date,
                                           wxPoint *point);
    bool WinUIGetPeerRightToLeftForTesting(bool *rightToLeft) const;
    bool WinUIGetPeerCalendarStyleForTesting(
        bool *mondayFirst,
        bool *surroundingWeeks,
        bool *weekNumbers) const;
    bool WinUIGetWeekNumberForTesting(const wxDateTime& date,
                                      wxDateTime *weekStart,
                                      int *weekNumber);
    unsigned long long
    WinUIGetWeekRefreshRunCountForTesting() const;
    bool WinUIRequestWeekRefreshForTesting();
    struct WinUIWeekRefreshRecoveryForTesting
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
    bool WinUISetWeekRefreshFailuresForTesting(unsigned count);
    bool WinUISetWeekRefreshPendingPassesForTesting(unsigned count);
    bool WinUISetWeekRefreshPeerMutationPassesForTesting(unsigned count);
    bool WinUIRequestDayLayoutValidationForTesting();
    enum class WinUIDayLayoutValidationFailureForTesting : unsigned
    {
        Exception = 1,
        TransientRead = 2,
        QueueRefusal = 3
    };
    bool WinUIRequestDayLayoutValidationFailureForTesting(
        WinUIDayLayoutValidationFailureForTesting failure =
            WinUIDayLayoutValidationFailureForTesting::Exception);
    bool WinUIGetWeekRefreshRecoveryForTesting(
        WinUIWeekRefreshRecoveryForTesting *recovery) const;
    struct WinUIMonthTopologyDayForTesting
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
    struct WinUIMonthTopologyResultForTesting
    {
        int month = 0;
        unsigned dayCount = 0;
        unsigned columnCount = 0;
        unsigned rowCount = 0;
        double firstDayTop = 0.0;
    };
    static bool WinUIResolveActiveMonthTopologyForTesting(
        const std::vector<WinUIMonthTopologyDayForTesting>& days,
        WinUIMonthTopologyResultForTesting *result,
        int truncatedMonth = 0,
        int truncatedFirstDay = 0,
        int truncatedLastDay = 0);
    struct WinUILayoutTicketForTesting
    {
        std::uint64_t layoutRevision = 0;
        std::uint64_t realizedDaysRevision = 0;
        std::uint64_t weekRefreshRevision = 0;
        std::uint64_t navigationGeneration = 0;
        std::uintptr_t decrementIdentity = 0;
        std::uintptr_t incrementIdentity = 0;
    };
    bool WinUIGetStableLayoutTicketForTesting(
        WinUILayoutTicketForTesting *ticket) const;
    static wxString WinUISetLanguageForTesting(const wxString& language);

protected:
    wxSize DoGetBestSize() const override;
    void DoSetSize(int x, int y, int width, int height,
                   int sizeFlags) override;

    bool ApplyInitialSizeTransaction(const wxSize& size,
                                     std::uint64_t expectedRevision);
    void OnLayoutLoaded();
    bool ApplyToPeer();
    bool ApplyStateToPeer(const wxDateTime& date,
                          const wxDateTime& lowerdate,
                          const wxDateTime& upperdate,
                          bool monthChangeEnabled);
    void OnPeerSelectionChanged();
    void HandleVisualThemeChanged();
    void OnSysColourChanged(wxSysColourChangedEvent& event);
    void ResetHolidayAttrs() override;
    void RefreshHolidays() override;

    std::unique_ptr<wxWinUICalendarImpl> m_winui;
    wxDateTime m_date;
    wxDateTime m_lowerDate;
    wxDateTime m_upperDate;
    std::uint32_t m_marks = 0;
    std::uint32_t m_holidays = 0;

private:
    friend class wxWinUICalendarImpl;

    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxCalendarCtrl);
};

#endif // _WX_WINUI_CALCTRL_H_
