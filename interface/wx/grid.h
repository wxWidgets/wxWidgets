/////////////////////////////////////////////////////////////////////////////
// Name:        grid.h
// Purpose:     interface of wxGridCellFloatRenderer
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxGridCellFloatRenderer

    This class may be used to format floating point data in a cell.

    @library{wxadv}
    @category{grid}

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
        Returns the precision.
    */
    int GetPrecision() const;

    /**
        Returns the width.
    */
    int GetWidth() const;

    /**
        Parameters string format is "width[,precision]".
    */
    virtual void SetParameters(const wxString& params);

    /**
        Sets the precision.
    */
    void SetPrecision(int precision);

    /**
        Sets the width.
    */
    void SetWidth(int width);
};



/**
    @class wxGridTableBase

    The almost abstract base class for grid tables.

    A grid table is responsible for storing the grid data and, indirectly, grid
    cell attributes. The data can be stored in the way most convenient for the
    application but has to be provided in string form to wxGrid. It is also
    possible to provide cells values in other formats if appropriate, e.g. as
    numbers.

    This base class is not quite abstract as it implements a trivial strategy
    for storing the attributes by forwarding it to wxGridCellAttrProvider and
    also provides stubs for some other functions. However it does have a number
    of pure virtual methods which must be implemented in the derived classes.

    @see wxGridStringTable

    @library{wxadv}
    @category{grid}
*/
class wxGridTableBase : public wxObject
{
public:
    /// Default constructor.
    wxGridTableBase();

    /// Destructor frees the attribute provider if it was created.
    virtual ~wxGridTableBase();

    /**
        Must be overridden to return the number of rows in the table.

        For backwards compatibility reasons, this method is not const.
        Use GetRowsCount() instead of it in const methods of derived table
        classes.
     */
    virtual int GetNumberRows() = 0;

    /**
        Must be overridden to return the number of columns in the table.

        For backwards compatibility reasons, this method is not const.
        Use GetColsCount() instead of it in const methods of derived table
        classes,
     */
    virtual int GetNumberCols() = 0;

    /**
        Return the number of rows in the table.

        This method is not virtual and is only provided as a convenience for
        the derived classes which can't call GetNumberRows() without a @c
        const_cast from their const methods.
     */
    int GetRowsCount() const;

    /**
        Return the number of columns in the table.

        This method is not virtual and is only provided as a convenience for
        the derived classes which can't call GetNumberCols() without a @c
        const_cast from their const methods.
     */
    int GetColsCount() const;


    /**
        Accessing table cells.
     */
    //@{

    /// Must be overridden to implement testing for empty cells.
    virtual bool IsEmptyCell(int row, int col) = 0;

    /**
        Same as IsEmptyCell() but taking wxGridCellCoords.

        Notice that this method is not virtual, only IsEmptyCell() should be
        overridden.
     */
    bool IsEmpty(const wxGridCellCoords& coords);

    /// Must be overridden to implement accessing the table values as text.
    virtual wxString GetValue(int row, int col) = 0;

    /// Must be overridden to implement setting the table values as text.
    virtual void SetValue(int row, int col, const wxString& value) = 0;

    /**
        Returns the type of the value in the given cell.

        By default all cells are strings and this method returns @c
        wxGRID_VALUE_STRING.
     */
    virtual wxString GetTypeName(int row, int col);

    /**
        Returns true if the value of the given cell can be accessed as if it
        were of the specified type.

        By default the cells can only be accessed as strings. Note that a cell
        could be accessible in different ways, e.g. a numeric cell may return
        @true for @c wxGRID_VALUE_NUMBER but also for @c wxGRID_VALUE_STRING
        indicating that the value can be coerced to a string form.
     */
    virtual bool CanGetValueAs(int row, int col, const wxString& typeName);

    /**
        Returns true if the value of the given cell can be set as if it were of
        the specified type.

        @see CanGetValueAs()
     */
    virtual bool CanSetValueAs(int row, int col, const wxString& typeName);

    /**
        Returns the value of the given cell as a long.

        This should only be called if CanGetValueAs() returns @true when called
        with @c wxGRID_VALUE_NUMBER argument. Default implementation always
        return 0.
     */
    virtual long GetValueAsLong(int row, int col);

    /**
        Returns the value of the given cell as a double.

        This should only be called if CanGetValueAs() returns @true when called
        with @c wxGRID_VALUE_FLOAT argument. Default implementation always
        return 0.0.
     */
    virtual double GetValueAsDouble(int row, int col);

    /**
        Returns the value of the given cell as a boolean.

        This should only be called if CanGetValueAs() returns @true when called
        with @c wxGRID_VALUE_BOOL argument. Default implementation always
        return false.
     */
    virtual bool GetValueAsBool(int row, int col);

    /**
        Returns the value of the given cell as a user-defined type.

        This should only be called if CanGetValueAs() returns @true when called
        with @a typeName. Default implementation always return @NULL.
     */
    virtual void *GetValueAsCustom(int row, int col, const wxString& typeName);


    /**
        Sets the value of the given cell as a long.

        This should only be called if CanSetValueAs() returns @true when called
        with @c wxGRID_VALUE_NUMBER argument. Default implementation doesn't do
        anything.
     */
    virtual void SetValueAsLong(int row, int col, long value);

    /**
        Sets the value of the given cell as a double.

        This should only be called if CanSetValueAs() returns @true when called
        with @c wxGRID_VALUE_FLOAT argument. Default implementation doesn't do
        anything.
     */
    virtual void SetValueAsDouble(int row, int col, double value);

    /**
        Sets the value of the given cell as a boolean.

        This should only be called if CanSetValueAs() returns @true when called
        with @c wxGRID_VALUE_BOOL argument. Default implementation doesn't do
        anything.
     */
    virtual void SetValueAsBool( int row, int col, bool value );

    /**
        Sets the value of the given cell as a user-defined type.

        This should only be called if CanSetValueAs() returns @true when called
        with @a typeName. Default implementation doesn't do anything.
     */
    virtual void SetValueAsCustom(int row, int col, const wxString& typeName,
                                  void *value);

    //@}

    /**
        Called by the grid when the table is associated with it.

        The default implementation stores the pointer and returns it from its
        GetView() and so only makes sense if the table cannot be associated
        with more than one grid at a time.
     */
    virtual void SetView(wxGrid *grid);

    /**
        Returns the last grid passed to SetView().
     */
    virtual wxGrid *GetView() const;


    /**
        Modifying the table structure.

        Notice that none of these functions are pure virtual as they don't have
        to be implemented if the table structure is never modified after
        creation, i.e. neither rows nor columns are never added or deleted but
        that you do need to implement them if they are called, i.e. if your
        code either calls them directly or uses the matching wxGrid methods, as
        by default they simply do nothing which is definitely inappropriate.
     */
    //@{

    /**
        Clear the table contents.

        This method is used by wxGrid::ClearGrid().
     */
    virtual void Clear();

    /**
        Insert additional rows into the table.

        @param pos
            The position of the first new row.
        @param numRows
            The number of rows to insert.
     */
    virtual bool InsertRows(size_t pos = 0, size_t numRows = 1);

    /**
        Append additional rows at the end of the table.

        This method is provided in addition to InsertRows() as some data models
        may only support appending rows to them but not inserting them at
        arbitrary locations. In such case you may implement this method only
        and leave InsertRows() unimplemented.

        @param pos
            The position of the first new row.
        @param numRows
            The number of rows to add.
     */
    virtual bool AppendRows(size_t numRows = 1);

    /**
        Delete rows from the table.

        @param pos
            The first row to delete.
        @param numRows
            The number of rows to delete.
     */
    virtual bool DeleteRows(size_t pos = 0, size_t numRows = 1);

    /// Exactly the same as InsertRows() but for columns.
    virtual bool InsertCols(size_t pos = 0, size_t numCols = 1);

    /// Exactly the same as AppendRows() but for columns.
    virtual bool AppendCols(size_t numCols = 1);

    /// Exactly the same as DeleteRows() but for columns.
    virtual bool DeleteCols(size_t pos = 0, size_t numCols = 1);

