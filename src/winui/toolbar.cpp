/////////////////////////////////////////////////////////////////////////////
// Name:        src/winui/toolbar.cpp
// Purpose:     wxWinUI wxToolBar implementation
// Author:      wxWidgets development team
// Created:     2026-07-20
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_TOOLBAR

#include "wx/toolbar.h"
#include "wx/bitmap.h"
#include "wx/scopeguard.h"
#include "wx/weakref.h"
#if wxUSE_TOOLTIPS
    #include "wx/tooltip.h"
#endif

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/event.h"
    #include "wx/log.h"
#endif

#if wxUSE_MENUS
    #include "wx/menu.h"
#endif

#include "private.h"
#ifdef WXWINUI_TEST_SUPPORT
    #include "../../tests/winui/test-support/toolbar-test-access.h"
#endif
#include "wx/winui/private/appearance.h"
#include "wx/winui/private/tlwhost.h"

#include <winrt/Microsoft.UI.Xaml.Automation.h>
#include <winrt/Microsoft.UI.Xaml.Automation.Peers.h>
#ifdef WXWINUI_TEST_SUPPORT
#include <winrt/Microsoft.UI.Xaml.Automation.Provider.h>
#endif
#include <winrt/Microsoft.UI.Xaml.Input.h>

#include <algorithm>
#include <atomic>
#include <cmath>
#include <cstdint>
#include <limits>
#include <memory>
#include <set>
#include <utility>
#include <vector>

namespace MUX = winrt::Microsoft::UI::Xaml;
namespace MUXA = winrt::Microsoft::UI::Xaml::Automation;
namespace MUXAP = winrt::Microsoft::UI::Xaml::Automation::Peers;
#ifdef WXWINUI_TEST_SUPPORT
namespace MUXAPR = winrt::Microsoft::UI::Xaml::Automation::Provider;
#endif
namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;
namespace MUXCP = winrt::Microsoft::UI::Xaml::Controls::Primitives;
namespace MUXI = winrt::Microsoft::UI::Xaml::Input;
namespace MUXM = winrt::Microsoft::UI::Xaml::Media;
namespace WF = winrt::Windows::Foundation;

namespace
{

constexpr int wxWINUI_TOOL_HEIGHT = 48;
constexpr int wxWINUI_TOOL_HEIGHT_LABEL_BELOW = 68;
constexpr int wxWINUI_TOOL_MIN_WIDTH = 48;
constexpr int wxWINUI_TOOL_SEPARATOR_WIDTH = 12;
constexpr int wxWINUI_TOOL_DROPDOWN_WIDTH = 24;
constexpr int wxWINUI_TOOL_OVERFLOW_WIDTH = 40;

std::atomic<std::uint64_t> gs_nextToolPeerKey{0};
#ifdef WXWINUI_TEST_SUPPORT
std::atomic<std::size_t> gs_liveToolBarCallbackStates{0};
#endif

class wxWinUIToolBarCallbackState final
{
public:
    explicit wxWinUIToolBarCallbackState(wxToolBar *owner)
        : m_owner(owner)
    {
#ifdef WXWINUI_TEST_SUPPORT
        gs_liveToolBarCallbackStates.fetch_add(
            1, std::memory_order_relaxed);
#endif
    }

    ~wxWinUIToolBarCallbackState()
    {
        Invalidate();
#ifdef WXWINUI_TEST_SUPPORT
        gs_liveToolBarCallbackStates.fetch_sub(
            1, std::memory_order_relaxed);
#endif
    }

    std::uint64_t PrepareGeneration()
    {
        std::uint64_t generation =
            m_nextGeneration.fetch_add(1, std::memory_order_acq_rel) + 1;
        if ( generation == 0 )
        {
            generation =
                m_nextGeneration.fetch_add(1, std::memory_order_acq_rel) + 1;
        }
        return generation;
    }

    void CommitGeneration(std::uint64_t generation)
    {
        m_generation.store(generation, std::memory_order_release);
    }

    std::uint64_t GetCommittedGeneration() const
    {
        return m_generation.load(std::memory_order_acquire);
    }

    wxToolBar *GetOwner(std::uint64_t generation) const
    {
        if ( generation == 0 ||
             generation != m_generation.load(std::memory_order_acquire) ||
             m_peerMutationDepth.load(std::memory_order_acquire) != 0 )
        {
            return nullptr;
        }

        return m_owner.load(std::memory_order_acquire);
    }

    wxToolBar *GetLiveOwner() const
    {
        return m_owner.load(std::memory_order_acquire);
    }

    bool IsRebuildActive() const
    {
        return m_activeRebuildGeneration.load(
                   std::memory_order_acquire) != 0;
    }

    bool TryBeginLabelSync()
    {
        bool expected = false;
        return m_labelSync.compare_exchange_strong(
            expected, true, std::memory_order_acq_rel);
    }

    void EndLabelSync()
    {
        m_labelSync.store(false, std::memory_order_release);
    }

    bool IsLabelSyncActive() const
    {
        return m_labelSync.load(std::memory_order_acquire);
    }

    bool TryBeginRebuild(std::uint64_t generation)
    {
        if ( generation == 0 )
            return false;

        std::uint64_t expected = 0;
        return m_activeRebuildGeneration.compare_exchange_strong(
            expected, generation, std::memory_order_acq_rel);
    }

    void EndRebuild(std::uint64_t generation)
    {
        std::uint64_t expected = generation;
        const bool ended =
            m_activeRebuildGeneration.compare_exchange_strong(
                expected, 0, std::memory_order_acq_rel);
        wxASSERT_MSG(ended || expected == 0,
                     "unbalanced WinUI toolbar rebuild");
    }

    wxToolBar *GetOwnerDuringRebuild(std::uint64_t generation) const
    {
        if ( generation == 0 ||
             generation !=
                 m_activeRebuildGeneration.load(std::memory_order_acquire) )
        {
            return nullptr;
        }

        return m_owner.load(std::memory_order_acquire);
    }

    wxToolBar *GetOwnerForModelMutation(std::uint64_t generation) const
    {
        if ( generation == 0 )
            return nullptr;

        if ( generation != m_generation.load(std::memory_order_acquire) &&
             generation !=
                 m_activeRebuildGeneration.load(std::memory_order_acquire) )
        {
            return nullptr;
        }
        return m_owner.load(std::memory_order_acquire);
    }

    void Invalidate()
    {
        m_owner.store(nullptr, std::memory_order_release);
        m_generation.store(0, std::memory_order_release);
        m_activeRebuildGeneration.store(0, std::memory_order_release);
        m_nextGeneration.fetch_add(1, std::memory_order_acq_rel);
    }

    bool TryBeginControlSync()
    {
        bool expected = false;
        return m_controlSync.compare_exchange_strong(
            expected, true, std::memory_order_acq_rel);
    }

    void EndControlSync()
    {
        m_controlSync.store(false, std::memory_order_release);
    }

    bool TryBeginOverflowSync()
    {
        bool expected = false;
        return m_overflowSync.compare_exchange_strong(
            expected, true, std::memory_order_acq_rel);
    }

    void EndOverflowSync()
    {
        m_overflowSync.store(false, std::memory_order_release);
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
                     "unbalanced WinUI toolbar peer mutation");
    }

private:
    std::atomic<wxToolBar *> m_owner;
    std::atomic<std::uint64_t> m_generation{0};
    std::atomic<std::uint64_t> m_nextGeneration{0};
    std::atomic<std::uint64_t> m_activeRebuildGeneration{0};
    std::atomic<bool> m_controlSync{false};
    std::atomic<bool> m_overflowSync{false};
    std::atomic<bool> m_labelSync{false};
    std::atomic<unsigned> m_peerMutationDepth{0};
};

class wxWinUIToolBarRebuildGuard final
{
public:
    wxWinUIToolBarRebuildGuard(
        std::shared_ptr<wxWinUIToolBarCallbackState> state,
        std::uint64_t generation)
        : m_state(std::move(state)),
          m_generation(generation),
          m_acquired(m_state->TryBeginRebuild(generation))
    {
    }

    ~wxWinUIToolBarRebuildGuard()
    {
        if ( m_acquired )
            m_state->EndRebuild(m_generation);
    }

    explicit operator bool() const { return m_acquired; }

private:
    std::shared_ptr<wxWinUIToolBarCallbackState> m_state;
    std::uint64_t m_generation;
    bool m_acquired;
};

class wxWinUIToolBarControlSyncGuard final
{
public:
    explicit wxWinUIToolBarControlSyncGuard(
        std::shared_ptr<wxWinUIToolBarCallbackState> state)
        : m_state(std::move(state))
    {
    }

    ~wxWinUIToolBarControlSyncGuard()
    {
        m_state->EndControlSync();
    }

private:
    std::shared_ptr<wxWinUIToolBarCallbackState> m_state;
};

class wxWinUIToolBarOverflowSyncGuard final
{
public:
    explicit wxWinUIToolBarOverflowSyncGuard(
        std::shared_ptr<wxWinUIToolBarCallbackState> state)
        : m_state(std::move(state))
    {
    }

    ~wxWinUIToolBarOverflowSyncGuard()
    {
        m_state->EndOverflowSync();
    }

private:
    std::shared_ptr<wxWinUIToolBarCallbackState> m_state;
};

class wxWinUIToolBarPeerMutationGuard final
{
public:
    explicit wxWinUIToolBarPeerMutationGuard(
        std::shared_ptr<wxWinUIToolBarCallbackState> state)
        : m_state(std::move(state))
    {
        m_state->BeginPeerMutation();
    }

    ~wxWinUIToolBarPeerMutationGuard()
    {
        m_state->EndPeerMutation();
    }

private:
    std::shared_ptr<wxWinUIToolBarCallbackState> m_state;
};

} // anonymous namespace

wxIMPLEMENT_DYNAMIC_CLASS(wxToolBar, wxControl);

// ----------------------------------------------------------------------------
// wxWinUIToolBarTool
// ----------------------------------------------------------------------------

class wxWinUIToolBarTool final : public wxToolBarToolBase
{
private:
    class ControlLifetime final : public wxWeakRef<wxWindow>
    {
    public:
        ControlLifetime(wxWinUIToolBarTool* const owner,
                        wxWindow* const control)
            : wxWeakRef<wxWindow>(control),
              m_owner(owner)
        {
        }

        void OnObjectDestroy() override
        {
            wxWeakRef<wxWindow>::OnObjectDestroy();
            m_owner->OnControlDestroyed();
        }

    private:
        wxWinUIToolBarTool* const m_owner;
    };

    ControlLifetime m_controlLifetime;

public:
    wxWinUIToolBarTool(wxToolBar *tbar,
                       int toolid,
                       const wxString& label,
                       const wxBitmapBundle& bmpNormal,
                       const wxBitmapBundle& bmpDisabled,
                       wxItemKind kind,
                       wxObject *clientData,
                       const wxString& shortHelp,
                       const wxString& longHelp)
        : wxToolBarToolBase(tbar, toolid, label, bmpNormal, bmpDisabled, kind,
                            clientData, shortHelp, longHelp),
          m_controlLifetime(this, nullptr),
          peerKey(gs_nextToolPeerKey.fetch_add(
                      1, std::memory_order_relaxed) + 1)
    {
    }

    wxWinUIToolBarTool(wxToolBar *tbar,
                       wxControl *control,
                       const wxString& label)
        : wxToolBarToolBase(tbar, control, label),
          m_controlLifetime(this, control),
          peerKey(gs_nextToolPeerKey.fetch_add(
                      1, std::memory_order_relaxed) + 1)
    {
    }

    ~wxWinUIToolBarTool() override
    {
        m_lifetimeToken.reset();
        // The tracker normally neutralizes m_control at the instant an
        // application destroys it. Keep the destructor independently safe if
        // a future tracker implementation delays that notification.
        ForgetControlIfDestroyed();
    }

    wxControl *GetLiveControl() const
    {
        wxWindow* const live = m_controlLifetime.get();
        return live && live == m_control
            ? static_cast<wxControl*>(live)
            : nullptr;
    }

    std::weak_ptr<void> GetLifetimeToken() const
    {
        return m_lifetimeToken;
    }

    std::uint64_t GetOwnershipEpoch() const
    {
        return m_ownershipEpoch;
    }

    bool TryClaimInsertion(wxToolBar *toolbar)
    {
        if ( !toolbar || m_insertionClaim )
            return false;

        m_insertionClaim = toolbar;
        return true;
    }

    bool IsInsertionClaimedBy(const wxToolBar *toolbar) const
    {
        return m_insertionClaim == toolbar;
    }

    void ReleaseInsertionClaim(const wxToolBar *toolbar)
    {
        if ( m_insertionClaim == toolbar )
            m_insertionClaim = nullptr;
    }

    void Attach(wxToolBarBase *toolbar) override
    {
        wxToolBarToolBase::Attach(toolbar);
        BumpOwnershipEpoch();
    }

    void Detach() override
    {
        wxToolBarToolBase::Detach();
        BumpOwnershipEpoch();
    }

    void SetLabel(const wxString& label) override
    {
        wxToolBar * const toolbar =
            static_cast<wxToolBar *>(GetToolBar());
        if ( !toolbar )
        {
            if ( GetLabel() == label )
                return;
            wxToolBarToolBase::SetLabel(label);
            if ( ++labelRevision == 0 )
                ++labelRevision;
            return;
        }

        toolbar->DoSetToolLabel(this, label);
    }

    void ForgetControlIfDestroyed()
    {
        if ( IsControl() && !GetLiveControl() )
        {
            ForgetDestroyedControl();
            m_controlLifetime.Release();
        }
    }

    bool SetToggle(bool toggle) override
    {
        const wxItemKind oldKind = m_kind;
        if ( !wxToolBarToolBase::SetToggle(toggle) )
            return false;

        m_pendingSetToggleKinds.push_back(oldKind);
        return true;
    }

    void CommitSetToggle()
    {
        wxASSERT_MSG(!m_pendingSetToggleKinds.empty(),
                     "missing WinUI toolbar SetToggle transaction");
        if ( !m_pendingSetToggleKinds.empty() )
            m_pendingSetToggleKinds.pop_back();
    }

    void RollbackSetToggle()
    {
        wxASSERT_MSG(!m_pendingSetToggleKinds.empty(),
                     "missing WinUI toolbar SetToggle rollback");
        if ( m_pendingSetToggleKinds.empty() )
            return;

        m_kind = m_pendingSetToggleKinds.back();
        m_pendingSetToggleKinds.pop_back();
    }

    const std::uint64_t peerKey;
    std::uint64_t shortHelpRevision = 0;
    std::uint64_t enabledRevision = 0;
    std::uint64_t labelRevision = 0;
    unsigned shortHelpProjectionFailures = 0;
    unsigned enabledProjectionFailures = 0;
#ifdef WXWINUI_TEST_SUPPORT
    unsigned shortHelpSetterFaultsForTesting = 0;
#endif
    bool shortHelpProjectionQuarantined = false;
    bool enabledProjectionQuarantined = false;
    bool controlHiddenByToolbar = false;
    bool controlShowPending = false;
#if wxUSE_TOOLTIPS
    bool controlToolTipOwned = false;
    wxToolTip *controlToolTipObject = nullptr;
    unsigned long long controlToolTipObjectIdentity = 0;
    wxToolTip *controlToolTipBaselineObject = nullptr;
    unsigned long long controlToolTipBaselineIdentity = 0;
    wxString controlToolTipBaselineText;
    wxString controlToolTipAppliedText;
    bool controlToolTipCreated = false;
#endif

private:
    void BumpOwnershipEpoch()
    {
        if ( ++m_ownershipEpoch == 0 )
            ++m_ownershipEpoch;
    }

    void OnControlDestroyed()
    {
        if ( IsControl() )
            ForgetDestroyedControl();
    }

    std::vector<wxItemKind> m_pendingSetToggleKinds;
    std::shared_ptr<void> m_lifetimeToken =
        std::make_shared<unsigned char>(0);
    std::uint64_t m_ownershipEpoch = 1;
    wxToolBar *m_insertionClaim = nullptr;
};

#if wxUSE_TOOLTIPS

void wxWinUIClearControlToolTipOwnership(wxWinUIToolBarTool& tool)
{
    tool.controlToolTipOwned = false;
    tool.controlToolTipObject = nullptr;
    tool.controlToolTipObjectIdentity = 0;
    tool.controlToolTipBaselineObject = nullptr;
    tool.controlToolTipBaselineIdentity = 0;
    tool.controlToolTipBaselineText.clear();
    tool.controlToolTipAppliedText.clear();
    tool.controlToolTipCreated = false;
}

bool wxWinUIReleaseControlToolTip(wxWinUIToolBarTool& tool)
{
    if ( !tool.controlToolTipOwned )
        return false;

    wxControl * const control = tool.GetLiveControl();
    wxToolTip * const ownedObject = tool.controlToolTipObject;
    const unsigned long long ownedIdentity =
        tool.controlToolTipObjectIdentity;
    wxToolTip * const baselineObject = tool.controlToolTipBaselineObject;
    const unsigned long long baselineIdentity =
        tool.controlToolTipBaselineIdentity;
    const wxString baselineText = tool.controlToolTipBaselineText;
    const wxString appliedText = tool.controlToolTipAppliedText;
    const bool created = tool.controlToolTipCreated;

    // Retire ownership before crossing a wx tooltip setter: any reentrant
    // toolbar removal/destruction then sees a completed release transaction.
    wxWinUIClearControlToolTipOwnership(tool);

    if ( !control )
        return false;
    wxToolTip * const current = control->GetToolTip();
    if ( !current ||
         current != ownedObject ||
         current->GetWinUIIdentity() != ownedIdentity ||
         current->GetTip() != appliedText )
    {
        // Replacement object or in-place text mutation is application state.
        return false;
    }

    if ( created )
    {
        control->UnsetToolTip();
    }
    else if ( baselineObject == current &&
              current->GetWinUIIdentity() == baselineIdentity )
    {
        current->SetTip(baselineText);
    }
    return true;
}

enum class wxWinUIControlToolTipApplyResult
{
    NoSetter,
    SetterCalled,
    CaptureCreated
};

wxWinUIControlToolTipApplyResult
wxWinUIApplyControlToolTip(wxWinUIToolBarTool& tool,
                           const wxString& tip,
                           wxToolTip **createdObject,
                           unsigned long long *createdIdentity)
{
    if ( createdObject )
        *createdObject = nullptr;
    if ( createdIdentity )
        *createdIdentity = 0;
    wxControl * const control = tool.GetLiveControl();
    if ( !control )
        return wxWinUIControlToolTipApplyResult::NoSetter;

    if ( tip.empty() )
    {
        return wxWinUIReleaseControlToolTip(tool)
            ? wxWinUIControlToolTipApplyResult::SetterCalled
            : wxWinUIControlToolTipApplyResult::NoSetter;
    }

    if ( tool.controlToolTipOwned )
    {
        wxToolTip * const current = control->GetToolTip();
        if ( !current ||
             current != tool.controlToolTipObject ||
             current->GetWinUIIdentity() !=
                 tool.controlToolTipObjectIdentity ||
             current->GetTip() != tool.controlToolTipAppliedText )
        {
            // A direct replacement/mutation wins permanently for this
            // ownership interval. Do not overwrite it with toolbar metadata.
            wxWinUIClearControlToolTipOwnership(tool);
            return wxWinUIControlToolTipApplyResult::NoSetter;
        }

        tool.controlToolTipAppliedText = tip;
        current->SetTip(tip);
        return wxWinUIControlToolTipApplyResult::SetterCalled;
    }

    wxToolTip * const baseline = control->GetToolTip();
    const wxString baselineText =
        baseline ? baseline->GetTip() : wxString();

    if ( baseline )
    {
        tool.controlToolTipOwned = true;
        tool.controlToolTipObject = baseline;
        tool.controlToolTipObjectIdentity =
            baseline->GetWinUIIdentity();
        tool.controlToolTipBaselineObject = baseline;
        tool.controlToolTipBaselineIdentity =
            baseline->GetWinUIIdentity();
        tool.controlToolTipBaselineText = baselineText;
        tool.controlToolTipAppliedText = tip;
        tool.controlToolTipCreated = false;
        baseline->SetTip(tip);
        return wxWinUIControlToolTipApplyResult::SetterCalled;
    }

    // Allocate the candidate explicitly so completion can prove identity.
    // An application override may synchronously replace it with another
    // wxToolTip carrying the same text; text equality must never grant the
    // toolbar ownership of that application object.
    wxToolTip * const created = new wxToolTip(tip);
    if ( createdObject )
        *createdObject = created;
    if ( createdIdentity )
        *createdIdentity = created->GetWinUIIdentity();
    // Do not touch `created` after this virtual setter: an override may have
    // adopted, replaced or destroyed it.
    control->SetToolTip(created);
    return wxWinUIControlToolTipApplyResult::CaptureCreated;
}

void wxWinUICompleteControlToolTipApply(
    wxWinUIToolBarTool& tool,
    const wxString& tip,
    wxWinUIControlToolTipApplyResult result,
    wxToolTip *createdObject,
    unsigned long long createdIdentity)
{
    wxControl * const control = tool.GetLiveControl();
    if ( !control || result == wxWinUIControlToolTipApplyResult::NoSetter )
        return;

    wxToolTip * const current = control->GetToolTip();
    if ( result == wxWinUIControlToolTipApplyResult::CaptureCreated )
    {
        if ( current && current == createdObject &&
             current->GetWinUIIdentity() == createdIdentity &&
             current->GetTip() == tip )
        {
            tool.controlToolTipOwned = true;
            tool.controlToolTipObject = current;
            tool.controlToolTipObjectIdentity = createdIdentity;
            tool.controlToolTipBaselineObject = nullptr;
            tool.controlToolTipBaselineIdentity = 0;
            tool.controlToolTipBaselineText.clear();
            tool.controlToolTipAppliedText = tip;
            tool.controlToolTipCreated = true;
        }
        return;
    }

    if ( tool.controlToolTipOwned &&
         (!current ||
          current != tool.controlToolTipObject ||
          current->GetWinUIIdentity() !=
              tool.controlToolTipObjectIdentity ||
          current->GetTip() != tool.controlToolTipAppliedText) )
    {
        // An application replacement/mutation made from the setter boundary
        // wins. Relinquish ownership without changing it.
        wxWinUIClearControlToolTipOwnership(tool);
    }
}

#endif // wxUSE_TOOLTIPS

// ----------------------------------------------------------------------------
// Peer model and cleanup
// ----------------------------------------------------------------------------

namespace
{

struct wxWinUIToolPeer
{
    wxWinUIToolBarTool *tool = nullptr;
    MUX::FrameworkElement element{ nullptr };
    MUX::FrameworkElement controlPlaceholder{ nullptr };
    MUXCP::ButtonBase primaryButton{ nullptr };
    MUXCP::ButtonBase dropdownButton{ nullptr };
    MUXC::StackPanel radioContent{ nullptr };
    MUXC::TextBlock radioLabel{ nullptr };
    winrt::event_token primaryClickToken{};
    winrt::event_token dropdownClickToken{};
    winrt::event_token pointerEnteredToken{};
    winrt::event_token pointerExitedToken{};
    winrt::event_token rightTappedToken{};
    wxSize selectedPixelSize;
    double naturalExtent = 0.0;
    bool hasIcon = false;
    bool usesDisabledBitmap = false;
    bool inOverflow = false;
    bool overflowEligible = false;
};

void wxWinUIRevokeToolPeer(wxWinUIToolPeer& peer)
{
    // Explicitly retire helper-registry entries while the elements are still
    // alive. Depending on a later opportunistic weak purge would retain the
    // ToolTip objects and make rebuild/destroy cleanup non-deterministic.
    wxWinUIForgetManagedToolTip(peer.element);
    wxWinUIForgetManagedToolTip(peer.primaryButton);
    wxWinUIForgetManagedToolTip(peer.dropdownButton);

    if ( peer.primaryButton && peer.primaryClickToken.value )
    {
        try
        {
            peer.primaryButton.Click(peer.primaryClickToken);
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException("WinUI toolbar Click removal", e);
        }
    }
    peer.primaryClickToken = {};

    if ( peer.dropdownButton && peer.dropdownClickToken.value )
    {
        try
        {
            peer.dropdownButton.Click(peer.dropdownClickToken);
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException(
                "WinUI toolbar dropdown Click removal", e);
        }
    }
    peer.dropdownClickToken = {};

    if ( peer.element && peer.pointerEnteredToken.value )
    {
        try
        {
            peer.element.PointerEntered(peer.pointerEnteredToken);
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException(
                "WinUI toolbar PointerEntered removal", e);
        }
    }
    peer.pointerEnteredToken = {};

    if ( peer.element && peer.pointerExitedToken.value )
    {
        try
        {
            peer.element.PointerExited(peer.pointerExitedToken);
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException(
                "WinUI toolbar PointerExited removal", e);
        }
    }
    peer.pointerExitedToken = {};

    if ( peer.element && peer.rightTappedToken.value )
    {
        try
        {
            peer.element.RightTapped(peer.rightTappedToken);
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException(
                "WinUI toolbar RightTapped removal", e);
        }
    }
    peer.rightTappedToken = {};
}

void wxWinUIRevokeToolPeers(std::vector<wxWinUIToolPeer>& peers)
{
    for ( wxWinUIToolPeer& peer : peers )
        wxWinUIRevokeToolPeer(peer);
}

wxBitmap wxWinUISelectToolBitmap(const wxWinUIToolBarTool& tool,
                                 const wxToolBar& owner,
                                 double requestedScale,
                                 bool *usesDisabled)
{
    const bool disabled =
        !tool.IsEnabled() ||
        !owner.IsEnabled();
    wxBitmapBundle bundle;
    bool synthesizeDisabled = false;

    if ( disabled && tool.GetDisabledBitmapBundle().IsOk() )
    {
        bundle = tool.GetDisabledBitmapBundle();
    }
    else
    {
        bundle = tool.GetNormalBitmapBundle();
        synthesizeDisabled = disabled && bundle.IsOk();
    }

    wxBitmap bitmap;
    if ( bundle.IsOk() )
    {
        const wxSize configuredSize = owner.GetToolBitmapSize();
        if ( configuredSize.x > 0 && configuredSize.y > 0 )
        {
            const wxSize requestedSize =
                requestedScale > 0.0
                    ? wxSize(
                          std::max(
                              1,
                              static_cast<int>(std::lround(
                                  configuredSize.x * requestedScale))),
                          std::max(
                              1,
                              static_cast<int>(std::lround(
                                  configuredSize.y * requestedScale))))
                    : owner.FromDIP(configuredSize);
            bitmap = bundle.GetBitmap(requestedSize);
        }
        else
        {
            bitmap = requestedScale > 0.0
                ? bundle.GetBitmap(
                      bundle.GetPreferredBitmapSizeAtScale(requestedScale))
                : bundle.GetBitmapFor(&owner);
        }
    }

    if ( synthesizeDisabled && bitmap.IsOk() )
        bitmap = bitmap.ConvertToDisabled();

    if ( usesDisabled )
        *usesDisabled = disabled && bitmap.IsOk();
    return bitmap;
}

bool wxWinUIBuildToolIcon(const wxWinUIToolBarTool& tool,
                          const wxToolBar& owner,
                          bool showIcons,
                          double requestedScale,
                          MUXC::IconElement *icon,
                          wxSize *pixelSize,
                          bool *usesDisabled)
{
    if ( icon )
        *icon = nullptr;
    if ( pixelSize )
        *pixelSize = wxSize();
    if ( usesDisabled )
        *usesDisabled = false;

    if ( !showIcons )
        return true;

    bool selectedDisabled = false;
    const wxBitmap bitmap =
        wxWinUISelectToolBitmap(
            tool, owner, requestedScale, &selectedDisabled);
    if ( !bitmap.IsOk() )
        return true;

    const auto source = wxWinUIWriteableBitmapFromBitmap(bitmap);
    if ( !source )
        return false;

    MUXC::ImageIcon imageIcon;
    imageIcon.Source(source);
    const wxSize dipSize = bitmap.GetDIPSize();
    if ( dipSize.x > 0 && dipSize.y > 0 )
    {
        imageIcon.Width(dipSize.x);
        imageIcon.Height(dipSize.y);
    }
    if ( icon )
        *icon = imageIcon;
    if ( pixelSize )
        *pixelSize = bitmap.GetSize();
    if ( usesDisabled )
        *usesDisabled = selectedDisabled;
    return true;
}

void wxWinUISetButtonIcon(const MUXCP::ButtonBase& button,
                          const MUXC::IconElement& icon)
{
    if ( const auto normal = button.try_as<MUXC::AppBarButton>() )
        normal.Icon(icon);
    else if ( const auto toggle =
                  button.try_as<MUXC::AppBarToggleButton>() )
        toggle.Icon(icon);
}

MUXC::StackPanel wxWinUIBuildHorizontalToolContent(
    const wxWinUIToolBarTool& tool,
    const MUXC::IconElement& icon,
    MUXC::TextBlock *labelOut)
{
    MUXC::StackPanel content;
    content.Orientation(MUXC::Orientation::Horizontal);
    content.Spacing(6.0);
    content.VerticalAlignment(MUX::VerticalAlignment::Center);
    if ( icon )
        content.Children().Append(icon.as<MUX::UIElement>());

    MUXC::TextBlock text;
    text.Text(wxWinUIToHString(
        wxWinUIRemoveMnemonics(tool.GetLabel())));
    text.VerticalAlignment(MUX::VerticalAlignment::Center);
    content.Children().Append(text);
    if ( labelOut )
        *labelOut = text;
    return content;
}

MUXC::StackPanel wxWinUIBuildRadioToolContent(
    const wxWinUIToolBarTool& tool,
    const MUXC::IconElement& icon,
    bool showText,
    bool horizontalText,
    MUXC::TextBlock *labelOut)
{
    MUXC::StackPanel content;
    content.Orientation(
        horizontalText
            ? MUXC::Orientation::Horizontal
            : MUXC::Orientation::Vertical);
    content.Spacing(horizontalText ? 6.0 : 2.0);
    content.VerticalAlignment(MUX::VerticalAlignment::Center);
    if ( icon )
        content.Children().Append(icon.as<MUX::UIElement>());

    MUXC::TextBlock label;
    label.Text(wxWinUIToHString(
        wxWinUIRemoveMnemonics(tool.GetLabel())));
    label.VerticalAlignment(MUX::VerticalAlignment::Center);
    label.Visibility(
        showText
            ? MUX::Visibility::Visible
            : MUX::Visibility::Collapsed);
    content.Children().Append(label);
    if ( labelOut )
        *labelOut = label;
    return content;
}

template <typename IsCurrent>
wxRect wxWinUIGetToolBounds(const MUXC::Grid& root,
                            const MUX::FrameworkElement& element,
                            const wxToolBar *toolbar,
                            const IsCurrent& isCurrent)
{
    if ( !root || !element || !toolbar || !isCurrent() )
        return wxRect();

    const double width = element.ActualWidth();
    if ( !isCurrent() )
        return wxRect();

    const double height = element.ActualHeight();
    if ( !isCurrent() || !std::isfinite(width) ||
         !std::isfinite(height) || width <= 0.0 || height <= 0.0 )
    {
        return wxRect();
    }

    WF::Rect clientBounds{};
    if ( wxWinUIVisualCoordinates::ElementBoundsToClient(
             const_cast<wxToolBar *>(toolbar),
             element,
             WF::Rect{
                 0.0f,
                 0.0f,
                 static_cast<float>(width),
                 static_cast<float>(height)},
             &clientBounds) != wxWinUICoordinateResult::Mapped ||
         !isCurrent() )
    {
        return wxRect();
    }

    // The public wxRect encloses the full fractional visual bounds.
    const int left =
        static_cast<int>(std::floor(clientBounds.X));
    const int top =
        static_cast<int>(std::floor(clientBounds.Y));
    const int right = static_cast<int>(
        std::ceil(clientBounds.X + clientBounds.Width));
    const int bottom = static_cast<int>(
        std::ceil(clientBounds.Y + clientBounds.Height));
    return wxRect(left, top,
                  wxMax(0, right - left),
                  wxMax(0, bottom - top));
}

template <typename IsCurrent>
bool wxWinUIGetToolRightClickPoint(
    const MUXC::Grid& root,
    const MUXC::Button& overflowButton,
    const wxWinUIToolPeer& peer,
    const MUXI::RightTappedRoutedEventArgs *event,
    const wxToolBar *toolbar,
    wxPoint *point,
    const IsCurrent& isCurrent)
{
    if ( !root || !toolbar || !point || !isCurrent() )
        return false;

    if ( peer.inOverflow )
    {
        // A flyout is a separate XAML visual tree, so GetPosition(root)
        // throws for an overflowed command. wxToolBar coordinates are still
        // client-relative: use the chevron that owns the flyout as the stable
        // toolbar-side anchor.
        if ( !overflowButton )
            return false;

        const wxRect bounds = wxWinUIGetToolBounds(
            root,
            overflowButton.as<MUX::FrameworkElement>(),
            toolbar,
            isCurrent);
        if ( bounds.IsEmpty() )
            return false;

        *point = wxPoint(
            bounds.x + bounds.width / 2,
            bounds.y + bounds.height / 2);
        return true;
    }

    if ( event )
    {
        const WF::Point position = event->GetPosition(root);
        if ( !isCurrent() )
            return false;

        WF::Point clientPoint{};
        if ( wxWinUIVisualCoordinates::ElementPointToClient(
                 const_cast<wxToolBar *>(toolbar),
                 root,
                 position,
                 &clientPoint) != wxWinUICoordinateResult::Mapped ||
             !isCurrent() )
        {
            return false;
        }
        *point = wxPoint(
            static_cast<int>(std::lround(clientPoint.X)),
            static_cast<int>(std::lround(clientPoint.Y)));
        return true;
    }

    const wxRect bounds =
        wxWinUIGetToolBounds(root, peer.element, toolbar, isCurrent);
    if ( bounds.IsEmpty() )
        return false;

    *point = wxPoint(
        bounds.x + bounds.width / 2,
        bounds.y + bounds.height / 2);
    return true;
}

MUX::FlowDirection wxWinUIToolBarFlowDirection(wxLayoutDirection direction)
{
    return direction == wxLayout_RightToLeft
        ? MUX::FlowDirection::RightToLeft
        : MUX::FlowDirection::LeftToRight;
}

#ifdef WXWINUI_TEST_SUPPORT
MUXAP::AutomationPeer wxWinUICreateToolAutomationPeer(
    const wxWinUIToolPeer& peer,
    bool dropdownPart)
{
    if ( dropdownPart )
    {
        if ( const auto button =
                 peer.dropdownButton.try_as<MUXC::Button>() )
        {
            return MUXAP::ButtonAutomationPeer(button);
        }
        return nullptr;
    }

    if ( const auto radio =
             peer.primaryButton.try_as<MUXC::RadioButton>() )
    {
        return MUXAP::RadioButtonAutomationPeer(radio);
    }
    if ( const auto button =
             peer.primaryButton.try_as<MUXC::AppBarButton>() )
    {
        return MUXAP::AppBarButtonAutomationPeer(button);
    }
    if ( const auto toggle =
             peer.primaryButton.try_as<MUXC::AppBarToggleButton>() )
    {
        return MUXAP::AppBarToggleButtonAutomationPeer(toggle);
    }
    if ( const auto button =
             peer.primaryButton.try_as<MUXC::Button>() )
    {
        return MUXAP::ButtonAutomationPeer(button);
    }
    if ( const auto toggle =
             peer.primaryButton.try_as<MUXCP::ToggleButton>() )
    {
        return MUXAP::ToggleButtonAutomationPeer(toggle);
    }

    return peer.primaryButton
        ? MUXAP::FrameworkElementAutomationPeer::CreatePeerForElement(
              peer.primaryButton)
        : nullptr;
}

bool wxWinUIInvokeToolAutomationPeer(const wxWinUIToolPeer& peer,
                                     bool dropdownPart)
{
    if ( dropdownPart )
    {
        if ( const auto button =
                 peer.dropdownButton.try_as<MUXC::Button>() )
        {
            MUXAP::ButtonAutomationPeer(button).Invoke();
            return true;
        }
        return false;
    }
    if ( const auto radio =
             peer.primaryButton.try_as<MUXC::RadioButton>() )
    {
        MUXAP::RadioButtonAutomationPeer(radio).Select();
        return true;
    }
    if ( const auto button =
             peer.primaryButton.try_as<MUXC::AppBarButton>() )
    {
        MUXAP::AppBarButtonAutomationPeer(button).Invoke();
        return true;
    }
    if ( const auto toggle =
             peer.primaryButton.try_as<MUXC::AppBarToggleButton>() )
    {
        MUXAP::AppBarToggleButtonAutomationPeer(toggle).Toggle();
        return true;
    }
    if ( const auto button =
             peer.primaryButton.try_as<MUXC::Button>() )
    {
        MUXAP::ButtonAutomationPeer(button).Invoke();
        return true;
    }
    if ( const auto toggle =
             peer.primaryButton.try_as<MUXCP::ToggleButton>() )
    {
        MUXAP::ToggleButtonAutomationPeer(toggle).Toggle();
        return true;
    }

    const MUXAP::AutomationPeer automationPeer =
        wxWinUICreateToolAutomationPeer(peer, dropdownPart);
    if ( !automationPeer )
        return false;

    if ( const auto invoke =
             automationPeer
                 .GetPattern(MUXAP::PatternInterface::Invoke)
                 .try_as<MUXAPR::IInvokeProvider>() )
    {
        invoke.Invoke();
        return true;
    }
    if ( const auto toggle =
             automationPeer
                 .GetPattern(MUXAP::PatternInterface::Toggle)
                 .try_as<MUXAPR::IToggleProvider>() )
    {
        toggle.Toggle();
        return true;
    }
    if ( const auto selection =
             automationPeer
                 .GetPattern(MUXAP::PatternInterface::SelectionItem)
                 .try_as<MUXAPR::ISelectionItemProvider>() )
    {
        selection.Select();
        return true;
    }
    return false;
}

#endif // WXWINUI_TEST_SUPPORT

} // anonymous namespace

