/////////////////////////////////////////////////////////////////////////////
// Name:        src/aui/auibookwinui.cpp
// Purpose:     Retained WinUI TabView headers for wxAuiNotebook panes
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if defined(__WXWINUI__) && wxUSE_WINUI3 && wxUSE_AUI

#include "wx/aui/auibook.h"
#include "wx/app.h"
#include "wx/private/windowlifetime.h"
#include "wx/weakref.h"
#include "wx/scopeguard.h"
#include "wx/winui/private/tlwhost.h"
#include "../winui/private.h"
#include "auibookwinui.h"

#include <winrt/Microsoft.UI.Xaml.Automation.h>
#include <winrt/Microsoft.UI.Xaml.Controls.Primitives.h>
#include <winrt/Microsoft.UI.Xaml.Media.h>
#include <winrt/Microsoft.UI.Xaml.Input.h>
#include <algorithm>
#include <cmath>
#include <vector>
#ifndef wxNO_RTTI
#include <typeinfo>
#endif

namespace X = winrt::Microsoft::UI::Xaml;
namespace C = X::Controls;
namespace M = X::Media;
namespace A = X::Automation;

namespace
{
template<typename T>
T FindTemplateControl(const X::DependencyObject& root)
{
    if ( !root ) return nullptr;
    // TabView's private x:Names are not a public contract. Like wxNotebook,
    // resolve the strip by its public base interface, bounded to this subtree.
    std::vector<X::DependencyObject> pending{root};
    constexpr size_t maxNodes = 4096;
    for ( size_t index = 0; index < pending.size(); ++index )
    {
        const auto current = pending[index];
        if ( const auto control = current.try_as<T>() ) return control;
        const int count = M::VisualTreeHelper::GetChildrenCount(current);
        for ( int child = 0; child < count; ++child )
        {
            if ( pending.size() == maxNodes ) return nullptr;
            pending.push_back(M::VisualTreeHelper::GetChild(current, child));
        }
    }
    return nullptr;
}

X::FrameworkElement TabViewport(const C::TabView& view)
{
    const auto list = FindTemplateControl<C::ListViewBase>(view);
    const auto scroll = FindTemplateControl<C::ScrollViewer>(list);
    return FindTemplateControl<C::ScrollContentPresenter>(scroll);
}

X::FrameworkElement FindPart(const X::DependencyObject& root,
                            const wchar_t* name)
{
    if ( !root ) return nullptr;
    if ( const auto element = root.try_as<X::FrameworkElement>();
         element && element.Name() == name ) return element;
    const int count = M::VisualTreeHelper::GetChildrenCount(root);
    for ( int i = 0; i < count; ++i )
        if ( auto part = FindPart(M::VisualTreeHelper::GetChild(root, i), name) )
            return part;
    return nullptr;
}

wxRect NativeRect(wxWindow* owner, const X::FrameworkElement& element)
{
    if ( !element ||
         element.Visibility() != X::Visibility::Visible ) return {};
    winrt::Windows::Foundation::Rect rect{};
    if ( wxWinUIVisualCoordinates::ElementBoundsToClient(owner, element,
            {0, 0, float(element.ActualWidth()), float(element.ActualHeight())},
            &rect) != wxWinUICoordinateResult::Mapped ) return {};
    return wxRect(wxRound(rect.X), wxRound(rect.Y),
                  wxRound(rect.Width), wxRound(rect.Height));
}
} // namespace

struct wxAuiTabCtrlWinUI::Impl
{
    struct Peer
    {
        wxWeakRef<wxWindow> page;
        C::TabViewItem tab{nullptr};
        wxString caption;
        wxString tooltip;
        wxBitmap bitmap;
        C::ImageIconSource icon{nullptr};
    };

    Impl(wxAuiTabCtrl* control, Command callback)
        : owner(control), command(std::move(callback)) {}

    wxAuiTabCtrl* Owner() const
    {
        auto* const live = owner.get();
        return !closed && live && !wxWindowIsUnavailableForCallbacks(live)
            ? live : nullptr;
    }

