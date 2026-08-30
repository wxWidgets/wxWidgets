/////////////////////////////////////////////////////////////////////////////
// Name:        src/winui/calctrl.cpp
// Purpose:     wxWinUI wxCalendarCtrl implementation (WinUI CalendarView)
// Author:      wxWidgets development team
// Created:     2026-06-01
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_CALENDARCTRL

#include "wx/calctrl.h"

#ifdef WXWINUI_TEST_SUPPORT
    #include "calendar-test-access.h"
#endif
#include "wx/log.h"
#include "wx/settings.h"
#include "wx/weakref.h"

#include "private.h"
#include "wx/winui/private/tlwhost.h"

#include <winrt/Microsoft.UI.Dispatching.h>
#include <winrt/Microsoft.UI.Xaml.Controls.Primitives.h>
#include <winrt/Microsoft.UI.Xaml.Input.h>
#include <winrt/Microsoft.UI.Xaml.Media.h>
#include <winrt/Windows.Globalization.h>
#include <winrt/Windows.Globalization.NumberFormatting.h>
#include <winrt/Windows.UI.h>

#include <algorithm>
#include <array>
#include <atomic>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <limits>
#include <map>
#include <memory>
#include <set>
#include <tuple>
#include <utility>
#include <vector>

namespace MUX = winrt::Microsoft::UI::Xaml;
namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;
namespace MUXCP = winrt::Microsoft::UI::Xaml::Controls::Primitives;
namespace MUXD = winrt::Microsoft::UI::Dispatching;
namespace MUXM = winrt::Microsoft::UI::Xaml::Media;
namespace WF = winrt::Windows::Foundation;
namespace WG = winrt::Windows::Globalization;
namespace WGNF = winrt::Windows::Globalization::NumberFormatting;
namespace WU = winrt::Windows::UI;

// wxCalendarCtrl RTTI and the wxEVT_CALENDAR_* events are provided by
// src/common/calctrlcmn.cpp.

namespace
{

#ifdef WXWINUI_TEST_SUPPORT
winrt::hstring gs_winuiCalendarLanguageForTesting;
#endif

WGNF::DecimalFormatter wxWinUICreateCalendarWeekNumberFormatter(
    const winrt::hstring& language)
{
    WGNF::DecimalFormatter formatter = language.empty()
        ? WGNF::DecimalFormatter()
        : WGNF::DecimalFormatter(
              std::vector<winrt::hstring>{language},
              WG::GeographicRegion().CodeTwoLetter());
    formatter.IntegerDigits(1);
    formatter.FractionDigits(0);
    return formatter;
}

wxDateTime wxWinUINormalizeCivilDate(const wxDateTime& value)
{
    if ( !value.IsValid() )
        return wxDefaultDateTime;

    wxDateTime date(value);
    date.ResetTime();
    return date;
}

// CalendarView represents a civil wx date with an instant-based WinRT value.
// Anchor that date at local noon: unlike midnight, noon does not normally
// intersect a daylight-saving gap and round-trips to the same civil day.
bool wxWinUIToDateTime(const wxDateTime& value, WF::DateTime *result)
{
    const wxDateTime date = wxWinUINormalizeCivilDate(value);
    if ( !date.IsValid() || !result )
        return false;

    try
    {
        // FILETIME cannot represent the negative WinRT ticks used for local
        // Gregorian dates before 1601. Windows.Globalization.Calendar spans
        // the complete WinRT calendar envelope and performs the same local
        // timezone projection without truncating those values.
        WG::Calendar calendar;
        calendar.ChangeCalendarSystem(
            WG::CalendarIdentifiers::Gregorian());
        calendar.ChangeClock(WG::ClockIdentifiers::TwentyFourHour());
        calendar.SetToNow();
        // Set the local-noon anchor before projecting historical civil
        // fields: an intermediate Y/M/D at the current wall clock may be an
        // invalid local instant even when noon on the requested date is not.
        calendar.Hour(12);
        calendar.Minute(0);
        calendar.Second(0);
        calendar.Nanosecond(0);
        calendar.Day(1);
        calendar.Month(1);
        calendar.Year(date.GetYear());
        calendar.Month(date.GetMonth() + 1);
        calendar.Day(date.GetDay());
        *result = calendar.GetDateTime();
        return calendar.Year() == date.GetYear() &&
               calendar.Month() == date.GetMonth() + 1 &&
               calendar.Day() == date.GetDay();
    }
    catch ( const winrt::hresult_error& )
    {
        return false;
    }
}

wxDateTime wxWinUIFromDateTime(const WF::DateTime& value)
{
    try
    {
        WG::Calendar calendar;
        calendar.ChangeCalendarSystem(
            WG::CalendarIdentifiers::Gregorian());
        calendar.ChangeClock(WG::ClockIdentifiers::TwentyFourHour());
        calendar.SetDateTime(value);
        const int year = calendar.Year();
        const int month = calendar.Month();
        const int day = calendar.Day();
        if ( year < 1 || year > 9999 || month < 1 || month > 12 ||
             day < 1 || day > 31 )
        {
            return wxDefaultDateTime;
        }

        const wxDateTime date(
            static_cast<wxDateTime::wxDateTime_t>(day),
            static_cast<wxDateTime::Month>(month - 1), year);
        return date.IsValid() ? date : wxDefaultDateTime;
    }
    catch ( const winrt::hresult_error& )
    {
        return wxDefaultDateTime;
    }
}

bool wxWinUIResolveCalendarEnvelope(
    const winrt::hstring& calendarIdentifier,
    WF::DateTime *minimum,
    WF::DateTime *maximum)
{
    if ( calendarIdentifier.empty() || !minimum || !maximum )
        return false;

    try
    {
        WG::Calendar calendar;
        calendar.ChangeCalendarSystem(calendarIdentifier);
        calendar.SetToMin();
        const WF::DateTime rawMinimum = calendar.GetDateTime();
        calendar.SetToMax();
        const WF::DateTime rawMaximum = calendar.GetDateTime();

        // A timezone conversion of the raw extrema can produce civil year 0
        // or 10000 even though the WinRT value itself is valid. Move the raw
        // instant inward before converting it to a wx civil date, then anchor
        // that date at local noon and verify it against the exact calendar
        // envelope.
        const WF::TimeSpan oneDay =
            std::chrono::duration_cast<WF::TimeSpan>(
                std::chrono::hours(24));
        const auto resolveEndpoint =
            [&](const WF::DateTime& endpoint,
                bool resolveMinimum,
                WF::DateTime *resolved)
            {
                for ( int n = 0; n != 3; ++n )
                {
                    const WF::TimeSpan offset = oneDay * n;
                    const WF::DateTime probe{
                        endpoint.time_since_epoch() +
                        (resolveMinimum ? offset : -offset)};
                    if ( probe.time_since_epoch() <
                             rawMinimum.time_since_epoch() ||
                         probe.time_since_epoch() >
                             rawMaximum.time_since_epoch() )
                    {
                        continue;
                    }

                    const wxDateTime civil =
                        wxWinUIFromDateTime(probe);
                    WF::DateTime candidate{};
                    if ( civil.IsValid() &&
                         wxWinUIToDateTime(civil, &candidate) &&
                         candidate.time_since_epoch() >=
                             rawMinimum.time_since_epoch() &&
                         candidate.time_since_epoch() <=
                             rawMaximum.time_since_epoch() )
                    {
                        calendar.SetDateTime(candidate);
                        *resolved = candidate;
                        return true;
                    }
                }
                return false;
            };

        const bool haveMinimum =
            resolveEndpoint(rawMinimum, true, minimum);
        const bool haveMaximum =
            resolveEndpoint(rawMaximum, false, maximum);
        return haveMinimum && haveMaximum &&
               minimum->time_since_epoch() <=
                   maximum->time_since_epoch();
    }
    catch ( const winrt::hresult_error& )
    {
        return false;
    }
}

struct wxWinUICalendarMonthKey
{
    int era = 0;
    int year = 0;
    int month = 0;
    bool valid = false;

    bool operator==(const wxWinUICalendarMonthKey& other) const
    {
        return valid && other.valid &&
               era == other.era && year == other.year &&
               month == other.month;
    }

    bool operator<(const wxWinUICalendarMonthKey& other) const
    {
        return std::tie(era, year, month) <
               std::tie(other.era, other.year, other.month);
    }
};

bool wxWinUIReadCalendarDay(
    const winrt::hstring& calendarIdentifier,
    const WF::DateTime& value,
    wxWinUICalendarMonthKey *month,
    int *day,
    int *daysInMonth = nullptr)
{
    if ( calendarIdentifier.empty() )
        return false;

    try
    {
        WG::Calendar calendar;
        calendar.ChangeCalendarSystem(calendarIdentifier);
        calendar.SetDateTime(value);
        if ( month )
        {
            month->era = calendar.Era();
            month->year = calendar.Year();
            month->month = calendar.Month();
            month->valid = true;
        }
        if ( day )
            *day = calendar.Day();
        if ( daysInMonth )
            *daysInMonth = calendar.NumberOfDaysInThisMonth();
        return true;
    }
    catch ( const winrt::hresult_error& )
    {
        return false;
    }
}

wxDateTime::WeekDay wxWinUIFromDayOfWeek(WG::DayOfWeek day)
{
    switch ( day )
    {
        case WG::DayOfWeek::Sunday: return wxDateTime::Sun;
        case WG::DayOfWeek::Monday: return wxDateTime::Mon;
        case WG::DayOfWeek::Tuesday: return wxDateTime::Tue;
        case WG::DayOfWeek::Wednesday: return wxDateTime::Wed;
        case WG::DayOfWeek::Thursday: return wxDateTime::Thu;
        case WG::DayOfWeek::Friday: return wxDateTime::Fri;
        case WG::DayOfWeek::Saturday: return wxDateTime::Sat;
    }
    return wxDateTime::Sun;
}

int wxWinUICalendarWeekNumber(
    const winrt::hstring& calendarIdentifier,
    const wxDateTime& weekStart,
    wxDateTime::WeekDay firstWeekday)
{
    if ( calendarIdentifier.empty() || !weekStart.IsValid() )
        return 0;

    try
    {
        // The fourth day determines the week-year under the native
        // MCS_WEEKNUMBERS rule (the first week has at least four days).
        WF::DateTime anchorPeer{};
        if ( !wxWinUIToDateTime(
                 weekStart + wxDateSpan::Days(3),
                 &anchorPeer) )
        {
            return 0;
        }

        WG::Calendar calendar;
        calendar.ChangeCalendarSystem(calendarIdentifier);
        calendar.SetDateTime(anchorPeer);
        calendar.Day(calendar.FirstDayInThisMonth());
        calendar.Month(calendar.FirstMonthInThisYear());
        calendar.Day(calendar.FirstDayInThisMonth());
        const wxDateTime firstDate =
            wxWinUIFromDateTime(calendar.GetDateTime());
        if ( !firstDate.IsValid() )
            return 0;

        const int offset =
            (static_cast<int>(firstDate.GetWeekDay()) -
             static_cast<int>(firstWeekday) + 7) % 7;
        wxDateTime firstWeekStart =
            firstDate - wxDateSpan::Days(offset);
        if ( 7 - offset < 4 )
            firstWeekStart += wxDateSpan::Days(7);

        // These are civil dates, not instants. Subtracting wxDateTime values
        // across a daylight-saving transition can produce e.g. 132 days and
        // 23 hours for two dates that are 133 calendar days apart. Counting
        // Gregorian civil ordinals keeps the native week rule independent of
        // the process time zone and of the current UTC offset.
        const auto civilOrdinal = [](const wxDateTime& date)
        {
            const long long previousYears =
                static_cast<long long>(date.GetYear()) - 1;
            return previousYears * 365 + previousYears / 4 -
                   previousYears / 100 + previousYears / 400 +
                   static_cast<long long>(date.GetDayOfYear());
        };
        const long long count =
            civilOrdinal(weekStart) - civilOrdinal(firstWeekStart);
        return count < 0 ? 0 : static_cast<int>(count / 7 + 1);
    }
    catch ( const winrt::hresult_error& )
    {
        return 0;
    }
}

bool wxWinUISameCivilDate(const wxDateTime& lhs,
                          const wxDateTime& rhs)
{
    if ( lhs.IsValid() != rhs.IsValid() )
        return false;
    return !lhs.IsValid() ||
           wxWinUINormalizeCivilDate(lhs) ==
               wxWinUINormalizeCivilDate(rhs);
}

bool wxWinUISameCalendarMonth(
    const winrt::hstring& calendarIdentifier,
    const wxDateTime& lhs,
    const wxDateTime& rhs)
{
    if ( !lhs.IsValid() || !rhs.IsValid() )
        return false;

    WF::DateTime lhsPeer{};
    WF::DateTime rhsPeer{};
    wxWinUICalendarMonthKey lhsMonth;
    wxWinUICalendarMonthKey rhsMonth;
    return wxWinUIToDateTime(lhs, &lhsPeer) &&
           wxWinUIToDateTime(rhs, &rhsPeer) &&
           wxWinUIReadCalendarDay(
               calendarIdentifier, lhsPeer, &lhsMonth, nullptr) &&
           wxWinUIReadCalendarDay(
               calendarIdentifier, rhsPeer, &rhsMonth, nullptr) &&
           lhsMonth == rhsMonth;
}

bool wxWinUIIsInsideRange(const wxDateTime& value,
                          const wxDateTime& minimum,
                          const wxDateTime& maximum)
{
    return value.IsValid() &&
           (!minimum.IsValid() || value >= minimum) &&
           (!maximum.IsValid() || value <= maximum);
}

int wxWinUICivilDateKey(const wxDateTime& value)
{
    const wxDateTime date = wxWinUINormalizeCivilDate(value);
    if ( !date.IsValid() )
        return 0;

    return date.GetYear() * 10000 +
           (static_cast<int>(date.GetMonth()) + 1) * 100 +
           static_cast<int>(date.GetDay());
}

void wxWinUIApplyCalendarBounds(
    const MUXC::CalendarView& calendar,
    const WF::DateTime& minimum,
    const WF::DateTime& maximum)
{
    const WF::DateTime currentMinimum = calendar.MinDate();
    const WF::DateTime currentMaximum = calendar.MaxDate();

    // Avoid a transient invalid interval when moving two disjoint ranges.
    if ( minimum.time_since_epoch() >
         currentMaximum.time_since_epoch() )
    {
        calendar.MaxDate(maximum);
        calendar.MinDate(minimum);
    }
    else if ( maximum.time_since_epoch() <
              currentMinimum.time_since_epoch() )
    {
        calendar.MinDate(minimum);
        calendar.MaxDate(maximum);
    }
    else
    {
        calendar.MinDate(minimum);
        calendar.MaxDate(maximum);
    }
}

bool wxWinUIReadSelectedDate(const MUXC::CalendarView& calendar,
                             wxDateTime *result)
{
    if ( !result )
        return true;

    const auto selected = calendar.SelectedDates();
    if ( selected.Size() != 1 )
    {
        *result = wxDefaultDateTime;
        return false;
    }

    *result = wxWinUIFromDateTime(selected.GetAt(0));
    return result->IsValid();
}

class wxWinUICalendarCallbackState
{
public:
    explicit wxWinUICalendarCallbackState(wxCalendarCtrl *owner)
        : m_owner(owner)
    {
    }

    std::uint64_t Generation() const
    {
        return m_generation.load(std::memory_order_acquire);
    }

    wxCalendarCtrl *GetOwner(std::uint64_t generation) const
    {
        if ( m_generation.load(std::memory_order_acquire) != generation )
            return nullptr;
        return m_owner.load(std::memory_order_acquire);
    }

    void Invalidate()
    {
        m_owner.store(nullptr, std::memory_order_release);
        m_generation.fetch_add(1, std::memory_order_acq_rel);
    }

    void BeginPeerMutation()
    {
        m_peerMutationDepth.fetch_add(1, std::memory_order_acq_rel);
    }

    bool EndPeerMutation()
    {
        const unsigned previous =
            m_peerMutationDepth.fetch_sub(1, std::memory_order_acq_rel);
        wxASSERT_MSG(previous != 0,
                     "unbalanced WinUI calendar peer mutation");
        return previous == 1;
    }

    bool IsPeerMutationInProgress() const
    {
        return m_peerMutationDepth.load(std::memory_order_acquire) != 0;
    }

    std::uint64_t BeginStateMutation()
    {
        return m_stateRevision.fetch_add(1, std::memory_order_acq_rel) + 1;
    }

    std::uint64_t StateRevision() const
    {
        return m_stateRevision.load(std::memory_order_acquire);
    }

    bool IsStateMutationCurrent(std::uint64_t revision) const
    {
        return StateRevision() == revision;
    }

private:
    std::atomic<wxCalendarCtrl *> m_owner;
    std::atomic<std::uint64_t> m_generation{1};
    std::atomic<unsigned> m_peerMutationDepth{0};
    std::atomic<std::uint64_t> m_stateRevision{1};
};

class wxWinUICalendarPeerMutationGuard
{
public:
    explicit wxWinUICalendarPeerMutationGuard(
        std::shared_ptr<wxWinUICalendarCallbackState> state)
        : m_state(std::move(state))
    {
        if ( m_state )
            m_state->BeginPeerMutation();
    }

    ~wxWinUICalendarPeerMutationGuard();

    wxWinUICalendarPeerMutationGuard(
        const wxWinUICalendarPeerMutationGuard&) = delete;
    wxWinUICalendarPeerMutationGuard& operator=(
        const wxWinUICalendarPeerMutationGuard&) = delete;

private:
    std::shared_ptr<wxWinUICalendarCallbackState> m_state;
};

bool wxWinUIDispatchCalendarEvent(
    const std::shared_ptr<wxWinUICalendarCallbackState>& state,
    std::uint64_t generation,
    const wxDateTime& date,
    wxEventType type)
{
    wxCalendarCtrl * const owner = state->GetOwner(generation);
    if ( !owner )
        return false;

    wxCalendarEvent event(owner, date, type);
    owner->HandleWindowEvent(event);

    // The handler is allowed to destroy the control. The callback state is
    // independently owned and is the only object inspected after dispatch.
    return state->GetOwner(generation) != nullptr;
}

void wxWinUIDispatchCalendarSelectionEvents(
    const std::shared_ptr<wxWinUICalendarCallbackState>& state,
    std::uint64_t generation,
    const wxDateTime& oldDate,
    const wxDateTime& newDate)
{
    // Keep this classification byte-for-byte equivalent in meaning to
    // wxCalendarCtrlBase::GenerateAllChangeEvents(), which is what wxMSW
    // invokes for MCN_SELCHANGE. CalendarView navigation itself follows the
    // active WinRT CalendarIdentifier, but the public legacy event split is
    // defined by the civil wxDateTime year/month, even for alternate calendars.
    const wxDateTime::Tm oldTm = oldDate.GetTm();
    const wxDateTime::Tm newTm = newDate.GetTm();
    const bool sameYear = oldTm.year == newTm.year;
    const bool sameMonth = sameYear && oldTm.mon == newTm.mon;

    if ( !wxWinUIDispatchCalendarEvent(
             state, generation, newDate,
             wxEVT_CALENDAR_SEL_CHANGED) )
    {
        return;
    }

    if ( !sameMonth )
    {
        if ( !wxWinUIDispatchCalendarEvent(
                 state, generation, newDate,
                 wxEVT_CALENDAR_PAGE_CHANGED) )
        {
            return;
        }
    }

    const wxEventType legacyType =
        !sameYear
            ? wxEVT_CALENDAR_YEAR_CHANGED
            : !sameMonth
                ? wxEVT_CALENDAR_MONTH_CHANGED
                : wxEVT_CALENDAR_DAY_CHANGED;
    wxWinUIDispatchCalendarEvent(
        state, generation, newDate, legacyType);
}

WU::Color wxWinUICalendarMarkColour(
    const MUXC::CalendarView& calendar)
{
    try
    {
        if ( const auto brush =
                 calendar.TodayForeground()
                     .try_as<MUXM::SolidColorBrush>() )
        {
            return brush.Color();
        }
    }
    catch ( const winrt::hresult_error& )
    {
    }

    const wxColour highlight =
        wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT);
    WU::Color colour{};
    colour.A = highlight.IsOk() ? highlight.Alpha() : 255;
    colour.R = highlight.IsOk() ? highlight.Red() : 0;
    colour.G = highlight.IsOk() ? highlight.Green() : 0;
    colour.B = highlight.IsOk() ? highlight.Blue() : 0;
    return colour;
}

} // namespace

class wxWinUICalendarImpl
{
public:
    static constexpr unsigned MaxWeekRefreshAutomaticRetries = 3;
    enum class WeekRefreshPendingCause : unsigned
    {
        None,
        PeerMutation,
        Reentrant,
        InvalidTicket,
        QueueFailure
    };
    enum class NavigationRefreshFailure : unsigned
    {
        None,
        SnapshotUnavailable,
        CandidatePairUnavailable,
        DetachedPeer,
        Superseded,
        HandlerRegistration
    };

    using RealizedDay =
        winrt::weak_ref<MUXC::CalendarViewDayItem>;

    struct RealizedDayTicket
    {
        int dateKey = 0;
        RealizedDay item;
    };

    struct WeekRow
    {
        MUXC::Border surface{ nullptr };
        MUXC::TextBlock label{ nullptr };
        wxDateTime startDate;
        int weekNumber = 0;
        double top = 0.0;
        double height = 0.0;
        bool visible = false;
    };

    struct WeekHitTicket
    {
        MUX::FrameworkElement element{ nullptr };
        wxDateTime startDate;
    };

    struct HitTestTicket
    {
        wxWinUICalendarImpl *implementation = nullptr;
        std::shared_ptr<wxWinUICalendarCallbackState> callbackState;
        std::uint64_t callbackGeneration = 0;
        std::uint64_t realizedDaysRevision = 0;
        MUXC::CalendarView calendar{ nullptr };
        std::vector<RealizedDayTicket> realizedDays;
        std::vector<WeekHitTicket> weekRows;
    };

    struct WeekLayoutInputs
    {
        MUX::XamlRoot xamlRoot{ nullptr };
        std::uint64_t layoutRevision = 0;
        std::uint64_t realizedDaysRevision = 0;
        double width = 0.0;
        double height = 0.0;
    };

    struct WeekDayLayoutInput
    {
        int dateKey = 0;
        double x = 0.0;
        double y = 0.0;
        double width = 0.0;
        double height = 0.0;
    };

    using WeekDayLayoutSignature =
        std::vector<WeekDayLayoutInput>;

    ~wxWinUICalendarImpl()
    {
        Close();
    }

    void NoteRealizedDaysChanged()
    {
        if ( ++realizedDaysRevision == 0 )
            ++realizedDaysRevision;
    }

    HitTestTicket CaptureHitTestTicket() const
    {
        HitTestTicket ticket;
        ticket.implementation =
            const_cast<wxWinUICalendarImpl *>(this);
        ticket.callbackState = callbackState;
        if ( ticket.callbackState )
        {
            ticket.callbackGeneration =
                ticket.callbackState->Generation();
        }
        ticket.realizedDaysRevision = realizedDaysRevision;
        ticket.calendar = cal;
        ticket.realizedDays.reserve(realizedDays.size());
        for ( const auto& realized : realizedDays )
        {
            ticket.realizedDays.push_back(
                RealizedDayTicket{realized.first, realized.second});
        }
        ticket.weekRows.reserve(weekRows.size());
        for ( const WeekRow& row : weekRows )
        {
            if ( row.visible && row.surface &&
                 row.startDate.IsValid() )
            {
                ticket.weekRows.push_back(
                    WeekHitTicket{
                        row.surface.as<MUX::FrameworkElement>(),
                        row.startDate});
            }
        }
        return ticket;
    }

    static wxCalendarCtrl *GetCurrentPeerOwner(
        const HitTestTicket& ticket)
    {
        if ( !ticket.implementation ||
             !ticket.callbackState ||
             !ticket.calendar ||
             ticket.callbackState->IsPeerMutationInProgress() )
        {
            return nullptr;
        }

        wxCalendarCtrl * const owner =
            ticket.callbackState->GetOwner(
                ticket.callbackGeneration);
        if ( !owner || !owner->m_winui )
            return nullptr;

        wxWinUICalendarImpl * const implementation =
            owner->m_winui.get();
        if ( implementation != ticket.implementation ||
             implementation->callbackState !=
                 ticket.callbackState ||
             winrt::get_abi(implementation->cal) !=
                 winrt::get_abi(ticket.calendar) )
        {
            return nullptr;
        }

        return owner;
    }

    static wxCalendarCtrl *GetCurrentOwner(
        const HitTestTicket& ticket)
    {
        wxCalendarCtrl * const owner = GetCurrentPeerOwner(ticket);
        return owner && owner->m_winui->realizedDaysRevision ==
                            ticket.realizedDaysRevision
                   ? owner
                   : nullptr;
    }

    struct DayHitGeometry
    {
        MUXC::CalendarViewDayItem item{ nullptr };
        WF::Rect bounds{};
        wxDateTime date;
        wxWinUICalendarMonthKey month;
        int calendarDay = 0;
        int daysInMonth = 0;
        bool authoritative = false;
    };

    struct ElementHitGeometry
    {
        MUX::FrameworkElement element{ nullptr };
        WF::Rect bounds{};
        bool isCalendarHeader = false;
    };

    struct PublicHitSnapshot
    {
        std::vector<DayHitGeometry> days;
        std::vector<ElementHitGeometry> weekdayLabels;
        std::vector<ElementHitGeometry> buttons;
        struct WeekHitGeometry
        {
            ElementHitGeometry element;
            wxDateTime startDate;
        };
        std::vector<WeekHitGeometry> weeks;
    };

    static bool ContainsPoint(const WF::Rect& bounds,
                              const WF::Point& point)
    {
        return point.X >= bounds.X &&
               point.Y >= bounds.Y &&
               point.X < bounds.X + bounds.Width &&
               point.Y < bounds.Y + bounds.Height;
    }

    static bool ReadElementBounds(
        const HitTestTicket& ticket,
        const MUX::FrameworkElement& element,
        bool requireHitTestVisible,
        WF::Rect *bounds)
    {
        if ( !element || !bounds || !GetCurrentOwner(ticket) )
            return false;

        const MUX::Visibility visibility = element.Visibility();
        if ( !GetCurrentOwner(ticket) ||
             visibility != MUX::Visibility::Visible )
        {
            return false;
        }

        if ( requireHitTestVisible )
        {
            const bool hitTestVisible = element.IsHitTestVisible();
            if ( !GetCurrentOwner(ticket) || !hitTestVisible )
                return false;
        }

        const double width = element.ActualWidth();
        if ( !GetCurrentOwner(ticket) )
            return false;
        const double height = element.ActualHeight();
        if ( !GetCurrentOwner(ticket) ||
             !std::isfinite(width) ||
             !std::isfinite(height) ||
             width <= 0.0 || height <= 0.0 )
        {
            return false;
        }

        const MUXM::GeneralTransform transform =
            element.TransformToVisual(ticket.calendar);
        if ( !GetCurrentOwner(ticket) )
            return false;
        const WF::Rect transformed = transform.TransformBounds(
            WF::Rect{
                0.0f,
                0.0f,
                static_cast<float>(width),
                static_cast<float>(height)
            });
        if ( !GetCurrentOwner(ticket) ||
             !std::isfinite(transformed.X) ||
             !std::isfinite(transformed.Y) ||
             !std::isfinite(transformed.Width) ||
             !std::isfinite(transformed.Height) ||
             transformed.Width <= 0.0f ||
             transformed.Height <= 0.0f )
        {
            return false;
        }

        *bounds = transformed;
        return true;
    }

    static bool IsElementAttachedToCalendar(
        const HitTestTicket& ticket,
        const MUX::FrameworkElement& element)
    {
        if ( !element || !GetCurrentOwner(ticket) )
            return false;

        const MUX::XamlRoot calendarRoot = ticket.calendar.XamlRoot();
        if ( !GetCurrentOwner(ticket) )
            return false;
        const MUX::XamlRoot elementRoot = element.XamlRoot();
        if ( !GetCurrentOwner(ticket) || !calendarRoot ||
             elementRoot != calendarRoot )
        {
            return false;
        }

        MUX::DependencyObject current = element;
        constexpr unsigned MaxAncestorDepth = 2048;
        for ( unsigned depth = 0; current && depth != MaxAncestorDepth;
              ++depth )
        {
            // get_abi() returns the pointer for the projected interface, not
            // the controlling-IUnknown identity. A DependencyObject pointer
            // can therefore differ from the CalendarView pointer for this
            // very same object. Compare two CalendarView projections instead.
            const MUXC::CalendarView currentCalendar =
                current.try_as<MUXC::CalendarView>();
            if ( !GetCurrentOwner(ticket) )
                return false;
            if ( currentCalendar &&
                 winrt::get_abi(currentCalendar) ==
                     winrt::get_abi(ticket.calendar) )
            {
                return GetCurrentOwner(ticket) != nullptr;
            }
            current = MUXM::VisualTreeHelper::GetParent(current);
            if ( !GetCurrentOwner(ticket) )
                return false;
        }
        return false;
    }

    static bool AppendDayGeometry(
        const HitTestTicket& ticket,
        const MUXC::CalendarViewDayItem& item,
        double calendarWidth,
        double calendarHeight,
        PublicHitSnapshot *snapshot,
        bool authoritative = false,
        int authoritativeDateKey = 0)
    {
        if ( !snapshot || !item || !GetCurrentOwner(ticket) )
            return false;

        for ( DayHitGeometry& existing : snapshot->days )
        {
            if ( winrt::get_abi(existing.item) ==
                 winrt::get_abi(item) )
            {
                if ( authoritative )
                    existing.authoritative = true;
                return true;
            }
        }

        const MUX::FrameworkElement element =
            item.as<MUX::FrameworkElement>();
        if ( !GetCurrentOwner(ticket) )
            return false;

        WF::Rect bounds{};
        if ( !ReadElementBounds(ticket, element, true, &bounds) )
            return GetCurrentOwner(ticket) != nullptr;

        const double centreX =
            static_cast<double>(bounds.X) +
            static_cast<double>(bounds.Width) / 2.0;
        const double centreY =
            static_cast<double>(bounds.Y) +
            static_cast<double>(bounds.Height) / 2.0;
        if ( centreX < 0.0 || centreY < 0.0 ||
             centreX >= calendarWidth ||
             centreY >= calendarHeight )
        {
            return true;
        }

        const WF::DateTime peerDate = item.Date();
        if ( !GetCurrentOwner(ticket) )
            return false;
        const wxDateTime date = wxWinUIFromDateTime(peerDate);
        if ( !date.IsValid() )
            return true;
        if ( authoritative &&
             wxWinUICivilDateKey(date) != authoritativeDateKey )
        {
            // The map entry was captured before this container was recycled.
            // Do not lend its old authoritative identity to the new date; the
            // bounded visual-tree pass below may still capture it as fallback.
            return true;
        }

        wxWinUICalendarMonthKey month;
        int calendarDay = 0;
        int daysInMonth = 0;
        if ( !wxWinUIReadCalendarDay(
                 ticket.calendar.CalendarIdentifier(),
                 peerDate, &month, &calendarDay, &daysInMonth) )
        {
            return true;
        }

        snapshot->days.push_back(
            DayHitGeometry{
                item, bounds, date, month, calendarDay,
                daysInMonth, authoritative});
        return true;
    }

    static bool SameElement(
        const MUX::FrameworkElement& lhs,
        const MUX::FrameworkElement& rhs)
    {
        return lhs && rhs &&
               winrt::get_abi(lhs) == winrt::get_abi(rhs);
    }

    static bool AppendElementGeometry(
        const HitTestTicket& ticket,
        const MUX::FrameworkElement& element,
        bool requireHitTestVisible,
        std::vector<ElementHitGeometry> *elements,
        bool isCalendarHeader = false)
    {
        if ( !elements || !element || !GetCurrentOwner(ticket) )
            return false;

        for ( const ElementHitGeometry& existing : *elements )
        {
            if ( SameElement(existing.element, element) )
                return true;
        }

        WF::Rect bounds{};
        if ( !ReadElementBounds(
                 ticket, element, requireHitTestVisible, &bounds) )
        {
            return GetCurrentOwner(ticket) != nullptr;
        }

        elements->push_back(
            ElementHitGeometry{element, bounds, isCalendarHeader});
        return true;
    }

