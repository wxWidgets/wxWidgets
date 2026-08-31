/////////////////////////////////////////////////////////////////////////////
// Name:        src/winui/treectrl.cpp
// Purpose:     wxWinUI wxTreeCtrl implementation
// Author:      wxWidgets development team
// Created:     2026-06-01
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_TREECTRL

#include "wx/treectrl.h"

#ifdef WXWINUI_TEST_SUPPORT
#include "treectrl-test-access.h"
#endif

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/settings.h"
    #include "wx/textctrl.h"
    #include "wx/utils.h"
#endif

#include "private.h"
#include "wx/winui/private/tlwhost.h"

#include "wx/log.h"
#include "wx/msw/private/keyboard.h"
#include "wx/scopeguard.h"
#include "wx/weakref.h"

#include <algorithm>
#include <atomic>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <limits>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include <winrt/Microsoft.UI.Dispatching.h>
#include <winrt/Microsoft.UI.Input.h>
#include <winrt/Microsoft.UI.Text.h>
#include <winrt/Microsoft.UI.Xaml.Automation.h>
#include <winrt/Microsoft.UI.Xaml.Input.h>
#include <winrt/Microsoft.UI.Xaml.Media.h>
#include <winrt/Windows.ApplicationModel.DataTransfer.h>
#include <winrt/Windows.UI.Text.h>

namespace MUX = winrt::Microsoft::UI::Xaml;
namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;
namespace MUXD = winrt::Microsoft::UI::Dispatching;
namespace MUXI = winrt::Microsoft::UI::Xaml::Input;
namespace MUXM = winrt::Microsoft::UI::Xaml::Media;
namespace MUXMI = winrt::Microsoft::UI::Xaml::Media::Imaging;
namespace WFC = winrt::Windows::Foundation::Collections;

namespace
{

#ifdef WXWINUI_TEST_SUPPORT
std::atomic<std::size_t> gs_liveTreeCallbackStates{0};
#endif
std::atomic<std::uint64_t> gs_nextTreeCallbackGeneration{0};

// XAML callbacks and dispatcher work retain only a weak reference to this
// state. The generation makes a callback from a retired peer fail closed even
// if a new control is later allocated at the same wx address.
class wxWinUITreeCallbackState final
{
public:
    explicit wxWinUITreeCallbackState(wxTreeCtrl *owner)
        : m_owner(owner),
          m_generation(
              gs_nextTreeCallbackGeneration.fetch_add(
                  1, std::memory_order_relaxed) + 1)
    {
#ifdef WXWINUI_TEST_SUPPORT
        gs_liveTreeCallbackStates.fetch_add(1, std::memory_order_relaxed);
#endif
    }

    ~wxWinUITreeCallbackState()
    {
        Invalidate();
#ifdef WXWINUI_TEST_SUPPORT
        gs_liveTreeCallbackStates.fetch_sub(1, std::memory_order_relaxed);
#endif
    }

    std::uint64_t Generation() const
    {
        return m_generation.load(std::memory_order_acquire);
    }

    wxTreeCtrl *GetOwner(std::uint64_t generation) const
    {
        if ( generation != m_generation.load(std::memory_order_acquire) )
            return nullptr;

        return m_owner.load(std::memory_order_acquire);
    }

    void Invalidate()
    {
        m_owner.store(nullptr, std::memory_order_release);
        m_generation.fetch_add(1, std::memory_order_acq_rel);
        m_operation.fetch_add(1, std::memory_order_acq_rel);
    }

    std::uint64_t BeginOperation()
    {
        return m_operation.fetch_add(1, std::memory_order_acq_rel) + 1;
    }

    bool IsCurrentOperation(std::uint64_t operation) const
    {
        return operation == m_operation.load(std::memory_order_acquire);
    }

    std::uint64_t CurrentOperation() const
    {
        return m_operation.load(std::memory_order_acquire);
    }

    void BeginPeerMutation()
    {
        m_peerMutationDepth.fetch_add(1, std::memory_order_acq_rel);
    }

    void EndPeerMutation()
    {
        const unsigned previous =
            m_peerMutationDepth.fetch_sub(1, std::memory_order_acq_rel);
        wxASSERT_MSG(previous != 0, "unbalanced WinUI tree peer mutation");
    }

    bool IsPeerMutationInProgress() const
    {
        return m_peerMutationDepth.load(std::memory_order_acquire) != 0;
    }

private:
    std::atomic<wxTreeCtrl *> m_owner;
    std::atomic<std::uint64_t> m_generation;
    std::atomic<std::uint64_t> m_operation{0};
    std::atomic<unsigned> m_peerMutationDepth{0};
};

class wxWinUITreePeerMutationGuard final
{
public:
    explicit wxWinUITreePeerMutationGuard(
        const std::shared_ptr<wxWinUITreeCallbackState>& state)
        : m_state(state)
    {
        if ( m_state )
            m_state->BeginPeerMutation();
    }

    ~wxWinUITreePeerMutationGuard()
    {
        if ( m_state )
            m_state->EndPeerMutation();
    }

    wxWinUITreePeerMutationGuard(
        const wxWinUITreePeerMutationGuard&) = delete;
    wxWinUITreePeerMutationGuard& operator=(
        const wxWinUITreePeerMutationGuard&) = delete;

private:
    std::shared_ptr<wxWinUITreeCallbackState> m_state;
};

} // anonymous namespace

class wxWinUITreeItemHandle final
{
};

class wxWinUITreeItem
{
public:
    explicit wxWinUITreeItem(wxWinUITreeItem *parentItem = nullptr)
        : parent(parentItem)
    {
        std::fill(images, images + wxTreeItemIcon_Max, wxTreeCtrl::NO_IMAGE);
    }

    ~wxWinUITreeItem()
    {
        delete data;
    }

    wxString text;
    int images[wxTreeItemIcon_Max];
    int state = wxTREE_ITEMSTATE_NONE;
    wxTreeItemData *data = nullptr;
    wxWinUITreeItem *parent = nullptr;
    std::vector<std::unique_ptr<wxWinUITreeItem>> children;
    bool expanded = false;
    bool selected = false;
    bool bold = false;
    bool hasChildrenOverride = false;
    wxColour textColour;
    wxColour backgroundColour;
    wxFont font;
    bool dropHighlighted = false;
    bool deleting = false;
    std::uint64_t id = 0;
    std::uint64_t textMutation = 0;
    size_t siblingIndex = 0;
    wxWinUITreeItemHandle *handle = nullptr;
    MUXC::TreeViewNode node{ nullptr };
    WFC::PropertySet content{ nullptr };
    bool contentAttached = false;
    std::uint64_t contentProjectionGeneration = 0;
};

static int wxWinUIEffectiveTreeItemImage(
    const wxWinUITreeItem *item,
    bool selected,
    bool expanded)
{
    if ( !item )
        return wxTreeCtrl::NO_IMAGE;

    if ( selected && expanded &&
         item->images[wxTreeItemIcon_SelectedExpanded] !=
             wxTreeCtrl::NO_IMAGE )
    {
        return item->images[wxTreeItemIcon_SelectedExpanded];
    }
    if ( selected &&
         item->images[wxTreeItemIcon_Selected] != wxTreeCtrl::NO_IMAGE )
    {
        return item->images[wxTreeItemIcon_Selected];
    }
    if ( expanded &&
         item->images[wxTreeItemIcon_Expanded] != wxTreeCtrl::NO_IMAGE )
    {
        return item->images[wxTreeItemIcon_Expanded];
    }
    return item->images[wxTreeItemIcon_Normal];
}

static void
wxWinUIDestroySubtree(std::unique_ptr<wxWinUITreeItem> root)
{
    if ( !root )
        return;

    std::vector<std::unique_ptr<wxWinUITreeItem>> pending;
    pending.push_back(std::move(root));
    while ( !pending.empty() )
    {
        std::unique_ptr<wxWinUITreeItem> current =
            std::move(pending.back());
        pending.pop_back();
        for ( auto& child : current->children )
            pending.push_back(std::move(child));
        current->children.clear();
        // current (and its user data) is destroyed here without recursively
        // walking a potentially 10k-deep ownership chain.
    }
}

class wxWinUITreeDetachedSubtree final
{
public:
    explicit wxWinUITreeDetachedSubtree(
        std::unique_ptr<wxWinUITreeItem> root)
        : m_root(std::move(root))
    {
    }

    ~wxWinUITreeDetachedSubtree()
    {
        wxWinUIDestroySubtree(std::move(m_root));
    }

private:
    std::unique_ptr<wxWinUITreeItem> m_root;
};

class wxWinUITreeDetachedForest final
{
public:
    void Swap(
        std::vector<std::unique_ptr<wxWinUITreeItem>>& roots)
    {
        m_roots.swap(roots);
    }

    ~wxWinUITreeDetachedForest()
    {
        for ( auto& root : m_roots )
            wxWinUIDestroySubtree(std::move(root));
    }

private:
    std::vector<std::unique_ptr<wxWinUITreeItem>> m_roots;
};

class wxWinUITreeCtrlImpl
{
public:
    ~wxWinUITreeCtrlImpl()
    {
        Close();
    }

    void Close()
    {
        // Invalidate before revoking anything: token removal and host
        // detachment can synchronously drain queued XAML work.
        if ( callbackState )
            callbackState->Invalidate();

        if ( closed )
            return;
        closed = true;

        const auto revoke =
            [](winrt::event_token& token,
               const char *what,
               const auto& remove)
            {
                if ( !token.value )
                    return;
                try
                {
                    remove();
                }
                catch ( const winrt::hresult_error& e )
                {
                    wxWinUILogException(what, e);
                }
                token = {};
            };

        if ( labelEditTimer )
        {
            try
            {
                labelEditTimer.Stop();
                if ( labelEditTimerToken.value )
                    labelEditTimer.Tick(labelEditTimerToken);
            }
            catch ( const winrt::hresult_error& e )
            {
                wxWinUILogException(
                    "WinUI TreeView label-edit timer removal", e);
            }
        }
        labelEditTimerToken = {};
        labelEditTimer = nullptr;
        clickEditCandidateId = 0;
        pendingLabelEditId = 0;
        pressedStateImageId = 0;
        ++labelEditDelayTicket;

        if ( treeView )
        {
            revoke(selectionChangedToken,
                   "WinUI TreeView SelectionChanged removal",
                   [&] { treeView.SelectionChanged(selectionChangedToken); });
            revoke(gotFocusToken,
                   "WinUI TreeView GotFocus removal",
                   [&] { treeView.GotFocus(gotFocusToken); });
            revoke(listHookLoadedToken,
                   "WinUI TreeView Loaded removal",
                   [&] { treeView.Loaded(listHookLoadedToken); });
            revoke(expandingToken,
                   "WinUI TreeView Expanding removal",
                   [&] { treeView.Expanding(expandingToken); });
            revoke(collapsedToken,
                   "WinUI TreeView Collapsed removal",
                   [&] { treeView.Collapsed(collapsedToken); });
            revoke(itemInvokedToken,
                   "WinUI TreeView ItemInvoked removal",
                   [&] { treeView.ItemInvoked(itemInvokedToken); });
            revoke(rightTappedToken,
                   "WinUI TreeView RightTapped removal",
                   [&] { treeView.RightTapped(rightTappedToken); });
            revoke(pointerPressedToken,
                   "WinUI TreeView PointerPressed removal",
                   [&] { treeView.PointerPressed(pointerPressedToken); });
            revoke(pointerMovedToken,
                   "WinUI TreeView PointerMoved removal",
                   [&] { treeView.PointerMoved(pointerMovedToken); });
            revoke(pointerReleasedToken,
                   "WinUI TreeView PointerReleased removal",
                   [&] { treeView.PointerReleased(pointerReleasedToken); });
            revoke(doubleTappedToken,
                   "WinUI TreeView DoubleTapped removal",
                   [&] { treeView.DoubleTapped(doubleTappedToken); });
            revoke(lostFocusToken,
                   "WinUI TreeView LostFocus removal",
                   [&] { treeView.LostFocus(lostFocusToken); });
            revoke(keyDownToken,
                   "WinUI TreeView KeyDown removal",
                   [&] { treeView.KeyDown(keyDownToken); });
            revoke(dragStartingToken,
                   "WinUI TreeView DragItemsStarting removal",
                   [&] { treeView.DragItemsStarting(dragStartingToken); });
            revoke(dragCompletedToken,
                   "WinUI TreeView DragItemsCompleted removal",
                   [&] { treeView.DragItemsCompleted(dragCompletedToken); });
        }

        selectionChangedToken = {};
        gotFocusToken = {};
        listHookLoadedToken = {};
        expandingToken = {};
        collapsedToken = {};
        itemInvokedToken = {};
        rightTappedToken = {};
        pointerPressedToken = {};
        pointerMovedToken = {};
        pointerReleasedToken = {};
        doubleTappedToken = {};
        lostFocusToken = {};
        keyDownToken = {};
        dragStartingToken = {};
        dragCompletedToken = {};

        if ( treeList )
        {
            revoke(containerContentChangingToken,
                   "WinUI TreeView container event removal",
                   [&]
                   {
                       treeList.ContainerContentChanging(
                           containerContentChangingToken);
                   });
        }
        containerContentChangingToken = {};

        // Snapshot model preorder while all peer nodes are still strongly
        // referenced. If breaking any COM parent->child edge fails, releasing
        // parent nodes first guarantees that every child also has an external
        // strong reference and prevents recursive destruction of a deep chain.
        std::vector<wxWinUITreeItem *> releaseOrder;
        releaseOrder.reserve(itemsById.size());
        if ( root )
        {
            std::vector<wxWinUITreeItem *> pending{root.get()};
            while ( !pending.empty() )
            {
                wxWinUITreeItem * const item = pending.back();
                pending.pop_back();
                releaseOrder.push_back(item);
                for ( auto child = item->children.rbegin();
                      child != item->children.rend();
                      ++child )
                {
                    pending.push_back(child->get());
                }
            }
        }

        // Keep one strong reference to every node while breaking the peer
        // parent->children graph. Releasing nodes from an unordered map first
        // can otherwise make the final root release recursively destroy a
        // 10k-deep COM chain even though the C++ model itself is torn down
        // iteratively below.
        // Break the deepest edges first while every model item still owns a
        // strong reference to its node. TreeView must never receive one root
        // removal containing an intact 10k-deep visual graph.
        for ( auto item = releaseOrder.rbegin();
              item != releaseOrder.rend();
              ++item )
        {
            if ( !(*item)->node )
                continue;
            try
            {
                const WFC::IVector<MUXC::TreeViewNode> children =
                    (*item)->node.Children();
                if ( children.Size() == 0 )
                    continue;
#ifdef WXWINUI_TEST_SUPPORT
                if ( ShouldFail(
                         wxWinUITreeCtrlTestAccess::PeerMutation::ClearItems) )
                {
                    throw winrt::hresult_error(
                        E_FAIL,
                        L"Injected TreeView teardown edge failure");
                }
#endif
                children.Clear();
            }
            catch ( const winrt::hresult_error& e )
            {
                wxWinUILogException(
                    "WinUI TreeView node teardown", e);
            }
        }
        if ( treeView )
        {
            try
            {
                treeView.RootNodes().Clear();
            }
            catch ( const winrt::hresult_error& e )
            {
                wxWinUILogException(
                    "WinUI TreeView root teardown", e);
            }

            try
            {
                // Publish the empty peer while it is still attached to its
                // XAML slot. Detaching a populated, recently virtualized
                // TreeView first can leave its internal BringIntoView cleanup
                // queued until another control renders, recursively releasing
                // an unrelated deep graph on that later stack.
                treeView.UpdateLayout();
            }
            catch ( const winrt::hresult_error& e )
            {
                wxWinUILogException(
                    "WinUI TreeView empty-layout teardown", e);
            }
        }

        // Only now detach the already flattened peer from the shared island.
        // No wx event can run here: callback state was invalidated and every
        // TreeView/list token was revoked above.
        host.Close();

        for ( wxWinUITreeItem * const item : releaseOrder )
        {
            if ( item->data )
                item->data->SetId(wxTreeItemId());
            item->id = 0;
            item->node = nullptr;
            item->content = nullptr;
            item->contentAttached = false;
        }
        selectedIds.clear();
        liveItems.clear();
        // Keep itemHandles allocated until the implementation itself dies:
        // stale wxTreeItemId values must never alias a later item.
        itemsById.clear();
        itemsByNode.clear();
        itemsByContent.clear();
        wxWinUIDestroySubtree(std::move(root));
        selectionId = 0;
        focusedId = 0;
        rejectedSelectionId = 0;
        editItemId = 0;
        dragItemId = 0;
        dropHighlightId = 0;
        itemCount = 0;
        expandableItemCount = 0;
        inPeerSelectionChange = false;
        peerCorrectionPending = false;
        peerCorrectionDriverActive = false;
        peerCorrectionQuarantined = false;
        peerCorrectionAttempts = 0;
#ifdef WXWINUI_TEST_SUPPORT
        failMutationCount = 0;
#endif
        projectionItemIds.clear();
        stateImageBundles.clear();
        projectionAllRequested = false;
        projectionInProgress = false;
        projectionReplayScheduled = false;
        projectionWarningIssued = false;
        projectionDeferredAttempts = 0;
        realizedMetadataDirty = false;
        peerStructureRepairPending = false;
        treeList = nullptr;
        treeView = nullptr;
        callbackState.reset();
    }

#ifdef WXWINUI_TEST_SUPPORT
    bool ShouldFail(wxWinUITreeCtrlTestAccess::PeerMutation mutation)
    {
        if ( failMutationCount == 0 || failedMutation != mutation )
            return false;

        --failMutationCount;
        return true;
    }
#endif

    std::uint64_t BumpModelRevision()
    {
        // Reentrant mutations share the retry budget of the transaction that
        // exposed them. A later external mutation rearms the single deferred
        // replay wave.
        const bool rearmDeferredBudget =
            !projectionInProgress && !projectionReplayScheduled;

        ++modelRevision;
        if ( modelRevision == 0 )
            ++modelRevision;
        if ( rearmDeferredBudget )
        {
            projectionDeferredAttempts = 0;
            projectionWarningIssued = false;
        }
        return modelRevision;
    }

    wxWinUIControlHost host;
    std::shared_ptr<wxWinUITreeCallbackState> callbackState;
    MUXC::TreeView treeView{ nullptr };
    MUXC::TreeViewList treeList{ nullptr };
    winrt::event_token selectionChangedToken{};
    winrt::event_token gotFocusToken{};
    winrt::event_token listHookLoadedToken{};
    winrt::event_token containerContentChangingToken{};
    winrt::event_token expandingToken{};
    winrt::event_token collapsedToken{};
    winrt::event_token itemInvokedToken{};
    winrt::event_token rightTappedToken{};
    winrt::event_token pointerPressedToken{};
    winrt::event_token pointerMovedToken{};
    winrt::event_token pointerReleasedToken{};
    winrt::event_token doubleTappedToken{};
    winrt::event_token lostFocusToken{};
    winrt::event_token keyDownToken{};
    winrt::event_token dragStartingToken{};
    winrt::event_token dragCompletedToken{};

    std::unique_ptr<wxWinUITreeItem> root;
    std::unordered_map<std::uint64_t, wxWinUITreeItem *> itemsById;
    std::unordered_map<std::uintptr_t, wxWinUITreeItem *> itemsByNode;
    std::unordered_map<std::uintptr_t, wxWinUITreeItem *> itemsByContent;
    // Handles are never recycled while this implementation exists, so a
    // deleted wxTreeItemId cannot alias a later item allocation.
    std::vector<std::unique_ptr<wxWinUITreeItemHandle>> itemHandles;
    std::unordered_map<wxWinUITreeItemHandle *, wxWinUITreeItem *> liveItems;
    std::unordered_set<std::uint64_t> selectedIds;
    wxVector<wxBitmapBundle> stateImageBundles;

    std::uint64_t nextItemId = 1;
    std::uint64_t selectionId = 0;
    std::uint64_t focusedId = 0;
    std::uint64_t rejectedSelectionId = 0;
    std::uint64_t editItemId = 0;
    std::uint64_t editSession = 0;
    std::uint64_t clickEditCandidateId = 0;
    std::uint64_t pendingLabelEditId = 0;
    std::uint64_t labelEditDelayTicket = 0;
    std::uint64_t pressedStateImageId = 0;
    wxPoint pointerPressPoint;
    MUXD::DispatcherQueueTimer labelEditTimer{ nullptr };
    winrt::event_token labelEditTimerToken{};
    std::uint64_t dragItemId = 0;
    std::uint64_t dragSession = 0;
    std::uint64_t dropHighlightId = 0;
    std::uint64_t modelRevision = 1;
    std::unordered_set<std::uint64_t> projectionItemIds;
    bool projectionAllRequested = false;
    bool projectionInProgress = false;
    bool projectionReplayScheduled = false;
    bool projectionWarningIssued = false;
    unsigned projectionDeferredAttempts = 0;
#ifdef WXWINUI_TEST_SUPPORT
    double projectionScaleOverrideForTesting = 0.0;
#endif
    bool realizedMetadataDirty = false;
    bool peerStructureRepairPending = false;

    size_t itemCount = 0;
    size_t expandableItemCount = 0;
#ifdef WXWINUI_TEST_SUPPORT
    size_t nodeLookupCount = 0;
    size_t peerUpdateCount = 0;
    size_t fullRefreshCount = 0;
    size_t modelGrowthCount = 0;
#endif

#ifdef WXWINUI_TEST_SUPPORT
    wxWinUITreeCtrlTestAccess::PeerMutation failedMutation =
        wxWinUITreeCtrlTestAccess::PeerMutation::InsertItem;
    unsigned failMutationCount = 0;
#endif
    bool inPeerSelectionChange = false;
    bool peerCorrectionPending = false;
    bool peerCorrectionDriverActive = false;
    bool peerCorrectionQuarantined = false;
    unsigned peerCorrectionAttempts = 0;
    double indentDips = 16.0;
    bool rightToLeft = false;
    bool showButtons = false;
    bool closed = false;
};

// The item text as stored in the node's PropertySet content (see the
// ItemTemplate bindings), used to give the generated TreeViewItem a UIA name.
static winrt::hstring wxWinUITreeNodeName(const MUXC::TreeViewNode& node)
{
    if ( node )
    {
        if ( auto content = node.Content().try_as<WFC::IPropertySet>() )
        {
            if ( content.HasKey(L"Text") )
                return winrt::unbox_value_or<winrt::hstring>(
                    content.Lookup(L"Text"), {});
        }
    }

    return {};
}

static MUXC::TreeViewList
wxWinUIFindTreeViewList(const MUX::DependencyObject& root)
{
    using winrt::Microsoft::UI::Xaml::Media::VisualTreeHelper;

    const int count = VisualTreeHelper::GetChildrenCount(root);
    for ( int i = 0; i < count; ++i )
    {
        const auto child = VisualTreeHelper::GetChild(root, i);
        if ( auto list = child.try_as<MUXC::TreeViewList>() )
            return list;
        if ( auto deeper = wxWinUIFindTreeViewList(child) )
            return deeper;
    }

    return nullptr;
}

static bool wxWinUISyncRealizedTreeItem(
    wxWinUITreeCtrlImpl *impl,
    const MUXC::TreeViewItem& container,
    const MUXC::TreeViewNode& node)
{
    if ( !impl || !container || !node )
        return false;

    try
    {
        using winrt::Microsoft::UI::Xaml::Automation::
            AutomationProperties;
        AutomationProperties::SetName(
            container, wxWinUITreeNodeName(node));

        // WinUI computes this read-only-looking template setting internally,
        // but exposes its DependencyProperty on the settings object. A local
        // value is the narrowest way to preserve the stock template and
        // virtualization while honouring wxTreeCtrl::SetIndent() exactly for
        // each realized depth.
        const double indentation =
            static_cast<double>(node.Depth()) * impl->indentDips;
        MUX::Thickness thickness{};
        if ( impl->rightToLeft )
            thickness.Right = indentation;
        else
            thickness.Left = indentation;
        const MUXC::TreeViewItemTemplateSettings settings =
            container.TreeViewItemTemplateSettings();
        settings.SetValue(
            MUXC::TreeViewItemTemplateSettings::IndentationProperty(),
            winrt::box_value(thickness));
        if ( impl->showButtons )
        {
            settings.ClearValue(
                MUXC::TreeViewItemTemplateSettings::
                    ExpandedGlyphVisibilityProperty());
            settings.ClearValue(
                MUXC::TreeViewItemTemplateSettings::
                    CollapsedGlyphVisibilityProperty());
        }
        else
        {
            const auto collapsed =
                winrt::box_value(MUX::Visibility::Collapsed);
            settings.SetValue(
                MUXC::TreeViewItemTemplateSettings::
                    ExpandedGlyphVisibilityProperty(),
                collapsed);
            settings.SetValue(
                MUXC::TreeViewItemTemplateSettings::
                    CollapsedGlyphVisibilityProperty(),
                collapsed);
        }
        return true;
    }
    catch ( const winrt::hresult_error& e )
    {
        // Container realization is a callback boundary. Never let a template
        // version rejecting the local DP value unwind through XAML.
        wxWinUILogException(
            "WinUI TreeView realized item synchronization", e);
        return false;
    }
}

// Name and indent every already-realized container.
static bool wxWinUISyncRealizedTreeItems(wxWinUITreeCtrlImpl *impl)
{
    if ( !impl || !impl->treeList || !impl->callbackState )
        return false;

    try
    {
        const std::shared_ptr<wxWinUITreeCallbackState> state =
            impl->callbackState;
        const std::uint64_t generation = state->Generation();
        const auto panel = impl->treeList.ItemsPanelRoot();
        if ( !panel )
            return false;

        const auto children = panel.Children();
        for ( uint32_t i = 0; i < children.Size(); ++i )
        {
            if ( auto item =
                     children.GetAt(i).try_as<MUXC::TreeViewItem>() )
            {
                const MUXC::TreeViewNode node =
                    impl->treeView.NodeFromContainer(item);
                wxTreeCtrl * const owner =
                    state->GetOwner(generation);
                if ( !owner || impl->callbackState != state ||
                     impl->closed )
                {
                    return false;
                }
                if ( !wxWinUISyncRealizedTreeItem(
                         impl, item, node) )
                {
                    return false;
                }
            }
        }
        return state->GetOwner(generation) != nullptr &&
               impl->callbackState == state && !impl->closed;
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException(
            "WinUI TreeView realized item enumeration", e);
        return false;
    }
}

// The generated TreeViewItem announces the data object's ToString()
// ("Microsoft.UI.Xaml.Controls.TreeViewNode") to UIA.  Name every realized
// container from the item text instead, via the inner TreeViewList's
// container pipeline so recycled containers are renamed too.
static void wxWinUIHookTreeListNaming(wxWinUITreeCtrlImpl *impl)
{
    if ( impl->treeList )
        return;

    impl->treeList = wxWinUIFindTreeViewList(impl->treeView);
    if ( !impl->treeList )
        return;

    impl->containerContentChangingToken =
        impl->treeList.ContainerContentChanging(
            [weakState =
                 std::weak_ptr<wxWinUITreeCallbackState>(
                     impl->callbackState),
             generation = impl->callbackState->Generation(),
             impl](MUXC::ListViewBase const&,
                   MUXC::ContainerContentChangingEventArgs const& args)
            {
                const std::shared_ptr<wxWinUITreeCallbackState> state =
                    weakState.lock();
                if ( !state )
                    return;
                wxTreeCtrl * const owner =
                    state->GetOwner(generation);
                if ( !owner || impl->callbackState != state ||
                     impl->closed )
                {
                    return;
                }

                const auto container =
                    args.ItemContainer().try_as<MUXC::TreeViewItem>();
                if ( !container )
                    return;
                wxWinUISyncRealizedTreeItem(
                    impl,
                    container,
                    args.Item().try_as<MUXC::TreeViewNode>());
            });

    wxWinUISyncRealizedTreeItems(impl);
}

static std::uintptr_t
wxWinUITreeNodeIdentity(const MUXC::TreeViewNode& node)
{
    return node
        ? reinterpret_cast<std::uintptr_t>(winrt::get_unknown(node))
        : 0;
}

static std::uintptr_t wxWinUIInspectableIdentity(
    const winrt::Windows::Foundation::IInspectable& value)
{
    return value
        ? reinterpret_cast<std::uintptr_t>(winrt::get_unknown(value))
        : 0;
}

static wxWinUITreeItem *
wxWinUIFindItemByNode(wxWinUITreeCtrlImpl *impl,
                      const MUXC::TreeViewNode& node)
{
    if ( !impl || !node )
        return nullptr;

#ifdef WXWINUI_TEST_SUPPORT
    ++impl->nodeLookupCount;
#endif
    const auto found = impl->itemsByNode.find(wxWinUITreeNodeIdentity(node));
    return found == impl->itemsByNode.end() ? nullptr : found->second;
}

static wxWinUITreeItem *wxWinUIFindItemByFocusedElement(
    wxWinUITreeCtrlImpl *impl,
    MUX::DependencyObject element)
{
    if ( !impl || !impl->treeView || !element )
        return nullptr;

    try
    {
        // Focus can land on the TreeViewItem itself or on any focusable
        // descendant of its template. Walk only the XAML ancestor chain and
        // resolve the first owning container through TreeView's authoritative
        // node mapping.
        for ( unsigned depth = 0; element && depth < 128; ++depth )
        {
            if ( const auto container =
                     element.try_as<MUXC::TreeViewItem>() )
            {
                return wxWinUIFindItemByNode(
                    impl, impl->treeView.NodeFromContainer(container));
            }
            if ( winrt::get_unknown(element) ==
                 winrt::get_unknown(impl->treeView) )
            {
                break;
            }
            element = MUXM::VisualTreeHelper::GetParent(element);
        }
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException(
            "WinUI TreeView focused item resolution", e);
    }
    return nullptr;
}

static wxWinUITreeItem *wxWinUIFindFocusedItem(
    wxWinUITreeCtrlImpl *impl)
{
    if ( !impl || !impl->treeView )
        return nullptr;

    try
    {
        const auto xamlRoot = impl->treeView.XamlRoot();
        if ( !xamlRoot )
            return nullptr;

        return wxWinUIFindItemByFocusedElement(
            impl,
            MUX::Input::FocusManager::GetFocusedElement(xamlRoot)
                .try_as<MUX::DependencyObject>());
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException(
            "WinUI TreeView FocusManager resolution", e);
    }
    return nullptr;
}

static size_t wxWinUIGetSubtreeCount(const wxWinUITreeItem *item)
{
    size_t count = 0;
    std::vector<const wxWinUITreeItem *> pending;
    pending.reserve(item->children.size());
    for ( const auto& child : item->children )
        pending.push_back(child.get());
    while ( !pending.empty() )
    {
        const wxWinUITreeItem * const current = pending.back();
        pending.pop_back();
        ++count;
        for ( const auto& child : current->children )
            pending.push_back(child.get());
    }
    return count;
}

static bool wxWinUIItemIsExpandable(const wxWinUITreeItem *item)
{
    return item &&
           (!item->children.empty() || item->hasChildrenOverride);
}

static bool wxWinUIItemCountsAsExpandable(const wxTreeCtrl *tree,
                                           const wxWinUITreeItem *item)
{
    return wxWinUIItemIsExpandable(item) &&
           !(tree->HasFlag(wxTR_HIDE_ROOT) && !item->parent);
}

static void wxWinUIRegisterItem(wxWinUITreeCtrlImpl *impl,
                                wxWinUITreeItem *item)
{
    wxASSERT(impl);
    wxASSERT(item);
    wxASSERT(!item->id);
    wxASSERT(!item->handle);

    impl->itemHandles.push_back(
        std::make_unique<wxWinUITreeItemHandle>());
    item->handle = impl->itemHandles.back().get();
    const std::uint64_t itemId = impl->nextItemId;
    const std::uintptr_t nodeIdentity =
        wxWinUITreeNodeIdentity(item->node);
    try
    {
        impl->itemsById.emplace(itemId, item);
        impl->liveItems.emplace(item->handle, item);
        if ( nodeIdentity )
            impl->itemsByNode.emplace(nodeIdentity, item);
    }
    catch ( ... )
    {
        impl->itemsById.erase(itemId);
        impl->liveItems.erase(item->handle);
        if ( nodeIdentity )
            impl->itemsByNode.erase(nodeIdentity);
        item->handle = nullptr;
        throw;
    }

    item->id = itemId;
    ++impl->nextItemId;
    ++impl->itemCount;
}

static void wxWinUIUnregisterItem(wxWinUITreeCtrlImpl *impl,
                                  wxWinUITreeItem *item)
{
    if ( !impl || !item || !item->id )
        return;

    impl->selectedIds.erase(item->id);
    impl->itemsById.erase(item->id);
    impl->liveItems.erase(item->handle);
    if ( item->node )
        impl->itemsByNode.erase(wxWinUITreeNodeIdentity(item->node));
    if ( item->content )
    {
        impl->itemsByContent.erase(
            wxWinUIInspectableIdentity(item->content));
    }
    if ( item->data )
        item->data->SetId(wxTreeItemId());
    item->id = 0;
    item->node = nullptr;
    item->content = nullptr;
    item->contentAttached = false;
}

static void wxWinUICollectSubtree(wxWinUITreeItem *item,
                                  std::vector<wxWinUITreeItem *>& items)
{
    if ( !item )
        return;

    std::vector<wxWinUITreeItem *> pending;
    pending.push_back(item);
    while ( !pending.empty() )
    {
        wxWinUITreeItem * const current = pending.back();
        pending.pop_back();
        items.push_back(current);
        for ( auto child = current->children.rbegin();
              child != current->children.rend();
              ++child )
        {
            pending.push_back(child->get());
        }
    }
}

// wxGenericTreeCtrl sends the delete event for each direct child before
// descending into it and sends the explicitly deleted item last.
static void wxWinUICollectDeleteOrder(wxWinUITreeItem *item,
                                      bool includeItem,
                                      std::vector<wxWinUITreeItem *>& items)
{
    if ( !item )
        return;

    std::vector<wxWinUITreeItem *> pending;
    for ( auto child = item->children.rbegin();
          child != item->children.rend();
          ++child )
    {
        pending.push_back(child->get());
    }
    while ( !pending.empty() )
    {
        wxWinUITreeItem * const current = pending.back();
        pending.pop_back();
        items.push_back(current);
        for ( auto child = current->children.rbegin();
              child != current->children.rend();
              ++child )
        {
            pending.push_back(child->get());
        }
    }

    if ( includeItem )
        items.push_back(item);
}

static bool wxWinUIIsDescendantOf(const wxWinUITreeItem *item,
                                  const wxWinUITreeItem *ancestor)
{
    for ( const wxWinUITreeItem *parent = item; parent; parent = parent->parent )
    {
        if ( parent == ancestor )
            return true;
    }

    return false;
}

static WFC::IVector<MUXC::TreeViewNode>
wxWinUIGetPeerChildren(const wxTreeCtrl *tree,
                       wxWinUITreeCtrlImpl *impl,
                       wxWinUITreeItem *parent)
{
    if ( parent )
    {
        if ( !parent->parent && tree->HasFlag(wxTR_HIDE_ROOT) )
            return impl->treeView.RootNodes();

        return parent->node.Children();
    }

    return impl->treeView.RootNodes();
}

static bool wxWinUIEnsurePeerNodeAbsent(
    const WFC::IVector<MUXC::TreeViewNode>& nodes,
    const MUXC::TreeViewNode& node,
    const char *what,
    bool injectAfterCommit = false)
{
    // A mutating WinRT collection ABI may commit and then throw. Resolve by
    // identity after every exception and retry a pre-commit failure once.
    for ( unsigned attempt = 0; attempt < 2; ++attempt )
    {
        try
        {
            uint32_t index = 0;
            if ( !nodes.IndexOf(node, index) )
                return true;
            nodes.RemoveAt(index);
            if ( injectAfterCommit )
            {
                injectAfterCommit = false;
                throw winrt::hresult_error(
                    E_FAIL,
                    L"Injected failure after TreeViewNode rollback removal");
            }
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException(what, e);
        }

        try
        {
            uint32_t index = 0;
            if ( !nodes.IndexOf(node, index) )
                return true;
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException(what, e);
        }
    }
    return false;
}

static void wxWinUIRestorePeerNodes(
    const WFC::IVector<MUXC::TreeViewNode>& nodes,
    const std::vector<MUXC::TreeViewNode>& expected)
{
    // Reconcile one position at a time instead of clearing and blindly
    // appending again. This remains safe to retry after any individual WinRT
    // mutation and cannot create duplicate peer nodes.
    for ( uint32_t pos = 0; pos < expected.size(); ++pos )
    {
        const std::uintptr_t expectedIdentity =
            wxWinUITreeNodeIdentity(expected[pos]);
        if ( pos < nodes.Size() &&
             wxWinUITreeNodeIdentity(nodes.GetAt(pos)) ==
                 expectedIdentity )
        {
            continue;
        }

        uint32_t current = pos;
        bool present = false;
        for ( ; current < nodes.Size(); ++current )
        {
            if ( wxWinUITreeNodeIdentity(nodes.GetAt(current)) ==
                 expectedIdentity )
            {
                present = true;
                break;
            }
        }
        if ( present )
            nodes.RemoveAt(current);

        if ( pos >= nodes.Size() )
            nodes.Append(expected[pos]);
        else
            nodes.InsertAt(pos, expected[pos]);
    }

    while ( nodes.Size() > expected.size() )
        nodes.RemoveAtEnd();
}

// TreeViewNode tears down and attaches its descendants recursively. Moving a
// root while it still owns a sufficiently deep subtree can therefore overflow
// the native stack even though the wx model itself is always walked
// iteratively. All structural transactions use this strong, iterative
// snapshot and make every root shallow before touching its parent vector.
struct wxWinUITreePeerNodeSnapshot
{
    MUXC::TreeViewNode node{ nullptr };
    std::vector<MUXC::TreeViewNode> children;
    bool expanded = false;
};

struct wxWinUITreePeerForestSnapshot
{
    std::vector<MUXC::TreeViewNode> roots;
    std::vector<wxWinUITreePeerNodeSnapshot> nodes;
};

static wxWinUITreePeerForestSnapshot
wxWinUICapturePeerForest(
    const WFC::IVector<MUXC::TreeViewNode>& roots,
    const std::vector<MUXC::TreeViewNode>& additionalRoots = {})
{
    wxWinUITreePeerForestSnapshot snapshot;
    snapshot.roots.reserve(roots.Size());
    for ( uint32_t i = 0; i < roots.Size(); ++i )
        snapshot.roots.push_back(roots.GetAt(i));

    std::vector<MUXC::TreeViewNode> pending;
    pending.reserve(snapshot.roots.size() + additionalRoots.size());
    for ( auto node = additionalRoots.rbegin();
          node != additionalRoots.rend();
          ++node )
    {
        if ( *node )
            pending.push_back(*node);
    }
    for ( auto node = snapshot.roots.rbegin();
          node != snapshot.roots.rend();
          ++node )
    {
        if ( *node )
            pending.push_back(*node);
    }

    std::unordered_set<std::uintptr_t> visited;
    while ( !pending.empty() )
    {
        const MUXC::TreeViewNode node = pending.back();
        pending.pop_back();
        const std::uintptr_t identity =
            wxWinUITreeNodeIdentity(node);
        if ( !identity || !visited.insert(identity).second )
            continue;

        wxWinUITreePeerNodeSnapshot nodeSnapshot;
        nodeSnapshot.node = node;
        nodeSnapshot.expanded = node.IsExpanded();
        const WFC::IVector<MUXC::TreeViewNode> children =
            node.Children();
        nodeSnapshot.children.reserve(children.Size());
        for ( uint32_t i = 0; i < children.Size(); ++i )
            nodeSnapshot.children.push_back(children.GetAt(i));
        for ( auto child = nodeSnapshot.children.rbegin();
              child != nodeSnapshot.children.rend();
              ++child )
        {
            pending.push_back(*child);
        }
        snapshot.nodes.push_back(std::move(nodeSnapshot));
    }

    return snapshot;
}

