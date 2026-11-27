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

#ifdef WXWINUI_TEST_SUPPORT
    #include "date-time-test-access.h"
#endif
#include "wx/dateevt.h"

#include "private.h"

#include <winrt/Microsoft.UI.Xaml.Automation.h>
#include <winrt/Microsoft.UI.Xaml.Input.h>
#include <winrt/Microsoft.UI.Xaml.Media.h>
#include <winrt/Windows.Globalization.h>
#include <winrt/Windows.Globalization.DateTimeFormatting.h>
#include <winrt/Windows.Globalization.NumberFormatting.h>
#include <winrt/Windows.System.h>

#include <chrono>
#include <cmath>
#include <limits>
#include <memory>
#include <vector>

namespace MUX = winrt::Microsoft::UI::Xaml;
namespace MUXA = winrt::Microsoft::UI::Xaml::Automation;
namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;
namespace MUXI = winrt::Microsoft::UI::Xaml::Input;
namespace MUXM = winrt::Microsoft::UI::Xaml::Media;
namespace WF = winrt::Windows::Foundation;
namespace WG = winrt::Windows::Globalization;
namespace WGDT = winrt::Windows::Globalization::DateTimeFormatting;
namespace WGNF = winrt::Windows::Globalization::NumberFormatting;
namespace WS = winrt::Windows::System;

// The control is the TimePicker of WinUI, which shows the hours, the minutes
// and, with a 12-hour clock, the half of the day, and opens the list of each
// when it is pressed. A TimePicker has no seconds and a wxTimePickerCtrl
// does (SetTime() and GetTime() take them, and the control of wxMSW lets
// them be changed): they are in a NumberBox beside it.

namespace
{

// Sizes, in DIPs. A TimePicker is 242 wide at least in its style, for three
// columns with room to spare: too much beside the controls of a wx layout,
// which was made for the narrow control of Win32. It takes the room it is
// given up to the 456 of its style.
enum
{
    wxWINUI_TIME_MIN_24H = 120,     // two columns
    wxWINUI_TIME_MIN_12H = 180,     // three columns
    wxWINUI_TIME_MAX = 456,
    wxWINUI_TIME_GAP = 8,
    // What a NumberBox needs around its number with its two buttons in line
    // (11 before the text, 32 for the button which empties it, 76 for the
    // two others, as wxSpinCtrl counts) and two digits.
    wxWINUI_TIME_SECONDS = 11 + 32 + 76 + 17,
    wxWINUI_TIME_HEIGHT = 32
};

#ifdef WXWINUI_TEST_SUPPORT
winrt::hstring gs_winuiTimeLanguageForTesting;
#endif

// The date is not part of the value of the control: always the same one,
// far from any change of daylight saving time.
wxDateTime wxWinUINormalizeTime(const wxDateTime& value)
{
    if ( !value.IsValid() )
        return wxDefaultDateTime;

    const wxDateTime::Tm tm = value.GetTm();
    return wxDateTime(1, wxDateTime::Jan, 2012, tm.hour, tm.min, tm.sec);
}

WF::IReference<WF::TimeSpan> wxWinUIToPeerTime(const wxDateTime::Tm& tm)
{
    const WF::TimeSpan time =
        std::chrono::hours(tm.hour) + std::chrono::minutes(tm.min);
    return winrt::box_value(time).as<WF::IReference<WF::TimeSpan>>();
}

wxDateTime wxWinUIFromPeerTime(const WF::TimeSpan& value, double seconds)
{
    const auto minutes =
        std::chrono::duration_cast<std::chrono::minutes>(value).count();
    if ( minutes < 0 || minutes >= 24 * 60 || !std::isfinite(seconds) )
        return wxDefaultDateTime;

    const int second = static_cast<int>(std::lround(seconds));
    if ( second < 0 || second > 59 )
        return wxDefaultDateTime;

    return wxDateTime(1, wxDateTime::Jan, 2012,
                      static_cast<int>(minutes / 60),
                      static_cast<int>(minutes % 60),
                      second);
}

// The clock of the user, 12 or 24 hours: the one their times are written
// with.
winrt::hstring wxWinUIGetLocaleClock(const winrt::hstring& language)
{
    try
    {
        const WGDT::DateTimeFormatter formatter =
            language.empty()
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
                      std::vector<winrt::hstring>{language});
        return formatter.Clock();
    }
    catch ( const winrt::hresult_error& )
    {
        return {};
    }
}

