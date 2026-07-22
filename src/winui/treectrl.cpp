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

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/settings.h"
    #include "wx/textctrl.h"
    #include "wx/utils.h"
#endif

#include "private.h"

#include "wx/msw/private/keyboard.h"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <memory>
#include <vector>

#include <winrt/Microsoft.UI.Dispatching.h>
#include <winrt/Microsoft.UI.Xaml.Automation.h>
#include <winrt/Microsoft.UI.Xaml.Controls.Primitives.h>
#include <winrt/Microsoft.UI.Xaml.Media.h>

namespace MUX = winrt::Microsoft::UI::Xaml;
namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;
namespace WFC = winrt::Windows::Foundation::Collections;

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
    MUXC::TreeViewNode node{ nullptr };
};

class wxWinUITreeCtrlImpl
{
public:
    wxWinUIControlHost host;
    MUXC::TreeView treeView{ nullptr };
    MUXC::TreeViewList treeList{ nullptr };
    winrt::event_token selectionChangedToken{};
    winrt::event_token listHookLoadedToken{};
    winrt::event_token containerContentChangingToken{};
    winrt::event_token expandingToken{};
    winrt::event_token collapsedToken{};
    winrt::event_token itemInvokedToken{};
    winrt::event_token rightTappedToken{};
    winrt::event_token keyDownToken{};
    winrt::event_token dragStartingToken{};
    winrt::event_token dragCompletedToken{};