    static bool IsWeekdayLabel(
        const winrt::hstring& text,
        const std::array<winrt::hstring, 7>& labels)
    {
        if ( text.empty() )
            return false;

        return std::find(labels.begin(), labels.end(), text) !=
               labels.end();
    }

    static bool CapturePublicHitSnapshot(
        const HitTestTicket& ticket,
        PublicHitSnapshot *snapshot)
    {
        if ( !snapshot || !GetCurrentOwner(ticket) )
            return false;

        const double calendarWidth = ticket.calendar.ActualWidth();
        if ( !GetCurrentOwner(ticket) )
            return false;
        const double calendarHeight = ticket.calendar.ActualHeight();
        if ( !GetCurrentOwner(ticket) ||
             !std::isfinite(calendarWidth) ||
             !std::isfinite(calendarHeight) ||
             calendarWidth <= 0.0 || calendarHeight <= 0.0 )
        {
            return false;
        }

        for ( const WeekHitTicket& week : ticket.weekRows )
        {
            WF::Rect bounds{};
            if ( !ReadElementBounds(
                     ticket, week.element, true, &bounds) )
            {
                if ( !GetCurrentOwner(ticket) )
                    return false;
                continue;
            }
            snapshot->weeks.push_back(
                PublicHitSnapshot::WeekHitGeometry{
                    ElementHitGeometry{week.element, bounds},
                    week.startDate});
        }

        for ( const RealizedDayTicket& realized : ticket.realizedDays )
        {
            const MUXC::CalendarViewDayItem item = realized.item.get();
            if ( !GetCurrentOwner(ticket) )
                return false;
            if ( item &&
                 !AppendDayGeometry(
                     ticket, item, calendarWidth,
                     calendarHeight, snapshot, true,
                     realized.dateKey) )
            {
                return false;
            }
        }

        const MUXCP::CalendarViewTemplateSettings settings =
            ticket.calendar.TemplateSettings();
        if ( !GetCurrentOwner(ticket) )
            return false;
        const winrt::hstring headerText = settings.HeaderText();
        if ( !GetCurrentOwner(ticket) )
            return false;

        std::array<winrt::hstring, 7> weekdayLabels;
        weekdayLabels[0] = settings.WeekDay1();
        if ( !GetCurrentOwner(ticket) )
            return false;
        weekdayLabels[1] = settings.WeekDay2();
        if ( !GetCurrentOwner(ticket) )
            return false;
        weekdayLabels[2] = settings.WeekDay3();
        if ( !GetCurrentOwner(ticket) )
            return false;
        weekdayLabels[3] = settings.WeekDay4();
        if ( !GetCurrentOwner(ticket) )
            return false;
        weekdayLabels[4] = settings.WeekDay5();
        if ( !GetCurrentOwner(ticket) )
            return false;
        weekdayLabels[5] = settings.WeekDay6();
        if ( !GetCurrentOwner(ticket) )
            return false;
        weekdayLabels[6] = settings.WeekDay7();
        if ( !GetCurrentOwner(ticket) )
            return false;

        // Only walk the already-realized CalendarView subtree. The explicit
        // node ceiling makes malformed/custom templates fail closed rather
        // than turning a public HitTest() into an unbounded tree operation.
        constexpr std::size_t MaxRealizedNodes = 2048;
        std::vector<MUX::DependencyObject> pending;
        pending.push_back(ticket.calendar);
        std::size_t visited = 0;
        while ( !pending.empty() )
        {
            if ( ++visited > MaxRealizedNodes )
                return false;

            const MUX::DependencyObject current = pending.back();
            pending.pop_back();

            const MUXC::CalendarViewDayItem dayItem =
                current.try_as<MUXC::CalendarViewDayItem>();
            if ( !GetCurrentOwner(ticket) )
                return false;
            if ( dayItem &&
                 !AppendDayGeometry(
                     ticket, dayItem, calendarWidth,
                     calendarHeight, snapshot) )
            {
                return false;
            }

            const MUXC::TextBlock textBlock =
                current.try_as<MUXC::TextBlock>();
            if ( !GetCurrentOwner(ticket) )
                return false;
            if ( textBlock )
            {
                const winrt::hstring text = textBlock.Text();
                if ( !GetCurrentOwner(ticket) )
                    return false;
                if ( IsWeekdayLabel(text, weekdayLabels) &&
                     !AppendElementGeometry(
                         ticket,
                         textBlock.as<MUX::FrameworkElement>(),
                         false, &snapshot->weekdayLabels) )
                {
                    return false;
                }
                if ( !GetCurrentOwner(ticket) )
                    return false;
            }

            const MUXCP::ButtonBase button =
                current.try_as<MUXCP::ButtonBase>();
            if ( !GetCurrentOwner(ticket) )
                return false;
            bool isCalendarHeader = false;
            if ( button )
            {
                const MUXC::ContentControl contentControl =
                    button.try_as<MUXC::ContentControl>();
                if ( !GetCurrentOwner(ticket) )
                    return false;
                if ( contentControl )
                {
                    const WF::IInspectable content =
                        contentControl.Content();
                    if ( !GetCurrentOwner(ticket) )
                        return false;
                    isCalendarHeader =
                        !headerText.empty() &&
                        winrt::unbox_value_or<winrt::hstring>(
                            content, {}) == headerText;
                    if ( !GetCurrentOwner(ticket) )
                        return false;
                }
            }
            if ( button &&
                 !AppendElementGeometry(
                     ticket,
                     button.as<MUX::FrameworkElement>(), true,
                     &snapshot->buttons, isCalendarHeader) )
            {
                return false;
            }
            if ( button && !GetCurrentOwner(ticket) )
                return false;

            const int count =
                MUXM::VisualTreeHelper::GetChildrenCount(current);
            if ( !GetCurrentOwner(ticket) )
                return false;
            if ( count < 0 ||
                 pending.size() +
                     static_cast<std::size_t>(count) >
                         MaxRealizedNodes )
            {
                return false;
            }

            for ( int n = 0; n != count; ++n )
            {
                const MUX::DependencyObject child =
                    MUXM::VisualTreeHelper::GetChild(current, n);
                if ( !GetCurrentOwner(ticket) )
                    return false;
                pending.push_back(child);
            }
        }

        return GetCurrentOwner(ticket) != nullptr &&
               !snapshot->days.empty();
    }

    struct MonthTopologyInput
    {
        wxWinUICalendarMonthKey month;
        int calendarDay = 0;
        int daysInMonth = 0;
        int dateKey = 0;
        double x = 0.0;
        double y = 0.0;
        double width = 0.0;
        double height = 0.0;
        std::size_t sourceIndex = 0;
        std::uintptr_t identity = 0;
        bool authoritative = false;
    };

    struct TruncatedMonthAuthority
    {
        wxWinUICalendarMonthKey month;
        int dayCount = 0;
        int firstDay = 1;
        int lastDay = 0;
        bool valid = false;
    };

    struct MonthTopologyResult
    {
        wxWinUICalendarMonthKey month;
        std::vector<std::size_t> sourceIndices;
        std::vector<std::size_t> canonicalSourceIndices;
        unsigned columnCount = 0;
        unsigned rowCount = 0;
        int dayCount = 0;
        int firstDay = 0;
        int lastDay = 0;
        int firstDaySlot = 0;
        bool rightToLeft = false;
        std::vector<double> columnCentres;
        std::vector<double> columnExtents;
        std::vector<double> rowCentres;
        std::vector<double> rowExtents;
    };

    static bool ResolveActiveMonthTopology(
        const std::vector<MonthTopologyInput>& inputs,
        const TruncatedMonthAuthority *truncated,
        MonthTopologyResult *result)
    {
        if ( !result )
            return false;
        *result = {};

        std::map<wxWinUICalendarMonthKey,
                 std::vector<const MonthTopologyInput *>> rawPages;
        for ( const MonthTopologyInput& input : inputs )
        {
            if ( !input.month.valid || input.calendarDay <= 0 ||
                 input.daysInMonth <= 0 || input.daysInMonth > 64 ||
                 input.dateKey == 0 || input.width <= 0.0 ||
                 input.height <= 0.0 || !std::isfinite(input.x) ||
                 !std::isfinite(input.y) ||
                 !std::isfinite(input.width) ||
                 !std::isfinite(input.height) )
            {
                continue;
            }
            rawPages[input.month].push_back(&input);
        }

        // Canonicalize every civil date before doing any hit testing. The
        // date-keyed realized-day ticket wins over a visual-tree duplicate;
        // two fallback identities are an ambiguous recycle/transition state.
        std::map<wxWinUICalendarMonthKey,
                 std::vector<const MonthTopologyInput *>> pages;
        for ( const auto& rawPage : rawPages )
        {
            std::map<int, std::vector<const MonthTopologyInput *>> byDay;
            for ( const MonthTopologyInput *input : rawPage.second )
            {
                if ( input->calendarDay > input->daysInMonth )
                    return false;
                byDay[input->calendarDay].push_back(input);
            }

            std::set<int> dates;
            std::vector<const MonthTopologyInput *>& page =
                pages[rawPage.first];
            page.reserve(byDay.size());
            for ( const auto& candidates : byDay )
            {
                const MonthTopologyInput *selected = nullptr;
                unsigned authoritativeCount = 0;
                for ( const MonthTopologyInput *candidate :
                      candidates.second )
                {
                    if ( candidate->authoritative )
                    {
                        selected = candidate;
                        ++authoritativeCount;
                    }
                }
                if ( authoritativeCount > 1 ||
                     (authoritativeCount == 0 &&
                      candidates.second.size() != 1) )
                {
                    return false;
                }
                if ( !selected )
                    selected = candidates.second.front();
                if ( !dates.insert(selected->dateKey).second )
                    return false;
                page.push_back(selected);
                result->canonicalSourceIndices.push_back(
                    selected->sourceIndex);
            }
        }

        struct AxisBand
        {
            double centre = 0.0;
            double extent = 0.0;
            unsigned samples = 0;
        };

        const auto resolvePage = [](
            const std::vector<const MonthTopologyInput *>& page,
            int firstDay, int lastDay, bool natural,
            MonthTopologyResult *resolved) -> bool
        {
            if ( !resolved || page.empty() || firstDay <= 0 ||
                 lastDay < firstDay )
            {
                return false;
            }
            *resolved = {};
            const int dayCount = page.front()->daysInMonth;
            if ( dayCount <= 0 || lastDay > dayCount ||
                 page.size() !=
                     static_cast<std::size_t>(lastDay - firstDay + 1) )
            {
                return false;
            }

            std::vector<const MonthTopologyInput *> ordered = page;
            std::sort(
                ordered.begin(), ordered.end(),
                [](const MonthTopologyInput *lhs,
                   const MonthTopologyInput *rhs)
                {
                    return lhs->calendarDay < rhs->calendarDay;
                });
            for ( std::size_t n = 0; n != ordered.size(); ++n )
            {
                if ( ordered[n]->daysInMonth != dayCount ||
                     ordered[n]->calendarDay !=
                         firstDay + static_cast<int>(n) )
                {
                    return false;
                }
            }

            const auto makeBands = [&ordered](
                bool horizontal, std::vector<AxisBand> *bands)
            {
                bands->clear();
                for ( const MonthTopologyInput *input : ordered )
                {
                    const double origin = horizontal ? input->x : input->y;
                    const double extent = horizontal
                        ? input->width : input->height;
                    const double centre = origin + extent / 2.0;
                    AxisBand *nearest = nullptr;
                    double nearestDistance = 0.0;
                    for ( AxisBand& band : *bands )
                    {
                        const double distance =
                            std::abs(band.centre - centre);
                        const double tolerance = std::max(
                            1.0, std::min(band.extent, extent) / 3.0);
                        if ( distance <= tolerance &&
                             (!nearest || distance < nearestDistance) )
                        {
                            nearest = &band;
                            nearestDistance = distance;
                        }
                    }
                    if ( nearest )
                    {
                        nearest->centre =
                            (nearest->centre * nearest->samples + centre) /
                            (nearest->samples + 1);
                        nearest->extent =
                            std::min(nearest->extent, extent);
                        ++nearest->samples;
                    }
                    else
                    {
                        bands->push_back(AxisBand{centre, extent, 1});
                    }
                }
                std::sort(
                    bands->begin(), bands->end(),
                    [](const AxisBand& lhs, const AxisBand& rhs)
                    {
                        return lhs.centre < rhs.centre;
                    });
            };

            std::vector<AxisBand> columns;
            std::vector<AxisBand> rows;
            makeBands(true, &columns);
            makeBands(false, &rows);
            if ( columns.size() != 7 || rows.empty() ||
                 rows.size() > 6 || (natural && rows.size() < 4) )
            {
                return false;
            }

            const auto findBand = [](double centre, double extent,
                                     const std::vector<AxisBand>& bands)
            {
                int found = -1;
                double nearestDistance = 0.0;
                for ( std::size_t n = 0; n != bands.size(); ++n )
                {
                    const double distance =
                        std::abs(bands[n].centre - centre);
                    const double tolerance = std::max(
                        1.0, std::min(bands[n].extent, extent) / 3.0);
                    if ( distance <= tolerance &&
                         (found < 0 || distance < nearestDistance) )
                    {
                        found = static_cast<int>(n);
                        nearestDistance = distance;
                    }
                }
                return found;
            };

            bool leftToRight = true;
            bool rightToLeft = true;
            int leftToRightDelta = 0;
            int rightToLeftDelta = 0;
            std::set<std::pair<int, int>> cells;
            for ( std::size_t n = 0; n != ordered.size(); ++n )
            {
                const MonthTopologyInput *input = ordered[n];
                const int column = findBand(
                    input->x + input->width / 2.0,
                    input->width, columns);
                const int row = findBand(
                    input->y + input->height / 2.0,
                    input->height, rows);
                if ( column < 0 || row < 0 ||
                     !cells.insert(std::make_pair(row, column)).second )
                {
                    return false;
                }
                const int dayIndex = input->calendarDay - firstDay;
                const int ltr = row * 7 + column - dayIndex;
                const int rtl = row * 7 + (6 - column) - dayIndex;
                if ( n == 0 )
                {
                    leftToRightDelta = ltr;
                    rightToLeftDelta = rtl;
                }
                else
                {
                    leftToRight = leftToRight &&
                                  ltr == leftToRightDelta;
                    rightToLeft = rightToLeft &&
                                  rtl == rightToLeftDelta;
                }
            }
            if ( leftToRight == rightToLeft )
                return false;

            resolved->month = ordered.front()->month;
            resolved->dayCount = dayCount;
            resolved->firstDay = firstDay;
            resolved->lastDay = lastDay;
            resolved->firstDaySlot = rightToLeft
                ? rightToLeftDelta : leftToRightDelta;
            resolved->rightToLeft = rightToLeft;
            resolved->columnCount =
                static_cast<unsigned>(columns.size());
            resolved->rowCount = static_cast<unsigned>(rows.size());
            resolved->columnCentres.reserve(columns.size());
            resolved->columnExtents.reserve(columns.size());
            for ( const AxisBand& column : columns )
            {
                resolved->columnCentres.push_back(column.centre);
                resolved->columnExtents.push_back(column.extent);
            }
            resolved->rowCentres.reserve(rows.size());
            resolved->rowExtents.reserve(rows.size());
            for ( const AxisBand& row : rows )
            {
                resolved->rowCentres.push_back(row.centre);
                resolved->rowExtents.push_back(row.extent);
            }
            resolved->sourceIndices.reserve(ordered.size());
            for ( const MonthTopologyInput *input : ordered )
                resolved->sourceIndices.push_back(input->sourceIndex);
            return true;
        };

        MonthTopologyResult unique;
        unsigned naturalPageCount = 0;
        for ( const auto& page : pages )
        {
            if ( page.second.empty() )
                continue;
            MonthTopologyResult candidate;
            const int dayCount = page.second.front()->daysInMonth;
            if ( resolvePage(
                     page.second, 1, dayCount, true, &candidate) )
            {
                unique = std::move(candidate);
                ++naturalPageCount;
            }
        }
        if ( !truncated || !truncated->valid )
        {
            if ( naturalPageCount != 1 )
                return false;
            unique.canonicalSourceIndices =
                std::move(result->canonicalSourceIndices);
            *result = std::move(unique);
            return true;
        }

        // A range-clipped page is authoritative only because the exact
        // selected civil month and wx bounds prove its missing dates cannot
        // be realized. Never let an old complete page win over that guard.
        if ( naturalPageCount != 0 || truncated->firstDay <= 0 ||
             truncated->lastDay < truncated->firstDay ||
             (truncated->firstDay == 1 &&
              truncated->lastDay == truncated->dayCount) )
        {
            return false;
        }
        const auto page = pages.find(truncated->month);
        MonthTopologyResult candidate;
        if ( page == pages.end() ||
             !resolvePage(page->second, truncated->firstDay,
                          truncated->lastDay, false, &candidate) )
        {
            return false;
        }
        candidate.canonicalSourceIndices =
            std::move(result->canonicalSourceIndices);
        *result = std::move(candidate);
        return true;
    }

    static bool GetTruncatedMonthAuthority(
        const HitTestTicket& ticket,
        TruncatedMonthAuthority *authority)
    {
        if ( !authority )
            return false;
        *authority = {};

        wxCalendarCtrl * const owner = GetCurrentOwner(ticket);
        if ( !owner || !owner->m_winui )
            return false;
        const wxDateTime ownerDate = owner->m_date;
        const wxDateTime lowerDate = owner->m_lowerDate;
        const wxDateTime upperDate = owner->m_upperDate;
        const winrt::hstring identifier =
            owner->m_winui->calendarIdentifier;
        wxDateTime selectedDate;
        try
        {
            if ( !wxWinUIReadSelectedDate(ticket.calendar, &selectedDate) )
                return false;
        }
        catch ( const winrt::hresult_error& )
        {
            return false;
        }
        if ( !GetCurrentOwner(ticket) ||
             !wxWinUISameCivilDate(ownerDate, selectedDate) )
        {
            return false;
        }

        const auto readDate = [&identifier](
            const wxDateTime& date, wxWinUICalendarMonthKey *month,
            int *day, int *dayCount = nullptr)
        {
            WF::DateTime peer{};
            return date.IsValid() && wxWinUIToDateTime(date, &peer) &&
                   wxWinUIReadCalendarDay(
                       identifier, peer, month, day, dayCount);
        };

        wxWinUICalendarMonthKey month;
        int dayCount = 0;
        if ( !readDate(selectedDate, &month, nullptr, &dayCount) ||
             !GetCurrentOwner(ticket) )
        {
            return false;
        }

        int firstDay = 1;
        int lastDay = dayCount;
        wxWinUICalendarMonthKey boundMonth;
        int boundDay = 0;
        if ( lowerDate.IsValid() &&
             readDate(lowerDate, &boundMonth, &boundDay) &&
             boundMonth == month )
        {
            firstDay = std::max(firstDay, boundDay);
        }
        if ( !GetCurrentOwner(ticket) )
            return false;
        boundMonth = {};
        boundDay = 0;
        if ( upperDate.IsValid() &&
             readDate(upperDate, &boundMonth, &boundDay) &&
             boundMonth == month )
        {
            lastDay = std::min(lastDay, boundDay);
        }
        if ( !GetCurrentOwner(ticket) || firstDay > lastDay )
            return false;

        authority->month = month;
        authority->dayCount = dayCount;
        authority->firstDay = firstDay;
        authority->lastDay = lastDay;
        authority->valid = month.valid && dayCount > 0 &&
                           (firstDay != 1 || lastDay != dayCount);
        return authority->valid;
    }

    struct ActiveMonthPage
    {
        wxWinUICalendarMonthKey month;
        std::vector<const DayHitGeometry *> currentDays;
        std::vector<const DayHitGeometry *> canonicalDays;
        unsigned columnCount = 0;
    };

    static bool ResolveActiveMonthPage(
        const HitTestTicket& ticket,
        const PublicHitSnapshot& snapshot,
        ActiveMonthPage *page)
    {
        if ( !page )
            return false;
        *page = {};

        std::vector<MonthTopologyInput> inputs;
        inputs.reserve(snapshot.days.size());
        for ( std::size_t n = 0; n != snapshot.days.size(); ++n )
        {
            const DayHitGeometry& day = snapshot.days[n];
            inputs.push_back(
                MonthTopologyInput{
                    day.month,
                    day.calendarDay,
                    day.daysInMonth,
                    wxWinUICivilDateKey(day.date),
                    day.bounds.X,
                    day.bounds.Y,
                    day.bounds.Width,
                    day.bounds.Height,
                    n,
                    reinterpret_cast<std::uintptr_t>(
                        winrt::get_abi(day.item)),
                    day.authoritative});
        }

        TruncatedMonthAuthority truncated;
        const TruncatedMonthAuthority *truncatedPtr =
            GetTruncatedMonthAuthority(ticket, &truncated)
                ? &truncated : nullptr;
        if ( !GetCurrentOwner(ticket) )
            return false;

        MonthTopologyResult topology;
        if ( !ResolveActiveMonthTopology(
                 inputs, truncatedPtr, &topology) )
        {
            return false;
        }

        page->month = topology.month;
        page->columnCount = topology.columnCount;
        page->currentDays.reserve(topology.sourceIndices.size());
        for ( const std::size_t index : topology.sourceIndices )
        {
            if ( index >= snapshot.days.size() )
                return false;
            page->currentDays.push_back(&snapshot.days[index]);
        }

        if ( topology.columnCentres.size() != 7 ||
             topology.columnExtents.size() != 7 ||
             topology.rowCentres.empty() ||
             topology.rowCentres.size() !=
                 topology.rowExtents.size() )
        {
            return false;
        }

        const DayHitGeometry * const firstCurrent =
            page->currentDays.front();
        const DayHitGeometry * const lastCurrent =
            page->currentDays.back();
        page->canonicalDays.reserve(
            topology.canonicalSourceIndices.size());
        for ( const std::size_t index :
              topology.canonicalSourceIndices )
        {
            if ( index >= snapshot.days.size() )
                return false;
            const DayHitGeometry * const candidate =
                &snapshot.days[index];
            if ( candidate->month == page->month )
            {
                page->canonicalDays.push_back(candidate);
                continue;
            }

            int expectedSlot = -1;
            constexpr int MaxCalendarSlots = 6 * 7;
            for ( int distance = 1;
                  distance != MaxCalendarSlots; ++distance )
            {
                if ( wxWinUISameCivilDate(
                         candidate->date,
                         firstCurrent->date -
                             wxDateSpan::Days(distance)) )
                {
                    if ( topology.firstDay == 1 )
                    {
                        expectedSlot =
                            topology.firstDaySlot - distance;
                    }
                    break;
                }
                if ( wxWinUISameCivilDate(
                         candidate->date,
                         lastCurrent->date +
                             wxDateSpan::Days(distance)) )
                {
                    if ( topology.lastDay == topology.dayCount )
                    {
                        expectedSlot = topology.firstDaySlot +
                            (topology.lastDay - topology.firstDay) +
                            distance;
                    }
                    break;
                }
            }
            if ( expectedSlot < 0 ||
                 expectedSlot >= MaxCalendarSlots )
            {
                continue;
            }

            const int logicalColumn = expectedSlot % 7;
            const int physicalColumn = topology.rightToLeft
                ? 6 - logicalColumn : logicalColumn;
            const int row = expectedSlot / 7;
            const double expectedX =
                topology.columnCentres[physicalColumn];
            const double candidateX = candidate->bounds.X +
                candidate->bounds.Width / 2.0;
            const double columnTolerance = std::max(
                1.0,
                std::min(
                    static_cast<double>(candidate->bounds.Width),
                    topology.columnExtents[physicalColumn]) /
                    3.0);
            if ( std::abs(candidateX - expectedX) > columnTolerance )
                continue;

            double rowPitch = topology.rowExtents.front();
            if ( topology.rowCentres.size() > 1 )
            {
                rowPitch =
                    (topology.rowCentres.back() -
                     topology.rowCentres.front()) /
                    (topology.rowCentres.size() - 1);
            }
            if ( !std::isfinite(rowPitch) || rowPitch <= 0.0 )
                return false;
            const double expectedY = topology.rowCentres.front() +
                row * rowPitch;
            const double candidateY = candidate->bounds.Y +
                candidate->bounds.Height / 2.0;
            const double referenceHeight =
                row < static_cast<int>(topology.rowExtents.size())
                    ? topology.rowExtents[row]
                    : topology.rowExtents.back();
            const double rowTolerance = std::max(
                1.0,
                std::min(
                    static_cast<double>(candidate->bounds.Height),
                    referenceHeight) /
                    3.0);
            if ( std::abs(candidateY - expectedY) <= rowTolerance )
            {
                page->canonicalDays.push_back(candidate);
            }
        }
        return page->month.valid && !page->currentDays.empty();
    }

    static bool ResolveHeaderAtPoint(
        const HitTestTicket& ticket,
        const PublicHitSnapshot& snapshot,
        const WF::Point& point,
        wxDateTime::WeekDay *weekday)
    {
        if ( snapshot.days.empty() || !GetCurrentOwner(ticket) )
        {
            return false;
        }

        ActiveMonthPage activePage;
        if ( !ResolveActiveMonthPage(
                 ticket, snapshot, &activePage) )
            return false;

        const std::vector<const DayHitGeometry *>& displayedMonthDays =
            activePage.currentDays;

        // Calendar day one in the selected, complete and unambiguous page
        // anchors its first active row. Recycled or transition-page peers
        // never participate in this fallback band.
        float firstDayTop = displayedMonthDays.front()->bounds.Y;
        float tallestDay = displayedMonthDays.front()->bounds.Height;
        for ( const DayHitGeometry *day : displayedMonthDays )
        {
            tallestDay = std::max(tallestDay, day->bounds.Height);
        }

        for ( const ElementHitGeometry& label :
              snapshot.weekdayLabels )
        {
            const WF::Rect& labelBounds = label.bounds;
            if ( labelBounds.Y + labelBounds.Height >
                     firstDayTop + 1.0f ||
                 firstDayTop -
                         (labelBounds.Y + labelBounds.Height) >
                     tallestDay ||
                 point.Y < labelBounds.Y ||
                 point.Y >= labelBounds.Y + labelBounds.Height )
            {
                continue;
            }

            const double labelCentreX =
                static_cast<double>(labelBounds.X) +
                static_cast<double>(labelBounds.Width) / 2.0;
            const DayHitGeometry *columnDay = nullptr;
            double nearestDistance = 0.0;
            for ( const DayHitGeometry *day : displayedMonthDays )
            {
                const double dayCentreX =
                    static_cast<double>(day->bounds.X) +
                    static_cast<double>(day->bounds.Width) / 2.0;
                const double distance =
                    std::abs(dayCentreX - labelCentreX);
                if ( !columnDay || distance < nearestDistance )
                {
                    columnDay = day;
                    nearestDistance = distance;
                }
            }

            if ( !columnDay )
                continue;

            const double halfColumnWidth =
                static_cast<double>(columnDay->bounds.Width) / 2.0;
            if ( nearestDistance > halfColumnWidth + 1.0 ||
                 point.X < columnDay->bounds.X ||
                 point.X >= columnDay->bounds.X +
                                columnDay->bounds.Width )
            {
                continue;
            }

            if ( !GetCurrentOwner(ticket) )
                return false;
            if ( weekday )
                *weekday = columnDay->date.GetWeekDay();
            return true;
        }

        // Custom SDK/theme templates don't have to expose weekday captions as
        // TextBlocks (nor preserve their localized text verbatim). Fall back
        // to the actual realized day columns and the one-cell header band.
        // Dates in the same physical column always share a weekday, including
        // in RTL layouts.
        if ( point.Y >= firstDayTop - tallestDay &&
             point.Y < firstDayTop )
        {
            for ( const DayHitGeometry *day : displayedMonthDays )
            {
                if ( point.X >= day->bounds.X &&
                     point.X < day->bounds.X + day->bounds.Width )
                {
                    if ( !GetCurrentOwner(ticket) )
                        return false;
                    if ( weekday )
                        *weekday = day->date.GetWeekDay();
                    return true;
                }
            }
        }

        return false;
    }

    static bool ResolveDateProgression(
        const PublicHitSnapshot& snapshot,
        bool *datesIncreaseToRight)
    {
        if ( !datesIncreaseToRight )
            return false;

        for ( const DayHitGeometry& day : snapshot.days )
        {
            const int nextKey = wxWinUICivilDateKey(
                day.date + wxDateSpan::Day());
            for ( const DayHitGeometry& candidate : snapshot.days )
            {
                if ( wxWinUICivilDateKey(candidate.date) != nextKey )
                    continue;

                const double dayCentreY =
                    static_cast<double>(day.bounds.Y) +
                    static_cast<double>(day.bounds.Height) / 2.0;
                const double candidateCentreY =
                    static_cast<double>(candidate.bounds.Y) +
                    static_cast<double>(candidate.bounds.Height) / 2.0;
                const double rowTolerance =
                    std::max(1.0,
                             std::min(
                                 static_cast<double>(
                                     day.bounds.Height),
                                 static_cast<double>(
                                     candidate.bounds.Height)) /
                                 4.0);
                if ( std::abs(dayCentreY - candidateCentreY) >
                     rowTolerance )
                {
                    continue;
                }

                const double dayCentreX =
                    static_cast<double>(day.bounds.X) +
                    static_cast<double>(day.bounds.Width) / 2.0;
                const double candidateCentreX =
                    static_cast<double>(candidate.bounds.X) +
                    static_cast<double>(
                        candidate.bounds.Width) / 2.0;
                if ( std::abs(dayCentreX - candidateCentreX) <= 1.0 )
                    continue;

                *datesIncreaseToRight =
                    candidateCentreX > dayCentreX;
                return true;
            }
        }

        return false;
    }

    static bool ResolveNavigationCandidatePair(
        const PublicHitSnapshot& snapshot,
        const ElementHitGeometry **left,
        const ElementHitGeometry **right,
        unsigned *candidateCount = nullptr)
    {
        if ( left )
            *left = nullptr;
        if ( right )
            *right = nullptr;
        if ( candidateCount )
            *candidateCount = 0;
        if ( !left || !right || snapshot.days.empty() )
            return false;

        // CalendarView keeps day containers from neighbouring pages realized
        // while it animates/recycles its month panel. Their transformed
        // bounds can overlap the navigation row, so the minimum day Y is not
        // a stable boundary for that row. Anchor it to the one semantic
        // HeaderButton instead: the stock HeaderButton, PreviousButton and
        // NextButton are sibling Buttons in the same template Grid row.
        const ElementHitGeometry *calendarHeader = nullptr;
        for ( const ElementHitGeometry& button : snapshot.buttons )
        {
            if ( !button.isCalendarHeader )
                continue;

            // More than one semantic header is an ambiguous/custom template.
            if ( calendarHeader )
                return false;
            calendarHeader = &button;
        }

        const auto isInHeaderRow =
            [calendarHeader](const ElementHitGeometry& button)
            {
                if ( !calendarHeader )
                    return true;

                const double headerCentreY =
                    static_cast<double>(calendarHeader->bounds.Y) +
                    static_cast<double>(calendarHeader->bounds.Height) / 2.0;
                const double buttonCentreY =
                    static_cast<double>(button.bounds.Y) +
                    static_cast<double>(button.bounds.Height) / 2.0;
                const double rowTolerance =
                    std::max(
                        1.0,
                        std::min(
                            static_cast<double>(
                                calendarHeader->bounds.Height),
                            static_cast<double>(button.bounds.Height)) /
                            4.0);
                return std::abs(headerCentreY - buttonCentreY) <=
                       rowTolerance;
            };

        std::vector<const ElementHitGeometry *> candidates;
        for ( const ElementHitGeometry& button : snapshot.buttons )
        {
            if ( !button.isCalendarHeader && isInHeaderRow(button) )
                candidates.push_back(&button);
        }
        if ( candidateCount )
        {
            *candidateCount =
                static_cast<unsigned>(candidates.size());
        }
        if ( candidates.size() < 2 )
            return false;

        const auto area =
            [](const ElementHitGeometry *element)
            {
                return static_cast<double>(element->bounds.Width) *
                       static_cast<double>(element->bounds.Height);
            };
        std::sort(
            candidates.begin(), candidates.end(),
            [&area](const ElementHitGeometry *lhs,
                    const ElementHitGeometry *rhs)
            {
                const double lhsArea = area(lhs);
                const double rhsArea = area(rhs);
                if ( lhsArea != rhsArea )
                    return lhsArea < rhsArea;
                return lhs->bounds.X < rhs->bounds.X;
            });

        const ElementHitGeometry * const first = candidates[0];
        const ElementHitGeometry * const second = candidates[1];
        const auto dimensionsMatch =
            [](const ElementHitGeometry *lhs,
               const ElementHitGeometry *rhs)
            {
                const double maxWidth =
                    std::max(lhs->bounds.Width, rhs->bounds.Width);
                const double minWidth =
                    std::min(lhs->bounds.Width, rhs->bounds.Width);
                const double maxHeight =
                    std::max(lhs->bounds.Height, rhs->bounds.Height);
                const double minHeight =
                    std::min(lhs->bounds.Height, rhs->bounds.Height);
                return minWidth > 0.0 && minHeight > 0.0 &&
                       maxWidth / minWidth <= 1.25 &&
                       maxHeight / minHeight <= 1.25;
            };
        if ( !dimensionsMatch(first, second) )
            return false;

        // HeaderText authenticates the semantic view-selection button when
        // the template exposes it. If a custom template exposes three
        // indistinguishable compact buttons, fail closed instead of binding
        // an arbitrary command.
        if ( candidates.size() > 2 &&
             dimensionsMatch(first, candidates[2]) &&
             area(candidates[2]) <= area(second) * 1.35 )
        {
            return false;
        }

        const double firstCentreX =
            static_cast<double>(first->bounds.X) +
            static_cast<double>(first->bounds.Width) / 2.0;
        const double secondCentreX =
            static_cast<double>(second->bounds.X) +
            static_cast<double>(second->bounds.Width) / 2.0;
        if ( std::abs(firstCentreX - secondCentreX) <= 1.0 )
            return false;

        if ( firstCentreX < secondCentreX )
        {
            *left = first;
            *right = second;
        }
        else
        {
            *left = second;
            *right = first;
        }
        return true;
    }

