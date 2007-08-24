/////////////////////////////////////////////////////////////////////////////
// Name:        wx/dataview.h
// Purpose:     wxDataViewCtrl base classes
// Author:      Robert Roebling
// Modified by: Bo Yang
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
#include "wx/listctrl.h"
#include "wx/dynarray.h"

#if defined(__WXGTK20__)
    // for testing
    #define wxUSE_GENERICDATAVIEWCTRL 1
#elif defined(__WXMAC__)
#else
    #define wxUSE_GENERICDATAVIEWCTRL 1
#endif

// ----------------------------------------------------------------------------
// wxDataViewCtrl flags
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// wxDataViewCtrl globals
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_FWD_ADV wxDataViewItem;
class WXDLLIMPEXP_FWD_ADV wxDataViewModel;
class WXDLLIMPEXP_FWD_ADV wxDataViewCtrl;
class WXDLLIMPEXP_FWD_ADV wxDataViewColumn;
class WXDLLIMPEXP_FWD_ADV wxDataViewRenderer;
class WXDLLIMPEXP_FWD_ADV wxDataViewModelNotifier;

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
// wxDataViewItem
// ---------------------------------------------------------

class WXDLLIMPEXP_ADV wxDataViewItem
{
public:
    wxDataViewItem( void* id = NULL ) 
        { m_id = id; }
    wxDataViewItem( const wxDataViewItem &item )
        { m_id = item.m_id; }
    bool IsOk() const                  { return m_id != NULL; }
    void* GetID() const                { return m_id; }
    operator const void* () const      { return m_id; }
    
private:
    void* m_id;
};

bool operator == (const wxDataViewItem &left, const wxDataViewItem &right);

WX_DEFINE_ARRAY(wxDataViewItem, wxDataViewItemArray);

// ---------------------------------------------------------
// wxDataViewModelNotifier
// ---------------------------------------------------------

class WXDLLIMPEXP_ADV wxDataViewModelNotifier
{
public:
    wxDataViewModelNotifier() { }
    virtual ~wxDataViewModelNotifier() { m_owner = NULL; }

    virtual bool ItemAdded( const wxDataViewItem &parent, const wxDataViewItem &item ) = 0;
    virtual bool ItemDeleted( const wxDataViewItem &parent, const wxDataViewItem &item ) = 0;
    virtual bool ItemChanged( const wxDataViewItem &item ) = 0;
    virtual bool ValueChanged( const wxDataViewItem &item, unsigned int col ) = 0;
    virtual bool Cleared() = 0;
    
    virtual void Resort() = 0;

    void SetOwner( wxDataViewModel *owner ) { m_owner = owner; }
    wxDataViewModel *GetOwner()             { return m_owner; }

private:
    wxDataViewModel *m_owner;
};


// ---------------------------------------------------------
// wxDataViewModel
// ---------------------------------------------------------

WX_DECLARE_LIST(wxDataViewModelNotifier, wxDataViewModelNotifiers );

class WXDLLIMPEXP_ADV wxDataViewModel: public wxObjectRefData
{
public:
    wxDataViewModel();

    virtual unsigned int GetColumnCount() const = 0;

    // return type as reported by wxVariant
    virtual wxString GetColumnType( unsigned int col ) const = 0;

    // get value into a wxVariant
    virtual void GetValue( wxVariant &variant, 
                           const wxDataViewItem &item, unsigned int col ) const = 0;

    // set value, call ValueChanged() afterwards!
    virtual bool SetValue( const wxVariant &variant, 
                           const wxDataViewItem &item, unsigned int col ) = 0;

    // define hierachy
    virtual wxDataViewItem GetParent( const wxDataViewItem &item ) const = 0;
    virtual bool IsContainer( const wxDataViewItem &item ) const = 0;
    virtual wxDataViewItem GetFirstChild( const wxDataViewItem &parent ) const = 0;
    virtual wxDataViewItem GetNextSibling( const wxDataViewItem &item ) const = 0;

