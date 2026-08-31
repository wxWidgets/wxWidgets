/////////////////////////////////////////////////////////////////////////////
// Name:        src/winui/hyperlink.cpp
// Purpose:     wxWinUI wxHyperlinkCtrl implementation (WinUI HyperlinkButton)
// Author:      wxWidgets development team
// Created:     2026-06-01
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_HYPERLINKCTRL

#include "wx/hyperlink.h"

#include "wx/app.h"
#include "wx/clipbrd.h"
#include "wx/dataobj.h"
#include "wx/log.h"
#if wxUSE_MENUS
    #include "wx/menu.h"
#endif
#include "wx/utils.h"
#include "wx/private/hyperlink.h"
#include "wx/winui/winui.h"

#include "private.h"

#ifdef WXWINUI_TEST_SUPPORT
#include "feedback-test-access.h"
#endif
#include "wx/winui/private/appearance.h"

#include <winrt/Microsoft.UI.Xaml.Automation.h>
#include <winrt/Microsoft.UI.Xaml.Automation.Peers.h>
#include <winrt/Microsoft.UI.Xaml.Automation.Provider.h>

#include <atomic>
#include <cmath>
#include <cstdint>
#include <memory>
#include <utility>
#include <vector>

namespace MUX = winrt::Microsoft::UI::Xaml;
namespace MUXA = winrt::Microsoft::UI::Xaml::Automation;
namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;
namespace MUXI = winrt::Microsoft::UI::Xaml::Input;
namespace MUXM = winrt::Microsoft::UI::Xaml::Media;

namespace
{

#ifdef WXWINUI_TEST_SUPPORT
std::atomic<unsigned> gs_liveHyperlinkCallbackStates{ 0 };
#endif

wxColour wxWinUIGetDefaultHyperlinkColour()
{
    // wxPrivate::GetLinkColour() is the generic contract for the system
    // appearance. Honour an explicit WinUI application theme as the same
    // generic light/dark choice so forcing Dark never leaves a light-only
    // default brush behind (and vice versa).
    switch ( wxWinUIGetAppTheme() )
    {
        case wxWinUIAppTheme::Light:
            return wxColour(0x00, 0x00, 0xee);

        case wxWinUIAppTheme::Dark:
            return wxColour(0x87, 0xce, 0xfa);

        case wxWinUIAppTheme::System:
            return wxPrivate::GetLinkColour();
    }

    wxFAIL_MSG("unknown WinUI application theme");
    return wxPrivate::GetLinkColour();
}

class wxWinUIHyperlinkCallbackState
{
public:
    static constexpr unsigned MaxApplyPassBudget = 16;

    explicit wxWinUIHyperlinkCallbackState(wxHyperlinkCtrl *owner)
        : m_owner(owner)
    {
#ifdef WXWINUI_TEST_SUPPORT
        ++gs_liveHyperlinkCallbackStates;
#endif
    }

    ~wxWinUIHyperlinkCallbackState()
    {
#ifdef WXWINUI_TEST_SUPPORT
        --gs_liveHyperlinkCallbackStates;
#endif
    }

    std::uint64_t GetGeneration() const
    {
        return m_generation.load(std::memory_order_acquire);
    }

    wxHyperlinkCtrl *GetOwner(std::uint64_t generation) const
    {
        if ( generation != GetGeneration() )
            return nullptr;

        wxHyperlinkCtrl * const owner =
            m_owner.load(std::memory_order_acquire);
        return generation == GetGeneration() ? owner : nullptr;
    }

    void Invalidate()
    {
        m_owner.store(nullptr, std::memory_order_release);
        m_generation.fetch_add(1, std::memory_order_acq_rel);
        BumpModelRevision();
        RequestApply();
        CancelDeferredApply();
        QuarantineApply();
    }

    std::uint64_t BumpModelRevision()
    {
        // Only a mutation which begins outside the active projection is a new
        // request. A reentrant mutation produced by a peer write belongs to
        // the current transaction and must not refresh its global budget.
        if ( !m_applyActive.load(std::memory_order_acquire) )
        {
            m_applyPassesRemaining.store(
                MaxApplyPassBudget, std::memory_order_release);
            m_applyQuarantined.store(false, std::memory_order_release);
            ClearApplyBudgetWarning();
        }

        std::uint64_t revision =
            m_modelRevision.fetch_add(1, std::memory_order_acq_rel) + 1;
        if ( !revision )
        {
            revision =
                m_modelRevision.fetch_add(1, std::memory_order_acq_rel) + 1;
        }
        return revision;
    }

    std::uint64_t GetModelRevision() const
    {
        return m_modelRevision.load(std::memory_order_acquire);
    }

    bool TryBeginApply()
    {
        bool expected = false;
        if ( !m_applyActive.compare_exchange_strong(
                 expected, true, std::memory_order_acq_rel) )
        {
            RequestApply();
            return false;
        }
        return true;
    }

    void EndApply()
    {
        m_applyActive.store(false, std::memory_order_release);
    }

    void RequestApply()
    {
        m_applyRequested.store(true, std::memory_order_release);
    }

    bool ConsumeApplyRequest()
    {
        return m_applyRequested.exchange(false, std::memory_order_acq_rel);
    }

    bool TryScheduleDeferredApply()
    {
        bool expected = false;
        return m_deferredApplyScheduled.compare_exchange_strong(
            expected, true, std::memory_order_acq_rel);
    }