    static wxCalendarHitTestResult ResolveNavigationAtPoint(
        const HitTestTicket& ticket,
        const PublicHitSnapshot& snapshot,
        const WF::Point& point,
        bool allowGeometricFallback = true)
    {
        if ( snapshot.days.empty() || !GetCurrentOwner(ticket) )
        {
            return wxCAL_HITTEST_NOWHERE;
        }

        const auto resolveFromDayGrid = [&]()
        {
            wxCalendarCtrl * const owner = GetCurrentOwner(ticket);
            if ( !owner || !owner->AllowMonthChange() )
                return wxCAL_HITTEST_NOWHERE;

            float firstDayTop = snapshot.days.front().bounds.Y;
            float left = snapshot.days.front().bounds.X;
            float right = left + snapshot.days.front().bounds.Width;
            float widest = snapshot.days.front().bounds.Width;
            float tallest = snapshot.days.front().bounds.Height;
            for ( const DayHitGeometry& day : snapshot.days )
            {
                firstDayTop = std::min(firstDayTop, day.bounds.Y);
                left = std::min(left, day.bounds.X);
                right = std::max(
                    right, day.bounds.X + day.bounds.Width);
                widest = std::max(widest, day.bounds.Width);
                tallest = std::max(tallest, day.bounds.Height);
            }

            const float headerTop = firstDayTop - tallest;
            if ( point.Y < 0.0f || point.Y >= headerTop )
                return wxCAL_HITTEST_NOWHERE;

            bool datesIncreaseToRight = true;
            if ( !ResolveDateProgression(
                     snapshot, &datesIncreaseToRight) )
            {
                return wxCAL_HITTEST_NOWHERE;
            }

            const float edgeWidth = widest * 1.75f;
            const bool onLeft =
                point.X >= left && point.X < left + edgeWidth;
            const bool onRight =
                point.X < right && point.X >= right - edgeWidth;
            if ( onLeft == onRight )
                return wxCAL_HITTEST_NOWHERE;

            const bool isDecrement =
                datesIncreaseToRight ? onLeft : onRight;
            return isDecrement
                       ? wxCAL_HITTEST_DECMONTH
                       : wxCAL_HITTEST_INCMONTH;
        };

        const ElementHitGeometry *left = nullptr;
        const ElementHitGeometry *right = nullptr;
        if ( !ResolveNavigationCandidatePair(
                 snapshot, &left, &right) )
        {
            return allowGeometricFallback
                       ? resolveFromDayGrid()
                       : wxCAL_HITTEST_NOWHERE;
        }

        const ElementHitGeometry *hit = nullptr;
        if ( ContainsPoint(left->bounds, point) )
            hit = left;
        else if ( ContainsPoint(right->bounds, point) )
            hit = right;
        if ( !hit )
            return wxCAL_HITTEST_NOWHERE;

        bool datesIncreaseToRight = true;
        if ( !ResolveDateProgression(
                 snapshot, &datesIncreaseToRight) )
        {
            return allowGeometricFallback
                       ? resolveFromDayGrid()
                       : wxCAL_HITTEST_NOWHERE;
        }

        const bool hitIsLeft = hit == left;

        if ( !GetCurrentOwner(ticket) )
            return wxCAL_HITTEST_NOWHERE;
        const bool isDecrement =
            datesIncreaseToRight ? hitIsLeft : !hitIsLeft;
        return isDecrement
                   ? wxCAL_HITTEST_DECMONTH
                   : wxCAL_HITTEST_INCMONTH;
    }

    static wxCalendarHitTestResult ResolvePublicHitAtPoint(
        const HitTestTicket& ticket,
        const WF::Point& point,
        wxDateTime *date,
        wxDateTime::WeekDay *weekday)
    {
        PublicHitSnapshot snapshot;
        if ( !CapturePublicHitSnapshot(ticket, &snapshot) )
            return wxCAL_HITTEST_NOWHERE;

        ActiveMonthPage activePage;
        if ( !ResolveActiveMonthPage(ticket, snapshot, &activePage) )
            return wxCAL_HITTEST_NOWHERE;

        PublicHitSnapshot activeSnapshot = snapshot;
        activeSnapshot.days.clear();
        activeSnapshot.days.reserve(activePage.canonicalDays.size());
        for ( const DayHitGeometry *day : activePage.canonicalDays )
            activeSnapshot.days.push_back(*day);

        for ( const DayHitGeometry& day : activeSnapshot.days )
        {
            if ( !ContainsPoint(day.bounds, point) )
                continue;

            if ( !GetCurrentOwner(ticket) )
                return wxCAL_HITTEST_NOWHERE;

            if ( date )
                *date = day.date;

            return !(day.month == activePage.month)
                       ? wxCAL_HITTEST_SURROUNDING_WEEK
                       : wxCAL_HITTEST_DAY;
        }

        // Realized navigation peers are the strongest public hit-test
        // authority. In templates without TextBlock weekday captions the
        // geometric header fallback can overlap the lower edge of an arrow;
        // never let that extrapolated band consume a real button centre.
        const wxCalendarHitTestResult navigation =
            ResolveNavigationAtPoint(
                ticket, activeSnapshot, point, false);
        if ( navigation != wxCAL_HITTEST_NOWHERE )
            return navigation;

        wxDateTime::WeekDay resolvedWeekday =
            wxDateTime::Inv_WeekDay;
        if ( ResolveHeaderAtPoint(
                 ticket, activeSnapshot, point, &resolvedWeekday) )
        {
            if ( !GetCurrentOwner(ticket) )
                return wxCAL_HITTEST_NOWHERE;
            if ( weekday )
                *weekday = resolvedWeekday;
            return wxCAL_HITTEST_HEADER;
        }
        if ( !GetCurrentOwner(ticket) )
            return wxCAL_HITTEST_NOWHERE;

        for ( const PublicHitSnapshot::WeekHitGeometry& week :
              activeSnapshot.weeks )
        {
            if ( !ContainsPoint(week.element.bounds, point) )
                continue;
            if ( !GetCurrentOwner(ticket) )
                return wxCAL_HITTEST_NOWHERE;
            if ( date )
                *date = week.startDate;
            if ( weekday )
                *weekday = week.startDate.GetWeekDay();
            return wxCAL_HITTEST_WEEK;
        }

        return ResolveNavigationAtPoint(
            ticket, activeSnapshot, point);
    }

    static bool ResolveNavigationButtons(
        const HitTestTicket& ticket,
        const PublicHitSnapshot& snapshot,
        MUXCP::ButtonBase *decrement,
        MUXCP::ButtonBase *increment,
        unsigned *candidateCount = nullptr)
    {
        if ( !decrement || !increment )
            return false;
        *decrement = nullptr;
        *increment = nullptr;

        const ElementHitGeometry *left = nullptr;
        const ElementHitGeometry *right = nullptr;
        if ( !ResolveNavigationCandidatePair(
                 snapshot, &left, &right, candidateCount) )
        {
            return false;
        }

        const MUXCP::ButtonBase leftButton =
            left->element.try_as<MUXCP::ButtonBase>();
        if ( !GetCurrentOwner(ticket) )
            return false;
        const MUXCP::ButtonBase rightButton =
            right->element.try_as<MUXCP::ButtonBase>();
        if ( !GetCurrentOwner(ticket) || !leftButton || !rightButton )
            return false;

        bool datesIncreaseToRight = true;
        if ( !ResolveDateProgression(
                 snapshot, &datesIncreaseToRight) )
        {
            return false;
        }
        if ( !GetCurrentOwner(ticket) )
            return false;

        *decrement = datesIncreaseToRight ? leftButton : rightButton;
        *increment = datesIncreaseToRight ? rightButton : leftButton;
        return *decrement && *increment &&
               winrt::get_abi(*decrement) !=
                   winrt::get_abi(*increment);
    }

    void RevokeNavigationButtons()
    {
        const bool hadBinding = decrementButton || incrementButton ||
                                decrementClickToken.value ||
                                incrementClickToken.value;
        const auto revoke =
            [](MUXCP::ButtonBase& button,
               winrt::event_token& token,
               const char *operation)
            {
                if ( button && token.value )
                {
                    try
                    {
                        button.Click(token);
                    }
                    catch ( const winrt::hresult_error& e )
                    {
                        wxWinUILogException(operation, e);
                    }
                }
                token = {};
                button = nullptr;
            };
        revoke(decrementButton, decrementClickToken,
               "WinUI CalendarView decrement Click removal");
        revoke(incrementButton, incrementClickToken,
               "WinUI CalendarView increment Click removal");
        if ( hadBinding )
        {
            if ( ++navigationButtonGeneration == 0 )
                ++navigationButtonGeneration;
        }
    }

    void SelectRelativeMonth(wxCalendarCtrl *owner, int delta)
    {
        const auto state = callbackState;
        const std::uint64_t generation =
            state ? state->Generation() : 0;
        const MUXC::CalendarView calendar = cal;
        const winrt::hstring identifier = calendarIdentifier;
        if ( !state || !owner || delta == 0 ||
             state->GetOwner(generation) != owner ||
             !owner->m_winui || owner->m_winui.get() != this ||
             owner->m_winui->callbackState != state ||
             owner->m_winui->cal != calendar ||
             state->IsPeerMutationInProgress() ||
             !owner->AllowMonthChange() ||
             calendar.DisplayMode() !=
                 MUXC::CalendarViewDisplayMode::Month )
        {
            return;
        }

        owner = state->GetOwner(generation);
        if ( !owner || !owner->m_winui ||
             owner->m_winui.get() != this ||
             owner->m_winui->callbackState != state ||
             owner->m_winui->cal != calendar )
        {
            return;
        }

        try
        {
            const auto selected = calendar.SelectedDates();
            if ( selected.Size() != 1 )
                return;
            const WF::DateTime selectedDate = selected.GetAt(0);
            owner = state->GetOwner(generation);
            if ( !owner || !owner->m_winui ||
                 owner->m_winui.get() != this ||
                 owner->m_winui->callbackState != state ||
                 owner->m_winui->cal != calendar )
            {
                return;
            }

            WG::Calendar model;
            model.ChangeCalendarSystem(identifier);
            model.SetDateTime(selectedDate);
            model.AddMonths(delta);
            WF::DateTime target = model.GetDateTime();

            const WF::DateTime minimum = calendar.MinDate();
            const WF::DateTime maximum = calendar.MaxDate();
            owner = state->GetOwner(generation);
            if ( !owner || !owner->m_winui ||
                 owner->m_winui.get() != this ||
                 owner->m_winui->callbackState != state ||
                 owner->m_winui->cal != calendar )
            {
                return;
            }
            if ( target.time_since_epoch() <
                 minimum.time_since_epoch() )
            {
                target = minimum;
            }
            else if ( target.time_since_epoch() >
                      maximum.time_since_epoch() )
            {
                target = maximum;
            }

            if ( target.time_since_epoch() ==
                 selectedDate.time_since_epoch() )
            {
                return;
            }

            calendar.SetDisplayDate(target);
            owner = state->GetOwner(generation);
            if ( !owner || !owner->m_winui ||
                 owner->m_winui.get() != this ||
                 owner->m_winui->callbackState != state ||
                 owner->m_winui->cal != calendar )
            {
                return;
            }

            // This is deliberately not wrapped in PeerMutationGuard: it is a
            // real user navigation and SelectedDatesChanged must publish the
            // standard wx selection/page/month-or-year event sequence.
            selected.SetAt(0, target);
            // The selection callback may destroy the wx owner and this
            // implementation. Do not inspect either after SetAt().
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException(
                "WinUI CalendarView month navigation", e);
        }
    }

    void QueueRelativeMonth(wxCalendarCtrl *owner, int delta)
    {
        const auto state = callbackState;
        const std::uint64_t generation =
            state ? state->Generation() : 0;
        const MUXC::CalendarView calendar = cal;
        if ( !state || !owner || !owner->m_winui ||
             owner->m_winui.get() != this ||
             owner->m_winui->callbackState != state ||
             owner->m_winui->cal != calendar )
        {
            return;
        }

        try
        {
            const bool queued = calendar.DispatcherQueue().TryEnqueue(
                [state, generation, delta]()
                {
                    wxCalendarCtrl * const liveOwner =
                        state->GetOwner(generation);
                    if ( !liveOwner || !liveOwner->m_winui ||
                         liveOwner->m_winui->callbackState != state )
                    {
                        return;
                    }
                    liveOwner->m_winui->SelectRelativeMonth(
                        liveOwner, delta);
                });
            if ( !queued )
            {
                // A valid UI-thread CalendarView normally always has a live
                // dispatcher queue. Fail closed if shutdown has begun.
                return;
            }
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException(
                "WinUI CalendarView month navigation queue", e);
        }
    }

    void QueueNavigationRefresh(wxCalendarCtrl *owner,
                                unsigned attemptsRemaining = 3)
    {
        const auto state = callbackState;
        const std::uint64_t generation =
            state ? state->Generation() : 0;
        const MUXC::CalendarView calendar = cal;
        const MUX::FrameworkElement refreshRoot = layoutRoot;
        if ( attemptsRemaining == 0 || navigationRefreshScheduled ||
             !state || !owner ||
             state->GetOwner(generation) != owner ||
             !owner->m_winui || owner->m_winui.get() != this ||
             owner->m_winui->callbackState != state ||
             owner->m_winui->cal != calendar || !refreshRoot )
        {
            return;
        }

        if ( ++navigationRefreshScheduleGeneration == 0 )
            ++navigationRefreshScheduleGeneration;
        const std::uint64_t scheduleGeneration =
            navigationRefreshScheduleGeneration;
        navigationRefreshScheduled = true;

        bool queued = false;
        try
        {
            queued = calendar.DispatcherQueue().TryEnqueue(
                MUXD::DispatcherQueuePriority::Low,
                [state, generation, this, calendar, refreshRoot,
                 scheduleGeneration, attemptsRemaining]()
                {
                    wxCalendarCtrl *owner = state->GetOwner(generation);
                    if ( !owner || !owner->m_winui ||
                         owner->m_winui.get() != this ||
                         owner->m_winui->callbackState != state ||
                         owner->m_winui->cal != calendar ||
                         navigationRefreshScheduleGeneration !=
                             scheduleGeneration ||
                         !navigationRefreshScheduled )
                    {
                        return;
                    }

                    // A low-priority callback runs after the current
                    // CalendarView template/layout wave. Clear the flag
                    // before retrying so a still-transient or newly reparented
                    // peer can schedule one fresh ticket instead of getting
                    // permanently stuck on this obsolete attempt.
                    navigationRefreshScheduled = false;
                    if ( owner->m_winui->layoutRoot != refreshRoot )
                    {
                        owner->m_winui->RefreshNavigationButtons(owner);
                        return;
                    }
                    const bool refreshed =
                        owner->m_winui->RefreshNavigationButtons(
                            owner, false);
                    owner = state->GetOwner(generation);
                    if ( !refreshed && attemptsRemaining > 1 &&
                         owner && owner->m_winui &&
                         owner->m_winui.get() == this &&
                         owner->m_winui->callbackState == state &&
                         owner->m_winui->cal == calendar )
                    {
                        owner->m_winui->QueueNavigationRefresh(
                            owner, attemptsRemaining - 1);
                    }
                    else if ( !refreshed && attemptsRemaining == 1 &&
                              owner && owner->m_winui &&
                              owner->m_winui.get() == this &&
                              owner->m_winui->callbackState == state &&
                              owner->m_winui->cal == calendar )
                    {
                        owner->m_winui->navigationBindingRequired = true;
                        ++owner->m_winui->navigationRetryExhaustionCount;
                    }
                });
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException(
                "WinUI CalendarView navigation reconciliation queue", e);
        }

        owner = state->GetOwner(generation);
        if ( !queued && owner && owner->m_winui &&
             owner->m_winui.get() == this &&
             owner->m_winui->callbackState == state &&
             navigationRefreshScheduleGeneration == scheduleGeneration )
        {
            navigationRefreshScheduled = false;
        }
    }

    bool RefreshNavigationButtons(wxCalendarCtrl *owner,
                                  bool allowDeferredRetry = true)
    {
        const auto state = callbackState;
        const std::uint64_t generation =
            state ? state->Generation() : 0;
        const MUXC::CalendarView calendar = cal;
        if ( !state || !owner ||
             state->GetOwner(generation) != owner ||
             !owner->m_winui || owner->m_winui.get() != this ||
             owner->m_winui->callbackState != state ||
             owner->m_winui->cal != calendar || !calendar )
        {
            return false;
        }

        const HitTestTicket ticket = CaptureHitTestTicket();
        PublicHitSnapshot snapshot;
        MUXCP::ButtonBase decrement{ nullptr };
        MUXCP::ButtonBase increment{ nullptr };
        unsigned candidateCount = 0;
        const bool snapshotCaptured =
            CapturePublicHitSnapshot(ticket, &snapshot);
        const bool pairResolved =
            snapshotCaptured &&
            ResolveNavigationButtons(
                ticket, snapshot, &decrement, &increment,
                &candidateCount);
        const auto recordFailure =
            [&](NavigationRefreshFailure failure)
            {
                wxCalendarCtrl * const liveOwner =
                    GetCurrentPeerOwner(ticket);
                if ( !liveOwner || !liveOwner->m_winui ||
                     liveOwner->m_winui.get() != this )
                {
                    return;
                }

                wxWinUICalendarImpl * const liveImpl =
                    liveOwner->m_winui.get();
                liveImpl->navigationSnapshotButtonCount =
                    static_cast<unsigned>(snapshot.buttons.size());
                liveImpl->navigationCandidateCount = candidateCount;
                liveImpl->navigationLastFailure = failure;
                liveImpl->navigationBindingRequired = true;
                ++liveImpl->navigationResolveFailureCount;
                if ( allowDeferredRetry )
                    liveImpl->QueueNavigationRefresh(liveOwner);
            };
        if ( !pairResolved )
        {
            recordFailure(
                !GetCurrentOwner(ticket)
                    ? NavigationRefreshFailure::Superseded
                    : snapshotCaptured
                        ? NavigationRefreshFailure::CandidatePairUnavailable
                        : NavigationRefreshFailure::SnapshotUnavailable);
            return false;
        }

        const bool peersAttached =
            IsElementAttachedToCalendar(
                ticket, decrement.as<MUX::FrameworkElement>()) &&
            IsElementAttachedToCalendar(
                ticket, increment.as<MUX::FrameworkElement>());
        if ( !peersAttached )
        {
            recordFailure(
                GetCurrentOwner(ticket)
                    ? NavigationRefreshFailure::DetachedPeer
                    : NavigationRefreshFailure::Superseded);
            return false;
        }

        navigationSnapshotButtonCount =
            static_cast<unsigned>(snapshot.buttons.size());
        navigationCandidateCount = candidateCount;

        if ( decrementButton && incrementButton &&
             winrt::get_abi(decrementButton) ==
                 winrt::get_abi(decrement) &&
             winrt::get_abi(incrementButton) ==
                 winrt::get_abi(increment) )
        {
            navigationBindingRequired = false;
            navigationLastFailure = NavigationRefreshFailure::None;
            return true;
        }

        RevokeNavigationButtons();
        owner = state->GetOwner(generation);
        if ( !owner || !owner->m_winui ||
             owner->m_winui.get() != this ||
             owner->m_winui->callbackState != state ||
             owner->m_winui->cal != calendar )
        {
            return false;
        }

        winrt::event_token decrementToken{};
        winrt::event_token incrementToken{};
        const auto revokeLocal =
            [](const MUXCP::ButtonBase& button,
               winrt::event_token& token)
            {
                if ( button && token.value )
                {
                    try
                    {
                        button.Click(token);
                    }
                    catch ( const winrt::hresult_error& e )
                    {
                        wxWinUILogException(
                            "WinUI CalendarView temporary navigation "
                            "handler removal", e);
                    }
                    token = {};
                }
            };

        try
        {
            decrementToken = decrement.Click(
                [state, generation](
                    WF::IInspectable const&,
                    MUX::RoutedEventArgs const&)
                {
                    wxCalendarCtrl * const liveOwner =
                        state->GetOwner(generation);
                    if ( !liveOwner || !liveOwner->m_winui ||
                         liveOwner->m_winui->callbackState != state )
                    {
                        return;
                    }
                    liveOwner->m_winui->QueueRelativeMonth(
                        liveOwner, -1);
                });
            owner = state->GetOwner(generation);
            if ( !owner || !owner->m_winui ||
                 owner->m_winui.get() != this ||
                 owner->m_winui->callbackState != state ||
                 owner->m_winui->cal != calendar )
            {
                revokeLocal(decrement, decrementToken);
                return false;
            }

            incrementToken = increment.Click(
                [state, generation](
                    WF::IInspectable const&,
                    MUX::RoutedEventArgs const&)
                {
                    wxCalendarCtrl * const liveOwner =
                        state->GetOwner(generation);
                    if ( !liveOwner || !liveOwner->m_winui ||
                         liveOwner->m_winui->callbackState != state )
                    {
                        return;
                    }
                    liveOwner->m_winui->QueueRelativeMonth(
                        liveOwner, 1);
                });
        }
        catch ( const winrt::hresult_error& e )
        {
            revokeLocal(decrement, decrementToken);
            revokeLocal(increment, incrementToken);
            wxWinUILogException(
                "WinUI CalendarView navigation handler registration", e);
            recordFailure(
                NavigationRefreshFailure::HandlerRegistration);
            return false;
        }

        const bool peersStillAttached =
            IsElementAttachedToCalendar(
                ticket, decrement.as<MUX::FrameworkElement>()) &&
            IsElementAttachedToCalendar(
                ticket, increment.as<MUX::FrameworkElement>());
        if ( !peersStillAttached )
        {
            revokeLocal(decrement, decrementToken);
            revokeLocal(increment, incrementToken);
            recordFailure(
                GetCurrentOwner(ticket)
                    ? NavigationRefreshFailure::DetachedPeer
                    : NavigationRefreshFailure::Superseded);
            return false;
        }

        owner = state->GetOwner(generation);
        if ( !owner || !owner->m_winui ||
             owner->m_winui.get() != this ||
             owner->m_winui->callbackState != state ||
             owner->m_winui->cal != calendar )
        {
            revokeLocal(decrement, decrementToken);
            revokeLocal(increment, incrementToken);
            return false;
        }

        decrementButton = decrement;
        incrementButton = increment;
        decrementClickToken = decrementToken;
        incrementClickToken = incrementToken;
        navigationBindingRequired = false;
        navigationLastFailure = NavigationRefreshFailure::None;
        if ( ++navigationButtonGeneration == 0 )
            ++navigationButtonGeneration;
        return true;
    }

    void Close()
    {
        if ( callbackState )
            callbackState->Invalidate();

        RevokeNavigationButtons();

        if ( cal && layoutUpdatedToken.value )
        {
            try
            {
                cal.LayoutUpdated(layoutUpdatedToken);
            }
            catch ( const winrt::hresult_error& e )
            {
                wxWinUILogException(
                    "WinUI CalendarView LayoutUpdated removal", e);
            }
        }
        layoutUpdatedToken = {};

        if ( layoutRoot && layoutLoadedToken.value )
        {
            try
            {
                layoutRoot.Loaded(layoutLoadedToken);
            }
            catch ( const winrt::hresult_error& e )
            {
                wxWinUILogException(
                    "WinUI CalendarView layout Loaded removal", e);
            }
        }
        layoutLoadedToken = {};

        if ( cal && actualThemeChangedToken.value )
        {
            try
            {
                cal.ActualThemeChanged(actualThemeChangedToken);
            }
            catch ( const winrt::hresult_error& e )
            {
                wxWinUILogException(
                    "WinUI CalendarView ActualThemeChanged removal", e);
            }
        }
        actualThemeChangedToken = {};

        if ( cal && dayItemChangingToken.value )
        {
            try
            {
                cal.CalendarViewDayItemChanging(dayItemChangingToken);
            }
            catch ( const winrt::hresult_error& e )
            {
                wxWinUILogException(
                    "WinUI CalendarView day item removal", e);
            }
        }
        dayItemChangingToken = {};

        if ( cal && doubleTappedToken.value )
        {
            try
            {
                cal.DoubleTapped(doubleTappedToken);
            }
            catch ( const winrt::hresult_error& e )
            {
                wxWinUILogException(
                    "WinUI CalendarView DoubleTapped removal", e);
            }
        }
        doubleTappedToken = {};

        if ( cal && selectionToken.value )
        {
            try
            {
                cal.SelectedDatesChanged(selectionToken);
            }
            catch ( const winrt::hresult_error& e )
            {
                wxWinUILogException(
                    "WinUI CalendarView selection removal", e);
            }
        }
        selectionToken = {};

        if ( !realizedDays.empty() )
        {
            realizedDays.clear();
            NoteRealizedDaysChanged();
        }
        appliedDensityColours.clear();
        appliedDensityColourRevisions.clear();
        automaticHolidayKeys.clear();
        refreshHolidaysInProgress = false;
        refreshHolidaysPending = false;
        refreshMarksInProgress = false;
        refreshMarksPending = false;
        refreshWeeksInProgress = false;
        refreshWeeksPending = false;
        weekRefreshPendingCause = WeekRefreshPendingCause::None;
        weekRefreshScheduled = false;
        projectedWeekLayoutInputsValid = false;
        projectedWeekDayLayoutValid = false;
        projectedWeekDayLayout.clear();
        projectedWeekMonthDayCount = 0;
        projectedWeekColumnCount = 0;
        weekDayLayoutValidationPending = false;
        weekDayLayoutValidationScheduled = false;
        if ( ++weekDayLayoutValidationGeneration == 0 )
            ++weekDayLayoutValidationGeneration;
        if ( ++weekRefreshScheduleGeneration == 0 )
            ++weekRefreshScheduleGeneration;
        weekRefreshRetriesRemaining = 0;
        weekRefreshDegraded = false;
#ifdef WXWINUI_TEST_SUPPORT
        weekRefreshFaultsForTesting = 0;
        weekRefreshPendingPassesForTesting = 0;
        weekRefreshPeerMutationPassesForTesting = 0;
        weekDayLayoutValidationFaultsForTesting = 0;
        weekDayLayoutValidationFailureModeForTesting = 0;
#endif
        weekRefreshWaitingForMutationEnd = false;
        weekMutationProbeScheduled = false;
        if ( ++weekMutationProbeGeneration == 0 )
            ++weekMutationProbeGeneration;
        if ( ++weekRefreshRetryGeneration == 0 )
            ++weekRefreshRetryGeneration;
        navigationRefreshScheduled = false;
        if ( ++navigationRefreshScheduleGeneration == 0 )
            ++navigationRefreshScheduleGeneration;
        navigationBindingRequired = false;
        navigationSnapshotButtonCount = 0;
        navigationCandidateCount = 0;
        navigationLastFailure = NavigationRefreshFailure::None;
        host.Close();
        weekRows.clear();
        weekNumberFormatter = nullptr;
        weekCanvas = nullptr;
        weekColumn = nullptr;
        root = nullptr;
        layoutRoot = nullptr;
        layoutXamlRoot = nullptr;
        cal = nullptr;
        callbackState.reset();
        defaultsCaptured = false;
        initialSizePending = false;
        initialSizeProjectionComplete = false;
        suppressNextMinSizeWriter = false;
        suppressNextSizeWriter = false;
    }

    bool RemoveRealizedDay(const MUXC::CalendarViewDayItem& item)
    {
        bool changed = false;
        for ( auto it = realizedDays.begin();
              it != realizedDays.end(); )
        {
            const MUXC::CalendarViewDayItem realized = it->second.get();
            if ( !realized ||
                 winrt::get_abi(realized) == winrt::get_abi(item) )
            {
                appliedDensityColours.erase(it->first);
                appliedDensityColourRevisions.erase(it->first);
                it = realizedDays.erase(it);
                changed = true;
            }
            else
            {
                ++it;
            }
        }

        if ( changed )
            NoteRealizedDaysChanged();
        return changed;
    }

    bool IsMarked(const wxCalendarCtrl *owner,
                  const WF::DateTime& peerDate) const
    {
        if ( !owner )
            return false;

        WF::DateTime selectedPeer{};
        wxWinUICalendarMonthKey selectedMonth;
        wxWinUICalendarMonthKey itemMonth;
        int itemDay = 0;
        if ( !wxWinUIToDateTime(owner->m_date, &selectedPeer) ||
             !wxWinUIReadCalendarDay(
                 calendarIdentifier, selectedPeer,
                 &selectedMonth, nullptr) ||
             !wxWinUIReadCalendarDay(
                 calendarIdentifier, peerDate,
                 &itemMonth, &itemDay) ||
             !(selectedMonth == itemMonth) )
        {
            return false;
        }

        const unsigned day = static_cast<unsigned>(itemDay);
        return day >= 1 && day <= 31 &&
               (owner->m_marks & (1u << (day - 1))) != 0;
    }

    bool IsHoliday(const wxCalendarCtrl *owner,
                   const WF::DateTime& peerDate) const
    {
        if ( !owner || !owner->HasFlag(wxCAL_SHOW_HOLIDAYS) )
        {
            return false;
        }

        const wxDateTime civilDate =
            wxWinUIFromDateTime(peerDate);
        const int civilKey = wxWinUICivilDateKey(civilDate);
        if ( civilKey != 0 &&
             automaticHolidayKeys.find(civilKey) !=
                 automaticHolidayKeys.end() )
        {
            return true;
        }

        WF::DateTime selectedPeer{};
        wxWinUICalendarMonthKey selectedMonth;
        wxWinUICalendarMonthKey itemMonth;
        int itemDay = 0;
        if ( !wxWinUIToDateTime(owner->m_date, &selectedPeer) ||
             !wxWinUIReadCalendarDay(
                 calendarIdentifier, selectedPeer,
                 &selectedMonth, nullptr) ||
             !wxWinUIReadCalendarDay(
                 calendarIdentifier, peerDate,
                 &itemMonth, &itemDay) ||
             !(selectedMonth == itemMonth) )
        {
            return false;
        }

        const unsigned day = static_cast<unsigned>(itemDay);
        return day >= 1 && day <= 31 &&
               (owner->m_holidays & (1u << (day - 1))) != 0;
    }

