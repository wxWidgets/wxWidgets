/////////////////////////////////////////////////////////////////////////////
// Name:        wx/generic/dataview.h
// Purpose:     wxDataViewCtrl generic implementation header
// Author:      Robert Roebling
// Modified By: Bo Yang
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef __GENERICDATAVIEWCTRLH__
#define __GENERICDATAVIEWCTRLH__

#include "wx/defs.h"
#include "wx/object.h"
#include "wx/compositewin.h"
#include "wx/control.h"
#include "wx/scrolwin.h"
#include "wx/icon.h"
#include "wx/vector.h"
#if wxUSE_ACCESSIBILITY
    #include "wx/access.h"
#endif // wxUSE_ACCESSIBILITY

class WXDLLIMPEXP_FWD_CORE wxDataViewMainWindow;
class WXDLLIMPEXP_FWD_CORE wxDataViewHeaderWindow;
class WXDLLIMPEXP_FWD_CORE wxTreeListCtrl;
#if wxUSE_ACCESSIBILITY
class WXDLLIMPEXP_FWD_CORE wxDataViewCtrlAccessible;
#endif // wxUSE_ACCESSIBILITY

// ---------------------------------------------------------
// wxDataViewColumn
// ---------------------------------------------------------

class WXDLLIMPEXP_CORE wxDataViewColumn : public wxDataViewColumnBase
{
public:
    wxDataViewColumn(const wxString& title,
                     wxDataViewRenderer *renderer,
                     unsigned int model_column,
                     int width = wxDVC_DEFAULT_WIDTH,
                     wxAlignment align = wxALIGN_CENTER,
                     int flags = wxDATAVIEW_COL_RESIZABLE)
        : wxDataViewColumnBase(renderer, model_column),
          m_title(title)
    {
        Init(width, align, flags);
    }

    wxDataViewColumn(const wxBitmapBundle& bitmap,
                     wxDataViewRenderer *renderer,
                     unsigned int model_column,
                     int width = wxDVC_DEFAULT_WIDTH,
                     wxAlignment align = wxALIGN_CENTER,
                     int flags = wxDATAVIEW_COL_RESIZABLE)
        : wxDataViewColumnBase(bitmap, renderer, model_column)
    {
        Init(width, align, flags);
    }

    // implement wxHeaderColumnBase methods
    virtual void SetTitle(const wxString& title) override
    {
        m_title = title;
        UpdateWidth();
    }
    virtual wxString GetTitle() const override
    {
        return m_title;
    }

    virtual void SetWidth(int width) override
    {
        // Call the actual update method, used for both automatic and "manual"
        // width changes.
        WXUpdateWidth(width);

        // Do remember the last explicitly set width: this is used to prevent
        // UpdateColumnSizes() from resizing the last column to be smaller than
        // this size.
        m_manuallySetWidth = width;
    }
    virtual int GetWidth() const override;

    virtual void SetMinWidth(int minWidth) override
    {
        m_minWidth = minWidth;
        UpdateWidth();
    }
    virtual int GetMinWidth() const override
    {
        return m_minWidth;
    }

    virtual void SetAlignment(wxAlignment align) override
    {
        m_align = align;
        UpdateDisplay();
    }
    virtual wxAlignment GetAlignment() const override
    {
        return m_align;
    }

    virtual void SetFlags(int flags) override
    {
        m_flags = flags;
        UpdateDisplay();
    }
    virtual int GetFlags() const override
    {
        return m_flags;
    }

    virtual bool IsSortKey() const override
    {
        return m_sort;
    }

    virtual void UnsetAsSortKey() override;

    virtual void SetSortOrder(bool ascending) override;

    virtual bool IsSortOrderAscending() const override
    {
        return m_sortAscending;
    }

    virtual void SetBitmap( const wxBitmapBundle& bitmap ) override
    {
        wxDataViewColumnBase::SetBitmap(bitmap);
        UpdateWidth();
    }

    // This method is specific to the generic implementation and is used only
    // by wxWidgets itself.
    void WXUpdateWidth(int width)
    {
        if ( width == m_width )
            return;

        m_width = width;
        UpdateWidth();
    }

    // This method is also internal and called when the column is resized by
    // user interactively.
    void WXOnResize(int width);

    // Copy all presentation and sorting state, but neither the renderer,
    // owner nor model-column mapping. Used when wxTreeListCtrl promotes a
    // regular column to its renderer-special primary column.
    void WXCopyStateFrom(const wxDataViewColumn& other)
    {
        m_title = other.m_title;
        m_bitmap = other.m_bitmap;
        m_width = other.m_width;
        m_manuallySetWidth = other.m_manuallySetWidth;
        m_minWidth = other.m_minWidth;
        m_align = other.m_align;
        m_flags = other.m_flags;
        m_sort = other.m_sort;
        m_sortAscending = other.m_sortAscending;
    }

