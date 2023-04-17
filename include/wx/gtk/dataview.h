/////////////////////////////////////////////////////////////////////////////
// Name:        wx/gtk/dataview.h
// Purpose:     wxDataViewCtrl GTK+2 implementation header
// Author:      Robert Roebling
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GTKDATAVIEWCTRL_H_
#define _WX_GTKDATAVIEWCTRL_H_

#include <memory>
#include <vector>

class WXDLLIMPEXP_FWD_CORE wxDataViewCtrlInternal;

struct _GtkTreePath;

// ---------------------------------------------------------
// wxDataViewColumn
// ---------------------------------------------------------

class WXDLLIMPEXP_CORE wxDataViewColumn: public wxDataViewColumnBase
{
public:
    wxDataViewColumn( const wxString &title, wxDataViewRenderer *renderer,
                      unsigned int model_column, int width = wxDVC_DEFAULT_WIDTH,
                      wxAlignment align = wxALIGN_CENTER,
                      int flags = wxDATAVIEW_COL_RESIZABLE );
    wxDataViewColumn( const wxBitmapBundle &bitmap, wxDataViewRenderer *renderer,
                      unsigned int model_column, int width = wxDVC_DEFAULT_WIDTH,
                      wxAlignment align = wxALIGN_CENTER,
                      int flags = wxDATAVIEW_COL_RESIZABLE );


    // setters:

    virtual void SetTitle( const wxString &title ) override;
    virtual void SetBitmap( const wxBitmapBundle &bitmap ) override;

    virtual void SetOwner( wxDataViewCtrl *owner ) override;

    virtual void SetAlignment( wxAlignment align ) override;

    virtual void SetSortable( bool sortable ) override;
    virtual void SetSortOrder( bool ascending ) override;
    virtual void UnsetAsSortKey() override;

    virtual void SetResizeable( bool resizable ) override;
    virtual void SetHidden( bool hidden ) override;

    virtual void SetMinWidth( int minWidth ) override;
    virtual void SetWidth( int width ) override;

    virtual void SetReorderable( bool reorderable ) override;

    virtual void SetFlags(int flags) override { SetIndividualFlags(flags); }

    // getters:

    virtual wxString GetTitle() const override;
    virtual wxAlignment GetAlignment() const override;

    virtual bool IsSortable() const override;
    virtual bool IsSortOrderAscending() const override;
    virtual bool IsSortKey() const override;

    virtual bool IsResizeable() const override;
    virtual bool IsHidden() const override;

    virtual int GetWidth() const override;
    virtual int GetMinWidth() const override;

    virtual bool IsReorderable() const override;

    virtual int GetFlags() const override { return GetFromIndividualFlags(); }

    // implementation
    GtkWidget* GetGtkHandle() const { return m_column; }

private:
    // holds the GTK handle
    GtkWidget   *m_column;

    // holds GTK handles for title/bitmap in the header
    GtkWidget   *m_image;
    GtkWidget   *m_label;

    // delayed connection to mouse events
    friend class wxDataViewCtrl;
    void OnInternalIdle();
    bool    m_isConnected;

    void Init(wxAlignment align, int flags, int width);
};

// ---------------------------------------------------------
// wxDataViewCtrl
// ---------------------------------------------------------

class WXDLLIMPEXP_CORE wxDataViewCtrl: public wxDataViewCtrlBase
{
public:
    wxDataViewCtrl()
    {
        Init();
    }

    wxDataViewCtrl( wxWindow *parent, wxWindowID id,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize, long style = 0,
           const wxValidator& validator = wxDefaultValidator,
           const wxString& name = wxASCII_STR(wxDataViewCtrlNameStr) )
    {
        Init();

        Create(parent, id, pos, size, style, validator, name);
    }

    bool Create(wxWindow *parent, wxWindowID id,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize, long style = 0,
           const wxValidator& validator = wxDefaultValidator,
           const wxString& name = wxASCII_STR(wxDataViewCtrlNameStr));

    virtual ~wxDataViewCtrl();

    virtual bool AssociateModel( wxDataViewModel *model ) override;

    virtual bool PrependColumn( wxDataViewColumn *col ) override;
    virtual bool AppendColumn( wxDataViewColumn *col ) override;
    virtual bool InsertColumn( unsigned int pos, wxDataViewColumn *col ) override;

    virtual unsigned int GetColumnCount() const override;
    virtual wxDataViewColumn* GetColumn( unsigned int pos ) const override;
    virtual bool DeleteColumn( wxDataViewColumn *column ) override;
    virtual bool ClearColumns() override;
    virtual int GetColumnPosition( const wxDataViewColumn *column ) const override;

