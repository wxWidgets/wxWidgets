/////////////////////////////////////////////////////////////////////////////
// Name:        event.h
// Purpose:     interface of wxEventHandler, wxEventBlocker and many
//              wxEvent-derived classes
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////



/**
    @class wxEvent

    An event is a structure holding information about an event passed to a
    callback or member function.

    wxEvent used to be a multipurpose event object, and is an abstract base class
    for other event classes (see below).

    For more information about events, see the @ref overview_eventhandling overview.

    @beginWxPerlOnly
    In wxPerl custom event classes should be derived from
    @c Wx::PlEvent and @c Wx::PlCommandEvent.
    @endWxPerlOnly

    @library{wxbase}
    @category{events}

    @see wxCommandEvent, wxMouseEvent
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

        Any event that is posted to the wxWidgets event system for later action
        (via wxEvtHandler::AddPendingEvent or wxPostEvent()) must implement
        this method.

        All wxWidgets events fully implement this method, but any derived events
        implemented by the user should also implement this method just in case they
        (or some event derived from them) are ever posted.

        All wxWidgets events implement a copy constructor, so the easiest way of
        implementing the Clone function is to implement a copy constructor for
        a new event (call it MyEvent) and then define the Clone function like this:

        @code
        wxEvent *Clone() const { return new MyEvent(*this); }
        @endcode
    */
    virtual wxEvent* Clone() const = 0;

    /**
        Returns the object (usually a window) associated with the event, if any.
    */
    wxObject* GetEventObject() const;

    /**
        Returns the identifier of the given event type, such as @c wxEVT_COMMAND_BUTTON_CLICKED.
    */
    wxEventType GetEventType() const;

    /**
        Returns the identifier associated with this event, such as a button command id.
    */
    int GetId() const;

    /**
        Returns @true if the event handler should be skipped, @false otherwise.
    */
    bool GetSkipped() const;

    /**
        Gets the timestamp for the event. The timestamp is the time in milliseconds
        since some fixed moment (not necessarily the standard Unix Epoch, so only
        differences between the timestamps and not their absolute values usually make sense).
    */
    long GetTimestamp() const;

    /**
        Returns @true if the event is or is derived from wxCommandEvent else it returns @false.

        @note exists only for optimization purposes.
    */
    bool IsCommandEvent() const;

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
    void SetTimestamp(long = 0);

    /**
        Test if this event should be propagated or not, i.e. if the propagation level
        is currently greater than 0.
    */
    bool ShouldPropagate() const;

    /**
        This method can be used inside an event handler to control whether further
        event handlers bound to this event will be called after the current one returns.

        Without Skip() (or equivalently if Skip(@false) is used), the event will not
        be processed any more. If Skip(@true) is called, the event processing system
        continues searching for a further handler function for this event, even though
        it has been processed already in the current handler.

        In general, it is recommended to skip all non-command events to allow the
        default handling to take place. The command events are, however, normally not
        skipped as usually a single command such as a button click or menu item
        selection must only be processed by one handler.
    */
    void Skip(bool skip = true);

    /**
        Stop the event from propagating to its parent window.

        Returns the old propagation level value which may be later passed to
        ResumePropagation() to allow propagating the event again.
    */
    int StopPropagation();

protected:
    /**
        Indicates how many levels the event can propagate.

        This member is protected and should typically only be set in the constructors
        of the derived classes. It may be temporarily changed by StopPropagation()
        and ResumePropagation() and tested with ShouldPropagate().

        The initial value is set to either @c wxEVENT_PROPAGATE_NONE (by default)
        meaning that the event shouldn't be propagated at all or to
        @c wxEVENT_PROPAGATE_MAX (for command events) meaning that it should be
        propagated as much as necessary.

        Any positive number means that the event should be propagated but no more than
        the given number of times. E.g. the propagation level may be set to 1 to
        propagate the event to its parent only, but not to its grandparent.
    */
    int m_propagationLevel;
};

/**
    @class wxEventBlocker

    This class is a special event handler which allows to discard
    any event (or a set of event types) directed to a specific window.

    Example:

    @code
    void MyWindow::DoSomething()
    {
        {
            // block all events directed to this window while
            // we do the 1000 FunctionWhichSendsEvents() calls
            wxEventBlocker blocker(this);

            for ( int i = 0; i  1000; i++ )
                FunctionWhichSendsEvents(i);

        } // ~wxEventBlocker called, old event handler is restored

        // the event generated by this call will be processed:
        FunctionWhichSendsEvents(0)
    }
    @endcode

    @library{wxcore}
    @category{events}

    @see @ref overview_eventhandling, wxEvtHandler
*/
class wxEventBlocker : public wxEvtHandler
{
public:
    /**
        Constructs the blocker for the given window and for the given event type.

        If @a type is @c wxEVT_ANY, then all events for that window are blocked.
        You can call Block() after creation to add other event types to the list
        of events to block.

        Note that the @a win window @b must remain alive until the
        wxEventBlocker object destruction.
    */
    wxEventBlocker(wxWindow* win, wxEventType type = -1);

    /**
        Destructor. The blocker will remove itself from the chain of event handlers for
        the window provided in the constructor, thus restoring normal processing of events.
    */
    virtual ~wxEventBlocker();

    /**
        Adds to the list of event types which should be blocked the given @a eventType.
    */
    void Block(wxEventType eventType);
};



/**
    @class wxEvtHandler

    A class that can handle events from the windowing system.
    wxWindow (and therefore all window classes) are derived from this class.

    When events are received, wxEvtHandler invokes the method listed in the
    event table using itself as the object.  When using multiple inheritance
    it is imperative that the wxEvtHandler(-derived) class be the first
    class inherited such that the "this" pointer for the overall object
    will be identical to the "this" pointer for the wxEvtHandler portion.

    @library{wxbase}
    @category{events}

    @see @ref overview_eventhandling
*/
class wxEvtHandler : public wxObject
{
public:
    /**
        Constructor.
    */
    wxEvtHandler();

    /**
        Destructor.

        If the handler is part of a chain, the destructor will unlink itself and
        restore the previous and next handlers so that they point to each other.
    */
    virtual ~wxEvtHandler();

    /**
        Queue event for a later processing.

        This method is similar to ProcessEvent() but while the latter is
        synchronous, i.e. the event is processed immediately, before the
        function returns, this one is asynchronous and returns immediately
        while the event will be processed at some later time (usually during
        the next event loop iteration).

        Another important difference is that this method takes ownership of the
        @a event parameter, i.e. it will delete it itself. This implies that
        the event should be allocated on the heap and that the pointer can't be
        used any more after the function returns (as it can be deleted at any
        moment).

        QueueEvent() can be used for inter-thread communication from the worker
        threads to the main thread, it is safe in the sense that it uses
        locking internally and avoids the problem mentioned in AddPendingEvent()
        documentation by ensuring that the @a event object is not used by the
        calling thread any more. Care should still be taken to avoid that some
        fields of this object are used by it, notably any wxString members of
        the event object must not be shallow copies of another wxString object
        as this would result in them still using the same string buffer behind
        the scenes. For example
        @code
            void FunctionInAWorkerThread(const wxString& str)
            {
                wxCommandEvent* evt = new wxCommandEvent;

                // NOT evt->SetString(str) as this would be a shallow copy
                evt->SetString(str.c_str()); // make a deep copy

                wxTheApp->QueueEvent( evt );
            }
        @endcode

        Finally notice that this method automatically wakes up the event loop
        if it is currently idle by calling ::wxWakeUpIdle() so there is no need
        to do it manually when using it.

        @since 2.9.0

        @param event
            A heap-allocated event to be queued, QueueEvent() takes ownership
            of it. This parameter shouldn't be @c NULL.
     */
    virtual void QueueEvent(wxEvent *event);

    /**
        Post an event to be processed later.

        This function is similar to QueueEvent() but can't be used to post
        events from worker threads for the event objects with wxString fields
        (i.e. in practice most of them) because of an unsafe use of the same
        wxString object which happens because the wxString field in the
        original @a event object and its copy made internally by this function
        share the same string buffer internally. Use QueueEvent() to avoid
        this.

        A copy of event is made by the function, so the original can be deleted
        as soon as function returns (it is common that the original is created
        on the stack). This requires that the wxEvent::Clone() method be
        implemented by event so that it can be duplicated and stored until it
        gets processed.

        @param event
            Event to add to the pending events queue.
    */
    virtual void AddPendingEvent(const wxEvent& event);

    /**
        Connects the given function dynamically with the event handler, id and event type.
        This is an alternative to the use of static event tables.

        See the @ref page_samples_event sample for usage.

        This specific overload allows you to connect an event handler to a @e range
        of @e source IDs.
        Do not confuse @e source IDs with event @e types: source IDs identify the
        event generator objects (typically wxMenuItem or wxWindow objects) while the
        event @e type identify which type of events should be handled by the
        given @e function (an event generator object may generate many different
        types of events!).

        @param id
            The first ID of the identifier range to be associated with the event
            handler function.
        @param lastId
            The last ID of the identifier range to be associated with the event
            handler function.
        @param eventType
            The event type to be associated with this event handler.
        @param function
            The event handler function. Note that this function should
            be explicitly converted to the correct type which can be done using a macro
            called @c wxFooEventHandler for the handler for any @c wxFooEvent.
        @param userData
            Data to be associated with the event table entry.
        @param eventSink
            Object whose member function should be called.
            If this is @NULL, @c *this will be used.
    */
    void Connect(int id, int lastId, wxEventType eventType,
                 wxObjectEventFunction function,
                 wxObject* userData = NULL,
                 wxEvtHandler* eventSink = NULL);

    /**
        See the Connect(int, int, wxEventType, wxObjectEventFunction, wxObject*, wxEvtHandler*)
        overload for more info.

        This overload can be used to attach an event handler to a single source ID:

        Example:
        @code
        frame->Connect( wxID_EXIT,
                        wxEVT_COMMAND_MENU_SELECTED,
                        wxCommandEventHandler(MyFrame::OnQuit) );
        @endcode
    */
    void Connect(int id, wxEventType eventType,
                 wxObjectEventFunction function,
                 wxObject* userData = NULL,
                 wxEvtHandler* eventSink = NULL);

    /**
        See the Connect(int, int, wxEventType, wxObjectEventFunction, wxObject*, wxEvtHandler*)
        overload for more info.

        This overload will connect the given event handler so that regardless of the
        ID of the event source, the handler will be called.
    */
    void Connect(wxEventType eventType,
                 wxObjectEventFunction function,
                 wxObject* userData = NULL,
                 wxEvtHandler* eventSink = NULL);

    /**
        Disconnects the given function dynamically from the event handler, using the
        specified parameters as search criteria and returning @true if a matching
        function has been found and removed.

        This method can only disconnect functions which have been added using the
        Connect() method. There is no way to disconnect functions connected using
        the (static) event tables.

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

    /**
        See the Disconnect(wxEventType, wxObjectEventFunction, wxObject*, wxEvtHandler*)
        overload for more info.

        This overload takes the additional @a id parameter.
    */
    bool Disconnect(int id = wxID_ANY,
                    wxEventType eventType = wxEVT_NULL,
                    wxObjectEventFunction function = NULL,
                    wxObject* userData = NULL,
                    wxEvtHandler* eventSink = NULL);

