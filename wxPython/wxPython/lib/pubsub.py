#---------------------------------------------------------------------------
# Name:        wxPython.lib.pubsub
# Purpose:     The Publish/Subscribe framework used by evtmgr.EventManager
#
# Author:      Robb Shecter and Robin Dunn
#
# Created:     12-December-2002
# RCS-ID:      $Id$
# Copyright:   (c) 2002 by Robb Shecter <robb@acm.org>
# Licence:     wxWindows license
#---------------------------------------------------------------------------
"""
This module has classes for implementing the Publish/Subscribe design
pattern.

It's a very flexible PS implementation: The message topics are tuples
of any length, containing any objects (that can be used as hash keys).
A subscriber's topic matches any message topic for which it's a
sublist.

It also has many optimizations to favor time efficiency (ie., run-time
speed).  I did this because I use it to support extreme uses.  For
example, piping every wxWindows mouse event through to multiple
listeners, and expecting the app to have no noticeable slowdown.  This
has made the code somewhat obfuscated, but I've done my best to
document it.

The Server and Message classes are the two that clients interact
with..

This module is compatible with Python 2.1.

Author: Robb Shecter
"""

#---------------------------------------------------------------------------

class Publisher:
    """
    The publish/subscribe server.  This class is a Singleton.
    """
    def __init__(self):
        self.topicDict         = {}
        self.functionDict      = {}
        self.subscribeAllList  = []
        self.messageCount      = 0
        self.deliveryCount     = 0


    #
    # Public API
    #

    def subscribe(self, topic, listener):
        """
        Add the given subscription to the list.  This will
        add an entry recording the fact that the listener wants
        to get messages for (at least) the given topic.  This
        method may be called multiple times for one listener,
        registering it with many topics.  It can also be invoked
        many times for a particular topic, each time with a
        different listener.

        listener: expected to be either a method or function that
        takes zero or one parameters.  (Not counting 'self' in the
        case of methods. If it accepts a parameter, it will be given
        a reference to a Message object.

        topic: will  be converted to a tuple if it isn't one.
        It's a pattern matches any topic that it's a sublist
        of.  For example, this pattern:

          ('sports',)

        would match these:

          ('sports',)
          ('sports', 'baseball')
          ('sports', 'baseball', 'highscores')

        but not these:

          ()
          ('news')
          (12345)
        """
        if not callable(listener):
            raise TypeError('The P/S listener, '+`listener`+', is not callable.')
        aTopic = Topic(topic)

        # Determine now (at registration time) how many parameters
        # the listener expects, and get a reference to a function which
        # calls it correctly at message-send time.
        callableVersion = self.__makeCallable(listener)

        # Add this tuple to a list which is in a dict keyed by
        # the topic's first element.
        self.__addTopicToCorrectList(aTopic, listener, callableVersion)

        # Add to a dict in order to speed-up unsubscribing.
        self.__addFunctionLookup(listener, aTopic)


    def unsubscribe(self, listener):
        """
        Remove the given listener from the registry,
        for all topics that it's associated with.
        """
        if not callable(listener):
            raise TypeError('The P/S listener, '+`listener`+', is not callable.')
        topicList = self.getAssociatedTopics(listener)
        for aTopic in topicList:
            subscriberList = self.__getTopicList(aTopic)
            listToKeep = []
            for subscriber in subscriberList:
                if subscriber[0] != listener:
                    listToKeep.append(subscriber)
            self.__setTopicList(aTopic, listToKeep)
        self.__delFunctionLookup(listener)


    def getAssociatedTopics(self, listener):
        """
        Return a list of topics the given listener is
        registered with.
        """
        return self.functionDict.get(listener, [])


    def sendMessage(self, topic, data=None):
        """
        Relay a message to registered listeners.
        """
        aTopic    = Topic(topic)
        message   = Message(aTopic.items, data)
        topicList = self.__getTopicList(aTopic)

        # Send to the matching topics
        for subscriber in topicList:
            if subscriber[1].matches(aTopic):
                subscriber[2](message)

        # Send to any listeners registered for ALL
        for subscriber in self.subscribeAllList:
            subscriber[2](message)


    #
    # Private methods
    #

    def __makeCallable(self, function):
        """
        Return a function that is what the server
        will actually call.

        This is a time optimization: this removes a test
        for the number of parameters from the inner loop
        of sendMessage().
        """
        parameters = self.__parameterCount(function)
        if parameters == 0:
            # Return a function that calls the listener
            # with no arguments.
            return lambda m, f=function: f()
        elif parameters == 1:
            # Return a function that calls the listener
            # with one argument (which will be the message).
            return lambda m, f=function: f(m)
        else:
            raise TypeError('The publish/subscribe listener, '+`function`+', has wrong parameter count')


    def __parameterCount(self, callableObject):
        """
        Return the effective number of parameters required
        by the callable object.  In other words, the 'self'
        parameter of methods is not counted.
        """
        try:
            # Try to handle this like a method
            return callableObject.im_func.func_code.co_argcount - 1
        except AttributeError:
            pass

        try:
            # Try to handle this like a function
            return callableObject.func_code.co_argcount
        except AttributeError:
            raise 'Cannot determine if this is a method or function: '+str(callableObject)

    def __addFunctionLookup(self, aFunction, aTopic):
        try:
            aList = self.functionDict[aFunction]
        except KeyError:
            aList = []
            self.functionDict[aFunction] = aList
        aList.append(aTopic)


    def __delFunctionLookup(self, aFunction):
        try:
            del self.functionDict[aFunction]
        except KeyError:
            print 'Warning: listener not found. Logic error in PublishSubscribe?', aFunction


    def __addTopicToCorrectList(self, topic, listener, callableVersion):
        if len(topic.items) == 0:
            self.subscribeAllList.append((listener, topic, callableVersion))
        else:
            self.__getTopicList(topic).append((listener, topic, callableVersion))


    def __getTopicList(self, aTopic):
        """
        Return the correct sublist of subscribers based on the
        given topic.
        """
        try:
            elementZero = aTopic.items[0]
        except IndexError:
            return self.subscribeAllList

        try:
            subList = self.topicDict[elementZero]
        except KeyError:
            subList = []
            self.topicDict[elementZero] = subList
        return subList


    def __setTopicList(self, aTopic, aSubscriberList):
        try:
            self.topicDict[aTopic.items[0]] = aSubscriberList
        except IndexError:
            self.subscribeAllList = aSubscriberList


    def __call__(self):
        return self


