/////////////////////////////////////////////////////////////////////////////
// Name:        src/winui/dataview.cpp
// Purpose:     Virtualized XAML presentation of the public wxDataView model
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_DATAVIEWCTRL

#include "wx/dataview.h"
#ifdef wxHAS_WINUI_DATAVIEWCTRL
#include "wx/dcmemory.h"
#include "wx/settings.h"
#include "wx/uilocale.h"
#include "wx/scopeguard.h"
#include "wx/weakref.h"
#include "wx/private/dataview.h"
#include "wx/winui/private/appearance.h"
#include "wx/winui/private/tlwhost.h"
#include "private.h"
#include "dataviewheader.h"
#include "datavieweditors.h"
#include "dataviewautomation.h"
#include <winrt/Microsoft.UI.Xaml.Automation.h>
#include <winrt/Windows.Globalization.h>
#include <winrt/Windows.Globalization.NumberFormatting.h>
#include <algorithm>
#include <cmath>
#include <map>
#include <unordered_set>
#include <unordered_map>
#include <vector>

namespace X = winrt::Microsoft::UI::Xaml;
namespace C = winrt::Microsoft::UI::Xaml::Controls;
namespace F = winrt::Windows::Foundation;
namespace U = wxWinUIDataViewAutomation;

// The chevron of a WinUI TreeView, from the font of its icons.
static void SetExpanderGlyph(
    const winrt::Microsoft::UI::Xaml::Controls::Button& toggle, bool opened)
{
    winrt::Microsoft::UI::Xaml::Controls::FontIcon icon;
    icon.Glyph(opened ? L"\uE70D" : L"\uE76C");
    icon.FontSize(12);
    toggle.Content(icon);
}

namespace
{
using ItemKey = std::uint64_t;

ItemKey Key(const wxDataViewItem& item)
{
    return static_cast<ItemKey>(reinterpret_cast<std::uintptr_t>(item.GetID()));
}

wxDataViewItem Item(ItemKey key)
{
    return wxDataViewItem(reinterpret_cast<void*>(static_cast<std::uintptr_t>(key)));
}

ItemKey Key(const F::IInspectable& value)
{
    return value ? winrt::unbox_value<ItemKey>(value) : 0;
}

C::ScrollViewer FindScrollViewer(const X::DependencyObject& parent)
{
    if ( auto scroll = parent.try_as<C::ScrollViewer>() )
        return scroll;
    const int count = X::Media::VisualTreeHelper::GetChildrenCount(parent);
    for ( int n = 0; n < count; ++n )
    {
        if ( auto scroll = FindScrollViewer(
                X::Media::VisualTreeHelper::GetChild(parent, n)) )
            return scroll;
    }
    return nullptr;
}

C::TextBox FindEditorTextBox(const X::DependencyObject& parent)
{
    if ( auto text = parent.try_as<C::TextBox>() ) return text;
    for ( int n = 0; n < X::Media::VisualTreeHelper::GetChildrenCount(parent); ++n )
        if ( auto text = FindEditorTextBox(X::Media::VisualTreeHelper::GetChild(parent, n)) )
            return text;
    return nullptr;
}

struct ModelPin
{
    explicit ModelPin(wxDataViewModel* value) : model(value)
    {
        if ( model ) model->IncRef();
    }
    ~ModelPin() { if ( model ) model->DecRef(); }
    wxDataViewModel* model;
};

struct InvalidatedDataViewSnapshot {};

enum class NativeEditorKind { Custom, Text, Choice, ChoiceIndex, Spin, Date };

NativeEditorKind EditorKind(wxDataViewRenderer* renderer)
{
    if ( renderer->IsKindOf(wxCLASSINFO(wxDataViewChoiceByIndexRenderer)) ) return NativeEditorKind::ChoiceIndex;
    if ( renderer->IsKindOf(wxCLASSINFO(wxDataViewChoiceRenderer)) ) return NativeEditorKind::Choice;
#if wxUSE_SPINCTRL
    if ( renderer->IsKindOf(wxCLASSINFO(wxDataViewSpinRenderer)) ) return NativeEditorKind::Spin;
#endif
#if wxUSE_DATEPICKCTRL
    if ( renderer->IsKindOf(wxCLASSINFO(wxDataViewDateRenderer)) ) return NativeEditorKind::Date;
#endif
    return renderer->GetClassInfo() == wxCLASSINFO(wxDataViewTextRenderer)
        ? NativeEditorKind::Text : NativeEditorKind::Custom;
}

#if wxUSE_DATEPICKCTRL
bool ToNativeCivilDate(const wxDateTime& date, F::DateTime* value)
{
    if ( !date.IsValid() ) return false;
    try
    {
        namespace G = winrt::Windows::Globalization;
        G::Calendar calendar;
        calendar.ChangeCalendarSystem(G::CalendarIdentifiers::Gregorian());
        calendar.ChangeClock(G::ClockIdentifiers::TwentyFourHour());
        // Match wxDatePickerCtrl: civil dates cross WinRT at local noon,
        // avoiding DST gaps and preserving dates before the FILETIME epoch.
        calendar.SetToNow();
        calendar.Hour(12); calendar.Minute(0); calendar.Second(0); calendar.Nanosecond(0);
        calendar.Day(1); calendar.Month(1); calendar.Year(date.GetYear());
        calendar.Month(date.GetMonth() + 1); calendar.Day(date.GetDay());
        *value = calendar.GetDateTime();
        return calendar.Year() == date.GetYear() && calendar.Month() == date.GetMonth() + 1 &&
               calendar.Day() == date.GetDay();
    }
    catch ( const winrt::hresult_error& ) { return false; }
}

wxDateTime FromNativeCivilDate(const F::DateTime& value)
{
    namespace G = winrt::Windows::Globalization;
    G::Calendar calendar;
    calendar.ChangeCalendarSystem(G::CalendarIdentifiers::Gregorian());
    calendar.SetDateTime(value);
    return wxDateTime(static_cast<wxDateTime::wxDateTime_t>(calendar.Day()),
        static_cast<wxDateTime::Month>(calendar.Month() - 1), calendar.Year());
}
#endif // wxUSE_DATEPICKCTRL
}

class wxWinUIDataViewImpl : public std::enable_shared_from_this<wxWinUIDataViewImpl>
{
public:
    explicit wxWinUIDataViewImpl(wxDataViewCtrl* view) : owner(view) {}

    struct Row { ItemKey key; unsigned depth; bool container; };
    wxWeakRef<wxDataViewCtrl> owner;
    wxWinUIControlHost host;
    C::Grid root{nullptr};
    C::ListView list{nullptr};
    C::Canvas header{nullptr};
    C::ScrollViewer headerScroll{nullptr};
    // False when the program asked for no border: no frame around the table.
    bool framed = true;
    std::unique_ptr<wxWinUIDataViewHeader::InsertionMarker> columnInsertionMarker;
    wxDataViewColumn* columnInsertionOwner = nullptr;
    C::ScrollViewer scroll{nullptr};
    C::Control editor{nullptr};
    C::Canvas editorOverlay{nullptr};
    NativeEditorKind editorKind = NativeEditorKind::Custom;
    bool editorFocusCheckQueued = false;
    double editorHorizontalOffset = 0;
    double editorVerticalOffset = 0;
    std::shared_ptr<wxWinUIPhysicalDisconnectGate> editorGate;
    std::shared_ptr<wxWinUIDataViewEditorRetirement> editorRetirement;
    unsigned retiringEditors = 0;
    bool pendingProjection = false;
    winrt::Windows::Foundation::Collections::IObservableVector<F::IInspectable>
        source{nullptr};
    winrt::event_token selectionToken{}, contentToken{}, loadedToken{}, scrollToken{};
    std::vector<Row> rows;
    wxVector<wxDataViewColumn*> columns;
    wxVector<wxDataViewColumn*> displayColumns;
    std::unordered_map<const wxDataViewColumn*, unsigned int> widthCache;
    std::unordered_map<wxDataViewColumn*,
        std::shared_ptr<wxWinUIDataViewHeader::Header>> headers;
    struct AutomationCell
    {
        U::A::AutomationPeer peer{nullptr};
        winrt::weak_ref<X::FrameworkElement> visual;
        winrt::weak_ref<X::FrameworkElement> content;
        winrt::weak_ref<C::Button> expander;
        double indent = 0;
        unsigned depth = 0;
        bool container = false;
        bool expanded = false;
        // A presentation has a separate lifetime from its model-backed UIA
        // peer. Unrelated value notifications advance this epoch in place;
        // replacing/recycling the native cell retires its event handlers.
        std::shared_ptr<std::uint64_t> presentation;
        bool custom = false;
        bool selected = false;
        bool focused = false;
        wxDataViewRenderer* renderer = nullptr;
        wxDataViewCellMode mode = wxDATAVIEW_CELL_INERT;
        int alignment = 0;
    };
    std::unordered_map<ItemKey, std::unordered_map<wxDataViewColumn*, AutomationCell>> automationCells;
    std::unordered_map<ItemKey, std::shared_ptr<int>> automationRows;
    std::unordered_map<wxDataViewColumn*, std::shared_ptr<int>> automationColumns;
    std::unordered_map<const wxDataViewColumn*, double> headerWidths;
    bool automationAllHeaders = false;
    bool automationHeaderQuery = false;
    bool automationWriteInProgress = false;
    bool nativeFocused = false;
    std::unordered_set<ItemKey> expanded;
    std::unordered_set<ItemKey> selected;
    wxDataViewModelNotifier* notifier = nullptr;
    std::uint64_t generation = 0;
    unsigned suppressSelection = 0;
    bool rebuilding = false;
    bool publishingRows = false;
    bool refreshingVisuals = false;
    bool rebuildQueued = false;
    bool columnMutation = false;
    bool multiSort = false;
    bool endingEdit = false;
    bool layingOut = false;
    ItemKey current = 0;
    ItemKey editingItem = 0;
    wxDataViewColumn* currentColumn = nullptr;
    wxDataViewColumn* editingColumn = nullptr;
    wxVariant editingValue;
    wxDataViewRendererBase* nativeEditRenderer = nullptr;
    wxPrivate::DataViewEditGeneration editGeneration = 0;
    // The least height of a row of a list of WinUI, as in WinUI.TableView.
    double rowHeight = 40;

    double Scale() const
    {
        return owner ? std::max(0.01, owner->GetDPIScaleFactor()) : 1.0;
    }
    bool Alive(std::uint64_t stamp) const
    {
        return owner && generation == stamp;
    }
    int ColumnIndex(const wxDataViewColumn* col) const
    {
        const auto it = std::find(columns.begin(), columns.end(), col);
        return it == columns.end() ? wxNOT_FOUND : static_cast<int>(it - columns.begin());
    }
    int ColumnPosition(const wxDataViewColumn* col) const
    {
        const auto it = std::find(displayColumns.begin(), displayColumns.end(), col);
        return it == displayColumns.end() ? wxNOT_FOUND : static_cast<int>(it - displayColumns.begin());
    }
    int RowIndex(ItemKey key) const
    {
        for ( size_t n = 0; n < rows.size(); ++n )
            if ( rows[n].key == key ) return static_cast<int>(n);
        return wxNOT_FOUND;
    }
    double ColumnWidth(const wxDataViewColumn* col) const
    {
        // A header-only accessibility request must never autosize by reading
        // model rows. Use the widths from the last committed presentation.
        if ( automationHeaderQuery )
        {
            const auto found = headerWidths.find(col);
            if ( found == headerWidths.end() ) U::Unavailable();
            return found->second;
        }
        return col->GetWidth() / Scale();
    }
    double TotalWidth() const
    {
        const auto stamp = generation;
        const auto snapshot = columns;
        double result = 0;
        for ( const auto col : snapshot )
        {
            if ( !Alive(stamp) ) return 0;
            if ( !col->IsHidden() ) result += ColumnWidth(col);
            if ( !Alive(stamp) ) return 0;
        }
        return result;
    }
    bool VisibleColumn(double left, double width) const
    {
        const double offset = scroll ? scroll.HorizontalOffset() : 0;
        const double viewport = list ? std::max(1.0, list.ActualWidth()) : 1;
        return left + width >= offset && left <= offset + viewport;
    }

    bool Send(wxEventType type, ItemKey key,
               wxDataViewColumn* column = nullptr,
               const wxVariant* value = nullptr, bool cancelled = false)
    {
        const auto keepAlive = shared_from_this();
        if ( !owner || (column && ColumnIndex(column) == wxNOT_FOUND) ) return false;
        const auto stamp = generation;
        wxDataViewEvent event(type, owner.get(), column, Item(key));
        if ( value ) event.SetValue(*value);
        if ( cancelled ) event.SetEditCancelled();
        owner->GetEventHandler()->ProcessEvent(event);
        return Alive(stamp) && event.IsAllowed();
    }