MUXC::NumberBox wxWinUICreateSecondsBox(const winrt::hstring& language)
{
    MUXC::NumberBox box;
    if ( !language.empty() )
        box.Language(language);
    box.Minimum(0);
    box.Maximum(59);
    box.SmallChange(1);
    box.LargeChange(10);
    box.IsWrapEnabled(true);
    box.AcceptsExpression(false);
    box.ValidationMode(
        MUXC::NumberBoxValidationMode::InvalidInputOverwritten);
    box.SpinButtonPlacementMode(
        MUXC::NumberBoxSpinButtonPlacementMode::Inline);

    // Two digits, as the minutes beside them.
    WGNF::DecimalFormatter formatter =
        language.empty()
            ? WGNF::DecimalFormatter()
            : WGNF::DecimalFormatter(
                  std::vector<winrt::hstring>{language},
                  WG::GeographicRegion().CodeTwoLetter());
    formatter.IntegerDigits(2);
    formatter.FractionDigits(0);
    box.NumberFormatter(formatter);

    box.Width(wxWINUI_TIME_SECONDS);
    box.VerticalAlignment(MUX::VerticalAlignment::Center);
    MUXA::AutomationProperties::SetName(box, wxWinUIToHString(_("Second")));
    return box;
}

MUXC::Button wxWinUIFindTimeButton(const MUX::DependencyObject& root)
{
    if ( !root )
        return nullptr;

    if ( const MUXC::Button button = root.try_as<MUXC::Button>() )
    {
        if ( button.Name() == L"FlyoutButton" )
            return button;
    }

    const int count = MUXM::VisualTreeHelper::GetChildrenCount(root);
    for ( int n = 0; n != count; ++n )
    {
        if ( const MUXC::Button button = wxWinUIFindTimeButton(
                 MUXM::VisualTreeHelper::GetChild(root, n)) )
        {
            return button;
        }
    }
    return nullptr;
}

} // namespace

// What the callbacks of XAML keep, in place of the control: they may run
// after it is gone.
struct wxWinUITimeState
{
    wxTimePickerCtrl *owner = nullptr;

    // The program is writing to the peers: what they report then is not a
    // change made by the user.
    int writing = 0;
};

namespace
{

class wxWinUITimeWriting
{
public:
    explicit wxWinUITimeWriting(std::shared_ptr<wxWinUITimeState> state)
        : m_state(std::move(state))
    {
        ++m_state->writing;
    }

    ~wxWinUITimeWriting()
    {
        --m_state->writing;
    }

    wxWinUITimeWriting(const wxWinUITimeWriting&) = delete;
    wxWinUITimeWriting& operator=(const wxWinUITimeWriting&) = delete;

private:
    std::shared_ptr<wxWinUITimeState> m_state;
};

} // namespace

class wxWinUITimePickerImpl
{
public:
    ~wxWinUITimePickerImpl()
    {
        Close();
    }

    void Close()
    {
        if ( state )
            state->owner = nullptr;

        try
        {
            if ( picker )
            {
                if ( timeToken.value )
                    picker.SelectedTimeChanged(timeToken);
                if ( loadedToken.value )
                    picker.Loaded(loadedToken);
                if ( pickerKeyToken.value )
                    picker.PreviewKeyDown(pickerKeyToken);
            }
            if ( seconds )
            {
                if ( secondsToken.value )
                    seconds.ValueChanged(secondsToken);
                if ( secondsKeyToken.value )
                    seconds.PreviewKeyDown(secondsKeyToken);
            }
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException("WinUI TimePicker teardown", e);
        }
        timeToken = {};
        loadedToken = {};
        pickerKeyToken = {};
        secondsToken = {};
        secondsKeyToken = {};

        host.Close();
        root = nullptr;
        seconds = nullptr;
        picker = nullptr;
        state.reset();
    }

