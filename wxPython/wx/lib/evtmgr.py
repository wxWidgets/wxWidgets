#---------------------------------------------------------------------------
# Name:        wxPython.lib.evtmgr
# Purpose:     An easier, more "Pythonic" and more OO method of registering
#              handlers for wxWindows events using the Publish/Subscribe
#              pattern.
#
# Author:      Robb Shecter and Robin Dunn
#
# Created:     12-December-2002
# RCS-ID:      $Id$
# Copyright:   (c) 2003 by db-X Corporation
# Licence:     wxWindows license
#---------------------------------------------------------------------------
# 12/02/2003 - Jeff Grimmett (grimmtooth@softhome.net)
#
# o Updated for 2.5 compatability.
#

"""
A module that allows multiple handlers to respond to single wxWindows
events.  This allows true NxN Observer/Observable connections: One
event can be received by multiple handlers, and one handler can
receive multiple events.

There are two ways to register event handlers.  The first way is
similar to standard wxPython handler registration:

    from wxPython.lib.evtmgr import eventManager
    eventManager.Register(handleEvents, EVT_BUTTON, win=frame, id=101)

There's also a new object-oriented way to register for events.  This
invocation is equivalent to the one above, but does not require the
programmer to declare or track control ids or parent containers:

    eventManager.Register(handleEvents, EVT_BUTTON, myButton)

This module is Python 2.1+ compatible.

"""
import  wx
import  pubsub # publish / subscribe library

#---------------------------------------------------------------------------


