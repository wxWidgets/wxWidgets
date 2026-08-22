/////////////////////////////////////////////////////////////////////////////
// Name:        src/winui/menubar.cpp
// Purpose:     wxWinUI frame and popup menus
// Author:      wxWidgets development team
// Created:     2026-06-01
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#include "wx/winui/winui.h"

#if wxUSE_WINUI3 && wxUSE_MENUS

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/control.h"
    #include "wx/event.h"
    #include "wx/frame.h"
    #include "wx/menu.h"
    #include "wx/timer.h"
    #include "wx/utils.h"
#endif

#include "wx/evtloop.h"
#include "wx/msw/private.h"
#include "wx/weakref.h"

#include "private.h"
#include "wx/winui/private/menutest.h"
#include "wx/winui/private/tlwhost.h"
#include "wx/winui/private/tlwhostmsw.h"

#include <algorithm>
#include <chrono>
#include <memory>
#include <utility>
#include <vector>

#include <winrt/Microsoft.UI.Dispatching.h>
#include <winrt/Microsoft.UI.Xaml.Automation.h>
#include <winrt/Microsoft.UI.Xaml.Automation.Peers.h>
#include <winrt/Microsoft.UI.Xaml.Automation.Provider.h>
#include <winrt/Microsoft.UI.Xaml.Input.h>
#include <winrt/Microsoft.UI.Xaml.Controls.Primitives.h>
#include <winrt/Microsoft.UI.Xaml.Media.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Xaml.Interop.h>

namespace MUX = winrt::Microsoft::UI::Xaml;
namespace MUXA = winrt::Microsoft::UI::Xaml::Automation;
namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;
namespace MUXCP = winrt::Microsoft::UI::Xaml::Controls::Primitives;
namespace MUXD = winrt::Microsoft::UI::Dispatching;
namespace MUXM = winrt::Microsoft::UI::Xaml::Media;
namespace MUXAP = winrt::Microsoft::UI::Xaml::Automation::Peers;
namespace MUXAPR = winrt::Microsoft::UI::Xaml::Automation::Provider;
namespace WFC = winrt::Windows::Foundation::Collections;

namespace
{

using wxWinUIMenuItems = WFC::IVector<MUXC::MenuFlyoutItemBase>;

struct wxWinUIMenuProjection;

struct wxWinUIPendingMenuCommand
{
    wxWeakRef<wxMenu> owner;
    int id = wxID_NONE;

    bool IsOk() const { return owner && id != wxID_NONE; }

    void Clear()
    {
        owner.Release();
        id = wxID_NONE;
    }
};

// Every XAML generation has one invalidatable state. Event handlers only keep
// weak_ptrs to it, so replacing a MenuBar cannot create a peer -> callback ->
// peer retention cycle or call a destroyed wx frame.
struct wxWinUIMenuProjectionState
{
    bool active = true;
    bool menuBarMode = false;
    bool rebuildPending = false;
    bool deferredActionsScheduled = false;
    unsigned openMenus = 0;
    double bitmapScaleForTesting = 0.0;
    MUX::FlowDirection flowDirection =
        MUX::FlowDirection::LeftToRight;

