
#---------------------------------------------------------------------------
"""
This module provides a publish-subscribe component that allows
listeners to subcribe to messages of a given topic. Contrary to the
original wxPython.lib.pubsub module, which it is based on, it uses 
weak referencing to the subscribers so the subscribers are not kept
alive by the Publisher. Also, callable objects can be used in addition
to functions and bound methods. See Publisher class docs for more
details. 

Thanks to Robb Shecter and Robin Dunn for having provided 
the basis for this module (which now shares most of the concepts but
very little design or implementation with the original 
wxPython.lib.pubsub).

:Author:      Oliver Schoenborn
:Since:       Apr 2004
:Version:     $Id$
:Copyright:   \(c) 2004 Oliver Schoenborn
:License:     wxWidgets

Implementation notes
--------------------

In class Publisher, I represent the topics-listener set as a tree
where each node is a topic, and contains a list of listeners of that
topic, and a dictionary of subtopics of that topic. When the Publisher
is told to send a message for a given topic, it traverses the tree
down to the topic for which a message is being generated, all
listeners on the way get sent the message.

Publisher currently uses a weak listener topic tree to store the
topics for each listener, and if a listener dies before being
unsubscribed, the tree is notified, and the tree eliminates the
listener from itself.

Ideally, _TopicTreeNode would be a generic _TreeNode with named
subnodes, and _TopicTreeRoot would be a generic _Tree with named
nodes, and Publisher would store listeners in each node and a topic
tuple would be converted to a path in the tree.  This would lead to a
much cleaner separation of concerns. But time is over, tim to move on.

"""
#---------------------------------------------------------------------------

# for function and method parameter counting:
from types   import InstanceType
from inspect import getargspec, ismethod, isfunction
# for weakly bound methods:
from new     import instancemethod as InstanceMethod
from weakref import ref as WeakRef

# -----------------------------------------------------------------------------

def _isbound(method):
    """Return true if method is a bound method, false otherwise"""
    assert ismethod(method)
    return method.im_self is not None


def _paramMinCountFunc(function):
    """Given a function, return pair (min,d) where min is minimum # of
    args required, and d is number of default arguments."""
    assert isfunction(function)
    (args, va, kwa, dflt) = getargspec(function)
    lenDef = len(dflt or ())
    return (len(args or ()) - lenDef, lenDef)


def _paramMinCount(callableObject):
    """
    Given a callable object (function, method or callable instance),
    return pair (min,d) where min is minimum # of args required, and d
    is number of default arguments. The 'self' parameter, in the case
    of methods, is not counted.
    """
    if type(callableObject) is InstanceType:
        min, d = _paramMinCountFunc(callableObject.__call__.im_func)
        return min-1, d
    elif ismethod(callableObject):
        min, d = _paramMinCountFunc(callableObject.im_func)
        return min-1, d
    elif isfunction(callableObject):
        return _paramMinCountFunc(callableObject)
    else:
        raise 'Cannot determine type of callable: '+repr(callableObject)


def _tupleize(items):
    """Convert items to tuple if not already one, 
    so items must be a list, tuple or non-sequence"""
    if isinstance(items, type([])):
        raise TypeError, 'Not allowed to tuple-ize a list'
    elif not isinstance(items, type(())):
        items = (items,)
    return items


def _getCallableName(callable):
    """Get name for a callable, ie function, bound 
    method or callable instance"""
    if ismethod(callable):
        return '%s.%s ' % (callable.im_self, callable.im_func.func_name)
    elif isfunction(callable):
        return '%s ' % callable.__name__
    else:
        return '%s ' % callable
    
    
def _removeItem(item, fromList):
    """Attempt to remove item from fromList, return true 
    if successful, false otherwise."""
    try: 
        fromList.remove(item)
        return True
    except ValueError:
        return False
        
        
# -----------------------------------------------------------------------------

class _WeakMethod:
    """Represent a weak bound method, i.e. a method doesn't keep alive the 
    object that it is bound to. It uses WeakRef which, used on its own, 
    produces weak methods that are dead on creation, not very useful. 
    Typically, you will use the getRef() function instead of using
    this class directly. """
    
    def __init__(self, method, notifyDead = None):
        """The method must be bound. notifyDead will be called when 
        object that method is bound to dies. """
        assert ismethod(method)
        if method.im_self is None:
            raise ValueError, "We need a bound method!"
        if notifyDead is None:
            self.objRef = WeakRef(method.im_self)
        else:
            self.objRef = WeakRef(method.im_self, notifyDead)
        self.fun = method.im_func
        self.cls = method.im_class
        
    def __call__(self):
        """Returns a new.instancemethod if object for method still alive. 
        Otherwise return None. Note that instancemethod causes a 
        strong reference to object to be created, so shouldn't save 
        the return value of this call. Note also that this __call__
        is required only for compatibility with WeakRef.ref(), otherwise
        there would be more efficient ways of providing this functionality."""
        if self.objRef() is None:
            return None
        else:
            return InstanceMethod(self.fun, self.objRef(), self.cls)
        
    def __cmp__(self, method2):
        """Two _WeakMethod objects compare equal if they refer to the same method
        of the same instance."""
        return hash(self) - hash(method2)
    
    def __hash__(self):
        """Hash must depend on WeakRef of object, and on method, so that
        separate methods, bound to same object, can be distinguished. 
        I'm not sure how robust this hash function is, any feedback 
        welcome."""
        return hash(self.fun)/2 + hash(self.objRef)/2
    
    def __repr__(self):
        dead = ''
        if self.objRef() is None: 
            dead = '; DEAD'
        obj = '<%s at %s%s>' % (self.__class__, id(self), dead)
        return obj
        
    def refs(self, weakRef):
        """Return true if we are storing same object referred to by weakRef."""
        return self.objRef == weakRef


