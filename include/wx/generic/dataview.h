/////////////////////////////////////////////////////////////////////////////
// Name:        wx/generic/dataview.h
// Purpose:     wxDataViewCtrl generic implementation header
// Author:      Robert Roebling
// Modified By: Bo Yang
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef __GENERICDATAVIEWCTRLH__
#define __GENERICDATAVIEWCTRLH__

#include "wx/defs.h"
#include "wx/object.h"
#include "wx/list.h"
#include "wx/control.h"
#include "wx/scrolwin.h"
#include "wx/icon.h"

// ---------------------------------------------------------
// classes
// ---------------------------------------------------------

class WXDLLIMPEXP_FWD_ADV wxDataViewCtrl;
class WXDLLIMPEXP_FWD_ADV wxDataViewMainWindow;
class WXDLLIMPEXP_FWD_ADV wxDataViewHeaderWindow;

// ---------------------------------------------------------
// wxDataViewRenderer
// ---------------------------------------------------------

class WXDLLIMPEXP_ADV wxDataViewRenderer: public wxDataViewRendererBase
{
public:
    wxDataViewRenderer( const wxString &varianttype, 
                        wxDataViewCellMode mode = wxDATAVIEW_CELL_INERT,
                        int align = wxDVR_DEFAULT_ALIGNMENT );
    virtual ~wxDataViewRenderer();

    virtual bool Render( wxRect cell, wxDC *dc, int state ) = 0;
    virtual wxSize GetSize() const = 0;

    virtual void SetAlignment( int align );
    virtual int GetAlignment() const;

    virtual void SetMode( wxDataViewCellMode mode )
        { m_mode=mode; }
    virtual wxDataViewCellMode GetMode() const
        { return m_mode; }

    virtual bool Activate( wxRect WXUNUSED(cell),
                           wxDataViewModel *WXUNUSED(model),
                           const wxDataViewItem & WXUNUSED(item), 
                           unsigned int WXUNUSED(col) )
                           { return false; }

    virtual bool LeftClick( wxPoint WXUNUSED(cursor),
                            wxRect WXUNUSED(cell),
                            wxDataViewModel *WXUNUSED(model),
                            const wxDataViewItem & WXUNUSED(item), 
                            unsigned int WXUNUSED(col) )
                            { return false; }
    virtual bool RightClick( wxPoint WXUNUSED(cursor),
                             wxRect WXUNUSED(cell),
                             wxDataViewModel *WXUNUSED(model),
                             const wxDataViewItem & WXUNUSED(item), 
                             unsigned int WXUNUSED(col) )
                             { return false; }
    virtual bool StartDrag( wxPoint WXUNUSED(cursor),
                            wxRect WXUNUSED(cell),
                            wxDataViewModel *WXUNUSED(model),
                            const wxDataViewItem & WXUNUSED(item), 
                            unsigned int WXUNUSED(col) )
                            { return false; }

    // Create DC on request
    virtual wxDC *GetDC();
    
    void SetHasAttr( bool set )  { m_hasAttr = set; }
    void SetAttr( const wxDataViewItemAttr &attr ) { m_attr = attr; }
    bool GetWantsAttr() { return m_wantsAttr; }

    // implementation
    int CalculateAlignment() const;

private:
    wxDC                        *m_dc;
    int                          m_align;
    wxDataViewCellMode           m_mode;
    
protected:
    bool                         m_wantsAttr;
    bool                         m_hasAttr;
    wxDataViewItemAttr           m_attr;

protected:
    DECLARE_DYNAMIC_CLASS_NO_COPY(wxDataViewRenderer)
};

// ---------------------------------------------------------
// wxDataViewCustomRenderer
// ---------------------------------------------------------

class WXDLLIMPEXP_ADV wxDataViewCustomRenderer: public wxDataViewRenderer
{
public:
    wxDataViewCustomRenderer( const wxString &varianttype = wxT("string"),
                              wxDataViewCellMode mode = wxDATAVIEW_CELL_INERT,
                              int align = wxDVR_DEFAULT_ALIGNMENT );

    void RenderText( const wxString &text, int xoffset, wxRect cell, wxDC *dc, int state );

protected:
    DECLARE_DYNAMIC_CLASS_NO_COPY(wxDataViewCustomRenderer)
};


// ---------------------------------------------------------
// wxDataViewTextRenderer
// ---------------------------------------------------------