    std::unique_ptr<wxWinUITreeItem> root;
    wxWinUITreeItem *selection = nullptr;
    wxWinUITreeItem *focused = nullptr;
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

// Name every already-realized container from its item text.
static void wxWinUINameRealizedTreeItems(wxWinUITreeCtrlImpl *impl)
{
    if ( !impl->treeList )
        return;

    const auto panel = impl->treeList.ItemsPanelRoot();
    if ( !panel )
        return;

    using winrt::Microsoft::UI::Xaml::Automation::AutomationProperties;

    const auto children = panel.Children();
    for ( uint32_t i = 0; i < children.Size(); ++i )
    {
        if ( auto item = children.GetAt(i).try_as<MUXC::TreeViewItem>() )
        {
            AutomationProperties::SetName(item,
                wxWinUITreeNodeName(impl->treeView.NodeFromContainer(item)));
        }
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
            [](MUXC::ListViewBase const&,
               MUXC::ContainerContentChangingEventArgs const& args)
            {
                const auto container = args.ItemContainer();
                if ( !container )
                    return;

                using winrt::Microsoft::UI::Xaml::Automation::
                    AutomationProperties;
                AutomationProperties::SetName(
                    container,
                    wxWinUITreeNodeName(
                        args.Item().try_as<MUXC::TreeViewNode>()));
            });

    wxWinUINameRealizedTreeItems(impl);
}

static wxWinUITreeItem *wxWinUIFindItemByNode(wxWinUITreeItem *item,
                                              const MUXC::TreeViewNode& node)
{
    if ( !item || !node )
        return nullptr;

    if ( item->node && winrt::get_abi(item->node) == winrt::get_abi(node) )
        return item;

    for ( const auto& child : item->children )
    {
        if ( wxWinUITreeItem *found = wxWinUIFindItemByNode(child.get(), node) )
            return found;
    }

    return nullptr;
}

static size_t wxWinUIGetSubtreeCount(const wxWinUITreeItem *item)
{
    size_t count = 0;
    for ( const auto& child : item->children )
        count += 1 + wxWinUIGetSubtreeCount(child.get());
    return count;
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

static void wxWinUIRemovePeerNode(const wxTreeCtrl *tree,
                                  wxWinUITreeCtrlImpl *impl,
                                  wxWinUITreeItem *item)
{
    if ( !impl->treeView || !item || !item->node )
        return;

    auto nodes = wxWinUIGetPeerChildren(tree, impl, item->parent);
    for ( uint32_t i = 0; i < nodes.Size(); ++i )
    {
        if ( winrt::get_abi(nodes.GetAt(i)) == winrt::get_abi(item->node) )
        {
            nodes.RemoveAt(i);
            return;
        }
    }
}

static bool wxWinUIItemIsVisible(const wxTreeCtrl *tree,
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

static wxWinUITreeItem *wxWinUIGetNextSiblingItem(wxWinUITreeItem *item)
{
    if ( !item || !item->parent )
        return nullptr;

    auto& siblings = item->parent->children;
    for ( size_t i = 0; i + 1 < siblings.size(); ++i )
    {
        if ( siblings[i].get() == item )
            return siblings[i + 1].get();
    }

    return nullptr;
}

static wxWinUITreeItem *wxWinUIGetPrevSiblingItem(wxWinUITreeItem *item)
{
    if ( !item || !item->parent )
        return nullptr;

    auto& siblings = item->parent->children;
    for ( size_t i = 1; i < siblings.size(); ++i )
    {
        if ( siblings[i].get() == item )
            return siblings[i - 1].get();
    }

    return nullptr;
}

static wxWinUITreeItem *wxWinUIGetDeepestVisibleChild(wxWinUITreeItem *item)
{
    while ( item && item->expanded && !item->children.empty() )
        item = item->children.back().get();

    return item;
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

wxTreeCtrl::~wxTreeCtrl() = default;

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
    if ( !m_winui->host.Initialize(this) )
        return false;

    try
    {
        m_winui->treeView = MUXC::TreeView();
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
            LR"(<DataTemplate xmlns="http://schemas.microsoft.com/winfx/2006/xaml/presentation">
                    <StackPanel Orientation="Horizontal" Spacing="8"
                                Margin="{Binding Content[ContentMargin]}">
                        <Image Width="16" Height="16"
                               VerticalAlignment="Center"
                               Visibility="{Binding Content[ImageVisibility]}"
                               Source="{Binding Content[Image]}"/>
                        <TextBlock VerticalAlignment="Center"
                                   Text="{Binding Content[Text]}"/>
                    </StackPanel>
                </DataTemplate>)";
        m_winui->treeView.ItemTemplate(
            winrt::Microsoft::UI::Xaml::Markup::XamlReader::Load(itemTemplate)
                .as<MUX::DataTemplate>());

        // Hook the container-naming pipeline before anything can be
        // realized: force the template (the inner TreeViewList only exists
        // once it is applied), and keep a Loaded retry for the case where
        // the template really cannot resolve this early.
        m_winui->treeView.ApplyTemplate();
        wxWinUIHookTreeListNaming(m_winui.get());
        if ( !m_winui->treeList )
        {
            m_winui->listHookLoadedToken = m_winui->treeView.Loaded(
                [this](winrt::Windows::Foundation::IInspectable const&,
                       MUX::RoutedEventArgs const&)
                {
                    if ( m_winui && m_winui->treeView )
                        wxWinUIHookTreeListNaming(m_winui.get());
                });
        }

        m_winui->selectionChangedToken = m_winui->treeView.SelectionChanged(
            [this](MUXC::TreeView const&,
                   MUXC::TreeViewSelectionChangedEventArgs const&)
            {
                if ( !m_winui || m_updatingPeer )
                    return;
                OnPeerSelectionChanged();
            });

        m_winui->expandingToken = m_winui->treeView.Expanding(
            [this](MUXC::TreeView const&,
                   MUXC::TreeViewExpandingEventArgs const& event)
            {
                if ( !m_winui || m_updatingPeer )
                    return;
                OnPeerNodeExpanded(wxWinUIFindItemByNode(m_winui->root.get(),
                                                         event.Node()));
            });

        m_winui->collapsedToken = m_winui->treeView.Collapsed(
            [this](MUXC::TreeView const&,
                   MUXC::TreeViewCollapsedEventArgs const& event)
            {
                if ( !m_winui || m_updatingPeer )
                    return;
                OnPeerNodeCollapsed(wxWinUIFindItemByNode(m_winui->root.get(),
                                                          event.Node()));
            });

        m_winui->itemInvokedToken = m_winui->treeView.ItemInvoked(
            [this](MUXC::TreeView const&,
                   MUXC::TreeViewItemInvokedEventArgs const&)
            {
                if ( m_winui && m_winui->selection )
                    SendTreeEvent(wxEVT_TREE_ITEM_ACTIVATED, m_winui->selection);
            });

        // Item dragging: wx semantics are that the application performs the
        // actual move itself from the END_DRAG handler, so keep the WinUI
        // automatic reordering off and only report the gesture.
        m_winui->treeView.CanDragItems(true);
        m_winui->treeView.CanReorderItems(false);

        m_winui->dragStartingToken = m_winui->treeView.DragItemsStarting(
            [this](MUXC::TreeView const&,
                   MUXC::TreeViewDragItemsStartingEventArgs const& args)
            {
                if ( !m_winui )
                    return;

                wxWinUITreeItem *item = m_winui->selection;
                wxTreeEvent event(wxEVT_TREE_BEGIN_DRAG, this, MakeId(item));
                event.SetPoint(ScreenToClient(wxGetMousePosition()));

                // As in the other ports, dragging must be explicitly allowed
                // by the handler calling event.Allow().
                if ( GetEventHandler()->ProcessEvent(event) && event.IsAllowed() )
                    m_dragItem = item;
                else
                    args.Cancel(true);
            });

        m_winui->dragCompletedToken = m_winui->treeView.DragItemsCompleted(
            [this](MUXC::TreeView const&,
                   MUXC::TreeViewDragItemsCompletedEventArgs const&)
            {
                if ( !m_winui || !m_dragItem )
                    return;

                m_dragItem = nullptr;

                const wxPoint pt = ScreenToClient(wxGetMousePosition());
                int flags = 0;
                const wxTreeItemId target = DoTreeHitTest(pt, flags);

                wxTreeEvent event(wxEVT_TREE_END_DRAG, this, target);
                event.SetPoint(pt);
                GetEventHandler()->ProcessEvent(event);
            });

        m_winui->keyDownToken = m_winui->treeView.KeyDown(
            [this](winrt::Windows::Foundation::IInspectable const&,
                   MUX::Input::KeyRoutedEventArgs const& event)
            {
                if ( !m_winui )
                    return;

                wchar_t uc = 0;
                const int code = wxMSWKeyboard::VKToWX(
                    static_cast<WXWORD>(event.Key()), 0, &uc);
                if ( code == WXK_NONE )
                    return;

                wxKeyEvent keyEvent(wxEVT_KEY_DOWN);
                keyEvent.m_keyCode = code;
#if wxUSE_UNICODE
                keyEvent.m_uniChar = uc;
#endif
                keyEvent.m_shiftDown = (::GetKeyState(VK_SHIFT) & 0x8000) != 0;
                keyEvent.m_controlDown = (::GetKeyState(VK_CONTROL) & 0x8000) != 0;
                keyEvent.m_altDown = (::GetKeyState(VK_MENU) & 0x8000) != 0;
                keyEvent.SetEventObject(this);
                keyEvent.SetId(GetId());

                wxTreeEvent treeEvent(wxEVT_TREE_KEY_DOWN, this);
                treeEvent.SetKeyEvent(keyEvent);
                GetEventHandler()->ProcessEvent(treeEvent);
            });

        m_winui->rightTappedToken = m_winui->treeView.RightTapped(
            [this](winrt::Windows::Foundation::IInspectable const&,
                   MUX::Input::RightTappedRoutedEventArgs const& event)
            {
                if ( !m_winui )
                    return;

                const auto pos = event.GetPosition(m_winui->treeView);
                OnPeerRightTapped(FromDIP(wxPoint(
                    static_cast<int>(std::lround(pos.X)),
                    static_cast<int>(std::lround(pos.Y)))));
            });

        m_winui->host.SetContent(m_winui->treeView);
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI TreeView creation", e);
        return false;
    }

    SetInitialSize(size);
    return true;
}

unsigned int wxTreeCtrl::GetCount() const
{
    if ( !m_winui || !m_winui->root )
        return 0;

    const size_t count = wxWinUIGetSubtreeCount(m_winui->root.get());
    return static_cast<unsigned int>(HasFlag(wxTR_HIDE_ROOT) ? count : count + 1);
}

unsigned int wxTreeCtrl::GetIndent() const
{
    return m_indent;
}

void wxTreeCtrl::SetIndent(unsigned int indent)
{
    m_indent = indent;
}

void wxTreeCtrl::SetStateImages(const wxVector<wxBitmapBundle>& images)
{
    m_imagesState.SetImages(images);
    RefreshPeerItems();
}

void wxTreeCtrl::SetImageList(wxImageList *imageList)
{
    wxWithImages::SetImageList(imageList);
    RefreshPeerItems();
}

void wxTreeCtrl::SetStateImageList(wxImageList *imageList)
{
    m_imagesState.SetImageList(imageList);
    RefreshPeerItems();
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
    treeItem->text = text;
    UpdatePeerItem(treeItem);
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

    delete treeItem->data;
    treeItem->data = data;
    if ( data )
        data->SetId(item);
}

void wxTreeCtrl::SetItemHasChildren(const wxTreeItemId& item, bool has)
{
    wxWinUITreeItem *treeItem = GetItem(item);
    wxCHECK_RET( treeItem, wxT("invalid tree item") );
    treeItem->hasChildrenOverride = has;
    if ( treeItem->node )
        treeItem->node.HasUnrealizedChildren(has && treeItem->children.empty());
}

void wxTreeCtrl::SetItemBold(const wxTreeItemId& item, bool bold)
{
    wxWinUITreeItem *treeItem = GetItem(item);
    wxCHECK_RET( treeItem, wxT("invalid tree item") );
    treeItem->bold = bold;
    UpdatePeerItem(treeItem);
}

void wxTreeCtrl::SetItemDropHighlight(const wxTreeItemId& WXUNUSED(item),
                                      bool WXUNUSED(highlight))
{
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
    return wxWinUIItemIsVisible(this, GetItem(item));
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
    return m_winui ? MakeId(m_winui->selection) : wxTreeItemId();
}

size_t wxTreeCtrl::GetSelections(wxArrayTreeItemIds& selections) const
{
    selections.Empty();

    if ( !m_winui || !m_winui->root )
        return 0;

    std::vector<wxWinUITreeItem *> stack;
    stack.push_back(m_winui->root.get());
    while ( !stack.empty() )
    {
        wxWinUITreeItem *item = stack.back();
        stack.pop_back();

        if ( item->selected )
            selections.Add(MakeId(item));

        for ( const auto& child : item->children )
            stack.push_back(child.get());
    }

    return selections.GetCount();
}

wxTreeItemId wxTreeCtrl::GetFocusedItem() const
{
    return m_winui ? MakeId(m_winui->focused) : wxTreeItemId();
}

void wxTreeCtrl::ClearFocusedItem()
{
    if ( m_winui )
        m_winui->focused = nullptr;
}

void wxTreeCtrl::SetFocusedItem(const wxTreeItemId& item)
{
    if ( m_winui )
        m_winui->focused = GetItem(item);
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

    if ( HasFlag(wxTR_HIDE_ROOT) )
    {
        return m_winui->root->children.empty()
            ? wxTreeItemId()
            : MakeId(m_winui->root->children.front().get());
    }

    return MakeId(m_winui->root.get());
}

wxTreeItemId wxTreeCtrl::GetNextVisible(const wxTreeItemId& item) const
{
    wxWinUITreeItem *treeItem = GetItem(item);
    if ( !treeItem )
        return wxTreeItemId();

    if ( treeItem->expanded && !treeItem->children.empty() )
        return MakeId(treeItem->children.front().get());

    while ( treeItem )
    {
        if ( wxWinUITreeItem *next = wxWinUIGetNextSiblingItem(treeItem) )
            return MakeId(next);

        treeItem = treeItem->parent;
        if ( treeItem && !treeItem->parent && HasFlag(wxTR_HIDE_ROOT) )
            return wxTreeItemId();
    }

    return wxTreeItemId();
}

wxTreeItemId wxTreeCtrl::GetPrevVisible(const wxTreeItemId& item) const
{
    wxWinUITreeItem *treeItem = GetItem(item);
    if ( !treeItem )
        return wxTreeItemId();

    if ( wxWinUITreeItem *prev = wxWinUIGetPrevSiblingItem(treeItem) )
        return MakeId(wxWinUIGetDeepestVisibleChild(prev));

    if ( treeItem->parent && (!HasFlag(wxTR_HIDE_ROOT) || treeItem->parent->parent) )
        return MakeId(treeItem->parent);

    return wxTreeItemId();
}

wxTreeItemId wxTreeCtrl::AddRoot(const wxString& text,
                                 int image,
                                 int selectedImage,
                                 wxTreeItemData *data)
{
    DeleteAllItems();

    m_winui->root.reset(new wxWinUITreeItem);
    m_winui->root->text = text;
    m_winui->root->images[wxTreeItemIcon_Normal] = image;
    m_winui->root->images[wxTreeItemIcon_Selected] = selectedImage;
    m_winui->root->data = data;
    m_winui->root->node = MUXC::TreeViewNode();
    if ( data )
        data->SetId(MakeId(m_winui->root.get()));

    UpdatePeerItem(m_winui->root.get());

    if ( !HasFlag(wxTR_HIDE_ROOT) )
    {
        m_updatingPeer = true;
        m_winui->treeView.RootNodes().Append(m_winui->root->node);
        m_updatingPeer = false;
    }

    return MakeId(m_winui->root.get());
}

void wxTreeCtrl::Delete(const wxTreeItemId& item)
{
    wxWinUITreeItem *treeItem = GetItem(item);
    wxCHECK_RET( treeItem, wxT("invalid tree item") );

    if ( treeItem == m_winui->root.get() )
    {
        DeleteAllItems();
        return;
    }

    if ( m_editItem && wxWinUIIsDescendantOf(m_editItem, treeItem) )
        EndEditLabel(MakeId(m_editItem), true);

    if ( m_winui->selection && wxWinUIIsDescendantOf(m_winui->selection, treeItem) )
        ChangeSelection(nullptr, false, true);

    wxWinUIRemovePeerNode(this, m_winui.get(), treeItem);

    auto& siblings = treeItem->parent->children;
    siblings.erase(std::remove_if(siblings.begin(), siblings.end(),
        [treeItem](const std::unique_ptr<wxWinUITreeItem>& child)
        {
            return child.get() == treeItem;
        }), siblings.end());

    m_winui->host.ForceRender();
}

void wxTreeCtrl::DeleteChildren(const wxTreeItemId& item)
{
    wxWinUITreeItem *treeItem = GetItem(item);
    wxCHECK_RET( treeItem, wxT("invalid tree item") );

    if ( m_editItem && wxWinUIIsDescendantOf(m_editItem, treeItem) &&
         m_editItem != treeItem )
    {
        EndEditLabel(MakeId(m_editItem), true);
    }

    if ( m_winui->selection && wxWinUIIsDescendantOf(m_winui->selection, treeItem) &&
         m_winui->selection != treeItem )
    {
        ChangeSelection(nullptr, false, true);
    }

    if ( treeItem->node )
    {
        if ( treeItem == m_winui->root.get() && HasFlag(wxTR_HIDE_ROOT) )
            m_winui->treeView.RootNodes().Clear();
        else
            treeItem->node.Children().Clear();
    }

    treeItem->children.clear();
    treeItem->expanded = false;
    m_winui->host.ForceRender();
}

void wxTreeCtrl::DeleteAllItems()
{
    if ( !m_winui )
        return;

    if ( m_editControl )
        EndEditLabel(MakeId(m_editItem), true);

    m_updatingPeer = true;
    if ( m_winui->treeView )
        m_winui->treeView.RootNodes().Clear();
    m_winui->root.reset();
    m_winui->selection = nullptr;
    m_winui->focused = nullptr;
    m_updatingPeer = false;
}

void wxTreeCtrl::Expand(const wxTreeItemId& item)
{
    wxWinUITreeItem *treeItem = GetItem(item);
    wxCHECK_RET( treeItem, wxT("invalid tree item") );
    wxCHECK_RET( !HasFlag(wxTR_HIDE_ROOT) || treeItem != m_winui->root.get(),
                 wxT("can't expand hidden root") );

    if ( treeItem->expanded || !ItemHasChildren(item) )
        return;

    SendTreeEvent(wxEVT_TREE_ITEM_EXPANDING, treeItem);
    treeItem->expanded = true;

    m_updatingPeer = true;
    try
    {
        treeItem->node.IsExpanded(true);
    }
    catch ( const winrt::hresult_error& )
    {
    }
    m_updatingPeer = false;

    if ( !treeItem->children.empty() )
        SendTreeEvent(wxEVT_TREE_ITEM_EXPANDED, treeItem);
}

void wxTreeCtrl::Collapse(const wxTreeItemId& item)
{
    wxWinUITreeItem *treeItem = GetItem(item);
    wxCHECK_RET( treeItem, wxT("invalid tree item") );
    wxCHECK_RET( !HasFlag(wxTR_HIDE_ROOT) || treeItem != m_winui->root.get(),
                 wxT("can't collapse hidden root") );

    if ( !treeItem->expanded )
        return;

    SendTreeEvent(wxEVT_TREE_ITEM_COLLAPSING, treeItem);
    treeItem->expanded = false;

    m_updatingPeer = true;
    try
    {
        treeItem->node.IsExpanded(false);
    }
    catch ( const winrt::hresult_error& )
    {
    }
    m_updatingPeer = false;

    SendTreeEvent(wxEVT_TREE_ITEM_COLLAPSED, treeItem);
}

void wxTreeCtrl::CollapseAndReset(const wxTreeItemId& item)
{
    Collapse(item);
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
    if ( m_winui && m_winui->selection )
        SelectItem(MakeId(m_winui->selection), false);
}

void wxTreeCtrl::UnselectAll()
{
    if ( !m_winui || !m_winui->root )
        return;

    std::vector<wxWinUITreeItem *> stack;
    stack.push_back(m_winui->root.get());
    while ( !stack.empty() )
    {
        wxWinUITreeItem *item = stack.back();
        stack.pop_back();
        item->selected = false;
        for ( const auto& child : item->children )
            stack.push_back(child.get());
    }

    m_winui->selection = nullptr;
    ApplySelectionToPeer();
}

void wxTreeCtrl::SelectItem(const wxTreeItemId& item, bool select)
{
    wxWinUITreeItem *treeItem = GetItem(item);
    wxCHECK_RET( treeItem || !select, wxT("invalid tree item") );

    if ( select )
        ChangeSelection(treeItem, true, true);
    else if ( treeItem && treeItem->selected )
    {
        if ( IsSelectionChangeAllowed(nullptr, treeItem) )
        {
            treeItem->selected = false;
            if ( m_winui->selection == treeItem )
                m_winui->selection = nullptr;
            ApplySelectionToPeer();
            SendTreeEvent(wxEVT_TREE_SEL_CHANGED, treeItem);
        }
    }
}

void wxTreeCtrl::SelectChildren(const wxTreeItemId& parent)
{
    wxWinUITreeItem *treeItem = GetItem(parent);
    wxCHECK_RET( treeItem, wxT("invalid tree item") );

    for ( const auto& child : treeItem->children )
        child->selected = true;

    if ( !treeItem->children.empty() )
        m_winui->selection = treeItem->children.front().get();

    ApplySelectionToPeer();
}

void wxTreeCtrl::EnsureVisible(const wxTreeItemId& item)
{
    wxWinUITreeItem *treeItem = GetItem(item);
    if ( !treeItem )
        return;

    for ( wxWinUITreeItem *parent = treeItem->parent; parent; parent = parent->parent )
    {
        if ( parent->parent || !HasFlag(wxTR_HIDE_ROOT) )
        {
            parent->expanded = true;
            try
            {
                parent->node.IsExpanded(true);
            }
            catch ( const winrt::hresult_error& )
            {
            }
        }
    }

    ApplySelectionToPeer();
}

void wxTreeCtrl::ScrollTo(const wxTreeItemId& item)
{
    EnsureVisible(item);
}

wxTextCtrl *wxTreeCtrl::EditLabel(const wxTreeItemId& item,
                                  wxClassInfo* textCtrlClass)
{
    wxWinUITreeItem *treeItem = GetItem(item);
    wxCHECK_MSG( treeItem, nullptr, wxT("invalid tree item") );

    // Only one edit at a time.
    if ( m_editControl )
        EndEditLabel(MakeId(m_editItem), true);

    wxTreeEvent event(wxEVT_TREE_BEGIN_LABEL_EDIT, this, item);
    if ( GetEventHandler()->ProcessEvent(event) && !event.IsAllowed() )
        return nullptr;

    wxRect rect;
    if ( !GetBoundingRect(item, rect, true) )
        rect = wxRect(0, 0, GetClientSize().x, FromDIP(32));

    // The edit control is a plain wx child of the tree: its HWND (and island)
    // paints above the TreeView island, which is pinned to the bottom.
    wxTextCtrl *text = wxStaticCast(textCtrlClass->CreateObject(), wxTextCtrl);
    if ( !text->Create(this, wxID_ANY, treeItem->text,
                       rect.GetPosition(),
                       wxSize(wxMax(rect.width, FromDIP(80)), rect.height),
                       wxTE_PROCESS_ENTER) )
    {
        delete text;
        return nullptr;
    }

    m_editControl = text;
    m_editItem = treeItem;

    text->SelectAll();
    text->SetFocus();

    text->Bind(wxEVT_TEXT_ENTER,
        [this](wxCommandEvent&)
        {
            EndEditLabel(MakeId(m_editItem), false);
        });
    text->Bind(wxEVT_KILL_FOCUS,
        [this](wxFocusEvent& e)
        {
            e.Skip();
            EndEditLabel(MakeId(m_editItem), false);
        });
    text->Bind(wxEVT_CHAR_HOOK,
        [this](wxKeyEvent& e)
        {
            if ( e.GetKeyCode() == WXK_ESCAPE )
                EndEditLabel(MakeId(m_editItem), true);
            else
                e.Skip();
        });

    return text;
}

wxTextCtrl *wxTreeCtrl::GetEditControl() const
{
    return m_editControl;
}

void wxTreeCtrl::EndEditLabel(const wxTreeItemId& WXUNUSED(item),
                              bool discardChanges)
{
    if ( !m_editControl )
        return;

    wxTextCtrl * const text = m_editControl;
    wxWinUITreeItem * const treeItem = m_editItem;
    m_editControl = nullptr;
    m_editItem = nullptr;

    const wxString value = text->GetValue();

    // We may be called from one of the edit control's own event handlers, so
    // don't delete it right away.
    text->Hide();
    wxTheApp->ScheduleForDestruction(text);

    wxTreeEvent event(wxEVT_TREE_END_LABEL_EDIT, this, MakeId(treeItem));
    event.SetLabel(value);
    event.SetEditCanceled(discardChanges);

    const bool processed = GetEventHandler()->ProcessEvent(event);
    if ( !discardChanges && (!processed || event.IsAllowed()) )
        SetItemText(MakeId(treeItem), value);

    SetFocus();
}

void wxTreeCtrl::SortChildren(const wxTreeItemId& item)
{
    wxWinUITreeItem *treeItem = GetItem(item);
    wxCHECK_RET( treeItem, wxT("invalid tree item") );

    std::sort(treeItem->children.begin(), treeItem->children.end(),
        [this](const std::unique_ptr<wxWinUITreeItem>& a,
               const std::unique_ptr<wxWinUITreeItem>& b)
        {
            return OnCompareItems(MakeId(a.get()), MakeId(b.get())) < 0;
        });

    RefreshPeerItems();
}

bool wxTreeCtrl::HasExpandableItem() const
{
    if ( !m_winui || !m_winui->root )
        return false;

    // The hidden root doesn't own an expander of its own, so start from its
    // children in that case.
    std::vector<const wxWinUITreeItem *> stack;
    if ( HasFlag(wxTR_HIDE_ROOT) )
    {
        for ( const auto& child : m_winui->root->children )
            stack.push_back(child.get());
    }
    else
    {
        stack.push_back(m_winui->root.get());
    }

    while ( !stack.empty() )
    {
        const wxWinUITreeItem *item = stack.back();
        stack.pop_back();

        if ( !item->children.empty() || item->hasChildrenOverride )
            return true;

        for ( const auto& child : item->children )
            stack.push_back(child.get());
    }

    return false;
}

bool wxTreeCtrl::GetItemPeerRect(wxWinUITreeItem *item, wxRect& rect) const
{
    if ( !m_winui || !m_winui->treeView || !item || !item->node )
        return false;

    try
    {
        const auto container = m_winui->treeView.ContainerFromNode(item->node);
        const auto element = container.try_as<MUX::FrameworkElement>();
        if ( !element || element.ActualHeight() <= 0 )
            return false;

        const auto transform = element.TransformToVisual(m_winui->treeView);
        const auto origin =
            transform.TransformPoint(winrt::Windows::Foundation::Point{ 0, 0 });

        rect = wxRect(FromDIP(wxPoint(
                          static_cast<int>(std::lround(origin.X)),
                          static_cast<int>(std::lround(origin.Y)))),
                      FromDIP(wxSize(
                          static_cast<int>(std::lround(element.ActualWidth())),
                          static_cast<int>(std::lround(element.ActualHeight())))));
        return true;
    }
    catch ( const winrt::hresult_error& )
    {
        return false;
    }
}

bool wxTreeCtrl::GetBoundingRect(const wxTreeItemId& item,
                                 wxRect& rect,
                                 bool WXUNUSED(textOnly)) const
{
    wxWinUITreeItem *treeItem = GetItem(item);
    if ( !treeItem )
        return false;

    if ( GetItemPeerRect(treeItem, rect) )
        return true;

    // The container isn't realized: fall back to an estimate.
    rect = wxRect(wxPoint(0, 0), wxSize(GetClientSize().x, FromDIP(32)));
    return true;
}

wxVisualAttributes
wxTreeCtrl::GetClassDefaultAttributes(wxWindowVariant WXUNUSED(variant))
{
    wxVisualAttributes attrs;
    attrs.colFg = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT);
    attrs.colBg = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW);
    attrs.font = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
    return attrs;
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