    virtual wxDataViewColumn *GetSortingColumn() const override;

    virtual int GetSelectedItemsCount() const override;
    virtual int GetSelections( wxDataViewItemArray & sel ) const override;
    virtual void SetSelections( const wxDataViewItemArray & sel ) override;
    virtual void Select( const wxDataViewItem & item ) override;
    virtual void Unselect( const wxDataViewItem & item ) override;
    virtual bool IsSelected( const wxDataViewItem & item ) const override;
    virtual void SelectAll() override;
    virtual void UnselectAll() override;

    virtual void EnsureVisible( const wxDataViewItem& item,
                                const wxDataViewColumn *column = nullptr ) override;
    virtual void HitTest( const wxPoint &point,
                          wxDataViewItem &item,
                          wxDataViewColumn *&column ) const override;
    virtual wxRect GetItemRect( const wxDataViewItem &item,
                                const wxDataViewColumn *column = nullptr ) const override;

    virtual bool SetRowHeight( int rowHeight ) override;

    virtual void EditItem(const wxDataViewItem& item, const wxDataViewColumn *column) override;

    virtual void Collapse( const wxDataViewItem & item ) override;
    virtual bool IsExpanded( const wxDataViewItem & item ) const override;

    virtual bool EnableDragSource( const wxDataFormat &format ) override;
    virtual bool DoEnableDropTarget( const wxVector<wxDataFormat>& formats ) override;

    virtual wxDataViewColumn *GetCurrentColumn() const override;

    virtual wxDataViewItem GetTopItem() const override;
    virtual int GetCountPerPage() const override;

    static wxVisualAttributes
    GetClassDefaultAttributes(wxWindowVariant variant = wxWINDOW_VARIANT_NORMAL);

    wxWindow *GetMainWindow() { return (wxWindow*) this; }

    GtkWidget *GtkGetTreeView() { return m_treeview; }
    wxDataViewCtrlInternal* GtkGetInternal() { return m_internal; }

    // Convert GTK path to our item. Returned item may be invalid if get_iter()
    // failed.
    wxDataViewItem GTKPathToItem(struct _GtkTreePath *path) const;

    // Return wxDataViewColumn matching the given GtkTreeViewColumn.
    //
    // If the input argument is null, return nullptr too. Otherwise we must find
    // the matching column and assert if we didn't.
    wxDataViewColumn* GTKColumnToWX(GtkTreeViewColumn *gtk_col) const;

    virtual void OnInternalIdle() override;

    int GTKGetUniformRowHeight() const { return m_uniformRowHeight; }

    // Simple RAII helper for disabling selection events during its lifetime.
    class SelectionEventsSuppressor
    {
    public:
        explicit SelectionEventsSuppressor(wxDataViewCtrl* ctrl)
            : m_ctrl(ctrl)
        {
            m_ctrl->GtkDisableSelectionEvents();
        }

        ~SelectionEventsSuppressor()
        {
            m_ctrl->GtkEnableSelectionEvents();
        }

    private:
        wxDataViewCtrl* const m_ctrl;
    };

protected:
    virtual void DoSetExpanderColumn() override;
    virtual void DoSetIndent() override;

    virtual void DoExpand(const wxDataViewItem& item, bool expandChildren) override;

    virtual void DoApplyWidgetStyle(GtkRcStyle *style) override;
    virtual GdkWindow* GTKGetWindow(wxArrayGdkWindows& windows) const override;

private:
    void Init();

    virtual wxDataViewItem DoGetCurrentItem() const override;
    virtual void DoSetCurrentItem(const wxDataViewItem& item) override;

    friend class wxDataViewCtrlDCImpl;
    friend class wxDataViewColumn;
    friend class wxDataViewCtrlInternal;

    GtkWidget               *m_treeview;
    wxDataViewCtrlInternal  *m_internal;

    using wxDataViewColumnPtr = std::unique_ptr<wxDataViewColumn>;
    std::vector<wxDataViewColumnPtr> m_cols;

    wxDataViewItem           m_ensureVisibleDefered;

    // By default this is set to -1 and the height of the rows is determined by
    // GetRect() methods of the renderers but this can be set to a positive
    // value to force the height of all rows to the given value.
    int m_uniformRowHeight;

    virtual void AddChildGTK(wxWindowGTK* child) override;
    void GtkEnableSelectionEvents();
    void GtkDisableSelectionEvents();

    wxDECLARE_DYNAMIC_CLASS(wxDataViewCtrl);
    wxDECLARE_NO_COPY_CLASS(wxDataViewCtrl);
};


#endif // _WX_GTKDATAVIEWCTRL_H_
