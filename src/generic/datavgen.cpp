/////////////////////////////////////////////////////////////////////////////
// Name:        src/generic/datavgen.cpp
// Purpose:     wxDataViewCtrl generic implementation
// Author:      Robert Roebling
// Modified by: Francesco Montorsi, Guru Kathiresan, Bo Yang
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_DATAVIEWCTRL

#include "wx/dataview.h"

#ifdef wxUSE_GENERICDATAVIEWCTRL

#ifndef WX_PRECOMP
    #ifdef __WXMSW__
        #include "wx/app.h"          // GetRegisteredClassName()
        #include "wx/msw/private.h"
        #include "wx/msw/wrapwin.h"
        #include "wx/msw/wrapcctl.h" // include <commctrl.h> "properly"
    #endif
    #include "wx/sizer.h"
    #include "wx/log.h"
    #include "wx/dcclient.h"
    #include "wx/timer.h"
    #include "wx/settings.h"
    #include "wx/msgdlg.h"
    #include "wx/dcscreen.h"
    #include "wx/frame.h"
#endif

#include "wx/stockitem.h"
#include "wx/popupwin.h"
#include "wx/renderer.h"
#include "wx/dcbuffer.h"
#include "wx/icon.h"
#include "wx/itemattr.h"
#include "wx/list.h"
#include "wx/listimpl.cpp"
#include "wx/imaglist.h"
#include "wx/headerctrl.h"
#include "wx/dnd.h"
#include "wx/selstore.h"
#include "wx/stopwatch.h"
#include "wx/weakref.h"
#include "wx/generic/private/markuptext.h"
#include "wx/generic/private/widthcalc.h"
#if wxUSE_ACCESSIBILITY
#include "wx/private/markupparser.h"
#endif // wxUSE_ACCESSIBILITY

//-----------------------------------------------------------------------------
// classes
//-----------------------------------------------------------------------------

class wxDataViewColumn;
class wxDataViewHeaderWindow;
class wxDataViewCtrl;

//-----------------------------------------------------------------------------
// classes
//-----------------------------------------------------------------------------

static const int SCROLL_UNIT_X = 15;

// the cell padding on the left/right
static const int PADDING_RIGHTLEFT = 3;

// the expander space margin
static const int EXPANDER_MARGIN = 4;

#ifdef __WXMSW__
static const int EXPANDER_OFFSET = 4;
#else
static const int EXPANDER_OFFSET = 1;
#endif

// Below is the compare stuff.
// For the generic implementation, both the leaf nodes and the nodes are sorted for
// fast search when needed

// The column is either the index of the column to be used for sorting or one
// of the special values in this enum:
enum
{
    // Sort when we're thawed later.
    SortColumn_OnThaw = -3,

    // Don't sort at all.
    SortColumn_None = -2,

    // Sort using the model default sort order.
    SortColumn_Default = -1
};

// ----------------------------------------------------------------------------
// helper functions
// ----------------------------------------------------------------------------

namespace
{

// Return the expander column or, if it is not set, the first column and also
// set it as the expander one for the future.
wxDataViewColumn* GetExpanderColumnOrFirstOne(wxDataViewCtrl* dataview)
{
    wxDataViewColumn* expander = dataview->GetExpanderColumn();
    if (!expander)
    {
        // TODO-RTL: last column for RTL support
        expander = dataview->GetColumnAt( 0 );
        dataview->SetExpanderColumn(expander);
    }

    return expander;
}

wxTextCtrl *CreateEditorTextCtrl(wxWindow *parent, const wxRect& labelRect, const wxString& value)
{
    wxTextCtrl* ctrl = new wxTextCtrl(parent, wxID_ANY, value,
                                      labelRect.GetPosition(),
                                      labelRect.GetSize(),
                                      wxTE_PROCESS_ENTER);

    // Adjust size of wxTextCtrl editor to fit text, even if it means being
    // wider than the corresponding column (this is how Explorer behaves).
    const int fitting = ctrl->GetSizeFromTextSize(ctrl->GetTextExtent(ctrl->GetValue())).x;
    const int current = ctrl->GetSize().x;
    const int maxwidth = ctrl->GetParent()->GetSize().x - ctrl->GetPosition().x;

    // Adjust size so that it fits all content. Don't change anything if the
    // allocated space is already larger than needed and don't extend wxDVC's
    // boundaries.
    int width = wxMin(wxMax(current, fitting), maxwidth);

    if ( width != current )
        ctrl->SetSize(wxSize(width, -1));

    // select the text in the control an place the cursor at the end
    ctrl->SetInsertionPointEnd();
    ctrl->SelectAll();

    return ctrl;
}

} // anonymous namespace

//-----------------------------------------------------------------------------
// wxDataViewColumn
//-----------------------------------------------------------------------------

void wxDataViewColumn::Init(int width, wxAlignment align, int flags)
{
    m_width = width;
    m_minWidth = 0;
    m_align = align;
    m_flags = flags;
    m_sort = false;
    m_sortAscending = true;
}

int wxDataViewColumn::GetWidth() const
{
    switch ( m_width )
    {
        case wxCOL_WIDTH_DEFAULT:
            return wxDVC_DEFAULT_WIDTH;

        case wxCOL_WIDTH_AUTOSIZE:
            wxCHECK_MSG( m_owner, wxDVC_DEFAULT_WIDTH, "no owner control" );
            return m_owner->GetBestColumnWidth(m_owner->GetColumnIndex(this));

        default:
            return m_width;
    }
}

void wxDataViewColumn::UpdateDisplay()
{
    if (m_owner)
    {
        int idx = m_owner->GetColumnIndex( this );
        m_owner->OnColumnChange( idx );
    }
}

void wxDataViewColumn::UpdateWidth()
{
    if (m_owner)
    {
        int idx = m_owner->GetColumnIndex( this );
        m_owner->OnColumnWidthChange( idx );
    }
}

void wxDataViewColumn::UnsetAsSortKey()
{
    m_sort = false;

    if ( m_owner )
        m_owner->DontUseColumnForSorting(m_owner->GetColumnIndex(this));

    UpdateDisplay();
}

void wxDataViewColumn::SetSortOrder(bool ascending)
{
    if ( !m_owner )
        return;

    const int idx = m_owner->GetColumnIndex(this);

    // If this column isn't sorted already, mark it as sorted
    if ( !m_sort )
    {
        wxASSERT(!m_owner->IsColumnSorted(idx));

        // Now set this one as the new sort column.
        m_owner->UseColumnForSorting(idx);
        m_sort = true;
    }

   m_sortAscending = ascending;

    // Call this directly instead of using UpdateDisplay() as we already have
    // the column index, no need to look it up again.
    m_owner->OnColumnChange(idx);
}

//-----------------------------------------------------------------------------
// wxDataViewHeaderWindow
//-----------------------------------------------------------------------------

class wxDataViewHeaderWindow : public wxHeaderCtrl
{
public:
    wxDataViewHeaderWindow(wxDataViewCtrl *parent)
        : wxHeaderCtrl(parent)
    {
    }

    wxDataViewCtrl *GetOwner() const
        { return static_cast<wxDataViewCtrl *>(GetParent()); }

    // Add/Remove additional column to sorting columns
    void ToggleSortByColumn(int column)
    {
        wxDataViewCtrl * const owner = GetOwner();

        if ( !owner->IsMultiColumnSortAllowed() )
            return;

        wxDataViewColumn * const col = owner->GetColumn(column);
        if ( !col->IsSortable() )
            return;

        if ( owner->IsColumnSorted(column) )
        {
            col->UnsetAsSortKey();
            SendEvent(wxEVT_DATAVIEW_COLUMN_SORTED, column);
        }
        else // Do start sortign by it.
        {
            col->SetSortOrder(true);
            SendEvent(wxEVT_DATAVIEW_COLUMN_SORTED, column);
        }
    }

#if wxUSE_ACCESSIBILITY
    virtual wxAccessible* CreateAccessible() wxOVERRIDE
    {
        // Under MSW wxHeadrCtrl is a native control
        // so we just need to pass all requests
        // to the accessibility framework.
        return new wxAccessible(this);
    }
#endif // wxUSE_ACCESSIBILITY

protected:
    // implement/override wxHeaderCtrl functions by forwarding them to the main
    // control
    virtual const wxHeaderColumn& GetColumn(unsigned int idx) const wxOVERRIDE
    {
        return *(GetOwner()->GetColumn(idx));
    }

    virtual bool UpdateColumnWidthToFit(unsigned int idx, int widthTitle) wxOVERRIDE
    {
        wxDataViewCtrl * const owner = GetOwner();

        int widthContents = owner->GetBestColumnWidth(idx);
        owner->GetColumn(idx)->SetWidth(wxMax(widthTitle, widthContents));
        owner->OnColumnChange(idx);

        return true;
    }

private:
    void FinishEditing();

    bool SendEvent(wxEventType type, unsigned int n)
    {
        wxDataViewCtrl * const owner = GetOwner();
        wxDataViewEvent event(type, owner, owner->GetColumn(n));

        // for events created by wxDataViewHeaderWindow the
        // row / value fields are not valid
        return owner->ProcessWindowEvent(event);
    }

    void OnClick(wxHeaderCtrlEvent& event)
    {
        FinishEditing();

        const unsigned idx = event.GetColumn();

        if ( SendEvent(wxEVT_DATAVIEW_COLUMN_HEADER_CLICK, idx) )
            return;

        // default handling for the column click is to sort by this column or
        // toggle its sort order
        wxDataViewCtrl * const owner = GetOwner();
        wxDataViewColumn * const col = owner->GetColumn(idx);
        if ( !col->IsSortable() )
        {
            // no default handling for non-sortable columns
            event.Skip();
            return;
        }

        if ( col->IsSortKey() )
        {
            // already using this column for sorting, just change the order
            col->ToggleSortOrder();
        }
        else // not using this column for sorting yet
        {
            // We will sort by this column only now, so reset all the
            // previously used ones.
            owner->ResetAllSortColumns();

            // Sort the column
            col->SetSortOrder(true);
        }

        wxDataViewModel * const model = owner->GetModel();
        if ( model )
            model->Resort();

        owner->OnColumnChange(idx);

        SendEvent(wxEVT_DATAVIEW_COLUMN_SORTED, idx);
    }

    void OnRClick(wxHeaderCtrlEvent& event)
    {
        // Event wasn't processed somewhere, use default behaviour
        if ( !SendEvent(wxEVT_DATAVIEW_COLUMN_HEADER_RIGHT_CLICK,
                        event.GetColumn()) )
        {
            event.Skip();
            ToggleSortByColumn(event.GetColumn());
        }
    }

    void OnResize(wxHeaderCtrlEvent& event)
    {
        FinishEditing();

        wxDataViewCtrl * const owner = GetOwner();

        const unsigned col = event.GetColumn();
        owner->GetColumn(col)->SetWidth(event.GetWidth());
        GetOwner()->OnColumnChange(col);
    }

    void OnEndReorder(wxHeaderCtrlEvent& event)
    {
        FinishEditing();

        wxDataViewCtrl * const owner = GetOwner();
        owner->ColumnMoved(owner->GetColumn(event.GetColumn()),
                        event.GetNewOrder());
    }

    wxDECLARE_EVENT_TABLE();
    wxDECLARE_NO_COPY_CLASS(wxDataViewHeaderWindow);
};

wxBEGIN_EVENT_TABLE(wxDataViewHeaderWindow, wxHeaderCtrl)
    EVT_HEADER_CLICK(wxID_ANY, wxDataViewHeaderWindow::OnClick)
    EVT_HEADER_RIGHT_CLICK(wxID_ANY, wxDataViewHeaderWindow::OnRClick)

    EVT_HEADER_RESIZING(wxID_ANY, wxDataViewHeaderWindow::OnResize)
    EVT_HEADER_END_RESIZE(wxID_ANY, wxDataViewHeaderWindow::OnResize)

    EVT_HEADER_END_REORDER(wxID_ANY, wxDataViewHeaderWindow::OnEndReorder)
wxEND_EVENT_TABLE()

//-----------------------------------------------------------------------------
// wxDataViewRenameTimer
//-----------------------------------------------------------------------------

class wxDataViewRenameTimer: public wxTimer
{
private:
    wxDataViewMainWindow *m_owner;

public:
    wxDataViewRenameTimer( wxDataViewMainWindow *owner );
    void Notify() wxOVERRIDE;
};

//-----------------------------------------------------------------------------
// wxDataViewTreeNode
//-----------------------------------------------------------------------------

class wxDataViewMainWindow;
class wxDataViewTreeNode;
WX_DEFINE_ARRAY( wxDataViewTreeNode *, wxDataViewTreeNodes );

class wxDataViewTreeNode
{
public:
    wxDataViewTreeNode(wxDataViewMainWindow *window,
                       wxDataViewTreeNode *parent,
                       const wxDataViewItem& item)
        : m_window(window),
          m_parent(parent),
          m_item(item),
          m_branchData(NULL)
    {
    }

    ~wxDataViewTreeNode()
    {
        if ( m_branchData )
        {
            wxDataViewTreeNodes& nodes = m_branchData->children;
            for ( wxDataViewTreeNodes::iterator i = nodes.begin();
                  i != nodes.end();
                  ++i )
            {
                delete *i;
            }

            delete m_branchData;
        }
    }

    static wxDataViewTreeNode* CreateRootNode(wxDataViewMainWindow *w)
    {
        wxDataViewTreeNode *n = new wxDataViewTreeNode(w, NULL, wxDataViewItem());
        n->m_branchData = new BranchNodeData;
        n->m_branchData->open = true;
        return n;
    }

    wxDataViewTreeNode * GetParent() const { return m_parent; }

    const wxDataViewTreeNodes& GetChildNodes() const
    {
        wxASSERT( m_branchData != NULL );
        return m_branchData->children;
    }

    void InsertChild(wxDataViewTreeNode *node, unsigned index);

    void RemoveChild(wxDataViewTreeNode *node)
    {
        wxCHECK_RET( m_branchData != NULL, "leaf node doesn't have children" );
        m_branchData->children.Remove(node);
    }

    // returns position of child node for given item in children list or wxNOT_FOUND
    int FindChildByItem(const wxDataViewItem& item) const
    {
        if ( !m_branchData )
            return wxNOT_FOUND;

        const wxDataViewTreeNodes& nodes = m_branchData->children;
        const int len = nodes.size();
        for ( int i = 0; i < len; i++ )
        {
            if ( nodes[i]->m_item == item )
                return i;
        }
        return wxNOT_FOUND;
    }

    const wxDataViewItem & GetItem() const { return m_item; }
    void SetItem( const wxDataViewItem & item ) { m_item = item; }

    int GetIndentLevel() const
    {
        int ret = 0;
        const wxDataViewTreeNode * node = this;
        while( node->GetParent()->GetParent() != NULL )
        {
            node = node->GetParent();
            ret ++;
        }
        return ret;
    }

    bool IsOpen() const
    {
        return m_branchData && m_branchData->open;
    }

    void ToggleOpen()
    {
        // We do not allow the (invisible) root node to be collapsed because
        // there is no way to expand it again.
        if ( !m_parent )
            return;

        wxCHECK_RET( m_branchData != NULL, "can't open leaf node" );

        int sum = 0;

        const wxDataViewTreeNodes& nodes = m_branchData->children;
        const int len = nodes.GetCount();
        for ( int i = 0;i < len; i ++)
            sum += 1 + nodes[i]->GetSubTreeCount();

        if (m_branchData->open)
        {
            ChangeSubTreeCount(-sum);
            m_branchData->open = !m_branchData->open;
        }
        else
        {
            m_branchData->open = !m_branchData->open;
            ChangeSubTreeCount(+sum);
        }
    }

    // "HasChildren" property corresponds to model's IsContainer(). Note that it may be true
    // even if GetChildNodes() is empty; see below.
    bool HasChildren() const
    {
        return m_branchData != NULL;
    }

    void SetHasChildren(bool has)
    {
        // The invisible root item always has children, so ignore any attempts
        // to change this.
        if ( !m_parent )
            return;

        if ( !has )
        {
            wxDELETE(m_branchData);
        }
        else if ( m_branchData == NULL )
        {
            m_branchData = new BranchNodeData;
        }
    }

    int GetSubTreeCount() const
    {
        return m_branchData ? m_branchData->subTreeCount : 0;
    }

    void ChangeSubTreeCount( int num )
    {
        wxASSERT( m_branchData != NULL );

        if( !m_branchData->open )
            return;

        m_branchData->subTreeCount += num;
        wxASSERT( m_branchData->subTreeCount >= 0 );

        if( m_parent )
            m_parent->ChangeSubTreeCount(num);
    }

    void Resort();

private:
    wxDataViewMainWindow * const m_window;
    wxDataViewTreeNode  *m_parent;

    // Corresponding model item.
    wxDataViewItem       m_item;

    // Data specific to non-leaf (branch, inner) nodes. They are kept in a
    // separate struct in order to conserve memory.
    struct BranchNodeData
    {
        BranchNodeData()
            : open(false),
              subTreeCount(0)
        {
        }

        // Child nodes. Note that this may be empty even if m_hasChildren in
        // case this branch of the tree wasn't expanded and realized yet.
        wxDataViewTreeNodes  children;

        // Is the branch node currently open (expanded)?
        bool                 open;

        // Total count of expanded (i.e. visible with the help of some
        // scrolling) items in the subtree, but excluding this node. I.e. it is
        // 0 for leaves and is the number of rows the subtree occupies for
        // branch nodes.
        int                  subTreeCount;
    };

    BranchNodeData *m_branchData;
};



//-----------------------------------------------------------------------------
// wxDataViewMainWindow
//-----------------------------------------------------------------------------

WX_DEFINE_SORTED_ARRAY_SIZE_T(unsigned int, wxDataViewSelection);

class wxDataViewMainWindow: public wxWindow
{
public:
    wxDataViewMainWindow( wxDataViewCtrl *parent,
                            wxWindowID id,
                            const wxPoint &pos = wxDefaultPosition,
                            const wxSize &size = wxDefaultSize,
                            const wxString &name = wxT("wxdataviewctrlmainwindow") );
    virtual ~wxDataViewMainWindow();

    bool IsList() const { return GetModel()->IsListModel(); }
    bool IsVirtualList() const { return m_root == NULL; }

    // notifications from wxDataViewModel
    bool ItemAdded( const wxDataViewItem &parent, const wxDataViewItem &item );
    bool ItemDeleted( const wxDataViewItem &parent, const wxDataViewItem &item );
    bool ItemChanged( const wxDataViewItem &item );
    bool ValueChanged( const wxDataViewItem &item, unsigned int model_column );
    bool Cleared();
    void Resort()
    {
        if (!IsVirtualList())
        {
            SortPrepare();
            m_root->Resort();
        }
        UpdateDisplay();
    }

    // Override the base class method to resort if needed, i.e. if
    // SortPrepare() was called -- and ignored -- while we were frozen.
    virtual void DoThaw() wxOVERRIDE
    {
        if ( m_sortColumn == SortColumn_OnThaw )
        {
            Resort();
            m_sortColumn = SortColumn_None;
        }

        wxWindow::DoThaw();
    }

    void SortPrepare()
    {
        wxDataViewModel* model = GetModel();

        wxDataViewColumn* col = GetOwner()->GetSortingColumn();
        if( !col )
        {
            if (model->HasDefaultCompare())
            {
                // See below for the explanation of IsFrozen() test.
                if ( IsFrozen() )
                    m_sortColumn = SortColumn_OnThaw;
                else
                    m_sortColumn = SortColumn_Default;
            }
            else
                m_sortColumn = SortColumn_None;

            m_sortAscending = true;
            return;
        }

        // Avoid sorting while the window is frozen, this allows to quickly add
        // many items without resorting after each addition and only resort
        // them all at once when the window is finally thawed, see above.
        if ( IsFrozen() )
        {
            m_sortColumn = SortColumn_OnThaw;
            return;
        }

        m_sortColumn = col->GetModelColumn();
        m_sortAscending = col->IsSortOrderAscending();
    }

    void SetOwner( wxDataViewCtrl* owner ) { m_owner = owner; }
    wxDataViewCtrl *GetOwner() { return m_owner; }
    const wxDataViewCtrl *GetOwner() const { return m_owner; }

    wxDataViewModel* GetModel() { return GetOwner()->GetModel(); }
    const wxDataViewModel* GetModel() const { return GetOwner()->GetModel(); }

#if wxUSE_DRAG_AND_DROP
    wxBitmap CreateItemBitmap( unsigned int row, int &indent );
#endif // wxUSE_DRAG_AND_DROP
    void OnPaint( wxPaintEvent &event );
    void OnCharHook( wxKeyEvent &event );
    void OnChar( wxKeyEvent &event );
    void OnVerticalNavigation(const wxKeyEvent& event, int delta);
    void OnLeftKey(wxKeyEvent& event);
    void OnRightKey(wxKeyEvent& event);
    void OnMouse( wxMouseEvent &event );
    void OnSetFocus( wxFocusEvent &event );
    void OnKillFocus( wxFocusEvent &event );

    void UpdateDisplay();
    void RecalculateDisplay();
    void OnInternalIdle() wxOVERRIDE;

    void OnRenameTimer();

    void ScrollWindow( int dx, int dy, const wxRect *rect = NULL ) wxOVERRIDE;
    void ScrollTo( int rows, int column );

    unsigned GetCurrentRow() const { return m_currentRow; }
    bool HasCurrentRow() { return m_currentRow != (unsigned int)-1; }
    void ChangeCurrentRow( unsigned int row );
    bool TryAdvanceCurrentColumn(wxDataViewTreeNode *node, wxKeyEvent& event, bool forward);

    wxDataViewColumn *GetCurrentColumn() const { return m_currentCol; }
    void ClearCurrentColumn() { m_currentCol = NULL; }

    bool IsSingleSel() const { return !GetParent()->HasFlag(wxDV_MULTIPLE); }
    bool IsEmpty() { return GetRowCount() == 0; }

    int GetCountPerPage() const;
    int GetEndOfLastCol() const;
    unsigned int GetFirstVisibleRow() const;
    wxDataViewItem GetTopItem() const;

    // I change this method to un const because in the tree view,
    // the displaying number of the tree are changing along with the
    // expanding/collapsing of the tree nodes
    unsigned int GetLastVisibleRow();
    unsigned int GetLastFullyVisibleRow();
    unsigned int GetRowCount() const;

    const wxSelectionStore& GetSelections() const { return m_selection; }
    void ClearSelection() { m_selection.SelectRange(0, GetRowCount() - 1, false); }
    void Select( const wxArrayInt& aSelections );

    void SelectAllRows()
    {
        m_selection.SelectRange(0, GetRowCount() - 1);
        Refresh();
    }

    // If a valid row is specified and it was previously selected, it is left
    // selected and the function returns false. Otherwise, i.e. if there is
    // really no selection left in the control, it returns true.
    bool UnselectAllRows(unsigned int except = (unsigned int)-1);

    void SelectRow( unsigned int row, bool on );
    void SelectRows( unsigned int from, unsigned int to );
    void ReverseRowSelection( unsigned int row );
    bool IsRowSelected( unsigned int row );
    void SendSelectionChangedEvent( const wxDataViewItem& item);

    void RefreshRow( unsigned int row );
    void RefreshRows( unsigned int from, unsigned int to );
    void RefreshRowsAfter( unsigned int firstRow );

    // returns the colour to be used for drawing the rules
    wxColour GetRuleColour() const
    {
        return wxSystemSettings::GetColour(wxSYS_COLOUR_3DLIGHT);
    }

    wxRect GetLineRect( unsigned int row ) const;

    int GetLineStart( unsigned int row ) const;  // row * m_lineHeight in fixed mode
    int GetLineHeight( unsigned int row ) const; // m_lineHeight in fixed mode
    int GetLineAt( unsigned int y ) const;       // y / m_lineHeight in fixed mode

    void SetRowHeight( int lineHeight ) { m_lineHeight = lineHeight; }
    int GetRowHeight() const { return m_lineHeight; }
    int GetDefaultRowHeight() const;

    // Some useful functions for row and item mapping
    wxDataViewItem GetItemByRow( unsigned int row ) const;
    int GetRowByItem( const wxDataViewItem & item ) const;

    wxDataViewTreeNode * GetTreeNodeByRow( unsigned int row ) const;
    // We did not need this temporarily
    // wxDataViewTreeNode * GetTreeNodeByItem( const wxDataViewItem & item );

    // Methods for building the mapping tree
    void BuildTree( wxDataViewModel  * model );
    void DestroyTree();
    void HitTest( const wxPoint & point, wxDataViewItem & item, wxDataViewColumn* &column );
    wxRect GetItemRect( const wxDataViewItem & item, const wxDataViewColumn* column );

