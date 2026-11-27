/////////////////////////////////////////////////////////////////////////////
// Name:        src/winui/datectrl.cpp
// Purpose:     wxWinUI wxDatePickerCtrl implementation (DatePicker,
//              CalendarDatePicker)
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
#include "datepresentation.h"

#include <winrt/Microsoft.UI.Xaml.Automation.h>
#include <winrt/Microsoft.UI.Xaml.Input.h>
#include <winrt/Microsoft.UI.Xaml.Media.h>
#include <winrt/Windows.Globalization.h>
#include <winrt/Windows.Globalization.DateTimeFormatting.h>
#include <winrt/Windows.Globalization.NumberFormatting.h>
#include <winrt/Windows.System.h>

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

using namespace wxWinUIDatePresentation;

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


// The control has two faces. With wxDP_DROPDOWN it is the CalendarDatePicker
// of WinUI, a field which opens a calendar. Without it (wxDP_SPIN, and the
// default) it is the DatePicker of WinUI: the day, the month and the year in
// the order of the user, each with its list when the control is pressed.

// Sizes, in DIPs. A DatePicker is 296 wide at least in its style: too much
// beside the controls of a wx layout, which was made for the narrow control
// of Win32. It takes the room it is given up to the 456 of its style.
enum
{
    wxWINUI_DATE_MIN = 232,
    wxWINUI_DATE_MAX = 456,
    wxWINUI_DATE_CLEAR = 60,
    wxWINUI_DATE_GAP = 8,
    wxWINUI_DATE_HEIGHT = 32
};

