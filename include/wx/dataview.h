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
#include "wx/headercol.h"
#include "wx/variant.h"
#include "wx/dynarray.h"
#include "wx/icon.h"
#include "wx/imaglist.h"
#include "wx/weakref.h"

#if !(defined(__WXGTK20__) || defined(__WXMAC__)) || defined(__WXUNIVERSAL__)
// #if !(defined(__WXMAC__)) || defined(__WXUNIVERSAL__)
    #define wxHAS_GENERIC_DATAVIEWCTRL
#endif

class WXDLLIMPEXP_FWD_CORE wxDataFormat;

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

extern WXDLLIMPEXP_DATA_ADV(const char) wxDataViewCtrlNameStr[];

// the default width of new (text) columns:
#define wxDVC_DEFAULT_WIDTH             80

// the default width of new toggle columns:
#define wxDVC_TOGGLE_DEFAULT_WIDTH      30

// the default minimal width of the columns:
#define wxDVC_DEFAULT_MINWIDTH          30

// The default alignment of wxDataViewRenderers is to take
// the alignment from the column it owns.
#define wxDVR_DEFAULT_ALIGNMENT         -1


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

#ifdef __WXDEBUG__
    void Print( const wxString &text ) const;
#endif

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
    wxDataViewModelNotifier() { m_owner = NULL; }
    virtual ~wxDataViewModelNotifier() { m_owner = NULL; }

    virtual bool ItemAdded( const wxDataViewItem &parent, const wxDataViewItem &item ) = 0;
    virtual bool ItemDeleted( const wxDataViewItem &parent, const wxDataViewItem &item ) = 0;
    virtual bool ItemChanged( const wxDataViewItem &item ) = 0;
    virtual bool ItemsAdded( const wxDataViewItem &parent, const wxDataViewItemArray &items );
    virtual bool ItemsDeleted( const wxDataViewItem &parent, const wxDataViewItemArray &items );
    virtual bool ItemsChanged( const wxDataViewItemArray &items );
    virtual bool ValueChanged( const wxDataViewItem &item, unsigned int col ) = 0;
    virtual bool Cleared() = 0;

    virtual void Resort() = 0;

    void SetOwner( wxDataViewModel *owner ) { m_owner = owner; }
    wxDataViewModel *GetOwner() const       { return m_owner; }

private:
    wxDataViewModel *m_owner;
};



// ----------------------------------------------------------------------------
// wxDataViewItemAttr: a structure containing the visual attributes of an item
// ----------------------------------------------------------------------------

// TODO: this should be renamed to wxItemAttr or something general like this

class WXDLLIMPEXP_ADV wxDataViewItemAttr
{
public:
    // ctors
    wxDataViewItemAttr()
    {
        m_bold = false;
        m_italic = false;
    }

    // setters
    void SetColour(const wxColour& colour) { m_colour = colour; }
    void SetBold( bool set ) { m_bold = set; }
    void SetItalic( bool set ) { m_italic = set; }

    // accessors
    bool HasColour() const { return m_colour.Ok(); }
    const wxColour& GetColour() const { return m_colour; }

    bool GetBold() const { return m_bold; }
    bool GetItalic() const { return m_italic; }

private:
    wxColour m_colour;
    bool     m_bold;
    bool     m_italic;
};


// ---------------------------------------------------------
// wxDataViewModel
// ---------------------------------------------------------

WX_DECLARE_LIST_WITH_DECL(wxDataViewModelNotifier, wxDataViewModelNotifiers,
                          class WXDLLIMPEXP_ADV);

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

    // Get text attribute, return false of default attributes should be used
    virtual bool GetAttr( const wxDataViewItem &WXUNUSED(item), unsigned int WXUNUSED(col), wxDataViewItemAttr &WXUNUSED(attr) )
        { return false; }

    // define hierachy
    virtual wxDataViewItem GetParent( const wxDataViewItem &item ) const = 0;
    virtual bool IsContainer( const wxDataViewItem &item ) const = 0;
    // Is the container just a header or an item with all columns
    virtual bool HasContainerColumns(const wxDataViewItem& WXUNUSED(item)) const
        { return false; }
    virtual unsigned int GetChildren( const wxDataViewItem &item, wxDataViewItemArray &children ) const = 0;

    // define DnD capabilities
    virtual bool IsDraggable( const wxDataViewItem &WXUNUSED(item) )
        { return false; }
    virtual size_t GetDragDataSize( const wxDataViewItem &WXUNUSED(item), const wxDataFormat &WXUNUSED(format) )
        { return 0; }
    virtual bool GetDragData( const wxDataViewItem &WXUNUSED(item), const wxDataFormat &WXUNUSED(format),
                              void* WXUNUSED(data), size_t WXUNUSED(size) )
        { return FALSE; }

    // delegated notifiers
    virtual bool ItemAdded( const wxDataViewItem &parent, const wxDataViewItem &item );
    virtual bool ItemsAdded( const wxDataViewItem &parent, const wxDataViewItemArray &items );
    virtual bool ItemDeleted( const wxDataViewItem &parent, const wxDataViewItem &item );
    virtual bool ItemsDeleted( const wxDataViewItem &parent, const wxDataViewItemArray &items );
    virtual bool ItemChanged( const wxDataViewItem &item );
    virtual bool ItemsChanged( const wxDataViewItemArray &items );
    virtual bool ValueChanged( const wxDataViewItem &item, unsigned int col );
    virtual bool Cleared();

    // delegatd action
    virtual void Resort();

    void AddNotifier( wxDataViewModelNotifier *notifier );
    void RemoveNotifier( wxDataViewModelNotifier *notifier );

    // default compare function
    virtual int Compare( const wxDataViewItem &item1, const wxDataViewItem &item2,
                         unsigned int column, bool ascending );
    virtual bool HasDefaultCompare() const { return false; }

    // internal
    virtual bool IsVirtualListModel() const { return false; }