    void Expand( unsigned int row );
    void Collapse( unsigned int row );
    bool IsExpanded( unsigned int row ) const;
    bool HasChildren( unsigned int row ) const;

#if wxUSE_DRAG_AND_DROP
    bool EnableDragSource( const wxDataFormat &format );
    bool EnableDropTarget( const wxDataFormat &format );

    void RemoveDropHint();
    wxDragResult OnDragOver( wxDataFormat format, wxCoord x, wxCoord y, wxDragResult def );
    bool OnDrop( wxDataFormat format, wxCoord x, wxCoord y );
    wxDragResult OnData( wxDataFormat format, wxCoord x, wxCoord y, wxDragResult def );
    void OnLeave();
#endif // wxUSE_DRAG_AND_DROP

    void OnColumnsCountChanged();

    // Adjust last column to window size
    void UpdateColumnSizes();

    // Called by wxDataViewCtrl and our own OnRenameTimer() to start edit the
    // specified item in the given column.
    void StartEditing(const wxDataViewItem& item, const wxDataViewColumn* col);
    void FinishEditing();
    bool HasEditableColumn(const wxDataViewItem& item) const
    {
        return FindColumnForEditing(item, wxDATAVIEW_CELL_EDITABLE) != NULL;
    }

    int GetSortColumn() const { return m_sortColumn; }
    bool IsAscendingSort() const { return m_sortAscending; }

private:
    void InvalidateCount() { m_count = -1; }
    void UpdateCount(int count)
    {
        m_count = count;
        m_selection.SetItemCount(count);
    }

    int RecalculateCount() const;

    // Return false only if the event was vetoed by its handler.
    bool SendExpanderEvent(wxEventType type, const wxDataViewItem& item);

    wxDataViewTreeNode * FindNode( const wxDataViewItem & item );

    wxDataViewColumn *FindColumnForEditing(const wxDataViewItem& item, wxDataViewCellMode mode) const;

    bool IsCellEditableInMode(const wxDataViewItem& item, const wxDataViewColumn *col, wxDataViewCellMode mode) const;

    void DrawCellBackground( wxDataViewRenderer* cell, wxDC& dc, const wxRect& rect );

private:
    wxDataViewCtrl             *m_owner;
    int                         m_lineHeight;
    bool                        m_dirty;

    wxDataViewColumn           *m_currentCol;
    unsigned int                m_currentRow;
    wxSelectionStore            m_selection;

    wxDataViewRenameTimer      *m_renameTimer;
    bool                        m_lastOnSame;

    bool                        m_hasFocus;
    bool                        m_useCellFocus;
    bool                        m_currentColSetByKeyboard;

#if wxUSE_DRAG_AND_DROP
    int                         m_dragCount;
    wxPoint                     m_dragStart;

    bool                        m_dragEnabled;
    wxDataFormat                m_dragFormat;

    bool                        m_dropEnabled;
    wxDataFormat                m_dropFormat;
    bool                        m_dropHint;
    unsigned int                m_dropHintLine;
#endif // wxUSE_DRAG_AND_DROP

    // for double click logic
    unsigned int m_lineLastClicked,
        m_lineBeforeLastClicked,
        m_lineSelectSingleOnUp;

    // the pen used to draw horiz/vertical rules
    wxPen m_penRule;

    // the pen used to draw the expander and the lines
    wxPen m_penExpander;

    // This is the tree structure of the model
    wxDataViewTreeNode * m_root;
    int m_count;

    // This is the tree node under the cursor
    wxDataViewTreeNode * m_underMouse;

    // sorted column + extra flags
    int m_sortColumn;
    bool m_sortAscending;

    // The control used for editing or NULL.
    wxWeakRef<wxWindow> m_editorCtrl;

    // Id m_editorCtrl is non-NULL, pointer to the associated renderer.
    wxDataViewRenderer* m_editorRenderer;

private:
    wxDECLARE_DYNAMIC_CLASS(wxDataViewMainWindow);
    wxDECLARE_EVENT_TABLE();
};

// ---------------------------------------------------------
// wxGenericDataViewModelNotifier
// ---------------------------------------------------------

class wxGenericDataViewModelNotifier: public wxDataViewModelNotifier
{
public:
    wxGenericDataViewModelNotifier( wxDataViewMainWindow *mainWindow )
        { m_mainWindow = mainWindow; }

    virtual bool ItemAdded( const wxDataViewItem & parent, const wxDataViewItem & item ) wxOVERRIDE
        { return m_mainWindow->ItemAdded( parent , item ); }
    virtual bool ItemDeleted( const wxDataViewItem &parent, const wxDataViewItem &item ) wxOVERRIDE
        { return m_mainWindow->ItemDeleted( parent, item ); }
    virtual bool ItemChanged( const wxDataViewItem & item ) wxOVERRIDE
        { return m_mainWindow->ItemChanged(item);  }
    virtual bool ValueChanged( const wxDataViewItem & item , unsigned int col ) wxOVERRIDE
        { return m_mainWindow->ValueChanged( item, col ); }
    virtual bool Cleared() wxOVERRIDE
        { return m_mainWindow->Cleared(); }
    virtual void Resort() wxOVERRIDE
        { m_mainWindow->Resort(); }

    wxDataViewMainWindow    *m_mainWindow;
};

// ---------------------------------------------------------
// wxDataViewRenderer
// ---------------------------------------------------------

wxIMPLEMENT_ABSTRACT_CLASS(wxDataViewRenderer, wxDataViewRendererBase);

wxDataViewRenderer::wxDataViewRenderer( const wxString &varianttype,
                                        wxDataViewCellMode mode,
                                        int align) :
    wxDataViewCustomRendererBase( varianttype, mode, align )
{
    m_align = align;
    m_mode = mode;
    m_ellipsizeMode = wxELLIPSIZE_MIDDLE;
    m_dc = NULL;
    m_state = 0;
}

wxDataViewRenderer::~wxDataViewRenderer()
{
    delete m_dc;
}

wxDC *wxDataViewRenderer::GetDC()
{
    if (m_dc == NULL)
    {
        if (GetOwner() == NULL)
            return NULL;
        if (GetOwner()->GetOwner() == NULL)
            return NULL;
        m_dc = new wxClientDC( GetOwner()->GetOwner() );
    }

    return m_dc;
}

void wxDataViewRenderer::SetAlignment( int align )
{
    m_align=align;
}

int wxDataViewRenderer::GetAlignment() const
{
    return m_align;
}

// ---------------------------------------------------------
// wxDataViewCustomRenderer
// ---------------------------------------------------------

wxIMPLEMENT_ABSTRACT_CLASS(wxDataViewCustomRenderer, wxDataViewRenderer);

wxDataViewCustomRenderer::wxDataViewCustomRenderer( const wxString &varianttype,
                        wxDataViewCellMode mode, int align ) :
    wxDataViewRenderer( varianttype, mode, align )
{
}

#if wxUSE_ACCESSIBILITY
wxString wxDataViewCustomRenderer::GetAccessibleDescription() const
{
    wxVariant val;
    GetValue(val);

    wxString strVal;
    if ( val.IsType(wxS("bool")) )
    {
        /* TRANSLATORS: Name of Boolean true value */
        strVal = val.GetBool() ? _("true")
        /* TRANSLATORS: Name of Boolean false value */
                               : _("false");
    }
    else
    {
        strVal = val.MakeString();
    }

    return strVal;
}
#endif // wxUSE_ACCESSIBILITY

// ---------------------------------------------------------
// wxDataViewTextRenderer
// ---------------------------------------------------------

wxIMPLEMENT_CLASS(wxDataViewTextRenderer, wxDataViewRenderer);

wxDataViewTextRenderer::wxDataViewTextRenderer( const wxString &varianttype,
                                                wxDataViewCellMode mode, int align ) :
    wxDataViewRenderer( varianttype, mode, align )
{
#if wxUSE_MARKUP
    m_markupText = NULL;
#endif // wxUSE_MARKUP
}

wxDataViewTextRenderer::~wxDataViewTextRenderer()
{
#if wxUSE_MARKUP
    delete m_markupText;
#endif // wxUSE_MARKUP
}

#if wxUSE_MARKUP
void wxDataViewTextRenderer::EnableMarkup(bool enable)
{
    if ( enable )
    {
        if ( !m_markupText )
        {
            m_markupText = new wxItemMarkupText(wxString());
        }
    }
    else
    {
        if ( m_markupText )
        {
            delete m_markupText;
            m_markupText = NULL;
        }
    }
}
#endif // wxUSE_MARKUP

bool wxDataViewTextRenderer::SetValue( const wxVariant &value )
{
    m_text = value.GetString();

#if wxUSE_MARKUP
    if ( m_markupText )
        m_markupText->SetMarkup(m_text);
#endif // wxUSE_MARKUP

    return true;
}

bool wxDataViewTextRenderer::GetValue( wxVariant& WXUNUSED(value) ) const
{
    return false;
}

#if wxUSE_ACCESSIBILITY
wxString wxDataViewTextRenderer::GetAccessibleDescription() const
{
#if wxUSE_MARKUP
    if ( m_markupText )
        return wxMarkupParser::Strip(m_text);
#endif // wxUSE_MARKUP
    return m_text;
}
#endif // wxUSE_ACCESSIBILITY

bool wxDataViewTextRenderer::HasEditorCtrl() const
{
    return true;
}

wxWindow* wxDataViewTextRenderer::CreateEditorCtrl( wxWindow *parent,
        wxRect labelRect, const wxVariant &value )
{
    return CreateEditorTextCtrl(parent, labelRect, value);
}

bool wxDataViewTextRenderer::GetValueFromEditorCtrl( wxWindow *editor, wxVariant &value )
{
    wxTextCtrl *text = (wxTextCtrl*) editor;
    value = text->GetValue();
    return true;
}

bool wxDataViewTextRenderer::Render(wxRect rect, wxDC *dc, int state)
{
#if wxUSE_MARKUP
    if ( m_markupText )
    {
        int flags = 0;
        if ( state & wxDATAVIEW_CELL_SELECTED )
            flags |= wxCONTROL_SELECTED;
        m_markupText->Render(GetView(), *dc, rect, flags, GetEllipsizeMode());
    }
    else
#endif // wxUSE_MARKUP
        RenderText(m_text, 0, rect, dc, state);

    return true;
}

wxSize wxDataViewTextRenderer::GetSize() const
{
    if (!m_text.empty())
    {
#if wxUSE_MARKUP
        if ( m_markupText )
        {
            wxDataViewCtrl* const view = GetView();
            wxClientDC dc(view);
            if ( GetAttr().HasFont() )
                dc.SetFont(GetAttr().GetEffectiveFont(view->GetFont()));

            return m_markupText->Measure(dc);
        }
#endif // wxUSE_MARKUP

        return GetTextExtent(m_text);
    }
    else
        return wxSize(wxDVC_DEFAULT_RENDERER_SIZE,wxDVC_DEFAULT_RENDERER_SIZE);
}

// ---------------------------------------------------------
// wxDataViewBitmapRenderer
// ---------------------------------------------------------

wxIMPLEMENT_CLASS(wxDataViewBitmapRenderer, wxDataViewRenderer);

wxDataViewBitmapRenderer::wxDataViewBitmapRenderer( const wxString &varianttype,
                                                    wxDataViewCellMode mode, int align ) :
    wxDataViewRenderer( varianttype, mode, align )
{
}

bool wxDataViewBitmapRenderer::SetValue( const wxVariant &value )
{
    if (value.GetType() == wxT("wxBitmap"))
    {
        m_bitmap << value;
    }
    else if (value.GetType() == wxT("wxIcon"))
    {
        m_icon << value;
    }
    else
    {
        m_icon = wxNullIcon;
        m_bitmap = wxNullBitmap;
    }

    return true;
}

bool wxDataViewBitmapRenderer::GetValue( wxVariant& WXUNUSED(value) ) const
{
    return false;
}

#if wxUSE_ACCESSIBILITY
wxString wxDataViewBitmapRenderer::GetAccessibleDescription() const
{
    return wxEmptyString;
}
#endif // wxUSE_ACCESSIBILITY

bool wxDataViewBitmapRenderer::Render( wxRect cell, wxDC *dc, int WXUNUSED(state) )
{
    if (m_bitmap.IsOk())
        dc->DrawBitmap( m_bitmap, cell.x, cell.y, true /* use mask */ );
    else if (m_icon.IsOk())
        dc->DrawIcon( m_icon, cell.x, cell.y );

    return true;
}

wxSize wxDataViewBitmapRenderer::GetSize() const
{
    if (m_bitmap.IsOk())
        return wxSize( m_bitmap.GetWidth(), m_bitmap.GetHeight() );
    else if (m_icon.IsOk())
        return wxSize( m_icon.GetWidth(), m_icon.GetHeight() );

    return wxSize(wxDVC_DEFAULT_RENDERER_SIZE,wxDVC_DEFAULT_RENDERER_SIZE);
}

// ---------------------------------------------------------
// wxDataViewToggleRenderer
// ---------------------------------------------------------

wxIMPLEMENT_ABSTRACT_CLASS(wxDataViewToggleRenderer, wxDataViewRenderer);

wxDataViewToggleRenderer::wxDataViewToggleRenderer( const wxString &varianttype,
                        wxDataViewCellMode mode, int align ) :
    wxDataViewRenderer( varianttype, mode, align )
{
    m_toggle = false;
}

bool wxDataViewToggleRenderer::SetValue( const wxVariant &value )
{
    m_toggle = value.GetBool();

    return true;
}

bool wxDataViewToggleRenderer::GetValue( wxVariant &WXUNUSED(value) ) const
{
    return false;
}

#if wxUSE_ACCESSIBILITY
wxString wxDataViewToggleRenderer::GetAccessibleDescription() const
{
    /* TRANSLATORS: Checkbox state name */
    return m_toggle ? _("checked")
    /* TRANSLATORS: Checkbox state name */
                    : _("unchecked");
}
#endif // wxUSE_ACCESSIBILITY

bool wxDataViewToggleRenderer::Render( wxRect cell, wxDC *dc, int WXUNUSED(state) )
{
    int flags = 0;
    if (m_toggle)
        flags |= wxCONTROL_CHECKED;
    if (GetMode() != wxDATAVIEW_CELL_ACTIVATABLE ||
        GetEnabled() == false)
        flags |= wxCONTROL_DISABLED;

    // Ensure that the check boxes always have at least the minimal required
    // size, otherwise DrawCheckBox() doesn't really work well. If this size is
    // greater than the cell size, the checkbox will be truncated but this is a
    // lesser evil.
    wxSize size = cell.GetSize();
    size.IncTo(GetSize());
    cell.SetSize(size);

    wxRendererNative::Get().DrawCheckBox(
            GetOwner()->GetOwner(),
            *dc,
            cell,
            flags );

    return true;
}

bool wxDataViewToggleRenderer::WXActivateCell(const wxRect& WXUNUSED(cellRect),
                                              wxDataViewModel *model,
                                              const wxDataViewItem& item,
                                              unsigned int col,
                                              const wxMouseEvent *mouseEvent)
{
    if ( mouseEvent )
    {
        // Only react to clicks directly on the checkbox, not elsewhere in the
        // same cell.
        if ( !wxRect(GetSize()).Contains(mouseEvent->GetPosition()) )
            return false;
    }

    model->ChangeValue(!m_toggle, item, col);
    return true;
}

wxSize wxDataViewToggleRenderer::GetSize() const
{
    return wxRendererNative::Get().GetCheckBoxSize(GetView());
}

// ---------------------------------------------------------
// wxDataViewProgressRenderer
// ---------------------------------------------------------

wxIMPLEMENT_ABSTRACT_CLASS(wxDataViewProgressRenderer, wxDataViewRenderer);

wxDataViewProgressRenderer::wxDataViewProgressRenderer( const wxString &label,
    const wxString &varianttype, wxDataViewCellMode mode, int align ) :
    wxDataViewRenderer( varianttype, mode, align )
{
    m_label = label;
    m_value = 0;
}

bool wxDataViewProgressRenderer::SetValue( const wxVariant &value )
{
    m_value = (long) value;

    if (m_value < 0) m_value = 0;
    if (m_value > 100) m_value = 100;

    return true;
}

bool wxDataViewProgressRenderer::GetValue( wxVariant &value ) const
{
    value = (long) m_value;
    return true;
}

#if wxUSE_ACCESSIBILITY
wxString wxDataViewProgressRenderer::GetAccessibleDescription() const
{
    return wxString::Format(wxS("%i %%"), m_value);
}
#endif // wxUSE_ACCESSIBILITY

bool
wxDataViewProgressRenderer::Render(wxRect rect, wxDC *dc, int WXUNUSED(state))
{
    wxRendererNative::Get().DrawGauge(
        GetOwner()->GetOwner(),
        *dc,
        rect,
        m_value,
        100);

    return true;
}

wxSize wxDataViewProgressRenderer::GetSize() const
{
    // Return -1 width because a progress bar fits any width; unlike most
    // renderers, it doesn't have a "good" width for the content. This makes it
    // grow to the whole column, which is pretty much always the desired
    // behaviour. Keep the height fixed so that the progress bar isn't too fat.
    return wxSize(-1, 12);
}

// ---------------------------------------------------------
// wxDataViewIconTextRenderer
// ---------------------------------------------------------

wxIMPLEMENT_CLASS(wxDataViewIconTextRenderer, wxDataViewRenderer);

wxDataViewIconTextRenderer::wxDataViewIconTextRenderer(
const wxString &varianttype, wxDataViewCellMode mode, int align ) :
    wxDataViewRenderer( varianttype, mode, align )
{
    SetMode(mode);
    SetAlignment(align);
}

bool wxDataViewIconTextRenderer::SetValue( const wxVariant &value )
{
    m_value << value;
    return true;
}

bool wxDataViewIconTextRenderer::GetValue( wxVariant& WXUNUSED(value) ) const
{
    return false;
}

#if wxUSE_ACCESSIBILITY
wxString wxDataViewIconTextRenderer::GetAccessibleDescription() const
{
    return m_value.GetText();
}
#endif // wxUSE_ACCESSIBILITY

bool wxDataViewIconTextRenderer::Render(wxRect rect, wxDC *dc, int state)
{
    int xoffset = 0;

    const wxIcon& icon = m_value.GetIcon();
    if ( icon.IsOk() )
    {
        dc->DrawIcon(icon, rect.x, rect.y + (rect.height - icon.GetHeight())/2);
        xoffset = icon.GetWidth()+4;
    }

    RenderText(m_value.GetText(), xoffset, rect, dc, state);

    return true;
}

wxSize wxDataViewIconTextRenderer::GetSize() const
{
    if (!m_value.GetText().empty())
    {
        wxSize size = GetTextExtent(m_value.GetText());

        if (m_value.GetIcon().IsOk())
            size.x += m_value.GetIcon().GetWidth() + 4;
        return size;
    }
    return wxSize(80,20);
}

wxWindow* wxDataViewIconTextRenderer::CreateEditorCtrl(wxWindow *parent, wxRect labelRect, const wxVariant& value)
{
    wxDataViewIconText iconText;
    iconText << value;

    wxString text = iconText.GetText();

    // adjust the label rect to take the width of the icon into account
    if (iconText.GetIcon().IsOk())
    {
        int w = iconText.GetIcon().GetWidth() + 4;
        labelRect.x += w;
        labelRect.width -= w;
    }

    return CreateEditorTextCtrl(parent, labelRect, text);
}

bool wxDataViewIconTextRenderer::GetValueFromEditorCtrl( wxWindow *editor, wxVariant& value )
{
    wxTextCtrl *text = (wxTextCtrl*) editor;

    // The icon can't be edited so get its old value and reuse it.
    wxVariant valueOld;
    wxDataViewColumn* const col = GetOwner();
    GetView()->GetModel()->GetValue(valueOld, m_item, col->GetModelColumn());

    wxDataViewIconText iconText;
    iconText << valueOld;

    // But replace the text with the value entered by user.
    iconText.SetText(text->GetValue());

    value << iconText;
    return true;
}

//-----------------------------------------------------------------------------
// wxDataViewDropTarget
//-----------------------------------------------------------------------------

#if wxUSE_DRAG_AND_DROP

class wxBitmapCanvas: public wxWindow
{
public:
    wxBitmapCanvas( wxWindow *parent, const wxBitmap &bitmap, const wxSize &size ) :
    wxWindow( parent, wxID_ANY, wxPoint(0,0), size )
    {
        m_bitmap = bitmap;
        Connect( wxEVT_PAINT, wxPaintEventHandler(wxBitmapCanvas::OnPaint) );
    }

    void OnPaint( wxPaintEvent &WXUNUSED(event) )
    {
        wxPaintDC dc(this);
        dc.DrawBitmap( m_bitmap, 0, 0);
    }

    wxBitmap m_bitmap;
};

class wxDataViewDropSource: public wxDropSource
{
public:
    wxDataViewDropSource( wxDataViewMainWindow *win, unsigned int row ) :
        wxDropSource( win )
    {
        m_win = win;
        m_row = row;
        m_hint = NULL;
    }

    ~wxDataViewDropSource()
    {
        delete m_hint;
    }

    virtual bool GiveFeedback( wxDragResult WXUNUSED(effect) ) wxOVERRIDE
    {
        wxPoint pos = wxGetMousePosition();

        if (!m_hint)
        {
            int liney = m_win->GetLineStart( m_row );
            int linex = 0;
            m_win->GetOwner()->CalcUnscrolledPosition( 0, liney, NULL, &liney );
            m_win->ClientToScreen( &linex, &liney );
            m_dist_x = pos.x - linex;
            m_dist_y = pos.y - liney;

            int indent = 0;
            wxBitmap ib = m_win->CreateItemBitmap( m_row, indent );
            m_dist_x -= indent;
            m_hint = new wxFrame( m_win->GetParent(), wxID_ANY, wxEmptyString,
                                        wxPoint(pos.x - m_dist_x, pos.y + 5 ),
                                        ib.GetSize(),
                                        wxFRAME_TOOL_WINDOW |
                                        wxFRAME_FLOAT_ON_PARENT |
                                        wxFRAME_NO_TASKBAR |
                                        wxNO_BORDER );
            new wxBitmapCanvas( m_hint, ib, ib.GetSize() );
            m_hint->Show();
        }
        else
        {
            m_hint->Move( pos.x - m_dist_x, pos.y + 5  );
            m_hint->SetTransparent( 128 );
        }

        return false;
    }

    wxDataViewMainWindow   *m_win;
    unsigned int            m_row;
    wxFrame                *m_hint;
    int m_dist_x,m_dist_y;
};


class wxDataViewDropTarget: public wxDropTarget
{
public:
    wxDataViewDropTarget( wxDataObject *obj, wxDataViewMainWindow *win ) :
        wxDropTarget( obj )
    {
        m_win = win;
    }

    virtual wxDragResult OnDragOver( wxCoord x, wxCoord y, wxDragResult def ) wxOVERRIDE
    {
        wxDataFormat format = GetMatchingPair();
        if (format == wxDF_INVALID)
            return wxDragNone;
        return m_win->OnDragOver( format, x, y, def);
    }

    virtual bool OnDrop( wxCoord x, wxCoord y ) wxOVERRIDE
    {
        wxDataFormat format = GetMatchingPair();
        if (format == wxDF_INVALID)
            return false;
        return m_win->OnDrop( format, x, y );
    }

    virtual wxDragResult OnData( wxCoord x, wxCoord y, wxDragResult def ) wxOVERRIDE
    {
        wxDataFormat format = GetMatchingPair();
        if (format == wxDF_INVALID)
            return wxDragNone;
        if (!GetData())
            return wxDragNone;
        return m_win->OnData( format, x, y, def );
    }

    virtual void OnLeave() wxOVERRIDE
        { m_win->OnLeave(); }

    wxDataViewMainWindow   *m_win;
};

#endif // wxUSE_DRAG_AND_DROP

//-----------------------------------------------------------------------------
// wxDataViewRenameTimer
//-----------------------------------------------------------------------------

wxDataViewRenameTimer::wxDataViewRenameTimer( wxDataViewMainWindow *owner )
{
    m_owner = owner;
}

void wxDataViewRenameTimer::Notify()
{
    m_owner->OnRenameTimer();
}



// ----------------------------------------------------------------------------
// wxDataViewTreeNode
// ----------------------------------------------------------------------------

// Used by wxGenericTreeModelNodeCmp sort callback only.
//
// This is not thread safe but it should be fine if all events are handled in
// one thread.
static wxDataViewModel* g_model;
static int g_column;
static bool g_asending;