    wxWeakRef<wxWindow> invokingWindow;
    wxWeakRef<wxMenuBar> expectedMenuBar;
    wxWeakRef<wxWindow> menuBarWindow;
    wxWinUIPendingMenuCommand pendingCommand;
    MUXD::DispatcherQueueTimer submenuMonitor{ nullptr };
    winrt::event_token submenuMonitorToken{};
    unsigned long long submenuMonitorGeneration = 0;
    std::vector<std::shared_ptr<wxWinUIMenuProjection>> projections;
};

struct wxWinUIMenuProjection
{
    std::weak_ptr<wxWinUIMenuProjectionState> state;
    wxWeakRef<wxMenu> menu;
    wxWinUIMenuItems items{ nullptr };
    MUX::FrameworkElement lifecycleOwner{ nullptr };
    MUXCP::FlyoutBase lifecycleFlyout{ nullptr };
    winrt::event_token ownerLoadedToken{};
    winrt::event_token openingToken{};
    winrt::event_token closedToken{};
    unsigned long long reconciliationGeneration = 0;
    MUXC::MenuFlyoutItem sentinel{ nullptr };
    std::weak_ptr<wxWinUIMenuProjection> parent;
    bool physicalExpandedKnown = false;
    bool physicalExpanded = false;
    unsigned physicalCollapsedSamples = 0;
    bool open = false;
    std::vector<std::shared_ptr<wxWinUIMenuProjection>> children;
};

struct wxWinUIMenuLabel
{
    wxString text;
    wxString accessKey;
    wxString accelerator;
};

wxWinUIMenuLabel wxWinUIParseMenuLabel(const wxString& label,
                                       const wxMenuItem *item = nullptr)
{
    wxWinUIMenuLabel result;

    const wxString labelWithoutAccelerator = label.BeforeFirst('\t');
    const int accessIndex =
        wxControl::FindAccelIndex(labelWithoutAccelerator, &result.text);
    if ( accessIndex >= 0 &&
            static_cast<size_t>(accessIndex) < result.text.length() )
    {
        result.accessKey = result.text.Mid(accessIndex, 1);
    }

#if wxUSE_ACCEL
    if ( item )
    {
        std::unique_ptr<wxAcceleratorEntry> accel(item->GetAccel());
        if ( accel )
        {
            result.accelerator = accel->ToString();
        }
        else if ( !item->GetExtraAccels().empty() )
        {
            result.accelerator = item->GetExtraAccels().front().ToString();
        }
    }
#else
    wxUnusedVar(item);
#endif

    return result;
}

wxMenuItem *wxWinUIFindCurrentMenuItem(wxMenu *menu,
                                       size_t position,
                                       int expectedId)
{
    if ( !menu || position >= menu->GetMenuItemCount() )
        return nullptr;

    wxMenuItem * const item = menu->FindItemByPosition(position);
    if ( !item || (!item->IsSubMenu() && item->GetId() != expectedId) )
        return nullptr;

    return item;
}

void wxWinUISendMenuHighlight(
    const std::weak_ptr<wxWinUIMenuProjectionState>& weakState,
    const wxWeakRef<wxMenu>& weakMenu,
    size_t position,
    int id)
{
    const std::shared_ptr<wxWinUIMenuProjectionState> state =
        weakState.lock();
    wxWindow * const invoking = state ? state->invokingWindow.get() : nullptr;
    wxMenu * const menu = weakMenu.get();
    if ( !state || !state->active || !invoking || !menu )
        return;

    wxMenuItem * const item =
        wxWinUIFindCurrentMenuItem(menu, position, id);
    if ( !item )
        return;

    const int eventId = item->IsSubMenu() ? wxID_NONE : item->GetId();
    wxMenuEvent event(wxEVT_MENU_HIGHLIGHT, eventId, menu, item);
    if ( !wxMenu::ProcessMenuEvent(menu, event, invoking) &&
            eventId == wxID_NONE )
    {
        if ( wxFrame * const frame =
                wxDynamicCast(wxGetTopLevelParent(invoking), wxFrame) )
        {
            frame->DoGiveHelp(wxString(), true);
        }
    }
}

void wxWinUIAttachMenuHighlight(
    const MUXC::MenuFlyoutItemBase& peer,
    const std::shared_ptr<wxWinUIMenuProjectionState>& state,
    wxMenu *menu,
    size_t position,
    int id)
{
    const std::weak_ptr<wxWinUIMenuProjectionState> weakState(state);
    const wxWeakRef<wxMenu> weakMenu(menu);

    peer.PointerEntered(
        [weakState, weakMenu, position, id](
            const winrt::Windows::Foundation::IInspectable&,
            const MUX::Input::PointerRoutedEventArgs&)
        {
            wxWinUISendMenuHighlight(
                weakState, weakMenu, position, id);
        });

    peer.GotFocus(
        [weakState, weakMenu, position, id](
            const winrt::Windows::Foundation::IInspectable&,
            const MUX::RoutedEventArgs&)
        {
            wxWinUISendMenuHighlight(
                weakState, weakMenu, position, id);
        });
}

wxBitmap wxWinUIGetMenuItemBitmap(wxMenuItem *item,
                                  double requestedScale)
{
    const bool checkedBitmap =
        !item->IsCheckable() || item->IsChecked();
    if ( requestedScale > 0.0 )
    {
        const wxBitmapBundle bundle =
            item->GetBitmapBundle(checkedBitmap);
        if ( bundle.IsOk() )
        {
            return bundle.GetBitmap(
                bundle.GetPreferredBitmapSizeAtScale(
                    requestedScale));
        }
    }

    return item->GetBitmap(checkedBitmap);
}

void wxWinUIApplyMenuItemPresentation(
    const MUXC::MenuFlyoutItemBase& peer,
    wxMenuItem *item,
    const wxWinUIMenuProjectionState& state)
{
    const wxWinUIMenuLabel label =
        wxWinUIParseMenuLabel(item->GetItemLabel(), item);

    peer.IsEnabled(item->IsEnabled());
    peer.AccessKey(wxWinUIToHString(label.accessKey));

    MUXC::IconElement icon{ nullptr };
    peer.FlowDirection(state.flowDirection);

    const wxBitmap bitmap = wxWinUIGetMenuItemBitmap(
        item, state.bitmapScaleForTesting);
    if ( bitmap.IsOk() )
    {
        if ( auto source = wxWinUIWriteableBitmapFromBitmap(bitmap) )
        {
            MUXC::ImageIcon imageIcon;
            imageIcon.Source(source);
            icon = imageIcon;
        }
    }

    if ( const auto normal = peer.try_as<MUXC::MenuFlyoutItem>() )
    {
        normal.Text(wxWinUIToHString(label.text));
        normal.KeyboardAcceleratorTextOverride(
            wxWinUIToHString(label.accelerator));
        if ( icon )
            normal.Icon(icon);
    }
    else if ( const auto submenu = peer.try_as<MUXC::MenuFlyoutSubItem>() )
    {
        submenu.Text(wxWinUIToHString(label.text));
        if ( icon )
            submenu.Icon(icon);
    }
}

void wxWinUIScheduleMenuBarActions(
    const std::shared_ptr<wxWinUIMenuProjectionState>& state);

void wxWinUIRecordMenuCommand(
    const std::weak_ptr<wxWinUIMenuProjectionState>& weakState,
    const wxWeakRef<wxMenu>& weakOwner,
    int id)
{
    const std::shared_ptr<wxWinUIMenuProjectionState> state =
        weakState.lock();
    if ( !state || !state->active || !weakOwner )
        return;

    state->pendingCommand.owner = weakOwner;
    state->pendingCommand.id = id;

    // A sentinel should have marked the menu open before an item can be
    // clicked. Keep this fallback for a future XAML template which realizes
    // collapsed children differently.
    if ( state->menuBarMode && state->openMenus == 0 )
        wxWinUIScheduleMenuBarActions(state);
}

void wxWinUIAttachMenuCommand(
    const MUXC::MenuFlyoutItemBase& peer,
    const std::shared_ptr<wxWinUIMenuProjectionState>& state,
    wxMenu *owner,
    int id)
{
    const std::weak_ptr<wxWinUIMenuProjectionState> weakState(state);
    const wxWeakRef<wxMenu> weakOwner(owner);

    if ( const auto normal = peer.try_as<MUXC::MenuFlyoutItem>() )
    {
        normal.Click(
            [weakState, weakOwner, id](
                const winrt::Windows::Foundation::IInspectable&,
                const MUX::RoutedEventArgs&)
            {
                wxWinUIRecordMenuCommand(weakState, weakOwner, id);
            });
    }
}

void wxWinUIPopulateMenuItems(
    const wxWinUIMenuItems& items,
    wxMenu *menu,
    const std::shared_ptr<wxWinUIMenuProjectionState>& state,
    uint32_t retainedItems,
    const std::shared_ptr<wxWinUIMenuProjection>& parentProjection);

void wxWinUIEndMenuProjection(
    const std::shared_ptr<wxWinUIMenuProjection>& projection);

void wxWinUIBeginMenuProjection(
    const std::shared_ptr<wxWinUIMenuProjection>& projection);

void wxWinUIStopSubmenuMonitor(
    const std::shared_ptr<wxWinUIMenuProjectionState>& state);

MUXAPR::IExpandCollapseProvider wxWinUIGetMenuExpandCollapseProvider(
    const MUX::FrameworkElement& element)
{
    if ( !element )
        return nullptr;

    MUXAP::AutomationPeer peer =
        MUXAP::FrameworkElementAutomationPeer::CreatePeerForElement(element);

    // MenuBarItem does not necessarily have a peer until an automation client
    // asks for one. Its public peer constructor gives lifecycle reconciliation
    // the same physical IsFlyoutOpen state even in an ordinary mouse session.
    if ( !peer )
    {
        if ( const auto topItem = element.try_as<MUXC::MenuBarItem>() )
            peer = MUXAP::MenuBarItemAutomationPeer(topItem);
    }

    return peer.try_as<MUXAPR::IExpandCollapseProvider>();
}

bool wxWinUITryGetMenuExpanded(
    const std::shared_ptr<wxWinUIMenuProjection>& projection,
    bool *expanded)
{
    if ( !projection || !expanded || !projection->lifecycleOwner )
        return false;

    try
    {
        const auto provider = wxWinUIGetMenuExpandCollapseProvider(
            projection->lifecycleOwner);
        if ( !provider )
            return false;

        *expanded =
            provider.ExpandCollapseState() ==
            MUXA::ExpandCollapseState::Expanded;
        return true;
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException(
            "WinUI menu expand/collapse state reconciliation", e);
        return false;
    }
}

enum class wxWinUIMenuReconcileReason
{
    SentinelUnloaded,
    FlyoutClosed
};

constexpr unsigned wxWinUIMaxMenuReconcilePasses = 16;

void wxWinUICancelMenuProjectionReconciliation(
    const std::shared_ptr<wxWinUIMenuProjection>& projection)
{
    if ( projection && ++projection->reconciliationGeneration == 0 )
        ++projection->reconciliationGeneration;
}

bool wxWinUIQueueMenuProjectionReconciliation(
    const std::shared_ptr<wxWinUIMenuProjection>& projection,
    wxWinUIMenuReconcileReason reason,
    unsigned long long generation,
    unsigned pass);

void wxWinUIRunMenuProjectionReconciliation(
    const std::weak_ptr<wxWinUIMenuProjection>& weakProjection,
    wxWinUIMenuReconcileReason reason,
    unsigned long long generation,
    unsigned pass)
{
    const auto projection = weakProjection.lock();
    const auto state = projection ? projection->state.lock() : nullptr;
    if ( !projection || !state || !state->active || !projection->open ||
            projection->reconciliationGeneration != generation )
    {
        return;
    }

    bool sentinelLoaded = false;
    if ( reason == wxWinUIMenuReconcileReason::SentinelUnloaded )
    {
        try
        {
            sentinelLoaded =
                projection->sentinel && projection->sentinel.IsLoaded();
        }
        catch ( const winrt::hresult_error& )
        {
            // A detached/retired peer is equivalent to an unloaded one.
        }
    }
    else
    {
        // WinUI may close the parent MenuBarItemFlyout while transferring the
        // popup chain to an expanded MenuFlyoutSubItem. The wx ancestor menu
        // remains open for the lifetime of that descendant; its sentinel
        // unload will provide the close authority if the entire chain really
        // disappears.
        const auto children = projection->children;
        bool descendantExpanded = false;
        for ( const auto& child : children )
        {
            bool expanded = false;
            if ( child &&
                    (child->open ||
                     (wxWinUITryGetMenuExpanded(child, &expanded) &&
                      expanded)) )
            {
                descendantExpanded = true;
                break;
            }
        }
        if ( descendantExpanded )
        {
            return;
        }
    }

    bool expanded = false;
    const bool expandedKnown =
        wxWinUITryGetMenuExpanded(projection, &expanded);

    // MenuBarItemFlyout can briefly close and reopen when one of its child
    // submenus changes state. Conversely, a submenu peer can continue to
    // report Expanded for a dispatcher turn after its sentinel was unloaded.
    // Yield a bounded number of low-priority passes for either transition to
    // settle before publishing the canonical wx CLOSE.
    if ( reason == wxWinUIMenuReconcileReason::FlyoutClosed &&
            expandedKnown && expanded )
    {
        return;
    }

    if ( pass + 1 < wxWinUIMaxMenuReconcilePasses &&
            wxWinUIQueueMenuProjectionReconciliation(
                projection, reason, generation, pass + 1) )
    {
        return;
    }

    // The automation provider is the final physical state authority. A
    // sentinel may remain unrealized while an open presenter virtualizes its
    // first item, so never publish CLOSE while the provider still reports
    // Expanded. If the peer disappeared or can't be queried during shutdown,
    // the real Unloaded/Closed notification remains authoritative.
    if ( expandedKnown && expanded )
        return;
    if ( reason == wxWinUIMenuReconcileReason::SentinelUnloaded &&
            sentinelLoaded && !expandedKnown )
    {
        return;
    }

    if ( !expandedKnown || !expanded )
    {
        wxWinUIEndMenuProjection(projection);
    }
}

bool wxWinUIQueueMenuProjectionReconciliation(
    const std::shared_ptr<wxWinUIMenuProjection>& projection,
    wxWinUIMenuReconcileReason reason,
    unsigned long long generation,
    unsigned pass)
{
    try
    {
        const MUXD::DispatcherQueue queue =
            projection && projection->lifecycleOwner
                ? projection->lifecycleOwner.DispatcherQueue()
                : nullptr;
        if ( !queue )
            return false;

        const std::weak_ptr<wxWinUIMenuProjection> weakProjection(projection);
        return queue.TryEnqueue(
            MUXD::DispatcherQueuePriority::Low,
            [weakProjection, reason, generation, pass]()
            {
                wxWinUIRunMenuProjectionReconciliation(
                    weakProjection, reason, generation, pass);
            });
    }
    catch ( const winrt::hresult_error& )
    {
        return false;
    }
}

void wxWinUIStartMenuProjectionReconciliation(
    const std::shared_ptr<wxWinUIMenuProjection>& projection,
    wxWinUIMenuReconcileReason reason)
{
    const auto state = projection ? projection->state.lock() : nullptr;
    if ( !projection || !state || !state->active || !projection->open )
        return;

    wxWinUICancelMenuProjectionReconciliation(projection);
    const unsigned long long generation =
        projection->reconciliationGeneration;
    if ( !wxWinUIQueueMenuProjectionReconciliation(
             projection, reason, generation, 0) )
    {
        // Queue shutdown must not leave an open projection pinning deferred
        // mutations forever. The end transition remains exact-once.
        wxWinUIEndMenuProjection(projection);
    }
}

void wxWinUIRevokeMenuProjectionOwnerLoaded(
    const std::shared_ptr<wxWinUIMenuProjection>& projection)
{
    if ( !projection )
        return;

    const MUX::FrameworkElement owner = projection->lifecycleOwner;
    const winrt::event_token token = projection->ownerLoadedToken;
    projection->ownerLoadedToken = {};
    if ( !owner || !token.value )
        return;

    try
    {
        owner.Loaded(token);
    }
    catch ( const winrt::hresult_error& )
    {
    }
}

void wxWinUIRevokeMenuProjectionFlyoutLifecycle(
    const std::shared_ptr<wxWinUIMenuProjection>& projection)
{
    if ( !projection )
        return;

    const MUXCP::FlyoutBase flyout = projection->lifecycleFlyout;
    const winrt::event_token openingToken = projection->openingToken;
    const winrt::event_token closedToken = projection->closedToken;
    projection->openingToken = {};
    projection->closedToken = {};
    projection->lifecycleFlyout = nullptr;
    if ( !flyout )
        return;

    try
    {
        if ( openingToken.value )
            flyout.Opening(openingToken);
    }
    catch ( const winrt::hresult_error& )
    {
    }

    try
    {
        if ( closedToken.value )
            flyout.Closed(closedToken);
    }
    catch ( const winrt::hresult_error& )
    {
    }
}

void wxWinUIRevokeMenuProjectionLifecycle(
    const std::shared_ptr<wxWinUIMenuProjection>& projection)
{
    wxWinUIRevokeMenuProjectionOwnerLoaded(projection);
    wxWinUIRevokeMenuProjectionFlyoutLifecycle(projection);
}

void wxWinUIClearMenuProjectionState(
    const std::shared_ptr<wxWinUIMenuProjectionState>& state)
{
    if ( !state )
        return;

    wxWinUIStopSubmenuMonitor(state);
    const auto projections = state->projections;
    for ( auto it = projections.rbegin(); it != projections.rend(); ++it )
        wxWinUIRevokeMenuProjectionLifecycle(*it);
    state->projections.clear();
}

void wxWinUIEraseProjectionTree(
    const std::shared_ptr<wxWinUIMenuProjectionState>& state,
    const std::shared_ptr<wxWinUIMenuProjection>& projection)
{
    if ( !state || !projection )
        return;

    const auto children = projection->children;
    for ( auto it = children.rbegin(); it != children.rend(); ++it )
        wxWinUIEraseProjectionTree(state, *it);
    projection->children.clear();

    if ( projection->open )
        wxWinUIEndMenuProjection(projection);

    const auto found =
        std::find(state->projections.begin(),
                  state->projections.end(),
                  projection);
    if ( found != state->projections.end() )
        state->projections.erase(found);

    wxWinUIRevokeMenuProjectionLifecycle(projection);
    projection->items = nullptr;
    projection->lifecycleOwner = nullptr;
    projection->sentinel = nullptr;
    projection->parent.reset();
    projection->menu.Release();
}

void wxWinUIClearProjectionChildren(
    const std::shared_ptr<wxWinUIMenuProjectionState>& state,
    const std::shared_ptr<wxWinUIMenuProjection>& projection)
{
    if ( !state || !projection )
        return;

    const auto children = projection->children;
    for ( auto it = children.rbegin(); it != children.rend(); ++it )
        wxWinUIEraseProjectionTree(state, *it);
    projection->children.clear();
}

void wxWinUIStopSubmenuMonitor(
    const std::shared_ptr<wxWinUIMenuProjectionState>& state)
{
    if ( !state )
        return;

    // Publish the terminal generation before crossing either ABI boundary.
    // A Tick already queued by Stop()/revocation then observes inert state.
    const MUXD::DispatcherQueueTimer timer = state->submenuMonitor;
    const winrt::event_token token = state->submenuMonitorToken;
    state->submenuMonitor = nullptr;
    state->submenuMonitorToken = {};
    if ( ++state->submenuMonitorGeneration == 0 )
        ++state->submenuMonitorGeneration;

    if ( !timer )
        return;

    try
    {
        timer.Stop();
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI submenu monitor stop", e);
    }

    try
    {
        if ( token.value )
            timer.Tick(token);
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI submenu monitor revocation", e);
    }
}

void wxWinUIRunSubmenuMonitor(
    const std::weak_ptr<wxWinUIMenuProjectionState>& weakState,
    unsigned long long generation)
{
    const auto state = weakState.lock();
    if ( !state || state->submenuMonitorGeneration != generation )
        return;

    if ( !state->active || !state->menuBarMode || state->openMenus == 0 )
    {
        wxWinUIStopSubmenuMonitor(state);
        return;
    }

    // The sentinel remains the immediate path when WinUI realizes it. Some
    // MenuFlyoutSubItem presenters virtualize that zero-height item for their
    // entire lifetime, however, while their real automation provider still
    // exposes the exact physical expand/collapse state. Sample those peers
    // only while a menu-bar chain is live. Two collapsed samples filter the
    // transient Collapsed state observed during popup-chain hand-off.
    const auto projections = state->projections;
    for ( const auto& projection : projections )
    {
        if ( !state->active ||
                state->submenuMonitorGeneration != generation )
        {
            return;
        }
        if ( !projection || projection->state.lock() != state )
        {
            continue;
        }

        const bool isTop =
            !!projection->lifecycleOwner.try_as<MUXC::MenuBarItem>();
        const bool isSubmenu =
            !!projection->lifecycleOwner.try_as<MUXC::MenuFlyoutSubItem>();
        if ( !isTop && !isSubmenu )
            continue;

        if ( isSubmenu )
        {
            const auto parent = projection->parent.lock();
            if ( !parent || !parent->open )
            {
                projection->physicalCollapsedSamples = 0;
                continue;
            }
        }

        bool expanded = false;
        if ( !wxWinUITryGetMenuExpanded(projection, &expanded) )
            continue;

        if ( expanded )
        {
            projection->physicalExpandedKnown = true;
            projection->physicalExpanded = true;
            projection->physicalCollapsedSamples = 0;
            if ( !projection->open )
                wxWinUIBeginMenuProjection(projection);
        }
        else if ( !projection->physicalExpandedKnown )
        {
            projection->physicalExpandedKnown = true;
            projection->physicalExpanded = false;
            projection->physicalCollapsedSamples = 0;
        }
        else if ( projection->physicalExpanded || projection->open )
        {
            // MenuBarItem can report Collapsed while WinUI hands presentation
            // to an expanded descendant flyout. Keep the logical ancestor
            // alive until no real child provider owns the popup chain.
            bool descendantExpanded = false;
            if ( isTop )
            {
                const auto children = projection->children;
                for ( const auto& child : children )
                {
                    bool childExpanded = false;
                    if ( child &&
                            (child->open ||
                             (wxWinUITryGetMenuExpanded(
                                  child, &childExpanded) &&
                              childExpanded)) )
                    {
                        descendantExpanded = true;
                        break;
                    }
                }
            }
            if ( descendantExpanded )
            {
                projection->physicalCollapsedSamples = 0;
                continue;
            }

            if ( ++projection->physicalCollapsedSamples >= 2 )
            {
                projection->physicalExpanded = false;
                projection->physicalCollapsedSamples = 0;
                if ( projection->open )
                    wxWinUIEndMenuProjection(projection);
            }
        }

        // OPEN/CLOSE handlers can destroy or replace the entire generation.
        if ( !state->active ||
                state->submenuMonitorGeneration != generation )
        {
            return;
        }
    }
}

void wxWinUIEnsureSubmenuMonitor(
    const std::shared_ptr<wxWinUIMenuProjectionState>& state,
    const MUX::FrameworkElement& dispatcherOwner)
{
    if ( !state || !state->active || !state->menuBarMode ||
            state->openMenus == 0 || state->submenuMonitor ||
            !dispatcherOwner )
    {
        return;
    }

    MUXD::DispatcherQueueTimer candidate{ nullptr };
    winrt::event_token candidateToken{};
    try
    {
        const MUXD::DispatcherQueue queue = dispatcherOwner.DispatcherQueue();
        if ( !queue )
            return;

        candidate = queue.CreateTimer();
        candidate.Interval(std::chrono::milliseconds(16));
        candidate.IsRepeating(true);

        if ( ++state->submenuMonitorGeneration == 0 )
            ++state->submenuMonitorGeneration;
        const unsigned long long generation =
            state->submenuMonitorGeneration;
        const std::weak_ptr<wxWinUIMenuProjectionState> weakState(state);
        candidateToken = candidate.Tick(
            [weakState, generation](
                const winrt::Windows::Foundation::IInspectable&,
                const winrt::Windows::Foundation::IInspectable&)
            {
                wxWinUIRunSubmenuMonitor(weakState, generation);
            });

        if ( !state->active || !state->menuBarMode ||
                state->openMenus == 0 || state->submenuMonitor ||
                state->submenuMonitorGeneration != generation )
        {
            candidate.Tick(candidateToken);
            return;
        }

        state->submenuMonitor = candidate;
        state->submenuMonitorToken = candidateToken;
        candidate.Start();
    }
    catch ( const winrt::hresult_error& e )
    {
        if ( candidate && candidate != state->submenuMonitor )
        {
            try
            {
                if ( candidateToken.value )
                    candidate.Tick(candidateToken);
            }
            catch ( const winrt::hresult_error& )
            {
            }
        }
        else if ( state->submenuMonitor == candidate )
        {
            wxWinUIStopSubmenuMonitor(state);
        }
        wxWinUILogException("WinUI submenu monitor creation", e);
    }
}

void wxWinUIBeginMenuProjection(
    const std::shared_ptr<wxWinUIMenuProjection>& projection)
{
    const std::shared_ptr<wxWinUIMenuProjectionState> state =
        projection ? projection->state.lock() : nullptr;
    wxWindow * const invoking = state ? state->invokingWindow.get() : nullptr;
    wxMenu * const menu = projection ? projection->menu.get() : nullptr;
    if ( !state || !state->active || !invoking || !menu )
        return;

    if ( projection->open )
        return;

    // Starting a new logical session invalidates callbacks left by the exact
    // projection's previous session. Loading while it is already open does
    // not cancel reconciliation: WinUI can reload a sentinel transiently in
    // the middle of a real collapse before the automation state settles.
    wxWinUICancelMenuProjectionReconciliation(projection);

    projection->open = true;
    projection->physicalExpandedKnown = true;
    projection->physicalExpanded = true;
    projection->physicalCollapsedSamples = 0;
    ++state->openMenus;

    if ( state->menuBarMode )
        wxWinUIEnsureSubmenuMonitor(state, projection->lifecycleOwner);

    // This must precede projection: handlers are allowed to add/remove items
    // and change every piece of presentation state for this very opening.
    invoking->DoSendMenuOpenCloseEvent(wxEVT_MENU_OPEN, menu);

    // The OPEN handler may replace the MenuBar, destroy the frame or delete
    // this menu. Resolve all weak references again before building peers.
    if ( !state->active || !state->invokingWindow ||
            !projection->menu || !projection->items )
    {
        return;
    }

    try
    {
        // A parent flyout may keep its presenter alive across openings. Drop
        // the previous descendant projection generation before replacing its
        // peer items, otherwise every reopen retains another XAML subtree and
        // FindProjection() resolves stale descendants.
        wxWinUIClearProjectionChildren(state, projection);

        // Ending a still-open descendant above sends wxEVT_MENU_CLOSE. Its
        // handler may replace the MenuBar, destroy the frame or delete this
        // menu, just like the OPEN handler above.
        if ( !state->active || !state->invokingWindow ||
                !projection->menu || !projection->items )
        {
            return;
        }

        wxWinUIPopulateMenuItems(
            projection->items,
            projection->menu.get(),
            state,
            1,
            projection);
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI menu projection after OPEN", e);
    }
}

void wxWinUIEndMenuProjection(
    const std::shared_ptr<wxWinUIMenuProjection>& projection)
{
    const std::shared_ptr<wxWinUIMenuProjectionState> state =
        projection ? projection->state.lock() : nullptr;
    if ( !state || !state->active || !projection->open )
        return;

    // A real parent collapse closes the whole popup chain. Emit the canonical
    // wx sequence from the deepest live submenu towards its parent even if
    // WinUI delivers the individual Unloaded callbacks in another order.
    const auto children = projection->children;
    for ( auto it = children.rbegin(); it != children.rend(); ++it )
        wxWinUIEndMenuProjection(*it);

    // A descendant CLOSE handler may replace/destroy the entire generation.
    if ( !state->active || !projection->open )
        return;

    projection->open = false;
    projection->physicalExpandedKnown = true;
    projection->physicalExpanded = false;
    projection->physicalCollapsedSamples = 0;

    // Keep openMenus non-zero while CLOSE handlers run. If they mutate the
    // menu and call Refresh(), the current XAML tree must not be replaced from
    // inside its own Unloaded callback.
    if ( wxWindow * const invoking = state->invokingWindow.get() )
    {
        if ( wxMenu * const menu = projection->menu.get() )
            invoking->DoSendMenuOpenCloseEvent(wxEVT_MENU_CLOSE, menu);
    }

    if ( !state->active )
        return;

    if ( state->openMenus )
        --state->openMenus;

    if ( state->menuBarMode && state->openMenus == 0 )
    {
        wxWinUIStopSubmenuMonitor(state);
        wxWinUIScheduleMenuBarActions(state);
    }
}

MUXCP::FlyoutBase wxWinUIFindMenuBarFlyout(
    const MUX::FrameworkElement& owner)
{
    if ( !owner || !owner.try_as<MUXC::MenuBarItem>() )
        return nullptr;

    std::vector<MUX::DependencyObject> pending;
    pending.push_back(owner);
    for ( size_t index = 0; index < pending.size(); ++index )
    {
        const MUX::DependencyObject current = pending[index];
        if ( const auto element = current.try_as<MUX::UIElement>() )
        {
            if ( const auto flyout = element.ContextFlyout() )
                return flyout;
        }

        const int count = MUXM::VisualTreeHelper::GetChildrenCount(current);
        for ( int child = 0; child < count; ++child )
        {
            pending.push_back(
                MUXM::VisualTreeHelper::GetChild(current, child));
        }
    }

    return nullptr;
}

void wxWinUIAttachMenuBarFlyoutLifecycle(
    const std::shared_ptr<wxWinUIMenuProjection>& projection)
{
    const std::shared_ptr<wxWinUIMenuProjectionState> state =
        projection ? projection->state.lock() : nullptr;
    if ( !projection || !state || !state->active ||
            !projection->lifecycleOwner )
    {
        return;
    }

    try
    {
        const MUXCP::FlyoutBase flyout =
            wxWinUIFindMenuBarFlyout(projection->lifecycleOwner);
        if ( !flyout )
            return;
        if ( flyout == projection->lifecycleFlyout )
            return;

        // Keep the owner's tokenized Loaded hook for its entire projection
        // lifetime: a template reload may replace the physical flyout.
        wxWinUIRevokeMenuProjectionFlyoutLifecycle(projection);
        if ( !state->active || projection->state.lock() != state )
            return;
        projection->lifecycleFlyout = flyout;

        const std::weak_ptr<wxWinUIMenuProjection> weakProjection(projection);
        projection->openingToken = flyout.Opening(
            [weakProjection](
                const winrt::Windows::Foundation::IInspectable&,
                const winrt::Windows::Foundation::IInspectable&)
            {
                if ( const auto projectionNow = weakProjection.lock() )
                    wxWinUIBeginMenuProjection(projectionNow);
            });
        projection->closedToken = flyout.Closed(
            [weakProjection](
                const winrt::Windows::Foundation::IInspectable&,
                const winrt::Windows::Foundation::IInspectable&)
            {
                if ( const auto projectionNow = weakProjection.lock() )
                    wxWinUIStartMenuProjectionReconciliation(
                        projectionNow,
                        wxWinUIMenuReconcileReason::FlyoutClosed);
            });
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI MenuBar flyout lifecycle hookup", e);
        wxWinUIRevokeMenuProjectionFlyoutLifecycle(projection);
    }
}

std::shared_ptr<wxWinUIMenuProjection>
wxWinUICreateMenuProjection(
    const wxWinUIMenuItems& items,
    wxMenu *menu,
    const std::shared_ptr<wxWinUIMenuProjectionState>& state,
    const MUX::FrameworkElement& lifecycleOwner,
    const std::shared_ptr<wxWinUIMenuProjection>& parentProjection = nullptr)
{
    auto projection = std::make_shared<wxWinUIMenuProjection>();
    projection->state = state;
    projection->menu = wxWeakRef<wxMenu>(menu);
    projection->items = items;
    projection->lifecycleOwner = lifecycleOwner;

    MUXC::MenuFlyoutItem sentinel;
    sentinel.Text(L"");
    sentinel.IsEnabled(false);
    sentinel.IsTabStop(false);
    sentinel.IsHitTestVisible(false);
    sentinel.Opacity(0.0);
    sentinel.FlowDirection(state->flowDirection);

    // MenuFlyoutSubItem doesn't expose its popup lifecycle. A Collapsed child
    // is never realized, so using one as the sentinel makes a real UIA
    // Expand() open an empty presenter without ever sending wxEVT_MENU_OPEN.
    // Keep this sentinel in the item collection, but make it take no layout
    // space and no input/access-key participation. It is an immediate
    // lifecycle path when realized; the physical-owner hooks and provider
    // monitor cover presenters which virtualize it.
    sentinel.Margin(MUX::Thickness{});
    sentinel.Padding(MUX::Thickness{});
    sentinel.Height(0.0);
    MUXA::AutomationProperties::SetAccessibilityView(
        sentinel, MUXAP::AccessibilityView::Raw);
    projection->sentinel = sentinel;

    const std::weak_ptr<wxWinUIMenuProjection> weakProjection(projection);
    sentinel.Loading(
        [weakProjection](const MUX::FrameworkElement&,
                         const winrt::Windows::Foundation::IInspectable&)
        {
            if ( const auto projectionNow = weakProjection.lock() )
            {
                wxWinUIAttachMenuBarFlyoutLifecycle(projectionNow);
                wxWinUIBeginMenuProjection(projectionNow);
            }
        });
    sentinel.Unloaded(
        [weakProjection](
            const winrt::Windows::Foundation::IInspectable&,
            const MUX::RoutedEventArgs&)
        {
            if ( const auto projectionNow = weakProjection.lock() )
            {
                wxWinUIStartMenuProjectionReconciliation(
                    projectionNow,
                    wxWinUIMenuReconcileReason::SentinelUnloaded);
            }
        });

    items.Append(sentinel);
    state->projections.push_back(projection);
    if ( parentProjection )
    {
        projection->parent = parentProjection;
        parentProjection->children.push_back(projection);
    }

    // A top-level MenuBarItem creates its real MenuBarItemFlyout while its
    // template is loaded, before any item in that flyout is necessarily
    // realized. Hook that physical flyout at the owner boundary: a zero-size
    // sentinel can be virtualized, so its Loading event is not an authority
    // for the first OPEN transition. Submenus have no public flyout boundary
    // and continue to use the sentinel lifecycle below.
    if ( lifecycleOwner.try_as<MUXC::MenuBarItem>() )
    {
        projection->ownerLoadedToken = lifecycleOwner.Loaded(
            [weakProjection](
                const winrt::Windows::Foundation::IInspectable&,
                const MUX::RoutedEventArgs&)
            {
                if ( const auto projectionNow = weakProjection.lock() )
                    wxWinUIAttachMenuBarFlyoutLifecycle(projectionNow);
            });
    }
    return projection;
}

void wxWinUIPopulateMenuItems(
    const wxWinUIMenuItems& items,
    wxMenu *menu,
    const std::shared_ptr<wxWinUIMenuProjectionState>& state,
    uint32_t retainedItems,
    const std::shared_ptr<wxWinUIMenuProjection>& parentProjection)
{
    while ( items.Size() > retainedItems )
        items.RemoveAtEnd();

    int radioGroup = 0;
    bool inRadioRun = false;
    const size_t count = menu->GetMenuItemCount();

    for ( size_t position = 0; position < count; ++position )
    {
        wxMenuItem * const item = menu->FindItemByPosition(position);
        if ( !item )
            continue;

        if ( item->IsSeparator() )
        {
            inRadioRun = false;
            MUXC::MenuFlyoutSeparator separator;
            separator.FlowDirection(state->flowDirection);
            items.Append(separator);
            continue;
        }

        if ( item->IsSubMenu() )
        {
            inRadioRun = false;
            MUXC::MenuFlyoutSubItem submenu;
            wxWinUIApplyMenuItemPresentation(
                submenu, item, *state);
            wxWinUIAttachMenuHighlight(
                submenu, state, menu, position, wxID_NONE);
            wxWinUICreateMenuProjection(
                submenu.Items(),
                item->GetSubMenu(),
                state,
                submenu,
                parentProjection);
            items.Append(submenu);
            continue;
        }

        MUXC::MenuFlyoutItemBase peer{ nullptr };
        const int id = item->GetId();

        if ( item->GetKind() == wxITEM_RADIO )
        {
            if ( !inRadioRun )
            {
                ++radioGroup;
                inRadioRun = true;
            }

            MUXC::RadioMenuFlyoutItem radio;
            radio.GroupName(wxWinUIToHString(
                wxString::Format("wxWinUIRadio_%p_%d",
                                 menu, radioGroup)));
            radio.IsChecked(item->IsChecked());
            peer = radio;

            const wxWeakRef<wxMenu> weakMenu(menu);
            radio.Loaded(
                [weakMenu, position, id](
                    const winrt::Windows::Foundation::IInspectable& sender,
                    const MUX::RoutedEventArgs&)
                {
                    const auto current =
                        sender.try_as<MUXC::RadioMenuFlyoutItem>();
                    wxMenuItem * const currentItem =
                        wxWinUIFindCurrentMenuItem(
                            weakMenu.get(), position, id);
                    if ( current && currentItem )
                        current.IsChecked(currentItem->IsChecked());
                });
        }
        else
        {
            inRadioRun = false;
            if ( item->IsCheckable() )
            {
                MUXC::ToggleMenuFlyoutItem toggle;
                toggle.IsChecked(item->IsChecked());
                peer = toggle;
            }
            else
            {
                peer = MUXC::MenuFlyoutItem();
            }
        }

        wxWinUIApplyMenuItemPresentation(peer, item, *state);
        wxWinUIAttachMenuHighlight(peer, state, menu, position, id);
        wxWinUIAttachMenuCommand(peer, state, menu, id);
        items.Append(peer);
    }
}

void wxWinUIApplyMenuFlowDirection(
    const wxWinUIMenuItems& items,
    MUX::FlowDirection direction)
{
    for ( uint32_t i = 0; i < items.Size(); ++i )
    {
        const MUXC::MenuFlyoutItemBase item = items.GetAt(i);
        if ( const auto element = item.try_as<MUX::FrameworkElement>() )
            element.FlowDirection(direction);
        if ( const auto submenu = item.try_as<MUXC::MenuFlyoutSubItem>() )
            wxWinUIApplyMenuFlowDirection(submenu.Items(), direction);
    }
}

void wxWinUICloseOutstandingSubmenus(
    const std::shared_ptr<wxWinUIMenuProjectionState>& state)
{
    // Closing a parent normally unloads every child sentinel. Explicitly close
    // anything still marked open before destroying a popup generation, and
    // let the idempotent callback ignore later Unloaded notifications.
    const auto projections = state->projections;
    for ( auto it = projections.rbegin(); it != projections.rend(); ++it )
    {
        if ( (*it)->open )
            wxWinUIEndMenuProjection(*it);
    }
}

// ----------------------------------------------------------------------------
// wxWinUIMenuBarWindow
// ----------------------------------------------------------------------------

class wxWinUIMenuBarWindow : public wxControl
{
public:
    bool Create(wxWindow *frame, wxMenuBar *menubar)
    {
        m_frame = frame;
        m_menubar = menubar;
        m_barHeight = frame->FromDIP(40);

        if ( !wxControl::Create(frame, wxID_ANY, wxPoint(0, 0),
                                wxSize(wxDefaultCoord, m_barHeight),
                                wxBORDER_NONE) )
        {
            return false;
        }

        if ( !m_host.Initialize(this) )
            return false;

        const wxWeakRef<wxWindow> stillAlive(this);
        if ( !BeginBuildTransaction() )
            return false;
        wxWinUIMenuBarWindow * const owner =
            static_cast<wxWinUIMenuBarWindow *>(stillAlive.get());
        if ( !owner || owner->m_frame != frame ||
             owner->m_menubar != menubar ||
             owner->m_destroyScheduled )
        {
            return false;
        }

        frame->Bind(
            wxEVT_SIZE, &wxWinUIMenuBarWindow::OnFrameSize, this);
        frame->Bind(
            wxEVT_DPI_CHANGED, &wxWinUIMenuBarWindow::OnDPIChanged, this);
        owner->Reposition();
        wxWinUIMenuBarWindow * const positionedOwner =
            static_cast<wxWinUIMenuBarWindow *>(stillAlive.get());
        return positionedOwner &&
               positionedOwner->m_frame == frame &&
               !positionedOwner->m_destroyScheduled;
    }

