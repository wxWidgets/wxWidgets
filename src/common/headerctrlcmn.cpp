///////////////////////////////////////////////////////////////////////////////
// Name:        src/common/headerctrlcmn.cpp
// Purpose:     implementation of wxHeaderCtrlBase
// Author:      Vadim Zeitlin
// Created:     2008-12-02
// Copyright:   (c) 2008 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// for compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#if wxUSE_HEADERCTRL

#ifndef WX_PRECOMP
    #include "wx/menu.h"
#endif // WX_PRECOMP

#include "wx/headerctrl.h"
#include "wx/private/columnorder.h"
#include "wx/rearrangectrl.h"
#include "wx/renderer.h"
#include "wx/weakref.h"

#include <algorithm>
#include <cstdint>
#include <limits>
#include <vector>

namespace
{

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

const unsigned int wxNO_COLUMN = static_cast<unsigned>(-1);
const unsigned int wxID_COLUMNS_BASE = 1;

struct SimpleHeaderState
{
    explicit SimpleHeaderState(wxHeaderCtrlSimple* header)
        : weakHeader(header),
          revision(0)
    {
    }

    wxWeakRef<wxWindow> weakHeader;
    std::uint64_t revision;
};

std::vector<SimpleHeaderState>& GetSimpleHeaderStates()
{
    static std::vector<SimpleHeaderState> states;
    return states;
}

SimpleHeaderState& GetSimpleHeaderState(wxHeaderCtrlSimple* header)
{
    std::vector<SimpleHeaderState>& states = GetSimpleHeaderStates();
    states.erase(
        std::remove_if(
            states.begin(),
            states.end(),
            [](const SimpleHeaderState& state)
            {
                return !state.weakHeader;
            }),
        states.end());

    for ( SimpleHeaderState& state : states )
    {
        if ( state.weakHeader.get() == header )
            return state;
    }

    states.emplace_back(header);
    return states.back();
}

std::uint64_t GetSimpleHeaderRevision(wxHeaderCtrlSimple* header)
{
    return GetSimpleHeaderState(header).revision;
}

void BumpSimpleHeaderRevision(wxHeaderCtrlSimple* header)
{
    ++GetSimpleHeaderState(header).revision;
}

// Keep the reentrancy guard out of wxHeaderCtrlSimple itself: this class is
// exported and adding private data to it would break its ABI on every port.
std::vector<const wxHeaderCtrlSimple*>& GetActiveSimpleHeaderMutations()
{
    static std::vector<const wxHeaderCtrlSimple*> active;
    return active;
}

class SimpleHeaderMutationGuard final
{
public:
    explicit SimpleHeaderMutationGuard(const wxHeaderCtrlSimple* header)
        : m_header(header),
          m_entered(false)
    {
        std::vector<const wxHeaderCtrlSimple*>& active =
            GetActiveSimpleHeaderMutations();
        if ( std::find(active.begin(), active.end(), header) == active.end() )
        {
            active.push_back(header);
            m_entered = true;
        }
    }

    ~SimpleHeaderMutationGuard()
    {
        if ( !m_entered )
            return;

        std::vector<const wxHeaderCtrlSimple*>& active =
            GetActiveSimpleHeaderMutations();
        wxASSERT_MSG( !active.empty() && active.back() == m_header,
                      "unbalanced header mutation guard" );
        active.pop_back();
    }

    bool IsEntered() const { return m_entered; }

private:
    const wxHeaderCtrlSimple* const m_header;
    bool m_entered;
};

bool IsNaturalColumnOrder(const wxArrayInt& order, unsigned int count)
{
    if ( order.size() != count )
        return false;

    for ( unsigned int pos = 0; pos < count; ++pos )
    {
        if ( order[pos] != static_cast<int>(pos) )
            return false;
    }

    return true;
}

// ----------------------------------------------------------------------------
// wxHeaderColumnsRearrangeDialog: dialog for customizing our columns
// ----------------------------------------------------------------------------

#if wxUSE_REARRANGECTRL

class wxHeaderColumnsRearrangeDialog : public wxRearrangeDialog
{
public:
    wxHeaderColumnsRearrangeDialog(wxWindow *parent,
                                   const wxArrayInt& order,
                                   const wxArrayString& items)
        : wxRearrangeDialog
          (
            parent,
            _("Please select the columns to show and define their order:"),
            _("Customize Columns"),
            order,
            items
          )
    {
    }
};

#endif // wxUSE_REARRANGECTRL

} // anonymous namespace

