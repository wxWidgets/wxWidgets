/////////////////////////////////////////////////////////////////////////////
// Name:        src/ribbon/ribboncontainerswinui.cpp
// Purpose:     wxRibbon containers backed by native WinUI / Toolkit controls
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if defined(WXWINUI_RIBBON_TOOLKIT)

#include "wx/ribbon/bar.h"
#include "wx/ribbon/page.h"
#include "wx/ribbon/panel.h"
#include "wx/ribbon/buttonbar.h"
#include "wx/scopeguard.h"
#include "wx/sizer.h"
#include "wx/intl.h"
#include "wx/msw/private.h"
#include "wx/eventfilter.h"
#include "wx/toplevel.h"
#include "ribboncontainerswinui.h"
#include "wx/winui/private/tlwhost.h"
#include "wx/winui/private/appearance.h"
#include <algorithm>
#include <cmath>
#include <limits>
#include <winrt/Microsoft.UI.Xaml.Automation.h>
#include <winrt/Microsoft.UI.Xaml.Controls.Primitives.h>
#include <winrt/Microsoft.UI.Xaml.Data.h>
#include <winrt/Microsoft.UI.Xaml.Input.h>
#include <winrt/Microsoft.UI.Xaml.Markup.h>

// The XAML layout pass consumes immutable wx layout results. It must never
// invoke a wxSizer: SetDimension sends wxSizeEvents, whose handlers may rebuild
// command peers and synchronously call UpdateLayout on this same tree.
struct wxRibbonCommandLayout
{
    struct Item
    {
        winrt::Microsoft::UI::Xaml::Controls::ContentControl mount{nullptr};
        winrt::Windows::Foundation::Rect rect{};
    };
    bool hasSizer = false;
    winrt::Windows::Foundation::Size minimum{};
    winrt::Windows::Foundation::Size available{};
    std::vector<Item> items;
    std::function<void(const winrt::Windows::Foundation::Size&)> arranged;
};

namespace
{
namespace X = winrt::Microsoft::UI::Xaml;
namespace C = X::Controls;
namespace Labs = winrt::XamlToolkit::Labs::WinUI;
namespace F = winrt::Windows::Foundation;

bool RibbonContainsVisual(const X::DependencyObject& root,
                          X::DependencyObject node)
{
    if ( !root ) return false;
    while ( node )
    {
        if ( node == root ) return true;
        node = X::Media::VisualTreeHelper::GetParent(node);
    }
    return false;
}

class RibbonFocusFilter final : public wxEventFilter
{
public:
    explicit RibbonFocusFilter(std::function<void(wxEvent&)> notify)
        : m_notify(std::move(notify)) { wxEvtHandler::AddFilter(this); }
    ~RibbonFocusFilter() override { wxEvtHandler::RemoveFilter(this); }
    int FilterEvent(wxEvent& event) override
    {
        if ( event.GetEventType() == wxEVT_KILL_FOCUS ||
             event.GetEventType() == wxEVT_ACTIVATE ) m_notify(event);
        return Event_Skip;
    }
private:
    const std::function<void(wxEvent&)> m_notify;
};

// SelectionChanged cannot observe activation of the already selected tab.
// Handle the activation before TabViewItem's native selection behavior, so a
// pointer press on a different tab cannot be mistaken for a second press on
// the current tab. The native template, selection provider and input handling
// remain those of TabViewItem.
class RibbonTabItem : public C::TabViewItemT<RibbonTabItem>
{
public:
    explicit RibbonTabItem(std::function<bool(const C::TabViewItem&)> activate)
        : m_activate(std::move(activate)) {}

    winrt::hstring GetRuntimeClassName() const
    {
        // This private activation adapter adds no XAML type or properties.
        // Keep the native class identity for theme/template lookup instead
        // of exposing C++/WinRT's first implemented override interface.
        return winrt::hstring(winrt::name_of<C::TabViewItem>());
    }

    void OnApplyTemplate()
    {
        const auto item = get_strong().as<C::TabViewItem>();
        C::TabViewItemT<RibbonTabItem>::OnApplyTemplate();
        const auto presenter = GetTemplateChild(L"ContentPresenter")
            .try_as<C::ContentPresenter>();
        if ( !presenter || !RibbonContainsVisual(item, presenter) ) return;

        // WinUI 1.8's real TabViewItem template binds FontWeight, but fixes
        // its header FontSize to TabViewItemHeaderFontSize (12 DIPs). Let our
        // art-provider font reach that presenter too, without replacing the
        // native template, its brushes or its generated TextBlock. Resolve
        // the exact current part on every retemplate; retain no old part.
        X::Data::Binding binding;
        binding.Path(X::PropertyPath(L"FontSize"));
        X::Data::RelativeSource relative;
        relative.Mode(X::Data::RelativeSourceMode::TemplatedParent);
        binding.RelativeSource(relative);
        binding.Mode(X::Data::BindingMode::OneWay);
        presenter.SetBinding(C::ContentPresenter::FontSizeProperty(), binding);
    }

    void OnPointerPressed(const X::Input::PointerRoutedEventArgs& args)
    {
        const auto item = get_strong().as<C::TabViewItem>();
        if ( args.GetCurrentPoint(item).Properties().IsLeftButtonPressed() &&
             !m_activate(item) )
        {
            args.Handled(true);
            return;
        }
        C::TabViewItemT<RibbonTabItem>::OnPointerPressed(args);
    }

    void OnKeyDown(const X::Input::KeyRoutedEventArgs& args)
    {
        const auto key = args.Key();
        if ( key == winrt::Windows::System::VirtualKey::Enter ||
             key == winrt::Windows::System::VirtualKey::Space )
        {
            if ( !m_activate(get_strong().as<C::TabViewItem>()) )
            {
                args.Handled(true);
                return;
            }
        }
        C::TabViewItemT<RibbonTabItem>::OnKeyDown(args);
    }

private:
    const std::function<bool(const C::TabViewItem&)> m_activate;
};

struct RibbonCommandPanel : C::GridT<RibbonCommandPanel>
{
    explicit RibbonCommandPanel(std::shared_ptr<wxRibbonCommandLayout> layout)
        : m_layout(std::move(layout)) {}

    F::Size MeasureOverride(const F::Size& available)
    {
        if ( m_layout->hasSizer )
        {
            const auto items = m_layout->items;
            const auto minimum = m_layout->minimum;
            for ( const auto& item : items )
                item.mount.Measure({item.rect.Width, item.rect.Height});
            return minimum;
        }
        F::Size desired{};
        for ( const auto& child : Children() )
        {
            child.Measure({std::numeric_limits<float>::infinity(), available.Height});
            const auto size = child.DesiredSize();
            if ( desired.Width > 0 ) desired.Width += 4;
            desired.Width += size.Width;
            desired.Height = std::max(desired.Height, size.Height);
        }
        return desired;
    }

    F::Size ArrangeOverride(const F::Size& finalSize)
    {
        if ( m_layout->hasSizer )
        {
            const auto items = m_layout->items;
            for ( const auto& item : items ) item.mount.Arrange(item.rect);
        }
        else
        {
            float x = 0;
            for ( const auto& child : Children() )
            {
                const auto size = child.DesiredSize();
                child.Arrange({x, 0, size.Width, finalSize.Height});
                x += size.Width + 4;
            }
        }
        if ( m_layout->available != finalSize )
        {
            m_layout->available = finalSize;
            if ( const auto notify = m_layout->arranged ) notify(finalSize);
        }
        return finalSize;
    }

private:
    std::shared_ptr<wxRibbonCommandLayout> m_layout;
};

// Callbacks operate on exact identities, never a cached index that a native
// collection callback could have made stale. The host owns publication,
// rollback, and the physical-disconnect receipt.
template <typename Owner, typename Items>
std::shared_ptr<wxWinUIDirectCompositeMount> MakeMount(
    const Owner& owner, Items items)
{
    auto mount = std::make_shared<wxWinUIDirectCompositeMount>();
    mount->owner = winrt::make_weak(owner.template as<X::FrameworkElement>());
    const auto weak = winrt::make_weak(owner);
    mount->contains = [weak, items](const X::UIElement& element)
    {
        const auto live = weak.get();
        uint32_t index = 0;
        return live && items(live).IndexOf(element, index);
    };
    mount->insert = [weak, items](const X::UIElement& element)
    {
        const auto live = weak.get();
        if ( !live ) return false;
        uint32_t index = 0;
        const auto collection = items(live);
        if ( collection.IndexOf(element, index) ) return false;
        collection.Append(element);
        return collection.IndexOf(element, index);
    };
    mount->remove = [weak, items](const X::UIElement& element)
    {
        const auto live = weak.get();
        if ( !live ) return false;
        uint32_t index = 0;
        const auto collection = items(live);
        if ( collection.IndexOf(element, index) ) collection.RemoveAt(index);
        return !collection.IndexOf(element, index);
    };
    return mount;
}
} // namespace

wxRibbonContainerWinUI::wxRibbonContainerWinUI(wxRibbonControl* owner)
    : m_owner(owner)
{
}

wxRibbonControl* wxRibbonContainerWinUI::Owner() const
{
    auto* const owner = m_owner.get();
    return !m_retired && owner && !owner->IsBeingDeleted() &&
           owner->m_winuiContainer.get() == this ? owner : nullptr;
}

bool wxRibbonContainerWinUI::UpdateIcon(Icon& icon, const wxBitmapBundle& bundle,
                                      const wxSize& size)
{
    if ( !bundle.IsOk() )
    {
        icon = {};
        return true;
    }
    // GetBitmap() can clone its ref data to set the DPI scale factor. Cache
    // the immutable bundle and requested pixels, not the resulting bitmap.
    if ( icon.source && icon.bundle.IsSameAs(bundle) && icon.size == size )
        return true;
    const wxBitmap bitmap = bundle.GetBitmap(size);
    const auto image = wxWinUIWriteableBitmapFromBitmap(bitmap);
    if ( !image ) return false;
    const C::ImageIconSource source;
    source.ImageSource(image);
    icon = {bundle, size, source};
    return true;
}

bool wxRibbonContainerWinUI::Create(wxRibbonControl* owner)
{
    if ( !owner ) return false;
    wxWeakRef<wxRibbonControl> alive(owner);
    if ( owner->m_winuiContainer ) return Native(owner);
    auto peer = std::shared_ptr<wxRibbonContainerWinUI>(
        new wxRibbonContainerWinUI(owner));
    owner->m_winuiContainer = peer;
    try
    {
        if ( peer->Initialize() ) return true;
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("native ribbon container initialization", e);
    }
    if ( alive && alive->m_winuiContainer == peer ) alive->m_winuiContainer.reset();
    peer->Retire();
    return false;
}

bool wxRibbonContainerWinUI::Native(const wxRibbonControl* owner)
{
    return owner && owner->m_winuiContainer &&
           owner->m_winuiContainer->m_published &&
           owner->m_winuiContainer->Owner() == owner;
}

