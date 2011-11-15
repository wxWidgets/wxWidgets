/////////////////////////////////////////////////////////////////////////////
// Name:        uiaction.h
// Purpose:     interface of wxUIActionSimulator
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxUIActionSimulator

    wxUIActionSimulator is a class used to simulate user interface actions
    such as a mouse click or a key press.

    Common usage for this class would be to provide playback and record (aka
    macro recording) functionality for users, or to drive unit tests by
    simulating user sessions.

    See the @ref page_samples_uiaction for an example of using this class.

    @since 2.9.2

    @library{wxcore}
*/

class wxUIActionSimulator
{
public:
    /**
        Default constructor.
    */
    wxUIActionSimulator();

    /**
        Move the mouse to the specified coordinates.

        @param x
            x coordinate to move to, in screen coordinates.

        @param y
            y coordinate to move to, in screen coordinates.
    */
    bool MouseMove(long x, long y);

    /**
        Move the mouse to the specified coordinates.

        @param point
            Point to move to, in screen coordinates.
    */
    bool MouseMove(const wxPoint& point);

    /**
        Press a mouse button.

        @param button
            Button to press. Valid constants are @c wxMOUSE_BTN_LEFT,
            @c wxMOUSE_BTN_MIDDLE, and @c wxMOUSE_BTN_RIGHT.
    */
    bool MouseDown(int button = wxMOUSE_BTN_LEFT);

    /**
        Release a mouse button.

        @param button
            Button to press. See wxUIActionSimulator::MouseDown for a list of
            valid constants.
    */
    bool MouseUp(int button = wxMOUSE_BTN_LEFT);
    /**
        Click a mouse button.

        @param button
            Button to press. See wxUIActionSimulator::MouseDown for a list of
            valid constants.
    */
    bool MouseClick(int button = wxMOUSE_BTN_LEFT);
    /**
        Double-click a mouse button.

        @param button
            Button to press. See wxUIActionSimulator::MouseDown for a list of
            valid constants.
    */
    bool MouseDblClick(int button = wxMOUSE_BTN_LEFT);

    /**
        Perform a drag and drop operation.

        @param x1
            x start coordinate, in screen coordinates.

        @param y1
            y start coordinate, in screen coordinates.

        @param x2
            x destination coordinate, in screen coordinates.

        @param y2
            y destination coordinate, in screen coordinates.

        @param button
            Button to press. See wxUIActionSimulator::MouseDown for a list of
            valid constants.
    */
    bool MouseDragDrop(long x1, long y1, long x2, long y2,
                       int button = wxMOUSE_BTN_LEFT);

    /**
        Press a key.

        If you are using modifiers then it needs to be paired with an identical
        KeyUp or the modifiers will not be released (MSW and OSX).

        @param keycode
            Key to operate on, as an integer. It is interpreted as a wxKeyCode.

        @param modifiers
            A combination of ::wxKeyModifier flags to be pressed with the given
            keycode.
    */
    bool KeyDown(int keycode, int modifiers = wxMOD_NONE);

    /**
        Release a key.

        @param keycode
            Key to operate on, as an integer. It is interpreted as a wxKeyCode.

        @param modifiers
            A combination of ::wxKeyModifier flags to be pressed with the given
            keycode.
    */
    bool KeyUp(int keycode, int modifiers = wxMOD_NONE);

    /**
        Press and release a key.

        @param keycode
            Key to operate on, as an integer. It is interpreted as a wxKeyCode.

        @param modifiers
            A combination of ::wxKeyModifier flags to be pressed with the given
            keycode.
    */
    bool Char(int keycode, int modifiers = wxMOD_NONE);

    /**
        Emulate typing in the keys representing the given string.

        Currently only the ASCII letters, digits and characters for the definition
        of numbers (i.e. characters @c a-z @c A-Z @c 0-9 @c + @c - @c . @c , @c 'space')
        are supported.

        @param text
            The string to type.
    */
    bool Text(const wxString& text);
};

