/////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/dataview.h
// Author:      Peter Most
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_QT_DATAVIEW_H_
#define _WX_QT_DATAVIEW_H_

class WXDLLIMPEXP_ADV wxDataViewColumn: public wxDataViewColumnBase
{
public:
    wxDataViewColumn( const wxString &title, wxDataViewRenderer *renderer,
                      unsigned int model_column, int width = wxDVC_DEFAULT_WIDTH,
                      wxAlignment align = wxALIGN_CENTER,
                      int flags = wxDATAVIEW_COL_RESIZABLE );
    wxDataViewColumn( const wxBitmap &bitmap, wxDataViewRenderer *renderer,
                      unsigned int model_column, int width = wxDVC_DEFAULT_WIDTH,
                      wxAlignment align = wxALIGN_CENTER,
                      int flags = wxDATAVIEW_COL_RESIZABLE );


    // setters:

    virtual void SetTitle( const wxString &title );
    virtual void SetBitmap( const wxBitmap &bitmap );

    virtual void SetOwner( wxDataViewCtrl *owner );

    virtual void SetAlignment( wxAlignment align );

    virtual void SetSortable( bool sortable );
    virtual void SetSortOrder( bool ascending );
    virtual void SetAsSortKey(bool sort = true);

    virtual void SetResizeable( bool resizeable );
    virtual void SetHidden( bool hidden );

    virtual void SetMinWidth( int minWidth );
    virtual void SetWidth( int width );

    virtual void SetReorderable( bool reorderable );

    virtual void SetFlags(int flags);

    // getters:

    virtual wxString GetTitle() const;
    virtual wxAlignment GetAlignment() const;

    virtual bool IsSortable() const;
    virtual bool IsSortOrderAscending() const;
    virtual bool IsSortKey() const;

    virtual bool IsResizeable() const;
    virtual bool IsHidden() const;

    virtual int GetWidth() const;
    virtual int GetMinWidth() const;

    virtual bool IsReorderable() const;

    virtual int GetFlags() const;

};


class WXDLLIMPEXP_ADV wxDataViewCtrl: public wxDataViewCtrlBase
{
public:
    wxDataViewCtrl();

    wxDataViewCtrl( wxWindow *parent, wxWindowID id,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize, long style = 0,
           const wxValidator& validator = wxDefaultValidator );

    virtual ~wxDataViewCtrl();


    bool Create(wxWindow *parent, wxWindowID id,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize, long style = 0,
           const wxValidator& validator = wxDefaultValidator );

    virtual bool AssociateModel( wxDataViewModel *model );

    virtual bool PrependColumn( wxDataViewColumn *col );
    virtual bool AppendColumn( wxDataViewColumn *col );
    virtual bool InsertColumn( unsigned int pos, wxDataViewColumn *col );

    virtual unsigned int GetColumnCount() const;
    virtual wxDataViewColumn* GetColumn( unsigned int pos ) const;
    virtual bool DeleteColumn( wxDataViewColumn *column );
    virtual bool ClearColumns();
    virtual int GetColumnPosition( const wxDataViewColumn *column ) const;

    virtual wxDataViewColumn *GetSortingColumn() const;

    virtual wxDataViewItem GetSelection() const;
    virtual int GetSelections( wxDataViewItemArray & sel ) const;
    virtual void SetSelections( const wxDataViewItemArray & sel );
    virtual void Select( const wxDataViewItem & item );
    virtual void Unselect( const wxDataViewItem & item );
    virtual bool IsSelected( const wxDataViewItem & item ) const;
    virtual void SelectAll();
    virtual void UnselectAll();

    virtual void EnsureVisible( const wxDataViewItem& item,
                                const wxDataViewColumn *column = NULL );
    virtual void HitTest( const wxPoint &point,
                          wxDataViewItem &item,
                          wxDataViewColumn *&column ) const;
    virtual wxRect GetItemRect( const wxDataViewItem &item,
                                const wxDataViewColumn *column = NULL ) const;

    virtual void Collapse( const wxDataViewItem & item );
    virtual bool IsExpanded( const wxDataViewItem & item ) const;

    virtual bool EnableDragSource( const wxDataFormat &format );
    virtual bool EnableDropTarget( const wxDataFormat &format );

    static wxVisualAttributes
    GetClassDefaultAttributes(wxWindowVariant variant = wxWINDOW_VARIANT_NORMAL);

    wxWindow *GetMainWindow() { return (wxWindow*) this; }

    virtual void OnInternalIdle();

protected:
    virtual void DoSetExpanderColumn();
    virtual void DoSetIndent();
    virtual void DoExpand( const wxDataViewItem & item, bool expandChildren );

private:
    virtual wxDataViewItem DoGetCurrentItem() const;
    virtual void DoSetCurrentItem(const wxDataViewItem& item);
};

#endif // _WX_QT_DATAVIEW_H_