    bool ConsumeDeferredApply()
    {
        return m_deferredApplyScheduled.exchange(
            false, std::memory_order_acq_rel);
    }

    void CancelDeferredApply()
    {
        m_deferredApplyScheduled.store(false, std::memory_order_release);
    }

    bool HasDeferredApply() const
    {
        return m_deferredApplyScheduled.load(std::memory_order_acquire);
    }

    bool TryConsumeApplyPass()
    {
        unsigned remaining =
            m_applyPassesRemaining.load(std::memory_order_acquire);
        while ( remaining )
        {
            if ( m_applyPassesRemaining.compare_exchange_weak(
                     remaining, remaining - 1,
                     std::memory_order_acq_rel,
                     std::memory_order_acquire) )
            {
                return true;
            }
        }
        return false;
    }

    bool HasApplyPassesRemaining() const
    {
        return m_applyPassesRemaining.load(std::memory_order_acquire) != 0;
    }

    void QuarantineApply()
    {
        m_applyPassesRemaining.store(0, std::memory_order_release);
        m_applyRequested.store(false, std::memory_order_release);
        m_applyQuarantined.store(true, std::memory_order_release);
        CancelDeferredApply();
    }

    bool IsApplyQuarantined() const
    {
        return m_applyQuarantined.load(std::memory_order_acquire);
    }

    bool MarkApplyBudgetWarning()
    {
        return !m_applyBudgetWarning.exchange(
            true, std::memory_order_acq_rel);
    }

    void ClearApplyBudgetWarning()
    {
        m_applyBudgetWarning.store(false, std::memory_order_release);
    }

private:
    std::atomic<wxHyperlinkCtrl *> m_owner;
    std::atomic<std::uint64_t> m_generation{ 1 };
    std::atomic<std::uint64_t> m_modelRevision{ 1 };
    std::atomic<bool> m_applyActive{ false };
    std::atomic<bool> m_applyRequested{ false };
    std::atomic<bool> m_deferredApplyScheduled{ false };
    std::atomic<bool> m_applyBudgetWarning{ false };
    std::atomic<unsigned> m_applyPassesRemaining{ MaxApplyPassBudget };
    std::atomic<bool> m_applyQuarantined{ false };
};

class wxWinUIHyperlinkApplyGuard final
{
public:
    explicit wxWinUIHyperlinkApplyGuard(
        std::shared_ptr<wxWinUIHyperlinkCallbackState> state)
        : m_state(std::move(state)),
          m_acquired(m_state->TryBeginApply())
    {
    }

    ~wxWinUIHyperlinkApplyGuard()
    {
        if ( m_acquired )
            m_state->EndApply();
    }

    explicit operator bool() const
    {
        return m_acquired;
    }

private:
    std::shared_ptr<wxWinUIHyperlinkCallbackState> m_state;
    bool m_acquired;
};

} // namespace

class wxWinUIHyperlinkImpl
{
public:
    ~wxWinUIHyperlinkImpl()
    {
        Close();
    }

    void Close()
    {
        // SearchDynamicEventTable() explicitly supports deleting its
        // wxEvtHandler from a dynamic handler, but only when that handler is
        // considered to have handled the event. If it leaves Skip() set, the
        // event engine must continue with the just-deleted handler's static
        // table and chain. Mark every nested hyperlink dispatch handled
        // before wxEvtHandler is torn down. This also suppresses the default
        // browser fallback, which cannot safely run after its source died.
        for ( wxEvent * const event : activeEvents )
        {
            if ( event )
                event->Skip(false);
        }
        activeEvents.clear();

        if ( callbackState )
            callbackState->Invalidate();

#ifdef WXWINUI_TEST_SUPPORT
        nextPeerWriteHookForTesting = nullptr;
        nextPeerWriteContextForTesting = nullptr;
#endif

        if ( button )
        {
            const auto revoke =
                [this](winrt::event_token& token,
                       const char *what,
                       const auto& remover)
                {
                    if ( !token.value )
                        return;
                    try
                    {
                        remover(token);
                    }
                    catch ( const winrt::hresult_error& e )
                    {
                        wxWinUILogException(what, e);
                    }
                    token = {};
                };

            revoke(clickToken, "HyperlinkButton::Click removal",
                   [this](winrt::event_token token)
                   {
                       button.Click(token);
                   });
            revoke(pointerEnteredToken,
                   "HyperlinkButton::PointerEntered removal",
                   [this](winrt::event_token token)
                   {
                       button.PointerEntered(token);
                   });
            revoke(pointerExitedToken,
                   "HyperlinkButton::PointerExited removal",
                   [this](winrt::event_token token)
                   {
                       button.PointerExited(token);
                   });
            revoke(contextRequestedToken,
                   "HyperlinkButton::ContextRequested removal",
                   [this](winrt::event_token token)
                   {
                       button.ContextRequested(token);
                   });
            revoke(actualThemeChangedToken,
                   "HyperlinkButton::ActualThemeChanged removal",
                   [this](winrt::event_token token)
                   {
                       button.ActualThemeChanged(token);
                   });
        }

        // Revoke and invalidate every callback before disconnecting the slot.
        host.Close();
        button = nullptr;
        callbackState.reset();
    }