    wxWeakRef<wxAuiTabCtrl> owner;
    Command command;
    wxWinUIControlHost host;
    C::TabView view{nullptr};
    C::TabViewItem scrollGuard{nullptr};
    winrt::event_token bring{};
    std::shared_ptr<Peer> revealPeer;
    unsigned revealAttempts = 0;
    unsigned long long revealSerial = 0;
    bool revealQueued = false;
    bool applyingReveal = false;
    winrt::Windows::Foundation::IInspectable pointerIntent{nullptr}, keyIntent{nullptr};
    C::Button closeButton{nullptr};
    C::Button listButton{nullptr};
    std::vector<std::shared_ptr<Peer>> peers;
    winrt::event_token selection{}, close{}, loaded{}, layout{}, focus{};
    winrt::event_token globalClose{}, windowList{};
    bool closed = false;
    bool updating = false;
    bool queued = false;
    bool dirty = true;
    bool attached = false;
    bool geometry = false;
    bool retryingReveal = false;
    unsigned long long revision = 1;
};

wxAuiTabCtrlWinUI::wxAuiTabCtrlWinUI(wxAuiTabCtrl* owner, Command command)
    : m_impl(new Impl(owner, std::move(command))) {}

wxAuiTabCtrlWinUI::~wxAuiTabCtrlWinUI() { Close(); }

bool wxAuiTabCtrlWinUI::CanPresent(const wxAuiTabCtrl* owner)
{
    if ( !owner || owner->IsFlagSet(wxAUI_NB_MULTILINE) ) return false;
    return MinimumHeight(*owner, const_cast<wxAuiTabCtrl*>(owner)) != 0;
}

int wxAuiTabCtrlWinUI::MinimumHeight(const wxAuiTabContainer& tabs, wxWindow* owner)
{
    if ( !owner || tabs.IsFlagSet(wxAUI_NB_MULTILINE) ) return 0;
#ifndef wxNO_RTTI
    // A user Draw() implementation is an explicit custom presentation, not a
    // Fluent tab. Keep it callable instead of silently discarding its contract.
    const auto* const art = tabs.GetArtProvider();
    const bool native = art && (typeid(*art) == typeid(wxAuiDefaultTabArt) ||
                   typeid(*art) == typeid(wxAuiGenericTabArt) ||
                   typeid(*art) == typeid(wxAuiSimpleTabArt) ||
                   typeid(*art) == typeid(wxAuiFlatTabArt));
    return native ? owner->FromDIP(48) : 0;
#else
    return 0;
#endif
}

std::shared_ptr<wxAuiTabCtrlWinUI> wxAuiTabCtrlWinUI::Create(
    wxAuiTabCtrl* owner, Command command)
{
    auto projection = std::shared_ptr<wxAuiTabCtrlWinUI>(
        new wxAuiTabCtrlWinUI(owner, std::move(command)));
    return projection->Initialize() ? projection : nullptr;
}