    wxWinUIControlHost host;
    std::shared_ptr<wxWinUITimeState> state;
    MUXC::Grid root{ nullptr };
    MUXC::TimePicker picker{ nullptr };
    MUXC::NumberBox seconds{ nullptr };
    winrt::event_token timeToken{};
    winrt::event_token loadedToken{};
    winrt::event_token pickerKeyToken{};
    winrt::event_token secondsToken{};
    winrt::event_token secondsKeyToken{};
    bool twelveHour = false;
};

wxIMPLEMENT_DYNAMIC_CLASS(wxTimePickerCtrl, wxControl);

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

    m_value = wxWinUINormalizeTime(dt.IsValid() ? dt : wxDateTime::Now());
    if ( !m_value.IsValid() )
        return false;

    m_winui.reset(new wxWinUITimePickerImpl);
    const std::shared_ptr<wxWinUITimeState> state =
        std::make_shared<wxWinUITimeState>();
    state->owner = this;
    m_winui->state = state;
    if ( !m_winui->host.Initialize(this) )
    {
        m_winui->Close();
        return false;
    }

    try
    {
        winrt::hstring language;
#ifdef WXWINUI_TEST_SUPPORT
        language = gs_winuiTimeLanguageForTesting;
#endif

        const MUXC::TimePicker picker;
        if ( !language.empty() )
            picker.Language(language);
        const winrt::hstring clock = wxWinUIGetLocaleClock(language);
        if ( !clock.empty() )
            picker.ClockIdentifier(clock);
        m_winui->twelveHour = picker.ClockIdentifier() == L"12HourClock";
        picker.HorizontalAlignment(MUX::HorizontalAlignment::Stretch);
        picker.VerticalAlignment(MUX::VerticalAlignment::Center);
        // The style of a TimePicker makes it 30 high, beside a field of 32
        // here and among the controls of a form, which are 32: two more
        // above its text, which is then on the line of the text of a field.
        picker.Resources().Insert(
            winrt::box_value(L"TimePickerHostPadding"),
            winrt::box_value(MUX::Thickness{0, 5, 0, 6}));
        MUXA::AutomationProperties::SetName(
            picker, wxWinUIToHString(_("Time")));
        m_winui->picker = picker;

        const MUXC::NumberBox seconds = wxWinUICreateSecondsBox(language);
        m_winui->seconds = seconds;

        const MUXC::Grid root;
        if ( !language.empty() )
            root.Language(language);
        MUXC::ColumnDefinition pickerColumn;
        pickerColumn.Width(
            MUX::GridLengthHelper::FromValueAndType(1, MUX::GridUnitType::Star));
        pickerColumn.MaxWidth(wxWINUI_TIME_MAX);
        MUXC::ColumnDefinition secondsColumn;
        secondsColumn.Width(MUX::GridLengthHelper::Auto());
        root.ColumnDefinitions().Append(pickerColumn);
        root.ColumnDefinitions().Append(secondsColumn);
        root.ColumnSpacing(wxWINUI_TIME_GAP);
        MUXC::Grid::SetColumn(seconds, 1);
        root.Children().Append(picker);
        root.Children().Append(seconds);
        m_winui->root = root;

        // The least width of the button of a TimePicker is a fixed resource
        // of its template: it is set on the button itself, once there is
        // one.
        const double minWidth = m_winui->twelveHour ? wxWINUI_TIME_MIN_12H
                                                    : wxWINUI_TIME_MIN_24H;
        m_winui->loadedToken = picker.Loaded(
            [minWidth](WF::IInspectable const& sender,
                       MUX::RoutedEventArgs const&)
            {
                if ( const MUXC::Button button = wxWinUIFindTimeButton(
                         sender.try_as<MUX::DependencyObject>()) )
                {
                    button.MinWidth(minWidth);
                }
            });

        m_winui->timeToken = picker.SelectedTimeChanged(
            [state](MUXC::TimePicker const&,
                    MUXC::TimePickerSelectedValueChangedEventArgs const&)
            {
                if ( state->owner && !state->writing )
                    state->owner->OnPeerTimeChanged();
            });
        m_winui->secondsToken = seconds.ValueChanged(
            [state](MUXC::NumberBox const&,
                    MUXC::NumberBoxValueChangedEventArgs const&)
            {
                if ( state->owner && !state->writing )
                    state->owner->OnPeerTimeChanged();
            });

        // The arrows go from one part to the other, as between the fields
        // of the control of Win32.
        const auto weakRoot = winrt::make_weak(root);
        const auto weakPicker = winrt::make_weak(picker);
        const auto weakSeconds = winrt::make_weak(seconds);
        const auto moveFocus =
            [weakRoot, weakPicker, weakSeconds](
                WF::IInspectable const& sender,
                MUXI::KeyRoutedEventArgs const& args)
            {
                const WS::VirtualKey key = args.Key();
                if ( key != WS::VirtualKey::Left &&
                     key != WS::VirtualKey::Right )
                {
                    return;
                }

                const MUXC::Grid liveRoot = weakRoot.get();
                const MUXC::TimePicker livePicker = weakPicker.get();
                const MUXC::NumberBox liveSeconds = weakSeconds.get();
                if ( !liveRoot || !livePicker || !liveSeconds )
                    return;

                bool forward = key == WS::VirtualKey::Right;
                if ( liveRoot.FlowDirection() ==
                        MUX::FlowDirection::RightToLeft )
                {
                    forward = !forward;
                }

                const bool fromPicker =
                    sender.try_as<MUXC::TimePicker>() != nullptr;
                if ( fromPicker != forward )
                    return;

                const MUXC::Control target = fromPicker
                    ? liveSeconds.as<MUXC::Control>()
                    : livePicker.as<MUXC::Control>();
                if ( target.Focus(MUX::FocusState::Keyboard) )
                    args.Handled(true);
            };
        m_winui->pickerKeyToken = picker.PreviewKeyDown(moveFocus);
        m_winui->secondsKeyToken = seconds.PreviewKeyDown(moveFocus);

        {
            wxWinUITimeWriting writing(state);
            const wxDateTime::Tm tm = m_value.GetTm();
            picker.SelectedTime(wxWinUIToPeerTime(tm));
            seconds.Value(tm.sec);
        }

        // Publishing the content runs code of the program, which may
        // destroy this control.
        const bool published = m_winui->host.SetContent(root, picker);
        if ( state->owner != this )
            return false;
        if ( !published )
        {
            m_winui->Close();
            return false;
        }

        m_winui->host.SetPreferredFocus(picker);
        if ( state->owner != this )
            return false;
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI TimePicker creation", e);
        if ( state->owner == this )
            m_winui->Close();
        return false;
    }

    InvalidateBestSize();
    SetInitialSize(size);
    return state->owner == this;
}