    bool ApplyMark(wxCalendarCtrl *owner,
                   const MUXC::CalendarViewDayItem& item)
    {
        const auto state = callbackState;
        const std::uint64_t generation =
            state ? state->Generation() : 0;
        const MUXC::CalendarView calendar = cal;
        if ( !state || state->GetOwner(generation) != owner ||
             !owner || !owner->m_winui ||
             owner->m_winui.get() != this ||
             owner->m_winui->callbackState != state ||
             owner->m_winui->cal != calendar )
        {
            return false;
        }

        const WF::DateTime peerDate = item.Date();
        const wxDateTime date = wxWinUIFromDateTime(peerDate);
        owner = state->GetOwner(generation);
        if ( !owner || !owner->m_winui ||
             owner->m_winui.get() != this ||
             owner->m_winui->callbackState != state ||
             owner->m_winui->cal != calendar )
        {
            return false;
        }
        if ( !date.IsValid() )
            return true;

        const int key = wxWinUICivilDateKey(date);
        if ( key == 0 )
            return true;

        const auto realized = realizedDays.find(key);
        if ( realized == realizedDays.end() )
            return true;
        const MUXC::CalendarViewDayItem currentItem =
            realized->second.get();
        owner = state->GetOwner(generation);
        if ( !owner || !owner->m_winui ||
             owner->m_winui.get() != this ||
             owner->m_winui->callbackState != state ||
             owner->m_winui->cal != calendar )
        {
            return false;
        }
        if ( !currentItem ||
             winrt::get_abi(currentItem) != winrt::get_abi(item) )
        {
            return true;
        }

        std::vector<WU::Color> applied;
        auto colours = winrt::single_threaded_vector<WU::Color>();
        // wxMSW represents both explicit marks and locale holidays through
        // the same native DAYSTATE emphasis. Keep that observable parity in
        // CalendarView while retaining separate model bitsets.
        if ( IsMarked(owner, peerDate) ||
             IsHoliday(owner, peerDate) )
        {
            const WU::Color colour =
                wxWinUICalendarMarkColour(calendar);
            colours.Append(colour);
            applied.push_back(colour);
        }
        owner = state->GetOwner(generation);
        if ( !owner || !owner->m_winui ||
             owner->m_winui.get() != this ||
             owner->m_winui->callbackState != state ||
             owner->m_winui->cal != calendar )
        {
            return false;
        }

        appliedDensityColours.erase(key);
        appliedDensityColourRevisions.erase(key);
        item.SetDensityColors(colours);

        // SetDensityColors is a synchronous WinRT boundary. It can realize or
        // recycle day items and, under test seams, can destroy the wx owner.
        // Reacquire the exact generation and exact realized item before
        // publishing a successful-write snapshot.
        owner = state->GetOwner(generation);
        if ( !owner || !owner->m_winui ||
             owner->m_winui.get() != this ||
             owner->m_winui->callbackState != state ||
             owner->m_winui->cal != calendar )
        {
            return false;
        }
        const auto current = realizedDays.find(key);
        if ( current == realizedDays.end() )
            return true;
        const MUXC::CalendarViewDayItem itemAfterWrite =
            current->second.get();
        owner = state->GetOwner(generation);
        if ( !owner || !owner->m_winui ||
             owner->m_winui.get() != this ||
             owner->m_winui->callbackState != state ||
             owner->m_winui->cal != calendar )
        {
            return false;
        }
        if ( !itemAfterWrite ||
             winrt::get_abi(itemAfterWrite) !=
                 winrt::get_abi(item) )
        {
            return true;
        }

        // CalendarViewDayItem has a setter-only DensityColors contract.
        // Publish the seam snapshot only after the real peer accepted the
        // exact iterable, never from the wx mark bitset alone.
        appliedDensityColours[key] = std::move(applied);
        if ( ++densityColourWriteRevision == 0 )
            ++densityColourWriteRevision;
        appliedDensityColourRevisions[key] =
            densityColourWriteRevision;
        return true;
    }

    void OnDayItemChanging(
        wxCalendarCtrl *owner,
        const MUXC::CalendarViewDayItemChangingEventArgs& args)
    {
        wxWinUICalendarImpl * const impl = this;
        const auto state = callbackState;
        const std::uint64_t generation =
            state ? state->Generation() : 0;
        const MUXC::CalendarView calendar = cal;
        const auto getExactOwner =
            [state, generation, impl, calendar]() -> wxCalendarCtrl *
            {
                wxCalendarCtrl * const live =
                    state ? state->GetOwner(generation) : nullptr;
                return live && live->m_winui &&
                       live->m_winui.get() == impl &&
                       live->m_winui->callbackState == state &&
                       live->m_winui->cal == calendar
                           ? live
                           : nullptr;
            };
        if ( getExactOwner() != owner )
            return;

        const MUXC::CalendarViewDayItem item = args.Item();
        if ( !item )
            return;
        owner = getExactOwner();
        if ( !owner )
            return;

        const bool inRecycleQueue = args.InRecycleQueue();
        owner = getExactOwner();
        if ( !owner )
            return;
        if ( inRecycleQueue )
        {
            const bool realizationChanged = RemoveRealizedDay(item);
            owner = getExactOwner();
            if ( !owner )
                return;
            if ( realizationChanged )
                ++dayItemIdentityChangeCount;
            else
                ++dayItemRepeatNotificationCount;
            weekDayLayoutValidationPending = true;
            if ( navigationBindingRequired )
            {
                QueueNavigationRefresh(owner);
                owner = getExactOwner();
                if ( !owner )
                    return;
            }
            // Container identity alone is not a new layout request. The
            // queued validator authenticates the resulting civil-date and
            // geometry signature after CalendarView finishes the recycle
            // batch, coalescing any number of item notifications into one
            // genuine refresh only when that signature actually changed.
            QueueWeekDayLayoutValidation(owner);
            return;
        }

        const wxDateTime date = wxWinUIFromDateTime(item.Date());
        owner = getExactOwner();
        if ( !owner )
            return;
        const int key = wxWinUICivilDateKey(date);
        bool realizationChanged = false;
        if ( key != 0 )
        {
            const auto existing = realizedDays.find(key);
            const MUXC::CalendarViewDayItem existingItem =
                existing != realizedDays.end()
                    ? existing->second.get()
                    : MUXC::CalendarViewDayItem{ nullptr };
            if ( !existingItem ||
                 winrt::get_abi(existingItem) !=
                     winrt::get_abi(item) )
            {
                realizedDays[key] = winrt::make_weak(item);
                NoteRealizedDaysChanged();
                realizationChanged = true;
            }
        }
        owner = getExactOwner();
        if ( !owner )
            return;
        if ( realizationChanged )
            ++dayItemIdentityChangeCount;
        else
            ++dayItemRepeatNotificationCount;
        if ( navigationBindingRequired )
        {
            QueueNavigationRefresh(owner);
            owner = getExactOwner();
            if ( !owner )
                return;
        }

        try
        {
            // This callback realizes one item at a time. Reapplying density
            // colours to every item already on screen here turns initial
            // realization into an O(n^2) sequence of synchronous XAML writes
            // (and can keep the UI dispatcher busy for many seconds). Model
            // and theme changes still use RefreshMarks(); realization only
            // needs to initialize the item which actually changed.
            if ( !ApplyMark(owner, item) )
                return;
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException(
                "WinUI CalendarView realized-day mark", e);
        }
        owner = getExactOwner();
        if ( !owner )
            return;
        // CalendarView can deliver later phases or a new projected identity
        // after a stable projection. Neither is independently a layout
        // request: authenticate the completed civil-date/geometry signature
        // once for the entire recycle batch.
        weekDayLayoutValidationPending = true;
        QueueWeekDayLayoutValidation(owner);
    }

    void RefreshMarks(wxCalendarCtrl *owner)
    {
        const auto state = callbackState;
        const std::uint64_t generation =
            state ? state->Generation() : 0;
        const MUXC::CalendarView calendar = cal;
        if ( !state || state->GetOwner(generation) != owner ||
             !owner || !owner->m_winui ||
             owner->m_winui.get() != this ||
             owner->m_winui->callbackState != state ||
             owner->m_winui->cal != calendar )
        {
            return;
        }

        if ( refreshMarksInProgress )
        {
            refreshMarksPending = true;
            return;
        }

        refreshMarksInProgress = true;
        constexpr unsigned MaxRefreshPasses = 8;
        for ( unsigned pass = 0; pass < MaxRefreshPasses; ++pass )
        {
            refreshMarksPending = false;
            std::vector<std::pair<int, RealizedDay>> snapshot;
            snapshot.reserve(realizedDays.size());
            for ( const auto& realized : realizedDays )
                snapshot.push_back(realized);

            for ( const auto& realized : snapshot )
            {
                owner = state->GetOwner(generation);
                if ( !owner || !owner->m_winui ||
                     owner->m_winui.get() != this ||
                     owner->m_winui->callbackState != state ||
                     owner->m_winui->cal != calendar )
                {
                    return;
                }

                const MUXC::CalendarViewDayItem item =
                    realized.second.get();
                owner = state->GetOwner(generation);
                if ( !owner || !owner->m_winui ||
                     owner->m_winui.get() != this ||
                     owner->m_winui->callbackState != state ||
                     owner->m_winui->cal != calendar )
                {
                    return;
                }

                if ( !item )
                {
                    const auto current =
                        realizedDays.find(realized.first);
                    if ( current != realizedDays.end() &&
                         !current->second.get() )
                    {
                        appliedDensityColours.erase(
                            realized.first);
                        appliedDensityColourRevisions.erase(
                            realized.first);
                        realizedDays.erase(current);
                        NoteRealizedDaysChanged();
                    }
                    continue;
                }

                try
                {
                    if ( !ApplyMark(owner, item) )
                        return;
                }
                catch ( const winrt::hresult_error& e )
                {
                    // Mark refresh is presentation-only and must never
                    // unwind through a CalendarView selection callback.
                    wxWinUILogException(
                        "WinUI CalendarView mark refresh", e);
                    owner = state->GetOwner(generation);
                    if ( !owner || !owner->m_winui ||
                         owner->m_winui.get() != this ||
                         owner->m_winui->callbackState != state ||
                         owner->m_winui->cal != calendar )
                    {
                        return;
                    }
                }
            }

            owner = state->GetOwner(generation);
            if ( !owner || !owner->m_winui ||
                 owner->m_winui.get() != this ||
                 owner->m_winui->callbackState != state ||
                 owner->m_winui->cal != calendar )
            {
                return;
            }
            if ( !refreshMarksPending )
            {
                refreshMarksInProgress = false;
                return;
            }
        }

        refreshMarksPending = false;
        refreshMarksInProgress = false;
        wxLogWarning(
            "WinUI CalendarView mark refresh did not converge; "
            "deferring further work to the next theme/model change.");
    }

    void RefreshHolidayDates(wxCalendarCtrl *owner)
    {
        const auto state = callbackState;
        const std::uint64_t generation =
            state ? state->Generation() : 0;
        const MUXC::CalendarView calendar = cal;
        if ( !state || !owner ||
             state->GetOwner(generation) != owner ||
             !owner->m_winui || owner->m_winui.get() != this ||
             owner->m_winui->callbackState != state ||
             owner->m_winui->cal != calendar )
        {
            return;
        }

        // Invalidate the currently-computing request before crossing any
        // WinRT or application-code boundary. A holiday authority is fully
        // extensible: it may change the selected month/range/style, reparent
        // the control, recursively refresh holidays, or destroy it outright.
        // A nested refresh only records a newer request; the outermost call
        // owns convergence and is the only one allowed to publish.
        if ( ++holidayRefreshRevision == 0 )
            ++holidayRefreshRevision;
        refreshHolidaysPending = true;
        if ( refreshHolidaysInProgress )
            return;
        refreshHolidaysInProgress = true;

        constexpr unsigned MaxRefreshPasses = 8;
        for ( unsigned pass = 0; pass != MaxRefreshPasses; ++pass )
        {
            owner = state->GetOwner(generation);
            if ( !owner || !owner->m_winui ||
                 owner->m_winui.get() != this ||
                 owner->m_winui->callbackState != state ||
                 owner->m_winui->cal != calendar )
            {
                // The implementation may already have been destroyed. Do not
                // touch any member on this path.
                return;
            }

            refreshHolidaysPending = false;
            const std::uint64_t requestRevision =
                holidayRefreshRevision;
            const long requestedStyle = owner->GetWindowStyleFlag();
            const wxDateTime requestedDate = owner->m_date;
            const wxDateTime requestedMinimum = owner->m_lowerDate;
            const wxDateTime requestedMaximum = owner->m_upperDate;
            wxWindow * const requestedTopLevel =
                wxGetTopLevelParent(owner);
            const winrt::hstring identifier = calendarIdentifier;

            winrt::hstring peerIdentifier;
            winrt::hstring peerLanguage;
            MUX::XamlRoot peerXamlRoot{ nullptr };
            wxWinUICalendarMonthKey requestedMonth;
            try
            {
                WF::DateTime selectedPeer{};
                if ( !wxWinUIToDateTime(
                         requestedDate, &selectedPeer) )
                {
                    refreshHolidaysInProgress = false;
                    return;
                }

                peerIdentifier = calendar.CalendarIdentifier();
                peerLanguage = calendar.Language();
                peerXamlRoot = calendar.XamlRoot();
                const bool haveRequestedMonth =
                    wxWinUIReadCalendarDay(
                        identifier, selectedPeer,
                        &requestedMonth, nullptr);

                owner = state->GetOwner(generation);
                if ( !owner || !owner->m_winui ||
                     owner->m_winui.get() != this ||
                     owner->m_winui->callbackState != state ||
                     owner->m_winui->cal != calendar )
                {
                    return;
                }
                if ( peerIdentifier != identifier ||
                     !haveRequestedMonth )
                {
                    refreshHolidaysInProgress = false;
                    refreshHolidaysPending = true;
                    return;
                }

                std::set<int> holidayKeys;
                if ( requestedStyle & wxCAL_SHOW_HOLIDAYS )
                {
                    WG::Calendar model;
                    model.ChangeCalendarSystem(identifier);
                    model.SetDateTime(selectedPeer);
                    model.Day(model.FirstDayInThisMonth());
                    const wxDateTime first =
                        wxWinUIFromDateTime(model.GetDateTime());
                    model.Day(model.LastDayInThisMonth());
                    const wxDateTime last =
                        wxWinUIFromDateTime(model.GetDateTime());
                    if ( !first.IsValid() || !last.IsValid() ||
                         first > last )
                    {
                        refreshHolidaysInProgress = false;
                        return;
                    }

                    wxDateTimeArray holidays;
                    wxDateTimeHolidayAuthority::GetHolidaysInRange(
                        first, last, holidays);

                    // Holiday authorities are extensible application code.
                    // Reacquire the exact peer generation before even reading
                    // the returned array or any live wx state.
                    owner = state->GetOwner(generation);
                    if ( !owner || !owner->m_winui ||
                         owner->m_winui.get() != this ||
                         owner->m_winui->callbackState != state ||
                         owner->m_winui->cal != calendar )
                    {
                        return;
                    }

                    for ( const wxDateTime& holiday : holidays )
                    {
                        const int key = wxWinUICivilDateKey(holiday);
                        if ( key != 0 )
                            holidayKeys.insert(key);
                    }
                }

                // A nested refresh invalidates this request before doing any
                // work. Context changes which do not themselves refresh
                // holidays (notably same-month SetDateRange() and reparent)
                // are detected explicitly and retried against the new state.
                WF::DateTime currentPeer{};
                wxWinUICalendarMonthKey currentMonth;
                const winrt::hstring currentPeerIdentifier =
                    calendar.CalendarIdentifier();
                const winrt::hstring currentPeerLanguage =
                    calendar.Language();
                const MUX::XamlRoot currentPeerXamlRoot =
                    calendar.XamlRoot();
                owner = state->GetOwner(generation);
                if ( !owner || !owner->m_winui ||
                     owner->m_winui.get() != this ||
                     owner->m_winui->callbackState != state ||
                     owner->m_winui->cal != calendar )
                {
                    return;
                }

                const bool haveCurrentMonth =
                    wxWinUIToDateTime(owner->m_date, &currentPeer) &&
                    wxWinUIReadCalendarDay(
                        identifier, currentPeer,
                        &currentMonth, nullptr);
                owner = state->GetOwner(generation);
                if ( !owner || !owner->m_winui ||
                     owner->m_winui.get() != this ||
                     owner->m_winui->callbackState != state ||
                     owner->m_winui->cal != calendar )
                {
                    return;
                }

                const bool contextIsCurrent =
                    holidayRefreshRevision == requestRevision &&
                    owner->GetWindowStyleFlag() == requestedStyle &&
                    wxWinUISameCivilDate(owner->m_date, requestedDate) &&
                    wxWinUISameCivilDate(
                        owner->m_lowerDate, requestedMinimum) &&
                    wxWinUISameCivilDate(
                        owner->m_upperDate, requestedMaximum) &&
                    wxGetTopLevelParent(owner) == requestedTopLevel &&
                    owner->m_winui->calendarIdentifier == identifier &&
                    currentPeerIdentifier == peerIdentifier &&
                    currentPeerLanguage == peerLanguage &&
                    winrt::get_abi(currentPeerXamlRoot) ==
                        winrt::get_abi(peerXamlRoot) &&
                    haveCurrentMonth &&
                    currentMonth == requestedMonth;
                if ( !contextIsCurrent )
                {
                    if ( holidayRefreshRevision == requestRevision &&
                         ++holidayRefreshRevision == 0 )
                    {
                        ++holidayRefreshRevision;
                    }
                    refreshHolidaysPending = true;
                    continue;
                }

                owner->m_winui->automaticHolidayKeys =
                    std::move(holidayKeys);
                owner->m_winui->RefreshMarks(owner);
                // RefreshMarks() crosses XAML and can destroy the owner.
            }
            catch ( const winrt::hresult_error& e )
            {
                wxWinUILogException(
                    "WinUI CalendarView holiday range", e);
                owner = state->GetOwner(generation);
                if ( !owner || !owner->m_winui ||
                     owner->m_winui.get() != this ||
                     owner->m_winui->callbackState != state ||
                     owner->m_winui->cal != calendar )
                {
                    return;
                }
                refreshHolidaysInProgress = false;
                refreshHolidaysPending = true;
                return;
            }

            owner = state->GetOwner(generation);
            if ( !owner || !owner->m_winui ||
                 owner->m_winui.get() != this ||
                 owner->m_winui->callbackState != state ||
                 owner->m_winui->cal != calendar )
            {
                return;
            }
            if ( !refreshHolidaysPending )
            {
                refreshHolidaysInProgress = false;
                return;
            }
        }

        refreshHolidaysInProgress = false;
        refreshHolidaysPending = true;
        wxLogWarning(
            "WinUI CalendarView holiday refresh did not converge; "
            "deferring further work to the next model change.");
    }

    void RefreshWeekNumbers(wxCalendarCtrl *owner)
    {
        const auto state = callbackState;
        const std::uint64_t generation = state ? state->Generation() : 0;
        const MUXC::CalendarView calendar = cal;
        if ( !state || !owner ||
             state->GetOwner(generation) != owner ||
             !owner->m_winui || owner->m_winui.get() != this ||
             owner->m_winui->callbackState != state ||
             owner->m_winui->cal != calendar )
        {
            return;
        }

        // Only a genuine model/layout request starts a new recovery
        // generation. Automatic recovery callbacks must never manufacture a
        // new request revision or rearm their own retry budget.
        if ( ++weekRefreshRequestRevision == 0 )
            ++weekRefreshRequestRevision;
        if ( ++weekRefreshRetryGeneration == 0 )
            ++weekRefreshRetryGeneration;
        weekRefreshRetriesRemaining = MaxWeekRefreshAutomaticRetries;
        weekRefreshDegraded = false;
        refreshWeeksPending = false;
        weekRefreshPendingCause = WeekRefreshPendingCause::None;
        weekRefreshWaitingForMutationEnd = false;
        weekMutationProbeScheduled = false;
        if ( ++weekMutationProbeGeneration == 0 )
            ++weekMutationProbeGeneration;
        ScheduleWeekRefresh(owner);
    }

    void ScheduleWeekRefresh(wxCalendarCtrl *owner)
    {
        const auto state = callbackState;
        const std::uint64_t generation = state ? state->Generation() : 0;
        const MUXC::CalendarView calendar = cal;
        const MUX::FrameworkElement refreshRoot =
            layoutRoot ? layoutRoot : calendar.try_as<MUX::FrameworkElement>();
        if ( !state || !owner || !owner->m_winui ||
             owner->m_winui.get() != this ||
             owner->m_winui->callbackState != state ||
             owner->m_winui->cal != calendar || !refreshRoot )
        {
            return;
        }

        if ( weekRefreshScheduled )
            return;

        MUX::XamlRoot scheduledRoot{ nullptr };
        try
        {
            scheduledRoot = refreshRoot.XamlRoot();
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException(
                "WinUI CalendarView week-number XamlRoot", e);
            refreshWeeksPending = true;
            weekRefreshPendingCause = WeekRefreshPendingCause::QueueFailure;
            return;
        }

        owner = state->GetOwner(generation);
        if ( !owner || !owner->m_winui ||
             owner->m_winui.get() != this ||
             owner->m_winui->callbackState != state ||
             owner->m_winui->cal != calendar ||
             owner->m_winui->layoutRoot != refreshRoot )
        {
            return;
        }

        if ( ++weekRefreshScheduleGeneration == 0 )
            ++weekRefreshScheduleGeneration;
        const std::uint64_t scheduleGeneration =
            weekRefreshScheduleGeneration;
        const std::uint64_t scheduledLayoutRevision = layoutRevision;
        weekRefreshScheduled = true;
        bool queued = false;
        try
        {
            queued = calendar.DispatcherQueue().TryEnqueue(
                [state, generation, this, calendar, refreshRoot,
                 scheduledRoot, scheduleGeneration,
                 scheduledLayoutRevision]()
                {
                    wxCalendarCtrl *owner = state->GetOwner(generation);
                    if ( !owner || !owner->m_winui ||
                         owner->m_winui.get() != this ||
                         owner->m_winui->callbackState != state ||
                         owner->m_winui->cal != calendar ||
                         owner->m_winui->layoutRoot != refreshRoot ||
                         weekRefreshScheduleGeneration !=
                             scheduleGeneration ||
                         !weekRefreshScheduled )
                    {
                        return;
                    }

                    // Clear before executing the wave. Any XAML/layout edge
                    // caused by the refresh can then queue exactly one later
                    // wave instead of recursively refreshing this one.
                    weekRefreshScheduled = false;

                    MUX::XamlRoot currentRoot{ nullptr };
                    try
                    {
                        currentRoot = refreshRoot.XamlRoot();
                    }
                    catch ( const winrt::hresult_error& e )
                    {
                        wxWinUILogException(
                            "WinUI CalendarView queued week XamlRoot", e);
                        owner = state->GetOwner(generation);
                        if ( owner && owner->m_winui &&
                             owner->m_winui.get() == this &&
                             owner->m_winui->callbackState == state )
                        {
                            refreshWeeksPending = true;
                            weekRefreshPendingCause =
                                WeekRefreshPendingCause::QueueFailure;
                        }
                        return;
                    }

                    owner = state->GetOwner(generation);
                    if ( !owner || !owner->m_winui ||
                         owner->m_winui.get() != this ||
                         owner->m_winui->callbackState != state ||
                         owner->m_winui->cal != calendar ||
                         owner->m_winui->layoutRoot != refreshRoot ||
                         weekRefreshScheduleGeneration !=
                             scheduleGeneration )
                    {
                        return;
                    }

                    if ( currentRoot != scheduledRoot )
                    {
                        // The old island/root owns no writes. Loaded on the
                        // replacement root is a genuine new layout request.
                        RefreshWeekNumbers(owner);
                        return;
                    }
                    if ( layoutRevision != scheduledLayoutRevision )
                    {
                        // Loaded retargeted this same live root after the
                        // request was queued. Refresh the ticket, not the
                        // request: an automatic dispatcher callback must not
                        // manufacture a new recovery generation or retry
                        // budget for the same work.
                        ScheduleWeekRefresh(owner);
                        return;
                    }

                    if ( ++weekRefreshRunCount == 0 )
                        ++weekRefreshRunCount;
                    const std::uint64_t consumedRevision =
                        weekRefreshRequestRevision;
                    const std::uint64_t retryGeneration =
                        weekRefreshRetryGeneration;
                    bool refreshFailed = false;
#ifdef WXWINUI_TEST_SUPPORT
                    if ( weekRefreshFaultsForTesting != 0 )
                    {
                        --weekRefreshFaultsForTesting;
                        refreshFailed = true;
                    }
                    else
#endif
                    {
                        try
                        {
                            RefreshWeekNumbersImpl(owner);
                        }
                        catch ( const winrt::hresult_error& e )
                        {
                            wxWinUILogException(
                                "WinUI CalendarView week-number refresh", e);
                            refreshFailed = true;
                        }
                    }

                    owner = state->GetOwner(generation);
                    if ( !owner || !owner->m_winui ||
                         owner->m_winui.get() != this ||
                         owner->m_winui->callbackState != state ||
                         owner->m_winui->cal != calendar )
                    {
                        return;
                    }
                    if ( refreshFailed )
                    {
                        refreshWeeksInProgress = false;
                        refreshWeeksPending = false;
                        weekRefreshPendingCause =
                            WeekRefreshPendingCause::None;
                        weekRefreshWaitingForMutationEnd = false;
                        if ( weekRefreshRetryGeneration != retryGeneration ||
                             weekRefreshRequestRevision != consumedRevision )
                        {
                            // A genuine request arrived across the failing
                            // XAML boundary and already owns a fresh budget.
                            ScheduleWeekRefresh(owner);
                        }
                        else if ( weekRefreshRetriesRemaining != 0 )
                        {
                            --weekRefreshRetriesRemaining;
                            ScheduleWeekRefresh(owner);
                        }
                        else
                        {
                            // Fail closed after the bounded automatic budget.
                            // A later model/layout request clears this state
                            // and starts an independent recovery generation.
                            weekRefreshDegraded = true;
                        }
                        return;
                    }
                    const bool hasNewExternalRequest =
                        weekRefreshRetryGeneration != retryGeneration ||
                        weekRefreshRequestRevision != consumedRevision;
                    if ( hasNewExternalRequest )
                    {
                        ScheduleWeekRefresh(owner);
                    }
                    else if ( refreshWeeksPending )
                    {
                        if ( weekRefreshPendingCause ==
                                 WeekRefreshPendingCause::PeerMutation )
                        {
                            weekRefreshWaitingForMutationEnd = true;
                            if ( !state->IsPeerMutationInProgress() )
                            {
                                weekRefreshWaitingForMutationEnd = false;
                                refreshWeeksPending = false;
                                weekRefreshPendingCause =
                                    WeekRefreshPendingCause::None;
                                ScheduleWeekRefresh(owner);
                            }
                            else
                            {
                                QueueWeekRefreshAfterMutation(
                                    owner, consumedRevision,
                                    retryGeneration);
                            }
                        }
                        else if ( weekRefreshRetriesRemaining != 0 )
                        {
                            --weekRefreshRetriesRemaining;
                            ++boundedWeekPendingRetryCount;
                            ScheduleWeekRefresh(owner);
                        }
                        else
                        {
                            refreshWeeksInProgress = false;
                            refreshWeeksPending = false;
                            weekRefreshDegraded = true;
                        }
                    }
                    else
                    {
                        weekRefreshCompletedRevision = consumedRevision;
                    }
                });
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException(
                "WinUI CalendarView week-number queue", e);
        }

        owner = state->GetOwner(generation);
        if ( !queued && owner && owner->m_winui &&
             owner->m_winui.get() == this &&
             owner->m_winui->callbackState == state &&
             weekRefreshScheduleGeneration == scheduleGeneration )
        {
            weekRefreshScheduled = false;
            refreshWeeksPending = true;
            weekRefreshPendingCause = WeekRefreshPendingCause::QueueFailure;
        }
    }

    void QueueWeekRefreshAfterMutation(
        wxCalendarCtrl *owner,
        std::uint64_t requestRevision,
        std::uint64_t retryGeneration)
    {
        const auto state = callbackState;
        const std::uint64_t generation = state ? state->Generation() : 0;
        const MUXC::CalendarView calendar = cal;
        if ( weekMutationProbeScheduled ||
             !weekRefreshWaitingForMutationEnd || !state || !owner ||
             state->GetOwner(generation) != owner ||
             !owner->m_winui || owner->m_winui.get() != this ||
             owner->m_winui->callbackState != state ||
             owner->m_winui->cal != calendar ||
             weekRefreshRequestRevision != requestRevision ||
             weekRefreshRetryGeneration != retryGeneration )
        {
            return;
        }

        if ( ++weekMutationProbeGeneration == 0 )
            ++weekMutationProbeGeneration;
        const std::uint64_t probeGeneration =
            weekMutationProbeGeneration;
        weekMutationProbeScheduled = true;
        bool queued = false;
        try
        {
            queued = calendar.DispatcherQueue().TryEnqueue(
                MUXD::DispatcherQueuePriority::Low,
                [state, generation, this, calendar, requestRevision,
                 retryGeneration, probeGeneration]()
                {
                    wxCalendarCtrl *owner = state->GetOwner(generation);
                    if ( !owner || !owner->m_winui ||
                         owner->m_winui.get() != this ||
                         owner->m_winui->callbackState != state ||
                         owner->m_winui->cal != calendar ||
                         weekMutationProbeGeneration != probeGeneration ||
                         !weekMutationProbeScheduled )
                    {
                        return;
                    }

                    weekMutationProbeScheduled = false;
                    if ( !weekRefreshWaitingForMutationEnd ||
                         weekRefreshRequestRevision != requestRevision ||
                         weekRefreshRetryGeneration != retryGeneration ||
                         state->IsPeerMutationInProgress() )
                    {
                        return;
                    }

                    weekRefreshWaitingForMutationEnd = false;
                    refreshWeeksPending = false;
                    weekRefreshPendingCause =
                        WeekRefreshPendingCause::None;
                    ScheduleWeekRefresh(owner);
                });
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException(
                "WinUI CalendarView mutation-end probe queue", e);
        }

        owner = state->GetOwner(generation);
        if ( !queued && owner && owner->m_winui &&
             owner->m_winui.get() == this &&
             owner->m_winui->callbackState == state &&
             weekMutationProbeGeneration == probeGeneration )
        {
            weekMutationProbeScheduled = false;
        }
    }

    void OnPeerMutationEnded(wxCalendarCtrl *owner)
    {
        const auto state = callbackState;
        const std::uint64_t generation = state ? state->Generation() : 0;
        if ( !weekRefreshWaitingForMutationEnd || !state || !owner ||
             state->IsPeerMutationInProgress() ||
             state->GetOwner(generation) != owner ||
             !owner->m_winui || owner->m_winui.get() != this ||
             owner->m_winui->callbackState != state )
        {
            return;
        }

        weekRefreshWaitingForMutationEnd = false;
        refreshWeeksPending = false;
        weekRefreshPendingCause = WeekRefreshPendingCause::None;
        weekMutationProbeScheduled = false;
        if ( ++weekMutationProbeGeneration == 0 )
            ++weekMutationProbeGeneration;
        // Schedule only: the mutation-ended notification must not perform
        // the CalendarView projection synchronously in the guard destructor.
        ScheduleWeekRefresh(owner);
    }

    static void NotifyPeerMutationEnded(
        const std::shared_ptr<wxWinUICalendarCallbackState>& state,
        std::uint64_t generation)
    {
        wxCalendarCtrl * const owner =
            state ? state->GetOwner(generation) : nullptr;
        if ( !owner || !owner->m_winui ||
             owner->m_winui->callbackState != state )
        {
            return;
        }

        wxWinUICalendarImpl * const impl = owner->m_winui.get();
        impl->OnPeerMutationEnded(owner);
        // OnPeerMutationEnded() may queue through XAML and destroy the owner.
        // Do not inspect either pointer after this call.
    }

    bool CaptureWeekLayoutInputs(wxCalendarCtrl *owner,
                                 WeekLayoutInputs *inputs) const
    {
        if ( !inputs )
            return false;

        const auto state = callbackState;
        const std::uint64_t generation = state ? state->Generation() : 0;
        const MUXC::CalendarView calendar = cal;
        const MUX::FrameworkElement refreshRoot =
            layoutRoot ? layoutRoot : calendar.try_as<MUX::FrameworkElement>();
        if ( !state || state->GetOwner(generation) != owner ||
             !owner || !owner->m_winui ||
             owner->m_winui.get() != this ||
             owner->m_winui->callbackState != state ||
             owner->m_winui->cal != calendar || !refreshRoot )
        {
            return false;
        }

        WeekLayoutInputs captured;
        captured.xamlRoot = refreshRoot.XamlRoot();
        captured.width = refreshRoot.ActualWidth();
        captured.height = refreshRoot.ActualHeight();

        owner = state->GetOwner(generation);
        if ( !owner || !owner->m_winui ||
             owner->m_winui.get() != this ||
             owner->m_winui->callbackState != state ||
             owner->m_winui->cal != calendar ||
             owner->m_winui->layoutRoot != refreshRoot )
        {
            return false;
        }

        captured.layoutRevision = layoutRevision;
        captured.realizedDaysRevision = realizedDaysRevision;
        *inputs = captured;
        return true;
    }