bool wxAuiTabCtrlWinUI::Initialize()
{
    auto* const owner = m_impl->Owner();
    if ( !owner || !m_impl->host.Initialize(owner) ) return false;
    try
    {
        auto& state = *m_impl;
        state.view = C::TabView();
        state.view.IsAddTabButtonVisible(false);
        // AUI's captured drag is also responsible for splits and external
        // notebooks. A second TabView reorder source would bypass that model.
        state.view.CanDragTabs(false);
        state.view.CanReorderTabs(false);
        state.view.AllowDrop(false);
        A::AutomationProperties::SetAutomationId(state.view, L"wxAui.TabView");

        const auto weak = weak_from_this();
        state.selection = state.view.SelectionChanged(
            [weak](const auto&, const auto&)
            {
                const auto self = weak.lock();
                if ( !self || self->m_impl->updating || !self->IsActive() ) return;
                self->ClearVisibilityRequest();
                const auto selected = self->m_impl->view.SelectedItem();
                for ( const auto& peer : self->m_impl->peers )
                {
                    if ( selected == peer->tab )
                    {
                        auto* const page = peer->page.get();
                        self->Dispatch(page, 0);
                        return;
                    }
                }
            });
        state.close = state.view.TabCloseRequested(
            [weak](const auto&, const C::TabViewTabCloseRequestedEventArgs& args)
            {
                const auto self = weak.lock();
                if ( !self || self->m_impl->updating || !self->IsActive() ) return;
                for ( const auto& peer : self->m_impl->peers )
                {
                    if ( args.Tab() == peer->tab )
                    {
                        self->Dispatch(peer->page.get(), wxAUI_BUTTON_CLOSE);
                        return;
                    }
                }
            });
        state.loaded = state.view.Loaded([weak](const auto&, const auto&)
        {
            if ( const auto self = weak.lock() ) self->Invalidate();
        });
        state.layout = state.view.LayoutUpdated([weak](const auto&, const auto&)
        {
            if ( const auto self = weak.lock() )
            {
                self->SynchronizeGeometry();
                self->ContinueVisibilityRequest();
            }
        });
        const auto inputIntent = [weak](const auto&, const auto&)
        {
            if ( const auto self = weak.lock() ) self->ClearVisibilityRequest();
        };
        state.pointerIntent = winrt::box_value(X::Input::PointerEventHandler(inputIntent));
        state.keyIntent = winrt::box_value(X::Input::KeyEventHandler(inputIntent));
        state.view.AddHandler(X::UIElement::PointerPressedEvent(), state.pointerIntent, true);
        state.view.AddHandler(X::UIElement::PointerWheelChangedEvent(), state.pointerIntent, true);
        state.view.AddHandler(X::UIElement::KeyDownEvent(), state.keyIntent, true);
        state.focus = state.view.GotFocus(inputIntent);

        C::StackPanel footer;
        footer.Orientation(C::Orientation::Horizontal);
        state.closeButton = C::Button();
        C::FontIcon closeIcon;
        closeIcon.Glyph(L"\xE711");
        state.closeButton.Content(closeIcon);
        A::AutomationProperties::SetName(state.closeButton, L"Close active tab");
        A::AutomationProperties::SetAutomationId(state.closeButton, L"wxAui.CloseActive");
        state.globalClose = state.closeButton.Click([weak](const auto&, const auto&)
        {
            if ( const auto self = weak.lock(); self && self->IsActive() )
            {
                auto* const live = self->m_impl->Owner();
                const int active = live->GetActivePage();
                if ( active != wxNOT_FOUND )
                    self->Dispatch(live->GetWindowFromIdx(active), wxAUI_BUTTON_CLOSE);
            }
        });
        state.listButton = C::Button();
        C::FontIcon listIcon;
        listIcon.Glyph(L"\xE70D");
        state.listButton.Content(listIcon);
        A::AutomationProperties::SetName(state.listButton, L"Open tab list");
        A::AutomationProperties::SetAutomationId(state.listButton, L"wxAui.TabList");
        state.windowList = state.listButton.Click([weak](const auto&, const auto&)
        {
            if ( const auto self = weak.lock() )
                self->Dispatch(nullptr, wxAUI_BUTTON_WINDOWLIST);
        });
        footer.Children().Append(state.listButton);
        footer.Children().Append(state.closeButton);
        state.view.TabStripFooter(footer);
        Invalidate();
        return m_impl->Owner() != nullptr;
    }
    catch ( const winrt::hresult_error& error )
    {
        wxWinUILogException("WinUI AUI TabView creation", error);
        Close();
        return false;
    }
}

