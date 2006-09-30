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
    #define wxUSE_GENERICDATAVIEWCTRL 1
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
class WXDLLIMPEXP_ADV wxDataViewCell;

extern WXDLLIMPEXP_DATA_ADV(const wxChar) wxDataViewCtrlNameStr[];

// ---------------------------------------------------------
// wxDataViewModel
// ---------------------------------------------------------

class WXDLLIMPEXP_ADV wxDataViewModel: public wxObject
{
public:
    wxDataViewModel() { }
    virtual ~wxDataViewModel() { }

protected:
    DECLARE_DYNAMIC_CLASS_NO_COPY(wxDataViewModel)
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
    virtual bool RowInserted( size_t before ) = 0;
    virtual bool RowDeleted( size_t row ) = 0;
    virtual bool RowChanged( size_t row ) = 0;
    virtual bool ValueChanged( size_t col, size_t row ) = 0;
    virtual bool RowsReordered( size_t *new_order ) = 0;
    virtual bool Cleared() = 0;

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
    wxDataViewViewingColumn( wxDataViewColumn *view_column, size_t model_column )
    {
        m_viewColumn = view_column;
        m_modelColumn = model_column;
    }

    wxDataViewColumn   *m_viewColumn;
    size_t              m_modelColumn;
};

class WXDLLIMPEXP_ADV wxDataViewListModel: public wxDataViewModel
{
public:
    wxDataViewListModel();
    virtual ~wxDataViewListModel();

    virtual size_t GetNumberOfRows() = 0;
    virtual size_t GetNumberOfCols() = 0;
    // return type as reported by wxVariant
    virtual wxString GetColType( size_t col ) = 0;
    // get value into a wxVariant
    virtual void GetValue( wxVariant &variant, size_t col, size_t row ) = 0;
    // set value, call ValueChanged() afterwards!
    virtual bool SetValue( wxVariant &variant, size_t col, size_t row ) = 0;

    // delegated notifiers
    virtual bool RowAppended();
    virtual bool RowPrepended();
    virtual bool RowInserted( size_t before );
    virtual bool RowDeleted( size_t row );
    virtual bool RowChanged( size_t row );
    virtual bool ValueChanged( size_t col, size_t row );
    virtual bool RowsReordered( size_t *new_order );
    virtual bool Cleared();

    // Used internally
    virtual void AddViewingColumn( wxDataViewColumn *view_column, size_t model_column );
    virtual void RemoveViewingColumn( wxDataViewColumn *column );

    virtual void AddNotifier( wxDataViewListModelNotifier *notifier );
    virtual void RemoveNotifier( wxDataViewListModelNotifier *notifier );

    wxList                      m_notifiers;
    wxList                      m_viewingColumns;

protected:
    DECLARE_DYNAMIC_CLASS_NO_COPY(wxDataViewListModel)
};

// ---------------------------------------------------------
// wxDataViewSortedListModel
// ---------------------------------------------------------

typedef int (wxCALLBACK *wxDataViewListModelCompare)
    (size_t row1, size_t row2, size_t col, wxDataViewListModel* model );

WX_DEFINE_SORTED_USER_EXPORTED_ARRAY_SIZE_T(size_t, wxDataViewSortedIndexArray, WXDLLIMPEXP_ADV);

class WXDLLIMPEXP_ADV wxDataViewSortedListModel: public wxDataViewListModel
{
public:
    wxDataViewSortedListModel( wxDataViewListModel *child );
    virtual ~wxDataViewSortedListModel();

    virtual size_t GetNumberOfRows();
    virtual size_t GetNumberOfCols();
    // return type as reported by wxVariant
    virtual wxString GetColType( size_t col );
    // get value into a wxVariant
    virtual void GetValue( wxVariant &variant, size_t col, size_t row );
    // set value, call ValueChanged() afterwards!
    virtual bool SetValue( wxVariant &variant, size_t col, size_t row );

    // called from user
    virtual bool RowAppended();
    virtual bool RowPrepended();
    virtual bool RowInserted( size_t before );
    virtual bool RowDeleted( size_t row );
    virtual bool RowChanged( size_t row );
    virtual bool ValueChanged( size_t col, size_t row );
    virtual bool RowsReordered( size_t *new_order );
    virtual bool Cleared();