    ~wxWinUIMenuBarWindow() override
    {
        Deactivate();
        if ( m_frame )
        {
            m_frame->Unbind(
                wxEVT_SIZE, &wxWinUIMenuBarWindow::OnFrameSize, this);
            m_frame->Unbind(
                wxEVT_DPI_CHANGED,
                &wxWinUIMenuBarWindow::OnDPIChanged,
                this);
        }
    }

    void Retire()
    {
        if ( m_destroyScheduled )
            return;

        // Mark first: a CLOSE handler may synchronously call SetMenuBar()
        // again with this same child as the apparent old generation.
        m_destroyScheduled = true;
        const wxWeakRef<wxWindow> stillAlive(this);
        Deactivate();
        if ( !stillAlive )
            return;

        Hide();
        if ( !stillAlive )
            return;
        Disable();

        // wxWindowBase::Destroy() deletes non-TLW children synchronously.
        // Deferring it is essential because Retire() is often entered from a
        // XAML OPEN/CLOSE callback whose stack still belongs to this object.
        if ( wxTheApp )
        {
            wxTheApp->CallAfter(
                [stillAlive]()
                {
                    if ( wxWindow * const window = stillAlive.get() )
                        window->Destroy();
                });
        }
    }

    void Deactivate()
    {
        if ( m_deactivating )
            return;
        m_deactivating = true;

        const wxWeakRef<wxWindow> stillAlive(this);
        const auto state = m_state;
        if ( state )
        {
            if ( state->active )
                wxWinUICloseOutstandingSubmenus(state);

            // Invalidate the detached generation even if a CLOSE handler
            // destroyed this window. All peer callbacks only see this shared
            // state and will become inert immediately.
            state->active = false;
            state->pendingCommand.Clear();
            wxWinUIClearMenuProjectionState(state);

            if ( !stillAlive )
                return;

            // A CLOSE handler may already have replaced this window/state.
            if ( m_state == state )
            {
                m_state.reset();
                m_bar = nullptr;
            }
        }
        else
        {
            m_bar = nullptr;
        }

        m_deactivating = false;
    }

