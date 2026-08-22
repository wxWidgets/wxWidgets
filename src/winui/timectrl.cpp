/////////////////////////////////////////////////////////////////////////////
// Name:        src/winui/timectrl.cpp
// Purpose:     wxWinUI wxTimePickerCtrl implementation (WinUI TimePicker)
// Author:      wxWidgets development team
// Created:     2026-06-01
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_TIMEPICKCTRL

#include "wx/timectrl.h"
#include "wx/dateevt.h"
#include "wx/scopeguard.h"

#include "private.h"

#include <winrt/Microsoft.UI.Dispatching.h>
#include <winrt/Microsoft.UI.Xaml.Automation.h>
#include <winrt/Microsoft.UI.Xaml.Controls.Primitives.h>
#include <winrt/Microsoft.UI.Xaml.Input.h>
#include <winrt/Microsoft.UI.Xaml.Media.h>
#include <winrt/Windows.Globalization.h>
#include <winrt/Windows.Globalization.DateTimeFormatting.h>
#include <winrt/Windows.Globalization.NumberFormatting.h>
#include <winrt/Windows.System.h>

#include <algorithm>
#include <atomic>
#include <array>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <limits>
#include <memory>
#include <utility>
#include <vector>

namespace MUX = winrt::Microsoft::UI::Xaml;
namespace MUXA = winrt::Microsoft::UI::Xaml::Automation;
namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;
namespace MUXCP = winrt::Microsoft::UI::Xaml::Controls::Primitives;
namespace MUXI = winrt::Microsoft::UI::Xaml::Input;
namespace MUXM = winrt::Microsoft::UI::Xaml::Media;
namespace WF = winrt::Windows::Foundation;
namespace WG = winrt::Windows::Globalization;
namespace WGDT = winrt::Windows::Globalization::DateTimeFormatting;
namespace WGNF = winrt::Windows::Globalization::NumberFormatting;
namespace WS = winrt::Windows::System;

namespace
{

winrt::hstring gs_winuiTimeLanguageForTesting;
wxTimePickerCtrl::WinUIHourLoadedHookForTesting
    gs_winuiTimeHourLoadedHookForTesting = nullptr;
void *gs_winuiTimeHourLoadedHookDataForTesting = nullptr;

WGNF::DecimalFormatter wxWinUICreateTimeDecimalFormatter(
    const winrt::hstring& language)
{
    if ( language.empty() )
        return WGNF::DecimalFormatter();

    const WG::GeographicRegion region;
    return WGNF::DecimalFormatter(
        std::vector<winrt::hstring>{language},
        region.CodeTwoLetter());
}

// TimePicker itself is minute-precision. wxWinUI keeps it as the native
// transactional property peer and exposes locale-ordered NumberBoxes for all
// three editable fields, including seconds. The fixed date is DST-neutral and
// is intentionally not part of the public value contract.
wxDateTime wxWinUINormalizeTime(const wxDateTime& value)
{
    if ( !value.IsValid() )
        return wxDefaultDateTime;

    const wxDateTime::Tm tm = value.GetTm();
    return wxDateTime(1, wxDateTime::Jan, 2012,
                      tm.hour, tm.min, tm.sec);
}

bool wxWinUIToTimeSpan(const wxDateTime& value, WF::TimeSpan *result)
{
    const wxDateTime time = wxWinUINormalizeTime(value);
    if ( !time.IsValid() || !result )
        return false;

    const wxDateTime::Tm tm = time.GetTm();
    *result = std::chrono::hours(tm.hour) +
              std::chrono::minutes(tm.min);
    return true;
}

wxDateTime wxWinUIFromTimeSpan(const WF::TimeSpan& value, int second)
{
    const auto totalMinutes =
        std::chrono::duration_cast<std::chrono::minutes>(value).count();
    constexpr long long MinutesPerDay = 24 * 60;
    if ( totalMinutes < 0 || totalMinutes >= MinutesPerDay ||
         second < 0 || second > 59 )
        return wxDefaultDateTime;

    return wxDateTime(
        1, wxDateTime::Jan, 2012,
        static_cast<int>(totalMinutes / 60),
        static_cast<int>(totalMinutes % 60),
        second);
}

MUXC::NumberBox wxWinUICreateTimeBox(double minimum,
                                     double maximum,
                                     unsigned integerDigits,
                                     double minimumWidth,
                                     const winrt::hstring& language)
{
    MUXC::NumberBox box;
    if ( !language.empty() )
        box.Language(language);
    box.Minimum(minimum);
    box.Maximum(maximum);
    box.SmallChange(1);
    box.LargeChange(10);
    box.IsWrapEnabled(true);
    box.AcceptsExpression(false);
    box.ValidationMode(
        MUXC::NumberBoxValidationMode::InvalidInputOverwritten);
    box.SpinButtonPlacementMode(
        MUXC::NumberBoxSpinButtonPlacementMode::Inline);
    WGNF::DecimalFormatter formatter =
        wxWinUICreateTimeDecimalFormatter(language);
    formatter.IntegerDigits(integerDigits);
    formatter.FractionDigits(0);
    box.NumberFormatter(formatter);
    box.MinWidth(minimumWidth);
    return box;
}

enum class wxWinUITimeLayoutKind
{
    Literal,
    Hour,
    Minute,
    Second,
    Period
};

struct wxWinUITimeLayoutToken
{
    wxWinUITimeLayoutKind kind = wxWinUITimeLayoutKind::Literal;
    wxString text;
};

std::vector<wxWinUITimeLayoutToken>
wxWinUIParseTimePattern(const wxString& pattern, bool *complete)
{
    if ( complete )
        *complete = false;

    std::vector<wxWinUITimeLayoutToken> result;
    wxString literal;
    const auto flushLiteral = [&]()
    {
        if ( !literal.empty() )
        {
            result.push_back(
                {wxWinUITimeLayoutKind::Literal, literal});
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
            return {};
        }

        const wxString field = pattern.Mid(pos + 1, end - pos - 1);
        wxWinUITimeLayoutKind kind = wxWinUITimeLayoutKind::Literal;
        if ( field.StartsWith("hour.") )
            kind = wxWinUITimeLayoutKind::Hour;
        else if ( field.StartsWith("minute.") )
            kind = wxWinUITimeLayoutKind::Minute;
        else if ( field.StartsWith("second.") )
            kind = wxWinUITimeLayoutKind::Second;
        else if ( field.StartsWith("period.") )
            kind = wxWinUITimeLayoutKind::Period;

        if ( kind == wxWinUITimeLayoutKind::Literal )
        {
            // DateTimeFormatter was explicitly asked for hour, minute and
            // second only. An unknown field means that we don't understand
            // the complete locale pattern and must not replace it with a
            // guessed Western order.
            return {};
        }

        flushLiteral();
        result.push_back({kind, wxString()});
        pos = end + 1;
    }
    flushLiteral();
    if ( complete )
        *complete = true;
    return result;
}

wxString wxWinUIGetLocaleTimePattern(winrt::hstring *clock)
{
    wxString pattern;
    try
    {
        const WGDT::DateTimeFormatter formatter =
            gs_winuiTimeLanguageForTesting.empty()
                ? WGDT::DateTimeFormatter(
                      WGDT::HourFormat::Default,
                      WGDT::MinuteFormat::Default,
                      WGDT::SecondFormat::Default)
                : WGDT::DateTimeFormatter(
                      WGDT::YearFormat::None,
                      WGDT::MonthFormat::None,
                      WGDT::DayFormat::None,
                      WGDT::DayOfWeekFormat::None,
                      WGDT::HourFormat::Default,
                      WGDT::MinuteFormat::Default,
                      WGDT::SecondFormat::Default,
                      std::vector<winrt::hstring>{
                          gs_winuiTimeLanguageForTesting});
        if ( clock )
            *clock = formatter.Clock();
        const auto patterns = formatter.Patterns();
        if ( patterns.Size() )
            pattern = wxWinUIFromHString(patterns.GetAt(0));
    }
    catch ( const winrt::hresult_error& )
    {
    }

    return pattern;
}

bool wxWinUIGetPeriodLabels(
    const winrt::hstring& clock,
    std::array<winrt::hstring, 2> *labels)
{
    if ( clock.empty() || !labels )
        return false;

    try
    {
        WG::Calendar calendar{ nullptr };
        if ( gs_winuiTimeLanguageForTesting.empty() )
        {
            calendar = WG::Calendar();
        }
        else
        {
            calendar = WG::Calendar(
                std::vector<winrt::hstring>{
                    gs_winuiTimeLanguageForTesting});
        }
        calendar.ChangeClock(clock);
        calendar.SetToNow();

        const int first = calendar.FirstPeriodInThisDay();
        const int last = calendar.LastPeriodInThisDay();
        if ( first == last )
            return false;

        calendar.Period(first);
        const winrt::hstring firstLabel = calendar.PeriodAsString();
        calendar.Period(last);
        const winrt::hstring lastLabel = calendar.PeriodAsString();
        if ( firstLabel.empty() || lastLabel.empty() )
            return false;

        (*labels)[0] = firstLabel;
        (*labels)[1] = lastLabel;
        return true;
    }
    catch ( const winrt::hresult_error& )
    {
        return false;
    }
}

bool wxWinUIReadTimePart(const MUXC::NumberBox& box, int *value)
{
    if ( !box || !value )
        return false;
    const double peerValue = box.Value();
    if ( !std::isfinite(peerValue) )
        return false;
    *value = static_cast<int>(std::lround(peerValue));
    return true;
}

template <typename T>
T wxWinUIFindTimeDescendantByName(
    const MUX::DependencyObject& root,
    const wchar_t *name)
{
    if ( !root || !name )
        return nullptr;

    if ( const T candidate = root.try_as<T>() )
    {
        if ( candidate.Name() == name )
            return candidate;
    }

    const int count = MUXM::VisualTreeHelper::GetChildrenCount(root);
    for ( int n = 0; n != count; ++n )
    {
        if ( const T candidate = wxWinUIFindTimeDescendantByName<T>(
                 MUXM::VisualTreeHelper::GetChild(root, n), name) )
        {
            return candidate;
        }
    }
    return nullptr;
}

bool wxWinUISameTime(const wxDateTime& lhs, const wxDateTime& rhs)
{
    if ( lhs.IsValid() != rhs.IsValid() )
        return false;
    return !lhs.IsValid() ||
           wxWinUINormalizeTime(lhs) == wxWinUINormalizeTime(rhs);
}

wxDateTime wxWinUIFromDisplayedTime(int hour,
                                    int minute,
                                    int second,
                                    bool twelveHour,
                                    int period)
{
    if ( minute < 0 || minute > 59 || second < 0 || second > 59 )
        return wxDefaultDateTime;

    if ( twelveHour )
    {
        if ( hour < 1 || hour > 12 || period < 0 || period > 1 )
            return wxDefaultDateTime;
        hour %= 12;
        if ( period == 1 )
            hour += 12;
    }
    else if ( hour < 0 || hour > 23 )
    {
        return wxDefaultDateTime;
    }

    return wxDateTime(
        1, wxDateTime::Jan, 2012, hour, minute, second);
}

class wxWinUITimeCallbackState
{
public:
    explicit wxWinUITimeCallbackState(wxTimePickerCtrl *owner)
        : m_owner(owner)
    {
    }