static wxWinUITreePeerForestSnapshot
wxWinUICapturePeerSubtrees(
    const std::vector<MUXC::TreeViewNode>& roots)
{
    const auto noAttachedRoots =
        winrt::single_threaded_vector<MUXC::TreeViewNode>();
    wxWinUITreePeerForestSnapshot snapshot =
        wxWinUICapturePeerForest(noAttachedRoots, roots);
    // additionalRoots only extends the traversal set in
    // wxWinUICapturePeerForest(): it is deliberately not copied into the
    // attached root vector used by style/reconciliation transactions. A
    // detached-subtree transaction, however, needs these exact nodes as its
    // rollback roots.
    snapshot.roots = roots;
    return snapshot;
}

static void wxWinUIDetachPeerForest(
    const wxWinUITreePeerForestSnapshot& snapshot)
{
    // snapshot.nodes is preorder, so reverse iteration disconnects every
    // descendant before its parent.
    for ( auto node = snapshot.nodes.rbegin();
          node != snapshot.nodes.rend();
          ++node )
    {
        node->node.Children().Clear();
    }
}

static void wxWinUIRestoreDetachedPeerForest(
    const WFC::IVector<MUXC::TreeViewNode>& roots,
    const wxWinUITreePeerForestSnapshot& snapshot)
{
    // This is deliberately idempotent. A mutating ABI call can commit and
    // still throw, so first flatten any partially restored edges, then attach
    // shallow roots and rebuild parent-to-child edges in preorder.
    wxWinUIDetachPeerForest(snapshot);
    wxWinUIRestorePeerNodes(roots, snapshot.roots);
    for ( const wxWinUITreePeerNodeSnapshot& node : snapshot.nodes )
    {
        wxWinUIRestorePeerNodes(
            node.node.Children(), node.children);
    }
    for ( const wxWinUITreePeerNodeSnapshot& node : snapshot.nodes )
        node.node.IsExpanded(node.expanded);
}

static void wxWinUIRestoreDetachedPeerSubtree(
    const WFC::IVector<MUXC::TreeViewNode>& siblings,
    uint32_t position,
    const wxWinUITreePeerForestSnapshot& snapshot)
{
    wxCHECK_RET(
        snapshot.roots.size() == 1,
        "a peer subtree rollback must have exactly one root");

    wxWinUIDetachPeerForest(snapshot);
    const MUXC::TreeViewNode root = snapshot.roots.front();
    const std::uintptr_t identity = wxWinUITreeNodeIdentity(root);
    uint32_t current = 0;
    bool present = false;
    for ( ; current < siblings.Size(); ++current )
    {
        if ( wxWinUITreeNodeIdentity(siblings.GetAt(current)) == identity )
        {
            present = true;
            break;
        }
    }
    if ( present && current != position )
    {
        siblings.RemoveAt(current);
        if ( current < position )
            --position;
        present = false;
    }
    if ( !present )
    {
        if ( position >= siblings.Size() )
            siblings.Append(root);
        else
            siblings.InsertAt(position, root);
    }

    for ( const wxWinUITreePeerNodeSnapshot& node : snapshot.nodes )
    {
        wxWinUIRestorePeerNodes(
            node.node.Children(), node.children);
    }
    for ( const wxWinUITreePeerNodeSnapshot& node : snapshot.nodes )
        node.node.IsExpanded(node.expanded);
}

static std::vector<MUXC::TreeViewNode>
wxWinUICollectModelPeerNodes(wxWinUITreeItem *root)
{
    std::vector<MUXC::TreeViewNode> nodes;
    if ( !root )
        return nodes;

    std::vector<wxWinUITreeItem *> pending;
    pending.push_back(root);
    while ( !pending.empty() )
    {
        wxWinUITreeItem * const item = pending.back();
        pending.pop_back();
        nodes.push_back(item->node);
        for ( auto child = item->children.rbegin();
              child != item->children.rend();
              ++child )
        {
            pending.push_back(child->get());
        }
    }
    return nodes;
}

static wxWinUITreePeerForestSnapshot
wxWinUIBuildModelPeerForest(wxWinUITreeCtrlImpl *impl,
                            bool hiddenRoot)
{
    wxWinUITreePeerForestSnapshot snapshot;
    if ( !impl || !impl->root )
        return snapshot;

    if ( hiddenRoot )
    {
        snapshot.roots.reserve(impl->root->children.size());
        for ( const auto& child : impl->root->children )
            snapshot.roots.push_back(child->node);
    }
    else
    {
        snapshot.roots.push_back(impl->root->node);
    }

    std::vector<wxWinUITreeItem *> pending;
    pending.push_back(impl->root.get());
    while ( !pending.empty() )
    {
        wxWinUITreeItem * const item = pending.back();
        pending.pop_back();

        wxWinUITreePeerNodeSnapshot nodeSnapshot;
        nodeSnapshot.node = item->node;
        nodeSnapshot.expanded =
            item == impl->root.get() && hiddenRoot
                ? true
                : item->expanded;
        if ( item != impl->root.get() || !hiddenRoot )
        {
            nodeSnapshot.children.reserve(item->children.size());
            for ( const auto& child : item->children )
                nodeSnapshot.children.push_back(child->node);
        }
        snapshot.nodes.push_back(std::move(nodeSnapshot));

        for ( auto child = item->children.rbegin();
              child != item->children.rend();
              ++child )
        {
            pending.push_back(child->get());
        }
    }

    return snapshot;
}

static void wxWinUIApplyPeerForest(
    const WFC::IVector<MUXC::TreeViewNode>& roots,
    const wxWinUITreePeerForestSnapshot& current,
    const wxWinUITreePeerForestSnapshot& desired)
{
    // current may contain stale peer-only nodes while desired may contain an
    // off-forest model node (notably the hidden root). Flatten both sets
    // before attaching anything.
    wxWinUIDetachPeerForest(current);
    wxWinUIDetachPeerForest(desired);
    roots.Clear();
    wxWinUIRestorePeerNodes(roots, desired.roots);
    for ( const wxWinUITreePeerNodeSnapshot& node : desired.nodes )
    {
        wxWinUIRestorePeerNodes(
            node.node.Children(), node.children);
    }
    for ( const wxWinUITreePeerNodeSnapshot& node : desired.nodes )
        node.node.IsExpanded(node.expanded);
}

static bool wxWinUIItemIsLogicallyVisible(const wxTreeCtrl *tree,
                                          const wxWinUITreeItem *item)
{
    if ( !item )
        return false;

    if ( !item->parent )
        return !tree->HasFlag(wxTR_HIDE_ROOT);

    for ( const wxWinUITreeItem *parent = item->parent; parent; parent = parent->parent )
    {
        if ( !parent->parent )
            return tree->HasFlag(wxTR_HIDE_ROOT) || parent->expanded;

        if ( !parent->expanded )
            return false;
    }

    return true;
}

static void wxWinUIReindexChildren(wxWinUITreeItem *parent,
                                   size_t first = 0)
{
    if ( !parent )
        return;

    for ( size_t i = first; i < parent->children.size(); ++i )
        parent->children[i]->siblingIndex = i;
}

static wxWinUITreeItem *wxWinUIGetNextSiblingItem(wxWinUITreeItem *item)
{
    if ( !item || !item->parent )
        return nullptr;

    const auto& siblings = item->parent->children;
    const size_t index = item->siblingIndex;
    if ( index >= siblings.size() ||
         siblings[index].get() != item )
    {
        return nullptr;
    }
    if ( index + 1 < siblings.size() )
        return siblings[index + 1].get();

    return nullptr;
}

static wxWinUITreeItem *wxWinUIGetPrevSiblingItem(wxWinUITreeItem *item)
{
    if ( !item || !item->parent )
        return nullptr;

    const auto& siblings = item->parent->children;
    const size_t index = item->siblingIndex;
    if ( index >= siblings.size() ||
         siblings[index].get() != item )
    {
        return nullptr;
    }
    if ( index > 0 )
        return siblings[index - 1].get();

    return nullptr;
}

static wxWinUITreeItem *wxWinUIGetDeepestVisibleChild(wxWinUITreeItem *item)
{
    while ( item && item->expanded && !item->children.empty() )
        item = item->children.back().get();

    return item;
}

static wxWinUITreeItem *
wxWinUIGetNextLogicalVisibleItem(wxWinUITreeItem *item, bool hiddenRoot)
{
    if ( !item )
        return nullptr;

    if ( item->expanded && !item->children.empty() )
        return item->children.front().get();

    while ( item )
    {
        if ( wxWinUITreeItem * const next =
                 wxWinUIGetNextSiblingItem(item) )
        {
            return next;
        }

        item = item->parent;
        if ( item && !item->parent && hiddenRoot )
            return nullptr;
    }
    return nullptr;
}

static wxWinUITreeItem *
wxWinUIGetPreviousLogicalVisibleItem(wxWinUITreeItem *item, bool hiddenRoot)
{
    if ( !item )
        return nullptr;

    if ( wxWinUITreeItem * const previous =
             wxWinUIGetPrevSiblingItem(item) )
    {
        return wxWinUIGetDeepestVisibleChild(previous);
    }

    if ( item->parent &&
         (!hiddenRoot || item->parent->parent) )
    {
        return item->parent;
    }
    return nullptr;
}

wxTreeCtrl::wxTreeCtrl()
{
}

wxTreeCtrl::wxTreeCtrl(wxWindow *parent,
                       wxWindowID id,
                       const wxPoint& pos,
                       const wxSize& size,
                       long style,
                       const wxValidator& validator,
                       const wxString& name)
{
    Create(parent, id, pos, size, style, validator, name);
}

wxTreeCtrl::~wxTreeCtrl()
{
    Unbind(wxEVT_DPI_CHANGED, &wxTreeCtrl::OnDPIChanged, this);

    // Child HWND destruction owns the edit control. Clear our observer before
    // invalidating the peer so a focus-loss event cannot commit a dying item.
    m_editControl = nullptr;
    if ( m_winui )
        m_winui->Close();
}

bool wxTreeCtrl::Create(wxWindow *parent,
                        wxWindowID id,
                        const wxPoint& pos,
                        const wxSize& size,
                        long style,
                        const wxValidator& validator,
                        const wxString& name)
{
    style = (style & ~wxBORDER_MASK) | wxBORDER_NONE;

    if ( !wxControl::Create(parent, id, pos, size, style, validator, name) )
        return false;

    m_winui.reset(new wxWinUITreeCtrlImpl);
    // SetStateImages() is valid before two-step Create(). Keep the public
    // wxWithImages storage authoritative and seed the peer implementation
    // from it instead of requiring a post-Create setter call.
    m_winui->stateImageBundles = m_winuiStateImageBundles;
    if ( !m_winui->host.Initialize(this) )
    {
        m_winui->Close();
        m_winui.reset();
        return false;
    }

    m_winui->callbackState =
        std::make_shared<wxWinUITreeCallbackState>(this);
    wxWinUITreeCtrlImpl * const createImpl = m_winui.get();
    const std::shared_ptr<wxWinUITreeCallbackState> callbackState =
        createImpl->callbackState;
    const std::uint64_t callbackGeneration =
        callbackState->Generation();
    const wxLayoutDirection initialDirection =
        GetLayoutDirection();
    if ( callbackState->GetOwner(callbackGeneration) != this ||
         !m_winui || m_winui.get() != createImpl )
    {
        return false;
    }
    createImpl->indentDips = static_cast<double>(m_indent);
    createImpl->rightToLeft =
        initialDirection == wxLayout_RightToLeft;
    createImpl->showButtons = HasFlag(wxTR_HAS_BUTTONS);

    try
    {
        m_winui->treeView = MUXC::TreeView();
        m_winui->treeView.FlowDirection(
            createImpl->rightToLeft
                ? MUX::FlowDirection::RightToLeft
                : MUX::FlowDirection::LeftToRight);
        m_winui->treeView.SelectionMode(HasFlag(wxTR_MULTIPLE)
            ? MUXC::TreeViewSelectionMode::Multiple
            : MUXC::TreeViewSelectionMode::Single);

        // The Image is collapsed for items without an icon, otherwise an
        // always-present 16px image reserves a blank gutter in front of every
        // label in a tree that has no image list at all.  The visibility comes
        // from an explicit value put in the item's property set: WinUI has no
        // implicit null-to-Visibility conversion for a classic Binding.
        // The Margin binding reclaims the expander gutter that a TreeViewItem
        // always reserves, even for items that can't be expanded: without it a
        // tree that has no expandable item at all (a plain list, as used for
        // navigation panes) shows every label pushed right by an empty column.
        const wchar_t *itemTemplate =
            LR"(<DataTemplate xmlns="http://schemas.microsoft.com/winfx/2006/xaml/presentation"
                              xmlns:x="http://schemas.microsoft.com/winfx/2006/xaml">
                    <Border x:Name="wxTreeContentBorder"
                            Background="{Binding Content[Background]}"
                            Margin="{Binding Content[ContentMargin]}">
                      <StackPanel x:Name="wxTreeContent"
                                  Orientation="Horizontal" Spacing="8">
                        <Image x:Name="wxTreeStateImage"
                               VerticalAlignment="Center"
                               Visibility="{Binding Content[StateImageVisibility]}"
                               Width="{Binding Content[StateImageWidth]}"
                               Height="{Binding Content[StateImageHeight]}"
                               Source="{Binding Content[StateImage]}"/>
                        <Image x:Name="wxTreeImage"
                               VerticalAlignment="Center"
                               Visibility="{Binding Content[ImageVisibility]}"
                               Width="{Binding Content[ImageWidth]}"
                               Height="{Binding Content[ImageHeight]}"
                               Source="{Binding Content[Image]}"/>
                        <TextBlock x:Name="wxTreeLabel"
                                   VerticalAlignment="Center"
                                   Text="{Binding Content[Text]}"
                                   Foreground="{Binding Content[Foreground]}"
                                   FontFamily="{Binding Content[FontFamily]}"
                                   FontSize="{Binding Content[FontSize]}"
                                   FontStyle="{Binding Content[FontStyle]}"
                                   FontWeight="{Binding Content[FontWeight]}"/>
                      </StackPanel>
                    </Border>
                </DataTemplate>)";
        m_winui->treeView.ItemTemplate(
            winrt::Microsoft::UI::Xaml::Markup::XamlReader::Load(itemTemplate)
                .as<MUX::DataTemplate>());

        // Hook the container-naming pipeline before anything can be
        // realized: force the template (the inner TreeViewList only exists
        // once it is applied), and keep a Loaded retry for the case where
        // the template really cannot resolve this early.
        const std::weak_ptr<wxWinUITreeCallbackState> weakCallbackState =
            callbackState;

        m_winui->treeView.ApplyTemplate();
        wxWinUIHookTreeListNaming(m_winui.get());
        if ( !m_winui->treeList )
        {
            m_winui->listHookLoadedToken = m_winui->treeView.Loaded(
                [weakCallbackState, callbackGeneration](
                    winrt::Windows::Foundation::IInspectable const&,
                    MUX::RoutedEventArgs const&)
                {
                    const auto state = weakCallbackState.lock();
                    wxTreeCtrl * const owner =
                        state ? state->GetOwner(callbackGeneration) : nullptr;
                    if ( owner && owner->m_winui &&
                         owner->m_winui->treeView )
                    {
                        wxWinUIHookTreeListNaming(owner->m_winui.get());
                    }
                });
        }

        m_winui->selectionChangedToken = m_winui->treeView.SelectionChanged(
            [weakCallbackState, callbackGeneration](
                MUXC::TreeView const&,
                MUXC::TreeViewSelectionChangedEventArgs const&)
            {
                const auto state = weakCallbackState.lock();
                if ( !state || state->IsPeerMutationInProgress() )
                    return;

                if ( wxTreeCtrl * const owner =
                         state->GetOwner(callbackGeneration) )
                {
                    owner->OnPeerSelectionChanged();
                }
            });

        m_winui->gotFocusToken = m_winui->treeView.GotFocus(
            [weakCallbackState, callbackGeneration](
                winrt::Windows::Foundation::IInspectable const&,
                MUX::RoutedEventArgs const& event)
            {
                const auto state = weakCallbackState.lock();
                wxTreeCtrl * const owner =
                    state ? state->GetOwner(callbackGeneration) : nullptr;
                if ( !owner || !owner->m_winui )
                    return;

                wxWinUITreeCtrlImpl * const impl = owner->m_winui.get();
                wxWinUITreeItem * const focused =
                    wxWinUIFindItemByFocusedElement(
                        impl,
                        event.OriginalSource()
                            .try_as<MUX::DependencyObject>());
                if ( state->GetOwner(callbackGeneration) == owner &&
                     owner->m_winui &&
                     owner->m_winui.get() == impl &&
                     focused && !focused->deleting &&
                     owner->ResolveItem(focused->id) == focused )
                {
                    impl->focusedId = focused->id;
                }
            });

        m_winui->expandingToken = m_winui->treeView.Expanding(
            [weakCallbackState, callbackGeneration](
                MUXC::TreeView const&,
                MUXC::TreeViewExpandingEventArgs const& event)
            {
                const auto state = weakCallbackState.lock();
                if ( !state || state->IsPeerMutationInProgress() )
                    return;

                wxTreeCtrl *owner =
                    state->GetOwner(callbackGeneration);
                if ( owner && owner->m_winui )
                {
                    owner->OnPeerNodeExpanded(
                        wxWinUIFindItemByNode(
                            owner->m_winui.get(), event.Node()));
                }
            });

        m_winui->collapsedToken = m_winui->treeView.Collapsed(
            [weakCallbackState, callbackGeneration](
                MUXC::TreeView const&,
                MUXC::TreeViewCollapsedEventArgs const& event)
            {
                const auto state = weakCallbackState.lock();
                if ( !state || state->IsPeerMutationInProgress() )
                    return;

                wxTreeCtrl * const owner =
                    state->GetOwner(callbackGeneration);
                if ( owner && owner->m_winui )
                {
                    owner->OnPeerNodeCollapsed(
                        wxWinUIFindItemByNode(
                            owner->m_winui.get(), event.Node()));
                }
            });

        m_winui->itemInvokedToken = m_winui->treeView.ItemInvoked(
            [weakCallbackState, callbackGeneration](
                MUXC::TreeView const&,
                MUXC::TreeViewItemInvokedEventArgs const& event)
            {
                const auto state = weakCallbackState.lock();
                wxTreeCtrl *owner =
                    state ? state->GetOwner(callbackGeneration) : nullptr;
                if ( !owner || !owner->m_winui )
                    return;

                const auto invoked = event.InvokedItem();
                wxWinUITreeItem *item = wxWinUIFindItemByNode(
                    owner->m_winui.get(),
                    invoked.try_as<MUXC::TreeViewNode>());
                if ( !item )
                {
                    const auto found =
                        owner->m_winui->itemsByContent.find(
                            wxWinUIInspectableIdentity(invoked));
                    if ( found !=
                         owner->m_winui->itemsByContent.end() )
                    {
                        item = found->second;
                    }
                }
                if ( !item )
                {
                    item = owner->ResolveItem(
                        owner->m_winui->selectionId);
                }
                if ( item && !item->deleting )
                    owner->OnPeerItemInvoked(item);
            });

        m_winui->pointerPressedToken =
            m_winui->treeView.PointerPressed(
                [weakCallbackState, callbackGeneration](
                    winrt::Windows::Foundation::IInspectable const&,
                    MUX::Input::PointerRoutedEventArgs const& event)
                {
                    const auto state = weakCallbackState.lock();
                    wxTreeCtrl *owner =
                        state ? state->GetOwner(callbackGeneration) : nullptr;
                    if ( !owner || !owner->m_winui ||
                         !owner->m_winui->treeView )
                    {
                        return;
                    }

                    try
                    {
                        wxWinUITreeCtrlImpl * const callbackImpl =
                            owner->m_winui.get();
                        const MUXC::TreeView peer =
                            callbackImpl->treeView;
                        const auto pointer =
                            event.GetCurrentPoint(peer);
                        if ( !pointer.Properties().IsLeftButtonPressed() )
                            return;
                        const auto position = pointer.Position();
                        owner = state->GetOwner(callbackGeneration);
                        if ( !owner || !owner->m_winui ||
                             owner->m_winui.get() != callbackImpl ||
                             owner->m_winui->callbackState != state ||
                             wxWinUIInspectableIdentity(
                                 owner->m_winui->treeView) !=
                                 wxWinUIInspectableIdentity(peer) )
                        {
                            return;
                        }
                        winrt::Windows::Foundation::Point clientPosition{};
                        if ( wxWinUIVisualCoordinates::ElementPointToClient(
                                 owner,
                                 peer,
                                 position,
                                 &clientPosition) !=
                             wxWinUICoordinateResult::Mapped )
                        {
                            return;
                        }
                        owner = state->GetOwner(callbackGeneration);
                        if ( !owner || !owner->m_winui ||
                             owner->m_winui.get() != callbackImpl ||
                             owner->m_winui->callbackState != state ||
                             wxWinUIInspectableIdentity(
                                 owner->m_winui->treeView) !=
                                 wxWinUIInspectableIdentity(peer) )
                        {
                            return;
                        }
                        const wxPoint point(
                            static_cast<int>(
                                std::lround(clientPosition.X)),
                            static_cast<int>(
                                std::lround(clientPosition.Y)));
                        owner->OnPeerPointerPressed(point);
                    }
                    catch ( const winrt::hresult_error& e )
                    {
                        wxWinUILogException(
                            "WinUI TreeView pointer press", e);
                    }
                });

        m_winui->pointerMovedToken =
            m_winui->treeView.PointerMoved(
                [weakCallbackState, callbackGeneration](
                    winrt::Windows::Foundation::IInspectable const&,
                    MUX::Input::PointerRoutedEventArgs const& event)
                {
                    const auto state = weakCallbackState.lock();
                    wxTreeCtrl *owner =
                        state ? state->GetOwner(callbackGeneration) : nullptr;
                    if ( !owner || !owner->m_winui ||
                         (!owner->m_winui->clickEditCandidateId &&
                          !owner->m_winui->pendingLabelEditId &&
                          !owner->m_winui->pressedStateImageId) )
                    {
                        return;
                    }

                    try
                    {
                        wxWinUITreeCtrlImpl * const callbackImpl =
                            owner->m_winui.get();
                        const MUXC::TreeView peer =
                            callbackImpl->treeView;
                        const auto position =
                            event.GetCurrentPoint(peer).Position();
                        owner = state->GetOwner(callbackGeneration);
                        if ( !owner || !owner->m_winui ||
                             owner->m_winui.get() != callbackImpl ||
                             owner->m_winui->callbackState != state ||
                             wxWinUIInspectableIdentity(
                                 owner->m_winui->treeView) !=
                                 wxWinUIInspectableIdentity(peer) )
                        {
                            return;
                        }
                        winrt::Windows::Foundation::Point clientPosition{};
                        if ( wxWinUIVisualCoordinates::ElementPointToClient(
                                 owner,
                                 peer,
                                 position,
                                 &clientPosition) !=
                             wxWinUICoordinateResult::Mapped )
                        {
                            return;
                        }
                        owner = state->GetOwner(callbackGeneration);
                        if ( !owner || !owner->m_winui ||
                             owner->m_winui.get() != callbackImpl ||
                             owner->m_winui->callbackState != state ||
                             wxWinUIInspectableIdentity(
                                 owner->m_winui->treeView) !=
                                 wxWinUIInspectableIdentity(peer) )
                        {
                            return;
                        }
                        const wxPoint point(
                            static_cast<int>(
                                std::lround(clientPosition.X)),
                            static_cast<int>(
                                std::lround(clientPosition.Y)));
                        owner->OnPeerPointerMoved(point);
                    }
                    catch ( const winrt::hresult_error& e )
                    {
                        wxWinUILogException(
                            "WinUI TreeView pointer move", e);
                    }
                });

        m_winui->pointerReleasedToken =
            m_winui->treeView.PointerReleased(
                [weakCallbackState, callbackGeneration](
                    winrt::Windows::Foundation::IInspectable const&,
                    MUX::Input::PointerRoutedEventArgs const& event)
                {
                    const auto state = weakCallbackState.lock();
                    wxTreeCtrl *owner =
                        state ? state->GetOwner(callbackGeneration) : nullptr;
                    if ( !owner || !owner->m_winui )
                        return;

                    try
                    {
                        wxWinUITreeCtrlImpl * const callbackImpl =
                            owner->m_winui.get();
                        const MUXC::TreeView peer =
                            callbackImpl->treeView;
                        const auto position =
                            event.GetCurrentPoint(peer).Position();
                        owner = state->GetOwner(callbackGeneration);
                        if ( !owner || !owner->m_winui ||
                             owner->m_winui.get() != callbackImpl ||
                             owner->m_winui->callbackState != state ||
                             wxWinUIInspectableIdentity(
                                 owner->m_winui->treeView) !=
                                 wxWinUIInspectableIdentity(peer) )
                        {
                            return;
                        }
                        winrt::Windows::Foundation::Point clientPosition{};
                        if ( wxWinUIVisualCoordinates::ElementPointToClient(
                                 owner,
                                 peer,
                                 position,
                                 &clientPosition) !=
                             wxWinUICoordinateResult::Mapped )
                        {
                            return;
                        }
                        owner = state->GetOwner(callbackGeneration);
                        if ( !owner || !owner->m_winui ||
                             owner->m_winui.get() != callbackImpl ||
                             owner->m_winui->callbackState != state ||
                             wxWinUIInspectableIdentity(
                                 owner->m_winui->treeView) !=
                                 wxWinUIInspectableIdentity(peer) )
                        {
                            return;
                        }
                        const wxPoint point(
                            static_cast<int>(
                                std::lround(clientPosition.X)),
                            static_cast<int>(
                                std::lround(clientPosition.Y)));
                        owner->OnPeerPointerReleased(point);
                    }
                    catch ( const winrt::hresult_error& e )
                    {
                        wxWinUILogException(
                            "WinUI TreeView pointer release", e);
                    }
                });

        m_winui->doubleTappedToken =
            m_winui->treeView.DoubleTapped(
                [weakCallbackState, callbackGeneration](
                    winrt::Windows::Foundation::IInspectable const&,
                    MUX::Input::DoubleTappedRoutedEventArgs const& event)
                {
                    const auto state = weakCallbackState.lock();
                    wxTreeCtrl *owner =
                        state
                            ? state->GetOwner(callbackGeneration)
                            : nullptr;
                    if ( !owner || !owner->m_winui ||
                         !owner->m_winui->treeView )
                    {
                        return;
                    }

                    try
                    {
                        wxWinUITreeCtrlImpl * const callbackImpl =
                            owner->m_winui.get();
                        const MUXC::TreeView peer =
                            callbackImpl->treeView;
                        const auto position =
                            event.GetPosition(peer);
                        owner = state->GetOwner(callbackGeneration);
                        if ( !owner || !owner->m_winui ||
                             owner->m_winui.get() != callbackImpl ||
                             owner->m_winui->callbackState != state ||
                             wxWinUIInspectableIdentity(
                                 owner->m_winui->treeView) !=
                                 wxWinUIInspectableIdentity(peer) )
                        {
                            return;
                        }
                        winrt::Windows::Foundation::Point clientPosition{};
                        if ( wxWinUIVisualCoordinates::ElementPointToClient(
                                 owner,
                                 peer,
                                 position,
                                 &clientPosition) !=
                             wxWinUICoordinateResult::Mapped )
                        {
                            return;
                        }
                        owner = state->GetOwner(callbackGeneration);
                        if ( !owner || !owner->m_winui ||
                             owner->m_winui.get() != callbackImpl ||
                             owner->m_winui->callbackState != state ||
                             wxWinUIInspectableIdentity(
                                 owner->m_winui->treeView) !=
                                 wxWinUIInspectableIdentity(peer) )
                        {
                            return;
                        }
                        const wxPoint point(
                            static_cast<int>(
                                std::lround(clientPosition.X)),
                            static_cast<int>(
                                std::lround(clientPosition.Y)));
                        owner->OnPeerDoubleTapped(point);
                    }
                    catch ( const winrt::hresult_error& e )
                    {
                        wxWinUILogException(
                            "WinUI TreeView double tap", e);
                    }
                });

        m_winui->lostFocusToken =
            m_winui->treeView.LostFocus(
                [weakCallbackState, callbackGeneration](
                    winrt::Windows::Foundation::IInspectable const&,
                    MUX::RoutedEventArgs const&)
                {
                    const auto state = weakCallbackState.lock();
                    if ( wxTreeCtrl * const owner =
                             state
                                 ? state->GetOwner(callbackGeneration)
                                 : nullptr )
                    {
                        owner->CancelDelayedLabelEdit();
                    }
                });

        // Item dragging: wx semantics are that the application performs the
        // actual move itself from the END_DRAG handler, so keep the WinUI
        // automatic reordering off and only report the gesture.
        m_winui->treeView.CanDragItems(true);
        m_winui->treeView.CanReorderItems(false);

        m_winui->dragStartingToken = m_winui->treeView.DragItemsStarting(
            [weakCallbackState, callbackGeneration](
                MUXC::TreeView const&,
                MUXC::TreeViewDragItemsStartingEventArgs const& args)
            {
                const auto state = weakCallbackState.lock();
                wxTreeCtrl *owner =
                    state ? state->GetOwner(callbackGeneration) : nullptr;
                if ( !owner || !owner->m_winui )
                    return;

                std::uint64_t itemId = 0;
                const auto items = args.Items();
                owner = state->GetOwner(callbackGeneration);
                if ( !owner || !owner->m_winui )
                    return;
                const uint32_t itemCount = items.Size();
                owner = state->GetOwner(callbackGeneration);
                if ( !owner || !owner->m_winui )
                    return;
                if ( itemCount )
                {
                    const auto dragged = items.GetAt(0);
                    owner = state->GetOwner(callbackGeneration);
                    if ( !owner || !owner->m_winui )
                        return;
                    const MUXC::TreeViewNode draggedNode =
                        dragged.try_as<MUXC::TreeViewNode>();
                    owner = state->GetOwner(callbackGeneration);
                    if ( !owner || !owner->m_winui )
                        return;
                    wxWinUITreeItem *item =
                        wxWinUIFindItemByNode(
                            owner->m_winui.get(), draggedNode);
                    if ( !item )
                    {
                        const std::uintptr_t draggedIdentity =
                            wxWinUIInspectableIdentity(dragged);
                        owner = state->GetOwner(callbackGeneration);
                        if ( !owner || !owner->m_winui )
                            return;
                        const auto found =
                            owner->m_winui->itemsByContent.find(
                                draggedIdentity);
                        if ( found !=
                             owner->m_winui->itemsByContent.end() )
                        {
                            item = found->second;
                        }
                    }
                    itemId = item ? item->id : 0;
                }

                const wxPoint point =
                    owner->ScreenToClient(wxGetMousePosition());
                owner = state->GetOwner(callbackGeneration);
                if ( !owner || !owner->m_winui )
                    return;
                wxWinUITreeItem *item = itemId
                    ? owner->ResolveItem(itemId)
                    : owner->ResolveItem(owner->m_winui->selectionId);
                const bool started =
                    owner->HandlePeerDragStarting(item, point);
                if ( !started )
                {
                    args.Cancel(true);
                }
            });

        m_winui->dragCompletedToken = m_winui->treeView.DragItemsCompleted(
            [weakCallbackState, callbackGeneration](
                MUXC::TreeView const&,
                MUXC::TreeViewDragItemsCompletedEventArgs const& args)
            {
                const auto state = weakCallbackState.lock();
                wxTreeCtrl *owner =
                    state ? state->GetOwner(callbackGeneration) : nullptr;
                if ( !owner || !owner->m_winui ||
                     !owner->m_winui->dragItemId )
                {
                    return;
                }

                const wxPoint pt =
                    owner->ScreenToClient(wxGetMousePosition());
                owner = state->GetOwner(callbackGeneration);
                if ( !owner || !owner->m_winui ||
                     !owner->m_winui->dragItemId )
                {
                    return;
                }
                int flags = 0;
                const wxTreeItemId target =
                    owner->DoTreeHitTest(pt, flags);
                owner = state->GetOwner(callbackGeneration);
                if ( !owner || !owner->m_winui ||
                     !owner->m_winui->dragItemId )
                {
                    return;
                }
                const auto dropResult = args.DropResult();
                owner = state->GetOwner(callbackGeneration);
                if ( !owner || !owner->m_winui ||
                     !owner->m_winui->dragItemId )
                {
                    return;
                }
                owner->HandlePeerDragCompleted(
                    owner->GetItem(target),
                    pt,
                    dropResult ==
                        winrt::Windows::ApplicationModel::DataTransfer::
                            DataPackageOperation::None);
            });

        m_winui->keyDownToken = m_winui->treeView.KeyDown(
            [weakCallbackState, callbackGeneration](
                winrt::Windows::Foundation::IInspectable const&,
                MUX::Input::KeyRoutedEventArgs const& event)
            {
                const auto state = weakCallbackState.lock();
                wxTreeCtrl *owner =
                    state ? state->GetOwner(callbackGeneration) : nullptr;
                if ( !owner )
                    return;

                wchar_t uc = 0;
                const int code = wxMSWKeyboard::VKToWX(
                    static_cast<WXWORD>(event.Key()), 0, &uc);
                if ( code == WXK_NONE )
                    return;

                if ( owner->OnPeerKeyDown(code, uc) )
                    event.Handled(true);
            });

        m_winui->rightTappedToken = m_winui->treeView.RightTapped(
            [weakCallbackState, callbackGeneration](
                winrt::Windows::Foundation::IInspectable const&,
                MUX::Input::RightTappedRoutedEventArgs const& event)
            {
                const auto state = weakCallbackState.lock();
                wxTreeCtrl *owner =
                    state ? state->GetOwner(callbackGeneration) : nullptr;
                if ( !owner || !owner->m_winui ||
                     !owner->m_winui->treeView )
                {
                    return;
                }

                wxWinUITreeCtrlImpl * const callbackImpl =
                    owner->m_winui.get();
                const MUXC::TreeView treeView =
                    callbackImpl->treeView;
                const auto pos = event.GetPosition(treeView);
                owner = state->GetOwner(callbackGeneration);
                if ( !owner || !owner->m_winui ||
                     owner->m_winui.get() != callbackImpl ||
                     owner->m_winui->callbackState != state ||
                     wxWinUIInspectableIdentity(owner->m_winui->treeView) !=
                         wxWinUIInspectableIdentity(treeView) )
                {
                    return;
                }
                winrt::Windows::Foundation::Point clientPosition{};
                if ( wxWinUIVisualCoordinates::ElementPointToClient(
                         owner,
                         treeView,
                         pos,
                         &clientPosition) !=
                     wxWinUICoordinateResult::Mapped )
                {
                    return;
                }
                owner = state->GetOwner(callbackGeneration);
                if ( !owner || !owner->m_winui ||
                     owner->m_winui.get() != callbackImpl ||
                     owner->m_winui->callbackState != state ||
                     wxWinUIInspectableIdentity(owner->m_winui->treeView) !=
                         wxWinUIInspectableIdentity(treeView) )
                {
                    return;
                }
                const wxPoint point(
                    static_cast<int>(std::lround(clientPosition.X)),
                    static_cast<int>(std::lround(clientPosition.Y)));
                owner->OnPeerRightTapped(point);
            });

        const bool contentSet =
            createImpl->host.SetContent(createImpl->treeView);
        wxTreeCtrl * const owner =
            callbackState->GetOwner(callbackGeneration);
        if ( !owner || !owner->m_winui ||
             owner->m_winui.get() != createImpl )
        {
            return false;
        }
        if ( !contentSet )
        {
            owner->m_winui.reset();
            return false;
        }
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI TreeView creation", e);
        wxTreeCtrl * const owner =
            callbackState->GetOwner(callbackGeneration);
        if ( owner && owner->m_winui &&
             owner->m_winui.get() == createImpl )
        {
            owner->m_winui.reset();
        }
        return false;
    }

    wxTreeCtrl *owner =
        callbackState->GetOwner(callbackGeneration);
    if ( !owner || !owner->m_winui ||
         owner->m_winui.get() != createImpl )
    {
        return false;
    }

    owner->SetInitialSize(size);
    owner = callbackState->GetOwner(callbackGeneration);
    if ( !owner || !owner->m_winui ||
         owner->m_winui.get() != createImpl )
    {
        return false;
    }
    owner->Unbind(
        wxEVT_DPI_CHANGED, &wxTreeCtrl::OnDPIChanged, owner);
    owner->Bind(
        wxEVT_DPI_CHANGED, &wxTreeCtrl::OnDPIChanged, owner);
    return true;
}

#ifdef WXWINUI_TEST_SUPPORT
void wxWinUITreeCtrlTestAccess::FailNextPeerMutation(wxTreeCtrl &control,
                                                     PeerMutation mutation)
{
    wxTreeCtrl *const self = &control;
    FailPeerMutations(*self, mutation, 1);
}
#endif // WXWINUI_TEST_SUPPORT

#ifdef WXWINUI_TEST_SUPPORT
void wxWinUITreeCtrlTestAccess::FailPeerMutations(wxTreeCtrl &control,
                                                  PeerMutation mutation,
                                                  unsigned count)
{
    wxTreeCtrl *const self = &control;
    wxCHECK_RET(self->m_winui && !self->m_winui->closed,
                "uninitialized wxTreeCtrl test seam");
    self->m_winui->failedMutation = mutation;
    self->m_winui->failMutationCount = count;
}
#endif // WXWINUI_TEST_SUPPORT

