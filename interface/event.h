/////////////////////////////////////////////////////////////////////////////
// Name:        event.h
// Purpose:     documentation for wxKeyEvent class
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxKeyEvent
    @wxheader{event.h}

    This event class contains information about keypress (character) events.

    Notice that there are three different kinds of keyboard events in wxWidgets:
    key down and up events and char events. The difference between the first two
    is clear - the first corresponds to a key press and the second to a key
    release - otherwise they are identical. Just note that if the key is
    maintained in a pressed state you will typically get a lot of (automatically
    generated) down events but only one up so it is wrong to assume that there is
    one up event corresponding to each down one.

    Both key events provide untranslated key codes while the char event carries
    the translated one. The untranslated code for alphanumeric keys is always
    an upper case value. For the other keys it is one of @c WXK_XXX values
    from the @ref overview_keycodes "keycodes table". The translated key is, in
    general, the character the user expects to appear as the result of the key
    combination when typing the text into a text entry zone, for example.

    A few examples to clarify this (all assume that CAPS LOCK is unpressed
    and the standard US keyboard): when the @c 'A' key is pressed, the key down
    event key code is equal to @c ASCII A == 65. But the char event key code
    is @c ASCII a == 97. On the other hand, if you press both SHIFT and
    @c 'A' keys simultaneously , the key code in key down event will still be
    just @c 'A' while the char event key code parameter will now be @c 'A'
    as well.

    Although in this simple case it is clear that the correct key code could be
    found in the key down event handler by checking the value returned by
    wxKeyEvent::ShiftDown, in general you should use
    @c EVT_CHAR for this as for non-alphanumeric keys the translation is
    keyboard-layout dependent and can only be done properly by the system itself.

    Another kind of translation is done when the control key is pressed: for
    example, for CTRL-A key press the key down event still carries the
    same key code @c 'a' as usual but the char event will have key code of
    1, the ASCII value of this key combination.

    You may discover how the other keys on your system behave interactively by
    running the text wxWidgets sample and pressing some keys
    in any of the text controls shown in it.

    @b Note: If a key down (@c EVT_KEY_DOWN) event is caught and
    the event handler does not call @c event.Skip() then the corresponding
    char event (@c EVT_CHAR) will not happen.  This is by design and
    enables the programs that handle both types of events to be a bit
    simpler.

    @b Note for Windows programmers: The key and char events in wxWidgets are
    similar to but slightly different from Windows @c WM_KEYDOWN and
    @c WM_CHAR events. In particular, Alt-x combination will generate a char
    event in wxWidgets (unless it is used as an accelerator).

    @b Tip: be sure to call @c event.Skip() for events that you don't process in
    key event function, otherwise menu shortcuts may cease to work under Windows.

    @library{wxcore}
    @category{events}
*/
class wxKeyEvent : public wxEvent
{
public:
    /**
        Constructor. Currently, the only valid event types are wxEVT_CHAR and
        wxEVT_CHAR_HOOK.
    */
    wxKeyEvent(WXTYPE keyEventType);

    /**
        Returns @true if the Alt key was down at the time of the key event.
        Notice that GetModifiers() is easier to use
        correctly than this function so you should consider using it in new code.
    */
    bool AltDown();

    /**
        CMD is a pseudo key which is the same as Control for PC and Unix
        platforms but the special APPLE (a.k.a as COMMAND) key under
        Macs: it makes often sense to use it instead of, say, ControlDown() because Cmd
        key is used for the same thing under Mac as Ctrl elsewhere (but Ctrl still
        exists, just not used for this purpose under Mac). So for non-Mac platforms
        this is the same as ControlDown() and under
        Mac this is the same as MetaDown().
    */
    bool CmdDown();

    /**
        Returns @true if the control key was down at the time of the key event.
        Notice that GetModifiers() is easier to use
        correctly than this function so you should consider using it in new code.
    */
    bool ControlDown();

    /**
        Returns the virtual key code. ASCII events return normal ASCII values,
        while non-ASCII events return values such as @b WXK_LEFT for the
        left cursor key. See Keycodes for a full list of
        the virtual key codes.
        Note that in Unicode build, the returned value is meaningful only if the
        user entered a character that can be represented in current locale's default
        charset. You can obtain the corresponding Unicode character using
        GetUnicodeKey().
    */
    int GetKeyCode();

    /**
        Return the bitmask of modifier keys which were pressed when this event
        happened. See @ref overview_keymodifiers "key modifier constants" for the full
        list
        of modifiers.
        Notice that this function is easier to use correctly than, for example,
        ControlDown() because when using the latter you
        also have to remember to test that none of the other modifiers is pressed:
        
        and forgetting to do it can result in serious program bugs (e.g. program not
        working with European keyboard layout where ALTGR key which is seen by
        the program as combination of CTRL and ALT is used). On the
        other hand, you can simply write
        
        with this function.
    */
    int GetModifiers();

    //@{
    /**
        Obtains the position (in client coordinates) at which the key was pressed.
    */
    wxPoint GetPosition();
    void GetPosition(long* x, long* y);
    //@}

    /**
        Returns the raw key code for this event. This is a platform-dependent scan code
        which should only be used in advanced applications.
        @b NB: Currently the raw key codes are not supported by all ports, use
        @c #ifdef wxHAS_RAW_KEY_CODES to determine if this feature is available.
    */
    wxUint32 GetRawKeyCode();

    /**
        Returns the low level key flags for this event. The flags are
        platform-dependent and should only be used in advanced applications.
        @b NB: Currently the raw key flags are not supported by all ports, use
        @c #ifdef wxHAS_RAW_KEY_CODES to determine if this feature is available.
    */
    wxUint32 GetRawKeyFlags();

    /**
        Returns the Unicode character corresponding to this key event.
        This function is only available in Unicode build, i.e. when
        @c wxUSE_UNICODE is 1.
    */
    wxChar GetUnicodeKey();

    /**
        Returns the X position (in client coordinates) of the event.
    */
    long GetX();

    /**
        Returns the Y (in client coordinates) position of the event.
    */
    long GetY();

    /**
        Returns @true if either CTRL or ALT keys was down
        at the time of the key event. Note that this function does not take into
        account neither SHIFT nor META key states (the reason for ignoring
        the latter is that it is common for NUMLOCK key to be configured as
        META under X but the key presses even while NUMLOCK is on should
        be still processed normally).
    */
    bool HasModifiers();

    /**
        Returns @true if the Meta key was down at the time of the key event.
        Notice that GetModifiers() is easier to use
        correctly than this function so you should consider using it in new code.
    */
    bool MetaDown();

    /**
        Returns @true if the shift key was down at the time of the key event.
        Notice that GetModifiers() is easier to use
        correctly than this function so you should consider using it in new code.
    */
    bool ShiftDown();

    /**
        bool m_altDown
        @b Deprecated: Please use GetModifiers()
        instead!
        @true if the Alt key is pressed down.
    */


    /**
        bool m_controlDown
        @b Deprecated: Please use GetModifiers()
        instead!
        @true if control is pressed down.
    */


    /**
        long m_keyCode
        @b Deprecated: Please use GetKeyCode()
        instead!
        Virtual keycode. See Keycodes for a list of identifiers.
    */


    /**
        bool m_metaDown
        @b Deprecated: Please use GetModifiers()
        instead!
        @true if the Meta key is pressed down.
    */


    /**
        bool m_shiftDown
        @b Deprecated: Please use GetModifiers()
        instead!
        @true if shift is pressed down.
    */


    /**
        int m_x
        @b Deprecated: Please use GetX() instead!
        X position of the event.
    */


    /**
        int m_y
        @b Deprecated: Please use GetY() instead!
        Y position of the event.
    */
};


/**
    @class wxJoystickEvent
    @wxheader{event.h}

    This event class contains information about mouse events, particularly
    events received by windows.

    @library{wxcore}
    @category{events}

    @seealso
    wxJoystick
*/
class wxJoystickEvent : public wxEvent
{
public:
    /**
        Constructor.
    */
    wxJoystickEvent(WXTYPE eventType = 0, int state = 0,
                    int joystick = wxJOYSTICK1,
                    int change = 0);

    /**
        Returns @true if the event was a down event from the specified button (or any
        button).
        
        @param button
            Can be wxJOY_BUTTONn where n is 1, 2, 3 or 4; or wxJOY_BUTTON_ANY to
            indicate any button down event.
    */
    bool ButtonDown(int button = wxJOY_BUTTON_ANY);

    /**
        Returns @true if the specified button (or any button) was in a down state.
        
        @param button
            Can be wxJOY_BUTTONn where n is 1, 2, 3 or 4; or wxJOY_BUTTON_ANY to
            indicate any button down event.
    */
    bool ButtonIsDown(int button = wxJOY_BUTTON_ANY);