// ----------------------------------------------------------------------------
// wxWinUIToolBarImpl
// ----------------------------------------------------------------------------

class wxWinUIToolBarImpl
{
public:
    ~wxWinUIToolBarImpl()
    {
        Close();
    }

    void Close()
    {
        const std::shared_ptr<wxWinUIToolBarCallbackState> closingState =
            callbackState;
        wxToolBar * const closingOwner =
            closingState ? closingState->GetLiveOwner() : nullptr;
        const wxWeakRef<wxToolBar> ownerLifetime(closingOwner);

        // Invalidate before revocation: WinRT is allowed to synchronously
        // release or drain a delegate while its token is being removed.
        if ( closingState )
            closingState->Invalidate();

        if ( closed )
            return;
        closed = true;
#ifdef WXWINUI_TEST_SUPPORT
        nextRebuildLoadedHookForTesting = nullptr;
        nextRebuildLoadedContextForTesting = nullptr;
        nextShortHelpSetterHookForTesting = nullptr;
        nextShortHelpSetterContextForTesting = nullptr;
        nextEnableSetterHookForTesting = nullptr;
        nextEnableSetterContextForTesting = nullptr;
        nextOverflowMutationHookForTesting = nullptr;
        nextOverflowMutationContextForTesting = nullptr;
        failNextOverflowMutationBoundaryForTesting = 0;
#endif
        pendingShortHelpKeys.clear();
        pendingEnabledKeys.clear();
        shortHelpFairnessCursor = 0;
        enabledFairnessCursor = 0;
        shortHelpSyncActive = false;
        shortHelpSyncScheduled = false;
        shortHelpRetryCallbackActive = false;
        enabledSyncActive = false;
        enabledSyncScheduled = false;
        enabledRetryCallbackActive = false;

        // Publish a completely inert implementation before crossing the first
        // XAML/wx callback boundary. If one of the cleanup calls destroys the
        // toolbar, its nested destructor sees `closed` and empty state, while
        // this outer call can finish releasing only its local WinRT handles.
        const MUXC::Grid closingRoot = std::exchange(root, nullptr);
        const MUXC::Button closingOverflowButton =
            std::exchange(overflowButton, nullptr);
        MUXC::StackPanel closingOverflowPanel =
            std::exchange(overflowPanel, nullptr);
        MUXC::Flyout closingOverflowFlyout =
            std::exchange(overflowFlyout, nullptr);
        const winrt::event_token closingSizeChangedToken =
            std::exchange(sizeChangedToken, winrt::event_token{});
        std::vector<wxWinUIToolPeer> closingPeers;
        closingPeers.swap(peers);
        peerGeneration = 0;
        callbackState.reset();

        if ( closingRoot && closingSizeChangedToken.value )
        {
            try
            {
                closingRoot.SizeChanged(closingSizeChangedToken);
            }
            catch ( const winrt::hresult_error& e )
            {
                wxWinUILogException(
                    "WinUI toolbar SizeChanged removal", e);
            }
        }

        if ( closingOverflowFlyout )
        {
            try
            {
                closingOverflowFlyout.Hide();
            }
            catch ( const winrt::hresult_error& e )
            {
                wxWinUILogException(
                    "WinUI toolbar overflow flyout close", e);
            }
        }
        wxWinUIForgetManagedToolTip(closingOverflowButton);
        wxWinUIRevokeToolPeers(closingPeers);

#if wxUSE_TOOLTIPS
        // Tool wrappers belong to the toolbar base. They remain valid only
        // while the original owner does: any cleanup callback may synchronously
        // delete it and all of its tools.
        if ( !closingOwner || ownerLifetime.get() == closingOwner )
        {
            for ( wxWinUIToolPeer& peer : closingPeers )
            {
                if ( peer.tool && peer.tool->IsControl() )
                    wxWinUIReleaseControlToolTip(*peer.tool);

                if ( closingOwner && ownerLifetime.get() != closingOwner )
                    break;
            }
        }
#endif

        wxUnusedVar(closingOverflowPanel);

        // The host is still a member, so it may only be touched if the
        // implementation survived every preceding boundary. This is the final
        // operation: slot detachment can itself dispatch focus/lifetime work.
        if ( !closingOwner || ownerLifetime.get() == closingOwner )
            host.Close();
    }

    void RevokeRootSizeChanged()
    {
        if ( root && sizeChangedToken.value )
        {
            try
            {
                root.SizeChanged(sizeChangedToken);
            }
            catch ( const winrt::hresult_error& e )
            {
                wxWinUILogException(
                    "WinUI toolbar SizeChanged removal", e);
            }
        }
        sizeChangedToken = {};
    }

    wxWinUIToolPeer *FindPeerByKey(std::uint64_t key)
    {
        const auto it = std::find_if(
            peers.begin(), peers.end(),
            [key](const wxWinUIToolPeer& peer)
            {
                return peer.tool && peer.tool->peerKey == key;
            });
        return it == peers.end() ? nullptr : &*it;
    }

    const wxWinUIToolPeer *FindPeerByKey(std::uint64_t key) const
    {
        const auto it = std::find_if(
            peers.begin(), peers.end(),
            [key](const wxWinUIToolPeer& peer)
            {
                return peer.tool && peer.tool->peerKey == key;
            });
        return it == peers.end() ? nullptr : &*it;
    }

    wxWinUIToolPeer *FindPeerByTool(const wxToolBarToolBase *tool)
    {
        for ( wxWinUIToolPeer& peer : peers )
        {
            if ( peer.tool == tool )
                return &peer;
        }
        return nullptr;
    }

    const wxWinUIToolPeer *FindPeerByTool(
        const wxToolBarToolBase *tool) const
    {
        for ( const wxWinUIToolPeer& peer : peers )
        {
            if ( peer.tool == tool )
                return &peer;
        }
        return nullptr;
    }

    wxWinUIControlHost host;
    std::shared_ptr<wxWinUIToolBarCallbackState> callbackState;
    MUXC::Grid root{ nullptr };
    MUXC::Button overflowButton{ nullptr };
    MUXC::StackPanel overflowPanel{ nullptr };
    MUXC::Flyout overflowFlyout{ nullptr };
    std::vector<wxWinUIToolPeer> peers;
    winrt::event_token sizeChangedToken{};
    std::uint64_t peerGeneration = 0;
    std::uint64_t modelRevision = 0;
    std::uint64_t bitmapSizeRevision = 0;
    std::vector<wxSize> requestedBitmapSizeRollbackStack;
    std::uint64_t layoutRevision = 0;
    std::set<std::uint64_t> pendingShortHelpKeys;
    std::set<std::uint64_t> pendingEnabledKeys;
    std::uint64_t shortHelpFairnessCursor = 0;
    std::uint64_t enabledFairnessCursor = 0;
    bool shortHelpSyncActive = false;
    bool shortHelpSyncScheduled = false;
    bool shortHelpRetryCallbackActive = false;
    unsigned shortHelpDeferredCallbacksRemaining = 0;
    bool shortHelpDriverWarningEmitted = false;
    bool enabledSyncActive = false;
    bool enabledSyncScheduled = false;
    bool enabledRetryCallbackActive = false;
    unsigned enabledDeferredCallbacksRemaining = 0;
    bool enabledDriverWarningEmitted = false;
    bool realized = false;
    bool closed = false;
#ifdef WXWINUI_TEST_SUPPORT
    bool failNextRebuildForTesting = false;
    wxWinUIToolBarTestAccess::CallbackHook
        nextRebuildLoadedHookForTesting = nullptr;
    void *nextRebuildLoadedContextForTesting = nullptr;
    wxWinUIToolBarTestAccess::CallbackHook
        nextShortHelpSetterHookForTesting = nullptr;
    void *nextShortHelpSetterContextForTesting = nullptr;
    wxWinUIToolBarTestAccess::CallbackHook
        nextEnableSetterHookForTesting = nullptr;
    void *nextEnableSetterContextForTesting = nullptr;
    wxWinUIToolBarTestAccess::CallbackHook
        nextOverflowMutationHookForTesting = nullptr;
    void *nextOverflowMutationContextForTesting = nullptr;
    unsigned failNextOverflowMutationBoundaryForTesting = 0;
#endif
};

namespace
{

class wxWinUIToolBarCandidate final
{
public:
    ~wxWinUIToolBarCandidate()
    {
        if ( published )
            return;

        if ( overflowFlyout )
        {
            try
            {
                overflowFlyout.Hide();
            }
            catch ( const winrt::hresult_error& e )
            {
                wxWinUILogException(
                    "candidate WinUI toolbar overflow flyout close", e);
            }
        }
        if ( root && sizeChangedToken.value )
        {
            try
            {
                root.SizeChanged(sizeChangedToken);
            }
            catch ( const winrt::hresult_error& e )
            {
                wxWinUILogException(
                    "candidate WinUI toolbar SizeChanged removal", e);
            }
        }
        sizeChangedToken = {};
        wxWinUIForgetManagedToolTip(overflowButton);
        wxWinUIRevokeToolPeers(peers);
    }

    MUXC::Grid root{ nullptr };
    MUXC::Button overflowButton{ nullptr };
    MUXC::StackPanel overflowPanel{ nullptr };
    MUXC::Flyout overflowFlyout{ nullptr };
    std::vector<wxWinUIToolPeer> peers;
    winrt::event_token sizeChangedToken{};
    bool published = false;
};

} // anonymous namespace

// ----------------------------------------------------------------------------
// wxToolBar creation and tools
// ----------------------------------------------------------------------------

wxToolBar::wxToolBar()
{
}

wxToolBar::wxToolBar(wxWindow *parent,
                     wxWindowID id,
                     const wxPoint& pos,
                     const wxSize& size,
                     long style,
                     const wxString& name)
{
    Create(parent, id, pos, size, style, name);
}

wxToolBar::~wxToolBar()
{
    Unbind(wxEVT_DPI_CHANGED, &wxToolBar::OnDPIChanged, this);
    if ( m_winui )
        m_winui->Close();
}

bool wxToolBar::Create(wxWindow *parent,
                       wxWindowID id,
                       const wxPoint& pos,
                       const wxSize& size,
                       long style,
                       const wxString& name)
{
    style = (style & ~wxBORDER_MASK) | wxBORDER_NONE;

    if ( !wxToolBarBase::Create(parent, id, pos, size, style,
                                wxDefaultValidator, name) )
    {
        return false;
    }

    FixupStyle();

    m_winui = std::make_unique<wxWinUIToolBarImpl>();
    m_winui->callbackState =
        std::make_shared<wxWinUIToolBarCallbackState>(this);
    wxWinUIToolBarImpl * const createImpl = m_winui.get();
    const std::shared_ptr<wxWinUIToolBarCallbackState> callbackState =
        createImpl->callbackState;
    if ( !createImpl->host.Initialize(this) )
    {
        createImpl->Close();
        return false;
    }

    if ( !RebuildPeer() )
    {
        wxToolBar * const liveOwner = callbackState->GetLiveOwner();
        if ( liveOwner && liveOwner->m_winui &&
             liveOwner->m_winui.get() == createImpl &&
             liveOwner->m_winui->callbackState == callbackState )
        {
            createImpl->Close();
        }
        return false;
    }

    wxToolBar *liveOwner = callbackState->GetLiveOwner();
    if ( !liveOwner || !liveOwner->m_winui ||
         liveOwner->m_winui.get() != createImpl ||
         liveOwner->m_winui->callbackState != callbackState )
    {
        return false;
    }

    const std::uint64_t createGeneration =
        createImpl->peerGeneration;
    liveOwner->SetInitialSize(size);
    liveOwner = callbackState->GetLiveOwner();
    if ( !liveOwner || !liveOwner->m_winui ||
         liveOwner->m_winui.get() != createImpl ||
         liveOwner->m_winui->callbackState != callbackState ||
         createImpl->peerGeneration != createGeneration )
    {
        return false;
    }

    liveOwner->Bind(
        wxEVT_DPI_CHANGED, &wxToolBar::OnDPIChanged, liveOwner);
    return true;
}

wxToolBarToolBase *wxToolBar::CreateTool(
    int toolid,
    const wxString& label,
    const wxBitmapBundle& bmpNormal,
    const wxBitmapBundle& bmpDisabled,
    wxItemKind kind,
    wxObject *clientData,
    const wxString& shortHelp,
    const wxString& longHelp)
{
    return new wxWinUIToolBarTool(
        this, toolid, label, bmpNormal, bmpDisabled,
        kind, clientData, shortHelp, longHelp);
}

wxToolBarToolBase *wxToolBar::CreateTool(wxControl *control,
                                         const wxString& label)
{
    return new wxWinUIToolBarTool(this, control, label);
}

bool wxToolBar::ApplyToolLabel(unsigned long long peerKey)
{
    if ( !m_winui || !m_winui->callbackState )
        return false;

    wxWinUIToolBarImpl * const updateImpl = m_winui.get();
    const std::shared_ptr<wxWinUIToolBarCallbackState> callbackState =
        updateImpl->callbackState;
    if ( !callbackState->TryBeginLabelSync() )
        return false;
    wxScopeGuard labelSyncGuard = wxMakeGuard(
        [callbackState]()
        {
            callbackState->EndLabelSync();
        });
    wxUnusedVar(labelSyncGuard);
    const std::uint64_t generation = updateImpl->peerGeneration;

    // Dependency-property writes can synchronously run callbacks. Restart
    // from the retained model whenever one of them becomes a newer writer.
    for ( unsigned attempt = 0; attempt < 4; ++attempt )
    {
        wxToolBar *owner = callbackState->GetLiveOwner();
        if ( !owner || !owner->m_winui ||
             owner->m_winui.get() != updateImpl ||
             owner->m_winui->callbackState != callbackState ||
             updateImpl->peerGeneration != generation )
        {
            return false;
        }

        wxWinUIToolPeer *peer = updateImpl->FindPeerByKey(peerKey);
        if ( !peer || !peer->tool )
            return false;
        wxWinUIToolBarTool * const tool = peer->tool;
        const std::uint64_t revision = tool->labelRevision;
        const wxString label =
            wxWinUIRemoveMnemonics(tool->GetLabel());
        const winrt::hstring xamlLabel = wxWinUIToHString(label);

        const auto getCurrentPeer =
            [&]() -> wxWinUIToolPeer *
            {
                wxToolBar * const currentOwner =
                    callbackState->GetLiveOwner();
                if ( !currentOwner || !currentOwner->m_winui ||
                     currentOwner->m_winui.get() != updateImpl ||
                     currentOwner->m_winui->callbackState != callbackState ||
                     updateImpl->peerGeneration != generation )
                {
                    return nullptr;
                }
                wxWinUIToolPeer * const current =
                    updateImpl->FindPeerByKey(peerKey);
                return current && current->tool &&
                       current->tool->labelRevision == revision
                    ? current
                    : nullptr;
            };

        try
        {
            if ( const auto button =
                     peer->primaryButton.try_as<MUXC::AppBarButton>() )
            {
                button.Label(xamlLabel);
                peer = getCurrentPeer();
                if ( !peer )
                    continue;
            }
            else if ( const auto toggleButton =
                          peer->primaryButton
                              .try_as<MUXC::AppBarToggleButton>() )
            {
                toggleButton.Label(xamlLabel);
                peer = getCurrentPeer();
                if ( !peer )
                    continue;
            }

            if ( peer->radioLabel )
            {
                peer->radioLabel.Text(xamlLabel);
                peer = getCurrentPeer();
                if ( !peer )
                    continue;

                if ( peer->tool->IsControl() )
                {
                    peer->radioLabel.Visibility(
                        label.empty()
                            ? MUX::Visibility::Collapsed
                            : MUX::Visibility::Visible);
                    peer = getCurrentPeer();
                    if ( !peer )
                        continue;
                }
            }

            if ( peer->primaryButton )
            {
                MUXA::AutomationProperties::SetName(
                    peer->primaryButton, xamlLabel);
                peer = getCurrentPeer();
                if ( !peer )
                    continue;
            }
            if ( peer->dropdownButton )
            {
                MUXA::AutomationProperties::SetName(
                    peer->dropdownButton,
                    wxWinUIToHString(
                        wxString::Format(
                            _("%s menu"), label.c_str())));
                peer = getCurrentPeer();
                if ( !peer )
                    continue;
            }

            peer->naturalExtent = 0.0;
            return true;
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException("WinUI toolbar label update", e);
            return false;
        }
    }

    return false;
}

void wxToolBar::DoSetToolLabel(wxToolBarToolBase *tool,
                               const wxString& label)
{
    wxWinUIToolBarTool * const winuiTool =
        static_cast<wxWinUIToolBarTool *>(tool);
    if ( winuiTool->GetLabel() == label )
        return;

    // SetLabel() on a detached candidate remains a pure retained-model write.
    // RebuildPeer() performs an unconditional label reconciliation before
    // publishing, covering candidates changed from its Loaded seam.
    if ( winuiTool->GetToolBar() != this )
    {
        winuiTool->wxToolBarToolBase::SetLabel(label);
        return;
    }

    winuiTool->wxToolBarToolBase::SetLabel(label);
    if ( ++winuiTool->labelRevision == 0 )
        ++winuiTool->labelRevision;

    if ( m_winui )
    {
        if ( ++m_winui->modelRevision == 0 )
            ++m_winui->modelRevision;
    }

    const std::uint64_t peerKey = winuiTool->peerKey;
    const std::shared_ptr<wxWinUIToolBarCallbackState> callbackState =
        m_winui ? m_winui->callbackState : nullptr;
    if ( !m_winui || !m_winui->realized || !callbackState )
    {
        InvalidateBestSize();
        return;
    }

    // A Loaded/property callback is already inside a candidate transaction.
    // Retain only the model write here: the outer RebuildPeer() unconditionally
    // reconciles every label before commit (and again on rollback), which also
    // prevents recursive setter stacks.
    if ( callbackState->IsRebuildActive() ||
         callbackState->IsLabelSyncActive() )
        return;

    wxWinUIToolBarImpl * const rebuildImpl = m_winui.get();
    const bool rebuilt = RebuildPeer();
    wxToolBar *liveOwner = callbackState->GetLiveOwner();
    if ( !liveOwner || !liveOwner->m_winui ||
         liveOwner->m_winui.get() != rebuildImpl ||
         liveOwner->m_winui->callbackState != callbackState )
    {
        return;
    }

    wxWinUIToolPeer * const livePeer =
        rebuildImpl->FindPeerByKey(peerKey);
    wxWinUIToolBarTool * const liveTool =
        livePeer ? livePeer->tool : nullptr;
    if ( !liveTool || liveTool->GetToolBar() != liveOwner )
        return;

    // The setter itself remains authoritative even if rebuilding unrelated
    // peer structure fails. Rollback restores the old generation and
    // reconciles it from the current label model; an early pre-publication
    // failure gets the same direct convergence here.
    if ( !rebuilt )
    {
        (void)liveOwner->ApplyToolLabel(peerKey);

        // Every dependency-property setter in ApplyToolLabel() may synchronously
        // re-enter wx code and destroy or replace this toolbar. Re-resolve the
        // owner and implementation before touching either one again.
        liveOwner = callbackState->GetLiveOwner();
        if ( !liveOwner || !liveOwner->m_winui ||
             liveOwner->m_winui.get() != rebuildImpl ||
             liveOwner->m_winui->callbackState != callbackState )
        {
            return;
        }
    }
    liveOwner->InvalidateBestSize();
}

bool wxToolBar::DoInsertTool(size_t pos, wxToolBarToolBase *tool)
{
    wxWinUIToolBarTool * const winuiTool =
        static_cast<wxWinUIToolBarTool *>(tool);
    if ( m_tools.Find(tool) )
        return false;

    wxToolBarBase * const initialToolBar = tool->GetToolBar();
    if ( initialToolBar && initialToolBar != this )
        return false;

    if ( !winuiTool->TryClaimInsertion(this) )
        return false;

    const std::weak_ptr<void> toolLifetime =
        winuiTool->GetLifetimeToken();
    const std::uint64_t ownershipEpoch =
        winuiTool->GetOwnershipEpoch();
    wxScopeGuard insertionClaim = wxMakeGuard(
        [winuiTool, toolLifetime, this]()
        {
            if ( !toolLifetime.expired() )
                winuiTool->ReleaseInsertionClaim(this);
        });
    wxUnusedVar(insertionClaim);

    if ( winuiTool->IsControl() && !winuiTool->GetLiveControl() )
    {
        winuiTool->ForgetControlIfDestroyed();
        return false;
    }
    if ( winuiTool->IsControl() &&
         winuiTool->GetLiveControl()->GetParent() != this )
    {
        return false;
    }

    if ( !m_winui || !m_winui->realized )
        return true;
    if ( m_winui->closed || !m_winui->callbackState )
        return false;

    wxWinUIToolBarImpl * const rebuildImpl = m_winui.get();
    const std::shared_ptr<wxWinUIToolBarCallbackState> callbackState =
        rebuildImpl->callbackState;
    const bool rebuilt = RebuildPeer(nullptr, pos, tool);

    // The candidate is not in m_tools yet and remains owned by the common
    // insertion helper even if a synchronous callback destroyed this toolbar.
    // Neutralize a dead embedded control before that helper deletes the tool.
    if ( toolLifetime.expired() )
        return false;
    if ( winuiTool->GetOwnershipEpoch() != ownershipEpoch ||
         winuiTool->GetToolBar() != initialToolBar ||
         !winuiTool->IsInsertionClaimedBy(this) )
    {
        return false;
    }
    winuiTool->ForgetControlIfDestroyed();
    if ( !rebuilt )
        return false;

    wxToolBar * const liveOwner = callbackState->GetLiveOwner();
    if ( !liveOwner || !liveOwner->m_winui ||
         liveOwner->m_winui.get() != rebuildImpl ||
         liveOwner->m_winui->callbackState != callbackState )
    {
        return false;
    }

    if ( tool->IsControl() )
    {
        if ( winuiTool->controlHiddenByToolbar )
        {
            winuiTool->controlHiddenByToolbar = false;
            winuiTool->controlShowPending = true;
        }
    }
    return true;
}

bool wxToolBar::DoDeleteTool(size_t WXUNUSED(pos),
                             wxToolBarToolBase *tool)
{
    wxWinUIToolBarTool * const winuiTool =
        static_cast<wxWinUIToolBarTool *>(tool);
    if ( !m_winui || !m_winui->realized )
    {
        winuiTool->ForgetControlIfDestroyed();
        return true;
    }

    // The common base still owns `tool` here. Building an explicit snapshot
    // without it is what prevents Delete/Remove/Clear from publishing a ghost.
    wxWinUIToolBarImpl * const rebuildImpl = m_winui.get();
    const std::shared_ptr<wxWinUIToolBarCallbackState> callbackState =
        rebuildImpl->callbackState;
    const bool rebuilt = RebuildPeer(tool);
    wxToolBar * const liveOwner = callbackState->GetLiveOwner();
    if ( !liveOwner || !liveOwner->m_winui ||
         liveOwner->m_winui.get() != rebuildImpl ||
         liveOwner->m_winui->callbackState != callbackState )
    {
        return false;
    }

    // The excluded control is intentionally not part of the candidate peer,
    // but the deterministic Loaded boundary may still destroy it.
    winuiTool->ForgetControlIfDestroyed();
    return rebuilt;
}

wxToolBarToolBase *wxToolBar::RemoveTool(int toolid)
{
    size_t pos = 0;
    wxToolBarToolsList::compatibility_iterator node;
    for ( node = m_tools.GetFirst(); node; node = node->GetNext(), ++pos )
    {
        if ( node->GetData()->GetId() == toolid )
            break;
    }

    if ( !node )
        return nullptr;

    wxToolBarToolBase * const tool = node->GetData();
    if ( !tool || !DoDeleteTool(pos, tool) )
        return nullptr;
    wxWinUIToolBarTool * const winuiTool =
        static_cast<wxWinUIToolBarTool *>(tool);

    // Commit the common model before crossing tooltip/Show boundaries. The
    // stock base implementation performs these operations in the opposite
    // order through DoDeleteTool(), which lets a synchronous wxEVT_SHOW
    // handler erase the same list node twice.
    m_tools.Erase(node);
    tool->Detach();
    const std::uint64_t ownershipEpoch =
        winuiTool->GetOwnershipEpoch();
    if ( !tool->IsControl() )
    {
        return winuiTool->GetOwnershipEpoch() == ownershipEpoch &&
               !tool->GetToolBar()
            ? tool
            : nullptr;
    }

    const std::weak_ptr<void> toolLifetime =
        winuiTool->GetLifetimeToken();
    wxControl * const control = winuiTool->GetLiveControl();
    wxWeakRef<wxWindow> controlLifetime(control);
    winuiTool->controlHiddenByToolbar = true;
    winuiTool->controlShowPending = false;

#if wxUSE_TOOLTIPS
    wxWinUIReleaseControlToolTip(*winuiTool);
    if ( toolLifetime.expired() ||
         winuiTool->GetOwnershipEpoch() != ownershipEpoch ||
         tool->GetToolBar() )
        return nullptr;
#endif

    // The target is no longer discoverable through the toolbar model, so a
    // synchronous hide handler can safely call RemoveTool/DeleteTool again.
    // Hide is the final operation before returning the detached tool.
    if ( wxWindow * const liveControl = controlLifetime.get() )
        liveControl->Hide();
    return !toolLifetime.expired() &&
           winuiTool->GetOwnershipEpoch() == ownershipEpoch &&
           !tool->GetToolBar()
        ? tool
        : nullptr;
}

bool wxToolBar::DeleteToolByPos(size_t pos)
{
    wxCHECK_MSG(pos < GetToolsCount(), false,
                "invalid position in wxToolBar::DeleteToolByPos()");

    wxToolBarToolsList::compatibility_iterator node = m_tools.Item(pos);
    wxToolBarToolBase * const tool = node ? node->GetData() : nullptr;
    if ( !tool || !DoDeleteTool(pos, tool) )
        return false;

    // Erase by exact list identity, not by ID: duplicate IDs are legal and
    // DeleteToolByPos() must remove the selected tool from both projections.
    m_tools.Erase(node);

    // Deleting a control tool destroys its child control. No explicit Hide()
    // is needed and, crucially, no toolbar state is touched after this call:
    // the child destruction path may synchronously destroy the toolbar.
    delete tool;
    return true;
}

bool wxToolBar::DeleteTool(int toolid)
{
    size_t pos = 0;
    for ( wxToolBarToolsList::compatibility_iterator node = m_tools.GetFirst();
          node;
          node = node->GetNext(), ++pos )
    {
        if ( node->GetData()->GetId() == toolid )
            return DeleteToolByPos(pos);
    }
    return false;
}