#ifdef WXWINUI_TEST_SUPPORT
bool wxWinUITreeCtrlTestAccess::SelectPeerItem(wxTreeCtrl &control,
                                               const wxTreeItemId &item,
                                               bool addToSelection)
{
    wxTreeCtrl *const self = &control;
    wxWinUITreeItem *const treeItem = self->GetItem(item);
    if ( !self->m_winui || !self->m_winui->treeView ||
         (item.IsOk() && !treeItem) )
    {
        return false;
    }

    const std::shared_ptr<wxWinUITreeCallbackState> state =
        self->m_winui->callbackState;
    const std::uint64_t generation = state->Generation();
    try
    {
        {
            // Suppress the real SelectionChanged callback while setting up
            // the requested peer state. Invoke the wx path exactly once
            // below, avoiding the duplicate dispatch that a synchronous
            // XAML SelectionChanged notification would otherwise cause.
            wxWinUITreePeerMutationGuard mutation(self->m_winui->callbackState);
            if ( self->HasFlag(wxTR_MULTIPLE) )
            {
                auto selected = self->m_winui->treeView.SelectedNodes();
                if ( !addToSelection )
                    selected.Clear();
                if ( treeItem )
                {
                    bool alreadySelected = false;
                    if ( addToSelection )
                    {
                        for ( const MUXC::TreeViewNode& selectedNode :
                              selected )
                        {
                            if ( wxWinUITreeNodeIdentity(selectedNode) ==
                                 wxWinUITreeNodeIdentity(treeItem->node) )
                            {
                                alreadySelected = true;
                                break;
                            }
                        }
                    }
                    if ( !alreadySelected )
                        selected.Append(treeItem->node);
                }
            }
            else
            {
                self->m_winui->treeView.SelectedNode(
                    treeItem ? treeItem->node : MUXC::TreeViewNode{nullptr});
            }
        }
        if ( state->GetOwner(generation) != self )
            return false;
        self->OnPeerSelectionChanged();
        return state->GetOwner(generation) == self;
    }
    catch ( const winrt::hresult_error& )
    {
        return false;
    }
}
#endif // WXWINUI_TEST_SUPPORT

#ifdef WXWINUI_TEST_SUPPORT
bool wxWinUITreeCtrlTestAccess::FocusPeerItem(wxTreeCtrl &control,
                                              const wxTreeItemId &item)
{
    wxTreeCtrl *const self = &control;
    wxWinUITreeItem *treeItem = self->GetItem(item);
    if ( !treeItem || !self->m_winui || !self->m_winui->treeView )
        return false;

    const std::shared_ptr<wxWinUITreeCallbackState> state =
        self->m_winui->callbackState;
    const std::uint64_t generation = state->Generation();
    const std::uint64_t itemId = treeItem->id;
    try
    {
        self->EnsureVisible(item);
        wxTreeCtrl *owner = state->GetOwner(generation);
        treeItem = owner == self ? owner->ResolveItem(itemId) : nullptr;
        if ( !treeItem || !owner->m_winui )
            return false;

        owner->m_winui->host.ForceRender();
        wxWinUIHookTreeListNaming(owner->m_winui.get());
        if ( owner->m_winui->treeList )
            owner->m_winui->treeList.ScrollIntoView(treeItem->node);
        owner = state->GetOwner(generation);
        treeItem = owner == self ? owner->ResolveItem(itemId) : nullptr;
        if ( !treeItem || !owner->m_winui )
            return false;
        owner->m_winui->treeView.UpdateLayout();
        owner->m_winui->host.ForceRender();

        owner = state->GetOwner(generation);
        treeItem = owner == self ? owner->ResolveItem(itemId) : nullptr;
        if ( !treeItem || !owner->m_winui )
            return false;
        const auto container =
            owner->m_winui->treeView.ContainerFromNode(treeItem->node)
                .try_as<MUXC::Control>();
        if ( !container )
            return false;

        // A false return is not necessarily failure: XAML can retain only
        // logical focus or decide no physical focus transition was needed.
        // The authoritative postcondition is FocusManager's current element.
        (void)container.Focus(MUX::FocusState::Programmatic);

        owner = state->GetOwner(generation);
        if ( owner != self || !owner->m_winui )
            return false;
        wxWinUITreeItem * const focused =
            wxWinUIFindFocusedItem(owner->m_winui.get());
        if ( !focused || focused->deleting ||
             owner->ResolveItem(focused->id) != focused )
        {
            return false;
        }
        owner->m_winui->focusedId = focused->id;
        return owner == self && owner->m_winui &&
               owner->m_winui->focusedId == itemId;
    }
    catch ( const winrt::hresult_error& )
    {
        return false;
    }
}
#endif // WXWINUI_TEST_SUPPORT

#ifdef WXWINUI_TEST_SUPPORT
bool wxWinUITreeCtrlTestAccess::SetPeerExpanded(wxTreeCtrl &control,
                                                const wxTreeItemId &item,
                                                bool expanded)
{
    wxTreeCtrl *const self = &control;
    wxWinUITreeItem *const treeItem = self->GetItem(item);
    if ( !treeItem || !treeItem->node )
        return false;

    const std::uint64_t itemId = treeItem->id;
    const std::shared_ptr<wxWinUITreeCallbackState> state =
        self->m_winui->callbackState;
    const std::uint64_t generation = state->Generation();
    try
    {
        {
            // As for the selection seam above, suppress the synchronous XAML
            // notification and exercise the wx callback path exactly once.
            wxWinUITreePeerMutationGuard mutation(self->m_winui->callbackState);
            treeItem->node.IsExpanded(expanded);
        }
        if ( state->GetOwner(generation) != self )
            return false;

        if ( expanded )
            self->OnPeerNodeExpanded(treeItem);
        else
            self->OnPeerNodeCollapsed(treeItem);

        if ( state->GetOwner(generation) != self )
            return false;
        wxWinUITreeItem *const liveItem = self->ResolveItem(itemId);
        return liveItem && liveItem->expanded == expanded;
    }
    catch ( const winrt::hresult_error& )
    {
        return false;
    }
}
#endif // WXWINUI_TEST_SUPPORT

#ifdef WXWINUI_TEST_SUPPORT
bool wxWinUITreeCtrlTestAccess::IsPeerExpanded(const wxTreeCtrl &control,
                                               const wxTreeItemId &item)
{
    const wxTreeCtrl *const self = &control;
    wxWinUITreeItem *const treeItem = self->GetItem(item);
    if ( !treeItem || !treeItem->node )
        return false;
    try
    {
        return treeItem->node.IsExpanded();
    }
    catch ( const winrt::hresult_error& )
    {
        return false;
    }
}
#endif // WXWINUI_TEST_SUPPORT

#ifdef WXWINUI_TEST_SUPPORT
bool wxWinUITreeCtrlTestAccess::KeyDown(wxTreeCtrl &control, int keyCode,
                                        bool controlDown, bool shiftDown,
                                        bool altDown)
{
    wxTreeCtrl *const self = &control;
    return self->OnPeerKeyDown(keyCode, 0, controlDown, shiftDown, altDown,
                               true);
}
#endif // WXWINUI_TEST_SUPPORT

#ifdef WXWINUI_TEST_SUPPORT
bool wxWinUITreeCtrlTestAccess::BeginInternalDrag(wxTreeCtrl &control,
                                                  const wxTreeItemId &item)
{
    wxTreeCtrl *const self = &control;
    return self->HandlePeerDragStarting(self->GetItem(item), wxPoint());
}
#endif // WXWINUI_TEST_SUPPORT

#ifdef WXWINUI_TEST_SUPPORT
bool wxWinUITreeCtrlTestAccess::CompleteInternalDrag(wxTreeCtrl &control,
                                                     const wxTreeItemId &target,
                                                     DragCompletion completion)
{
    wxTreeCtrl *const self = &control;
    if ( !self->m_winui || !self->m_winui->dragItemId )
        return false;

    self->CompleteInternalDrag(self->GetItem(target), wxPoint(),
                               completion == DragCompletion::Cancel);
    return true;
}
#endif // WXWINUI_TEST_SUPPORT

#ifdef WXWINUI_TEST_SUPPORT
bool wxWinUITreeCtrlTestAccess::CompletePeerDrag(wxTreeCtrl &control,
                                                 const wxTreeItemId &target,
                                                 bool dropResultNone)
{
    wxTreeCtrl *const self = &control;
    if ( !self->m_winui || !self->m_winui->dragItemId )
        return false;

    self->HandlePeerDragCompleted(self->GetItem(target), wxPoint(),
                                  dropResultNone);
    return true;
}
#endif // WXWINUI_TEST_SUPPORT

#ifdef WXWINUI_TEST_SUPPORT
bool wxWinUITreeCtrlTestAccess::ScheduleLabelEdit(wxTreeCtrl &control,
                                                  const wxTreeItemId &item)
{
    wxTreeCtrl *const self = &control;
    return self->ScheduleDelayedLabelEdit(self->GetItem(item));
}
#endif // WXWINUI_TEST_SUPPORT

#ifdef WXWINUI_TEST_SUPPORT
bool wxWinUITreeCtrlTestAccess::FireLabelEditDelay(wxTreeCtrl &control)
{
    wxTreeCtrl *const self = &control;
    return self->m_winui && self->m_winui->pendingLabelEditId &&
           self->CompleteDelayedLabelEdit(self->m_winui->labelEditDelayTicket);
}
#endif // WXWINUI_TEST_SUPPORT

#ifdef WXWINUI_TEST_SUPPORT
bool wxWinUITreeCtrlTestAccess::ClickStateImage(wxTreeCtrl &control,
                                                const wxTreeItemId &item)
{
    wxTreeCtrl *const self = &control;
    wxWinUITreeItem *const treeItem = self->GetItem(item);
    if ( !treeItem )
        return false;

    const std::shared_ptr<wxWinUITreeCallbackState> state =
        self->m_winui->callbackState;
    const std::uint64_t generation = state->Generation();
    const std::uint64_t itemId = treeItem->id;
    self->SendStateImageClick(treeItem, wxPoint());
    return state->GetOwner(generation) == self &&
           self->ResolveItem(itemId) != nullptr;
}
#endif // WXWINUI_TEST_SUPPORT

#ifdef WXWINUI_TEST_SUPPORT
bool wxWinUITreeCtrlTestAccess::DoubleClickItem(wxTreeCtrl &control,
                                                const wxTreeItemId &item)
{
    wxTreeCtrl *const self = &control;
    wxWinUITreeItem *const treeItem = self->GetItem(item);
    if ( !treeItem )
        return false;

    const std::shared_ptr<wxWinUITreeCallbackState> state =
        self->m_winui->callbackState;
    const std::uint64_t generation = state->Generation();
    const std::uint64_t itemId = treeItem->id;
    self->CancelDelayedLabelEdit();
    wxWinUITreeItem *const liveItem = self->ResolveItem(itemId);
    if ( !liveItem )
        return false;
    self->SendTreeEvent(wxEVT_TREE_ITEM_ACTIVATED, liveItem);
    return state->GetOwner(generation) == self;
}
#endif // WXWINUI_TEST_SUPPORT

#ifdef WXWINUI_TEST_SUPPORT
bool wxWinUITreeCtrlTestAccess::RightClickItem(wxTreeCtrl &control,
                                               const wxTreeItemId &item)
{
    wxTreeCtrl *const self = &control;
    return self->SendRightClickEvents(self->GetItem(item), wxPoint());
}
#endif // WXWINUI_TEST_SUPPORT

#ifdef WXWINUI_TEST_SUPPORT
bool wxWinUITreeCtrlTestAccess::InvokeItem(wxTreeCtrl &control,
                                           const wxTreeItemId &item)
{
    wxTreeCtrl *const self = &control;
    wxWinUITreeItem *const treeItem = self->GetItem(item);
    if ( !treeItem )
        return false;
    self->OnPeerItemInvoked(treeItem);
    return true;
}
#endif // WXWINUI_TEST_SUPPORT

#ifdef WXWINUI_TEST_SUPPORT
void wxWinUITreeCtrlTestAccess::PointerPressed(wxTreeCtrl &control,
                                               const wxPoint &point)
{
    wxTreeCtrl *const self = &control;
    self->OnPeerPointerPressed(point);
}
#endif // WXWINUI_TEST_SUPPORT

#ifdef WXWINUI_TEST_SUPPORT
void wxWinUITreeCtrlTestAccess::PointerMoved(wxTreeCtrl &control,
                                             const wxPoint &point)
{
    wxTreeCtrl *const self = &control;
    self->OnPeerPointerMoved(point);
}
#endif // WXWINUI_TEST_SUPPORT

#ifdef WXWINUI_TEST_SUPPORT
void wxWinUITreeCtrlTestAccess::PointerReleased(wxTreeCtrl &control,
                                                const wxPoint &point)
{
    wxTreeCtrl *const self = &control;
    self->OnPeerPointerReleased(point);
}
#endif // WXWINUI_TEST_SUPPORT

#ifdef WXWINUI_TEST_SUPPORT
bool wxWinUITreeCtrlTestAccess::QueueSelectionCorrection(wxTreeCtrl &control)
{
    wxTreeCtrl *const self = &control;
    if ( !self->m_winui || !self->m_winui->treeView )
        return false;

    self->SchedulePeerSelectionCorrection();
    return self->m_winui && self->m_winui->peerCorrectionPending;
}
#endif // WXWINUI_TEST_SUPPORT

#ifdef WXWINUI_TEST_SUPPORT
bool wxWinUITreeCtrlTestAccess::IsPeerSelectionChange(const wxTreeCtrl &control)
{
    const wxTreeCtrl *const self = &control;
    return self->m_winui && self->m_winui->inPeerSelectionChange;
}
#endif // WXWINUI_TEST_SUPPORT

#ifdef WXWINUI_TEST_SUPPORT
bool wxWinUITreeCtrlTestAccess::IsPeerStructureRepairPending(
    const wxTreeCtrl &control)
{
    const wxTreeCtrl *const self = &control;
    return self->m_winui && self->m_winui->peerStructureRepairPending;
}
#endif // WXWINUI_TEST_SUPPORT

#ifdef WXWINUI_TEST_SUPPORT
std::uintptr_t
wxWinUITreeCtrlTestAccess::GetItemPeerIdentity(const wxTreeCtrl &control,
                                               const wxTreeItemId &item)
{
    const wxTreeCtrl *const self = &control;
    wxWinUITreeItem *const treeItem = self->GetItem(item);
    return treeItem
        ? wxWinUITreeNodeIdentity(treeItem->node)
        : 0;
}
#endif // WXWINUI_TEST_SUPPORT

#ifdef WXWINUI_TEST_SUPPORT
bool wxWinUITreeCtrlTestAccess::IsItemAttachedToPeer(const wxTreeCtrl &control,
                                                     const wxTreeItemId &item)
{
    const wxTreeCtrl *const self = &control;
    wxWinUITreeItem *const treeItem = self->GetItem(item);
    if ( !self->m_winui || !self->m_winui->treeView || !treeItem ||
         (treeItem == self->m_winui->root.get() &&
          self->HasFlag(wxTR_HIDE_ROOT)) )
    {
        return false;
    }

    wxWinUITreeCtrlImpl *const impl = self->m_winui.get();
    const std::shared_ptr<wxWinUITreeCallbackState> state =
        impl->callbackState;
    const std::uint64_t generation = state->Generation();
    const std::uint64_t itemId = treeItem->id;
    const std::uintptr_t identity =
        wxWinUITreeNodeIdentity(treeItem->node);
    const uint32_t expectedIndex = treeItem->parent
        ? static_cast<uint32_t>(treeItem->siblingIndex)
        : 0;
    try
    {
        const auto nodes = wxWinUIGetPeerChildren(self, impl, treeItem->parent);
        unsigned matches = 0;
        uint32_t actualIndex = 0;
        for ( uint32_t i = 0; i < nodes.Size(); ++i )
        {
            if ( wxWinUITreeNodeIdentity(nodes.GetAt(i)) == identity )
            {
                ++matches;
                actualIndex = i;
            }
        }

        const wxTreeCtrl *const owner = state->GetOwner(generation);
        const wxWinUITreeItem *const liveItem =
            owner == self ? owner->ResolveItem(itemId) : nullptr;
        return liveItem &&
               wxWinUITreeNodeIdentity(liveItem->node) == identity &&
               matches == 1 && actualIndex == expectedIndex;
    }
    catch ( const winrt::hresult_error& )
    {
        return false;
    }
}
#endif // WXWINUI_TEST_SUPPORT

#ifdef WXWINUI_TEST_SUPPORT
size_t wxWinUITreeCtrlTestAccess::GetPeerChildCount(const wxTreeCtrl &control,
                                                    const wxTreeItemId &parent)
{
    const wxTreeCtrl *const self = &control;
    if ( !self->m_winui || !self->m_winui->treeView )
        return 0;
    wxWinUITreeItem *const parentItem =
        parent.IsOk() ? self->GetItem(parent) : nullptr;
    if ( parent.IsOk() && !parentItem )
        return 0;
    try
    {
        return wxWinUIGetPeerChildren(self, self->m_winui.get(), parentItem)
            .Size();
    }
    catch ( const winrt::hresult_error& )
    {
        return 0;
    }
}
#endif // WXWINUI_TEST_SUPPORT

#ifdef WXWINUI_TEST_SUPPORT
wxWinUITreeCtrlTestAccess::ModelStats
wxWinUITreeCtrlTestAccess::GetModelStats(const wxTreeCtrl &control)
{
    const wxTreeCtrl *const self = &control;
    ModelStats stats;
    if ( self->m_winui )
    {
        stats.itemCount = self->m_winui->itemCount;
        stats.expandableItemCount = self->m_winui->expandableItemCount;
        stats.nodeLookupCount = self->m_winui->nodeLookupCount;
        stats.peerUpdateCount = self->m_winui->peerUpdateCount;
        stats.fullRefreshCount = self->m_winui->fullRefreshCount;
        stats.modelGrowthCount = self->m_winui->modelGrowthCount;
    }
    return stats;
}
#endif // WXWINUI_TEST_SUPPORT

#ifdef WXWINUI_TEST_SUPPORT
void wxWinUITreeCtrlTestAccess::ResetModelStats(wxTreeCtrl &control)
{
    wxTreeCtrl *const self = &control;
    if ( self->m_winui )
    {
        self->m_winui->nodeLookupCount = 0;
        self->m_winui->peerUpdateCount = 0;
        self->m_winui->fullRefreshCount = 0;
        self->m_winui->modelGrowthCount = 0;
    }
}
#endif // WXWINUI_TEST_SUPPORT

#ifdef WXWINUI_TEST_SUPPORT
void wxWinUITreeCtrlTestAccess::ClosePeer(wxTreeCtrl &control)
{
    wxTreeCtrl *const self = &control;
    const std::shared_ptr<wxWinUITreeCallbackState> state =
        self->m_winui ? self->m_winui->callbackState : nullptr;
    const std::uint64_t generation =
        state ? state->Generation() : 0;
    if ( self->m_editControl )
    {
        // Hide() can synchronously transfer focus and dispatch the editor's
        // KILL_FOCUS handler. Retire the logical edit session first so that
        // neither this callback nor an already queued TEXT_ENTER can publish
        // a late END_LABEL_EDIT while peer teardown is in progress.
        wxTextCtrl *const editor = self->m_editControl;
        self->m_editControl = nullptr;
        if ( self->m_winui )
        {
            self->m_winui->editItemId = 0;
            ++self->m_winui->editSession;
        }

        editor->Hide();
        if ( !state || state->GetOwner(generation) != self )
            return;
    }
    if ( self->m_winui )
        self->m_winui->Close();
}
#endif // WXWINUI_TEST_SUPPORT

#ifdef WXWINUI_TEST_SUPPORT
size_t wxWinUITreeCtrlTestAccess::GetLiveCallbackStateCount()
{
    return gs_liveTreeCallbackStates.load(
        std::memory_order_relaxed);
}
#endif // WXWINUI_TEST_SUPPORT

void wxTreeCtrl::SetWindowStyleFlag(long style)
{
    const bool wasMultiple = HasFlag(wxTR_MULTIPLE);
    const bool isMultiple = (style & wxTR_MULTIPLE) != 0;
    const bool wasHiddenRoot = HasFlag(wxTR_HIDE_ROOT);
    const bool isHiddenRoot = (style & wxTR_HIDE_ROOT) != 0;
    const bool hadButtons = HasFlag(wxTR_HAS_BUTTONS);
    const bool hasButtons = (style & wxTR_HAS_BUTTONS) != 0;

    if ( !m_winui || !m_winui->treeView ||
         (wasMultiple == isMultiple &&
          wasHiddenRoot == isHiddenRoot &&
          hadButtons == hasButtons) )
    {
        wxControl::SetWindowStyleFlag(style);
        return;
    }

    wxWinUITreeCtrlImpl * const impl = m_winui.get();
    const std::shared_ptr<wxWinUITreeCallbackState> state =
        impl->callbackState;
    const std::uint64_t generation = state->Generation();

    WFC::IVector<MUXC::TreeViewNode> peerRoots{ nullptr };
    std::vector<MUXC::TreeViewNode> modelPeerNodes;
    wxWinUITreePeerForestSnapshot oldHierarchy;
    wxWinUITreePeerForestSnapshot newHierarchy;
    bool hierarchySnapshotted = false;

    // Apply every peer-facing style transition as one transaction. In
    // particular, wxTR_HIDE_ROOT changes the actual TreeView hierarchy:
    // children move between root.Children and TreeView.RootNodes instead of
    // leaving a stale duplicate representation behind.
    try
    {
        wxWinUITreePeerMutationGuard mutation(
            m_winui->callbackState);

        if ( wasHiddenRoot != isHiddenRoot && impl->root )
        {
            peerRoots = impl->treeView.RootNodes();
            modelPeerNodes =
                wxWinUICollectModelPeerNodes(impl->root.get());
            oldHierarchy =
                wxWinUICapturePeerForest(peerRoots, modelPeerNodes);
            newHierarchy =
                wxWinUIBuildModelPeerForest(impl, isHiddenRoot);
            hierarchySnapshotted = true;
            wxWinUIApplyPeerForest(
                peerRoots, oldHierarchy, newHierarchy);
        }

        if ( wasMultiple != isMultiple )
        {
            m_winui->treeView.SelectionMode(
                isMultiple
                    ? MUXC::TreeViewSelectionMode::Multiple
                    : MUXC::TreeViewSelectionMode::Single);
        }
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException(
            "WinUI TreeView style transaction", e);
        if ( state->GetOwner(generation) != this || !m_winui ||
             m_winui.get() != impl || impl->closed )
        {
            return;
        }
        try
        {
            wxWinUITreePeerMutationGuard mutation(state);
            if ( wasMultiple != isMultiple )
            {
                impl->treeView.SelectionMode(
                    wasMultiple
                        ? MUXC::TreeViewSelectionMode::Multiple
                        : MUXC::TreeViewSelectionMode::Single);
            }
            if ( hierarchySnapshotted )
            {
                const wxWinUITreePeerForestSnapshot currentHierarchy =
                    wxWinUICapturePeerForest(
                        peerRoots, modelPeerNodes);
                wxWinUIApplyPeerForest(
                    peerRoots, currentHierarchy, oldHierarchy);
            }
        }
        catch ( const winrt::hresult_error& rollbackError )
        {
            wxWinUILogException(
                "WinUI TreeView style rollback", rollbackError);
            // The public style is still the old one, so this bounded rebuild
            // reconstructs the authoritative old representation.
            if ( state->GetOwner(generation) == this && m_winui &&
                 m_winui.get() == impl )
            {
                (void)ReconcilePeerStructureFromModel();
            }
        }
        return;
    }
    if ( state->GetOwner(generation) != this || !m_winui ||
         m_winui.get() != impl )
    {
        return;
    }

    const bool rootWasExpandable =
        wxWinUIItemCountsAsExpandable(this, impl->root.get());
    wxControl::SetWindowStyleFlag(style);
    if ( state->GetOwner(generation) != this || !m_winui ||
         m_winui.get() != impl )
    {
        return;
    }
    impl->showButtons = hasButtons;

    std::vector<std::uint64_t> changedIds;
    if ( wasHiddenRoot != isHiddenRoot && impl->root )
    {
        if ( isHiddenRoot )
        {
            // The hidden root is the logical anchor of all visible top-level
            // items and must remain expanded when it is shown again.
            impl->root->expanded = true;
            if ( impl->root->selected )
            {
                impl->root->selected = false;
                impl->selectedIds.erase(impl->root->id);
                changedIds.push_back(impl->root->id);
            }
            if ( impl->selectionId == impl->root->id )
                impl->selectionId = 0;
            if ( impl->focusedId == impl->root->id )
                impl->focusedId = 0;
        }

        const bool rootIsExpandable =
            wxWinUIItemCountsAsExpandable(this, impl->root.get());
        if ( rootWasExpandable != rootIsExpandable )
        {
            if ( rootIsExpandable )
                ++impl->expandableItemCount;
            else
                --impl->expandableItemCount;
        }
    }

    if ( !isMultiple )
    {
        const std::uint64_t keepId = m_winui->selectionId;
        for ( const std::uint64_t selectedId : m_winui->selectedIds )
        {
            if ( selectedId == keepId )
                continue;
            if ( wxWinUITreeItem * const selected =
                     ResolveItem(selectedId) )
            {
                selected->selected = false;
                changedIds.push_back(selectedId);
            }
        }
        m_winui->selectedIds.clear();
        if ( keepId )
            m_winui->selectedIds.insert(keepId);
    }

    const PeerProjectionResult projection =
        wasHiddenRoot != isHiddenRoot || hadButtons != hasButtons
            ? RefreshProjectedItems()
            : (changedIds.empty()
                   ? PeerProjectionResult::Done
                   : UpdatePeerItems(changedIds));
    if ( state->GetOwner(generation) != this || !m_winui ||
         m_winui.get() != impl )
    {
        return;
    }
    if ( projection != PeerProjectionResult::Done )
        SchedulePeerSelectionCorrection();

    if ( !ApplySelectionToPeer() &&
         state->GetOwner(generation) == this )
    {
        SchedulePeerSelectionCorrection();
    }

    if ( state->GetOwner(generation) == this && m_winui &&
         m_winui.get() == impl )
    {
        impl->host.ForceRender();
        if ( state->GetOwner(generation) == this && m_winui &&
             m_winui.get() == impl )
        {
            (void)wxWinUISyncRealizedTreeItems(impl);
        }
    }
}

bool wxTreeCtrl::MSWOnEffectiveLayoutDirectionChanged()
{
    wxWinUITreeCtrlImpl * const impl = m_winui.get();
    if ( !impl )
        return true;

    const std::shared_ptr<wxWinUITreeCallbackState> state =
        impl->callbackState;
    const std::uint64_t generation =
        state ? state->Generation() : 0;

    if ( !impl || !state ||
         state->GetOwner(generation) != this || !m_winui ||
         m_winui.get() != impl || impl->closed )
    {
        return false;
    }

    const wxLayoutDirection effectiveDirection =
        GetLayoutDirection();
    if ( state->GetOwner(generation) != this || !m_winui ||
         m_winui.get() != impl || impl->closed )
    {
        return false;
    }
    impl->rightToLeft =
        effectiveDirection == wxLayout_RightToLeft;
    try
    {
        impl->treeView.FlowDirection(
            impl->rightToLeft
                ? MUX::FlowDirection::RightToLeft
                : MUX::FlowDirection::LeftToRight);
        if ( state->GetOwner(generation) != this || !m_winui ||
             m_winui.get() != impl || impl->closed )
        {
            return false;
        }
        const PeerProjectionResult projection =
            RefreshProjectedItems();
        if ( projection != PeerProjectionResult::Done &&
             state->GetOwner(generation) == this )
        {
            SchedulePeerSelectionCorrection();
        }
        if ( state->GetOwner(generation) != this || !m_winui ||
             m_winui.get() != impl || impl->closed )
        {
            return false;
        }
        return wxWinUISyncRealizedTreeItems(impl);
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException(
            "WinUI TreeView layout direction", e);
        return false;
    }
}

bool wxTreeCtrl::Reparent(wxWindowBase *newParent)
{
    // A delayed label-edit request is tied to the old island/viewport. Cancel
    // it before the shared host migrates this window to another top-level.
    CancelDelayedLabelEdit();
    return wxControl::Reparent(newParent);
}

unsigned int wxTreeCtrl::GetCount() const
{
    if ( !m_winui || !m_winui->root )
        return 0;

    const size_t count =
        m_winui->itemCount - (HasFlag(wxTR_HIDE_ROOT) ? 1u : 0u);
    return static_cast<unsigned int>(
        wxMin(count,
              static_cast<size_t>(
                  std::numeric_limits<unsigned int>::max())));
}

unsigned int wxTreeCtrl::GetIndent() const
{
    return m_indent;
}

void wxTreeCtrl::SetIndent(unsigned int indent)
{
    m_indent = indent;
    if ( !m_winui || !m_winui->callbackState || m_winui->closed )
        return;

    wxWinUITreeCtrlImpl * const impl = m_winui.get();
    const std::shared_ptr<wxWinUITreeCallbackState> state =
        impl->callbackState;
    const std::uint64_t generation = state->Generation();
    const wxLayoutDirection effectiveDirection =
        GetLayoutDirection();
    if ( state->GetOwner(generation) != this || !m_winui ||
         m_winui.get() != impl )
    {
        return;
    }
    impl->indentDips = static_cast<double>(indent);
    impl->rightToLeft =
        effectiveDirection == wxLayout_RightToLeft;

    wxWinUISyncRealizedTreeItems(impl);
    if ( state->GetOwner(generation) == this && m_winui &&
         m_winui.get() == impl )
    {
        impl->host.ForceRender();
    }
}

void wxTreeCtrl::SetStateImages(const wxVector<wxBitmapBundle>& images)
{
    m_winuiStateImageBundles = images;
    if ( m_winui )
        m_winui->stateImageBundles = images;
    m_imagesState.SetImages(images);
    OnImagesChanged();
}

void wxTreeCtrl::SetImageList(wxImageList *imageList)
{
    wxWithImages::SetImageList(imageList);
}

void wxTreeCtrl::SetStateImageList(wxImageList *imageList)
{
    m_winuiStateImageBundles.clear();
    if ( m_winui )
        m_winui->stateImageBundles.clear();
    m_imagesState.SetImageList(imageList);
    OnImagesChanged();
}

wxString wxTreeCtrl::GetItemText(const wxTreeItemId& item) const
{
    wxWinUITreeItem *treeItem = GetItem(item);
    wxCHECK_MSG( treeItem, wxString(), wxT("invalid tree item") );
    return treeItem->text;
}

int wxTreeCtrl::GetItemImage(const wxTreeItemId& item, wxTreeItemIcon which) const
{
    wxWinUITreeItem *treeItem = GetItem(item);
    wxCHECK_MSG( treeItem, NO_IMAGE, wxT("invalid tree item") );
    return treeItem->images[which];
}

wxTreeItemData *wxTreeCtrl::GetItemData(const wxTreeItemId& item) const
{
    wxWinUITreeItem *treeItem = GetItem(item);
    return treeItem ? treeItem->data : nullptr;
}

wxColour wxTreeCtrl::GetItemTextColour(const wxTreeItemId& item) const
{
    wxWinUITreeItem *treeItem = GetItem(item);
    return treeItem ? treeItem->textColour : wxColour();
}

wxColour wxTreeCtrl::GetItemBackgroundColour(const wxTreeItemId& item) const
{
    wxWinUITreeItem *treeItem = GetItem(item);
    return treeItem ? treeItem->backgroundColour : wxColour();
}

wxFont wxTreeCtrl::GetItemFont(const wxTreeItemId& item) const
{
    wxWinUITreeItem *treeItem = GetItem(item);
    return treeItem ? treeItem->font : wxFont();
}

void wxTreeCtrl::SetItemText(const wxTreeItemId& item, const wxString& text)
{
    wxWinUITreeItem *treeItem = GetItem(item);
    wxCHECK_RET( treeItem, wxT("invalid tree item") );
    wxWinUITreeCtrlImpl * const impl = m_winui.get();
    ++treeItem->textMutation;
    treeItem->text = text;
    impl->realizedMetadataDirty = true;
    (void)UpdatePeerItem(treeItem);

    // The generated container carries a separate AutomationProperties.Name
    // local value. SyncPeerProjection() publishes it only after the content
    // transaction has converged (including a deferred replay), so a bounded
    // callback storm cannot leave UIA one writer behind.
}

void wxTreeCtrl::SetItemImage(const wxTreeItemId& item,
                              int image,
                              wxTreeItemIcon which)
{
    wxWinUITreeItem *treeItem = GetItem(item);
    wxCHECK_RET( treeItem, wxT("invalid tree item") );
    treeItem->images[which] = image;
    UpdatePeerItem(treeItem);
}

void wxTreeCtrl::SetItemData(const wxTreeItemId& item, wxTreeItemData *data)
{
    wxWinUITreeItem *treeItem = GetItem(item);
    wxCHECK_RET( treeItem, wxT("invalid tree item") );
    if ( treeItem->data == data )
        return;

    // Match wxGenericTreeCtrl/wxMSW exactly: replacing item data only detaches
    // the previous pointer. It neither destroys nor mutates that object; its
    // owner is responsible for it after replacement.
    treeItem->data = data;
    if ( data )
        data->SetId(MakeId(treeItem));
}

void wxTreeCtrl::SetItemHasChildren(const wxTreeItemId& item, bool has)
{
    wxWinUITreeItem *treeItem = GetItem(item);
    wxCHECK_RET( treeItem, wxT("invalid tree item") );

    const bool hadAnyExpandable = HasExpandableItem();
    const bool wasExpandable =
        wxWinUIItemCountsAsExpandable(this, treeItem);
    treeItem->hasChildrenOverride = has;
    const bool isExpandable =
        wxWinUIItemCountsAsExpandable(this, treeItem);
    if ( wasExpandable != isExpandable )
    {
        if ( isExpandable )
            ++m_winui->expandableItemCount;
        else
            --m_winui->expandableItemCount;
    }

    if ( hadAnyExpandable != HasExpandableItem() )
        (void)RefreshProjectedItems();
    else
        (void)UpdatePeerItem(treeItem);
}

void wxTreeCtrl::SetItemBold(const wxTreeItemId& item, bool bold)
{
    wxWinUITreeItem *treeItem = GetItem(item);
    wxCHECK_RET( treeItem, wxT("invalid tree item") );
    treeItem->bold = bold;
    UpdatePeerItem(treeItem);
}

void wxTreeCtrl::SetItemDropHighlight(const wxTreeItemId& item,
                                      bool highlight)
{
    wxWinUITreeItem *treeItem = GetItem(item);
    wxCHECK_RET( treeItem, wxT("invalid tree item") );

    std::vector<std::uint64_t> changedIds;
    if ( highlight && m_winui->dropHighlightId != treeItem->id )
    {
        if ( wxWinUITreeItem * const old =
                 ResolveItem(m_winui->dropHighlightId) )
        {
            old->dropHighlighted = false;
            changedIds.push_back(old->id);
        }
        m_winui->dropHighlightId = treeItem->id;
    }
    else if ( !highlight && m_winui->dropHighlightId == treeItem->id )
    {
        m_winui->dropHighlightId = 0;
    }

    treeItem->dropHighlighted = highlight;
    changedIds.push_back(treeItem->id);
    (void)UpdatePeerItems(changedIds);
}

void wxTreeCtrl::SetItemTextColour(const wxTreeItemId& item, const wxColour& col)
{
    wxWinUITreeItem *treeItem = GetItem(item);
    wxCHECK_RET( treeItem, wxT("invalid tree item") );
    treeItem->textColour = col;
    UpdatePeerItem(treeItem);
}

void wxTreeCtrl::SetItemBackgroundColour(const wxTreeItemId& item, const wxColour& col)
{
    wxWinUITreeItem *treeItem = GetItem(item);
    wxCHECK_RET( treeItem, wxT("invalid tree item") );
    treeItem->backgroundColour = col;
    UpdatePeerItem(treeItem);
}

void wxTreeCtrl::SetItemFont(const wxTreeItemId& item, const wxFont& font)
{
    wxWinUITreeItem *treeItem = GetItem(item);
    wxCHECK_RET( treeItem, wxT("invalid tree item") );
    treeItem->font = font;
    UpdatePeerItem(treeItem);
}

bool wxTreeCtrl::IsVisible(const wxTreeItemId& item) const
{
    wxWinUITreeItem * const treeItem = GetItem(item);
    if ( !treeItem ||
         !wxWinUIItemIsLogicallyVisible(this, treeItem) ||
         !m_winui->callbackState )
    {
        return false;
    }

    wxWinUITreeCtrlImpl * const impl = m_winui.get();
    const std::shared_ptr<wxWinUITreeCallbackState> state =
        impl->callbackState;
    const std::uint64_t generation = state->Generation();
    const std::uint64_t itemId = treeItem->id;
    wxRect peerRect;
    if ( !GetItemPeerRect(treeItem, peerRect, false) )
        return false;

    wxTreeCtrl *owner = state->GetOwner(generation);
    if ( owner != this || !owner->m_winui ||
         owner->m_winui.get() != impl ||
         owner->ResolveItem(itemId) == nullptr )
    {
        return false;
    }

    const wxRect clientRect = owner->GetClientRect();
    owner = state->GetOwner(generation);
    if ( owner != this || !owner->m_winui ||
         owner->m_winui.get() != impl ||
         owner->ResolveItem(itemId) == nullptr )
    {
        return false;
    }

    return !peerRect.IsEmpty() && peerRect.Intersects(clientRect);
}

bool wxTreeCtrl::ItemHasChildren(const wxTreeItemId& item) const
{
    wxWinUITreeItem *treeItem = GetItem(item);
    return treeItem && (!treeItem->children.empty() || treeItem->hasChildrenOverride);
}

bool wxTreeCtrl::IsExpanded(const wxTreeItemId& item) const
{
    wxWinUITreeItem *treeItem = GetItem(item);
    return treeItem && treeItem->expanded;
}

bool wxTreeCtrl::IsSelected(const wxTreeItemId& item) const
{
    wxWinUITreeItem *treeItem = GetItem(item);
    return treeItem && treeItem->selected;
}

bool wxTreeCtrl::IsBold(const wxTreeItemId& item) const
{
    wxWinUITreeItem *treeItem = GetItem(item);
    return treeItem && treeItem->bold;
}

size_t wxTreeCtrl::GetChildrenCount(const wxTreeItemId& item, bool recursively) const
{
    wxWinUITreeItem *treeItem = GetItem(item);
    wxCHECK_MSG( treeItem, 0, wxT("invalid tree item") );

    if ( !recursively )
        return treeItem->children.size();

    return wxWinUIGetSubtreeCount(treeItem);
}

wxTreeItemId wxTreeCtrl::GetRootItem() const
{
    return m_winui ? MakeId(m_winui->root.get()) : wxTreeItemId();
}

wxTreeItemId wxTreeCtrl::GetSelection() const
{
    return m_winui
        ? MakeId(ResolveItem(m_winui->selectionId))
        : wxTreeItemId();
}

size_t wxTreeCtrl::GetSelections(wxArrayTreeItemIds& selections) const
{
    selections.Empty();

    if ( !m_winui || !m_winui->root )
        return 0;

    std::vector<wxWinUITreeItem *> stack{m_winui->root.get()};
    while ( !stack.empty() )
    {
        wxWinUITreeItem *item = stack.back();
        stack.pop_back();

        if ( m_winui->selectedIds.count(item->id) )
            selections.Add(MakeId(item));

        for ( auto it = item->children.rbegin();
              it != item->children.rend(); ++it )
        {
            stack.push_back(it->get());
        }
    }

    return selections.GetCount();
}

wxTreeItemId wxTreeCtrl::GetFocusedItem() const
{
    return m_winui
        ? MakeId(ResolveItem(m_winui->focusedId))
        : wxTreeItemId();
}

void wxTreeCtrl::ClearFocusedItem()
{
    if ( !m_winui )
        return;

    m_winui->focusedId = 0;
    try
    {
        if ( m_winui->treeView )
            m_winui->treeView.Focus(MUX::FocusState::Programmatic);
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI TreeView focus clear", e);
    }
}

