"""Decorator classes for documentation and shell scripting.
"""

__author__ = "Patrick K. O'Brien <pobrien@orbtech.com>"
__cvsid__ = "$Id$"
__revision__ = "$Revision$"[11:-2]


# These are not the real wxPython classes. These are Python versions
# for documentation purposes. They are also used to apply docstrings
# to the real wxPython classes, which are SWIG-generated wrappers for
# C-language classes.


import Parameters as wx


class Object:
    """Base class for all other wxPython classes."""
    
    def __init__(self):
        """Create a Object instance."""
        pass

    def Destroy(self):
        """Destroy the Object instance."""
        pass

    def GetClassName(self):
        """Return the name of the class."""
        pass


class EvtHandler(Object):
    """Base class that can handle events from the windowing system.

    If the handler is part of a chain, the destructor will unlink
    itself and restore the previous and next handlers so that they
    point to each other."""

    def __init__(self):
        """Create a EvtHandler instance."""
        pass

    def AddPendingEvent(self, event):
        """Post an event to be processed later.

        event is an Event instance to add to process queue.

        The difference between sending an event (using the
        ProcessEvent method) and posting it is that in the first case
        the event is processed before the function returns, while in
        the second case, the function returns immediately and the
        event will be processed sometime later (usually during the
        next event loop iteration).

        A copy of event is made by the function, so the original can
        be deleted as soon as function returns (it is common that the
        original is created on the stack).  This requires that the
        Event::Clone method be implemented by event so that it can
        be duplicated and stored until it gets processed.

        This is also the method to call for inter-thread
        communication.  It will post events safely between different
        threads which means that this method is thread-safe by using
        critical sections where needed.  In a multi-threaded program,
        you often need to inform the main GUI thread about the status
        of other working threads and such notification should be done
        using this method.

        This method automatically wakes up idle handling if the
        underlying window system is currently idle and thus would not
        send any idle events.  (Waking up idle handling is done
        calling WakeUpIdle.)"""
        pass

    def Connect(self, id, lastId, eventType, func):
        """Connects the given function dynamically with the event
        handler, id and event type.  This is an alternative to the use
        of static event tables.

        id is the identifier (or first of the identifier range) to be
        associated with the event handler function.

        lastId is the second part of the identifier range to be
        associated with the event handler function.

        eventType is the event type to be associated with this event
        handler.

        function is the event handler function.

        userData is data to be associated with the event table entry."""
        pass

    def Disconnect(self, id, lastId=-1, eventType=wx.EVT_NULL):
        """Disconnects the given function dynamically from the event
        handler, using the specified parameters as search criteria and
        returning True if a matching function has been found and
        removed.  This method can only disconnect functions which have
        been added using the EvtHandler.Connect method. There is no
        way to disconnect functions connected using the (static) event
        tables.

        id is the identifier (or first of the identifier range) to be
        associated with the event handler function.

        lastId is the second part of the identifier range to be
        associated with the event handler function.

        eventType is the event type to be associated with this event
        handler.

        function is the event handler function.

        userData is data to be associated with the event table entry."""
        pass

    def GetEvtHandlerEnabled(self):
        """Return True if the event handler is enabled, False
        otherwise."""
        pass

    def GetNextHandler(self):
        """Return the next handler in the chain."""
        pass

    def GetPreviousHandler(self):
        """Return the previous handler in the chain."""
        pass

    def ProcessEvent(self, event):
        """Processes an event, searching event tables and calling zero
        or more suitable event handler function(s).  Return True if a
        suitable event handler function was found and executed, and
        the function did not call Event.Skip().

        event is an Event to process.

        Normally, your application would not call this function: it is
        called in the wxPython implementation to dispatch incoming
        user interface events to the framework (and application).

        However, you might need to call it if implementing new
        functionality (such as a new control) where you define new
        event types, as opposed to allowing the user to override
        virtual functions.

        An instance where you might actually override the ProcessEvent
        function is where you want to direct event processing to event
        handlers not normally noticed by wxWindows.  For example, in
        the document/view architecture, documents and views are
        potential event handlers.  When an event reaches a frame,
        ProcessEvent will need to be called on the associated document
        and view in case event handler functions are associated with
        these objects.  The property classes library (Property) also
        overrides ProcessEvent for similar reasons.

        The normal order of event table searching is as follows:

        1. If the object is disabled (via a call to
        EvtHandler.SetEvtHandlerEnabled) the function skips to step
        (6).

        2. If the object is a Window, ProcessEvent is recursively
        called on the window's Validator.  If this returns TRUE, the
        function exits.

        3. SearchEventTable is called for this event handler.  If this
        fails, the base class table is tried, and so on until no more
        tables exist or an appropriate function was found, in which
        case the function exits.

        4. The search is applied down the entire chain of event
        handlers (usually the chain has a length of one).  If this
        succeeds, the function exits.

        5. If the object is a Window and the event is a
        CommandEvent, ProcessEvent is recursively applied to the
        parent window's event handler.  If this returns TRUE, the
        function exits.

        6. Finally, ProcessEvent is called on the App object.

        See also:

        EvtHandler::SearchEventTable"""
        pass

    def SetEvtHandlerEnabled(self, enabled):
        """Enable or disable the event handler.

        You can use this function to avoid having to remove the event
        handler from the chain, for example when implementing a dialog
        editor and changing from edit to test mode."""
        pass

    def SetNextHandler(self, handler):
        """Set the pointer to the next handler."""
        pass

    def SetPreviousHandler(self, handler):
        """Set the pointer to the previous handler."""
        pass