# Create an instance with the same name as the class, effectivly
# hiding the class object so it can't be instantiated any more.  From
# this point forward any calls to Publisher() will invoke the __call__
# of this instance which just returns itself.
#
# The only flaw with this approach is that you can't derive a new
# class from Publisher without jumping through hoops.  If this ever
# becomes an issue then a new Singleton implementaion will need to be
# employed.
Publisher = Publisher()


#---------------------------------------------------------------------------

class Message:
    """
    A simple container object for the two components of
    a message; the topic and the data.
    """
    def __init__(self, topic, data):
        self.topic = topic
        self.data  = data

    def __str__(self):
        return '[Topic: '+`self.topic`+',  Data: '+`self.data`+']'


#---------------------------------------------------------------------------

class Topic:
    """
    A class that represents a publish/subscribe topic.
    Currently, it's only used internally in the framework; the
    API expects and returns plain old tuples.

    It currently exists mostly as a place to keep the matches()
    function.  This function, though, could also correctly be
    seen as an attribute of the P/S server.  Getting rid of this
    class would also mean one fewer object instantiation per
    message send.
    """

    listType   = type([])
    tupleType  = type(())

    def __init__(self, items):
        # Make sure we have a tuple.
        if type(items) == self.__class__.listType:
            items = tuple(items)
        elif type(items) != self.__class__.tupleType:
            items = (items,)
        self.items  = items
        self.length = len(items)


    def matches(self, aTopic):
        """
        Consider myself to be a topic pattern,
        and return True if I match the given specific
        topic.  For example,
        a = ('sports')
        b = ('sports','baseball')
        a.matches(b) --> 1
        b.matches(a) --> 0
        """
        # The question this method answers is equivalent to;
        # is my list a sublist of aTopic's?  So, my algorithm
        # is: 1) make a copy of the aTopic list which is
        # truncated to the pattern's length. 2) Test for
        # equality.
        #
        # This algorithm may be somewhat memory-intensive,
        # because it creates a temporary list on each
        # call to match.  A possible to-do would be to
        # re-write this with a hand-coded loop.
        return (self.items == aTopic.items[:self.length])


    def __repr__(self):
        import string
        return '<Topic>' + string.join(map(repr, self.items), ', ') + '</Topic>'


    def __eq__(self, aTopic):
        """
        Return True if I equal the given topic.  We're considered
        equal if our tuples are equal.
        """
        if type(self) != type(aTopic):
            return 0
        else:
            return self.items == aTopic.items


    def __ne__(self, aTopic):
        """
        Return False if I equal the given topic.
        """
        return not self == aTopic


#---------------------------------------------------------------------------


#
# Code for a simple command-line test
#
if __name__ == '__main__':

    class SimpleListener:
        def __init__(self, number):
            self.number = number
        def notify(self, message):
            print '#'+str(self.number)+' got the message:', message

    # Build a list of ten listeners.
    lList = []
    for x in range(10):
        lList.append(SimpleListener(x))

    server = Publisher()

    # Everyone's interested in politics...
    for x in lList:
        Publisher().subscribe(topic='politics', listener=x.notify)  # also tests singleton

    # But only the first four are interested in trivia.
    for x in lList[:4]:
        server.subscribe(topic='trivia',   listener=x.notify)

    # This one subscribes to everything.
    everythingListener = SimpleListener(999)
    server.subscribe(topic=(), listener=everythingListener.notify)

    # Now send out two messages, testing topic matching.
    server.sendMessage(topic='trivia',               data='What is the capitol of Oregon?')
    server.sendMessage(topic=('politics','germany'), data='The Greens have picked up another seat in the Bundestag.')

#---------------------------------------------------------------------------