int LINKAGEMODE wxGenericTreeModelNodeCmp(wxDataViewTreeNode ** node1,
    wxDataViewTreeNode ** node2)
{
    return g_model->Compare((*node1)->GetItem(), (*node2)->GetItem(), g_column, g_asending);
}


void wxDataViewTreeNode::InsertChild(wxDataViewTreeNode *node, unsigned index)
{
    if (!m_branchData)
        m_branchData = new BranchNodeData;

    m_branchData->children.Insert(node, index);

    // TODO: insert into sorted array directly in O(log n) instead of resorting in O(n log n)
    if ((g_column = m_window->GetSortColumn()) >= -1)
    {
        g_model = m_window->GetModel();
        g_asending = m_window->IsAscendingSort();

        m_branchData->children.Sort(&wxGenericTreeModelNodeCmp);
    }
}

void wxDataViewTreeNode::Resort()
{
    if (!m_branchData)
        return;

    if ((g_column = m_window->GetSortColumn()) >= -1)
    {
        wxDataViewTreeNodes& nodes = m_branchData->children;

        g_model = m_window->GetModel();
        g_asending = m_window->IsAscendingSort();
        nodes.Sort(&wxGenericTreeModelNodeCmp);
        int len = nodes.GetCount();
        for (int i = 0; i < len; i++)
        {
            if (nodes[i]->HasChildren())
                nodes[i]->Resort();
        }
    }
}

//-----------------------------------------------------------------------------
// wxDataViewMainWindow
//-----------------------------------------------------------------------------

// The tree building helper, declared firstly
static void BuildTreeHelper(wxDataViewMainWindow *window,
                            const wxDataViewModel *model,
                            const wxDataViewItem & item,
                            wxDataViewTreeNode * node);

wxIMPLEMENT_ABSTRACT_CLASS(wxDataViewMainWindow, wxWindow);

wxBEGIN_EVENT_TABLE(wxDataViewMainWindow,wxWindow)
    EVT_PAINT         (wxDataViewMainWindow::OnPaint)
    EVT_MOUSE_EVENTS  (wxDataViewMainWindow::OnMouse)
    EVT_SET_FOCUS     (wxDataViewMainWindow::OnSetFocus)
    EVT_KILL_FOCUS    (wxDataViewMainWindow::OnKillFocus)
    EVT_CHAR_HOOK     (wxDataViewMainWindow::OnCharHook)
    EVT_CHAR          (wxDataViewMainWindow::OnChar)
wxEND_EVENT_TABLE()

wxDataViewMainWindow::wxDataViewMainWindow( wxDataViewCtrl *parent, wxWindowID id,
    const wxPoint &pos, const wxSize &size, const wxString &name )
{
    // We want to use a specific class name for this window in wxMSW to make it
    // possible to configure screen readers to handle it specifically.
#ifdef __WXMSW__
    CreateUsingMSWClass
    (
      wxApp::GetRegisteredClassName
             (
                  wxT("wxDataView"),
                  -1, // no specific background brush
                  0, // no special styles neither
                  wxApp::RegClass_OnlyNR
             ),
      parent, id, pos, size, wxWANTS_CHARS|wxBORDER_NONE, name
    );
#else
    Create( parent, id, pos, size, wxWANTS_CHARS|wxBORDER_NONE, name );
#endif

    SetOwner( parent );

    m_editorRenderer = NULL;

    m_lastOnSame = false;
    m_renameTimer = new wxDataViewRenameTimer( this );

    // TODO: user better initial values/nothing selected
    m_currentCol = NULL;
    m_currentColSetByKeyboard = false;
    m_useCellFocus = false;
    m_currentRow = (unsigned)-1;
    m_lineHeight = GetDefaultRowHeight();

#if wxUSE_DRAG_AND_DROP
    m_dragCount = 0;
    m_dragStart = wxPoint(0,0);

    m_dragEnabled = false;
    m_dropEnabled = false;
    m_dropHint = false;
    m_dropHintLine = (unsigned int) -1;
#endif // wxUSE_DRAG_AND_DROP

    m_lineLastClicked = (unsigned int) -1;
    m_lineBeforeLastClicked = (unsigned int) -1;
    m_lineSelectSingleOnUp = (unsigned int) -1;

    m_hasFocus = false;

    SetBackgroundColour( *wxWHITE );

    SetBackgroundStyle(wxBG_STYLE_CUSTOM);

    m_penRule = wxPen(GetRuleColour());

    // compose a pen whichcan draw black lines
    // TODO: maybe there is something system colour to use
    m_penExpander = wxPen(wxColour(0,0,0));

    m_root = wxDataViewTreeNode::CreateRootNode(this);

    // Make m_count = -1 will cause the class recaculate the real displaying number of rows.
    m_count = -1;
    m_underMouse = NULL;

    m_sortColumn = SortColumn_None;
    m_sortAscending = true;

    UpdateDisplay();
}

wxDataViewMainWindow::~wxDataViewMainWindow()
{
    DestroyTree();
    delete m_renameTimer;
}


int wxDataViewMainWindow::GetDefaultRowHeight() const
{
#ifdef __WXMSW__
    // We would like to use the same line height that Explorer uses. This is
    // different from standard ListView control since Vista.
    if ( wxGetWinVersion() >= wxWinVersion_Vista )
        return wxMax(16, GetCharHeight()) + 6; // 16 = mini icon height
    else
#endif // __WXMSW__
        return wxMax(16, GetCharHeight()) + 1; // 16 = mini icon height
}



#if wxUSE_DRAG_AND_DROP
bool wxDataViewMainWindow::EnableDragSource( const wxDataFormat &format )
{
    m_dragFormat = format;
    m_dragEnabled = format != wxDF_INVALID;

    return true;
}

bool wxDataViewMainWindow::EnableDropTarget( const wxDataFormat &format )
{
    m_dropFormat = format;
    m_dropEnabled = format != wxDF_INVALID;

    if (m_dropEnabled)
        SetDropTarget( new wxDataViewDropTarget( new wxCustomDataObject( format ), this ) );

    return true;
}

void wxDataViewMainWindow::RemoveDropHint()
{
    if (m_dropHint)
    {
            m_dropHint = false;
            RefreshRow( m_dropHintLine );
            m_dropHintLine = (unsigned int) -1;
    }
}

wxDragResult wxDataViewMainWindow::OnDragOver( wxDataFormat format, wxCoord x,
                                               wxCoord y, wxDragResult def )
{
    int xx = x;
    int yy = y;
    m_owner->CalcUnscrolledPosition( xx, yy, &xx, &yy );
    unsigned int row = GetLineAt( yy );

    wxDataViewItem item;

    if ( row < GetRowCount() && xx <= GetEndOfLastCol() )
        item = GetItemByRow( row );

    wxDataViewEvent event(wxEVT_DATAVIEW_ITEM_DROP_POSSIBLE, m_owner, item);
    event.SetDataFormat( format );
    event.SetDropEffect( def );
    if ( !m_owner->HandleWindowEvent( event ) || !event.IsAllowed() )
    {
        RemoveDropHint();
        return wxDragNone;
    }

    if ( item.IsOk() )
    {
        if (m_dropHint && (row != m_dropHintLine))
            RefreshRow( m_dropHintLine );
        m_dropHint = true;
        m_dropHintLine = row;
        RefreshRow( row );
    }
    else
    {
        RemoveDropHint();
    }

    return def;
}

bool wxDataViewMainWindow::OnDrop( wxDataFormat format, wxCoord x, wxCoord y )
{
    RemoveDropHint();

    int xx = x;
    int yy = y;
    m_owner->CalcUnscrolledPosition( xx, yy, &xx, &yy );
    unsigned int row = GetLineAt( yy );

    wxDataViewItem item;

    if ( row < GetRowCount() && xx <= GetEndOfLastCol())
       item = GetItemByRow( row );

    wxDataViewEvent event(wxEVT_DATAVIEW_ITEM_DROP_POSSIBLE, m_owner, item);
    event.SetDataFormat( format );
    if (!m_owner->HandleWindowEvent( event ) || !event.IsAllowed())
        return false;

    return true;
}

wxDragResult wxDataViewMainWindow::OnData( wxDataFormat format, wxCoord x, wxCoord y,
                                           wxDragResult def )
{
    int xx = x;
    int yy = y;
    m_owner->CalcUnscrolledPosition( xx, yy, &xx, &yy );
    unsigned int row = GetLineAt( yy );

    wxDataViewItem item;

    if ( row < GetRowCount() && xx <= GetEndOfLastCol() )
        item = GetItemByRow( row );

    wxCustomDataObject *obj = (wxCustomDataObject *) GetDropTarget()->GetDataObject();

    wxDataViewEvent event(wxEVT_DATAVIEW_ITEM_DROP, m_owner, item);
    event.SetDataFormat( format );
    event.SetDataSize( obj->GetSize() );
    event.SetDataBuffer( obj->GetData() );
    event.SetDropEffect( def );
    if ( !m_owner->HandleWindowEvent( event ) || !event.IsAllowed() )
        return wxDragNone;

    return def;
}

void wxDataViewMainWindow::OnLeave()
{
    RemoveDropHint();
}

wxBitmap wxDataViewMainWindow::CreateItemBitmap( unsigned int row, int &indent )
{
    int height = GetLineHeight( row );
    int width = 0;
    unsigned int cols = GetOwner()->GetColumnCount();
    unsigned int col;
    for (col = 0; col < cols; col++)
    {
        wxDataViewColumn *column = GetOwner()->GetColumnAt(col);
        if (column->IsHidden())
            continue;      // skip it!
        width += column->GetWidth();
    }

    indent = 0;
    if (!IsList())
    {
        wxDataViewTreeNode *node = GetTreeNodeByRow(row);
        indent = GetOwner()->GetIndent() * node->GetIndentLevel();
        indent = indent + m_lineHeight;
            // try to use the m_lineHeight as the expander space
    }
    width -= indent;

    wxBitmap bitmap( width, height );
    wxMemoryDC dc( bitmap );
    dc.SetFont( GetFont() );
    dc.SetPen( *wxBLACK_PEN );
    dc.SetBrush( *wxWHITE_BRUSH );
    dc.DrawRectangle( 0,0,width,height );

    wxDataViewModel *model = m_owner->GetModel();

    wxDataViewColumn * const
        expander = GetExpanderColumnOrFirstOne(GetOwner());

    int x = 0;
    for (col = 0; col < cols; col++)
    {
        wxDataViewColumn *column = GetOwner()->GetColumnAt( col );
        wxDataViewRenderer *cell = column->GetRenderer();

        if (column->IsHidden())
            continue;       // skip it!

        width = column->GetWidth();

        if (column == expander)
            width -= indent;

        wxDataViewItem item = GetItemByRow( row );
        cell->PrepareForItem(model, item, column->GetModelColumn());

        wxRect item_rect(x, 0, width, height);
        item_rect.Deflate(PADDING_RIGHTLEFT, 0);

        // dc.SetClippingRegion( item_rect );
        cell->WXCallRender(item_rect, &dc, 0);
        // dc.DestroyClippingRegion();

        x += width;
    }

    return bitmap;
}

#endif // wxUSE_DRAG_AND_DROP

void wxDataViewMainWindow::OnPaint( wxPaintEvent &WXUNUSED(event) )
{
    wxDataViewModel *model = GetModel();
    wxAutoBufferedPaintDC dc( this );

    const wxSize size = GetClientSize();

    dc.SetBrush(GetOwner()->GetBackgroundColour());
    dc.SetPen( *wxTRANSPARENT_PEN );
    dc.DrawRectangle(size);

    if ( IsEmpty() )
    {
        // No items to draw.
        return;
    }

    // prepare the DC
    GetOwner()->PrepareDC( dc );
    dc.SetFont( GetFont() );

    wxRect update = GetUpdateRegion().GetBox();
    m_owner->CalcUnscrolledPosition( update.x, update.y, &update.x, &update.y );

    // compute which items needs to be redrawn
    unsigned int item_start = GetLineAt( wxMax(0,update.y) );
    unsigned int item_count =
        wxMin( (int)(  GetLineAt( wxMax(0,update.y+update.height) ) - item_start + 1),
            (int)(GetRowCount( ) - item_start));
    unsigned int item_last = item_start + item_count;

    // Send the event to wxDataViewCtrl itself.
    wxDataViewEvent cache_event(wxEVT_DATAVIEW_CACHE_HINT, m_owner, NULL);
    cache_event.SetCache(item_start, item_last - 1);
    m_owner->ProcessWindowEvent(cache_event);

    // compute which columns needs to be redrawn
    unsigned int cols = GetOwner()->GetColumnCount();
    if ( !cols )
    {
        // we assume that we have at least one column below and painting an
        // empty control is unnecessary anyhow
        return;
    }

    unsigned int col_start = 0;
    unsigned int x_start;
    for (x_start = 0; col_start < cols; col_start++)
    {
        wxDataViewColumn *col = GetOwner()->GetColumnAt(col_start);
        if (col->IsHidden())
            continue;      // skip it!

        unsigned int w = col->GetWidth();
        if (x_start+w >= (unsigned int)update.x)
            break;

        x_start += w;
    }

    unsigned int col_last = col_start;
    unsigned int x_last = x_start;
    for (; col_last < cols; col_last++)
    {
        wxDataViewColumn *col = GetOwner()->GetColumnAt(col_last);
        if (col->IsHidden())
            continue;      // skip it!

        if (x_last > (unsigned int)update.GetRight())
            break;

        x_last += col->GetWidth();
    }

    // Instead of calling GetLineStart() for each line from the first to the
    // last one, we will compute the starts of the lines as we iterate over
    // them starting from this one, as this is much more efficient when using
    // wxDV_VARIABLE_LINE_HEIGHT (and doesn't really change anything when not
    // using it, so there is no need to use two different approaches).
    const unsigned int first_line_start = GetLineStart(item_start);

    // Draw background of alternate rows specially if required
    if ( m_owner->HasFlag(wxDV_ROW_LINES) )
    {
        wxColour altRowColour = m_owner->m_alternateRowColour;
        if ( !altRowColour.IsOk() )
        {
            // Determine the alternate rows colour automatically from the
            // background colour.
            const wxColour bgColour = m_owner->GetBackgroundColour();

            // Depending on the background, alternate row color
            // will be 3% more dark or 50% brighter.
            int alpha = bgColour.GetRGB() > 0x808080 ? 97 : 150;
            altRowColour = bgColour.ChangeLightness(alpha);
        }

        dc.SetPen(*wxTRANSPARENT_PEN);
        dc.SetBrush(wxBrush(altRowColour));

        // We only need to draw the visible part, so limit the rectangle to it.
        const int xRect = m_owner->CalcUnscrolledPosition(wxPoint(0, 0)).x;
        const int widthRect = size.x;
        unsigned int cur_line_start = first_line_start;
        for (unsigned int item = item_start; item < item_last; item++)
        {
            const int h = GetLineHeight(item);
            if ( item % 2 )
            {
                dc.DrawRectangle(xRect, cur_line_start, widthRect, h);
            }
            cur_line_start += h;
        }
    }

    // Draw horizontal rules if required
    if ( m_owner->HasFlag(wxDV_HORIZ_RULES) )
    {
        dc.SetPen(m_penRule);
        dc.SetBrush(*wxTRANSPARENT_BRUSH);

        unsigned int cur_line_start = first_line_start;
        for (unsigned int i = item_start; i <= item_last; i++)
        {
            const int h = GetLineHeight(i);
            dc.DrawLine(x_start, cur_line_start, x_last, cur_line_start);
            cur_line_start += h;
        }
    }

    // Draw vertical rules if required
    if ( m_owner->HasFlag(wxDV_VERT_RULES) )
    {
        dc.SetPen(m_penRule);
        dc.SetBrush(*wxTRANSPARENT_BRUSH);

        // NB: Vertical rules are drawn in the last pixel of a column so that
        //     they align perfectly with native MSW wxHeaderCtrl as well as for
        //     consistency with MSW native list control. There's no vertical
        //     rule at the most-left side of the control.

        int x = x_start - 1;
        int line_last = GetLineStart(item_last);
        for (unsigned int i = col_start; i < col_last; i++)
        {
            wxDataViewColumn *col = GetOwner()->GetColumnAt(i);
            if (col->IsHidden())
                continue;       // skip it

            x += col->GetWidth();

            dc.DrawLine(x, first_line_start,
                        x, line_last);
        }
    }

    // redraw the background for the items which are selected/current
    unsigned int cur_line_start = first_line_start;
    for (unsigned int item = item_start; item < item_last; item++)
    {
        bool selected = m_selection.IsSelected(item);
        const int line_height = GetLineHeight(item);

        if (selected || item == m_currentRow)
        {
            wxRect rowRect( x_start, cur_line_start,
                            x_last - x_start, line_height );

            bool renderColumnFocus = false;

            int flags = wxCONTROL_SELECTED;
            if ( m_hasFocus )
                flags |= wxCONTROL_FOCUSED;

            // draw keyboard focus rect if applicable
            if ( item == m_currentRow && m_hasFocus )
            {

                if ( m_useCellFocus && m_currentCol && m_currentColSetByKeyboard )
                {
                    renderColumnFocus = true;

                    // If this is container node without columns, render full-row focus:
                    if ( !IsList() )
                    {
                        wxDataViewTreeNode *node = GetTreeNodeByRow(item);
                        if ( node->HasChildren() && !model->HasContainerColumns(node->GetItem()) )
                            renderColumnFocus = false;
                    }
                }

                if ( renderColumnFocus )
                {
                    wxRect colRect(rowRect);

                    for ( unsigned int i = col_start; i < col_last; i++ )
                    {
                        wxDataViewColumn *col = GetOwner()->GetColumnAt(i);
                        if ( col->IsHidden() )
                            continue;

                        colRect.width = col->GetWidth();

                        if ( col == m_currentCol )
                        {
                            // Draw selection rect left of column
                            {
                                wxRect clipRect(rowRect);
                                clipRect.width = colRect.x;

                                wxDCClipper clip(dc, clipRect);
                                wxRendererNative::Get().DrawItemSelectionRect
                                    (
                                    this,
                                    dc,
                                    rowRect,
                                    flags
                                    );
                            }

                            // Draw selection rect right of column
                            {
                                wxRect clipRect(rowRect);
                                clipRect.x = colRect.x + colRect.width;
                                clipRect.width = rowRect.width - clipRect.x;

                                wxDCClipper clip(dc, clipRect);
                                wxRendererNative::Get().DrawItemSelectionRect
                                    (
                                    this,
                                    dc,
                                    rowRect,
                                    flags
                                    );
                            }

                            // Draw column selection rect
                            wxRendererNative::Get().DrawItemSelectionRect
                                (
                                this,
                                dc,
                                colRect,
                                flags | wxCONTROL_CURRENT | wxCONTROL_CELL
                                );

                            break;
                        }

                        colRect.x += colRect.width;
                    }
                }
            }

            // draw selection and whole-item focus:
            if ( selected && !renderColumnFocus )
            {
                wxRendererNative::Get().DrawItemSelectionRect
                    (
                    this,
                    dc,
                    rowRect,
                    flags | wxCONTROL_CURRENT
                    );
            }
        }
        cur_line_start += line_height;
    }

#if wxUSE_DRAG_AND_DROP
    if (m_dropHint)
    {
        wxRect rect( x_start, GetLineStart( m_dropHintLine ),
                     x_last - x_start, GetLineHeight( m_dropHintLine ) );
        dc.SetPen( *wxBLACK_PEN );
        dc.SetBrush( *wxTRANSPARENT_BRUSH );
        dc.DrawRectangle( rect );
    }
#endif // wxUSE_DRAG_AND_DROP

    wxDataViewColumn * const
        expander = GetExpanderColumnOrFirstOne(GetOwner());

    // redraw all cells for all rows which must be repainted and all columns
    wxRect cell_rect;
    cell_rect.x = x_start;
    for (unsigned int i = col_start; i < col_last; i++)
    {
        wxDataViewColumn *col = GetOwner()->GetColumnAt( i );
        wxDataViewRenderer *cell = col->GetRenderer();
        cell_rect.width = col->GetWidth();

        if ( col->IsHidden() || cell_rect.width <= 0 )
            continue;       // skip it!

        cell_rect.y = first_line_start;
        for (unsigned int item = item_start; item < item_last; item++)
        {
            // get the cell value and set it into the renderer
            wxDataViewTreeNode *node = NULL;
            wxDataViewItem dataitem;
            const int line_height = GetLineHeight(item);

            if (!IsVirtualList())
            {
                node = GetTreeNodeByRow(item);
                if (node == NULL)
                {
                    cell_rect.y += line_height;
                    continue;
                }

                dataitem = node->GetItem();

                // Skip all columns of "container" rows except the expander
                // column itself unless HasContainerColumns() overrides this.
                if ( col != expander &&
                        model->IsContainer(dataitem) &&
                            !model->HasContainerColumns(dataitem) )
                {
                    cell_rect.y += line_height;
                    continue;
                }
            }
            else
            {
                dataitem = wxDataViewItem( wxUIntToPtr(item+1) );
            }

            // update cell_rect
            cell_rect.height = line_height;

            bool selected = m_selection.IsSelected(item);

            int state = 0;
            if (m_hasFocus && selected)
                state |= wxDATAVIEW_CELL_SELECTED;

            cell->SetState(state);
            cell->PrepareForItem(model, dataitem, col->GetModelColumn());

            // draw the background
            if ( !selected )
                DrawCellBackground( cell, dc, cell_rect );

            // deal with the expander
            int indent = 0;
            if ((!IsList()) && (col == expander))
            {
                // Calculate the indent first
                indent = GetOwner()->GetIndent() * node->GetIndentLevel();

                // we reserve m_lineHeight of horizontal space for the expander
                // but leave EXPANDER_MARGIN around the expander itself
                int exp_x = cell_rect.x + indent + EXPANDER_MARGIN;

                indent += m_lineHeight;

                // draw expander if needed and visible
                if ( node->HasChildren() && exp_x < cell_rect.GetRight() )
                {
                    dc.SetPen( m_penExpander );
                    dc.SetBrush( wxNullBrush );

                    int exp_size = m_lineHeight - 2*EXPANDER_MARGIN;
                    int exp_y = cell_rect.y + (cell_rect.height - exp_size)/2
                                   + EXPANDER_MARGIN - EXPANDER_OFFSET;

                    const wxRect rect(exp_x, exp_y, exp_size, exp_size);

                    int flag = 0;
                    if ( m_underMouse == node )
                        flag |= wxCONTROL_CURRENT;
                    if ( node->IsOpen() )
                        flag |= wxCONTROL_EXPANDED;

                    // ensure that we don't overflow the cell (which might
                    // happen if the column is very narrow)
                    wxDCClipper clip(dc, cell_rect);

                    wxRendererNative::Get().DrawTreeItemButton( this, dc, rect, flag);
                }

                // force the expander column to left-center align
                cell->SetAlignment( wxALIGN_CENTER_VERTICAL );
            }

            wxRect item_rect = cell_rect;
            item_rect.Deflate(PADDING_RIGHTLEFT, 0);

            // account for the tree indent (harmless if we're not indented)
            item_rect.x += indent;
            item_rect.width -= indent;

            if ( item_rect.width <= 0 )
            {
                cell_rect.y += line_height;
                continue;
            }

            // TODO: it would be much more efficient to create a clipping
            //       region for the entire column being rendered (in the OnPaint
            //       of wxDataViewMainWindow) instead of a single clip region for
            //       each cell. However it would mean that each renderer should
            //       respect the given wxRect's top & bottom coords, eventually
            //       violating only the left & right coords - however the user can
            //       make its own renderer and thus we cannot be sure of that.
            wxDCClipper clip(dc, item_rect);

            cell->WXCallRender(item_rect, &dc, state);

            cell_rect.y += line_height;
        }

        cell_rect.x += cell_rect.width;
    }
}


void wxDataViewMainWindow::DrawCellBackground( wxDataViewRenderer* cell, wxDC& dc, const wxRect& rect )
{
    wxRect rectBg( rect );

    // don't overlap the horizontal rules
    if ( m_owner->HasFlag(wxDV_HORIZ_RULES) )
    {
        rectBg.y++;
        rectBg.height--;
    }

    // don't overlap the vertical rules
    if ( m_owner->HasFlag(wxDV_VERT_RULES) )
    {
        // same note as in OnPaint handler above
        // NB: Vertical rules are drawn in the last pixel of a column so that
        //     they align perfectly with native MSW wxHeaderCtrl as well as for
        //     consistency with MSW native list control. There's no vertical
        //     rule at the most-left side of the control.
        rectBg.width--;
    }

    cell->RenderBackground(&dc, rectBg);
}