    /**
        See the Disconnect(wxEventType, wxObjectEventFunction, wxObject*, wxEvtHandler*)
        overload for more info.

        This overload takes an additional range of source IDs.
    */
    bool Disconnect(int id, int lastId = wxID_ANY,
                    wxEventType eventType = wxEVT_NULL,
                    wxObjectEventFunction function = NULL,
                    wxObject* userData = NULL,
                    wxEvtHandler* eventSink = NULL);

    /**
        Returns user-supplied client data.

        @remarks Normally, any extra data the programmer wishes to associate with
                 the object should be made available by deriving a new class with
                 new data members.

        @see SetClientData()
    */
    void* GetClientData() const;

    /**
        Returns a pointer to the user-supplied client data object.

        @see SetClientObject(), wxClientData
    */
    wxClientData* GetClientObject() const;

    /**
        Returns @true if the event handler is enabled, @false otherwise.

        @see SetEvtHandlerEnabled()
    */
    bool GetEvtHandlerEnabled() const;

    /**
        Returns the pointer to the next handler in the chain.

        @see SetNextHandler(), GetPreviousHandler(), SetPreviousHandler(),
             wxWindow::PushEventHandler, wxWindow::PopEventHandler
    */
    wxEvtHandler* GetNextHandler() const;

    /**
        Returns the pointer to the previous handler in the chain.

        @see SetPreviousHandler(), GetNextHandler(), SetNextHandler(),
             wxWindow::PushEventHandler, wxWindow::PopEventHandler
    */
    wxEvtHandler* GetPreviousHandler() const;

    /**
        Processes an event, searching event tables and calling zero or more suitable
        event handler function(s).

        Normally, your application would not call this function: it is called in the
        wxWidgets implementation to dispatch incoming user interface events to the
        framework (and application).

        However, you might need to call it if implementing new functionality
        (such as a new control) where you define new event types, as opposed to
        allowing the user to override virtual functions.

        An instance where you might actually override the ProcessEvent function is where
        you want to direct event processing to event handlers not normally noticed by
        wxWidgets. For example, in the document/view architecture, documents and views
        are potential event handlers. When an event reaches a frame, ProcessEvent will
        need to be called on the associated document and view in case event handler functions
        are associated with these objects. The property classes library (wxProperty) also
        overrides ProcessEvent for similar reasons.

        The normal order of event table searching is as follows:
        -# If the object is disabled (via a call to wxEvtHandler::SetEvtHandlerEnabled)
            the function skips to step (6).
        -# If the object is a wxWindow, ProcessEvent() is recursively called on the
            window's wxValidator. If this returns @true, the function exits.
        -# SearchEventTable() is called for this event handler. If this fails, the base
            class table is tried, and so on until no more tables exist or an appropriate
            function was found, in which case the function exits.
        -# The search is applied down the entire chain of event handlers (usually the
            chain has a length of one). If this succeeds, the function exits.
        -# If the object is a wxWindow and the event is a wxCommandEvent, ProcessEvent()
            is recursively applied to the parent window's event handler.
            If this returns true, the function exits.
        -# Finally, ProcessEvent() is called on the wxApp object.

        @param event
            Event to process.

        @return @true if a suitable event handler function was found and
                 executed, and the function did not call wxEvent::Skip.

        @see SearchEventTable()
    */
    virtual bool ProcessEvent(wxEvent& event);

    /**
        Processes an event by calling ProcessEvent() and handles any exceptions
        that occur in the process.
        If an exception is thrown in event handler, wxApp::OnExceptionInMainLoop is called.

        @param event
            Event to process.

        @return @true if the event was processed, @false if no handler was found
                 or an exception was thrown.

        @see wxWindow::HandleWindowEvent
    */
    bool SafelyProcessEvent(wxEvent& event);

    /**
        Searches the event table, executing an event handler function if an appropriate
        one is found.

        @param table
            Event table to be searched.
        @param event
            Event to be matched against an event table entry.

        @return @true if a suitable event handler function was found and
                 executed, and the function did not call wxEvent::Skip.

        @remarks This function looks through the object's event table and tries
                 to find an entry that will match the event.
                 An entry will match if:
                 @li The event type matches, and
                 @li the identifier or identifier range matches, or the event table
                     entry's identifier is zero.
                 If a suitable function is called but calls wxEvent::Skip, this
                 function will fail, and searching will continue.

        @see ProcessEvent()
    */
    virtual bool SearchEventTable(wxEventTable& table,
                                  wxEvent& event);

    /**
        Sets user-supplied client data.

        @param data
            Data to be associated with the event handler.

        @remarks Normally, any extra data the programmer wishes to associate
                 with the object should be made available by deriving a new
                 class with new data members. You must not call this method
                 and SetClientObject on the same class - only one of them.

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

        @see GetNextHandler(), SetPreviousHandler(), GetPreviousHandler(),
             wxWindow::PushEventHandler, wxWindow::PopEventHandler
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
    @class wxKeyEvent

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
    from the @ref page_keycodes.
    The translated key is, in general, the character the user expects to appear
    as the result of the key combination when typing the text into a text entry
    zone, for example.

    A few examples to clarify this (all assume that CAPS LOCK is unpressed
    and the standard US keyboard): when the @c 'A' key is pressed, the key down
    event key code is equal to @c ASCII A == 65. But the char event key code
    is @c ASCII a == 97. On the other hand, if you press both SHIFT and
    @c 'A' keys simultaneously , the key code in key down event will still be
    just @c 'A' while the char event key code parameter will now be @c 'A'
    as well.

    Although in this simple case it is clear that the correct key code could be
    found in the key down event handler by checking the value returned by
    wxKeyEvent::ShiftDown(), in general you should use @c EVT_CHAR for this as
    for non-alphanumeric keys the translation is keyboard-layout dependent and
    can only be done properly by the system itself.

    Another kind of translation is done when the control key is pressed: for
    example, for CTRL-A key press the key down event still carries the
    same key code @c 'a' as usual but the char event will have key code of 1,
    the ASCII value of this key combination.

    You may discover how the other keys on your system behave interactively by
    running the @ref page_samples_text wxWidgets sample and pressing some keys
    in any of the text controls shown in it.

    @b Tip: be sure to call @c event.Skip() for events that you don't process in
    key event function, otherwise menu shortcuts may cease to work under Windows.

    @note If a key down (@c EVT_KEY_DOWN) event is caught and the event handler
          does not call @c event.Skip() then the corresponding char event
          (@c EVT_CHAR) will not happen.
          This is by design and enables the programs that handle both types of
          events to be a bit simpler.

    @note For Windows programmers: The key and char events in wxWidgets are
          similar to but slightly different from Windows @c WM_KEYDOWN and
          @c WM_CHAR events. In particular, Alt-x combination will generate a
          char event in wxWidgets (unless it is used as an accelerator).


    @beginEventTable{wxKeyEvent}
    @event{EVT_KEY_DOWN(func)}
           Process a wxEVT_KEY_DOWN event (any key has been pressed).
    @event{EVT_KEY_UP(func)}
           Process a wxEVT_KEY_UP event (any key has been released).
    @event{EVT_CHAR(func)}
           Process a wxEVT_CHAR event.
    @endEventTable

    @see wxKeyboardState

    @library{wxcore}
    @category{events}
*/
class wxKeyEvent : public wxEvent,
                   public wxKeyboardState
{
public:
    /**
        Constructor.
        Currently, the only valid event types are @c wxEVT_CHAR and @c wxEVT_CHAR_HOOK.
    */
    wxKeyEvent(wxEventType keyEventType = wxEVT_NULL);

    /**
        Returns the virtual key code. ASCII events return normal ASCII values,
        while non-ASCII events return values such as @b WXK_LEFT for the left cursor
        key. See @ref page_keycodes for a full list of the virtual key codes.

        Note that in Unicode build, the returned value is meaningful only if the
        user entered a character that can be represented in current locale's default
        charset. You can obtain the corresponding Unicode character using GetUnicodeKey().
    */
    int GetKeyCode() const;

    //@{
    /**
        Obtains the position (in client coordinates) at which the key was pressed.
    */
    wxPoint GetPosition() const;
    void GetPosition(long* x, long* y) const;
    //@}

    /**
        Returns the raw key code for this event. This is a platform-dependent scan code
        which should only be used in advanced applications.

        @note Currently the raw key codes are not supported by all ports, use
              @ifdef_ wxHAS_RAW_KEY_CODES to determine if this feature is available.
    */
    wxUint32 GetRawKeyCode() const;

    /**
        Returns the low level key flags for this event. The flags are
        platform-dependent and should only be used in advanced applications.

        @note Currently the raw key flags are not supported by all ports, use
              @ifdef_ wxHAS_RAW_KEY_CODES to determine if this feature is available.
    */
    wxUint32 GetRawKeyFlags() const;

    /**
        Returns the Unicode character corresponding to this key event.

        This function is only available in Unicode build, i.e. when
        @c wxUSE_UNICODE is 1.
    */
    wxChar GetUnicodeKey() const;

    /**
        Returns the X position (in client coordinates) of the event.
    */
    wxCoord GetX() const;

    /**
        Returns the Y position (in client coordinates) of the event.
    */
    wxCoord GetY() const;
};



/**
    @class wxJoystickEvent

    This event class contains information about joystick events, particularly
    events received by windows.

    @beginEventTable{wxJoystickEvent}
    @style{EVT_JOY_BUTTON_DOWN(func)}
        Process a wxEVT_JOY_BUTTON_DOWN event.
    @style{EVT_JOY_BUTTON_UP(func)}
        Process a wxEVT_JOY_BUTTON_UP event.
    @style{EVT_JOY_MOVE(func)}
        Process a wxEVT_JOY_MOVE event.
    @style{EVT_JOY_ZMOVE(func)}
        Process a wxEVT_JOY_ZMOVE event.
    @style{EVT_JOYSTICK_EVENTS(func)}
        Processes all joystick events.
    @endEventTable

    @library{wxcore}
    @category{events}

    @see wxJoystick
*/
class wxJoystickEvent : public wxEvent
{
public:
    /**
        Constructor.
    */
    wxJoystickEvent(wxEventType eventType = wxEVT_NULL, int state = 0,
                    int joystick = wxJOYSTICK1,
                    int change = 0);

    /**
        Returns @true if the event was a down event from the specified button
        (or any button).

        @param button
            Can be @c wxJOY_BUTTONn where @c n is 1, 2, 3 or 4; or @c wxJOY_BUTTON_ANY to
            indicate any button down event.
    */
    bool ButtonDown(int button = wxJOY_BUTTON_ANY) const;

    /**
        Returns @true if the specified button (or any button) was in a down state.

        @param button
            Can be @c wxJOY_BUTTONn where @c n is 1, 2, 3 or 4; or @c wxJOY_BUTTON_ANY to
            indicate any button down event.
    */
    bool ButtonIsDown(int button = wxJOY_BUTTON_ANY) const;

    /**
        Returns @true if the event was an up event from the specified button
        (or any button).

        @param button
            Can be @c wxJOY_BUTTONn where @c n is 1, 2, 3 or 4; or @c wxJOY_BUTTON_ANY to
            indicate any button down event.
    */
    bool ButtonUp(int button = wxJOY_BUTTON_ANY) const;

