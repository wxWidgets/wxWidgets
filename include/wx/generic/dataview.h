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

//-----------------------------------------------------------------------------
// wxDataViewEditorCtrlEvtHandler
//-----------------------------------------------------------------------------

class wxDataViewEditorCtrlEvtHandler: public wxEvtHandler
{
public:
    wxDataViewEditorCtrlEvtHandler( wxControl *editor, wxDataViewRenderer *owner );
                         
    void AcceptChangesAndFinish();

protected:
    void OnChar( wxKeyEvent &event );
    void OnKillFocus( wxFocusEvent &event );

private:
    wxDataViewRenderer     *m_owner;
    wxControl              *m_editorCtrl;
    bool                    m_finished;

private:
    DECLARE_EVENT_TABLE()
};

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

    virtual void SetAlignment( int align )
        { m_align=align; }
    virtual int GetAlignment() const
        { return m_align; }

    virtual void SetMode( wxDataViewCellMode mode )
        { m_mode=mode; }
    virtual wxDataViewCellMode GetMode() const
        { return m_mode; }

    virtual bool Activate( wxRect WXUNUSED(cell),
                           wxDataViewListModel *WXUNUSED(model),
                           unsigned int WXUNUSED(col),
                           unsigned int WXUNUSED(row) )
                           { return false; }

    virtual bool LeftClick( wxPoint WXUNUSED(cursor),
                            wxRect WXUNUSED(cell),
                            wxDataViewListModel *WXUNUSED(model),
                            unsigned int WXUNUSED(col),
                            unsigned int WXUNUSED(row) )
                            { return false; }
    virtual bool RightClick( wxPoint WXUNUSED(cursor),
                             wxRect WXUNUSED(cell),
                             wxDataViewListModel *WXUNUSED(model),
                             unsigned int WXUNUSED(col),
                             unsigned int WXUNUSED(row) )
                             { return false; }
    virtual bool StartDrag( wxPoint WXUNUSED(cursor),
                            wxRect WXUNUSED(cell),
                            wxDataViewListModel *WXUNUSED(model),
                            unsigned int WXUNUSED(col),
                            unsigned int WXUNUSED(row) )
                            { return false; }

    // Create DC on request
    virtual wxDC *GetDC();

    // in-place editing
    virtual bool HasEditorCtrl()
        { return false; }
    virtual wxControl* CreateEditorCtrl( wxWindow *parent, wxRect labelRect, const wxVariant &value )
        { return NULL; }
    virtual bool GetValueFromEditorCtrl( wxControl* editor, wxVariant &value )
        { return false; }
        
    virtual bool StartEditing( unsigned int row, wxRect labelRect );
    virtual void CancelEditing();
    virtual bool FinishEditing();
    
private:
    wxDC                        *m_dc;
    int                          m_align;
    wxDataViewCellMode           m_mode;
    wxControl                   *m_editorCtrl;
    unsigned int                 m_row; // for m_editorCtrl

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
        
private:
    wxString   m_text;

protected:
    DECLARE_DYNAMIC_CLASS_NO_COPY(wxDataViewTextRenderer)
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
    bool Activate( wxRect cell, wxDataViewListModel *model, unsigned int col, 
                   unsigned int row );
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
                           wxDataViewListModel *model, unsigned int col, unsigned int row );

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


private:
    int                      m_width;
    int                      m_minWidth;
    int                      m_flags;
    wxAlignment              m_align;
    wxString                 m_title;

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

    virtual bool AssociateModel( wxDataViewListModel *model );
    virtual bool AppendColumn( wxDataViewColumn *col );

    virtual void SetSelection( int row ); // -1 for unselect
    virtual void SetSelectionRange( unsigned int from, unsigned int to );
    virtual void SetSelections( const wxArrayInt& aSelections);
    virtual void Unselect( unsigned int row );
    
    virtual bool IsSelected( unsigned int row ) const;
    virtual int GetSelection() const;
    virtual int GetSelections(wxArrayInt& aSelections) const;

public:     // utility functions not part of the API

    // returns the "best" width for the idx-th column
    unsigned int GetBestColumnWidth(int WXUNUSED(idx)) const
    {
        return GetClientSize().GetWidth() / GetColumnCount();
    }

    // updates the header window after a change in a column setting
    void OnColumnChange();

    wxDataViewMainWindow* GetMainWindow() { return m_clientArea; }

private:
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