void wxDataViewMainWindow::OnRenameTimer()
{
    // We have to call this here because changes may just have
    // been made and no screen update taken place.
    if ( m_dirty )
    {
        // TODO: use wxTheApp->SafeYieldFor(NULL, wxEVT_CATEGORY_UI) instead
        //       (needs to be tested!)
        wxSafeYield();
    }

    wxDataViewItem item = GetItemByRow( m_currentRow );

    StartEditing( item, m_currentCol );
}

void
wxDataViewMainWindow::StartEditing(const wxDataViewItem& item,
                                   const wxDataViewColumn* col)
{
    wxDataViewRenderer* renderer = col->GetRenderer();
    if ( !IsCellEditableInMode(item, col, wxDATAVIEW_CELL_EDITABLE) )
        return;

    const wxRect itemRect = GetItemRect(item, col);
    if ( renderer->StartEditing(item, itemRect) )
    {
        renderer->NotifyEditingStarted(item);

        // Save the renderer to be able to finish/cancel editing it later and
        // save the control to be able to detect if we're still editing it.
        m_editorRenderer = renderer;
        m_editorCtrl = renderer->GetEditorCtrl();
    }
}

void wxDataViewMainWindow::FinishEditing()
{
    if ( m_editorCtrl )
    {
        m_editorRenderer->FinishEditing();
    }
}

void wxDataViewHeaderWindow::FinishEditing()
{
    wxDataViewMainWindow *win = static_cast<wxDataViewMainWindow*>(GetOwner()->GetMainWindow());
    win->FinishEditing();
}

//-----------------------------------------------------------------------------
// Helper class for do operation on the tree node
//-----------------------------------------------------------------------------
class DoJob
{
public:
    DoJob() { }
    virtual ~DoJob() { }

    // The return value control how the tree-walker tranverse the tree
    enum
    {
        DONE,          // Job done, stop traversing and return
        SKIP_SUBTREE,  // Ignore the current node's subtree and continue
        CONTINUE       // Job not done, continue
    };

    virtual int operator() ( wxDataViewTreeNode * node ) = 0;
};

bool Walker( wxDataViewTreeNode * node, DoJob & func )
{
    wxCHECK_MSG( node, false, "can't walk NULL node" );

    switch( func( node ) )
    {
        case DoJob::DONE:
            return true;
        case DoJob::SKIP_SUBTREE:
            return false;
        case DoJob::CONTINUE:
            break;
    }

    if ( node->HasChildren() )
    {
        const wxDataViewTreeNodes& nodes = node->GetChildNodes();

        for ( wxDataViewTreeNodes::const_iterator i = nodes.begin();
              i != nodes.end();
              ++i )
        {
            if ( Walker(*i, func) )
                return true;
        }
    }

    return false;
}

bool wxDataViewMainWindow::ItemAdded(const wxDataViewItem & parent, const wxDataViewItem & item)
{
    if (IsVirtualList())
    {
        wxDataViewVirtualListModel *list_model =
            (wxDataViewVirtualListModel*) GetModel();
        m_count = list_model->GetCount();

        m_selection.OnItemsInserted(GetRowByItem(item), 1);
    }
    else
    {
        SortPrepare();

        wxDataViewTreeNode *parentNode = FindNode(parent);

        if ( !parentNode )
            return false;

        wxDataViewItemArray modelSiblings;
        GetModel()->GetChildren(parent, modelSiblings);
        const int modelSiblingsSize = modelSiblings.size();

        int posInModel = modelSiblings.Index(item, /*fromEnd=*/true);
        wxCHECK_MSG( posInModel != wxNOT_FOUND, false, "adding non-existent item?" );

        wxDataViewTreeNode *itemNode = new wxDataViewTreeNode(this, parentNode, item);
        itemNode->SetHasChildren(GetModel()->IsContainer(item));

        parentNode->SetHasChildren(true);

        const wxDataViewTreeNodes& nodeSiblings = parentNode->GetChildNodes();
        const int nodeSiblingsSize = nodeSiblings.size();

        int nodePos = 0;

        if ( posInModel == modelSiblingsSize - 1 )
        {
            nodePos = nodeSiblingsSize;
        }
        else if ( modelSiblingsSize == nodeSiblingsSize + 1 )
        {
            // This is the simple case when our node tree already matches the
            // model and only this one item is missing.
            nodePos = posInModel;
        }
        else
        {
            // It's possible that a larger discrepancy between the model and
            // our realization exists. This can happen e.g. when adding a bunch
            // of items to the model and then calling ItemsAdded() just once
            // afterwards. In this case, we must find the right position by
            // looking at sibling items.

            // append to the end if we won't find a better position:
            nodePos = nodeSiblingsSize;

            for ( int nextItemPos = posInModel + 1;
                  nextItemPos < modelSiblingsSize;
                  nextItemPos++ )
            {
                int nextNodePos = parentNode->FindChildByItem(modelSiblings[nextItemPos]);
                if ( nextNodePos != wxNOT_FOUND )
                {
                    nodePos = nextNodePos;
                    break;
                }
            }
        }

        parentNode->ChangeSubTreeCount(+1);
        parentNode->InsertChild(itemNode, nodePos);

        InvalidateCount();
    }

    GetOwner()->InvalidateColBestWidths();
    UpdateDisplay();

    return true;
}

bool wxDataViewMainWindow::ItemDeleted(const wxDataViewItem& parent,
                                       const wxDataViewItem& item)
{
    if (IsVirtualList())
    {
        wxDataViewVirtualListModel *list_model =
            (wxDataViewVirtualListModel*) GetModel();
        m_count = list_model->GetCount();

        m_selection.OnItemDelete(GetRowByItem(item));
    }
    else // general case
    {
        wxDataViewTreeNode *parentNode = FindNode(parent);

        // Notice that it is possible that the item being deleted is not in the
        // tree at all, for example we could be deleting a never shown (because
        // collapsed) item in a tree model. So it's not an error if we don't know
        // about this item, just return without doing anything then.
        if ( !parentNode )
            return true;

        wxCHECK_MSG( parentNode->HasChildren(), false, "parent node doesn't have children?" );
        const wxDataViewTreeNodes& parentsChildren = parentNode->GetChildNodes();

        // We can't use FindNode() to find 'item', because it was already
        // removed from the model by the time ItemDeleted() is called, so we
        // have to do it manually. We keep track of its position as well for
        // later use.
        int itemPosInNode = 0;
        wxDataViewTreeNode *itemNode = NULL;
        for ( wxDataViewTreeNodes::const_iterator i = parentsChildren.begin();
              i != parentsChildren.end();
              ++i, ++itemPosInNode )
        {
            if( (*i)->GetItem() == item )
            {
                itemNode = *i;
                break;
            }
        }

        // If the parent wasn't expanded, it's possible that we didn't have a
        // node corresponding to 'item' and so there's nothing left to do.
        if ( !itemNode )
        {
            // If this was the last child to be removed, it's possible the parent
            // node became a leaf. Let's ask the model about it.
            if ( parentNode->GetChildNodes().empty() )
                parentNode->SetHasChildren(GetModel()->IsContainer(parent));

            return true;
        }

        // Delete the item from wxDataViewTreeNode representation:
        const int itemsDeleted = 1 + itemNode->GetSubTreeCount();

        parentNode->RemoveChild(itemNode);
        delete itemNode;
        parentNode->ChangeSubTreeCount(-itemsDeleted);

        // Make the row number invalid and get a new valid one when user call GetRowCount
        InvalidateCount();

        // If this was the last child to be removed, it's possible the parent
        // node became a leaf. Let's ask the model about it.
        if ( parentNode->GetChildNodes().empty() )
        {
            bool isContainer = GetModel()->IsContainer(parent);
            parentNode->SetHasChildren(isContainer);
            if ( isContainer )
            {
                // If it's still a container, make sure we show "+" icon for it
                // and not "-" one as there is nothing to collapse any more.
                if ( parentNode->IsOpen() )
                    parentNode->ToggleOpen();
            }
        }

        // Update selection by removing 'item' and its entire children tree from the selection.
        if ( !m_selection.IsEmpty() )
        {
            // we can't call GetRowByItem() on 'item', as it's already deleted, so compute it from
            // the parent ('parentNode') and position in its list of children
            int itemRow;
            if ( itemPosInNode == 0 )
            {
                // 1st child, row number is that of the parent parentNode + 1
                itemRow = GetRowByItem(parentNode->GetItem()) + 1;
            }
            else
            {
                // row number is that of the sibling above 'item' + its subtree if any + 1
                const wxDataViewTreeNode *siblingNode = parentNode->GetChildNodes()[itemPosInNode - 1];

                itemRow = GetRowByItem(siblingNode->GetItem()) +
                          siblingNode->GetSubTreeCount() +
                          1;
            }

            m_selection.OnItemsDeleted(itemRow, itemsDeleted);
        }
    }

    // Change the current row to the last row if the current exceed the max row number
    if ( m_currentRow >= GetRowCount() )
        ChangeCurrentRow(m_count - 1);

    GetOwner()->InvalidateColBestWidths();
    UpdateDisplay();

    return true;
}

bool wxDataViewMainWindow::ItemChanged(const wxDataViewItem & item)
{
    SortPrepare();
    GetModel()->Resort();

    GetOwner()->InvalidateColBestWidths();

    // Send event
    wxDataViewEvent le(wxEVT_DATAVIEW_ITEM_VALUE_CHANGED, m_owner, item);
    m_owner->ProcessWindowEvent(le);

    return true;
}

bool wxDataViewMainWindow::ValueChanged( const wxDataViewItem & item, unsigned int model_column )
{
    int view_column = m_owner->GetModelColumnIndex(model_column);
    if ( view_column == wxNOT_FOUND )
        return false;

    // NOTE: to be valid, we cannot use e.g. INT_MAX - 1
/*#define MAX_VIRTUAL_WIDTH       100000

    wxRect rect( 0, row*m_lineHeight, MAX_VIRTUAL_WIDTH, m_lineHeight );
    m_owner->CalcScrolledPosition( rect.x, rect.y, &rect.x, &rect.y );
    Refresh( true, &rect );

    return true;
*/
    SortPrepare();
    GetModel()->Resort();

    GetOwner()->InvalidateColBestWidth(view_column);

    // Send event
    wxDataViewColumn* const column = m_owner->GetColumn(view_column);
    wxDataViewEvent le(wxEVT_DATAVIEW_ITEM_VALUE_CHANGED, m_owner, column, item);
    m_owner->ProcessWindowEvent(le);

    return true;
}

bool wxDataViewMainWindow::Cleared()
{
    DestroyTree();
    m_selection.Clear();
    m_currentRow = (unsigned)-1;

    if (GetModel())
    {
        SortPrepare();
        BuildTree( GetModel() );
    }
    else
    {
        m_count = 0;
    }

    GetOwner()->InvalidateColBestWidths();
    UpdateDisplay();

    return true;
}

void wxDataViewMainWindow::UpdateDisplay()
{
    m_dirty = true;
    m_underMouse = NULL;
}

void wxDataViewMainWindow::OnInternalIdle()
{
    wxWindow::OnInternalIdle();

    if (m_dirty)
    {
        UpdateColumnSizes();
        RecalculateDisplay();
        m_dirty = false;
    }
}

void wxDataViewMainWindow::RecalculateDisplay()
{
    wxDataViewModel *model = GetModel();
    if (!model)
    {
        Refresh();
        return;
    }

    int width = GetEndOfLastCol();
    int height = GetLineStart( GetRowCount() );

    SetVirtualSize( width, height );
    GetOwner()->SetScrollRate( 10, m_lineHeight );

    Refresh();
}

void wxDataViewMainWindow::ScrollWindow( int dx, int dy, const wxRect *rect )
{
    m_underMouse = NULL;

    wxWindow::ScrollWindow( dx, dy, rect );

    if (GetOwner()->m_headerArea)
        GetOwner()->m_headerArea->ScrollWindow( dx, 0 );
}

void wxDataViewMainWindow::ScrollTo( int rows, int column )
{
    m_underMouse = NULL;

    int x, y;
    m_owner->GetScrollPixelsPerUnit( &x, &y );
    int sy = GetLineStart( rows )/y;
    int sx = -1;
    if( column != -1 )
    {
        wxRect rect = GetClientRect();
        int colnum = 0;
        int x_start, w = 0;
        int xx, yy, xe;
        m_owner->CalcUnscrolledPosition( rect.x, rect.y, &xx, &yy );
        for (x_start = 0; colnum < column; colnum++)
        {
            wxDataViewColumn *col = GetOwner()->GetColumnAt(colnum);
            if (col->IsHidden())
                continue;      // skip it!

            w = col->GetWidth();
            x_start += w;
        }

        int x_end = x_start + w;
        xe = xx + rect.width;
        if( x_end > xe )
        {
            sx = ( xx + x_end - xe )/x;
        }
        if( x_start < xx )
        {
            sx = x_start/x;
        }
    }
    m_owner->Scroll( sx, sy );
}

int wxDataViewMainWindow::GetCountPerPage() const
{
    wxSize size = GetClientSize();
    return size.y / m_lineHeight;
}

wxDataViewItem wxDataViewMainWindow::GetTopItem() const
{
    unsigned int item = GetFirstVisibleRow();
    wxDataViewTreeNode *node = NULL;
    wxDataViewItem dataitem;

    if ( !IsVirtualList() )
    {
        node = GetTreeNodeByRow(item);
        if( node == NULL ) return wxDataViewItem(0);

        dataitem = node->GetItem();
    }
    else
    {
        dataitem = wxDataViewItem( wxUIntToPtr(item+1) );
    }

    return dataitem;
}

int wxDataViewMainWindow::GetEndOfLastCol() const
{
    int width = 0;
    unsigned int i;
    for (i = 0; i < GetOwner()->GetColumnCount(); i++)
    {
        const wxDataViewColumn *c =
            const_cast<wxDataViewCtrl*>(GetOwner())->GetColumnAt( i );

        if (!c->IsHidden())
            width += c->GetWidth();
    }
    return width;
}

unsigned int wxDataViewMainWindow::GetFirstVisibleRow() const
{
    int x = 0;
    int y = 0;
    m_owner->CalcUnscrolledPosition( x, y, &x, &y );

    return GetLineAt( y );
}

unsigned int wxDataViewMainWindow::GetLastVisibleRow()
{
    wxSize client_size = GetClientSize();
    // Find row occupying the bottom line of the client area (dimY-1).
    m_owner->CalcUnscrolledPosition( client_size.x, client_size.y-1,
                                    &client_size.x, &client_size.y );
    unsigned int row = GetLineAt(client_size.y);

    return wxMin( GetRowCount()-1, row );
}

unsigned int wxDataViewMainWindow::GetLastFullyVisibleRow()
{
    unsigned int row = GetLastVisibleRow();

    int bottom = GetLineStart(row) + GetLineHeight(row);
    m_owner->CalcScrolledPosition(-1, bottom, NULL, &bottom);

    if ( bottom > GetClientSize().y )
        return wxMax(0, row - 1);
    else
        return row;
}

unsigned int wxDataViewMainWindow::GetRowCount() const
{
    if ( m_count == -1 )
    {
        wxDataViewMainWindow* const
            self = const_cast<wxDataViewMainWindow*>(this);
        self->UpdateCount(RecalculateCount());
        self->UpdateDisplay();
    }
    return m_count;
}

void wxDataViewMainWindow::ChangeCurrentRow( unsigned int row )
{
    m_currentRow = row;

    // send event
#if wxUSE_ACCESSIBILITY
    wxAccessible::NotifyEvent(wxACC_EVENT_OBJECT_FOCUS, m_owner, wxOBJID_CLIENT, m_currentRow+1);
#endif // wxUSE_ACCESSIBILITY
}

bool wxDataViewMainWindow::UnselectAllRows(unsigned int except)
{
    if (!m_selection.IsEmpty())
    {
        for (unsigned i = GetFirstVisibleRow(); i <= GetLastVisibleRow(); i++)
        {
            if (m_selection.IsSelected(i) && i != except)
                RefreshRow(i);
        }

        if (except != (unsigned int)-1)
        {
            const bool wasSelected = m_selection.IsSelected(except);
            ClearSelection();
            if (wasSelected)
            {
                m_selection.SelectItem(except);

                // The special item is still selected.
                return false;
            }
        }
        else
        {
            ClearSelection();
        }
    }

    // There are no selected items left.
    return true;
}

void wxDataViewMainWindow::SelectRow( unsigned int row, bool on )
{
    if ( m_selection.SelectItem(row, on) )
        RefreshRow(row);
}

void wxDataViewMainWindow::SelectRows( unsigned int from, unsigned int to )
{
    wxArrayInt changed;
    if ( m_selection.SelectRange(from, to, true, &changed) )
    {
        for (unsigned i = 0; i < changed.size(); i++)
            RefreshRow(changed[i]);
    }
    else // Selection of too many rows has changed.
    {
        RefreshRows( from, to );
    }
}

void wxDataViewMainWindow::Select( const wxArrayInt& aSelections )
{
    for (size_t i=0; i < aSelections.GetCount(); i++)
    {
        int n = aSelections[i];

        if ( m_selection.SelectItem(n) )
            RefreshRow( n );
    }
}

void wxDataViewMainWindow::ReverseRowSelection( unsigned int row )
{
    m_selection.SelectItem(row, !m_selection.IsSelected(row));
    RefreshRow( row );
}

bool wxDataViewMainWindow::IsRowSelected( unsigned int row )
{
    return m_selection.IsSelected(row);
}

void wxDataViewMainWindow::SendSelectionChangedEvent( const wxDataViewItem& item)
{
#if wxUSE_ACCESSIBILITY
    wxAccessible::NotifyEvent(wxACC_EVENT_OBJECT_SELECTIONWITHIN, m_owner, wxOBJID_CLIENT, wxACC_SELF);
#endif // wxUSE_ACCESSIBILITY

    wxDataViewEvent le(wxEVT_DATAVIEW_SELECTION_CHANGED, m_owner, item);
    m_owner->ProcessWindowEvent(le);
}

void wxDataViewMainWindow::RefreshRow( unsigned int row )
{
    wxRect rect( 0, GetLineStart( row ), GetEndOfLastCol(), GetLineHeight( row ) );
    m_owner->CalcScrolledPosition( rect.x, rect.y, &rect.x, &rect.y );

    wxSize client_size = GetClientSize();
    wxRect client_rect( 0, 0, client_size.x, client_size.y );
    wxRect intersect_rect = client_rect.Intersect( rect );
    if (intersect_rect.width > 0)
        Refresh( true, &intersect_rect );
}

void wxDataViewMainWindow::RefreshRows( unsigned int from, unsigned int to )
{
    if (from > to)
    {
        unsigned int tmp = to;
        to = from;
        from = tmp;
    }

    wxRect rect( 0, GetLineStart( from ), GetEndOfLastCol(), GetLineStart( (to-from+1) ) );
    m_owner->CalcScrolledPosition( rect.x, rect.y, &rect.x, &rect.y );

    wxSize client_size = GetClientSize();
    wxRect client_rect( 0, 0, client_size.x, client_size.y );
    wxRect intersect_rect = client_rect.Intersect( rect );
    if (intersect_rect.width > 0)
        Refresh( true, &intersect_rect );
}

void wxDataViewMainWindow::RefreshRowsAfter( unsigned int firstRow )
{
    wxSize client_size = GetClientSize();
    int start = GetLineStart( firstRow );
    m_owner->CalcScrolledPosition( start, 0, &start, NULL );
    if (start > client_size.y) return;

    wxRect rect( 0, start, client_size.x, client_size.y - start );

    Refresh( true, &rect );
}

wxRect wxDataViewMainWindow::GetLineRect( unsigned int row ) const
{
    wxRect rect;
    rect.x = 0;
    rect.y = GetLineStart( row );
    rect.width = GetEndOfLastCol();
    rect.height = GetLineHeight( row );

    return rect;
}

int wxDataViewMainWindow::GetLineStart( unsigned int row ) const
{
    const wxDataViewModel *model = GetModel();

    if (GetOwner()->GetWindowStyle() & wxDV_VARIABLE_LINE_HEIGHT)
    {
        // TODO make more efficient

        int start = 0;

        unsigned int r;
        for (r = 0; r < row; r++)
        {
            const wxDataViewTreeNode* node = GetTreeNodeByRow(r);
            if (!node) return start;

            wxDataViewItem item = node->GetItem();

            unsigned int cols = GetOwner()->GetColumnCount();
            unsigned int col;
            int height = m_lineHeight;
            for (col = 0; col < cols; col++)
            {
                const wxDataViewColumn *column = GetOwner()->GetColumn(col);
                if (column->IsHidden())
                    continue;      // skip it!

                if ((col != 0) &&
                    model->IsContainer(item) &&
                    !model->HasContainerColumns(item))
                    continue;      // skip it!

                wxDataViewRenderer *renderer =
                    const_cast<wxDataViewRenderer*>(column->GetRenderer());
                renderer->PrepareForItem(model, item, column->GetModelColumn());

                height = wxMax( height, renderer->GetSize().y );
            }

            start += height;
        }

        return start;
    }
    else
    {
        return row * m_lineHeight;
    }
}

int wxDataViewMainWindow::GetLineAt( unsigned int y ) const
{
    const wxDataViewModel *model = GetModel();

    // check for the easy case first
    if ( !GetOwner()->HasFlag(wxDV_VARIABLE_LINE_HEIGHT) )
        return y / m_lineHeight;

    // TODO make more efficient
    unsigned int row = 0;
    unsigned int yy = 0;
    for (;;)
    {
        const wxDataViewTreeNode* node = GetTreeNodeByRow(row);
        if (!node)
        {
            // not really correct...
            return row + ((y-yy) / m_lineHeight);
        }

        wxDataViewItem item = node->GetItem();

        unsigned int cols = GetOwner()->GetColumnCount();
        unsigned int col;
        int height = m_lineHeight;
        for (col = 0; col < cols; col++)
        {
            const wxDataViewColumn *column = GetOwner()->GetColumn(col);
            if (column->IsHidden())
                continue;      // skip it!

            if ((col != 0) &&
                model->IsContainer(item) &&
                !model->HasContainerColumns(item))
                continue;      // skip it!

            wxDataViewRenderer *renderer =
                const_cast<wxDataViewRenderer*>(column->GetRenderer());
            renderer->PrepareForItem(model, item, column->GetModelColumn());

            height = wxMax( height, renderer->GetSize().y );
        }

        yy += height;
        if (y < yy)
            return row;

        row++;
    }
}

int wxDataViewMainWindow::GetLineHeight( unsigned int row ) const
{
    const wxDataViewModel *model = GetModel();

    if (GetOwner()->GetWindowStyle() & wxDV_VARIABLE_LINE_HEIGHT)
    {
        wxASSERT( !IsVirtualList() );

        const wxDataViewTreeNode* node = GetTreeNodeByRow(row);
        // wxASSERT( node );
        if (!node) return m_lineHeight;

        wxDataViewItem item = node->GetItem();

        int height = m_lineHeight;

        unsigned int cols = GetOwner()->GetColumnCount();
        unsigned int col;
        for (col = 0; col < cols; col++)
        {
            const wxDataViewColumn *column = GetOwner()->GetColumn(col);
            if (column->IsHidden())
                continue;      // skip it!

            if ((col != 0) &&
                model->IsContainer(item) &&
                !model->HasContainerColumns(item))
                continue;      // skip it!

            wxDataViewRenderer *renderer =
                const_cast<wxDataViewRenderer*>(column->GetRenderer());
            renderer->PrepareForItem(model, item, column->GetModelColumn());

            height = wxMax( height, renderer->GetSize().y );
        }

        return height;
    }
    else
    {
        return m_lineHeight;
    }
}


class RowToTreeNodeJob: public DoJob
{
public:
    RowToTreeNodeJob( unsigned int row_ , int current_, wxDataViewTreeNode * node )
    {
        this->row = row_;
        this->current = current_;
        ret = NULL;
        parent = node;
    }

    virtual int operator() ( wxDataViewTreeNode * node ) wxOVERRIDE
    {
        current ++;
        if( current == static_cast<int>(row))
        {
            ret = node;
            return DoJob::DONE;
        }

        if( node->GetSubTreeCount() + current < static_cast<int>(row) )
        {
            current += node->GetSubTreeCount();
            return  DoJob::SKIP_SUBTREE;
        }
        else
        {
            parent = node;

            // If the current node has only leaf children, we can find the
            // desired node directly. This can speed up finding the node
            // in some cases, and will have a very good effect for list views.
            if ( node->HasChildren() &&
                 (int)node->GetChildNodes().size() == node->GetSubTreeCount() )
            {
                const int index = static_cast<int>(row) - current - 1;
                ret = node->GetChildNodes()[index];
                return DoJob::DONE;
            }

            return DoJob::CONTINUE;
        }
    }

