/////////////////////////////////////////////////////////////////////////////
// Name:        wx/dataview.h
// Purpose:     wxDataViewCtrl base classes
// Author:      Robert Roebling
// Modified by:
// Created:     08.01.06
// RCS-ID:      $Id$
// Copyright:   (c) Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_DATAVIEW_H_BASE_
#define _WX_DATAVIEW_H_BASE_

#include "wx/defs.h"

#if wxUSE_DATAVIEWCTRL

#include "wx/control.h"
#include "wx/textctrl.h"
#include "wx/bitmap.h"
#include "wx/variant.h"


#if defined(__WXGTK20__)
    // for testing
    // #define wxUSE_GENERICDATAVIEWCTRL 1
#elif defined(__WXMAC__)
    #define wxUSE_GENERICDATAVIEWCTRL 1
#else
    #define wxUSE_GENERICDATAVIEWCTRL 1
#endif

// ----------------------------------------------------------------------------
// wxDataViewCtrl flags
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// wxDataViewCtrl globals
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_ADV wxDataViewModel;
class WXDLLIMPEXP_ADV wxDataViewListModel;
class WXDLLIMPEXP_ADV wxDataViewCtrl;
class WXDLLIMPEXP_ADV wxDataViewColumn;
class WXDLLIMPEXP_ADV wxDataViewRenderer;

extern WXDLLIMPEXP_DATA_ADV(const wxChar) wxDataViewCtrlNameStr[];

// the default width of new (text) columns:
#define wxDVC_DEFAULT_WIDTH             80

// the default width of new toggle columns:
#define wxDVC_TOGGLE_DEFAULT_WIDTH      30

// the default minimal width of the columns:
#define wxDVC_DEFAULT_MINWIDTH          30

// the default alignment of wxDataViewRenderers:
#define wxDVR_DEFAULT_ALIGNMENT         (wxALIGN_LEFT|wxALIGN_TOP)


// ---------------------------------------------------------
// wxDataViewModel
// ---------------------------------------------------------

class WXDLLIMPEXP_ADV wxDataViewModel: public wxObjectRefData
{
public:
    wxDataViewModel() { }

protected:
    // the user should not delete this class directly: he should use DecRef() instead!
    virtual ~wxDataViewModel() { }
};

// ---------------------------------------------------------
// wxDataViewListModelNotifier
// ---------------------------------------------------------

class WXDLLIMPEXP_ADV wxDataViewListModelNotifier: public wxObject
{
public:
    wxDataViewListModelNotifier() { }
    virtual ~wxDataViewListModelNotifier() { }

    virtual bool RowAppended() = 0;
    virtual bool RowPrepended() = 0;
    virtual bool RowInserted( unsigned int before ) = 0;
    virtual bool RowDeleted( unsigned int row ) = 0;
    virtual bool RowChanged( unsigned int row ) = 0;
    virtual bool ValueChanged( unsigned int col, unsigned int row ) = 0;
    virtual bool RowsReordered( unsigned int *new_order ) = 0;
    virtual bool Cleared() = 0;
    virtual bool Freed()
        { m_owner = NULL; return true; }

    void SetOwner( wxDataViewListModel *owner ) { m_owner = owner; }
    wxDataViewListModel *GetOwner()             { return m_owner; }

private:
    wxDataViewListModel *m_owner;
};

// ---------------------------------------------------------
// wxDataViewListModel
// ---------------------------------------------------------

class WXDLLIMPEXP_ADV wxDataViewViewingColumn: public wxObject
{
public:
    wxDataViewViewingColumn( wxDataViewColumn *view_column, unsigned int model_column )
    {
        m_viewColumn = view_column;
        m_modelColumn = model_column;
    }

    wxDataViewColumn   *m_viewColumn;
    unsigned int        m_modelColumn;
};