    static bool SameWeekLayoutInputs(const WeekLayoutInputs& lhs,
                                     const WeekLayoutInputs& rhs)
    {
        return lhs.xamlRoot == rhs.xamlRoot &&
               lhs.layoutRevision == rhs.layoutRevision &&
               lhs.realizedDaysRevision == rhs.realizedDaysRevision &&
               std::abs(lhs.width - rhs.width) <= 0.25 &&
               std::abs(lhs.height - rhs.height) <= 0.25;
    }

    static bool MakeWeekDayLayoutSignature(
        const ActiveMonthPage& activePage,
        WeekDayLayoutSignature *signature,
        unsigned *columnCount = nullptr)
    {
        if ( !signature )
            return false;
        signature->clear();

        if ( activePage.currentDays.empty() )
            return false;
        if ( columnCount )
            *columnCount = activePage.columnCount;

        signature->reserve(activePage.currentDays.size());
        for ( const DayHitGeometry *day : activePage.currentDays )
        {
            signature->push_back(
                WeekDayLayoutInput{
                    wxWinUICivilDateKey(day->date),
                    day->bounds.X,
                    day->bounds.Y,
                    day->bounds.Width,
                    day->bounds.Height});
        }

        std::sort(
            signature->begin(), signature->end(),
            [](const WeekDayLayoutInput& lhs,
               const WeekDayLayoutInput& rhs)
            {
                if ( lhs.dateKey != rhs.dateKey )
                    return lhs.dateKey < rhs.dateKey;
                if ( lhs.y != rhs.y )
                    return lhs.y < rhs.y;
                if ( lhs.x != rhs.x )
                    return lhs.x < rhs.x;
                if ( lhs.height != rhs.height )
                    return lhs.height < rhs.height;
                return lhs.width < rhs.width;
            });
        return true;
    }

    static bool SameWeekDayLayoutSignature(
        const WeekDayLayoutSignature& lhs,
        const WeekDayLayoutSignature& rhs)
    {
        if ( lhs.size() != rhs.size() )
            return false;

        for ( std::size_t n = 0; n != lhs.size(); ++n )
        {
            if ( lhs[n].dateKey != rhs[n].dateKey ||
                 std::abs(lhs[n].x - rhs[n].x) > 0.25 ||
                 std::abs(lhs[n].y - rhs[n].y) > 0.25 ||
                 std::abs(lhs[n].width - rhs[n].width) > 0.25 ||
                 std::abs(lhs[n].height - rhs[n].height) > 0.25 )
            {
                return false;
            }
        }
        return true;
    }

    bool CaptureWeekDayLayoutSignature(
        wxCalendarCtrl *owner,
        WeekDayLayoutSignature *signature,
        std::uint64_t *realizedRevision,
        unsigned *monthDayCount = nullptr,
        unsigned *columnCount = nullptr) const
    {
        if ( !signature || !realizedRevision )
            return false;
        signature->clear();
        *realizedRevision = 0;
        if ( monthDayCount )
            *monthDayCount = 0;
        if ( columnCount )
            *columnCount = 0;

        const HitTestTicket ticket = CaptureHitTestTicket();
        if ( GetCurrentOwner(ticket) != owner )
            return false;

        PublicHitSnapshot snapshot;
        if ( !CapturePublicHitSnapshot(ticket, &snapshot) ||
             GetCurrentOwner(ticket) != owner )
        {
            return false;
        }

        ActiveMonthPage activePage;
        if ( !ResolveActiveMonthPage(
                 ticket, snapshot, &activePage) )
            return false;

        WeekDayLayoutSignature captured;
        unsigned capturedColumnCount = 0;
        if ( !MakeWeekDayLayoutSignature(
                 activePage, &captured,
                 &capturedColumnCount) )
        {
            return false;
        }
        if ( GetCurrentOwner(ticket) != owner )
            return false;

        *signature = std::move(captured);
        *realizedRevision = ticket.realizedDaysRevision;
        if ( monthDayCount )
        {
            *monthDayCount =
                static_cast<unsigned>(signature->size());
        }
        if ( columnCount )
            *columnCount = capturedColumnCount;
        return true;
    }

    void RecordWeekLayoutProjection(wxCalendarCtrl *owner)
    {
        WeekDayLayoutSignature signature;
        std::uint64_t signatureRevision = 0;
        unsigned monthDayCount = 0;
        unsigned columnCount = 0;
        if ( !CaptureWeekDayLayoutSignature(
                 owner, &signature, &signatureRevision,
                 &monthDayCount, &columnCount) )
        {
            return;
        }

        WeekLayoutInputs inputs;
        if ( CaptureWeekLayoutInputs(owner, &inputs) &&
             inputs.realizedDaysRevision == signatureRevision )
        {
            projectedWeekLayoutInputs = inputs;
            projectedWeekDayLayout = std::move(signature);
            projectedWeekDayLayoutValid = true;
            projectedWeekLayoutInputsValid = true;
            projectedWeekMonthDayCount = monthDayCount;
            projectedWeekColumnCount = columnCount;
            weekDayLayoutValidationPending = false;
        }
    }

    void RecoverWeekDayLayoutValidationFailure(
        wxCalendarCtrl *owner,
        std::uint64_t validationGeneration,
        std::uint64_t requestRevision,
        std::uint64_t retryGeneration)
    {
        const auto state = callbackState;
        const std::uint64_t generation = state ? state->Generation() : 0;
        const MUXC::CalendarView calendar = cal;
        if ( !state || state->GetOwner(generation) != owner || !owner ||
             !owner->m_winui || owner->m_winui.get() != this ||
             owner->m_winui->callbackState != state ||
             owner->m_winui->cal != calendar ||
             weekDayLayoutValidationGeneration !=
                 validationGeneration ||
             weekDayLayoutValidationScheduled ||
             !weekDayLayoutValidationPending )
        {
            return;
        }

        ++weekDayLayoutValidationFailureRecoveryCount;

        if ( weekRefreshRequestRevision != requestRevision ||
             weekRefreshRetryGeneration != retryGeneration )
        {
            // A genuine request crossed the failing XAML boundary and owns
            // its own generation and full budget. Revalidate that live work
            // without mutating the stale callback's accounting.
            QueueWeekDayLayoutValidation(owner);
            return;
        }

        if ( weekRefreshRetriesRemaining != 0 )
        {
            --weekRefreshRetriesRemaining;
            ++boundedWeekPendingRetryCount;
            QueueWeekDayLayoutValidation(owner);
            return;
        }

        weekDayLayoutValidationPending = false;
        weekRefreshDegraded = true;
        wxLogWarning(
            "WinUI CalendarView day-layout validation failed to converge; "
            "deferring further work to the next model change.");
    }

    void QueueWeekDayLayoutValidation(wxCalendarCtrl *owner)
    {
        const auto state = callbackState;
        const std::uint64_t generation = state ? state->Generation() : 0;
        const MUXC::CalendarView calendar = cal;
        if ( weekDayLayoutValidationScheduled || !state || !owner ||
             state->GetOwner(generation) != owner || !owner->m_winui ||
             owner->m_winui.get() != this ||
             owner->m_winui->callbackState != state ||
             owner->m_winui->cal != calendar )
        {
            return;
        }

        if ( ++weekDayLayoutValidationGeneration == 0 )
            ++weekDayLayoutValidationGeneration;
        const std::uint64_t validationGeneration =
            weekDayLayoutValidationGeneration;
        const std::uint64_t validationRequestRevision =
            weekRefreshRequestRevision;
        const std::uint64_t validationRetryGeneration =
            weekRefreshRetryGeneration;
        weekDayLayoutValidationScheduled = true;
        bool queued = false;
#ifdef WXWINUI_TEST_SUPPORT
        const bool injectQueueRefusal =
            weekDayLayoutValidationFaultsForTesting != 0 &&
            weekDayLayoutValidationFailureModeForTesting ==
                static_cast<unsigned>(
                    wxWinUICalendarTestAccess::DayLayoutValidationFailure::
                            QueueRefusal);
        if ( injectQueueRefusal )
        {
            --weekDayLayoutValidationFaultsForTesting;
            weekDayLayoutValidationFailureModeForTesting = 0;
        }
        else
#endif
        try
        {
            queued = calendar.DispatcherQueue().TryEnqueue(
                [state, generation, implementation = this, calendar,
                 validationGeneration]()
                {
                    wxCalendarCtrl *owner = state->GetOwner(generation);
                    if ( !owner || !owner->m_winui ||
                         owner->m_winui.get() != implementation ||
                         owner->m_winui->callbackState != state ||
                         owner->m_winui->cal != calendar ||
                         implementation->
                             weekDayLayoutValidationGeneration !=
                             validationGeneration ||
                         !implementation->
                             weekDayLayoutValidationScheduled )
                    {
                        return;
                    }

                    implementation->weekDayLayoutValidationScheduled =
                        false;
                    if ( !implementation->weekDayLayoutValidationPending )
                        return;

                    const std::uint64_t requestRevision =
                        implementation->weekRefreshRequestRevision;
                    const std::uint64_t retryGeneration =
                        implementation->weekRefreshRetryGeneration;
                    try
                    {
#ifdef WXWINUI_TEST_SUPPORT
                        bool injectTransientRead = false;
                        if ( implementation->
                                 weekDayLayoutValidationFaultsForTesting !=
                             0 )
                        {
                            const unsigned failureMode = implementation->
                                weekDayLayoutValidationFailureModeForTesting;
                            --implementation->
                                weekDayLayoutValidationFaultsForTesting;
                            implementation->
                                weekDayLayoutValidationFailureModeForTesting =
                                    0;
                            if ( failureMode ==
                                 static_cast<unsigned>(
                                     wxWinUICalendarTestAccess::DayLayoutValidationFailure::
                                             Exception) )
                            {
                                throw winrt::hresult_error(E_FAIL);
                            }
                            injectTransientRead =
                                failureMode ==
                                static_cast<unsigned>(
                                    wxWinUICalendarTestAccess::DayLayoutValidationFailure::
                                            TransientRead);
                        }
                        if ( !injectTransientRead )
#endif
                        {
                            implementation->
                                RefreshWeekNumbersFromLayout(owner);
                        }
                    }
                    catch ( const winrt::hresult_error& e )
                    {
                        wxWinUILogException(
                            "WinUI CalendarView day-layout validation", e);
                        owner = state->GetOwner(generation);
                        if ( !owner || !owner->m_winui ||
                             owner->m_winui.get() != implementation ||
                             owner->m_winui->callbackState != state ||
                             owner->m_winui->cal != calendar )
                        {
                            return;
                        }
                        owner->m_winui->
                            RecoverWeekDayLayoutValidationFailure(
                                owner, validationGeneration,
                                requestRevision, retryGeneration);
                        return;
                    }
                    catch ( ... )
                    {
                        wxLogWarning(
                            "Unknown exception in WinUI CalendarView "
                            "day-layout validation.");
                        owner = state->GetOwner(generation);
                        if ( !owner || !owner->m_winui ||
                             owner->m_winui.get() != implementation ||
                             owner->m_winui->callbackState != state ||
                             owner->m_winui->cal != calendar )
                        {
                            return;
                        }
                        owner->m_winui->
                            RecoverWeekDayLayoutValidationFailure(
                                owner, validationGeneration,
                                requestRevision, retryGeneration);
                        return;
                    }

                    owner = state->GetOwner(generation);
                    if ( !owner || !owner->m_winui ||
                         owner->m_winui.get() != implementation ||
                         owner->m_winui->callbackState != state ||
                         owner->m_winui->cal != calendar )
                    {
                        return;
                    }

                    wxWinUICalendarImpl * const liveImpl =
                        owner->m_winui.get();
                    if ( liveImpl->weekDayLayoutValidationGeneration !=
                             validationGeneration ||
                         !liveImpl->weekDayLayoutValidationPending ||
                         liveImpl->weekDayLayoutValidationScheduled ||
                         liveImpl->refreshWeeksInProgress ||
                         liveImpl->weekRefreshScheduled ||
                         liveImpl->weekRefreshWaitingForMutationEnd ||
                         liveImpl->weekMutationProbeScheduled )
                    {
                        return;
                    }

                    // A non-throwing transient read failure has the same
                    // ownership contract as an exception: it must either
                    // retry within the existing finite budget or clear the
                    // pending ticket in degraded state.
                    liveImpl->RecoverWeekDayLayoutValidationFailure(
                        owner, validationGeneration,
                        requestRevision, retryGeneration);
                });
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException(
                "WinUI CalendarView day-layout validation queue", e);
        }
        catch ( ... )
        {
            wxLogWarning(
                "Unknown exception while queueing WinUI CalendarView "
                "day-layout validation.");
        }

        owner = state->GetOwner(generation);
        if ( !queued && owner && owner->m_winui &&
             owner->m_winui.get() == this &&
             owner->m_winui->callbackState == state &&
             owner->m_winui->cal == calendar &&
             weekDayLayoutValidationGeneration == validationGeneration )
        {
            weekDayLayoutValidationScheduled = false;
            owner->m_winui->RecoverWeekDayLayoutValidationFailure(
                owner, validationGeneration,
                validationRequestRevision,
                validationRetryGeneration);
            return;
        }
    }

    void RefreshWeekNumbersFromLayout(wxCalendarCtrl *owner)
    {
        // Writes to the week-number overlay can raise LayoutUpdated both
        // synchronously and after the current dispatcher callback. Neither
        // edge represents new CalendarView input and neither may create a
        // phantom refresh wave.
        if ( refreshWeeksInProgress )
            return;

        if ( weekRefreshWaitingForMutationEnd )
        {
            const auto state = callbackState;
            const std::uint64_t generation =
                state ? state->Generation() : 0;
            if ( !state || state->IsPeerMutationInProgress() ||
                 state->GetOwner(generation) != owner || !owner ||
                 !owner->m_winui || owner->m_winui.get() != this ||
                 owner->m_winui->callbackState != state )
            {
                return;
            }

            weekRefreshWaitingForMutationEnd = false;
            refreshWeeksPending = false;
            weekRefreshPendingCause = WeekRefreshPendingCause::None;
            weekMutationProbeScheduled = false;
            if ( ++weekMutationProbeGeneration == 0 )
                ++weekMutationProbeGeneration;
            ScheduleWeekRefresh(owner);
            return;
        }

        WeekLayoutInputs inputs;
        if ( !CaptureWeekLayoutInputs(owner, &inputs) )
            return;
        if ( projectedWeekLayoutInputsValid &&
             SameWeekLayoutInputs(inputs, projectedWeekLayoutInputs) &&
             !weekDayLayoutValidationPending )
        {
            return;
        }

        if ( projectedWeekLayoutInputsValid &&
             projectedWeekDayLayoutValid &&
             inputs.xamlRoot == projectedWeekLayoutInputs.xamlRoot )
        {
            // LayoutUpdated is also raised by the week overlay writes. The
            // lightweight root/revision tuple can change on that echo even
            // though every realized date and rectangle consumed by the week
            // projection is identical. Authenticate the real day peers before
            // deciding: only an identical signature is absorbed; any date or
            // geometry change remains an external layout request.
            WeekDayLayoutSignature signature;
            std::uint64_t signatureRevision = 0;
            if ( !CaptureWeekDayLayoutSignature(
                     owner, &signature, &signatureRevision) )
            {
                return;
            }

            WeekLayoutInputs currentInputs;
            if ( !CaptureWeekLayoutInputs(owner, &currentInputs) )
                return;
            if ( currentInputs.realizedDaysRevision == signatureRevision &&
                 currentInputs.xamlRoot ==
                     projectedWeekLayoutInputs.xamlRoot &&
                 SameWeekDayLayoutSignature(
                     signature, projectedWeekDayLayout) )
            {
                projectedWeekLayoutInputs = currentInputs;
                weekDayLayoutValidationPending = false;
                // A late validation can prove that an already-published
                // projection is still current after its own retry budget was
                // exhausted. Heal that validator-only degraded state without
                // manufacturing a request or rearming the finite budget. An
                // incomplete week refresh must remain degraded until a real
                // model/layout request completes it.
                if ( weekRefreshCompletedRevision ==
                         weekRefreshRequestRevision )
                {
                    weekRefreshDegraded = false;
                }
                ++weekLayoutEchoAbsorptionCount;
                return;
            }
        }

        weekDayLayoutValidationPending = false;
        RefreshWeekNumbers(owner);
    }

    void RefreshWeekNumbersImpl(wxCalendarCtrl *owner)
    {
        const auto state = callbackState;
        const std::uint64_t generation =
            state ? state->Generation() : 0;
        const MUXC::CalendarView calendar = cal;
        if ( !state || state->GetOwner(generation) != owner ||
             !owner || !owner->m_winui ||
             owner->m_winui.get() != this ||
             owner->m_winui->callbackState != state ||
             owner->m_winui->cal != calendar )
        {
            return;
        }

        // Consume the reason for this validated dispatcher wave before every
        // possible early exit. Only a new request made while this pass is in
        // progress may set it again and schedule a follow-up wave.
        refreshWeeksPending = false;
        weekRefreshPendingCause = WeekRefreshPendingCause::None;

        if ( weekRows.empty() || !weekCanvas )
            return;

        if ( !owner->HasFlag(wxCAL_SHOW_WEEK_NUMBERS) )
        {
            bool changed = false;
            for ( WeekRow& row : weekRows )
            {
                if ( !row.visible )
                    continue;

                row.surface.Visibility(MUX::Visibility::Collapsed);
                owner = state->GetOwner(generation);
                if ( !owner || !owner->m_winui ||
                     owner->m_winui.get() != this ||
                     owner->m_winui->callbackState != state ||
                     owner->m_winui->cal != calendar )
                {
                    return;
                }
                row.visible = false;
                row.startDate = wxDefaultDateTime;
                row.weekNumber = 0;
                changed = true;
            }
            if ( changed )
                NoteRealizedDaysChanged();
            owner = state->GetOwner(generation);
            if ( owner && owner->m_winui &&
                 owner->m_winui.get() == this &&
                 owner->m_winui->callbackState == state &&
                 owner->m_winui->cal == calendar )
            {
                RecordWeekLayoutProjection(owner);
            }
            return;
        }

#ifdef WXWINUI_TEST_SUPPORT
        std::unique_ptr<wxWinUICalendarPeerMutationGuard>
            injectedMutation;
        if ( weekRefreshPeerMutationPassesForTesting != 0 )
        {
            --weekRefreshPeerMutationPassesForTesting;
            injectedMutation =
                std::make_unique<wxWinUICalendarPeerMutationGuard>(state);
        }
#endif
        if ( state->IsPeerMutationInProgress() )
        {
            refreshWeeksPending = true;
            weekRefreshPendingCause =
                WeekRefreshPendingCause::PeerMutation;
            ++weekPeerMutationDeferralCount;
            return;
        }

        if ( refreshWeeksInProgress )
        {
            refreshWeeksPending = true;
            weekRefreshPendingCause = WeekRefreshPendingCause::Reentrant;
            return;
        }

        refreshWeeksInProgress = true;
        constexpr unsigned MaxRefreshPasses = 4;
        for ( unsigned pass = 0; pass != MaxRefreshPasses; ++pass )
        {
            refreshWeeksPending = false;
            weekRefreshPendingCause = WeekRefreshPendingCause::None;
            const HitTestTicket ticket = CaptureHitTestTicket();
#ifdef WXWINUI_TEST_SUPPORT
            if ( weekRefreshPendingPassesForTesting != 0 )
            {
                --weekRefreshPendingPassesForTesting;
                NoteRealizedDaysChanged();
            }
#endif
            PublicHitSnapshot snapshot;
            if ( !CapturePublicHitSnapshot(ticket, &snapshot) )
            {
                if ( !GetCurrentOwner(ticket) )
                {
                    owner = state->GetOwner(generation);
                    if ( owner && owner->m_winui &&
                         owner->m_winui.get() == this &&
                         owner->m_winui->callbackState == state )
                    {
                        refreshWeeksInProgress = false;
                        refreshWeeksPending = true;
                        weekRefreshPendingCause =
                            WeekRefreshPendingCause::InvalidTicket;
                        ++weekInvalidTicketDeferralCount;
                    }
                    return;
                }

                bool changed = false;
                for ( WeekRow& row : weekRows )
                {
                    if ( !row.visible )
                        continue;
                    row.surface.Visibility(MUX::Visibility::Collapsed);
                    owner = state->GetOwner(generation);
                    if ( !owner || !owner->m_winui ||
                         owner->m_winui.get() != this ||
                         owner->m_winui->callbackState != state ||
                         owner->m_winui->cal != calendar )
                    {
                        return;
                    }
                    row.visible = false;
                    row.startDate = wxDefaultDateTime;
                    row.weekNumber = 0;
                    changed = true;
                }
                if ( changed )
                    NoteRealizedDaysChanged();
            }
            else
            {
                ActiveMonthPage activePage;
                if ( !ResolveActiveMonthPage(
                         ticket, snapshot, &activePage) )
                {
                    if ( !GetCurrentOwner(ticket) )
                        return;
                    refreshWeeksInProgress = false;
                    refreshWeeksPending = true;
                    weekRefreshPendingCause =
                        WeekRefreshPendingCause::InvalidTicket;
                    ++weekInvalidTicketDeferralCount;
                    return;
                }

                const std::vector<const DayHitGeometry *>&
                    displayedMonthDays = activePage.currentDays;

                struct RowLayout
                {
                    double top = 0.0;
                    double bottom = 0.0;
                    double centre = 0.0;
                    double tolerance = 1.0;
                    wxDateTime startDate;
                };

                std::vector<const DayHitGeometry *> orderedDays;
                orderedDays = displayedMonthDays;
                std::sort(
                    orderedDays.begin(), orderedDays.end(),
                    [](const DayHitGeometry *lhs,
                       const DayHitGeometry *rhs)
                    {
                        const double lhsCentre = lhs->bounds.Y +
                            lhs->bounds.Height / 2.0;
                        const double rhsCentre = rhs->bounds.Y +
                            rhs->bounds.Height / 2.0;
                        if ( std::abs(lhsCentre - rhsCentre) > 0.5 )
                            return lhsCentre < rhsCentre;
                        return lhs->date < rhs->date;
                    });

                std::vector<RowLayout> layouts;
                for ( const DayHitGeometry *day : orderedDays )
                {
                    const double centre = day->bounds.Y +
                        day->bounds.Height / 2.0;
                    const double tolerance =
                        std::max(1.0,
                                 static_cast<double>(day->bounds.Height) /
                                     3.0);
                    RowLayout *layout = nullptr;
                    if ( !layouts.empty() &&
                         std::abs(layouts.back().centre - centre) <=
                             std::min(layouts.back().tolerance,
                                      tolerance) )
                    {
                        layout = &layouts.back();
                    }
                    else
                    {
                        layouts.push_back(RowLayout{});
                        layout = &layouts.back();
                        layout->top = day->bounds.Y;
                        layout->bottom = day->bounds.Y +
                            day->bounds.Height;
                        layout->centre = centre;
                        layout->tolerance = tolerance;
                        layout->startDate = day->date;
                    }

                    layout->top = std::min(
                        layout->top,
                        static_cast<double>(day->bounds.Y));
                    layout->bottom = std::max(
                        layout->bottom,
                        static_cast<double>(day->bounds.Y) +
                            day->bounds.Height);
                    if ( !layout->startDate.IsValid() ||
                         day->date < layout->startDate )
                    {
                        layout->startDate = day->date;
                    }
                }

                bool changed = false;
                const std::size_t visibleCount =
                    std::min(layouts.size(), weekRows.size());
                for ( std::size_t n = 0; n != weekRows.size(); ++n )
                {
                    WeekRow& row = weekRows[n];
                    if ( n >= visibleCount ||
                         !layouts[n].startDate.IsValid() )
                    {
                        if ( row.visible )
                        {
                            row.surface.Visibility(
                                MUX::Visibility::Collapsed);
                            owner = state->GetOwner(generation);
                            if ( !owner || !owner->m_winui ||
                                 owner->m_winui.get() != this ||
                                 owner->m_winui->callbackState != state ||
                                 owner->m_winui->cal != calendar )
                            {
                                return;
                            }
                            row.visible = false;
                            row.startDate = wxDefaultDateTime;
                            row.weekNumber = 0;
                            changed = true;
                        }
                        continue;
                    }

                    const RowLayout& layout = layouts[n];
                    const double height =
                        std::max(1.0, layout.bottom - layout.top);
                    const wxDateTime::WeekDay firstWeekday =
                        wxWinUIFromDayOfWeek(calendar.FirstDayOfWeek());
                    owner = state->GetOwner(generation);
                    if ( !owner || !owner->m_winui ||
                         owner->m_winui.get() != this ||
                         owner->m_winui->callbackState != state ||
                         owner->m_winui->cal != calendar )
                    {
                        return;
                    }
                    const int daysSinceWeekStart =
                        (static_cast<int>(layout.startDate.GetWeekDay()) -
                         static_cast<int>(firstWeekday) + 7) % 7;
                    const wxDateTime weekStart =
                        layout.startDate -
                            wxDateSpan::Days(daysSinceWeekStart);
                    const int weekNumber =
                        wxWinUICalendarWeekNumber(
                            calendarIdentifier,
                            weekStart, firstWeekday);
                    if ( weekNumber == 0 )
                    {
                        if ( row.visible )
                        {
                            row.surface.Visibility(
                                MUX::Visibility::Collapsed);
                            owner = state->GetOwner(generation);
                            if ( !owner || !owner->m_winui ||
                                 owner->m_winui.get() != this ||
                                 owner->m_winui->callbackState != state ||
                                 owner->m_winui->cal != calendar )
                            {
                                return;
                            }
                            row.visible = false;
                            row.startDate = wxDefaultDateTime;
                            row.weekNumber = 0;
                            changed = true;
                        }
                        continue;
                    }
                    const bool geometryChanged =
                        !row.visible ||
                        std::abs(row.top - layout.top) > 0.25 ||
                        std::abs(row.height - height) > 0.25;
                    const bool valueChanged =
                        !row.startDate.IsValid() ||
                        row.startDate != weekStart ||
                        row.weekNumber != weekNumber;

                    if ( geometryChanged )
                    {
                        MUXC::Canvas::SetTop(row.surface, layout.top);
                        owner = state->GetOwner(generation);
                        if ( !owner || !owner->m_winui ||
                             owner->m_winui.get() != this ||
                             owner->m_winui->callbackState != state ||
                             owner->m_winui->cal != calendar )
                        {
                            return;
                        }
                        row.surface.Height(height);
                        owner = state->GetOwner(generation);
                        if ( !owner || !owner->m_winui ||
                             owner->m_winui.get() != this ||
                             owner->m_winui->callbackState != state ||
                             owner->m_winui->cal != calendar )
                        {
                            return;
                        }
                        row.top = layout.top;
                        row.height = height;
                    }
                    if ( valueChanged )
                    {
                        row.label.Text(
                            weekNumberFormatter.FormatInt(weekNumber));
                        owner = state->GetOwner(generation);
                        if ( !owner || !owner->m_winui ||
                             owner->m_winui.get() != this ||
                             owner->m_winui->callbackState != state ||
                             owner->m_winui->cal != calendar )
                        {
                            return;
                        }
                        row.startDate = weekStart;
                        row.weekNumber = weekNumber;
                    }
                    if ( !row.visible )
                    {
                        row.surface.Visibility(MUX::Visibility::Visible);
                        owner = state->GetOwner(generation);
                        if ( !owner || !owner->m_winui ||
                             owner->m_winui.get() != this ||
                             owner->m_winui->callbackState != state ||
                             owner->m_winui->cal != calendar )
                        {
                            return;
                        }
                        row.visible = true;
                    }
                    changed = changed || geometryChanged || valueChanged;
                }

                if ( changed )
                    NoteRealizedDaysChanged();
            }

            owner = state->GetOwner(generation);
            if ( !owner || !owner->m_winui ||
                 owner->m_winui.get() != this ||
                 owner->m_winui->callbackState != state ||
                 owner->m_winui->cal != calendar )
            {
                return;
            }
            if ( !refreshWeeksPending )
            {
                RecordWeekLayoutProjection(owner);
                owner = state->GetOwner(generation);
                if ( !owner || !owner->m_winui ||
                     owner->m_winui.get() != this ||
                     owner->m_winui->callbackState != state ||
                     owner->m_winui->cal != calendar )
                {
                    return;
                }
                refreshWeeksInProgress = false;
                if ( navigationBindingRequired )
                {
                    // Completion of the real realized-day/overlay projection
                    // is another authoritative materialization edge. A
                    // bounded early retry may have observed an incomplete
                    // template, so let this completed wave reconcile it once.
                    RefreshNavigationButtons(owner);
                }
                return;
            }
        }

        refreshWeeksPending = false;
        weekRefreshPendingCause = WeekRefreshPendingCause::None;
        refreshWeeksInProgress = false;
        wxLogWarning(
            "WinUI CalendarView week-number layout did not converge; "
            "deferring further work to the next layout edge.");
    }

    MUXC::CalendarViewDayItem FindRealizedDay(
        const wxDateTime& date)
    {
        const int key = wxWinUICivilDateKey(date);
        const auto known = realizedDays.find(key);
        if ( known != realizedDays.end() )
        {
            if ( const MUXC::CalendarViewDayItem item =
                     known->second.get() )
            {
                return item;
            }
            realizedDays.erase(known);
            NoteRealizedDaysChanged();
        }

        if ( !cal )
            return nullptr;

        std::vector<MUX::DependencyObject> pending;
        pending.push_back(cal);
        while ( !pending.empty() )
        {
            const MUX::DependencyObject current = pending.back();
            pending.pop_back();

            if ( const auto item =
                     current.try_as<MUXC::CalendarViewDayItem>() )
            {
                const wxDateTime itemDate =
                    wxWinUIFromDateTime(item.Date());
                const int itemKey = wxWinUICivilDateKey(itemDate);
                if ( itemKey != 0 )
                {
                    realizedDays[itemKey] = winrt::make_weak(item);
                    NoteRealizedDaysChanged();
                }
                if ( itemKey == key )
                    return item;
            }

            const int count = MUXM::VisualTreeHelper::GetChildrenCount(
                current);
            for ( int i = 0; i < count; ++i )
            {
                pending.push_back(
                    MUXM::VisualTreeHelper::GetChild(current, i));
            }
        }

        return nullptr;
    }

    static bool ResolveDayAtPoint(
        const HitTestTicket& ticket,
        const WF::Point& point,
        wxDateTime *date)
    {
        if ( !GetCurrentOwner(ticket) )
            return false;

        // The mapper supplies CalendarView-local DIPs. Resolve against the
        // realized day bounds in that same coordinate space, and revalidate
        // the wx owner, peer and realized-day model after every WinRT call.
        const auto containsPoint =
            [&ticket, &point, date](
                const MUXC::CalendarViewDayItem& item)
            {
                if ( !item )
                    return false;

                const MUX::Visibility visibility =
                    item.Visibility();
                if ( !GetCurrentOwner(ticket) )
                    return false;
                if ( visibility != MUX::Visibility::Visible )
                    return false;

                const bool isHitTestVisible =
                    item.IsHitTestVisible();
                if ( !GetCurrentOwner(ticket) )
                    return false;
                if ( !isHitTestVisible )
                    return false;

                const double width = item.ActualWidth();
                if ( !GetCurrentOwner(ticket) )
                    return false;
                const double height = item.ActualHeight();
                if ( !GetCurrentOwner(ticket) )
                    return false;
                if ( width <= 0.0 || height <= 0.0 )
                {
                    return false;
                }

                const MUXM::GeneralTransform toCalendar =
                    item.TransformToVisual(ticket.calendar);
                if ( !GetCurrentOwner(ticket) )
                    return false;
                const WF::Rect bounds = toCalendar.TransformBounds(
                    WF::Rect{
                        0.0f,
                        0.0f,
                        static_cast<float>(width),
                        static_cast<float>(height)
                    });
                if ( !GetCurrentOwner(ticket) )
                    return false;
                if ( point.X < bounds.X || point.Y < bounds.Y ||
                     point.X >= bounds.X + bounds.Width ||
                     point.Y >= bounds.Y + bounds.Height )
                {
                    return false;
                }

                const WF::DateTime itemDate = item.Date();
                if ( !GetCurrentOwner(ticket) )
                    return false;
                const wxDateTime resolved =
                    wxWinUIFromDateTime(itemDate);
                if ( !resolved.IsValid() )
                    return false;
                if ( date )
                    *date = resolved;
                return true;
            };

        for ( const RealizedDayTicket& realized : ticket.realizedDays )
        {
            const MUXC::CalendarViewDayItem item = realized.item.get();
            if ( !GetCurrentOwner(ticket) )
                return false;
            if ( containsPoint(item) )
                return true;
            if ( !GetCurrentOwner(ticket) )
                return false;
        }

        // CalendarViewDayItemChanging normally populates realizedDays, but an
        // initial synchronous hit test can precede that notification. Fall
        // back to a bounded walk of the already-realized visual tree.
        std::vector<MUX::DependencyObject> pending;
        pending.push_back(ticket.calendar);
        while ( !pending.empty() )
        {
            const MUX::DependencyObject current = pending.back();
            pending.pop_back();
            const MUXC::CalendarViewDayItem item =
                current.try_as<MUXC::CalendarViewDayItem>();
            if ( !GetCurrentOwner(ticket) )
                return false;
            if ( item )
            {
                if ( containsPoint(item) )
                    return true;
                if ( !GetCurrentOwner(ticket) )
                    return false;
            }

            const int count =
                MUXM::VisualTreeHelper::GetChildrenCount(current);
            if ( !GetCurrentOwner(ticket) )
                return false;
            for ( int n = 0; n != count; ++n )
            {
                const MUX::DependencyObject child =
                    MUXM::VisualTreeHelper::GetChild(current, n);
                if ( !GetCurrentOwner(ticket) )
                    return false;
                pending.push_back(child);
            }
        }

        return false;
    }