    auto newItem = std::make_unique<wxWinUITreeItem>(parentItem);
    wxWinUITreeItem *newItemRaw = newItem.get();
    newItemRaw->text = text;
    newItemRaw->images[wxTreeItemIcon_Normal] = image;
    newItemRaw->images[wxTreeItemIcon_Selected] = selImage;
    newItemRaw->data = data;
    newItemRaw->node = MUXC::TreeViewNode();
    if ( data )
        data->SetId(MakeId(newItemRaw));

    UpdatePeerItem(newItemRaw);

    if ( pos == static_cast<size_t>(-1) || pos > parentItem->children.size() )
        pos = parentItem->children.size();

    parentItem->children.insert(parentItem->children.begin() + pos, std::move(newItem));

    m_updatingPeer = true;
    auto peerChildren = wxWinUIGetPeerChildren(this, m_winui.get(), parentItem);
    if ( pos >= peerChildren.Size() )
        peerChildren.Append(newItemRaw->node);
    else
        peerChildren.InsertAt(static_cast<uint32_t>(pos), newItemRaw->node);
    m_updatingPeer = false;

    // The very first item that gives the tree an expandable node makes the
    // expander column meaningful, so the margin that hides it has to go from
    // every item that was already created.
    if ( HasExpandableItem() != m_hadExpandableItem )
    {
        m_hadExpandableItem = !m_hadExpandableItem;
        RefreshPeerItems();
    }