void wxAuiTabCtrlWinUI::Close()
{
    auto& state = *m_impl;
    if ( state.closed ) return;
    state.closed = true;
    state.owner = nullptr;
    ++state.revision;
    const auto revoke = [](auto& token, auto&& remove)
    {
        if ( !token.value ) return;
        const auto old = token;
        token = {};
        try { remove(old); } catch ( ... ) {}
    };
    revoke(state.selection, [&](auto token) { state.view.SelectionChanged(token); });
    revoke(state.close, [&](auto token) { state.view.TabCloseRequested(token); });
    revoke(state.loaded, [&](auto token) { state.view.Loaded(token); });
    revoke(state.layout, [&](auto token) { state.view.LayoutUpdated(token); });
    revoke(state.focus, [&](auto token) { state.view.GotFocus(token); });
    revoke(state.globalClose, [&](auto token) { state.closeButton.Click(token); });
    revoke(state.windowList, [&](auto token) { state.listButton.Click(token); });
    ClearVisibilityRequest();
    if ( state.view )
    {
        try
        {
            if ( state.pointerIntent )
            {
                state.view.RemoveHandler(X::UIElement::PointerPressedEvent(), state.pointerIntent);
                state.view.RemoveHandler(X::UIElement::PointerWheelChangedEvent(), state.pointerIntent);
            }
            if ( state.keyIntent )
                state.view.RemoveHandler(X::UIElement::KeyDownEvent(), state.keyIntent);
        }
        catch ( ... ) {}
    }
    state.host.Close();
    state.peers.clear();
    state.view = nullptr;
}

bool wxAuiTabCtrlWinUI::IsActive() const
{
    const auto& state = *m_impl;
    auto* const owner = state.Owner();
    return owner && state.attached && state.view && CanPresent(owner) &&
           state.host.GetContent() == state.view;
}

void wxAuiTabCtrlWinUI::Invalidate()
{
    auto& state = *m_impl;
    auto* const owner = state.Owner();
    if ( !owner ) return;
    state.dirty = true;
    ++state.revision;
    if ( state.queued ) return;
    state.queued = true;
    owner->CallAfter([weak = weak_from_this()]
    {
        if ( const auto self = weak.lock() )
        {
            self->m_impl->queued = false;
            self->Update();
        }
    });
}

void wxAuiTabCtrlWinUI::Dispatch(wxWindow* page, int command)
{
    auto& state = *m_impl;
    auto* const owner = state.Owner();
    if ( !owner || state.updating || !IsActive() ) return;
    if ( page && (wxWindowIsUnavailableForCallbacks(page) ||
                  owner->GetIdxFromWindow(page) == wxNOT_FOUND) ) return;
    if ( !page && command != wxAUI_BUTTON_WINDOWLIST ) return;
    if ( command == wxAUI_BUTTON_CLOSE &&
         owner->GetPages()[size_t(owner->GetIdxFromWindow(page))].kind ==
             wxAuiTabKind::Locked ) return;
    // The callback re-enters the existing AUI selection/close transaction.
    // A veto, nested mutation or destruction always wins over the native value.
    const auto callback = state.command;
    callback(owner, page, command);
    if ( state.Owner() )
    {
        Invalidate();
        Update();
    }
}