bool wxRibbonContainerWinUI::Initialize()
{
    auto* const owner = Owner();
    if ( !owner || !m_host.Initialize(owner) ) return false;
    const auto weak = weak_from_this();
    if ( wxDynamicCast(owner, wxRibbonBar) )
    {
        wxRibbonToolkitFailure failure;
        if ( !wxRibbonToolkitRuntime::TryAcquire(m_runtime, &failure) )
        {
            wxLogDebug("Native Ribbon runtime failed at %s: %s",
                         failure.stage, failure.detail);
            return false;
        }
        const C::Grid root;
        C::RowDefinition header, body;
        header.Height(X::GridLength{1, X::GridUnitType::Auto});
        body.Height(X::GridLength{1, X::GridUnitType::Star});
        root.RowDefinitions().Append(header);
        root.RowDefinitions().Append(body);
        m_tabs = C::TabView();
        m_tabs.IsAddTabButtonVisible(false);
        m_tabs.CanDragTabs(false);
        m_tabs.CanReorderTabs(false);
        m_tabs.TabWidthMode(C::TabViewWidthMode::SizeToContent);
        const C::StackPanel chrome;
        chrome.Orientation(C::Orientation::Horizontal);
        chrome.VerticalAlignment(X::VerticalAlignment::Center);
        m_toggleButton = C::Button();
        m_helpButton = C::Button();
        m_toggleIcon = C::FontIcon();
        m_toggleIcon.FontSize(14);
        m_toggleButton.Content(m_toggleIcon);
        const C::SymbolIcon helpIcon(C::Symbol::Help);
        m_helpButton.Content(helpIcon);
        for ( const auto& button : {m_toggleButton, m_helpButton} )
        {
            button.MinWidth(0);
            button.MinHeight(0);
            button.Width(32);
            button.Height(32);
            button.Padding({4, 4, 4, 4});
            chrome.Children().Append(button);
        }
        X::Automation::AutomationProperties::SetAutomationId(
            m_toggleButton, L"wxRibbonToggleButton");
        X::Automation::AutomationProperties::SetAutomationId(
            m_helpButton, L"wxRibbonHelpButton");
        const auto helpName = wxWinUIToHString(_("Help"));
        X::Automation::AutomationProperties::SetName(m_helpButton, helpName);
        C::ToolTipService::SetToolTip(m_helpButton, winrt::box_value(helpName));
        m_toggleToken = m_toggleButton.Click([weak](const auto&, const auto&)
        {
            if ( const auto peer = weak.lock() ) peer->OnBarButton(true);
        });
        m_helpToken = m_helpButton.Click([weak](const auto&, const auto&)
        {
            if ( const auto peer = weak.lock() ) peer->OnBarButton(false);
        });
        m_tabs.TabStripFooter(chrome);
        m_pages = C::Grid();
        C::Grid::SetRow(m_pages, 1);
        root.Children().Append(m_tabs);
        root.Children().Append(m_pages);
        m_root = root;
        m_selectionToken = m_tabs.SelectionChanged(
            [weak](const auto&, const auto&)
            {
                if ( const auto peer = weak.lock() ) peer->OnSelectionChanged();
            });
        if ( !m_host.SetContent(root, m_tabs) ) return false;
        m_published = true;
        // Popup focus does not bubble through the bar's inline visual root.
        // Observe actual XAML focus transitions, then resolve the current host
        // and exact owned popup trees after the native transition has settled.
        m_focusGotToken = X::Input::FocusManager::GotFocus(
            [weak](const auto&, const auto&)
            {
                if ( const auto peer = weak.lock() ) peer->QueueBarFocusCheck();
            });
        m_focusLostToken = X::Input::FocusManager::LostFocus(
            [weak](const auto&, const auto&)
            {
                if ( const auto peer = weak.lock() ) peer->QueueBarFocusCheck();
            });
        // FocusManager may retain its last leaf when focus leaves the island
        // for a classic HWND. Observe the ordinary wx loss/activation events
        // too, without consuming them or publishing any new focus authority.
        m_focusFilter = std::make_shared<RibbonFocusFilter>([weak](wxEvent& event)
        {
            const auto peer = weak.lock();
            auto* const bar = peer ? wxDynamicCast(peer->Owner(), wxRibbonBar) : nullptr;
            if ( !bar || bar->GetDisplayMode() != wxRIBBON_BAR_EXPANDED ) return;
            auto* const window = wxDynamicCast(event.GetEventObject(), wxWindow);
            if ( window && (window == bar || bar->IsDescendant(window) ||
                 (event.GetEventType() == wxEVT_ACTIVATE &&
                  window == wxGetTopLevelParent(bar))) ) peer->QueueBarFocusCheck();
        });
    }
    else
    {
        auto* const parent = wxDynamicCast(owner->GetParent(), wxRibbonControl);
        if ( !Native(parent) ) return false;
        const auto parentPeer = parent->m_winuiContainer;
        m_runtime = parentPeer->m_runtime;
        if ( wxDynamicCast(owner, wxRibbonPage) )
        {
            if ( !parentPeer->m_pages || !m_runtime->TryCreateRibbon(m_ribbon) )
                return false;
            m_root = m_ribbon;
            m_mount = MakeMount(parentPeer->m_pages,
                [](const C::Grid& grid) { return grid.Children(); });
            if ( !m_host.SetContentInParentDirect(
                    m_ribbon, m_ribbon, parent, m_mount) ) return false;
            m_pageSizeToken = m_ribbon.SizeChanged([weak](const auto&, const auto&)
            {
                if ( const auto peer = weak.lock() ) peer->QueuePageLayout();
            });
            m_pageItemsToken = m_ribbon.Items()
                .as<F::Collections::IObservableVector<X::UIElement>>()
                .VectorChanged([weak](const auto&, const auto&)
            {
                if ( const auto peer = weak.lock() ) peer->QueuePageLayout();
            });
        }
        else if ( auto* const panel = wxDynamicCast(owner, wxRibbonPanel) )
        {
            if ( !parentPeer->m_ribbon ) return false;
            if ( panel->GetFlags() & wxRIBBON_PANEL_NO_AUTO_MINIMISE )
            {
                if ( !m_runtime->TryCreateGroup(m_group) ) return false;
            }
            else
            {
                Labs::RibbonCollapsibleGroup collapsible{nullptr};
                if ( !m_runtime->TryCreateCollapsibleGroup(collapsible) )
                    return false;
                m_group = collapsible.as<Labs::RibbonGroup>();
            }
            m_group.Label(winrt::hstring(owner->GetLabel().wc_str()));
            m_commandLayout = std::make_shared<wxRibbonCommandLayout>();
            m_commandLayout->arranged = [weak](const F::Size& size)
            {
                if ( const auto peer = weak.lock() ) peer->OnCommandsArranged(size);
            };
            m_commands = winrt::make<RibbonCommandPanel>(m_commandLayout);
            m_group.Content(m_commands);
            m_root = m_group;
            m_panelGate = wxWinUICreateCompositePhysicalDisconnectGate();
            if ( !m_panelGate ) return false;
            m_host.SetPhysicalDisconnectGate(m_panelGate);
            m_mount = MakeMount(parentPeer->m_ribbon,
                [](const Labs::Ribbon& ribbon) { return ribbon.Items(); });
            if ( !m_host.SetContentInParentDirect(
                    m_group, m_group, parent, m_mount) ) return false;
            const auto extension = std::make_shared<wxWinUICompositeContentExtension>();
            extension->root = winrt::make_weak(m_commands.as<X::FrameworkElement>());
            const auto weakGroup = winrt::make_weak(m_group);
            const auto weakCommands = winrt::make_weak(m_commands);
            extension->isCurrent = [weakGroup, weakCommands]()
            {
                const auto group = weakGroup.get();
                const auto commands = weakCommands.get();
                if ( !group || !commands || group.Content() != commands ) return false;
                const auto collapsible = group.try_as<Labs::RibbonCollapsibleGroup>();
                if ( !collapsible ) return false;
                wxRibbonFlyoutParts parts;
                // Do not apply a template in a host validation callback.
                // The adapter prepares it before publishing child mounts.
                return wxRibbonReadCurrentFlyoutParts(collapsible, parts) ==
                           wxRibbonFlyoutResolveResult::Ready &&
                    (parts.inlineMount.Content() == commands ||
                     parts.popupMount.Content() == commands);
            };
            if ( !m_host.SetCompositeContentExtension(extension) ) return false;
            if ( const auto collapsible = m_group.try_as<Labs::RibbonCollapsibleGroup>() )
            {
                m_nativeStateToken = collapsible.RegisterPropertyChangedCallback(
                    Labs::RibbonCollapsibleGroup::StateProperty(),
                    [weak](const auto&, const auto&)
                    {
                        if ( const auto peer = weak.lock() ) peer->OnNativePanelState();
                    });
                m_nativeStateHooked = true;
            }
        }
        else
            return false;
        m_published = true;
        parentPeer->Synchronize();
    }
    if ( !Owner() ) return false;
    m_loadedToken = m_root.Loaded([weak](const auto&, const auto&)
    {
        if ( const auto peer = weak.lock() ) peer->OnLoaded();
    });
    if ( m_root.IsLoaded() ) OnLoaded();
    return Synchronize();
}

void wxRibbonContainerWinUI::OnLoaded()
{
    auto* const owner = Owner();
    if ( !owner || m_loadedWorkQueued ) return;
    // Arm the native button's Opening handler in Loaded itself: the deferred
    // child Realize below must not leave an interactive, unobserved Flyout.
    if ( m_group )
    {
        try { PrepareFlyout(); }
        catch ( const winrt::hresult_error& e )
            { wxWinUILogException("native ribbon flyout template", e); }
        if ( !Owner() ) return;
    }
    m_loadedWorkQueued = true;
    const auto weak = weak_from_this();
    owner->CallAfter([weak]()
    {
        const auto peer = weak.lock();
        auto* const live = peer ? peer->Owner() : nullptr;
        if ( !live ) return;
        peer->m_loadedWorkQueued = false;
        if ( !peer->Synchronize() || peer->Owner() != live ) return;
        // Templates now expose the Group.Content mount. No independent
        // temporary slot is created while this first publication is pending.
        std::vector<wxWeakRef<wxRibbonControl>> children;
        for ( auto* child : live->GetChildren() )
            if ( auto* control = wxDynamicCast(child, wxRibbonControl) )
                children.emplace_back(control);
        for ( const auto& child : children )
        {
            if ( !peer->Owner() ) return;
            if ( auto* control = child.get(); control && !control->IsBeingDeleted() )
                control->Realize();
        }
        if ( !peer->Owner() ) return;
        live->InvalidateBestSize();
        auto* bar = wxDynamicCast(live, wxRibbonBar);
        if ( !bar ) bar = live->GetAncestorRibbonBar();
        if ( bar )
        {
            // Commands acquire their real templates after the bar's first
            // Loaded notification. Their new natural size must reach the
            // sizer allocating the BAR, not merely the page/panel HWND. A
            // StaticBox is also not necessarily its sizer's containing window.
            bar->InvalidateBestSize();
            auto* const sizer = bar->GetContainingSizer();
            auto* const layoutOwner = sizer ? sizer->GetContainingWindow()
                                           : bar->GetParent();
            if ( layoutOwner ) layoutOwner->Layout();
        }
        else if ( auto* parent = live->GetParent() ) parent->Layout();
    });
}

