/////////////////////////////////////////////////////////////////////////////
// Name:        wx/generic/grid.h
// Purpose:     wxGrid and related classes
// Author:      Michael Bedward (based on code by Julian Smart, Robin Dunn)
// Modified by:
// Created:     1/08/1999
// RCS-ID:      $Id$
// Copyright:   (c) Michael Bedward
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/defs.h"

#if !defined(wxUSE_NEW_GRID) || !(wxUSE_NEW_GRID)
#include "gridg.h"
#else

#ifndef __WXGRID_H__
#define __WXGRID_H__

#ifdef __GNUG__
#pragma interface "grid.h"
#endif

#include "wx/hash.h"
#include "wx/panel.h"
#include "wx/scrolwin.h"
#include "wx/string.h"
#include "wx/scrolbar.h"
#include "wx/event.h"
#include "wx/combobox.h"
#include "wx/dynarray.h"
#include "wx/timer.h"

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// Default parameters for wxGrid
//
#define WXGRID_DEFAULT_NUMBER_ROWS            10
#define WXGRID_DEFAULT_NUMBER_COLS            10
#ifdef __WXMSW__
#define WXGRID_DEFAULT_ROW_HEIGHT             25
#else
#define WXGRID_DEFAULT_ROW_HEIGHT             30
#endif  // __WXMSW__
#define WXGRID_DEFAULT_COL_WIDTH              80
#define WXGRID_DEFAULT_COL_LABEL_HEIGHT       32
#define WXGRID_DEFAULT_ROW_LABEL_WIDTH        82
#define WXGRID_LABEL_EDGE_ZONE                 2
#define WXGRID_MIN_ROW_HEIGHT                 15
#define WXGRID_MIN_COL_WIDTH                  15
#define WXGRID_DEFAULT_SCROLLBAR_WIDTH        16

// type names for grid table values
#define wxGRID_VALUE_STRING     _T("string")
#define wxGRID_VALUE_BOOL       _T("bool")
#define wxGRID_VALUE_NUMBER     _T("long")
#define wxGRID_VALUE_FLOAT      _T("double")
#define wxGRID_VALUE_CHOICE     _T("choice")

#define wxGRID_VALUE_TEXT wxGRID_VALUE_STRING
#define wxGRID_VALUE_LONG wxGRID_VALUE_NUMBER

// ----------------------------------------------------------------------------
// forward declarations
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxGrid;
class WXDLLEXPORT wxGridCellAttr;
class WXDLLEXPORT wxGridCellAttrProviderData;
class WXDLLEXPORT wxGridColLabelWindow;
class WXDLLEXPORT wxGridCornerLabelWindow;
class WXDLLEXPORT wxGridRowLabelWindow;
class WXDLLEXPORT wxGridTableBase;
class WXDLLEXPORT wxGridWindow;
class WXDLLEXPORT wxGridTypeRegistry;
class WXDLLEXPORT wxGridSelection;

class WXDLLEXPORT wxCheckBox;
class WXDLLEXPORT wxComboBox;
class WXDLLEXPORT wxTextCtrl;
class WXDLLEXPORT wxSpinCtrl;

// ----------------------------------------------------------------------------
// macros
// ----------------------------------------------------------------------------

#define wxSafeIncRef(p) if ( p ) (p)->IncRef()
#define wxSafeDecRef(p) if ( p ) (p)->DecRef()

// ----------------------------------------------------------------------------
// wxGridCellWorker: common base class for wxGridCellRenderer and
// wxGridCellEditor
//
// NB: this is more an implementation convenience than a design issue, so this
//     class is not documented and is not public at all
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxGridCellWorker
{
public:
    wxGridCellWorker() { m_nRef = 1; }

    // this class is ref counted: it is created with ref count of 1, so
    // calling DecRef() once will delete it. Calling IncRef() allows to lock
    // it until the matching DecRef() is called
    void IncRef() { m_nRef++; }
    void DecRef() { if ( !--m_nRef ) delete this; }

    // interpret renderer parameters: arbitrary string whose interpretatin is
    // left to the derived classes
    virtual void SetParameters(const wxString& params);

protected:
    // virtual dtor for any base class - private because only DecRef() can
    // delete us
    virtual ~wxGridCellWorker();

private:
    size_t m_nRef;

    // suppress the stupid gcc warning about the class having private dtor and
    // no friends
    friend class wxGridCellWorkerDummyFriend;
};

// ----------------------------------------------------------------------------
// wxGridCellRenderer: this class is responsible for actually drawing the cell
// in the grid. You may pass it to the wxGridCellAttr (below) to change the
// format of one given cell or to wxGrid::SetDefaultRenderer() to change the
// view of all cells. This is an ABC, you will normally use one of the
// predefined derived classes or derive your own class from it.
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxGridCellRenderer : public wxGridCellWorker
{
public:
    // draw the given cell on the provided DC inside the given rectangle
    // using the style specified by the attribute and the default or selected
    // state corresponding to the isSelected value.
    //
    // this pure virtual function has a default implementation which will
    // prepare the DC using the given attribute: it will draw the rectangle
    // with the bg colour from attr and set the text colour and font
    virtual void Draw(wxGrid& grid,
                      wxGridCellAttr& attr,
                      wxDC& dc,
                      const wxRect& rect,
                      int row, int col,
                      bool isSelected) = 0;

    // get the preferred size of the cell for its contents
    virtual wxSize GetBestSize(wxGrid& grid,
                               wxGridCellAttr& attr,
                               wxDC& dc,
                               int row, int col) = 0;

    // create a new object which is the copy of this one
    virtual wxGridCellRenderer *Clone() const = 0;
};

// the default renderer for the cells containing string data
class WXDLLEXPORT wxGridCellStringRenderer : public wxGridCellRenderer
{
public:
    // draw the string
    virtual void Draw(wxGrid& grid,
                      wxGridCellAttr& attr,
                      wxDC& dc,
                      const wxRect& rect,
                      int row, int col,
                      bool isSelected);

    // return the string extent
    virtual wxSize GetBestSize(wxGrid& grid,
                               wxGridCellAttr& attr,
                               wxDC& dc,
                               int row, int col);

    virtual wxGridCellRenderer *Clone() const
        { return new wxGridCellStringRenderer; }

protected:
    // set the text colours before drawing
    void SetTextColoursAndFont(wxGrid& grid,
                               wxGridCellAttr& attr,
                               wxDC& dc,
                               bool isSelected);

    // calc the string extent for given string/font
    wxSize DoGetBestSize(wxGridCellAttr& attr,
                         wxDC& dc,
                         const wxString& text);
};

// the default renderer for the cells containing numeric (long) data
class WXDLLEXPORT wxGridCellNumberRenderer : public wxGridCellStringRenderer
{
public:
    // draw the string right aligned
    virtual void Draw(wxGrid& grid,
                      wxGridCellAttr& attr,
                      wxDC& dc,
                      const wxRect& rect,
                      int row, int col,
                      bool isSelected);

    virtual wxSize GetBestSize(wxGrid& grid,
                               wxGridCellAttr& attr,
                               wxDC& dc,
                               int row, int col);

    virtual wxGridCellRenderer *Clone() const
        { return new wxGridCellNumberRenderer; }

protected:
    wxString GetString(wxGrid& grid, int row, int col);
};

class WXDLLEXPORT wxGridCellFloatRenderer : public wxGridCellStringRenderer
{
public:
    wxGridCellFloatRenderer(int width = -1, int precision = -1);

    // get/change formatting parameters
    int GetWidth() const { return m_width; }
    void SetWidth(int width) { m_width = width; m_format.clear(); }
    int GetPrecision() const { return m_precision; }
    void SetPrecision(int precision) { m_precision = precision; m_format.clear(); }

    // draw the string right aligned with given width/precision
    virtual void Draw(wxGrid& grid,
                      wxGridCellAttr& attr,
                      wxDC& dc,
                      const wxRect& rect,
                      int row, int col,
                      bool isSelected);

    virtual wxSize GetBestSize(wxGrid& grid,
                               wxGridCellAttr& attr,
                               wxDC& dc,
                               int row, int col);

    // parameters string format is "width[,precision]"
    virtual void SetParameters(const wxString& params);

    virtual wxGridCellRenderer *Clone() const;

protected:
    wxString GetString(wxGrid& grid, int row, int col);

private:
    // formatting parameters
    int m_width,
        m_precision;

    wxString m_format;
};

// renderer for boolean fields
class WXDLLEXPORT wxGridCellBoolRenderer : public wxGridCellRenderer
{
public:
    // draw a check mark or nothing
    virtual void Draw(wxGrid& grid,
                      wxGridCellAttr& attr,
                      wxDC& dc,
                      const wxRect& rect,
                      int row, int col,
                      bool isSelected);