void wxAuiTabCtrlWinUI::Update()
{
    const auto keepAlive = shared_from_this();
    auto& state = *m_impl;
    auto* owner = state.Owner();
    if ( !owner || state.updating || !state.view ) return;
    if ( !CanPresent(owner) )
    {
        if ( state.attached )
        {
            state.attached = false;
            state.host.ClearContent();
        }
        return;
    }
    if ( !state.dirty && state.attached ) { SynchronizeGeometry(); return; }
    state.dirty = false;
    state.updating = true;
    const auto mutation = wxMakeGuard([&state] { state.updating = false; });
    const auto revision = state.revision;
    struct Item
    {
        wxWeakRef<wxWindow> page;
        wxString caption, tooltip;
        wxBitmapBundle bitmap;
        wxAuiTabKind kind;
        bool active;
    };
    std::vector<Item> model;
    for ( const auto& info : owner->GetPages() )
        model.push_back({info.window, info.caption, info.tooltip,
                         info.bitmap, info.kind, info.active});
    const unsigned flags = owner->GetFlags();
    const auto current = [&]
    {
        auto* const live = state.Owner();
        if ( live != owner || state.revision != revision ||
             live->GetFlags() != flags || live->GetPageCount() != model.size() ) return false;
        for ( size_t i = 0; i < model.size(); ++i )
        {
            auto* const page = model[i].page.get();
            const auto& info = live->GetPages()[i];
            if ( !page || wxWindowIsUnavailableForCallbacks(page) ||
                 info.window != page || info.caption != model[i].caption ||
                 info.active != model[i].active || info.kind != model[i].kind ) return false;
        }
        return true;
    };
    try
    {
        std::vector<std::shared_ptr<Impl::Peer>> desired;
        for ( const auto& item : model )
        {
            auto found = std::find_if(state.peers.begin(), state.peers.end(),
                [&](const auto& peer) { return peer->page.get() == item.page.get(); });
            auto peer = found == state.peers.end() ? std::make_shared<Impl::Peer>() : *found;
            if ( !peer->tab )
            {
                peer->page = item.page;
                peer->tab = C::TabViewItem();
                A::AutomationProperties::SetAutomationId(peer->tab,
                    wxWinUIToHString(wxString::Format("wxAui.Tab.%d", item.page->GetId())));
            }
            if ( peer->caption != item.caption )
            {
                peer->caption = item.caption;
                peer->tab.Header(winrt::box_value(wxWinUIToHString(item.caption)));
                if ( !current() ) return;
                A::AutomationProperties::SetName(peer->tab, wxWinUIToHString(item.caption));
                if ( !current() ) return;
            }
            if ( peer->tooltip != item.tooltip )
            {
                peer->tooltip = item.tooltip;
                C::ToolTipService::SetToolTip(peer->tab,
                    winrt::box_value(wxWinUIToHString(item.tooltip)));
                if ( !current() ) return;
            }
            const wxBitmap bitmap = item.bitmap.IsOk()
                ? item.bitmap.GetBitmapFor(owner) : wxNullBitmap;
            if ( !current() ) return;
            if ( !bitmap.IsSameAs(peer->bitmap) )
            {
                peer->bitmap = bitmap;
                peer->icon = nullptr;
                if ( bitmap.IsOk() )
                {
                    peer->icon = C::ImageIconSource();
                    peer->icon.ImageSource(wxWinUIWriteableBitmapFromBitmap(bitmap));
                    if ( !current() ) return;
                }
                peer->tab.IconSource(peer->icon);
                if ( !current() ) return;
            }
            const bool closable = item.kind != wxAuiTabKind::Locked &&
                ((flags & wxAUI_NB_CLOSE_ON_ALL_TABS) ||
                 ((flags & wxAUI_NB_CLOSE_ON_ACTIVE_TAB) && item.active));
            if ( peer->tab.IsClosable() != closable ) peer->tab.IsClosable(closable);
            if ( !current() ) return;
            desired.push_back(std::move(peer));
        }
        auto nativeItems = state.view.TabItems();
        // Publish the logical peer roster first: SelectionChanged caused by
        // list edits is suppressed; stale retained providers no longer resolve.
        state.peers = desired;
        for ( uint32_t i = 0; i < desired.size(); ++i )
        {
            uint32_t old = 0;
            if ( nativeItems.IndexOf(desired[i]->tab, old) )
            {
                if ( old == i ) continue;
                nativeItems.RemoveAt(old);
                if ( !current() ) return;
            }
            nativeItems.InsertAt(i, desired[i]->tab);
            if ( !current() ) return;
        }
        while ( nativeItems.Size() > desired.size() )
        {
            nativeItems.RemoveAtEnd();
            if ( !current() ) return;
        }
        const int active = owner->GetActivePage();
        const auto selected = active >= 0 && size_t(active) < desired.size()
            ? desired[active]->tab : C::TabViewItem{nullptr};
        if ( state.view.SelectedItem() != selected )
        {
            ClearVisibilityRequest();
            state.view.SelectedItem(selected);
        }
        if ( !current() ) return;
        const auto widthMode = (flags & wxAUI_NB_TAB_FIXED_WIDTH)
            ? C::TabViewWidthMode::Equal : C::TabViewWidthMode::SizeToContent;
        if ( state.view.TabWidthMode() != widthMode ) state.view.TabWidthMode(widthMode);
        if ( !current() ) return;
        const auto closeVisible = (flags & wxAUI_NB_CLOSE_BUTTON)
            ? X::Visibility::Visible : X::Visibility::Collapsed;
        if ( state.closeButton.Visibility() != closeVisible ) state.closeButton.Visibility(closeVisible);
        if ( !current() ) return;
        const bool closeEnabled = active >= 0 && size_t(active) < model.size() &&
                                  model[active].kind != wxAuiTabKind::Locked;
        if ( state.closeButton.IsEnabled() != closeEnabled ) state.closeButton.IsEnabled(closeEnabled);
        if ( !current() ) return;
        const auto listVisible = (flags & wxAUI_NB_WINDOWLIST_BUTTON)
            ? X::Visibility::Visible : X::Visibility::Collapsed;
        if ( state.listButton.Visibility() != listVisible ) state.listButton.Visibility(listVisible);
        if ( !current() ) return;
        if ( !state.attached )
        {
            state.attached = state.host.SetContent(state.view);
            if ( !current() ) return;
        }
        if ( selected ) state.host.SetPreferredFocus(selected);
        if ( !current() ) return;
        SynchronizeGeometry();
    }
    catch ( const winrt::hresult_error& error )
    {
        state.dirty = true;
        wxWinUILogException("WinUI AUI TabView projection", error);
    }
}

