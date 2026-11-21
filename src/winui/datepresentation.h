/////////////////////////////////////////////////////////////////////////////
// Name:        src/winui/datepresentation.h
// Purpose:     Shared civil-date and native date presentation primitives
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////
#ifndef _WX_WINUI_DATEPRESENTATION_H_
#define _WX_WINUI_DATEPRESENTATION_H_

#if defined(__WXWINUI__) && wxUSE_WINUI3 && wxUSE_DATEPICKCTRL
#include "wx/datetime.h"
#include "private.h"
#include <winrt/Windows.Globalization.h>
#include <winrt/Windows.Globalization.DateTimeFormatting.h>
#include <winrt/Windows.Globalization.NumberFormatting.h>
#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <functional>
#include <limits>
#include <vector>

// Source-private: the interactive DatePicker and passive PropertyGrid date
// presentation share conversion and locale policy, not event/model ownership.
namespace wxWinUIDatePresentation
{
namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;
namespace WF = winrt::Windows::Foundation;
namespace WG = winrt::Windows::Globalization;
namespace WGDT = winrt::Windows::Globalization::DateTimeFormatting;
namespace WGNF = winrt::Windows::Globalization::NumberFormatting;

inline WGNF::DecimalFormatter wxWinUICreateDateDecimalFormatter(
    const winrt::hstring& language)
{
    if ( language.empty() )
        return WGNF::DecimalFormatter();

    const WG::GeographicRegion region;
    return WGNF::DecimalFormatter(
        std::vector<winrt::hstring>{language},
        region.CodeTwoLetter());
}

inline wxDateTime wxWinUINormalizeCivilDate(const wxDateTime& value)
{
    if ( !value.IsValid() )
        return wxDefaultDateTime;

    wxDateTime date(value);
    date.ResetTime();
    return date;
}

inline wxString wxWinUIGetLocaleShortDatePattern(
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

inline std::vector<wxWinUIDateLayoutToken>
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
inline bool wxWinUIToDateTime(const wxDateTime& value, WF::DateTime *result)
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

inline wxDateTime wxWinUIFromDateTime(const WF::DateTime& value)
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

inline bool wxWinUIResolveCalendarEnvelope(
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

inline bool wxWinUIDateRef(const wxDateTime& value,
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

inline bool wxWinUISameCivilDate(const wxDateTime& lhs,
                          const wxDateTime& rhs)
{
    if ( lhs.IsValid() != rhs.IsValid() )
        return false;
    return !lhs.IsValid() ||
           wxWinUINormalizeCivilDate(lhs) ==
               wxWinUINormalizeCivilDate(rhs);
}

inline bool wxWinUIIsInsideRange(const wxDateTime& value,
                          const wxDateTime& minimum,
                          const wxDateTime& maximum)
{
    return !value.IsValid() ||
           ((!minimum.IsValid() || value >= minimum) &&
            (!maximum.IsValid() || value <= maximum));
}

inline void wxWinUIApplyDateBounds(
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
inline int wxWinUIResolveTwoDigitYearMaximum(
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

inline MUXC::NumberBox wxWinUICreateDatePart(double minimum,
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

inline void wxWinUIApplyNumberBounds(const MUXC::NumberBox& box,
                              double minimum,
                              double maximum,
                              const std::function<void()>& check = {})
{
    const double oldMinimum = box.Minimum();
    const double oldMaximum = box.Maximum();
    if ( minimum > oldMaximum )
    {
        box.Maximum(maximum);
        if ( check ) check();
        box.Minimum(minimum);
        if ( check ) check();
    }
    else if ( maximum < oldMinimum )
    {
        box.Minimum(minimum);
        if ( check ) check();
        box.Maximum(maximum);
        if ( check ) check();
    }
    else
    {
        box.Minimum(minimum);
        if ( check ) check();
        box.Maximum(maximum);
        if ( check ) check();
    }
}

inline bool wxWinUIReadIntegralPart(const MUXC::NumberBox& box,
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

inline std::array<MUXC::NumberBox, 3> wxWinUIDateParts(
    const MUXC::NumberBox& year,
    const MUXC::NumberBox& month,
    const MUXC::NumberBox& day)
{
    return {{year, month, day}};
}

inline bool wxWinUICaptureDateParts(
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

inline void wxWinUIRestoreDateParts(
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

inline bool wxWinUIReadCalendarDateParts(
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

inline bool wxWinUIFindCalendarEraAnchor(
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

inline bool wxWinUIFindCalendarEraEnd(
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

inline bool wxWinUIApplyDateParts(
    const MUXC::ComboBox& era,
    const MUXC::NumberBox& year,
    const MUXC::NumberBox& month,
    const MUXC::NumberBox& day,
    int firstEra,
    const winrt::hstring& calendarIdentifier,
    const wxDateTime& value,
    const wxDateTime& effectiveMinimum,
    const wxDateTime& effectiveMaximum,
    const std::function<void()>& check = {})
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
        wxWinUIApplyNumberBounds(year, 1, 9999, check);
        wxWinUIApplyNumberBounds(month, 1, 13, check);
        wxWinUIApplyNumberBounds(day, 1, 31, check);
        const double blank =
            std::numeric_limits<double>::quiet_NaN();
        if ( era )
        {
            era.SelectedIndex(-1);
            if ( check ) check();
        }
        year.Value(blank);
        if ( check ) check();
        month.Value(blank);
        if ( check ) check();
        day.Value(blank);
        if ( check ) check();
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
    wxWinUIApplyNumberBounds(year, 1, 9999, check);
    wxWinUIApplyNumberBounds(month, 1, 13, check);
    wxWinUIApplyNumberBounds(day, 1, 31, check);
    if ( era )
    {
        era.SelectedIndex(selectedParts.era - firstEra);
        if ( check ) check();
    }
    year.Value(selectedYear);
    if ( check ) check();
    month.Value(selectedMonth);
    if ( check ) check();
    day.Value(selectedDay);
    if ( check ) check();
    wxWinUIApplyNumberBounds(year, minimumYear, maximumYear, check);
    wxWinUIApplyNumberBounds(month, minimumMonth, maximumMonth, check);
    wxWinUIApplyNumberBounds(day, minimumDay, maximumDay, check);

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
} // namespace wxWinUIDatePresentation
#endif
#endif