bool wxRibbonContainerWinUI::Synchronize()
{
    auto* const owner = Owner();
    if ( !owner ) return false;
    if ( m_synchronizing )
    {
        m_syncDeferred = true;
        return true;
    }
    m_syncDeferred = false;
    m_synchronizing = true;
    const auto reset = wxMakeGuard([this]()
    {
        m_synchronizing = false;
        auto* const live = Owner();
        if ( !live || !m_syncDeferred || m_syncQueued ) return;
        m_syncQueued = true;
        const auto weak = weak_from_this();
        live->CallAfter([weak]()
        {
            const auto peer = weak.lock();
            auto* const current = peer ? peer->Owner() : nullptr;
            if ( !current ) return;
            peer->m_syncQueued = false;
            Sync(current);
        });
    });
    if ( auto* const bar = wxDynamicCast(owner, wxRibbonBar) )
    {
        const double scale = bar->GetDPIScaleFactor();
        if ( !std::isfinite(scale) || scale <= 0 ) return false;
        // GetFont is an application-overridable art-provider boundary.
        // Retain only the value, and do not use a changed/deleted provider or
        // owner after that callback. Native templates still own the text's
        // rendering and measurement; no DrawTab painting is substituted.
        auto* const art = bar->GetArtProvider();
        const wxFont tabLabelFont = art
            ? art->GetFont(wxRIBBON_ART_TAB_LABEL_FONT) : wxNullFont;
        if ( Owner() != bar || bar->GetArtProvider() != art ) return false;
        // The public margins reserve space beside the tab strip only. Pages
        // remain full width, and the existing TabView/items/providers stay
        // mounted. Keep wx pixels rather than cached DIPs across DPI changes.
        const X::Thickness margin{m_tabMargins.x / scale, 0,
                                  m_tabMargins.y / scale, 0};
        if ( m_tabs.Margin() != margin ) m_tabs.Margin(margin);
        if ( Owner() != bar ) return false;
        // Keep existing tab identities stable when a different page is
        // inserted/deleted. No command-containing visual tree is rebuilt.
        for ( size_t i = m_tabModels.size(); i-- > 0; )
        {
            auto* const page = m_tabModels[i].page.get();
            bool keep = page && !page->IsBeingDeleted();
            if ( keep )
            {
                keep = false;
                for ( size_t p = 0; p < bar->GetPageCount(); ++p )
                    if ( bar->GetPage(static_cast<int>(p)) == page ) keep = true;
            }
            if ( !keep )
            {
                uint32_t index = 0;
                if ( m_tabs.TabItems().IndexOf(m_tabModels[i].item, index) )
                    m_tabs.TabItems().RemoveAt(index);
                if ( Owner() != bar ) return false;
                m_tabModels.erase(m_tabModels.begin() + i);
            }
        }
        for ( size_t p = 0; p < bar->GetPageCount(); ++p )
        {
            auto* const page = bar->GetPage(static_cast<int>(p));
            const wxWeakRef<wxRibbonPage> livePage(page);
            const auto current = [&]()
            {
                return Owner() == bar && livePage && !livePage->IsBeingDeleted() &&
                       bar->GetArtProvider() == art &&
                       bar->GetPageNumber(livePage.get()) == static_cast<int>(p);
            };
            auto it = std::find_if(m_tabModels.begin(), m_tabModels.end(),
                [page](const Tab& tab) { return tab.page.get() == page; });
            if ( it == m_tabModels.end() )
            {
                const auto weak = weak_from_this();
                const auto item = winrt::make<RibbonTabItem>(
                    [weak, livePage](const C::TabViewItem& sender)
                    {
                        const auto peer = weak.lock();
                        return peer && livePage && peer->OnTabPressed(livePage.get(), sender);
                    }).as<C::TabViewItem>();
                Tab tab{page, item, wxString(), {}, {}};
                tab.item.IsClosable(false);
                m_tabModels.push_back(tab);
                m_tabs.TabItems().Append(tab.item);
                if ( !current() ) return false;
                it = std::prev(m_tabModels.end());
            }
            const wxString label = page->GetLabel();
            const long pageFlags = bar->GetWindowStyleFlag();
            const wxString shownLabel = pageFlags & wxRIBBON_BAR_SHOW_PAGE_LABELS
                                            ? label : wxString();
            const auto item = it->item;
            if ( !item.Header() || it->label != shownLabel )
            {
                item.Header(winrt::box_value(wxWinUIToHString(shownLabel)));
                if ( !current() ) return false;
                it->label = shownLabel;
            }
            C::IconSource source{nullptr};
            if ( pageFlags & wxRIBBON_BAR_SHOW_PAGE_ICONS )
            {
                const auto bundle = page->GetIconBundle();
                const auto size = bundle.IsOk()
                    ? bundle.GetPreferredBitmapSizeFor(page) : wxDefaultSize;
                if ( !current() || !UpdateIcon(it->icon, bundle, size) || !current() )
                    return false;
                source = it->icon.source;
            }
            if ( item.IconSource() != source ) item.IconSource(source);
            if ( !current() ) return false;
            // Hiding visual text must not remove the page's accessible name.
            const auto name = wxWinUIToHString(label);
            if ( X::Automation::AutomationProperties::GetName(item) != name )
                X::Automation::AutomationProperties::SetName(item, name);
            if ( !current() ) return false;
            if ( it->labelFont != tabLabelFont )
            {
                wxWinUIApplyFont(item, tabLabelFont);
                if ( !current() ) return false;
                it->labelFont = tabLabelFont;
            }
            item.Visibility(bar->IsPageShown(p) ? X::Visibility::Visible
                                                : X::Visibility::Collapsed);
            if ( !current() ) return false;
            if ( static_cast<int>(p) == bar->GetActivePage() )
                m_tabs.SelectedItem(item);
            if ( !current() ) return false;
        }
        if ( bar->GetActivePage() == wxNOT_FOUND ) m_tabs.SelectedItem(nullptr);
        if ( !bar->ArePanelsShown() )
        {
            // A group's Flyout lives outside m_pages. Collapsing the inline
            // body alone must not leave its command popup open. Snapshot wx
            // identities before Hide(): Closing handlers can delete/reopen
            // the bar or change pages synchronously.
            std::vector<wxWeakRef<wxRibbonPanel>> panels;
            for ( size_t p = 0; p < bar->GetPageCount(); ++p )
                for ( auto* child : bar->GetPage(static_cast<int>(p))->GetChildren() )
                    if ( auto* panel = wxDynamicCast(child, wxRibbonPanel);
                         panel && panel->GetExpandedPanel() ) panels.emplace_back(panel);
            for ( const auto& panel : panels )
            {
                if ( Owner() != bar ) return false;
                if ( bar->ArePanelsShown() ) break;
                if ( panel && !panel->IsBeingDeleted() && Native(panel.get()) )
                    HideExpanded(panel.get());
            }
            if ( Owner() != bar ) return false;
        }
        m_pages.Visibility(bar->ArePanelsShown() ? X::Visibility::Visible
                                               : X::Visibility::Collapsed);
        if ( Owner() != bar ) return false;
        const long flags = bar->GetWindowStyleFlag();
        const bool shown = bar->ArePanelsShown();
        m_toggleButton.Visibility(flags & wxRIBBON_BAR_SHOW_TOGGLE_BUTTON
                                     ? X::Visibility::Visible : X::Visibility::Collapsed);
        m_helpButton.Visibility(flags & wxRIBBON_BAR_SHOW_HELP_BUTTON
                                   ? X::Visibility::Visible : X::Visibility::Collapsed);
        m_toggleIcon.Glyph(shown ? L"\uE70E" : L"\uE70D");
        const auto toggleName = wxWinUIToHString(shown ? _("Minimize Ribbon")
                                                     : _("Expand Ribbon"));
        X::Automation::AutomationProperties::SetName(m_toggleButton, toggleName);
        C::ToolTipService::SetToolTip(m_toggleButton, winrt::box_value(toggleName));
        if ( Owner() != bar ) return false;
        const bool extensions = flags & wxRIBBON_BAR_SHOW_PANEL_EXT_BUTTONS;
        if ( !m_panelExtensionsKnown || m_panelExtensionsShown != extensions )
        {
            m_panelExtensionsKnown = true;
            m_panelExtensionsShown = extensions;
            std::vector<wxWeakRef<wxRibbonPanel>> panels;
            for ( size_t p = 0; p < bar->GetPageCount(); ++p )
                for ( auto* child : bar->GetPage(static_cast<int>(p))->GetChildren() )
                    if ( auto* panel = wxDynamicCast(child, wxRibbonPanel) )
                        panels.emplace_back(panel);
            for ( const auto& panel : panels )
            {
                if ( Owner() != bar ) return false;
                if ( panel && !panel->IsBeingDeleted() ) Sync(panel.get());
            }
        }
        if ( Owner() != bar ) return false;
        for ( size_t p = 0; p < bar->GetPageCount(); ++p )
        {
            auto* const page = bar->GetPage(static_cast<int>(p));
            if ( !Native(page) ) continue;
            const auto pagePeer = page->m_winuiContainer;
            const bool vertical = (flags & wxRIBBON_BAR_FLOW_VERTICAL) != 0;
            if ( !pagePeer->m_pageAxisKnown || pagePeer->m_pageVertical != vertical )
                pagePeer->QueuePageLayout();
            if ( Owner() != bar ) return false;
        }
    }
    else if ( m_ribbon )
    {
        if ( !SynchronizePageLayout() ) return false;
    }
    else if ( m_group )
    {
        m_group.Label(winrt::hstring(owner->GetLabel().wc_str()));
        if ( Owner() != owner ) return false;
        if ( const auto group = m_group.try_as<Labs::RibbonCollapsibleGroup>() )
        {
            auto* const panel = wxDynamicCast(owner, wxRibbonPanel);
            if ( !panel ) return false;
            const auto bundle = panel->GetMinimisedIconBundle();
            const auto size = bundle.IsOk()
                ? bundle.GetPreferredBitmapSizeFor(panel) : wxDefaultSize;
            if ( Owner() != owner || !UpdateIcon(m_minimisedIcon, bundle, size) ||
                 Owner() != owner )
                return false;
            if ( group.IconSource() != m_minimisedIcon.source )
                group.IconSource(m_minimisedIcon.source);
            if ( Owner() != owner ) return false;
        }
        if ( m_group.IsLoaded() && !PrepareFlyout() ) return false;
        if ( m_group.IsLoaded() && !PreparePanelButtons() ) return false;
        if ( !LayoutCommands() ) return false;
    }
    m_host.ForceRender();
    return Owner() != nullptr;
}

bool wxRibbonContainerWinUI::LayoutCommands()
{
    if ( const auto original = m_proxyFor.lock() ) return original->LayoutCommands();
    // Reparent fires RemoveChild/Sync between individual child transfers. The
    // native mounts never move: keep the complete layout snapshot until both
    // logical parent links and sizer ownership have committed or rolled back.
    if ( !m_commandLayout || m_arrangingCommands || m_expansionTransaction )
        return true;
    auto* panel = wxDynamicCast(Owner(), wxRibbonPanel);
    if ( !panel ) return false;
    if ( m_expanded && m_expanded->GetSizer() ) panel = m_expanded.get();
    const wxWeakRef<wxRibbonPanel> logical(panel);
    auto* const sizer = panel->GetSizer();
    m_arrangingCommands = true;
    const auto arranging = wxMakeGuard([this]()
    {
        m_arrangingCommands = false;
        if ( m_commandLayoutDeferred )
        {
            m_commandLayoutDeferred = false;
            OnCommandsArranged(m_commandLayout->available);
        }
    });
    const auto current = [&]()
    {
        return Owner() && logical && !logical->IsBeingDeleted() &&
               logical->GetSizer() == sizer;
    };
    F::Size minimum{};
    std::vector<wxRibbonCommandLayout::Item> items;
    if ( sizer )
    {
        const wxSize minSize = sizer->GetMinSize();
        if ( !current() ) return false;
        const auto minDIP = logical->ToDIP(minSize);
        minimum = {static_cast<float>(std::max(0, minDIP.x)),
                   static_cast<float>(std::max(0, minDIP.y))};
        const auto available = m_commandLayout->available;
        // XAML owns the available content area (including inside a Flyout).
        // Before first Arrange, use the sizer's own minimum, never the dummy
        // HWND size: that HWND may describe a collapsed button instead.
        const auto size = logical->FromDIP(wxSize(
            static_cast<int>(std::ceil(std::max(minimum.Width, available.Width))),
            static_cast<int>(std::ceil(std::max(minimum.Height, available.Height)))));
        if ( sizer->GetPosition() != wxPoint(0, 0) || sizer->GetSize() != size ||
             !m_commandLayout->hasSizer )
            sizer->SetDimension(wxPoint(0, 0), size);
        else
            sizer->Layout(); // flags/orientation/items may have changed
        if ( !current() ) return false;
        const auto models = m_commandModels;
        for ( const auto& model : models )
        {
            auto* const child = model.owner.get();
            if ( !child || child->IsBeingDeleted() || child->GetParent() != logical.get() )
                continue;
            const wxPoint pos = logical->ToDIP(child->GetPosition());
            const wxSize extent = logical->ToDIP(child->GetSize());
            items.push_back({model.mount,
                {static_cast<float>(pos.x), static_cast<float>(pos.y),
                 static_cast<float>(std::max(0, extent.x)),
                 static_cast<float>(std::max(0, extent.y))}});
        }
    }
    if ( !current() ) return false;
    bool changed = m_commandLayout->hasSizer != (sizer != nullptr) ||
                   m_commandLayout->minimum != minimum ||
                   m_commandLayout->items.size() != items.size();
    for ( size_t i = 0; !changed && i != items.size(); ++i )
        changed = m_commandLayout->items[i].mount != items[i].mount ||
                  m_commandLayout->items[i].rect != items[i].rect;
    if ( changed )
    {
        m_commandLayout->hasSizer = sizer != nullptr;
        m_commandLayout->minimum = minimum;
        m_commandLayout->items = std::move(items);
        m_commands.InvalidateMeasure();
        m_commands.InvalidateArrange();
        NotifyOwnerPageLayout();
    }
    return true;
}

void wxRibbonContainerWinUI::OnCommandsArranged(const F::Size& WXUNUSED(size))
{
    auto* const owner = Owner();
    if ( !owner || m_commandLayoutQueued ) return;
    m_commandLayoutQueued = true;
    const auto weak = weak_from_this();
    owner->CallAfter([weak]()
    {
        const auto peer = weak.lock();
        if ( !peer || !peer->Owner() ) return;
        peer->m_commandLayoutQueued = false;
        if ( peer->m_arrangingCommands )
        {
            // An application wxSizeEvent may pump this callback inside the
            // sizer transaction. Re-arm only after it returns, not from the
            // nested event loop which would otherwise drain it indefinitely.
            peer->m_commandLayoutDeferred = true;
            return;
        }
        try
        {
            if ( peer->LayoutCommands() && peer->Owner() ) peer->m_host.ForceRender();
        }
        catch ( const winrt::hresult_error& e )
            { wxWinUILogException("native ribbon sizer layout", e); }
    });
}

bool wxRibbonContainerWinUI::Layout(wxRibbonPanel* owner)
{
    if ( !Native(owner) ) return false;
    auto peer = owner->m_winuiContainer;
    if ( const auto original = peer->m_proxyFor.lock() ) peer = original;
    if ( peer->m_arrangingCommands ) return true;
    try
    {
        if ( !peer->LayoutCommands() || !peer->Owner() ) return false;
        peer->m_host.ForceRender();
        return peer->Owner() != nullptr;
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("native ribbon panel layout", e);
        return false;
    }
}

void wxRibbonContainerWinUI::NotifyFocusChange(wxRibbonControl* owner)
{
    if ( Native(owner) ) owner->m_winuiContainer->QueueBarFocusCheck();
}

void wxRibbonContainerWinUI::QueueBarFocusCheck()
{
    auto* const bar = wxDynamicCast(Owner(), wxRibbonBar);
    if ( !bar || bar->GetDisplayMode() != wxRIBBON_BAR_EXPANDED ||
         m_barFocusCheckQueued ) return;
    m_barFocusCheckQueued = true;
    const auto weak = weak_from_this();
    bar->CallAfter([weak]()
    {
        if ( const auto peer = weak.lock(); peer && peer->Owner() )
        {
            peer->m_barFocusCheckQueued = false;
            peer->CheckBarFocus();
        }
    });
}