// The button of a DatePicker, which is what is seen of it.
MUXC::Button wxWinUIFindDateButton(const MUX::DependencyObject& root)
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
        if ( const MUXC::Button button = wxWinUIFindDateButton(
                 MUXM::VisualTreeHelper::GetChild(root, n)) )
        {
            return button;
        }
    }
    return nullptr;
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

        if ( native )
        {
            try
            {
                if ( nativeChangedToken.value )
                    native.SelectedDateChanged(nativeChangedToken);
                if ( nativeLoadedToken.value )
                    native.Loaded(nativeLoadedToken);
            }
            catch ( const winrt::hresult_error& e )
            {
                wxWinUILogException(
                    "WinUI DatePicker callback removal", e);
            }
        }
        nativeChangedToken = {};
        nativeLoadedToken = {};

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
        clear = nullptr;
        native = nullptr;
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
    // Holds the date and its range in the two styles; shown with
    // wxDP_DROPDOWN only.
    MUXC::CalendarDatePicker picker{ nullptr };
    // Without wxDP_DROPDOWN: the DatePicker, with the button which empties
    // it beside it when the control may have no date (wxDP_ALLOWNONE).
    MUXC::Grid spinRoot{ nullptr };
    MUXC::DatePicker native{ nullptr };
    MUXC::Button clear{ nullptr };
    MUX::FrameworkElement layoutRoot{ nullptr };
    MUX::XamlRoot layoutXamlRoot{ nullptr };
    winrt::event_token dateChangedToken{};
    winrt::event_token nativeChangedToken{};
    winrt::event_token nativeLoadedToken{};
    winrt::event_token clearToken{};
    winrt::event_token layoutLoadedToken{};
    std::vector<MUXC::Control> focusParts;
    std::vector<winrt::event_token> focusKeyTokens;
    WF::DateTime defaultMinimum{};
    WF::DateTime defaultMaximum{};
    WF::DateTime civilMinimum{};
    WF::DateTime civilMaximum{};
    // The years of the lists of a DatePicker which was given none: a hundred
    // before today and a hundred after.
    WF::DateTime nativeDefaultMinimum{};
    WF::DateTime nativeDefaultMaximum{};
    winrt::hstring calendarIdentifier;
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
            const MUXC::DatePicker native;
            if ( !language.empty() )
                native.Language(language);
            native.CalendarIdentifier(m_winui->calendarIdentifier);
            native.YearFormat((style & wxDP_SHOWCENTURY)
                                  ? L"{year.full}"
                                  : L"{year.abbreviated}");
            native.HorizontalAlignment(MUX::HorizontalAlignment::Stretch);
            native.VerticalAlignment(MUX::VerticalAlignment::Center);
            // The style of a DatePicker makes it 30 high, among the
            // controls of a form, which are 32: two more above its text,
            // which is then on the line of the text of a field.
            native.Resources().Insert(
                winrt::box_value(L"DatePickerHostPadding"),
                winrt::box_value(MUX::Thickness{0, 5, 0, 6}));
            native.Resources().Insert(
                winrt::box_value(L"DatePickerHostMonthPadding"),
                winrt::box_value(MUX::Thickness{9, 5, 0, 6}));
            MUXA::AutomationProperties::SetName(
                native, wxWinUIToHString(_("Date")));
            m_winui->native = native;
            m_winui->nativeDefaultMinimum = native.MinYear();
            m_winui->nativeDefaultMaximum = native.MaxYear();

            m_winui->spinRoot = MUXC::Grid();
            if ( !language.empty() )
                m_winui->spinRoot.Language(language);
            MUXC::ColumnDefinition nativeColumn;
            nativeColumn.Width(MUX::GridLengthHelper::FromValueAndType(
                1, MUX::GridUnitType::Star));
            nativeColumn.MaxWidth(wxWINUI_DATE_MAX);
            m_winui->spinRoot.ColumnDefinitions().Append(nativeColumn);
            m_winui->spinRoot.Children().Append(native);
            m_winui->focusParts.push_back(native.as<MUXC::Control>());

            // The least width of the button of a DatePicker is a fixed
            // resource of its template: it is set on the button itself,
            // once there is one.
            m_winui->nativeLoadedToken = native.Loaded(
                [](WF::IInspectable const& sender,
                   MUX::RoutedEventArgs const&)
                {
                    if ( const MUXC::Button button = wxWinUIFindDateButton(
                             sender.try_as<MUX::DependencyObject>()) )
                    {
                        button.MinWidth(wxWINUI_DATE_MIN);
                    }
                });

            m_winui->nativeChangedToken = native.SelectedDateChanged(
                [callbackState, generation](
                    MUXC::DatePicker const&,
                    MUXC::DatePickerSelectedValueChangedEventArgs const&)
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

            if ( style & wxDP_ALLOWNONE )
            {
                // A DatePicker without a date shows the names of its parts
                // and has nothing to take a date away: a button beside it.
                MUXC::ColumnDefinition clearColumn;
                clearColumn.Width(MUX::GridLengthHelper::Auto());
                m_winui->spinRoot.ColumnDefinitions().Append(clearColumn);
                m_winui->spinRoot.ColumnSpacing(wxWINUI_DATE_GAP);
                m_winui->clear = MUXC::Button();
                m_winui->clear.Content(
                    winrt::box_value(wxWinUIToHString(_("Clear"))));
                MUXA::AutomationProperties::SetName(
                    m_winui->clear, wxWinUIToHString(_("Clear")));
                m_winui->clear.MinWidth(
                    wxWINUI_DATE_CLEAR - wxWINUI_DATE_GAP);
                m_winui->clear.VerticalAlignment(
                    MUX::VerticalAlignment::Center);
                MUXC::Grid::SetColumn(m_winui->clear, 1);
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
                ? MUXC::Control{ nullptr }
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
        // The CalendarDatePicker shows it. A DatePicker without a date
        // shows the names of its parts, and no other text.
        m_winui->picker.PlaceholderText(wxWinUIToHString(text));
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
    if ( m_winui && m_winui->spinStyle )
    {
        // Not what the DatePicker measures: its least width is set once it
        // has its template, and the size the layout of the program is made
        // with must not depend on when it is asked.
        // (Its height is measured: a line of text is not a whole number of
        // pixels at every scale of the screen.)
        int height = wxWINUI_DATE_HEIGHT;
        if ( m_winui->spinRoot )
        {
            try
            {
                const float infinity = std::numeric_limits<float>::infinity();
                m_winui->spinRoot.Measure(WF::Size{infinity, infinity});
                const float desired = m_winui->spinRoot.DesiredSize().Height;
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

        return wxWindow::FromDIP(
            wxSize(wxWINUI_DATE_MIN +
                        (HasFlag(wxDP_ALLOWNONE) ? wxWINUI_DATE_CLEAR : 0),
                   height),
            const_cast<wxDatePickerCtrl*>(this));
    }

    wxSize size(200, 32);
    const MUX::FrameworkElement root =
        m_winui ? m_winui->picker.as<MUX::FrameworkElement>() : nullptr;
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
    const MUXC::DatePicker native = m_winui->native;

    // The lists of a DatePicker go from one year to another: those of the
    // range of the program or, without one, the hundred years before and
    // after today of the control (all the years the calendar has would make
    // lists of thousands of lines, filled each time the date changes), and
    // always the year of the value. The days of the range are seen to when
    // the user has chosen, in OnPeerDateChanged().
    WF::DateTime nativeMinimum = minimum.IsValid()
        ? peerMinimum : m_winui->nativeDefaultMinimum;
    WF::DateTime nativeMaximum = maximum.IsValid()
        ? peerMaximum : m_winui->nativeDefaultMaximum;
    if ( peerValue )
    {
        const WF::DateTime date = peerValue.Value();
        if ( date < nativeMinimum )
            nativeMinimum = date;
        if ( date > nativeMaximum )
            nativeMaximum = date;
    }
    const auto applyNativeBounds =
        [&native](const WF::DateTime& first, const WF::DateTime& last)
        {
            // Never a first year after the last one on the way.
            if ( first > native.MaxYear() )
            {
                native.MaxYear(last);
                native.MinYear(first);
            }
            else
            {
                native.MinYear(first);
                native.MaxYear(last);
            }
        };
    wxWinUIDatePeerMutationGuard guard(state);

    WF::DateTime previousMinimum{};
    WF::DateTime previousMaximum{};
    WF::IReference<WF::DateTime> previousValue{ nullptr };
    WF::DateTime previousNativeMinimum{};
    WF::DateTime previousNativeMaximum{};
    WF::IReference<WF::DateTime> previousNativeValue{ nullptr };
    bool snapshotCaptured = false;
    bool applied = false;
    try
    {
        previousMinimum = picker.MinDate();
        previousMaximum = picker.MaxDate();
        previousValue = picker.Date();
        if ( native )
        {
            previousNativeMinimum = native.MinYear();
            previousNativeMaximum = native.MaxYear();
            previousNativeValue = native.SelectedDate();
        }
        snapshotCaptured = true;

        wxWinUIApplyDateBounds(picker, peerMinimum, peerMaximum);
        picker.Date(peerValue);
        const WF::IReference<WF::DateTime> actualValue = picker.Date();
        wxDateTime actualCivil;
        const bool pickerApplied =
            wxWinUIReadPeerDate(actualValue, &actualCivil) &&
            wxWinUISameCivilDate(actualCivil, value);
        bool nativeApplied = true;
        if ( native )
        {
            applyNativeBounds(nativeMinimum, nativeMaximum);
            native.SelectedDate(peerValue);
            wxDateTime nativeCivil;
            nativeApplied =
                wxWinUIReadPeerDate(native.SelectedDate(), &nativeCivil) &&
                wxWinUISameCivilDate(nativeCivil, value);
        }
        applied = pickerApplied && nativeApplied;
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
                if ( native )
                {
                    applyNativeBounds(
                        previousNativeMinimum, previousNativeMaximum);
                    native.SelectedDate(previousNativeValue);
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
            if ( const auto ref = m_winui->native.SelectedDate() )
            {
                value = wxWinUIFromDateTime(ref.Value());
                if ( !value.IsValid() )
                {
                    ApplyToPeer();
                    return;
                }
                // The lists of a DatePicker are limited by years only: a
                // day chosen before the first one of the range, or after
                // its last one, becomes that one.
                if ( m_rangeMin.IsValid() && value < m_rangeMin )
                    value = m_rangeMin;
                if ( m_rangeMax.IsValid() && value > m_rangeMax )
                    value = m_rangeMax;
            }
            else if ( HasFlag(wxDP_ALLOWNONE) )
            {
                value = wxDefaultDateTime;
            }
            else
            {
                // No date where one is required: the last one comes back.
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
                if ( !wxWinUIReadPeerDate(
                         control.m_winui->native.SelectedDate(), value) )
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

    // What the DatePicker shows: the parts of its date in its calendar.
    wxWinUICalendarDateParts parts;
    try
    {
        const auto date = control.m_winui->native.SelectedDate();
        if ( !date ||
             !wxWinUIReadCalendarDateParts(
                 control.m_winui->native.CalendarIdentifier(),
                 date.Value(), &parts) )
        {
            return false;
        }
    }
    catch ( const winrt::hresult_error& )
    {
        return false;
    }

    if ( year )
        *year = parts.year;
    if ( month )
        *month = parts.month;
    if ( day )
        *day = parts.day;
    return true;
}

bool wxWinUIDatePickerTestAccess::UsesNativePicker(
    const wxDatePickerCtrl& control)
{
    return control.m_winui && control.m_winui->spinStyle &&
           control.m_winui->native;
}

wxString wxWinUIDatePickerTestAccess::GetPeerYearFormat(
    const wxDatePickerCtrl& control)
{
    if ( !control.m_winui || !control.m_winui->native )
        return wxString();

    try
    {
        return wxWinUIFromHString(control.m_winui->native.YearFormat());
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
        return wxWinUIFromHString(
            control.m_winui->picker.PlaceholderText());
    }
    catch ( const winrt::hresult_error& )
    {
        return wxString();
    }
}
#endif

#endif // wxUSE_DATEPICKCTRL