    // return the checkmark size
    virtual wxSize GetBestSize(wxGrid& grid,
                               wxGridCellAttr& attr,
                               wxDC& dc,
                               int row, int col);

    virtual wxGridCellRenderer *Clone() const
        { return new wxGridCellBoolRenderer; }

private:
    static wxSize ms_sizeCheckMark;
};

// ----------------------------------------------------------------------------
// wxGridCellEditor:  This class is responsible for providing and manipulating
// the in-place edit controls for the grid.  Instances of wxGridCellEditor
// (actually, instances of derived classes since it is an ABC) can be
// associated with the cell attributes for individual cells, rows, columns, or
// even for the entire grid.
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxGridCellEditor : public wxGridCellWorker
{
public:
    wxGridCellEditor();

    bool IsCreated() { return m_control != NULL; }

    // Creates the actual edit control
    virtual void Create(wxWindow* parent,
                        wxWindowID id,
                        wxEvtHandler* evtHandler) = 0;

    // Size and position the edit control
    virtual void SetSize(const wxRect& rect);

    // Show or hide the edit control, use the specified attributes to set
    // colours/fonts for it
    virtual void Show(bool show, wxGridCellAttr *attr = (wxGridCellAttr *)NULL);

    // Draws the part of the cell not occupied by the control: the base class
    // version just fills it with background colour from the attribute
    virtual void PaintBackground(const wxRect& rectCell, wxGridCellAttr *attr);

    // Fetch the value from the table and prepare the edit control
    // to begin editing.  Set the focus to the edit control.
    virtual void BeginEdit(int row, int col, wxGrid* grid) = 0;

    // Complete the editing of the current cell. Returns true if the value has
    // changed.  If necessary, the control may be destroyed.
    virtual bool EndEdit(int row, int col, wxGrid* grid) = 0;

    // Reset the value in the control back to its starting value
    virtual void Reset() = 0;

    // If the editor is enabled by pressing keys on the grid,
    // this will be called to let the editor do something about
    // that first key if desired.
    virtual void StartingKey(wxKeyEvent& event);

    // if the editor is enabled by clicking on the cell, this method will be
    // called
    virtual void StartingClick();

    // Some types of controls on some platforms may need some help
    // with the Return key.
    virtual void HandleReturn(wxKeyEvent& event);

    // Final cleanup
    virtual void Destroy();

    // create a new object which is the copy of this one
    virtual wxGridCellEditor *Clone() const = 0;

protected:
    // the dtor is private because only DecRef() can delete us
    virtual ~wxGridCellEditor();

    // the control we show on screen
    wxControl*  m_control;

    // if we change the colours/font of the control from the default ones, we
    // must restore the default later and we save them here between calls to
    // Show(TRUE) and Show(FALSE)
    wxColour m_colFgOld,
             m_colBgOld;
    wxFont m_fontOld;

    // suppress the stupid gcc warning about the class having private dtor and
    // no friends
    friend class wxGridCellEditorDummyFriend;
};

// the editor for string/text data
class WXDLLEXPORT wxGridCellTextEditor : public wxGridCellEditor
{
public:
    wxGridCellTextEditor();

    virtual void Create(wxWindow* parent,
                        wxWindowID id,
                        wxEvtHandler* evtHandler);
    virtual void SetSize(const wxRect& rect);

    virtual void PaintBackground(const wxRect& rectCell, wxGridCellAttr *attr);

    virtual void BeginEdit(int row, int col, wxGrid* grid);
    virtual bool EndEdit(int row, int col, wxGrid* grid);

    virtual void Reset();
    virtual void StartingKey(wxKeyEvent& event);
    virtual void HandleReturn(wxKeyEvent& event);

    // parameters string format is "max_width"
    virtual void SetParameters(const wxString& params);

    virtual wxGridCellEditor *Clone() const
        { return new wxGridCellTextEditor; }

protected:
    wxTextCtrl *Text() const { return (wxTextCtrl *)m_control; }

    // parts of our virtual functions reused by the derived classes
    void DoBeginEdit(const wxString& startValue);
    void DoReset(const wxString& startValue);

private:
    size_t   m_maxChars;        // max number of chars allowed
    wxString m_startValue;
};

// the editor for numeric (long) data
class WXDLLEXPORT wxGridCellNumberEditor : public wxGridCellTextEditor
{
public:
    // allows to specify the range - if min == max == -1, no range checking is
    // done
    wxGridCellNumberEditor(int min = -1, int max = -1);

    virtual void Create(wxWindow* parent,
                        wxWindowID id,
                        wxEvtHandler* evtHandler);

    virtual void BeginEdit(int row, int col, wxGrid* grid);
    virtual bool EndEdit(int row, int col, wxGrid* grid);

    virtual void Reset();
    virtual void StartingKey(wxKeyEvent& event);

    // parameters string format is "min,max"
    virtual void SetParameters(const wxString& params);

    virtual wxGridCellEditor *Clone() const
        { return new wxGridCellNumberEditor(m_min, m_max); }

protected:
    wxSpinCtrl *Spin() const { return (wxSpinCtrl *)m_control; }

    // if HasRange(), we use wxSpinCtrl - otherwise wxTextCtrl
    bool HasRange() const { return m_min != m_max; }

    // string representation of m_valueOld
    wxString GetString() const
        { return wxString::Format(_T("%ld"), m_valueOld); }

private:
    int m_min,
        m_max;

    long m_valueOld;
};

// the editor for floating point numbers (double) data
class WXDLLEXPORT wxGridCellFloatEditor : public wxGridCellTextEditor
{
public:
    virtual void Create(wxWindow* parent,
                        wxWindowID id,
                        wxEvtHandler* evtHandler);

    virtual void BeginEdit(int row, int col, wxGrid* grid);
    virtual bool EndEdit(int row, int col, wxGrid* grid);

    virtual void Reset();
    virtual void StartingKey(wxKeyEvent& event);

    virtual wxGridCellEditor *Clone() const
        { return new wxGridCellFloatEditor; }

protected:
    // string representation of m_valueOld
    wxString GetString() const
        { return wxString::Format(_T("%f"), m_valueOld); }

private:
    double m_valueOld;
};

// the editor for boolean data
class WXDLLEXPORT wxGridCellBoolEditor : public wxGridCellEditor
{
public:
    virtual void Create(wxWindow* parent,
                        wxWindowID id,
                        wxEvtHandler* evtHandler);

    virtual void SetSize(const wxRect& rect);
    virtual void Show(bool show, wxGridCellAttr *attr = (wxGridCellAttr *)NULL);

    virtual void BeginEdit(int row, int col, wxGrid* grid);
    virtual bool EndEdit(int row, int col, wxGrid* grid);

    virtual void Reset();
    virtual void StartingClick();

    virtual wxGridCellEditor *Clone() const
        { return new wxGridCellBoolEditor; }

protected:
    wxCheckBox *CBox() const { return (wxCheckBox *)m_control; }

private:
    bool m_startValue;
};

// the editor for string data allowing to choose from the list of strings
class WXDLLEXPORT wxGridCellChoiceEditor : public wxGridCellEditor
{
public:
    // if !allowOthers, user can't type a string not in choices array
    wxGridCellChoiceEditor(size_t count = 0,
                           const wxChar* choices[] = NULL,
                           bool allowOthers = FALSE);

    virtual void Create(wxWindow* parent,
                        wxWindowID id,
                        wxEvtHandler* evtHandler);

    virtual void PaintBackground(const wxRect& rectCell, wxGridCellAttr *attr);

    virtual void BeginEdit(int row, int col, wxGrid* grid);
    virtual bool EndEdit(int row, int col, wxGrid* grid);

    virtual void Reset();

    // parameters string format is "item1[,item2[...,itemN]]"
    virtual void SetParameters(const wxString& params);

    virtual wxGridCellEditor *Clone() const;

protected:
    wxComboBox *Combo() const { return (wxComboBox *)m_control; }

private:
    wxString        m_startValue;
    wxArrayString   m_choices;
    bool            m_allowOthers;
};