// ============================================================================
// wxHeaderCtrlBase implementation
// ============================================================================

extern WXDLLIMPEXP_DATA_CORE(const char) wxHeaderCtrlNameStr[] = "wxHeaderCtrl";

wxBEGIN_EVENT_TABLE(wxHeaderCtrlBase, wxControl)
    EVT_HEADER_SEPARATOR_DCLICK(wxID_ANY, wxHeaderCtrlBase::OnSeparatorDClick)
#if wxUSE_MENUS
    EVT_HEADER_RIGHT_CLICK(wxID_ANY, wxHeaderCtrlBase::OnRClick)
#endif // wxUSE_MENUS
wxEND_EVENT_TABLE()

void wxHeaderCtrlBase::ScrollWindow(int dx,
                                    int WXUNUSED_UNLESS_DEBUG(dy),
                                    const wxRect * WXUNUSED_UNLESS_DEBUG(rect))

{
    // this doesn't make sense at all
    wxASSERT_MSG( !dy, "header window can't be scrolled vertically" );

    // this would actually be nice to support for "frozen" headers but it isn't
    // supported currently
    wxASSERT_MSG( !rect, "header window can't be scrolled partially" );

    DoScrollHorz(dx);
}

void wxHeaderCtrlBase::SetColumnCount(unsigned int count)
{
    if ( count != GetColumnCount() )
        OnColumnCountChanging(count);

    // still call DoSetCount() even if the count didn't really change in order
    // to update all the columns
    DoSetCount(count);
}

int wxHeaderCtrlBase::GetColumnTitleWidth(const wxHeaderColumn& col)
{
    const wxWeakRef<wxWindow> weakThis(this);
    const wxString title = col.GetTitle();
    if ( !weakThis )
        return 0;

    long long width = wxWindowBase::GetTextExtent(title).x;

    // add some margin:
    width += wxRendererNative::Get().GetHeaderButtonMargin(this);

    // if a bitmap is used, add space for it and 2px border:
    wxBitmapBundle bmp = col.GetBitmapBundle();
    if ( !weakThis )
        return 0;

    if ( bmp.IsOk() )
    {
        width +=
            static_cast<long long>(
                bmp.GetPreferredLogicalSizeFor(this).GetWidth()) + 2;
    }

    if ( width <= 0 )
        return 0;
    if ( width > std::numeric_limits<int>::max() )
        return std::numeric_limits<int>::max();

    return static_cast<int>(width);
}

// ----------------------------------------------------------------------------
// wxHeaderCtrlBase event handling
// ----------------------------------------------------------------------------

void wxHeaderCtrlBase::OnSeparatorDClick(wxHeaderCtrlEvent& event)
{
    const unsigned col = event.GetColumn();
    if ( col >= GetColumnCount() )
        return;

    const wxHeaderColumn* const column = &GetColumn(col);
    const wxWeakRef<wxWindow> weakThis(this);
    const auto isSameColumn = [this, weakThis, col, column]()
    {
        return weakThis.get() == this &&
               col < GetColumnCount() &&
               &GetColumn(col) == column;
    };

    if ( !column->IsResizeable() )
    {
        if ( !isSameColumn() )
            return;

        event.Skip();
        return;
    }
    if ( !isSameColumn() )
        return;

    int w = GetColumnTitleWidth(*column);
    if ( !isSameColumn() )
        return;

    const bool updated = UpdateColumnWidthToFit(col, w);
    if ( !isSameColumn() )
        return;

    if ( !updated )
        event.Skip();
    else
        UpdateColumn(col);
}

#if wxUSE_MENUS

void wxHeaderCtrlBase::OnRClick(wxHeaderCtrlEvent& event)
{
    if ( !HasFlag(wxHD_ALLOW_HIDE) )
    {
        event.Skip();
        return;
    }

    ShowColumnsMenu(ScreenToClient(wxGetMousePosition()));
}

