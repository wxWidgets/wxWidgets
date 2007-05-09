/////////////////////////////////////////////////////////////////////////////
// Name:        wx/gtk/dataview.h
// Purpose:     wxDataViewCtrl GTK+2 implementation header
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef __GTKDATAVIEWCTRLH__
#define __GTKDATAVIEWCTRLH__

#include "wx/defs.h"
#include "wx/object.h"
#include "wx/list.h"
#include "wx/control.h"

// --------------------------------------------------------- 
// classes
// --------------------------------------------------------- 

class WXDLLIMPEXP_CORE wxDataViewCtrl;


// --------------------------------------------------------- 
// wxDataViewRenderer
// --------------------------------------------------------- 

class wxDataViewRenderer: public wxDataViewRendererBase
{
public:
    wxDataViewRenderer( const wxString &varianttype, 
                        wxDataViewCellMode mode = wxDATAVIEW_CELL_INERT,
                        int align = wxDVR_DEFAULT_ALIGNMENT );

    // implementation
    GtkCellRenderer* GetGtkHandle() { return m_renderer; }

    virtual void SetMode( wxDataViewCellMode mode );
    virtual wxDataViewCellMode GetMode() const;

    virtual void SetAlignment( int align );
    virtual int GetAlignment() const;

protected:
    GtkCellRenderer   *m_renderer;

protected:
    DECLARE_DYNAMIC_CLASS_NO_COPY(wxDataViewRenderer)
};

// --------------------------------------------------------- 
// wxDataViewTextRenderer
// --------------------------------------------------------- 

class wxDataViewTextRenderer: public wxDataViewRenderer
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
// wxDataViewBitmapRenderer
// --------------------------------------------------------- 

class wxDataViewBitmapRenderer: public wxDataViewRenderer
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

class wxDataViewToggleRenderer: public wxDataViewRenderer
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

class wxDataViewCustomRenderer: public wxDataViewRenderer
{
public:
    wxDataViewCustomRenderer( const wxString &varianttype = wxT("string"), 
                              wxDataViewCellMode mode = wxDATAVIEW_CELL_INERT,
                              int align = wxDVR_DEFAULT_ALIGNMENT,
                              bool no_init = false );
    virtual ~wxDataViewCustomRenderer();


    virtual bool Render( wxRect cell, wxDC *dc, int state ) = 0;
    virtual wxSize GetSize() const = 0;

    virtual bool Activate( wxRect cell,
                           wxDataViewListModel *model, unsigned int col, unsigned int row )   
                           { return false; }

    virtual bool LeftClick( wxPoint cursor, wxRect cell, 
                           wxDataViewListModel *model, unsigned int col, unsigned int row )   
                           { return false; }
    virtual bool RightClick( wxPoint cursor, wxRect cell,
                           wxDataViewListModel *model, unsigned int col, unsigned int row )   
                           { return false; }
    virtual bool StartDrag( wxPoint cursor, wxRect cell, 
                           wxDataViewListModel *model, unsigned int col, unsigned int row )   
                           { return false; }
    
    // Create DC on request
    virtual wxDC *GetDC();
    

protected:

    bool Init(wxDataViewCellMode mode, int align);

private:
    wxDC        *m_dc;
    
protected:
    DECLARE_DYNAMIC_CLASS_NO_COPY(wxDataViewCustomRenderer)
};
    
// --------------------------------------------------------- 
// wxDataViewProgressRenderer
// --------------------------------------------------------- 

class wxDataViewProgressRenderer: public wxDataViewCustomRenderer
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
// wxDataViewDateRenderer
// --------------------------------------------------------- 

class wxDataViewDateRenderer: public wxDataViewCustomRenderer
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
                           wxDataViewListModel *model, unsigned int col, unsigned int row );
    
private:
    wxDateTime    m_date;
    
protected:
    DECLARE_DYNAMIC_CLASS_NO_COPY(wxDataViewDateRenderer)
};
    
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
    wxDataViewColumn( const wxBitmap &bitmap, wxDataViewRenderer *renderer,
                      unsigned int model_column, int width = wxDVC_DEFAULT_WIDTH,
                      wxAlignment align = wxALIGN_CENTER,
                      int flags = wxDATAVIEW_COL_RESIZABLE );

    virtual ~wxDataViewColumn();


    // setters:

    virtual void SetTitle( const wxString &title );
    virtual void SetBitmap( const wxBitmap &bitmap );

    virtual void SetOwner( wxDataViewCtrl *owner );

    virtual void SetAlignment( wxAlignment align );
    
    virtual void SetSortable( bool sortable );
    virtual void SetSortOrder( bool ascending );

    virtual void SetResizeable( bool resizeable );
    virtual void SetHidden( bool hidden );

    virtual void SetMinWidth( int minWidth );
    virtual void SetWidth( int width );


    // getters:

    virtual wxString GetTitle() const;
    virtual wxAlignment GetAlignment() const;

    virtual bool IsSortable() const;
    virtual bool IsSortOrderAscending() const;
    virtual bool IsResizeable() const;
    virtual bool IsHidden() const;

    virtual int GetWidth() const;
    virtual int GetMinWidth() const;

    // implementation
    GtkWidget* GetGtkHandle() { return m_column; }

private:
    // holds the GTK handle
    GtkWidget   *m_column;
    
    // delayed connection to mouse events
    friend class wxDataViewCtrl;
    void OnInternalIdle();
    bool    m_isConnected;

    void Init(wxAlignment align, int flags, int width);

protected:
    DECLARE_DYNAMIC_CLASS_NO_COPY(wxDataViewColumn)
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

    virtual bool AssociateModel( wxDataViewListModel *model );
    virtual bool AppendColumn( wxDataViewColumn *col );
    
    virtual void SetSelection( int row ); // -1 for unselect
    virtual void SetSelectionRange( unsigned int from, unsigned int to );
    virtual void SetSelections( const wxArrayInt& aSelections);
    virtual void Unselect( unsigned int row );
    
    virtual bool IsSelected( unsigned int row ) const;
    virtual int GetSelection() const;
    virtual int GetSelections(wxArrayInt& aSelections) const;

    static wxVisualAttributes
    GetClassDefaultAttributes(wxWindowVariant variant = wxWINDOW_VARIANT_NORMAL);
    
    GtkWidget *GtkGetTreeView() { return m_treeview; }
    wxWindow *GetMainWindow() { return (wxWindow*) this; }
    
private:
    friend class wxDataViewCtrlDC;
    friend class wxDataViewColumn;
    friend class wxGtkDataViewListModelNotifier;
    GtkWidget                   *m_treeview;
    wxDataViewListModelNotifier *m_notifier;
    
    virtual void OnInternalIdle();
    
private:
    DECLARE_DYNAMIC_CLASS(wxDataViewCtrl)
    DECLARE_NO_COPY_CLASS(wxDataViewCtrl)
};


#endif // __GTKDATAVIEWCTRLH__