    // delegated notifiers
    virtual bool ItemAdded( const wxDataViewItem &parent, const wxDataViewItem &item );
    virtual bool ItemDeleted( const wxDataViewItem &parent, const wxDataViewItem &item );
    virtual bool ItemChanged( const wxDataViewItem &item );
    virtual bool ValueChanged( const wxDataViewItem &item, unsigned int col );
    virtual bool Cleared();

    // delegatd action
    virtual void Resort();

    void AddNotifier( wxDataViewModelNotifier *notifier );
    void RemoveNotifier( wxDataViewModelNotifier *notifier );
    
    // default compare function
    virtual int Compare( const wxDataViewItem &item1, const wxDataViewItem &item2, 
                         unsigned int column, bool ascending );
    virtual bool HasDefaultCompare() { return false; }

protected:
    // the user should not delete this class directly: he should use DecRef() instead!
    virtual ~wxDataViewModel() { }

    wxDataViewModelNotifiers  m_notifiers;
};

// ---------------------------------------------------------
// wxDataViewIndexListModel
// ---------------------------------------------------------

class wxDataViewIndexListModel: public wxDataViewModel
{
public:
    wxDataViewIndexListModel( unsigned int initial_size = 0 );
    ~wxDataViewIndexListModel();
    
    virtual unsigned int GetRowCount() = 0;
    
    virtual void GetValue( wxVariant &variant, 
                           unsigned int row, unsigned int col ) const = 0;

    virtual bool SetValue( const wxVariant &variant, 
                           unsigned int row, unsigned int col ) = 0;
    
    void RowPrepended();
    void RowInserted( unsigned int before );
    void RowAppended();
    void RowDeleted( unsigned int row );
    void RowChanged( unsigned int row );
    void RowValueChanged( unsigned int row, unsigned int col );
    
    // convert to/from row/wxDataViewItem
    
    unsigned int GetRow( const wxDataViewItem &item ) const;
    wxDataViewItem GetItem( unsigned int row ) const;
    
    // compare based on index
    
    virtual int Compare( const wxDataViewItem &item1, const wxDataViewItem &item2, 
                         unsigned int column, bool ascending );
    virtual bool HasDefaultCompare() { return true; }

    // implement base methods

    virtual void GetValue( wxVariant &variant, 
                           const wxDataViewItem &item, unsigned int col ) const;
    virtual bool SetValue( const wxVariant &variant, 
                           const wxDataViewItem &item, unsigned int col );
    virtual wxDataViewItem GetParent( const wxDataViewItem &item ) const;
    virtual bool IsContainer( const wxDataViewItem &item ) const;
    virtual wxDataViewItem GetFirstChild( const wxDataViewItem &parent ) const;
    virtual wxDataViewItem GetNextSibling( const wxDataViewItem &item ) const;
    
private:
    wxDataViewItemArray m_hash;
    unsigned int m_lastIndex;
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
    virtual wxControl* CreateEditorCtrl(wxWindow * WXUNUSED(parent),
                                        wxRect WXUNUSED(labelRect),
                                        const wxVariant& WXUNUSED(value))
        { return NULL; }
    virtual bool GetValueFromEditorCtrl(wxControl * WXUNUSED(editor),
                                        wxVariant& WXUNUSED(value))
        { return false; }

    virtual bool StartEditing( const wxDataViewItem &item, wxRect labelRect );
    virtual void CancelEditing();
    virtual bool FinishEditing();
    
    wxControl *GetEditorCtrl() { return m_editorCtrl; }
    
protected:
    wxString                m_variantType;
    wxDataViewColumn       *m_owner;
    wxControl              *m_editorCtrl;
    wxDataViewItem          m_item; // for m_editorCtrl

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

    virtual bool AssociateModel( wxDataViewModel *model );
    wxDataViewModel* GetModel();