    wxWinUIControlHost host;
    MUXC::HyperlinkButton button{ nullptr };
    std::shared_ptr<wxWinUIHyperlinkCallbackState> callbackState;
    winrt::event_token clickToken{};
    winrt::event_token pointerEnteredToken{};
    winrt::event_token pointerExitedToken{};
    winrt::event_token contextRequestedToken{};
    winrt::event_token actualThemeChangedToken{};
    std::vector<wxEvent *> activeEvents;
#ifdef WXWINUI_TEST_SUPPORT
    using PeerWriteHook = void (*)(void *);
    PeerWriteHook nextPeerWriteHookForTesting = nullptr;
    void *nextPeerWriteContextForTesting = nullptr;
#endif
};

wxHyperlinkCtrl::wxHyperlinkCtrl()
    : m_hoverColour(*wxRED),
      m_normalColour(wxWinUIGetDefaultHyperlinkColour()),
      m_visitedColour("#551a8b")
{
}

wxHyperlinkCtrl::wxHyperlinkCtrl(wxWindow *parent,
                                 wxWindowID id,
                                 const wxString& label,
                                 const wxString& url,
                                 const wxPoint& pos,
                                 const wxSize& size,
                                 long style,
                                 const wxString& name)
    : wxHyperlinkCtrl()
{
    Create(parent, id, label, url, pos, size, style, name);
}

wxHyperlinkCtrl::~wxHyperlinkCtrl()
{
    if ( m_winui )
        m_winui->Close();
}

bool wxHyperlinkCtrl::Create(wxWindow *parent,
                             wxWindowID id,
                             const wxString& label,
                             const wxString& url,
                             const wxPoint& pos,
                             const wxSize& size,
                             long style,
                             const wxString& name)
{
    CheckParams(label, url, style);
    style = (style & ~wxBORDER_MASK) | wxBORDER_NONE;

    if ( !wxControl::Create(
             parent, id, pos, size, style, wxDefaultValidator, name) )
    {
        return false;
    }

    m_url = url.empty() ? label : url;
    wxControl::SetLabel(label.empty() ? url : label);
    m_visited = false;
    m_pointerOver = false;
    m_normalColour = wxWinUIGetDefaultHyperlinkColour();
    m_hoverColour = *wxRED;
    m_visitedColour = wxColour("#551a8b");
    m_hasCustomNormalColour = false;
    m_hasCustomHoverColour = false;
    m_hasCustomVisitedColour = false;
    Unbind(wxEVT_SYS_COLOUR_CHANGED,
           &wxHyperlinkCtrl::OnSysColourChanged, this);
    Bind(wxEVT_SYS_COLOUR_CHANGED,
         &wxHyperlinkCtrl::OnSysColourChanged, this);
    m_winui.reset(new wxWinUIHyperlinkImpl);
    if ( !m_winui->host.Initialize(this) )
    {
        m_winui.reset();
        return false;
    }

    wxWinUIHyperlinkImpl * const impl = m_winui.get();
    wxWeakRef<wxHyperlinkCtrl> alive(this);
    const auto abandonCreate = [&]()
    {
        if ( alive && alive->m_winui.get() == impl )
            alive->m_winui.reset();
        return false;
    };
    try
    {
        impl->button = MUXC::HyperlinkButton();
        // A wxHyperlinkCtrl can be wider than its label. Keep the native
        // HyperlinkButton itself content-sized so XAML pointer, hover,
        // context-menu and UIA invoke semantics all share the same label-only
        // interactive area as wxGenericHyperlinkCtrl.
        impl->button.Padding(
            MUX::ThicknessHelper::FromUniformLength(0));
        impl->button.MinWidth(0);
        impl->button.MinHeight(0);
        impl->button.VerticalAlignment(
            MUX::VerticalAlignment::Center);
        impl->button.VerticalContentAlignment(
            MUX::VerticalAlignment::Center);

        impl->callbackState =
            std::make_shared<wxWinUIHyperlinkCallbackState>(this);
        const auto callbackState = impl->callbackState;
        const std::uint64_t generation =
            callbackState->GetGeneration();

        impl->clickToken = impl->button.Click(
            [callbackState, generation](
                winrt::Windows::Foundation::IInspectable const&,
                MUX::RoutedEventArgs const&)
            {
                if ( wxHyperlinkCtrl * const owner =
                         callbackState->GetOwner(generation) )
                {
                    owner->HandleClick();
                }
            });

        impl->pointerEnteredToken = impl->button.PointerEntered(
            [callbackState, generation](
                winrt::Windows::Foundation::IInspectable const&,
                MUXI::PointerRoutedEventArgs const&)
            {
                if ( wxHyperlinkCtrl * const owner =
                         callbackState->GetOwner(generation) )
                {
                    owner->SetPointerOver(true);
                }
            });

        impl->pointerExitedToken = impl->button.PointerExited(
            [callbackState, generation](
                winrt::Windows::Foundation::IInspectable const&,
                MUXI::PointerRoutedEventArgs const&)
            {
                if ( wxHyperlinkCtrl * const owner =
                         callbackState->GetOwner(generation) )
                {
                    owner->SetPointerOver(false);
                }
            });

        impl->actualThemeChangedToken = impl->button.ActualThemeChanged(
            [callbackState, generation](
                MUX::FrameworkElement const&,
                winrt::Windows::Foundation::IInspectable const&)
            {
                if ( wxHyperlinkCtrl * const owner =
                         callbackState->GetOwner(generation) )
                {
                    owner->HandleThemeChanged();
                }
            });

#if wxUSE_MENUS
        if ( style & wxHL_CONTEXTMENU )
        {
            impl->contextRequestedToken =
                impl->button.ContextRequested(
                    [callbackState, generation](
                        winrt::Windows::Foundation::IInspectable const&,
                        MUXI::ContextRequestedEventArgs const& event)
                    {
                        // Claim the request before entering PopupMenu(): an
                        // event handler may destroy the hyperlink while the
                        // native menu is active.
                        event.Handled(true);
                        if ( wxHyperlinkCtrl * const owner =
                                 callbackState->GetOwner(generation) )
                        {
                            owner->HandleContextRequested();
                        }
                    });
        }
#endif

        if ( !UpdateWinUIContent() )
            return abandonCreate();
        if ( !alive || !alive->m_winui ||
             alive->m_winui.get() != impl ||
             alive->m_winui->callbackState != callbackState )
        {
            return false;
        }
        if ( !impl->host.SetContent(impl->button) )
            return abandonCreate();
        wxHyperlinkCtrl * const owner =
            callbackState->GetOwner(generation);
        if ( !owner || !owner->m_winui ||
             owner->m_winui.get() != impl ||
             owner->m_winui->callbackState != callbackState )
        {
            return false;
        }
        impl->host.SetAutomationNameOverride(owner->GetLabel());
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI HyperlinkButton creation", e);
        return abandonCreate();
    }

    wxHyperlinkCtrl * const owner = alive.get();
    if ( !owner || !owner->m_winui ||
         owner->m_winui.get() != impl )
    {
        return false;
    }
    owner->SetInitialSize(size);
    return alive && alive->m_winui &&
           alive->m_winui.get() == impl;
}