// ----------------------------------------------------------------------------
// wxGridCellAttr: this class can be used to alter the cells appearance in
// the grid by changing their colour/font/... from default. An object of this
// class may be returned by wxGridTable::GetAttr().
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxGridCellAttr
{
public:
    // ctors
    wxGridCellAttr()
    {
        Init();
        SetAlignment(0, 0);
    }

    // VZ: considering the number of members wxGridCellAttr has now, this ctor
    //     seems to be pretty useless... may be we should just remove it?
    wxGridCellAttr(const wxColour& colText,
                   const wxColour& colBack,
                   const wxFont& font,
                   int hAlign,
                   int vAlign)
        : m_colText(colText), m_colBack(colBack), m_font(font)
    {
        Init();
        SetAlignment(hAlign, vAlign);
    }

    // creates a new copy of this object
    wxGridCellAttr *Clone() const;

    // this class is ref counted: it is created with ref count of 1, so
    // calling DecRef() once will delete it. Calling IncRef() allows to lock
    // it until the matching DecRef() is called
    void IncRef() { m_nRef++; }
    void DecRef() { if ( !--m_nRef ) delete this; }

    // setters
    void SetTextColour(const wxColour& colText) { m_colText = colText; }
    void SetBackgroundColour(const wxColour& colBack) { m_colBack = colBack; }
    void SetFont(const wxFont& font) { m_font = font; }
    void SetAlignment(int hAlign, int vAlign)
    {
        m_hAlign = hAlign;
        m_vAlign = vAlign;
    }
    void SetReadOnly(bool isReadOnly = TRUE) { m_isReadOnly = isReadOnly; }

    // takes ownership of the pointer
    void SetRenderer(wxGridCellRenderer *renderer)
        { wxSafeDecRef(m_renderer); m_renderer = renderer; }
    void SetEditor(wxGridCellEditor* editor)
        { wxSafeDecRef(m_editor); m_editor = editor; }

    // accessors
    bool HasTextColour() const { return m_colText.Ok(); }
    bool HasBackgroundColour() const { return m_colBack.Ok(); }
    bool HasFont() const { return m_font.Ok(); }
    bool HasAlignment() const { return m_hAlign || m_vAlign; }
    bool HasRenderer() const { return m_renderer != NULL; }
    bool HasEditor() const { return m_editor != NULL; }

    const wxColour& GetTextColour() const;
    const wxColour& GetBackgroundColour() const;
    const wxFont& GetFont() const;
    void GetAlignment(int *hAlign, int *vAlign) const;
    wxGridCellRenderer *GetRenderer(wxGrid* grid, int row, int col) const;
    wxGridCellEditor *GetEditor(wxGrid* grid, int row, int col) const;

    bool IsReadOnly() const { return m_isReadOnly; }

    void SetDefAttr(wxGridCellAttr* defAttr) { m_defGridAttr = defAttr; }

private:
    // the common part of all ctors
    void Init()
    {
        m_nRef = 1;

        m_isReadOnly = FALSE;

        m_renderer = NULL;
        m_editor = NULL;
    }

    // the dtor is private because only DecRef() can delete us
    ~wxGridCellAttr()
    {
        wxSafeDecRef(m_renderer);
        wxSafeDecRef(m_editor);
    }

    // the ref count - when it goes to 0, we die
    size_t   m_nRef;

    wxColour m_colText,
             m_colBack;
    wxFont   m_font;
    int      m_hAlign,
             m_vAlign;

    wxGridCellRenderer* m_renderer;
    wxGridCellEditor*   m_editor;
    wxGridCellAttr*     m_defGridAttr;

    bool m_isReadOnly;

    // use Clone() instead
    DECLARE_NO_COPY_CLASS(wxGridCellAttr);

    // suppress the stupid gcc warning about the class having private dtor and
    // no friends
    friend class wxGridCellAttrDummyFriend;
};

// ----------------------------------------------------------------------------
// wxGridCellAttrProvider: class used by wxGridTableBase to retrieve/store the
// cell attributes.
// ----------------------------------------------------------------------------

// implementation note: we separate it from wxGridTableBase because we wish to
// avoid deriving a new table class if possible, and sometimes it will be
// enough to just derive another wxGridCellAttrProvider instead
//
// the default implementation is reasonably efficient for the generic case,
// but you might still wish to implement your own for some specific situations
// if you have performance problems with the stock one
class WXDLLEXPORT wxGridCellAttrProvider
{
public:
    wxGridCellAttrProvider();
    virtual ~wxGridCellAttrProvider();

    // DecRef() must be called on the returned pointer
    virtual wxGridCellAttr *GetAttr(int row, int col) const;

    // all these functions take ownership of the pointer, don't call DecRef()
    // on it
    virtual void SetAttr(wxGridCellAttr *attr, int row, int col);
    virtual void SetRowAttr(wxGridCellAttr *attr, int row);
    virtual void SetColAttr(wxGridCellAttr *attr, int col);

    // these functions must be called whenever some rows/cols are deleted
    // because the internal data must be updated then
    void UpdateAttrRows( size_t pos, int numRows );
    void UpdateAttrCols( size_t pos, int numCols );

private:
    void InitData();

    wxGridCellAttrProviderData *m_data;
};

//////////////////////////////////////////////////////////////////////
//
//  Grid table classes
//
//////////////////////////////////////////////////////////////////////


class WXDLLEXPORT wxGridTableBase : public wxObject
{
public:
    wxGridTableBase();
    virtual ~wxGridTableBase();

    // You must override these functions in a derived table class
    //
    virtual int GetNumberRows() = 0;
    virtual int GetNumberCols() = 0;
    virtual bool IsEmptyCell( int row, int col ) = 0;
    virtual wxString GetValue( int row, int col ) = 0;
    virtual void SetValue( int row, int col, const wxString& value ) = 0;

    // Data type determination and value access
    virtual wxString GetTypeName( int row, int col );
    virtual bool CanGetValueAs( int row, int col, const wxString& typeName );
    virtual bool CanSetValueAs( int row, int col, const wxString& typeName );

    virtual long GetValueAsLong( int row, int col );
    virtual double GetValueAsDouble( int row, int col );
    virtual bool GetValueAsBool( int row, int col );

    virtual void SetValueAsLong( int row, int col, long value );
    virtual void SetValueAsDouble( int row, int col, double value );
    virtual void SetValueAsBool( int row, int col, bool value );

    // For user defined types
    virtual void* GetValueAsCustom( int row, int col, const wxString& typeName );
    virtual void  SetValueAsCustom( int row, int col, const wxString& typeName, void* value );


    // Overriding these is optional
    //
    virtual void SetView( wxGrid *grid ) { m_view = grid; }
    virtual wxGrid * GetView() const { return m_view; }

    virtual void Clear() {}
    virtual bool InsertRows( size_t pos = 0, size_t numRows = 1 );
    virtual bool AppendRows( size_t numRows = 1 );
    virtual bool DeleteRows( size_t pos = 0, size_t numRows = 1 );
    virtual bool InsertCols( size_t pos = 0, size_t numCols = 1 );
    virtual bool AppendCols( size_t numCols = 1 );
    virtual bool DeleteCols( size_t pos = 0, size_t numCols = 1 );

    virtual wxString GetRowLabelValue( int row );
    virtual wxString GetColLabelValue( int col );
    virtual void SetRowLabelValue( int WXUNUSED(row), const wxString& ) {}
    virtual void SetColLabelValue( int WXUNUSED(col), const wxString& ) {}

    // Attribute handling
    //

    // give us the attr provider to use - we take ownership of the pointer
    void SetAttrProvider(wxGridCellAttrProvider *attrProvider);

    // get the currently used attr provider (may be NULL)
    wxGridCellAttrProvider *GetAttrProvider() const { return m_attrProvider; }

    // Does this table allow attributes?  Default implementation creates
    // a wxGridCellAttrProvider if necessary.
    virtual bool CanHaveAttributes();


    // change row/col number in attribute if needed
    virtual void UpdateAttrRows( size_t pos, int numRows );
    virtual void UpdateAttrCols( size_t pos, int numCols );

    // by default forwarded to wxGridCellAttrProvider if any. May be
    // overridden to handle attributes directly in the table.
    virtual wxGridCellAttr *GetAttr( int row, int col );

    // these functions take ownership of the pointer
    virtual void SetAttr(wxGridCellAttr* attr, int row, int col);
    virtual void SetRowAttr(wxGridCellAttr *attr, int row);
    virtual void SetColAttr(wxGridCellAttr *attr, int col);

private:
    wxGrid * m_view;
    wxGridCellAttrProvider *m_attrProvider;

    DECLARE_ABSTRACT_CLASS( wxGridTableBase );
};


// ----------------------------------------------------------------------------
// wxGridTableMessage
// ----------------------------------------------------------------------------

// IDs for messages sent from grid table to view
//
enum wxGridTableRequest
{
    wxGRIDTABLE_REQUEST_VIEW_GET_VALUES = 2000,
    wxGRIDTABLE_REQUEST_VIEW_SEND_VALUES,
    wxGRIDTABLE_NOTIFY_ROWS_INSERTED,
    wxGRIDTABLE_NOTIFY_ROWS_APPENDED,
    wxGRIDTABLE_NOTIFY_ROWS_DELETED,
    wxGRIDTABLE_NOTIFY_COLS_INSERTED,
    wxGRIDTABLE_NOTIFY_COLS_APPENDED,
    wxGRIDTABLE_NOTIFY_COLS_DELETED
};