    m_winui->host.ForceRender();
    return MakeId(newItemRaw);
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
    if ( previousItem )
    {
        auto it = std::find_if(parentItem->children.begin(), parentItem->children.end(),
            [previousItem](const std::unique_ptr<wxWinUITreeItem>& child)
            {
                return child.get() == previousItem;
            });
        pos = it == parentItem->children.end()
            ? parentItem->children.size()
            : static_cast<size_t>(std::distance(parentItem->children.begin(), it)) + 1;
    }

    return DoInsertItem(parent, pos, text, image, selImage, data);
}

wxTreeItemId wxTreeCtrl::DoTreeHitTest(const wxPoint& point, int& flags) const
{
    flags = 0;
    const wxSize client = GetClientSize();
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

    // Prefer the real geometry of the realized item containers.
    bool anyRealized = false;
    for ( wxTreeItemId id = GetFirstVisibleItem(); id.IsOk(); id = GetNextVisible(id) )
    {
        wxRect rect;
        if ( !GetItemPeerRect(GetItem(id), rect) )
            continue;

        anyRealized = true;
        if ( point.y >= rect.y && point.y < rect.y + rect.height )
        {
            flags = wxTREE_HITTEST_ONITEM | wxTREE_HITTEST_ONITEMLABEL;
            return id;
        }
    }

    if ( anyRealized )
        return wxTreeItemId();

    // No containers realized (control not shown yet): estimate with a fixed
    // row height.
    const int itemHeight = FromDIP(32);
    const int target = itemHeight > 0 ? point.y / itemHeight : 0;
    int index = 0;

    for ( wxTreeItemId id = GetFirstVisibleItem(); id.IsOk(); id = GetNextVisible(id) )
    {
        if ( index == target )
        {
            flags = wxTREE_HITTEST_ONITEM | wxTREE_HITTEST_ONITEMLABEL;
            return id;
        }
        ++index;
    }

    return wxTreeItemId();
}