protected:
    // the user should not delete this class directly: he should use DecRef() instead!
    virtual ~wxDataViewModel() { }

    wxDataViewModelNotifiers  m_notifiers;
};

// ---------------------------------------------------------
// wxDataViewIndexListModel
// ---------------------------------------------------------

class WXDLLIMPEXP_ADV wxDataViewIndexListModel: public wxDataViewModel
{
public:
    wxDataViewIndexListModel( unsigned int initial_size = 0 );
    ~wxDataViewIndexListModel();

    virtual void GetValue( wxVariant &variant,
                           unsigned int row, unsigned int col ) const = 0;

    virtual bool SetValue( const wxVariant &variant,
                           unsigned int row, unsigned int col ) = 0;

    virtual bool GetAttr( unsigned int WXUNUSED(row), unsigned int WXUNUSED(col), wxDataViewItemAttr &WXUNUSED(attr) )
        { return false; }

    void RowPrepended();
    void RowInserted( unsigned int before );
    void RowAppended();
    void RowDeleted( unsigned int row );
    void RowsDeleted( const wxArrayInt &rows );
    void RowChanged( unsigned int row );
    void RowValueChanged( unsigned int row, unsigned int col );
    void Reset( unsigned int new_size );

    // convert to/from row/wxDataViewItem

    unsigned int GetRow( const wxDataViewItem &item ) const;
    wxDataViewItem GetItem( unsigned int row ) const;

    // compare based on index

    virtual int Compare( const wxDataViewItem &item1, const wxDataViewItem &item2,
                         unsigned int column, bool ascending );
    virtual bool HasDefaultCompare() const;

    // implement base methods

    virtual void GetValue( wxVariant &variant,
                           const wxDataViewItem &item, unsigned int col ) const;
    virtual bool SetValue( const wxVariant &variant,
                           const wxDataViewItem &item, unsigned int col );
    virtual bool GetAttr( const wxDataViewItem &item, unsigned int col, wxDataViewItemAttr &attr );
    virtual wxDataViewItem GetParent( const wxDataViewItem &item ) const;
    virtual bool IsContainer( const wxDataViewItem &item ) const;
    virtual unsigned int GetChildren( const wxDataViewItem &item, wxDataViewItemArray &children ) const;

    // internal
    virtual bool IsVirtualListModel() const { return false; }
    unsigned int GetLastIndex() const { return m_lastIndex; }

private:
    wxDataViewItemArray m_hash;
    unsigned int m_lastIndex;
    bool m_ordered;
};

// ---------------------------------------------------------
// wxDataViewVirtualListModel
// ---------------------------------------------------------

#ifdef __WXMAC__
// better than nothing
typedef wxDataViewIndexListModel wxDataViewVirtualListModel;
#else

class WXDLLIMPEXP_ADV wxDataViewVirtualListModel: public wxDataViewModel
{
public:
    wxDataViewVirtualListModel( unsigned int initial_size = 0 );
    ~wxDataViewVirtualListModel();

    virtual void GetValue( wxVariant &variant,
                           unsigned int row, unsigned int col ) const = 0;

    virtual bool SetValue( const wxVariant &variant,
                           unsigned int row, unsigned int col ) = 0;

    virtual bool GetAttr( unsigned int WXUNUSED(row), unsigned int WXUNUSED(col), wxDataViewItemAttr &WXUNUSED(attr) )
        { return false; }

    void RowPrepended();
    void RowInserted( unsigned int before );
    void RowAppended();
    void RowDeleted( unsigned int row );
    void RowsDeleted( const wxArrayInt &rows );
    void RowChanged( unsigned int row );
    void RowValueChanged( unsigned int row, unsigned int col );
    void Reset( unsigned int new_size );

    // convert to/from row/wxDataViewItem

    unsigned int GetRow( const wxDataViewItem &item ) const;
    wxDataViewItem GetItem( unsigned int row ) const;

    // compare based on index

    virtual int Compare( const wxDataViewItem &item1, const wxDataViewItem &item2,
                         unsigned int column, bool ascending );
    virtual bool HasDefaultCompare() const;

    // implement base methods

    virtual void GetValue( wxVariant &variant,
                           const wxDataViewItem &item, unsigned int col ) const;
    virtual bool SetValue( const wxVariant &variant,
                           const wxDataViewItem &item, unsigned int col );
    virtual bool GetAttr( const wxDataViewItem &item, unsigned int col, wxDataViewItemAttr &attr );
    virtual wxDataViewItem GetParent( const wxDataViewItem &item ) const;
    virtual bool IsContainer( const wxDataViewItem &item ) const;
    virtual unsigned int GetChildren( const wxDataViewItem &item, wxDataViewItemArray &children ) const;

