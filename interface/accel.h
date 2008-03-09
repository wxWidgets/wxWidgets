/////////////////////////////////////////////////////////////////////////////
// Name:        accel.h
// Purpose:     documentation for wxAcceleratorEntry class
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxAcceleratorEntry
    @wxheader{accel.h}

    An object used by an application wishing to create an @ref
    overview_wxacceleratortable "accelerator table".

    @library{wxcore}
    @category{FIXME}

    @seealso
    wxAcceleratorTable, wxWindow::SetAcceleratorTable
*/
class wxAcceleratorEntry
{
public:
    //@{
    /**
        Constructor.
        
        @param flags
            One of wxACCEL_ALT, wxACCEL_SHIFT, wxACCEL_CTRL and wxACCEL_NORMAL.
        Indicates
            which modifier key is held down.
        @param keyCode
            The keycode to be detected. See Keycodes for a full list of keycodes.
        @param cmd
            The menu or control command identifier.
    */
    wxAcceleratorEntry();
    wxAcceleratorEntry(int flags, int keyCode, int cmd);
    //@}

    /**
        Returns the command identifier for the accelerator table entry.
    */
    int GetCommand() const;

    /**
        Returns the flags for the accelerator table entry.
    */
    int GetFlags() const;

    /**
        Returns the keycode for the accelerator table entry.
    */
    int GetKeyCode() const;

    /**
        Sets the accelerator entry parameters.
        
        @param flags
            One of wxACCEL_ALT, wxACCEL_SHIFT, wxACCEL_CTRL and wxACCEL_NORMAL.
        Indicates
            which modifier key is held down.
        @param keyCode
            The keycode to be detected. See Keycodes for a full list of keycodes.
        @param cmd
            The menu or control command identifier.
    */
    void Set(int flags, int keyCode, int cmd);
};


/**
    @class wxAcceleratorTable
    @wxheader{accel.h}

    An accelerator table allows the application to specify a table of keyboard
    shortcuts for
    menus or other commands. On Windows and Mac OS X, menu or button commands are
    supported; on GTK,
    only menu commands are supported.

    The object @b wxNullAcceleratorTable is defined to be a table with no data, and
    is the
    initial accelerator table for a window.

    @library{wxcore}
    @category{misc}

    @stdobjects
    Objects:
    wxNullAcceleratorTable

    @seealso
    wxAcceleratorEntry, wxWindow::SetAcceleratorTable
*/
class wxAcceleratorTable : public wxObject
{
public:
    //@{
    /**
        Loads the accelerator table from a Windows resource (Windows only).
        
        @param n
            Number of accelerator entries.
        @param entries
            The array of entries.
        @param resource
            Name of a Windows accelerator.
    */
    wxAcceleratorTable();
    wxAcceleratorTable(const wxAcceleratorTable& bitmap);
    wxAcceleratorTable(int n, wxAcceleratorEntry entries[]);
    wxAcceleratorTable(const wxString& resource);
    //@}

    /**
        Destroys the wxAcceleratorTable object.
        See @ref overview_refcountdestruct "reference-counted object destruction" for
        more info.
    */
    ~wxAcceleratorTable();

    /**
        Returns @true if the accelerator table is valid.
    */
    bool IsOk() const;

    /**
        Assignment operator, using @ref overview_trefcount "reference counting".
        
        @param accel
            Accelerator table to assign.
    */
    wxAcceleratorTable operator =(const wxAcceleratorTable& accel);
};