void wxRibbonContainerWinUI::CheckBarFocus()
{
    auto* const bar = wxDynamicCast(Owner(), wxRibbonBar);
    if ( !bar || bar->GetDisplayMode() != wxRIBBON_BAR_EXPANDED ) return;
    try
    {
        // FindFocus follows the actual foreground thread HWND, not merely a
        // cached FocusManager leaf. It also covers ordinary wx descendants.
        auto* const logical = wxWindow::FindFocus();
        if ( logical && (logical == bar || bar->IsDescendant(logical)) ) return;
        auto* const host = wxWinUITopLevelHost::FindSlotOwner(bar);
        const HWND focus = ::GetFocus();
        const HWND bridge = host ? host->GetBridgeHwnd() : nullptr;
        if ( bridge && focus && (focus == bridge || ::IsChild(bridge, focus)) )
        {
            const auto root = host->GetXamlRoot();
            const auto focused = root
                ? X::Input::FocusManager::GetFocusedElement(root).try_as<X::DependencyObject>()
                : nullptr;
            if ( RibbonContainsVisual(m_root, focused) ) return;
            if ( Owner() != bar ) return;
            // A group's footer is outside the command slot. Accept only the
            // exact current group's Flyout.Content, never any popup sharing
            // the island or a flag saying that a popup happens to be open.
            std::vector<std::shared_ptr<wxRibbonContainerWinUI>> panels;
            for ( size_t p = 0; p < bar->GetPageCount(); ++p )
                for ( auto* child : bar->GetPage(static_cast<int>(p))->GetChildren() )
                    if ( auto* panel = wxDynamicCast(child, wxRibbonPanel); Native(panel) )
                        panels.push_back(panel->m_winuiContainer);
            for ( const auto& panel : panels )
            {
                const auto parts = panel->m_flyoutParts;
                if ( parts && panel->Owner() && wxRibbonFlyoutPartsAreCurrent(*parts) &&
                     parts->group.XamlRoot() == root && parts->flyout.IsOpen() )
                {
                    const auto content = parts->flyout.Content();
                    bool inside = RibbonContainsVisual(content, focused);
                    // WinUI can focus the Popup itself while transferring
                    // focus to its first command. Accept its native chrome
                    // only after proving it hosts this exact current content.
                    if ( !inside )
                        for ( const auto& popup : X::Media::VisualTreeHelper::
                                  GetOpenPopupsForXamlRoot(root) )
                        {
                            const auto child = popup.Child();
                            if ( popup.IsOpen() && child &&
                                 RibbonContainsVisual(child, content) &&
                                 (focused == popup || RibbonContainsVisual(child, focused)) )
                            {
                                inside = true;
                                break;
                            }
                        }
                    if ( inside && panel->Owner() && panel->m_flyoutParts == parts &&
                         wxRibbonFlyoutPartsAreCurrent(*parts) ) return;
                }
                if ( Owner() != bar ) return;
            }
        }
        if ( Owner() == bar ) bar->HideIfExpanded();
    }
    catch ( const winrt::hresult_error& e )
        { wxWinUILogException("native ribbon focus boundary", e); }
}

bool wxRibbonContainerWinUI::OnTabPressed(wxRibbonPage* page,
                                        const C::TabViewItem& item)
{
    try
    {
        auto* const bar = wxDynamicCast(Owner(), wxRibbonBar);
        if ( !bar || !page || page->IsBeingDeleted() ||
             !bar->IsEnabled() || !bar->IsShownOnScreen() ) return false;
        const int index = bar->GetPageNumber(page);
        if ( index == wxNOT_FOUND || !bar->IsPageShown(index) ||
             std::none_of(m_tabModels.begin(), m_tabModels.end(),
                 [&](const Tab& tab) { return tab.page.get() == page && tab.item == item; }) )
            return false;
        const wxWeakRef<wxRibbonPage> alive(page);
        if ( bar->GetDisplayMode() == wxRIBBON_BAR_MINIMIZED )
            bar->ShowPanels(wxRIBBON_BAR_EXPANDED);
        else if ( bar->GetDisplayMode() == wxRIBBON_BAR_EXPANDED &&
                  bar->GetActivePage() == index )
            bar->HidePanels();
        return Owner() == bar && alive && !alive->IsBeingDeleted() &&
               bar->GetPageNumber(alive.get()) != wxNOT_FOUND;
    }
    catch ( const winrt::hresult_error& e )
        { wxWinUILogException("native ribbon tab activation", e); }
    catch ( ... )
        { wxLogDebug("Exception in native ribbon tab activation handler"); }
    return false;
}

void wxRibbonContainerWinUI::OnSelectionChanged()
{
    if ( m_synchronizing ) return;
    auto* const bar = wxDynamicCast(Owner(), wxRibbonBar);
    if ( !bar ) return;
    if ( !bar->IsEnabled() || !bar->IsShownOnScreen() )
    {
        Synchronize();
        return;
    }
    wxWeakRef<wxRibbonPage> requested;
    for ( const auto& tab : m_tabModels )
        if ( tab.item == m_tabs.SelectedItem() ) requested = tab.page;
    auto* const page = requested.get();
    if ( !page || page->IsBeingDeleted() ) { Synchronize(); return; }
    if ( bar->GetActivePage() != wxNOT_FOUND &&
         bar->GetPage(bar->GetActivePage()) == page ) return;
    // As in the generic mouse path, selecting a tab opens the panel area
    // before PAGE_CHANGING is sent. A veto preserves the old page but must
    // not silently pin the bar or undo the temporary expansion.
    if ( bar->GetDisplayMode() == wxRIBBON_BAR_MINIMIZED )
    {
        bar->ShowPanels(wxRIBBON_BAR_EXPANDED);
        if ( Owner() != bar || !requested || requested->IsBeingDeleted() ||
             bar->GetPageNumber(requested.get()) == wxNOT_FOUND ) return;
    }
    wxRibbonBarEvent changing(wxEVT_RIBBONBAR_PAGE_CHANGING, bar->GetId(), page);
    changing.SetEventObject(bar);
    bar->GetEventHandler()->ProcessEvent(changing);
    if ( Owner() != bar ) return;
    if ( changing.IsAllowed() )
    {
        // The event may redirect, destroy or reorder a page. Compare its
        // identity to the current model before dereferencing it.
        auto* const redirected = changing.GetPage();
        for ( size_t i = 0; i < bar->GetPageCount(); ++i )
        {
            if ( bar->GetPage(static_cast<int>(i)) != redirected ) continue;
            if ( !redirected || redirected->IsBeingDeleted() ) break;
            if ( bar->SetActivePage(i) && Owner() == bar )
            {
                wxRibbonBarEvent changed(wxEVT_RIBBONBAR_PAGE_CHANGED,
                                         bar->GetId(), redirected);
                changed.SetEventObject(bar);
                bar->GetEventHandler()->ProcessEvent(changed);
            }
            break;
        }
    }
    if ( Owner() == bar ) Synchronize();
}

void wxRibbonContainerWinUI::OnBarButton(bool toggle)
{
    auto* const bar = wxDynamicCast(Owner(), wxRibbonBar);
    const long flag = toggle ? wxRIBBON_BAR_SHOW_TOGGLE_BUTTON
                             : wxRIBBON_BAR_SHOW_HELP_BUTTON;
    // Retained UIA providers must not invoke a now-hidden/disabled command.
    // The host's carrier supplies native inherited IsEnabled; this check also
    // covers model changes not yet delivered to the native dispatcher.
    if ( !bar || !bar->IsEnabled() || !bar->IsShownOnScreen() ||
         !(bar->GetWindowStyleFlag() & flag) ) return;
    try
    {
        if ( toggle )
        {
            bar->ShowPanels(bar->ArePanelsShown() ? wxRIBBON_BAR_MINIMIZED
                                                 : wxRIBBON_BAR_PINNED);
            if ( Owner() != bar ) return;
        }
        // As on the generic port, the new mode is already public when the
        // event is delivered, and neither chrome event carries a page.
        wxRibbonBarEvent event(toggle ? wxEVT_RIBBONBAR_TOGGLED
                                      : wxEVT_RIBBONBAR_HELP_CLICK, bar->GetId());
        event.SetEventObject(bar);
        bar->ProcessWindowEvent(event);
        // The handler may delete the bar, change its mode or hide the button.
        // Nothing may restore the pre-event state or dereference it here.
    }
    catch ( const winrt::hresult_error& e )
        { wxWinUILogException("native ribbon chrome command", e); }
    catch ( ... )
        { wxLogDebug("Exception in native ribbon chrome command handler"); }
}

bool wxRibbonContainerWinUI::Realize(wxRibbonControl* owner)
{
    if ( !Native(owner) ) return false;
    const auto peer = owner->m_winuiContainer;
    try
    {
        if ( !peer->Synchronize() ) return false;
        std::vector<wxWeakRef<wxRibbonControl>> children;
        for ( auto* child : owner->GetChildren() )
            if ( auto* control = wxDynamicCast(child, wxRibbonControl) )
                children.emplace_back(control);
        bool realized = true;
        for ( const auto& child : children )
        {
            if ( !peer->Owner() ) return false;
            if ( auto* control = child.get(); control && !control->IsBeingDeleted() )
                realized = control->Realize() && realized;
        }
        if ( peer->Owner() && !peer->LayoutCommands() ) return false;
        if ( peer->Owner() ) peer->m_host.ForceRender();
        return realized && peer->Owner();
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("native ribbon container synchronization", e);
        return false;
    }
}

void wxRibbonContainerWinUI::Sync(wxRibbonControl* owner)
{
    if ( !Native(owner) ) return;
    auto peer = owner->m_winuiContainer;
    if ( const auto original = peer->m_proxyFor.lock() ) peer = original;
    try { peer->Synchronize(); }
    catch ( const winrt::hresult_error& e )
        { wxWinUILogException("native ribbon model synchronization", e); }
}

wxSize wxRibbonContainerWinUI::BestSize(wxRibbonControl* owner)
{
    if ( !Native(owner) ) return wxDefaultSize;
    const auto peer = owner->m_winuiContainer;
    try
    {
        peer->m_root.Measure({std::numeric_limits<float>::infinity(),
                              std::numeric_limits<float>::infinity()});
        if ( peer->Owner() != owner ) return wxDefaultSize;
        auto size = peer->m_root.DesiredSize();
        if ( peer->Owner() != owner ) return wxDefaultSize;
        if ( peer->m_tabs && size.Height == 0 )
        {
            // TabView does not realize its strip while its host carrier has
            // no allocated height. Returning that zero to a proportion-zero
            // wxSizer prevents Loaded forever. Even ApplyTemplate on an item
            // cannot realize its default style before mounting. Bootstrap
            // from the SAME ThemeResource used by the native TabViewItem
            // style's MinHeight setter, not a hard-coded Ribbon height. The
            // application dictionary includes XamlControlsResources and its
            // overrides; Lookup (unlike HasKey) follows merged dictionaries.
            const auto application = X::Application::Current();
            if ( !application ) return wxDefaultSize;
            const auto metric = application.Resources().Lookup(
                winrt::box_value(winrt::hstring(L"TabViewItemMinHeight")));
            if ( peer->Owner() != owner || !metric ) return wxDefaultSize;
            const double headerHeight = winrt::unbox_value<double>(metric);
            if ( !std::isfinite(headerHeight) || headerHeight <= 0 ||
                 headerHeight > std::numeric_limits<int>::max() )
                return wxDefaultSize;
            size.Height = static_cast<float>(headerHeight);
        }
        if ( peer->Owner() != owner ||
             !std::isfinite(size.Width) || !std::isfinite(size.Height) ||
             size.Width < 0 || size.Height < 0 ||
             static_cast<double>(size.Width) > std::numeric_limits<int>::max() ||
             static_cast<double>(size.Height) > std::numeric_limits<int>::max() )
            return wxDefaultSize;
        return owner->FromDIP(wxSize(static_cast<int>(std::ceil(size.Width)),
                                     static_cast<int>(std::ceil(size.Height))));
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("native ribbon container measure", e);
        return wxDefaultSize;
    }
}

void wxRibbonContainerWinUI::SetTabMargins(wxRibbonBar* owner, int left, int right)
{
    if ( !Native(owner) ) return;
    const auto peer = owner->m_winuiContainer;
    peer->m_tabMargins = wxSize(left, right);
    Sync(owner);
}

void wxRibbonContainerWinUI::QueuePageLayout()
{
    auto* const owner = Owner();
    if ( !owner || !m_ribbon || m_pageLayoutQueued ) return;
    if ( m_pageLayoutTransaction )
    {
        m_pageLayoutDeferred = true;
        return;
    }
    m_pageLayoutQueued = true;
    const auto weak = weak_from_this();
    owner->CallAfter([weak]()
    {
        const auto peer = weak.lock();
        auto* const page = peer ? peer->Owner() : nullptr;
        if ( !page ) return;
        peer->m_pageLayoutQueued = false;
        if ( peer->m_pageLayoutTransaction )
        {
            peer->m_pageLayoutDeferred = true;
            return;
        }
        Sync(page);
    });
}

