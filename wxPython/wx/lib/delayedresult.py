"""
This module supports the thread-safe, asynchronous transmission of data 
('delayed results') from a worker (non-GUI) thread to the main thread. Ie you don't 
need to mutex lock any data, the worker thread doesn't wait (or even check) 
for the result to be received, and the main thread doesn't wait for the 
worker thread to send the result. Instead, the consumer will be called 
automatically by the wx app when the worker thread result is available. 

In most cases you just need to use startWorker() with the correct parameters
(your worker function and your 'consumer' in the simplest of cases). The 
only requirement on consumer is that it must accept a DelayedResult instance 
as first arg. 

In the following example, this will call consumer(delayedResult) with the 
return value from workerFn::

    from delayedresult import startWorker
    startWorker(consumer, workerFn)

More advanced uses: 

- The other parameters to startWorker()
- Derive from Producer to override _extraInfo (e.g. to provide traceback info)
- Create your own worker-function-thread wrapper instead of using Producer
- Create your own Handler-like wrapper to pre- or post-process the result 
  (see PreProcessChain)
- Derive from Sender to use your own way of making result hop over the 
  "thread boundary" (from non-main thread to main thread), e.g. using Queue

Thanks to Josiah Carlson for critical feedback/ideas that helped me 
improve this module. 

:Copyright: (c) 2006 by Oliver Schoenborn
:License: wxWidgets license
:Version: 1.0

"""

__author__  = 'Oliver Schoenborn at utoronto dot ca'
__version__ = '1.0'

__all__ = ('Sender', 'SenderNoWx', 'SenderWxEvent', 'SenderCallAfter', 
    'Handler', 'DelayedResult', 'Producer', 'startWorker', 'PreProcessChain')


import wx
import threading


class Struct:
    """
    An object that has attributes built from the dictionary given in 
    constructor. So ss=Struct(a=1, b='b') will satisfy assert ss.a == 1
    and assert ss.b == 'b'.
    """
    
    def __init__(self, **kwargs):
        self.__dict__.update( kwargs )


class Handler:
    """
    Bind some of the arguments and keyword arguments of a callable ('listener'). 
    Then when the Handler instance is called (e.g. handler(result, **kwargs))
    the result is passed as first argument to callable, the kwargs is 
    combined with those given at construction, and the args are those
    given at construction. Its return value is returned.
    """
    def __init__(self, listener, *args, **kwargs ):
        """Bind args and kwargs to listener. """
        self.__listener = listener
        self.__args = args
        self.__kwargs = kwargs
        
    def __call__(self, result, **moreKwargs):
        """Listener is assumed to take result as first arg, then *args, 
        then the combination of moreKwargs and the kwargs given at construction."""
        if moreKwargs:
            moreKwargs.update(self.__kwargs)
        else:
            moreKwargs = self.__kwargs
        return self.__listener(result, *self.__args, **moreKwargs)

        
class Sender:
    """
    Base class for various kinds of senders. A sender sends a result
    produced by a worker funtion to a result handler (listener). Note
    that each sender can be given a "job id". This can be anything
    (number, string, id, and object, etc) and is not used, it is 
    simply added as attribute whenever a DelayedResult is created. 
    This allows you to know, if desired, what result corresponds to 
    which sender. Note that uniqueness is not necessary. 
    
    Derive from this class if none of the existing derived classes
    are adequate, and override _sendImpl(). 
    """
    
    def __init__(self, jobID=None):
        """The optional jobID can be anything that you want to use to 
        track which sender particular results come from. """
        self.__jobID = jobID

    def getJobID(self):
        """Return the jobID given at construction"""
        return self.__jobID
    
    def sendResult(self, result):
        """This will send the result to handler, using whatever 
        technique the derived class uses. """
        delayedResult = DelayedResult(result, jobID=self.__jobID)
        self._sendImpl(delayedResult)

    def sendException(self, exception, extraInfo = None):
        """Use this when the worker function raised an exception.
        The *exception* is the instance of Exception caught. The extraInfo
        could be anything you want (e.g. locals or traceback etc), 
        it will be added to the exception as attribute 'extraInfo'. The
        exception will be raised when DelayedResult.get() is called."""
        assert exception is not None
        delayedResult = DelayedResult(extraInfo, 
            exception=exception, jobID=self.__jobID)
        self._sendImpl(delayedResult)

    def _sendImpl(self, delayedResult):
        msg = '_sendImpl() must be implemented in %s' % self.__class__
        raise NotImplementedError(msg)
        
    
