/////////////////////////////////////////////////////////////////////////////
// Name:        src/winui/radiobox.cpp
// Purpose:     wxWinUI wxRadioBox implementation
// Author:      wxWidgets development team
// Created:     2026-06-01
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_RADIOBOX

#include "wx/radiobox.h"
#include "wx/app.h"
#include "wx/log.h"
#include "wx/winui/winui.h"

#ifndef WX_PRECOMP
    #include "wx/event.h"
    #include "wx/arrstr.h"
#endif

#include "private.h"
#include "wx/winui/private/appearance.h"
#include "wx/winui/private/tlwhost.h"

#if wxUSE_TOOLTIPS
    #include "wx/tooltip.h"
#endif

#include <algorithm>
#include <atomic>
#include <cmath>
#include <cstdint>
#include <limits>
#include <memory>
#include <utility>

namespace MUX = winrt::Microsoft::UI::Xaml;
namespace MUXA = winrt::Microsoft::UI::Xaml::Automation;
namespace MUXAP = winrt::Microsoft::UI::Xaml::Automation::Peers;
namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;
namespace WF = winrt::Windows::Foundation;

#if wxUSE_EXTENDED_RTTI
// The XTI version is defined in src/common/radiocmn.cpp.
#else
wxIMPLEMENT_DYNAMIC_CLASS(wxRadioBox, wxControl);
#endif

namespace
{
// Whether new radio boxes draw the Win32-style titled frame.
bool gs_radioBoxBorder = true;
std::atomic<std::uint64_t> gs_checkedHandlersAdded{ 0 };
std::atomic<std::uint64_t> gs_checkedHandlersRevoked{ 0 };
} // namespace

void wxWinUISetRadioBoxBorder(bool useBorder) { gs_radioBoxBorder = useBorder; }
bool wxWinUIGetRadioBoxBorder() { return gs_radioBoxBorder; }

class wxWinUIRadioBoxCallbackState final
{
public:
    static constexpr unsigned MaxApplyPassBudget = 16;

    explicit wxWinUIRadioBoxCallbackState(wxRadioBox *owner)
        : m_owner(owner)
    {
    }

    std::uint64_t GetGeneration() const
    {
        return m_generation.load(std::memory_order_acquire);
    }