def _getWeakRef(obj, notifyDead=None):
    """Get a weak reference to obj. If obj is a bound method, a _WeakMethod
    object, that behaves like a WeakRef, is returned, if it is
    anything else a WeakRef is returned. If obj is an unbound method,
    a ValueError will be raised."""
    if ismethod(obj):
        createRef = _WeakMethod
    else:
        createRef = WeakRef
        
    if notifyDead is None:
        return createRef(obj)
    else:
        return createRef(obj, notifyDead)
    
    
# -----------------------------------------------------------------------------

def getStrAllTopics():
    """Function to call if, for whatever reason, you need to know 
    explicitely what is the string to use to indicate 'all topics'."""
    return ''


# alias, easier to see where used
ALL_TOPICS = getStrAllTopics()

# -----------------------------------------------------------------------------


class _NodeCallback:
    """Encapsulate a weak reference to a method of a TopicTreeNode
    in such a way that the method can be called, if the node is 
    still alive, but the callback does not *keep* the node alive.
    Also, define two methods, preNotify() and noNotify(), which can 
    be redefined to something else, very useful for testing. 
    """
    
    def __init__(self, obj):
        self.objRef = _getWeakRef(obj)
        
    def __call__(self, weakCB):
        notify = self.objRef()
        if notify is not None: 
            self.preNotify(weakCB)
            notify(weakCB)
        else: 
            self.noNotify()
            
    def preNotify(self, dead):
        """'Gets called just before our callback (self.objRef) is called"""
        pass
        
    def noNotify(self):
        """Gets called if the TopicTreeNode for this callback is dead"""
        pass


class _TopicTreeNode:
    """A node in the topic tree. This contains a list of callables
    that are interested in the topic that this node is associated
    with, and contains a dictionary of subtopics, whose associated
    values are other _TopicTreeNodes. The topic of a node is not stored
    in the node, so that the tree can be implemented as a dictionary
    rather than a list, for ease of use (and, likely, performance).
    
    Note that it uses _NodeCallback to encapsulate a callback for 
    when a registered listener dies, possible thanks to WeakRef.
    Whenever this callback is called, the onDeadListener() function, 
    passed in at construction time, is called (unless it is None).
    """
    
    def __init__(self, topicPath, onDeadListenerWeakCB):
        self.__subtopics = {}
        self.__callables = []
        self.__topicPath = topicPath
        self.__onDeadListenerWeakCB = onDeadListenerWeakCB
        
    def getPathname(self): 
        """The complete node path to us, ie., the topic tuple that would lead to us"""
        return self.__topicPath
    
    def createSubtopic(self, subtopic, topicPath):
        """Create a child node for subtopic"""
        return self.__subtopics.setdefault(subtopic,
                    _TopicTreeNode(topicPath, self.__onDeadListenerWeakCB))
    
    def hasSubtopic(self, subtopic):
        """Return true only if topic string is one of subtopics of this node"""
        return self.__subtopics.has_key(subtopic)
    
    def getNode(self, subtopic):
        """Return ref to node associated with subtopic"""
        return self.__subtopics[subtopic]
    
    def addCallable(self, callable):
        """Add a callable to list of callables for this topic node"""
        try:
            id = self.__callables.index(_getWeakRef(callable))
            return self.__callables[id]
        except ValueError:
            wrCall = _getWeakRef(callable, _NodeCallback(self.__notifyDead))
            self.__callables.append(wrCall)
            return wrCall
            
    def getCallables(self):
        """Get callables associated with this topic node"""
        return [cb() for cb in self.__callables if cb() is not None]
    
    def hasCallable(self, callable):
        """Return true if callable in this node"""
        try: 
            self.__callables.index(_getWeakRef(callable))
            return True
        except ValueError:
            return False
    
    def sendMessage(self, message):
        """Send a message to our callables"""
        deliveryCount = 0
        for cb in self.__callables:
            listener = cb()
            if listener is not None:
                listener(message)
                deliveryCount += 1
        return deliveryCount
    
    def removeCallable(self, callable):
        """Remove weak callable from our node (and return True). 
        Does nothing if not here (and returns False)."""
        try: 
            self.__callables.remove(_getWeakRef(callable))
            return True
        except ValueError:
            return False
        
    def clearCallables(self):
        """Abandon list of callables to caller. We no longer have 
        any callables after this method is called."""
        tmpList = [cb for cb in self.__callables if cb() is not None]
        self.__callables = []
        return tmpList
        
    def __notifyDead(self, dead):
        """Gets called when a listener dies, thanks to WeakRef"""
        #print 'TreeNODE', `self`, 'received death certificate for ', dead
        self.__cleanupDead()
        if self.__onDeadListenerWeakCB is not None:
            cb = self.__onDeadListenerWeakCB()
            if cb is not None: 
                cb(dead)
        
    def __cleanupDead(self):
        """Remove all dead objects from list of callables"""
        self.__callables = [cb for cb in self.__callables if cb() is not None]
        
    def __str__(self):
        """Print us in a not-so-friendly, but readable way, good for debugging."""
        strVal = []
        for callable in self.getCallables():
            strVal.append(_getCallableName(callable))
        for topic, node in self.__subtopics.iteritems():
            strVal.append(' (%s: %s)' %(topic, node))
        return ''.join(strVal)
      
      