    // short cuts
    wxDataViewColumn *AppendTextColumn( const wxString &label, unsigned int model_column, 
                    wxDataViewCellMode mode = wxDATAVIEW_CELL_INERT, int width = -1,
                    wxAlignment align = wxALIGN_CENTER,
                    int flags = wxDATAVIEW_COL_RESIZABLE );
    wxDataViewColumn *AppendToggleColumn( const wxString &label, unsigned int model_column,
                    wxDataViewCellMode mode = wxDATAVIEW_CELL_INERT, int width = wxDVC_TOGGLE_DEFAULT_WIDTH,
                    wxAlignment align = wxALIGN_CENTER,
                    int flags = wxDATAVIEW_COL_RESIZABLE );
    wxDataViewColumn *AppendProgressColumn( const wxString &label, unsigned int model_column, 
                    wxDataViewCellMode mode = wxDATAVIEW_CELL_INERT, int width = wxDVC_DEFAULT_WIDTH,
                    wxAlignment align = wxALIGN_CENTER,
                    int flags = wxDATAVIEW_COL_RESIZABLE );
    wxDataViewColumn *AppendDateColumn( const wxString &label, unsigned int model_column,
                    wxDataViewCellMode mode = wxDATAVIEW_CELL_ACTIVATABLE, int width = -1,
                    wxAlignment align = wxALIGN_CENTER,
                    int flags = wxDATAVIEW_COL_RESIZABLE );
    wxDataViewColumn *AppendBitmapColumn( const wxString &label, unsigned int model_column,
                    wxDataViewCellMode mode = wxDATAVIEW_CELL_INERT, int width = -1,
                    wxAlignment align = wxALIGN_CENTER,
                    int flags = wxDATAVIEW_COL_RESIZABLE );
    wxDataViewColumn *AppendTextColumn( const wxBitmap &label, unsigned int model_column,
                    wxDataViewCellMode mode = wxDATAVIEW_CELL_INERT, int width = -1,
                    wxAlignment align = wxALIGN_CENTER,
                    int flags = wxDATAVIEW_COL_RESIZABLE );
    wxDataViewColumn *AppendToggleColumn( const wxBitmap &label, unsigned int model_column,
                    wxDataViewCellMode mode = wxDATAVIEW_CELL_INERT, int width = wxDVC_TOGGLE_DEFAULT_WIDTH,
                    wxAlignment align = wxALIGN_CENTER,
                    int flags = wxDATAVIEW_COL_RESIZABLE );
    wxDataViewColumn *AppendProgressColumn( const wxBitmap &label, unsigned int model_column,
                    wxDataViewCellMode mode = wxDATAVIEW_CELL_INERT, int width = wxDVC_DEFAULT_WIDTH,
                    wxAlignment align = wxALIGN_CENTER,
                    int flags = wxDATAVIEW_COL_RESIZABLE );
    wxDataViewColumn *AppendDateColumn( const wxBitmap &label, unsigned int model_column,
                    wxDataViewCellMode mode = wxDATAVIEW_CELL_ACTIVATABLE, int width = -1,
                    wxAlignment align = wxALIGN_CENTER,
                    int flags = wxDATAVIEW_COL_RESIZABLE );
    
    wxDataViewColumn *AppendBitmapColumn( const wxBitmap &label, unsigned int model_column,
                    wxDataViewCellMode mode = wxDATAVIEW_CELL_INERT, int width = -1,
                    wxAlignment align = wxALIGN_CENTER,
                    int flags = wxDATAVIEW_COL_RESIZABLE );
    
    virtual bool AppendColumn( wxDataViewColumn *col );

    virtual unsigned int GetColumnCount() const;

    virtual bool DeleteColumn( unsigned int pos );
    virtual bool ClearColumns();
    virtual wxDataViewColumn* GetColumn( unsigned int pos ) const;

    void SetExpanderColumn( unsigned int col )
        { m_expander_column = col ; DoSetExpanderColumn(); }
    unsigned int GetExpanderColumn() const 
        { return m_expander_column; }

    void SetIndent( int indent )
        { m_indent = indent ; DoSetIndent(); }
    int GetIndent() const 
        { return m_indent; } 

