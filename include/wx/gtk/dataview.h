/////////////////////////////////////////////////////////////////////////////
// Name:        wx/gtk/dataview.h
// Purpose:     wxDataViewCtrl GTK+2 implementation header
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GTKDATAVIEWCTRL_H_
#define _WX_GTKDATAVIEWCTRL_H_

#include "wx/list.h"

// ---------------------------------------------------------
// classes
// ---------------------------------------------------------

class WXDLLIMPEXP_FWD_ADV wxDataViewCtrl;
class WXDLLIMPEXP_FWD_ADV wxDataViewCtrlInternal;


// ---------------------------------------------------------
// wxDataViewRenderer
// ---------------------------------------------------------

class WXDLLIMPEXP_ADV wxDataViewRenderer: public wxDataViewRendererBase
{
public:
    wxDataViewRenderer( const wxString &varianttype,
                        wxDataViewCellMode mode = wxDATAVIEW_CELL_INERT,
                        int align = wxDVR_DEFAULT_ALIGNMENT );

    virtual void SetMode( wxDataViewCellMode mode );
    virtual wxDataViewCellMode GetMode() const;

    virtual void SetAlignment( int align );
    virtual int GetAlignment() const;

    // implementation
    GtkCellRenderer* GetGtkHandle() { return m_renderer; }
    void GtkInitHandlers();
    virtual bool GtkHasAttributes() { return false; }
    void GtkUpdateAlignment();

protected:
    GtkCellRenderer   *m_renderer;
    int                m_alignment;

protected:
    DECLARE_DYNAMIC_CLASS_NO_COPY(wxDataViewRenderer)
};

// ---------------------------------------------------------
// wxDataViewTextRenderer
// ---------------------------------------------------------

class WXDLLIMPEXP_ADV wxDataViewTextRenderer: public wxDataViewRenderer
{
public:
    wxDataViewTextRenderer( const wxString &varianttype = wxT("string"),
                            wxDataViewCellMode mode = wxDATAVIEW_CELL_INERT,
                            int align = wxDVR_DEFAULT_ALIGNMENT );

    bool SetValue( const wxVariant &value );
    bool GetValue( wxVariant &value ) const;

    void SetAlignment( int align );

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

    // implementation
    bool GtkHasAttributes() { return true; }

protected:
    DECLARE_DYNAMIC_CLASS_NO_COPY(wxDataViewTextRendererAttr)
};

// ---------------------------------------------------------
// wxDataViewBitmapRenderer
// ---------------------------------------------------------

class WXDLLIMPEXP_ADV wxDataViewBitmapRenderer: public wxDataViewRenderer
{
public:
    wxDataViewBitmapRenderer( const wxString &varianttype = wxT("wxBitmap"),
                              wxDataViewCellMode mode = wxDATAVIEW_CELL_INERT,
                              int align = wxDVR_DEFAULT_ALIGNMENT );

    bool SetValue( const wxVariant &value );
    bool GetValue( wxVariant &value ) const;

protected:
    DECLARE_DYNAMIC_CLASS_NO_COPY(wxDataViewBitmapRenderer)
};

// ---------------------------------------------------------
// wxDataViewToggleRenderer
// ---------------------------------------------------------

class WXDLLIMPEXP_ADV wxDataViewToggleRenderer: public wxDataViewRenderer
{
public:
    wxDataViewToggleRenderer( const wxString &varianttype = wxT("bool"),
                              wxDataViewCellMode mode = wxDATAVIEW_CELL_INERT,
                              int align = wxDVR_DEFAULT_ALIGNMENT );

    bool SetValue( const wxVariant &value );
    bool GetValue( wxVariant &value ) const;

protected:
    DECLARE_DYNAMIC_CLASS_NO_COPY(wxDataViewToggleRenderer)
};

// ---------------------------------------------------------
// wxDataViewCustomRenderer
// ---------------------------------------------------------

class WXDLLIMPEXP_ADV wxDataViewCustomRenderer: public wxDataViewRenderer
{
public:
    wxDataViewCustomRenderer( const wxString &varianttype = wxT("string"),
                              wxDataViewCellMode mode = wxDATAVIEW_CELL_INERT,
                              int align = wxDVR_DEFAULT_ALIGNMENT,
                              bool no_init = false );
    virtual ~wxDataViewCustomRenderer();


    virtual bool Render( wxRect cell, wxDC *dc, int state ) = 0;

    void RenderText( const wxString &text, int xoffset,  wxRect cell, wxDC *dc, int state );

    virtual wxSize GetSize() const = 0;

    virtual bool Activate( wxRect WXUNUSED(cell),
                           wxDataViewModel *WXUNUSED(model), const wxDataViewItem &WXUNUSED(item), unsigned int WXUNUSED(col) )
                           { return false; }

