/////////////////////////////////////////////////////////////////////////////
// Name:        src/winui/spinctrl.cpp
// Purpose:     wxWinUI wxSpinCtrl / wxSpinCtrlDouble (WinUI NumberBox)
// Author:      wxWidgets development team
// Created:     2026-06-01
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_SPINCTRL

#include "wx/spinctrl.h"

#ifdef WXWINUI_TEST_SUPPORT
    #include "spinctrl-test-access.h"
#endif

#ifndef WX_PRECOMP
    #include "wx/event.h"
#endif

#include "wx/numformatter.h"
#include "wx/private/spinctrl.h"
#include "wx/scopeguard.h"

#include "private.h"

#include <atomic>
#include <cmath>
#include <cstdint>
#include <limits>
#include <memory>

#include <winrt/Windows.Globalization.NumberFormatting.h>
#include <winrt/Windows.System.h>

namespace MUX = winrt::Microsoft::UI::Xaml;
namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;
namespace MUXI = winrt::Microsoft::UI::Xaml::Input;
namespace MUXM = winrt::Microsoft::UI::Xaml::Media;
namespace WGNF = winrt::Windows::Globalization::NumberFormatting;

// Normally defined by the generic wxSpinCtrlDouble implementation, which is
// not built under the WinUI toolkit.
wxIMPLEMENT_DYNAMIC_CLASS(wxSpinDoubleEvent, wxNotifyEvent);

namespace
{

class wxWinUINumberBoxCallbackState
{
public:
    explicit wxWinUINumberBoxCallbackState(wxSpinCtrlBase *owner)
        : m_owner(owner)
    {
    }

    std::uint64_t Generation() const
    {
        return m_generation.load(std::memory_order_acquire);
    }

    template <typename T>
    T *GetOwner(std::uint64_t generation) const
    {
        if ( generation != m_generation.load(std::memory_order_acquire) )
            return nullptr;
        return static_cast<T *>(m_owner.load(std::memory_order_acquire));
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
                     "unbalanced WinUI NumberBox peer mutation");
    }

    bool IsPeerMutationInProgress() const
    {
        return m_peerMutationDepth.load(std::memory_order_acquire) != 0;
    }

private:
    std::atomic<wxSpinCtrlBase *> m_owner;
    std::atomic<std::uint64_t> m_generation{1};
    std::atomic<unsigned> m_peerMutationDepth{0};
};

class wxWinUINumberBoxMutationGuard
{
public:
    explicit wxWinUINumberBoxMutationGuard(
        std::shared_ptr<wxWinUINumberBoxCallbackState> state)
        : m_state(std::move(state))
    {
        if ( m_state )
            m_state->BeginPeerMutation();
    }

    ~wxWinUINumberBoxMutationGuard()
    {
        if ( m_state )
            m_state->EndPeerMutation();
    }

    wxWinUINumberBoxMutationGuard(
        const wxWinUINumberBoxMutationGuard&) = delete;
    wxWinUINumberBoxMutationGuard& operator=(
        const wxWinUINumberBoxMutationGuard&) = delete;

private:
    std::shared_ptr<wxWinUINumberBoxCallbackState> m_state;
};

MUXC::TextBox wxWinUIFindNumberBoxTextBox(
    const MUX::DependencyObject& root)
{
    if ( !root )
        return nullptr;

    if ( const auto textBox = root.try_as<MUXC::TextBox>() )
        return textBox;

    const int count = MUXM::VisualTreeHelper::GetChildrenCount(root);
    for ( int n = 0; n != count; ++n )
    {
        if ( const auto textBox = wxWinUIFindNumberBoxTextBox(
                 MUXM::VisualTreeHelper::GetChild(root, n)) )
        {
            return textBox;
        }
    }
    return nullptr;
}

MUXC::NumberBox wxWinUICreateNumberBox()
{
    MUXC::NumberBox box;
    box.SpinButtonPlacementMode(
        MUXC::NumberBoxSpinButtonPlacementMode::Inline);
    box.ValidationMode(
        MUXC::NumberBoxValidationMode::InvalidInputOverwritten);
    box.AcceptsExpression(false);
    return box;
}

void wxWinUIApplyDigits(const MUXC::NumberBox& box, unsigned digits)
{
    using namespace winrt::Windows::Globalization::NumberFormatting;

    DecimalFormatter formatter;
    formatter.IntegerDigits(1);
    formatter.FractionDigits(static_cast<int32_t>(digits));

    IncrementNumberRounder rounder;
    rounder.Increment(std::pow(10.0, -static_cast<double>(digits)));
    formatter.NumberRounder(rounder);

    box.NumberFormatter(formatter);
}

class wxWinUIHexNumberFormatter
    : public winrt::implements<
          wxWinUIHexNumberFormatter,
          WGNF::INumberFormatter2,
          WGNF::INumberParser>
{
public:
    explicit wxWinUIHexNumberFormatter(int maximum)
        : m_maximum(maximum)
    {
    }

    winrt::hstring FormatInt(std::int64_t value) const
    {
        if ( value < 0 )
            return winrt::hstring();
        return FormatUInt(static_cast<std::uint64_t>(value));
    }

    winrt::hstring FormatUInt(std::uint64_t value) const
    {
        if ( value <=
             static_cast<std::uint64_t>(
                 std::numeric_limits<long>::max()) )
        {
            return wxWinUIToHString(
                wxSpinCtrlImpl::FormatAsHex(
                    static_cast<long>(value), m_maximum));
        }
        return wxWinUIToHString(
            wxString::Format("0x%llX",
                             static_cast<unsigned long long>(value)));
    }

    winrt::hstring FormatDouble(double value) const
    {
        if ( !std::isfinite(value) || value < 0 ||
             value >
                 static_cast<double>(
                     std::numeric_limits<std::uint64_t>::max()) )
        {
            return winrt::hstring();
        }
        return FormatUInt(
            static_cast<std::uint64_t>(std::llround(value)));
    }

    winrt::Windows::Foundation::IReference<std::int64_t>
    ParseInt(const winrt::hstring& text) const
    {
        std::uint64_t value;
        if ( !Parse(text, &value) ||
             value >
                 static_cast<std::uint64_t>(
                     std::numeric_limits<std::int64_t>::max()) )
        {
            return nullptr;
        }
        return winrt::box_value(
                   static_cast<std::int64_t>(value)).
            as<winrt::Windows::Foundation::IReference<std::int64_t>>();
    }

    winrt::Windows::Foundation::IReference<std::uint64_t>
    ParseUInt(const winrt::hstring& text) const
    {
        std::uint64_t value;
        if ( !Parse(text, &value) )
            return nullptr;
        return winrt::box_value(value).
            as<winrt::Windows::Foundation::IReference<std::uint64_t>>();
    }

    winrt::Windows::Foundation::IReference<double>
    ParseDouble(const winrt::hstring& text) const
    {
        std::uint64_t value;
        if ( !Parse(text, &value) )
            return nullptr;
        return winrt::box_value(static_cast<double>(value)).
            as<winrt::Windows::Foundation::IReference<double>>();
    }

private:
    static bool Parse(const winrt::hstring& input,
                      std::uint64_t *value)
    {
        wxString text = wxWinUIFromHString(input);
        text.Trim(true).Trim(false);
        if ( text.length() >= 2 && text[0] == '0' &&
             (text[1] == 'x' || text[1] == 'X') )
        {
            text.erase(0, 2);
        }
        if ( text.empty() )
            return false;

        unsigned long long parsed;
        if ( !text.ToULongLong(&parsed, 16) )
            return false;
        if ( value )
            *value = static_cast<std::uint64_t>(parsed);
        return true;
    }

    int m_maximum;
};

void wxWinUIApplyIntegerFormatter(const MUXC::NumberBox& box,
                                  int base,
                                  int maximum)
{
    if ( base == 16 )
    {
        box.NumberFormatter(
            winrt::make<wxWinUIHexNumberFormatter>(
                wxMax(0, maximum)));
    }
    else
    {
        wxWinUIApplyDigits(box, 0);
    }
}

void wxWinUIApplyNumberBoxRange(const MUXC::NumberBox& box,
                                double minimum,
                                double maximum)
{
    // NumberBox rejects a transient Minimum > Maximum. Pick the first write
    // from the current interval so even disjoint range changes are atomic from
    // the peer's point of view.
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

template <typename T>
T wxWinUIClampToRange(T value, T minValue, T maxValue)
{
    const T low = wxMin(minValue, maxValue);
    const T high = wxMax(minValue, maxValue);
    return wxMin(wxMax(value, low), high);
}

} // anonymous namespace