    virtual int WXGetSpecifiedWidth() const override;

private:
    // common part of all ctors
    void Init(int width, wxAlignment align, int flags);

    // Change the sort state without acquiring the source-side transaction.
    // wxDataViewCtrl uses this while resetting a snapshotted set of keys.
    void DoUnsetAsSortKey();

    // These methods forward to wxDataViewCtrl::OnColumnChange() and
    // OnColumnWidthChange() respectively, i.e. the latter is stronger than the
    // former.
    void UpdateDisplay();
    void UpdateWidth();

    // Return the effective value corresponding to the given width, handling
    // its negative values such as wxCOL_WIDTH_DEFAULT.
    int DoGetEffectiveWidth(int width) const;


    wxString m_title;
    int m_width,
        m_manuallySetWidth,
        m_minWidth;
    wxAlignment m_align;
    int m_flags;
    bool m_sort,
         m_sortAscending;

    friend class wxDataViewHeaderWindowBase;
    friend class wxDataViewHeaderWindow;
    friend class wxDataViewHeaderWindowMSW;
    friend class wxDataViewCtrl;
};

// ---------------------------------------------------------
// wxDataViewCtrl
// ---------------------------------------------------------

class WXDLLIMPEXP_CORE wxDataViewCtrl
    : public wxCompositeWindow<wxDataViewCtrlBase>,
      public wxScrollHelper
{
    friend class wxDataViewMainWindow;
    friend class wxDataViewHeaderWindowBase;
    friend class wxDataViewHeaderWindow;
    friend class wxDataViewHeaderWindowMSW;
    friend class wxDataViewColumn;
    friend class wxDataViewCallbackContext;
#if wxUSE_ACCESSIBILITY
    friend class wxDataViewCtrlAccessible;
#endif // wxUSE_ACCESSIBILITY

    typedef wxCompositeWindow<wxDataViewCtrlBase> BaseType;

public:
    wxDataViewCtrl() : wxScrollHelper(this)
    {
        Init();
    }

    wxDataViewCtrl( wxWindow *parent, wxWindowID id,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize, long style = 0,
           const wxValidator& validator = wxDefaultValidator,
           const wxString& name = wxASCII_STR(wxDataViewCtrlNameStr) )
             : wxScrollHelper(this)
    {
        Create(parent, id, pos, size, style, validator, name);
    }

    virtual ~wxDataViewCtrl();

    void Init();

    bool Create(wxWindow *parent, wxWindowID id,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize, long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxASCII_STR(wxDataViewCtrlNameStr));

    virtual bool AssociateModel( wxDataViewModel *model ) override;

    virtual bool AppendColumn( wxDataViewColumn *col ) override;
    virtual bool PrependColumn( wxDataViewColumn *col ) override;
    virtual bool InsertColumn( unsigned int pos, wxDataViewColumn *col ) override;

    virtual void DoSetExpanderColumn() override;
    virtual void DoSetIndent() override;

    virtual unsigned int GetColumnCount() const override;
    virtual wxDataViewColumn* GetColumn( unsigned int pos ) const override;
    virtual bool DeleteColumn( wxDataViewColumn *column ) override;
    virtual bool ClearColumns() override;
    virtual int GetColumnPosition( const wxDataViewColumn *column ) const override;

    virtual wxDataViewColumn *GetSortingColumn() const override;
    virtual wxVector<wxDataViewColumn *> GetSortingColumns() const override;

    virtual wxDataViewItem GetTopItem() const override;
    virtual int GetCountPerPage() const override;

    virtual int GetSelectedItemsCount() const override;
    virtual int GetSelections( wxDataViewItemArray & sel ) const override;
    virtual void SetSelections( const wxDataViewItemArray & sel ) override;
    virtual void Select( const wxDataViewItem & item ) override;
    virtual void Unselect( const wxDataViewItem & item ) override;
    virtual bool IsSelected( const wxDataViewItem & item ) const override;

    virtual void SelectAll() override;
    virtual void UnselectAll() override;

    virtual void EnsureVisible( const wxDataViewItem & item,
                                const wxDataViewColumn *column = nullptr ) override;
    virtual void HitTest( const wxPoint & point, wxDataViewItem & item,
                          wxDataViewColumn* &column ) const override;
    virtual wxRect GetItemRect( const wxDataViewItem & item,
                                const wxDataViewColumn *column = nullptr ) const override;

    virtual bool SetRowHeight( int rowHeight ) override;

    virtual void Collapse( const wxDataViewItem & item ) override;
    virtual bool IsExpanded( const wxDataViewItem & item ) const override;

    virtual void SetFocus() override;

    virtual bool SetFont(const wxFont & font) override;
    virtual bool SetForegroundColour(const wxColour& colour) override;
    virtual bool SetBackgroundColour(const wxColour& colour) override;

#if wxUSE_ACCESSIBILITY
    virtual bool Show(bool show = true) override;
    virtual void SetName(const wxString &name) override;
    virtual bool Reparent(wxWindowBase *newParent) override;
#endif // wxUSE_ACCESSIBILITY
    virtual bool Enable(bool enable = true) override;

    virtual bool AllowMultiColumnSort(bool allow) override;
    virtual bool IsMultiColumnSortAllowed() const override { return m_allowMultiColumnSort; }
    virtual void ToggleSortByColumn(int column) override;

#if wxUSE_DRAG_AND_DROP
    virtual bool EnableDragSource( const wxDataFormat &format ) override;
    virtual bool DoEnableDropTarget(const wxVector<wxDataFormat>& formats) override;
#endif // wxUSE_DRAG_AND_DROP

    virtual wxBorder GetDefaultBorder() const override;

    virtual void EditItem(const wxDataViewItem& item, const wxDataViewColumn *column) override;

    virtual bool SetHeaderAttr(const wxItemAttr& attr) override;

    virtual bool SetAlternateRowColour(const wxColour& colour) override;

    // This method is specific to generic wxDataViewCtrl implementation and
    // should not be used in portable code.
    wxColour GetAlternateRowColour() const { return m_alternateRowColour; }

    // The returned pointer is null if the control has wxDV_NO_HEADER style.
    //
    // This method is only available in the generic versions.
    wxHeaderCtrl* GenericGetHeader() const;

protected:
    // Finish any active in-place editor before changing the column model.
    // This is exposed to wxDataViewListCtrl so that its companion list store
    // can be kept in lock-step with the generic control.
    bool PrepareForColumnMutation();

    void EnsureVisibleRowCol( int row, int column );

    // Notice that row here may be invalid (i.e. >= GetRowCount()), this is not
    // an error and this function simply returns an invalid item in this case.
    wxDataViewItem GetItemByRow( unsigned int row ) const;
    int GetRowByItem( const wxDataViewItem & item ) const;

    // Mark the column as being used or not for sorting.
    void UseColumnForSorting(int idx);
    void DontUseColumnForSorting(int idx);

    // Return true if the given column is sorted
    bool IsColumnSorted(int idx) const;

    // Reset all columns currently used for sorting.
    void ResetAllSortColumns();

    virtual void DoEnableSystemTheme(bool enable, wxWindow* window) override;

    void OnDPIChanged(wxDPIChangedEvent& event);

public:     // utility functions not part of the API

    // returns the "best" width for the idx-th column
    unsigned int GetBestColumnWidth(int idx) const;
    bool TryGetBestColumnWidth(int idx, unsigned int* width) const;

    // called by header window after reorder
    void ColumnMoved( wxDataViewColumn* col, unsigned int new_pos );

    // update the display after a change to an individual column
    void OnColumnChange(unsigned int idx);

    // update after the column width changes due to interactive resizing
    void OnColumnResized();

    // update after the column width changes because of e.g. title or bitmap
    // change, invalidates the column best width and calls OnColumnChange()
    void OnColumnWidthChange(unsigned int idx);

    // update after a change to the number of columns
    void OnColumnsCountChanged();

    wxWindow *GetMainWindow() { return (wxWindow*) m_clientArea; }

    // return the index of the given column in m_cols
    int GetColumnIndex(const wxDataViewColumn *column) const;

    // Return the index of the column having the given model index.
    int GetModelColumnIndex(unsigned int model_column) const;

    // return the column displayed at the given position in the control
    wxDataViewColumn *GetColumnAt(unsigned int pos) const;

    virtual wxDataViewColumn *GetCurrentColumn() const override;

    virtual void OnInternalIdle() override;

#if wxUSE_ACCESSIBILITY
    virtual wxAccessible* CreateAccessible() override;
#endif // wxUSE_ACCESSIBILITY

private:
    friend class wxTreeListCtrl;

    // Implement pure virtual method inherited from wxCompositeWindow.
    virtual wxWindowList GetCompositeWindowParts() const override;

    virtual wxDataViewItem DoGetCurrentItem() const override;
    virtual void DoSetCurrentItem(const wxDataViewItem& item) override;

    virtual void DoExpand(const wxDataViewItem& item, bool expandChildren) override;

    void InvalidateColBestWidths();
    void InvalidateColBestWidth(int idx);
    void UpdateColWidths();

    // Publish the new count and, if specified, its display order before the
    // client area is notified. Kept separate from the historical exported
    // OnColumnsCountChanged() symbol to preserve binary compatibility.
    bool PublishColumnsAfterMutation(const wxArrayInt* order);
    bool DoInsertColumn(unsigned int pos, wxDataViewColumn* col);
    bool DoDeleteColumn(wxDataViewColumn* column,
                        wxDataViewColumn* promoted,
                        wxDataViewColumn* replacement);
    bool DeleteColumnForTreeList(wxDataViewColumn* column,
                                 wxDataViewColumn* promoted,
                                 wxDataViewColumn* replacement)
    {
        return DoDeleteColumn(column, promoted, replacement);
    }
    void DoClearColumns();

    wxVector<wxDataViewColumn*> m_cols;
    // cached column best widths information, values are for
    // respective columns from m_cols and the arrays have same size
    struct CachedColWidthInfo
    {
        CachedColWidthInfo() : width(0), dirty(true) {}
        int width;  // cached width or 0 if not computed
        bool dirty; // column was invalidated, header needs updating
    };
    wxVector<CachedColWidthInfo> m_colsBestWidths;
    // This indicates that at least one entry in m_colsBestWidths has 'dirty'
    // flag set. It's cheaper to check one flag in OnInternalIdle() than to
    // iterate over m_colsBestWidths to check if anything needs to be done.
    bool                      m_colsDirty;

    wxDataViewModelNotifier  *m_notifier;
    wxDataViewMainWindow     *m_clientArea;
    wxDataViewHeaderWindow   *m_headerArea;

    // user defined color to draw row lines, may be invalid
    wxColour m_alternateRowColour;

    // columns indices used for sorting, empty if nothing is sorted
    wxVector<int> m_sortingColumnIdxs;

    // if true, allow sorting by more than one column
    bool m_allowMultiColumnSort;

private:
    void OnSize( wxSizeEvent &event );
    virtual wxSize GetSizeAvailableForScrollTarget(const wxSize& size) override;

    // we need to return a special WM_GETDLGCODE value to process just the
    // arrows but let the other navigation characters through
#ifdef __WXMSW__
    virtual WXLRESULT MSWWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam) override;