#endif // wxUSE_MENUS

// ----------------------------------------------------------------------------
// wxHeaderCtrlBase column reordering
// ----------------------------------------------------------------------------

void wxHeaderCtrlBase::SetColumnsOrder(const wxArrayInt& order)
{
    const unsigned count = GetColumnCount();
    wxCHECK_RET( order.size() == count, "wrong number of columns" );

    // check the array validity
    wxArrayInt seen(count, 0);
    for ( unsigned n = 0; n < count; n++ )
    {
        const unsigned idx = order[n];
        wxCHECK_RET( idx < count, "invalid column index" );
        wxCHECK_RET( !seen[idx], "duplicate column index" );

        seen[idx] = 1;
    }

    DoSetColumnsOrder(order);

    // TODO-RTL: do we need to reverse the array?
}

void wxHeaderCtrlBase::ResetColumnsOrder()
{
    const unsigned count = GetColumnCount();
    wxArrayInt order(count);
    for ( unsigned n = 0; n < count; n++ )
        order[n] = n;

    DoSetColumnsOrder(order);
}

wxArrayInt wxHeaderCtrlBase::GetColumnsOrder() const
{
    const wxArrayInt order = DoGetColumnsOrder();

    wxASSERT_MSG( order.size() == GetColumnCount(), "invalid order array" );

    return order;
}

unsigned int wxHeaderCtrlBase::GetColumnAt(unsigned int pos) const
{
    wxCHECK_MSG( pos < GetColumnCount(), wxNO_COLUMN, "invalid position" );

    return GetColumnsOrder()[pos];
}

unsigned int wxHeaderCtrlBase::GetColumnPos(unsigned int idx) const
{
    const unsigned count = GetColumnCount();

    wxCHECK_MSG( idx < count, wxNO_COLUMN, "invalid index" );

    const wxArrayInt order = GetColumnsOrder();
    int pos = order.Index(idx);
    wxCHECK_MSG( pos != wxNOT_FOUND, wxNO_COLUMN, "column unexpectedly not displayed at all" );

    return (unsigned int)pos;
}

/* static */
void wxHeaderCtrlBase::MoveColumnInOrderArray(wxArrayInt& order,
                                              unsigned int idx,
                                              unsigned int pos)
{
    int posOld = order.Index(idx);
    wxASSERT_MSG( posOld != wxNOT_FOUND, "invalid index" );

    if ( pos != (unsigned int)posOld )
    {
        order.RemoveAt(posOld);
        order.Insert(idx, pos);
    }
}

void
wxHeaderCtrlBase::DoResizeColumnIndices(wxArrayInt& colIndices, unsigned int count)
{
    // update the column indices array if necessary
    const unsigned countOld = colIndices.size();
    if ( count > countOld )
    {
        // all new columns have default positions equal to their indices
        for ( unsigned n = countOld; n < count; n++ )
            colIndices.push_back(n);
    }
    else if ( count < countOld )
    {
        // filter out all the positions which are invalid now while keeping the
        // order of the remaining ones
        wxArrayInt colIndicesNew;
        colIndicesNew.reserve(count);
        for ( unsigned n = 0; n < countOld; n++ )
        {
            const unsigned idx = colIndices[n];
            if ( idx < count )
                colIndicesNew.push_back(idx);
        }

        colIndices.swap(colIndicesNew);
    }
    //else: count didn't really change, nothing to do

    wxASSERT_MSG( colIndices.size() == count, "logic error" );
}

// ----------------------------------------------------------------------------
// wxHeaderCtrl extra UI
// ----------------------------------------------------------------------------

#if wxUSE_MENUS

void wxHeaderCtrlBase::AddColumnsItems(wxMenu& menu, int idColumnsBase)
{
    const unsigned count = GetColumnCount();
    for ( unsigned n = 0; n < count; n++ )
    {
        const wxHeaderColumn& col = GetColumn(n);
        menu.AppendCheckItem(idColumnsBase + n, col.GetTitle());
        if ( col.IsShown() )
            menu.Check(idColumnsBase + n, true);
    }
}