    //@}

    /**
        Table rows and columns labels.

        By default the numbers are used for labeling rows and Latin letters for
        labeling columns. If the table has more than 26 columns, the pairs of
        letters are used starting from the 27-th one and so on, i.e. the
        sequence of labels is A, B, ..., Z, AA, AB, ..., AZ, BA, ..., ..., ZZ,
        AAA, ...
     */
    //@{

    /// Return the label of the specified row.
    virtual wxString GetRowLabelValue(int row);

    /// Return the label of the specified column.
    virtual wxString GetColLabelValue(int col);

    /**
        Set the given label for the specified row.

        The default version does nothing, i.e. the label is not stored. You
        must override this method in your derived class if you wish
        wxGrid::SetRowLabelValue() to work.
     */
    virtual void SetRowLabelValue(int row, const wxString& label);

    /// Exactly the same as SetRowLabelValue() but for columns.
    virtual void SetColLabelValue(int col, const wxString& label);

    //@}


    /**
        Attributes management.

        By default the attributes management is delegated to
        wxGridCellAttrProvider class. You may override the methods in this
        section to handle the attributes directly if, for example, they can be
        computed from the cell values.
     */
    //@{

    /**
        Associate this attributes provider with the table.

        The table takes ownership of @a attrProvider pointer and will delete it
        when it doesn't need it any more. The pointer can be @NULL, however
        this won't disable attributes management in the table but will just
        result in a default attributes being recreated the next time any of the
        other functions in this section is called. To completely disable the
        attributes support, should this be needed, you need to override
        CanHaveAttributes() to return @false.
     */
    void SetAttrProvider(wxGridCellAttrProvider *attrProvider);

    /**
        Returns the attribute provider currently being used.

        This function may return @NULL if the attribute provider hasn't been
        neither associated with this table by SetAttrProvider() nor created on
        demand by any other methods.
     */
    wxGridCellAttrProvider *GetAttrProvider() const;

    /**
        Return the attribute for the given cell.

        By default this function is simply forwarded to
        wxGridCellAttrProvider::GetAttr() but it may be overridden to handle
        attributes directly in the table.
     */
    virtual wxGridCellAttr *GetAttr(int row, int col,
                                    wxGridCellAttr::wxAttrKind kind);

    /**
        Set attribute of the specified cell.

        By default this function is simply forwarded to
        wxGridCellAttrProvider::SetAttr().

        The table takes ownership of @a attr, i.e. will call DecRef() on it.
     */
    virtual void SetAttr(wxGridCellAttr* attr, int row, int col);

    /**
        Set attribute of the specified row.

        By default this function is simply forwarded to
        wxGridCellAttrProvider::SetRowAttr().

        The table takes ownership of @a attr, i.e. will call DecRef() on it.
     */
    virtual void SetRowAttr(wxGridCellAttr *attr, int row);

    /**
        Set attribute of the specified column.

        By default this function is simply forwarded to
        wxGridCellAttrProvider::SetColAttr().

        The table takes ownership of @a attr, i.e. will call DecRef() on it.
     */
    virtual void SetColAttr(wxGridCellAttr *attr, int col);

    //@}

    /**
        Returns true if this table supports attributes or false otherwise.

        By default, the table automatically creates a wxGridCellAttrProvider
        when this function is called if it had no attribute provider before and
        returns @true.
     */
    virtual bool CanHaveAttributes();
};



/**
    @class wxGridCellEditor

    This class is responsible for providing and manipulating
    the in-place edit controls for the grid.  Instances of wxGridCellEditor
    (actually, instances of derived classes since it is an abstract class) can be
    associated with the cell attributes for individual cells, rows, columns, or
    even for the entire grid.

    @library{wxadv}
    @category{grid}

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
        Fetch the value from the table and prepare the edit control
        to begin editing. Set the focus to the edit control.
    */
    virtual void BeginEdit(int row, int col, wxGrid* grid) = 0;

    /**
        Create a new object which is the copy of this one.
    */
    virtual wxGridCellEditor* Clone() const = 0;

    /**
        Creates the actual edit control.
    */
    virtual void Create(wxWindow* parent, wxWindowID id,
                        wxEvtHandler* evtHandler) = 0;

    /**
        Final cleanup.
    */
    virtual void Destroy();

    /**
        Complete the editing of the current cell. Returns @true if the value has
        changed. If necessary, the control may be destroyed.
    */
    virtual bool EndEdit(int row, int col, wxGrid* grid) = 0;

    /**
        Some types of controls on some platforms may need some help
        with the Return key.
    */
    virtual void HandleReturn(wxKeyEvent& event);

    /**

    */
    bool IsCreated();

    /**
        Draws the part of the cell not occupied by the control: the base class
        version just fills it with background colour from the attribute.
    */
    virtual void PaintBackground(const wxRect& rectCell, wxGridCellAttr* attr);

    /**
        Reset the value in the control back to its starting value.
    */
    virtual void Reset() = 0;

    /**
        Size and position the edit control.
    */
    virtual void SetSize(const wxRect& rect);

    /**
        Show or hide the edit control, use the specified attributes to set
        colours/fonts for it.
    */
    virtual void Show(bool show, wxGridCellAttr* attr = NULL);

    /**
        If the editor is enabled by clicking on the cell, this method will be
        called.
    */
    virtual void StartingClick();

    /**
        If the editor is enabled by pressing keys on the grid,
        this will be called to let the editor do something about
        that first key if desired.
    */
    virtual void StartingKey(wxKeyEvent& event);

protected:

    /**
        The dtor is private because only DecRef() can delete us.
    */
    virtual ~wxGridCellEditor();
};



/**
    @class wxGridCellTextEditor

    The editor for string/text data.

    @library{wxadv}
    @category{grid}

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
    virtual void SetParameters(const wxString& params);
};



/**
    @class wxGridCellStringRenderer

    This class may be used to format string data in a cell; it is the default
    for string cells.

    @library{wxadv}
    @category{grid}

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

    The editor for string data allowing to choose from a list of strings.

    @library{wxadv}
    @category{grid}

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
    virtual void SetParameters(const wxString& params);
};



/**
    @class wxGridEditorCreatedEvent


    @library{wxadv}
    @category{grid}
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


    @library{wxadv}
    @category{grid}
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
    bool AltDown() const;

    /**
        Returns @true if the Control key was down at the time of the event.
    */
    bool ControlDown() const;

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
    bool MetaDown() const;

    /**
        Returns @true if the area was selected, @false otherwise.
    */
    bool Selecting();

    /**
        Returns @true if the Shift key was down at the time of the event.
    */
    bool ShiftDown() const;
};



/**
    @class wxGridCellRenderer

    This class is responsible for actually drawing the cell
    in the grid. You may pass it to the wxGridCellAttr (below) to change the
    format of one given cell or to wxGrid::SetDefaultRenderer() to change the
    view of all cells. This is an abstract class, and you will normally use one of
    the
    predefined derived classes or derive your own class from it.

    @library{wxadv}
    @category{grid}

    @see wxGridCellStringRenderer, wxGridCellNumberRenderer,
    wxGridCellFloatRenderer, wxGridCellBoolRenderer
*/
class wxGridCellRenderer
{
public:
    /**

    */
    virtual wxGridCellRenderer* Clone() const = 0;

    /**
        Draw the given cell on the provided DC inside the given rectangle
        using the style specified by the attribute and the default or selected
        state corresponding to the isSelected value.
        This pure virtual function has a default implementation which will
        prepare the DC using the given attribute: it will draw the rectangle
        with the background colour from attr and set the text colour and font.
    */
    virtual void Draw(wxGrid& grid, wxGridCellAttr& attr, wxDC& dc,
                      const wxRect& rect, int row, int col,
                      bool isSelected) = 0;

    /**
        Get the preferred size of the cell for its contents.
    */
    virtual wxSize GetBestSize(wxGrid& grid, wxGridCellAttr& attr, wxDC& dc,
                               int row, int col) = 0;
};