void wxTimePickerCtrl::SetValue(const wxDateTime& dt)
{
    wxCHECK_RET( dt.IsValid(), wxT("invalid time") );

    m_value = wxWinUINormalizeTime(dt);

    // Last: see ApplyToPeer().
    ApplyToPeer();
}

wxDateTime wxTimePickerCtrl::GetValue() const
{
    return m_value;
}

wxSize wxTimePickerCtrl::DoGetBestSize() const
{
    const int picker = m_winui && m_winui->twelveHour ? wxWINUI_TIME_MIN_12H
                                                      : wxWINUI_TIME_MIN_24H;

    // The height is the one the parts ask for when it is more than the 32
    // of a control: a line of text is not a whole number of pixels at every
    // scale of the screen. The width is not measured: the least width of
    // the TimePicker is set once it has its template, and what the layout
    // of the program is made with must not depend on when it is asked.
    int height = wxWINUI_TIME_HEIGHT;
    if ( m_winui && m_winui->root )
    {
        try
        {
            const float infinity = std::numeric_limits<float>::infinity();
            m_winui->root.Measure(WF::Size{infinity, infinity});
            const float desired = m_winui->root.DesiredSize().Height;
            if ( std::isfinite(desired) )
            {
                height = wxMax(height,
                               static_cast<int>(std::ceil(desired)));
            }
        }
        catch ( const winrt::hresult_error& )
        {
        }
    }

    return FromDIP(wxSize(picker + wxWINUI_TIME_GAP + wxWINUI_TIME_SECONDS,
                          height));
}

