/////////////////////////////////////////////////////////////////////////////
// Name:        grid.h
// Purpose:     interface of wxGridCellFloatRenderer
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxGridCellFloatRenderer
    @wxheader{grid.h}

    This class may be used to format floating point data in a cell.

    @library{wxadv}
    @category{FIXME}

    @see wxGridCellRenderer, wxGridCellNumberRenderer, wxGridCellStringRenderer,
    wxGridCellBoolRenderer
*/
class wxGridCellFloatRenderer : public wxGridCellStringRenderer
{
public:
    /**
        @param width
            Minimum number of characters to be shown.
        @param precision
            Number of digits after the decimal dot.
    */
    wxGridCellFloatRenderer(int width = -1, int precision = -1);

    /**
        Returns the precision ( see @ref constr() wxGridCellFloatRenderer ).
    */
    int GetPrecision() const;

    /**
        Returns the width ( see @ref constr() wxGridCellFloatRenderer ).
    */
    int GetWidth() const;

    /**
        Parameters string format is "width[,precision]".
    */
    void SetParameters(const wxString& params);

    /**
        Sets the precision ( see @ref constr() wxGridCellFloatRenderer ).
    */
    void SetPrecision(int precision);

    /**
        Sets the width ( see @ref constr() wxGridCellFloatRenderer )
    */
    void SetWidth(int width);
};



/**
    @class wxGridTableBase
    @wxheader{grid.h}

    Grid table classes.

    @library{wxadv}
    @category{FIXME}
*/
class wxGridTableBase : public wxObject
{
public:
    /**
        
    */
    wxGridTableBase();

    /**
        
    */
    ~wxGridTableBase();

    /**
        
    */
    bool AppendCols(size_t numCols = 1);

    /**
        
    */
    bool AppendRows(size_t numRows = 1);

    /**
        
    */
    bool CanGetValueAs(int row, int col, const wxString& typeName);

    /**
        Does this table allow attributes?  Default implementation creates
        a wxGridCellAttrProvider if necessary.
    */
    bool CanHaveAttributes();

    /**
        
    */
    bool CanSetValueAs(int row, int col, const wxString& typeName);

    /**
        
    */
    void Clear();

    /**
        
    */
    bool DeleteCols(size_t pos = 0, size_t numCols = 1);

    /**
        
    */
    bool DeleteRows(size_t pos = 0, size_t numRows = 1);

    /**
        by default forwarded to wxGridCellAttrProvider if any. May be
        overridden to handle attributes directly in the table.
    */
    wxGridCellAttr* GetAttr(int row, int col);

    /**
        get the currently used attr provider (may be @NULL)
    */
    wxGridCellAttrProvider* GetAttrProvider() const;

    /**
        
    */
    wxString GetColLabelValue(int col);

    /**
        
    */
    int GetNumberCols();

    /**
        You must override these functions in a derived table class.
    */
    int GetNumberRows();

    /**
        
    */
    wxString GetRowLabelValue(int row);

    /**
        Data type determination and value access.
    */
    wxString GetTypeName(int row, int col);

    /**
        
    */
    wxString GetValue(int row, int col);

    /**
        
    */
    bool GetValueAsBool(int row, int col);

    /**
        For user defined types
    */
    void* GetValueAsCustom(int row, int col,
                           const wxString& typeName);

    /**
        
    */
    double GetValueAsDouble(int row, int col);

    /**
        
    */
    long GetValueAsLong(int row, int col);

    /**
        
    */
    wxGrid* GetView() const;

    /**
        
    */
    bool InsertCols(size_t pos = 0, size_t numCols = 1);

    /**
        
    */
    bool InsertRows(size_t pos = 0, size_t numRows = 1);

    /**
        
    */
    bool IsEmptyCell(int row, int col);

    /**
        these functions take ownership of the pointer
    */
    void SetAttr(wxGridCellAttr* attr, int row, int col);

    /**
        Attribute handling
        give us the attr provider to use - we take ownership of the pointer
    */
    void SetAttrProvider(wxGridCellAttrProvider* attrProvider);

    /**
        
    */
    void SetColAttr(wxGridCellAttr* attr, int col);

    /**
        , @e wxString)
    */
    void SetColLabelValue() const;

    /**
        
    */
    void SetRowAttr(wxGridCellAttr* attr, int row);

    /**
        , @e wxString)
    */
    void SetRowLabelValue() const;

    /**
        
    */
    void SetValue(int row, int col, const wxString& value);

    /**
        
    */
    void SetValueAsBool(int row, int col, bool value);

    /**
        
    */
    void SetValueAsCustom(int row, int col, const wxString& typeName,
                          void* value);

    /**
        
    */
    void SetValueAsDouble(int row, int col, double value);

    /**
        
    */
    void SetValueAsLong(int row, int col, long value);

    /**
        Overriding these is optional
    */
    void SetView(wxGrid* grid);

    /**
        
    */
    void UpdateAttrCols(size_t pos, int numCols);

    /**
        change row/col number in attribute if needed
    */
    void UpdateAttrRows(size_t pos, int numRows);
};



/**
    @class wxGridCellEditor
    @wxheader{grid.h}

    This class is responsible for providing and manipulating
    the in-place edit controls for the grid.  Instances of wxGridCellEditor
    (actually, instances of derived classes since it is an abstract class) can be
    associated with the cell attributes for individual cells, rows, columns, or
    even for the entire grid.

    @library{wxadv}
    @category{FIXME}

    @see wxGridCellTextEditor, wxGridCellFloatEditor, wxGridCellBoolEditor,
    wxGridCellNumberEditor, wxGridCellChoiceEditor
*/
class wxGridCellEditor
{
public:
    /**
        
    */
    wxGridCellEditor();

    /**
        The dtor is private because only DecRef() can delete us.
    */
    ~wxGridCellEditor();

    /**
        Fetch the value from the table and prepare the edit control
        to begin editing. Set the focus to the edit control.
    */
    void BeginEdit(int row, int col, wxGrid* grid);

    /**
        Create a new object which is the copy of this one.
    */
    wxGridCellEditor* Clone() const;

    /**
        Creates the actual edit control.
    */
    void Create(wxWindow* parent, wxWindowID id,
                wxEvtHandler* evtHandler);

    /**
        Final cleanup.
    */
    void Destroy();

    /**
        Complete the editing of the current cell. Returns @true if the value has
        changed. If necessary, the control may be destroyed.
    */
    bool EndEdit(int row, int col, wxGrid* grid);

    /**
        Some types of controls on some platforms may need some help
        with the Return key.
    */
    void HandleReturn(wxKeyEvent& event);

    /**
        
    */
    bool IsCreated();

    /**
        Draws the part of the cell not occupied by the control: the base class
        version just fills it with background colour from the attribute.
    */
    void PaintBackground(const wxRect& rectCell,
                         wxGridCellAttr* attr);

    /**
        Reset the value in the control back to its starting value.
    */
    void Reset();

    /**
        Size and position the edit control.
    */
    void SetSize(const wxRect& rect);

    /**
        Show or hide the edit control, use the specified attributes to set
        colours/fonts for it.
    */
    void Show(bool show, wxGridCellAttr* attr = NULL);

    /**
        If the editor is enabled by clicking on the cell, this method will be
        called.
    */
    void StartingClick();

    /**
        If the editor is enabled by pressing keys on the grid,
        this will be called to let the editor do something about
        that first key if desired.
    */
    void StartingKey(wxKeyEvent& event);
};



/**
    @class wxGridCellTextEditor
    @wxheader{grid.h}

    The editor for string/text data.

    @library{wxadv}
    @category{FIXME}

    @see wxGridCellEditor, wxGridCellFloatEditor, wxGridCellBoolEditor,
    wxGridCellNumberEditor, wxGridCellChoiceEditor
*/
class wxGridCellTextEditor : public wxGridCellEditor
{
public:
    /**
        Default constructor.
    */
    wxGridCellTextEditor();

    /**
        The parameters string format is "n" where n is a number representing the
        maximum width.
    */
    void SetParameters(const wxString& params);
};



/**
    @class wxGridCellStringRenderer
    @wxheader{grid.h}

    This class may be used to format string data in a cell; it is the default
    for string cells.

    @library{wxadv}
    @category{FIXME}

    @see wxGridCellRenderer, wxGridCellNumberRenderer, wxGridCellFloatRenderer,
    wxGridCellBoolRenderer
*/
class wxGridCellStringRenderer : public wxGridCellRenderer
{
public:
    /**
        Default constructor
    */
    wxGridCellStringRenderer();
};