    std::uint64_t Generation() const
    {
        return m_generation.load(std::memory_order_acquire);
    }

    wxTimePickerCtrl *GetOwner(std::uint64_t generation) const
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
                     "unbalanced WinUI time peer mutation");
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
    std::atomic<wxTimePickerCtrl *> m_owner;
    std::atomic<std::uint64_t> m_generation{1};
    std::atomic<unsigned> m_peerMutationDepth{0};
    std::atomic<std::uint64_t> m_stateRevision{1};
};

class wxWinUITimePeerMutationGuard
{
public:
    explicit wxWinUITimePeerMutationGuard(
        std::shared_ptr<wxWinUITimeCallbackState> state)
        : m_state(std::move(state))
    {
        if ( m_state )
            m_state->BeginPeerMutation();
    }

    ~wxWinUITimePeerMutationGuard()
    {
        if ( m_state )
            m_state->EndPeerMutation();
    }

    wxWinUITimePeerMutationGuard(
        const wxWinUITimePeerMutationGuard&) = delete;
    wxWinUITimePeerMutationGuard& operator=(
        const wxWinUITimePeerMutationGuard&) = delete;

private:
    std::shared_ptr<wxWinUITimeCallbackState> m_state;
};

} // namespace

struct wxWinUITimePendingHourChange
{
    double oldValue = 0;
    double newValue = 0;
    int stepDirection = 0;
    std::uint64_t stateRevision = 0;
};

class wxWinUITimePickerImpl
{
public:
    ~wxWinUITimePickerImpl()
    {
        Close();
    }

    void Close()
    {
        if ( callbackState )
            callbackState->Invalidate();

        if ( picker && selectedTimeChangedToken.value )
        {
            try
            {
                picker.SelectedTimeChanged(selectedTimeChangedToken);
            }
            catch ( const winrt::hresult_error& e )
            {
                wxWinUILogException(
                    "WinUI TimePicker SelectedTimeChanged removal", e);
            }
        }
        selectedTimeChangedToken = {};

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
        revokePart(hour, hourChangedToken,
                   "WinUI TimePicker hour ValueChanged removal");
        revokePart(minute, minuteChangedToken,
                   "WinUI TimePicker minute ValueChanged removal");
        revokePart(seconds, secondsChangedToken,
                   "WinUI TimePicker seconds ValueChanged removal");

        if ( period && periodChangedToken.value )
        {
            try
            {
                period.SelectionChanged(periodChangedToken);
            }
            catch ( const winrt::hresult_error& e )
            {
                wxWinUILogException(
                    "WinUI TimePicker period SelectionChanged removal", e);
            }
        }
        periodChangedToken = {};

        if ( hour && hourLoadedToken.value )
        {
            try
            {
                hour.Loaded(hourLoadedToken);
            }
            catch ( const winrt::hresult_error& e )
            {
                wxWinUILogException(
                    "WinUI TimePicker hour Loaded removal", e);
            }
        }
        hourLoadedToken = {};

        if ( hour && hourLayoutUpdatedToken.value )
        {
            try
            {
                hour.LayoutUpdated(hourLayoutUpdatedToken);
            }
            catch ( const winrt::hresult_error& e )
            {
                wxWinUILogException(
                    "WinUI TimePicker hour LayoutUpdated removal", e);
            }
        }
        hourLayoutUpdatedToken = {};

        if ( root && rootLoadedToken.value )
        {
            try
            {
                root.Loaded(rootLoadedToken);
            }
            catch ( const winrt::hresult_error& e )
            {
                wxWinUILogException(
                    "WinUI TimePicker root Loaded removal", e);
            }
        }
        rootLoadedToken = {};

        DetachHourSpinButtons();
        pendingHourChange.reset();
        nextHourStepDirection = 0;

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
                        "WinUI TimePicker PreviewKeyDown removal", e);
                }
            }
        }
        focusKeyTokens.clear();
        focusParts.clear();

        host.Close();
        root = nullptr;
        period = nullptr;
        seconds = nullptr;
        minute = nullptr;
        hour = nullptr;
        picker = nullptr;
        callbackState.reset();
        resolvingHourSpinButtons = false;
        pendingHourSpinButtonResolve = false;
        layoutXamlRoot = nullptr;
        initialSizePending = false;
        initialSizeProjectionComplete = false;
        suppressNextMinSizeWriter = false;
        suppressNextSizeWriter = false;
    }

    void DetachHourSpinButtons()
    {
        const MUXCP::RepeatButton oldIncrement = hourIncrement;
        const MUXCP::RepeatButton oldDecrement = hourDecrement;
        const winrt::event_token oldIncrementToken =
            hourIncrementToken;
        const winrt::event_token oldDecrementToken =
            hourDecrementToken;
        hourIncrement = nullptr;
        hourDecrement = nullptr;
        hourIncrementToken = {};
        hourDecrementToken = {};
        if ( ++hourSpinButtonGeneration == 0 )
            ++hourSpinButtonGeneration;

        const auto revokeSpinButton =
            [](const MUXCP::RepeatButton& button,
               winrt::event_token token,
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
            };
        revokeSpinButton(oldIncrement, oldIncrementToken,
                          "WinUI TimePicker increment Click removal");
        revokeSpinButton(oldDecrement, oldDecrementToken,
                          "WinUI TimePicker decrement Click removal");
    }

    wxWinUIControlHost host;
    std::shared_ptr<wxWinUITimeCallbackState> callbackState;
    MUXC::Grid root{ nullptr };
    MUX::XamlRoot layoutXamlRoot{ nullptr };
    MUXC::TimePicker picker{ nullptr };
    MUXC::NumberBox hour{ nullptr };
    MUXC::NumberBox minute{ nullptr };
    MUXC::NumberBox seconds{ nullptr };
    MUXC::ComboBox period{ nullptr };
    MUXCP::RepeatButton hourIncrement{ nullptr };
    MUXCP::RepeatButton hourDecrement{ nullptr };
    winrt::event_token selectedTimeChangedToken{};
    winrt::event_token hourChangedToken{};
    winrt::event_token minuteChangedToken{};
    winrt::event_token secondsChangedToken{};
    winrt::event_token periodChangedToken{};
    winrt::event_token hourLoadedToken{};
    winrt::event_token hourLayoutUpdatedToken{};
    winrt::event_token rootLoadedToken{};
    winrt::event_token hourIncrementToken{};
    winrt::event_token hourDecrementToken{};
    std::uint64_t hourSpinButtonGeneration = 1;
    std::shared_ptr<wxWinUITimePendingHourChange> pendingHourChange;
    int nextHourStepDirection = 0;
    bool resolvingHourSpinButtons = false;
    bool pendingHourSpinButtonResolve = false;
    bool initialSizePending = true;
    bool initialSizeProjectionComplete = false;
    bool suppressNextMinSizeWriter = false;
    bool suppressNextSizeWriter = false;
    std::uint64_t sizeWriterRevision = 1;
    std::uint64_t initialSizeWriterRevision = 0;
    std::uint64_t layoutRevision = 1;
    wxSize initialSizeRequest = wxDefaultSize;
    wxSize initialMinSize = wxDefaultSize;
    std::vector<MUXC::Control> focusParts;
    std::vector<winrt::event_token> focusKeyTokens;
    wxString localePattern;
    std::array<int, 4> fieldOrder{{-1, -1, -1, -1}};
    bool twelveHour = false;
};