    /**
        Returns @true if the event was an up event from the specified button (or any
        button).
        
        @param button
            Can be wxJOY_BUTTONn where n is 1, 2, 3 or 4; or wxJOY_BUTTON_ANY to
            indicate any button down event.
    */
    bool ButtonUp(int button = wxJOY_BUTTON_ANY);

    /**
        Returns the identifier of the button changing state. This is a wxJOY_BUTTONn
        identifier, where
        n is one of 1, 2, 3, 4.
    */
    int GetButtonChange();

    /**
        Returns the down state of the buttons. This is a bitlist of wxJOY_BUTTONn
        identifiers, where
        n is one of 1, 2, 3, 4.
    */
    int GetButtonState();

    /**
        Returns the identifier of the joystick generating the event - one of
        wxJOYSTICK1 and wxJOYSTICK2.
    */
    int GetJoystick();

    /**
        Returns the x, y position of the joystick event.
    */
    wxPoint GetPosition();

    /**
        Returns the z position of the joystick event.
    */
    int GetZPosition();

    /**
        Returns @true if this was a button up or down event (@e not 'is any button
        down?').
    */
    bool IsButton();

    /**
        Returns @true if this was an x, y move event.
    */
    bool IsMove();

    /**
        Returns @true if this was a z move event.
    */
    bool IsZMove();
};


/**
    @class wxScrollWinEvent
    @wxheader{event.h}

    A scroll event holds information about events sent from scrolling windows.

    @library{wxcore}
    @category{events}

    @seealso
    wxScrollEvent, @ref overview_eventhandlingoverview
*/
class wxScrollWinEvent : public wxEvent
{
public:
    /**
        Constructor.
    */
    wxScrollWinEvent(WXTYPE commandType = 0, int pos = 0,
                     int orientation = 0);

    /**
        Returns wxHORIZONTAL or wxVERTICAL, depending on the orientation of the
        scrollbar.
    */
    int GetOrientation();

    /**
        Returns the position of the scrollbar for the thumb track and release events.
        Note that this field can't be used for the other events, you need to query
        the window itself for the current position in that case.
    */
    int GetPosition();
};


/**
    @class wxSysColourChangedEvent
    @wxheader{event.h}

    This class is used for system colour change events, which are generated
    when the user changes the colour settings using the control panel.
    This is only appropriate under Windows.

    @library{wxcore}
    @category{events}

    @seealso
    @ref overview_eventhandlingoverview
*/
class wxSysColourChangedEvent : public wxEvent
{
public:
    /**
        Constructor.
    */
    wxSysColourChangedEvent();
};


/**
    @class wxWindowCreateEvent
    @wxheader{event.h}

    This event is sent just after the actual window associated with a wxWindow
    object
    has been created. Since it is derived from wxCommandEvent, the event propagates
    up
    the window hierarchy.

    @library{wxcore}
    @category{events}

    @seealso
    @ref overview_eventhandlingoverview, wxWindowDestroyEvent
*/
class wxWindowCreateEvent : public wxCommandEvent
{
public:
    /**
        Constructor.
    */
    wxWindowCreateEvent(wxWindow* win = NULL);
};


/**
    @class wxPaintEvent
    @wxheader{event.h}

    A paint event is sent when a window's contents needs to be repainted.

    Please notice that in general it is impossible to change the drawing of a
    standard control (such as wxButton) and so you shouldn't
    attempt to handle paint events for them as even if it might work on some
    platforms, this is inherently not portable and won't work everywhere.

    @library{wxcore}
    @category{events}

    @seealso
    @ref overview_eventhandlingoverview
*/
class wxPaintEvent : public wxEvent
{
public:
    /**
        Constructor.
    */
    wxPaintEvent(int id = 0);
};


/**
    @class wxMaximizeEvent
    @wxheader{event.h}

    An event being sent when a top level window is maximized. Notice that it is
    not sent when the window is restored to its original size after it had been
    maximized, only a normal wxSizeEvent is generated in
    this case.

    @library{wxcore}
    @category{events}

    @seealso
    @ref overview_eventhandlingoverview, wxTopLevelWindow::Maximize,
    wxTopLevelWindow::IsMaximized
*/
class wxMaximizeEvent : public wxEvent
{
public:
    /**
        Constructor. Only used by wxWidgets internally.
    */
    wxMaximizeEvent(int id = 0);
};


/**
    @class wxUpdateUIEvent
    @wxheader{event.h}

    This class is used for pseudo-events which are called by wxWidgets
    to give an application the chance to update various user interface elements.

    @library{wxcore}
    @category{events}

    @seealso
    @ref overview_eventhandlingoverview
*/
class wxUpdateUIEvent : public wxCommandEvent
{
public:
    /**
        Constructor.
    */
    wxUpdateUIEvent(wxWindowID commandId = 0);

    /**
        Returns @true if it is appropriate to update (send UI update events to)
        this window.
        This function looks at the mode used (see wxUpdateUIEvent::SetMode),
        the wxWS_EX_PROCESS_UI_UPDATES flag in @e window,
        the time update events were last sent in idle time, and
        the update interval, to determine whether events should be sent to
        this window now. By default this will always return @true because
        the update mode is initially wxUPDATE_UI_PROCESS_ALL and
        the interval is set to 0; so update events will be sent as
        often as possible. You can reduce the frequency that events
        are sent by changing the mode and/or setting an update interval.
        
        @see ResetUpdateTime(), SetUpdateInterval(),
             SetMode()
    */
    static bool CanUpdate(wxWindow* window);

    /**
        Check or uncheck the UI element.
    */
    void Check(bool check);

    /**
        Enable or disable the UI element.
    */
    void Enable(bool enable);

    /**
        Returns @true if the UI element should be checked.
    */
    bool GetChecked();

    /**
        Returns @true if the UI element should be enabled.
    */
    bool GetEnabled();

    /**
        Static function returning a value specifying how wxWidgets
        will send update events: to all windows, or only to those which specify that
        they
        will process the events.
        See SetMode().
    */
    static wxUpdateUIMode GetMode();

    /**
        Returns @true if the application has called Check(). For wxWidgets internal use
        only.
    */
    bool GetSetChecked();

    /**
        Returns @true if the application has called Enable(). For wxWidgets internal use
        only.
    */
    bool GetSetEnabled();

    /**
        Returns @true if the application has called Show(). For wxWidgets internal use
        only.
    */
    bool GetSetShown();

    /**
        Returns @true if the application has called SetText(). For wxWidgets internal
        use only.
    */
    bool GetSetText();

    /**
        Returns @true if the UI element should be shown.
    */
    bool GetShown();

    /**
        Returns the text that should be set for the UI element.
    */
    wxString GetText();

    /**
        Returns the current interval between updates in milliseconds.
        -1 disables updates, 0 updates as frequently as possible.
        See SetUpdateInterval().
    */
    static long GetUpdateInterval();

    /**
        Used internally to reset the last-updated time to the
        current time. It is assumed that update events are
        normally sent in idle time, so this is called at the end of
        idle processing.
        
        @see CanUpdate(), SetUpdateInterval(),
             SetMode()
    */
    static void ResetUpdateTime();

    /**
        Specify how wxWidgets will send update events: to
        all windows, or only to those which specify that they
        will process the events.
        @a mode may be one of the following values.
        The default is wxUPDATE_UI_PROCESS_ALL.
    */
    static void SetMode(wxUpdateUIMode mode);

    /**
        Sets the text for this UI element.
    */
    void SetText(const wxString& text);

    /**
        Sets the interval between updates in milliseconds.
        Set to -1 to disable updates, or to 0 to update as frequently as possible.
        The default is 0.
        Use this to reduce the overhead of UI update events if your application
        has a lot of windows. If you set the value to -1 or greater than 0,
        you may also need to call wxWindow::UpdateWindowUI
        at appropriate points in your application, such as when a dialog
        is about to be shown.
    */
    static void SetUpdateInterval(long updateInterval);

    /**
        Show or hide the UI element.
    */
    void Show(bool show);
};


/**
    @class wxClipboardTextEvent
    @wxheader{event.h}

    This class represents the events generated by a control (typically a
    wxTextCtrl but other windows can generate these events as
    well) when its content gets copied or cut to, or pasted from the clipboard.
    There are three types of corresponding events wxEVT_COMMAND_TEXT_COPY,
    wxEVT_COMMAND_TEXT_CUT and wxEVT_COMMAND_TEXT_PASTE.

    If any of these events is processed (without being skipped) by an event
    handler, the corresponding operation doesn't take place which allows to prevent
    the text from being copied from or pasted to a control. It is also possible to
    examine the clipboard contents in the PASTE event handler and transform it in
    some way before inserting in a control -- for example, changing its case or
    removing invalid characters.

    Finally notice that a CUT event is always preceded by the COPY event which
    makes it possible to only process the latter if it doesn't matter if the text
    was copied or cut.

    @library{wxcore}
    @category{events}

    @seealso
    wxClipboard
*/
class wxClipboardTextEvent : public wxCommandEvent
{
public:
    /**
        
    */
    wxClipboardTextEvent(wxEventType commandType = wxEVT_NULL,
                         int id = 0);
};