void wxRibbonContainerWinUI::NotifyOwnerPageLayout()
{
    auto* const owner = Owner();
    auto* const page = owner ? wxDynamicCast(owner->GetParent(), wxRibbonPage) : nullptr;
    if ( Native(page) )
        page->m_winuiContainer->QueuePageLayout();
}

bool wxRibbonContainerWinUI::InstallPageTemplate(const C::ControlTemplate& target)
{
    if ( !Owner() || !m_ribbon || !target ) return false;
    if ( !m_emptyPageTemplate )
        m_emptyPageTemplate = X::Markup::XamlReader::Load(LR"xaml(
<ControlTemplate xmlns="http://schemas.microsoft.com/winfx/2006/xaml/presentation"
 xmlns:lab="using:XamlToolkit.Labs.WinUI" TargetType="lab:Ribbon"><Grid /></ControlTemplate>)xaml")
            .as<C::ControlTemplate>();
    const auto previous = m_ribbon.Template();
    const auto apply = [&](const C::ControlTemplate& value)
    {
        const auto control = m_ribbon.as<C::IControlProtected>();
        const auto oldPanel = control.GetTemplateChild(L"Panel").try_as<C::Panel>();
        if ( m_pageScroll && m_pageScrollToken.value )
            m_pageScroll.ViewChanged(m_pageScrollToken);
        if ( m_pagePanel && m_pagePanelSizeToken.value )
            m_pagePanel.SizeChanged(m_pagePanelSizeToken);
        m_pageScrollToken = {};
        m_pagePanelSizeToken = {};
        m_pageScroll = nullptr;
        m_pagePanel = nullptr;
        m_pageScrollTemplate = nullptr;
        ++m_pageScrollRevision;
        m_pageScrollPending = false;
        m_pageScrollOffset = 0;
        // The pin retains its previous Panel and mirrors Items mutations into
        // it. First apply a real empty template so its private Panel is null.
        // Only then detach the old children: an Unloaded handler can delete or
        // append wx groups without corrupting that native vector mirror.
        // Ribbon.Items itself is NEVER cleared or rebuilt during this switch.
        m_ribbon.Template(m_emptyPageTemplate);
        if ( !Owner() || m_ribbon.Template() != m_emptyPageTemplate ) return false;
        m_ribbon.ApplyTemplate();
        if ( !Owner() || control.GetTemplateChild(L"Panel") ) return false;
        if ( oldPanel ) oldPanel.Children().Clear();
        if ( !Owner() ) return false;
        m_ribbon.Template(value);
        if ( !Owner() || m_ribbon.Template() != value ) return false;
        m_ribbon.ApplyTemplate();
        return Owner() && m_ribbon.Template() == value;
    };
    try
    {
        return apply(target);
    }
    catch ( const winrt::hresult_error& )
    {
        if ( Owner() && previous )
        {
            try { apply(previous); }
            catch ( const winrt::hresult_error& e )
                { wxWinUILogException("native ribbon template rollback", e); }
        }
        throw;
    }
}

bool wxRibbonContainerWinUI::SynchronizePageLayout()
{
    auto* const page = wxDynamicCast(Owner(), wxRibbonPage);
    if ( !page || !m_ribbon ) return false;
    if ( m_pageLayoutTransaction )
    {
        m_pageLayoutDeferred = true;
        return true;
    }
    m_pageLayoutTransaction = true;
    const auto reset = wxMakeGuard([this]()
    {
        m_pageLayoutTransaction = false;
        if ( m_pageLayoutDeferred )
        {
            m_pageLayoutDeferred = false;
            QueuePageLayout();
        }
    });
    const bool vertical = page->GetMajorAxis() == wxVERTICAL;
    if ( Owner() != page ) return false;
    m_ribbon.ApplyTemplate();
    if ( Owner() != page ) return false;
    if ( !m_ribbon.Template() ) return true; // OnLoaded retries the first template
    if ( !m_horizontalPageTemplate ) m_horizontalPageTemplate = m_ribbon.Template();
    if ( vertical != m_pageVertical )
    {
        // Replacing a page template unloads the group visuals. A live popup
        // must first complete its exact physical retirement/focus debt, not
        // merely report IsOpen=false. The observer below retries causally.
        std::vector<wxWeakRef<wxRibbonPanel>> panels;
        for ( auto* child : page->GetChildren() )
            if ( auto* panel = wxDynamicCast(child, wxRibbonPanel) ) panels.emplace_back(panel);
        for ( const auto& panel : panels )
        {
            if ( Owner() != page ) return false;
            if ( Native(panel.get()) && panel->GetExpandedPanel() ) HideExpanded(panel.get());
        }
        if ( Owner() != page ) return false;
        auto* const bar = page->GetAncestorRibbonBar();
        if ( bar && !wxWinUITopLevelHost::CanReparentSubtreeNow(bar) ) return true;
        if ( vertical && !m_verticalPageTemplate )
            m_verticalPageTemplate = X::Markup::XamlReader::Load(LR"xaml(
<ControlTemplate xmlns="http://schemas.microsoft.com/winfx/2006/xaml/presentation"
 xmlns:x="http://schemas.microsoft.com/winfx/2006/xaml"
 xmlns:lab="using:XamlToolkit.Labs.WinUI" TargetType="lab:Ribbon">
 <Grid Background="{TemplateBinding Background}" BorderBrush="{TemplateBinding BorderBrush}"
       BorderThickness="{TemplateBinding BorderThickness}">
  <ScrollViewer x:Name="ScrollViewer" IsTabStop="False"
      HorizontalScrollMode="Disabled" HorizontalScrollBarVisibility="Disabled"
      VerticalScrollMode="Auto" VerticalScrollBarVisibility="Auto"
      HorizontalContentAlignment="Stretch" VerticalContentAlignment="Top">
   <StackPanel x:Name="Panel" Orientation="Vertical" HorizontalAlignment="Stretch" />
  </ScrollViewer>
 </Grid>
</ControlTemplate>)xaml").as<C::ControlTemplate>();
        if ( Owner() != page ) return false;
        if ( !InstallPageTemplate(vertical ? m_verticalPageTemplate : m_horizontalPageTemplate) )
            return false;
        m_pageVertical = vertical;
    }
    m_pageAxisKnown = true;
    if ( !PreparePageScroll() ) return !m_ribbon.IsLoaded();
    return !m_pageVertical || AdaptVerticalPage();
}

bool wxRibbonContainerWinUI::AdaptVerticalPage()
{
    if ( !PageScrollIsCurrent() || !m_pageVertical ) return false;
    if ( !m_pageScroll.IsLoaded() ) return true;
    const double width = m_pageScroll.ViewportWidth();
    const double height = m_pageScroll.ViewportHeight();
    if ( width <= 0 || height <= 0 || !std::isfinite(width) || !std::isfinite(height) )
        return true;
    struct GroupSize { Labs::RibbonGroup group; F::Size expanded; bool pinned; };
    std::vector<GroupSize> groups;
    // Snapshot before any State/Measure callback. Neither an application
    // property handler nor a popup close may invalidate the iteration.
    const auto items = m_ribbon.Items();
    for ( const auto& item : items )
        if ( const auto group = item.try_as<Labs::RibbonGroup>();
             group && group.Visibility() == X::Visibility::Visible )
            groups.push_back({group, {}, false});
    const auto current = [&](const Labs::RibbonGroup& group)
    {
        uint32_t index = 0;
        return Owner() && PageScrollIsCurrent() && items.IndexOf(group, index);
    };
    const F::Size available{static_cast<float>(width), std::numeric_limits<float>::infinity()};
    double total = 0;
    for ( auto& entry : groups )
    {
        const auto collapsible = entry.group.try_as<Labs::RibbonCollapsibleGroup>();
        if ( collapsible )
        {
            wxRibbonFlyoutParts parts;
            entry.pinned = wxRibbonReadCurrentFlyoutParts(collapsible, parts) ==
                wxRibbonFlyoutResolveResult::Ready && parts.flyout.IsOpen();
            if ( !entry.pinned ) collapsible.State(X::Visibility::Visible);
        }
        if ( !current(entry.group) ) return false;
        entry.group.Measure(available);
        if ( !current(entry.group) ) return false;
        entry.expanded = entry.group.DesiredSize();
        if ( !std::isfinite(entry.expanded.Height) || !std::isfinite(entry.expanded.Width) )
            return false;
        total += entry.expanded.Height;
    }
    // As in the pin, lower priority is retained first, then earlier groups.
    std::stable_sort(groups.begin(), groups.end(), [](const auto& a, const auto& b)
    {
        const auto ga = a.group.template try_as<Labs::RibbonCollapsibleGroup>();
        const auto gb = b.group.template try_as<Labs::RibbonCollapsibleGroup>();
        return (ga ? ga.Priority() : 0) < (gb ? gb.Priority() : 0);
    });
    for ( auto it = groups.rbegin(); it != groups.rend(); ++it )
    {
        const auto group = it->group.try_as<Labs::RibbonCollapsibleGroup>();
        if ( !group || it->pinned || (total <= height && it->expanded.Width <= width) ) continue;
        if ( !current(it->group) ) return false;
        group.State(X::Visibility::Collapsed);
        if ( !current(it->group) ) return false;
        group.Measure(available);
        if ( !current(it->group) ) return false;
        const auto compact = group.DesiredSize();
        if ( !std::isfinite(compact.Height) || !std::isfinite(compact.Width) ) return false;
        if ( compact.Height < it->expanded.Height ||
             (it->expanded.Width > width && compact.Width < it->expanded.Width) )
            total += compact.Height - it->expanded.Height;
        else
        {
            group.State(X::Visibility::Visible);
            if ( !current(it->group) ) return false;
            group.Measure(available);
        }
    }
    return Owner() && PageScrollIsCurrent();
}

bool wxRibbonContainerWinUI::PageScrollIsCurrent() const
{
    if ( !Owner() || !m_ribbon || !m_pageScroll || !m_pageScrollTemplate ||
         m_ribbon.Template() != m_pageScrollTemplate ) return false;
    const auto control = m_ribbon.try_as<C::IControlProtected>();
    const auto panel = control
        ? control.GetTemplateChild(L"Panel").try_as<C::Panel>() : nullptr;
    return panel && control.GetTemplateChild(L"ScrollViewer") == m_pageScroll &&
           m_pageScroll.Content() == panel &&
           RibbonContainsVisual(m_ribbon, m_pageScroll) &&
           Owner() && m_ribbon.Template() == m_pageScrollTemplate;
}

bool wxRibbonContainerWinUI::PreparePageScroll()
{
    if ( !Owner() || !m_ribbon ) return false;
    m_ribbon.ApplyTemplate();
    if ( !Owner() ) return false;
    if ( PageScrollIsCurrent() ) return true;

    // These are the actual 0.8.0-alpha Ribbon template parts. Never select a
    // descendant gallery's ScrollViewer or retain a part from an old template.
    const auto controlTemplate = m_ribbon.Template();
    const auto control = m_ribbon.try_as<C::IControlProtected>();
    if ( !controlTemplate || !control ) return false;
    const auto scroll = control.GetTemplateChild(L"ScrollViewer").try_as<C::ScrollViewer>();
    const auto panel = control.GetTemplateChild(L"Panel").try_as<C::Panel>();
    if ( !scroll || !panel || scroll.Content() != panel ||
         !RibbonContainsVisual(m_ribbon, scroll) || !Owner() ||
         m_ribbon.Template() != controlTemplate ) return false;
    if ( m_pageScroll && m_pageScrollToken.value )
        m_pageScroll.ViewChanged(m_pageScrollToken);
    if ( m_pagePanel && m_pagePanelSizeToken.value )
        m_pagePanel.SizeChanged(m_pagePanelSizeToken);
    if ( !Owner() ) return false;
    m_pageScroll = scroll;
    m_pagePanel = panel;
    m_pageScrollTemplate = controlTemplate;
    m_pageScrollOffset = m_pageVertical ? scroll.VerticalOffset() : scroll.HorizontalOffset();
    m_pageScrollPending = false;
    ++m_pageScrollRevision;
    const auto weak = weak_from_this();
    m_pageScrollToken = scroll.ViewChanged([weak](const auto& sender, const auto& args)
    {
        if ( const auto peer = weak.lock(); peer && sender == peer->m_pageScroll )
            peer->OnPageViewChanged(args.IsIntermediate());
    });
    m_pagePanelSizeToken = panel.SizeChanged([weak](const auto&, const auto&)
    {
        if ( const auto peer = weak.lock() ) peer->QueuePageLayout();
    });
    return PageScrollIsCurrent();
}