class wxWinUINumberBoxImpl
{
public:
    ~wxWinUINumberBoxImpl()
    {
        Close();
    }

    void Close()
    {
        if ( callbackState )
            callbackState->Invalidate();

        DetachTextPart();

        if ( box && valueChangedToken.value )
        {
            try
            {
                box.ValueChanged(valueChangedToken);
            }
            catch ( const winrt::hresult_error& e )
            {
                wxWinUILogException(
                    "WinUI NumberBox ValueChanged removal", e);
            }
        }
        valueChangedToken = {};

        if ( box && loadedToken.value )
        {
            try
            {
                box.Loaded(loadedToken);
            }
            catch ( const winrt::hresult_error& e )
            {
                wxWinUILogException(
                    "WinUI NumberBox Loaded removal", e);
            }
        }
        loadedToken = {};

        if ( box && layoutUpdatedToken.value )
        {
            try
            {
                box.LayoutUpdated(layoutUpdatedToken);
            }
            catch ( const winrt::hresult_error& e )
            {
                wxWinUILogException(
                    "WinUI NumberBox LayoutUpdated removal", e);
            }
        }
        layoutUpdatedToken = {};

        if ( box && keyDownToken.value )
        {
            try
            {
                box.PreviewKeyDown(keyDownToken);
            }
            catch ( const winrt::hresult_error& e )
            {
                wxWinUILogException(
                    "WinUI NumberBox PreviewKeyDown removal", e);
            }
        }
        keyDownToken = {};

        host.Close();
        box = nullptr;
        callbackState.reset();
        resolvingTextPart = false;
        pendingTextPartResolve = false;
        restoreSelectionOnNextTextPart = false;
    }

    void DetachTextPart()
    {
        const MUXC::TextBox oldTextBox = textBox;
        const winrt::event_token oldTextToken = textChangedToken;
        const winrt::event_token oldSelectionToken =
            selectionChangedToken;
        textBox = nullptr;
        textChangedToken = {};
        selectionChangedToken = {};
        if ( ++textPartGeneration == 0 )
            ++textPartGeneration;

        if ( !oldTextBox )
            return;

        // A template replacement must not discard the logical selection just
        // because its old TextBox part is about to disappear.
        restoreSelectionOnNextTextPart = true;

        if ( oldTextToken.value )
        {
            try
            {
                oldTextBox.TextChanged(oldTextToken);
            }
            catch ( const winrt::hresult_error& e )
            {
                wxWinUILogException(
                    "WinUI NumberBox TextChanged removal", e);
            }
        }
        if ( oldSelectionToken.value )
        {
            try
            {
                oldTextBox.SelectionChanged(oldSelectionToken);
            }
            catch ( const winrt::hresult_error& e )
            {
                wxWinUILogException(
                    "WinUI NumberBox SelectionChanged removal", e);
            }
        }
    }

    template <typename Owner>
    bool Configure(Owner *owner)
    {
        const std::shared_ptr<wxWinUINumberBoxCallbackState> state =
            callbackState;
        if ( !state )
            return false;

        const std::uint64_t generation = state->Generation();
        wxWinUINumberBoxImpl * const createImpl = this;
        const auto getCurrentOwner =
            [state, generation, createImpl]() -> Owner *
            {
                Owner * const liveOwner =
                    state->GetOwner<Owner>(generation);
                if ( !liveOwner || !liveOwner->m_winui ||
                     liveOwner->m_winui.get() != createImpl ||
                     liveOwner->m_winui->callbackState != state )
                {
                    return nullptr;
                }
                return liveOwner;
            };

        if ( getCurrentOwner() != owner )
            return false;

        box = wxWinUICreateNumberBox();
        const MUXC::NumberBox createPeer = box;

        valueChangedToken = box.ValueChanged(
            [state, generation](
                MUXC::NumberBox const&,
                MUXC::NumberBoxValueChangedEventArgs const& event)
            {
                Owner * const liveOwner =
                    state->GetOwner<Owner>(generation);
                if ( !liveOwner ||
                     state->IsPeerMutationInProgress() )
                {
                    return;
                }
                liveOwner->OnPeerValueChanged(event.NewValue());
            });

        loadedToken = box.Loaded(
            [state, generation](
                winrt::Windows::Foundation::IInspectable const&,
                MUX::RoutedEventArgs const&)
            {
                if ( Owner * const liveOwner =
                         state->GetOwner<Owner>(generation) )
                {
                    liveOwner->ResolveTextPart(false);
                }
            });

        // Theme/template replacement doesn't necessarily raise Loaded again.
        // Re-resolve by identity on bounded layout edges and revoke the old
        // template-part delegates before attaching the new ones.
        layoutUpdatedToken = box.LayoutUpdated(
            [state, generation](
                winrt::Windows::Foundation::IInspectable const&,
                winrt::Windows::Foundation::IInspectable const&)
            {
                if ( Owner * const liveOwner =
                         state->GetOwner<Owner>(generation) )
                {
                    liveOwner->ResolveTextPart(false);
                }
            });

        keyDownToken = box.PreviewKeyDown(
            [state, generation](
                winrt::Windows::Foundation::IInspectable const&,
                MUXI::KeyRoutedEventArgs const& event)
            {
                Owner * const liveOwner =
                    state->GetOwner<Owner>(generation);
                if ( !liveOwner || !liveOwner->IsEnabled() )
                    return;

                const auto key = event.Key();
                int direction = 0;
                if ( liveOwner->HasFlag(wxSP_ARROW_KEYS) )
                {
                    if ( key == winrt::Windows::System::VirtualKey::Up )
                        direction = +1;
                    else if ( key ==
                              winrt::Windows::System::VirtualKey::Down )
                        direction = -1;
                }

                if ( direction )
                {
                    event.Handled(true);
                    liveOwner->OnPeerValueChanged(
                        liveOwner->ValueAfterStep(direction));
                    return;
                }

                if ( key == winrt::Windows::System::VirtualKey::Enter &&
                     liveOwner->HasFlag(wxTE_PROCESS_ENTER) )
                {
                    event.Handled(true);
                    liveOwner->OnPeerEnter();
                }
            });

        Owner *liveOwner = getCurrentOwner();
        if ( !liveOwner )
            return false;

        if ( !liveOwner->ApplyToPeer() )
            return false;
        liveOwner = getCurrentOwner();
        if ( !liveOwner ||
             liveOwner->m_winui->box != createPeer )
        {
            return false;
        }

        const bool contentSet =
            liveOwner->m_winui->host.SetContent(createPeer);

        // SetContent() can synchronously realize NumberBox and enter its
        // Loaded callback (or application code reached from the shared
        // host). Never resume through `this`: destruction retires both the
        // implementation and its owner generation while the strong peer and
        // callback state above remain valid locals.
        liveOwner = getCurrentOwner();
        if ( !liveOwner ||
             liveOwner->m_winui->box != createPeer )
        {
            return false;
        }
        if ( !contentSet )
            return false;

        liveOwner->m_winui->host.SetPreferredFocus(createPeer);
        liveOwner = getCurrentOwner();
        if ( !liveOwner ||
             liveOwner->m_winui->box != createPeer )
        {
            return false;
        }

        liveOwner->ResolveTextPart();
        liveOwner = getCurrentOwner();
        return liveOwner &&
               liveOwner->m_winui->box == createPeer;
    }