class WXDLLIMPEXP_ADV wxDataViewTextRenderer: public wxDataViewCustomRenderer
{
public:
    wxDataViewTextRenderer( const wxString &varianttype = wxT("string"),
                            wxDataViewCellMode mode = wxDATAVIEW_CELL_INERT,
                            int align = wxDVR_DEFAULT_ALIGNMENT );

    bool SetValue( const wxVariant &value );
    bool GetValue( wxVariant &value ) const;

    bool Render( wxRect cell, wxDC *dc, int state );
    wxSize GetSize() const;

    // in-place editing
    virtual bool HasEditorCtrl();
    virtual wxControl* CreateEditorCtrl( wxWindow *parent, wxRect labelRect, const wxVariant &value );
    virtual bool GetValueFromEditorCtrl( wxControl* editor, wxVariant &value );
        
protected:
    wxString   m_text;

protected:
    DECLARE_DYNAMIC_CLASS_NO_COPY(wxDataViewTextRenderer)
};

// ---------------------------------------------------------
// wxDataViewTextRendererAttr
// ---------------------------------------------------------

class WXDLLIMPEXP_ADV wxDataViewTextRendererAttr: public wxDataViewTextRenderer
{
public:
    wxDataViewTextRendererAttr( const wxString &varianttype = wxT("string"),
                            wxDataViewCellMode mode = wxDATAVIEW_CELL_INERT,
                            int align = wxDVR_DEFAULT_ALIGNMENT );

    bool Render( wxRect cell, wxDC *dc, int state );
        
protected:
    DECLARE_DYNAMIC_CLASS_NO_COPY(wxDataViewTextRendererAttr)
};

// ---------------------------------------------------------
// wxDataViewBitmapRenderer
// ---------------------------------------------------------

class WXDLLIMPEXP_ADV wxDataViewBitmapRenderer: public wxDataViewCustomRenderer
{
public:
    wxDataViewBitmapRenderer( const wxString &varianttype = wxT("wxBitmap"),
                              wxDataViewCellMode mode = wxDATAVIEW_CELL_INERT,
                              int align = wxDVR_DEFAULT_ALIGNMENT );

    bool SetValue( const wxVariant &value );
    bool GetValue( wxVariant &value ) const;

    bool Render( wxRect cell, wxDC *dc, int state );
    wxSize GetSize() const;

private:
    wxIcon m_icon;
    wxBitmap m_bitmap;

protected:
    DECLARE_DYNAMIC_CLASS_NO_COPY(wxDataViewBitmapRenderer)
};

// ---------------------------------------------------------
// wxDataViewToggleRenderer
// ---------------------------------------------------------

class WXDLLIMPEXP_ADV wxDataViewToggleRenderer: public wxDataViewCustomRenderer
{
public:
    wxDataViewToggleRenderer( const wxString &varianttype = wxT("bool"),
                              wxDataViewCellMode mode = wxDATAVIEW_CELL_INERT,
                              int align = wxDVR_DEFAULT_ALIGNMENT );

    bool SetValue( const wxVariant &value );
    bool GetValue( wxVariant &value ) const;

    bool Render( wxRect cell, wxDC *dc, int state );
    bool Activate( wxRect cell, wxDataViewModel *model, const wxDataViewItem & item, 
                            unsigned int col );
    wxSize GetSize() const;

private:
    bool    m_toggle;

protected:
    DECLARE_DYNAMIC_CLASS_NO_COPY(wxDataViewToggleRenderer)
};

// ---------------------------------------------------------
// wxDataViewProgressRenderer
// ---------------------------------------------------------

class WXDLLIMPEXP_ADV wxDataViewProgressRenderer: public wxDataViewCustomRenderer
{
public:
    wxDataViewProgressRenderer( const wxString &label = wxEmptyString,
                                const wxString &varianttype = wxT("long"),
                                wxDataViewCellMode mode = wxDATAVIEW_CELL_INERT,
                                int align = wxDVR_DEFAULT_ALIGNMENT );
    virtual ~wxDataViewProgressRenderer();

    bool SetValue( const wxVariant &value );
    bool GetValue( wxVariant& value ) const;

    virtual bool Render( wxRect cell, wxDC *dc, int state );
    virtual wxSize GetSize() const;

private:
    wxString    m_label;
    int         m_value;

protected:
    DECLARE_DYNAMIC_CLASS_NO_COPY(wxDataViewProgressRenderer)
};
    
// --------------------------------------------------------- 
// wxDataViewIconTextRenderer
// --------------------------------------------------------- 