void wxHyperlinkCtrl::SetHoverColour(const wxColour& colour)
{
    m_hasCustomHoverColour = colour.IsOk();
    m_hoverColour =
        m_hasCustomHoverColour ? colour : *wxRED;
    BumpWinUIModelRevision();
    UpdateWinUIContent();
}

wxColour wxHyperlinkCtrl::GetNormalColour() const
{
    // Keep the default observable through the wx API in lockstep with the
    // brush projected below. Unlike a cached Create()-time value, the class
    // default follows a light/dark appearance change.
    return m_hasCustomNormalColour
        ? m_normalColour
        : wxWinUIGetDefaultHyperlinkColour();
}

void wxHyperlinkCtrl::SetNormalColour(const wxColour& colour)
{
    m_hasCustomNormalColour = colour.IsOk();
    m_normalColour =
        m_hasCustomNormalColour
            ? colour
            : wxWinUIGetDefaultHyperlinkColour();
    BumpWinUIModelRevision();
    UpdateWinUIContent();
}

void wxHyperlinkCtrl::SetVisitedColour(const wxColour& colour)
{
    m_hasCustomVisitedColour = colour.IsOk();
    m_visitedColour =
        m_hasCustomVisitedColour ? colour : wxColour("#551a8b");
    BumpWinUIModelRevision();
    UpdateWinUIContent();
}

void wxHyperlinkCtrl::SetURL(const wxString& url)
{
    if ( m_url == url )
        return;

    // Match the MSW contract: navigating the same control to another URL
    // makes it unvisited again. Publish both model changes before bumping the
    // revision, so a synchronous peer callback can only observe the old pair
    // or the new pair, and the existing projection loop remains
    // last-writer-wins under reentrancy.
    m_url = url;
    m_visited = false;
    BumpWinUIModelRevision();
    UpdateWinUIContent();
}

void wxHyperlinkCtrl::SetVisited(bool visited)
{
    if ( m_visited == visited )
        return;
    m_visited = visited;
    BumpWinUIModelRevision();
    UpdateWinUIContent();
}

void wxHyperlinkCtrl::SetLabel(const wxString& label)
{
    wxControl::SetLabel(label);
    InvalidateBestSize();
    BumpWinUIModelRevision();
    wxWeakRef<wxHyperlinkCtrl> alive(this);
    UpdateWinUIContent();
    wxHyperlinkCtrl * const live = alive.get();
    if ( !live )
        return;
    if ( live->m_winui )
        live->m_winui->host.SetAutomationNameOverride(live->GetLabel());

    if ( live->GetParent() && live->GetParent()->GetSizer() )
        live->GetParent()->Layout();
}

bool wxHyperlinkCtrl::SetFont(const wxFont& font)
{
    const bool changed = wxControl::SetFont(font);
    InvalidateBestSize();
    BumpWinUIModelRevision();
    UpdateWinUIContent();
    return changed;
}

bool wxHyperlinkCtrl::SetForegroundColour(const wxColour& colour)
{
    const bool changed = wxControl::SetForegroundColour(colour);
    m_hasCustomNormalColour = colour.IsOk();
    m_normalColour =
        m_hasCustomNormalColour
            ? colour
            : wxWinUIGetDefaultHyperlinkColour();
    BumpWinUIModelRevision();
    UpdateWinUIContent();
    return changed;
}

bool wxHyperlinkCtrl::SetBackgroundColour(const wxColour& colour)
{
    const bool changed = wxControl::SetBackgroundColour(colour);
    BumpWinUIModelRevision();
    UpdateWinUIContent();
    return changed;
}

wxVisualAttributes wxHyperlinkCtrl::GetDefaultAttributes() const
{
    return GetClassDefaultAttributes(GetWindowVariant());
}