    template <typename Owner>
    void ResolveTextPart(Owner *expectedOwner, bool updateLayout)
    {
        if ( !box || !callbackState )
            return;

        if ( resolvingTextPart )
        {
            pendingTextPartResolve = true;
            return;
        }

        resolvingTextPart = true;
        wxWinUINumberBoxImpl * const impl = this;
        const std::shared_ptr<wxWinUINumberBoxCallbackState> state =
            callbackState;
        const std::uint64_t ownerGeneration = state->Generation();
        wxScopeGuard resolvingGuard = wxMakeGuard(
            [state, ownerGeneration, impl]()
            {
                Owner * const owner =
                    state->GetOwner<Owner>(ownerGeneration);
                if ( owner && owner->m_winui &&
                     owner->m_winui.get() == impl )
                {
                    impl->resolvingTextPart = false;
                }
            });
        wxUnusedVar(resolvingGuard);

        for ( unsigned pass = 0; pass != 2; ++pass )
        {
            Owner *owner =
                state->GetOwner<Owner>(ownerGeneration);
            if ( owner != expectedOwner || !owner->m_winui ||
                 owner->m_winui.get() != impl )
            {
                break;
            }

            pendingTextPartResolve = false;
            try
            {
                if ( updateLayout && pass == 0 )
                {
                    box.ApplyTemplate();
                    box.UpdateLayout();
                }

                owner = state->GetOwner<Owner>(ownerGeneration);
                if ( owner != expectedOwner || !owner->m_winui ||
                     owner->m_winui.get() != impl )
                {
                    break;
                }

                const MUXC::TextBox candidate =
                    wxWinUIFindNumberBoxTextBox(box);
                if ( candidate != textBox )
                {
                    DetachTextPart();
                    const std::uint64_t partGeneration =
                        textPartGeneration;
                    winrt::event_token newTextToken{};
                    winrt::event_token newSelectionToken{};
                    bool tokensAdopted = false;
                    wxScopeGuard tokenGuard = wxMakeGuard(
                        [&candidate, &newTextToken,
                         &newSelectionToken, &tokensAdopted]()
                        {
                            if ( tokensAdopted || !candidate )
                                return;
                            try
                            {
                                if ( newTextToken.value )
                                    candidate.TextChanged(
                                        newTextToken);
                                if ( newSelectionToken.value )
                                    candidate.SelectionChanged(
                                        newSelectionToken);
                            }
                            catch ( const winrt::hresult_error& )
                            {
                            }
                        });
                    wxUnusedVar(tokenGuard);

                    if ( candidate )
                    {
                        newTextToken = candidate.TextChanged(
                            [state, ownerGeneration, partGeneration](
                                winrt::Windows::Foundation::
                                    IInspectable const& sender,
                                MUXC::TextChangedEventArgs const&)
                            {
                                Owner * const liveOwner =
                                    state->GetOwner<Owner>(
                                        ownerGeneration);
                                if ( !liveOwner ||
                                     !liveOwner->m_winui ||
                                     liveOwner->m_winui->
                                         callbackState != state ||
                                     liveOwner->m_winui->
                                         textPartGeneration !=
                                             partGeneration ||
                                     state->
                                         IsPeerMutationInProgress() )
                                {
                                    return;
                                }

                                try
                                {
                                    liveOwner->OnPeerTextChanged(
                                        wxWinUIFromHString(
                                            sender.as<MUXC::TextBox>().
                                                Text()));
                                }
                                catch ( const winrt::hresult_error& )
                                {
                                }
                            });

                        newSelectionToken =
                            candidate.SelectionChanged(
                                [state, ownerGeneration,
                                 partGeneration](
                                    winrt::Windows::Foundation::
                                        IInspectable const& sender,
                                    MUX::RoutedEventArgs const&)
                                {
                                    Owner * const liveOwner =
                                        state->GetOwner<Owner>(
                                            ownerGeneration);
                                    if ( !liveOwner ||
                                         !liveOwner->m_winui ||
                                         liveOwner->m_winui->
                                             callbackState != state ||
                                         liveOwner->m_winui->
                                             textPartGeneration !=
                                                 partGeneration ||
                                         state->
                                             IsPeerMutationInProgress() ||
                                         liveOwner->
                                             m_hasPendingSelection )
                                     {
                                         return;
                                     }

                                    try
                                    {
                                        const MUXC::TextBox edit =
                                            sender.as<MUXC::TextBox>();
                                        liveOwner->m_selectionFrom =
                                            edit.SelectionStart();
                                        liveOwner->m_selectionTo =
                                            liveOwner->
                                                m_selectionFrom +
                                            edit.SelectionLength();
                                    }
                                    catch ( const winrt::hresult_error& )
                                    {
                                    }
                                });
                    }

                    owner = state->GetOwner<Owner>(ownerGeneration);
                    if ( owner != expectedOwner || !owner ||
                         !owner->m_winui ||
                         owner->m_winui.get() != impl ||
                         textPartGeneration != partGeneration )
                    {
                        break;
                    }

                    textBox = candidate;
                    textChangedToken = newTextToken;
                    selectionChangedToken = newSelectionToken;
                    tokensAdopted = true;
                    if ( candidate &&
                         restoreSelectionOnNextTextPart )
                    {
                        owner->m_hasPendingSelection = true;
                        restoreSelectionOnNextTextPart = false;
                    }
                }

                owner = state->GetOwner<Owner>(ownerGeneration);
                if ( owner != expectedOwner || !owner ||
                     !owner->m_winui ||
                     owner->m_winui.get() != impl )
                {
                    break;
                }

                if ( textBox )
                {
                    host.SetPreferredFocus(textBox);

                    if ( owner->m_hasPendingSelection )
                    {
                        const long length =
                            static_cast<long>(
                                owner->m_textValue.length());
                        long from = owner->m_selectionFrom;
                        long to = owner->m_selectionTo;
                        if ( from == -1 && to == -1 )
                        {
                            from = 0;
                            to = length;
                        }
                        from = wxClip(from, 0L, length);
                        to = wxClip(to, 0L, length);
                        textBox.Select(
                            static_cast<int32_t>(wxMin(from, to)),
                            static_cast<int32_t>(
                                std::abs(to - from)));
                        owner->m_selectionFrom = from;
                        owner->m_selectionTo = to;
                        owner->m_hasPendingSelection = false;
                    }
                }
            }
            catch ( const winrt::hresult_error& e )
            {
                wxWinUILogException(
                    "WinUI NumberBox text-part resolution", e);
            }

            owner = state->GetOwner<Owner>(ownerGeneration);
            if ( owner != expectedOwner || !owner ||
                 !owner->m_winui ||
                 owner->m_winui.get() != impl ||
                 !pendingTextPartResolve )
            {
                break;
            }
        }
    }

    wxWinUIControlHost host;
    std::shared_ptr<wxWinUINumberBoxCallbackState> callbackState;
    MUXC::NumberBox box{ nullptr };
    MUXC::TextBox textBox{ nullptr };
    winrt::event_token valueChangedToken{};
    winrt::event_token loadedToken{};
    winrt::event_token layoutUpdatedToken{};
    winrt::event_token keyDownToken{};
    winrt::event_token textChangedToken{};
    winrt::event_token selectionChangedToken{};
    std::uint64_t textPartGeneration = 1;
    bool resolvingTextPart = false;
    bool pendingTextPartResolve = false;
    bool restoreSelectionOnNextTextPart = false;
};

//-----------------------------------------------------------------------------
// wxSpinCtrl
//-----------------------------------------------------------------------------

wxSpinCtrl::wxSpinCtrl()
{
}

wxSpinCtrl::wxSpinCtrl(wxWindow *parent,
                       wxWindowID id,
                       const wxString& value,
                       const wxPoint& pos,
                       const wxSize& size,
                       long style,
                       int min,
                       int max,
                       int initial,
                       const wxString& name)
{
    Create(parent, id, value, pos, size, style,
           min, max, initial, name);
}

wxSpinCtrl::~wxSpinCtrl()
{
    // m_winui is declared before the scalar/string model and would otherwise
    // be destroyed after it. Revoke callbacks before any model member ends
    // its lifetime.
    if ( m_winui )
        m_winui->Close();
    m_winui.reset();
}

