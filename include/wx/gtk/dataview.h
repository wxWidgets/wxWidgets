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
// wxDataViewCell
// --------------------------------------------------------- 

class wxDataViewCell: public wxDataViewCellBase
{
public:
    wxDataViewCell( const wxString &varianttype, wxDataViewCellMode mode = wxDATAVIEW_CELL_INERT );

    // implementation
    void* GetGtkHandle() { return m_renderer; }

protected:
    // holds the GTK handle
    void*   m_renderer;

protected:
    DECLARE_DYNAMIC_CLASS_NO_COPY(wxDataViewCell)
};
    
// --------------------------------------------------------- 
// wxDataViewTextCell
// --------------------------------------------------------- 

class wxDataViewTextCell: public wxDataViewCell
{
public:
    wxDataViewTextCell( const wxString &varianttype = wxT("string"), 
                        wxDataViewCellMode mode = wxDATAVIEW_CELL_INERT );

    bool SetValue( const wxVariant &value );
    bool GetValue( wxVariant &value );
    
protected:
    DECLARE_DYNAMIC_CLASS_NO_COPY(wxDataViewTextCell)
};
    
// --------------------------------------------------------- 
// wxDataViewBitmapCell
// --------------------------------------------------------- 

class wxDataViewBitmapCell: public wxDataViewCell
{
public:
    wxDataViewBitmapCell( const wxString &varianttype = wxT("wxBitmap"), 
                        wxDataViewCellMode mode = wxDATAVIEW_CELL_INERT );

    bool SetValue( const wxVariant &value );
    bool GetValue( wxVariant &value );
    
protected:
    DECLARE_DYNAMIC_CLASS_NO_COPY(wxDataViewBitmapCell)
};
    
// --------------------------------------------------------- 
// wxDataViewToggleCell
// --------------------------------------------------------- 

class wxDataViewToggleCell: public wxDataViewCell
{
public:
    wxDataViewToggleCell( const wxString &varianttype = wxT("bool"), 
                        wxDataViewCellMode mode = wxDATAVIEW_CELL_INERT );

    bool SetValue( const wxVariant &value );
    bool GetValue( wxVariant &value );
    
protected:
    DECLARE_DYNAMIC_CLASS_NO_COPY(wxDataViewToggleCell)
};
    
// --------------------------------------------------------- 
// wxDataViewCustomCell
// --------------------------------------------------------- 

class wxDataViewCustomCell: public wxDataViewCell
{
public:
    wxDataViewCustomCell( const wxString &varianttype = wxT("string"), 
                          wxDataViewCellMode mode = wxDATAVIEW_CELL_INERT,
                          bool no_init = false );
    virtual ~wxDataViewCustomCell();
    bool Init();
    
    virtual bool Render( wxRect cell, wxDC *dc, int state ) = 0;
    virtual wxSize GetSize() = 0;
    
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
    
private:
    wxDC        *m_dc;
    
protected:
    DECLARE_DYNAMIC_CLASS_NO_COPY(wxDataViewCustomCell)
};
    
// --------------------------------------------------------- 
// wxDataViewProgressCell
// --------------------------------------------------------- 

class wxDataViewProgressCell: public wxDataViewCustomCell
{
public:
    wxDataViewProgressCell( const wxString &label = wxEmptyString, 
                            const wxString &varianttype = wxT("long"), 
                            wxDataViewCellMode mode = wxDATAVIEW_CELL_INERT );
    virtual ~wxDataViewProgressCell();
    
    bool SetValue( const wxVariant &value );
    
    virtual bool Render( wxRect cell, wxDC *dc, int state );
    virtual wxSize GetSize();
    
private:
    wxString    m_label;
    int         m_value;
    
protected:
    DECLARE_DYNAMIC_CLASS_NO_COPY(wxDataViewProgressCell)
};
    
// --------------------------------------------------------- 
// wxDataViewDateCell
// --------------------------------------------------------- 

class wxDataViewDateCell: public wxDataViewCustomCell
{
public:
    wxDataViewDateCell( const wxString &varianttype = wxT("datetime"), 
                        wxDataViewCellMode mode = wxDATAVIEW_CELL_ACTIVATABLE );
    
    bool SetValue( const wxVariant &value );
    
    virtual bool Render( wxRect cell, wxDC *dc, int state );
    virtual wxSize GetSize();
    virtual bool Activate( wxRect cell,
                           wxDataViewListModel *model, unsigned int col, unsigned int row );
    
private:
    wxDateTime    m_date;
    
protected:
    DECLARE_DYNAMIC_CLASS_NO_COPY(wxDataViewDateCell)
};
    
// --------------------------------------------------------- 
// wxDataViewColumn
// --------------------------------------------------------- 

class WXDLLIMPEXP_CORE wxDataViewColumn: public wxDataViewColumnBase
{
public:
    wxDataViewColumn( const wxString &title, wxDataViewCell *cell, unsigned int model_column,
        int width = 80, int flags = wxDATAVIEW_COL_RESIZABLE );
    virtual ~wxDataViewColumn();

    virtual void SetTitle( const wxString &title );

    virtual int GetWidth();
    
    virtual void SetFixedWidth( int width );
    virtual int GetFixedWidth();
    
    // implementation
    void* GetGtkHandle() { return m_column; }

private:
    // holds the GTK handle
    void*   m_column;

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
    
private:
    friend class wxDataViewCtrlDC;
    friend class wxGtkDataViewListModelNotifier;
    GtkWidget                   *m_treeview;
    wxDataViewListModelNotifier *m_notifier;
    
private:
    DECLARE_DYNAMIC_CLASS(wxDataViewCtrl)
    DECLARE_NO_COPY_CLASS(wxDataViewCtrl)
};


#endif // __GTKDATAVIEWCTRLH__
