/////////////////////////////////////////////////////////////////////////////
// Name:        dnd.h
// Purpose:     interface of wxTextDropTarget
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxTextDropTarget
    @wxheader{dnd.h}

    A predefined drop target for dealing with text data.

    @library{wxcore}
    @category{dnd}

    @see @ref overview_wxdndoverview, wxDropSource, wxDropTarget, wxFileDropTarget
*/
class wxTextDropTarget : public wxDropTarget
{
public:
    /**
        Constructor.
    */
    wxTextDropTarget();

    /**
        See wxDropTarget::OnDrop. This function is implemented
        appropriately for text, and calls OnDropText().
    */
    virtual bool OnDrop(long x, long y, const void data, size_t size);

    /**
        Override this function to receive dropped text.

        @param x
            The x coordinate of the mouse.
        @param y
            The y coordinate of the mouse.
        @param data
            The data being dropped: a wxString.
    */
    virtual bool OnDropText(wxCoord x, wxCoord y,
                            const wxString& data);
};



/**
    @class wxDropTarget
    @wxheader{dnd.h}

    This class represents a target for a drag and drop operation. A wxDataObject
    can be associated with it and by default, this object will be filled with the
    data from the
    drag source, if the data formats supported by the data object match the drag
    source data
    format.

    There are various virtual handler functions defined in this class which may be
    overridden
    to give visual feedback or react in a more fine-tuned way, e.g. by not
    accepting data on
    the whole window area, but only a small portion of it. The normal sequence of
    calls is
    wxDropTarget::OnEnter, possibly many times wxDropTarget::OnDragOver,
    wxDropTarget::OnDrop and finally wxDropTarget::OnData.

    See @ref overview_wxdndoverview and @ref overview_wxdataobjectoverview
    for more information.

    @library{wxcore}
    @category{dnd}

    @see wxDropSource, wxTextDropTarget, wxFileDropTarget, wxDataFormat,
    wxDataObject
*/
class wxDropTarget
{
public:
    /**
        Constructor. @a data is the data to be associated with the drop target.
    */
    wxDropTarget(wxDataObject* data = NULL);

    /**
        Destructor. Deletes the associated data object, if any.
    */
    ~wxDropTarget();

    /**
        This method may only be called from within OnData().
        By default, this method copies the data from the drop source to the
        wxDataObject associated with this drop target,
        calling its wxDataObject::SetData method.
    */
    virtual void GetData();

    /**
        Called after OnDrop() returns @true. By default this
        will usually GetData() and will return the suggested
        default value @e def.
    */
    virtual wxDragResult OnData(wxCoord x, wxCoord y,
                                wxDragResult def);

    /**
        Called when the mouse is being dragged over the drop target. By default,
        this calls functions return the suggested return value @e def.

        @param x
            The x coordinate of the mouse.
        @param y
            The y coordinate of the mouse.
        @param def
            Suggested value for return value. Determined by SHIFT or CONTROL key states.

        @returns Returns the desired operation or wxDragNone. This is used for
                 optical feedback from the side of the drop source,
                 typically in form of changing the icon.
    */
    virtual wxDragResult OnDragOver(wxCoord x, wxCoord y,
                                    wxDragResult def);

    /**
        Called when the user drops a data object on the target. Return @false to veto
        the operation.

        @param x
            The x coordinate of the mouse.
        @param y
            The y coordinate of the mouse.

        @returns Return @true to accept the data, @false to veto the operation.
    */
    virtual bool OnDrop(wxCoord x, wxCoord y);

    /**
        Called when the mouse enters the drop target. By default, this calls
        OnDragOver().

        @param x
            The x coordinate of the mouse.
        @param y
            The y coordinate of the mouse.
        @param def
            Suggested default for return value. Determined by SHIFT or CONTROL key
        states.

        @returns Returns the desired operation or wxDragNone. This is used for
                 optical feedback from the side of the drop source,
                 typically in form of changing the icon.
    */
    virtual wxDragResult OnEnter(wxCoord x, wxCoord y,
                                 wxDragResult def);

    /**
        Called when the mouse leaves the drop target.
    */
    virtual void OnLeave();

    /**
        Sets the data wxDataObject associated with the
        drop target and deletes any previously associated data object.
    */
    void SetDataObject(wxDataObject* data);
};