bool wxSpinCtrl::Create(wxWindow *parent,
                        wxWindowID id,
                        const wxString& value,
                        const wxPoint& pos,
                        const wxSize& size,
                        long style,
                        int min,
                        int max,
                        int initial,
                        const wxString& name)
{
    style = (style & ~wxBORDER_MASK) | wxBORDER_NONE;
    if ( !wxControl::Create(parent, id, pos, size, style,
                            wxDefaultValidator, name) )
    {
        return false;
    }

    m_min = min;
    m_max = max;
    m_increment = 1;
    m_numBase = 10;
    m_snapToTicks = false;
    m_value = AdjustValue(initial);
    m_textValue = FormatValue(m_value);
    bool selectInitialText = false;

    if ( !value.empty() )
    {
        int parsed;
        if ( ParseText(value, &parsed) &&
             parsed >= wxMin(m_min, m_max) &&
             parsed <= wxMax(m_min, m_max) )
        {
            m_value = AdjustValue(parsed);
            m_textValue = FormatValue(m_value);
        }
        else
        {
            m_value = AdjustValue(m_min);
            m_textValue = value;
            selectInitialText = true;
        }
    }
    m_selectionFrom = selectInitialText
                          ? 0
                          : static_cast<long>(m_textValue.length());
    m_selectionTo = static_cast<long>(m_textValue.length());
    m_hasPendingSelection = selectInitialText;

    m_winui = std::make_unique<wxWinUINumberBoxImpl>();
    m_winui->callbackState =
        std::make_shared<wxWinUINumberBoxCallbackState>(this);
    wxWinUINumberBoxImpl * const createImpl = m_winui.get();
    const std::shared_ptr<wxWinUINumberBoxCallbackState> createState =
        m_winui->callbackState;
    const std::uint64_t createGeneration =
        createState->Generation();
    if ( !m_winui->host.Initialize(this) )
    {
        m_winui->Close();
        return false;
    }

    try
    {
        const bool configured = createImpl->Configure(this);
        wxSpinCtrl * const liveOwner =
            createState->GetOwner<wxSpinCtrl>(createGeneration);
        if ( !liveOwner || !liveOwner->m_winui ||
             liveOwner->m_winui.get() != createImpl ||
             liveOwner->m_winui->callbackState != createState )
        {
            return false;
        }

        if ( !configured )
        {
            liveOwner->m_winui->Close();
            return false;
        }
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI integer NumberBox creation", e);
        wxSpinCtrl * const liveOwner =
            createState->GetOwner<wxSpinCtrl>(createGeneration);
        if ( liveOwner && liveOwner->m_winui &&
             liveOwner->m_winui.get() == createImpl &&
             liveOwner->m_winui->callbackState == createState )
        {
            liveOwner->m_winui->Close();
        }
        return false;
    }

    wxSpinCtrl *liveOwner =
        createState->GetOwner<wxSpinCtrl>(createGeneration);
    if ( !liveOwner || !liveOwner->m_winui ||
         liveOwner->m_winui.get() != createImpl ||
         liveOwner->m_winui->callbackState != createState )
    {
        return false;
    }

    liveOwner->SetInitialSize(size);
    liveOwner =
        createState->GetOwner<wxSpinCtrl>(createGeneration);
    return liveOwner && liveOwner->m_winui &&
           liveOwner->m_winui.get() == createImpl &&
           liveOwner->m_winui->callbackState == createState;
}

void wxSpinCtrl::SetValue(int value)
{
    const int oldValue = m_value;
    const wxString oldText = m_textValue;
    m_value = AdjustValue(value);
    m_textValue = FormatValue(m_value);
    ApplyProgrammaticChange(
        m_value != oldValue || m_textValue != oldText);
}

void wxSpinCtrl::SetValue(const wxString& value)
{
    int parsed;
    if ( ParseText(value, &parsed) &&
         parsed >= wxMin(m_min, m_max) &&
         parsed <= wxMax(m_min, m_max) )
    {
        SetValue(parsed);
        return;
    }

    m_value = AdjustValue(m_min);
    m_textValue = value;
    ApplyProgrammaticChange(true);
}

void wxSpinCtrl::SetRange(int minValue, int maxValue)
{
    if ( m_numBase == 16 && (minValue < 0 || maxValue < 0) )
        return;

    int parsedText;
    const bool hadNumericText =
        ParseText(m_textValue, &parsedText);
    const int oldValue = m_value;
    const wxString oldText = m_textValue;
    m_min = minValue;
    m_max = maxValue;
    m_value = AdjustValue(m_value);
    if ( m_value != oldValue || hadNumericText )
        m_textValue = FormatValue(m_value);
    ApplyProgrammaticChange(
        m_value != oldValue || m_textValue != oldText);
    InvalidateBestSize();
}

void wxSpinCtrl::SetIncrement(int increment)
{
    wxCHECK_RET(increment > 0, "spin control increment must be positive");
    const int oldValue = m_value;
    const wxString oldText = m_textValue;
    m_increment = increment;
    if ( m_snapToTicks )
    {
        m_value = AdjustValue(m_value);
        m_textValue = FormatValue(m_value);
    }
    ApplyProgrammaticChange(
        m_value != oldValue || m_textValue != oldText);
}

wxString wxSpinCtrl::GetTextValue() const
{
    return m_textValue;
}

void wxSpinCtrl::SetSnapToTicks(bool snap)
{
    if ( snap == m_snapToTicks )
        return;

    const int oldValue = m_value;
    const wxString oldText = m_textValue;
    m_snapToTicks = snap;
    m_value = AdjustValue(m_value);
    m_textValue = FormatValue(m_value);
    ApplyProgrammaticChange(
        m_value != oldValue || m_textValue != oldText);
}

bool wxSpinCtrl::SetBase(int base)
{
    if ( base != 10 && base != 16 )
        return false;
    if ( base == 16 && (m_min < 0 || m_max < 0) )
        return false;
    if ( base == m_numBase )
        return true;

    int parsed;
    const bool hadValidText = ParseText(m_textValue, &parsed);
    m_numBase = base;
    if ( hadValidText )
        m_textValue = FormatValue(m_value);
    if ( !ApplyToPeer() )
        return false;
    InvalidateBestSize();
    return true;
}

void wxSpinCtrl::SetSelection(long from, long to)
{
    m_selectionFrom = from;
    m_selectionTo = to;
    m_hasPendingSelection = true;
    ApplySelectionToPeer();
}

wxSize wxSpinCtrl::DoGetBestSize() const
{
    return wxWindow::FromDIP(
        wxSize(110, 32), const_cast<wxSpinCtrl*>(this));
}

bool wxSpinCtrl::ParseText(const wxString& text, int *value) const
{
    long parsed;
    if ( !text.ToLong(&parsed, m_numBase) ||
         parsed < std::numeric_limits<int>::min() ||
         parsed > std::numeric_limits<int>::max() )
    {
        return false;
    }
    if ( value )
        *value = static_cast<int>(parsed);
    return true;
}

wxString wxSpinCtrl::FormatValue(int value) const
{
    if ( m_numBase == 16 )
        return wxSpinCtrlImpl::FormatAsHex(value, wxMax(m_min, m_max));
    return wxString::Format("%d", value);
}

int wxSpinCtrl::AdjustValue(int value, bool wrap) const
{
    const int low = wxMin(m_min, m_max);
    const int high = wxMax(m_min, m_max);
    if ( wrap && HasFlag(wxSP_WRAP) )
    {
        if ( value < low )
            return high;
        if ( value > high )
            return low;
    }

    long long adjusted =
        wxWinUIClampToRange<long long>(
            value, low, high);
    if ( m_snapToTicks && m_increment > 0 )
    {
        const double scaled =
            static_cast<double>(adjusted) / m_increment;
        const double lower = std::floor(scaled);
        const double upper = std::ceil(scaled);
        const long long ticks = static_cast<long long>(
            scaled - lower < upper - scaled ? lower : upper);
        adjusted = ticks * static_cast<long long>(m_increment);
        adjusted = wxWinUIClampToRange<long long>(
            adjusted, low, high);
    }
    return static_cast<int>(adjusted);
}

int wxSpinCtrl::ValueAfterStep(int direction) const
{
    const long long low = wxMin(m_min, m_max);
    const long long high = wxMax(m_min, m_max);
    long long value =
        static_cast<long long>(m_value) +
        static_cast<long long>(direction > 0 ? 1 : -1) *
            m_increment;

    if ( HasFlag(wxSP_WRAP) )
    {
        if ( value < low )
            value = high;
        else if ( value > high )
            value = low;
    }
    value = wxWinUIClampToRange<long long>(value, low, high);
    return AdjustValue(static_cast<int>(value));
}