void wxToolBar::ClearTools()
{
    if ( !GetToolsCount() )
        return;

    wxWeakRef<wxWindow> lifetime(this);
    if ( m_winui && m_winui->realized &&
         !RebuildPeer(nullptr, static_cast<size_t>(-1), nullptr, 0.0, true) )
    {
        return;
    }

    wxToolBar* const liveOwner =
        static_cast<wxToolBar*>(lifetime.get());
    if ( liveOwner != this )
        return;

    // Publish the complete common model empty before crossing the first
    // control destructor. This matches the single empty XAML candidate above:
    // failure leaves both projections unchanged, success exposes neither a
    // partially-cleared list nor a sequence of intermediate peer generations.
    struct DetachedTool
    {
        wxWinUIToolBarTool *tool;
        std::weak_ptr<void> lifetime;
        std::uint64_t ownershipEpoch = 0;
    };
    std::vector<DetachedTool> tools;
    tools.reserve(m_tools.GetCount());
    for ( wxToolBarToolsList::compatibility_iterator node = m_tools.GetFirst();
          node; )
    {
        wxToolBarToolsList::compatibility_iterator next = node->GetNext();
        wxWinUIToolBarTool* const tool =
            static_cast<wxWinUIToolBarTool*>(node->GetData());
        tools.push_back({tool, tool->GetLifetimeToken(), 0});
        m_tools.Erase(node);
        node = next;
    }

    for ( DetachedTool& detached : tools )
    {
        wxWinUIToolBarTool * const tool = detached.tool;
        tool->Detach();
        detached.ownershipEpoch = tool->GetOwnershipEpoch();
        tool->ForgetControlIfDestroyed();
    }

    // Nothing below dereferences the toolbar. Destroying one embedded control
    // is allowed to destroy its parent and all remaining child controls.
    for ( const DetachedTool& detached : tools )
    {
        if ( detached.lifetime.expired() ||
             detached.tool->GetOwnershipEpoch() !=
                 detached.ownershipEpoch ||
             detached.tool->GetToolBar() )
        {
            // A synchronous destructor callback either deleted this wrapper
            // already or transferred it back to a toolbar. In both cases the
            // nested writer owns the final lifetime.
            continue;
        }

        wxWinUIToolBarTool * const tool = detached.tool;
        tool->ForgetControlIfDestroyed();
        delete tool;
    }
}

void wxToolBar::DoEnableTool(wxToolBarToolBase *tool,
                             bool WXUNUSED(enable))
{
    if ( !m_winui || !m_winui->callbackState )
        return;

    wxWinUIToolBarImpl * const updateImpl = m_winui.get();
    wxWinUIToolBarTool * const winuiTool =
        static_cast<wxWinUIToolBarTool *>(tool);
    ++updateImpl->modelRevision;
    if ( updateImpl->modelRevision == 0 )
        ++updateImpl->modelRevision;
    if ( ++winuiTool->enabledRevision == 0 )
        ++winuiTool->enabledRevision;
    winuiTool->enabledProjectionFailures = 0;
    winuiTool->enabledProjectionQuarantined = false;

    if ( !updateImpl->enabledSyncActive &&
         !updateImpl->enabledRetryCallbackActive )
    {
        updateImpl->enabledDeferredCallbacksRemaining = 2;
        updateImpl->enabledDriverWarningEmitted = false;
    }
    updateImpl->pendingEnabledKeys.insert(winuiTool->peerKey);
    ConvergeToolEnabledUpdates();
}

void wxToolBar::DoToggleTool(wxToolBarToolBase *tool, bool toggle)
{
    if ( !m_winui )
        return;

    wxWinUIToolPeer * const peer = m_winui->FindPeerByTool(tool);
    if ( !peer || !peer->primaryButton )
        return;

    try
    {
        if ( const auto button =
                 peer->primaryButton.try_as<MUXC::AppBarToggleButton>() )
        {
            button.IsChecked(toggle);
        }
        else if ( const auto toggleButton =
                      peer->primaryButton.try_as<MUXCP::ToggleButton>() )
        {
            toggleButton.IsChecked(toggle);
        }
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI toolbar toggle update", e);
    }
}

void wxToolBar::DoSetToggle(wxToolBarToolBase *tool,
                            bool WXUNUSED(toggle))
{
    wxWinUIToolBarTool * const requestedTool =
        static_cast<wxWinUIToolBarTool *>(tool);
    const std::uint64_t peerKey = requestedTool->peerKey;
    if ( !m_winui || !m_winui->realized ||
         !m_winui->callbackState )
    {
        requestedTool->CommitSetToggle();
        return;
    }

    wxWinUIToolBarImpl * const rebuildImpl = m_winui.get();
    const std::shared_ptr<wxWinUIToolBarCallbackState> callbackState =
        rebuildImpl->callbackState;
    const bool rebuilt = RebuildPeer();

    // RebuildPeer() crosses XAML Loaded/layout callbacks. Resolve the tool
    // again from the published peer: the original pointer may have been
    // removed (and deleted) by application code while the setter ran.
    wxToolBar * const liveOwner =
        callbackState->GetLiveOwner();
    if ( !liveOwner || !liveOwner->m_winui ||
         liveOwner->m_winui.get() != rebuildImpl ||
         liveOwner->m_winui->callbackState != callbackState )
    {
        return;
    }

    wxWinUIToolPeer * const livePeer =
        liveOwner->m_winui->FindPeerByKey(peerKey);
    if ( !livePeer || !livePeer->tool )
        return;

    if ( rebuilt )
        livePeer->tool->CommitSetToggle();
    else
        livePeer->tool->RollbackSetToggle();
}

void wxToolBar::SetToolBitmapSize(const wxSize& size)
{
    if ( m_winui )
    {
        // wxToolBarBase updates its retained DIP preference before calling
        // our virtual DoSetToolBitmapSize(). Preserve the exact previous
        // preference so a failed peer transaction can roll it back without
        // initiating another rebuild. A stack is required because XAML
        // Loaded/setter callbacks may make a nested application request.
        m_winui->requestedBitmapSizeRollbackStack.push_back(
            GetRequestedToolBitmapSizeInDIPs());
    }

    wxToolBarBase::SetToolBitmapSize(size);
}

void wxToolBar::DoSetToolBitmapSize(const wxSize& size)
{
    bool hasRequestedSizeRollback = false;
    wxSize previousRequestedSizeInDIPs;
    if ( m_winui &&
         !m_winui->requestedBitmapSizeRollbackStack.empty() )
    {
        hasRequestedSizeRollback = true;
        previousRequestedSizeInDIPs =
            m_winui->requestedBitmapSizeRollbackStack.back();
        m_winui->requestedBitmapSizeRollbackStack.pop_back();
    }

    const wxSize previousSize = GetToolBitmapSize();
    wxToolBarBase::DoSetToolBitmapSize(size);

    if ( !m_winui || !m_winui->realized ||
         !m_winui->callbackState ||
         previousSize == size )
    {
        return;
    }

    wxWinUIToolBarImpl * const rebuildImpl = m_winui.get();
    const std::shared_ptr<wxWinUIToolBarCallbackState> callbackState =
        rebuildImpl->callbackState;
    const std::uint64_t previousRevision =
        rebuildImpl->bitmapSizeRevision;
    std::uint64_t revision = previousRevision + 1;
    if ( revision == 0 )
        revision = 1;
    rebuildImpl->bitmapSizeRevision = revision;

    if ( RebuildPeer() )
        return;

    // Preserve a reentrant application update made from a XAML setter. Only
    // roll back the common-model size if this exact request is still current.
    wxToolBar * const liveOwner = callbackState->GetLiveOwner();
    if ( liveOwner && liveOwner->m_winui &&
         liveOwner->m_winui.get() == rebuildImpl &&
         liveOwner->m_winui->callbackState == callbackState &&
         rebuildImpl->bitmapSizeRevision == revision )
    {
        if ( hasRequestedSizeRollback )
        {
            SetRequestedToolBitmapSizeInDIPs(
                previousRequestedSizeInDIPs);
        }
        wxToolBarBase::DoSetToolBitmapSize(previousSize);
        // This transaction produced no winning writer. Restore its parent
        // revision so a rejected nested request doesn't make the still-live
        // outer transaction look stale. Conversely, a nested request that
        // actually published leaves a different revision and bypasses this
        // rollback block entirely.
        rebuildImpl->bitmapSizeRevision = previousRevision;
    }
}

void wxToolBar::DoEnable(bool enable)
{
    wxWinUIToolBarImpl * const updateImpl = m_winui.get();
    const std::shared_ptr<wxWinUIToolBarCallbackState> callbackState =
        updateImpl ? updateImpl->callbackState : nullptr;

    wxControl::DoEnable(enable);
    wxToolBar * const postBaseOwner =
        callbackState ? callbackState->GetLiveOwner() : nullptr;
    if ( !postBaseOwner || !postBaseOwner->m_winui ||
         postBaseOwner->m_winui.get() != updateImpl ||
         postBaseOwner->m_winui->callbackState != callbackState ||
         updateImpl->closed )
    {
        return;
    }

    // A nested enable request from the native focus/WM_ENABLE boundary is the
    // authoritative writer. Project the current wx state, not this outer
    // call's possibly stale argument.
    enable = postBaseOwner->IsEnabled();
    if ( ++updateImpl->modelRevision == 0 )
        ++updateImpl->modelRevision;
    for ( const wxWinUIToolPeer& peer : updateImpl->peers )
    {
        // Native AddControl children retain their own logical enabled state;
        // disabling their parent HWND already makes them effectively inert.
        // Rewriting the child flag here would destroy an application-owned
        // disabled state when the toolbar is enabled again.
        if ( peer.tool && !peer.tool->IsControl() )
            updateImpl->pendingEnabledKeys.insert(peer.tool->peerKey);
    }
    ConvergeToolEnabledUpdates();

    wxToolBar * const owner = callbackState->GetLiveOwner();
    if ( !owner || !owner->m_winui ||
         owner->m_winui.get() != updateImpl ||
         owner->m_winui->callbackState != callbackState )
    {
        return;
    }
    const std::uint64_t generation =
        owner->m_winui->peerGeneration;

    try
    {
        if ( updateImpl->overflowButton )
            updateImpl->overflowButton.IsEnabled(enable);

        wxToolBar *liveOwner =
            callbackState->GetOwnerForModelMutation(
                generation);
        if ( !liveOwner || !liveOwner->m_winui ||
             liveOwner->m_winui.get() != updateImpl ||
             liveOwner->m_winui->callbackState != callbackState )
        {
            return;
        }
        if ( !enable && liveOwner->m_winui->overflowFlyout )
            liveOwner->m_winui->overflowFlyout.Hide();

        liveOwner = callbackState->GetOwnerForModelMutation(
            generation);
        if ( !liveOwner || !liveOwner->m_winui ||
             liveOwner->m_winui.get() != updateImpl ||
             liveOwner->m_winui->callbackState != callbackState )
        {
            return;
        }
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI toolbar enabled update", e);
    }
}

bool wxToolBar::Realize()
{
    if ( !wxToolBarBase::Realize() )
        return false;

    if ( !m_winui || !m_winui->callbackState )
        return false;
    wxWinUIToolBarImpl * const rebuildImpl = m_winui.get();
    const std::shared_ptr<wxWinUIToolBarCallbackState> callbackState =
        rebuildImpl->callbackState;
    if ( !RebuildPeer() )
        return false;

    wxToolBar *liveOwner = callbackState->GetLiveOwner();
    if ( !liveOwner || !liveOwner->m_winui ||
         liveOwner->m_winui.get() != rebuildImpl ||
         liveOwner->m_winui->callbackState != callbackState )
    {
        return false;
    }

    rebuildImpl->realized = true;
    liveOwner->InvalidateBestSize();
    liveOwner = callbackState->GetLiveOwner();
    if ( !liveOwner || !liveOwner->m_winui ||
         liveOwner->m_winui.get() != rebuildImpl ||
         liveOwner->m_winui->callbackState != callbackState )
    {
        return false;
    }

    // Realize() is the public point at which hit testing and embedded-control
    // bounds become usable. Publish the slot geometry and synchronously
    // measure the freshly replaced XAML tree before returning.
    const std::uint64_t realizedGeneration =
        rebuildImpl->peerGeneration;
    rebuildImpl->host.ForceRender();
    liveOwner = callbackState->GetLiveOwner();
    return liveOwner && liveOwner->m_winui &&
           liveOwner->m_winui.get() == rebuildImpl &&
           liveOwner->m_winui->callbackState == callbackState &&
           rebuildImpl->realized &&
           rebuildImpl->peerGeneration == realizedGeneration;
}

// ----------------------------------------------------------------------------
// Transactional peer construction
// ----------------------------------------------------------------------------