class WXDLLIMPEXP_ADV wxDataViewListModel: public wxDataViewModel
{
    friend class WXDLLIMPEXP_ADV wxDataViewCtrl;
    friend class WXDLLIMPEXP_ADV wxDataViewCtrlBase;
    friend class WXDLLIMPEXP_ADV wxDataViewSortedListModel;
    friend class WXDLLIMPEXP_ADV wxDataViewColumnBase;
    friend class WXDLLIMPEXP_ADV wxGtkDataViewListModelNotifier;

public:
    wxDataViewListModel();

    virtual unsigned int GetRowCount() const = 0;
    virtual unsigned int GetColumnCount() const = 0;

    // return type as reported by wxVariant
    virtual wxString GetColumnType( unsigned int col ) const = 0;

    // get value into a wxVariant
    virtual void GetValue( wxVariant &variant, unsigned int col, unsigned int row ) const = 0;

    // set value, call ValueChanged() afterwards!
    virtual bool SetValue( wxVariant &variant, unsigned int col, unsigned int row ) = 0;

    // delegated notifiers
    virtual bool RowAppended();
    virtual bool RowPrepended();
    virtual bool RowInserted( unsigned int before );
    virtual bool RowDeleted( unsigned int row );
    virtual bool RowChanged( unsigned int row );
    virtual bool ValueChanged( unsigned int col, unsigned int row );
    virtual bool RowsReordered( unsigned int *new_order );
    virtual bool Cleared();

protected:
    // the user should not delete this class directly: he should use DecRef() instead!
    virtual ~wxDataViewListModel();

    // Used internally
    void AddViewingColumn( wxDataViewColumn *view_column, unsigned int model_column );
    void RemoveViewingColumn( wxDataViewColumn *column );

    void AddNotifier( wxDataViewListModelNotifier *notifier );
    void RemoveNotifier( wxDataViewListModelNotifier *notifier );

    wxList                      m_notifiers;
    wxList                      m_viewingColumns;
};



// ---------------------------------------------------------
// wxDataViewSortedListModel
// ---------------------------------------------------------

typedef int (wxCALLBACK *wxDataViewListModelCompare)
    (unsigned int row1, unsigned int row2, unsigned int col, wxDataViewListModel* model );

WX_DEFINE_SORTED_USER_EXPORTED_ARRAY_SIZE_T(unsigned int, 
                                            wxDataViewSortedIndexArray, WXDLLIMPEXP_ADV);

class WXDLLIMPEXP_ADV wxDataViewSortedListModel: public wxDataViewListModel
{
    friend class wxDataViewSortedListModelNotifier;

public:
    wxDataViewSortedListModel( wxDataViewListModel *child );
    virtual ~wxDataViewSortedListModel();

    void SetAscending( bool ascending ) { m_ascending = ascending; }
    bool IsAscending() const { return m_ascending; }

    virtual unsigned int GetRowCount() const;
    virtual unsigned int GetColumnCount() const;

    // return type as reported by wxVariant
    virtual wxString GetColumnType( unsigned int col ) const;

    // get value into a wxVariant
    virtual void GetValue( wxVariant &variant, unsigned int col, unsigned int row ) const;

    // set value, call ValueChanged() afterwards!
    virtual bool SetValue( wxVariant &variant, unsigned int col, unsigned int row );

    // called from user
    virtual bool RowAppended();
    virtual bool RowPrepended();
    virtual bool RowInserted( unsigned int before );
    virtual bool RowDeleted( unsigned int row );
    virtual bool RowChanged( unsigned int row );
    virtual bool ValueChanged( unsigned int col, unsigned int row );
    virtual bool RowsReordered( unsigned int *new_order );
    virtual bool Cleared();

    // called if child's notifiers are called
    bool ChildRowAppended();
    bool ChildRowPrepended();
    bool ChildRowInserted( unsigned int before );
    bool ChildRowDeleted( unsigned int row );
    bool ChildRowChanged( unsigned int row );
    bool ChildValueChanged( unsigned int col, unsigned int row );
    bool ChildRowsReordered( unsigned int *new_order );
    bool ChildCleared();