class EventManager:
    """
    This is the main class in the module, and is the only class that
    the application programmer needs to use.  There is a pre-created
    instance of this class called 'eventManager'.  It should not be
    necessary to create other instances.
    """
    def __init__(self):
        self.eventAdapterDict    = {}
        self.messageAdapterDict  = {}
        self.windowTopicLookup   = {}
        self.listenerTopicLookup = {}
        self.__publisher         = pubsub.Publisher()
        self.EMPTY_LIST          = []


    def Register(self, listener, event, source=None, win=None, id=None):
        """
        Registers a listener function (or any callable object) to
        receive events of type event coming from the source window.
        For example::
            eventManager.Register(self.OnButton, EVT_BUTTON, theButton)

        Alternatively, the specific window where the event is
        delivered, and/or the ID of the event source can be specified.
        For example::
            eventManager.Register(self.OnButton, EVT_BUTTON, win=self, id=ID_BUTTON)
            
        or::
            eventManager.Register(self.OnButton, EVT_BUTTON, theButton, self)
            
        """

        # 1. Check if the 'event' is actually one of the multi-
        #    event macros.
        if _macroInfo.isMultiEvent(event):
            raise 'Cannot register the macro, '+`event`+'.  Register instead the individual events.'

        # Support a more OO API.  This allows the GUI widget itself to
        # be specified, and the id to be retrieved from the system,
        # instead of kept track of explicitly by the programmer.
        # (Being used to doing GUI work with Java, this seems to me to be
        # the natural way of doing things.)
        if source is not None:
            id  = source.GetId()
            
        if win is None:
            # Some widgets do not function as their own windows.
            win = self._determineWindow(source)
            
        topic = (event, win, id)

        #  Create an adapter from the PS system back to wxEvents, and
        #  possibly one from wxEvents:
        if not self.__haveMessageAdapter(listener, topic):
            messageAdapter = MessageAdapter(eventHandler=listener, topicPattern=topic)
            try:
                self.messageAdapterDict[topic][listener] = messageAdapter
            except KeyError:
                self.messageAdapterDict[topic] = {}
                self.messageAdapterDict[topic][listener] = messageAdapter

            if not self.eventAdapterDict.has_key(topic):
                self.eventAdapterDict[topic] = EventAdapter(event, win, id)
        else:
            # Throwing away a duplicate request
            pass

        # For time efficiency when deregistering by window:
        try:
            self.windowTopicLookup[win].append(topic)
        except KeyError:
            self.windowTopicLookup[win] = []
            self.windowTopicLookup[win].append(topic)

        # For time efficiency when deregistering by listener:
        try:
            self.listenerTopicLookup[listener].append(topic)
        except KeyError:
            self.listenerTopicLookup[listener] = []
            self.listenerTopicLookup[listener].append(topic)

        # See if the source understands the listeningFor protocol.
        # This is a bit of a test I'm working on - it allows classes
        # to know when their events are being listened to.  I use
        # it to enable chaining events from contained windows only
        # when needed.
        if source is not None:
            try:
                # Let the source know that we're listening  for this
                # event.
                source.listeningFor(event)
            except AttributeError:
                pass

    # Some aliases for Register, just for kicks
    Bind = Register
    Subscribe = Register


    def DeregisterWindow(self, win):
        """
        Deregister all events coming from the given window.
        """
        win    = self._determineWindow(win)
        topics = self.__getTopics(win)

        if topics:
            for aTopic in topics:
                self.__deregisterTopic(aTopic)

            del self.windowTopicLookup[win]


    def DeregisterListener(self, listener):
        """
        Deregister all event notifications for the given listener.
        """
        try:
            topicList = self.listenerTopicLookup[listener]
        except KeyError:
            return

        for topic in topicList:
            topicDict = self.messageAdapterDict[topic]

            if topicDict.has_key(listener):
                topicDict[listener].Destroy()
                del topicDict[listener]

                if len(topicDict) == 0:
                    self.eventAdapterDict[topic].Destroy()
                    del self.eventAdapterDict[topic]
                    del self.messageAdapterDict[topic]

        del self.listenerTopicLookup[listener]


    def GetStats(self):
        """
        Return a dictionary with data about my state.
        """
        stats = {}
        stats['Adapters: Message'] = reduce(lambda x,y: x+y, [0] + map(len, self.messageAdapterDict.values()))
        stats['Adapters: Event']   = len(self.eventAdapterDict)
        stats['Topics: Total']     = len(self.__getTopics())
        stats['Topics: Dead']      = len(self.GetDeadTopics())
        return stats


    def DeregisterDeadTopics(self):
        """
        Deregister any entries relating to dead
        wxPython objects.  Not sure if this is an
        important issue; 1) My app code always de-registers
        listeners it doesn't need.  2) I don't think
        that lingering references to these dead objects
        is a problem.
        """
        for topic in self.GetDeadTopics():
            self.__deregisterTopic(topic)


    def GetDeadTopics(self):
        """
        Return a list of topics relating to dead wxPython
        objects.
        """
        return filter(self.__isDeadTopic, self.__getTopics())


    def __winString(self, aWin):
        """
        A string rep of a window for debugging
        """
        try:
            name = aWin.GetClassName()
            i    = id(aWin)
            return '%s #%d' % (name, i)
        except wx.PyDeadObjectError:
            return '(dead wx.Object)'


    def __topicString(self, aTopic):
        """
        A string rep of a topic for debugging
        """
        return '[%-26s %s]' % (aTopic[0].__name__, self.winString(aTopic[1]))


    def __listenerString(self, aListener):
        """
        A string rep of a listener for debugging
        """
        try:
            return aListener.im_class.__name__ + '.' + aListener.__name__
        except:
            return 'Function ' + aListener.__name__


    def __deregisterTopic(self, aTopic):
        try:
            messageAdapterList = self.messageAdapterDict[aTopic].values()
        except KeyError:
            # This topic isn't valid.  Probably because it was deleted
            # by listener.
            return

        for messageAdapter in messageAdapterList:
            messageAdapter.Destroy()

        self.eventAdapterDict[aTopic].Destroy()
        del self.messageAdapterDict[aTopic]
        del self.eventAdapterDict[aTopic]


    def __getTopics(self, win=None):
        if win is None:
            return self.messageAdapterDict.keys()

        if win is not None:
            try:
                return self.windowTopicLookup[win]
            except KeyError:
                return self.EMPTY_LIST


    def __isDeadWxObject(self, anObject):
        return isinstance(anObject, wx._wxPyDeadObject)


    def __isDeadTopic(self, aTopic):
        return self.__isDeadWxObject(aTopic[1])


    def __haveMessageAdapter(self, eventHandler, topicPattern):
        """
        Return True if there's already a message adapter
        with these specs.
        """
        try:
            return self.messageAdapterDict[topicPattern].has_key(eventHandler)
        except KeyError:
            return 0


    def _determineWindow(self, aComponent):
        """
        Return the window that corresponds to this component.
        A window is something that supports the Connect protocol.
        Most things registered with the event manager are a window,
        but there are apparently some exceptions.  If more are
        discovered, the implementation can be changed to a dictionary
        lookup along the lines of class : function-to-get-window.
        """
        if isinstance(aComponent, wx.MenuItem):
            return aComponent.GetMenu()
        else:
            return aComponent



#---------------------------------------------------------------------------
# From here down is implementaion and support classes, although you may
# find some of them useful in other contexts.
#---------------------------------------------------------------------------


class EventMacroInfo:
    """
    A class that provides information about event macros.
    """
    def __init__(self):
        self.lookupTable = {}


    def getEventTypes(self, eventMacro):
        """
        Return the list of event types that the given
        macro corresponds to.
        """
        try:
            return self.lookupTable[eventMacro]
        except KeyError:
            win = FakeWindow()
            try:
                eventMacro(win, None, None)
            except (TypeError, AssertionError):
                eventMacro(win, None)
            self.lookupTable[eventMacro] = win.eventTypes
            return win.eventTypes


    def eventIsA(self, event, macroList):
        """
        Return True if the event is one of the given
        macros.
        """
        eventType = event.GetEventType()
        for macro in macroList:
            if eventType in self.getEventTypes(macro):
                return 1
        return 0


    def macroIsA(self, macro, macroList):
        """
        Return True if the macro is in the macroList.
        The added value of this method is that it takes
        multi-events into account.  The macroList parameter
        will be coerced into a sequence if needed.
        """
        if callable(macroList):
            macroList = (macroList,)
        testList  = self.getEventTypes(macro)
        eventList = []
        for m in macroList:
            eventList.extend(self.getEventTypes(m))
        # Return True if every element in testList is in eventList
        for element in testList:
            if element not in eventList:
                return 0
        return 1


    def isMultiEvent(self, macro):
        """
        Return True if the given macro actually causes
        multiple events to be registered.
        """
        return len(self.getEventTypes(macro)) > 1