bool wxToolBar::RebuildPeer(wxToolBarToolBase *excluded,
                             size_t insertPos,
                             wxToolBarToolBase *inserted,
                             double requestedScale,
                             bool excludeAll)
{
    if ( !m_winui || m_winui->closed || !m_winui->callbackState )
        return false;

    wxWinUIToolBarImpl * const rebuildImpl = m_winui.get();
    const std::shared_ptr<wxWinUIToolBarCallbackState> callbackState =
        rebuildImpl->callbackState;
    const std::uint64_t generation =
        callbackState->PrepareGeneration();
    const wxWinUIToolBarRebuildGuard rebuildGuard(
        callbackState, generation);
    if ( !rebuildGuard )
    {
        // A Loaded or layout callback may ask for another rebuild while the
        // candidate of the outer transaction is being attached. Reject the
        // nested transaction: the outer candidate still describes the
        // unchanged common model and remains the sole publisher.
        return false;
    }

    wxWinUIToolBarTool * const insertedTool =
        inserted
            ? static_cast<wxWinUIToolBarTool *>(inserted)
            : nullptr;
    const std::weak_ptr<void> insertedLifetime =
        insertedTool ? insertedTool->GetLifetimeToken()
                     : std::weak_ptr<void>();
    const std::uint64_t insertedOwnershipEpoch =
        insertedTool ? insertedTool->GetOwnershipEpoch() : 0;
    wxToolBarBase * const insertedInitialToolBar =
        insertedTool ? insertedTool->GetToolBar() : nullptr;
    if ( insertedTool && !insertedTool->IsInsertionClaimedBy(this) )
        return false;

#ifdef WXWINUI_TEST_SUPPORT
    // The deterministic failure seam is consumed now but injected only
    // after provisional publication, so it exercises the same exact rollback
    // transaction as a host SetContent() failure.
    const bool failAfterPublishForTesting =
        rebuildImpl->failNextRebuildForTesting;
    rebuildImpl->failNextRebuildForTesting = false;
#endif
    const std::uint64_t candidateModelRevision =
        rebuildImpl->modelRevision;
    wxScopeGuard reconcileLabelsOnEarlyExit = wxMakeGuard(
        [callbackState, rebuildImpl, candidateModelRevision]()
        {
            wxToolBar * const owner =
                callbackState->GetLiveOwner();
            if ( !owner || !owner->m_winui ||
                 owner->m_winui.get() != rebuildImpl ||
                 owner->m_winui->callbackState != callbackState ||
                 rebuildImpl->modelRevision == candidateModelRevision )
            {
                return;
            }

            std::vector<std::uint64_t> keys;
            keys.reserve(rebuildImpl->peers.size());
            for ( const wxWinUIToolPeer& peer : rebuildImpl->peers )
            {
                if ( peer.tool )
                    keys.push_back(peer.tool->peerKey);
            }
            for ( const std::uint64_t peerKey : keys )
            {
                wxToolBar * const current =
                    callbackState->GetLiveOwner();
                if ( !current || !current->m_winui ||
                     current->m_winui.get() != rebuildImpl ||
                     current->m_winui->callbackState != callbackState )
                {
                    return;
                }
                (void)current->ApplyToolLabel(peerKey);
            }
        });
    wxUnusedVar(reconcileLabelsOnEarlyExit);

    std::vector<wxWinUIToolBarTool *> tools;
    tools.reserve(m_tools.GetCount() + (inserted ? 1u : 0u));
    for ( wxToolBarToolsList::compatibility_iterator node = m_tools.GetFirst();
          node;
          node = node->GetNext() )
    {
        wxToolBarToolBase * const tool = node->GetData();
        if ( !excludeAll && tool != excluded )
            tools.push_back(static_cast<wxWinUIToolBarTool *>(tool));
    }
    if ( inserted && !excludeAll )
    {
        const size_t position = std::min(insertPos, tools.size());
        tools.insert(tools.begin() + position,
                     static_cast<wxWinUIToolBarTool *>(inserted));
    }

    // A control may die at any synchronous wx/XAML boundary below. Existing
    // tools remain owned by a live toolbar model; an inserted candidate remains
    // owned by DoInsertNewTool(). Neutralize only those provably-live tool
    // objects, so their eventual destructors never dereference a dead control.
    wxScopeGuard neutralizeDestroyedControls = wxMakeGuard(
        [callbackState, generation, rebuildImpl, insertedTool,
         insertedLifetime, insertedOwnershipEpoch]()
        {
            wxToolBar* const owner =
                callbackState->GetOwnerDuringRebuild(generation);
            if ( owner && owner->m_winui &&
                 owner->m_winui.get() == rebuildImpl &&
                 owner->m_winui->callbackState == callbackState )
            {
                for ( wxToolBarToolsList::compatibility_iterator node =
                          owner->m_tools.GetFirst();
                      node;
                      node = node->GetNext() )
                {
                    static_cast<wxWinUIToolBarTool*>(node->GetData())->
                        ForgetControlIfDestroyed();
                }
            }

            if ( insertedTool && !insertedLifetime.expired() &&
                 insertedTool->GetOwnershipEpoch() ==
                     insertedOwnershipEpoch &&
                 insertedTool->IsInsertionClaimedBy(owner) )
            {
                insertedTool->ForgetControlIfDestroyed();
            }
        });
    wxUnusedVar(neutralizeDestroyedControls);

    const auto getRebuildOwner = [&]() -> wxToolBar*
    {
        wxToolBar* const owner =
            callbackState->GetOwnerDuringRebuild(generation);
        return owner && owner->m_winui &&
               owner->m_winui.get() == rebuildImpl &&
               owner->m_winui->callbackState == callbackState
            ? owner
            : nullptr;
    };
    const auto allControlsCurrent = [&]() -> bool
    {
        wxToolBar * const owner = getRebuildOwner();
        if ( !owner )
            return false;

        if ( insertedTool )
        {
            if ( insertedLifetime.expired() ||
                 insertedTool->GetOwnershipEpoch() !=
                     insertedOwnershipEpoch ||
                 insertedTool->GetToolBar() !=
                     insertedInitialToolBar ||
                 !insertedTool->IsInsertionClaimedBy(owner) )
            {
                return false;
            }
        }

        for ( wxWinUIToolBarTool* const tool : tools )
        {
            if ( tool->IsControl() && !tool->GetLiveControl() )
                return false;
        }
        return true;
    };
    if ( !allControlsCurrent() )
        return false;

    wxWinUIToolBarCandidate candidate;
    MUXC::Grid previousRoot{ nullptr };
    MUXC::Button previousOverflowButton{ nullptr };
    MUXC::StackPanel previousOverflowPanel{ nullptr };
    MUXC::Flyout previousOverflowFlyout{ nullptr };
    std::vector<wxWinUIToolPeer> previousPeers;
    winrt::event_token previousSizeChangedToken{};
    std::uint64_t previousPeerGeneration = 0;
    std::uint64_t previousCallbackGeneration = 0;
    bool provisionalPublished = false;
    bool candidateAttached = false;

    const auto retirePreviousGeneration =
        [&]()
        {
            if ( previousRoot && previousSizeChangedToken.value )
            {
                try
                {
                    previousRoot.SizeChanged(previousSizeChangedToken);
                }
                catch ( const winrt::hresult_error& e )
                {
                    wxWinUILogException(
                        "retired WinUI toolbar SizeChanged removal", e);
                }
            }
            previousSizeChangedToken = {};
            if ( previousOverflowFlyout )
            {
                try
                {
                    previousOverflowFlyout.Hide();
                }
                catch ( const winrt::hresult_error& e )
                {
                    wxWinUILogException(
                        "retired WinUI toolbar overflow flyout close", e);
                }
            }
            wxWinUIForgetManagedToolTip(previousOverflowButton);
            previousOverflowButton = nullptr;
            previousOverflowPanel = nullptr;
            previousOverflowFlyout = nullptr;
            previousRoot = nullptr;
            wxWinUIRevokeToolPeers(previousPeers);
            previousPeers.clear();
        };

    const auto rollbackProvisionalCandidate =
        [&]()
        {
            if ( !provisionalPublished )
                return;

            wxToolBar * const owner =
                callbackState->GetOwnerDuringRebuild(generation);
            if ( !owner || !owner->m_winui ||
                 owner->m_winui.get() != rebuildImpl ||
                 owner->m_winui->callbackState != callbackState ||
                 owner->m_winui->peerGeneration != generation )
            {
                // Destruction owns cleanup once callback-state invalidation
                // makes the implementation unreachable.
                 return;
             }

#ifdef WXWINUI_TEST_SUPPORT
            // The candidate may be rejected before the normal one-shot cleanup
            // below. Never let its test callback leak into the restored root.
            rebuildImpl->host.SetNextContentLoadedHookForTesting({});
#endif

            // Keep the rejected generation entirely local. Its destructor
            // revokes only local XAML handles and remains safe even if
            // restoring the previous tree synchronously destroys the owner.
            wxWinUIToolBarCandidate rejected;
            rejected.root = rebuildImpl->root;
            rejected.overflowButton = rebuildImpl->overflowButton;
            rejected.overflowPanel = rebuildImpl->overflowPanel;
            rejected.overflowFlyout = rebuildImpl->overflowFlyout;
            rejected.sizeChangedToken = rebuildImpl->sizeChangedToken;
            rejected.peers.swap(rebuildImpl->peers);

            rebuildImpl->root = previousRoot;
            rebuildImpl->overflowButton = previousOverflowButton;
            rebuildImpl->overflowPanel = previousOverflowPanel;
            rebuildImpl->overflowFlyout = previousOverflowFlyout;
            rebuildImpl->sizeChangedToken = previousSizeChangedToken;
            rebuildImpl->peers.swap(previousPeers);
            rebuildImpl->peerGeneration = previousPeerGeneration;
            callbackState->CommitGeneration(previousCallbackGeneration);
            provisionalPublished = false;
            const MUXC::Grid rootToRestore = rebuildImpl->root;
            const bool restoreHostContent = candidateAttached;
            candidateAttached = false;
            previousSizeChangedToken = {};
            previousOverflowButton = nullptr;
            previousOverflowPanel = nullptr;
            previousOverflowFlyout = nullptr;
            previousRoot = nullptr;

            // A setter invoked from the rejected candidate's Loaded callback
            // may have changed the retained label. Reconcile the restored peer
            // before it is reattached, so rollback never resurrects stale
            // visual text or an old UIA name.
            std::vector<std::uint64_t> restoredKeys;
            restoredKeys.reserve(rebuildImpl->peers.size());
            for ( const wxWinUIToolPeer& peer : rebuildImpl->peers )
            {
                if ( peer.tool )
                    restoredKeys.push_back(peer.tool->peerKey);
            }
            bool labelReconcileFailed = false;
            for ( const std::uint64_t peerKey : restoredKeys )
            {
                wxToolBar * const labelOwner =
                    callbackState->GetOwnerDuringRebuild(generation);
                if ( !labelOwner || !labelOwner->m_winui ||
                     labelOwner->m_winui.get() != rebuildImpl ||
                     labelOwner->m_winui->callbackState != callbackState )
                {
                    return;
                }
                if ( !labelOwner->ApplyToolLabel(peerKey) )
                    labelReconcileFailed = true;

                // ApplyToolLabel() crosses several dependency-property
                // setters. The final iteration needs the same lifetime check
                // as every earlier one before the host member is touched.
                wxToolBar * const postLabelOwner =
                    callbackState->GetOwnerDuringRebuild(generation);
                if ( !postLabelOwner || !postLabelOwner->m_winui ||
                     postLabelOwner->m_winui.get() != rebuildImpl ||
                     postLabelOwner->m_winui->callbackState != callbackState )
                {
                    return;
                }
            }

            if ( labelReconcileFailed && wxTheApp )
            {
                // Keep rollback recoverable even if an individual XAML label
                // setter failed. The callback runs after this rebuild guard
                // ends and reconstructs the complete peer transactionally.
                wxTheApp->CallAfter(
                    [callbackState]()
                    {
                        wxToolBar * const owner =
                            callbackState->GetLiveOwner();
                        if ( owner && owner->m_winui &&
                             owner->m_winui->callbackState ==
                                 callbackState )
                        {
                            (void)owner->RebuildPeer();
                        }
                    });
            }

            // SetContent() may already have attached the rejected candidate
            // before a Loaded callback invalidated one of its control tools.
            // Restore the exact previous root after the implementation model
            // and generation are coherent again. No implementation member is
            // touched after a successful restore: it can synchronously run
            // Loaded/focus application code and destroy the toolbar.
            wxToolBar * const restoreOwner =
                callbackState->GetOwnerDuringRebuild(generation);
            if ( !restoreOwner || !restoreOwner->m_winui ||
                 restoreOwner->m_winui.get() != rebuildImpl ||
                 restoreOwner->m_winui->callbackState != callbackState )
            {
                return;
            }
            if ( restoreHostContent &&
                 !restoreOwner->m_winui->host.SetContent(rootToRestore) )
            {
                // The shared slot reports failure truthfully, including the
                // rare install+restore fault where its carrier is now empty.
                // Never leave that actual carrier showing a rejected
                // generation while the toolbar model claims the old one.
                // Quarantine the peer by closing its host; the common tool
                // model remains authoritative and no ghost can receive input.
                wxToolBar * const liveOwner =
                    callbackState->GetLiveOwner();
                if ( liveOwner && liveOwner->m_winui &&
                     liveOwner->m_winui.get() == rebuildImpl &&
                     liveOwner->m_winui->callbackState == callbackState &&
                     rebuildImpl->peerGeneration ==
                         previousPeerGeneration )
                {
                    wxLogWarning(
                        "Could not restore the previous WinUI toolbar peer; "
                        "the peer has been closed to avoid a ghost tree");
                    rebuildImpl->Close();
                }
            }
        };

    try
    {
        candidate.root = wxWinUICreateAccessibleGrid(
            MUXAP::AutomationControlType::ToolBar, "wxToolBar");
        MUXA::AutomationProperties::SetLocalizedControlType(
            candidate.root, wxWinUIToHString(_("tool bar")));
        candidate.root.HorizontalAlignment(
            MUX::HorizontalAlignment::Stretch);
        candidate.root.VerticalAlignment(
            MUX::VerticalAlignment::Stretch);
        candidate.root.FlowDirection(
            wxWinUIToolBarFlowDirection(GetLayoutDirection()));

        const bool vertical = IsVertical();
        const bool rtl =
            GetLayoutDirection() == wxLayout_RightToLeft;
        const bool showText = HasFlag(wxTB_TEXT);
        const bool horizontalText =
            showText && HasFlag(wxTB_HORZ_LAYOUT);
        const bool showIcons = !HasFlag(wxTB_NOICONS);
        const wxSize margin = ToDIP(GetToolMargins());
        candidate.root.Margin(
            { static_cast<float>(std::max(margin.x, 0)),
              static_cast<float>(std::max(margin.y, 0)),
              static_cast<float>(std::max(margin.x, 0)),
              static_cast<float>(std::max(margin.y, 0)) });
        const int packing =
            std::max(ToDIP(wxSize(GetToolPacking(), 0)).x, 0);
        const int separatorExtent =
            GetToolSeparation() > 0
                ? std::max(
                      ToDIP(wxSize(GetToolSeparation(), 0)).x,
                      1)
                : wxWINUI_TOOL_SEPARATOR_WIDTH;
        const auto labelPosition =
            showText
                ? MUXC::CommandBarLabelPosition::Default
                : MUXC::CommandBarLabelPosition::Collapsed;
        const int buttonHeight =
            showText && !horizontalText
                ? wxWINUI_TOOL_HEIGHT_LABEL_BELOW
                : wxWINUI_TOOL_HEIGHT;

        candidate.peers.reserve(tools.size());
        for ( size_t index = 0; index < tools.size(); ++index )
        {
            wxWinUIToolBarTool * const tool = tools[index];
            wxWinUIToolPeer peer;
            peer.tool = tool;
            // Embedded controls are native wx/HWND identities and stretch
            // spacers define primary-bar layout. Neither may be projected
            // into a popup. Every other element is physically reparented
            // (never cloned) when it enters the overflow flyout.
            peer.overflowEligible =
                !tool->IsControl() && !tool->IsStretchable();

            if ( vertical )
            {
                MUXC::RowDefinition row;
                row.Height(
                    tool->IsStretchable()
                        ? MUX::GridLengthHelper::FromValueAndType(
                              1.0, MUX::GridUnitType::Star)
                        : MUX::GridLengthHelper::Auto());
                candidate.root.RowDefinitions().Append(row);
            }
            else
            {
                MUXC::ColumnDefinition column;
                column.Width(
                    tool->IsStretchable()
                        ? MUX::GridLengthHelper::FromValueAndType(
                              1.0, MUX::GridUnitType::Star)
                        : MUX::GridLengthHelper::Auto());
                candidate.root.ColumnDefinitions().Append(column);
            }

            if ( tool->IsStretchable() )
            {
                MUXC::Border spacer;
                spacer.IsHitTestVisible(false);
                spacer.HorizontalAlignment(
                    MUX::HorizontalAlignment::Stretch);
                spacer.VerticalAlignment(
                    MUX::VerticalAlignment::Stretch);
                MUXA::AutomationProperties::SetAccessibilityView(
                    spacer, MUXAP::AccessibilityView::Raw);
                peer.element = spacer;
            }
            else if ( tool->IsSeparator() )
            {
                MUXC::AppBarSeparator separator;
                separator.IsHitTestVisible(false);
                if ( vertical )
                {
                    // AppBarSeparator is intrinsically vertical. Rotate it
                    // inside a fixed cross-axis container so a vertical wx
                    // toolbar gets an actual horizontal separator without
                    // changing the row's requested 12-DIP extent.
                    separator.Width(separatorExtent);
                    separator.Height(wxWINUI_TOOL_MIN_WIDTH);
                    separator.HorizontalAlignment(
                        MUX::HorizontalAlignment::Center);
                    separator.VerticalAlignment(
                        MUX::VerticalAlignment::Center);
                    separator.RenderTransformOrigin({ 0.5f, 0.5f });
                    MUXM::RotateTransform rotate;
                    rotate.Angle(90.0);
                    separator.RenderTransform(rotate);

                    MUXC::Grid separatorContainer;
                    separatorContainer.Width(wxWINUI_TOOL_MIN_WIDTH);
                    separatorContainer.Height(
                        separatorExtent);
                    separatorContainer.IsHitTestVisible(false);
                    MUXA::AutomationProperties::SetAccessibilityView(
                        separatorContainer,
                        MUXAP::AccessibilityView::Raw);
                    separatorContainer.Children().Append(separator);
                    peer.element = separatorContainer;
                }
                else
                {
                    separator.Width(separatorExtent);
                    separator.Height(buttonHeight);
                    peer.element = separator;
                }
                MUXA::AutomationProperties::SetAccessibilityView(
                    separator, MUXAP::AccessibilityView::Raw);
            }
            else if ( tool->IsControl() )
            {
                wxControl * const control = tool->GetLiveControl();
                if ( !control )
                    return false;

                wxSize controlSize = control->GetBestSize();
                if ( !allControlsCurrent() )
                    return false;
                if ( controlSize.x <= 0 || controlSize.y <= 0 )
                {
                    wxControl* const currentControl =
                        tool->GetLiveControl();
                    if ( !currentControl )
                        return false;
                    controlSize = currentControl->GetSize();
                    if ( !allControlsCurrent() )
                        return false;
                }
                controlSize.x = std::max(controlSize.x, 1);
                controlSize.y = std::max(controlSize.y, 1);
                wxToolBar* const owner = getRebuildOwner();
                if ( !owner )
                    return false;
                const wxSize dipSize = owner->ToDIP(controlSize);
                if ( !allControlsCurrent() )
                    return false;

                MUXC::Border placeholder;
                placeholder.Width(std::max(dipSize.x, 1));
                placeholder.Height(std::max(dipSize.y, 1));
                placeholder.IsHitTestVisible(false);
                MUXA::AutomationProperties::SetAccessibilityView(
                    placeholder, MUXAP::AccessibilityView::Raw);
                peer.controlPlaceholder = placeholder;

                MUXC::StackPanel item;
                item.IsHitTestVisible(false);
                MUXA::AutomationProperties::SetAccessibilityView(
                    item, MUXAP::AccessibilityView::Raw);
                item.Orientation(
                    horizontalText
                        ? MUXC::Orientation::Horizontal
                        : MUXC::Orientation::Vertical);
                item.Spacing(horizontalText ? 6.0 : 2.0);
                // The AddControl() label is part of that API's own contract,
                // independent of whether button labels use wxTB_TEXT.
                MUXC::TextBlock label;
                label.Text(wxWinUIToHString(
                    wxWinUIRemoveMnemonics(tool->GetLabel())));
                label.VerticalAlignment(
                    MUX::VerticalAlignment::Center);
                label.Visibility(
                    tool->GetLabel().empty()
                        ? MUX::Visibility::Collapsed
                        : MUX::Visibility::Visible);
                item.Children().Append(label);
                peer.radioLabel = label;
                item.Children().Append(placeholder);
                peer.element = item;
            }
            else
            {
                MUXC::IconElement icon{ nullptr };
                if ( !wxWinUIBuildToolIcon(
                         *tool, *this, showIcons, requestedScale,
                         &icon, &peer.selectedPixelSize,
                         &peer.usesDisabledBitmap) )
                {
                    return false;
                }
                peer.hasIcon = static_cast<bool>(icon);

                const winrt::hstring label = wxWinUIToHString(
                    wxWinUIRemoveMnemonics(tool->GetLabel()));

                if ( tool->GetKind() == wxITEM_RADIO )
                {
                    MUXC::RadioButton button;
                    MUXC::TextBlock radioLabel{ nullptr };
                    const MUXC::StackPanel content =
                        wxWinUIBuildRadioToolContent(
                            *tool, icon, showText, horizontalText,
                            &radioLabel);
                    button.Content(content);
                    button.Height(buttonHeight);
                    button.MinWidth(wxWINUI_TOOL_MIN_WIDTH);
                    button.IsChecked(tool->IsToggled());
                    button.IsEnabled(
                        tool->IsEnabled() && IsEnabled());
                    peer.radioContent = content;
                    peer.radioLabel = radioLabel;
                    peer.primaryButton = button;
                    peer.element = button;
                }
                else if ( horizontalText )
                {
                    MUXC::TextBlock horizontalLabel{ nullptr };
                    const MUXC::StackPanel content =
                        wxWinUIBuildHorizontalToolContent(
                            *tool, icon, &horizontalLabel);
                    peer.radioLabel = horizontalLabel;
                    if ( tool->CanBeToggled() )
                    {
                        MUXCP::ToggleButton button;
                        button.Content(content);
                        button.Height(buttonHeight);
                        button.MinWidth(wxWINUI_TOOL_MIN_WIDTH);
                        button.IsChecked(tool->IsToggled());
                        button.IsEnabled(
                            tool->IsEnabled() && IsEnabled());
                        peer.primaryButton = button;
                        peer.element = button;
                    }
                    else
                    {
                        MUXC::Button button;
                        button.Content(content);
                        button.Height(buttonHeight);
                        button.MinWidth(wxWINUI_TOOL_MIN_WIDTH);
                        button.IsEnabled(
                            tool->IsEnabled() && IsEnabled());
                        peer.primaryButton = button;
                        peer.element = button;
                    }
                }
                else if ( tool->CanBeToggled() )
                {
                    MUXC::AppBarToggleButton button;
                    if ( icon )
                        button.Icon(icon);
                    button.Label(label);
                    button.LabelPosition(labelPosition);
                    button.Height(buttonHeight);
                    button.MinWidth(wxWINUI_TOOL_MIN_WIDTH);
                    button.IsChecked(tool->IsToggled());
                    button.IsEnabled(
                        tool->IsEnabled() && IsEnabled());
                    peer.primaryButton = button;
                    peer.element = button;
                }
                else
                {
                    MUXC::AppBarButton button;
                    if ( icon )
                        button.Icon(icon);
                    button.Label(label);
                    button.LabelPosition(labelPosition);
                    button.Height(buttonHeight);
                    button.MinWidth(wxWINUI_TOOL_MIN_WIDTH);
                    button.IsEnabled(
                        tool->IsEnabled() && IsEnabled());
                    peer.primaryButton = button;
                    peer.element = button;
                }

                MUXA::AutomationProperties::SetName(peer.primaryButton, label);

                const std::uint64_t peerKey = tool->peerKey;
                peer.primaryClickToken = peer.primaryButton.Click(
                    [callbackState, generation, peerKey](
                        const WF::IInspectable&,
                        const MUX::RoutedEventArgs&)
                    {
                        wxToolBar * const owner =
                            callbackState->GetOwner(generation);
                        if ( !owner || !owner->m_winui ||
                             owner->m_winui->callbackState != callbackState ||
                             owner->m_winui->peerGeneration != generation )
                        {
                            return;
                        }
                        owner->OnToolClicked(peerKey, generation);
                    });

                if ( tool->GetKind() == wxITEM_DROPDOWN )
                {
                    // A wx dropdown tool has two independent actions. Keep
                    // the main AppBarButton and add a narrow arrow button
                    // instead of turning the entire command into a flyout.
                    MUXC::Grid split;
                    MUXC::ColumnDefinition mainColumn;
                    mainColumn.Width(
                        MUX::GridLengthHelper::FromValueAndType(
                            1.0, MUX::GridUnitType::Star));
                    MUXC::ColumnDefinition arrowColumn;
                    arrowColumn.Width(
                        MUX::GridLengthHelper::FromPixels(
                            wxWINUI_TOOL_DROPDOWN_WIDTH));
                    split.ColumnDefinitions().Append(mainColumn);
                    split.ColumnDefinitions().Append(arrowColumn);
                    MUXA::AutomationProperties::SetAccessibilityView(
                        split, MUXAP::AccessibilityView::Raw);
                    MUXC::Grid::SetColumn(peer.primaryButton, 0);
                    split.Children().Append(peer.primaryButton);

                    MUXC::Button arrow;
                    arrow.Content(winrt::box_value(
                        winrt::hstring(L"\u25BE")));
                    arrow.Padding({ 0, 0, 0, 0 });
                    arrow.MinWidth(wxWINUI_TOOL_DROPDOWN_WIDTH);
                    arrow.Height(buttonHeight);
                    arrow.IsEnabled(
                        tool->IsEnabled() && IsEnabled());
                    MUXC::Grid::SetColumn(arrow, 1);
                    split.Children().Append(arrow);
                    peer.dropdownButton = arrow;

                    MUXA::AutomationProperties::SetName(
                        arrow,
                        wxWinUIToHString(
                            wxString::Format(
                                _("%s menu"),
                                wxWinUIRemoveMnemonics(
                                    tool->GetLabel()).c_str())));
                    peer.dropdownClickToken = arrow.Click(
                        [callbackState, generation, peerKey](
                            const WF::IInspectable&,
                            const MUX::RoutedEventArgs&)
                        {
                            wxToolBar * const owner =
                                callbackState->GetOwner(generation);
                            if ( !owner || !owner->m_winui ||
                                 owner->m_winui->callbackState !=
                                     callbackState ||
                                 owner->m_winui->peerGeneration != generation )
                            {
                                return;
                            }
                            owner->OnDropdownClicked(peerKey, generation);
                        });
                    peer.element = split;
                }

                const wxString visualTip =
                    HasFlag(wxTB_NO_TOOLTIPS)
                        ? wxString()
                        : tool->GetShortHelp();
                const wxString accessibleHelp =
                    !tool->GetLongHelp().empty()
                        ? tool->GetLongHelp()
                        : tool->GetShortHelp();
                if ( !wxWinUISetToolTip(
                         peer.element, visualTip, this) )
                    return false;
                if ( peer.primaryButton &&
                     !wxWinUISetToolTip(
                         peer.primaryButton, visualTip, this) )
                {
                    return false;
                }
                if ( peer.dropdownButton &&
                     !wxWinUISetToolTip(
                         peer.dropdownButton, visualTip, this) )
                {
                    return false;
                }
                MUXA::AutomationProperties::SetHelpText(
                    peer.element,
                    wxWinUIToHString(accessibleHelp));
                MUXA::AutomationProperties::SetHelpText(
                    peer.primaryButton,
                    wxWinUIToHString(accessibleHelp));
                if ( peer.dropdownButton )
                {
                    MUXA::AutomationProperties::SetHelpText(
                        peer.dropdownButton,
                        wxWinUIToHString(accessibleHelp));
                }

                peer.pointerEnteredToken = peer.element.PointerEntered(
                    [callbackState, generation, peerKey](
                        const WF::IInspectable&,
                        const MUXI::PointerRoutedEventArgs&)
                    {
                        wxToolBar * const owner =
                            callbackState->GetOwner(generation);
                        if ( !owner || !owner->m_winui ||
                             owner->m_winui->callbackState !=
                                 callbackState ||
                             owner->m_winui->peerGeneration != generation )
                        {
                            return;
                        }
                        owner->OnToolHovered(
                            peerKey, generation, true);
                    });
                peer.pointerExitedToken = peer.element.PointerExited(
                    [callbackState, generation, peerKey](
                        const WF::IInspectable&,
                        const MUXI::PointerRoutedEventArgs&)
                    {
                        wxToolBar * const owner =
                            callbackState->GetOwner(generation);
                        if ( !owner || !owner->m_winui ||
                             owner->m_winui->callbackState !=
                                 callbackState ||
                             owner->m_winui->peerGeneration != generation )
                        {
                            return;
                        }
                        owner->OnToolHovered(
                            peerKey, generation, false);
                    });
                peer.rightTappedToken = peer.element.RightTapped(
                    [callbackState, generation, peerKey](
                        const WF::IInspectable&,
                        const MUXI::RightTappedRoutedEventArgs& event)
                    {
                        wxToolBar *owner =
                            callbackState->GetOwner(generation);
                        if ( !owner || !owner->m_winui ||
                             owner->m_winui->callbackState !=
                                 callbackState ||
                             owner->m_winui->peerGeneration != generation )
                        {
                            return;
                        }

                        const MUXC::Grid root = owner->m_winui->root;
                        const MUXC::Button overflowButton =
                            owner->m_winui->overflowButton;
                        wxWinUIToolBarImpl * const eventImpl =
                            owner->m_winui.get();
                        const void * const rootIdentity =
                            winrt::get_abi(root);
                        const void * const overflowButtonIdentity =
                            winrt::get_abi(overflowButton);
                        const wxWinUIToolPeer * const peer =
                            owner->m_winui->FindPeerByKey(peerKey);
                        if ( !peer || !peer->tool )
                            return;

                        const bool wasInOverflow = peer->inOverflow;
                        const void * const peerIdentity =
                            winrt::get_abi(peer->element);
                        const auto pointIsCurrent =
                            [callbackState,
                             generation,
                             peerKey,
                             eventImpl,
                             rootIdentity,
                             overflowButtonIdentity,
                             wasInOverflow,
                             peerIdentity]()
                            {
                                wxToolBar * const currentOwner =
                                    callbackState->GetOwner(generation);
                                if ( !currentOwner ||
                                     !currentOwner->m_winui ||
                                     currentOwner->m_winui.get() !=
                                         eventImpl ||
                                     currentOwner->m_winui->
                                         callbackState !=
                                         callbackState ||
                                     currentOwner->m_winui->
                                         peerGeneration != generation ||
                                     winrt::get_abi(
                                         currentOwner->m_winui->root) !=
                                         rootIdentity ||
                                     winrt::get_abi(
                                         currentOwner->m_winui->
                                             overflowButton) !=
                                         overflowButtonIdentity )
                                {
                                    return false;
                                }

                                const wxWinUIToolPeer * const
                                    currentPeer =
                                        currentOwner->m_winui->
                                            FindPeerByKey(peerKey);
                                return currentPeer &&
                                       currentPeer->tool &&
                                       currentPeer->inOverflow ==
                                           wasInOverflow &&
                                       winrt::get_abi(
                                           currentPeer->element) ==
                                           peerIdentity;
                            };
                        wxPoint point;
                        try
                        {
                            if ( !wxWinUIGetToolRightClickPoint(
                                     root,
                                     overflowButton,
                                     *peer,
                                     &event,
                                     owner,
                                     &point,
                                     pointIsCurrent) )
                            {
                                return;
                            }
                        }
                        catch ( const winrt::hresult_error& e )
                        {
                            wxWinUILogException(
                                "WinUI toolbar RightTapped position", e);
                            return;
                        }

                        owner =
                            callbackState->GetOwner(generation);
                        if ( !owner || !owner->m_winui ||
                             owner->m_winui->callbackState !=
                                 callbackState ||
                             owner->m_winui->peerGeneration != generation ||
                             winrt::get_abi(owner->m_winui->root) !=
                                 winrt::get_abi(root) )
                        {
                            return;
                        }

                        const wxWinUIToolPeer * const livePeer =
                            owner->m_winui->FindPeerByKey(peerKey);
                        if ( !livePeer || !livePeer->tool ||
                             livePeer->inOverflow != wasInOverflow ||
                             winrt::get_abi(livePeer->element) !=
                                 peerIdentity )
                        {
                            return;
                        }

                        event.Handled(true);
                        owner->OnToolRightClicked(
                            peerKey, generation, point);
                    });
            }

            if ( packing > 0 && !tool->IsStretchable() )
            {
                const float leading =
                    static_cast<float>(packing / 2);
                const float trailing =
                    static_cast<float>(packing - packing / 2);
                peer.element.Margin(
                    vertical
                        ? MUX::Thickness{
                              0.0f, leading, 0.0f, trailing }
                        : MUX::Thickness{
                              leading, 0.0f, trailing, 0.0f });
            }

            if ( vertical )
                MUXC::Grid::SetRow(peer.element, static_cast<int>(index));
            else
                MUXC::Grid::SetColumn(
                    peer.element,
                    static_cast<int>(
                        rtl ? tools.size() - index : index));
            candidate.root.Children().Append(
                peer.element.as<MUX::UIElement>());
            candidate.peers.push_back(std::move(peer));
        }

        if ( vertical )
        {
            MUXC::RowDefinition overflowRow;
            overflowRow.Height(MUX::GridLengthHelper::Auto());
            candidate.root.RowDefinitions().Append(overflowRow);
        }
        else
        {
            MUXC::ColumnDefinition overflowColumn;
            overflowColumn.Width(MUX::GridLengthHelper::Auto());
            candidate.root.ColumnDefinitions().Append(overflowColumn);
        }

        candidate.overflowPanel = MUXC::StackPanel();
        candidate.overflowPanel.Orientation(MUXC::Orientation::Vertical);
        candidate.overflowPanel.MinWidth(
            wxWINUI_TOOL_MIN_WIDTH * 3.0);
        candidate.overflowPanel.FlowDirection(
            wxWinUIToolBarFlowDirection(GetLayoutDirection()));
        MUXA::AutomationProperties::SetAccessibilityView(
            candidate.overflowPanel, MUXAP::AccessibilityView::Raw);

        candidate.overflowFlyout = MUXC::Flyout();
        candidate.overflowFlyout.Placement(
            MUXCP::FlyoutPlacementMode::Auto);
        candidate.overflowFlyout.Content(candidate.overflowPanel);

        candidate.overflowButton = MUXC::Button();
        candidate.overflowButton.MinWidth(wxWINUI_TOOL_OVERFLOW_WIDTH);
        candidate.overflowButton.Height(buttonHeight);
        candidate.overflowButton.Padding({ 0, 0, 0, 0 });
        candidate.overflowButton.Visibility(MUX::Visibility::Collapsed);
        MUXC::FontIcon overflowIcon;
        overflowIcon.Glyph(winrt::hstring(L"\uE712"));
        candidate.overflowButton.Content(overflowIcon);
        const wxString overflowName = _("More commands");
        MUXA::AutomationProperties::SetName(
            candidate.overflowButton,
            wxWinUIToHString(overflowName));
        MUXA::AutomationProperties::SetHelpText(
            candidate.overflowButton,
            wxWinUIToHString(overflowName));
        if ( !wxWinUISetToolTip(
                 candidate.overflowButton, overflowName, this) )
        {
            return false;
        }
        candidate.overflowButton.Flyout(candidate.overflowFlyout);
        if ( vertical )
        {
            MUXC::Grid::SetRow(
                candidate.overflowButton,
                static_cast<int>(tools.size()));
        }
        else
        {
            MUXC::Grid::SetColumn(
                candidate.overflowButton,
                rtl ? 0 : static_cast<int>(tools.size()));
        }
        candidate.root.Children().Append(candidate.overflowButton);

        candidate.sizeChangedToken = candidate.root.SizeChanged(
            [callbackState, generation](
                const WF::IInspectable&,
                const MUX::SizeChangedEventArgs& args)
            {
                wxToolBar * const owner =
                    callbackState->GetOwner(generation);
                if ( !owner || !owner->m_winui ||
                     owner->m_winui->callbackState != callbackState ||
                     owner->m_winui->peerGeneration != generation )
                {
                    return;
                }
                const WF::Size size = args.NewSize();
                const double extent =
                    owner->IsVertical() ? size.Height : size.Width;
                owner->UpdateOverflow(extent, generation);

                wxToolBar * const liveOwner =
                    callbackState->GetOwner(generation);
                if ( !liveOwner || !liveOwner->m_winui ||
                     liveOwner->m_winui->callbackState != callbackState ||
                     liveOwner->m_winui->peerGeneration != generation )
                {
                    return;
                }
                liveOwner->SyncControlTools(generation);
            });

#ifdef WXWINUI_TEST_SUPPORT
        const wxWinUIToolBarTestAccess::CallbackHook loadedHook =
            rebuildImpl->nextRebuildLoadedHookForTesting;
        void * const loadedHookContext =
            rebuildImpl->nextRebuildLoadedContextForTesting;
        const std::shared_ptr<bool> loadedHookInvoked =
            std::make_shared<bool>(false);
        rebuildImpl->nextRebuildLoadedHookForTesting = nullptr;
        rebuildImpl->nextRebuildLoadedContextForTesting = nullptr;
        const auto invokeLoadedHook =
            [callbackState, generation, rebuildImpl,
             loadedHook, loadedHookContext,
             loadedHookInvoked]()
            {
                if ( !loadedHook || *loadedHookInvoked )
                    return;

                // Claim the one-shot before calling application code: a
                // reentrant layout/Loaded turn must never deliver it twice.
                *loadedHookInvoked = true;
                wxToolBar * const owner =
                    callbackState->GetOwnerDuringRebuild(generation);
                if ( !owner || !owner->m_winui ||
                     owner->m_winui.get() != rebuildImpl ||
                     owner->m_winui->callbackState != callbackState )
                {
                    return;
                }
                loadedHook(owner, loadedHookContext);
            };
        if ( loadedHook )
        {
            // wxWinUIControlHost invokes this one-shot callback from its real
            // Loaded path while this rebuild transaction is still active.
            // Resolve through shared state so a preceding focus callback may
            // already have destroyed the toolbar.
            rebuildImpl->host.SetNextContentLoadedHookForTesting(
                invokeLoadedHook);
        }
#endif

        // Loaded, focus and attached-property setters may synchronously call
        // the public toolbar API from SetContent(). Publish the complete
        // candidate first so these callbacks observe and mutate the tree
        // that is actually being attached. Event dispatch remains gated by
        // the peer-mutation depth until the transaction finishes.
        previousRoot = rebuildImpl->root;
        previousOverflowButton = rebuildImpl->overflowButton;
        previousOverflowPanel = rebuildImpl->overflowPanel;
        previousOverflowFlyout = rebuildImpl->overflowFlyout;
        previousSizeChangedToken = rebuildImpl->sizeChangedToken;
        previousPeers.swap(rebuildImpl->peers);
        previousPeerGeneration = rebuildImpl->peerGeneration;
        previousCallbackGeneration =
            callbackState->GetCommittedGeneration();

        rebuildImpl->root = candidate.root;
        rebuildImpl->overflowButton = candidate.overflowButton;
        rebuildImpl->overflowPanel = candidate.overflowPanel;
        rebuildImpl->overflowFlyout = candidate.overflowFlyout;
        rebuildImpl->sizeChangedToken = candidate.sizeChangedToken;
        rebuildImpl->peers.swap(candidate.peers);
        rebuildImpl->peerGeneration = generation;
        callbackState->CommitGeneration(generation);
        candidate.published = true;
        provisionalPublished = true;

        const wxWinUIToolBarPeerMutationGuard mutationGuard(callbackState);
        wxUnusedVar(mutationGuard);
#ifdef WXWINUI_TEST_SUPPORT
        if ( failAfterPublishForTesting && loadedHook )
        {
            // The failure seam deliberately skips SetContent(), so there can
            // be no real Loaded callback. Still execute its configured
            // transaction-boundary hook against the provisionally published
            // candidate: this deterministically covers a nested application
            // mutation followed by host rejection without attaching a tree
            // that the rollback path could no longer replace atomically.
            invokeLoadedHook();
        }
#endif
        const bool contentSet =
#ifdef WXWINUI_TEST_SUPPORT
            !failAfterPublishForTesting &&
#endif
            rebuildImpl->host.SetContent(rebuildImpl->root);
        candidateAttached = contentSet;

        // SetContent() can synchronously run Loaded/focus application code.
        // Never touch `this` or rebuildImpl until the shared owner state and
        // the exact implementation identity have both been revalidated.
        wxToolBar *liveOwner =
            callbackState->GetOwnerDuringRebuild(generation);
        if ( !liveOwner || !liveOwner->m_winui ||
             liveOwner->m_winui.get() != rebuildImpl ||
             liveOwner->m_winui->callbackState != callbackState ||
             !allControlsCurrent() )
        {
            rollbackProvisionalCandidate();
            retirePreviousGeneration();
            return false;
        }

#ifdef WXWINUI_TEST_SUPPORT
        // A normal rebuild leaves layout asynchronous. The deterministic
        // Loaded seam explicitly asks us to cross that boundary now so tests
        // exercise the same callback window before this transaction publishes.
        if ( contentSet && loadedHook )
        {
            rebuildImpl->host.ForceRender();
            for ( unsigned pump = 0;
                  pump < 16 && !*loadedHookInvoked;
                  ++pump )
            {
                // The shared island may defer Loaded by a few dispatcher turns
                // even after synchronous measure/arrange. This pump exists
                // only for the explicitly armed deterministic test seam and
                // still delivers the real FrameworkElement::Loaded callback.
                wxYield();
            }
            if ( !*loadedHookInvoked )
            {
                // A hidden shared-host slot is permitted to remain unloaded
                // indefinitely. The implementation-only seam must still be
                // deterministic, so after the bounded real-Loaded attempt,
                // invoke the same generation-checked callback at the exact
                // committed SetContent()/ForceRender boundary. The pending
                // host hook is cleared below, preventing a later duplicate.
                invokeLoadedHook();
            }
            liveOwner =
                callbackState->GetOwnerDuringRebuild(generation);
            if ( !liveOwner || !liveOwner->m_winui ||
                 liveOwner->m_winui.get() != rebuildImpl ||
                 liveOwner->m_winui->callbackState != callbackState ||
                 !allControlsCurrent() )
            {
                rollbackProvisionalCandidate();
                retirePreviousGeneration();
                return false;
            }
        }

        // The test seam is intentionally synchronous. Don't retain a stale
        // transaction callback into a later Loaded generation.
        rebuildImpl->host.SetNextContentLoadedHookForTesting({});
#endif
        if ( !contentSet )
        {
            rollbackProvisionalCandidate();
            return false;
        }
        if ( !allControlsCurrent() )
        {
            rollbackProvisionalCandidate();
            retirePreviousGeneration();
            return false;
        }

        liveOwner =
            callbackState->GetOwnerDuringRebuild(generation);
        if ( !liveOwner || !liveOwner->m_winui ||
             liveOwner->m_winui.get() != rebuildImpl ||
             liveOwner->m_winui->callbackState != callbackState ||
             rebuildImpl->peerGeneration != generation )
        {
            return false;
        }

        // Reconcile every peer from the current common model. This closes the
        // window in which a detached XAML setter or Loaded callback changed
        // label or short help while the candidate was being built, and also
        // applies logical wxToolTip ownership to embedded controls only after
        // the host accepted the candidate.
        std::vector<std::uint64_t> labelKeys;
        labelKeys.reserve(rebuildImpl->peers.size());
        for ( const wxWinUIToolPeer& peer : rebuildImpl->peers )
        {
            if ( peer.tool )
                labelKeys.push_back(peer.tool->peerKey);
        }
        for ( const std::uint64_t peerKey : labelKeys )
        {
            if ( !liveOwner->ApplyToolLabel(peerKey) )
            {
                rollbackProvisionalCandidate();
                retirePreviousGeneration();
                return false;
            }
            liveOwner =
                callbackState->GetOwnerDuringRebuild(generation);
            if ( !liveOwner || !liveOwner->m_winui ||
                 liveOwner->m_winui.get() != rebuildImpl ||
                 liveOwner->m_winui->callbackState != callbackState ||
                 rebuildImpl->peerGeneration != generation ||
                 !allControlsCurrent() )
            {
                rollbackProvisionalCandidate();
                retirePreviousGeneration();
                return false;
            }
        }

        if ( !rebuildImpl->shortHelpSyncActive &&
             !rebuildImpl->shortHelpRetryCallbackActive )
        {
            rebuildImpl->shortHelpDeferredCallbacksRemaining = 2;
            rebuildImpl->shortHelpDriverWarningEmitted = false;
        }
        if ( !rebuildImpl->enabledSyncActive &&
             !rebuildImpl->enabledRetryCallbackActive )
        {
            rebuildImpl->enabledDeferredCallbacksRemaining = 2;
            rebuildImpl->enabledDriverWarningEmitted = false;
        }
        const bool enabledModelChangedDuringBuild =
            rebuildImpl->modelRevision != candidateModelRevision;
        bool hasControlPeer = false;
        for ( const wxWinUIToolPeer& peer : rebuildImpl->peers )
        {
            if ( !peer.tool )
                continue;

            peer.tool->shortHelpProjectionFailures = 0;
            peer.tool->shortHelpProjectionQuarantined = false;
            rebuildImpl->pendingShortHelpKeys.insert(
                peer.tool->peerKey);
            if ( enabledModelChangedDuringBuild ||
                 peer.tool->enabledProjectionFailures ||
                 peer.tool->enabledProjectionQuarantined )
            {
                peer.tool->enabledProjectionFailures = 0;
                peer.tool->enabledProjectionQuarantined = false;
                rebuildImpl->pendingEnabledKeys.insert(
                    peer.tool->peerKey);
            }
            if ( peer.tool->IsControl() )
                hasControlPeer = true;
        }

        if ( !rebuildImpl->pendingShortHelpKeys.empty() )
        {
            liveOwner->ConvergeToolShortHelpUpdates();
            liveOwner =
                callbackState->GetOwnerDuringRebuild(generation);
            if ( !liveOwner || !liveOwner->m_winui ||
                 liveOwner->m_winui.get() != rebuildImpl ||
                 liveOwner->m_winui->callbackState != callbackState ||
                 rebuildImpl->peerGeneration != generation ||
                 !allControlsCurrent() )
            {
                rollbackProvisionalCandidate();
                retirePreviousGeneration();
                return false;
            }
        }

        if ( !rebuildImpl->pendingEnabledKeys.empty() )
        {
            liveOwner->ConvergeToolEnabledUpdates();
            liveOwner =
                callbackState->GetOwnerDuringRebuild(generation);
            if ( !liveOwner || !liveOwner->m_winui ||
                 liveOwner->m_winui.get() != rebuildImpl ||
                 liveOwner->m_winui->callbackState != callbackState ||
                 rebuildImpl->peerGeneration != generation ||
                 !allControlsCurrent() )
            {
                rollbackProvisionalCandidate();
                retirePreviousGeneration();
                return false;
            }
        }

        // Control tools are independent slots in the same TLW island. Their
        // transparent XAML placeholders supply layout; the wx controls stay
        // normal child windows/slots and are positioned over those bounds.
        // SetSize() on an embedded control is an application re-entrancy
        // point. Defer it out of the mutation transaction; generation checks
        // make this harmless if another rebuild or destruction wins first.
        if ( hasControlPeer && wxTheApp )
        {
            wxTheApp->CallAfter(
                [callbackState, generation]()
                {
                    wxToolBar * const owner =
                        callbackState->GetOwner(generation);
                    if ( !owner || !owner->m_winui ||
                         owner->m_winui->callbackState != callbackState ||
                         owner->m_winui->peerGeneration != generation )
                    {
                        return;
                    }
                    owner->SyncControlTools(generation);
                });
        }

        if ( !allControlsCurrent() )
        {
            rollbackProvisionalCandidate();
            retirePreviousGeneration();
            return false;
        }

        // Every callback-capable reconciliation boundary accepted the same
        // live control topology. Only now is the host candidate irrevocable.
        candidateAttached = false;
        provisionalPublished = false;
        retirePreviousGeneration();
        return true;
    }
    catch ( const winrt::hresult_error& e )
    {
        rollbackProvisionalCandidate();
        retirePreviousGeneration();
#ifdef WXWINUI_TEST_SUPPORT
        wxToolBar * const liveOwner =
            callbackState->GetOwnerDuringRebuild(generation);
        if ( liveOwner && liveOwner->m_winui &&
             liveOwner->m_winui.get() == rebuildImpl &&
             liveOwner->m_winui->callbackState == callbackState )
        {
            // SetContent() may have failed after the one-shot test callback
            // was armed but before Loaded consumed it.
            rebuildImpl->host.SetNextContentLoadedHookForTesting({});
        }
#endif
        wxWinUILogException("WinUI toolbar rebuild", e);
        return false;
    }
}

// ----------------------------------------------------------------------------
// Event routing
// ----------------------------------------------------------------------------

void wxToolBar::OnToolClicked(std::uint64_t peerKey,
                              std::uint64_t generation)
{
    if ( !m_winui || !m_winui->callbackState ||
         m_winui->peerGeneration != generation || !IsEnabled() )
    {
        return;
    }

    const std::shared_ptr<wxWinUIToolBarCallbackState> callbackState =
        m_winui->callbackState;
    wxWinUIToolPeer *peer = m_winui->FindPeerByKey(peerKey);
    if ( !peer || !peer->tool || !peer->tool->IsEnabled() )
        return;

    if ( peer->inOverflow && m_winui->overflowFlyout )
    {
        try
        {
            m_winui->overflowFlyout.Hide();
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException(
                "WinUI toolbar overflow command close", e);
        }

        wxToolBar * const owner =
            callbackState->GetOwner(generation);
        if ( !owner || !owner->m_winui ||
             owner->m_winui->callbackState != callbackState ||
             owner->m_winui->peerGeneration != generation )
        {
            return;
        }
        peer = owner->m_winui->FindPeerByKey(peerKey);
        if ( !peer || !peer->tool || !peer->tool->IsEnabled() )
            return;
    }

    wxWinUIToolBarTool *tool = peer->tool;
    const int toolid = tool->GetId();
    bool toggled = tool->IsToggled();
    std::vector<std::pair<std::uint64_t, bool>> oldRadioStates;

    if ( tool->CanBeToggled() )
    {
        if ( tool->GetKind() == wxITEM_RADIO )
        {
            // Capture the complete contiguous group: a veto must restore the
            // previous selected radio, not merely uncheck the clicked one.
            wxToolBarToolsList::compatibility_iterator node =
                m_tools.Find(tool);
            wxToolBarToolsList::compatibility_iterator first = node;
            while ( first && first->GetPrevious() )
            {
                wxToolBarToolBase * const previous =
                    first->GetPrevious()->GetData();
                if ( !previous->IsButton() ||
                     previous->GetKind() != wxITEM_RADIO )
                {
                    break;
                }
                first = first->GetPrevious();
            }
            for ( wxToolBarToolsList::compatibility_iterator current = first;
                  current;
                  current = current->GetNext() )
            {
                wxToolBarToolBase * const radio = current->GetData();
                if ( !radio->IsButton() ||
                     radio->GetKind() != wxITEM_RADIO )
                {
                    break;
                }
                wxWinUIToolBarTool * const winuiRadio =
                    static_cast<wxWinUIToolBarTool *>(radio);
                oldRadioStates.emplace_back(
                    winuiRadio->peerKey, radio->IsToggled());
            }

            toggled = true;
            tool->Toggle(true);
            DoToggleTool(tool, true);
            UnToggleRadioGroup(tool);
        }
        else
        {
            toggled = !tool->IsToggled();
            tool->Toggle(toggled);
            DoToggleTool(tool, toggled);
        }
    }

    const bool accepted = OnLeftClick(toolid, toggled);

    wxToolBar * const liveOwner =
        callbackState->GetOwner(generation);
    if ( !liveOwner || !liveOwner->m_winui ||
         liveOwner->m_winui->callbackState != callbackState ||
         liveOwner->m_winui->peerGeneration != generation )
    {
        return;
    }

    peer = liveOwner->m_winui->FindPeerByKey(peerKey);
    if ( accepted || !peer || !peer->tool ||
         !peer->tool->CanBeToggled() )
    {
        return;
    }

    if ( peer->tool->GetKind() == wxITEM_RADIO )
    {
        for ( const auto& oldState : oldRadioStates )
        {
            wxWinUIToolPeer * const radioPeer =
                liveOwner->m_winui->FindPeerByKey(oldState.first);
            if ( radioPeer && radioPeer->tool &&
                 radioPeer->tool->CanBeToggled() )
            {
                radioPeer->tool->Toggle(oldState.second);
                liveOwner->DoToggleTool(
                    radioPeer->tool, oldState.second);
            }
        }
    }
    else
    {
        peer->tool->Toggle(!toggled);
        liveOwner->DoToggleTool(peer->tool, !toggled);
    }
}