#endif // __WXMSW__

    WX_FORWARD_TO_SCROLL_HELPER()

private:
    wxDECLARE_DYNAMIC_CLASS(wxDataViewCtrl);
    wxDECLARE_NO_COPY_CLASS(wxDataViewCtrl);
    wxDECLARE_EVENT_TABLE();
};

#if wxUSE_ACCESSIBILITY
//-----------------------------------------------------------------------------
// wxDataViewCtrlAccessible
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxDataViewCtrlAccessible: public wxWindowAccessible
{
public:
    wxDataViewCtrlAccessible(wxDataViewCtrl* win);
    virtual ~wxDataViewCtrlAccessible() = default;

    virtual wxAccStatus HitTest(const wxPoint& pt, int* childId,
                                wxAccessible** childObject) override;

    virtual wxAccStatus GetLocation(wxRect& rect, int elementId) override;

    virtual wxAccStatus Navigate(wxNavDir navDir, int fromId,
                                 int* toId, wxAccessible** toObject) override;

    virtual wxAccStatus GetName(int childId, wxString* name) override;

    virtual wxAccStatus GetChildCount(int* childCount) override;

    virtual wxAccStatus GetChild(int childId, wxAccessible** child) override;

    // wxWindowAccessible::GetParent() implementation is enough.
    // virtual wxAccStatus GetParent(wxAccessible** parent) override;

    virtual wxAccStatus DoDefaultAction(int childId) override;

    virtual wxAccStatus GetDefaultAction(int childId, wxString* actionName) override;

    virtual wxAccStatus GetDescription(int childId, wxString* description) override;

    virtual wxAccStatus GetHelpText(int childId, wxString* helpText) override;

    virtual wxAccStatus GetKeyboardShortcut(int childId, wxString* shortcut) override;

    virtual wxAccStatus GetRole(int childId, wxAccRole* role) override;

    virtual wxAccStatus GetState(int childId, long* state) override;

    virtual wxAccStatus GetValue(int childId, wxString* strValue) override;

    virtual wxAccStatus Select(int childId, wxAccSelectionFlags selectFlags) override;

    virtual wxAccStatus GetFocus(int* childId, wxAccessible** child) override;

    virtual wxAccStatus GetSelections(wxVariant* selections) override;
};
#endif // wxUSE_ACCESSIBILITY

#endif // __GENERICDATAVIEWCTRLH__