/**
    @class wxMouseEvent
    @wxheader{event.h}

    This event class contains information about the events generated by the mouse:
    they include mouse buttons press and release events and mouse move events.

    All mouse events involving the buttons use @c wxMOUSE_BTN_LEFT for the
    left mouse button, @c wxMOUSE_BTN_MIDDLE for the middle one and
    @c wxMOUSE_BTN_RIGHT for the right one. And if the system supports more
    buttons, the @c wxMOUSE_BTN_AUX1 and @c wxMOUSE_BTN_AUX2 events
    can also be generated. Note that not all mice have even a middle button so a
    portable application should avoid relying on the events from it (but the right
    button click can be emulated using the left mouse button with the control key
    under Mac platforms with a single button mouse).

    For the @c wxEVT_ENTER_WINDOW and @c wxEVT_LEAVE_WINDOW events
    purposes, the mouse is considered to be inside the window if it is in the
    window client area and not inside one of its children. In other words, the
    parent window receives @c wxEVT_LEAVE_WINDOW event not only when the
    mouse leaves the window entirely but also when it enters one of its children.

    @b NB: Note that under Windows CE mouse enter and leave events are not natively
    supported
    by the system but are generated by wxWidgets itself. This has several
    drawbacks: the LEAVE_WINDOW event might be received some time after the mouse
    left the window and the state variables for it may have changed during this
    time.

    @b NB: Note the difference between methods like
    wxMouseEvent::LeftDown and
    wxMouseEvent::LeftIsDown: the former returns @true
    when the event corresponds to the left mouse button click while the latter
    returns @true if the left mouse button is currently being pressed. For
    example, when the user is dragging the mouse you can use
    wxMouseEvent::LeftIsDown to test
    whether the left mouse button is (still) depressed. Also, by convention, if
    wxMouseEvent::LeftDown returns @true,
    wxMouseEvent::LeftIsDown will also return @true in
    wxWidgets whatever the underlying GUI behaviour is (which is
    platform-dependent). The same applies, of course, to other mouse buttons as
    well.

    @library{wxcore}
    @category{events}

    @seealso
    wxKeyEvent::CmdDown
*/
class wxMouseEvent : public wxEvent
{
public:
    /**
        Constructor. Valid event types are:
        
         @b wxEVT_ENTER_WINDOW
         @b wxEVT_LEAVE_WINDOW
         @b wxEVT_LEFT_DOWN
         @b wxEVT_LEFT_UP
         @b wxEVT_LEFT_DCLICK
         @b wxEVT_MIDDLE_DOWN
         @b wxEVT_MIDDLE_UP
         @b wxEVT_MIDDLE_DCLICK
         @b wxEVT_RIGHT_DOWN
         @b wxEVT_RIGHT_UP
         @b wxEVT_RIGHT_DCLICK
         @b wxEVT_MOUSE_AUX1_DOWN
         @b wxEVT_MOUSE_AUX1_UP
         @b wxEVT_MOUSE_AUX1_DCLICK
         @b wxEVT_MOUSE_AUX2_DOWN
         @b wxEVT_MOUSE_AUX2_UP
         @b wxEVT_MOUSE_AUX2_DCLICK
         @b wxEVT_MOTION
         @b wxEVT_MOUSEWHEEL
    */
    wxMouseEvent(WXTYPE mouseEventType = 0);

    /**
        Returns @true if the Alt key was down at the time of the event.
    */
    bool AltDown();

    /**
        Returns @true if the event was a first extra button double click.
    */
    bool Aux1DClick();

    /**
        Returns @true if the first extra button mouse button changed to down.
    */
    bool Aux1Down();

    /**
        Returns @true if the first extra button mouse button is currently down,
        independent
        of the current event type.
    */
    bool Aux1IsDown();

    /**
        Returns @true if the first extra button mouse button changed to up.
    */
    bool Aux1Up();

    /**
        Returns @true if the event was a second extra button double click.
    */
    bool Aux2DClick();

    /**
        Returns @true if the second extra button mouse button changed to down.
    */
    bool Aux2Down();

    /**
        Returns @true if the second extra button mouse button is currently down,
        independent
        of the current event type.
    */
    bool Aux2IsDown();

    /**
        Returns @true if the second extra button mouse button changed to up.
    */
    bool Aux2Up();

    /**
        Returns @true if the identified mouse button is changing state. Valid
        values of @a button are:
        
        @c wxMOUSE_BTN_LEFT
        
        check if left button was pressed
        
        @c wxMOUSE_BTN_MIDDLE
        
        check if middle button was pressed
        
        @c wxMOUSE_BTN_RIGHT
        
        check if right button was pressed
        
        @c wxMOUSE_BTN_AUX1
        
        check if the first extra button was pressed
        
        @c wxMOUSE_BTN_AUX2
        
        check if the second extra button was pressed
        
        @c wxMOUSE_BTN_ANY
        
        check if any button was pressed
    */
    bool Button(int button);

    /**
        If the argument is omitted, this returns @true if the event was a mouse
        double click event. Otherwise the argument specifies which double click event
        was generated (see Button() for the possible
        values).
    */
    bool ButtonDClick(int but = wxMOUSE_BTN_ANY);

    /**
        If the argument is omitted, this returns @true if the event was a mouse
        button down event. Otherwise the argument specifies which button-down event
        was generated (see Button() for the possible
        values).
    */
    bool ButtonDown(int but = -1);

    /**
        If the argument is omitted, this returns @true if the event was a mouse
        button up event. Otherwise the argument specifies which button-up event
        was generated (see Button() for the possible
        values).
    */
    bool ButtonUp(int but = -1);

    /**
        Same as MetaDown() under Mac, same as
        ControlDown() elsewhere.
        
        @see wxKeyEvent::CmdDown
    */
    bool CmdDown();

    /**
        Returns @true if the control key was down at the time of the event.
    */
    bool ControlDown();

    /**
        Returns @true if this was a dragging event (motion while a button is depressed).
        
        @see Moving()
    */
    bool Dragging();

    /**
        Returns @true if the mouse was entering the window.
        See also Leaving().
    */
    bool Entering();

    /**
        Returns the mouse button which generated this event or @c wxMOUSE_BTN_NONE
        if no button is involved (for mouse move, enter or leave event, for example).
        Otherwise @c wxMOUSE_BTN_LEFT is returned for the left button down, up and
        double click events, @c wxMOUSE_BTN_MIDDLE and @c wxMOUSE_BTN_RIGHT
        for the same events for the middle and the right buttons respectively.
    */
    int GetButton();

    /**
        Returns the number of mouse clicks for this event: 1 for a simple click, 2
        for a double-click, 3 for a triple-click and so on.
        Currently this function is implemented only in wxMac and returns -1 for the
        other platforms (you can still distinguish simple clicks from double-clicks as
        they generate different kinds of events however).
        This function is new since wxWidgets version 2.9.0
    */
    int GetClickCount();

    /**
        Returns the configured number of lines (or whatever) to be scrolled per
        wheel action.  Defaults to three.
    */
    int GetLinesPerAction();

    /**
        Returns the logical mouse position in pixels (i.e. translated according to the
        translation set for the DC, which usually indicates that the window has been
        scrolled).
    */
    wxPoint GetLogicalPosition(const wxDC& dc);

    //@{
    /**
        Sets *x and *y to the position at which the event occurred.
        Returns the physical mouse position in pixels.
        Note that if the mouse event has been artificially generated from a special
        keyboard combination (e.g. under Windows when the "menu'' key is pressed), the
        returned position is @c wxDefaultPosition.
    */
    wxPoint GetPosition();
    void GetPosition(wxCoord* x, wxCoord* y);
    void GetPosition(long* x, long* y);
    //@}

    /**
        Get wheel delta, normally 120.  This is the threshold for action to be
        taken, and one such action (for example, scrolling one increment)
        should occur for each delta.
    */
    int GetWheelDelta();

    /**
        Get wheel rotation, positive or negative indicates direction of
        rotation.  Current devices all send an event when rotation is at least
        +/-WheelDelta, but finer resolution devices can be created in the future.
        Because of this you shouldn't assume that one event is equal to 1 line, but you
        should be able to either do partial line scrolling or wait until several
        events accumulate before scrolling.
    */
    int GetWheelRotation();

    /**
        Returns X coordinate of the physical mouse event position.
    */
    long GetX();

    /**
        Returns Y coordinate of the physical mouse event position.
    */
    long GetY();

    /**
        Returns @true if the event was a mouse button event (not necessarily a button
        down event -
        that may be tested using @e ButtonDown).
    */
    bool IsButton();

    /**
        Returns @true if the system has been setup to do page scrolling with
        the mouse wheel instead of line scrolling.
    */
    bool IsPageScroll();