    bool HandleDoubleTapAt(const WF::Point& point)
    {
        const HitTestTicket ticket = CaptureHitTestTicket();
        if ( !GetCurrentOwner(ticket) )
            return false;

        if ( !ResolveDayAtPoint(ticket, point, nullptr) )
            return false;

        wxCalendarCtrl * const owner = GetCurrentOwner(ticket);
        if ( !owner )
            return false;

        // Match wxMSW: hit testing only qualifies the target as a day. The
        // wx calendar event itself carries the control's selected date.
        const wxDateTime selectedDate = owner->GetDate();
        wxWinUIDispatchCalendarEvent(
            ticket.callbackState, ticket.callbackGeneration,
            selectedDate,
            wxEVT_CALENDAR_DOUBLECLICKED);
        // The event may have destroyed the owner and this implementation. Do
        // not inspect either after dispatch.
        return true;
    }

    wxWinUIControlHost host;
    std::shared_ptr<wxWinUICalendarCallbackState> callbackState;
    MUXC::Grid root{ nullptr };
    MUX::FrameworkElement layoutRoot{ nullptr };
    MUX::XamlRoot layoutXamlRoot{ nullptr };
    MUXC::Canvas weekCanvas{ nullptr };
    MUXC::ColumnDefinition weekColumn{ nullptr };
    WGNF::DecimalFormatter weekNumberFormatter{ nullptr };
    MUXC::CalendarView cal{ nullptr };
    winrt::event_token selectionToken{};
    winrt::event_token doubleTappedToken{};
    winrt::event_token dayItemChangingToken{};
    winrt::event_token actualThemeChangedToken{};
    winrt::event_token layoutUpdatedToken{};
    winrt::event_token layoutLoadedToken{};
    MUXCP::ButtonBase decrementButton{ nullptr };
    MUXCP::ButtonBase incrementButton{ nullptr };
    winrt::event_token decrementClickToken{};
    winrt::event_token incrementClickToken{};
    WF::DateTime defaultMinimum{};
    WF::DateTime defaultMaximum{};
    WF::DateTime civilMinimum{};
    WF::DateTime civilMaximum{};
    winrt::hstring calendarIdentifier;
    WG::DayOfWeek defaultFirstDayOfWeek = WG::DayOfWeek::Sunday;
    bool defaultsCaptured = false;
    std::map<int, RealizedDay> realizedDays;
    std::map<int, std::vector<WU::Color>> appliedDensityColours;
    std::map<int, unsigned long long>
        appliedDensityColourRevisions;
    unsigned long long densityColourWriteRevision = 0;
    std::set<int> automaticHolidayKeys;
    std::uint64_t holidayRefreshRevision = 0;
    bool refreshHolidaysInProgress = false;
    bool refreshHolidaysPending = false;
    bool refreshMarksInProgress = false;
    bool refreshMarksPending = false;
    bool refreshWeeksInProgress = false;
    bool refreshWeeksPending = false;
    bool weekRefreshScheduled = false;
    bool projectedWeekLayoutInputsValid = false;
    bool projectedWeekDayLayoutValid = false;
    bool weekDayLayoutValidationPending = false;
    bool weekDayLayoutValidationScheduled = false;
    bool navigationRefreshScheduled = false;
    bool navigationBindingRequired = true;
    bool initialSizePending = true;
    bool initialSizeProjectionComplete = false;
    bool suppressNextMinSizeWriter = false;
    bool suppressNextSizeWriter = false;
    std::vector<WeekRow> weekRows;
    std::uint64_t realizedDaysRevision = 1;
    std::uint64_t weekRefreshRequestRevision = 1;
    std::uint64_t weekRefreshCompletedRevision = 0;
    std::uint64_t weekRefreshScheduleGeneration = 1;
    std::uint64_t weekRefreshRetryGeneration = 1;
    unsigned weekRefreshRetriesRemaining = 0;
#ifdef WXWINUI_TEST_SUPPORT
    unsigned weekRefreshFaultsForTesting = 0;
    unsigned weekRefreshPendingPassesForTesting = 0;
    unsigned weekRefreshPeerMutationPassesForTesting = 0;
    unsigned weekDayLayoutValidationFaultsForTesting = 0;
    unsigned weekDayLayoutValidationFailureModeForTesting = 0;
#endif
    bool weekRefreshDegraded = false;
    bool weekRefreshWaitingForMutationEnd = false;
    bool weekMutationProbeScheduled = false;
    WeekRefreshPendingCause weekRefreshPendingCause =
        WeekRefreshPendingCause::None;
    std::uint64_t weekMutationProbeGeneration = 1;
    std::uint64_t weekPeerMutationDeferralCount = 0;
    std::uint64_t boundedWeekPendingRetryCount = 0;
    std::uint64_t weekInvalidTicketDeferralCount = 0;
    std::uint64_t dayItemIdentityChangeCount = 0;
    std::uint64_t dayItemRepeatNotificationCount = 0;
    std::uint64_t weekLayoutEchoAbsorptionCount = 0;
    std::uint64_t weekDayLayoutValidationFailureRecoveryCount = 0;
    std::uint64_t weekDayLayoutValidationGeneration = 1;
    std::uint64_t navigationButtonGeneration = 1;
    std::uint64_t navigationRefreshScheduleGeneration = 1;
    std::uint64_t navigationResolveFailureCount = 0;
    std::uint64_t navigationRetryExhaustionCount = 0;
    unsigned navigationSnapshotButtonCount = 0;
    unsigned navigationCandidateCount = 0;
    NavigationRefreshFailure navigationLastFailure =
        NavigationRefreshFailure::None;
    unsigned long long weekRefreshRunCount = 0;
    WeekLayoutInputs projectedWeekLayoutInputs;
    WeekDayLayoutSignature projectedWeekDayLayout;
    unsigned projectedWeekMonthDayCount = 0;
    unsigned projectedWeekColumnCount = 0;
    std::uint64_t sizeWriterRevision = 1;
    std::uint64_t initialSizeWriterRevision = 0;
    std::uint64_t layoutRevision = 1;
    wxSize initialSizeRequest = wxDefaultSize;
    wxSize initialMinSize = wxDefaultSize;
};

namespace
{

wxWinUICalendarPeerMutationGuard::~wxWinUICalendarPeerMutationGuard()
{
    if ( !m_state )
        return;

    const std::uint64_t generation = m_state->Generation();
    if ( !m_state->EndPeerMutation() )
        return;

    wxWinUICalendarImpl::NotifyPeerMutationEnded(m_state, generation);
}

} // namespace

wxCalendarCtrl::wxCalendarCtrl()
{
}

#ifdef WXWINUI_TEST_SUPPORT
wxString wxWinUICalendarTestAccess::SetLanguage(const wxString& language)
{
    const wxString previous =
        wxWinUIFromHString(gs_winuiCalendarLanguageForTesting);
    gs_winuiCalendarLanguageForTesting = wxWinUIToHString(language);
    return previous;
}
#endif

wxCalendarCtrl::wxCalendarCtrl(wxWindow *parent, wxWindowID id,
                               const wxDateTime& date,
                               const wxPoint& pos,
                               const wxSize& size, long style,
                               const wxString& name)
{
    Create(parent, id, date, pos, size, style, name);
}

wxCalendarCtrl::~wxCalendarCtrl()
{
    if ( m_winui )
        m_winui->Close();
}

bool wxCalendarCtrl::Create(wxWindow *parent, wxWindowID id,
                            const wxDateTime& date,
                            const wxPoint& pos,
                            const wxSize& size, long style,
                            const wxString& name)
{
    style = (style & ~wxBORDER_MASK) | wxBORDER_NONE;

    if ( !wxControl::Create(parent, id, pos, size, style,
                            wxDefaultValidator, name) )
    {
        return false;
    }

    m_date = wxWinUINormalizeCivilDate(
        date.IsValid() ? date : wxDateTime::Today());
    if ( !m_date.IsValid() )
        return false;
    m_lowerDate = wxDefaultDateTime;
    m_upperDate = wxDefaultDateTime;
    m_marks = 0;
    m_holidays = 0;
    Unbind(wxEVT_SYS_COLOUR_CHANGED,
           &wxCalendarCtrl::OnSysColourChanged, this);
    Bind(wxEVT_SYS_COLOUR_CHANGED,
         &wxCalendarCtrl::OnSysColourChanged, this);

    m_winui.reset(new wxWinUICalendarImpl);
    m_winui->callbackState =
        std::make_shared<wxWinUICalendarCallbackState>(this);
    wxWinUICalendarImpl * const createImpl = m_winui.get();
    const std::shared_ptr<wxWinUICalendarCallbackState> createState =
        m_winui->callbackState;
    const std::uint64_t createGeneration =
        createState->Generation();
    if ( !m_winui->host.Initialize(this) )
    {
        m_winui->Close();
        return false;
    }

    MUXC::CalendarView createPeer{ nullptr };
    try
    {
        m_winui->cal = MUXC::CalendarView();
        createPeer = m_winui->cal;
        winrt::hstring language;
#ifdef WXWINUI_TEST_SUPPORT
        language = gs_winuiCalendarLanguageForTesting;
#endif
        if ( !language.empty() )
        {
            m_winui->cal.Language(language);
            const WG::Calendar localeCalendar(
                std::vector<winrt::hstring>{language});
            m_winui->cal.CalendarIdentifier(
                localeCalendar.GetCalendarSystem());
        }
        m_winui->cal.SelectionMode(
            MUXC::CalendarViewSelectionMode::Single);
        m_winui->cal.IsOutOfScopeEnabled(
            (style & wxCAL_SHOW_SURROUNDING_WEEKS) != 0);
        m_winui->defaultMinimum = m_winui->cal.MinDate();
        m_winui->defaultMaximum = m_winui->cal.MaxDate();
        m_winui->calendarIdentifier =
            m_winui->cal.CalendarIdentifier();
        m_winui->defaultFirstDayOfWeek =
            m_winui->cal.FirstDayOfWeek();
        if ( !wxWinUIResolveCalendarEnvelope(
                 m_winui->calendarIdentifier,
                 &m_winui->civilMinimum,
                 &m_winui->civilMaximum) )
        {
            m_winui->Close();
            return false;
        }
        m_winui->defaultsCaptured = true;

        if ( style & wxCAL_MONDAY_FIRST )
        {
            m_winui->cal.FirstDayOfWeek(
                winrt::Windows::Globalization::DayOfWeek::Monday);
        }
        else if ( style & wxCAL_SUNDAY_FIRST )
        {
            m_winui->cal.FirstDayOfWeek(
                winrt::Windows::Globalization::DayOfWeek::Sunday);
        }
        // Otherwise keep the locale default.

        // Always keep the real week-number composite available so changing
        // wxCAL_SHOW_WEEK_NUMBERS after creation can update the peer just as
        // wxMSW updates MCS_WEEKNUMBERS. Its column has zero width and the
        // canvas is collapsed while the style is disabled.
        m_winui->root = MUXC::Grid();
        m_winui->weekCanvas = MUXC::Canvas();
        m_winui->weekNumberFormatter =
            wxWinUICreateCalendarWeekNumberFormatter(language);
        if ( !language.empty() )
        {
            m_winui->root.Language(language);
            m_winui->weekCanvas.Language(language);
        }
        m_winui->weekColumn = MUXC::ColumnDefinition();
        const bool showWeekNumbers =
            (style & wxCAL_SHOW_WEEK_NUMBERS) != 0;
        m_winui->weekColumn.Width(
            MUX::GridLengthHelper::FromPixels(
                showWeekNumbers ? 36 : 0));
        m_winui->weekCanvas.Visibility(
            showWeekNumbers
                ? MUX::Visibility::Visible
                : MUX::Visibility::Collapsed);

        MUXC::ColumnDefinition calendarColumn;
        calendarColumn.Width(
            MUX::GridLengthHelper::FromValueAndType(
                1, MUX::GridUnitType::Star));
        m_winui->root.ColumnDefinitions().Append(
            m_winui->weekColumn);
        m_winui->root.ColumnDefinitions().Append(calendarColumn);
        MUXC::Grid::SetColumn(m_winui->cal, 1);

        m_winui->weekRows.reserve(6);
        for ( unsigned n = 0; n != 6; ++n )
        {
            wxWinUICalendarImpl::WeekRow row;
            row.surface = MUXC::Border();
            row.label = MUXC::TextBlock();
            if ( !language.empty() )
            {
                row.surface.Language(language);
                row.label.Language(language);
            }
            row.surface.Width(36);
            row.surface.Visibility(MUX::Visibility::Collapsed);
            row.label.HorizontalAlignment(
                MUX::HorizontalAlignment::Right);
            row.label.VerticalAlignment(
                MUX::VerticalAlignment::Center);
            row.label.Margin(MUX::Thickness{2, 0, 6, 0});
            row.surface.Child(row.label);
            MUXC::Canvas::SetLeft(row.surface, 0);
            m_winui->weekCanvas.Children().Append(row.surface);
            m_winui->weekRows.push_back(std::move(row));
        }

        m_winui->root.Children().Append(m_winui->weekCanvas);
        m_winui->root.Children().Append(m_winui->cal);

        const std::shared_ptr<wxWinUICalendarCallbackState> callbackState =
            m_winui->callbackState;
        const std::uint64_t generation =
            callbackState->Generation();

        m_winui->actualThemeChangedToken =
            m_winui->cal.ActualThemeChanged(
                [callbackState, generation](
                    MUX::FrameworkElement const& sender,
                    WF::IInspectable const&)
                {
                    wxCalendarCtrl * const owner =
                        callbackState->GetOwner(generation);
                    const MUXC::CalendarView senderCalendar =
                        sender.try_as<MUXC::CalendarView>();
                    if ( !owner || !owner->m_winui ||
                         owner->m_winui->callbackState != callbackState ||
                         owner->m_winui->cal != senderCalendar )
                    {
                        return;
                    }

                    owner->HandleVisualThemeChanged();
                });

        m_winui->selectionToken =
            m_winui->cal.SelectedDatesChanged(
                [callbackState, generation](
                    MUXC::CalendarView const&,
                    MUXC::CalendarViewSelectedDatesChangedEventArgs const&)
                {
                    wxCalendarCtrl * const owner =
                        callbackState->GetOwner(generation);
                    if ( !owner || !owner->m_winui ||
                         owner->m_winui->callbackState != callbackState ||
                         callbackState->IsPeerMutationInProgress() )
                    {
                        return;
                    }

                    owner->OnPeerSelectionChanged();
                });

        m_winui->doubleTappedToken =
            m_winui->cal.DoubleTapped(
                [callbackState, generation](
                    WF::IInspectable const&,
                    MUX::Input::DoubleTappedRoutedEventArgs const& args)
                {
                    wxCalendarCtrl *owner =
                        callbackState->GetOwner(generation);
                    if ( !owner || !owner->m_winui ||
                         owner->m_winui->callbackState != callbackState ||
                         callbackState->IsPeerMutationInProgress() )
                    {
                        return;
                    }

                    try
                    {
                        wxWinUICalendarImpl * const callbackImpl =
                            owner->m_winui.get();
                        const MUXC::CalendarView calendar =
                            callbackImpl->cal;
                        if ( !calendar )
                            return;

                        const WF::Point point =
                            args.GetPosition(calendar);

                        // GetPosition() is a WinRT/re-entrancy boundary. Only
                        // the exact callback generation, implementation and
                        // CalendarView peer may consume its result.
                        owner = callbackState->GetOwner(generation);
                        if ( !owner || !owner->m_winui ||
                             owner->m_winui.get() != callbackImpl ||
                             owner->m_winui->callbackState != callbackState ||
                             callbackState->IsPeerMutationInProgress() ||
                             winrt::get_abi(owner->m_winui->cal) !=
                                 winrt::get_abi(calendar) )
                        {
                            return;
                        }

                        callbackImpl->HandleDoubleTapAt(point);
                    }
                    catch ( const winrt::hresult_error& e )
                    {
                        wxWinUILogException(
                            "WinUI CalendarView double-tap hit test", e);
                    }
                });

        m_winui->dayItemChangingToken =
            m_winui->cal.CalendarViewDayItemChanging(
                [callbackState, generation](
                    MUXC::CalendarView const&,
                    MUXC::CalendarViewDayItemChangingEventArgs const& args)
                {
                    wxCalendarCtrl * const owner =
                        callbackState->GetOwner(generation);
                    if ( !owner || !owner->m_winui ||
                         owner->m_winui->callbackState != callbackState )
                    {
                        return;
                    }

                    try
                    {
                        owner->m_winui->OnDayItemChanging(owner, args);
                    }
                    catch ( const winrt::hresult_error& e )
                    {
                        wxWinUILogException(
                            "WinUI CalendarView day item update", e);
                    }
                });

        m_winui->layoutUpdatedToken =
            m_winui->cal.LayoutUpdated(
                    [callbackState, generation](
                        WF::IInspectable const&,
                        WF::IInspectable const&)
                    {
                        wxCalendarCtrl *owner =
                            callbackState->GetOwner(generation);
                        if ( !owner || !owner->m_winui ||
                             owner->m_winui->callbackState !=
                                 callbackState ||
                             callbackState->IsPeerMutationInProgress() )
                        {
                            return;
                        }

                        try
                        {
                            owner->m_winui->RefreshNavigationButtons(owner);
                            owner = callbackState->GetOwner(generation);
                            if ( !owner || !owner->m_winui ||
                                 owner->m_winui->callbackState !=
                                     callbackState ||
                                 callbackState->IsPeerMutationInProgress() )
                            {
                                return;
                            }
                            owner->m_winui->RefreshWeekNumbersFromLayout(owner);
                        }
                        catch ( const winrt::hresult_error& e )
                        {
                            wxWinUILogException(
                                "WinUI CalendarView week-number layout", e);
                        }
                    });

        const bool peerApplied = ApplyToPeer();
        wxCalendarCtrl *liveOwner =
            createState->GetOwner(createGeneration);
        if ( !liveOwner || !liveOwner->m_winui ||
             liveOwner->m_winui.get() != createImpl ||
             liveOwner->m_winui->callbackState != createState ||
             liveOwner->m_winui->cal != createPeer )
        {
            return false;
        }
        if ( !peerApplied )
        {
            liveOwner->m_winui->Close();
            return false;
        }

        const MUXC::Grid createRoot = createImpl->root;
        const MUX::UIElement content =
            createRoot
                ? createRoot.as<MUX::UIElement>()
                : createPeer.as<MUX::UIElement>();
        const MUX::FrameworkElement layoutRoot =
            content.as<MUX::FrameworkElement>();
        createImpl->layoutRoot = layoutRoot;
        createImpl->initialSizeRequest = size;
        createImpl->initialSizePending = true;
        createImpl->layoutLoadedToken = layoutRoot.Loaded(
            [createState, createGeneration, createImpl, layoutRoot](
                WF::IInspectable const&,
                MUX::RoutedEventArgs const&)
            {
                wxCalendarCtrl * const owner =
                    createState->GetOwner(createGeneration);
                if ( !owner || !owner->m_winui ||
                     owner->m_winui.get() != createImpl ||
                     owner->m_winui->callbackState != createState ||
                     owner->m_winui->layoutRoot != layoutRoot )
                {
                    return;
                }
                owner->OnLayoutLoaded();
            });
        const bool contentSet =
            createImpl->host.SetContent(content, createPeer);
        liveOwner = createState->GetOwner(createGeneration);
        if ( !liveOwner || !liveOwner->m_winui ||
             liveOwner->m_winui.get() != createImpl ||
             liveOwner->m_winui->callbackState != createState ||
             liveOwner->m_winui->cal != createPeer )
        {
            return false;
        }
        if ( !contentSet )
        {
            liveOwner->m_winui->Close();
            return false;
        }

        liveOwner->m_winui->host.SetPreferredFocus(createPeer);
        liveOwner = createState->GetOwner(createGeneration);
        if ( !liveOwner || !liveOwner->m_winui ||
             liveOwner->m_winui.get() != createImpl ||
             liveOwner->m_winui->callbackState != createState ||
             liveOwner->m_winui->cal != createPeer )
        {
            return false;
        }

        // ForceRender() enters the shared host's synchronous flush and may
        // therefore run arbitrary wx callbacks. Reacquire both owner and the
        // exact implementation before refreshing realized day items.
        liveOwner->m_winui->host.ForceRender();
        liveOwner = createState->GetOwner(createGeneration);
        if ( !liveOwner || !liveOwner->m_winui ||
             liveOwner->m_winui.get() != createImpl ||
             liveOwner->m_winui->callbackState != createState ||
             liveOwner->m_winui->cal != createPeer )
        {
            return false;
        }

        liveOwner->m_winui->RefreshNavigationButtons(liveOwner);
        liveOwner = createState->GetOwner(createGeneration);
        if ( !liveOwner || !liveOwner->m_winui ||
             liveOwner->m_winui.get() != createImpl ||
             liveOwner->m_winui->callbackState != createState ||
             liveOwner->m_winui->cal != createPeer )
        {
            return false;
        }

        liveOwner->m_winui->RefreshHolidayDates(liveOwner);
        liveOwner = createState->GetOwner(createGeneration);
        if ( !liveOwner || !liveOwner->m_winui ||
             liveOwner->m_winui.get() != createImpl ||
             liveOwner->m_winui->callbackState != createState ||
             liveOwner->m_winui->cal != createPeer )
        {
            return false;
        }

        liveOwner->m_winui->RefreshWeekNumbers(liveOwner);
        liveOwner = createState->GetOwner(createGeneration);
        if ( !liveOwner || !liveOwner->m_winui ||
             liveOwner->m_winui.get() != createImpl ||
             liveOwner->m_winui->callbackState != createState ||
             liveOwner->m_winui->cal != createPeer )
        {
            return false;
        }

        // wxEVT_CALENDAR_PAGE_CHANGED describes a change of the selected
        // month/year, not an otherwise independent change of the visible
        // CalendarView page (see wxCalendarCtrlBase::GenerateAllChangeEvents).
        // CalendarView has no DisplayDateChanged event, but none is required
        // for this contract: selected-month changes are handled below in the
        // standard wx event order.
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI CalendarView creation", e);
        wxCalendarCtrl * const liveOwner =
            createState->GetOwner(createGeneration);
        if ( liveOwner && liveOwner->m_winui &&
             liveOwner->m_winui.get() == createImpl &&
             liveOwner->m_winui->callbackState == createState )
        {
            liveOwner->m_winui->Close();
        }
        return false;
    }

    wxCalendarCtrl *liveOwner =
        createState->GetOwner(createGeneration);
    if ( !liveOwner || !liveOwner->m_winui ||
         liveOwner->m_winui.get() != createImpl ||
         liveOwner->m_winui->callbackState != createState ||
         liveOwner->m_winui->cal != createPeer )
    {
        return false;
    }

    // CalendarView and the optional week-number column are not available when
    // the base Create() path first asks for a best size.
    liveOwner->InvalidateBestSize();
    const std::uint64_t initialSizeRevision =
        liveOwner->m_winui->sizeWriterRevision;
    if ( !liveOwner->ApplyInitialSizeTransaction(
             size, initialSizeRevision) )
    {
        return false;
    }
    liveOwner = createState->GetOwner(createGeneration);
    if ( !liveOwner || !liveOwner->m_winui ||
         liveOwner->m_winui.get() != createImpl ||
         liveOwner->m_winui->callbackState != createState ||
         liveOwner->m_winui->cal != createPeer )
    {
        return false;
    }
    createImpl->initialSizeProjectionComplete = true;
    createImpl->initialSizeWriterRevision = initialSizeRevision;
    createImpl->initialMinSize = liveOwner->GetMinSize();
    if ( createImpl->sizeWriterRevision != initialSizeRevision ||
         createImpl->layoutXamlRoot )
    {
        createImpl->initialSizePending = false;
    }
    return true;
}

bool wxCalendarCtrl::SetDate(const wxDateTime& date)
{
    wxCHECK_MSG( date.IsValid(), false, "invalid date" );

    const wxDateTime value = wxWinUINormalizeCivilDate(date);
    if ( !wxWinUIIsInsideRange(
             value, m_lowerDate, m_upperDate) )
    {
        return false;
    }

    if ( !AllowMonthChange() &&
         (!m_winui ||
          !wxWinUISameCalendarMonth(
              m_winui->calendarIdentifier, value, m_date)) )
    {
        return false;
    }
    if ( !m_winui || !m_winui->callbackState )
        return false;

    const wxDateTime previousDate = m_date;
    const auto state = m_winui->callbackState;
    const std::uint64_t generation = state->Generation();
    wxWinUICalendarImpl * const updateImpl = m_winui.get();
    const std::uint64_t revision = state->BeginStateMutation();
    const winrt::hstring calendarIdentifier = m_winui->calendarIdentifier;
    if ( !ApplyStateToPeer(
             value, m_lowerDate, m_upperDate,
             AllowMonthChange()) )
    {
        return false;
    }

    wxCalendarCtrl *owner = state->GetOwner(generation);
    if ( !owner || !owner->m_winui ||
         owner->m_winui.get() != updateImpl ||
         owner->m_winui->callbackState != state ||
         !state->IsStateMutationCurrent(revision) )
    {
        return false;
    }
    owner->m_date = value;
    if ( !wxWinUISameCalendarMonth(
             calendarIdentifier, previousDate, value) )
    {
        owner->m_holidays = 0;
        owner->m_winui->RefreshHolidayDates(owner);
    }
    else
        owner->m_winui->RefreshMarks(owner);

    owner = state->GetOwner(generation);
    if ( !owner || !owner->m_winui ||
         owner->m_winui.get() != updateImpl ||
         owner->m_winui->callbackState != state ||
         !state->IsStateMutationCurrent(revision) )
    {
        return false;
    }
    owner->m_winui->RefreshWeekNumbers(owner);
    owner = state->GetOwner(generation);
    return owner && owner->m_winui &&
           owner->m_winui.get() == updateImpl &&
           owner->m_winui->callbackState == state &&
           state->IsStateMutationCurrent(revision);
}

wxDateTime wxCalendarCtrl::GetDate() const
{
    return m_date;
}

bool wxCalendarCtrl::SetDateRange(const wxDateTime& lowerdate,
                                  const wxDateTime& upperdate)
{
    const wxDateTime minimum =
        wxWinUINormalizeCivilDate(lowerdate);
    const wxDateTime maximum =
        wxWinUINormalizeCivilDate(upperdate);
    if ( minimum.IsValid() && maximum.IsValid() &&
         minimum > maximum )
    {
        return false;
    }

    if ( !m_winui || !m_winui->defaultsCaptured )
        return false;
    const wxDateTime civilMinimum =
        wxWinUIFromDateTime(m_winui->civilMinimum);
    const wxDateTime civilMaximum =
        wxWinUIFromDateTime(m_winui->civilMaximum);
    if ( (minimum.IsValid() && minimum < civilMinimum) ||
         (maximum.IsValid() && maximum > civilMaximum) )
    {
        return false;
    }

    wxDateTime value = m_date;
    if ( minimum.IsValid() && value < minimum )
        value = minimum;
    if ( maximum.IsValid() && value > maximum )
        value = maximum;

    // With month changes disabled, a range that excludes the locked month
    // cannot be represented without silently navigating to another month.
    if ( !AllowMonthChange() &&
         (!m_winui ||
          !wxWinUISameCalendarMonth(
              m_winui->calendarIdentifier, value, m_date)) )
    {
        return false;
    }

    const wxDateTime previousDate = m_date;
    const auto state = m_winui->callbackState;
    const std::uint64_t generation = state->Generation();
    wxWinUICalendarImpl * const updateImpl = m_winui.get();
    const std::uint64_t revision = state->BeginStateMutation();
    const winrt::hstring calendarIdentifier =
        m_winui->calendarIdentifier;
    if ( !ApplyStateToPeer(
             value, minimum, maximum,
             AllowMonthChange()) )
    {
        return false;
    }

    wxCalendarCtrl *owner = state->GetOwner(generation);
    if ( !owner || !owner->m_winui ||
         owner->m_winui.get() != updateImpl ||
         owner->m_winui->callbackState != state ||
         !state->IsStateMutationCurrent(revision) )
    {
        return false;
    }
    owner->m_lowerDate = minimum;
    owner->m_upperDate = maximum;
    owner->m_date = value;
    if ( !wxWinUISameCalendarMonth(
             calendarIdentifier, previousDate, value) )
    {
        owner->m_holidays = 0;
        owner->m_winui->RefreshHolidayDates(owner);
    }
    else
        owner->m_winui->RefreshMarks(owner);
    owner = state->GetOwner(generation);
    if ( !owner || !owner->m_winui ||
         owner->m_winui.get() != updateImpl ||
         owner->m_winui->callbackState != state ||
         !state->IsStateMutationCurrent(revision) )
    {
        return false;
    }
    owner->m_winui->RefreshWeekNumbers(owner);

    // This API uses false both for a rejected update and for the documented
    // successful operation that removes all restrictions.
    owner = state->GetOwner(generation);
    return owner && owner->m_winui &&
           owner->m_winui.get() == updateImpl &&
           owner->m_winui->callbackState == state &&
           state->IsStateMutationCurrent(revision) &&
           (minimum.IsValid() || maximum.IsValid());
}

bool wxCalendarCtrl::GetDateRange(wxDateTime *lowerdate,
                                  wxDateTime *upperdate) const
{
    if ( lowerdate )
        *lowerdate = m_lowerDate;
    if ( upperdate )
        *upperdate = m_upperDate;
    return m_lowerDate.IsValid() || m_upperDate.IsValid();
}

wxCalendarHitTestResult
wxCalendarCtrl::HitTest(const wxPoint& pos,
                        wxDateTime *date,
                        wxDateTime::WeekDay *wd)
{
    if ( !m_winui || !m_winui->cal )
        return wxCAL_HITTEST_NOWHERE;

    try
    {
        const wxWinUICalendarImpl::HitTestTicket ticket =
            m_winui->CaptureHitTestTicket();
        if ( !wxWinUICalendarImpl::GetCurrentOwner(ticket) )
            return wxCAL_HITTEST_NOWHERE;

        WF::Point point{};
        const wxWinUICoordinateResult mapped =
            wxWinUIVisualCoordinates::ClientPointToElement(
                this,
                WF::Point{
                    static_cast<float>(pos.x),
                    static_cast<float>(pos.y)
                },
                ticket.calendar,
                &point);
        if ( mapped != wxWinUICoordinateResult::Mapped ||
             !wxWinUICalendarImpl::GetCurrentOwner(ticket) )
        {
            return wxCAL_HITTEST_NOWHERE;
        }

        wxDateTime resolvedDate;
        wxDateTime::WeekDay resolvedWeekday =
            wxDateTime::Inv_WeekDay;
        const wxCalendarHitTestResult result =
            wxWinUICalendarImpl::ResolvePublicHitAtPoint(
                ticket, point, &resolvedDate,
                &resolvedWeekday);
        if ( !wxWinUICalendarImpl::GetCurrentOwner(ticket) )
            return wxCAL_HITTEST_NOWHERE;

        if ( (result == wxCAL_HITTEST_DAY ||
              result == wxCAL_HITTEST_SURROUNDING_WEEK ||
              result == wxCAL_HITTEST_WEEK) &&
             date )
        {
            *date = resolvedDate;
        }
        if ( (result == wxCAL_HITTEST_HEADER ||
              result == wxCAL_HITTEST_WEEK) && wd )
        {
            *wd = resolvedWeekday;
        }
        return result;
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException(
            "WinUI CalendarView HitTest", e);
        return wxCAL_HITTEST_NOWHERE;
    }
}

