/////////////////////////////////////////////////////////////////////////////
// Name:        src/winui/gridprojection.cpp
// Purpose:     Native cells and headers of the authoritative public wxGrid
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if defined(__WXWINUI__) && wxUSE_WINUI3 && wxUSE_GRID

#include "gridprojection.h"
#include "gridautomation.h"
#include "wx/grid.h"
#include "wx/generic/private/grid.h"
#include "wx/generic/gridctrl.h"
#include "wx/generic/grideditors.h"
#include "wx/dcmemory.h"
#include "wx/log.h"
#include "wx/numformatter.h"
#include "wx/scopeguard.h"
#include "wx/weakref.h"
#include "wx/textctrl.h"
#if wxUSE_SPINCTRL
#include "wx/spinctrl.h"
#endif
#include "wx/winui/private/appearance.h"
#include "wx/winui/private/tlwhost.h"
#include "private.h"
#include <winrt/Microsoft.UI.Xaml.Automation.h>
#include <algorithm>
#include <map>
#include <set>
#include <utility>
#ifndef wxNO_RTTI
#include <typeinfo>
#endif

namespace X = winrt::Microsoft::UI::Xaml;
namespace C = X::Controls;
namespace F = winrt::Windows::Foundation;
namespace U = wxWinUIGridAutomation;

namespace
{
enum class GridPaneKind { Cells, Rows, Columns, Corner };
enum class GridCellKind { Text, Number, Float, Bool, WrappedText, Date, Enum, Custom };
using CellKey = std::pair<int, int>;
struct InvalidGridSnapshot {};

GridCellKind ClassifyRenderer(const wxGridCellRenderer& renderer)
{
#ifndef wxNO_RTTI
    // Subclasses can override Draw(): never replace their explicit contract
    // merely because they derive from one of the standard renderers.
    if ( typeid(renderer) == typeid(wxGridCellStringRenderer) ) return GridCellKind::Text;
    if ( typeid(renderer) == typeid(wxGridCellNumberRenderer) ) return GridCellKind::Number;
    if ( typeid(renderer) == typeid(wxGridCellFloatRenderer) ) return GridCellKind::Float;
    if ( typeid(renderer) == typeid(wxGridCellBoolRenderer) ) return GridCellKind::Bool;
    if ( typeid(renderer) == typeid(wxGridCellAutoWrapStringRenderer) ) return GridCellKind::WrappedText;
    if ( typeid(renderer) == typeid(wxGridCellChoiceRenderer) ) return GridCellKind::Text;
    if ( typeid(renderer) == typeid(wxGridCellEnumRenderer) ) return GridCellKind::Enum;
#if wxUSE_DATETIME
    if ( typeid(renderer) == typeid(wxGridCellDateRenderer) ||
         typeid(renderer) == typeid(wxGridCellDateTimeRenderer) ) return GridCellKind::Date;
#endif
#else
    wxUnusedVar(renderer);
#endif
    // Without RTTI there is no safe distinction from application subclasses.
    // Preserve their drawing contract instead of assuming a standard type.
    return GridCellKind::Custom;
}

bool HasTextValueEditor(const wxGridCellEditor& editor)
{
#if wxUSE_TEXTCTRL && !defined(wxNO_RTTI)
    return typeid(editor) == typeid(wxGridCellTextEditor) ||
           typeid(editor) == typeid(wxGridCellNumberEditor) ||
           typeid(editor) == typeid(wxGridCellFloatEditor) ||
           typeid(editor) == typeid(wxGridCellAutoWrapStringEditor);
#else
    wxUnusedVar(editor);
    return false;
#endif
}

X::HorizontalAlignment HorizontalAlignment(int alignment)
{
    if ( alignment == wxALIGN_RIGHT ) return X::HorizontalAlignment::Right;
    if ( alignment == wxALIGN_CENTRE ) return X::HorizontalAlignment::Center;
    return X::HorizontalAlignment::Left;
}

X::VerticalAlignment VerticalAlignment(int alignment)
{
    if ( alignment == wxALIGN_BOTTOM ) return X::VerticalAlignment::Bottom;
    if ( alignment == wxALIGN_CENTRE ) return X::VerticalAlignment::Center;
    return X::VerticalAlignment::Top;
}
}

struct wxWinUIGridProjection::Impl : std::enable_shared_from_this<Impl>
{
    struct Peer
    {
        CellKey key;
        GridCellKind kind = GridCellKind::Text;
        C::Control control{nullptr};
        C::TextBlock text{nullptr};
        C::Image image{nullptr};
        wxGridTableBase* table = nullptr;
        unsigned long long rows = 0;
        unsigned long long columns = 0;
        wxRect rect;
    };

    struct Pane
    {
        wxWeakRef<wxWindow> window;
        wxWeakRef<wxWindow> gridWindow;
        GridPaneKind kind = GridPaneKind::Cells;
        wxWinUIControlHost host;
        C::Grid root{nullptr};
        C::Canvas canvas{nullptr};
        std::map<CellKey, std::shared_ptr<Peer>> peers;
        unsigned long long inputSerial = 0;
        bool inputActive = false;
        bool published = false;
    };

    struct Cell
    {
        CellKey key;
        GridCellKind kind = GridCellKind::Text;
        wxRect rect;
        wxString text;
        wxColour background;
        wxColour foreground;
        wxFont font;
        wxBitmap bitmap;
        int horizontal = wxALIGN_LEFT;
        int vertical = wxALIGN_TOP;
        bool checked = false;
        bool enabled = true;
        bool current = false;
    };

    struct Snapshot
    {
        wxGridTableBase* table = nullptr;
        unsigned long long rows = 0;
        unsigned long long columns = 0;
        unsigned long long revision = 0;
    };

    explicit Impl(wxGrid* grid) : owner(grid) {}
    wxWeakRef<wxGrid> owner;
    std::vector<std::shared_ptr<Pane>> panes;
    unsigned long long revision = 0;
    unsigned suppressNative = 0;
    bool scheduled = false;
    bool synchronizing = false;
    bool dirty = true;
    bool closed = false;
    Snapshot automationSchema{};
    bool hasAutomationSchema = false;
    bool settingAutomationValue = false;
    std::map<CellKey, winrt::weak_ref<U::A::AutomationPeer>> automationItems;

    bool AutomationCurrent() const
    {
        return !closed && owner && !owner->IsBeingDeleted() && owner->CanUpdateWinUIProjection();
    }
    void AutomationCheck(const Snapshot& snapshot, CellKey key) const;
    wxGridCellAttrPtr AutomationAttr(const Snapshot& snapshot, CellKey key) const;
    CellKey AutomationAnchor(const Snapshot& snapshot, CellKey key) const;
    U::ItemBinding::Position AutomationPosition(const Snapshot& snapshot, CellKey key) const;
    std::shared_ptr<Pane> AutomationMainPane() const;
    std::pair<std::shared_ptr<Pane>, std::shared_ptr<Peer>> AutomationVisual(CellKey key) const;
    U::A::AutomationPeer AutomationGrid();
    U::A::AutomationPeer AutomationItem(CellKey key);
    std::shared_ptr<U::GridBinding> AutomationBinding();
    F::Rect AutomationBounds(const Snapshot& snapshot, CellKey key) const;
    winrt::hstring AutomationName(const Snapshot& snapshot, CellKey key);
    bool AutomationToggle(const Snapshot& snapshot, CellKey key) const;
    bool AutomationEnabled(const Snapshot& snapshot, CellKey key) const;
    wxGridCellEditorPtr AutomationValueEditor(const Snapshot& snapshot, CellKey key) const;
    bool AutomationReadOnly(const Snapshot& snapshot, CellKey key) const;
    void AutomationSetValue(const Snapshot& snapshot, CellKey key, const winrt::hstring& value);
    void AutomationRealize(const Snapshot& snapshot, CellKey key);
    std::vector<U::A::AutomationPeer> AutomationHeaders(bool rows);