    void Rebuild()
    {
        // A setter reached from SetContent() belongs to the transaction which
        // is already running. It may request another pass, but must never
        // replenish that transaction's finite convergence budget.
        if ( m_buildInProgress )
        {
            m_buildPending = true;
            return;
        }

        if ( m_state && m_state->active && m_state->openMenus )
        {
            m_state->rebuildPending = true;
            return;
        }

        BeginBuildTransaction();
    }

    void RunDeferredActions(
        const std::shared_ptr<wxWinUIMenuProjectionState>& state)
    {
        if ( !state || !state->active || state != m_state )
            return;

        state->deferredActionsScheduled = false;
        if ( state->openMenus )
            return;

        const wxWinUIPendingMenuCommand command = state->pendingCommand;
        state->pendingCommand.Clear();

        if ( command.IsOk() &&
                state->invokingWindow &&
                state->expectedMenuBar &&
                m_menubar == state->expectedMenuBar.get() )
        {
            wxFrame * const frame =
                wxDynamicCast(state->invokingWindow.get(), wxFrame);
            if ( frame && frame->GetMenuBar() == state->expectedMenuBar.get() )
            {
                // MSWCommand is the single canonical implementation for
                // normal/check/radio semantics and menu->parent->bar->frame
                // routing. The exact owner disambiguates duplicate IDs in
                // different branches.
                command.owner->MSWCommand(
                    0, static_cast<WXWORD>(command.id));
            }
        }

        // The command can synchronously destroy/rebuild this bar.
        if ( !state->active || state != m_state )
            return;

        if ( state->rebuildPending )
        {
            state->rebuildPending = false;
            BeginBuildTransaction();
        }
    }