class WXDLLEXPORT wxGridTableMessage
{
public:
    wxGridTableMessage();
    wxGridTableMessage( wxGridTableBase *table, int id,
                        int comInt1 = -1,
                        int comInt2 = -1 );

    void SetTableObject( wxGridTableBase *table ) { m_table = table; }
    wxGridTableBase * GetTableObject() const { return m_table; }
    void SetId( int id ) { m_id = id; }
    int  GetId() { return m_id; }
    void SetCommandInt( int comInt1 ) { m_comInt1 = comInt1; }
    int  GetCommandInt() { return m_comInt1; }
    void SetCommandInt2( int comInt2 ) { m_comInt2 = comInt2; }
    int  GetCommandInt2() { return m_comInt2; }

private:
    wxGridTableBase *m_table;
    int m_id;
    int m_comInt1;
    int m_comInt2;
};



// ------ wxGridStringArray
// A 2-dimensional array of strings for data values
//

WX_DECLARE_EXPORTED_OBJARRAY(wxArrayString, wxGridStringArray);



// ------ wxGridStringTable
//
// Simplest type of data table for a grid for small tables of strings
// that are stored in memory
//

class WXDLLEXPORT wxGridStringTable : public wxGridTableBase
{
public:
    wxGridStringTable();
    wxGridStringTable( int numRows, int numCols );
    ~wxGridStringTable();

    // these are pure virtual in wxGridTableBase
    //
    int GetNumberRows();
    int GetNumberCols();
    wxString GetValue( int row, int col );
    void SetValue( int row, int col, const wxString& s );
    bool IsEmptyCell( int row, int col );

    // overridden functions from wxGridTableBase
    //
    void Clear();
    bool InsertRows( size_t pos = 0, size_t numRows = 1 );
    bool AppendRows( size_t numRows = 1 );
    bool DeleteRows( size_t pos = 0, size_t numRows = 1 );
    bool InsertCols( size_t pos = 0, size_t numCols = 1 );
    bool AppendCols( size_t numCols = 1 );
    bool DeleteCols( size_t pos = 0, size_t numCols = 1 );

    void SetRowLabelValue( int row, const wxString& );
    void SetColLabelValue( int col, const wxString& );
    wxString GetRowLabelValue( int row );
    wxString GetColLabelValue( int col );

private:
    wxGridStringArray m_data;

    // These only get used if you set your own labels, otherwise the
    // GetRow/ColLabelValue functions return wxGridTableBase defaults
    //
    wxArrayString     m_rowLabels;
    wxArrayString     m_colLabels;

    DECLARE_DYNAMIC_CLASS( wxGridStringTable )
};



// ============================================================================
//  Grid view classes
// ============================================================================

// ----------------------------------------------------------------------------
// wxGridCellCoords: location of a cell in the grid
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxGridCellCoords
{
public:
    wxGridCellCoords() { m_row = m_col = -1; }
    wxGridCellCoords( int r, int c ) { m_row = r; m_col = c; }

    // default copy ctor is ok

    int GetRow() const { return m_row; }
    void SetRow( int n ) { m_row = n; }
    int GetCol() const { return m_col; }
    void SetCol( int n ) { m_col = n; }
    void Set( int row, int col ) { m_row = row; m_col = col; }

    wxGridCellCoords& operator=( const wxGridCellCoords& other )
    {
        if ( &other != this )
        {
            m_row=other.m_row;
            m_col=other.m_col;
        }
        return *this;
    }

    bool operator==( const wxGridCellCoords& other ) const
    {
        return (m_row == other.m_row  &&  m_col == other.m_col);
    }

    bool operator!=( const wxGridCellCoords& other ) const
    {
        return (m_row != other.m_row  ||  m_col != other.m_col);
    }

    bool operator!() const
    {
        return (m_row == -1 && m_col == -1 );
    }

private:
    int m_row;
    int m_col;
};


// For comparisons...
//
extern wxGridCellCoords wxGridNoCellCoords;
extern wxRect           wxGridNoCellRect;

// An array of cell coords...
//
WX_DECLARE_EXPORTED_OBJARRAY(wxGridCellCoords, wxGridCellCoordsArray);