bool wxHeaderCtrlBase::ShowColumnsMenu(const wxPoint& pt, const wxString& title)
{
    // construct the menu with the entries for all columns
    wxMenu menu;
    if ( !title.empty() )
        menu.SetTitle(title);

    AddColumnsItems(menu, wxID_COLUMNS_BASE);

    // ... and an extra one to show the customization dialog if the user is
    // allowed to reorder the columns too
    const unsigned idCustomize = GetColumnCount() + wxID_COLUMNS_BASE;
    if ( HasFlag(wxHD_ALLOW_REORDER) )
    {
        menu.AppendSeparator();
        menu.Append(idCustomize, _("&Customize..."));
    }

    // do show the menu and get the user selection
    const int rc = GetPopupMenuSelectionFromUser(menu, pt);
    if ( rc == wxID_NONE )
        return false;

    if ( static_cast<unsigned>(rc) == idCustomize )
    {
        return ShowCustomizeDialog();
    }
    else // a column selected from the menu
    {
        const int columnIndex = rc - wxID_COLUMNS_BASE;
        UpdateColumnVisibility(columnIndex, !GetColumn(columnIndex).IsShown());
        UpdateColumn(columnIndex);
    }

    return true;
}

#endif // wxUSE_MENUS

bool wxHeaderCtrlBase::ShowCustomizeDialog()
{
#if wxUSE_REARRANGECTRL
    // prepare the data for showing the dialog
    wxArrayInt order = GetColumnsOrder();

    const unsigned count = GetColumnCount();

    // notice that titles are always in the index order, they will be shown
    // rearranged according to the display order in the dialog
    wxArrayString titles;
    titles.reserve(count);
    for ( unsigned n = 0; n < count; n++ )
        titles.push_back(GetColumn(n).GetTitle());

    // this loop is however over positions and not indices
    unsigned pos;
    for ( pos = 0; pos < count; pos++ )
    {
        int& idx = order[pos];
        if ( GetColumn(idx).IsHidden() )
        {
            // indicate that this one is hidden
            idx = ~idx;
        }
    }

    // do show it
    wxHeaderColumnsRearrangeDialog dlg(this, order, titles);
    if ( dlg.ShowModal() == wxID_OK )
    {
        // and apply the changes
        order = dlg.GetOrder();
        for ( pos = 0; pos < count; pos++ )
        {
            int& idx = order[pos];
            const bool show = idx >= 0;
            if ( !show )
            {
                // make all indices positive for passing them to SetColumnsOrder()
                idx = ~idx;
            }

            if ( show != GetColumn(idx).IsShown() )
                UpdateColumnVisibility(idx, show);
        }

        UpdateColumnsOrder(order);
        SetColumnsOrder(order);

        return true;
    }
#endif // wxUSE_REARRANGECTRL

    return false;
}

// ============================================================================
// wxHeaderCtrlSimple implementation
// ============================================================================

wxBEGIN_EVENT_TABLE(wxHeaderCtrlSimple, wxHeaderCtrl)
    EVT_HEADER_RESIZING(wxID_ANY, wxHeaderCtrlSimple::OnHeaderResizing)
wxEND_EVENT_TABLE()

void wxHeaderCtrlSimple::Init()
{
    m_sortKey = wxNO_COLUMN;
}

const wxHeaderColumn& wxHeaderCtrlSimple::GetColumn(unsigned int idx) const
{
    return m_cols[idx];
}