/**
    @class wxGridCellNumberEditor

    The editor for numeric integer data.

    @library{wxadv}
    @category{grid}

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
        Parameters string format is "min,max".
    */
    virtual void SetParameters(const wxString& params);

protected:

    /**
        If the return value is @true, the editor uses a wxSpinCtrl to get user input,
        otherwise it uses a wxTextCtrl.
    */
    bool HasRange() const;

    /**
        String representation of the value.
    */
    wxString GetString() const;
};



/**
    @class wxGridSizeEvent

    This event class contains information about a row/column resize event.

    @library{wxadv}
    @category{grid}
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
    bool AltDown() const;

    /**
        Returns @true if the Control key was down at the time of the event.
    */
    bool ControlDown() const;

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
    bool MetaDown() const;

    /**
        Returns @true if the Shift key was down at the time of the event.
    */
    bool ShiftDown() const;
};



/**
    @class wxGridCellNumberRenderer

    This class may be used to format integer data in a cell.

    @library{wxadv}
    @category{grid}

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

    This class can be used to alter the cells' appearance in
    the grid by changing their colour/font/... from default. An object of this
    class may be returned by wxGridTableBase::GetAttr.

    @library{wxadv}
    @category{grid}
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
    const wxColour& GetBackgroundColour() const;

    /**

    */
    wxGridCellEditor* GetEditor(const wxGrid* grid, int row, int col) const;

    /**

    */
    const wxFont& GetFont() const;

    /**

    */
    wxGridCellRenderer* GetRenderer(const wxGrid* grid, int row, int col) const;

    /**

    */
    const wxColour& GetTextColour() const;

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

    This class may be used to format boolean data in a cell.
    for string cells.

    @library{wxadv}
    @category{grid}

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

    This event class contains information about various grid events.

    @library{wxadv}
    @category{grid}
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
    bool AltDown() const;

    /**
        Returns @true if the Control key was down at the time of the event.
    */
    bool ControlDown() const;

    /**
        Column at which the event occurred.
    */
    virtual int GetCol();

    /**
        Position in pixels at which the event occurred.
    */
    wxPoint GetPosition();

    /**
        Row at which the event occurred.
    */
    virtual int GetRow();

    /**
        Returns @true if the Meta key was down at the time of the event.
    */
    bool MetaDown() const;

    /**
        Returns @true if the user is selecting grid cells, @false -- if
        deselecting.
    */
    bool Selecting();

    /**
        Returns @true if the Shift key was down at the time of the event.
    */
    bool ShiftDown() const;
};



/**
    @class wxGridCellFloatEditor

    The editor for floating point numbers data.

    @library{wxadv}
    @category{grid}

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
    virtual void SetParameters(const wxString& params);
};



/**
    @class wxGrid

    wxGrid and its related classes are used for displaying and editing tabular
    data.
    They provide a rich set of features for display, editing, and interacting
    with a variety of data sources. For simple applications, and to help you
    get started, wxGrid is the only class you need to refer to directly. It
    will set up default instances of the other classes and manage them for you.
    For more complex applications you can derive your own classes for custom
    grid views, grid data tables, cell editors and renderers. The @ref
    overview_grid "wxGrid overview" has examples of simple and more complex applications,
    explains the relationship between the various grid classes and has a
    summary of the keyboard shortcuts and mouse functions provided by wxGrid.

    wxGrid has been greatly expanded and redesigned for wxWidgets 2.2 onwards.
    The new grid classes are reasonably backward-compatible but there are some
    exceptions. There are also easier ways of doing many things compared to the
    previous implementation.

    A wxGridTableBase class holds the actual data to be displayed by a wxGrid
    class. One or more wxGrid classes may act as a view for one table class.
    The default table class is called wxGridStringTable and holds an array of
    strings. An instance of such a class is created by wxGrid::CreateGrid.

    wxGridCellRenderer is the abstract base class for rendereing contents in a
    cell. The following renderers are predefined:
     - wxGridCellStringRenderer,
     - wxGridCellBoolRenderer,
     - wxGridCellFloatRenderer,
     - wxGridCellNumberRenderer.
    The look of a cell can be further defined using wxGridCellAttr. An object
    of this type may be returned by wxGridTableBase::GetAttr.

    wxGridCellEditor is the abstract base class for editing the value of a
    cell. The following editors are predefined:
     - wxGridCellTextEditor
     - wxGridCellBoolEditor
     - wxGridCellChoiceEditor
     - wxGridCellNumberEditor.

    @library{wxadv}
    @category{grid}

    @see @ref overview_grid "wxGrid overview"
*/
class wxGrid : public wxScrolledWindow
{
public:
    /**
        Different selection modes supported by the grid.
     */
    enum wxGridSelectionModes
    {
        /**
            The default selection mode allowing selection of the individual
            cells as well as of the entire rows and columns.
         */
        wxGridSelectCells,

        /**
            The selection mode allowing the selection of the entire rows only.

            The user won't be able to select any cells or columns in this mode.
         */
        wxGridSelectRows,

        /**
            The selection mode allowing the selection of the entire columns only.

            The user won't be able to select any cells or rows in this mode.
         */
        wxGridSelectColumns
    };

    /**
        Default constructor.

        You must call Create() to really create the grid window and also call
        CreateGrid() or SetTable() to initialize the grid contents.
     */
    wxGrid();

    /**
        Constructor creating the grid window.

        You must call either CreateGrid() or SetTable() to initialize the grid
        contents before using it.
    */
    wxGrid(wxWindow* parent,
           wxWindowID id,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize,
           long style = wxWANTS_CHARS,
           const wxString& name = wxGridNameStr);

    /**
        Creates the grid window for an object initialized using the default
        constructor.

        You must call either CreateGrid() or SetTable() to initialize the grid
        contents before using it.
     */
    bool Create(wxWindow* parent,
                wxWindowID id,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxWANTS_CHARS,
                const wxString& name = wxGridNameStr);

    /**
        Destructor.

        This will also destroy the associated grid table unless you passed a
        table object to the grid and specified that the grid should not take
        ownership of the table (see wxGrid::SetTable).
    */
    virtual ~wxGrid();

    /**
        Appends one or more new columns to the right of the grid.

        The @a updateLabels argument is not used at present. If you are using a
        derived grid table class you will need to override
        wxGridTableBase::AppendCols. See InsertCols() for further information.

        @return @true on success or @false if appending columns failed.
    */
    bool AppendCols(int numCols = 1, bool updateLabels = true);

    /**
        Appends one or more new rows to the bottom of the grid.

        The @a updateLabels argument is not used at present. If you are using a
        derived grid table class you will need to override
        wxGridTableBase::AppendRows. See InsertRows() for further information.

        @return @true on success or @false if appending rows failed.
    */
    bool AppendRows(int numRows = 1, bool updateLabels = true);

    /**
        Return @true if the horizontal grid lines stop at the last column
        boundary or @false if they continue to the end of the window.

        The default is to clip grid lines.

        @see ClipHorzGridLines(), AreVertGridLinesClipped()
     */
    bool AreHorzGridLinesClipped() const;

    /**
        Return @true if the vertical grid lines stop at the last row
        boundary or @false if they continue to the end of the window.

        The default is to clip grid lines.

        @see ClipVertGridLines(), AreHorzGridLinesClipped()
     */
    bool AreVertGridLinesClipped() const;

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
        Increments the grid's batch count.

        When the count is greater than zero repainting of the grid is
        suppressed. Each call to BeginBatch must be matched by a later call to
        EndBatch(). Code that does a lot of grid modification can be enclosed
        between BeginBatch and EndBatch calls to avoid screen flicker. The
        final EndBatch will cause the grid to be repainted.

        Notice that you should use wxGridUpdateLocker which ensures that there
        is always a matching EndBatch() call for this BeginBatch() if possible
        instead of calling this method directly.
    */
    void BeginBatch();

    /**
        Convert grid cell coordinates to grid window pixel coordinates.

        This function returns the rectangle that encloses the block of cells
        limited by @a topLeft and @a bottomRight cell in device coords and
        clipped to the client size of the grid window.

        @see CellToRect()
    */
    wxRect BlockToDeviceRect(const wxGridCellCoords& topLeft,
                             const wxGridCellCoords& bottomRight) const;