class _TopicTreeRoot(_TopicTreeNode):
    """
    The root of the tree knows how to access other node of the 
    tree and is the gateway of the tree user to the tree nodes. 
    It can create topics, and and remove callbacks, etc. 
    
    For efficiency, it stores a dictionary of listener-topics, 
    so that unsubscribing a listener just requires finding the 
    topics associated to a listener, and finding the corresponding
    nodes of the tree. Without it, unsubscribing would require 
    that we search the whole tree for all nodes that contain 
    given listener. Since Publisher is a singleton, it will 
    contain all topics in the system so it is likely to be a large
    tree. However, it is possible that in some runs, unsubscribe()
    is called very little by the user, in which case most unsubscriptions
    are automatic, ie caused by the listeners dying. In this case, 
    a flag is set to indicate that the dictionary should be cleaned up
    at the next opportunity. This is not necessary, it is just an 
    optimization.
    """
    
    def __init__(self):
        self.__callbackDict  = {}
        self.__callbackDictCleanup = 0
        # all child nodes will call our __rootNotifyDead method
        # when one of their registered listeners dies 
        _TopicTreeNode.__init__(self, (ALL_TOPICS,), 
                                _getWeakRef(self.__rootNotifyDead))
        
    def addTopic(self, topic, listener):
        """Add topic to tree if doesnt exist, and add listener to topic node"""
        assert isinstance(topic, tuple)
        topicNode = self.__getTreeNode(topic, make=True)
        weakCB = topicNode.addCallable(listener)
        assert topicNode.hasCallable(listener)

        theList = self.__callbackDict.setdefault(weakCB, [])
        assert self.__callbackDict.has_key(weakCB)
        # add it only if we don't already have it
        try:
            weakTopicNode = WeakRef(topicNode)
            theList.index(weakTopicNode)
        except ValueError:
            theList.append(weakTopicNode)
        assert self.__callbackDict[weakCB].index(weakTopicNode) >= 0
        
    def getTopics(self, listener):
        """Return the list of topics for given listener"""
        weakNodes = self.__callbackDict.get(_getWeakRef(listener), [])
        return [weakNode().getPathname() for weakNode in weakNodes 
                if weakNode() is not None]

    def isSubscribed(self, listener, topic=None):
        """Return true if listener is registered for topic specified. 
        If no topic specified, return true if subscribed to something.
        Use topic=getStrAllTopics() to determine if a listener will receive 
        messages for all topics."""
        weakCB = _getWeakRef(listener)
        if topic is None: 
            return self.__callbackDict.has_key(weakCB)
        else:
            topicPath = _tupleize(topic)
            for weakNode in self.__callbackDict[weakCB]:
                if topicPath == weakNode().getPathname():
                    return True
            return False
            
    def unsubscribe(self, listener, topicList):
        """Remove listener from given list of topics. If topicList
        doesn't have any topics for which listener has subscribed,
        the onNotSubscribed callback, if not None, will be called,
        as onNotSubscribed(listener, topic)."""
        weakCB = _getWeakRef(listener)
        if not self.__callbackDict.has_key(weakCB):
            return
        
        cbNodes = self.__callbackDict[weakCB] 
        if topicList is None:
            for weakNode in cbNodes:
                weakNode().removeCallable(listener)
            del self.__callbackDict[weakCB] 
            return

        for weakNode in cbNodes:
            node = weakNode()
            if node is not None and node.getPathname() in topicList:
                success = node.removeCallable(listener)
                assert success == True
                cbNodes.remove(weakNode)
                assert not self.isSubscribed(listener, node.getPathname())

    def unsubAll(self, topicList, onNoSuchTopic):
        """Unsubscribe all listeners registered for any topic in 
        topicList. If a topic in the list does not exist, and 
        onNoSuchTopic is not None, a call
        to onNoSuchTopic(topic) is done for that topic."""
        for topic in topicList:
            node = self.__getTreeNode(topic)
            if node is not None:
                weakCallables = node.clearCallables()
                for callable in weakCallables:
                    weakNodes = self.__callbackDict[callable]
                    success = _removeItem(WeakRef(node), weakNodes)
                    assert success == True
                    if weakNodes == []:
                        del self.__callbackDict[callable]
            elif onNoSuchTopic is not None: 
                onNoSuchTopic(topic)
            
    def sendMessage(self, topic, message, onTopicNeverCreated):
        """Send a message for given topic to all registered listeners. If 
        topic doesn't exist, call onTopicNeverCreated(topic)."""
        # send to the all-toipcs listeners
        deliveryCount = _TopicTreeNode.sendMessage(self, message)
        # send to those who listen to given topic or any of its supertopics
        node = self
        for topicItem in topic:
            assert topicItem != ''
            if node.hasSubtopic(topicItem):
                node = node.getNode(topicItem)
                deliveryCount += node.sendMessage(message)
            else: # topic never created, don't bother continuing
                if onTopicNeverCreated is not None:
                    onTopicNeverCreated(aTopic)
                break
        return deliveryCount

    def numListeners(self):
        """Return a pair (live, dead) with count of live and dead listeners in tree"""
        dead, live = 0, 0
        for cb in self.__callbackDict:
            if cb() is None: 
                dead += 1
            else:
                live += 1
        return live, dead
    
    # clean up the callback dictionary after how many dead listeners
    callbackDeadLimit = 10

    def __rootNotifyDead(self, dead):
        #print 'TreeROOT received death certificate for ', dead
        self.__callbackDictCleanup += 1
        if self.__callbackDictCleanup > _TopicTreeRoot.callbackDeadLimit:
            self.__callbackDictCleanup = 0
            oldDict = self.__callbackDict
            self.__callbackDict = {}
            for weakCB, weakNodes in oldDict.iteritems():
                if weakCB() is not None:
                    self.__callbackDict[weakCB] = weakNodes
        
    def __getTreeNode(self, topic, make=False):
        """Return the tree node for 'topic' from the topic tree. If it 
        doesnt exist and make=True, create it first."""
        # if the all-topics, give root; 
        if topic == (ALL_TOPICS,):
            return self
            
        # not root, so traverse tree
        node = self
        path = ()
        for topicItem in topic:
            path += (topicItem,)
            if topicItem == ALL_TOPICS:
                raise ValueError, 'Topic tuple must not contain ""'
            if make: 
                node = node.createSubtopic(topicItem, path)
            elif node.hasSubtopic(topicItem):
                node = node.getNode(topicItem)
            else:
                return None
        # done
        return node
        
    def printCallbacks(self):
        strVal = ['Callbacks:\n']
        for listener, weakTopicNodes in self.__callbackDict.iteritems():
            topics = [topic() for topic in weakTopicNodes if topic() is not None]
            strVal.append('  %s: %s\n' % (_getCallableName(listener()), topics))
        return ''.join(strVal)
        
    def __str__(self):
        return 'all: %s' % _TopicTreeNode.__str__(self)
    
    