class SenderNoWx( Sender ):
    """
    Sender that works without wx. The results are sent directly, ie
    the consumer will get them "in the worker thread". So it should 
    only be used for testing. 
    """
    def __init__(self, consumer, jobID=None, args=(), kwargs={}):
        """The consumer can be any callable of the form 
        callable(result, *args, **kwargs)"""
        Sender.__init__(self, jobID)
        if args or kwargs:
            self.__consumer = Handler(consumer, *args, **kwargs)
        else: 
            self.__consumer = consumer
            
    def _sendImpl(self, delayedResult):
        self.__consumer(delayedResult)
        

class SenderWxEvent( Sender ):
    """
    This sender sends the delayed result produced in the worker thread
    to an event handler in the main thread, via a wx event of class 
    *eventClass*. The result is an attribute of the event (default: 
    "delayedResult". 
    """
    def __init__(self, handler, eventClass, resultAttr="delayedResult", 
        jobID=None, **kwargs):
        """The handler must derive from wx.EvtHandler. The event class 
        is typically the first item in the pair returned by 
        wx.lib.newevent.NewEvent(). You can use the *resultAttr* 
        to change the attribute name of the generated event's 
        delayed result. """
        Sender.__init__(self, jobID)
        if not isinstance(handler, wx.EvtHandler):
            msg = 'SenderWxEvent(handler=%s, ...) not allowed,' % type(handler)
            msg = '%s handler must derive from wx.EvtHandler' % msg
            raise ValueError(msg)
        self.__consumer = Struct(handler=handler, eventClass=eventClass, 
                                 resultAttr=resultAttr, kwargs=kwargs)
        
    def _sendImpl(self, delayedResult):
        """Must not modify the consumer (that was created at construction) 
        since might be shared by several senders, each sending from 
        separate threads."""
        consumer = self.__consumer
        kwargs = consumer.kwargs.copy()
        kwargs[ consumer.resultAttr ] = delayedResult
        event = consumer.eventClass(** kwargs)
        wx.PostEvent(consumer.handler, event)


class SenderCallAfter( Sender ):
    """
    This sender sends the delayed result produced in the worker thread
    to a callable in the main thread, via wx.CallAfter. 
    """
    def __init__(self, listener, jobID=None, args=(), kwargs={}):
        Sender.__init__(self, jobID)
        if args or kwargs:
            self.__consumer = Handler(listener, *args, **kwargs)
        else:
            self.__consumer = listener
            
    def _sendImpl(self, delayedResult):
        wx.CallAfter(self.__consumer, delayedResult)
        

class DelayedResult:
    """
    Represent the actual delayed result coming from the non-main thread. 
    An instance of this is given to the result handler. This result is 
    either a (reference to a) the value sent, or an exception. 
    If the latter, the exception is raised when the get() method gets
    called. 
    """
    
    def __init__(self, result, jobID=None, exception = None):
        """You should never have to call this yourself. A DelayedResult 
        is created by a concrete Sender for you."""
        self.__result = result
        self.__exception = exception
        self.__jobID = jobID

    def getJobID(self):
        """Return the jobID given when Sender initialized, 
        or None if none given. """
        return self.__jobID 
    
    def get(self):
        """Get the result. If an exception was sent instead of a result, 
        (via Sender's sendExcept()), that **exception is raised**.
        Otherwise the result is simply returned. """
        if self.__exception: # exception was raised!
            self.__exception.extraInfo = self.__result
            raise self.__exception
        
        return self.__result


class AbortedException(Exception):
    """Raise this in your worker function so that the sender knows 
    not to send a result to handler."""
    pass
    

class Producer(threading.Thread):
    """
    Represent the worker thread that produces delayed results. 
    It causes the given function to run in a separate thread, 
    and a sender to be used to send the return value of the function.
    As with any threading.Thread, instantiate and call start().
    Note that if the workerFn raises AbortedException, the result is not 
    sent and the thread terminates gracefully.
    """
    
    def __init__(self, sender, workerFn, args=(), kwargs={}, 
                 name=None, group=None, daemon=False, 
                 sendReturn=True, senderArg=None):
        """The sender will send the return value of 
        workerFn(*args, **kwargs) to the main thread. The name and group 
        are same as threading.Thread constructor parameters. Daemon causes 
        setDaemon() to be called. If sendReturn is False, then the return 
        value of workerFn() will not be sent. If senderArg is given, it 
        must be the name of the keyword arg to use to pass the sender into 
        the workerFn, so the function can send (typically many) results."""
        if senderArg:
            kwargs[senderArg] = sender
        def wrapper():
            try: 
                result = workerFn(*args, **kwargs)
            except AbortedException:
                pass
            except Exception, exc:
                extraInfo = self._extraInfo(exc)
                sender.sendException(exc, extraInfo)
            else:
                if sendReturn:
                    sender.sendResult(result)
            
        threading.Thread.__init__(self, name=name, group=group, target=wrapper)
        if daemon:
            self.setDaemon(daemon)
    
    def _extraInfo(self, exception):
        """This method could be overridden in a derived class to provide 
        extra information when an exception is being sent instead of a 
        result. """
        return None