wxSize wxTreeCtrl::DoGetBestSize() const
{
    return FromDIP(wxSize(180, 240));
}

void wxTreeCtrl::OnImagesChanged()
{
    RefreshPeerItems();
}

wxWinUITreeItem *wxTreeCtrl::GetItem(const wxTreeItemId& item) const
{
    return static_cast<wxWinUITreeItem *>(item.GetID());
}

wxTreeItemId wxTreeCtrl::MakeId(wxWinUITreeItem *item) const
{
    return wxTreeItemId(item);
}

bool wxTreeCtrl::IsSelectionChangeAllowed(wxWinUITreeItem *item,
                                          wxWinUITreeItem *oldItem)
{
    wxTreeEvent event(wxEVT_TREE_SEL_CHANGING, this, MakeId(item));
    event.SetOldItem(MakeId(oldItem));
    return !GetEventHandler()->ProcessEvent(event) || event.IsAllowed();
}

void wxTreeCtrl::SendTreeEvent(wxEventType type,
                               wxWinUITreeItem *item,
                               wxWinUITreeItem *oldItem)
{
    wxTreeEvent event(type, this, MakeId(item));
    event.SetOldItem(MakeId(oldItem));
    GetEventHandler()->ProcessEvent(event);
}

bool wxTreeCtrl::ChangeSelection(wxWinUITreeItem *item,
                                 bool sendEvent,
                                 bool updatePeer)
{
    wxWinUITreeItem *oldItem = m_winui->selection;
    if ( oldItem == item )
        return true;

    if ( sendEvent && !IsSelectionChangeAllowed(item, oldItem) )
        return false;

    if ( !HasFlag(wxTR_MULTIPLE) && m_winui->root )
    {
        std::vector<wxWinUITreeItem *> stack;
        stack.push_back(m_winui->root.get());
        while ( !stack.empty() )
        {
            wxWinUITreeItem *current = stack.back();
            stack.pop_back();
            current->selected = false;

            for ( const auto& child : current->children )
                stack.push_back(child.get());
        }
    }

    if ( oldItem )
        oldItem->selected = false;

    if ( item )
    {
        wxTreeItemId parent = GetItemParent(MakeId(item));
        while ( parent.IsOk() )
        {
            wxWinUITreeItem *parentItem = GetItem(parent);
            if ( parentItem && (parentItem->parent || !HasFlag(wxTR_HIDE_ROOT)) )
            {
                parentItem->expanded = true;
                try
                {
                    parentItem->node.IsExpanded(true);
                }
                catch ( const winrt::hresult_error& )
                {
                }
            }
            parent = GetItemParent(parent);
        }

        item->selected = true;
    }

    m_winui->selection = item;
    m_winui->focused = item;

    if ( updatePeer )
        ApplySelectionToPeer();

    if ( sendEvent )
        SendTreeEvent(wxEVT_TREE_SEL_CHANGED, item, oldItem);

    return true;
}