    wxDataViewTreeNode * GetResult() const
        { return ret; }

private:
    unsigned int row;
    int current;
    wxDataViewTreeNode * ret;
    wxDataViewTreeNode * parent;
};

wxDataViewTreeNode * wxDataViewMainWindow::GetTreeNodeByRow(unsigned int row) const
{
    wxASSERT( !IsVirtualList() );

    if ( row == (unsigned)-1 )
        return NULL;

    RowToTreeNodeJob job( row , -2, m_root );
    Walker( m_root , job );
    return job.GetResult();
}

wxDataViewItem wxDataViewMainWindow::GetItemByRow(unsigned int row) const
{
    wxDataViewItem item;
    if (IsVirtualList())
    {
        if ( row < GetRowCount() )
            item = wxDataViewItem(wxUIntToPtr(row+1));
    }
    else
    {
        wxDataViewTreeNode *node = GetTreeNodeByRow(row);
        if ( node )
            item = node->GetItem();
    }

    return item;
}

bool
wxDataViewMainWindow::SendExpanderEvent(wxEventType type,
                                        const wxDataViewItem& item)
{
#if wxUSE_ACCESSIBILITY
    if ( type == wxEVT_DATAVIEW_ITEM_EXPANDED || type == wxEVT_DATAVIEW_ITEM_COLLAPSED )
        wxAccessible::NotifyEvent(wxACC_EVENT_OBJECT_REORDER, m_owner, wxOBJID_CLIENT, wxACC_SELF);
#endif // wxUSE_ACCESSIBILITY

    wxDataViewEvent le(type, m_owner, item);
    return !m_owner->ProcessWindowEvent(le) || le.IsAllowed();
}

bool wxDataViewMainWindow::IsExpanded( unsigned int row ) const
{
    if (IsList())
        return false;

    wxDataViewTreeNode * node = GetTreeNodeByRow(row);
    if (!node)
        return false;

    if (!node->HasChildren())
        return false;

    return node->IsOpen();
}

bool wxDataViewMainWindow::HasChildren( unsigned int row ) const
{
    if (IsList())
        return false;

    wxDataViewTreeNode * node = GetTreeNodeByRow(row);
    if (!node)
        return false;

    if (!node->HasChildren())
        return false;

    return true;
}

void wxDataViewMainWindow::Expand( unsigned int row )
{
    if (IsList())
        return;

    wxDataViewTreeNode * node = GetTreeNodeByRow(row);
    if (!node)
        return;

    if (!node->HasChildren())
        return;

    if (!node->IsOpen())
    {
        if ( !SendExpanderEvent(wxEVT_DATAVIEW_ITEM_EXPANDING, node->GetItem()) )
        {
            // Vetoed by the event handler.
            return;
        }

        node->ToggleOpen();

        // build the children of current node
        if( node->GetChildNodes().empty() )
        {
            SortPrepare();
            ::BuildTreeHelper(this, GetModel(), node->GetItem(), node);
        }

        const unsigned countNewRows = node->GetSubTreeCount();

        // Shift all stored indices after this row by the number of newly added
        // rows.
        m_selection.OnItemsInserted(row + 1, countNewRows);
        if ( m_currentRow > row )
            ChangeCurrentRow(m_currentRow + countNewRows);

        if ( m_count != -1 )
            m_count += countNewRows;

        // Expanding this item means the previously cached column widths could
        // have become invalid as new items are now visible.
        GetOwner()->InvalidateColBestWidths();

        UpdateDisplay();
        // Send the expanded event
        SendExpanderEvent(wxEVT_DATAVIEW_ITEM_EXPANDED,node->GetItem());
    }
}

void wxDataViewMainWindow::Collapse(unsigned int row)
{
    if (IsList())
        return;

    wxDataViewTreeNode *node = GetTreeNodeByRow(row);
    if (!node)
        return;

    if (!node->HasChildren())
        return;

    if (node->IsOpen())
    {
        if ( !SendExpanderEvent(wxEVT_DATAVIEW_ITEM_COLLAPSING,node->GetItem()) )
        {
            // Vetoed by the event handler.
            return;
        }

        const unsigned countDeletedRows = node->GetSubTreeCount();

        if ( m_selection.OnItemsDeleted(row + 1, countDeletedRows) )
        {
            SendSelectionChangedEvent(GetItemByRow(row));
        }

        node->ToggleOpen();

        // Adjust the current row if necessary.
        if ( m_currentRow > row )
        {
            // If the current row was among the collapsed items, make the
            // parent itself current.
            if ( m_currentRow <= row + countDeletedRows )
                ChangeCurrentRow(row);
            else // Otherwise just update the index.
                ChangeCurrentRow(m_currentRow - countDeletedRows);
        }

        if ( m_count != -1 )
            m_count -= countDeletedRows;

        GetOwner()->InvalidateColBestWidths();

        UpdateDisplay();
        SendExpanderEvent(wxEVT_DATAVIEW_ITEM_COLLAPSED,node->GetItem());
    }
}

wxDataViewTreeNode * wxDataViewMainWindow::FindNode( const wxDataViewItem & item )
{
    const wxDataViewModel * model = GetModel();
    if( model == NULL )
        return NULL;

    if (!item.IsOk())
        return m_root;

    // Compose the parent-chain for the item we are looking for
    wxVector<wxDataViewItem> parentChain;
    wxDataViewItem it( item );
    while( it.IsOk() )
    {
        parentChain.push_back(it);
        it = model->GetParent(it);
    }

    // Find the item along the parent-chain.
    // This algorithm is designed to speed up the node-finding method
    wxDataViewTreeNode* node = m_root;
    for( unsigned iter = parentChain.size()-1; ; --iter )
    {
        if( node->HasChildren() )
        {
            if( node->GetChildNodes().empty() )
            {
                // Even though the item is a container, it doesn't have any
                // child nodes in the control's representation yet. We have
                // to realize its subtree now.
                SortPrepare();
                ::BuildTreeHelper(this, model, node->GetItem(), node);
            }

            const wxDataViewTreeNodes& nodes = node->GetChildNodes();
            bool found = false;

            for (unsigned i = 0; i < nodes.GetCount(); ++i)
            {
                wxDataViewTreeNode* currentNode = nodes[i];
                if (currentNode->GetItem() == parentChain[iter])
                {
                    if (currentNode->GetItem() == item)
                        return currentNode;

                    node = currentNode;
                    found = true;
                    break;
                }
            }
            if (!found)
                return NULL;
        }
        else
            return NULL;

        if ( !iter )
            break;
    }
    return NULL;
}

void wxDataViewMainWindow::HitTest( const wxPoint & point, wxDataViewItem & item,
                                    wxDataViewColumn* &column )
{
    wxDataViewColumn *col = NULL;
    unsigned int cols = GetOwner()->GetColumnCount();
    unsigned int colnum = 0;
    int x, y;
    m_owner->CalcUnscrolledPosition( point.x, point.y, &x, &y );
    for (unsigned x_start = 0; colnum < cols; colnum++)
    {
        col = GetOwner()->GetColumnAt(colnum);
        if (col->IsHidden())
            continue;      // skip it!

        unsigned int w = col->GetWidth();
        if (x_start+w >= (unsigned int)x)
            break;

        x_start += w;
    }

    column = col;
    item = GetItemByRow( GetLineAt( y ) );
}

wxRect wxDataViewMainWindow::GetItemRect( const wxDataViewItem & item,
                                          const wxDataViewColumn* column )
{
    int xpos = 0;
    int width = 0;

    unsigned int cols = GetOwner()->GetColumnCount();
    // If column is null the loop will compute the combined width of all columns.
    // Otherwise, it will compute the x position of the column we are looking for.
    for (unsigned int i = 0; i < cols; i++)
    {
        wxDataViewColumn* col = GetOwner()->GetColumnAt( i );

        if (col == column)
            break;

        if (col->IsHidden())
            continue;      // skip it!

        xpos += col->GetWidth();
        width += col->GetWidth();
    }

    if(column != 0)
    {
        // If we have a column, we need can get its width directly.
        if(column->IsHidden())
            width = 0;
        else
            width = column->GetWidth();

    }
    else
    {
        // If we have no column, we reset the x position back to zero.
        xpos = 0;
    }

    // we have to take an expander column into account and compute its indentation
    // to get the correct x position where the actual text is
    int indent = 0;
    int row = GetRowByItem(item);
    if (!IsList() &&
            (column == 0 || GetExpanderColumnOrFirstOne(GetOwner()) == column) )
    {
        wxDataViewTreeNode* node = GetTreeNodeByRow(row);
        indent = GetOwner()->GetIndent() * node->GetIndentLevel();
        indent = indent + m_lineHeight; // use m_lineHeight as the width of the expander
    }

    wxRect itemRect( xpos + indent,
                     GetLineStart( row ),
                     width - indent,
                     GetLineHeight( row ) );

    GetOwner()->CalcScrolledPosition(  itemRect.x,  itemRect.y,
                                      &itemRect.x, &itemRect.y );

    return itemRect;
}

int wxDataViewMainWindow::RecalculateCount() const
{
    if (IsVirtualList())
    {
        wxDataViewVirtualListModel *list_model =
            (wxDataViewVirtualListModel*) GetModel();

        return list_model->GetCount();
    }
    else
    {
        return m_root->GetSubTreeCount();
    }
}

class ItemToRowJob : public DoJob
{
public:
    ItemToRowJob(const wxDataViewItem& item_, wxVector<wxDataViewItem>::reverse_iterator iter)
        : m_iter(iter),
        item(item_)
    {
        ret = -1;
    }

    // Maybe binary search will help to speed up this process
    virtual int operator() ( wxDataViewTreeNode * node) wxOVERRIDE
    {
        ret ++;
        if( node->GetItem() == item )
        {
            return DoJob::DONE;
        }

        if( node->GetItem() == *m_iter )
        {
            m_iter++;
            return DoJob::CONTINUE;
        }
        else
        {
            ret += node->GetSubTreeCount();
            return DoJob::SKIP_SUBTREE;
        }

    }

    // the row number is begin from zero
    int GetResult() const
        { return ret -1; }

private:
    wxVector<wxDataViewItem>::reverse_iterator m_iter;
    wxDataViewItem item;
    int ret;

};

int wxDataViewMainWindow::GetRowByItem(const wxDataViewItem & item) const
{
    const wxDataViewModel * model = GetModel();
    if( model == NULL )
        return -1;

    if (IsVirtualList())
    {
        return wxPtrToUInt( item.GetID() ) -1;
    }
    else
    {
        if( !item.IsOk() )
            return -1;

        // Compose the parent-chain of the item we are looking for
        wxVector<wxDataViewItem> parentChain;
        wxDataViewItem it( item );
        while( it.IsOk() )
        {
            parentChain.push_back(it);
            it = model->GetParent(it);
        }

        // add an 'invalid' item to represent our 'invisible' root node
        parentChain.push_back(wxDataViewItem());

        // the parent chain was created by adding the deepest parent first.
        // so if we want to start at the root node, we have to iterate backwards through the vector
        ItemToRowJob job( item, parentChain.rbegin() );
        Walker( m_root, job );
        return job.GetResult();
    }
}

static void BuildTreeHelper( wxDataViewMainWindow *window, const wxDataViewModel * model,
                             const wxDataViewItem & item, wxDataViewTreeNode * node)
{
    if( !model->IsContainer( item ) )
        return;

    wxDataViewItemArray children;
    unsigned int num = model->GetChildren( item, children);

    for ( unsigned int index = 0; index < num; index++ )
    {
        wxDataViewTreeNode *n = new wxDataViewTreeNode(window, node, children[index]);

        if( model->IsContainer(children[index]) )
            n->SetHasChildren( true );

        node->InsertChild(n, index);
    }

    wxASSERT( node->IsOpen() );
    node->ChangeSubTreeCount(+num);
}

void wxDataViewMainWindow::BuildTree(wxDataViewModel * model)
{
    DestroyTree();

    if (GetModel()->IsVirtualListModel())
    {
        InvalidateCount();
        return;
    }

    m_root = wxDataViewTreeNode::CreateRootNode(this);

    // First we define a invalid item to fetch the top-level elements
    wxDataViewItem item;
    SortPrepare();
    BuildTreeHelper(this, model, item, m_root);
    InvalidateCount();
}

void wxDataViewMainWindow::DestroyTree()
{
    if (!IsVirtualList())
    {
        wxDELETE(m_root);
        m_count = 0;
    }
}

wxDataViewColumn*
wxDataViewMainWindow::FindColumnForEditing(const wxDataViewItem& item, wxDataViewCellMode mode) const
{
    // Edit the current column editable in 'mode'. If no column is focused
    // (typically because the user has full row selected), try to find the
    // first editable column (this would typically be a checkbox for
    // wxDATAVIEW_CELL_ACTIVATABLE and we don't want to force the user to set
    // focus on the checkbox column; or on the only editable text column).

    wxDataViewColumn *candidate = m_currentCol;

    if ( candidate &&
         !IsCellEditableInMode(item, candidate, mode) &&
         !m_currentColSetByKeyboard )
    {
        // If current column was set by mouse to something not editable (in
        // 'mode') and the user pressed Space/F2 to edit it, treat the
        // situation as if there was whole-row focus, because that's what is
        // visually indicated and the mouse click could very well be targeted
        // on the row rather than on an individual cell.
        //
        // But if it was done by keyboard, respect that even if the column
        // isn't editable, because focus is visually on that column and editing
        // something else would be surprising.
        candidate = NULL;
    }

    if ( !candidate )
    {
        const unsigned cols = GetOwner()->GetColumnCount();
        for ( unsigned i = 0; i < cols; i++ )
        {
            wxDataViewColumn *c = GetOwner()->GetColumnAt(i);
            if ( c->IsHidden() )
                continue;

            if ( IsCellEditableInMode(item, c, mode) )
            {
                candidate = c;
                break;
            }
        }
    }

    // If on container item without columns, only the expander column
    // may be directly editable:
    if ( candidate &&
         GetOwner()->GetExpanderColumn() != candidate &&
         GetModel()->IsContainer(item) &&
         !GetModel()->HasContainerColumns(item) )
    {
        candidate = GetOwner()->GetExpanderColumn();
    }

    if ( !candidate )
       return NULL;

   if ( !IsCellEditableInMode(item, candidate, mode) )
       return NULL;

   return candidate;
}

bool wxDataViewMainWindow::IsCellEditableInMode(const wxDataViewItem& item,
                                                const wxDataViewColumn *col,
                                                wxDataViewCellMode mode) const
{
    if ( col->GetRenderer()->GetMode() != mode )
        return false;

    if ( !GetModel()->IsEnabled(item, col->GetModelColumn()) )
        return false;

    return true;
}

void wxDataViewMainWindow::OnCharHook(wxKeyEvent& event)
{
    if ( m_editorCtrl )
    {
        // Handle any keys special for the in-place editor and return without
        // calling Skip() below.
        switch ( event.GetKeyCode() )
        {
            case WXK_ESCAPE:
                m_editorRenderer->CancelEditing();
                return;

            case WXK_RETURN:
                // Shift-Enter is not special neither.
                if ( event.ShiftDown() )
                    break;
                wxFALLTHROUGH;

            case WXK_TAB:
                // Ctrl/Alt-Tab or Enter could be used for something else, so
                // don't handle them here.
                if ( event.HasModifiers() )
                    break;

                m_editorRenderer->FinishEditing();
                return;
        }
    }
    else if ( m_useCellFocus )
    {
        if ( event.GetKeyCode() == WXK_TAB && !event.HasModifiers() )
        {
            if ( event.ShiftDown() )
                OnLeftKey(event);
            else
                OnRightKey(event);
            return;
        }
    }

    event.Skip();
}

void wxDataViewMainWindow::OnChar( wxKeyEvent &event )
{
    wxWindow * const parent = GetParent();

    // propagate the char event upwards
    wxKeyEvent eventForParent(event);
    eventForParent.SetEventObject(parent);
    if ( parent->ProcessWindowEvent(eventForParent) )
        return;

    if ( parent->HandleAsNavigationKey(event) )
        return;

    // no item -> nothing to do
    if (!HasCurrentRow())
    {
        event.Skip();
        return;
    }

    switch ( event.GetKeyCode() )
    {
        case WXK_RETURN:
            if ( event.HasModifiers() )
            {
                event.Skip();
                break;
            }
            else
            {
                // Enter activates the item, i.e. sends wxEVT_DATAVIEW_ITEM_ACTIVATED to
                // it. Only if that event is not handled do we activate column renderer (which
                // is normally done by Space) or even inline editing.

                const wxDataViewItem item = GetItemByRow(m_currentRow);

                wxDataViewEvent le(wxEVT_DATAVIEW_ITEM_ACTIVATED, m_owner, item);
                if ( m_owner->ProcessWindowEvent(le) )
                    break;
                // else: fall through to WXK_SPACE handling
            }
            wxFALLTHROUGH;

        case WXK_SPACE:
            if ( event.HasModifiers() )
            {
                event.Skip();
                break;
            }
            else
            {
                // Space toggles activatable items or -- if not activatable --
                // starts inline editing (this is normally done using F2 on
                // Windows, but Space is common everywhere else, so use it too
                // for greater cross-platform compatibility).

                const wxDataViewItem item = GetItemByRow(m_currentRow);

                // Activate the current activatable column. If not column is focused (typically
                // because the user has full row selected), try to find the first activatable
                // column (this would typically be a checkbox and we don't want to force the user
                // to set focus on the checkbox column).
                wxDataViewColumn *activatableCol = FindColumnForEditing(item, wxDATAVIEW_CELL_ACTIVATABLE);

                if ( activatableCol )
                {
                    const unsigned colIdx = activatableCol->GetModelColumn();
                    const wxRect cell_rect = GetOwner()->GetItemRect(item, activatableCol);

                    wxDataViewRenderer *cell = activatableCol->GetRenderer();
                    cell->PrepareForItem(GetModel(), item, colIdx);
                    cell->WXActivateCell(cell_rect, GetModel(), item, colIdx, NULL);

                    break;
                }
                // else: fall through to WXK_F2 handling
                wxFALLTHROUGH;
            }

        case WXK_F2:
            if ( event.HasModifiers() )
            {
                event.Skip();
                break;
            }
            else
            {
                if ( !m_selection.IsEmpty() )
                {
                    // Mimic Windows 7 behavior: edit the item that has focus
                    // if it is selected and the first selected item if focus
                    // is out of selection.
                    unsigned sel;
                    if ( m_selection.IsSelected(m_currentRow) )
                    {
                        sel = m_currentRow;
                    }
                    else // Focused item is not selected.
                    {
                        wxSelectionStore::IterationState cookie;
                        sel = m_selection.GetFirstSelectedItem(cookie);
                    }


                    const wxDataViewItem item = GetItemByRow(sel);

                    // Edit the current column. If no column is focused
                    // (typically because the user has full row selected), try
                    // to find the first editable column.
                    wxDataViewColumn *editableCol = FindColumnForEditing(item, wxDATAVIEW_CELL_EDITABLE);

                    if ( editableCol )
                        GetOwner()->EditItem(item, editableCol);
                }
            }
            break;

        case WXK_UP:
            OnVerticalNavigation(event, -1);
            break;

        case WXK_DOWN:
            OnVerticalNavigation(event, +1);
            break;
        // Add the process for tree expanding/collapsing
        case WXK_LEFT:
            OnLeftKey(event);
            break;

        case WXK_RIGHT:
            OnRightKey(event);
            break;

        case WXK_END:
            OnVerticalNavigation(event, +(int)GetRowCount());
            break;

        case WXK_HOME:
            OnVerticalNavigation(event, -(int)GetRowCount());
            break;

        case WXK_PAGEUP:
            OnVerticalNavigation(event, -(GetCountPerPage() - 1));
            break;

        case WXK_PAGEDOWN:
            OnVerticalNavigation(event, +(GetCountPerPage() - 1));
            break;

        default:
            event.Skip();
    }
}

void wxDataViewMainWindow::OnVerticalNavigation(const wxKeyEvent& event, int delta)
{
    // if there is no selection, we cannot move it anywhere
    if (!HasCurrentRow() || IsEmpty())
        return;

    int newRow = (int)m_currentRow + delta;

    // let's keep the new row inside the allowed range
    if ( newRow < 0 )
        newRow = 0;

    const int rowCount = (int)GetRowCount();
    if ( newRow >= rowCount )
        newRow = rowCount - 1;

    unsigned int oldCurrent = m_currentRow;
    unsigned int newCurrent = (unsigned int)newRow;

    if ( newCurrent == oldCurrent )
        return;

    // in single selection we just ignore Shift as we can't select several
    // items anyhow
    if ( event.ShiftDown() && !IsSingleSel() )
    {
        RefreshRow( oldCurrent );

        ChangeCurrentRow( newCurrent );

        // select all the items between the old and the new one
        if ( oldCurrent > newCurrent )
        {
            newCurrent = oldCurrent;
            oldCurrent = m_currentRow;
        }

        SelectRows(oldCurrent, newCurrent);

        wxSelectionStore::IterationState cookie;
        const unsigned firstSel = m_selection.GetFirstSelectedItem(cookie);
        if ( firstSel != wxSelectionStore::NO_SELECTION )
            SendSelectionChangedEvent(GetItemByRow(firstSel));
    }
    else // !shift
    {
        RefreshRow( oldCurrent );

        // all previously selected items are unselected unless ctrl is held
        if ( !event.ControlDown() )
            UnselectAllRows();

        ChangeCurrentRow( newCurrent );

        if ( !event.ControlDown() )
        {
            SelectRow( m_currentRow, true );
            SendSelectionChangedEvent(GetItemByRow(m_currentRow));
        }
        else
            RefreshRow( m_currentRow );
    }

    GetOwner()->EnsureVisibleRowCol( m_currentRow, -1 );
}

void wxDataViewMainWindow::OnLeftKey(wxKeyEvent& event)
{
    if ( IsList() )
    {
        TryAdvanceCurrentColumn(NULL, event, /*forward=*/false);
    }
    else
    {
        wxDataViewTreeNode* node = GetTreeNodeByRow(m_currentRow);
        if ( !node )
            return;

        if ( TryAdvanceCurrentColumn(node, event, /*forward=*/false) )
            return;

        const bool dontCollapseNodes = event.GetKeyCode() == WXK_TAB;
        if ( dontCollapseNodes )
        {
            m_currentCol = NULL;
            // allow focus change
            event.Skip();
            return;
        }

        // Because TryAdvanceCurrentColumn() return false, we are at the first
        // column or using whole-row selection. In this situation, we can use
        // the standard TreeView handling of the left key.
        if (node->HasChildren() && node->IsOpen())
        {
            Collapse(m_currentRow);
        }
        else
        {
            // if the node is already closed, we move the selection to its parent
            wxDataViewTreeNode *parent_node = node->GetParent();

            if (parent_node)
            {
                int parent = GetRowByItem( parent_node->GetItem() );
                if ( parent >= 0 )
                {
                    unsigned int row = m_currentRow;
                    SelectRow( row, false);
                    SelectRow( parent, true );
                    ChangeCurrentRow( parent );
                    GetOwner()->EnsureVisibleRowCol( parent, -1 );
                    SendSelectionChangedEvent( parent_node->GetItem() );
                }
            }
        }
    }
}

void wxDataViewMainWindow::OnRightKey(wxKeyEvent& event)
{
    if ( IsList() )
    {
        TryAdvanceCurrentColumn(NULL, event, /*forward=*/true);
    }
    else
    {
        wxDataViewTreeNode* node = GetTreeNodeByRow(m_currentRow);
        if ( !node )
            return;

        if ( node->HasChildren() )
        {
            if ( !node->IsOpen() )
            {
                Expand( m_currentRow );
            }
            else
            {
                // if the node is already open, we move the selection to the first child
                unsigned int row = m_currentRow;
                SelectRow( row, false );
                SelectRow( row + 1, true );
                ChangeCurrentRow( row + 1 );
                GetOwner()->EnsureVisibleRowCol( row + 1, -1 );
                SendSelectionChangedEvent( GetItemByRow(row+1) );
            }
        }
        else
        {
            TryAdvanceCurrentColumn(node, event, /*forward=*/true);
        }
    }
}