# -----------------------------------------------------------------------------

class Publisher:
    """
    The publish/subscribe manager.  It keeps track of which listeners
    are interested in which topics (see subscribe()), and sends a
    Message for a given topic to listeners that have subscribed to
    that topic, with optional user data (see sendMessage()).
    
    The three important concepts for Publisher are:
        
    - listener: a function, bound method or
      callable object that can be called with only one parameter
      (not counting 'self' in the case of methods). The parameter
      will be a reference to a Message object. E.g., these listeners
      are ok::
          
          class Foo:
              def __call__(self, a, b=1): pass # can be called with only one arg
              def meth(self,  a):         pass # takes only one arg
              def meth2(self, a=2, b=''): pass # can be called with one arg
        
          def func(a, b=''): pass
          
          Foo foo
          Publisher().subscribe(foo)           # functor
          Publisher().subscribe(foo.meth)      # bound method
          Publisher().subscribe(foo.meth2)     # bound method
          Publisher().subscribe(func)          # function
          
      The three types of callables all have arguments that allow a call 
      with only one argument. In every case, the parameter 'a' will contain
      the message. 

    - topic: a single word or tuple of words (though word could probably 
      be any kind of object, not just a string, but this has not been 
      tested). A tuple denotes a hierarchy of topics from most general
      to least. For example, a listener of this topic::

          ('sports','baseball')

      would receive messages for these topics::

          ('sports', 'baseball')                 # because same
          ('sports', 'baseball', 'highscores')   # because more specific

      but not these::

           'sports'      # because more general
          ('sports',)    # because more general
          () or ('')     # because only for those listening to 'all' topics
          ('news')       # because different topic
          
    - message: this is an instance of Message, containing the topic for 
      which the message was sent, and any data the sender specified. 
      
    :note: This class is visible to importers of pubsub only as a 
    Singleton. I.e., every time you execute 'Publisher()', it's 
    actually the same instance of publisher that is returned. So to 
    use, just do 'Publisher().method()'.
    """
    
    __ALL_TOPICS_TPL = (ALL_TOPICS, )
    
    def __init__(self):
        self.__messageCount  = 0
        self.__deliveryCount = 0
        self.__topicTree     = _TopicTreeRoot()

    #
    # Public API
    #

    def getDeliveryCount(self):
        """How many listeners have received a message since beginning of run"""
        return self.__deliveryCount
    
    def getMessageCount(self):
        """How many times sendMessage() was called since beginning of run"""
        return self.__messageCount
    
    def subscribe(self, listener, topic = ALL_TOPICS):
        """
        Subscribe listener for given topic. If topic is not specified,
        listener will be subscribed for all topics (that listener will 
        receive a Message for any topic for which a message is generated). 
        
        This method may be
        called multiple times for one listener, registering it with
        many topics.  It can also be invoked many times for a
        particular topic, each time with a different listener.
        See the class doc for requirements on listener and topic.

        :note: Calling 
        this method for the same listener, with two topics in the same 
        branch of the topic hierarchy, will cause the listener to be
        notified twice when a message for the deepest topic is sent. E.g.
        subscribe(listener, 't1') and then subscribe(listener, ('t1','t2'))
        means that when calling sendMessage('t1'), listener gets one message,
        but when calling sendMessage(('t1','t2')), listener gets message 
        twice. This effect could be eliminated but it would not be safe to 
        do so: how do we know what topic to give the listener? Answer appears
        trivial at first but is far from obvious. It is best to rely on the
        user to be careful about who registers for what topics. 
        """
        self.validate(listener)

        if topic is None: 
            raise TypeError, 'Topic must be either a word, tuple of '\
                             'words, or getStrAllTopics()'
            
        self.__topicTree.addTopic(_tupleize(topic), listener)

    def isSubscribed(self, listener, topic=None):
        """Return true if listener has subscribed to topic specified. 
        If no topic specified, return true if subscribed to something.
        Use getStrAllTopics() to determine if a listener will receive 
        messages for all topics."""
        return self.__topicTree.isSubscribed(listener, topic)
            
    def validate(self, listener):
        """Similar to isValid(), but raises a TypeError exception if not valid"""
        # check callable
        if not callable(listener):
            raise TypeError, 'Listener '+`listener`+' must be a '\
                             'function, bound method or instance.'
        # ok, callable, but if method, is it bound:
        elif ismethod(listener) and not _isbound(listener):
            raise TypeError, 'Listener '+`listener`+\
                             ' is a method but it is unbound!'
                             
        # check that it takes the right number of parameters
        min, d = _paramMinCount(listener)
        if min > 1:
            raise TypeError, 'Listener '+`listener`+" can't"\
                             ' require more than one parameter!'
        if min <= 0 and d == 0:
            raise TypeError, 'Listener '+`listener`+' lacking arguments!'
                             
        assert (min == 0 and d>0) or (min == 1)

    def isValid(self, listener):
        """Return true only if listener will be able to subscribe to Publisher."""
        try: 
            self.validate(listener)
            return True
        except TypeError:
            return False

    def unsubAll(self, topics=None, onNoSuchTopic=None):
        """Unsubscribe all listeners subscribed for topics. Topics can 
        be a single topic (string or tuple) or a list of topics (ie 
        list containing strings and/or tuples). If topics is not 
        specified, all listeners for all topics will be unsubscribed, 
        ie. the Publisher singleton will have no topics and no listeners
        left. If topics was specified and is not found among contained
        topics, the onNoSuchTopic, if specified, will be called, with 
        the name of the topic."""
        if topics is None: 
            del self.__topicTree
            self.__topicTree = _TopicTreeRoot()
            return
        
        # make sure every topics are in tuple form
        if isinstance(topics, type([])):
            topicList = [_tupleize(x) for x in topics]
        else:
            topicList = [_tupleize(topics)]
            
        # unsub every listener of topics
        self.__topicTree.unsubAll(topicList, onNoSuchTopic)
            
    def unsubscribe(self, listener, topics=None):
        """Unsubscribe listener. If topics not specified, listener is
        completely unsubscribed. Otherwise, it is unsubscribed only 
        for the topic (the usual tuple) or list of topics (ie a list
        of tuples) specified. In this case, if listener is not actually
        subscribed for (one of) the topics, the optional onNotSubscribed
        callback will be called, as onNotSubscribed(listener, missingTopic).
        
        Note that if listener subscribed for two topics (a,b) and (a,c), 
        then unsubscribing for topic (a) will do nothing. You must 
        use getAssociatedTopics(listener) and give unsubscribe() the returned 
        list (or a subset thereof).
        """
        self.validate(listener)
        topicList = None
        if topics is not None:
            if isinstance(topics, list):
                topicList = [_tupleize(x) for x in topics]
            else:
                topicList = [_tupleize(topics)]
            
        self.__topicTree.unsubscribe(listener, topicList)
        
    def getAssociatedTopics(self, listener):
        """Return a list of topics the given listener is registered with. 
        Returns [] if listener never subscribed.
        
        :attention: when using the return of this method to compare to 
        expected list of topics, remember that topics that are not in the
        form of a tuple appear as a one-tuple in the return. E.g. if you 
        have subscribed a listener to 'topic1' and ('topic2','subtopic2'), 
        this method returns::
            
            associatedTopics = [('topic1',), ('topic2','subtopic2')]
        """
        return self.__topicTree.getTopics(listener)
    
    def sendMessage(self, topic=ALL_TOPICS, data=None, onTopicNeverCreated=None):
        """Send a message for given topic, with optional data, to
        subscribed listeners. If topic is not specified, only the
        listeners that are interested in all topics will receive
        message. The onTopicNeverCreated is an optional callback of
        your choice that will be called if the topic given was never
        created (i.e. it, or one of its subtopics, was never
        subscribed to). The callback must be of the form f(a)."""
        aTopic  = _tupleize(topic)
        message = Message(aTopic, data)
        self.__messageCount += 1
        
        # send to those who listen to all topics
        self.__deliveryCount += \
            self.__topicTree.sendMessage(aTopic, message, onTopicNeverCreated)
        
    #
    # Private methods
    #

    def __call__(self):
        """Allows for singleton"""
        return self
    
    def __str__(self):
        return str(self.__topicTree)

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
    a message; the topic and the user data.
    """
    def __init__(self, topic, data):
        self.topic = topic
        self.data  = data

    def __str__(self):
        return '[Topic: '+`self.topic`+',  Data: '+`self.data`+']'


#---------------------------------------------------------------------------


#
# Code for a simple command-line test
#
def test():
    def done(funcName):
        print '----------- Done %s -----------' % funcName
        
    def testParam():
        def testFunc(a,b,c=1): pass
        class Foo:
            def testMeth(self,a,b): pass
            def __call__(self, a): pass
            
        foo = Foo()
        assert _paramMinCount(testFunc)==(2,1)
        assert _paramMinCount(Foo.testMeth)==(2,0)
        assert _paramMinCount(foo.testMeth)==(2,0)
        assert _paramMinCount(foo)==(1,0)
    
        done('testParam')

    testParam()
    #------------------------

    _NodeCallback.notified = 0
    def testPreNotifyNode(self, dead):
        _NodeCallback.notified += 1
        print 'testPreNotifyNODE heard notification of', `dead`
    _NodeCallback.preNotify = testPreNotifyNode
    
    def testTreeNode():

        class WS:
            def __init__(self, s):
                self.s = s
            def __call__(self, msg):
                print 'WS#', self.s, ' received msg ', msg
            def __str__(self):
                return self.s
            
        def testPreNotifyRoot(dead):
            print 'testPreNotifyROOT heard notification of', `dead`
    
        node = _TopicTreeNode((ALL_TOPICS,), WeakRef(testPreNotifyRoot))
        boo, baz, bid = WS('boo'), WS('baz'), WS('bid')
        node.addCallable(boo)
        node.addCallable(baz)
        node.addCallable(boo)
        assert node.getCallables() == [boo,baz]
        assert node.hasCallable(boo)
        
        node.removeCallable(bid) # no-op
        assert node.hasCallable(baz)
        assert node.getCallables() == [boo,baz]
        
        node.removeCallable(boo)
        assert node.getCallables() == [baz]
        assert node.hasCallable(baz)
        assert not node.hasCallable(boo)
        
        node.removeCallable(baz)
        assert node.getCallables() == []
        assert not node.hasCallable(baz)

        node2 = node.createSubtopic('st1', ('st1',))
        node3 = node.createSubtopic('st2', ('st2',))
        cb1, cb2, cb = WS('st1_cb1'), WS('st1_cb2'), WS('st2_cb')
        node2.addCallable(cb1)
        node2.addCallable(cb2)
        node3.addCallable(cb)
        node2.createSubtopic('st3', ('st1','st3'))
        node2.createSubtopic('st4', ('st1','st4'))
       
        print str(node)
        assert str(node) == ' (st1: st1_cb1 st1_cb2  (st4: ) (st3: )) (st2: st2_cb )'
    
        # verify send message, and that a dead listener does not get sent one
        delivered = node2.sendMessage('hello')
        assert delivered == 2
        del cb1
        delivered = node2.sendMessage('hello')
        assert delivered == 1
        assert _NodeCallback.notified == 1
        
        done('testTreeNode')
        
    testTreeNode()
    #------------------------
    
    def testValidate():
        class Foo:
            def __call__(self, a):   pass
            def fun(self, b):        pass
            def fun2(self, b=1):     pass
            def fun3(self, a, b=2):  pass
            def badFun(self):        pass
            def badFun2():           pass
            def badFun3(self, a, b): pass
            
        server = Publisher()
        foo = Foo()
        server.validate(foo)
        server.validate(foo.fun)
        server.validate(foo.fun2)
        server.validate(foo.fun3)
        assert not server.isValid(foo.badFun)
        assert not server.isValid(foo.badFun2)
        assert not server.isValid(foo.badFun3)
    
        done('testValidate')

    testValidate()
    #------------------------
    
    class SimpleListener:
        def __init__(self, number):
            self.number = number
        def __call__(self, message = ''): 
            print 'Callable #%s got the message "%s"' %(self.number, message)
        def notify(self, message):
            print '%s.notify() got the message "%s"' %(self.number, message)
        def __str__(self):
            return "SimpleListener_%s" % self.number

    def testSubscribe():
        publisher = Publisher()
        
        topic1 = 'politics'
        topic2 = ('history','middle age')
        topic3 = ('politics','UN')
        topic4 = ('politics','NATO')
        topic5 = ('politics','NATO','US')
        
        lisnr1 = SimpleListener(1)
        lisnr2 = SimpleListener(2)
        def func(message, a=1): 
            print 'Func received message "%s"' % message
        lisnr3 = func
        lisnr4 = lambda x: 'Lambda received message "%s"' % x

        assert not publisher.isSubscribed(lisnr1)
        assert not publisher.isSubscribed(lisnr2)
        assert not publisher.isSubscribed(lisnr3)
        assert not publisher.isSubscribed(lisnr4)
        
        publisher.subscribe(lisnr1, topic1)
        assert publisher.getAssociatedTopics(lisnr1) == [(topic1,)]
        publisher.subscribe(lisnr1, topic2)
        publisher.subscribe(lisnr1, topic1) # do it again, should be no-op
        assert publisher.getAssociatedTopics(lisnr1) == [(topic1,),topic2]
        publisher.subscribe(lisnr2.notify, topic3)
        assert publisher.getAssociatedTopics(lisnr2.notify) == [topic3]
        assert publisher.getAssociatedTopics(lisnr1) == [(topic1,),topic2]
        publisher.subscribe(lisnr3, topic5)
        assert publisher.getAssociatedTopics(lisnr3) == [topic5]
        assert publisher.getAssociatedTopics(lisnr2.notify) == [topic3]
        assert publisher.getAssociatedTopics(lisnr1) == [(topic1,),topic2]
        publisher.subscribe(lisnr4)
        
        print "Publisher tree: ", publisher
        assert publisher.isSubscribed(lisnr1)
        assert publisher.isSubscribed(lisnr1, topic1)
        assert publisher.isSubscribed(lisnr1, topic2)
        assert publisher.isSubscribed(lisnr2.notify)
        assert publisher.isSubscribed(lisnr3, topic5)
        assert publisher.isSubscribed(lisnr4, ALL_TOPICS)
        expectTopicTree = 'all: <lambda>  (politics: SimpleListener_1  (UN: SimpleListener_2.notify ) (NATO:  (US: func ))) (history:  (middle age: SimpleListener_1 ))'
        print "Publisher tree: ", publisher
        assert str(publisher) == expectTopicTree
        
        publisher.unsubscribe(lisnr1, 'booboo') # should do nothing
        assert publisher.getAssociatedTopics(lisnr1) == [(topic1,),topic2]
        assert publisher.getAssociatedTopics(lisnr2.notify) == [topic3]
        assert publisher.getAssociatedTopics(lisnr3) == [topic5]
        publisher.unsubscribe(lisnr1, topic1)
        assert publisher.getAssociatedTopics(lisnr1) == [topic2]
        assert publisher.getAssociatedTopics(lisnr2.notify) == [topic3]
        assert publisher.getAssociatedTopics(lisnr3) == [topic5]
        publisher.unsubscribe(lisnr1, topic2)
        publisher.unsubscribe(lisnr1, topic2)
        publisher.unsubscribe(lisnr2.notify, topic3)
        publisher.unsubscribe(lisnr3, topic5)
        assert publisher.getAssociatedTopics(lisnr1) == []
        assert publisher.getAssociatedTopics(lisnr2.notify) == []
        assert publisher.getAssociatedTopics(lisnr3) == []
        publisher.unsubscribe(lisnr4)
        
        expectTopicTree = 'all:  (politics:  (UN: ) (NATO:  (US: ))) (history:  (middle age: ))'
        print "Publisher tree: ", publisher
        assert str(publisher) == expectTopicTree
        assert publisher.getDeliveryCount() == 0
        assert publisher.getMessageCount() == 0
        
        publisher.unsubAll()
        assert str(publisher) == 'all: '
        
        done('testSubscribe')
    
    testSubscribe()
    #------------------------
    
    def testUnsubAll():
        publisher = Publisher()
        
        topic1 = 'politics'
        topic2 = ('history','middle age')
        topic3 = ('politics','UN')
        topic4 = ('politics','NATO')
        topic5 = ('politics','NATO','US')
        
        lisnr1 = SimpleListener(1)
        lisnr2 = SimpleListener(2)
        def func(message, a=1): 
            print 'Func received message "%s"' % message
        lisnr3 = func
        lisnr4 = lambda x: 'Lambda received message "%s"' % x

        publisher.subscribe(lisnr1, topic1)
        publisher.subscribe(lisnr1, topic2)
        publisher.subscribe(lisnr2.notify, topic3)
        publisher.subscribe(lisnr3, topic2)
        publisher.subscribe(lisnr3, topic5)
        publisher.subscribe(lisnr4)
        
        expectTopicTree = 'all: <lambda>  (politics: SimpleListener_1  (UN: SimpleListener_2.notify ) (NATO:  (US: func ))) (history:  (middle age: SimpleListener_1 func ))'
        print "Publisher tree: ", publisher
        assert str(publisher) == expectTopicTree
    
        publisher.unsubAll(topic1)
        assert publisher.getAssociatedTopics(lisnr1) == [topic2]
        assert not publisher.isSubscribed(lisnr1, topic1)
        
        publisher.unsubAll(topic2)
        print publisher
        assert publisher.getAssociatedTopics(lisnr1) == []
        assert publisher.getAssociatedTopics(lisnr3) == [topic5]
        assert not publisher.isSubscribed(lisnr1)
        assert publisher.isSubscribed(lisnr3, topic5)
        
        #print "Publisher tree: ", publisher
        expectTopicTree = 'all: <lambda>  (politics:  (UN: SimpleListener_2.notify ) (NATO:  (US: func ))) (history:  (middle age: ))'
        assert str(publisher) == expectTopicTree
        publisher.unsubAll(ALL_TOPICS)
        #print "Publisher tree: ", publisher
        expectTopicTree = 'all:  (politics:  (UN: SimpleListener_2.notify ) (NATO:  (US: func ))) (history:  (middle age: ))'
        assert str(publisher) == expectTopicTree
        
        publisher.unsubAll()
        done('testUnsubAll')
    
    testUnsubAll()
    #------------------------
    
    def testSend():
        publisher = Publisher()
        called = []
        
        class TestListener:
            def __init__(self, num):
                self.number = num
            def __call__(self, b): 
                called.append( 'TL%scb' % self.number )
            def notify(self, b):
                called.append( 'TL%sm' % self.number )
        def funcListener(b):
            called.append('func')
            
        lisnr1 = TestListener(1)
        lisnr2 = TestListener(2)
        lisnr3 = funcListener
        lisnr4 = lambda x: called.append('lambda')

        topic1 = 'politics'
        topic2 = 'history'
        topic3 = ('politics','UN')
        topic4 = ('politics','NATO','US')
        topic5 = ('politics','NATO')
        
        publisher.subscribe(lisnr1, topic1)
        publisher.subscribe(lisnr2, topic2)
        publisher.subscribe(lisnr2.notify, topic2)
        publisher.subscribe(lisnr3, topic4)
        publisher.subscribe(lisnr4)
        
        print publisher
        
        # setup ok, now test send/receipt
        publisher.sendMessage(topic1)
        assert called == ['lambda','TL1cb']
        called = []
        publisher.sendMessage(topic2)
        assert called == ['lambda','TL2cb','TL2m']
        called = []
        publisher.sendMessage(topic3)
        assert called == ['lambda','TL1cb']
        called = []
        publisher.sendMessage(topic4)
        assert called == ['lambda','TL1cb','func']
        called = []
        publisher.sendMessage(topic5)
        assert called == ['lambda','TL1cb']
        assert publisher.getDeliveryCount() == 12
        assert publisher.getMessageCount() == 5
    
        # test weak referencing works:
        _NodeCallback.notified = 0
        del lisnr2
        called = []
        publisher.sendMessage(topic2)
        assert called == ['lambda']
        assert _NodeCallback.notified == 2
        
        done('testSend')
        
    testSend()
    assert _NodeCallback.notified == 5
    
    def testDead():
        # verify if weak references work as expected
        print '------ Starting testDead ----------'
        node = _TopicTreeNode('t1', None)
        lisnr1 = SimpleListener(1)
        lisnr2 = SimpleListener(2)
        lisnr3 = SimpleListener(3)
        lisnr4 = SimpleListener(4)

        node.addCallable(lisnr1)
        node.addCallable(lisnr2)
        node.addCallable(lisnr3)
        node.addCallable(lisnr4)
        
        print 'Deleting listeners first'
        _NodeCallback.notified = 0
        del lisnr1
        del lisnr2
        assert _NodeCallback.notified == 2
        
        print 'Deleting node first'
        _NodeCallback.notified = 0
        del node
        del lisnr3
        del lisnr4
        assert _NodeCallback.notified == 0
        
        lisnr1 = SimpleListener(1)
        lisnr2 = SimpleListener(2)
        lisnr3 = SimpleListener(3)
        lisnr4 = SimpleListener(4)
        
        # try same with root of tree
        node = _TopicTreeRoot()
        node.addTopic(('',), lisnr1)
        node.addTopic(('',), lisnr2)
        node.addTopic(('',), lisnr3)
        node.addTopic(('',), lisnr4)
        # add objects that will die immediately to see if cleanup occurs
        # this must be done visually as it is a low-level detail
        _NodeCallback.notified = 0
        _TopicTreeRoot.callbackDeadLimit = 3
        node.addTopic(('',), SimpleListener(5))
        node.addTopic(('',), SimpleListener(6))
        node.addTopic(('',), SimpleListener(7))
        print node.numListeners()
        assert node.numListeners() == (4, 3)
        node.addTopic(('',), SimpleListener(8))
        assert node.numListeners() == (4, 0)
        assert _NodeCallback.notified == 4
        
        print 'Deleting listeners first'
        _NodeCallback.notified = 0
        del lisnr1
        del lisnr2
        assert _NodeCallback.notified == 2
        print 'Deleting node first'
        _NodeCallback.notified = 0
        del node
        del lisnr3
        del lisnr4
        assert _NodeCallback.notified == 0
        
        done('testDead')
    
    testDead()
    
    print 'Exiting tests'
#---------------------------------------------------------------------------

if __name__ == '__main__':
    test()