void wxRibbonContainerWinUI::OnPageViewChanged(bool intermediate)
{
    try
    {
        if ( !PageScrollIsCurrent() || intermediate || m_pageScrollIssuing ) return;
        const double offset = m_pageVertical ? m_pageScroll.VerticalOffset()
                                            : m_pageScroll.HorizontalOffset();
        if ( !std::isfinite(offset) || !PageScrollIsCurrent() ) return;
        // A native arrow, wheel or UIA scroll has the same authority as a wx
        // request. Once the native change settles, the next relative wx call
        // must start from that real offset rather than an obsolete request.
        m_pageScrollOffset = offset;
        m_pageScrollPending = false;
        ++m_pageScrollRevision;
    }
    catch ( const winrt::hresult_error& e )
        { wxWinUILogException("native ribbon page scroll observation", e); }
}

bool wxRibbonContainerWinUI::ScrollPage(wxRibbonPage* owner, double pixels)
{
    if ( !Native(owner) || !std::isfinite(pixels) || pixels == 0 ) return false;
    const auto peer = owner->m_winuiContainer;
    try
    {
        if ( !peer->SynchronizePageLayout() ||
             peer->Owner() != owner ||
             peer->m_pageVertical != (owner->GetMajorAxis() == wxVERTICAL) ||
             !peer->PreparePageScroll() ||
             peer->Owner() != owner ) return false;
        const auto scroll = peer->m_pageScroll;
        const bool vertical = peer->m_pageVertical;
        if ( !scroll.IsLoaded() ||
             (vertical ? scroll.ViewportHeight() : scroll.ViewportWidth()) <= 0 ) return false;
        const double limit = vertical ? scroll.ScrollableHeight() : scroll.ScrollableWidth();
        const double actual = vertical ? scroll.VerticalOffset() : scroll.HorizontalOffset();
        const double scale = owner->GetDPIScaleFactor();
        if ( !peer->PageScrollIsCurrent() || !std::isfinite(limit) || limit < 0 ||
             !std::isfinite(actual) || !std::isfinite(scale) || scale <= 0 ) return false;

        if ( peer->m_pageScrollPending &&
             std::abs(actual - peer->m_pageScrollOffset) < 0.01 )
            peer->m_pageScrollPending = false;
        const double previous = peer->m_pageScrollPending
            ? std::clamp(peer->m_pageScrollOffset, 0.0, limit) : actual;
        const double target = std::clamp(previous + pixels / scale, 0.0, limit);
        if ( std::abs(target - previous) < 0.01 ) return false;

        const auto revision = ++peer->m_pageScrollRevision;
        const bool wasPending = peer->m_pageScrollPending;
        const bool wasIssuing = peer->m_pageScrollIssuing;
        peer->m_pageScrollOffset = target;
        peer->m_pageScrollPending = true;
        peer->m_pageScrollIssuing = true;
        bool accepted = false;
        const auto rollback = wxMakeGuard([peer, revision, previous, wasPending, &accepted]()
        {
            if ( !accepted && peer->m_pageScrollRevision == revision )
            {
                peer->m_pageScrollOffset = previous;
                peer->m_pageScrollPending = wasPending;
            }
        });
        const auto issuing = wxMakeGuard([peer, wasIssuing]()
            { peer->m_pageScrollIssuing = wasIssuing; });
        const auto boxed = winrt::box_value(target).as<F::IReference<double>>();
        accepted = vertical ? scroll.ChangeView(nullptr, boxed, nullptr, true)
                            : scroll.ChangeView(boxed, nullptr, nullptr, true);
        if ( peer->Owner() != owner || peer->m_pageScroll != scroll ||
             !peer->PageScrollIsCurrent() ) return false;
        return accepted;
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("native ribbon page scrolling", e);
        return false;
    }
}

bool wxRibbonContainerWinUI::ScrollPageSections(wxRibbonPage* owner, int sections)
{
    // The public API documents one section in either direction, not a count
    // of arbitrary magnitude. Keep the generic path on other backends intact.
    if ( !Native(owner) || (sections != -1 && sections != 1) ) return false;
    const auto peer = owner->m_winuiContainer;
    try
    {
        if ( !peer->SynchronizePageLayout() || peer->Owner() != owner ||
             peer->m_pageVertical != (owner->GetMajorAxis() == wxVERTICAL) ||
             !peer->PreparePageScroll() ||
             peer->Owner() != owner ) return false;
        const auto scroll = peer->m_pageScroll;
        const auto panel = scroll.Content().try_as<C::Panel>();
        const bool vertical = peer->m_pageVertical;
        const double viewport = vertical ? scroll.ViewportHeight() : scroll.ViewportWidth();
        const double limit = vertical ? scroll.ScrollableHeight() : scroll.ScrollableWidth();
        const double actual = vertical ? scroll.VerticalOffset() : scroll.HorizontalOffset();
        const double scale = owner->GetDPIScaleFactor();
        if ( !panel || !scroll.IsLoaded() || !std::isfinite(viewport) || viewport <= 0 ||
             !std::isfinite(limit) || limit <= 0 || !std::isfinite(actual) ||
             !std::isfinite(scale) || scale <= 0 || !peer->PageScrollIsCurrent() )
            return false;
        const double offset = peer->m_pageScrollPending
            ? std::clamp(peer->m_pageScrollOffset, 0.0, limit) : actual;
        const double edge = sections > 0 ? offset + viewport : offset;
        const double minimum = sections * 8.0 / scale;
        const auto revision = peer->m_pageScrollRevision;
        const auto items = peer->m_ribbon.Items();
        std::vector<F::Rect> bounds;
        bounds.reserve(items.Size());
        for ( const auto& item : items )
        {
            const auto group = item.try_as<Labs::RibbonGroup>();
            if ( !group || X::Media::VisualTreeHelper::GetParent(group) != panel )
                return false;
            if ( group.Visibility() != X::Visibility::Visible ) continue;
            // The pinned RibbonPanel (X) and native vertical StackPanel (Y)
            // arrange direct groups in increasing content coordinates.
            // Layout slots are its actual assigned content coordinates, before
            // the ScrollViewer applies offset/RTL mirroring. Do not derive
            // these bounds from invisible wx HWNDs or screen-space X values.
            const auto rect = C::Primitives::LayoutInformation::GetLayoutSlot(group);
            if ( !std::isfinite(vertical ? rect.Y : rect.X) ||
                 !std::isfinite(vertical ? rect.Height : rect.Width) ||
                 (vertical ? rect.Height : rect.Width) <= 0 )
                return false;
            bounds.push_back(rect);
        }
        if ( bounds.empty() || peer->Owner() != owner ||
             !peer->PageScrollIsCurrent() || peer->m_pageScrollRevision != revision )
            return false;
        if ( sections < 0 ) std::reverse(bounds.begin(), bounds.end());
        double pixels = 0;
        for ( const auto& rect : bounds )
        {
            const double begin = vertical ? rect.Y : rect.X;
            const double end = begin + (vertical ? rect.Height : rect.Width);
            if ( edge >= begin && edge <= end )
            {
                pixels += sections > 0 ? end - edge : begin - edge;
                if ( std::abs(pixels) >= std::abs(minimum) ) break;
            }
            if ( sections > 0 && edge <= begin )
            {
                pixels += end - begin;
                break;
            }
            if ( sections < 0 && edge >= end )
            {
                pixels += begin - end;
                break;
            }
        }
        if ( pixels == 0 ) pixels = minimum;
        if ( pixels * minimum < 0 ) pixels = -pixels;
        // ScrollPage owns clamping, pending relative requests and exact
        // native-client lifetime/template validation for every wx entry.
        return ScrollPage(owner, pixels * scale);
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("native ribbon page section scrolling", e);
        return false;
    }
}

wxRibbonContainerWinUI::MountStatus wxRibbonContainerWinUI::CommandMount(
    wxRibbonControl* command, wxRibbonControl*& compositeOwner,
    C::ContentControl& mount)
{
    auto* const parent = command
        ? wxDynamicCast(command->GetParent(), wxRibbonPanel) : nullptr;
    if ( !Native(parent) ) return MountStatus::Ordinary;
    auto peer = parent->m_winuiContainer;
    if ( const auto original = peer->m_proxyFor.lock() ) peer = original;
    auto* const logicalOwner = peer->Owner();
    if ( !logicalOwner || !logicalOwner->IsDescendant(command) )
        return MountStatus::Failed;
    if ( !peer->m_commands ) return MountStatus::Failed;
    try
    {
        for ( const auto& item : peer->m_commandModels )
            if ( item.owner.get() == command )
            {
                mount = item.mount;
                compositeOwner = logicalOwner;
                return MountStatus::Ready;
            }
        // Collapsing unloads the popup payload, not the group template. New
        // commands can still join its exact declared content while closed;
        // waiting for payload Loaded would prevent the expansion preflight.
        if ( !peer->m_group.IsLoaded() ) return MountStatus::Pending;
        const C::ContentControl content;
        content.IsTabStop(false);
        content.HorizontalContentAlignment(X::HorizontalAlignment::Stretch);
        content.VerticalContentAlignment(X::VerticalAlignment::Stretch);
        peer->m_commandModels.push_back({command, content});
        try { peer->m_commands.Children().Append(content); }
        catch (...)
        {
            peer->m_commandModels.pop_back();
            throw;
        }
        compositeOwner = logicalOwner;
        mount = content;
        return MountStatus::Ready;
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("native ribbon command mount", e);
        return MountStatus::Failed;
    }
}

bool wxRibbonContainerWinUI::IsMinimised(const wxRibbonPanel* owner)
{
    if ( !Native(owner) ) return false;
    const auto peer = owner->m_winuiContainer;
    if ( !peer->m_proxyFor.expired() ) return false;
    const auto group = peer->m_group.try_as<Labs::RibbonCollapsibleGroup>();
    X::Visibility state = X::Visibility::Visible;
    return group && wxRibbonReadFlyoutState(group, state) &&
           state == X::Visibility::Collapsed;
}

bool wxRibbonContainerWinUI::FooterIsCurrent(const Footer& footer, bool popup) const
{
    if ( !Owner() || !m_group || !footer.root || !footer.parent ||
         m_group.Template() != footer.controlTemplate ||
         footer.root.Children().Size() != 2 ||
         footer.root.Children().GetAt(0) != footer.label ||
         footer.root.Children().GetAt(1) != footer.state ||
         !footer.state || footer.state.Content() != footer.button ) return false;
    // Closed Flyout content has no visual Parent yet. Its exact declared
    // collection still proves ownership, just as for the popup command mount.
    uint32_t index = 0;
    if ( !footer.parent.Children().IndexOf(footer.root, index) ) return false;
    if ( popup )
        return m_flyoutParts && wxRibbonFlyoutPartsAreCurrent(*m_flyoutParts) &&
               m_flyoutParts->flyout.Content() == footer.parent;
    return X::Media::VisualTreeHelper::GetChildrenCount(m_group) == 1 &&
           X::Media::VisualTreeHelper::GetChild(m_group, 0) == footer.parent;
}