bool wxDataViewMainWindow::TryAdvanceCurrentColumn(wxDataViewTreeNode *node, wxKeyEvent& event, bool forward)
{
    if ( GetOwner()->GetColumnCount() == 0 )
        return false;

    if ( !m_useCellFocus )
        return false;

    const bool wrapAround = event.GetKeyCode() == WXK_TAB;

    if ( node )
    {
        // navigation shouldn't work in branch nodes without other columns:
        if ( node->HasChildren() && !GetModel()->HasContainerColumns(node->GetItem()) )
            return false;
    }

    if ( m_currentCol == NULL || !m_currentColSetByKeyboard )
    {
        if ( forward )
        {
            m_currentCol = GetOwner()->GetColumnAt(0);
            m_currentColSetByKeyboard = true;
            RefreshRow(m_currentRow);
            return true;
        }
        else
        {
            if ( !wrapAround )
                return false;
        }
    }

    int idx = GetOwner()->GetColumnIndex(m_currentCol) + (forward ? +1 : -1);

    if ( idx >= (int)GetOwner()->GetColumnCount() )
    {
        if ( !wrapAround )
            return false;

        if ( GetCurrentRow() < GetRowCount() - 1 )
        {
            // go to the first column of the next row:
            idx = 0;
            OnVerticalNavigation(wxKeyEvent()/*dummy*/, +1);
        }
        else
        {
            // allow focus change
            event.Skip();
            return false;
        }
    }

    if ( idx < 0 && wrapAround )
    {
        if ( GetCurrentRow() > 0 )
        {
            // go to the last column of the previous row:
            idx = (int)GetOwner()->GetColumnCount() - 1;
            OnVerticalNavigation(wxKeyEvent()/*dummy*/, -1);
        }
        else
        {
            // allow focus change
            event.Skip();
            return false;
        }
    }

    GetOwner()->EnsureVisibleRowCol(m_currentRow, idx);

    if ( idx < 1 )
    {
        // We are going to the left of the second column. Reset to whole-row
        // focus (which means first column would be edited).
        m_currentCol = NULL;
        RefreshRow(m_currentRow);
        return true;
    }

    m_currentCol = GetOwner()->GetColumnAt(idx);
    m_currentColSetByKeyboard = true;
    RefreshRow(m_currentRow);
    return true;
}

void wxDataViewMainWindow::OnMouse( wxMouseEvent &event )
{
    if (event.GetEventType() == wxEVT_MOUSEWHEEL)
    {
        // let the base handle mouse wheel events.
        event.Skip();
        return;
    }

    if(event.ButtonDown())
    {
        // Not skipping button down events would prevent the system from
        // setting focus to this window as most (all?) of them do by default,
        // so skip it to enable default handling.
        event.Skip();
    }

    int x = event.GetX();
    int y = event.GetY();
    m_owner->CalcUnscrolledPosition( x, y, &x, &y );
    wxDataViewColumn *col = NULL;

    int xpos = 0;
    unsigned int cols = GetOwner()->GetColumnCount();
    unsigned int i;
    for (i = 0; i < cols; i++)
    {
        wxDataViewColumn *c = GetOwner()->GetColumnAt( i );
        if (c->IsHidden())
            continue;      // skip it!

        if (x < xpos + c->GetWidth())
        {
            col = c;
            break;
        }
        xpos += c->GetWidth();
    }

    wxDataViewModel* const model = GetModel();

    const unsigned int current = GetLineAt( y );
    const wxDataViewItem item = GetItemByRow(current);

    // Handle right clicking here, before everything else as context menu
    // events should be sent even when we click outside of any item, unlike all
    // the other ones.
    if (event.RightUp())
    {
        wxDataViewEvent le(wxEVT_DATAVIEW_ITEM_CONTEXT_MENU, m_owner, col, item);
        m_owner->ProcessWindowEvent(le);
        return;
    }

#if wxUSE_DRAG_AND_DROP
    if (event.Dragging() || ((m_dragCount > 0) && event.Leaving()))
    {
        if (m_dragCount == 0)
        {
            // we have to report the raw, physical coords as we want to be
            // able to call HitTest(event.m_pointDrag) from the user code to
            // get the item being dragged
            m_dragStart = event.GetPosition();
        }

        m_dragCount++;
        if ((m_dragCount < 3) && (event.Leaving()))
            m_dragCount = 3;
        else if (m_dragCount != 3)
            return;

        if (event.LeftIsDown())
        {
            m_owner->CalcUnscrolledPosition( m_dragStart.x, m_dragStart.y,
                                             &m_dragStart.x, &m_dragStart.y );
            unsigned int drag_item_row = GetLineAt( m_dragStart.y );
            wxDataViewItem itemDragged = GetItemByRow( drag_item_row );

            // Notify cell about drag
            wxDataViewEvent evt(wxEVT_DATAVIEW_ITEM_BEGIN_DRAG, m_owner, itemDragged);
            if (!m_owner->HandleWindowEvent( evt ))
                return;

            if (!evt.IsAllowed())
                return;

            wxDataObject *obj = evt.GetDataObject();
            if (!obj)
                return;

            wxDataViewDropSource drag( this, drag_item_row );
            drag.SetData( *obj );
            /* wxDragResult res = */ drag.DoDragDrop(evt.GetDragFlags());
            delete obj;
        }
        return;
    }
    else
    {
        m_dragCount = 0;
    }
#endif // wxUSE_DRAG_AND_DROP

    // Check if we clicked outside the item area.
    if ((current >= GetRowCount()) || !col)
    {
        // Follow Windows convention here: clicking either left or right (but
        // not middle) button clears the existing selection.
        if (m_owner && (event.LeftDown() || event.RightDown()))
        {
            if (!m_selection.IsEmpty())
            {
                m_owner->UnselectAll();
                SendSelectionChangedEvent(wxDataViewItem());
            }
        }
        event.Skip();
        return;
    }

    wxDataViewRenderer *cell = col->GetRenderer();
    wxDataViewColumn* const
        expander = GetExpanderColumnOrFirstOne(GetOwner());

    // Test whether the mouse is hovering over the expander (a.k.a tree "+"
    // button) and also determine the offset of the real cell start, skipping
    // the indentation and the expander itself.
    bool hoverOverExpander = false;
    int itemOffset = 0;
    if ((!IsList()) && (expander == col))
    {
        wxDataViewTreeNode * node = GetTreeNodeByRow(current);

        int indent = node->GetIndentLevel();
        itemOffset = GetOwner()->GetIndent()*indent;

        if ( node->HasChildren() )
        {
            // we make the rectangle we are looking in a bit bigger than the actual
            // visual expander so the user can hit that little thing reliably
            wxRect rect(xpos + itemOffset,
                        GetLineStart( current ) + (GetLineHeight(current) - m_lineHeight)/2,
                        m_lineHeight, m_lineHeight);

            if( rect.Contains(x, y) )
            {
                // So the mouse is over the expander
                hoverOverExpander = true;
                if (m_underMouse && m_underMouse != node)
                {
                    // wxLogMessage("Undo the row: %d", GetRowByItem(m_underMouse->GetItem()));
                    RefreshRow(GetRowByItem(m_underMouse->GetItem()));
                }
                if (m_underMouse != node)
                {
                    // wxLogMessage("Do the row: %d", current);
                    RefreshRow(current);
                }
                m_underMouse = node;
            }
        }

        // Account for the expander as well, even if this item doesn't have it,
        // its parent does so it still counts for the offset.
        itemOffset += m_lineHeight;
    }
    if (!hoverOverExpander)
    {
        if (m_underMouse != NULL)
        {
            // wxLogMessage("Undo the row: %d", GetRowByItem(m_underMouse->GetItem()));
            RefreshRow(GetRowByItem(m_underMouse->GetItem()));
            m_underMouse = NULL;
        }
    }

    bool simulateClick = false;

    if (event.ButtonDClick())
    {
        m_renameTimer->Stop();
        m_lastOnSame = false;
    }

    bool ignore_other_columns =
        ((expander != col) &&
        (model->IsContainer(item)) &&
        (!model->HasContainerColumns(item)));

    if (event.LeftDClick())
    {
        if ( !hoverOverExpander && (current == m_lineLastClicked) )
        {
            wxDataViewEvent le(wxEVT_DATAVIEW_ITEM_ACTIVATED, m_owner, col, item);
            if ( m_owner->ProcessWindowEvent(le) )
            {
                // Item activation was handled from the user code.
                return;
            }
        }

        // Either it was a double click over the expander, or the second click
        // happened on another item than the first one or it was a bona fide
        // double click which was unhandled. In all these cases we continue
        // processing this event as a simple click, e.g. to select the item or
        // activate the renderer.
        simulateClick = true;
    }

    if (event.LeftUp() && !hoverOverExpander)
    {
        if (m_lineSelectSingleOnUp != (unsigned int)-1)
        {
            // select single line
            if ( UnselectAllRows(m_lineSelectSingleOnUp) )
            {
                SelectRow( m_lineSelectSingleOnUp, true );
                SendSelectionChangedEvent( GetItemByRow(m_lineSelectSingleOnUp) );
            }
            //else: it was already selected, nothing to do
        }

        // If the user click the expander, we do not do editing even if the column
        // with expander are editable
        if (m_lastOnSame && !ignore_other_columns)
        {
            if ((col == m_currentCol) && (current == m_currentRow) &&
                IsCellEditableInMode(item, col, wxDATAVIEW_CELL_EDITABLE) )
            {
                m_renameTimer->Start( 100, true );
            }
        }

        m_lastOnSame = false;
        m_lineSelectSingleOnUp = (unsigned int)-1;
    }
    else if(!event.LeftUp())
    {
        // This is necessary, because after a DnD operation in
        // from and to ourself, the up event is swallowed by the
        // DnD code. So on next non-up event (which means here and
        // now) m_lineSelectSingleOnUp should be reset.
        m_lineSelectSingleOnUp = (unsigned int)-1;
    }

    if (event.RightDown())
    {
        m_lineBeforeLastClicked = m_lineLastClicked;
        m_lineLastClicked = current;

        // If the item is already selected, do not update the selection.
        // Multi-selections should not be cleared if a selected item is clicked.
        if (!IsRowSelected(current))
        {
            UnselectAllRows();

            const unsigned oldCurrent = m_currentRow;
            ChangeCurrentRow(current);
            SelectRow(m_currentRow,true);
            RefreshRow(oldCurrent);
            SendSelectionChangedEvent(GetItemByRow( m_currentRow ) );
        }
    }
    else if (event.MiddleDown())
    {
    }

    if((event.LeftDown() || simulateClick) && hoverOverExpander)
    {
        wxDataViewTreeNode* node = GetTreeNodeByRow(current);

        // hoverOverExpander being true tells us that our node must be
        // valid and have children.
        // So we don't need any extra checks.
        if( node->IsOpen() )
            Collapse(current);
        else
            Expand(current);
    }
    else if ((event.LeftDown() || simulateClick) && !hoverOverExpander)
    {
        m_lineBeforeLastClicked = m_lineLastClicked;
        m_lineLastClicked = current;

        unsigned int oldCurrentRow = m_currentRow;
        bool oldWasSelected = IsRowSelected(m_currentRow);

        bool cmdModifierDown = event.CmdDown();
        if ( IsSingleSel() || !(cmdModifierDown || event.ShiftDown()) )
        {
            if ( IsSingleSel() || !IsRowSelected(current) )
            {
                ChangeCurrentRow(current);
                if ( UnselectAllRows(current) )
                {
                    SelectRow(m_currentRow,true);
                    SendSelectionChangedEvent(GetItemByRow( m_currentRow ) );
                }
            }
            else // multi sel & current is highlighted & no mod keys
            {
                m_lineSelectSingleOnUp = current;
                ChangeCurrentRow(current); // change focus
            }
        }
        else // multi sel & either ctrl or shift is down
        {
            if (cmdModifierDown)
            {
                ChangeCurrentRow(current);
                ReverseRowSelection(m_currentRow);
                SendSelectionChangedEvent(GetItemByRow(m_currentRow));
            }
            else if (event.ShiftDown())
            {
                ChangeCurrentRow(current);

                unsigned int lineFrom = oldCurrentRow,
                    lineTo = current;

                if ( lineFrom == static_cast<unsigned>(-1) )
                {
                    // If we hadn't had any current row before, treat this as a
                    // simple click and select the new row only.
                    lineFrom = current;
                }

                if ( lineTo < lineFrom )
                {
                    lineTo = lineFrom;
                    lineFrom = m_currentRow;
                }

                SelectRows(lineFrom, lineTo);

                wxSelectionStore::IterationState cookie;
                const unsigned firstSel = m_selection.GetFirstSelectedItem(cookie);
                if ( firstSel != wxSelectionStore::NO_SELECTION )
                    SendSelectionChangedEvent(GetItemByRow(firstSel) );
            }
            else // !ctrl, !shift
            {
                // test in the enclosing if should make it impossible
                wxFAIL_MSG( wxT("how did we get here?") );
            }
        }

        if (m_currentRow != oldCurrentRow)
            RefreshRow( oldCurrentRow );

        wxDataViewColumn *oldCurrentCol = m_currentCol;

        // Update selection here...
        m_currentCol = col;
        m_currentColSetByKeyboard = false;

        // This flag is used to decide whether we should start editing the item
        // label. We do it if the user clicks twice (but not double clicks,
        // i.e. simulateClick is false) on the same item but not if the click
        // was used for something else already, e.g. selecting the item (so it
        // must have been already selected) or giving the focus to the control
        // (so it must have had focus already).
        m_lastOnSame = !simulateClick && ((col == oldCurrentCol) &&
                        (current == oldCurrentRow)) && oldWasSelected &&
                        HasFocus();

        // Call ActivateCell() after everything else as under GTK+
        if ( IsCellEditableInMode(item, col, wxDATAVIEW_CELL_ACTIVATABLE) )
        {
            // notify cell about click

            wxRect cell_rect( xpos + itemOffset,
                              GetLineStart( current ),
                              col->GetWidth() - itemOffset,
                              GetLineHeight( current ) );

            // Note that PrepareForItem() should be called after GetLineStart()
            // call in cell_rect initialization above as GetLineStart() calls
            // PrepareForItem() for other items from inside it.
            cell->PrepareForItem(model, item, col->GetModelColumn());

            // Report position relative to the cell's custom area, i.e.
            // not the entire space as given by the control but the one
            // used by the renderer after calculation of alignment etc.
            //
            // Notice that this results in negative coordinates when clicking
            // in the upper left corner of a centre-aligned cell which doesn't
            // fill its column entirely so this is somewhat surprising, but we
            // do it like this for compatibility with the native GTK+ version,
            // see #12270.

            // adjust the rectangle ourselves to account for the alignment
            const int align = cell->GetEffectiveAlignment();

            wxRect rectItem = cell_rect;
            const wxSize size = cell->GetSize();
            if ( size.x >= 0 && size.x < cell_rect.width )
            {
                if ( align & wxALIGN_CENTER_HORIZONTAL )
                    rectItem.x += (cell_rect.width - size.x)/2;
                else if ( align & wxALIGN_RIGHT )
                    rectItem.x += cell_rect.width - size.x;
                // else: wxALIGN_LEFT is the default
            }

            if ( size.y >= 0 && size.y < cell_rect.height )
            {
                if ( align & wxALIGN_CENTER_VERTICAL )
                    rectItem.y += (cell_rect.height - size.y)/2;
                else if ( align & wxALIGN_BOTTOM )
                    rectItem.y += cell_rect.height - size.y;
                // else: wxALIGN_TOP is the default
            }

            wxMouseEvent event2(event);
            event2.m_x -= rectItem.x;
            event2.m_y -= rectItem.y;
            m_owner->CalcUnscrolledPosition(event2.m_x, event2.m_y, &event2.m_x, &event2.m_y);

             /* ignore ret */ cell->WXActivateCell
                                    (
                                        cell_rect,
                                        model,
                                        item,
                                        col->GetModelColumn(),
                                        &event2
                                    );
        }
    }
}

void wxDataViewMainWindow::OnSetFocus( wxFocusEvent &event )
{
    m_hasFocus = true;

    // Make the control usable from keyboard once it gets focus by ensuring
    // that it has a current row, if at all possible.
    if ( !HasCurrentRow() && !IsEmpty() )
    {
        ChangeCurrentRow(0);
    }

    if (HasCurrentRow())
    {
        Refresh();
    }
#if wxUSE_ACCESSIBILITY
    else
    {
        wxAccessible::NotifyEvent(wxACC_EVENT_OBJECT_FOCUS, m_owner, wxOBJID_CLIENT, wxACC_SELF);
    }
#endif // wxUSE_ACCESSIBILITY

    event.Skip();
}

void wxDataViewMainWindow::OnKillFocus( wxFocusEvent &event )
{
    m_hasFocus = false;

    if (HasCurrentRow())
        Refresh();

    event.Skip();
}

void wxDataViewMainWindow::OnColumnsCountChanged()
{
    int editableCount = 0;

    const unsigned cols = GetOwner()->GetColumnCount();
    for ( unsigned i = 0; i < cols; i++ )
    {
        wxDataViewColumn *c = GetOwner()->GetColumnAt(i);
        if ( c->IsHidden() )
            continue;
        if ( c->GetRenderer()->GetMode() != wxDATAVIEW_CELL_INERT )
            editableCount++;
    }

    m_useCellFocus = (editableCount > 0);

    UpdateDisplay();
}

void wxDataViewMainWindow::UpdateColumnSizes()
{
    int colsCount = GetOwner()->GetColumnCount();
    if ( !colsCount )
        return;

    wxDataViewCtrl *owner = GetOwner();

    int fullWinWidth = GetSize().x;

    wxDataViewColumn *lastCol = owner->GetColumn(colsCount - 1);
    int colswidth = GetEndOfLastCol();
    int lastColX = colswidth - lastCol->GetWidth();
    if ( lastColX < fullWinWidth )
    {
        int desiredWidth = wxMax(fullWinWidth - lastColX, lastCol->GetMinWidth());
        if ( !lastCol->WXUpdateWidth(desiredWidth) )
        {
            // The column width didn't change, no need to do anything else.
            return;
        }

        // All columns fit on screen, so we don't need horizontal scrolling.
        // To prevent flickering scrollbar when resizing the window to be
        // narrower, force-set the virtual width to 0 here. It will eventually
        // be corrected at idle time.
        SetVirtualSize(0, m_virtualSize.y);

        RefreshRect(wxRect(lastColX, 0, fullWinWidth - lastColX, GetSize().y));
    }
    else
    {
        // else: don't bother, the columns won't fit anyway
        SetVirtualSize(colswidth, m_virtualSize.y);
    }
}

//-----------------------------------------------------------------------------
// wxDataViewCtrl
//-----------------------------------------------------------------------------

WX_DEFINE_LIST(wxDataViewColumnList)

wxIMPLEMENT_DYNAMIC_CLASS(wxDataViewCtrl, wxDataViewCtrlBase);
wxBEGIN_EVENT_TABLE(wxDataViewCtrl, wxDataViewCtrlBase)
    EVT_SIZE(wxDataViewCtrl::OnSize)
wxEND_EVENT_TABLE()

wxDataViewCtrl::~wxDataViewCtrl()
{
    if (m_notifier)
        GetModel()->RemoveNotifier( m_notifier );

    m_cols.Clear();
    m_colsBestWidths.clear();

#if wxUSE_ACCESSIBILITY
    SetAccessible(NULL);
    wxAccessible::NotifyEvent(wxACC_EVENT_OBJECT_DESTROY, this, wxOBJID_CLIENT, wxACC_SELF);
#endif // wxUSE_ACCESSIBILITY
}

void wxDataViewCtrl::Init()
{
    m_cols.DeleteContents(true);
    m_notifier = NULL;

    m_headerArea = NULL;
    m_clientArea = NULL;

    m_colsDirty = false;

    m_allowMultiColumnSort = false;
}

bool wxDataViewCtrl::Create(wxWindow *parent,
                            wxWindowID id,
                            const wxPoint& pos,
                            const wxSize& size,
                            long style,
                            const wxValidator& validator,
                            const wxString& name)
{
//    if ( (style & wxBORDER_MASK) == 0)
//        style |= wxBORDER_SUNKEN;

    Init();

    if (!wxControl::Create( parent, id, pos, size,
                            style | wxScrolledWindowStyle, validator, name))
        return false;

    SetInitialSize(size);

#ifdef __WXMAC__
    MacSetClipChildren( true );
#endif

    m_clientArea = new wxDataViewMainWindow( this, wxID_ANY );

    // We use the cursor keys for moving the selection, not scrolling, so call
    // this method to ensure wxScrollHelperEvtHandler doesn't catch all
    // keyboard events forwarded to us from wxListMainWindow.
    DisableKeyboardScrolling();

    if (HasFlag(wxDV_NO_HEADER))
        m_headerArea = NULL;
    else
        m_headerArea = new wxDataViewHeaderWindow(this);

    SetTargetWindow( m_clientArea );

    wxBoxSizer *sizer = new wxBoxSizer( wxVERTICAL );
    if (m_headerArea)
        sizer->Add( m_headerArea, 0, wxGROW );
    sizer->Add( m_clientArea, 1, wxGROW );
    SetSizer( sizer );

    EnableSystemTheme();

#if wxUSE_ACCESSIBILITY
    wxAccessible::NotifyEvent(wxACC_EVENT_OBJECT_CREATE, this, wxOBJID_CLIENT, wxACC_SELF);
#endif // wxUSE_ACCESSIBILITY

    return true;
}

wxBorder wxDataViewCtrl::GetDefaultBorder() const
{
    return wxBORDER_THEME;
}

wxHeaderCtrl* wxDataViewCtrl::GenericGetHeader() const
{
    return m_headerArea;
}

#ifdef __WXMSW__
WXLRESULT wxDataViewCtrl::MSWWindowProc(WXUINT nMsg,
                                        WXWPARAM wParam,
                                        WXLPARAM lParam)
{
    WXLRESULT rc = wxDataViewCtrlBase::MSWWindowProc(nMsg, wParam, lParam);

    // we need to process arrows ourselves for scrolling
    if ( nMsg == WM_GETDLGCODE )
    {
        rc |= DLGC_WANTARROWS;
    }

    return rc;
}
#endif

wxSize wxDataViewCtrl::GetSizeAvailableForScrollTarget(const wxSize& size)
{
    wxSize newsize = size;
    if (!HasFlag(wxDV_NO_HEADER) && (m_headerArea))
    newsize.y -= m_headerArea->GetSize().y;

    return newsize;
}

void wxDataViewCtrl::OnSize( wxSizeEvent &WXUNUSED(event) )
{
    if ( m_clientArea && GetColumnCount() )
        m_clientArea->UpdateColumnSizes();

    // We need to override OnSize so that our scrolled
    // window a) does call Layout() to use sizers for
    // positioning the controls but b) does not query
    // the sizer for their size and use that for setting
    // the scrollable area as set that ourselves by
    // calling SetScrollbar() further down.

    Layout();

    AdjustScrollbars();

    // We must redraw the headers if their height changed. Normally this
    // shouldn't happen as the control shouldn't let itself be resized beneath
    // its minimal height but avoid the display artefacts that appear if it
    // does happen, e.g. because there is really not enough vertical space.
    if ( !HasFlag(wxDV_NO_HEADER) && m_headerArea &&
            m_headerArea->GetSize().y <= m_headerArea->GetBestSize(). y )
    {
        m_headerArea->Refresh();
    }
}

void wxDataViewCtrl::SetFocus()
{
    if (m_clientArea)
        m_clientArea->SetFocus();
}

bool wxDataViewCtrl::SetFont(const wxFont & font)
{
    if (!wxControl::SetFont(font))
        return false;

    if (m_headerArea)
        m_headerArea->SetFont(font);

    if (m_clientArea)
    {
        m_clientArea->SetFont(font);
        m_clientArea->SetRowHeight(m_clientArea->GetDefaultRowHeight());
    }

    if (m_headerArea || m_clientArea)
    {
        InvalidateColBestWidths();
        Layout();
    }

    return true;
}

#if wxUSE_ACCESSIBILITY
bool wxDataViewCtrl::Show(bool show)
{
    bool changed = wxControl::Show(show);
    if ( changed )
    {
        wxAccessible::NotifyEvent(show ? wxACC_EVENT_OBJECT_SHOW : wxACC_EVENT_OBJECT_HIDE,
                                  this, wxOBJID_CLIENT, wxACC_SELF);
    }

    return changed;
}

bool wxDataViewCtrl::Enable(bool enable)
{
    bool changed = wxControl::Enable(enable);
    if ( changed )
    {
        wxAccessible::NotifyEvent(wxACC_EVENT_OBJECT_STATECHANGE, this, wxOBJID_CLIENT, wxACC_SELF);
    }

    return changed;
}

void wxDataViewCtrl::SetName(const wxString &name)
{
    wxControl::SetName(name);
    wxAccessible::NotifyEvent(wxACC_EVENT_OBJECT_NAMECHANGE, this, wxOBJID_CLIENT, wxACC_SELF);
}