void wxTreeCtrl::SetFocusedItem(const wxTreeItemId& item)
{
    wxWinUITreeItem * const treeItem = GetItem(item);
    wxCHECK_RET( treeItem, wxT("invalid tree item") );
    wxCHECK_RET( !HasFlag(wxTR_HIDE_ROOT) ||
                     treeItem != m_winui->root.get(),
                 wxT("can't focus hidden root item") );
    const std::shared_ptr<wxWinUITreeCallbackState> state =
        m_winui->callbackState;
    const std::uint64_t generation = state->Generation();
    const std::uint64_t itemId = treeItem->id;
    const std::uint64_t operation = state->BeginOperation();
    if ( !EnsureVisibleItem(itemId, operation, true) ||
         state->GetOwner(generation) != this ||
         !state->IsCurrentOperation(operation) )
    {
        return;
    }

    wxWinUITreeItem * const liveItem = ResolveItem(itemId);
    if ( !liveItem )
        return;

    try
    {
        const auto container =
            m_winui->treeView.ContainerFromNode(liveItem->node)
                .try_as<MUXC::Control>();
        if ( !container )
            return;

        // UIElement::Focus() returning false doesn't imply that logical focus
        // failed. Resolve the actual focused element below instead of treating
        // the bool as the transaction's commit result.
        (void)container.Focus(MUX::FocusState::Programmatic);
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI TreeView focus set", e);
        return;
    }

    // Focus() is observable and may have run a newer SetFocusedItem() from a
    // focus callback. Commit only if this request remains the latest writer.
    if ( state->GetOwner(generation) == this &&
         state->IsCurrentOperation(operation) &&
         ResolveItem(itemId) )
    {
        wxWinUITreeItem * const focused =
            wxWinUIFindFocusedItem(m_winui.get());
        if ( focused && !focused->deleting &&
             focused->id == itemId &&
             ResolveItem(itemId) == focused )
        {
            m_winui->focusedId = itemId;
        }
    }
}

wxTreeItemId wxTreeCtrl::GetItemParent(const wxTreeItemId& item) const
{
    wxWinUITreeItem *treeItem = GetItem(item);
    return treeItem ? MakeId(treeItem->parent) : wxTreeItemId();
}

wxTreeItemId wxTreeCtrl::GetFirstChild(const wxTreeItemId& item,
                                       wxTreeItemIdValue& cookie) const
{
    wxWinUITreeItem *treeItem = GetItem(item);
    if ( !treeItem || treeItem->children.empty() )
    {
        cookie = nullptr;
        return wxTreeItemId();
    }

    cookie = reinterpret_cast<wxTreeItemIdValue>(static_cast<uintptr_t>(1));
    return MakeId(treeItem->children.front().get());
}

wxTreeItemId wxTreeCtrl::GetNextChild(const wxTreeItemId& item,
                                      wxTreeItemIdValue& cookie) const
{
    wxWinUITreeItem *treeItem = GetItem(item);
    const uintptr_t index = reinterpret_cast<uintptr_t>(cookie);
    if ( !treeItem || index >= treeItem->children.size() )
    {
        cookie = nullptr;
        return wxTreeItemId();
    }

    cookie = reinterpret_cast<wxTreeItemIdValue>(index + 1);
    return MakeId(treeItem->children[index].get());
}

wxTreeItemId wxTreeCtrl::GetLastChild(const wxTreeItemId& item) const
{
    wxWinUITreeItem *treeItem = GetItem(item);
    if ( !treeItem || treeItem->children.empty() )
        return wxTreeItemId();

    return MakeId(treeItem->children.back().get());
}

wxTreeItemId wxTreeCtrl::GetNextSibling(const wxTreeItemId& item) const
{
    return MakeId(wxWinUIGetNextSiblingItem(GetItem(item)));
}

wxTreeItemId wxTreeCtrl::GetPrevSibling(const wxTreeItemId& item) const
{
    return MakeId(wxWinUIGetPrevSiblingItem(GetItem(item)));
}

wxTreeItemId wxTreeCtrl::GetFirstVisibleItem() const
{
    if ( !m_winui || !m_winui->root )
        return wxTreeItemId();

    const bool hiddenRoot = HasFlag(wxTR_HIDE_ROOT);
    wxWinUITreeItem *candidate = hiddenRoot
        ? (m_winui->root->children.empty()
               ? nullptr
               : m_winui->root->children.front().get())
        : m_winui->root.get();
    const std::shared_ptr<wxWinUITreeCallbackState> state =
        m_winui->callbackState;
    const std::uint64_t generation = state->Generation();
    while ( candidate )
    {
        const std::uint64_t candidateId = candidate->id;
        if ( IsVisible(MakeId(candidate)) )
        {
            wxTreeCtrl * const owner = state->GetOwner(generation);
            return owner == this
                ? MakeId(ResolveItem(candidateId))
                : wxTreeItemId();
        }
        if ( state->GetOwner(generation) != this )
            return wxTreeItemId();
        candidate = wxWinUIGetNextLogicalVisibleItem(
            ResolveItem(candidateId), hiddenRoot);
    }
    return wxTreeItemId();
}

wxTreeItemId wxTreeCtrl::GetNextVisible(const wxTreeItemId& item) const
{
    wxWinUITreeItem *treeItem = GetItem(item);
    if ( !treeItem )
        return wxTreeItemId();

    const bool hiddenRoot = HasFlag(wxTR_HIDE_ROOT);
    const std::shared_ptr<wxWinUITreeCallbackState> state =
        m_winui->callbackState;
    const std::uint64_t generation = state->Generation();
    treeItem = wxWinUIGetNextLogicalVisibleItem(treeItem, hiddenRoot);
    while ( treeItem )
    {
        const std::uint64_t candidateId = treeItem->id;
        if ( IsVisible(MakeId(treeItem)) )
        {
            if ( state->GetOwner(generation) != this )
                return wxTreeItemId();
            return MakeId(ResolveItem(candidateId));
        }
        if ( state->GetOwner(generation) != this )
            return wxTreeItemId();
        treeItem = wxWinUIGetNextLogicalVisibleItem(
            ResolveItem(candidateId), hiddenRoot);
    }
    return wxTreeItemId();
}

wxTreeItemId wxTreeCtrl::GetPrevVisible(const wxTreeItemId& item) const
{
    wxWinUITreeItem *treeItem = GetItem(item);
    if ( !treeItem )
        return wxTreeItemId();

    const bool hiddenRoot = HasFlag(wxTR_HIDE_ROOT);
    const std::shared_ptr<wxWinUITreeCallbackState> state =
        m_winui->callbackState;
    const std::uint64_t generation = state->Generation();
    treeItem =
        wxWinUIGetPreviousLogicalVisibleItem(treeItem, hiddenRoot);
    while ( treeItem )
    {
        const std::uint64_t candidateId = treeItem->id;
        if ( IsVisible(MakeId(treeItem)) )
        {
            if ( state->GetOwner(generation) != this )
                return wxTreeItemId();
            return MakeId(ResolveItem(candidateId));
        }
        if ( state->GetOwner(generation) != this )
            return wxTreeItemId();
        treeItem = wxWinUIGetPreviousLogicalVisibleItem(
            ResolveItem(candidateId), hiddenRoot);
    }
    return wxTreeItemId();
}

wxTreeItemId wxTreeCtrl::AddRoot(const wxString& text,
                                 int image,
                                 int selectedImage,
                                 wxTreeItemData *data)
{
    wxCHECK_MSG( m_winui && m_winui->treeView && !m_winui->closed,
                 wxTreeItemId(), wxT("uninitialized tree") );
    // Match the generic and MSW contracts: a tree has exactly one root and a
    // rejected second AddRoot() must not delete or otherwise mutate it.
    if ( m_winui->root )
        return wxTreeItemId();

    wxWinUITreeCtrlImpl * const impl = m_winui.get();
    const std::shared_ptr<wxWinUITreeCallbackState> state =
        impl->callbackState;
    if ( !state )
        return wxTreeItemId();
    const std::uint64_t generation = state->Generation();
    if ( state->GetOwner(generation) != this )
        return wxTreeItemId();
    state->BeginOperation();

    auto root = std::make_unique<wxWinUITreeItem>();
    wxWinUITreeItem *rootRaw = root.get();
    rootRaw->text = text;
    rootRaw->images[wxTreeItemIcon_Normal] = image;
    rootRaw->images[wxTreeItemIcon_Selected] = selectedImage;
    const bool hiddenRoot = HasFlag(wxTR_HIDE_ROOT);
    try
    {
        rootRaw->node = MUXC::TreeViewNode();
        if ( hiddenRoot )
        {
            // Match wxGenericTreeCtrl: the invisible anchor is permanently
            // expanded so its children are navigable as top-level items.
            rootRaw->node.IsExpanded(true);
            rootRaw->expanded = true;
        }
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException(
            "WinUI TreeView root node creation", e);
        return wxTreeItemId();
    }
    wxWinUIRegisterItem(impl, rootRaw);
    const std::uint64_t rootId = rootRaw->id;
    const MUXC::TreeViewNode rootNode = rootRaw->node;
    const MUXC::TreeView treeView = impl->treeView;
    rootRaw->data = data;
    if ( data )
        data->SetId(MakeId(rootRaw));

    impl->root = std::move(root);
    const auto rollbackModel =
        [state, generation, impl, rootId]()
        {
            wxTreeCtrl * const owner = state->GetOwner(generation);
            if ( !owner || !owner->m_winui ||
                 owner->m_winui.get() != impl ||
                 owner->m_winui->callbackState != state )
            {
                return;
            }

            wxWinUITreeItem * const liveRoot =
                impl->root && impl->root->id == rootId
                    ? impl->root.get()
                    : nullptr;
            if ( !liveRoot )
                return;

            if ( liveRoot->data )
                liveRoot->data->SetId(wxTreeItemId());
            liveRoot->data = nullptr;
            --impl->itemCount;
            wxWinUIUnregisterItem(impl, liveRoot);
            impl->root.reset();
        };
    const auto rollbackPeer =
        [treeView, rootNode](bool injectAfterCommit)
        {
            try
            {
                auto roots = treeView.RootNodes();
                return wxWinUIEnsurePeerNodeAbsent(
                    roots, rootNode,
                    "WinUI TreeView root insertion rollback",
                    injectAfterCommit);
            }
            catch ( const winrt::hresult_error& e )
            {
                wxWinUILogException(
                    "WinUI TreeView root insertion rollback", e);
                return false;
            }
        };

    const PeerProjectionResult projection =
        UpdatePeerItem(rootRaw);
    wxTreeCtrl *owner = state->GetOwner(generation);
    if ( !owner || !owner->m_winui ||
         owner->m_winui.get() != impl )
    {
        return wxTreeItemId();
    }

    rootRaw = ResolveItem(rootId);
    if ( !rootRaw || rootRaw != impl->root.get() )
    {
        return wxTreeItemId();
    }
    if ( projection == PeerProjectionResult::Failed ||
         (projection == PeerProjectionResult::Done &&
          !rootRaw->contentAttached) )
    {
        rollbackModel();
        return wxTreeItemId();
    }

    try
    {
        wxWinUITreePeerMutationGuard mutation(state);
        if ( !HasFlag(wxTR_HIDE_ROOT) )
        {
            treeView.RootNodes().Append(rootNode);
            owner = state->GetOwner(generation);
            if ( !owner || !owner->m_winui ||
                 owner->m_winui.get() != impl ||
                 owner->m_winui->callbackState != state ||
                 owner->ResolveItem(rootId) != impl->root.get() )
            {
                rollbackPeer(false);
                return wxTreeItemId();
            }
        }

#ifdef WXWINUI_TEST_SUPPORT
        const bool injectRollbackFailure = impl->ShouldFail(
            wxWinUITreeCtrlTestAccess::PeerMutation::InsertRollbackAfterCommit);
        if ( impl->ShouldFail(
                 wxWinUITreeCtrlTestAccess::PeerMutation::InsertItem) ||
             injectRollbackFailure )
        {
            // Always inspect/remove by node identity. A WinRT ABI call may
            // commit the append and then throw before peerInserted is set.
            if ( rollbackPeer(injectRollbackFailure) )
            {
                rollbackModel();
                return wxTreeItemId();
            }
            // A peer that cannot be removed has committed the insertion.
            // Keep the exact model/data ownership and return its valid id.
        }
#endif
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI TreeView root insertion", e);
        const bool peerRolledBack = rollbackPeer(false);
        owner = state->GetOwner(generation);
        if ( owner && owner->m_winui &&
             owner->m_winui.get() == impl &&
             owner->m_winui->callbackState == state &&
             peerRolledBack )
        {
            rollbackModel();
            return wxTreeItemId();
        }
        // As for child insertion, an unremovable peer is a committed model
        // item. The bounded projection driver will retry its content later.
    }

    owner = state->GetOwner(generation);
    if ( !owner || !owner->m_winui ||
         owner->m_winui.get() != impl ||
         owner->m_winui->callbackState != state ||
         owner->ResolveItem(rootId) != impl->root.get() )
    {
        return wxTreeItemId();
    }

    owner->m_winui->host.ForceRender();
    owner = state->GetOwner(generation);
    if ( !owner || !owner->m_winui ||
         owner->m_winui.get() != impl ||
         owner->m_winui->callbackState != state ||
         owner->ResolveItem(rootId) != impl->root.get() )
    {
        return wxTreeItemId();
    }

    return owner->MakeId(owner->ResolveItem(rootId));
}

void wxTreeCtrl::Delete(const wxTreeItemId& item)
{
    wxWinUITreeItem *treeItem = GetItem(item);
    wxCHECK_RET( treeItem, wxT("invalid tree item") );
    const std::shared_ptr<wxWinUITreeCallbackState> lifetimeState =
        m_winui->callbackState;
    const std::uint64_t lifetimeGeneration =
        lifetimeState->Generation();
    std::uint64_t operation =
        lifetimeState->BeginOperation();

    if ( treeItem->deleting )
        return;

    if ( treeItem == m_winui->root.get() )
    {
        DeleteAllItems();
        return;
    }

    const std::uint64_t itemId = treeItem->id;
    bool endedEdit = false;
    if ( wxWinUITreeItem * const editItem =
             ResolveItem(m_winui->editItemId) )
    {
        if ( wxWinUIIsDescendantOf(editItem, treeItem) )
        {
            endedEdit = true;
            EndEditLabel(MakeId(editItem), true);
        }
    }
    if ( lifetimeState->GetOwner(lifetimeGeneration) != this )
        return;
    if ( endedEdit && m_editControl )
        return;
    treeItem = ResolveItem(itemId);
    if ( !treeItem || treeItem->deleting )
        return;

    wxWinUITreeItem *parent = treeItem->parent;
    bool selectionAffected = false;
    bool selectionAllowed = false;
    std::uint64_t oldSelectionId = 0;
    std::uint64_t replacementId = 0;
    if ( wxWinUITreeItem * const selected =
             ResolveItem(m_winui->selectionId);
         selected && wxWinUIIsDescendantOf(selected, treeItem) )
    {
        selectionAffected = true;
        wxWinUITreeItem *replacement = nullptr;
        if ( treeItem->siblingIndex + 1 < parent->children.size() )
            replacement =
                parent->children[treeItem->siblingIndex + 1].get();
        else if ( parent->parent || !HasFlag(wxTR_HIDE_ROOT) )
            replacement = parent;
        replacementId = replacement ? replacement->id : 0;

        const SelectionPreflightResult preflight =
            PreflightSelectionChange(replacement, &oldSelectionId);
        if ( preflight == SelectionPreflightResult::Superseded ||
             lifetimeState->GetOwner(lifetimeGeneration) != this )
        {
            return;
        }
        selectionAllowed =
            preflight == SelectionPreflightResult::Allowed;

        // The preflight ran arbitrary application code while the model and
        // peer were still exactly aligned. A nested tree mutation owns the
        // operation and causes PreflightSelectionChange() to return
        // Superseded; only a callback-free commit continues below.
        treeItem = ResolveItem(itemId);
        parent = treeItem ? treeItem->parent : nullptr;
        if ( !treeItem || !parent || treeItem->deleting )
            return;
        operation = lifetimeState->BeginOperation();
    }

    const std::uint64_t parentId = parent->id;
    auto& siblings = parent->children;
    size_t modelIndex = treeItem->siblingIndex;
    if ( modelIndex >= siblings.size() ||
         siblings[modelIndex].get() != treeItem )
    {
        return;
    }
    auto found = siblings.begin() + modelIndex;
    const bool parentWasExpanded = parent->expanded;
    bool collapseParent = false;

    uint32_t peerIndex = 0;
    const std::uintptr_t itemNodeIdentity =
        wxWinUITreeNodeIdentity(treeItem->node);
    WFC::IVector<MUXC::TreeViewNode> peerSiblings{ nullptr };
    wxWinUITreePeerForestSnapshot removedPeerSubtree;
    try
    {
        wxWinUITreePeerMutationGuard mutation(m_winui->callbackState);
        peerSiblings =
            wxWinUIGetPeerChildren(this, m_winui.get(), parent);
        if ( modelIndex > std::numeric_limits<uint32_t>::max() )
            return;
        peerIndex = static_cast<uint32_t>(modelIndex);
        if ( peerIndex >= peerSiblings.Size() ||
             wxWinUITreeNodeIdentity(peerSiblings.GetAt(peerIndex)) !=
                 itemNodeIdentity )
        {
            wxFAIL_MSG("WinUI TreeView peer/model order diverged");
            return;
        }
        removedPeerSubtree =
            wxWinUICapturePeerSubtrees({ treeItem->node });
        wxWinUIDetachPeerForest(removedPeerSubtree);
        // The root is shallow now: RemoveAt cannot recursively walk its
        // descendants.
        peerSiblings.RemoveAt(peerIndex);
#ifdef WXWINUI_TEST_SUPPORT
        if ( m_winui->ShouldFail(
                 wxWinUITreeCtrlTestAccess::PeerMutation::RemoveItem) )
        {
            wxWinUIRestoreDetachedPeerSubtree(
                peerSiblings, peerIndex, removedPeerSubtree);
            if ( lifetimeState->GetOwner(lifetimeGeneration) == this )
                (void)ApplySelectionToPeer();
            return;
        }
#endif
#ifdef WXWINUI_TEST_SUPPORT
        if ( m_winui->ShouldFail(wxWinUITreeCtrlTestAccess::PeerMutation::
                                     RemoveItemAfterCommit) )
        {
            throw winrt::hresult_error(
                E_FAIL,
                L"Injected failure after TreeViewNode removal");
        }
#endif
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI TreeView item removal", e);
        try
        {
            wxTreeCtrl * const owner =
                lifetimeState->GetOwner(lifetimeGeneration);
            if ( owner == this && owner->m_winui &&
                 owner->m_winui->callbackState == lifetimeState &&
                 peerSiblings && !removedPeerSubtree.roots.empty() )
            {
                wxWinUITreePeerMutationGuard rollbackMutation(
                    lifetimeState);
                // RemoveAt() may have committed before surfacing the error.
                // Restore entirely from strong peer handles, without
                // dereferencing model pointers across the ABI boundary.
                wxWinUIRestoreDetachedPeerSubtree(
                    peerSiblings, peerIndex, removedPeerSubtree);
            }
        }
        catch ( const winrt::hresult_error& rollbackError )
        {
            wxWinUILogException(
                "WinUI TreeView item removal rollback",
                rollbackError);
            (void)ReconcilePeerStructureFromModel();
        }
        if ( lifetimeState->GetOwner(lifetimeGeneration) == this )
            (void)ApplySelectionToPeer();
        return;
    }

    treeItem = ResolveItem(itemId);
    parent = ResolveItem(parentId);
    if ( !treeItem || !parent || treeItem->deleting ||
         treeItem->parent != parent ||
         treeItem->siblingIndex >= parent->children.size() ||
         parent->children[treeItem->siblingIndex].get() != treeItem )
    {
        return;
    }
    modelIndex = treeItem->siblingIndex;
    found = siblings.begin() + modelIndex;
    collapseParent =
        parent->expanded &&
        !(HasFlag(wxTR_HIDE_ROOT) && parent == m_winui->root.get()) &&
        !(parent->hasChildrenOverride || parent->children.size() > 1);
    if ( collapseParent )
    {
        try
        {
            wxWinUITreePeerMutationGuard mutation(
                m_winui->callbackState);
            parent->node.IsExpanded(false);
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException(
                "WinUI TreeView parent collapse", e);
            try
            {
                wxWinUITreePeerMutationGuard mutation(
                    m_winui->callbackState);
                wxWinUIRestoreDetachedPeerSubtree(
                    peerSiblings, peerIndex, removedPeerSubtree);
                parent->node.IsExpanded(parentWasExpanded);
            }
            catch ( const winrt::hresult_error& rollbackError )
            {
                wxWinUILogException(
                    "WinUI TreeView parent-collapse rollback",
                    rollbackError);
                (void)ReconcilePeerStructureFromModel();
            }
            if ( lifetimeState->GetOwner(lifetimeGeneration) == this )
                (void)ApplySelectionToPeer();
            return;
        }
    }

    std::vector<wxWinUITreeItem *> subtree;
    wxWinUICollectSubtree(treeItem, subtree);
    std::vector<wxWinUITreeItem *> deleteOrder;
    wxWinUICollectDeleteOrder(treeItem, true, deleteOrder);

    if ( selectionAffected )
    {
        wxWinUITreeItem * const replacement =
            selectionAllowed && replacementId
                ? ResolveItem(replacementId)
                : nullptr;
        if ( selectionAllowed && replacementId && !replacement )
        {
            (void)ReconcilePeerStructureFromModel();
            return;
        }
        CommitSelectionForDeletion(replacement, treeItem);
    }

    const bool hadAnyExpandable = HasExpandableItem();
    const bool parentWasExpandable =
        wxWinUIItemCountsAsExpandable(this, parent);

    for ( wxWinUITreeItem * const current : subtree )
    {
        current->deleting = true;
        if ( wxWinUIItemCountsAsExpandable(this, current) )
            --m_winui->expandableItemCount;

        m_winui->selectedIds.erase(current->id);
        if ( m_winui->selectionId == current->id )
            m_winui->selectionId = 0;
        if ( m_winui->focusedId == current->id )
            m_winui->focusedId = 0;
        if ( m_winui->rejectedSelectionId == current->id )
            m_winui->rejectedSelectionId = 0;
        if ( m_winui->dragItemId == current->id )
            m_winui->dragItemId = 0;
        if ( m_winui->dropHighlightId == current->id )
            m_winui->dropHighlightId = 0;
        m_winui->itemsByNode.erase(
            wxWinUITreeNodeIdentity(current->node));
    }
    m_winui->itemCount -= subtree.size();

    wxWinUITreeDetachedSubtree removed(std::move(*found));
    siblings.erase(siblings.begin() + modelIndex);
    wxWinUIReindexChildren(parent, modelIndex);

    const bool parentIsExpandable =
        wxWinUIItemCountsAsExpandable(this, parent);
    if ( collapseParent )
        parent->expanded = false;
    if ( parentWasExpandable != parentIsExpandable )
    {
        if ( parentIsExpandable )
            ++m_winui->expandableItemCount;
        else
            --m_winui->expandableItemCount;
    }
    const bool refreshAll =
        hadAnyExpandable != HasExpandableItem();
    if ( selectionAffected && selectionAllowed )
    {
        wxWinUITreeItem * const newSelection =
            replacementId ? ResolveItem(replacementId) : nullptr;
        wxWinUITreeItem * const oldSelection =
            ResolveItem(oldSelectionId);
        wxTreeEvent selectionEvent(
            wxEVT_TREE_SEL_CHANGED, this, MakeId(newSelection));
        selectionEvent.SetOldItem(MakeId(oldSelection));
        HandleWindowEvent(selectionEvent);
        if ( lifetimeState->GetOwner(lifetimeGeneration) != this )
            return;
    }
    for ( wxWinUITreeItem * const current : deleteOrder )
    {
        wxTreeEvent event(wxEVT_TREE_DELETE_ITEM, this, MakeId(current));
        HandleWindowEvent(event);
        if ( lifetimeState->GetOwner(lifetimeGeneration) != this )
            return;
    }

    for ( wxWinUITreeItem * const current : subtree )
        wxWinUIUnregisterItem(m_winui.get(), current);

    const bool superseded =
        !lifetimeState->IsCurrentOperation(operation);
    wxWinUITreeItem * const liveParent = ResolveItem(parentId);
    if ( !liveParent )
        return;
    const PeerProjectionResult projection = (refreshAll || superseded)
        ? RefreshProjectedItems()
        : UpdatePeerItem(liveParent);
    if ( projection != PeerProjectionResult::Done ||
         lifetimeState->GetOwner(lifetimeGeneration) != this )
    {
        if ( lifetimeState->GetOwner(lifetimeGeneration) == this )
            SchedulePeerSelectionCorrection();
        return;
    }

    (void)ApplySelectionToPeer();
}

void wxTreeCtrl::DeleteChildren(const wxTreeItemId& item)
{
    wxWinUITreeItem *treeItem = GetItem(item);
    wxCHECK_RET( treeItem, wxT("invalid tree item") );
    const std::shared_ptr<wxWinUITreeCallbackState> lifetimeState =
        m_winui->callbackState;
    const std::uint64_t lifetimeGeneration =
        lifetimeState->Generation();
    std::uint64_t operation =
        lifetimeState->BeginOperation();

    if ( treeItem->deleting || treeItem->children.empty() )
        return;

    const std::uint64_t itemId = treeItem->id;
    bool endedEdit = false;
    if ( wxWinUITreeItem * const editItem =
             ResolveItem(m_winui->editItemId) )
    {
        if ( editItem != treeItem &&
             wxWinUIIsDescendantOf(editItem, treeItem) )
        {
            endedEdit = true;
            EndEditLabel(MakeId(editItem), true);
        }
    }
    if ( lifetimeState->GetOwner(lifetimeGeneration) != this )
        return;
    if ( endedEdit && m_editControl )
        return;
    treeItem = ResolveItem(itemId);
    if ( !treeItem || treeItem->deleting || treeItem->children.empty() )
        return;

    const bool selectionAffected =
        [&]()
        {
            wxWinUITreeItem * const selected =
                ResolveItem(m_winui->selectionId);
            return selected && selected != treeItem &&
                   wxWinUIIsDescendantOf(selected, treeItem);
        }();
    bool selectionAllowed = false;
    std::uint64_t oldSelectionId = 0;
    if ( selectionAffected )
    {
        const SelectionPreflightResult preflight =
            PreflightSelectionChange(treeItem, &oldSelectionId);
        if ( preflight == SelectionPreflightResult::Superseded ||
             lifetimeState->GetOwner(lifetimeGeneration) != this )
        {
            return;
        }
        selectionAllowed =
            preflight == SelectionPreflightResult::Allowed;
        treeItem = ResolveItem(itemId);
        if ( !treeItem || treeItem->deleting ||
             treeItem->children.empty() )
        {
            return;
        }
        operation = lifetimeState->BeginOperation();
    }

    WFC::IVector<MUXC::TreeViewNode> peerChildren{ nullptr };
    wxWinUITreePeerForestSnapshot removedPeerForest;
    const MUXC::TreeViewNode parentPeerNode = treeItem->node;
    try
    {
        peerChildren =
            wxWinUIGetPeerChildren(this, m_winui.get(), treeItem);
        removedPeerForest =
            wxWinUICapturePeerForest(peerChildren);
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException(
            "WinUI TreeView children lookup", e);
        return;
    }
    bool wasExpanded = treeItem->expanded;
    const bool keepExpanded =
        HasFlag(wxTR_HIDE_ROOT) && treeItem == m_winui->root.get();
    try
    {
        wxWinUITreePeerMutationGuard mutation(m_winui->callbackState);
        wxWinUIDetachPeerForest(removedPeerForest);
        peerChildren.Clear();
        parentPeerNode.IsExpanded(keepExpanded);
#ifdef WXWINUI_TEST_SUPPORT
        if ( m_winui->ShouldFail(
                 wxWinUITreeCtrlTestAccess::PeerMutation::ClearItems) )
        {
            wxWinUIRestoreDetachedPeerForest(
                peerChildren, removedPeerForest);
            parentPeerNode.IsExpanded(wasExpanded);
            if ( lifetimeState->GetOwner(lifetimeGeneration) == this )
                (void)ApplySelectionToPeer();
            return;
        }
#endif
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI TreeView children removal", e);
        try
        {
            wxTreeCtrl * const owner =
                lifetimeState->GetOwner(lifetimeGeneration);
            if ( owner == this && owner->m_winui &&
                 owner->m_winui->callbackState == lifetimeState &&
                 peerChildren )
            {
                wxWinUITreePeerMutationGuard rollbackMutation(
                    lifetimeState);
                wxWinUIRestoreDetachedPeerForest(
                    peerChildren, removedPeerForest);
                parentPeerNode.IsExpanded(wasExpanded);
            }
        }
        catch ( const winrt::hresult_error& rollbackError )
        {
            wxWinUILogException(
                "WinUI TreeView children removal rollback",
                rollbackError);
            (void)ReconcilePeerStructureFromModel();
        }
        if ( lifetimeState->GetOwner(lifetimeGeneration) == this )
            (void)ApplySelectionToPeer();
        return;
    }

    treeItem = ResolveItem(itemId);
    if ( !treeItem || treeItem->deleting ||
         treeItem->children.empty() )
    {
        return;
    }

    std::vector<wxWinUITreeItem *> subtree;
    std::vector<wxWinUITreeItem *> deleteOrder;
    for ( const auto& child : treeItem->children )
        wxWinUICollectSubtree(child.get(), subtree);
    wxWinUICollectDeleteOrder(treeItem, false, deleteOrder);

    if ( selectionAffected )
    {
        CommitSelectionForDeletion(
            selectionAllowed ? treeItem : nullptr,
            treeItem);
    }

    const bool hadAnyExpandable = HasExpandableItem();
    const bool parentWasExpandable =
        wxWinUIItemCountsAsExpandable(this, treeItem);

    for ( wxWinUITreeItem * const current : subtree )
    {
        current->deleting = true;
        if ( wxWinUIItemCountsAsExpandable(this, current) )
            --m_winui->expandableItemCount;
        m_winui->selectedIds.erase(current->id);
        if ( m_winui->selectionId == current->id )
            m_winui->selectionId = 0;
        if ( m_winui->focusedId == current->id )
            m_winui->focusedId = 0;
        if ( m_winui->rejectedSelectionId == current->id )
            m_winui->rejectedSelectionId = 0;
        if ( m_winui->dragItemId == current->id )
            m_winui->dragItemId = 0;
        if ( m_winui->dropHighlightId == current->id )
            m_winui->dropHighlightId = 0;
        m_winui->itemsByNode.erase(
            wxWinUITreeNodeIdentity(current->node));
    }
    m_winui->itemCount -= subtree.size();

    wxWinUITreeDetachedForest removed;
    removed.Swap(treeItem->children);
    treeItem->expanded = keepExpanded;
    const bool parentIsExpandable =
        wxWinUIItemCountsAsExpandable(this, treeItem);
    if ( parentWasExpandable != parentIsExpandable )
    {
        if ( parentIsExpandable )
            ++m_winui->expandableItemCount;
        else
            --m_winui->expandableItemCount;
    }
    const bool refreshAll =
        hadAnyExpandable != HasExpandableItem();
    if ( selectionAffected && selectionAllowed )
    {
        wxWinUITreeItem * const oldSelection =
            ResolveItem(oldSelectionId);
        wxTreeEvent selectionEvent(
            wxEVT_TREE_SEL_CHANGED, this, MakeId(treeItem));
        selectionEvent.SetOldItem(MakeId(oldSelection));
        HandleWindowEvent(selectionEvent);
        if ( lifetimeState->GetOwner(lifetimeGeneration) != this )
            return;
    }
    for ( wxWinUITreeItem * const current : deleteOrder )
    {
        wxTreeEvent event(wxEVT_TREE_DELETE_ITEM, this, MakeId(current));
        HandleWindowEvent(event);
        if ( lifetimeState->GetOwner(lifetimeGeneration) != this )
            return;
    }

    for ( wxWinUITreeItem * const current : subtree )
        wxWinUIUnregisterItem(m_winui.get(), current);

    const bool superseded =
        !lifetimeState->IsCurrentOperation(operation);
    treeItem = ResolveItem(itemId);
    if ( !treeItem )
        return;
    const PeerProjectionResult projection = (refreshAll || superseded)
        ? RefreshProjectedItems()
        : UpdatePeerItem(treeItem);
    if ( projection != PeerProjectionResult::Done ||
         lifetimeState->GetOwner(lifetimeGeneration) != this )
    {
        if ( lifetimeState->GetOwner(lifetimeGeneration) == this )
            SchedulePeerSelectionCorrection();
        return;
    }

    (void)ApplySelectionToPeer();
}

void wxTreeCtrl::DeleteAllItems()
{
    if ( !m_winui || !m_winui->root )
        return;
    const std::shared_ptr<wxWinUITreeCallbackState> state =
        m_winui->callbackState;
    const std::uint64_t generation = state->Generation();
    state->BeginOperation();
    m_winui->BumpModelRevision();

    const bool endedEdit = m_editControl != nullptr;
    if ( endedEdit )
        EndEditLabel(MakeId(ResolveItem(m_winui->editItemId)), true);
    if ( state->GetOwner(generation) != this )
        return;
    if ( endedEdit && m_editControl )
        return;
    if ( !m_winui || !m_winui->root )
        return;

    const bool selectionAffected = m_winui->selectionId != 0;
    bool selectionAllowed = false;
    std::uint64_t oldSelectionId = 0;
    const std::uint64_t rootId = m_winui->root->id;
    if ( selectionAffected )
    {
        const SelectionPreflightResult preflight =
            PreflightSelectionChange(nullptr, &oldSelectionId);
        if ( preflight == SelectionPreflightResult::Superseded ||
             state->GetOwner(generation) != this )
        {
            return;
        }
        selectionAllowed =
            preflight == SelectionPreflightResult::Allowed;
        if ( !m_winui || ResolveItem(rootId) != m_winui->root.get() )
            return;
    }

    wxWinUITreeCtrlImpl * const impl = m_winui.get();
    const std::uint64_t operation = state->BeginOperation();
    const auto isCurrent =
        [&]()
        {
            wxTreeCtrl * const owner = state->GetOwner(generation);
            return owner == this && state->IsCurrentOperation(operation) &&
                   owner->m_winui && owner->m_winui.get() == impl &&
                   !impl->closed && impl->root &&
                   owner->ResolveItem(rootId) == impl->root.get();
        };

    struct PeerNodeSnapshot
    {
        MUXC::TreeViewNode node{ nullptr };
        std::vector<MUXC::TreeViewNode> children;
        bool expanded = false;
    };

    std::vector<wxWinUITreeItem *> subtree;
    wxWinUICollectSubtree(m_winui->root.get(), subtree);

    std::vector<MUXC::TreeViewNode> peerNodes;
    std::vector<PeerNodeSnapshot> peerStructure;
    peerStructure.reserve(subtree.size());
    WFC::IVector<MUXC::TreeViewNode> roots{ nullptr };
    try
    {
        roots = m_winui->treeView.RootNodes();
        for ( uint32_t i = 0; i < roots.Size(); ++i )
            peerNodes.push_back(roots.GetAt(i));

        // Keep a strong reference to every node and to all of its current
        // children. Removing a 10k-deep root while these peer edges are still
        // connected makes TreeView recursively detach the entire graph and
        // overflows the native stack even though the wx model is iterative.
        for ( wxWinUITreeItem * const current : subtree )
        {
            if ( !isCurrent() )
                return;

            PeerNodeSnapshot snapshot;
            snapshot.node = current->node;
            snapshot.expanded = current->node.IsExpanded();
            const WFC::IVector<MUXC::TreeViewNode> children =
                current->node.Children();
            snapshot.children.reserve(children.Size());
            for ( uint32_t i = 0; i < children.Size(); ++i )
                snapshot.children.push_back(children.GetAt(i));
            peerStructure.push_back(std::move(snapshot));

            if ( !isCurrent() )
                return;
        }
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException(
            "WinUI TreeView clear snapshot", e);
        return;
    }
    if ( !isCurrent() )
        return;

    const auto restorePeerStructure =
        [&]()
        {
            // The success path already owns a mutation guard, but an ABI
            // exception unwinds it before entering the catch below. Always
            // guard the rollback itself so a partially restored projection
            // cannot synchronously re-enter wx selection/expansion handlers.
            wxWinUITreePeerMutationGuard rollbackMutation(state);

            // Attach the shallow roots first and then rebuild each child edge
            // in model preorder. Every node is therefore added while it is
            // still shallow; attaching one fully connected 10k-deep subtree
            // here would merely move the stack overflow into the rollback.
            wxWinUIRestorePeerNodes(roots, peerNodes);
            for ( const PeerNodeSnapshot& snapshot : peerStructure )
            {
                wxWinUIRestorePeerNodes(
                    snapshot.node.Children(), snapshot.children);
                snapshot.node.IsExpanded(snapshot.expanded);
            }
        };

    bool mutationSuperseded = false;
    try
    {
        wxWinUITreePeerMutationGuard mutation(m_winui->callbackState);

        // Disconnect leaves before their parents. Clearing the roots first,
        // or clearing a parent which still owns a deep descendant chain,
        // causes TreeView to tear the peer graph down recursively.
        for ( auto snapshot = peerStructure.rbegin();
              snapshot != peerStructure.rend();
              ++snapshot )
        {
            snapshot->node.Children().Clear();
        }
        roots.Clear();
        mutationSuperseded = !isCurrent();
#ifdef WXWINUI_TEST_SUPPORT
        if ( !mutationSuperseded &&
             impl->ShouldFail(
                 wxWinUITreeCtrlTestAccess::PeerMutation::ClearItems) )
        {
            restorePeerStructure();
            if ( isCurrent() )
                (void)ApplySelectionToPeer();
            return;
        }
#endif
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI TreeView clear", e);
        if ( isCurrent() )
        {
            try
            {
                restorePeerStructure();
            }
            catch ( const winrt::hresult_error& rollbackError )
            {
                wxWinUILogException(
                    "WinUI TreeView clear rollback",
                    rollbackError);
                (void)ReconcilePeerStructureFromModel();
            }
            if ( isCurrent() )
                (void)ApplySelectionToPeer();
        }
        else if ( state->GetOwner(generation) == this && m_winui )
        {
            (void)ReconcilePeerStructureFromModel();
        }
        return;
    }

    if ( mutationSuperseded || !isCurrent() )
    {
        if ( state->GetOwner(generation) == this && m_winui )
            (void)ReconcilePeerStructureFromModel();
        return;
    }

    wxWinUITreeItem *liveRoot = ResolveItem(rootId);

    std::vector<wxWinUITreeItem *> deleteOrder;
    wxWinUICollectDeleteOrder(liveRoot, true, deleteOrder);

    if ( selectionAffected )
        CommitSelectionForDeletion(nullptr, liveRoot);

    for ( wxWinUITreeItem * const current : subtree )
    {
        current->deleting = true;
        m_winui->itemsByNode.erase(
            wxWinUITreeNodeIdentity(current->node));
    }

    wxWinUITreeDetachedSubtree removed(
        std::move(m_winui->root));
    m_winui->selectedIds.clear();
    m_winui->selectionId = 0;
    m_winui->focusedId = 0;
    m_winui->rejectedSelectionId = 0;
    m_winui->editItemId = 0;
    m_winui->dragItemId = 0;
    m_winui->dropHighlightId = 0;
    m_winui->itemCount = 0;
    m_winui->expandableItemCount = 0;

    if ( selectionAffected && selectionAllowed )
    {
        wxWinUITreeItem * const oldSelection =
            ResolveItem(oldSelectionId);
        wxTreeEvent selectionEvent(
            wxEVT_TREE_SEL_CHANGED, this, wxTreeItemId());
        selectionEvent.SetOldItem(MakeId(oldSelection));
        HandleWindowEvent(selectionEvent);
        if ( state->GetOwner(generation) != this )
            return;
    }

    for ( wxWinUITreeItem * const current : deleteOrder )
    {
        wxTreeEvent event(wxEVT_TREE_DELETE_ITEM, this, MakeId(current));
        HandleWindowEvent(event);
        if ( state->GetOwner(generation) != this )
            return;
    }

    for ( wxWinUITreeItem * const current : subtree )
        wxWinUIUnregisterItem(m_winui.get(), current);
}