/* static */
wxVisualAttributes
wxHyperlinkCtrl::GetClassDefaultAttributes(wxWindowVariant variant)
{
    // This native peer deliberately exposes the same wx defaults as the
    // documented generic implementation. The values are explicitly projected
    // to XAML, instead of returning generic colours while silently displaying
    // unrelated HyperlinkButton theme-resource colours.
    auto attrs = wxHyperlinkCtrlBase::GetClassDefaultAttributes(variant);
    attrs.colFg = wxWinUIGetDefaultHyperlinkColour();
    return attrs;
}

wxSize wxHyperlinkCtrl::DoGetBestSize() const
{
    if ( m_winui && m_winui->host.IsContentLoaded() )
    {
        const wxSize measured = m_winui->host.MeasureContent();
        if ( measured != wxDefaultSize )
            return measured;
    }

    const wxString text = wxControl::GetLabelText(GetLabel());
    if ( text.empty() )
        return wxControl::DoGetBestSize();

    wxSize best = GetTextExtent(text);
    best.x += FromDIP(22);
    best.y += FromDIP(12);
    return best;
}

void wxHyperlinkCtrl::BumpWinUIModelRevision()
{
    if ( m_winui && m_winui->callbackState )
        m_winui->callbackState->BumpModelRevision();
}

