/////////////////////////////////////////////////////////////////////////////
// Name:        src/winui/listprojection.cpp
// Purpose:     Viewport-bounded retained WinUI cells for public wxListCtrl
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////
#include "wx/wxprec.h"
#if defined(__WXWINUI__) && wxUSE_WINUI3 && wxUSE_LISTCTRL
#include "listprojection.h"
#include "wx/listctrl.h"
#include "wx/generic/private/listctrl.h"
#include "wx/scopeguard.h"
#include "wx/weakref.h"
#include "wx/image.h"
#if wxUSE_TEXTCTRL
#include "wx/textctrl.h"
#endif
#include "wx/winui/private/appearance.h"
#include "wx/winui/private/tlwhost.h"
#include "private.h"
#include "listautomation.h"
#include <winrt/Microsoft.UI.Xaml.Automation.h>
#include <winrt/Microsoft.UI.Xaml.Automation.Peers.h>
#include <winrt/Microsoft.UI.Xaml.Automation.Provider.h>
#include <winrt/Microsoft.UI.Xaml.Controls.Primitives.h>
#include <winrt/Microsoft.UI.Xaml.Markup.h>
#include <algorithm>
#include <map>
#include <set>
#include <utility>
#include <cstring>
#include <type_traits>

namespace X = winrt::Microsoft::UI::Xaml;
namespace C = X::Controls;
namespace P = C::Primitives;
namespace U = wxWinUIListAutomation;

namespace
{
using Key = std::pair<long, int>;
enum class Kind { Cell, Check, Header, Row };
struct InvalidListSnapshot {};

namespace A = X::Automation::Peers;
namespace AP = X::Automation::Provider;

struct ItemSelectionBinding
{
    std::function<bool()> current, selected, enabled;
    std::function<void(int)> change;
    std::function<A::AutomationPeer()> container;
    std::function<std::vector<A::AutomationPeer>()> children;
    void Check() const
    {
        if ( !current || !current() )
            throw winrt::hresult_error(static_cast<int32_t>(0x80040201u)); // UIA_E_ELEMENTNOTAVAILABLE
    }
};

template<bool Icon> struct ItemSelectionPeer;
template<bool Icon> using ItemPeerBase = std::conditional_t<Icon,
    A::GridViewItemAutomationPeerT<ItemSelectionPeer<Icon>, AP::ISelectionItemProvider>,
    A::ListViewItemAutomationPeerT<ItemSelectionPeer<Icon>, AP::ISelectionItemProvider>>;
template<bool Icon> using NativeItem = std::conditional_t<Icon, C::GridViewItem, C::ListViewItem>;

// Keep the real WinUI item peer (bounds, theme, focus, names). Only selection
// commands use wx's full model: an exclusive Select must also clear selected
// rows outside the viewport, which are not in the bounded native item slice.
template<bool Icon>
struct ItemSelectionPeer : ItemPeerBase<Icon>
{
    using Base = ItemPeerBase<Icon>;
    ItemSelectionPeer(const NativeItem<Icon>& item,
                      std::shared_ptr<ItemSelectionBinding> binding)
        : Base(item), m_binding(std::move(binding)) {}
    winrt::Windows::Foundation::IInspectable GetPatternCore(A::PatternInterface pattern)
    {
        m_binding->Check();
        if ( pattern == A::PatternInterface::SelectionItem ) return *this;
        return Base::GetPatternCore(pattern);
    }
    bool IsSelected() { m_binding->Check(); return m_binding->selected(); }
    AP::IRawElementProviderSimple SelectionContainer()
    {
        m_binding->Check();
        const auto peer = m_binding->container();
        m_binding->Check();
        return this->ProviderFromPeer(peer);
    }
    void Select() { Change(0); }
    void AddToSelection() { Change(1); }
    void RemoveFromSelection() { Change(2); }
    winrt::Windows::Foundation::Collections::IVector<A::AutomationPeer> GetChildrenCore()
    {
        m_binding->Check();
        if ( !m_binding->children ) return Base::GetChildrenCore();
        auto children = m_binding->children();
        m_binding->Check();
        for ( const auto& child : children ) child.SetParent(this->template try_as<A::AutomationPeer>());
        return winrt::single_threaded_vector<A::AutomationPeer>(std::move(children));
    }
private:
    void Change(int operation)
    {
        m_binding->Check();
        if ( !m_binding->enabled() )
            throw winrt::hresult_error(static_cast<int32_t>(0x80040200u)); // UIA_E_ELEMENTNOTENABLED
        m_binding->change(operation);
    }
    std::shared_ptr<ItemSelectionBinding> m_binding;
};
template<bool Icon> struct ItemElement;
template<bool Icon> using ItemElementBase = std::conditional_t<Icon,
    C::GridViewItemT<ItemElement<Icon>>, C::ListViewItemT<ItemElement<Icon>>>;
template<bool Icon>
struct ItemElement : ItemElementBase<Icon>
{
    explicit ItemElement(std::shared_ptr<ItemSelectionBinding> binding)
        : m_binding(std::move(binding)) {}
    A::AutomationPeer OnCreateAutomationPeer()
        { return winrt::make<ItemSelectionPeer<Icon>>(this->template try_as<NativeItem<Icon>>(), m_binding); }
private:
    std::shared_ptr<ItemSelectionBinding> m_binding;
};

#if wxUSE_IMAGE
bool SameImage(const wxImage& a, const wxImage& b)
{
    if ( !a.IsOk() || !b.IsOk() ) return a.IsOk() == b.IsOk();
    if ( a.GetSize() != b.GetSize() || a.HasAlpha() != b.HasAlpha() ||
         a.HasMask() != b.HasMask() ) return false;
    if ( a.HasMask() && (a.GetMaskRed() != b.GetMaskRed() ||
         a.GetMaskGreen() != b.GetMaskGreen() || a.GetMaskBlue() != b.GetMaskBlue()) ) return false;
    const size_t pixels = static_cast<size_t>(a.GetWidth()) * a.GetHeight();
    return std::memcmp(a.GetData(), b.GetData(), pixels * 3) == 0 &&
        (!a.HasAlpha() || std::memcmp(a.GetAlpha(), b.GetAlpha(), pixels) == 0);
}
#endif
}

struct wxWinUIListProjection::Impl : std::enable_shared_from_this<Impl>
{
    struct Snapshot
    {
        wxListMainWindow* main;
        std::uint64_t structure;
        std::uint64_t revision;
        long style;
        int columns;
    };
    struct Cell
    {
        Key key;
        Kind kind = Kind::Cell;
        wxRect rect;
        wxString text;
        wxBitmap bitmap;
        wxColour foreground, background;
        wxFont font;
        int alignment = wxLIST_FORMAT_LEFT;
        bool checked = false;
        bool current = false;
        bool vertical = false;
        bool customBackground = false;
    };
    struct Peer
    {
        Key key;
        Kind kind;
        std::uint64_t structure = 0;
        int columns = 0;
        long style = 0;
        C::Control control{nullptr};
        C::TextBlock text{nullptr};
        C::Image image{nullptr};
        C::StackPanel content{nullptr};
        C::Grid itemContent{nullptr};
        C::Canvas rowContent{nullptr};
        C::Canvas mount{nullptr};
        Cell painted;
#if wxUSE_IMAGE
        wxImage paintedImage;
#endif
        double scale = 0;
    };
    struct Pane
    {
        wxWeakRef<wxWindow> window;
        bool header = false;
        bool published = false;
        bool inputActive = false;
        std::uint64_t inputSerial = 0;
        wxWinUIControlHost host;
        C::Grid root{nullptr};
        C::Canvas canvas{nullptr};
        C::ListViewBase selector{nullptr};
        long mode = 0;
        wxColour background;
        std::map<Key, std::shared_ptr<Peer>> peers;
    };

    explicit Impl(wxGenericListCtrl* list) : owner(list) {}
    wxWeakRef<wxGenericListCtrl> owner;
    std::vector<std::shared_ptr<Pane>> panes;
    std::uint64_t revision = 0;
    unsigned suppress = 0;
    bool dirty = true, scheduled = false, synchronizing = false, closed = false;
    bool automationWriting = false;
    bool automationSelecting = false;
    std::vector<long> automationSelection;
    bool automationSelectionKnown = false;
    bool selectionScheduled = false;
    Snapshot automationSchema{};
    std::map<Key, winrt::weak_ref<A::AutomationPeer>> automationCells;
    // At most one real Button per explicitly requested column, never rows.
    std::map<int, C::Button> automationHeaders;