bool wxSpinCtrl::ApplyToPeer()
{
    if ( !m_winui )
        return true;
    if ( !m_winui->box ||
         !m_winui->callbackState )
    {
        return false;
    }

    const auto state = m_winui->callbackState;
    const std::uint64_t generation = state->Generation();
    wxWinUINumberBoxImpl * const updateImpl = m_winui.get();
    const MUXC::NumberBox updatePeer = m_winui->box;
    const int minimum = wxMin(m_min, m_max);
    const int maximum = wxMax(m_min, m_max);
    const int increment = m_increment;
    const bool wrap = HasFlag(wxSP_WRAP);
    const int base = m_numBase;
    const int value = m_value;
    const wxString text = m_textValue;
    wxWinUINumberBoxMutationGuard guard(state);
    try
    {
        wxWinUIApplyNumberBoxRange(
            updatePeer, minimum, maximum);
        updatePeer.SmallChange(increment);
        updatePeer.LargeChange(increment);
        updatePeer.IsWrapEnabled(wrap);
        wxWinUIApplyIntegerFormatter(
            updatePeer, base, maximum);
        updatePeer.Value(value);
        updatePeer.Text(wxWinUIToHString(text));
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI integer NumberBox update", e);
    }

    // ForceRender() synchronously flushes the shared host. Its callbacks may
    // delete this NumberBox owner, so the raw implementation is used only to
    // enter the call and every subsequent operation starts from the retained
    // callback state.
    updateImpl->host.ForceRender();
    wxSpinCtrl *liveOwner =
        state->GetOwner<wxSpinCtrl>(generation);
    if ( !liveOwner || !liveOwner->m_winui ||
         liveOwner->m_winui.get() != updateImpl ||
         liveOwner->m_winui->callbackState != state ||
         liveOwner->m_winui->box != updatePeer )
    {
        return false;
    }

    // NumberBox validation can replace invalid programmatic text with its
    // numeric Value. The wx contract keeps this text verbatim, so write the
    // resolved edit part last while callbacks remain suppressed.
    liveOwner->ResolveTextPart();
    liveOwner = state->GetOwner<wxSpinCtrl>(generation);
    if ( !liveOwner || !liveOwner->m_winui ||
         liveOwner->m_winui.get() != updateImpl ||
         liveOwner->m_winui->callbackState != state ||
         liveOwner->m_winui->box != updatePeer )
    {
        return false;
    }

    if ( liveOwner->m_winui->textBox )
    {
        try
        {
            const wxString currentText = liveOwner->m_textValue;
            const MUXC::TextBox textBox =
                liveOwner->m_winui->textBox;
            if ( wxWinUIFromHString(textBox.Text()) != currentText )
            {
                textBox.Text(wxWinUIToHString(currentText));
            }
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException(
                "WinUI integer NumberBox text update", e);
        }
    }

    liveOwner = state->GetOwner<wxSpinCtrl>(generation);
    return liveOwner && liveOwner->m_winui &&
           liveOwner->m_winui.get() == updateImpl &&
           liveOwner->m_winui->callbackState == state &&
           liveOwner->m_winui->box == updatePeer;
}

void wxSpinCtrl::ApplyProgrammaticChange(bool selectAll)
{
    if ( selectAll )
    {
        m_selectionFrom = 0;
        m_selectionTo = static_cast<long>(m_textValue.length());
        m_hasPendingSelection = true;
    }

    if ( !ApplyToPeer() )
        return;
    if ( selectAll || m_hasPendingSelection )
        ApplySelectionToPeer();
}

void wxSpinCtrl::ResolveTextPart(bool updateLayout)
{
    if ( m_winui )
        m_winui->ResolveTextPart(this, updateLayout);
}

void wxSpinCtrl::ApplySelectionToPeer()
{
    ResolveTextPart();
    if ( !m_winui || !m_winui->textBox )
        return;

    try
    {
        const long length =
            static_cast<long>(m_textValue.length());
        long from = m_selectionFrom;
        long to = m_selectionTo;
        if ( from == -1 && to == -1 )
        {
            from = 0;
            to = length;
        }
        from = wxClip(from, 0L, length);
        to = wxClip(to, 0L, length);
        m_winui->textBox.Select(
            static_cast<int32_t>(wxMin(from, to)),
            static_cast<int32_t>(std::abs(to - from)));
        m_selectionFrom = from;
        m_selectionTo = to;
        m_hasPendingSelection = false;
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException(
            "WinUI integer NumberBox selection", e);
    }
}

void wxSpinCtrl::OnPeerTextChanged(const wxString& text)
{
    if ( !IsEnabled() )
    {
        ApplyToPeer();
        return;
    }
    if ( text == m_textValue || !m_winui ||
         !m_winui->callbackState )
    {
        return;
    }

    const auto state = m_winui->callbackState;
    const std::uint64_t generation = state->Generation();
    m_textValue = text;

    wxCommandEvent event(wxEVT_TEXT, GetId());
    event.SetEventObject(this);
    event.SetString(text);
    HandleWindowEvent(event);
    wxUnusedVar(generation);
    // No member access: the handler may have destroyed this control.
}

void wxSpinCtrl::OnPeerValueChanged(double newValue)
{
    if ( !std::isfinite(newValue) || !m_winui ||
         !m_winui->callbackState )
    {
        return;
    }

    if ( !IsEnabled() )
    {
        ApplyToPeer();
        return;
    }

    const auto state = m_winui->callbackState;
    const std::uint64_t generation = state->Generation();
    const int low = wxMin(m_min, m_max);
    const int high = wxMax(m_min, m_max);
    int value;
    if ( newValue < low )
        value = HasFlag(wxSP_WRAP) ? high : low;
    else if ( newValue > high )
        value = HasFlag(wxSP_WRAP) ? low : high;
    else
        value = AdjustValue(
            static_cast<int>(std::lround(newValue)));
    const int oldValue = m_value;
    const wxString text = FormatValue(value);
    const bool textChanged = text != m_textValue;

    m_value = value;
    m_textValue = text;
    ApplyToPeer();

    wxSpinCtrl *owner = state->GetOwner<wxSpinCtrl>(generation);
    if ( !owner )
        return;

    if ( textChanged )
    {
        wxCommandEvent textEvent(wxEVT_TEXT, owner->GetId());
        textEvent.SetEventObject(owner);
        textEvent.SetString(text);
        owner->HandleWindowEvent(textEvent);

        owner = state->GetOwner<wxSpinCtrl>(generation);
        if ( !owner || owner->m_value != value ||
             owner->m_textValue != text )
        {
            return;
        }
    }

    if ( value != oldValue )
    {
        wxSpinEvent event(wxEVT_SPINCTRL, owner->GetId());
        event.SetEventObject(owner);
        event.SetPosition(value);
        event.SetString(text);
        owner->HandleWindowEvent(event);
    }
}

void wxSpinCtrl::OnPeerEnter()
{
    if ( !m_winui || !m_winui->callbackState )
        return;

    const auto state = m_winui->callbackState;
    const std::uint64_t generation = state->Generation();

    int value;
    if ( ParseText(m_textValue, &value) &&
         value >= wxMin(m_min, m_max) &&
         value <= wxMax(m_min, m_max) )
    {
        OnPeerValueChanged(value);
    }
    else
    {
        OnPeerValueChanged(m_value);
    }

    wxSpinCtrl * const owner =
        state->GetOwner<wxSpinCtrl>(generation);
    if ( !owner || !owner->HasFlag(wxTE_PROCESS_ENTER) )
        return;

    wxCommandEvent event(wxEVT_TEXT_ENTER, owner->GetId());
    event.SetEventObject(owner);
    event.SetString(owner->m_textValue);
    event.SetInt(owner->m_value);
    owner->HandleWindowEvent(event);
}

#ifdef WXWINUI_TEST_SUPPORT
bool wxWinUISpinCtrlTestAccess::SetPeerValue(
    wxSpinCtrl& control,
    double value)
{
    if ( !control.m_winui || !control.m_winui->box ||
         !control.m_winui->callbackState )
    {
        return false;
    }

    const auto state = control.m_winui->callbackState;
    const std::uint64_t generation = state->Generation();
    try
    {
        control.m_winui->box.Value(value);
    }
    catch ( const winrt::hresult_error& )
    {
        return false;
    }
    return state->GetOwner<wxSpinCtrl>(generation) != nullptr;
}

bool wxWinUISpinCtrlTestAccess::SetPeerText(
    wxSpinCtrl& control,
    const wxString& text)
{
    control.ResolveTextPart();
    if ( !control.m_winui || !control.m_winui->callbackState )
        return false;

    const auto state = control.m_winui->callbackState;
    const std::uint64_t generation = state->Generation();
    const wxString oldText = control.m_textValue;
    try
    {
        if ( control.m_winui->textBox )
            control.m_winui->textBox.Text(wxWinUIToHString(text));
        else
            control.m_winui->box.Text(wxWinUIToHString(text));
    }
    catch ( const winrt::hresult_error& )
    {
        return false;
    }

    // Before the NumberBox template is fully live, assigning its edit part
    // may not deliver TextChanged (or NumberBox may restore its formatted
    // value first). Complete the same callback transaction deterministically,
    // but never overwrite a synchronous reentrant change.
    wxSpinCtrl *owner = state->GetOwner<wxSpinCtrl>(generation);
    if ( owner && owner->m_textValue == oldText && text != oldText )
        owner->OnPeerTextChanged(text);

    return state->GetOwner<wxSpinCtrl>(generation) != nullptr;
}