void wxTreeCtrl::ApplySelectionToPeer()
{
    if ( !m_winui || !m_winui->treeView )
        return;

    // The WinUI TreeView silently ignores a SelectedNode write performed
    // synchronously from inside its own SelectionChanged callback.  When we
    // need to correct the selection in that context (e.g. an application
    // vetoed the change), defer the write to the dispatcher queue instead.
    if ( m_inPeerSelectionChange )
    {
        SchedulePeerSelectionCorrection();
        return;
    }

    const bool wasUpdating = m_updatingPeer;
    m_updatingPeer = true;
    try
    {
        if ( HasFlag(wxTR_MULTIPLE) )
        {
            auto selectedNodes = m_winui->treeView.SelectedNodes();
            selectedNodes.Clear();

            wxArrayTreeItemIds selections;
            GetSelections(selections);
            for ( size_t i = 0; i < selections.GetCount(); ++i )
            {
                if ( wxWinUITreeItem *item = GetItem(selections[i]) )
                    selectedNodes.Append(item->node);
            }
        }
        else
        {
            m_winui->treeView.SelectedNode(m_winui->selection
                ? m_winui->selection->node
                : MUXC::TreeViewNode{ nullptr });
        }

        if ( m_winui->selection )
        {
            try
            {
                m_winui->selection->node.IsExpanded(m_winui->selection->expanded);
            }
            catch ( const winrt::hresult_error& )
            {
            }
        }
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI TreeView selection", e);
    }
    m_updatingPeer = wasUpdating;

    m_winui->host.ForceRender();
}