bool wxHyperlinkCtrl::UpdateWinUIContent()
{
    if ( !m_winui || !m_winui->button || !m_winui->callbackState )
        return false;

    wxWinUIHyperlinkImpl * const impl = m_winui.get();
    const std::shared_ptr<wxWinUIHyperlinkCallbackState> callbackState =
        impl->callbackState;
    const std::uint64_t lifetimeGeneration =
        callbackState->GetGeneration();
    const wxWinUIHyperlinkApplyGuard applyGuard(callbackState);
    if ( !applyGuard )
    {
        // The active transaction will observe the request bit and the model
        // revision changed by this nested caller. Never recursively project a
        // partially written dependency-property set.
        return true;
    }

    const auto button = impl->button;

    enum class ApplyState
    {
        Current,
        Restart,
        Dead
    };

    constexpr unsigned MaxSynchronousApplyPasses = 8;
    const auto quarantineLatestRevision =
        [&]() -> bool
        {
            callbackState->QuarantineApply();
            if ( callbackState->MarkApplyBudgetWarning() )
            {
                wxLogWarning(
                    "WinUI HyperlinkButton model kept changing during "
                    "projection; quarantining this request until the next "
                    "external model mutation.");
            }
            return false;
        };
    const auto deferLatestRevision =
        [&]() -> bool
        {
            if ( !callbackState->HasApplyPassesRemaining() )
                return quarantineLatestRevision();

            callbackState->RequestApply();
            if ( callbackState->HasDeferredApply() )
                return true;

            if ( !wxTheApp )
                return quarantineLatestRevision();

            // Logging can invoke an application log target which yields the
            // event loop. Do it before making the replay ticket consumable.
            if ( callbackState->MarkApplyBudgetWarning() )
            {
                wxLogWarning(
                    "WinUI HyperlinkButton model kept changing during "
                    "projection; using its one bounded asynchronous replay.");
            }

            if ( !callbackState->TryScheduleDeferredApply() )
            {
                return callbackState->HasDeferredApply()
                    ? true
                    : quarantineLatestRevision();
            }

            const std::weak_ptr<wxWinUIHyperlinkCallbackState>
                weakState(callbackState);
            const std::uint64_t scheduledRevision =
                callbackState->GetModelRevision();

            // Keep posting as the final operation before the active projection
            // guard unwinds.
            wxTheApp->CallAfter(
                [weakState, lifetimeGeneration, scheduledRevision, impl]()
                {
                    const std::shared_ptr<
                        wxWinUIHyperlinkCallbackState> state =
                            weakState.lock();
                    if ( !state || !state->ConsumeDeferredApply() )
                        return;

                    wxHyperlinkCtrl * const owner =
                        state->GetOwner(lifetimeGeneration);
                    if ( !owner || !owner->m_winui ||
                         owner->m_winui.get() != impl ||
                         owner->m_winui->callbackState != state )
                    {
                        return;
                    }

                    // A newer revision supersedes the one which exhausted
                    // the synchronous budget. UpdateWinUIContent() always
                    // snapshots the current revision, so this callback is
                    // deliberately last-writer-wins rather than tied to a
                    // stale snapshot.
                    if ( state->GetModelRevision() != scheduledRevision )
                        state->RequestApply();
                    owner->UpdateWinUIContent();
                });
            return true;
        };

    for ( unsigned pass = 0;
          pass < MaxSynchronousApplyPasses;
          ++pass )
    {
        if ( !callbackState->TryConsumeApplyPass() )
            return quarantineLatestRevision();

        callbackState->ConsumeApplyRequest();
        wxHyperlinkCtrl * const live =
            callbackState->GetOwner(lifetimeGeneration);
        if ( !live || !live->m_winui ||
             live->m_winui.get() != impl ||
             live->m_winui->callbackState != callbackState )
        {
            return false;
        }

        const std::uint64_t revision =
            callbackState->GetModelRevision();
        const wxString rawLabel = live->GetLabel();
        const wxWinUILabelPresentation label =
            wxWinUIParseLabel(rawLabel);
        const wxString url = live->m_url;
        MUX::HorizontalAlignment alignment =
            MUX::HorizontalAlignment::Center;
        if ( live->HasFlag(wxHL_ALIGN_LEFT) )
            alignment = MUX::HorizontalAlignment::Left;
        else if ( live->HasFlag(wxHL_ALIGN_RIGHT) )
            alignment = MUX::HorizontalAlignment::Right;
        const wxFont font =
            live->m_hasFont ? live->GetFont() : wxNullFont;
        const wxColour background =
            live->m_hasBgCol
                ? live->GetBackgroundColour()
                : wxNullColour;
        wxColour effective;
        bool effectiveIsCustom = false;
        if ( live->m_pointerOver )
        {
            effective = live->GetHoverColour();
            effectiveIsCustom = live->m_hasCustomHoverColour;
        }
        else if ( live->m_visited )
        {
            effective = live->GetVisitedColour();
            effectiveIsCustom = live->m_hasCustomVisitedColour;
        }
        else
        {
            effective = live->GetNormalColour();
            effectiveIsCustom = live->m_hasCustomNormalColour;
        }
        if ( wxWinUIIsHighContrastActive() && !effectiveIsCustom )
            effective = wxNullColour;

        const auto checkState =
            [callbackState, lifetimeGeneration, impl, revision]()
            {
                wxHyperlinkCtrl * const current =
                    callbackState->GetOwner(lifetimeGeneration);
                if ( !current || !current->m_winui ||
                     current->m_winui.get() != impl ||
                     current->m_winui->callbackState != callbackState )
                {
                    return ApplyState::Dead;
                }
                return callbackState->GetModelRevision() == revision
                    ? ApplyState::Current
                    : ApplyState::Restart;
            };

#ifdef WXWINUI_TEST_SUPPORT
        const wxWinUIHyperlinkImpl::PeerWriteHook hook =
            impl->nextPeerWriteHookForTesting;
        void * const hookContext =
            impl->nextPeerWriteContextForTesting;
        impl->nextPeerWriteHookForTesting = nullptr;
        impl->nextPeerWriteContextForTesting = nullptr;
#endif

        try
        {
            button.Content(
                winrt::box_value(wxWinUIToHString(label.text)));
            ApplyState state = checkState();
            if ( state == ApplyState::Dead )
                return false;
            if ( state == ApplyState::Restart )
                continue;

#ifdef WXWINUI_TEST_SUPPORT
            if ( hook )
            {
                hook(hookContext);
                state = checkState();
                if ( state == ApplyState::Dead )
                    return false;
                if ( state == ApplyState::Restart )
                    continue;
            }
#endif

            wxWinUIApplyAccessKey(button, rawLabel);
            state = checkState();
            if ( state == ApplyState::Dead )
                return false;
            if ( state == ApplyState::Restart )
                continue;

            MUXA::AutomationProperties::SetHelpText(
                button, wxWinUIToHString(url));
            state = checkState();
            if ( state == ApplyState::Dead )
                return false;
            if ( state == ApplyState::Restart )
                continue;

            button.HorizontalContentAlignment(alignment);
            state = checkState();
            if ( state == ApplyState::Dead )
                return false;
            if ( state == ApplyState::Restart )
                continue;

            // The shared slot stretches its content by default. Override the
            // element alignment as well as its content alignment, otherwise
            // the full wx client rectangle remains a clickable hyperlink.
            button.HorizontalAlignment(alignment);
            state = checkState();
            if ( state == ApplyState::Dead )
                return false;
            if ( state == ApplyState::Restart )
                continue;

            wxWinUIApplyFont(button, font);
            state = checkState();
            if ( state == ApplyState::Dead )
                return false;
            if ( state == ApplyState::Restart )
                continue;

            wxWinUIApplyBackground(button, background);
            state = checkState();
            if ( state == ApplyState::Dead )
                return false;
            if ( state == ApplyState::Restart )
                continue;

            wxWinUIApplyForeground(button, effective);
            state = checkState();
            if ( state == ApplyState::Dead )
                return false;
            if ( state == ApplyState::Restart )
                continue;

            impl->host.ForceRender();
            state = checkState();
            if ( state == ApplyState::Dead )
                return false;
            if ( state == ApplyState::Restart )
                continue;

            if ( callbackState->ConsumeApplyRequest() )
                continue;

            callbackState->CancelDeferredApply();
            callbackState->ClearApplyBudgetWarning();
            return true;
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException("WinUI HyperlinkButton content", e);
            return false;
        }
    }

    return deferLatestRevision();
}

void wxHyperlinkCtrl::HandleClick()
{
    wxWeakRef<wxHyperlinkCtrl> alive(this);
    SetVisited(true);
    if ( !alive )
        return;

    // wxHyperlinkCtrlBase::SendEvent() keeps a reference to the control's URL
    // across ProcessEvent(). Keep an owned copy instead: deleting the source
    // from its handler is explicitly supported by this port.
    const wxString url = m_url;
    wxHyperlinkEvent event(this, GetId(), url);
    wxWinUIHyperlinkImpl * const impl = m_winui.get();
    impl->activeEvents.push_back(&event);
    const bool handled = HandleWindowEvent(event);

    // ProcessEvent() is a deletion boundary. Close() has already made the
    // event handled and cleared the dispatch stack if this generation died
    // (or was replaced), so never dereference the stale impl in that case.
    wxHyperlinkCtrl * const live = alive.get();
    if ( !live || !live->m_winui || live->m_winui.get() != impl )
        return;

    wxASSERT_MSG(!impl->activeEvents.empty() &&
                     impl->activeEvents.back() == &event,
                 "unbalanced WinUI hyperlink event dispatch");
    if ( !impl->activeEvents.empty() )
        impl->activeEvents.pop_back();

    if ( handled )
        return;

    if ( !wxLaunchDefaultBrowser(url) )
    {
        wxLogWarning(
            _("Failed to open URL \"%s\" in the default browser"), url);
    }
}