    bool TestOpenMenu(const size_t *menuPath, size_t menuPathLength)
    {
        const wxWeakRef<wxWindow> stillAlive(this);
        const auto expectedState = m_state;
        wxMenuBar * const expectedMenuBar =
            expectedState ? expectedState->expectedMenuBar.get() : nullptr;
        if ( !menuPath || !menuPathLength || !expectedState ||
                !expectedState->active || !expectedMenuBar ||
                m_menubar != expectedMenuBar ||
                menuPath[0] >= expectedMenuBar->GetMenuCount() )
        {
            return false;
        }

        wxWeakRef<wxMenu> currentMenu(
            expectedMenuBar->GetMenu(menuPath[0]));
        for ( size_t level = 0; level < menuPathLength; ++level )
        {
            wxWinUIMenuBarWindow * const owner =
                static_cast<wxWinUIMenuBarWindow *>(stillAlive.get());
            wxMenu * const menu = currentMenu.get();
            if ( !owner || owner->m_destroyScheduled ||
                    !owner->m_frame ||
                    owner->m_state != expectedState ||
                    !expectedState->active ||
                    expectedState->expectedMenuBar.get() != expectedMenuBar ||
                    owner->m_menubar != expectedMenuBar || !menu ||
                    owner->ResolveMenuPath(menuPath, level + 1) != menu ||
                    wxWinUITLWHostIsDestroyScheduled(owner->m_frame) )
            {
                return false;
            }

            const auto projection = owner->FindProjection(menu);
            if ( !projection ||
                    projection->state.lock() != expectedState ||
                    projection->menu.get() != menu )
            {
                return false;
            }

            wxWinUIBeginMenuProjection(projection);

            // OPEN is an arbitrary application callback: it may replace the
            // bar, delete this menu, or destroy either the child or its TLW.
            // Re-resolve every weak identity before reading the next path
            // component; no pointer captured above crosses this boundary.
            wxWinUIMenuBarWindow * const currentOwner =
                static_cast<wxWinUIMenuBarWindow *>(stillAlive.get());
            wxMenu * const menuAfterOpen = currentMenu.get();
            if ( !currentOwner || currentOwner->m_destroyScheduled ||
                    !currentOwner->m_frame ||
                    currentOwner->m_state != expectedState ||
                    !expectedState->active ||
                    expectedState->expectedMenuBar.get() != expectedMenuBar ||
                    currentOwner->m_menubar != expectedMenuBar ||
                    !menuAfterOpen || !projection->open ||
                    projection->state.lock() != expectedState ||
                    projection->menu.get() != menuAfterOpen ||
                    currentOwner->ResolveMenuPath(
                        menuPath, level + 1) != menuAfterOpen ||
                    currentOwner->FindProjection(menuAfterOpen) != projection ||
                    wxWinUITLWHostIsDestroyScheduled(
                        currentOwner->m_frame) )
            {
                return false;
            }

            if ( level + 1 < menuPathLength )
            {
                wxMenuItem * const submenuItem =
                    menuAfterOpen->FindItemByPosition(
                        menuPath[level + 1]);
                if ( !submenuItem || !submenuItem->IsSubMenu() )
                    return false;
                currentMenu =
                    wxWeakRef<wxMenu>(submenuItem->GetSubMenu());
            }
        }

        return true;
    }

    bool TestCloseMenu(const size_t *menuPath, size_t menuPathLength)
    {
        std::vector<std::shared_ptr<wxWinUIMenuProjection>> projections;
        if ( !ResolveProjectionPath(
                menuPath, menuPathLength, &projections) )
        {
            return false;
        }

        for ( auto it = projections.rbegin(); it != projections.rend(); ++it )
            wxWinUIEndMenuProjection(*it);
        return true;
    }

    bool TestSnapshot(const size_t *menuPath,
                      size_t menuPathLength,
                      size_t itemPosition,
                      wxWinUIMenuItemSnapshot *snapshot)
    {
        if ( !snapshot )
            return false;

        std::vector<std::shared_ptr<wxWinUIMenuProjection>> projections;
        if ( !ResolveProjectionPath(
                menuPath, menuPathLength, &projections) ||
                projections.empty() )
        {
            return false;
        }

        const auto& projection = projections.back();
        wxMenu * const menu = projection->menu.get();
        if ( !menu || itemPosition >= menu->GetMenuItemCount() ||
                !projection->items ||
                itemPosition + 1 >= projection->items.Size() )
        {
            return false;
        }

        const MUXC::MenuFlyoutItemBase peer =
            projection->items.GetAt(
                static_cast<uint32_t>(itemPosition + 1));
        wxMenuItem * const item = menu->FindItemByPosition(itemPosition);
        if ( !peer || !item || item->IsSeparator() )
            return false;

        wxWinUIMenuItemSnapshot result;
        result.accessKey = wxString(peer.AccessKey().c_str());
        result.enabled = peer.IsEnabled();
        result.flowDirection =
            static_cast<wxWinUIMenuFlowDirection>(
                static_cast<int>(peer.FlowDirection()));
        result.radio =
            static_cast<bool>(peer.try_as<MUXC::RadioMenuFlyoutItem>());
        result.submenu =
            static_cast<bool>(peer.try_as<MUXC::MenuFlyoutSubItem>());

        MUXC::IconElement icon{ nullptr };
        if ( const auto normal = peer.try_as<MUXC::MenuFlyoutItem>() )
        {
            result.label = wxString(normal.Text().c_str());
            result.accelerator = wxString(
                normal.KeyboardAcceleratorTextOverride().c_str());
            icon = normal.Icon();
        }
        else if ( const auto submenu =
                    peer.try_as<MUXC::MenuFlyoutSubItem>() )
        {
            result.label = wxString(submenu.Text().c_str());
            icon = submenu.Icon();
        }

        result.hasIcon = static_cast<bool>(icon);
        if ( const auto imageIcon = icon.try_as<MUXC::ImageIcon>() )
        {
            if ( const auto source =
                     imageIcon.Source()
                         .try_as<
                             winrt::Microsoft::UI::Xaml::Media::Imaging::
                                 WriteableBitmap>() )
            {
                result.iconPixelWidth = source.PixelWidth();
                result.iconPixelHeight = source.PixelHeight();
            }
        }

        if ( const auto radio =
                peer.try_as<MUXC::RadioMenuFlyoutItem>() )
        {
            result.checked = radio.IsChecked();
        }
        else if ( const auto toggle =
                    peer.try_as<MUXC::ToggleMenuFlyoutItem>() )
        {
            result.checked = toggle.IsChecked();
        }

        *snapshot = result;
        return true;
    }

    bool TestInvoke(const size_t *menuPath,
                    size_t menuPathLength,
                    size_t itemPosition)
    {
        wxMenu * const menu = ResolveMenuPath(
            menuPath, menuPathLength);
        if ( !menu || itemPosition >= menu->GetMenuItemCount() )
            return false;

        wxMenuItem * const item = menu->FindItemByPosition(itemPosition);
        if ( !item || item->IsSeparator() || item->IsSubMenu() )
            return false;

        wxWinUIRecordMenuCommand(
            m_state, wxWeakRef<wxMenu>(menu), item->GetId());
        return true;
    }

    bool TestSetExpanded(const size_t *menuPath,
                         size_t menuPathLength,
                         bool expanded)
    {
        const MUX::FrameworkElement element =
            ResolveAutomationElement(menuPath, menuPathLength);
        if ( !element )
            return false;

        try
        {
            const auto provider =
                wxWinUIGetMenuExpandCollapseProvider(element);
            if ( !provider )
                return false;

            if ( expanded )
                provider.Expand();
            else
                provider.Collapse();
            return true;
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException(
                "WinUI MenuBar automation expand/collapse", e);
            return false;
        }
    }

    bool TestGetExpanded(const size_t *menuPath,
                         size_t menuPathLength,
                         bool *expanded) const
    {
        if ( !expanded )
            return false;

        const MUX::FrameworkElement element =
            ResolveAutomationElement(menuPath, menuPathLength);
        if ( !element )
            return false;

        try
        {
            const auto provider =
                wxWinUIGetMenuExpandCollapseProvider(element);
            if ( !provider )
                return false;

            *expanded =
                provider.ExpandCollapseState() ==
                MUXA::ExpandCollapseState::Expanded;
            return true;
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException(
                "WinUI MenuBar automation expanded snapshot", e);
            return false;
        }
    }

    size_t TestProjectionCount() const
    {
        return m_state ? m_state->projections.size() : 0;
    }

    bool TestSentinelContract(const size_t *menuPath,
                              size_t menuPathLength) const
    {
        std::vector<std::shared_ptr<wxWinUIMenuProjection>> projections;
        if ( !ResolveProjectionPath(
                menuPath, menuPathLength, &projections) ||
                projections.empty() || !projections.back()->sentinel )
        {
            return false;
        }

        const MUXC::MenuFlyoutItem sentinel =
            projections.back()->sentinel;
        return !sentinel.IsEnabled() &&
               !sentinel.IsTabStop() &&
               !sentinel.IsHitTestVisible() &&
               sentinel.Opacity() == 0.0 &&
               sentinel.Height() == 0.0 &&
               MUXA::AutomationProperties::GetAccessibilityView(sentinel) ==
                   MUXAP::AccessibilityView::Raw;
    }

    bool TestTopEnabled(size_t topIndex, bool *enabled) const
    {
        if ( !enabled || !m_bar || topIndex >= m_bar.Items().Size() )
            return false;

        try
        {
            *enabled = m_bar.Items().GetAt(
                static_cast<uint32_t>(topIndex)).IsEnabled();
            return true;
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException(
                "WinUI MenuBar top enabled snapshot", e);
            return false;
        }
    }

    bool TestTopLabel(size_t topIndex,
                      wxString *label,
                      wxString *accessKey) const
    {
        if ( !label || !accessKey || !m_bar ||
             topIndex >= m_bar.Items().Size() )
        {
            return false;
        }

        try
        {
            const MUXC::MenuBarItem item = m_bar.Items().GetAt(
                static_cast<uint32_t>(topIndex));
            *label = wxString(item.Title().c_str());
            *accessKey = wxString(item.AccessKey().c_str());
            return true;
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException(
                "WinUI MenuBar top label snapshot", e);
            return false;
        }
    }