    virtual void Resort();

private:
    bool                             m_ascending;
    wxDataViewListModel             *m_child;
    wxDataViewSortedIndexArray       m_array;
    wxDataViewListModelNotifier     *m_notifierOnChild;
    
    void InitStatics(); // BAD
};

//-----------------------------------------------------------------------------
// wxDataViewEditorCtrlEvtHandler
//-----------------------------------------------------------------------------

class wxDataViewEditorCtrlEvtHandler: public wxEvtHandler
{
public:
    wxDataViewEditorCtrlEvtHandler( wxControl *editor, wxDataViewRenderer *owner );
                         
    void AcceptChangesAndFinish();
    void SetFocusOnIdle( bool focus = true ) { m_focusOnIdle = focus; }

protected:
    void OnChar( wxKeyEvent &event );
    void OnKillFocus( wxFocusEvent &event );
    void OnIdle( wxIdleEvent &event );

private:
    wxDataViewRenderer     *m_owner;
    wxControl              *m_editorCtrl;
    bool                    m_finished;
    bool                    m_focusOnIdle;

private:
    DECLARE_EVENT_TABLE()
};

// ---------------------------------------------------------
// wxDataViewRendererBase
// ---------------------------------------------------------

enum wxDataViewCellMode
{
    wxDATAVIEW_CELL_INERT,
    wxDATAVIEW_CELL_ACTIVATABLE,
    wxDATAVIEW_CELL_EDITABLE
};

enum wxDataViewCellRenderState
{
    wxDATAVIEW_CELL_SELECTED    = 1,
    wxDATAVIEW_CELL_PRELIT      = 2,
    wxDATAVIEW_CELL_INSENSITIVE = 4,
    wxDATAVIEW_CELL_FOCUSED     = 8
};

class WXDLLIMPEXP_ADV wxDataViewRendererBase: public wxObject
{
public:
    wxDataViewRendererBase( const wxString &varianttype, 
                            wxDataViewCellMode mode = wxDATAVIEW_CELL_INERT,
                            int alignment = wxDVR_DEFAULT_ALIGNMENT );

    virtual bool Validate( wxVariant& WXUNUSED(value) )
        { return true; }

    void SetOwner( wxDataViewColumn *owner )    { m_owner = owner; }
    wxDataViewColumn* GetOwner()                { return m_owner; }

    // renderer properties:

    virtual bool SetValue( const wxVariant& WXUNUSED(value) ) = 0;
    virtual bool GetValue( wxVariant& WXUNUSED(value) ) const = 0;

    wxString GetVariantType() const             { return m_variantType; }

    virtual void SetMode( wxDataViewCellMode mode ) = 0;
    virtual wxDataViewCellMode GetMode() const = 0;

    // NOTE: Set/GetAlignment do not take/return a wxAlignment enum but
    //       rather an "int"; that's because for rendering cells it's allowed
    //       to combine alignment flags (e.g. wxALIGN_LEFT|wxALIGN_BOTTOM)
    virtual void SetAlignment( int align ) = 0;
    virtual int GetAlignment() const = 0;
    
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
    
    wxControl *GetEditorCtrl() { return m_editorCtrl; }
    
protected:
    wxString                m_variantType;
    wxDataViewColumn       *m_owner;
    wxControl              *m_editorCtrl;
    unsigned int            m_row; // for m_editorCtrl

    // internal utility:
    const wxDataViewCtrl* GetView() const;

protected:
    DECLARE_DYNAMIC_CLASS_NO_COPY(wxDataViewRendererBase)
};

// ---------------------------------------------------------
// wxDataViewColumnBase
// ---------------------------------------------------------

enum wxDataViewColumnFlags
{
    wxDATAVIEW_COL_RESIZABLE  = 1,
    wxDATAVIEW_COL_SORTABLE   = 2,
    wxDATAVIEW_COL_HIDDEN     = 4
};

