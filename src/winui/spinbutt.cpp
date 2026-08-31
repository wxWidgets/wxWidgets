/////////////////////////////////////////////////////////////////////////////
// Name:        src/winui/spinbutt.cpp
// Purpose:     wxWinUI wxSpinButton implementation (WinUI RepeatButtons)
// Author:      wxWidgets development team
// Created:     2026-06-01
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_SPINBTN

#include "wx/spinbutt.h"

#include "private.h"
#include "wx/winui/private/appearance.h"

#ifdef WXWINUI_TEST_SUPPORT
    #include "range-test-access.h"
#endif

#include <atomic>
#include <cstdint>

#include <winrt/Windows.System.h>
#include <winrt/Microsoft.UI.Xaml.Automation.h>
#include <winrt/Microsoft.UI.Xaml.Automation.Peers.h>
#include <winrt/Microsoft.UI.Xaml.Automation.Provider.h>

namespace MUX = winrt::Microsoft::UI::Xaml;
namespace MUXA = winrt::Microsoft::UI::Xaml::Automation;
namespace MUXAP = winrt::Microsoft::UI::Xaml::Automation::Peers;
namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;
namespace MUXCP = winrt::Microsoft::UI::Xaml::Controls::Primitives;
namespace MUXI = winrt::Microsoft::UI::Xaml::Input;

// wxSpinButton RTTI (and wxSpinEvent) are provided by
// src/common/spinbtncmn.cpp.

namespace
{

// XAML delegates retain this invalidatable state, never the wx control. A
// generation makes any notification already queued for a retired peer a no-op.
class wxWinUISpinButtonCallbackState
{
public:
    explicit wxWinUISpinButtonCallbackState(wxSpinButton *owner)
        : m_owner(owner)
    {
    }

    std::uint64_t Generation() const
    {
        return m_generation.load(std::memory_order_acquire);
    }

    wxSpinButton *GetOwner(std::uint64_t generation) const
    {
        if ( generation != m_generation.load(std::memory_order_acquire) )
            return nullptr;
        return m_owner.load(std::memory_order_acquire);
    }

    void Invalidate()
    {
        m_owner.store(nullptr, std::memory_order_release);
        m_generation.fetch_add(1, std::memory_order_acq_rel);
    }

private:
    std::atomic<wxSpinButton *> m_owner;
    std::atomic<std::uint64_t> m_generation{1};
};

MUXCP::RepeatButton wxWinUIMakeArrow(const wchar_t* glyph,
                                     const wxString& automationName,
                                     const wchar_t* automationId)
{
    MUXC::FontIcon icon;
    icon.Glyph(glyph);
    icon.FontSize(12);

    MUXCP::RepeatButton button;
    button.Content(icon);
    button.Padding(MUX::ThicknessHelper::FromUniformLength(0));
    button.MinWidth(0);
    button.MinHeight(0);
    button.HorizontalAlignment(MUX::HorizontalAlignment::Stretch);
    button.VerticalAlignment(MUX::VerticalAlignment::Stretch);
    button.HorizontalContentAlignment(MUX::HorizontalAlignment::Center);
    button.VerticalContentAlignment(MUX::VerticalAlignment::Center);
    MUXA::AutomationProperties::SetName(
        button, wxWinUIToHString(automationName));
    MUXA::AutomationProperties::SetAutomationId(
        button, winrt::hstring(automationId));
    return button;
}

int wxWinUIClampSpinValue(int value, int minValue, int maxValue)
{
    const int low = wxMin(minValue, maxValue);
    const int high = wxMax(minValue, maxValue);
    return wxClip(value, low, high);
}

} // anonymous namespace

class wxWinUISpinButtonImpl
{
public:
    ~wxWinUISpinButtonImpl()
    {
        Close();
    }

