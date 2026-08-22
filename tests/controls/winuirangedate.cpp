///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/winuirangedate.cpp
// Purpose:     deterministic WinUI range/date/time/calendar contract tests
// Author:      wxWidgets development team
// Created:     2026-07-26
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#if defined(__WXWINUI__) && wxUSE_WINUI3

#include "testableframe.h"
#include "waitfor.h"

#include "wx/app.h"
#include "wx/calctrl.h"
#include "wx/datectrl.h"
#include "wx/dateevt.h"
#include "wx/event.h"
#include "wx/gauge.h"
#include "wx/panel.h"
#include "wx/settings.h"
#include "wx/timectrl.h"
#include "wx/uiaction.h"
#include "wx/winui/winui.h"
#include "wx/winui/private/tlwhost.h"

#include <winrt/Microsoft.UI.Dispatching.h>
#include <winrt/Microsoft.UI.Xaml.Automation.h>
#include <winrt/Microsoft.UI.Xaml.Automation.Peers.h>
#include <winrt/Microsoft.UI.Xaml.Automation.Provider.h>
#include <winrt/Microsoft.UI.Xaml.Controls.Primitives.h>
#include <winrt/Microsoft.UI.Xaml.Input.h>
#include <winrt/Microsoft.UI.Xaml.Markup.h>
#include <winrt/Microsoft.UI.Xaml.Media.h>
#include <winrt/Windows.Globalization.h>
#include <winrt/Windows.Globalization.NumberFormatting.h>

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <functional>
#include <initializer_list>
#include <limits>
#include <memory>
#include <utility>
#include <vector>

namespace
{

namespace MUX = winrt::Microsoft::UI::Xaml;
namespace MUXA = winrt::Microsoft::UI::Xaml::Automation;
namespace MUXAP = winrt::Microsoft::UI::Xaml::Automation::Peers;
namespace MUXAPR = winrt::Microsoft::UI::Xaml::Automation::Provider;
namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;
namespace MUXCP = winrt::Microsoft::UI::Xaml::Controls::Primitives;
namespace MUXD = winrt::Microsoft::UI::Dispatching;
namespace MUXMK = winrt::Microsoft::UI::Xaml::Markup;
namespace MUXM = winrt::Microsoft::UI::Xaml::Media;
namespace WF = winrt::Windows::Foundation;
namespace WG = winrt::Windows::Globalization;
namespace WGNF = winrt::Windows::Globalization::NumberFormatting;

winrt::hstring wxWinUITestFormatDecimal(
    const winrt::hstring& language,
    unsigned integerDigits,
    std::int64_t value)
{
    const WG::GeographicRegion region;
    WGNF::DecimalFormatter formatter(
        std::vector<winrt::hstring>{language},
        region.CodeTwoLetter());
    formatter.IntegerDigits(integerDigits);
    formatter.FractionDigits(0);
    return formatter.FormatInt(value);
}

class wxWinUITestLanguageOverride
{
public:
    explicit wxWinUITestLanguageOverride(const winrt::hstring& language)
    {
        const wxString value(language.c_str());
#if wxUSE_DATEPICKCTRL
        m_previousDate =
            wxDatePickerCtrl::WinUISetLanguageForTesting(value);
#endif
#if wxUSE_TIMEPICKCTRL
        m_previousTime =
            wxTimePickerCtrl::WinUISetLanguageForTesting(value);
#endif
#if wxUSE_CALENDARCTRL
        m_previousCalendar =
            wxCalendarCtrl::WinUISetLanguageForTesting(value);
#endif
    }

    ~wxWinUITestLanguageOverride()
    {
#if wxUSE_CALENDARCTRL
        wxCalendarCtrl::WinUISetLanguageForTesting(m_previousCalendar);
#endif
#if wxUSE_TIMEPICKCTRL
        wxTimePickerCtrl::WinUISetLanguageForTesting(m_previousTime);
#endif
#if wxUSE_DATEPICKCTRL
        wxDatePickerCtrl::WinUISetLanguageForTesting(m_previousDate);
#endif
    }

private:
#if wxUSE_DATEPICKCTRL
    wxString m_previousDate;
#endif
#if wxUSE_TIMEPICKCTRL
    wxString m_previousTime;
#endif
#if wxUSE_CALENDARCTRL
    wxString m_previousCalendar;
#endif
};

struct wxWinUITestCalendarMonthKey
{
    int era = 0;
    int year = 0;
    int month = 0;

    bool operator==(const wxWinUITestCalendarMonthKey& other) const
    {
        return era == other.era && year == other.year &&
               month == other.month;
    }
};

bool wxWinUITestReadCalendarMonth(
    const winrt::hstring& identifier,
    const wxDateTime& civil,
    wxWinUITestCalendarMonthKey *key)
{
    if ( identifier.empty() || !civil.IsValid() || !key )
        return false;

    wxDateTime date(civil);
    date.ResetTime();
    SYSTEMTIME localTime{};
    localTime.wYear = static_cast<WORD>(date.GetYear());
    localTime.wMonth = static_cast<WORD>(date.GetMonth() + 1);
    localTime.wDay = static_cast<WORD>(date.GetDay());
    localTime.wHour = 12;
    SYSTEMTIME utcTime{};
    if ( !::TzSpecificLocalTimeToSystemTime(
             nullptr, &localTime, &utcTime) )
    {
        return false;
    }
    FILETIME fileTime{};
    if ( !::SystemTimeToFileTime(&utcTime, &fileTime) )
        return false;

    try
    {
        WG::Calendar calendar;
        calendar.ChangeCalendarSystem(identifier);
        calendar.SetDateTime(winrt::clock::from_FILETIME(fileTime));
        key->era = calendar.Era();
        key->year = calendar.Year();
        key->month = calendar.Month();
        return true;
    }
    catch ( const winrt::hresult_error& )
    {
        return false;
    }
}

class wxWinUITestHolidayAuthority : public wxDateTimeHolidayAuthority
{
public:
    wxWinUITestHolidayAuthority(
        const wxDateTime& holiday,
        std::function<void()> callback = {})
        : m_holiday(holiday), m_callback(std::move(callback))
    {
    }

    void SetHoliday(const wxDateTime& holiday)
    {
        m_holiday = holiday;
    }

    unsigned GetRangeRequestCount() const
    {
        return m_rangeRequestCount;
    }

protected:
    bool DoIsHoliday(const wxDateTime& date) const override
    {
        return date == m_holiday;
    }

    size_t DoGetHolidaysInRange(
        const wxDateTime& first,
        const wxDateTime& last,
        wxDateTimeArray& holidays) const override
    {
        ++m_rangeRequestCount;
        if ( m_callback && !m_callbackDelivered )
        {
            m_callbackDelivered = true;
            m_callback();
        }
        holidays.Clear();
        if ( m_holiday >= first && m_holiday <= last )
        {
            holidays.Add(m_holiday);
            return 1;
        }
        return 0;
    }

private:
    wxDateTime m_holiday;
    std::function<void()> m_callback;
    mutable bool m_callbackDelivered = false;
    mutable unsigned m_rangeRequestCount = 0;
};

class wxWinUITestHolidayAuthorityScope
{
public:
    explicit wxWinUITestHolidayAuthorityScope(
        wxDateTimeHolidayAuthority *authority)
    {
        wxDateTimeHolidayAuthority::ClearAllAuthorities();
        wxDateTimeHolidayAuthority::AddAuthority(authority);
    }

    ~wxWinUITestHolidayAuthorityScope()
    {
        wxDateTimeHolidayAuthority::ClearAllAuthorities();
        wxDateTimeHolidayAuthority::AddAuthority(
            new wxDateTimeWorkDays);
    }
};

MUX::UIElement wxWinUIGetHostedDateElement(wxWindow *window)
{
    wxWinUITopLevelHost * const host =
        wxWinUITopLevelHost::FindSlotOwner(window);
    if ( !host )
        return nullptr;
    host->FlushSync();
    wxWinUISlot * const slot = host->FindSlot(window);
    return slot ? slot->GetContent() : nullptr;
}

template <typename T>
void wxWinUICollectDateElements(
    const MUX::DependencyObject& root,
    std::vector<T> *elements)
{
    if ( !root || !elements )
        return;
    if ( const T element = root.try_as<T>() )
        elements->push_back(element);

    const int count = MUXM::VisualTreeHelper::GetChildrenCount(root);
    for ( int n = 0; n != count; ++n )
    {
        wxWinUICollectDateElements<T>(
            MUXM::VisualTreeHelper::GetChild(root, n), elements);
    }
}

bool wxWinUIIsFocusedWithin(
    MUX::DependencyObject focused,
    const MUX::DependencyObject& ancestor)
{
    while ( focused )
    {
        if ( winrt::get_abi(focused) == winrt::get_abi(ancestor) )
            return true;
        focused = MUXM::VisualTreeHelper::GetParent(focused);
    }
    return false;
}

std::vector<MUXC::Control> wxWinUIGetCompositeParts(wxWindow *window)
{
    const MUX::UIElement content = wxWinUIGetHostedDateElement(window);
    std::vector<MUXC::Control> result;
    const int count = MUXM::VisualTreeHelper::GetChildrenCount(content);
    for ( int n = 0; n != count; ++n )
    {
        const MUX::DependencyObject child =
            MUXM::VisualTreeHelper::GetChild(content, n);
        if ( child.try_as<MUXC::NumberBox>() ||
             child.try_as<MUXC::ComboBox>() ||
             child.try_as<MUXC::Button>() )
        {
            result.push_back(child.as<MUXC::Control>());
        }
    }
    std::sort(
        result.begin(), result.end(),
        [](const MUXC::Control& lhs, const MUXC::Control& rhs)
        {
            return MUXC::Grid::GetColumn(lhs) <
                   MUXC::Grid::GetColumn(rhs);
        });
    return result;
}

void wxWinUICheckCompositeKeyboardAndAutomation(wxWindow *window)
{
    // The shared DesktopWindowXamlSource attaches slot content on the next UI
    // turn. Wait for the real Loaded/template path before checking geometry,
    // focus and island keyboard routing.
    wxYield();
    wxWinUITopLevelHost * const host =
        wxWinUITopLevelHost::FindSlotOwner(window);
    REQUIRE(host);
    const MUX::UIElement content = wxWinUIGetHostedDateElement(window);
    const MUX::FrameworkElement root =
        content.try_as<MUX::FrameworkElement>();
    REQUIRE(root);

    const auto parts = wxWinUIGetCompositeParts(window);
    REQUIRE(parts.size() >= 3);
    for ( const MUXC::Control& part : parts )
    {
        CHECK_FALSE(MUXA::AutomationProperties::GetName(part).empty());
    }

    const float infinity = std::numeric_limits<float>::infinity();
    root.Measure(WF::Size{infinity, infinity});
    const WF::Size desired = root.DesiredSize();
    const wxSize best = window->GetBestSize();
    CHECK(best.x >= window->FromDIP(
        static_cast<int>(std::ceil(desired.Width))));
    CHECK(best.y >= window->FromDIP(
        static_cast<int>(std::ceil(desired.Height))));

    // Enter through the wx focus pipeline first. Directly focusing a XAML
    // child can legitimately fail while the shared island is still retiring
    // the previously destroyed slot, and doesn't exercise wxWindow::SetFocus
    // or the host's preferred-focus contract at all.
    window->SetFocus();
    REQUIRE(WaitFor("composite owner keyboard focus", [&]()
    {
        const MUX::DependencyObject focused =
            MUX::Input::FocusManager::GetFocusedElement(
                host->GetXamlRoot())
                .try_as<MUX::DependencyObject>();
        return wxWinUIIsFocusedWithin(focused, parts[0]);
    }, 1000));
    const WXWPARAM key = root.FlowDirection() ==
                                 MUX::FlowDirection::RightToLeft
                             ? VK_LEFT
                             : VK_RIGHT;
    MSG down = {};
    down.hwnd = ::GetFocus();
    down.message = WM_KEYDOWN;
    down.wParam = key;
    down.lParam = 1 |
        (static_cast<LPARAM>(::MapVirtualKeyW(
             static_cast<UINT>(key), MAPVK_VK_TO_VSC)) << 16);
    REQUIRE(down.hwnd);
    REQUIRE(wxWinUI3DispatchIslandKeyboard(&down));
    MSG up = down;
    up.message = WM_KEYUP;
    up.lParam |= static_cast<LPARAM>(0xC0000000u);
    REQUIRE(wxWinUI3DispatchIslandKeyboard(&up));
    REQUIRE(WaitFor("composite keyboard field navigation", [&]()
    {
        const MUX::DependencyObject focused =
            MUX::Input::FocusManager::GetFocusedElement(
                host->GetXamlRoot())
                .try_as<MUX::DependencyObject>();
        return wxWinUIIsFocusedWithin(focused, parts[1]);
    }, 1000));
}

#if wxUSE_TIMEPICKCTRL

bool wxWinUIDispatchTimeKey(WXHWND hwnd, WXWPARAM key)
{
    if ( !hwnd )
        return false;
    MSG down = {};
    down.hwnd = hwnd;
    down.message = WM_KEYDOWN;
    down.wParam = key;
    down.lParam = 1 |
        (static_cast<LPARAM>(::MapVirtualKeyW(
             static_cast<UINT>(key), MAPVK_VK_TO_VSC)) << 16);
    if ( !wxWinUI3DispatchIslandKeyboard(&down) )
        return false;

    MSG up = down;
    up.message = WM_KEYUP;
    up.lParam |= static_cast<LPARAM>(0xC0000000u);
    return wxWinUI3DispatchIslandKeyboard(&up);
}

bool wxWinUIDispatchTimeCharacter(WXHWND hwnd, wchar_t character)
{
    if ( !hwnd )
        return false;
    MSG message = {};
    message.hwnd = hwnd;
    message.message = WM_CHAR;
    message.wParam = static_cast<WPARAM>(character);
    message.lParam = 1;
    return wxWinUI3DispatchIslandKeyboard(&message);
}

struct wxWinUITimeSpinParts
{
    MUXC::NumberBox hour{ nullptr };
    MUXC::ComboBox period{ nullptr };
    MUXCP::RepeatButton increment{ nullptr };
    MUXCP::RepeatButton decrement{ nullptr };
};

wxWinUITimeSpinParts wxWinUIGetTimeSpinParts(
    wxTimePickerCtrl *picker)
{
    wxWinUITimeSpinParts result;
    if ( !picker )
        return result;

    wxYield();
    if ( wxWinUITopLevelHost * const host =
             wxWinUITopLevelHost::FindSlotOwner(picker) )
    {
        host->FlushSync();
    }

    int hourColumn = -1;
    int periodColumn = -1;
    if ( !picker->WinUIGetTimeFieldOrderForTesting(
             &hourColumn, nullptr, nullptr, &periodColumn) )
    {
        return result;
    }

    const auto parts = wxWinUIGetCompositeParts(picker);
    for ( const MUXC::Control& part : parts )
    {
        const int column = MUXC::Grid::GetColumn(part);
        if ( column == hourColumn )
            result.hour = part.try_as<MUXC::NumberBox>();
        if ( column == periodColumn )
            result.period = part.try_as<MUXC::ComboBox>();
    }
    if ( !result.hour || !result.period )
        return result;

    result.hour.ApplyTemplate();
    result.hour.UpdateLayout();
    if ( wxWinUITopLevelHost * const host =
             wxWinUITopLevelHost::FindSlotOwner(picker) )
    {
        host->FlushSync();
    }

    std::vector<MUXCP::RepeatButton> buttons;
    wxWinUICollectDateElements(result.hour, &buttons);
    for ( const MUXCP::RepeatButton& button : buttons )
    {
        const winrt::hstring name = button.Name();
        if ( name == L"UpSpinButton" )
            result.increment = button;
        else if ( name == L"DownSpinButton" )
            result.decrement = button;
    }
    return result;
}

MUXC::ControlTemplate wxWinUICreateTimeHourReplacementTemplate()
{
    // Use a fresh supported NumberBox template rather than attempting to
    // reapply the effective theme ControlTemplate object. WinUI owns that
    // object, and a NumberBox whose local Template was explicitly cleared can
    // reject reapplication of the theme instance while restoring its internal
    // TextBox template bindings. The named controls below are the real
    // NumberBox template contract and produce discoverable UIA peers.
    return MUXMK::XamlReader::Load(
        LR"xaml(
<ControlTemplate
    xmlns="http://schemas.microsoft.com/winfx/2006/xaml/presentation"
    xmlns:x="http://schemas.microsoft.com/winfx/2006/xaml"
    xmlns:muxc="using:Microsoft.UI.Xaml.Controls"
    TargetType="muxc:NumberBox">
    <Grid>
        <Grid.ColumnDefinitions>
            <ColumnDefinition Width="*" />
            <ColumnDefinition Width="Auto" />
            <ColumnDefinition Width="Auto" />
        </Grid.ColumnDefinitions>
        <TextBox x:Name="InputBox" Grid.Column="0" />
        <RepeatButton x:Name="UpSpinButton"
                      Grid.Column="1"
                      Width="32"
                      Content="+" />
        <RepeatButton x:Name="DownSpinButton"
                      Grid.Column="2"
                      Width="32"
                      Content="-" />
    </Grid>
</ControlTemplate>)xaml")
        .as<MUXC::ControlTemplate>();
}

#endif // wxUSE_TIMEPICKCTRL

MUXAPR::IInvokeProvider wxWinUIGetInvokeProvider(
    const MUXCP::ButtonBase& button);

#if wxUSE_CALENDARCTRL

struct wxWinUICalendarNavigationButtons
{
    MUXCP::ButtonBase decrement{ nullptr };
    MUXCP::ButtonBase increment{ nullptr };
};

struct wxWinUICalendarNavigationHitProbe
{
    std::uintptr_t identity = 0;
    bool mapped = false;
    WF::Point clientPixels{};
    wxCalendarHitTestResult hit = wxCAL_HITTEST_NOWHERE;
};

struct wxWinUICalendarWeekOverlay
{
    MUXC::Grid root{ nullptr };
    MUXC::Canvas canvas{ nullptr };
    std::vector<MUXC::TextBlock> labels;
};

wxWinUICalendarWeekOverlay wxWinUIGetCalendarWeekOverlay(
    wxCalendarCtrl *calendar)
{
    wxWinUICalendarWeekOverlay result;
    result.root = wxWinUIGetHostedDateElement(calendar)
                      .try_as<MUXC::Grid>();
    if ( !result.root )
        return result;

    const auto rootChildren = result.root.Children();
    for ( std::uint32_t n = 0; n != rootChildren.Size(); ++n )
    {
        if ( const MUXC::Canvas canvas =
                 rootChildren.GetAt(n).try_as<MUXC::Canvas>() )
        {
            result.canvas = canvas;
            break;
        }
    }
    if ( !result.canvas )
        return result;

    const auto rows = result.canvas.Children();
    result.labels.reserve(rows.Size());
    for ( std::uint32_t n = 0; n != rows.Size(); ++n )
    {
        const MUXC::Border row =
            rows.GetAt(n).try_as<MUXC::Border>();
        if ( row )
        {
            if ( const MUXC::TextBlock label =
                     row.Child().try_as<MUXC::TextBlock>() )
            {
                result.labels.push_back(label);
            }
        }
    }
    return result;
}

wxWinUICalendarNavigationButtons wxWinUIGetCalendarNavigationButtons(
    wxCalendarCtrl *calendar,
    std::vector<wxWinUICalendarNavigationHitProbe> *probes = nullptr)
{
    wxWinUICalendarNavigationButtons result;
    const MUX::UIElement content =
        wxWinUIGetHostedDateElement(calendar);
    std::vector<MUXCP::ButtonBase> buttons;
    wxWinUICollectDateElements(content, &buttons);
    for ( const MUXCP::ButtonBase& button : buttons )
    {
        const MUX::FrameworkElement element =
            button.as<MUX::FrameworkElement>();
        if ( element.ActualWidth() <= 0 || element.ActualHeight() <= 0 ||
             element.Visibility() != MUX::Visibility::Visible )
        {
            continue;
        }

        WF::Point clientPixels{};
        wxWinUICalendarNavigationHitProbe probe;
        probe.identity = reinterpret_cast<std::uintptr_t>(
            winrt::get_abi(button));
        probe.mapped =
            wxWinUIVisualCoordinates::ElementPointToClient(
                 calendar, element,
                 WF::Point{
                     static_cast<float>(element.ActualWidth() / 2.0),
                     static_cast<float>(element.ActualHeight() / 2.0)},
                 &clientPixels) == wxWinUICoordinateResult::Mapped;
        probe.clientPixels = clientPixels;
        if ( !probe.mapped )
        {
            if ( probes )
                probes->push_back(probe);
            continue;
        }
        const wxPoint point(
            static_cast<int>(std::lround(clientPixels.X)),
            static_cast<int>(std::lround(clientPixels.Y)));
        const wxCalendarHitTestResult hit = calendar->HitTest(point);
        probe.hit = hit;
        if ( probes )
            probes->push_back(probe);
        if ( hit == wxCAL_HITTEST_DECMONTH )
            result.decrement = button;
        else if ( hit == wxCAL_HITTEST_INCMONTH )
            result.increment = button;
    }
    return result;
}

bool wxWinUIWaitCalendarDispatcherBarrier(
    const MUXC::CalendarView& calendar,
    const char *description,
    MUXD::DispatcherQueuePriority priority =
        MUXD::DispatcherQueuePriority::Normal)
{
    if ( !calendar )
        return false;

    const auto reached = std::make_shared<bool>(false);
    if ( !calendar.DispatcherQueue().TryEnqueue(
             priority,
             [reached]()
             {
                 *reached = true;
             }) )
    {
        return false;
    }

    return WaitFor(description, [reached]()
    {
        return *reached;
    }, 1000);
}

bool wxWinUISameCalendarLayoutTicket(
    const wxCalendarCtrl::WinUILayoutTicketForTesting& lhs,
    const wxCalendarCtrl::WinUILayoutTicketForTesting& rhs)
{
    return lhs.layoutRevision == rhs.layoutRevision &&
           lhs.realizedDaysRevision == rhs.realizedDaysRevision &&
           lhs.weekRefreshRevision == rhs.weekRefreshRevision &&
           lhs.navigationGeneration == rhs.navigationGeneration &&
           lhs.decrementIdentity == rhs.decrementIdentity &&
           lhs.incrementIdentity == rhs.incrementIdentity;
}

wxWinUICalendarNavigationButtons wxWinUIGetBoundCalendarNavigationButtons(
    wxCalendarCtrl *calendar,
    const wxCalendarCtrl::WinUILayoutTicketForTesting& ticket)
{
    wxWinUICalendarNavigationButtons result;
    const MUX::UIElement content =
        wxWinUIGetHostedDateElement(calendar);
    std::vector<MUXCP::ButtonBase> buttons;
    wxWinUICollectDateElements(content, &buttons);
    for ( const MUXCP::ButtonBase& button : buttons )
    {
        const std::uintptr_t identity =
            reinterpret_cast<std::uintptr_t>(winrt::get_abi(button));
        if ( identity == ticket.decrementIdentity )
            result.decrement = button;
        else if ( identity == ticket.incrementIdentity )
            result.increment = button;
    }
    return result;
}

bool wxWinUIWaitForStableCalendarLayout(
    wxCalendarCtrl *calendar,
    const MUXC::CalendarView& calendarPeer,
    const char *description,
    wxCalendarCtrl::WinUILayoutTicketForTesting *stableTicket,
    wxWinUICalendarNavigationButtons *stableButtons)
{
    if ( !calendar || !calendarPeer || !stableTicket || !stableButtons )
        return false;

    wxCalendarCtrl::WinUILayoutTicketForTesting candidate;
    bool haveCandidate = false;
    unsigned stableEdges = 0;
    bool barrierQueued = false;
    std::shared_ptr<bool> barrierReached;
    return WaitFor(description, [&]()
    {
        if ( barrierQueued )
        {
            if ( !*barrierReached )
                return false;
            barrierQueued = false;
        }

        calendarPeer.UpdateLayout();
        wxCalendarCtrl::WinUILayoutTicketForTesting current;
        if ( !calendar->WinUIGetStableLayoutTicketForTesting(&current) )
        {
            haveCandidate = false;
            stableEdges = 0;
            return false;
        }

        const wxWinUICalendarNavigationButtons buttons =
            wxWinUIGetBoundCalendarNavigationButtons(calendar, current);
        if ( !buttons.decrement || !buttons.increment ||
             !wxWinUIIsFocusedWithin(buttons.decrement, calendarPeer) ||
             !wxWinUIIsFocusedWithin(buttons.increment, calendarPeer) ||
             !wxWinUIGetInvokeProvider(buttons.decrement) ||
             !wxWinUIGetInvokeProvider(buttons.increment) )
        {
            haveCandidate = false;
            stableEdges = 0;
            return false;
        }

        if ( haveCandidate &&
             wxWinUISameCalendarLayoutTicket(candidate, current) )
        {
            ++stableEdges;
        }
        else
        {
            stableEdges = 0;
        }
        candidate = current;
        haveCandidate = true;

        // Three identical production tickets separated by two low-priority
        // dispatcher edges prove both progressive day realization and the
        // navigation-handler reconciliation have settled. Unlike a sleep or
        // two blind FIFO barriers, any intervening real peer work changes a
        // revision or makes the ticket temporarily unavailable.
        if ( stableEdges == 2 )
        {
            *stableTicket = current;
            *stableButtons = buttons;
            return true;
        }

        barrierReached = std::make_shared<bool>(false);
        barrierQueued = calendarPeer.DispatcherQueue().TryEnqueue(
            MUXD::DispatcherQueuePriority::Low,
            [reached = barrierReached]()
            {
                *reached = true;
            });
        if ( !barrierQueued )
        {
            haveCandidate = false;
            stableEdges = 0;
        }
        return false;
    }, 1000);
}

#endif // wxUSE_CALENDARCTRL

MUXAPR::IInvokeProvider wxWinUIGetInvokeProvider(
    const MUXCP::ButtonBase& button)
{
    if ( !button )
        return nullptr;
    const MUXAP::AutomationPeer peer =
        MUXAP::FrameworkElementAutomationPeer::CreatePeerForElement(
            button.as<MUX::FrameworkElement>());
    return peer
        ? peer.GetPattern(MUXAP::PatternInterface::Invoke)
              .try_as<MUXAPR::IInvokeProvider>()
        : nullptr;
}

template <typename Factory>
void wxWinUICheckTransactionalLoadedSizing(
    Factory&& factory,
    const wxSize& requestedSize,
    const wxSize& requestedMinimum)
{
    wxFrame source(nullptr, wxID_ANY, "sizing source");
    wxPanel sourceParent(&source);
    source.SetClientSize(wxSize(720, 560));
    sourceParent.SetSize(source.GetClientSize());

    std::unique_ptr<wxWindow> control(factory(&sourceParent));
    REQUIRE(control);

    // Both calls occur before the hidden top-level is first shown. Loaded is
    // therefore forbidden from replaying the constructor size/minimum over
    // either application writer.
    control->SetMinSize(requestedMinimum);
    control->SetSize(requestedSize);
    source.Show();

    MUX::XamlRoot originalRoot{ nullptr };
    REQUIRE(WaitFor("transactional control first XamlRoot", [&]()
    {
        const MUX::FrameworkElement root =
            wxWinUIGetHostedDateElement(control.get())
                .try_as<MUX::FrameworkElement>();
        originalRoot = root ? root.XamlRoot() : nullptr;
        return originalRoot != nullptr;
    }, 1000));
    CHECK(control->GetMinSize() == requestedMinimum);
    CHECK(control->GetSize() == requestedSize);
    const wxSize firstBest = control->GetBestSize();
    CHECK(firstBest.x > 0);
    CHECK(firstBest.y > 0);

    wxFrame destination(nullptr, wxID_ANY, "sizing destination");
    wxPanel destinationParent(&destination);
    destination.SetClientSize(wxSize(760, 600));
    destinationParent.SetSize(destination.GetClientSize());
    destination.Show();
    wxYield();

    REQUIRE(control->Reparent(&destinationParent));
    MUX::XamlRoot destinationRoot{ nullptr };
    REQUIRE(WaitFor("transactional control reparent XamlRoot", [&]()
    {
        const MUX::FrameworkElement root =
            wxWinUIGetHostedDateElement(control.get())
                .try_as<MUX::FrameworkElement>();
        destinationRoot = root ? root.XamlRoot() : nullptr;
        return destinationRoot &&
               winrt::get_abi(destinationRoot) !=
                   winrt::get_abi(originalRoot);
    }, 1000));
    CHECK(control->GetMinSize() == requestedMinimum);
    CHECK(control->GetSize() == requestedSize);
    const wxSize reparentedBest = control->GetBestSize();
    CHECK(reparentedBest.x > 0);
    CHECK(reparentedBest.y > 0);

    // Destroy while its destination top-level is still alive.
    control.reset();
}

#if wxUSE_TIMEPICKCTRL

class wxWinUITestTimeLoadedHookScope
{
public:
    wxWinUITestTimeLoadedHookScope(
        wxTimePickerCtrl::WinUIHourLoadedHookForTesting hook,
        void *data)
    {
        wxTimePickerCtrl::WinUISetHourLoadedHookForTesting(hook, data);
    }