/**
    @class wxDropSource
    @wxheader{dnd.h}

    This class represents a source for a drag and drop operation.

    See @ref overview_wxdndoverview and @ref overview_wxdataobjectoverview
    for more information.

    @library{wxcore}
    @category{dnd}

    @see wxDropTarget, wxTextDropTarget, wxFileDropTarget
*/
class wxDropSource
{
public:
    //@{
    /**
        The constructors for wxDataObject.
        If you use the constructor without @a data parameter you must call
        SetData() later.
        Note that the exact type of @a iconCopy and subsequent parameters differs
        between wxMSW and wxGTK: these are cursors under Windows but icons for GTK.
        You should use the macro wxDROP_ICON() in portable
        programs instead of directly using either of these types.

        @param win
            The window which initiates the drag and drop operation.
        @param iconCopy
            The icon or cursor used for feedback for copy operation.
        @param iconMove
            The icon or cursor used for feedback for move operation.
        @param iconNone
            The icon or cursor used for feedback when operation can't be done.
    */
    wxDropSource(wxWindow* win = NULL,
                 const wxIconOrCursor& iconCopy = wxNullIconOrCursor,
                 const wxIconOrCursor& iconMove = wxNullIconOrCursor,
                 const wxIconOrCursor& iconNone = wxNullIconOrCursor);
    wxDropSource(wxDataObject& data, wxWindow* win = NULL,
                 const wxIconOrCursor& iconCopy = wxNullIconOrCursor,
                 const wxIconOrCursor& iconMove = wxNullIconOrCursor,
                 const wxIconOrCursor& iconNone = wxNullIconOrCursor);
    //@}

    /**

    */
    ~wxDropSource();

    /**
        Do it (call this in response to a mouse button press, for example). This starts
        the drag-and-drop operation which will terminate when the user releases the
        mouse.

        @param flags
            If wxDrag_AllowMove is included in the flags, data may
            be moved and not only copied (default). If wxDrag_DefaultMove is
            specified (which includes the previous flag), this is even the default
            operation

        @returns Returns the operation requested by the user, may be wxDragCopy,
                 wxDragMove, wxDragLink, wxDragCancel or wxDragNone if
                 an error occurred.
    */
    virtual wxDragResult DoDragDrop(int flags = wxDrag_CopyOnly);

    /**
        Returns the wxDataObject object that has been assigned previously.
    */
    wxDataObject* GetDataObject();

    /**
        Overridable: you may give some custom UI feedback during the drag and drop
        operation
        in this function. It is called on each mouse move, so your implementation must
        not be too
        slow.

        @param effect
            The effect to implement. One of wxDragCopy, wxDragMove, wxDragLink and
        wxDragNone.
        @param scrolling
            @true if the window is scrolling. MSW only.

        @returns Return @false if you want default feedback, or @true if you
                 implement your own feedback. The return values is
                 ignored under GTK.
    */
    virtual bool GiveFeedback(wxDragResult effect);

    /**
        Set the icon to use for a certain drag result.

        @param res
            The drag result to set the icon for.
        @param cursor
            The ion to show when this drag result occurs.
    */
    void SetCursor(wxDragResult res, const wxCursor& cursor);

    /**
        Sets the data wxDataObject associated with the
        drop source. This will not delete any previously associated data.
    */
    void SetData(wxDataObject& data);
};



/**
    @class wxFileDropTarget
    @wxheader{dnd.h}

    This is a @ref overview_wxdroptarget "drop target" which accepts files (dragged
    from File Manager or Explorer).

    @library{wxcore}
    @category{dnd}

    @see @ref overview_wxdndoverview, wxDropSource, wxDropTarget, wxTextDropTarget
*/
class wxFileDropTarget : public wxDropTarget
{
public:
    /**
        Constructor.
    */
    wxFileDropTarget();

    /**
        See wxDropTarget::OnDrop. This function is implemented
        appropriately for files, and calls OnDropFiles().
    */
    virtual bool OnDrop(long x, long y, const void data, size_t size);

    /**
        Override this function to receive dropped files.

        @param x
            The x coordinate of the mouse.
        @param y
            The y coordinate of the mouse.
        @param filenames
            An array of filenames.
    */
    virtual bool OnDropFiles(wxCoord x, wxCoord y,
                             const wxArrayString& filenames);
};



// ============================================================================
// Global functions/macros
// ============================================================================

/** @ingroup group_funcmacro_gdi */
//@{

/**
    This macro creates either a cursor (MSW) or an icon (elsewhere) with the
    given @a name (of type <tt>const char*</tt>). Under MSW, the cursor is
    loaded from the resource file and the icon is loaded from XPM file under
    other platforms.

    This macro should be used with wxDropSource::wxDropSource().

    @returns wxCursor on MSW, otherwise returns a wxIcon

    @header{wx/dnd.h}
*/
#define wxDROP_ICON(name)

//@}