void wxToolBar::OnDropdownClicked(std::uint64_t peerKey,
                                  std::uint64_t generation)
{
    if ( !m_winui || !m_winui->callbackState ||
         m_winui->peerGeneration != generation || !m_winui->root )
    {
        return;
    }

    wxWinUIToolBarImpl * const dropdownImpl = m_winui.get();
    const std::shared_ptr<wxWinUIToolBarCallbackState> callbackState =
        m_winui->callbackState;
    const void * const rootIdentity =
        winrt::get_abi(m_winui->root);
    wxToolBar * const toolbarIdentity = this;
    const auto getCurrentOwner =
        [callbackState,
         generation,
         dropdownImpl,
         rootIdentity,
         toolbarIdentity]() -> wxToolBar *
        {
            wxToolBar * const owner =
                callbackState->GetOwner(generation);
            if ( owner != toolbarIdentity || !owner->m_winui ||
                 owner->m_winui.get() != dropdownImpl ||
                 owner->m_winui->callbackState != callbackState ||
                 owner->m_winui->peerGeneration != generation ||
                 winrt::get_abi(owner->m_winui->root) != rootIdentity )
            {
                return nullptr;
            }
            return owner;
        };
    const auto resolvePeer =
        [&getCurrentOwner, peerKey]() -> wxWinUIToolPeer *
        {
            wxToolBar * const owner = getCurrentOwner();
            return owner ? owner->m_winui->FindPeerByKey(peerKey)
                         : nullptr;
        };

    wxToolBar *owner = getCurrentOwner();
    if ( !owner )
        return;
    const bool toolbarEnabled = owner->IsEnabled();
    if ( !toolbarEnabled || !getCurrentOwner() )
        return;

    wxWinUIToolPeer *initialPeer = resolvePeer();
    if ( !initialPeer || !initialPeer->tool || !initialPeer->element )
        return;
    const void * const peerIdentity =
        winrt::get_abi(initialPeer->element);
    const bool fromOverflow = initialPeer->inOverflow;
    const auto getCurrentPeer =
        [&resolvePeer, peerIdentity, fromOverflow]()
            -> wxWinUIToolPeer *
        {
            wxWinUIToolPeer * const peer = resolvePeer();
            if ( !peer || !peer->tool ||
                 winrt::get_abi(peer->element) != peerIdentity ||
                 peer->inOverflow != fromOverflow )
            {
                return nullptr;
            }
            return peer;
        };

    {
        wxWinUIToolPeer * const peer = getCurrentPeer();
        if ( !peer || !peer->tool->IsEnabled() )
            return;
    }
    if ( !getCurrentPeer() )
        return;
    {
        wxWinUIToolPeer * const peer = getCurrentPeer();
        if ( !peer || !peer->tool->IsButton() )
            return;
    }
    if ( !getCurrentPeer() )
        return;
    {
        wxWinUIToolPeer * const peer = getCurrentPeer();
        if ( !peer ||
             peer->tool->GetKind() != wxITEM_DROPDOWN )
        {
            return;
        }
    }
    if ( !getCurrentPeer() )
    {
        return;
    }

#if wxUSE_MENUS
    wxRect popupAnchorBounds;
    try
    {
        owner = getCurrentOwner();
        wxWinUIToolPeer * const peer = getCurrentPeer();
        if ( !owner || !peer )
            return;

        const MUX::FrameworkElement anchor = fromOverflow
            ? owner->m_winui->overflowButton
                  .as<MUX::FrameworkElement>()
            : peer->element;
        if ( !anchor || !getCurrentPeer() )
            return;

        const void * const anchorIdentity = winrt::get_abi(anchor);
        const auto geometryIsCurrent =
            [&getCurrentOwner,
             &getCurrentPeer,
             fromOverflow,
             anchorIdentity]()
            {
                wxToolBar * const currentOwner =
                    getCurrentOwner();
                wxWinUIToolPeer * const currentPeer =
                    getCurrentPeer();
                if ( !currentOwner || !currentPeer )
                    return false;

                return fromOverflow
                    ? winrt::get_abi(
                          currentOwner->m_winui->overflowButton) ==
                          anchorIdentity
                    : winrt::get_abi(currentPeer->element) ==
                          anchorIdentity;
            };
        popupAnchorBounds =
            wxWinUIGetToolBounds(
                owner->m_winui->root,
                anchor,
                owner,
                geometryIsCurrent);
        if ( !geometryIsCurrent() )
            return;
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException(
            "WinUI toolbar dropdown anchor snapshot", e);
        return;
    }
#endif

    if ( fromOverflow )
    {
        owner = getCurrentOwner();
        if ( !owner || !getCurrentPeer() )
            return;

        const MUXC::Flyout overflowFlyout =
            owner->m_winui->overflowFlyout;
        const void * const overflowFlyoutIdentity =
            winrt::get_abi(overflowFlyout);
        if ( !getCurrentPeer() )
            return;

        try
        {
            if ( overflowFlyout )
                overflowFlyout.Hide();
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException(
                "WinUI toolbar overflow dropdown close", e);
        }

        owner = getCurrentOwner();
        if ( !owner || !getCurrentPeer() ||
             winrt::get_abi(owner->m_winui->overflowFlyout) !=
                 overflowFlyoutIdentity )
        {
            return;
        }
    }

    int toolid = wxID_NONE;
    {
        wxWinUIToolPeer * const peer = getCurrentPeer();
        if ( !peer )
            return;
        toolid = peer->tool->GetId();
    }
    owner = getCurrentOwner();
    if ( !owner || !getCurrentPeer() )
        return;

    wxCommandEvent event(wxEVT_TOOL_DROPDOWN, toolid);
    event.SetEventObject(owner);
    const bool handled = owner->HandleWindowEvent(event);

    wxToolBar * const liveOwner =
        getCurrentOwner();
    if ( !liveOwner || !getCurrentPeer() || handled )
    {
        return;
    }

#if wxUSE_MENUS
    wxMenu *menu = nullptr;
    {
        wxWinUIToolPeer * const peer = getCurrentPeer();
        if ( !peer )
            return;
        menu = peer->tool->GetDropdownMenu();
    }
    if ( !getCurrentOwner() || !getCurrentPeer() )
        return;
    if ( !menu )
        return;

    liveOwner->PopupMenu(
        menu,
        popupAnchorBounds.x,
        popupAnchorBounds.GetBottom());
#endif
}

void wxToolBar::OnToolHovered(std::uint64_t peerKey,
                              std::uint64_t generation,
                              bool entered)
{
    if ( !m_winui || !m_winui->callbackState ||
         m_winui->peerGeneration != generation )
    {
        return;
    }

    wxWinUIToolPeer * const peer =
        m_winui->FindPeerByKey(peerKey);
    if ( !peer || !peer->tool || !peer->tool->IsButton() )
        return;

    OnMouseEnter(entered ? peer->tool->GetId() : wxID_ANY);
}

void wxToolBar::OnToolRightClicked(std::uint64_t peerKey,
                                   std::uint64_t generation,
                                   const wxPoint& point)
{
    if ( !m_winui || !m_winui->callbackState ||
         m_winui->peerGeneration != generation || !IsEnabled() )
    {
        return;
    }

    wxWinUIToolPeer * const peer =
        m_winui->FindPeerByKey(peerKey);
    if ( !peer || !peer->tool || !peer->tool->IsButton() ||
         !peer->tool->IsEnabled() )
    {
        return;
    }

    OnRightClick(peer->tool->GetId(), point.x, point.y);
}

// ----------------------------------------------------------------------------
// Incremental public updates
// ----------------------------------------------------------------------------

void wxToolBar::ConvergeToolEnabledUpdates()
{
    if ( !m_winui || m_winui->closed || !m_winui->callbackState ||
         m_winui->enabledSyncActive )
    {
        return;
    }

    wxWinUIToolBarImpl * const updateImpl = m_winui.get();
    const std::shared_ptr<wxWinUIToolBarCallbackState> callbackState =
        updateImpl->callbackState;
    updateImpl->enabledSyncActive = true;

    const auto finish =
        [&](bool defer, std::uint64_t retryKey)
        {
            wxToolBar * const owner = callbackState->GetLiveOwner();
            if ( !owner || !owner->m_winui ||
                 owner->m_winui.get() != updateImpl ||
                 owner->m_winui->callbackState != callbackState ||
                 owner->m_winui->closed )
            {
                return;
            }

            wxWinUIToolBarImpl * const liveImpl = owner->m_winui.get();
            if ( retryKey )
            {
                wxWinUIToolPeer * const peer =
                    liveImpl->FindPeerByKey(retryKey);
                if ( !peer || !peer->tool ||
                     !peer->tool->enabledProjectionQuarantined )
                {
                    liveImpl->pendingEnabledKeys.insert(retryKey);
                }
            }
            liveImpl->enabledSyncActive = false;

            if ( liveImpl->pendingEnabledKeys.empty() ||
                 !defer || !wxTheApp ||
                 liveImpl->enabledSyncScheduled )
            {
                return;
            }

            if ( !liveImpl->enabledDeferredCallbacksRemaining )
            {
                if ( !liveImpl->enabledDriverWarningEmitted )
                {
                    liveImpl->enabledDriverWarningEmitted = true;
                    wxLogWarning(
                        "WinUI toolbar enabled projection did not converge "
                        "within its bounded retry budget");
                }
                return;
            }
            --liveImpl->enabledDeferredCallbacksRemaining;

            liveImpl->enabledSyncScheduled = true;
            wxTheApp->CallAfter(
                [callbackState, updateImpl]()
                {
                    wxToolBar *liveOwner =
                        callbackState->GetLiveOwner();
                    if ( !liveOwner || !liveOwner->m_winui ||
                         liveOwner->m_winui.get() != updateImpl ||
                         liveOwner->m_winui->callbackState != callbackState ||
                         liveOwner->m_winui->closed )
                    {
                        return;
                    }

                    liveOwner->m_winui->enabledSyncScheduled = false;
                    liveOwner->m_winui->enabledRetryCallbackActive = true;
                    liveOwner->ConvergeToolEnabledUpdates();

                    liveOwner = callbackState->GetLiveOwner();
                    if ( liveOwner && liveOwner->m_winui &&
                         liveOwner->m_winui.get() == updateImpl &&
                         liveOwner->m_winui->callbackState == callbackState )
                    {
                        liveOwner->m_winui->
                            enabledRetryCallbackActive = false;
                    }
                });
        };

    const auto queueLatest =
        [&](std::uint64_t peerKey)
        {
            wxToolBar * const owner = callbackState->GetLiveOwner();
            if ( owner && owner->m_winui &&
                 owner->m_winui.get() == updateImpl &&
                 owner->m_winui->callbackState == callbackState &&
                 !owner->m_winui->closed )
            {
                wxWinUIToolPeer * const peer =
                    owner->m_winui->FindPeerByKey(peerKey);
                if ( !peer || !peer->tool ||
                     !peer->tool->enabledProjectionQuarantined )
                {
                    owner->m_winui->pendingEnabledKeys.insert(peerKey);
                }
            }
        };

    const auto recordFailure =
        [&](std::uint64_t generation,
            std::uint64_t peerKey,
            std::uint64_t toolRevision) -> bool
        {
            wxToolBar * const owner =
                callbackState->GetOwnerForModelMutation(generation);
            if ( !owner || !owner->m_winui ||
                 owner->m_winui.get() != updateImpl ||
                 owner->m_winui->callbackState != callbackState ||
                 owner->m_winui->peerGeneration != generation )
            {
                return false;
            }

            wxWinUIToolPeer * const peer =
                owner->m_winui->FindPeerByKey(peerKey);
            if ( !peer || !peer->tool )
                return true;
            if ( peer->tool->enabledRevision != toolRevision )
            {
                owner->m_winui->pendingEnabledKeys.insert(peerKey);
                return true;
            }

            if ( ++peer->tool->enabledProjectionFailures >= 3 )
            {
                peer->tool->enabledProjectionQuarantined = true;
                wxLogWarning(
                    "WinUI toolbar quarantined a failing enabled peer "
                    "(tool id %d); a new model write or peer rebuild will "
                    "retry it",
                    peer->tool->GetId());
            }
            else
            {
                owner->m_winui->pendingEnabledKeys.insert(peerKey);
            }
            return true;
        };

    constexpr unsigned MaxSynchronousPasses = 16;
    for ( unsigned pass = 0; pass < MaxSynchronousPasses; ++pass )
    {
        wxToolBar *owner = callbackState->GetLiveOwner();
        if ( !owner || !owner->m_winui ||
             owner->m_winui.get() != updateImpl ||
             owner->m_winui->callbackState != callbackState ||
             owner->m_winui->closed )
        {
            return;
        }

        wxWinUIToolBarImpl * const liveImpl = owner->m_winui.get();
        if ( liveImpl->pendingEnabledKeys.empty() )
        {
            finish(false, 0);
            return;
        }

        auto pendingIt = liveImpl->pendingEnabledKeys.upper_bound(
            liveImpl->enabledFairnessCursor);
        if ( pendingIt == liveImpl->pendingEnabledKeys.end() )
            pendingIt = liveImpl->pendingEnabledKeys.begin();
        const std::uint64_t peerKey = *pendingIt;
        liveImpl->enabledFairnessCursor = peerKey;
        liveImpl->pendingEnabledKeys.erase(pendingIt);
        const std::uint64_t generation = liveImpl->peerGeneration;
        wxWinUIToolPeer * const peer =
            liveImpl->FindPeerByKey(peerKey);
        if ( !peer || !peer->tool )
            continue;
        if ( peer->tool->enabledProjectionQuarantined )
            continue;

        const std::uint64_t toolRevision =
            peer->tool->enabledRevision;
            const bool desiredEnabled =
                peer->tool->IsEnabled() && owner->IsEnabled();
            const bool isControl = peer->tool->IsControl();
            const bool isRadio =
                peer->tool->IsButton() &&
                peer->tool->GetKind() == wxITEM_RADIO;
            const bool wasInOverflow = peer->inOverflow;
            const MUXCP::ButtonBase primaryButton = peer->primaryButton;
        const MUXCP::ButtonBase dropdownButton = peer->dropdownButton;
        wxWeakRef<wxWindow> controlLifetime(
            isControl ? peer->tool->GetLiveControl() : nullptr);

        // 0: owner/generation lost, 1: a newer model write won, 2: current.
        const auto afterBoundary =
            [&](bool invokeEnableHook) -> int
            {
                wxToolBar *liveOwner =
                    callbackState->GetOwnerForModelMutation(generation);
                if ( !liveOwner || !liveOwner->m_winui ||
                     liveOwner->m_winui.get() != updateImpl ||
                     liveOwner->m_winui->callbackState != callbackState ||
                     liveOwner->m_winui->peerGeneration != generation )
                {
                    return 0;
                }

                wxWinUIToolPeer *currentPeer =
                    liveOwner->m_winui->FindPeerByKey(peerKey);
                if ( !currentPeer || !currentPeer->tool ||
                     currentPeer->tool->enabledRevision != toolRevision )
                {
                    return 1;
                }

#ifdef WXWINUI_TEST_SUPPORT
                if ( invokeEnableHook )
                {
                    const wxWinUIToolBarTestAccess::CallbackHook hook =
                        liveOwner->m_winui->
                            nextEnableSetterHookForTesting;
                    void * const hookContext =
                        liveOwner->m_winui->
                            nextEnableSetterContextForTesting;
                    liveOwner->m_winui->
                        nextEnableSetterHookForTesting = nullptr;
                    liveOwner->m_winui->
                        nextEnableSetterContextForTesting = nullptr;
                    if ( hook )
                    {
                        hook(liveOwner, hookContext);
                        liveOwner =
                            callbackState->GetOwnerForModelMutation(
                                generation);
                        if ( !liveOwner || !liveOwner->m_winui ||
                             liveOwner->m_winui.get() != updateImpl ||
                             liveOwner->m_winui->callbackState !=
                                 callbackState ||
                             liveOwner->m_winui->peerGeneration != generation )
                        {
                            return 0;
                        }
                    }
                }
#else
                wxUnusedVar(invokeEnableHook);
#endif

                currentPeer =
                    liveOwner->m_winui->FindPeerByKey(peerKey);
                if ( !currentPeer || !currentPeer->tool ||
                     currentPeer->tool->enabledRevision != toolRevision )
                {
                    return 1;
                }
                return 2;
            };

        MUXC::IconElement icon{ nullptr };
        wxSize selectedPixelSize;
        bool usesDisabled = false;
        if ( !isControl )
        {
            if ( !wxWinUIBuildToolIcon(
                     *peer->tool, *owner,
                     !owner->HasFlag(wxTB_NOICONS),
                     0.0, &icon, &selectedPixelSize, &usesDisabled) )
            {
                if ( !recordFailure(generation, peerKey, toolRevision) )
                {
                    finish(true, peerKey);
                    return;
                }
                continue;
            }

            const int state = afterBoundary(false);
            if ( state == 0 )
            {
                finish(true, peerKey);
                return;
            }
            if ( state == 1 )
            {
                queueLatest(peerKey);
                continue;
            }
        }

        bool restartLatest = false;
        bool setterFailed = false;
        try
        {
            if ( isControl )
            {
                wxWindow * const control = controlLifetime.get();
                if ( control )
                {
                    control->Enable(desiredEnabled);
                    const int state = afterBoundary(true);
                    if ( state == 0 )
                    {
                        finish(true, peerKey);
                        return;
                    }
                    restartLatest = state == 1;
                }
            }
            else
            {
                if ( primaryButton )
                {
                    if ( isRadio )
                    {
                        MUXC::TextBlock radioLabel{ nullptr };
                        const bool showText =
                            owner->HasFlag(wxTB_TEXT);
                        const bool horizontalText =
                            showText &&
                            owner->HasFlag(wxTB_HORZ_LAYOUT);
                        const MUXC::StackPanel content =
                            wxWinUIBuildRadioToolContent(
                                *peer->tool, icon,
                                showText || wasInOverflow,
                                horizontalText || wasInOverflow,
                                &radioLabel);
                        primaryButton
                            .as<MUXC::ContentControl>()
                            .Content(content);
                        const int state = afterBoundary(false);
                        if ( state == 0 )
                        {
                            finish(true, peerKey);
                            return;
                        }
                        if ( state == 1 )
                        {
                            restartLatest = true;
                        }
                        else
                        {
                            wxWinUIToolPeer * const currentPeer =
                                owner->m_winui->FindPeerByKey(peerKey);
                            if ( !currentPeer )
                            {
                                finish(true, peerKey);
                                return;
                            }
                            currentPeer->radioContent = content;
                            currentPeer->radioLabel = radioLabel;
                        }
                    }
                    else if ( owner->HasFlag(wxTB_TEXT) &&
                         owner->HasFlag(wxTB_HORZ_LAYOUT) )
                    {
                        if ( const auto content =
                                 primaryButton.try_as<MUXC::ContentControl>() )
                        {
                            MUXC::TextBlock horizontalLabel{ nullptr };
                            content.Content(
                                wxWinUIBuildHorizontalToolContent(
                                    *peer->tool, icon,
                                    &horizontalLabel));
                            const int state = afterBoundary(false);
                            if ( state == 0 )
                            {
                                finish(true, peerKey);
                                return;
                            }
                            if ( state == 1 )
                                restartLatest = true;
                            else
                            {
                                wxWinUIToolPeer * const currentPeer =
                                    owner->m_winui->FindPeerByKey(peerKey);
                                if ( !currentPeer )
                                {
                                    finish(true, peerKey);
                                    return;
                                }
                                currentPeer->radioLabel =
                                    horizontalLabel;
                            }
                        }
                    }
                    else
                    {
                        wxWinUISetButtonIcon(primaryButton, icon);
                        const int state = afterBoundary(false);
                        if ( state == 0 )
                        {
                            finish(true, peerKey);
                            return;
                        }
                        if ( state == 1 )
                            restartLatest = true;
                    }

                    if ( !restartLatest )
                    {
                        primaryButton.IsEnabled(desiredEnabled);
                        const int state = afterBoundary(true);
                        if ( state == 0 )
                        {
                            finish(true, peerKey);
                            return;
                        }
                        if ( state == 1 )
                            restartLatest = true;
                    }
                }

                if ( !restartLatest && dropdownButton )
                {
                    dropdownButton.IsEnabled(desiredEnabled);
                    const int state = afterBoundary(true);
                    if ( state == 0 )
                    {
                        finish(true, peerKey);
                        return;
                    }
                    if ( state == 1 )
                        restartLatest = true;
                }
            }
        }
        catch ( const winrt::hresult_error& e )
        {
            setterFailed = true;
            wxWinUILogException("WinUI toolbar enabled update", e);
        }

        if ( restartLatest )
        {
            queueLatest(peerKey);
            continue;
        }
        if ( setterFailed )
        {
            if ( !recordFailure(generation, peerKey, toolRevision) )
            {
                finish(true, peerKey);
                return;
            }
            continue;
        }

        owner = callbackState->GetOwnerForModelMutation(generation);
        if ( !owner || !owner->m_winui ||
             owner->m_winui.get() != updateImpl ||
             owner->m_winui->callbackState != callbackState ||
             owner->m_winui->peerGeneration != generation )
        {
            finish(true, peerKey);
            return;
        }
        wxWinUIToolPeer * const currentPeer =
            owner->m_winui->FindPeerByKey(peerKey);
        if ( !currentPeer || !currentPeer->tool )
            continue;
        if ( currentPeer->tool->enabledRevision != toolRevision )
        {
            queueLatest(peerKey);
            continue;
        }

        const wxSize previousPixelSize =
            currentPeer->selectedPixelSize;
        currentPeer->selectedPixelSize = selectedPixelSize;
        currentPeer->hasIcon = static_cast<bool>(icon);
        currentPeer->usesDisabledBitmap = usesDisabled;
        // The disabled bundle is allowed to have a different intrinsic size.
        // Preserve a valid primary-surface measurement when both selected
        // bitmaps have the same contract size, but invalidate it otherwise.
        if ( previousPixelSize != selectedPixelSize )
            currentPeer->naturalExtent = 0.0;
        currentPeer->tool->enabledProjectionFailures = 0;
        currentPeer->tool->enabledProjectionQuarantined = false;

        // Flush may emit focus changes. Revalidate its exact model revision
        // before considering this peer converged.
        updateImpl->host.ForceRender();
        const int state = afterBoundary(false);
        if ( state == 0 )
        {
            finish(true, peerKey);
            return;
        }
        if ( state == 1 )
        {
            queueLatest(peerKey);
            continue;
        }

        owner = callbackState->GetOwnerForModelMutation(generation);
        if ( !owner || !owner->m_winui ||
             owner->m_winui.get() != updateImpl ||
             owner->m_winui->callbackState != callbackState ||
             owner->m_winui->peerGeneration != generation )
        {
            finish(true, peerKey);
            return;
        }
        const double extent = owner->IsVertical()
            ? owner->m_winui->root.ActualHeight()
            : owner->m_winui->root.ActualWidth();
        if ( extent > 0.0 )
        {
            owner->UpdateOverflow(extent, generation);
            owner = callbackState->GetOwnerForModelMutation(generation);
            if ( !owner || !owner->m_winui ||
                 owner->m_winui.get() != updateImpl ||
                 owner->m_winui->callbackState != callbackState ||
                 owner->m_winui->peerGeneration != generation )
            {
                finish(true, peerKey);
                return;
            }
        }
    }

    finish(true, 0);
}

void wxToolBar::ConvergeToolShortHelpUpdates()
{
    if ( !m_winui || m_winui->closed || !m_winui->callbackState ||
         m_winui->shortHelpSyncActive )
    {
        return;
    }

    wxWinUIToolBarImpl * const updateImpl = m_winui.get();
    const std::shared_ptr<wxWinUIToolBarCallbackState> callbackState =
        updateImpl->callbackState;
    updateImpl->shortHelpSyncActive = true;

    const auto finish =
        [&](bool defer, std::uint64_t retryKey)
        {
            wxToolBar * const owner = callbackState->GetLiveOwner();
            if ( !owner || !owner->m_winui ||
                 owner->m_winui.get() != updateImpl ||
                 owner->m_winui->callbackState != callbackState ||
                 owner->m_winui->closed )
            {
                return;
            }

            wxWinUIToolBarImpl * const liveImpl = owner->m_winui.get();
            if ( retryKey )
            {
                wxWinUIToolPeer * const peer =
                    liveImpl->FindPeerByKey(retryKey);
                if ( !peer || !peer->tool ||
                     !peer->tool->shortHelpProjectionQuarantined )
                {
                    liveImpl->pendingShortHelpKeys.insert(retryKey);
                }
            }
            liveImpl->shortHelpSyncActive = false;

            if ( liveImpl->pendingShortHelpKeys.empty() )
                return;

            if ( !defer || !wxTheApp ||
                 liveImpl->shortHelpSyncScheduled )
            {
                return;
            }

            // The driver budget is armed only by an external model write (or
            // a fresh peer generation), never by a nested setter callback.
            // Thus a callback which changes short help on every projection
            // cannot manufacture an unbounded CallAfter chain.
            if ( !liveImpl->shortHelpDeferredCallbacksRemaining )
            {
                if ( !liveImpl->shortHelpDriverWarningEmitted )
                {
                    liveImpl->shortHelpDriverWarningEmitted = true;
                    wxLogWarning(
                        "WinUI toolbar short-help projection did not "
                        "converge within its bounded retry budget");
                }
                return;
            }
            --liveImpl->shortHelpDeferredCallbacksRemaining;

            liveImpl->shortHelpSyncScheduled = true;
            wxTheApp->CallAfter(
                [callbackState, updateImpl]()
                {
                    wxToolBar *liveOwner =
                        callbackState->GetLiveOwner();
                    if ( !liveOwner || !liveOwner->m_winui ||
                         liveOwner->m_winui.get() != updateImpl ||
                         liveOwner->m_winui->callbackState != callbackState ||
                         liveOwner->m_winui->closed )
                    {
                        return;
                    }

                    liveOwner->m_winui->shortHelpSyncScheduled = false;
                    liveOwner->m_winui->shortHelpRetryCallbackActive = true;
                    liveOwner->ConvergeToolShortHelpUpdates();

                    liveOwner = callbackState->GetLiveOwner();
                    if ( liveOwner && liveOwner->m_winui &&
                         liveOwner->m_winui.get() == updateImpl &&
                         liveOwner->m_winui->callbackState == callbackState )
                    {
                        liveOwner->m_winui->
                            shortHelpRetryCallbackActive = false;
                    }
                });
        };

    const auto queueLatest =
        [&](std::uint64_t peerKey)
        {
            wxToolBar * const owner = callbackState->GetLiveOwner();
            if ( owner && owner->m_winui &&
                 owner->m_winui.get() == updateImpl &&
                 owner->m_winui->callbackState == callbackState &&
                 !owner->m_winui->closed )
            {
                wxWinUIToolPeer * const peer =
                    owner->m_winui->FindPeerByKey(peerKey);
                if ( !peer || !peer->tool ||
                     !peer->tool->shortHelpProjectionQuarantined )
                {
                    owner->m_winui->pendingShortHelpKeys.insert(peerKey);
                }
            }
        };

    const auto recordFailure =
        [&](std::uint64_t generation,
            std::uint64_t peerKey,
            std::uint64_t toolRevision) -> bool
        {
            wxToolBar * const owner =
                callbackState->GetOwnerForModelMutation(generation);
            if ( !owner || !owner->m_winui ||
                 owner->m_winui.get() != updateImpl ||
                 owner->m_winui->callbackState != callbackState ||
                 owner->m_winui->peerGeneration != generation )
            {
                return false;
            }

            wxWinUIToolPeer * const peer =
                owner->m_winui->FindPeerByKey(peerKey);
            if ( !peer || !peer->tool )
                return true;
            if ( peer->tool->shortHelpRevision != toolRevision )
            {
                owner->m_winui->pendingShortHelpKeys.insert(peerKey);
                return true;
            }

            if ( ++peer->tool->shortHelpProjectionFailures >= 3 )
            {
                peer->tool->shortHelpProjectionQuarantined = true;
                wxLogWarning(
                    "WinUI toolbar quarantined a failing short-help peer "
                    "(tool id %d); a new model write or peer rebuild will "
                    "retry it",
                    peer->tool->GetId());
            }
            else
            {
                owner->m_winui->pendingShortHelpKeys.insert(peerKey);
            }
            return true;
        };

    const auto recordSuccess =
        [&](std::uint64_t generation,
            std::uint64_t peerKey,
            std::uint64_t toolRevision)
        {
            wxToolBar * const owner =
                callbackState->GetOwnerForModelMutation(generation);
            if ( !owner || !owner->m_winui ||
                 owner->m_winui.get() != updateImpl ||
                 owner->m_winui->callbackState != callbackState ||
                 owner->m_winui->peerGeneration != generation )
            {
                return;
            }
            wxWinUIToolPeer * const peer =
                owner->m_winui->FindPeerByKey(peerKey);
            if ( peer && peer->tool &&
                 peer->tool->shortHelpRevision == toolRevision )
            {
                peer->tool->shortHelpProjectionFailures = 0;
                peer->tool->shortHelpProjectionQuarantined = false;
            }
        };

    constexpr unsigned MaxSynchronousPasses = 32;
    for ( unsigned pass = 0; pass < MaxSynchronousPasses; ++pass )
    {
        wxToolBar *owner = callbackState->GetLiveOwner();
        if ( !owner || !owner->m_winui ||
             owner->m_winui.get() != updateImpl ||
             owner->m_winui->callbackState != callbackState ||
             owner->m_winui->closed )
        {
            return;
        }

        wxWinUIToolBarImpl * const liveImpl = owner->m_winui.get();
        if ( liveImpl->pendingShortHelpKeys.empty() )
        {
            finish(false, 0);
            return;
        }

        auto pendingIt = liveImpl->pendingShortHelpKeys.upper_bound(
            liveImpl->shortHelpFairnessCursor);
        if ( pendingIt == liveImpl->pendingShortHelpKeys.end() )
            pendingIt = liveImpl->pendingShortHelpKeys.begin();
        const std::uint64_t peerKey = *pendingIt;
        liveImpl->shortHelpFairnessCursor = peerKey;
        liveImpl->pendingShortHelpKeys.erase(pendingIt);
        const std::uint64_t generation = liveImpl->peerGeneration;
        wxWinUIToolPeer * const peer =
            liveImpl->FindPeerByKey(peerKey);
        if ( !peer || !peer->tool )
            continue;
        if ( peer->tool->shortHelpProjectionQuarantined )
            continue;

        const wxString effectiveTip =
            owner->HasFlag(wxTB_NO_TOOLTIPS)
                ? wxString()
                : peer->tool->GetShortHelp();
        const wxString accessibleHelp =
            !peer->tool->GetLongHelp().empty()
                ? peer->tool->GetLongHelp()
                : peer->tool->GetShortHelp();
        const std::uint64_t toolRevision =
            peer->tool->shortHelpRevision;

        // 0: owner/generation lost, 1: this write became stale or the tool
        // disappeared, 2: still current. The one-shot hook is invoked after
        // a real setter attempt (or its deterministic fault seam) and is
        // itself followed by full revalidation.
        const auto afterSetter =
            [&]() -> int
            {
                wxToolBar *liveOwner =
                    callbackState->GetOwnerForModelMutation(generation);
                if ( !liveOwner || !liveOwner->m_winui ||
                     liveOwner->m_winui.get() != updateImpl ||
                     liveOwner->m_winui->callbackState != callbackState ||
                     liveOwner->m_winui->peerGeneration != generation )
                {
                    return 0;
                }

                wxWinUIToolBarImpl * const currentImpl =
                    liveOwner->m_winui.get();
                wxWinUIToolPeer *currentPeer =
                    currentImpl->FindPeerByKey(peerKey);
                if ( !currentPeer || !currentPeer->tool ||
                     currentPeer->tool->shortHelpRevision != toolRevision )
                {
                    return 1;
                }

#ifdef WXWINUI_TEST_SUPPORT
                const wxWinUIToolBarTestAccess::CallbackHook hook =
                    currentImpl->nextShortHelpSetterHookForTesting;
                void * const hookContext =
                    currentImpl->nextShortHelpSetterContextForTesting;
                currentImpl->nextShortHelpSetterHookForTesting = nullptr;
                currentImpl->nextShortHelpSetterContextForTesting = nullptr;
                if ( hook )
                {
                    hook(liveOwner, hookContext);
                    liveOwner =
                        callbackState->GetOwnerForModelMutation(generation);
                    if ( !liveOwner || !liveOwner->m_winui ||
                         liveOwner->m_winui.get() != updateImpl ||
                         liveOwner->m_winui->callbackState != callbackState ||
                         liveOwner->m_winui->peerGeneration != generation )
                    {
                        return 0;
                    }
                }
#endif

                currentPeer =
                    liveOwner->m_winui->FindPeerByKey(peerKey);
                if ( !currentPeer || !currentPeer->tool ||
                     currentPeer->tool->shortHelpRevision != toolRevision )
                {
                    return 1;
                }
                return 2;
            };

#ifdef WXWINUI_TEST_SUPPORT
        // Deterministic per-key failure seam. It models a platform setter
        // rejecting this peer without poisoning unrelated peers.
        if ( peer->tool->shortHelpSetterFaultsForTesting )
        {
            --peer->tool->shortHelpSetterFaultsForTesting;
            const int state = afterSetter();
            if ( state == 0 )
            {
                finish(true, peerKey);
                return;
            }
            if ( state == 1 )
            {
                queueLatest(peerKey);
                continue;
            }
            if ( !recordFailure(generation, peerKey, toolRevision) )
            {
                finish(true, peerKey);
                return;
            }
            continue;
        }
#endif

        if ( peer->tool->IsControl() )
        {
#if wxUSE_TOOLTIPS
            wxWinUIToolBarTool * const controlTool = peer->tool;
            wxToolTip *createdToolTip = nullptr;
            unsigned long long createdToolTipIdentity = 0;
            const wxWinUIControlToolTipApplyResult result =
                wxWinUIApplyControlToolTip(
                    *controlTool, effectiveTip, &createdToolTip,
                    &createdToolTipIdentity);
            if ( result != wxWinUIControlToolTipApplyResult::NoSetter )
            {
                owner =
                    callbackState->GetOwnerForModelMutation(generation);
                if ( !owner || !owner->m_winui ||
                     owner->m_winui.get() != updateImpl ||
                     owner->m_winui->callbackState != callbackState ||
                     owner->m_winui->peerGeneration != generation )
                {
                    finish(true, peerKey);
                    return;
                }

                wxWinUIToolPeer * const currentPeer =
                    owner->m_winui->FindPeerByKey(peerKey);
                if ( !currentPeer || !currentPeer->tool ||
                     !currentPeer->tool->IsControl() )
                {
                    continue;
                }
                wxWinUICompleteControlToolTipApply(
                    *currentPeer->tool, effectiveTip, result,
                    createdToolTip, createdToolTipIdentity);

                const int state = afterSetter();
                if ( state == 0 )
                {
                    finish(true, peerKey);
                    return;
                }
                if ( state == 1 )
                    queueLatest(peerKey);
                else
                    recordSuccess(generation, peerKey, toolRevision);
            }
            else
            {
                recordSuccess(generation, peerKey, toolRevision);
            }
#endif // wxUSE_TOOLTIPS
            continue;
        }

        // Strong projected handles, never a raw peer, cross the setters.
        std::vector<MUX::UIElement> elements;
        elements.reserve(3);
        if ( peer->element )
            elements.push_back(peer->element);
        if ( peer->primaryButton )
            elements.push_back(peer->primaryButton);
        if ( peer->dropdownButton )
            elements.push_back(peer->dropdownButton);

        bool restartLatest = false;
        bool setterFailed = false;
        try
        {
            for ( const MUX::UIElement& element : elements )
            {
                const bool applied =
                    wxWinUISetToolTip(element, effectiveTip, owner);
                const int state = afterSetter();
                if ( state == 0 )
                {
                    finish(true, peerKey);
                    return;
                }
                if ( state == 1 )
                {
                    restartLatest = true;
                    break;
                }
                if ( !applied )
                {
                    setterFailed = true;
                    break;
                }
            }

            if ( !restartLatest && !setterFailed )
            {
                for ( const MUX::UIElement& element : elements )
                {
                    MUXA::AutomationProperties::SetHelpText(
                        element, wxWinUIToHString(accessibleHelp));
                    const int state = afterSetter();
                    if ( state == 0 )
                    {
                        finish(true, peerKey);
                        return;
                    }
                    if ( state == 1 )
                    {
                        restartLatest = true;
                        break;
                    }
                }
            }
        }
        catch ( const winrt::hresult_error& e )
        {
            setterFailed = true;
            wxWinUILogException("WinUI toolbar help-text update", e);
        }

        if ( restartLatest )
        {
            queueLatest(peerKey);
            continue;
        }

        if ( setterFailed )
        {
            if ( !recordFailure(generation, peerKey, toolRevision) )
            {
                finish(true, peerKey);
                return;
            }
            continue;
        }
        recordSuccess(generation, peerKey, toolRevision);
    }

    // Bound synchronous work for toolbars with many controls. The single
    // coalesced callback resumes from the remaining peer keys.
    finish(true, 0);
}