    /**
        Returns the identifier of the button changing state.

        This is a @c wxJOY_BUTTONn identifier, where @c n is one of 1, 2, 3, 4.
    */
    int GetButtonChange() const;

    /**
        Returns the down state of the buttons.

        This is a @c wxJOY_BUTTONn identifier, where @c n is one of 1, 2, 3, 4.
    */
    int GetButtonState() const;

    /**
        Returns the identifier of the joystick generating the event - one of
        wxJOYSTICK1 and wxJOYSTICK2.
    */
    int GetJoystick() const;

    /**
        Returns the x, y position of the joystick event.
    */
    wxPoint GetPosition() const;

    /**
        Returns the z position of the joystick event.
    */
    int GetZPosition() const;

    /**
        Returns @true if this was a button up or down event
        (@e not 'is any button down?').
    */
    bool IsButton() const;

    /**
        Returns @true if this was an x, y move event.
    */
    bool IsMove() const;

    /**
        Returns @true if this was a z move event.
    */
    bool IsZMove() const;
};



/**
    @class wxScrollWinEvent

    A scroll event holds information about events sent from scrolling windows.


    @beginEventTable{wxScrollWinEvent}
    You can use the EVT_SCROLLWIN* macros for intercepting scroll window events
    from the receiving window.
    @event{EVT_SCROLLWIN(func)}
        Process all scroll events.
    @event{EVT_SCROLLWIN_TOP(func)}
        Process wxEVT_SCROLLWIN_TOP scroll-to-top events.
    @event{EVT_SCROLLWIN_BOTTOM(func)}
        Process wxEVT_SCROLLWIN_BOTTOM scroll-to-bottom events.
    @event{EVT_SCROLLWIN_LINEUP(func)}
        Process wxEVT_SCROLLWIN_LINEUP line up events.
    @event{EVT_SCROLLWIN_LINEDOWN(func)}
        Process wxEVT_SCROLLWIN_LINEDOWN line down events.
    @event{EVT_SCROLLWIN_PAGEUP(func)}
        Process wxEVT_SCROLLWIN_PAGEUP page up events.
    @event{EVT_SCROLLWIN_PAGEDOWN(func)}
        Process wxEVT_SCROLLWIN_PAGEDOWN page down events.
    @event{EVT_SCROLLWIN_THUMBTRACK(func)}
        Process wxEVT_SCROLLWIN_THUMBTRACK thumbtrack events
        (frequent events sent as the user drags the thumbtrack).
    @event{EVT_SCROLLWIN_THUMBRELEASE(func)}
        Process wxEVT_SCROLLWIN_THUMBRELEASE thumb release events.
    @endEventTable


    @library{wxcore}
    @category{events}

    @see wxScrollEvent, @ref overview_eventhandling
*/
class wxScrollWinEvent : public wxEvent
{
public:
    /**
        Constructor.
    */
    wxScrollWinEvent(wxEventType commandType = wxEVT_NULL, int pos = 0,
                     int orientation = 0);

    /**
        Returns wxHORIZONTAL or wxVERTICAL, depending on the orientation of the
        scrollbar.

        @todo wxHORIZONTAL and wxVERTICAL should go in their own enum
    */
    int GetOrientation() const;

    /**
        Returns the position of the scrollbar for the thumb track and release events.

        Note that this field can't be used for the other events, you need to query
        the window itself for the current position in that case.
    */
    int GetPosition() const;
};



/**
    @class wxSysColourChangedEvent

    This class is used for system colour change events, which are generated
    when the user changes the colour settings using the control panel.
    This is only appropriate under Windows.

    @remarks
        The default event handler for this event propagates the event to child windows,
        since Windows only sends the events to top-level windows.
        If intercepting this event for a top-level window, remember to call the base
        class handler, or to pass the event on to the window's children explicitly.

    @beginEventTable{wxSysColourChangedEvent}
    @event{EVT_SYS_COLOUR_CHANGED(func)}
        Process a wxEVT_SYS_COLOUR_CHANGED event.
    @endEventTable

    @library{wxcore}
    @category{events}

    @see @ref overview_eventhandling
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

    This event is sent just after the actual window associated with a wxWindow
    object has been created.

    Since it is derived from wxCommandEvent, the event propagates up
    the window hierarchy.

    @beginEventTable{wxWindowCreateEvent}
    @event{EVT_WINDOW_CREATE(func)}
        Process a wxEVT_CREATE event.
    @endEventTable

    @library{wxcore}
    @category{events}

    @see @ref overview_eventhandling, wxWindowDestroyEvent
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

    A paint event is sent when a window's contents needs to be repainted.

    Please notice that in general it is impossible to change the drawing of a
    standard control (such as wxButton) and so you shouldn't attempt to handle
    paint events for them as even if it might work on some platforms, this is
    inherently not portable and won't work everywhere.

    @remarks
    Note that in a paint event handler, the application must always create a
    wxPaintDC object, even if you do not use it. Otherwise, under MS Windows,
    refreshing for this and other windows will go wrong.
    For example:
    @code
    void MyWindow::OnPaint(wxPaintEvent& event)
    {
        wxPaintDC dc(this);

        DrawMyDocument(dc);
    }
    @endcode
    You can optimize painting by retrieving the rectangles that have been damaged
    and only repainting these. The rectangles are in terms of the client area,
    and are unscrolled, so you will need to do some calculations using the current
    view position to obtain logical, scrolled units.
    Here is an example of using the wxRegionIterator class:
    @code
    // Called when window needs to be repainted.
    void MyWindow::OnPaint(wxPaintEvent& event)
    {
        wxPaintDC dc(this);

        // Find Out where the window is scrolled to
        int vbX,vbY;                     // Top left corner of client
        GetViewStart(&vbX,&vbY);

        int vX,vY,vW,vH;                 // Dimensions of client area in pixels
        wxRegionIterator upd(GetUpdateRegion()); // get the update rect list

        while (upd)
        {
            vX = upd.GetX();
            vY = upd.GetY();
            vW = upd.GetW();
            vH = upd.GetH();

            // Alternatively we can do this:
            // wxRect rect(upd.GetRect());

            // Repaint this rectangle
            ...some code...

            upd ++ ;
        }
    }
    @endcode


    @beginEventTable{wxPaintEvent}
    @event{EVT_PAINT(func)}
        Process a wxEVT_PAINT event.
    @endEventTable

    @library{wxcore}
    @category{events}

    @see @ref overview_eventhandling
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

    An event being sent when a top level window is maximized. Notice that it is
    not sent when the window is restored to its original size after it had been
    maximized, only a normal wxSizeEvent is generated in this case.

    @beginEventTable{wxMaximizeEvent}
    @event{EVT_MAXIMIZE(func)}
        Process a wxEVT_MAXIMIZE event.
    @endEventTable

    @library{wxcore}
    @category{events}

    @see @ref overview_eventhandling, wxTopLevelWindow::Maximize,
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
    The possibles modes to pass to wxUpdateUIEvent::SetMode().
*/
enum wxUpdateUIMode
{
        /** Send UI update events to all windows. */
    wxUPDATE_UI_PROCESS_ALL,

        /** Send UI update events to windows that have
            the wxWS_EX_PROCESS_UI_UPDATES flag specified. */
    wxUPDATE_UI_PROCESS_SPECIFIED
};


/**
    @class wxUpdateUIEvent

    This class is used for pseudo-events which are called by wxWidgets
    to give an application the chance to update various user interface elements.

    Without update UI events, an application has to work hard to check/uncheck,
    enable/disable, show/hide, and set the text for elements such as menu items
    and toolbar buttons. The code for doing this has to be mixed up with the code
    that is invoked when an action is invoked for a menu item or button.

    With update UI events, you define an event handler to look at the state of the
    application and change UI elements accordingly. wxWidgets will call your member
    functions in idle time, so you don't have to worry where to call this code.

    In addition to being a clearer and more declarative method, it also means you don't
    have to worry whether you're updating a toolbar or menubar identifier. The same
    handler can update a menu item and toolbar button, if the identifier is the same.
    Instead of directly manipulating the menu or button, you call functions in the event
    object, such as wxUpdateUIEvent::Check. wxWidgets will determine whether such a
    call has been made, and which UI element to update.

    These events will work for popup menus as well as menubars. Just before a menu is
    popped up, wxMenu::UpdateUI is called to process any UI events for the window that
    owns the menu.

    If you find that the overhead of UI update processing is affecting your application,
    you can do one or both of the following:
    @li Call wxUpdateUIEvent::SetMode with a value of wxUPDATE_UI_PROCESS_SPECIFIED,
        and set the extra style wxWS_EX_PROCESS_UI_UPDATES for every window that should
        receive update events. No other windows will receive update events.
    @li Call wxUpdateUIEvent::SetUpdateInterval with a millisecond value to set the delay
        between updates. You may need to call wxWindow::UpdateWindowUI at critical points,
        for example when a dialog is about to be shown, in case the user sees a slight
        delay before windows are updated.

    Note that although events are sent in idle time, defining a wxIdleEvent handler
    for a window does not affect this because the events are sent from wxWindow::OnInternalIdle
    which is always called in idle time.

    wxWidgets tries to optimize update events on some platforms.
    On Windows and GTK+, events for menubar items are only sent when the menu is about
    to be shown, and not in idle time.


    @beginEventTable{wxUpdateUIEvent}
    @event{EVT_UPDATE_UI(id, func)}
        Process a wxEVT_UPDATE_UI event for the command with the given id.
    @event{EVT_UPDATE_UI_RANGE(id1, id2, func)}
        Process a wxEVT_UPDATE_UI event for any command with id included in the given range.
    @endEventTable

    @library{wxcore}
    @category{events}

    @see @ref overview_eventhandling
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
        the wxWS_EX_PROCESS_UI_UPDATES flag in @a window, the time update events
        were last sent in idle time, and the update interval, to determine whether
        events should be sent to this window now. By default this will always
        return @true because the update mode is initially wxUPDATE_UI_PROCESS_ALL
        and the interval is set to 0; so update events will be sent as often as
        possible. You can reduce the frequency that events are sent by changing the
        mode and/or setting an update interval.

        @see ResetUpdateTime(), SetUpdateInterval(), SetMode()
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
    bool GetChecked() const;

    /**
        Returns @true if the UI element should be enabled.
    */
    bool GetEnabled() const;

    /**
        Static function returning a value specifying how wxWidgets will send update
        events: to all windows, or only to those which specify that they will process
        the events.

        @see SetMode()
    */
    static wxUpdateUIMode GetMode();

    /**
        Returns @true if the application has called Check().
        For wxWidgets internal use only.
    */
    bool GetSetChecked() const;

    /**
        Returns @true if the application has called Enable().
        For wxWidgets internal use only.
    */
    bool GetSetEnabled() const;

    /**
        Returns @true if the application has called Show().
        For wxWidgets internal use only.
    */
    bool GetSetShown() const;

    /**
        Returns @true if the application has called SetText().
        For wxWidgets internal use only.
    */
    bool GetSetText() const;

    /**
        Returns @true if the UI element should be shown.
    */
    bool GetShown() const;

    /**
        Returns the text that should be set for the UI element.
    */
    wxString GetText() const;