bool wxRibbonContainerWinUI::PreparePanelButtons()
{
    auto* const panel = wxDynamicCast(Owner(), wxRibbonPanel);
    if ( !panel || !m_group ) return false;
    const bool collapsible = bool(m_group.try_as<Labs::RibbonCollapsibleGroup>());
    const bool hasExtension = panel->HasExtButton();
    if ( Owner() != panel ) return false;
    m_hasPanelExtension = hasExtension;
    const auto name = wxWinUIToHString(
        wxString::Format(_("More options for %s"), panel->GetLabel()));
    for ( size_t i = 0; i < (collapsible ? 2u : 1u); ++i )
    {
        const bool popup = i != 0;
        auto& footer = m_footers[i];
        if ( !FooterIsCurrent(footer, popup) )
        {
            const auto controlTemplate = m_group.Template();
            C::Panel parent{nullptr};
            C::TextBlock label{nullptr};
            if ( collapsible )
            {
                const auto control = m_group.as<C::IControlProtected>();
                label = control.GetTemplateChild(popup ? L"FlyoutLabel" : L"Label")
                               .try_as<C::TextBlock>();
            }
            if ( popup )
            {
                if ( !m_flyoutParts || !wxRibbonFlyoutPartsAreCurrent(*m_flyoutParts) )
                    return false;
                parent = m_flyoutParts->flyout.Content().try_as<C::StackPanel>();
            }
            else
            {
                if ( X::Media::VisualTreeHelper::GetChildrenCount(m_group) != 1 )
                    return false;
                parent = X::Media::VisualTreeHelper::GetChild(m_group, 0).try_as<C::Grid>();
                // The pinned non-collapsible Group has one unnamed footer
                // label, directly in row 1. Do not select arbitrary text.
                if ( parent && !collapsible )
                    for ( const auto& child : parent.Children() )
                        if ( const auto text = child.try_as<C::TextBlock>();
                             text && C::Grid::GetRow(text) == 1 )
                        {
                            if ( label ) return false;
                            label = text;
                        }
            }
            uint32_t index = 0;
            if ( !parent || !label || !parent.Children().IndexOf(label, index) )
                return false;
            Footer next;
            next.controlTemplate = controlTemplate;
            next.parent = parent;
            next.label = label;
            next.root = C::Grid();
            C::ColumnDefinition title, action;
            title.Width({1, X::GridUnitType::Star});
            action.Width({1, X::GridUnitType::Auto});
            next.root.ColumnDefinitions().Append(title);
            next.root.ColumnDefinitions().Append(action);
            C::Grid::SetRow(next.root, C::Grid::GetRow(label));
            C::Grid::SetColumn(next.root, C::Grid::GetColumn(label));
            next.button = C::Button();
            next.button.MinWidth(0);
            next.button.MinHeight(0);
            next.button.Width(24);
            next.button.Height(24);
            next.button.Padding({2, 2, 2, 2});
            next.button.VerticalAlignment(X::VerticalAlignment::Bottom);
            next.button.Visibility(X::Visibility::Collapsed);
            const C::FontIcon icon;
            icon.FontSize(12);
            icon.Glyph(L"\uE8A7");
            next.button.Content(icon);
            // The popup has no wx host slot for its proxy panel. This carrier
            // projects the proxy's own enabled state without replacing the
            // button's binding to the dummy/group's inherited state below.
            next.state = C::ContentControl();
            next.state.IsTabStop(false);
            next.state.HorizontalContentAlignment(X::HorizontalAlignment::Stretch);
            next.state.VerticalContentAlignment(X::VerticalAlignment::Stretch);
            next.state.Content(next.button);
            C::Grid::SetColumn(next.state, 1);
            X::Automation::AutomationProperties::SetAutomationId(
                next.button, L"wxRibbonPanelExtension");
            // The popup is outside the group's visual ancestry. Bind effective
            // enabled state explicitly so UIA and keyboard agree with the bar.
            const X::Data::Binding enabled;
            enabled.Source(m_group);
            enabled.Path(X::PropertyPath(L"IsEnabled"));
            next.button.SetBinding(C::Control::IsEnabledProperty(), enabled);
            const auto weak = weak_from_this();
            const auto button = winrt::make_weak(next.button);
            next.click = next.button.Click([weak, button, popup](const auto&, const auto&)
            {
                if ( const auto peer = weak.lock(); peer &&
                     peer->m_footers[popup ? 1 : 0].button == button.get() )
                    peer->OnPanelButton(popup);
            });
            next.root.Children().Append(next.state);
            if ( footer.button && footer.click.value ) footer.button.Click(footer.click);
            footer = next;
            parent.Children().SetAt(index, next.root);
            if ( Owner() != panel || m_group.Template() != controlTemplate ) return false;
            C::Grid::SetRow(label, 0);
            C::Grid::SetColumn(label, 0);
            next.root.Children().InsertAt(0, label);
            if ( !FooterIsCurrent(footer, popup) ) return false;
        }
        if ( X::Automation::AutomationProperties::GetName(footer.button) != name )
        {
            X::Automation::AutomationProperties::SetName(footer.button, name);
            C::ToolTipService::SetToolTip(footer.button, winrt::box_value(name));
            if ( !FooterIsCurrent(footer, popup) ) return false;
        }
        const bool visible = hasExtension && (popup || !IsMinimised(panel));
        const auto visibility = visible ? X::Visibility::Visible : X::Visibility::Collapsed;
        if ( footer.button.Visibility() != visibility ) footer.button.Visibility(visibility);
        if ( !FooterIsCurrent(footer, popup) ) return false;
        const wxWeakRef<wxRibbonPanel> expanded(popup ? m_expanded.get() : nullptr);
        const bool enabled = !expanded || expanded->IsThisEnabled();
        const bool shown = !expanded || expanded->IsShown();
        if ( footer.state.IsEnabled() != enabled ) footer.state.IsEnabled(enabled);
        if ( !FooterIsCurrent(footer, popup) ||
             (popup && m_expanded.get() != expanded.get()) ) return false;
        // Hiding the proxy hides its label as well as the launcher. Command
        // carriers already follow their real wx parent's effective state via
        // the common host; do not overwrite individual command-local values.
        const auto footerVisibility = shown ? X::Visibility::Visible
                                            : X::Visibility::Collapsed;
        if ( footer.root.Visibility() != footerVisibility )
            footer.root.Visibility(footerVisibility);
        if ( !FooterIsCurrent(footer, popup) ) return false;
    }
    return true;
}

void wxRibbonContainerWinUI::OnPanelButton(bool popup)
{
    try
    {
        const auto& footer = m_footers[popup ? 1 : 0];
        auto* const original = wxDynamicCast(Owner(), wxRibbonPanel);
        wxWeakRef<wxRibbonPanel> panel(popup ? m_expanded.get() : original);
        if ( !original || !panel || panel->IsBeingDeleted() ||
             !FooterIsCurrent(footer, popup) ||
             footer.button.Visibility() != X::Visibility::Visible ||
             !panel->HasExtButton() || !panel || Owner() != original || !panel->IsEnabled() ||
             !panel->IsShownOnScreen() ) return;
        if ( popup )
        {
            if ( !m_flyoutParts || !m_flyoutParts->flyout.IsOpen() ||
                 panel->GetExpandedDummy() != original ||
                 original->GetExpandedPanel() != panel.get() ) return;
        }
        else if ( IsMinimised(original) ) return;
        wxRibbonPanelEvent event(wxEVT_RIBBONPANEL_EXTBUTTON_ACTIVATED, panel->GetId());
        event.SetEventObject(panel.get());
        event.SetPanel(panel.get());
        // TryAfter forwards the proxy's event to the dummy unchanged. The
        // application may close/delete either panel or the bar from here.
        panel->ProcessWindowEvent(event);
    }
    catch ( const winrt::hresult_error& e )
        { wxWinUILogException("native ribbon panel extension", e); }
    catch ( ... )
        { wxLogDebug("Exception in native ribbon panel extension handler"); }
}

bool wxRibbonContainerWinUI::IsExtensionHovered(const wxRibbonPanel* owner)
{
    if ( !Native(owner) ) return false;
    auto peer = owner->m_winuiContainer;
    const auto original = peer->m_proxyFor.lock();
    const bool popup = bool(original);
    if ( original ) peer = original;
    try
    {
        const auto& footer = peer->m_footers[popup ? 1 : 0];
        return peer->FooterIsCurrent(footer, popup) &&
               footer.button.Visibility() == X::Visibility::Visible &&
               footer.button.IsPointerOver();
    }
    catch ( const winrt::hresult_error& ) { return false; }
}

bool wxRibbonContainerWinUI::PrepareMinimisedIcon(const wxRibbonFlyoutParts& parts)
{
    if ( !m_minimisedIcon.source ) return true;
    auto* const owner = Owner();
    if ( !owner || !wxRibbonFlyoutPartsAreCurrent(parts) ) return false;
    const auto source = m_minimisedIcon.source;
    const auto bitmapSize = m_minimisedIcon.size;
    const double scale = owner->GetDPIScaleFactor();
    const auto current = [&]()
    {
        return Owner() == owner && m_minimisedIcon.source == source &&
               wxRibbonFlyoutPartsAreCurrent(parts);
    };
    if ( !current() || !std::isfinite(scale) || scale <= 0 ||
         bitmapSize.x <= 0 || bitmapSize.y <= 0 ) return false;
    const double width = bitmapSize.x / scale;
    const double height = bitmapSize.y / scale;
    const auto sizeIcon = [&](const C::IconElement& icon)
    {
        // ImageIcon stretches its image to the available measure constraint.
        // The pin leaves IconSourceElement.Width/Height as Auto, so copying
        // those alone makes a compact group grow to the viewport width. Give
        // the replacement the wx preferred bitmap extent in native DIPs.
        // Repeat this even for an unchanged ImageSource after a DPI change.
        if ( icon.Width() != width ) icon.Width(width);
        if ( !current() ) return false;
        if ( icon.Height() != height ) icon.Height(height);
        return current();
    };
    const auto stack = parts.button.Content().try_as<C::StackPanel>();
    if ( !stack ) return false;
    const auto children = stack.Children();
    for ( uint32_t i = 0; i < children.Size(); ++i )
    {
        const auto child = children.GetAt(i);
        if ( m_minimisedImage && child == m_minimisedImage )
        {
            if ( m_minimisedImageSource == source ) return sizeIcon(m_minimisedImage);
        }
        else
        {
            const auto element = child.try_as<C::IconSourceElement>();
            if ( !element || element.IconSource() != source ) continue;
        }
        // WinUI's IconSourceElement treats ImageIconSource as BitmapIconSource
        // and binds its empty UriSource. Use the public virtual factory instead.
        // Only replace this image in the qualified Toolkit button: keep its
        // chevron, command/provider, template, Flyout and content mounts intact.
        const auto previous = child.as<X::FrameworkElement>();
        const auto icon = source.CreateIconElement();
        if ( !icon ) return false;
        icon.Margin(previous.Margin());
        icon.HorizontalAlignment(previous.HorizontalAlignment());
        icon.VerticalAlignment(previous.VerticalAlignment());
        icon.Visibility(previous.Visibility());
        icon.MinWidth(previous.MinWidth());
        icon.MinHeight(previous.MinHeight());
        icon.MaxWidth(previous.MaxWidth());
        icon.MaxHeight(previous.MaxHeight());
        if ( !sizeIcon(icon) ) return false;
        m_minimisedImage = icon;
        m_minimisedImageSource = source;
        children.SetAt(i, icon);
        return current();
    }
    return false;
}

bool wxRibbonContainerWinUI::PrepareFlyout()
{
    const auto group = m_group.try_as<Labs::RibbonCollapsibleGroup>();
    if ( !group ) return true;
    // Keep the snapshot alive if publishing the icon reenters template work.
    if ( const auto current = m_flyoutParts;
         current && wxRibbonFlyoutPartsAreCurrent(*current) )
        return PrepareMinimisedIcon(*current);
    // Never replace the authority of an in-flight native popup with newly
    // discovered template parts. Its exact retirement state owns that graph.
    if ( m_flyoutSession &&
         !wxWinUIGetFlyoutPopupRetirementGate(m_flyoutSession)->IsSatisfied() )
        return false;
    wxRibbonFlyoutParts parts;
    if ( wxRibbonResolveFlyoutParts(group, parts) != wxRibbonFlyoutResolveResult::Ready )
        return false;
    if ( m_flyoutParts )
    {
        if ( m_openingToken.value ) m_flyoutParts->flyout.Opening(m_openingToken);
        if ( m_closedToken.value ) m_flyoutParts->flyout.Closed(m_closedToken);
    }
    m_flyoutParts = std::make_shared<wxRibbonFlyoutParts>(parts);
    const auto weak = weak_from_this();
    m_openingToken = parts.flyout.Opening([weak](const auto&, const auto&)
    {
        if ( const auto peer = weak.lock() ) peer->OnFlyoutOpening();
    });
    m_closedToken = parts.flyout.Closed([weak](const auto&, const auto&)
    {
        if ( const auto peer = weak.lock() ) peer->OnFlyoutClosed();
    });
    return PrepareMinimisedIcon(parts);
}