bool wxCalendarCtrl::EnableMonthChange(bool enable)
{
    if ( enable == AllowMonthChange() )
        return false;

    const auto state = m_winui
        ? m_winui->callbackState
        : std::shared_ptr<wxWinUICalendarCallbackState>();
    const std::uint64_t generation = state
        ? state->Generation()
        : 0;
    long style = GetWindowStyleFlag();
    if ( enable )
        style &= ~wxCAL_NO_MONTH_CHANGE;
    else
        style |= wxCAL_NO_MONTH_CHANGE;
    SetWindowStyleFlag(style);

    wxCalendarCtrl * const owner = state
        ? state->GetOwner(generation)
        : this;
    return owner && owner->AllowMonthChange() == enable;
}

void wxCalendarCtrl::SetMinSize(const wxSize& minSize)
{
    if ( m_winui )
    {
        if ( m_winui->suppressNextMinSizeWriter )
            m_winui->suppressNextMinSizeWriter = false;
        else if ( ++m_winui->sizeWriterRevision == 0 )
            ++m_winui->sizeWriterRevision;
    }
    wxCalendarCtrlBase::SetMinSize(minSize);
}

void wxCalendarCtrl::DoSetSize(int x, int y, int width, int height,
                               int sizeFlags)
{
    if ( m_winui )
    {
        if ( m_winui->suppressNextSizeWriter )
            m_winui->suppressNextSizeWriter = false;
        else if ( ++m_winui->sizeWriterRevision == 0 )
            ++m_winui->sizeWriterRevision;
    }
    wxCalendarCtrlBase::DoSetSize(
        x, y, width, height, sizeFlags);
}

bool wxCalendarCtrl::ApplyInitialSizeTransaction(
    const wxSize& size, std::uint64_t expectedRevision)
{
    if ( !m_winui || !m_winui->callbackState )
        return false;

    wxWinUICalendarImpl * const impl = m_winui.get();
    const auto state = m_winui->callbackState;
    const std::uint64_t generation = state->Generation();
    const auto getExactOwner = [&]() -> wxCalendarCtrl *
    {
        wxCalendarCtrl * const owner = state->GetOwner(generation);
        return owner && owner->m_winui &&
               owner->m_winui.get() == impl &&
               owner->m_winui->callbackState == state
                   ? owner
                   : nullptr;
    };

    if ( impl->sizeWriterRevision != expectedRevision )
        return true;

    impl->suppressNextMinSizeWriter = true;
    SetMinSize(size);
    wxCalendarCtrl *owner = getExactOwner();
    if ( !owner )
        return false;
    impl->suppressNextMinSizeWriter = false;
    if ( impl->sizeWriterRevision != expectedRevision )
        return true;

    const wxSize best = owner->GetEffectiveMinSize();
    owner = getExactOwner();
    if ( !owner )
        return false;
    if ( impl->sizeWriterRevision != expectedRevision ||
         owner->GetSize() == best )
    {
        return true;
    }

    impl->suppressNextSizeWriter = true;
    owner->SetSize(best);
    owner = getExactOwner();
    if ( !owner )
        return false;
    impl->suppressNextSizeWriter = false;
    return true;
}

void wxCalendarCtrl::OnLayoutLoaded()
{
    if ( !m_winui || !m_winui->callbackState ||
         !m_winui->layoutRoot )
    {
        return;
    }

    wxWinUICalendarImpl * const impl = m_winui.get();
    const auto state = m_winui->callbackState;
    const std::uint64_t generation = state->Generation();
    const MUX::FrameworkElement layoutRoot = m_winui->layoutRoot;
    const auto getExactOwner = [&]() -> wxCalendarCtrl *
    {
        wxCalendarCtrl * const owner = state->GetOwner(generation);
        return owner && owner->m_winui &&
               owner->m_winui.get() == impl &&
               owner->m_winui->callbackState == state &&
               owner->m_winui->layoutRoot == layoutRoot
                   ? owner
                   : nullptr;
    };

    MUX::XamlRoot xamlRoot{ nullptr };
    try
    {
        xamlRoot = layoutRoot.XamlRoot();
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI CalendarView Loaded XamlRoot", e);
        return;
    }

    wxCalendarCtrl *owner = getExactOwner();
    if ( !owner )
        return;
    if ( impl->layoutXamlRoot != xamlRoot )
    {
        impl->layoutXamlRoot = xamlRoot;
        if ( ++impl->layoutRevision == 0 )
            ++impl->layoutRevision;
    }
    const std::uint64_t layoutRevision = impl->layoutRevision;
    owner->InvalidateBestSize();
    owner = getExactOwner();
    if ( !owner || impl->layoutRevision != layoutRevision ||
         impl->layoutXamlRoot != xamlRoot )
    {
        return;
    }

    // Loaded is the authoritative edge at which a newly-created or
    // reparented CalendarView template joins its XamlRoot. Reconcile the
    // actual template buttons here instead of relying exclusively on a
    // bounded dispatcher retry that may have run before template expansion.
    impl->navigationBindingRequired = true;
    impl->RefreshNavigationButtons(owner);
    owner = getExactOwner();
    if ( !owner || impl->layoutRevision != layoutRevision ||
         impl->layoutXamlRoot != xamlRoot )
    {
        return;
    }

    if ( !impl->initialSizeProjectionComplete ||
         !impl->initialSizePending )
    {
        return;
    }
    impl->initialSizePending = false;
    if ( impl->sizeWriterRevision != impl->initialSizeWriterRevision ||
         owner->GetMinSize() != impl->initialMinSize )
    {
        return;
    }
    owner->ApplyInitialSizeTransaction(
        impl->initialSizeRequest, impl->initialSizeWriterRevision);
}

void wxCalendarCtrl::EnableHolidayDisplay(bool display)
{
    if ( display == HasFlag(wxCAL_SHOW_HOLIDAYS) )
        return;

    const auto state = m_winui
        ? m_winui->callbackState
        : std::shared_ptr<wxWinUICalendarCallbackState>();
    const std::uint64_t generation = state
        ? state->Generation()
        : 0;
    long style = GetWindowStyleFlag();
    if ( display )
        style |= wxCAL_SHOW_HOLIDAYS;
    else
        style &= ~wxCAL_SHOW_HOLIDAYS;
    SetWindowStyleFlag(style);
    wxCalendarCtrl * const owner = state
        ? state->GetOwner(generation)
        : this;
    if ( owner && !display &&
         !owner->HasFlag(wxCAL_SHOW_HOLIDAYS) )
    {
        owner->ResetHolidayAttrs();
    }
}

void wxCalendarCtrl::SetWindowStyleFlag(long style)
{
    const long oldStyle = GetWindowStyleFlag();
    const long firstDayMask =
        wxCAL_MONDAY_FIRST | wxCAL_SUNDAY_FIRST;
    const long hadFirstDayStyle = oldStyle & firstDayMask;
    const bool hadSurroundingWeeks =
        HasFlag(wxCAL_SHOW_SURROUNDING_WEEKS);
    const bool hadWeekNumbers = HasFlag(wxCAL_SHOW_WEEK_NUMBERS);
    const bool hadHolidays = HasFlag(wxCAL_SHOW_HOLIDAYS);
    const bool hadMonthChange = AllowMonthChange();
    const bool hasMonthChange =
        (style & wxCAL_NO_MONTH_CHANGE) == 0;

    if ( hadMonthChange != hasMonthChange &&
         !ApplyStateToPeer(
             m_date, m_lowerDate, m_upperDate,
             hasMonthChange) )
    {
        return;
    }

    wxCalendarCtrlBase::SetWindowStyleFlag(style);

    const long hasFirstDayStyle = style & firstDayMask;
    const bool hasSurroundingWeeks =
        HasFlag(wxCAL_SHOW_SURROUNDING_WEEKS);
    const bool hasWeekNumbers = HasFlag(wxCAL_SHOW_WEEK_NUMBERS);
    const bool hasHolidays = HasFlag(wxCAL_SHOW_HOLIDAYS);
    if ( !m_winui || !m_winui->cal || !m_winui->callbackState )
        return;
    if ( hadFirstDayStyle == hasFirstDayStyle &&
         hadSurroundingWeeks == hasSurroundingWeeks &&
         hadWeekNumbers == hasWeekNumbers &&
         hadHolidays == hasHolidays &&
         hadMonthChange == hasMonthChange )
    {
        return;
    }

    wxWinUICalendarImpl * const updateImpl = m_winui.get();
    const std::shared_ptr<wxWinUICalendarCallbackState> state =
        m_winui->callbackState;
    const std::uint64_t generation = state->Generation();
    const MUXC::CalendarView calendar = m_winui->cal;
    const MUXC::Canvas weekCanvas = m_winui->weekCanvas;
    const MUXC::ColumnDefinition weekColumn = m_winui->weekColumn;

    try
    {
        wxWinUICalendarPeerMutationGuard guard(state);
        if ( hadFirstDayStyle != hasFirstDayStyle )
        {
            WG::DayOfWeek firstDay =
                updateImpl->defaultFirstDayOfWeek;
            if ( hasFirstDayStyle & wxCAL_MONDAY_FIRST )
                firstDay = WG::DayOfWeek::Monday;
            else if ( hasFirstDayStyle & wxCAL_SUNDAY_FIRST )
                firstDay = WG::DayOfWeek::Sunday;
            calendar.FirstDayOfWeek(
                firstDay);
        }
        if ( hadSurroundingWeeks != hasSurroundingWeeks )
            calendar.IsOutOfScopeEnabled(hasSurroundingWeeks);
        if ( hadWeekNumbers != hasWeekNumbers &&
             weekCanvas && weekColumn )
        {
            weekColumn.Width(
                MUX::GridLengthHelper::FromPixels(
                    hasWeekNumbers ? 36 : 0));
            weekCanvas.Visibility(
                hasWeekNumbers
                    ? MUX::Visibility::Visible
                    : MUX::Visibility::Collapsed);
            InvalidateBestSize();
        }

        // The shared-host flush is synchronous and may destroy this control.
        // Keep callbacks suppressed for the whole peer transaction and use
        // only the independently-owned callback state afterwards.
        updateImpl->host.ForceRender();
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException(
            "WinUI CalendarView style update", e);
        return;
    }

    wxCalendarCtrl * const owner = state->GetOwner(generation);
    if ( !owner || !owner->m_winui ||
         owner->m_winui.get() != updateImpl ||
         owner->m_winui->callbackState != state ||
         owner->m_winui->cal != calendar )
    {
        return;
    }
    owner->m_winui->RefreshNavigationButtons(owner);
    wxCalendarCtrl * const ownerAfterNavigation =
        state->GetOwner(generation);
    if ( !ownerAfterNavigation || !ownerAfterNavigation->m_winui ||
         ownerAfterNavigation->m_winui.get() != updateImpl ||
         ownerAfterNavigation->m_winui->callbackState != state ||
         ownerAfterNavigation->m_winui->cal != calendar )
    {
        return;
    }
    wxCalendarCtrl *ownerForWeeks = ownerAfterNavigation;
    if ( hadHolidays != hasHolidays )
    {
        ownerAfterNavigation->m_winui->RefreshHolidayDates(
            ownerAfterNavigation);
        ownerForWeeks = state->GetOwner(generation);
        if ( !ownerForWeeks || !ownerForWeeks->m_winui ||
             ownerForWeeks->m_winui.get() != updateImpl ||
             ownerForWeeks->m_winui->callbackState != state ||
             ownerForWeeks->m_winui->cal != calendar )
        {
            return;
        }
    }
    ownerForWeeks->m_winui->RefreshWeekNumbers(ownerForWeeks);
    // RefreshWeekNumbers() crosses XAML property boundaries. Do not access the
    // control after it returns.
}

void wxCalendarCtrl::Mark(size_t day, bool mark)
{
    wxCHECK_RET( day > 0 && day < 32, "invalid day" );

    const std::uint32_t mask =
        1u << static_cast<unsigned>(day - 1);
    const std::uint32_t previous = m_marks;
    if ( mark )
        m_marks |= mask;
    else
        m_marks &= ~mask;

    if ( m_marks != previous && m_winui )
        m_winui->RefreshMarks(this);
}

void wxCalendarCtrl::SetHoliday(size_t day)
{
    wxCHECK_RET( day > 0 && day < 32, "invalid holiday day" );

    const std::uint32_t mask =
        1u << static_cast<unsigned>(day - 1);
    if ( m_holidays & mask )
        return;

    m_holidays |= mask;
    if ( m_winui )
        m_winui->RefreshMarks(this);
}

void wxCalendarCtrl::ResetHolidayAttrs()
{
    const bool hadAutomatic =
        m_winui && !m_winui->automaticHolidayKeys.empty();
    if ( m_holidays == 0 && !hadAutomatic )
        return;

    m_holidays = 0;
    if ( m_winui )
    {
        m_winui->automaticHolidayKeys.clear();
        m_winui->RefreshMarks(this);
    }
}

void wxCalendarCtrl::RefreshHolidays()
{
    if ( m_winui )
        m_winui->RefreshHolidayDates(this);
}

void wxCalendarCtrl::HandleVisualThemeChanged()
{
    if ( m_winui )
    {
        // DensityColors are resolved colours, not ThemeResource references.
        // Re-issue every realized mark when WinUI publishes a new effective
        // theme so Light/Dark/High Contrast never retain the previous
        // CalendarView foreground.
        m_winui->RefreshMarks(this);
    }
}

void wxCalendarCtrl::OnSysColourChanged(
    wxSysColourChangedEvent& event)
{
    event.Skip();

    const wxWeakRef<wxCalendarCtrl> alive(this);
    HandleVisualThemeChanged();

    // A future peer write seam or XAML callback may destroy this handler.
    // Stop event-table continuation if it did.
    if ( !alive )
        event.Skip(false);
}

wxSize wxCalendarCtrl::DoGetBestSize() const
{
    wxSize size(296 + (HasFlag(wxCAL_SHOW_WEEK_NUMBERS) ? 36 : 0),
                348);
    if ( m_winui && m_winui->root )
    {
        try
        {
            const float infinity =
                std::numeric_limits<float>::infinity();
            m_winui->root.Measure(WF::Size{infinity, infinity});
            const WF::Size desired = m_winui->root.DesiredSize();
            if ( std::isfinite(desired.Width) && desired.Width > 0 &&
                 std::isfinite(desired.Height) && desired.Height > 0 )
            {
                size.x = wxMax(size.x,
                    static_cast<int>(std::ceil(desired.Width)));
                size.y = wxMax(size.y,
                    static_cast<int>(std::ceil(desired.Height)));
            }
        }
        catch ( const winrt::hresult_error& )
        {
        }
    }
    return wxWindow::FromDIP(
        size, const_cast<wxCalendarCtrl*>(this));
}

bool wxCalendarCtrl::ApplyToPeer()
{
    return ApplyStateToPeer(
        m_date, m_lowerDate, m_upperDate,
        AllowMonthChange());
}

bool wxCalendarCtrl::ApplyStateToPeer(
    const wxDateTime& date,
    const wxDateTime& lowerdate,
    const wxDateTime& upperdate,
    bool monthChangeEnabled)
{
    if ( !m_winui || !m_winui->cal )
        return true;
    if ( !m_winui->callbackState ||
         !m_winui->defaultsCaptured || !date.IsValid() )
        return false;

    wxWinUICalendarImpl * const updateImpl = m_winui.get();
    const std::shared_ptr<wxWinUICalendarCallbackState> state =
        m_winui->callbackState;
    const std::uint64_t generation = state->Generation();
    const std::uint64_t revision = state->StateRevision();
    const MUXC::CalendarView calendar = m_winui->cal;
    const WF::DateTime civilPeerMinimum =
        m_winui->civilMinimum;
    const WF::DateTime civilPeerMaximum =
        m_winui->civilMaximum;

    wxDateTime effectiveMinimum =
        lowerdate.IsValid()
            ? lowerdate
            : wxWinUIFromDateTime(civilPeerMinimum);
    wxDateTime effectiveMaximum =
        upperdate.IsValid()
            ? upperdate
            : wxWinUIFromDateTime(civilPeerMaximum);
    if ( !effectiveMinimum.IsValid() ||
         !effectiveMaximum.IsValid() )
    {
        return false;
    }

    WF::DateTime peerDate{};
    if ( !wxWinUIToDateTime(date, &peerDate) )
        return false;

    if ( !monthChangeEnabled )
    {
        wxDateTime monthMinimum;
        wxDateTime monthMaximum;
        try
        {
            WG::Calendar model;
            model.ChangeCalendarSystem(
                m_winui->calendarIdentifier);
            model.SetDateTime(peerDate);
            model.Day(model.FirstDayInThisMonth());
            monthMinimum =
                wxWinUIFromDateTime(model.GetDateTime());
            model.Day(model.LastDayInThisMonth());
            monthMaximum =
                wxWinUIFromDateTime(model.GetDateTime());
        }
        catch ( const winrt::hresult_error& )
        {
            return false;
        }
        if ( !monthMinimum.IsValid() || !monthMaximum.IsValid() )
            return false;
        if ( effectiveMinimum < monthMinimum )
            effectiveMinimum = monthMinimum;
        if ( effectiveMaximum > monthMaximum )
            effectiveMaximum = monthMaximum;
    }

    if ( effectiveMinimum > effectiveMaximum ||
         date < effectiveMinimum || date > effectiveMaximum )
    {
        return false;
    }

    WF::DateTime peerMinimum{};
    WF::DateTime peerMaximum{};

    if ( monthChangeEnabled )
    {
        if ( lowerdate.IsValid() )
        {
            if ( !wxWinUIToDateTime(
                     effectiveMinimum, &peerMinimum) )
            {
                return false;
            }
        }
        else
        {
            peerMinimum = civilPeerMinimum;
        }

        if ( upperdate.IsValid() )
        {
            if ( !wxWinUIToDateTime(
                     effectiveMaximum, &peerMaximum) )
            {
                return false;
            }
        }
        else
        {
            peerMaximum = civilPeerMaximum;
        }
    }
    else if ( !wxWinUIToDateTime(
                  effectiveMinimum, &peerMinimum) ||
              !wxWinUIToDateTime(
                  effectiveMaximum, &peerMaximum) )
    {
        return false;
    }

    wxWinUICalendarPeerMutationGuard guard(state);

    WF::DateTime previousMinimum{};
    WF::DateTime previousMaximum{};
    std::vector<WF::DateTime> previousSelection;
    bool snapshotCaptured = false;
    bool applied = false;
    bool peerWritePerformed = false;
    try
    {
        previousMinimum = calendar.MinDate();
        previousMaximum = calendar.MaxDate();
        const auto selected = calendar.SelectedDates();
        previousSelection.reserve(selected.Size());
        for ( std::uint32_t i = 0; i < selected.Size(); ++i )
            previousSelection.push_back(selected.GetAt(i));
        snapshotCaptured = true;

        const bool boundsAlreadyApplied =
            previousMinimum.time_since_epoch() ==
                peerMinimum.time_since_epoch() &&
            previousMaximum.time_since_epoch() ==
                peerMaximum.time_since_epoch();
        const bool selectionAlreadyApplied =
            previousSelection.size() == 1 &&
            wxWinUISameCivilDate(
                wxWinUIFromDateTime(previousSelection.front()), date);
        if ( boundsAlreadyApplied && selectionAlreadyApplied )
        {
            applied = true;
        }
        else
        {
            peerWritePerformed = true;
            // A range may be disjoint from the previous selection. Remove it
            // while callbacks are suppressed before changing the bounds so
            // XAML never has to coerce an out-of-range selected date.
            selected.Clear();
            wxWinUIApplyCalendarBounds(
                calendar, peerMinimum, peerMaximum);

            selected.Append(peerDate);
            calendar.SetDisplayDate(peerDate);

            wxDateTime actualDate;
            applied =
                wxWinUIReadSelectedDate(calendar, &actualDate) &&
                wxWinUISameCivilDate(actualDate, date) &&
                calendar.MinDate().time_since_epoch() ==
                    peerMinimum.time_since_epoch() &&
                calendar.MaxDate().time_since_epoch() ==
                    peerMaximum.time_since_epoch();
        }
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException(
            "WinUI CalendarView state update", e);
    }

    if ( !state->IsStateMutationCurrent(revision) )
        return false;

    if ( !applied )
    {
        if ( snapshotCaptured )
        {
            try
            {
                const auto selected = calendar.SelectedDates();
                selected.Clear();
                wxWinUIApplyCalendarBounds(
                    calendar,
                    previousMinimum,
                    previousMaximum);
                for ( const WF::DateTime& previous :
                      previousSelection )
                {
                    selected.Append(previous);
                }
                if ( !previousSelection.empty() )
                    calendar.SetDisplayDate(previousSelection.front());
            }
            catch ( const winrt::hresult_error& e )
            {
                wxWinUILogException(
                    "WinUI CalendarView state rollback", e);
            }
        }
        return false;
    }

    wxCalendarCtrl *liveOwner = state->GetOwner(generation);
    if ( !liveOwner || !liveOwner->m_winui ||
         liveOwner->m_winui.get() != updateImpl ||
         liveOwner->m_winui->callbackState != state ||
         liveOwner->m_winui->cal != calendar ||
         !state->IsStateMutationCurrent(revision) )
    {
        return false;
    }

    if ( peerWritePerformed )
        liveOwner->m_winui->host.ForceRender();
    liveOwner = state->GetOwner(generation);
    return liveOwner && liveOwner->m_winui &&
           liveOwner->m_winui.get() == updateImpl &&
           liveOwner->m_winui->callbackState == state &&
           liveOwner->m_winui->cal == calendar &&
           state->IsStateMutationCurrent(revision);
}

void wxCalendarCtrl::OnPeerSelectionChanged()
{
    if ( !m_winui || !m_winui->cal )
        return;

    wxWinUICalendarImpl * const callbackImpl = m_winui.get();
    const std::shared_ptr<wxWinUICalendarCallbackState> state =
        m_winui->callbackState;
    if ( !state )
        return;
    const std::uint64_t generation = state->Generation();
    const MUXC::CalendarView calendar = m_winui->cal;
    const winrt::hstring calendarIdentifier =
        m_winui->calendarIdentifier;

    wxDateTime value;
    try
    {
        if ( !wxWinUIReadSelectedDate(calendar, &value) )
        {
            wxCalendarCtrl * const owner = state->GetOwner(generation);
            if ( owner && owner->m_winui &&
                 owner->m_winui.get() == callbackImpl &&
                 owner->m_winui->callbackState == state &&
                 owner->m_winui->cal == calendar )
            {
                owner->ApplyToPeer();
            }
            return;
        }
    }
    catch ( const winrt::hresult_error& )
    {
        return;
    }

    wxCalendarCtrl *owner = state->GetOwner(generation);
    if ( !owner || !owner->m_winui ||
         owner->m_winui.get() != callbackImpl ||
         owner->m_winui->callbackState != state ||
         owner->m_winui->cal != calendar )
    {
        return;
    }

    if ( !wxWinUIIsInsideRange(
         value, owner->m_lowerDate, owner->m_upperDate) ||
         (!owner->AllowMonthChange() &&
          !wxWinUISameCalendarMonth(
              calendarIdentifier, value, owner->m_date)) )
    {
        owner->ApplyToPeer();
        return;
    }

    if ( wxWinUISameCivilDate(value, owner->m_date) )
        return;

    const wxDateTime oldDate = owner->m_date;
    const bool changedActiveMonth = !wxWinUISameCalendarMonth(
        calendarIdentifier, oldDate, value);
    owner->m_date = value;

    // wxMSW publishes GenerateAllChangeEvents() before asking holiday
    // authorities for the new page. Keep the same observable ordering, with
    // generation checks between events so deleting the control from the first
    // handler cannot make the remaining events dereference it.
    wxWinUIDispatchCalendarSelectionEvents(
        state, generation, oldDate, value);

    owner = state->GetOwner(generation);
    if ( !owner || !owner->m_winui ||
         owner->m_winui.get() != callbackImpl ||
         owner->m_winui->callbackState != state ||
         owner->m_winui->cal != calendar ||
         !wxWinUISameCivilDate(owner->m_date, value) )
    {
        return;
    }

    if ( changedActiveMonth )
    {
        owner->m_holidays = 0;
        owner->m_winui->RefreshHolidayDates(owner);
    }
    else
    {
        owner->m_winui->RefreshMarks(owner);
    }

    owner = state->GetOwner(generation);
    if ( !owner || !owner->m_winui ||
         owner->m_winui.get() != callbackImpl ||
         owner->m_winui->callbackState != state ||
         owner->m_winui->cal != calendar )
    {
        return;
    }
    owner->m_winui->RefreshWeekNumbers(owner);
    // RefreshWeekNumbers() crosses XAML. Do not access any member afterwards.
}

#ifdef WXWINUI_TEST_SUPPORT
bool wxWinUICalendarTestAccess::SetPeerDate(
    wxCalendarCtrl& control,
    const wxDateTime& date)
{
    if ( !control.m_winui || !control.m_winui->cal ||
         !control.m_winui->callbackState || !date.IsValid() )
        return false;

    WF::DateTime peerDate{};
    if ( !wxWinUIToDateTime(date, &peerDate) )
        return false;

    wxWinUICalendarImpl * const updateImpl = control.m_winui.get();
    const std::shared_ptr<wxWinUICalendarCallbackState> state =
        control.m_winui->callbackState;
    const std::uint64_t generation = state->Generation();
    const MUXC::CalendarView calendar = control.m_winui->cal;
    try
    {
        // Keep the deterministic peer seam faithful to a real selectable day:
        // a user can only select a date displayed by CalendarView. Merely
        // replacing SelectedDates with an off-screen year does not navigate
        // the view and leaves no realized day item for subsequent hit tests.
        calendar.SetDisplayDate(peerDate);
        const auto selected = calendar.SelectedDates();
        if ( selected.Size() == 1 )
            selected.SetAt(0, peerDate);
        else
            selected.Append(peerDate);

        wxCalendarCtrl * const liveOwner =
            state->GetOwner(generation);
        if ( !liveOwner || !liveOwner->m_winui ||
             liveOwner->m_winui.get() != updateImpl ||
             liveOwner->m_winui->callbackState != state ||
             liveOwner->m_winui->cal != calendar )
        {
            // The peer mutation was completed before its wx event destroyed
            // the owner. Preserve the seam's historical success verdict.
            return true;
        }

        liveOwner->m_winui->host.ForceRender();
        return true;
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException(
            "WinUI CalendarView test date mutation", e);
        return false;
    }
}

bool wxWinUICalendarTestAccess::GetPeerState(
    const wxCalendarCtrl& control,
    wxDateTime *date,
    wxDateTime *minimum,
    wxDateTime *maximum)
{
    if ( !control.m_winui || !control.m_winui->cal )
        return false;

    try
    {
        const MUXC::CalendarView calendar = control.m_winui->cal;
        if ( date &&
             !wxWinUIReadSelectedDate(calendar, date) )
        {
            return false;
        }
        if ( minimum )
        {
            *minimum =
                wxWinUIFromDateTime(calendar.MinDate());
            if ( !minimum->IsValid() )
                return false;
        }
        if ( maximum )
        {
            *maximum =
                wxWinUIFromDateTime(calendar.MaxDate());
            if ( !maximum->IsValid() )
                return false;
        }
        return true;
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException(
            "WinUI CalendarView test state query", e);
        return false;
    }
}

bool wxWinUICalendarTestAccess::GetDefaultPeerRange(
    const wxCalendarCtrl& control,
    wxDateTime *minimum,
    wxDateTime *maximum)
{
    if ( !control.m_winui || !control.m_winui->defaultsCaptured )
        return false;

    if ( minimum )
    {
        *minimum =
            wxWinUIFromDateTime(control.m_winui->defaultMinimum);
        if ( !minimum->IsValid() )
            return false;
    }
    if ( maximum )
    {
        *maximum =
            wxWinUIFromDateTime(control.m_winui->defaultMaximum);
        if ( !maximum->IsValid() )
            return false;
    }
    return true;
}

bool wxWinUICalendarTestAccess::DoubleTapDate(
    wxCalendarCtrl& control,
    const wxDateTime& date)
{
    if ( !control.m_winui || !control.m_winui->cal ||
         !control.m_winui->callbackState || !date.IsValid() )
        return false;

    wxWinUICalendarImpl * const updateImpl = control.m_winui.get();
    const std::shared_ptr<wxWinUICalendarCallbackState> state =
        control.m_winui->callbackState;
    const std::uint64_t generation = state->Generation();
    const MUXC::CalendarView calendar = control.m_winui->cal;
    updateImpl->host.ForceRender();
    wxCalendarCtrl * const liveOwner =
        state->GetOwner(generation);
    if ( !liveOwner || !liveOwner->m_winui ||
         liveOwner->m_winui.get() != updateImpl ||
         liveOwner->m_winui->callbackState != state ||
         liveOwner->m_winui->cal != calendar )
    {
        return false;
    }

    const MUXC::CalendarViewDayItem item =
        liveOwner->m_winui->FindRealizedDay(date);
    if ( !item )
        return false;
    if ( item.ActualWidth() <= 0 || item.ActualHeight() <= 0 )
        return false;

    try
    {
        const MUXM::GeneralTransform transform =
            item.TransformToVisual(calendar);
        const WF::Point centre{
            static_cast<float>(item.ActualWidth() / 2.0),
            static_cast<float>(item.ActualHeight() / 2.0)
        };
        const WF::Point point = transform.TransformPoint(centre);
        return liveOwner->m_winui->HandleDoubleTapAt(point);
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException(
            "WinUI CalendarView test double tap", e);
        return false;
    }
}

bool wxWinUICalendarTestAccess::DoubleTapNonDay(wxCalendarCtrl& control)
{
    if ( !control.m_winui || !control.m_winui->cal ||
         !control.m_winui->callbackState )
        return false;

    wxWinUICalendarImpl * const updateImpl = control.m_winui.get();
    const std::shared_ptr<wxWinUICalendarCallbackState> state =
        control.m_winui->callbackState;
    const std::uint64_t generation = state->Generation();
    const MUXC::CalendarView calendar = control.m_winui->cal;
    try
    {
        // Resolve a point in the realized header/navigation band and prove it
        // is outside every live day geometry. Using half of the selected
        // day's Y coordinate was not sufficient: a selected day in a later
        // week can put that midpoint directly over an earlier day row.
        updateImpl->host.ForceRender();
        wxCalendarCtrl * const liveOwner =
            state->GetOwner(generation);
        if ( !liveOwner || !liveOwner->m_winui ||
             liveOwner->m_winui.get() != updateImpl ||
             liveOwner->m_winui->callbackState != state ||
             liveOwner->m_winui->cal != calendar )
        {
            return false;
        }

        const wxWinUICalendarImpl::HitTestTicket ticket =
            updateImpl->CaptureHitTestTicket();
        wxWinUICalendarImpl::PublicHitSnapshot snapshot;
        if ( !wxWinUICalendarImpl::CapturePublicHitSnapshot(
                 ticket, &snapshot) )
        {
            return false;
        }

        const auto isDayPoint = [&snapshot](const WF::Point& candidate)
        {
            return std::any_of(
                snapshot.days.begin(), snapshot.days.end(),
                [&candidate](
                    const wxWinUICalendarImpl::DayHitGeometry& day)
                {
                    return wxWinUICalendarImpl::ContainsPoint(
                        day.bounds, candidate);
                });
        };

        WF::Point point{-1.0f, -1.0f};
        for ( const wxWinUICalendarImpl::ElementHitGeometry& label :
              snapshot.weekdayLabels )
        {
            const WF::Point candidate{
                label.bounds.X + label.bounds.Width / 2.0f,
                label.bounds.Y + label.bounds.Height / 2.0f};
            if ( !isDayPoint(candidate) )
            {
                point = candidate;
                break;
            }
        }

        if ( point.X < 0.0f && !snapshot.days.empty() )
        {
            const auto first = std::min_element(
                snapshot.days.begin(), snapshot.days.end(),
                [](const wxWinUICalendarImpl::DayHitGeometry& lhs,
                   const wxWinUICalendarImpl::DayHitGeometry& rhs)
                {
                    return lhs.bounds.Y < rhs.bounds.Y;
                });
            const WF::Point candidate{
                first->bounds.X + first->bounds.Width / 2.0f,
                std::max(0.5f, first->bounds.Y / 2.0f)};
            if ( candidate.Y < first->bounds.Y &&
                 !isDayPoint(candidate) )
            {
                point = candidate;
            }
        }

        if ( point.X < 0.0f ||
             !wxWinUICalendarImpl::GetCurrentOwner(ticket) )
        {
            return false;
        }

        // Return whether an event was emitted; the expected value is false.
        return liveOwner->m_winui->HandleDoubleTapAt(point);
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException(
            "WinUI CalendarView test non-day double tap", e);
        return false;
    }
}