void wxTreeCtrl::SchedulePeerSelectionCorrection()
{
    if ( m_peerCorrectionPending || !m_winui || !m_winui->treeView )
        return;

    m_peerCorrectionPending = true;

    auto dispatcher = m_winui->treeView.DispatcherQueue();
    if ( !dispatcher )
    {
        // No dispatcher available: fall back to a direct write.  This may be
        // ignored by the control, but it is the best we can do.
        m_peerCorrectionPending = false;
        ApplySelectionToPeer();
        return;
    }

    dispatcher.TryEnqueue([this]()
    {
        m_peerCorrectionPending = false;
        if ( m_winui && m_winui->treeView )
            ApplySelectionToPeer();
    });
}

void wxTreeCtrl::UpdatePeerItem(wxWinUITreeItem *item)
{
    if ( !item || !item->node )
        return;

    try
    {
        WFC::PropertySet content;
        content.Insert(L"Text", winrt::box_value(wxWinUIToHString(item->text)));

        int image = item->images[wxTreeItemIcon_Normal];
        if ( item->selected && item->expanded &&
             item->images[wxTreeItemIcon_SelectedExpanded] != NO_IMAGE )
        {
            image = item->images[wxTreeItemIcon_SelectedExpanded];
        }
        else if ( item->selected &&
                  item->images[wxTreeItemIcon_Selected] != NO_IMAGE )
        {
            image = item->images[wxTreeItemIcon_Selected];
        }
        else if ( item->expanded &&
                  item->images[wxTreeItemIcon_Expanded] != NO_IMAGE )
        {
            image = item->images[wxTreeItemIcon_Expanded];
        }

        bool hasImage = false;
        if ( image != NO_IMAGE && image >= 0 && image < GetImageCount() )
        {
            const wxBitmap bitmap = GetImageBitmapFor(this, image);
            if ( bitmap.IsOk() )
            {
                if ( auto source = wxWinUIWriteableBitmapFromBitmap(bitmap) )
                {
                    content.Insert(L"Image", source);
                    hasImage = true;
                }
            }
        }

        content.Insert(L"ImageVisibility",
                       winrt::box_value(hasImage
                           ? MUX::Visibility::Visible
                           : MUX::Visibility::Collapsed));

        // Pull the content back over the (empty) expander column when nothing
        // in this tree can ever be expanded.
        content.Insert(L"ContentMargin",
                       winrt::box_value(MUX::ThicknessHelper::FromLengths(
                           HasExpandableItem() ? 0 : -28, 0, 0, 0)));

        item->node.Content(content);

        item->node.HasUnrealizedChildren(item->hasChildrenOverride &&
                                         item->children.empty());

        // Keep the UIA name of an already-realized container in sync with a
        // text change; unrealized ones are named on realization by the
        // ContainerContentChanging hook.
        if ( auto container = m_winui->treeView.ContainerFromNode(item->node) )
        {
            using winrt::Microsoft::UI::Xaml::Automation::AutomationProperties;
            AutomationProperties::SetName(container,
                                          wxWinUIToHString(item->text));
        }
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI TreeView item update", e);
    }
}