class WXDLLIMPEXP_ADV wxDataViewColumnBase: public wxObject
{
public:
    wxDataViewColumnBase( const wxString &title, wxDataViewRenderer *renderer, 
                          unsigned int model_column, int width = wxDVC_DEFAULT_WIDTH, 
                          wxAlignment align = wxALIGN_CENTER,
                          int flags = wxDATAVIEW_COL_RESIZABLE );
    wxDataViewColumnBase( const wxBitmap &bitmap, wxDataViewRenderer *renderer, 
                          unsigned int model_column, int width = wxDVC_DEFAULT_WIDTH, 
                          wxAlignment align = wxALIGN_CENTER,
                          int flags = wxDATAVIEW_COL_RESIZABLE );
    virtual ~wxDataViewColumnBase();

    // setters:

    virtual void SetTitle( const wxString &title ) = 0;
    virtual void SetAlignment( wxAlignment align ) = 0;
    virtual void SetSortable( bool sortable ) = 0;
    virtual void SetResizeable( bool resizeable ) = 0;
    virtual void SetHidden( bool hidden ) = 0;
    virtual void SetSortOrder( bool ascending ) = 0;
    virtual void SetFlags( int flags );
    virtual void SetOwner( wxDataViewCtrl *owner )  
        { m_owner = owner; }
    virtual void SetBitmap( const wxBitmap &bitmap )
        { m_bitmap=bitmap; }

    virtual void SetMinWidth( int minWidth ) = 0;
    virtual void SetWidth( int width ) = 0;


    // getters:

    virtual wxString GetTitle() const = 0;
    virtual wxAlignment GetAlignment() const = 0;
    virtual int GetWidth() const = 0;
    virtual int GetMinWidth() const = 0;

    virtual int GetFlags() const;

    virtual bool IsSortable() const = 0;
    virtual bool IsResizeable() const = 0;
    virtual bool IsHidden() const = 0;
    virtual bool IsSortOrderAscending() const = 0;

    const wxBitmap &GetBitmap() const       { return m_bitmap; }
    unsigned int GetModelColumn() const     { return m_model_column; }

    wxDataViewCtrl *GetOwner()              { return m_owner; }
    wxDataViewRenderer* GetRenderer()       { return m_renderer; }

protected:
    wxDataViewRenderer      *m_renderer;
    int                      m_model_column;
    wxBitmap                 m_bitmap;
    wxDataViewCtrl          *m_owner;

protected:
    DECLARE_DYNAMIC_CLASS_NO_COPY(wxDataViewColumnBase)
};

// ---------------------------------------------------------
// wxDataViewCtrlBase
// ---------------------------------------------------------

#define wxDV_SINGLE                  0x0000     // for convenience
#define wxDV_MULTIPLE                0x0001     // can select multiple items

#define wxDV_NO_HEADER               0x0002     // column titles not visible
#define wxDV_HORIZ_RULES             0x0004     // light horizontal rules between rows
#define wxDV_VERT_RULES              0x0008     // light vertical rules between columns

class WXDLLIMPEXP_ADV wxDataViewCtrlBase: public wxControl
{
public:
    wxDataViewCtrlBase();
    virtual ~wxDataViewCtrlBase();

    virtual bool AssociateModel( wxDataViewListModel *model );
    wxDataViewListModel* GetModel();