void wxHeaderCtrlSimple::DoInsert(const wxHeaderColumnSimple& col, unsigned int idx)
{
    SimpleHeaderMutationGuard mutationGuard(this);
    wxCHECK_RET( mutationGuard.IsEntered(),
                 "reentrant column insertion is not allowed" );

    const unsigned int countOld = GetColumnCount();
    wxArrayInt order = GetColumnsOrder();
    wxCHECK_RET(
        wxPrivate::ColumnOrderMutation::Insert(
            order, countOld, idx, 1, idx),
        "invalid column order before insertion" );

    const wxWeakRef<wxWindow> weakThis(this);

    // SetColumnCount() can only infer an append/remove-at-end mutation. Put
    // its platform-specific implementation in the natural order first, then
    // publish the transactionally computed order once the new count exists.
    ResetColumnsOrder();
    if ( weakThis.get() != this )
        return;
    const wxArrayInt naturalOrder = GetColumnsOrder();
    if ( weakThis.get() != this )
        return;
    wxCHECK_RET( IsNaturalColumnOrder(naturalOrder, countOld),
                 "column order changed reentrantly while inserting" );

    m_cols.insert(m_cols.begin() + idx, col);
    if ( m_sortKey != wxNO_COLUMN && m_sortKey >= idx )
        ++m_sortKey;
    if ( m_cols[idx].IsSortKey() )
    {
        if ( m_sortKey != wxNO_COLUMN && m_sortKey != idx )
            m_cols[m_sortKey].UnsetAsSortKey();
        m_sortKey = idx;
    }
    BumpSimpleHeaderRevision(this);
    UpdateColumnCount();
    if ( weakThis.get() != this )
        return;
    wxCHECK_RET( GetColumnCount() == m_cols.size(),
                 "column count changed reentrantly while inserting" );

    SetColumnsOrder(order);
}

void wxHeaderCtrlSimple::DoDelete(unsigned int idx)
{
    SimpleHeaderMutationGuard mutationGuard(this);
    wxCHECK_RET( mutationGuard.IsEntered(),
                 "reentrant column deletion is not allowed" );

    const unsigned int countOld = GetColumnCount();
    wxArrayInt order = GetColumnsOrder();
    wxCHECK_RET(
        wxPrivate::ColumnOrderMutation::Erase(order, countOld, idx, 1),
        "invalid column order before deletion" );

    const wxWeakRef<wxWindow> weakThis(this);
    ResetColumnsOrder();
    if ( weakThis.get() != this )
        return;
    const wxArrayInt naturalOrder = GetColumnsOrder();
    if ( weakThis.get() != this )
        return;
    wxCHECK_RET( IsNaturalColumnOrder(naturalOrder, countOld),
                 "column order changed reentrantly while deleting" );

    m_cols.erase(m_cols.begin() + idx);
    if ( idx == m_sortKey )
        m_sortKey = wxNO_COLUMN;
    else if ( m_sortKey != wxNO_COLUMN && m_sortKey > idx )
        --m_sortKey;
    BumpSimpleHeaderRevision(this);
    UpdateColumnCount();
    if ( weakThis.get() != this )
        return;
    wxCHECK_RET( GetColumnCount() == m_cols.size(),
                 "column count changed reentrantly while deleting" );

    SetColumnsOrder(order);
}

void wxHeaderCtrlSimple::DeleteAllColumns()
{
    SimpleHeaderMutationGuard mutationGuard(this);
    wxCHECK_RET( mutationGuard.IsEntered(),
                 "reentrant column deletion is not allowed" );

    m_cols.clear();
    m_sortKey = wxNO_COLUMN;
    BumpSimpleHeaderRevision(this);

    const wxWeakRef<wxWindow> weakThis(this);
    UpdateColumnCount();
    if ( weakThis.get() != this )
        return;
    wxCHECK_RET( GetColumnCount() == m_cols.size(),
                 "column count changed reentrantly while deleting" );
}


void wxHeaderCtrlSimple::DoShowColumn(unsigned int idx, bool show)
{
    if ( show != m_cols[idx].IsShown() )
    {
        m_cols[idx].SetHidden(!show);
        BumpSimpleHeaderRevision(this);

        UpdateColumn(idx);
    }
}

void wxHeaderCtrlSimple::DoShowSortIndicator(unsigned int idx, bool ascending)
{
    const wxWeakRef<wxWindow> weakThis(this);
    const std::uint64_t revision = GetSimpleHeaderRevision(this);
    const bool hadSortKey = m_sortKey != wxNO_COLUMN;
    RemoveSortIndicator();
    if ( weakThis.get() != this )
        return;
    if ( GetSimpleHeaderRevision(this) != revision + hadSortKey )
        return;
    wxCHECK_RET( idx < m_cols.size(),
                 "columns changed while updating sort indicator" );

    m_cols[idx].SetSortOrder(ascending);
    m_sortKey = idx;
    BumpSimpleHeaderRevision(this);

    UpdateColumn(idx);
}