    // internal
    virtual bool IsVirtualListModel() const { return true; }
    unsigned int GetLastIndex() const { return m_lastIndex; }

private:
    wxDataViewItemArray m_hash;
    unsigned int m_lastIndex;
    bool m_ordered;
};
#endif

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
    void OnTextEnter( wxCommandEvent &event );
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
    ~wxDataViewRendererBase();

    virtual bool Validate( wxVariant& WXUNUSED(value) )
        { return true; }

    void SetOwner( wxDataViewColumn *owner )    { m_owner = owner; }
    wxDataViewColumn* GetOwner() const          { return m_owner; }

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
    wxWeakRef<wxControl>    m_editorCtrl;
    wxDataViewItem          m_item; // for m_editorCtrl

    // internal utility:
    const wxDataViewCtrl* GetView() const;

protected:
    DECLARE_DYNAMIC_CLASS_NO_COPY(wxDataViewRendererBase)
};

//-----------------------------------------------------------------------------
// wxDataViewIconText
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_ADV wxDataViewIconText: public wxObject
{
public:
    wxDataViewIconText( const wxString &text = wxEmptyString, const wxIcon& icon = wxNullIcon )
        : m_text(text), m_icon(icon)
    { }
    wxDataViewIconText( const wxDataViewIconText &other )
        : wxObject()
    { m_icon = other.m_icon; m_text = other.m_text; }

    void SetText( const wxString &text ) { m_text = text; }
    wxString GetText() const             { return m_text; }
    void SetIcon( const wxIcon &icon )   { m_icon = icon; }
    const wxIcon &GetIcon() const        { return m_icon; }

private:
    wxString    m_text;
    wxIcon      m_icon;

private:
    DECLARE_DYNAMIC_CLASS(wxDataViewIconText)
};

bool operator == (const wxDataViewIconText &one, const wxDataViewIconText &two);

DECLARE_VARIANT_OBJECT_EXPORTED(wxDataViewIconText, WXDLLIMPEXP_ADV)

// ---------------------------------------------------------
// wxDataViewColumnBase
// ---------------------------------------------------------

// for compatibility only, do not use
enum wxDataViewColumnFlags
{
    wxDATAVIEW_COL_RESIZABLE     = wxCOL_RESIZABLE,
    wxDATAVIEW_COL_SORTABLE      = wxCOL_SORTABLE,
    wxDATAVIEW_COL_REORDERABLE   = wxCOL_REORDERABLE,
    wxDATAVIEW_COL_HIDDEN        = wxCOL_HIDDEN
};

class WXDLLIMPEXP_ADV wxDataViewColumnBase : public wxSettableHeaderColumn
{
public:
    // ctor for the text columns: takes ownership of renderer
    wxDataViewColumnBase(wxDataViewRenderer *renderer,
                         unsigned int model_column)
    {
        Init(renderer, model_column);
    }

    // ctor for the bitmap columns
    wxDataViewColumnBase(const wxBitmap& bitmap,
                         wxDataViewRenderer *renderer,
                         unsigned int model_column)
        : m_bitmap(bitmap)
    {
        Init(renderer, model_column);
    }

    virtual ~wxDataViewColumnBase();

    // setters:
    virtual void SetOwner( wxDataViewCtrl *owner )
        { m_owner = owner; }

    // getters:
    unsigned int GetModelColumn() const { return static_cast<unsigned int>(m_model_column); }
    wxDataViewCtrl *GetOwner() const        { return m_owner; }
    wxDataViewRenderer* GetRenderer() const { return m_renderer; }

    // implement some of base class pure virtuals (the rest is port-dependent
    // and done differently in generic and native versions)
    virtual void SetBitmap( const wxBitmap& bitmap ) { m_bitmap = bitmap; }
    virtual wxBitmap GetBitmap() const { return m_bitmap; }

protected:
    wxDataViewRenderer      *m_renderer;
    int                      m_model_column;
    wxBitmap                 m_bitmap;
    wxDataViewCtrl          *m_owner;

private:
    // common part of all ctors
    void Init(wxDataViewRenderer *renderer, unsigned int model_column);
};

// ---------------------------------------------------------
// wxDataViewCtrlBase
// ---------------------------------------------------------

#define wxDV_SINGLE                  0x0000     // for convenience
#define wxDV_MULTIPLE                0x0001     // can select multiple items

#define wxDV_NO_HEADER               0x0002     // column titles not visible
#define wxDV_HORIZ_RULES             0x0004     // light horizontal rules between rows
#define wxDV_VERT_RULES              0x0008     // light vertical rules between columns

#define wxDV_ROW_LINES               0x0010     // alternating colour in rows
#define wxDV_VARIABLE_LINE_HEIGHT    0x0020     // variable line height

class WXDLLIMPEXP_ADV wxDataViewCtrlBase: public wxControl
{
public:
    wxDataViewCtrlBase();
    virtual ~wxDataViewCtrlBase();

    virtual bool AssociateModel( wxDataViewModel *model );
    wxDataViewModel* GetModel();
    const wxDataViewModel* GetModel() const;