    // called if child's notifiers are called
    bool ChildRowAppended();
    bool ChildRowPrepended();
    bool ChildRowInserted( size_t before );
    bool ChildRowDeleted( size_t row );
    bool ChildRowChanged( size_t row );
    bool ChildValueChanged( size_t col, size_t row );
    bool ChildRowsReordered( size_t *new_order );
    bool ChildCleared();

    virtual void Resort();

private:
    wxDataViewListModel             *m_child;
    wxDataViewSortedIndexArray       m_array;
    wxDataViewListModelNotifier     *m_notifierOnChild;

protected:
    DECLARE_DYNAMIC_CLASS_NO_COPY(wxDataViewSortedListModel)
};

// ---------------------------------------------------------
// wxDataViewCellBase
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

class WXDLLIMPEXP_ADV wxDataViewCellBase: public wxObject
{
public:
    wxDataViewCellBase( const wxString &varianttype, wxDataViewCellMode mode = wxDATAVIEW_CELL_INERT );

    virtual bool SetValue( const wxVariant& WXUNUSED(value) ) { return true; }
    virtual bool GetValue( wxVariant& WXUNUSED(value) )       { return true; }
    virtual bool Validate( wxVariant& WXUNUSED(value) )       { return true; }

    wxString GetVariantType()       { return m_variantType; }
    wxDataViewCellMode GetMode()    { return m_mode; }

    void SetOwner( wxDataViewColumn *owner )    { m_owner = owner; }
    wxDataViewColumn* GetOwner()                { return m_owner; }

protected:
    wxDataViewCellMode      m_mode;
    wxString                m_variantType;
    wxDataViewColumn       *m_owner;

protected:
    DECLARE_DYNAMIC_CLASS_NO_COPY(wxDataViewCellBase)
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

enum wxDataViewColumnSizing
{
    wxDATAVIEW_COL_WIDTH_FIXED,
    wxDATAVIEW_COL_WIDTH_AUTO,
    wxDATAVIEW_COL_WIDTH_GROW
};

class WXDLLIMPEXP_ADV wxDataViewColumnBase: public wxObject
{
public:
    wxDataViewColumnBase( const wxString &title, wxDataViewCell *cell, size_t model_column,
        int fixed_width = 80, wxDataViewColumnSizing sizing = wxDATAVIEW_COL_WIDTH_FIXED, int flags = 0 );
    virtual ~wxDataViewColumnBase();

    virtual void SetTitle( const wxString &title );
    virtual wxString GetTitle();

    wxDataViewCell* GetCell()               { return m_cell; }

    size_t GetModelColumn()                 { return m_model_column; }

    void SetOwner( wxDataViewCtrl *owner )  { m_owner = owner; }
    wxDataViewCtrl *GetOwner()              { return m_owner; }

    virtual int GetWidth() = 0;

    virtual void SetFixedWidth( int width ) = 0;
    virtual int GetFixedWidth() = 0;

private:
    wxDataViewCtrl          *m_ctrl;
    wxDataViewCell          *m_cell;
    int                      m_model_column;
    int                      m_flags;
    wxString                 m_title;
    wxDataViewCtrl          *m_owner;

protected:
    DECLARE_DYNAMIC_CLASS_NO_COPY(wxDataViewColumnBase)
};

// ---------------------------------------------------------
// wxDataViewCtrlBase
// ---------------------------------------------------------

#define wxDV_SINGLE                  0x0000     // for convenience
#define wxDV_MULTIPLE                0x0020     // can select multiple items

class WXDLLIMPEXP_ADV wxDataViewCtrlBase: public wxControl
{
public:
    wxDataViewCtrlBase();
    virtual ~wxDataViewCtrlBase();

    virtual bool AssociateModel( wxDataViewListModel *model );
    wxDataViewListModel* GetModel();

    virtual bool AppendTextColumn( const wxString &label, size_t model_column );
    virtual bool AppendToggleColumn( const wxString &label, size_t model_column );
    virtual bool AppendProgressColumn( const wxString &label, size_t model_column );
    virtual bool AppendDateColumn( const wxString &label, size_t model_column );
    virtual bool AppendColumn( wxDataViewColumn *col );
    virtual size_t GetNumberOfColumns();
    virtual bool DeleteColumn( size_t pos );
    virtual bool ClearColumns();
    virtual wxDataViewColumn* GetColumn( size_t pos );

private:
    wxDataViewListModel    *m_model;
    wxList                  m_cols;

protected:
    DECLARE_DYNAMIC_CLASS_NO_COPY(wxDataViewCtrlBase)
};

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