/**
    @class wxGridCellChoiceEditor
    @wxheader{grid.h}

    The editor for string data allowing to choose from a list of strings.

    @library{wxadv}
    @category{FIXME}

    @see wxGridCellEditor, wxGridCellFloatEditor, wxGridCellBoolEditor,
    wxGridCellTextEditor, wxGridCellNumberEditor
*/
class wxGridCellChoiceEditor : public wxGridCellEditor
{
public:
    //@{
    /**
        @param count
            Number of strings from which the user can choose.
        @param choices
            An array of strings from which the user can choose.
        @param allowOthers
            If allowOthers is @true, the user can type a string not in choices array.
    */
    wxGridCellChoiceEditor(size_t count = 0,
                           const wxString choices[] = NULL,
                           bool allowOthers = false);
    wxGridCellChoiceEditor(const wxArrayString& choices,
                           bool allowOthers = false);
    //@}

    /**
        Parameters string format is "item1[,item2[...,itemN]]"
    */
    void SetParameters(const wxString& params);
};



/**
    @class wxGridEditorCreatedEvent
    @wxheader{grid.h}


    @library{wxadv}
    @category{FIXME}
*/
class wxGridEditorCreatedEvent : public wxCommandEvent
{
public:
    //@{
    /**
        
    */
    wxGridEditorCreatedEvent();
    wxGridEditorCreatedEvent(int id, wxEventType type,
                             wxObject* obj,
                             int row,
                             int col,
                             wxControl* ctrl);
    //@}

    /**
        Returns the column at which the event occurred.
    */
    int GetCol();

    /**
        Returns the edit control.
    */
    wxControl* GetControl();

    /**
        Returns the row at which the event occurred.
    */
    int GetRow();

    /**
        Sets the column at which the event occurred.
    */
    void SetCol(int col);

    /**
        Sets the edit control.
    */
    void SetControl(wxControl* ctrl);

    /**
        Sets the row at which the event occurred.
    */
    void SetRow(int row);
};



/**
    @class wxGridRangeSelectEvent
    @wxheader{grid.h}


    @library{wxadv}
    @category{FIXME}
*/
class wxGridRangeSelectEvent : public wxNotifyEvent
{
public:
    //@{
    /**
        
    */
    wxGridRangeSelectEvent();
    wxGridRangeSelectEvent(int id, wxEventType type,
                           wxObject* obj,
                           const wxGridCellCoords& topLeft,
                           const wxGridCellCoords& bottomRight,
                           bool sel = true,
                           bool control = false,
                           bool shift = false,
                           bool alt = false,
                           bool meta = false);
    //@}

    /**
        Returns @true if the Alt key was down at the time of the event.
    */
    bool AltDown();

    /**
        Returns @true if the Control key was down at the time of the event.
    */
    bool ControlDown();

    /**
        Top left corner of the rectangular area that was (de)selected.
    */
    wxGridCellCoords GetBottomRightCoords();

    /**
        Bottom row of the rectangular area that was (de)selected.
    */
    int GetBottomRow();

    /**
        Left column of the rectangular area that was (de)selected.
    */
    int GetLeftCol();

    /**
        Right column of the rectangular area that was (de)selected.
    */
    int GetRightCol();

    /**
        Top left corner of the rectangular area that was (de)selected.
    */
    wxGridCellCoords GetTopLeftCoords();

    /**
        Top row of the rectangular area that was (de)selected.
    */
    int GetTopRow();

    /**
        Returns @true if the Meta key was down at the time of the event.
    */
    bool MetaDown();

    /**
        Returns @true if the area was selected, @false otherwise.
    */
    bool Selecting();

    /**
        Returns @true if the Shift key was down at the time of the event.
    */
    bool ShiftDown();
};



/**
    @class wxGridCellRenderer
    @wxheader{grid.h}

    This class is responsible for actually drawing the cell
    in the grid. You may pass it to the wxGridCellAttr (below) to change the
    format of one given cell or to wxGrid::SetDefaultRenderer() to change the
    view of all cells. This is an abstract class, and you will normally use one of
    the
    predefined derived classes or derive your own class from it.

    @library{wxadv}
    @category{FIXME}

    @see wxGridCellStringRenderer, wxGridCellNumberRenderer,
    wxGridCellFloatRenderer, wxGridCellBoolRenderer
*/
class wxGridCellRenderer
{
public:
    /**
        
    */
    wxGridCellRenderer* Clone() const;

    /**
        Draw the given cell on the provided DC inside the given rectangle
        using the style specified by the attribute and the default or selected
        state corresponding to the isSelected value.
        This pure virtual function has a default implementation which will
        prepare the DC using the given attribute: it will draw the rectangle
        with the background colour from attr and set the text colour and font.
    */
    void Draw(wxGrid& grid, wxGridCellAttr& attr, wxDC& dc,
              const wxRect& rect, int row, int col,
              bool isSelected);

    /**
        Get the preferred size of the cell for its contents.
    */
    wxSize GetBestSize(wxGrid& grid, wxGridCellAttr& attr, wxDC& dc,
                       int row, int col);
};



/**
    @class wxGridCellNumberEditor
    @wxheader{grid.h}

    The editor for numeric integer data.

    @library{wxadv}
    @category{FIXME}

    @see wxGridCellEditor, wxGridCellFloatEditor, wxGridCellBoolEditor,
    wxGridCellTextEditor, wxGridCellChoiceEditor
*/
class wxGridCellNumberEditor : public wxGridCellTextEditor
{
public:
    /**
        Allows to specify the range for acceptable data;
        if min == max == -1, no range checking is done
    */
    wxGridCellNumberEditor(int min = -1, int max = -1);

    /**
        String representation of the value.
    */
    wxString GetString() const;

    /**
        If the return value is @true, the editor uses a wxSpinCtrl to get user input,
        otherwise it uses a wxTextCtrl.
    */
    bool HasRange() const;

    /**
        Parameters string format is "min,max".
    */
    void SetParameters(const wxString& params);
};



/**
    @class wxGridSizeEvent
    @wxheader{grid.h}

    This event class contains information about a row/column resize event.

    @library{wxadv}
    @category{FIXME}
*/
class wxGridSizeEvent : public wxNotifyEvent
{
public:
    //@{
    /**
        
    */
    wxGridSizeEvent();
    wxGridSizeEvent(int id, wxEventType type, wxObject* obj,
                    int rowOrCol = -1,
                    int x = -1,
                    int y = -1,
                    bool control = false,
                    bool shift = false,
                    bool alt = false,
                    bool meta = false);
    //@}

    /**
        Returns @true if the Alt key was down at the time of the event.
    */
    bool AltDown();

    /**
        Returns @true if the Control key was down at the time of the event.
    */
    bool ControlDown();

    /**
        Position in pixels at which the event occurred.
    */
    wxPoint GetPosition();

    /**
        Row or column at that was resized.
    */
    int GetRowOrCol();

    /**
        Returns @true if the Meta key was down at the time of the event.
    */
    bool MetaDown();

    /**
        Returns @true if the Shift key was down at the time of the event.
    */
    bool ShiftDown();
};



/**
    @class wxGridCellNumberRenderer
    @wxheader{grid.h}

    This class may be used to format integer data in a cell.

    @library{wxadv}
    @category{FIXME}

    @see wxGridCellRenderer, wxGridCellStringRenderer, wxGridCellFloatRenderer,
    wxGridCellBoolRenderer
*/
class wxGridCellNumberRenderer : public wxGridCellStringRenderer
{
public:
    /**
        Default constructor
    */
    wxGridCellNumberRenderer();
};



/**
    @class wxGridCellAttr
    @wxheader{grid.h}

    This class can be used to alter the cells' appearance in
    the grid by changing their colour/font/... from default. An object of this
    class may be returned by wxGridTableBase::GetAttr.

    @library{wxadv}
    @category{FIXME}
*/
class wxGridCellAttr
{
public:
    //@{
    /**
        Constructor specifying some of the often used attributes.
    */
    wxGridCellAttr();
    wxGridCellAttr(const wxColour& colText,
                   const wxColour& colBack,
                   const wxFont& font,
                   int hAlign, int vAlign);
    //@}

    /**
        Creates a new copy of this object.
    */
    wxGridCellAttr* Clone() const;

    /**
        
    */
    void DecRef();

    /**
        See SetAlignment() for the returned values.
    */
    void GetAlignment(int* hAlign, int* vAlign) const;

    /**
        
    */
    const wxColour GetBackgroundColour() const;

    /**
        
    */
    wxGridCellEditor* GetEditor(wxGrid* grid, int row, int col) const;

    /**
        
    */
    const wxFont GetFont() const;