    /**
        Returns the current interval between updates in milliseconds.
        The value -1 disables updates, 0 updates as frequently as possible.

        @see SetUpdateInterval().
    */
    static long GetUpdateInterval();

    /**
        Used internally to reset the last-updated time to the current time.

        It is assumed that update events are normally sent in idle time, so this
        is called at the end of idle processing.

        @see CanUpdate(), SetUpdateInterval(), SetMode()
    */
    static void ResetUpdateTime();

    /**
        Specify how wxWidgets will send update events: to all windows, or only to
        those which specify that they will process the events.

        @param mode
            this parameter may be one of the ::wxUpdateUIMode enumeration values.
            The default mode is wxUPDATE_UI_PROCESS_ALL.
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
        you may also need to call wxWindow::UpdateWindowUI at appropriate points
        in your application, such as when a dialog is about to be shown.
    */
    static void SetUpdateInterval(long updateInterval);

    /**
        Show or hide the UI element.
    */
    void Show(bool show);
};



/**
    @class wxClipboardTextEvent

    This class represents the events generated by a control (typically a
    wxTextCtrl but other windows can generate these events as well) when its
    content gets copied or cut to, or pasted from the clipboard.

    There are three types of corresponding events wxEVT_COMMAND_TEXT_COPY,
    wxEVT_COMMAND_TEXT_CUT and wxEVT_COMMAND_TEXT_PASTE.

    If any of these events is processed (without being skipped) by an event
    handler, the corresponding operation doesn't take place which allows to
    prevent the text from being copied from or pasted to a control. It is also
    possible to examine the clipboard contents in the PASTE event handler and
    transform it in some way before inserting in a control -- for example,
    changing its case or removing invalid characters.

    Finally notice that a CUT event is always preceded by the COPY event which
    makes it possible to only process the latter if it doesn't matter if the
    text was copied or cut.

    @note
    These events are currently only generated by wxTextCtrl under GTK+.
    They are generated by all controls under Windows.

    @beginEventTable{wxClipboardTextEvent}
    @event{EVT_TEXT_COPY(id, func)}
           Some or all of the controls content was copied to the clipboard.
    @event{EVT_TEXT_CUT(id, func)}
           Some or all of the controls content was cut (i.e. copied and
           deleted).
    @event{EVT_TEXT_PASTE(id, func)}
           Clipboard content was pasted into the control.
    @endEventTable


    @library{wxcore}
    @category{events}

    @see wxClipboard
*/
class wxClipboardTextEvent : public wxCommandEvent
{
public:
    /**
        Constructor.
    */
    wxClipboardTextEvent(wxEventType commandType = wxEVT_NULL, int id = 0);
};



/**
    @class wxMouseEvent

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

    @note Note that under Windows CE mouse enter and leave events are not natively
          supported by the system but are generated by wxWidgets itself. This has several
          drawbacks: the LEAVE_WINDOW event might be received some time after the mouse
          left the window and the state variables for it may have changed during this time.

    @note Note the difference between methods like wxMouseEvent::LeftDown and
          wxMouseEvent::LeftIsDown: the former returns @true when the event corresponds
          to the left mouse button click while the latter returns @true if the left
          mouse button is currently being pressed. For example, when the user is dragging
          the mouse you can use wxMouseEvent::LeftIsDown to test whether the left mouse
          button is (still) depressed. Also, by convention, if wxMouseEvent::LeftDown
          returns @true, wxMouseEvent::LeftIsDown will also return @true in wxWidgets
          whatever the underlying GUI behaviour is (which is platform-dependent).
          The same applies, of course, to other mouse buttons as well.


    @beginEventTable{wxMouseEvent}
    @event{EVT_LEFT_DOWN(func)}
        Process a wxEVT_LEFT_DOWN event. The handler of this event should normally
        call event.Skip() to allow the default processing to take place as otherwise
        the window under mouse wouldn't get the focus.
    @event{EVT_LEFT_UP(func)}
        Process a wxEVT_LEFT_UP event.
    @event{EVT_LEFT_DCLICK(func)}
        Process a wxEVT_LEFT_DCLICK event.
    @event{EVT_MIDDLE_DOWN(func)}
        Process a wxEVT_MIDDLE_DOWN event.
    @event{EVT_MIDDLE_UP(func)}
        Process a wxEVT_MIDDLE_UP event.
    @event{EVT_MIDDLE_DCLICK(func)}
        Process a wxEVT_MIDDLE_DCLICK event.
    @event{EVT_RIGHT_DOWN(func)}
        Process a wxEVT_RIGHT_DOWN event.
    @event{EVT_RIGHT_UP(func)}
        Process a wxEVT_RIGHT_UP event.
    @event{EVT_RIGHT_DCLICK(func)}
        Process a wxEVT_RIGHT_DCLICK event.
    @event{EVT_MOUSE_AUX1_DOWN(func)}
        Process a wxEVT_MOUSE_AUX1_DOWN event.
    @event{EVT_MOUSE_AUX1_UP(func)}
        Process a wxEVT_MOUSE_AUX1_UP event.
    @event{EVT_MOUSE_AUX1_DCLICK(func)}
        Process a wxEVT_MOUSE_AUX1_DCLICK event.
    @event{EVT_MOUSE_AUX2_DOWN(func)}
        Process a wxEVT_MOUSE_AUX2_DOWN event.
    @event{EVT_MOUSE_AUX2_UP(func)}
        Process a wxEVT_MOUSE_AUX2_UP event.
    @event{EVT_MOUSE_AUX2_DCLICK(func)}
        Process a wxEVT_MOUSE_AUX2_DCLICK event.
    @event{EVT_MOTION(func)}
        Process a wxEVT_MOTION event.
    @event{EVT_ENTER_WINDOW(func)}
        Process a wxEVT_ENTER_WINDOW event.
    @event{EVT_LEAVE_WINDOW(func)}
        Process a wxEVT_LEAVE_WINDOW event.
    @event{EVT_MOUSEWHEEL(func)}
        Process a wxEVT_MOUSEWHEEL event.
    @event{EVT_MOUSE_EVENTS(func)}
        Process all mouse events.
    @endEventTable

    @library{wxcore}
    @category{events}

    @see wxKeyEvent
*/
class wxMouseEvent : public wxEvent,
                     public wxMouseState
{
public:
    /**
        Constructor. Valid event types are:

         @li wxEVT_ENTER_WINDOW
         @li wxEVT_LEAVE_WINDOW
         @li wxEVT_LEFT_DOWN
         @li wxEVT_LEFT_UP
         @li wxEVT_LEFT_DCLICK
         @li wxEVT_MIDDLE_DOWN
         @li wxEVT_MIDDLE_UP
         @li wxEVT_MIDDLE_DCLICK
         @li wxEVT_RIGHT_DOWN
         @li wxEVT_RIGHT_UP
         @li wxEVT_RIGHT_DCLICK
         @li wxEVT_MOUSE_AUX1_DOWN
         @li wxEVT_MOUSE_AUX1_UP
         @li wxEVT_MOUSE_AUX1_DCLICK
         @li wxEVT_MOUSE_AUX2_DOWN
         @li wxEVT_MOUSE_AUX2_UP
         @li wxEVT_MOUSE_AUX2_DCLICK
         @li wxEVT_MOTION
         @li wxEVT_MOUSEWHEEL
    */
    wxMouseEvent(wxEventType mouseEventType = wxEVT_NULL);

    /**
        Returns @true if the event was a first extra button double click.
    */
    bool Aux1DClick() const;

    /**
        Returns @true if the first extra button mouse button changed to down.
    */
    bool Aux1Down() const;

    /**
        Returns @true if the first extra button mouse button is currently down,
        independent of the current event type.
    */
    bool Aux1IsDown() const;

    /**
        Returns @true if the first extra button mouse button changed to up.
    */
    bool Aux1Up() const;

    /**
        Returns @true if the event was a second extra button double click.
    */
    bool Aux2DClick() const;

    /**
        Returns @true if the second extra button mouse button changed to down.
    */
    bool Aux2Down() const;

    /**
        Returns @true if the second extra button mouse button is currently down,
        independent of the current event type.
    */
    bool Aux2IsDown() const;

    /**
        Returns @true if the second extra button mouse button changed to up.
    */
    bool Aux2Up() const;

    /**
        Returns @true if the identified mouse button is changing state.
        Valid values of @a button are:

        @li @c wxMOUSE_BTN_LEFT: check if left button was pressed
        @li @c wxMOUSE_BTN_MIDDLE: check if middle button was pressed
        @li @c wxMOUSE_BTN_RIGHT: check if right button was pressed
        @li @c wxMOUSE_BTN_AUX1: check if the first extra button was pressed
        @li @c wxMOUSE_BTN_AUX2: check if the second extra button was pressed
        @li @c wxMOUSE_BTN_ANY: check if any button was pressed

        @todo introduce wxMouseButton enum
    */
    bool Button(int button) const;

    /**
        If the argument is omitted, this returns @true if the event was a mouse
        double click event. Otherwise the argument specifies which double click event
        was generated (see Button() for the possible values).
    */
    bool ButtonDClick(int but = wxMOUSE_BTN_ANY) const;

    /**
        If the argument is omitted, this returns @true if the event was a mouse
        button down event. Otherwise the argument specifies which button-down event
        was generated (see Button() for the possible values).
    */
    bool ButtonDown(int = wxMOUSE_BTN_ANY) const;

    /**
        If the argument is omitted, this returns @true if the event was a mouse
        button up event. Otherwise the argument specifies which button-up event
        was generated (see Button() for the possible values).
    */
    bool ButtonUp(int = wxMOUSE_BTN_ANY) const;

    /**
        Returns @true if this was a dragging event (motion while a button is depressed).

        @see Moving()
    */
    bool Dragging() const;

    /**
        Returns @true if the mouse was entering the window.

        @see Leaving()
    */
    bool Entering() const;

    /**
        Returns the mouse button which generated this event or @c wxMOUSE_BTN_NONE
        if no button is involved (for mouse move, enter or leave event, for example).
        Otherwise @c wxMOUSE_BTN_LEFT is returned for the left button down, up and
        double click events, @c wxMOUSE_BTN_MIDDLE and @c wxMOUSE_BTN_RIGHT
        for the same events for the middle and the right buttons respectively.
    */
    int GetButton() const;

    /**
        Returns the number of mouse clicks for this event: 1 for a simple click, 2
        for a double-click, 3 for a triple-click and so on.

        Currently this function is implemented only in wxMac and returns -1 for the
        other platforms (you can still distinguish simple clicks from double-clicks as
        they generate different kinds of events however).

        @since 2.9.0
    */
    int GetClickCount() const;

    /**
        Returns the configured number of lines (or whatever) to be scrolled per
        wheel action. Defaults to three.
    */
    int GetLinesPerAction() const;

    /**
        Returns the logical mouse position in pixels (i.e. translated according to the
        translation set for the DC, which usually indicates that the window has been
        scrolled).
    */
    wxPoint GetLogicalPosition(const wxDC& dc) const;

    //@{
    /**
        Sets *x and *y to the position at which the event occurred.
        Returns the physical mouse position in pixels.

        Note that if the mouse event has been artificially generated from a special
        keyboard combination (e.g. under Windows when the "menu" key is pressed), the
        returned position is ::wxDefaultPosition.
    */
    wxPoint GetPosition() const;
    void GetPosition(wxCoord* x, wxCoord* y) const;
    void GetPosition(long* x, long* y) const;
    //@}

    /**
        Get wheel delta, normally 120.

        This is the threshold for action to be taken, and one such action
        (for example, scrolling one increment) should occur for each delta.
    */
    int GetWheelDelta() const;

    /**
        Get wheel rotation, positive or negative indicates direction of rotation.

        Current devices all send an event when rotation is at least +/-WheelDelta, but
        finer resolution devices can be created in the future.

        Because of this you shouldn't assume that one event is equal to 1 line, but you
        should be able to either do partial line scrolling or wait until several
        events accumulate before scrolling.
    */
    int GetWheelRotation() const;

    /**
        Returns X coordinate of the physical mouse event position.
    */
    wxCoord GetX() const;

    /**
        Returns Y coordinate of the physical mouse event position.
    */
    wxCoord GetY() const;

    /**
        Returns @true if the event was a mouse button event (not necessarily a button
        down event - that may be tested using ButtonDown()).
    */
    bool IsButton() const;

    /**
        Returns @true if the system has been setup to do page scrolling with
        the mouse wheel instead of line scrolling.
    */
    bool IsPageScroll() const;

    /**
        Returns @true if the mouse was leaving the window.

        @see Entering().
    */
    bool Leaving() const;

    /**
        Returns @true if the event was a left double click.
    */
    bool LeftDClick() const;

    /**
        Returns @true if the left mouse button changed to down.
    */
    bool LeftDown() const;

    /**
        Returns @true if the left mouse button is currently down, independent
        of the current event type.

        Please notice that it is not the same as LeftDown() which returns @true if the
        event was generated by the left mouse button being pressed. Rather, it simply
        describes the state of the left mouse button at the time when the event was
        generated (so while it will be @true for a left click event, it can also be @true
        for a right click if it happened while the left mouse button was pressed).

        This event is usually used in the mouse event handlers which process "move
        mouse" messages to determine whether the user is (still) dragging the mouse.
    */
    bool LeftIsDown() const;

    /**
        Returns @true if the left mouse button changed to up.
    */
    bool LeftUp() const;

    /**
        Returns @true if the Meta key was down at the time of the event.
    */
    bool MetaDown() const;

    /**
        Returns @true if the event was a middle double click.
    */
    bool MiddleDClick() const;

    /**
        Returns @true if the middle mouse button changed to down.
    */
    bool MiddleDown() const;

    /**
        Returns @true if the middle mouse button is currently down, independent
        of the current event type.
    */
    bool MiddleIsDown() const;

    /**
        Returns @true if the middle mouse button changed to up.
    */
    bool MiddleUp() const;

    /**
        Returns @true if this was a motion event and no mouse buttons were pressed.
        If any mouse button is held pressed, then this method returns @false and
        Dragging() returns @true.
    */
    bool Moving() const;

    /**
        Returns @true if the event was a right double click.
    */
    bool RightDClick() const;

    /**
        Returns @true if the right mouse button changed to down.
    */
    bool RightDown() const;

    /**
        Returns @true if the right mouse button is currently down, independent
        of the current event type.
    */
    bool RightIsDown() const;

    /**
        Returns @true if the right mouse button changed to up.
    */
    bool RightUp() const;
};