void wxAuiTabCtrlWinUI::SynchronizeGeometry()
{
    const auto keepAlive = shared_from_this();
    auto& state = *m_impl;
    auto* const owner = state.Owner();
    if ( !owner || !IsActive() || state.geometry ) return;
    state.geometry = true;
    const auto guard = wxMakeGuard([&state] { state.geometry = false; });
    try
    {
        // Native item bounds, not art-provider measurements, drive AUI hit
        // tests and its existing captured drag/cross-pane docking machinery.
        const auto peers = state.peers;
        const auto revision = state.revision;
        const wxRect viewport = NativeRect(owner, TabViewport(state.view));
        if ( state.Owner() != owner || state.revision != revision ) return;
        size_t firstVisible = peers.size();
        for ( const auto& peer : peers )
        {
            auto* const page = peer->page.get();
            const int index = page ? owner->GetIdxFromWindow(page) : wxNOT_FOUND;
            if ( index == wxNOT_FOUND ) continue;
            const wxRect bounds = NativeRect(owner, peer->tab);
            if ( state.Owner() != owner || state.revision != revision ) return;
            const auto closePart = FindPart(peer->tab, L"CloseButton");
            const wxRect closeBounds = NativeRect(owner, closePart);
            if ( state.Owner() != owner || state.revision != revision ||
                 owner->GetIdxFromWindow(page) != index ) return;
            auto& info = owner->m_pages[size_t(index)];
            info.rect = bounds;
            if ( !bounds.IsEmpty() && bounds.Intersects(viewport) )
                firstVisible = wxMin(firstVisible, size_t(index));
            info.rowEnd = size_t(index) + 1 == owner->m_pages.size();
            for ( auto& button : info.buttons )
            {
                button.rect = button.id == wxAUI_BUTTON_CLOSE ? closeBounds : wxRect();
                if ( button.rect.IsEmpty() ) button.curState |= wxAUI_BUTTON_STATE_HIDDEN;
                else button.curState &= ~wxAUI_BUTTON_STATE_HIDDEN;
            }
        }
        owner->m_tabOffset = firstVisible < peers.size() ? firstVisible : 0;
        for ( size_t i = 0; i < owner->m_buttons.size(); ++i )
        {
            const int id = owner->m_buttons[i].id;
            const wxRect bounds = id == wxAUI_BUTTON_CLOSE
                ? NativeRect(owner, state.closeButton)
                : id == wxAUI_BUTTON_WINDOWLIST
                    ? NativeRect(owner, state.listButton) : wxRect();
            if ( state.Owner() != owner || state.revision != revision ||
                 i >= owner->m_buttons.size() || owner->m_buttons[i].id != id ) return;
            auto& button = owner->m_buttons[i];
            button.rect = bounds;
            if ( button.rect.IsEmpty() ) button.curState |= wxAUI_BUTTON_STATE_HIDDEN;
            else button.curState &= ~wxAUI_BUTTON_STATE_HIDDEN;
        }
    }
    catch ( const winrt::hresult_error& error )
    {
        wxWinUILogException("WinUI AUI TabView geometry", error);
    }
}