    virtual wxDataViewItem GetSelection() const = 0;
    virtual int GetSelections( wxDataViewItemArray & sel ) const = 0;
    virtual void SetSelections( const wxDataViewItemArray & sel ) = 0;
    virtual void Select( const wxDataViewItem & item ) = 0;
    virtual void Unselect( const wxDataViewItem & item ) = 0;
    virtual bool IsSelected( const wxDataViewItem & item ) const = 0;

    virtual void SelectAll() = 0;
    virtual void UnselectAll() = 0;

    virtual void EnsureVisible( const wxDataViewItem & item,
                                const wxDataViewColumn *column = NULL ) = 0;
    virtual void HitTest( const wxPoint & point, wxDataViewItem &item, wxDataViewColumn* column ) const = 0;
    virtual wxRect GetItemRect( const wxDataViewItem & item, const wxDataViewColumn *column = NULL ) const = 0;

protected:
    virtual void DoSetExpanderColumn() = 0 ;
    virtual void DoSetIndent() = 0;

private:
    wxDataViewModel        *m_model;
    wxList                  m_cols;
    unsigned int m_expander_column;
    int m_indent ;
	
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
        m_item(0),
        m_col(-1),
        m_model(NULL),
        m_value(wxNullVariant),
        m_column(NULL)
        { }

    wxDataViewEvent(const wxDataViewEvent& event)
        : wxNotifyEvent(event),
        m_item(event.m_item),
        m_col(event.m_col),
        m_model(event.m_model),
        m_value(event.m_value),
        m_column(event.m_column)
        { }

    wxDataViewItem GetItem() const { return m_item; }
    void SetItem( const wxDataViewItem &item ) { m_item = item; }

    int GetColumn() const { return m_col; }
    void SetColumn( int col ) { m_col = col; }

    wxDataViewModel* GetModel() const { return m_model; }
    void SetModel( wxDataViewModel *model ) { m_model = model; }

    const wxVariant &GetValue() const { return m_value; }
    void SetValue( const wxVariant &value ) { m_value = value; }

    // for wxEVT_DATAVIEW_COLUMN_HEADER_CLICKED only
    void SetDataViewColumn( wxDataViewColumn *col ) { m_column = col; }
    wxDataViewColumn *GetDataViewColumn() const { return m_column; }

    virtual wxEvent *Clone() const { return new wxDataViewEvent(*this); }

protected:
    wxDataViewItem      m_item;
    int                 m_col;
    wxDataViewModel    *m_model;
    wxVariant           m_value;
    wxDataViewColumn   *m_column;

private:
    DECLARE_DYNAMIC_CLASS_NO_ASSIGN(wxDataViewEvent)
};

BEGIN_DECLARE_EVENT_TYPES()
    DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_ADV, wxEVT_COMMAND_DATAVIEW_ITEM_SELECTED, -1)
    DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_ADV, wxEVT_COMMAND_DATAVIEW_ITEM_DESELECTED, -1)
    DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_ADV, wxEVT_COMMAND_DATAVIEW_ITEM_ACTIVATED, -1)
    DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_ADV, wxEVT_COMMAND_DATAVIEW_ITEM_COLLAPSED, -1)
    DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_ADV, wxEVT_COMMAND_DATAVIEW_ITEM_EXPANDED, -1)
    DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_ADV, wxEVT_COMMAND_DATAVIEW_ITEM_COLLAPSING, -1)
    DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_ADV, wxEVT_COMMAND_DATAVIEW_ITEM_EXPANDING, -1)
    DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_ADV, wxEVT_COMMAND_DATAVIEW_COLUMN_HEADER_CLICK, -1)
    DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_ADV, wxEVT_COMMAND_DATAVIEW_COLUMN_HEADER_RIGHT_CLICK, -1)
    DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_ADV, wxEVT_COMMAND_DATAVIEW_COLUMN_SORTED, -1)
    // notifications from the model to the control
    DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_ADV, wxEVT_COMMAND_DATAVIEW_MODEL_ITEM_ADDED, -1)
    DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_ADV, wxEVT_COMMAND_DATAVIEW_MODEL_ITEM_DELETED, -1)
    DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_ADV, wxEVT_COMMAND_DATAVIEW_MODEL_ITEM_CHANGED, -1)
    DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_ADV, wxEVT_COMMAND_DATAVIEW_MODEL_VALUE_CHANGED, -1)
    DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_ADV, wxEVT_COMMAND_DATAVIEW_MODEL_CLEARED, -1)