    Snapshot Capture() const
    {
        return {owner->GetTable(), owner->GetRowMutationRevision(),
                owner->GetColumnMutationRevision(), revision};
    }

    bool IdentityCurrent(const Snapshot& snapshot) const
    {
        return !closed && owner && !owner->IsBeingDeleted() &&
               owner->CanUpdateWinUIProjection() &&
               owner->GetTable() == snapshot.table &&
               owner->GetRowMutationRevision() == snapshot.rows &&
               owner->GetColumnMutationRevision() == snapshot.columns;
    }

    bool Current(const Snapshot& snapshot) const
    {
        return IdentityCurrent(snapshot) && revision == snapshot.revision;
    }

    void Check(const Snapshot& snapshot) const
    {
        if ( !Current(snapshot) ) throw InvalidGridSnapshot{};
    }

    void Invalidate()
    {
        if ( closed || !owner ) return;
        ++revision;
        dirty = true;
        if ( scheduled || synchronizing || !owner->CanUpdateWinUIProjection() ) return;
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

    bool IsPeerCurrent(const std::shared_ptr<Pane>& pane,
                       const std::shared_ptr<Peer>& peer) const
    {
        if ( closed || !owner || owner->IsBeingDeleted() || !owner->CanUpdateWinUIProjection() || !pane->window ||
             !pane->published || owner->GetTable() != peer->table ||
             owner->GetRowMutationRevision() != peer->rows ||
             owner->GetColumnMutationRevision() != peer->columns ) return false;
        const auto found = pane->peers.find(peer->key);
        return found != pane->peers.end() && found->second == peer;
    }

    void Activate(const std::shared_ptr<Pane>& pane,
                  const std::shared_ptr<Peer>& peer)
    {
        if ( suppressNative || pane->inputActive || !IsPeerCurrent(pane, peer) ) return;
        const auto snapshot = Capture();
        const int row = peer->key.first, col = peer->key.second;
        wxMouseEvent click(wxEVT_LEFT_DOWN);
        click.SetEventObject(pane->window.get());
        click.SetPosition(peer->rect.GetPosition() +
                          wxPoint(peer->rect.width / 2, peer->rect.height / 2));

        // Physical input already reaches the existing wxGrid window handlers
        // through the host's handledEventsToo routing. This is the equivalent
        // semantic entry for native UIA Invoke/Toggle, not a second mouse path.
        try
        {
            if ( pane->kind != GridPaneKind::Cells )
            {
                const auto result = owner->SendEvent(wxEVT_GRID_LABEL_LEFT_CLICK,
                                                      row, col, click);
                if ( !IdentityCurrent(snapshot) ) return;
                if ( result == wxGrid::Event_Unhandled )
                {
                    if ( row >= 0 ) owner->SelectRow(row);
                    else if ( col >= 0 ) owner->SelectCol(col);
                    else owner->SelectAll();
                }
            }
            else
            {
                const auto result = owner->SendEvent(wxEVT_GRID_CELL_LEFT_CLICK,
                                                      row, col, click);
                if ( !IdentityCurrent(snapshot) ) return;
                if ( result == wxGrid::Event_Unhandled &&
                     owner->SetCurrentCell(row, col) )
                {
                    // SetCurrentCell refreshes selection and may therefore
                    // advance the presentation revision. Model identity, not
                    // this coalesced repaint ticket, governs the activation.
                    if ( !IsPeerCurrent(pane, peer) ) return;
                    owner->ClearSelection();
                    if ( !IsPeerCurrent(pane, peer) ) return;
                    if ( peer->kind == GridCellKind::Bool && owner->CanEnableCellControl() )
                        owner->DoEnableCellEditControl(wxGridActivationSource::From(click));
                }
            }
        }
        catch ( const InvalidGridSnapshot& ) {}
        if ( owner && !closed ) Invalidate(); // Includes native toggle veto.
    }

    void AddInputObservation(const std::shared_ptr<Pane>& pane)
    {
        const std::weak_ptr<Impl> weak = shared_from_this();
        const std::weak_ptr<Pane> weakPane = pane;
        const auto begin = [weakPane](const auto&, const auto&)
        {
            if ( auto live = weakPane.lock() )
            {
                ++live->inputSerial;
                live->inputActive = true;
            }
        };
        const auto end = [weak, weakPane](const auto&, const auto&)
        {
            auto self = weak.lock();
            auto live = weakPane.lock();
            if ( !self || !self->owner || !live ) return;
            const auto serial = live->inputSerial;
            self->owner->CallAfter([weak, weakPane, serial]()
            {
                auto state = weak.lock();
                auto current = weakPane.lock();
                if ( !state || !current || current->inputSerial != serial ) return;
                current->inputActive = false;
                state->Invalidate();
            });
        };
        pane->canvas.AddHandler(X::UIElement::PointerPressedEvent(),
            winrt::box_value(X::Input::PointerEventHandler(begin)), true);
        pane->canvas.AddHandler(X::UIElement::PointerReleasedEvent(),
            winrt::box_value(X::Input::PointerEventHandler(end)), true);
        pane->canvas.AddHandler(X::UIElement::PointerCanceledEvent(),
            winrt::box_value(X::Input::PointerEventHandler(end)), true);
        pane->canvas.AddHandler(X::UIElement::PointerCaptureLostEvent(),
            winrt::box_value(X::Input::PointerEventHandler(end)), true);
        pane->canvas.AddHandler(X::UIElement::KeyDownEvent(),
            winrt::box_value(X::Input::KeyEventHandler(begin)), true);
        pane->canvas.AddHandler(X::UIElement::KeyUpEvent(),
            winrt::box_value(X::Input::KeyEventHandler(end)), true);
    }

    void ClosePane(const std::shared_ptr<Pane>& pane)
    {
        pane->published = false;
        pane->peers.clear();
        pane->host.Close();
    }

    void UpdatePanes(const Snapshot& snapshot)
    {
        struct Wanted { wxWindow* window; wxGridWindow* body; GridPaneKind kind; };
        const Wanted wanted[] = {
            {owner->m_gridWin, owner->m_gridWin, GridPaneKind::Cells},
            {owner->m_frozenRowGridWin, owner->m_frozenRowGridWin, GridPaneKind::Cells},
            {owner->m_frozenColGridWin, owner->m_frozenColGridWin, GridPaneKind::Cells},
            {owner->m_frozenCornerGridWin, owner->m_frozenCornerGridWin, GridPaneKind::Cells},
            {owner->m_rowLabelWin, owner->m_gridWin, GridPaneKind::Rows},
            {owner->m_rowFrozenLabelWin, owner->m_frozenRowGridWin, GridPaneKind::Rows},
            // A requested wxHeaderCtrl remains that public native control.
            {owner->m_useNativeHeader ? nullptr : owner->m_colLabelWin,
                owner->m_gridWin, GridPaneKind::Columns},
            {owner->m_colFrozenLabelWin, owner->m_frozenColGridWin, GridPaneKind::Columns},
            {owner->m_cornerLabelWin, owner->m_gridWin, GridPaneKind::Corner}
        };
        for ( auto it = panes.begin(); it != panes.end(); )
        {
            const auto& pane = *it;
            const auto match = std::find_if(std::begin(wanted), std::end(wanted),
                [&](const Wanted& item) { return pane->window && item.window == pane->window.get(); });
            if ( match == std::end(wanted) )
            {
                const auto retired = pane;
                it = panes.erase(it);
                ClosePane(retired);
                Check(snapshot);
            }
            else
            {
                pane->gridWindow = match->body;
                ++it;
            }
        }
        for ( const auto& item : wanted )
        {
            if ( !item.window || !item.body ) continue;
            const auto found = std::find_if(panes.begin(), panes.end(),
                [&](const auto& pane) { return pane->window.get() == item.window; });
            if ( found != panes.end() ) continue;
            auto pane = std::make_shared<Pane>();
            pane->window = item.window;
            pane->gridWindow = item.body;
            pane->kind = item.kind;
            const bool initialized = pane->host.Initialize(item.window);
            Check(snapshot);
            if ( !initialized ) continue;
            pane->root = item.window == owner->m_gridWin
                ? winrt::make<U::GridElement>(AutomationBinding())
                : winrt::make<U::PartElement>();
            pane->canvas = C::Canvas();
            // Canvas coordinates below are wx's logical-leading coordinates;
            // inheriting the slot direction mirrors the same coordinates once.
            X::Automation::AutomationProperties::SetAutomationId(
                pane->root, L"wxGrid.NativeViewport");
            pane->root.Children().Append(pane->canvas);
            AddInputObservation(pane);
            const std::weak_ptr<Impl> weak = shared_from_this();
            const std::weak_ptr<Pane> weakPane = pane;
            item.window->Bind(wxEVT_DESTROY, [weak, weakPane](wxWindowDestroyEvent& event)
            {
                if ( auto self = weak.lock() )
                    if ( auto live = weakPane.lock(); live && event.GetEventObject() == live->window.get() )
                        self->ClosePane(live);
                event.Skip();
            });
            const bool published = pane->host.SetContent(pane->root);
            Check(snapshot);
            if ( !published )
            {
                pane->host.Close();
                Check(snapshot);
                continue;
            }
            pane->published = true;
            panes.push_back(std::move(pane));
        }
    }

    wxString ReadText(const Snapshot& snapshot, int row, int col,
                      GridCellKind kind, wxGridCellRenderer& renderer)
    {
        auto* table = snapshot.table;
        wxString text;
        if ( kind == GridCellKind::Number )
        {
            const bool typed = table->CanGetValueAs(row, col, wxGRID_VALUE_NUMBER);
            Check(snapshot);
            if ( typed )
            {
                const long number = table->GetValueAsLong(row, col);
                Check(snapshot);
                return wxString::Format("%ld", number);
            }
        }
        if ( kind == GridCellKind::Float )
        {
            const bool typed = table->CanGetValueAs(row, col, wxGRID_VALUE_FLOAT);
            Check(snapshot);
            double value = 0;
            bool hasValue = typed;
            if ( typed ) value = table->GetValueAsDouble(row, col);
            else
            {
                text = table->GetValue(row, col);
                Check(snapshot);
                hasValue = wxNumberFormatter::FromString(text, &value);
            }
            Check(snapshot);
            if ( hasValue )
            {
                const auto& numeric = static_cast<const wxGridCellFloatRenderer&>(renderer);
                wxString format("%");
                if ( numeric.GetWidth() != -1 ) format += wxString::Format("%d", numeric.GetWidth());
                if ( numeric.GetPrecision() != -1 ) format += wxString::Format(".%d", numeric.GetPrecision());
                else if ( numeric.GetWidth() != -1 ) format += ".";
                const int style = numeric.GetFormat();
                const bool upper = (style & wxGRID_FLOAT_FORMAT_UPPER) == wxGRID_FLOAT_FORMAT_UPPER;
                format += (style & wxGRID_FLOAT_FORMAT_SCIENTIFIC) == wxGRID_FLOAT_FORMAT_SCIENTIFIC
                    ? (upper ? "E" : "e")
                    : (style & wxGRID_FLOAT_FORMAT_COMPACT) == wxGRID_FLOAT_FORMAT_COMPACT
                        ? (upper ? "G" : "g") : "f";
                return wxNumberFormatter::Format(format, value);
            }
            return text;
        }
        if ( kind == GridCellKind::Enum )
        {
            const bool typed = table->CanGetValueAs(row, col, wxGRID_VALUE_NUMBER);
            Check(snapshot);
            if ( typed )
            {
                const long choice = table->GetValueAsLong(row, col);
                Check(snapshot);
                const auto& enumeration = static_cast<const wxGridCellEnumRenderer&>(renderer);
                // A valid typed enum indexes the renderer's actual choices.
                // An invalid model value must not become an unchecked read.
                if ( choice < 0 || static_cast<unsigned long>(choice) >= enumeration.m_choices.size() )
                {
                    wxLogDebug("Invalid wxGrid enum choice index %ld", choice);
                    return wxString();
                }
                return enumeration.m_choices[static_cast<size_t>(choice)];
            }
            // The generic Enum renderer deliberately leaves string values
            // untouched: it does not parse a string as an integer choice.
        }
#if wxUSE_DATETIME
        if ( kind == GridCellKind::Date )
        {
            const auto& dateRenderer = static_cast<const wxGridCellDateRenderer&>(renderer);
            wxGridPrivate::DateParseParams params;
            dateRenderer.GetDateParseParams(params);
            Check(snapshot);
            const wxString format = dateRenderer.m_oformat;
            const auto timeZone = dateRenderer.m_tz;
            wxDateTime value;
            bool hasValue = false;
            const bool typed = table->CanGetValueAs(row, col, wxGRID_VALUE_DATETIME);
            Check(snapshot);
            if ( typed )
            {
                // GetValueAsCustom transfers this date's ownership, just as
                // TryGetValueAsDate does. Own it before the callback check so
                // a reentrant table replacement/destruction cannot leak it.
                const std::unique_ptr<wxDateTime> custom(static_cast<wxDateTime*>(
                    table->GetValueAsCustom(row, col, wxGRID_VALUE_DATETIME)));
                Check(snapshot);
                if ( custom )
                {
                    value = *custom;
                    hasValue = true;
                }
            }
            if ( !hasValue )
            {
                text = table->GetValue(row, col);
                Check(snapshot);
                hasValue = wxGridPrivate::TryParseDate(value, params, text);
            }
            return hasValue ? value.Format(format, timeZone) : wxString();
        }
#endif
        text = table->GetValue(row, col);
        Check(snapshot);
        return text;
    }

    Cell ReadCell(const std::shared_ptr<Pane>& pane, CellKey key,
                  const Snapshot& snapshot)
    {
        Cell cell;
        cell.key = key;
        const int row = key.first, col = key.second;
        auto* body = static_cast<wxGridWindow*>(pane->gridWindow.get());
        if ( pane->kind == GridPaneKind::Cells )
        {
            auto attr = owner->GetCellAttrPtr(row, col);
            Check(snapshot);
            auto renderer = attr->GetRendererPtr(owner.get(), row, col);
            Check(snapshot);
            cell.kind = ClassifyRenderer(*renderer);
            cell.rect = owner->CellToRect(row, col);
            const wxRect logical = cell.rect;
            cell.rect.Offset(-owner->GetGridWindowOffset(body));
            owner->CalcGridWindowScrolledPosition(cell.rect.x, cell.rect.y,
                &cell.rect.x, &cell.rect.y, body);
            const bool selected = owner->IsInSelection(row, col);
            cell.current = owner->GetGridCursorCoords() == wxGridCellCoords(row, col);
            cell.background = selected ? owner->GetSelectionBackground() : attr->GetBackgroundColour();
            cell.foreground = selected ? owner->GetSelectionForeground() : attr->GetTextColour();
            cell.font = attr->GetFont();
            cell.horizontal = cell.kind == GridCellKind::Number || cell.kind == GridCellKind::Float ||
                cell.kind == GridCellKind::Date || cell.kind == GridCellKind::Enum
                ? wxALIGN_RIGHT : wxALIGN_LEFT;
            cell.vertical = wxALIGN_TOP;
            attr->GetNonDefaultAlignment(&cell.horizontal, &cell.vertical);
            cell.enabled = owner->IsEnabled() &&
                (cell.kind != GridCellKind::Bool || (owner->IsEditable() && !attr->IsReadOnly()));
            if ( cell.kind == GridCellKind::Bool )
            {
                const bool typed = snapshot.table->CanGetValueAs(row, col, wxGRID_VALUE_BOOL);
                Check(snapshot);
                if ( typed ) cell.checked = snapshot.table->GetValueAsBool(row, col);
                else cell.checked = wxGridCellBoolEditor::IsTrueValue(snapshot.table->GetValue(row, col));
                Check(snapshot);
                cell.text = cell.checked ? "1" : "";
            }
            else if ( cell.kind == GridCellKind::Custom )
            {
                if ( logical.width > 0 && logical.height > 0 )
                {
                    cell.bitmap = wxBitmap(logical.width, logical.height, 24);
                    wxMemoryDC dc(cell.bitmap);
                    dc.SetBackground(wxBrush(cell.background));
                    dc.Clear();
                    dc.SetDeviceOrigin(-logical.x, -logical.y);
                    renderer->Draw(*owner, *attr, dc, logical, row, col, selected);
                    Check(snapshot);
                }
            }
            else cell.text = ReadText(snapshot, row, col, cell.kind, *renderer);
        }
        else
        {
            cell.background = owner->GetLabelBackgroundColour();
            cell.foreground = owner->GetLabelTextColour();
            cell.font = owner->GetLabelFont();
            cell.enabled = owner->IsEnabled();
            if ( pane->kind == GridPaneKind::Rows )
            {
                cell.text = owner->GetRowLabelValue(row);
                Check(snapshot);
                cell.rect = wxRect(0, owner->GetRowTop(row),
                    pane->window->GetClientSize().x, owner->GetRowSize(row));
                cell.rect.y -= owner->GetGridWindowOffset(body).y;
                owner->CalcGridWindowScrolledPosition(0, cell.rect.y, nullptr, &cell.rect.y, body);
                owner->GetRowLabelAlignment(&cell.horizontal, &cell.vertical);
            }
            else if ( pane->kind == GridPaneKind::Columns )
            {
                cell.text = owner->GetColLabelValue(col);
                Check(snapshot);
                cell.rect = wxRect(owner->GetColLeft(col), 0,
                    owner->GetColSize(col), pane->window->GetClientSize().y);
                cell.rect.x -= owner->GetGridWindowOffset(body).x;
                owner->CalcGridWindowScrolledPosition(cell.rect.x, 0, &cell.rect.x, nullptr, body);
                owner->GetColLabelAlignment(&cell.horizontal, &cell.vertical);
            }
            else
            {
                cell.text = owner->GetCornerLabelValue();
                Check(snapshot);
                cell.rect = pane->window->GetClientRect();
                owner->GetCornerLabelAlignment(&cell.horizontal, &cell.vertical);
            }
        }
        return cell;
    }

    std::vector<Cell> ReadPane(const std::shared_ptr<Pane>& pane, const Snapshot& snapshot)
    {
        std::vector<Cell> cells;
        if ( !pane->window || !pane->gridWindow || !pane->window->IsShown() ) return cells;
        const auto size = pane->window->GetClientSize();
        if ( size.x <= 0 || size.y <= 0 ) return cells;
        auto* body = static_cast<wxGridWindow*>(pane->gridWindow.get());
        const wxRegion exposed(0, 0, size.x, size.y);
        std::set<CellKey> keys;
        if ( pane->kind == GridPaneKind::Cells && snapshot.table )
        {
            // The generic paint path establishes the initial current cell in
            // DrawHighlight(), not in SetTable(). Preserve that silent first
            // presentation transition: keyboard movement needs a current cell,
            // but merely attaching a table must not send SELECT_CELL or select
            // a cell in a grid which has not been presented yet.
            if ( pane->window->IsShownOnScreen() &&
                 owner->m_currentCellCoords == wxGridNoCellCoords &&
                 owner->m_numRows && owner->m_numCols )
            {
                owner->m_currentCellCoords.Set(0, 0);
            }
            for ( const auto& coords : owner->CalcCellsExposed(exposed, body) )
            {
                int row = coords.GetRow(), col = coords.GetCol(), rows = 1, cols = 1;
                if ( row < 0 || col < 0 || row >= owner->GetNumberRows() || col >= owner->GetNumberCols() ) continue;
                if ( owner->GetCellSize(row, col, &rows, &cols) == wxGrid::CellSpan_Inside )
                {
                    row += rows;
                    col += cols;
                }
                Check(snapshot);
                keys.emplace(row, col);
            }
        }
        else if ( pane->kind == GridPaneKind::Rows && snapshot.table )
        {
            for ( int row : owner->CalcRowLabelsExposed(exposed, body) ) keys.emplace(row, -1);
        }
        else if ( pane->kind == GridPaneKind::Columns && snapshot.table )
        {
            for ( int col : owner->CalcColLabelsExposed(exposed, body) ) keys.emplace(-1, col);
        }
        else if ( pane->kind == GridPaneKind::Corner ) keys.emplace(-1, -1);
        for ( const auto& key : keys )
        {
            auto cell = ReadCell(pane, key, snapshot);
            Check(snapshot);
            if ( !cell.rect.IsEmpty() && cell.rect.Intersects(wxRect(size)) ) cells.push_back(std::move(cell));
        }
        return cells;
    }

    std::shared_ptr<Peer> CreatePeer(const std::shared_ptr<Pane>& pane, const Cell& cell)
    {
        auto peer = std::make_shared<Peer>();
        peer->key = cell.key;
        peer->kind = cell.kind;
        const std::weak_ptr<Impl> weak = shared_from_this();
        const std::weak_ptr<Pane> weakPane = pane;
        const std::weak_ptr<Peer> weakPeer = peer;
        const auto automation = [weak, weakPane, weakPeer]() -> U::A::AutomationPeer
        {
            const auto self = weak.lock(); const auto currentPane = weakPane.lock();
            const auto currentPeer = weakPeer.lock();
            if ( !self || !currentPane || !currentPeer ||
                 !self->IdentityCurrent({currentPeer->table, currentPeer->rows,
                                        currentPeer->columns, self->revision}) ) U::Unavailable();
            return self->AutomationItem(currentPeer->key);
        };
        const auto activate = [weak, weakPane, weakPeer](const auto&, const auto&)
        {
            auto self = weak.lock();
            auto currentPane = weakPane.lock();
            auto currentPeer = weakPeer.lock();
            if ( self && currentPane && currentPeer ) self->Activate(currentPane, currentPeer);
        };
        if ( cell.kind == GridCellKind::Bool )
        {
            C::CheckBox check = winrt::make<U::ToggleElement>(automation);
            check.Checked(activate);
            check.Unchecked(activate);
            check.HorizontalContentAlignment(X::HorizontalAlignment::Center);
            check.VerticalContentAlignment(X::VerticalAlignment::Center);
            peer->control = check;
        }
        else
        {
            C::Button button = winrt::make<U::ButtonElement>(automation);
            button.Click(activate);
            button.HorizontalContentAlignment(X::HorizontalAlignment::Stretch);
            button.VerticalContentAlignment(X::VerticalAlignment::Stretch);
            if ( cell.kind == GridCellKind::Custom )
            {
                peer->image = C::Image();
                peer->image.Stretch(X::Media::Stretch::Fill);
                button.Content(peer->image);
            }
            else
            {
                peer->text = C::TextBlock();
                peer->text.Margin(X::Thickness{2, 1, 2, 1});
                button.Content(peer->text);
            }
            peer->control = button;
        }
        peer->control.MinWidth(0);
        peer->control.MinHeight(0);
        peer->control.Padding(X::Thickness{0, 0, 0, 0});
        X::Automation::AutomationProperties::SetAutomationId(peer->control,
            wxWinUIToHString(wxString::Format("wxGrid.Cell.%d.%d", cell.key.first, cell.key.second)));
        return peer;
    }

    void Publish(const std::shared_ptr<Pane>& pane, const std::vector<Cell>& cells,
                 const Snapshot& snapshot)
    {
        if ( !pane->window || !pane->published ) return;
        const double scale = std::max(0.01, pane->window->GetDPIScaleFactor());
        const auto size = pane->window->GetClientSize();
        const double border = owner->GridLinesEnabled() ? 1 / scale : 0;
        const wxColour highlightColour = owner->GetCellHighlightColour();
        const wxColour gridLineColour = owner->GetGridLineColour();
        const wxColour background = pane->kind == GridPaneKind::Cells
            ? owner->GetDefaultCellBackgroundColour() : owner->GetLabelBackgroundColour();
        X::Media::RectangleGeometry clip;
        clip.Rect(F::Rect{0, 0, static_cast<float>(std::max(0, size.x) / scale),
                                static_cast<float>(std::max(0, size.y) / scale)});
        pane->canvas.Clip(clip);
        pane->canvas.Background(wxWinUICreateColourBrush(background));
        Check(snapshot);
        std::set<CellKey> wanted;
        for ( const auto& cell : cells ) wanted.insert(cell.key);
        for ( auto it = pane->peers.begin(); it != pane->peers.end(); )
        {
            if ( wanted.count(it->first) ) { ++it; continue; }
            const auto retired = it->second;
            it = pane->peers.erase(it); // Retire callbacks before Unloaded.
            uint32_t index = 0;
            if ( pane->canvas.Children().IndexOf(retired->control, index) )
                pane->canvas.Children().RemoveAt(index);
            Check(snapshot);
        }
        for ( const auto& cell : cells )
        {
            Check(snapshot);
            auto found = pane->peers.find(cell.key);
            if ( found != pane->peers.end() &&
                 (found->second->kind != cell.kind ||
                  found->second->table != snapshot.table ||
                  found->second->rows != snapshot.rows ||
                  found->second->columns != snapshot.columns) )
            {
                const auto retired = found->second;
                pane->peers.erase(found);
                uint32_t index = 0;
                if ( pane->canvas.Children().IndexOf(retired->control, index) )
                    pane->canvas.Children().RemoveAt(index);
                Check(snapshot);
                found = pane->peers.end();
            }
            const bool added = found == pane->peers.end();
            auto peer = added ? CreatePeer(pane, cell) : found->second;
            peer->table = snapshot.table;
            peer->rows = snapshot.rows;
            peer->columns = snapshot.columns;
            peer->rect = cell.rect;
            peer->control.Width(cell.rect.width / scale);
            peer->control.Height(cell.rect.height / scale);
            C::Canvas::SetLeft(peer->control, cell.rect.x / scale);
            C::Canvas::SetTop(peer->control, cell.rect.y / scale);
            peer->control.IsEnabled(cell.enabled);
            peer->control.Background(wxWinUICreateColourBrush(cell.background));
            wxWinUIApplyForeground(peer->control, cell.foreground);
            peer->control.BorderThickness(cell.current
                ? X::Thickness{1, 1, 1, 1} : X::Thickness{0, 0, border, border});
            peer->control.BorderBrush(wxWinUICreateColourBrush(cell.current
                ? highlightColour : gridLineColour));
            Check(snapshot);
            if ( peer->text )
            {
                const auto text = wxWinUIToHString(cell.text);
                if ( peer->text.Text() != text ) peer->text.Text(text);
                peer->text.HorizontalAlignment(HorizontalAlignment(cell.horizontal));
                peer->text.VerticalAlignment(VerticalAlignment(cell.vertical));
                peer->text.TextWrapping(cell.kind == GridCellKind::WrappedText
                    ? X::TextWrapping::Wrap : X::TextWrapping::NoWrap);
                wxWinUIApplyFont(peer->text, cell.font);
                wxWinUIApplyForeground(peer->text, cell.foreground);
            }
            if ( auto check = peer->control.try_as<C::CheckBox>() )
                check.IsChecked(cell.checked);
            if ( peer->image ) peer->image.Source(wxWinUIWriteableBitmapFromBitmap(cell.bitmap));
            X::Automation::AutomationProperties::SetName(peer->control, wxWinUIToHString(cell.text));
            Check(snapshot);
            if ( added )
            {
                pane->peers.emplace(cell.key, peer);
                pane->canvas.Children().Append(peer->control);
                Check(snapshot);
            }
            if ( cell.current ) pane->host.SetPreferredFocus(peer->control);
        }
    }

    void Synchronize()
    {
        if ( closed || !owner || !owner->CanUpdateWinUIProjection() || synchronizing ) return;
        synchronizing = true;
        const auto finish = wxMakeGuard([this]() { synchronizing = false; });
        // One repair pass covers a callback mutation. A pathological getter
        // which Refresh()es every time cannot perpetually refill CallAfter.
        for ( unsigned attempt = 0; dirty && attempt != 2; ++attempt )
        {
            dirty = false;
            try
            {
                const auto snapshot = Capture();
                UpdatePanes(snapshot);
                Check(snapshot);
                const auto currentPanes = panes;
                std::vector<std::vector<Cell>> presentations;
                for ( const auto& pane : currentPanes ) presentations.push_back(ReadPane(pane, snapshot));
                Check(snapshot);
                ++suppressNative;
                const auto restore = wxMakeGuard([this]() { --suppressNative; });
                for ( size_t n = 0; n != currentPanes.size(); ++n )
                {
                    Publish(currentPanes[n], presentations[n], snapshot);
                    Check(snapshot);
                }
                if ( const auto main = AutomationMainPane() )
                {
                    const auto peer = U::A::FrameworkElementAutomationPeer::FromElement(main->root);
                    if ( peer )
                        peer.RaiseStructureChangedEvent(U::A::AutomationStructureChangeType::ChildrenInvalidated, nullptr);
                    // No InvalidatePeer: its queued native peg outlives a
                    // retained C++/WinRT peer during reentrant destruction.
                    Check(snapshot);
                }
            }
            catch ( const InvalidGridSnapshot& )
            {
                if ( !owner || closed ) return;
                dirty = true;
            }
            catch ( const winrt::hresult_error& error )
            {
                wxWinUILogException("grid native presentation", error);
                dirty = true;
                return;
            }
        }
    }

    void Close()
    {
        if ( closed ) return;
        closed = true;
        ++revision;
        automationItems.clear();
        const auto retiring = std::move(panes);
        for ( const auto& pane : retiring ) ClosePane(pane);
        owner = nullptr;
    }
};

void wxWinUIGridProjection::Impl::AutomationCheck(const Snapshot& snapshot, CellKey key) const
{
    if ( !IdentityCurrent(snapshot) || key.first < -1 || key.second < -1 ||
         key.first >= owner->GetNumberRows() || key.second >= owner->GetNumberCols() ) U::Unavailable();
}

wxGridCellAttrPtr wxWinUIGridProjection::Impl::AutomationAttr(const Snapshot& snapshot, CellKey key) const
{
    AutomationCheck(snapshot, key);
    // GetCellAttr()/GetCellSize() update wxGrid's attribute cache after a
    // virtual table callback. UIA observation must not run that mutating path
    // (nor dereference the grid if the callback destroys it).
    owner->m_defaultCellAttr->IncRef();
    const wxGridCellAttrPtr defaultAttr(owner->m_defaultCellAttr);
    wxGridCellAttrPtr attr(snapshot.table
        ? snapshot.table->GetAttr(key.first, key.second, wxGridCellAttr::Any) : nullptr);
    if ( !Current(snapshot) ) U::Unavailable();
    if ( attr ) attr->SetDefAttr(defaultAttr.get());
    else attr = defaultAttr;
    return attr;
}

CellKey wxWinUIGridProjection::Impl::AutomationAnchor(const Snapshot& snapshot, CellKey key) const
{
    AutomationCheck(snapshot, key);
    if ( key.first < 0 || key.second < 0 ) return key;
    int rows = 1, columns = 1;
    const auto attr = AutomationAttr(snapshot, key);
    attr->GetSize(&rows, &columns);
    if ( !Current(snapshot) ) U::Unavailable();
    if ( rows <= 0 || columns <= 0 )
    {
        const auto row = static_cast<long long>(key.first) + rows;
        const auto column = static_cast<long long>(key.second) + columns;
        if ( row < 0 || column < 0 || row >= owner->GetNumberRows() || column >= owner->GetNumberCols() ) U::Unavailable();
        key = {static_cast<int>(row), static_cast<int>(column)};
    }
    return key;
}

U::ItemBinding::Position wxWinUIGridProjection::Impl::AutomationPosition(
    const Snapshot& identity, CellKey key) const
{
    AutomationCheck(identity, key);
    if ( key.first < 0 || key.second < 0 ) throw winrt::hresult_error(UIA_E_NOTSUPPORTED);
    const auto snapshot = Capture();
    if ( AutomationAnchor(snapshot, key) != key ) U::Unavailable();
    int rows = 1, columns = 1;
    const auto attr = AutomationAttr(snapshot, key);
    attr->GetSize(&rows, &columns);
    if ( !Current(snapshot) || rows < 1 || columns < 1 ||
         rows > owner->GetNumberRows() - key.first ||
         columns > owner->GetNumberCols() - key.second ) U::Unavailable();
    const int rowPosition = owner->GetRowPos(key.first), columnPosition = owner->GetColPos(key.second);
    // wx permits order changes independently of merged-cell attributes. A
    // non-contiguous/reversed logical span cannot be described by one UIA
    // rectangle; do not silently claim coverage over unrelated cells.
    if ( rows > owner->GetNumberRows() - rowPosition || columns > owner->GetNumberCols() - columnPosition )
        throw winrt::hresult_error(UIA_E_NOTSUPPORTED);
    if ( !owner->m_rowAt.empty() )
        for ( int offset = 1; offset < rows; ++offset )
            if ( owner->GetRowAt(rowPosition + offset) != key.first + offset )
                throw winrt::hresult_error(UIA_E_NOTSUPPORTED);
    if ( !owner->m_colAt.empty() )
        for ( int offset = 1; offset < columns; ++offset )
            if ( owner->GetColAt(columnPosition + offset) != key.second + offset )
                throw winrt::hresult_error(UIA_E_NOTSUPPORTED);
    return {rowPosition, columnPosition, rows, columns};
}

std::shared_ptr<wxWinUIGridProjection::Impl::Pane>
wxWinUIGridProjection::Impl::AutomationMainPane() const
{
    if ( AutomationCurrent() )
        for ( const auto& pane : panes )
            if ( pane->published && pane->window.get() == owner->m_gridWin ) return pane;
    return {};
}

std::pair<std::shared_ptr<wxWinUIGridProjection::Impl::Pane>,
          std::shared_ptr<wxWinUIGridProjection::Impl::Peer>>
wxWinUIGridProjection::Impl::AutomationVisual(CellKey key) const
{
    for ( const auto& pane : panes )
    {
        const auto found = pane->peers.find(key);
        if ( found != pane->peers.end() && IsPeerCurrent(pane, found->second) &&
             pane->window->IsShownOnScreen() && found->second->control.IsLoaded() &&
             X::Media::VisualTreeHelper::GetParent(found->second->control) )
            return {pane, found->second};
    }
    return {};
}

U::A::AutomationPeer wxWinUIGridProjection::Impl::AutomationGrid()
{
    const auto main = AutomationMainPane();
    if ( !main ) U::Unavailable();
    return U::A::FrameworkElementAutomationPeer::CreatePeerForElement(main->root);
}

F::Rect wxWinUIGridProjection::Impl::AutomationBounds(const Snapshot& snapshot, CellKey key) const
{
    AutomationCheck(snapshot, key);
    if ( (key.first >= 0 && owner->GetRowSize(key.first) == 0) ||
         (key.second >= 0 && owner->GetColSize(key.second) == 0) ) return {};
    const auto visual = AutomationVisual(key);
    if ( !visual.first ) return {};
    auto* const nativeHost = wxWinUITopLevelHost::FindSlotOwner(visual.first->window.get());
    if ( !nativeHost || !nativeHost->Root() ) return {};
    const auto nativeRoot = nativeHost->Root();
    const auto element = visual.second->control;
    const auto viewport = visual.first->root;
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

winrt::hstring wxWinUIGridProjection::Impl::AutomationName(const Snapshot& identity, CellKey key)
{
    AutomationCheck(identity, key);
    const auto snapshot = Capture();
    if ( AutomationAnchor(snapshot, key) != key ) U::Unavailable();
    wxString text;
    if ( key.first < 0 )
        text = key.second < 0 ? owner->GetCornerLabelValue() : owner->GetColLabelValue(key.second);
    else if ( key.second < 0 ) text = owner->GetRowLabelValue(key.first);
    else text = snapshot.table->GetValue(key.first, key.second);
    // Names and Value use the authoritative model string, without another
    // renderer invocation. Formatting/custom Draw stay in the visual path.
    if ( !Current(snapshot) ) U::Unavailable();
    return wxWinUIToHString(text);
}

bool wxWinUIGridProjection::Impl::AutomationToggle(const Snapshot& identity, CellKey key) const
{
    AutomationCheck(identity, key);
    if ( key.first < 0 || key.second < 0 ) return false;
    const auto visual = AutomationVisual(key);
    return visual.second && visual.second->kind == GridCellKind::Bool;
}

bool wxWinUIGridProjection::Impl::AutomationEnabled(const Snapshot& identity, CellKey key) const
{
    AutomationCheck(identity, key);
    if ( !owner->IsEnabled() ) return false;
    if ( !AutomationToggle(identity, key) ) return true;
    const auto snapshot = Capture();
    const auto attr = AutomationAttr(snapshot, key);
    return owner->IsEditable() && !attr->IsReadOnly();
}

wxGridCellEditorPtr wxWinUIGridProjection::Impl::AutomationValueEditor(
    const Snapshot& identity, CellKey key) const
{
    AutomationCheck(identity, key);
    if ( key.first < 0 || key.second < 0 ) return wxGridCellEditorPtr(nullptr);
    const auto snapshot = Capture();
    const auto attr = AutomationAttr(snapshot, key);
    const auto editor = attr->GetEditorPtr(owner.get(), key.first, key.second);
    if ( !Current(snapshot) ) U::Unavailable();
    return editor && HasTextValueEditor(*editor) ? editor : wxGridCellEditorPtr();
}

bool wxWinUIGridProjection::Impl::AutomationReadOnly(const Snapshot& identity, CellKey key) const
{
    AutomationCheck(identity, key);
    const auto snapshot = Capture();
    const auto attr = AutomationAttr(snapshot, key);
    return !owner->IsEditable() || attr->IsReadOnly();
}

void wxWinUIGridProjection::Impl::AutomationSetValue(
    const Snapshot& identity, CellKey key, const winrt::hstring& input)
{
    AutomationCheck(identity, key);
    if ( settingAutomationValue ) throw winrt::hresult_error(UIA_E_INVALIDOPERATION);
    if ( !owner->IsEnabled() ) throw winrt::hresult_error(UIA_E_ELEMENTNOTENABLED);
    if ( AutomationReadOnly(identity, key) ) throw winrt::hresult_error(UIA_E_INVALIDOPERATION);
    auto editor = AutomationValueEditor(identity, key);
    if ( !editor ) throw winrt::hresult_error(UIA_E_NOTSUPPORTED);
    const wxString value = wxWinUIFromHString(input);
#ifndef wxNO_RTTI
    // Reject invalid numeric input before opening an editor. The editor still
    // owns conversion/range/validation and the eventual model transaction.
    if ( typeid(*editor) == typeid(wxGridCellNumberEditor) && !value.empty() )
    {
        long number;
        if ( !value.ToLong(&number) ) throw winrt::hresult_invalid_argument();
    }
    if ( typeid(*editor) == typeid(wxGridCellFloatEditor) && !value.empty() )
    {
        double number;
        if ( !wxNumberFormatter::FromString(value, &number) ) throw winrt::hresult_invalid_argument();
    }
#endif
    settingAutomationValue = true;
    const auto reset = wxMakeGuard([this]() { settingAutomationValue = false; });
    AutomationRealize(identity, key);
    if ( owner->GetGridCursorRow() != key.first || owner->GetGridCursorCol() != key.second )
    {
        const bool selected = owner->SetCurrentCell(key.first, key.second);
        AutomationCheck(identity, key);
        if ( !selected ) throw winrt::hresult_error(UIA_E_INVALIDOPERATION);
    }
    if ( !owner->IsCellEditControlEnabled() )
    {
        const bool shown = owner->DoEnableCellEditControl(wxGridActivationSource::FromProgram());
        AutomationCheck(identity, key);
        if ( !shown ) throw winrt::hresult_error(UIA_E_INVALIDOPERATION);
    }
    editor = owner->m_activeCellEditor;
    if ( !editor || !HasTextValueEditor(*editor) ) throw winrt::hresult_error(UIA_E_NOTSUPPORTED);
    wxWeakRef<wxWindow> window(editor->GetWindow());
    const auto checkEditor = [&]()
    {
        AutomationCheck(identity, key);
        if ( owner->GetGridCursorRow() != key.first || owner->GetGridCursorCol() != key.second ||
             owner->m_activeCellEditor != editor || !owner->IsCellEditControlEnabled() ||
             !window || window->IsBeingDeleted() || editor->GetWindow() != window.get() ) U::Unavailable();
    };
    checkEditor();
    if ( !owner->IsEnabled() ) throw winrt::hresult_error(UIA_E_ELEMENTNOTENABLED);
    if ( AutomationReadOnly(identity, key) ) throw winrt::hresult_error(UIA_E_INVALIDOPERATION);
    checkEditor();
    if ( auto* const text = wxDynamicCast(window.get(), wxTextCtrl) )
        text->ChangeValue(value);
#if wxUSE_SPINCTRL
    else if ( auto* const spin = wxDynamicCast(window.get(), wxSpinCtrl) )
    {
        long number;
        if ( !value.ToLong(&number) || number < spin->GetMin() || number > spin->GetMax() )
            throw winrt::hresult_invalid_argument();
        spin->SetValue(static_cast<int>(number));
    }
#endif
    else throw winrt::hresult_error(UIA_E_NOTSUPPORTED);
    checkEditor();
    const bool valid = window->Validate();
    checkEditor();
    if ( !valid ) throw winrt::hresult_invalid_argument();
    if ( !owner->IsEnabled() ) throw winrt::hresult_error(UIA_E_ELEMENTNOTENABLED);
    if ( AutomationReadOnly(identity, key) ) throw winrt::hresult_error(UIA_E_INVALIDOPERATION);
    checkEditor();
    // This is the same HIDDEN -> EndEdit -> CHANGING -> ApplyEdit -> CHANGED
    // path used by keyboard/focus completion. It retains both public vetoes.
    owner->DoDisableCellEditControl();
    AutomationCheck(identity, key);
}

void wxWinUIGridProjection::Impl::AutomationRealize(const Snapshot& identity, CellKey key)
{
    AutomationCheck(identity, key);
    if ( !owner->IsEnabled() ) throw winrt::hresult_error(UIA_E_ELEMENTNOTENABLED);
    if ( !owner->IsShownOnScreen() ) throw winrt::hresult_error(UIA_E_INVALIDOPERATION);
    const auto snapshot = Capture();
    if ( AutomationAnchor(snapshot, key) != key ) U::Unavailable();
    if ( (key.first >= 0 && owner->GetRowSize(key.first) == 0) ||
         (key.second >= 0 && owner->GetColSize(key.second) == 0) )
        throw winrt::hresult_error(UIA_E_INVALIDOPERATION);
    if ( owner->GetNumberRows() == 0 || owner->GetNumberCols() == 0 )
        throw winrt::hresult_error(UIA_E_INVALIDOPERATION);
    // Header realization keeps the orthogonal cursor coordinate where possible.
    const int row = key.first < 0 ? std::max(0, owner->GetGridCursorRow()) : key.first;
    const int column = key.second < 0 ? std::max(0, owner->GetGridCursorCol()) : key.second;
    owner->MakeCellVisible(row, column);
    AutomationCheck(identity, key);
    Synchronize();
    AutomationCheck(identity, key);
}

U::A::AutomationPeer wxWinUIGridProjection::Impl::AutomationItem(CellKey key)
{
    if ( !AutomationCurrent() ) U::Unavailable();
    const auto snapshot = Capture();
    key = AutomationAnchor(snapshot, key);
    if ( !hasAutomationSchema || !IdentityCurrent(automationSchema) )
    {
        automationItems.clear();
        automationSchema = snapshot;
        hasAutomationSchema = true;
    }
    const auto found = automationItems.find(key);
    if ( found != automationItems.end() )
        if ( const auto peer = found->second.get() ) return peer;
    // Evict expired requested identities, without enumerating model cells.
    if ( automationItems.size() % 128 == 127 )
        for ( auto it = automationItems.begin(); it != automationItems.end(); )
            if ( !it->second.get() ) it = automationItems.erase(it); else ++it;

    auto binding = std::make_shared<U::ItemBinding>();
    const std::weak_ptr<Impl> weak = shared_from_this();
    const auto resolve = [weak, snapshot, key]() -> std::shared_ptr<Impl>
    {
        const auto self = weak.lock();
        if ( !self ) U::Unavailable();
        self->AutomationCheck(snapshot, key);
        if ( self->AutomationAnchor(self->Capture(), key) != key ) U::Unavailable();
        return self;
    };
    binding->header = key.first < 0 || key.second < 0;
    binding->check = [resolve]() { (void)resolve(); };
    binding->position = [resolve, snapshot, key]() { return resolve()->AutomationPosition(snapshot, key); };
    binding->name = [resolve, snapshot, key]() { return resolve()->AutomationName(snapshot, key); };
    binding->bounds = [resolve, snapshot, key]() { return resolve()->AutomationBounds(snapshot, key); };
    binding->grid = [resolve]() { return resolve()->AutomationGrid(); };
    binding->toggle = [resolve, snapshot, key]() { return resolve()->AutomationToggle(snapshot, key); };
    binding->hasValue = [resolve, snapshot, key]()
    {
        return static_cast<bool>(resolve()->AutomationValueEditor(snapshot, key));
    };
    binding->readOnly = [resolve, snapshot, key]() { return resolve()->AutomationReadOnly(snapshot, key); };
    binding->value = [resolve, snapshot, key]() { return resolve()->AutomationName(snapshot, key); };
    binding->setValue = [resolve, snapshot, key](const winrt::hstring& value)
    {
        resolve()->AutomationSetValue(snapshot, key, value);
    };
    binding->invokable = [resolve, key]()
    {
        const auto visual = resolve()->AutomationVisual(key);
        return visual.second && visual.second->kind != GridCellKind::Bool;
    };
    binding->enabled = [resolve, snapshot, key]() { return resolve()->AutomationEnabled(snapshot, key); };
    binding->toggleState = [resolve, snapshot, key]()
    {
        const auto self = resolve();
        self->AutomationCheck(snapshot, key);
        const auto visual = self->AutomationVisual(key);
        const auto check = visual.second ? visual.second->control.try_as<C::CheckBox>() : nullptr;
        if ( !check ) throw winrt::hresult_error(UIA_E_NOTSUPPORTED);
        const auto checked = check.IsChecked();
        return !checked ? X::Automation::ToggleState::Indeterminate
            : checked.Value() ? X::Automation::ToggleState::On : X::Automation::ToggleState::Off;
    };
    binding->realize = [resolve, snapshot, key]() { resolve()->AutomationRealize(snapshot, key); };
    binding->activate = [resolve, snapshot, key]()
    {
        const auto self = resolve();
        if ( !self->AutomationEnabled(snapshot, key) ) throw winrt::hresult_error(UIA_E_ELEMENTNOTENABLED);
        self->AutomationRealize(snapshot, key);
        const auto visual = self->AutomationVisual(key);
        if ( !visual.first ) throw winrt::hresult_error(UIA_E_INVALIDOPERATION);
        self->Activate(visual.first, visual.second);
    };
    binding->headers = [resolve, snapshot, key](bool rows)
    {
        const auto self = resolve();
        std::vector<U::A::AutomationPeer> result;
        const auto position = self->AutomationPosition(snapshot, key);
        if ( rows ? self->owner->GetRowLabelSize() <= 0 : self->owner->GetColLabelSize() <= 0 ) return result;
        const int count = rows ? position.rowSpan : position.columnSpan;
        for ( int offset = 0; offset < count; ++offset )
        {
            result.push_back(self->AutomationItem(rows ? CellKey{key.first + offset, -1}
                                                       : CellKey{-1, key.second + offset}));
            self->AutomationCheck(snapshot, key);
        }
        return result;
    };
    const auto peer = winrt::make<U::ItemPeer>(std::move(binding));
    automationItems[key] = winrt::make_weak(peer);
    return peer;
}

std::vector<U::A::AutomationPeer> wxWinUIGridProjection::Impl::AutomationHeaders(bool rows)
{
    if ( !AutomationCurrent() ) U::Unavailable();
    const auto snapshot = Capture();
    std::vector<U::A::AutomationPeer> result;
    if ( rows ? owner->GetRowLabelSize() <= 0 : owner->GetColLabelSize() <= 0 ) return result;
    const int count = rows ? owner->GetNumberRows() : owner->GetNumberCols();
    result.reserve(static_cast<size_t>(count));
    // O(requested headers), zero table values. Ordinary GetChildren only
    // returns the viewport set, not this explicit full-table header request.
    for ( int position = 0; position < count; ++position )
    {
        if ( !Current(snapshot) ) U::Unavailable();
        const int index = rows ? owner->GetRowAt(position) : owner->GetColAt(position);
        result.push_back(AutomationItem(rows ? CellKey{index, -1} : CellKey{-1, index}));
    }
    if ( !Current(snapshot) ) U::Unavailable();
    return result;
}

std::shared_ptr<U::GridBinding> wxWinUIGridProjection::Impl::AutomationBinding()
{
    auto binding = std::make_shared<U::GridBinding>();
    const std::weak_ptr<Impl> weak = shared_from_this();
    const auto resolve = [weak]() -> std::shared_ptr<Impl>
    {
        const auto self = weak.lock();
        if ( !self || !self->AutomationCurrent() || !self->AutomationMainPane() ) U::Unavailable();
        return self;
    };
    binding->check = [resolve]() { (void)resolve(); };
    binding->rows = [resolve]() { return resolve()->owner->GetNumberRows(); };
    binding->columns = [resolve]() { return resolve()->owner->GetNumberCols(); };
    binding->name = [resolve]() { return wxWinUIToHString(resolve()->owner->GetName()); };
    binding->enabled = [resolve]() { return resolve()->owner->IsEnabled(); };
    binding->cell = [resolve](int row, int column)
    {
        const auto self = resolve();
        const auto snapshot = self->Capture();
        if ( row < 0 || column < 0 || row >= self->owner->GetNumberRows() || column >= self->owner->GetNumberCols() )
            throw winrt::hresult_invalid_argument();
        const CellKey key{self->owner->GetRowAt(row), self->owner->GetColAt(column)};
        const auto peer = self->AutomationItem(key);
        if ( !self->Current(snapshot) ) U::Unavailable();
        return peer;
    };
    binding->headers = [resolve](bool rows) { return resolve()->AutomationHeaders(rows); };
    binding->children = [resolve]()
    {
        const auto self = resolve();
        const auto snapshot = self->Capture();
        std::set<CellKey> keys;
        for ( const auto& pane : self->panes )
            for ( const auto& item : pane->peers )
                if ( self->IsPeerCurrent(pane, item.second) ) keys.insert(item.first);
        std::vector<U::A::AutomationPeer> result;
        result.reserve(keys.size());
        for ( const auto& key : keys )
        {
            result.push_back(self->AutomationItem(key));
            if ( !self->Current(snapshot) ) U::Unavailable();
        }
        return result;
    };
    return binding;
}

wxWinUIGridProjection::wxWinUIGridProjection(std::shared_ptr<Impl> impl)
    : m_impl(std::move(impl)) {}

std::shared_ptr<wxWinUIGridProjection> wxWinUIGridProjection::Create(wxGrid* grid)
{
    auto impl = std::make_shared<Impl>(grid);
    auto projection = std::shared_ptr<wxWinUIGridProjection>(new wxWinUIGridProjection(impl));
    impl->Synchronize();
    return projection;
}

wxWinUIGridProjection::~wxWinUIGridProjection() { Close(); }
void wxWinUIGridProjection::Invalidate() { m_impl->Invalidate(); }
void wxWinUIGridProjection::SyncViewport() { m_impl->Invalidate(); }
void wxWinUIGridProjection::Update()
{
    // Like wxWindow::Update(), finish already requested painting now. Do not
    // invalidate again: a queued presentation after this flush must be a no-op.
    // Keep Impl alive if a model/renderer callback destroys the public grid.
    const auto impl = m_impl;
    impl->Synchronize();
}
void wxWinUIGridProjection::Close() { m_impl->Close(); }
bool wxWinUIGridProjection::OwnsWindow(wxWindow* window) const
{
    for ( const auto& pane : m_impl->panes )
    {
        if ( !pane->published || pane->window.get() != window ||
             !pane->host.IsOk() || pane->host.GetContent() != pane->root ) continue;
        const auto* host = wxWinUITopLevelHost::FindSlotOwner(window);
        const auto* slot = host ? host->FindSlot(window) : nullptr;
        if ( slot && slot->GetContent() == pane->root ) return true;
    }
    return false;
}

#endif // __WXWINUI__ && wxUSE_WINUI3 && wxUSE_GRID