    bool Create()
    {
        const auto keepAlive = shared_from_this();
        if ( !host.Initialize(owner.get()) ) return false;
        editorGate = wxWinUICreateCompositePhysicalDisconnectGate();
        host.SetPhysicalDisconnectGate(editorGate);
        root = C::Grid();
        C::RowDefinition heading, body;
        heading.Height({owner->HasFlag(wxDV_NO_HEADER) ? 0.0 : 32.0, X::GridUnitType::Pixel});
        body.Height({1, X::GridUnitType::Star});
        root.RowDefinitions().Append(heading);
        root.RowDefinitions().Append(body);
        header = C::Canvas();
        header.Height(32);
        headerScroll = C::ScrollViewer();
        headerScroll.HorizontalScrollBarVisibility(C::ScrollBarVisibility::Hidden);
        headerScroll.VerticalScrollBarVisibility(C::ScrollBarVisibility::Disabled);
        headerScroll.VerticalScrollMode(C::ScrollMode::Disabled);
        headerScroll.Content(header);
        // The band of the header row, under the buttons of the columns, with
        // the corners of the frame.
        // The header stays the first child of the root, as what looks for
        // it expects: the band is put under it by its Z index.
        root.Children().Append(headerScroll);
        if ( C::Border band = wxWinUICreateTableHeaderBand(framed ? 4 : 0) )
        {
            C::Canvas::SetZIndex(band, -1);
            root.Children().Append(band);
        }
        list = winrt::make<U::GridElement>(CreateAutomationBinding());
        list.IsItemClickEnabled(false);
        list.SelectionMode(owner->HasFlag(wxDV_MULTIPLE)
                               ? C::ListViewSelectionMode::Extended
                               : C::ListViewSelectionMode::Single);
        list.ItemsPanel(X::Markup::XamlReader::Load(
            LR"(<ItemsPanelTemplate xmlns="http://schemas.microsoft.com/winfx/2006/xaml/presentation"><ItemsStackPanel Orientation="Vertical" /></ItemsPanelTemplate>)")
                .as<C::ItemsPanelTemplate>());
        list.ItemTemplate(X::Markup::XamlReader::Load(
            LR"(<DataTemplate xmlns="http://schemas.microsoft.com/winfx/2006/xaml/presentation"><Canvas Height="40" /></DataTemplate>)")
                .as<X::DataTemplate>());
        list.ItemContainerStyle(X::Markup::XamlReader::Load(
            LR"(<Style xmlns="http://schemas.microsoft.com/winfx/2006/xaml/presentation" TargetType="ListViewItem"><Setter Property="HorizontalContentAlignment" Value="Stretch"/><Setter Property="Padding" Value="0"/><Setter Property="MinHeight" Value="0"/></Style>)")
                .as<X::Style>());
        C::ScrollViewer::SetHorizontalScrollMode(list, C::ScrollMode::Enabled);
        C::ScrollViewer::SetHorizontalScrollBarVisibility(list, C::ScrollBarVisibility::Auto);
        source = winrt::single_threaded_observable_vector<F::IInspectable>();
        list.ItemsSource(source);
        C::Grid::SetRow(list, 1);
        root.Children().Append(list);
        // Popup editors must not be mounted under a recyclable ListViewItem.
        // The stable overlay shares the host's physical-disconnect gate.
        editorOverlay = C::Canvas();
        C::Grid::SetRow(editorOverlay, 1);
        root.Children().Append(editorOverlay);
        // The frame of a table of WinUI: the stroke of a control, with its
        // corners, over the edge of the header and of the rows.
        if ( framed )
        {
            if ( C::Border frame = wxWinUICreateThemeBrushBorder(
                    "ControlStrokeColorDefaultBrush", wxWinUIThemeBrushProperty::BorderBrush) )
            {
                frame.IsHitTestVisible(false);
                frame.BorderThickness({1, 1, 1, 1});
                frame.CornerRadius({4, 4, 4, 4});
                C::Grid::SetRowSpan(frame, 2);
                root.Children().Append(frame);
            }
        }
        wxWinUIApplyFont(list, owner->GetFont());
        const std::weak_ptr<wxWinUIDataViewImpl> weak = shared_from_this();
        selectionToken = list.SelectionChanged([weak](const auto&, const auto&)
        {
            auto self = weak.lock();
            if ( !self || !self->owner || self->suppressSelection ||
                 self->retiringEditors || self->pendingProjection ) return;
            self->selected.clear();
            for ( const auto& value : self->list.SelectedItems() )
                self->selected.insert(Key(value));
            self->current = Key(self->list.SelectedItem());
            self->RefreshVisuals();
            self->Send(wxEVT_DATAVIEW_SELECTION_CHANGED, self->current);
        });
        contentToken = list.ContainerContentChanging([weak](const auto&,
                                     const C::ContainerContentChangingEventArgs& args)
        {
            auto self = weak.lock();
            if ( !self || !self->owner ) return;
            if ( self->publishingRows )
            {
                // The vector sends synchronous notifications between splice
                // steps. Final RefreshVisuals realizes the coherent sequence.
                args.Handled(true);
                return;
            }
            if ( args.InRecycleQueue() )
            {
                if ( auto canvas = args.ItemContainer().ContentTemplateRoot().try_as<C::Canvas>() )
                {
                    if ( self->editor &&
                         X::Media::VisualTreeHelper::GetParent(self->editor) == canvas )
                        self->EndEdit(true);
                    canvas.Children().Clear();
                }
            }
            else
            {
                self->Realize(args.ItemContainer().as<C::ListViewItem>(), Key(args.Item()));
                if ( !args.ItemContainer().ContentTemplateRoot() && args.Phase() == 0 )
                    args.RegisterUpdateCallback(1, [weak](const auto&, const C::ContainerContentChangingEventArgs& update)
                    {
                        if ( auto live = weak.lock() )
                            if ( live->owner && !update.InRecycleQueue() )
                                live->Realize(update.ItemContainer().as<C::ListViewItem>(), Key(update.Item()));
                        update.Handled(true);
                    });
            }
            args.Handled(true);
        });
        loadedToken = list.Loaded([weak](const auto&, const auto&)
        {
            if ( auto self = weak.lock() ) self->ConnectScroll();
        });
        root.GotFocus([weak](const auto&, const auto&)
        {
            if ( auto self = weak.lock() )
                if ( self->owner ) self->RefreshVisuals();
        });
        root.LostFocus([weak](const auto&, const auto&)
        {
            if ( auto self = weak.lock() )
                if ( self->owner ) self->RefreshVisuals();
        });
        list.ActualThemeChanged([weak](const auto&, const auto&)
        {
            if ( auto self = weak.lock() )
                if ( self->owner ) self->ModelChanged();
        });
        // A window resize is geometry, not a model notification. Publish on
        // the size stack too: CallAfter alone does not run during USER32's
        // modal sizing loop. The native event covers the final viewport after
        // the host has committed the corresponding slot rectangle.
        owner->Bind(wxEVT_SIZE, [weak](wxSizeEvent& event)
        {
            if ( const auto self = weak.lock() ) self->GeometryChanged();
            event.Skip();
        });
        list.SizeChanged([weak](const auto&, const auto&)
        {
            if ( const auto self = weak.lock() ) self->GeometryChanged();
        });
        return host.SetContent(root, list);
    }

    void ConnectScroll()
    {
        if ( !owner || scroll ) return;
        scroll = FindScrollViewer(list);
        if ( !scroll ) return;
        const std::weak_ptr<wxWinUIDataViewImpl> weak = shared_from_this();
        scrollToken = scroll.ViewChanged([weak](const auto&, const auto&)
        {
            if ( auto self = weak.lock() )
            {
                if ( !self->owner ) return;
                // ViewChanged may report the EnsureVisible request which ran
                // before this editor was published. Only actual displacement
                // invalidates the stable overlay's captured row position.
                if ( self->editorRetirement &&
                     (self->scroll.HorizontalOffset() != self->editorHorizontalOffset ||
                      self->scroll.VerticalOffset() != self->editorVerticalOffset) )
                    self->EndEdit(false);
                if ( !self->owner ) return;
                self->headerScroll.ChangeView(self->scroll.HorizontalOffset(), nullptr, nullptr, true);
                self->RefreshVisuals();
            }
        });
        RefreshVisuals();
    }

    void Close()
    {
        wxPrivate::EndDataViewEdit(owner.get(), nativeEditRenderer, editGeneration);
        nativeEditRenderer = nullptr;
        editGeneration = 0;
        owner = nullptr;
        ++generation;
        ResetAutomationRows();
        automationColumns.clear();
        RetireEditor();
        auto oldHeaders = std::move(headers);
        for ( const auto& entry : oldHeaders ) entry.second->Retire();
        if ( list )
        {
            list.SelectionChanged(selectionToken);
            list.ContainerContentChanging(contentToken);
            list.Loaded(loadedToken);
        }
        if ( scroll ) scroll.ViewChanged(scrollToken);
        host.Close(editorGate);
    }

    void QueueRebuild()
    {
        if ( !owner || rebuildQueued ) return;
        rebuildQueued = true;
        const std::weak_ptr<wxWinUIDataViewImpl> weak = shared_from_this();
        owner->CallAfter([weak]
        {
            if ( auto self = weak.lock() )
            {
                self->rebuildQueued = false;
                self->Rebuild();
            }
        });
    }

    void ModelChanged(ItemKey changedItem = 0, int changedColumn = wxNOT_FOUND)
    {
        const auto keepAlive = shared_from_this();
        const auto previousGeneration = generation;
        // A value used for ordering changes the flattened projection. Keep
        // that structural path explicit instead of reusing a stale row order.
        if ( changedItem && owner )
        {
            const auto snapshot = columns;
            for ( auto col : snapshot )
            {
                if ( !Alive(previousGeneration) ) return;
                const bool sorting = col->IsSortKey();
                if ( !Alive(previousGeneration) ) return;
                if ( sorting && (changedColumn == wxNOT_FOUND ||
                        col->GetModelColumn() == static_cast<unsigned>(changedColumn)) )
                { changedItem = 0; break; }
            }
        }
        ++generation;
        const auto stamp = generation;
        PruneAutomationColumns();
        if ( !Alive(stamp) ) return;
        if ( changedItem )
        {
            for ( auto& row : automationCells )
                for ( auto& cell : row.second )
                    if ( cell.second.presentation && *cell.second.presentation == previousGeneration &&
                         (row.first != changedItem || (changedColumn != wxNOT_FOUND &&
                          cell.first->GetModelColumn() != static_cast<unsigned>(changedColumn))) )
                        *cell.second.presentation = generation;
            for ( auto col : columns )
                if ( changedColumn == wxNOT_FOUND ||
                     col->GetModelColumn() == static_cast<unsigned>(changedColumn) )
                    widthCache.erase(col);
        }
        else widthCache.clear();
        if ( editingItem && !endingEdit ) EndEdit(true);
        if ( !owner ) return;
        // A callback from the candidate build is retried by that build itself.
        // Queuing here would let a self-notifying model spin CallAfter forever.
        if ( rebuilding ) return;
        if ( columnMutation ) QueueRebuild();
        else if ( changedItem ) RefreshVisuals();
        else Rebuild();
    }

    void GeometryChanged(wxDataViewColumn* column = nullptr)
    {
        const auto keepAlive = shared_from_this();
        if ( !owner || !list ) return;
        const auto previous = generation++;
        // Keep handlers/providers tied to the same cells valid, while still
        // invalidating an in-flight geometry snapshot on reentrant resize.
        for ( auto& row : automationCells )
            for ( auto& cell : row.second )
                if ( cell.second.presentation && *cell.second.presentation == previous )
                    *cell.second.presentation = generation;
        if ( column ) widthCache.erase(column);
        if ( rebuilding || columnMutation ) return;
        RefreshVisuals();
    }

    void VisibleStructureChanged(ItemKey key, bool opening)
    {
        const auto keepAlive = shared_from_this();
        if ( !owner || !list ) return;
        const auto previous = generation++;
        for ( auto& row : automationCells )
            for ( auto& cell : row.second )
                if ( cell.second.presentation && *cell.second.presentation == previous )
                    *cell.second.presentation = generation;
        // Autosize measures the visible rows, so opening a branch may change
        // that result. Explicit widths never consult this cache or the model.
        widthCache.clear();
        if ( rebuilding ) return;
        if ( retiringEditors ) { pendingProjection = true; return; }
        bool committed = false;
        {
            rebuilding = true;
            auto guard = wxMakeGuard([this] { rebuilding = false; });
            const auto stamp = generation;
            const int index = RowIndex(key);
            if ( index == wxNOT_FOUND ) return; // A hidden descendant has no visual rows yet.
            ModelPin model(owner->GetModel());
            if ( !model.model ) return;
            auto candidate = rows;
            const auto first = candidate.begin() + index + 1;
            auto last = first;
            while ( last != candidate.end() && last->depth > candidate[index].depth ) ++last;
            candidate.erase(first, last);
            std::vector<Row> children;
            std::unordered_set<ItemKey> seen;
            for ( const auto& row : candidate ) seen.insert(row.key);
            if ( !opening || Walk(model.model, Item(key), rows[index].depth + 1,
                                  children, seen, stamp) )
            {
                if ( Alive(stamp) )
                {
                    candidate.insert(candidate.begin() + index + 1, children.begin(), children.end());
                    committed = PublishRows(std::move(candidate), stamp, true);
                }
            }
        }
        // Public/model callbacks may supersede the branch operation. Reconcile
        // once through the existing bounded model transaction, never apply the
        // old candidate over their changes.
        if ( !committed && owner ) Rebuild();
    }

    bool Walk(wxDataViewModel* model, const wxDataViewItem& parent,
               unsigned depth, std::vector<Row>& result,
               std::unordered_set<ItemKey>& seen, std::uint64_t stamp)
    {
        if ( depth > 1024 || !Alive(stamp) ) return false;
        wxDataViewItemArray children;
        const bool virtualList = model->IsVirtualListModel();
        if ( !Alive(stamp) ) return false;
        if ( virtualList && !parent.IsOk() )
        {
            // wx virtual lists deliberately return no children. Their public
            // identity contract is GetCount()/GetItem(row), not GetChildren().
            auto virtualModel = static_cast<wxDataViewVirtualListModel*>(model);
            const unsigned count = virtualModel->GetCount();
            if ( !Alive(stamp) ) return false;
            children.Alloc(count);
            for ( unsigned n = 0; n < count; ++n ) children.Add(virtualModel->GetItem(n));
        }
        else model->GetChildren(parent, children);
        if ( !Alive(stamp) ) return false;
        const auto sortColumns = owner->GetSortingColumns();
        if ( !Alive(stamp) ) return false;
        if ( !sortColumns.empty() )
        {
            // Never retain a column pointer across a model comparison: its
            // callback can remove columns or replace the entire model.
            struct SortKey { unsigned column; bool ascending; };
            std::vector<SortKey> keys;
            for ( auto col : sortColumns )
            {
                if ( ColumnIndex(col) == wxNOT_FOUND ) return false;
                keys.push_back({col->GetModelColumn(), col->IsSortOrderAscending()});
                if ( !Alive(stamp) ) return false;
            }
            try
            {
                std::stable_sort(children.begin(), children.end(), [&](const auto& a, const auto& b)
                {
                    if ( !Alive(stamp) ) throw InvalidatedDataViewSnapshot();
                    for ( const auto& key : keys )
                    {
                        const int cmp = model->Compare(a, b, key.column, key.ascending);
                        if ( !Alive(stamp) ) throw InvalidatedDataViewSnapshot();
                        if ( cmp ) return cmp < 0;
                    }
                    return false;
                });
            }
            catch ( const InvalidatedDataViewSnapshot& ) { return false; }
            if ( !Alive(stamp) ) return false;
        }
        for ( const auto& child : children )
        {
            const auto key = Key(child);
            if ( !key || !seen.insert(key).second ) continue;
            const bool container = model->IsContainer(child);
            if ( !Alive(stamp) ) return false;
            result.push_back({key, depth, container});
            if ( container && expanded.count(key) &&
                    !Walk(model, child, depth + 1, result, seen, stamp) ) return false;
        }
        return true;
    }

    bool PublishRows(std::vector<Row> candidate, std::uint64_t stamp,
                     bool incremental = false)
    {
        if ( !Alive(stamp) ) return false;
        if ( editingItem && std::none_of(candidate.begin(), candidate.end(),
                [this](const Row& row) { return row.key == editingItem; }) )
        {
            EndEdit(true);
            if ( !Alive(stamp) || retiringEditors )
            {
                if ( owner && retiringEditors ) pendingProjection = true;
                return false;
            }
        }
        ++suppressSelection;
        auto selectionGuard = wxMakeGuard([this] { --suppressSelection; });
        {
            publishingRows = true;
            auto publicationGuard = wxMakeGuard([this] { publishingRows = false; });
            if ( !incremental || source.Size() != rows.size() )
            {
                // Genuine model replacement/reordering keeps the bulk path;
                // inserting an entire large virtual model one row at a time
                // would produce quadratic native vector notifications.
                const bool sameItems = source.Size() == candidate.size() &&
                    rows.size() == candidate.size() &&
                    std::equal(rows.begin(), rows.end(), candidate.begin(),
                        [](const Row& a, const Row& b) { return a.key == b.key; });
                rows = std::move(candidate);
                PruneAutomationRows();
                if ( !sameItems )
                {
                    std::vector<F::IInspectable> values;
                    values.reserve(rows.size());
                    for ( const auto& row : rows ) values.push_back(winrt::box_value(row.key));
                    source.ReplaceAll(values);
                }
                if ( !Alive(stamp) ) return false;
            }
            else
            {
                size_t prefix = 0;
                while ( prefix < rows.size() && prefix < candidate.size() &&
                        rows[prefix].key == candidate[prefix].key ) ++prefix;
                size_t oldEnd = rows.size(), newEnd = candidate.size();
                while ( oldEnd > prefix && newEnd > prefix &&
                        rows[oldEnd - 1].key == candidate[newEnd - 1].key )
                { --oldEnd; --newEnd; }
                std::unordered_set<ItemKey> survivingKeys;
                for ( const auto& row : candidate ) survivingKeys.insert(row.key);
                // Keep the canonical row sequence in lockstep with each
                // synchronous vector notification. The common prefix/suffix
                // retain their boxed values, containers and UIA providers.
                while ( oldEnd > prefix )
                {
                    const size_t index = --oldEnd;
                    const auto removed = rows[index].key;
                    rows.erase(rows.begin() + index);
                    if ( !survivingKeys.count(removed) )
                    {
                        automationRows.erase(removed);
                        automationCells.erase(removed);
                    }
                    source.RemoveAt(static_cast<uint32_t>(index));
                    if ( !Alive(stamp) ) return false;
                }
                for ( size_t index = prefix; index < newEnd; ++index )
                {
                    rows.insert(rows.begin() + index, candidate[index]);
                    source.InsertAt(static_cast<uint32_t>(index), winrt::box_value(candidate[index].key));
                    if ( !Alive(stamp) ) return false;
                }
                rows = std::move(candidate); // Updated depth/container metadata.
                PruneAutomationRows();
            }
        }
        RestoreSelection();
        if ( !Alive(stamp) ) return false;
        const auto peer = U::A::FrameworkElementAutomationPeer::FromElement(list);
        if ( peer )
        {
            // Do not use InvalidatePeer: its delayed native unpeg can outlive
            // this source-private C++/WinRT peer during host teardown.
            peer.RaiseStructureChangedEvent(U::A::AutomationStructureChangeType::ChildrenInvalidated, nullptr);
        }
        return Alive(stamp);
    }

    void Rebuild()
    {
        const auto keepAlive = shared_from_this();
        if ( !owner || !list ) return;
        if ( retiringEditors ) { pendingProjection = true; return; }
        pendingProjection = false;
        if ( rebuilding ) return;
        rebuilding = true;
        auto guard = wxMakeGuard([this] { rebuilding = false; });
        wxUnusedVar(guard);
        for ( unsigned attempt = 0; attempt != 2 && owner; ++attempt )
        {
            const auto stamp = generation;
            ModelPin model(owner->GetModel());
            std::vector<Row> candidate;
            std::unordered_set<ItemKey> seen;
            if ( model.model && !Walk(model.model, wxDataViewItem(), 0, candidate, seen, stamp) ) continue;
            if ( !Alive(stamp) ) continue;
            if ( PublishRows(std::move(candidate), stamp) ) return;
            if ( retiringEditors ) return;
        }
        // No coherent candidate after the bounded retry: stop presenting old
        // identities to a changed model. A later external notification retries.
        rows.clear();
        ResetAutomationRows();
        selected.clear();
        ++suppressSelection;
        auto selectionGuard = wxMakeGuard([this] { --suppressSelection; });
        wxUnusedVar(selectionGuard);
        source.Clear();
    }

    void RestoreSelection()
    {
        if ( !owner || !list ) return;
        ++suppressSelection;
        auto guard = wxMakeGuard([this] { --suppressSelection; });
        wxUnusedVar(guard);
        const bool multiple = owner->HasFlag(wxDV_MULTIPLE);
        if ( multiple )
            for ( auto n = list.SelectedItems().Size(); n != 0; --n )
                if ( !selected.count(Key(list.SelectedItems().GetAt(n - 1))) )
                    list.SelectedItems().RemoveAt(n - 1);
        ItemKey single = 0;
        for ( auto it = selected.begin(); it != selected.end(); )
        {
            const int index = RowIndex(*it);
            if ( index == wxNOT_FOUND ) it = selected.erase(it);
            else
            {
                if ( multiple )
                {
                    bool present = false;
                    for ( const auto& value : list.SelectedItems() )
                        if ( Key(value) == *it ) { present = true; break; }
                    if ( !present ) list.SelectedItems().Append(source.GetAt(index));
                }
                else single = *it;
                ++it;
            }
        }
        if ( !multiple && Key(list.SelectedItem()) != single )
            list.SelectedItem(single ? source.GetAt(RowIndex(single)) : nullptr);
        RefreshVisuals();
    }

    void BuildHeader();
    bool CanMoveColumn(const wxDataViewColumn* column) const
    {
        if ( !owner || ColumnIndex(column) == wxNOT_FOUND || !owner->IsEnabled() ) return false;
        const auto stamp = generation;
        const bool reorderable = column->IsReorderable();
        if ( !Alive(stamp) ) return false;
        const bool hidden = column->IsHidden();
        return Alive(stamp) && reorderable && !hidden;
    }
    bool MoveColumn(wxDataViewColumn* column, unsigned position, bool requireReorderable);
    double ColumnLeft(const wxDataViewColumn* column) const;
    bool MoveColumnToLeft(wxDataViewColumn* column, double left);
    bool MoveColumnToScreen(wxDataViewColumn* column, double x, double y);
    bool ColumnDropLocation(wxDataViewColumn* column, double left,
                            wxWinUIDataViewHeader::DropLocation& location);
    bool PreviewColumnMove(wxDataViewColumn* column, double left);
    void ClearColumnMove(wxDataViewColumn* column);
    bool AutomationCurrent() const
        { return owner && list && !pendingProjection && !retiringEditors && !publishingRows; }
    void ResetAutomationRows() { automationRows.clear(); automationCells.clear(); }
    void PruneAutomationRows();
    void PruneAutomationColumns();
    std::vector<wxDataViewColumn*> AutomationVisibleColumns() const;
    std::shared_ptr<U::GridBinding> CreateAutomationBinding();
    U::A::AutomationPeer AutomationPeer();
    U::A::AutomationPeer AutomationCellPeer(ItemKey key, wxDataViewColumn* col);
    std::shared_ptr<U::RowBinding> AutomationRow(const F::IInspectable& value);
    std::vector<U::A::AutomationPeer> AutomationHeaders(bool all);
    F::Rect AutomationBounds(const X::FrameworkElement& element,
                             const X::FrameworkElement& viewport) const;
    X::FrameworkElement AutomationVisual(ItemKey key, wxDataViewColumn* col) const;
    wxVariant AutomationReadValue(ItemKey key, wxDataViewColumn* col);
    bool AutomationReadOnly(ItemKey key, wxDataViewColumn* col);
    U::CellBinding::Range AutomationRange(ItemKey key, wxDataViewColumn* col);
    void AutomationWriteValue(ItemKey key, wxDataViewColumn* col,
                              const winrt::hstring* text, const double* number,
                              const std::function<void()>& checkIdentity);
    bool ResizeColumn(wxDataViewColumn* column, double width)
    {
        const auto keepAlive = shared_from_this();
        if ( !owner || ColumnIndex(column) == wxNOT_FOUND ||
             !column->IsResizeable() || !std::isfinite(width) ) return false;
        const double pixels = std::max<double>(column->GetMinWidth(), width * Scale());
        const int bounded = static_cast<int>(std::min<double>(INT_MAX, std::max(0.0, pixels)));
        column->SetWidth(bounded);
        return owner && ColumnIndex(column) != wxNOT_FOUND;
    }
    bool SyncLayout()
    {
        const auto keepAlive = shared_from_this();
        if ( !owner || !list ) return false;
        if ( layingOut ) return true;
        layingOut = true;
        auto guard = wxMakeGuard([this] { layingOut = false; });
        wxUnusedVar(guard);
        const auto stamp = generation;
        // Geometry queries and explicit editing/scroll requests require the
        // currently committed wx rectangle, not the next composition tick.
        host.ForceRender();
        if ( !Alive(stamp) ) return false;
        list.ApplyTemplate();
        list.UpdateLayout();
        if ( !Alive(stamp) ) return false;
        ConnectScroll();
        return Alive(stamp);
    }
    void Realize(const C::ListViewItem& container, ItemKey key);
    void RefreshVisuals();
    void LayoutEditor();
    void BeginEdit(ItemKey key, wxDataViewColumn* col);
    bool EndEdit(bool cancel);
    bool ReadEditorValue(wxVariant* value);
    bool EditorPopupOpen() const;
    void CheckEditorFocus();
    void RetireEditor();
    void ChangeValue(ItemKey key, wxDataViewColumn* col, wxVariant value);
};

void wxWinUIDataViewImpl::PruneAutomationRows()
{
    std::unordered_set<ItemKey> live;
    for ( const auto& row : rows ) live.insert(row.key);
    for ( auto it = automationRows.begin(); it != automationRows.end(); )
        if ( !live.count(it->first) ) it = automationRows.erase(it); else ++it;
    for ( auto it = automationCells.begin(); it != automationCells.end(); )
        if ( !live.count(it->first) ) it = automationCells.erase(it); else ++it;
}

void wxWinUIDataViewImpl::PruneAutomationColumns()
{
    const auto stamp = generation;
    const auto snapshot = automationColumns;
    for ( const auto& entry : snapshot )
    {
        if ( !Alive(stamp) ) return;
        const bool removed = ColumnIndex(entry.first) == wxNOT_FOUND;
        const bool hidden = !removed && entry.first->IsHidden();
        if ( !Alive(stamp) ) return;
        if ( removed || hidden )
        {
            automationColumns.erase(entry.first);
            headerWidths.erase(entry.first);
            for ( auto& row : automationCells ) row.second.erase(entry.first);
        }
    }
}

std::vector<wxDataViewColumn*> wxWinUIDataViewImpl::AutomationVisibleColumns() const
{
    if ( !AutomationCurrent() ) U::Unavailable();
    const auto stamp = generation;
    const auto snapshot = displayColumns;
    std::vector<wxDataViewColumn*> result;
    for ( auto col : snapshot )
    {
        if ( !Alive(stamp) || ColumnIndex(col) == wxNOT_FOUND ) U::Unavailable();
        const bool hidden = col->IsHidden();
        if ( !Alive(stamp) ) U::Unavailable();
        if ( !hidden ) result.push_back(col);
    }
    if ( result.size() > INT_MAX ) U::Unavailable();
    return result;
}

std::shared_ptr<U::GridBinding> wxWinUIDataViewImpl::CreateAutomationBinding()
{
    auto binding = std::make_shared<U::GridBinding>();
    const std::weak_ptr<wxWinUIDataViewImpl> weak = shared_from_this();
    const auto resolve = [weak]
    {
        auto self = weak.lock();
        if ( !self || !self->AutomationCurrent() ) U::Unavailable();
        return self;
    };
    binding->current = [weak]
    {
        const auto self = weak.lock();
        return self && self->AutomationCurrent();
    };
    binding->rowCount = [resolve]
    {
        auto self = resolve();
        if ( self->rows.size() > INT_MAX ) U::Unavailable();
        return static_cast<int>(self->rows.size());
    };
    binding->version = [resolve] { return resolve()->generation; };
    binding->columnCount = [resolve]
        { return static_cast<int>(resolve()->AutomationVisibleColumns().size()); };
    binding->item = [resolve](int row)
    {
        auto self = resolve();
        if ( row < 0 || static_cast<size_t>(row) >= self->rows.size() ) U::Unavailable();
        return winrt::box_value(self->rows[row].key);
    };
    binding->cell = [resolve](int row, int column)
    {
        auto self = resolve();
        const auto stamp = self->generation;
        const auto visible = self->AutomationVisibleColumns();
        if ( !self->Alive(stamp) || row < 0 || static_cast<size_t>(row) >= self->rows.size() ||
             column < 0 || static_cast<size_t>(column) >= visible.size() ) U::Unavailable();
        return self->AutomationCellPeer(self->rows[row].key, visible[column]);
    };
    binding->row = [resolve](const F::IInspectable& item) { return resolve()->AutomationRow(item); };
    binding->headers = [resolve](bool all) { return resolve()->AutomationHeaders(all); };
    return binding;
}

U::A::AutomationPeer wxWinUIDataViewImpl::AutomationPeer()
{
    if ( !AutomationCurrent() ) U::Unavailable();
    return U::A::FrameworkElementAutomationPeer::CreatePeerForElement(list);
}

X::FrameworkElement wxWinUIDataViewImpl::AutomationVisual(ItemKey key, wxDataViewColumn* col) const
{
    const auto row = automationCells.find(key);
    if ( row == automationCells.end() ) return nullptr;
    const auto cell = row->second.find(col);
    if ( cell == row->second.end() ) return nullptr;
    const auto visual = cell->second.visual.get();
    return visual && visual.IsLoaded() && X::Media::VisualTreeHelper::GetParent(visual)
        ? visual : nullptr;
}

F::Rect wxWinUIDataViewImpl::AutomationBounds(const X::FrameworkElement& element,
                                             const X::FrameworkElement& viewport) const
{
    if ( !AutomationCurrent() || !owner->IsShownOnScreen() || !element || !viewport ||
         !element.IsLoaded() || !viewport.IsLoaded() || !element.XamlRoot() ||
         !X::Media::VisualTreeHelper::GetParent(element) ) return {};
    auto* const nativeHost = wxWinUITopLevelHost::FindSlotOwner(owner.get());
    if ( !nativeHost || !nativeHost->Root() ) return {};
    // The host canvas is physical LTR. A wx child HWND's (0,0) is its right
    // edge in RTL, and adding XAML-local x to it mirrors/offsets UIA twice.
    const auto nativeRoot = nativeHost->Root();
    const auto rect = element.TransformToVisual(nativeRoot).TransformBounds(
        {0, 0, static_cast<float>(element.ActualWidth()), static_cast<float>(element.ActualHeight())});
    const auto clip = viewport.TransformToVisual(nativeRoot).TransformBounds(
        {0, 0, static_cast<float>(viewport.ActualWidth()), static_cast<float>(viewport.ActualHeight())});
    const float left = std::max(rect.X, clip.X), top = std::max(rect.Y, clip.Y);
    const float right = std::min(rect.X + rect.Width, clip.X + clip.Width);
    const float bottom = std::min(rect.Y + rect.Height, clip.Y + clip.Height);
    if ( right <= left || bottom <= top ) return {};
    const auto origin = nativeHost->GetClientScreenRect();
    const auto scale = static_cast<float>(nativeHost->GetScale());
    return {origin.left + left * scale, origin.top + top * scale,
            (right - left) * scale, (bottom - top) * scale};
}

U::A::AutomationPeer wxWinUIDataViewImpl::AutomationCellPeer(ItemKey key, wxDataViewColumn* col)
{
    const auto visible = AutomationVisibleColumns();
    if ( RowIndex(key) == wxNOT_FOUND ||
         std::find(visible.begin(), visible.end(), col) == visible.end() ) U::Unavailable();
    auto& rowToken = automationRows[key];
    if ( !rowToken ) rowToken = std::make_shared<int>(0);
    *rowToken = 1; // A logical provider, not just a recyclable visual, uses it.
    auto& columnToken = automationColumns[col];
    if ( !columnToken ) columnToken = std::make_shared<int>(0);
    auto& entry = automationCells[key][col];
    if ( entry.peer ) return entry.peer;
    const std::weak_ptr<wxWinUIDataViewImpl> weak = shared_from_this();
    const std::weak_ptr<int> weakRow = rowToken, weakColumn = columnToken;
    const auto resolve = [weak, weakRow, weakColumn, key, col]
    {
        auto self = weak.lock();
        if ( !self || !self->AutomationCurrent() || weakRow.expired() || weakColumn.expired() ||
             self->RowIndex(key) == wxNOT_FOUND || self->ColumnIndex(col) == wxNOT_FOUND ) U::Unavailable();
        return self;
    };
    auto binding = std::make_shared<U::CellBinding>();
    binding->current = [weak, weakRow, weakColumn, key, col]
    {
        const auto self = weak.lock();
        return self && self->AutomationCurrent() && !weakRow.expired() && !weakColumn.expired() &&
               self->RowIndex(key) != wxNOT_FOUND && self->ColumnIndex(col) != wxNOT_FOUND;
    };
    binding->coordinates = [resolve, key, col]
    {
        auto self = resolve();
        const auto visibleColumns = self->AutomationVisibleColumns();
        const auto found = std::find(visibleColumns.begin(), visibleColumns.end(), col);
        if ( found == visibleColumns.end() ) U::Unavailable();
        resolve();
        return std::make_pair(self->RowIndex(key), static_cast<int>(found - visibleColumns.begin()));
    };
    binding->name = [resolve, key, col]
    {
        auto self = resolve();
        const auto stamp = self->generation;
        ModelPin model(self->owner->GetModel());
        if ( !model.model ) U::Unavailable();
        const unsigned modelColumn = col->GetModelColumn();
        const bool hasValue = model.model->HasValue(Item(key), modelColumn);
        if ( !self->Alive(stamp) ) U::Unavailable();
        resolve();
        if ( !hasValue ) return winrt::hstring();
        wxVariant value;
        model.model->GetValue(value, Item(key), modelColumn);
        if ( !self->Alive(stamp) ) U::Unavailable();
        resolve();
        wxString text;
        if ( value.IsType("wxDataViewIconText") )
        { wxDataViewIconText icon; icon << value; text = icon.GetText(); }
        else if ( value.IsType("wxDataViewCheckIconText") )
        { wxDataViewCheckIconText icon; icon << value; text = icon.GetText(); }
        else if ( EditorKind(col->GetRenderer()) == NativeEditorKind::ChoiceIndex && value.IsType("long") )
        {
            const auto& choices = static_cast<wxDataViewChoiceRenderer*>(col->GetRenderer())->GetChoices();
            const auto index = value.GetLong();
            if ( index >= 0 && static_cast<size_t>(index) < choices.size() ) text = choices[index];
        }
        else text = value.MakeString();
        if ( !self->Alive(stamp) ) U::Unavailable();
        return wxWinUIToHString(text);
    };
    binding->element = [resolve, key, col] { return resolve()->AutomationVisual(key, col); };
    binding->bounds = [resolve, key, col]
    {
        auto self = resolve();
        return self->AutomationBounds(self->AutomationVisual(key, col), self->list);
    };
    binding->grid = [resolve] { return resolve()->AutomationPeer(); };
    binding->header = [resolve, col]() -> U::A::AutomationPeer
    {
        auto self = resolve();
        if ( self->owner->HasFlag(wxDV_NO_HEADER) ) return nullptr;
        self->AutomationHeaders(true);
        resolve();
        const auto found = self->headers.find(col);
        if ( found == self->headers.end() ) U::Unavailable();
        return U::A::FrameworkElementAutomationPeer::CreatePeerForElement(found->second->grid);
    };
    binding->enabled = [resolve, key, col]
    {
        auto self = resolve();
        const auto stamp = self->generation;
        ModelPin model(self->owner->GetModel());
        if ( !model.model ) U::Unavailable();
        const bool enabled = model.model->IsEnabled(Item(key), col->GetModelColumn());
        if ( !self->Alive(stamp) ) U::Unavailable();
        resolve();
        return enabled && self->owner->IsEnabled();
    };
    binding->realize = [resolve, key, col]
    {
        auto self = resolve();
        self->owner->EnsureVisible(Item(key), col);
        resolve();
        if ( !self->SyncLayout() ) U::Unavailable();
        self->RefreshVisuals();
        resolve();
        self->list.UpdateLayout();
        resolve();
        if ( !self->AutomationVisual(key, col) ) U::Unavailable();
    };
    binding->supportsValue = [resolve, col]
        { resolve(); return EditorKind(col->GetRenderer()) != NativeEditorKind::Custom; };
    binding->supportsRange = [resolve, col]
        { resolve(); return EditorKind(col->GetRenderer()) == NativeEditorKind::Spin; };
    binding->readOnly = [resolve, key, col]
    {
        const bool result = resolve()->AutomationReadOnly(key, col);
        resolve();
        return result;
    };
    binding->value = [resolve, key, col, name = binding->name]
    {
        auto self = resolve();
        if ( EditorKind(col->GetRenderer()) == NativeEditorKind::ChoiceIndex ) return name();
        const auto value = self->AutomationReadValue(key, col);
        resolve();
#if wxUSE_DATEPICKCTRL
        // A civil-date value has a stable, round-trippable UIA representation;
        // the native visual remains localized independently.
        if ( value.IsType("datetime") )
            return value.GetDateTime().IsValid()
                ? wxWinUIToHString(value.GetDateTime().FormatISODate()) : winrt::hstring();
#endif
        return wxWinUIToHString(value.MakeString());
    };
    binding->range = [resolve, key, col]
    {
        const auto result = resolve()->AutomationRange(key, col);
        resolve();
        return result;
    };
    const auto checkWriteIdentity = [weak, weakRow, weakColumn, key, col]
    {
        const auto self = weak.lock();
        // Logical commit may precede popup physical retirement. Do not turn
        // a successful commit into failure merely because that gate is busy.
        if ( !self || !self->owner || weakRow.expired() || weakColumn.expired() ||
             self->RowIndex(key) == wxNOT_FOUND || self->ColumnIndex(col) == wxNOT_FOUND ) U::Unavailable();
    };
    binding->setValue = [resolve, checkWriteIdentity, key, col](const winrt::hstring& text)
        { resolve()->AutomationWriteValue(key, col, &text, nullptr, checkWriteIdentity); };
    binding->setRange = [resolve, checkWriteIdentity, key, col](double value)
        { resolve()->AutomationWriteValue(key, col, nullptr, &value, checkWriteIdentity); };
    entry.peer = winrt::make<U::CellPeer>(binding);
    return entry.peer;
}

wxVariant wxWinUIDataViewImpl::AutomationReadValue(ItemKey key, wxDataViewColumn* col)
{
    const auto keepAlive = shared_from_this();
    if ( !owner || ColumnIndex(col) == wxNOT_FOUND || RowIndex(key) == wxNOT_FOUND ) U::Unavailable();
    const auto stamp = generation;
    ModelPin model(owner->GetModel());
    if ( !model.model ) U::Unavailable();
    const auto modelColumn = col->GetModelColumn();
    const bool hasValue = model.model->HasValue(Item(key), modelColumn);
    if ( !Alive(stamp) ) U::Unavailable();
    wxVariant value;
    if ( hasValue ) model.model->GetValue(value, Item(key), modelColumn);
    if ( !Alive(stamp) ) U::Unavailable();
    return value;
}

bool wxWinUIDataViewImpl::AutomationReadOnly(ItemKey key, wxDataViewColumn* col)
{
    const auto keepAlive = shared_from_this();
    if ( !owner || ColumnIndex(col) == wxNOT_FOUND || RowIndex(key) == wxNOT_FOUND ) U::Unavailable();
    if ( col->GetRenderer()->GetMode() != wxDATAVIEW_CELL_EDITABLE ||
         EditorKind(col->GetRenderer()) == NativeEditorKind::Custom ) return true;
    const auto stamp = generation;
    ModelPin model(owner->GetModel());
    if ( !model.model ) U::Unavailable();
    const bool hasValue = model.model->HasValue(Item(key), col->GetModelColumn());
    if ( !Alive(stamp) ) U::Unavailable();
    return !hasValue;
}

U::CellBinding::Range wxWinUIDataViewImpl::AutomationRange(ItemKey key, wxDataViewColumn* col)
{
#if wxUSE_SPINCTRL
    const auto value = AutomationReadValue(key, col);
    if ( !value.IsType("long") || EditorKind(col->GetRenderer()) != NativeEditorKind::Spin )
        throw winrt::hresult_error(UIA_E_NOTSUPPORTED);
    const auto renderer = static_cast<wxDataViewSpinRenderer*>(col->GetRenderer());
    return {static_cast<double>(value.GetLong()), static_cast<double>(renderer->m_min),
            static_cast<double>(renderer->m_max)};
#else
    wxUnusedVar(key);
    wxUnusedVar(col);
    throw winrt::hresult_error(UIA_E_NOTSUPPORTED);
#endif
}

void wxWinUIDataViewImpl::AutomationWriteValue(ItemKey key, wxDataViewColumn* col,
    const winrt::hstring* text, const double* number, const std::function<void()>& checkIdentity)
{
    const auto keepAlive = shared_from_this();
    checkIdentity();
    if ( automationWriteInProgress || endingEdit ||
         (editGeneration && (editingItem != key || editingColumn != col)) )
        throw winrt::hresult_error(UIA_E_INVALIDOPERATION);
    automationWriteInProgress = true;
    auto writeGuard = wxMakeGuard([this] { automationWriteInProgress = false; });
    wxUnusedVar(writeGuard);
    const auto stamp = generation;
    const bool readOnly = AutomationReadOnly(key, col);
    checkIdentity();
    ModelPin model(owner->GetModel());
    if ( !model.model ) U::Unavailable();
    const bool enabled = model.model->IsEnabled(Item(key), col->GetModelColumn());
    if ( !Alive(stamp) ) U::Unavailable();
    checkIdentity();
    if ( !enabled || !owner->IsEnabled() ) throw winrt::hresult_error(UIA_E_ELEMENTNOTENABLED);
    if ( readOnly ) throw winrt::hresult_error(UIA_E_INVALIDOPERATION);

    const auto kind = EditorKind(col->GetRenderer());
    const auto renderer = col->GetRenderer();
    const wxString input = text ? wxWinUIFromHString(*text) : wxString();
    long choiceIndex = wxNOT_FOUND, integer = 0;
#if wxUSE_DATEPICKCTRL
    wxDateTime civil;
    F::DateTime nativeDate;
#endif
    // Validate the transport representation before touching an existing edit.
    // The renderer's semantic Validate() still runs only in EndEdit below.
    if ( kind == NativeEditorKind::Choice || kind == NativeEditorKind::ChoiceIndex )
    {
        if ( !text ) throw winrt::hresult_invalid_argument();
        const auto choices = static_cast<wxDataViewChoiceRenderer*>(renderer)->GetChoices();
        choiceIndex = choices.Index(input);
        if ( choiceIndex == wxNOT_FOUND ) throw winrt::hresult_invalid_argument();
        if ( kind == NativeEditorKind::ChoiceIndex )
        {
            const auto value = AutomationReadValue(key, col);
            checkIdentity();
            if ( !value.IsType("long") ) throw winrt::hresult_invalid_argument();
            const long previous = value.GetLong();
            if ( previous >= 0 && static_cast<size_t>(previous) < choices.size() && choices[previous] == input )
                choiceIndex = previous;
        }
    }
    else if ( kind == NativeEditorKind::Spin )
    {
        const auto range = AutomationRange(key, col);
        checkIdentity();
        if ( number )
        {
            if ( !std::isfinite(*number) || std::trunc(*number) != *number ||
                 *number < LONG_MIN || *number > LONG_MAX ) throw winrt::hresult_invalid_argument();
            integer = static_cast<long>(*number);
        }
        else if ( !text || !input.ToLong(&integer) ) throw winrt::hresult_invalid_argument();
        if ( integer < range.minimum || integer > range.maximum ) throw winrt::hresult_invalid_argument();
    }
#if wxUSE_DATEPICKCTRL
    else if ( kind == NativeEditorKind::Date )
    {
        if ( !text || (!input.empty() &&
             (!civil.ParseISODate(input) || !ToNativeCivilDate(civil, &nativeDate))) )
            throw winrt::hresult_invalid_argument();
    }
#endif
    else if ( kind != NativeEditorKind::Text || !text ) throw winrt::hresult_error(UIA_E_NOTSUPPORTED);
    if ( !Alive(stamp) ) U::Unavailable();
    checkIdentity();
    if ( !editingItem ) BeginEdit(key, col);
    checkIdentity();
    if ( editingItem != key || editingColumn != col || !editor ||
         nativeEditRenderer != renderer || editorKind != kind )
        throw winrt::hresult_error(UIA_E_INVALIDOPERATION);
    const auto native = editor;
    const auto transaction = editGeneration;
    bool finished = false;
    auto cancelGuard = wxMakeGuard([this, transaction, &finished]
    {
        // Cancel only our exact transaction, never a reentrant successor.
        if ( !finished && owner && editGeneration == transaction && editingItem ) EndEdit(true);
    });
    wxUnusedVar(cancelGuard);
    const auto checkEdit = [&]
    {
        checkIdentity();
        if ( editGeneration != transaction || editor != native || editingItem != key || editingColumn != col )
            throw winrt::hresult_error(UIA_E_INVALIDOPERATION);
    };
    const auto checkWritable = [&]
    {
        checkEdit();
        const auto currentStamp = generation;
        const bool nowReadOnly = AutomationReadOnly(key, col);
        checkEdit();
        const bool nowEnabled = model.model->IsEnabled(Item(key), col->GetModelColumn());
        if ( !Alive(currentStamp) ) U::Unavailable();
        checkEdit();
        if ( !nowEnabled || !owner->IsEnabled() ) throw winrt::hresult_error(UIA_E_ELEMENTNOTENABLED);
        if ( nowReadOnly ) throw winrt::hresult_error(UIA_E_INVALIDOPERATION);
    };
    checkWritable();
    if ( kind == NativeEditorKind::Text ) native.as<C::TextBox>().Text(*text);
    else if ( kind == NativeEditorKind::Choice || kind == NativeEditorKind::ChoiceIndex )
        native.as<C::ComboBox>().SelectedIndex(static_cast<int>(choiceIndex));
    else if ( kind == NativeEditorKind::Spin ) native.as<C::NumberBox>().Value(integer);
#if wxUSE_DATEPICKCTRL
    else if ( kind == NativeEditorKind::Date )
    {
        const auto date = native.as<C::CalendarDatePicker>();
        if ( input.empty() ) date.Date(nullptr);
        else
        {
            if ( nativeDate < date.MinDate() ) { date.MinDate(nativeDate); checkEdit(); }
            if ( nativeDate > date.MaxDate() ) { date.MaxDate(nativeDate); checkEdit(); }
            date.Date(nativeDate);
        }
    }
#endif
    checkWritable();
    const bool accepted = EndEdit(false);
    finished = true;
    checkIdentity();
    if ( !accepted ) throw winrt::hresult_error(UIA_E_INVALIDOPERATION);
}

std::shared_ptr<U::RowBinding> wxWinUIDataViewImpl::AutomationRow(const F::IInspectable& value)
{
    const auto key = Key(value);
    if ( !AutomationCurrent() || RowIndex(key) == wxNOT_FOUND ) U::Unavailable();
    auto& token = automationRows[key];
    if ( !token ) token = std::make_shared<int>(0);
    *token = 1;
    const std::weak_ptr<int> weakToken = token;
    const std::weak_ptr<wxWinUIDataViewImpl> weak = shared_from_this();
    const auto resolve = [weak, weakToken, key]
    {
        auto self = weak.lock();
        if ( !self || !self->AutomationCurrent() || weakToken.expired() ||
             self->RowIndex(key) == wxNOT_FOUND ) U::Unavailable();
        return self;
    };
    auto binding = std::make_shared<U::RowBinding>();
    binding->current = [weak, weakToken, key]
    {
        const auto self = weak.lock();
        return self && self->AutomationCurrent() && !weakToken.expired() && self->RowIndex(key) != wxNOT_FOUND;
    };
    binding->name = [resolve, key]
    {
        auto self = resolve();
        const auto visible = self->AutomationVisibleColumns();
        return visible.empty() ? winrt::hstring() : self->AutomationCellPeer(key, visible.front()).GetName();
    };
    binding->cells = [resolve, key]
    {
        auto self = resolve();
        const auto stamp = self->generation;
        const auto visible = self->AutomationVisibleColumns();
        std::vector<U::A::AutomationPeer> result;
        for ( auto col : visible )
        {
            if ( !self->Alive(stamp) ) U::Unavailable();
            result.push_back(self->AutomationCellPeer(key, col));
        }
        resolve();
        return result;
    };
    binding->realize = [resolve, key]
    {
        auto self = resolve();
        self->owner->EnsureVisible(Item(key));
        resolve();
        if ( !self->SyncLayout() ) U::Unavailable();
        resolve();
    };
    return binding;
}

std::vector<U::A::AutomationPeer> wxWinUIDataViewImpl::AutomationHeaders(bool all)
{
    const auto keepAlive = shared_from_this();
    const auto stamp = generation;
    const auto visible = AutomationVisibleColumns();
    std::vector<U::A::AutomationPeer> result;
    if ( owner->HasFlag(wxDV_NO_HEADER) ) return result;
    if ( all )
    {
        automationAllHeaders = true;
        const bool previous = automationHeaderQuery;
        automationHeaderQuery = true;
        auto guard = wxMakeGuard([this, previous] { automationHeaderQuery = previous; });
        wxUnusedVar(guard);
        BuildHeader();
        if ( !Alive(stamp) ) U::Unavailable();
    }
    for ( auto col : visible )
    {
        if ( !Alive(stamp) ) U::Unavailable();
        const auto found = headers.find(col);
        if ( found == headers.end() )
        { if ( all ) U::Unavailable(); else continue; }
        result.push_back(U::A::FrameworkElementAutomationPeer::CreatePeerForElement(found->second->grid));
        if ( !Alive(stamp) ) U::Unavailable();
    }
    return result;
}

namespace
{
class DataViewNotifier : public wxDataViewModelNotifier
{
public:
    explicit DataViewNotifier(const std::shared_ptr<wxWinUIDataViewImpl>& state)
        : m_state(state) {}
    bool Changed()
    {
        if ( auto state = m_state.lock() ) state->ModelChanged();
        return true;
    }
    bool StructureChanged()
    {
        // Virtual-list row IDs can be reused after insertion/removal. Never
        // allow a retained provider to silently change its model identity.
        if ( auto state = m_state.lock() ) state->ResetAutomationRows();
        return Changed();
    }
    bool ItemAdded(const wxDataViewItem&, const wxDataViewItem&) override { return StructureChanged(); }
    bool ItemDeleted(const wxDataViewItem&, const wxDataViewItem&) override { return StructureChanged(); }
    bool ItemChanged(const wxDataViewItem& item) override
    {
        return NotifyValue(item, wxNOT_FOUND);
    }
    bool ValueChanged(const wxDataViewItem& item, unsigned int col) override
    {
        return NotifyValue(item, static_cast<int>(col));
    }
    bool ItemsAdded(const wxDataViewItem&, const wxDataViewItemArray&) override { return StructureChanged(); }
    bool ItemsDeleted(const wxDataViewItem&, const wxDataViewItemArray&) override { return StructureChanged(); }
    bool ItemsChanged(const wxDataViewItemArray&) override { return Changed(); }
    bool Cleared() override
    {
        if ( auto state = m_state.lock() )
        {
            state->expanded.clear();
            state->selected.clear();
            state->current = 0;
        }
        return StructureChanged();
    }
    void Resort() override { Changed(); }
private:
    bool NotifyValue(const wxDataViewItem& item, int modelColumn)
    {
        auto state = m_state.lock();
        if ( !state || !state->owner ) return false;
        ModelPin model(GetOwner());
        // ItemChanged can alter container/child semantics as well as values;
        // only ValueChanged promises a single model-column invalidation.
        state->ModelChanged(modelColumn == wxNOT_FOUND ? 0 : Key(item), modelColumn);
        if ( !state->owner || state->owner->GetModel() != model.model ) return false;
        wxDataViewColumn* column = nullptr;
        if ( modelColumn != wxNOT_FOUND )
        {
            const int index = state->owner->GetModelColumnIndex(modelColumn);
            if ( index == wxNOT_FOUND ) return false;
            column = state->owner->GetColumn(index);
        }
        state->Send(wxEVT_DATAVIEW_ITEM_VALUE_CHANGED, Key(item), column);
        return true;
    }
    std::weak_ptr<wxWinUIDataViewImpl> m_state;
};
}

double wxWinUIDataViewImpl::ColumnLeft(const wxDataViewColumn* column) const
{
    const auto stamp = generation;
    const auto snapshot = displayColumns;
    double left = 0;
    for ( auto candidate : snapshot )
    {
        if ( !Alive(stamp) ) return -1;
        if ( candidate == column ) return left;
        if ( !candidate->IsHidden() ) left += ColumnWidth(candidate);
        if ( !Alive(stamp) ) return -1;
    }
    return -1;
}

bool wxWinUIDataViewImpl::MoveColumn(wxDataViewColumn* column, unsigned position,
                                    bool requireReorderable)
{
    const auto keepAlive = shared_from_this();
    if ( !owner || ColumnIndex(column) == wxNOT_FOUND || position >= displayColumns.size() ||
         position > INT_MAX || (requireReorderable && !CanMoveColumn(column)) ) return false;
    if ( !owner->PrepareForColumnMutation() || !owner || retiringEditors || pendingProjection ||
         ColumnIndex(column) == wxNOT_FOUND || position >= displayColumns.size() ||
         (requireReorderable && !CanMoveColumn(column)) ) return false;
    const int previous = ColumnPosition(column);
    if ( previous == static_cast<int>(position) ) return true;
    {
        columnMutation = true;
        auto guard = wxMakeGuard([this] { columnMutation = false; });
        wxUnusedVar(guard);
        displayColumns.erase(displayColumns.begin() + previous);
        displayColumns.insert(displayColumns.begin() + position, column);
        const auto before = generation++;
        // Reordering does not mutate values or column identities. Keep live
        // cells and their handlers; geometry changes are reconciled in place.
        for ( auto& row : automationCells )
            for ( auto& cell : row.second )
                if ( cell.second.presentation && *cell.second.presentation == before )
                    *cell.second.presentation = generation;
        RefreshVisuals();
    }
    if ( !owner || ColumnIndex(column) == wxNOT_FOUND ) return false;
    const auto peer = U::A::FrameworkElementAutomationPeer::FromElement(list);
    if ( peer )
        peer.RaiseStructureChangedEvent(U::A::AutomationStructureChangeType::ChildrenInvalidated, nullptr);
    if ( !owner || ColumnIndex(column) == wxNOT_FOUND ) return false;
    wxDataViewEvent event(wxEVT_DATAVIEW_COLUMN_REORDERED, owner.get(), column);
    event.SetColumn(ColumnPosition(column));
    owner->GetEventHandler()->ProcessEvent(event);
    return owner && ColumnIndex(column) != wxNOT_FOUND;
}

bool wxWinUIDataViewImpl::MoveColumnToLeft(wxDataViewColumn* column, double left)
{
    const auto keepAlive = shared_from_this();
    if ( !owner || !std::isfinite(left) || ColumnIndex(column) == wxNOT_FOUND ||
         !owner->PrepareForColumnMutation() || !owner || ColumnIndex(column) == wxNOT_FOUND ) return false;
    wxWinUIDataViewHeader::DropLocation location;
    if ( !ColumnDropLocation(column, left, location) ) return false;
    return MoveColumn(column, location.position, true);
}

bool wxWinUIDataViewImpl::ColumnDropLocation(wxDataViewColumn* column, double left,
                                            wxWinUIDataViewHeader::DropLocation& location)
{
    if ( !owner || ColumnIndex(column) == wxNOT_FOUND ) return false;
    const auto stamp = generation;
    const auto snapshot = displayColumns;
    std::vector<double> widths;
    unsigned sourceIndex = 0;
    // Hidden columns retain their place in the permutation. Moving across
    // them changes display position, never the model/logical column indexes.
    for ( auto candidate : snapshot )
    {
        if ( !Alive(stamp) ) return false;
        const double candidateWidth = candidate->IsHidden() ? 0 : ColumnWidth(candidate);
        if ( !Alive(stamp) ) return false;
        if ( candidate == column ) sourceIndex = static_cast<unsigned>(widths.size());
        widths.push_back(candidateWidth);
    }
    return wxWinUIDataViewHeader::FindDropLocation(widths, sourceIndex, left, location);
}

bool wxWinUIDataViewImpl::PreviewColumnMove(wxDataViewColumn* column, double left)
{
    const auto keepAlive = shared_from_this();
    if ( !header || !CanMoveColumn(column) ) return false;
    const auto stamp = generation;
    wxWinUIDataViewHeader::DropLocation location;
    if ( !ColumnDropLocation(column, left, location) || !Alive(stamp) ) return false;
    if ( !columnInsertionMarker )
    {
        columnInsertionMarker = std::make_unique<wxWinUIDataViewHeader::InsertionMarker>();
        if ( !Alive(stamp) ) return false;
        header.Children().Append(columnInsertionMarker->grid);
        if ( !Alive(stamp) ) return false;
    }
    columnInsertionOwner = column;
    columnInsertionMarker->Update(location.edge, location.changesOrder);
    return Alive(stamp);
}

void wxWinUIDataViewImpl::ClearColumnMove(wxDataViewColumn* column)
{
    if ( columnInsertionOwner != column ) return;
    columnInsertionOwner = nullptr;
    if ( columnInsertionMarker ) columnInsertionMarker->Clear();
}

bool wxWinUIDataViewImpl::MoveColumnToScreen(wxDataViewColumn* column, double x, double y)
{
    const auto keepAlive = shared_from_this();
    if ( !owner || !root || !header || !std::isfinite(x) || !std::isfinite(y) ||
         ColumnIndex(column) == wxNOT_FOUND ) return false;
    auto* const nativeHost = wxWinUITopLevelHost::FindSlotOwner(owner.get());
    if ( !nativeHost || !nativeHost->Root() ) return false;
    const auto stamp = generation;
    const auto screenOrigin = nativeHost->GetClientScreenRect();
    const auto nativeRoot = nativeHost->Root();
    const auto headerOrigin = header.TransformToVisual(nativeRoot).TransformPoint({0, 0});
    const double scale = nativeHost->GetScale();
    if ( std::abs(y - screenOrigin.top - headerOrigin.Y * scale) > 0.5 ) return false;
    const auto point = nativeRoot.TransformToVisual(header).TransformPoint(
        {static_cast<float>((x - screenOrigin.left) / scale), static_cast<float>((y - screenOrigin.top) / scale)});
    const double width = ColumnWidth(column);
    if ( !Alive(stamp) ) return false;
    const bool rtl = owner->GetLayoutDirection() == wxLayout_RightToLeft;
    return MoveColumnToLeft(column, point.X - (rtl ? width : 0));
}

void wxWinUIDataViewImpl::BuildHeader()
{
    const auto keepAlive = shared_from_this();
    if ( !owner || !header ) return;
    const auto stamp = generation;
    const double totalWidth = TotalWidth();
    if ( !Alive(stamp) ) return;
    header.Width(totalWidth);
    if ( !Alive(stamp) ) return;
    double left = 0;
    const std::weak_ptr<wxWinUIDataViewImpl> weak = shared_from_this();
    const auto snapshot = displayColumns;
    std::unordered_set<wxDataViewColumn*> retained;
    for ( auto col : snapshot )
    {
        if ( !Alive(stamp) ) return;
        if ( col->IsHidden() ) continue;
        const double width = ColumnWidth(col);
        if ( !Alive(stamp) ) return;
        if ( !automationHeaderQuery ) headerWidths[col] = width;
        const auto existing = headers.find(col);
        const auto retainedPeer = existing == headers.end() ? nullptr : existing->second;
        if ( automationAllHeaders || VisibleColumn(left, width) ||
             (retainedPeer && (retainedPeer->binding->dragging || retainedPeer->binding->moving ||
                               retainedPeer->binding->automationRequested)) )
        {
            retained.insert(col);
            auto peer = retainedPeer ? retainedPeer : std::make_shared<wxWinUIDataViewHeader::Header>();
            if ( !Alive(stamp) ) return;
            if ( !retainedPeer )
            {
                headers[col] = peer;
                const std::weak_ptr<wxWinUIDataViewHeader::Header> weakPeer = peer;
                const auto headerCurrent = [weak, weakPeer, col]
                {
                    auto self = weak.lock();
                    auto native = weakPeer.lock();
                    if ( !self || !self->owner || !native ||
                         self->ColumnIndex(col) == wxNOT_FOUND ) return false;
                    const auto entry = self->headers.find(col);
                    return entry != self->headers.end() && entry->second == native;
                };
                peer->binding->current = headerCurrent;
                peer->binding->bounds = [weak, weakPeer, headerCurrent]
                {
                    auto self = weak.lock();
                    auto native = weakPeer.lock();
                    if ( !self || !native || !headerCurrent() ) U::Unavailable();
                    return self->AutomationBounds(native->grid, self->headerScroll);
                };
                peer->binding->resizable = [weak, headerCurrent, col]
                {
                    auto self = weak.lock();
                    return headerCurrent() && self && self->owner->IsEnabled() &&
                           col->IsResizeable() && !col->IsHidden();
                };
                peer->binding->beginResize = [weak, headerCurrent]
                {
                    auto self = weak.lock();
                    // Resizing does not change the edited item/column. Keep
                    // its native editor and uncommitted value in place.
                    return headerCurrent() && self && !self->columnMutation &&
                           !self->retiringEditors && !self->pendingProjection;
                };
                peer->binding->movable = [weak, headerCurrent, col]
                {
                    auto self = weak.lock();
                    return headerCurrent() && self && self->CanMoveColumn(col) && headerCurrent();
                };
                peer->binding->beginMove = [weak, headerCurrent]
                {
                    auto self = weak.lock();
                    return headerCurrent() && self && self->owner->PrepareForColumnMutation() &&
                           headerCurrent() && !self->retiringEditors && !self->pendingProjection;
                };
                peer->binding->left = [weak, headerCurrent, col]
                {
                    auto self = weak.lock();
                    return headerCurrent() && self ? self->ColumnLeft(col) : -1.0;
                };
                peer->binding->version = [weak]
                {
                    const auto self = weak.lock();
                    return self ? self->generation : std::uint64_t(0);
                };
                peer->binding->moveToLeft = [weak, headerCurrent, col](double position)
                {
                    auto self = weak.lock();
                    return headerCurrent() && self && self->MoveColumnToLeft(col, position) && headerCurrent();
                };
                peer->binding->previewMove = [weak, headerCurrent, col](double position)
                {
                    const auto self = weak.lock();
                    return headerCurrent() && self && self->PreviewColumnMove(col, position) && headerCurrent();
                };
                peer->binding->clearMove = [weak, col]
                {
                    if ( const auto self = weak.lock() ) self->ClearColumnMove(col);
                };
                peer->binding->moveToScreen = [weak, headerCurrent, col](double x, double y)
                {
                    auto self = weak.lock();
                    return headerCurrent() && self && self->MoveColumnToScreen(col, x, y) && headerCurrent();
                };
                peer->binding->width = [weak, headerCurrent, col]
                {
                    auto self = weak.lock();
                    return headerCurrent() && self ? self->ColumnWidth(col) : 0.0;
                };
                peer->binding->scale = [weak]
                {
                    auto self = weak.lock();
                    return self ? self->Scale() : 1.0;
                };
                peer->binding->rightToLeft = [weak, headerCurrent]
                {
                    auto self = weak.lock();
                    return headerCurrent() && self && self->owner->GetLayoutDirection() == wxLayout_RightToLeft;
                };
                peer->binding->resize = [weak, headerCurrent, col](double dipWidth)
                {
                    auto self = weak.lock();
                    return headerCurrent() && self && self->ResizeColumn(col, dipWidth) && headerCurrent();
                };
                peer->binding->autoSize = [weak, headerCurrent, col]
                {
                    auto self = weak.lock();
                    if ( !headerCurrent() || !self || self->columnMutation ||
                         self->retiringEditors || self->pendingProjection ) return;
                    col->SetWidth(wxCOL_WIDTH_AUTOSIZE);
                };
                peer->binding->click = [weak, headerCurrent, col](bool rightClick)
                {
                    auto self = weak.lock();
                    if ( !headerCurrent() || !self ) return;
                    if ( !self->Send(rightClick ? wxEVT_DATAVIEW_COLUMN_HEADER_RIGHT_CLICK :
                                                  wxEVT_DATAVIEW_COLUMN_HEADER_CLICK, 0, col) ||
                         !headerCurrent() || rightClick ) return;
                    if ( col->IsSortable() )
                    {
                        col->SetSortOrder(!col->IsSortKey() || !col->IsSortOrderAscending());
                        if ( headerCurrent() ) self->Send(wxEVT_DATAVIEW_COLUMN_SORTED, 0, col);
                    }
                };
                header.Children().Append(peer->grid);
                if ( !Alive(stamp) ) return;
            }
            peer->grid.Width(width);
            if ( !Alive(stamp) ) return;
            peer->grip.Visibility(col->IsResizeable() ? X::Visibility::Visible : X::Visibility::Collapsed);
            if ( !Alive(stamp) ) return;
            peer->moveGrip.Visibility(col->IsReorderable() ? X::Visibility::Visible : X::Visibility::Collapsed);
            if ( !Alive(stamp) ) return;
            peer->button.Padding({12, 0, 12, 0});
            if ( !Alive(stamp) ) return;
            if ( peer->binding->moving && (peer->binding->moveVersion != stamp || !col->IsReorderable()) )
                peer->moveThumb.CancelDrag();
            if ( !Alive(stamp) ) return;
            wxString title = col->GetTitle();
            if ( !Alive(stamp) ) return;
            if ( col->IsSortKey() ) title += col->IsSortOrderAscending() ? wxS(" \u25b2") : wxS(" \u25bc");
            if ( !Alive(stamp) ) return;
            if ( peer->title != title )
            {
                peer->title = title;
                peer->button.Content(winrt::box_value(wxWinUIToHString(title)));
                if ( !Alive(stamp) ) return;
            }
            const auto accessibleTitle = col->GetTitle();
            if ( !Alive(stamp) ) return;
            X::Automation::AutomationProperties::SetName(peer->grid, wxWinUIToHString(accessibleTitle));
            if ( !Alive(stamp) ) return;
            wxWinUIApplyTableHeaderFont(peer->button, owner->GetFont());
            if ( !Alive(stamp) ) return;
            C::Canvas::SetLeft(peer->grid, left);
            if ( !Alive(stamp) ) return;
        }
        left += width;
    }
    // Keep live headers stable through width/model updates: destroying the
    // Thumb during DragDelta would prematurely release native pointer capture.
    const auto oldHeaders = headers;
    for ( const auto& entry : oldHeaders )
    {
        if ( retained.count(entry.first) ) continue;
        headers.erase(entry.first);
        entry.second->Retire();
        if ( !Alive(stamp) ) return;
        uint32_t index;
        if ( header.Children().IndexOf(entry.second->grid, index) )
            header.Children().RemoveAt(index);
        if ( !Alive(stamp) ) return;
    }
}

void wxWinUIDataViewImpl::Realize(const C::ListViewItem& container, ItemKey key)
{
    const auto keepAlive = shared_from_this();
    if ( !owner || !owner->GetModel() ) return;
    if ( retiringEditors ) { pendingProjection = true; return; }
    const int rowIndex = RowIndex(key);
    if ( rowIndex == wxNOT_FOUND ) return;
    auto nativeCanvas = container.ContentTemplateRoot().try_as<C::Canvas>();
    if ( !nativeCanvas ) return;
    const Row row = rows[rowIndex];
    const auto stamp = generation;
    ModelPin model(owner->GetModel());
    const auto item = Item(key);
    const auto columnSnapshot = displayColumns;
    const std::weak_ptr<wxWinUIDataViewImpl> weak = shared_from_this();
    const double totalWidth = std::max(TotalWidth(), list.ActualWidth());
    if ( !Alive(stamp) ) return;
    std::vector<C::Grid> cells;
    if ( container.MinHeight() != rowHeight ) container.MinHeight(rowHeight);
    double left = 0;
    for ( auto col : columnSnapshot )
    {
        if ( !Alive(stamp) ) return;
        if ( col->IsHidden() ) continue;
        const double width = ColumnWidth(col);
        if ( !Alive(stamp) ) return;
        if ( !VisibleColumn(left, width) ) { left += width; continue; }
        auto renderer = col->GetRenderer();
        const auto mode = renderer->GetMode();
        const auto rowCache = automationCells.find(key);
        if ( rowCache != automationCells.end() )
        {
            const auto cached = rowCache->second.find(col);
            if ( cached != rowCache->second.end() )
            {
                const auto saved = cached->second;
                const auto visual = saved.visual.get();
                if ( saved.presentation && *saved.presentation == stamp && visual &&
                     X::Media::VisualTreeHelper::GetParent(visual) == nativeCanvas &&
                     saved.depth == row.depth && saved.container == row.container &&
                     saved.renderer == renderer && saved.mode == mode &&
                     saved.alignment == renderer->GetAlignment() &&
                     (!saved.custom || (visual.Width() == width && visual.Height() == rowHeight &&
                                        saved.selected == (selected.count(key) != 0) &&
                                        saved.focused == nativeFocused)) )
                {
                    // Built-in XAML cells adapt their layout in place. Only a
                    // custom renderer's size-dependent bitmap needs rendering.
                    if ( visual.Width() != width ) visual.Width(width);
                    if ( !Alive(stamp) ) return;
                    if ( visual.Height() != rowHeight ) visual.Height(rowHeight);
                    if ( !Alive(stamp) ) return;
                    if ( const auto content = saved.content.get() )
                    {
                        const double available = std::max(0.0, width - saved.indent - 25);
                        if ( content.MaxWidth() != available ) content.MaxWidth(available);
                        if ( !Alive(stamp) ) return;
                    }
                    if ( const auto toggle = saved.expander.get() )
                    {
                        const bool opened = expanded.count(key) != 0;
                        if ( saved.expanded != opened )
                            SetExpanderGlyph(toggle, opened);
                        if ( !Alive(stamp) ) return;
                        automationCells[key][col].expanded = opened;
                    }
                    if ( C::Canvas::GetLeft(visual) != left ) C::Canvas::SetLeft(visual, left);
                    if ( !Alive(stamp) ) return;
                    cells.push_back(visual.as<C::Grid>());
                    left += width;
                    continue;
                }
            }
        }
        const unsigned modelColumn = col->GetModelColumn();
        const bool hasValue = model.model->HasValue(item, modelColumn);
        if ( !Alive(stamp) ) return;
        wxVariant value;
        if ( hasValue ) model.model->GetValue(value, item, modelColumn);
        if ( !Alive(stamp) ) return;
        const bool enabled = model.model->IsEnabled(item, modelColumn);
        if ( !Alive(stamp) ) return;
        wxDataViewItemAttr attr;
        model.model->GetAttr(item, modelColumn, attr);
        if ( !Alive(stamp) ) return;

        auto& rowToken = automationRows[key];
        if ( !rowToken ) rowToken = std::make_shared<int>(0);
        auto& columnToken = automationColumns[col];
        if ( !columnToken ) columnToken = std::make_shared<int>(0);
        const std::weak_ptr<int> weakRow = rowToken, weakColumn = columnToken;
        const auto presentation = std::make_shared<std::uint64_t>(stamp);
        const std::weak_ptr<std::uint64_t> weakPresentation = presentation;
        C::Grid cell = winrt::make<U::CellElement>([weak, weakRow, weakColumn, key, col]() -> U::A::AutomationPeer
        {
            auto self = weak.lock();
            if ( !self || !self->AutomationCurrent() || weakRow.expired() || weakColumn.expired() ) return nullptr;
            return self->AutomationCellPeer(key, col);
        });
        const auto weakVisual = winrt::make_weak(cell.as<X::FrameworkElement>());
        const auto weakCanvas = winrt::make_weak(nativeCanvas);
        const auto resolvePresentation = [weak, weakRow, weakColumn, weakPresentation,
                                          weakVisual, weakCanvas, key, col]
        {
            auto self = weak.lock();
            const auto epoch = weakPresentation.lock();
            const auto visual = weakVisual.get();
            const auto mount = weakCanvas.get();
            if ( !self || self->publishingRows || !epoch || !self->Alive(*epoch) || weakRow.expired() ||
                 weakColumn.expired() || !visual || !mount ||
                 X::Media::VisualTreeHelper::GetParent(visual) != mount ||
                 self->RowIndex(key) == wxNOT_FOUND || self->ColumnIndex(col) == wxNOT_FOUND )
                return std::shared_ptr<wxWinUIDataViewImpl>();
            return self;
        };
        cell.Width(width);
        cell.Height(rowHeight);
        // The lines of a table, for what is one: a list, or a tree whose
        // program asked for rules.
        const bool table = model.model && model.model->IsListModel();
        C::Border border = wxWinUICreateTableCellBorder(
            table || owner->HasFlag(wxDV_VERT_RULES),
            table || owner->HasFlag(wxDV_HORIZ_RULES));
        border.Padding({12, 0, 12, 0});
        if ( attr.HasBackgroundColour() && !wxWinUIIsHighContrastActive() )
            border.Background(wxWinUICreateColourBrush(attr.GetBackgroundColour()));
        C::Grid content;
        border.Child(content);
        cell.Children().Append(border);
        X::FrameworkElement visual{nullptr};
        bool custom = false;

        if ( !hasValue ) visual = C::TextBlock();
        else if ( renderer->GetClassInfo() == wxCLASSINFO(wxDataViewToggleRenderer) && value.IsType(wxS("bool")) )
        {
            C::CheckBox check;
            check.IsChecked(value.GetBool());
            check.MinWidth(0);
            check.MinHeight(0);
            check.VerticalAlignment(X::VerticalAlignment::Center);
            check.IsEnabled(enabled && mode != wxDATAVIEW_CELL_INERT);
            check.Click([resolvePresentation, key, col](const auto& sender, const auto&)
            {
                auto self = resolvePresentation();
                if ( !self ) return;
                auto native = sender.template as<C::CheckBox>();
                self->ChangeValue(key, col, wxVariant(native.IsChecked().Value()));
            });
            visual = check;
        }
        else if ( renderer->GetClassInfo() == wxCLASSINFO(wxDataViewProgressRenderer) && value.IsType(wxS("long")) )
        {
            C::ProgressBar progress;
            progress.Minimum(0);
            progress.Maximum(100);
            progress.Value(std::max(0L, std::min(100L, value.GetLong())));
            progress.VerticalAlignment(X::VerticalAlignment::Center);
            visual = progress;
        }
        else if ( renderer->GetClassInfo() == wxCLASSINFO(wxDataViewTextRenderer) ||
                  renderer->GetClassInfo() == wxCLASSINFO(wxDataViewIconTextRenderer) ||
                  renderer->GetClassInfo() == wxCLASSINFO(wxDataViewCheckIconTextRenderer) ||
                  renderer->GetClassInfo() == wxCLASSINFO(wxDataViewBitmapRenderer) )
        {
            C::StackPanel stack;
            stack.Orientation(C::Orientation::Horizontal);
            stack.Spacing(6);
            stack.VerticalAlignment(X::VerticalAlignment::Center);
            wxString text;
            wxBitmap bitmap;
            if ( value.IsType(wxS("wxDataViewCheckIconText")) )
            {
                wxDataViewCheckIconText iconText;
                iconText << value;
                text = iconText.GetText();
                bitmap = iconText.GetBitmapBundle().GetBitmapFor(owner.get());
                C::CheckBox check;
                check.MinWidth(0);
                check.MinHeight(0);
                check.VerticalAlignment(X::VerticalAlignment::Center);
                check.IsEnabled(enabled && mode != wxDATAVIEW_CELL_INERT);
                if ( iconText.GetCheckedState() == wxCHK_UNDETERMINED )
                    check.IsChecked(nullptr);
                else
                    check.IsChecked(iconText.GetCheckedState() == wxCHK_CHECKED);
                X::Automation::AutomationProperties::SetName(check, wxWinUIToHString(text));
                check.Click([resolvePresentation, key, col](const auto&, const auto&)
                {
                    auto self = resolvePresentation();
                    if ( !self ) return;
                    const auto stamp = self->generation;
                    ModelPin currentModel(self->owner->GetModel());
                    if ( !currentModel.model ) return;
                    const unsigned modelColumn = col->GetModelColumn();
                    auto renderer = col->GetRenderer();
                    // The public renderer owns the two/three-state cycle.
                    // Reuse its activation semantics, never its wxDC drawing.
                    if ( !renderer->PrepareForItem(currentModel.model, Item(key), modelColumn) ||
                         !self->Alive(stamp) ) return;
                    renderer->WXActivateCell(wxRect(), currentModel.model,
                                               Item(key), modelColumn, nullptr);
                    // A rejected model write must also restore native state.
                    // Successful notifications have already rebuilt the row.
                    if ( self->Alive(stamp) ) self->ModelChanged(key, static_cast<int>(modelColumn));
                });
                stack.Children().Append(check);
            }
            else if ( value.IsType(wxS("wxDataViewIconText")) )
            {
                wxDataViewIconText iconText;
                iconText << value;
                text = iconText.GetText();
                bitmap = iconText.GetBitmapBundle().GetBitmapFor(owner.get());
            }
            else if ( value.IsType(wxS("wxBitmap")) ) bitmap << value;
            else if ( value.IsType(wxS("wxBitmapBundle")) )
            {
                wxBitmapBundle bundle;
                bundle << value;
                bitmap = bundle.GetBitmapFor(owner.get());
            }
            else text = value.MakeString();
            if ( bitmap.IsOk() )
            {
                C::Image image;
                image.Source(wxWinUIWriteableBitmapFromBitmap(bitmap));
                image.Width(bitmap.GetLogicalWidth() / Scale());
                image.Height(std::min(rowHeight - 4, bitmap.GetLogicalHeight() / Scale()));
                stack.Children().Append(image);
            }
            if ( !text.empty() )
            {
                C::TextBlock label;
                label.Text(wxWinUIToHString(text));
                label.TextTrimming(X::TextTrimming::CharacterEllipsis);
                label.VerticalAlignment(X::VerticalAlignment::Center);
                wxWinUIApplyFont(label, attr.GetEffectiveFont(owner->GetFont()));
                if ( attr.HasColour() ) wxWinUIApplyForeground(label, attr.GetColour());
                else wxWinUIApplyForeground(label, owner->GetForegroundColour());
                stack.Children().Append(label);
            }
            visual = stack;
        }
        else if ( EditorKind(renderer) != NativeEditorKind::Custom )
        {
            wxString text;
            switch ( EditorKind(renderer) )
            {
                case NativeEditorKind::Choice:
                    if ( value.IsType(wxS("string")) ) text = value.GetString();
                    break;
                case NativeEditorKind::ChoiceIndex:
                    if ( value.IsType(wxS("long")) )
                    {
                        const auto& choices = static_cast<wxDataViewChoiceRenderer*>(renderer)->GetChoices();
                        const long index = value.GetLong();
                        if ( index >= 0 && static_cast<size_t>(index) < choices.size() )
                            text = choices[static_cast<size_t>(index)];
                    }
                    break;
                case NativeEditorKind::Spin:
                    if ( value.IsType(wxS("long")) ) text = wxString::Format("%ld", value.GetLong());
                    break;
#if wxUSE_DATEPICKCTRL
                case NativeEditorKind::Date:
                    if ( value.IsType(wxS("datetime")) && value.GetDateTime().IsValid() )
                        text = value.GetDateTime().Format(wxGetUIDateFormat());
                    break;
#endif
                default: break;
            }
            C::TextBlock label;
            label.Text(wxWinUIToHString(text));
            label.TextTrimming(X::TextTrimming::CharacterEllipsis);
            label.VerticalAlignment(X::VerticalAlignment::Center);
            label.HorizontalAlignment(renderer->GetAlignment() & wxALIGN_RIGHT ? X::HorizontalAlignment::Right :
                renderer->GetAlignment() & wxALIGN_CENTER_HORIZONTAL ? X::HorizontalAlignment::Center : X::HorizontalAlignment::Left);
            wxWinUIApplyFont(label, attr.GetEffectiveFont(owner->GetFont()));
            wxWinUIApplyForeground(label, attr.HasColour() ? attr.GetColour() : owner->GetForegroundColour());
            visual = label;
        }
        else
        {
            custom = true;
            // Public custom renderers explicitly own their wxDC drawing. Keep
            // this compatibility path confined to their realized cell; built-in
            // text, bitmap, toggle and progress cells above are genuine XAML.
            const bool prepared = renderer->PrepareForItem(model.model, item, modelColumn);
            if ( !Alive(stamp) ) return;
            if ( prepared )
            {
                wxBitmap bitmap(std::max(1, static_cast<int>(width * Scale())),
                                std::max(1, static_cast<int>(rowHeight * Scale())), 32);
                wxMemoryDC dc(bitmap);
                dc.SetBackground(wxBrush(owner->GetBackgroundColour()));
                dc.Clear();
                dc.SetFont(attr.GetEffectiveFont(owner->GetFont()));
                dc.SetTextForeground(attr.HasColour() ? attr.GetColour() : owner->GetForegroundColour());
                renderer->Render(wxRect(wxPoint(), bitmap.GetSize()), &dc,
                    (selected.count(key) ? wxDATAVIEW_CELL_SELECTED : 0) |
                    (nativeFocused ? wxDATAVIEW_CELL_FOCUSED : 0) |
                    (!enabled ? wxDATAVIEW_CELL_INSENSITIVE : 0));
                dc.SelectObject(wxNullBitmap);
                if ( !Alive(stamp) ) return;
                C::Image image;
                image.Source(wxWinUIWriteableBitmapFromBitmap(bitmap));
                image.Stretch(X::Media::Stretch::Fill);
                visual = image;
            }
        }
        if ( !Alive(stamp) ) return;
        const bool expander = col == owner->GetExpanderColumn() ||
            (!owner->GetExpanderColumn() && col == columns.front());
        double indent = 0;
        C::Button expanderButton{nullptr};
        if ( expander && !model.model->IsListModel() )
        {
            if ( !Alive(stamp) ) return;
            indent = row.depth * std::max(16, owner->GetIndent()) / Scale() + 24;
            if ( row.container )
            {
                C::Button toggle;
                wxWinUIApplySubtleButtonStyle(toggle);
                SetExpanderGlyph(toggle, expanded.count(key) != 0);
                toggle.Width(24);
                toggle.Height(24);
                toggle.Padding({0, 0, 0, 0});
                toggle.HorizontalAlignment(X::HorizontalAlignment::Left);
                toggle.VerticalAlignment(X::VerticalAlignment::Center);
                toggle.Margin({indent - 24, 0, 0, 0});
                toggle.Click([resolvePresentation, key](const auto&, const auto&)
                {
                    auto self = resolvePresentation();
                    if ( !self ) return;
                    if ( self->expanded.count(key) ) self->owner->Collapse(Item(key));
                    else self->owner->Expand(Item(key));
                });
                content.Children().Append(toggle);
                expanderButton = toggle;
            }
        }
        if ( visual )
        {
            visual.Margin({indent, 0, 0, 0});
            // Between the paddings of the cell, 12 on each side, and before
            // the line at its end: wider, the cell is wider than its column.
            visual.MaxWidth(std::max(0.0, width - indent - 25));
            visual.Opacity(enabled ? 1.0 : 0.45);
            content.Children().Append(visual);
        }
        cell.Tapped([resolvePresentation, key, col](const auto&, const auto&)
        {
            auto self = resolvePresentation();
            if ( !self ) return;
            self->current = key;
            self->currentColumn = col;
        });
        cell.DoubleTapped([resolvePresentation, key, col, hasValue](const auto&, const auto&)
        {
            auto self = resolvePresentation();
            if ( !self || !hasValue ) return;
            if ( col->GetRenderer()->GetMode() == wxDATAVIEW_CELL_EDITABLE )
                self->BeginEdit(key, col);
            else
            {
                // The activation handler may open a modal dialog: it runs
                // once this callback has returned.
                wxWinUIPostNotification([resolvePresentation, key, col]()
                {
                    auto live = resolvePresentation();
                    if ( live && live->owner &&
                         live->owner->GetColumnIndex(col) != wxNOT_FOUND )
                    {
                        live->Send(wxEVT_DATAVIEW_ITEM_ACTIVATED, key, col);
                    }
                });
            }
        });
        C::Canvas::SetLeft(cell, left);
        cells.push_back(cell);
        auto& saved = automationCells[key][col];
        saved.visual = winrt::make_weak(cell.as<X::FrameworkElement>());
        saved.content = visual ? winrt::make_weak(visual) : winrt::weak_ref<X::FrameworkElement>();
        saved.expander = expanderButton ? winrt::make_weak(expanderButton) : winrt::weak_ref<C::Button>();
        saved.indent = indent;
        saved.depth = row.depth;
        saved.container = row.container;
        saved.expanded = expanded.count(key) != 0;
        saved.presentation = presentation;
        saved.custom = custom;
        saved.selected = selected.count(key) != 0;
        saved.focused = nativeFocused;
        saved.renderer = renderer;
        saved.mode = mode;
        saved.alignment = renderer->GetAlignment();
        left += width;
    }
    if ( !Alive(stamp) ) return;
    // ListView's ItemsSource owns Content. Only ContentTemplateRoot is the
    // rendered row surface; assigning Content(Canvas) bypasses the template.
    if ( nativeCanvas.Width() != totalWidth ) nativeCanvas.Width(totalWidth);
    if ( nativeCanvas.Height() != rowHeight ) nativeCanvas.Height(rowHeight);
    // Keep unaffected native elements mounted. Clearing the Canvas on every
    // ViewChanged also restarts their visual states and makes a whole row flash.
    for ( auto n = nativeCanvas.Children().Size(); n != 0 && Alive(stamp); --n )
    {
        const auto child = nativeCanvas.Children().GetAt(n - 1);
        if ( child != editor && std::none_of(cells.begin(), cells.end(), [&child](const C::Grid& cell)
                { return cell == child; }) )
            nativeCanvas.Children().RemoveAt(n - 1);
    }
    for ( uint32_t n = 0; n < cells.size() && Alive(stamp); ++n )
    {
        if ( n < nativeCanvas.Children().Size() && nativeCanvas.Children().GetAt(n) == cells[n] ) continue;
        uint32_t previous;
        if ( nativeCanvas.Children().IndexOf(cells[n], previous) ) nativeCanvas.Children().RemoveAt(previous);
        nativeCanvas.Children().InsertAt(n, cells[n]);
    }
    // The only non-cell child retained above is the current in-row editor.
    // It stays above the cells without being removed/reinserted on resize.
    if ( !Alive(stamp) ) return;
    for ( auto& saved : automationCells[key] )
        if ( const auto visual = saved.second.visual.get() )
            if ( X::Media::VisualTreeHelper::GetParent(visual) != nativeCanvas )
                saved.second.presentation.reset();
}

void wxWinUIDataViewImpl::RefreshVisuals()
{
    const auto keepAlive = shared_from_this();
    if ( !owner || !list ) return;
    if ( retiringEditors ) { pendingProjection = true; return; }
    if ( refreshingVisuals ) return;
    refreshingVisuals = true;
    auto guard = wxMakeGuard([this] { refreshingVisuals = false; });
    wxUnusedVar(guard);
    // Routed focus events can be delayed or originate in an inner template.
    // Use the actual focused descendant, not their arrival order, as the
    // custom renderer's focus state (native controls manage their own states).
    nativeFocused = false;
    if ( const auto xamlRoot = root.XamlRoot() )
    {
        auto focused = X::Input::FocusManager::GetFocusedElement(xamlRoot).try_as<X::DependencyObject>();
        while ( focused )
        {
            if ( focused == root ) { nativeFocused = true; break; }
            focused = X::Media::VisualTreeHelper::GetParent(focused);
        }
    }
    // A value/attribute callback may invalidate this projection. Retry once
    // against the resulting state, never recursively render or queue an
    // unbounded series of work for a self-notifying model.
    for ( unsigned attempt = 0; attempt != 2 && owner; ++attempt )
    {
        const auto stamp = generation;
        BuildHeader();
        if ( !Alive(stamp) ) continue;
        // Recycled visuals must not leave one cache entry for every row ever
        // scrolled past. Keep model-backed peers explicitly requested by UIA.
        for ( auto row = automationCells.begin(); row != automationCells.end(); )
        {
            for ( auto cell = row->second.begin(); cell != row->second.end(); )
                if ( !cell->second.peer && !cell->second.visual.get() )
                    cell = row->second.erase(cell);
                else ++cell;
            if ( row->second.empty() )
            {
                const auto token = automationRows.find(row->first);
                if ( token != automationRows.end() && *token->second == 0 ) automationRows.erase(token);
                row = automationCells.erase(row);
            }
            else ++row;
        }
        // Query realized containers only. A value-only notification neither
        // walks the model nor rebuilds/replaces its native ItemsSource.
        const auto panel = list.ItemsPanelRoot().try_as<C::ItemsStackPanel>();
        if ( !panel ) return;
        const int first = panel.FirstCacheIndex();
        const int last = panel.LastCacheIndex();
        for ( int n = std::max(0, first); n <= last && n < static_cast<int>(rows.size()); ++n )
        {
            const auto container = list.ContainerFromIndex(n).try_as<C::ListViewItem>();
            if ( container ) Realize(container, rows[n].key);
            if ( !Alive(stamp) ) break;
        }
        if ( Alive(stamp) )
        {
            LayoutEditor();
            if ( Alive(stamp) ) return;
        }
    }
}

void wxWinUIDataViewImpl::LayoutEditor()
{
    if ( !owner || !editor || !editingItem || !editingColumn ) return;
    const auto native = editor;
    const auto transaction = editGeneration;
    const auto stamp = generation;
    const auto currentEditor = [&]
    {
        return Alive(stamp) && editor == native && editGeneration == transaction &&
               ColumnIndex(editingColumn) != wxNOT_FOUND;
    };
    const int index = RowIndex(editingItem);
    if ( index == wxNOT_FOUND ) return;
    const auto container = list.ContainerFromIndex(index).try_as<C::ListViewItem>();
    const auto canvas = container ? container.ContentTemplateRoot().try_as<C::Canvas>() : nullptr;
    if ( !canvas || !currentEditor() ) return;
    const double width = ColumnWidth(editingColumn);
    if ( !currentEditor() ) return;
    double left = ColumnLeft(editingColumn);
    if ( !currentEditor() ) return;
    const auto parent = X::Media::VisualTreeHelper::GetParent(native);
    if ( parent == editorOverlay )
    {
        const auto origin = canvas.TransformToVisual(editorOverlay).TransformPoint({0, 0});
        if ( !currentEditor() ) return;
        left += origin.X;
        if ( C::Canvas::GetTop(native) != origin.Y ) C::Canvas::SetTop(native, origin.Y);
        if ( !currentEditor() ) return;
    }
    else if ( parent != canvas ) return;
    if ( native.Width() != width ) native.Width(width);
    if ( !currentEditor() ) return;
    if ( C::Canvas::GetLeft(native) != left ) C::Canvas::SetLeft(native, left);
    if ( !currentEditor() ) return;
    const double height = std::max(32.0, rowHeight);
    if ( native.Height() != height ) native.Height(height);
}

void wxWinUIDataViewImpl::ChangeValue(ItemKey key, wxDataViewColumn* col, wxVariant value)
{
    const auto keepAlive = shared_from_this();
    if ( !owner || ColumnIndex(col) == wxNOT_FOUND || RowIndex(key) == wxNOT_FOUND ) return;
    const auto stamp = generation;
    ModelPin model(owner->GetModel());
    if ( !model.model ) return;
    const unsigned index = col->GetModelColumn();
    const bool valid = col->GetRenderer()->Validate(value);
    if ( !Alive(stamp) ) return;
    if ( valid ) model.model->ChangeValue(value, Item(key), index);
    // A rejected toggle must restore its model value too. Successful model
    // notifications have already advanced the epoch and updated this cell.
    if ( Alive(stamp) ) ModelChanged(key, static_cast<int>(index));
}

void wxWinUIDataViewImpl::BeginEdit(ItemKey key, wxDataViewColumn* col)
{
    const auto keepAlive = shared_from_this();
    if ( !owner || ColumnIndex(col) == wxNOT_FOUND || RowIndex(key) == wxNOT_FOUND ) return;
    if ( !EndEdit(false) || !owner || retiringEditors ||
         ColumnIndex(col) == wxNOT_FOUND || RowIndex(key) == wxNOT_FOUND ) return;
    if ( col->GetRenderer()->GetMode() != wxDATAVIEW_CELL_EDITABLE ) return;
    const auto kind = EditorKind(col->GetRenderer());
    if ( kind == NativeEditorKind::Custom )
    {
        // Retain custom editor callbacks for renderer-defined editors. They
        // are wx controls, which use their own WinUI peer when available.
        col->GetRenderer()->StartEditing(Item(key), owner->GetItemRect(Item(key), col));
        return;
    }
    auto renderer = col->GetRenderer();
    auto editOwner = owner.get();
    const auto transaction = wxPrivate::BeginDataViewEdit(editOwner, renderer);
    if ( !transaction ) return;
    nativeEditRenderer = renderer;
    editGeneration = transaction;
    bool editorPublished = false;
    auto reservationGuard = wxMakeGuard([&, editOwner, renderer, transaction]
    {
        if ( editorPublished ) return;
        wxPrivate::EndDataViewEdit(editOwner, renderer, transaction);
        if ( editGeneration == transaction )
        {
            nativeEditRenderer = nullptr;
            editGeneration = 0;
        }
    });
    wxUnusedVar(reservationGuard);
    const auto stamp = generation;
    ModelPin model(owner->GetModel());
    if ( !model.model || !model.model->IsEnabled(Item(key), col->GetModelColumn()) || !Alive(stamp) ) return;
    wxVariant value;
    model.model->GetValue(value, Item(key), col->GetModelColumn());
    if ( !Alive(stamp) || !Send(wxEVT_DATAVIEW_ITEM_START_EDITING, key, col) ) return;
    owner->EnsureVisible(Item(key), col);
    if ( !Alive(stamp) || ColumnIndex(col) == wxNOT_FOUND ||
         col->GetRenderer() != renderer ) return;
    list.UpdateLayout();
    if ( !Alive(stamp) ) return;
    const auto container = list.ContainerFromIndex(RowIndex(key)).try_as<C::ListViewItem>();
    if ( !container ) return;
    auto canvas = container.ContentTemplateRoot().try_as<C::Canvas>();
    if ( !canvas ) return;
    double left = 0;
    const auto columnSnapshot = displayColumns;
    for ( auto previous : columnSnapshot )
    {
        if ( !Alive(stamp) || ColumnIndex(previous) == wxNOT_FOUND ) return;
        if ( previous == col ) break;
        if ( !previous->IsHidden() ) left += ColumnWidth(previous);
        if ( !Alive(stamp) ) return;
    }
    const double editorWidth = ColumnWidth(col);
    if ( !Alive(stamp) || ColumnIndex(col) == wxNOT_FOUND ||
         col->GetRenderer() != renderer ) return;
    C::Control nativeEditor{nullptr};
    if ( kind == NativeEditorKind::Text )
    {
        C::TextBox text;
        text.Text(wxWinUIToHString(value.MakeString()));
        nativeEditor = text;
    }
    else if ( kind == NativeEditorKind::Choice || kind == NativeEditorKind::ChoiceIndex )
    {
        const auto& choices = static_cast<wxDataViewChoiceRenderer*>(renderer)->GetChoices();
        if ( (kind == NativeEditorKind::Choice && !value.IsType(wxS("string"))) ||
             (kind == NativeEditorKind::ChoiceIndex && !value.IsType(wxS("long"))) ) return;
        C::ComboBox choice;
        for ( const auto& label : choices ) choice.Items().Append(winrt::box_value(wxWinUIToHString(label)));
        long index = kind == NativeEditorKind::ChoiceIndex ? value.GetLong() : choices.Index(value.GetString());
        if ( index < 0 || static_cast<size_t>(index) >= choices.size() ) index = -1;
        choice.SelectedIndex(static_cast<int>(index));
        nativeEditor = choice;
    }
#if wxUSE_SPINCTRL
    else if ( kind == NativeEditorKind::Spin )
    {
        if ( !value.IsType(wxS("long")) ) return;
        const auto spinRenderer = static_cast<wxDataViewSpinRenderer*>(renderer);
        if ( spinRenderer->m_min > spinRenderer->m_max ) return;
        C::NumberBox spin;
        spin.Minimum(spinRenderer->m_min);
        spin.Maximum(spinRenderer->m_max);
        spin.SmallChange(1);
        spin.LargeChange(10);
        spin.SpinButtonPlacementMode(C::NumberBoxSpinButtonPlacementMode::Inline);
        spin.ValidationMode(C::NumberBoxValidationMode::InvalidInputOverwritten);
        spin.AcceptsExpression(false);
        namespace N = winrt::Windows::Globalization::NumberFormatting;
        N::DecimalFormatter formatter;
        formatter.IntegerDigits(1);
        formatter.FractionDigits(0);
        N::IncrementNumberRounder rounder;
        rounder.Increment(1);
        formatter.NumberRounder(rounder);
        spin.NumberFormatter(formatter);
        spin.Value(std::max(spinRenderer->m_min, std::min(spinRenderer->m_max, value.GetLong())));
        nativeEditor = spin;
    }
#endif
#if wxUSE_DATEPICKCTRL
    else if ( kind == NativeEditorKind::Date )
    {
        if ( !value.IsType(wxS("datetime")) ) return;
        C::CalendarDatePicker date;
        date.CalendarIdentifier(winrt::Windows::Globalization::CalendarIdentifiers::Gregorian());
        const auto civil = value.GetDateTime();
        F::DateTime native;
        if ( civil.IsValid() )
        {
            if ( !ToNativeCivilDate(civil, &native) ) return;
            // WinUI defaults to a moving century. Preserve an existing wx
            // value outside that window instead of silently changing it.
            if ( native < date.MinDate() ) date.MinDate(native);
            if ( native > date.MaxDate() ) date.MaxDate(native);
            date.Date(native);
        }
        else date.Date(nullptr);
        nativeEditor = date;
    }
#endif
    if ( !nativeEditor ) return;
    editor = nativeEditor;
    editorKind = kind;
    editorFocusCheckQueued = false;
    editorHorizontalOffset = scroll ? scroll.HorizontalOffset() : 0;
    editorVerticalOffset = scroll ? scroll.VerticalOffset() : 0;
    editor.Width(editorWidth);
    editor.Height(std::max(32.0, rowHeight));
    editor.Padding({6, 2, 6, 2});
    const bool popupEditor = kind == NativeEditorKind::Choice ||
                             kind == NativeEditorKind::ChoiceIndex || kind == NativeEditorKind::Date;
    if ( popupEditor )
    {
        const auto origin = canvas.TransformToVisual(editorOverlay).TransformPoint({0, 0});
        left += origin.X;
        C::Canvas::SetTop(editor, origin.Y);
        canvas = editorOverlay;
        editorRetirement = wxWinUIDataViewEditorRetirement::Create(editor, canvas, editorGate);
        if ( !editorRetirement ) { editor = nullptr; return; }
    }
    C::Canvas::SetLeft(editor, left);
    editingItem = key;
    editingColumn = col;
    editingValue = value;
    editorPublished = true;
    const std::weak_ptr<wxWinUIDataViewImpl> weak = shared_from_this();
    editor.KeyDown([weak, transaction](const auto&, const X::Input::KeyRoutedEventArgs& args)
    {
        auto self = weak.lock();
        if ( !self || !self->owner || self->editGeneration != transaction ) return;
        if ( args.Key() == winrt::Windows::System::VirtualKey::Enter ||
             args.Key() == winrt::Windows::System::VirtualKey::Escape )
        {
            // The popup owns Enter while choosing a value. Its close/focus
            // boundary decides whether the cell edit should finish.
            if ( args.Key() == winrt::Windows::System::VirtualKey::Enter && self->EditorPopupOpen() ) return;
            args.Handled(true);
            self->EndEdit(args.Key() == winrt::Windows::System::VirtualKey::Escape);
        }
    });
    editor.LostFocus([weak, transaction](const auto&, const auto&)
    {
        if ( auto self = weak.lock() )
            if ( self->editGeneration == transaction ) self->CheckEditorFocus();
    });
    if ( auto choice = editor.try_as<C::ComboBox>() )
        choice.DropDownClosed([weak, transaction](const auto&, const auto&)
        {
            if ( auto self = weak.lock() )
                if ( self->editGeneration == transaction ) self->CheckEditorFocus();
        });
    if ( auto date = editor.try_as<C::CalendarDatePicker>() )
        date.Closed([weak, transaction](const auto&, const auto&)
        {
            if ( auto self = weak.lock() )
                if ( self->editGeneration == transaction ) self->CheckEditorFocus();
        });
    canvas.Children().Append(editor);
    if ( !SyncLayout() || editingItem != key || editor != nativeEditor ) return;
    if ( auto text = nativeEditor.try_as<C::TextBox>() ) text.SelectAll();
    else if ( kind == NativeEditorKind::Spin )
        if ( auto spinText = FindEditorTextBox(nativeEditor) ) spinText.SelectAll();
    // A raw XAML Focus() does not acquire the wx/HWND focus authority. This
    // mirrors the common renderer's wx editor SetFocus() before native focus.
    owner->SetFocus();
    if ( !Alive(stamp) || editingItem != key || editor != nativeEditor ) return;
    if ( !nativeEditor.Focus(X::FocusState::Programmatic) )
    {
        EndEdit(true);
        return;
    }
    if ( !Alive(stamp) || editingItem != key || editor != nativeEditor ) return;
    Send(wxEVT_DATAVIEW_ITEM_EDITING_STARTED, key, col);
}

bool wxWinUIDataViewImpl::ReadEditorValue(wxVariant* value)
{
    if ( !editor ) return false;
    try
    {
        switch ( editorKind )
        {
            case NativeEditorKind::Text:
                *value = wxWinUIFromHString(editor.as<C::TextBox>().Text());
                return true;
            case NativeEditorKind::Choice:
            case NativeEditorKind::ChoiceIndex:
            {
                const auto choice = editor.as<C::ComboBox>();
                const int index = choice.SelectedIndex();
                if ( editorKind == NativeEditorKind::ChoiceIndex ) *value = static_cast<long>(index);
                else *value = index < 0 ? wxString() :
                    wxWinUIFromHString(winrt::unbox_value<winrt::hstring>(choice.Items().GetAt(index)));
                return true;
            }
            case NativeEditorKind::Spin:
            {
                const auto spin = editor.as<C::NumberBox>();
                const auto input = FindEditorTextBox(spin);
                const auto parser = spin.NumberFormatter().try_as<winrt::Windows::Globalization::NumberFormatting::INumberParser>();
                const auto number = parser ? parser.ParseDouble(input ? input.Text() : spin.Text()) : nullptr;
                if ( !number ) return false;
                double numeric = number.Value();
                if ( !std::isfinite(numeric) || std::trunc(numeric) != numeric ) return false;
                numeric = std::max(spin.Minimum(), std::min(spin.Maximum(), numeric));
                if ( numeric < LONG_MIN || numeric > LONG_MAX ) return false;
                *value = static_cast<long>(numeric);
                return true;
            }
#if wxUSE_DATEPICKCTRL
            case NativeEditorKind::Date:
            {
                const auto date = editor.as<C::CalendarDatePicker>().Date();
                *value = date ? FromNativeCivilDate(date.Value()) : wxDateTime();
                return true;
            }
#endif
            default: return false;
        }
    }
    catch ( const winrt::hresult_error& ) { return false; }
}

bool wxWinUIDataViewImpl::EditorPopupOpen() const
{
    return editorRetirement && editorRetirement->IsPopupOpen();
}

void wxWinUIDataViewImpl::CheckEditorFocus()
{
    if ( !owner || !editor || editorFocusCheckQueued ) return;
    editorFocusCheckQueued = true;
    const auto transaction = editGeneration;
    const std::weak_ptr<wxWinUIDataViewImpl> weak = shared_from_this();
    owner->CallAfter([weak, transaction]
    {
        auto self = weak.lock();
        if ( !self || !self->owner || self->editGeneration != transaction ) return;
        self->editorFocusCheckQueued = false;
        if ( !self->editor || self->EditorPopupOpen() ) return;
        const auto checkFocus = [weak, transaction]
        {
            auto live = weak.lock();
            if ( !live || !live->owner || live->editGeneration != transaction ||
                 !live->editor || live->EditorPopupOpen() ) return;
            const auto xamlRoot = live->editor.XamlRoot();
            if ( !xamlRoot ) return;
            auto focused = X::Input::FocusManager::GetFocusedElement(xamlRoot).try_as<X::DependencyObject>();
            while ( focused )
            {
                if ( focused == live->editor ) return;
                focused = X::Media::VisualTreeHelper::GetParent(focused);
            }
            live->EndEdit(false);
        };
        // Closed/IsOpen=false do not prove the popup focus transfer has
        // completed. This also gates older LostFocus callbacks already queued
        // before Closed, not just the callback originating from Closed itself.
        if ( self->editorRetirement ) self->editorRetirement->AfterPopupClosed(checkFocus);
        else checkFocus();
    });
}

void wxWinUIDataViewImpl::RetireEditor()
{
    const auto native = editor;
    editor = nullptr;
    editorKind = NativeEditorKind::Custom;
    editorFocusCheckQueued = false;
    if ( auto retirement = std::move(editorRetirement) )
    {
        ++retiringEditors;
        const std::weak_ptr<wxWinUIDataViewImpl> weak = shared_from_this();
        retirement->Retire([weak]
        {
            if ( auto self = weak.lock() )
            {
                --self->retiringEditors;
                if ( self->owner && !self->retiringEditors && self->pendingProjection )
                    self->QueueRebuild();
            }
        });
    }
    else if ( native )
    {
        if ( auto canvas = X::Media::VisualTreeHelper::GetParent(native).try_as<C::Canvas>() )
        {
            uint32_t index;
            if ( canvas.Children().IndexOf(native, index) ) canvas.Children().RemoveAt(index);
        }
    }
}

bool wxWinUIDataViewImpl::EndEdit(bool cancel)
{
    const auto keepAlive = shared_from_this();
    if ( !editingItem || endingEdit )
        return !editGeneration || !wxPrivate::IsCurrentDataViewEdit(
            owner.get(), nativeEditRenderer, editGeneration);
    auto editOwner = owner.get();
    const auto renderer = nativeEditRenderer;
    const auto transaction = editGeneration;
    endingEdit = true;
    auto guard = wxMakeGuard([this, editOwner, renderer, transaction]
    {
        wxPrivate::EndDataViewEdit(editOwner, renderer, transaction);
        if ( editGeneration == transaction )
        {
            nativeEditRenderer = nullptr;
            editGeneration = 0;
        }
        endingEdit = false;
    });
    wxUnusedVar(guard);
    const auto key = editingItem;
    const auto rowToken = automationRows.find(key);
    const auto columnToken = automationColumns.find(editingColumn);
    const std::weak_ptr<int> weakRow = rowToken == automationRows.end() ? std::weak_ptr<int>() : rowToken->second;
    const std::weak_ptr<int> weakColumn = columnToken == automationColumns.end() ? std::weak_ptr<int>() : columnToken->second;
    const bool explicitlyCancelled = cancel;
    auto col = editingColumn;
    wxVariant value(editingValue);
    if ( !cancel && !ReadEditorValue(&value) ) cancel = true;
    editingItem = 0;
    editingColumn = nullptr;
    RetireEditor();
    if ( !owner || ColumnIndex(col) == wxNOT_FOUND ) return false;
    ModelPin model(owner->GetModel());
    const auto stamp = generation;
    if ( !cancel && !col->GetRenderer()->Validate(value) ) cancel = true;
    if ( !Alive(stamp) || ColumnIndex(col) == wxNOT_FOUND ) return false;
    wxPrivate::ReleaseDataViewEditForDone(editOwner, renderer, transaction);
    wxDataViewEvent done(wxEVT_DATAVIEW_ITEM_EDITING_DONE, owner.get(), col, Item(key));
    if ( cancel ) done.SetEditCancelled();
    else done.SetValue(value);
    owner->GetEventHandler()->ProcessEvent(done);
    // The done handler may legitimately insert/remove other columns. Preserve
    // identity and use its NEW model mapping; do not require the old epoch.
    bool accepted = explicitlyCancelled;
    if ( !cancel && done.IsAllowed() && owner && owner->GetModel() == model.model &&
            model.model && !weakRow.expired() && !weakColumn.expired() &&
            ColumnIndex(col) != wxNOT_FOUND && RowIndex(key) != wxNOT_FOUND )
        accepted = model.model->ChangeValue(value, Item(key), col->GetModelColumn());
    if ( owner ) RefreshVisuals();
    return owner && !editingItem && accepted;
}

wxIMPLEMENT_DYNAMIC_CLASS(wxDataViewCtrl, wxDataViewCtrlBase);

wxDataViewColumn::wxDataViewColumn(const wxString& title,
                                  wxDataViewRenderer* renderer,
                                  unsigned int modelColumn, int width,
                                  wxAlignment align, int flags)
    : wxDataViewColumnBase(renderer, modelColumn), m_title(title),
      m_width(width), m_align(align), m_flags(flags)
{
    renderer->SetOwner(this);
}

wxDataViewColumn::wxDataViewColumn(const wxBitmapBundle& bitmap,
                                  wxDataViewRenderer* renderer,
                                  unsigned int modelColumn, int width,
                                  wxAlignment align, int flags)
    : wxDataViewColumnBase(bitmap, renderer, modelColumn),
      m_width(width), m_align(align), m_flags(flags)
{
    renderer->SetOwner(this);
}

void wxDataViewColumn::Changed()
{
    if ( m_owner ) m_owner->OnColumnChange(m_owner->GetColumnIndex(this));
}
void wxDataViewColumn::SetTitle(const wxString& title) { m_title = title; Changed(); }
void wxDataViewColumn::SetWidth(int width)
{
    if ( m_width == width && width != wxCOL_WIDTH_AUTOSIZE ) return;
    m_width = width;
    if ( m_owner ) m_owner->OnColumnWidthChange(m_owner->GetColumnIndex(this));
}
// A column left at a default width (80 pixels, 30 for a check box, whatever
// the scale of the screen) is given what its title needs: "Enabled" in 30
// pixels showed "Enabl". The width a program chose is not touched.
static int wxWinUIDataViewDefaultWidth(const wxDataViewColumn *column,
                                       wxDataViewCtrl *owner, int requested)
{
    const bool toggle = requested == wxDVC_TOGGLE_DEFAULT_WIDTH;
    if ( requested >= 0 && requested != wxDVC_DEFAULT_WIDTH && !toggle )
        return requested;

    int width = owner ? owner->FromDIP(toggle ? wxDVC_TOGGLE_DEFAULT_WIDTH
                                              : wxDVC_DEFAULT_WIDTH)
                      : requested < 0 ? wxDVC_DEFAULT_WIDTH : requested;
    if ( !owner )
        return width;

    // The title is measured once for each text: this is asked at every
    // layout.
    struct Measured { wxString title; int width; };
    static std::map<const wxDataViewColumn *, Measured> s_measured;
    const wxString title = column->GetTitle();
    Measured& measured = s_measured[column];
    if ( measured.title != title || !measured.width )
    {
        measured.title = title;
        const wxSize text = title.empty()
            ? wxSize() : wxWinUIMeasureText(owner, title, owner->GetFont());
        // The padding of a header cell and the room of its sort arrow.
        measured.width = text.x > 0 ? text.x + owner->FromDIP(12 + 8 + 12) : 0;
    }

    return std::max(width, measured.width);
}

int wxDataViewColumn::GetWidth() const
{
    const int minimum = m_minWidth;
    int width = std::max(minimum,
                         wxWinUIDataViewDefaultWidth(this, m_owner, m_width));
    const auto state = m_owner ? m_owner->m_winui : nullptr;
    const auto stamp = state ? state->generation : 0;
    if ( m_width == wxCOL_WIDTH_AUTOSIZE && m_owner )
        width = std::max(minimum, static_cast<int>(m_owner->GetBestColumnWidth(m_owner->GetColumnIndex(this))));
    if ( !state || !state->Alive(stamp) || state->ColumnIndex(this) == wxNOT_FOUND ) return width;
    const auto snapshot = state->displayColumns;
    auto last = std::find_if(snapshot.rbegin(), snapshot.rend(),
                            [](const auto col) { return !col->IsHidden(); });
    if ( last == snapshot.rend() || *last != this ) return width;
    int remaining = state->owner->GetClientSize().x;
    for ( const auto col : snapshot )
    {
        if ( col == this ) break;
        if ( !col->IsHidden() ) remaining -= col->GetWidth();
        if ( !state->Alive(stamp) ) return width;
    }
    return std::max(width, remaining);
}
void wxDataViewColumn::SetMinWidth(int width)
{
    width = std::max(0, width);
    if ( m_minWidth == width ) return;
    m_minWidth = width;
    if ( m_owner ) m_owner->OnColumnWidthChange(m_owner->GetColumnIndex(this));
}
void wxDataViewColumn::SetAlignment(wxAlignment align) { m_align = align; Changed(); }
void wxDataViewColumn::SetFlags(int flags) { m_flags = flags; Changed(); }
void wxDataViewColumn::SetBitmap(const wxBitmapBundle& bitmap)
{
    wxDataViewColumnBase::SetBitmap(bitmap);
    Changed();
}
void wxDataViewColumn::UnsetAsSortKey() { m_sort = false; Changed(); }
void wxDataViewColumn::SetSortOrder(bool ascending)
{
    if ( m_owner && !m_owner->IsMultiColumnSortAllowed() )
        for ( unsigned n = 0; n < m_owner->GetColumnCount(); ++n )
            m_owner->GetColumn(n)->m_sort = false;
    m_sort = true;
    m_ascending = ascending;
    Changed();
}
void wxDataViewColumn::WXCopyStateFrom(const wxDataViewColumn& other)
{
    m_title = other.m_title;
    m_bitmap = other.m_bitmap;
    m_width = other.m_width;
    m_minWidth = other.m_minWidth;
    m_align = other.m_align;
    m_flags = other.m_flags;
    m_sort = other.m_sort;
    m_ascending = other.m_ascending;
}

wxDataViewCtrl::wxDataViewCtrl()
    : m_winui(std::make_shared<wxWinUIDataViewImpl>(this)) {}

wxDataViewCtrl::wxDataViewCtrl(wxWindow* parent, wxWindowID id,
                               const wxPoint& pos, const wxSize& size, long style,
                               const wxValidator& validator, const wxString& name)
    : wxDataViewCtrl()
{
    Create(parent, id, pos, size, style, validator, name);
}

wxDataViewCtrl::~wxDataViewCtrl()
{
    auto state = m_winui;
    if ( GetModel() && state->notifier )
        GetModel()->RemoveNotifier(state->notifier);
    state->notifier = nullptr;
    state->Close();
    auto columns = std::move(state->columns);
    for ( auto col : columns )
    {
        col->SetOwner(nullptr);
        delete col;
    }
}

bool wxDataViewCtrl::Create(wxWindow* parent, wxWindowID id,
                           const wxPoint& pos, const wxSize& size, long style,
                           const wxValidator& validator, const wxString& name)
{
    // The frame is drawn by XAML, inside the window: Win32 keeps no border.
    m_winui->framed = (style & wxBORDER_MASK) != wxBORDER_NONE;
    if ( !wxControl::Create(parent, id, pos, size,
            (style & ~wxBORDER_MASK) | wxBORDER_NONE, validator, name) ) return false;
    try { return m_winui->Create(); }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("creating native DataView", e);
        return false;
    }
}

bool wxDataViewCtrl::AssociateModel(wxDataViewModel* model)
{
    auto state = m_winui;
    ModelPin old(GetModel()), next(model);
    if ( state->columnMutation || !PrepareForColumnMutation() ) return false;
    if ( !state->owner || GetModel() != old.model ) return false;
    if ( old.model && state->notifier ) old.model->RemoveNotifier(state->notifier);
    state->notifier = nullptr;
    ++state->generation;
    state->ResetAutomationRows();
    state->expanded.clear();
    state->selected.clear();
    state->rows.clear();
    state->current = 0;
    wxDataViewCtrlBase::AssociateModel(model);
    if ( model )
    {
        state->notifier = new DataViewNotifier(state);
        model->AddNotifier(state->notifier);
    }
    state->ModelChanged();
    return true;
}

bool wxDataViewCtrl::PrepareForColumnMutation()
{
    auto state = m_winui;
    if ( state->columnMutation || !state->EndEdit(false) || !state->owner ) return false;
    const auto snapshot = state->columns;
    for ( auto col : snapshot )
    {
        if ( state->ColumnIndex(col) == wxNOT_FOUND ) continue;
        auto renderer = col->GetRenderer();
        if ( renderer->GetEditorCtrl() && !renderer->FinishEditing() ) return false;
        if ( !state->owner ) return false;
    }
    return true;
}

bool wxDataViewCtrl::FinishNativeEditing(wxDataViewRendererBase* renderer,
                                         bool cancel, bool* handled)
{
    auto state = m_winui;
    *handled = state->nativeEditRenderer == renderer && state->editGeneration != 0;
    return *handled ? state->EndEdit(cancel) : true;
}

bool wxDataViewCtrl::AppendColumn(wxDataViewColumn* col)
{
    if ( !PrepareForColumnMutation() ) return false;
    return InsertColumn(GetColumnCount(), col);
}
bool wxDataViewCtrl::PrependColumn(wxDataViewColumn* col) { return InsertColumn(0, col); }
bool wxDataViewCtrl::InsertColumn(unsigned int pos, wxDataViewColumn* col)
{
    auto state = m_winui;
    if ( !col || !PrepareForColumnMutation() || !state->owner ||
         pos > state->columns.size() || state->ColumnIndex(col) != wxNOT_FOUND || col->GetOwner() ) return false;
    state->columnMutation = true;
    auto guard = wxMakeGuard([state] { state->columnMutation = false; });
    wxUnusedVar(guard);
    col->SetOwner(this);
    state->columns.insert(state->columns.begin() + pos, col);
    state->displayColumns.insert(state->displayColumns.begin() + pos, col);
    ++state->generation;
    state->widthCache.clear();
    state->Rebuild();
    return true;
}
unsigned int wxDataViewCtrl::GetColumnCount() const { return static_cast<unsigned>(m_winui->columns.size()); }
wxDataViewColumn* wxDataViewCtrl::GetColumn(unsigned int pos) const
{
    return pos < m_winui->columns.size() ? m_winui->columns[pos] : nullptr;
}
int wxDataViewCtrl::GetColumnIndex(const wxDataViewColumn* col) const { return m_winui->ColumnIndex(col); }
int wxDataViewCtrl::GetColumnPosition(const wxDataViewColumn* col) const { return m_winui->ColumnPosition(col); }
wxDataViewColumn* wxDataViewCtrl::GetColumnAt(unsigned int pos) const
{
    return pos < m_winui->displayColumns.size() ? m_winui->displayColumns[pos] : nullptr;
}
int wxDataViewCtrl::GetModelColumnIndex(unsigned int modelColumn) const
{
    for ( unsigned n = 0; n < GetColumnCount(); ++n )
        if ( GetColumn(n)->GetModelColumn() == modelColumn ) return static_cast<int>(n);
    return wxNOT_FOUND;
}
bool wxDataViewCtrl::DeleteColumn(wxDataViewColumn* col)
{
    return DeleteColumnForTreeList(col, nullptr, nullptr);
}
bool wxDataViewCtrl::DeleteColumnForTreeList(wxDataViewColumn* col,
                                            wxDataViewColumn* promoted,
                                            wxDataViewColumn* replacement)
{
    auto state = m_winui;
    if ( !PrepareForColumnMutation() || !state->owner ) return false;
    const int index = state->ColumnIndex(col);
    const int promotedIndex = state->ColumnIndex(promoted);
    if ( index == wxNOT_FOUND ||
        (replacement && (promotedIndex == wxNOT_FOUND || promoted == col || replacement->GetOwner())) ) return false;
    state->columnMutation = true;
    auto guard = wxMakeGuard([state] { state->columnMutation = false; });
    wxUnusedVar(guard);
    if ( replacement )
    {
        replacement->SetOwner(this);
        state->columns[promotedIndex] = replacement;
        state->displayColumns[state->ColumnPosition(promoted)] = replacement;
    }
    state->columns.erase(state->columns.begin() + index);
    state->displayColumns.erase(state->displayColumns.begin() + state->ColumnPosition(col));
    state->automationColumns.erase(col);
    state->headerWidths.erase(col);
    if ( replacement )
    {
        state->automationColumns.erase(promoted);
        state->headerWidths.erase(promoted);
    }
    for ( auto& row : state->automationCells )
    {
        row.second.erase(col);
        if ( replacement ) row.second.erase(promoted);
    }
    if ( GetExpanderColumn() == col || GetExpanderColumn() == promoted )
        WXSetExpanderColumnWithoutUpdate(replacement ? replacement :
            (state->columns.empty() ? nullptr : state->columns.front()));
    if ( state->currentColumn == col || state->currentColumn == promoted ) state->currentColumn = nullptr;
    ++state->generation;
    state->widthCache.clear();
    col->SetOwner(nullptr);
    if ( replacement ) promoted->SetOwner(nullptr);
    delete col;
    if ( replacement ) delete promoted;
    if ( state->owner ) state->Rebuild();
    return true;
}
bool wxDataViewCtrl::ClearColumns()
{
    auto state = m_winui;
    if ( !PrepareForColumnMutation() || !state->owner ) return false;
    state->columnMutation = true;
    auto guard = wxMakeGuard([state] { state->columnMutation = false; });
    wxUnusedVar(guard);
    auto columns = std::move(state->columns);
    state->displayColumns.clear();
    state->automationColumns.clear();
    state->automationCells.clear();
    state->headerWidths.clear();
    WXSetExpanderColumnWithoutUpdate(nullptr);
    state->currentColumn = nullptr;
    ++state->generation;
    state->widthCache.clear();
    for ( auto col : columns )
    {
        col->SetOwner(nullptr);
        delete col;
    }
    if ( state->owner ) state->Rebuild();
    return true;
}
wxVector<wxDataViewColumn*> wxDataViewCtrl::GetSortingColumns() const
{
    wxVector<wxDataViewColumn*> sorted;
    for ( auto col : m_winui->columns ) if ( col->IsSortKey() ) sorted.push_back(col);
    return sorted;
}
wxDataViewColumn* wxDataViewCtrl::GetSortingColumn() const
{
    for ( auto col : m_winui->columns ) if ( col->IsSortKey() ) return col;
    return nullptr;
}
bool wxDataViewCtrl::AllowMultiColumnSort(bool allow)
{
    m_winui->multiSort = allow;
    if ( !allow )
    {
        bool first = true;
        for ( auto col : m_winui->columns )
            if ( col->m_sort ) { col->m_sort = first; first = false; }
        m_winui->ModelChanged();
    }
    return true;
}
bool wxDataViewCtrl::IsMultiColumnSortAllowed() const { return m_winui->multiSort; }
void wxDataViewCtrl::ToggleSortByColumn(int column)
{
    if ( auto col = GetColumn(column) )
    {
        if ( col->IsSortKey() ) col->UnsetAsSortKey();
        else col->SetSortOrder(true);
    }
}
void wxDataViewCtrl::OnColumnChange(unsigned int WXUNUSED(index)) { m_winui->ModelChanged(); }
void wxDataViewCtrl::OnColumnWidthChange(unsigned int index)
{
    auto state = m_winui;
    if ( index < state->columns.size() ) state->GeometryChanged(state->columns[index]);
}
void wxDataViewCtrl::OnColumnResized() { m_winui->GeometryChanged(); }
void wxDataViewCtrl::OnColumnsCountChanged() { m_winui->ModelChanged(); }
void wxDataViewCtrl::ColumnMoved(wxDataViewColumn* col, unsigned int pos)
{
    m_winui->MoveColumn(col, pos, false);
}

wxDataViewColumn* wxDataViewCtrl::GetCurrentColumn() const { return m_winui->currentColumn; }
wxDataViewItem wxDataViewCtrl::DoGetCurrentItem() const { return Item(m_winui->current); }
void wxDataViewCtrl::DoSetCurrentItem(const wxDataViewItem& item) { m_winui->current = Key(item); }
int wxDataViewCtrl::GetSelectedItemsCount() const { return static_cast<int>(m_winui->selected.size()); }
int wxDataViewCtrl::GetSelections(wxDataViewItemArray& selections) const
{
    selections.clear();
    for ( const auto& row : m_winui->rows )
        if ( m_winui->selected.count(row.key) ) selections.Add(Item(row.key));
    return static_cast<int>(selections.size());
}
void wxDataViewCtrl::SetSelections(const wxDataViewItemArray& selections)
{
    auto state = m_winui;
    ModelPin model(GetModel());
    // Selecting a descendant has always expanded its ancestors in wx. Do
    // this before changing the selection, because expanding rebuilds rows.
    for ( const auto& item : selections )
    {
        if ( !item.IsOk() ) continue;
        ExpandAncestors(item);
        if ( !state->owner || state->owner->GetModel() != model.model ) return;
    }
    state->selected.clear();
    for ( const auto& item : selections )
    {
        if ( state->RowIndex(Key(item)) != wxNOT_FOUND )
        {
            state->selected.insert(Key(item));
            if ( !HasFlag(wxDV_MULTIPLE) ) break;
        }
    }
    state->RestoreSelection();
}
void wxDataViewCtrl::Select(const wxDataViewItem& item)
{
    auto state = m_winui;
    ModelPin model(GetModel());
    ExpandAncestors(item);
    if ( !state->owner || state->owner->GetModel() != model.model ||
         state->RowIndex(Key(item)) == wxNOT_FOUND ) return;
    if ( !HasFlag(wxDV_MULTIPLE) ) state->selected.clear();
    state->selected.insert(Key(item));
    state->RestoreSelection();
}
void wxDataViewCtrl::Unselect(const wxDataViewItem& item)
{
    m_winui->selected.erase(Key(item));
    m_winui->RestoreSelection();
}
bool wxDataViewCtrl::IsSelected(const wxDataViewItem& item) const { return m_winui->selected.count(Key(item)) != 0; }
void wxDataViewCtrl::SelectAll()
{
    m_winui->selected.clear();
    for ( const auto& row : m_winui->rows )
    {
        m_winui->selected.insert(row.key);
        if ( !HasFlag(wxDV_MULTIPLE) ) break;
    }
    m_winui->RestoreSelection();
}
void wxDataViewCtrl::UnselectAll() { m_winui->selected.clear(); m_winui->RestoreSelection(); }

void wxDataViewCtrl::DoExpand(const wxDataViewItem& item, bool children)
{
    auto state = m_winui;
    if ( !item.IsOk() || !GetModel() ) return;
    ModelPin model(GetModel());
    const auto stamp = state->generation;
    if ( !model.model->IsContainer(item) || !state->Alive(stamp) ) return;
    if ( !state->expanded.count(Key(item)) )
    {
        if ( !state->Send(wxEVT_DATAVIEW_ITEM_EXPANDING, Key(item)) ) return;
        state->expanded.insert(Key(item));
        state->VisibleStructureChanged(Key(item), true);
        if ( !state->owner || GetModel() != model.model ) return;
        state->Send(wxEVT_DATAVIEW_ITEM_EXPANDED, Key(item));
    }
    if ( children && state->owner && GetModel() == model.model )
    {
        wxDataViewItemArray nested;
        const auto childStamp = state->generation;
        model.model->GetChildren(item, nested);
        if ( !state->Alive(childStamp) ) return;
        for ( const auto& child : nested )
        {
            state->owner->DoExpand(child, true);
            if ( !state->owner || state->owner->GetModel() != model.model ) return;
        }
    }
}
void wxDataViewCtrl::Collapse(const wxDataViewItem& item)
{
    auto state = m_winui;
    if ( !state->expanded.count(Key(item)) ) return;
    if ( !state->Send(wxEVT_DATAVIEW_ITEM_COLLAPSING, Key(item)) ) return;
    state->expanded.erase(Key(item));
    state->VisibleStructureChanged(Key(item), false);
    if ( state->owner ) state->Send(wxEVT_DATAVIEW_ITEM_COLLAPSED, Key(item));
}
bool wxDataViewCtrl::IsExpanded(const wxDataViewItem& item) const { return m_winui->expanded.count(Key(item)) != 0; }
void wxDataViewCtrl::DoSetExpanderColumn() { m_winui->ModelChanged(); }
void wxDataViewCtrl::DoSetIndent() { m_winui->ModelChanged(); }
bool wxDataViewCtrl::SetRowHeight(int height)
{
    if ( height <= 0 ) return false;
    m_winui->rowHeight = height / m_winui->Scale();
    m_winui->RefreshVisuals();
    return true;
}
void wxDataViewCtrl::EnsureVisible(const wxDataViewItem& item, const wxDataViewColumn* col)
{
    auto state = m_winui;
    ExpandAncestors(item);
    if ( !state->SyncLayout() ) return;
    const auto stamp = state->generation;
    const int index = state->RowIndex(Key(item));
    if ( index == wxNOT_FOUND ) return;
    state->list.ScrollIntoView(state->source.GetAt(index));
    if ( !state->Alive(stamp) ) return;
    state->list.UpdateLayout();
    if ( !state->Alive(stamp) ) return;
    if ( col && state->scroll && state->ColumnIndex(col) != wxNOT_FOUND )
    {
        double left = 0;
        const auto columnSnapshot = state->displayColumns;
        for ( auto previous : columnSnapshot )
        {
            if ( !state->Alive(stamp) || state->ColumnIndex(previous) == wxNOT_FOUND ) return;
            if ( previous == col ) break;
            if ( !previous->IsHidden() ) left += state->ColumnWidth(previous);
            if ( !state->Alive(stamp) ) return;
        }
        if ( !state->Alive(stamp) || state->ColumnIndex(col) == wxNOT_FOUND ) return;
        state->scroll.ChangeView(left, nullptr, nullptr, true);
    }
    if ( state->Alive(stamp) ) state->list.UpdateLayout();
}
wxDataViewItem wxDataViewCtrl::GetTopItem() const
{
    const auto state = m_winui;
    if ( state->SyncLayout() )
    {
        auto panel = state->list.ItemsPanelRoot().try_as<C::ItemsStackPanel>();
        if ( panel && panel.FirstVisibleIndex() >= 0 &&
            panel.FirstVisibleIndex() < static_cast<int>(state->rows.size()) )
            return Item(state->rows[panel.FirstVisibleIndex()].key);
    }
    return wxDataViewItem();
}
int wxDataViewCtrl::GetCountPerPage() const
{
    return m_winui->list ? static_cast<int>(m_winui->list.ActualHeight() / m_winui->rowHeight) : 0;
}
wxRect wxDataViewCtrl::GetItemRect(const wxDataViewItem& item, const wxDataViewColumn* col) const
{
    auto state = m_winui;
    if ( !state->list || (col && (state->ColumnIndex(col) == wxNOT_FOUND || col->IsHidden())) ) return wxRect();
    if ( !state->SyncLayout() ) return wxRect();
    if ( col && state->ColumnIndex(col) == wxNOT_FOUND ) return wxRect();
    const auto stamp = state->generation;
    const int index = state->RowIndex(Key(item));
    if ( index == wxNOT_FOUND ) return wxRect();
    auto container = state->list.ContainerFromIndex(index).try_as<C::ListViewItem>();
    if ( !container ) return wxRect();
    const auto viewport = container.TransformToVisual(state->list).TransformPoint({0, 0});
    if ( viewport.Y + container.ActualHeight() <= 0 || viewport.Y >= state->list.ActualHeight() ) return wxRect();
    const auto origin = container.TransformToVisual(state->root).TransformPoint({0, 0});
    double left = 0;
    if ( col )
        for ( auto previous : state->displayColumns )
        {
            if ( previous == col ) break;
            if ( !previous->IsHidden() ) left += state->ColumnWidth(previous);
            if ( !state->Alive(stamp) ) return wxRect();
        }
    const double width = col ? state->ColumnWidth(col) : state->TotalWidth();
    if ( !state->Alive(stamp) ) return wxRect();
    const double scale = state->Scale();
    return wxRect(static_cast<int>(std::lround((origin.X + left) * scale)),
                  static_cast<int>(std::lround(origin.Y * scale)),
                  static_cast<int>(std::lround(width * scale)),
                  static_cast<int>(std::lround(state->rowHeight * scale)));
}
void wxDataViewCtrl::HitTest(const wxPoint& point, wxDataViewItem& item, wxDataViewColumn*& col) const
{
    item = wxDataViewItem();
    col = nullptr;
    auto state = m_winui;
    if ( !state->list ) return;
    auto panel = state->list.ItemsPanelRoot().try_as<C::ItemsStackPanel>();
    if ( !panel ) return;
    for ( int n = std::max(0, panel.FirstVisibleIndex());
          n <= panel.LastVisibleIndex() && n < static_cast<int>(state->rows.size()); ++n )
    {
        const auto candidate = Item(state->rows[n].key);
        if ( !GetItemRect(candidate).Contains(point) ) continue;
        item = candidate;
        for ( auto column : state->displayColumns )
            if ( !column->IsHidden() && GetItemRect(candidate, column).Contains(point) )
            { col = column; return; }
        return;
    }
}
void wxDataViewCtrl::EditItem(const wxDataViewItem& item, const wxDataViewColumn* col)
{
    m_winui->BeginEdit(Key(item), const_cast<wxDataViewColumn*>(col));
}
bool wxDataViewCtrl::SetFont(const wxFont& font)
{
    if ( !wxControl::SetFont(font) ) return false;
    if ( m_winui && m_winui->list ) { wxWinUIApplyFont(m_winui->list, font); m_winui->ModelChanged(); }
    return true;
}
bool wxDataViewCtrl::SetForegroundColour(const wxColour& colour)
{
    if ( !wxControl::SetForegroundColour(colour) ) return false;
    if ( m_winui && m_winui->list )
    {
        wxWinUIApplyForeground(m_winui->list, colour);
        ++m_winui->generation;
        m_winui->RefreshVisuals();
    }
    return true;
}
bool wxDataViewCtrl::SetBackgroundColour(const wxColour& colour)
{
    if ( !wxControl::SetBackgroundColour(colour) ) return false;
    if ( m_winui && m_winui->list )
    {
        m_winui->list.Background(wxWinUICreateColourBrush(colour));
        ++m_winui->generation;
        m_winui->RefreshVisuals();
    }
    return true;
}
void wxDataViewCtrl::SetFocus()
{
    const auto state = m_winui;
    wxControl::SetFocus();
    if ( state->owner && state->list )
    {
        state->host.SetPreferredFocus(state->list);
        state->host.SynchronizeForFocus();
        if ( !state->owner ) return;
        state->list.Focus(wxWinUIProgrammaticFocusState());
        if ( state->owner ) state->RefreshVisuals();
    }
}
wxSize wxDataViewCtrl::DoGetBestSize() const { return FromDIP(wxSize(320, 200)); }
unsigned int wxDataViewCtrl::GetBestColumnWidth(int index) const
{
    unsigned int width = FromDIP(wxDVC_DEFAULT_WIDTH);
    TryGetBestColumnWidth(index, &width);
    return width;
}
bool wxDataViewCtrl::TryGetBestColumnWidth(int index, unsigned int* width) const
{
    auto state = m_winui;
    auto col = GetColumn(index);
    if ( !col || !width ) return false;
    const auto found = state->widthCache.find(col);
    if ( found != state->widthCache.end() ) { *width = found->second; return true; }
    const auto stamp = state->generation;
    int best = wxWinUIMeasureText(this, col->GetTitle(), GetFont()).x + FromDIP(32);
    ModelPin model(const_cast<wxDataViewModel*>(GetModel()));
    const auto rowSnapshot = state->rows;
    auto renderer = col->GetRenderer();
    const auto modelColumn = col->GetModelColumn();
    // The expander column of a tree also holds each row's indentation and its
    // expander button (the same amounts the row presentation uses).
    const bool tree = model.model && !model.model->IsListModel() &&
        (col == GetExpanderColumn() || (!GetExpanderColumn() && col == GetColumn(0)));
    const int indentStep = std::max(16, GetIndent());
    // The renderer measures the check box of classic Windows, 13 wide; the
    // one shown is the CheckBox of WinUI, 20 and a gap of 8.
    const int checkExtra =
        dynamic_cast<wxDataViewCheckIconTextRenderer *>(renderer) ? FromDIP(16) : 0;
    if ( model.model )
        for ( const auto& row : rowSnapshot )
        {
            const bool prepared = renderer->PrepareForItem(model.model, Item(row.key), modelColumn);
            if ( !state->Alive(stamp) ) return false;
            if ( prepared )
            {
                const auto size = renderer->GetSize();
                if ( !state->Alive(stamp) ) return false;
                const int indent = tree
                    ? static_cast<int>(row.depth) * indentStep + FromDIP(24) : 0;
                best = std::max(best, size.x + FromDIP(24) + checkExtra + indent);
            }
        }
    *width = static_cast<unsigned>(std::max(best, col->GetMinWidth()));
    state->widthCache[col] = *width;
    return true;
}

#endif // wxHAS_WINUI_DATAVIEWCTRL
#endif // wxUSE_DATAVIEWCTRL