    void Close()
    {
        if ( callbackState )
            callbackState->Invalidate();

        if ( up && upToken.value )
        {
            try
            {
                up.Click(upToken);
            }
            catch ( const winrt::hresult_error& e )
            {
                wxWinUILogException(
                    "WinUI SpinButton up Click removal", e);
            }
        }
        upToken = {};

        if ( down && downToken.value )
        {
            try
            {
                down.Click(downToken);
            }
            catch ( const winrt::hresult_error& e )
            {
                wxWinUILogException(
                    "WinUI SpinButton down Click removal", e);
            }
        }
        downToken = {};

        if ( grid && keyDownToken.value )
        {
            try
            {
                grid.PreviewKeyDown(keyDownToken);
            }
            catch ( const winrt::hresult_error& e )
            {
                wxWinUILogException(
                    "WinUI SpinButton PreviewKeyDown removal", e);
            }
        }
        keyDownToken = {};

        host.Close();
        grid = nullptr;
        up = nullptr;
        down = nullptr;
        callbackState.reset();
    }

    wxWinUIControlHost host;
    std::shared_ptr<wxWinUISpinButtonCallbackState> callbackState;
    MUXC::Grid grid{ nullptr };
    MUXCP::RepeatButton up{ nullptr };
    MUXCP::RepeatButton down{ nullptr };
    winrt::event_token upToken{};
    winrt::event_token downToken{};
    winrt::event_token keyDownToken{};
};

wxSpinButton::wxSpinButton()
{
}

wxSpinButton::wxSpinButton(wxWindow *parent,
                           wxWindowID id,
                           const wxPoint& pos,
                           const wxSize& size,
                           long style,
                           const wxString& name)
{
    Create(parent, id, pos, size, style, name);
}

wxSpinButton::~wxSpinButton()
{
    // Revoke delegates while every model member is still alive. m_winui is
    // declared before the scalars and would otherwise be destroyed after
    // them, leaving a narrow teardown callback lifetime hole.
    if ( m_winui )
        m_winui->Close();
    m_winui.reset();
}