bool wxRibbonContainerWinUI::BeginExpansion()
{
    auto* const panel = wxDynamicCast(Owner(), wxRibbonPanel);
    if ( !panel || !IsMinimised(panel) || m_expansionTransaction ||
         m_expanded || panel->m_expanded_panel || panel->m_expanded_dummy ||
         !m_panelGate || m_panelGate->IsSealedForHostShutdown() ||
         !m_panelGate->IsSatisfied() || m_panelGate->IsDegraded() ||
         !PrepareFlyout() || !LayoutCommands() ) return false;

    // Only already-published native commands can be moved into this native
    // template. An unrelated HWND child is not silently reported as visible
    // in a XAML Flyout merely because its logical wx parent changed.
    std::vector<wxWeakRef<wxWindow>> children;
    std::vector<bool> shown;
    for ( auto* child : panel->GetChildren() )
    {
        const auto found = std::find_if(m_commandModels.begin(), m_commandModels.end(),
            [child](const Command& item) { return item.owner.get() == child; });
        if ( found == m_commandModels.end() || !found->mount.Content() ||
             child->IsBeingDeleted() ) return false;
        children.emplace_back(child);
        shown.push_back(child->IsShown());
    }

    m_expansionTransaction = true;
    const auto transaction = wxMakeGuard([this]() { m_expansionTransaction = false; });
    wxWeakRef<wxRibbonPanel> expanded(new wxRibbonPanel);
    m_expanded = expanded;
    const auto proxy = std::shared_ptr<wxRibbonContainerWinUI>(
        new wxRibbonContainerWinUI(expanded.get()));
    proxy->m_proxyFor = shared_from_this();
    proxy->m_runtime = m_runtime;
    proxy->m_root = m_commands;
    proxy->m_published = true;
    expanded->m_winuiContainer = proxy;
    bool committed = false;
    const auto rollback = wxMakeGuard([&]()
    {
        if ( committed || !Owner() ) return;
        RestoreExpansion();
        for ( size_t i = 0; i != children.size() && Owner(); ++i )
            if ( auto* child = children[i].get(); child && child->GetParent() == panel )
                child->Show(shown[i]);
    });
    if ( !expanded->wxRibbonControl::Create(panel, wxID_ANY, wxPoint(0, 0),
                                            panel->GetSize(), wxBORDER_NONE) ||
         !Owner() || !expanded ) return false;
    expanded->CommonInit(panel->GetLabel(), panel->GetMinimisedIconBundle(), panel->m_flags);
    if ( !Owner() || !expanded ) return false;
    expanded->m_art = panel->m_art;

    for ( const auto& weakChild : children )
    {
        auto* const child = weakChild.get();
        if ( !Owner() || !expanded || !child || child->GetParent() != panel ||
             !child->Reparent(expanded.get()) || !Owner() || !expanded ||
             !weakChild || weakChild->GetParent() != expanded.get() ) return false;
        child->Show();
        if ( !Owner() || !expanded ) return false;
    }
    if ( auto* const sizer = panel->GetSizer() )
    {
        panel->SetSizer(nullptr, false);
        expanded->SetSizer(sizer);
    }
    if ( !Owner() || !expanded ) return false;
    expanded->m_expanded_dummy = panel;
    panel->m_expanded_panel = expanded.get();
    expanded->Show();
    if ( !Owner() || !expanded || !PreparePanelButtons() ) return false;

    const auto parts = m_flyoutParts;
    const auto root = parts->group.XamlRoot();
    if ( !root || !wxRibbonFlyoutPartsAreCurrent(*parts) ) return false;
    m_flyoutSession = wxWinUICreateFlyoutPopupRetirementState(
        parts->flyout, root, parts->popupMount);
    const auto gate = wxWinUIGetFlyoutPopupRetirementGate(m_flyoutSession);
    if ( !gate || gate->IsDegraded() ||
         !wxWinUIAddCompositePhysicalDisconnectGate(m_panelGate, gate) )
        return false;
    const auto weak = weak_from_this();
    const std::weak_ptr<wxWinUIFlyoutPopupRetirementState> session = m_flyoutSession;
    if ( !wxWinUISetFlyoutPopupRetirementObserver(m_flyoutSession, [weak, session]()
    {
        if ( const auto peer = weak.lock(); peer && peer->Owner() &&
             peer->m_flyoutSession == session.lock() )
        {
            peer->RestoreExpansion();
            if ( peer->Owner() ) peer->NotifyOwnerPageLayout();
        }
    }) ) return false;
    committed = true;
    return true;
}

bool wxRibbonContainerWinUI::RestoreExpansion()
{
    if ( m_restoringExpansion ) return false;
    auto* const panel = wxDynamicCast(Owner(), wxRibbonPanel);
    wxWeakRef<wxRibbonPanel> expanded(m_expanded);
    if ( !panel || !expanded ) return false;
    const bool wasTransaction = m_expansionTransaction;
    m_expansionTransaction = true;
    m_restoringExpansion = true;
    const auto transaction = wxMakeGuard([this, wasTransaction]()
    {
        m_expansionTransaction = wasTransaction;
        m_restoringExpansion = false;
    });
    std::vector<wxWeakRef<wxWindow>> children;
    for ( auto* child : expanded->GetChildren() ) children.emplace_back(child);
    for ( const auto& weakChild : children )
    {
        auto* const child = weakChild.get();
        if ( !Owner() || !expanded ) return false;
        if ( !child || child->IsBeingDeleted() || child->GetParent() != expanded.get() )
            continue;
        if ( !child->Reparent(panel) || !Owner() || !expanded || !weakChild ||
             weakChild->GetParent() != panel ) return false;
        // Toolkit State, not the proxy HWND, owns visibility of the commands.
        // Keep them logically shown so a subsequent native expansion can
        // expose the very same slots without a separate visibility override.
    }
    if ( auto* const sizer = expanded->GetSizer() )
    {
        expanded->SetSizer(nullptr, false);
        panel->SetSizer(sizer);
    }
    if ( !Owner() || !expanded ) return false;
    panel->m_expanded_panel = nullptr;
    expanded->m_expanded_dummy = nullptr;
    m_expanded = nullptr;
    expanded->Destroy();
    if ( Owner() )
    {
        PreparePanelButtons();
        if ( Owner() ) m_host.ForceRender();
    }
    return true;
}

bool wxRibbonContainerWinUI::ShowExpanded(wxRibbonPanel* owner)
{
    if ( !Native(owner) ) return false;
    const auto peer = owner->m_winuiContainer;
    if ( !peer->m_proxyFor.expired() ) return false;
    try
    {
        if ( !peer->BeginExpansion() ) return false;
        const auto parts = peer->m_flyoutParts;
        parts->flyout.ShowAt(parts->button);
        wxWinUIResolveFlyoutPopupAfterShow(peer->m_flyoutSession);
        const auto gate = wxWinUIGetFlyoutPopupRetirementGate(peer->m_flyoutSession);
        return peer->Owner() && peer->m_expanded && gate && !gate->IsDegraded();
    }
    catch ( const winrt::hresult_error& e )
    {
        if ( peer->m_flyoutSession ) wxWinUIFailFlyoutPopupShow(peer->m_flyoutSession);
        if ( peer->Owner() ) peer->RestoreExpansion();
        wxWinUILogException("native ribbon panel expansion", e);
        return false;
    }
}

bool wxRibbonContainerWinUI::HideExpanded(wxRibbonPanel* owner)
{
    if ( !Native(owner) ) return false;
    auto peer = owner->m_winuiContainer;
    if ( const auto original = peer->m_proxyFor.lock() ) peer = original;
    if ( !peer->Owner() || peer->m_expansionTransaction || !peer->m_expanded )
        return false;
    try
    {
        peer->RequestFlyoutClose();
        // No content is detached: all mounts remain in the Toolkit group.
        // The slot's gate still retains the native popup until its exact
        // Closed/focus causal tail, independently of these wx parent links.
        return !peer->m_expanded || peer->RestoreExpansion();
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("native ribbon panel collapse", e);
        return false;
    }
}

void wxRibbonContainerWinUI::OnFlyoutOpening()
{
    if ( !Owner() ) return;
    if ( m_expanded && m_flyoutSession ) return; // public ShowExpanded path
    try
    {
        if ( !BeginExpansion() ) RequestFlyoutClose();
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("native ribbon button expansion", e);
        RequestFlyoutClose();
    }
}

void wxRibbonContainerWinUI::OnFlyoutClosed()
{
    if ( Owner() && !m_expansionTransaction ) RestoreExpansion();
}

void wxRibbonContainerWinUI::OnNativePanelState()
{
    try
    {
        const auto& footer = m_footers[0];
        if ( FooterIsCurrent(footer, false) )
        {
            // Include the launcher during the Toolkit's temporary Visible
            // measurement too. Deferring this visibility change can make the
            // natural width alternate across the collapse threshold.
            const auto group = m_group.as<Labs::RibbonCollapsibleGroup>();
            const auto visibility = m_hasPanelExtension && group.State() == X::Visibility::Visible
                                  ? X::Visibility::Visible : X::Visibility::Collapsed;
            const auto button = footer.button;
            if ( button.Visibility() != visibility ) button.Visibility(visibility);
        }
    }
    catch ( const winrt::hresult_error& e )
        { wxWinUILogException("native ribbon extension visibility", e); }
    auto* const panel = wxDynamicCast(Owner(), wxRibbonPanel);
    if ( !panel || m_nativeStateWorkQueued ) return;
    // Adaptive measure can temporarily restore Visible to measure a group's
    // natural width before choosing Collapsed again. Observe the committed
    // layout state on one coalesced wx turn, not those intermediate probes.
    m_nativeStateWorkQueued = true;
    const auto weak = weak_from_this();
    panel->CallAfter([weak]()
    {
        const auto peer = weak.lock();
        auto* const live = peer ? wxDynamicCast(peer->Owner(), wxRibbonPanel) : nullptr;
        if ( !live ) return;
        peer->m_nativeStateWorkQueued = false;
        if ( peer->m_expansionTransaction ) return;
        try
        {
            if ( !IsMinimised(live) && peer->m_expanded ) HideExpanded(live);
            if ( peer->Owner() ) Sync(live);
        }
        catch ( const winrt::hresult_error& e )
            { wxWinUILogException("native ribbon adaptive state", e); }
    });
}

void wxRibbonContainerWinUI::RequestFlyoutClose()
{
    if ( !m_flyoutParts || m_requestingFlyoutClose ) return;
    m_requestingFlyoutClose = true;
    const auto close = wxMakeGuard([this]() { m_requestingFlyoutClose = false; });
    m_flyoutParts->flyout.Hide();
}

void wxRibbonContainerWinUI::Retire()
{
    if ( m_retired ) return;
    m_retired = true;
    m_owner = nullptr;
    m_focusFilter.reset();
    try
    {
        if ( m_loadedToken.value ) m_root.Loaded(m_loadedToken);
        if ( m_selectionToken.value ) m_tabs.SelectionChanged(m_selectionToken);
        if ( m_toggleToken.value ) m_toggleButton.Click(m_toggleToken);
        if ( m_helpToken.value ) m_helpButton.Click(m_helpToken);
        if ( m_pageScroll && m_pageScrollToken.value )
            m_pageScroll.ViewChanged(m_pageScrollToken);
        if ( m_pagePanel && m_pagePanelSizeToken.value )
            m_pagePanel.SizeChanged(m_pagePanelSizeToken);
        if ( m_ribbon && m_pageSizeToken.value ) m_ribbon.SizeChanged(m_pageSizeToken);
        if ( m_ribbon && m_pageItemsToken.value )
            m_ribbon.Items().as<F::Collections::IObservableVector<X::UIElement>>()
                .VectorChanged(m_pageItemsToken);
        if ( m_focusGotToken.value ) X::Input::FocusManager::GotFocus(m_focusGotToken);
        if ( m_focusLostToken.value ) X::Input::FocusManager::LostFocus(m_focusLostToken);
        for ( const auto& footer : m_footers )
            if ( footer.button && footer.click.value ) footer.button.Click(footer.click);
        if ( m_flyoutParts )
        {
            if ( m_openingToken.value ) m_flyoutParts->flyout.Opening(m_openingToken);
            if ( m_closedToken.value ) m_flyoutParts->flyout.Closed(m_closedToken);
            RequestFlyoutClose();
        }
        if ( m_nativeStateHooked )
            m_group.as<Labs::RibbonCollapsibleGroup>().UnregisterPropertyChangedCallback(
                Labs::RibbonCollapsibleGroup::StateProperty(), m_nativeStateToken);
    }
    catch ( const winrt::hresult_error& e )
        { wxWinUILogException("native ribbon handler removal", e); }
    m_loadedToken = {};
    m_selectionToken = {};
    // The host retains roots until child popup/physical disconnection gates
    // finish. Never clear Ribbon.Items or Group.Content from this destructor.
    m_host.Close();
}

void wxRibbonContainerWinUI::Close(wxRibbonControl* owner)
{
    if ( !owner ) return;
    const auto peer = std::move(owner->m_winuiContainer);
    if ( !peer ) return;
    // No raw wx object is touched after the first native close callback.
    // A closing handler may destroy either logical panel or the whole TLW.
    if ( auto* const panel = wxDynamicCast(owner, wxRibbonPanel) )
    {
        if ( const auto original = peer->m_proxyFor.lock() )
        {
            if ( auto* const dummy = wxDynamicCast(original->Owner(), wxRibbonPanel) )
            {
                if ( dummy->m_expanded_panel == panel ) dummy->m_expanded_panel = nullptr;
                if ( original->m_expanded.get() == panel ) original->m_expanded = nullptr;
                // A directly deleted proxy is still in dummy.GetChildren()
                // during this derived destructor. Defer Hide until it has
                // left that list, so Closing may safely delete the dummy.
                if ( original->m_flyoutParts )
                {
                    const auto flyout = original->m_flyoutParts->flyout;
                    const auto session = original->m_flyoutSession;
                    const std::weak_ptr<wxRibbonContainerWinUI> weak = original;
                    dummy->CallAfter([weak, flyout, session]()
                    {
                        const auto live = weak.lock();
                        if ( !live || !live->Owner() || live->m_expanded ||
                             live->m_flyoutSession != session ) return;
                        try
                        {
                            if ( live->m_flyoutParts && live->m_flyoutParts->flyout == flyout )
                                live->RequestFlyoutClose();
                        }
                        catch ( const winrt::hresult_error& e )
                            { wxWinUILogException("native ribbon proxy retirement", e); }
                    });
                }
            }
        }
        panel->m_expanded_dummy = nullptr;
        if ( auto* const expanded = peer->m_expanded.get() )
            expanded->m_expanded_dummy = nullptr;
        panel->m_expanded_panel = nullptr;
    }
    peer->Retire();
}

#endif // WXWINUI_RIBBON_TOOLKIT
