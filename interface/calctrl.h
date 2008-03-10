/////////////////////////////////////////////////////////////////////////////
// Name:        calctrl.h
// Purpose:     interface of wxCalendarEvent
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxCalendarEvent
    @wxheader{calctrl.h}

    The wxCalendarEvent class is used together with
    wxCalendarCtrl.

    @library{wxadv}
    @category{events}

    @see wxCalendarCtrl
*/
class wxCalendarEvent : public wxDateEvent
{
public:
    /**
        Returns the week day on which the user clicked in
        @c EVT_CALENDAR_WEEKDAY_CLICKED handler. It doesn't make sense to call
        this function in other handlers.
    */
    wxDateTime::WeekDay GetWeekDay() const;

    /**
        Sets the week day carried by the event, normally only used by the library
        internally.
    */
    void SetWeekDay(wxDateTime::WeekDay day);
};



/**
    @class wxCalendarDateAttr
    @wxheader{calctrl.h}

    wxCalendarDateAttr is a custom attributes for a calendar date. The objects of
    this class are used with wxCalendarCtrl.

    @library{wxadv}
    @category{misc}

    @see wxCalendarCtrl
*/
class wxCalendarDateAttr
{
public:
    //@{
    /**
        The constructors.
    */
    wxCalendarDateAttr();
    wxCalendarDateAttr(const wxColour& colText,
                       const wxColour& colBack = wxNullColour,
                       const wxColour& colBorder = wxNullColour,
                       const wxFont& font = wxNullFont,
                       wxCalendarDateBorder border = wxCAL_BORDER_NONE);
    wxCalendarDateAttr(wxCalendarDateBorder border,
                       const wxColour& colBorder = wxNullColour);
    //@}

    /**
        Returns the background colour to use for the item with this attribute.
    */
    const wxColour GetBackgroundColour() const;

    /**
        Returns the border() to use for the item with this attribute.
    */
    wxCalendarDateBorder GetBorder() const;

    /**
        Returns the border colour to use for the item with this attribute.
    */
    const wxColour GetBorderColour() const;

    /**
        Returns the font to use for the item with this attribute.
    */
    const wxFont GetFont() const;

    /**
        Returns the text colour to use for the item with this attribute.
    */
    const wxColour GetTextColour() const;

    /**
        Returns @true if this attribute specifies a non-default text background
        colour.
    */
    bool HasBackgroundColour() const;

    /**
        Returns @true if this attribute specifies a non-default (i.e. any) border.
    */
    bool HasBorder() const;

    /**
        Returns @true if this attribute specifies a non-default border colour.
    */
    bool HasBorderColour() const;

    /**
        Returns @true if this attribute specifies a non-default font.
    */
    bool HasFont() const;

    /**
        Returns @true if this item has a non-default text foreground colour.
    */
    bool HasTextColour() const;

    /**
        Returns @true if this attribute specifies that this item should be
        displayed as a holiday.
    */
    bool IsHoliday() const;

    /**
        Sets the text background colour to use.
    */
    void SetBackgroundColour(const wxColour& colBack);

    /**
        Sets the @ref overview_wxcalendardateattr "border kind"
    */
    void SetBorder(wxCalendarDateBorder border);

    /**
        Sets the border colour to use.
    */
    void SetBorderColour(const wxColour& col);

    /**
        Sets the font to use.
    */
    void SetFont(const wxFont& font);

    /**
        Display the date with this attribute as a holiday.
    */
    void SetHoliday(bool holiday);

    /**
        Sets the text (foreground) colour to use.
    */
    void SetTextColour(const wxColour& colText);
};



/**
    @class wxCalendarCtrl
    @wxheader{calctrl.h}

    The calendar control allows the user to pick a date. For this,
    it displays a window containing several parts: a control at the top to pick the
    month
    and the year (either or both of them may be disabled), and a month
    area below them which shows all the days in the month. The user can move the
    current selection using the keyboard and select the date (generating
    @c EVT_CALENDAR event) by pressing @c Return or double clicking it.

    It has advanced possibilities for the customization of its display. All global
    settings (such as colours and fonts used) can, of course, be changed. But
    also, the display style for each day in the month can be set independently
    using wxCalendarDateAttr class.

    An item without custom attributes is drawn with the default colours and
    font and without border, but setting custom attributes with
    wxCalendarCtrl::SetAttr allows to modify its appearance. Just
    create a custom attribute object and set it for the day you want to be
    displayed specially (note that the control will take ownership of the pointer,
    i.e. it will delete it itself). A day may be marked as being a holiday, even
    if it is not recognized as one by wxDateTime using
    wxCalendarDateAttr::SetHoliday method.

    As the attributes are specified for each day, they may change when the month
    is changed, so you will often want to update them in
    @c EVT_CALENDAR_MONTH event handler.

    @beginStyleTable
    @style{wxCAL_SUNDAY_FIRST}:
           Show Sunday as the first day in the week
    @style{wxCAL_MONDAY_FIRST}:
           Show Monday as the first day in the week
    @style{wxCAL_SHOW_HOLIDAYS}:
           Highlight holidays in the calendar
    @style{wxCAL_NO_YEAR_CHANGE}:
           Disable the year changing
    @style{wxCAL_NO_MONTH_CHANGE}:
           Disable the month (and, implicitly, the year) changing
    @style{wxCAL_SHOW_SURROUNDING_WEEKS}:
           Show the neighbouring weeks in the previous and next months
    @style{wxCAL_SEQUENTIAL_MONTH_SELECTION}:
           Use alternative, more compact, style for the month and year
           selection controls.
    @endStyleTable

    @library{wxadv}
    @category{ctrl}
    @appearance{calendarctrl.png}

    @see @ref overview_samplecalendar "Calendar sample", wxCalendarDateAttr,
    wxCalendarEvent
*/
class wxCalendarCtrl : public wxControl
{
public:
    //@{
    /**
        Does the same as Create() method.
    */
    wxCalendarCtrl();
    wxCalendarCtrl(wxWindow* parent, wxWindowID id,
                   const wxDateTime& date = wxDefaultDateTime,
                   const wxPoint& pos = wxDefaultPosition,
                   const wxSize& size = wxDefaultSize,
                   long style = wxCAL_SHOW_HOLIDAYS,
                   const wxString& name = wxCalendarNameStr);
    //@}

