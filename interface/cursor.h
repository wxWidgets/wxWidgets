/////////////////////////////////////////////////////////////////////////////
// Name:        cursor.h
// Purpose:     documentation for wxCursor class
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxCursor
    @wxheader{cursor.h}

    A cursor is a small bitmap usually used for denoting where the mouse
    pointer is, with a picture that might indicate the interpretation of a
    mouse click. As with icons, cursors in X and MS Windows are created
    in a different manner. Therefore, separate cursors will be created for the
    different environments.  Platform-specific methods for creating a @b wxCursor
    object are catered for, and this is an occasion where
    conditional compilation will probably be required (see wxIcon for
    an example).

    A single cursor object may be used in many windows (any subwindow type).
    The wxWidgets convention is to set the cursor for a window, as in X,
    rather than to set it globally as in MS Windows, although a
    global ::wxSetCursor is also available for MS Windows use.

    @library{wxcore}
    @category{gdi}

    @stdobjects
    Objects:
    wxNullCursor
    Pointers:
    wxSTANDARD_CURSOR

    wxHOURGLASS_CURSOR

    wxCROSS_CURSOR

    @seealso
    wxBitmap, wxIcon, wxWindow::SetCursor, ::wxSetCursor
*/
class wxCursor : public wxBitmap
{
public:
    //@{
    /**
        Copy constructor, uses @ref overview_trefcount "reference counting".
        
        @param bits
            An array of bits.
        @param maskBits
            Bits for a mask bitmap.
        @param width
            Cursor width.
        @param height
            Cursor height.
        @param hotSpotX
            Hotspot x coordinate.
        @param hotSpotY
            Hotspot y coordinate.
        @param type
            Icon type to load. Under Motif, type defaults to wxBITMAP_TYPE_XBM. Under
        Windows,
            it defaults to wxBITMAP_TYPE_CUR_RESOURCE. Under MacOS, it defaults to
        wxBITMAP_TYPE_MACCURSOR_RESOURCE.
            Under X, the permitted cursor types are:
        
        
        
        
        
        
        
            wxBITMAP_TYPE_XBM
        
        
        
        
            Load an X bitmap file.
        
        
        
        
        
            Under Windows, the permitted types are:
        
        
        
        
        
        
        
            wxBITMAP_TYPE_CUR
        
        
        
        
            Load a cursor from a .cur cursor file (only if USE_RESOURCE_LOADING_IN_MSW
            is enabled in setup.h).
        
        
        
        
        
            wxBITMAP_TYPE_CUR_RESOURCE
        
        
        
        
            Load a Windows resource (as specified in the .rc file).
        
        
        
        
        
            wxBITMAP_TYPE_ICO
        
        
        
        
            Load a cursor from a .ico icon file (only if USE_RESOURCE_LOADING_IN_MSW
            is enabled in setup.h). Specify hotSpotX and hotSpotY.
        @param cursorId
            A stock cursor identifier. May be one of:
        
        
        
        
        
        
        
            wxCURSOR_ARROW
        
        
        
        
            A standard arrow cursor.
        
        
        
        
        
            wxCURSOR_RIGHT_ARROW
        
        
        
        
            A standard arrow cursor
            pointing to the right.
        
        
        
        
        
            wxCURSOR_BLANK
        
        
        
        
            Transparent cursor.
        
        
        
        
        
            wxCURSOR_BULLSEYE
        
        
        
        
            Bullseye cursor.
        
        
        
        
        
            wxCURSOR_CHAR
        
        
        
        
            Rectangular character cursor.
        
        
        
        
        
            wxCURSOR_CROSS
        
        
        
        
            A cross cursor.
        
        
        
        
        
            wxCURSOR_HAND
        
        
        
        
            A hand cursor.
        
        
        
        
        
            wxCURSOR_IBEAM
        
        
        
        
            An I-beam cursor (vertical line).
        
        
        
        
        
            wxCURSOR_LEFT_BUTTON
        
        
        
        
            Represents a mouse with the left button depressed.
        
        
        
        
        
            wxCURSOR_MAGNIFIER
        
        
        
        
            A magnifier icon.
        
        
        
        
        
            wxCURSOR_MIDDLE_BUTTON
        
        
        
        
            Represents a mouse with the middle button depressed.
        
        
        
        
        
            wxCURSOR_NO_ENTRY
        
        
        
        
            A no-entry sign cursor.
        
        
        
        
        
            wxCURSOR_PAINT_BRUSH
        
        
        
        
            A paintbrush cursor.
        
        
        
        
        
            wxCURSOR_PENCIL
        
        
        
        
            A pencil cursor.
        
        
        
        
        
            wxCURSOR_POINT_LEFT
        
        
        
        
            A cursor that points left.
        
        
        
        
        
            wxCURSOR_POINT_RIGHT
        
        
        
        
            A cursor that points right.
        
        
        
        
        
            wxCURSOR_QUESTION_ARROW
        
        
        
        
            An arrow and question mark.
        
        
        
        
        
            wxCURSOR_RIGHT_BUTTON
        
        
        
        
            Represents a mouse with the right button depressed.
        
        
        
        
        
            wxCURSOR_SIZENESW
        
        
        
        
            A sizing cursor pointing NE-SW.
        
        
        
        
        
            wxCURSOR_SIZENS
        
        
        
        
            A sizing cursor pointing N-S.
        
        
        
        
        
            wxCURSOR_SIZENWSE
        
        
        
        
            A sizing cursor pointing NW-SE.
        
        
        
        
        
            wxCURSOR_SIZEWE
        
        
        
        
            A sizing cursor pointing W-E.
        
        
        
        
        
            wxCURSOR_SIZING
        
        
        
        
            A general sizing cursor.
        
        
        
        
        
            wxCURSOR_SPRAYCAN
        
        
        
        
            A spraycan cursor.
        
        
        
        
        
            wxCURSOR_WAIT
        
        
        
        
            A wait cursor.
        
        
        
        
        
            wxCURSOR_WATCH
        
        
        
        
            A watch cursor.
        
        
        
        
        
            wxCURSOR_ARROWWAIT
        
        
        
        
            A cursor with both an arrow and
            an hourglass, (windows.)
        
        
        
        
        
            Note that not all cursors are available on all platforms.
        @param cursor
            Pointer or reference to a cursor to copy.
    */
    wxCursor();
    wxCursor(const char bits[], int width, int height,
             int hotSpotX = -1, int hotSpotY = -1,
             const char maskBits[] = NULL,
             wxColour* fg = NULL,
             wxColour* bg = NULL);
    wxCursor(const wxString& cursorName, long type,
             int hotSpotX = 0, int hotSpotY = 0);
    wxCursor(int cursorId);
    wxCursor(const wxImage& image);
    wxCursor(const wxCursor& cursor);
    //@}

    /**
        Destroys the cursor.
        See @ref overview_refcountdestruct "reference-counted object destruction" for
        more info.
        A cursor can be reused for more
        than one window, and does not get destroyed when the window is
        destroyed. wxWidgets destroys all cursors on application exit, although
        it is best to clean them up explicitly.
    */
    ~wxCursor();

    /**
        Returns @true if cursor data is present.
    */
    bool IsOk() const;

    /**
        Assignment operator, using @ref overview_trefcount "reference counting".
    */
    wxCursor operator =(const wxCursor& cursor);
};