    /**
        Returns @true if columns can be moved by dragging with the mouse.

        Columns can be moved by dragging on their labels.
    */
    bool CanDragColMove() const;

    /**
        Returns @true if columns can be resized by dragging with the mouse.

        Columns can be resized by dragging the edges of their labels. If grid
        line dragging is enabled they can also be resized by dragging the right
        edge of the column in the grid cell area (see
        wxGrid::EnableDragGridSize).
    */
    bool CanDragColSize() const;

    /**
        Return @true if the dragging of grid lines to resize rows and columns
        is enabled or @false otherwise.
    */
    bool CanDragGridSize() const;

    /**
        Returns @true if rows can be resized by dragging with the mouse.

        Rows can be resized by dragging the edges of their labels. If grid line
        dragging is enabled they can also be resized by dragging the lower edge
        of the row in the grid cell area (see wxGrid::EnableDragGridSize).
    */
    bool CanDragRowSize() const;

    /**
        Returns @true if the in-place edit control for the current grid cell
        can be used and @false otherwise.

        This function always returns @false for the read-only cells.
    */
    bool CanEnableCellControl() const;

    //@{
    /**
        Return the rectangle corresponding to the grid cell's size and position
        in logical coordinates.

        @see BlockToDeviceRect()
    */
    wxRect CellToRect(int row, int col) const;
    const wxRect CellToRect(const wxGridCellCoords& coords) const;

    //@}

    /**
        Clears all data in the underlying grid table and repaints the grid.

        The table is not deleted by this function. If you are using a derived
        table class then you need to override wxGridTableBase::Clear() for this
        function to have any effect.
    */
    void ClearGrid();

    /**
        Deselects all cells that are currently selected.
    */
    void ClearSelection();

    /**
        Change whether the horizontal grid lines are clipped by the end of the
        last column.

        By default the grid lines are not drawn beyond the end of the last
        column but after calling this function with @a clip set to @false they
        will be drawn across the entire grid window.

        @see AreHorzGridLinesClipped(), ClipVertGridLines()
     */
    void ClipHorzGridLines(bool clip);