bool wxWinUISpinCtrlTestAccess::GetPeerState(
    const wxSpinCtrl& control,
    double *minimum,
    double *maximum,
    double *increment,
    bool *wrap,
    wxString *text,
    double *value)
{
    if ( !control.m_winui || !control.m_winui->box )
        return false;

    try
    {
        if ( minimum )
            *minimum = control.m_winui->box.Minimum();
        if ( maximum )
            *maximum = control.m_winui->box.Maximum();
        if ( increment )
            *increment = control.m_winui->box.SmallChange();
        if ( wrap )
            *wrap = control.m_winui->box.IsWrapEnabled();
        if ( text )
        {
            *text = wxWinUIFromHString(
                control.m_winui->textBox
                    ? control.m_winui->textBox.Text()
                    : control.m_winui->box.Text());
        }
        if ( value )
            *value = control.m_winui->box.Value();
        return true;
    }
    catch ( const winrt::hresult_error& )
    {
        return false;
    }
}

bool wxWinUISpinCtrlTestAccess::GetPeerSelection(
    wxSpinCtrl& control,
    long *from, long *to)
{
    control.ResolveTextPart();
    if ( !control.m_winui || !control.m_winui->textBox )
        return false;

    try
    {
        if ( from )
            *from = control.m_winui->textBox.SelectionStart();
        if ( to )
        {
            *to = control.m_winui->textBox.SelectionStart() +
                  control.m_winui->textBox.SelectionLength();
        }
        return true;
    }
    catch ( const winrt::hresult_error& )
    {
        return false;
    }
}

bool wxWinUISpinCtrlTestAccess::Retemplate(wxSpinCtrl& control)
{
    control.ResolveTextPart();
    if ( !control.m_winui || !control.m_winui->textBox )
        return false;

    // Exercise the same revocation, generation and selection-restoration path
    // used when XAML replaces the NumberBox template part.
    control.m_winui->DetachTextPart();
    control.ResolveTextPart();
    return control.m_winui && control.m_winui->textBox;
}

bool wxWinUISpinCtrlTestAccess::Enter(wxSpinCtrl& control)
{
    if ( !control.m_winui || !control.m_winui->callbackState )
        return false;

    const auto state = control.m_winui->callbackState;
    const std::uint64_t generation = state->Generation();
    control.OnPeerEnter();
    return state->GetOwner<wxSpinCtrl>(generation) != nullptr;
}

bool wxWinUISpinCtrlTestAccess::Step(
    wxSpinCtrl& control,
    int direction)
{
    if ( direction == 0 || !control.IsEnabled() || !control.m_winui ||
         !control.m_winui->callbackState )
    {
        return false;
    }

    const auto state = control.m_winui->callbackState;
    const std::uint64_t generation = state->Generation();
    const int oldValue = control.m_value;
    control.OnPeerValueChanged(control.ValueAfterStep(direction));
    if ( wxSpinCtrl * const owner =
             state->GetOwner<wxSpinCtrl>(generation) )
    {
        return owner->m_value != oldValue;
    }
    return false;
}
#endif

//-----------------------------------------------------------------------------
// wxSpinCtrlDouble
//-----------------------------------------------------------------------------

wxIMPLEMENT_DYNAMIC_CLASS(wxSpinCtrlDouble, wxControl);

wxSpinCtrlDouble::wxSpinCtrlDouble()
{
}

wxSpinCtrlDouble::wxSpinCtrlDouble(wxWindow *parent,
                                   wxWindowID id,
                                   const wxString& value,
                                   const wxPoint& pos,
                                   const wxSize& size,
                                   long style,
                                   double min,
                                   double max,
                                   double initial,
                                   double increment,
                                   const wxString& name)
{
    Create(parent, id, value, pos, size, style,
           min, max, initial, increment, name);
}

wxSpinCtrlDouble::~wxSpinCtrlDouble()
{
    if ( m_winui )
        m_winui->Close();
    m_winui.reset();
}

bool wxSpinCtrlDouble::Create(wxWindow *parent,
                              wxWindowID id,
                              const wxString& value,
                              const wxPoint& pos,
                              const wxSize& size,
                              long style,
                              double min,
                              double max,
                              double initial,
                              double increment,
                              const wxString& name)
{
    style = (style & ~wxBORDER_MASK) | wxBORDER_NONE;
    if ( !wxControl::Create(parent, id, pos, size, style,
                            wxDefaultValidator, name) )
    {
        return false;
    }

    m_min = min;
    m_max = max;
    m_increment =
        increment > 0 && std::isfinite(increment) ? increment : 1.0;
    m_digits = wxSpinCtrlImpl::DetermineDigits(m_increment);
    m_snapToTicks = false;
    m_value = CanonicalizeValue(initial);
    m_textValue = FormatValue(m_value);
    bool selectInitialText = false;

    if ( !value.empty() )
    {
        double parsed;
        if ( ParseText(value, &parsed) &&
             parsed >= wxMin(m_min, m_max) &&
             parsed <= wxMax(m_min, m_max) )
        {
            m_value = CanonicalizeValue(parsed);
            m_textValue = FormatValue(m_value);
        }
        else
        {
            m_value = CanonicalizeValue(m_min);
            m_textValue = value;
            selectInitialText = true;
        }
    }
    m_selectionFrom = selectInitialText
                          ? 0
                          : static_cast<long>(m_textValue.length());
    m_selectionTo = static_cast<long>(m_textValue.length());
    m_hasPendingSelection = selectInitialText;

    m_winui = std::make_unique<wxWinUINumberBoxImpl>();
    m_winui->callbackState =
        std::make_shared<wxWinUINumberBoxCallbackState>(this);
    wxWinUINumberBoxImpl * const createImpl = m_winui.get();
    const std::shared_ptr<wxWinUINumberBoxCallbackState> createState =
        m_winui->callbackState;
    const std::uint64_t createGeneration =
        createState->Generation();
    if ( !m_winui->host.Initialize(this) )
    {
        m_winui->Close();
        return false;
    }

    try
    {
        const bool configured = createImpl->Configure(this);
        wxSpinCtrlDouble * const liveOwner =
            createState->GetOwner<wxSpinCtrlDouble>(
                createGeneration);
        if ( !liveOwner || !liveOwner->m_winui ||
             liveOwner->m_winui.get() != createImpl ||
             liveOwner->m_winui->callbackState != createState )
        {
            return false;
        }

        if ( !configured )
        {
            liveOwner->m_winui->Close();
            return false;
        }
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI double NumberBox creation", e);
        wxSpinCtrlDouble * const liveOwner =
            createState->GetOwner<wxSpinCtrlDouble>(
                createGeneration);
        if ( liveOwner && liveOwner->m_winui &&
             liveOwner->m_winui.get() == createImpl &&
             liveOwner->m_winui->callbackState == createState )
        {
            liveOwner->m_winui->Close();
        }
        return false;
    }

    wxSpinCtrlDouble *liveOwner =
        createState->GetOwner<wxSpinCtrlDouble>(
            createGeneration);
    if ( !liveOwner || !liveOwner->m_winui ||
         liveOwner->m_winui.get() != createImpl ||
         liveOwner->m_winui->callbackState != createState )
    {
        return false;
    }

    liveOwner->SetInitialSize(size);
    liveOwner =
        createState->GetOwner<wxSpinCtrlDouble>(
            createGeneration);
    return liveOwner && liveOwner->m_winui &&
           liveOwner->m_winui.get() == createImpl &&
           liveOwner->m_winui->callbackState == createState;
}

void wxSpinCtrlDouble::SetValue(double value)
{
    wxCHECK_RET(std::isfinite(value),
                "spin control value must be finite");
    const double oldValue = m_value;
    const wxString oldText = m_textValue;
    m_value = CanonicalizeValue(value);
    m_textValue = FormatValue(m_value);
    ApplyProgrammaticChange(
        m_value != oldValue || m_textValue != oldText);
}