    ~wxWinUITestTimeLoadedHookScope()
    {
        wxTimePickerCtrl::WinUISetHourLoadedHookForTesting(nullptr);
    }
};

#endif // wxUSE_TIMEPICKCTRL

} // namespace

TEST_CASE("wxWinUI date controls keep application sizing across Loaded roots",
          "[winui-range-date][sizing][loaded][reparent]")
{
#if wxUSE_DATEPICKCTRL
    SECTION("DatePicker")
    {
        wxWinUICheckTransactionalLoadedSizing(
            [](wxWindow *parent) -> wxWindow *
            {
                wxDatePickerCtrl * const picker =
                    new wxDatePickerCtrl;
                if ( !picker->Create(
                         parent, wxID_ANY,
                         wxDateTime(15, wxDateTime::May, 2024)) )
                {
                    delete picker;
                    return nullptr;
                }
                return picker;
            },
            wxSize(287, 53), wxSize(101, 37));
    }
#endif

#if wxUSE_TIMEPICKCTRL
    SECTION("TimePicker")
    {
        wxWinUICheckTransactionalLoadedSizing(
            [](wxWindow *parent) -> wxWindow *
            {
                wxTimePickerCtrl * const picker =
                    new wxTimePickerCtrl;
                if ( !picker->Create(
                         parent, wxID_ANY,
                         wxDateTime(1, wxDateTime::Jan, 2012,
                                    11, 17, 23)) )
                {
                    delete picker;
                    return nullptr;
                }
                return picker;
            },
            wxSize(359, 55), wxSize(103, 39));
    }
#endif

#if wxUSE_CALENDARCTRL
    SECTION("Calendar")
    {
        wxWinUICheckTransactionalLoadedSizing(
            [](wxWindow *parent) -> wxWindow *
            {
                wxCalendarCtrl * const calendar =
                    new wxCalendarCtrl;
                if ( !calendar->Create(
                         parent, wxID_ANY,
                         wxDateTime(15, wxDateTime::May, 2024)) )
                {
                    delete calendar;
                    return nullptr;
                }
                return calendar;
            },
            wxSize(517, 437), wxSize(107, 43));
    }
#endif
}

#if wxUSE_GAUGE

TEST_CASE("wxWinUI Gauge preserves base and peer contracts",
          "[winui-range-date][gauge][default-create]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    wxGauge gauge;
    REQUIRE(gauge.Create(parent, wxID_ANY, 0));
    CHECK(gauge.GetRange() == 0);
    CHECK(gauge.GetValue() == 0);

    double maximum = -1;
    double value = -1;
    bool indeterminate = true;
    bool vertical = true;
    REQUIRE(gauge.WinUIGetPeerStateForTesting(
        &maximum, &value, &indeterminate, &vertical));
    CHECK(maximum == 1.0);
    CHECK(value == 0.0);
    CHECK_FALSE(indeterminate);
    CHECK_FALSE(vertical);

    gauge.Pulse();
    REQUIRE(gauge.WinUIGetPeerStateForTesting(
        &maximum, &value, &indeterminate, nullptr));
    CHECK(indeterminate);

    // Either determinate setter must end native indeterminate mode.
    gauge.SetRange(50);
    REQUIRE(gauge.WinUIGetPeerStateForTesting(
        &maximum, &value, &indeterminate, nullptr));
    CHECK(gauge.GetRange() == 50);
    CHECK(maximum == 50.0);
    CHECK_FALSE(indeterminate);

    gauge.Pulse();
    gauge.SetValue(25);
    REQUIRE(gauge.WinUIGetPeerStateForTesting(
        &maximum, &value, &indeterminate, nullptr));
    CHECK(gauge.GetValue() == 25);
    CHECK(value == 25.0);
    CHECK_FALSE(indeterminate);

    // wx keeps the caller's model value while XAML receives a valid clamped
    // representation, including when the public range is zero.
    gauge.SetRange(0);
    gauge.SetValue(7);
    REQUIRE(gauge.WinUIGetPeerStateForTesting(
        &maximum, &value, &indeterminate, nullptr));
    CHECK(gauge.GetRange() == 0);
    CHECK(gauge.GetValue() == 7);
    CHECK(maximum == 1.0);
    CHECK(value == 1.0);

    wxGauge verticalGauge(
        parent, wxID_ANY, 100,
        wxDefaultPosition, wxDefaultSize,
        wxGA_VERTICAL | wxGA_PROGRESS);
    REQUIRE(verticalGauge.WinUIGetPeerStateForTesting(
        nullptr, nullptr, nullptr, &vertical));
    CHECK(vertical);
    CHECK(verticalGauge.IsVertical());
    CHECK(verticalGauge.WinUIHasAppProgressForTesting());
}

TEST_CASE("wxWinUI Gauge revokes vertical layout callbacks",
          "[winui-range-date][gauge][lifetime]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    // Every vertical instance installs a Grid::SizeChanged delegate. Resize
    // before teardown so the real callback is exercised, then repeat enough
    // times to expose a retained delegate or a late callback into a dead wx
    // owner under the lifetime gate.
    for ( int i = 0; i < 100; ++i )
    {
        wxGauge * const gauge =
            new wxGauge(
                parent, wxID_ANY, 100,
                wxDefaultPosition,
                wxSize(18 + i % 3, 80 + i % 11),
                wxGA_VERTICAL);
        gauge->SetSize(wxSize(20 + i % 5, 90 + i % 13));

        bool vertical = false;
        REQUIRE(gauge->WinUIGetPeerStateForTesting(
            nullptr, nullptr, nullptr, &vertical));
        CHECK(vertical);
        delete gauge;
    }

    // Drain any coalesced XAML layout work after every owner has gone away.
    wxYield();
}

#endif // wxUSE_GAUGE

#if wxUSE_DATEPICKCTRL

TEST_CASE("wxWinUI DatePicker has a canonical transactional range",
          "[winui-range-date][datepicker][default-create]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    wxDatePickerCtrl picker;
    REQUIRE(picker.Create(parent, wxID_ANY));

    wxDateTime today = wxDateTime::Today();
    today.ResetTime();
    const wxDateTime minimum = today - wxDateSpan::Years(2);
    const wxDateTime maximum = today + wxDateSpan::Years(2);
    const wxDateTime tightenedMaximum = maximum - wxDateSpan::Day();

    wxDateTime defaultPeerMinimum;
    wxDateTime defaultPeerMaximum;
    REQUIRE(picker.WinUIGetDefaultPeerRangeForTesting(
        &defaultPeerMinimum, &defaultPeerMaximum));
    wxDateTime civilPeerMinimum;
    wxDateTime civilPeerMaximum;
    REQUIRE(picker.WinUIGetPeerStateForTesting(
        nullptr, &civilPeerMinimum, &civilPeerMaximum));
    CHECK(defaultPeerMinimum >= civilPeerMinimum);
    CHECK(defaultPeerMaximum <= civilPeerMaximum);
    CHECK(civilPeerMinimum <= today);
    CHECK(civilPeerMaximum >= today);

    EventCounter dateEvents(&picker, wxEVT_DATE_CHANGED);

    wxDateTime withTime(today);
    withTime.SetHour(23);
    withTime.SetMinute(37);
    picker.SetValue(withTime);
    CHECK(picker.GetValue() == today);
    CHECK(dateEvents.GetCount() == 0);

    picker.SetRange(minimum, maximum);
    wxDateTime actualMinimum;
    wxDateTime actualMaximum;
    REQUIRE(picker.GetRange(&actualMinimum, &actualMaximum));
    CHECK(actualMinimum == minimum);
    CHECK(actualMaximum == maximum);
    CHECK(dateEvents.GetCount() == 0);

    picker.SetValue(maximum);
    picker.SetRange(minimum, tightenedMaximum);
    CHECK(picker.GetValue() == tightenedMaximum);
    CHECK(dateEvents.GetCount() == 0);

    // Out-of-range setters are silent no-ops, matching wxMSW.
    picker.SetValue(maximum);
    CHECK(picker.GetValue() == tightenedMaximum);
    CHECK(dateEvents.GetCount() == 0);

    // An invalid interval is rejected transactionally: neither model nor peer
    // is partially changed.
    picker.SetRange(maximum, minimum);
    REQUIRE(picker.GetRange(&actualMinimum, &actualMaximum));
    CHECK(actualMinimum == minimum);
    CHECK(actualMaximum == tightenedMaximum);

    wxDateTime peerValue;
    wxDateTime peerMinimum;
    wxDateTime peerMaximum;
    REQUIRE(picker.WinUIGetPeerStateForTesting(
        &peerValue, &peerMinimum, &peerMaximum));
    CHECK(peerValue == tightenedMaximum);
    CHECK(peerMinimum == minimum);
    CHECK(peerMaximum == tightenedMaximum);

    // Removing a wx bound exposes the stable civil envelope supported by the
    // port, not CalendarDatePicker's rolling +/-100-year defaults.
    picker.SetRange(wxDefaultDateTime, tightenedMaximum);
    REQUIRE(picker.GetRange(&actualMinimum, &actualMaximum));
    CHECK_FALSE(actualMinimum.IsValid());
    CHECK(actualMaximum == tightenedMaximum);
    REQUIRE(picker.WinUIGetPeerStateForTesting(
        nullptr, &peerMinimum, &peerMaximum));
    CHECK(peerMinimum == civilPeerMinimum);
    CHECK(peerMaximum == tightenedMaximum);

    picker.SetRange(minimum, wxDefaultDateTime);
    REQUIRE(picker.GetRange(&actualMinimum, &actualMaximum));
    CHECK(actualMinimum == minimum);
    CHECK_FALSE(actualMaximum.IsValid());
    REQUIRE(picker.WinUIGetPeerStateForTesting(
        nullptr, &peerMinimum, &peerMaximum));
    CHECK(peerMinimum == minimum);
    CHECK(peerMaximum == civilPeerMaximum);

    picker.SetRange(wxDefaultDateTime, wxDefaultDateTime);
    CHECK_FALSE(picker.GetRange(&actualMinimum, &actualMaximum));
    REQUIRE(picker.WinUIGetPeerStateForTesting(
        nullptr, &peerMinimum, &peerMaximum));
    CHECK(peerMinimum == civilPeerMinimum);
    CHECK(peerMaximum == civilPeerMaximum);
    CHECK(dateEvents.GetCount() == 0);

    if ( defaultPeerMaximum < civilPeerMaximum )
    {
        const wxDateTime future =
            defaultPeerMaximum + wxDateSpan::Day();
        picker.SetRange(future, wxDefaultDateTime);
        CHECK(picker.GetValue() == future);
        REQUIRE(picker.WinUIGetPeerStateForTesting(
            &peerValue, &peerMinimum, &peerMaximum));
        CHECK(peerValue == future);
        CHECK(peerMinimum == future);
        CHECK(peerMaximum == civilPeerMaximum);
    }
}

TEST_CASE("wxWinUI DatePicker preserves pre-FILETIME Gregorian dates",
          "[winui-range-date][datepicker][gregorian][historical]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    wxWinUITestLanguageOverride language(L"en-US");
    const wxDateTime historical(15, wxDateTime::Jun, 1500);
    wxDatePickerCtrl picker(
        parent, wxID_ANY, historical,
        wxDefaultPosition, wxDefaultSize,
        wxDP_DROPDOWN | wxDP_SHOWCENTURY);
    CHECK(picker.GetValue() == historical);

    wxDateTime peerDate;
    REQUIRE(picker.WinUIGetPeerStateForTesting(
        &peerDate, nullptr, nullptr));
    CHECK(peerDate == historical);

    wxYield();
    const MUX::UIElement content =
        wxWinUIGetHostedDateElement(&picker);
    std::vector<MUXC::CalendarDatePicker> peers;
    wxWinUICollectDateElements(content, &peers);
    REQUIRE(peers.size() == 1);
    REQUIRE(peers.front().CalendarIdentifier() ==
            WG::CalendarIdentifiers::Gregorian());
    REQUIRE(peers.front().Date());
    CHECK(peers.front().Date().Value().time_since_epoch().count() < 0);
    CHECK(peers.front().MinDate().time_since_epoch().count() < 0);
}

TEST_CASE("wxWinUI DatePicker language reaches the real spin formatters",
          "[winui-range-date][datepicker][language][peer]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    const auto verify = [&](const winrt::hstring& expectedLanguage,
                            long style)
    {
        wxDatePickerCtrl picker(
            parent, wxID_ANY,
            wxDateTime(15, wxDateTime::Jun, 2025),
            wxDefaultPosition, wxDefaultSize,
            wxDP_SPIN | style);
        wxYield();
        const MUX::FrameworkElement root =
            wxWinUIGetHostedDateElement(&picker)
                .try_as<MUX::FrameworkElement>();
        REQUIRE(root);
        CHECK(root.Language() == expectedLanguage);

        const auto parts = wxWinUIGetCompositeParts(&picker);
        std::vector<MUXC::NumberBox> numbers;
        for ( const MUXC::Control& part : parts )
        {
            if ( const MUXC::NumberBox number =
                     part.try_as<MUXC::NumberBox>() )
            {
                numbers.push_back(number);
            }
        }
        REQUIRE(numbers.size() == 3);
        for ( const MUXC::NumberBox& number : numbers )
        {
            CHECK(number.Language() == expectedLanguage);
            REQUIRE(number.NumberFormatter());
            const bool isYear = number.Maximum() > 100;
            const unsigned digits = isYear
                ? ((style & wxDP_SHOWCENTURY) ? 4u : 2u)
                : 1u;
            const std::int64_t input = isYear ? 2025 : 12;
            const std::int64_t formattedInput =
                isYear && !(style & wxDP_SHOWCENTURY)
                    ? input % 100
                    : input;
            CHECK(number.NumberFormatter().FormatInt(input) ==
                  wxWinUITestFormatDecimal(
                      expectedLanguage, digits, formattedInput));
            if ( isYear && !(style & wxDP_SHOWCENTURY) )
            {
                const WGNF::INumberParser parser =
                    number.NumberFormatter()
                        .try_as<WGNF::INumberParser>();
                REQUIRE(parser);
                const double peerValue = number.Value();
                REQUIRE(std::isfinite(peerValue));
                const std::int64_t activeCalendarYear =
                    static_cast<std::int64_t>(
                        std::llround(peerValue));
                const WF::IReference<std::int64_t> parsed =
                    parser.ParseInt(
                        number.NumberFormatter().FormatInt(
                            activeCalendarYear));
                REQUIRE(parsed);
                CHECK(parsed.Value() == activeCalendarYear);
            }
        }
    };

    wxWinUITestLanguageOverride outer(L"en-US");
    {
        wxWinUITestLanguageOverride nested(L"ar-SA");
        verify(L"ar-SA", wxDP_SHOWCENTURY);
        verify(L"ar-SA", 0);
    }
    // The nested scope must restore all three control-specific language
    // overrides, not just the process formatter used by the test itself.
    verify(L"en-US", wxDP_SHOWCENTURY);
}

TEST_CASE("wxWinUI DatePicker maps century and dropdown styles",
          "[winui-range-date][datepicker][styles]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    wxDatePickerCtrl full(
        parent, wxID_ANY, wxDefaultDateTime,
        wxDefaultPosition, wxDefaultSize,
        wxDP_DROPDOWN | wxDP_SHOWCENTURY);
    const wxString fullFormat =
        full.WinUIGetPeerDateFormatForTesting();
    CHECK(fullFormat.Contains("day"));
    CHECK(fullFormat.Contains("month"));
    CHECK(fullFormat.Contains("year.full"));
    CHECK(full.WinUIUsesDropdownForTesting());

    wxDatePickerCtrl abbreviated(
        parent, wxID_ANY, wxDefaultDateTime,
        wxDefaultPosition, wxDefaultSize,
        wxDP_SPIN);
    const wxString abbreviatedFormat =
        abbreviated.WinUIGetPeerDateFormatForTesting();
    CHECK(abbreviatedFormat.Contains("day"));
    CHECK(abbreviatedFormat.Contains("month"));
    CHECK(abbreviatedFormat.Contains("year.abbreviated"));
    CHECK_FALSE(abbreviated.WinUIUsesDropdownForTesting());
    CHECK(abbreviatedFormat ==
          abbreviated.WinUIGetLocaleDatePatternForTesting());

    int year = 0;
    int month = 0;
    int day = 0;
    REQUIRE(abbreviated.WinUIGetSpinFieldsForTesting(
        &year, &month, &day));
    const wxDateTime spinValue = abbreviated.GetValue();
    CHECK(year == spinValue.GetYear());
    CHECK(month == static_cast<int>(spinValue.GetMonth()) + 1);
    CHECK(day == spinValue.GetDay());
    wxWinUICheckCompositeKeyboardAndAutomation(&abbreviated);
    CHECK(abbreviated.WinUIGetSpinYearTextForTesting().length() == 2);

    // wxDP_DEFAULT follows wxMSW and selects the inline spinner too.
    wxDatePickerCtrl platformDefault(parent, wxID_ANY);
    CHECK_FALSE(platformDefault.WinUIUsesDropdownForTesting());
    CHECK(platformDefault.HasFlag(wxDP_SPIN));
}