void wxTimePickerCtrl::ResolveHourSpinButtons(bool updateLayout)
{
    if ( !m_winui || !m_winui->hour || !m_winui->callbackState )
        return;

    wxWinUITimePickerImpl * const impl = m_winui.get();
    const std::shared_ptr<wxWinUITimeCallbackState> state =
        m_winui->callbackState;
    const std::uint64_t generation = state->Generation();
    const MUXC::NumberBox hour = m_winui->hour;
    const auto getExactOwner =
        [state, generation, impl, hour]() -> wxTimePickerCtrl *
        {
            wxTimePickerCtrl * const owner =
                state->GetOwner(generation);
            return owner && owner->m_winui &&
                   owner->m_winui.get() == impl &&
                   owner->m_winui->callbackState == state &&
                   owner->m_winui->hour == hour
                       ? owner
                       : nullptr;
        };

    if ( impl->resolvingHourSpinButtons )
    {
        impl->pendingHourSpinButtonResolve = true;
        return;
    }

    impl->resolvingHourSpinButtons = true;
    wxScopeGuard resolvingGuard = wxMakeGuard(
        [state, generation, impl]()
        {
            wxTimePickerCtrl * const owner =
                state->GetOwner(generation);
            if ( owner && owner->m_winui &&
                 owner->m_winui.get() == impl )
            {
                impl->resolvingHourSpinButtons = false;
            }
        });
    wxUnusedVar(resolvingGuard);

    for ( unsigned pass = 0; pass != 2; ++pass )
    {
        wxTimePickerCtrl *owner = getExactOwner();
        if ( owner != this )
            break;

        impl->pendingHourSpinButtonResolve = false;
        try
        {
            if ( updateLayout && pass == 0 )
            {
                hour.ApplyTemplate();
                hour.UpdateLayout();
            }

            owner = getExactOwner();
            if ( owner != this )
                break;

            const MUXCP::RepeatButton increment =
                wxWinUIFindTimeDescendantByName<MUXCP::RepeatButton>(
                    hour, L"UpSpinButton");
            owner = getExactOwner();
            if ( owner != this )
                break;
            const MUXCP::RepeatButton decrement =
                wxWinUIFindTimeDescendantByName<MUXCP::RepeatButton>(
                    hour, L"DownSpinButton");
            owner = getExactOwner();
            if ( owner != this )
                break;

            if ( increment != impl->hourIncrement ||
                 decrement != impl->hourDecrement ||
                 (increment && !impl->hourIncrementToken.value) ||
                 (decrement && !impl->hourDecrementToken.value) )
            {
                impl->DetachHourSpinButtons();
                const std::uint64_t partGeneration =
                    impl->hourSpinButtonGeneration;
                winrt::event_token incrementToken{};
                winrt::event_token decrementToken{};
                bool tokensAdopted = false;
                wxScopeGuard tokenGuard = wxMakeGuard(
                    [&increment, &decrement,
                     &incrementToken, &decrementToken,
                     &tokensAdopted]()
                    {
                        if ( tokensAdopted )
                            return;
                        try
                        {
                            if ( increment && incrementToken.value )
                                increment.Click(incrementToken);
                            if ( decrement && decrementToken.value )
                                decrement.Click(decrementToken);
                        }
                        catch ( const winrt::hresult_error& )
                        {
                        }
                    });
                wxUnusedVar(tokenGuard);

                const auto registerStep =
                    [state, generation, impl, partGeneration](
                        const MUXCP::RepeatButton& button,
                        int direction)
                    {
                        return button.Click(
                            [state, generation, impl, partGeneration,
                             button, direction](
                                WF::IInspectable const& sender,
                                MUX::RoutedEventArgs const&)
                            {
                                wxTimePickerCtrl * const owner =
                                    state->GetOwner(generation);
                                if ( !owner || !owner->m_winui ||
                                     owner->m_winui.get() != impl ||
                                     owner->m_winui->callbackState != state ||
                                     owner->m_winui
                                         ->hourSpinButtonGeneration !=
                                             partGeneration ||
                                     sender.try_as<MUXCP::RepeatButton>() !=
                                         button )
                                {
                                    return;
                                }

                                if ( owner->m_winui->pendingHourChange &&
                                     owner->m_winui->pendingHourChange
                                         ->stepDirection == 0 )
                                {
                                    owner->m_winui->pendingHourChange
                                        ->stepDirection = direction;
                                }
                                else
                                {
                                    owner->m_winui->nextHourStepDirection =
                                        direction;
                                }
                            });
                    };

                if ( increment && decrement )
                {
                    incrementToken = registerStep(increment, 1);
                    owner = getExactOwner();
                    if ( owner != this ||
                         impl->hourSpinButtonGeneration != partGeneration )
                    {
                        break;
                    }
                    decrementToken = registerStep(decrement, -1);
                    owner = getExactOwner();
                    if ( owner != this ||
                         impl->hourSpinButtonGeneration != partGeneration )
                    {
                        break;
                    }

                    impl->hourIncrement = increment;
                    impl->hourDecrement = decrement;
                    impl->hourIncrementToken = incrementToken;
                    impl->hourDecrementToken = decrementToken;
                    tokensAdopted = true;
                }
            }
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException(
                "WinUI TimePicker spin-part resolution", e);
        }

        owner = getExactOwner();
        if ( owner != this ||
             !impl->pendingHourSpinButtonResolve )
        {
            break;
        }
    }
}