#---------------------------------------------------------------------------

class FakeWindow:
    """
    Used internally by the EventMacroInfo class.  The FakeWindow is
    the most important component of the macro-info utility: it
    implements the Connect() protocol of wxWindow, but instead of
    registering for events, it keeps track of what parameters were
    passed to it.
    """
    def __init__(self):
        self.eventTypes = []

    def Connect(self, id1, id2, eventType, handlerFunction):
        self.eventTypes.append(eventType)


#---------------------------------------------------------------------------

class EventAdapter:
    """
    A class that adapts incoming wxWindows events to
    Publish/Subscribe messages.

    In other words, this is the object that's seen by the
    wxWindows system.  Only one of these registers for any
    particular wxWindows event.  It then relays it into the
    PS system, which lets many listeners respond.
    """
    def __init__(self, func, win, id):
        """
        Instantiate a new adapter. Pre-compute my Publish/Subscribe
        topic, which is constant, and register with wxWindows.
        """
        self.publisher = pubsub.Publisher()
        self.topic     = ((func, win, id),)
        self.id        = id
        self.win       = win
        self.eventType = _macroInfo.getEventTypes(func)[0]

        # Register myself with the wxWindows event system
        try:
            func(win, id, self.handleEvent)
            self.callStyle = 3
        except (TypeError, AssertionError):
            func(win, self.handleEvent)
            self.callStyle = 2


    def disconnect(self):
        if self.callStyle == 3:
            return self.win.Disconnect(self.id, -1, self.eventType)
        else:
            return self.win.Disconnect(-1, -1, self.eventType)


    def handleEvent(self, event):
        """
        In response to a wxWindows event, send a PS message
        """
        self.publisher.sendMessage(topic=self.topic, data=event)


    def Destroy(self):
        try:
            if not self.disconnect():
                print 'disconnect failed'
        except wx.PyDeadObjectError:
            print 'disconnect failed: dead object'              ##????


#---------------------------------------------------------------------------

class MessageAdapter:
    """
    A class that adapts incoming Publish/Subscribe messages
    to wxWindows event calls.

    This class works opposite the EventAdapter, and
    retrieves the information an EventAdapter has sent in a message.
    Strictly speaking, this class is not required: Event listeners
    could pull the original wxEvent object out of the PS Message
    themselves.

    However, by pairing an instance of this class with each wxEvent
    handler, the handlers can use the standard API: they receive an
    event as a parameter.
    """
    def __init__(self, eventHandler, topicPattern):
        """
        Instantiate a new MessageAdapter that send wxEvents to the
        given eventHandler.
        """
        self.eventHandler = eventHandler
        pubsub.Publisher().subscribe(listener=self.deliverEvent, topic=(topicPattern,))

    def deliverEvent(self, message):
        event = message.data        # Extract the wxEvent
        self.eventHandler(event)    # Perform the call as wxWindows would

    def Destroy(self):
        pubsub.Publisher().unsubscribe(listener=self.deliverEvent)


#---------------------------------------------------------------------------
# Create globals

_macroInfo    = EventMacroInfo()

# For now a singleton is not enforced.  Should it be or can we trust
# the programmers?
eventManager  = EventManager()


#---------------------------------------------------------------------------
# simple test code


if __name__ == '__main__':
    app    = wx.PySimpleApp()
    frame  = wx.Frame(None, -1, 'Event Test', size=(300,300))
    button = wx.ToggleButton(frame, -1, 'Listen for Mouse Events')
    sizer  = wx.BoxSizer(wx.HORIZONTAL)
    sizer.Add(button, 0, 0 | wx.ALL, 10)
    frame.SetAutoLayout(1)
    frame.SetSizer(sizer)

    #
    # Demonstrate 1) register/deregister, 2) Multiple listeners receiving
    # one event, and 3) Multiple events going to one listener.
    #

    def printEvent(event):
        print 'Name:',event.GetClassName(),'Timestamp',event.GetTimestamp()

    def enableFrameEvents(event):
        # Turn the output of mouse events on and off
        if event.IsChecked():
            print '\nEnabling mouse events...'
            eventManager.Register(printEvent, wx.EVT_MOTION,    frame)
            eventManager.Register(printEvent, wx.EVT_LEFT_DOWN, frame)
        else:
            print '\nDisabling mouse events...'
            eventManager.DeregisterWindow(frame)

    # Send togglebutton events to both the on/off code as well
    # as the function that prints to stdout.
    eventManager.Register(printEvent,        wx.EVT_TOGGLEBUTTON, button)
    eventManager.Register(enableFrameEvents, wx.EVT_TOGGLEBUTTON, button)

    frame.CenterOnScreen()
    frame.Show(1)
    app.MainLoop()
