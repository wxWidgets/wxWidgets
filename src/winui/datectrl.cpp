/////////////////////////////////////////////////////////////////////////////
// Name:        src/winui/datectrl.cpp
// Purpose:     wxWinUI wxDatePickerCtrl implementation (CalendarDatePicker)
// Author:      wxWidgets development team
// Created:     2026-06-01
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_DATEPICKCTRL

#include "wx/datectrl.h"

#ifdef WXWINUI_TEST_SUPPORT
    #include "date-time-test-access.h"
#endif
#include "wx/dateevt.h"

#include "private.h"

#include <winrt/Microsoft.UI.Xaml.Automation.h>
#include <winrt/Microsoft.UI.Xaml.Input.h>
#include <winrt/Windows.Globalization.h>
#include <winrt/Windows.Globalization.DateTimeFormatting.h>
#include <winrt/Windows.Globalization.NumberFormatting.h>
#include <winrt/Windows.System.h>

namespace MUX = winrt::Microsoft::UI::Xaml;
namespace MUXA = winrt::Microsoft::UI::Xaml::Automation;
namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;
namespace MUXI = winrt::Microsoft::UI::Xaml::Input;
namespace WF = winrt::Windows::Foundation;
namespace WG = winrt::Windows::Globalization;
namespace WGDT = winrt::Windows::Globalization::DateTimeFormatting;
namespace WGNF = winrt::Windows::Globalization::NumberFormatting;
namespace WS = winrt::Windows::System;

#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <limits>
#include <vector>

namespace
{

#ifdef WXWINUI_TEST_SUPPORT
winrt::hstring gs_winuiDateLanguageForTesting;
#endif

WGNF::DecimalFormatter wxWinUICreateDateDecimalFormatter(
    const winrt::hstring& language)
{
    if ( language.empty() )
        return WGNF::DecimalFormatter();

    const WG::GeographicRegion region;
    return WGNF::DecimalFormatter(
        std::vector<winrt::hstring>{language},
        region.CodeTwoLetter());
}

wxDateTime wxWinUINormalizeCivilDate(const wxDateTime& value)
{
    if ( !value.IsValid() )
        return wxDefaultDateTime;

    wxDateTime date(value);
    date.ResetTime();
    return date;
}

wxString wxWinUIGetLocaleShortDatePattern(
    bool showCentury,
    const winrt::hstring& language)
{
    wxString pattern;
    try
    {
        const WGDT::DateTimeFormatter formatter = language.empty()
            ? WGDT::DateTimeFormatter::ShortDate()
            : WGDT::DateTimeFormatter(
                  L"shortdate",
                  std::vector<winrt::hstring>{language});
        const auto patterns = formatter.Patterns();
        if ( patterns.Size() )
            pattern = wxWinUIFromHString(patterns.GetAt(0));
    }
    catch ( const winrt::hresult_error& )
    {
    }

    if ( pattern.empty() )
        pattern = "{month.integer}/{day.integer}/{year.full}";

    if ( showCentury )
        pattern.Replace("year.abbreviated", "year.full");
    else
        pattern.Replace("year.full", "year.abbreviated");
    return pattern;
}

enum class wxWinUIDateLayoutKind
{
    Literal,
    Day,
    Month,
    Year,
    Era
};

struct wxWinUIDateLayoutToken
{
    wxWinUIDateLayoutKind kind = wxWinUIDateLayoutKind::Literal;
    wxString text;
};

std::vector<wxWinUIDateLayoutToken>
wxWinUIParseDatePattern(const wxString& pattern)
{
    std::vector<wxWinUIDateLayoutToken> result;
    wxString literal;
    const auto flushLiteral = [&]()
    {
        if ( !literal.empty() )
        {
            result.push_back(
                {wxWinUIDateLayoutKind::Literal, literal});
            literal.clear();
        }
    };

    for ( std::size_t pos = 0; pos < pattern.length(); )
    {
        if ( pattern[pos] != '{' )
        {
            literal += pattern[pos++];
            continue;
        }

        const std::size_t end = pattern.find('}', pos + 1);
        if ( end == wxString::npos )
        {
            literal += pattern.Mid(pos);
            break;
        }

        const wxString field = pattern.Mid(pos + 1, end - pos - 1);
        wxWinUIDateLayoutKind kind = wxWinUIDateLayoutKind::Literal;
        if ( field.StartsWith("day.") &&
             !field.StartsWith("dayofweek.") )
        {
            kind = wxWinUIDateLayoutKind::Day;
        }
        else if ( field.StartsWith("month.") )
        {
            kind = wxWinUIDateLayoutKind::Month;
        }
        else if ( field.StartsWith("year.") )
        {
            kind = wxWinUIDateLayoutKind::Year;
        }
        else if ( field.StartsWith("era.") )
        {
            kind = wxWinUIDateLayoutKind::Era;
        }

        if ( kind == wxWinUIDateLayoutKind::Literal )
        {
            // CalendarDatePicker can render any other locale-specific field
            // itself. Keep the editable spin surface limited to the public wx
            // civil components plus the real era label handled above.
            pos = end + 1;
            continue;
        }

        flushLiteral();
        result.push_back({kind, wxString()});
        pos = end + 1;
    }
    flushLiteral();

    const auto hasKind = [&result](wxWinUIDateLayoutKind kind)
    {
        return std::find_if(
                   result.begin(), result.end(),
                   [kind](const wxWinUIDateLayoutToken& token)
                   {
                       return token.kind == kind;
                   }) != result.end();
    };
    if ( !hasKind(wxWinUIDateLayoutKind::Day) ||
         !hasKind(wxWinUIDateLayoutKind::Month) ||
         !hasKind(wxWinUIDateLayoutKind::Year) )
    {
        result =
        {
            {wxWinUIDateLayoutKind::Month, wxString()},
            {wxWinUIDateLayoutKind::Literal, "/"},
            {wxWinUIDateLayoutKind::Day, wxString()},
            {wxWinUIDateLayoutKind::Literal, "/"},
            {wxWinUIDateLayoutKind::Year, wxString()}
        };
    }
    return result;
}

// CalendarDatePicker is an instant-based WinRT API representing a civil wx
// date. Anchor the civil date at local noon before crossing that boundary:
// noon cannot fall into the usual daylight-saving gap and converts back to
// the same local calendar day. In particular, never feed it the caller's time
// component or convert a date through local midnight/time_t.
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
        // Establish the safe local-noon anchor before changing the civil
        // fields. Setting Y/M/D while retaining the current wall clock can
        // cross a historical timezone gap (including pre-standard-time
        // transitions) before we ever get a chance to normalize the time.
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
                    const bool converted = civil.IsValid() &&
                        wxWinUIToDateTime(civil, &candidate);
                    const bool inRange = converted &&
                        candidate.time_since_epoch() >=
                            rawMinimum.time_since_epoch() &&
                        candidate.time_since_epoch() <=
                            rawMaximum.time_since_epoch();
                    if ( inRange )
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
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException(
            "WinUI DatePicker calendar-envelope resolution", e);
        return false;
    }
}