    /**
        Returns @true if the mouse was leaving the window.
        See also Entering().
    */
    bool Leaving();

    /**
        Returns @true if the event was a left double click.
    */
    bool LeftDClick();

    /**
        Returns @true if the left mouse button changed to down.
    */
    bool LeftDown();

    /**
        Returns @true if the left mouse button is currently down, independent
        of the current event type.
        Please notice that it is not the same as
        LeftDown() which returns @true if the event was
        generated by the left mouse button being pressed. Rather, it simply describes
        the state of the left mouse button at the time when the event was generated
        (so while it will be @true for a left click event, it can also be @true for
        a right click if it happened while the left mouse button was pressed).
        This event is usually used in the mouse event handlers which process "move
        mouse" messages to determine whether the user is (still) dragging the mouse.
    */
    bool LeftIsDown();

    /**
        Returns @true if the left mouse button changed to up.
    */
    bool LeftUp();

    /**
        Returns @true if the Meta key was down at the time of the event.
    */
    bool MetaDown();

    /**
        Returns @true if the event was a middle double click.
    */
    bool MiddleDClick();

    /**
        Returns @true if the middle mouse button changed to down.
    */
    bool MiddleDown();

    /**
        Returns @true if the middle mouse button is currently down, independent
        of the current event type.
    */
    bool MiddleIsDown();

    /**
        Returns @true if the middle mouse button changed to up.
    */
    bool MiddleUp();

    /**
        Returns @true if this was a motion event and no mouse buttons were pressed.
        If any mouse button is held pressed, then this method returns @false and
        Dragging() returns @true.
    */
    bool Moving();

    /**
        Returns @true if the event was a right double click.
    */
    bool RightDClick();

    /**
        Returns @true if the right mouse button changed to down.
    */
    bool RightDown();

    /**
        Returns @true if the right mouse button is currently down, independent
        of the current event type.
    */
    bool RightIsDown();

    /**
        Returns @true if the right mouse button changed to up.
    */
    bool RightUp();

    /**
        Returns @true if the shift key was down at the time of the event.
    */
    bool ShiftDown();

    /**
        bool m_altDown
        @true if the Alt key is pressed down.
    */


    /**
        bool m_controlDown
        @true if control key is pressed down.
    */


    /**
        bool m_leftDown
        @true if the left mouse button is currently pressed down.
    */


    /**
        int m_linesPerAction
        The configured number of lines (or whatever) to be scrolled per wheel
        action.
    */


    /**
        bool m_metaDown
        @true if the Meta key is pressed down.
    */


    /**
        bool m_middleDown
        @true if the middle mouse button is currently pressed down.
    */


    /**
        bool m_rightDown
        @true if the right mouse button is currently pressed down.
    */


    /**
        bool m_shiftDown
        @true if shift is pressed down.
    */


    /**
        int m_wheelDelta
        The wheel delta, normally 120.
    */


    /**
        int m_wheelRotation
        The distance the mouse wheel is rotated.
    */


    /**
        long m_x
        X-coordinate of the event.
    */


    /**
        long m_y
        Y-coordinate of the event.
    */
};


/**
    @class wxDropFilesEvent
    @wxheader{event.h}

    This class is used for drop files events, that is, when files have been dropped
    onto the window. This functionality is currently only available under Windows.
    The window must have previously been enabled for dropping by calling
    wxWindow::DragAcceptFiles.

    Important note: this is a separate implementation to the more general
    drag and drop implementation documented here. It uses the
    older, Windows message-based approach of dropping files.

    @library{wxcore}
    @category{events}

    @seealso
    @ref overview_eventhandlingoverview
*/
class wxDropFilesEvent : public wxEvent
{
public:
    /**
        Constructor.
    */
    wxDropFilesEvent(WXTYPE id = 0, int noFiles = 0,
                     wxString* files = NULL);

    /**
        Returns an array of filenames.
    */
    wxString* GetFiles();

    /**
        Returns the number of files dropped.
    */
    int GetNumberOfFiles();

    /**
        Returns the position at which the files were dropped.
        Returns an array of filenames.
    */
    wxPoint GetPosition();

    /**
        wxString* m_files
        An array of filenames.
    */


    /**
        int m_noFiles
        The number of files dropped.
    */


    /**
        wxPoint m_pos
        The point at which the drop took place.
    */
};


/**
    @class wxCommandEvent
    @wxheader{event.h}

    This event class contains information about command events, which originate
    from a variety of
    simple controls. More complex controls, such as wxTreeCtrl, have separate
    command event classes.

    @library{wxcore}
    @category{events}
*/
class wxCommandEvent : public wxEvent
{
public:
    /**
        Constructor.
    */
    wxCommandEvent(WXTYPE commandEventType = 0, int id = 0);

    /**
        Deprecated, use IsChecked() instead.
    */
    bool Checked();

    /**
        Returns client data pointer for a listbox or choice selection event
        (not valid for a deselection).
    */
    void* GetClientData();

    /**
        Returns client object pointer for a listbox or choice selection event
        (not valid for a deselection).
    */
    wxClientData* GetClientObject();

    /**
        Returns extra information dependant on the event objects type.
        If the event comes from a listbox selection, it is a boolean
        determining whether the event was a selection (@true) or a
        deselection (@false). A listbox deselection only occurs for
        multiple-selection boxes, and in this case the index and string values
        are indeterminate and the listbox must be examined by the application.
    */
    long GetExtraLong();

    /**
        Returns the integer identifier corresponding to a listbox, choice or
        radiobox selection (only if the event was a selection, not a
        deselection), or a boolean value representing the value of a checkbox.
    */
    int GetInt();

    /**
        Returns item index for a listbox or choice selection event (not valid for
        a deselection).
    */
    int GetSelection();

    /**
        Returns item string for a listbox or choice selection event (not valid for
        a deselection).
    */
    wxString GetString();

    /**
        This method can be used with checkbox and menu events: for the checkboxes, the
        method returns @true for a selection event and @false for a
        deselection one. For the menu events, this method indicates if the menu item
        just has become checked or unchecked (and thus only makes sense for checkable
        menu items).
        Notice that this method can not be used with
        wxCheckListBox currently.
    */
    bool IsChecked();

    /**
        For a listbox or similar event, returns @true if it is a selection, @false if it
        is a deselection.
    */
    bool IsSelection();

    /**
        Sets the client data for this event.
    */
    void SetClientData(void* clientData);

    /**
        Sets the client object for this event. The client object is not owned by the
        event
        object and the event object will not delete the client object in its destructor.
        The client object must be owned and deleted by another object (e.g. a control)
        that has longer life time than the event object.
    */
    void SetClientObject(wxClientData* clientObject);

    /**
        Sets the @b m_extraLong member.
    */
    void SetExtraLong(long extraLong);

    /**
        Sets the @b m_commandInt member.
    */
    void SetInt(int intCommand);

    /**
        Sets the @b m_commandString member.
    */
    void SetString(const wxString& string);
};


/**
    @class wxActivateEvent
    @wxheader{event.h}

    An activate event is sent when a window or application is being activated
    or deactivated.

    @library{wxcore}
    @category{events}

    @seealso
    @ref overview_eventhandlingoverview, wxApp::IsActive
*/
class wxActivateEvent : public wxEvent
{
public:
    /**
        Constructor.
    */
    wxActivateEvent(WXTYPE eventType = 0, bool active = true,
                    int id = 0);

    /**
        Returns @true if the application or window is being activated, @false otherwise.
    */
    bool GetActive();
};


/**
    @class wxContextMenuEvent
    @wxheader{event.h}

    This class is used for context menu events, sent to give
    the application a chance to show a context (popup) menu.

    Note that if wxContextMenuEvent::GetPosition returns wxDefaultPosition, this
    means that the event originated
    from a keyboard context button event, and you should compute a suitable
    position yourself,
    for example by calling wxGetMousePosition.

    When a keyboard context menu button is pressed on Windows, a right-click event
    with default position is sent first,
    and if this event is not processed, the context menu event is sent. So if you
    process mouse events and you find your context menu event handler
    is not being called, you could call wxEvent::Skip for mouse right-down events.

    @library{wxcore}
    @category{events}

    @seealso
    @ref overview_wxcommandevent "Command events", @ref
    overview_eventhandlingoverview
*/
class wxContextMenuEvent : public wxCommandEvent
{
public:
    /**
        Constructor.
    */
    wxContextMenuEvent(WXTYPE id = 0, int id = 0,
                       const wxPoint& pos = wxDefaultPosition);

    /**
        Returns the position in screen coordinates at which the menu should be shown.
        Use wxWindow::ScreenToClient to
        convert to client coordinates. You can also omit a position from
        wxWindow::PopupMenu in order to use
        the current mouse pointer position.
        If the event originated from a keyboard event, the value returned from this
        function will be wxDefaultPosition.
    */
    wxPoint GetPosition();