bool wxDataViewCtrl::Reparent(wxWindowBase *newParent)
{
    bool changed = wxControl::Reparent(newParent);
    if ( changed )
    {
        wxAccessible::NotifyEvent(wxACC_EVENT_OBJECT_PARENTCHANGE, this, wxOBJID_CLIENT, wxACC_SELF);
    }

    return changed;
}
#endif // wxUSE_ACCESIBILITY

bool wxDataViewCtrl::AssociateModel( wxDataViewModel *model )
{
    if (!wxDataViewCtrlBase::AssociateModel( model ))
        return false;

    if (model)
    {
        m_notifier = new wxGenericDataViewModelNotifier( m_clientArea );
        model->AddNotifier( m_notifier );
    }
    else
    {
        // Our previous notifier has either been already deleted when the
        // previous model was DecRef()'d in the base class AssociateModel() or
        // is not associated with us any more because if the model is still
        // alive, it's not used by this control.
        m_notifier = NULL;
    }

    m_clientArea->DestroyTree();

    if (model)
    {
        m_clientArea->BuildTree(model);
    }

    m_clientArea->UpdateDisplay();

    return true;
}

#if wxUSE_DRAG_AND_DROP

bool wxDataViewCtrl::EnableDragSource( const wxDataFormat &format )
{
    return m_clientArea->EnableDragSource( format );
}

bool wxDataViewCtrl::EnableDropTarget( const wxDataFormat &format )
{
    return m_clientArea->EnableDropTarget( format );
}

#endif // wxUSE_DRAG_AND_DROP

bool wxDataViewCtrl::AppendColumn( wxDataViewColumn *col )
{
    if (!wxDataViewCtrlBase::AppendColumn(col))
        return false;

    m_cols.Append( col );
    m_colsBestWidths.push_back(CachedColWidthInfo());
    OnColumnsCountChanged();
    return true;
}

bool wxDataViewCtrl::PrependColumn( wxDataViewColumn *col )
{
    if (!wxDataViewCtrlBase::PrependColumn(col))
        return false;

    m_cols.Insert( col );
    m_colsBestWidths.insert(m_colsBestWidths.begin(), CachedColWidthInfo());
    OnColumnsCountChanged();
    return true;
}

bool wxDataViewCtrl::InsertColumn( unsigned int pos, wxDataViewColumn *col )
{
    if (!wxDataViewCtrlBase::InsertColumn(pos,col))
        return false;

    m_cols.Insert( pos, col );
    m_colsBestWidths.insert(m_colsBestWidths.begin() + pos, CachedColWidthInfo());
    OnColumnsCountChanged();
    return true;
}

void wxDataViewCtrl::OnColumnWidthChange(unsigned int idx)
{
    InvalidateColBestWidth(idx);

    OnColumnChange(idx);
}

void wxDataViewCtrl::OnColumnChange(unsigned int idx)
{
    if ( m_headerArea )
        m_headerArea->UpdateColumn(idx);

    m_clientArea->UpdateDisplay();
}

void wxDataViewCtrl::OnColumnsCountChanged()
{
    if (m_headerArea)
        m_headerArea->SetColumnCount(GetColumnCount());

    m_clientArea->OnColumnsCountChanged();
}

void wxDataViewCtrl::DoSetExpanderColumn()
{
    wxDataViewColumn* column = GetExpanderColumn();
    if ( column )
    {
        int index = GetColumnIndex(column);
        if ( index != wxNOT_FOUND )
            InvalidateColBestWidth(index);
    }

    m_clientArea->UpdateDisplay();
}

void wxDataViewCtrl::DoSetIndent()
{
    m_clientArea->UpdateDisplay();
}

unsigned int wxDataViewCtrl::GetColumnCount() const
{
    return m_cols.GetCount();
}

bool wxDataViewCtrl::SetRowHeight( int lineHeight )
{
    if ( !m_clientArea )
        return false;

    m_clientArea->SetRowHeight(lineHeight);

    return true;
}

wxDataViewColumn* wxDataViewCtrl::GetColumn( unsigned int idx ) const
{
    return m_cols[idx];
}

wxDataViewColumn *wxDataViewCtrl::GetColumnAt(unsigned int pos) const
{
    // columns can't be reordered if there is no header window which allows
    // to do this
    const unsigned idx = m_headerArea ? m_headerArea->GetColumnsOrder()[pos]
                                    : pos;

    return GetColumn(idx);
}

int wxDataViewCtrl::GetColumnIndex(const wxDataViewColumn *column) const
{
    const unsigned count = m_cols.size();
    for ( unsigned n = 0; n < count; n++ )
    {
        if ( m_cols[n] == column )
            return n;
    }

    return wxNOT_FOUND;
}

int wxDataViewCtrl::GetModelColumnIndex( unsigned int model_column ) const
{
    const int count = GetColumnCount();
    for ( int index = 0; index < count; index++ )
    {
        wxDataViewColumn* column = GetColumn(index);
        if ( column->GetModelColumn() == model_column )
            return index;
    }
    return wxNOT_FOUND;
}

class wxDataViewMaxWidthCalculator : public wxMaxWidthCalculatorBase
{
public:
    wxDataViewMaxWidthCalculator(const wxDataViewCtrl *dvc,
                                 wxDataViewMainWindow *clientArea,
                                 wxDataViewRenderer *renderer,
                                 const wxDataViewModel *model,
                                 size_t model_column,
                                 int expanderSize)
        : wxMaxWidthCalculatorBase(model_column),
          m_dvc(dvc),
          m_clientArea(clientArea),
          m_renderer(renderer),
          m_model(model),
          m_expanderSize(expanderSize)
    {
        int index = dvc->GetModelColumnIndex( model_column );
        wxDataViewColumn* column = index == wxNOT_FOUND ? NULL : dvc->GetColumn(index);
        m_isExpanderCol =
            !clientArea->IsList() &&
            (column == 0 ||
             GetExpanderColumnOrFirstOne(const_cast<wxDataViewCtrl*>(dvc)) == column );
    }

    virtual void UpdateWithRow(int row) wxOVERRIDE
    {
        int indent = 0;
        wxDataViewItem item;

        if ( m_isExpanderCol )
        {
            wxDataViewTreeNode *node = m_clientArea->GetTreeNodeByRow(row);
            item = node->GetItem();
            indent = m_dvc->GetIndent() * node->GetIndentLevel() + m_expanderSize;
        }
        else
        {
            item = m_clientArea->GetItemByRow(row);
        }

        m_renderer->PrepareForItem(m_model, item, GetColumn());
        UpdateWithWidth(m_renderer->GetSize().x + indent);
    }

private:
    const wxDataViewCtrl *m_dvc;
    wxDataViewMainWindow *m_clientArea;
    wxDataViewRenderer *m_renderer;
    const wxDataViewModel *m_model;
    bool m_isExpanderCol;
    int m_expanderSize;
};


unsigned int wxDataViewCtrl::GetBestColumnWidth(int idx) const
{
    if ( m_colsBestWidths[idx].width != 0 )
        return m_colsBestWidths[idx].width;

    const int count = m_clientArea->GetRowCount();
    wxDataViewColumn *column = GetColumn(idx);
    wxDataViewRenderer *renderer =
        const_cast<wxDataViewRenderer*>(column->GetRenderer());

    wxDataViewMaxWidthCalculator calculator(this, m_clientArea, renderer,
                                            GetModel(), column->GetModelColumn(),
                                            m_clientArea->GetRowHeight());

    calculator.UpdateWithWidth(column->GetMinWidth());

    if ( m_headerArea )
        calculator.UpdateWithWidth(m_headerArea->GetColumnTitleWidth(*column));

    const wxPoint origin = CalcUnscrolledPosition(wxPoint(0, 0));
    calculator.ComputeBestColumnWidth(count,
                                      m_clientArea->GetLineAt(origin.y),
                                      m_clientArea->GetLineAt(origin.y + GetClientSize().y));

    int max_width = calculator.GetMaxWidth();
    if ( max_width > 0 )
        max_width += 2 * PADDING_RIGHTLEFT;

    const_cast<wxDataViewCtrl*>(this)->m_colsBestWidths[idx].width = max_width;
    return max_width;
}

void wxDataViewCtrl::ColumnMoved(wxDataViewColumn * WXUNUSED(col),
                                unsigned int WXUNUSED(new_pos))
{
    // do _not_ reorder m_cols elements here, they should always be in the
    // order in which columns were added, we only display the columns in
    // different order
    m_clientArea->UpdateDisplay();
}

bool wxDataViewCtrl::DeleteColumn( wxDataViewColumn *column )
{
    wxDataViewColumnList::compatibility_iterator ret = m_cols.Find( column );
    if (!ret)
        return false;

    m_colsBestWidths.erase(m_colsBestWidths.begin() + GetColumnIndex(column));
    m_cols.Erase(ret);

    if ( m_clientArea->GetCurrentColumn() == column )
        m_clientArea->ClearCurrentColumn();

    OnColumnsCountChanged();

    return true;
}

bool wxDataViewCtrl::ClearColumns()
{
    SetExpanderColumn(NULL);
    m_cols.Clear();
    m_sortingColumnIdxs.clear();
    m_colsBestWidths.clear();

    m_clientArea->ClearCurrentColumn();

    OnColumnsCountChanged();

    return true;
}

void wxDataViewCtrl::InvalidateColBestWidth(int idx)
{
    m_colsBestWidths[idx].width = 0;
    m_colsBestWidths[idx].dirty = true;
    m_colsDirty = true;
}

void wxDataViewCtrl::InvalidateColBestWidths()
{
    // mark all columns as dirty:
    m_colsBestWidths.clear();
    m_colsBestWidths.resize(m_cols.size());
    m_colsDirty = true;
}

void wxDataViewCtrl::UpdateColWidths()
{
    m_colsDirty = false;

    if ( !m_headerArea )
        return;

    const unsigned len = m_colsBestWidths.size();
    for ( unsigned i = 0; i < len; i++ )
    {
        // Note that we have to have an explicit 'dirty' flag here instead of
        // checking if the width==0, as is done in GetBestColumnWidth().
        //
        // Testing width==0 wouldn't work correctly if some code called
        // GetWidth() after col. width invalidation but before
        // wxDataViewCtrl::UpdateColWidths() was called at idle time. This
        // would result in the header's column width getting out of sync with
        // the control itself.
        if ( m_colsBestWidths[i].dirty )
        {
            m_headerArea->UpdateColumn(i);
            m_colsBestWidths[i].dirty = false;
        }
    }
}

void wxDataViewCtrl::OnInternalIdle()
{
    wxDataViewCtrlBase::OnInternalIdle();

    if ( m_colsDirty )
        UpdateColWidths();
}

int wxDataViewCtrl::GetColumnPosition( const wxDataViewColumn *column ) const
{
    unsigned int len = GetColumnCount();
    for ( unsigned int i = 0; i < len; i++ )
    {
        wxDataViewColumn * col = GetColumnAt(i);
        if (column==col)
            return i;
    }

    return wxNOT_FOUND;
}

wxDataViewColumn *wxDataViewCtrl::GetSortingColumn() const
{
    if ( m_sortingColumnIdxs.empty() )
        return NULL;

    return GetColumn(m_sortingColumnIdxs.front());
}

wxVector<wxDataViewColumn *> wxDataViewCtrl::GetSortingColumns() const
{
    wxVector<wxDataViewColumn *> out;

    for ( wxVector<int>::const_iterator it = m_sortingColumnIdxs.begin(),
                                       end = m_sortingColumnIdxs.end();
          it != end;
          ++it )
    {
        out.push_back(GetColumn(*it));
    }

    return out;
}

wxDataViewItem wxDataViewCtrl::DoGetCurrentItem() const
{
    return GetItemByRow(m_clientArea->GetCurrentRow());
}

void wxDataViewCtrl::DoSetCurrentItem(const wxDataViewItem& item)
{
    const int row = m_clientArea->GetRowByItem(item);

    const unsigned oldCurrent = m_clientArea->GetCurrentRow();
    if ( static_cast<unsigned>(row) != oldCurrent )
    {
        m_clientArea->ChangeCurrentRow(row);
        m_clientArea->RefreshRow(oldCurrent);
        m_clientArea->RefreshRow(row);
    }
}

wxDataViewColumn *wxDataViewCtrl::GetCurrentColumn() const
{
    return m_clientArea->GetCurrentColumn();
}

int wxDataViewCtrl::GetSelectedItemsCount() const
{
    return m_clientArea->GetSelections().GetSelectedCount();
}

wxDataViewItem wxDataViewCtrl::GetTopItem() const
{
    return m_clientArea->GetTopItem();
}

int wxDataViewCtrl::GetCountPerPage() const
{
    return m_clientArea->GetCountPerPage();
}

int wxDataViewCtrl::GetSelections( wxDataViewItemArray & sel ) const
{
    sel.Empty();
    const wxSelectionStore& selections = m_clientArea->GetSelections();

    wxSelectionStore::IterationState cookie;
    for ( unsigned row = selections.GetFirstSelectedItem(cookie);
          row != wxSelectionStore::NO_SELECTION;
          row = selections.GetNextSelectedItem(cookie) )
    {
        wxDataViewItem item = m_clientArea->GetItemByRow(row);
        if ( item.IsOk() )
        {
            sel.Add(item);
        }
        else
        {
            wxFAIL_MSG( "invalid item in selection - bad internal state" );
        }
    }

    return sel.size();
}

void wxDataViewCtrl::SetSelections( const wxDataViewItemArray & sel )
{
    m_clientArea->ClearSelection();

    if ( sel.empty() )
        return;

    wxDataViewItem last_parent;

    for ( size_t i = 0; i < sel.size(); i++ )
    {
        wxDataViewItem item = sel[i];
        wxDataViewItem parent = GetModel()->GetParent( item );
        if (parent)
        {
            if (parent != last_parent)
                ExpandAncestors(item);
        }

        last_parent = parent;
        int row = m_clientArea->GetRowByItem( item );
        if( row >= 0 )
            m_clientArea->SelectRow(static_cast<unsigned int>(row), true);
    }

    // Also make the last item as current item
    DoSetCurrentItem(sel.Last());
}

void wxDataViewCtrl::Select( const wxDataViewItem & item )
{
    ExpandAncestors( item );

    int row = m_clientArea->GetRowByItem( item );
    if( row >= 0 )
    {
        // Unselect all rows before select another in the single select mode
        if (m_clientArea->IsSingleSel())
            m_clientArea->UnselectAllRows();

        m_clientArea->SelectRow(row, true);

        // Also set focus to the selected item
        m_clientArea->ChangeCurrentRow( row );
    }
}

void wxDataViewCtrl::Unselect( const wxDataViewItem & item )
{
    int row = m_clientArea->GetRowByItem( item );
    if( row >= 0 )
        m_clientArea->SelectRow(row, false);
}

bool wxDataViewCtrl::IsSelected( const wxDataViewItem & item ) const
{
    int row = m_clientArea->GetRowByItem( item );
    if( row >= 0 )
    {
        return m_clientArea->IsRowSelected(row);
    }
    return false;
}

bool wxDataViewCtrl::SetHeaderAttr(const wxItemAttr& attr)
{
    if ( !m_headerArea )
        return false;

    // Call all functions unconditionally to reset the previously set
    // attributes, if any.
    m_headerArea->SetForegroundColour(attr.GetTextColour());
    m_headerArea->SetBackgroundColour(attr.GetBackgroundColour());
    m_headerArea->SetFont(attr.GetFont());

    // If the font has changed, the size of the header might need to be
    // updated.
    Layout();

    return true;
}

void wxDataViewCtrl::SetAlternateRowColour(const wxColour& colour)
{
    m_alternateRowColour = colour;
}

void wxDataViewCtrl::SelectAll()
{
    m_clientArea->SelectAllRows();
}

void wxDataViewCtrl::UnselectAll()
{
    m_clientArea->UnselectAllRows();
}

void wxDataViewCtrl::EnsureVisibleRowCol( int row, int column )
{
    if( row < 0 )
        row = 0;
    if( row > (int) m_clientArea->GetRowCount() )
        row = m_clientArea->GetRowCount();

    int first = m_clientArea->GetFirstVisibleRow();
    int last = m_clientArea->GetLastFullyVisibleRow();
    if( row < first )
        m_clientArea->ScrollTo( row, column );
    else if( row > last )
        m_clientArea->ScrollTo( row - last + first, column );
    else
        m_clientArea->ScrollTo( first, column );
}

void wxDataViewCtrl::EnsureVisible( const wxDataViewItem & item, const wxDataViewColumn * column )
{
    ExpandAncestors( item );

    m_clientArea->RecalculateDisplay();

    int row = m_clientArea->GetRowByItem(item);
    if( row >= 0 )
    {
        if( column == NULL )
            EnsureVisibleRowCol(row, -1);
        else
            EnsureVisibleRowCol( row, GetColumnIndex(column) );
    }

}

void wxDataViewCtrl::HitTest( const wxPoint & point, wxDataViewItem & item,
                              wxDataViewColumn* &column ) const
{
    // Convert from wxDataViewCtrl coordinates to wxDataViewMainWindow coordinates.
    // (They can be different due to the presence of the header.).
    const wxPoint clientPt = m_clientArea->ScreenToClient(ClientToScreen(point));
    m_clientArea->HitTest(clientPt, item, column);
}

wxRect wxDataViewCtrl::GetItemRect( const wxDataViewItem & item,
                                    const wxDataViewColumn* column ) const
{
    // Convert position from the main window coordinates to the control coordinates.
    // (They can be different due to the presence of the header.).
    wxRect r = m_clientArea->GetItemRect(item, column);
    const wxPoint ctrlPos = ScreenToClient(m_clientArea->ClientToScreen(r.GetPosition()));
    r.SetPosition(ctrlPos);
    return r;
}

wxDataViewItem wxDataViewCtrl::GetItemByRow( unsigned int row ) const
{
    return m_clientArea->GetItemByRow( row );
}

int wxDataViewCtrl::GetRowByItem( const wxDataViewItem & item ) const
{
    return m_clientArea->GetRowByItem( item );
}

void wxDataViewCtrl::DoExpand( const wxDataViewItem & item )
{
    int row = m_clientArea->GetRowByItem( item );
    if (row != -1)
        m_clientArea->Expand(row);
}

void wxDataViewCtrl::Collapse( const wxDataViewItem & item )
{
    int row = m_clientArea->GetRowByItem( item );
    if (row != -1)
        m_clientArea->Collapse(row);
}

bool wxDataViewCtrl::IsExpanded( const wxDataViewItem & item ) const
{
    int row = m_clientArea->GetRowByItem( item );
    if (row != -1)
        return m_clientArea->IsExpanded(row);
    return false;
}

void wxDataViewCtrl::EditItem(const wxDataViewItem& item, const wxDataViewColumn *column)
{
    wxCHECK_RET( item.IsOk(), "invalid item" );
    wxCHECK_RET( column, "no column provided" );

    m_clientArea->StartEditing(item, column);
}

void wxDataViewCtrl::ResetAllSortColumns()
{
    // Must make copy, because unsorting will remove it from original vector
    wxVector<int> const copy(m_sortingColumnIdxs);
    for ( wxVector<int>::const_iterator it = copy.begin(),
                                       end = copy.end();
          it != end;
          ++it )
    {
        GetColumn(*it)->UnsetAsSortKey();
    }

    wxASSERT( m_sortingColumnIdxs.empty() );
}

bool wxDataViewCtrl::AllowMultiColumnSort(bool allow)
{
    if ( m_allowMultiColumnSort == allow )
        return true;

    m_allowMultiColumnSort = allow;

    // If disabling, must disable any multiple sort that are active
    if ( !allow )
    {
        ResetAllSortColumns();

        if ( wxDataViewModel *model = GetModel() )
            model->Resort();
    }

    return true;
}


bool wxDataViewCtrl::IsColumnSorted(int idx) const
{
    for ( wxVector<int>::const_iterator it = m_sortingColumnIdxs.begin(),
                                       end = m_sortingColumnIdxs.end();
          it != end;
          ++it )
    {
        if ( *it == idx )
            return true;
    }

    return false;
}

void wxDataViewCtrl::UseColumnForSorting(int idx )
{
    m_sortingColumnIdxs.push_back(idx);
}

void wxDataViewCtrl::DontUseColumnForSorting(int idx)
{
    for ( wxVector<int>::iterator it = m_sortingColumnIdxs.begin(),
                                 end = m_sortingColumnIdxs.end();
          it != end;
          ++it )
    {
        if ( *it == idx )
        {
            m_sortingColumnIdxs.erase(it);
            return;
        }
    }

    wxFAIL_MSG( "Column is not used for sorting" );
}

void wxDataViewCtrl::ToggleSortByColumn(int column)
{
    m_headerArea->ToggleSortByColumn(column);
}

void wxDataViewCtrl::DoEnableSystemTheme(bool enable, wxWindow* window)
{
    typedef wxSystemThemedControl<wxControl> Base;
    Base::DoEnableSystemTheme(enable, window);
    Base::DoEnableSystemTheme(enable, m_clientArea);
    if ( m_headerArea )
        Base::DoEnableSystemTheme(enable, m_headerArea);
}

#if wxUSE_ACCESSIBILITY
wxAccessible* wxDataViewCtrl::CreateAccessible()
{
    return new wxDataViewCtrlAccessible(this);
}
#endif // wxUSE_ACCESSIBILITY

#if wxUSE_ACCESSIBILITY
//-----------------------------------------------------------------------------
// wxDataViewCtrlAccessible
//-----------------------------------------------------------------------------

wxDataViewCtrlAccessible::wxDataViewCtrlAccessible(wxDataViewCtrl* win)
    : wxWindowAccessible(win)
{
}

// Can return either a child object, or an integer
// representing the child element, starting from 1.
wxAccStatus wxDataViewCtrlAccessible::HitTest(const wxPoint& pt,
                            int* childId, wxAccessible** childObject)
{
    wxDataViewCtrl* dvCtrl = wxDynamicCast(GetWindow(), wxDataViewCtrl);
    wxCHECK( dvCtrl, wxACC_FAIL );

    wxDataViewItem item;
    wxDataViewColumn* col;
    const wxPoint posCtrl = dvCtrl->ScreenToClient(pt);
    dvCtrl->HitTest(posCtrl, item, col);
    if ( item.IsOk() )
    {
        *childId = dvCtrl->GetRowByItem(item)+1;
        *childObject = NULL;
    }
    else
    {
        if( ((wxWindow*)dvCtrl)->HitTest(posCtrl) == wxHT_WINDOW_INSIDE )
        {
            // First check if provided point belongs to the header
            // because header control handles accesibility requestes on its own.
            wxHeaderCtrl* dvHdr = dvCtrl->GenericGetHeader();
            if ( dvHdr )
            {
                const wxPoint posHdr = dvHdr->ScreenToClient(pt);
                if ( dvHdr->HitTest(posHdr) == wxHT_WINDOW_INSIDE )
                {
                    *childId = wxACC_SELF;
                    *childObject = dvHdr->GetOrCreateAccessible();
                    return wxACC_OK;
                }
            }

            *childId = wxACC_SELF;
            *childObject = this;
        }
        else
        {
            *childId = wxACC_SELF;
            *childObject = NULL;
        }
    }

    return wxACC_OK;
}

// Returns the rectangle for this object (id = 0) or a child element (id > 0).
wxAccStatus wxDataViewCtrlAccessible::GetLocation(wxRect& rect, int elementId)
{
    wxDataViewCtrl* dvCtrl = wxDynamicCast(GetWindow(), wxDataViewCtrl);
    wxCHECK( dvCtrl, wxACC_FAIL );
    wxDataViewMainWindow* dvWnd = wxDynamicCast(dvCtrl->GetMainWindow(), wxDataViewMainWindow);

    if ( elementId == wxACC_SELF )
    {
        // Header accesibility requestes are handled separately
        // so header is excluded from effective client area
        // and hence only main window area is reported.
        rect = dvWnd->GetScreenRect();
    }
    else
    {
        wxDataViewItem item = dvWnd->GetItemByRow(elementId-1);
        if ( !item.IsOk() )
        {
            return wxACC_NOT_IMPLEMENTED;
        }

        rect = dvWnd->GetItemRect(item, NULL);
        // Indentation and expander column should be included here and therefore
        // reported row width should by the same as the width of the client area.
        rect.width += rect.x;
        rect.x = 0;
        wxPoint posScreen = dvWnd->ClientToScreen(rect.GetPosition());
        rect.SetPosition(posScreen);
    }

    return wxACC_OK;
}