/**
    @class wxDropFilesEvent

    This class is used for drop files events, that is, when files have been dropped
    onto the window. This functionality is currently only available under Windows.

    The window must have previously been enabled for dropping by calling
    wxWindow::DragAcceptFiles().

    Important note: this is a separate implementation to the more general drag and drop
    implementation documented in the @ref overview_dnd. It uses the older, Windows
    message-based approach of dropping files.

    @beginEventTable{wxDropFilesEvent}
    @event{EVT_DROP_FILES(func)}
        Process a wxEVT_DROP_FILES event.
    @endEventTable

    @onlyfor{wxmsw}

    @library{wxcore}
    @category{events}

    @see @ref overview_eventhandling
*/
class wxDropFilesEvent : public wxEvent
{
public:
    /**
        Constructor.
    */
    wxDropFilesEvent(wxEventType id = 0, int noFiles = 0,
                     wxString* files = NULL);

    /**
        Returns an array of filenames.
    */
    wxString* GetFiles() const;

    /**
        Returns the number of files dropped.
    */
    int GetNumberOfFiles() const;

    /**
        Returns the position at which the files were dropped.
        Returns an array of filenames.
    */
    wxPoint GetPosition() const;
};



/**
    @class wxCommandEvent

    This event class contains information about command events, which originate
    from a variety of simple controls.

    More complex controls, such as wxTreeCtrl, have separate command event classes.

    @beginEventTable{wxCommandEvent}
    @event{EVT_COMMAND(id, event, func)}
        Process a command, supplying the window identifier, command event identifier,
        and member function.
    @event{EVT_COMMAND_RANGE(id1, id2, event, func)}
        Process a command for a range of window identifiers, supplying the minimum and
        maximum window identifiers, command event identifier, and member function.
    @event{EVT_BUTTON(id, func)}
        Process a wxEVT_COMMAND_BUTTON_CLICKED command, which is generated by a wxButton control.
    @event{EVT_CHECKBOX(id, func)}
        Process a wxEVT_COMMAND_CHECKBOX_CLICKED command, which is generated by a wxCheckBox control.
    @event{EVT_CHOICE(id, func)}
        Process a wxEVT_COMMAND_CHOICE_SELECTED command, which is generated by a wxChoice control.
    @event{EVT_COMBOBOX(id, func)}
        Process a wxEVT_COMMAND_COMBOBOX_SELECTED command, which is generated by a wxComboBox control.
    @event{EVT_LISTBOX(id, func)}
        Process a wxEVT_COMMAND_LISTBOX_SELECTED command, which is generated by a wxListBox control.
    @event{EVT_LISTBOX_DCLICK(id, func)}
        Process a wxEVT_COMMAND_LISTBOX_DOUBLECLICKED command, which is generated by a wxListBox control.
    @event{EVT_MENU(id, func)}
        Process a wxEVT_COMMAND_MENU_SELECTED command, which is generated by a menu item.
    @event{EVT_MENU_RANGE(id1, id2, func)}
        Process a wxEVT_COMMAND_MENU_RANGE command, which is generated by a range of menu items.
    @event{EVT_CONTEXT_MENU(func)}
        Process the event generated when the user has requested a popup menu to appear by
        pressing a special keyboard key (under Windows) or by right clicking the mouse.
    @event{EVT_RADIOBOX(id, func)}
        Process a wxEVT_COMMAND_RADIOBOX_SELECTED command, which is generated by a wxRadioBox control.
    @event{EVT_RADIOBUTTON(id, func)}
        Process a wxEVT_COMMAND_RADIOBUTTON_SELECTED command, which is generated by a wxRadioButton control.
    @event{EVT_SCROLLBAR(id, func)}
        Process a wxEVT_COMMAND_SCROLLBAR_UPDATED command, which is generated by a wxScrollBar
        control. This is provided for compatibility only; more specific scrollbar event macros
        should be used instead (see wxScrollEvent).
    @event{EVT_SLIDER(id, func)}
        Process a wxEVT_COMMAND_SLIDER_UPDATED command, which is generated by a wxSlider control.
    @event{EVT_TEXT(id, func)}
        Process a wxEVT_COMMAND_TEXT_UPDATED command, which is generated by a wxTextCtrl control.
    @event{EVT_TEXT_ENTER(id, func)}
        Process a wxEVT_COMMAND_TEXT_ENTER command, which is generated by a wxTextCtrl control.
        Note that you must use wxTE_PROCESS_ENTER flag when creating the control if you want it
        to generate such events.
    @event{EVT_TEXT_MAXLEN(id, func)}
        Process a wxEVT_COMMAND_TEXT_MAXLEN command, which is generated by a wxTextCtrl control
        when the user tries to enter more characters into it than the limit previously set
        with SetMaxLength().
    @event{EVT_TOGGLEBUTTON(id, func)}
        Process a wxEVT_COMMAND_TOGGLEBUTTON_CLICKED event.
    @event{EVT_TOOL(id, func)}
        Process a wxEVT_COMMAND_TOOL_CLICKED event (a synonym for wxEVT_COMMAND_MENU_SELECTED).
        Pass the id of the tool.
    @event{EVT_TOOL_RANGE(id1, id2, func)}
        Process a wxEVT_COMMAND_TOOL_CLICKED event for a range of identifiers. Pass the ids of the tools.
    @event{EVT_TOOL_RCLICKED(id, func)}
        Process a wxEVT_COMMAND_TOOL_RCLICKED event. Pass the id of the tool.
    @event{EVT_TOOL_RCLICKED_RANGE(id1, id2, func)}
        Process a wxEVT_COMMAND_TOOL_RCLICKED event for a range of ids. Pass the ids of the tools.
    @event{EVT_TOOL_ENTER(id, func)}
        Process a wxEVT_COMMAND_TOOL_ENTER event. Pass the id of the toolbar itself.
        The value of wxCommandEvent::GetSelection() is the tool id, or -1 if the mouse cursor
        has moved off a tool.
    @event{EVT_COMMAND_LEFT_CLICK(id, func)}
        Process a wxEVT_COMMAND_LEFT_CLICK command, which is generated by a control (Windows 95 and NT only).
    @event{EVT_COMMAND_LEFT_DCLICK(id, func)}
        Process a wxEVT_COMMAND_LEFT_DCLICK command, which is generated by a control (Windows 95 and NT only).
    @event{EVT_COMMAND_RIGHT_CLICK(id, func)}
        Process a wxEVT_COMMAND_RIGHT_CLICK command, which is generated by a control (Windows 95 and NT only).
    @event{EVT_COMMAND_SET_FOCUS(id, func)}
        Process a wxEVT_COMMAND_SET_FOCUS command, which is generated by a control (Windows 95 and NT only).
    @event{EVT_COMMAND_KILL_FOCUS(id, func)}
        Process a wxEVT_COMMAND_KILL_FOCUS command, which is generated by a control (Windows 95 and NT only).
    @event{EVT_COMMAND_ENTER(id, func)}
        Process a wxEVT_COMMAND_ENTER command, which is generated by a control.
    @endEventTable

    @library{wxcore}
    @category{events}
*/
class wxCommandEvent : public wxEvent
{
public:
    /**
        Constructor.
    */
    wxCommandEvent(wxEventType commandEventType = wxEVT_NULL, int id = 0);

    /**
        Returns client data pointer for a listbox or choice selection event
        (not valid for a deselection).
    */
    void* GetClientData() const;

    /**
        Returns client object pointer for a listbox or choice selection event
        (not valid for a deselection).
    */
    wxClientData* GetClientObject() const;

    /**
        Returns extra information dependant on the event objects type.

        If the event comes from a listbox selection, it is a boolean
        determining whether the event was a selection (@true) or a
        deselection (@false). A listbox deselection only occurs for
        multiple-selection boxes, and in this case the index and string values
        are indeterminate and the listbox must be examined by the application.
    */
    long GetExtraLong() const;