void wxToolBar::SetToolShortHelp(int toolid,
                                 const wxString& helpString)
{
    wxToolBarToolBase * const tool = FindById(toolid);
    if ( !tool || !tool->SetShortHelp(helpString) )
        return;

    if ( !m_winui || m_winui->closed || !m_winui->callbackState )
        return;

    wxWinUIToolBarImpl * const updateImpl = m_winui.get();
    wxWinUIToolBarTool * const winuiTool =
        static_cast<wxWinUIToolBarTool *>(tool);
    ++updateImpl->modelRevision;
    if ( updateImpl->modelRevision == 0 )
        ++updateImpl->modelRevision;
    if ( ++winuiTool->shortHelpRevision == 0 )
        ++winuiTool->shortHelpRevision;
    winuiTool->shortHelpProjectionFailures = 0;
    winuiTool->shortHelpProjectionQuarantined = false;

    // Only an external model write starts a fresh automatic retry budget.
    // Nested setter callbacks are already inside the bounded driver.
    if ( !updateImpl->shortHelpSyncActive &&
         !updateImpl->shortHelpRetryCallbackActive )
    {
        updateImpl->shortHelpDeferredCallbacksRemaining = 2;
        updateImpl->shortHelpDriverWarningEmitted = false;
    }
    updateImpl->pendingShortHelpKeys.insert(winuiTool->peerKey);
    ConvergeToolShortHelpUpdates();
}

void wxToolBar::SetToolLongHelp(int toolid,
                                const wxString& helpString)
{
    wxToolBarToolBase * const tool = FindById(toolid);
    if ( !tool || !tool->SetLongHelp(helpString) )
        return;

    if ( !m_winui || m_winui->closed || !m_winui->callbackState )
        return;

    wxWinUIToolBarImpl * const updateImpl = m_winui.get();
    wxWinUIToolBarTool * const winuiTool =
        static_cast<wxWinUIToolBarTool *>(tool);
    ++updateImpl->modelRevision;
    if ( updateImpl->modelRevision == 0 )
        ++updateImpl->modelRevision;
    if ( ++winuiTool->shortHelpRevision == 0 )
        ++winuiTool->shortHelpRevision;
    winuiTool->shortHelpProjectionFailures = 0;
    winuiTool->shortHelpProjectionQuarantined = false;
    if ( !updateImpl->shortHelpSyncActive &&
         !updateImpl->shortHelpRetryCallbackActive )
    {
        updateImpl->shortHelpDeferredCallbacksRemaining = 2;
        updateImpl->shortHelpDriverWarningEmitted = false;
    }
    updateImpl->pendingShortHelpKeys.insert(winuiTool->peerKey);
    ConvergeToolShortHelpUpdates();
}

void wxToolBar::SetToolNormalBitmap(int toolid,
                                    const wxBitmapBundle& bitmap)
{
    wxToolBarToolBase * const tool = FindById(toolid);
    SetToolNormalBitmapForTool(tool, bitmap);
}

void wxToolBar::DoSetToolNormalBitmapByPos(
    size_t pos,
    const wxBitmapBundle& bitmap)
{
    if ( pos >= GetToolsCount() )
        return;

    SetToolNormalBitmapForTool(
        GetToolByPos(static_cast<int>(pos)), bitmap);
}

void wxToolBar::SetToolNormalBitmapForTool(
    wxToolBarToolBase *tool,
    const wxBitmapBundle& bitmap)
{
    if ( !tool || !tool->IsButton() )
        return;

    wxWinUIToolBarTool * const winuiTool =
        static_cast<wxWinUIToolBarTool *>(tool);
    const std::uint64_t peerKey = winuiTool->peerKey;
    const wxBitmapBundle old = tool->GetNormalBitmapBundle();
    tool->SetNormalBitmap(bitmap);
    if ( m_winui && m_winui->realized && m_winui->callbackState )
    {
        wxWinUIToolBarImpl * const rebuildImpl = m_winui.get();
        const std::shared_ptr<wxWinUIToolBarCallbackState> callbackState =
            rebuildImpl->callbackState;
        const bool rebuilt = RebuildPeer();
        wxToolBar * const liveOwner = callbackState->GetLiveOwner();
        if ( !liveOwner || !liveOwner->m_winui ||
             liveOwner->m_winui.get() != rebuildImpl ||
             liveOwner->m_winui->callbackState != callbackState )
        {
            return;
        }
        wxWinUIToolPeer * const peer =
            liveOwner->m_winui->FindPeerByKey(peerKey);
        wxToolBarToolBase * const liveTool =
            peer ? peer->tool : nullptr;
        if ( !liveTool || liveTool != tool )
            return;
        if ( !rebuilt )
            liveTool->SetNormalBitmap(old);
        liveOwner->InvalidateBestSize();
        return;
    }
    InvalidateBestSize();
}

void wxToolBar::SetToolDisabledBitmap(int toolid,
                                      const wxBitmapBundle& bitmap)
{
    wxToolBarToolBase * const tool = FindById(toolid);
    if ( !tool || !tool->IsButton() )
        return;

    const wxBitmapBundle old = tool->GetDisabledBitmapBundle();
    tool->SetDisabledBitmap(bitmap);
    if ( m_winui && m_winui->realized && m_winui->callbackState )
    {
        wxWinUIToolBarImpl * const rebuildImpl = m_winui.get();
        const std::shared_ptr<wxWinUIToolBarCallbackState> callbackState =
            rebuildImpl->callbackState;
        const bool rebuilt = RebuildPeer();
        wxToolBar * const liveOwner = callbackState->GetLiveOwner();
        if ( !liveOwner || !liveOwner->m_winui ||
             liveOwner->m_winui.get() != rebuildImpl ||
             liveOwner->m_winui->callbackState != callbackState )
        {
            return;
        }
        wxToolBarToolBase * const liveTool =
            liveOwner->FindById(toolid);
        if ( liveTool && liveTool == tool && !rebuilt )
            liveTool->SetDisabledBitmap(old);
    }
}

void wxToolBar::SetMargins(int x, int y)
{
    const wxSize previous = GetToolMargins();
    wxToolBarBase::SetMargins(x, y);
    if ( previous == wxSize(x, y) )
        return;

    if ( !m_winui || !m_winui->callbackState )
    {
        InvalidateBestSize();
        return;
    }
    wxWinUIToolBarImpl * const rebuildImpl = m_winui.get();
    const std::shared_ptr<wxWinUIToolBarCallbackState> callbackState =
        rebuildImpl->callbackState;
    std::uint64_t revision = ++rebuildImpl->layoutRevision;
    if ( revision == 0 )
        revision = ++rebuildImpl->layoutRevision;
    if ( ++rebuildImpl->modelRevision == 0 )
        ++rebuildImpl->modelRevision;

    const bool rebuilt =
        !rebuildImpl->realized || RebuildPeer();
    wxToolBar * const owner = callbackState->GetLiveOwner();
    if ( !owner || !owner->m_winui ||
         owner->m_winui.get() != rebuildImpl ||
         owner->m_winui->callbackState != callbackState )
    {
        return;
    }
    if ( !rebuilt && rebuildImpl->layoutRevision == revision )
    {
        wxToolBarBase::SetMargins(previous.x, previous.y);
        ++rebuildImpl->layoutRevision;
        ++rebuildImpl->modelRevision;
    }
    owner->InvalidateBestSize();
}

void wxToolBar::SetToolPacking(int packing)
{
    const int previous = GetToolPacking();
    wxToolBarBase::SetToolPacking(packing);
    if ( previous == packing )
        return;

    if ( !m_winui || !m_winui->callbackState )
    {
        InvalidateBestSize();
        return;
    }
    wxWinUIToolBarImpl * const rebuildImpl = m_winui.get();
    const std::shared_ptr<wxWinUIToolBarCallbackState> callbackState =
        rebuildImpl->callbackState;
    std::uint64_t revision = ++rebuildImpl->layoutRevision;
    if ( revision == 0 )
        revision = ++rebuildImpl->layoutRevision;
    if ( ++rebuildImpl->modelRevision == 0 )
        ++rebuildImpl->modelRevision;

    const bool rebuilt =
        !rebuildImpl->realized || RebuildPeer();
    wxToolBar * const owner = callbackState->GetLiveOwner();
    if ( !owner || !owner->m_winui ||
         owner->m_winui.get() != rebuildImpl ||
         owner->m_winui->callbackState != callbackState )
    {
        return;
    }
    if ( !rebuilt && rebuildImpl->layoutRevision == revision )
    {
        wxToolBarBase::SetToolPacking(previous);
        ++rebuildImpl->layoutRevision;
        ++rebuildImpl->modelRevision;
    }
    owner->InvalidateBestSize();
}

void wxToolBar::SetToolSeparation(int separation)
{
    const int previous = GetToolSeparation();
    wxToolBarBase::SetToolSeparation(separation);
    if ( previous == separation )
        return;

    if ( !m_winui || !m_winui->callbackState )
    {
        InvalidateBestSize();
        return;
    }
    wxWinUIToolBarImpl * const rebuildImpl = m_winui.get();
    const std::shared_ptr<wxWinUIToolBarCallbackState> callbackState =
        rebuildImpl->callbackState;
    std::uint64_t revision = ++rebuildImpl->layoutRevision;
    if ( revision == 0 )
        revision = ++rebuildImpl->layoutRevision;
    if ( ++rebuildImpl->modelRevision == 0 )
        ++rebuildImpl->modelRevision;

    const bool rebuilt =
        !rebuildImpl->realized || RebuildPeer();
    wxToolBar * const owner = callbackState->GetLiveOwner();
    if ( !owner || !owner->m_winui ||
         owner->m_winui.get() != rebuildImpl ||
         owner->m_winui->callbackState != callbackState )
    {
        return;
    }
    if ( !rebuilt && rebuildImpl->layoutRevision == revision )
    {
        wxToolBarBase::SetToolSeparation(previous);
        ++rebuildImpl->layoutRevision;
        ++rebuildImpl->modelRevision;
    }
    owner->InvalidateBestSize();
}

bool wxToolBar::MSWOnEffectiveLayoutDirectionChanged()
{
    wxWinUIToolBarImpl * const updateImpl = m_winui.get();
    const std::shared_ptr<wxWinUIToolBarCallbackState> callbackState =
        updateImpl ? updateImpl->callbackState : nullptr;

    wxToolBar * const postBaseOwner =
        callbackState ? callbackState->GetLiveOwner() : nullptr;
    if ( !postBaseOwner || !postBaseOwner->m_winui ||
         postBaseOwner->m_winui.get() != updateImpl ||
         postBaseOwner->m_winui->callbackState != callbackState ||
         updateImpl->closed )
    {
        return false;
    }

    if ( ++updateImpl->modelRevision == 0 )
        ++updateImpl->modelRevision;
    const std::uint64_t generation = updateImpl->peerGeneration;
    const MUX::FlowDirection flow =
        wxWinUIToolBarFlowDirection(postBaseOwner->GetLayoutDirection());
    try
    {
        if ( updateImpl->overflowPanel )
            updateImpl->overflowPanel.FlowDirection(flow);

        wxToolBar *owner =
            callbackState->GetOwnerForModelMutation(generation);
        if ( !owner || !owner->m_winui ||
             owner->m_winui.get() != updateImpl ||
             owner->m_winui->callbackState != callbackState ||
             owner->m_winui->peerGeneration != generation )
        {
            return false;
        }

        if ( updateImpl->root )
            updateImpl->root.FlowDirection(flow);

        owner = callbackState->GetOwnerForModelMutation(generation);
        if ( !owner || !owner->m_winui ||
             owner->m_winui.get() != updateImpl ||
             owner->m_winui->callbackState != callbackState ||
             owner->m_winui->peerGeneration != generation )
        {
            return false;
        }

        if ( !owner->IsVertical() )
        {
            const bool rtl =
                owner->GetLayoutDirection() ==
                wxLayout_RightToLeft;
            const size_t count =
                owner->m_winui->peers.size();
            for ( size_t index = 0; index < count; ++index )
            {
                const std::uint64_t peerKey =
                    owner->m_winui->peers[index].tool
                        ? owner->m_winui->peers[index].tool->peerKey
                        : 0;
                const MUX::FrameworkElement element =
                    owner->m_winui->peers[index].element;
                if ( !element )
                    continue;
                MUXC::Grid::SetColumn(
                    element,
                    static_cast<int>(
                        rtl ? count - index : index));

                owner =
                    callbackState->GetOwnerForModelMutation(
                        generation);
                if ( !owner || !owner->m_winui ||
                     owner->m_winui.get() != updateImpl ||
                     owner->m_winui->callbackState != callbackState ||
                     owner->m_winui->peerGeneration != generation ||
                     (peerKey &&
                      !owner->m_winui->FindPeerByKey(peerKey)) )
                {
                    return false;
                }
            }
            MUXC::Grid::SetColumn(
                owner->m_winui->overflowButton,
                rtl ? 0 : static_cast<int>(count));
            owner =
                callbackState->GetOwnerForModelMutation(
                    generation);
            if ( !owner || !owner->m_winui ||
                 owner->m_winui.get() != updateImpl ||
                 owner->m_winui->callbackState != callbackState ||
                 owner->m_winui->peerGeneration != generation )
            {
                return false;
            }
        }

        const double extent = owner->IsVertical()
            ? updateImpl->root.ActualHeight()
            : updateImpl->root.ActualWidth();
        const bool overflowProjected =
            extent <= 0.0 ||
            owner->UpdateOverflow(extent, generation);
        owner = callbackState->GetOwnerForModelMutation(generation);
        return overflowProjected && owner && owner->m_winui &&
               owner->m_winui.get() == updateImpl &&
               owner->m_winui->callbackState == callbackState &&
               owner->m_winui->peerGeneration == generation;
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException(
            "WinUI toolbar layout direction update", e);
        return false;
    }
}

void wxToolBar::OnDPIChanged(wxDPIChangedEvent& event)
{
    event.Skip();
    if ( !m_winui || !m_winui->realized ||
         !m_winui->callbackState )
    {
        return;
    }

    wxWinUIToolBarImpl * const rebuildImpl = m_winui.get();
    const std::shared_ptr<wxWinUIToolBarCallbackState> callbackState =
        rebuildImpl->callbackState;
    if ( !RebuildPeer() )
        return;

    wxToolBar *liveOwner = callbackState->GetLiveOwner();
    if ( !liveOwner || !liveOwner->m_winui ||
         liveOwner->m_winui.get() != rebuildImpl ||
         liveOwner->m_winui->callbackState != callbackState )
    {
        return;
    }
    const std::uint64_t dpiGeneration =
        rebuildImpl->peerGeneration;
    liveOwner->InvalidateBestSize();

    liveOwner = callbackState->GetLiveOwner();
    if ( !liveOwner || !liveOwner->m_winui ||
         liveOwner->m_winui.get() != rebuildImpl ||
         liveOwner->m_winui->callbackState != callbackState ||
         rebuildImpl->peerGeneration != dpiGeneration )
    {
        return;
    }
    wxWindow * const parent = liveOwner->GetParent();
    if ( parent && liveOwner->GetContainingSizer() )
        parent->Layout();
}

// ----------------------------------------------------------------------------
// Embedded controls and geometry
// ----------------------------------------------------------------------------