    /**
        Sets the position at which the menu should be shown.
    */
    void SetPosition(const wxPoint& point);
};


/**
    @class wxEraseEvent
    @wxheader{event.h}

    An erase event is sent when a window's background needs to be repainted.

    On some platforms, such as GTK+, this event is simulated (simply generated just
    before the
    paint event) and may cause flicker. It is therefore recommended that
    you set the text background colour explicitly in order to prevent flicker.
    The default background colour under GTK+ is grey.

    To intercept this event, use the EVT_ERASE_BACKGROUND macro in an event table
    definition.

    You must call wxEraseEvent::GetDC and use the returned device context if it is
    non-@NULL.
    If it is @NULL, create your own temporary wxClientDC object.

    @library{wxcore}
    @category{events}

    @seealso
    @ref overview_eventhandlingoverview
*/
class wxEraseEvent : public wxEvent
{
public:
    /**
        Constructor.
    */
    wxEraseEvent(int id = 0, wxDC* dc = NULL);

    /**
        Returns the device context associated with the erase event to draw on.
    */
    wxDC* GetDC();
};


/**
    @class wxFocusEvent
    @wxheader{event.h}

    A focus event is sent when a window's focus changes. The window losing focus
    receives a "kill focus'' event while the window gaining it gets a "set
    focus'' one.

    Notice that the set focus event happens both when the user gives focus to the
    window (whether using the mouse or keyboard) and when it is done from the
    program itself using wxWindow::SetFocus.

    @library{wxcore}
    @category{events}

    @seealso
    @ref overview_eventhandlingoverview
*/
class wxFocusEvent : public wxEvent
{
public:
    /**
        Constructor.
    */
    wxFocusEvent(WXTYPE eventType = 0, int id = 0);

    /**
        Returns the window associated with this event, that is the window which had the
        focus before for the @c wxEVT_SET_FOCUS event and the window which is
        going to receive focus for the @c wxEVT_KILL_FOCUS one.
        Warning: the window pointer may be @NULL!
    */
};


/**
    @class wxChildFocusEvent
    @wxheader{event.h}

    A child focus event is sent to a (parent-)window when one of its child windows
    gains focus,
    so that the window could restore the focus back to its corresponding child
    if it loses it now and regains later.

    Notice that child window is the direct child of the window receiving event.
    Use wxWindow::FindFocus to retreive the window which is actually getting focus.

    @library{wxcore}
    @category{events}

    @seealso
    @ref overview_eventhandlingoverview
*/
class wxChildFocusEvent : public wxCommandEvent
{
public:
    /**
        Constructor.
        
        @param win
            The direct child which is (or which contains the window which is) receiving
        the focus.
    */
    wxChildFocusEvent(wxWindow* win = NULL);

    /**
        Returns the direct child which receives the focus, or a (grand-)parent of the
        control receiving the focus.
        To get the actually focused control use wxWindow::FindFocus.
    */
};


/**
    @class wxMouseCaptureLostEvent
    @wxheader{event.h}

    An mouse capture lost event is sent to a window that obtained mouse capture,
    which was subsequently loss due to "external" event, for example when a dialog
    box is shown or if another application captures the mouse.

    If this happens, this event is sent to all windows that are on capture stack
    (i.e. called CaptureMouse, but didn't call ReleaseMouse yet). The event is
    not sent if the capture changes because of a call to CaptureMouse or
    ReleaseMouse.

    This event is currently emitted under Windows only.

    @library{wxcore}
    @category{events}

    @seealso
    wxMouseCaptureChangedEvent, @ref overview_eventhandlingoverview,
    wxWindow::CaptureMouse, wxWindow::ReleaseMouse, wxWindow::GetCapture
*/
class wxMouseCaptureLostEvent : public wxEvent
{
public:
    /**
        Constructor.
    */
    wxMouseCaptureLostEvent(wxWindowID windowId = 0);
};


/**
    @class wxNotifyEvent
    @wxheader{event.h}

    This class is not used by the event handlers by itself, but is a base class
    for other event classes (such as wxNotebookEvent).

    It (or an object of a derived class) is sent when the controls state is being
    changed and allows the program to wxNotifyEvent::Veto this
    change if it wants to prevent it from happening.

    @library{wxcore}
    @category{events}

    @seealso
    wxNotebookEvent
*/
class wxNotifyEvent : public wxCommandEvent
{
public:
    /**
        Constructor (used internally by wxWidgets only).
    */
    wxNotifyEvent(wxEventType eventType = wxEVT_NULL, int id = 0);

    /**
        This is the opposite of Veto(): it explicitly
        allows the event to be processed. For most events it is not necessary to call
        this method as the events are allowed anyhow but some are forbidden by default
        (this will be mentioned in the corresponding event description).
    */
    void Allow();

    /**
        Returns @true if the change is allowed (Veto()
        hasn't been called) or @false otherwise (if it was).
    */
    bool IsAllowed();

    /**
        Prevents the change announced by this event from happening.
        It is in general a good idea to notify the user about the reasons for vetoing
        the change because otherwise the applications behaviour (which just refuses to
        do what the user wants) might be quite surprising.
    */
    void Veto();
};


/**
    @class wxHelpEvent
    @wxheader{event.h}

    A help event is sent when the user has requested context-sensitive help.
    This can either be caused by the application requesting
    context-sensitive help mode via wxContextHelp, or
    (on MS Windows) by the system generating a WM_HELP message when the user
    pressed F1 or clicked
    on the query button in a dialog caption.

    A help event is sent to the window that the user clicked on, and is propagated
    up the
    window hierarchy until the event is processed or there are no more event
    handlers.
    The application should call wxEvent::GetId to check the identity of the
    clicked-on window,
    and then either show some suitable help or call wxEvent::Skip if the identifier
    is unrecognised.
    Calling Skip is important because it allows wxWidgets to generate further
    events for ancestors
    of the clicked-on window. Otherwise it would be impossible to show help for
    container windows,
    since processing would stop after the first window found.

    @library{wxcore}
    @category{FIXME}

    @seealso
    wxContextHelp, wxDialog, @ref overview_eventhandlingoverview
*/
class wxHelpEvent : public wxCommandEvent
{
public:
    /**
        Constructor.
    */
    wxHelpEvent(WXTYPE eventType = 0, wxWindowID id = 0,
                const wxPoint& point);

    /**
        Returns the origin of the help event which is one of the following values:
        
        @b Origin_Unknown
        
        Unrecognized event source.
        
        @b Origin_Keyboard
        
        Event generated by @c F1 key press.
        
        @b Origin_HelpButton
        
        Event generated by
        wxContextHelp or using the "?" title bur button under
        MS Windows.
        
        The application may handle events generated using the keyboard or mouse
        differently, e.g. by using wxGetMousePosition
        for the mouse events.
        
        @see SetOrigin()
    */
    wxHelpEvent::Origin GetOrigin();

    /**
        Returns the left-click position of the mouse, in screen coordinates. This allows
        the application to position the help appropriately.
    */
    const wxPoint GetPosition();

    /**
        Set the help event origin, only used internally by wxWidgets normally.
        
        @see GetOrigin()
    */
    void SetOrigin(wxHelpEvent::Origin origin);

    /**
        Sets the left-click position of the mouse, in screen coordinates.
    */
    void SetPosition(const wxPoint& pt);
};


/**
    @class wxScrollEvent
    @wxheader{event.h}

    A scroll event holds information about events sent from stand-alone
    scrollbars and sliders. Note that
    starting from wxWidgets 2.1, scrolled windows send the
    wxScrollWinEvent which does not derive from
    wxCommandEvent, but from wxEvent directly - don't confuse these two kinds of
    events and use the event table macros mentioned below only for the
    scrollbar-like controls.

    @library{wxcore}
    @category{events}

    @seealso
    wxScrollBar, wxSlider, wxSpinButton, , wxScrollWinEvent, @ref
    overview_eventhandlingoverview
*/
class wxScrollEvent : public wxCommandEvent
{
public:
    /**
        Constructor.
    */
    wxScrollEvent(WXTYPE commandType = 0, int id = 0, int pos = 0,
                  int orientation = 0);

    /**
        Returns wxHORIZONTAL or wxVERTICAL, depending on the orientation of the
        scrollbar.
    */
    int GetOrientation();

    /**
        Returns the position of the scrollbar.
    */
    int GetPosition();
};