    // short cuts
    wxDataViewColumn *PrependTextColumn( const wxString &label, unsigned int model_column,
                    wxDataViewCellMode mode = wxDATAVIEW_CELL_INERT, int width = -1,
                    wxAlignment align = wxALIGN_NOT,
                    int flags = wxDATAVIEW_COL_RESIZABLE );
    wxDataViewColumn *PrependIconTextColumn( const wxString &label, unsigned int model_column,
                    wxDataViewCellMode mode = wxDATAVIEW_CELL_INERT, int width = -1,
                    wxAlignment align = wxALIGN_NOT,
                    int flags = wxDATAVIEW_COL_RESIZABLE );
    wxDataViewColumn *PrependToggleColumn( const wxString &label, unsigned int model_column,
                    wxDataViewCellMode mode = wxDATAVIEW_CELL_INERT, int width = wxDVC_TOGGLE_DEFAULT_WIDTH,
                    wxAlignment align = wxALIGN_CENTER,
                    int flags = wxDATAVIEW_COL_RESIZABLE );
    wxDataViewColumn *PrependProgressColumn( const wxString &label, unsigned int model_column,
                    wxDataViewCellMode mode = wxDATAVIEW_CELL_INERT, int width = wxDVC_DEFAULT_WIDTH,
                    wxAlignment align = wxALIGN_CENTER,
                    int flags = wxDATAVIEW_COL_RESIZABLE );
    wxDataViewColumn *PrependDateColumn( const wxString &label, unsigned int model_column,
                    wxDataViewCellMode mode = wxDATAVIEW_CELL_ACTIVATABLE, int width = -1,
                    wxAlignment align = wxALIGN_NOT,
                    int flags = wxDATAVIEW_COL_RESIZABLE );
    wxDataViewColumn *PrependBitmapColumn( const wxString &label, unsigned int model_column,
                    wxDataViewCellMode mode = wxDATAVIEW_CELL_INERT, int width = -1,
                    wxAlignment align = wxALIGN_CENTER,
                    int flags = wxDATAVIEW_COL_RESIZABLE );
    wxDataViewColumn *PrependTextColumn( const wxBitmap &label, unsigned int model_column,
                    wxDataViewCellMode mode = wxDATAVIEW_CELL_INERT, int width = -1,
                    wxAlignment align = wxALIGN_NOT,
                    int flags = wxDATAVIEW_COL_RESIZABLE );
    wxDataViewColumn *PrependIconTextColumn( const wxBitmap &label, unsigned int model_column,
                    wxDataViewCellMode mode = wxDATAVIEW_CELL_INERT, int width = -1,
                    wxAlignment align = wxALIGN_NOT,
                    int flags = wxDATAVIEW_COL_RESIZABLE );
    wxDataViewColumn *PrependToggleColumn( const wxBitmap &label, unsigned int model_column,
                    wxDataViewCellMode mode = wxDATAVIEW_CELL_INERT, int width = wxDVC_TOGGLE_DEFAULT_WIDTH,
                    wxAlignment align = wxALIGN_CENTER,
                    int flags = wxDATAVIEW_COL_RESIZABLE );
    wxDataViewColumn *PrependProgressColumn( const wxBitmap &label, unsigned int model_column,
                    wxDataViewCellMode mode = wxDATAVIEW_CELL_INERT, int width = wxDVC_DEFAULT_WIDTH,
                    wxAlignment align = wxALIGN_CENTER,
                    int flags = wxDATAVIEW_COL_RESIZABLE );
    wxDataViewColumn *PrependDateColumn( const wxBitmap &label, unsigned int model_column,
                    wxDataViewCellMode mode = wxDATAVIEW_CELL_ACTIVATABLE, int width = -1,
                    wxAlignment align = wxALIGN_NOT,
                    int flags = wxDATAVIEW_COL_RESIZABLE );
    wxDataViewColumn *PrependBitmapColumn( const wxBitmap &label, unsigned int model_column,
                    wxDataViewCellMode mode = wxDATAVIEW_CELL_INERT, int width = -1,
                    wxAlignment align = wxALIGN_CENTER,
                    int flags = wxDATAVIEW_COL_RESIZABLE );

    wxDataViewColumn *AppendTextColumn( const wxString &label, unsigned int model_column,
                    wxDataViewCellMode mode = wxDATAVIEW_CELL_INERT, int width = -1,
                    wxAlignment align = wxALIGN_NOT,
                    int flags = wxDATAVIEW_COL_RESIZABLE );
    wxDataViewColumn *AppendIconTextColumn( const wxString &label, unsigned int model_column,
                    wxDataViewCellMode mode = wxDATAVIEW_CELL_INERT, int width = -1,
                    wxAlignment align = wxALIGN_NOT,
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
                    wxAlignment align = wxALIGN_NOT,
                    int flags = wxDATAVIEW_COL_RESIZABLE );
    wxDataViewColumn *AppendBitmapColumn( const wxString &label, unsigned int model_column,
                    wxDataViewCellMode mode = wxDATAVIEW_CELL_INERT, int width = -1,
                    wxAlignment align = wxALIGN_CENTER,
                    int flags = wxDATAVIEW_COL_RESIZABLE );
    wxDataViewColumn *AppendTextColumn( const wxBitmap &label, unsigned int model_column,
                    wxDataViewCellMode mode = wxDATAVIEW_CELL_INERT, int width = -1,
                    wxAlignment align = wxALIGN_NOT,
                    int flags = wxDATAVIEW_COL_RESIZABLE );
    wxDataViewColumn *AppendIconTextColumn( const wxBitmap &label, unsigned int model_column,
                    wxDataViewCellMode mode = wxDATAVIEW_CELL_INERT, int width = -1,
                    wxAlignment align = wxALIGN_NOT,
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
                    wxAlignment align = wxALIGN_NOT,
                    int flags = wxDATAVIEW_COL_RESIZABLE );
    wxDataViewColumn *AppendBitmapColumn( const wxBitmap &label, unsigned int model_column,
                    wxDataViewCellMode mode = wxDATAVIEW_CELL_INERT, int width = -1,
                    wxAlignment align = wxALIGN_CENTER,
                    int flags = wxDATAVIEW_COL_RESIZABLE );