bool wxToolBar::UpdateOverflow(double availableExtent,
                               std::uint64_t generation)
{
    if ( availableExtent <= 0.0 || !m_winui || !m_winui->root ||
         !m_winui->overflowPanel || !m_winui->overflowButton ||
         !m_winui->callbackState ||
         m_winui->peerGeneration != generation )
    {
        return false;
    }

    wxWinUIToolBarImpl * const updateImpl = m_winui.get();
    const std::shared_ptr<wxWinUIToolBarCallbackState> callbackState =
        updateImpl->callbackState;
    if ( !callbackState->TryBeginOverflowSync() )
        return false;
    auto syncGuard =
        std::make_unique<wxWinUIToolBarOverflowSyncGuard>(
            callbackState);
    auto mutationGuard =
        std::make_unique<wxWinUIToolBarPeerMutationGuard>(
            callbackState);
    const std::uint64_t startModelRevision =
        updateImpl->modelRevision;

    struct OverflowPlan
    {
        std::uint64_t peerKey = 0;
        bool overflow = false;
        bool visible = true;
    };

    struct PeerRollbackState
    {
        std::uint64_t peerKey = 0;
        MUX::UIElement element{ nullptr };
        MUX::Visibility visibility = MUX::Visibility::Visible;
        MUXC::CommandBarLabelPosition labelPosition =
            MUXC::CommandBarLabelPosition::Default;
        bool hasLabelPosition = false;
        MUXC::Orientation radioOrientation =
            MUXC::Orientation::Horizontal;
        MUX::Visibility radioLabelVisibility =
            MUX::Visibility::Visible;
        double radioSpacing = 0.0;
        bool hasRadioPresentation = false;
        bool inOverflow = false;
    };

    const bool vertical = IsVertical();
    const bool showText = HasFlag(wxTB_TEXT);
    std::vector<OverflowPlan> plan(updateImpl->peers.size());
    std::vector<double> extents(updateImpl->peers.size(), 0.0);
    double primaryExtent = 0.0;
    std::vector<MUX::UIElement> originalRootChildren;
    std::vector<MUX::UIElement> originalOverflowChildren;
    std::vector<PeerRollbackState> rollbackPeers;
    MUX::Visibility originalChevronVisibility =
        MUX::Visibility::Collapsed;
    bool originalChevronEnabled = false;
    bool rollbackSnapshotReady = false;
    bool forwardMutationStarted = false;
    bool superseded = false;
#ifdef WXWINUI_TEST_SUPPORT
    unsigned mutationBoundary = 0;
    const unsigned failBoundary =
        updateImpl->failNextOverflowMutationBoundaryForTesting;
    updateImpl->failNextOverflowMutationBoundaryForTesting = 0;
#endif

    const auto getLiveOwner =
        [&]() -> wxToolBar *
        {
            wxToolBar * const owner =
                callbackState->GetOwnerForModelMutation(generation);
            if ( !owner || !owner->m_winui ||
                 owner->m_winui.get() != updateImpl ||
                 owner->m_winui->callbackState != callbackState ||
                 owner->m_winui->peerGeneration != generation )
            {
                return nullptr;
            }
            return owner;
        };

    const auto beforeForwardMutation =
        [&]()
        {
#ifdef WXWINUI_TEST_SUPPORT
            ++mutationBoundary;
            if ( failBoundary != 0 && mutationBoundary == failBoundary )
            {
                throw winrt::hresult_error(
                    E_FAIL,
                    L"Injected WinUI toolbar overflow mutation failure");
            }
#endif
            forwardMutationStarted = true;
        };

    const auto copyChildren =
        [](const auto& children)
        {
            std::vector<MUX::UIElement> result;
            const uint32_t count = children.Size();
            result.reserve(count);
            for ( uint32_t index = 0; index < count; ++index )
                result.push_back(children.GetAt(index));
            return result;
        };

    // Restore the exact XAML collection order and peer metadata captured
    // before the transaction. Fault injection is deliberately disabled here:
    // a test failure exercises the forward path, never sabotages recovery.
    const auto rollback =
        [&]() -> bool
        {
            wxToolBar *owner = getLiveOwner();
            if ( !owner )
                return false;

            auto rootChildren = owner->m_winui->root.Children();
            rootChildren.Clear();
            owner = getLiveOwner();
            if ( !owner )
                return false;

            auto overflowChildren =
                owner->m_winui->overflowPanel.Children();
            overflowChildren.Clear();
            owner = getLiveOwner();
            if ( !owner )
                return false;

            for ( const MUX::UIElement& element : originalRootChildren )
            {
                owner->m_winui->root.Children().Append(element);
                owner = getLiveOwner();
                if ( !owner )
                    return false;
            }
            for ( const MUX::UIElement& element :
                  originalOverflowChildren )
            {
                owner->m_winui->overflowPanel.Children().Append(element);
                owner = getLiveOwner();
                if ( !owner )
                    return false;
            }

            for ( const PeerRollbackState& state : rollbackPeers )
            {
                wxWinUIToolPeer *peer =
                    owner->m_winui->FindPeerByKey(state.peerKey);
                if ( !peer || !peer->element )
                    return false;

                const MUX::FrameworkElement element = peer->element;
                if ( element.Visibility() != state.visibility )
                {
                    element.Visibility(state.visibility);
                    owner = getLiveOwner();
                    if ( !owner )
                        return false;
                    peer = owner->m_winui->FindPeerByKey(state.peerKey);
                    if ( !peer )
                        return false;
                }

                if ( state.hasLabelPosition && peer->primaryButton )
                {
                    if ( const auto button =
                             peer->primaryButton
                                 .try_as<MUXC::AppBarButton>() )
                    {
                        if ( button.LabelPosition() !=
                             state.labelPosition )
                        {
                            button.LabelPosition(state.labelPosition);
                            owner = getLiveOwner();
                            if ( !owner )
                                return false;
                        }
                    }
                    else if ( const auto toggleButton =
                                  peer->primaryButton.try_as<
                                      MUXC::AppBarToggleButton>() )
                    {
                        if ( toggleButton.LabelPosition() !=
                             state.labelPosition )
                        {
                            toggleButton.LabelPosition(
                                state.labelPosition);
                            owner = getLiveOwner();
                            if ( !owner )
                                return false;
                        }
                    }
                }
                else if ( state.hasRadioPresentation &&
                          peer->radioContent &&
                          peer->radioLabel )
                {
                    const MUXC::StackPanel content =
                        peer->radioContent;
                    const MUXC::TextBlock label = peer->radioLabel;
                    if ( content.Orientation() !=
                         state.radioOrientation )
                    {
                        content.Orientation(state.radioOrientation);
                        owner = getLiveOwner();
                        if ( !owner )
                            return false;
                    }
                    if ( content.Spacing() != state.radioSpacing )
                    {
                        content.Spacing(state.radioSpacing);
                        owner = getLiveOwner();
                        if ( !owner )
                            return false;
                    }
                    if ( label.Visibility() !=
                         state.radioLabelVisibility )
                    {
                        label.Visibility(
                            state.radioLabelVisibility);
                        owner = getLiveOwner();
                        if ( !owner )
                            return false;
                    }
                }

                peer = owner->m_winui->FindPeerByKey(state.peerKey);
                if ( !peer )
                    return false;
                peer->inOverflow = state.inOverflow;
            }

            if ( owner->m_winui->overflowButton.Visibility() !=
                 originalChevronVisibility )
            {
                owner->m_winui->overflowButton.Visibility(
                    originalChevronVisibility);
                owner = getLiveOwner();
                if ( !owner )
                    return false;
            }
            if ( owner->m_winui->overflowButton.IsEnabled() !=
                 originalChevronEnabled )
            {
                owner->m_winui->overflowButton.IsEnabled(
                    originalChevronEnabled);
                owner = getLiveOwner();
                if ( !owner )
                    return false;
            }
            return true;
        };

    try
    {
        for ( size_t index = 0; index < updateImpl->peers.size(); ++index )
        {
            wxToolBar *owner = getLiveOwner();
            if ( !owner )
                return false;

            wxWinUIToolPeer& peer = owner->m_winui->peers[index];
            if ( !peer.tool )
                continue;

            plan[index].peerKey = peer.tool->peerKey;
            if ( !peer.tool->IsStretchable() )
            {
                if ( peer.naturalExtent <= 0.0 )
                {
                    const MUX::FrameworkElement element = peer.element;
                    element.Measure(
                        { std::numeric_limits<float>::infinity(),
                          std::numeric_limits<float>::infinity() });
                    owner = getLiveOwner();
                    if ( !owner )
                        return false;

                    wxWinUIToolPeer * const measuredPeer =
                        owner->m_winui->FindPeerByKey(
                            plan[index].peerKey);
                    if ( !measuredPeer || !measuredPeer->element )
                        return false;
                    const WF::Size desired = element.DesiredSize();
                    owner = getLiveOwner();
                    if ( !owner )
                        return false;
                    wxWinUIToolPeer * const currentPeer =
                        owner->m_winui->FindPeerByKey(
                            plan[index].peerKey);
                    if ( !currentPeer )
                        return false;
                    currentPeer->naturalExtent = std::max(
                        static_cast<double>(
                            vertical
                                ? desired.Height
                                : desired.Width),
                        1.0);
                }

                owner = getLiveOwner();
                if ( !owner )
                    return false;
                const wxWinUIToolPeer * const currentPeer =
                    owner->m_winui->FindPeerByKey(
                        plan[index].peerKey);
                if ( !currentPeer )
                    return false;
                extents[index] = currentPeer->naturalExtent;
                primaryExtent += extents[index];
            }
        }

        const double primaryLimit = std::max(
            0.0,
            availableExtent -
                static_cast<double>(wxWINUI_TOOL_OVERFLOW_WIDTH));
        bool hasOverflowAction = false;
        if ( primaryExtent > availableExtent + 0.5 )
        {
            for ( size_t reverse = plan.size(); reverse != 0; --reverse )
            {
                const size_t index = reverse - 1;
                wxToolBar * const owner = getLiveOwner();
                if ( !owner )
                    return false;
                wxWinUIToolPeer& peer =
                    owner->m_winui->peers[index];
                if ( !peer.tool || !peer.overflowEligible )
                    continue;

                plan[index].overflow = true;
                primaryExtent =
                    std::max(0.0, primaryExtent - extents[index]);
                if ( !peer.tool->IsSeparator() )
                    hasOverflowAction = true;

                if ( hasOverflowAction &&
                     primaryExtent <= primaryLimit + 0.5 )
                {
                    break;
                }
            }
        }

        if ( !hasOverflowAction )
        {
            for ( OverflowPlan& item : plan )
                item.overflow = false;
        }

        // Separators are group punctuation, not commands. Keep at most one
        // separator between two visible commands in each surface, and never
        // expose a leading/trailing/duplicate separator in the flyout.
        for ( size_t index = 0; index < plan.size(); ++index )
        {
            wxToolBar * const owner = getLiveOwner();
            if ( !owner )
                return false;
            wxWinUIToolPeer& peer =
                owner->m_winui->peers[index];
            if ( !peer.tool || !peer.tool->IsSeparator() ||
                 peer.tool->IsStretchable() )
            {
                continue;
            }

            const bool surface = plan[index].overflow;
            bool hasBefore = false;
            bool hasAfter = false;
            bool duplicate = false;
            for ( size_t before = index; before != 0; --before )
            {
                const size_t candidate = before - 1;
                const wxWinUIToolPeer& other =
                    owner->m_winui->peers[candidate];
                if ( !other.tool )
                    continue;
                if ( other.tool->IsStretchable() )
                    continue;
                if ( plan[candidate].overflow != surface )
                    continue;
                if ( other.tool->IsSeparator() )
                {
                    duplicate = true;
                    continue;
                }
                hasBefore = true;
                break;
            }
            for ( size_t after = index + 1;
                  after < plan.size(); ++after )
            {
                const wxWinUIToolPeer& other =
                    owner->m_winui->peers[after];
                if ( !other.tool )
                    continue;
                if ( other.tool->IsStretchable() )
                    continue;
                if ( plan[after].overflow != surface )
                    continue;
                if ( other.tool->IsSeparator() )
                    continue;
                hasAfter = true;
                break;
            }
            plan[index].visible =
                hasBefore && hasAfter && !duplicate;
        }

        wxToolBar *owner = getLiveOwner();
        if ( !owner )
            return false;
        if ( owner->m_winui->modelRevision != startModelRevision )
            return false;

        if ( owner->m_winui->overflowFlyout &&
             owner->m_winui->overflowFlyout.IsOpen() )
        {
            beforeForwardMutation();
            owner->m_winui->overflowFlyout.Hide();
            owner = getLiveOwner();
            if ( !owner )
                return false;
        }

        originalRootChildren =
            copyChildren(owner->m_winui->root.Children());
        originalOverflowChildren =
            copyChildren(owner->m_winui->overflowPanel.Children());
        rollbackPeers.reserve(plan.size());
        for ( const OverflowPlan& item : plan )
        {
            if ( !item.peerKey )
                continue;
            const wxWinUIToolPeer * const peer =
                owner->m_winui->FindPeerByKey(item.peerKey);
            if ( !peer || !peer->element )
                return false;

            PeerRollbackState state;
            state.peerKey = item.peerKey;
            state.element = peer->element.as<MUX::UIElement>();
            state.visibility = peer->element.Visibility();
            state.inOverflow = peer->inOverflow;
            if ( const auto button =
                     peer->primaryButton.try_as<MUXC::AppBarButton>() )
            {
                state.labelPosition = button.LabelPosition();
                state.hasLabelPosition = true;
            }
            else if ( const auto toggleButton =
                          peer->primaryButton.try_as<
                              MUXC::AppBarToggleButton>() )
            {
                state.labelPosition = toggleButton.LabelPosition();
                state.hasLabelPosition = true;
            }
            else if ( peer->radioContent && peer->radioLabel )
            {
                state.radioOrientation =
                    peer->radioContent.Orientation();
                state.radioSpacing =
                    peer->radioContent.Spacing();
                state.radioLabelVisibility =
                    peer->radioLabel.Visibility();
                state.hasRadioPresentation = true;
            }
            rollbackPeers.push_back(std::move(state));
        }
        originalChevronVisibility =
            owner->m_winui->overflowButton.Visibility();
        originalChevronEnabled =
            owner->m_winui->overflowButton.IsEnabled();
        rollbackSnapshotReady = true;

#ifdef WXWINUI_TEST_SUPPORT
        const wxWinUIToolBarTestAccess::CallbackHook mutationHook =
            updateImpl->nextOverflowMutationHookForTesting;
        void * const mutationHookContext =
            updateImpl->nextOverflowMutationContextForTesting;
        updateImpl->nextOverflowMutationHookForTesting = nullptr;
        updateImpl->nextOverflowMutationContextForTesting = nullptr;
        bool hookCalled = false;
#endif
        const auto invokeMutationHook =
            [&](wxToolBar *currentOwner) -> wxToolBar *
            {
#ifdef WXWINUI_TEST_SUPPORT
                if ( !hookCalled && mutationHook )
                {
                    hookCalled = true;
                    mutationHook(
                        currentOwner, mutationHookContext);
                }
#else
                wxUnusedVar(currentOwner);
#endif
                return getLiveOwner();
            };
        uint32_t rootPosition = 0;
        uint32_t overflowPosition = 0;

        for ( const OverflowPlan& item : plan )
        {
            owner = getLiveOwner();
            if ( !owner )
                return false;

            wxWinUIToolPeer *peer =
                owner->m_winui->FindPeerByKey(item.peerKey);
            if ( !peer || !peer->element )
                continue;

            const MUX::UIElement element =
                peer->element.as<MUX::UIElement>();
#ifdef WXWINUI_TEST_SUPPORT
            bool changed = false;
#endif

            if ( item.overflow )
            {
                uint32_t index = 0;
                auto rootChildren = owner->m_winui->root.Children();
                if ( rootChildren.IndexOf(element, index) )
                {
                    beforeForwardMutation();
                    rootChildren.RemoveAt(index);
#ifdef WXWINUI_TEST_SUPPORT
                    changed = true;
#endif
                    owner = getLiveOwner();
                    if ( !owner )
                        return false;
                    owner = invokeMutationHook(owner);
                    if ( !owner )
                        return false;
                }

                auto overflowChildren =
                    owner->m_winui->overflowPanel.Children();
                if ( overflowChildren.IndexOf(element, index) &&
                     index != overflowPosition )
                {
                    beforeForwardMutation();
                    overflowChildren.RemoveAt(index);
                    owner = getLiveOwner();
                    if ( !owner )
                        return false;
                    owner = invokeMutationHook(owner);
                    if ( !owner )
                        return false;

                    beforeForwardMutation();
                    owner->m_winui->overflowPanel.Children().InsertAt(
                        overflowPosition, element);
                    owner = getLiveOwner();
                    if ( !owner )
                        return false;
                    owner = invokeMutationHook(owner);
                    if ( !owner )
                        return false;
#ifdef WXWINUI_TEST_SUPPORT
                    changed = true;
#endif
                }
                else if ( !overflowChildren.IndexOf(element, index) )
                {
                    beforeForwardMutation();
                    owner->m_winui->overflowPanel.Children().InsertAt(
                        overflowPosition, element);
                    owner = getLiveOwner();
                    if ( !owner )
                        return false;
#ifdef WXWINUI_TEST_SUPPORT
                    changed = true;
#endif
                }
                ++overflowPosition;
            }
            else
            {
                uint32_t index = 0;
                auto overflowChildren =
                    owner->m_winui->overflowPanel.Children();
                if ( overflowChildren.IndexOf(element, index) )
                {
                    beforeForwardMutation();
                    overflowChildren.RemoveAt(index);
                    owner = getLiveOwner();
                    if ( !owner )
                        return false;
                    owner = invokeMutationHook(owner);
                    if ( !owner )
                        return false;
#ifdef WXWINUI_TEST_SUPPORT
                    changed = true;
#endif
                }

                auto rootChildren = owner->m_winui->root.Children();
                if ( rootChildren.IndexOf(element, index) &&
                     index != rootPosition )
                {
                    beforeForwardMutation();
                    rootChildren.RemoveAt(index);
                    owner = getLiveOwner();
                    if ( !owner )
                        return false;
                    owner = invokeMutationHook(owner);
                    if ( !owner )
                        return false;

                    beforeForwardMutation();
                    owner->m_winui->root.Children().InsertAt(
                        rootPosition, element);
                    owner = getLiveOwner();
                    if ( !owner )
                        return false;
#ifdef WXWINUI_TEST_SUPPORT
                    changed = true;
#endif
                }
                else if ( !rootChildren.IndexOf(element, index) )
                {
                    beforeForwardMutation();
                    owner->m_winui->root.Children().InsertAt(
                        rootPosition, element);
                    owner = getLiveOwner();
                    if ( !owner )
                        return false;
#ifdef WXWINUI_TEST_SUPPORT
                    changed = true;
#endif
                }
                ++rootPosition;
            }

            peer = owner->m_winui->FindPeerByKey(item.peerKey);
            if ( !peer || !peer->element )
                return false;
            const auto labelPosition =
                item.overflow || showText
                    ? MUXC::CommandBarLabelPosition::Default
                    : MUXC::CommandBarLabelPosition::Collapsed;
            if ( const auto button =
                     peer->primaryButton.try_as<MUXC::AppBarButton>() )
            {
                if ( button.LabelPosition() != labelPosition )
                {
                    beforeForwardMutation();
                    button.LabelPosition(labelPosition);
                    owner = getLiveOwner();
                    if ( !owner )
                        return false;
#ifdef WXWINUI_TEST_SUPPORT
                    changed = true;
#endif
                }
            }
            else if ( const auto toggleButton =
                          peer->primaryButton.try_as<
                              MUXC::AppBarToggleButton>() )
            {
                if ( toggleButton.LabelPosition() != labelPosition )
                {
                    beforeForwardMutation();
                    toggleButton.LabelPosition(labelPosition);
                    owner = getLiveOwner();
                    if ( !owner )
                        return false;
#ifdef WXWINUI_TEST_SUPPORT
                    changed = true;
#endif
                }
            }
            else if ( peer->radioContent && peer->radioLabel )
            {
                const MUXC::Orientation orientation =
                    item.overflow ||
                    (showText && HasFlag(wxTB_HORZ_LAYOUT))
                        ? MUXC::Orientation::Horizontal
                        : MUXC::Orientation::Vertical;
                const double spacing =
                    orientation == MUXC::Orientation::Horizontal
                        ? 6.0
                        : 2.0;
                const MUX::Visibility labelVisibility =
                    item.overflow || showText
                        ? MUX::Visibility::Visible
                        : MUX::Visibility::Collapsed;
                const MUXC::StackPanel content =
                    peer->radioContent;
                const MUXC::TextBlock label = peer->radioLabel;
                if ( content.Orientation() != orientation )
                {
                    beforeForwardMutation();
                    content.Orientation(orientation);
                    owner = getLiveOwner();
                    if ( !owner )
                        return false;
#ifdef WXWINUI_TEST_SUPPORT
                    changed = true;
#endif
                }
                if ( content.Spacing() != spacing )
                {
                    beforeForwardMutation();
                    content.Spacing(spacing);
                    owner = getLiveOwner();
                    if ( !owner )
                        return false;
#ifdef WXWINUI_TEST_SUPPORT
                    changed = true;
#endif
                }
                if ( label.Visibility() != labelVisibility )
                {
                    beforeForwardMutation();
                    label.Visibility(labelVisibility);
                    owner = getLiveOwner();
                    if ( !owner )
                        return false;
#ifdef WXWINUI_TEST_SUPPORT
                    changed = true;
#endif
                }
            }

            peer = owner->m_winui->FindPeerByKey(item.peerKey);
            if ( !peer || !peer->element )
                return false;
            const MUX::Visibility visibility =
                item.visible
                    ? MUX::Visibility::Visible
                    : MUX::Visibility::Collapsed;
            const MUX::FrameworkElement peerElement = peer->element;
            if ( peerElement.Visibility() != visibility )
            {
                beforeForwardMutation();
                peerElement.Visibility(visibility);
                owner = getLiveOwner();
                if ( !owner )
                    return false;
#ifdef WXWINUI_TEST_SUPPORT
                changed = true;
#endif
            }

            peer = owner->m_winui->FindPeerByKey(item.peerKey);
            if ( !peer )
                return false;

#ifdef WXWINUI_TEST_SUPPORT
            // This seam is deliberately after one real parent/visibility
            // mutation. It proves that destruction or a nested resize at the
            // exact reparent boundary cannot leave a late callback alive.
            if ( changed && !hookCalled && mutationHook )
            {
                hookCalled = true;
                mutationHook(owner, mutationHookContext);
                owner = getLiveOwner();
                if ( !owner )
                    return false;
            }
#endif
        }

        owner = getLiveOwner();
        if ( !owner )
            return false;

        const MUX::Visibility chevronVisibility =
            hasOverflowAction
                ? MUX::Visibility::Visible
                : MUX::Visibility::Collapsed;
        {
            const MUX::UIElement chevron =
                owner->m_winui->overflowButton.as<MUX::UIElement>();
            uint32_t chevronIndex = 0;
            auto rootChildren = owner->m_winui->root.Children();
            if ( rootChildren.IndexOf(chevron, chevronIndex) &&
                 chevronIndex != rootPosition )
            {
                beforeForwardMutation();
                rootChildren.RemoveAt(chevronIndex);
                owner = getLiveOwner();
                if ( !owner )
                    return false;

                beforeForwardMutation();
                owner->m_winui->root.Children().InsertAt(
                    rootPosition, chevron);
                owner = getLiveOwner();
                if ( !owner )
                    return false;
            }
        }

        if ( owner->m_winui->overflowButton.Visibility() !=
             chevronVisibility )
        {
            beforeForwardMutation();
            owner->m_winui->overflowButton.Visibility(
                chevronVisibility);
            owner = getLiveOwner();
            if ( !owner )
                return false;
        }
        const bool chevronEnabled = owner->IsEnabled();
        if ( owner->m_winui->overflowButton.IsEnabled() !=
             chevronEnabled )
        {
            beforeForwardMutation();
            owner->m_winui->overflowButton.IsEnabled(
                chevronEnabled);
            owner = getLiveOwner();
            if ( !owner )
                return false;
        }

        if ( owner->m_winui->modelRevision != startModelRevision )
        {
            superseded = true;
            throw winrt::hresult_error(
                E_ABORT,
                L"WinUI toolbar overflow plan superseded");
        }

        // All throwing XAML operations have committed. Publish the matching
        // non-throwing wx peer metadata in one pass so event routing never
        // observes a partially updated logical overflow model.
        for ( const OverflowPlan& item : plan )
        {
            if ( !item.peerKey )
                continue;
            wxWinUIToolPeer * const peer =
                owner->m_winui->FindPeerByKey(item.peerKey);
            if ( !peer )
                return false;
            peer->inOverflow = item.overflow;
        }

        // Hiding the popup is a terminal best-effort cleanup. It cannot make
        // the committed parentage/metadata divergent, and a collapsed anchor
        // prevents the stale flyout from being reopened by the user.
        if ( !hasOverflowAction && owner->m_winui->overflowFlyout )
        {
            try
            {
                owner->m_winui->overflowFlyout.Hide();
            }
            catch ( const winrt::hresult_error& e )
            {
                wxWinUILogException(
                    "WinUI toolbar overflow flyout hide", e);
            }
        }
        return true;
    }
    catch ( const winrt::hresult_error& e )
    {
        bool rollbackComplete = true;
        if ( rollbackSnapshotReady && forwardMutationStarted &&
             getLiveOwner() )
        {
            try
            {
                rollbackComplete = rollback();
                if ( !rollbackComplete && getLiveOwner() )
                {
                    wxLogError(
                        "WinUI toolbar overflow rollback lost its "
                        "generation");
                }
            }
            catch ( const winrt::hresult_error& rollbackError )
            {
                rollbackComplete = false;
                wxWinUILogException(
                    "WinUI toolbar overflow rollback",
                    rollbackError);
            }
        }
        if ( !rollbackComplete && getLiveOwner() )
        {
            // Recovery may rebuild and therefore change peer identity, but
            // only after exact rollback itself proved impossible. Release
            // both guards first so the replacement generation can publish.
            mutationGuard.reset();
            syncGuard.reset();
            wxToolBar * const owner = callbackState->GetLiveOwner();
            if ( owner && owner->m_winui &&
                 owner->m_winui.get() == updateImpl &&
                 owner->m_winui->callbackState == callbackState &&
                 owner->m_winui->peerGeneration == generation &&
                 !owner->RebuildPeer() )
            {
                wxLogError(
                    "WinUI toolbar overflow recovery rebuild failed");
            }
        }
        else if ( superseded && rollbackComplete )
        {
            // One bounded retry publishes the last model writer that won
            // during a setter boundary. The one-shot mutation hook has
            // already been consumed, so this cannot form an unbounded loop.
            mutationGuard.reset();
            syncGuard.reset();
            wxToolBar * const owner =
                callbackState->GetOwnerForModelMutation(generation);
            if ( owner && owner->m_winui &&
                 owner->m_winui.get() == updateImpl &&
                 owner->m_winui->callbackState == callbackState &&
                 owner->m_winui->peerGeneration == generation )
            {
                return owner->UpdateOverflow(
                    availableExtent, generation);
            }
        }
        if ( !superseded )
            wxWinUILogException("WinUI toolbar overflow layout", e);
        return false;
    }
}

void wxToolBar::SyncControlTools(std::uint64_t generation)
{
    if ( !m_winui || m_winui->peerGeneration != generation ||
         !m_winui->root || !m_winui->callbackState )
    {
        return;
    }

    wxWinUIToolBarImpl * const syncImpl = m_winui.get();
    const std::shared_ptr<wxWinUIToolBarCallbackState> callbackState =
        m_winui->callbackState;
    const void * const rootIdentity =
        winrt::get_abi(m_winui->root);
    wxToolBar * const toolbarIdentity = this;
    const auto getCurrentOwner =
        [callbackState,
         generation,
         syncImpl,
         rootIdentity,
         toolbarIdentity]() -> wxToolBar *
        {
            wxToolBar * const owner =
                callbackState->GetOwner(generation);
            if ( owner != toolbarIdentity || !owner->m_winui ||
                 owner->m_winui.get() != syncImpl ||
                 owner->m_winui->callbackState != callbackState ||
                 owner->m_winui->peerGeneration != generation ||
                 winrt::get_abi(owner->m_winui->root) != rootIdentity )
            {
                return nullptr;
            }
            return owner;
        };
    if ( !getCurrentOwner() )
        return;

    if ( !callbackState->TryBeginControlSync() )
        return;
    const wxWinUIToolBarControlSyncGuard syncGuard(callbackState);
    wxUnusedVar(syncGuard);

    if ( !getCurrentOwner() )
        return;

    std::vector<std::uint64_t> controlKeys;
    controlKeys.reserve(syncImpl->peers.size());
    for ( const wxWinUIToolPeer& peer : syncImpl->peers )
    {
        if ( peer.tool )
            controlKeys.push_back(peer.tool->peerKey);
    }

    try
    {
        for ( const std::uint64_t peerKey : controlKeys )
        {
            wxToolBar *liveOwner = getCurrentOwner();
            if ( !liveOwner )
            {
                return;
            }

            wxWinUIToolPeer *peer =
                liveOwner->m_winui->FindPeerByKey(peerKey);
            if ( !peer || !peer->tool )
                continue;

            const bool isControl = peer->tool->IsControl();
            liveOwner = getCurrentOwner();
            peer = liveOwner
                ? liveOwner->m_winui->FindPeerByKey(peerKey)
                : nullptr;
            if ( !liveOwner )
                return;
            if ( !isControl || !peer || !peer->tool ||
                 !peer->tool->IsControl() ||
                 !peer->controlPlaceholder )
            {
                continue;
            }

            const MUX::FrameworkElement placeholder =
                peer->controlPlaceholder;
            const void * const placeholderIdentity =
                winrt::get_abi(placeholder);
            wxControl * const controlIdentity =
                peer->tool->GetLiveControl();
            if ( !controlIdentity ||
                 peer->tool->controlHiddenByToolbar )
            {
                continue;
            }
            const wxWeakRef<wxWindow> controlLifetime(controlIdentity);

            const auto getCurrentControlPeer =
                [&getCurrentOwner,
                 peerKey,
                 placeholderIdentity,
                 controlIdentity,
                 &controlLifetime]() -> wxWinUIToolPeer *
                {
                    wxToolBar * const owner = getCurrentOwner();
                    wxWindow * const liveControl =
                        controlLifetime.get();
                    if ( !owner || liveControl != controlIdentity )
                        return nullptr;

                    wxWinUIToolPeer * const currentPeer =
                        owner->m_winui->FindPeerByKey(peerKey);
                    if ( !currentPeer || !currentPeer->tool ||
                         !currentPeer->tool->IsControl() ||
                         currentPeer->tool->controlHiddenByToolbar ||
                         currentPeer->tool->GetLiveControl() !=
                             controlIdentity ||
                         winrt::get_abi(
                             currentPeer->controlPlaceholder) !=
                             placeholderIdentity )
                    {
                        return nullptr;
                    }
                    return currentPeer;
                };
            peer = getCurrentControlPeer();
            if ( !peer )
                continue;

            const bool showPending = peer->tool->controlShowPending;
            peer->tool->controlShowPending = false;
            const wxRect bounds = wxWinUIGetToolBounds(
                liveOwner->m_winui->root,
                placeholder,
                liveOwner,
                [&getCurrentControlPeer]()
                {
                    return getCurrentControlPeer() != nullptr;
                });

            liveOwner = getCurrentOwner();
            peer = getCurrentControlPeer();
            if ( !liveOwner )
                return;
            if ( !peer )
                continue;

            if ( bounds.width > 0 && bounds.height > 0 )
            {
                wxControl * const control =
                    peer->tool->GetLiveControl();
                if ( !control )
                    continue;

                // SetSize() may synchronously mutate or destroy either side.
                // Do not retain the peer/control across this final call.
                control->SetSize(bounds);
            }

            // SetSize() is an application re-entrancy point. Re-resolve
            // before a pending Show(), and make Show() the final operation
            // for this iteration because it can re-enter too.
            if ( showPending )
            {
                wxToolBar * const ownerAfterSize =
                    getCurrentOwner();
                if ( !ownerAfterSize )
                {
                    return;
                }

                wxWinUIToolPeer * const peerAfterSize =
                    getCurrentControlPeer();
                if ( !peerAfterSize )
                {
                    continue;
                }
                if ( wxControl * const controlAfterSize =
                         peerAfterSize->tool->GetLiveControl() )
                {
                    controlAfterSize->Show();
                }
            }
        }
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI toolbar control layout", e);
    }
}

wxToolBarToolBase *wxToolBar::FindToolForPosition(wxCoord x,
                                                  wxCoord y) const
{
    if ( !m_winui || !m_winui->root || !m_winui->callbackState ||
         x < 0 || y < 0 )
        return nullptr;

    wxWinUIToolBarImpl * const findImpl = m_winui.get();
    const std::shared_ptr<wxWinUIToolBarCallbackState> callbackState =
        m_winui->callbackState;
    const std::uint64_t generation = m_winui->peerGeneration;
    const void * const rootIdentity =
        winrt::get_abi(m_winui->root);
    const wxToolBar * const toolbarIdentity = this;
    const auto getCurrentOwner =
        [callbackState,
         generation,
         findImpl,
         rootIdentity,
         toolbarIdentity]() -> const wxToolBar *
        {
            wxToolBar * const owner =
                callbackState->GetOwner(generation);
            if ( owner != toolbarIdentity || !owner->m_winui ||
                 owner->m_winui.get() != findImpl ||
                 owner->m_winui->callbackState != callbackState ||
                 owner->m_winui->peerGeneration != generation ||
                 winrt::get_abi(owner->m_winui->root) != rootIdentity )
            {
                return nullptr;
            }
            return owner;
        };
    if ( !getCurrentOwner() )
        return nullptr;

    // Never range-iterate peer references across XAML/mapper calls: a
    // synchronous rebuild can replace the vector. Stable, globally unique
    // peer keys are the only state carried across those boundaries.
    std::vector<std::uint64_t> peerKeys;
    peerKeys.reserve(findImpl->peers.size());
    for ( const wxWinUIToolPeer& peer : findImpl->peers )
    {
        if ( peer.tool )
            peerKeys.push_back(peer.tool->peerKey);
    }

    try
    {
        for ( const std::uint64_t peerKey : peerKeys )
        {
            const wxToolBar *owner = getCurrentOwner();
            if ( !owner )
                return nullptr;

            const wxWinUIToolPeer *peer =
                owner->m_winui->FindPeerByKey(peerKey);
            if ( !peer || !peer->tool )
                continue;

            const bool separator = peer->tool->IsSeparator();
            owner = getCurrentOwner();
            peer = owner
                ? owner->m_winui->FindPeerByKey(peerKey)
                : nullptr;
            if ( !owner )
                return nullptr;
            if ( separator || !peer || !peer->tool ||
                 peer->inOverflow || !peer->element )
            {
                continue;
            }

            const MUX::FrameworkElement element = peer->element;
            const void * const peerIdentity =
                winrt::get_abi(element);
            const auto getCurrentPeer =
                [&getCurrentOwner,
                 peerKey,
                 peerIdentity]() -> const wxWinUIToolPeer *
                {
                    const wxToolBar * const currentOwner =
                        getCurrentOwner();
                    if ( !currentOwner )
                        return nullptr;

                    const wxWinUIToolPeer * const currentPeer =
                        currentOwner->m_winui->FindPeerByKey(peerKey);
                    if ( !currentPeer || !currentPeer->tool ||
                         currentPeer->inOverflow ||
                         winrt::get_abi(currentPeer->element) !=
                             peerIdentity )
                    {
                        return nullptr;
                    }
                    return currentPeer;
                };

            try
            {
                const wxRect bounds =
                    wxWinUIGetToolBounds(
                        owner->m_winui->root,
                        element,
                        owner,
                        [&getCurrentPeer]()
                        {
                            return getCurrentPeer() != nullptr;
                        });

                owner = getCurrentOwner();
                peer = getCurrentPeer();
                if ( !owner )
                    return nullptr;
                if ( !peer )
                    continue;
                if ( bounds.Contains(x, y) )
                    return peer->tool;
            }
            catch ( const winrt::hresult_error& )
            {
                // One detached/faulted peer must not make later primary
                // commands disappear from hit testing.
            }
        }
    }
    catch ( const winrt::hresult_error& )
    {
    }
    return nullptr;
}

wxSize wxToolBar::DoGetBestSize() const
{
    const bool vertical = IsVertical();
    const bool showText = HasFlag(wxTB_TEXT);
    const bool horizontalText =
        showText && HasFlag(wxTB_HORZ_LAYOUT);
    const bool showIcons = !HasFlag(wxTB_NOICONS);
    const int defaultThickness = FromDIP(
        showText && !horizontalText
            ? wxWINUI_TOOL_HEIGHT_LABEL_BELOW
            : wxWINUI_TOOL_HEIGHT);
    const int separatorExtent =
        GetToolSeparation() > 0
            ? GetToolSeparation()
            : FromDIP(wxWINUI_TOOL_SEPARATOR_WIDTH);
    const int packing = std::max(GetToolPacking(), 0);

    int length = 0;
    int thickness = defaultThickness;
    for ( wxToolBarToolsList::compatibility_iterator node = m_tools.GetFirst();
          node;
          node = node->GetNext() )
    {
        const wxToolBarToolBase * const tool = node->GetData();
        if ( tool->IsStretchable() )
            continue;

        wxSize itemSize;
        if ( tool->IsSeparator() )
        {
            itemSize = vertical
                ? wxSize(defaultThickness,
                         separatorExtent)
                : wxSize(separatorExtent,
                         defaultThickness);
        }
        else if ( tool->IsControl() &&
                  static_cast<const wxWinUIToolBarTool*>(tool)->
                      GetLiveControl() )
        {
            itemSize =
                static_cast<const wxWinUIToolBarTool*>(tool)->
                    GetLiveControl()->GetBestSize();
            if ( !tool->GetLabel().empty() )
            {
                const wxSize labelSize = GetTextExtent(
                    wxWinUIRemoveMnemonics(tool->GetLabel()));
                if ( horizontalText )
                {
                    itemSize.x += labelSize.x + FromDIP(6);
                    itemSize.y = std::max(itemSize.y, labelSize.y);
                }
                else
                {
                    itemSize.x = std::max(itemSize.x, labelSize.x);
                    itemSize.y += labelSize.y + FromDIP(4);
                }
            }
        }
        else
        {
            int itemWidth = FromDIP(wxWINUI_TOOL_MIN_WIDTH);
            if ( showText )
            {
                const int textWidth = GetTextExtent(
                    wxWinUIRemoveMnemonics(tool->GetLabel())).x;
                if ( horizontalText )
                {
                    itemWidth = std::max(
                        itemWidth,
                        textWidth +
                        (showIcons ? GetToolBitmapSize().x : 0) +
                        FromDIP(24));
                }
                else if ( !showIcons )
                {
                    itemWidth =
                        std::max(itemWidth, textWidth + FromDIP(20));
                }
            }
            if ( tool->GetKind() == wxITEM_DROPDOWN )
                itemWidth += FromDIP(wxWINUI_TOOL_DROPDOWN_WIDTH);
            itemSize = wxSize(itemWidth, defaultThickness);
        }

        if ( vertical )
        {
            length += std::max(itemSize.y, 1) + packing;
            thickness = std::max(thickness, itemSize.x);
        }
        else
        {
            length += std::max(itemSize.x, 1) + packing;
            thickness = std::max(thickness, itemSize.y);
        }
    }

    if ( length == 0 )
        length = FromDIP(wxWINUI_TOOL_MIN_WIDTH);
    const wxSize margins = GetToolMargins();
    length += 2 * std::max(
        vertical ? margins.y : margins.x, 0);
    thickness += 2 * std::max(
        vertical ? margins.x : margins.y, 0);
    return vertical ? wxSize(thickness, length)
                    : wxSize(length, thickness);
}

// ----------------------------------------------------------------------------
// Deterministic test seams
// ----------------------------------------------------------------------------

#ifdef WXWINUI_TEST_SUPPORT

bool wxWinUIToolBarTestAccess::InvokeTool(
    wxToolBar& toolbar, int toolid, bool dropdownPart)
{
    wxToolBarToolBase * const tool = toolbar.FindById(toolid);
    if ( !tool || !toolbar.m_winui )
        return false;
    const wxWinUIToolPeer * const peer = toolbar.m_winui->FindPeerByTool(tool);
    if ( !peer || !tool->IsEnabled() || !toolbar.IsEnabled() )
        return false;

    if ( dropdownPart )
    {
        if ( !tool->IsButton() ||
             tool->GetKind() != wxITEM_DROPDOWN ||
             !peer->dropdownButton )
        {
            return false;
        }
        toolbar.OnDropdownClicked(
            static_cast<wxWinUIToolBarTool *>(tool)->peerKey,
            toolbar.m_winui->peerGeneration);
    }
    else
    {
        if ( !tool->IsButton() || !peer->primaryButton )
            return false;
        toolbar.OnToolClicked(
            static_cast<wxWinUIToolBarTool *>(tool)->peerKey,
            toolbar.m_winui->peerGeneration);
    }
    return true;
}

bool wxWinUIToolBarTestAccess::IsRootLoaded(const wxToolBar& toolbar)
{
    if ( !toolbar.m_winui || !toolbar.m_winui->callbackState || !toolbar.m_winui->root )
        return false;

    wxWinUIToolBarImpl * const expectedImpl = toolbar.m_winui.get();
    const std::shared_ptr<wxWinUIToolBarCallbackState> callbackState =
        toolbar.m_winui->callbackState;
    const std::uint64_t generation = toolbar.m_winui->peerGeneration;
    const MUXC::Grid root = toolbar.m_winui->root;
    const void * const rootIdentity = winrt::get_abi(root);
    const wxToolBar * const toolbarIdentity = &toolbar;
    const auto isCurrent =
        [callbackState,
         generation,
         expectedImpl,
         rootIdentity,
         toolbarIdentity]()
        {
            wxToolBar * const owner =
                callbackState->GetOwner(generation);
            return owner == toolbarIdentity && owner->m_winui &&
                   owner->m_winui.get() == expectedImpl &&
                   owner->m_winui->callbackState == callbackState &&
                   owner->m_winui->peerGeneration == generation &&
                   winrt::get_abi(owner->m_winui->root) ==
                       rootIdentity;
        };

    try
    {
        return isCurrent() && root.IsLoaded() && root.XamlRoot() &&
               isCurrent();
    }
    catch ( const winrt::hresult_error& )
    {
        return false;
    }
}

bool wxWinUIToolBarTestAccess::IsOverflowChevronReady(const wxToolBar& toolbar)
{
    if ( !toolbar.m_winui || !toolbar.m_winui->callbackState ||
         !toolbar.m_winui->overflowButton )
    {
        return false;
    }

    wxWinUIToolBarImpl * const expectedImpl = toolbar.m_winui.get();
    const std::shared_ptr<wxWinUIToolBarCallbackState> callbackState =
        toolbar.m_winui->callbackState;
    const std::uint64_t generation = toolbar.m_winui->peerGeneration;
    const MUXC::Button overflowButton = toolbar.m_winui->overflowButton;
    const void * const overflowButtonIdentity =
        winrt::get_abi(overflowButton);
    const wxToolBar * const toolbarIdentity = &toolbar;
    const auto isCurrent =
        [callbackState,
         generation,
         expectedImpl,
         overflowButtonIdentity,
         toolbarIdentity]()
        {
            wxToolBar * const owner =
                callbackState->GetOwner(generation);
            return owner == toolbarIdentity && owner->m_winui &&
                   owner->m_winui.get() == expectedImpl &&
                   owner->m_winui->callbackState == callbackState &&
                   owner->m_winui->peerGeneration == generation &&
                   winrt::get_abi(owner->m_winui->overflowButton) ==
                       overflowButtonIdentity;
        };

    try
    {
        return isCurrent() &&
               overflowButton.Visibility() ==
                   MUX::Visibility::Visible &&
               overflowButton.IsEnabled() &&
               overflowButton.IsLoaded() &&
               overflowButton.XamlRoot() &&
               isCurrent();
    }
    catch ( const winrt::hresult_error& )
    {
        return false;
    }
}