    /**
        
    */
    wxGridCellRenderer* GetRenderer(wxGrid* grid, int row, int col) const;

    /**
        
    */
    const wxColour GetTextColour() const;

    /**
        
    */
    bool HasAlignment() const;

    /**
        
    */
    bool HasBackgroundColour() const;

    /**
        
    */
    bool HasEditor() const;

    /**
        
    */
    bool HasFont() const;

    /**
        
    */
    bool HasRenderer() const;

    /**
        accessors
    */
    bool HasTextColour() const;

    /**
        This class is ref counted: it is created with ref count of 1, so
        calling DecRef() once will delete it. Calling IncRef() allows to lock
        it until the matching DecRef() is called
    */
    void IncRef();

    /**
        
    */
    bool IsReadOnly() const;

    /**
        Sets the alignment. @a hAlign can be one of @c wxALIGN_LEFT,
        @c wxALIGN_CENTRE or @c wxALIGN_RIGHT and @a vAlign can be one
        of @c wxALIGN_TOP, @c wxALIGN_CENTRE or @c wxALIGN_BOTTOM.
    */
    void SetAlignment(int hAlign, int vAlign);

    /**
        Sets the background colour.
    */
    void SetBackgroundColour(const wxColour& colBack);

    /**
        
    */
    void SetDefAttr(wxGridCellAttr* defAttr);

    /**
        
    */
    void SetEditor(wxGridCellEditor* editor);

    /**
        Sets the font.
    */
    void SetFont(const wxFont& font);

    /**
        
    */
    void SetReadOnly(bool isReadOnly = true);

    /**
        takes ownership of the pointer
    */
    void SetRenderer(wxGridCellRenderer* renderer);

    /**
        Sets the text colour.
    */
    void SetTextColour(const wxColour& colText);
};



/**
    @class wxGridCellBoolRenderer
    @wxheader{grid.h}

    This class may be used to format boolean data in a cell.
    for string cells.

    @library{wxadv}
    @category{FIXME}

    @see wxGridCellRenderer, wxGridCellStringRenderer, wxGridCellFloatRenderer,
    wxGridCellNumberRenderer
*/
class wxGridCellBoolRenderer : public wxGridCellRenderer
{
public:
    /**
        Default constructor
    */
    wxGridCellBoolRenderer();
};



/**
    @class wxGridEvent
    @wxheader{grid.h}

    This event class contains information about various grid events.

    @library{wxadv}
    @category{FIXME}
*/
class wxGridEvent : public wxNotifyEvent
{
public:
    //@{
    /**
        
    */
    wxGridEvent();
    wxGridEvent(int id, wxEventType type, wxObject* obj,
                int row = -1, int col = -1,
                int x = -1, int y = -1,
                bool sel = true,
                bool control = false,
                bool shift = false,
                bool alt = false,
                bool meta = false);
    //@}

    /**
        Returns @true if the Alt key was down at the time of the event.
    */
    bool AltDown();

    /**
        Returns @true if the Control key was down at the time of the event.
    */
    bool ControlDown();

    /**
        Column at which the event occurred.
    */
    int GetCol();

    /**
        Position in pixels at which the event occurred.
    */
    wxPoint GetPosition();

    /**
        Row at which the event occurred.
    */
    int GetRow();

    /**
        Returns @true if the Meta key was down at the time of the event.
    */
    bool MetaDown();

    /**
        Returns @true if the user is selecting grid cells, @false -- if
        deselecting.
    */
    bool Selecting();

    /**
        Returns @true if the Shift key was down at the time of the event.
    */
    bool ShiftDown();
};



/**
    @class wxGridCellFloatEditor
    @wxheader{grid.h}

    The editor for floating point numbers data.

    @library{wxadv}
    @category{FIXME}

    @see wxGridCellEditor, wxGridCellNumberEditor, wxGridCellBoolEditor,
    wxGridCellTextEditor, wxGridCellChoiceEditor
*/
class wxGridCellFloatEditor : public wxGridCellTextEditor
{
public:
    /**
        @param width
            Minimum number of characters to be shown.
        @param precision
            Number of digits after the decimal dot.
    */
    wxGridCellFloatEditor(int width = -1, int precision = -1);

    /**
        Parameters string format is "width,precision"
    */
    void SetParameters(const wxString& params);
};



/**
    @class wxGrid
    @wxheader{grid.h}

    wxGrid and its related classes are used for displaying and editing tabular
    data. They provide a rich set of features for display, editing, and
    interacting with a variety of data sources. For simple applications, and to
    help you get started, wxGrid is the only class you need to refer to
    directly. It will set up default instances of the other classes and manage
    them for you. For more complex applications you can derive your own
    classes for custom grid views, grid data tables, cell editors and
    renderers. The @ref overview_gridoverview has
    examples of simple and more complex applications, explains the
    relationship between the various grid classes and has a summary of the
    keyboard shortcuts and mouse functions provided by wxGrid.

    wxGrid has been greatly expanded and redesigned for wxWidgets 2.2
    onwards. The new grid classes are reasonably backward-compatible
    but there are some exceptions. There are also easier ways of doing many things
    compared to
    the previous implementation.

    A wxGridTableBase class holds the actual
    data to be displayed by a wxGrid class. One or more wxGrid classes
    may act as a view for one table class.
    The default table class is called wxGridStringTable and
    holds an array of strings. An instance of such a class is created
    by wxGrid::CreateGrid.

    wxGridCellRenderer is the abstract base
    class for rendereing contents in a cell. The following renderers are
    predefined:
    wxGridCellStringRenderer,
    wxGridCellBoolRenderer,
    wxGridCellFloatRenderer,
    wxGridCellNumberRenderer. The
    look of a cell can be further defined using wxGridCellAttr.
    An object of this type may be returned by wxGridTableBase::GetAttr.

    wxGridCellEditor is the abstract base
    class for editing the value of a cell. The following editors are
    predefined:
    wxGridCellTextEditor
    wxGridCellBoolEditor
    wxGridCellChoiceEditor
    wxGridCellNumberEditor.

    @library{wxadv}
    @category{miscwnd}

    @see @ref overview_gridoverview "wxGrid overview"
*/
class wxGrid : public wxScrolledWindow
{
public:
    //@{
    /**
        Constructor to create a grid object. Call either CreateGrid() or
        SetTable() directly after this to initialize the grid before using
        it.
    */
    wxGrid();
    wxGrid(wxWindow* parent, wxWindowID id,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize,
           long style = wxWANTS_CHARS,
           const wxString& name = wxPanelNameStr);
    //@}

    /**
        Destructor. This will also destroy the associated grid table unless you passed
        a table
        object to the grid and specified that the grid should not take ownership of the
        table (see wxGrid::SetTable).
    */
    ~wxGrid();

    /**
        Appends one or more new columns to the right of the grid and returns @true if
        successful. The updateLabels argument is not used at present.
        If you are using a derived grid table class you will need to override
        wxGridTableBase::AppendCols. See
        InsertCols() for further information.
    */
    bool AppendCols(int numCols = 1, bool updateLabels = true);

    /**
        Appends one or more new rows to the bottom of the grid and returns @true if
        successful. The updateLabels argument is not used at present.
        If you are using a derived grid table class you will need to override
        wxGridTableBase::AppendRows. See
        InsertRows() for further information.
    */
    bool AppendRows(int numRows = 1, bool updateLabels = true);

    /**
        Automatically sets the height and width of all rows and columns to fit their
        contents.
    */
    void AutoSize();

    /**
        Automatically adjusts width of the column to fit its label.
    */
    void AutoSizeColLabelSize(int col);

    /**
        Automatically sizes the column to fit its contents. If setAsMin is @true the
        calculated width will
        also be set as the minimal width for the column.
    */
    void AutoSizeColumn(int col, bool setAsMin = true);

    /**
        Automatically sizes all columns to fit their contents. If setAsMin is @true the
        calculated widths will
        also be set as the minimal widths for the columns.
    */
    void AutoSizeColumns(bool setAsMin = true);

    /**
        Automatically sizes the row to fit its contents. If setAsMin is @true the
        calculated height will
        also be set as the minimal height for the row.
    */
    void AutoSizeRow(int row, bool setAsMin = true);

    /**
        Automatically adjusts height of the row to fit its label.
    */
    void AutoSizeRowLabelSize(int col);

    /**
        Automatically sizes all rows to fit their contents. If setAsMin is @true the
        calculated heights will
        also be set as the minimal heights for the rows.
    */
    void AutoSizeRows(bool setAsMin = true);

