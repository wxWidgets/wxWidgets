/////////////////////////////////////////////////////////////////////////////
// Name:        wx/busycursor.h
// Purpose:     Busy cursor-related stuff
// Author:      wxWidgets team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

/** @addtogroup group_funcmacro_dialog */
///@{

/**
    Changes the cursor to the given cursor for all windows in the application.
    Use wxEndBusyCursor() to revert the cursor back to its previous state.
    These two calls can be nested, and a counter ensures that only the outer
    calls take effect.

    Prefer using wxBusyCursor to using this function directly.

    @see wxIsBusy(), wxBusyCursor

    @header{wx/busycursor.h}
    @library{wxcore}
*/
void wxBeginBusyCursor(const wxCursor* cursor = wxHOURGLASS_CURSOR);

/**
    Overload of wxBeginBusyCursor() taking a cursor bundle.

    This overload selects the cursor of the appropriate size from @a cursors.

    As with wxSetCursor(), the busy cursor is currently not updated
    automatically if the DPI or preferred cursor size changes because this is
    considered to be unlikely to happen during the brief time while the busy
    cursor is shown.

    @since 3.3.0
 */
void wxBeginBusyCursor(const wxCursorBundle& cursors);

/**
    Changes the cursor back to the original cursor, for all windows in the
    application. Use with wxBeginBusyCursor().

    @see wxIsBusy(), wxBusyCursor

    @header{wx/busycursor.h}
    @library{wxcore}
*/
void wxEndBusyCursor();

/**
    Returns @true if between two wxBeginBusyCursor() and wxEndBusyCursor()
    calls.

    @see wxBusyCursor.

    @header{wx/busycursor.h}
    @library{wxcore}
*/
bool wxIsBusy();

///@}


/**
    @class wxBusyCursor

    This class makes it easy to tell your user that the program is temporarily
    busy. Just create a wxBusyCursor object on the stack, and within the
    current scope, the hourglass will be shown.

    For example:

    @code
    wxBusyCursor wait;

    for (int i = 0; i < 100000; i++)
        DoACalculation();
    @endcode

    It works by calling wxBeginBusyCursor() in the constructor, and
    wxEndBusyCursor() in the destructor.

    @library{wxcore}
    @category{misc}

    @see wxBeginBusyCursor(), wxEndBusyCursor(), wxWindowDisabler, wxBusyInfo
*/
class wxBusyCursor
{
public:
    /**
        Constructs a busy cursor object, calling wxBeginBusyCursor().
    */
    explicit wxBusyCursor(const wxCursor* cursor = wxHOURGLASS_CURSOR);

    /**
        Constructs a busy cursor object, calling wxBeginBusyCursor() with the
        given cursor bundle.

        @since 3.3.0
    */
    explicit wxBusyCursor(const wxCursorBundle& cursors);

    /**
        Destroys the busy cursor object, calling wxEndBusyCursor().
    */
    ~wxBusyCursor();
};