    /**
        Destroys the control.
    */
    ~wxCalendarCtrl();

    /**
        Creates the control. See @ref wxWindow::ctor wxWindow for the meaning of
        the parameters and the control overview for the possible styles.
    */
    bool Create(wxWindow* parent, wxWindowID id,
                const wxDateTime& date = wxDefaultDateTime,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxCAL_SHOW_HOLIDAYS,
                const wxString& name = wxCalendarNameStr);

    /**
        This function should be used instead of changing @c wxCAL_SHOW_HOLIDAYS
        style bit directly. It enables or disables the special highlighting of the
        holidays.
    */
    void EnableHolidayDisplay(bool display = true);

    /**
        This function should be used instead of changing
        @c wxCAL_NO_MONTH_CHANGE style bit. It allows or disallows the user to
        change the month interactively. Note that if the month can not be changed, the
        year can not be changed neither.
    */
    void EnableMonthChange(bool enable = true);

    /**
        This function should be used instead of changing @c wxCAL_NO_YEAR_CHANGE
        style bit directly. It allows or disallows the user to change the year
        interactively.
    */
    void EnableYearChange(bool enable = true);

    /**
        Returns the attribute for the given date (should be in the range 1...31).
        The returned pointer may be @NULL.
    */
    wxCalendarDateAttr* GetAttr(size_t day) const;

    /**
        Gets the currently selected date.
    */
    const wxDateTime GetDate() const;

    /**
        Gets the background colour of the header part of the calendar window.
        
        @see SetHeaderColours()
    */
    const wxColour GetHeaderColourBg() const;

    /**
        Gets the foreground colour of the header part of the calendar window.
        
        @see SetHeaderColours()
    */
    const wxColour GetHeaderColourFg() const;

    /**
        Gets the background highlight colour.
        
        @see SetHighlightColours()
    */
    const wxColour GetHighlightColourBg() const;

    /**
        Gets the foreground highlight colour.
        
        @see SetHighlightColours()
    */
    const wxColour GetHighlightColourFg() const;

    /**
        Return the background colour currently used for holiday highlighting.
        
        @see SetHolidayColours()
    */
    const wxColour GetHolidayColourBg() const;

    /**
        Return the foreground colour currently used for holiday highlighting.
        
        @see SetHolidayColours()
    */
    const wxColour GetHolidayColourFg() const;

    /**
        Returns one of @c wxCAL_HITTEST_XXX
        constants() and fills either @a date or
        @a wd pointer with the corresponding value depending on the hit test code.
    */
    wxCalendarHitTestResult HitTest(const wxPoint& pos,
                                    wxDateTime* date = NULL,
                                    wxDateTime::WeekDay* wd = NULL);

    /**
        Clears any attributes associated with the given day (in the range
        1...31).
    */
    void ResetAttr(size_t day);

    /**
        Associates the attribute with the specified date (in the range 1...31).
        If the pointer is @NULL, the items attribute is cleared.
    */
    void SetAttr(size_t day, wxCalendarDateAttr* attr);

    /**
        Sets the current date.
    */
    void SetDate(const wxDateTime& date);

    /**
        Set the colours used for painting the weekdays at the top of the control.
    */
    void SetHeaderColours(const wxColour& colFg,
                          const wxColour& colBg);

    /**
        Set the colours to be used for highlighting the currently selected date.
    */
    void SetHighlightColours(const wxColour& colFg,
                             const wxColour& colBg);

    /**
        Marks the specified day as being a holiday in the current month.
    */
    void SetHoliday(size_t day);

    /**
        Sets the colours to be used for the holidays highlighting (only used if the
        window style includes @c wxCAL_SHOW_HOLIDAYS flag).
    */
    void SetHolidayColours(const wxColour& colFg,
                           const wxColour& colBg);
};