    /**
        AutoSizeColumn()
        
        AutoSizeRow()
        
        AutoSizeColumns()
        
        AutoSizeRows()
        
        AutoSize()
        
        SetColMinimalWidth()
        
        SetRowMinimalHeight()
        
        SetColMinimalAcceptableWidth()
        
        SetRowMinimalAcceptableHeight()
        
        GetColMinimalAcceptableWidth()
        
        GetRowMinimalAcceptableHeight()
    */


    /**
        Increments the grid's batch count. When the count is greater than zero
        repainting of
        the grid is suppressed. Each call to BeginBatch must be matched by a later call
        to
        EndBatch(). Code that does a lot of grid
        modification can be enclosed between BeginBatch and EndBatch calls to avoid
        screen flicker. The final EndBatch will cause the grid to be repainted.
        
        @see wxGridUpdateLocker
    */
    void BeginBatch();

    /**
        This function returns the rectangle that encloses the block of cells
        limited by TopLeft and BottomRight cell in device coords and clipped
        to the client size of the grid window.
    */
    wxRect BlockToDeviceRect(const wxGridCellCoords& topLeft,
                             const wxGridCellCoords& bottomRight) const;

    /**
        Returns @true if columns can be moved by dragging with the mouse. Columns can be
        moved
        by dragging on their labels.
    */
    bool CanDragColMove() const;

    /**
        Returns @true if columns can be resized by dragging with the mouse. Columns can
        be resized
        by dragging the edges of their labels. If grid line dragging is enabled they
        can also be
        resized by dragging the right edge of the column in the grid cell area
        (see wxGrid::EnableDragGridSize).
    */
    bool CanDragColSize() const;

    /**
        Return @true if the dragging of grid lines to resize rows and columns is enabled
        or @false otherwise.
    */
    bool CanDragGridSize() const;

    /**
        Returns @true if rows can be resized by dragging with the mouse. Rows can be
        resized
        by dragging the edges of their labels. If grid line dragging is enabled they
        can also be
        resized by dragging the lower edge of the row in the grid cell area
        (see wxGrid::EnableDragGridSize).
    */
    bool CanDragRowSize() const;

    /**
        Returns @true if the in-place edit control for the current grid cell can be used
        and
        @false otherwise (e.g. if the current cell is read-only).
    */
    bool CanEnableCellControl() const;

    /**
        Do we have some place to store attributes in?
    */
    bool CanHaveAttributes() const;

    /**
        EnableDragRowSize()
        
        EnableDragColSize()
        
        CanDragRowSize()
        
        CanDragColSize()
        
        EnableDragColMove()
        
        CanDragColMove()
        
        EnableDragGridSize()
        
        CanDragGridSize()
        
        GetColAt()
        
        SetColPos()
        
        GetColPos()
        
        EnableDragCell()
        
        CanDragCell()
    */


    //@{
    /**
        Return the rectangle corresponding to the grid cell's size and position in
        logical
        coordinates.
    */
    wxRect CellToRect(int row, int col) const;
    const wxRect  CellToRect(const wxGridCellCoords& coords) const;
    //@}

    /**
        Clears all data in the underlying grid table and repaints the grid. The table
        is not deleted by
        this function. If you are using a derived table class then you need to override
        wxGridTableBase::Clear for this function to have any effect.
    */
    void ClearGrid();

    /**
        Deselects all cells that are currently selected.
    */
    void ClearSelection();

    /**
        @ref ctor() wxGrid
        
        @ref dtor() ~wxGrid
        
        CreateGrid()
        
        SetTable()
    */


    /**
        Creates a grid with the specified initial number of rows and columns.
        Call this directly after the grid constructor. When you use this
        function wxGrid will create and manage a simple table of string values
        for you. All of the grid data will be stored in memory.
        For applications with more complex data types or relationships, or for
        dealing with very large datasets, you should derive your own grid table
        class and pass a table object to the grid with SetTable().
    */
    bool CreateGrid(int numRows, int numCols,
                    wxGrid::wxGridSelectionModes selmode = wxGrid::wxGridSelectCells);

    /**
        MoveCursorUp()
        
        MoveCursorDown()
        
        MoveCursorLeft()
        
        MoveCursorRight()
        
        MoveCursorPageUp()
        
        MoveCursorPageDown()
        
        MoveCursorUpBlock()
        
        MoveCursorDownBlock()
        
        MoveCursorLeftBlock()
        
        MoveCursorRightBlock()
    */


    /**
        Deletes one or more columns from a grid starting at the specified position and
        returns
        @true if successful. The updateLabels argument is not used at present.
        If you are using a derived grid table class you will need to override
        wxGridTableBase::DeleteCols. See
        InsertCols() for further information.
    */
    bool DeleteCols(int pos = 0, int numCols = 1,
                    bool updateLabels = true);

    /**
        Deletes one or more rows from a grid starting at the specified position and
        returns
        @true if successful. The updateLabels argument is not used at present.
        If you are using a derived grid table class you will need to override
        wxGridTableBase::DeleteRows. See
        InsertRows() for further information.
    */
    bool DeleteRows(int pos = 0, int numRows = 1,
                    bool updateLabels = true);

    /**
        Disables in-place editing of grid cells.
        Equivalent to calling EnableCellEditControl(@false).
    */
    void DisableCellEditControl();

    /**
        Disables column moving by dragging with the mouse. Equivalent to passing @false
        to
        EnableDragColMove().
    */
    void DisableDragColMove();

    /**
        Disables column sizing by dragging with the mouse. Equivalent to passing @false
        to
        EnableDragColSize().
    */
    void DisableDragColSize();

    /**
        Disable mouse dragging of grid lines to resize rows and columns. Equivalent to
        passing
        @false to EnableDragGridSize()
    */
    void DisableDragGridSize();

    /**
        Disables row sizing by dragging with the mouse. Equivalent to passing @false to
        EnableDragRowSize().
    */
    void DisableDragRowSize();

    /**
        Enables or disables in-place editing of grid cell data. The grid will issue
        either a
        wxEVT_GRID_EDITOR_SHOWN or wxEVT_GRID_EDITOR_HIDDEN event.
    */
    void EnableCellEditControl(bool enable = true);

    /**
        Enables or disables column moving by dragging with the mouse.
    */
    void EnableDragColMove(bool enable = true);

    /**
        Enables or disables column sizing by dragging with the mouse.
    */
    void EnableDragColSize(bool enable = true);

    /**
        Enables or disables row and column resizing by dragging gridlines with the
        mouse.
    */
    void EnableDragGridSize(bool enable = true);

    /**
        Enables or disables row sizing by dragging with the mouse.
    */
    void EnableDragRowSize(bool enable = true);

    /**
        If the edit argument is @false this function sets the whole grid as read-only.
        If the
        argument is @true the grid is set to the default state where cells may be
        editable. In the
        default state you can set single grid cells and whole rows and columns to be
        editable or
        read-only via
        wxGridCellAttribute::SetReadOnly. For single
        cells you can also use the shortcut function
        SetReadOnly().
        For more information about controlling grid cell attributes see the
        wxGridCellAttr cell attribute class and the
        @ref overview_gridoverview.
    */
    void EnableEditing(bool edit);

    /**
        Turns the drawing of grid lines on or off.
    */
    void EnableGridLines(bool enable = true);

    /**
        Decrements the grid's batch count. When the count is greater than zero
        repainting of
        the grid is suppressed. Each previous call to
        BeginBatch() must be matched by a later call to
        EndBatch. Code that does a lot of grid modification can be enclosed between
        BeginBatch and EndBatch calls to avoid screen flicker. The final EndBatch will
        cause the grid to be repainted.
        
        @see wxGridUpdateLocker
    */
    void EndBatch();

    /**
        Overridden wxWindow method.
    */
    void Fit();

    /**
        Causes immediate repainting of the grid. Use this instead of the usual
        wxWindow::Refresh.
    */
    void ForceRefresh();

    /**
        Returns the number of times that BeginBatch() has been called
        without (yet) matching calls to EndBatch(). While
        the grid's batch count is greater than zero the display will not be updated.
    */
    int GetBatchCount() const;

    /**
        Sets the arguments to the horizontal and vertical text alignment values for the
        grid cell at the specified location.
        Horizontal alignment will be one of wxALIGN_LEFT, wxALIGN_CENTRE or
        wxALIGN_RIGHT.
        
        Vertical alignment will be one of wxALIGN_TOP, wxALIGN_CENTRE or wxALIGN_BOTTOM.
    */
    void GetCellAlignment(int row, int col, int* horiz, int* vert) const;

    /**
        Returns the background colour of the cell at the specified location.
    */
    wxColour GetCellBackgroundColour(int row, int col) const;