// ----------------------------------------------------------------------------
// wxGrid
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxGrid : public wxScrolledWindow
{
public:
    wxGrid()
        {
            Create();
        }

    wxGrid( wxWindow *parent,
            wxWindowID id,
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize,
            long style = wxWANTS_CHARS,
            const wxString& name = wxPanelNameStr );

    ~wxGrid();

    enum wxGridSelectionModes {wxGridSelectCells,
                               wxGridSelectRows,
                               wxGridSelectColumns};

    bool CreateGrid( int numRows, int numCols,
                     wxGrid::wxGridSelectionModes selmode =
                     wxGrid::wxGridSelectCells );

    void SetSelectionMode(wxGrid::wxGridSelectionModes selmode);

    // ------ grid dimensions
    //
    int      GetNumberRows() { return  m_numRows; }
    int      GetNumberCols() { return  m_numCols; }


    // ------ display update functions
    //
    void CalcRowLabelsExposed( wxRegion& reg );

    void CalcColLabelsExposed( wxRegion& reg );
    void CalcCellsExposed( wxRegion& reg );


    // ------ event handlers
    //
    void ProcessRowLabelMouseEvent( wxMouseEvent& event );
    void ProcessColLabelMouseEvent( wxMouseEvent& event );
    void ProcessCornerLabelMouseEvent( wxMouseEvent& event );
    void ProcessGridCellMouseEvent( wxMouseEvent& event );
    bool ProcessTableMessage( wxGridTableMessage& );

    void DoEndDragResizeRow();
    void DoEndDragResizeCol();

    wxGridTableBase * GetTable() const { return m_table; }
    bool SetTable( wxGridTableBase *table, bool takeOwnership=FALSE,
                   wxGrid::wxGridSelectionModes selmode =
                   wxGrid::wxGridSelectCells );

    void ClearGrid();
    bool InsertRows( int pos = 0, int numRows = 1, bool updateLabels=TRUE );
    bool AppendRows( int numRows = 1, bool updateLabels=TRUE );
    bool DeleteRows( int pos = 0, int numRows = 1, bool updateLabels=TRUE );
    bool InsertCols( int pos = 0, int numCols = 1, bool updateLabels=TRUE );
    bool AppendCols( int numCols = 1, bool updateLabels=TRUE );
    bool DeleteCols( int pos = 0, int numCols = 1, bool updateLabels=TRUE );

    void DrawGridCellArea( wxDC& dc );
    void DrawGridSpace( wxDC& dc );
    void DrawCellBorder( wxDC& dc, const wxGridCellCoords& );
    void DrawAllGridLines( wxDC& dc, const wxRegion & reg );
    void DrawCell( wxDC& dc, const wxGridCellCoords& );
    void DrawHighlight(wxDC& dc);

    // this function is called when the current cell highlight must be redrawn
    // and may be overridden by the user
    virtual void DrawCellHighlight( wxDC& dc, const wxGridCellAttr *attr );

    void DrawRowLabels( wxDC& dc );
    void DrawRowLabel( wxDC& dc, int row );

    void DrawColLabels( wxDC& dc );
    void DrawColLabel( wxDC& dc, int col );


    // ------ Cell text drawing functions
    //
    void DrawTextRectangle( wxDC& dc, const wxString&, const wxRect&,
                            int horizontalAlignment = wxLEFT,
                            int verticalAlignment = wxTOP );

    // Split a string containing newline chararcters into an array of
    // strings and return the number of lines
    //
    void StringToLines( const wxString& value, wxArrayString& lines );

    void GetTextBoxSize( wxDC& dc,
                         wxArrayString& lines,
                         long *width, long *height );


    // ------
    // Code that does a lot of grid modification can be enclosed
    // between BeginBatch() and EndBatch() calls to avoid screen
    // flicker
    //
    void     BeginBatch() { m_batchCount++; }
    void     EndBatch() { if ( m_batchCount > 0 ) m_batchCount--; }
    int      GetBatchCount() { return m_batchCount; }


    // ------ edit control functions
    //
    bool IsEditable() { return m_editable; }
    void EnableEditing( bool edit );

    void EnableCellEditControl( bool enable = TRUE );
    void DisableCellEditControl() { EnableCellEditControl(FALSE); }
    bool CanEnableCellControl() const;
    bool IsCellEditControlEnabled() const;

    bool IsCurrentCellReadOnly() const;

    void ShowCellEditControl();
    void HideCellEditControl();
    void SaveEditControlValue();


    // ------ grid location functions
    //  Note that all of these functions work with the logical coordinates of
    //  grid cells and labels so you will need to convert from device
    //  coordinates for mouse events etc.
    //
    void XYToCell( int x, int y, wxGridCellCoords& );
    int  YToRow( int y );
    int  XToCol( int x );

    int  YToEdgeOfRow( int y );
    int  XToEdgeOfCol( int x );

    wxRect CellToRect( int row, int col );
    wxRect CellToRect( const wxGridCellCoords& coords )
        { return CellToRect( coords.GetRow(), coords.GetCol() ); }

    int  GetGridCursorRow() { return m_currentCellCoords.GetRow(); }
    int  GetGridCursorCol() { return m_currentCellCoords.GetCol(); }

    // check to see if a cell is either wholly visible (the default arg) or
    // at least partially visible in the grid window
    //
    bool IsVisible( int row, int col, bool wholeCellVisible = TRUE );
    bool IsVisible( const wxGridCellCoords& coords, bool wholeCellVisible = TRUE )
        { return IsVisible( coords.GetRow(), coords.GetCol(), wholeCellVisible ); }
    void MakeCellVisible( int row, int col );
    void MakeCellVisible( const wxGridCellCoords& coords )
        { MakeCellVisible( coords.GetRow(), coords.GetCol() ); }


    // ------ grid cursor movement functions
    //
    void SetGridCursor( int row, int col )
        { SetCurrentCell( wxGridCellCoords(row, col) ); }

    bool MoveCursorUp( bool expandSelection );
    bool MoveCursorDown( bool expandSelection );
    bool MoveCursorLeft( bool expandSelection );
    bool MoveCursorRight( bool expandSelection );
    bool MovePageDown();
    bool MovePageUp();
    bool MoveCursorUpBlock( bool expandSelection );
    bool MoveCursorDownBlock( bool expandSelection );
    bool MoveCursorLeftBlock( bool expandSelection );
    bool MoveCursorRightBlock( bool expandSelection );


    // ------ label and gridline formatting
    //
    int      GetDefaultRowLabelSize() { return WXGRID_DEFAULT_ROW_LABEL_WIDTH; }
    int      GetRowLabelSize() { return m_rowLabelWidth; }
    int      GetDefaultColLabelSize() { return WXGRID_DEFAULT_COL_LABEL_HEIGHT; }
    int      GetColLabelSize() { return m_colLabelHeight; }
    wxColour GetLabelBackgroundColour() { return m_labelBackgroundColour; }
    wxColour GetLabelTextColour() { return m_labelTextColour; }
    wxFont   GetLabelFont() { return m_labelFont; }
    void     GetRowLabelAlignment( int *horiz, int *vert );
    void     GetColLabelAlignment( int *horiz, int *vert );
    wxString GetRowLabelValue( int row );
    wxString GetColLabelValue( int col );
    wxColour GetGridLineColour() { return m_gridLineColour; }

    void     SetRowLabelSize( int width );
    void     SetColLabelSize( int height );
    void     SetLabelBackgroundColour( const wxColour& );
    void     SetLabelTextColour( const wxColour& );
    void     SetLabelFont( const wxFont& );
    void     SetRowLabelAlignment( int horiz, int vert );
    void     SetColLabelAlignment( int horiz, int vert );
    void     SetRowLabelValue( int row, const wxString& );
    void     SetColLabelValue( int col, const wxString& );
    void     SetGridLineColour( const wxColour& );

    void     EnableDragRowSize( bool enable = TRUE );
    void     DisableDragRowSize() { EnableDragRowSize( FALSE ); }
    bool     CanDragRowSize() { return m_canDragRowSize; }
    void     EnableDragColSize( bool enable = TRUE );
    void     DisableDragColSize() { EnableDragColSize( FALSE ); }
    bool     CanDragColSize() { return m_canDragColSize; }
    void     EnableDragGridSize(bool enable = TRUE);
    void     DisableDragGridSize() { EnableDragGridSize(FALSE); }
    bool     CanDragGridSize() { return m_canDragGridSize; }

    // this sets the specified attribute for all cells in this row/col
    void     SetRowAttr(int row, wxGridCellAttr *attr);
    void     SetColAttr(int col, wxGridCellAttr *attr);

    // shortcuts for setting the column parameters

    // set the format for the data in the column: default is string
    void     SetColFormatBool(int col);
    void     SetColFormatNumber(int col);
    void     SetColFormatFloat(int col, int width = -1, int precision = -1);
    void     SetColFormatCustom(int col, const wxString& typeName);

    void     EnableGridLines( bool enable = TRUE );
    bool     GridLinesEnabled() { return m_gridLinesEnabled; }

    // ------ row and col formatting
    //
    int      GetDefaultRowSize();
    int      GetRowSize( int row );
    int      GetDefaultColSize();
    int      GetColSize( int col );
    wxColour GetDefaultCellBackgroundColour();
    wxColour GetCellBackgroundColour( int row, int col );
    wxColour GetDefaultCellTextColour();
    wxColour GetCellTextColour( int row, int col );
    wxFont   GetDefaultCellFont();
    wxFont   GetCellFont( int row, int col );
    void     GetDefaultCellAlignment( int *horiz, int *vert );
    void     GetCellAlignment( int row, int col, int *horiz, int *vert );

    void     SetDefaultRowSize( int height, bool resizeExistingRows = FALSE );
    void     SetRowSize( int row, int height );
    void     SetDefaultColSize( int width, bool resizeExistingCols = FALSE );

    void     SetColSize( int col, int width );

    // automatically size the column or row to fit to its contents, if
    // setAsMin is TRUE, this optimal width will also be set as minimal width
    // for this column
    void     AutoSizeColumn( int col, bool setAsMin = TRUE )
        { AutoSizeColOrRow(col, setAsMin, TRUE); }
    void     AutoSizeRow( int row, bool setAsMin = TRUE )
        { AutoSizeColOrRow(row, setAsMin, FALSE); }

    // auto size all columns (very ineffective for big grids!)
    void     AutoSizeColumns( bool setAsMin = TRUE )
        { (void)SetOrCalcColumnSizes(FALSE, setAsMin); }

    void     AutoSizeRows( bool setAsMin = TRUE )
        { (void)SetOrCalcRowSizes(FALSE, setAsMin); }

    // auto size the grid, that is make the columns/rows of the "right" size
    // and also set the grid size to just fit its contents
    void     AutoSize();

    // column won't be resized to be lesser width - this must be called during
    // the grid creation because it won't resize the column if it's already
    // narrower than the minimal width
    void     SetColMinimalWidth( int col, int width );
    void     SetRowMinimalHeight( int row, int width );

    void     SetDefaultCellBackgroundColour( const wxColour& );
    void     SetCellBackgroundColour( int row, int col, const wxColour& );
    void     SetDefaultCellTextColour( const wxColour& );

    void     SetCellTextColour( int row, int col, const wxColour& );
    void     SetDefaultCellFont( const wxFont& );
    void     SetCellFont( int row, int col, const wxFont& );
    void     SetDefaultCellAlignment( int horiz, int vert );
    void     SetCellAlignment( int row, int col, int horiz, int vert );

    // takes ownership of the pointer
    void SetDefaultRenderer(wxGridCellRenderer *renderer);
    void SetCellRenderer(int row, int col, wxGridCellRenderer *renderer);
    wxGridCellRenderer *GetDefaultRenderer() const;
    wxGridCellRenderer* GetCellRenderer(int row, int col);

    // takes ownership of the pointer
    void SetDefaultEditor(wxGridCellEditor *editor);
    void SetCellEditor(int row, int col, wxGridCellEditor *editor);
    wxGridCellEditor *GetDefaultEditor() const;
    wxGridCellEditor* GetCellEditor(int row, int col);



    // ------ cell value accessors
    //
    wxString GetCellValue( int row, int col )
    {
        if ( m_table )
        {
            return m_table->GetValue( row, col );
        }
        else
        {
            return wxEmptyString;
        }
    }

    wxString GetCellValue( const wxGridCellCoords& coords )
        { return GetCellValue( coords.GetRow(), coords.GetCol() ); }

    void SetCellValue( int row, int col, const wxString& s );
    void SetCellValue( const wxGridCellCoords& coords, const wxString& s )
        { SetCellValue( coords.GetRow(), coords.GetCol(), s ); }

    // returns TRUE if the cell can't be edited
    bool IsReadOnly(int row, int col) const;

    // make the cell editable/readonly
    void SetReadOnly(int row, int col, bool isReadOnly = TRUE);

    // ------ selections of blocks of cells
    //
    void SelectRow( int row, bool addToSelected = FALSE );
    void SelectCol( int col, bool addToSelected = FALSE );

    void SelectBlock( int topRow, int leftCol, int bottomRow, int rightCol );

    void SelectBlock( const wxGridCellCoords& topLeft,
                      const wxGridCellCoords& bottomRight )
        { SelectBlock( topLeft.GetRow(), topLeft.GetCol(),
                       bottomRight.GetRow(), bottomRight.GetCol() ); }

    void SelectAll();

    bool IsSelection();

    void ClearSelection();

    bool IsInSelection( int row, int col );

    bool IsInSelection( const wxGridCellCoords& coords )
        { return IsInSelection( coords.GetRow(), coords.GetCol() ); }


    // This function returns the rectangle that encloses the block of cells
    // limited by TopLeft and BottomRight cell in device coords and clipped
    //  to the client size of the grid window.
    //
    wxRect BlockToDeviceRect( const wxGridCellCoords & topLeft,
                              const wxGridCellCoords & bottomRight );

    // This function returns the rectangle that encloses the selected cells
    // in device coords and clipped to the client size of the grid window.
    //
    wxRect SelectionToDeviceRect()
        {
            return BlockToDeviceRect( m_selectingTopLeft,
                                      m_selectingBottomRight );
        }

    // Access or update the selection fore/back colours
    wxColour GetSelectionBackground() const
        { return m_selectionBackground; }
    wxColour GetSelectionForeground() const
        { return m_selectionForeground; }

    void SetSelectionBackground(const wxColour& c) { m_selectionBackground = c; }
    void SetSelectionForeground(const wxColour& c) { m_selectionForeground = c; }


    // Methods for a registry for mapping data types to Renderers/Editors
    void RegisterDataType(const wxString& typeName,
                          wxGridCellRenderer* renderer,
                          wxGridCellEditor* editor);
    wxGridCellEditor* GetDefaultEditorForCell(int row, int col) const;
    wxGridCellEditor* GetDefaultEditorForCell(const wxGridCellCoords& c) const
        { return GetDefaultEditorForCell(c.GetRow(), c.GetCol()); }
    wxGridCellRenderer* GetDefaultRendererForCell(int row, int col) const;
    wxGridCellEditor* GetDefaultEditorForType(const wxString& typeName) const;
    wxGridCellRenderer* GetDefaultRendererForType(const wxString& typeName) const;

    // grid may occupy more space than needed for its rows/columns, this
    // function allows to set how big this extra space is
    void SetMargins(int extraWidth, int extraHeight)
    {
        m_extraWidth = extraWidth;
        m_extraHeight = extraHeight;
    }

    // ------ For compatibility with previous wxGrid only...
    //
    //  ************************************************
    //  **  Don't use these in new code because they  **
    //  **  are liable to disappear in a future       **
    //  **  revision                                  **
    //  ************************************************
    //

    wxGrid( wxWindow *parent,
            int x, int y, int w = -1, int h = -1,
            long style = wxWANTS_CHARS,
            const wxString& name = wxPanelNameStr )
        : wxScrolledWindow( parent, -1, wxPoint(x,y), wxSize(w,h), 
                            (style|wxWANTS_CHARS), name )
        {
            Create();
        }

    void SetCellValue( const wxString& val, int row, int col )
        { SetCellValue( row, col, val ); }

    void UpdateDimensions()
        { CalcDimensions(); }

    int GetRows() { return GetNumberRows(); }
    int GetCols() { return GetNumberCols(); }
    int GetCursorRow() { return GetGridCursorRow(); }
    int GetCursorColumn() { return GetGridCursorCol(); }

    int GetScrollPosX() { return 0; }
    int GetScrollPosY() { return 0; }

    void SetScrollX( int WXUNUSED(x) ) { }
    void SetScrollY( int WXUNUSED(y) ) { }

    void SetColumnWidth( int col, int width )
        { SetColSize( col, width ); }

    int GetColumnWidth( int col )
        { return GetColSize( col ); }

    void SetRowHeight( int row, int height )
        { SetRowSize( row, height ); }

    // GetRowHeight() is below

    int GetViewHeight() // returned num whole rows visible
        { return 0; }

    int GetViewWidth() // returned num whole cols visible
        { return 0; }

    void SetLabelSize( int orientation, int sz )
        {
            if ( orientation == wxHORIZONTAL )
                SetColLabelSize( sz );
            else
                SetRowLabelSize( sz );
        }

    int GetLabelSize( int orientation )
        {
            if ( orientation == wxHORIZONTAL )
                return GetColLabelSize();
            else
                return GetRowLabelSize();
        }

    void SetLabelAlignment( int orientation, int align )
        {
            if ( orientation == wxHORIZONTAL )
                SetColLabelAlignment( align, -1 );
            else
                SetRowLabelAlignment( align, -1 );
        }

    int GetLabelAlignment( int orientation, int WXUNUSED(align) )
        {
            int h, v;
            if ( orientation == wxHORIZONTAL )
            {
                GetColLabelAlignment( &h, &v );
                return h;
            }
            else
            {
                GetRowLabelAlignment( &h, &v );
                return h;
            }
        }

    void SetLabelValue( int orientation, const wxString& val, int pos )
        {
            if ( orientation == wxHORIZONTAL )
                SetColLabelValue( pos, val );
            else
                SetRowLabelValue( pos, val );
        }

    wxString GetLabelValue( int orientation, int pos)
        {
            if ( orientation == wxHORIZONTAL )
                return GetColLabelValue( pos );
            else
                return GetRowLabelValue( pos );
        }

    wxFont GetCellTextFont() const
        { return m_defaultCellAttr->GetFont(); }

    wxFont GetCellTextFont(int WXUNUSED(row), int WXUNUSED(col)) const
        { return m_defaultCellAttr->GetFont(); }

    void SetCellTextFont(const wxFont& fnt)
        { SetDefaultCellFont( fnt ); }

    void SetCellTextFont(const wxFont& fnt, int row, int col)
        { SetCellFont( row, col, fnt ); }

    void SetCellTextColour(const wxColour& val, int row, int col)
        { SetCellTextColour( row, col, val ); }

    void SetCellTextColour(const wxColour& col)
        { SetDefaultCellTextColour( col ); }

    void SetCellBackgroundColour(const wxColour& col)
        { SetDefaultCellBackgroundColour( col ); }

    void SetCellBackgroundColour(const wxColour& colour, int row, int col)
        { SetCellBackgroundColour( row, col, colour ); }

    bool GetEditable() { return IsEditable(); }
    void SetEditable( bool edit = TRUE ) { EnableEditing( edit ); }
    bool GetEditInPlace() { return IsCellEditControlEnabled(); }

    void SetEditInPlace(bool WXUNUSED(edit) = TRUE) { }

    void SetCellAlignment( int align, int row, int col)
    { SetCellAlignment(row, col, align, wxCENTER); }
    void SetCellAlignment( int WXUNUSED(align) ) {}
    void SetCellBitmap(wxBitmap *WXUNUSED(bitmap), int WXUNUSED(row), int WXUNUSED(col))
    { }
    void SetDividerPen(const wxPen& WXUNUSED(pen)) { }
    wxPen& GetDividerPen() const { return wxNullPen; }
    void OnActivate(bool WXUNUSED(active)) {}

    // ******** End of compatibility functions **********



    // ------ control IDs
    enum { wxGRID_CELLCTRL = 2000,
           wxGRID_TOPCTRL };

    // ------ control types
    enum { wxGRID_TEXTCTRL = 2100,
           wxGRID_CHECKBOX,
           wxGRID_CHOICE,
           wxGRID_COMBOBOX };

    // overridden wxWindow methods
    virtual void Fit();

protected:
    virtual wxSize DoGetBestSize() const;

    bool m_created;

    wxGridWindow             *m_gridWin;
    wxGridRowLabelWindow     *m_rowLabelWin;
    wxGridColLabelWindow     *m_colLabelWin;
    wxGridCornerLabelWindow  *m_cornerLabelWin;

    wxGridTableBase          *m_table;
    bool                      m_ownTable;

    int m_left;
    int m_top;
    int m_right;
    int m_bottom;

    int m_numRows;
    int m_numCols;

    wxGridCellCoords m_currentCellCoords;

    wxGridCellCoords m_selectingTopLeft;
    wxGridCellCoords m_selectingBottomRight;
    wxGridCellCoords m_selectingKeyboard;
    wxGridSelection  *m_selection;
    wxColour    m_selectionBackground;
    wxColour    m_selectionForeground;

    // NB: *never* access m_row/col arrays directly because they are created
    //     on demand, *always* use accessor functions instead!

    // init the m_rowHeights/Bottoms arrays with default values
    void InitRowHeights();

    int        m_defaultRowHeight;
    wxArrayInt m_rowHeights;
    wxArrayInt m_rowBottoms;

    // init the m_colWidths/Rights arrays
    void InitColWidths();

    int        m_defaultColWidth;
    wxArrayInt m_colWidths;
    wxArrayInt m_colRights;

    // get the col/row coords
    int GetColWidth(int col) const;
    int GetColLeft(int col) const;
    int GetColRight(int col) const;

    // this function must be public for compatibility...
public:
    int GetRowHeight(int row) const;
protected:

    int GetRowTop(int row) const;
    int GetRowBottom(int row) const;

    int m_rowLabelWidth;
    int m_colLabelHeight;

    // the size of the margin left to the right and bottom of the cell area
    int m_extraWidth,
        m_extraHeight;

    wxColour   m_labelBackgroundColour;
    wxColour   m_labelTextColour;
    wxFont     m_labelFont;

    int        m_rowLabelHorizAlign;
    int        m_rowLabelVertAlign;
    int        m_colLabelHorizAlign;
    int        m_colLabelVertAlign;

    bool       m_defaultRowLabelValues;
    bool       m_defaultColLabelValues;

    wxColour   m_gridLineColour;
    bool       m_gridLinesEnabled;

    // common part of AutoSizeColumn/Row() and GetBestSize()
    int SetOrCalcColumnSizes(bool calcOnly, bool setAsMin = TRUE);
    int SetOrCalcRowSizes(bool calcOnly, bool setAsMin = TRUE);

    // common part of AutoSizeColumn/Row()
    void AutoSizeColOrRow(int n, bool setAsMin, bool column /* or row? */);

    // if a column has a minimal width, it will be the value for it in this
    // hash table
    wxHashTableLong m_colMinWidths,
                    m_rowMinHeights;

    // get the minimal width of the given column/row
    int GetColMinimalWidth(int col) const;
    int GetRowMinimalHeight(int col) const;

    // do we have some place to store attributes in?
    bool CanHaveAttributes();

    // returns the attribute we may modify in place: a new one if this cell
    // doesn't have any yet or the existing one if it does
    //
    // DecRef() must be called on the returned pointer, as usual
    wxGridCellAttr *GetOrCreateCellAttr(int row, int col) const;

    // cell attribute cache (currently we only cache 1, may be will do
    // more/better later)
    struct CachedAttr
    {
        int             row, col;
        wxGridCellAttr *attr;
    } m_attrCache;

    // invalidates the attribute cache
    void ClearAttrCache();

    // adds an attribute to cache
    void CacheAttr(int row, int col, wxGridCellAttr *attr) const;

    // looks for an attr in cache, returns TRUE if found
    bool LookupAttr(int row, int col, wxGridCellAttr **attr) const;

    // looks for the attr in cache, if not found asks the table and caches the
    // result
    wxGridCellAttr *GetCellAttr(int row, int col) const;
    wxGridCellAttr *GetCellAttr(const wxGridCellCoords& coords )
        { return GetCellAttr( coords.GetRow(), coords.GetCol() ); }

    // the default cell attr object for cells that don't have their own
    wxGridCellAttr*     m_defaultCellAttr;


    wxGridCellCoordsArray  m_cellsExposed;
    wxArrayInt             m_rowsExposed;
    wxArrayInt             m_colsExposed;
    wxArrayInt             m_rowLabelsExposed;
    wxArrayInt             m_colLabelsExposed;

    bool m_inOnKeyDown;
    int  m_batchCount;


    wxGridTypeRegistry*    m_typeRegistry;

    enum CursorMode
    {
        WXGRID_CURSOR_SELECT_CELL,
        WXGRID_CURSOR_RESIZE_ROW,
        WXGRID_CURSOR_RESIZE_COL,
        WXGRID_CURSOR_SELECT_ROW,
        WXGRID_CURSOR_SELECT_COL
    };

    // this method not only sets m_cursorMode but also sets the correct cursor
    // for the given mode and, if captureMouse is not FALSE releases the mouse
    // if it was captured and captures it if it must be captured
    //
    // for this to work, you should always use it and not set m_cursorMode
    // directly!
    void ChangeCursorMode(CursorMode mode,
                          wxWindow *win = (wxWindow *)NULL,
                          bool captureMouse = TRUE);

    wxWindow *m_winCapture;     // the window which captured the mouse
    CursorMode m_cursorMode;

    bool    m_canDragRowSize;
    bool    m_canDragColSize;
    bool    m_canDragGridSize;
    int     m_dragLastPos;
    int     m_dragRowOrCol;
    bool    m_isDragging;
    wxPoint m_startDragPos;

    bool    m_waitForSlowClick;

    wxGridCellCoords m_selectionStart;

    wxCursor m_rowResizeCursor;
    wxCursor m_colResizeCursor;

    bool       m_editable;              // applies to whole grid
    bool       m_cellEditCtrlEnabled;   // is in-place edit currently shown?


    void Create();
    void Init();
    void CalcDimensions();
    void CalcWindowSizes();
    bool Redimension( wxGridTableMessage& );


    bool SendEvent( const wxEventType, int row, int col, wxMouseEvent& );
    bool SendEvent( const wxEventType, int row, int col );
    bool SendEvent( const wxEventType type)
    {
        return SendEvent(type,
                         m_currentCellCoords.GetRow(),
                         m_currentCellCoords.GetCol());
    }

    void OnPaint( wxPaintEvent& );
    void OnSize( wxSizeEvent& );
    void OnKeyDown( wxKeyEvent& );
    void OnEraseBackground( wxEraseEvent& );


    void SetCurrentCell( const wxGridCellCoords& coords );
    void SetCurrentCell( int row, int col )
        { SetCurrentCell( wxGridCellCoords(row, col) ); }


    // ------ functions to get/send data (see also public functions)
    //
    bool GetModelValues();
    bool SetModelValues();

        friend class wxGridSelection;

    DECLARE_DYNAMIC_CLASS( wxGrid )
    DECLARE_EVENT_TABLE()
};