void wxHyperlinkCtrl::HandleContextRequested()
{
#if wxUSE_MENUS
    if ( !HasFlag(wxHL_CONTEXTMENU) || !m_winui ||
         !m_winui->callbackState )
    {
        return;
    }

    constexpr int copyUrlId = wxID_HIGHEST + 817;
    wxMenu menu;
    menu.Append(copyUrlId, _("&Copy URL"));

    const auto state = m_winui->callbackState;
    const std::uint64_t generation = state->GetGeneration();
    menu.Bind(
        wxEVT_MENU,
        [state, generation](wxCommandEvent&)
        {
            if ( wxHyperlinkCtrl * const owner =
                     state->GetOwner(generation) )
            {
                owner->CopyURLToClipboard();
            }
        },
        copyUrlId);

    // PopupMenu() is a reentrancy boundary. Nothing may access this after it.
    PopupMenu(&menu);
#endif // wxUSE_MENUS
}

bool wxHyperlinkCtrl::CopyURLToClipboard()
{
#if wxUSE_CLIPBOARD
    const wxString url = m_url;
    if ( !wxTheClipboard->Open() )
        return false;

    const bool copied =
        wxTheClipboard->SetData(new wxTextDataObject(url));
    wxTheClipboard->Close();
    return copied;
#else
    return false;
#endif
}

#ifdef WXWINUI_TEST_SUPPORT
bool wxWinUIHyperlinkTestAccess::Invoke(
    wxHyperlinkCtrl& link)
{
    if ( !link.m_winui || !link.m_winui->button )
        return false;

    const auto button = link.m_winui->button;
    try
    {
        winrt::Microsoft::UI::Xaml::Automation::Peers::
            HyperlinkButtonAutomationPeer peer(button);
        peer.Invoke();
        // Invoke() may synchronously destroy this. Do not inspect members.
        return true;
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI HyperlinkButton invoke", e);
        return false;
    }
}

bool wxWinUIHyperlinkTestAccess::GetInteractiveRect(
    const wxHyperlinkCtrl& link,
    wxRect *rect)
{
    if ( !rect )
        return false;

    *rect = wxRect();
    if ( !link.m_winui || !link.m_winui->button || !link.m_winui->callbackState )
        return false;

    wxWinUIHyperlinkImpl * const impl = link.m_winui.get();
    const auto callbackState = impl->callbackState;
    const std::uint64_t generation = callbackState->GetGeneration();
    const auto button = impl->button;

    try
    {
        impl->host.ForceRender();
        wxHyperlinkCtrl * const live =
            callbackState->GetOwner(generation);
        if ( !live || live != &link || !live->m_winui ||
             live->m_winui.get() != impl ||
             live->m_winui->callbackState != callbackState ||
             live->m_winui->button != button )
        {
            return false;
        }

        const double actualWidth = button.ActualWidth();
        const double actualHeight = button.ActualHeight();
        if ( actualWidth <= 0.0 || actualHeight <= 0.0 )
            return false;

        const auto xamlRoot = button.XamlRoot();
        const auto rootContent =
            xamlRoot ? xamlRoot.Content() : MUX::UIElement{ nullptr };
        if ( !rootContent )
            return false;

        const auto transform = button.TransformToVisual(rootContent);
        if ( !transform )
            return false;
        const auto bounds = transform.TransformBounds(
            winrt::Windows::Foundation::Rect{
                0.0f,
                0.0f,
                static_cast<float>(actualWidth),
                static_cast<float>(actualHeight)});
        const double scale = xamlRoot.RasterizationScale();
        if ( scale <= 0.0 )
            return false;

        HWND const bridge = impl->host.GetBridgeHWND();
        POINT bridgeOrigin{ 0, 0 };
        if ( !bridge || !::ClientToScreen(bridge, &bridgeOrigin) )
            return false;
        const wxPoint controlOrigin =
            live->ClientToScreen(wxPoint(0, 0));

        // Transform the actual peer rectangle into the XamlRoot, then convert
        // its edges (not origin and size independently) from DIPs to physical
        // pixels. Finally translate from the shared island to wx client
        // coordinates. This observes the real XAML alignment and stays exact
        // at fractional DPI.
        const int left = bridgeOrigin.x +
            static_cast<int>(std::lround(bounds.X * scale)) -
            controlOrigin.x;
        const int top = bridgeOrigin.y +
            static_cast<int>(std::lround(bounds.Y * scale)) -
            controlOrigin.y;
        const int right = bridgeOrigin.x +
            static_cast<int>(
                std::lround((bounds.X + bounds.Width) * scale)) -
            controlOrigin.x;
        const int bottom = bridgeOrigin.y +
            static_cast<int>(
                std::lround((bounds.Y + bounds.Height) * scale)) -
            controlOrigin.y;

        if ( right <= left || bottom <= top )
        {
            return false;
        }

        wxRect interactive(
            left, top, right - left, bottom - top);
        interactive.Intersect(live->GetClientRect());
        if ( interactive.IsEmpty() )
            return false;

        *rect = interactive;
        return true;
    }
    catch ( const winrt::hresult_error& )
    {
        return false;
    }
}