/**
    @class wxIdleEvent
    @wxheader{event.h}

    This class is used for idle events, which are generated when the system becomes
    idle. Note that, unless you do something specifically, the idle events are not
    sent if the system remains idle once it has become it, e.g. only a single idle
    event will be generated until something else resulting in more normal events
    happens and only then is the next idle event sent again. If you need to ensure
    a continuous stream of idle events, you can either use
    wxIdleEvent::RequestMore method in your handler or call
    wxWakeUpIdle periodically (for example from timer
    event), but note that both of these approaches (and especially the first one)
    increase the system load and so should be avoided if possible.

    By default, idle events are sent to all windows (and also
    wxApp, as usual). If this is causing a significant
    overhead in your application, you can call wxIdleEvent::SetMode with
    the value wxIDLE_PROCESS_SPECIFIED, and set the wxWS_EX_PROCESS_IDLE extra
    window style for every window which should receive idle events.

    @library{wxbase}
    @category{events}

    @seealso
    @ref overview_eventhandlingoverview, wxUpdateUIEvent, wxWindow::OnInternalIdle
*/
class wxIdleEvent : public wxEvent
{
public:
    /**
        Constructor.
    */
    wxIdleEvent();

    /**
        Returns @true if it is appropriate to send idle events to
        this window.
        This function looks at the mode used (see wxIdleEvent::SetMode),
        and the wxWS_EX_PROCESS_IDLE style in @a window to determine whether idle
        events should be sent to
        this window now. By default this will always return @true because
        the update mode is initially wxIDLE_PROCESS_ALL. You can change the mode
        to only send idle events to windows with the wxWS_EX_PROCESS_IDLE extra window
        style set.
        
        @see SetMode()
    */
    static bool CanSend(wxWindow* window);

    /**
        Static function returning a value specifying how wxWidgets
        will send idle events: to all windows, or only to those which specify that they
        will process the events.
        See SetMode().
    */
    static wxIdleMode GetMode();

    /**
        Returns @true if the OnIdle function processing this event requested more
        processing time.
        
        @see RequestMore()
    */
    bool MoreRequested();

    /**
        Tells wxWidgets that more processing is required. This function can be called
        by an OnIdle
        handler for a window or window event handler to indicate that wxApp::OnIdle
        should
        forward the OnIdle event once more to the application windows. If no window
        calls this function
        during OnIdle, then the application will remain in a passive event loop (not
        calling OnIdle) until a
        new event is posted to the application by the windowing system.
        
        @see MoreRequested()
    */
    void RequestMore(bool needMore = true);

    /**
        Static function for specifying how wxWidgets will send idle events: to
        all windows, or only to those which specify that they
        will process the events.
        @a mode can be one of the following values.
        The default is wxIDLE_PROCESS_ALL.
    */
    static void SetMode(wxIdleMode mode);
};


/**
    @class wxInitDialogEvent
    @wxheader{event.h}

    A wxInitDialogEvent is sent as a dialog or panel is being initialised.
    Handlers for this event can transfer data to the window.
    The default handler calls wxWindow::TransferDataToWindow.

    @library{wxcore}
    @category{events}

    @seealso
    @ref overview_eventhandlingoverview
*/
class wxInitDialogEvent : public wxEvent
{
public:
    /**
        Constructor.
    */
    wxInitDialogEvent(int id = 0);
};


/**
    @class wxWindowDestroyEvent
    @wxheader{event.h}

    This event is sent from the wxWindow destructor wxWindow::~wxWindow() when a
    window is destroyed.

    When a class derived from wxWindow is destroyed its destructor will have
    already run by the time this event is sent. Therefore this event will not
    usually be received at all.

    To receive this event wxEvtHandler::Connect
    must be used (using an event table macro will not work). Since it is
    received after the destructor has run, an object should not handle its
    own wxWindowDestroyEvent, but it can be used to get notification of the
    destruction of another window.

    @library{wxcore}
    @category{events}

    @seealso
    @ref overview_eventhandlingoverview, wxWindowCreateEvent
*/
class wxWindowDestroyEvent : public wxCommandEvent
{
public:
    /**
        Constructor.
    */
    wxWindowDestroyEvent(wxWindow* win = NULL);
};


/**
    @class wxNavigationKeyEvent
    @wxheader{event.h}

    This event class contains information about navigation events,
    generated by navigation keys such as tab and page down.

    This event is mainly used by wxWidgets implementations. A
    wxNavigationKeyEvent handler is automatically provided by wxWidgets
    when you make a class into a control container with the macro
    WX_DECLARE_CONTROL_CONTAINER.

    @library{wxcore}
    @category{events}

    @seealso
    wxWindow::Navigate, wxWindow::NavigateIn
*/
class wxNavigationKeyEvent
{
public:
    //@{
    /**
        Constructor.
    */
    wxNavigationKeyEvent();
    wxNavigationKeyEvent(const wxNavigationKeyEvent& event);
    //@}

    /**
        Returns the child that has the focus, or @NULL.
    */
    wxWindow* GetCurrentFocus();

    /**
        Returns @true if the navigation was in the forward direction.
    */
    bool GetDirection();

    /**
        Returns @true if the navigation event was from a tab key. This is required
        for proper navigation over radio buttons.
    */
    bool IsFromTab();

    /**
        Returns @true if the navigation event represents a window change (for
        example, from Ctrl-Page Down
        in a notebook).
    */
    bool IsWindowChange();

    /**
        Sets the current focus window member.
    */
    void SetCurrentFocus(wxWindow* currentFocus);

    /**
        Sets the direction to forward if @a direction is @true, or backward if @c
        @false.
    */
    void SetDirection(bool direction);

    /**
        Sets the flags.
    */
    void SetFlags(long flags);

    /**
        Marks the navigation event as from a tab key.
    */
    void SetFromTab(bool fromTab);

    /**
        Marks the event as a window change event.
    */
    void SetWindowChange(bool windowChange);
};


/**
    @class wxMouseCaptureChangedEvent
    @wxheader{event.h}

    An mouse capture changed event is sent to a window that loses its
    mouse capture. This is called even if wxWindow::ReleaseCapture
    was called by the application code. Handling this event allows
    an application to cater for unexpected capture releases which
    might otherwise confuse mouse handling code.

    This event is implemented under Windows only.

    @library{wxcore}
    @category{events}

    @seealso
    wxMouseCaptureLostEvent, @ref overview_eventhandlingoverview,
    wxWindow::CaptureMouse, wxWindow::ReleaseMouse, wxWindow::GetCapture
*/
class wxMouseCaptureChangedEvent : public wxEvent
{
public:
    /**
        Constructor.
    */
    wxMouseCaptureChangedEvent(wxWindowID windowId = 0,
                               wxWindow* gainedCapture = NULL);

    /**
        Returns the window that gained the capture, or @NULL if it was a non-wxWidgets
        window.
    */
    wxWindow* GetCapturedWindow();
};


/**
    @class wxCloseEvent
    @wxheader{event.h}

    This event class contains information about window and session close events.

    The handler function for EVT_CLOSE is called when the user has tried to close a
    a frame
    or dialog box using the window manager (X) or system menu (Windows). It can
    also be invoked by the application itself programmatically, for example by
    calling the wxWindow::Close function.

    You should check whether the application is forcing the deletion of the window
    using wxCloseEvent::CanVeto. If this is @false,
    you @e must destroy the window using wxWindow::Destroy.
    If the return value is @true, it is up to you whether you respond by destroying
    the window.

    If you don't destroy the window, you should call wxCloseEvent::Veto to
    let the calling code know that you did not destroy the window. This allows the
    wxWindow::Close function
    to return @true or @false depending on whether the close instruction was
    honoured or not.

    @library{wxcore}
    @category{events}

    @seealso
    wxWindow::Close, @ref overview_windowdeletionoverview "Window deletion overview"
*/
class wxCloseEvent : public wxEvent
{
public:
    /**
        Constructor.
    */
    wxCloseEvent(WXTYPE commandEventType = 0, int id = 0);

    /**
        Returns @true if you can veto a system shutdown or a window close event.
        Vetoing a window close event is not possible if the calling code wishes to
        force the application to exit, and so this function must be called to check
        this.
    */
    bool CanVeto();

    /**
        Returns @true if the user is just logging off or @false if the system is
        shutting down. This method can only be called for end session and query end
        session events, it doesn't make sense for close window event.
    */
    bool GetLoggingOff();

    /**
        Sets the 'can veto' flag.
    */
    void SetCanVeto(bool canVeto);

    /**
        Sets the 'force' flag.
    */
    void SetForce(bool force);

    /**
        Sets the 'logging off' flag.
    */
    void SetLoggingOff(bool loggingOff);

    /**
        Call this from your event handler to veto a system shutdown or to signal
        to the calling application that a window close did not happen.
        You can only veto a shutdown if CanVeto() returns
        @true.
    */
    void Veto(bool veto = true);
};


/**
    @class wxMenuEvent
    @wxheader{event.h}

    This class is used for a variety of menu-related events. Note that
    these do not include menu command events, which are
    handled using wxCommandEvent objects.

    The default handler for wxEVT_MENU_HIGHLIGHT displays help
    text in the first field of the status bar.

    @library{wxcore}
    @category{events}

    @seealso
    @ref overview_wxcommandevent "Command events", @ref
    overview_eventhandlingoverview
*/
class wxMenuEvent : public wxEvent
{
public:
    /**
        Constructor.
    */
    wxMenuEvent(WXTYPE id = 0, int id = 0, wxMenu* menu = NULL);