bool wxWinUIToolBarTestAccess::RequestOpenOverflow(wxToolBar& toolbar)
{
    if ( !toolbar.m_winui || !toolbar.m_winui->callbackState ||
         !toolbar.m_winui->overflowButton ||
         !toolbar.m_winui->overflowFlyout ||
         !toolbar.m_winui->overflowPanel ||
         !toolbar.IsEnabled() )
    {
        return false;
    }

    wxWinUIToolBarImpl * const invokeImpl = toolbar.m_winui.get();
    const std::shared_ptr<wxWinUIToolBarCallbackState> callbackState =
        toolbar.m_winui->callbackState;
    const std::uint64_t generation = toolbar.m_winui->peerGeneration;
    const MUXC::Button overflowButton = toolbar.m_winui->overflowButton;
    const void * const overflowButtonIdentity =
        winrt::get_abi(overflowButton);
    const void * const overflowFlyoutIdentity =
        winrt::get_abi(toolbar.m_winui->overflowFlyout);
    const void * const overflowPanelIdentity =
        winrt::get_abi(toolbar.m_winui->overflowPanel);

    try
    {
        if ( overflowButton.Visibility() != MUX::Visibility::Visible ||
             !overflowButton.IsEnabled() ||
             !overflowButton.IsLoaded() ||
             !overflowButton.XamlRoot() )
        {
            return false;
        }

        const MUXAP::AutomationPeer automationPeer =
            MUXAP::FrameworkElementAutomationPeer::CreatePeerForElement(
                overflowButton);
        if ( !automationPeer )
            return false;

        const MUXAPR::IInvokeProvider invokeProvider =
            automationPeer
                .GetPattern(MUXAP::PatternInterface::Invoke)
                .try_as<MUXAPR::IInvokeProvider>();
        if ( !invokeProvider )
            return false;

        // Use the peer owned by the live XAML tree, exactly as an external UIA
        // client does. Constructing a second ButtonAutomationPeer directly
        // bypasses that association and may reject Invoke with E_INVALIDARG.
        invokeProvider.Invoke();

        // Invoke is a WinRT/re-entrancy boundary. Report acceptance only if
        // the exact toolbar generation still owns the loaded chevron and its
        // attached overflow surface.
        wxToolBar * const owner =
            callbackState->GetOwner(generation);
        return owner == &toolbar && owner->m_winui &&
               owner->m_winui.get() == invokeImpl &&
               owner->m_winui->callbackState == callbackState &&
               owner->m_winui->peerGeneration == generation &&
               winrt::get_abi(owner->m_winui->overflowButton) ==
                   overflowButtonIdentity &&
               winrt::get_abi(owner->m_winui->overflowFlyout) ==
                   overflowFlyoutIdentity &&
               winrt::get_abi(owner->m_winui->overflowPanel) ==
                   overflowPanelIdentity;
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException(
            "WinUI toolbar overflow chevron automation invoke", e);
        return false;
    }
}

bool wxWinUIToolBarTestAccess::IsOverflowToolLoaded(
    const wxToolBar& toolbar, int toolid,
    bool dropdownPart)
{
    wxToolBarToolBase * const tool =
        const_cast<wxToolBar *>(&toolbar)->FindById(toolid);
    if ( !tool || !toolbar.m_winui || !toolbar.m_winui->callbackState ||
         !toolbar.m_winui->overflowButton ||
         !toolbar.m_winui->overflowFlyout ||
         !toolbar.m_winui->overflowPanel )
    {
        return false;
    }

    const wxWinUIToolPeer * const initialPeer =
        toolbar.m_winui->FindPeerByTool(tool);
    if ( !initialPeer || !initialPeer->inOverflow )
        return false;

    const MUX::FrameworkElement element =
        dropdownPart
            ? initialPeer->dropdownButton
                  .try_as<MUX::FrameworkElement>()
            : initialPeer->element;
    if ( !element )
        return false;

    wxWinUIToolBarImpl * const expectedImpl = toolbar.m_winui.get();
    const std::shared_ptr<wxWinUIToolBarCallbackState> callbackState =
        toolbar.m_winui->callbackState;
    const std::uint64_t generation = toolbar.m_winui->peerGeneration;
    const std::uint64_t peerKey =
        static_cast<wxWinUIToolBarTool *>(tool)->peerKey;
    const void * const elementIdentity = winrt::get_abi(element);
    const void * const overflowButtonIdentity =
        winrt::get_abi(toolbar.m_winui->overflowButton);
    const void * const overflowFlyoutIdentity =
        winrt::get_abi(toolbar.m_winui->overflowFlyout);
    const void * const overflowPanelIdentity =
        winrt::get_abi(toolbar.m_winui->overflowPanel);
    const wxToolBar * const toolbarIdentity = &toolbar;

    const auto getCurrentPeer =
        [callbackState,
         generation,
         peerKey,
         expectedImpl,
         elementIdentity,
         overflowButtonIdentity,
         overflowFlyoutIdentity,
         overflowPanelIdentity,
         dropdownPart,
         toolbarIdentity]() -> const wxWinUIToolPeer *
        {
            wxToolBar * const owner =
                callbackState->GetOwner(generation);
            if ( owner != toolbarIdentity || !owner->m_winui ||
                 owner->m_winui.get() != expectedImpl ||
                 owner->m_winui->callbackState != callbackState ||
                 owner->m_winui->peerGeneration != generation ||
                 winrt::get_abi(owner->m_winui->overflowButton) !=
                     overflowButtonIdentity ||
                 winrt::get_abi(owner->m_winui->overflowFlyout) !=
                     overflowFlyoutIdentity ||
                 winrt::get_abi(owner->m_winui->overflowPanel) !=
                     overflowPanelIdentity )
            {
                return nullptr;
            }

            const wxWinUIToolPeer * const peer =
                owner->m_winui->FindPeerByKey(peerKey);
            if ( !peer || !peer->inOverflow )
                return nullptr;

            const MUX::FrameworkElement currentElement =
                dropdownPart
                    ? peer->dropdownButton
                          .try_as<MUX::FrameworkElement>()
                    : peer->element;
            return currentElement &&
                           winrt::get_abi(currentElement) ==
                               elementIdentity
                ? peer
                : nullptr;
        };

    try
    {
        if ( !getCurrentPeer() ||
             !toolbar.m_winui->overflowFlyout.IsOpen() )
        {
            return false;
        }
        if ( !getCurrentPeer() || !element.IsLoaded() )
            return false;
        if ( !getCurrentPeer() || !element.XamlRoot() )
            return false;
        return getCurrentPeer() != nullptr;
    }
    catch ( const winrt::hresult_error& )
    {
        return false;
    }
}

bool wxWinUIToolBarTestAccess::InvokeOverflowTool(wxToolBar& toolbar, int toolid,
                                                  bool dropdownPart)
{
    wxToolBarToolBase * const tool = toolbar.FindById(toolid);
    if ( !tool || !toolbar.m_winui )
        return false;
    const wxWinUIToolPeer * const peer = toolbar.m_winui->FindPeerByTool(tool);
    if ( !peer || !peer->inOverflow || !tool->IsEnabled() ||
         !toolbar.IsEnabled() )
    {
        return false;
    }

    wxWinUIToolBarImpl * const invokeImpl = toolbar.m_winui.get();
    const std::shared_ptr<wxWinUIToolBarCallbackState> callbackState =
        invokeImpl->callbackState;
    const std::uint64_t generation = invokeImpl->peerGeneration;
    const std::uint64_t peerKey =
        static_cast<wxWinUIToolBarTool *>(tool)->peerKey;
    if ( dropdownPart &&
         (tool->GetKind() != wxITEM_DROPDOWN ||
          !peer->dropdownButton) )
    {
        return false;
    }

    try
    {
        wxToolBar * const owner =
            callbackState->GetOwner(generation);
        if ( !owner || !owner->m_winui ||
             owner->m_winui.get() != invokeImpl ||
             owner->m_winui->callbackState != callbackState ||
             owner->m_winui->peerGeneration != generation )
        {
            return false;
        }
        const wxWinUIToolPeer * const livePeer =
            owner->m_winui->FindPeerByKey(peerKey);
        if ( !livePeer || !livePeer->inOverflow )
            return false;

        if ( dropdownPart &&
             !wxWinUIToolBarTestAccess::IsOverflowToolLoaded(*owner,
                 toolid, true) )
        {
            return false;
        }

        // Drive the physical peer through its UI Automation provider. This
        // exercises the real XAML Click/Toggle/Selection callback and proves
        // overflow did not replace it with a synthetic wx-only path.
        return wxWinUIInvokeToolAutomationPeer(
            *livePeer, dropdownPart);
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException(
            "WinUI toolbar overflow automation invoke", e);
        return false;
    }
}

bool wxWinUIToolBarTestAccess::HoverTool(wxToolBar& toolbar, int toolid, bool entered)
{
    wxToolBarToolBase * const tool = toolbar.FindById(toolid);
    if ( !tool || !tool->IsButton() || !toolbar.m_winui )
        return false;
    const wxWinUIToolPeer * const peer =
        toolbar.m_winui->FindPeerByTool(tool);
    if ( !peer )
        return false;

    toolbar.OnToolHovered(
        static_cast<wxWinUIToolBarTool *>(tool)->peerKey,
        toolbar.m_winui->peerGeneration,
        entered);
    return true;
}

bool wxWinUIToolBarTestAccess::RightClickTool(wxToolBar& toolbar, int toolid)
{
    wxToolBarToolBase * const tool = toolbar.FindById(toolid);
    if ( !tool || !tool->IsButton() || !toolbar.m_winui ||
         !toolbar.m_winui->callbackState || !toolbar.m_winui->root ||
         !tool->IsEnabled() || !toolbar.IsEnabled() )
    {
        return false;
    }
    const wxWinUIToolPeer * const peer =
        toolbar.m_winui->FindPeerByTool(tool);
    if ( !peer )
        return false;

    wxWinUIToolBarImpl * const rightClickImpl = toolbar.m_winui.get();
    const std::shared_ptr<wxWinUIToolBarCallbackState> callbackState =
        toolbar.m_winui->callbackState;
    const std::uint64_t generation = toolbar.m_winui->peerGeneration;
    const std::uint64_t peerKey =
        static_cast<wxWinUIToolBarTool *>(tool)->peerKey;
    const bool wasInOverflow = peer->inOverflow;
    const void * const peerIdentity = winrt::get_abi(peer->element);
    const MUXC::Grid root = toolbar.m_winui->root;
    const MUXC::Button overflowButton = toolbar.m_winui->overflowButton;
    const void * const rootIdentity = winrt::get_abi(root);
    const void * const overflowButtonIdentity =
        winrt::get_abi(overflowButton);
    wxToolBar * const toolbarIdentity = &toolbar;
    const auto pointIsCurrent =
        [callbackState,
         generation,
         peerKey,
         rightClickImpl,
         rootIdentity,
         overflowButtonIdentity,
         wasInOverflow,
         peerIdentity,
         toolbarIdentity]()
        {
            wxToolBar * const owner =
                callbackState->GetOwner(generation);
            if ( owner != toolbarIdentity || !owner->m_winui ||
                 owner->m_winui.get() != rightClickImpl ||
                 owner->m_winui->callbackState != callbackState ||
                 owner->m_winui->peerGeneration != generation ||
                 winrt::get_abi(owner->m_winui->root) !=
                     rootIdentity ||
                 winrt::get_abi(owner->m_winui->overflowButton) !=
                     overflowButtonIdentity )
            {
                return false;
            }

            const wxWinUIToolPeer * const currentPeer =
                owner->m_winui->FindPeerByKey(peerKey);
            return currentPeer && currentPeer->tool &&
                   currentPeer->inOverflow == wasInOverflow &&
                   winrt::get_abi(currentPeer->element) ==
                       peerIdentity;
        };
    wxPoint point;
    try
    {
        if ( !wxWinUIGetToolRightClickPoint(
                 root,
                 overflowButton,
                 *peer,
                 nullptr,
                 &toolbar,
                 &point,
                 pointIsCurrent) )
        {
            return false;
        }
    }
    catch ( const winrt::hresult_error& )
    {
        return false;
    }

    wxToolBar * const owner =
        callbackState->GetOwner(generation);
    if ( !owner || !pointIsCurrent() )
        return false;
    const wxWinUIToolPeer * const livePeer =
        owner->m_winui->FindPeerByKey(peerKey);
    if ( !livePeer || !livePeer->tool ||
         livePeer->inOverflow != wasInOverflow ||
         winrt::get_abi(livePeer->element) != peerIdentity )
    {
        return false;
    }

    owner->OnToolRightClicked(peerKey, generation, point);
    return true;
}

bool wxWinUIToolBarTestAccess::QueueToolClick(wxToolBar& toolbar, int toolid)
{
    wxToolBarToolBase * const tool = toolbar.FindById(toolid);
    if ( !tool || !tool->IsButton() || !toolbar.m_winui ||
         !toolbar.m_winui->callbackState || !wxTheApp )
    {
        return false;
    }

    const wxWinUIToolPeer * const peer = toolbar.m_winui->FindPeerByTool(tool);
    if ( !peer || !peer->primaryButton )
        return false;

    const std::shared_ptr<wxWinUIToolBarCallbackState> callbackState =
        toolbar.m_winui->callbackState;
    const std::uint64_t generation = toolbar.m_winui->peerGeneration;
    const std::uint64_t peerKey =
        static_cast<wxWinUIToolBarTool *>(tool)->peerKey;
    wxTheApp->CallAfter(
        [callbackState, generation, peerKey]()
        {
            wxToolBar * const owner =
                callbackState->GetOwner(generation);
            if ( !owner || !owner->m_winui ||
                 owner->m_winui->callbackState != callbackState ||
                 owner->m_winui->peerGeneration != generation )
            {
                return;
            }
            owner->OnToolClicked(peerKey, generation);
        });
    return true;
}

bool wxWinUIToolBarTestAccess::GetToolPeerState(const wxToolBar& toolbar, int toolid,
    PeerSnapshot *snapshot)
{
    if ( !snapshot || !toolbar.m_winui || !toolbar.m_winui->callbackState ||
         !toolbar.m_winui->root )
        return false;

    const wxToolBarToolBase *tool = toolbar.FindById(toolid);
    if ( !tool )
        return false;
    const wxWinUIToolBarTool *winuiTool =
        static_cast<const wxWinUIToolBarTool *>(tool);
    const wxWinUIToolPeer *peer =
        toolbar.m_winui->FindPeerByTool(tool);
    if ( !peer )
        return false;

    wxWinUIToolBarImpl * const snapshotImpl = toolbar.m_winui.get();
    const std::shared_ptr<wxWinUIToolBarCallbackState> callbackState =
        toolbar.m_winui->callbackState;
    const std::uint64_t generation = toolbar.m_winui->peerGeneration;
    const std::uint64_t peerKey = winuiTool->peerKey;
    const void * const peerIdentity =
        winrt::get_abi(peer->element);
    const void * const rootIdentity =
        winrt::get_abi(toolbar.m_winui->root);
    const wxToolBar * const toolbarIdentity = &toolbar;
    const auto getCurrentPeer =
        [callbackState,
         generation,
         peerKey,
         snapshotImpl,
         peerIdentity,
         rootIdentity,
         toolbarIdentity]() -> const wxWinUIToolPeer *
        {
            wxToolBar *owner = callbackState->GetOwner(generation);
            if ( !owner )
            {
                // This implementation-only snapshot is also used from the
                // transaction-bound Loaded seam after the candidate and its
                // generation have been provisionally published. Event routing
                // must remain gated by peerMutationDepth, but introspection may
                // observe only that exact active rebuild generation.
                owner =
                    callbackState->GetOwnerDuringRebuild(generation);
            }
            if ( owner != toolbarIdentity || !owner->m_winui ||
                 owner->m_winui.get() != snapshotImpl ||
                 owner->m_winui->callbackState != callbackState ||
                 owner->m_winui->peerGeneration != generation ||
                 winrt::get_abi(owner->m_winui->root) != rootIdentity )
            {
                return nullptr;
            }

            const wxWinUIToolPeer * const currentPeer =
                owner->m_winui->FindPeerByKey(peerKey);
            return currentPeer && currentPeer->tool &&
                   winrt::get_abi(currentPeer->element) ==
                       peerIdentity
                ? currentPeer
                : nullptr;
        };

    snapshot->enabled = tool->IsEnabled();
    snapshot->toggled = tool->CanBeToggled() && tool->IsToggled();
    snapshot->overflowed = peer->inOverflow;
    snapshot->overflowEligible = peer->overflowEligible;
    snapshot->hasIcon = peer->hasIcon;
    snapshot->usesDisabledBitmap = peer->usesDisabledBitmap;
    snapshot->showsText = toolbar.HasFlag(wxTB_TEXT);
    snapshot->horizontalText =
        toolbar.HasFlag(wxTB_TEXT) && toolbar.HasFlag(wxTB_HORZ_LAYOUT);
    snapshot->stretchable = tool->IsStretchable();
    snapshot->control = tool->IsControl();
    snapshot->selectedPixelSize = peer->selectedPixelSize;
    snapshot->bounds = peer->inOverflow
        ? wxRect()
        : wxWinUIGetToolBounds(
              toolbar.m_winui->root,
              peer->element,
              &toolbar,
              [&getCurrentPeer]()
              {
                  return getCurrentPeer() != nullptr;
              });
    peer = getCurrentPeer();
    if ( !peer )
        return false;
    tool = peer->tool;
    winuiTool =
        static_cast<const wxWinUIToolBarTool *>(tool);
    snapshot->toolTip =
        toolbar.HasFlag(wxTB_NO_TOOLTIPS)
            ? wxString()
            : tool->GetShortHelp();
    snapshot->peerToolTip.clear();
    snapshot->helpText.clear();
    snapshot->automationName.clear();
    snapshot->automationLocalizedControlType.clear();
    snapshot->peerIdentity = static_cast<unsigned long long>(
        reinterpret_cast<std::uintptr_t>(
            winrt::get_abi(peer->element)));
    snapshot->automationControlType = 0;
    snapshot->radioAutomationRole = false;
    snapshot->supportsInvokePattern = false;
    snapshot->supportsTogglePattern = false;
    snapshot->supportsSelectionItemPattern = false;
    try
    {
        if ( tool->IsControl() )
        {
            if ( wxControl * const control = winuiTool->GetLiveControl() )
                snapshot->enabled = control->IsEnabled();
#if wxUSE_TOOLTIPS
            if ( wxControl * const control = winuiTool->GetLiveControl() )
            {
                if ( wxToolTip * const tip = control->GetToolTip() )
                    snapshot->peerToolTip = tip->GetTip();
            }
#endif
        }
        else
        {
            if ( peer->primaryButton )
            {
                snapshot->enabled = peer->primaryButton.IsEnabled();
                if ( const auto toggle =
                         peer->primaryButton.try_as<MUXCP::ToggleButton>() )
                {
                    const auto checked = toggle.IsChecked();
                    snapshot->toggled =
                        checked && checked.Value();
                }
                snapshot->automationName = wxString(
                    MUXA::AutomationProperties::GetName(
                        peer->primaryButton).c_str());
                snapshot->automationLocalizedControlType = wxString(
                    MUXA::AutomationProperties::
                        GetLocalizedControlType(
                            peer->primaryButton).c_str());

                const MUXAP::AutomationPeer automationPeer =
                    wxWinUICreateToolAutomationPeer(*peer, false);
                if ( automationPeer )
                {
                    snapshot->automationControlType =
                        static_cast<int>(
                            automationPeer.GetAutomationControlType());
                    snapshot->radioAutomationRole =
                        automationPeer.GetAutomationControlType() ==
                        MUXAP::AutomationControlType::RadioButton;
                    snapshot->supportsInvokePattern =
                        static_cast<bool>(
                            automationPeer.GetPattern(
                                MUXAP::PatternInterface::Invoke));
                    snapshot->supportsTogglePattern =
                        static_cast<bool>(
                            automationPeer.GetPattern(
                                MUXAP::PatternInterface::Toggle));
                    snapshot->supportsSelectionItemPattern =
                        static_cast<bool>(
                            automationPeer.GetPattern(
                                MUXAP::PatternInterface::SelectionItem));
                }
            }
            if ( peer->dropdownButton )
            {
                snapshot->enabled =
                    snapshot->enabled &&
                    peer->dropdownButton.IsEnabled();
            }

            const auto value =
                MUXC::ToolTipService::GetToolTip(peer->element);
            if ( const auto tip = value.try_as<MUXC::ToolTip>() )
            {
                snapshot->peerToolTip = wxString(
                    winrt::unbox_value_or<winrt::hstring>(
                        tip.Content(), {}).c_str());
            }
            snapshot->helpText = wxString(
                MUXA::AutomationProperties::GetHelpText(
                    peer->element).c_str());
            if ( snapshot->automationName.empty() )
            {
                snapshot->automationName = wxString(
                    MUXA::AutomationProperties::GetName(
                        peer->element).c_str());
            }
        }
    }
    catch ( const winrt::hresult_error& )
    {
        return false;
    }
    return true;
}

size_t wxWinUIToolBarTestAccess::GetPeerToolCount(const wxToolBar& toolbar)
{
    return toolbar.m_winui ? toolbar.m_winui->peers.size() : 0;
}

size_t wxWinUIToolBarTestAccess::GetOverflowedToolCount(const wxToolBar& toolbar)
{
    if ( !toolbar.m_winui )
        return 0;

    return static_cast<size_t>(std::count_if(
        toolbar.m_winui->peers.begin(), toolbar.m_winui->peers.end(),
        [](const wxWinUIToolPeer& peer)
        {
            return peer.tool && !peer.tool->IsSeparator() &&
                   peer.inOverflow;
        }));
}

bool wxWinUIToolBarTestAccess::ApplyOverflowExtent(wxToolBar& toolbar, wxCoord extent)
{
    if ( extent <= 0 || !toolbar.m_winui || !toolbar.m_winui->realized ||
         !toolbar.m_winui->callbackState )
    {
        return false;
    }

    wxWinUIToolBarImpl * const updateImpl = toolbar.m_winui.get();
    const std::shared_ptr<wxWinUIToolBarCallbackState> callbackState =
        updateImpl->callbackState;
    const std::uint64_t generation = updateImpl->peerGeneration;
    wxToolBar *owner = callbackState->GetOwner(generation);
    if ( !owner || !owner->m_winui ||
         owner->m_winui.get() != updateImpl ||
         owner->m_winui->callbackState != callbackState ||
         owner->m_winui->peerGeneration != generation )
    {
        return false;
    }

    const wxSize dipExtent =
        owner->ToDIP(wxSize(extent, extent));
    const WF::Size arrangedSize{
        static_cast<float>(dipExtent.x),
        static_cast<float>(dipExtent.y)
    };
    try
    {
        owner->m_winui->root.Measure(arrangedSize);
        owner->m_winui->root.Arrange(
            { 0.0f, 0.0f, arrangedSize.Width, arrangedSize.Height });
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException(
            "WinUI toolbar test overflow arrange", e);
        return false;
    }
    owner = callbackState->GetOwner(generation);
    if ( !owner || !owner->m_winui ||
         owner->m_winui.get() != updateImpl ||
         owner->m_winui->callbackState != callbackState ||
         owner->m_winui->peerGeneration != generation )
    {
        return false;
    }
    if ( !owner->UpdateOverflow(
             owner->IsVertical()
                 ? static_cast<double>(dipExtent.y)
                 : static_cast<double>(dipExtent.x),
             generation) )
    {
        return false;
    }

    wxToolBar *liveOwner =
        callbackState->GetOwner(generation);
    if ( !liveOwner || !liveOwner->m_winui ||
         liveOwner->m_winui.get() != updateImpl ||
         liveOwner->m_winui->callbackState != callbackState ||
         liveOwner->m_winui->peerGeneration != generation )
    {
        return false;
    }
    liveOwner->m_winui->host.ForceRender();
    liveOwner = callbackState->GetOwner(generation);
    if ( !liveOwner || !liveOwner->m_winui ||
         liveOwner->m_winui.get() != updateImpl ||
         liveOwner->m_winui->callbackState != callbackState ||
         liveOwner->m_winui->peerGeneration != generation )
    {
        return false;
    }
    try
    {
        liveOwner->m_winui->root.Measure(arrangedSize);
        liveOwner->m_winui->root.Arrange(
            { 0.0f, 0.0f, arrangedSize.Width, arrangedSize.Height });
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException(
            "WinUI toolbar test overflow re-arrange", e);
        return false;
    }
    liveOwner = callbackState->GetOwner(generation);
    if ( !liveOwner || !liveOwner->m_winui ||
         liveOwner->m_winui.get() != updateImpl ||
         liveOwner->m_winui->callbackState != callbackState ||
         liveOwner->m_winui->peerGeneration != generation )
    {
        return false;
    }
    if ( !liveOwner->UpdateOverflow(
             liveOwner->IsVertical()
                 ? static_cast<double>(dipExtent.y)
                 : static_cast<double>(dipExtent.x),
             generation) )
    {
        return false;
    }
    liveOwner = callbackState->GetOwner(generation);
    if ( !liveOwner || !liveOwner->m_winui ||
         liveOwner->m_winui.get() != updateImpl ||
         liveOwner->m_winui->callbackState != callbackState ||
         liveOwner->m_winui->peerGeneration != generation )
    {
        return false;
    }
    liveOwner->SyncControlTools(generation);
    return true;
}

bool wxWinUIToolBarTestAccess::IsOverflowChevronVisible(const wxToolBar& toolbar)
{
    if ( !toolbar.m_winui || !toolbar.m_winui->overflowButton )
        return false;
    try
    {
        return toolbar.m_winui->overflowButton.Visibility() ==
               MUX::Visibility::Visible;
    }
    catch ( const winrt::hresult_error& )
    {
        return false;
    }
}

wxRect wxWinUIToolBarTestAccess::GetOverflowChevronBounds(const wxToolBar& toolbar)
{
    if ( !toolbar.m_winui || !toolbar.m_winui->callbackState || !toolbar.m_winui->root ||
         !toolbar.m_winui->overflowButton )
    {
        return wxRect();
    }

    wxWinUIToolBarImpl * const boundsImpl = toolbar.m_winui.get();
    const std::shared_ptr<wxWinUIToolBarCallbackState> callbackState =
        toolbar.m_winui->callbackState;
    const std::uint64_t generation = toolbar.m_winui->peerGeneration;
    const MUXC::Grid root = toolbar.m_winui->root;
    const MUXC::Button overflowButton = toolbar.m_winui->overflowButton;
    const void * const rootIdentity = winrt::get_abi(root);
    const void * const overflowButtonIdentity =
        winrt::get_abi(overflowButton);
    const wxToolBar * const toolbarIdentity = &toolbar;
    const auto isCurrent =
        [callbackState,
         generation,
         boundsImpl,
         rootIdentity,
         overflowButtonIdentity,
         toolbarIdentity]()
        {
            wxToolBar * const owner =
                callbackState->GetOwner(generation);
            return owner == toolbarIdentity && owner->m_winui &&
                   owner->m_winui.get() == boundsImpl &&
                   owner->m_winui->callbackState == callbackState &&
                   owner->m_winui->peerGeneration == generation &&
                   winrt::get_abi(owner->m_winui->root) ==
                       rootIdentity &&
                   winrt::get_abi(owner->m_winui->overflowButton) ==
                       overflowButtonIdentity;
        };
    if ( !isCurrent() ||
         !wxWinUIToolBarTestAccess::IsOverflowChevronVisible(toolbar) ||
         !isCurrent() )
    {
        return wxRect();
    }

    try
    {
        return wxWinUIGetToolBounds(
            root, overflowButton, &toolbar, isCurrent);
    }
    catch ( const winrt::hresult_error& )
    {
        return wxRect();
    }
}

wxString wxWinUIToolBarTestAccess::GetOverflowChevronName(const wxToolBar& toolbar)
{
    if ( !toolbar.m_winui || !toolbar.m_winui->overflowButton )
        return wxString();
    try
    {
        return wxString(
            MUXA::AutomationProperties::GetName(
                toolbar.m_winui->overflowButton).c_str());
    }
    catch ( const winrt::hresult_error& )
    {
        return wxString();
    }
}

bool wxWinUIToolBarTestAccess::RefreshForScale(wxToolBar& toolbar, double scale)
{
    return scale > 0.0 && toolbar.m_winui && toolbar.m_winui->realized &&
           toolbar.RebuildPeer(nullptr, static_cast<size_t>(-1), nullptr, scale);
}

void wxWinUIToolBarTestAccess::SetNextRebuildLoadedHook(
    wxToolBar& toolbar, CallbackHook hook,
    void *context)
{
    if ( !toolbar.m_winui || !toolbar.m_winui->callbackState || toolbar.m_winui->closed )
        return;

    toolbar.m_winui->nextRebuildLoadedHookForTesting = hook;
    toolbar.m_winui->nextRebuildLoadedContextForTesting =
        hook ? context : nullptr;
}

void wxWinUIToolBarTestAccess::SetNextShortHelpSetterHook(
    wxToolBar& toolbar, CallbackHook hook,
    void *context)
{
    if ( !toolbar.m_winui || !toolbar.m_winui->callbackState || toolbar.m_winui->closed )
        return;

    toolbar.m_winui->nextShortHelpSetterHookForTesting = hook;
    toolbar.m_winui->nextShortHelpSetterContextForTesting =
        hook ? context : nullptr;
}

void wxWinUIToolBarTestAccess::SetNextEnableSetterHook(
    wxToolBar& toolbar, CallbackHook hook,
    void *context)
{
    if ( !toolbar.m_winui || !toolbar.m_winui->callbackState || toolbar.m_winui->closed )
        return;

    toolbar.m_winui->nextEnableSetterHookForTesting = hook;
    toolbar.m_winui->nextEnableSetterContextForTesting =
        hook ? context : nullptr;
}

void wxWinUIToolBarTestAccess::SetNextOverflowMutationHook(
    wxToolBar& toolbar, CallbackHook hook,
    void *context)
{
    if ( !toolbar.m_winui || !toolbar.m_winui->callbackState || toolbar.m_winui->closed )
        return;

    toolbar.m_winui->nextOverflowMutationHookForTesting = hook;
    toolbar.m_winui->nextOverflowMutationContextForTesting =
        hook ? context : nullptr;
}

void wxWinUIToolBarTestAccess::FailNextOverflowMutation(
    wxToolBar& toolbar, unsigned boundary)
{
    if ( !toolbar.m_winui || !toolbar.m_winui->callbackState || toolbar.m_winui->closed )
        return;

    toolbar.m_winui->failNextOverflowMutationBoundaryForTesting = boundary;
}

void wxWinUIToolBarTestAccess::FailNextShortHelpSetters(wxToolBar& toolbar, int toolid,
    unsigned count)
{
    wxToolBarToolBase * const tool = toolbar.FindById(toolid);
    if ( !tool )
        return;
    static_cast<wxWinUIToolBarTool *>(tool)->
        shortHelpSetterFaultsForTesting = count;
}

void wxWinUIToolBarTestAccess::FailNextRebuild(wxToolBar& toolbar)
{
    if ( toolbar.m_winui )
        toolbar.m_winui->failNextRebuildForTesting = true;
}

void wxWinUIToolBarTestAccess::ClosePeer(wxToolBar& toolbar)
{
    if ( toolbar.m_winui )
        toolbar.m_winui->Close();
}

size_t wxWinUIToolBarTestAccess::GetLiveCallbackStateCount()
{
    return gs_liveToolBarCallbackStates.load(std::memory_order_relaxed);
}

#endif // WXWINUI_TEST_SUPPORT

#endif // wxUSE_TOOLBAR