    // short cuts
    bool AppendTextColumn( const wxString &label, unsigned int model_column, 
                    wxDataViewCellMode mode = wxDATAVIEW_CELL_INERT, int width = -1,
                    wxAlignment align = wxALIGN_CENTER,
                    int flags = wxDATAVIEW_COL_RESIZABLE );
    bool AppendToggleColumn( const wxString &label, unsigned int model_column,
                    wxDataViewCellMode mode = wxDATAVIEW_CELL_INERT, int width = wxDVC_TOGGLE_DEFAULT_WIDTH,
                    wxAlignment align = wxALIGN_CENTER,
                    int flags = wxDATAVIEW_COL_RESIZABLE );
    bool AppendProgressColumn( const wxString &label, unsigned int model_column, 
                    wxDataViewCellMode mode = wxDATAVIEW_CELL_INERT, int width = wxDVC_DEFAULT_WIDTH,
                    wxAlignment align = wxALIGN_CENTER,
                    int flags = wxDATAVIEW_COL_RESIZABLE );
    bool AppendDateColumn( const wxString &label, unsigned int model_column,
                    wxDataViewCellMode mode = wxDATAVIEW_CELL_ACTIVATABLE, int width = -1,
                    wxAlignment align = wxALIGN_CENTER,
                    int flags = wxDATAVIEW_COL_RESIZABLE );
    bool AppendBitmapColumn( const wxString &label, unsigned int model_column,
                    wxDataViewCellMode mode = wxDATAVIEW_CELL_INERT, int width = -1,
                    wxAlignment align = wxALIGN_CENTER,
                    int flags = wxDATAVIEW_COL_RESIZABLE );
    bool AppendTextColumn( const wxBitmap &label, unsigned int model_column,
                    wxDataViewCellMode mode = wxDATAVIEW_CELL_INERT, int width = -1,
                    wxAlignment align = wxALIGN_CENTER,
                    int flags = wxDATAVIEW_COL_RESIZABLE );
    bool AppendToggleColumn( const wxBitmap &label, unsigned int model_column,
                    wxDataViewCellMode mode = wxDATAVIEW_CELL_INERT, int width = wxDVC_TOGGLE_DEFAULT_WIDTH,
                    wxAlignment align = wxALIGN_CENTER,
                    int flags = wxDATAVIEW_COL_RESIZABLE );
    bool AppendProgressColumn( const wxBitmap &label, unsigned int model_column,
                    wxDataViewCellMode mode = wxDATAVIEW_CELL_INERT, int width = wxDVC_DEFAULT_WIDTH,
                    wxAlignment align = wxALIGN_CENTER,
                    int flags = wxDATAVIEW_COL_RESIZABLE );
    bool AppendDateColumn( const wxBitmap &label, unsigned int model_column,
                    wxDataViewCellMode mode = wxDATAVIEW_CELL_ACTIVATABLE, int width = -1,
                    wxAlignment align = wxALIGN_CENTER,
                    int flags = wxDATAVIEW_COL_RESIZABLE );
    bool AppendBitmapColumn( const wxBitmap &label, unsigned int model_column,
                    wxDataViewCellMode mode = wxDATAVIEW_CELL_INERT, int width = -1,
                    wxAlignment align = wxALIGN_CENTER,
                    int flags = wxDATAVIEW_COL_RESIZABLE );
    
    virtual bool AppendColumn( wxDataViewColumn *col );

    virtual unsigned int GetColumnCount() const;

    virtual bool DeleteColumn( unsigned int pos );
    virtual bool ClearColumns();
    virtual wxDataViewColumn* GetColumn( unsigned int pos );

    virtual void SetSelection( int row ) = 0; // -1 for unselect
    inline void ClearSelection() { SetSelection( -1 ); }
    virtual void Unselect( unsigned int row ) = 0;
    virtual void SetSelectionRange( unsigned int from, unsigned int to ) = 0;
    virtual void SetSelections( const wxArrayInt& aSelections) = 0;
    
    virtual bool IsSelected( unsigned int row ) const = 0;
    virtual int GetSelection() const = 0;
    virtual int GetSelections(wxArrayInt& aSelections) const = 0;

private:
    wxDataViewListModel    *m_model;
    wxList                  m_cols;
    
protected:
    DECLARE_DYNAMIC_CLASS_NO_COPY(wxDataViewCtrlBase)
};