    /**
        Returns the integer identifier corresponding to a listbox, choice or
        radiobox selection (only if the event was a selection, not a deselection),
        or a boolean value representing the value of a checkbox.
    */
    int GetInt() const;

    /**
        Returns item index for a listbox or choice selection event (not valid for
        a deselection).
    */
    int GetSelection() const;

    /**
        Returns item string for a listbox or choice selection event. If one
        or several items have been deselected, returns the index of the first
        deselected item. If some items have been selected and others deselected
        at the same time, it will return the index of the first selected item.
    */
    wxString GetString() const;

    /**
        This method can be used with checkbox and menu events: for the checkboxes, the
        method returns @true for a selection event and @false for a deselection one.
        For the menu events, this method indicates if the menu item just has become
        checked or unchecked (and thus only makes sense for checkable menu items).

        Notice that this method can not be used with wxCheckListBox currently.
    */
    bool IsChecked() const;

    /**
        For a listbox or similar event, returns @true if it is a selection, @false
        if it is a deselection. If some items have been selected and others deselected
        at the same time, it will return @true.
    */
    bool IsSelection() const;

    /**
        Sets the client data for this event.
    */
    void SetClientData(void* clientData);

    /**
        Sets the client object for this event. The client object is not owned by the
        event object and the event object will not delete the client object in its destructor.

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

    An activate event is sent when a window or application is being activated
    or deactivated.

    @beginEventTable{wxActivateEvent}
    @event{EVT_ACTIVATE(func)}
        Process a wxEVT_ACTIVATE event.
    @event{EVT_ACTIVATE_APP(func)}
        Process a wxEVT_ACTIVATE_APP event.
    @event{EVT_HIBERNATE(func)}
        Process a hibernate event, supplying the member function. This event applies
        to wxApp only, and only on Windows SmartPhone and PocketPC.
        It is generated when the system is low on memory; the application should free
        up as much memory as possible, and restore full working state when it receives
        a wxEVT_ACTIVATE or wxEVT_ACTIVATE_APP event.
    @endEventTable


    @library{wxcore}
    @category{events}

    @see @ref overview_eventhandling, wxApp::IsActive
*/
class wxActivateEvent : public wxEvent
{
public:
    /**
        Constructor.
    */
    wxActivateEvent(wxEventType eventType = wxEVT_NULL, bool active = true,
                    int id = 0);

    /**
        Returns @true if the application or window is being activated, @false otherwise.
    */
    bool GetActive() const;
};



/**
    @class wxContextMenuEvent

    This class is used for context menu events, sent to give
    the application a chance to show a context (popup) menu.

    Note that if wxContextMenuEvent::GetPosition returns wxDefaultPosition, this
    means that the event originated from a keyboard context button event, and you
    should compute a suitable position yourself, for example by calling wxGetMousePosition().

    When a keyboard context menu button is pressed on Windows, a right-click event
    with default position is sent first, and if this event is not processed, the
    context menu event is sent. So if you process mouse events and you find your
    context menu event handler is not being called, you could call wxEvent::Skip()
    for mouse right-down events.

    @beginEventTable{wxContextMenuEvent}
    @event{EVT_CONTEXT_MENU(func)}
        A right click (or other context menu command depending on platform) has been detected.
    @endEventTable


    @library{wxcore}
    @category{events}

    @see wxCommandEvent, @ref overview_eventhandling
*/
class wxContextMenuEvent : public wxCommandEvent
{
public:
    /**
        Constructor.
    */
    wxContextMenuEvent(wxEventType id = wxEVT_NULL, int id = 0,
                       const wxPoint& pos = wxDefaultPosition);

    /**
        Returns the position in screen coordinates at which the menu should be shown.
        Use wxWindow::ScreenToClient to convert to client coordinates.

        You can also omit a position from  wxWindow::PopupMenu in order to use
        the current mouse pointer position.

        If the event originated from a keyboard event, the value returned from this
        function will be wxDefaultPosition.
    */
    const wxPoint& GetPosition() const;

    /**
        Sets the position at which the menu should be shown.
    */
    void SetPosition(const wxPoint& point);
};



/**
    @class wxEraseEvent

    An erase event is sent when a window's background needs to be repainted.

    On some platforms, such as GTK+, this event is simulated (simply generated just
    before the paint event) and may cause flicker. It is therefore recommended that
    you set the text background colour explicitly in order to prevent flicker.
    The default background colour under GTK+ is grey.

    To intercept this event, use the EVT_ERASE_BACKGROUND macro in an event table
    definition.

    You must call wxEraseEvent::GetDC and use the returned device context if it is
    non-@NULL. If it is @NULL, create your own temporary wxClientDC object.

    @remarks
        Use the device context returned by GetDC to draw on, don't create
        a wxPaintDC in the event handler.

    @beginEventTable{wxEraseEvent}
    @event{EVT_ERASE_BACKGROUND(func)}
        Process a wxEVT_ERASE_BACKGROUND event.
    @endEventTable

    @library{wxcore}
    @category{events}

    @see @ref overview_eventhandling
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
    wxDC* GetDC() const;
};



/**
    @class wxFocusEvent

    A focus event is sent when a window's focus changes. The window losing focus
    receives a "kill focus" event while the window gaining it gets a "set focus" one.

    Notice that the set focus event happens both when the user gives focus to the
    window (whether using the mouse or keyboard) and when it is done from the
    program itself using wxWindow::SetFocus.

    @beginEventTable{wxFocusEvent}
    @event{EVT_SET_FOCUS(func)}
        Process a wxEVT_SET_FOCUS event.
    @event{EVT_KILL_FOCUS(func)}
        Process a wxEVT_KILL_FOCUS event.
    @endEventTable

    @library{wxcore}
    @category{events}

    @see @ref overview_eventhandling
*/
class wxFocusEvent : public wxEvent
{
public:
    /**
        Constructor.
    */
    wxFocusEvent(wxEventType eventType = wxEVT_NULL, int id = 0);

    /**
        Returns the window associated with this event, that is the window which had the
        focus before for the @c wxEVT_SET_FOCUS event and the window which is
        going to receive focus for the @c wxEVT_KILL_FOCUS one.

        Warning: the window pointer may be @NULL!
    */
    wxWindow *GetWindow() const;
};



/**
    @class wxChildFocusEvent

    A child focus event is sent to a (parent-)window when one of its child windows
    gains focus, so that the window could restore the focus back to its corresponding
    child if it loses it now and regains later.

    Notice that child window is the direct child of the window receiving event.
    Use wxWindow::FindFocus() to retreive the window which is actually getting focus.

    @beginEventTable{wxChildFocusEvent}
    @event{EVT_CHILD_FOCUS(func)}
        Process a wxEVT_CHILD_FOCUS event.
    @endEventTable

    @library{wxcore}
    @category{events}

    @see @ref overview_eventhandling
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
    wxWindow *GetWindow() const;
};



/**
    @class wxMouseCaptureLostEvent

    An mouse capture lost event is sent to a window that obtained mouse capture,
    which was subsequently loss due to "external" event, for example when a dialog
    box is shown or if another application captures the mouse.

    If this happens, this event is sent to all windows that are on capture stack
    (i.e. called CaptureMouse, but didn't call ReleaseMouse yet). The event is
    not sent if the capture changes because of a call to CaptureMouse or
    ReleaseMouse.

    This event is currently emitted under Windows only.

    @beginEventTable{wxMouseCaptureLostEvent}
    @event{EVT_MOUSE_CAPTURE_LOST(func)}
        Process a wxEVT_MOUSE_CAPTURE_LOST event.
    @endEventTable

    @onlyfor{wxmsw}

    @library{wxcore}
    @category{events}

    @see wxMouseCaptureChangedEvent, @ref overview_eventhandling,
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

    This class is not used by the event handlers by itself, but is a base class
    for other event classes (such as wxBookCtrlEvent).

    It (or an object of a derived class) is sent when the controls state is being
    changed and allows the program to wxNotifyEvent::Veto() this change if it wants
    to prevent it from happening.

    @library{wxcore}
    @category{events}

    @see wxBookCtrlEvent
*/
class wxNotifyEvent : public wxCommandEvent
{
public:
    /**
        Constructor (used internally by wxWidgets only).
    */
    wxNotifyEvent(wxEventType eventType = wxEVT_NULL, int id = 0);

    /**
        This is the opposite of Veto(): it explicitly allows the event to be processed.
        For most events it is not necessary to call this method as the events are allowed
        anyhow but some are forbidden by default (this will be mentioned in the corresponding
        event description).
    */
    void Allow();

    /**
        Returns @true if the change is allowed (Veto() hasn't been called) or @false
        otherwise (if it was).
    */
    bool IsAllowed() const;

    /**
        Prevents the change announced by this event from happening.

        It is in general a good idea to notify the user about the reasons for vetoing
        the change because otherwise the applications behaviour (which just refuses to
        do what the user wants) might be quite surprising.
    */
    void Veto();
};




/**
    Indicates how a wxHelpEvent was generated.
*/
enum wxHelpEventOrigin
{
    wxHE_ORIGIN_UNKNOWN = -1,    /**< unrecognized event source. */
    wxHE_ORIGIN_KEYBOARD,        /**< event generated from F1 key press. */

    /** event generated by wxContextHelp or from the [?] button on
        the title bar (Windows). */
    wxHE_ORIGIN_HELPBUTTON
};

/**
    @class wxHelpEvent

    A help event is sent when the user has requested context-sensitive help.
    This can either be caused by the application requesting context-sensitive help mode
    via wxContextHelp, or (on MS Windows) by the system generating a WM_HELP message when
    the user pressed F1 or clicked on the query button in a dialog caption.

    A help event is sent to the window that the user clicked on, and is propagated
    up the window hierarchy until the event is processed or there are no more event
    handlers.

    The application should call wxEvent::GetId to check the identity of the
    clicked-on window, and then either show some suitable help or call wxEvent::Skip()
    if the identifier is unrecognised.

    Calling Skip is important because it allows wxWidgets to generate further
    events for ancestors of the clicked-on window. Otherwise it would be impossible to
    show help for container windows, since processing would stop after the first window
    found.

    @beginEventTable{wxHelpEvent}
    @event{EVT_HELP(id, func)}
        Process a wxEVT_HELP event.
    @event{EVT_HELP_RANGE(id1, id2, func)}
        Process a wxEVT_HELP event for a range of ids.
    @endEventTable

    @library{wxcore}
    @category{events}

    @see wxContextHelp, wxDialog, @ref overview_eventhandling
*/
class wxHelpEvent : public wxCommandEvent
{
public:
    /**
        Constructor.
    */
    wxHelpEvent(wxEventType type = wxEVT_NULL,
                wxWindowID winid = 0,
                const wxPoint& pt = wxDefaultPosition,
                wxHelpEventOrigin origin = wxHE_ORIGIN_UNKNOWN);

    /**
        Returns the origin of the help event which is one of the ::wxHelpEventOrigin
        values.

        The application may handle events generated using the keyboard or mouse
        differently, e.g. by using wxGetMousePosition() for the mouse events.

        @see SetOrigin()
    */
    wxHelpEvent::Origin GetOrigin() const;

    /**
        Returns the left-click position of the mouse, in screen coordinates.
        This allows the application to position the help appropriately.
    */
    const wxPoint& GetPosition() const;

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