END_DECLARE_EVENT_TYPES()

typedef void (wxEvtHandler::*wxDataViewEventFunction)(wxDataViewEvent&);

#define wxDataViewEventHandler(func) \
    (wxObjectEventFunction)(wxEventFunction)wxStaticCastEvent(wxDataViewEventFunction, &func)

#define wx__DECLARE_DATAVIEWEVT(evt, id, fn) \
    wx__DECLARE_EVT1(wxEVT_COMMAND_DATAVIEW_ ## evt, id, wxDataViewEventHandler(fn))

#define EVT_DATAVIEW_ITEM_SELECTED(id, fn) wx__DECLARE_DATAVIEWEVT(ITEM_SELECTED, id, fn)
#define EVT_DATAVIEW_ITEM_DESELECTED(id, fn) wx__DECLARE_DATAVIEWEVT(ITEM_DESELECTED, id, fn)
#define EVT_DATAVIEW_ITEM_ACTIVATED(id, fn) wx__DECLARE_DATAVIEWEVT(ITEM_ACTIVATED, id, fn)
#define EVT_DATAVIEW_ITEM_COLLAPSING(id, fn) wx__DECLARE_DATAVIEWEVT(ITEM_COLLAPSING, id, fn)
#define EVT_DATAVIEW_ITEM_COLLAPSED(id, fn) wx__DECLARE_DATAVIEWEVT(ITEM_COLLAPSED, id, fn)
#define EVT_DATAVIEW_ITEM_EXPANDING(id, fn) wx__DECLARE_DATAVIEWEVT(ITEM_EXPANDING, id, fn)
#define EVT_DATAVIEW_ITEM_EXPANDED(id, fn) wx__DECLARE_DATAVIEWEVT(ITEM_EXPANDED, id, fn)
#define EVT_DATAVIEW_COLUMN_HEADER_CLICK(id, fn) wx__DECLARE_DATAVIEWEVT(COLUMN_HEADER_CLICK, id, fn)
#define EVT_DATAVIEW_COLUMN_HEADER_RIGHT_CLICKED(id, fn) wx__DECLARE_DATAVIEWEVT(COLUMN_HEADER_RIGHT_CLICK, id, fn)
#define EVT_DATAVIEW_COLUMN_SORTED(id, fn) wx__DECLARE_DATAVIEWEVT(COLUMN_SORTED, id, fn)

#define EVT_DATAVIEW_MODEL_ITEM_ADDED(id, fn) wx__DECLARE_DATAVIEWEVT(MODEL_ITEM_ADDED, id, fn)
#define EVT_DATAVIEW_MODEL_ITEM_DELETED(id, fn) wx__DECLARE_DATAVIEWEVT(MODEL_ITEM_DELETED, id, fn)
#define EVT_DATAVIEW_MODEL_ITEM_CHANGED(id, fn) wx__DECLARE_DATAVIEWEVT(MODEL_ITEM_CHANGED, id, fn)
#define EVT_DATAVIEW_MODEL_VALUE_CHANGED(id, fn) wx__DECLARE_DATAVIEWEVT(MODEL_VALUE_CHANGED, id, fn)
#define EVT_DATAVIEW_MODEL_CLEARED(id, fn) wx__DECLARE_DATAVIEWEVT(MODEL_CLEARED, id, fn)


#if defined(wxUSE_GENERICDATAVIEWCTRL)
    #include "wx/generic/dataview.h"
#elif defined(__WXGTK20__)
    #include "wx/gtk/dataview.h"
#elif defined(__WXMAC__)
    #include "wx/mac/dataview.h"
#else
    #include "wx/generic/dataview.h"
#endif

#endif // wxUSE_DATAVIEWCTRL

#endif
    // _WX_DATAVIEW_H_BASE_