// ----------------------------------------------------------------------------
// Grid event class and event types
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxGridEvent : public wxNotifyEvent
{
public:
    wxGridEvent()
        : wxNotifyEvent(), m_row(-1), m_col(-1), m_x(-1), m_y(-1),
        m_selecting(0), m_control(0), m_meta(0), m_shift(0), m_alt(0)
        {
        }

    wxGridEvent(int id, wxEventType type, wxObject* obj,
                int row=-1, int col=-1, int x=-1, int y=-1, bool sel = TRUE,
                bool control=FALSE, bool shift=FALSE, bool alt=FALSE, bool meta=FALSE);

    virtual int GetRow() { return m_row; }
    virtual int GetCol() { return m_col; }
    wxPoint     GetPosition() { return wxPoint( m_x, m_y ); }
    bool        Selecting() { return m_selecting; }
    bool        ControlDown() { return m_control; }
    bool        MetaDown() { return m_meta; }
    bool        ShiftDown() { return m_shift; }
    bool        AltDown() { return m_alt; }

protected:
    int         m_row;
    int         m_col;
    int         m_x;
    int         m_y;
    bool        m_selecting;
    bool        m_control;
    bool        m_meta;
    bool        m_shift;
    bool        m_alt;

    DECLARE_DYNAMIC_CLASS(wxGridEvent)
};