    int TestGetBarHeight() const { return m_barHeight; }

    bool TestGetFlowDirection(
        wxWinUIMenuFlowDirection *barDirection,
        wxWinUIMenuFlowDirection *firstTopDirection) const
    {
        if ( !barDirection || !firstTopDirection || !m_bar ||
             m_bar.Items().Size() == 0 )
        {
            return false;
        }

        try
        {
            *barDirection =
                static_cast<wxWinUIMenuFlowDirection>(
                    static_cast<int>(m_bar.FlowDirection()));
            *firstTopDirection =
                static_cast<wxWinUIMenuFlowDirection>(
                    static_cast<int>(
                        m_bar.Items().GetAt(0).FlowDirection()));
            return true;
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException(
                "WinUI MenuBar flow-direction snapshot", e);
            return false;
        }
    }

    bool TestRefreshForScale(double scale)
    {
        if ( scale <= 0.0 || !m_menubar ||
             (m_state && m_state->active && m_state->openMenus) )
        {
            return false;
        }

        m_bitmapScaleForTesting = scale;
        return BeginBuildTransaction();
    }

    bool TestBuildQuarantined() const { return m_buildQuarantined; }

private:
    MUX::FrameworkElement ResolveAutomationElement(
        const size_t *menuPath,
        size_t menuPathLength) const
    {
        if ( !menuPath || !menuPathLength || !m_bar ||
                menuPath[0] >= m_bar.Items().Size() )
        {
            return nullptr;
        }

        if ( menuPathLength == 1 )
        {
            return m_bar.Items().GetAt(
                static_cast<uint32_t>(menuPath[0]));
        }

        std::vector<std::shared_ptr<wxWinUIMenuProjection>> parents;
        if ( !ResolveProjectionPath(
                menuPath, menuPathLength - 1, &parents) ||
                parents.empty() )
        {
            return nullptr;
        }

        const auto& parent = parents.back();
        const size_t itemPosition = menuPath[menuPathLength - 1];
        if ( !parent->items ||
                itemPosition + 1 >= parent->items.Size() )
        {
            return nullptr;
        }

        return parent->items.GetAt(
            static_cast<uint32_t>(itemPosition + 1))
            .try_as<MUX::FrameworkElement>();
    }

    wxMenu *ResolveMenuPath(const size_t *menuPath,
                            size_t menuPathLength) const
    {
        if ( !menuPath || !menuPathLength || !m_menubar ||
                menuPath[0] >= m_menubar->GetMenuCount() )
        {
            return nullptr;
        }

        wxMenu *menu = m_menubar->GetMenu(menuPath[0]);
        for ( size_t level = 1; level < menuPathLength; ++level )
        {
            if ( !menu || menuPath[level] >= menu->GetMenuItemCount() )
                return nullptr;

            wxMenuItem * const item =
                menu->FindItemByPosition(menuPath[level]);
            if ( !item || !item->IsSubMenu() )
                return nullptr;
            menu = item->GetSubMenu();
        }

        return menu;
    }

    std::shared_ptr<wxWinUIMenuProjection>
    FindProjection(wxMenu *menu) const
    {
        if ( !m_state || !menu )
            return nullptr;

        for ( const auto& projection : m_state->projections )
        {
            if ( projection && projection->menu.get() == menu )
                return projection;
        }
        return nullptr;
    }

    bool ResolveProjectionPath(
        const size_t *menuPath,
        size_t menuPathLength,
        std::vector<std::shared_ptr<wxWinUIMenuProjection>> *projections)
        const
    {
        if ( !projections )
            return false;
        projections->clear();

        if ( !menuPath || !menuPathLength || !m_menubar ||
                menuPath[0] >= m_menubar->GetMenuCount() )
        {
            return false;
        }

        wxMenu *menu = m_menubar->GetMenu(menuPath[0]);
        for ( size_t level = 0; level < menuPathLength; ++level )
        {
            const auto projection = FindProjection(menu);
            if ( !projection )
                return false;
            projections->push_back(projection);

            if ( level + 1 < menuPathLength )
            {
                if ( !menu ||
                        menuPath[level + 1] >=
                            menu->GetMenuItemCount() )
                {
                    return false;
                }

                wxMenuItem * const item =
                    menu->FindItemByPosition(menuPath[level + 1]);
                if ( !item || !item->IsSubMenu() )
                    return false;
                menu = item->GetSubMenu();
            }
        }

        return true;
    }

    void OnFrameSize(wxSizeEvent& event)
    {
        Reposition();
        event.Skip();
    }

    void OnDPIChanged(wxDPIChangedEvent& event)
    {
        if ( m_frame )
            m_barHeight = m_frame->FromDIP(40);
        Reposition();
        // Rebuild image sources so wxBitmapBundle-backed menu icons are
        // materialized for the new monitor DPI. Rebuild() safely defers while
        // a menu presenter is open.
        Rebuild();
        event.Skip();
    }

    bool MSWOnEffectiveLayoutDirectionChanged() override
    {
        if ( !m_bar || !m_state || !m_state->active )
            return true;

        const MUX::FlowDirection direction =
            GetLayoutDirection() == wxLayout_RightToLeft
                ? MUX::FlowDirection::RightToLeft
                : MUX::FlowDirection::LeftToRight;
        const auto expectedState = m_state;
        const MUXC::MenuBar expectedBar = m_bar;
        const wxWeakRef<wxWindow> alive(this);
        // Publish the target before the first XAML write. A Loading callback
        // can populate a detached presenter synchronously from that write,
        // and it must materialize the new direction even if its top item was
        // already visited by this transaction.
        expectedState->flowDirection = direction;

        try
        {
            expectedBar.FlowDirection(direction);
            wxWinUIMenuBarWindow *owner =
                static_cast<wxWinUIMenuBarWindow *>(alive.get());
            if ( !owner || owner->m_destroyScheduled ||
                 owner->m_state != expectedState ||
                 owner->m_bar != expectedBar ||
                 !expectedState->active )
            {
                return false;
            }

            const uint32_t count = expectedBar.Items().Size();
            for ( uint32_t i = 0; i < count; ++i )
            {
                const MUXC::MenuBarItem top =
                    expectedBar.Items().GetAt(i);
                top.FlowDirection(direction);
                wxWinUIApplyMenuFlowDirection(
                    top.Items(), direction);

                owner = static_cast<wxWinUIMenuBarWindow *>(
                    alive.get());
                if ( !owner || owner->m_destroyScheduled ||
                     owner->m_state != expectedState ||
                     owner->m_bar != expectedBar ||
                     !expectedState->active )
                {
                    return false;
                }
            }
            return true;
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException(
                "WinUI MenuBar flow-direction update", e);
            return false;
        }
    }

    void Reposition()
    {
        if ( !m_frame )
            return;

        HWND hwnd = static_cast<HWND>(GetHWND());
        if ( !hwnd )
            return;

        const int width = m_frame->GetClientSize().x;
        if ( !::SetWindowPos(hwnd, nullptr, 0, 0, width, m_barHeight,
                             SWP_NOZORDER | SWP_NOACTIVATE) )
        {
            wxLogLastError("SetWindowPos(WinUI MenuBar)");
        }
    }

    bool BeginBuildTransaction()
    {
        if ( m_buildInProgress )
        {
            m_buildPending = true;
            return true;
        }

        // Starting a transaction invalidates any older queued continuation.
        // Its callback checks this epoch before touching even the scheduling
        // flag, so it cannot cancel a newer transaction's continuation.
        if ( ++m_buildEpoch == 0 )
            ++m_buildEpoch;
        m_buildPassesRemaining = MaxBuildPassesPerTransaction;
        m_buildQuarantined = false;
        m_buildRetryScheduled = false;
        return ContinueBuildTransaction(m_buildEpoch);
    }

    bool ContinueBuildTransaction(unsigned long long expectedEpoch)
    {
        if ( expectedEpoch != m_buildEpoch || m_buildQuarantined )
            return true;

        if ( !m_frame || !m_menubar )
            return false;

        // Rebuild() defers while a menu is open. Keep this invariant explicit:
        // replacing a live presenter from inside OPEN/CLOSE is not safe.
        if ( m_state && m_state->active && m_state->openMenus )
            return false;

        wxASSERT_MSG(!m_buildInProgress,
                     "a build continuation cannot overlap its transaction");
        m_buildInProgress = true;

        const wxWeakRef<wxWindow> stillAlive(this);
        const auto finish =
            [&](bool result)
            {
                wxWinUIMenuBarWindow * const owner =
                    static_cast<wxWinUIMenuBarWindow *>(stillAlive.get());
                if ( owner && owner->m_buildEpoch == expectedEpoch )
                    owner->m_buildInProgress = false;
                return result;
            };
        const auto retireCandidate =
            [](const std::shared_ptr<wxWinUIMenuProjectionState>& state)
            {
                if ( !state )
                    return;
                state->active = false;
                state->pendingCommand.Clear();
                wxWinUIClearMenuProjectionState(state);
            };

        unsigned passesThisDispatch = 0;
        while ( passesThisDispatch < MaxBuildPassesPerDispatch &&
                m_buildPassesRemaining )
        {
            ++passesThisDispatch;
            --m_buildPassesRemaining;

            wxWinUIMenuBarWindow *owner =
                static_cast<wxWinUIMenuBarWindow *>(stillAlive.get());
            if ( !owner || owner->m_destroyScheduled ||
                 owner->m_buildEpoch != expectedEpoch ||
                 !owner->m_frame || !owner->m_menubar )
            {
                return finish(false);
            }

            if ( owner->m_state && owner->m_state->active &&
                 owner->m_state->openMenus )
            {
                owner->m_state->rebuildPending = true;
                return finish(true);
            }

            owner->m_buildPending = false;
            auto state =
                std::make_shared<wxWinUIMenuProjectionState>();
            state->menuBarMode = true;
            state->bitmapScaleForTesting =
                owner->m_bitmapScaleForTesting;
            state->flowDirection =
                owner->GetLayoutDirection() ==
                        wxLayout_RightToLeft
                    ? MUX::FlowDirection::RightToLeft
                    : MUX::FlowDirection::LeftToRight;
            state->invokingWindow =
                wxWeakRef<wxWindow>(owner->m_frame);
            state->expectedMenuBar =
                wxWeakRef<wxMenuBar>(owner->m_menubar);
            state->menuBarWindow =
                wxWeakRef<wxWindow>(owner);

            try
            {
                MUXC::MenuBar bar;
                bar.FlowDirection(state->flowDirection);
                const size_t count =
                    owner->m_menubar->GetMenuCount();
                for ( size_t i = 0; i < count; ++i )
                {
                    wxMenu * const menu =
                        owner->m_menubar->GetMenu(i);
                    if ( !menu )
                        continue;

                    const wxWinUIMenuLabel label =
                        wxWinUIParseMenuLabel(
                            owner->m_menubar->GetMenuLabel(i));

                    MUXC::MenuBarItem barItem;
                    barItem.FlowDirection(
                        state->flowDirection);
                    barItem.Title(wxWinUIToHString(label.text));
                    barItem.AccessKey(
                        wxWinUIToHString(label.accessKey));
                    barItem.IsEnabled(
                        owner->m_menubar->IsEnabledTop(i));

                    // Only the inert lifecycle sentinel exists until this
                    // exact menu is opened. Its Loading callback sends OPEN,
                    // then builds from the possibly mutated wxMenu.
                    wxWinUICreateMenuProjection(
                        barItem.Items(), menu, state, barItem);
                    bar.Items().Append(barItem);
                }

                // The shared host is transactional, but attaching the
                // candidate can synchronously enter Rebuild(). The nested
                // call only records m_buildPending; this outer transaction
                // publishes its accepted tree, then projects the newest menu
                // model in the next bounded pass.
                if ( !owner->m_host.SetContent(bar) )
                {
                    retireCandidate(state);
                    wxLogWarning(
                        "WinUI MenuBar content replacement failed.");
                    return finish(false);
                }

                owner =
                    static_cast<wxWinUIMenuBarWindow *>(
                        stillAlive.get());
                if ( !owner || owner->m_destroyScheduled ||
                     owner->m_buildEpoch != expectedEpoch )
                {
                    retireCandidate(state);
                    return finish(false);
                }

                const auto previousState = owner->m_state;
                owner->m_bar = bar;
                owner->m_state = state;
                retireCandidate(previousState);

                owner =
                    static_cast<wxWinUIMenuBarWindow *>(
                        stillAlive.get());
                if ( !owner || owner->m_destroyScheduled ||
                     owner->m_buildEpoch != expectedEpoch ||
                     owner->m_state != state )
                {
                    retireCandidate(state);
                    return finish(false);
                }

                if ( !owner->m_buildPending )
                    return finish(true);
            }
            catch ( const winrt::hresult_error& e )
            {
                retireCandidate(state);
                wxWinUILogException("WinUI MenuBar creation", e);
                return finish(false);
            }
        }

        wxWinUIMenuBarWindow * const owner =
            static_cast<wxWinUIMenuBarWindow *>(stillAlive.get());
        if ( !owner || owner->m_destroyScheduled ||
             owner->m_buildEpoch != expectedEpoch )
        {
            return finish(false);
        }

        owner->m_buildInProgress = false;
        if ( !owner->m_buildPassesRemaining )
        {
            // A continuously reentrant projection is quarantined instead of
            // being allowed to monopolize the dispatcher forever. Only a
            // later external Rebuild()/Create transaction (or the explicit
            // post-close rebuild path) can replenish this budget.
            owner->m_buildPending = false;
            owner->m_buildQuarantined = true;
            owner->m_buildRetryScheduled = false;
            wxLogWarning(
                "WinUI MenuBar model kept changing during projection; "
                "quarantining this request until the next external "
                "mutation.");
            return true;
        }

        if ( !owner->m_buildRetryScheduled )
        {
            owner->m_buildRetryScheduled = true;
            owner->CallAfter(
                [stillAlive, expectedEpoch]()
                {
                    wxWinUIMenuBarWindow * const current =
                        static_cast<wxWinUIMenuBarWindow *>(
                            stillAlive.get());
                    if ( !current || current->m_destroyScheduled ||
                         current->m_buildEpoch != expectedEpoch )
                    {
                        return;
                    }

                    current->m_buildRetryScheduled = false;
                    current->ContinueBuildTransaction(expectedEpoch);
                });
        }
        return true;
    }