class AbortEvent:
    """
    Convenience class that represents a kind of threading.Event that
    raises AbortedException when called (see the __call__ method, everything
    else is just to make it look like threading.Event).
    """
    
    def __init__(self):
        self.__ev = threading.Event()

    def __call__(self, timeout=None):
        """See if event has been set (wait at most timeout if given).  If so, 
        raise AbortedException. Otherwise return None. Allows you to do
        'while not event():' which will always succeed unless the event 
        has been set (then AbortedException will cause while to exit)."""
        if timeout:
            self.__ev.wait(timeout)
        if self.__ev.isSet():
            raise AbortedException()
        return None
    
    def __getattr__(self, name):
        """This allows us to be a kind of threading.Event."""
        if name in ('set','clear','wait','isSet'):
            return getattr(self.__ev, name)


def startWorker(
    consumer, workerFn, 
    cargs=(), ckwargs={}, 
    wargs=(), wkwargs={},
    jobID=None, group=None, daemon=False, 
    sendReturn=True, senderArg=None):
    """
    Convenience function to send data produced by workerFn(*wargs, **wkwargs) 
    running in separate thread, to a consumer(*cargs, **ckwargs) running in
    the main thread. This function merely creates a SenderCallAfter (or a
    SenderWxEvent, if consumer derives from wx.EvtHandler), and a Producer,
    and returns immediately after starting the Producer thread. The jobID
    is used for the Sender and as name for the Producer thread. Returns the 
    thread created, in case caller needs join/etc.
    """
    
    if isinstance(consumer, wx.EvtHandler):
        eventClass = cargs[0]
        sender = SenderWxEvent(consumer, eventClass, jobID=jobID, **ckwargs)
    else:
        sender = SenderCallAfter(consumer, jobID, args=cargs, kwargs=ckwargs)
        
    thread = Producer(
        sender, workerFn, args=wargs, kwargs=wkwargs, 
        name=jobID, group=group, daemon=daemon, 
        senderArg=senderArg, sendReturn=sendReturn)
        
    thread.start() 
    return thread


class PreProcessChain:
    """
    Represent a 'delayed result pre-processing chain', a kind of Handler. 
    Useful when lower-level objects need to apply a sequence of transformations 
    to the delayed result before handing it over to a final handler. 
    This allows the starter of the worker function to not know 
    anything about the lower-level objects. 
    """
    def __init__(self, handler, *args, **kwargs):
        """Wrap handler(result, *args, **kwargs) so that the result 
        it receives has been transformed by us. """
        if handler is None:# assume rhs is a chain
            self.__chain = args[0]
        else:
            if args or kwargs:
                handler = Handler(handler, *args, **kwargs)
            self.__chain = [handler]

    def addSub(self, callable, *args, **kwargs):
        """Add a sub-callable, ie a callable(result, *args, **kwargs)
        that returns a transformed result to the previously added
        sub-callable (or the handler given at construction, if this is 
        the first call to addSub). """
        self.__chain.append( Handler(callable, *args, **kwargs) )
        
    def clone(self):
        """Clone the chain. Shallow only. Useful when several threads 
        must be started but have different sub-callables. """
        return PreProcessChain(None, self.__chain[:] )
    
    def cloneAddSub(self, callable, *args, **kwargs):
        """Convenience method that first clones self, then calls addSub() 
        on that clone with given arguments. """
        cc = self.clone()
        cc.addSub(callable, *args, **kwargs)
        
    def count(self):
        """How many pre-processors in the chain"""
        return len(self.__chain)
    
    class Traverser:
        """
        Traverses the chain of pre-processors it is given, transforming
        the original delayedResult along the way. The return value of each 
        callable added via addSub() is given to the previous addSub() callable,
        until the handler is reached. 
        """
        def __init__(self, delayedResult, chain):
            self.__dr = delayedResult
            self.__chain = chain
            
        def get(self):
            """This makes handler think we are a delayedResult."""
            if not self.__chain:
                return self.__dr.get()
            
            handler = self.__chain[0]
            del self.__chain[0]
            return handler(self)
        
        def getJobID(self):
            """Return the job id for the delayedResult we transform."""
            return self.__dr.getJobID()


    def __call__(self, delayedResult):
        """This makes us a Handler. We just call handler(Traverser). The
        handler will think it is getting a delayed result, but in fact 
        will be getting an instance of Traverser, which will take care
        of properly applying the chain of transformations to delayedResult."""
        chainTrav = self.Traverser(delayedResult, self.__chain[1:])
        handler = self.__chain[0]
        handler( chainTrav )
        