    /**
        Returns a pointer to the editor for the cell at the specified location.
        See wxGridCellEditor and
        the @ref overview_gridoverview "wxGrid overview" for more information about
        cell editors and renderers.
    */
    wxGridCellEditor* GetCellEditor(int row, int col) const;

    /**
        Returns the font for text in the grid cell at the specified location.
    */
    wxFont GetCellFont(int row, int col) const;

    /**
        Returns a pointer to the renderer for the grid cell at the specified location.
        See wxGridCellRenderer and
        the @ref overview_gridoverview "wxGrid overview" for more information about
        cell editors and renderers.
    */
    wxGridCellRenderer* GetCellRenderer(int row, int col) const;

    /**
        Returns the text colour for the grid cell at the specified location.
    */
    wxColour GetCellTextColour(int row, int col) const;

    //@{
    /**
        Returns the string contained in the cell at the specified location. For simple
        applications where a
        grid object automatically uses a default grid table of string values you use
        this function together
        with SetCellValue() to access cell values.
        For more complex applications where you have derived your own grid table class
        that contains
        various data types (e.g. numeric, boolean or user-defined custom types) then
        you only use this
        function for those cells that contain string values.
        See wxGridTableBase::CanGetValueAs
        and the @ref overview_gridoverview "wxGrid overview" for more information.
    */
    wxString GetCellValue(int row, int col) const;
    const wxString  GetCellValue(const wxGridCellCoords& coords) const;
    //@}

    /**
        Returns the column ID of the specified column position.
    */
    int GetColAt(int colPos) const;

    /**
        Returns the pen used for vertical grid lines. This virtual function may be
        overridden in derived classes in order to change the appearance of individual
        grid lines for the given column @e col.
        See GetRowGridLinePen() for an example.
    */
    wxPen GetColGridLinePen(int col);

    /**
        Sets the arguments to the current column label alignment values.
        Horizontal alignment will be one of wxALIGN_LEFT, wxALIGN_CENTRE or
        wxALIGN_RIGHT.
        
        Vertical alignment will be one of wxALIGN_TOP, wxALIGN_CENTRE or wxALIGN_BOTTOM.
    */
    void GetColLabelAlignment(int* horiz, int* vert) const;

    /**
        Returns the current height of the column labels.
    */
    int GetColLabelSize() const;

    /**
        Returns the specified column label. The default grid table class provides
        column labels of
        the form A,B...Z,AA,AB...ZZ,AAA... If you are using a custom grid table you can
        override
        wxGridTableBase::GetColLabelValue to provide
        your own labels.
    */
    wxString GetColLabelValue(int col) const;

    /**
        
    */
    int GetColLeft(int col) const;

    /**
        This returns the value of the lowest column width that can be handled
        correctly. See
        member SetColMinimalAcceptableWidth() for details.
    */
    int GetColMinimalAcceptableWidth() const;

    /**
        Get the minimal width of the given column/row.
    */
    int GetColMinimalWidth(int col) const;

    /**
        Returns the position of the specified column.
    */
    int GetColPos(int colID) const;

    /**
        
    */
    int GetColRight(int col) const;

    /**
        Returns the width of the specified column.
    */
    int GetColSize(int col) const;

    /**
        Sets the arguments to the current default horizontal and vertical text alignment
        values.
        Horizontal alignment will be one of wxALIGN_LEFT, wxALIGN_CENTRE or
        wxALIGN_RIGHT.
        
        Vertical alignment will be one of wxALIGN_TOP, wxALIGN_CENTRE or wxALIGN_BOTTOM.
    */
    void GetDefaultCellAlignment(int* horiz, int* vert) const;

    /**
        Returns the current default background colour for grid cells.
    */
    wxColour GetDefaultCellBackgroundColour() const;

    /**
        Returns the current default font for grid cell text.
    */
    wxFont GetDefaultCellFont() const;

    /**
        Returns the current default colour for grid cell text.
    */
    wxColour GetDefaultCellTextColour() const;

    /**
        Returns the default height for column labels.
    */
    int GetDefaultColLabelSize() const;

    /**
        Returns the current default width for grid columns.
    */
    int GetDefaultColSize() const;

    /**
        Returns a pointer to the current default grid cell editor.
        See wxGridCellEditor and
        the @ref overview_gridoverview "wxGrid overview" for more information about
        cell editors and renderers.
    */
    wxGridCellEditor* GetDefaultEditor() const;

    //@{
    /**
        
    */
    wxGridCellEditor* GetDefaultEditorForCell(int row, int col) const;
    const wxGridCellEditor*  GetDefaultEditorForCell(const wxGridCellCoords& c) const;
    //@}

    /**
        
    */
    wxGridCellEditor* GetDefaultEditorForType(const wxString& typeName) const;

    /**
        Returns the pen used for grid lines. This virtual function may be overridden in
        derived classes in order to change the appearance of grid lines. Note that
        currently the pen width must be 1.
        
        @see GetColGridLinePen(), GetRowGridLinePen()
    */
    wxPen GetDefaultGridLinePen();

    /**
        Returns a pointer to the current default grid cell renderer.
        See wxGridCellRenderer and
        the @ref overview_gridoverview "wxGrid overview" for more information about
        cell editors and renderers.
    */
    wxGridCellRenderer* GetDefaultRenderer() const;

    /**
        
    */
    wxGridCellRenderer* GetDefaultRendererForCell(int row, int col) const;

    /**
        
    */
    wxGridCellRenderer* GetDefaultRendererForType(const wxString& typeName) const;

    /**
        Returns the default width for the row labels.
    */
    int GetDefaultRowLabelSize() const;

    /**
        Returns the current default height for grid rows.
    */
    int GetDefaultRowSize() const;

    /**
        Returns the current grid cell column position.
    */
    int GetGridCursorCol() const;

    /**
        Returns the current grid cell row position.
    */
    int GetGridCursorRow() const;

    /**
        Returns the colour used for grid lines.
        
        @see GetDefaultGridLinePen()
    */
    wxColour GetGridLineColour() const;

    /**
        Returns the colour used for the background of row and column labels.
    */
    wxColour GetLabelBackgroundColour() const;

    /**
        Returns the font used for row and column labels.
    */
    wxFont GetLabelFont() const;

    /**
        Returns the colour used for row and column label text.
    */
    wxColour GetLabelTextColour() const;

    /**
        Returns the total number of grid columns (actually the number of columns in the
        underlying grid
        table).
    */
    int GetNumberCols() const;

    /**
        Returns the total number of grid rows (actually the number of rows in the
        underlying grid table).
    */
    int GetNumberRows() const;

    /**
        
    */
    wxGridCellAttr* GetOrCreateCellAttr(int row, int col) const;

    /**
        Returns the pen used for horizontal grid lines. This virtual function may be
        overridden in derived classes in order to change the appearance of individual
        grid line for the given row @e row.
        Example:
    */
    wxPen GetRowGridLinePen(int row);

    /**
        Sets the arguments to the current row label alignment values.
        Horizontal alignment will be one of wxLEFT, wxCENTRE or wxRIGHT.
        
        Vertical alignment will be one of wxTOP, wxCENTRE or wxBOTTOM.
    */
    void GetRowLabelAlignment(int* horiz, int* vert) const;

    /**
        Returns the current width of the row labels.
    */
    int GetRowLabelSize() const;

    /**
        Returns the specified row label. The default grid table class provides numeric
        row labels.
        If you are using a custom grid table you can override
        wxGridTableBase::GetRowLabelValue to provide
        your own labels.
    */
    wxString GetRowLabelValue(int row) const;

    /**
        This returns the value of the lowest row width that can be handled correctly.
        See
        member SetRowMinimalAcceptableHeight() for details.
    */
    int GetRowMinimalAcceptableHeight() const;

    /**
        
    */
    int GetRowMinimalHeight(int col) const;

    /**
        Returns the height of the specified row.
    */
    int GetRowSize(int row) const;

    /**
        Returns the number of pixels per horizontal scroll increment. The default is 15.
        
        @see GetScrollLineY(), SetScrollLineX(), SetScrollLineY()
    */
    int GetScrollLineX() const;

    /**
        Returns the number of pixels per vertical scroll increment. The default is 15.
        
        @see GetScrollLineX(), SetScrollLineX(), SetScrollLineY()
    */
    int GetScrollLineY() const;

    /**
        Returns an array of singly selected cells.
    */
    wxGridCellCoordsArray GetSelectedCells() const;

    /**
        Returns an array of selected cols.
    */
    wxArrayInt GetSelectedCols() const;

    /**
        Returns an array of selected rows.
    */
    wxArrayInt GetSelectedRows() const;