    virtual bool LeftClick( wxPoint WXUNUSED(cursor), wxRect WXUNUSED(cell),
                           wxDataViewModel *WXUNUSED(model), const wxDataViewItem &WXUNUSED(item), unsigned int WXUNUSED(col) )
                           { return false; }
    virtual bool StartDrag( wxPoint WXUNUSED(cursor), wxRect WXUNUSED(cell),
                           wxDataViewModel *WXUNUSED(model), const wxDataViewItem &WXUNUSED(item), unsigned int WXUNUSED(col) )
                           { return false; }

    // Create DC on request
    virtual wxDC *GetDC();


protected:

    bool Init(wxDataViewCellMode mode, int align);

private:
    wxDC        *m_dc;

public:
    // Internal, temporay for RenderText.
    GtkCellRenderer      *m_text_renderer;
    GdkWindow            *window;
    GtkWidget            *widget;
    void                 *background_area;
    void                 *cell_area;
    void                 *expose_area;
    int                   flags;

protected:
    DECLARE_DYNAMIC_CLASS_NO_COPY(wxDataViewCustomRenderer)
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
    bool GetValue( wxVariant &value ) const;

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
    bool GetValue( wxVariant &value ) const;

    virtual bool Render( wxRect cell, wxDC *dc, int state );
    virtual wxSize GetSize() const;
    virtual bool Activate( wxRect cell,
                           wxDataViewModel *model, const wxDataViewItem &item, unsigned int col );

private:
    wxDateTime    m_date;

protected:
    DECLARE_DYNAMIC_CLASS_NO_COPY(wxDataViewDateRenderer)
};

// -------------------------------------
// wxDataViewChoiceRenderer
// -------------------------------------

class WXDLLIMPEXP_ADV wxDataViewChoiceRenderer: public wxDataViewCustomRenderer
{
public:
    wxDataViewChoiceRenderer( const wxArrayString &choices,
                            wxDataViewCellMode mode = wxDATAVIEW_CELL_EDITABLE,
                            int alignment = wxDVR_DEFAULT_ALIGNMENT );
    virtual bool Render( wxRect rect, wxDC *dc, int state );
    virtual wxSize GetSize() const;
    virtual bool SetValue( const wxVariant &value );
    virtual bool GetValue( wxVariant &value ) const;

    void SetAlignment( int align );
private:
    wxArrayString m_choices;
    wxString      m_data;
};

// ---------------------------------------------------------
// wxDataViewColumn
// ---------------------------------------------------------

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

    virtual void SetFlags(int flags) { SetIndividualFlags(flags); }

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

    virtual int GetFlags() const { return GetFromIndividualFlags(); }

    // implementation
    GtkWidget* GetGtkHandle() { return m_column; }
    GtkWidget* GetConstGtkHandle() const { return m_column; }

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

WX_DECLARE_LIST_WITH_DECL(wxDataViewColumn, wxDataViewColumnList,
                          class WXDLLIMPEXP_ADV);

// ---------------------------------------------------------
// wxDataViewCtrl
// ---------------------------------------------------------

class WXDLLIMPEXP_ADV wxDataViewCtrl: public wxDataViewCtrlBase
{
public:
    wxDataViewCtrl()
    {
        Init();
    }

    wxDataViewCtrl( wxWindow *parent, wxWindowID id,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize, long style = 0,
           const wxValidator& validator = wxDefaultValidator )
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

    virtual void Expand( const wxDataViewItem & item );
    virtual void Collapse( const wxDataViewItem & item );
    virtual bool IsExpanded( const wxDataViewItem & item ) const;

    virtual bool EnableDragSource( const wxDataFormat &format );
    virtual bool EnableDropTarget( const wxDataFormat &format );

    static wxVisualAttributes
    GetClassDefaultAttributes(wxWindowVariant variant = wxWINDOW_VARIANT_NORMAL);

    wxWindow *GetMainWindow() { return (wxWindow*) this; }

    GtkWidget *GtkGetTreeView() { return m_treeview; }
    wxDataViewCtrlInternal* GtkGetInternal() { return m_internal; }

    virtual void OnInternalIdle();

protected:
    virtual void DoSetExpanderColumn();
    virtual void DoSetIndent();

private:
    friend class wxDataViewCtrlDCImpl;
    friend class wxDataViewColumn;
    friend class wxGtkDataViewModelNotifier;
    friend class wxDataViewCtrlInternal;

    GtkWidget               *m_treeview;
    wxDataViewModelNotifier *m_notifier;
    wxDataViewCtrlInternal  *m_internal;
    wxDataViewColumnList     m_cols;

    virtual void AddChildGTK(wxWindowGTK* child);
    void GtkEnableSelectionEvents();
    void GtkDisableSelectionEvents();

    DECLARE_DYNAMIC_CLASS(wxDataViewCtrl)
    DECLARE_NO_COPY_CLASS(wxDataViewCtrl)
};


#endif // _WX_GTKDATAVIEWCTRL_H_
