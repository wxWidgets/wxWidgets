/////////////////////////////////////////////////////////////////////////////
// Name:        wx/generic/dataview.h
// Purpose:     wxDataViewCtrl generic implementation header
// Author:      Robert Roebling
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

class WXDLLIMPEXP_ADV wxDataViewCtrl;
class WXDLLIMPEXP_ADV wxDataViewMainWindow;
class WXDLLIMPEXP_ADV wxDataViewHeaderWindow;

// ---------------------------------------------------------
// wxDataViewCell
// ---------------------------------------------------------

class WXDLLIMPEXP_ADV wxDataViewCell: public wxDataViewCellBase
{
public:
    wxDataViewCell( const wxString &varianttype, wxDataViewCellMode mode = wxDATAVIEW_CELL_INERT );
    virtual ~wxDataViewCell();

    virtual bool Render( wxRect cell, wxDC *dc, int state ) = 0;
    virtual wxSize GetSize() = 0;

    virtual bool Activate( wxRect WXUNUSED(cell),
                           wxDataViewListModel *WXUNUSED(model),
                           size_t WXUNUSED(col),
                           size_t WXUNUSED(row) )
                           { return false; }

    virtual bool LeftClick( wxPoint WXUNUSED(cursor),
                            wxRect WXUNUSED(cell),
                            wxDataViewListModel *WXUNUSED(model),
                            size_t WXUNUSED(col),
                            size_t WXUNUSED(row) )
                            { return false; }
    virtual bool RightClick( wxPoint WXUNUSED(cursor),
                             wxRect WXUNUSED(cell),
                             wxDataViewListModel *WXUNUSED(model),
                             size_t WXUNUSED(col),
                             size_t WXUNUSED(row) )
                             { return false; }
    virtual bool StartDrag( wxPoint WXUNUSED(cursor),
                            wxRect WXUNUSED(cell),
                            wxDataViewListModel *WXUNUSED(model),
                            size_t WXUNUSED(col),
                            size_t WXUNUSED(row) )
                            { return false; }

    // Create DC on request
    virtual wxDC *GetDC();

private:
    wxDC        *m_dc;

protected:
    DECLARE_DYNAMIC_CLASS_NO_COPY(wxDataViewCell)
};

// ---------------------------------------------------------
// wxDataViewCustomCell
// ---------------------------------------------------------

class WXDLLIMPEXP_ADV wxDataViewCustomCell: public wxDataViewCell
{
public:
    wxDataViewCustomCell( const wxString &varianttype = wxT("string"),
                          wxDataViewCellMode mode = wxDATAVIEW_CELL_INERT );

protected:
    DECLARE_DYNAMIC_CLASS_NO_COPY(wxDataViewCustomCell)
};

// ---------------------------------------------------------
// wxDataViewTextCell
// ---------------------------------------------------------

class WXDLLIMPEXP_ADV wxDataViewTextCell: public wxDataViewCustomCell
{
public:
    wxDataViewTextCell( const wxString &varianttype = wxT("string"),
                        wxDataViewCellMode mode = wxDATAVIEW_CELL_INERT );

    bool SetValue( const wxVariant &value );
    bool GetValue( wxVariant &value );

    bool Render( wxRect cell, wxDC *dc, int state );
    wxSize GetSize();

private:
    wxString m_text;

protected:
    DECLARE_DYNAMIC_CLASS_NO_COPY(wxDataViewTextCell)
};

// ---------------------------------------------------------
// wxDataViewBitmapCell
// ---------------------------------------------------------

class WXDLLIMPEXP_ADV wxDataViewBitmapCell: public wxDataViewCustomCell
{
public:
    wxDataViewBitmapCell( const wxString &varianttype = wxT("wxBitmap"),
                        wxDataViewCellMode mode = wxDATAVIEW_CELL_INERT );

    bool SetValue( const wxVariant &value );
    bool GetValue( wxVariant &value );

    bool Render( wxRect cell, wxDC *dc, int state );
    wxSize GetSize();

private:
    wxIcon m_icon;
    wxBitmap m_bitmap;

protected:
    DECLARE_DYNAMIC_CLASS_NO_COPY(wxDataViewBitmapCell)
};

// ---------------------------------------------------------
// wxDataViewToggleCell
// ---------------------------------------------------------

class WXDLLIMPEXP_ADV wxDataViewToggleCell: public wxDataViewCustomCell
{
public:
    wxDataViewToggleCell( const wxString &varianttype = wxT("bool"),
                        wxDataViewCellMode mode = wxDATAVIEW_CELL_INERT );

    bool SetValue( const wxVariant &value );
    bool GetValue( wxVariant &value );

    bool Render( wxRect cell, wxDC *dc, int state );
    bool Activate( wxRect cell, wxDataViewListModel *model, size_t col, size_t row );
    wxSize GetSize();

private:
    bool    m_toggle;

protected:
    DECLARE_DYNAMIC_CLASS_NO_COPY(wxDataViewToggleCell)
};

// ---------------------------------------------------------
// wxDataViewProgressCell
// ---------------------------------------------------------

class WXDLLIMPEXP_ADV wxDataViewProgressCell: public wxDataViewCustomCell
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

class WXDLLIMPEXP_ADV wxDataViewDateCell: public wxDataViewCustomCell
{
public:
    wxDataViewDateCell( const wxString &varianttype = wxT("datetime"),
                        wxDataViewCellMode mode = wxDATAVIEW_CELL_ACTIVATABLE );

    bool SetValue( const wxVariant &value );

    virtual bool Render( wxRect cell, wxDC *dc, int state );
    virtual wxSize GetSize();
    virtual bool Activate( wxRect cell,
                           wxDataViewListModel *model, size_t col, size_t row );

private:
    wxDateTime    m_date;

protected:
    DECLARE_DYNAMIC_CLASS_NO_COPY(wxDataViewDateCell)
};

// ---------------------------------------------------------
// wxDataViewColumn
// ---------------------------------------------------------

class WXDLLIMPEXP_ADV wxDataViewColumn: public wxDataViewColumnBase
{
public:
    wxDataViewColumn( const wxString &title, wxDataViewCell *cell, size_t model_column, 
        int fixed_width = 80, wxDataViewColumnSizing sizing = wxDATAVIEW_COL_WIDTH_FIXED, int flags = 0 );
    virtual ~wxDataViewColumn();

    virtual void SetTitle( const wxString &title );

    virtual int GetWidth();

    virtual void SetFixedWidth( int width );
    virtual int GetFixedWidth();
    
private:
    int                      m_width;
    wxDataViewColumnSizing   m_sizing;
    int                      m_fixedWidth;

protected:
    DECLARE_DYNAMIC_CLASS_NO_COPY(wxDataViewColumn)
};

// ---------------------------------------------------------
// wxDataViewCtrl
// ---------------------------------------------------------

class WXDLLIMPEXP_ADV wxDataViewCtrl: public wxDataViewCtrlBase,
                                       public wxScrollHelperNative
{
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

    virtual bool AssociateModel( wxDataViewListModel *model );
    virtual bool AppendColumn( wxDataViewColumn *col );

private:
    friend class wxDataViewMainWindow;
    friend class wxDataViewHeaderWindow;
    wxDataViewListModelNotifier *m_notifier;
    wxDataViewMainWindow        *m_clientArea;
    wxDataViewHeaderWindow      *m_headerArea;

private:
    void OnSize( wxSizeEvent &event );

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