void wxAuiTabCtrlWinUI::ClearVisibilityRequest()
{
    auto& state = *m_impl;
    ++state.revealSerial;
    state.revealPeer.reset();
    state.revealQueued = false;
    state.revealAttempts = 0;
    const auto tab = state.scrollGuard;
    const auto token = state.bring;
    state.scrollGuard = nullptr;
    state.bring = {};
    if ( tab && token.value )
        try { tab.BringIntoViewRequested(token); } catch ( ... ) {}
}

void wxAuiTabCtrlWinUI::ContinueVisibilityRequest()
{
    auto& state = *m_impl;
    if ( state.applyingReveal || state.revealQueued || !state.revealPeer ||
         !IsActive() ) return;
    if ( state.revealAttempts >= 4 )
    {
        ClearVisibilityRequest();
        return;
    }
    const auto peer = state.revealPeer;
    const auto serial = state.revealSerial;
    const auto* const page = peer->page.get();
    const int index = state.Owner()->GetIdxFromWindow(page);
    if ( index == wxNOT_FOUND || size_t(index) >= state.peers.size() ||
         state.peers[size_t(index)] != peer )
    {
        ClearVisibilityRequest();
        return;
    }
    const bool visible = peer->tab.IsLoaded() && IsTabVisible(index);
    if ( !IsActive() || state.revealSerial != serial || state.revealPeer != peer ) return;
    if ( visible )
    {
        ClearVisibilityRequest();
        return;
    }
    if ( !IsActive() ) return;
    state.revealQueued = true;
    state.Owner()->CallAfter([weak = weak_from_this(), serial, peer]
    {
        const auto self = weak.lock();
        if ( !self || !self->IsActive() ) return;
        auto& live = *self->m_impl;
        if ( live.revealSerial != serial || live.revealPeer != peer ) return;
        live.revealQueued = false;
        const int index = live.Owner()->GetIdxFromWindow(peer->page.get());
        if ( index == wxNOT_FOUND ) { self->ClearVisibilityRequest(); return; }
        ++live.revealAttempts;
        live.retryingReveal = true;
        const auto guard = wxMakeGuard([&live] { live.retryingReveal = false; });
        self->ShowTab(index);
    });
}