    wxRadioBox *GetOwner(std::uint64_t generation) const
    {
        if ( !generation || generation != GetGeneration() )
            return nullptr;

        wxRadioBox * const owner =
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

    std::uint64_t BumpModelRevision(bool rearmProjection = true)
    {
        // Only a mutation which starts outside projection may re-arm the
        // bounded budget. Reentrant mutations belong to the active request;
        // allowing them to reset it would create an unbounded event-loop pump.
        if ( rearmProjection &&
             !m_applyActive.load(std::memory_order_acquire) )
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

    bool IsApplyActive() const
    {
        return m_applyActive.load(std::memory_order_acquire);
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
    std::atomic<wxRadioBox *> m_owner;
    std::atomic<std::uint64_t> m_generation{ 1 };
    std::atomic<std::uint64_t> m_modelRevision{ 1 };
    std::atomic<bool> m_applyActive{ false };
    std::atomic<bool> m_applyRequested{ false };
    std::atomic<bool> m_deferredApplyScheduled{ false };
    std::atomic<bool> m_applyBudgetWarning{ false };
    std::atomic<unsigned> m_applyPassesRemaining{ MaxApplyPassBudget };
    std::atomic<bool> m_applyQuarantined{ false };
};

class wxWinUIRadioBoxApplyGuard final
{
public:
    explicit wxWinUIRadioBoxApplyGuard(
        std::shared_ptr<wxWinUIRadioBoxCallbackState> state)
        : m_state(std::move(state)),
          m_acquired(m_state->TryBeginApply())
    {
    }

    ~wxWinUIRadioBoxApplyGuard()
    {
        if ( m_acquired )
            m_state->EndApply();
    }

    explicit operator bool() const
    {
        return m_acquired;
    }

private:
    std::shared_ptr<wxWinUIRadioBoxCallbackState> m_state;
    bool m_acquired;
};

class wxWinUIRadioBoxImpl
{
public:
    ~wxWinUIRadioBoxImpl()
    {
        Close();
    }

    static void RevokeCheckedHandlers(
        const std::vector<MUXC::RadioButton>& peers,
        std::vector<winrt::event_token>& tokens)
    {
        const size_t count = wxMin(peers.size(), tokens.size());
        for ( size_t i = 0; i < count; ++i )
        {
            if ( !peers[i] || !tokens[i].value )
                continue;

            try
            {
                peers[i].Checked(tokens[i]);
                gs_checkedHandlersRevoked.fetch_add(
                    1, std::memory_order_relaxed);
            }
            catch ( const winrt::hresult_error& e )
            {
                wxWinUILogException("WinUI RadioBox Checked removal", e);
            }
            tokens[i] = {};
        }

#if wxUSE_TOOLTIPS
        // Item peers can disappear as rejected candidates as well as committed
        // generations. Retire their managed-tooltip registry entries in both
        // cases; direct application-authored XAML values are deliberately not
        // touched by this helper.
        for ( const MUXC::RadioButton& peer : peers )
            wxWinUIForgetManagedToolTip(peer);
#endif // wxUSE_TOOLTIPS
    }

    void Close()
    {
        // Individual item peers can be retained by UI Automation after their
        // wxRadioBox or even their content generation has disappeared.
        // Invalidate the shared state before best-effort revocation.
        if ( callbackState )
            callbackState->Invalidate();

        nextPeerWriteHookForTesting = nullptr;
        nextPeerWriteContextForTesting = nullptr;
        RevokeCheckedHandlers(buttons, checkedTokens);
        host.Close();
        root = nullptr;
        background = nullptr;
        titleText = nullptr;
        buttons.clear();
        checkedTokens.clear();
        callbackState.reset();
    }

    wxWinUIControlHost host;
    std::shared_ptr<wxWinUIRadioBoxCallbackState> callbackState;
    MUX::UIElement root{ nullptr };   // the element hosted (border or panel)
    MUXC::Border background{ nullptr };
    MUXC::TextBlock titleText{ nullptr };
    std::vector<MUXC::RadioButton> buttons;
    std::vector<winrt::event_token> checkedTokens;
    std::uint64_t generation = 0;
    wxWinUIRadioBoxPeerWriteHookForTesting
        nextPeerWriteHookForTesting = nullptr;
    void *nextPeerWriteContextForTesting = nullptr;
};

wxRadioBox::wxRadioBox()
{
}

wxRadioBox::wxRadioBox(wxWindow *parent, wxWindowID id, const wxString& title,
                       const wxPoint& pos, const wxSize& size,
                       int n, const wxString choices[], int majorDim,
                       long style, const wxValidator& validator,
                       const wxString& name)
{
    Create(parent, id, title, pos, size, n, choices, majorDim, style,
           validator, name);
}

wxRadioBox::wxRadioBox(wxWindow *parent, wxWindowID id, const wxString& title,
                       const wxPoint& pos, const wxSize& size,
                       const wxArrayString& choices, int majorDim,
                       long style, const wxValidator& validator,
                       const wxString& name)
{
    Create(parent, id, title, pos, size, choices, majorDim, style,
           validator, name);
}

wxRadioBox::~wxRadioBox()
{
    // Invalidate every generation's retained UIA callback before the item
    // model members begin their own teardown.
    m_winui.reset();
}

bool wxRadioBox::Create(wxWindow *parent, wxWindowID id, const wxString& title,
                        const wxPoint& pos, const wxSize& size,
                        int n, const wxString choices[], int majorDim,
                        long style, const wxValidator& validator,
                        const wxString& name)
{
    m_strings.Clear();
    for ( int i = 0; i < n; ++i )
        m_strings.Add(choices[i]);
    return DoCreate(parent, id, title, pos, size, majorDim, style,
                    validator, name);
}

bool wxRadioBox::Create(wxWindow *parent, wxWindowID id, const wxString& title,
                        const wxPoint& pos, const wxSize& size,
                        const wxArrayString& choices, int majorDim,
                        long style, const wxValidator& validator,
                        const wxString& name)
{
    m_strings = choices;
    return DoCreate(parent, id, title, pos, size, majorDim,
                    style, validator, name);
}

bool wxRadioBox::DoCreate(wxWindow *parent, wxWindowID id, const wxString& title,
                          const wxPoint& pos, const wxSize& size, int majorDim,
                          long style, const wxValidator& validator,
                          const wxString& name)
{
    style = (style & ~wxBORDER_MASK) | wxBORDER_NONE;

    if ( !wxControl::Create(parent, id, pos, size, style, validator, name) )
        return false;

    wxControl::SetLabel(title);

    SetMajorDim(majorDim == 0 ? static_cast<int>(m_strings.GetCount()) : majorDim,
                style);

    const size_t count = m_strings.GetCount();
    m_itemEnabled.assign(count, true);
    m_itemShown.assign(count, true);
    m_selection = count ? 0 : wxNOT_FOUND;

    m_winui.reset(new wxWinUIRadioBoxImpl);
    m_winui->callbackState =
        std::make_shared<wxWinUIRadioBoxCallbackState>(this);
    wxWinUIRadioBoxImpl * const createImpl = m_winui.get();
    const std::shared_ptr<wxWinUIRadioBoxCallbackState> callbackState =
        createImpl->callbackState;
    const std::uint64_t lifetimeGeneration =
        callbackState->GetGeneration();
    if ( !m_winui->host.Initialize(this) )
        return false;

    try
    {
        if ( !RebuildItems() )
            return false;
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI RadioBox creation", e);
        return false;
    }

    wxRadioBox * const owner =
        callbackState->GetOwner(lifetimeGeneration);
    if ( !owner || !owner->m_winui ||
         owner->m_winui.get() != createImpl )
    {
        return false;
    }

    owner->SetInitialSize(size);
    wxRadioBox * const ownerAfterSize =
        callbackState->GetOwner(lifetimeGeneration);
    return ownerAfterSize && ownerAfterSize->m_winui &&
           ownerAfterSize->m_winui.get() == createImpl;
}

unsigned int wxRadioBox::GetCount() const
{
    return m_strings.GetCount();
}

wxString wxRadioBox::GetString(unsigned int n) const
{
    wxCHECK_MSG( n < m_strings.GetCount(), wxString(), wxT("invalid radiobox index") );
    return m_strings[n];
}

void wxRadioBox::SetString(unsigned int n, const wxString& s)
{
    wxCHECK_RET( n < m_strings.GetCount(), wxT("invalid radiobox index") );
    if ( m_strings[n] == s )
        return;

    wxWinUIRadioBoxImpl * const impl = m_winui.get();
    const std::shared_ptr<wxWinUIRadioBoxCallbackState> callbackState =
        impl ? impl->callbackState : nullptr;
    const std::uint64_t lifetimeGeneration =
        callbackState ? callbackState->GetGeneration() : 0;
    const wxString old = m_strings[n];
    m_strings[n] = s;
    const std::uint64_t transactionRevision =
        BumpWinUIModelRevision();
    const bool rebuilt = RebuildItems();
    wxRadioBox * const owner =
        callbackState
            ? callbackState->GetOwner(lifetimeGeneration)
            : this;
    if ( !owner || owner->m_winui.get() != impl ||
         (callbackState &&
          owner->m_winui->callbackState != callbackState) )
    {
        return;
    }

    if ( !rebuilt )
    {
        // Roll back only while this setter still owns the model revision.
        // A reentrant later writer is authoritative even when its projection
        // subsequently failed.
        if ( callbackState &&
             callbackState->GetModelRevision() == transactionRevision )
        {
            owner->m_strings[n] = old;
            callbackState->BumpModelRevision();
        }
        return;
    }

    owner->InvalidateBestSize();
}

namespace
{

// Push the radio box's preferred focus target -- its selected item -- to
// the shared host, as a weak XAML reference (see SetSlotPreferredFocus).
void wxWinUIPushRadioPreferredFocus(
    wxRadioBox *radio,
    const std::vector<MUXC::RadioButton>& buttons,
    int sel)
{
    if ( wxWinUITopLevelHost * const host =
            wxWinUITopLevelHost::ForWindow(radio, false) )
    {
        MUX::UIElement target{ nullptr };
        if ( sel >= 0 && sel < static_cast<int>(buttons.size()) )
            target = buttons[sel];
        host->SetSlotPreferredFocus(radio, target);
    }
}

} // anonymous namespace

void wxRadioBox::SetSelection(int n)
{
    wxCHECK_RET( n == wxNOT_FOUND ||
                 (n >= 0 && static_cast<unsigned int>(n) < m_strings.GetCount()),
                 wxT("invalid radiobox index") );

    if ( n == m_selection )
        return;

    wxWinUIRadioBoxImpl * const impl = m_winui.get();
    const std::shared_ptr<wxWinUIRadioBoxCallbackState> callbackState =
        impl ? impl->callbackState : nullptr;
    const std::uint64_t lifetimeGeneration =
        callbackState ? callbackState->GetGeneration() : 0;
    const int oldSelection = m_selection;
    m_selection = n;
    const std::uint64_t transactionRevision =
        BumpWinUIModelRevision();
    const bool rebuilt = SyncItemState();

    wxRadioBox * const owner =
        callbackState
            ? callbackState->GetOwner(lifetimeGeneration)
            : this;
    if ( !owner || owner->m_winui.get() != impl ||
         (callbackState &&
          owner->m_winui->callbackState != callbackState) )
    {
        return;
    }

    if ( !rebuilt && callbackState &&
         callbackState->GetModelRevision() == transactionRevision )
    {
        owner->m_selection = oldSelection;
        callbackState->BumpModelRevision();
    }
}

int wxRadioBox::GetSelection() const
{
    return m_selection;
}

void wxRadioBox::SetLabel(const wxString& label)
{
    if ( label == GetLabel() )
        return;

    wxWinUIRadioBoxImpl * const impl = m_winui.get();
    const std::shared_ptr<wxWinUIRadioBoxCallbackState> callbackState =
        impl ? impl->callbackState : nullptr;
    const std::uint64_t lifetimeGeneration =
        callbackState ? callbackState->GetGeneration() : 0;
    const wxString old = GetLabel();
    wxControl::SetLabel(label);

    wxRadioBox *owner =
        callbackState
            ? callbackState->GetOwner(lifetimeGeneration)
            : nullptr;
    if ( !owner || !owner->m_winui ||
         owner->m_winui.get() != impl ||
         owner->m_winui->callbackState != callbackState )
    {
        return;
    }

    const std::uint64_t transactionRevision =
        owner->BumpWinUIModelRevision();
    const bool rebuilt = owner->RebuildItems();
    owner = callbackState->GetOwner(lifetimeGeneration);
    if ( !owner || !owner->m_winui ||
         owner->m_winui.get() != impl ||
         owner->m_winui->callbackState != callbackState )
    {
        return;
    }

    if ( !rebuilt )
    {
        if ( callbackState &&
             callbackState->GetModelRevision() == transactionRevision )
        {
            owner->wxControl::SetLabel(old);
            owner = callbackState->GetOwner(lifetimeGeneration);
            if ( owner && owner->m_winui &&
                 owner->m_winui.get() == impl &&
                 owner->m_winui->callbackState == callbackState &&
                 callbackState->GetModelRevision() ==
                     transactionRevision )
            {
                callbackState->BumpModelRevision();
            }
        }
        return;
    }

    owner->InvalidateBestSize();
}

bool wxRadioBox::SetFont(const wxFont& font)
{
    wxWinUIRadioBoxImpl * const impl = m_winui.get();
    const std::shared_ptr<wxWinUIRadioBoxCallbackState> callbackState =
        impl ? impl->callbackState : nullptr;
    const std::uint64_t lifetimeGeneration =
        callbackState ? callbackState->GetGeneration() : 0;
    const bool changed = wxControl::SetFont(font);

    wxRadioBox * const owner =
        callbackState
            ? callbackState->GetOwner(lifetimeGeneration)
            : nullptr;
    if ( !owner || !owner->m_winui ||
         owner->m_winui.get() != impl ||
         owner->m_winui->callbackState != callbackState )
    {
        return changed;
    }

    owner->InvalidateBestSize();
    owner->BumpWinUIModelRevision();
    owner->RebuildItems();
    // RebuildItems() is the final synchronous boundary: it may destroy this.
    // Do not touch the object after it returns.
    return changed;
}

bool wxRadioBox::SetForegroundColour(const wxColour& colour)
{
    wxWinUIRadioBoxImpl * const impl = m_winui.get();
    const std::shared_ptr<wxWinUIRadioBoxCallbackState> callbackState =
        impl ? impl->callbackState : nullptr;
    const std::uint64_t lifetimeGeneration =
        callbackState ? callbackState->GetGeneration() : 0;
    const bool changed = wxControl::SetForegroundColour(colour);

    wxRadioBox * const owner =
        callbackState
            ? callbackState->GetOwner(lifetimeGeneration)
            : nullptr;
    if ( !owner || !owner->m_winui ||
         owner->m_winui.get() != impl ||
         owner->m_winui->callbackState != callbackState )
    {
        return changed;
    }

    owner->BumpWinUIModelRevision();
    owner->RebuildItems();
    return changed;
}

bool wxRadioBox::SetBackgroundColour(const wxColour& colour)
{
    const wxColour oldColour = GetBackgroundColour();
    const bool hadOldColour = m_hasBgCol;
    wxWinUIRadioBoxImpl * const impl = m_winui.get();
    const std::shared_ptr<wxWinUIRadioBoxCallbackState> callbackState =
        impl ? impl->callbackState : nullptr;
    const std::uint64_t lifetimeGeneration =
        callbackState ? callbackState->GetGeneration() : 0;
    const bool changed = wxControl::SetBackgroundColour(colour);
    wxRadioBox *owner =
        callbackState
            ? callbackState->GetOwner(lifetimeGeneration)
            : nullptr;
    if ( !changed || !owner || !owner->m_winui ||
         owner->m_winui.get() != impl ||
         owner->m_winui->callbackState != callbackState )
    {
        return changed;
    }

    // The title chip owns a ThemeResource expression when no custom colour
    // is present. Rebuilding is the only way to restore that expression after
    // a custom background; assigning the current brush would freeze the theme.
    const std::uint64_t transactionRevision =
        owner->BumpWinUIModelRevision();
    const bool rebuilt = owner->RebuildItems();
    owner = callbackState->GetOwner(lifetimeGeneration);
    if ( !owner || !owner->m_winui ||
         owner->m_winui.get() != impl ||
         owner->m_winui->callbackState != callbackState )
    {
        return changed;
    }

    if ( !rebuilt )
    {
        if ( callbackState->GetModelRevision() == transactionRevision )
        {
            owner->wxControl::SetBackgroundColour(
                hadOldColour ? oldColour : wxNullColour);
            owner = callbackState->GetOwner(lifetimeGeneration);
            if ( owner && owner->m_winui &&
                 owner->m_winui.get() == impl &&
                 owner->m_winui->callbackState == callbackState &&
                 callbackState->GetModelRevision() ==
                     transactionRevision )
            {
                callbackState->BumpModelRevision();
            }
        }
        return false;
    }

    return true;
}

bool wxRadioBox::Enable(bool enable)
{
    if ( enable == IsThisEnabled() )
        return false;

    // wxRadioBox::Enable() applies to every item, matching the established
    // wxMSW contract. Do this here, rather than in DoEnable(): the latter is
    // also called when an ancestor is enabled/disabled and that propagation
    // must preserve the logical per-item state.
    std::fill(m_itemEnabled.begin(), m_itemEnabled.end(), enable);
    return wxControl::Enable(enable);
}

bool wxRadioBox::Enable(unsigned int n, bool enable)
{
    wxCHECK_MSG( n < m_itemEnabled.size(), false, wxT("invalid radiobox index") );
    if ( m_itemEnabled[n] == enable )
        return false;

    wxWinUIRadioBoxImpl * const impl = m_winui.get();
    const std::shared_ptr<wxWinUIRadioBoxCallbackState> callbackState =
        impl ? impl->callbackState : nullptr;
    const std::uint64_t lifetimeGeneration =
        callbackState ? callbackState->GetGeneration() : 0;
    const bool old = m_itemEnabled[n];
    m_itemEnabled[n] = enable;
    const std::uint64_t transactionRevision =
        BumpWinUIModelRevision();
    const bool rebuilt = SyncItemState();
    wxRadioBox * const owner =
        callbackState
            ? callbackState->GetOwner(lifetimeGeneration)
            : this;
    if ( !owner || owner->m_winui.get() != impl ||
         (callbackState &&
          owner->m_winui->callbackState != callbackState) )
    {
        return rebuilt;
    }
    if ( !rebuilt && callbackState &&
         callbackState->GetModelRevision() == transactionRevision )
    {
        owner->m_itemEnabled[n] = old;
        callbackState->BumpModelRevision();
    }
    return rebuilt;
}

bool wxRadioBox::Show(unsigned int n, bool show)
{
    wxCHECK_MSG( n < m_itemShown.size(), false, wxT("invalid radiobox index") );
    if ( m_itemShown[n] == show )
        return false;

    wxWinUIRadioBoxImpl * const impl = m_winui.get();
    const std::shared_ptr<wxWinUIRadioBoxCallbackState> callbackState =
        impl ? impl->callbackState : nullptr;
    const std::uint64_t lifetimeGeneration =
        callbackState ? callbackState->GetGeneration() : 0;
    const bool old = m_itemShown[n];
    m_itemShown[n] = show;
    const std::uint64_t transactionRevision =
        BumpWinUIModelRevision();
    const bool rebuilt = SyncItemState();
    wxRadioBox * const owner =
        callbackState
            ? callbackState->GetOwner(lifetimeGeneration)
            : this;
    if ( !owner || owner->m_winui.get() != impl ||
         (callbackState &&
          owner->m_winui->callbackState != callbackState) )
    {
        return rebuilt;
    }
    if ( !rebuilt && callbackState &&
         callbackState->GetModelRevision() == transactionRevision )
    {
        owner->m_itemShown[n] = old;
        callbackState->BumpModelRevision();
    }
    if ( rebuilt )
        owner->InvalidateBestSize();
    return rebuilt;
}

bool wxRadioBox::Show(bool show)
{
    wxWinUIRadioBoxImpl * const impl = m_winui.get();
    const std::shared_ptr<wxWinUIRadioBoxCallbackState> callbackState =
        impl ? impl->callbackState : nullptr;
    const std::uint64_t lifetimeGeneration =
        callbackState ? callbackState->GetGeneration() : 0;
    const std::uint64_t entryRevision =
        callbackState ? callbackState->GetModelRevision() : 0;
    const bool changed = wxControl::Show(show);
    wxRadioBox * const owner =
        callbackState
            ? callbackState->GetOwner(lifetimeGeneration)
            : this;
    if ( !changed || !owner || owner->m_winui.get() != impl ||
         (callbackState &&
          (owner->m_winui->callbackState != callbackState ||
           callbackState->GetModelRevision() != entryRevision)) )
    {
        return false;
    }

    // Match the established wxMSW RadioBox contract: toggling the whole box
    // also resets the logical per-item shown state. Parent visibility changes
    // do not call this method and therefore preserve individual item state.
    std::fill(owner->m_itemShown.begin(), owner->m_itemShown.end(), show);
    owner->BumpWinUIModelRevision();
    owner->SyncItemState();
    return true;
}

void wxRadioBox::DoEnable(bool enable)
{
    wxWinUIRadioBoxImpl * const impl = m_winui.get();
    const std::shared_ptr<wxWinUIRadioBoxCallbackState> callbackState =
        impl ? impl->callbackState : nullptr;
    const std::uint64_t lifetimeGeneration =
        callbackState ? callbackState->GetGeneration() : 0;
    const std::uint64_t entryRevision =
        callbackState ? callbackState->GetModelRevision() : 0;

    wxControl::DoEnable(enable);

    wxRadioBox * const owner =
        callbackState
            ? callbackState->GetOwner(lifetimeGeneration)
            : nullptr;
    if ( !owner || !owner->m_winui ||
         owner->m_winui.get() != impl ||
         owner->m_winui->callbackState != callbackState )
    {
        return;
    }

    // DoEnable() is also invoked recursively for an ancestor state change.
    // Synchronize from the effective state without overwriting per-item flags
    // and without replacing the item peers retained by UI Automation.
    if ( callbackState->GetModelRevision() == entryRevision )
        owner->BumpWinUIModelRevision();
    owner->SyncItemState();
}

bool wxRadioBox::IsItemEnabled(unsigned int n) const
{
    wxCHECK_MSG( n < m_itemEnabled.size(), false, wxT("invalid radiobox index") );
    return m_itemEnabled[n];
}

bool wxRadioBox::IsItemShown(unsigned int n) const
{
    wxCHECK_MSG( n < m_itemShown.size(), false, wxT("invalid radiobox index") );
    return m_itemShown[n];
}

bool wxRadioBox::CanBeFocused() const
{
    if ( !wxControl::CanBeFocused() )
        return false;

    for ( unsigned int item = 0; item < GetCount(); ++item )
    {
        if ( IsItemEnabled(item) && IsItemShown(item) )
            return true;
    }

    return false;
}

void wxRadioBox::SetFocus()
{
    if ( !m_winui || !m_winui->callbackState ||
         m_winui->buttons.empty() )
    {
        return;
    }

    wxWinUIRadioBoxImpl * const impl = m_winui.get();
    const std::shared_ptr<wxWinUIRadioBoxCallbackState> callbackState =
        impl->callbackState;
    const std::uint64_t lifetimeGeneration =
        callbackState->GetGeneration();
    const std::uint64_t peerGeneration = impl->generation;
    const std::vector<MUXC::RadioButton> buttons = impl->buttons;

    int item = m_selection;
    if ( item < 0 || item >= static_cast<int>(buttons.size()) )
        item = 0;

    wxWinUIPushRadioPreferredFocus(this, buttons, item);

    wxRadioBox * const owner =
        callbackState->GetOwner(lifetimeGeneration);
    if ( !owner || !owner->m_winui ||
         owner->m_winui.get() != impl ||
         owner->m_winui->callbackState != callbackState ||
         owner->m_winui->generation != peerGeneration ||
         owner->m_winui->buttons.size() != buttons.size() )
    {
        return;
    }

    for ( size_t i = 0; i < buttons.size(); ++i )
    {
        if ( owner->m_winui->buttons[i] != buttons[i] )
            return;
    }

    // wxWindowMSW::SetFocus() enters the common host focus transaction. Its
    // preferred target is the selected item installed immediately above,
    // matching the native wxMSW RadioBox contract.
    owner->wxControl::SetFocus();
}

int wxRadioBox::GetItemFromPoint(const wxPoint& point) const
{
    if ( !m_winui || !m_winui->callbackState ||
         m_winui->buttons.empty() )
    {
        return wxNOT_FOUND;
    }

    wxWinUIRadioBoxImpl * const impl = m_winui.get();
    const std::shared_ptr<wxWinUIRadioBoxCallbackState> callbackState =
        impl->callbackState;
    const std::uint64_t lifetimeGeneration =
        callbackState->GetGeneration();
    const std::uint64_t peerGeneration = impl->generation;
    const std::vector<MUXC::RadioButton> buttons = impl->buttons;

    const auto getCurrentOwner =
        [callbackState, lifetimeGeneration, peerGeneration, impl,
         &buttons]() -> wxRadioBox *
        {
            wxRadioBox * const owner =
                callbackState->GetOwner(lifetimeGeneration);
            if ( !owner || !owner->m_winui ||
                 owner->m_winui.get() != impl ||
                 owner->m_winui->callbackState != callbackState ||
                 owner->m_winui->generation != peerGeneration ||
                 owner->m_winui->buttons.size() != buttons.size() )
            {
                return nullptr;
            }

            for ( size_t i = 0; i < buttons.size(); ++i )
            {
                if ( owner->m_winui->buttons[i] != buttons[i] )
                    return nullptr;
            }
            return owner;
        };

    if ( !getCurrentOwner() )
        return wxNOT_FOUND;

    try
    {
        for ( size_t i = 0; i < buttons.size(); ++i )
        {
            const MUX::Visibility visibility = buttons[i].Visibility();
            if ( !getCurrentOwner() )
                return wxNOT_FOUND;
            if ( visibility != MUX::Visibility::Visible )
                continue;

            const float width = static_cast<float>(buttons[i].ActualWidth());
            if ( !getCurrentOwner() )
                return wxNOT_FOUND;
            const float height =
                static_cast<float>(buttons[i].ActualHeight());
            if ( !getCurrentOwner() )
                return wxNOT_FOUND;
            if ( width <= 0.0f || height <= 0.0f )
                continue;

            WF::Rect clientBounds{};
            const wxWinUICoordinateResult mapped =
                wxWinUIVisualCoordinates::ElementBoundsToClient(
                    const_cast<wxRadioBox *>(this),
                    buttons[i],
                    WF::Rect{ 0.0f, 0.0f, width, height },
                    &clientBounds);
            if ( mapped != wxWinUICoordinateResult::Mapped ||
                 !getCurrentOwner() )
            {
                return wxNOT_FOUND;
            }

            const float x = static_cast<float>(point.x);
            const float y = static_cast<float>(point.y);
            if ( x >= clientBounds.X &&
                 y >= clientBounds.Y &&
                 x < clientBounds.X + clientBounds.Width &&
                 y < clientBounds.Y + clientBounds.Height )
            {
                return static_cast<int>(i);
            }
        }
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI RadioBox item hit test", e);
    }

    return wxNOT_FOUND;
}

wxSize wxRadioBox::DoGetBestSize() const
{
    // Measure the actual radio buttons (a single control measures reliably,
    // unlike the whole detached panel) to get the real per-item size including
    // the WinUI glyph + spacing + margins, then lay out the grid and frame.
    // WinUI radio buttons are wider than a hand-rolled estimate, which otherwise
    // caused the last column to be clipped.
    int itemW = 0;
    int itemH = FromDIP(30);

    if ( m_winui && !m_winui->buttons.empty() )
    {
        const float inf = std::numeric_limits<float>::infinity();
        for ( auto& button : m_winui->buttons )
        {
            try
            {
                button.Measure({ inf, inf });
                const auto d = button.DesiredSize();   // includes the margins
                // DesiredSize is in DIPs, the rest of this function works in
                // physical pixels.
                const wxSize item = FromDIP(
                    wxSize(static_cast<int>(std::ceil(d.Width)),
                           static_cast<int>(std::ceil(d.Height))));
                itemW = wxMax(itemW, item.x);
                itemH = wxMax(itemH, item.y);
            }
            catch ( const winrt::hresult_error& )
            {
            }
        }
    }

    if ( itemW <= 0 )   // measuring failed: fall back to a generous estimate
    {
        int maxText = 0;
        for ( unsigned int i = 0; i < m_strings.GetCount(); ++i )
            maxText = wxMax(maxText, GetTextExtent(m_strings[i]).x);
        itemW = maxText + FromDIP(60);
    }

    const int cols = wxMax(1, GetColumnCount());
    const int rows = wxMax(1, GetRowCount());

    int w = cols * itemW;
    int h = rows * itemH;

    if ( gs_radioBoxBorder )
    {
        w += FromDIP(2 * 12 + 2);              // frame left/right padding + border
        h += FromDIP(14 + 12 + 2);             // frame top/bottom padding + border
        if ( !GetLabel().empty() )
            h += FromDIP(8);                   // frame top margin under the title
    }
    else if ( !GetLabel().empty() )
    {
        h += FromDIP(24);                      // plain header height
    }

    return wxSize(w, h);
}

unsigned long long wxRadioBox::BumpWinUIModelRevision()
{
    return m_winui && m_winui->callbackState
        ? m_winui->callbackState->BumpModelRevision()
        : 0;
}

bool wxRadioBox::SyncItemState()
{
    if ( !m_winui || !m_winui->host.IsOk() ||
         !m_winui->callbackState )
    {
        return false;
    }

    wxWinUIRadioBoxImpl * const impl = m_winui.get();
    const std::shared_ptr<wxWinUIRadioBoxCallbackState> callbackState =
        impl->callbackState;
    const std::uint64_t lifetimeGeneration =
        callbackState->GetGeneration();
    const std::uint64_t peerGeneration = impl->generation;
    const std::vector<MUXC::RadioButton> buttons = impl->buttons;

    // All public state mutations keep the existing item peers alive. In
    // particular, UIA clients are allowed to retain an item provider across
    // SetSelection(), Enable() and Show(). A reentrant structural mutation is
    // different: it requests a full projection while this guard is active and
    // is replayed after the in-place transaction releases the guard.
    bool rebuildRequested = false;
    bool succeeded = false;
    {
        const wxWinUIRadioBoxApplyGuard applyGuard(callbackState);
        if ( !applyGuard )
            return true;

        const auto getCurrentOwner =
            [callbackState, lifetimeGeneration, impl, peerGeneration,
             &buttons]() -> wxRadioBox *
            {
                wxRadioBox * const owner =
                    callbackState->GetOwner(lifetimeGeneration);
                if ( !owner || !owner->m_winui ||
                     owner->m_winui.get() != impl ||
                     owner->m_winui->callbackState != callbackState ||
                     owner->m_winui->generation != peerGeneration ||
                     owner->m_winui->buttons.size() != buttons.size() )
                {
                    return nullptr;
                }

                for ( size_t i = 0; i < buttons.size(); ++i )
                {
                    if ( owner->m_winui->buttons[i] != buttons[i] )
                        return nullptr;
                }
                return owner;
            };

        wxRadioBox *owner = getCurrentOwner();
        if ( !owner )
            return false;

        const std::uint64_t revision =
            callbackState->GetModelRevision();
        const std::vector<bool> itemEnabled = owner->m_itemEnabled;
        const std::vector<bool> itemShown = owner->m_itemShown;
        const int selection = owner->m_selection;
        const bool effectiveEnabled = owner->IsEnabled();

        if ( itemEnabled.size() != buttons.size() ||
             itemShown.size() != buttons.size() )
        {
            rebuildRequested = true;
        }
        else
        {
            struct PeerState final
            {
                bool enabled = false;
                bool shown = false;
                bool checked = false;
            };

            std::vector<PeerState> previous;
            previous.reserve(buttons.size());

            const auto isCurrent =
                [&]() -> bool
                {
                    return getCurrentOwner() &&
                           callbackState->GetModelRevision() == revision;
                };

            try
            {
                for ( const MUXC::RadioButton& button : buttons )
                {
                    const auto checked = button.IsChecked();
                    previous.push_back(
                        {
                            button.IsEnabled(),
                            button.Visibility() == MUX::Visibility::Visible,
                            checked && checked.Value()
                        });
                    if ( !isCurrent() )
                    {
                        rebuildRequested = getCurrentOwner() != nullptr;
                        break;
                    }
                }

                for ( size_t i = 0;
                      !rebuildRequested && i < buttons.size();
                      ++i )
                {
                    buttons[i].IsEnabled(
                        effectiveEnabled && itemEnabled[i]);
                    if ( !isCurrent() )
                    {
                        rebuildRequested = getCurrentOwner() != nullptr;
                        break;
                    }

                    buttons[i].Visibility(
                        itemShown[i] ? MUX::Visibility::Visible
                                     : MUX::Visibility::Collapsed);
                    if ( !isCurrent() )
                    {
                        rebuildRequested = getCurrentOwner() != nullptr;
                        break;
                    }

                    buttons[i].IsChecked(
                        static_cast<int>(i) == selection);
                    if ( !isCurrent() )
                    {
                        rebuildRequested = getCurrentOwner() != nullptr;
                        break;
                    }
                }

                if ( !rebuildRequested )
                {
                    owner = getCurrentOwner();
                    if ( !owner )
                        return false;

                    wxWinUIPushRadioPreferredFocus(
                        owner, buttons, selection);
                    if ( !isCurrent() )
                    {
                        rebuildRequested = getCurrentOwner() != nullptr;
                    }
                    else
                    {
                        owner = getCurrentOwner();
                        if ( !owner )
                            return false;
                        owner->m_winui->host.ForceRender();
                        if ( !isCurrent() )
                        {
                            rebuildRequested =
                                getCurrentOwner() != nullptr;
                        }
                    }
                }

                if ( callbackState->ConsumeApplyRequest() )
                    rebuildRequested = getCurrentOwner() != nullptr;

                succeeded = !rebuildRequested &&
                            getCurrentOwner() != nullptr;
            }
            catch ( const winrt::hresult_error& e )
            {
                // Property assignment is not atomic across multiple peers.
                // Restore the exact previous peer state while this generation
                // is still current, then rebuild the current wx model after
                // releasing the apply guard. The full rebuild is the safe
                // fallback if either the write or this best-effort rollback
                // fails.
                if ( previous.size() == buttons.size() &&
                     isCurrent() )
                {
                    try
                    {
                        for ( size_t i = 0; i < buttons.size(); ++i )
                        {
                            buttons[i].IsEnabled(previous[i].enabled);
                            buttons[i].Visibility(
                                previous[i].shown
                                    ? MUX::Visibility::Visible
                                    : MUX::Visibility::Collapsed);
                            buttons[i].IsChecked(previous[i].checked);
                            if ( !isCurrent() )
                                break;
                        }
                    }
                    catch ( const winrt::hresult_error& restoreError )
                    {
                        wxWinUILogException(
                            "WinUI RadioBox state rollback",
                            restoreError);
                    }
                }

                wxWinUILogException("WinUI RadioBox state update", e);
                rebuildRequested = getCurrentOwner() != nullptr;
                if ( callbackState->ConsumeApplyRequest() )
                    rebuildRequested = getCurrentOwner() != nullptr;
            }
        }
    }

    if ( rebuildRequested )
    {
        wxRadioBox * const owner =
            callbackState->GetOwner(lifetimeGeneration);
        if ( !owner || !owner->m_winui ||
             owner->m_winui.get() != impl ||
             owner->m_winui->callbackState != callbackState )
        {
            return false;
        }
        return owner->RebuildItems();
    }

    return succeeded;
}

bool wxRadioBox::RebuildItems()
{
    if ( !m_winui || !m_winui->host.IsOk() ||
         !m_winui->callbackState )
    {
        return false;
    }

    wxWinUIRadioBoxImpl * const impl = m_winui.get();
    const std::shared_ptr<wxWinUIRadioBoxCallbackState> callbackState =
        impl->callbackState;
    const std::uint64_t lifetimeGeneration =
        callbackState->GetGeneration();
    const wxWinUIRadioBoxApplyGuard applyGuard(callbackState);
    if ( !applyGuard )
        return true;

    enum class ApplyState
    {
        Current,
        Restart,
        Dead
    };

    struct Candidate final
    {
        ~Candidate()
        {
            wxWinUIRadioBoxImpl::RevokeCheckedHandlers(
                buttons, checkedTokens);
        }

        MUX::UIElement root{ nullptr };
        MUXC::Border background{ nullptr };
        MUXC::TextBlock titleText{ nullptr };
        std::vector<MUXC::RadioButton> buttons;
        std::vector<winrt::event_token> checkedTokens;
    };

    constexpr unsigned MaxSynchronousApplyPasses = 8;
    const auto quarantineLatestRevision =
        [&]() -> bool
        {
            callbackState->QuarantineApply();
            if ( callbackState->MarkApplyBudgetWarning() )
            {
                wxLogWarning(
                    "WinUI RadioBox model kept changing during projection; "
                    "quarantining this request until the next external model "
                    "mutation.");
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

            if ( !wxTheApp ||
                 !callbackState->TryScheduleDeferredApply() )
            {
                return quarantineLatestRevision();
            }

            // No callback is queued yet. Logging is deliberately done before
            // CallAfter(): a custom log target may pump pending events, and a
            // callback queued first could otherwise consume the sole ticket
            // while this apply guard is still active.
            if ( callbackState->MarkApplyBudgetWarning() )
            {
                wxLogWarning(
                    "WinUI RadioBox model kept changing during projection; "
                    "using its one bounded asynchronous replay.");
            }

            wxRadioBox * const owner =
                callbackState->GetOwner(lifetimeGeneration);
            wxApp * const app = wxTheApp;
            if ( !app || !owner || !owner->m_winui ||
                 owner->m_winui.get() != impl ||
                 owner->m_winui->callbackState != callbackState )
            {
                callbackState->CancelDeferredApply();
                return false;
            }

            const std::weak_ptr<wxWinUIRadioBoxCallbackState>
                weakState(callbackState);
            const std::uint64_t scheduledRevision =
                callbackState->GetModelRevision();
            app->CallAfter(
                [weakState, lifetimeGeneration, scheduledRevision, impl]()
                {
                    const std::shared_ptr<
                        wxWinUIRadioBoxCallbackState> state =
                            weakState.lock();
                    if ( !state || !state->ConsumeDeferredApply() )
                        return;

                    wxRadioBox * const owner =
                        state->GetOwner(lifetimeGeneration);
                    if ( !owner || !owner->m_winui ||
                         owner->m_winui.get() != impl ||
                         owner->m_winui->callbackState != state )
                    {
                        return;
                    }

                    if ( state->GetModelRevision() != scheduledRevision )
                        state->RequestApply();
                    owner->RebuildItems();
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
        wxRadioBox * const live =
            callbackState->GetOwner(lifetimeGeneration);
        if ( !live || !live->m_winui ||
             live->m_winui.get() != impl ||
             live->m_winui->callbackState != callbackState )
        {
            return false;
        }

        const std::uint64_t revision =
            callbackState->GetModelRevision();
        const std::uint64_t baseGeneration = impl->generation;
        std::uint64_t newGeneration = baseGeneration + 1;
        if ( !newGeneration )
            ++newGeneration;

        // Snapshot the entire wx model before creating detached peers. Every
        // committed generation therefore represents exactly one revision.
        const wxArrayString strings = live->m_strings;
        const std::vector<bool> itemEnabled = live->m_itemEnabled;
        const std::vector<bool> itemShown = live->m_itemShown;
        const int selection = live->m_selection;
        const bool effectiveEnabled = live->IsEnabled();
        const bool specifyRows =
            (live->GetWindowStyle() & wxRA_SPECIFY_ROWS) != 0;
        const int cols = wxMax(1, live->GetColumnCount());
        const int rows = wxMax(1, live->GetRowCount());
        const wxString rawTitle = live->GetLabel();
        const wxString title = wxControl::GetLabelText(rawTitle);
        const bool hasFont = live->m_hasFont;
        const wxFont font = hasFont ? live->GetFont() : wxNullFont;
        const bool hasForeground = live->m_hasFgCol;
        const wxColour foreground =
            hasForeground ? live->GetForegroundColour() : wxNullColour;
        const bool hasBackground = live->m_hasBgCol;
        const wxColour background =
            hasBackground ? live->GetBackgroundColour() : wxNullColour;
#if wxUSE_TOOLTIPS
        std::vector<wxString> itemToolTips(strings.GetCount());
        std::vector<bool> hasItemToolTip(strings.GetCount(), false);
        std::vector<int> itemToolTipMaximumWidths(
            strings.GetCount(), -2);
        for ( size_t i = 0; i < strings.GetCount(); ++i )
        {
            if ( wxToolTip * const toolTip = live->GetItemToolTip(i) )
            {
                hasItemToolTip[i] = true;
                itemToolTips[i] = toolTip->GetTip();
                itemToolTipMaximumWidths[i] =
                    toolTip->GetWinUIMaxWidthAtCreation();
            }
        }
#endif // wxUSE_TOOLTIPS
        const winrt::hstring groupName =
            wxWinUIToHString(wxString::Format(
                "wxRadioBox_%p", static_cast<void *>(live)));

        const auto checkState =
            [callbackState, lifetimeGeneration, impl, revision]()
            {
                wxRadioBox * const current =
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

        Candidate candidate;
        candidate.buttons.reserve(strings.GetCount());
        candidate.checkedTokens.reserve(strings.GetCount());

        try
        {
            MUXC::Grid grid;
            for ( int c = 0; c < cols; ++c )
            {
                MUXC::ColumnDefinition col;
                col.Width(MUX::GridLengthHelper::Auto());
                grid.ColumnDefinitions().Append(col);
            }
            for ( int r = 0; r < rows; ++r )
            {
                MUXC::RowDefinition row;
                row.Height(MUX::GridLengthHelper::Auto());
                grid.RowDefinitions().Append(row);
            }

            ApplyState itemConstructionState = ApplyState::Current;
            const int count = static_cast<int>(strings.GetCount());
            for ( int i = 0; i < count; ++i )
            {
                int row, colpos;
                if ( specifyRows )
                {
                    row = i % rows;
                    colpos = i / rows;
                }
                else
                {
                    row = i / cols;
                    colpos = i % cols;
                }

                MUXC::RadioButton button;
                button.Content(winrt::box_value(wxWinUIToHString(
                    wxControl::GetLabelText(strings[i]))));
                wxWinUIApplyAccessKey(button, strings[i]);
                wxWinUIApplyFont(button, font);
                wxWinUIApplyForeground(button, foreground);
                button.GroupName(groupName);
                if ( i < static_cast<int>(itemEnabled.size()) )
                {
                    button.IsEnabled(
                        effectiveEnabled && itemEnabled[i]);
                }
                if ( i < static_cast<int>(itemShown.size()) &&
                     !itemShown[i] )
                {
                    button.Visibility(MUX::Visibility::Collapsed);
                }
                if ( i == selection )
                    button.IsChecked(true);

                MUX::Thickness margin{};
                margin.Left = 2.0;
                margin.Right = 12.0;
                margin.Top = 2.0;
                margin.Bottom = 2.0;
                button.Margin(margin);

                const winrt::event_token checkedToken = button.Checked(
                    [callbackState, lifetimeGeneration, newGeneration, i](
                        winrt::Windows::Foundation::IInspectable const&,
                        MUX::RoutedEventArgs const&)
                    {
                        wxRadioBox *owner =
                            callbackState->GetOwner(lifetimeGeneration);
                        if ( !owner || !owner->m_winui ||
                             owner->m_winui->callbackState != callbackState ||
                             owner->m_winui->generation != newGeneration ||
                             callbackState->IsApplyActive() ||
                             i == owner->m_selection )
                        {
                            return;
                        }

                        owner->m_selection = i;
                        const std::uint64_t eventRevision =
                            callbackState->BumpModelRevision(
                                false /* peer is already authoritative */);
                        wxWinUIPushRadioPreferredFocus(
                            owner, owner->m_winui->buttons, i);

                        owner =
                            callbackState->GetOwner(lifetimeGeneration);
                        if ( !owner || !owner->m_winui ||
                             owner->m_winui->callbackState != callbackState ||
                             owner->m_winui->generation != newGeneration ||
                             callbackState->GetModelRevision() !=
                                 eventRevision )
                        {
                            return;
                        }

                        // The command event can destroy owner. Nothing follows.
                        owner->SendSelectionEvent();
                    });
                // Publish the revocation pair before any later XAML call can
                // throw. Candidate's destructor owns it from this point.
                candidate.buttons.push_back(button);
                candidate.checkedTokens.push_back(checkedToken);
                gs_checkedHandlersAdded.fetch_add(
                    1, std::memory_order_relaxed);

#if wxUSE_TOOLTIPS
                if ( hasItemToolTip[i] )
                {
                    if ( !wxWinUISetToolTip(
                             button,
                             itemToolTips[i],
                             live,
                             nullptr,
                             itemToolTipMaximumWidths[i]) )
                    {
                        return false;
                    }

                    itemConstructionState = checkState();
                    if ( itemConstructionState != ApplyState::Current )
                        break;
                }
#endif // wxUSE_TOOLTIPS

                MUXC::Grid::SetRow(button, row);
                MUXC::Grid::SetColumn(button, colpos);
                grid.Children().Append(button);
            }

            if ( itemConstructionState == ApplyState::Dead )
                return false;
            if ( itemConstructionState == ApplyState::Restart )
                continue;

            MUX::UIElement visualRoot{ nullptr };
            if ( gs_radioBoxBorder )
            {
                MUXC::Grid root;
                MUXC::Border frame = wxWinUICreateThemeBrushBorder(
                    "ControlStrokeColorDefaultBrush",
                    wxWinUIThemeBrushProperty::BorderBrush);
                if ( !frame )
                    frame = MUXC::Border();
                MUX::Thickness borderThickness{};
                borderThickness.Left = borderThickness.Top =
                    borderThickness.Right = borderThickness.Bottom = 1;
                frame.BorderThickness(borderThickness);
                MUX::CornerRadius radius{};
                radius.TopLeft = radius.TopRight =
                    radius.BottomLeft = radius.BottomRight = 8;
                frame.CornerRadius(radius);
                MUX::Thickness frameMargin{};
                frameMargin.Top = title.empty() ? 0.0 : 8.0;
                frame.Margin(frameMargin);
                MUX::Thickness framePadding{};
                framePadding.Left = framePadding.Right = 12.0;
                framePadding.Top = title.empty() ? 10.0 : 14.0;
                framePadding.Bottom = 12.0;
                frame.Padding(framePadding);
                frame.Child(grid);
                root.Children().Append(frame);

                if ( !title.empty() )
                {
                    MUXC::Border labelBackground =
                        hasBackground
                            ? MUXC::Border()
                            : wxWinUICreateThemeBrushBorder(
                                  "SolidBackgroundFillColorBaseBrush",
                                  wxWinUIThemeBrushProperty::Background);
                    if ( !labelBackground )
                        labelBackground = MUXC::Border();
                    if ( hasBackground )
                    {
                        wxWinUIApplyBackground(
                            labelBackground, background);
                    }
                    MUXA::AutomationProperties::SetAccessibilityView(
                        labelBackground, MUXAP::AccessibilityView::Raw);
                    labelBackground.VerticalAlignment(
                        MUX::VerticalAlignment::Top);
                    labelBackground.HorizontalAlignment(
                        MUX::HorizontalAlignment::Left);
                    MUX::Thickness labelMargin{};
                    labelMargin.Left = 8.0;
                    labelBackground.Margin(labelMargin);
                    MUX::Thickness labelPadding{};
                    labelPadding.Left = labelPadding.Right = 4.0;
                    labelBackground.Padding(labelPadding);

                    MUXC::TextBlock text;
                    text.Text(wxWinUIToHString(title));
                    wxWinUIApplyFont(text, font);
                    wxWinUIApplyForeground(text, foreground);
                    MUXA::AutomationProperties::SetAccessibilityView(
                        text, MUXAP::AccessibilityView::Raw);
                    labelBackground.Child(text);
                    root.Children().Append(labelBackground);
                    candidate.titleText = text;
                }
                visualRoot = root;
            }
            else
            {
                MUXC::StackPanel panel;
                panel.Orientation(MUXC::Orientation::Vertical);
                if ( !title.empty() )
                {
                    MUXC::TextBlock header;
                    header.Text(wxWinUIToHString(title));
                    wxWinUIApplyFont(header, font);
                    wxWinUIApplyForeground(header, foreground);
                    MUXA::AutomationProperties::SetAccessibilityView(
                        header, MUXAP::AccessibilityView::Raw);
                    MUX::Thickness headerMargin{};
                    headerMargin.Bottom = 6.0;
                    header.Margin(headerMargin);
                    panel.Children().Append(header);
                    candidate.titleText = header;
                }
                panel.Children().Append(grid);
                visualRoot = panel;
            }

            candidate.background = MUXC::Border();
            candidate.background.IsHitTestVisible(false);
            wxWinUIApplyBackground(candidate.background, background);
            MUXA::AutomationProperties::SetAccessibilityView(
                candidate.background, MUXAP::AccessibilityView::Raw);
            candidate.background.Child(visualRoot);

            MUXC::Grid automationRoot = wxWinUICreateAccessibleGrid(
                MUXAP::AutomationControlType::Group, "wxRadioBox");
            automationRoot.Children().Append(candidate.background);
            wxWinUIApplyAccessKey(automationRoot, rawTitle);
            candidate.root = automationRoot;

            ApplyState state = checkState();
            if ( state == ApplyState::Dead )
                return false;
            if ( state == ApplyState::Restart )
                continue;

            const wxWinUIRadioBoxPeerWriteHookForTesting hook =
                impl->nextPeerWriteHookForTesting;
            void * const hookContext =
                impl->nextPeerWriteContextForTesting;
            impl->nextPeerWriteHookForTesting = nullptr;
            impl->nextPeerWriteContextForTesting = nullptr;
            if ( hook )
            {
                hook(hookContext);
                state = checkState();
                if ( state == ApplyState::Dead )
                    return false;
                if ( state == ApplyState::Restart )
                    continue;
            }

            if ( !impl->host.SetContent(candidate.root) )
                return false;

            wxRadioBox * const owner =
                callbackState->GetOwner(lifetimeGeneration);
            if ( !owner || !owner->m_winui ||
                 owner->m_winui.get() != impl ||
                 owner->m_winui->callbackState != callbackState ||
                 impl->generation != baseGeneration )
            {
                return false;
            }

            std::vector<MUXC::RadioButton> oldButtons =
                std::move(impl->buttons);
            std::vector<winrt::event_token> oldCheckedTokens =
                std::move(impl->checkedTokens);
            // Retain every old projected object until after the generation is
            // committed. Assigning the new handles must not make a final COM
            // release (and an Unloaded callback) occur halfway through the
            // publication transaction.
            MUX::UIElement oldRoot = impl->root;
            MUXC::Border oldBackground = impl->background;
            MUXC::TextBlock oldTitleText = impl->titleText;
            impl->root = candidate.root;
            impl->background = candidate.background;
            impl->titleText = candidate.titleText;
            impl->buttons = std::move(candidate.buttons);
            impl->checkedTokens = std::move(candidate.checkedTokens);
            impl->generation = newGeneration;

            // Commit the generation before revocation: even if a provider
            // retains an old peer after removal fails, its callback is inert.
            wxWinUIRadioBoxImpl::RevokeCheckedHandlers(
                oldButtons, oldCheckedTokens);

            // Revocation and final releases are both callback/logging
            // boundaries. Release them deliberately, then resolve the exact
            // owner again before touching the newly committed generation.
            oldCheckedTokens.clear();
            oldButtons.clear();
            oldTitleText = nullptr;
            oldBackground = nullptr;
            oldRoot = nullptr;
            state = checkState();
            if ( state == ApplyState::Dead )
                return false;
            if ( state == ApplyState::Restart )
                continue;

            wxRadioBox * const ownerAfterCommit =
                callbackState->GetOwner(lifetimeGeneration);
            if ( !ownerAfterCommit || !ownerAfterCommit->m_winui ||
                 ownerAfterCommit->m_winui.get() != impl ||
                 ownerAfterCommit->m_winui->callbackState != callbackState ||
                 ownerAfterCommit->m_winui->generation != newGeneration )
            {
                return false;
            }

            wxWinUIPushRadioPreferredFocus(
                ownerAfterCommit,
                ownerAfterCommit->m_winui->buttons,
                selection);
            state = checkState();
            if ( state == ApplyState::Dead )
                return false;
            if ( state == ApplyState::Restart )
                continue;

            wxRadioBox * const ownerBeforeRender =
                callbackState->GetOwner(lifetimeGeneration);
            if ( !ownerBeforeRender || !ownerBeforeRender->m_winui ||
                 ownerBeforeRender->m_winui.get() != impl ||
                 ownerBeforeRender->m_winui->callbackState != callbackState ||
                 ownerBeforeRender->m_winui->generation != newGeneration )
            {
                return false;
            }
            ownerBeforeRender->m_winui->host.ForceRender();
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
            wxWinUILogException("WinUI RadioBox item update", e);
            return false;
        }
    }

    return deferLatestRevision();
}

bool wxRadioBox::WinUIGetAppearanceForTesting(
    wxWinUIAppearanceSnapshot *snapshot,
    bool *titleIsRaw) const
{
    if ( !snapshot || !m_winui || m_winui->buttons.empty() ||
         !m_winui->background || !m_winui->root )
    {
        return false;
    }

    try
    {
        *snapshot = wxWinUICaptureAppearance(
            m_winui->buttons.front(),
            m_winui->root);
        snapshot->hasBackground =
            wxWinUICaptureAppearance(
                MUXC::TextBlock(), m_winui->background,
                m_winui->root).hasBackground;

        if ( titleIsRaw )
        {
            *titleIsRaw =
                !m_winui->titleText ||
                MUXA::AutomationProperties::GetAccessibilityView(
                    m_winui->titleText) ==
                    MUXAP::AccessibilityView::Raw;
        }
        return true;
    }
    catch ( const winrt::hresult_error& )
    {
        return false;
    }
}

bool wxRadioBox::WinUIGetPeerStateForTesting(
    wxArrayString *strings,
    int *selection,
    unsigned long long *generation) const
{
    if ( !m_winui )
        return false;

    try
    {
        if ( strings )
        {
            strings->Clear();
            for ( const MUXC::RadioButton& button : m_winui->buttons )
            {
                const winrt::hstring content =
                    winrt::unbox_value<winrt::hstring>(button.Content());
                strings->Add(wxString(content.c_str()));
            }
        }

        if ( selection )
        {
            *selection = wxNOT_FOUND;
            for ( size_t i = 0; i < m_winui->buttons.size(); ++i )
            {
                const auto checked = m_winui->buttons[i].IsChecked();
                if ( checked && checked.Value() )
                {
                    *selection = static_cast<int>(i);
                    break;
                }
            }
        }

        if ( generation )
            *generation = m_winui->generation;
        return true;
    }
    catch ( const winrt::hresult_error& )
    {
        return false;
    }
}

bool wxRadioBox::WinUISelectItemForTesting(unsigned int item)
{
    if ( !m_winui || !m_winui->callbackState ||
         item >= m_winui->buttons.size() )
    {
        return false;
    }

    wxWinUIRadioBoxImpl * const impl = m_winui.get();
    const std::shared_ptr<wxWinUIRadioBoxCallbackState> callbackState =
        impl->callbackState;
    const std::uint64_t lifetimeGeneration =
        callbackState->GetGeneration();
    const std::uint64_t peerGeneration = impl->generation;
    const MUXC::RadioButton button = impl->buttons[item];

    try
    {
        button.IsChecked(true);
    }
    catch ( const winrt::hresult_error& )
    {
        return false;
    }

    wxRadioBox * const owner =
        callbackState->GetOwner(lifetimeGeneration);
    return owner && owner->m_winui &&
           owner->m_winui.get() == impl &&
           owner->m_winui->callbackState == callbackState &&
           owner->m_winui->generation == peerGeneration &&
           item < owner->m_winui->buttons.size() &&
           owner->m_winui->buttons[item] == button &&
           owner->m_selection == static_cast<int>(item);
}

void wxRadioBox::WinUISetNextPeerWriteHookForTesting(
    wxWinUIRadioBoxPeerWriteHookForTesting hook,
    void *context)
{
    if ( !m_winui )
        return;

    m_winui->nextPeerWriteHookForTesting = hook;
    m_winui->nextPeerWriteContextForTesting =
        hook ? context : nullptr;
}

bool wxRadioBox::WinUIHasDeferredPeerWriteForTesting() const
{
    return m_winui && m_winui->callbackState &&
           m_winui->callbackState->HasDeferredApply();
}

bool wxRadioBox::WinUIIsPeerProjectionQuarantinedForTesting() const
{
    return m_winui && m_winui->callbackState &&
           m_winui->callbackState->IsApplyQuarantined();
}

unsigned long long wxRadioBox::WinUIGetModelRevisionForTesting() const
{
    return m_winui && m_winui->callbackState
        ? m_winui->callbackState->GetModelRevision()
        : 0;
}

void wxRadioBox::WinUIGetCheckedHandlerCountsForTesting(
    unsigned long long *added,
    unsigned long long *revoked) const
{
    if ( added )
    {
        *added = gs_checkedHandlersAdded.load(
            std::memory_order_relaxed);
    }
    if ( revoked )
    {
        *revoked = gs_checkedHandlersRevoked.load(
            std::memory_order_relaxed);
    }
}

#if wxUSE_TOOLTIPS
bool wxRadioBox::HasToolTips() const
{
    return wxControl::HasToolTips() || wxRadioBoxBase::HasItemToolTips();
}

void wxRadioBox::DoSetItemToolTip(unsigned int item, wxToolTip * WXUNUSED(toolTip))
{
    wxCHECK_RET( item < GetCount(), "invalid RadioBox tooltip item" );

    // Rebuild transactionally instead of mutating a retained child peer in
    // place. This keeps item tooltips attached to the same content generation
    // as labels, selection and UIA callbacks, and automatically retires the
    // managed tooltip if this request is superseded or destroys the control.
    BumpWinUIModelRevision();
    RebuildItems();
}

void wxRadioBox::DoSetToolTipText(const wxString& tip)
{
    wxControl::DoSetToolTipText(tip);
}

void wxRadioBox::DoSetToolTip(wxToolTip *tip)
{
    wxControl::DoSetToolTip(tip);
}
#endif // wxUSE_TOOLTIPS

int wxRadioBox::FindSelectedItem() const
{
    if ( !m_winui )
        return wxNOT_FOUND;

    for ( size_t i = 0; i < m_winui->buttons.size(); ++i )
    {
        const auto checked = m_winui->buttons[i].IsChecked();
        if ( checked && checked.Value() )
            return static_cast<int>(i);
    }

    return wxNOT_FOUND;
}

void wxRadioBox::SendSelectionEvent()
{
    wxCommandEvent event(wxEVT_RADIOBOX, GetId());
    event.SetEventObject(this);
    event.SetInt(m_selection);
    if ( m_selection != wxNOT_FOUND )
        event.SetString(m_strings[m_selection]);
    ProcessCommand(event);
}

#endif // wxUSE_RADIOBOX