void wxHeaderCtrlSimple::RemoveSortIndicator()
{
    if ( m_sortKey != wxNO_COLUMN )
    {
        const unsigned sortOld = m_sortKey;
        m_sortKey = wxNO_COLUMN;

        m_cols[sortOld].UnsetAsSortKey();
        BumpSimpleHeaderRevision(this);

        UpdateColumn(sortOld);
    }
}

bool
wxHeaderCtrlSimple::UpdateColumnWidthToFit(unsigned int idx, int widthTitle)
{
    if ( idx >= m_cols.size() )
        return false;

    wxHeaderCtrlSimple* const header = this;
    const wxWeakRef<wxWindow> weakHeader(header);
    const std::uint64_t revision = GetSimpleHeaderRevision(header);
    const wxHeaderColumnSimple* const column = &m_cols[idx];

    const int widthContents = GetBestFittingWidth(idx);
    if ( weakHeader.get() != header ||
            GetSimpleHeaderRevision(header) != revision ||
            idx >= m_cols.size() ||
            &m_cols[idx] != column ||
            widthContents == -1 )
    {
        return false;
    }

    m_cols[idx].SetWidth(wxMax(widthContents, widthTitle));
    BumpSimpleHeaderRevision(this);

    return true;
}

void
wxHeaderCtrlSimple::UpdateColumnVisibility(unsigned int idx, bool show)
{
    ShowColumn(idx, show);
}

void
wxHeaderCtrlSimple::UpdateColumnsOrder(const wxArrayInt& WXUNUSED(order))
{
    // Nothing to do here, we only override this function to prevent the base
    // class version from asserting that it should be implemented.
}

void wxHeaderCtrlSimple::OnHeaderResizing(wxHeaderCtrlEvent& evt)
{
    const int col = evt.GetColumn();
    wxCHECK_RET( col >= 0 &&
                     static_cast<size_t>(col) < m_cols.size(),
                 "columns changed while handling resize event" );

    m_cols[col].SetWidth(evt.GetWidth());
    BumpSimpleHeaderRevision(this);
    InvalidateBestSize();
    Refresh();
}

// ============================================================================
// wxHeaderCtrlEvent implementation
// ============================================================================

wxIMPLEMENT_DYNAMIC_CLASS(wxHeaderCtrlEvent, wxNotifyEvent);

wxDEFINE_EVENT( wxEVT_HEADER_CLICK, wxHeaderCtrlEvent);
wxDEFINE_EVENT( wxEVT_HEADER_RIGHT_CLICK, wxHeaderCtrlEvent);
wxDEFINE_EVENT( wxEVT_HEADER_MIDDLE_CLICK, wxHeaderCtrlEvent);

wxDEFINE_EVENT( wxEVT_HEADER_DCLICK, wxHeaderCtrlEvent);
wxDEFINE_EVENT( wxEVT_HEADER_RIGHT_DCLICK, wxHeaderCtrlEvent);
wxDEFINE_EVENT( wxEVT_HEADER_MIDDLE_DCLICK, wxHeaderCtrlEvent);

wxDEFINE_EVENT( wxEVT_HEADER_SEPARATOR_DCLICK, wxHeaderCtrlEvent);

wxDEFINE_EVENT( wxEVT_HEADER_BEGIN_RESIZE, wxHeaderCtrlEvent);
wxDEFINE_EVENT( wxEVT_HEADER_RESIZING, wxHeaderCtrlEvent);
wxDEFINE_EVENT( wxEVT_HEADER_END_RESIZE, wxHeaderCtrlEvent);

wxDEFINE_EVENT( wxEVT_HEADER_BEGIN_REORDER, wxHeaderCtrlEvent);
wxDEFINE_EVENT( wxEVT_HEADER_END_REORDER, wxHeaderCtrlEvent);

wxDEFINE_EVENT( wxEVT_HEADER_DRAGGING_CANCELLED, wxHeaderCtrlEvent);

#endif // wxUSE_HEADERCTRL