void wxTreeCtrl::Expand(const wxTreeItemId& item)
{
    wxWinUITreeItem *treeItem = GetItem(item);
    wxCHECK_RET( treeItem, wxT("invalid tree item") );
    wxCHECK_RET( !HasFlag(wxTR_HIDE_ROOT) || treeItem != m_winui->root.get(),
                 wxT("can't expand hidden root") );

    SetExpanded(
        treeItem, true, true, PeerExpansionMode::Immediate);
}

void wxTreeCtrl::Collapse(const wxTreeItemId& item)
{
    wxWinUITreeItem *treeItem = GetItem(item);
    wxCHECK_RET( treeItem, wxT("invalid tree item") );
    wxCHECK_RET( !HasFlag(wxTR_HIDE_ROOT) || treeItem != m_winui->root.get(),
                 wxT("can't collapse hidden root") );

    SetExpanded(
        treeItem, false, true, PeerExpansionMode::Immediate);
}

void wxTreeCtrl::CollapseAndReset(const wxTreeItemId& item)
{
    if ( !m_winui )
        return;
    const std::shared_ptr<wxWinUITreeCallbackState> state =
        m_winui->callbackState;
    const std::uint64_t generation = state->Generation();

    Collapse(item);
    if ( state->GetOwner(generation) == this && GetItem(item) )
        DeleteChildren(item);
}

void wxTreeCtrl::Toggle(const wxTreeItemId& item)
{
    if ( IsExpanded(item) )
        Collapse(item);
    else
        Expand(item);
}

void wxTreeCtrl::Unselect()
{
    wxCHECK_RET( !HasFlag(wxTR_MULTIPLE),
                 "doesn't make sense, use UnselectAll() instead" );
    if ( m_winui )
        SelectItem(MakeId(ResolveItem(m_winui->selectionId)), false);
}

void wxTreeCtrl::UnselectAll()
{
    if ( !m_winui || !m_winui->root )
        return;
    if ( !HasFlag(wxTR_MULTIPLE) )
    {
        Unselect();
        return;
    }
    if ( m_winui->selectedIds.empty() )
        return;

    const std::shared_ptr<wxWinUITreeCallbackState> state =
        m_winui->callbackState;
    const std::uint64_t generation = state->Generation();
    const std::uint64_t operation = state->BeginOperation();
    const std::uint64_t oldItemId = m_winui->selectionId;
    wxWinUITreeItem * const oldItem = ResolveItem(oldItemId);

    wxTreeEvent changing(
        wxEVT_TREE_SEL_CHANGING, this, wxTreeItemId());
    changing.SetOldItem(MakeId(oldItem));
    const bool processed =
        HandleWindowEvent(changing);
    if ( state->GetOwner(generation) != this ||
         !state->IsCurrentOperation(operation) )
    {
        return;
    }
    if ( processed && !changing.IsAllowed() )
    {
        if ( m_winui->inPeerSelectionChange )
            SchedulePeerSelectionCorrection();
        return;
    }

    std::vector<std::uint64_t> changedIds;
    changedIds.reserve(m_winui->selectedIds.size());
    for ( const std::uint64_t selectedId : m_winui->selectedIds )
    {
        if ( wxWinUITreeItem * const selected =
                 ResolveItem(selectedId) )
        {
            selected->selected = false;
            changedIds.push_back(selectedId);
        }
    }

    m_winui->selectedIds.clear();
    m_winui->selectionId = 0;
    m_winui->focusedId = 0;
    PeerProjectionResult projection = PeerProjectionResult::Done;
    if ( !changedIds.empty() )
    {
        projection = UpdatePeerItems(changedIds);
        if ( state->GetOwner(generation) != this )
            return;
        if ( projection != PeerProjectionResult::Done )
            SchedulePeerSelectionCorrection();
    }
    if ( !state->IsCurrentOperation(operation) ||
         !m_winui->selectedIds.empty() ||
         m_winui->selectionId != 0 )
    {
        return;
    }
    if ( !ApplySelectionToPeer() &&
         state->GetOwner(generation) == this )
    {
        SchedulePeerSelectionCorrection();
    }
    if ( state->GetOwner(generation) != this ||
         !state->IsCurrentOperation(operation) ||
         !m_winui->selectedIds.empty() )
    {
        return;
    }

    wxTreeEvent changed(
        wxEVT_TREE_SEL_CHANGED, this, wxTreeItemId());
    changed.SetOldItem(MakeId(ResolveItem(oldItemId)));
    HandleWindowEvent(changed);
}

void wxTreeCtrl::SelectItem(const wxTreeItemId& item, bool select)
{
    wxWinUITreeItem *treeItem = GetItem(item);
    wxCHECK_RET( treeItem || !select, wxT("invalid tree item") );
    wxCHECK_RET( !treeItem || !HasFlag(wxTR_HIDE_ROOT) ||
                     treeItem != m_winui->root.get(),
                 wxT("can't select hidden root item") );

    if ( select )
        ChangeSelection(treeItem, true, true);
    else if ( treeItem && treeItem->selected )
    {
        const std::shared_ptr<wxWinUITreeCallbackState> state =
            m_winui->callbackState;
        const std::uint64_t generation = state->Generation();
        const std::uint64_t operation = state->BeginOperation();
        const std::uint64_t itemId = treeItem->id;

        wxTreeEvent changing(
            wxEVT_TREE_SEL_CHANGING, this, wxTreeItemId());
        changing.SetOldItem(MakeId(treeItem));
        const bool processed =
            HandleWindowEvent(changing);
        if ( state->GetOwner(generation) != this ||
             !state->IsCurrentOperation(operation) )
        {
            return;
        }

        treeItem = ResolveItem(itemId);
        if ( !treeItem || treeItem->deleting ||
             (processed && !changing.IsAllowed()) )
        {
            if ( m_winui->inPeerSelectionChange )
                SchedulePeerSelectionCorrection();
            return;
        }

        treeItem->selected = false;
        m_winui->selectedIds.erase(treeItem->id);
        if ( m_winui->selectionId == treeItem->id )
            m_winui->selectionId = 0;
        const PeerProjectionResult projection =
            UpdatePeerItem(treeItem);
        if ( state->GetOwner(generation) != this )
            return;
        if ( projection != PeerProjectionResult::Done )
            SchedulePeerSelectionCorrection();
        if ( !state->IsCurrentOperation(operation) )
            return;
        if ( !ApplySelectionToPeer() &&
             state->GetOwner(generation) == this )
        {
            SchedulePeerSelectionCorrection();
        }
        if ( state->GetOwner(generation) != this ||
             !state->IsCurrentOperation(operation) )
        {
            return;
        }

        treeItem = ResolveItem(itemId);
        if ( !treeItem || treeItem->selected )
            return;

        wxTreeEvent changed(
            wxEVT_TREE_SEL_CHANGED, this, wxTreeItemId());
        changed.SetOldItem(MakeId(treeItem));
        HandleWindowEvent(changed);
    }
}

void wxTreeCtrl::SelectChildren(const wxTreeItemId& parent)
{
    wxWinUITreeItem *treeItem = GetItem(parent);
    wxCHECK_RET( treeItem, wxT("invalid tree item") );
    wxCHECK_RET( HasFlag(wxTR_MULTIPLE),
                 "this only works with multiple selection controls" );

    if ( treeItem->children.empty() )
    {
        UnselectAll();
        return;
    }

    const std::shared_ptr<wxWinUITreeCallbackState> state =
        m_winui->callbackState;
    const std::uint64_t generation = state->Generation();
    const std::uint64_t operation = state->BeginOperation();
    const std::uint64_t parentId = treeItem->id;
    const std::uint64_t firstId = treeItem->children.front()->id;
    wxWinUITreeItem * const oldItem =
        ResolveItem(m_winui->selectionId);
    const std::uint64_t oldId = oldItem ? oldItem->id : 0;

    wxTreeEvent changing(
        wxEVT_TREE_SEL_CHANGING,
        this, MakeId(treeItem->children.front().get()));
    changing.SetOldItem(MakeId(oldItem));
    const bool processed =
        HandleWindowEvent(changing);
    if ( state->GetOwner(generation) != this ||
         !state->IsCurrentOperation(operation) )
    {
        return;
    }

    treeItem = ResolveItem(parentId);
    wxWinUITreeItem * const first = ResolveItem(firstId);
    if ( !treeItem || !first || first->parent != treeItem ||
         (processed && !changing.IsAllowed()) )
    {
        return;
    }

    std::vector<std::uint64_t> changedIds;
    changedIds.reserve(
        m_winui->selectedIds.size() + treeItem->children.size());
    for ( const std::uint64_t selectedId : m_winui->selectedIds )
    {
        if ( wxWinUITreeItem * const selected =
                 ResolveItem(selectedId) )
        {
            selected->selected = false;
            changedIds.push_back(selectedId);
        }
    }
    m_winui->selectedIds.clear();

    for ( const auto& child : treeItem->children )
    {
        child->selected = true;
        m_winui->selectedIds.insert(child->id);
        changedIds.push_back(child->id);
    }

    m_winui->selectionId = treeItem->children.back()->id;
    m_winui->focusedId = m_winui->selectionId;
    const PeerProjectionResult projection =
        UpdatePeerItems(changedIds);
    if ( state->GetOwner(generation) != this )
        return;
    if ( projection != PeerProjectionResult::Done )
        SchedulePeerSelectionCorrection();
    if ( !state->IsCurrentOperation(operation) )
        return;
    if ( !ApplySelectionToPeer() &&
         state->GetOwner(generation) == this )
    {
        SchedulePeerSelectionCorrection();
    }
    if ( state->GetOwner(generation) != this ||
         !state->IsCurrentOperation(operation) )
    {
        return;
    }

    wxWinUITreeItem * const liveFirst = ResolveItem(firstId);
    if ( !liveFirst || liveFirst->parent != ResolveItem(parentId) )
        return;

    wxTreeEvent changed(
        wxEVT_TREE_SEL_CHANGED, this, MakeId(liveFirst));
    changed.SetOldItem(MakeId(ResolveItem(oldId)));
    HandleWindowEvent(changed);
}

void wxTreeCtrl::EnsureVisible(const wxTreeItemId& item)
{
    wxWinUITreeItem *treeItem = GetItem(item);
    if ( !treeItem )
        return;
    const std::shared_ptr<wxWinUITreeCallbackState> state =
        m_winui->callbackState;
    const std::uint64_t operation = state->BeginOperation();
    (void)EnsureVisibleItem(treeItem->id, operation, true);
}

bool wxTreeCtrl::EnsureVisibleItem(std::uint64_t itemId,
                                   std::uint64_t operation,
                                   bool scroll)
{
    wxWinUITreeItem *treeItem = ResolveItem(itemId);
    if ( !treeItem || !operation )
        return false;

    const std::shared_ptr<wxWinUITreeCallbackState> state =
        m_winui->callbackState;
    const std::uint64_t generation = state->Generation();

    std::vector<std::uint64_t> projectionIds;
    const AncestorExpansionResult expansion =
        ExpandAncestorPath(itemId, operation, &projectionIds);
    if ( expansion == AncestorExpansionResult::Stale )
        return false;

    if ( !projectionIds.empty() )
    {
        const PeerProjectionResult projection =
            UpdatePeerItems(projectionIds);
        if ( state->GetOwner(generation) != this ||
             !state->IsCurrentOperation(operation) )
        {
            return false;
        }
        if ( projection != PeerProjectionResult::Done )
        {
            SchedulePeerSelectionCorrection();
        }
    }

    if ( expansion == AncestorExpansionResult::PeerDeferred )
        return false;

    treeItem = ResolveItem(itemId);
    const bool logicallyVisible =
        treeItem && wxWinUIItemIsLogicallyVisible(this, treeItem);
    if ( logicallyVisible && scroll )
        ScrollTo(MakeId(treeItem));
    return state->GetOwner(generation) == this &&
           state->IsCurrentOperation(operation) &&
           logicallyVisible;
}

void wxTreeCtrl::ScrollTo(const wxTreeItemId& item)
{
    wxWinUITreeItem *treeItem = GetItem(item);
    if ( treeItem && m_winui && HasFlag(wxTR_HIDE_ROOT) &&
         treeItem == m_winui->root.get() )
    {
        // The virtual root has no peer container. Keep ScrollTo() free of
        // expansion side effects, but map it to the first visible top-level
        // item as the generic/MSW ports do.
        treeItem = treeItem->children.empty()
            ? nullptr
            : treeItem->children.front().get();
    }
    if ( !treeItem ||
         !wxWinUIItemIsLogicallyVisible(this, treeItem) )
    {
        return;
    }

    wxWinUITreeCtrlImpl * const impl = m_winui.get();
    const std::shared_ptr<wxWinUITreeCallbackState> state =
        impl->callbackState;
    const std::uint64_t generation = state->Generation();
    const std::uint64_t itemId = treeItem->id;
    try
    {
        impl->host.ForceRender();
        wxTreeCtrl *owner = state->GetOwner(generation);
        treeItem = owner == this ? owner->ResolveItem(itemId) : nullptr;
        if ( !treeItem || !owner->m_winui ||
             owner->m_winui.get() != impl ||
             !wxWinUIItemIsLogicallyVisible(owner, treeItem) )
        {
            return;
        }

        wxWinUIHookTreeListNaming(m_winui.get());
        owner = state->GetOwner(generation);
        treeItem = owner == this ? owner->ResolveItem(itemId) : nullptr;
        if ( !treeItem || !owner->m_winui ||
             owner->m_winui.get() != impl )
        {
            return;
        }
        if ( impl->treeList )
        {
            impl->treeList.ScrollIntoView(treeItem->node);
            owner = state->GetOwner(generation);
            treeItem = owner == this ? owner->ResolveItem(itemId) : nullptr;
            if ( !treeItem || !owner->m_winui ||
                 owner->m_winui.get() != impl )
            {
                return;
            }

            // ScrollIntoView() queues virtualization work. UpdateLayout() is
            // the synchronous boundary needed by IsVisible()/focus callers;
            // revalidate after it because XAML layout can run wx callbacks.
            impl->treeView.UpdateLayout();
            owner = state->GetOwner(generation);
            treeItem = owner == this ? owner->ResolveItem(itemId) : nullptr;
            if ( !treeItem || !owner->m_winui ||
                 owner->m_winui.get() != impl )
            {
                return;
            }
            impl->host.ForceRender();
        }
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI TreeView scroll into view", e);
    }
}

wxTextCtrl *wxTreeCtrl::EditLabel(const wxTreeItemId& item,
                                  wxClassInfo* textCtrlClass)
{
    wxWinUITreeItem *treeItem = GetItem(item);
    wxCHECK_MSG( treeItem, nullptr, wxT("invalid tree item") );
    const std::uint64_t itemId = treeItem->id;
    const std::shared_ptr<wxWinUITreeCallbackState> state =
        m_winui->callbackState;
    const std::uint64_t generation = state->Generation();

    // Only one edit at a time.
    if ( m_editControl )
        EndEditLabel(
            MakeId(ResolveItem(m_winui->editItemId)), true);
    if ( state->GetOwner(generation) != this )
        return nullptr;
    if ( m_editControl )
        return nullptr;
    treeItem = ResolveItem(itemId);
    if ( !treeItem || treeItem->deleting )
        return nullptr;

    const std::uint64_t operation = state->BeginOperation();

    wxTreeEvent event(wxEVT_TREE_BEGIN_LABEL_EDIT, this, item);
    const bool processed = HandleWindowEvent(event);
    if ( state->GetOwner(generation) != this ||
         !state->IsCurrentOperation(operation) )
    {
        return nullptr;
    }
    treeItem = ResolveItem(itemId);
    if ( !treeItem || treeItem->deleting ||
         (processed && !event.IsAllowed()) )
    {
        return nullptr;
    }

    // Editing is a public operation, not a realized-container-only seam.
    // Expand and scroll the path first so callers don't have to force XAML
    // layout themselves. EnsureVisibleItem() shares this operation ticket and
    // revalidates across every observable expansion/layout boundary.
    if ( !EnsureVisibleItem(itemId, operation, true) ||
         state->GetOwner(generation) != this ||
         !state->IsCurrentOperation(operation) )
    {
        return nullptr;
    }
    treeItem = ResolveItem(itemId);
    if ( !treeItem || treeItem->deleting )
        return nullptr;

    wxRect rect;
    if ( !GetBoundingRect(item, rect, true) )
    {
        // Never guess a row rectangle after a failed realization: doing so can
        // place the editor over a different virtualized item.
        return nullptr;
    }
    if ( state->GetOwner(generation) != this ||
         !state->IsCurrentOperation(operation) )
    {
        return nullptr;
    }
    treeItem = ResolveItem(itemId);
    if ( !treeItem || treeItem->deleting )
        return nullptr;
    const wxString editValue = treeItem->text;
    const std::uint64_t editTextMutation = treeItem->textMutation;
    const int editMinWidth = FromDIP(80);
    if ( state->GetOwner(generation) != this ||
         !state->IsCurrentOperation(operation) )
    {
        return nullptr;
    }
    treeItem = ResolveItem(itemId);
    if ( !treeItem || treeItem->deleting ||
         treeItem->textMutation != editTextMutation )
    {
        return nullptr;
    }

    // The edit control is a plain wx child of the tree: its HWND (and island)
    // paints above the TreeView island, which is pinned to the bottom.
    wxObject * const created = textCtrlClass
        ? textCtrlClass->CreateObject()
        : nullptr;
    if ( state->GetOwner(generation) != this ||
         !state->IsCurrentOperation(operation) )
    {
        delete created;
        return nullptr;
    }
    wxTextCtrl *text = wxDynamicCast(created, wxTextCtrl);
    if ( !text )
    {
        delete created;
        return nullptr;
    }

    wxWeakRef<wxWindow> textLifetime(text);
    const bool textCreated =
        text->Create(this, wxID_ANY, editValue,
                     rect.GetPosition(),
                     wxSize(wxMax(rect.width, editMinWidth), rect.height),
                     wxTE_PROCESS_ENTER);
    if ( textLifetime.get() != text )
        return nullptr;
    if ( state->GetOwner(generation) != this ||
         !state->IsCurrentOperation(operation) )
    {
        if ( wxWindow * const orphan = textLifetime.get() )
            orphan->Destroy();
        return nullptr;
    }
    if ( !textCreated )
    {
        delete text;
        return nullptr;
    }
    treeItem = ResolveItem(itemId);
    if ( !treeItem || treeItem->deleting ||
         treeItem->textMutation != editTextMutation )
    {
        text->Destroy();
        return nullptr;
    }

    m_editControl = text;
    m_winui->editItemId = itemId;
    const std::uint64_t editSession = ++m_winui->editSession;
    const std::weak_ptr<wxWinUITreeCallbackState> weakState = state;

    text->Bind(wxEVT_TEXT_ENTER,
        [weakState, generation, editSession, itemId](wxCommandEvent&)
        {
            const auto locked = weakState.lock();
            wxTreeCtrl * const owner =
                locked ? locked->GetOwner(generation) : nullptr;
            if ( owner && owner->m_winui &&
                 owner->m_winui->editSession == editSession &&
                 owner->m_winui->editItemId == itemId )
            {
                owner->EndEditLabel(
                    owner->MakeId(owner->ResolveItem(itemId)), false);
            }
        });
    text->Bind(wxEVT_KILL_FOCUS,
        [weakState, generation, editSession, itemId](wxFocusEvent& e)
        {
            e.Skip();
            const auto locked = weakState.lock();
            wxTreeCtrl * const owner =
                locked ? locked->GetOwner(generation) : nullptr;
            if ( owner && owner->m_winui &&
                 owner->m_winui->editSession == editSession &&
                 owner->m_winui->editItemId == itemId )
            {
                owner->EndEditLabel(
                    owner->MakeId(owner->ResolveItem(itemId)), false);
            }
        });
    text->Bind(wxEVT_CHAR_HOOK,
        [weakState, generation, editSession, itemId](wxKeyEvent& e)
        {
            if ( e.GetKeyCode() == WXK_ESCAPE )
            {
                const auto locked = weakState.lock();
                wxTreeCtrl * const owner =
                    locked ? locked->GetOwner(generation) : nullptr;
                if ( owner && owner->m_winui &&
                     owner->m_winui->editSession == editSession &&
                     owner->m_winui->editItemId == itemId )
                {
                    owner->EndEditLabel(
                        owner->MakeId(owner->ResolveItem(itemId)), true);
                }
            }
            else
                e.Skip();
        });
    text->Bind(wxEVT_DESTROY,
        [weakState, generation, editSession](wxWindowDestroyEvent& e)
        {
            e.Skip();
            const auto locked = weakState.lock();
            wxTreeCtrl * const owner =
                locked ? locked->GetOwner(generation) : nullptr;
            if ( owner && owner->m_winui &&
                 owner->m_winui->editSession == editSession )
            {
                owner->m_editControl = nullptr;
                owner->m_winui->editItemId = 0;
                ++owner->m_winui->editSession;
            }
        });

    text->SelectAll();
    if ( state->GetOwner(generation) != this ||
         !m_winui ||
         m_winui->editSession != editSession ||
         m_editControl != text )
    {
        return nullptr;
    }
    text->SetFocus();
    if ( state->GetOwner(generation) != this ||
         !m_winui ||
         m_winui->editSession != editSession ||
         m_editControl != text )
    {
        return nullptr;
    }

    return text;
}

wxTextCtrl *wxTreeCtrl::GetEditControl() const
{
    return m_editControl;
}

void wxTreeCtrl::EndEditLabel(const wxTreeItemId& item,
                              bool discardChanges)
{
    if ( !m_editControl || !m_winui )
        return;

    wxWinUITreeItem * const requested = GetItem(item);
    if ( requested && requested->id != m_winui->editItemId )
        return;

    wxTextCtrl * const text = m_editControl;
    const std::uint64_t itemId = m_winui->editItemId;
    wxWinUITreeItem * const treeItem = ResolveItem(itemId);
    const std::uint64_t textMutation =
        treeItem ? treeItem->textMutation : 0;
    const std::shared_ptr<wxWinUITreeCallbackState> state =
        m_winui->callbackState;
    const std::uint64_t generation = state->Generation();
    wxWeakRef<wxWindow> textLifetime(text);
    m_editControl = nullptr;
    m_winui->editItemId = 0;
    ++m_winui->editSession;
    const std::uint64_t endedEditSession = m_winui->editSession;

    const wxString value = text->GetValue();
    if ( textLifetime.get() != text ||
         state->GetOwner(generation) != this )
        return;

    // We may be called from one of the edit control's own event handlers, so
    // don't delete it right away.
    text->Hide();
    if ( textLifetime.get() != text ||
         state->GetOwner(generation) != this )
        return;
    wxTheApp->ScheduleForDestruction(text);
    if ( state->GetOwner(generation) != this )
        return;

    wxWinUITreeItem *liveItem = ResolveItem(itemId);
    if ( !liveItem || liveItem->deleting )
        return;

    const std::uint64_t operation = state->BeginOperation();

    wxTreeEvent event(
        wxEVT_TREE_END_LABEL_EDIT, this, MakeId(liveItem));
    event.SetLabel(value);
    event.SetEditCanceled(discardChanges);

    const bool processed = HandleWindowEvent(event);
    if ( state->GetOwner(generation) != this ||
         !state->IsCurrentOperation(operation) )
    {
        return;
    }

    if ( !discardChanges && (!processed || event.IsAllowed()) )
    {
        liveItem = ResolveItem(itemId);
        if ( liveItem && liveItem->textMutation == textMutation )
        {
            SetItemText(MakeId(liveItem), value);
            if ( state->GetOwner(generation) != this ||
                 !state->IsCurrentOperation(operation) )
                return;
        }
    }

    // SetItemText() projects through WinUI and can run application callbacks.
    // Never let a completed stale edit steal focus from a newer selection or
    // label-edit session created by such a callback.
    if ( state->GetOwner(generation) != this ||
         !state->IsCurrentOperation(operation) ||
         !m_winui ||
         m_winui->editSession != endedEditSession ||
         m_winui->editItemId != 0 ||
         m_editControl )
    {
        return;
    }

    SetFocus();
}

void wxTreeCtrl::SortChildren(const wxTreeItemId& item)
{
    wxWinUITreeItem *treeItem = GetItem(item);
    wxCHECK_RET( treeItem, wxT("invalid tree item") );

    if ( treeItem->children.size() < 2 )
        return;

    const std::shared_ptr<wxWinUITreeCallbackState> state =
        m_winui->callbackState;
    const std::uint64_t generation = state->Generation();
    const std::uint64_t operation = state->BeginOperation();
    const std::uint64_t parentId = treeItem->id;

    std::vector<std::uint64_t> oldOrder;
    oldOrder.reserve(treeItem->children.size());
    for ( const auto& child : treeItem->children )
        oldOrder.push_back(child->id);
    std::vector<std::uint64_t> newOrder = oldOrder;

    struct SortAborted final {};
    try
    {
        std::stable_sort(newOrder.begin(), newOrder.end(),
            [state, generation, operation](
                std::uint64_t aId, std::uint64_t bId)
            {
                wxTreeCtrl * const owner =
                    state->GetOwner(generation);
                if ( !owner )
                    throw SortAborted{};

                wxWinUITreeItem * const a =
                    owner->ResolveItem(aId);
                wxWinUITreeItem * const b =
                    owner->ResolveItem(bId);
                if ( !a || !b )
                    throw SortAborted{};

                const int result =
                    owner->OnCompareItems(
                        owner->MakeId(a), owner->MakeId(b));
                if ( state->GetOwner(generation) != owner ||
                     !state->IsCurrentOperation(operation) )
                {
                    throw SortAborted{};
                }
                return result < 0;
            });
    }
    catch ( const SortAborted& )
    {
        return;
    }

    if ( state->GetOwner(generation) != this ||
         !state->IsCurrentOperation(operation) )
    {
        return;
    }

    treeItem = ResolveItem(parentId);
    if ( !treeItem || treeItem->children.size() != oldOrder.size() )
        return;
    for ( size_t i = 0; i < oldOrder.size(); ++i )
    {
        if ( treeItem->children[i]->id != oldOrder[i] )
            return;
    }

    std::unordered_map<std::uint64_t, size_t> oldIndexes;
    oldIndexes.reserve(oldOrder.size());
    for ( size_t i = 0; i < oldOrder.size(); ++i )
        oldIndexes.emplace(oldOrder[i], i);
    std::vector<std::unique_ptr<wxWinUITreeItem>> sorted(
        newOrder.size());

    WFC::IVector<MUXC::TreeViewNode> peerChildren{ nullptr };
    wxWinUITreePeerForestSnapshot oldPeerForest;
    wxWinUITreePeerForestSnapshot newPeerForest;
    try
    {
        peerChildren =
            wxWinUIGetPeerChildren(this, m_winui.get(), treeItem);
        oldPeerForest =
            wxWinUICapturePeerForest(peerChildren);
        if ( oldPeerForest.roots.size() != oldOrder.size() )
        {
            wxFAIL_MSG("WinUI TreeView peer/model order diverged");
            return;
        }
        for ( size_t i = 0; i < oldOrder.size(); ++i )
        {
            wxWinUITreeItem * const child = ResolveItem(oldOrder[i]);
            if ( !child || child->parent != treeItem ||
                 wxWinUITreeNodeIdentity(oldPeerForest.roots[i]) !=
                     wxWinUITreeNodeIdentity(child->node) )
            {
                wxFAIL_MSG("WinUI TreeView peer/model order diverged");
                return;
            }
        }
        newPeerForest = oldPeerForest;
        newPeerForest.roots.clear();
        newPeerForest.roots.reserve(newOrder.size());
        for ( const std::uint64_t id : newOrder )
        {
            wxWinUITreeItem * const child = ResolveItem(id);
            if ( !child || child->parent != treeItem )
                return;
            newPeerForest.roots.push_back(child->node);
        }
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException(
            "WinUI TreeView sort children lookup", e);
        return;
    }
    try
    {
        wxWinUITreePeerMutationGuard mutation(m_winui->callbackState);
        wxWinUIApplyPeerForest(
            peerChildren, oldPeerForest, newPeerForest);

#ifdef WXWINUI_TEST_SUPPORT
        if ( m_winui->ShouldFail(
                 wxWinUITreeCtrlTestAccess::PeerMutation::ClearItems) )
        {
            wxWinUIApplyPeerForest(
                peerChildren, newPeerForest, oldPeerForest);
            if ( state->GetOwner(generation) == this )
                (void)ApplySelectionToPeer();
            return;
        }
#endif
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI TreeView sort", e);
        try
        {
            wxTreeCtrl * const owner =
                state->GetOwner(generation);
            if ( owner == this && owner->m_winui &&
                 owner->m_winui->callbackState == state &&
                 peerChildren )
            {
                wxWinUITreePeerMutationGuard rollbackMutation(state);
                wxWinUIApplyPeerForest(
                    peerChildren, newPeerForest, oldPeerForest);
            }
        }
        catch ( const winrt::hresult_error& rollbackError )
        {
            wxWinUILogException(
                "WinUI TreeView sort rollback",
                rollbackError);
            (void)ReconcilePeerStructureFromModel();
        }
        if ( state->GetOwner(generation) == this )
            (void)ApplySelectionToPeer();
        return;
    }

    if ( state->GetOwner(generation) != this ||
         !state->IsCurrentOperation(operation) )
    {
        if ( state->GetOwner(generation) == this && m_winui )
            (void)ReconcilePeerStructureFromModel();
        return;
    }
    treeItem = ResolveItem(parentId);
    if ( !treeItem || treeItem->children.size() != oldOrder.size() )
        return;
    for ( size_t i = 0; i < oldOrder.size(); ++i )
    {
        if ( treeItem->children[i]->id != oldOrder[i] )
            return;
    }

    for ( size_t i = 0; i < newOrder.size(); ++i )
        sorted[i] = std::move(
            treeItem->children[oldIndexes.at(newOrder[i])]);

    treeItem->children = std::move(sorted);
    wxWinUIReindexChildren(treeItem);
    m_winui->BumpModelRevision();
    if ( state->GetOwner(generation) == this )
        (void)ApplySelectionToPeer();
}

bool wxTreeCtrl::HasExpandableItem() const
{
    return m_winui && m_winui->expandableItemCount != 0;
}

static MUX::FrameworkElement
wxWinUIFindNamedElement(const MUX::DependencyObject& root,
                        const wchar_t *name)
{
    if ( !root )
        return nullptr;

    if ( const auto element = root.try_as<MUX::FrameworkElement>() )
    {
        if ( element.Name() == name )
            return element;
    }

    const int count = MUXM::VisualTreeHelper::GetChildrenCount(root);
    for ( int i = 0; i < count; ++i )
    {
        if ( const auto found = wxWinUIFindNamedElement(
                 MUXM::VisualTreeHelper::GetChild(root, i), name) )
        {
            return found;
        }
    }

    return nullptr;
}

static bool wxWinUIIsExpanderElement(
    const MUX::DependencyObject& object)
{
    if ( !object )
        return false;

    const auto element = object.try_as<MUX::FrameworkElement>();
    if ( !element )
        return false;
    if ( element.Visibility() != MUX::Visibility::Visible )
        return false;
    const wxString name(element.Name().c_str());
    return name.Contains("Expand") ||
           name.Contains("Collapse") ||
           name.Contains("Chevron");
}

static MUX::FrameworkElement wxWinUIFindExpanderElement(
    const MUX::DependencyObject& root)
{
    if ( !root )
        return nullptr;
    if ( wxWinUIIsExpanderElement(root) )
        return root.try_as<MUX::FrameworkElement>();

    const int count = MUXM::VisualTreeHelper::GetChildrenCount(root);
    for ( int i = 0; i < count; ++i )
    {
        if ( const auto found = wxWinUIFindExpanderElement(
                 MUXM::VisualTreeHelper::GetChild(root, i)) )
        {
            return found;
        }
    }
    return nullptr;
}

template <typename IsCurrent>
static bool wxWinUIGetElementRect(
    wxTreeCtrl *anchor,
    const MUX::FrameworkElement& element,
    wxRect& rect,
    const IsCurrent& isCurrent)
{
    if ( !anchor || !element || !isCurrent() )
        return false;

    const double width = element.ActualWidth();
    if ( !isCurrent() )
        return false;

    const double height = element.ActualHeight();
    if ( !isCurrent() ||
         !(width > 0.0) || !(height > 0.0) ||
         !std::isfinite(width) || !std::isfinite(height) )
    {
        return false;
    }

    winrt::Windows::Foundation::Rect clientBounds{};
    if ( wxWinUIVisualCoordinates::ElementBoundsToClient(
             anchor,
             element,
             winrt::Windows::Foundation::Rect{
                 0.0f,
                 0.0f,
                  static_cast<float>(width),
                  static_cast<float>(height)},
             &clientBounds) != wxWinUICoordinateResult::Mapped ||
         !isCurrent() )
    {
        return false;
    }

    const int left =
        static_cast<int>(std::floor(clientBounds.X));
    const int top =
        static_cast<int>(std::floor(clientBounds.Y));
    const int right = static_cast<int>(
        std::ceil(clientBounds.X + clientBounds.Width));
    const int bottom = static_cast<int>(
        std::ceil(clientBounds.Y + clientBounds.Height));
    const wxRect candidate(
        left,
        top,
        wxMax(0, right - left),
        wxMax(0, bottom - top));

    if ( !isCurrent() )
        return false;

    rect = candidate;
    return true;
}

bool wxTreeCtrl::GetItemPeerRect(wxWinUITreeItem *item,
                                 wxRect& rect,
                                 bool textOnly) const
{
    if ( !m_winui || !m_winui->treeView || !item || !item->node )
        return false;

    wxWinUITreeCtrlImpl * const impl = m_winui.get();
    const std::shared_ptr<wxWinUITreeCallbackState> state =
        impl->callbackState;
    const std::uint64_t generation = state->Generation();
    const std::uint64_t itemId = item->id;
    const MUXC::TreeViewNode node = item->node;
    const MUXC::TreeView treeView = impl->treeView;
    const auto isCurrent =
        [state, generation, impl, this, itemId, node, treeView]()
        {
            const wxTreeCtrl * const owner =
                state->GetOwner(generation);
            if ( owner != this || !owner->m_winui ||
                 owner->m_winui.get() != impl ||
                 owner->m_winui->callbackState != state ||
                 winrt::get_abi(owner->m_winui->treeView) !=
                     winrt::get_abi(treeView) )
            {
                return false;
            }
            wxWinUITreeItem * const liveItem =
                owner->ResolveItem(itemId);
            return liveItem &&
                   winrt::get_abi(liveItem->node) ==
                       winrt::get_abi(node);
        };

    try
    {
        const auto container = treeView.ContainerFromNode(node);
        if ( !isCurrent() )
            return false;
        MUX::FrameworkElement element =
            container.try_as<MUX::FrameworkElement>();
        if ( textOnly )
        {
            element = wxWinUIFindNamedElement(container, L"wxTreeLabel");
            if ( !isCurrent() )
                return false;
        }

        const bool measured = wxWinUIGetElementRect(
            const_cast<wxTreeCtrl *>(this), element, rect, isCurrent);
        return isCurrent() && measured;
    }
    catch ( const winrt::hresult_error& )
    {
        return false;
    }
}

bool wxTreeCtrl::GetBoundingRect(const wxTreeItemId& item,
                                 wxRect& rect,
                                 bool textOnly) const
{
    wxWinUITreeItem *treeItem = GetItem(item);
    if ( !treeItem )
        return false;

    return GetItemPeerRect(treeItem, rect, textOnly);
}

#ifdef WXWINUI_TEST_SUPPORT
bool wxWinUITreeCtrlTestAccess::GetMeasuredItemParts(const wxTreeCtrl &control,
                                                     const wxTreeItemId &item,
                                                     MeasuredItemParts *parts)
{
    const wxTreeCtrl *const self = &control;
    wxCHECK_MSG( parts, false, "null TreeView measurement output" );
    *parts = {};

    wxWinUITreeItem *const treeItem = self->GetItem(item);
    if ( !self->m_winui || !self->m_winui->treeView || !treeItem )
        return false;

    wxWinUITreeCtrlImpl *const impl = self->m_winui.get();
    const std::shared_ptr<wxWinUITreeCallbackState> state =
        impl->callbackState;
    const std::uint64_t generation = state->Generation();
    const std::uint64_t itemId = treeItem->id;
    const MUXC::TreeViewNode node = treeItem->node;
    const MUXC::TreeView treeView = impl->treeView;
    const auto isCurrent =
        [state, generation, impl, self, itemId, node, treeView]()
    {
        const wxTreeCtrl *const owner = state->GetOwner(generation);
        if ( owner != self || !owner->m_winui || owner->m_winui.get() != impl ||
             owner->m_winui->callbackState != state ||
             winrt::get_abi(owner->m_winui->treeView) !=
                 winrt::get_abi(treeView) )
        {
            return false;
        }
        wxWinUITreeItem *const liveItem = owner->ResolveItem(itemId);
        return liveItem &&
               winrt::get_abi(liveItem->node) == winrt::get_abi(node);
    };

    try
    {
        const auto container = treeView.ContainerFromNode(node);
        if ( !isCurrent() || !container )
            return false;

        const auto itemElement =
            container.as<MUX::FrameworkElement>();
        const auto label =
            wxWinUIFindNamedElement(container, L"wxTreeLabel");
        if ( !isCurrent() )
            return false;
        if ( !wxWinUIGetElementRect(const_cast<wxTreeCtrl *>(self), itemElement,
                                    parts->item, isCurrent) ||
             !isCurrent() )
        {
            return false;
        }
        if ( !wxWinUIGetElementRect(const_cast<wxTreeCtrl *>(self), label,
                                    parts->label, isCurrent) ||
             !isCurrent() )
        {
            return false;
        }

        if ( const auto expander =
                 wxWinUIFindExpanderElement(container) )
        {
            wxWinUIGetElementRect(const_cast<wxTreeCtrl *>(self), expander,
                                  parts->expander, isCurrent);
            if ( !isCurrent() )
                return false;
        }
        if ( const auto stateImage =
                 wxWinUIFindNamedElement(
                     container, L"wxTreeStateImage") )
        {
            const bool visible =
                stateImage.Visibility() == MUX::Visibility::Visible;
            if ( !isCurrent() )
                return false;
            if ( visible )
            {
                wxWinUIGetElementRect(const_cast<wxTreeCtrl *>(self),
                                      stateImage, parts->stateImage, isCurrent);
                if ( !isCurrent() )
                    return false;
            }
        }
        if ( const auto image =
                 wxWinUIFindNamedElement(container, L"wxTreeImage") )
        {
            const bool visible =
                image.Visibility() == MUX::Visibility::Visible;
            if ( !isCurrent() )
                return false;
            if ( visible )
            {
                wxWinUIGetElementRect(const_cast<wxTreeCtrl *>(self), image,
                                      parts->image, isCurrent);
                if ( !isCurrent() )
                    return false;
            }
        }
        return isCurrent();
    }
    catch ( const winrt::hresult_error& )
    {
        return false;
    }
}
#endif // WXWINUI_TEST_SUPPORT