    /**
        Access or update the selection fore/back colours
    */
    wxColour GetSelectionBackground() const;

    /**
        Returns an array of the bottom right corners of blocks of selected cells,
        see GetSelectionBlockTopLeft().
    */
    wxGridCellCoordsArray GetSelectionBlockBottomRight() const;

    /**
        Returns an array of the top left corners of blocks of selected cells,
        see GetSelectionBlockBottomRight().
    */
    wxGridCellCoordsArray GetSelectionBlockTopLeft() const;

    /**
        
    */
    wxColour GetSelectionForeground() const;

    /**
        Returns the current selection mode, see SetSelectionMode().
    */
    wxGrid::wxGridSelectionModes GetSelectionMode() const;

    /**
        Returns a base pointer to the current table object.
    */
    wxGridTableBase* GetTable() const;

    /**
        Returned number of whole cols visible.
    */
    int GetViewWidth() const;

    /**
        EnableGridLines()
        
        GridLinesEnabled()
        
        SetGridLineColour()
        
        GetGridLineColour()
        
        GetDefaultGridLinePen()
        
        GetRowGridLinePen()
        
        GetColGridLinePen()
    */


    /**
        Returns @true if drawing of grid lines is turned on, @false otherwise.
    */
    bool GridLinesEnabled() const;

    /**
        Hides the in-place cell edit control.
    */
    void HideCellEditControl();

    /**
        Hides the column labels by calling SetColLabelSize()
        with a size of 0. Show labels again by calling that method with
        a width greater than 0.
    */
    void HideColLabels();

    /**
        Hides the row labels by calling SetRowLabelSize()
        with a size of 0. Show labels again by calling that method with
        a width greater than 0.
    */
    void HideRowLabels();

    /**
        Init the m_colWidths/Rights arrays
    */
    void InitColWidths();

    /**
        NB: @e never access m_row/col arrays directly because they are created
        on demand, @e always use accessor functions instead!
        Init the m_rowHeights/Bottoms arrays with default values.
    */
    void InitRowHeights();

    /**
        Inserts one or more new columns into a grid with the first new column at the
        specified position and returns @true if successful. The updateLabels argument is
        not
        used at present.
        The sequence of actions begins with the grid object requesting the underlying
        grid
        table to insert new columns. If this is successful the table notifies the grid
        and the
        grid updates the display. For a default grid (one where you have called
        wxGrid::CreateGrid) this process is automatic. If you are
        using a custom grid table (specified with wxGrid::SetTable)
        then you must override
        wxGridTableBase::InsertCols in your derived
        table class.
    */
    bool InsertCols(int pos = 0, int numCols = 1,
                    bool updateLabels = true);

    /**
        Inserts one or more new rows into a grid with the first new row at the specified
        position and returns @true if successful. The updateLabels argument is not used
        at
        present.
        The sequence of actions begins with the grid object requesting the underlying
        grid
        table to insert new rows. If this is successful the table notifies the grid and
        the
        grid updates the display. For a default grid (one where you have called
        wxGrid::CreateGrid) this process is automatic. If you are
        using a custom grid table (specified with wxGrid::SetTable)
        then you must override
        wxGridTableBase::InsertRows in your derived
        table class.
    */
    bool InsertRows(int pos = 0, int numRows = 1,
                    bool updateLabels = true);

    /**
        Returns @true if the in-place edit control is currently enabled.
    */
    bool IsCellEditControlEnabled() const;

    /**
        Returns @true if the current cell has been set to read-only
        (see wxGrid::SetReadOnly).
    */
    bool IsCurrentCellReadOnly() const;

    /**
        Returns @false if the whole grid has been set as read-only or @true otherwise.
        See EnableEditing() for more information about
        controlling the editing status of grid cells.
    */
    bool IsEditable() const;

    //@{
    /**
        Is this cell currently selected.
    */
    bool IsInSelection(int row, int col) const;
    const bool IsInSelection(const wxGridCellCoords& coords) const;
    //@}

    /**
        Returns @true if the cell at the specified location can't be edited.
        See also IsReadOnly().
    */
    bool IsReadOnly(int row, int col) const;

    /**
        Returns @true if there are currently rows, columns or blocks of cells selected.
    */
    bool IsSelection() const;

    //@{
    /**
        Returns @true if a cell is either wholly visible (the default) or at least
        partially
        visible in the grid window.
    */
    bool IsVisible(int row, int col, bool wholeCellVisible = true) const;
    const bool IsVisible(const wxGridCellCoords& coords,
                         bool wholeCellVisible = true) const;
    //@}

    //@{
    /**
        Brings the specified cell into the visible grid cell area with minimal
        scrolling. Does
        nothing if the cell is already visible.
    */
    void MakeCellVisible(int row, int col);
    void MakeCellVisible(const wxGridCellCoords& coords);
    //@}

    /**
        Moves the grid cursor down by one row. If a block of cells was previously
        selected it
        will expand if the argument is @true or be cleared if the argument is @false.
    */
    bool MoveCursorDown(bool expandSelection);

    /**
        Moves the grid cursor down in the current column such that it skips to the
        beginning or
        end of a block of non-empty cells. If a block of cells was previously selected
        it
        will expand if the argument is @true or be cleared if the argument is @false.
    */
    bool MoveCursorDownBlock(bool expandSelection);

    /**
        Moves the grid cursor left by one column. If a block of cells was previously
        selected it
        will expand if the argument is @true or be cleared if the argument is @false.
    */
    bool MoveCursorLeft(bool expandSelection);

    /**
        Moves the grid cursor left in the current row such that it skips to the
        beginning or
        end of a block of non-empty cells. If a block of cells was previously selected
        it
        will expand if the argument is @true or be cleared if the argument is @false.
    */
    bool MoveCursorLeftBlock(bool expandSelection);

    /**
        Moves the grid cursor right by one column. If a block of cells was previously
        selected it
        will expand if the argument is @true or be cleared if the argument is @false.
    */
    bool MoveCursorRight(bool expandSelection);

    /**
        Moves the grid cursor right in the current row such that it skips to the
        beginning or
        end of a block of non-empty cells. If a block of cells was previously selected
        it
        will expand if the argument is @true or be cleared if the argument is @false.
    */
    bool MoveCursorRightBlock(bool expandSelection);

    /**
        Moves the grid cursor up by one row. If a block of cells was previously
        selected it
        will expand if the argument is @true or be cleared if the argument is @false.
    */
    bool MoveCursorUp(bool expandSelection);

    /**
        Moves the grid cursor up in the current column such that it skips to the
        beginning or
        end of a block of non-empty cells. If a block of cells was previously selected
        it
        will expand if the argument is @true or be cleared if the argument is @false.
    */
    bool MoveCursorUpBlock(bool expandSelection);

    /**
        Moves the grid cursor down by some number of rows so that the previous bottom
        visible row
        becomes the top visible row.
    */
    bool MovePageDown();

    /**
        Moves the grid cursor up by some number of rows so that the previous top
        visible row
        becomes the bottom visible row.
    */
    bool MovePageUp();

    /**
        Methods for a registry for mapping data types to Renderers/Editors
    */
    void RegisterDataType(const wxString& typeName,
                          wxGridCellRenderer* renderer,
                          wxGridCellEditor* editor);

    /**
        SetRowLabelValue()
        
        SetColLabelValue()
        
        GetRowLabelValue()
        
        GetColLabelValue()
        
        SetUseNativeColLabels()
        
        HideColLabels()
        
        HideRowLabels()
        
        SetRowLabelSize()
        
        SetColLabelSize()
        
        GetRowLabelSize()
        
        GetColLabelSize()
        
        AutoSizeRowLabelSize()
        
        AutoSizeColLabelSize()
        
        GetDefaultRowLabelSize()
        
        GetDefaultColLabelSize()
        
        SetRowLabelAlignment()
        
        SetColLabelAlignment()
        
        GetRowLabelAlignment()
        
        GetColLabelAlignment()
        
        SetLabelFont()
        
        SetLabelTextColour()
        
        SetLabelBackgroundColour()
        
        GetLabelFont()
        
        GetLabelBackgroundColour()
        
        GetLabelTextColour()
        
        SetColLabelTextOrientation()
        
        GetColLabelTextOrientation()
    */


    /**
        Sets the value of the current grid cell to the current in-place edit control
        value.
        This is called automatically when the grid cursor moves from the current cell
        to a
        new cell. It is also a good idea to call this function when closing a grid since
        any edits to the final cell location will not be saved otherwise.
    */
    void SaveEditControlValue();

    /**
        Selects all cells in the grid.
    */
    void SelectAll();