    A scroll event holds information about events sent from stand-alone
    scrollbars (see wxScrollBar) and sliders (see wxSlider).

    Note that scrolled windows send the wxScrollWinEvent which does not derive from
    wxCommandEvent, but from wxEvent directly - don't confuse these two kinds of
    events and use the event table macros mentioned below only for the scrollbar-like
    controls.

    @section scrollevent_diff The difference between EVT_SCROLL_THUMBRELEASE and EVT_SCROLL_CHANGED

    The EVT_SCROLL_THUMBRELEASE event is only emitted when actually dragging the thumb
    using the mouse and releasing it (This EVT_SCROLL_THUMBRELEASE event is also followed
    by an EVT_SCROLL_CHANGED event).

    The EVT_SCROLL_CHANGED event also occurs when using the keyboard to change the thumb
    position, and when clicking next to the thumb (In all these cases the EVT_SCROLL_THUMBRELEASE
    event does not happen).

    In short, the EVT_SCROLL_CHANGED event is triggered when scrolling/ moving has finished
    independently of the way it had started. Please see the widgets sample ("Slider" page)
    to see the difference between EVT_SCROLL_THUMBRELEASE and EVT_SCROLL_CHANGED in action.

    @remarks
    Note that unless specifying a scroll control identifier, you will need to test for scrollbar
    orientation with wxScrollEvent::GetOrientation, since horizontal and vertical scroll events
    are processed using the same event handler.

    @beginEventTable{wxScrollEvent}
    You can use EVT_COMMAND_SCROLL... macros with window IDs for when intercepting
    scroll events from controls, or EVT_SCROLL... macros without window IDs for
    intercepting scroll events from the receiving window -- except for this, the
    macros behave exactly the same.
    @event{EVT_SCROLL(func)}
        Process all scroll events.
    @event{EVT_SCROLL_TOP(func)}
        Process wxEVT_SCROLL_TOP scroll-to-top events (minimum position).
    @event{EVT_SCROLL_BOTTOM(func)}
        Process wxEVT_SCROLL_BOTTOM scroll-to-bottom events (maximum position).
    @event{EVT_SCROLL_LINEUP(func)}
        Process wxEVT_SCROLL_LINEUP line up events.
    @event{EVT_SCROLL_LINEDOWN(func)}
        Process wxEVT_SCROLL_LINEDOWN line down events.
    @event{EVT_SCROLL_PAGEUP(func)}
        Process wxEVT_SCROLL_PAGEUP page up events.
    @event{EVT_SCROLL_PAGEDOWN(func)}
        Process wxEVT_SCROLL_PAGEDOWN page down events.
    @event{EVT_SCROLL_THUMBTRACK(func)}
        Process wxEVT_SCROLL_THUMBTRACK thumbtrack events (frequent events sent as the
        user drags the thumbtrack).
    @event{EVT_SCROLL_THUMBRELEASE(func)}
        Process wxEVT_SCROLL_THUMBRELEASE thumb release events.
    @event{EVT_SCROLL_CHANGED(func)}
        Process wxEVT_SCROLL_CHANGED end of scrolling events (MSW only).
    @event{EVT_COMMAND_SCROLL(id, func)}
        Process all scroll events.
    @event{EVT_COMMAND_SCROLL_TOP(id, func)}
        Process wxEVT_SCROLL_TOP scroll-to-top events (minimum position).
    @event{EVT_COMMAND_SCROLL_BOTTOM(id, func)}
        Process wxEVT_SCROLL_BOTTOM scroll-to-bottom events (maximum position).
    @event{EVT_COMMAND_SCROLL_LINEUP(id, func)}
        Process wxEVT_SCROLL_LINEUP line up events.
    @event{EVT_COMMAND_SCROLL_LINEDOWN(id, func)}
        Process wxEVT_SCROLL_LINEDOWN line down events.
    @event{EVT_COMMAND_SCROLL_PAGEUP(id, func)}
        Process wxEVT_SCROLL_PAGEUP page up events.
    @event{EVT_COMMAND_SCROLL_PAGEDOWN(id, func)}
        Process wxEVT_SCROLL_PAGEDOWN page down events.
    @event{EVT_COMMAND_SCROLL_THUMBTRACK(id, func)}
        Process wxEVT_SCROLL_THUMBTRACK thumbtrack events (frequent events sent
        as the user drags the thumbtrack).
    @event{EVT_COMMAND_SCROLL_THUMBRELEASE(func)}
        Process wxEVT_SCROLL_THUMBRELEASE thumb release events.
    @event{EVT_COMMAND_SCROLL_CHANGED(func)}
        Process wxEVT_SCROLL_CHANGED end of scrolling events (MSW only).
    @endEventTable

    @library{wxcore}
    @category{events}

    @see wxScrollBar, wxSlider, wxSpinButton, wxScrollWinEvent, @ref overview_eventhandling
*/
class wxScrollEvent : public wxCommandEvent
{
public:
    /**
        Constructor.
    */
    wxScrollEvent(wxEventType commandType = wxEVT_NULL, int id = 0, int pos = 0,
                  int orientation = 0);

    /**
        Returns wxHORIZONTAL or wxVERTICAL, depending on the orientation of the
        scrollbar.
    */
    int GetOrientation() const;

    /**
        Returns the position of the scrollbar.
    */
    int GetPosition() const;
};

/**
    See wxIdleEvent::SetMode() for more info.
*/
enum wxIdleMode
{
        /** Send idle events to all windows */
    wxIDLE_PROCESS_ALL,

        /** Send idle events to windows that have the wxWS_EX_PROCESS_IDLE flag specified */
    wxIDLE_PROCESS_SPECIFIED
};


/**
    @class wxIdleEvent

    This class is used for idle events, which are generated when the system becomes
    idle. Note that, unless you do something specifically, the idle events are not
    sent if the system remains idle once it has become it, e.g. only a single idle
    event will be generated until something else resulting in more normal events
    happens and only then is the next idle event sent again.

    If you need to ensure a continuous stream of idle events, you can either use
    wxIdleEvent::RequestMore method in your handler or call wxWakeUpIdle() periodically
    (for example from a timer event handler), but note that both of these approaches
    (and especially the first one) increase the system load and so should be avoided
    if possible.

    By default, idle events are sent to all windows (and also wxApp, as usual).
    If this is causing a significant overhead in your application, you can call
    wxIdleEvent::SetMode with the value wxIDLE_PROCESS_SPECIFIED, and set the
    wxWS_EX_PROCESS_IDLE extra window style for every window which should receive
    idle events.

    @beginEventTable{wxIdleEvent}
    @event{EVT_IDLE(func)}
        Process a wxEVT_IDLE event.
    @endEventTable

    @library{wxbase}
    @category{events}

    @see @ref overview_eventhandling, wxUpdateUIEvent, wxWindow::OnInternalIdle
*/
class wxIdleEvent : public wxEvent
{
public:
    /**
        Constructor.
    */
    wxIdleEvent();

    /**
        Returns @true if it is appropriate to send idle events to this window.

        This function looks at the mode used (see wxIdleEvent::SetMode),
        and the wxWS_EX_PROCESS_IDLE style in @a window to determine whether idle
        events should be sent to this window now.

        By default this will always return @true because the update mode is initially
        wxIDLE_PROCESS_ALL. You can change the mode to only send idle events to
        windows with the wxWS_EX_PROCESS_IDLE extra window style set.

        @see SetMode()
    */
    static bool CanSend(wxWindow* window);

    /**
        Static function returning a value specifying how wxWidgets will send idle
        events: to all windows, or only to those which specify that they
        will process the events.

        @see SetMode().
    */
    static wxIdleMode GetMode();

    /**
        Returns @true if the OnIdle function processing this event requested more
        processing time.

        @see RequestMore()
    */
    bool MoreRequested() const;

    /**
        Tells wxWidgets that more processing is required.

        This function can be called by an OnIdle handler for a window or window event
        handler to indicate that wxApp::OnIdle should forward the OnIdle event once
        more to the application windows.

        If no window calls this function during OnIdle, then the application will
        remain in a passive event loop (not calling OnIdle) until a new event is
        posted to the application by the windowing system.

        @see MoreRequested()
    */
    void RequestMore(bool needMore = true);

    /**
        Static function for specifying how wxWidgets will send idle events: to
        all windows, or only to those which specify that they will process the events.

        @param mode
            Can be one of the ::wxIdleMode values.
            The default is wxIDLE_PROCESS_ALL.
    */
    static void SetMode(wxIdleMode mode);
};



/**
    @class wxInitDialogEvent

    A wxInitDialogEvent is sent as a dialog or panel is being initialised.
    Handlers for this event can transfer data to the window.

    The default handler calls wxWindow::TransferDataToWindow.

    @beginEventTable{wxInitDialogEvent}
    @event{EVT_INIT_DIALOG(func)}
        Process a wxEVT_INIT_DIALOG event.
    @endEventTable

    @library{wxcore}
    @category{events}

    @see @ref overview_eventhandling
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

    This event is sent from the wxWindow destructor wxWindow::~wxWindow() when a
    window is destroyed.

    When a class derived from wxWindow is destroyed its destructor will have
    already run by the time this event is sent. Therefore this event will not
    usually be received at all.

    To receive this event wxEvtHandler::Connect() must be used (using an event
    table macro will not work). Since it is received after the destructor has run,
    an object should not handle its own wxWindowDestroyEvent, but it can be used
    to get notification of the destruction of another window.

    @library{wxcore}
    @category{events}

    @see @ref overview_eventhandling, wxWindowCreateEvent
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
    The possible flag values for a wxNavigationKeyEvent.
*/
enum wxNavigationKeyEventFlags
{
    wxNKEF_IS_BACKWARD = 0x0000,
    wxNKEF_IS_FORWARD = 0x0001,
    wxNKEF_WINCHANGE = 0x0002,
    wxNKEF_FROMTAB = 0x0004
};


/**
    @class wxNavigationKeyEvent

    This event class contains information about navigation events,
    generated by navigation keys such as tab and page down.

    This event is mainly used by wxWidgets implementations.
    A wxNavigationKeyEvent handler is automatically provided by wxWidgets
    when you make a class into a control container with the macro
    WX_DECLARE_CONTROL_CONTAINER.

    @beginEventTable{wxNavigationKeyEvent}
    @event{EVT_NAVIGATION_KEY(func)}
        Process a navigation key event.
    @endEventTable

    @library{wxcore}
    @category{events}

    @see wxWindow::Navigate, wxWindow::NavigateIn
*/
class wxNavigationKeyEvent : public wxEvent
{
public:
    wxNavigationKeyEvent();
    wxNavigationKeyEvent(const wxNavigationKeyEvent& event);

    /**
        Returns the child that has the focus, or @NULL.
    */
    wxWindow* GetCurrentFocus() const;

    /**
        Returns @true if the navigation was in the forward direction.
    */
    bool GetDirection() const;

    /**
        Returns @true if the navigation event was from a tab key.
        This is required for proper navigation over radio buttons.
    */
    bool IsFromTab() const;

    /**
        Returns @true if the navigation event represents a window change
        (for example, from Ctrl-Page Down in a notebook).
    */
    bool IsWindowChange() const;