bool wxSpinButton::Create(wxWindow *parent,
                          wxWindowID id,
                          const wxPoint& pos,
                          const wxSize& size,
                          long style,
                          const wxString& name)
{
    style = (style & ~wxBORDER_MASK) | wxBORDER_NONE;
    if ( style & wxSP_VERTICAL )
        style &= ~wxSP_HORIZONTAL;
    else if ( !(style & wxSP_HORIZONTAL) )
        style |= wxSP_VERTICAL;

    if ( !wxControl::Create(parent, id, pos, size, style,
                            wxDefaultValidator, name) )
    {
        return false;
    }

    m_value = wxWinUIClampSpinValue(m_min, m_min, m_max);
    m_increment = 1;

    m_winui = std::make_unique<wxWinUISpinButtonImpl>();
    m_winui->callbackState =
        std::make_shared<wxWinUISpinButtonCallbackState>(this);
    wxWinUISpinButtonImpl * const impl = m_winui.get();
    const std::shared_ptr<wxWinUISpinButtonCallbackState> state =
        impl->callbackState;
    const std::uint64_t generation = state->Generation();
    if ( !impl->host.Initialize(this) )
    {
        wxSpinButton * const liveOwner = state->GetOwner(generation);
        if ( liveOwner && liveOwner->m_winui &&
             liveOwner->m_winui.get() == impl &&
             liveOwner->m_winui->callbackState == state )
        {
            impl->Close();
        }
        return false;
    }

    wxSpinButton * const ownerAfterInitialize =
        state->GetOwner(generation);
    if ( !ownerAfterInitialize || !ownerAfterInitialize->m_winui ||
         ownerAfterInitialize->m_winui.get() != impl ||
         ownerAfterInitialize->m_winui->callbackState != state )
    {
        return false;
    }

    try
    {
        const bool vertical = IsVertical();
        impl->up = wxWinUIMakeArrow(
            vertical ? L"\uE70E" : L"\uE76C",
            _("Increment"), L"SmallIncrement");
        impl->down = wxWinUIMakeArrow(
            vertical ? L"\uE70D" : L"\uE76B",
            _("Decrement"), L"SmallDecrement");
        // The pair is a single logical control: only its preferred child
        // participates in Tab traversal.
        impl->down.IsTabStop(false);
        impl->grid = wxWinUICreateAccessibleGrid(
            MUXAP::AutomationControlType::Spinner, "wxSpinButton");
        MUXA::AutomationProperties::SetLocalizedControlType(
            impl->grid, wxWinUIToHString(_("spinner")));

        impl->upToken = impl->up.Click(
            [state, generation](
                winrt::Windows::Foundation::IInspectable const&,
                MUX::RoutedEventArgs const&)
            {
                if ( wxSpinButton * const owner =
                         state->GetOwner(generation) )
                {
                    owner->Step(+1);
                }
            });
        impl->downToken = impl->down.Click(
            [state, generation](
                winrt::Windows::Foundation::IInspectable const&,
                MUX::RoutedEventArgs const&)
            {
                if ( wxSpinButton * const owner =
                         state->GetOwner(generation) )
                {
                    owner->Step(-1);
                }
            });

        const auto star = MUX::GridLengthHelper::FromValueAndType(
            1, MUX::GridUnitType::Star);
        if ( vertical )
        {
            MUXC::RowDefinition first;
            MUXC::RowDefinition second;
            first.Height(star);
            second.Height(star);
            impl->grid.RowDefinitions().Append(first);
            impl->grid.RowDefinitions().Append(second);
            MUXC::Grid::SetRow(impl->up, 0);
            MUXC::Grid::SetRow(impl->down, 1);
        }
        else
        {
            MUXC::ColumnDefinition first;
            MUXC::ColumnDefinition second;
            first.Width(star);
            second.Width(star);
            impl->grid.ColumnDefinitions().Append(first);
            impl->grid.ColumnDefinitions().Append(second);
            MUXC::Grid::SetColumn(impl->down, 0);
            MUXC::Grid::SetColumn(impl->up, 1);
        }

        impl->grid.Children().Append(impl->up);
        impl->grid.Children().Append(impl->down);

        impl->keyDownToken = impl->grid.PreviewKeyDown(
            [state, generation, vertical](
                winrt::Windows::Foundation::IInspectable const&,
                MUXI::KeyRoutedEventArgs const& event)
            {
                wxSpinButton * const owner =
                    state->GetOwner(generation);
                if ( !owner || !(owner->GetWindowStyle() &
                                  wxSP_ARROW_KEYS) )
                {
                    return;
                }

                int direction = 0;
                const auto key = event.Key();
                if ( vertical )
                {
                    if ( key == winrt::Windows::System::VirtualKey::Up )
                        direction = +1;
                    else if ( key ==
                              winrt::Windows::System::VirtualKey::Down )
                        direction = -1;
                }
                else
                {
                    if ( key ==
                         winrt::Windows::System::VirtualKey::Right )
                        direction = +1;
                    else if ( key ==
                              winrt::Windows::System::VirtualKey::Left )
                        direction = -1;
                }

                if ( direction )
                {
                    // Mark it before dispatch: Step() may synchronously
                    // destroy the wx control and its peer.
                    event.Handled(true);
                    owner->Step(direction);
                }
            });

        if ( !impl->host.SetContent(impl->grid) )
        {
            if ( state->GetOwner(generation) )
                impl->Close();
            return false;
        }

        // SetContent() can synchronously realize the XAML peer and enter user
        // code. Publish no preferred element through a retired implementation.
        wxSpinButton * const liveOwner = state->GetOwner(generation);
        if ( !liveOwner || !liveOwner->m_winui ||
             liveOwner->m_winui.get() != impl )
        {
            return false;
        }
        impl->host.SetPreferredFocus(impl->up);

        // SetPreferredFocus() crosses the shared host and can synchronously
        // run application code. Do not fall through to SetInitialSize() on a
        // retired wx object or replacement implementation.
        const wxSpinButton * const ownerAfterFocus =
            state->GetOwner(generation);
        if ( !ownerAfterFocus || !ownerAfterFocus->m_winui ||
             ownerAfterFocus->m_winui.get() != impl ||
             ownerAfterFocus->m_winui->callbackState != state )
        {
            return false;
        }
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI SpinButton creation", e);
        wxSpinButton * const liveOwner = state->GetOwner(generation);
        if ( liveOwner && liveOwner->m_winui &&
             liveOwner->m_winui.get() == impl &&
             liveOwner->m_winui->callbackState == state )
        {
            impl->Close();
        }
        return false;
    }

    wxSpinButton * const liveOwner = state->GetOwner(generation);
    if ( !liveOwner || !liveOwner->m_winui ||
         liveOwner->m_winui.get() != impl ||
         liveOwner->m_winui->callbackState != state )
    {
        return false;
    }
    liveOwner->SetInitialSize(size);
    return true;
}