    //@{
    /**
        Selects a rectangular block of cells. If addToSelected is @false then any
        existing selection will be
        deselected; if @true the column will be added to the existing selection.
    */
    void SelectBlock(int topRow, int leftCol, int bottomRow,
                     int rightCol,
                     bool addToSelected = false);
    void SelectBlock(const wxGridCellCoords& topLeft,
                     const wxGridCellCoords& bottomRight,
                     bool addToSelected = false);
    //@}

    /**
        Selects the specified column. If addToSelected is @false then any existing
        selection will be
        deselected; if @true the column will be added to the existing selection.
    */
    void SelectCol(int col, bool addToSelected = false);

    /**
        Selects the specified row. If addToSelected is @false then any existing
        selection will be
        deselected; if @true the row will be added to the existing selection.
    */
    void SelectRow(int row, bool addToSelected = false);

    /**
        ClearSelection()
        
        IsSelection()
        
        SelectAll()
        
        SelectBlock()
        
        SelectCol()
        
        SelectRow()
    */


    /**
        This function returns the rectangle that encloses the selected cells
        in device coords and clipped to the client size of the grid window.
    */
    wxRect SelectionToDeviceRect() const;

    //@{
    /**
        Sets the horizontal and vertical alignment for grid cell text at the specified
        location.
        Horizontal alignment should be one of wxALIGN_LEFT, wxALIGN_CENTRE or
        wxALIGN_RIGHT.
        
        Vertical alignment should be one of wxALIGN_TOP, wxALIGN_CENTRE or
        wxALIGN_BOTTOM.
    */
    void SetCellAlignment(int row, int col, int horiz, int vert);
    void SetCellAlignment(int align, int row, int col);
    //@}

    /**
        
    */
    void SetCellBackgroundColour(int row, int col,
                                 const wxColour& colour);

    /**
        Sets the editor for the grid cell at the specified location.
        The grid will take ownership of the pointer.
        See wxGridCellEditor and
        the @ref overview_gridoverview "wxGrid overview" for more information about
        cell editors and renderers.
    */
    void SetCellEditor(int row, int col, wxGridCellEditor* editor);

    /**
        Sets the font for text in the grid cell at the specified location.
    */
    void SetCellFont(int row, int col, const wxFont& font);

    /**
        Sets the renderer for the grid cell at the specified location.
        The grid will take ownership of the pointer.
        See wxGridCellRenderer and
        the @ref overview_gridoverview "wxGrid overview" for more information about
        cell editors and renderers.
    */
    void SetCellRenderer(int row, int col,
                         wxGridCellRenderer* renderer);

    //@{
    /**
        Sets the text colour for the grid cell at the specified location.
    */
    void SetCellTextColour(int row, int col, const wxColour& colour);
    void SetCellTextColour(const wxColour& val, int row, int col);
    void SetCellTextColour(const wxColour& colour);
    //@}

    //@{
    /**
        Sets the string value for the cell at the specified location. For simple
        applications where a
        grid object automatically uses a default grid table of string values you use
        this function together
        with GetCellValue() to access cell values.
        For more complex applications where you have derived your own grid table class
        that contains
        various data types (e.g. numeric, boolean or user-defined custom types) then
        you only use this
        function for those cells that contain string values.
        The last form is for backward compatibility only.
        See wxGridTableBase::CanSetValueAs
        and the @ref overview_gridoverview "wxGrid overview" for more information.
    */
    void SetCellValue(int row, int col, const wxString& s);
    void SetCellValue(const wxGridCellCoords& coords,
                      const wxString& s);
    void SetCellValue(const wxString& val, int row, int col);
    //@}

    /**
        Sets the cell attributes for all cells in the specified column.
        For more information about controlling grid cell attributes see the
        wxGridCellAttr cell attribute class and the
        @ref overview_gridoverview.
    */
    void SetColAttr(int col, wxGridCellAttr* attr);

    /**
        Sets the specified column to display boolean values. wxGrid displays boolean
        values with a checkbox.
    */
    void SetColFormatBool(int col);

    /**
        Sets the specified column to display data in a custom format.
        See the @ref overview_gridoverview "wxGrid overview" for more information on
        working
        with custom data types.
    */
    void SetColFormatCustom(int col, const wxString& typeName);

    /**
        Sets the specified column to display floating point values with the given width
        and precision.
    */
    void SetColFormatFloat(int col, int width = -1,
                           int precision = -1);

    /**
        Sets the specified column to display integer values.
    */
    void SetColFormatNumber(int col);

    /**
        Sets the horizontal and vertical alignment of column label text.
        Horizontal alignment should be one of wxALIGN_LEFT, wxALIGN_CENTRE or
        wxALIGN_RIGHT.
        Vertical alignment should be one of wxALIGN_TOP, wxALIGN_CENTRE or
        wxALIGN_BOTTOM.
    */
    void SetColLabelAlignment(int horiz, int vert);

    /**
        Sets the height of the column labels.
        If @a height equals to @c wxGRID_AUTOSIZE then height is calculated
        automatically
        so that no label is truncated. Note that this could be slow for a large table.
    */
    void SetColLabelSize(int height);

    /**
        Set the value for the given column label. If you are using a derived grid table
        you must
        override wxGridTableBase::SetColLabelValue
        for this to have any effect.
    */
    void SetColLabelValue(int col, const wxString& value);

    /**
        This modifies the minimum column width that can be handled correctly.
        Specifying a low value here
        allows smaller grid cells to be dealt with correctly. Specifying a value here
        which is much smaller
        than the actual minimum size will incur a performance penalty in the functions
        which perform
        grid cell index lookup on the basis of screen coordinates.
        This should normally be called when creating the grid because it will not
        resize existing columns
        with sizes smaller than the value specified here.
    */
    void SetColMinimalAcceptableWidth(int width);

    /**
        Sets the minimal width for the specified column. This should normally be called
        when creating the grid
        because it will not resize a column that is already narrower than the minimal
        width.
        The width argument must be higher than the minimimal acceptable column width,
        see
        GetColMinimalAcceptableWidth().
    */
    void SetColMinimalWidth(int col, int width);

    /**
        Sets the position of the specified column.
    */
    void SetColPos(int colID, int newPos);

    /**
        Sets the width of the specified column.
        This function does not refresh the grid. If you are calling it outside of a
        BeginBatch / EndBatch
        block you can use ForceRefresh() to see the changes.
        Automatically sizes the column to fit its contents. If setAsMin is @true the
        calculated width will
        also be set as the minimal width for the column.
    */
    void SetColSize(int col, int width);

    /**
        Sets the default horizontal and vertical alignment for grid cell text.
        Horizontal alignment should be one of wxALIGN_LEFT, wxALIGN_CENTRE or
        wxALIGN_RIGHT.
        Vertical alignment should be one of wxALIGN_TOP, wxALIGN_CENTRE or
        wxALIGN_BOTTOM.
    */
    void SetDefaultCellAlignment(int horiz, int vert);

    /**
        Sets the default background colour for grid cells.
    */
    void SetDefaultCellBackgroundColour(const wxColour& colour);

    /**
        Sets the default font to be used for grid cell text.
    */
    void SetDefaultCellFont(const wxFont& font);

    /**
        Sets the current default colour for grid cell text.
    */
    void SetDefaultCellTextColour(const wxColour& colour);

    /**
        Sets the default width for columns in the grid. This will only affect columns
        subsequently added to
        the grid unless resizeExistingCols is @true.
    */
    void SetDefaultColSize(int width,
                           bool resizeExistingCols = false);

    /**
        Sets the default editor for grid cells. The grid will take ownership of the
        pointer.
        See wxGridCellEditor and
        the @ref overview_gridoverview "wxGrid overview" for more information about
        cell editors and renderers.
    */
    void SetDefaultEditor(wxGridCellEditor* editor);

    /**
        Sets the default renderer for grid cells. The grid will take ownership of the
        pointer.
        See wxGridCellRenderer and
        the @ref overview_gridoverview "wxGrid overview" for more information about
        cell editors and renderers.
    */
    void SetDefaultRenderer(wxGridCellRenderer* renderer);

    /**
        Sets the default height for rows in the grid. This will only affect rows
        subsequently added
        to the grid unless resizeExistingRows is @true.
    */
    void SetDefaultRowSize(int height,
                           bool resizeExistingRows = false);

    /**
        Set the grid cursor to the specified cell.
        This function calls MakeCellVisible().
    */
    void SetGridCursor(int row, int col);

    /**
        Sets the colour used to draw grid lines.
    */
    void SetGridLineColour(const wxColour& colour);