TEST_CASE("wxWinUI DatePicker user events and ALLOWNONE are exact",
          "[winui-range-date][datepicker][events]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    wxDateTime today = wxDateTime::Today();
    today.ResetTime();
    const wxDateTime tomorrow = today + wxDateSpan::Day();

    wxDatePickerCtrl picker(
        parent, wxID_ANY, wxDefaultDateTime,
        wxDefaultPosition, wxDefaultSize,
        wxDP_DEFAULT | wxDP_SHOWCENTURY | wxDP_ALLOWNONE);
    CHECK_FALSE(picker.GetValue().IsValid());
    picker.SetNullText("No civil date");
    CHECK(picker.WinUIGetPeerNullTextForTesting() ==
          "No civil date");

    wxDateTime peerValue;
    REQUIRE(picker.WinUIGetPeerStateForTesting(
        &peerValue, nullptr, nullptr));
    CHECK_FALSE(peerValue.IsValid());

    EventCounter events(&picker, wxEVT_DATE_CHANGED);
    picker.SetValue(today);
    CHECK(events.GetCount() == 0);

    REQUIRE(picker.WinUIClearPeerDateForTesting());
    CHECK_FALSE(picker.GetValue().IsValid());
    CHECK(events.GetCount() == 1);

    // Re-enter a nullable spin value one real NumberBox at a time. Partial
    // edits stay in the peer and do not fabricate a wx date or event.
    int partValue = -1;
    bool blank = false;
    REQUIRE(picker.WinUISetSpinPartForTesting(0, 2024));
    REQUIRE(picker.WinUIGetSpinPartForTesting(
        0, &partValue, &blank));
    CHECK_FALSE(blank);
    CHECK(partValue == 2024);
    REQUIRE(picker.WinUIGetSpinPartForTesting(
        1, nullptr, &blank));
    CHECK(blank);
    CHECK_FALSE(picker.GetValue().IsValid());
    CHECK(events.GetCount() == 1);

    REQUIRE(picker.WinUISetSpinPartForTesting(1, 2));
    REQUIRE(picker.WinUIGetSpinPartForTesting(
        1, &partValue, &blank));
    CHECK_FALSE(blank);
    CHECK(partValue == 2);
    CHECK_FALSE(picker.GetValue().IsValid());
    CHECK(events.GetCount() == 1);

    REQUIRE(picker.WinUISetSpinPartForTesting(2, 29));
    CHECK(picker.GetValue() ==
          wxDateTime(29, wxDateTime::Feb, 2024));
    CHECK(events.GetCount() == 2);

    REQUIRE(picker.WinUIClearPeerDateForTesting());
    CHECK_FALSE(picker.GetValue().IsValid());
    CHECK(events.GetCount() == 3);

    REQUIRE(picker.WinUISetPeerDateForTesting(tomorrow));
    CHECK(picker.GetValue() == tomorrow);
    CHECK(events.GetCount() == 4);

    picker.SetValue(wxDefaultDateTime);
    CHECK_FALSE(picker.GetValue().IsValid());
    CHECK(events.GetCount() == 4);

    // A required picker rejects a peer clear and restores its civil model
    // without fabricating a wx event.
    wxDatePickerCtrl required(parent, wxID_ANY, today);
    EventCounter requiredEvents(&required, wxEVT_DATE_CHANGED);
    requiredEvents.Clear();
    REQUIRE(required.WinUIClearPeerDateForTesting());
    CHECK(required.GetValue() == today);
    REQUIRE(required.WinUIGetPeerStateForTesting(
        &peerValue, nullptr, nullptr));
    CHECK(peerValue == today);
    CHECK(requiredEvents.GetCount() == 0);
}

TEST_CASE("wxWinUI DatePicker civil dates survive DST boundaries",
          "[winui-range-date][datepicker][dst]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    wxDatePickerCtrl picker(parent, wxID_ANY);
    EventCounter events(&picker, wxEVT_DATE_CHANGED);

    const std::array<wxDateTime, 2> transitionDates =
    {{
        wxDateTime(31, wxDateTime::Mar, 2024),
        wxDateTime(27, wxDateTime::Oct, 2024)
    }};

    for ( const wxDateTime& date : transitionDates )
    {
        picker.SetValue(date);
        CHECK(picker.GetValue() == date);

        wxDateTime peerValue;
        REQUIRE(picker.WinUIGetPeerStateForTesting(
            &peerValue, nullptr, nullptr));
        CHECK(peerValue == date);
    }
    CHECK(events.GetCount() == 0);
}

TEST_CASE("wxWinUI DatePicker callback teardown is destruction-safe",
          "[winui-range-date][datepicker][lifetime]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    wxDateTime today = wxDateTime::Today();
    today.ResetTime();

    wxDatePickerCtrl *picker =
        new wxDatePickerCtrl(parent, wxID_ANY, today);
    unsigned events = 0;
    picker->Bind(wxEVT_DATE_CHANGED, [&](wxDateEvent&)
    {
        ++events;
        wxDatePickerCtrl * const doomed = picker;
        picker = nullptr;
        delete doomed;
    });

    wxDatePickerCtrl * const invoking = picker;
    REQUIRE(invoking->WinUISetPeerDateForTesting(
        today + wxDateSpan::Day()));
    CHECK(picker == nullptr);
    CHECK(events == 1);

    for ( int i = 0; i < 100; ++i )
    {
        wxDatePickerCtrl * const transient =
            new wxDatePickerCtrl(parent, wxID_ANY, today);
        delete transient;
    }
}

TEST_CASE("wxWinUI DatePicker nested range setter is last writer",
          "[winui-range-date][datepicker][range][reentrance][force-render]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);
    const wxDateTime initial(15, wxDateTime::May, 2024);
    wxDatePickerCtrl picker(parent, wxID_ANY, initial);
    if ( wxWinUITopLevelHost * const host =
             wxWinUITopLevelHost::ForWindow(parent, false) )
    {
        host->FlushSync();
    }

    const wxDateTime outerMinimum(1, wxDateTime::May, 2024);
    const wxDateTime outerMaximum(31, wxDateTime::May, 2024);
    const wxDateTime nestedMinimum(10, wxDateTime::May, 2024);
    const wxDateTime nestedMaximum(20, wxDateTime::May, 2024);
    picker.SetSize(wxSize(229, 41));
    bool slotSynced = false;
    wxWinUITopLevelHost::TestOnNextSlotSynced(
        [&](wxWindow *window)
        {
            slotSynced = true;
            CHECK(window == &picker);
            picker.SetRange(nestedMinimum, nestedMaximum);
        });

    picker.SetRange(outerMinimum, outerMaximum);
    wxWinUITopLevelHost::TestOnNextSlotSynced({});
    CHECK(slotSynced);
    wxDateTime minimum;
    wxDateTime maximum;
    REQUIRE(picker.GetRange(&minimum, &maximum));
    CHECK(minimum == nestedMinimum);
    CHECK(maximum == nestedMaximum);
    wxDateTime peerValue;
    wxDateTime peerMinimum;
    wxDateTime peerMaximum;
    REQUIRE(picker.WinUIGetPeerStateForTesting(
        &peerValue, &peerMinimum, &peerMaximum));
    CHECK(peerValue == initial);
    CHECK(peerMinimum == nestedMinimum);
    CHECK(peerMaximum == nestedMaximum);
}

TEST_CASE("wxWinUI DatePicker nested value setter is last writer",
          "[winui-range-date][datepicker][value][reentrance][force-render]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);
    wxDatePickerCtrl picker(
        parent, wxID_ANY,
        wxDateTime(15, wxDateTime::May, 2024));
    if ( wxWinUITopLevelHost * const host =
             wxWinUITopLevelHost::ForWindow(parent, false) )
    {
        host->FlushSync();
    }

    const wxDateTime outer(16, wxDateTime::May, 2024);
    const wxDateTime nested(17, wxDateTime::May, 2024);
    picker.SetSize(wxSize(231, 43));
    bool slotSynced = false;
    wxWinUITopLevelHost::TestOnNextSlotSynced(
        [&](wxWindow *window)
        {
            slotSynced = true;
            CHECK(window == &picker);
            picker.SetValue(nested);
        });

    picker.SetValue(outer);
    wxWinUITopLevelHost::TestOnNextSlotSynced({});
    CHECK(slotSynced);
    CHECK(picker.GetValue() == nested);
    wxDateTime peer;
    REQUIRE(picker.WinUIGetPeerStateForTesting(
        &peer, nullptr, nullptr));
    CHECK(peer == nested);
}

#endif // wxUSE_DATEPICKCTRL

#if wxUSE_TIMEPICKCTRL

TEST_CASE("wxWinUI TimePicker preserves its second-precision contract",
          "[winui-range-date][timepicker][default-create]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    const wxDateTime initial(
        17, wxDateTime::Jul, 2026, 13, 47, 59);
    wxTimePickerCtrl picker;
    REQUIRE(picker.Create(parent, wxID_ANY, initial));

    int hour = -1;
    int minute = -1;
    int second = -1;
    REQUIRE(picker.GetTime(&hour, &minute, &second));
    CHECK(hour == 13);
    CHECK(minute == 47);
    CHECK(second == 59);

    wxDateTime peerValue;
    REQUIRE(picker.WinUIGetPeerTimeForTesting(&peerValue));
    CHECK(peerValue == picker.GetValue());

    const wxString localePattern =
        picker.WinUIGetLocaleTimePatternForTesting();
    CHECK_FALSE(localePattern.empty());
    int hourOrder = -1;
    int minuteOrder = -1;
    int secondOrder = -1;
    int periodOrder = -1;
    REQUIRE(picker.WinUIGetTimeFieldOrderForTesting(
        &hourOrder, &minuteOrder, &secondOrder, &periodOrder));
    CHECK(hourOrder >= 0);
    CHECK(minuteOrder >= 0);
    CHECK(secondOrder >= 0);
    // Preserve the complete WinRT pattern order. Some locales put the period
    // before the numeric fields, others put it after them.
    if ( periodOrder >= 0 )
    {
        const int secondPatternOrder =
            localePattern.Find("{second.");
        const int periodPatternOrder =
            localePattern.Find("{period.");
        REQUIRE(secondPatternOrder != wxNOT_FOUND);
        REQUIRE(periodPatternOrder != wxNOT_FOUND);
        CHECK((secondOrder < periodOrder) ==
              (secondPatternOrder < periodPatternOrder));

        const auto parts = wxWinUIGetCompositeParts(&picker);
        std::vector<MUXC::ComboBox> periodParts;
        for ( const MUXC::Control& part : parts )
        {
            if ( const MUXC::ComboBox period =
                     part.try_as<MUXC::ComboBox>() )
            {
                periodParts.push_back(period);
            }
        }
        REQUIRE(periodParts.size() == 1);
        const auto items = periodParts.front().Items();
        REQUIRE(items.Size() == 2);

        WG::Calendar periodCalendar;
        periodCalendar.ChangeClock(
            WG::ClockIdentifiers::TwelveHour());
        periodCalendar.SetToNow();
        const int firstPeriod =
            periodCalendar.FirstPeriodInThisDay();
        const int lastPeriod =
            periodCalendar.LastPeriodInThisDay();
        REQUIRE(firstPeriod != lastPeriod);
        periodCalendar.Period(firstPeriod);
        const winrt::hstring expectedFirst =
            periodCalendar.PeriodAsString();
        periodCalendar.Period(lastPeriod);
        const winrt::hstring expectedLast =
            periodCalendar.PeriodAsString();
        CHECK(winrt::unbox_value<winrt::hstring>(
                  items.GetAt(0)) == expectedFirst);
        CHECK(winrt::unbox_value<winrt::hstring>(
                  items.GetAt(1)) == expectedLast);
    }
    wxWinUICheckCompositeKeyboardAndAutomation(&picker);

    EventCounter events(&picker, wxEVT_TIME_CHANGED);
    picker.SetValue(
        wxDateTime(31, wxDateTime::Dec, 2035, 22, 5, 44));
    REQUIRE(picker.GetTime(&hour, &minute, &second));
    CHECK(hour == 22);
    CHECK(minute == 5);
    CHECK(second == 44);
    CHECK(picker.GetValue().GetDay() == 1);
    CHECK(picker.GetValue().GetMonth() == wxDateTime::Jan);
    CHECK(picker.GetValue().GetYear() == 2012);
    REQUIRE(picker.WinUIGetPeerTimeForTesting(&peerValue));
    CHECK(peerValue == picker.GetValue());
    CHECK(events.GetCount() == 0);
}

TEST_CASE("wxWinUI TimePicker language reaches every real field formatter",
          "[winui-range-date][timepicker][language][peer]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    const auto verify = [&](const winrt::hstring& expectedLanguage)
    {
        wxTimePickerCtrl picker(
            parent, wxID_ANY,
            wxDateTime(1, wxDateTime::Jan, 2012, 8, 7, 6));
        wxYield();
        const MUX::FrameworkElement root =
            wxWinUIGetHostedDateElement(&picker)
                .try_as<MUX::FrameworkElement>();
        REQUIRE(root);
        CHECK(root.Language() == expectedLanguage);

        const auto parts = wxWinUIGetCompositeParts(&picker);
        std::vector<MUXC::NumberBox> numbers;
        for ( const MUXC::Control& part : parts )
        {
            CHECK(part.Language() == expectedLanguage);
            if ( const MUXC::NumberBox number =
                     part.try_as<MUXC::NumberBox>() )
            {
                numbers.push_back(number);
            }
        }
        REQUIRE(numbers.size() == 3);
        for ( const MUXC::NumberBox& number : numbers )
        {
            REQUIRE(number.NumberFormatter());
            CHECK(number.NumberFormatter().FormatInt(7) ==
                  wxWinUITestFormatDecimal(
                      expectedLanguage, 2, 7));
        }
    };

    wxWinUITestLanguageOverride outer(L"en-US");
    {
        wxWinUITestLanguageOverride nested(L"ar-SA");
        verify(L"ar-SA");
    }
    verify(L"en-US");
}

TEST_CASE("wxWinUI composite physical keyboard navigation",
          "[winui-range-date][timepicker][.][physical]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    wxTimePickerCtrl picker(
        parent, wxID_ANY,
        wxDateTime(1, wxDateTime::Jan, 2012, 8, 10, 0));
    wxWinUITopLevelHost * const host =
        wxWinUITopLevelHost::FindSlotOwner(&picker);
    REQUIRE(host);
    const MUX::FrameworkElement root =
        wxWinUIGetHostedDateElement(&picker)
            .try_as<MUX::FrameworkElement>();
    REQUIRE(root);
    const auto parts = wxWinUIGetCompositeParts(&picker);
    REQUIRE(parts.size() >= 3);

    REQUIRE(parts[0].Focus(MUX::FocusState::Keyboard));
    wxYield();
    wxUIActionSimulator simulator;
    const int key = root.FlowDirection() ==
                            MUX::FlowDirection::RightToLeft
                        ? WXK_LEFT
                        : WXK_RIGHT;
    REQUIRE(simulator.Char(key));
    REQUIRE(WaitFor("physical composite keyboard navigation", [&]()
    {
        const MUX::DependencyObject focused =
            MUX::Input::FocusManager::GetFocusedElement(
                host->GetXamlRoot())
                .try_as<MUX::DependencyObject>();
        return wxWinUIIsFocusedWithin(focused, parts[1]);
    }, 1000));
}

TEST_CASE("wxWinUI TimePicker 12-hour spin boundaries are exact",
          "[winui-range-date][timepicker][12-hour][spin][events]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    wxWinUITestLanguageOverride language(L"en-US");
    wxTimePickerCtrl picker(
        parent, wxID_ANY,
        wxDateTime(1, wxDateTime::Jan, 2012, 11, 17, 23));
    const wxWinUITimeSpinParts spin =
        wxWinUIGetTimeSpinParts(&picker);
    REQUIRE(spin.hour);
    REQUIRE(spin.period);
    REQUIRE(spin.increment);
    REQUIRE(spin.decrement);
    const MUXAPR::IInvokeProvider increment =
        wxWinUIGetInvokeProvider(spin.increment);
    const MUXAPR::IInvokeProvider decrement =
        wxWinUIGetInvokeProvider(spin.decrement);
    REQUIRE(increment);
    REQUIRE(decrement);

    EventCounter events(&picker, wxEVT_TIME_CHANGED);
    const auto checkState = [&](int expectedHour,
                                int displayedHour,
                                int expectedPeriod,
                                int expectedEvents)
    {
        int hour = -1;
        int minute = -1;
        int second = -1;
        REQUIRE(picker.GetTime(&hour, &minute, &second));
        CHECK(hour == expectedHour);
        CHECK(minute == 17);
        CHECK(second == 23);
        CHECK(static_cast<int>(std::lround(spin.hour.Value())) ==
              displayedHour);
        CHECK(spin.period.SelectedIndex() == expectedPeriod);
        CHECK(events.GetCount() == expectedEvents);
    };
    const auto invokeAndCheck = [&](const MUXAPR::IInvokeProvider& provider,
                                    int expectedHour,
                                    int displayedHour,
                                    int expectedPeriod,
                                    int expectedEvents)
    {
        provider.Invoke();
        REQUIRE(WaitFor("12-hour NumberBox spin transaction", [&]()
        {
            int hour = -1;
            int minute = -1;
            int second = -1;
            return picker.GetTime(&hour, &minute, &second) &&
                   hour == expectedHour && minute == 17 && second == 23 &&
                   events.GetCount() == expectedEvents;
        }, 1000));
        checkState(expectedHour, displayedHour,
                   expectedPeriod, expectedEvents);
    };

    checkState(11, 11, 0, 0);

    // Public programmatic mutations cross both 11/12 boundaries under the
    // peer guard: they set, rather than toggle, the requested period and do
    // not emit user events.
    picker.SetValue(
        wxDateTime(1, wxDateTime::Jan, 2012, 12, 17, 23));
    checkState(12, 12, 1, 0);
    picker.SetValue(
        wxDateTime(1, wxDateTime::Jan, 2012, 11, 17, 23));
    checkState(11, 11, 0, 0);

    // A rejected partial programmatic peer edit rolls back under the same
    // guard and must not toggle AM/PM or publish an event.
    spin.hour.Value(std::numeric_limits<double>::quiet_NaN());
    REQUIRE(WaitFor("12-hour invalid peer rollback", [&]()
    {
        return std::isfinite(spin.hour.Value()) &&
               static_cast<int>(std::lround(spin.hour.Value())) == 11;
    }, 1000));
    checkState(11, 11, 0, 0);

    // Noon: 11 AM -> 12 PM toggles; 12 -> 1 wraps in PM; the reverse wrap
    // retains PM; 12 PM -> 11 AM toggles back.
    invokeAndCheck(increment, 12, 12, 1, 1);
    invokeAndCheck(increment, 13, 1, 1, 2);
    invokeAndCheck(decrement, 12, 12, 1, 3);
    invokeAndCheck(decrement, 11, 11, 0, 4);

    // Midnight exercises the opposite two period boundaries with the same
    // exact-once event and wrap rules.
    picker.SetValue(
        wxDateTime(1, wxDateTime::Jan, 2012, 23, 17, 23));
    checkState(23, 11, 1, 4);
    invokeAndCheck(increment, 0, 12, 0, 5);
    invokeAndCheck(increment, 1, 1, 0, 6);
    invokeAndCheck(decrement, 0, 12, 0, 7);
    invokeAndCheck(decrement, 23, 11, 1, 8);
}

TEST_CASE("wxWinUI TimePicker absolute 12-hour input preserves period",
          "[winui-range-date][timepicker][12-hour][typing][peer]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    wxWinUITestLanguageOverride language(L"en-US");
    wxTimePickerCtrl picker(
        parent, wxID_ANY,
        wxDateTime(1, wxDateTime::Jan, 2012, 23, 17, 23));
    const wxWinUITimeSpinParts spin = wxWinUIGetTimeSpinParts(&picker);
    REQUIRE(spin.hour);
    REQUIRE(spin.period);
    CHECK(spin.period.SelectedIndex() == 1);

    std::vector<MUXC::TextBox> inputs;
    wxWinUICollectDateElements(spin.hour, &inputs);
    REQUIRE_FALSE(inputs.empty());
    const MUXC::TextBox input = inputs.front();
    REQUIRE(input.Focus(MUX::FocusState::Keyboard));
    wxYield();

    EventCounter events(&picker, wxEVT_TIME_CHANGED);
    input.SelectAll();
    REQUIRE(wxWinUIDispatchTimeCharacter(::GetFocus(), L'1'));
    REQUIRE(wxWinUIDispatchTimeCharacter(::GetFocus(), L'2'));
    REQUIRE(wxWinUIDispatchTimeKey(::GetFocus(), VK_RETURN));
    REQUIRE(WaitFor("absolute 12-hour peer commit", [&]()
    {
        int hour = -1;
        int minute = -1;
        int second = -1;
        return picker.GetTime(&hour, &minute, &second) &&
               hour == 12 && minute == 17 && second == 23 &&
               spin.period.SelectedIndex() == 1 &&
               events.GetCount() == 1;
    }, 1000));
}