wxIMPLEMENT_DYNAMIC_CLASS(wxTimePickerCtrl, wxControl);

wxString wxTimePickerCtrl::WinUISetLanguageForTesting(
    const wxString& language)
{
    const wxString previous =
        wxWinUIFromHString(gs_winuiTimeLanguageForTesting);
    gs_winuiTimeLanguageForTesting = wxWinUIToHString(language);
    return previous;
}

wxTimePickerCtrl::wxTimePickerCtrl()
{
}

wxTimePickerCtrl::wxTimePickerCtrl(wxWindow *parent,
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

wxTimePickerCtrl::~wxTimePickerCtrl()
{
    if ( m_winui )
        m_winui->Close();
}

void wxTimePickerCtrl::SetMinSize(const wxSize& minSize)
{
    if ( m_winui )
    {
        if ( m_winui->suppressNextMinSizeWriter )
            m_winui->suppressNextMinSizeWriter = false;
        else if ( ++m_winui->sizeWriterRevision == 0 )
            ++m_winui->sizeWriterRevision;
    }
    wxTimePickerCtrlBase::SetMinSize(minSize);
}

void wxTimePickerCtrl::DoSetSize(int x, int y, int width, int height,
                                 int sizeFlags)
{
    if ( m_winui )
    {
        if ( m_winui->suppressNextSizeWriter )
            m_winui->suppressNextSizeWriter = false;
        else if ( ++m_winui->sizeWriterRevision == 0 )
            ++m_winui->sizeWriterRevision;
    }
    wxTimePickerCtrlBase::DoSetSize(
        x, y, width, height, sizeFlags);
}

bool wxTimePickerCtrl::ApplyInitialSizeTransaction(
    const wxSize& size, std::uint64_t expectedRevision)
{
    if ( !m_winui || !m_winui->callbackState )
        return false;

    wxWinUITimePickerImpl * const impl = m_winui.get();
    const auto state = m_winui->callbackState;
    const std::uint64_t generation = state->Generation();
    const auto getExactOwner = [&]() -> wxTimePickerCtrl *
    {
        wxTimePickerCtrl * const owner = state->GetOwner(generation);
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
    wxTimePickerCtrl *owner = getExactOwner();
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

void wxTimePickerCtrl::OnLayoutLoaded()
{
    if ( !m_winui || !m_winui->callbackState || !m_winui->root )
        return;

    wxWinUITimePickerImpl * const impl = m_winui.get();
    const auto state = m_winui->callbackState;
    const std::uint64_t generation = state->Generation();
    const MUXC::Grid layoutRoot = m_winui->root;
    const auto getExactOwner = [&]() -> wxTimePickerCtrl *
    {
        wxTimePickerCtrl * const owner = state->GetOwner(generation);
        return owner && owner->m_winui &&
               owner->m_winui.get() == impl &&
               owner->m_winui->callbackState == state &&
               owner->m_winui->root == layoutRoot
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
        wxWinUILogException("WinUI TimePicker Loaded XamlRoot", e);
        return;
    }

    wxTimePickerCtrl *owner = getExactOwner();
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
    owner->ApplyInitialSizeTransaction(
        impl->initialSizeRequest, impl->initialSizeWriterRevision);
}

bool wxTimePickerCtrl::Create(wxWindow *parent,
                              wxWindowID id,
                              const wxDateTime& dt,
                              const wxPoint& pos,
                              const wxSize& size,
                              long style,
                              const wxValidator& validator,
                              const wxString& name)
{
    style = (style & ~wxBORDER_MASK) | wxBORDER_NONE;

    if ( !wxControl::Create(parent, id, pos, size, style, validator, name) )
        return false;

    m_value = wxWinUINormalizeTime(
        dt.IsValid() ? dt : wxDateTime::Now());
    if ( !m_value.IsValid() )
        return false;

    m_winui.reset(new wxWinUITimePickerImpl);
    m_winui->callbackState =
        std::make_shared<wxWinUITimeCallbackState>(this);
    wxWinUITimePickerImpl * const createImpl = m_winui.get();
    const std::shared_ptr<wxWinUITimeCallbackState> createState =
        m_winui->callbackState;
    const std::uint64_t createGeneration =
        createState->Generation();
    if ( !m_winui->host.Initialize(this) )
    {
        m_winui->Close();
        return false;
    }

    MUXC::TimePicker createPeer{ nullptr };
    try
    {
        m_winui->picker = MUXC::TimePicker();
        createPeer = m_winui->picker;
        const winrt::hstring language =
            gs_winuiTimeLanguageForTesting;
        if ( !language.empty() )
            m_winui->picker.Language(language);
        winrt::hstring clock;
        m_winui->localePattern = wxWinUIGetLocaleTimePattern(&clock);
        if ( clock.empty() || m_winui->localePattern.empty() )
        {
            m_winui->Close();
            return false;
        }
        m_winui->twelveHour = clock == L"12HourClock";
        m_winui->picker.ClockIdentifier(clock);
        m_winui->picker.Visibility(MUX::Visibility::Collapsed);
        m_winui->hour = wxWinUICreateTimeBox(
            m_winui->twelveHour ? 1 : 0,
            m_winui->twelveHour ? 12 : 23,
            2, 72, language);
        m_winui->minute = wxWinUICreateTimeBox(
            0, 59, 2, 72, language);
        m_winui->seconds = wxWinUICreateTimeBox(
            0, 59, 2, 72, language);
        if ( m_winui->twelveHour )
        {
            m_winui->period = MUXC::ComboBox();
            if ( !language.empty() )
                m_winui->period.Language(language);
            std::array<winrt::hstring, 2> periodLabels;
            if ( !wxWinUIGetPeriodLabels(clock, &periodLabels) )
            {
                m_winui->Close();
                return false;
            }
            m_winui->period.Items().Append(
                winrt::box_value(periodLabels[0]));
            m_winui->period.Items().Append(
                winrt::box_value(periodLabels[1]));
            m_winui->period.SelectedIndex(0);
            m_winui->period.MinWidth(76);
        }
        MUXA::AutomationProperties::SetName(
            m_winui->hour, wxWinUIToHString(_("Hour")));
        MUXA::AutomationProperties::SetName(
            m_winui->minute, wxWinUIToHString(_("Minute")));
        MUXA::AutomationProperties::SetName(
            m_winui->seconds, wxWinUIToHString(_("Second")));
        if ( m_winui->period )
        {
            MUXA::AutomationProperties::SetName(
                m_winui->period, wxWinUIToHString(_("Period")));
        }
        m_winui->root = MUXC::Grid();
        if ( !language.empty() )
            m_winui->root.Language(language);

        const std::shared_ptr<wxWinUITimeCallbackState> callbackState =
            m_winui->callbackState;
        const std::uint64_t generation =
            callbackState->Generation();
        m_winui->initialSizeRequest = size;
        m_winui->initialSizePending = true;
        const MUXC::Grid layoutRoot = m_winui->root;
        m_winui->rootLoadedToken = layoutRoot.Loaded(
            [callbackState, generation, createImpl, layoutRoot](
                WF::IInspectable const&,
                MUX::RoutedEventArgs const&)
            {
                wxTimePickerCtrl * const owner =
                    callbackState->GetOwner(generation);
                if ( !owner || !owner->m_winui ||
                     owner->m_winui.get() != createImpl ||
                     owner->m_winui->callbackState != callbackState ||
                     owner->m_winui->root != layoutRoot )
                {
                    return;
                }
                owner->OnLayoutLoaded();
            });
        m_winui->hourLoadedToken = m_winui->hour.Loaded(
            [callbackState, generation, createImpl,
             createHour = m_winui->hour](
                WF::IInspectable const& sender,
                MUX::RoutedEventArgs const&)
            {
                wxTimePickerCtrl *owner =
                    callbackState->GetOwner(generation);
                if ( !owner || !owner->m_winui ||
                     owner->m_winui.get() != createImpl ||
                     owner->m_winui->callbackState != callbackState ||
                     owner->m_winui->hour != createHour ||
                     sender.try_as<MUXC::NumberBox>() != createHour )
                {
                    return;
                }

                // The hook is one-shot and runs only inside the actual
                // NumberBox Loaded delegate. It exists to prove that every
                // subsequent template projection is destruction-safe.
                const auto hook =
                    gs_winuiTimeHourLoadedHookForTesting;
                void * const hookData =
                    gs_winuiTimeHourLoadedHookDataForTesting;
                gs_winuiTimeHourLoadedHookForTesting = nullptr;
                gs_winuiTimeHourLoadedHookDataForTesting = nullptr;
                if ( hook )
                    hook(owner, hookData);

                owner = callbackState->GetOwner(generation);
                if ( !owner || !owner->m_winui ||
                     owner->m_winui.get() != createImpl ||
                     owner->m_winui->callbackState != callbackState ||
                     owner->m_winui->hour != createHour )
                {
                    return;
                }
                // ResolveHourSpinButtons() revalidates after ApplyTemplate,
                // UpdateLayout and every visual-tree lookup. It must remain
                // the final operation of this Loaded callback.
                owner->ResolveHourSpinButtons(false);
            });
        // Theme/template replacement doesn't necessarily raise Loaded again.
        // Re-resolve by identity on layout boundaries, revoking delegates from
        // any retired template parts before adopting their replacements.
        m_winui->hourLayoutUpdatedToken = m_winui->hour.LayoutUpdated(
            [callbackState, generation](
                WF::IInspectable const&,
                WF::IInspectable const&)
            {
                wxTimePickerCtrl * const owner =
                    callbackState->GetOwner(generation);
                if ( owner && owner->m_winui &&
                     owner->m_winui->callbackState == callbackState )
                {
                    owner->ResolveHourSpinButtons(false);
                }
            });
        m_winui->selectedTimeChangedToken =
            m_winui->picker.SelectedTimeChanged(
                [callbackState, generation](
                    MUXC::TimePicker const&,
                    MUXC::TimePickerSelectedValueChangedEventArgs const&)
                {
                    wxTimePickerCtrl * const owner =
                        callbackState->GetOwner(generation);
                    if ( !owner || !owner->m_winui ||
                         owner->m_winui->callbackState != callbackState ||
                         callbackState->IsPeerMutationInProgress() )
                    {
                        return;
                    }

                    owner->OnPeerTimeChanged();
                });

        m_winui->hourChangedToken =
            m_winui->hour.ValueChanged(
                [callbackState, generation](
                    MUXC::NumberBox const& sender,
                    MUXC::NumberBoxValueChangedEventArgs const& event)
                {
                    wxTimePickerCtrl * const owner =
                        callbackState->GetOwner(generation);
                    if ( !owner || !owner->m_winui ||
                         owner->m_winui->callbackState != callbackState ||
                         callbackState->IsPeerMutationInProgress() )
                    {
                        return;
                    }

                    wxWinUITimePickerImpl * const callbackImpl =
                        owner->m_winui.get();
                    const MUXC::NumberBox hour =
                        owner->m_winui->hour;
                    if ( !hour ||
                         winrt::get_abi(sender) != winrt::get_abi(hour) )
                    {
                        return;
                    }

                    const auto getExactOwner =
                        [callbackState, generation, callbackImpl, hour]()
                    {
                        wxTimePickerCtrl * const liveOwner =
                            callbackState->GetOwner(generation);
                        return liveOwner && liveOwner->m_winui &&
                               liveOwner->m_winui.get() == callbackImpl &&
                               liveOwner->m_winui->callbackState ==
                                    callbackState &&
                               liveOwner->m_winui->hour == hour
                                    ? liveOwner
                                    : nullptr;
                    };

                    try
                    {
                        const auto pending =
                            std::make_shared<wxWinUITimePendingHourChange>();
                        pending->oldValue = event.OldValue();
                        pending->newValue = event.NewValue();
                        pending->stepDirection =
                            owner->m_winui->nextHourStepDirection;
                        pending->stateRevision =
                            callbackState->StateRevision();
                        owner->m_winui->nextHourStepDirection = 0;
                        owner->m_winui->pendingHourChange = pending;

                        // NumberBox exposes the same ValueChanged event for an
                        // absolute edit and for its template RepeatButtons.
                        // Defer one dispatcher turn so the originating Click
                        // can tag this exact change; direct text/UIA commits
                        // deliberately remain untagged and preserve AM/PM.
                        const bool queued = hour.DispatcherQueue().TryEnqueue(
                            [callbackState, generation, callbackImpl,
                             hour, pending]()
                            {
                                wxTimePickerCtrl * const liveOwner =
                                    callbackState->GetOwner(generation);
                                if ( !liveOwner || !liveOwner->m_winui ||
                                     liveOwner->m_winui.get() != callbackImpl ||
                                     liveOwner->m_winui->callbackState !=
                                         callbackState ||
                                     liveOwner->m_winui->hour != hour ||
                                     liveOwner->m_winui->pendingHourChange !=
                                         pending )
                                {
                                    return;
                                }
                                if ( !callbackState->IsStateMutationCurrent(
                                         pending->stateRevision) )
                                {
                                    liveOwner->m_winui
                                        ->pendingHourChange.reset();
                                    return;
                                }

                                liveOwner->m_winui->pendingHourChange.reset();
                                liveOwner->OnPeerHourChanged(
                                    pending->oldValue,
                                    pending->newValue,
                                    pending->stepDirection);
                            });
                        if ( queued )
                            return;

                        wxTimePickerCtrl * const liveOwner = getExactOwner();
                        if ( !liveOwner ||
                             liveOwner->m_winui->pendingHourChange != pending )
                        {
                            return;
                        }
                        if ( !callbackState->IsStateMutationCurrent(
                                 pending->stateRevision) )
                        {
                            liveOwner->m_winui->pendingHourChange.reset();
                            return;
                        }
                        liveOwner->m_winui->pendingHourChange.reset();
                        liveOwner->OnPeerHourChanged(
                            pending->oldValue,
                            pending->newValue,
                            pending->stepDirection);
                    }
                    catch ( const winrt::hresult_error& e )
                    {
                        wxWinUILogException(
                            "WinUI TimePicker hour change queue", e);
                        if ( wxTimePickerCtrl * const liveOwner =
                                 getExactOwner() )
                        {
                            liveOwner->m_winui->pendingHourChange.reset();
                            liveOwner->ApplyToPeer();
                        }
                    }
                });

        const auto partChanged =
                [callbackState, generation](
                    MUXC::NumberBox const&,
                    MUXC::NumberBoxValueChangedEventArgs const&)
                {
                    wxTimePickerCtrl * const owner =
                        callbackState->GetOwner(generation);
                    if ( !owner || !owner->m_winui ||
                         owner->m_winui->callbackState != callbackState ||
                         callbackState->IsPeerMutationInProgress() )
                    {
                        return;
                    }

                    owner->OnPeerTimeChanged();
                };
        m_winui->minuteChangedToken =
            m_winui->minute.ValueChanged(partChanged);
        m_winui->secondsChangedToken =
            m_winui->seconds.ValueChanged(partChanged);
        if ( m_winui->period )
        {
            m_winui->periodChangedToken =
                m_winui->period.SelectionChanged(
                    [callbackState, generation](
                        WF::IInspectable const&,
                        MUXC::SelectionChangedEventArgs const&)
                    {
                        wxTimePickerCtrl * const owner =
                            callbackState->GetOwner(generation);
                        if ( !owner || !owner->m_winui ||
                             owner->m_winui->callbackState !=
                                 callbackState ||
                             callbackState->IsPeerMutationInProgress() )
                        {
                            return;
                        }
                        owner->OnPeerTimeChanged();
                    });
        }

        bool completePattern = false;
        const auto layout = wxWinUIParseTimePattern(
            m_winui->localePattern, &completePattern);
        const auto countKind = [&layout](wxWinUITimeLayoutKind kind)
        {
            return std::count_if(
                layout.begin(), layout.end(),
                [kind](const wxWinUITimeLayoutToken& token)
                {
                    return token.kind == kind;
                });
        };
        if ( !completePattern ||
             countKind(wxWinUITimeLayoutKind::Hour) != 1 ||
             countKind(wxWinUITimeLayoutKind::Minute) != 1 ||
             countKind(wxWinUITimeLayoutKind::Second) != 1 ||
             countKind(wxWinUITimeLayoutKind::Period) !=
                 (m_winui->twelveHour ? 1 : 0) )
        {
            m_winui->Close();
            return false;
        }
        int column = 0;
        for ( const wxWinUITimeLayoutToken& token : layout )
        {
            MUXC::ColumnDefinition tokenColumn;
            tokenColumn.Width(MUX::GridLengthHelper::Auto());
            m_winui->root.ColumnDefinitions().Append(tokenColumn);
            MUX::FrameworkElement element{ nullptr };
            int orderSlot = -1;
            switch ( token.kind )
            {
                case wxWinUITimeLayoutKind::Hour:
                    element = m_winui->hour;
                    orderSlot = 0;
                    break;
                case wxWinUITimeLayoutKind::Minute:
                    element = m_winui->minute;
                    orderSlot = 1;
                    break;
                case wxWinUITimeLayoutKind::Second:
                    element = m_winui->seconds;
                    orderSlot = 2;
                    break;
                case wxWinUITimeLayoutKind::Period:
                    if ( m_winui->period )
                        element = m_winui->period;
                    orderSlot = 3;
                    break;
                case wxWinUITimeLayoutKind::Literal:
                {
                    MUXC::TextBlock literal;
                    literal.Text(wxWinUIToHString(token.text));
                    literal.Margin(MUX::Thickness{2, 0, 2, 0});
                    literal.VerticalAlignment(
                        MUX::VerticalAlignment::Center);
                    element = literal;
                    break;
                }
            }
            if ( element )
            {
                MUXC::Grid::SetColumn(element, column);
                m_winui->root.Children().Append(element);
                if ( orderSlot >= 0 )
                {
                    m_winui->fieldOrder[orderSlot] = column;
                    m_winui->focusParts.push_back(
                        element.as<MUXC::Control>());
                }
            }
            ++column;
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
                        wxTimePickerCtrl * const owner =
                            callbackState->GetOwner(generation);
                        if ( !owner || !owner->m_winui ||
                             owner->m_winui->callbackState !=
                                 callbackState )
                        {
                            return;
                        }

                        wxWinUITimePickerImpl * const impl =
                            owner->m_winui.get();
                        const MUXC::Control current =
                            sender.try_as<MUXC::Control>();
                        if ( current && impl->hour &&
                             winrt::get_abi(current) ==
                                 winrt::get_abi(impl->hour) )
                        {
                            if ( key == WS::VirtualKey::Up )
                                impl->nextHourStepDirection = 1;
                            else if ( key == WS::VirtualKey::Down )
                                impl->nextHourStepDirection = -1;
                            else
                                impl->nextHourStepDirection = 0;
                        }

                        if ( key != WS::VirtualKey::Left &&
                             key != WS::VirtualKey::Right )
                        {
                            return;
                        }

                        const auto it = std::find_if(
                            impl->focusParts.begin(),
                            impl->focusParts.end(),
                            [&current](const MUXC::Control& candidate)
                            {
                                return current && candidate &&
                                       winrt::get_abi(current) ==
                                           winrt::get_abi(candidate);
                            });
                        if ( it == impl->focusParts.end() )
                            return;

                        int step = key == WS::VirtualKey::Right ? 1 : -1;
                        if ( impl->root.FlowDirection() ==
                             MUX::FlowDirection::RightToLeft )
                        {
                            step = -step;
                        }
                        const int index = static_cast<int>(
                            std::distance(impl->focusParts.begin(), it));
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

        const bool peerApplied = ApplyToPeer();
        wxTimePickerCtrl *liveOwner =
            createState->GetOwner(createGeneration);
        if ( !liveOwner || !liveOwner->m_winui ||
             liveOwner->m_winui.get() != createImpl ||
             liveOwner->m_winui->callbackState != createState ||
             liveOwner->m_winui->picker != createPeer )
        {
            return false;
        }
        if ( !peerApplied )
        {
            liveOwner->m_winui->Close();
            return false;
        }

        const MUXC::Grid createRoot = createImpl->root;
        const MUXC::NumberBox createHour = createImpl->hour;
        const MUXC::NumberBox createMinute = createImpl->minute;
        const MUXC::NumberBox createSeconds = createImpl->seconds;
        const bool contentSet =
            createImpl->host.SetContent(createRoot, createHour);
        liveOwner = createState->GetOwner(createGeneration);
        if ( !liveOwner || !liveOwner->m_winui ||
             liveOwner->m_winui.get() != createImpl ||
             liveOwner->m_winui->callbackState != createState ||
             liveOwner->m_winui->picker != createPeer ||
             liveOwner->m_winui->root != createRoot ||
             liveOwner->m_winui->hour != createHour ||
             liveOwner->m_winui->minute != createMinute ||
             liveOwner->m_winui->seconds != createSeconds )
        {
            return false;
        }
        if ( !contentSet )
        {
            liveOwner->m_winui->Close();
            return false;
        }

        createImpl->host.ForceRender();
        liveOwner = createState->GetOwner(createGeneration);
        if ( !liveOwner || !liveOwner->m_winui ||
             liveOwner->m_winui.get() != createImpl ||
             liveOwner->m_winui->callbackState != createState ||
             liveOwner->m_winui->picker != createPeer ||
             liveOwner->m_winui->hour != createHour )
        {
            return false;
        }

        liveOwner->ResolveHourSpinButtons(true);
        liveOwner = createState->GetOwner(createGeneration);
        if ( !liveOwner || !liveOwner->m_winui ||
             liveOwner->m_winui.get() != createImpl ||
             liveOwner->m_winui->callbackState != createState ||
             liveOwner->m_winui->hour != createHour )
        {
            return false;
        }

        const MUXC::Control createFocus =
            createImpl->focusParts.empty()
                ? createHour.as<MUXC::Control>()
                : createImpl->focusParts.front();
        liveOwner->m_winui->host.SetPreferredFocus(createFocus);
        liveOwner = createState->GetOwner(createGeneration);
        if ( !liveOwner || !liveOwner->m_winui ||
             liveOwner->m_winui.get() != createImpl ||
             liveOwner->m_winui->callbackState != createState ||
             liveOwner->m_winui->picker != createPeer )
        {
            return false;
        }
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI TimePicker creation", e);
        wxTimePickerCtrl * const liveOwner =
            createState->GetOwner(createGeneration);
        if ( liveOwner && liveOwner->m_winui &&
             liveOwner->m_winui.get() == createImpl &&
             liveOwner->m_winui->callbackState == createState )
        {
            liveOwner->m_winui->Close();
        }
        return false;
    }

    wxTimePickerCtrl *liveOwner =
        createState->GetOwner(createGeneration);
    if ( !liveOwner || !liveOwner->m_winui ||
         liveOwner->m_winui.get() != createImpl ||
         liveOwner->m_winui->callbackState != createState ||
         liveOwner->m_winui->picker != createPeer )
    {
        return false;
    }

    // The locale-specific field composite is created after wxControl cached
    // its fallback size. Recompute from the real XAML tree for the caller's
    // default-size request.
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
         liveOwner->m_winui->picker != createPeer )
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

void wxTimePickerCtrl::SetValue(const wxDateTime& dt)
{
    wxCHECK_RET( dt.IsValid(), wxT("invalid time") );

    const wxDateTime value = wxWinUINormalizeTime(dt);
    if ( !m_winui || !m_winui->callbackState )
        return;
    wxWinUITimePickerImpl * const updateImpl = m_winui.get();
    const std::shared_ptr<wxWinUITimeCallbackState> state =
        m_winui->callbackState;
    const std::uint64_t generation = state->Generation();
    const std::uint64_t revision = state->BeginStateMutation();
    if ( !value.IsValid() || !ApplyValueToPeer(value) )
        return;

    wxTimePickerCtrl * const owner = state->GetOwner(generation);
    if ( owner && owner->m_winui &&
         owner->m_winui.get() == updateImpl &&
         owner->m_winui->callbackState == state &&
         state->IsStateMutationCurrent(revision) )
    {
        owner->m_value = value;
    }
}

wxDateTime wxTimePickerCtrl::GetValue() const
{
    return m_value;
}

wxSize wxTimePickerCtrl::DoGetBestSize() const
{
    wxSize size(m_winui && m_winui->twelveHour ? 332 : 248, 32);
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
        size, const_cast<wxTimePickerCtrl*>(this));
}

bool wxTimePickerCtrl::ApplyToPeer()
{
    return ApplyValueToPeer(m_value);
}

bool wxTimePickerCtrl::ApplyValueToPeer(const wxDateTime& value)
{
    if ( !m_winui || !m_winui->picker || !m_winui->hour ||
         !m_winui->minute || !m_winui->seconds )
        return true;
    if ( !m_winui->callbackState )
        return false;

    WF::TimeSpan peerValue{};
    if ( !wxWinUIToTimeSpan(value, &peerValue) )
        return false;

    wxWinUITimePickerImpl * const updateImpl = m_winui.get();
    const std::shared_ptr<wxWinUITimeCallbackState> state =
        m_winui->callbackState;
    const std::uint64_t generation = state->Generation();
    const std::uint64_t revision = state->StateRevision();
    const MUXC::TimePicker picker = m_winui->picker;
    const MUXC::NumberBox hour = m_winui->hour;
    const MUXC::NumberBox minute = m_winui->minute;
    const MUXC::NumberBox seconds = m_winui->seconds;
    const MUXC::ComboBox period = m_winui->period;
    const bool twelveHour = m_winui->twelveHour;
    wxWinUITimePeerMutationGuard guard(state);

    WF::IReference<WF::TimeSpan> previousValue{ nullptr };
    double previousHour = 0;
    double previousMinute = 0;
    double previousSeconds = 0;
    int previousPeriod = -1;
    bool snapshotCaptured = false;
    bool applied = false;
    try
    {
        previousValue = picker.SelectedTime();
        previousHour = hour.Value();
        previousMinute = minute.Value();
        previousSeconds = seconds.Value();
        if ( period )
            previousPeriod = period.SelectedIndex();
        snapshotCaptured = true;

        const wxDateTime::Tm tm = value.GetTm();
        int displayedHour = tm.hour;
        int displayedPeriod = -1;
        if ( twelveHour )
        {
            displayedPeriod = displayedHour >= 12 ? 1 : 0;
            displayedHour %= 12;
            if ( displayedHour == 0 )
                displayedHour = 12;
        }
        picker.SelectedTime(
            winrt::box_value(peerValue)
                .as<WF::IReference<WF::TimeSpan>>());
        hour.Value(displayedHour);
        minute.Value(tm.min);
        seconds.Value(tm.sec);
        if ( period )
            period.SelectedIndex(displayedPeriod);

        if ( const auto actual = picker.SelectedTime() )
        {
            int actualHour = 0;
            int actualMinute = 0;
            int actualSecond = 0;
            const int actualPeriod = period ? period.SelectedIndex() : -1;
            applied =
                wxWinUIReadTimePart(hour, &actualHour) &&
                wxWinUIReadTimePart(minute, &actualMinute) &&
                wxWinUIReadTimePart(seconds, &actualSecond) &&
                wxWinUISameTime(
                    wxWinUIFromDisplayedTime(
                        actualHour, actualMinute, actualSecond,
                        twelveHour, actualPeriod),
                    value) &&
                wxWinUISameTime(
                    wxWinUIFromTimeSpan(actual.Value(), tm.sec),
                    value);
        }
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI TimePicker state update", e);
    }

    if ( !state->IsStateMutationCurrent(revision) )
        return false;

    if ( !applied )
    {
        if ( snapshotCaptured )
        {
            try
            {
                picker.SelectedTime(previousValue);
                hour.Value(previousHour);
                minute.Value(previousMinute);
                seconds.Value(previousSeconds);
                if ( period )
                    period.SelectedIndex(previousPeriod);
            }
            catch ( const winrt::hresult_error& e )
            {
                wxWinUILogException(
                    "WinUI TimePicker state rollback", e);
            }
        }
        return false;
    }

    wxTimePickerCtrl *liveOwner = state->GetOwner(generation);
    if ( !liveOwner || !liveOwner->m_winui ||
         liveOwner->m_winui.get() != updateImpl ||
         liveOwner->m_winui->callbackState != state ||
         liveOwner->m_winui->picker != picker ||
         liveOwner->m_winui->hour != hour ||
         liveOwner->m_winui->minute != minute ||
         liveOwner->m_winui->seconds != seconds ||
         !state->IsStateMutationCurrent(revision) )
    {
        return false;
    }

    liveOwner->m_winui->host.ForceRender();
    liveOwner = state->GetOwner(generation);
    return liveOwner && liveOwner->m_winui &&
           liveOwner->m_winui.get() == updateImpl &&
           liveOwner->m_winui->callbackState == state &&
           liveOwner->m_winui->picker == picker &&
           liveOwner->m_winui->hour == hour &&
           liveOwner->m_winui->minute == minute &&
           liveOwner->m_winui->seconds == seconds &&
           state->IsStateMutationCurrent(revision);
}

bool wxTimePickerCtrl::OnPeerHourChanged(double oldValue,
                                         double newValue,
                                         int stepDirection)
{
    if ( !m_winui || !m_winui->callbackState )
        return false;

    wxWinUITimePickerImpl * const callbackImpl = m_winui.get();
    const std::shared_ptr<wxWinUITimeCallbackState> state =
        m_winui->callbackState;
    const std::uint64_t generation = state->Generation();
    const MUXC::NumberBox hour = m_winui->hour;
    const MUXC::ComboBox period = m_winui->period;
    const auto getExactOwner = [&]()
    {
        wxTimePickerCtrl * const owner = state->GetOwner(generation);
        return owner && owner->m_winui &&
               owner->m_winui.get() == callbackImpl &&
               owner->m_winui->callbackState == state &&
               owner->m_winui->hour == hour &&
               owner->m_winui->period == period
                   ? owner
                   : nullptr;
    };
    const auto restoreCommittedValue = [&]()
    {
        if ( wxTimePickerCtrl * const owner = getExactOwner() )
            owner->ApplyToPeer();
    };

    try
    {
        const bool integralValues =
            std::isfinite(oldValue) && std::isfinite(newValue) &&
            std::floor(oldValue) == oldValue &&
            std::floor(newValue) == newValue;
        const int oldHour = integralValues
            ? static_cast<int>(oldValue)
            : -1;
        const int newHour = integralValues
            ? static_cast<int>(newValue)
            : -1;
        const bool crossesPeriod = period && m_winui->twelveHour &&
            ((stepDirection > 0 && oldHour == 11 && newHour == 12) ||
             (stepDirection < 0 && oldHour == 12 && newHour == 11));

        if ( crossesPeriod )
        {
            const int previousPeriod = period.SelectedIndex();
            if ( !getExactOwner() )
                return false;
            if ( previousPeriod < 0 || previousPeriod > 1 )
            {
                restoreCommittedValue();
                return false;
            }

            {
                wxWinUITimePeerMutationGuard guard(state);
                period.SelectedIndex(1 - previousPeriod);
            }
            if ( !getExactOwner() ||
                 period.SelectedIndex() != 1 - previousPeriod )
            {
                restoreCommittedValue();
                return false;
            }
        }
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException(
            "WinUI TimePicker hour-period transition", e);
        restoreCommittedValue();
        return false;
    }

    wxTimePickerCtrl * const owner = getExactOwner();
    if ( !owner || !owner->OnPeerTimeChanged() )
    {
        restoreCommittedValue();
        return false;
    }
    return true;
}

bool wxTimePickerCtrl::OnPeerTimeChanged()
{
    if ( !m_winui || !m_winui->picker || !m_winui->hour ||
         !m_winui->minute || !m_winui->seconds )
        return false;

    const std::shared_ptr<wxWinUITimeCallbackState> state =
        m_winui->callbackState;
    if ( !state )
        return false;
    const std::uint64_t generation = state->Generation();
    const std::uint64_t revision = state->BeginStateMutation();
    wxWinUITimePickerImpl * const callbackImpl = m_winui.get();
    const MUXC::TimePicker picker = m_winui->picker;
    const MUXC::NumberBox hourPart = m_winui->hour;
    const MUXC::NumberBox minutePart = m_winui->minute;
    const MUXC::NumberBox secondPart = m_winui->seconds;
    const MUXC::ComboBox periodPart = m_winui->period;
    const bool twelveHour = m_winui->twelveHour;
    const auto getExactOwner = [&]()
    {
        wxTimePickerCtrl * const owner = state->GetOwner(generation);
        return owner && owner->m_winui &&
               owner->m_winui.get() == callbackImpl &&
               owner->m_winui->callbackState == state &&
               owner->m_winui->picker == picker &&
               owner->m_winui->hour == hourPart &&
               owner->m_winui->minute == minutePart &&
               owner->m_winui->seconds == secondPart &&
               owner->m_winui->period == periodPart &&
               state->IsStateMutationCurrent(revision)
                   ? owner
                   : nullptr;
    };

    wxDateTime value;
    try
    {
        int hour = 0;
        int minute = 0;
        int second = 0;
        if ( !wxWinUIReadTimePart(hourPart, &hour) ||
             !getExactOwner() ||
             !wxWinUIReadTimePart(minutePart, &minute) ||
             !getExactOwner() ||
             !wxWinUIReadTimePart(secondPart, &second) ||
             !getExactOwner() )
        {
            if ( wxTimePickerCtrl * const owner = getExactOwner() )
                owner->ApplyToPeer();
            return false;
        }

        int period = -1;
        if ( periodPart )
        {
            period = periodPart.SelectedIndex();
            if ( !getExactOwner() )
                return false;
        }
        value = wxWinUIFromDisplayedTime(
            hour, minute, second, twelveHour, period);
        if ( !value.IsValid() )
        {
            if ( wxTimePickerCtrl * const owner = getExactOwner() )
                owner->ApplyToPeer();
            return false;
        }
    }
    catch ( const winrt::hresult_error& )
    {
        return false;
    }

    wxTimePickerCtrl *owner = getExactOwner();
    if ( !owner || !owner->ApplyValueToPeer(value) )
        return false;

    owner = getExactOwner();
    if ( !owner )
        return false;

    if ( wxWinUISameTime(value, owner->m_value) )
        return true;

    owner->m_value = value;
    wxDateEvent event(owner, value, wxEVT_TIME_CHANGED);
    // Event code is allowed to destroy this control. Do not access any member
    // after dispatch returns.
    owner->HandleWindowEvent(event);
    return state->GetOwner(generation) != nullptr;
}

bool wxTimePickerCtrl::WinUISetPeerTimeForTesting(
    const wxDateTime& dt)
{
    if ( !m_winui || !m_winui->picker || !m_winui->hour ||
         !m_winui->minute || !m_winui->seconds ||
         !m_winui->callbackState || !dt.IsValid() )
        return false;

    WF::TimeSpan peerValue{};
    if ( !wxWinUIToTimeSpan(dt, &peerValue) )
        return false;

    const MUXC::TimePicker picker = m_winui->picker;
    const MUXC::NumberBox hour = m_winui->hour;
    const MUXC::NumberBox minute = m_winui->minute;
    const MUXC::NumberBox seconds = m_winui->seconds;
    const MUXC::ComboBox period = m_winui->period;
    const std::shared_ptr<wxWinUITimeCallbackState> state =
        m_winui->callbackState;
    const std::uint64_t generation = state->Generation();
    try
    {
        {
            wxWinUITimePeerMutationGuard guard(state);
            const wxDateTime normalized = wxWinUINormalizeTime(dt);
            const wxDateTime::Tm tm = normalized.GetTm();
            int displayedHour = tm.hour;
            int displayedPeriod = -1;
            if ( m_winui->twelveHour )
            {
                displayedPeriod = displayedHour >= 12 ? 1 : 0;
                displayedHour %= 12;
                if ( displayedHour == 0 )
                    displayedHour = 12;
            }
            picker.SelectedTime(
                winrt::box_value(peerValue)
                    .as<WF::IReference<WF::TimeSpan>>());
            hour.Value(displayedHour);
            minute.Value(tm.min);
            seconds.Value(tm.sec);
            if ( period )
                period.SelectedIndex(displayedPeriod);
        }

        wxTimePickerCtrl * const owner = state->GetOwner(generation);
        if ( !owner || !owner->m_winui ||
             owner->m_winui->callbackState != state ||
             owner->m_winui->picker != picker ||
             owner->m_winui->hour != hour ||
             owner->m_winui->minute != minute ||
             owner->m_winui->seconds != seconds )
        {
            return false;
        }
        owner->OnPeerTimeChanged();
        return true;
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException(
            "WinUI TimePicker test time mutation", e);
        return false;
    }
}

bool wxTimePickerCtrl::WinUIGetPeerTimeForTesting(
    wxDateTime *value) const
{
    if ( !m_winui || !m_winui->picker || !m_winui->hour ||
         !m_winui->minute || !m_winui->seconds || !value )
        return false;

    try
    {
        int hour = 0;
        int minute = 0;
        int second = 0;
        if ( !wxWinUIReadTimePart(m_winui->hour, &hour) ||
             !wxWinUIReadTimePart(m_winui->minute, &minute) ||
             !wxWinUIReadTimePart(m_winui->seconds, &second) )
        {
            return false;
        }
        *value = wxWinUIFromDisplayedTime(
            hour, minute, second, m_winui->twelveHour,
            m_winui->period
                ? m_winui->period.SelectedIndex()
                : -1);
        return value->IsValid();
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException(
            "WinUI TimePicker test state query", e);
        return false;
    }
}

wxString wxTimePickerCtrl::WinUIGetLocaleTimePatternForTesting() const
{
    return m_winui ? m_winui->localePattern : wxString();
}

bool wxTimePickerCtrl::WinUIGetTimeFieldOrderForTesting(
    int *hour, int *minute, int *second, int *period) const
{
    if ( !m_winui || !m_winui->hour || !m_winui->minute ||
         !m_winui->seconds )
    {
        return false;
    }
    if ( hour )
        *hour = m_winui->fieldOrder[0];
    if ( minute )
        *minute = m_winui->fieldOrder[1];
    if ( second )
        *second = m_winui->fieldOrder[2];
    if ( period )
        *period = m_winui->fieldOrder[3];
    return m_winui->fieldOrder[0] >= 0 &&
           m_winui->fieldOrder[1] >= 0 &&
           m_winui->fieldOrder[2] >= 0;
}

bool wxTimePickerCtrl::WinUIGetHourSpinBindingForTesting(
    std::uintptr_t *incrementIdentity,
    std::uintptr_t *decrementIdentity,
    std::uint64_t *generation) const
{
    if ( !m_winui || !incrementIdentity || !decrementIdentity )
        return false;

    *incrementIdentity = reinterpret_cast<std::uintptr_t>(
        winrt::get_abi(m_winui->hourIncrement));
    *decrementIdentity = reinterpret_cast<std::uintptr_t>(
        winrt::get_abi(m_winui->hourDecrement));
    if ( generation )
        *generation = m_winui->hourSpinButtonGeneration;
    return m_winui->hourIncrement && m_winui->hourDecrement &&
           m_winui->hourIncrementToken.value &&
           m_winui->hourDecrementToken.value;
}

void wxTimePickerCtrl::WinUISetHourLoadedHookForTesting(
    WinUIHourLoadedHookForTesting hook, void *data)
{
    gs_winuiTimeHourLoadedHookForTesting = hook;
    gs_winuiTimeHourLoadedHookDataForTesting = hook ? data : nullptr;
}

#endif // wxUSE_TIMEPICKCTRL