class WXDLLIMPEXP_ADV wxDataViewIconTextRenderer: public wxDataViewCustomRenderer
{
public:
    wxDataViewIconTextRenderer( const wxString &varianttype = wxT("wxDataViewIconText"), 
                                wxDataViewCellMode mode = wxDATAVIEW_CELL_INERT,
                                int align = wxDVR_DEFAULT_ALIGNMENT );
    virtual ~wxDataViewIconTextRenderer();
    
    bool SetValue( const wxVariant &value );
    bool GetValue( wxVariant &value ) const;
    
    virtual bool Render( wxRect cell, wxDC *dc, int state );
    virtual wxSize GetSize() const;
    
    virtual bool HasEditorCtrl() { return true; }
    virtual wxControl* CreateEditorCtrl( wxWindow *parent, wxRect labelRect, const wxVariant &value );
    virtual bool GetValueFromEditorCtrl( wxControl* editor, wxVariant &value );
    
private:
    wxDataViewIconText   m_value;
    
protected:
    DECLARE_DYNAMIC_CLASS_NO_COPY(wxDataViewIconTextRenderer)
};

// ---------------------------------------------------------
// wxDataViewDateRenderer
// ---------------------------------------------------------

class WXDLLIMPEXP_ADV wxDataViewDateRenderer: public wxDataViewCustomRenderer
{
public:
    wxDataViewDateRenderer( const wxString &varianttype = wxT("datetime"),
                            wxDataViewCellMode mode = wxDATAVIEW_CELL_ACTIVATABLE,
                            int align = wxDVR_DEFAULT_ALIGNMENT );

    bool SetValue( const wxVariant &value );
    bool GetValue( wxVariant& value ) const;

    virtual bool Render( wxRect cell, wxDC *dc, int state );
    virtual wxSize GetSize() const;
    virtual bool Activate( wxRect cell,
                           wxDataViewModel *model, const wxDataViewItem & item, unsigned int col );

private:
    wxDateTime    m_date;

protected:
    DECLARE_DYNAMIC_CLASS_NO_COPY(wxDataViewDateRenderer)
};

// ---------------------------------------------------------
// wxDataViewColumn
// ---------------------------------------------------------

class WXDLLIMPEXP_ADV wxDataViewColumn: public wxDataViewColumnBase
{
    friend class wxDataViewHeaderWindowBase;
    friend class wxDataViewHeaderWindow;
    friend class wxDataViewHeaderWindowMSW;

public:
    wxDataViewColumn( const wxString &title, wxDataViewRenderer *renderer, 
                      unsigned int model_column, int width = wxDVC_DEFAULT_WIDTH, 
                      wxAlignment align = wxALIGN_CENTER,
                      int flags = wxDATAVIEW_COL_RESIZABLE );
    wxDataViewColumn( const wxBitmap &bitmap, wxDataViewRenderer *renderer, 
                      unsigned int model_column, int width = wxDVC_DEFAULT_WIDTH, 
                      wxAlignment align = wxALIGN_CENTER,
                      int flags = wxDATAVIEW_COL_RESIZABLE );
    virtual ~wxDataViewColumn();

    // setters:

    virtual void SetTitle( const wxString &title )
        { m_title=title; }
    virtual void SetAlignment( wxAlignment align )
        { m_align=align; }
    virtual void SetMinWidth( int minWidth )
        { m_minWidth=minWidth; }
    virtual void SetWidth( int width );
    virtual void SetSortable( bool sortable );
    virtual void SetResizeable( bool resizeable );
    virtual void SetHidden( bool hidden );
    virtual void SetSortOrder( bool ascending );
    virtual void SetReorderable( bool reorderable );

    // getters:

    virtual wxString GetTitle() const
        { return m_title; }
    virtual wxAlignment GetAlignment() const
        { return m_align; }
    virtual int GetWidth() const
        { return m_width; }
    virtual int GetMinWidth() const
        { return m_minWidth; }
    virtual bool IsSortable() const
        { return (m_flags & wxDATAVIEW_COL_SORTABLE) != 0; }
    virtual bool IsResizeable() const
        { return (m_flags & wxDATAVIEW_COL_RESIZABLE) != 0; }
    virtual bool IsHidden() const
        { return (m_flags & wxDATAVIEW_COL_HIDDEN) != 0; }
    virtual bool IsSortOrderAscending() const;
    virtual bool IsReorderable() const
        { return (m_flags & wxDATAVIEW_COL_REORDERABLE) != 0; }

private:
    int                      m_width;
    int                      m_minWidth;
    int                      m_flags;
    wxAlignment              m_align;
    wxString                 m_title;
    bool                     m_ascending;
    bool                     m_autosize;

    void Init(int width);