TEST_CASE("wxWinUI TimePicker rebinds real hour template spin peers",
          "[winui-range-date][timepicker][12-hour][template][uia]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);
    wxWinUITestLanguageOverride language(L"en-US");
    wxTimePickerCtrl picker(
        parent, wxID_ANY,
        wxDateTime(1, wxDateTime::Jan, 2012, 11, 17, 23));

    wxWinUITimeSpinParts spin = wxWinUIGetTimeSpinParts(&picker);
    REQUIRE(spin.hour);
    REQUIRE(spin.increment);
    REQUIRE(spin.decrement);
    const MUXC::ControlTemplate originalTemplate = spin.hour.Template();
    REQUIRE(originalTemplate);

    std::uintptr_t boundIncrement = 0;
    std::uintptr_t boundDecrement = 0;
    std::uint64_t bindingGeneration = 0;
    REQUIRE(picker.WinUIGetHourSpinBindingForTesting(
        &boundIncrement, &boundDecrement, &bindingGeneration));
    CHECK(boundIncrement == reinterpret_cast<std::uintptr_t>(
        winrt::get_abi(spin.increment)));
    CHECK(boundDecrement == reinterpret_cast<std::uintptr_t>(
        winrt::get_abi(spin.decrement)));
    const std::uintptr_t retiredIncrementIdentity = boundIncrement;
    const std::uintptr_t retiredDecrementIdentity = boundDecrement;
    const std::uint64_t originalGeneration = bindingGeneration;
    const MUXAPR::IInvokeProvider retiredIncrementProvider =
        wxWinUIGetInvokeProvider(spin.increment);
    REQUIRE(retiredIncrementProvider);

    // Remove the actual NumberBox template. LayoutUpdated must revoke the
    // production Click delegates before any replacement visual is adopted.
    spin.hour.Template(nullptr);
    spin.hour.ApplyTemplate();
    spin.hour.UpdateLayout();
    REQUIRE(WaitFor("hour template detach", [&]()
    {
        boundIncrement = 1;
        boundDecrement = 1;
        const bool bound = picker.WinUIGetHourSpinBindingForTesting(
            &boundIncrement, &boundDecrement, &bindingGeneration);
        return !bound && boundIncrement == 0 && boundDecrement == 0 &&
               bindingGeneration != originalGeneration;
    }, 1000));

    // Keeping a COM automation provider alive does not revoke NumberBox's
    // own template binding: invoking such a stale, unreachable provider can
    // still mutate NumberBox even though no user or UIA client can discover
    // it in the live tree. Prove retirement by the actual visual ancestry and
    // production binding generation, then invoke only the rebound live peer.
    REQUIRE(WaitFor("retired hour spin visual disconnected", [&]()
    {
        return !wxWinUIIsFocusedWithin(spin.increment, spin.hour) &&
               !wxWinUIIsFocusedWithin(spin.decrement, spin.hour);
    }, 1000));

    EventCounter events(&picker, wxEVT_TIME_CHANGED);
    CHECK(events.GetCount() == 0);

    // Apply a fresh valid NumberBox template. This is the supported destructive
    // retemplate path and materializes new real RepeatButton/UIA peers; trying
    // to reuse or implicitly reapply WinUI's effective theme template on this
    // already-detemplated NumberBox fails inside its TextBox template binding.
    const MUXC::ControlTemplate replacementTemplate =
        wxWinUICreateTimeHourReplacementTemplate();
    REQUIRE(replacementTemplate);
    spin.hour.Template(replacementTemplate);
    spin.hour.ApplyTemplate();
    spin.hour.UpdateLayout();
    REQUIRE(WaitFor("hour template rebind", [&]()
    {
        spin = wxWinUIGetTimeSpinParts(&picker);
        return spin.increment && spin.decrement &&
               picker.WinUIGetHourSpinBindingForTesting(
                   &boundIncrement, &boundDecrement,
                   &bindingGeneration);
    }, 1000));
    CHECK(bindingGeneration != originalGeneration);
    CHECK(boundIncrement == reinterpret_cast<std::uintptr_t>(
        winrt::get_abi(spin.increment)));
    CHECK(boundDecrement == reinterpret_cast<std::uintptr_t>(
        winrt::get_abi(spin.decrement)));
    CHECK(boundIncrement != retiredIncrementIdentity);
    CHECK(boundDecrement != retiredDecrementIdentity);
    CHECK(events.GetCount() == 0);

    const MUXAPR::IInvokeProvider reboundIncrement =
        wxWinUIGetInvokeProvider(spin.increment);
    const MUXAPR::IInvokeProvider reboundDecrement =
        wxWinUIGetInvokeProvider(spin.decrement);
    REQUIRE(reboundIncrement);
    REQUIRE(reboundDecrement);
    reboundIncrement.Invoke();
    REQUIRE(WaitFor("rebound hour UIA transaction", [&]()
    {
        int hour = -1;
        int minute = -1;
        int second = -1;
        return picker.GetTime(&hour, &minute, &second) &&
               hour == 12 && minute == 17 && second == 23 &&
               events.GetCount() == 1;
    }, 1000));
}

TEST_CASE("wxWinUI TimePicker real hour Loaded is destruction-safe",
          "[winui-range-date][timepicker][loaded][lifetime][reparent]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);
    wxWinUITestLanguageOverride language(L"en-US");

    wxFrame destination(nullptr, wxID_ANY, "hour Loaded target");
    wxPanel targetParent(&destination);
    destination.SetClientSize(wxSize(420, 180));
    targetParent.SetSize(destination.GetClientSize());
    destination.Show();
    wxYield();

    wxTimePickerCtrl *picker = new wxTimePickerCtrl(
        parent, wxID_ANY,
        wxDateTime(1, wxDateTime::Jan, 2012, 11, 17, 23));
    struct DestructionContext
    {
        wxTimePickerCtrl **slot = nullptr;
        bool loaded = false;
    } context{&picker, false};
    wxWinUITestTimeLoadedHookScope hook(
        [](wxTimePickerCtrl *owner, void *data)
        {
            DestructionContext * const context =
                static_cast<DestructionContext *>(data);
            context->loaded = true;
            CHECK((context->slot && *context->slot == owner));
            wxTimePickerCtrl * const doomed = *context->slot;
            *context->slot = nullptr;
            delete doomed;
        },
        &context);

    wxTimePickerCtrl * const invoking = picker;
    invoking->Reparent(&targetParent);
    REQUIRE(WaitFor("destructive real hour Loaded", [&]()
    {
        return context.loaded && picker == nullptr;
    }, 1000));
    delete picker;
}

TEST_CASE("wxWinUI TimePicker spin-boundary callbacks are lifetime-safe",
          "[winui-range-date][timepicker][12-hour][spin][lifetime][reparent]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);
    wxWinUITestLanguageOverride language(L"en-US");
    const wxDateTime initial(
        1, wxDateTime::Jan, 2012, 11, 17, 23);

    SECTION("destruction")
    {
        wxTimePickerCtrl *picker =
            new wxTimePickerCtrl(parent, wxID_ANY, initial);
        const wxWinUITimeSpinParts spin =
            wxWinUIGetTimeSpinParts(picker);
        REQUIRE(spin.increment);
        const MUXAPR::IInvokeProvider increment =
            wxWinUIGetInvokeProvider(spin.increment);
        REQUIRE(increment);

        unsigned eventCount = 0;
        picker->Bind(wxEVT_TIME_CHANGED, [&](wxDateEvent&)
        {
            ++eventCount;
            wxTimePickerCtrl * const doomed = picker;
            picker = nullptr;
            delete doomed;
        });
        increment.Invoke();
        REQUIRE(WaitFor("12-hour boundary destruction", [&]()
        {
            return picker == nullptr;
        }, 1000));
        CHECK(eventCount == 1);
        delete picker;
    }

    SECTION("cross top-level reparent")
    {
        wxFrame destination(
            nullptr, wxID_ANY, "time spin reparent target");
        wxPanel targetParent(&destination);
        destination.SetClientSize(wxSize(420, 160));
        targetParent.SetSize(destination.GetClientSize());
        destination.Show();
        wxYield();

        wxTimePickerCtrl picker(parent, wxID_ANY, initial);
        const wxWinUITimeSpinParts spin =
            wxWinUIGetTimeSpinParts(&picker);
        REQUIRE(spin.increment);
        const MUXAPR::IInvokeProvider increment =
            wxWinUIGetInvokeProvider(spin.increment);
        REQUIRE(increment);

        unsigned eventCount = 0;
        bool reparented = false;
        picker.Bind(wxEVT_TIME_CHANGED, [&](wxDateEvent&)
        {
            ++eventCount;
            reparented = picker.Reparent(&targetParent);
        });
        increment.Invoke();
        REQUIRE(WaitFor("12-hour boundary reparent", [&]()
        {
            return reparented && picker.GetParent() == &targetParent;
        }, 1000));
        int hour = -1;
        int minute = -1;
        int second = -1;
        REQUIRE(picker.GetTime(&hour, &minute, &second));
        CHECK(hour == 12);
        CHECK(minute == 17);
        CHECK(second == 23);
        CHECK(eventCount == 1);
    }
}

TEST_CASE("wxWinUI TimePicker peer events are exact and lifetime-safe",
          "[winui-range-date][timepicker][events][lifetime]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    const wxDateTime initial(
        1, wxDateTime::Jan, 2012, 8, 10, 0);
    wxTimePickerCtrl picker(parent, wxID_ANY, initial);
    EventCounter events(&picker, wxEVT_TIME_CHANGED);

    REQUIRE(picker.WinUISetPeerTimeForTesting(
        wxDateTime(9, wxDateTime::Sep, 2040, 9, 25, 58)));
    int hour = -1;
    int minute = -1;
    int second = -1;
    REQUIRE(picker.GetTime(&hour, &minute, &second));
    CHECK(hour == 9);
    CHECK(minute == 25);
    CHECK(second == 58);
    CHECK(events.GetCount() == 1);

    // The locale-ordered peer includes a real seconds spinner, so a change
    // within the same hidden native minute is observable as one wx event.
    REQUIRE(picker.WinUISetPeerTimeForTesting(
        wxDateTime(3, wxDateTime::Mar, 2030, 9, 25, 17)));
    CHECK(events.GetCount() == 2);
    REQUIRE(picker.GetTime(&hour, &minute, &second));
    CHECK(hour == 9);
    CHECK(minute == 25);
    CHECK(second == 17);

    // Only a complete hour/minute/second duplicate is coalesced.
    REQUIRE(picker.WinUISetPeerTimeForTesting(
        wxDateTime(12, wxDateTime::Oct, 2050, 9, 25, 17)));
    CHECK(events.GetCount() == 2);

    wxTimePickerCtrl *doomed =
        new wxTimePickerCtrl(parent, wxID_ANY, initial);
    unsigned destructionEvents = 0;
    doomed->Bind(wxEVT_TIME_CHANGED, [&](wxDateEvent&)
    {
        ++destructionEvents;
        wxTimePickerCtrl * const control = doomed;
        doomed = nullptr;
        delete control;
    });

    wxTimePickerCtrl * const invoking = doomed;
    REQUIRE(invoking->WinUISetPeerTimeForTesting(
        wxDateTime(1, wxDateTime::Jan, 2012, 10, 30, 0)));
    CHECK(doomed == nullptr);
    CHECK(destructionEvents == 1);

    for ( int i = 0; i < 100; ++i )
    {
        wxTimePickerCtrl * const transient =
            new wxTimePickerCtrl(parent, wxID_ANY, initial);
        delete transient;
    }
}

TEST_CASE("wxWinUI TimePicker Create survives synchronous Loaded destruction",
          "[winui-range-date][timepicker][create][lifetime]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    wxTimePickerCtrl *picker = new wxTimePickerCtrl;
    wxTimePickerCtrl * const invoking = picker;
    bool attached = false;
    wxWinUITopLevelHost::TestOnNextSlotAttached(
        [&](wxWindow *window)
        {
            attached = true;
            CHECK(window == invoking);
            wxTimePickerCtrl * const doomed = picker;
            picker = nullptr;
            delete doomed;
        });

    const bool created = invoking->Create(
        parent, wxID_ANY,
        wxDateTime(1, wxDateTime::Jan, 2012, 8, 15, 0));
    wxWinUITopLevelHost::TestOnNextSlotAttached({});
    CHECK(attached);
    CHECK_FALSE(created);
    CHECK(picker == nullptr);
    delete picker;
}

TEST_CASE("wxWinUI TimePicker setter stops after destructive ForceRender",
          "[winui-range-date][timepicker][lifetime][force-render]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    wxTimePickerCtrl *picker = new wxTimePickerCtrl(
        parent, wxID_ANY,
        wxDateTime(1, wxDateTime::Jan, 2012, 8, 15, 0));
    wxTimePickerCtrl * const invoking = picker;
    if ( wxWinUITopLevelHost * const host =
             wxWinUITopLevelHost::ForWindow(parent, false) )
    {
        host->FlushSync();
    }
    invoking->SetSize(wxSize(219, 41));
    bool slotSynced = false;
    wxWinUITopLevelHost::TestOnNextSlotSynced(
        [&](wxWindow *window)
        {
            slotSynced = true;
            CHECK(window == invoking);
            wxTimePickerCtrl * const doomed = picker;
            picker = nullptr;
            delete doomed;
        });

    invoking->SetValue(
        wxDateTime(1, wxDateTime::Jan, 2012, 9, 45, 0));
    wxWinUITopLevelHost::TestOnNextSlotSynced({});
    CHECK(slotSynced);
    CHECK(picker == nullptr);
    delete picker;
}

TEST_CASE("wxWinUI TimePicker nested setter is last writer",
          "[winui-range-date][timepicker][reentrance][force-render]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);
    wxTimePickerCtrl picker(
        parent, wxID_ANY,
        wxDateTime(1, wxDateTime::Jan, 2012, 8, 15, 0));
    if ( wxWinUITopLevelHost * const host =
             wxWinUITopLevelHost::ForWindow(parent, false) )
    {
        host->FlushSync();
    }

    const wxDateTime outer(
        1, wxDateTime::Jan, 2012, 9, 45, 0);
    const wxDateTime nested(
        1, wxDateTime::Jan, 2012, 10, 30, 0);
    picker.SetSize(wxSize(221, 43));
    bool slotSynced = false;
    wxWinUITopLevelHost::TestOnNextSlotSynced(
        [&](wxWindow *window)
        {
            slotSynced = true;
            CHECK(window == &picker);
            picker.SetValue(nested);
        });

    picker.SetValue(outer);
    wxWinUITopLevelHost::TestOnNextSlotSynced({});
    CHECK(slotSynced);
    CHECK(picker.GetValue() == nested);
    wxDateTime peer;
    REQUIRE(picker.WinUIGetPeerTimeForTesting(&peer));
    CHECK(peer == nested);
}

#endif // wxUSE_TIMEPICKCTRL

#if wxUSE_CALENDARCTRL

TEST_CASE("wxWinUI Calendar has a canonical transactional range",
          "[winui-range-date][calendar][range][dst]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    const wxDateTime initial(
        15, wxDateTime::May, 2024, 23, 41, 0);
    wxCalendarCtrl calendar(parent, wxID_ANY, initial);
    CHECK(calendar.GetDate() ==
          wxDateTime(15, wxDateTime::May, 2024));

    wxDateTime defaultMinimum;
    wxDateTime defaultMaximum;
    REQUIRE(calendar.WinUIGetDefaultPeerRangeForTesting(
        &defaultMinimum, &defaultMaximum));
    wxDateTime civilMinimum;
    wxDateTime civilMaximum;
    REQUIRE(calendar.WinUIGetPeerStateForTesting(
        nullptr, &civilMinimum, &civilMaximum));
    CHECK(defaultMinimum >= civilMinimum);
    CHECK(defaultMaximum <= civilMaximum);
    CHECK(civilMinimum <= calendar.GetDate());
    CHECK(civilMaximum >= calendar.GetDate());

    const wxDateTime minimum(1, wxDateTime::Jan, 2024);
    const wxDateTime maximum(31, wxDateTime::Dec, 2024);
    REQUIRE(calendar.SetDateRange(minimum, maximum));

    wxDateTime actualMinimum;
    wxDateTime actualMaximum;
    REQUIRE(calendar.GetDateRange(
        &actualMinimum, &actualMaximum));
    CHECK(actualMinimum == minimum);
    CHECK(actualMaximum == maximum);

    const wxDateTime previous = calendar.GetDate();
    CHECK_FALSE(calendar.SetDate(
        wxDateTime(1, wxDateTime::Jan, 2025)));
    CHECK(calendar.GetDate() == previous);

    // Reversed limits are rejected without partially changing model or peer.
    CHECK_FALSE(calendar.SetDateRange(maximum, minimum));
    REQUIRE(calendar.GetDateRange(
        &actualMinimum, &actualMaximum));
    CHECK(actualMinimum == minimum);
    CHECK(actualMaximum == maximum);

    wxDateTime peerDate;
    wxDateTime peerMinimum;
    wxDateTime peerMaximum;
    REQUIRE(calendar.WinUIGetPeerStateForTesting(
        &peerDate, &peerMinimum, &peerMaximum));
    CHECK(peerDate == previous);
    CHECK(peerMinimum == minimum);
    CHECK(peerMaximum == maximum);

    REQUIRE(calendar.SetDate(maximum));
    const wxDateTime tightenedMaximum =
        maximum - wxDateSpan::Day();
    REQUIRE(calendar.SetDateRange(minimum, tightenedMaximum));
    CHECK(calendar.GetDate() == tightenedMaximum);
    REQUIRE(calendar.WinUIGetPeerStateForTesting(
        &peerDate, &peerMinimum, &peerMaximum));
    CHECK(peerDate == tightenedMaximum);
    CHECK(peerMinimum == minimum);
    CHECK(peerMaximum == tightenedMaximum);

    // A civil interval outside CalendarView's rolling defaults is still a
    // supported peer range and clamps the current model transactionally.
    if ( defaultMaximum < civilMaximum )
    {
        const wxDateTime distantMinimum =
            defaultMaximum + wxDateSpan::Day();
        REQUIRE(calendar.SetDateRange(
            distantMinimum, wxDefaultDateTime));
        REQUIRE(calendar.GetDateRange(
            &actualMinimum, &actualMaximum));
        CHECK(actualMinimum == distantMinimum);
        CHECK_FALSE(actualMaximum.IsValid());
        REQUIRE(calendar.WinUIGetPeerStateForTesting(
            &peerDate, &peerMinimum, &peerMaximum));
        CHECK(peerDate == distantMinimum);
        CHECK(peerMinimum == distantMinimum);
        CHECK(peerMaximum == civilMaximum);
    }

    REQUIRE(calendar.SetDateRange(minimum, tightenedMaximum));

    REQUIRE(calendar.SetDateRange(
        wxDefaultDateTime, tightenedMaximum));
    REQUIRE(calendar.WinUIGetPeerStateForTesting(
        nullptr, &peerMinimum, &peerMaximum));
    CHECK(peerMinimum == civilMinimum);
    CHECK(peerMaximum == tightenedMaximum);

    REQUIRE(calendar.SetDateRange(
        minimum, wxDefaultDateTime));
    REQUIRE(calendar.WinUIGetPeerStateForTesting(
        nullptr, &peerMinimum, &peerMaximum));
    CHECK(peerMinimum == minimum);
    CHECK(peerMaximum == civilMaximum);

    // false is the documented successful return for removing both limits.
    CHECK_FALSE(calendar.SetDateRange(
        wxDefaultDateTime, wxDefaultDateTime));
    CHECK_FALSE(calendar.GetDateRange(
        &actualMinimum, &actualMaximum));
    REQUIRE(calendar.WinUIGetPeerStateForTesting(
        nullptr, &peerMinimum, &peerMaximum));
    CHECK(peerMinimum == civilMinimum);
    CHECK(peerMaximum == civilMaximum);

    const std::array<wxDateTime, 2> transitionDates =
    {{
        wxDateTime(31, wxDateTime::Mar, 2024),
        wxDateTime(27, wxDateTime::Oct, 2024)
    }};
    for ( const wxDateTime& transition : transitionDates )
    {
        REQUIRE(calendar.SetDate(transition));
        REQUIRE(calendar.WinUIGetPeerStateForTesting(
            &peerDate, nullptr, nullptr));
        CHECK(peerDate == transition);
    }
}

TEST_CASE("wxWinUI Calendar month lock is an effective native range",
          "[winui-range-date][calendar][month-lock]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    const wxDateTime initial(15, wxDateTime::May, 2024);
    wxCalendarCtrl calendar(
        parent, wxID_ANY, initial,
        wxDefaultPosition, wxDefaultSize,
        wxCAL_SHOW_HOLIDAYS | wxCAL_NO_MONTH_CHANGE);
    CHECK_FALSE(calendar.AllowMonthChange());

    wxDateTime peerMinimum;
    wxDateTime peerMaximum;
    REQUIRE(calendar.WinUIGetPeerStateForTesting(
        nullptr, &peerMinimum, &peerMaximum));
    CHECK(peerMinimum == wxDateTime(1, wxDateTime::May, 2024));
    CHECK(peerMaximum == wxDateTime(31, wxDateTime::May, 2024));

    REQUIRE(calendar.SetDate(
        wxDateTime(20, wxDateTime::May, 2024)));
    CHECK_FALSE(calendar.SetDate(
        wxDateTime(1, wxDateTime::Jun, 2024)));
    CHECK(calendar.GetDate() ==
          wxDateTime(20, wxDateTime::May, 2024));

    CHECK_FALSE(calendar.EnableMonthChange(false));
    REQUIRE(calendar.EnableMonthChange(true));
    CHECK(calendar.AllowMonthChange());

    REQUIRE(calendar.WinUIGetPeerStateForTesting(
        nullptr, &peerMinimum, &peerMaximum));
    CHECK(peerMinimum < calendar.GetDate());
    CHECK(peerMaximum > calendar.GetDate());

    REQUIRE(calendar.SetDateRange(
        wxDateTime(10, wxDateTime::May, 2024),
        wxDateTime(25, wxDateTime::Jun, 2024)));
    REQUIRE(calendar.EnableMonthChange(false));
    REQUIRE(calendar.WinUIGetPeerStateForTesting(
        nullptr, &peerMinimum, &peerMaximum));
    CHECK(peerMinimum == wxDateTime(10, wxDateTime::May, 2024));
    CHECK(peerMaximum == wxDateTime(31, wxDateTime::May, 2024));
}

TEST_CASE("wxWinUI Calendar preserves pre-FILETIME Gregorian dates",
          "[winui-range-date][calendar][gregorian][historical]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    wxWinUITestLanguageOverride language(L"en-US");
    const wxDateTime historical(15, wxDateTime::Jun, 1500);
    wxCalendarCtrl calendar(parent, wxID_ANY, historical);
    CHECK(calendar.GetDate() == historical);

    wxDateTime peerDate;
    REQUIRE(calendar.WinUIGetPeerStateForTesting(
        &peerDate, nullptr, nullptr));
    CHECK(peerDate == historical);

    wxYield();
    const MUX::UIElement content =
        wxWinUIGetHostedDateElement(&calendar);
    std::vector<MUXC::CalendarView> peers;
    wxWinUICollectDateElements(content, &peers);
    REQUIRE(peers.size() == 1);
    REQUIRE(peers.front().CalendarIdentifier() ==
            WG::CalendarIdentifiers::Gregorian());
    REQUIRE(peers.front().SelectedDates().Size() == 1);
    CHECK(peers.front().SelectedDates().GetAt(0)
              .time_since_epoch().count() < 0);
    CHECK(peers.front().MinDate().time_since_epoch().count() < 0);
}