#ifdef WXWINUI_TEST_SUPPORT
bool wxWinUITreeCtrlTestAccess::GetPeerIndent(const wxTreeCtrl &control,
                                              const wxTreeItemId &item,
                                              double *leading, double *trailing)
{
    const wxTreeCtrl *const self = &control;
    wxCHECK_MSG( leading && trailing, false,
                 "null TreeView indentation output" );
    *leading = 0.0;
    *trailing = 0.0;

    wxWinUITreeItem *const treeItem = self->GetItem(item);
    if ( !self->m_winui || !self->m_winui->treeView || !treeItem ||
         !self->m_winui->callbackState )
    {
        return false;
    }

    wxWinUITreeCtrlImpl *const impl = self->m_winui.get();
    const std::shared_ptr<wxWinUITreeCallbackState> state =
        impl->callbackState;
    const std::uint64_t generation = state->Generation();
    const std::uint64_t itemId = treeItem->id;
    const MUXC::TreeViewNode node = treeItem->node;
    try
    {
        const auto container =
            impl->treeView.ContainerFromNode(node)
                .try_as<MUXC::TreeViewItem>();
        wxTreeCtrl *const owner = state->GetOwner(generation);
        if ( owner != self || !owner->m_winui || owner->m_winui.get() != impl ||
             owner->ResolveItem(itemId) != treeItem || !container )
        {
            return false;
        }

        const MUX::Thickness indentation =
            container.TreeViewItemTemplateSettings().Indentation();
        if ( state->GetOwner(generation) != self || !self->m_winui ||
             self->m_winui.get() != impl ||
             self->ResolveItem(itemId) != treeItem )
        {
            return false;
        }

        *leading = impl->rightToLeft
            ? indentation.Right
            : indentation.Left;
        *trailing = impl->rightToLeft
            ? indentation.Left
            : indentation.Right;
        return true;
    }
    catch ( const winrt::hresult_error& )
    {
        return false;
    }
}
#endif // WXWINUI_TEST_SUPPORT

#ifdef WXWINUI_TEST_SUPPORT
bool wxWinUITreeCtrlTestAccess::IsPeerDropHighlighted(const wxTreeCtrl &control,
                                                      const wxTreeItemId &item)
{
    const wxTreeCtrl *const self = &control;
    wxWinUITreeItem *const treeItem = self->GetItem(item);
    if ( !self->m_winui || !self->m_winui->treeView || !treeItem )
        return false;

    try
    {
        const auto container =
            self->m_winui->treeView.ContainerFromNode(treeItem->node);
        const auto border = wxWinUIFindNamedElement(
            container, L"wxTreeContentBorder");
        const auto borderControl =
            border.try_as<MUXC::Border>();
        if ( !borderControl )
            return false;
        const auto brush = borderControl.Background()
            .try_as<MUXM::SolidColorBrush>();
        if ( !brush )
            return false;

        const wxColour expected =
            wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT);
        const auto actual = brush.Color();
        return treeItem->dropHighlighted &&
               actual.R == expected.Red() &&
               actual.G == expected.Green() &&
               actual.B == expected.Blue() &&
               actual.A == expected.Alpha();
    }
    catch ( const winrt::hresult_error& )
    {
        return false;
    }
}
#endif // WXWINUI_TEST_SUPPORT

#ifdef WXWINUI_TEST_SUPPORT
wxString
wxWinUITreeCtrlTestAccess::GetPeerAutomationName(const wxTreeCtrl &control,
                                                 const wxTreeItemId &item)
{
    const wxTreeCtrl *const self = &control;
    wxWinUITreeItem *const treeItem = self->GetItem(item);
    if ( !self->m_winui || !self->m_winui->treeView || !treeItem )
        return wxString();

    try
    {
        const auto container =
            self->m_winui->treeView.ContainerFromNode(treeItem->node);
        return container
            ? wxString(
                  MUX::Automation::AutomationProperties::GetName(
                      container).c_str())
            : wxString();
    }
    catch ( const winrt::hresult_error& )
    {
        return wxString();
    }
}
#endif // WXWINUI_TEST_SUPPORT

#ifdef WXWINUI_TEST_SUPPORT
wxString wxWinUITreeCtrlTestAccess::GetPeerItemText(const wxTreeCtrl &control,
                                                    const wxTreeItemId &item)
{
    const wxTreeCtrl *const self = &control;
    wxWinUITreeItem *const treeItem = self->GetItem(item);
    if ( !treeItem || !treeItem->content )
        return wxString();

    try
    {
        return treeItem->content.HasKey(L"Text")
            ? wxString(
                  winrt::unbox_value_or<winrt::hstring>(
                      treeItem->content.Lookup(L"Text"), {}).c_str())
            : wxString();
    }
    catch ( const winrt::hresult_error& )
    {
        return wxString();
    }
}
#endif // WXWINUI_TEST_SUPPORT

#ifdef WXWINUI_TEST_SUPPORT
std::uintptr_t
wxWinUITreeCtrlTestAccess::GetPeerItemImageIdentity(const wxTreeCtrl &control,
                                                    const wxTreeItemId &item)
{
    const wxTreeCtrl *const self = &control;
    wxWinUITreeItem *const treeItem = self->GetItem(item);
    if ( !treeItem || !treeItem->content )
        return 0;

    try
    {
        return treeItem->content.HasKey(L"Image")
            ? wxWinUIInspectableIdentity(
                  treeItem->content.Lookup(L"Image"))
            : 0;
    }
    catch ( const winrt::hresult_error& )
    {
        return 0;
    }
}
#endif // WXWINUI_TEST_SUPPORT

#ifdef WXWINUI_TEST_SUPPORT
bool wxWinUITreeCtrlTestAccess::RefreshForScale(wxTreeCtrl &control,
                                                double scale)
{
    wxTreeCtrl *const self = &control;
    if ( !std::isfinite(scale) || scale <= 0.0 || !self->m_winui ||
         !self->m_winui->callbackState || self->m_winui->closed )
    {
        return false;
    }

    wxWinUITreeCtrlImpl *const impl = self->m_winui.get();
    const auto state = impl->callbackState;
    const std::uint64_t callbackGeneration = state->Generation();
    impl->projectionScaleOverrideForTesting = scale;
    const wxTreeCtrl::PeerProjectionResult result =
        self->RefreshProjectedItems();

    const wxTreeCtrl *const owner = state->GetOwner(callbackGeneration);
    return result == wxTreeCtrl::PeerProjectionResult::Done && owner == self &&
           owner->m_winui && owner->m_winui.get() == impl &&
           owner->m_winui->callbackState == state &&
           !impl->projectionInProgress && !impl->projectionReplayScheduled;
}
#endif // WXWINUI_TEST_SUPPORT

#ifdef WXWINUI_TEST_SUPPORT
bool wxWinUITreeCtrlTestAccess::GetPeerItemImageProjection(
    const wxTreeCtrl &control, const wxTreeItemId &item, wxSize *imagePixelSize,
    wxSize *stateImagePixelSize, std::uint64_t *generation,
    wxSize *imageDIPSize, wxSize *stateImageDIPSize)
{
    const wxTreeCtrl *const self = &control;
    wxWinUITreeItem *const treeItem = self->GetItem(item);
    if ( !treeItem || !treeItem->content ||
         !treeItem->contentAttached )
    {
        return false;
    }

    const auto readPixelSize =
        [&treeItem](const wchar_t *key)
        {
            if ( !treeItem->content.HasKey(key) )
                return wxSize();

            const auto source =
                treeItem->content.Lookup(key)
                    .try_as<MUXMI::WriteableBitmap>();
            return source
                ? wxSize(source.PixelWidth(),
                         source.PixelHeight())
                 : wxSize();
        };
    const auto readDIPSize =
        [&treeItem](const wchar_t *widthKey,
                    const wchar_t *heightKey)
        {
            if ( !treeItem->content.HasKey(widthKey) ||
                 !treeItem->content.HasKey(heightKey) )
            {
                return wxSize();
            }

            return wxSize(
                static_cast<int>(std::lround(
                    winrt::unbox_value<double>(
                        treeItem->content.Lookup(widthKey)))),
                static_cast<int>(std::lround(
                    winrt::unbox_value<double>(
                        treeItem->content.Lookup(heightKey)))));
        };

    try
    {
        if ( imagePixelSize )
            *imagePixelSize = readPixelSize(L"Image");
        if ( stateImagePixelSize )
            *stateImagePixelSize =
                readPixelSize(L"StateImage");
        if ( imageDIPSize )
        {
            *imageDIPSize =
                readDIPSize(L"ImageWidth", L"ImageHeight");
        }
        if ( stateImageDIPSize )
        {
            *stateImageDIPSize =
                readDIPSize(
                    L"StateImageWidth", L"StateImageHeight");
        }
        if ( generation )
        {
            *generation =
                treeItem->contentProjectionGeneration;
        }
        return true;
    }
    catch ( const winrt::hresult_error& )
    {
        return false;
    }
}
#endif // WXWINUI_TEST_SUPPORT

wxVisualAttributes
wxTreeCtrl::GetClassDefaultAttributes(wxWindowVariant WXUNUSED(variant))
{
    wxVisualAttributes attrs;
    attrs.colFg = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT);
    attrs.colBg = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW);
    attrs.font = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
    return attrs;
}

void wxTreeCtrl::DoThaw()
{
    const std::shared_ptr<wxWinUITreeCallbackState> state =
        m_winui ? m_winui->callbackState : nullptr;
    const std::uint64_t generation =
        state ? state->Generation() : 0;
    wxWinUITreeCtrlImpl * const impl = m_winui.get();

    wxTreeCtrlBase::DoThaw();

    // Freeze() is the public batching contract used by controls such as
    // wxGenericDirCtrl while filling a directory containing many entries.
    // Model and peer-structure mutations still happen synchronously, but all
    // expensive content projections are coalesced by item identity and
    // published once, at the outermost Thaw().
    wxTreeCtrl * const owner =
        state ? state->GetOwner(generation) : nullptr;
    if ( !owner || !owner->m_winui ||
         owner->m_winui.get() != impl ||
         owner->m_winui->callbackState != state ||
         owner->m_winui->closed )
    {
        return;
    }

    if ( owner->m_winui->projectionAllRequested ||
         !owner->m_winui->projectionItemIds.empty() )
    {
        (void)owner->SyncPeerProjection();
    }
}

int wxTreeCtrl::DoGetItemState(const wxTreeItemId& item) const
{
    wxWinUITreeItem *treeItem = GetItem(item);
    return treeItem ? treeItem->state : wxTREE_ITEMSTATE_NONE;
}

void wxTreeCtrl::DoSetItemState(const wxTreeItemId& item, int state)
{
    wxWinUITreeItem *treeItem = GetItem(item);
    wxCHECK_RET( treeItem, wxT("invalid tree item") );
    treeItem->state = state;
    UpdatePeerItem(treeItem);
}

wxTreeItemId wxTreeCtrl::DoInsertItem(const wxTreeItemId& parent,
                                      size_t pos,
                                      const wxString& text,
                                      int image,
                                      int selImage,
                                      wxTreeItemData *data)
{
    wxWinUITreeItem *parentItem = GetItem(parent);
    wxCHECK_MSG( parentItem, wxTreeItemId(), wxT("invalid tree parent") );
    wxCHECK_MSG( !parentItem->deleting,
                 wxTreeItemId(), wxT("tree parent is being deleted") );

    wxWinUITreeCtrlImpl * const impl = m_winui.get();
    const std::shared_ptr<wxWinUITreeCallbackState> state =
        impl->callbackState;
    const std::uint64_t generation = state->Generation();
    state->BeginOperation();
    const std::uint64_t parentId = parentItem->id;

    if ( pos == static_cast<size_t>(-1) ||
         pos > parentItem->children.size() )
    {
        pos = parentItem->children.size();
    }

    // Allocate model capacity before creating or registering the new item.
    // Grow geometrically: reserve(size + 1) on every append would defeat
    // vector amortization and make a flat 10k tree quadratic. Moving
    // unique_ptrs into the resulting slot is noexcept, so the complete model
    // can be committed before any observable peer or bundle call.
    const size_t requiredCapacity = parentItem->children.size() + 1;
    if ( parentItem->children.capacity() < requiredCapacity )
    {
        const size_t oldCapacity = parentItem->children.capacity();
        size_t newCapacity = oldCapacity ? oldCapacity * 2 : 1;
        if ( newCapacity < oldCapacity )
            newCapacity = std::numeric_limits<size_t>::max();
        if ( newCapacity < requiredCapacity )
            newCapacity = requiredCapacity;
        parentItem->children.reserve(newCapacity);
#ifdef WXWINUI_TEST_SUPPORT
        ++impl->modelGrowthCount;
#endif
    }

    auto newItem = std::make_unique<wxWinUITreeItem>(parentItem);
    wxWinUITreeItem *newItemRaw = newItem.get();
    newItemRaw->text = text;
    newItemRaw->images[wxTreeItemIcon_Normal] = image;
    newItemRaw->images[wxTreeItemIcon_Selected] = selImage;
    try
    {
        newItemRaw->node = MUXC::TreeViewNode();
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException(
            "WinUI TreeView item node creation", e);
        return wxTreeItemId();
    }
    wxWinUIRegisterItem(impl, newItemRaw);
    const std::uint64_t newItemId = newItemRaw->id;
    const MUXC::TreeViewNode newItemNode = newItemRaw->node;
    newItemRaw->data = data;
    if ( data )
        data->SetId(MakeId(newItemRaw));

    const bool hadAnyExpandable = HasExpandableItem();
    const bool parentWasExpandable =
        wxWinUIItemCountsAsExpandable(this, parentItem);
    parentItem->children.insert(
        parentItem->children.begin() + pos, std::move(newItem));
    wxWinUIReindexChildren(parentItem, pos);
    const bool parentIsExpandable =
        wxWinUIItemCountsAsExpandable(this, parentItem);
    if ( parentWasExpandable != parentIsExpandable )
    {
        if ( parentIsExpandable )
            ++impl->expandableItemCount;
        else
            --impl->expandableItemCount;
    }
    const bool refreshAll =
        hadAnyExpandable != HasExpandableItem();

    const auto rollbackModel =
        [state, generation, impl, parentId, newItemId]()
        {
            wxTreeCtrl * const owner = state->GetOwner(generation);
            if ( !owner || !owner->m_winui ||
                 owner->m_winui.get() != impl ||
                 owner->m_winui->callbackState != state )
            {
                return false;
            }

            const auto parentIt = impl->itemsById.find(parentId);
            const auto itemIt = impl->itemsById.find(newItemId);
            if ( parentIt == impl->itemsById.end() ||
                 itemIt == impl->itemsById.end() )
            {
                return false;
            }

            wxWinUITreeItem * const liveParent = parentIt->second;
            wxWinUITreeItem * const liveItem = itemIt->second;
            const size_t index = liveItem->siblingIndex;
            if ( index >= liveParent->children.size() ||
                 liveParent->children[index].get() != liveItem )
            {
                return false;
            }

            const bool wasExpandable =
                wxWinUIItemCountsAsExpandable(owner, liveParent);
            if ( liveItem->data )
                liveItem->data->SetId(wxTreeItemId());
            liveItem->data = nullptr;
            --impl->itemCount;
            wxWinUIUnregisterItem(impl, liveItem);
            liveParent->children.erase(
                liveParent->children.begin() + index);
            wxWinUIReindexChildren(liveParent, index);

            const bool isExpandable =
                wxWinUIItemCountsAsExpandable(owner, liveParent);
            if ( wasExpandable != isExpandable )
            {
                if ( isExpandable )
                    ++impl->expandableItemCount;
                else
                    --impl->expandableItemCount;
            }
            return true;
        };
    const auto rollbackPeer =
        [state, generation, impl, parentId, newItemNode](
            bool injectAfterCommit)
        {
            wxTreeCtrl * const owner = state->GetOwner(generation);
            if ( !owner || !owner->m_winui ||
                 owner->m_winui.get() != impl ||
                 owner->m_winui->callbackState != state )
            {
                return false;
            }

            wxWinUITreeItem * const liveParent =
                owner->ResolveItem(parentId);
            if ( !liveParent )
                return false;

            try
            {
                auto peerChildren =
                    wxWinUIGetPeerChildren(owner, impl, liveParent);
                return wxWinUIEnsurePeerNodeAbsent(
                    peerChildren, newItemNode,
                    "WinUI TreeView item insertion rollback",
                    injectAfterCommit);
            }
            catch ( const winrt::hresult_error& e )
            {
                wxWinUILogException(
                    "WinUI TreeView item insertion rollback", e);
                return false;
            }
        };

    try
    {
        wxWinUITreePeerMutationGuard mutation(state);
        auto peerChildren =
            wxWinUIGetPeerChildren(this, impl, parentItem);
        if ( pos >= peerChildren.Size() )
            peerChildren.Append(newItemRaw->node);
        else
            peerChildren.InsertAt(
                static_cast<uint32_t>(pos), newItemRaw->node);
        wxTreeCtrl * const owner = state->GetOwner(generation);
        if ( !owner || !owner->m_winui ||
             owner->m_winui.get() != impl ||
             owner->m_winui->callbackState != state ||
             !owner->ResolveItem(newItemId) )
        {
            (void)rollbackPeer(false);
            return wxTreeItemId();
        }

#ifdef WXWINUI_TEST_SUPPORT
        const bool injectRollbackFailure = impl->ShouldFail(
            wxWinUITreeCtrlTestAccess::PeerMutation::InsertRollbackAfterCommit);
        if ( impl->ShouldFail(
                 wxWinUITreeCtrlTestAccess::PeerMutation::InsertItem) ||
             injectRollbackFailure )
        {
            if ( rollbackPeer(injectRollbackFailure) )
            {
                (void)rollbackModel();
                return wxTreeItemId();
            }
        }
#endif
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI TreeView item insertion", e);
        // Do not trust the local commit bit across an ABI exception: the
        // collection can already contain the node. rollbackPeer() is
        // identity-based and idempotent, so it is always the authority.
        if ( rollbackPeer(false) )
        {
            (void)rollbackModel();
            return wxTreeItemId();
        }
        // The peer could not be removed safely. Keep and return the exact
        // model item instead of reporting failure and transferring its data
        // ownership back to the caller while a live peer still references it.
    }

    wxTreeCtrl *owner = state->GetOwner(generation);
    if ( !owner || !owner->m_winui ||
         owner->m_winui.get() != impl ||
         owner->m_winui->callbackState != state )
    {
        return wxTreeItemId();
    }

    const PeerProjectionResult projection = refreshAll
        ? owner->RefreshProjectedItems()
        : owner->UpdatePeerItems({newItemId, parentId});
    owner = state->GetOwner(generation);
    if ( !owner || !owner->m_winui ||
         owner->m_winui.get() != impl ||
         owner->m_winui->callbackState != state )
    {
        return wxTreeItemId();
    }
    newItemRaw = owner->ResolveItem(newItemId);
    if ( !newItemRaw )
        return wxTreeItemId();

    if ( projection == PeerProjectionResult::Failed ||
         (projection == PeerProjectionResult::Done &&
          !newItemRaw->contentAttached) )
    {
        if ( rollbackPeer(false) )
        {
            (void)rollbackModel();
            return wxTreeItemId();
        }
        // As above, an unremovable peer means the insertion is committed.
        // A later model mutation will retry the bounded projection.
        owner = state->GetOwner(generation);
        if ( !owner || !owner->m_winui ||
             owner->m_winui.get() != impl ||
             owner->m_winui->callbackState != state )
        {
            return wxTreeItemId();
        }
        newItemRaw = owner->ResolveItem(newItemId);
        if ( !newItemRaw )
            return wxTreeItemId();
    }

    return owner->MakeId(newItemRaw);
}

wxTreeItemId wxTreeCtrl::DoInsertAfter(const wxTreeItemId& parent,
                                       const wxTreeItemId& idPrevious,
                                       const wxString& text,
                                       int image,
                                       int selImage,
                                       wxTreeItemData *data)
{
    wxWinUITreeItem *parentItem = GetItem(parent);
    wxWinUITreeItem *previousItem = GetItem(idPrevious);
    wxCHECK_MSG( parentItem, wxTreeItemId(), wxT("invalid tree parent") );

    size_t pos = 0;
    if ( previousItem && previousItem->parent == parentItem )
    {
        pos = previousItem->siblingIndex + 1;
    }
    else if ( previousItem )
    {
        pos = parentItem->children.size();
    }

    return DoInsertItem(parent, pos, text, image, selImage, data);
}

wxTreeItemId wxTreeCtrl::DoTreeHitTest(const wxPoint& point, int& flags) const
{
    flags = 0;
    if ( !m_winui || !m_winui->treeView ||
         !m_winui->callbackState )
    {
        return wxTreeItemId();
    }

    wxWinUITreeCtrlImpl * const impl = m_winui.get();
    const std::shared_ptr<wxWinUITreeCallbackState> state =
        impl->callbackState;
    const std::uint64_t generation = state->Generation();
    const MUXC::TreeView treeView = impl->treeView;
    const bool showButtons = HasFlag(wxTR_HAS_BUTTONS);
    const auto getCurrentOwner =
        [state, generation, impl, this]() -> const wxTreeCtrl *
        {
            const wxTreeCtrl * const owner =
                state->GetOwner(generation);
            return owner == this && owner->m_winui &&
                           owner->m_winui.get() == impl &&
                           owner->m_winui->callbackState == state
                ? owner
                : nullptr;
        };

    const wxSize client = GetClientSize();
    if ( !getCurrentOwner() )
        return wxTreeItemId();
    if ( point.x < 0 )
        flags |= wxTREE_HITTEST_TOLEFT;
    if ( point.x >= client.x )
        flags |= wxTREE_HITTEST_TORIGHT;
    if ( point.y < 0 )
        flags |= wxTREE_HITTEST_ABOVE;
    if ( point.y >= client.y )
        flags |= wxTREE_HITTEST_BELOW;
    if ( flags )
        return wxTreeItemId();

    flags = wxTREE_HITTEST_NOWHERE;

    try
    {
        const auto treeElement = treeView.as<MUX::UIElement>();
        const auto xamlRoot = treeElement.XamlRoot();
        const auto rootContent =
            xamlRoot ? xamlRoot.Content() : MUX::UIElement{ nullptr };
        if ( !getCurrentOwner() || !rootContent )
            return wxTreeItemId();
        const wxTreeCtrl * const currentOwner = getCurrentOwner();
        if ( !currentOwner )
            return wxTreeItemId();
        winrt::Windows::Foundation::Point hostPoint{};
        if ( wxWinUIVisualCoordinates::ClientPointToRoot(
                 const_cast<wxTreeCtrl *>(currentOwner),
                 winrt::Windows::Foundation::Point{
                     static_cast<float>(point.x),
                     static_cast<float>(point.y)},
                 &hostPoint) != wxWinUICoordinateResult::Mapped ||
             !getCurrentOwner() )
        {
            return wxTreeItemId();
        }

        // FindElementsInHostCoordinates() always consumes app-window
        // coordinates, even when its second argument restricts the search to
        // a subtree. wxTreeCtrl::HitTest(), however, receives TreeView-client
        // coordinates. The common mapper performs the mirrored-client and
        // fractional XamlRoot-scale projection transactionally.
        const auto hits =
            MUXM::VisualTreeHelper::FindElementsInHostCoordinates(
                hostPoint, treeElement);
        if ( !getCurrentOwner() )
            return wxTreeItemId();

        const auto classifyRowSpace =
            [this, impl, &point, &flags, &getCurrentOwner](
                std::uint64_t itemId) -> wxTreeItemId
            {
                const wxTreeCtrl *owner = getCurrentOwner();
                wxWinUITreeItem *item =
                    owner ? owner->ResolveItem(itemId) : nullptr;
                if ( !item )
                    return wxTreeItemId();

                wxRect labelRect;
                if ( GetItemPeerRect(item, labelRect, true) )
                {
                    owner = getCurrentOwner();
                    item = owner ? owner->ResolveItem(itemId) : nullptr;
                    if ( !item )
                        return wxTreeItemId();

                    const wxLayoutDirection direction =
                        owner->GetLayoutDirection();
                    owner = getCurrentOwner();
                    item = owner ? owner->ResolveItem(itemId) : nullptr;
                    if ( !item )
                        return wxTreeItemId();

                    if ( point.x < labelRect.x )
                    {
                        flags =
                            direction == wxLayout_RightToLeft
                                ? wxTREE_HITTEST_ONITEMRIGHT
                                : wxTREE_HITTEST_ONITEMINDENT;
                    }
                    else if ( point.x > labelRect.GetRight() )
                    {
                        flags =
                            direction == wxLayout_RightToLeft
                                ? wxTREE_HITTEST_ONITEMINDENT
                                : wxTREE_HITTEST_ONITEMRIGHT;
                    }
                    else
                    {
                        flags = wxTREE_HITTEST_ONITEMLABEL;
                    }
                }
                else
                {
                    // The row is realized, but its named content has not
                    // measured yet. Report the label rather than ONITEM:
                    // ONITEM is a convenience mask combining ICON|LABEL and
                    // would falsely claim that both subregions were hit.
                    flags = wxTREE_HITTEST_ONITEMLABEL;
                }

                owner = getCurrentOwner();
                item = owner ? owner->ResolveItem(itemId) : nullptr;
                return item ? owner->MakeId(item) : wxTreeItemId();
            };

        for ( const auto& hit : hits )
        {
            MUX::DependencyObject current = hit;
            MUXC::TreeViewItem container{ nullptr };
            bool onLabel = false;
            bool onIcon = false;
            bool onStateIcon = false;
            bool onButton = false;

            while ( current )
            {
                if ( const auto element =
                         current.try_as<MUX::FrameworkElement>() )
                {
                    const auto name = element.Name();
                    onLabel = onLabel || name == L"wxTreeLabel";
                    onIcon = onIcon || name == L"wxTreeImage";
                    onStateIcon =
                        onStateIcon || name == L"wxTreeStateImage";
                }
                onButton =
                    onButton || wxWinUIIsExpanderElement(current);

                if ( const auto item =
                         current.try_as<MUXC::TreeViewItem>() )
                {
                    container = item;
                    break;
                }
                current = MUXM::VisualTreeHelper::GetParent(current);
                if ( !getCurrentOwner() )
                    return wxTreeItemId();
            }

            if ( !container )
                continue;

            const MUXC::TreeViewNode node =
                treeView.NodeFromContainer(container);
            if ( !getCurrentOwner() )
                return wxTreeItemId();
            wxWinUITreeItem *item =
                wxWinUIFindItemByNode(impl, node);
            if ( !item )
                continue;
            const std::uint64_t itemId = item->id;

            if ( onButton && showButtons )
                flags = wxTREE_HITTEST_ONITEMBUTTON;
            else if ( onStateIcon )
                flags = wxTREE_HITTEST_ONITEMSTATEICON;
            else if ( onIcon )
                flags = wxTREE_HITTEST_ONITEMICON;
            else if ( onLabel )
                flags = wxTREE_HITTEST_ONITEMLABEL;
            else
                return classifyRowSpace(itemId);

            const wxTreeCtrl * const owner = getCurrentOwner();
            item = owner ? owner->ResolveItem(itemId) : nullptr;
            return item ? owner->MakeId(item) : wxTreeItemId();
        }

        // A TreeViewItem can measure across the full row while its stock XAML
        // template leaves the blank leading/trailing area with no hit-testable
        // visual. wxTreeCtrl nevertheless defines these as ONITEMINDENT and
        // ONITEMRIGHT. Search only the realized ItemsPanel children, keeping
        // this fallback bounded by the viewport instead of the model size.
        if ( !getCurrentOwner() )
            return wxTreeItemId();
        const auto treeList = impl->treeList;
        const auto panel = treeList ? treeList.ItemsPanelRoot() : nullptr;
        if ( !getCurrentOwner() || !panel )
            return wxTreeItemId();

        const auto geometryIsCurrent =
            [&getCurrentOwner]()
            {
                return getCurrentOwner() != nullptr;
            };
        const auto children = panel.Children();
        for ( uint32_t i = 0; i < children.Size(); ++i )
        {
            const auto container =
                children.GetAt(i).try_as<MUXC::TreeViewItem>();
            if ( !getCurrentOwner() )
                return wxTreeItemId();
            if ( !container )
                continue;

            wxRect rowRect;
            const wxTreeCtrl * const ownerBeforeProjection =
                getCurrentOwner();
            if ( !ownerBeforeProjection )
                return wxTreeItemId();
            if ( !wxWinUIGetElementRect(
                     const_cast<wxTreeCtrl *>(ownerBeforeProjection),
                     container.as<MUX::FrameworkElement>(),
                     rowRect,
                     geometryIsCurrent) )
            {
                if ( !getCurrentOwner() )
                    return wxTreeItemId();
                continue;
            }
            if ( !getCurrentOwner() )
                return wxTreeItemId();
            if ( !rowRect.Contains(point) )
                continue;

            const MUXC::TreeViewNode node =
                treeView.NodeFromContainer(container);
            if ( !getCurrentOwner() )
                return wxTreeItemId();
            wxWinUITreeItem * const item =
                wxWinUIFindItemByNode(impl, node);
            if ( !item )
                continue;
            const std::uint64_t itemId = item->id;

            const auto containsPoint =
                [&getCurrentOwner, &geometryIsCurrent, &point](
                    const MUX::FrameworkElement& element)
                {
                    const wxTreeCtrl * const owner = getCurrentOwner();
                    if ( !owner )
                        return false;
                    wxRect rect;
                    return wxWinUIGetElementRect(
                               const_cast<wxTreeCtrl *>(owner),
                               element,
                               rect,
                               geometryIsCurrent) &&
                           rect.Contains(point);
                };

            if ( showButtons )
            {
                if ( const auto expander =
                         wxWinUIFindExpanderElement(container) )
                {
                    const bool contains = containsPoint(expander);
                    if ( !getCurrentOwner() )
                        return wxTreeItemId();
                    if ( contains )
                        flags = wxTREE_HITTEST_ONITEMBUTTON;
                }
            }
            if ( flags == wxTREE_HITTEST_NOWHERE )
            {
                if ( const auto stateImage =
                         wxWinUIFindNamedElement(
                             container, L"wxTreeStateImage") )
                {
                    const bool visible =
                        stateImage.Visibility() ==
                            MUX::Visibility::Visible;
                    if ( !getCurrentOwner() )
                        return wxTreeItemId();
                    const bool contains =
                        visible && containsPoint(stateImage);
                    if ( !getCurrentOwner() )
                        return wxTreeItemId();
                    if ( contains )
                    {
                        flags = wxTREE_HITTEST_ONITEMSTATEICON;
                    }
                }
            }
            if ( flags == wxTREE_HITTEST_NOWHERE )
            {
                if ( const auto image =
                         wxWinUIFindNamedElement(
                             container, L"wxTreeImage") )
                {
                    const bool visible =
                        image.Visibility() == MUX::Visibility::Visible;
                    if ( !getCurrentOwner() )
                        return wxTreeItemId();
                    const bool contains = visible && containsPoint(image);
                    if ( !getCurrentOwner() )
                        return wxTreeItemId();
                    if ( contains )
                    {
                        flags = wxTREE_HITTEST_ONITEMICON;
                    }
                }
            }
            if ( flags == wxTREE_HITTEST_NOWHERE )
            {
                if ( const auto label =
                         wxWinUIFindNamedElement(
                             container, L"wxTreeLabel") )
                {
                    const bool contains = containsPoint(label);
                    if ( !getCurrentOwner() )
                        return wxTreeItemId();
                    if ( contains )
                        flags = wxTREE_HITTEST_ONITEMLABEL;
                }
            }

            if ( !getCurrentOwner() )
                return wxTreeItemId();
            if ( flags != wxTREE_HITTEST_NOWHERE )
            {
                const wxTreeCtrl * const owner = getCurrentOwner();
                wxWinUITreeItem * const currentItem =
                    owner ? owner->ResolveItem(itemId) : nullptr;
                return currentItem
                    ? owner->MakeId(currentItem)
                    : wxTreeItemId();
            }
            return classifyRowSpace(itemId);
        }
    }
    catch ( const winrt::hresult_error& )
    {
    }

    return wxTreeItemId();
}

wxSize wxTreeCtrl::DoGetBestSize() const
{
    return FromDIP(wxSize(180, 240));
}

void wxTreeCtrl::OnImagesChanged()
{
    if ( RefreshProjectedItems() == PeerProjectionResult::Failed )
        return;
    InvalidateBestSize();
}

void wxTreeCtrl::OnDPIChanged(wxDPIChangedEvent& event)
{
    // wxTreeCtrlBase also handles this event and calls OnImagesChanged() when
    // either the normal or state image collection is present. That virtual
    // path reprojects every item bitmap at the new DPI without replacing its
    // TreeViewNode, so selection and expansion state remain untouched.
    InvalidateBestSize();
    if ( m_winui && m_winui->callbackState && !m_winui->closed )
    {
        wxWinUITreeCtrlImpl * const impl = m_winui.get();
#ifdef WXWINUI_TEST_SUPPORT
        impl->projectionScaleOverrideForTesting = 0.0;
#endif
        const std::shared_ptr<wxWinUITreeCallbackState> state =
            impl->callbackState;
        const std::uint64_t generation = state->Generation();
        const wxLayoutDirection effectiveDirection =
            GetLayoutDirection();
        if ( state->GetOwner(generation) != this || !m_winui ||
             m_winui.get() != impl )
        {
            return;
        }

        impl->indentDips = static_cast<double>(m_indent);
        impl->rightToLeft =
            effectiveDirection == wxLayout_RightToLeft;
        wxWinUISyncRealizedTreeItems(impl);
        if ( state->GetOwner(generation) != this || !m_winui ||
             m_winui.get() != impl )
        {
            return;
        }

        impl->host.ForceRender();
        if ( state->GetOwner(generation) != this || !m_winui ||
             m_winui.get() != impl )
        {
            return;
        }
    }
    event.Skip();
}

wxWinUITreeItem *wxTreeCtrl::GetItem(const wxTreeItemId& item) const
{
    if ( !m_winui || m_winui->closed || !item.IsOk() )
        return nullptr;

    wxWinUITreeItemHandle * const handle =
        static_cast<wxWinUITreeItemHandle *>(item.GetID());
    if ( !handle )
        return nullptr;

    const auto found = m_winui->liveItems.find(handle);
    return found == m_winui->liveItems.end()
        ? nullptr
        : found->second;
}

wxTreeItemId wxTreeCtrl::MakeId(wxWinUITreeItem *item) const
{
    return item
        ? wxTreeItemId(item->handle)
        : wxTreeItemId();
}

wxWinUITreeItem *wxTreeCtrl::ResolveItem(std::uint64_t itemId) const
{
    if ( !m_winui || m_winui->closed || !itemId )
        return nullptr;

    const auto found = m_winui->itemsById.find(itemId);
    return found == m_winui->itemsById.end() ? nullptr : found->second;
}

void wxTreeCtrl::SendTreeEvent(wxEventType type,
                               wxWinUITreeItem *item,
                               wxWinUITreeItem *oldItem)
{
    wxTreeEvent event(type, this, MakeId(item));
    event.SetOldItem(MakeId(oldItem));
    HandleWindowEvent(event);
}