    static constexpr unsigned MaxBuildPassesPerDispatch = 8;
    static constexpr unsigned MaxBuildPassesPerTransaction = 16;

    wxWinUIControlHost m_host;
    wxWindow *m_frame = nullptr;
    wxMenuBar *m_menubar = nullptr;
    int m_barHeight = 0;
    MUXC::MenuBar m_bar{ nullptr };
    std::shared_ptr<wxWinUIMenuProjectionState> m_state;
    bool m_buildInProgress = false;
    bool m_buildPending = false;
    bool m_buildRetryScheduled = false;
    bool m_buildQuarantined = false;
    unsigned m_buildPassesRemaining = 0;
    unsigned long long m_buildEpoch = 0;
    double m_bitmapScaleForTesting = 0.0;
    bool m_deactivating = false;
    bool m_destroyScheduled = false;
};

void wxWinUIScheduleMenuBarActions(
    const std::shared_ptr<wxWinUIMenuProjectionState>& state)
{
    if ( !state || !state->active || !state->menuBarMode ||
            state->deferredActionsScheduled )
    {
        return;
    }

    state->deferredActionsScheduled = true;
    const std::weak_ptr<wxWinUIMenuProjectionState> weakState(state);
    wxTheApp->CallAfter(
        [weakState]()
        {
            const auto stateNow = weakState.lock();
            if ( !stateNow || !stateNow->active )
                return;

            wxWindow * const barWindow = stateNow->menuBarWindow.get();
            if ( !barWindow )
                return;

            static_cast<wxWinUIMenuBarWindow *>(barWindow)
                ->RunDeferredActions(stateNow);
        });
}

// ----------------------------------------------------------------------------
// Popup loop helpers
// ----------------------------------------------------------------------------

struct wxWinUIPopupLoopState
{
    bool closed = false;
    bool aborted = false;
    wxEventLoop *loop = nullptr;
    wxWeakRef<wxWindow> invokingWindow;
    wxWeakRef<wxWindow> topLevelWindow;
    MUXD::DispatcherQueue queue{ nullptr };

    void ExitLoop()
    {
        if ( loop && loop->IsRunning() )
            loop->Exit();
    }
};

class wxWinUIPopupWatchdog final : public wxTimer
{
public:
    explicit wxWinUIPopupWatchdog(
        const std::shared_ptr<wxWinUIPopupLoopState>& state)
        : m_state(state)
    {
    }

    void Notify() override
    {
        const auto state = m_state.lock();
        if ( !state )
        {
            Stop();
            return;
        }

        bool dispatcherAlive = false;
        try
        {
            dispatcherAlive =
                state->queue &&
                state->queue.TryEnqueue([]() {});
        }
        catch ( const winrt::hresult_error& )
        {
        }

        if ( !state->invokingWindow || !state->topLevelWindow ||
                !dispatcherAlive )
        {
            state->aborted = true;
            state->ExitLoop();
            Stop();
        }
    }

private:
    std::weak_ptr<wxWinUIPopupLoopState> m_state;
};

struct wxWinUIDrainState
{
    bool drained = false;
    wxEventLoop *loop = nullptr;

    void ExitLoop()
    {
        if ( loop && loop->IsRunning() )
            loop->Exit();
    }
};

class wxWinUIDrainTimeout final : public wxTimer
{
public:
    explicit wxWinUIDrainTimeout(
        const std::shared_ptr<wxWinUIDrainState>& state)
        : m_state(state)
    {
    }

    void Notify() override
    {
        if ( const auto state = m_state.lock() )
            state->ExitLoop();
    }

private:
    std::weak_ptr<wxWinUIDrainState> m_state;
};

bool gs_rejectNextPopupDrainForTesting = false;
bool gs_failNextDrainTimerForTesting = false;
bool gs_failNextPopupWatchdogForTesting = false;

bool wxWinUIDrainDispatcher(const MUXD::DispatcherQueue& queue)
{
    if ( !queue )
        return false;

    if ( gs_rejectNextPopupDrainForTesting )
    {
        gs_rejectNextPopupDrainForTesting = false;
        return false;
    }

    auto state = std::make_shared<wxWinUIDrainState>();
    const std::weak_ptr<wxWinUIDrainState> weakState(state);

    bool accepted = false;
    try
    {
        accepted = queue.TryEnqueue(
            [weakState]()
            {
                if ( const auto stateNow = weakState.lock() )
                {
                    stateNow->drained = true;
                    stateNow->ExitLoop();
                }
            });
    }
    catch ( const winrt::hresult_error& )
    {
    }

    // A rejected queue must never enter a nested loop.
    if ( !accepted || state->drained )
        return state->drained;

    wxEventLoop loop;
    state->loop = &loop;
    wxWinUIDrainTimeout timeout(state);
    const bool timerStarted =
        !std::exchange(gs_failNextDrainTimerForTesting, false) &&
        timeout.StartOnce(250);
    if ( !timerStarted )
    {
        state->loop = nullptr;
        return false;
    }
    loop.Run();
    timeout.Stop();
    state->loop = nullptr;
    return state->drained;
}

class wxWinUIMenuCloseGuard
{
public:
    wxWinUIMenuCloseGuard(wxWindow *invoking, wxMenu *menu)
        : m_invoking(invoking),
          m_menu(menu)
    {
    }

    ~wxWinUIMenuCloseGuard() { Close(); }

    void Close(wxWindow *knownLiveInvoking = nullptr)
    {
        if ( !m_open )
            return;
        m_open = false;

        wxWindow * const invoking =
            knownLiveInvoking ? knownLiveInvoking : m_invoking.get();
        if ( invoking && m_menu )
        {
            invoking->DoSendMenuOpenCloseEvent(
                wxEVT_MENU_CLOSE, m_menu.get());
        }
    }

private:
    wxWeakRef<wxWindow> m_invoking;
    wxWeakRef<wxMenu> m_menu;
    bool m_open = true;
};

} // anonymous namespace

// ----------------------------------------------------------------------------
// Public frame menu-bar entry points
// ----------------------------------------------------------------------------

wxWindow* wxWinUIAttachFrameMenuBar(wxWindow *frame,
                                    wxMenuBar *menubar,
                                    wxWindow *existing)
{
    if ( existing )
        static_cast<wxWinUIMenuBarWindow *>(existing)->Retire();

    if ( !frame || !menubar )
        return nullptr;

    wxWinUIMenuBarWindow * const win = new wxWinUIMenuBarWindow();
    const wxWeakRef<wxWindow> stillAlive(win);
    if ( !win->Create(frame, menubar) )
    {
        if ( wxWindow * const survivor = stillAlive.get() )
            survivor->Destroy();
        return nullptr;
    }

    return win;
}

void wxWinUIRefreshFrameMenuBar(wxWindow *menuBarWin)
{
    if ( menuBarWin )
        static_cast<wxWinUIMenuBarWindow *>(menuBarWin)->Rebuild();
}

bool wxWinUI3MenuBarOpenForTesting(wxWindow *menuBarWindow,
                                   const size_t *menuPath,
                                   size_t menuPathLength)
{
    return menuBarWindow &&
           static_cast<wxWinUIMenuBarWindow *>(menuBarWindow)
               ->TestOpenMenu(menuPath, menuPathLength);
}

bool wxWinUI3MenuBarCloseForTesting(wxWindow *menuBarWindow,
                                    const size_t *menuPath,
                                    size_t menuPathLength)
{
    return menuBarWindow &&
           static_cast<wxWinUIMenuBarWindow *>(menuBarWindow)
               ->TestCloseMenu(menuPath, menuPathLength);
}

bool wxWinUI3MenuBarSnapshotForTesting(
    wxWindow *menuBarWindow,
    const size_t *menuPath,
    size_t menuPathLength,
    size_t itemPosition,
    wxWinUIMenuItemSnapshot *snapshot)
{
    return menuBarWindow &&
           static_cast<wxWinUIMenuBarWindow *>(menuBarWindow)
               ->TestSnapshot(
                   menuPath, menuPathLength, itemPosition, snapshot);
}

bool wxWinUI3MenuBarInvokeForTesting(wxWindow *menuBarWindow,
                                     const size_t *menuPath,
                                     size_t menuPathLength,
                                     size_t itemPosition)
{
    return menuBarWindow &&
           static_cast<wxWinUIMenuBarWindow *>(menuBarWindow)
               ->TestInvoke(menuPath, menuPathLength, itemPosition);
}