TEST_CASE("wxWinUI alternate calendar drives ranges fields and holidays",
          "[winui-range-date][calendar][datepicker][non-gregorian][holiday]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    wxDateTime today = wxDateTime::Today();
    today.ResetTime();
    wxWinUITestLanguageOverride language(L"ar-SA");
    wxWinUITestHolidayAuthorityScope holidays(
        new wxWinUITestHolidayAuthority(today));

    wxCalendarCtrl calendar(
        parent, wxID_ANY, today,
        wxDefaultPosition, wxDefaultSize,
        wxCAL_SHOW_HOLIDAYS |
            wxCAL_SHOW_SURROUNDING_WEEKS);
    const MUX::UIElement content =
        wxWinUIGetHostedDateElement(&calendar);
    std::vector<MUXC::CalendarView> calendarViews;
    wxWinUICollectDateElements(content, &calendarViews);
    REQUIRE(calendarViews.size() == 1);
    const MUXC::CalendarView peer = calendarViews.front();
    REQUIRE(peer.CalendarIdentifier() !=
            WG::CalendarIdentifiers::Gregorian());
    REQUIRE(peer.SelectedDates().Size() == 1);

    WG::Calendar model;
    model.ChangeCalendarSystem(peer.CalendarIdentifier());
    model.SetDateTime(peer.SelectedDates().GetAt(0));
    const int expectedYear = model.Year();
    const int expectedMonth = model.Month();
    const int expectedDay = model.Day();

    wxDateTime peerMinimum;
    wxDateTime peerMaximum;
    REQUIRE(calendar.WinUIGetPeerStateForTesting(
        nullptr, &peerMinimum, &peerMaximum));
    CHECK(peerMinimum <= today);
    CHECK(peerMaximum >= today);
    REQUIRE(calendar.SetDateRange(peerMinimum, peerMaximum));

    unsigned holidayColourCount = 0;
    REQUIRE(WaitFor("alternate-calendar absolute holiday emphasis", [&]()
    {
        return calendar.WinUIGetAppliedDensityColourForTesting(
                   today.GetDay(), nullptr,
                   &holidayColourCount) &&
               holidayColourCount == 1;
    }, 1000));

    // Exercise the actual native envelope at both ends. These are peer dates,
    // not a duplicated Gregorian 1601/9999 oracle, and are particularly
    // important for finite calendars such as Um Al Qura.
    REQUIRE(calendar.SetDate(peerMinimum));
    CHECK(calendar.GetDate() == peerMinimum);
    REQUIRE(calendar.SetDate(peerMaximum));
    CHECK(calendar.GetDate() == peerMaximum);
    REQUIRE(calendar.SetDate(today));
    if ( peerMinimum > wxDateTime(1, wxDateTime::Jan, 1601) )
    {
        CHECK_FALSE(calendar.SetDateRange(
            peerMinimum - wxDateSpan::Day(), peerMaximum));
    }

#if wxUSE_DATEPICKCTRL
    wxDatePickerCtrl picker(
        parent, wxID_ANY, today,
        wxDefaultPosition, wxDefaultSize,
        wxDP_SPIN | wxDP_SHOWCENTURY);
    int year = 0;
    int month = 0;
    int day = 0;
    REQUIRE(picker.WinUIGetSpinFieldsForTesting(
        &year, &month, &day));
    CHECK(year == expectedYear);
    CHECK(month == expectedMonth);
    CHECK(day == expectedDay);
    picker.SetRange(peerMinimum, peerMaximum);
    wxDateTime pickerMinimum;
    wxDateTime pickerMaximum;
    REQUIRE(picker.GetRange(&pickerMinimum, &pickerMaximum));
    CHECK(pickerMinimum == peerMinimum);
    CHECK(pickerMaximum == peerMaximum);
    picker.SetValue(peerMinimum);
    CHECK(picker.GetValue() == peerMinimum);
    picker.SetValue(peerMaximum);
    CHECK(picker.GetValue() == peerMaximum);
    picker.SetValue(today);
    CHECK(picker.GetValue() == today);
    if ( peerMinimum > wxDateTime(1, wxDateTime::Jan, 1601) )
    {
        const wxDateTime beforeMinimum =
            peerMinimum - wxDateSpan::Day();
        picker.SetRange(beforeMinimum, peerMaximum);
        REQUIRE(picker.GetRange(&pickerMinimum, &pickerMaximum));
        CHECK(pickerMinimum == peerMinimum);
        CHECK(pickerMaximum == peerMaximum);
    }
    wxWinUICheckCompositeKeyboardAndAutomation(&picker);
#endif // wxUSE_DATEPICKCTRL
}

TEST_CASE("wxWinUI alternate-calendar navigation keeps wxMSW civil events",
          "[winui-range-date][calendar][non-gregorian][events]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    wxDateTime today = wxDateTime::Today();
    today.ResetTime();
    wxWinUITestLanguageOverride language(L"ar-SA");
    wxCalendarCtrl calendar(
        parent, wxID_ANY, today,
        wxDefaultPosition, wxDefaultSize,
        wxCAL_SHOW_SURROUNDING_WEEKS);

    const MUX::UIElement content =
        wxWinUIGetHostedDateElement(&calendar);
    std::vector<MUXC::CalendarView> peers;
    wxWinUICollectDateElements(content, &peers);
    REQUIRE(peers.size() == 1);
    const winrt::hstring identifier =
        peers.front().CalendarIdentifier();
    REQUIRE(identifier != WG::CalendarIdentifiers::Gregorian());

    wxDateTime peerMinimum;
    wxDateTime peerMaximum;
    REQUIRE(calendar.WinUIGetPeerStateForTesting(
        nullptr, &peerMinimum, &peerMaximum));

    wxDateTime scanStart(
        1, wxDateTime::Jan, today.GetYear() - 3);
    wxDateTime scanEnd(
        31, wxDateTime::Dec, today.GetYear() + 3);
    if ( scanStart < peerMinimum )
        scanStart = peerMinimum;
    if ( scanEnd > peerMaximum )
        scanEnd = peerMaximum;
    REQUIRE(scanStart < scanEnd);

    std::array<wxDateTime, 2> activeOnly;
    std::array<wxDateTime, 2> civilMonthOnly;
    std::array<wxDateTime, 2> civilYearOnly;
    for ( wxDateTime first = scanStart;
          first < scanEnd &&
              (!activeOnly[0].IsValid() ||
               !civilMonthOnly[0].IsValid() ||
               !civilYearOnly[0].IsValid());
          first += wxDateSpan::Day() )
    {
        const wxDateTime second = first + wxDateSpan::Day();
        wxWinUITestCalendarMonthKey firstActive;
        wxWinUITestCalendarMonthKey secondActive;
        REQUIRE(wxWinUITestReadCalendarMonth(
            identifier, first, &firstActive));
        REQUIRE(wxWinUITestReadCalendarMonth(
            identifier, second, &secondActive));

        const bool activeChanged = !(firstActive == secondActive);
        const bool civilYearChanged =
            first.GetYear() != second.GetYear();
        const bool civilMonthChanged = civilYearChanged ||
            first.GetMonth() != second.GetMonth();
        if ( activeChanged && !civilMonthChanged &&
             !activeOnly[0].IsValid() )
        {
            activeOnly = {{first, second}};
        }
        if ( !activeChanged && !civilYearChanged &&
             civilMonthChanged &&
             !civilMonthOnly[0].IsValid() )
        {
            civilMonthOnly = {{first, second}};
        }
        if ( !activeChanged && civilYearChanged &&
             !civilYearOnly[0].IsValid() )
        {
            civilYearOnly = {{first, second}};
        }
    }

    REQUIRE(activeOnly[0].IsValid());
    REQUIRE(civilMonthOnly[0].IsValid());
    REQUIRE(civilYearOnly[0].IsValid());

    std::vector<wxEventType> events;
    const auto record = [&events](wxCalendarEvent& event)
    {
        events.push_back(event.GetEventType());
    };
    calendar.Bind(wxEVT_CALENDAR_SEL_CHANGED, record);
    calendar.Bind(wxEVT_CALENDAR_PAGE_CHANGED, record);
    calendar.Bind(wxEVT_CALENDAR_DAY_CHANGED, record);
    calendar.Bind(wxEVT_CALENDAR_MONTH_CHANGED, record);
    calendar.Bind(wxEVT_CALENDAR_YEAR_CHANGED, record);

    const auto exercise =
        [&](const std::array<wxDateTime, 2>& dates,
            std::initializer_list<wxEventType> expected)
        {
            REQUIRE(calendar.SetDate(dates[0]));
            events.clear();
            REQUIRE(calendar.WinUISetPeerDateForTesting(dates[1]));
            CHECK(calendar.GetDate() == dates[1]);
            REQUIRE(events.size() == expected.size());
            std::size_t n = 0;
            for ( const wxEventType type : expected )
                CHECK(events[n++] == type);
        };

    // The peer crossed an ar-SA calendar month, but wxMSW's public event
    // classifier sees two dates in the same civil Gregorian month.
    exercise(activeOnly,
             {wxEVT_CALENDAR_SEL_CHANGED,
              wxEVT_CALENDAR_DAY_CHANGED});

    // Conversely, Gregorian month/year boundaries are PAGE + legacy
    // MONTH/YEAR even when the active ar-SA calendar stays in one month.
    exercise(civilMonthOnly,
             {wxEVT_CALENDAR_SEL_CHANGED,
              wxEVT_CALENDAR_PAGE_CHANGED,
              wxEVT_CALENDAR_MONTH_CHANGED});
    exercise(civilYearOnly,
             {wxEVT_CALENDAR_SEL_CHANGED,
              wxEVT_CALENDAR_PAGE_CHANGED,
              wxEVT_CALENDAR_YEAR_CHANGED});
}

#if wxUSE_DATEPICKCTRL

TEST_CASE("wxWinUI Japanese calendar preserves era transitions and bounds",
          "[winui-range-date][calendar][datepicker][japanese][boundary]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    wxWinUITestLanguageOverride language(L"ja-JP-u-ca-japanese");
    const wxDateTime heiseiLast(30, wxDateTime::Apr, 2019);
    const wxDateTime reiwaFirst(1, wxDateTime::May, 2019);
    wxCalendarCtrl calendar(
        parent, wxID_ANY, heiseiLast,
        wxDefaultPosition, wxDefaultSize,
        wxCAL_SHOW_SURROUNDING_WEEKS);
    const MUX::UIElement calendarContent =
        wxWinUIGetHostedDateElement(&calendar);
    std::vector<MUXC::CalendarView> calendarViews;
    wxWinUICollectDateElements(calendarContent, &calendarViews);
    REQUIRE(calendarViews.size() == 1);
    const MUXC::CalendarView calendarPeer = calendarViews.front();
    CHECK(calendarPeer.Language() == L"ja-JP-u-ca-japanese");
    REQUIRE(calendarPeer.CalendarIdentifier() ==
            WG::CalendarIdentifiers::Japanese());

    wxDateTime peerMinimum;
    wxDateTime peerMaximum;
    REQUIRE(calendar.WinUIGetPeerStateForTesting(
        nullptr, &peerMinimum, &peerMaximum));
    // Windows.Globalization documents Meiji 1 as beginning on this civil
    // date. This independent public contract catches a regression back to a
    // generic Gregorian envelope.
    CHECK(peerMinimum == wxDateTime(1, wxDateTime::Jan, 1868));
    CHECK(peerMaximum > reiwaFirst);
    REQUIRE(calendar.SetDateRange(peerMinimum, peerMaximum));

    wxDatePickerCtrl picker(
        parent, wxID_ANY, heiseiLast,
        wxDefaultPosition, wxDefaultSize,
        wxDP_SPIN | wxDP_SHOWCENTURY);
    wxYield();
    const MUX::FrameworkElement pickerRoot =
        wxWinUIGetHostedDateElement(&picker)
            .try_as<MUX::FrameworkElement>();
    REQUIRE(pickerRoot);
    CHECK(pickerRoot.Language() == L"ja-JP-u-ca-japanese");
    wxDateTime pickerMinimum;
    wxDateTime pickerMaximum;
    REQUIRE(picker.WinUIGetPeerStateForTesting(
        nullptr, &pickerMinimum, &pickerMaximum));
    CHECK(pickerMinimum == peerMinimum);
    CHECK(pickerMaximum == peerMaximum);
    picker.SetRange(peerMinimum, peerMaximum);

    const auto pickerParts = wxWinUIGetCompositeParts(&picker);
    std::vector<MUXC::ComboBox> eraParts;
    for ( const MUXC::Control& part : pickerParts )
    {
        if ( const MUXC::ComboBox era = part.try_as<MUXC::ComboBox>() )
            eraParts.push_back(era);
    }
    REQUIRE(eraParts.size() == 1);
    CHECK(eraParts.front().Language() ==
          L"ja-JP-u-ca-japanese");

    const auto checkCivilDate = [&](const wxDateTime& civil)
    {
        REQUIRE(calendar.SetDate(civil));
        REQUIRE(calendarPeer.SelectedDates().Size() == 1);
        WG::Calendar model(
            std::vector<winrt::hstring>{
                L"ja-JP-u-ca-japanese"});
        model.ChangeCalendarSystem(
            WG::CalendarIdentifiers::Japanese());
        model.SetDateTime(calendarPeer.SelectedDates().GetAt(0));

        picker.SetValue(civil);
        REQUIRE(picker.GetValue() == civil);
        int year = 0;
        int month = 0;
        int day = 0;
        REQUIRE(picker.WinUIGetSpinFieldsForTesting(
            &year, &month, &day));
        CHECK(year == model.Year());
        CHECK(month == model.Month());
        CHECK(day == model.Day());
        CHECK(eraParts.front().SelectedIndex() ==
              model.Era() - model.FirstEra());
        const int eraIndex = eraParts.front().SelectedIndex();
        REQUIRE(eraIndex >= 0);
        REQUIRE(static_cast<std::uint32_t>(eraIndex) <
                eraParts.front().Items().Size());
        CHECK(winrt::unbox_value<winrt::hstring>(
                  eraParts.front().Items().GetAt(eraIndex)) ==
              model.EraAsString());
        return eraParts.front().SelectedIndex();
    };

    const int heiseiEra = checkCivilDate(heiseiLast);
    const int reiwaEra = checkCivilDate(reiwaFirst);
    CHECK(reiwaEra != heiseiEra);
    checkCivilDate(peerMinimum);
    checkCivilDate(peerMaximum);

    const wxDateTime beforeMinimum =
        peerMinimum - wxDateSpan::Day();
    CHECK_FALSE(calendar.SetDateRange(beforeMinimum, peerMaximum));
    picker.SetRange(beforeMinimum, peerMaximum);
    REQUIRE(picker.GetRange(&pickerMinimum, &pickerMaximum));
    CHECK(pickerMinimum == peerMinimum);
    CHECK(pickerMaximum == peerMaximum);
}

#endif // wxUSE_DATEPICKCTRL

TEST_CASE("wxWinUI holiday authority may destroy Calendar safely",
          "[winui-range-date][calendar][holiday][lifetime]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    wxDateTime today = wxDateTime::Today();
    today.ResetTime();
    wxCalendarCtrl *calendar = nullptr;
    wxWinUITestHolidayAuthorityScope holidays(
        new wxWinUITestHolidayAuthority(today, [&]()
        {
            wxCalendarCtrl * const doomed = calendar;
            calendar = nullptr;
            delete doomed;
        }));

    calendar = new wxCalendarCtrl(
        parent, wxID_ANY, today,
        wxDefaultPosition, wxDefaultSize, 0);
    wxCalendarCtrl * const invoking = calendar;
    invoking->EnableHolidayDisplay(true);
    CHECK(calendar == nullptr);
    delete calendar;
}

TEST_CASE("wxWinUI holiday authority reentrance converges on live state",
          "[winui-range-date][calendar][holiday][reentrance][reparent]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    const wxDateTime initial(15, wxDateTime::May, 2024);
    const wxDateTime target(15, wxDateTime::Jun, 2024);

    SECTION("SetDate")
    {
        wxCalendarCtrl calendar(
            parent, wxID_ANY, initial,
            wxDefaultPosition, wxDefaultSize, 0);
        bool changedDate = false;
        wxWinUITestHolidayAuthority *authority = nullptr;
        authority = new wxWinUITestHolidayAuthority(initial, [&]()
        {
            authority->SetHoliday(target);
            changedDate = calendar.SetDate(target);
        });
        wxWinUITestHolidayAuthorityScope holidays(authority);

        calendar.EnableHolidayDisplay(true);
        REQUIRE(changedDate);
        CHECK(calendar.GetDate() == target);
        REQUIRE(WaitFor("holiday refresh after reentrant SetDate", [&]()
        {
            unsigned colourCount = 0;
            return calendar.WinUIGetAppliedDensityColourForTesting(
                       target.GetDay(), nullptr, &colourCount) &&
                   colourCount == 1;
        }, 1000));
    }

    SECTION("SetDateRange")
    {
        const wxDateTime rangeMinimum(10, wxDateTime::May, 2024);
        const wxDateTime rangeMaximum(25, wxDateTime::May, 2024);
        const wxDateTime rangeHoliday(20, wxDateTime::May, 2024);
        wxCalendarCtrl calendar(
            parent, wxID_ANY, initial,
            wxDefaultPosition, wxDefaultSize, 0);
        bool changedRange = false;
        wxWinUITestHolidayAuthority *authority = nullptr;
        authority = new wxWinUITestHolidayAuthority(initial, [&]()
        {
            authority->SetHoliday(rangeHoliday);
            changedRange = calendar.SetDateRange(
                rangeMinimum, rangeMaximum);
        });
        wxWinUITestHolidayAuthorityScope holidays(authority);

        calendar.EnableHolidayDisplay(true);
        REQUIRE(changedRange);
        CHECK(calendar.GetDate() == initial);
        wxDateTime minimum;
        wxDateTime maximum;
        REQUIRE(calendar.GetDateRange(&minimum, &maximum));
        CHECK(minimum == rangeMinimum);
        CHECK(maximum == rangeMaximum);
        CHECK(authority->GetRangeRequestCount() >= 2);
        REQUIRE(WaitFor("holiday refresh after reentrant SetDateRange", [&]()
        {
            unsigned colourCount = 0;
            return calendar.WinUIGetAppliedDensityColourForTesting(
                       rangeHoliday.GetDay(), nullptr, &colourCount) &&
                   colourCount == 1;
        }, 1000));
    }

    SECTION("EnableHolidayDisplay")
    {
        wxCalendarCtrl calendar(
            parent, wxID_ANY, initial,
            wxDefaultPosition, wxDefaultSize, 0);
        bool toggled = false;
        wxWinUITestHolidayAuthorityScope holidays(
            new wxWinUITestHolidayAuthority(initial, [&]()
            {
                calendar.EnableHolidayDisplay(false);
                calendar.EnableHolidayDisplay(true);
                toggled = true;
            }));

        calendar.EnableHolidayDisplay(true);
        REQUIRE(toggled);
        CHECK(calendar.HasFlag(wxCAL_SHOW_HOLIDAYS));
        REQUIRE(WaitFor("holiday refresh after reentrant style toggle", [&]()
        {
            unsigned colourCount = 0;
            return calendar.WinUIGetAppliedDensityColourForTesting(
                       initial.GetDay(), nullptr, &colourCount) &&
                   colourCount == 1;
        }, 1000));
    }

    SECTION("cross top-level reparent")
    {
        wxFrame destination(nullptr, wxID_ANY, "holiday reparent target");
        wxPanel targetParent(&destination);
        destination.SetClientSize(wxSize(480, 420));
        targetParent.SetSize(destination.GetClientSize());
        destination.Show();
        wxYield();

        wxCalendarCtrl calendar(
            parent, wxID_ANY, initial,
            wxDefaultPosition, wxDefaultSize, 0);
        wxWinUITopLevelHost * const originalHost =
            wxWinUITopLevelHost::FindSlotOwner(&calendar);
        REQUIRE(originalHost);
        const MUX::UIElement originalContent =
            wxWinUIGetHostedDateElement(&calendar);
        std::vector<MUXC::CalendarView> originalPeers;
        wxWinUICollectDateElements(originalContent, &originalPeers);
        REQUIRE(originalPeers.size() == 1);
        const MUX::XamlRoot originalRoot =
            originalPeers.front().XamlRoot();
        REQUIRE(originalRoot);
        bool reparented = false;
        wxWinUITestHolidayAuthority *authority = nullptr;
        authority = new wxWinUITestHolidayAuthority(initial, [&]()
        {
            reparented = calendar.Reparent(&targetParent);
        });
        wxWinUITestHolidayAuthorityScope holidays(authority);

        calendar.EnableHolidayDisplay(true);
        REQUIRE(reparented);
        CHECK(calendar.GetParent() == &targetParent);
        wxYield();
        wxWinUITopLevelHost * const reparentedHost =
            wxWinUITopLevelHost::FindSlotOwner(&calendar);
        REQUIRE(reparentedHost);
        CHECK(reparentedHost != originalHost);
        const MUX::UIElement reparentedContent =
            wxWinUIGetHostedDateElement(&calendar);
        std::vector<MUXC::CalendarView> reparentedPeers;
        wxWinUICollectDateElements(reparentedContent, &reparentedPeers);
        REQUIRE(reparentedPeers.size() == 1);
        const MUX::XamlRoot reparentedRoot =
            reparentedPeers.front().XamlRoot();
        REQUIRE(reparentedRoot);
        CHECK(winrt::get_abi(reparentedRoot) !=
              winrt::get_abi(originalRoot));
        CHECK(authority->GetRangeRequestCount() >= 2);
        REQUIRE(WaitFor("holiday refresh after authority reparent", [&]()
        {
            unsigned colourCount = 0;
            return calendar.WinUIGetAppliedDensityColourForTesting(
                       initial.GetDay(), nullptr, &colourCount) &&
                   colourCount == 1;
        }, 1000));
    }
}