    void AutomationCheck(const Snapshot& s, Key key) const;
    std::shared_ptr<Pane> AutomationMainPane() const;
    std::pair<std::shared_ptr<Pane>, std::shared_ptr<Peer>> AutomationVisual(Key key) const;
    U::F::Rect AutomationBounds(const Snapshot& s, Key key) const;
    A::AutomationPeer AutomationCell(Key key);
    A::AutomationPeer AutomationRow(long row);
    std::vector<long> AutomationSelectedRows() const;
    void AutomationSelectRow(const Snapshot& s, long row, int operation);
    void PublishSelection()
    {
        const auto pane = AutomationMainPane();
        if ( !pane ) return;
        const auto identity = Capture();
        const auto peer = A::FrameworkElementAutomationPeer::FromElement(pane->selector);
        if ( !peer || !IdentityCurrent(identity) ) return;
        std::vector<std::shared_ptr<Peer>> rows;
        for ( const auto& entry : pane->peers )
            if ( entry.second->kind == Kind::Row ) rows.push_back(entry.second);
        ++suppress;
        const auto restore = wxMakeGuard([this] { --suppress; });
        for ( const auto& row : rows )
        {
            if ( !IdentityCurrent(identity) || !Current(pane, row) ) return;
            const bool selected = identity.main->IsHighlighted(row->key.first);
            const auto item = row->control.as<P::SelectorItem>();
            if ( item.IsSelected() != selected ) item.IsSelected(selected);
            if ( !IdentityCurrent(identity) || !Current(pane, row) ) return;
            row->painted.checked = selected;
        }
        auto selected = AutomationSelectedRows();
        const bool changed = automationSelectionKnown && selected != automationSelection;
        automationSelection = std::move(selected);
        automationSelectionKnown = true;
        // No model getters, native item replacement, focus, or layout work.
        if ( changed ) peer.RaiseAutomationEvent(A::AutomationEvents::SelectionPatternOnInvalidated);
    }
    void NotifySelectionChanged()
    {
        if ( closed || !owner || owner->IsBeingDeleted() || selectionScheduled ) return;
        selectionScheduled = true;
        const std::weak_ptr<Impl> weak = shared_from_this();
        owner->CallAfter([weak]
        {
            if ( const auto self = weak.lock() )
            {
                self->selectionScheduled = false;
                self->PublishSelection();
            }
        });
    }
    std::vector<A::AutomationPeer> AutomationRowChildren(const std::shared_ptr<Pane>& pane, long row);
    std::shared_ptr<U::GridBinding> AutomationBinding(const std::shared_ptr<Pane>& pane);
    void AutomationRealize(const Snapshot& s, Key key);
    void AutomationSetValue(const Snapshot& s, Key key, const winrt::hstring& value);

    static bool IsNativeItem(const std::shared_ptr<Pane>& pane, Kind kind)
    {
        return pane->selector && (kind == Kind::Row ||
            (pane->mode != wxLC_REPORT && kind == Kind::Cell));
    }

    Snapshot Capture() const
    {
        return {owner->m_mainWin, owner->m_mainWin->m_structureRevision,
                revision, owner->GetWindowStyleFlag(), owner->GetColumnCount()};
    }
    bool IdentityCurrent(const Snapshot& s) const
    {
        return !closed && owner && !owner->IsBeingDeleted() &&
            owner->m_mainWin == s.main &&
            s.main->m_structureRevision == s.structure &&
            owner->GetWindowStyleFlag() == s.style && owner->GetColumnCount() == s.columns;
    }
    void Check(const Snapshot& s) const
    {
        if ( !IdentityCurrent(s) || s.revision != revision ) throw InvalidListSnapshot{};
    }
    void Invalidate()
    {
        if ( closed || !owner ) return;
        dirty = true;
        ++revision;
        if ( scheduled || synchronizing ) return;
        scheduled = true;
        const std::weak_ptr<Impl> weak = shared_from_this();
        owner->CallAfter([weak]()
        {
            if ( auto self = weak.lock() )
            {
                self->scheduled = false;
                self->Synchronize();
            }
        });
    }
    bool Current(const std::shared_ptr<Pane>& pane, const std::shared_ptr<Peer>& peer) const
    {
        if ( closed || !owner || owner->IsBeingDeleted() || !pane->window ||
             !pane->published || !owner->m_mainWin ||
             owner->m_mainWin->m_structureRevision != peer->structure ||
             owner->GetWindowStyleFlag() != peer->style ||
             owner->GetColumnCount() != peer->columns ) return false;
        const auto found = pane->peers.find(peer->key);
        return found != pane->peers.end() && found->second == peer;
    }

    void Activate(const std::shared_ptr<Pane>& pane, const std::shared_ptr<Peer>& peer)
    {
        if ( suppress || pane->inputActive || !Current(pane, peer) ) return;
        const auto s = Capture();
        // Physical gestures already reach wx's mouse/key state machine through
        // the host. These handlers are the native automation semantic path.
        if ( peer->kind == Kind::Header )
        {
            wxListEvent event(wxEVT_LIST_COL_CLICK, owner->GetId());
            event.SetEventObject(owner.get());
            event.SetColumn(peer->key.second);
            owner->HandleWindowEvent(event);
        }
        else if ( peer->kind == Kind::Check )
        {
            const auto checked = peer->control.as<C::CheckBox>().IsChecked();
            s.main->CheckItem(peer->key.first, checked && checked.Value());
        }
        if ( owner && !closed ) Invalidate();
    }

    void SelectNativeItems(const std::shared_ptr<Pane>& pane,
                           const C::SelectionChangedEventArgs& args)
    {
        if ( suppress || pane->inputActive || closed || !owner || !pane->published ) return;
        const auto s = Capture();
        std::vector<std::pair<std::shared_ptr<Peer>, bool>> changes;
        const auto collect = [&](const auto& items, bool selected)
        {
            for ( const auto& value : items )
            {
                const auto control = value.template try_as<C::Control>();
                for ( const auto& entry : pane->peers )
                    if ( IsNativeItem(pane, entry.second->kind) && entry.second->control == control )
                    {
                        changes.emplace_back(entry.second, selected);
                        break;
                    }
            }
        };
        collect(args.RemovedItems(), false);
        collect(args.AddedItems(), true);
        for ( const auto& change : changes )
        {
            if ( !IdentityCurrent(s) || !Current(pane, change.first) ) return;
            const long flags = change.second ? wxLIST_STATE_SELECTED | wxLIST_STATE_FOCUSED : 0;
            s.main->SetItemState(change.first->key.first, flags,
                wxLIST_STATE_SELECTED | (change.second ? wxLIST_STATE_FOCUSED : 0));
            if ( !IdentityCurrent(s) || !Current(pane, change.first) ) return;
            if ( change.second ) s.main->m_anchor = static_cast<size_t>(change.first->key.first);
        }
        if ( !IdentityCurrent(s) ) return;
        if ( !args.AddedItems().Size() ) { Invalidate(); return; }
        s.main->SetFocus();
        if ( IdentityCurrent(s) ) Invalidate();
    }

    void SelectAutomationItem(const std::shared_ptr<Pane>& pane,
                              const std::shared_ptr<Peer>& peer, int operation)
    {
        if ( !Current(pane, peer) ) return;
        const auto s = Capture();
        if ( operation == 1 && (s.style & wxLC_SINGLE_SEL) &&
             !s.main->IsHighlighted(peer->key.first) && owner->GetSelectedItemCount() )
            throw winrt::hresult_illegal_method_call();
        if ( operation == 0 )
        {
            for ( long row = owner->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
                  row != wxNOT_FOUND;
                  row = owner->GetNextItem(row, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED) )
            {
                if ( row != peer->key.first ) s.main->SetItemState(row, 0, wxLIST_STATE_SELECTED);
                if ( !IdentityCurrent(s) || !Current(pane, peer) ) return;
            }
        }
        s.main->SetItemState(peer->key.first,
            operation == 2 ? 0 : wxLIST_STATE_SELECTED | wxLIST_STATE_FOCUSED,
            wxLIST_STATE_SELECTED | (operation == 2 ? 0 : wxLIST_STATE_FOCUSED));
        if ( !IdentityCurrent(s) || !Current(pane, peer) ) return;
        if ( operation != 2 )
        {
            // Native selection is a user/automation selection action, not
            // just the public SetItemState setter. Preserve the same anchor
            // transition as mouse selection so a later Shift navigation
            // extends from this item instead of the uninitialized SIZE_MAX.
            s.main->m_anchor = static_cast<size_t>(peer->key.first);
            s.main->SetFocus();
        }
        if ( IdentityCurrent(s) )
        {
            Invalidate();
            Synchronize();
        }
    }