bool wxWinUI3MenuBarSetExpandedForTesting(
    wxWindow *menuBarWindow,
    const size_t *menuPath,
    size_t menuPathLength,
    bool expanded)
{
    return menuBarWindow &&
           static_cast<wxWinUIMenuBarWindow *>(menuBarWindow)
               ->TestSetExpanded(menuPath, menuPathLength, expanded);
}

bool wxWinUI3MenuBarGetExpandedForTesting(
    wxWindow *menuBarWindow,
    const size_t *menuPath,
    size_t menuPathLength,
    bool *expanded)
{
    return menuBarWindow &&
           static_cast<wxWinUIMenuBarWindow *>(menuBarWindow)
               ->TestGetExpanded(menuPath, menuPathLength, expanded);
}

size_t wxWinUI3MenuBarProjectionCountForTesting(wxWindow *menuBarWindow)
{
    return menuBarWindow
        ? static_cast<wxWinUIMenuBarWindow *>(menuBarWindow)
              ->TestProjectionCount()
        : 0;
}

bool wxWinUI3MenuBarSentinelContractForTesting(
    wxWindow *menuBarWindow,
    const size_t *menuPath,
    size_t menuPathLength)
{
    return menuBarWindow &&
           static_cast<wxWinUIMenuBarWindow *>(menuBarWindow)
               ->TestSentinelContract(menuPath, menuPathLength);
}

bool wxWinUI3MenuBarTopEnabledForTesting(wxWindow *menuBarWindow,
                                         size_t topIndex,
                                         bool *enabled)
{
    return menuBarWindow &&
           static_cast<wxWinUIMenuBarWindow *>(menuBarWindow)
               ->TestTopEnabled(topIndex, enabled);
}

bool wxWinUI3MenuBarTopLabelForTesting(wxWindow *menuBarWindow,
                                       size_t topIndex,
                                       wxString *label,
                                       wxString *accessKey)
{
    return menuBarWindow &&
           static_cast<wxWinUIMenuBarWindow *>(menuBarWindow)
               ->TestTopLabel(topIndex, label, accessKey);
}

int wxWinUI3MenuBarHeightForTesting(wxWindow *menuBarWindow)
{
    return menuBarWindow
        ? static_cast<wxWinUIMenuBarWindow *>(menuBarWindow)
              ->TestGetBarHeight()
        : 0;
}

bool wxWinUI3MenuBarFlowDirectionForTesting(
    wxWindow *menuBarWindow,
    wxWinUIMenuFlowDirection *barDirection,
    wxWinUIMenuFlowDirection *firstTopDirection)
{
    return menuBarWindow &&
           static_cast<wxWinUIMenuBarWindow *>(menuBarWindow)
               ->TestGetFlowDirection(
                   barDirection, firstTopDirection);
}

bool wxWinUI3MenuBarRefreshForScaleForTesting(
    wxWindow *menuBarWindow,
    double scale)
{
    return menuBarWindow &&
           static_cast<wxWinUIMenuBarWindow *>(menuBarWindow)
               ->TestRefreshForScale(scale);
}

bool wxWinUI3MenuBarBuildQuarantinedForTesting(wxWindow *menuBarWindow)
{
    return menuBarWindow &&
           static_cast<wxWinUIMenuBarWindow *>(menuBarWindow)
               ->TestBuildQuarantined();
}

void wxWinUI3RejectNextPopupDrainForTesting()
{
    gs_rejectNextPopupDrainForTesting = true;
}

void wxWinUI3FailNextPopupTimersForTesting(bool drain, bool watchdog)
{
    gs_failNextDrainTimerForTesting = drain;
    gs_failNextPopupWatchdogForTesting = watchdog;
}

// ----------------------------------------------------------------------------
// WinUI popup menus
// ----------------------------------------------------------------------------

bool wxWinUIPopupMenu(wxWindow *win, wxMenu *menu, int x, int y)
{
    if ( !win || !menu || !wxWinUI3Initialize() || !GetHwndOf(win) )
        return false;

    wxWinUITopLevelHost * const host =
        wxWinUITopLevelHost::ForWindow(win, true);
    if ( !host || !host->GetXamlRoot() )
        return false;

    bool menuSessionStarted = false;
    try
    {
        using winrt::Windows::Foundation::IInspectable;
        using winrt::Windows::Foundation::Point;

        const MUX::FrameworkElement root = host->Root();
        const MUXD::DispatcherQueue queue = root.DispatcherQueue();
        if ( !queue )
            return false;

        auto projectionState =
            std::make_shared<wxWinUIMenuProjectionState>();
        projectionState->invokingWindow = wxWeakRef<wxWindow>(win);
        const wxWeakRef<wxMenu> popupMenu(menu);

        MUXC::MenuFlyout flyout;
        flyout.ShouldConstrainToRootBounds(false);

        // Retain the coordinate in its source space. OPEN is arbitrary
        // application code and may move/reparent the invoking window, so the
        // common mapper resolves the current host and XamlRoot only after the
        // event has returned.
        const bool useMousePosition =
            x == wxDefaultCoord && y == wxDefaultCoord;
        const wxPoint popupPoint =
            useMousePosition ? wxGetMousePosition() : wxPoint(x, y);

        // Canonical OPEN first, then build the current menu. This is the same
        // ordering used by native popup menus and allows the OPEN handler to
        // change labels, enabled/check states, bitmaps and structure.
        wxWinUIMenuCloseGuard closeGuard(win, menu);
        menuSessionStarted = true;
        win->DoSendMenuOpenCloseEvent(wxEVT_MENU_OPEN, menu);
        wxWindow * const invokingAfterOpen =
            projectionState->invokingWindow.get();
        if ( !invokingAfterOpen || !popupMenu )
        {
            return true;
        }
        if ( wxWinUITLWHostIsDestroyScheduled(invokingAfterOpen) )
        {
            // A WinUI callback can park a TLW in the private deferred-destroy
            // queue instead of wxPendingDelete. Close the logical menu
            // session while this exact invoking object is still known alive;
            // the guard remains disarmed for its destructor.
            if ( !invokingAfterOpen->IsBeingDeleted() )
                closeGuard.Close(invokingAfterOpen);
            return true;
        }

        wxWinUITopLevelHost * const hostAfterOpen =
            wxWinUITopLevelHost::ForWindow(invokingAfterOpen, false);
        if ( hostAfterOpen != host ||
             !hostAfterOpen->GetXamlRoot() ||
             hostAfterOpen->Root() != root )
        {
            return true;
        }

        Point dip{};
        const Point sourcePoint{
            static_cast<float>(popupPoint.x),
            static_cast<float>(popupPoint.y)
        };
        const wxWinUICoordinateResult mapped =
            useMousePosition
                ? wxWinUIVisualCoordinates::ScreenPointToRoot(
                      invokingAfterOpen, sourcePoint, &dip)
                : wxWinUIVisualCoordinates::ClientPointToRoot(
                      invokingAfterOpen, sourcePoint, &dip);
        if ( mapped != wxWinUICoordinateResult::Mapped )
            return true;

        const MUX::FlowDirection flowDirection =
            invokingAfterOpen->GetLayoutDirection() ==
                    wxLayout_RightToLeft
                ? MUX::FlowDirection::RightToLeft
                : MUX::FlowDirection::LeftToRight;
        if ( projectionState->invokingWindow.get() !=
                 invokingAfterOpen ||
             wxWinUITopLevelHost::ForWindow(
                 invokingAfterOpen, false) != hostAfterOpen ||
             hostAfterOpen->Root() != root )
        {
            return true;
        }

        projectionState->flowDirection = flowDirection;
        wxWinUIPopulateMenuItems(
            flyout.Items(),
            popupMenu.get(),
            projectionState,
            0,
            nullptr);
        wxWinUIApplyMenuFlowDirection(
            flyout.Items(), flowDirection);

        // The flyout presenter is created outside the explicitly-LTR island
        // root. Give it the invoking wx window's semantic direction as a
        // local style; descendants and submenu roots then inherit the same
        // direction without changing root coordinate semantics.
        MUX::Style presenterStyle;
        presenterStyle.TargetType(
            winrt::Windows::UI::Xaml::Interop::TypeName{
                L"Microsoft.UI.Xaml.Controls.MenuFlyoutPresenter",
                winrt::Windows::UI::Xaml::Interop::TypeKind::Metadata});
        MUX::Setter flowSetter;
        flowSetter.Property(
            MUX::FrameworkElement::FlowDirectionProperty());
        flowSetter.Value(winrt::box_value(flowDirection));
        presenterStyle.Setters().Append(flowSetter);
        flyout.MenuFlyoutPresenterStyle(presenterStyle);

        auto loopState = std::make_shared<wxWinUIPopupLoopState>();
        loopState->invokingWindow = wxWeakRef<wxWindow>(win);
        loopState->topLevelWindow =
            wxWeakRef<wxWindow>(wxGetTopLevelParent(win));
        loopState->queue = queue;
        const std::weak_ptr<wxWinUIPopupLoopState> weakLoopState(loopState);

        const auto closedToken = flyout.Closed(
            [weakLoopState](const IInspectable&, const IInspectable&)
            {
                if ( const auto state = weakLoopState.lock() )
                {
                    state->closed = true;
                    state->ExitLoop();
                }
            });

        flyout.ShowAt(
            root,
            dip);

        if ( !loopState->closed )
        {
            wxEventLoop loop;
            loopState->loop = &loop;
            wxWinUIPopupWatchdog watchdog(loopState);
            const bool watchdogStarted =
                !std::exchange(
                    gs_failNextPopupWatchdogForTesting, false) &&
                watchdog.Start(100);
            if ( watchdogStarted )
            {
                loop.Run();
            }
            else
            {
                // Never enter an unbounded nested loop without its shutdown
                // watchdog. Hide below and report the session as consumed.
                loopState->aborted = true;
            }
            watchdog.Stop();
            loopState->loop = nullptr;
        }

        if ( loopState->aborted && !loopState->closed )
        {
            try
            {
                flyout.Hide();
            }
            catch ( const winrt::hresult_error& )
            {
            }
        }

        // Click can be queued immediately behind Closed. A marker accepted by
        // the same dispatcher proves all earlier callbacks were delivered.
        // Rejection or shutdown returns in bounded time without a nested loop.
        wxWinUIDrainDispatcher(queue);

        try
        {
            flyout.Closed(closedToken);
        }
        catch ( const winrt::hresult_error& )
        {
        }

        wxWinUICloseOutstandingSubmenus(projectionState);
        closeGuard.Close();

        const wxWinUIPendingMenuCommand command =
            projectionState->pendingCommand;
        projectionState->active = false;
        wxWinUIClearMenuProjectionState(projectionState);

        wxWindow * const invoking =
            projectionState->invokingWindow.get();
        if ( command.IsOk() && invoking )
        {
            wxMenu *commandRoot = command.owner.get();
            while ( commandRoot && commandRoot->GetParent() )
                commandRoot = commandRoot->GetParent();

            if ( !commandRoot )
                return true;

            wxMenuInvokingWindowSetter setInvokingWindow(
                *commandRoot, invoking);
            command.owner->MSWCommand(
                0, static_cast<WXWORD>(command.id));
        }

        return true;
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI popup menu", e);
    }

    // Once wxEVT_MENU_OPEN was emitted, falling back to TrackPopupMenu would
    // run a second OPEN/CLOSE sequence and replay user mutations. Treat that
    // attempted session as consumed; only pre-OPEN failures may fall back.
    return menuSessionStarted;
}

bool wxWinUI3PopupMenuForTesting(wxWindow *win,
                                 wxMenu *menu,
                                 int x,
                                 int y)
{
    return wxWinUIPopupMenu(win, menu, x, y);
}

#endif // wxUSE_WINUI3 && wxUSE_MENUS