TEST_CASE("wxWinUI Calendar events, marks and day hit testing are exact",
          "[winui-range-date][calendar][events][marks][hit-test]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    wxCalendarCtrl calendar(
        parent, wxID_ANY,
        wxDateTime(15, wxDateTime::May, 2024),
        wxDefaultPosition, wxDefaultSize,
        wxCAL_MONDAY_FIRST |
            wxCAL_SHOW_HOLIDAYS |
            wxCAL_SHOW_SURROUNDING_WEEKS |
            wxCAL_SHOW_WEEK_NUMBERS);

    std::vector<wxEventType> events;
    const auto record = [&](wxCalendarEvent& event)
    {
        events.push_back(event.GetEventType());
    };
    calendar.Bind(wxEVT_CALENDAR_SEL_CHANGED, record);
    calendar.Bind(wxEVT_CALENDAR_PAGE_CHANGED, record);
    calendar.Bind(wxEVT_CALENDAR_DAY_CHANGED, record);
    calendar.Bind(wxEVT_CALENDAR_MONTH_CHANGED, record);
    calendar.Bind(wxEVT_CALENDAR_YEAR_CHANGED, record);

    REQUIRE(calendar.WinUISetPeerDateForTesting(
        wxDateTime(16, wxDateTime::May, 2024)));
    REQUIRE(events.size() == 2);
    CHECK(events[0] == wxEVT_CALENDAR_SEL_CHANGED);
    CHECK(events[1] == wxEVT_CALENDAR_DAY_CHANGED);

    events.clear();
    REQUIRE(calendar.WinUISetPeerDateForTesting(
        wxDateTime(3, wxDateTime::Jun, 2024)));
    REQUIRE(events.size() == 3);
    CHECK(events[0] == wxEVT_CALENDAR_SEL_CHANGED);
    CHECK(events[1] == wxEVT_CALENDAR_PAGE_CHANGED);
    CHECK(events[2] == wxEVT_CALENDAR_MONTH_CHANGED);

    events.clear();
    REQUIRE(calendar.WinUISetPeerDateForTesting(
        wxDateTime(4, wxDateTime::Jun, 2025)));
    REQUIRE(events.size() == 3);
    CHECK(events[0] == wxEVT_CALENDAR_SEL_CHANGED);
    CHECK(events[1] == wxEVT_CALENDAR_PAGE_CHANGED);
    CHECK(events[2] == wxEVT_CALENDAR_YEAR_CHANGED);

    calendar.Mark(4, true);
    CHECK(calendar.WinUIIsMarkedForTesting(4));
    calendar.Mark(4, false);
    CHECK_FALSE(calendar.WinUIIsMarkedForTesting(4));

    calendar.SetHoliday(17);
    CHECK(calendar.WinUIIsHolidayForTesting(17));
    unsigned holidayColourCount = 0;
    REQUIRE(WaitFor("holiday CalendarView day realization", [&]()
    {
        return calendar.WinUIGetAppliedDensityColourForTesting(
                   17, nullptr, &holidayColourCount) &&
               holidayColourCount == 1;
    }, 1000));
    calendar.EnableHolidayDisplay(false);
    CHECK_FALSE(calendar.WinUIIsHolidayForTesting(17));
    REQUIRE(WaitFor("holiday CalendarView emphasis removal", [&]()
    {
        return calendar.WinUIGetAppliedDensityColourForTesting(
                   17, nullptr, &holidayColourCount) &&
               holidayColourCount == 0;
    }, 1000));

    unsigned doubleClicks = 0;
    wxDateTime doubleClickedDate;
    calendar.Bind(
        wxEVT_CALENDAR_DOUBLECLICKED,
        [&](wxCalendarEvent& event)
        {
            ++doubleClicks;
            doubleClickedDate = event.GetDate();
        });
    const wxDateTime selectedDate = calendar.GetDate();
    const wxDateTime adjacentDay =
        selectedDate + wxDateSpan::Day();
    REQUIRE(WaitFor("adjacent CalendarView day realization", [&]()
    {
        return calendar.WinUIDoubleTapDateForTesting(adjacentDay);
    }, 1000));
    CHECK(doubleClicks == 1);
    CHECK(calendar.GetDate() == selectedDate);
    CHECK(doubleClickedDate == selectedDate);

    // Exercise the broad public physical-client-pixel HitTest regions after
    // the CalendarView template and day items are realized. Navigation peers
    // can be narrower than the phase of any fixed sampling lattice, so their
    // contract is verified separately at the centres of the real live peers.
    wxDateTime hitDate;
    wxDateTime surroundingDate;
    wxDateTime weekDate;
    wxDateTime::WeekDay headerWeekday =
        wxDateTime::Inv_WeekDay;
    wxDateTime::WeekDay weekWeekday =
        wxDateTime::Inv_WeekDay;
    wxPoint headerPoint = wxDefaultPosition;
    bool foundDay = false;
    bool foundSurrounding = false;
    bool foundHeader = false;
    bool foundWeekNumber = false;
    const wxSize clientSize = calendar.GetClientSize();
    const int physicalStep = calendar.FromDIP(8);
    REQUIRE(physicalStep > 0);
    for ( int y = physicalStep / 2;
          y < clientSize.y;
          y += physicalStep )
    {
        for ( int x = physicalStep / 2;
              x < clientSize.x;
              x += physicalStep )
        {
            wxDateTime candidate;
            wxDateTime::WeekDay candidateWeekday =
                wxDateTime::Inv_WeekDay;
            switch ( calendar.HitTest(
                         wxPoint(x, y), &candidate,
                         &candidateWeekday) )
            {
                case wxCAL_HITTEST_DAY:
                    if ( !foundDay )
                        hitDate = candidate;
                    foundDay = true;
                    break;

                case wxCAL_HITTEST_SURROUNDING_WEEK:
                    if ( !foundSurrounding )
                        surroundingDate = candidate;
                    foundSurrounding = true;
                    break;

                case wxCAL_HITTEST_HEADER:
                    if ( !foundHeader )
                    {
                        headerWeekday = candidateWeekday;
                        headerPoint = wxPoint(x, y);
                    }
                    foundHeader = true;
                    break;

                case wxCAL_HITTEST_INCMONTH:
                case wxCAL_HITTEST_DECMONTH:
                    break;

                case wxCAL_HITTEST_WEEK:
                    if ( !foundWeekNumber )
                    {
                        weekDate = candidate;
                        weekWeekday = candidateWeekday;
                    }
                    foundWeekNumber = true;
                    break;

                case wxCAL_HITTEST_NOWHERE:
                    break;
            }
        }
    }
    REQUIRE(foundDay);
    CHECK(hitDate.IsValid());
    CHECK(hitDate.GetMonth() == selectedDate.GetMonth());
    CHECK(hitDate.GetYear() == selectedDate.GetYear());
    REQUIRE(foundSurrounding);
    CHECK(surroundingDate.IsValid());
    CHECK((surroundingDate.GetMonth() != selectedDate.GetMonth() ||
           surroundingDate.GetYear() != selectedDate.GetYear()));
    REQUIRE(foundHeader);
    CHECK(headerWeekday != wxDateTime::Inv_WeekDay);

    wxDateTime sameColumnDate;
    bool foundSameColumnDay = false;
    for ( int y = physicalStep / 2;
          y < clientSize.y;
          y += physicalStep )
    {
        const wxCalendarHitTestResult result =
            calendar.HitTest(
                wxPoint(headerPoint.x, y), &sameColumnDate);
        if ( result == wxCAL_HITTEST_DAY ||
             result == wxCAL_HITTEST_SURROUNDING_WEEK )
        {
            foundSameColumnDay = true;
            break;
        }
    }
    REQUIRE(foundSameColumnDay);
    CHECK(headerWeekday == sameColumnDate.GetWeekDay());
    const MUX::UIElement calendarContent =
        wxWinUIGetHostedDateElement(&calendar);
    std::vector<MUXC::CalendarView> calendarPeers;
    wxWinUICollectDateElements(calendarContent, &calendarPeers);
    REQUIRE(calendarPeers.size() == 1);
    wxCalendarCtrl::WinUILayoutTicketForTesting navigationTicket;
    wxWinUICalendarNavigationButtons boundNavigationButtons;
    REQUIRE(wxWinUIWaitForStableCalendarLayout(
        &calendar, calendarPeers.front(),
        "hit-test stable CalendarView navigation peers",
        &navigationTicket, &boundNavigationButtons));
    std::vector<wxWinUICalendarNavigationHitProbe> navigationProbes;
    const wxWinUICalendarNavigationButtons navigationButtons =
        wxWinUIGetCalendarNavigationButtons(
            &calendar, &navigationProbes);
    for ( const wxWinUICalendarNavigationHitProbe& probe :
          navigationProbes )
    {
        INFO("navigation hit: identity=" << probe.identity
             << " mapped=" << probe.mapped
             << " clientX=" << probe.clientPixels.X
             << " clientY=" << probe.clientPixels.Y
             << " hit=" << static_cast<unsigned>(probe.hit));
    }
    REQUIRE(navigationButtons.increment);
    REQUIRE(navigationButtons.decrement);
    CHECK(winrt::get_abi(navigationButtons.increment) ==
          winrt::get_abi(boundNavigationButtons.increment));
    CHECK(winrt::get_abi(navigationButtons.decrement) ==
          winrt::get_abi(boundNavigationButtons.decrement));

    // CalendarView has no native week-number column, so wxWinUI renders and
    // hit-tests a real sibling column aligned to the realized day rows.
    CHECK(calendar.HasFlag(wxCAL_SHOW_WEEK_NUMBERS));
    REQUIRE(foundWeekNumber);
    CHECK(weekDate.IsValid());
    CHECK(weekWeekday == weekDate.GetWeekDay());
    CHECK(weekWeekday == wxDateTime::Mon);
    CHECK(weekDate.GetWeekOfYear() >= 1);
    CHECK(weekDate.GetWeekOfYear() <= 53);
    wxDateTime untouchedDate(1, wxDateTime::Jan, 2001);
    wxDateTime::WeekDay untouchedWeekday = wxDateTime::Fri;
    CHECK(calendar.HitTest(
              wxPoint(-1, -1), &untouchedDate,
              &untouchedWeekday) ==
          wxCAL_HITTEST_NOWHERE);
    CHECK(untouchedDate ==
          wxDateTime(1, wxDateTime::Jan, 2001));
    CHECK(untouchedWeekday == wxDateTime::Fri);

    // Header/background double taps must not fabricate a day event.
    CHECK_FALSE(calendar.WinUIDoubleTapNonDayForTesting());
    CHECK(doubleClicks == 1);
}

TEST_CASE("wxWinUI Calendar template navigation publishes native events",
          "[winui-range-date][calendar][navigation][automation]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    wxCalendarCtrl calendar(
        parent, wxID_ANY,
        wxDateTime(15, wxDateTime::Jul, 2024),
        wxDefaultPosition, wxDefaultSize,
        wxCAL_SHOW_SURROUNDING_WEEKS);

    std::vector<wxEventType> events;
    const auto record = [&events](wxCalendarEvent& event)
    {
        events.push_back(event.GetEventType());
    };
    calendar.Bind(wxEVT_CALENDAR_SEL_CHANGED, record);
    calendar.Bind(wxEVT_CALENDAR_PAGE_CHANGED, record);
    calendar.Bind(wxEVT_CALENDAR_MONTH_CHANGED, record);
    calendar.Bind(wxEVT_CALENDAR_YEAR_CHANGED, record);

    wxWinUICalendarNavigationButtons buttons;
    REQUIRE(WaitFor("CalendarView automation navigation buttons", [&]()
    {
        buttons = wxWinUIGetCalendarNavigationButtons(&calendar);
        return buttons.decrement && buttons.increment &&
               wxWinUIGetInvokeProvider(buttons.decrement) &&
               wxWinUIGetInvokeProvider(buttons.increment);
    }, 1000));

    const wxDateTime initial = calendar.GetDate();
    wxWinUIGetInvokeProvider(buttons.decrement).Invoke();
    REQUIRE(WaitFor("CalendarView decrement navigation", [&]()
    {
        return calendar.GetDate() != initial && events.size() >= 3;
    }, 1000));
    REQUIRE(events.size() == 3);
    CHECK(events[0] == wxEVT_CALENDAR_SEL_CHANGED);
    CHECK(events[1] == wxEVT_CALENDAR_PAGE_CHANGED);
    CHECK((events[2] == wxEVT_CALENDAR_MONTH_CHANGED ||
           events[2] == wxEVT_CALENDAR_YEAR_CHANGED));

    events.clear();
    REQUIRE(WaitFor("CalendarView refreshed increment button", [&]()
    {
        buttons = wxWinUIGetCalendarNavigationButtons(&calendar);
        return buttons.increment &&
               wxWinUIGetInvokeProvider(buttons.increment);
    }, 1000));
    wxWinUIGetInvokeProvider(buttons.increment).Invoke();
    REQUIRE(WaitFor("CalendarView increment navigation", [&]()
    {
        return calendar.GetDate() == initial && events.size() >= 3;
    }, 1000));
    REQUIRE(events.size() == 3);
    CHECK(events[0] == wxEVT_CALENDAR_SEL_CHANGED);
    CHECK(events[1] == wxEVT_CALENDAR_PAGE_CHANGED);
    CHECK((events[2] == wxEVT_CALENDAR_MONTH_CHANGED ||
           events[2] == wxEVT_CALENDAR_YEAR_CHANGED));
}

TEST_CASE("wxWinUI Calendar presentation styles reach the real peer",
          "[winui-range-date][calendar][style][week-numbers]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    wxCalendarCtrl calendar(
        parent, wxID_ANY,
        wxDateTime(15, wxDateTime::May, 2024),
        wxDefaultPosition, wxDefaultSize,
        wxCAL_SUNDAY_FIRST);

    bool mondayFirst = true;
    bool surroundingWeeks = true;
    bool weekNumbers = true;
    REQUIRE(calendar.WinUIGetPeerCalendarStyleForTesting(
        &mondayFirst, &surroundingWeeks, &weekNumbers));
    CHECK_FALSE(mondayFirst);
    CHECK_FALSE(surroundingWeeks);
    CHECK_FALSE(weekNumbers);

    long style = calendar.GetWindowStyleFlag();
    style &= ~wxCAL_SUNDAY_FIRST;
    style |= wxCAL_MONDAY_FIRST |
             wxCAL_SHOW_SURROUNDING_WEEKS |
             wxCAL_SHOW_WEEK_NUMBERS;
    calendar.SetWindowStyleFlag(style);
    REQUIRE(calendar.WinUIGetPeerCalendarStyleForTesting(
        &mondayFirst, &surroundingWeeks, &weekNumbers));
    CHECK(mondayFirst);
    CHECK(surroundingWeeks);
    CHECK(weekNumbers);

    style &= ~(wxCAL_MONDAY_FIRST |
               wxCAL_SHOW_SURROUNDING_WEEKS |
               wxCAL_SHOW_WEEK_NUMBERS);
    style |= wxCAL_SUNDAY_FIRST;
    calendar.SetWindowStyleFlag(style);
    REQUIRE(calendar.WinUIGetPeerCalendarStyleForTesting(
        &mondayFirst, &surroundingWeeks, &weekNumbers));
    CHECK_FALSE(mondayFirst);
    CHECK_FALSE(surroundingWeeks);
    CHECK_FALSE(weekNumbers);

    style |= wxCAL_NO_MONTH_CHANGE;
    calendar.SetWindowStyleFlag(style);
    CHECK_FALSE(calendar.AllowMonthChange());
    wxDateTime lockedMinimum;
    wxDateTime lockedMaximum;
    REQUIRE(calendar.WinUIGetPeerStateForTesting(
        nullptr, &lockedMinimum, &lockedMaximum));
    CHECK(lockedMinimum <= calendar.GetDate());
    CHECK(lockedMaximum >= calendar.GetDate());
    CHECK((lockedMaximum - lockedMinimum).GetDays() < 32);

    style &= ~wxCAL_NO_MONTH_CHANGE;
    calendar.SetWindowStyleFlag(style);
    CHECK(calendar.AllowMonthChange());
    wxDateTime unlockedMinimum;
    wxDateTime unlockedMaximum;
    REQUIRE(calendar.WinUIGetPeerStateForTesting(
        nullptr, &unlockedMinimum, &unlockedMaximum));
    CHECK(unlockedMinimum < lockedMinimum);
    CHECK(unlockedMaximum > lockedMaximum);
}

TEST_CASE("wxWinUI Calendar week numbers cross years in both conventions",
          "[winui-range-date][calendar][week-numbers][boundary]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    const auto checkWeek =
        [](wxCalendarCtrl& calendar,
           const wxDateTime& date,
           const wxDateTime& expectedStart,
           int expectedNumber)
        {
            wxDateTime start;
            int number = 0;
            REQUIRE(WaitFor("CalendarView boundary week realization", [&]()
            {
                return calendar.WinUIGetWeekNumberForTesting(
                           date, &start, &number) &&
                       start == expectedStart &&
                       number == expectedNumber;
            }, 1000));
        };

    wxCalendarCtrl monday(
        parent, wxID_ANY,
        wxDateTime(1, wxDateTime::Jan, 2021),
        wxDefaultPosition, wxDefaultSize,
        wxCAL_MONDAY_FIRST |
            wxCAL_SHOW_SURROUNDING_WEEKS |
            wxCAL_SHOW_WEEK_NUMBERS);
    checkWeek(
        monday,
        wxDateTime(1, wxDateTime::Jan, 2021),
        wxDateTime(28, wxDateTime::Dec, 2020), 53);
    REQUIRE(monday.SetDate(
        wxDateTime(4, wxDateTime::Jan, 2021)));
    checkWeek(
        monday,
        wxDateTime(4, wxDateTime::Jan, 2021),
        wxDateTime(4, wxDateTime::Jan, 2021), 1);

    wxCalendarCtrl sunday(
        parent, wxID_ANY,
        wxDateTime(1, wxDateTime::Jan, 2021),
        wxDefaultPosition, wxDefaultSize,
        wxCAL_SUNDAY_FIRST |
            wxCAL_SHOW_SURROUNDING_WEEKS |
            wxCAL_SHOW_WEEK_NUMBERS);
    checkWeek(
        sunday,
        wxDateTime(1, wxDateTime::Jan, 2021),
        wxDateTime(27, wxDateTime::Dec, 2020), 53);
    REQUIRE(sunday.SetDate(
        wxDateTime(3, wxDateTime::Jan, 2021)));
    checkWeek(
        sunday,
        wxDateTime(3, wxDateTime::Jan, 2021),
        wxDateTime(3, wxDateTime::Jan, 2021), 1);

    // Assert the shared civil calculation used to populate the real labels.
    CHECK(wxDateTime(1, wxDateTime::Jan, 2021).GetWeekOfYear(
              wxDateTime::Monday_First) == 53);
    CHECK(wxDateTime(1, wxDateTime::Jan, 2021).GetWeekOfYear(
              wxDateTime::Sunday_First) == 53);
}

TEST_CASE("wxWinUI Calendar language reaches real week-number overlay",
          "[winui-range-date][calendar][week-numbers][language][peer]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    const winrt::hstring expectedLanguage = L"ar-SA";
    wxWinUITestLanguageOverride language(expectedLanguage);
    const wxDateTime selected(15, wxDateTime::May, 2024);
    wxCalendarCtrl calendar(
        parent, wxID_ANY, selected,
        wxDefaultPosition, wxDefaultSize,
        wxCAL_MONDAY_FIRST |
            wxCAL_SHOW_SURROUNDING_WEEKS |
            wxCAL_SHOW_WEEK_NUMBERS);

    wxDateTime weekStart;
    int weekNumber = 0;
    REQUIRE(WaitFor("localized CalendarView week realization", [&]()
    {
        return calendar.WinUIGetWeekNumberForTesting(
            selected, &weekStart, &weekNumber);
    }, 1000));

    wxWinUICalendarWeekOverlay overlay;
    REQUIRE(WaitFor("localized real week-number labels", [&]()
    {
        overlay = wxWinUIGetCalendarWeekOverlay(&calendar);
        return overlay.root && overlay.canvas &&
               std::any_of(
                   overlay.labels.begin(), overlay.labels.end(),
                   [](const MUXC::TextBlock& label)
                   {
                       return label.Visibility() ==
                                  MUX::Visibility::Visible &&
                              !label.Text().empty();
                   });
    }, 1000));

    CHECK(overlay.root.Language() == expectedLanguage);
    CHECK(overlay.canvas.Language() == expectedLanguage);
    REQUIRE(overlay.labels.size() == 6);
    const winrt::hstring expectedText = wxWinUITestFormatDecimal(
        expectedLanguage, 1, weekNumber);
    bool foundSelectedWeek = false;
    for ( const MUXC::TextBlock& label : overlay.labels )
    {
        CHECK(label.Language() == expectedLanguage);
        if ( label.Visibility() == MUX::Visibility::Visible &&
             label.Text() == expectedText )
        {
            foundSelectedWeek = true;
        }
    }
    CHECK(foundSelectedWeek);
}

TEST_CASE("wxWinUI Calendar coalesces week layout waves across recycling",
          "[winui-range-date][calendar][week-numbers][coalescing][navigation]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);
    const wxDateTime initial(15, wxDateTime::May, 2024);
    const wxDateTime holiday(16, wxDateTime::May, 2024);
    wxWinUITestHolidayAuthorityScope holidays(
        new wxWinUITestHolidayAuthority(holiday));
    wxCalendarCtrl calendar(
        parent, wxID_ANY, initial,
        wxDefaultPosition, wxDefaultSize,
        wxCAL_MONDAY_FIRST |
            wxCAL_SHOW_SURROUNDING_WEEKS |
            wxCAL_SHOW_WEEK_NUMBERS |
            wxCAL_SHOW_HOLIDAYS);

    wxDateTime weekStart;
    int weekNumber = 0;
    REQUIRE(WaitFor("initial coalesced week layout", [&]()
    {
        return calendar.WinUIGetWeekNumberForTesting(
            initial, &weekStart, &weekNumber);
    }, 1000));
    wxYield();
    wxYield();

    const MUX::UIElement content =
        wxWinUIGetHostedDateElement(&calendar);
    std::vector<MUXC::CalendarView> calendarPeers;
    wxWinUICollectDateElements(content, &calendarPeers);
    REQUIRE(calendarPeers.size() == 1);
    const MUXC::CalendarView calendarPeer = calendarPeers.front();

    // CalendarView realizes day containers progressively and can publish a
    // layout edge after ordinary FIFO work queued by a preceding control.
    // Wait for the production projection revisions and the production-bound
    // navigation peers to remain identical across two low-priority edges.
    wxCalendarCtrl::WinUILayoutTicketForTesting stableTicket;
    wxWinUICalendarNavigationButtons buttons;
    const bool initialLayoutStable = wxWinUIWaitForStableCalendarLayout(
        &calendar, calendarPeer, "initial stable CalendarView layout",
        &stableTicket, &buttons);
    if ( !initialLayoutStable )
    {
        wxCalendarCtrl::WinUIWeekRefreshRecoveryForTesting diagnostic;
        REQUIRE(calendar.WinUIGetWeekRefreshRecoveryForTesting(&diagnostic));
        INFO("week recovery: generation=" << diagnostic.generation
             << " request=" << diagnostic.requestRevision
             << " completed=" << diagnostic.completedRevision
             << " pendingCause=" << diagnostic.pendingCause
             << " peerDeferrals=" << diagnostic.peerMutationDeferrals
             << " boundedPending=" << diagnostic.boundedPendingRetries
             << " invalidTickets=" << diagnostic.invalidTicketDeferrals
             << " navGeneration=" << diagnostic.navigationGeneration
             << " navFailures=" << diagnostic.navigationResolveFailures
             << " navExhaustions="
             << diagnostic.navigationRetryExhaustions
             << " navButtons=" << diagnostic.navigationSnapshotButtons
             << " navCandidates=" << diagnostic.navigationCandidates
             << " navFailure=" << diagnostic.navigationFailure
             << " navScheduled=" << diagnostic.navigationScheduled
             << " navRequired="
             << diagnostic.navigationBindingRequired
             << " navBound=" << diagnostic.navigationBound
             << " dayIdentity=" << diagnostic.dayItemIdentityChanges
             << " dayRepeat=" << diagnostic.dayItemRepeatNotifications
             << " layoutEchoes="
             << diagnostic.weekLayoutEchoAbsorptions
             << " dayValidation="
             << diagnostic.dayLayoutValidationPending
             << " dayValidationScheduled="
             << diagnostic.dayLayoutValidationScheduled
             << " scheduled=" << diagnostic.scheduled
             << " degraded=" << diagnostic.degraded
             << " waitingMutation="
             << diagnostic.waitingForMutationEnd);
        REQUIRE(initialLayoutStable);
    }
    REQUIRE(initialLayoutStable);

    const unsigned long long beforeBatch =
        calendar.WinUIGetWeekRefreshRunCountForTesting();
    for ( int n = 0; n != 32; ++n )
        REQUIRE(calendar.WinUIRequestWeekRefreshForTesting());
    // TryEnqueue is the real dispatcher boundary: no caller can turn this
    // same-wave burst into synchronous O(realized-days) work.
    CHECK(calendar.WinUIGetWeekRefreshRunCountForTesting() ==
          beforeBatch);
    REQUIRE(wxWinUIWaitCalendarDispatcherBarrier(
        calendarPeer, "first CalendarView dispatcher FIFO barrier"));
    CHECK(calendar.WinUIGetWeekRefreshRunCountForTesting() ==
          beforeBatch + 1);

    // A refresh callback runs before the first FIFO barrier and would queue
    // any purported follow-up behind it. This second barrier drains that
    // tail and proves the consumed pending bit cannot create a phantom wave.
    REQUIRE(wxWinUIWaitCalendarDispatcherBarrier(
        calendarPeer, "second CalendarView dispatcher FIFO barrier"));
    wxCalendarCtrl::WinUIWeekRefreshRecoveryForTesting batchDiagnostic;
    REQUIRE(calendar.WinUIGetWeekRefreshRecoveryForTesting(
        &batchDiagnostic));
    INFO("coalesced batch: dayIdentity="
         << batchDiagnostic.dayItemIdentityChanges
         << " dayRepeat="
         << batchDiagnostic.dayItemRepeatNotifications
         << " layoutEchoes="
         << batchDiagnostic.weekLayoutEchoAbsorptions
         << " dayValidation="
         << batchDiagnostic.dayLayoutValidationPending
         << " dayValidationScheduled="
         << batchDiagnostic.dayLayoutValidationScheduled
         << " request=" << batchDiagnostic.requestRevision
         << " completed=" << batchDiagnostic.completedRevision);
    CHECK(calendar.WinUIGetWeekRefreshRunCountForTesting() ==
          beforeBatch + 1);

    calendar.Mark(15, true);
    unsigned markColours = 0;
    unsigned holidayColours = 0;
    REQUIRE(WaitFor("marked and holiday days before recycle", [&]()
    {
        return calendar.WinUIGetAppliedDensityColourForTesting(
                   15, nullptr, &markColours) &&
               calendar.WinUIGetAppliedDensityColourForTesting(
                   16, nullptr, &holidayColours) &&
               markColours == 1 && holidayColours == 1;
    }, 1000));
    REQUIRE(wxWinUIWaitForStableCalendarLayout(
        &calendar, calendarPeer, "marked stable CalendarView layout",
        &stableTicket, &buttons));

    const unsigned long long beforeNavigation =
        calendar.WinUIGetWeekRefreshRunCountForTesting();
    const MUXAPR::IInvokeProvider incrementProvider =
        wxWinUIGetInvokeProvider(buttons.increment);
    REQUIRE(incrementProvider);
    incrementProvider.Invoke();
    REQUIRE(WaitFor("week rows after forward navigation", [&]()
    {
        return calendar.GetDate().GetMonth() != initial.GetMonth() &&
               calendar.WinUIGetWeekRefreshRunCountForTesting() >
                   beforeNavigation;
    }, 1000));
    REQUIRE(wxWinUIWaitForStableCalendarLayout(
        &calendar, calendarPeer, "forward stable CalendarView layout",
        &stableTicket, &buttons));
    const MUXAPR::IInvokeProvider decrementProvider =
        wxWinUIGetInvokeProvider(buttons.decrement);
    REQUIRE(decrementProvider);
    decrementProvider.Invoke();
    REQUIRE(WaitFor("week rows after reverse navigation", [&]()
    {
        return calendar.GetDate() == initial &&
               calendar.WinUIGetWeekNumberForTesting(
                   initial, &weekStart, &weekNumber);
    }, 1000));
    REQUIRE(wxWinUIWaitForStableCalendarLayout(
        &calendar, calendarPeer, "reverse stable CalendarView layout",
        &stableTicket, &buttons));
    REQUIRE(WaitFor("marks and holidays survive day recycling", [&]()
    {
        return calendar.WinUIGetAppliedDensityColourForTesting(
                   15, nullptr, &markColours) &&
               calendar.WinUIGetAppliedDensityColourForTesting(
                   16, nullptr, &holidayColours) &&
               markColours == 1 && holidayColours == 1;
    }, 1000));
    CHECK(weekStart == wxDateTime(13, wxDateTime::May, 2024));
    CHECK(weekNumber == 20);

    // Two genuine navigation/layout waves may each need one follow-up after
    // CalendarView recycles its realized containers, but never one refresh
    // per day item or per mark/holiday write.
    CHECK(calendar.WinUIGetWeekRefreshRunCountForTesting() <=
          beforeNavigation + 6);
}