bool wxTreeCtrl::SetExpanded(wxWinUITreeItem *item,
                             bool expanded,
                             bool sendEvent,
                             PeerExpansionMode peerMode,
                             std::uint64_t operation)
{
    if ( !m_winui || !item || item->deleting ||
         item->expanded == expanded )
    {
        return true;
    }
    if ( expanded && !wxWinUIItemIsExpandable(item) )
        return false;

    const std::shared_ptr<wxWinUITreeCallbackState> state =
        m_winui->callbackState;
    const std::uint64_t generation = state->Generation();
    if ( !operation )
        operation = state->BeginOperation();
    const std::uint64_t itemId = item->id;
    const wxEventType beforeType = expanded
        ? wxEVT_TREE_ITEM_EXPANDING
        : wxEVT_TREE_ITEM_COLLAPSING;
    const wxEventType afterType = expanded
        ? wxEVT_TREE_ITEM_EXPANDED
        : wxEVT_TREE_ITEM_COLLAPSED;

    if ( sendEvent )
    {
        wxTreeEvent event(beforeType, this, MakeId(item));
        const bool processed =
            HandleWindowEvent(event);
        if ( state->GetOwner(generation) != this )
        {
            return false;
        }

        item = ResolveItem(itemId);
        if ( !item || item->deleting )
            return false;

        if ( !state->IsCurrentOperation(operation) ||
             (expanded && !wxWinUIItemIsExpandable(item)) ||
             (processed && !event.IsAllowed()) )
        {
            // Peer-originated notifications arrive after WinUI toggled the
            // node. Restore the newest wx model state under the callback
            // guard so veto is observable by the actual peer too.
            if ( peerMode == PeerExpansionMode::PeerAlreadyUpdated )
            {
                try
                {
                    wxWinUITreePeerMutationGuard mutation(
                        m_winui->callbackState);
                    item->node.IsExpanded(item->expanded);
#ifdef WXWINUI_TEST_SUPPORT
                    if ( m_winui->ShouldFail(
                             wxWinUITreeCtrlTestAccess::PeerMutation::
                                 SetExpandedAfterCommit) )
                    {
                        throw winrt::hresult_error(
                            E_FAIL,
                            L"Injected failure after TreeViewNode "
                            L"expansion restoration");
                    }
#endif
                }
                catch ( const winrt::hresult_error& e )
                {
                    wxWinUILogException(
                        "WinUI TreeView expansion veto restoration", e);
                    wxTreeCtrl * const owner =
                        state->GetOwner(generation);
                    wxWinUITreeItem * const liveItem =
                        owner == this
                            ? owner->ResolveItem(itemId)
                            : nullptr;
                    if ( liveItem && !liveItem->deleting )
                    {
                        try
                        {
                            wxWinUITreePeerMutationGuard mutation(
                                owner->m_winui->callbackState);
                            liveItem->node.IsExpanded(
                                liveItem->expanded);
                        }
                        catch ( const winrt::hresult_error&
                                    rollbackError )
                        {
                            wxWinUILogException(
                                "WinUI TreeView expansion veto "
                                "reconciliation",
                                rollbackError);
                            (void)owner->
                                ReconcilePeerStructureFromModel();
                        }
                    }
                }
            }
            return false;
        }
    }

    item->expanded = expanded;
    if ( peerMode == PeerExpansionMode::Immediate )
    {
        try
        {
            wxWinUITreePeerMutationGuard mutation(
                m_winui->callbackState);
            item->node.IsExpanded(expanded);
#ifdef WXWINUI_TEST_SUPPORT
            if ( m_winui->ShouldFail(wxWinUITreeCtrlTestAccess::PeerMutation::
                                         SetExpandedAfterCommit) )
            {
                throw winrt::hresult_error(
                    E_FAIL,
                    L"Injected failure after TreeViewNode expansion");
            }
#endif
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException(
                "WinUI TreeView expansion transaction", e);
            wxTreeCtrl * const owner =
                state->GetOwner(generation);
            wxWinUITreeItem * const liveItem =
                owner == this ? owner->ResolveItem(itemId) : nullptr;
            if ( !liveItem || liveItem->deleting )
                return false;

            liveItem->expanded = !expanded;
            try
            {
                // The setter may have committed before the ABI raised.
                // Restore the authoritative model value idempotently.
                wxWinUITreePeerMutationGuard mutation(
                    owner->m_winui->callbackState);
                liveItem->node.IsExpanded(liveItem->expanded);
            }
            catch ( const winrt::hresult_error& rollbackError )
            {
                wxWinUILogException(
                    "WinUI TreeView expansion rollback",
                    rollbackError);
                (void)owner->ReconcilePeerStructureFromModel();
            }
            return false;
        }
    }
    if ( state->GetOwner(generation) != this ||
         !state->IsCurrentOperation(operation) )
    {
        return false;
    }
    item = ResolveItem(itemId);
    if ( !item || item->deleting )
        return false;

    if ( peerMode != PeerExpansionMode::Deferred )
    {
        const PeerProjectionResult projection =
            UpdatePeerItem(item);
        if ( state->GetOwner(generation) != this )
        {
            return false;
        }
        if ( projection != PeerProjectionResult::Done )
            SchedulePeerSelectionCorrection();
    }
    else
    {
        // A deferred expansion still invalidates any projection snapshot
        // exposed by the vetoable events. The caller will update only the
        // properties whose effective value depends on expansion after it has
        // projected the TreeViewNode path in a stack-safe order.
        m_winui->BumpModelRevision();
    }

    // Projection is a recoverable peer concern. The logical expansion was
    // already committed above, so preserve the EXPANDING/EXPANDED pairing as
    // long as no nested writer superseded this exact item state.
    if ( !state->IsCurrentOperation(operation) )
        return false;
    item = ResolveItem(itemId);
    if ( !item || item->deleting || item->expanded != expanded )
        return false;

    if ( sendEvent )
    {
        item = ResolveItem(itemId);
        if ( !item )
            return false;
        wxTreeEvent event(afterType, this, MakeId(item));
        HandleWindowEvent(event);
    }
    return true;
}

wxTreeCtrl::AncestorExpansionResult
wxTreeCtrl::ExpandAncestorPath(
    std::uint64_t itemId,
    std::uint64_t operation,
    std::vector<std::uint64_t> *projectionIds)
{
    if ( !m_winui || !m_winui->callbackState || !operation )
        return AncestorExpansionResult::Stale;

    wxWinUITreeItem *item = ResolveItem(itemId);
    if ( !item || item->deleting )
        return AncestorExpansionResult::Stale;

    wxWinUITreeCtrlImpl * const impl = m_winui.get();
    const std::shared_ptr<wxWinUITreeCallbackState> state =
        impl->callbackState;
    const std::uint64_t generation = state->Generation();
    std::vector<std::uint64_t> ancestorIds;
    for ( wxWinUITreeItem *parent = item->parent;
          parent;
          parent = parent->parent )
    {
        if ( parent->parent || !HasFlag(wxTR_HIDE_ROOT) )
            ancestorIds.push_back(parent->id);
    }

    enum class PeerSyncResult
    {
        Done,
        Stale,
        Failed
    };

    const auto syncAncestorPeers =
        [state, generation, impl, &ancestorIds](
            std::uint64_t expectedOperation,
            bool allowMissing) -> PeerSyncResult
        {
            wxTreeCtrl *owner = state->GetOwner(generation);
            if ( !owner || !owner->m_winui ||
                 owner->m_winui.get() != impl ||
                 owner->m_winui->callbackState != state ||
                 owner->m_winui->closed ||
                 (expectedOperation &&
                  !state->IsCurrentOperation(expectedOperation)) )
            {
                return PeerSyncResult::Stale;
            }

            try
            {
                wxWinUITreePeerMutationGuard mutation(state);
                // ancestorIds is immediate-parent to root. Apply the peer
                // state in the reverse order so WinUI never flattens an
                // already-expanded deep subtree in one recursive step.
                for ( auto it = ancestorIds.rbegin();
                      it != ancestorIds.rend();
                      ++it )
                {
                    owner = state->GetOwner(generation);
                    if ( !owner || !owner->m_winui ||
                         owner->m_winui.get() != impl ||
                         owner->m_winui->callbackState != state ||
                         owner->m_winui->closed ||
                         (expectedOperation &&
                          !state->IsCurrentOperation(
                              expectedOperation)) )
                    {
                        return PeerSyncResult::Stale;
                    }

                    wxWinUITreeItem * const ancestor =
                        owner->ResolveItem(*it);
                    if ( !ancestor || ancestor->deleting )
                    {
                        if ( allowMissing )
                            continue;
                        return PeerSyncResult::Stale;
                    }

                    ancestor->node.IsExpanded(ancestor->expanded);
                    owner = state->GetOwner(generation);
                    if ( !owner || !owner->m_winui ||
                         owner->m_winui.get() != impl ||
                         owner->m_winui->callbackState != state ||
                         owner->m_winui->closed ||
                         (expectedOperation &&
                          !state->IsCurrentOperation(
                              expectedOperation)) )
                    {
                        return PeerSyncResult::Stale;
                    }
#ifdef WXWINUI_TEST_SUPPORT
                    if ( impl->ShouldFail(
                             wxWinUITreeCtrlTestAccess::PeerMutation::
                                 SetExpandedAfterCommit) )
                    {
                        throw winrt::hresult_error(
                            E_FAIL,
                            L"Injected failure after deferred "
                            L"TreeViewNode expansion");
                    }
#endif
                }
            }
            catch ( const winrt::hresult_error& e )
            {
                wxWinUILogException(
                    "WinUI TreeView ancestor expansion projection", e);
                owner = state->GetOwner(generation);
                if ( !owner || !owner->m_winui ||
                     owner->m_winui.get() != impl ||
                     owner->m_winui->callbackState != state ||
                     owner->m_winui->closed )
                {
                    return PeerSyncResult::Stale;
                }

                const bool reconciled =
                    owner->ReconcilePeerStructureFromModel();
                owner = state->GetOwner(generation);
                if ( !owner || !owner->m_winui ||
                     owner->m_winui.get() != impl ||
                     owner->m_winui->callbackState != state ||
                     owner->m_winui->closed ||
                     (expectedOperation &&
                      !state->IsCurrentOperation(expectedOperation)) )
                {
                    return PeerSyncResult::Stale;
                }
                if ( reconciled )
                    return PeerSyncResult::Done;

                // EXPANDED has already been emitted. Keep the wx model
                // authoritative and retry the complete forest after this
                // transaction; selection application must remain deferred
                // until that repair succeeds.
                impl->peerStructureRepairPending = true;
                if ( wxTheApp )
                {
                    const std::weak_ptr<wxWinUITreeCallbackState> weakState =
                        state;
                    wxTheApp->CallAfter(
                        [weakState, generation, impl]()
                        {
                            const auto liveState = weakState.lock();
                            wxTreeCtrl * const liveOwner =
                                liveState
                                    ? liveState->GetOwner(generation)
                                    : nullptr;
                            if ( liveOwner && liveOwner->m_winui &&
                                 liveOwner->m_winui.get() == impl &&
                                 liveOwner->m_winui->callbackState ==
                                     liveState )
                            {
                                (void)liveOwner->
                                    ReconcilePeerStructureFromModel();
                            }
                        });
                }
                return PeerSyncResult::Failed;
            }

            owner = state->GetOwner(generation);
            if ( !owner || !owner->m_winui ||
                 owner->m_winui.get() != impl ||
                 owner->m_winui->callbackState != state ||
                 owner->m_winui->closed ||
                 (expectedOperation &&
                  !state->IsCurrentOperation(expectedOperation)) )
            {
                return PeerSyncResult::Stale;
            }
            return impl->peerStructureRepairPending
                ? PeerSyncResult::Failed
                : PeerSyncResult::Done;
        };

    bool peerSyncPending = false;
    wxScopeGuard peerSyncGuard = wxMakeGuard(
        [&]()
        {
            // A nested latest writer can supersede us after earlier logical
            // commits. Repair surviving nodes to the newest model before
            // abandoning this stack frame.
            if ( peerSyncPending )
                (void)syncAncestorPeers(0, true);
        });

    for ( const std::uint64_t ancestorId : ancestorIds )
    {
        wxWinUITreeItem *ancestor = ResolveItem(ancestorId);
        if ( !ancestor || ancestor->deleting )
            return AncestorExpansionResult::Stale;

        if ( !ancestor->expanded )
        {
            peerSyncPending = true;
            const bool didExpand =
                SetExpanded(
                    ancestor,
                    true,
                    true,
                    PeerExpansionMode::Deferred,
                    operation);
            if ( state->GetOwner(generation) != this ||
                 !m_winui || m_winui.get() != impl ||
                 !state->IsCurrentOperation(operation) )
            {
                return AncestorExpansionResult::Stale;
            }

            // EXPANDING is application code and may have changed any of the
            // image slots. Re-resolve and compare the current effective
            // collapsed/expanded images only after the logical commit.
            ancestor = ResolveItem(ancestorId);
            if ( !ancestor || ancestor->deleting )
                return AncestorExpansionResult::Stale;
            if ( didExpand && ancestor->expanded &&
                 wxWinUIEffectiveTreeItemImage(
                     ancestor, ancestor->selected, false) !=
                 wxWinUIEffectiveTreeItemImage(
                     ancestor, ancestor->selected, true) &&
                 projectionIds )
            {
                projectionIds->push_back(ancestorId);
            }
        }

        if ( !ResolveItem(itemId) )
            return AncestorExpansionResult::Stale;
    }

    PeerSyncResult syncResult = PeerSyncResult::Done;
    if ( peerSyncPending )
        syncResult = syncAncestorPeers(operation, false);
    peerSyncGuard.Dismiss();

    if ( syncResult == PeerSyncResult::Stale )
        return AncestorExpansionResult::Stale;
    if ( syncResult == PeerSyncResult::Failed ||
         impl->peerStructureRepairPending )
    {
        return AncestorExpansionResult::PeerDeferred;
    }
    return AncestorExpansionResult::Done;
}

wxTreeCtrl::SelectionPreflightResult
wxTreeCtrl::PreflightSelectionChange(wxWinUITreeItem *item,
                                     std::uint64_t *oldItemId)
{
    if ( !m_winui || (item && item->deleting) )
        return SelectionPreflightResult::Superseded;

    wxWinUITreeItem * const oldItem =
        ResolveItem(m_winui->selectionId);
    const std::uint64_t newId = item ? item->id : 0;
    const std::uint64_t oldId = oldItem ? oldItem->id : 0;
    if ( oldItemId )
        *oldItemId = oldId;

    const std::shared_ptr<wxWinUITreeCallbackState> state =
        m_winui->callbackState;
    const std::uint64_t generation = state->Generation();
    const std::uint64_t operation = state->BeginOperation();

    wxTreeEvent event(
        wxEVT_TREE_SEL_CHANGING, this, MakeId(item));
    event.SetOldItem(MakeId(oldItem));
    const bool processed =
        HandleWindowEvent(event);
    if ( state->GetOwner(generation) != this ||
         !state->IsCurrentOperation(operation) )
    {
        return SelectionPreflightResult::Superseded;
    }

    if ( (newId && !ResolveItem(newId)) ||
         (oldId && !ResolveItem(oldId)) )
    {
        return SelectionPreflightResult::Superseded;
    }

    return processed && !event.IsAllowed()
        ? SelectionPreflightResult::Vetoed
        : SelectionPreflightResult::Allowed;
}

void wxTreeCtrl::CommitSelectionForDeletion(
    wxWinUITreeItem *replacement,
    wxWinUITreeItem *doomedSubtree)
{
    if ( !m_winui )
        return;

    const bool multiple = HasFlag(wxTR_MULTIPLE);
    std::vector<std::uint64_t> selectedIds(
        m_winui->selectedIds.begin(),
        m_winui->selectedIds.end());
    for ( const std::uint64_t selectedId : selectedIds )
    {
        wxWinUITreeItem * const selected = ResolveItem(selectedId);
        if ( !selected )
        {
            m_winui->selectedIds.erase(selectedId);
            continue;
        }

        if ( !multiple ||
             (doomedSubtree &&
              wxWinUIIsDescendantOf(selected, doomedSubtree)) )
        {
            selected->selected = false;
            m_winui->selectedIds.erase(selectedId);
        }
    }

    if ( replacement && !replacement->deleting )
    {
        replacement->selected = true;
        m_winui->selectedIds.insert(replacement->id);
    }

    m_winui->selectionId = replacement ? replacement->id : 0;
    m_winui->focusedId = replacement ? replacement->id : 0;
    m_winui->rejectedSelectionId = 0;
}

bool wxTreeCtrl::ChangeSelection(wxWinUITreeItem *item,
                                 bool sendEvent,
                                 bool updatePeer,
                                 std::uint64_t *operationOut)
{
    if ( !m_winui || (item && item->deleting) )
        return false;

    wxWinUITreeItem *oldItem =
        ResolveItem(m_winui->selectionId);
    if ( oldItem == item &&
         (!item || item->selected) )
    {
        return true;
    }

    const std::shared_ptr<wxWinUITreeCallbackState> state =
        m_winui->callbackState;
    const std::uint64_t generation = state->Generation();
    const std::uint64_t operation = state->BeginOperation();
    if ( operationOut )
        *operationOut = operation;
    const std::uint64_t itemId = item ? item->id : 0;
    const std::uint64_t oldItemId = oldItem ? oldItem->id : 0;

    if ( sendEvent )
    {
        wxTreeEvent event(
            wxEVT_TREE_SEL_CHANGING, this, MakeId(item));
        event.SetOldItem(MakeId(oldItem));
        const bool processed =
            HandleWindowEvent(event);
        if ( state->GetOwner(generation) != this ||
             !state->IsCurrentOperation(operation) )
        {
            return false;
        }

        item = ResolveItem(itemId);
        oldItem = ResolveItem(oldItemId);
        if ( (itemId && !item) || (oldItemId && !oldItem) )
            return false;
        if ( processed && !event.IsAllowed() )
            return false;
    }

    std::vector<std::uint64_t> projectionIds;
    std::vector<std::uint64_t> deselectionIds;
    if ( !HasFlag(wxTR_MULTIPLE) )
    {
        projectionIds.reserve(m_winui->selectedIds.size() + 1);
        deselectionIds.reserve(m_winui->selectedIds.size());
        for ( const std::uint64_t selectedId : m_winui->selectedIds )
        {
            if ( ResolveItem(selectedId) )
            {
                deselectionIds.push_back(selectedId);
                projectionIds.push_back(selectedId);
            }
        }
    }
    else if ( !item && oldItem )
    {
        deselectionIds.push_back(oldItem->id);
        projectionIds.push_back(oldItem->id);
    }

    bool ancestorPeerReady = true;
    if ( item )
    {
        const AncestorExpansionResult expansion =
            ExpandAncestorPath(itemId, operation, &projectionIds);
        if ( expansion == AncestorExpansionResult::Stale )
            return false;
        ancestorPeerReady =
            expansion == AncestorExpansionResult::Done;

        item = ResolveItem(itemId);
        oldItem = ResolveItem(oldItemId);
        if ( !item || (oldItemId && !oldItem) )
            return false;
    }

    if ( state->GetOwner(generation) != this ||
         !state->IsCurrentOperation(operation) )
    {
        return false;
    }
    item = ResolveItem(itemId);
    oldItem = ResolveItem(oldItemId);
    if ( (itemId && !item) || (oldItemId && !oldItem) )
        return false;

    for ( const std::uint64_t selectedId : deselectionIds )
    {
        if ( wxWinUITreeItem * const selected =
                 ResolveItem(selectedId) )
        {
            selected->selected = false;
            m_winui->selectedIds.erase(selectedId);
        }
    }
    if ( !HasFlag(wxTR_MULTIPLE) )
        m_winui->selectedIds.clear();

    if ( item )
    {
        item->selected = true;
        m_winui->selectedIds.insert(item->id);
        projectionIds.push_back(item->id);
    }

    m_winui->selectionId = item ? item->id : 0;
    m_winui->focusedId = item ? item->id : 0;
    m_winui->rejectedSelectionId = 0;

    if ( !projectionIds.empty() )
    {
        const PeerProjectionResult projection =
            UpdatePeerItems(projectionIds);
        if ( state->GetOwner(generation) != this )
            return false;
        if ( projection != PeerProjectionResult::Done )
        {
            SchedulePeerSelectionCorrection();
        }
    }
    if ( state->GetOwner(generation) != this ||
         !state->IsCurrentOperation(operation) ||
         m_winui->selectionId != itemId )
    {
        if ( state->GetOwner(generation) == this )
            SchedulePeerSelectionCorrection();
        return false;
    }

    if ( updatePeer && !ancestorPeerReady )
    {
        if ( state->GetOwner(generation) == this )
            SchedulePeerSelectionCorrection();
    }
    else if ( updatePeer && !ApplySelectionToPeer() )
    {
        if ( state->GetOwner(generation) == this )
            SchedulePeerSelectionCorrection();
    }
    if ( state->GetOwner(generation) != this ||
         !state->IsCurrentOperation(operation) ||
         m_winui->selectionId != itemId )
    {
        return false;
    }

    if ( sendEvent )
    {
        item = ResolveItem(itemId);
        oldItem = ResolveItem(oldItemId);
        if ( (itemId && !item) || (oldItemId && !oldItem) )
            return false;
        wxTreeEvent event(
            wxEVT_TREE_SEL_CHANGED, this, MakeId(item));
        event.SetOldItem(MakeId(oldItem));
        HandleWindowEvent(event);
    }

    return true;
}

bool wxTreeCtrl::ApplySelectionToPeer()
{
    if ( !m_winui || !m_winui->treeView )
        return false;

    wxWinUITreeCtrlImpl * const impl = m_winui.get();
    const std::shared_ptr<wxWinUITreeCallbackState> state =
        impl->callbackState;
    const std::uint64_t generation = state->Generation();
    const auto isCurrent =
        [this, impl, state, generation]()
        {
            wxTreeCtrl * const owner = state->GetOwner(generation);
            return owner == this && owner->m_winui &&
                   owner->m_winui.get() == impl &&
                   owner->m_winui->callbackState == state &&
                   !impl->closed;
        };

    // The WinUI TreeView silently ignores a SelectedNode write performed
    // synchronously from inside its own SelectionChanged callback.  When we
    // need to correct the selection in that context (e.g. an application
    // vetoed the change), defer the write to the dispatcher queue instead.
    if ( impl->inPeerSelectionChange )
    {
        SchedulePeerSelectionCorrection();
        return isCurrent();
    }

    const std::unordered_set<std::uint64_t> selectedIdsSnapshot =
        impl->selectedIds;
    const std::uint64_t primaryIdSnapshot = impl->selectionId;
    std::vector<MUXC::TreeViewNode> selectedNodesSnapshot;
    selectedNodesSnapshot.reserve(impl->selectedIds.size());
    for ( const std::uint64_t selectedId : impl->selectedIds )
    {
        if ( wxWinUITreeItem * const item = ResolveItem(selectedId) )
            selectedNodesSnapshot.push_back(item->node);
    }

    MUXC::TreeViewNode primaryNode{ nullptr };
    bool primaryExpanded = false;
    if ( wxWinUITreeItem * const selection =
             ResolveItem(impl->selectionId) )
    {
        primaryNode = selection->node;
        primaryExpanded = selection->expanded;
    }

    const MUXC::TreeView treeView = impl->treeView;

    try
    {
        wxWinUITreePeerMutationGuard mutation(state);
        if ( HasFlag(wxTR_MULTIPLE) )
        {
            auto selectedNodes = treeView.SelectedNodes();
            selectedNodes.Clear();
            if ( !isCurrent() )
                return false;

            for ( const auto& node : selectedNodesSnapshot )
            {
                selectedNodes.Append(node);
                if ( !isCurrent() )
                    return false;
            }
        }
        else
        {
            treeView.SelectedNode(primaryNode);
            if ( !isCurrent() )
                return false;
        }

        if ( primaryNode )
        {
            try
            {
                primaryNode.IsExpanded(primaryExpanded);
                if ( !isCurrent() )
                    return false;
            }
            catch ( const winrt::hresult_error& )
            {
            }
        }
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI TreeView selection", e);
        if ( isCurrent() )
            SchedulePeerSelectionCorrection(false);
        return false;
    }

    if ( !isCurrent() )
        return false;

    // FlushSync() can run arbitrary application code and destroy the tree.
    // Keep the callback state alive and never dereference impl after this call.
    impl->host.ForceRender();
    wxTreeCtrl * const owner = state->GetOwner(generation);
    if ( owner != this || !owner->m_winui ||
         owner->m_winui.get() != impl ||
         owner->m_winui->callbackState != state )
    {
        return false;
    }
    if ( impl->selectionId != primaryIdSnapshot ||
         impl->selectedIds != selectedIdsSnapshot )
    {
        owner->SchedulePeerSelectionCorrection();
        return false;
    }

    impl->peerCorrectionPending = false;
    impl->peerCorrectionDriverActive = false;
    impl->peerCorrectionQuarantined = false;
    impl->peerCorrectionAttempts = 0;
    return true;
}

bool wxTreeCtrl::ReconcilePeerStructureFromModel()
{
    if ( !m_winui || !m_winui->treeView || !m_winui->root )
        return false;

    wxWinUITreeCtrlImpl * const impl = m_winui.get();
    impl->peerStructureRepairPending = true;
    const std::shared_ptr<wxWinUITreeCallbackState> state =
        impl->callbackState;
    const std::uint64_t generation = state->Generation();

    // A single retry is enough to recover a projection invalidated by one
    // re-entrant latest writer, while remaining strictly bounded under an
    // adversarial callback storm.
    for ( unsigned attempt = 0; attempt < 2; ++attempt )
    {
        wxTreeCtrl * const owner = state->GetOwner(generation);
        if ( owner != this || !owner->m_winui ||
             owner->m_winui.get() != impl || impl->closed ||
             !impl->root )
        {
            return false;
        }

        const std::uint64_t revision = impl->modelRevision;
        const bool hiddenRoot = HasFlag(wxTR_HIDE_ROOT);
        const std::vector<MUXC::TreeViewNode> modelPeerNodes =
            wxWinUICollectModelPeerNodes(impl->root.get());
        const wxWinUITreePeerForestSnapshot desired =
            wxWinUIBuildModelPeerForest(impl, hiddenRoot);

        const auto isCurrent =
            [&]()
            {
                wxTreeCtrl * const liveOwner =
                    state->GetOwner(generation);
                return liveOwner == this && liveOwner->m_winui &&
                       liveOwner->m_winui.get() == impl &&
                       !impl->closed &&
                       impl->modelRevision == revision;
            };

        try
        {
            wxWinUITreePeerMutationGuard mutation(state);
            WFC::IVector<MUXC::TreeViewNode> peerRoots =
                impl->treeView.RootNodes();
            const wxWinUITreePeerForestSnapshot current =
                wxWinUICapturePeerForest(
                    peerRoots, modelPeerNodes);
            wxWinUIApplyPeerForest(
                peerRoots, current, desired);
#ifdef WXWINUI_TEST_SUPPORT
            if ( impl->ShouldFail(wxWinUITreeCtrlTestAccess::PeerMutation::
                                      SetExpandedAfterCommit) )
            {
                throw winrt::hresult_error(
                    E_FAIL,
                    L"Injected TreeView structural reconciliation "
                    L"failure");
            }
#endif
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException(
                "WinUI TreeView structural reconciliation", e);
            // The failed ABI call may have applied only part of this pass.
            // The next attempt first snapshots and flattens that actual graph,
            // so it repairs both pre-commit and post-commit failures without
            // ever attaching a deep root.
            if ( state->GetOwner(generation) != this ||
                 !m_winui || m_winui.get() != impl || impl->closed )
            {
                return false;
            }
            continue;
        }

        if ( isCurrent() )
        {
            const PeerProjectionResult projection =
                RefreshProjectedItems();
            wxTreeCtrl * const liveOwner =
                state->GetOwner(generation);
            if ( liveOwner != this || !liveOwner->m_winui ||
                 liveOwner->m_winui.get() != impl || impl->closed )
            {
                return false;
            }
            if ( projection != PeerProjectionResult::Done )
            {
                SchedulePeerSelectionCorrection();
                return false;
            }

            const bool selectionApplied =
                ApplySelectionToPeer();
            if ( selectionApplied &&
                 state->GetOwner(generation) == this )
            {
                impl->peerStructureRepairPending = false;
                return true;
            }
            return false;
        }
    }

    SchedulePeerSelectionCorrection();
    return false;
}

void wxTreeCtrl::SchedulePeerSelectionCorrection(bool newRequest)
{
    if ( !m_winui || !m_winui->treeView )
    {
        return;
    }

    constexpr unsigned MaxDeferredAttempts = 2;
    if ( newRequest && m_winui->peerCorrectionQuarantined )
    {
        m_winui->peerCorrectionQuarantined = false;
        m_winui->peerCorrectionAttempts = 0;
    }
    if ( !m_winui->peerCorrectionDriverActive )
    {
        m_winui->peerCorrectionDriverActive = true;
        m_winui->peerCorrectionAttempts = 0;
    }
    if ( m_winui->peerCorrectionPending )
        return;
    if ( m_winui->peerCorrectionQuarantined && !newRequest )
        return;
    if ( m_winui->peerCorrectionAttempts >= MaxDeferredAttempts )
    {
        m_winui->peerCorrectionQuarantined = true;
        wxLogWarning(
            "wxWinUI tree peer selection did not converge after bounded "
            "retries; the next selection request will re-arm it");
        return;
    }

    ++m_winui->peerCorrectionAttempts;
    m_winui->peerCorrectionPending = true;

    const std::weak_ptr<wxWinUITreeCallbackState> weakState =
        m_winui->callbackState;
    const std::uint64_t generation =
        m_winui->callbackState->Generation();
    const auto correction =
        [weakState, generation]()
        {
            const auto state = weakState.lock();
            wxTreeCtrl * const owner =
                state ? state->GetOwner(generation) : nullptr;
            if ( !owner || !owner->m_winui )
                return;

            owner->m_winui->peerCorrectionPending = false;
            owner->m_winui->rejectedSelectionId = 0;
            (void)owner->ApplySelectionToPeer();
        };

    auto dispatcher = m_winui->treeView.DispatcherQueue();
    if ( !dispatcher )
    {
        if ( wxTheApp )
            wxTheApp->CallAfter(correction);
        else
        {
            m_winui->peerCorrectionPending = false;
            m_winui->peerCorrectionQuarantined = true;
        }
        return;
    }

    if ( !dispatcher.TryEnqueue(correction) )
    {
        // Never recurse synchronously after a rejected queue write: persistent
        // dispatcher failure would otherwise turn correction into an
        // unbounded stack/CPU loop.
        m_winui->peerCorrectionPending = false;
        if ( wxTheApp )
        {
            m_winui->peerCorrectionPending = true;
            wxTheApp->CallAfter(correction);
        }
        else
        {
            m_winui->peerCorrectionQuarantined = true;
        }
    }
}

wxTreeCtrl::PeerProjectionResult
wxTreeCtrl::ProjectPeerItemPass(std::uint64_t itemId,
                                std::uint64_t revision,
                                wxWinUITreeCtrlImpl *impl)
{
    if ( !impl || !impl->callbackState )
        return PeerProjectionResult::Failed;

    const std::shared_ptr<wxWinUITreeCallbackState> state =
        impl->callbackState;
    const std::uint64_t generation = state->Generation();
    const auto getLiveOwner =
        [state, generation, impl]() -> wxTreeCtrl *
        {
            wxTreeCtrl * const owner = state->GetOwner(generation);
            return owner && owner->m_winui &&
                           owner->m_winui.get() == impl &&
                           owner->m_winui->callbackState == state &&
                           !impl->closed
                       ? owner
                       : nullptr;
        };

    wxTreeCtrl *owner = getLiveOwner();
    if ( !owner )
        return PeerProjectionResult::Failed;
    if ( impl->modelRevision != revision )
        return PeerProjectionResult::Stale;

    wxWinUITreeItem * const item = owner->ResolveItem(itemId);
    if ( !item || item->deleting || !item->node )
        return PeerProjectionResult::Done;

    // Snapshot all wx model state before the first observable call. Strong
    // WinRT and bitmap-bundle handles remain valid even if a callback retires
    // the item or destroys the control.
    const MUXC::TreeViewNode node = item->node;
    const std::uintptr_t nodeIdentity =
        wxWinUITreeNodeIdentity(node);
    const wxString text = item->text;
    const bool selected = item->selected;
    const bool expanded = item->expanded;
    const bool hasUnrealizedChildren =
        item->hasChildrenOverride && item->children.empty();
    const bool dropHighlighted = item->dropHighlighted;
    const bool bold = item->bold;
    const wxColour itemBackground = item->backgroundColour;
    const wxColour itemForeground = item->textColour;
    const wxFont itemFont = item->font;

    const int image =
        wxWinUIEffectiveTreeItemImage(item, selected, expanded);
    const int stateImage = item->state;
    const wxVector<wxBitmapBundle> stateBundles =
        impl->stateImageBundles;
    const bool attachContent = !item->contentAttached;
    WFC::PropertySet content = item->content;

    const auto checkCurrent =
        [state, generation, impl, revision, itemId,
         nodeIdentity]() -> PeerProjectionResult
        {
            wxTreeCtrl * const current = state->GetOwner(generation);
            if ( !current || !current->m_winui ||
                 current->m_winui.get() != impl ||
                 current->m_winui->callbackState != state ||
                 impl->closed )
            {
                return PeerProjectionResult::Failed;
            }
            if ( impl->modelRevision != revision )
                return PeerProjectionResult::Stale;

            wxWinUITreeItem * const liveItem =
                current->ResolveItem(itemId);
            return liveItem && !liveItem->deleting &&
                           wxWinUITreeNodeIdentity(liveItem->node) ==
                               nodeIdentity
                       ? PeerProjectionResult::Done
                       : PeerProjectionResult::Stale;
        };

    PeerProjectionResult snapshotResult = checkCurrent();
    if ( snapshotResult != PeerProjectionResult::Done )
        return snapshotResult;

    wxFont font = itemFont;
    if ( !font.IsOk() )
    {
        owner = getLiveOwner();
        if ( !owner )
            return PeerProjectionResult::Failed;
        font = owner->GetFont();
        snapshotResult = checkCurrent();
        if ( snapshotResult != PeerProjectionResult::Done )
            return snapshotResult;
    }

    owner = getLiveOwner();
    if ( !owner )
        return PeerProjectionResult::Failed;
    const bool hasExpandableItem = owner->HasExpandableItem();
    const bool showButtons = owner->HasFlag(wxTR_HAS_BUTTONS);
    const bool rightToLeft = impl->rightToLeft;
    const bool reclaimExpanderGutter =
        !showButtons || !hasExpandableItem;
    snapshotResult = checkCurrent();
    if ( snapshotResult != PeerProjectionResult::Done )
        return snapshotResult;

    owner = getLiveOwner();
    if ( !owner )
        return PeerProjectionResult::Failed;
    double scale = 0.0;
#ifdef WXWINUI_TEST_SUPPORT
    scale = impl->projectionScaleOverrideForTesting;
#endif
    if ( !std::isfinite(scale) || scale <= 0.0 )
    {
        scale = owner->GetDPIScaleFactor();
        snapshotResult = checkCurrent();
        if ( snapshotResult != PeerProjectionResult::Done )
            return snapshotResult;
    }
    if ( !std::isfinite(scale) || scale <= 0.0 )
        scale = 1.0;

    owner = getLiveOwner();
    if ( !owner )
        return PeerProjectionResult::Failed;
    const wxWithImages::Images normalBundles = owner->GetImages();
    snapshotResult = checkCurrent();
    if ( snapshotResult != PeerProjectionResult::Done )
        return snapshotResult;

    wxBitmap normalListBitmap;
    if ( normalBundles.empty() && image >= 0 )
    {
        owner = getLiveOwner();
        if ( !owner )
            return PeerProjectionResult::Failed;
        wxImageList * const imageList = owner->GetImageList();
        if ( imageList )
        {
            const int imageCount = imageList->GetImageCount();
            snapshotResult = checkCurrent();
            if ( snapshotResult != PeerProjectionResult::Done )
                return snapshotResult;

            owner = getLiveOwner();
            if ( !owner )
                return PeerProjectionResult::Failed;
            if ( owner->GetImageList() != imageList )
                return PeerProjectionResult::Stale;

            if ( image < imageCount )
            {
                normalListBitmap = imageList->GetBitmap(image);
                snapshotResult = checkCurrent();
                if ( snapshotResult != PeerProjectionResult::Done )
                    return snapshotResult;
            }
        }
    }

    wxBitmap stateListBitmap;
    if ( stateBundles.empty() && stateImage >= 0 )
    {
        owner = getLiveOwner();
        if ( !owner )
            return PeerProjectionResult::Failed;
        wxImageList * const imageList = owner->GetStateImageList();
        if ( imageList )
        {
            const int imageCount = imageList->GetImageCount();
            snapshotResult = checkCurrent();
            if ( snapshotResult != PeerProjectionResult::Done )
                return snapshotResult;

            owner = getLiveOwner();
            if ( !owner )
                return PeerProjectionResult::Failed;
            if ( owner->GetStateImageList() != imageList )
                return PeerProjectionResult::Stale;

            if ( stateImage < imageCount )
            {
                stateListBitmap = imageList->GetBitmap(stateImage);
                snapshotResult = checkCurrent();
                if ( snapshotResult != PeerProjectionResult::Done )
                    return snapshotResult;
            }
        }
    }

    try
    {
#ifdef WXWINUI_TEST_SUPPORT
        ++impl->peerUpdateCount;
#endif
        if ( !content )
            content = WFC::PropertySet();

        wxWinUITreePeerMutationGuard mutation(state);
        const auto insertProperty =
            [&content, &checkCurrent](const wchar_t *key,
                                      const auto& value)
            {
                content.Insert(key, value);
                return checkCurrent();
            };
        const auto removeProperty =
            [&content, &checkCurrent](const wchar_t *key)
            {
                const bool present = content.HasKey(key);
                PeerProjectionResult result = checkCurrent();
                if ( result != PeerProjectionResult::Done || !present )
                    return result;
                content.Remove(key);
                return checkCurrent();
            };
        const auto loadBundleBitmap =
             [&checkCurrent, scale](
                 const wxVector<wxBitmapBundle>& bundles,
                 int bitmapIndex,
                 wxBitmap *bitmap) -> PeerProjectionResult
             {
                *bitmap = wxBitmap();
                if ( bitmapIndex < 0 ||
                     static_cast<size_t>(bitmapIndex) >= bundles.size() )
                {
                    return PeerProjectionResult::Done;
                }

                const wxBitmapBundle selectedBundle =
                    bundles[bitmapIndex];
                struct SizePreference
                {
                    wxSize size;
                    int count = 0;
                };
                std::vector<SizePreference> preferences;
                std::vector<bool> preferredValid(
                    bundles.size(), false);
                for ( size_t bundleIndex = 0;
                      bundleIndex < bundles.size();
                      ++bundleIndex )
                {
                    const wxBitmapBundle& candidate =
                        bundles[bundleIndex];
                    if ( !candidate.IsOk() )
                        continue;

                    const wxSize preferred =
                        candidate.GetPreferredBitmapSizeAtScale(scale);
                    PeerProjectionResult result = checkCurrent();
                    if ( result != PeerProjectionResult::Done )
                        return result;
                    if ( preferred.x <= 0 || preferred.y <= 0 )
                        continue;

                    preferredValid[bundleIndex] = true;
                    const auto existing = std::find_if(
                        preferences.begin(),
                        preferences.end(),
                        [&preferred](const SizePreference& entry)
                        {
                            return entry.size == preferred;
                        });
                    if ( existing != preferences.end() )
                    {
                        ++existing->count;
                    }
                    else
                    {
                        preferences.push_back({preferred, 1});
                    }
                }

                int maximumVotes = 0;
                wxSize size;
                for ( const SizePreference& preference :
                      preferences )
                {
                    if ( preference.count > maximumVotes ||
                         (preference.count == maximumVotes &&
                          preference.size.y > size.y) )
                    {
                        maximumVotes = preference.count;
                        size = preference.size;
                    }
                }
                if ( size.x <= 0 || size.y <= 0 ||
                     !preferredValid[bitmapIndex] ||
                     !selectedBundle.IsOk() )
                {
                    return PeerProjectionResult::Done;
                }

                // Preserve the wrapper's callback order while making its
                // hidden GetDefaultSize()/GetBitmap() virtual boundaries
                // observable to the item revision transaction.
                const wxSize defaultSize =
                    selectedBundle.GetDefaultSize();
                PeerProjectionResult result = checkCurrent();
                if ( result != PeerProjectionResult::Done )
                    return result;
                wxBitmapBundleImpl * const bundleImpl =
                    selectedBundle.GetImpl();
                if ( defaultSize.x <= 0 || defaultSize.y <= 0 ||
                     !bundleImpl )
                {
                    return PeerProjectionResult::Done;
                }

                *bitmap = bundleImpl->GetBitmap(size);
                result = checkCurrent();
                if ( result != PeerProjectionResult::Done )
                    return result;
                if ( bitmap->IsOk() )
                {
                    const double bitmapScale =
                        static_cast<double>(size.y) /
                        defaultSize.y;
                    if ( !std::isfinite(bitmapScale) ||
                         bitmapScale <= 0.0 )
                    {
                        *bitmap = wxBitmap();
                        return PeerProjectionResult::Done;
                    }
                    // Match wxBitmapBundle::GetBitmap() without crossing an
                    // additional application callback after revalidation.
                    bitmap->SetScaleFactor(bitmapScale);
                }
                return checkCurrent();
            };

        PeerProjectionResult result = insertProperty(
            L"Text", winrt::box_value(wxWinUIToHString(text)));
        if ( result != PeerProjectionResult::Done )
            return result;

        wxBitmap bitmap = normalListBitmap;
        if ( !normalBundles.empty() )
        {
            result = loadBundleBitmap(normalBundles, image, &bitmap);
            if ( result != PeerProjectionResult::Done )
                return result;
        }

        bool hasImage = false;
        if ( bitmap.IsOk() )
        {
            const auto source = wxWinUIWriteableBitmapFromBitmap(bitmap);
            result = checkCurrent();
            if ( result != PeerProjectionResult::Done )
                return result;
            if ( source )
            {
                // PropertySet dimensions feed XAML Width/Height and are DIPs.
                // wxBitmap::GetLogicalSize() is physical on MSW, so only
                // GetDIPSize() avoids scaling a multi-resolution source twice.
                const wxSize dipSize = bitmap.GetDIPSize();
                result = insertProperty(L"Image", source);
                if ( result != PeerProjectionResult::Done )
                    return result;
                result = insertProperty(
                    L"ImageWidth",
                    winrt::box_value(
                        static_cast<double>(dipSize.x)));
                if ( result != PeerProjectionResult::Done )
                    return result;
                result = insertProperty(
                    L"ImageHeight",
                    winrt::box_value(
                        static_cast<double>(dipSize.y)));
                if ( result != PeerProjectionResult::Done )
                    return result;
                hasImage = true;
            }
        }
        if ( !hasImage )
        {
            for ( const wchar_t * const key :
                  {L"Image", L"ImageWidth", L"ImageHeight"} )
            {
                result = removeProperty(key);
                if ( result != PeerProjectionResult::Done )
                    return result;
            }
        }
        result = insertProperty(
            L"ImageVisibility",
            winrt::box_value(
                hasImage ? MUX::Visibility::Visible
                         : MUX::Visibility::Collapsed));
        if ( result != PeerProjectionResult::Done )
            return result;

        bitmap = stateListBitmap;
        if ( !stateBundles.empty() )
        {
            result = loadBundleBitmap(
                stateBundles, stateImage, &bitmap);
            if ( result != PeerProjectionResult::Done )
                return result;
        }

        bool hasStateImage = false;
        if ( bitmap.IsOk() )
        {
            const auto source = wxWinUIWriteableBitmapFromBitmap(bitmap);
            result = checkCurrent();
            if ( result != PeerProjectionResult::Done )
                return result;
            if ( source )
            {
                const wxSize dipSize = bitmap.GetDIPSize();
                result = insertProperty(L"StateImage", source);
                if ( result != PeerProjectionResult::Done )
                    return result;
                result = insertProperty(
                    L"StateImageWidth",
                    winrt::box_value(
                        static_cast<double>(dipSize.x)));
                if ( result != PeerProjectionResult::Done )
                    return result;
                result = insertProperty(
                    L"StateImageHeight",
                    winrt::box_value(
                        static_cast<double>(dipSize.y)));
                if ( result != PeerProjectionResult::Done )
                    return result;
                hasStateImage = true;
            }
        }
        if ( !hasStateImage )
        {
            for ( const wchar_t * const key :
                  {L"StateImage", L"StateImageWidth", L"StateImageHeight"} )
            {
                result = removeProperty(key);
                if ( result != PeerProjectionResult::Done )
                    return result;
            }
        }
        result = insertProperty(
            L"StateImageVisibility",
            winrt::box_value(
                hasStateImage ? MUX::Visibility::Visible
                              : MUX::Visibility::Collapsed));
        if ( result != PeerProjectionResult::Done )
            return result;

        result = insertProperty(
            L"ContentMargin",
            winrt::box_value(MUX::ThicknessHelper::FromLengths(
                !rightToLeft && reclaimExpanderGutter ? -28 : 0,
                0,
                rightToLeft && reclaimExpanderGutter ? -28 : 0,
                0)));
        if ( result != PeerProjectionResult::Done )
            return result;

        const wxColour background = dropHighlighted
            ? wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT)
            : itemBackground;
        if ( background.IsOk() )
        {
            result = insertProperty(
                L"Background",
                wxWinUIBrush(
                    background.Red(), background.Green(),
                    background.Blue(), background.Alpha()));
        }
        else
        {
            result = removeProperty(L"Background");
        }
        if ( result != PeerProjectionResult::Done )
            return result;

        const wxColour foreground =
            dropHighlighted && !itemForeground.IsOk()
                ? wxSystemSettings::GetColour(
                      wxSYS_COLOUR_HIGHLIGHTTEXT)
                : itemForeground;
        if ( foreground.IsOk() )
        {
            result = insertProperty(
                L"Foreground",
                wxWinUIBrush(
                    foreground.Red(), foreground.Green(),
                    foreground.Blue(), foreground.Alpha()));
        }
        else
        {
            result = removeProperty(L"Foreground");
        }
        if ( result != PeerProjectionResult::Done )
            return result;

        if ( font.IsOk() )
        {
            const wxString face = font.GetFaceName();
            result = face.empty()
                ? removeProperty(L"FontFamily")
                : insertProperty(
                      L"FontFamily",
                      MUXM::FontFamily(wxWinUIToHString(face)));
            if ( result != PeerProjectionResult::Done )
                return result;

            const double pointSize = font.GetFractionalPointSize();
            result = pointSize > 0
                ? insertProperty(
                      L"FontSize",
                      winrt::box_value(pointSize * 96.0 / 72.0))
                : removeProperty(L"FontSize");
            if ( result != PeerProjectionResult::Done )
                return result;

            winrt::Windows::UI::Text::FontWeight weight{};
            weight.Weight = static_cast<std::uint16_t>(
                bold ? wxFONTWEIGHT_BOLD : font.GetNumericWeight());
            result = insertProperty(
                L"FontWeight", winrt::box_value(weight));
            if ( result != PeerProjectionResult::Done )
                return result;

            const auto style =
                font.GetStyle() == wxFONTSTYLE_ITALIC
                    ? winrt::Windows::UI::Text::FontStyle::Italic
                : font.GetStyle() == wxFONTSTYLE_SLANT
                    ? winrt::Windows::UI::Text::FontStyle::Oblique
                    : winrt::Windows::UI::Text::FontStyle::Normal;
            result = insertProperty(
                L"FontStyle", winrt::box_value(style));
            if ( result != PeerProjectionResult::Done )
                return result;
        }
        else
        {
            for ( const wchar_t * const key :
                  {L"FontFamily", L"FontSize",
                   L"FontWeight", L"FontStyle"} )
            {
                result = removeProperty(key);
                if ( result != PeerProjectionResult::Done )
                    return result;
            }
        }

        // Publish content lookup state only after the observable node attach
        // completed and the item/revision are still current.
        if ( attachContent )
        {
            node.Content(content);
            result = checkCurrent();
            if ( result != PeerProjectionResult::Done )
                return result;

            owner = getLiveOwner();
            wxWinUITreeItem * const liveItem =
                owner ? owner->ResolveItem(itemId) : nullptr;
            if ( !liveItem )
                return PeerProjectionResult::Stale;
            impl->itemsByContent[
                wxWinUIInspectableIdentity(content)] = liveItem;
            liveItem->content = content;
            liveItem->contentAttached = true;
        }

        node.HasUnrealizedChildren(hasUnrealizedChildren);
        result = checkCurrent();
        if ( result != PeerProjectionResult::Done )
            return result;

        owner = getLiveOwner();
        wxWinUITreeItem * const committedItem =
            owner ? owner->ResolveItem(itemId) : nullptr;
        if ( !committedItem || committedItem->deleting )
            return PeerProjectionResult::Stale;
        committedItem->contentProjectionGeneration = revision;
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI TreeView item update", e);
        return PeerProjectionResult::Failed;
    }

    return checkCurrent();
}