bool wxWinUIHyperlinkTestAccess::HitTest(
    const wxHyperlinkCtrl& link,
    const wxPoint& point)
{
    wxRect interactive;
    return GetInteractiveRect(link, &interactive) &&
           interactive.Contains(point);
}

bool wxWinUIHyperlinkTestAccess::InvokeAt(
    wxHyperlinkCtrl& link,
    const wxPoint& point)
{
    if ( !HitTest(link, point) )
        return false;

    // Invoke() may synchronously delete this control. Return its result
    // directly and never inspect any member after the call.
    return Invoke(link);
}

#endif // WXWINUI_TEST_SUPPORT

void wxHyperlinkCtrl::SetPointerOver(bool pointerOver)
{
    if ( m_pointerOver == pointerOver )
        return;
    m_pointerOver = pointerOver;
    BumpWinUIModelRevision();
    UpdateWinUIContent();
}

void wxHyperlinkCtrl::HandleThemeChanged()
{
    // wxWinUISetAppTheme() updates the global policy before changing every
    // root RequestedTheme. ActualThemeChanged therefore provides the exact
    // point at which an already-created peer must republish its active visual
    // state. This cannot depend only on the normal-colour provenance: hover
    // or visited may still be a default brush which must be relinquished in
    // High Contrast. No state is touched after the reentrancy-safe projection.
    BumpWinUIModelRevision();
    UpdateWinUIContent();
}

void wxHyperlinkCtrl::OnSysColourChanged(wxSysColourChangedEvent& event)
{
    // Preserve normal wx propagation when the projection returns normally.
    // UpdateWinUIContent() is intentionally allowed to synchronously destroy
    // this control, in which case continuing into its static event table
    // would dereference the deleted handler.
    event.Skip();

    // Reproject the active normal/hover/visited state on every system-colour
    // notification. In High Contrast a default state clears the local
    // Foreground and lets WinUI's ThemeResource win; a custom active state
    // remains authoritative.
    BumpWinUIModelRevision();
    wxWeakRef<wxHyperlinkCtrl> alive(this);
    UpdateWinUIContent();
    if ( !alive )
        event.Skip(false);
}

#ifdef WXWINUI_TEST_SUPPORT
void wxWinUIHyperlinkTestAccess::SetPointerOver(
    wxHyperlinkCtrl& link,
    bool pointerOver)
{
    link.SetPointerOver(pointerOver);
}

bool wxWinUIHyperlinkTestAccess::GetState(
    const wxHyperlinkCtrl& link,
    bool *pointerOver,
    int *horizontalAlignment,
    bool *contextMenuEnabled,
    wxColour *effectiveColour,
    wxWinUIAppearanceSnapshot *appearance,
    bool *peerEnabled)
{
    if ( !link.m_winui || !link.m_winui->button )
        return false;

    try
    {
        if ( pointerOver )
            *pointerOver = link.m_pointerOver;
        if ( horizontalAlignment )
        {
            *horizontalAlignment = static_cast<int>(
                link.m_winui->button.HorizontalContentAlignment());
        }
        if ( contextMenuEnabled )
        {
            *contextMenuEnabled =
                link.m_winui->contextRequestedToken.value != 0;
        }
        if ( effectiveColour )
        {
            *effectiveColour = wxNullColour;
            if ( const auto brush =
                     link.m_winui->button.Foreground()
                         .try_as<MUXM::SolidColorBrush>() )
            {
                const auto colour = brush.Color();
                *effectiveColour =
                    wxColour(colour.R, colour.G, colour.B, colour.A);
            }
        }
        if ( appearance )
        {
            *appearance = wxWinUICaptureAppearance(
                link.m_winui->button, link.m_winui->button);
        }
        if ( peerEnabled )
            *peerEnabled = link.m_winui->button.IsEnabled();
        return true;
    }
    catch ( const winrt::hresult_error& )
    {
        return false;
    }
}


void wxWinUIHyperlinkTestAccess::SetNextPeerWriteHook(
    wxHyperlinkCtrl& link,
    PeerWriteHook hook,
    void *context)
{
    if ( !link.m_winui )
        return;

    link.m_winui->nextPeerWriteHookForTesting = hook;
    link.m_winui->nextPeerWriteContextForTesting =
        hook ? context : nullptr;
}

bool wxWinUIHyperlinkTestAccess::HasDeferredPeerWrite(
    const wxHyperlinkCtrl& link)
{
    return link.m_winui && link.m_winui->callbackState &&
           link.m_winui->callbackState->HasDeferredApply();
}

bool wxWinUIHyperlinkTestAccess::IsPeerProjectionQuarantined(
    const wxHyperlinkCtrl& link)
{
    return link.m_winui && link.m_winui->callbackState &&
           link.m_winui->callbackState->IsApplyQuarantined();
}

unsigned long long wxWinUIHyperlinkTestAccess::GetModelRevision(
    const wxHyperlinkCtrl& link)
{
    return link.m_winui && link.m_winui->callbackState
        ? link.m_winui->callbackState->GetModelRevision()
        : 0;
}

unsigned wxWinUIHyperlinkTestAccess::GetLiveCallbackStateCount()
{
    return gs_liveHyperlinkCallbackStates.load(std::memory_order_acquire);
}

#endif // WXWINUI_TEST_SUPPORT

#endif // wxUSE_HYPERLINKCTRL