void wxAuiTabCtrlWinUI::ShowTab(int index)
{
    const auto keepAlive = shared_from_this();
    auto& state = *m_impl;
    // Consume this internal continuation marker before any native boundary.
    // An application ShowTab re-entered by layout is a NEW request and must
    // invalidate the older transaction, not inherit its continuation status.
    const bool continuing = state.retryingReveal;
    state.retryingReveal = false;
    const auto previousSerial = state.revealSerial;
    Update();
    if ( state.revealSerial != previousSerial ) return;
    if ( !IsActive() || index < 0 || size_t(index) >= state.peers.size() ) return;
    const auto peer = state.peers[size_t(index)];
    if ( !continuing ) ClearVisibilityRequest();
    state.revealPeer = peer;
    const bool wasApplying = state.applyingReveal;
    state.applyingReveal = true;
    const auto applying = wxMakeGuard([&state, wasApplying] { state.applyingReveal = wasApplying; });
    const auto serial = state.revealSerial;
    const auto current = [&]
    {
        auto* const owner = state.Owner();
        return owner && IsActive() && state.revealSerial == serial &&
               state.revealPeer == peer && peer->page &&
               owner->GetIdxFromWindow(peer->page.get()) == index &&
               size_t(index) < state.peers.size() && state.peers[size_t(index)] == peer;
    };
    try
    {
        // Match wxNotebook's native visibility transaction: attach/layout the
        // island before looking for the virtualized header strip, then commit
        // its layout before returning the synchronous wx visibility result.
        state.host.ForceRender();
        if ( !current() ) return;
        state.view.ApplyTemplate();
        state.view.UpdateLayout();
        if ( !current() ) return;
        const auto list = FindTemplateControl<C::ListViewBase>(state.view);
        if ( !list ) return;
        if ( state.scrollGuard )
        {
            state.scrollGuard.BringIntoViewRequested(state.bring);
            state.scrollGuard = nullptr;
            state.bring = {};
        }
        const auto selected = state.view.SelectedItem().try_as<C::TabViewItem>();
        if ( selected && selected != peer->tab )
        {
            // TabView brings its selected item back on ItemsPresenter resize.
            // While this explicit reveal settles, redirect that request to
            // the requested header without changing notebook selection.
            state.scrollGuard = selected;
            state.bring = selected.BringIntoViewRequested(
                [weak = weak_from_this(), target = std::weak_ptr<Impl::Peer>(peer), serial]
                (const auto&, const X::BringIntoViewRequestedEventArgs& args)
                {
                    const auto self = weak.lock();
                    const auto requested = target.lock();
                    if ( !self || !requested || !self->IsActive() ) return;
                    auto& live = *self->m_impl;
                    if ( live.revealSerial != serial || live.revealPeer != requested ||
                         !requested->page || wxWindowIsUnavailableForCallbacks(requested->page.get()) ) return;
                    const int index = live.Owner()->GetIdxFromWindow(requested->page.get());
                    if ( index == wxNOT_FOUND || size_t(index) >= live.peers.size() ||
                         live.peers[size_t(index)] != requested ) return;
                    args.TargetElement(requested->tab);
                    const auto size = requested->tab.DesiredSize();
                    args.TargetRect({0, 0, size.Width, size.Height});
                    args.AnimationDesired(false);
                });
        }
        // ScrollIntoView handles virtualized items and RTL using the stock
        // TabView's own ItemsStackPanel/ScrollViewer rather than GDI widths.
        list.ScrollIntoView(peer->tab, C::ScrollIntoViewAlignment::Leading);
        if ( !current() ) return;
        list.UpdateLayout();
        if ( !current() ) return;
        state.view.UpdateLayout();
        // Virtualized widths can settle after this synchronous layout. The
        // LayoutUpdated continuation re-applies this exact request, bounded
        // and superseded by new selection, input, focus or page identity.
        if ( current() ) SynchronizeGeometry();
    }
    catch ( const winrt::hresult_error& error )
    {
        wxWinUILogException("WinUI AUI TabView visibility", error);
    }
}

bool wxAuiTabCtrlWinUI::IsTabVisible(int index)
{
    const auto keepAlive = shared_from_this();
    auto& state = *m_impl;
    auto* const owner = state.Owner();
    if ( !owner || !IsActive() || index < 0 || size_t(index) >= state.peers.size() ) return false;
    const auto peer = state.peers[size_t(index)];
    try
    {
        const wxRect rect = NativeRect(owner, peer->tab);
        if ( state.Owner() != owner ) return false;
        const wxRect viewport = NativeRect(owner, TabViewport(state.view));
        if ( state.Owner() != owner || !peer->page ||
             owner->GetIdxFromWindow(peer->page.get()) != index ) return false;
        const wxRect visible = rect.Intersect(viewport);
        return !rect.IsEmpty() && !viewport.IsEmpty() &&
               visible.width + 1 >= wxMin(rect.width, viewport.width) &&
               visible.height + 1 >= wxMin(rect.height, viewport.height);
    }
    catch ( const winrt::hresult_error& error )
    {
        wxWinUILogException("WinUI AUI TabView visible bounds", error);
        return false;
    }
}

#endif // __WXWINUI__ && wxUSE_WINUI3 && wxUSE_AUI