// ----------------------------------------------------------------------------
// wxDataViewEvent - the event class for the wxDataViewCtrl notifications
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_ADV wxDataViewEvent : public wxNotifyEvent
{
public:
    wxDataViewEvent(wxEventType commandType = wxEVT_NULL, int winid = 0)
        : wxNotifyEvent(commandType, winid),
        m_col(-1),
        m_row(-1),
        m_model(NULL),
        m_value(wxNullVariant),
        m_editCancelled(false),
        m_column(NULL)
        { }

    wxDataViewEvent(const wxDataViewEvent& event)
        : wxNotifyEvent(event),
        m_col(event.m_col),
        m_row(event.m_col),
        m_model(event.m_model),
        m_value(event.m_value),
        m_editCancelled(event.m_editCancelled),
        m_column(event.m_column)
        { }

    int GetColumn() const { return m_col; }
    void SetColumn( int col ) { m_col = col; }

    int GetRow() const { return m_row; }
    void SetRow( int row ) { m_row = row; }

    wxDataViewModel* GetModel() const { return m_model; }
    void SetModel( wxDataViewModel *model ) { m_model = model; }

    const wxVariant &GetValue() const { return m_value; }
    void SetValue( const wxVariant &value ) { m_value = value; }

    // for wxEVT_DATAVIEW_COLUMN_HEADER_CLICKED only
    void SetDataViewColumn( wxDataViewColumn *col ) { m_column = col; }
    wxDataViewColumn *GetDataViewColumn() const { return m_column; }

    // was label editing canceled? (for wxEVT_COMMAND_DATVIEW_END_LABEL_EDIT only)
    bool IsEditCancelled() const { return m_editCancelled; }
    void SetEditCanceled(bool editCancelled) { m_editCancelled = editCancelled; }

    virtual wxEvent *Clone() const { return new wxDataViewEvent(*this); }

protected:
    int                 m_col;
    int                 m_row;
    wxDataViewModel    *m_model;
    wxVariant           m_value;
    bool                m_editCancelled;
    wxDataViewColumn   *m_column;

private:
    DECLARE_DYNAMIC_CLASS_NO_ASSIGN(wxDataViewEvent)
};

BEGIN_DECLARE_EVENT_TYPES()
    DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_ADV, wxEVT_COMMAND_DATAVIEW_ROW_SELECTED, -1)
    DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_ADV, wxEVT_COMMAND_DATAVIEW_ROW_ACTIVATED, -1)
    DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_ADV, wxEVT_COMMAND_DATAVIEW_COLUMN_HEADER_CLICK, -1)
    DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_ADV, wxEVT_COMMAND_DATAVIEW_COLUMN_HEADER_RIGHT_CLICK, -1)
END_DECLARE_EVENT_TYPES()

typedef void (wxEvtHandler::*wxDataViewEventFunction)(wxDataViewEvent&);

#define wxDataViewEventHandler(func) \
    (wxObjectEventFunction)(wxEventFunction)wxStaticCastEvent(wxDataViewEventFunction, &func)

#define wx__DECLARE_DATAVIEWEVT(evt, id, fn) \
    wx__DECLARE_EVT1(wxEVT_COMMAND_DATAVIEW_ ## evt, id, wxDataViewEventHandler(fn))

#define EVT_DATAVIEW_ROW_SELECTED(id, fn) wx__DECLARE_DATAVIEWEVT(ROW_SELECTED, id, fn)
#define EVT_DATAVIEW_ROW_ACTIVATED(id, fn) wx__DECLARE_DATAVIEWEVT(ROW_ACTIVATED, id, fn)
#define EVT_DATAVIEW_COLUMN_HEADER_CLICK(id, fn) wx__DECLARE_DATAVIEWEVT(COLUMN_HEADER_CLICK, id, fn)
#define EVT_DATAVIEW_COLUMN_HEADER_RIGHT_CLICKED(id, fn) wx__DECLARE_DATAVIEWEVT(COLUMN_HEADER_RIGHT_CLICK, id, fn)


#if defined(wxUSE_GENERICDATAVIEWCTRL)
    #include "wx/generic/dataview.h"
#elif defined(__WXGTK20__)
    #include "wx/gtk/dataview.h"
#elif defined(__WXMAC__)
    // TODO
    // #include "wx/mac/dataview.h"
#else
    #include "wx/generic/dataview.h"
#endif

#endif // wxUSE_DATAVIEWCTRL

#endif
    // _WX_DATAVIEW_H_BASE_