    /**
        Returns the menu which is being opened or closed. This method should only be
        used with the @c OPEN and @c CLOSE events and even for them the
        returned pointer may be @NULL in some ports.
    */
    wxMenu* GetMenu();

    /**
        Returns the menu identifier associated with the event. This method should be
        only used with the @c HIGHLIGHT events.
    */
    int GetMenuId();

    /**
        Returns @true if the menu which is being opened or closed is a popup menu,
        @false if it is a normal one.
        This method should only be used with the @c OPEN and @c CLOSE events.
    */
    bool IsPopup();
};


/**
    @class wxEventBlocker
    @wxheader{event.h}

    This class is a special event handler which allows to discard
    any event (or a set of event types) directed to a specific window.

    Example:

    @code
    {
        // block all events directed to this window while
        // we do the 1000 FuncWhichSendsEvents() calls
        wxEventBlocker blocker(this);

        for ( int i = 0; i  1000; i++ )
           FuncWhichSendsEvents(i);

      } // ~wxEventBlocker called, old event handler is restored

      // the event generated by this call will be processed
      FuncWhichSendsEvents(0)
    @endcode

    @library{wxcore}
    @category{FIXME}

    @seealso
    @ref overview_eventhandlingoverview, wxEvtHandler
*/
class wxEventBlocker : public wxEvtHandler
{
public:
    /**
        Constructs the blocker for the given window and for the given event type.
        If @a type is @c wxEVT_ANY, then all events for that window are
        blocked. You can call Block() after creation to
        add other event types to the list of events to block.
        Note that the @a win window @b must remain alive until the
        wxEventBlocker object destruction.
    */
    wxEventBlocker(wxWindow* win, wxEventType type = wxEVT_ANY);

    /**
        Destructor. The blocker will remove itself from the chain of event handlers for
        the window provided in the constructor, thus restoring normal processing of
        events.
    */
    ~wxEventBlocker();

    /**
        Adds to the list of event types which should be blocked the given @e eventType.
    */
    void Block(wxEventType eventType);
};


/**
    @class wxEvtHandler
    @wxheader{event.h}

    A class that can handle events from the windowing system.
    wxWindow (and therefore all window classes) are derived from
    this class.

    When events are received, wxEvtHandler invokes the method listed in the
    event table using itself as the object.  When using multiple inheritance
    it is imperative that the wxEvtHandler(-derived) class be the first
    class inherited such that the "this" pointer for the overall object
    will be identical to the "this" pointer for the wxEvtHandler portion.

    @library{wxbase}
    @category{FIXME}

    @seealso
    @ref overview_eventhandlingoverview
*/
class wxEvtHandler : public wxObject
{
public:
    /**
        Constructor.
    */
    wxEvtHandler();

    /**
        Destructor. If the handler is part of a chain, the destructor will
        unlink itself and restore the previous and next handlers so that they point to
        each other.
    */
    ~wxEvtHandler();

    /**
        This function posts an event to be processed later.
        
        @param event
            Event to add to process queue.
        
        @remarks The difference between sending an event (using the ProcessEvent
                 method) and posting it is that in the first case the
                 event is processed before the function returns, while
                 in the second case, the function returns immediately
                 and the event will be processed sometime later (usually
                 during the next event loop iteration).
    */
    virtual void AddPendingEvent(const wxEvent& event);

    //@{
    /**
        Connects the given function dynamically with the event handler, id and event
        type. This
        is an alternative to the use of static event tables. See the 'event' or the old
        'dynamic' sample for usage.
        
        @param id
            The identifier (or first of the identifier range) to be
            associated with the event handler function. For the version not taking this
            argument, it defaults to wxID_ANY.
        @param lastId
            The second part of the identifier range to be associated with the event
        handler function.
        @param eventType
            The event type to be associated with this event handler.
        @param function
            The event handler function. Note that this function should
            be explicitly converted to the correct type which can be done using a macro
            called wxFooEventHandler for the handler for any wxFooEvent.
        @param userData
            Data to be associated with the event table entry.
        @param eventSink
            Object whose member function should be called. If this is @NULL,
            this will be used.
    */
    void Connect(int id, int lastId, wxEventType eventType,
                 wxObjectEventFunction function,
                 wxObject* userData = NULL,
                 wxEvtHandler* eventSink = NULL);
    void Connect(int id, wxEventType eventType,
                 wxObjectEventFunction function,
                 wxObject* userData = NULL,
                 wxEvtHandler* eventSink = NULL);
    void Connect(wxEventType eventType,
                 wxObjectEventFunction function,
                 wxObject* userData = NULL,
                 wxEvtHandler* eventSink = NULL);
    //@}

    //@{
    /**
        Disconnects the given function dynamically from the event handler, using the
        specified
        parameters as search criteria and returning @true if a matching function has been
        found and removed. This method can only disconnect functions which have been
        added
        using the Connect() method. There is no way
        to disconnect functions connected using the (static) event tables.
        
        @param id
            The identifier (or first of the identifier range) associated with the event
        handler function.
        @param lastId
            The second part of the identifier range associated with the event handler
        function.
        @param eventType
            The event type associated with this event handler.
        @param function
            The event handler function.
        @param userData
            Data associated with the event table entry.
        @param eventSink
            Object whose member function should be called.
    */
    bool Disconnect(wxEventType eventType = wxEVT_NULL,
                    wxObjectEventFunction function = NULL,
                    wxObject* userData = NULL,
                    wxEvtHandler* eventSink = NULL);
    bool Disconnect(int id = wxID_ANY,
                    wxEventType eventType = wxEVT_NULL,
                    wxObjectEventFunction function = NULL,
                    wxObject* userData = NULL,
                    wxEvtHandler* eventSink = NULL);
    bool Disconnect(int id, int lastId = wxID_ANY,
                    wxEventType eventType = wxEVT_NULL,
                    wxObjectEventFunction function = NULL,
                    wxObject* userData = NULL,
                    wxEvtHandler* eventSink = NULL);
    //@}

    /**
        Gets user-supplied client data.
        
        @remarks Normally, any extra data the programmer wishes to associate with
                 the object should be made available by deriving a new
                 class with new data members.
        
        @see SetClientData()
    */
    void* GetClientData();

    /**
        Get a pointer to the user-supplied client data object.
        
        @see SetClientObject(), wxClientData
    */
    wxClientData* GetClientObject();

    /**
        Returns @true if the event handler is enabled, @false otherwise.
        
        @see SetEvtHandlerEnabled()
    */
    bool GetEvtHandlerEnabled();

    /**
        Gets the pointer to the next handler in the chain.
        
        @see SetNextHandler(), GetPreviousHandler(),
             SetPreviousHandler(), wxWindow::PushEventHandler,
             wxWindow::PopEventHandler
    */
    wxEvtHandler* GetNextHandler();

    /**
        Gets the pointer to the previous handler in the chain.
        
        @see SetPreviousHandler(), GetNextHandler(),
             SetNextHandler(), wxWindow::PushEventHandler,
             wxWindow::PopEventHandler
    */
    wxEvtHandler* GetPreviousHandler();

    /**
        Processes an event, searching event tables and calling zero or more suitable
        event handler function(s).
        
        @param event
            Event to process.
        
        @returns @true if a suitable event handler function was found and
                 executed, and the function did not call wxEvent::Skip.
        
        @remarks Normally, your application would not call this function: it is
                 called in the wxWidgets implementation to dispatch
                 incoming user interface events to the framework (and
                 application).
        
        @see SearchEventTable()
    */
    virtual bool ProcessEvent(wxEvent& event);

    /**
        Processes an event by calling ProcessEvent()
        and handles any exceptions that occur in the process. If an exception is
        thrown in event handler, wxApp::OnExceptionInMainLoop
        is called.
        
        @param event
            Event to process.
        
        @returns @true if the event was processed, @false if no handler was found
                 or an exception was thrown.
        
        @see wxWindow::HandleWindowEvent
    */
    bool SafelyProcessEvent(wxEvent& event);

    /**
        Searches the event table, executing an event handler function if an appropriate
        one
        is found.
        
        @param table
            Event table to be searched.
        @param event
            Event to be matched against an event table entry.
        
        @returns @true if a suitable event handler function was found and
                 executed, and the function did not call wxEvent::Skip.
        
        @remarks This function looks through the object's event table and tries
                 to find an entry that will match the event.
        
        @see ProcessEvent()
    */
    virtual bool SearchEventTable(wxEventTable& table,
                                  wxEvent& event);

    /**
        Sets user-supplied client data.
        
        @param data
            Data to be associated with the event handler.
        
        @remarks Normally, any extra data the programmer wishes to associate with
                  the object should be made available by deriving a new
                 class with new data members. You must not call this
                 method and SetClientObject on the same class - only one
                 of them.
        
        @see GetClientData()
    */
    void SetClientData(void* data);