    // like SetWidth() but does not ask the header window of the
    // wxDataViewCtrl to reflect the width-change.
    void SetInternalWidth(int width);

protected:
    DECLARE_DYNAMIC_CLASS_NO_COPY(wxDataViewColumn)
};

// ---------------------------------------------------------
// wxDataViewCtrl
// ---------------------------------------------------------

WX_DECLARE_LIST_WITH_DECL(wxDataViewColumn, wxDataViewColumnList,
                          class WXDLLIMPEXP_ADV);

class WXDLLIMPEXP_ADV wxDataViewCtrl: public wxDataViewCtrlBase,
                                       public wxScrollHelperNative
{
    friend class wxDataViewMainWindow;
    friend class wxDataViewHeaderWindowBase;
    friend class wxDataViewHeaderWindow;
    friend class wxDataViewHeaderWindowMSW;
    friend class wxDataViewColumn;

public:
    wxDataViewCtrl() : wxScrollHelperNative(this)
    {
        Init();
    }

    wxDataViewCtrl( wxWindow *parent, wxWindowID id,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize, long style = 0,
           const wxValidator& validator = wxDefaultValidator )
             : wxScrollHelperNative(this)
    {
        Create(parent, id, pos, size, style, validator );
    }

    virtual ~wxDataViewCtrl();

    void Init();

    bool Create(wxWindow *parent, wxWindowID id,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize, long style = 0,
           const wxValidator& validator = wxDefaultValidator );

    virtual bool AssociateModel( wxDataViewModel *model );
    
    virtual bool AppendColumn( wxDataViewColumn *col );
    virtual bool PrependColumn( wxDataViewColumn *col );
    virtual bool InsertColumn( unsigned int pos, wxDataViewColumn *col );

    virtual void DoSetExpanderColumn();
    virtual void DoSetIndent();

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

    virtual void EnsureVisible( const wxDataViewItem & item,
                                const wxDataViewColumn *column = NULL );
    virtual void HitTest( const wxPoint & point, wxDataViewItem & item, wxDataViewColumn* &column ) const;
    virtual wxRect GetItemRect( const wxDataViewItem & item, const wxDataViewColumn *column = NULL ) const;

    virtual void Expand( const wxDataViewItem & item );
    virtual void Collapse( const wxDataViewItem & item );
    
    virtual void SetFocus();

protected:
    virtual int GetSelections( wxArrayInt & sel ) const; 
    virtual void SetSelections( const wxArrayInt & sel );
    virtual void Select( int row );
    virtual void Unselect( int row );
    virtual bool IsSelected( int row ) const;
    virtual void SelectRange( int from, int to );
    virtual void UnselectRange( int from, int to );

    virtual void EnsureVisible( int row, int column );

    virtual wxDataViewItem GetItemByRow( unsigned int row ) const;
    virtual int GetRowByItem( const wxDataViewItem & item ) const;

    wxDataViewColumn* GetSortingColumn() { return m_sortingColumn; }
    void SetSortingColumn( wxDataViewColumn* column ) { m_sortingColumn = column; }

public:     // utility functions not part of the API

    // returns the "best" width for the idx-th column
    unsigned int GetBestColumnWidth(int WXUNUSED(idx)) const
    {
        return GetClientSize().GetWidth() / GetColumnCount();
    }

    // called by header window after reorder
    void ColumnMoved( wxDataViewColumn* col, unsigned int new_pos );

    // updates the header window after a change in a column setting
    void OnColumnChange();

    wxWindow *GetMainWindow() { return (wxWindow*) m_clientArea; }

private:
    wxDataViewColumnList      m_cols;
    wxDataViewModelNotifier  *m_notifier;
    wxDataViewMainWindow     *m_clientArea;
    wxDataViewHeaderWindow   *m_headerArea;
    wxDataViewColumn         *m_sortingColumn;

private:
    void OnSize( wxSizeEvent &event );
    virtual wxSize GetSizeAvailableForScrollTarget(const wxSize& size);

    // we need to return a special WM_GETDLGCODE value to process just the
    // arrows but let the other navigation characters through
#ifdef __WXMSW__
    virtual WXLRESULT MSWWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam);
#endif // __WXMSW__

    WX_FORWARD_TO_SCROLL_HELPER()

private:
    DECLARE_DYNAMIC_CLASS(wxDataViewCtrl)
    DECLARE_NO_COPY_CLASS(wxDataViewCtrl)
    DECLARE_EVENT_TABLE()
};


#endif // __GENERICDATAVIEWCTRLH__