// Navigates from fromId to toId/toObject.
wxAccStatus wxDataViewCtrlAccessible::Navigate(wxNavDir navDir, int fromId,
            int* toId, wxAccessible** toObject)
{
    wxDataViewCtrl* dvCtrl = wxDynamicCast(GetWindow(), wxDataViewCtrl);
    wxCHECK( dvCtrl, wxACC_FAIL );
    wxDataViewMainWindow* dvWnd = wxDynamicCast(dvCtrl->GetMainWindow(), wxDataViewMainWindow);

    const int numRows = (int)dvWnd->GetRowCount();

    if ( fromId == wxACC_SELF )
    {
        switch ( navDir )
        {
        case wxNAVDIR_FIRSTCHILD:
            if ( numRows > 0 )
            {
                *toId = 1;
                *toObject = NULL;
                return wxACC_OK;
            }
            return wxACC_FALSE;
        case wxNAVDIR_LASTCHILD:
            if ( numRows > 0 )
            {
                *toId = numRows;
                *toObject = NULL;
                return wxACC_OK;
            }
            return wxACC_FALSE;
        case wxNAVDIR_DOWN:
            wxFALLTHROUGH;
        case wxNAVDIR_NEXT:
            wxFALLTHROUGH;
        case wxNAVDIR_UP:
            wxFALLTHROUGH;
        case wxNAVDIR_PREVIOUS:
            wxFALLTHROUGH;
        case wxNAVDIR_LEFT:
            wxFALLTHROUGH;
        case wxNAVDIR_RIGHT:
            // Standard wxWindow navigation is applicable here.
            return wxWindowAccessible::Navigate(navDir, fromId, toId, toObject);
        }
    }
    else
    {
        switch ( navDir )
        {
        case wxNAVDIR_FIRSTCHILD:
            return wxACC_FALSE;
        case wxNAVDIR_LASTCHILD:
            return wxACC_FALSE;
        case wxNAVDIR_LEFT:
            return wxACC_FALSE;
        case wxNAVDIR_RIGHT:
            return wxACC_FALSE;
        case wxNAVDIR_DOWN:
            wxFALLTHROUGH;
        case wxNAVDIR_NEXT:
            if ( fromId < numRows )
            {
                *toId = fromId + 1;
                *toObject = NULL;
                return wxACC_OK;
            }
            return wxACC_FALSE;
        case wxNAVDIR_PREVIOUS:
            wxFALLTHROUGH;
        case wxNAVDIR_UP:
            if ( fromId > 1 )
            {
                *toId = fromId - 1;
                *toObject = NULL;
                return wxACC_OK;
            }
            return wxACC_FALSE;
        }
    }

    // Let the framework handle the other cases.
    return wxACC_NOT_IMPLEMENTED;
}

// Gets the name of the specified object.
wxAccStatus wxDataViewCtrlAccessible::GetName(int childId, wxString* name)
{
    wxDataViewCtrl* dvCtrl = wxDynamicCast(GetWindow(), wxDataViewCtrl);
    wxCHECK( dvCtrl, wxACC_FAIL );

    if ( childId == wxACC_SELF )
    {
        *name = dvCtrl->GetName();
    }
    else
    {
        wxDataViewItem item = dvCtrl->GetItemByRow(childId-1);
        if ( !item.IsOk() )
        {
            return wxACC_NOT_IMPLEMENTED;
        }

        // Name is the value in the first textual column
        // plus the name of this column:
        // Column1: Value1
        wxString itemName;

        wxDataViewModel* model = dvCtrl->GetModel();
        const unsigned int numCols = dvCtrl->GetColumnCount();
        for ( unsigned int col = 0; col < numCols; col++ )
        {
            wxDataViewColumn *dvCol = dvCtrl->GetColumnAt(col);
            if ( dvCol->IsHidden() )
                continue; // skip it

            wxVariant value;
            model->GetValue(value, item, dvCol->GetModelColumn());
            if ( value.IsNull() || value.IsType(wxS("bool")) )
                continue; // Skip non-textual items

            wxDataViewRenderer* r = dvCol->GetRenderer();
            r->PrepareForItem(model, item, dvCol->GetModelColumn());
            wxString vs = r->GetAccessibleDescription();
            if ( !vs.empty() )
            {
                itemName = vs;
                break;
            }
        }

        if ( itemName.empty() )
        {
            // Return row number if no textual column found.
            // Rows are numbered from 1.
            *name = wxString::Format(_("Row %i"), childId);
        }
        else
        {
            *name = itemName;
        }
    }

    return wxACC_OK;
}

// Gets the number of children.
wxAccStatus wxDataViewCtrlAccessible::GetChildCount(int* childCount)
{
    wxDataViewCtrl* dvCtrl = wxDynamicCast(GetWindow(), wxDataViewCtrl);
    wxCHECK( dvCtrl, wxACC_FAIL );
    wxDataViewMainWindow* dvWnd = wxDynamicCast(dvCtrl->GetMainWindow(), wxDataViewMainWindow);

    *childCount = (int)dvWnd->GetRowCount();
    return wxACC_OK;
}

// Gets the specified child (starting from 1).
// If *child is NULL and return value is wxACC_OK,
// this means that the child is a simple element and
// not an accessible object.
wxAccStatus wxDataViewCtrlAccessible::GetChild(int childId, wxAccessible** child)
{
    *child = (childId == wxACC_SELF) ? this : NULL;
    return wxACC_OK;
}

// Performs the default action. childId is 0 (the action for this object)
// or > 0 (the action for a child).
// Return wxACC_NOT_SUPPORTED if there is no default action for this
// window (e.g. an edit control).
wxAccStatus wxDataViewCtrlAccessible::DoDefaultAction(int childId)
{
    wxDataViewCtrl* dvCtrl = wxDynamicCast(GetWindow(), wxDataViewCtrl);
    wxCHECK( dvCtrl, wxACC_FAIL );

    if ( childId != wxACC_SELF )
    {
        wxDataViewMainWindow* dvWnd = wxDynamicCast(dvCtrl->GetMainWindow(), wxDataViewMainWindow);
        if ( !dvWnd->IsList() )
        {
            const unsigned int row = childId-1;
            wxDataViewTreeNode* node = dvWnd->GetTreeNodeByRow(row);
            if ( node )
            {
                if ( node->HasChildren() )
                {
                    // Expand or collapse the node.
                    if ( node->IsOpen() )
                        dvWnd->Collapse(row);
                    else
                        dvWnd->Expand(row);
                    return wxACC_OK;
                }
            }
        }
    }

    return wxACC_NOT_SUPPORTED;
}

// Gets the default action for this object (0) or > 0 (the action for a child).
// Return wxACC_OK even if there is no action. actionName is the action, or the empty
// string if there is no action.
// The retrieved string describes the action that is performed on an object,
// not what the object does as a result. For example, a toolbar button that prints
// a document has a default action of "Press" rather than "Prints the current document."
wxAccStatus wxDataViewCtrlAccessible::GetDefaultAction(int childId, wxString* actionName)
{
    wxDataViewCtrl* dvCtrl = wxDynamicCast(GetWindow(), wxDataViewCtrl);
    wxCHECK( dvCtrl, wxACC_FAIL );

    wxString action;
    if ( childId != wxACC_SELF )
    {
        wxDataViewMainWindow* dvWnd = wxDynamicCast(dvCtrl->GetMainWindow(), wxDataViewMainWindow);
        if ( !dvWnd->IsList() )
        {
            wxDataViewTreeNode* node = dvWnd->GetTreeNodeByRow(childId-1);
            if ( node )
            {
                if ( node->HasChildren() )
                {
                    if ( node->IsOpen() )
                        /* TRANSLATORS: Action for manipulating a tree control */
                        action = _("Collapse");
                    else
                        /* TRANSLATORS: Action for manipulating a tree control */
                        action = _("Expand");
                }
            }
        }
    }

    *actionName = action;
    return wxACC_OK;
}

// Returns the description for this object or a child.
wxAccStatus wxDataViewCtrlAccessible::GetDescription(int childId, wxString* description)
{
    wxDataViewCtrl* dvCtrl = wxDynamicCast(GetWindow(), wxDataViewCtrl);
    wxCHECK( dvCtrl, wxACC_FAIL );

    if ( childId == wxACC_SELF )
    {
        wxDataViewMainWindow* dvWnd = wxDynamicCast(dvCtrl->GetMainWindow(), wxDataViewMainWindow);
        *description = wxString::Format(_("%s (%d items)"),
                                        dvCtrl->GetName().c_str(), dvWnd->GetRowCount());
    }
    else
    {
        wxDataViewItem item = dvCtrl->GetItemByRow(childId-1);
        if ( !item.IsOk() )
        {
            return wxACC_NOT_IMPLEMENTED;
        }

        // Description is concatenation of the contents of items in all columns:
        // Column1: Value1, Column2: Value2, ...
        // First textual item should be skipped because it is returned
        // as a Name property.
        wxString itemDesc;

        bool firstTextSkipped = false;
        wxDataViewModel* model = dvCtrl->GetModel();
        const unsigned int numCols = dvCtrl->GetColumnCount();
        for ( unsigned int col = 0; col < numCols; col++ )
        {
            if ( model->IsContainer(item) && !model->HasContainerColumns(item) )
                continue; // skip it

            wxDataViewColumn *dvCol = dvCtrl->GetColumnAt(col);
            if ( dvCol->IsHidden() )
                continue; // skip it

            wxVariant value;
            model->GetValue(value, item, dvCol->GetModelColumn());

            wxDataViewRenderer* r = dvCol->GetRenderer();
            r->PrepareForItem(model, item, dvCol->GetModelColumn());
            wxString valStr = r->GetAccessibleDescription();
            // Skip first textual item
            if ( !firstTextSkipped && !value.IsNull() && !value.IsType(wxS("bool")) && !valStr.empty() )
            {
                firstTextSkipped = true;
                continue;
            }

            if ( !valStr.empty() )
            {
                wxString colName = dvCol->GetTitle();
                // If column has no label then present its index.
                if ( colName.empty() )
                {
                    // Columns are numbered from 1.
                    colName = wxString::Format(_("Column %u"), col+1);
                }

                if ( !itemDesc.empty() )
                    itemDesc.Append(wxS(", "));
                itemDesc.Append(colName);
                itemDesc.Append(wxS(": "));
                itemDesc.Append(valStr);
            }
        }

        *description = itemDesc;
    }

    return wxACC_OK;
}

// Returns help text for this object or a child, similar to tooltip text.
wxAccStatus wxDataViewCtrlAccessible::GetHelpText(int childId, wxString* helpText)
{
    wxDataViewCtrl* dvCtrl = wxDynamicCast(GetWindow(), wxDataViewCtrl);
    wxCHECK( dvCtrl, wxACC_FAIL );

#if wxUSE_HELP
    if ( childId == wxACC_SELF )
    {
        *helpText = dvCtrl->GetHelpText();
    }
    else
    {
        wxDataViewItem item = dvCtrl->GetItemByRow(childId-1);
        if ( item.IsOk() )
        {
            wxDataViewMainWindow* dvWnd = wxDynamicCast(dvCtrl->GetMainWindow(), wxDataViewMainWindow);
            wxRect rect = dvWnd->GetItemRect(item, NULL);
            *helpText = dvWnd->GetHelpTextAtPoint(rect.GetPosition(), wxHelpEvent::Origin_Keyboard);
        }
        else
        {
            *helpText = wxEmptyString;
        }
    }
    return wxACC_OK;
#else
    (void)childId;
    (void)helpText;
    return wxACC_NOT_IMPLEMENTED;
#endif
}

// Returns the keyboard shortcut for this object or child.
// Return e.g. ALT+K
wxAccStatus wxDataViewCtrlAccessible::GetKeyboardShortcut(int childId, wxString* shortcut)
{
    wxDataViewCtrl* dvCtrl = wxDynamicCast(GetWindow(), wxDataViewCtrl);
    wxCHECK( dvCtrl, wxACC_FAIL );

    if ( childId != wxACC_SELF )
    {
        wxDataViewMainWindow* dvWnd = wxDynamicCast(dvCtrl->GetMainWindow(), wxDataViewMainWindow);
        if ( !dvWnd->IsList() )
        {
            wxDataViewTreeNode* node = dvWnd->GetTreeNodeByRow(childId-1);
            if ( node )
            {
                if ( node->HasChildren() )
                {
                    if ( node->IsOpen() )
                        /* TRANSLATORS: Keystroke for manipulating a tree control */
                        *shortcut = _("Left");
                    else
                        /* TRANSLATORS: Keystroke for manipulating a tree control */
                        *shortcut = _("Right");

                    return wxACC_OK;
                }
            }
        }
    }

    return wxACC_FALSE;
}

// Returns a role constant.
wxAccStatus wxDataViewCtrlAccessible::GetRole(int childId, wxAccRole* role)
{
    wxDataViewCtrl* dvCtrl = wxDynamicCast(GetWindow(), wxDataViewCtrl);
    wxCHECK( dvCtrl, wxACC_FAIL );
    wxDataViewMainWindow* dvWnd = wxDynamicCast(dvCtrl->GetMainWindow(), wxDataViewMainWindow);

    if ( childId == wxACC_SELF )
        *role = dvWnd->IsList() ? wxROLE_SYSTEM_LIST : wxROLE_SYSTEM_OUTLINE;
    else
        *role = dvWnd->IsList() ? wxROLE_SYSTEM_LISTITEM : wxROLE_SYSTEM_OUTLINEITEM;

    return wxACC_OK;
}

// Returns a state constant.
wxAccStatus wxDataViewCtrlAccessible::GetState(int childId, long* state)
{
    wxDataViewCtrl* dvCtrl = wxDynamicCast(GetWindow(), wxDataViewCtrl);
    wxCHECK( dvCtrl, wxACC_FAIL );
    wxDataViewMainWindow* dvWnd = wxDynamicCast(dvCtrl->GetMainWindow(), wxDataViewMainWindow);

    long st = 0;
    // State flags common to the object and its children.
    if ( !dvWnd->IsEnabled() )
        st |= wxACC_STATE_SYSTEM_UNAVAILABLE;
    if ( !dvWnd->IsShown() )
        st |= wxACC_STATE_SYSTEM_INVISIBLE;

    if ( childId == wxACC_SELF )
    {
        if( dvWnd->IsFocusable() )
            st |= wxACC_STATE_SYSTEM_FOCUSABLE;
        if ( dvWnd->HasFocus() )
            st |=  wxACC_STATE_SYSTEM_FOCUSED;
    }
    else
    {
        const unsigned int rowNum = childId-1;

        if( dvWnd->IsFocusable() )
            st |= wxACC_STATE_SYSTEM_FOCUSABLE | wxACC_STATE_SYSTEM_SELECTABLE;
        if ( !dvWnd->IsSingleSel() )
            st |= wxACC_STATE_SYSTEM_MULTISELECTABLE | wxACC_STATE_SYSTEM_EXTSELECTABLE;

        if ( rowNum < dvWnd->GetFirstVisibleRow() || rowNum > dvWnd->GetLastFullyVisibleRow() )
            st |= wxACC_STATE_SYSTEM_OFFSCREEN;
        if ( dvWnd->GetCurrentRow() == rowNum )
            st |= wxACC_STATE_SYSTEM_FOCUSED;
        if ( dvWnd->IsRowSelected(rowNum) )
            st |= wxACC_STATE_SYSTEM_SELECTED;

        if ( !dvWnd->IsList() )
        {
            wxDataViewTreeNode* node = dvWnd->GetTreeNodeByRow(rowNum);
            if ( node )
            {
                if ( node->HasChildren() )
                {
                    if ( node->IsOpen() )
                        st |= wxACC_STATE_SYSTEM_EXPANDED;
                    else
                        st |= wxACC_STATE_SYSTEM_COLLAPSED;
                }
            }
        }
    }
    *state = st;
    return wxACC_OK;
}

// Returns a localized string representing the value for the object
// or child.
wxAccStatus wxDataViewCtrlAccessible::GetValue(int childId, wxString* strValue)
{
    wxDataViewCtrl* dvCtrl = wxDynamicCast(GetWindow(), wxDataViewCtrl);
    wxCHECK( dvCtrl, wxACC_FAIL );

    wxString val;

    if ( childId != wxACC_SELF )
    {
        wxDataViewMainWindow* dvWnd = wxDynamicCast(dvCtrl->GetMainWindow(), wxDataViewMainWindow);
        if ( !dvWnd->IsList() )
        {
            // In the tree view each item within the control has a zero-based value
            // that represents its level within the hierarchy and this value
            // is returned as a Value property.
            wxDataViewTreeNode *node = dvWnd->GetTreeNodeByRow(childId-1);
            if ( node )
            {
                val = wxString::Format(wxS("%i"), node->GetIndentLevel());
            }
        }
    }

    *strValue = val;
    return wxACC_OK;
}

// Selects the object or child.
wxAccStatus wxDataViewCtrlAccessible::Select(int childId, wxAccSelectionFlags selectFlags)
{
    wxDataViewCtrl* dvCtrl = wxDynamicCast(GetWindow(), wxDataViewCtrl);
    wxCHECK( dvCtrl, wxACC_FAIL );
    wxDataViewMainWindow* dvWnd = wxDynamicCast(dvCtrl->GetMainWindow(), wxDataViewMainWindow);

    if ( childId == wxACC_SELF )
    {
        if ( selectFlags == wxACC_SEL_TAKEFOCUS )
        {
            dvWnd->SetFocus();
        }
        else if ( selectFlags != wxACC_SEL_NONE )
        {
            wxFAIL_MSG( wxS("Invalid selection flag") );
            return wxACC_INVALID_ARG;
        }
    }
    else
    {
        // These flags are not allowed in the single-selection mode:
        if ( dvWnd->IsSingleSel() &&
             selectFlags & (wxACC_SEL_EXTENDSELECTION | wxACC_SEL_ADDSELECTION | wxACC_SEL_REMOVESELECTION) )
        {
            wxFAIL_MSG( wxS("Invalid selection flag") );
            return wxACC_INVALID_ARG;
        }

        const int row = childId-1;

        if ( selectFlags == wxACC_SEL_TAKEFOCUS )
        {
            dvWnd->ChangeCurrentRow(row);
        }
        else if ( selectFlags & wxACC_SEL_TAKESELECTION )
        {
            // This flag must not be combined with the following flags:
            if ( selectFlags & (wxACC_SEL_EXTENDSELECTION | wxACC_SEL_ADDSELECTION | wxACC_SEL_REMOVESELECTION) )
            {
                wxFAIL_MSG( wxS("Invalid selection flag") );
                return wxACC_INVALID_ARG;
            }

            dvWnd->UnselectAllRows();
            dvWnd->SelectRow(row, true);
            if ( selectFlags & wxACC_SEL_TAKEFOCUS || dvWnd->IsSingleSel() )
            {
                dvWnd->ChangeCurrentRow(row);
            }
        }
        else if ( selectFlags & wxACC_SEL_EXTENDSELECTION )
        {
            // This flag must not be combined with the following flag:
            if ( selectFlags & wxACC_SEL_TAKESELECTION )
            {
                wxFAIL_MSG( wxS("Invalid selection flag") );
                return wxACC_INVALID_ARG;
            }
            // These flags cannot be set together:
            if ( (selectFlags & (wxACC_SEL_ADDSELECTION | wxACC_SEL_REMOVESELECTION))
                 == (wxACC_SEL_ADDSELECTION | wxACC_SEL_REMOVESELECTION) )
            {
                wxFAIL_MSG( wxS("Invalid selection flag") );
                return wxACC_INVALID_ARG;
            }

            // We have to have a focused object as a selection anchor.
            unsigned int focusedRow = dvWnd->GetCurrentRow();
            if ( focusedRow == (unsigned int)-1 )
            {
                wxFAIL_MSG( wxS("No selection anchor") );
                return wxACC_INVALID_ARG;
            }

            bool doSelect;
            if ( selectFlags & wxACC_SEL_ADDSELECTION )
                doSelect = true;
            else if ( selectFlags & wxACC_SEL_REMOVESELECTION )
                doSelect = false;
            else
                // If the anchor object is selected, the selection is extended.
                // If the anchor object is not selected, all objects are unselected.
                doSelect = dvWnd->IsRowSelected(focusedRow);

            if ( doSelect )
            {
                dvWnd->SelectRows(focusedRow, row);
            }
            else
            {
                for( int r = focusedRow; r <= row; r++ )
                    dvWnd->SelectRow(r, false);
            }

            if ( selectFlags & wxACC_SEL_TAKEFOCUS )
            {
                dvWnd->ChangeCurrentRow(row);
            }
        }
        else if ( selectFlags & wxACC_SEL_ADDSELECTION )
        {
            // This flag must not be combined with the following flags:
            if ( selectFlags & (wxACC_SEL_TAKESELECTION | wxACC_SEL_REMOVESELECTION) )
            {
                wxFAIL_MSG( wxS("Invalid selection flag") );
                return wxACC_INVALID_ARG;
            }

            // Combination with wxACC_SEL_EXTENDSELECTION is already handled
            // (see wxACC_SEL_EXTENDSELECTION block).
            dvWnd->SelectRow(row, true);
            if ( selectFlags & wxACC_SEL_TAKEFOCUS )
            {
                dvWnd->ChangeCurrentRow(row);
            }
        }
        else if ( selectFlags & wxACC_SEL_REMOVESELECTION )
        {
            // This flag must not be combined with the following flags:
            if ( selectFlags & (wxACC_SEL_TAKESELECTION | wxACC_SEL_ADDSELECTION) )
            {
                wxFAIL_MSG( wxS("Invalid selection flag") );
                return wxACC_INVALID_ARG;
            }

            // Combination with wxACC_SEL_EXTENDSELECTION is already handled
            // (see wxACC_SEL_EXTENDSELECTION block).
            dvWnd->SelectRow(row, false);
            if ( selectFlags & wxACC_SEL_TAKEFOCUS )
            {
                dvWnd->ChangeCurrentRow(row);
            }
        }
    }

    return wxACC_OK;
}

// Gets the window with the keyboard focus.
// If childId is 0 and child is NULL, no object in
// this subhierarchy has the focus.
// If this object has the focus, child should be 'this'.
wxAccStatus wxDataViewCtrlAccessible::GetFocus(int* childId, wxAccessible** child)
{
    wxDataViewCtrl* dvCtrl = wxDynamicCast(GetWindow(), wxDataViewCtrl);
    wxCHECK( dvCtrl, wxACC_FAIL );
    wxDataViewMainWindow* dvWnd = wxDynamicCast(dvCtrl->GetMainWindow(), wxDataViewMainWindow);

    const unsigned int row = dvWnd->GetCurrentRow();
    if ( row != (unsigned int)*childId-1 )
    {
        *childId = row+1;
        *child = NULL;
    }
    else
    {
        // First check if header is focused because header control
        // handles accesibility requestes on its own.
        wxHeaderCtrl* dvHdr = dvCtrl->GenericGetHeader();
        if ( dvHdr )
        {
            if ( dvHdr->HasFocus() )
            {
                *childId = wxACC_SELF;
                *child = dvHdr->GetOrCreateAccessible();
                return wxACC_OK;
            }
        }

        if ( dvWnd->HasFocus() )
        {
            *childId = wxACC_SELF;
            *child = this;
        }
        else
        {
            *childId = 0;
            *child = NULL;
        }
    }

    return wxACC_OK;
}

// Gets a variant representing the selected children
// of this object.
// Acceptable values:
// - a null variant (IsNull() returns true)
// - a "void*" pointer to a wxAccessible child object
// - an integer representing the selected child element,
//   or 0 if this object is selected (GetType() == wxT("long"))
// - a list variant (GetType() == wxT("list"))
wxAccStatus wxDataViewCtrlAccessible::GetSelections(wxVariant* selections)
{
    wxDataViewCtrl* dvCtrl = wxDynamicCast(GetWindow(), wxDataViewCtrl);
    wxCHECK( dvCtrl, wxACC_FAIL );

    wxDataViewItemArray sel;
    dvCtrl->GetSelections(sel);
    if ( sel.IsEmpty() )
    {
        selections->MakeNull();
    }
    else
    {
        wxVariantList tempList;
        wxVariant v(tempList);

        for( size_t i = 0; i < sel.GetCount(); i++ )
        {
            int row = dvCtrl->GetRowByItem(sel[i]);
            v.Append(wxVariant((long)row+1));
        }

        // Don't return the list if one child is selected.
        if ( v.GetCount() == 1 )
            *selections = wxVariant(v[0].GetLong());
        else
            *selections = v;
    }

    return wxACC_OK;
}
#endif // wxUSE_ACCESSIBILITY

#endif // !wxUSE_GENERICDATAVIEWCTRL

#endif // wxUSE_DATAVIEWCTRL