    /**
        Set the client data object. Any previous object will be deleted.
        
        @see GetClientObject(), wxClientData
    */
    void SetClientObject(wxClientData* data);

    /**
        Enables or disables the event handler.
        
        @param enabled
            @true if the event handler is to be enabled, @false if it is to be disabled.
        
        @remarks You can use this function to avoid having to remove the event
                 handler from the chain, for example when implementing a
                 dialog editor and changing from edit to test mode.
        
        @see GetEvtHandlerEnabled()
    */
    void SetEvtHandlerEnabled(bool enabled);

    /**
        Sets the pointer to the next handler.
        
        @param handler
            Event handler to be set as the next handler.
        
        @see GetNextHandler(), SetPreviousHandler(),
             GetPreviousHandler(), wxWindow::PushEventHandler,
             wxWindow::PopEventHandler
    */
    void SetNextHandler(wxEvtHandler* handler);

    /**
        Sets the pointer to the previous handler.
        
        @param handler
            Event handler to be set as the previous handler.
    */
    void SetPreviousHandler(wxEvtHandler* handler);
};


/**
    @class wxIconizeEvent
    @wxheader{event.h}

    An event being sent when the frame is iconized (minimized) or restored.

    Currently only wxMSW and wxGTK generate such events.

    @library{wxcore}
    @category{events}

    @seealso
    @ref overview_eventhandlingoverview, wxTopLevelWindow::Iconize,
    wxTopLevelWindow::IsIconized
*/
class wxIconizeEvent : public wxEvent
{
public:
    /**
        Constructor.
    */
    wxIconizeEvent(int id = 0, bool iconized = true);

    /**
        Returns @true if the frame has been iconized, @false if it has been
        restored.
    */
    bool Iconized();
};


/**
    @class wxMoveEvent
    @wxheader{event.h}

    A move event holds information about move change events.

    @library{wxcore}
    @category{events}

    @seealso
    wxPoint, @ref overview_eventhandlingoverview
*/
class wxMoveEvent : public wxEvent
{
public:
    /**
        Constructor.
    */
    wxMoveEvent(const wxPoint& pt, int id = 0);

    /**
        Returns the position of the window generating the move change event.
    */
    wxPoint GetPosition();
};


/**
    @class wxEvent
    @wxheader{event.h}

    An event is a structure holding information about an event passed to a
    callback or member function. @b wxEvent used to be a multipurpose
    event object, and is an abstract base class for other event classes (see below).

    For more information about events, see the @ref overview_eventhandlingoverview.

    @b wxPerl note: In wxPerl custom event classes should be derived from
    @c Wx::PlEvent and @c Wx::PlCommandEvent.

    @library{wxbase}
    @category{events}

    @seealso
    wxCommandEvent, wxMouseEvent
*/
class wxEvent : public wxObject
{
public:
    /**
        Constructor. Should not need to be used directly by an application.
    */
    wxEvent(int id = 0, wxEventType eventType = wxEVT_NULL);

    /**
        Returns a copy of the event.
        Any event that is posted to the wxWidgets event system for later action (via
        wxEvtHandler::AddPendingEvent or
        wxPostEvent) must implement this method. All wxWidgets
        events fully implement this method, but any derived events implemented by the
        user should also implement this method just in case they (or some event
        derived from them) are ever posted.
        All wxWidgets events implement a copy constructor, so the easiest way of
        implementing the Clone function is to implement a copy constructor for
        a new event (call it MyEvent) and then define the Clone function like this:
    */
    virtual wxEvent* Clone();

    /**
        Returns the object (usually a window) associated with the
        event, if any.
    */
    wxObject* GetEventObject();

    /**
        Returns the identifier of the given event type,
        such as @c wxEVT_COMMAND_BUTTON_CLICKED.
    */
    wxEventType GetEventType();

    /**
        Returns the identifier associated with this event, such as a button command id.
    */
    int GetId();

    /**
        Returns @true if the event handler should be skipped, @false otherwise.
    */
    bool GetSkipped();

    /**
        Gets the timestamp for the event. The timestamp is the time in milliseconds
        since some fixed moment (not necessarily the standard Unix Epoch, so
        only differences between the timestamps and not their absolute values usually
        make sense).
    */
    long GetTimestamp();

    /**
        Returns @true if the event is or is derived from
        wxCommandEvent else it returns @false.
        Note: Exists only for optimization purposes.
    */
    bool IsCommandEvent();

    /**
        Sets the propagation level to the given value (for example returned from an
        earlier call to wxEvent::StopPropagation).
    */
    void ResumePropagation(int propagationLevel);

    /**
        Sets the originating object.
    */
    void SetEventObject(wxObject* object);

    /**
        Sets the event type.
    */
    void SetEventType(wxEventType type);

    /**
        Sets the identifier associated with this event, such as a button command id.
    */
    void SetId(int id);

    /**
        Sets the timestamp for the event.
    */
    void SetTimestamp(long timeStamp);

    /**
        Test if this event should be propagated or not, i.e. if the propagation level
        is currently greater than 0.
    */
    bool ShouldPropagate();

    /**
        This method can be used inside an event handler to control whether further
        event handlers bound to this event will be called after the current one
        returns. Without Skip() (or equivalently if Skip(@false) is used),
        the event will not be processed any more. If Skip(@true) is called, the event
        processing system continues searching for a further handler function for this
        event, even though it has been processed already in the current handler.
        In general, it is recommended to skip all non-command events to allow the
        default handling to take place. The command events are, however, normally not
        skipped as usually a single command such as a button click or menu item
        selection must only be processed by one handler.
    */
    void Skip(bool skip = true);

    /**
        Stop the event from propagating to its parent window.
        Returns the old propagation level value which may be later passed to
        ResumePropagation() to allow propagating the
        event again.
    */
    int StopPropagation();

    /**
        int m_propagationLevel
        Indicates how many levels the event can propagate. This member is protected and
        should typically only be set in the constructors of the derived classes. It
        may be temporarily changed by StopPropagation()
        and ResumePropagation() and tested with
        ShouldPropagate().
        The initial value is set to either @c wxEVENT_PROPAGATE_NONE (by
        default) meaning that the event shouldn't be propagated at all or to
        @c wxEVENT_PROPAGATE_MAX (for command events) meaning that it should be
        propagated as much as necessary.
        Any positive number means that the event should be propagated but no more than
        the given number of times. E.g. the propagation level may be set to 1 to
        propagate the event to its parent only, but not to its grandparent.
    */
};


/**
    @class wxSizeEvent
    @wxheader{event.h}

    A size event holds information about size change events.

    The EVT_SIZE handler function will be called when the window has been resized.

    You may wish to use this for frames to resize their child windows as
    appropriate.

    Note that the size passed is of
    the whole window: call wxWindow::GetClientSize for the area which may be
    used by the application.

    When a window is resized, usually only a small part of the window is damaged
    and you
    may only need to repaint that area. However, if your drawing depends on the
    size of the window,
    you may need to clear the DC explicitly and repaint the whole window. In which
    case, you
    may need to call wxWindow::Refresh to invalidate the entire window.

    @library{wxcore}
    @category{events}

    @seealso
    wxSize, @ref overview_eventhandlingoverview
*/
class wxSizeEvent : public wxEvent
{
public:
    /**
        Constructor.
    */
    wxSizeEvent(const wxSize& sz, int id = 0);

    /**
        Returns the entire size of the window generating the size change event.
    */
    wxSize GetSize();
};


/**
    @class wxSetCursorEvent
    @wxheader{event.h}

    A SetCursorEvent is generated when the mouse cursor is about to be set as a
    result of mouse motion. This event gives the application the chance to perform
    specific mouse cursor processing based on the current position of the mouse
    within the window. Use wxSetCursorEvent::SetCursor to
    specify the cursor you want to be displayed.

    @library{wxcore}
    @category{FIXME}

    @seealso
    ::wxSetCursor, wxWindow::wxSetCursor
*/
class wxSetCursorEvent : public wxEvent
{
public:
    /**
        Constructor, used by the library itself internally to initialize the event
        object.
    */
    wxSetCursorEvent(wxCoord x = 0, wxCoord y = 0);

    /**
        Returns a reference to the cursor specified by this event.
    */
    wxCursor GetCursor();

    /**
        Returns the X coordinate of the mouse in client coordinates.
    */
    wxCoord GetX();

    /**
        Returns the Y coordinate of the mouse in client coordinates.
    */
    wxCoord GetY();

    /**
        Returns @true if the cursor specified by this event is a valid cursor.
        
        @remarks You cannot specify wxNullCursor with this event, as it is not
                 considered a valid cursor.
    */
    bool HasCursor();

    /**
        Sets the cursor associated with this event.
    */
    void SetCursor(const wxCursor& cursor);
};