int wxSpinButton::GetValue() const
{
    return m_value;
}

void wxSpinButton::SetValue(int value)
{
    m_value = wxWinUIClampSpinValue(value, m_min, m_max);
}

void wxSpinButton::SetRange(int minValue, int maxValue)
{
    wxSpinButtonBase::SetRange(minValue, maxValue);
    m_value = wxWinUIClampSpinValue(m_value, m_min, m_max);
}

void wxSpinButton::SetIncrement(int value)
{
    wxCHECK_RET(value > 0, "spin button increment must be positive");
    m_increment = value;
}

int wxSpinButton::GetIncrement() const
{
    return m_increment;
}

wxSize wxSpinButton::DoGetBestSize() const
{
    return wxWindow::FromDIP(
        IsVertical() ? wxSize(28, 48) : wxSize(48, 28),
        const_cast<wxSpinButton*>(this));
}

bool wxSpinButton::Step(int direction)
{
    if ( !IsEnabled() || !m_winui || !m_winui->callbackState ||
         direction == 0 )
        return false;

    const std::shared_ptr<wxWinUISpinButtonCallbackState> state =
        m_winui->callbackState;
    const std::uint64_t generation = state->Generation();
    const int oldValue = m_value;
    const int low = wxMin(m_min, m_max);
    const int high = wxMax(m_min, m_max);

    const long long candidate =
        static_cast<long long>(oldValue) +
        static_cast<long long>(direction > 0 ? 1 : -1) * m_increment;

    int newValue;
    if ( candidate < low )
        newValue = HasFlag(wxSP_WRAP) ? high : low;
    else if ( candidate > high )
        newValue = HasFlag(wxSP_WRAP) ? low : high;
    else
        newValue = static_cast<int>(candidate);

    if ( newValue == oldValue )
        return false;

    wxSpinEvent directional(
        direction > 0 ? wxEVT_SPIN_UP : wxEVT_SPIN_DOWN, GetId());
    directional.SetPosition(newValue);
    directional.SetEventObject(this);
    HandleWindowEvent(directional);

    wxSpinButton *owner = state->GetOwner(generation);
    if ( !owner || !directional.IsAllowed() )
        return false;

    // A veto handler may have changed the range synchronously. Never commit a
    // proposal computed from stale bounds outside the now-live model.
    newValue = wxWinUIClampSpinValue(
        newValue, owner->m_min, owner->m_max);
    if ( newValue == owner->m_value )
        return false;
    owner->m_value = newValue;

    wxSpinEvent changed(wxEVT_SPIN, owner->GetId());
    changed.SetPosition(newValue);
    changed.SetEventObject(owner);
    owner->HandleWindowEvent(changed);
    // No access to owner here: the final handler may have destroyed it.
    return true;
}

#ifdef WXWINUI_TEST_SUPPORT
bool wxWinUIRangeTestAccess::Step(wxSpinButton& spin, int direction)
{
    return spin.Step(direction);
}

bool wxWinUIRangeTestAccess::GetPeerLayout(const wxSpinButton& spin,
                                           bool *vertical,
                                           unsigned *rows,
                                           unsigned *columns)
{
    if ( !spin.m_winui || !spin.m_winui->grid )
        return false;

    try
    {
        if ( vertical )
            *vertical = spin.IsVertical();
        if ( rows )
            *rows = spin.m_winui->grid.RowDefinitions().Size();
        if ( columns )
            *columns = spin.m_winui->grid.ColumnDefinitions().Size();
        return true;
    }
    catch ( const winrt::hresult_error& )
    {
        return false;
    }
}