TEST_CASE("wxWinUI Calendar active month topology is unambiguous",
          "[winui-range-date][calendar][hit-test][topology]")
{
    using Day = wxCalendarCtrl::WinUIMonthTopologyDayForTesting;
    using Result =
        wxCalendarCtrl::WinUIMonthTopologyResultForTesting;

    const auto appendSpan = [](
        std::vector<Day> *days,
        int month, int daysInMonth, int firstDay, int lastDay,
        int firstDayOffset, double scale, bool rightToLeft,
        double firstRowTop, bool authoritative)
    {
        REQUIRE(days);
        for ( int day = firstDay; day <= lastDay; ++day )
        {
            const int slot = firstDayOffset + day - 1;
            const int logicalColumn = slot % 7;
            const int column = rightToLeft
                ? 6 - logicalColumn : logicalColumn;
            const int row = slot / 7;
            days->push_back(
                Day{
                    month,
                    day,
                    daysInMonth,
                    column * 20.0 * scale,
                    firstRowTop + row * 18.0 * scale,
                    18.0 * scale,
                    16.0 * scale,
                    static_cast<std::uintptr_t>(days->size() + 1),
                    authoritative});
        }
    };

    const auto makeMay = [&appendSpan](
        bool rightToLeft = false, double scale = 1.0)
    {
        std::vector<Day> days;
        appendSpan(&days, 5, 31, 1, 31, 2, scale,
                   rightToLeft, 80.0, true);
        // Stable surrounding dates are canonical but never a candidate page.
        appendSpan(&days, 4, 30, 25, 30, 1, scale,
                   rightToLeft, 62.0, false);
        return days;
    };

    Result result;
    std::vector<Day> may = makeMay();
    REQUIRE(wxCalendarCtrl::WinUIResolveActiveMonthTopologyForTesting(
        may, &result));
    CHECK(result.month == 5);
    CHECK(result.dayCount == 31);
    CHECK(result.columnCount == 7);
    CHECK(result.rowCount == 5);
    CHECK(result.firstDayTop == 80.0);

    std::vector<Day> rtlScale2 = makeMay(true, 2.0);
    REQUIRE(wxCalendarCtrl::WinUIResolveActiveMonthTopologyForTesting(
        rtlScale2, &result));
    CHECK(result.month == 5);
    CHECK(result.dayCount == 31);
    CHECK(result.columnCount == 7);

    // A recycled fallback for day one is ignored only because the captured
    // realized-day ticket authenticates the live y=80 container.
    std::vector<Day> recycled = makeMay();
    recycled.push_back(
        Day{5, 1, 31, 40.0, 14.0, 18.0, 16.0, 9001, false});
    REQUIRE(wxCalendarCtrl::WinUIResolveActiveMonthTopologyForTesting(
        recycled, &result));
    CHECK(result.firstDayTop == 80.0);

    std::vector<Day> fallbackConflict = makeMay();
    for ( Day& day : fallbackConflict )
        day.authoritative = false;
    fallbackConflict.push_back(
        Day{5, 1, 31, 40.0, 14.0, 18.0, 16.0, 9002, false});
    CHECK_FALSE(
        wxCalendarCtrl::WinUIResolveActiveMonthTopologyForTesting(
            fallbackConflict, &result));

    std::vector<Day> june;
    appendSpan(&june, 6, 30, 1, 30, 5, 1.0,
               false, 210.0, true);
    std::vector<Day> dual = makeMay();
    dual.insert(dual.end(), june.begin(), june.end());
    CHECK_FALSE(
        wxCalendarCtrl::WinUIResolveActiveMonthTopologyForTesting(
            dual, &result));
    std::rotate(dual.begin(), dual.begin() + may.size(), dual.end());
    CHECK_FALSE(
        wxCalendarCtrl::WinUIResolveActiveMonthTopologyForTesting(
            dual, &result));

    std::vector<Day> twoThirtyDayPages = june;
    appendSpan(&twoThirtyDayPages, 9, 30, 1, 30, 3, 1.0,
               false, 340.0, true);
    CHECK_FALSE(
        wxCalendarCtrl::WinUIResolveActiveMonthTopologyForTesting(
            twoThirtyDayPages, &result));

    std::vector<Day> partialSeven;
    appendSpan(&partialSeven, 6, 30, 1, 7, 0, 1.0,
               false, 80.0, true);
    CHECK_FALSE(
        wxCalendarCtrl::WinUIResolveActiveMonthTopologyForTesting(
            partialSeven, &result));
    REQUIRE(wxCalendarCtrl::WinUIResolveActiveMonthTopologyForTesting(
        partialSeven, &result, 6, 1, 7));
    CHECK(result.month == 6);
    CHECK(result.dayCount == 7);
    CHECK(result.columnCount == 7);
    CHECK(result.rowCount == 1);

    // Even an otherwise complete outgoing page cannot override a range-
    // authenticated seven-day page; the transition is intentionally closed.
    std::vector<Day> oldFullWithTruncated = makeMay();
    oldFullWithTruncated.insert(
        oldFullWithTruncated.end(),
        partialSeven.begin(), partialSeven.end());
    CHECK_FALSE(
        wxCalendarCtrl::WinUIResolveActiveMonthTopologyForTesting(
            oldFullWithTruncated, &result, 6, 1, 7));

    std::vector<Day> missingDay = makeMay();
    missingDay.erase(
        std::remove_if(
            missingDay.begin(), missingDay.end(),
            [](const Day& day)
            {
                return day.month == 5 && day.calendarDay == 17;
            }),
        missingDay.end());
    CHECK_FALSE(
        wxCalendarCtrl::WinUIResolveActiveMonthTopologyForTesting(
            missingDay, &result));

    std::vector<Day> wrongSlot = makeMay();
    const auto day17 = std::find_if(
        wrongSlot.begin(), wrongSlot.end(),
        [](const Day& day)
        {
            return day.month == 5 && day.calendarDay == 17;
        });
    REQUIRE(day17 != wrongSlot.end());
    day17->x += 20.0;
    CHECK_FALSE(
        wxCalendarCtrl::WinUIResolveActiveMonthTopologyForTesting(
            wrongSlot, &result));

    std::vector<Day> sixColumns = makeMay();
    for ( Day& day : sixColumns )
    {
        if ( day.month == 5 )
            day.x = ((day.calendarDay - 1) % 6) * 20.0;
    }
    CHECK_FALSE(
        wxCalendarCtrl::WinUIResolveActiveMonthTopologyForTesting(
            sixColumns, &result));
}

TEST_CASE("wxWinUI Calendar bounds automatic week refresh recovery",
          "[winui-range-date][calendar][week-numbers][recovery]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);
    wxCalendarCtrl calendar(
        parent, wxID_ANY,
        wxDateTime(15, wxDateTime::May, 2024),
        wxDefaultPosition, wxDefaultSize,
        wxCAL_MONDAY_FIRST |
            wxCAL_SHOW_SURROUNDING_WEEKS |
            wxCAL_SHOW_WEEK_NUMBERS);

    const MUX::UIElement content =
        wxWinUIGetHostedDateElement(&calendar);
    std::vector<MUXC::CalendarView> calendarPeers;
    wxWinUICollectDateElements(content, &calendarPeers);
    REQUIRE(calendarPeers.size() == 1);
    const MUXC::CalendarView calendarPeer = calendarPeers.front();

    wxCalendarCtrl::WinUILayoutTicketForTesting stableTicket;
    wxWinUICalendarNavigationButtons buttons;
    const bool recoveryLayoutStable = wxWinUIWaitForStableCalendarLayout(
        &calendar, calendarPeer, "week recovery initial stable layout",
        &stableTicket, &buttons);
    if ( !recoveryLayoutStable )
    {
        wxCalendarCtrl::WinUIWeekRefreshRecoveryForTesting diagnostic;
        REQUIRE(calendar.WinUIGetWeekRefreshRecoveryForTesting(&diagnostic));
        INFO("week recovery: generation=" << diagnostic.generation
             << " request=" << diagnostic.requestRevision
             << " completed=" << diagnostic.completedRevision
             << " pendingCause=" << diagnostic.pendingCause
             << " peerDeferrals=" << diagnostic.peerMutationDeferrals
             << " boundedPending=" << diagnostic.boundedPendingRetries
             << " invalidTickets=" << diagnostic.invalidTicketDeferrals
             << " navGeneration=" << diagnostic.navigationGeneration
             << " navFailures=" << diagnostic.navigationResolveFailures
             << " navExhaustions="
             << diagnostic.navigationRetryExhaustions
             << " navButtons=" << diagnostic.navigationSnapshotButtons
             << " navCandidates=" << diagnostic.navigationCandidates
             << " navFailure=" << diagnostic.navigationFailure
             << " navScheduled=" << diagnostic.navigationScheduled
             << " navRequired="
             << diagnostic.navigationBindingRequired
             << " navBound=" << diagnostic.navigationBound
             << " dayIdentity=" << diagnostic.dayItemIdentityChanges
             << " dayRepeat=" << diagnostic.dayItemRepeatNotifications
             << " layoutEchoes="
             << diagnostic.weekLayoutEchoAbsorptions
             << " dayValidation="
             << diagnostic.dayLayoutValidationPending
             << " dayValidationScheduled="
             << diagnostic.dayLayoutValidationScheduled
             << " scheduled=" << diagnostic.scheduled
             << " degraded=" << diagnostic.degraded
             << " waitingMutation="
             << diagnostic.waitingForMutationEnd);
        REQUIRE(recoveryLayoutStable);
    }
    REQUIRE(recoveryLayoutStable);

    wxCalendarCtrl::WinUIWeekRefreshRecoveryForTesting recovery;
    REQUIRE(calendar.WinUIGetWeekRefreshRecoveryForTesting(&recovery));
    // The projection signature is sourced only from the uniquely dominant
    // current month. May 2024 supplies all 31 civil dates and exactly seven
    // physical weekday columns; transient surrounding/recycle peers are not
    // allowed to become recovery input.
    CHECK(recovery.projectedMonthDays == 31);
    CHECK(recovery.projectedWeekdayColumns == 7);
    const std::uint64_t initialGeneration = recovery.generation;
    const unsigned long long beforeFailure =
        calendar.WinUIGetWeekRefreshRunCountForTesting();

    // Inject at the real dispatcher worker boundary. The initial attempt plus
    // exactly three automatic retries fail; no test calls the recovery helper
    // directly and no artificial dispatcher completion is synthesized.
    REQUIRE(calendar.WinUISetWeekRefreshFailuresForTesting(4));
    REQUIRE(calendar.WinUIRequestWeekRefreshForTesting());
    REQUIRE(WaitFor("bounded week refresh recovery exhaustion", [&]()
    {
        return calendar.WinUIGetWeekRefreshRecoveryForTesting(&recovery) &&
               recovery.generation != initialGeneration &&
               recovery.degraded && !recovery.scheduled &&
               recovery.automaticRetriesRemaining == 0 &&
               recovery.injectedFailuresRemaining == 0;
    }, 1000));
    const unsigned long long stoppedAt =
        calendar.WinUIGetWeekRefreshRunCountForTesting();
    CHECK(stoppedAt == beforeFailure + 4);
    CHECK(recovery.completedRevision != recovery.requestRevision);

    REQUIRE(wxWinUIWaitCalendarDispatcherBarrier(
        calendarPeer, "degraded week recovery low-priority barrier",
        MUXD::DispatcherQueuePriority::Low));
    REQUIRE(wxWinUIWaitCalendarDispatcherBarrier(
        calendarPeer, "degraded week recovery follow-up barrier",
        MUXD::DispatcherQueuePriority::Low));
    CHECK(calendar.WinUIGetWeekRefreshRunCountForTesting() == stoppedAt);
    REQUIRE(calendar.WinUIGetWeekRefreshRecoveryForTesting(&recovery));
    REQUIRE(recovery.degraded);
    REQUIRE_FALSE(recovery.dayLayoutValidationPending);
    REQUIRE_FALSE(recovery.dayLayoutValidationScheduled);

    // An identical late day-layout signature is not proof that the failed
    // week-refresh request was published. It may drain its own validation
    // ticket, but must leave this incomplete request degraded and must not
    // create a fresh generation or retry budget.
    const std::uint64_t incompleteGeneration = recovery.generation;
    const std::uint64_t incompleteRequest = recovery.requestRevision;
    const std::uint64_t incompleteCompleted = recovery.completedRevision;
    const std::uint64_t beforeIncompleteEchoes =
        recovery.weekLayoutEchoAbsorptions;
    REQUIRE(calendar.WinUIRequestDayLayoutValidationForTesting());
    REQUIRE(WaitFor("incomplete identical day-layout validation", [&]()
    {
        return calendar.WinUIGetWeekRefreshRecoveryForTesting(&recovery) &&
               !recovery.dayLayoutValidationPending &&
               !recovery.dayLayoutValidationScheduled &&
               !recovery.scheduled;
    }, 1000));
    CHECK(recovery.generation == incompleteGeneration);
    CHECK(recovery.requestRevision == incompleteRequest);
    CHECK(recovery.completedRevision == incompleteCompleted);
    CHECK(recovery.completedRevision != recovery.requestRevision);
    CHECK(recovery.degraded);
    CHECK(recovery.automaticRetriesRemaining == 0);
    CHECK(recovery.weekLayoutEchoAbsorptions ==
          beforeIncompleteEchoes + 1);
    CHECK(calendar.WinUIGetWeekRefreshRunCountForTesting() == stoppedAt);
    wxCalendarCtrl::WinUILayoutTicketForTesting incompleteTicket;
    CHECK_FALSE(calendar.WinUIGetStableLayoutTicketForTesting(
        &incompleteTicket));

    // A new genuine request owns a new recovery generation and is the only
    // operation allowed to rearm the budget after degradation.
    const std::uint64_t degradedGeneration = recovery.generation;
    const std::uint64_t degradedRequestRevision =
        recovery.requestRevision;
    const std::uint64_t beforeRearmIdentityChanges =
        recovery.dayItemIdentityChanges;
    const std::uint64_t beforeRearmRepeatNotifications =
        recovery.dayItemRepeatNotifications;
    const std::uint64_t beforeRearmLayoutEchoes =
        recovery.weekLayoutEchoAbsorptions;
    REQUIRE(calendar.WinUIRequestWeekRefreshForTesting());
    REQUIRE(wxWinUIWaitForStableCalendarLayout(
        &calendar, calendarPeer, "week recovery rearmed stable layout",
        &stableTicket, &buttons));
    REQUIRE(calendar.WinUIGetWeekRefreshRecoveryForTesting(&recovery));
    CHECK(recovery.generation != degradedGeneration);
    CHECK(recovery.generation == degradedGeneration + 1);
    CHECK(recovery.requestRevision == degradedRequestRevision + 1);
    CHECK_FALSE(recovery.degraded);
    CHECK_FALSE(recovery.scheduled);
    CHECK_FALSE(recovery.dayLayoutValidationPending);
    CHECK_FALSE(recovery.dayLayoutValidationScheduled);
    CHECK(recovery.automaticRetriesRemaining == 3);
    CHECK(recovery.completedRevision == recovery.requestRevision);
    CAPTURE(stoppedAt,
            calendar.WinUIGetWeekRefreshRunCountForTesting(),
            degradedGeneration,
            recovery.generation,
            degradedRequestRevision,
            recovery.requestRevision,
            beforeRearmIdentityChanges,
            recovery.dayItemIdentityChanges,
            beforeRearmRepeatNotifications,
            recovery.dayItemRepeatNotifications,
            beforeRearmLayoutEchoes,
            recovery.weekLayoutEchoAbsorptions,
            recovery.boundedPendingRetries,
            recovery.invalidTicketDeferrals);
    CHECK(calendar.WinUIGetWeekRefreshRunCountForTesting() == stoppedAt + 1);

    // A peer mutation is a benign deferral, not failed convergence. Exercise
    // the actual PeerMutation guard branch twice and prove mutation-ended
    // resumption preserves the entire retry budget.
    const std::uint64_t beforeMutationGeneration = recovery.generation;
    const std::uint64_t beforeMutationRequestRevision =
        recovery.requestRevision;
    const std::uint64_t beforePeerDeferrals =
        recovery.peerMutationDeferrals;
    const std::uint64_t beforeBoundedPending =
        recovery.boundedPendingRetries;
    const unsigned long long beforeMutation =
        calendar.WinUIGetWeekRefreshRunCountForTesting();
    REQUIRE(calendar.WinUISetWeekRefreshPeerMutationPassesForTesting(2));
    REQUIRE(calendar.WinUIRequestWeekRefreshForTesting());
    REQUIRE(wxWinUIWaitForStableCalendarLayout(
        &calendar, calendarPeer, "peer mutation week recovery stable layout",
        &stableTicket, &buttons));
    REQUIRE(calendar.WinUIGetWeekRefreshRecoveryForTesting(&recovery));
    CHECK(recovery.generation != beforeMutationGeneration);
    CHECK(recovery.generation == beforeMutationGeneration + 1);
    CHECK(recovery.requestRevision ==
          beforeMutationRequestRevision + 1);
    CHECK_FALSE(recovery.degraded);
    CHECK_FALSE(recovery.scheduled);
    CHECK_FALSE(recovery.waitingForMutationEnd);
    CHECK_FALSE(recovery.dayLayoutValidationPending);
    CHECK_FALSE(recovery.dayLayoutValidationScheduled);
    CHECK(recovery.automaticRetriesRemaining == 3);
    CHECK(recovery.injectedPeerMutationPassesRemaining == 0);
    CHECK(recovery.peerMutationDeferrals == beforePeerDeferrals + 2);
    CHECK(recovery.boundedPendingRetries == beforeBoundedPending);
    CHECK(recovery.projectedMonthDays == 31);
    CHECK(recovery.projectedWeekdayColumns == 7);
    CAPTURE(beforeMutation,
            calendar.WinUIGetWeekRefreshRunCountForTesting(),
            beforeMutationGeneration,
            recovery.generation,
            beforeMutationRequestRevision,
            recovery.requestRevision,
            recovery.dayItemRepeatNotifications,
            recovery.weekLayoutEchoAbsorptions);
    CHECK(calendar.WinUIGetWeekRefreshRunCountForTesting() ==
          beforeMutation + 3);

    // Exercise the distinct same-generation invalid-ticket pending path. It
    // must share the finite error budget instead of creating an unbounded
    // dispatcher chain under a nested pump.
    const std::uint64_t beforePendingGeneration = recovery.generation;
    const std::uint64_t beforeInvalidTickets =
        recovery.invalidTicketDeferrals;
    const std::uint64_t beforePendingRetries =
        recovery.boundedPendingRetries;
    const unsigned long long beforePending =
        calendar.WinUIGetWeekRefreshRunCountForTesting();
    REQUIRE(calendar.WinUISetWeekRefreshPendingPassesForTesting(4));
    REQUIRE(calendar.WinUIRequestWeekRefreshForTesting());
    REQUIRE(WaitFor("bounded pending week refresh exhaustion", [&]()
    {
        return calendar.WinUIGetWeekRefreshRecoveryForTesting(&recovery) &&
               recovery.generation != beforePendingGeneration &&
               recovery.degraded && !recovery.scheduled &&
               recovery.automaticRetriesRemaining == 0 &&
               recovery.injectedPendingPassesRemaining == 0;
    }, 1000));
    const unsigned long long pendingStoppedAt =
        calendar.WinUIGetWeekRefreshRunCountForTesting();
    CHECK(pendingStoppedAt == beforePending + 4);
    CHECK(recovery.completedRevision != recovery.requestRevision);
    CHECK(recovery.invalidTicketDeferrals == beforeInvalidTickets + 4);
    CHECK(recovery.boundedPendingRetries == beforePendingRetries + 3);

    REQUIRE(wxWinUIWaitCalendarDispatcherBarrier(
        calendarPeer, "degraded pending recovery low-priority barrier",
        MUXD::DispatcherQueuePriority::Low));
    REQUIRE(wxWinUIWaitCalendarDispatcherBarrier(
        calendarPeer, "degraded pending recovery follow-up barrier",
        MUXD::DispatcherQueuePriority::Low));
    CHECK(calendar.WinUIGetWeekRefreshRunCountForTesting() ==
          pendingStoppedAt);

    const std::uint64_t pendingDegradedGeneration = recovery.generation;
    REQUIRE(calendar.WinUIRequestWeekRefreshForTesting());
    REQUIRE(wxWinUIWaitForStableCalendarLayout(
        &calendar, calendarPeer, "pending recovery rearmed stable layout",
        &stableTicket, &buttons));
    REQUIRE(calendar.WinUIGetWeekRefreshRecoveryForTesting(&recovery));
    CHECK(recovery.generation != pendingDegradedGeneration);
    CHECK_FALSE(recovery.degraded);
    CHECK_FALSE(recovery.scheduled);
    CHECK(recovery.completedRevision == recovery.requestRevision);
    CHECK(calendar.WinUIGetWeekRefreshRunCountForTesting() ==
          pendingStoppedAt + 1);

    // Throw inside the actual queued day-layout callback. The exception must
    // be contained there, consume exactly one existing same-generation retry,
    // and leave neither a stuck pending flag nor an unbounded callback chain.
    const std::uint64_t beforeValidationGeneration = recovery.generation;
    const std::uint64_t beforeValidationRequest = recovery.requestRevision;
    const std::uint64_t beforeValidationRecoveries =
        recovery.dayLayoutFailureRecoveries;
    const std::uint64_t beforeValidationRetries =
        recovery.boundedPendingRetries;
    REQUIRE(calendar.WinUIRequestDayLayoutValidationFailureForTesting());
    REQUIRE(WaitFor("day-layout callback exception recovery", [&]()
    {
        return calendar.WinUIGetWeekRefreshRecoveryForTesting(&recovery) &&
               recovery.injectedDayLayoutFailuresRemaining == 0 &&
               !recovery.dayLayoutValidationPending &&
               !recovery.dayLayoutValidationScheduled;
    }, 1000));
    CHECK(recovery.generation == beforeValidationGeneration);
    CHECK(recovery.requestRevision == beforeValidationRequest);
    CHECK_FALSE(recovery.degraded);
    CHECK(recovery.automaticRetriesRemaining == 2);
    CHECK(recovery.boundedPendingRetries ==
          beforeValidationRetries + 1);
    CHECK(recovery.dayLayoutFailureRecoveries ==
          beforeValidationRecoveries + 1);

    std::uint64_t validationRecoveries =
        recovery.dayLayoutFailureRecoveries;
    std::uint64_t validationRetries = recovery.boundedPendingRetries;
    REQUIRE(calendar.WinUIRequestDayLayoutValidationFailureForTesting(
        wxCalendarCtrl::WinUIDayLayoutValidationFailureForTesting::
            TransientRead));
    REQUIRE(WaitFor("day-layout transient-read recovery", [&]()
    {
        return calendar.WinUIGetWeekRefreshRecoveryForTesting(&recovery) &&
               recovery.injectedDayLayoutFailuresRemaining == 0 &&
               !recovery.dayLayoutValidationPending &&
               !recovery.dayLayoutValidationScheduled;
    }, 1000));
    CHECK(recovery.generation == beforeValidationGeneration);
    CHECK(recovery.requestRevision == beforeValidationRequest);
    CHECK_FALSE(recovery.degraded);
    CHECK(recovery.automaticRetriesRemaining == 1);
    CHECK(recovery.boundedPendingRetries == validationRetries + 1);
    CHECK(recovery.dayLayoutFailureRecoveries ==
          validationRecoveries + 1);

    validationRecoveries = recovery.dayLayoutFailureRecoveries;
    validationRetries = recovery.boundedPendingRetries;
    REQUIRE(calendar.WinUIRequestDayLayoutValidationFailureForTesting(
        wxCalendarCtrl::WinUIDayLayoutValidationFailureForTesting::
            QueueRefusal));
    REQUIRE(WaitFor("day-layout queue-refusal recovery", [&]()
    {
        return calendar.WinUIGetWeekRefreshRecoveryForTesting(&recovery) &&
               recovery.injectedDayLayoutFailuresRemaining == 0 &&
               !recovery.dayLayoutValidationPending &&
               !recovery.dayLayoutValidationScheduled;
    }, 1000));
    CHECK(recovery.generation == beforeValidationGeneration);
    CHECK(recovery.requestRevision == beforeValidationRequest);
    CHECK_FALSE(recovery.degraded);
    CHECK(recovery.automaticRetriesRemaining == 0);
    CHECK(recovery.boundedPendingRetries == validationRetries + 1);
    CHECK(recovery.dayLayoutFailureRecoveries ==
          validationRecoveries + 1);

    // Exhaust the validator with a projection that is already completed.
    // A later successful validation of the identical signature may clear
    // only this validator-degraded bit: it must not rearm the exhausted
    // budget or advance the request/retry generation.
    const std::uint64_t completedGeneration = recovery.generation;
    const std::uint64_t completedRequest = recovery.requestRevision;
    const std::uint64_t completedRevision = recovery.completedRevision;
    const std::uint64_t beforeExhaustedValidationRecoveries =
        recovery.dayLayoutFailureRecoveries;
    const unsigned long long beforeExhaustedValidationRuns =
        calendar.WinUIGetWeekRefreshRunCountForTesting();
    REQUIRE(completedRevision == completedRequest);
    REQUIRE(recovery.automaticRetriesRemaining == 0);
    REQUIRE(calendar.WinUIRequestDayLayoutValidationFailureForTesting());
    REQUIRE(WaitFor("completed day-layout validation exhaustion", [&]()
    {
        return calendar.WinUIGetWeekRefreshRecoveryForTesting(&recovery) &&
               recovery.injectedDayLayoutFailuresRemaining == 0 &&
               !recovery.dayLayoutValidationPending &&
               !recovery.dayLayoutValidationScheduled &&
               recovery.degraded;
    }, 1000));
    CHECK(recovery.generation == completedGeneration);
    CHECK(recovery.requestRevision == completedRequest);
    CHECK(recovery.completedRevision == completedRevision);
    CHECK(recovery.automaticRetriesRemaining == 0);
    CHECK(recovery.dayLayoutFailureRecoveries ==
          beforeExhaustedValidationRecoveries + 1);
    CHECK(calendar.WinUIGetWeekRefreshRunCountForTesting() ==
          beforeExhaustedValidationRuns);

    const std::uint64_t beforeHealingEchoes =
        recovery.weekLayoutEchoAbsorptions;
    REQUIRE(calendar.WinUIRequestDayLayoutValidationForTesting());
    wxCalendarCtrl::WinUILayoutTicketForTesting healedTicket;
    REQUIRE(WaitFor("completed identical day-layout validation healing", [&]()
    {
        return calendar.WinUIGetWeekRefreshRecoveryForTesting(&recovery) &&
               !recovery.dayLayoutValidationPending &&
               !recovery.dayLayoutValidationScheduled &&
               !recovery.scheduled && !recovery.degraded &&
               recovery.completedRevision == recovery.requestRevision &&
               calendar.WinUIGetStableLayoutTicketForTesting(&healedTicket);
    }, 1000));
    CHECK(recovery.generation == completedGeneration);
    CHECK(recovery.requestRevision == completedRequest);
    CHECK(recovery.completedRevision == completedRevision);
    CHECK(recovery.automaticRetriesRemaining == 0);
    CHECK(recovery.weekLayoutEchoAbsorptions == beforeHealingEchoes + 1);
    CHECK(calendar.WinUIGetWeekRefreshRunCountForTesting() ==
          beforeExhaustedValidationRuns);

    REQUIRE(wxWinUIWaitCalendarDispatcherBarrier(
        calendarPeer, "healed day-layout validation barrier",
        MUXD::DispatcherQueuePriority::Low));
    wxCalendarCtrl::WinUILayoutTicketForTesting confirmedTicket;
    REQUIRE(calendar.WinUIGetStableLayoutTicketForTesting(&confirmedTicket));
    CHECK(confirmedTicket.layoutRevision == healedTicket.layoutRevision);
    CHECK(confirmedTicket.realizedDaysRevision ==
          healedTicket.realizedDaysRevision);
    CHECK(confirmedTicket.weekRefreshRevision ==
          healedTicket.weekRefreshRevision);
    CHECK(confirmedTicket.navigationGeneration ==
          healedTicket.navigationGeneration);
    CHECK(confirmedTicket.decrementIdentity == healedTicket.decrementIdentity);
    CHECK(confirmedTicket.incrementIdentity == healedTicket.incrementIdentity);
}