void wxSpinCtrlDouble::SetValue(const wxString& value)
{
    double parsed;
    if ( ParseText(value, &parsed) &&
         parsed >= wxMin(m_min, m_max) &&
         parsed <= wxMax(m_min, m_max) )
    {
        SetValue(parsed);
        return;
    }

    m_value = CanonicalizeValue(m_min);
    m_textValue = value;
    ApplyProgrammaticChange(true);
}

void wxSpinCtrlDouble::SetRange(double minValue, double maxValue)
{
    wxCHECK_RET(std::isfinite(minValue) && std::isfinite(maxValue),
                "spin control range must be finite");

    double parsedText;
    const bool hadNumericText =
        ParseText(m_textValue, &parsedText);
    const double oldValue = m_value;
    const wxString oldText = m_textValue;
    m_min = minValue;
    m_max = maxValue;
    m_value = CanonicalizeValue(m_value);
    if ( m_value != oldValue || hadNumericText )
        m_textValue = FormatValue(m_value);
    ApplyProgrammaticChange(
        m_value != oldValue || m_textValue != oldText);
    InvalidateBestSize();
}

void wxSpinCtrlDouble::SetIncrement(double increment)
{
    wxCHECK_RET(increment > 0 && std::isfinite(increment),
                "spin control increment must be positive and finite");
    if ( increment == m_increment )
        return;

    const double oldValue = m_value;
    const wxString oldText = m_textValue;
    m_increment = increment;
    const unsigned required =
        wxSpinCtrlImpl::DetermineDigits(increment);
    if ( required > m_digits )
        m_digits = required;

    if ( m_snapToTicks )
        m_value = CanonicalizeValue(m_value);
    m_textValue = FormatValue(m_value);
    ApplyProgrammaticChange(
        m_value != oldValue || m_textValue != oldText);
    InvalidateBestSize();
}

void wxSpinCtrlDouble::SetDigits(unsigned digits)
{
    wxCHECK_RET(digits <= wxSpinCtrlImpl::SPINCTRLDBL_MAX_DIGITS,
                "too many digits for wxSpinCtrlDouble");
    if ( digits == m_digits )
        return;

    const double oldValue = m_value;
    const wxString oldText = m_textValue;
    m_digits = digits;
    m_value = CanonicalizeValue(m_value);
    m_textValue = FormatValue(m_value);
    ApplyProgrammaticChange(
        m_value != oldValue || m_textValue != oldText);
    InvalidateBestSize();
}

wxString wxSpinCtrlDouble::GetTextValue() const
{
    return m_textValue;
}

void wxSpinCtrlDouble::SetSnapToTicks(bool snap)
{
    if ( snap == m_snapToTicks )
        return;

    const double oldValue = m_value;
    const wxString oldText = m_textValue;
    m_snapToTicks = snap;
    m_value = CanonicalizeValue(m_value);
    m_textValue = FormatValue(m_value);
    ApplyProgrammaticChange(
        m_value != oldValue || m_textValue != oldText);
}

void wxSpinCtrlDouble::SetSelection(long from, long to)
{
    m_selectionFrom = from;
    m_selectionTo = to;
    m_hasPendingSelection = true;
    ApplySelectionToPeer();
}

wxSize wxSpinCtrlDouble::DoGetBestSize() const
{
    return wxWindow::FromDIP(
        wxSize(120, 32), const_cast<wxSpinCtrlDouble*>(this));
}

bool wxSpinCtrlDouble::ParseText(const wxString& text,
                                 double *value) const
{
    double parsed;
    if ( !wxNumberFormatter::FromString(text, &parsed) ||
         !std::isfinite(parsed) )
    {
        return false;
    }
    if ( value )
        *value = parsed;
    return true;
}

wxString wxSpinCtrlDouble::FormatValue(double value) const
{
    return wxNumberFormatter::ToString(
        value, static_cast<int>(m_digits));
}

double wxSpinCtrlDouble::AdjustValue(double value, bool wrap) const
{
    const double low = wxMin(m_min, m_max);
    const double high = wxMax(m_min, m_max);
    if ( wrap && HasFlag(wxSP_WRAP) )
    {
        if ( value < low )
            return high;
        if ( value > high )
            return low;
    }

    value = wxWinUIClampToRange(value, m_min, m_max);
    if ( m_snapToTicks && m_increment > 0 )
    {
        const double scaled = value / m_increment;
        if ( std::isfinite(scaled) )
        {
            const double lower = std::floor(scaled);
            const double upper = std::ceil(scaled);
            const double ticks =
                scaled - lower < upper - scaled ? lower : upper;
            value = ticks * m_increment;
        }
        value = wxWinUIClampToRange(value, m_min, m_max);
    }
    return value;
}

double wxSpinCtrlDouble::CanonicalizeValue(double value, bool wrap) const
{
    value = AdjustValue(value, wrap);

    // Match the generic control's WYSIWYG contract: GetValue() represents
    // exactly the number visible with the current precision, not hidden
    // fractional digits which could reappear after SetDigits/SetIncrement.
    double displayed;
    if ( ParseText(FormatValue(value), &displayed) )
        value = AdjustValue(displayed);
    return value;
}

double wxSpinCtrlDouble::ValueAfterStep(int direction) const
{
    return AdjustValue(
        m_value +
            (direction > 0 ? m_increment : -m_increment),
        true);
}

bool wxSpinCtrlDouble::ApplyToPeer()
{
    if ( !m_winui )
        return true;
    if ( !m_winui->box ||
         !m_winui->callbackState )
    {
        return false;
    }

    const auto state = m_winui->callbackState;
    const std::uint64_t generation = state->Generation();
    wxWinUINumberBoxImpl * const updateImpl = m_winui.get();
    const MUXC::NumberBox updatePeer = m_winui->box;
    const double minimum = wxMin(m_min, m_max);
    const double maximum = wxMax(m_min, m_max);
    const double increment = m_increment;
    const bool wrap = HasFlag(wxSP_WRAP);
    const unsigned digits = m_digits;
    const double value = m_value;
    const wxString text = m_textValue;
    wxWinUINumberBoxMutationGuard guard(state);
    try
    {
        wxWinUIApplyNumberBoxRange(
            updatePeer, minimum, maximum);
        updatePeer.SmallChange(increment);
        updatePeer.LargeChange(increment);
        updatePeer.IsWrapEnabled(wrap);
        wxWinUIApplyDigits(updatePeer, digits);
        updatePeer.Value(value);
        updatePeer.Text(wxWinUIToHString(text));
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI double NumberBox update", e);
    }

    updateImpl->host.ForceRender();
    wxSpinCtrlDouble *liveOwner =
        state->GetOwner<wxSpinCtrlDouble>(generation);
    if ( !liveOwner || !liveOwner->m_winui ||
         liveOwner->m_winui.get() != updateImpl ||
         liveOwner->m_winui->callbackState != state ||
         liveOwner->m_winui->box != updatePeer )
    {
        return false;
    }

    liveOwner->ResolveTextPart();
    liveOwner =
        state->GetOwner<wxSpinCtrlDouble>(generation);
    if ( !liveOwner || !liveOwner->m_winui ||
         liveOwner->m_winui.get() != updateImpl ||
         liveOwner->m_winui->callbackState != state ||
         liveOwner->m_winui->box != updatePeer )
    {
        return false;
    }

    if ( liveOwner->m_winui->textBox )
    {
        try
        {
            const wxString currentText = liveOwner->m_textValue;
            const MUXC::TextBox textBox =
                liveOwner->m_winui->textBox;
            if ( wxWinUIFromHString(textBox.Text()) != currentText )
            {
                textBox.Text(wxWinUIToHString(currentText));
            }
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException(
                "WinUI double NumberBox text update", e);
        }
    }

    liveOwner =
        state->GetOwner<wxSpinCtrlDouble>(generation);
    return liveOwner && liveOwner->m_winui &&
           liveOwner->m_winui.get() == updateImpl &&
           liveOwner->m_winui->callbackState == state &&
           liveOwner->m_winui->box == updatePeer;
}

void wxSpinCtrlDouble::ApplyProgrammaticChange(bool selectAll)
{
    if ( selectAll )
    {
        m_selectionFrom = 0;
        m_selectionTo = static_cast<long>(m_textValue.length());
        m_hasPendingSelection = true;
    }

    if ( !ApplyToPeer() )
        return;
    if ( selectAll || m_hasPendingSelection )
        ApplySelectionToPeer();
}

void wxSpinCtrlDouble::ResolveTextPart(bool updateLayout)
{
    if ( m_winui )
        m_winui->ResolveTextPart(this, updateLayout);
}