// Shows the value of the control in its peers. Rendering runs code of the
// program, which may change the value again or destroy this control: nothing
// is done after it, here or in the callers.
void wxTimePickerCtrl::ApplyToPeer()
{
    if ( !m_winui || !m_winui->picker || !m_winui->seconds || !m_winui->state )
        return;

    const std::shared_ptr<wxWinUITimeState> state = m_winui->state;
    try
    {
        wxWinUITimeWriting writing(state);
        const wxDateTime::Tm tm = m_value.GetTm();
        m_winui->picker.SelectedTime(wxWinUIToPeerTime(tm));
        m_winui->seconds.Value(tm.sec);
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI TimePicker state update", e);
    }

    if ( state->owner == this )
        m_winui->host.ForceRender();
}

// The user changed the hours or the minutes in the list of the TimePicker,
// or the seconds in their field.
void wxTimePickerCtrl::OnPeerTimeChanged()
{
    if ( !m_winui || !m_winui->picker || !m_winui->seconds )
        return;

    wxDateTime value;
    try
    {
        if ( const auto time = m_winui->picker.SelectedTime() )
        {
            value = wxWinUIFromPeerTime(time.Value(),
                                        m_winui->seconds.Value());
        }
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI TimePicker state query", e);
    }

    if ( !value.IsValid() )
    {
        // A field left empty: what it showed comes back.
        ApplyToPeer();
        return;
    }

    if ( value == m_value )
        return;

    m_value = value;
    wxDateEvent event(this, value, wxEVT_TIME_CHANGED);
    // The handler may destroy this control: nothing after it.
    HandleWindowEvent(event);
}

#ifdef WXWINUI_TEST_SUPPORT

wxString wxWinUITimePickerTestAccess::SetLanguage(const wxString& language)
{
    const wxString previous =
        wxWinUIFromHString(gs_winuiTimeLanguageForTesting);
    gs_winuiTimeLanguageForTesting = wxWinUIToHString(language);
    return previous;
}

// As the user does: the parts are changed, then the control is told.
bool wxWinUITimePickerTestAccess::SetPeerTime(wxTimePickerCtrl& control,
                                              const wxDateTime& dt)
{
    if ( !control.m_winui || !control.m_winui->picker ||
         !control.m_winui->seconds || !control.m_winui->state ||
         !dt.IsValid() )
    {
        return false;
    }

    const std::shared_ptr<wxWinUITimeState> state = control.m_winui->state;
    try
    {
        {
            wxWinUITimeWriting writing(state);
            const wxDateTime::Tm tm = wxWinUINormalizeTime(dt).GetTm();
            control.m_winui->picker.SelectedTime(wxWinUIToPeerTime(tm));
            control.m_winui->seconds.Value(tm.sec);
        }
        if ( state->owner != &control )
            return false;

        control.OnPeerTimeChanged();
        return true;
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI TimePicker test time mutation", e);
        return false;
    }
}

bool wxWinUITimePickerTestAccess::GetPeerTime(const wxTimePickerCtrl& control,
                                              wxDateTime *value)
{
    if ( !control.m_winui || !control.m_winui->picker ||
         !control.m_winui->seconds || !value )
    {
        return false;
    }

    try
    {
        const auto time = control.m_winui->picker.SelectedTime();
        if ( !time )
            return false;

        *value = wxWinUIFromPeerTime(time.Value(),
                                     control.m_winui->seconds.Value());
        return value->IsValid();
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI TimePicker test state query", e);
        return false;
    }
}

bool wxWinUITimePickerTestAccess::UsesNativePicker(
    const wxTimePickerCtrl& control)
{
    try
    {
        return control.m_winui && control.m_winui->picker &&
               control.m_winui->picker.Visibility() ==
                    MUX::Visibility::Visible;
    }
    catch ( const winrt::hresult_error& )
    {
        return false;
    }
}

#endif // WXWINUI_TEST_SUPPORT

#endif // wxUSE_TIMEPICKCTRL