class WXDLLEXPORT wxGridSizeEvent : public wxNotifyEvent
{
public:
    wxGridSizeEvent()
        : wxNotifyEvent(), m_rowOrCol(-1), m_x(-1), m_y(-1),
        m_control(0), m_meta(0), m_shift(0), m_alt(0)
        {
        }

    wxGridSizeEvent(int id, wxEventType type, wxObject* obj,
                int rowOrCol=-1, int x=-1, int y=-1,
                bool control=FALSE, bool shift=FALSE, bool alt=FALSE, bool meta=FALSE);

    int         GetRowOrCol() { return m_rowOrCol; }
    wxPoint     GetPosition() { return wxPoint( m_x, m_y ); }
    bool        ControlDown() { return m_control; }
    bool        MetaDown() { return m_meta; }
    bool        ShiftDown() { return m_shift; }
    bool        AltDown() { return m_alt; }

protected:
    int         m_rowOrCol;
    int         m_x;
    int         m_y;
    bool        m_control;
    bool        m_meta;
    bool        m_shift;
    bool        m_alt;

    DECLARE_DYNAMIC_CLASS(wxGridSizeEvent)
};


class WXDLLEXPORT wxGridRangeSelectEvent : public wxNotifyEvent
{
public:
    wxGridRangeSelectEvent()
        : wxNotifyEvent()
        {
            m_topLeft     = wxGridNoCellCoords;
            m_bottomRight = wxGridNoCellCoords;
            m_selecting   = FALSE;
            m_control     = FALSE;
            m_meta        = FALSE;
            m_shift       = FALSE;
            m_alt         = FALSE;
        }

