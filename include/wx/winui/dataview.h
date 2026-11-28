/////////////////////////////////////////////////////////////////////////////
// Name:        wx/winui/dataview.h
// Purpose:     Native WinUI data view, retaining the wx model/renderer API
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WINUI_DATAVIEW_H_
#define _WX_WINUI_DATAVIEW_H_

#include <memory>

class wxWinUIDataViewImpl;
class wxTreeListCtrl;

class WXDLLIMPEXP_CORE wxDataViewColumn : public wxDataViewColumnBase
{
public:
    wxDataViewColumn(const wxString& title, wxDataViewRenderer* renderer,
                     unsigned int modelColumn, int width = wxDVC_DEFAULT_WIDTH,
                     wxAlignment align = wxALIGN_CENTER,
                     int flags = wxDATAVIEW_COL_RESIZABLE);
    wxDataViewColumn(const wxBitmapBundle& bitmap, wxDataViewRenderer* renderer,
                     unsigned int modelColumn, int width = wxDVC_DEFAULT_WIDTH,
                     wxAlignment align = wxALIGN_CENTER,
                     int flags = wxDATAVIEW_COL_RESIZABLE);

    void SetTitle(const wxString& title) override;
    wxString GetTitle() const override { return m_title; }
    void SetWidth(int width) override;
    int GetWidth() const override;
    int WXGetSpecifiedWidth() const override { return m_width; }
    void SetMinWidth(int width) override;
    int GetMinWidth() const override { return m_minWidth; }
    void SetAlignment(wxAlignment align) override;
    wxAlignment GetAlignment() const override { return m_align; }
    void SetFlags(int flags) override;
    int GetFlags() const override { return m_flags; }
    bool IsSortKey() const override { return m_sort; }
    void UnsetAsSortKey() override;
    void SetSortOrder(bool ascending) override;
    bool IsSortOrderAscending() const override { return m_ascending; }
    void SetBitmap(const wxBitmapBundle& bitmap) override;
    void WXUpdateWidth(int width) { SetWidth(width); }
    void WXOnResize(int width) { SetWidth(width); }
    void WXCopyStateFrom(const wxDataViewColumn& other);

private:
    void Changed();
    wxString m_title;
    int m_width;
    int m_minWidth = wxDVC_DEFAULT_MINWIDTH;
    wxAlignment m_align;
    int m_flags;
    bool m_sort = false;
    bool m_ascending = true;
    friend class wxDataViewCtrl;
};

// The renderer ABI remains compatible with wxDataViewCustomRenderer. It does
// not select the generic view: rows, headers, selection and editors are XAML.
class WXDLLIMPEXP_CORE wxDataViewCtrl : public wxDataViewCtrlBase
{
public:
    wxDataViewCtrl();
    wxDataViewCtrl(wxWindow* parent, wxWindowID id,
                   const wxPoint& pos = wxDefaultPosition,
                   const wxSize& size = wxDefaultSize, long style = 0,
                   const wxValidator& validator = wxDefaultValidator,
                   const wxString& name = wxASCII_STR(wxDataViewCtrlNameStr));
    ~wxDataViewCtrl() override;
    bool Create(wxWindow* parent, wxWindowID id,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize, long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxASCII_STR(wxDataViewCtrlNameStr));
    bool AssociateModel(wxDataViewModel* model) override;
    bool AppendColumn(wxDataViewColumn* col) override;
    bool PrependColumn(wxDataViewColumn* col) override;
    bool InsertColumn(unsigned int pos, wxDataViewColumn* col) override;
    unsigned int GetColumnCount() const override;
    wxDataViewColumn* GetColumn(unsigned int pos) const override;
    int GetColumnPosition(const wxDataViewColumn* col) const override;
    bool DeleteColumn(wxDataViewColumn* col) override;
    bool ClearColumns() override;
    wxDataViewColumn* GetSortingColumn() const override;
    wxVector<wxDataViewColumn*> GetSortingColumns() const override;
    bool AllowMultiColumnSort(bool allow) override;
    bool IsMultiColumnSortAllowed() const override;
    void ToggleSortByColumn(int column) override;

    wxDataViewColumn* GetCurrentColumn() const override;
    wxDataViewItem GetTopItem() const override;
    int GetCountPerPage() const override;
    int GetSelectedItemsCount() const override;
    int GetSelections(wxDataViewItemArray& selections) const override;
    void SetSelections(const wxDataViewItemArray& selections) override;
    void Select(const wxDataViewItem& item) override;
    void Unselect(const wxDataViewItem& item) override;
    bool IsSelected(const wxDataViewItem& item) const override;
    void SelectAll() override;
    void UnselectAll() override;
    void Collapse(const wxDataViewItem& item) override;
    bool IsExpanded(const wxDataViewItem& item) const override;
    void EnsureVisible(const wxDataViewItem& item,
                       const wxDataViewColumn* col = nullptr) override;
    void HitTest(const wxPoint& point, wxDataViewItem& item,
                 wxDataViewColumn*& col) const override;
    wxRect GetItemRect(const wxDataViewItem& item,
                       const wxDataViewColumn* col = nullptr) const override;
    bool SetRowHeight(int height) override;
    void EditItem(const wxDataViewItem& item,
                   const wxDataViewColumn* col) override;
    bool SetFont(const wxFont& font) override;
    bool SetForegroundColour(const wxColour& colour) override;
    bool SetBackgroundColour(const wxColour& colour) override;
    void SetFocus() override;
    wxBorder GetDefaultBorder() const override { return wxBORDER_NONE; }

    // Compatibility hooks used by the shared custom editor and TreeList
    // transactions. No generic header or child drawing window exists.
    wxWindow* GetMainWindow() { return this; }
    wxHeaderCtrl* GenericGetHeader() const { return nullptr; }
    int GetColumnIndex(const wxDataViewColumn* col) const;
    int GetModelColumnIndex(unsigned int modelColumn) const;
    wxDataViewColumn* GetColumnAt(unsigned int pos) const;
    unsigned int GetBestColumnWidth(int index) const;
    bool TryGetBestColumnWidth(int index, unsigned int* width) const;
    void OnColumnChange(unsigned int index);
    void OnColumnWidthChange(unsigned int index);
    void OnColumnResized();
    void OnColumnsCountChanged();
    void ColumnMoved(wxDataViewColumn* col, unsigned int pos);

protected:
    bool PrepareForColumnMutation();
    void DoSetExpanderColumn() override;
    void DoSetIndent() override;
    wxDataViewItem DoGetCurrentItem() const override;
    void DoSetCurrentItem(const wxDataViewItem& item) override;
    void DoExpand(const wxDataViewItem& item, bool children) override;
    wxSize DoGetBestSize() const override;

private:
    bool FinishNativeEditing(wxDataViewRendererBase* renderer,
                             bool cancel, bool* handled);
    bool DeleteColumnForTreeList(wxDataViewColumn* col,
                                 wxDataViewColumn* promoted,
                                 wxDataViewColumn* replacement);
    std::shared_ptr<wxWinUIDataViewImpl> m_winui;
    friend class wxDataViewColumn;
    friend class wxTreeListCtrl;
    friend class wxWinUIDataViewImpl;
    friend class wxDataViewRendererBase;
    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxDataViewCtrl);
};

#endif // _WX_WINUI_DATAVIEW_H_