TEST_CASE("wxWinUI Calendar marks follow hot theme and system colours",
          "[winui-range-date][calendar][marks][theme][winui-009]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    wxCalendarCtrl calendar(
        parent, wxID_ANY,
        wxDateTime(15, wxDateTime::May, 2024));
    REQUIRE(calendar.WinUIHasThemeChangedHandlerForTesting());
    calendar.Mark(15, true);

    wxColour applied;
    unsigned count = 0;
    unsigned long long revision = 0;
    REQUIRE(WaitFor("marked CalendarView day realization", [&]()
    {
        return calendar.WinUIGetAppliedDensityColourForTesting(
                   15, &applied, &count, &revision) &&
               count == 1;
    }, 1000));
    const unsigned long long initialRevision = revision;

    const wxColour lightThemeMark(17, 83, 149);
    REQUIRE(calendar.WinUISetTodayForegroundForTesting(
        lightThemeMark));
    REQUIRE(calendar.WinUIDeliverThemeChangedForTesting());
    REQUIRE(WaitFor("light CalendarView ActualThemeChanged refresh", [&]()
    {
        return calendar.WinUIGetAppliedDensityColourForTesting(
                   15, &applied, &count, &revision) &&
               revision > initialRevision;
    }, 1000));
    CHECK(count == 1);
    CHECK(applied == lightThemeMark);
    const unsigned long long lightRevision = revision;

    const wxColour darkThemeMark(241, 196, 15);
    REQUIRE(calendar.WinUISetTodayForegroundForTesting(
        darkThemeMark));
    REQUIRE(calendar.WinUIDeliverThemeChangedForTesting());
    REQUIRE(WaitFor("dark CalendarView ActualThemeChanged refresh", [&]()
    {
        return calendar.WinUIGetAppliedDensityColourForTesting(
                   15, &applied, &count, &revision) &&
               revision > lightRevision;
    }, 1000));
    CHECK(count == 1);
    CHECK(applied == darkThemeMark);
    const unsigned long long darkRevision = revision;

    REQUIRE(calendar.WinUIUseSystemMarkColourFallbackForTesting());
    REQUIRE(calendar.WinUIDeliverThemeChangedForTesting());
    REQUIRE(WaitFor("fallback CalendarView ActualThemeChanged refresh", [&]()
    {
        return calendar.WinUIGetAppliedDensityColourForTesting(
                   15, &applied, &count, &revision) &&
               revision > darkRevision;
    }, 1000));
    const wxColour systemHighlight =
        wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT);
    REQUIRE(systemHighlight.IsOk());
    CHECK(count == 1);
    CHECK(applied == systemHighlight);
    const unsigned long long fallbackRevision = revision;

    // The wx system-colour notification owns an independent refresh route
    // for a live High Contrast palette transition. The exact successful
    // SetDensityColors write must advance from the live system fallback.
    wxSysColourChangedEvent coloursChanged;
    coloursChanged.SetEventObject(&calendar);
    coloursChanged.SetId(calendar.GetId());
    calendar.ProcessWindowEvent(coloursChanged);
    REQUIRE(calendar.WinUIGetAppliedDensityColourForTesting(
        15, &applied, &count, &revision));
    CHECK(count == 1);
    CHECK(applied == systemHighlight);
    CHECK(revision > fallbackRevision);

    calendar.Mark(15, false);
    REQUIRE(calendar.WinUIGetAppliedDensityColourForTesting(
        15, &applied, &count, &revision));
    CHECK(count == 0);
    CHECK_FALSE(applied.IsOk());
}

TEST_CASE("wxWinUI Calendar hit testing stays coherent through LTR RTL LTR",
          "[winui-range-date][calendar][hit-test][rtl][winui-009]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    const wxDateTime selected(15, wxDateTime::May, 2024);
    wxCalendarCtrl calendar(
        parent, wxID_ANY, selected,
        wxDefaultPosition, wxDefaultSize,
        wxCAL_MONDAY_FIRST |
            wxCAL_SHOW_SURROUNDING_WEEKS);
    calendar.SetLayoutDirection(wxLayout_LeftToRight);

    const auto checkDateAtPoint =
        [&](const wxPoint& point)
        {
            wxDateTime hit;
            const wxCalendarHitTestResult result =
                calendar.HitTest(point, &hit);
            CHECK(result == wxCAL_HITTEST_DAY);
            CHECK(hit == selected);
        };

    bool rightToLeft = true;
    wxPoint firstLTR;
    REQUIRE(WaitFor("LTR CalendarView date geometry", [&]()
    {
        return calendar.WinUIGetPeerRightToLeftForTesting(
                   &rightToLeft) &&
               !rightToLeft &&
               calendar.WinUIGetDateClientPointForTesting(
                   selected, &firstLTR);
    }, 1000));
    checkDateAtPoint(firstLTR);

    calendar.SetLayoutDirection(wxLayout_RightToLeft);
    wxPoint rtl;
    REQUIRE(WaitFor("RTL CalendarView date geometry", [&]()
    {
        return calendar.WinUIGetPeerRightToLeftForTesting(
                   &rightToLeft) &&
               rightToLeft &&
               calendar.WinUIGetDateClientPointForTesting(
                   selected, &rtl);
    }, 1000));
    checkDateAtPoint(rtl);
    CHECK(rtl.x != firstLTR.x);

    calendar.SetLayoutDirection(wxLayout_LeftToRight);
    wxPoint secondLTR;
    REQUIRE(WaitFor("restored LTR CalendarView date geometry", [&]()
    {
        return calendar.WinUIGetPeerRightToLeftForTesting(
                   &rightToLeft) &&
               !rightToLeft &&
               calendar.WinUIGetDateClientPointForTesting(
                   selected, &secondLTR);
    }, 1000));
    checkDateAtPoint(secondLTR);
    CHECK(std::abs(secondLTR.x - firstLTR.x) <= 1);
    CHECK(std::abs(secondLTR.y - firstLTR.y) <= 1);
}

TEST_CASE("wxWinUI Calendar callback teardown is destruction-safe",
          "[winui-range-date][calendar][lifetime]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    wxCalendarCtrl *calendar =
        new wxCalendarCtrl(
            parent, wxID_ANY,
            wxDateTime(15, wxDateTime::May, 2024));
    unsigned events = 0;
    calendar->Bind(
        wxEVT_CALENDAR_SEL_CHANGED,
        [&](wxCalendarEvent&)
        {
            ++events;
            wxCalendarCtrl * const doomed = calendar;
            calendar = nullptr;
            delete doomed;
        });

    wxCalendarCtrl * const invoking = calendar;
    REQUIRE(invoking->WinUISetPeerDateForTesting(
        wxDateTime(16, wxDateTime::May, 2024)));
    CHECK(calendar == nullptr);
    CHECK(events == 1);

    for ( int i = 0; i < 100; ++i )
    {
        wxCalendarCtrl * const transient =
            new wxCalendarCtrl(
                parent, wxID_ANY,
                wxDateTime(15, wxDateTime::May, 2024));
        delete transient;
    }
}

TEST_CASE("wxWinUI Calendar Create revalidates Loaded and ForceRender",
          "[winui-range-date][calendar][create][lifetime]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);
    const wxDateTime initial(15, wxDateTime::May, 2024);

    SECTION("shared-host Loaded boundary")
    {
        wxCalendarCtrl *calendar = new wxCalendarCtrl;
        wxCalendarCtrl * const invoking = calendar;
        bool attached = false;
        wxWinUITopLevelHost::TestOnNextSlotAttached(
            [&](wxWindow *window)
            {
                attached = true;
                CHECK(window == invoking);
                wxCalendarCtrl * const doomed = calendar;
                calendar = nullptr;
                delete doomed;
            });

        const bool created =
            invoking->Create(parent, wxID_ANY, initial);
        wxWinUITopLevelHost::TestOnNextSlotAttached({});
        CHECK(attached);
        CHECK_FALSE(created);
        CHECK(calendar == nullptr);
        delete calendar;
    }

    SECTION("ForceRender synchronous slot flush")
    {
        if ( wxWinUITopLevelHost * const host =
                 wxWinUITopLevelHost::ForWindow(parent, false) )
        {
            host->FlushSync();
        }
        wxCalendarCtrl *calendar = new wxCalendarCtrl;
        wxCalendarCtrl * const invoking = calendar;
        bool attached = false;
        bool slotSynced = false;
        wxWinUITopLevelHost::TestOnNextSlotAttached(
            [&](wxWindow *window)
            {
                attached = true;
                CHECK(window == invoking);
                wxWinUITopLevelHost::TestOnNextSlotSynced(
                    [&](wxWindow *syncedWindow)
                    {
                        slotSynced = true;
                        CHECK(syncedWindow == invoking);
                        wxCalendarCtrl * const doomed =
                            calendar;
                        calendar = nullptr;
                        delete doomed;
                    });
            });

        const bool created =
            invoking->Create(parent, wxID_ANY, initial);
        wxWinUITopLevelHost::TestOnNextSlotAttached({});
        wxWinUITopLevelHost::TestOnNextSlotSynced({});
        CHECK(attached);
        CHECK(slotSynced);
        CHECK_FALSE(created);
        CHECK(calendar == nullptr);
        delete calendar;
    }
}

TEST_CASE("wxWinUI Calendar setter stops after destructive ForceRender",
          "[winui-range-date][calendar][lifetime][force-render]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    wxCalendarCtrl *calendar =
        new wxCalendarCtrl(
            parent, wxID_ANY,
            wxDateTime(15, wxDateTime::May, 2024));
    wxCalendarCtrl * const invoking = calendar;
    if ( wxWinUITopLevelHost * const host =
             wxWinUITopLevelHost::ForWindow(parent, false) )
    {
        host->FlushSync();
    }
    invoking->SetSize(wxSize(311, 359));
    bool slotSynced = false;
    wxWinUITopLevelHost::TestOnNextSlotSynced(
        [&](wxWindow *window)
        {
            slotSynced = true;
            CHECK(window == invoking);
            wxCalendarCtrl * const doomed = calendar;
            calendar = nullptr;
            delete doomed;
        });

    CHECK_FALSE(invoking->SetDate(
        wxDateTime(16, wxDateTime::May, 2024)));
    wxWinUITopLevelHost::TestOnNextSlotSynced({});
    CHECK(slotSynced);
    CHECK(calendar == nullptr);
    delete calendar;
}

TEST_CASE("wxWinUI Calendar nested setter is last writer",
          "[winui-range-date][calendar][reentrance][force-render]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);
    wxCalendarCtrl calendar(
        parent, wxID_ANY,
        wxDateTime(15, wxDateTime::May, 2024));
    if ( wxWinUITopLevelHost * const host =
             wxWinUITopLevelHost::ForWindow(parent, false) )
    {
        host->FlushSync();
    }

    const wxDateTime outer(16, wxDateTime::May, 2024);
    const wxDateTime nested(17, wxDateTime::May, 2024);
    calendar.SetSize(wxSize(313, 361));
    bool slotSynced = false;
    bool nestedResult = false;
    wxWinUITopLevelHost::TestOnNextSlotSynced(
        [&](wxWindow *window)
        {
            slotSynced = true;
            CHECK(window == &calendar);
            nestedResult = calendar.SetDate(nested);
        });

    CHECK_FALSE(calendar.SetDate(outer));
    wxWinUITopLevelHost::TestOnNextSlotSynced({});
    CHECK(slotSynced);
    CHECK(nestedResult);
    CHECK(calendar.GetDate() == nested);
    wxDateTime peer;
    REQUIRE(calendar.WinUIGetPeerStateForTesting(
        &peer, nullptr, nullptr));
    CHECK(peer == nested);
}

TEST_CASE("wxWinUI Calendar nested range setter is last writer",
          "[winui-range-date][calendar][range][reentrance][force-render]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);
    const wxDateTime initial(15, wxDateTime::May, 2024);
    wxCalendarCtrl calendar(parent, wxID_ANY, initial);
    if ( wxWinUITopLevelHost * const host =
             wxWinUITopLevelHost::ForWindow(parent, false) )
    {
        host->FlushSync();
    }

    const wxDateTime outerMinimum(1, wxDateTime::May, 2024);
    const wxDateTime outerMaximum(31, wxDateTime::May, 2024);
    const wxDateTime nestedMinimum(10, wxDateTime::May, 2024);
    const wxDateTime nestedMaximum(20, wxDateTime::May, 2024);
    calendar.SetSize(wxSize(315, 363));
    bool slotSynced = false;
    bool nestedResult = false;
    wxWinUITopLevelHost::TestOnNextSlotSynced(
        [&](wxWindow *window)
        {
            slotSynced = true;
            CHECK(window == &calendar);
            nestedResult = calendar.SetDateRange(
                nestedMinimum, nestedMaximum);
        });

    CHECK_FALSE(calendar.SetDateRange(outerMinimum, outerMaximum));
    wxWinUITopLevelHost::TestOnNextSlotSynced({});
    CHECK(slotSynced);
    CHECK(nestedResult);
    wxDateTime minimum;
    wxDateTime maximum;
    REQUIRE(calendar.GetDateRange(&minimum, &maximum));
    CHECK(minimum == nestedMinimum);
    CHECK(maximum == nestedMaximum);
    wxDateTime peer;
    wxDateTime peerMinimum;
    wxDateTime peerMaximum;
    REQUIRE(calendar.WinUIGetPeerStateForTesting(
        &peer, &peerMinimum, &peerMaximum));
    CHECK(peer == initial);
    CHECK(peerMinimum == nestedMinimum);
    CHECK(peerMaximum == nestedMaximum);
}

#endif // wxUSE_CALENDARCTRL

#endif // __WXWINUI__ && wxUSE_WINUI3