bool wxWinUIRangeTestAccess::GetAutomation(
    const wxSpinButton& spin,
    int *rootControlType,
    wxString *rootClassName,
    wxString *incrementId,
    wxString *decrementId,
    wxString *incrementName,
    wxString *decrementName)
{
    if ( !spin.m_winui || !spin.m_winui->grid || !spin.m_winui->up ||
         !spin.m_winui->down )
    {
        return false;
    }

    try
    {
        MUXAP::AutomationPeer peer =
            MUXAP::FrameworkElementAutomationPeer::CreatePeerForElement(
                spin.m_winui->grid);
        if ( !peer )
        {
            spin.m_winui->grid.UpdateLayout();
            peer =
                MUXAP::FrameworkElementAutomationPeer::
                    CreatePeerForElement(spin.m_winui->grid);
        }
        if ( !peer )
            return false;

        if ( rootControlType )
        {
            *rootControlType =
                static_cast<int>(peer.GetAutomationControlType());
        }
        if ( rootClassName )
            *rootClassName = wxString(peer.GetClassName().c_str());
        if ( incrementId )
        {
            *incrementId = wxString(
                MUXA::AutomationProperties::GetAutomationId(
                    spin.m_winui->up).c_str());
        }
        if ( decrementId )
        {
            *decrementId = wxString(
                MUXA::AutomationProperties::GetAutomationId(
                    spin.m_winui->down).c_str());
        }
        if ( incrementName )
        {
            *incrementName = wxString(
                MUXA::AutomationProperties::GetName(
                    spin.m_winui->up).c_str());
        }
        if ( decrementName )
        {
            *decrementName = wxString(
                MUXA::AutomationProperties::GetName(
                    spin.m_winui->down).c_str());
        }
        return true;
    }
    catch ( const winrt::hresult_error& )
    {
        return false;
    }
}

bool wxWinUIRangeTestAccess::InvokeArrow(wxSpinButton& spin, int direction)
{
    if ( !spin.m_winui || !spin.m_winui->callbackState || direction == 0 )
        return false;

    const auto state = spin.m_winui->callbackState;
    const std::uint64_t generation = state->Generation();
    wxWinUISpinButtonImpl * const impl = spin.m_winui.get();
    const auto getCurrentOwner =
        [state, generation, impl]() -> wxSpinButton *
        {
            wxSpinButton * const owner = state->GetOwner(generation);
            if ( !owner || !owner->m_winui ||
                 owner->m_winui.get() != impl ||
                 owner->m_winui->callbackState != state )
            {
                return nullptr;
            }
            return owner;
        };

    if ( !getCurrentOwner() )
        return false;

    const MUXCP::RepeatButton button =
        direction > 0 ? impl->up : impl->down;
    if ( !button )
        return false;

    try
    {
        // A peer constructed directly for deterministic tests doesn't pass
        // through the external UIA provider's availability guard. Preserve
        // both production guards explicitly: wx rejects the action from an
        // effectively disabled control and the peer must also report itself
        // enabled before the provider action can be exercised.
        wxSpinButton * const owner = getCurrentOwner();
        if ( !owner || !owner->IsEnabled() )
            return false;

        MUXAP::RepeatButtonAutomationPeer peer(button);
        const bool peerEnabled = peer.IsEnabled();
        if ( !getCurrentOwner() || !peerEnabled )
            return false;

        peer.Invoke();
        // UIA invocation synchronously dispatches wx events and can delete the
        // control. Never inspect spin or its implementation after Invoke().
        return true;
    }
    catch ( const winrt::hresult_error& e )
    {
        // Disabled controls are required to reject the UIA action. Avoid a
        // warning for that expected contract; log only live, enabled failures.
        if ( wxSpinButton * const owner = getCurrentOwner() )
        {
            const bool ownerEnabled = owner->IsEnabled();
            if ( ownerEnabled && getCurrentOwner() )
            {
                wxWinUILogException(
                    "WinUI SpinButton UIA arrow invoke", e);
            }
        }
        return false;
    }
}
#endif // WXWINUI_TEST_SUPPORT

#endif // wxUSE_SPINBTN