void wxTreeCtrl::RefreshPeerItems()
{
    if ( !m_winui || !m_winui->root || !m_winui->treeView )
        return;

    m_updatingPeer = true;
    try
    {
        m_winui->treeView.RootNodes().Clear();

        std::vector<wxWinUITreeItem *> stack;
        stack.push_back(m_winui->root.get());
        while ( !stack.empty() )
        {
            wxWinUITreeItem *item = stack.back();
            stack.pop_back();
            UpdatePeerItem(item);
            item->node.Children().Clear();

            for ( auto it = item->children.rbegin(); it != item->children.rend(); ++it )
                stack.push_back(it->get());
        }

        if ( HasFlag(wxTR_HIDE_ROOT) )
        {
            for ( const auto& child : m_winui->root->children )
                m_winui->treeView.RootNodes().Append(child->node);
        }
        else
        {
            m_winui->treeView.RootNodes().Append(m_winui->root->node);
        }

        std::vector<wxWinUITreeItem *> rebuild;
        rebuild.push_back(m_winui->root.get());
        while ( !rebuild.empty() )
        {
            wxWinUITreeItem *item = rebuild.back();
            rebuild.pop_back();

            if ( item != m_winui->root.get() || !HasFlag(wxTR_HIDE_ROOT) )
                item->node.IsExpanded(item->expanded);

            for ( const auto& child : item->children )
            {
                if ( item != m_winui->root.get() || !HasFlag(wxTR_HIDE_ROOT) )
                    item->node.Children().Append(child->node);
                rebuild.push_back(child.get());
            }
        }

        ApplySelectionToPeer();
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI TreeView refresh", e);
    }
    m_updatingPeer = false;

    m_winui->host.ForceRender();
}

void wxTreeCtrl::OnPeerSelectionChanged()
{
    // A correction is already queued: ignore the control's intermediate
    // selection thrashing until it runs (see SchedulePeerSelectionCorrection).
    if ( m_peerCorrectionPending )
        return;

    wxWinUITreeItem *item = nullptr;

    try
    {
        if ( HasFlag(wxTR_MULTIPLE) )
        {
            auto selected = m_winui->treeView.SelectedNodes();
            if ( selected.Size() )
                item = wxWinUIFindItemByNode(m_winui->root.get(), selected.GetAt(0));
        }
        else
        {
            item = wxWinUIFindItemByNode(m_winui->root.get(),
                                         m_winui->treeView.SelectedNode());
        }
    }
    catch ( const winrt::hresult_error& )
    {
    }

    // If the control insists on re-selecting an item whose selection the
    // application already refused (typically a wxTreebook category that has no
    // page), don't dispatch the event again: just push the real selection back.
    if ( item && item == m_peerRejectedItem )
    {
        SchedulePeerSelectionCorrection();
        return;
    }

    m_inPeerSelectionChange = true;
    if ( !ChangeSelection(item, true, false) )
        ApplySelectionToPeer();
    m_inPeerSelectionChange = false;

    // If a deferred correction got scheduled while dispatching the event, the
    // selection of "item" was refused: remember it so we can short-circuit any
    // further attempts by the control to select it.  Otherwise the selection
    // was accepted, so clear any previous rejection.
    m_peerRejectedItem = m_peerCorrectionPending ? item : nullptr;
}

void wxTreeCtrl::OnPeerRightTapped(const wxPoint& pt)
{
    int flags = 0;
    const wxTreeItemId id = DoTreeHitTest(pt, flags);
    if ( !id.IsOk() )
        return;

    wxTreeEvent rclick(wxEVT_TREE_ITEM_RIGHT_CLICK, this, id);
    rclick.SetPoint(pt);
    GetEventHandler()->ProcessEvent(rclick);

    // As under wxMSW, a right click is also the context menu request.
    wxTreeEvent menu(wxEVT_TREE_ITEM_MENU, this, id);
    menu.SetPoint(pt);
    GetEventHandler()->ProcessEvent(menu);
}

void wxTreeCtrl::OnPeerNodeExpanded(wxWinUITreeItem *item)
{
    if ( !item || item->expanded )
        return;

    SendTreeEvent(wxEVT_TREE_ITEM_EXPANDING, item);
    item->expanded = true;
    SendTreeEvent(wxEVT_TREE_ITEM_EXPANDED, item);
}

void wxTreeCtrl::OnPeerNodeCollapsed(wxWinUITreeItem *item)
{
    if ( !item || !item->expanded )
        return;

    SendTreeEvent(wxEVT_TREE_ITEM_COLLAPSING, item);
    item->expanded = false;
    SendTreeEvent(wxEVT_TREE_ITEM_COLLAPSED, item);
}

#endif // wxUSE_TREECTRL