bool wxWinUICalendarTestAccess::IsMarked(
    const wxCalendarCtrl& control,
    size_t day)
{
    if ( day == 0 || day > 31 )
        return false;

    return (control.m_marks &
            (1u << static_cast<unsigned>(day - 1))) != 0;
}

bool wxWinUICalendarTestAccess::IsHoliday(
    const wxCalendarCtrl& control,
    size_t day)
{
    if ( day == 0 || day > 31 )
        return false;

    return (control.m_holidays &
            (1u << static_cast<unsigned>(day - 1))) != 0;
}

bool wxWinUICalendarTestAccess::GetAppliedDensityColour(
    wxCalendarCtrl& control,
    size_t day,
    wxColour *colour,
    unsigned *colourCount,
    unsigned long long *writeRevision)
{
    if ( !control.m_winui || !control.m_winui->cal ||
         !control.m_winui->callbackState || day == 0 || day > 31 ||
         !control.m_date.IsValid() ||
         day > static_cast<size_t>(
             wxDateTime::GetNumberOfDays(
                 control.m_date.GetMonth(), control.m_date.GetYear())) )
    {
        return false;
    }

    wxWinUICalendarImpl * const implementation = control.m_winui.get();
    const auto state = control.m_winui->callbackState;
    const std::uint64_t generation = state->Generation();
    const MUXC::CalendarView calendar = control.m_winui->cal;
    implementation->host.ForceRender();

    wxCalendarCtrl *owner = state->GetOwner(generation);
    if ( !owner || !owner->m_winui ||
         owner->m_winui.get() != implementation ||
         owner->m_winui->callbackState != state ||
         owner->m_winui->cal != calendar )
    {
        return false;
    }

    wxDateTime date(owner->m_date);
    date.SetDay(static_cast<wxDateTime::wxDateTime_t>(day));
    const MUXC::CalendarViewDayItem item =
        implementation->FindRealizedDay(date);
    owner = state->GetOwner(generation);
    if ( !owner || !owner->m_winui ||
         owner->m_winui.get() != implementation ||
         owner->m_winui->callbackState != state ||
         owner->m_winui->cal != calendar )
    {
        return false;
    }
    if ( !item )
        return false;

    const int key = wxWinUICivilDateKey(date);
    const auto found =
        implementation->appliedDensityColours.find(key);
    if ( found == implementation->appliedDensityColours.end() )
        return false;
    const auto revisionFound =
        implementation->appliedDensityColourRevisions.find(key);
    if ( revisionFound ==
         implementation->appliedDensityColourRevisions.end() )
    {
        return false;
    }

    if ( colourCount )
    {
        *colourCount =
            static_cast<unsigned>(found->second.size());
    }
    if ( colour )
    {
        if ( found->second.empty() )
            *colour = wxNullColour;
        else
        {
            const WU::Color value = found->second.front();
            *colour = wxColour(
                value.R, value.G, value.B, value.A);
        }
    }
    if ( writeRevision )
        *writeRevision = revisionFound->second;
    return true;
}

bool wxWinUICalendarTestAccess::SetTodayForeground(
    wxCalendarCtrl& control,
    const wxColour& colour)
{
    if ( !colour.IsOk() || !control.m_winui || !control.m_winui->cal ||
         !control.m_winui->callbackState )
    {
        return false;
    }

    wxWinUICalendarImpl * const implementation = control.m_winui.get();
    const auto state = control.m_winui->callbackState;
    const std::uint64_t generation = state->Generation();
    const MUXC::CalendarView calendar = control.m_winui->cal;
    try
    {
        calendar.TodayForeground(wxWinUIBrush(
            colour.Red(), colour.Green(),
            colour.Blue(), colour.Alpha()));
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException(
            "WinUI CalendarView test theme colour", e);
        return false;
    }

    wxCalendarCtrl * const owner = state->GetOwner(generation);
    return owner && owner->m_winui &&
           owner->m_winui.get() == implementation &&
           owner->m_winui->callbackState == state &&
           owner->m_winui->cal == calendar;
}

bool wxWinUICalendarTestAccess::UseSystemMarkColourFallback(wxCalendarCtrl& control)
{
    if ( !control.m_winui || !control.m_winui->cal ||
         !control.m_winui->callbackState )
    {
        return false;
    }

    wxWinUICalendarImpl * const implementation = control.m_winui.get();
    const auto state = control.m_winui->callbackState;
    const std::uint64_t generation = state->Generation();
    const MUXC::CalendarView calendar = control.m_winui->cal;
    try
    {
        // A non-solid authored brush exercises the same production fallback
        // used when a theme does not resolve TodayForeground to one colour.
        calendar.TodayForeground(MUXM::LinearGradientBrush());
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException(
            "WinUI CalendarView test system-colour fallback", e);
        return false;
    }

    wxCalendarCtrl * const owner = state->GetOwner(generation);
    return owner && owner->m_winui &&
           owner->m_winui.get() == implementation &&
           owner->m_winui->callbackState == state &&
           owner->m_winui->cal == calendar;
}

bool wxWinUICalendarTestAccess::DeliverThemeChanged(wxCalendarCtrl& control)
{
    if ( !control.m_winui || !control.m_winui->cal ||
         !control.m_winui->callbackState ||
         !control.m_winui->actualThemeChangedToken.value )
    {
        return false;
    }

    wxWinUICalendarImpl * const implementation = control.m_winui.get();
    const auto state = control.m_winui->callbackState;
    const std::uint64_t generation = state->Generation();
    const MUXC::CalendarView calendar = control.m_winui->cal;
    try
    {
        // Drive the real FrameworkElement theme pipeline. In particular, do
        // not call HandleVisualThemeChanged() here: doing so would let the
        // test pass even if the ActualThemeChanged subscription were absent
        // or its callback stopped refreshing the realized DensityColors.
        const MUX::ElementTheme actual = calendar.ActualTheme();
        calendar.RequestedTheme(
            actual == MUX::ElementTheme::Dark
                ? MUX::ElementTheme::Light
                : MUX::ElementTheme::Dark);
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException(
            "WinUI CalendarView test theme transition", e);
        return false;
    }
    wxCalendarCtrl * const owner = state->GetOwner(generation);
    return owner && owner->m_winui &&
           owner->m_winui.get() == implementation &&
           owner->m_winui->callbackState == state &&
           owner->m_winui->cal == calendar;
}

bool wxWinUICalendarTestAccess::HasThemeChangedHandler(const wxCalendarCtrl& control)
{
    return control.m_winui && control.m_winui->cal &&
           control.m_winui->callbackState &&
           control.m_winui->actualThemeChangedToken.value != 0;
}

bool wxWinUICalendarTestAccess::GetDateClientPoint(
    wxCalendarCtrl& control,
    const wxDateTime& date,
    wxPoint *point)
{
    if ( !point || !date.IsValid() ||
         !control.m_winui || !control.m_winui->cal ||
         !control.m_winui->callbackState )
    {
        return false;
    }

    wxWinUICalendarImpl * const implementation = control.m_winui.get();
    const auto state = control.m_winui->callbackState;
    const std::uint64_t generation = state->Generation();
    const MUXC::CalendarView calendar = control.m_winui->cal;
    implementation->host.ForceRender();

    wxCalendarCtrl *owner = state->GetOwner(generation);
    if ( !owner || !owner->m_winui ||
         owner->m_winui.get() != implementation ||
         owner->m_winui->callbackState != state ||
         owner->m_winui->cal != calendar )
    {
        return false;
    }

    const MUXC::CalendarViewDayItem item =
        implementation->FindRealizedDay(date);
    owner = state->GetOwner(generation);
    if ( !owner || !owner->m_winui ||
         owner->m_winui.get() != implementation ||
         owner->m_winui->callbackState != state ||
         owner->m_winui->cal != calendar )
    {
        return false;
    }
    if ( !item )
        return false;

    const double width = item.ActualWidth();
    owner = state->GetOwner(generation);
    if ( !owner || !owner->m_winui ||
         owner->m_winui.get() != implementation ||
         owner->m_winui->callbackState != state ||
         owner->m_winui->cal != calendar )
    {
        return false;
    }
    const double height = item.ActualHeight();
    owner = state->GetOwner(generation);
    if ( !owner || !owner->m_winui ||
         owner->m_winui.get() != implementation ||
         owner->m_winui->callbackState != state ||
         owner->m_winui->cal != calendar )
    {
        return false;
    }
    if ( width <= 0.0 || height <= 0.0 )
        return false;

    WF::Point clientPixels{};
    const wxWinUICoordinateResult mapped =
        wxWinUIVisualCoordinates::ElementPointToClient(
            owner,
            item,
            WF::Point{
                static_cast<float>(width / 2.0),
                static_cast<float>(height / 2.0)
            },
            &clientPixels);
    if ( mapped != wxWinUICoordinateResult::Mapped )
        return false;

    owner = state->GetOwner(generation);
    if ( !owner || !owner->m_winui ||
         owner->m_winui.get() != implementation ||
         owner->m_winui->callbackState != state ||
         owner->m_winui->cal != calendar )
    {
        return false;
    }

    point->x = static_cast<int>(std::lround(clientPixels.X));
    point->y = static_cast<int>(std::lround(clientPixels.Y));
    return true;
}

bool wxWinUICalendarTestAccess::GetPeerRightToLeft(
    const wxCalendarCtrl& control,
    bool *rightToLeft)
{
    if ( !rightToLeft || !control.m_winui || !control.m_winui->cal )
        return false;

    try
    {
        *rightToLeft =
            control.m_winui->cal.FlowDirection() ==
                MUX::FlowDirection::RightToLeft;
        return true;
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException(
            "WinUI CalendarView flow-direction snapshot", e);
        return false;
    }
}

bool wxWinUICalendarTestAccess::GetPeerCalendarStyle(
    const wxCalendarCtrl& control,
    bool *mondayFirst,
    bool *surroundingWeeks,
    bool *weekNumbers)
{
    if ( !control.m_winui || !control.m_winui->cal || !control.m_winui->weekCanvas ||
         !control.m_winui->weekColumn )
    {
        return false;
    }

    try
    {
        if ( mondayFirst )
        {
            *mondayFirst =
                control.m_winui->cal.FirstDayOfWeek() ==
                    winrt::Windows::Globalization::DayOfWeek::Monday;
        }
        if ( surroundingWeeks )
            *surroundingWeeks = control.m_winui->cal.IsOutOfScopeEnabled();
        if ( weekNumbers )
        {
            const MUX::GridLength width = control.m_winui->weekColumn.Width();
            *weekNumbers =
                control.m_winui->weekCanvas.Visibility() ==
                    MUX::Visibility::Visible &&
                width.GridUnitType == MUX::GridUnitType::Pixel &&
                width.Value > 0.0;
        }
        return true;
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException(
            "WinUI CalendarView style snapshot", e);
        return false;
    }
}

bool wxWinUICalendarTestAccess::GetWeekNumber(
    wxCalendarCtrl& control,
    const wxDateTime& date,
    wxDateTime *weekStart,
    int *weekNumber)
{
    if ( !date.IsValid() || !control.m_winui || !control.m_winui->cal ||
         !control.m_winui->callbackState ||
         !control.HasFlag(wxCAL_SHOW_WEEK_NUMBERS) )
    {
        return false;
    }

    wxWinUICalendarImpl * const implementation = control.m_winui.get();
    const auto state = control.m_winui->callbackState;
    const std::uint64_t generation = state->Generation();
    const MUXC::CalendarView calendar = control.m_winui->cal;
    implementation->host.ForceRender();

    wxCalendarCtrl *owner = state->GetOwner(generation);
    if ( !owner || !owner->m_winui ||
         owner->m_winui.get() != implementation ||
         owner->m_winui->callbackState != state ||
         owner->m_winui->cal != calendar )
    {
        return false;
    }
    implementation->RefreshWeekNumbers(owner);

    owner = state->GetOwner(generation);
    if ( !owner || !owner->m_winui ||
         owner->m_winui.get() != implementation ||
         owner->m_winui->callbackState != state ||
         owner->m_winui->cal != calendar )
    {
        return false;
    }

    const wxDateTime::WeekDay firstWeekday =
        owner->WeekStartsOnMonday()
            ? wxDateTime::Mon
            : wxDateTime::Sun;
    const int daysSinceStart =
        (static_cast<int>(date.GetWeekDay()) -
         static_cast<int>(firstWeekday) + 7) % 7;
    const wxDateTime expectedStart =
        wxWinUINormalizeCivilDate(date) -
            wxDateSpan::Days(daysSinceStart);

    for ( const wxWinUICalendarImpl::WeekRow& row :
          implementation->weekRows )
    {
        if ( row.visible && row.startDate == expectedStart )
        {
            if ( weekStart )
                *weekStart = row.startDate;
            if ( weekNumber )
                *weekNumber = row.weekNumber;
            return row.weekNumber >= 1 && row.weekNumber <= 53;
        }
    }
    return false;
}

unsigned long long wxWinUICalendarTestAccess::GetWeekRefreshRunCount(const wxCalendarCtrl& control)
{
    return control.m_winui ? control.m_winui->weekRefreshRunCount : 0;
}

bool wxWinUICalendarTestAccess::RequestWeekRefresh(wxCalendarCtrl& control)
{
    if ( !control.m_winui || !control.m_winui->cal || !control.m_winui->callbackState )
        return false;

    wxWinUICalendarImpl * const impl = control.m_winui.get();
    const auto state = control.m_winui->callbackState;
    const std::uint64_t generation = state->Generation();
    impl->RefreshWeekNumbers(&control);
    wxCalendarCtrl * const owner = state->GetOwner(generation);
    return owner == &control && owner->m_winui &&
           owner->m_winui.get() == impl &&
           owner->m_winui->callbackState == state;
}

bool wxWinUICalendarTestAccess::SetWeekRefreshFailures(
    wxCalendarCtrl& control,
    unsigned count)
{
    if ( count >
             wxWinUICalendarImpl::MaxWeekRefreshAutomaticRetries + 1 ||
         !control.m_winui || !control.m_winui->cal || !control.m_winui->callbackState )
    {
        return false;
    }

    wxWinUICalendarImpl * const impl = control.m_winui.get();
    const auto state = control.m_winui->callbackState;
    const std::uint64_t generation = state->Generation();
    if ( state->GetOwner(generation) != &control )
        return false;
    impl->weekRefreshFaultsForTesting = count;
    return state->GetOwner(generation) == &control && control.m_winui &&
           control.m_winui.get() == impl && control.m_winui->callbackState == state;
}

bool wxWinUICalendarTestAccess::SetWeekRefreshPendingPasses(
    wxCalendarCtrl& control,
    unsigned count)
{
    if ( count >
             wxWinUICalendarImpl::MaxWeekRefreshAutomaticRetries + 1 ||
         !control.m_winui || !control.m_winui->cal || !control.m_winui->callbackState )
    {
        return false;
    }

    wxWinUICalendarImpl * const impl = control.m_winui.get();
    const auto state = control.m_winui->callbackState;
    const std::uint64_t generation = state->Generation();
    if ( state->GetOwner(generation) != &control )
        return false;
    impl->weekRefreshPendingPassesForTesting = count;
    return state->GetOwner(generation) == &control && control.m_winui &&
           control.m_winui.get() == impl && control.m_winui->callbackState == state;
}

bool wxWinUICalendarTestAccess::SetWeekRefreshPeerMutationPasses(
    wxCalendarCtrl& control,
    unsigned count)
{
    if ( count >
             wxWinUICalendarImpl::MaxWeekRefreshAutomaticRetries + 1 ||
         !control.m_winui || !control.m_winui->cal || !control.m_winui->callbackState )
    {
        return false;
    }

    wxWinUICalendarImpl * const impl = control.m_winui.get();
    const auto state = control.m_winui->callbackState;
    const std::uint64_t generation = state->Generation();
    if ( state->GetOwner(generation) != &control )
        return false;
    impl->weekRefreshPeerMutationPassesForTesting = count;
    return state->GetOwner(generation) == &control && control.m_winui &&
           control.m_winui.get() == impl && control.m_winui->callbackState == state;
}

bool wxWinUICalendarTestAccess::RequestDayLayoutValidationFailure(
    wxCalendarCtrl& control,
    DayLayoutValidationFailure failure)
{
    const unsigned failureMode = static_cast<unsigned>(failure);
    if ( failureMode <
             static_cast<unsigned>(
                 DayLayoutValidationFailure::Exception) ||
         failureMode >
             static_cast<unsigned>(
                 DayLayoutValidationFailure::QueueRefusal) )
    {
        return false;
    }
    if ( !control.m_winui || !control.m_winui->cal || !control.m_winui->callbackState )
        return false;

    wxWinUICalendarImpl * const impl = control.m_winui.get();
    const auto state = control.m_winui->callbackState;
    const std::uint64_t generation = state->Generation();
    if ( state->GetOwner(generation) != &control ||
         impl->weekDayLayoutValidationPending ||
         impl->weekDayLayoutValidationScheduled )
    {
        return false;
    }

    impl->weekDayLayoutValidationFaultsForTesting = 1;
    impl->weekDayLayoutValidationFailureModeForTesting = failureMode;
    impl->weekDayLayoutValidationPending = true;
    impl->QueueWeekDayLayoutValidation(&control);

    wxCalendarCtrl * const owner = state->GetOwner(generation);
    return owner == &control && owner->m_winui &&
           owner->m_winui.get() == impl &&
           owner->m_winui->callbackState == state;
}

bool wxWinUICalendarTestAccess::RequestDayLayoutValidation(wxCalendarCtrl& control)
{
    if ( !control.m_winui || !control.m_winui->cal || !control.m_winui->callbackState )
        return false;

    wxWinUICalendarImpl * const impl = control.m_winui.get();
    const auto state = control.m_winui->callbackState;
    const std::uint64_t generation = state->Generation();
    if ( state->GetOwner(generation) != &control ||
         impl->weekDayLayoutValidationPending ||
         impl->weekDayLayoutValidationScheduled )
    {
        return false;
    }

    impl->weekDayLayoutValidationPending = true;
    impl->QueueWeekDayLayoutValidation(&control);

    wxCalendarCtrl * const owner = state->GetOwner(generation);
    return owner == &control && owner->m_winui &&
           owner->m_winui.get() == impl &&
           owner->m_winui->callbackState == state;
}

bool wxWinUICalendarTestAccess::ResolveActiveMonthTopology(const std::vector<MonthTopologyDay>& days,
    MonthTopologyResult *resolved,
    int truncatedMonth,
    int truncatedFirstDay,
    int truncatedLastDay)
{
    if ( !resolved )
        return false;
    *resolved = {};

    std::vector<wxWinUICalendarImpl::MonthTopologyInput> inputs;
    inputs.reserve(days.size());
    for ( std::size_t n = 0; n != days.size(); ++n )
    {
        const MonthTopologyDay& day = days[n];
        inputs.push_back(
            wxWinUICalendarImpl::MonthTopologyInput{
                wxWinUICalendarMonthKey{
                    1, 2000, day.month, day.month != 0},
                day.calendarDay,
                day.daysInMonth,
                day.month * 100 + day.calendarDay,
                day.x,
                day.y,
                day.width,
                day.height,
                n,
                day.identity ? day.identity : n + 1,
                day.authoritative});
    }

    wxWinUICalendarImpl::TruncatedMonthAuthority truncated;
    const wxWinUICalendarImpl::TruncatedMonthAuthority *truncatedPtr =
        nullptr;
    if ( truncatedMonth != 0 )
    {
        const auto found = std::find_if(
            days.begin(), days.end(),
            [truncatedMonth](
                const MonthTopologyDay& day)
            {
                return day.month == truncatedMonth;
            });
        if ( found == days.end() )
            return false;
        truncated.month = wxWinUICalendarMonthKey{
            1, 2000, truncatedMonth, true};
        truncated.dayCount = found->daysInMonth;
        truncated.firstDay = truncatedFirstDay;
        truncated.lastDay = truncatedLastDay;
        truncated.valid = true;
        truncatedPtr = &truncated;
    }

    wxWinUICalendarImpl::MonthTopologyResult topology;
    if ( !wxWinUICalendarImpl::ResolveActiveMonthTopology(
             inputs, truncatedPtr, &topology) ||
         topology.sourceIndices.empty() )
    {
        return false;
    }

    const std::size_t firstIndex = topology.sourceIndices.front();
    if ( firstIndex >= days.size() )
        return false;
    resolved->month = topology.month.month;
    resolved->dayCount =
        static_cast<unsigned>(topology.sourceIndices.size());
    resolved->columnCount = topology.columnCount;
    resolved->rowCount = topology.rowCount;
    resolved->firstDayTop = days[firstIndex].y;
    return true;
}

bool wxWinUICalendarTestAccess::GetWeekRefreshRecovery(
    const wxCalendarCtrl& control,
    WeekRefreshRecovery *recovery)
{
    if ( !recovery )
        return false;
    *recovery = {};
    if ( !control.m_winui || !control.m_winui->cal || !control.m_winui->callbackState )
        return false;

    const wxWinUICalendarImpl * const impl = control.m_winui.get();
    const auto state = control.m_winui->callbackState;
    const std::uint64_t callbackGeneration = state->Generation();
    if ( state->GetOwner(callbackGeneration) != &control )
        return false;

    recovery->generation = impl->weekRefreshRetryGeneration;
    recovery->requestRevision = impl->weekRefreshRequestRevision;
    recovery->completedRevision = impl->weekRefreshCompletedRevision;
    recovery->automaticRetriesRemaining =
        impl->weekRefreshRetriesRemaining;
    recovery->injectedFailuresRemaining =
        impl->weekRefreshFaultsForTesting;
    recovery->injectedPendingPassesRemaining =
        impl->weekRefreshPendingPassesForTesting;
    recovery->injectedPeerMutationPassesRemaining =
        impl->weekRefreshPeerMutationPassesForTesting;
    recovery->injectedDayLayoutFailuresRemaining =
        impl->weekDayLayoutValidationFaultsForTesting;
    recovery->pendingCause =
        static_cast<unsigned>(impl->weekRefreshPendingCause);
    recovery->peerMutationDeferrals =
        impl->weekPeerMutationDeferralCount;
    recovery->boundedPendingRetries =
        impl->boundedWeekPendingRetryCount;
    recovery->invalidTicketDeferrals =
        impl->weekInvalidTicketDeferralCount;
    recovery->navigationGeneration =
        impl->navigationButtonGeneration;
    recovery->navigationResolveFailures =
        impl->navigationResolveFailureCount;
    recovery->navigationRetryExhaustions =
        impl->navigationRetryExhaustionCount;
    recovery->dayItemIdentityChanges =
        impl->dayItemIdentityChangeCount;
    recovery->dayItemRepeatNotifications =
        impl->dayItemRepeatNotificationCount;
    recovery->weekLayoutEchoAbsorptions =
        impl->weekLayoutEchoAbsorptionCount;
    recovery->dayLayoutFailureRecoveries =
        impl->weekDayLayoutValidationFailureRecoveryCount;
    recovery->projectedMonthDays =
        impl->projectedWeekMonthDayCount;
    recovery->projectedWeekdayColumns =
        impl->projectedWeekColumnCount;
    recovery->navigationSnapshotButtons =
        impl->navigationSnapshotButtonCount;
    recovery->navigationCandidates =
        impl->navigationCandidateCount;
    recovery->navigationFailure =
        static_cast<unsigned>(impl->navigationLastFailure);
    recovery->scheduled = impl->weekRefreshScheduled;
    recovery->degraded = impl->weekRefreshDegraded;
    recovery->waitingForMutationEnd =
        impl->weekRefreshWaitingForMutationEnd;
    recovery->navigationScheduled =
        impl->navigationRefreshScheduled;
    recovery->navigationBindingRequired =
        impl->navigationBindingRequired;
    recovery->navigationBound =
        impl->decrementButton && impl->incrementButton &&
        impl->decrementClickToken.value &&
        impl->incrementClickToken.value;
    recovery->dayLayoutValidationPending =
        impl->weekDayLayoutValidationPending;
    recovery->dayLayoutValidationScheduled =
        impl->weekDayLayoutValidationScheduled;
    return state->GetOwner(callbackGeneration) == &control && control.m_winui &&
           control.m_winui.get() == impl && control.m_winui->callbackState == state;
}

bool wxWinUICalendarTestAccess::GetStableLayoutTicket(
    const wxCalendarCtrl& control,
    LayoutTicket *ticket)
{
    if ( !ticket )
        return false;
    *ticket = {};

    if ( !control.m_winui || !control.m_winui->cal || !control.m_winui->layoutRoot ||
         !control.m_winui->callbackState )
    {
        return false;
    }

    wxWinUICalendarImpl * const impl = control.m_winui.get();
    const auto state = control.m_winui->callbackState;
    const std::uint64_t generation = state->Generation();
    wxCalendarCtrl * const owner = state->GetOwner(generation);
    if ( owner != &control || !owner->m_winui ||
         owner->m_winui.get() != impl ||
         owner->m_winui->callbackState != state ||
         state->IsPeerMutationInProgress() ||
         impl->refreshWeeksInProgress || impl->refreshWeeksPending ||
         impl->weekDayLayoutValidationPending ||
         impl->weekDayLayoutValidationScheduled ||
         impl->weekRefreshScheduled ||
         impl->weekRefreshDegraded ||
         impl->navigationRefreshScheduled ||
         impl->navigationBindingRequired ||
         impl->weekRefreshCompletedRevision !=
             impl->weekRefreshRequestRevision ||
         !impl->projectedWeekLayoutInputsValid ||
         !impl->decrementButton || !impl->incrementButton ||
         !impl->decrementClickToken.value ||
         !impl->incrementClickToken.value )
    {
        return false;
    }

    const MUXC::CalendarView calendar = impl->cal;
    const MUXCP::ButtonBase decrementButton = impl->decrementButton;
    const MUXCP::ButtonBase incrementButton = impl->incrementButton;
    const std::uint64_t layoutRevision = impl->layoutRevision;
    const std::uint64_t realizedDaysRevision =
        impl->realizedDaysRevision;
    const std::uint64_t weekRefreshRevision =
        impl->weekRefreshRequestRevision;
    const std::uint64_t navigationGeneration =
        impl->navigationButtonGeneration;
    const wxWinUICalendarImpl::WeekLayoutInputs projectedInputs =
        impl->projectedWeekLayoutInputs;

    try
    {
        wxWinUICalendarImpl::WeekLayoutInputs currentInputs;
        if ( !impl->CaptureWeekLayoutInputs(owner, &currentInputs) ||
             !wxWinUICalendarImpl::SameWeekLayoutInputs(
                 currentInputs, projectedInputs) )
        {
            return false;
        }

        const wxWinUICalendarImpl::HitTestTicket navigationTicket =
            impl->CaptureHitTestTicket();
        wxWinUICalendarImpl::PublicHitSnapshot navigationSnapshot;
        MUXCP::ButtonBase authenticatedDecrement{ nullptr };
        MUXCP::ButtonBase authenticatedIncrement{ nullptr };
        if ( !wxWinUICalendarImpl::CapturePublicHitSnapshot(
                 navigationTicket, &navigationSnapshot) ||
             !wxWinUICalendarImpl::ResolveNavigationButtons(
                 navigationTicket, navigationSnapshot,
                 &authenticatedDecrement,
                 &authenticatedIncrement) ||
             !wxWinUICalendarImpl::IsElementAttachedToCalendar(
                 navigationTicket,
                 authenticatedDecrement.as<MUX::FrameworkElement>()) ||
             !wxWinUICalendarImpl::IsElementAttachedToCalendar(
                 navigationTicket,
                 authenticatedIncrement.as<MUX::FrameworkElement>()) ||
             winrt::get_abi(authenticatedDecrement) !=
                 winrt::get_abi(decrementButton) ||
             winrt::get_abi(authenticatedIncrement) !=
                 winrt::get_abi(incrementButton) )
        {
            return false;
        }

        wxCalendarCtrl *liveOwner = state->GetOwner(generation);
        if ( liveOwner != &control || !liveOwner->m_winui ||
             liveOwner->m_winui.get() != impl ||
             liveOwner->m_winui->callbackState != state ||
             liveOwner->m_winui->cal != calendar )
        {
            return false;
        }

        const MUX::XamlRoot calendarRoot = calendar.XamlRoot();
        const MUX::FrameworkElement decrement =
            decrementButton.as<MUX::FrameworkElement>();
        const MUX::FrameworkElement increment =
            incrementButton.as<MUX::FrameworkElement>();
        if ( !calendarRoot || decrement.XamlRoot() != calendarRoot ||
             increment.XamlRoot() != calendarRoot ||
             decrement.Visibility() != MUX::Visibility::Visible ||
             increment.Visibility() != MUX::Visibility::Visible ||
             decrement.ActualWidth() <= 0 || decrement.ActualHeight() <= 0 ||
             increment.ActualWidth() <= 0 || increment.ActualHeight() <= 0 )
        {
            return false;
        }

        // Re-authenticate after the visibility/geometry reads above: those
        // are XAML boundaries and may have caused a template replacement.
        // Use the common helper so COM identity is compared on homogeneous
        // CalendarView projections and every ancestor read revalidates the
        // lifetime/revision ticket.
        if ( !wxWinUICalendarImpl::IsElementAttachedToCalendar(
                 navigationTicket, decrement) ||
             !wxWinUICalendarImpl::IsElementAttachedToCalendar(
                 navigationTicket, increment) )
            return false;

        liveOwner = state->GetOwner(generation);
        if ( liveOwner != &control || !liveOwner->m_winui ||
             liveOwner->m_winui.get() != impl ||
             liveOwner->m_winui->callbackState != state ||
             liveOwner->m_winui->cal != calendar ||
             impl->refreshWeeksInProgress || impl->refreshWeeksPending ||
             impl->weekDayLayoutValidationPending ||
             impl->weekDayLayoutValidationScheduled ||
             impl->weekRefreshScheduled ||
             impl->weekRefreshDegraded ||
             impl->navigationRefreshScheduled ||
             impl->navigationBindingRequired ||
             impl->weekRefreshCompletedRevision !=
                 impl->weekRefreshRequestRevision ||
             impl->layoutRevision != layoutRevision ||
             impl->realizedDaysRevision != realizedDaysRevision ||
             impl->weekRefreshRequestRevision != weekRefreshRevision ||
             impl->navigationButtonGeneration != navigationGeneration ||
             winrt::get_abi(impl->decrementButton) !=
                 winrt::get_abi(decrementButton) ||
             winrt::get_abi(impl->incrementButton) !=
                 winrt::get_abi(incrementButton) )
        {
            return false;
        }

        ticket->layoutRevision = layoutRevision;
        ticket->realizedDaysRevision = realizedDaysRevision;
        ticket->weekRefreshRevision = weekRefreshRevision;
        ticket->navigationGeneration = navigationGeneration;
        ticket->decrementIdentity = reinterpret_cast<std::uintptr_t>(
            winrt::get_abi(decrementButton));
        ticket->incrementIdentity = reinterpret_cast<std::uintptr_t>(
            winrt::get_abi(incrementButton));
        return ticket->decrementIdentity != 0 &&
               ticket->incrementIdentity != 0 &&
               ticket->decrementIdentity != ticket->incrementIdentity;
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException(
            "WinUI CalendarView stable layout test ticket", e);
        return false;
    }
}
#endif

#endif // wxUSE_CALENDARCTRL