    /**
        Change whether the vertical grid lines are clipped by the end of the
        last row.

        By default the grid lines are not drawn beyond the end of the last
        row but after calling this function with @a clip set to @false they
        will be drawn across the entire grid window.

        @see AreVertzGridLinesClipped(), ClipHorzGridLines()
     */
    void ClipVertzGridLines(bool clip);

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
                    wxGridSelectionModes selmode = wxGridSelectCells);

    /**
        Deletes one or more columns from a grid starting at the specified
        position.

        The @a updateLabels argument is not used at present. If you are using a
        derived grid table class you will need to override
        wxGridTableBase::DeleteCols. See InsertCols() for further information.

        @return @true on success or @false if deleting columns failed.
    */
    bool DeleteCols(int pos = 0, int numCols = 1, bool updateLabels = true);

    /**
        Deletes one or more rows from a grid starting at the specified position.

        The @a updateLabels argument is not used at present. If you are using a
        derived grid table class you will need to override
        wxGridTableBase::DeleteRows. See InsertRows() for further information.

        @return @true on success or @false if appending rows failed.
    */
    bool DeleteRows(int pos = 0, int numRows = 1, bool updateLabels = true);

    /**
        Disables in-place editing of grid cells.

        Equivalent to calling EnableCellEditControl(@false).
    */
    void DisableCellEditControl();

    /**
        Disables column moving by dragging with the mouse.

        Equivalent to passing @false to EnableDragColMove().
    */
    void DisableDragColMove();

    /**
        Disables column sizing by dragging with the mouse.

        Equivalent to passing @false to EnableDragColSize().
    */
    void DisableDragColSize();

    /**
        Disable mouse dragging of grid lines to resize rows and columns.

        Equivalent to passing @false to EnableDragGridSize()
    */
    void DisableDragGridSize();

    /**
        Disables row sizing by dragging with the mouse.

        Equivalent to passing @false to EnableDragRowSize().
    */
    void DisableDragRowSize();

    /**
        Enables or disables in-place editing of grid cell data.

        The grid will issue either a wxEVT_GRID_EDITOR_SHOWN or
        wxEVT_GRID_EDITOR_HIDDEN event.
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
        Makes the grid globally editable or read-only.

        If the edit argument is @false this function sets the whole grid as
        read-only. If the argument is @true the grid is set to the default
        state where cells may be editable. In the default state you can set
        single grid cells and whole rows and columns to be editable or
        read-only via wxGridCellAttribute::SetReadOnly. For single cells you
        can also use the shortcut function SetReadOnly().

        For more information about controlling grid cell attributes see the
        wxGridCellAttr cell attribute class and the
        @ref overview_grid "wxGrid overview".
    */
    void EnableEditing(bool edit);

    /**
        Turns the drawing of grid lines on or off.
    */
    void EnableGridLines(bool enable = true);

    /**
        Decrements the grid's batch count.

        When the count is greater than zero repainting of the grid is
        suppressed. Each previous call to BeginBatch() must be matched by a
        later call to EndBatch. Code that does a lot of grid modification can
        be enclosed between BeginBatch and EndBatch calls to avoid screen
        flicker. The final EndBatch will cause the grid to be repainted.

        @see wxGridUpdateLocker
    */
    void EndBatch();

    /**
        Overridden wxWindow method.
    */
    void Fit();

    /**
        Causes immediate repainting of the grid.

        Use this instead of the usual wxWindow::Refresh.
    */
    void ForceRefresh();

    /**
        Returns the number of times that BeginBatch() has been called
        without (yet) matching calls to EndBatch(). While
        the grid's batch count is greater than zero the display will not be updated.
    */
    int GetBatchCount();

    /**
        Sets the arguments to the horizontal and vertical text alignment values
        for the grid cell at the specified location.

        Horizontal alignment will be one of @c wxALIGN_LEFT, @c wxALIGN_CENTRE
        or @c wxALIGN_RIGHT.

        Vertical alignment will be one of @c wxALIGN_TOP, @c wxALIGN_CENTRE or
        @c wxALIGN_BOTTOM.
    */
    void GetCellAlignment(int row, int col, int* horiz, int* vert) const;

    /**
        Returns the background colour of the cell at the specified location.
    */
    wxColour GetCellBackgroundColour(int row, int col) const;

    /**
        Returns a pointer to the editor for the cell at the specified location.

        See wxGridCellEditor and the @ref overview_grid "wxGrid overview"
        for more information about cell editors and renderers.

        The caller must call DecRef() on the returned pointer.
    */
    wxGridCellEditor* GetCellEditor(int row, int col) const;

    /**
        Returns the font for text in the grid cell at the specified location.
    */
    wxFont GetCellFont(int row, int col) const;

    /**
        Returns a pointer to the renderer for the grid cell at the specified
        location.

        See wxGridCellRenderer and the @ref overview_grid "wxGrid overview"
        for more information about cell editors and renderers.

        The caller must call DecRef() on the returned pointer.
    */
    wxGridCellRenderer* GetCellRenderer(int row, int col) const;

    /**
        Returns the text colour for the grid cell at the specified location.
    */
    wxColour GetCellTextColour(int row, int col) const;

    //@{
    /**
        Returns the string contained in the cell at the specified location.

        For simple applications where a grid object automatically uses a
        default grid table of string values you use this function together with
        SetCellValue() to access cell values. For more complex applications
        where you have derived your own grid table class that contains various
        data types (e.g. numeric, boolean or user-defined custom types) then
        you only use this function for those cells that contain string values.

        See wxGridTableBase::CanGetValueAs and the @ref overview_grid "wxGrid overview"
        for more information.
    */
    wxString GetCellValue(int row, int col) const;
    const wxString  GetCellValue(const wxGridCellCoords& coords) const;
    //@}

    /**
        Returns the column ID of the specified column position.
    */
    int GetColAt(int colPos) const;

    /**
        Returns the pen used for vertical grid lines.

        This virtual function may be overridden in derived classes in order to
        change the appearance of individual grid lines for the given column @e
        col.

        See GetRowGridLinePen() for an example.
    */
    virtual wxPen GetColGridLinePen(int col);

    /**
        Sets the arguments to the current column label alignment values.

        Horizontal alignment will be one of @c wxALIGN_LEFT, @c wxALIGN_CENTRE
        or @c wxALIGN_RIGHT.

        Vertical alignment will be one of @c wxALIGN_TOP, @c wxALIGN_CENTRE or
        @c wxALIGN_BOTTOM.
    */
    void GetColLabelAlignment(int* horiz, int* vert) const;

    /**
        Returns the current height of the column labels.
    */
    int GetColLabelSize() const;

    /**
        Returns the specified column label.

        The default grid table class provides column labels of the form
        A,B...Z,AA,AB...ZZ,AAA... If you are using a custom grid table you can
        override wxGridTableBase::GetColLabelValue to provide your own labels.
    */
    wxString GetColLabelValue(int col) const;

    /**
        Returns the minimal width to which a column may be resized.

        Use SetColMinimalAcceptableWidth() to change this value globally or
        SetColMinimalWidth() to do it for individual columns.
    */
    int GetColMinimalAcceptableWidth() const;

    /**
        Returns the position of the specified column.
    */
    int GetColPos(int colID) const;

    /**
        Returns the width of the specified column.
    */
    int GetColSize(int col) const;

    /**
        Returns the default cell alignment.

        Horizontal alignment will be one of @c wxALIGN_LEFT, @c wxALIGN_CENTRE
        or @c wxALIGN_RIGHT.

        Vertical alignment will be one of @c wxALIGN_TOP, @c wxALIGN_CENTRE or
        @c wxALIGN_BOTTOM.

        @see SetDefaultCellAlignment()
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

        See wxGridCellEditor and the @ref overview_grid "wxGrid overview"
        for more information about cell editors and renderers.
    */
    wxGridCellEditor* GetDefaultEditor() const;

    //@{
    /**
        Returns the default editor for the specified cell.

        The base class version returns the editor appropriate for the current
        cell type but this method may be overridden in the derived classes to
        use custom editors for some cells by default.

        Notice that the same may be usually achieved in simpler way by
        associating a custom editor with the given cell or cells.

        The caller must call DecRef() on the returned pointer.
    */
    virtual wxGridCellEditor* GetDefaultEditorForCell(int row, int col) const;
    wxGridCellEditor*  GetDefaultEditorForCell(const wxGridCellCoords& c) const;
    //@}

    /**
        Returns the default editor for the cells containing values of the given
        type.

        The base class version returns the editor which was associated with the
        specified @a typeName when it was registered RegisterDataType() but
        this function may be overridden to return something different. This
        allows to override an editor used for one of the standard types.

        The caller must call DecRef() on the returned pointer.
    */
    virtual wxGridCellEditor *
    GetDefaultEditorForType(const wxString& typeName) const;

    /**
        Returns the pen used for grid lines.

        This virtual function may be overridden in derived classes in order to
        change the appearance of grid lines. Note that currently the pen width
        must be 1.

        @see GetColGridLinePen(), GetRowGridLinePen()
    */
    virtual wxPen GetDefaultGridLinePen();

    /**
        Returns a pointer to the current default grid cell renderer.

        See wxGridCellRenderer and the @ref overview_grid "wxGrid overview"
        for more information about cell editors and renderers.

        The caller must call DecRef() on the returned pointer.
    */
    wxGridCellRenderer* GetDefaultRenderer() const;

    /**
        Returns the default renderer for the given cell.

        The base class version returns the renderer appropriate for the current
        cell type but this method may be overridden in the derived classes to
        use custom renderers for some cells by default.

        The caller must call DecRef() on the returned pointer.
    */
    virtual wxGridCellRenderer *GetDefaultRendererForCell(int row, int col) const;

    /**
        Returns the default renderer for the cell containing values of the
        given type.

        @see GetDefaultEditorForType()
    */
    virtual wxGridCellRenderer *
    GetDefaultRendererForType(const wxString& typeName) const;

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
        Returns the total number of grid columns.

        This is the same as the number of columns in the underlying grid
        table.
    */
    int GetNumberCols() const;

    /**
        Returns the total number of grid rows.

        This is the same as the number of rows in the underlying grid table.
    */
    int GetNumberRows() const;

    /**
        Returns the attribute for the given cell creating one if necessary.

        If the cell already has an attribute, it is returned. Otherwise a new
        attribute is created, associated with the cell and returned. In any
        case the caller must call DecRef() on the returned pointer.

        This function may only be called if CanHaveAttributes() returns @true.
    */
    wxGridCellAttr *GetOrCreateCellAttr(int row, int col) const;

    /**
        Returns the pen used for horizontal grid lines.

        This virtual function may be overridden in derived classes in order to
        change the appearance of individual grid line for the given row @e row.

        Example:
        @code
           // in a grid displaying music notation, use a solid black pen between
           // octaves (C0=row 127, C1=row 115 etc.)
           wxPen MidiGrid::GetRowGridLinePen(int row)
           {
               if ( row % 12 == 7 )
                  return wxPen(*wxBLACK, 1, wxSOLID);
               else
                  return GetDefaultGridLinePen();
           }
        @endcode
    */
    virtual wxPen GetRowGridLinePen(int row);

    /**
        Returns the alignment used for row labels.

        Horizontal alignment will be one of @c wxALIGN_LEFT, @c wxALIGN_CENTRE
        or @c wxALIGN_RIGHT.

        Vertical alignment will be one of @c wxALIGN_TOP, @c wxALIGN_CENTRE or
        @c wxALIGN_BOTTOM.
    */
    void GetRowLabelAlignment(int* horiz, int* vert) const;

    /**
        Returns the current width of the row labels.
    */
    int GetRowLabelSize() const;

    /**
        Returns the specified row label.

        The default grid table class provides numeric row labels. If you are
        using a custom grid table you can override
        wxGridTableBase::GetRowLabelValue to provide your own labels.
    */
    wxString GetRowLabelValue(int row) const;

    /**
        Returns the minimal size to which rows can be resized.

        Use SetRowMinimalAcceptableHeight() to change this value globally or
        SetRowMinimalHeight() to do it for individual cells.

        @see GetColMinimalAcceptableWidth()
    */
    int GetRowMinimalAcceptableHeight() const;

    /**
        Returns the height of the specified row.
    */
    int GetRowSize(int row) const;

    /**
        Returns the number of pixels per horizontal scroll increment.

        The default is 15.

        @see GetScrollLineY(), SetScrollLineX(), SetScrollLineY()
    */
    int GetScrollLineX() const;

    /**
        Returns the number of pixels per vertical scroll increment.

        The default is 15.

        @see GetScrollLineX(), SetScrollLineX(), SetScrollLineY()
    */
    int GetScrollLineY() const;

    /**
        Returns an array of individually selected cells.

        Notice that this array does @em not contain all the selected cells in
        general as it doesn't include the cells selected as part of column, row
        or block selection. You must use this method, GetSelectedCols(),
        GetSelectedRows() and GetSelectionBlockTopLeft() and
        GetSelectionBlockBottomRight() methods to obtain the entire selection
        in general.

        Please notice this behaviour is by design and is needed in order to
        support grids of arbitrary size (when an entire column is selected in
        a grid with a million of columns, we don't want to create an array with
        a million of entries in this function, instead it returns an empty
        array and GetSelectedCols() returns an array containing one element).
    */
    wxGridCellCoordsArray GetSelectedCells() const;

    /**
        Returns an array of selected columns.

        Please notice that this method alone is not sufficient to find all the
        selected columns as it contains only the columns which were
        individually selected but not those being part of the block selection
        or being selected in virtue of all of their cells being selected
        individually, please see GetSelectedCells() for more details.
    */
    wxArrayInt GetSelectedCols() const;

    /**
        Returns an array of selected rows.

        Please notice that this method alone is not sufficient to find all the
        selected rows as it contains only the rows which were individually
        selected but not those being part of the block selection or being
        selected in virtue of all of their cells being selected individually,
        please see GetSelectedCells() for more details.
    */
    wxArrayInt GetSelectedRows() const;

    /**
        Access or update the selection fore/back colours
    */
    wxColour GetSelectionBackground() const;

    /**
        Returns an array of the bottom right corners of blocks of selected
        cells.

        Please see GetSelectedCells() for more information about the selection
        representation in wxGrid.

        @see GetSelectionBlockTopLeft()
    */
    wxGridCellCoordsArray GetSelectionBlockBottomRight() const;

    /**
        Returns an array of the top left corners of blocks of selected cells.

        Please see GetSelectedCells() for more information about the selection
        representation in wxGrid.

        @see GetSelectionBlockBottomRight()
    */
    wxGridCellCoordsArray GetSelectionBlockTopLeft() const;

    /**
        Returns the colour used for drawing the selection foreground.
    */
    wxColour GetSelectionForeground() const;

    /**
        Returns the current selection mode.

        @see SetSelectionMode().
    */
    wxGridSelectionModes GetSelectionMode() const;

    /**
        Returns a base pointer to the current table object.

        The returned pointer is still owned by the grid.
    */
    wxGridTableBase *GetTable() const;

    //@{
    /**
        Make the given cell current and ensure it is visible.

        This method is equivalent to calling MakeCellVisible() and
        SetGridCursor() and so, as with the latter, a wxEVT_GRID_SELECT_CELL
        event is generated by it and the selected cell doesn't change if the
        event is vetoed.
     */
    void GoToCell(int row, int col);
    void GoToCell(const wxGridCellCoords& coords);
    //@}

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
        Hides the row labels by calling SetRowLabelSize() with a size of 0.

        The labels can be shown again by calling SetRowLabelSize() with a width
        greater than 0.
    */
    void HideRowLabels();

    /**
        Inserts one or more new columns into a grid with the first new column
        at the specified position.

        Notice that inserting the columns in the grid requires grid table
        cooperation: when this method is called, grid object begins by
        requesting the underlying grid table to insert new columns. If this is
        successful the table notifies the grid and the grid updates the
        display. For a default grid (one where you have called
        wxGrid::CreateGrid) this process is automatic. If you are using a
        custom grid table (specified with wxGrid::SetTable) then you must
        override wxGridTableBase::InsertCols() in your derived table class.

        @param pos
            The position which the first newly inserted column will have.
        @param numCols
            The number of columns to insert.
        @param updateLabels
            Currently not used.
        @return
            @true if the columns were successfully inserted, @false if an error
            occurred (most likely the table couldn't be updated).
    */
    bool InsertCols(int pos = 0, int numCols = 1, bool updateLabels = true);

    /**
        Inserts one or more new rows into a grid with the first new row at the
        specified position.

        Notice that you must implement wxGridTableBase::InsertRows() if you use
        a grid with a custom table, please see InsertCols() for more
        information.

        @param pos
            The position which the first newly inserted row will have.
        @param numRows
            The number of rows to insert.
        @param updateLabels
            Currently not used.
        @return
            @true if the rows were successfully inserted, @false if an error
            occurred (most likely the table couldn't be updated).
    */
    bool InsertRows(int pos = 0, int numRows = 1, bool updateLabels = true);

    /**
        Returns @true if the in-place edit control is currently enabled.
    */
    bool IsCellEditControlEnabled() const;

    /**
        Returns @true if the current cell is read-only.

        @see SetReadOnly(), IsReadOnly()
    */
    bool IsCurrentCellReadOnly() const;

    /**
        Returns @false if the whole grid has been set as read-only or @true
        otherwise.

        See EnableEditing() for more information about controlling the editing
        status of grid cells.
    */
    bool IsEditable() const;

    //@{
    /**
        Is this cell currently selected?
    */
    bool IsInSelection(int row, int col) const;
    bool IsInSelection(const wxGridCellCoords& coords) const;
    //@}

    /**
        Returns @true if the cell at the specified location can't be edited.

        @see SetReadOnly(), IsCurrentCellReadOnly()
    */
    bool IsReadOnly(int row, int col) const;

    /**
        Returns @true if there are currently any selected cells, rows, columns
        or blocks.
    */
    bool IsSelection() const;

    //@{
    /**
        Returns @true if a cell is either wholly or at least partially visible
        in the grid window.

        By default, the cell must be entirely visible for this function to
        return true but if @a wholeCellVisible is @false, the function returns
        @true even if the cell is only partially visible.
    */
    bool IsVisible(int row, int col, bool wholeCellVisible = true) const;
    const bool IsVisible(const wxGridCellCoords& coords,
                         bool wholeCellVisible = true) const;
    //@}

    //@{
    /**
        Brings the specified cell into the visible grid cell area with minimal
        scrolling.

        Does nothing if the cell is already visible.
    */
    void MakeCellVisible(int row, int col);
    void MakeCellVisible(const wxGridCellCoords& coords);
    //@}

    /**
        Moves the grid cursor down by one row.

        If a block of cells was previously selected it will expand if the
        argument is @true or be cleared if the argument is @false.
    */
    bool MoveCursorDown(bool expandSelection);

    /**
        Moves the grid cursor down in the current column such that it skips to
        the beginning or end of a block of non-empty cells.

        If a block of cells was previously selected it will expand if the
        argument is @true or be cleared if the argument is @false.
    */
    bool MoveCursorDownBlock(bool expandSelection);

    /**
        Moves the grid cursor left by one column.

        If a block of cells was previously selected it will expand if the
        argument is @true or be cleared if the argument is @false.
    */
    bool MoveCursorLeft(bool expandSelection);

    /**
        Moves the grid cursor left in the current row such that it skips to the
        beginning or end of a block of non-empty cells.

        If a block of cells was previously selected it will expand if the
        argument is @true or be cleared if the argument is @false.
    */
    bool MoveCursorLeftBlock(bool expandSelection);

    /**
        Moves the grid cursor right by one column.

        If a block of cells was previously selected it will expand if the
        argument is @true or be cleared if the argument is @false.
    */
    bool MoveCursorRight(bool expandSelection);

    /**
        Moves the grid cursor right in the current row such that it skips to
        the beginning or end of a block of non-empty cells.

        If a block of cells was previously selected it will expand if the
        argument is @true or be cleared if the argument is @false.
    */
    bool MoveCursorRightBlock(bool expandSelection);

    /**
        Moves the grid cursor up by one row.

        If a block of cells was previously selected it will expand if the
        argument is @true or be cleared if the argument is @false.
    */
    bool MoveCursorUp(bool expandSelection);

    /**
        Moves the grid cursor up in the current column such that it skips to
        the beginning or end of a block of non-empty cells.

        If a block of cells was previously selected it will expand if the
        argument is @true or be cleared if the argument is @false.
    */
    bool MoveCursorUpBlock(bool expandSelection);

    /**
        Moves the grid cursor down by some number of rows so that the previous
        bottom visible row becomes the top visible row.
    */
    bool MovePageDown();

    /**
        Moves the grid cursor up by some number of rows so that the previous
        top visible row becomes the bottom visible row.
    */
    bool MovePageUp();

    /**
        Register a new data type.

        The data types allow to naturally associate specific renderers and
        editors to the cells containing values of the given type. For example,
        the grid automatically registers a data type with the name @c
        wxGRID_VALUE_STRING which uses wxGridCellStringRenderer and
        wxGridCellTextEditor as its renderer and editor respectively -- this is
        the data type used by all the cells of the default wxGridStringTable,
        so this renderer and editor are used by default for all grid cells.

        However if a custom table returns @c wxGRID_VALUE_BOOL from its
        wxGridTableBase::GetTypeName() method, then wxGridCellBoolRenderer and
        wxGridCellBoolEditor are used for it because the grid also registers a
        boolean data type with this name.

        And as this mechanism is completely generic, you may register your own
        data types using your own custom renderers and editors. Just remember
        that the table must identify a cell as being of the given type for them
        to be used for this cell.

        @param typeName
            Name of the new type. May be any string, but if the type name is
            the same as the name of an already registered type, including one
            of the standard ones (which are @c wxGRID_VALUE_STRING, @c
            wxGRID_VALUE_BOOL, @c wxGRID_VALUE_NUMBER, @c wxGRID_VALUE_FLOAT
            and @c wxGRID_VALUE_CHOICE), then the new registration information
            replaces the previously used renderer and editor.
        @param renderer
            The renderer to use for the cells of this type. Its ownership is
            taken by the grid, i.e. it will call DecRef() on this pointer when
            it doesn't need it any longer.
        @param editor
            The editor to use for the cells of this type. Its ownership is also
            taken by the grid.
    */
    void RegisterDataType(const wxString& typeName,
                          wxGridCellRenderer* renderer,
                          wxGridCellEditor* editor);

    /**
        Sets the value of the current grid cell to the current in-place edit
        control value.

        This is called automatically when the grid cursor moves from the
        current cell to a new cell. It is also a good idea to call this
        function when closing a grid since any edits to the final cell location
        will not be saved otherwise.
    */
    void SaveEditControlValue();

    /**
        Selects all cells in the grid.
    */
    void SelectAll();

    //@{
    /**
        Selects a rectangular block of cells.

        If @a addToSelected is @false then any existing selection will be
        deselected; if @true the column will be added to the existing
        selection.
    */
    void SelectBlock(int topRow, int leftCol, int bottomRow, int rightCol,
                     bool addToSelected = false);
    void SelectBlock(const wxGridCellCoords& topLeft,
                     const wxGridCellCoords& bottomRight,
                     bool addToSelected = false);
    //@}

    /**
        Selects the specified column.

        If @a addToSelected is @false then any existing selection will be
        deselected; if @true the column will be added to the existing
        selection.

        This method won't select anything if the current selection mode is
        wxGridSelectRows.
    */
    void SelectCol(int col, bool addToSelected = false);

    /**
        Selects the specified row.

        If @a addToSelected is @false then any existing selection will be
        deselected; if @true the row will be added to the existing selection.

        This method won't select anything if the current selection mode is
        wxGridSelectColumns.
    */
    void SelectRow(int row, bool addToSelected = false);

    //@{
    /**
        Sets the horizontal and vertical alignment for grid cell text at the
        specified location.

        Horizontal alignment should be one of @c wxALIGN_LEFT, @c
        wxALIGN_CENTRE or @c wxALIGN_RIGHT.

        Vertical alignment should be one of @c wxALIGN_TOP, @c wxALIGN_CENTRE
        or @c wxALIGN_BOTTOM.
    */
    void SetCellAlignment(int row, int col, int horiz, int vert);
    void SetCellAlignment(int align, int row, int col);
    //@}

    //@{
    /**
        Set the background colour for the given cell or all cells by default.
    */
    void SetCellBackgroundColour(int row, int col, const wxColour& colour);
    //@}

    /**
        Sets the editor for the grid cell at the specified location.

        The grid will take ownership of the pointer.

        See wxGridCellEditor and the @ref overview_grid "wxGrid overview"
        for more information about cell editors and renderers.
    */
    void SetCellEditor(int row, int col, wxGridCellEditor* editor);

    /**
        Sets the font for text in the grid cell at the specified location.
    */
    void SetCellFont(int row, int col, const wxFont& font);

    /**
        Sets the renderer for the grid cell at the specified location.

        The grid will take ownership of the pointer.

        See wxGridCellRenderer and the @ref overview_grid "wxGrid overview"
        for more information about cell editors and renderers.
    */
    void SetCellRenderer(int row, int col, wxGridCellRenderer* renderer);

    //@{
    /**
        Sets the text colour for the given cell or all cells by default.
    */
    void SetCellTextColour(int row, int col, const wxColour& colour);
    void SetCellTextColour(const wxColour& val, int row, int col);
    void SetCellTextColour(const wxColour& colour);
    //@}

    //@{
    /**
        Sets the string value for the cell at the specified location.

        For simple applications where a grid object automatically uses a
        default grid table of string values you use this function together with
        GetCellValue() to access cell values. For more complex applications
        where you have derived your own grid table class that contains various
        data types (e.g. numeric, boolean or user-defined custom types) then
        you only use this function for those cells that contain string values.
        The last form is for backward compatibility only.

        See wxGridTableBase::CanSetValueAs and the @ref overview_grid
        "wxGrid overview" for more information.
    */
    void SetCellValue(int row, int col, const wxString& s);
    void SetCellValue(const wxGridCellCoords& coords, const wxString& s);
    void SetCellValue(const wxString& val, int row, int col);
    //@}

    /**
        Sets the cell attributes for all cells in the specified column.

        For more information about controlling grid cell attributes see the
        wxGridCellAttr cell attribute class and the @ref overview_grid "wxGrid overview".
    */
    void SetColAttr(int col, wxGridCellAttr* attr);

    /**
        Sets the specified column to display boolean values.

        @see SetColFormatCustom()
    */
    void SetColFormatBool(int col);

    /**
        Sets the specified column to display data in a custom format.

        This method provides an alternative to defining a custom grid table
        which would return @a typeName from its GetTypeName() method for the
        cells in this column: while it doesn't really change the type of the
        cells in this column, it does associate the renderer and editor used
        for the cells of the specified type with them.

        See the @ref overview_grid "wxGrid overview" for more
        information on working with custom data types.
    */
    void SetColFormatCustom(int col, const wxString& typeName);

    /**
        Sets the specified column to display floating point values with the
        given width and precision.

        @see SetColFormatCustom()
    */
    void SetColFormatFloat(int col, int width = -1, int precision = -1);

    /**
        Sets the specified column to display integer values.

        @see SetColFormatCustom()
    */
    void SetColFormatNumber(int col);

    /**
        Sets the horizontal and vertical alignment of column label text.

        Horizontal alignment should be one of @c wxALIGN_LEFT, @c
        wxALIGN_CENTRE or @c wxALIGN_RIGHT.
        Vertical alignment should be one of @c wxALIGN_TOP, @c wxALIGN_CENTRE
        or @c wxALIGN_BOTTOM.
    */
    void SetColLabelAlignment(int horiz, int vert);

    /**
        Sets the height of the column labels.

        If @a height equals to @c wxGRID_AUTOSIZE then height is calculated
        automatically so that no label is truncated. Note that this could be
        slow for a large table.
    */
    void SetColLabelSize(int height);

    /**
        Set the value for the given column label.

        If you are using a custom grid table you must override
        wxGridTableBase::SetColLabelValue for this to have any effect.
    */
    void SetColLabelValue(int col, const wxString& value);

    /**
        Sets the minimal width to which the user can resize columns.

        @see GetColMinimalAcceptableWidth()
    */
    void SetColMinimalAcceptableWidth(int width);

    /**
        Sets the minimal width for the specified column.

        It is usually best to call this method during grid creation as calling
        it later will not resize the column to the given minimal width even if
        it is currently narrower than it.

        @a width must be greater than the minimal acceptable column width as
        returned by GetColMinimalAcceptableWidth().
    */
    void SetColMinimalWidth(int col, int width);

    /**
        Sets the position of the specified column.
    */
    void SetColPos(int colID, int newPos);

    /**
        Sets the width of the specified column.

        Notice that this function does not refresh the grid, you need to call
        ForceRefresh() to make the changes take effect immediately.

        @param col
            The column index.
        @param width
            The new column width in pixels or a negative value to fit the
            column width to its label width.
    */
    void SetColSize(int col, int width);

    /**
        Sets the default horizontal and vertical alignment for grid cell text.

        Horizontal alignment should be one of @c wxALIGN_LEFT, @c
        wxALIGN_CENTRE or @c wxALIGN_RIGHT.
        Vertical alignment should be one of @c wxALIGN_TOP, @c wxALIGN_CENTRE
        or @c wxALIGN_BOTTOM.
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
        Sets the default width for columns in the grid.

        This will only affect columns subsequently added to the grid unless
        @a resizeExistingCols is @true.

        If @a width is less than GetColMinimalAcceptableWidth(), then the
        minimal acceptable width is used instead of it.
    */
    void SetDefaultColSize(int width, bool resizeExistingCols = false);

    /**
        Sets the default editor for grid cells.

        The grid will take ownership of the pointer.

        See wxGridCellEditor and the @ref overview_grid "wxGrid overview"
        for more information about cell editors and renderers.
    */
    void SetDefaultEditor(wxGridCellEditor* editor);

    /**
        Sets the default renderer for grid cells.

        The grid will take ownership of the pointer.

        See wxGridCellRenderer and the @ref overview_grid "wxGrid overview"
        for more information about cell editors and renderers.
    */
    void SetDefaultRenderer(wxGridCellRenderer* renderer);

    /**
        Sets the default height for rows in the grid.

        This will only affect rows subsequently added to the grid unless
        @a resizeExistingRows is @true.

        If @a height is less than GetRowMinimalAcceptableHeight(), then the
        minimal acceptable heihgt is used instead of it.
    */
    void SetDefaultRowSize(int height, bool resizeExistingRows = false);

    //@{
    /**
        Set the grid cursor to the specified cell.

        The grid cursor indicates the current cell and can be moved by the user
        using the arrow keys or the mouse.

        Calling this function generates a wxEVT_GRID_SELECT_CELL event and if
        the event handler vetoes this event, the cursor is not moved.

        This function doesn't make the target call visible, use GoToCell() to
        do this.
    */
    void SetGridCursor(int row, int col);
    void SetGridCursor(const wxGridCellCoords& coords);
    //@}

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
        Sets the extra margins used around the grid area.

        A grid may occupy more space than needed for its data display and
        this function allows to set how big this extra space is
    */
    void SetMargins(int extraWidth, int extraHeight);

    /**
        Makes the cell at the specified location read-only or editable.

        @see IsReadOnly()
    */
    void SetReadOnly(int row, int col, bool isReadOnly = true);

    /**
        Sets the cell attributes for all cells in the specified row.

        The grid takes ownership of the attribute pointer.

        See the wxGridCellAttr class for more information about controlling
        cell attributes.
    */
    void SetRowAttr(int row, wxGridCellAttr* attr);

    /**
        Sets the horizontal and vertical alignment of row label text.

        Horizontal alignment should be one of @c wxALIGN_LEFT, @c
        wxALIGN_CENTRE or @c wxALIGN_RIGHT.
        Vertical alignment should be one of @c wxALIGN_TOP, @c wxALIGN_CENTRE
        or @c wxALIGN_BOTTOM.
    */
    void SetRowLabelAlignment(int horiz, int vert);

    /**
        Sets the width of the row labels.

        If @a width equals @c wxGRID_AUTOSIZE then width is calculated
        automatically so that no label is truncated. Note that this could be
        slow for a large table.
    */
    void SetRowLabelSize(int width);

    /**
        Sets the value for the given row label.

        If you are using a derived grid table you must override
        wxGridTableBase::SetRowLabelValue for this to have any effect.
    */
    void SetRowLabelValue(int row, const wxString& value);

    /**
        Sets the minimal row height used by default.

        See SetColMinimalAcceptableWidth() for more information.
    */
    void SetRowMinimalAcceptableHeight(int height);

    /**
        Sets the minimal height for the specified row.

        See SetColMinimalWidth() for more information.
    */
    void SetRowMinimalHeight(int row, int height);

    /**
        Sets the height of the specified row.

        See SetColSize() for more information.
    */
    void SetRowSize(int row, int height);

    /**
        Sets the number of pixels per horizontal scroll increment.

        The default is 15.

        @see GetScrollLineX(), GetScrollLineY(), SetScrollLineY()
    */
    void SetScrollLineX(int x);

    /**
        Sets the number of pixels per vertical scroll increment.

        The default is 15.

        @see GetScrollLineX(), GetScrollLineY(), SetScrollLineX()
    */
    void SetScrollLineY(int y);

    /**
        Set the colour to be used for drawing the selection background.
    */
    void SetSelectionBackground(const wxColour& c);

    /**
        Set the colour to be used for drawing the selection foreground.
    */
    void SetSelectionForeground(const wxColour& c);

    /**
        Set the selection behaviour of the grid.

        The existing selection is converted to conform to the new mode if
        possible and discarded otherwise (e.g. any individual selected cells
        are deselected if the new mode allows only the selection of the entire
        rows or columns).
    */
    void SetSelectionMode(wxGridSelectionModes selmode);

    /**
        Passes a pointer to a custom grid table to be used by the grid.

        This should be called after the grid constructor and before using the
        grid object. If @a takeOwnership is set to @true then the table will be
        deleted by the wxGrid destructor.

        Use this function instead of CreateGrid() when your application
        involves complex or non-string data or data sets that are too large to
        fit wholly in memory.
    */
    bool SetTable(wxGridTableBase* table,
                  bool takeOwnership = false,
                  wxGridSelectionModes selmode = wxGridSelectCells);

    /**
        Call this in order to make the column labels use a native look by using
        wxRenderer::DrawHeaderButton internally.

        There is no equivalent method for drawing row columns as there is not
        native look for that. This option is useful when using wxGrid for
        displaying tables and not as a spread-sheet.
    */
    void SetUseNativeColLabels(bool native = true);

    /**
        Displays the in-place cell edit control for the current cell.
    */
    void ShowCellEditControl();

    /**
        Returns the column at the given pixel position.

        @param x
            The x position to evaluate.
        @param clipToMinMax
            If @true, rather than returning wxNOT_FOUND, it returns either the
            first or last column depending on whether x is too far to the left
            or right respectively.
        @return
            The column index or wxNOT_FOUND.
    */
    int XToCol(int x, bool clipToMinMax = false) const;

    /**
        Returns the column whose right hand edge is close to the given logical
        x position.

        If no column edge is near to this position @c wxNOT_FOUND is returned.
    */
    int XToEdgeOfCol(int x) const;

    //@{
    /**
        Translates logical pixel coordinates to the grid cell coordinates.

        Notice that this function expects logical coordinates on input so if
        you use this function in a mouse event handler you need to translate
        the mouse position, which is expressed in device coordinates, to
        logical ones.

        @see XToCol(), YToRow()
     */

    // XYToCell(int, int, wxGridCellCoords&) overload is intentionally
    // undocumented, using it is ugly and non-const reference parameters are
    // not used in wxWidgets API

    wxGridCellCoords XYToCell(int x, int y) const;
    wxGridCellCoords XYToCell(const wxPoint& pos) const;

    //@}

    /**
        Returns the row whose bottom edge is close to the given logical y
        position.

        If no row edge is near to this position @c wxNOT_FOUND is returned.
    */
    int YToEdgeOfRow(int y) const;

    /**
        Returns the grid row that corresponds to the logical y coordinate.

        Returns @c wxNOT_FOUND if there is no row at the y position.
    */
    int YToRow(int y, bool clipToMinMax = false) const;

protected:
    /**
        Returns @true if this grid has support for cell attributes.

        The grid supports attributes if it has the associated table which, in
        turn, has attributes support, i.e. wxGridTableBase::CanHaveAttributes()
        returns @true.
    */
    bool CanHaveAttributes() const;

    /**
        Get the minimal width of the given column/row.

        The value returned by this function may be different than that returned
        by GetColMinimalAcceptableWidth() if SetColMinimalWidth() had been
        called for this column.
    */
    int GetColMinimalWidth(int col) const;

    /**
        Returns the coordinate of the right border specified column.
    */
    int GetColRight(int col) const;

    /**
        Returns the coordinate of the left border specified column.
    */
    int GetColLeft(int col) const;

    /**
        Returns the minimal size for the given column.

        The value returned by this function may be different than that returned
        by GetRowMinimalAcceptableHeight() if SetRowMinimalHeight() had been
        called for this row.
    */
    int GetRowMinimalHeight(int col) const;
};



/**
    @class wxGridCellBoolEditor

    The editor for boolean data.

    @library{wxadv}
    @category{grid}

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
    @category{grid}
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