    /**
        Sets the current focus window member.
    */
    void SetCurrentFocus(wxWindow* currentFocus);

    /**
        Sets the direction to forward if @a direction is @true, or backward
        if @false.
    */
    void SetDirection(bool direction);

    /**
        Sets the flags for this event.
        The @a flags can be a combination of the ::wxNavigationKeyEventFlags values.
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

    An mouse capture changed event is sent to a window that loses its
    mouse capture. This is called even if wxWindow::ReleaseCapture
    was called by the application code. Handling this event allows
    an application to cater for unexpected capture releases which
    might otherwise confuse mouse handling code.

    @onlyfor{wxmsw}

    @beginEventTable{wxMouseCaptureChangedEvent}
    @event{EVT_MOUSE_CAPTURE_CHANGED(func)}
        Process a wxEVT_MOUSE_CAPTURE_CHANGED event.
    @endEventTable

    @library{wxcore}
    @category{events}

    @see wxMouseCaptureLostEvent, @ref overview_eventhandling,
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
        Returns the window that gained the capture, or @NULL if it was a
        non-wxWidgets window.
    */
    wxWindow* GetCapturedWindow() const;
};



/**
    @class wxCloseEvent

    This event class contains information about window and session close events.

    The handler function for EVT_CLOSE is called when the user has tried to close a
    a frame or dialog box using the window manager (X) or system menu (Windows).
    It can also be invoked by the application itself programmatically, for example by
    calling the wxWindow::Close function.

    You should check whether the application is forcing the deletion of the window
    using wxCloseEvent::CanVeto. If this is @false, you @e must destroy the window
    using wxWindow::Destroy.

    If the return value is @true, it is up to you whether you respond by destroying
    the window.

    If you don't destroy the window, you should call wxCloseEvent::Veto to
    let the calling code know that you did not destroy the window.
    This allows the wxWindow::Close function to return @true or @false depending
    on whether the close instruction was honoured or not.

    The EVT_END_SESSION event is slightly different as it is sent by the system
    when the user session is ending (e.g. because of log out or shutdown) and
    so all windows are being forcefully closed. At least under MSW, after the
    handler for this event is executed the program is simply killed by the
    system. Because of this, the default handler for this event provided by
    wxWidgets calls all the usual cleanup code (including wxApp::OnExit()) so
    that it could still be executed and exit()s the process itself, without
    waiting for being killed. If this behaviour is for some reason undesirable,
    make sure that you define a handler for this event in your wxApp-derived
    class and do not call @c event.Skip() in it (but be aware that the system
    will still kill your application).

    @beginEventTable{wxCloseEvent}
    @event{EVT_CLOSE(func)}
        Process a close event, supplying the member function.
        This event applies to wxFrame and wxDialog classes.
    @event{EVT_QUERY_END_SESSION(func)}
        Process a query end session event, supplying the member function.
        This event can be handled in wxApp-derived class only.
    @event{EVT_END_SESSION(func)}
        Process an end session event, supplying the member function.
        This event can be handled in wxApp-derived class only.
    @endEventTable

    @library{wxcore}
    @category{events}

    @see wxWindow::Close, @ref overview_windowdeletion
*/
class wxCloseEvent : public wxEvent
{
public:
    /**
        Constructor.
    */
    wxCloseEvent(wxEventType commandEventType = wxEVT_NULL, int id = 0);

    /**
        Returns @true if you can veto a system shutdown or a window close event.
        Vetoing a window close event is not possible if the calling code wishes to
        force the application to exit, and so this function must be called to check this.
    */
    bool CanVeto() const;

    /**
        Returns @true if the user is just logging off or @false if the system is
        shutting down. This method can only be called for end session and query end
        session events, it doesn't make sense for close window event.
    */
    bool GetLoggingOff() const;

    /**
        Sets the 'can veto' flag.
    */
    void SetCanVeto(bool canVeto);

    /**
        Sets the 'logging off' flag.
    */
    void SetLoggingOff(bool loggingOff);

    /**
        Call this from your event handler to veto a system shutdown or to signal
        to the calling application that a window close did not happen.

        You can only veto a shutdown if CanVeto() returns @true.
    */
    void Veto(bool veto = true);
};



/**
    @class wxMenuEvent

    This class is used for a variety of menu-related events. Note that
    these do not include menu command events, which are
    handled using wxCommandEvent objects.

    The default handler for wxEVT_MENU_HIGHLIGHT displays help
    text in the first field of the status bar.

    @beginEventTable{wxMenuEvent}
    @event{EVT_MENU_OPEN(func)}
        A menu is about to be opened. On Windows, this is only sent once for each
        navigation of the menubar (up until all menus have closed).
    @event{EVT_MENU_CLOSE(func)}
        A menu has been just closed.
    @event{EVT_MENU_HIGHLIGHT(id, func)}
        The menu item with the specified id has been highlighted: used to show
        help prompts in the status bar by wxFrame
    @event{EVT_MENU_HIGHLIGHT_ALL(func)}
        A menu item has been highlighted, i.e. the currently selected menu item has changed.
    @endEventTable

    @library{wxcore}
    @category{events}

    @see wxCommandEvent, @ref overview_eventhandling
*/
class wxMenuEvent : public wxEvent
{
public:
    /**
        Constructor.
    */
    wxMenuEvent(wxEventType id = wxEVT_NULL, int id = 0, wxMenu* menu = NULL);

    /**
        Returns the menu which is being opened or closed. This method should only be
        used with the @c OPEN and @c CLOSE events and even for them the
        returned pointer may be @NULL in some ports.
    */
    wxMenu* GetMenu() const;

    /**
        Returns the menu identifier associated with the event.
        This method should be only used with the @c HIGHLIGHT events.
    */
    int GetMenuId() const;

    /**
        Returns @true if the menu which is being opened or closed is a popup menu,
        @false if it is a normal one.

        This method should only be used with the @c OPEN and @c CLOSE events.
    */
    bool IsPopup() const;
};

/**
    @class wxShowEvent

    An event being sent when the window is shown or hidden.

    Currently only wxMSW, wxGTK and wxOS2 generate such events.

    @onlyfor{wxmsw,wxgtk,wxos2}

    @beginEventTable{wxShowEvent}
    @event{EVT_SHOW(func)}
        Process a wxEVT_SHOW event.
    @endEventTable

    @library{wxcore}
    @category{events}

    @see @ref overview_eventhandling, wxWindow::Show,
         wxWindow::IsShown
*/

class wxShowEvent : public wxEvent
{
public:
    /**
        Constructor.
    */
    wxShowEvent(int winid = 0, bool show = false);

    /**
        Set whether the windows was shown or hidden.
    */
    void SetShow(bool show);

    /**
        Return @true if the window has been shown, @false if it has been
        hidden.
    */
    bool IsShown() const;

    /**
        @deprecated This function is deprecated in favour of IsShown().
    */
    bool GetShow() const;
};



/**
    @class wxIconizeEvent

    An event being sent when the frame is iconized (minimized) or restored.

    Currently only wxMSW and wxGTK generate such events.

    @onlyfor{wxmsw,wxgtk}

    @beginEventTable{wxIconizeEvent}
    @event{EVT_ICONIZE(func)}
        Process a wxEVT_ICONIZE event.
    @endEventTable

    @library{wxcore}
    @category{events}

    @see @ref overview_eventhandling, wxTopLevelWindow::Iconize,
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
    bool IsIconized() const;

    /**
        @deprecated This function is deprecated in favour of IsIconized().
    */
    bool Iconized() const;
};



/**
    @class wxMoveEvent

    A move event holds information about move change events.

    @beginEventTable{wxMoveEvent}
    @event{EVT_MOVE(func)}
        Process a wxEVT_MOVE event, which is generated when a window is moved.
    @event{EVT_MOVE_START(func)}
        Process a wxEVT_MOVE_START event, which is generated when the user starts
        to move or size a window. wxMSW only.
    @event{EVT_MOVE_END(func)}
        Process a wxEVT_MOVE_END event, which is generated when the user stops
        moving or sizing a window. wxMSW only.
    @endEventTable

    @library{wxcore}
    @category{events}

    @see wxPoint, @ref overview_eventhandling
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
    wxPoint GetPosition() const;
};


/**
    @class wxSizeEvent

    A size event holds information about size change events.

    The EVT_SIZE handler function will be called when the window has been resized.

    You may wish to use this for frames to resize their child windows as appropriate.

    Note that the size passed is of the whole window: call wxWindow::GetClientSize
    for the area which may be used by the application.

    When a window is resized, usually only a small part of the window is damaged
    and you  may only need to repaint that area. However, if your drawing depends on the
    size of the window, you may need to clear the DC explicitly and repaint the whole window.
    In which case, you may need to call wxWindow::Refresh to invalidate the entire window.

    @beginEventTable{wxSizeEvent}
    @event{EVT_SIZE(func)}
        Process a wxEVT_SIZE event.
    @endEventTable

    @library{wxcore}
    @category{events}

    @see wxSize, @ref overview_eventhandling
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
    wxSize GetSize() const;
};



/**
    @class wxSetCursorEvent

    A wxSetCursorEvent is generated when the mouse cursor is about to be set as a
    result of mouse motion.

    This event gives the application the chance to perform specific mouse cursor
    processing based on the current position of the mouse within the window.
    Use wxSetCursorEvent::SetCursor to specify the cursor you want to be displayed.

    @beginEventTable{wxSetCursorEvent}
    @event{EVT_SET_CURSOR(func)}
        Process a wxEVT_SET_CURSOR event.
    @endEventTable

    @library{wxcore}
    @category{events}

    @see ::wxSetCursor, wxWindow::wxSetCursor
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
    const wxCursor& GetCursor() const;

    /**
        Returns the X coordinate of the mouse in client coordinates.
    */
    wxCoord GetX() const;

    /**
        Returns the Y coordinate of the mouse in client coordinates.
    */
    wxCoord GetY() const;

    /**
        Returns @true if the cursor specified by this event is a valid cursor.

        @remarks You cannot specify wxNullCursor with this event, as it is not
                 considered a valid cursor.
    */
    bool HasCursor() const;

    /**
        Sets the cursor associated with this event.
    */
    void SetCursor(const wxCursor& cursor);
};



// ============================================================================
// Global functions/macros
// ============================================================================

/** @ingroup group_funcmacro_misc */
//@{

/**
    In a GUI application, this function posts @a event to the specified @e dest
    object using wxEvtHandler::AddPendingEvent().

    Otherwise, it dispatches @a event immediately using
    wxEvtHandler::ProcessEvent(). See the respective documentation for details
    (and caveats). Because of limitation of wxEvtHandler::AddPendingEvent()
    this function is not thread-safe for event objects having wxString fields,
    use wxQueueEvent() instead.

    @header{wx/event.h}
*/
void wxPostEvent(wxEvtHandler* dest, const wxEvent& event);

/**
    Queue an event for processing on the given object.

    This is a wrapper around wxEvtHandler::QueueEvent(), see its documentation
    for more details.

    @header{wx/event.h}

    @param dest
        The object to queue the event on, can't be @c NULL.
    @param event
        The heap-allocated and non-@c NULL event to queue, the function takes
        ownership of it.
 */
void wxQueueEvent(wxEvtHandler* dest, wxEvent *event);

//@}