wxTreeCtrl::PeerProjectionResult
wxTreeCtrl::SyncPeerProjection()
{
    if ( !m_winui || !m_winui->callbackState || m_winui->closed )
        return PeerProjectionResult::Failed;
    if ( IsFrozen() )
        return PeerProjectionResult::Stale;

    wxWinUITreeCtrlImpl * const impl = m_winui.get();
    const std::shared_ptr<wxWinUITreeCallbackState> state =
        impl->callbackState;
    const std::uint64_t generation = state->Generation();
    if ( impl->projectionInProgress )
        return PeerProjectionResult::Stale;

    impl->projectionInProgress = true;
    wxScopeGuard projectionGuard = wxMakeGuard(
        [state, generation, impl]()
        {
            wxTreeCtrl * const owner = state->GetOwner(generation);
            if ( owner && owner->m_winui &&
                 owner->m_winui.get() == impl &&
                 owner->m_winui->callbackState == state )
            {
                owner->m_winui->projectionInProgress = false;
            }
        });
    bool sawFailure = false;
    constexpr unsigned MaxSynchronousPasses = 8;
    for ( unsigned pass = 0; pass < MaxSynchronousPasses; ++pass )
    {
        wxTreeCtrl *owner = state->GetOwner(generation);
        if ( !owner || !owner->m_winui ||
             owner->m_winui.get() != impl ||
             owner->m_winui->callbackState != state )
        {
            return PeerProjectionResult::Failed;
        }

        const bool projectAll = impl->projectionAllRequested;
        impl->projectionAllRequested = false;
        std::vector<std::uint64_t> itemIds;
        if ( projectAll )
        {
            itemIds.reserve(impl->itemsById.size());
            for ( const auto& entry : impl->itemsById )
                itemIds.push_back(entry.first);
            impl->projectionItemIds.clear();
        }
        else
        {
            itemIds.reserve(impl->projectionItemIds.size());
            for ( const std::uint64_t itemId :
                  impl->projectionItemIds )
            {
                itemIds.push_back(itemId);
            }
            impl->projectionItemIds.clear();
        }

        const std::uint64_t revision = impl->modelRevision;
        PeerProjectionResult passResult =
            PeerProjectionResult::Done;
        for ( const std::uint64_t itemId : itemIds )
        {
            passResult = owner->ProjectPeerItemPass(
                itemId, revision, impl);
            owner = state->GetOwner(generation);
            if ( !owner || !owner->m_winui ||
                 owner->m_winui.get() != impl ||
                 owner->m_winui->callbackState != state )
            {
                return PeerProjectionResult::Failed;
            }
            if ( passResult != PeerProjectionResult::Done )
                break;
        }

        if ( passResult == PeerProjectionResult::Failed )
            sawFailure = true;
        if ( passResult != PeerProjectionResult::Done ||
             impl->modelRevision != revision )
        {
            if ( projectAll )
            {
                impl->projectionAllRequested = true;
            }
            else
            {
                impl->projectionItemIds.insert(
                    itemIds.begin(), itemIds.end());
            }
            continue;
        }

        if ( !impl->projectionAllRequested &&
             impl->projectionItemIds.empty() )
        {
            impl->projectionInProgress = false;
            projectionGuard.Dismiss();
            impl->projectionDeferredAttempts = 0;
            impl->projectionWarningIssued = false;

            if ( impl->realizedMetadataDirty )
            {
                const std::uint64_t metadataRevision =
                    impl->modelRevision;
                const bool synchronized =
                    wxWinUISyncRealizedTreeItems(impl);
                owner = state->GetOwner(generation);
                if ( !owner || !owner->m_winui ||
                     owner->m_winui.get() != impl ||
                     owner->m_winui->callbackState != state )
                {
                    return PeerProjectionResult::Failed;
                }
                if ( synchronized &&
                     impl->modelRevision == metadataRevision &&
                     !impl->projectionInProgress &&
                     !impl->projectionAllRequested &&
                     impl->projectionItemIds.empty() )
                {
                    impl->realizedMetadataDirty = false;
                }
            }
            return PeerProjectionResult::Done;
        }
    }

    wxTreeCtrl * const owner = state->GetOwner(generation);
    if ( !owner || !owner->m_winui ||
         owner->m_winui.get() != impl ||
         owner->m_winui->callbackState != state )
    {
        return PeerProjectionResult::Failed;
    }

    impl->projectionInProgress = false;
    projectionGuard.Dismiss();
    if ( impl->projectionDeferredAttempts == 0 &&
         !impl->projectionReplayScheduled && wxTheApp )
    {
        impl->projectionDeferredAttempts = 1;
        impl->projectionReplayScheduled = true;
        const std::weak_ptr<wxWinUITreeCallbackState> weakState(state);
        wxTheApp->CallAfter(
            [weakState, generation]()
            {
                const std::shared_ptr<wxWinUITreeCallbackState> state =
                    weakState.lock();
                if ( !state )
                    return;

                wxTreeCtrl * const liveOwner =
                    state->GetOwner(generation);
                if ( !liveOwner || !liveOwner->m_winui ||
                     liveOwner->m_winui->callbackState != state )
                {
                    return;
                }

                liveOwner->m_winui->projectionReplayScheduled = false;
                (void)liveOwner->SyncPeerProjection();
            });
    }
    else if ( !impl->projectionWarningIssued )
    {
        impl->projectionWarningIssued = true;
        wxLogWarning(
            "wxWinUI tree peer projection did not converge after "
            "bounded retries; it will resume on the next model change");
    }

    return sawFailure
        ? PeerProjectionResult::Failed
        : PeerProjectionResult::Stale;
}

wxTreeCtrl::PeerProjectionResult
wxTreeCtrl::RequestPeerProjection(
    const std::vector<std::uint64_t>& itemIds,
    bool allItems)
{
    if ( !m_winui || !m_winui->callbackState || m_winui->closed )
        return PeerProjectionResult::Failed;

    wxWinUITreeCtrlImpl * const impl = m_winui.get();
    const std::shared_ptr<wxWinUITreeCallbackState> state =
        impl->callbackState;
    const std::uint64_t generation = state->Generation();
    const std::uint64_t requestedRevision =
        impl->BumpModelRevision();
    if ( allItems )
    {
#ifdef WXWINUI_TEST_SUPPORT
        ++impl->fullRefreshCount;
#endif
        impl->projectionAllRequested = true;
        // A complete refresh subsumes every item-specific request accumulated
        // earlier in the same frozen transaction.
        impl->projectionItemIds.clear();
    }
    else if ( !impl->projectionAllRequested )
    {
        impl->projectionItemIds.insert(
            itemIds.begin(), itemIds.end());
    }

    // wxWindowBase invokes DoThaw() only for the outermost matching Thaw().
    // Keep the authoritative wx model and the lightweight peer hierarchy
    // current during the freeze, but defer bitmap/content projection until
    // then. The set above deduplicates repeated mutations without an
    // unbounded replay queue.
    if ( IsFrozen() )
        return PeerProjectionResult::Stale;

    if ( impl->projectionInProgress ||
         impl->projectionReplayScheduled )
        return PeerProjectionResult::Stale;

    const PeerProjectionResult result = SyncPeerProjection();
    wxTreeCtrl * const owner = state->GetOwner(generation);
    if ( !owner || !owner->m_winui ||
         owner->m_winui.get() != impl ||
         owner->m_winui->callbackState != state )
    {
        return PeerProjectionResult::Failed;
    }
    if ( impl->modelRevision != requestedRevision )
        return PeerProjectionResult::Stale;
    return result;
}

wxTreeCtrl::PeerProjectionResult
wxTreeCtrl::UpdatePeerItem(wxWinUITreeItem *item)
{
    if ( !item || !item->id || !item->node )
        return PeerProjectionResult::Done;

    return UpdatePeerItems({item->id});
}

wxTreeCtrl::PeerProjectionResult
wxTreeCtrl::UpdatePeerItems(
    const std::vector<std::uint64_t>& itemIds)
{
    return RequestPeerProjection(itemIds, false);
}

wxTreeCtrl::PeerProjectionResult
wxTreeCtrl::RefreshProjectedItems()
{
    return RequestPeerProjection({}, true);
}

void wxTreeCtrl::CancelDelayedLabelEdit()
{
    if ( !m_winui )
        return;

    wxWinUITreeCtrlImpl * const impl = m_winui.get();
    const MUXD::DispatcherQueueTimer timer =
        impl->labelEditTimer;
    const winrt::event_token token =
        impl->labelEditTimerToken;

    // Publish the terminal state before crossing either ABI boundary: a timer
    // callback drained by Stop()/revocation can only observe a retired ticket.
    impl->labelEditTimer = nullptr;
    impl->labelEditTimerToken = {};
    impl->clickEditCandidateId = 0;
    impl->pendingLabelEditId = 0;
    ++impl->labelEditDelayTicket;
    if ( impl->labelEditDelayTicket == 0 )
        ++impl->labelEditDelayTicket;

    if ( !timer )
        return;

    try
    {
        timer.Stop();
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException(
            "WinUI TreeView label-edit timer stop", e);
    }
    try
    {
        if ( token.value )
            timer.Tick(token);
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException(
            "WinUI TreeView label-edit timer revocation", e);
    }
}

bool wxTreeCtrl::ScheduleDelayedLabelEdit(wxWinUITreeItem *item)
{
    if ( !m_winui || !m_winui->callbackState || !item ||
         item->deleting || !HasFlag(wxTR_EDIT_LABELS) )
    {
        return false;
    }

    wxWinUITreeCtrlImpl * const impl = m_winui.get();
    const std::shared_ptr<wxWinUITreeCallbackState> state =
        impl->callbackState;
    const std::uint64_t generation = state->Generation();
    const std::uint64_t itemId = item->id;

    CancelDelayedLabelEdit();
    wxTreeCtrl *owner = state->GetOwner(generation);
    item = owner == this ? owner->ResolveItem(itemId) : nullptr;
    if ( !item || item->deleting || !owner->m_winui ||
         owner->m_winui.get() != impl ||
         owner->m_winui->selectedIds.size() != 1 ||
         !item->selected || owner->m_winui->selectionId != itemId )
    {
        return false;
    }

    MUXD::DispatcherQueue queue{ nullptr };
    try
    {
        queue = impl->treeView.DispatcherQueue();
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException(
            "WinUI TreeView label-edit dispatcher", e);
        return false;
    }
    owner = state->GetOwner(generation);
    if ( owner != this || !owner->m_winui ||
         owner->m_winui.get() != impl || !queue )
    {
        return false;
    }

    const std::uint64_t ticket = impl->labelEditDelayTicket;
    MUXD::DispatcherQueueTimer candidate{ nullptr };
    winrt::event_token candidateToken{};
    try
    {
        candidate = queue.CreateTimer();
        candidate.Interval(
            std::chrono::milliseconds(::GetDoubleClickTime()));
        candidate.IsRepeating(false);
        const std::weak_ptr<wxWinUITreeCallbackState> weakState =
            state;
        candidateToken = candidate.Tick(
            [weakState, generation, ticket](
                winrt::Windows::Foundation::IInspectable const&,
                winrt::Windows::Foundation::IInspectable const&)
            {
                const auto liveState = weakState.lock();
                wxTreeCtrl * const liveOwner =
                    liveState
                        ? liveState->GetOwner(generation)
                        : nullptr;
                if ( liveOwner )
                    (void)liveOwner->CompleteDelayedLabelEdit(ticket);
            });
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException(
            "WinUI TreeView label-edit timer creation", e);
        return false;
    }

    owner = state->GetOwner(generation);
    item = owner == this ? owner->ResolveItem(itemId) : nullptr;
    if ( !item || item->deleting || !owner->m_winui ||
         owner->m_winui.get() != impl ||
         owner->m_winui->labelEditDelayTicket != ticket )
    {
        try
        {
            candidate.Tick(candidateToken);
        }
        catch ( const winrt::hresult_error& )
        {
        }
        return false;
    }

    impl->pendingLabelEditId = itemId;
    impl->labelEditTimer = candidate;
    impl->labelEditTimerToken = candidateToken;
    try
    {
        candidate.Start();
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException(
            "WinUI TreeView label-edit timer start", e);
        owner = state->GetOwner(generation);
        if ( owner == this && owner->m_winui &&
             owner->m_winui.get() == impl )
        {
            owner->CancelDelayedLabelEdit();
        }
        return false;
    }

    owner = state->GetOwner(generation);
    return owner == this && owner->m_winui &&
           owner->m_winui.get() == impl &&
           impl->pendingLabelEditId == itemId &&
           impl->labelEditDelayTicket == ticket;
}

bool wxTreeCtrl::CompleteDelayedLabelEdit(std::uint64_t ticket)
{
    if ( !m_winui || !m_winui->callbackState ||
         ticket != m_winui->labelEditDelayTicket ||
         !m_winui->pendingLabelEditId )
    {
        return false;
    }

    wxWinUITreeCtrlImpl * const impl = m_winui.get();
    const std::shared_ptr<wxWinUITreeCallbackState> state =
        impl->callbackState;
    const std::uint64_t generation = state->Generation();
    const std::uint64_t itemId = impl->pendingLabelEditId;

    CancelDelayedLabelEdit();
    wxTreeCtrl * const owner = state->GetOwner(generation);
    if ( owner != this || !owner->m_winui ||
         owner->m_winui.get() != impl ||
         !owner->HasFlag(wxTR_EDIT_LABELS) ||
         impl->selectedIds.size() != 1 ||
         impl->selectionId != itemId )
    {
        return false;
    }

    wxWinUITreeItem * const item = owner->ResolveItem(itemId);
    if ( !item || item->deleting || !item->selected )
        return false;

    wxTextCtrl * const editor =
        owner->EditLabel(owner->MakeId(item));
    return state->GetOwner(generation) == owner &&
           editor && owner->m_editControl == editor;
}

void wxTreeCtrl::OnPeerPointerPressed(const wxPoint& pt)
{
    if ( !m_winui )
        return;

    CancelDelayedLabelEdit();
    if ( !m_winui )
        return;
    m_winui->pointerPressPoint = pt;
    m_winui->pressedStateImageId = 0;

    int flags = 0;
    const wxTreeItemId id = DoTreeHitTest(pt, flags);
    wxWinUITreeItem * const item = GetItem(id);
    if ( !item )
        return;

    if ( flags & wxTREE_HITTEST_ONITEMSTATEICON )
        m_winui->pressedStateImageId = item->id;

    if ( HasFlag(wxTR_EDIT_LABELS) &&
         (flags & wxTREE_HITTEST_ONITEMLABEL) &&
         item->selected && m_winui->selectionId == item->id &&
         m_winui->selectedIds.size() == 1 )
    {
        m_winui->clickEditCandidateId = item->id;
    }
}

void wxTreeCtrl::OnPeerPointerMoved(const wxPoint& pt)
{
    if ( !m_winui ||
         (!m_winui->clickEditCandidateId &&
          !m_winui->pendingLabelEditId &&
          !m_winui->pressedStateImageId) )
    {
        return;
    }

    const int limitX = wxMax(1, ::GetSystemMetrics(SM_CXDRAG));
    const int limitY = wxMax(1, ::GetSystemMetrics(SM_CYDRAG));
    if ( std::abs(pt.x - m_winui->pointerPressPoint.x) >= limitX ||
         std::abs(pt.y - m_winui->pointerPressPoint.y) >= limitY )
    {
        m_winui->pressedStateImageId = 0;
        CancelDelayedLabelEdit();
    }
}

void wxTreeCtrl::OnPeerPointerReleased(const wxPoint& pt)
{
    if ( !m_winui )
        return;

    const std::uint64_t pressedId =
        std::exchange(m_winui->pressedStateImageId, 0);
    if ( !pressedId )
        return;

    int flags = 0;
    const wxTreeItemId id = DoTreeHitTest(pt, flags);
    wxWinUITreeItem * const item = GetItem(id);
    if ( item && item->id == pressedId &&
         (flags & wxTREE_HITTEST_ONITEMSTATEICON) )
    {
        SendStateImageClick(item, pt);
    }
}

void wxTreeCtrl::OnPeerDoubleTapped(const wxPoint& pt)
{
    CancelDelayedLabelEdit();
    if ( !m_winui )
        return;

    int flags = 0;
    const wxTreeItemId id = DoTreeHitTest(pt, flags);
    wxWinUITreeItem * const item = GetItem(id);
    if ( item && (flags & wxTREE_HITTEST_ONITEM) )
        SendTreeEvent(wxEVT_TREE_ITEM_ACTIVATED, item);
}

void wxTreeCtrl::OnPeerItemInvoked(wxWinUITreeItem *item)
{
    if ( !m_winui || !item || item->deleting )
        return;

    const std::uint64_t itemId = item->id;
    const bool shouldEdit =
        m_winui->clickEditCandidateId == itemId;
    m_winui->clickEditCandidateId = 0;
    if ( shouldEdit )
        (void)ScheduleDelayedLabelEdit(item);
}

void wxTreeCtrl::SendStateImageClick(
    wxWinUITreeItem *item,
    const wxPoint& point)
{
    if ( !m_winui || !item || item->deleting )
        return;

    wxTreeEvent event(
        wxEVT_TREE_STATE_IMAGE_CLICK, this, MakeId(item));
    event.SetPoint(point);
    HandleWindowEvent(event);
}

void wxTreeCtrl::OnPeerSelectionChanged()
{
    // A correction is already queued: ignore the control's intermediate
    // selection thrashing until it runs (see SchedulePeerSelectionCorrection).
    if ( !m_winui || m_winui->peerCorrectionPending )
        return;

    CancelDelayedLabelEdit();
    if ( !m_winui )
        return;

    std::vector<std::uint64_t> peerSelection;

    try
    {
        if ( HasFlag(wxTR_MULTIPLE) )
        {
            auto selected = m_winui->treeView.SelectedNodes();
            peerSelection.reserve(selected.Size());
            for ( uint32_t i = 0; i < selected.Size(); ++i )
            {
                if ( wxWinUITreeItem * const item =
                         wxWinUIFindItemByNode(
                             m_winui.get(), selected.GetAt(i)) )
                {
                    peerSelection.push_back(item->id);
                }
            }
        }
        else
        {
            if ( wxWinUITreeItem * const item =
                     wxWinUIFindItemByNode(
                         m_winui.get(),
                         m_winui->treeView.SelectedNode()) )
            {
                peerSelection.push_back(item->id);
            }
        }
    }
    catch ( const winrt::hresult_error& e )
    {
        // A failed read is not an empty user selection. Keep the wx model
        // authoritative and repair the peer later instead of publishing a
        // spurious deselection.
        wxWinUILogException(
            "WinUI TreeView selection snapshot", e);
        SchedulePeerSelectionCorrection();
        return;
    }

    wxWinUITreeItem * const primary = peerSelection.empty()
        ? nullptr
        : ResolveItem(peerSelection.front());
    const std::uint64_t primaryId = primary ? primary->id : 0;

    // If the control insists on re-selecting an item whose selection the
    // application already refused (typically a wxTreebook category that has no
    // page), don't dispatch the event again: just push the real selection back.
    if ( primaryId &&
         primaryId == m_winui->rejectedSelectionId )
    {
        SchedulePeerSelectionCorrection();
        return;
    }

    const std::shared_ptr<wxWinUITreeCallbackState> state =
        m_winui->callbackState;
    const std::uint64_t generation = state->Generation();
    m_winui->inPeerSelectionChange = true;
    wxWinUITreeCtrlImpl * const impl = m_winui.get();
    const auto clearPeerSelectionChange =
        [state, generation, impl]()
        {
            wxTreeCtrl * const owner = state->GetOwner(generation);
            if ( owner && owner->m_winui &&
                 owner->m_winui.get() == impl &&
                 owner->m_winui->callbackState == state )
            {
                owner->m_winui->inPeerSelectionChange = false;
            }
        };
    wxScopeGuard peerSelectionGuard =
        wxMakeGuard(clearPeerSelectionChange);
    bool correctionNeeded = false;

    if ( HasFlag(wxTR_MULTIPLE) )
    {
        const std::unordered_set<std::uint64_t> peerIds(
            peerSelection.begin(), peerSelection.end());
        std::vector<std::uint64_t> removed;
        for ( const std::uint64_t selectedId : m_winui->selectedIds )
        {
            if ( !peerIds.count(selectedId) )
                removed.push_back(selectedId);
        }

        for ( const std::uint64_t removedId : removed )
        {
            if ( wxWinUITreeItem * const removedItem =
                     ResolveItem(removedId) )
            {
                const std::uint64_t operation =
                    state->BeginOperation();
                wxTreeEvent changing(
                    wxEVT_TREE_SEL_CHANGING,
                    this, wxTreeItemId());
                changing.SetOldItem(MakeId(removedItem));
                const bool processed =
                    HandleWindowEvent(changing);
                if ( state->GetOwner(generation) != this )
                    return;

                wxWinUITreeItem *liveRemoved =
                    ResolveItem(removedId);
                if ( !state->IsCurrentOperation(operation) ||
                     !liveRemoved ||
                     (processed && !changing.IsAllowed()) )
                {
                    correctionNeeded = true;
                    SchedulePeerSelectionCorrection();
                    continue;
                }

                liveRemoved->selected = false;
                m_winui->selectedIds.erase(removedId);
                if ( m_winui->selectionId == removedId )
                    m_winui->selectionId = 0;
                if ( UpdatePeerItem(liveRemoved) !=
                         PeerProjectionResult::Done ||
                     state->GetOwner(generation) != this ||
                     !state->IsCurrentOperation(operation) )
                {
                    return;
                }

                liveRemoved = ResolveItem(removedId);
                if ( !liveRemoved )
                    return;

                wxTreeEvent changed(
                    wxEVT_TREE_SEL_CHANGED,
                    this, wxTreeItemId());
                changed.SetOldItem(MakeId(liveRemoved));
                HandleWindowEvent(changed);
                if ( state->GetOwner(generation) != this )
                    return;
                if ( !state->IsCurrentOperation(operation) )
                {
                    // A nested selection made itself the latest writer from
                    // SEL_CHANGED. Never continue applying the stale peer
                    // snapshot over it.
                    SchedulePeerSelectionCorrection();
                    return;
                }
            }
            if ( state->GetOwner(generation) != this )
                return;
        }

        for ( const std::uint64_t selectedId : peerSelection )
        {
            if ( !m_winui->selectedIds.count(selectedId) )
            {
                std::uint64_t selectionOperation = 0;
                const bool selectionChanged =
                    ChangeSelection(
                        ResolveItem(selectedId), true, false,
                        &selectionOperation);
                if ( state->GetOwner(generation) != this )
                    return;
                if ( selectionOperation &&
                     !state->IsCurrentOperation(selectionOperation) )
                {
                    // The event callback installed a newer selection.
                    SchedulePeerSelectionCorrection();
                    return;
                }

                if ( !selectionChanged )
                {
                    correctionNeeded = true;
                    SchedulePeerSelectionCorrection();
                    m_winui->rejectedSelectionId = selectedId;
                    break;
                }
            }
            if ( state->GetOwner(generation) != this )
                return;
        }
        if ( !m_winui->selectionId && !peerSelection.empty() )
            m_winui->selectionId = peerSelection.front();
        if ( !m_winui->focusedId )
            m_winui->focusedId = m_winui->selectionId;
    }
    else
    {
        std::uint64_t selectionOperation = 0;
        const bool selectionChanged =
            ChangeSelection(
                primary, true, false, &selectionOperation);
        if ( state->GetOwner(generation) != this )
            return;
        if ( selectionOperation &&
             !state->IsCurrentOperation(selectionOperation) )
        {
            SchedulePeerSelectionCorrection();
            return;
        }

        if ( !selectionChanged )
        {
            correctionNeeded = true;
            SchedulePeerSelectionCorrection();
        }
    }

    if ( state->GetOwner(generation) != this )
        return;

    const bool correctionPending =
        m_winui->peerCorrectionPending;
    clearPeerSelectionChange();
    peerSelectionGuard.Dismiss();
    m_winui->rejectedSelectionId =
        correctionNeeded ? primaryId : 0;
    if ( correctionNeeded && !correctionPending )
    {
        if ( !ApplySelectionToPeer() ||
             state->GetOwner(generation) != this )
        {
            return;
        }
        m_winui->rejectedSelectionId = 0;
    }
}

void wxTreeCtrl::OnPeerRightTapped(const wxPoint& pt)
{
    int flags = 0;
    const wxTreeItemId id = DoTreeHitTest(pt, flags);
    if ( id.IsOk() )
        (void)SendRightClickEvents(GetItem(id), pt);
}

bool wxTreeCtrl::SendRightClickEvents(wxWinUITreeItem *item,
                                      const wxPoint& point)
{
    if ( !m_winui || !item || item->deleting )
        return false;

    const std::uint64_t itemId = item->id;
    const std::shared_ptr<wxWinUITreeCallbackState> state =
        m_winui->callbackState;
    const std::uint64_t generation = state->Generation();
    wxTreeEvent rclick(
        wxEVT_TREE_ITEM_RIGHT_CLICK, this, MakeId(item));
    rclick.SetPoint(point);
    HandleWindowEvent(rclick);
    if ( state->GetOwner(generation) != this )
        return false;

    // As under wxMSW, a right click is also the context menu request.
    item = ResolveItem(itemId);
    if ( !item || item->deleting )
        return false;

    wxTreeEvent menu(
        wxEVT_TREE_ITEM_MENU, this, MakeId(item));
    menu.SetPoint(point);
    HandleWindowEvent(menu);
    return state->GetOwner(generation) == this;
}

void wxTreeCtrl::OnPeerNodeExpanded(wxWinUITreeItem *item)
{
    if ( !item || item->expanded )
        return;

    SetExpanded(
        item,
        true,
        true,
        PeerExpansionMode::PeerAlreadyUpdated);
}

void wxTreeCtrl::OnPeerNodeCollapsed(wxWinUITreeItem *item)
{
    if ( !item || !item->expanded )
        return;

    SetExpanded(
        item,
        false,
        true,
        PeerExpansionMode::PeerAlreadyUpdated);
}

bool wxTreeCtrl::OnPeerKeyDown(int keyCode,
                               wchar_t unicode,
                               bool controlDown,
                               bool shiftDown,
                               bool altDown,
                               bool modifiersProvided)
{
    if ( !m_winui )
        return false;

    const std::shared_ptr<wxWinUITreeCallbackState> state =
        m_winui->callbackState;
    const std::uint64_t generation = state->Generation();
    wxKeyEvent keyEvent(wxEVT_KEY_DOWN);
    keyEvent.m_keyCode = keyCode;
#if wxUSE_UNICODE
    keyEvent.m_uniChar = unicode;
#endif
    keyEvent.m_shiftDown = modifiersProvided
        ? shiftDown
        : (::GetKeyState(VK_SHIFT) & 0x8000) != 0;
    keyEvent.m_controlDown = modifiersProvided
        ? controlDown
        : (::GetKeyState(VK_CONTROL) & 0x8000) != 0;
    keyEvent.m_altDown = modifiersProvided
        ? altDown
        : (::GetKeyState(VK_MENU) & 0x8000) != 0;
    keyEvent.SetEventObject(this);
    keyEvent.SetId(GetId());

    wxTreeEvent treeEvent(wxEVT_TREE_KEY_DOWN, this);
    treeEvent.SetKeyEvent(keyEvent);
    const bool processed =
        HandleWindowEvent(treeEvent);
    if ( state->GetOwner(generation) != this )
        return true;
    if ( processed && !treeEvent.GetSkipped() )
        return true;

    if ( keyCode == WXK_MENU )
    {
        wxWinUITreeItem * const current =
            ResolveItem(m_winui->focusedId
                ? m_winui->focusedId
                : m_winui->selectionId);
        if ( !current )
            return false;

        const std::uint64_t itemId = current->id;
        wxRect itemRect;
        if ( !GetBoundingRect(MakeId(current), itemRect, true) ||
             state->GetOwner(generation) != this )
        {
            return false;
        }
        wxWinUITreeItem * const liveCurrent = ResolveItem(itemId);
        if ( !liveCurrent )
            return false;

        wxTreeEvent menu(
            wxEVT_TREE_ITEM_MENU, this, MakeId(liveCurrent));
        menu.SetPoint(wxPoint(
            itemRect.x, itemRect.y + itemRect.height / 2));
        HandleWindowEvent(menu);
        return true;
    }

    if ( keyCode == WXK_RETURN ||
         keyCode == WXK_NUMPAD_ENTER ||
         keyCode == WXK_SPACE ||
         keyCode == ' ' )
    {
        if ( keyEvent.HasModifiers() )
            return false;

        wxWinUITreeItem * const current =
            ResolveItem(m_winui->focusedId
                ? m_winui->focusedId
                : m_winui->selectionId);
        if ( !current )
            return false;
        SendTreeEvent(wxEVT_TREE_ITEM_ACTIVATED, current);
        return true;
    }

    if ( keyCode == WXK_F2 && HasFlag(wxTR_EDIT_LABELS) )
    {
        CancelDelayedLabelEdit();
        if ( state->GetOwner(generation) != this )
            return true;
        wxWinUITreeItem * const current =
            ResolveItem(m_winui->focusedId
                ? m_winui->focusedId
                : m_winui->selectionId);
        return current &&
               EditLabel(MakeId(current)) != nullptr;
    }

    return false;
}

bool wxTreeCtrl::BeginInternalDrag(wxWinUITreeItem *item,
                                   const wxPoint& point)
{
    if ( !m_winui || !item || item->deleting )
        return false;

    const std::shared_ptr<wxWinUITreeCallbackState> state =
        m_winui->callbackState;
    const std::uint64_t generation = state->Generation();
    const std::uint64_t operation = state->BeginOperation();
    const std::uint64_t itemId = item->id;

    CancelDelayedLabelEdit();
    if ( state->GetOwner(generation) != this || !m_winui )
        return false;
    m_winui->pressedStateImageId = 0;

    wxTreeEvent event(
        wxEVT_TREE_BEGIN_DRAG, this, MakeId(item));
    event.SetPoint(point);
    // BEGIN_DRAG is the exceptional notify event that is denied by default:
    // applications must opt into managing the move from END_DRAG.
    event.Veto();
    const bool processed =
        HandleWindowEvent(event);
    if ( state->GetOwner(generation) != this ||
         !state->IsCurrentOperation(operation) ||
         !ResolveItem(itemId) )
    {
        return false;
    }

    if ( !processed || !event.IsAllowed() )
        return false;

    m_winui->dragItemId = itemId;
    ++m_winui->dragSession;
    return true;
}

bool wxTreeCtrl::HandlePeerDragStarting(
    wxWinUITreeItem *item,
    const wxPoint& point)
{
    return BeginInternalDrag(item, point);
}

void wxTreeCtrl::HandlePeerDragCompleted(
    wxWinUITreeItem *target,
    const wxPoint& point,
    bool dropResultNone)
{
    // With CanReorderItems(false), WinUI reports DropResult::None for a
    // perfectly valid release over a TreeView item because no native reorder
    // operation was accepted. wxTreeCtrl still owes END_DRAG in that case:
    // None is cancellation only when there is no exact target under release.
    CompleteInternalDrag(
        target, point, dropResultNone && !target);
}

void wxTreeCtrl::CompleteInternalDrag(wxWinUITreeItem *target,
                                      const wxPoint& point,
                                      bool cancelled)
{
    if ( !m_winui || !m_winui->dragItemId )
        return;

    // Clear first so reentrant event handling cannot complete the same drag a
    // second time. WinUI internal drag events are intentionally independent
    // of the HWND/OLE broker used by wxDropTarget.
    m_winui->dragItemId = 0;
    ++m_winui->dragSession;
    if ( cancelled )
        return;

    wxTreeEvent event(
        wxEVT_TREE_END_DRAG, this, MakeId(target));
    event.SetPoint(point);
    HandleWindowEvent(event);
}

#endif // wxUSE_TREECTRL