    /**
        Sets the background colour for row and column labels.
    */
    void SetLabelBackgroundColour(const wxColour& colour);

    /**
        Sets the font for row and column labels.
    */
    void SetLabelFont(const wxFont& font);

    /**
        Sets the colour for row and column label text.
    */
    void SetLabelTextColour(const wxColour& colour);

    /**
        A grid may occupy more space than needed for its rows/columns. This
        function allows to set how big this extra space is
    */
    void SetMargins(int extraWidth, int extraHeight);

    /**
        Common part of AutoSizeColumn/Row() and GetBestSize()
    */
    int SetOrCalcColumnSizes(bool calcOnly, bool setAsMin = true);

    /**
        
    */
    int SetOrCalcRowSizes(bool calcOnly, bool setAsMin = true);

    /**
        Makes the cell at the specified location read-only or editable.
        See also IsReadOnly().
    */
    void SetReadOnly(int row, int col, bool isReadOnly = true);

    /**
        Sets the cell attributes for all cells in the specified row.
        See the wxGridCellAttr class for more information
        about controlling cell attributes.
    */
    void SetRowAttr(int row, wxGridCellAttr* attr);

    /**
        Sets the horizontal and vertical alignment of row label text.
        Horizontal alignment should be one of wxALIGN_LEFT, wxALIGN_CENTRE or
        wxALIGN_RIGHT.
        Vertical alignment should be one of wxALIGN_TOP, wxALIGN_CENTRE or
        wxALIGN_BOTTOM.
    */
    void SetRowLabelAlignment(int horiz, int vert);

    /**
        Sets the width of the row labels.
        If @a width equals @c wxGRID_AUTOSIZE then width is calculated automatically
        so that no label is truncated. Note that this could be slow for a large table.
    */
    void SetRowLabelSize(int width);

    /**
        Set the value for the given row label. If you are using a derived grid table
        you must
        override wxGridTableBase::SetRowLabelValue
        for this to have any effect.
    */
    void SetRowLabelValue(int row, const wxString& value);

    /**
        This modifies the minimum row width that can be handled correctly. Specifying a
        low value here
        allows smaller grid cells to be dealt with correctly. Specifying a value here
        which is much smaller
        than the actual minimum size will incur a performance penalty in the functions
        which perform
        grid cell index lookup on the basis of screen coordinates.
        This should normally be called when creating the grid because it will not
        resize existing rows
        with sizes smaller than the value specified here.
    */
    void SetRowMinimalAcceptableHeight(int height);

    /**
        Sets the minimal height for the specified row. This should normally be called
        when creating the grid
        because it will not resize a row that is already shorter than the minimal
        height.
        The height argument must be higher than the minimimal acceptable row height, see
        GetRowMinimalAcceptableHeight().
    */
    void SetRowMinimalHeight(int row, int height);

    /**
        Sets the height of the specified row.
        This function does not refresh the grid. If you are calling it outside of a
        BeginBatch / EndBatch
        block you can use ForceRefresh() to see the changes.
        Automatically sizes the column to fit its contents. If setAsMin is @true the
        calculated width will
        also be set as the minimal width for the column.
    */
    void SetRowSize(int row, int height);

    /**
        Sets the number of pixels per horizontal scroll increment. The default is 15.
        Sometimes wxGrid has trouble setting the scrollbars correctly due to rounding
        errors: setting this to 1 can help.
        
        @see GetScrollLineX(), GetScrollLineY(), SetScrollLineY()
    */
    void SetScrollLineX(int x);

    /**
        Sets the number of pixels per vertical scroll increment. The default is 15.
        Sometimes wxGrid has trouble setting the scrollbars correctly due to rounding
        errors: setting this to 1 can help.
        
        @see GetScrollLineX(), GetScrollLineY(), SetScrollLineX()
    */
    void SetScrollLineY(int y);

    /**
        
    */
    void SetSelectionBackground(const wxColour& c);

    /**
        
    */
    void SetSelectionForeground(const wxColour& c);

    /**
        Set the selection behaviour of the grid.
        
        @param wxGridSelectCells()
            The default mode where individual cells are selected.
        @param wxGridSelectRows()
            Selections will consist of whole rows.
        @param wxGridSelectColumns()
            Selections will consist of whole columns.
    */
    void SetSelectionMode(wxGrid::wxGridSelectionModes selmode);

    /**
        Passes a pointer to a custom grid table to be used by the grid. This should be
        called
        after the grid constructor and before using the grid object. If takeOwnership
        is set to
        @true then the table will be deleted by the wxGrid destructor.
        Use this function instead of CreateGrid() when your
        application involves complex or non-string data or data sets that are too large
        to fit
        wholly in memory.
    */
    bool SetTable(wxGridTableBase* table, bool takeOwnership = false,
                  wxGrid::wxGridSelectionModes selmode = wxGrid::wxGridSelectCells);

    /**
        Call this in order to make the column labels use a native look by using
        wxRenderer::DrawHeaderButton
        internally. There is no equivalent method for drawing row columns as
        there is not native look for that. This option is useful when using
        wxGrid for displaying tables and not as a spread-sheet.
    */
    void SetUseNativeColLabels(bool native = true);

    /**
        Displays the in-place cell edit control for the current cell.
    */
    void ShowCellEditControl();

    /**
        @param x
            The x position to evaluate.
        @param clipToMinMax
            If @true, rather than returning wxNOT_FOUND, it returns either the first or
        last column depending on whether x is too far to the left or right respectively.
    */
    int XToCol(int x, bool clipToMinMax = false) const;

    /**
        Returns the column whose right hand edge is close to the given logical x
        position.
        If no column edge is near to this position @c wxNOT_FOUND is returned.
    */
    int XToEdgeOfCol(int x) const;

    /**
        Returns the row whose bottom edge is close to the given logical y position.
        If no row edge is near to this position @c wxNOT_FOUND is returned.
    */
    int YToEdgeOfRow(int y) const;

    /**
        Returns the grid row that corresponds to the logical y coordinate. Returns
        @c wxNOT_FOUND if there is no row at the y position.
    */
    int YToRow(int y) const;
};



/**
    @class wxGridCellBoolEditor
    @wxheader{grid.h}

    The editor for boolean data.

    @library{wxadv}
    @category{FIXME}

    @see wxGridCellEditor, wxGridCellFloatEditor, wxGridCellNumberEditor,
    wxGridCellTextEditor, wxGridCellChoiceEditor
*/
class wxGridCellBoolEditor : public wxGridCellEditor
{
public:
    /**
        Default constructor.
    */
    wxGridCellBoolEditor();

    /**
        Returns @true if the given @a value is equal to the string representation of
        the truth value we currently use (see
        wxGridCellBoolEditor::UseStringValues).
    */
    static bool IsTrueValue(const wxString& value);

    /**
        ,  wxString&@e valueFalse = _T(""))
        This method allows to customize the values returned by GetValue() method for
        the cell using this editor. By default, the default values of the arguments are
        used, i.e. @c "1" is returned if the cell is checked and an empty string
        otherwise, using this method allows to change this.
    */
    static void UseStringValues() const;
};



/**
    @class wxGridUpdateLocker
    @wxheader{grid.h}

    This small class can be used to prevent wxGrid from redrawing
    during its lifetime by calling wxGrid::BeginBatch
    in its constructor and wxGrid::EndBatch in its
    destructor. It is typically used in a function performing several operations
    with a grid which would otherwise result in flicker. For example:

    @code
    void MyFrame::Foo()
        {
            m_grid = new wxGrid(this, ...);

            wxGridUpdateLocker noUpdates(m_grid);
            m_grid-AppendColumn();
            ... many other operations with m_grid...
            m_grid-AppendRow();

            // destructor called, grid refreshed
        }
    @endcode

    Using this class is easier and safer than calling
    wxGrid::BeginBatch and wxGrid::EndBatch
    because you don't risk not to call the latter (due to an exception for example).

    @library{wxadv}
    @category{FIXME}
*/
class wxGridUpdateLocker
{
public:
    /**
        Creates an object preventing the updates of the specified @e grid. The
        parameter could be @NULL in which case nothing is done. If @a grid is
        non-@NULL then the grid must exist for longer than wxGridUpdateLocker object
        itself.
        The default constructor could be followed by a call to
        Create() to set the
        grid object later.
    */
    wxGridUpdateLocker(wxGrid* grid = NULL);

    /**
        Destructor reenables updates for the grid this object is associated with.
    */
    ~wxGridUpdateLocker();

    /**
        This method can be called if the object had been constructed using the default
        constructor. It must not be called more than once.
    */
    void Create(wxGrid* grid);
};