void wxSpinCtrlDouble::ApplySelectionToPeer()
{
    ResolveTextPart();
    if ( !m_winui || !m_winui->textBox )
        return;

    try
    {
        const long length =
            static_cast<long>(m_textValue.length());
        long from = m_selectionFrom;
        long to = m_selectionTo;
        if ( from == -1 && to == -1 )
        {
            from = 0;
            to = length;
        }
        from = wxClip(from, 0L, length);
        to = wxClip(to, 0L, length);
        m_winui->textBox.Select(
            static_cast<int32_t>(wxMin(from, to)),
            static_cast<int32_t>(std::abs(to - from)));
        m_selectionFrom = from;
        m_selectionTo = to;
        m_hasPendingSelection = false;
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException(
            "WinUI double NumberBox selection", e);
    }
}

void wxSpinCtrlDouble::OnPeerTextChanged(const wxString& text)
{
    if ( !IsEnabled() )
    {
        ApplyToPeer();
        return;
    }
    if ( text == m_textValue || !m_winui ||
         !m_winui->callbackState )
    {
        return;
    }

    m_textValue = text;
    wxCommandEvent event(wxEVT_TEXT, GetId());
    event.SetEventObject(this);
    event.SetString(text);
    HandleWindowEvent(event);
    // No member access: the handler may have destroyed this control.
}

void wxSpinCtrlDouble::OnPeerValueChanged(double newValue)
{
    if ( !std::isfinite(newValue) || !m_winui ||
         !m_winui->callbackState )
    {
        return;
    }

    if ( !IsEnabled() )
    {
        ApplyToPeer();
        return;
    }

    const auto state = m_winui->callbackState;
    const std::uint64_t generation = state->Generation();
    const double value = CanonicalizeValue(newValue, true);
    const double oldValue = m_value;
    const wxString text = FormatValue(value);
    const bool textChanged = text != m_textValue;

    m_value = value;
    m_textValue = text;
    ApplyToPeer();

    wxSpinCtrlDouble *owner =
        state->GetOwner<wxSpinCtrlDouble>(generation);
    if ( !owner )
        return;

    if ( textChanged )
    {
        wxCommandEvent textEvent(wxEVT_TEXT, owner->GetId());
        textEvent.SetEventObject(owner);
        textEvent.SetString(text);
        owner->HandleWindowEvent(textEvent);

        owner = state->GetOwner<wxSpinCtrlDouble>(generation);
        if ( !owner || owner->m_value != value ||
             owner->m_textValue != text )
        {
            return;
        }
    }

    if ( value != oldValue )
    {
        wxSpinDoubleEvent event(
            wxEVT_SPINCTRLDOUBLE, owner->GetId(), value);
        event.SetEventObject(owner);
        event.SetString(text);
        owner->HandleWindowEvent(event);
    }
}

void wxSpinCtrlDouble::OnPeerEnter()
{
    if ( !m_winui || !m_winui->callbackState )
        return;

    const auto state = m_winui->callbackState;
    const std::uint64_t generation = state->Generation();

    double value;
    if ( ParseText(m_textValue, &value) &&
         value >= wxMin(m_min, m_max) &&
         value <= wxMax(m_min, m_max) )
    {
        OnPeerValueChanged(value);
    }
    else
    {
        OnPeerValueChanged(m_value);
    }

    wxSpinCtrlDouble * const owner =
        state->GetOwner<wxSpinCtrlDouble>(generation);
    if ( !owner || !owner->HasFlag(wxTE_PROCESS_ENTER) )
        return;

    wxCommandEvent event(wxEVT_TEXT_ENTER, owner->GetId());
    event.SetEventObject(owner);
    event.SetString(owner->m_textValue);
    owner->HandleWindowEvent(event);
}

#ifdef WXWINUI_TEST_SUPPORT
bool wxWinUISpinCtrlTestAccess::SetPeerValue(
    wxSpinCtrlDouble& control,
    double value)
{
    if ( !control.m_winui || !control.m_winui->box ||
         !control.m_winui->callbackState )
    {
        return false;
    }

    const auto state = control.m_winui->callbackState;
    const std::uint64_t generation = state->Generation();
    try
    {
        control.m_winui->box.Value(value);
    }
    catch ( const winrt::hresult_error& )
    {
        return false;
    }
    return state->GetOwner<wxSpinCtrlDouble>(generation) != nullptr;
}

bool wxWinUISpinCtrlTestAccess::SetPeerText(
    wxSpinCtrlDouble& control,
    const wxString& text)
{
    control.ResolveTextPart();
    if ( !control.m_winui || !control.m_winui->callbackState )
        return false;

    const auto state = control.m_winui->callbackState;
    const std::uint64_t generation = state->Generation();
    const wxString oldText = control.m_textValue;
    try
    {
        if ( control.m_winui->textBox )
            control.m_winui->textBox.Text(wxWinUIToHString(text));
        else
            control.m_winui->box.Text(wxWinUIToHString(text));
    }
    catch ( const winrt::hresult_error& )
    {
        return false;
    }

    wxSpinCtrlDouble *owner =
        state->GetOwner<wxSpinCtrlDouble>(generation);
    if ( owner && owner->m_textValue == oldText && text != oldText )
        owner->OnPeerTextChanged(text);

    return state->GetOwner<wxSpinCtrlDouble>(generation) != nullptr;
}

bool wxWinUISpinCtrlTestAccess::GetPeerState(
    const wxSpinCtrlDouble& control,
    double *minimum,
    double *maximum,
    double *increment,
    bool *wrap,
    wxString *text,
    double *value)
{
    if ( !control.m_winui || !control.m_winui->box )
        return false;

    try
    {
        if ( minimum )
            *minimum = control.m_winui->box.Minimum();
        if ( maximum )
            *maximum = control.m_winui->box.Maximum();
        if ( increment )
            *increment = control.m_winui->box.SmallChange();
        if ( wrap )
            *wrap = control.m_winui->box.IsWrapEnabled();
        if ( text )
        {
            *text = wxWinUIFromHString(
                control.m_winui->textBox
                    ? control.m_winui->textBox.Text()
                    : control.m_winui->box.Text());
        }
        if ( value )
            *value = control.m_winui->box.Value();
        return true;
    }
    catch ( const winrt::hresult_error& )
    {
        return false;
    }
}

bool wxWinUISpinCtrlTestAccess::GetPeerSelection(
    wxSpinCtrlDouble& control,
    long *from,
    long *to)
{
    control.ResolveTextPart();
    if ( !control.m_winui || !control.m_winui->textBox )
        return false;

    try
    {
        if ( from )
            *from = control.m_winui->textBox.SelectionStart();
        if ( to )
        {
            *to = control.m_winui->textBox.SelectionStart() +
                  control.m_winui->textBox.SelectionLength();
        }
        return true;
    }
    catch ( const winrt::hresult_error& )
    {
        return false;
    }
}

bool wxWinUISpinCtrlTestAccess::Retemplate(wxSpinCtrlDouble& control)
{
    control.ResolveTextPart();
    if ( !control.m_winui || !control.m_winui->textBox )
        return false;

    control.m_winui->DetachTextPart();
    control.ResolveTextPart();
    return control.m_winui && control.m_winui->textBox;
}

bool wxWinUISpinCtrlTestAccess::Enter(wxSpinCtrlDouble& control)
{
    if ( !control.m_winui || !control.m_winui->callbackState )
        return false;

    const auto state = control.m_winui->callbackState;
    const std::uint64_t generation = state->Generation();
    control.OnPeerEnter();
    return state->GetOwner<wxSpinCtrlDouble>(generation) != nullptr;
}

bool wxWinUISpinCtrlTestAccess::Step(
    wxSpinCtrlDouble& control,
    int direction)
{
    if ( direction == 0 || !control.IsEnabled() || !control.m_winui ||
         !control.m_winui->callbackState )
    {
        return false;
    }

    const auto state = control.m_winui->callbackState;
    const std::uint64_t generation = state->Generation();
    const double oldValue = control.m_value;
    control.OnPeerValueChanged(control.ValueAfterStep(direction));
    if ( wxSpinCtrlDouble * const owner =
             state->GetOwner<wxSpinCtrlDouble>(generation) )
    {
        return owner->m_value != oldValue;
    }
    return false;
}
#endif

#endif // wxUSE_SPINCTRL