bool wxWinUIDateRef(const wxDateTime& value,
                    WF::IReference<WF::DateTime> *result)
{
    if ( !result )
        return false;

    if ( !value.IsValid() )
    {
        *result = nullptr;
        return true;
    }

    WF::DateTime peerValue{};
    if ( !wxWinUIToDateTime(value, &peerValue) )
        return false;

    *result = winrt::box_value(peerValue)
                  .as<WF::IReference<WF::DateTime>>();
    return true;
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

bool wxWinUIIsInsideRange(const wxDateTime& value,
                          const wxDateTime& minimum,
                          const wxDateTime& maximum)
{
    return !value.IsValid() ||
           ((!minimum.IsValid() || value >= minimum) &&
            (!maximum.IsValid() || value <= maximum));
}

void wxWinUIApplyDateBounds(
    const MUXC::CalendarDatePicker& picker,
    const WF::DateTime& minimum,
    const WF::DateTime& maximum)
{
    const WF::DateTime currentMinimum = picker.MinDate();
    const WF::DateTime currentMaximum = picker.MaxDate();

    // Avoid a transient invalid range while moving two disjoint intervals.
    if ( minimum.time_since_epoch() >
         currentMaximum.time_since_epoch() )
    {
        picker.MaxDate(maximum);
        picker.MinDate(minimum);
    }
    else if ( maximum.time_since_epoch() <
              currentMinimum.time_since_epoch() )
    {
        picker.MinDate(minimum);
        picker.MaxDate(maximum);
    }
    else
    {
        picker.MinDate(minimum);
        picker.MaxDate(maximum);
    }
}

class wxWinUIDateCallbackState
{
public:
    explicit wxWinUIDateCallbackState(wxDatePickerCtrl *owner)
        : m_owner(owner)
    {
    }

    std::uint64_t Generation() const
    {
        return m_generation.load(std::memory_order_acquire);
    }

    wxDatePickerCtrl *GetOwner(std::uint64_t generation) const
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

    void EndPeerMutation()
    {
        const unsigned previous =
            m_peerMutationDepth.fetch_sub(1, std::memory_order_acq_rel);
        wxASSERT_MSG(previous != 0,
                     "unbalanced WinUI date peer mutation");
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
    std::atomic<wxDatePickerCtrl *> m_owner;
    std::atomic<std::uint64_t> m_generation{1};
    std::atomic<unsigned> m_peerMutationDepth{0};
    std::atomic<std::uint64_t> m_stateRevision{1};
};

class wxWinUIDatePeerMutationGuard
{
public:
    explicit wxWinUIDatePeerMutationGuard(
        std::shared_ptr<wxWinUIDateCallbackState> state)
        : m_state(std::move(state))
    {
        if ( m_state )
            m_state->BeginPeerMutation();
    }

    ~wxWinUIDatePeerMutationGuard()
    {
        if ( m_state )
            m_state->EndPeerMutation();
    }

    wxWinUIDatePeerMutationGuard(
        const wxWinUIDatePeerMutationGuard&) = delete;
    wxWinUIDatePeerMutationGuard& operator=(
        const wxWinUIDatePeerMutationGuard&) = delete;

private:
    std::shared_ptr<wxWinUIDateCallbackState> m_state;
};

bool wxWinUIReadPeerDate(
    const WF::IReference<WF::DateTime>& reference,
    wxDateTime *result)
{
    if ( !result )
        return true;

    if ( !reference )
    {
        *result = wxDefaultDateTime;
        return true;
    }

    *result = wxWinUIFromDateTime(reference.Value());
    return result->IsValid();
}

int wxWinUIResolveTwoDigitYearMaximum(
    const winrt::hstring& calendarIdentifier,
    const winrt::hstring& language,
    int firstEra,
    int lastEra,
    const WF::DateTime& civilMinimum,
    const WF::DateTime& civilMaximum)
{
    // In a multiple-era calendar the adjacent ComboBox already supplies the
    // century-like discriminator and NumberBox contains the year within that
    // era. Expanding 07 to a Gregorian-style 2007 would make Japanese and
    // similar calendars impossible to edit.
    if ( firstEra != lastEra )
        return 99;

    DWORD gregorianMaximum = 2029;
    ::GetCalendarInfoEx(
        LOCALE_NAME_USER_DEFAULT, CAL_GREGORIAN, nullptr,
        CAL_ITWODIGITYEARMAX | CAL_RETURN_NUMBER,
        nullptr, 0, &gregorianMaximum);

    WF::DateTime cutoff{};
    const int civilYear =
        std::clamp(static_cast<int>(gregorianMaximum), 1, 9999);
    if ( !wxWinUIToDateTime(
             wxDateTime(31, wxDateTime::Dec, civilYear), &cutoff) )
    {
        return 99;
    }

    if ( cutoff.time_since_epoch() < civilMinimum.time_since_epoch() )
        cutoff = civilMinimum;
    else if ( cutoff.time_since_epoch() > civilMaximum.time_since_epoch() )
        cutoff = civilMaximum;

    try
    {
        WG::Calendar calendar = language.empty()
            ? WG::Calendar()
            : WG::Calendar(
                  std::vector<winrt::hstring>{language});
        calendar.ChangeCalendarSystem(calendarIdentifier);
        calendar.SetDateTime(cutoff);
        if ( calendar.Era() != firstEra )
            return 99;

        // The Gregorian user cutoff is an instant. Projecting that instant
        // through the active calendar preserves the same sliding 100-year
        // window for Hijri, Hebrew, Persian, Thai and offset calendars.
        return std::clamp(calendar.Year(), 99, 9999);
    }
    catch ( const winrt::hresult_error& )
    {
        return 99;
    }
}

class wxWinUITwoDigitYearFormatter
    : public winrt::implements<
          wxWinUITwoDigitYearFormatter,
          WGNF::INumberFormatter2,
          WGNF::INumberParser>
{
public:
    wxWinUITwoDigitYearFormatter(
        int maximumTwoDigitYear,
        const winrt::hstring& language)
        : m_maximumTwoDigitYear(maximumTwoDigitYear),
          m_formatter(wxWinUICreateDateDecimalFormatter(language))
    {
        m_formatter.IntegerDigits(2);
        m_formatter.FractionDigits(0);
    }

    winrt::hstring FormatInt(std::int64_t value) const
    {
        if ( value < 1 )
            return winrt::hstring();
        return m_formatter.FormatInt(value % 100);
    }

    winrt::hstring FormatUInt(std::uint64_t value) const
    {
        if ( value == 0 ||
             value > static_cast<std::uint64_t>(
                         std::numeric_limits<std::int64_t>::max()) )
        {
            return winrt::hstring();
        }
        return FormatInt(static_cast<std::int64_t>(value));
    }

    winrt::hstring FormatDouble(double value) const
    {
        if ( !std::isfinite(value) || value < 1 ||
             value > static_cast<double>(
                         std::numeric_limits<std::int64_t>::max()) )
        {
            return winrt::hstring();
        }
        return FormatInt(static_cast<std::int64_t>(std::llround(value)));
    }

    WF::IReference<std::int64_t>
    ParseInt(const winrt::hstring& text) const
    {
        std::int64_t value = 0;
        if ( !Parse(text, &value) )
            return nullptr;
        return winrt::box_value(value)
            .as<WF::IReference<std::int64_t>>();
    }

    WF::IReference<std::uint64_t>
    ParseUInt(const winrt::hstring& text) const
    {
        std::int64_t value = 0;
        if ( !Parse(text, &value) || value < 0 )
            return nullptr;
        return winrt::box_value(static_cast<std::uint64_t>(value))
            .as<WF::IReference<std::uint64_t>>();
    }

    WF::IReference<double>
    ParseDouble(const winrt::hstring& text) const
    {
        std::int64_t value = 0;
        if ( !Parse(text, &value) )
            return nullptr;
        return winrt::box_value(static_cast<double>(value))
            .as<WF::IReference<double>>();
    }

private:
    bool Parse(const winrt::hstring& input,
               std::int64_t *value) const
    {
        const WF::IReference<std::int64_t> parsedValue =
            m_formatter.ParseInt(input);
        if ( !parsedValue || parsedValue.Value() < 0 )
            return false;

        const std::int64_t parsed = parsedValue.Value();
        if ( parsed > std::numeric_limits<int>::max() )
            return false;
        int year = static_cast<int>(parsed);
        if ( parsed <= 99 )
        {
            const int century = m_maximumTwoDigitYear / 100;
            year = century * 100 + static_cast<int>(parsed);
            if ( year > m_maximumTwoDigitYear )
                year -= 100;
        }
        if ( year < 1 || year > 9999 )
            return false;

        if ( value )
            *value = year;
        return true;
    }

    int m_maximumTwoDigitYear;
    WGNF::DecimalFormatter m_formatter;
};

MUXC::NumberBox wxWinUICreateDatePart(double minimum,
                                      double maximum,
                                      double minimumWidth,
                                      unsigned integerDigits,
                                      const winrt::hstring& language)
{
    MUXC::NumberBox box;
    if ( !language.empty() )
        box.Language(language);
    box.Minimum(minimum);
    box.Maximum(maximum);
    box.SmallChange(1);
    box.LargeChange(1);
    box.AcceptsExpression(false);
    box.ValidationMode(
        MUXC::NumberBoxValidationMode::InvalidInputOverwritten);
    box.SpinButtonPlacementMode(
        MUXC::NumberBoxSpinButtonPlacementMode::Inline);
    WGNF::DecimalFormatter formatter =
        wxWinUICreateDateDecimalFormatter(language);
    formatter.IntegerDigits(integerDigits);
    formatter.FractionDigits(0);
    box.NumberFormatter(formatter);
    box.MinWidth(minimumWidth);
    return box;
}

void wxWinUIApplyNumberBounds(const MUXC::NumberBox& box,
                              double minimum,
                              double maximum)
{
    const double oldMinimum = box.Minimum();
    const double oldMaximum = box.Maximum();
    if ( minimum > oldMaximum )
    {
        box.Maximum(maximum);
        box.Minimum(minimum);
    }
    else if ( maximum < oldMinimum )
    {
        box.Minimum(minimum);
        box.Maximum(maximum);
    }
    else
    {
        box.Minimum(minimum);
        box.Maximum(maximum);
    }
}

bool wxWinUIReadIntegralPart(const MUXC::NumberBox& box,
                             int *value)
{
    if ( !box || !value )
        return false;

    const double peerValue = box.Value();
    if ( !std::isfinite(peerValue) )
        return false;

    const long long rounded = std::llround(peerValue);
    if ( rounded < std::numeric_limits<int>::min() ||
         rounded > std::numeric_limits<int>::max() )
    {
        return false;
    }

    *value = static_cast<int>(rounded);
    return true;
}

struct wxWinUIDatePartsSnapshot
{
    std::array<double, 3> values{};
    std::array<double, 3> minimums{};
    std::array<double, 3> maximums{};
    int eraSelectedIndex = -1;
    bool captured = false;
};

std::array<MUXC::NumberBox, 3> wxWinUIDateParts(
    const MUXC::NumberBox& year,
    const MUXC::NumberBox& month,
    const MUXC::NumberBox& day)
{
    return {{year, month, day}};
}

bool wxWinUICaptureDateParts(
    const MUXC::ComboBox& era,
    const MUXC::NumberBox& year,
    const MUXC::NumberBox& month,
    const MUXC::NumberBox& day,
    wxWinUIDatePartsSnapshot *snapshot)
{
    if ( !year || !month || !day || !snapshot )
        return false;

    const auto parts = wxWinUIDateParts(year, month, day);
    for ( std::size_t n = 0; n != parts.size(); ++n )
    {
        snapshot->values[n] = parts[n].Value();
        snapshot->minimums[n] = parts[n].Minimum();
        snapshot->maximums[n] = parts[n].Maximum();
    }
    if ( era )
        snapshot->eraSelectedIndex = era.SelectedIndex();
    snapshot->captured = true;
    return true;
}

void wxWinUIRestoreDateParts(
    const MUXC::ComboBox& era,
    const MUXC::NumberBox& year,
    const MUXC::NumberBox& month,
    const MUXC::NumberBox& day,
    const wxWinUIDatePartsSnapshot& snapshot)
{
    if ( !snapshot.captured )
        return;

    if ( era )
        era.SelectedIndex(snapshot.eraSelectedIndex);

    const auto parts = wxWinUIDateParts(year, month, day);
    for ( std::size_t n = 0; n != parts.size(); ++n )
    {
        wxWinUIApplyNumberBounds(
            parts[n], snapshot.minimums[n], snapshot.maximums[n]);
        parts[n].Value(snapshot.values[n]);
    }
}

struct wxWinUICalendarDateParts
{
    int era = 0;
    int year = 0;
    int month = 0;
    int day = 0;
};

bool wxWinUIReadCalendarDateParts(
    const winrt::hstring& calendarIdentifier,
    const WF::DateTime& date,
    wxWinUICalendarDateParts *parts,
    WG::Calendar *calendarResult = nullptr)
{
    if ( calendarIdentifier.empty() || !parts )
        return false;

    try
    {
        WG::Calendar calendar;
        calendar.ChangeCalendarSystem(calendarIdentifier);
        calendar.SetDateTime(date);
        parts->era = calendar.Era();
        parts->year = calendar.Year();
        parts->month = calendar.Month();
        parts->day = calendar.Day();
        if ( calendarResult )
            *calendarResult = calendar;
        return true;
    }
    catch ( const winrt::hresult_error& )
    {
        return false;
    }
}

bool wxWinUIFindCalendarEraAnchor(
    const winrt::hstring& calendarIdentifier,
    int era,
    const WF::DateTime& minimum,
    const WF::DateTime& maximum,
    WF::DateTime *anchor)
{
    if ( calendarIdentifier.empty() || !anchor ||
         minimum.time_since_epoch() > maximum.time_since_epoch() )
    {
        return false;
    }

    try
    {
        WG::Calendar calendar;
        calendar.ChangeCalendarSystem(calendarIdentifier);
        std::int64_t low = minimum.time_since_epoch().count();
        std::int64_t high = maximum.time_since_epoch().count();
        while ( low < high )
        {
            const std::int64_t middle =
                low + (high - low) / 2;
            calendar.SetDateTime(
                WF::DateTime{WF::TimeSpan{middle}});
            if ( calendar.Era() < era )
                low = middle + 1;
            else
                high = middle;
        }

        const WF::DateTime candidate{WF::TimeSpan{low}};
        calendar.SetDateTime(candidate);
        if ( calendar.Era() != era )
            return false;
        *anchor = candidate;
        return true;
    }
    catch ( const winrt::hresult_error& )
    {
        return false;
    }
}

bool wxWinUIFindCalendarEraEnd(
    const winrt::hstring& calendarIdentifier,
    int era,
    const WF::DateTime& minimum,
    const WF::DateTime& maximum,
    WF::DateTime *end)
{
    if ( calendarIdentifier.empty() || !end ||
         minimum.time_since_epoch() > maximum.time_since_epoch() )
    {
        return false;
    }

    try
    {
        WG::Calendar calendar;
        calendar.ChangeCalendarSystem(calendarIdentifier);
        calendar.SetDateTime(maximum);
        if ( calendar.Era() < era )
            return false;
        if ( calendar.Era() == era )
        {
            *end = maximum;
            return true;
        }

        std::int64_t low = minimum.time_since_epoch().count();
        std::int64_t high = maximum.time_since_epoch().count();
        while ( low < high )
        {
            const std::int64_t middle =
                low + (high - low) / 2;
            calendar.SetDateTime(
                WF::DateTime{WF::TimeSpan{middle}});
            if ( calendar.Era() <= era )
                low = middle + 1;
            else
                high = middle;
        }

        if ( low == minimum.time_since_epoch().count() )
            return false;
        const WF::DateTime candidate{WF::TimeSpan{low - 1}};
        calendar.SetDateTime(candidate);
        if ( calendar.Era() != era )
            return false;
        *end = candidate;
        return true;
    }
    catch ( const winrt::hresult_error& )
    {
        return false;
    }
}

bool wxWinUIApplyDateParts(
    const MUXC::ComboBox& era,
    const MUXC::NumberBox& year,
    const MUXC::NumberBox& month,
    const MUXC::NumberBox& day,
    int firstEra,
    const winrt::hstring& calendarIdentifier,
    const wxDateTime& value,
    const wxDateTime& effectiveMinimum,
    const wxDateTime& effectiveMaximum)
{
    if ( !year || !month || !day ||
         !effectiveMinimum.IsValid() ||
         !effectiveMaximum.IsValid() ||
         effectiveMinimum > effectiveMaximum )
    {
        return false;
    }

    WF::DateTime peerMinimum{};
    WF::DateTime peerMaximum{};
    if ( !wxWinUIToDateTime(effectiveMinimum, &peerMinimum) ||
         !wxWinUIToDateTime(effectiveMaximum, &peerMaximum) )
    {
        return false;
    }

    if ( !value.IsValid() )
    {
        wxWinUIApplyNumberBounds(year, 1, 9999);
        wxWinUIApplyNumberBounds(month, 1, 13);
        wxWinUIApplyNumberBounds(day, 1, 31);
        const double blank =
            std::numeric_limits<double>::quiet_NaN();
        if ( era )
            era.SelectedIndex(-1);
        year.Value(blank);
        month.Value(blank);
        day.Value(blank);
        return !std::isfinite(year.Value()) &&
               !std::isfinite(month.Value()) &&
               !std::isfinite(day.Value());
    }

    WF::DateTime peerValue{};
    wxWinUICalendarDateParts selectedParts;
    WG::Calendar selectedCalendar;
    if ( !wxWinUIToDateTime(value, &peerValue) ||
         !wxWinUIReadCalendarDateParts(
             calendarIdentifier, peerValue,
             &selectedParts, &selectedCalendar) )
    {
        return false;
    }

    WF::DateTime eraMinimum{};
    WF::DateTime eraMaximum{};
    wxWinUICalendarDateParts minimumParts;
    wxWinUICalendarDateParts maximumParts;
    if ( !wxWinUIFindCalendarEraAnchor(
             calendarIdentifier, selectedParts.era,
             peerMinimum, peerMaximum, &eraMinimum) ||
         !wxWinUIFindCalendarEraEnd(
             calendarIdentifier, selectedParts.era,
             peerMinimum, peerMaximum, &eraMaximum) ||
         !wxWinUIReadCalendarDateParts(
             calendarIdentifier, eraMinimum, &minimumParts) ||
         !wxWinUIReadCalendarDateParts(
             calendarIdentifier, eraMaximum, &maximumParts) )
    {
        return false;
    }

    const int selectedYear = selectedParts.year;
    const int selectedMonth = selectedParts.month;
    const int selectedDay = selectedParts.day;
    int minimumYear = selectedCalendar.FirstYearInThisEra();
    int maximumYear = selectedCalendar.LastYearInThisEra();
    minimumYear = wxMax(minimumYear, minimumParts.year);
    maximumYear = wxMin(maximumYear, maximumParts.year);
    if ( selectedYear < minimumYear || selectedYear > maximumYear )
        return false;

    int minimumMonth = selectedCalendar.FirstMonthInThisYear();
    int maximumMonth = selectedCalendar.LastMonthInThisYear();
    if ( selectedYear == minimumParts.year )
        minimumMonth = wxMax(minimumMonth, minimumParts.month);
    if ( selectedYear == maximumParts.year )
        maximumMonth = wxMin(maximumMonth, maximumParts.month);
    if ( selectedMonth < minimumMonth || selectedMonth > maximumMonth )
        return false;

    int minimumDay = selectedCalendar.FirstDayInThisMonth();
    int maximumDay = selectedCalendar.LastDayInThisMonth();
    if ( selectedYear == minimumParts.year &&
         selectedMonth == minimumParts.month )
        minimumDay = wxMax(minimumDay, minimumParts.day);
    if ( selectedYear == maximumParts.year &&
         selectedMonth == maximumParts.month )
        maximumDay = wxMin(maximumDay, maximumParts.day);
    if ( selectedDay < minimumDay || selectedDay > maximumDay )
        return false;

    // Broaden dependent fields before changing their values, then constrain
    // them to the exact civil interval. This avoids NumberBox coercing an old
    // 31st while a February value is being installed.
    wxWinUIApplyNumberBounds(year, 1, 9999);
    wxWinUIApplyNumberBounds(month, 1, 13);
    wxWinUIApplyNumberBounds(day, 1, 31);
    if ( era )
        era.SelectedIndex(selectedParts.era - firstEra);
    year.Value(selectedYear);
    month.Value(selectedMonth);
    day.Value(selectedDay);
    wxWinUIApplyNumberBounds(year, minimumYear, maximumYear);
    wxWinUIApplyNumberBounds(month, minimumMonth, maximumMonth);
    wxWinUIApplyNumberBounds(day, minimumDay, maximumDay);

    int actualYear = 0;
    int actualMonth = 0;
    int actualDay = 0;
    return wxWinUIReadIntegralPart(year, &actualYear) &&
           wxWinUIReadIntegralPart(month, &actualMonth) &&
           wxWinUIReadIntegralPart(day, &actualDay) &&
           actualYear == selectedYear &&
           actualMonth == selectedMonth &&
           actualDay == selectedDay &&
           (!era ||
            era.SelectedIndex() == selectedParts.era - firstEra);
}

enum class wxWinUIDatePartsState
{
    Invalid,
    Partial,
    Complete
};

wxWinUIDatePartsState wxWinUIReadDateParts(
    const MUXC::ComboBox& era,
    const MUXC::NumberBox& year,
    const MUXC::NumberBox& month,
    const MUXC::NumberBox& day,
    int firstEra,
    const winrt::hstring& calendarIdentifier,
    bool allowNone,
    const wxDateTime& minimum,
    const wxDateTime& maximum,
    wxDateTime *result)
{
    if ( !year || !month || !day || !result )
        return wxWinUIDatePartsState::Invalid;

    const double yearValue = year.Value();
    const double monthValue = month.Value();
    const double dayValue = day.Value();
    const bool yearBlank = !std::isfinite(yearValue);
    const bool monthBlank = !std::isfinite(monthValue);
    const bool dayBlank = !std::isfinite(dayValue);
    const bool eraBlank = era && era.SelectedIndex() < 0;
    if ( yearBlank || monthBlank || dayBlank || eraBlank )
    {
        if ( allowNone && yearBlank && monthBlank && dayBlank &&
             (!era || eraBlank) )
        {
            *result = wxDefaultDateTime;
            return wxWinUIDatePartsState::Complete;
        }
        return allowNone
                   ? wxWinUIDatePartsState::Partial
                   : wxWinUIDatePartsState::Invalid;
    }

    int selectedYear = 0;
    int selectedMonth = 0;
    int selectedDay = 0;
    if ( !wxWinUIReadIntegralPart(year, &selectedYear) ||
         !wxWinUIReadIntegralPart(month, &selectedMonth) ||
         !wxWinUIReadIntegralPart(day, &selectedDay) ||
         selectedYear < 1 || selectedMonth < 1 || selectedMonth > 13 )
    {
        return wxWinUIDatePartsState::Invalid;
    }

    if ( !minimum.IsValid() || !maximum.IsValid() ||
         minimum > maximum )
    {
        return wxWinUIDatePartsState::Invalid;
    }

    WF::DateTime peerMinimum{};
    WF::DateTime peerMaximum{};
    if ( !wxWinUIToDateTime(minimum, &peerMinimum) ||
         !wxWinUIToDateTime(maximum, &peerMaximum) )
    {
        return wxWinUIDatePartsState::Invalid;
    }

    wxWinUICalendarDateParts minimumParts;
    wxWinUICalendarDateParts maximumParts;
    if ( !wxWinUIReadCalendarDateParts(
             calendarIdentifier, peerMinimum, &minimumParts) ||
         !wxWinUIReadCalendarDateParts(
             calendarIdentifier, peerMaximum, &maximumParts) )
    {
        return wxWinUIDatePartsState::Invalid;
    }

    const int selectedEra = era
        ? firstEra + era.SelectedIndex()
        : minimumParts.era;
    if ( selectedEra < minimumParts.era ||
         selectedEra > maximumParts.era ||
         (!era && minimumParts.era != maximumParts.era) )
    {
        return wxWinUIDatePartsState::Invalid;
    }

    WF::DateTime anchor{};
    if ( !wxWinUIFindCalendarEraAnchor(
             calendarIdentifier, selectedEra,
             peerMinimum, peerMaximum, &anchor) )
    {
        return wxWinUIDatePartsState::Invalid;
    }

    WF::DateTime eraEnd{};
    if ( !wxWinUIFindCalendarEraEnd(
             calendarIdentifier, selectedEra,
             peerMinimum, peerMaximum, &eraEnd) )
    {
        return wxWinUIDatePartsState::Invalid;
    }

    wxWinUICalendarDateParts eraMinimumParts;
    wxWinUICalendarDateParts eraMaximumParts;
    if ( !wxWinUIReadCalendarDateParts(
             calendarIdentifier, anchor, &eraMinimumParts) ||
         !wxWinUIReadCalendarDateParts(
             calendarIdentifier, eraEnd, &eraMaximumParts) ||
         selectedYear < eraMinimumParts.year ||
         selectedYear > eraMaximumParts.year ||
         (selectedYear == eraMinimumParts.year &&
          selectedMonth < eraMinimumParts.month) ||
         (selectedYear == eraMaximumParts.year &&
          selectedMonth > eraMaximumParts.month) )
    {
        return wxWinUIDatePartsState::Invalid;
    }

    wxDateTime value;
    try
    {
        WG::Calendar calendar;
        calendar.ChangeCalendarSystem(calendarIdentifier);
        calendar.SetDateTime(anchor);
        calendar.AddYears(selectedYear - calendar.Year());
        if ( calendar.Era() != selectedEra ||
             calendar.Year() != selectedYear )
        {
            // An era may end before the anniversary of its first day (for
            // example Showa 64). Anchor from its end for that partial final
            // year instead.
            calendar.SetDateTime(eraEnd);
            calendar.AddYears(selectedYear - calendar.Year());
            if ( calendar.Era() != selectedEra ||
                 calendar.Year() != selectedYear )
            {
                return wxWinUIDatePartsState::Invalid;
            }
        }

        int firstMonth = calendar.FirstMonthInThisYear();
        int lastMonth = calendar.LastMonthInThisYear();
        if ( selectedYear == eraMinimumParts.year )
            firstMonth = wxMax(firstMonth, eraMinimumParts.month);
        if ( selectedYear == eraMaximumParts.year )
            lastMonth = wxMin(lastMonth, eraMaximumParts.month);
        if ( selectedMonth < firstMonth || selectedMonth > lastMonth )
            return wxWinUIDatePartsState::Invalid;
        calendar.AddMonths(selectedMonth - calendar.Month());
        if ( calendar.Era() != selectedEra ||
             calendar.Year() != selectedYear ||
             calendar.Month() != selectedMonth )
        {
            return wxWinUIDatePartsState::Invalid;
        }

        int firstDay = calendar.FirstDayInThisMonth();
        int lastDay = calendar.LastDayInThisMonth();
        if ( selectedYear == eraMinimumParts.year &&
             selectedMonth == eraMinimumParts.month )
        {
            firstDay = wxMax(firstDay, eraMinimumParts.day);
        }
        if ( selectedYear == eraMaximumParts.year &&
             selectedMonth == eraMaximumParts.month )
        {
            lastDay = wxMin(lastDay, eraMaximumParts.day);
        }
        selectedDay = wxClip(selectedDay, firstDay, lastDay);
        calendar.Day(selectedDay);
        if ( calendar.Era() != selectedEra ||
             calendar.Year() != selectedYear ||
             calendar.Month() != selectedMonth ||
             calendar.Day() != selectedDay )
        {
            return wxWinUIDatePartsState::Invalid;
        }
        value = wxWinUIFromDateTime(calendar.GetDateTime());
    }
    catch ( const winrt::hresult_error& )
    {
        return wxWinUIDatePartsState::Invalid;
    }
    if ( !value.IsValid() )
        return wxWinUIDatePartsState::Invalid;
    if ( minimum.IsValid() && value < minimum )
        value = minimum;
    if ( maximum.IsValid() && value > maximum )
        value = maximum;
    *result = value;
    return wxWinUIDatePartsState::Complete;
}

} // namespace

class wxWinUIDatePickerImpl
{
public:
    ~wxWinUIDatePickerImpl()
    {
        Close();
    }

    void Close()
    {
        if ( callbackState )
            callbackState->Invalidate();

        if ( picker && dateChangedToken.value )
        {
            try
            {
                picker.DateChanged(dateChangedToken);
            }
            catch ( const winrt::hresult_error& e )
            {
                wxWinUILogException(
                    "WinUI CalendarDatePicker DateChanged removal", e);
            }
        }
        dateChangedToken = {};

        const auto revokePart =
            [](const MUXC::NumberBox& part,
               winrt::event_token& token,
               const char *operation)
            {
                if ( part && token.value )
                {
                    try
                    {
                        part.ValueChanged(token);
                    }
                    catch ( const winrt::hresult_error& e )
                    {
                        wxWinUILogException(operation, e);
                    }
                }
                token = {};
            };
        revokePart(day, dayChangedToken,
                   "WinUI DatePicker day ValueChanged removal");
        revokePart(month, monthChangedToken,
                   "WinUI DatePicker month ValueChanged removal");
        revokePart(year, yearChangedToken,
                   "WinUI DatePicker year ValueChanged removal");

        if ( era && eraChangedToken.value )
        {
            try
            {
                era.SelectionChanged(eraChangedToken);
            }
            catch ( const winrt::hresult_error& e )
            {
                wxWinUILogException(
                    "WinUI DatePicker era SelectionChanged removal", e);
            }
        }
        eraChangedToken = {};

        if ( clear && clearToken.value )
        {
            try
            {
                clear.Click(clearToken);
            }
            catch ( const winrt::hresult_error& e )
            {
                wxWinUILogException(
                    "WinUI DatePicker clear Click removal", e);
            }
        }
        clearToken = {};

        for ( std::size_t n = 0; n != focusParts.size(); ++n )
        {
            if ( focusParts[n] && n < focusKeyTokens.size() &&
                 focusKeyTokens[n].value )
            {
                try
                {
                    focusParts[n].PreviewKeyDown(focusKeyTokens[n]);
                }
                catch ( const winrt::hresult_error& e )
                {
                    wxWinUILogException(
                        "WinUI DatePicker PreviewKeyDown removal", e);
                }
            }
        }
        focusKeyTokens.clear();
        focusParts.clear();

        if ( layoutRoot && layoutLoadedToken.value )
        {
            try
            {
                layoutRoot.Loaded(layoutLoadedToken);
            }
            catch ( const winrt::hresult_error& e )
            {
                wxWinUILogException(
                    "WinUI DatePicker layout Loaded removal", e);
            }
        }
        layoutLoadedToken = {};

        host.Close();
        layoutRoot = nullptr;
        spinRoot = nullptr;
        nullPlaceholder = nullptr;
        era = nullptr;
        clear = nullptr;
        day = nullptr;
        month = nullptr;
        year = nullptr;
        picker = nullptr;
        callbackState.reset();
        defaultsCaptured = false;
        spinStyle = false;
        layoutXamlRoot = nullptr;
        initialSizePending = false;
        initialSizeProjectionComplete = false;
        suppressNextMinSizeWriter = false;
        suppressNextSizeWriter = false;
    }

    wxWinUIControlHost host;
    std::shared_ptr<wxWinUIDateCallbackState> callbackState;
    MUXC::CalendarDatePicker picker{ nullptr };
    MUXC::Grid spinRoot{ nullptr };
    MUXC::TextBlock nullPlaceholder{ nullptr };
    MUXC::ComboBox era{ nullptr };
    MUXC::NumberBox day{ nullptr };
    MUXC::NumberBox month{ nullptr };
    MUXC::NumberBox year{ nullptr };
    MUXC::Button clear{ nullptr };
    MUX::FrameworkElement layoutRoot{ nullptr };
    MUX::XamlRoot layoutXamlRoot{ nullptr };
    winrt::event_token dateChangedToken{};
    winrt::event_token dayChangedToken{};
    winrt::event_token monthChangedToken{};
    winrt::event_token yearChangedToken{};
    winrt::event_token eraChangedToken{};
    winrt::event_token clearToken{};
    winrt::event_token layoutLoadedToken{};
    std::vector<MUXC::Control> focusParts;
    std::vector<winrt::event_token> focusKeyTokens;
    WF::DateTime defaultMinimum{};
    WF::DateTime defaultMaximum{};
    WF::DateTime civilMinimum{};
    WF::DateTime civilMaximum{};
    winrt::hstring calendarIdentifier;
    int firstEra = 0;
    int lastEra = 0;
    wxString localePattern;
    wxString nullText;
    bool defaultsCaptured = false;
    bool spinStyle = false;
    bool initialSizePending = true;
    bool initialSizeProjectionComplete = false;
    bool suppressNextMinSizeWriter = false;
    bool suppressNextSizeWriter = false;
    std::uint64_t sizeWriterRevision = 1;
    std::uint64_t initialSizeWriterRevision = 0;
    std::uint64_t layoutRevision = 1;
    wxSize initialSizeRequest = wxDefaultSize;
    wxSize initialMinSize = wxDefaultSize;
};

wxIMPLEMENT_DYNAMIC_CLASS(wxDatePickerCtrl, wxControl);

#ifdef WXWINUI_TEST_SUPPORT
wxString wxWinUIDatePickerTestAccess::SetLanguage(const wxString& language)
{
    const wxString previous =
        wxWinUIFromHString(gs_winuiDateLanguageForTesting);
    gs_winuiDateLanguageForTesting = wxWinUIToHString(language);
    return previous;
}
#endif

wxDatePickerCtrl::wxDatePickerCtrl()
{
}

wxDatePickerCtrl::wxDatePickerCtrl(wxWindow *parent,
                                   wxWindowID id,
                                   const wxDateTime& dt,
                                   const wxPoint& pos,
                                   const wxSize& size,
                                   long style,
                                   const wxValidator& validator,
                                   const wxString& name)
{
    Create(parent, id, dt, pos, size, style, validator, name);
}

wxDatePickerCtrl::~wxDatePickerCtrl()
{
    if ( m_winui )
        m_winui->Close();
}

void wxDatePickerCtrl::SetMinSize(const wxSize& minSize)
{
    if ( m_winui )
    {
        if ( m_winui->suppressNextMinSizeWriter )
        {
            // Consume this allowance before entering wx code: a nested
            // application SetMinSize() is a new writer and must win.
            m_winui->suppressNextMinSizeWriter = false;
        }
        else if ( ++m_winui->sizeWriterRevision == 0 )
        {
            ++m_winui->sizeWriterRevision;
        }
    }
    wxDatePickerCtrlBase::SetMinSize(minSize);
}

void wxDatePickerCtrl::DoSetSize(int x, int y, int width, int height,
                                 int sizeFlags)
{
    if ( m_winui )
    {
        if ( m_winui->suppressNextSizeWriter )
        {
            // As above, consume before the native size boundary so a nested
            // SetSize() is never mistaken for our initial projection.
            m_winui->suppressNextSizeWriter = false;
        }
        else if ( ++m_winui->sizeWriterRevision == 0 )
        {
            ++m_winui->sizeWriterRevision;
        }
    }
    wxDatePickerCtrlBase::DoSetSize(
        x, y, width, height, sizeFlags);
}

bool wxDatePickerCtrl::ApplyInitialSizeTransaction(
    const wxSize& size, std::uint64_t expectedRevision)
{
    if ( !m_winui || !m_winui->callbackState )
        return false;

    wxWinUIDatePickerImpl * const impl = m_winui.get();
    const auto state = m_winui->callbackState;
    const std::uint64_t generation = state->Generation();
    const auto getExactOwner = [&]() -> wxDatePickerCtrl *
    {
        wxDatePickerCtrl * const owner = state->GetOwner(generation);
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
    wxDatePickerCtrl *owner = getExactOwner();
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

void wxDatePickerCtrl::OnLayoutLoaded()
{
    if ( !m_winui || !m_winui->callbackState || !m_winui->layoutRoot )
        return;

    wxWinUIDatePickerImpl * const impl = m_winui.get();
    const auto state = m_winui->callbackState;
    const std::uint64_t generation = state->Generation();
    const MUX::FrameworkElement layoutRoot = m_winui->layoutRoot;
    const auto getExactOwner = [&]() -> wxDatePickerCtrl *
    {
        wxDatePickerCtrl * const owner = state->GetOwner(generation);
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
        wxWinUILogException("WinUI DatePicker Loaded XamlRoot", e);
        return;
    }

    wxDatePickerCtrl *owner = getExactOwner();
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

    // This is the last operation: it crosses normal wx sizing callbacks and
    // may destroy or reparent the control.
    owner->ApplyInitialSizeTransaction(
        impl->initialSizeRequest, impl->initialSizeWriterRevision);
}

bool wxDatePickerCtrl::Create(wxWindow *parent,
                              wxWindowID id,
                              const wxDateTime& dt,
                              const wxPoint& pos,
                              const wxSize& size,
                              long style,
                              const wxValidator& validator,
                              const wxString& name)
{
    style = (style & ~wxBORDER_MASK) | wxBORDER_NONE;
    if ( !(style & wxDP_DROPDOWN) )
        style |= wxDP_SPIN;

    if ( !wxControl::Create(parent, id, pos, size, style, validator, name) )
        return false;

    m_rangeMin = wxDefaultDateTime;
    m_rangeMax = wxDefaultDateTime;
    if ( dt.IsValid() || (style & wxDP_ALLOWNONE) )
        m_value = wxWinUINormalizeCivilDate(dt);
    else
        m_value = wxDateTime::Today();

    m_winui.reset(new wxWinUIDatePickerImpl);
    m_winui->callbackState =
        std::make_shared<wxWinUIDateCallbackState>(this);
    wxWinUIDatePickerImpl * const createImpl = m_winui.get();
    const std::shared_ptr<wxWinUIDateCallbackState> callbackState =
        m_winui->callbackState;
    const std::uint64_t generation = callbackState->Generation();
    if ( !m_winui->host.Initialize(this) )
        return false;

    MUXC::CalendarDatePicker createPeer{ nullptr };
    try
    {
        m_winui->picker = MUXC::CalendarDatePicker();
        createPeer = m_winui->picker;
        winrt::hstring language;
#ifdef WXWINUI_TEST_SUPPORT
        language = gs_winuiDateLanguageForTesting;
#endif
        if ( !language.empty() )
        {
            m_winui->picker.Language(language);
            const WG::Calendar localeCalendar(
                std::vector<winrt::hstring>{language});
            m_winui->picker.CalendarIdentifier(
                localeCalendar.GetCalendarSystem());
        }
        // Use the resolved ShortDate pattern, not the approximate IDATE/SDATE
        // pair. This preserves locale literals, field widths and any era
        // marker understood by CalendarDatePicker. wxDP_SHOWCENTURY changes
        // only the year token width.
        m_winui->localePattern = wxWinUIGetLocaleShortDatePattern(
            (style & wxDP_SHOWCENTURY) != 0, language);
        m_winui->picker.DateFormat(
            wxWinUIToHString(m_winui->localePattern));
        m_winui->spinStyle = (style & wxDP_DROPDOWN) == 0;
        m_winui->defaultMinimum = m_winui->picker.MinDate();
        m_winui->defaultMaximum = m_winui->picker.MaxDate();
        m_winui->calendarIdentifier =
            m_winui->picker.CalendarIdentifier();
        if ( !wxWinUIResolveCalendarEnvelope(
                 m_winui->calendarIdentifier,
                 &m_winui->civilMinimum,
                 &m_winui->civilMaximum) )
        {
            m_winui->Close();
            return false;
        }
        m_winui->nullText = wxWinUIFromHString(
            m_winui->picker.PlaceholderText());
        m_winui->defaultsCaptured = true;

        m_winui->dateChangedToken = m_winui->picker.DateChanged(
            [callbackState, generation](
                MUXC::CalendarDatePicker const&,
                MUXC::CalendarDatePickerDateChangedEventArgs const&)
            {
                wxDatePickerCtrl * const owner =
                    callbackState->GetOwner(generation);
                if ( !owner || !owner->m_winui ||
                     owner->m_winui->callbackState != callbackState ||
                     callbackState->IsPeerMutationInProgress() )
                {
                    return;
                }

                owner->OnPeerDateChanged();
            });

        if ( m_winui->spinStyle )
        {
            m_winui->day = wxWinUICreateDatePart(
                1, 31, 64, 1, language);
            m_winui->month = wxWinUICreateDatePart(
                1, 13, 64, 1, language);
            m_winui->year = wxWinUICreateDatePart(
                1, 9999, 88,
                (style & wxDP_SHOWCENTURY) ? 4 : 2,
                language);
            MUXA::AutomationProperties::SetName(
                m_winui->day, wxWinUIToHString(_("Day")));
            MUXA::AutomationProperties::SetName(
                m_winui->month, wxWinUIToHString(_("Month")));
            MUXA::AutomationProperties::SetName(
                m_winui->year, wxWinUIToHString(_("Year")));
            WG::Calendar eraCalendar = language.empty()
                ? WG::Calendar()
                : WG::Calendar(
                      std::vector<winrt::hstring>{language});
            eraCalendar.ChangeCalendarSystem(
                m_winui->calendarIdentifier);
            m_winui->firstEra = eraCalendar.FirstEra();
            m_winui->lastEra = eraCalendar.LastEra();
            if ( !(style & wxDP_SHOWCENTURY) )
            {
                m_winui->year.NumberFormatter(
                    winrt::make<wxWinUITwoDigitYearFormatter>(
                        wxWinUIResolveTwoDigitYearMaximum(
                            m_winui->calendarIdentifier,
                            language,
                            m_winui->firstEra,
                            m_winui->lastEra,
                            m_winui->civilMinimum,
                            m_winui->civilMaximum),
                        language));
            }
            const bool patternHasEra =
                m_winui->localePattern.Find("{era.") != wxNOT_FOUND;
            if ( patternHasEra ||
                 m_winui->firstEra != m_winui->lastEra )
            {
                m_winui->era = MUXC::ComboBox();
                if ( !language.empty() )
                    m_winui->era.Language(language);
                m_winui->era.MinWidth(72);
                MUXA::AutomationProperties::SetName(
                    m_winui->era, wxWinUIToHString(_("Era")));
                for ( int eraValue = m_winui->firstEra;
                      eraValue <= m_winui->lastEra;
                      ++eraValue )
                {
                    winrt::hstring label;
                    WF::DateTime anchor{};
                    if ( wxWinUIFindCalendarEraAnchor(
                             m_winui->calendarIdentifier,
                             eraValue,
                             m_winui->civilMinimum,
                             m_winui->civilMaximum,
                             &anchor) )
                    {
                        eraCalendar.SetDateTime(anchor);
                        label = eraCalendar.EraAsString();
                    }
                    if ( label.empty() )
                    {
                        label = wxWinUIToHString(
                            wxString::Format("%d", eraValue));
                    }
                    m_winui->era.Items().Append(
                        winrt::box_value(label));
                }
            }
            m_winui->spinRoot = MUXC::Grid();
            if ( !language.empty() )
                m_winui->spinRoot.Language(language);

            const auto spinChanged =
                [callbackState, generation](
                    MUXC::NumberBox const&,
                    MUXC::NumberBoxValueChangedEventArgs const&)
                {
                    wxDatePickerCtrl * const owner =
                        callbackState->GetOwner(generation);
                    if ( !owner || !owner->m_winui ||
                         owner->m_winui->callbackState != callbackState ||
                         callbackState->IsPeerMutationInProgress() )
                    {
                        return;
                    }

                    owner->OnPeerDateChanged();
                };
            m_winui->dayChangedToken =
                m_winui->day.ValueChanged(spinChanged);
            m_winui->monthChangedToken =
                m_winui->month.ValueChanged(spinChanged);
            m_winui->yearChangedToken =
                m_winui->year.ValueChanged(spinChanged);
            if ( m_winui->era )
            {
                m_winui->eraChangedToken =
                    m_winui->era.SelectionChanged(
                        [callbackState, generation](
                            WF::IInspectable const&,
                            MUXC::SelectionChangedEventArgs const&)
                        {
                            wxDatePickerCtrl * const owner =
                                callbackState->GetOwner(generation);
                            if ( !owner || !owner->m_winui ||
                                 owner->m_winui->callbackState !=
                                     callbackState ||
                                 callbackState->IsPeerMutationInProgress() )
                            {
                                return;
                            }
                            owner->OnPeerDateChanged();
                        });
            }

            // Reuse the same complete resolved pattern for the inline peer.
            // The fields remain independent NumberBoxes, which gives
            // wxDP_SPIN actual keyboard, pointer, touch and UIA increment /
            // decrement behaviour.
            auto layout =
                wxWinUIParseDatePattern(m_winui->localePattern);
            if ( m_winui->era &&
                 std::find_if(
                     layout.begin(), layout.end(),
                     [](const wxWinUIDateLayoutToken& token)
                     {
                         return token.kind ==
                             wxWinUIDateLayoutKind::Era;
                     }) == layout.end() )
            {
                layout.insert(
                    layout.begin(),
                    {wxWinUIDateLayoutKind::Literal, " "});
                layout.insert(
                    layout.begin(),
                    {wxWinUIDateLayoutKind::Era, wxString()});
            }
            int column = 0;
            for ( const wxWinUIDateLayoutToken& token : layout )
            {
                MUXC::ColumnDefinition tokenColumn;
                tokenColumn.Width(MUX::GridLengthHelper::Auto());
                m_winui->spinRoot.ColumnDefinitions().Append(tokenColumn);

                if ( token.kind == wxWinUIDateLayoutKind::Literal )
                {
                    MUXC::TextBlock literal;
                    literal.Text(wxWinUIToHString(token.text));
                    literal.Margin(MUX::Thickness{2, 0, 2, 0});
                    literal.VerticalAlignment(
                        MUX::VerticalAlignment::Center);
                    MUXC::Grid::SetColumn(literal, column);
                    m_winui->spinRoot.Children().Append(literal);
                }
                else
                {
                    if ( token.kind == wxWinUIDateLayoutKind::Era )
                    {
                        if ( m_winui->era )
                        {
                            m_winui->era.Margin(
                                MUX::Thickness{2, 0, 2, 0});
                            MUXC::Grid::SetColumn(
                                m_winui->era, column);
                            m_winui->spinRoot.Children().Append(
                                m_winui->era);
                            m_winui->focusParts.push_back(
                                m_winui->era.as<MUXC::Control>());
                        }
                        ++column;
                        continue;
                    }

                    MUXC::NumberBox part{ nullptr };
                    switch ( token.kind )
                    {
                        case wxWinUIDateLayoutKind::Day:
                            part = m_winui->day;
                            break;
                        case wxWinUIDateLayoutKind::Month:
                            part = m_winui->month;
                            break;
                        case wxWinUIDateLayoutKind::Year:
                            part = m_winui->year;
                            break;
                        case wxWinUIDateLayoutKind::Era:
                            break;
                        case wxWinUIDateLayoutKind::Literal:
                            break;
                    }
                    if ( part )
                    {
                        MUXC::Grid::SetColumn(part, column);
                        m_winui->spinRoot.Children().Append(part);
                        m_winui->focusParts.push_back(
                            part.as<MUXC::Control>());
                    }
                }
                ++column;
            }

            if ( style & wxDP_ALLOWNONE )
            {
                const int valueColumnCount = column;
                MUXC::ColumnDefinition clearColumn;
                clearColumn.Width(MUX::GridLengthHelper::Auto());
                m_winui->spinRoot.ColumnDefinitions().Append(clearColumn);
                m_winui->clear = MUXC::Button();
                m_winui->clear.Content(
                    winrt::box_value(wxWinUIToHString(_("Clear"))));
                MUXA::AutomationProperties::SetName(
                    m_winui->clear, wxWinUIToHString(_("Clear")));
                m_winui->clear.Margin(MUX::Thickness{4, 0, 0, 0});
                m_winui->clear.MinWidth(52);
                MUXC::Grid::SetColumn(m_winui->clear, column);
                m_winui->clearToken = m_winui->clear.Click(
                    [callbackState, generation](
                        WF::IInspectable const&,
                        MUX::RoutedEventArgs const&)
                    {
                        wxDatePickerCtrl * const owner =
                            callbackState->GetOwner(generation);
                        if ( !owner || !owner->m_winui ||
                             owner->m_winui->callbackState !=
                                 callbackState ||
                             callbackState->IsPeerMutationInProgress() )
                        {
                            return;
                        }
                        owner->ClearPeerDateAndNotify();
                    });
                m_winui->spinRoot.Children().Append(m_winui->clear);
                m_winui->focusParts.push_back(
                    m_winui->clear.as<MUXC::Control>());

                m_winui->nullPlaceholder = MUXC::TextBlock();
                m_winui->nullPlaceholder.Text(
                    wxWinUIToHString(m_winui->nullText));
                m_winui->nullPlaceholder.Margin(
                    MUX::Thickness{8, 0, 4, 0});
                m_winui->nullPlaceholder.VerticalAlignment(
                    MUX::VerticalAlignment::Center);
                m_winui->nullPlaceholder.IsHitTestVisible(false);
                m_winui->nullPlaceholder.Visibility(
                    m_value.IsValid()
                        ? MUX::Visibility::Collapsed
                        : MUX::Visibility::Visible);
                MUXC::Grid::SetColumn(m_winui->nullPlaceholder, 0);
                MUXC::Grid::SetColumnSpan(
                    m_winui->nullPlaceholder,
                    wxMax(1, valueColumnCount));
                m_winui->spinRoot.Children().Append(
                    m_winui->nullPlaceholder);
            }

            for ( const MUXC::Control& part : m_winui->focusParts )
            {
                m_winui->focusKeyTokens.push_back(
                    part.PreviewKeyDown(
                        [callbackState, generation](
                            WF::IInspectable const& sender,
                            MUXI::KeyRoutedEventArgs const& args)
                        {
                            const WS::VirtualKey key = args.Key();
                            if ( key != WS::VirtualKey::Left &&
                                 key != WS::VirtualKey::Right )
                            {
                                return;
                            }

                            wxDatePickerCtrl * const owner =
                                callbackState->GetOwner(generation);
                            if ( !owner || !owner->m_winui ||
                                 owner->m_winui->callbackState !=
                                     callbackState )
                            {
                                return;
                            }

                            wxWinUIDatePickerImpl * const impl =
                                owner->m_winui.get();
                            const MUXC::Control current =
                                sender.try_as<MUXC::Control>();
                            const auto it = std::find_if(
                                impl->focusParts.begin(),
                                impl->focusParts.end(),
                                [&current](
                                    const MUXC::Control& candidate)
                                {
                                    return current && candidate &&
                                           winrt::get_abi(current) ==
                                               winrt::get_abi(candidate);
                                });
                            if ( it == impl->focusParts.end() )
                                return;

                            int step =
                                key == WS::VirtualKey::Right ? 1 : -1;
                            if ( impl->spinRoot.FlowDirection() ==
                                 MUX::FlowDirection::RightToLeft )
                            {
                                step = -step;
                            }
                            const int index = static_cast<int>(
                                std::distance(
                                    impl->focusParts.begin(), it));
                            const int next = index + step;
                            if ( next < 0 ||
                                 next >= static_cast<int>(
                                     impl->focusParts.size()) )
                            {
                                return;
                            }

                            if ( impl->focusParts[next].Focus(
                                     MUX::FocusState::Keyboard) )
                            {
                                args.Handled(true);
                            }
                        }));
            }
        }

        const bool peerApplied = ApplyToPeer();
        wxDatePickerCtrl *owner =
            callbackState->GetOwner(generation);
        if ( owner != this || !owner->m_winui ||
             owner->m_winui.get() != createImpl ||
             owner->m_winui->callbackState != callbackState ||
             owner->m_winui->picker != createPeer )
        {
            return false;
        }
        if ( !peerApplied )
        {
            owner->m_winui->Close();
            return false;
        }

        owner = callbackState->GetOwner(generation);
        if ( owner != this || !owner->m_winui ||
             owner->m_winui.get() != createImpl ||
             owner->m_winui->callbackState != callbackState ||
             owner->m_winui->picker != createPeer )
        {
            return false;
        }

        const MUXC::CalendarDatePicker picker = owner->m_winui->picker;
        const MUXC::Grid spinRoot = owner->m_winui->spinRoot;
        const MUXC::Control spinFocus =
            owner->m_winui->focusParts.empty()
                ? owner->m_winui->day.as<MUXC::Control>()
                : owner->m_winui->focusParts.front();
        const bool spinStyle = owner->m_winui->spinStyle;
        const MUX::UIElement content =
            spinStyle
                ? spinRoot.as<MUX::UIElement>()
                : picker.as<MUX::UIElement>();
        const MUX::UIElement focus =
            spinStyle
                ? spinFocus.as<MUX::UIElement>()
                : picker.as<MUX::UIElement>();
        const MUX::FrameworkElement layoutRoot =
            content.as<MUX::FrameworkElement>();
        createImpl->layoutRoot = layoutRoot;
        createImpl->initialSizeRequest = size;
        createImpl->initialSizePending = true;
        createImpl->layoutLoadedToken = layoutRoot.Loaded(
            [callbackState, generation, createImpl, layoutRoot](
                WF::IInspectable const&,
                MUX::RoutedEventArgs const&)
            {
                wxDatePickerCtrl * const liveOwner =
                    callbackState->GetOwner(generation);
                if ( !liveOwner || !liveOwner->m_winui ||
                     liveOwner->m_winui.get() != createImpl ||
                     liveOwner->m_winui->callbackState != callbackState ||
                     liveOwner->m_winui->layoutRoot != layoutRoot )
                {
                    return;
                }
                liveOwner->OnLayoutLoaded();
            });
        if ( !owner->m_winui->host.SetContent(content, focus) )
        {
            if ( callbackState->GetOwner(generation) == owner &&
                 owner->m_winui &&
                 owner->m_winui->callbackState == callbackState )
            {
                owner->m_winui->Close();
            }
            return false;
        }

        owner = callbackState->GetOwner(generation);
        if ( owner != this || !owner->m_winui ||
             owner->m_winui.get() != createImpl ||
             owner->m_winui->callbackState != callbackState ||
             owner->m_winui->picker != createPeer )
        {
            return false;
        }
        owner->m_winui->host.SetPreferredFocus(focus);
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI CalendarDatePicker creation", e);
        wxDatePickerCtrl * const owner =
            callbackState->GetOwner(generation);
        if ( owner && owner->m_winui &&
             owner->m_winui.get() == createImpl &&
             owner->m_winui->callbackState == callbackState )
        {
            owner->m_winui->Close();
        }
        return false;
    }

    wxDatePickerCtrl *owner = callbackState->GetOwner(generation);
    if ( owner != this || !owner->m_winui ||
         owner->m_winui.get() != createImpl ||
         owner->m_winui->callbackState != callbackState ||
         owner->m_winui->picker != createPeer )
    {
        return false;
    }
    // wxControl::Create() queried the fallback size before the locale-aware
    // XAML composite existed. Discard that cache so RTL, era and localized
    // NumberBox templates participate in the initial wx size negotiation.
    owner->InvalidateBestSize();
    const std::uint64_t initialSizeRevision =
        owner->m_winui->sizeWriterRevision;
    if ( !owner->ApplyInitialSizeTransaction(
             size, initialSizeRevision) )
    {
        return false;
    }
    owner = callbackState->GetOwner(generation);
    if ( owner != this || !owner->m_winui ||
         owner->m_winui.get() != createImpl ||
         owner->m_winui->callbackState != callbackState ||
         owner->m_winui->picker != createPeer )
    {
        return false;
    }
    createImpl->initialSizeProjectionComplete = true;
    createImpl->initialSizeWriterRevision = initialSizeRevision;
    createImpl->initialMinSize = owner->GetMinSize();
    if ( createImpl->sizeWriterRevision != initialSizeRevision ||
         createImpl->layoutXamlRoot )
    {
        createImpl->initialSizePending = false;
    }
    return true;
}

void wxDatePickerCtrl::SetValue(const wxDateTime& dt)
{
    wxCHECK_RET( dt.IsValid() || HasFlag(wxDP_ALLOWNONE),
                 wxT("this control requires a valid date") );

    const wxDateTime value = wxWinUINormalizeCivilDate(dt);
    if ( !wxWinUIIsInsideRange(value, m_rangeMin, m_rangeMax) )
        return;

    if ( !m_winui || !m_winui->callbackState )
        return;

    const std::shared_ptr<wxWinUIDateCallbackState> callbackState =
        m_winui->callbackState;
    const std::uint64_t generation = callbackState->Generation();
    wxWinUIDatePickerImpl * const updateImpl = m_winui.get();
    const std::uint64_t revision = callbackState->BeginStateMutation();
    if ( !ApplyStateToPeer(value, m_rangeMin, m_rangeMax) )
        return;

    wxDatePickerCtrl * const owner =
        callbackState->GetOwner(generation);
    if ( owner == this && owner->m_winui &&
         owner->m_winui.get() == updateImpl &&
         owner->m_winui->callbackState == callbackState &&
         callbackState->IsStateMutationCurrent(revision) )
    {
        owner->m_value = value;
    }
}

wxDateTime wxDatePickerCtrl::GetValue() const
{
    return m_value;
}

void wxDatePickerCtrl::SetRange(const wxDateTime& dt1, const wxDateTime& dt2)
{
    const wxDateTime minimum = wxWinUINormalizeCivilDate(dt1);
    const wxDateTime maximum = wxWinUINormalizeCivilDate(dt2);
    if ( minimum.IsValid() && maximum.IsValid() && minimum > maximum )
        return;
    if ( !m_winui || !m_winui->defaultsCaptured )
        return;
    const wxDateTime civilMinimum =
        wxWinUIFromDateTime(m_winui->civilMinimum);
    const wxDateTime civilMaximum =
        wxWinUIFromDateTime(m_winui->civilMaximum);
    if ( (minimum.IsValid() && minimum < civilMinimum) ||
         (maximum.IsValid() && maximum > civilMaximum) )
    {
        return;
    }

    wxDateTime value = m_value;
    if ( value.IsValid() )
    {
        if ( minimum.IsValid() && value < minimum )
            value = minimum;
        else if ( maximum.IsValid() && value > maximum )
            value = maximum;
    }

    if ( !m_winui || !m_winui->callbackState )
        return;

    const std::shared_ptr<wxWinUIDateCallbackState> callbackState =
        m_winui->callbackState;
    const std::uint64_t generation = callbackState->Generation();
    wxWinUIDatePickerImpl * const updateImpl = m_winui.get();
    const std::uint64_t revision = callbackState->BeginStateMutation();
    if ( !ApplyStateToPeer(value, minimum, maximum) )
        return;

    wxDatePickerCtrl * const owner =
        callbackState->GetOwner(generation);
    if ( owner == this && owner->m_winui &&
         owner->m_winui.get() == updateImpl &&
         owner->m_winui->callbackState == callbackState &&
         callbackState->IsStateMutationCurrent(revision) )
    {
        owner->m_rangeMin = minimum;
        owner->m_rangeMax = maximum;
        owner->m_value = value;
    }
}

bool wxDatePickerCtrl::GetRange(wxDateTime *dt1, wxDateTime *dt2) const
{
    if ( dt1 )
        *dt1 = m_rangeMin;
    if ( dt2 )
        *dt2 = m_rangeMax;
    return m_rangeMin.IsValid() || m_rangeMax.IsValid();
}

void wxDatePickerCtrl::SetNullText(const wxString& text)
{
    if ( !m_winui || !m_winui->picker )
        return;

    try
    {
        m_winui->nullText = text;
        m_winui->picker.PlaceholderText(wxWinUIToHString(text));
        if ( m_winui->nullPlaceholder )
            m_winui->nullPlaceholder.Text(wxWinUIToHString(text));
        InvalidateBestSize();
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException(
            "WinUI CalendarDatePicker null text update", e);
    }
}

wxSize wxDatePickerCtrl::DoGetBestSize() const
{
    wxSize size(m_winui && m_winui->spinStyle
                    ? 232 + (HasFlag(wxDP_ALLOWNONE) ? 56 : 0)
                    : 200,
                32);
    const MUX::FrameworkElement root =
        m_winui
            ? (m_winui->spinStyle
                   ? m_winui->spinRoot.as<MUX::FrameworkElement>()
                   : m_winui->picker.as<MUX::FrameworkElement>())
            : nullptr;
    if ( root )
    {
        try
        {
            const float infinity =
                std::numeric_limits<float>::infinity();
            root.Measure(WF::Size{infinity, infinity});
            const WF::Size desired = root.DesiredSize();
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
        size, const_cast<wxDatePickerCtrl*>(this));
}

bool wxDatePickerCtrl::ApplyToPeer()
{
    return ApplyStateToPeer(m_value, m_rangeMin, m_rangeMax);
}

bool wxDatePickerCtrl::ApplyStateToPeer(const wxDateTime& value,
                                        const wxDateTime& minimum,
                                        const wxDateTime& maximum)
{
    if ( !m_winui || !m_winui->picker )
        return true;
    if ( !m_winui->defaultsCaptured )
        return false;

    WF::DateTime peerMinimum{};
    WF::DateTime peerMaximum{};
    if ( minimum.IsValid() )
    {
        if ( !wxWinUIToDateTime(minimum, &peerMinimum) )
            return false;
    }
    else
    {
        peerMinimum = m_winui->civilMinimum;
    }

    if ( maximum.IsValid() )
    {
        if ( !wxWinUIToDateTime(maximum, &peerMaximum) )
            return false;
    }
    else
    {
        peerMaximum = m_winui->civilMaximum;
    }

    if ( peerMinimum.time_since_epoch() >
         peerMaximum.time_since_epoch() )
    {
        return false;
    }

    const wxDateTime effectiveMinimum =
        wxWinUIFromDateTime(peerMinimum);
    const wxDateTime effectiveMaximum =
        wxWinUIFromDateTime(peerMaximum);
    if ( !effectiveMinimum.IsValid() ||
         !effectiveMaximum.IsValid() ||
         effectiveMinimum > effectiveMaximum )
    {
        return false;
    }

    WF::IReference<WF::DateTime> peerValue{ nullptr };
    if ( !wxWinUIDateRef(value, &peerValue) )
        return false;

    wxWinUIDatePickerImpl * const updateImpl = m_winui.get();
    const std::shared_ptr<wxWinUIDateCallbackState> state =
        m_winui->callbackState;
    const std::uint64_t generation = state->Generation();
    const std::uint64_t revision = state->StateRevision();
    const MUXC::CalendarDatePicker picker = m_winui->picker;
    const bool spinStyle = m_winui->spinStyle;
    const MUXC::NumberBox yearPart = m_winui->year;
    const MUXC::NumberBox monthPart = m_winui->month;
    const MUXC::NumberBox dayPart = m_winui->day;
    const MUXC::ComboBox eraPart = m_winui->era;
    const MUXC::TextBlock nullPlaceholder =
        m_winui->nullPlaceholder;
    const int firstEra = m_winui->firstEra;
    const winrt::hstring calendarIdentifier =
        m_winui->calendarIdentifier;
    wxWinUIDatePeerMutationGuard guard(state);

    WF::DateTime previousMinimum{};
    WF::DateTime previousMaximum{};
    WF::IReference<WF::DateTime> previousValue{ nullptr };
    wxWinUIDatePartsSnapshot previousParts;
    MUX::Visibility previousNullVisibility = MUX::Visibility::Collapsed;
    bool snapshotCaptured = false;
    bool applied = false;
    try
    {
        previousMinimum = picker.MinDate();
        previousMaximum = picker.MaxDate();
        previousValue = picker.Date();
        if ( spinStyle &&
             !wxWinUICaptureDateParts(
                 eraPart,
                 yearPart, monthPart, dayPart,
                 &previousParts) )
        {
            return false;
        }
        if ( nullPlaceholder )
            previousNullVisibility = nullPlaceholder.Visibility();
        snapshotCaptured = true;

        wxWinUIApplyDateBounds(picker, peerMinimum, peerMaximum);
        picker.Date(peerValue);
        const WF::IReference<WF::DateTime> actualValue = picker.Date();
        wxDateTime actualCivil;
        const bool pickerApplied =
            wxWinUIReadPeerDate(actualValue, &actualCivil) &&
            wxWinUISameCivilDate(actualCivil, value);
        const bool partsApplied =
            !spinStyle ||
            wxWinUIApplyDateParts(
                eraPart,
                yearPart, monthPart, dayPart,
                firstEra, calendarIdentifier,
                value, effectiveMinimum, effectiveMaximum);
        if ( nullPlaceholder )
        {
            nullPlaceholder.Visibility(
                value.IsValid()
                    ? MUX::Visibility::Collapsed
                    : MUX::Visibility::Visible);
        }
        applied = pickerApplied && partsApplied;
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI CalendarDatePicker state update", e);
    }

    if ( !state->IsStateMutationCurrent(revision) )
        return false;

    if ( !applied )
    {
        if ( snapshotCaptured )
        {
            try
            {
                wxWinUIApplyDateBounds(
                    picker, previousMinimum, previousMaximum);
                picker.Date(previousValue);
                if ( spinStyle )
                {
                    wxWinUIRestoreDateParts(
                        eraPart,
                        yearPart, monthPart, dayPart, previousParts);
                }
                if ( nullPlaceholder )
                {
                    nullPlaceholder.Visibility(
                        previousNullVisibility);
                }
            }
            catch ( const winrt::hresult_error& e )
            {
                wxWinUILogException(
                    "WinUI CalendarDatePicker state rollback", e);
            }
        }
        return false;
    }

    wxDatePickerCtrl *owner = state->GetOwner(generation);
    if ( !owner || !owner->m_winui ||
         owner->m_winui.get() != updateImpl ||
         owner->m_winui->callbackState != state ||
         owner->m_winui->picker != picker ||
         !state->IsStateMutationCurrent(revision) )
    {
        return false;
    }

    owner->m_winui->host.ForceRender();
    owner = state->GetOwner(generation);
    return owner && owner->m_winui &&
           owner->m_winui.get() == updateImpl &&
           owner->m_winui->callbackState == state &&
           owner->m_winui->picker == picker &&
           state->IsStateMutationCurrent(revision);
}

void wxDatePickerCtrl::OnPeerDateChanged()
{
    if ( !m_winui || !m_winui->picker )
        return;

    const std::shared_ptr<wxWinUIDateCallbackState> state =
        m_winui->callbackState;
    if ( !state )
        return;
    const std::uint64_t generation = state->Generation();
    wxWinUIDatePickerImpl * const callbackImpl = m_winui.get();
    const std::uint64_t revision = state->BeginStateMutation();
    const bool spinStyle = m_winui->spinStyle;
    wxDateTime value;
    try
    {
        if ( spinStyle )
        {
            const wxDateTime effectiveMinimum =
                m_rangeMin.IsValid()
                    ? m_rangeMin
                    : wxWinUIFromDateTime(m_winui->civilMinimum);
            const wxDateTime effectiveMaximum =
                m_rangeMax.IsValid()
                    ? m_rangeMax
                    : wxWinUIFromDateTime(m_winui->civilMaximum);
            const wxWinUIDatePartsState readState =
                wxWinUIReadDateParts(
                    m_winui->era,
                    m_winui->year, m_winui->month, m_winui->day,
                    m_winui->firstEra,
                    m_winui->calendarIdentifier,
                    HasFlag(wxDP_ALLOWNONE),
                    effectiveMinimum, effectiveMaximum, &value);
            if ( readState == wxWinUIDatePartsState::Partial )
            {
                // A nullable spin peer must allow the user to rebuild a date
                // one component at a time after clearing it. Preserve every
                // finite component and wait until the triplet is complete.
                if ( m_winui->nullPlaceholder )
                {
                    m_winui->nullPlaceholder.Visibility(
                        MUX::Visibility::Collapsed);
                }
                return;
            }
            if ( readState == wxWinUIDatePartsState::Invalid )
            {
                ApplyToPeer();
                return;
            }
        }
        else if ( auto ref = m_winui->picker.Date() )
        {
            value = wxWinUIFromDateTime(ref.Value());
            if ( !value.IsValid() ||
                 !wxWinUIIsInsideRange(
                     value, m_rangeMin, m_rangeMax) )
            {
                ApplyToPeer();
                return;
            }
        }
        else if ( HasFlag(wxDP_ALLOWNONE) )
        {
            value = wxDefaultDateTime;
        }
        else
        {
            // The picker cleared the date but we require one: restore it.
            ApplyToPeer();
            return;
        }
    }
    catch ( const winrt::hresult_error& )
    {
        return;
    }

    if ( spinStyle &&
         !ApplyStateToPeer(value, m_rangeMin, m_rangeMax) )
    {
        return;
    }

    wxDatePickerCtrl * const owner = state->GetOwner(generation);
    if ( !owner || !owner->m_winui ||
         owner->m_winui.get() != callbackImpl ||
         owner->m_winui->callbackState != state ||
         !state->IsStateMutationCurrent(revision) )
    {
        return;
    }

    if ( wxWinUISameCivilDate(value, owner->m_value) )
        return;

    owner->m_value = value;
    wxDateEvent event(owner, value, wxEVT_DATE_CHANGED);
    // Event code is allowed to destroy this control. Do not access any member
    // after dispatch returns.
    owner->HandleWindowEvent(event);
}

#ifdef WXWINUI_TEST_SUPPORT
bool wxWinUIDatePickerTestAccess::SetPeerDate(
    wxDatePickerCtrl& control,
    const wxDateTime& dt)
{
    if ( !control.m_winui || !control.m_winui->picker ||
         !control.m_winui->callbackState || !dt.IsValid() )
        return false;

    if ( control.m_winui->spinStyle )
    {
        const std::shared_ptr<wxWinUIDateCallbackState> state =
            control.m_winui->callbackState;
        const std::uint64_t generation = state->Generation();
        if ( !control.ApplyStateToPeer(
                 wxWinUINormalizeCivilDate(dt),
                 control.m_rangeMin, control.m_rangeMax) )
        {
            return false;
        }

        wxDatePickerCtrl * const owner =
            state->GetOwner(generation);
        if ( !owner || !owner->m_winui ||
             owner->m_winui->callbackState != state ||
             !owner->m_winui->spinStyle )
        {
            return false;
        }
        owner->OnPeerDateChanged();
        return true;
    }

    WF::IReference<WF::DateTime> reference{ nullptr };
    if ( !wxWinUIDateRef(
             wxWinUINormalizeCivilDate(dt), &reference) )
    {
        return false;
    }

    const MUXC::CalendarDatePicker picker = control.m_winui->picker;
    try
    {
        picker.Date(reference);
        return true;
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException(
            "WinUI CalendarDatePicker test date mutation", e);
        return false;
    }
}
#endif

bool wxDatePickerCtrl::ClearPeerDateAndNotify()
{
    if ( !m_winui || !m_winui->picker ||
         !m_winui->callbackState )
        return false;

    if ( m_winui->spinStyle )
    {
        const std::shared_ptr<wxWinUIDateCallbackState> state =
            m_winui->callbackState;
        const std::uint64_t generation = state->Generation();
        if ( !ApplyStateToPeer(
                 wxDefaultDateTime, m_rangeMin, m_rangeMax) )
        {
            return false;
        }

        wxDatePickerCtrl * const owner =
            state->GetOwner(generation);
        if ( !owner || !owner->m_winui ||
             owner->m_winui->callbackState != state ||
             !owner->m_winui->spinStyle )
        {
            return false;
        }
        owner->OnPeerDateChanged();
        return true;
    }

    const MUXC::CalendarDatePicker picker = m_winui->picker;
    try
    {
        picker.Date(nullptr);
        return true;
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException(
            "WinUI CalendarDatePicker test clear", e);
        return false;
    }
}

#ifdef WXWINUI_TEST_SUPPORT
bool wxWinUIDatePickerTestAccess::ClearPeerDate(wxDatePickerCtrl& control)
{
    return control.ClearPeerDateAndNotify();
}

bool wxWinUIDatePickerTestAccess::GetPeerState(
    const wxDatePickerCtrl& control,
    wxDateTime *value,
    wxDateTime *minimum,
    wxDateTime *maximum)
{
    if ( !control.m_winui || !control.m_winui->picker )
        return false;

    try
    {
        const MUXC::CalendarDatePicker picker = control.m_winui->picker;
        if ( value )
        {
            if ( control.m_winui->spinStyle )
            {
                const wxDateTime effectiveMinimum =
                    control.m_rangeMin.IsValid()
                        ? control.m_rangeMin
                        : wxWinUIFromDateTime(
                              control.m_winui->civilMinimum);
                const wxDateTime effectiveMaximum =
                    control.m_rangeMax.IsValid()
                        ? control.m_rangeMax
                        : wxWinUIFromDateTime(
                              control.m_winui->civilMaximum);
                if ( wxWinUIReadDateParts(
                         control.m_winui->era,
                         control.m_winui->year, control.m_winui->month,
                         control.m_winui->day,
                         control.m_winui->firstEra,
                         control.m_winui->calendarIdentifier,
                         control.HasFlag(wxDP_ALLOWNONE),
                         effectiveMinimum, effectiveMaximum,
                         value) !=
                     wxWinUIDatePartsState::Complete )
                {
                    return false;
                }
            }
            else if ( !wxWinUIReadPeerDate(picker.Date(), value) )
            {
                return false;
            }
        }
        if ( minimum )
        {
            *minimum = wxWinUIFromDateTime(picker.MinDate());
            if ( !minimum->IsValid() )
                return false;
        }
        if ( maximum )
        {
            *maximum = wxWinUIFromDateTime(picker.MaxDate());
            if ( !maximum->IsValid() )
                return false;
        }
        return true;
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException(
            "WinUI CalendarDatePicker test snapshot", e);
        return false;
    }
}

bool wxWinUIDatePickerTestAccess::GetDefaultPeerRange(
    const wxDatePickerCtrl& control,
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

wxString wxWinUIDatePickerTestAccess::GetPeerDateFormat(const wxDatePickerCtrl& control)
{
    if ( !control.m_winui || !control.m_winui->picker )
        return wxString();

    try
    {
        return wxString(control.m_winui->picker.DateFormat().c_str());
    }
    catch ( const winrt::hresult_error& )
    {
        return wxString();
    }
}

bool wxWinUIDatePickerTestAccess::UsesDropdown(const wxDatePickerCtrl& control)
{
    return control.m_winui && !control.m_winui->spinStyle;
}

bool wxWinUIDatePickerTestAccess::GetSpinFields(
    const wxDatePickerCtrl& control,
    int *year, int *month, int *day)
{
    if ( !control.m_winui || !control.m_winui->spinStyle )
        return false;

    int actualYear = 0;
    int actualMonth = 0;
    int actualDay = 0;
    if ( !wxWinUIReadIntegralPart(control.m_winui->year, &actualYear) ||
         !wxWinUIReadIntegralPart(control.m_winui->month, &actualMonth) ||
         !wxWinUIReadIntegralPart(control.m_winui->day, &actualDay) )
    {
        return false;
    }

    if ( year )
        *year = actualYear;
    if ( month )
        *month = actualMonth;
    if ( day )
        *day = actualDay;
    return true;
}

bool wxWinUIDatePickerTestAccess::SetSpinPart(
    wxDatePickerCtrl& control,
    unsigned part, int value)
{
    if ( !control.m_winui || !control.m_winui->spinStyle ||
         !control.m_winui->callbackState || part >= 3 )
    {
        return false;
    }

    const auto parts = wxWinUIDateParts(
        control.m_winui->year, control.m_winui->month, control.m_winui->day);
    const MUXC::NumberBox peerPart = parts[part];
    const auto state = control.m_winui->callbackState;
    const std::uint64_t generation = state->Generation();
    try
    {
        {
            wxWinUIDatePeerMutationGuard guard(state);
            peerPart.Value(value);
        }

        wxDatePickerCtrl * const owner = state->GetOwner(generation);
        if ( !owner || !owner->m_winui ||
             owner->m_winui->callbackState != state ||
             !owner->m_winui->spinStyle )
        {
            return false;
        }
        owner->OnPeerDateChanged();
        return true;
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException(
            "WinUI DatePicker test spin-part mutation", e);
        return false;
    }
}

bool wxWinUIDatePickerTestAccess::GetSpinPart(
    const wxDatePickerCtrl& control,
    unsigned part, int *value, bool *blank)
{
    if ( !control.m_winui || !control.m_winui->spinStyle || part >= 3 )
        return false;

    try
    {
        const auto parts = wxWinUIDateParts(
            control.m_winui->year, control.m_winui->month, control.m_winui->day);
        const double peerValue = parts[part].Value();
        const bool isBlank = !std::isfinite(peerValue);
        if ( blank )
            *blank = isBlank;
        if ( value )
        {
            *value = isBlank
                         ? 0
                         : static_cast<int>(std::lround(peerValue));
        }
        return true;
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException(
            "WinUI DatePicker test spin-part query", e);
        return false;
    }
}

wxString wxWinUIDatePickerTestAccess::GetSpinYearText(const wxDatePickerCtrl& control)
{
    if ( !control.m_winui || !control.m_winui->spinStyle || !control.m_winui->year )
        return wxString();

    try
    {
        return wxWinUIFromHString(control.m_winui->year.Text());
    }
    catch ( const winrt::hresult_error& )
    {
        return wxString();
    }
}

wxString wxWinUIDatePickerTestAccess::GetLocaleDatePattern(const wxDatePickerCtrl& control)
{
    return control.m_winui ? control.m_winui->localePattern : wxString();
}

wxString wxWinUIDatePickerTestAccess::GetPeerNullText(const wxDatePickerCtrl& control)
{
    if ( !control.m_winui || !control.m_winui->picker )
        return wxString();

    try
    {
        const wxString pickerText =
            wxWinUIFromHString(control.m_winui->picker.PlaceholderText());
        if ( control.m_winui->nullPlaceholder &&
             wxWinUIFromHString(control.m_winui->nullPlaceholder.Text()) !=
                 pickerText )
        {
            return wxString();
        }
        return pickerText;
    }
    catch ( const winrt::hresult_error& )
    {
        return wxString();
    }
}
#endif

#endif // wxUSE_DATEPICKCTRL