    virtual bool PrependColumn( wxDataViewColumn *col );
    virtual bool InsertColumn( unsigned int pos, wxDataViewColumn *col );
    virtual bool AppendColumn( wxDataViewColumn *col );

    virtual unsigned int GetColumnCount() const = 0;
    virtual wxDataViewColumn* GetColumn( unsigned int pos ) const = 0;
    virtual int GetColumnPosition( const wxDataViewColumn *column ) const = 0;

    virtual bool DeleteColumn( wxDataViewColumn *column ) = 0;
    virtual bool ClearColumns() = 0;

    void SetExpanderColumn( wxDataViewColumn *col )
        { m_expander_column = col ; DoSetExpanderColumn(); }
    wxDataViewColumn *GetExpanderColumn() const
        { return m_expander_column; }

    virtual wxDataViewColumn *GetSortingColumn() const = 0;

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

    virtual void Expand( const wxDataViewItem & item ) = 0;
    virtual void Collapse( const wxDataViewItem & item ) = 0;
    virtual bool IsExpanded( const wxDataViewItem & item ) const = 0;

    virtual void EnsureVisible( const wxDataViewItem & item,
                                const wxDataViewColumn *column = NULL );
    virtual void HitTest( const wxPoint & point, wxDataViewItem &item, wxDataViewColumn* &column ) const = 0;
    virtual wxRect GetItemRect( const wxDataViewItem & item, const wxDataViewColumn *column = NULL ) const = 0;

protected:
    virtual void DoSetExpanderColumn() = 0 ;
    virtual void DoSetIndent() = 0;

private:
    wxDataViewModel        *m_model;
    wxDataViewColumn       *m_expander_column;
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
        m_column(NULL),
        m_pos(-1,-1)
        { }

    wxDataViewEvent(const wxDataViewEvent& event)
        : wxNotifyEvent(event),
        m_item(event.m_item),
        m_col(event.m_col),
        m_model(event.m_model),
        m_value(event.m_value),
        m_column(event.m_column),
        m_pos(m_pos)
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

    // for wxEVT_DATAVIEW_CONTEXT_MENU only
    wxPoint GetPosition() const { return m_pos; }
    void SetPosition( int x, int y ) { m_pos.x = x; m_pos.y = y; }

    virtual wxEvent *Clone() const { return new wxDataViewEvent(*this); }

protected:
    wxDataViewItem      m_item;
    int                 m_col;
    wxDataViewModel    *m_model;
    wxVariant           m_value;
    wxDataViewColumn   *m_column;
    wxPoint             m_pos;

private:
    DECLARE_DYNAMIC_CLASS_NO_ASSIGN(wxDataViewEvent)
};

extern WXDLLIMPEXP_ADV const wxEventType wxEVT_COMMAND_DATAVIEW_SELECTION_CHANGED;

extern WXDLLIMPEXP_ADV const wxEventType wxEVT_COMMAND_DATAVIEW_ITEM_ACTIVATED;
extern WXDLLIMPEXP_ADV const wxEventType wxEVT_COMMAND_DATAVIEW_ITEM_COLLAPSED;
extern WXDLLIMPEXP_ADV const wxEventType wxEVT_COMMAND_DATAVIEW_ITEM_EXPANDED;
extern WXDLLIMPEXP_ADV const wxEventType wxEVT_COMMAND_DATAVIEW_ITEM_COLLAPSING;
extern WXDLLIMPEXP_ADV const wxEventType wxEVT_COMMAND_DATAVIEW_ITEM_EXPANDING;
extern WXDLLIMPEXP_ADV const wxEventType wxEVT_COMMAND_DATAVIEW_ITEM_EDITING_STARTED;
extern WXDLLIMPEXP_ADV const wxEventType wxEVT_COMMAND_DATAVIEW_ITEM_EDITING_DONE;
extern WXDLLIMPEXP_ADV const wxEventType wxEVT_COMMAND_DATAVIEW_ITEM_VALUE_CHANGED;

extern WXDLLIMPEXP_ADV const wxEventType wxEVT_COMMAND_DATAVIEW_ITEM_CONTEXT_MENU;

extern WXDLLIMPEXP_ADV const wxEventType wxEVT_COMMAND_DATAVIEW_COLUMN_HEADER_CLICK;
extern WXDLLIMPEXP_ADV const wxEventType wxEVT_COMMAND_DATAVIEW_COLUMN_HEADER_RIGHT_CLICK;
extern WXDLLIMPEXP_ADV const wxEventType wxEVT_COMMAND_DATAVIEW_COLUMN_SORTED;
extern WXDLLIMPEXP_ADV const wxEventType wxEVT_COMMAND_DATAVIEW_COLUMN_REORDERED;

typedef void (wxEvtHandler::*wxDataViewEventFunction)(wxDataViewEvent&);