    wxGridRangeSelectEvent(int id, wxEventType type, wxObject* obj,
                           const wxGridCellCoords& topLeft,
                           const wxGridCellCoords& bottomRight,
                           bool sel = TRUE,
                           bool control=FALSE, bool shift=FALSE,
                           bool alt=FALSE, bool meta=FALSE);

    wxGridCellCoords GetTopLeftCoords() { return m_topLeft; }
    wxGridCellCoords GetBottomRightCoords() { return m_bottomRight; }
    int         GetTopRow()    { return m_topLeft.GetRow(); }
    int         GetBottomRow() { return m_bottomRight.GetRow(); }
    int         GetLeftCol()   { return m_topLeft.GetCol(); }
    int         GetRightCol()  { return m_bottomRight.GetCol(); }
    bool        Selecting() { return m_selecting; }
    bool        ControlDown()  { return m_control; }
    bool        MetaDown()     { return m_meta; }
    bool        ShiftDown()    { return m_shift; }
    bool        AltDown()      { return m_alt; }

protected:
    wxGridCellCoords  m_topLeft;
    wxGridCellCoords  m_bottomRight;
    bool              m_selecting;
    bool              m_control;
    bool              m_meta;
    bool              m_shift;
    bool              m_alt;

    DECLARE_DYNAMIC_CLASS(wxGridRangeSelectEvent)
};

// TODO move to wx/event.h
const wxEventType wxEVT_GRID_CELL_LEFT_CLICK    = wxEVT_FIRST + 1580;
const wxEventType wxEVT_GRID_CELL_RIGHT_CLICK   = wxEVT_FIRST + 1581;
const wxEventType wxEVT_GRID_CELL_LEFT_DCLICK   = wxEVT_FIRST + 1582;
const wxEventType wxEVT_GRID_CELL_RIGHT_DCLICK  = wxEVT_FIRST + 1583;
const wxEventType wxEVT_GRID_LABEL_LEFT_CLICK   = wxEVT_FIRST + 1584;
const wxEventType wxEVT_GRID_LABEL_RIGHT_CLICK  = wxEVT_FIRST + 1585;
const wxEventType wxEVT_GRID_LABEL_LEFT_DCLICK  = wxEVT_FIRST + 1586;
const wxEventType wxEVT_GRID_LABEL_RIGHT_DCLICK = wxEVT_FIRST + 1587;
const wxEventType wxEVT_GRID_ROW_SIZE           = wxEVT_FIRST + 1588;
const wxEventType wxEVT_GRID_COL_SIZE           = wxEVT_FIRST + 1589;
const wxEventType wxEVT_GRID_RANGE_SELECT       = wxEVT_FIRST + 1590;
const wxEventType wxEVT_GRID_CELL_CHANGE        = wxEVT_FIRST + 1591;
const wxEventType wxEVT_GRID_SELECT_CELL        = wxEVT_FIRST + 1592;
const wxEventType wxEVT_GRID_EDITOR_SHOWN       = wxEVT_FIRST + 1593;
const wxEventType wxEVT_GRID_EDITOR_HIDDEN      = wxEVT_FIRST + 1594;


typedef void (wxEvtHandler::*wxGridEventFunction)(wxGridEvent&);
typedef void (wxEvtHandler::*wxGridSizeEventFunction)(wxGridSizeEvent&);
typedef void (wxEvtHandler::*wxGridRangeSelectEventFunction)(wxGridRangeSelectEvent&);

#define EVT_GRID_CELL_LEFT_CLICK(fn)     { wxEVT_GRID_CELL_LEFT_CLICK,    -1, -1, (wxObjectEventFunction) (wxEventFunction) (wxGridEventFunction) &fn, NULL },
#define EVT_GRID_CELL_RIGHT_CLICK(fn)    { wxEVT_GRID_CELL_RIGHT_CLICK,   -1, -1, (wxObjectEventFunction) (wxEventFunction) (wxGridEventFunction) &fn, NULL },
#define EVT_GRID_CELL_LEFT_DCLICK(fn)    { wxEVT_GRID_CELL_LEFT_DCLICK,   -1, -1, (wxObjectEventFunction) (wxEventFunction) (wxGridEventFunction) &fn, NULL },
#define EVT_GRID_CELL_RIGHT_DCLICK(fn)   { wxEVT_GRID_CELL_RIGHT_DCLICK,  -1, -1, (wxObjectEventFunction) (wxEventFunction) (wxGridEventFunction) &fn, NULL },
#define EVT_GRID_LABEL_LEFT_CLICK(fn)    { wxEVT_GRID_LABEL_LEFT_CLICK,   -1, -1, (wxObjectEventFunction) (wxEventFunction) (wxGridEventFunction) &fn, NULL },
#define EVT_GRID_LABEL_RIGHT_CLICK(fn)   { wxEVT_GRID_LABEL_RIGHT_CLICK,  -1, -1, (wxObjectEventFunction) (wxEventFunction) (wxGridEventFunction) &fn, NULL },
#define EVT_GRID_LABEL_LEFT_DCLICK(fn)   { wxEVT_GRID_LABEL_LEFT_DCLICK,  -1, -1, (wxObjectEventFunction) (wxEventFunction) (wxGridEventFunction) &fn, NULL },
#define EVT_GRID_LABEL_RIGHT_DCLICK(fn)  { wxEVT_GRID_LABEL_RIGHT_DCLICK, -1, -1, (wxObjectEventFunction) (wxEventFunction) (wxGridEventFunction) &fn, NULL },
#define EVT_GRID_ROW_SIZE(fn)            { wxEVT_GRID_ROW_SIZE,           -1, -1, (wxObjectEventFunction) (wxEventFunction) (wxGridSizeEventFunction) &fn, NULL },
#define EVT_GRID_COL_SIZE(fn)            { wxEVT_GRID_COL_SIZE,           -1, -1, (wxObjectEventFunction) (wxEventFunction) (wxGridSizeEventFunction) &fn, NULL },
#define EVT_GRID_RANGE_SELECT(fn)        { wxEVT_GRID_RANGE_SELECT,       -1, -1, (wxObjectEventFunction) (wxEventFunction) (wxGridRangeSelectEventFunction) &fn, NULL },
#define EVT_GRID_CELL_CHANGE(fn)         { wxEVT_GRID_CELL_CHANGE,        -1, -1, (wxObjectEventFunction) (wxEventFunction) (wxGridEventFunction) &fn, NULL },
#define EVT_GRID_SELECT_CELL(fn)         { wxEVT_GRID_SELECT_CELL,        -1, -1, (wxObjectEventFunction) (wxEventFunction) (wxGridEventFunction) &fn, NULL },
#define EVT_GRID_EDITOR_SHOWN(fn)        { wxEVT_GRID_EDITOR_SHOWN,       -1, -1, (wxObjectEventFunction) (wxEventFunction) (wxGridEventFunction) &fn, NULL },
#define EVT_GRID_EDITOR_HIDDEN(fn)       { wxEVT_GRID_EDITOR_HIDDEN,      -1, -1, (wxObjectEventFunction) (wxEventFunction) (wxGridEventFunction) &fn, NULL },


#if 0  // TODO: implement these ?  others ?

const wxEventType wxEVT_GRID_CREATE_CELL      = wxEVT_FIRST + 1576;
const wxEventType wxEVT_GRID_CHANGE_LABELS    = wxEVT_FIRST + 1577;
const wxEventType wxEVT_GRID_CHANGE_SEL_LABEL = wxEVT_FIRST + 1578;

#define EVT_GRID_CREATE_CELL(fn)      { wxEVT_GRID_CREATE_CELL,      -1, -1, (wxObjectEventFunction) (wxEventFunction) (wxGridEventFunction) &fn, NULL },
#define EVT_GRID_CHANGE_LABELS(fn)    { wxEVT_GRID_CHANGE_LABELS,    -1, -1, (wxObjectEventFunction) (wxEventFunction) (wxGridEventFunction) &fn, NULL },
#define EVT_GRID_CHANGE_SEL_LABEL(fn) { wxEVT_GRID_CHANGE_SEL_LABEL, -1, -1, (wxObjectEventFunction) (wxEventFunction) (wxGridEventFunction) &fn, NULL },

#endif

#endif  // #ifndef __WXGRID_H__

#endif  // ifndef wxUSE_NEW_GRID