    void ObserveInput(const std::shared_ptr<Pane>& pane)
    {
        const std::weak_ptr<Impl> weak = shared_from_this();
        const std::weak_ptr<Pane> weakPane = pane;
        const auto begin = [weakPane](const auto&, const auto&)
        {
            if ( auto live = weakPane.lock() ) { ++live->inputSerial; live->inputActive = true; }
        };
        const auto end = [weak, weakPane](const auto&, const auto&)
        {
            auto self = weak.lock();
            auto live = weakPane.lock();
            if ( !self || !self->owner || !live ) return;
            const auto serial = live->inputSerial;
            self->owner->CallAfter([weak, weakPane, serial]()
            {
                auto current = weakPane.lock();
                auto state = weak.lock();
                if ( !state || !current || current->inputSerial != serial ) return;
                current->inputActive = false;
                state->Invalidate();
            });
        };
        pane->root.AddHandler(X::UIElement::PointerPressedEvent(), winrt::box_value(X::Input::PointerEventHandler(begin)), true);
        pane->root.AddHandler(X::UIElement::PointerReleasedEvent(), winrt::box_value(X::Input::PointerEventHandler(end)), true);
        pane->root.AddHandler(X::UIElement::PointerCanceledEvent(), winrt::box_value(X::Input::PointerEventHandler(end)), true);
        pane->root.AddHandler(X::UIElement::PointerCaptureLostEvent(), winrt::box_value(X::Input::PointerEventHandler(end)), true);
        pane->root.AddHandler(X::UIElement::KeyDownEvent(), winrt::box_value(X::Input::KeyEventHandler(begin)), true);
        pane->root.AddHandler(X::UIElement::KeyUpEvent(), winrt::box_value(X::Input::KeyEventHandler(end)), true);
    }
    void ClosePane(const std::shared_ptr<Pane>& pane)
    {
        pane->published = false;
        const auto retiring = std::move(pane->peers);
        for ( const auto& item : retiring ) item.second->control.IsEnabled(false);
        pane->host.Close();
    }
    void UpdatePanes(const Snapshot& s)
    {
        wxWindow* wanted[] = {owner->m_mainWin, owner->m_headerWin};
        for ( auto it = panes.begin(); it != panes.end(); )
        {
            const auto pane = *it;
            if ( !pane->window || pane->mode != (s.style & wxLC_MASK_TYPE) ||
                 std::find(std::begin(wanted), std::end(wanted), pane->window.get()) == std::end(wanted) )
            {
                it = panes.erase(it);
                ClosePane(pane);
                Check(s);
            }
            else ++it;
        }
        for ( unsigned n = 0; n != 2; ++n )
        {
            Check(s);
            if ( !wanted[n] ) continue;
            if ( std::any_of(panes.begin(), panes.end(), [&](const auto& p) { return p->window.get() == wanted[n]; }) ) continue;
            auto pane = std::make_shared<Pane>();
            pane->window = wanted[n];
            pane->header = n == 1;
            pane->mode = s.style & wxLC_MASK_TYPE;
            const bool initialized = pane->host.Initialize(wanted[n]);
            Check(s);
            if ( !initialized ) continue;
            pane->root = C::Grid();
            pane->canvas = C::Canvas();
            X::Automation::AutomationProperties::SetAutomationId(pane->root,
                pane->header ? L"wxList.NativeHeader" : L"wxList.NativeViewport");
            const std::weak_ptr<Impl> weak = shared_from_this();
            const std::weak_ptr<Pane> weakPane = pane;
            if ( !pane->header )
            {
                pane->selector = pane->mode == wxLC_REPORT
                    ? winrt::make<U::GridElement>(AutomationBinding(pane)).as<C::ListViewBase>()
                    : pane->mode == wxLC_LIST ? C::ListView().as<C::ListViewBase>()
                                             : C::GridView().as<C::ListViewBase>();
                const auto selector = pane->selector;
                // wx's existing geometry/scroll model remains authoritative.
                // Only the bounded visible item slice is published; Canvas
                // arranges the real themed native containers at those rects.
                selector.ItemsPanel(X::Markup::XamlReader::Load(
                    LR"(<ItemsPanelTemplate xmlns="http://schemas.microsoft.com/winfx/2006/xaml/presentation"><Canvas /></ItemsPanelTemplate>)")
                    .as<C::ItemsPanelTemplate>());
                selector.Padding({0, 0, 0, 0});
                selector.BorderThickness({0, 0, 0, 0});
                selector.IsTabStop(false);
                selector.HorizontalContentAlignment(X::HorizontalAlignment::Stretch);
                selector.VerticalContentAlignment(X::VerticalAlignment::Stretch);
                selector.SelectionMode(s.style & wxLC_SINGLE_SEL
                    ? C::ListViewSelectionMode::Single : C::ListViewSelectionMode::Extended);
                C::ScrollViewer::SetHorizontalScrollMode(selector, C::ScrollMode::Disabled);
                C::ScrollViewer::SetVerticalScrollMode(selector, C::ScrollMode::Disabled);
                C::ScrollViewer::SetHorizontalScrollBarVisibility(selector, C::ScrollBarVisibility::Disabled);
                C::ScrollViewer::SetVerticalScrollBarVisibility(selector, C::ScrollBarVisibility::Disabled);
                X::Automation::AutomationProperties::SetAutomationId(selector, L"wxList.NativeItems");
                selector.SelectionChanged([weak, weakPane](const auto&, const C::SelectionChangedEventArgs& args)
                {
                    const auto self = weak.lock();
                    const auto current = weakPane.lock();
                    if ( self && current ) self->SelectNativeItems(current, args);
                });
                pane->root.Children().Append(selector);
            }
            pane->root.Children().Append(pane->canvas);
            ObserveInput(pane);
            wanted[n]->Bind(wxEVT_DESTROY, [weak, weakPane](wxWindowDestroyEvent& event)
            {
                if ( auto self = weak.lock() )
                    if ( auto live = weakPane.lock() )
                    {
                        // Destroy events from the in-place label editor also
                        // propagate through this window. They retire that
                        // editor's slot, not the containing list viewport.
                        if ( event.GetEventObject() == live->window.get() )
                            self->ClosePane(live);
                    }
                event.Skip();
            });
            const bool published = pane->host.SetContent(pane->root);
            Check(s);
            if ( !published ) { pane->host.Close(); Check(s); continue; }
            pane->published = true;
            panes.push_back(std::move(pane));
        }
    }

    std::vector<size_t> VisibleRows(const Snapshot& s)
    {
        std::vector<size_t> rows;
        auto* main = s.main;
        const size_t count = main->GetItemCount();
        if ( !count ) return rows;
        if ( main->InReportView() )
        {
            size_t first, last;
            main->GetVisibleLinesRange(&first, &last);
            for ( size_t row = first; row <= last && row < count; ++row ) rows.push_back(row);
            return rows;
        }
        // The common geometry is column-major (or one vertical/horizontal
        // strip with explicit alignment). Binary-search it, never visit a
        // million hidden items to find a screenful of native peers.
        const wxRect viewport(owner->CalcUnscrolledPosition(wxPoint(0, 0)), main->GetClientSize());
        const bool vertical = main->HasFlag(wxLC_ALIGN_TOP);
        const auto coordinate = [&](size_t row)
        {
            const auto& r = main->m_lines[row].m_gi->m_rectAll;
            return vertical ? r.y : r.x;
        };
        const auto lower = [&](int value)
        {
            size_t lo = 0, hi = count;
            while ( lo < hi )
            {
                const size_t mid = lo + (hi - lo) / 2;
                if ( coordinate(mid) < value ) lo = mid + 1;
                else hi = mid;
            }
            return lo;
        };
        size_t first = lower(vertical ? viewport.y : viewport.x);
        if ( first ) first = lower(coordinate(first - 1));
        const int end = vertical ? viewport.GetBottom() : viewport.GetRight();
        for ( size_t row = first; row < count && coordinate(row) <= end; ++row )
            if ( main->m_lines[row].m_gi->m_rectAll.Intersects(viewport) ) rows.push_back(row);
        return rows;
    }

    std::vector<Cell> ReadPane(const std::shared_ptr<Pane>& pane, const Snapshot& s)
    {
        std::vector<Cell> cells;
        auto* main = s.main;
        const wxPoint origin = owner->CalcScrolledPosition(wxPoint(0, 0));
        const wxRect viewport(wxPoint(0, 0), pane->window->GetClientSize());
        const wxColour foreground = main->GetForegroundColour();
        const wxColour background = main->GetBackgroundColour();
        const wxFont font = main->GetFont();
        if ( pane->header )
        {
            int x = origin.x;
            for ( int order = 0; order < s.columns; ++order )
            {
                const int col = main->GetColumnIndexFromOrder(order);
                const auto& header = main->m_columns[col];
                Cell cell;
                cell.key = {-1, col}; cell.kind = Kind::Header;
                cell.rect = wxRect(x, 0, header.GetWidth(), viewport.height);
                x += header.GetWidth();
                if ( !cell.rect.Intersects(viewport) ) continue;
                cell.text = header.GetText();
                if ( owner->m_headerWin->m_sortCol == col )
                    cell.text += owner->m_headerWin->m_sortAsc ? wxString::FromUTF8(" \xe2\x86\x91") : wxString::FromUTF8(" \xe2\x86\x93");
                cell.font = font; cell.foreground = foreground;
                cell.background = background; cell.alignment = header.GetFormat();
                if ( main->m_small_images && header.GetImage() >= 0 && header.GetImage() < main->m_small_images->GetImageCount() )
                    cell.bitmap = main->m_small_images->GetImageBitmapFor(main, header.GetImage());
                Check(s);
                cells.push_back(std::move(cell));
            }
            return cells;
        }
        const auto rows = VisibleRows(s);
        if ( main->IsVirtual() && !rows.empty() )
        {
            wxListEvent event(wxEVT_LIST_CACHE_HINT, owner->GetId());
            event.SetEventObject(owner.get());
            event.SetCacheFrom(static_cast<long>(rows.front()));
            event.SetCacheTo(static_cast<long>(rows.back()));
            owner->HandleWindowEvent(event);
            Check(s);
        }
        for ( const size_t row : rows )
        {
            Check(s);
            const int columns = main->InReportView() ? s.columns : 1;
            const wxRect line = main->GetLineRect(row);
            int x = line.x + origin.x;
            const bool selected = main->IsHighlighted(row);
            const bool current = main->m_current == row;
            const size_t rowIndex = cells.size();
            if ( main->InReportView() )
            {
                Cell nativeRow;
                nativeRow.key = {static_cast<long>(row), -2};
                nativeRow.kind = Kind::Row;
                nativeRow.rect = wxRect(0, line.y + origin.y, viewport.width, line.height);
                nativeRow.checked = selected;
                nativeRow.current = current;
                nativeRow.font = font;
                nativeRow.foreground = foreground;
                nativeRow.background = background;
                cells.push_back(std::move(nativeRow));
            }
            bool checked = false;
            if ( main->HasCheckBoxes() )
            {
                checked = main->IsVirtual() ? owner->OnGetItemIsChecked(static_cast<long>(row)) : main->m_lines[row].IsChecked();
                Check(s);
            }
            for ( int order = 0; order < columns; ++order )
            {
                const int col = main->InReportView()
                    ? main->GetColumnIndexFromOrder(order) : order;
                Cell cell;
                cell.key = {static_cast<long>(row), col};
                cell.rect = line;
                cell.rect.Offset(origin);
                if ( main->InReportView() )
                {
                    cell.rect.x = x;
                    cell.rect.width = main->GetColumnWidth(col);
                    x += cell.rect.width;
                    cell.alignment = main->m_columns[col].GetFormat();
                }
                if ( !cell.rect.Intersects(viewport) || cell.rect.IsEmpty() ) continue;
                cell.checked = selected;
                cell.current = !main->InReportView() && current && col == 0;
                cell.font = font; cell.foreground = foreground; cell.background = background;
                cell.vertical = main->HasFlag(wxLC_ICON | wxLC_SMALL_ICON);
                int image = -1;
                const wxItemAttr* attr = nullptr;
                if ( main->IsVirtual() )
                {
                    // Do not retain the generic dummy line across callbacks:
                    // applications may replace/delete this list from OnGet*.
                    cell.text = owner->OnGetItemText(static_cast<long>(row), col); Check(s);
                    image = owner->OnGetItemColumnImage(static_cast<long>(row), col); Check(s);
                    attr = owner->OnGetItemAttr(static_cast<long>(row)); Check(s);
                }
                else
                {
                    const auto& lineData = main->m_lines[row];
                    // Existing controls can change icon/report mode while
                    // retaining rows created with fewer subitems.
                    if ( static_cast<size_t>(col) < lineData.m_items.size() )
                    {
                        const auto& item = lineData.m_items[col];
                        cell.text = item.GetText(); image = item.GetImage();
                    }
                    if ( !lineData.m_items.empty() ) attr = lineData.GetAttr();
                }
                if ( attr )
                {
                    if ( attr->HasFont() ) cell.font = attr->GetFont();
                    if ( attr->HasTextColour() ) cell.foreground = attr->GetTextColour();
                    if ( attr->HasBackgroundColour() )
                    {
                        cell.background = attr->GetBackgroundColour();
                        cell.customBackground = true;
                    }
                }
                const auto* images = main->HasFlag(wxLC_ICON) ? main->m_normal_images : main->m_small_images;
                if ( images && image >= 0 && image < images->GetImageCount() )
                    cell.bitmap = images->GetImageBitmapFor(main, image);
                Check(s);
                if ( main->InReportView() && !cell.text.empty() )
                {
                    // The row's native accessible name is composed from its
                    // actual visible cells, without extra virtual-model reads.
                    if ( !cells[rowIndex].text.empty() ) cells[rowIndex].text += ", ";
                    cells[rowIndex].text += cell.text;
                }
                if ( col == 0 && main->HasCheckBoxes() )
                {
                    Cell check = cell;
                    check.kind = Kind::Check; check.key.second = -1; check.checked = checked;
                    check.rect.width = main->FromDIP(28);
                    cell.rect.x += check.rect.width;
                    cell.rect.width = wxMax(0, cell.rect.width - check.rect.width);
                    cells.push_back(std::move(check));
                }
                cells.push_back(std::move(cell));
            }
        }
        return cells;
    }

    std::shared_ptr<Peer> MakePeer(const std::shared_ptr<Pane>& pane, const Cell& cell)
    {
        auto peer = std::make_shared<Peer>();
        peer->key = cell.key; peer->kind = cell.kind;
        const std::weak_ptr<Impl> weak = shared_from_this();
        const std::weak_ptr<Pane> weakPane = pane;
        const std::weak_ptr<Peer> weakPeer = peer;
        const auto activate = [weak, weakPane, weakPeer](const auto&, const auto&)
        {
            auto self = weak.lock(); auto current = weakPane.lock(); auto item = weakPeer.lock();
            if ( self && current && item ) self->Activate(current, item);
        };
        const auto identity = Capture();
        const auto automation = [weak, identity, key = cell.key]() -> A::AutomationPeer
        {
            const auto self = weak.lock();
            if ( !self || !self->IdentityCurrent(identity) ) return nullptr;
            return self->AutomationCell(key);
        };
        wxString id;
        if ( cell.kind == Kind::Check )
        {
            C::CheckBox check;
            check.Checked(activate); check.Unchecked(activate);
            peer->control = check;
            id = wxString::Format("wxList.Check.%ld", cell.key.first);
        }
        else
        {
            if ( cell.kind == Kind::Header )
            {
                // The real Button is also retained by an offscreen UIA
                // header provider and remounted without changing identity.
                const auto headerPeer = AutomationCell(cell.key);
                wxUnusedVar(headerPeer);
                peer->control = automationHeaders.at(cell.key.second);
                id = wxString::Format("wxList.Header.%d", cell.key.second);
            }
            else
            {
                if ( cell.kind == Kind::Row )
                {
                    peer->control = winrt::make<U::RowElement>([weak, identity, row = cell.key.first]() -> A::AutomationPeer
                    {
                        const auto self = weak.lock();
                        if ( !self || !self->IdentityCurrent(identity) ) return nullptr;
                        return self->AutomationRow(row);
                    });
                }
                else if ( IsNativeItem(pane, cell.kind) )
                {
                    auto binding = std::make_shared<ItemSelectionBinding>();
                    binding->current = [weak, weakPane, weakPeer]
                    {
                        const auto self = weak.lock(); const auto live = weakPane.lock();
                        const auto item = weakPeer.lock();
                        return self && live && item && self->Current(live, item);
                    };
                    binding->selected = [weak, weakPeer]
                    {
                        const auto self = weak.lock(); const auto item = weakPeer.lock();
                        return self && self->owner && item && self->owner->m_mainWin->IsHighlighted(item->key.first);
                    };
                    binding->enabled = [weak]
                    { const auto self = weak.lock(); return self && self->owner && self->owner->IsEnabled(); };
                    binding->change = [weak, weakPane, weakPeer](int operation)
                    {
                        const auto self = weak.lock(); const auto live = weakPane.lock();
                        const auto item = weakPeer.lock();
                        if ( self && live && item ) self->SelectAutomationItem(live, item, operation);
                    };
                    binding->container = [weakPane]() -> A::AutomationPeer
                    {
                        const auto live = weakPane.lock();
                        return live ? A::FrameworkElementAutomationPeer::CreatePeerForElement(live->selector) : nullptr;
                    };
                    if ( cell.kind == Kind::Row )
                        binding->children = [weak, weakPane, row = cell.key.first]
                        {
                            const auto self = weak.lock(); const auto live = weakPane.lock();
                            if ( !self || !live ) U::Unavailable();
                            return self->AutomationRowChildren(live, row);
                        };
                    peer->control = pane->mode == wxLC_LIST || pane->mode == wxLC_REPORT
                        ? winrt::make<ItemElement<false>>(binding).as<C::Control>()
                        : winrt::make<ItemElement<true>>(binding).as<C::Control>();
                }
                else
                {
                    // A report subitem is native content inside its one
                    // selectable ListViewItem row, never an independent button.
                    peer->control = winrt::make<U::CellElement>(automation);
                    peer->control.IsTabStop(false);
                }
                id = cell.kind == Kind::Row
                    ? wxString::Format("wxList.Row.%ld", cell.key.first)
                    : wxString::Format("wxList.Item.%ld.%d", cell.key.first, cell.key.second);
            }
            if ( cell.kind == Kind::Row )
            {
                peer->rowContent = C::Canvas();
                peer->control.as<C::ContentControl>().Content(peer->rowContent);
            }
            else
            {
                peer->image = C::Image();
                peer->text = C::TextBlock();
                peer->text.VerticalAlignment(X::VerticalAlignment::Center);
                peer->text.TextTrimming(X::TextTrimming::CharacterEllipsis);
                if ( pane->selector && cell.kind == Kind::Cell )
                {
                    peer->itemContent = C::Grid();
                    if ( cell.vertical )
                    {
                        C::RowDefinition imageRow, textRow;
                        imageRow.Height({1, X::GridUnitType::Star});
                        textRow.Height({1, X::GridUnitType::Auto});
                        peer->itemContent.RowDefinitions().Append(imageRow);
                        peer->itemContent.RowDefinitions().Append(textRow);
                        C::Grid::SetRow(peer->text, 1);
                        peer->text.HorizontalAlignment(X::HorizontalAlignment::Center);
                        peer->image.Margin({0, 0, 0, 4});
                    }
                    else
                    {
                        C::ColumnDefinition imageColumn, textColumn;
                        imageColumn.Width({1, X::GridUnitType::Auto});
                        textColumn.Width({1, X::GridUnitType::Star});
                        peer->itemContent.ColumnDefinitions().Append(imageColumn);
                        peer->itemContent.ColumnDefinitions().Append(textColumn);
                        C::Grid::SetColumn(peer->text, 1);
                        peer->image.Margin({0, 0, 4, 0});
                    }
                    peer->itemContent.Children().Append(peer->image);
                    peer->itemContent.Children().Append(peer->text);
                    peer->control.as<C::ContentControl>().Content(peer->itemContent);
                }
                else
                {
                    peer->content = C::StackPanel();
                    peer->content.Spacing(4);
                    peer->content.Children().Append(peer->image);
                    peer->content.Children().Append(peer->text);
                    peer->control.as<C::ContentControl>().Content(peer->content);
                }
            }
        }
        peer->control.MinWidth(0); peer->control.MinHeight(0);
        peer->control.Padding(cell.kind == Kind::Row ? X::Thickness{0, 0, 0, 0} : X::Thickness{4, 2, 4, 2});
        peer->control.HorizontalContentAlignment(X::HorizontalAlignment::Stretch);
        peer->control.VerticalContentAlignment(cell.kind == Kind::Row
            ? X::VerticalAlignment::Stretch : X::VerticalAlignment::Center);
        X::Automation::AutomationProperties::SetAutomationId(peer->control, wxWinUIToHString(id));
        return peer;
    }

    void Publish(const std::shared_ptr<Pane>& pane, const std::vector<Cell>& cells, const Snapshot& s)
    {
        Check(s);
        const double scale = pane->window->GetDPIScaleFactor();
        if ( pane->selector )
        {
            const auto mode = s.style & wxLC_SINGLE_SEL
                ? C::ListViewSelectionMode::Single : C::ListViewSelectionMode::Extended;
            if ( pane->selector.SelectionMode() != mode ) pane->selector.SelectionMode(mode);
            Check(s);
        }
        const auto background = s.main->GetBackgroundColour();
        if ( pane->background != background )
        {
            // The overlay contains only optional checkboxes. An opaque Canvas
            // above the selector would obscure its native item presenters.
            if ( pane->selector ) pane->root.Background(wxWinUICreateColourBrush(background));
            else pane->canvas.Background(wxWinUICreateColourBrush(background));
            pane->background = background;
        }
        Check(s);
        std::set<Key> wanted;
        for ( const auto& cell : cells ) wanted.insert(cell.key);
        for ( auto it = pane->peers.begin(); it != pane->peers.end(); )
        {
            const auto peer = it->second;
            if ( !wanted.count(it->first) || peer->structure != s.structure ||
                 peer->columns != s.columns || peer->style != s.style )
            {
                it = pane->peers.erase(it); // Inert before Unloaded can call wx.
                peer->control.IsEnabled(false);
                // Disabling a focused native item can synchronously call wx
                // focus handlers, invalidating this snapshot. Finish removing
                // this exact, already-retired native peer before checking the
                // snapshot: after erasing its key no later retry can find it.
                // Only locally retained native objects are used below; no wx
                // owner/model access is allowed until Check() succeeds.
                uint32_t index = 0;
                if ( IsNativeItem(pane, peer->kind) )
                {
                    if ( pane->selector.Items().IndexOf(peer->control, index) ) pane->selector.Items().RemoveAt(index);
                }
                else if ( peer->mount && peer->mount.Children().IndexOf(peer->control, index) )
                    peer->mount.Children().RemoveAt(index);
                Check(s);
            }
            else ++it;
        }
        for ( const auto& cell : cells )
        {
            Check(s);
            const auto found = pane->peers.find(cell.key);
            const bool added = found == pane->peers.end();
            const auto peer = added ? MakePeer(pane, cell) : found->second;
            peer->structure = s.structure; peer->columns = s.columns; peer->style = s.style;
            if ( added || peer->painted.rect != cell.rect || peer->scale != scale )
            {
                peer->control.Width(cell.rect.width / scale);
                peer->control.Height(cell.rect.height / scale);
                C::Canvas::SetLeft(peer->control, cell.rect.x / scale);
                C::Canvas::SetTop(peer->control,
                    pane->mode == wxLC_REPORT && !pane->header && cell.kind != Kind::Row
                        ? 0 : cell.rect.y / scale);
            }
            if ( peer->control.IsEnabled() != owner->IsEnabled() ) peer->control.IsEnabled(owner->IsEnabled());
            if ( added || peer->painted.background != cell.background ||
                 peer->painted.customBackground != cell.customBackground )
            {
                if ( pane->selector && !cell.customBackground )
                    peer->control.ClearValue(C::Control::BackgroundProperty());
                else peer->control.Background(wxWinUICreateColourBrush(cell.background));
            }
            if ( added || peer->painted.foreground != cell.foreground )
                wxWinUIApplyForeground(peer->control, cell.foreground);
            Check(s);
            if ( cell.kind == Kind::Check )
            {
                const auto check = peer->control.as<C::CheckBox>();
                if ( !check.IsChecked() || check.IsChecked().Value() != cell.checked ) check.IsChecked(cell.checked);
            }
            else if ( cell.kind != Kind::Row )
            {
                if ( added || peer->painted.text != cell.text ) peer->text.Text(wxWinUIToHString(cell.text));
                if ( added || peer->painted.font != cell.font ) wxWinUIApplyFont(peer->text, cell.font);
                if ( added || peer->painted.foreground != cell.foreground ) wxWinUIApplyForeground(peer->text, cell.foreground);
                if ( peer->content && (added || peer->painted.vertical != cell.vertical) )
                    peer->content.Orientation(cell.vertical ? C::Orientation::Vertical : C::Orientation::Horizontal);
                if ( peer->content && (added || peer->painted.alignment != cell.alignment || peer->painted.vertical != cell.vertical) )
                    peer->content.HorizontalAlignment(cell.alignment == wxLIST_FORMAT_RIGHT ? X::HorizontalAlignment::Right :
                        cell.alignment == wxLIST_FORMAT_CENTRE || cell.vertical ? X::HorizontalAlignment::Center : X::HorizontalAlignment::Left);
                if ( peer->itemContent && (added || peer->painted.alignment != cell.alignment) )
                    peer->text.TextAlignment(cell.alignment == wxLIST_FORMAT_RIGHT ? X::TextAlignment::Right :
                        cell.alignment == wxLIST_FORMAT_CENTRE || cell.vertical ? X::TextAlignment::Center : X::TextAlignment::Left);
                if ( added || !peer->painted.bitmap.IsSameAs(cell.bitmap) )
                {
                    // MSW image lists return fresh bitmap handles even for the
                    // same image. Compare pixels before replacing the native
                    // ImageSource; RefreshItem must still observe Replace().
#if wxUSE_IMAGE
                    const wxImage image = cell.bitmap.IsOk() ? cell.bitmap.ConvertToImage() : wxImage();
                    if ( added || !SameImage(peer->paintedImage, image) )
#endif
                    {
                        peer->image.Source(cell.bitmap.IsOk() ? wxWinUIWriteableBitmapFromBitmap(cell.bitmap) : nullptr);
                        peer->image.Visibility(cell.bitmap.IsOk() ? X::Visibility::Visible : X::Visibility::Collapsed);
#if wxUSE_IMAGE
                        peer->paintedImage = image;
#endif
                    }
                }
                if ( cell.bitmap.IsOk() && (added || !peer->painted.bitmap.IsOk() ||
                     peer->painted.bitmap.GetLogicalSize() != cell.bitmap.GetLogicalSize() || peer->scale != scale) )
                {
                    peer->image.Width(cell.bitmap.GetLogicalWidth() / scale);
                    peer->image.Height(cell.bitmap.GetLogicalHeight() / scale);
                }
            }
            if ( added || peer->painted.text != cell.text )
                X::Automation::AutomationProperties::SetName(peer->control, wxWinUIToHString(cell.text));
            Check(s);
            peer->painted = cell;
            peer->scale = scale;
            if ( added )
            {
                pane->peers.emplace(cell.key, peer);
                if ( IsNativeItem(pane, cell.kind) )
                {
                    // Items are actual containers, kept in logical row order
                    // even when scrolling backwards realizes an earlier row.
                    uint32_t index = 0;
                    for ( const auto& entry : pane->peers )
                        if ( IsNativeItem(pane, entry.second->kind) && entry.first < cell.key ) ++index;
                    pane->selector.Items().InsertAt(index, peer->control);
                }
                else
                {
                    if ( pane->mode == wxLC_REPORT && !pane->header )
                    {
                        const auto row = pane->peers.find({cell.key.first, -2});
                        if ( row == pane->peers.end() ) throw InvalidListSnapshot{};
                        peer->mount = row->second->rowContent;
                    }
                    else peer->mount = pane->canvas;
                    peer->mount.Children().Append(peer->control);
                }
                Check(s);
            }
            if ( IsNativeItem(pane, cell.kind) )
            {
                const auto item = peer->control.as<P::SelectorItem>();
                if ( item.IsSelected() != cell.checked ) item.IsSelected(cell.checked);
                X::Automation::AutomationProperties::SetPositionInSet(item, static_cast<int>(cell.key.first + 1));
                X::Automation::AutomationProperties::SetSizeOfSet(item, static_cast<int>(s.main->GetItemCount()));
                Check(s);
            }
            if ( cell.current ) pane->host.SetPreferredFocus(peer->control);
        }
    }
    void Synchronize()
    {
        if ( closed || !owner || owner->IsBeingDeleted() || synchronizing || !owner->m_mainWin ) return;
        synchronizing = true;
        const auto finish = wxMakeGuard([this]() { synchronizing = false; });
        for ( unsigned attempt = 0; dirty && attempt != 2; ++attempt )
        {
            dirty = false;
            try
            {
                if ( owner->m_mainWin->m_dirty )
                {
                    owner->m_mainWin->RecalculatePositionsAndRefresh();
                    if ( closed || !owner || owner->IsBeingDeleted() ) return;
                    dirty = false;
                }
                const auto s = Capture();
                UpdatePanes(s);
                const auto currentPanes = panes;
                std::vector<std::vector<Cell>> presentations;
                for ( const auto& pane : currentPanes ) presentations.push_back(ReadPane(pane, s));
                Check(s);
                ++suppress;
                const auto restore = wxMakeGuard([this]() { --suppress; });
                for ( size_t n = 0; n < currentPanes.size(); ++n )
                    Publish(currentPanes[n], presentations[n], s);
                for ( auto it = automationCells.begin(); it != automationCells.end(); )
                    if ( !it->second.get() ) it = automationCells.erase(it); else ++it;
                if ( const auto pane = AutomationMainPane() )
                    if ( const auto peer = A::FrameworkElementAutomationPeer::FromElement(pane->selector) )
                    {
                        // Synchronous native notification only: never queue
                        // InvalidatePeer against a possibly retired peer.
                        peer.RaiseStructureChangedEvent(A::AutomationStructureChangeType::ChildrenInvalidated, nullptr);
                        Check(s);
                        PublishSelection();
                        Check(s);
                    }
            }
            catch ( const InvalidListSnapshot& )
            {
                if ( closed || !owner ) return;
                dirty = true;
            }
            catch ( const winrt::hresult_error& error )
            {
                wxWinUILogException("list native presentation", error);
                // Do not suppress generic drawing with a half-published peer.
                const auto retiring = std::move(panes);
                for ( const auto& pane : retiring ) ClosePane(pane);
                if ( owner && !closed ) owner->Refresh();
                return;
            }
        }
    }
    void Close()
    {
        if ( closed ) return;
        closed = true; ++revision;
        automationCells.clear();
        automationHeaders.clear();
        automationSelection.clear();
        automationSelectionKnown = false;
        const auto retiring = std::move(panes);
        for ( const auto& pane : retiring ) ClosePane(pane);
        owner = nullptr;
    }
};

void wxWinUIListProjection::Impl::AutomationCheck(const Snapshot& s, Key key) const
{
    if ( !IdentityCurrent(s) || (s.style & wxLC_MASK_TYPE) != wxLC_REPORT ||
         !AutomationMainPane() || key.first < -1 || key.first >= owner->GetItemCount() ||
         (key.second == -2 ? key.first < 0 : key.second < 0 || key.second >= s.columns) ) U::Unavailable();
}

std::shared_ptr<wxWinUIListProjection::Impl::Pane>
wxWinUIListProjection::Impl::AutomationMainPane() const
{
    if ( closed || !owner || owner->IsBeingDeleted() || !owner->HasFlag(wxLC_REPORT) ) return {};
    for ( const auto& pane : panes )
        if ( pane->published && !pane->header && pane->mode == wxLC_REPORT &&
             pane->window.get() == owner->m_mainWin ) return pane;
    return {};
}

std::pair<std::shared_ptr<wxWinUIListProjection::Impl::Pane>,
          std::shared_ptr<wxWinUIListProjection::Impl::Peer>>
wxWinUIListProjection::Impl::AutomationVisual(Key key) const
{
    for ( const auto& pane : panes )
    {
        const auto found = pane->peers.find(key);
        if ( found != pane->peers.end() && Current(pane, found->second) &&
             pane->window->IsShownOnScreen() && found->second->control.IsLoaded() &&
             X::Media::VisualTreeHelper::GetParent(found->second->control) )
            return {pane, found->second};
    }
    return {};
}

U::F::Rect wxWinUIListProjection::Impl::AutomationBounds(const Snapshot& s, Key key) const
{
    AutomationCheck(s, key);
    if ( key.second >= 0 && owner->GetColumnWidth(key.second) <= 0 ) return {};
    const auto visual = AutomationVisual(key);
    if ( !visual.first ) return {};
    const auto* const nativeHost = wxWinUITopLevelHost::FindSlotOwner(visual.first->window.get());
    if ( !nativeHost || !nativeHost->Root() ) return {};
    // Capture host-owned data before entering XAML. A native layout callback
    // can destroy wx and its host; only independent WinRT references survive.
    const auto nativeRoot = nativeHost->Root();
    const auto origin = nativeHost->GetClientScreenRect();
    const auto scale = static_cast<float>(nativeHost->GetScale());
    const auto element = visual.second->control;
    const auto viewport = visual.first->root;
    const auto rect = element.TransformToVisual(nativeRoot).TransformBounds(
        {0, 0, static_cast<float>(element.ActualWidth()), static_cast<float>(element.ActualHeight())});
    AutomationCheck(s, key);
    const auto clip = viewport.TransformToVisual(nativeRoot).TransformBounds(
        {0, 0, static_cast<float>(viewport.ActualWidth()), static_cast<float>(viewport.ActualHeight())});
    AutomationCheck(s, key);
    const float left = std::max(rect.X, clip.X), top = std::max(rect.Y, clip.Y);
    const float right = std::min(rect.X + rect.Width, clip.X + clip.Width);
    const float bottom = std::min(rect.Y + rect.Height, clip.Y + clip.Height);
    if ( right <= left || bottom <= top ) return {};
    return {origin.left + left * scale, origin.top + top * scale,
            (right - left) * scale, (bottom - top) * scale};
}

std::vector<long> wxWinUIListProjection::Impl::AutomationSelectedRows() const
{
    std::vector<long> selected;
    auto* const main = owner->m_mainWin;
    if ( main->IsVirtual() )
    {
        // The selection store already represents sparse/all-selected states.
        // Do not scan model values or the native viewport's SelectedItems.
        wxSelectionStore::IterationState cookie;
        for ( auto row = main->m_selStore.GetFirstSelectedItem(cookie);
              row != wxSelectionStore::NO_SELECTION;
              row = main->m_selStore.GetNextSelectedItem(cookie) )
            selected.push_back(static_cast<long>(row));
    }
    else
    {
        for ( size_t row = 0; row < main->m_lines.size(); ++row )
            if ( main->IsHighlighted(row) ) selected.push_back(static_cast<long>(row));
    }
    return selected;
}

void wxWinUIListProjection::Impl::AutomationSelectRow(const Snapshot& s, long row, int operation)
{
    const Key key{row, -2};
    AutomationCheck(s, key);
    if ( !owner->IsEnabled() ) throw winrt::hresult_error(UIA_E_ELEMENTNOTENABLED);
    if ( automationSelecting ) throw winrt::hresult_error(UIA_E_INVALIDOPERATION);
    automationSelecting = true;
    const auto restore = wxMakeGuard([this] { automationSelecting = false; });
    if ( operation == 1 && (s.style & wxLC_SINGLE_SEL) &&
         !s.main->IsHighlighted(row) && owner->GetSelectedItemCount() )
        throw winrt::hresult_error(UIA_E_INVALIDOPERATION);
    const auto current = [&] { return IdentityCurrent(s) && AutomationMainPane(); };
    if ( operation == 0 )
    {
        const auto selected = AutomationSelectedRows();
        for ( const auto oldRow : selected )
        {
            if ( oldRow != row ) s.main->SetItemState(oldRow, 0, wxLIST_STATE_SELECTED);
            if ( !current() ) return;
        }
    }
    s.main->SetItemState(row,
        operation == 2 ? 0 : wxLIST_STATE_SELECTED | wxLIST_STATE_FOCUSED,
        wxLIST_STATE_SELECTED | (operation == 2 ? 0 : wxLIST_STATE_FOCUSED));
    if ( !current() ) return;
    if ( operation != 2 )
    {
        s.main->m_anchor = static_cast<size_t>(row);
        s.main->SetFocus();
        if ( !current() ) return;
    }
    PublishSelection();
}

A::AutomationPeer wxWinUIListProjection::Impl::AutomationRow(long row)
{
    const Key key{row, -2};
    const auto identity = Capture();
    AutomationCheck(identity, key);
    if ( !IdentityCurrent(automationSchema) )
    {
        automationCells.clear();
        automationHeaders.clear();
        automationSchema = identity;
    }
    if ( const auto found = automationCells.find(key); found != automationCells.end() )
        if ( const auto peer = found->second.get() ) return peer;
    const std::weak_ptr<Impl> weak = shared_from_this();
    const std::weak_ptr<Pane> weakPane = AutomationMainPane();
    const auto resolve = [weak, weakPane, identity, key]
    {
        const auto self = weak.lock(); const auto pane = weakPane.lock();
        if ( !self || !pane || self->AutomationMainPane() != pane ) U::Unavailable();
        self->AutomationCheck(identity, key);
        return self;
    };
    auto binding = std::make_shared<U::RowBinding>();
    binding->check = [resolve] { (void)resolve(); };
    binding->selected = [resolve, row] { return resolve()->owner->m_mainWin->IsHighlighted(row); };
    binding->enabled = [resolve] { return resolve()->owner->IsEnabled(); };
    binding->position = [resolve, row] { (void)resolve(); return static_cast<int>(row + 1); };
    binding->count = [resolve] { return static_cast<int>(resolve()->owner->GetItemCount()); };
    binding->name = [resolve, row]() -> winrt::hstring
    {
        const auto self = resolve();
        if ( !self->owner->GetColumnCount() ) return {};
        const auto cell = self->AutomationCell({row, 0});
        const auto name = cell.GetName();
        resolve();
        return name;
    };
    binding->bounds = [resolve, identity, key] { return resolve()->AutomationBounds(identity, key); };
    binding->container = [resolve]
    {
        const auto self = resolve();
        const auto peer = A::FrameworkElementAutomationPeer::CreatePeerForElement(self->AutomationMainPane()->selector);
        resolve();
        return peer;
    };
    binding->children = [resolve, row]
    {
        const auto self = resolve();
        auto children = self->AutomationRowChildren(self->AutomationMainPane(), row);
        resolve();
        return children;
    };
    binding->change = [resolve, identity, row](int operation)
        { resolve()->AutomationSelectRow(identity, row, operation); };
    binding->realize = [resolve, identity, key] { resolve()->AutomationRealize(identity, key); };
    binding->focused = [resolve, key]
    {
        const auto self = resolve();
        const auto visual = self->AutomationVisual(key);
        const bool focused = visual.second && visual.second->control.FocusState() != X::FocusState::Unfocused;
        resolve();
        return focused;
    };
    binding->focus = [resolve, identity, key]
    {
        const auto self = resolve();
        self->AutomationRealize(identity, key);
        const auto visual = self->AutomationVisual(key);
        resolve();
        if ( !visual.first || !visual.second ) U::Unavailable();
        visual.first->host.SetPreferredFocus(visual.second->control);
        resolve();
        visual.first->host.NavigateFocus(true);
        resolve();
    };
    const auto parent = binding->container().as<A::ListViewBaseAutomationPeer>();
    resolve();
    const A::AutomationPeer peer = winrt::make<U::RowPeer>(winrt::box_value(static_cast<int64_t>(row)), parent, binding);
    resolve();
    automationCells[key] = winrt::make_weak(peer);
    return peer;
}

A::AutomationPeer wxWinUIListProjection::Impl::AutomationCell(Key key)
{
    const auto identity = Capture();
    AutomationCheck(identity, key);
    if ( !IdentityCurrent(automationSchema) )
    {
        automationCells.clear();
        automationHeaders.clear();
        automationSchema = identity;
    }
    if ( const auto found = automationCells.find(key); found != automationCells.end() )
        if ( const auto peer = found->second.get() ) return peer;
    const std::weak_ptr<Impl> weak = shared_from_this();
    const std::weak_ptr<Pane> weakPane = AutomationMainPane();
    const auto resolve = [weak, weakPane, identity, key]
    {
        const auto self = weak.lock(); const auto pane = weakPane.lock();
        if ( !self || !pane || !pane->published || self->AutomationMainPane() != pane ) U::Unavailable();
        self->AutomationCheck(identity, key);
        return self;
    };
    auto binding = std::make_shared<U::CellBinding>();
    binding->header = key.first == -1;
    binding->check = [resolve] { (void)resolve(); };
    binding->position = [resolve, key]
    {
        const auto self = resolve();
        return std::make_pair(static_cast<int>(key.first), self->owner->GetColumnOrder(key.second));
    };
    binding->name = [resolve, key]
    {
        const auto self = resolve();
        const auto snapshot = self->Capture();
        wxString name;
        if ( key.first < 0 ) name = snapshot.main->m_columns[key.second].GetText();
        else if ( snapshot.main->IsVirtual() )
            name = self->owner->OnGetItemText(key.first, key.second);
        else
        {
            const auto& line = snapshot.main->m_lines[static_cast<size_t>(key.first)];
            if ( static_cast<size_t>(key.second) < line.m_items.size() )
                name = line.m_items[key.second].GetText();
        }
        // A virtual getter may delete a row, replace the model or destroy wx.
        resolve();
        if ( snapshot.revision != self->revision ) U::Unavailable();
        return wxWinUIToHString(name);
    };
    binding->bounds = [resolve, identity, key] { return resolve()->AutomationBounds(identity, key); };
    binding->enabled = [resolve] { return resolve()->owner->IsEnabled(); };
    binding->grid = [resolve]
    {
        const auto self = resolve();
        return A::FrameworkElementAutomationPeer::CreatePeerForElement(self->AutomationMainPane()->selector);
    };
    binding->headerPeer = [resolve, key]() -> A::AutomationPeer
    {
        const auto self = resolve();
        return self->owner->HasFlag(wxLC_NO_HEADER) ? nullptr : self->AutomationCell({-1, key.second});
    };
    binding->realize = [resolve, identity, key] { resolve()->AutomationRealize(identity, key); };
    binding->readOnly = [resolve, key]
    {
        const auto self = resolve();
#if wxUSE_TEXTCTRL
        return key.second != 0 || !self->owner->HasFlag(wxLC_EDIT_LABELS);
#else
        wxUnusedVar(self);
        wxUnusedVar(key);
        return true;
#endif
    };
    binding->setValue = [resolve, identity, key](const winrt::hstring& value)
        { resolve()->AutomationSetValue(identity, key, value); };
    binding->focus = [resolve, identity, key]
    {
        const auto self = resolve();
        auto visual = self->AutomationVisual(key);
        if ( !visual.first )
        {
            self->AutomationRealize(identity, key);
            resolve();
            visual = self->AutomationVisual(key);
        }
        resolve();
        if ( !visual.first || !visual.second ) U::Unavailable();
        // This header has its own wx slot. A bare XAML/Button peer SetFocus
        // does not supersede a pending wx focus intent for the list's main
        // slot, which would take focus back at the next host flush. Request
        // the exact native Button through the existing HWND/XAML arbiter.
        visual.first->host.SetPreferredFocus(visual.second->control);
        resolve();
        if ( !self->Current(visual.first, visual.second) ) U::Unavailable();
        visual.first->host.NavigateFocus(true);
        resolve();
    };
    binding->invoke = [resolve, key]
    {
        const auto self = resolve();
        wxListEvent event(wxEVT_LIST_COL_CLICK, self->owner->GetId());
        event.SetEventObject(self->owner.get());
        event.SetColumn(key.second);
        self->owner->HandleWindowEvent(event);
        // No owner access after an application callback without a new guard.
    };
    binding->click = [weak, identity, key]
    {
        const auto self = weak.lock();
        if ( !self || !self->IdentityCurrent(identity) ) return;
        const auto visual = self->AutomationVisual(key);
        if ( visual.first && visual.second ) self->Activate(visual.first, visual.second);
    };
    A::AutomationPeer peer{nullptr};
    if ( key.first == -1 )
    {
        const C::Button button = winrt::make<U::HeaderElement>(binding);
        automationHeaders[key.second] = button;
        peer = A::FrameworkElementAutomationPeer::CreatePeerForElement(button);
    }
    else peer = winrt::make<U::CellPeer>(binding);
    automationCells[key] = winrt::make_weak(peer);
    return peer;
}

std::vector<A::AutomationPeer> wxWinUIListProjection::Impl::AutomationRowChildren(
    const std::shared_ptr<Pane>& pane, long row)
{
    std::vector<std::pair<int, A::AutomationPeer>> ordered;
    // Only materialized subitems: querying the tree never visits hidden rows
    // or forces every column/value to be loaded.
    for ( const auto& entry : pane->peers )
    {
        const auto& item = entry.second;
        if ( entry.first.first != row || !Current(pane, item) ) continue;
        if ( item->kind == Kind::Cell )
            ordered.emplace_back(owner->GetColumnOrder(entry.first.second) * 2 + 1,
                                 AutomationCell(entry.first));
        else if ( item->kind == Kind::Check )
            ordered.emplace_back(owner->GetColumnOrder(0) * 2,
                A::FrameworkElementAutomationPeer::CreatePeerForElement(item->control));
    }
    std::sort(ordered.begin(), ordered.end(), [](const auto& a, const auto& b) { return a.first < b.first; });
    std::vector<A::AutomationPeer> children;
    for ( const auto& entry : ordered ) if ( entry.second ) children.push_back(entry.second);
    return children;
}

std::shared_ptr<U::GridBinding> wxWinUIListProjection::Impl::AutomationBinding(const std::shared_ptr<Pane>& pane)
{
    const std::weak_ptr<Impl> weak = shared_from_this();
    const std::weak_ptr<Pane> weakPane = pane;
    const auto resolve = [weak, weakPane]
    {
        const auto self = weak.lock(); const auto live = weakPane.lock();
        if ( !self || !live || self->AutomationMainPane() != live ) U::Unavailable();
        return self;
    };
    auto binding = std::make_shared<U::GridBinding>();
    binding->check = [resolve]
    {
        const auto self = resolve();
        if ( !self->automationSelectionKnown )
        {
            self->automationSelection = self->AutomationSelectedRows();
            self->automationSelectionKnown = true;
        }
    };
    binding->multiple = [resolve] { return !resolve()->owner->HasFlag(wxLC_SINGLE_SEL); };
    binding->rows = [resolve] { return static_cast<int>(resolve()->owner->GetItemCount()); };
    binding->columns = [resolve] { return resolve()->owner->GetColumnCount(); };
    binding->cell = [resolve](int row, int column)
    {
        const auto self = resolve();
        return self->AutomationCell({row, self->owner->GetColumnIndexFromOrder(column)});
    };
    binding->parentRow = [resolve](int row) -> A::AutomationPeer
    {
        const auto self = resolve();
        return self->AutomationRow(row);
    };
    binding->selection = [resolve]
    {
        const auto self = resolve();
        const auto identity = self->Capture();
        const auto selected = self->AutomationSelectedRows();
        std::vector<A::AutomationPeer> rows;
        rows.reserve(selected.size());
        for ( const auto row : selected )
        {
            rows.push_back(self->AutomationRow(row));
            resolve();
            if ( !self->IdentityCurrent(identity) ) U::Unavailable();
        }
        // Native peer creation is a callback boundary too. Never return a
        // mixture of old and new selections if a nested writer changed it.
        if ( selected != self->AutomationSelectedRows() ) U::Unavailable();
        if ( !self->automationSelectionKnown )
        {
            self->automationSelection = selected;
            self->automationSelectionKnown = true;
        }
        return rows;
    };
    binding->headers = [resolve]
    {
        const auto self = resolve();
        std::vector<A::AutomationPeer> headers;
        if ( !self->owner->HasFlag(wxLC_NO_HEADER) )
            for ( int order = 0; order < self->owner->GetColumnCount(); ++order )
                headers.push_back(self->AutomationCell({-1, self->owner->GetColumnIndexFromOrder(order)}));
        return headers;
    };
    binding->children = [resolve]
    {
        const auto self = resolve();
        std::vector<A::AutomationPeer> children;
        const auto main = self->AutomationMainPane();
        for ( const auto& entry : main->peers )
            if ( entry.second->kind == Kind::Row && self->Current(main, entry.second) )
                children.push_back(A::FrameworkElementAutomationPeer::CreatePeerForElement(entry.second->control));
        return children;
    };
    return binding;
}

void wxWinUIListProjection::Impl::AutomationRealize(const Snapshot& s, Key key)
{
    AutomationCheck(s, key);
    if ( !owner->IsEnabled() ) throw winrt::hresult_error(UIA_E_ELEMENTNOTENABLED);
    const bool rowOnly = key.second == -2;
    if ( !owner->IsShownOnScreen() || (!rowOnly && owner->GetColumnWidth(key.second) <= 0) ||
         (key.first < 0 && owner->HasFlag(wxLC_NO_HEADER)) )
        throw winrt::hresult_error(UIA_E_INVALIDOPERATION);
    if ( key.first >= 0 )
    {
        owner->EnsureVisible(key.first);
        AutomationCheck(s, key);
    }
    if ( rowOnly )
    {
        Invalidate();
        Synchronize();
        AutomationCheck(s, key);
        return;
    }
    const int left = s.main->GetColumnStart(key.second);
    const int width = s.main->GetColumnWidth(key.second);
    const int viewport = s.main->GetClientSize().x;
    int unit = 0, x = 0, y = 0;
    owner->GetScrollPixelsPerUnit(&unit, nullptr);
    owner->GetViewStart(&x, &y);
    if ( unit > 0 && viewport > 0 )
    {
        const int offset = x * unit;
        int target = offset;
        if ( left < offset ) target = left;
        else if ( left + width > offset + viewport ) target = left + std::min(width, viewport) - viewport;
        if ( target != offset ) owner->Scroll(std::max(0, (target + unit - 1) / unit), y);
        AutomationCheck(s, key);
    }
    Invalidate();
    Synchronize();
    AutomationCheck(s, key);
}

void wxWinUIListProjection::Impl::AutomationSetValue(const Snapshot& s, Key key,
                                                   const winrt::hstring& value)
{
    AutomationCheck(s, key);
#if wxUSE_TEXTCTRL
    if ( !owner->IsEnabled() ) throw winrt::hresult_error(UIA_E_ELEMENTNOTENABLED);
    if ( key.first < 0 || key.second != 0 || !(s.style & wxLC_EDIT_LABELS) ||
         automationWriting || owner->GetEditControl() )
        throw winrt::hresult_error(UIA_E_INVALIDOPERATION);
    automationWriting = true;
    const auto finishWrite = wxMakeGuard([this] { automationWriting = false; });
    AutomationRealize(s, key);
    AutomationCheck(s, key);
    const wxWeakRef<wxListMainWindow> weakMain(s.main);
    wxTextCtrl* const editor = owner->EditLabel(key.first);
    const wxWeakRef<wxTextCtrl> weakEditor(editor);
    const wxWeakRef<wxListTextCtrlWrapper> weakWrapper(weakMain ? weakMain->m_textctrlWrapper : nullptr);
    const auto ownsEditor = [&]
    {
        return weakMain && weakWrapper && weakEditor &&
               weakMain->m_textctrlWrapper == weakWrapper.get() &&
               weakWrapper->GetText() == weakEditor.get();
    };
    // On failure retire only the editor this call created, never a newer
    // application edit. End_Destroy generates no spurious cancellation event.
    const auto retire = wxMakeGuard([&]
    {
        if ( ownsEditor() ) weakWrapper->EndEdit(wxListTextCtrlWrapper::End_Destroy);
    });
    AutomationCheck(s, key);
    if ( !editor ) throw winrt::hresult_error(UIA_E_INVALIDOPERATION); // BEGIN veto.
    if ( !ownsEditor() ) U::Unavailable();
    if ( !owner->IsEnabled() ) throw winrt::hresult_error(UIA_E_ELEMENTNOTENABLED);
    editor->SetValue(wxString(value.c_str(), value.size()));
    AutomationCheck(s, key);
    if ( !ownsEditor() ) throw winrt::hresult_error(UIA_E_INVALIDOPERATION);
    if ( !owner->IsEnabled() ) throw winrt::hresult_error(UIA_E_ELEMENTNOTENABLED);
    const bool accepted = weakWrapper->EndEdit(wxListTextCtrlWrapper::End_Accept);
    AutomationCheck(s, key);
    if ( !accepted ) throw winrt::hresult_error(UIA_E_INVALIDOPERATION); // END veto.
    // Virtual values are committed by the application's ordinary END handler,
    // just as for keyboard edits. There is no second model mutation here.
#else
    wxUnusedVar(value);
    throw winrt::hresult_error(UIA_E_INVALIDOPERATION);
#endif
}

wxWinUIListProjection::wxWinUIListProjection(std::shared_ptr<Impl> impl) : m_impl(std::move(impl)) {}
wxWinUIListProjection::~wxWinUIListProjection() { Close(); }
bool wxWinUIListProjection::MeasureItemText(wxWindow* window, const wxString& text,
                                           const wxFont& font, wxSize& size)
{
    // Keep WinRT headers out of the generic implementation and share the
    // port's existing typography/DPI measurement (and startup fallback).
    size = wxWinUIMeasureText(window, text, font);
    return size.x >= 0 && size.y >= 0;
}
std::shared_ptr<wxWinUIListProjection> wxWinUIListProjection::Create(wxGenericListCtrl* list)
{
    auto impl = std::make_shared<Impl>(list);
    auto projection = std::shared_ptr<wxWinUIListProjection>(new wxWinUIListProjection(impl));
    impl->Synchronize();
    return projection;
}
void wxWinUIListProjection::Invalidate() { m_impl->Invalidate(); }
void wxWinUIListProjection::SyncViewport() { m_impl->Invalidate(); }
void wxWinUIListProjection::NotifySelectionChanged() { m_impl->NotifySelectionChanged(); }
void wxWinUIListProjection::Update() { const auto impl = m_impl; impl->Synchronize(); }
void wxWinUIListProjection::Close() { m_impl->Close(); }
bool wxWinUIListProjection::OwnsWindow(wxWindow* window) const
{
    for ( const auto& pane : m_impl->panes )
    {
        if ( !pane->published || pane->window.get() != window || !pane->host.IsOk() ||
             pane->host.GetContent() != pane->root ) continue;
        const auto* host = wxWinUITopLevelHost::FindSlotOwner(window);
        const auto* slot = host ? host->FindSlot(window) : nullptr;
        if ( slot && slot->GetContent() == pane->root ) return true;
    }
    return false;
}
#endif