#define wxDataViewEventHandler(func) \
    (wxObjectEventFunction)(wxEventFunction)wxStaticCastEvent(wxDataViewEventFunction, &func)

#define wx__DECLARE_DATAVIEWEVT(evt, id, fn) \
    wx__DECLARE_EVT1(wxEVT_COMMAND_DATAVIEW_ ## evt, id, wxDataViewEventHandler(fn))

#define EVT_DATAVIEW_SELECTION_CHANGED(id, fn) wx__DECLARE_DATAVIEWEVT(SELECTION_CHANGED, id, fn)

#define EVT_DATAVIEW_ITEM_ACTIVATED(id, fn) wx__DECLARE_DATAVIEWEVT(ITEM_ACTIVATED, id, fn)
#define EVT_DATAVIEW_ITEM_COLLAPSING(id, fn) wx__DECLARE_DATAVIEWEVT(ITEM_COLLAPSING, id, fn)
#define EVT_DATAVIEW_ITEM_COLLAPSED(id, fn) wx__DECLARE_DATAVIEWEVT(ITEM_COLLAPSED, id, fn)
#define EVT_DATAVIEW_ITEM_EXPANDING(id, fn) wx__DECLARE_DATAVIEWEVT(ITEM_EXPANDING, id, fn)
#define EVT_DATAVIEW_ITEM_EXPANDED(id, fn) wx__DECLARE_DATAVIEWEVT(ITEM_EXPANDED, id, fn)
#define EVT_DATAVIEW_ITEM_EDITING_STARTED(id, fn) wx__DECLARE_DATAVIEWEVT(ITEM_EDITING_STARTED, id, fn)
#define EVT_DATAVIEW_ITEM_EDITING_DONE(id, fn) wx__DECLARE_DATAVIEWEVT(ITEM_EDITING_DONE, id, fn)
#define EVT_DATAVIEW_ITEM_VALUE_CHANGED(id, fn) wx__DECLARE_DATAVIEWEVT(ITEM_VALUE_CHANGED, id, fn)

#define EVT_DATAVIEW_ITEM_CONTEXT_MENU(id, fn) wx__DECLARE_DATAVIEWEVT(ITEM_CONTEXT_MENU, id, fn)

#define EVT_DATAVIEW_COLUMN_HEADER_CLICK(id, fn) wx__DECLARE_DATAVIEWEVT(COLUMN_HEADER_CLICK, id, fn)
#define EVT_DATAVIEW_COLUMN_HEADER_RIGHT_CLICKED(id, fn) wx__DECLARE_DATAVIEWEVT(COLUMN_HEADER_RIGHT_CLICK, id, fn)
#define EVT_DATAVIEW_COLUMN_SORTED(id, fn) wx__DECLARE_DATAVIEWEVT(COLUMN_SORTED, id, fn)
#define EVT_DATAVIEW_COLUMN_REORDERED(id, fn) wx__DECLARE_DATAVIEWEVT(COLUMN_REORDERED, id, fn)


#ifdef wxHAS_GENERIC_DATAVIEWCTRL
    // this symbol doesn't follow the convention for wxUSE_XXX symbols which
    // are normally always defined as either 0 or 1, so its use is deprecated
    // and it only exists for backwards compatibility, don't use it any more
    // and use wxHAS_GENERIC_DATAVIEWCTRL instead
    #define wxUSE_GENERICDATAVIEWCTRL

    #include "wx/generic/dataview.h"
#elif defined(__WXGTK20__)
    #include "wx/gtk/dataview.h"
#elif defined(__WXMAC__)
    #include "wx/osx/dataview.h"
#else
    #error "unknown native wxDataViewCtrl implementation"
#endif

// -------------------------------------
// wxDataViewSpinRenderer
// -------------------------------------

class WXDLLIMPEXP_ADV wxDataViewSpinRenderer: public wxDataViewCustomRenderer
{
public:
    wxDataViewSpinRenderer( int min, int max,
                            wxDataViewCellMode mode = wxDATAVIEW_CELL_EDITABLE,
                            int alignment = wxDVR_DEFAULT_ALIGNMENT );
    virtual bool HasEditorCtrl() { return true; }
    virtual wxControl* CreateEditorCtrl( wxWindow *parent, wxRect labelRect, const wxVariant &value );
    virtual bool GetValueFromEditorCtrl( wxControl* editor, wxVariant &value );
    virtual bool Render( wxRect rect, wxDC *dc, int state );
    virtual wxSize GetSize() const;
    virtual bool SetValue( const wxVariant &value );
    virtual bool GetValue( wxVariant &value ) const;

private:
    long    m_data;
    long    m_min,m_max;
};

#if defined(wxHAS_GENERIC_DATAVIEWCTRL) || defined(__WXMAC__)

// -------------------------------------
// wxDataViewChoiceRenderer
// -------------------------------------

class WXDLLIMPEXP_ADV wxDataViewChoiceRenderer: public wxDataViewCustomRenderer
{
public:
    wxDataViewChoiceRenderer( const wxArrayString &choices,
                            wxDataViewCellMode mode = wxDATAVIEW_CELL_EDITABLE,
                            int alignment = wxDVR_DEFAULT_ALIGNMENT );
    virtual bool HasEditorCtrl() { return true; }
    virtual wxControl* CreateEditorCtrl( wxWindow *parent, wxRect labelRect, const wxVariant &value );
    virtual bool GetValueFromEditorCtrl( wxControl* editor, wxVariant &value );
    virtual bool Render( wxRect rect, wxDC *dc, int state );
    virtual wxSize GetSize() const;
    virtual bool SetValue( const wxVariant &value );
    virtual bool GetValue( wxVariant &value ) const;

private:
    wxArrayString m_choices;
    wxString      m_data;
};

#endif

//-----------------------------------------------------------------------------
// wxDataViewTreeStore
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_ADV wxDataViewTreeStoreNode
{
public:
    wxDataViewTreeStoreNode( wxDataViewTreeStoreNode *parent,
        const wxString &text, const wxIcon &icon = wxNullIcon, wxClientData *data = NULL );
    virtual ~wxDataViewTreeStoreNode();

    void SetText( const wxString &text )
        { m_text = text; }
    wxString GetText() const
        { return m_text; }
    void SetIcon( const wxIcon &icon )
        { m_icon = icon; }
    const wxIcon &GetIcon() const
        { return m_icon; }
    void SetData( wxClientData *data )
        { if (m_data) delete m_data; m_data = data; }
    wxClientData *GetData() const
        { return m_data; }

    wxDataViewItem GetItem() const
        { return wxDataViewItem( (void*) this ); }

    virtual bool IsContainer()
        { return false; }

    wxDataViewTreeStoreNode *GetParent()
        { return m_parent; }

private:
    wxDataViewTreeStoreNode  *m_parent;
    wxString                  m_text;
    wxIcon                    m_icon;
    wxClientData             *m_data;
};

WX_DECLARE_LIST_WITH_DECL(wxDataViewTreeStoreNode, wxDataViewTreeStoreNodeList,
                          class WXDLLIMPEXP_ADV);

class WXDLLIMPEXP_ADV wxDataViewTreeStoreContainerNode: public wxDataViewTreeStoreNode
{
public:
    wxDataViewTreeStoreContainerNode( wxDataViewTreeStoreNode *parent,
        const wxString &text, const wxIcon &icon = wxNullIcon, const wxIcon &expanded = wxNullIcon,
        wxClientData *data = NULL );
    virtual ~wxDataViewTreeStoreContainerNode();

    const wxDataViewTreeStoreNodeList &GetChildren() const
        { return m_children; }
    wxDataViewTreeStoreNodeList &GetChildren()
        { return m_children; }

    void SetExpandedIcon( const wxIcon &icon )
        { m_iconExpanded = icon; }
    const wxIcon &GetExpandedIcon() const
        { return m_iconExpanded; }

    void SetExpanded( bool expanded = true )
        { m_isExpanded = expanded; }
    bool IsExpanded() const
        { return m_isExpanded; }

    virtual bool IsContainer()
        { return true; }

private:
    wxDataViewTreeStoreNodeList  m_children;
    wxIcon                       m_iconExpanded;
    bool                         m_isExpanded;
};

//-----------------------------------------------------------------------------

class WXDLLIMPEXP_ADV wxDataViewTreeStore: public wxDataViewModel
{
public:
    wxDataViewTreeStore();
    ~wxDataViewTreeStore();

    wxDataViewItem AppendItem( const wxDataViewItem& parent,
        const wxString &text, const wxIcon &icon = wxNullIcon, wxClientData *data = NULL );
    wxDataViewItem PrependItem( const wxDataViewItem& parent,
        const wxString &text, const wxIcon &icon = wxNullIcon, wxClientData *data = NULL );
    wxDataViewItem InsertItem( const wxDataViewItem& parent, const wxDataViewItem& previous,
        const wxString &text, const wxIcon &icon = wxNullIcon, wxClientData *data = NULL );

    wxDataViewItem PrependContainer( const wxDataViewItem& parent,
        const wxString &text, const wxIcon &icon = wxNullIcon, const wxIcon &expanded = wxNullIcon,
        wxClientData *data = NULL );
    wxDataViewItem AppendContainer( const wxDataViewItem& parent,
        const wxString &text, const wxIcon &icon = wxNullIcon, const wxIcon &expanded = wxNullIcon,
        wxClientData *data = NULL );
    wxDataViewItem InsertContainer( const wxDataViewItem& parent, const wxDataViewItem& previous,
        const wxString &text, const wxIcon &icon = wxNullIcon, const wxIcon &expanded = wxNullIcon,
        wxClientData *data = NULL );

    wxDataViewItem GetNthChild( const wxDataViewItem& parent, unsigned int pos ) const;
    int GetChildCount( const wxDataViewItem& parent ) const;

    void SetItemText( const wxDataViewItem& item, const wxString &text );
    wxString GetItemText( const wxDataViewItem& item ) const;
    void SetItemIcon( const wxDataViewItem& item, const wxIcon &icon );
    const wxIcon &GetItemIcon( const wxDataViewItem& item ) const;
    void SetItemExpandedIcon( const wxDataViewItem& item, const wxIcon &icon );
    const wxIcon &GetItemExpandedIcon( const wxDataViewItem& item ) const;
    void SetItemData( const wxDataViewItem& item, wxClientData *data );
    wxClientData *GetItemData( const wxDataViewItem& item ) const;

    void DeleteItem( const wxDataViewItem& item );
    void DeleteChildren( const wxDataViewItem& item );
    void DeleteAllItems();

    // implement base methods

    virtual void GetValue( wxVariant &variant,
                           const wxDataViewItem &item, unsigned int col ) const;
    virtual bool SetValue( const wxVariant &variant,
                           const wxDataViewItem &item, unsigned int col );
    virtual wxDataViewItem GetParent( const wxDataViewItem &item ) const;
    virtual bool IsContainer( const wxDataViewItem &item ) const;
    virtual unsigned int GetChildren( const wxDataViewItem &item, wxDataViewItemArray &children ) const;

    virtual int Compare( const wxDataViewItem &item1, const wxDataViewItem &item2,
                         unsigned int column, bool ascending );

    virtual bool HasDefaultCompare() const
        { return true; }
    virtual unsigned int GetColumnCount() const
        { return 1; };
    virtual wxString GetColumnType( unsigned int WXUNUSED(col) ) const
        { return wxT("wxDataViewIconText"); }

    wxDataViewTreeStoreNode *FindNode( const wxDataViewItem &item ) const;
    wxDataViewTreeStoreContainerNode *FindContainerNode( const wxDataViewItem &item ) const;
    wxDataViewTreeStoreNode *GetRoot() const { return m_root; }

public:
    wxDataViewTreeStoreNode *m_root;
};

class WXDLLIMPEXP_ADV wxDataViewTreeCtrl: public wxDataViewCtrl
{
public:
    wxDataViewTreeCtrl();
    wxDataViewTreeCtrl( wxWindow *parent, wxWindowID id,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize, long style = wxDV_NO_HEADER | wxDV_ROW_LINES,
           const wxValidator& validator = wxDefaultValidator );
    ~wxDataViewTreeCtrl();

    bool Create( wxWindow *parent, wxWindowID id,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize, long style = wxDV_NO_HEADER | wxDV_ROW_LINES,
           const wxValidator& validator = wxDefaultValidator );

    wxDataViewTreeStore *GetStore()
        { return (wxDataViewTreeStore*) GetModel(); }
    const wxDataViewTreeStore *GetStore() const
        { return (const wxDataViewTreeStore*) GetModel(); }

    void SetImageList( wxImageList *imagelist );
    wxImageList* GetImageList() { return m_imageList; }

    wxDataViewItem AppendItem( const wxDataViewItem& parent,
        const wxString &text, int icon = -1, wxClientData *data = NULL );
    wxDataViewItem PrependItem( const wxDataViewItem& parent,
        const wxString &text, int icon = -1, wxClientData *data = NULL );
    wxDataViewItem InsertItem( const wxDataViewItem& parent, const wxDataViewItem& previous,
        const wxString &text, int icon = -1, wxClientData *data = NULL );

    wxDataViewItem PrependContainer( const wxDataViewItem& parent,
        const wxString &text, int icon = -1, int expanded = -1,
        wxClientData *data = NULL );
    wxDataViewItem AppendContainer( const wxDataViewItem& parent,
        const wxString &text, int icon = -1, int expanded = -1,
        wxClientData *data = NULL );
    wxDataViewItem InsertContainer( const wxDataViewItem& parent, const wxDataViewItem& previous,
        const wxString &text, int icon = -1, int expanded = -1,
        wxClientData *data = NULL );

    wxDataViewItem GetNthChild( const wxDataViewItem& parent, unsigned int pos ) const
        { return GetStore()->GetNthChild(parent, pos); }
    int GetChildCount( const wxDataViewItem& parent ) const
        { return GetStore()->GetChildCount(parent); }

    void SetItemText( const wxDataViewItem& item, const wxString &text )
        { GetStore()->SetItemText(item,text); }
    wxString GetItemText( const wxDataViewItem& item ) const
        { return GetStore()->GetItemText(item); }
    void SetItemIcon( const wxDataViewItem& item, const wxIcon &icon )
        { GetStore()->SetItemIcon(item,icon); }
    const wxIcon &GetItemIcon( const wxDataViewItem& item ) const
        { return GetStore()->GetItemIcon(item); }
    void SetItemExpandedIcon( const wxDataViewItem& item, const wxIcon &icon )
        { GetStore()->SetItemExpandedIcon(item,icon); }
    const wxIcon &GetItemExpandedIcon( const wxDataViewItem& item ) const
        { return GetStore()->GetItemExpandedIcon(item); }
    void SetItemData( const wxDataViewItem& item, wxClientData *data )
        { GetStore()->SetItemData(item,data); }
    wxClientData *GetItemData( const wxDataViewItem& item ) const
        { return GetStore()->GetItemData(item); }

    void DeleteItem( const wxDataViewItem& item )
        { GetStore()->DeleteItem(item); }
    void DeleteChildren( const wxDataViewItem& item )
        { GetStore()->DeleteChildren(item); }
    void DeleteAllItems()
        { GetStore()->DeleteAllItems(); }

    void OnExpanded( wxDataViewEvent &event );
    void OnCollapsed( wxDataViewEvent &event );
    void OnSize( wxSizeEvent &event );

private:
    wxImageList  *m_imageList;

private:
    DECLARE_EVENT_TABLE()
    DECLARE_DYNAMIC_CLASS_NO_ASSIGN(wxDataViewTreeCtrl)
};

#endif // wxUSE_DATAVIEWCTRL

#endif
    // _WX_DATAVIEW_H_BASE_
