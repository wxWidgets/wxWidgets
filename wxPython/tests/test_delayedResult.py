
import wx
import delayedresult as dr


def testStruct():
    ss=dr.Struct(a='a', b='b')
    assert ss.a == 'a'
    assert ss.b == 'b'


def testHandler():
    def handler(b, d, a=None, c=None):
        assert a=='a'
        assert b=='b'
        assert c=='c'
        assert d==1
    hh=dr.Handler(handler, 1, a='a')
    hh('b', c='c')
    
    def handler2(*args, **kwargs):
        assert args[0] == 3
        assert args[1] == 1
        assert kwargs['a'] == 'a'
        assert kwargs['b'] == 'b'
    hh2 = dr.Handler(handler2, 1, a='a')
    args = ()
    hh3 = dr.Handler(hh2, b='b', *args)
    hh3(3)

    
def testSender():
    triplet = (1,'a',2.34)
    b = dr.Struct(called=False, which=1)
    assert not b.called
    def consumer(result, a, b=None):
        assert result.get() == 789 + b.which
        assert result.getJobID() == 456 
        assert a == 'a'
        b.called = True
    handler = dr.Handler(consumer, 'a', **dict(b=b))
    ss = dr.SenderNoWx( handler, jobID=456 )
    ss.sendResult(789+1)
    assert b.called 


def testSendExcept():
    def consumer(result):
        try:
            result.get()
            raise RuntimeError('should have raised!')
        except AssertionError:
            pass
    ss = dr.SenderNoWx( dr.Handler(consumer) )
    ss.sendException( AssertionError('test') )
    
    
def testThread():
    expect = dr.Struct(value=1)
    def consumer(result):
        assert result.getJobID() is None
        assert result.get() == expect.value
        expect.value += 2
    ss = dr.SenderNoWx( dr.Handler(consumer) )
    import time
    def worker(sender=None):
        sender.sendResult(1)
        time.sleep(0.1)
        sender.sendResult(3)
        time.sleep(0.1)
        sender.sendResult(5)
        time.sleep(0.1)
        return 7
    tt = dr.Producer(ss, worker, senderArg='sender')
    tt.start()
    while expect.value < 7:
        time.sleep(0.1)
        print '.'


def testStartWorker():
    print 'Doing worker thread with call-after'
    import time
    def handleButtonClick():
        produce = [123, 456, 789, 012, 345, 678, 901]
        expect = dr.Struct(idx=0)
        def worker(a, b=None, sender=None):
            assert a == 2
            assert b == 'b'
            for val in produce:
                time.sleep(0.5)
                sender.sendResult(val)
        def consumer(result, b, a=None):
            assert b == 1
            assert a=='a'
            result = result.get()
            print 'got result', result
            if expect.idx < len(produce):
                assert result == produce[ expect.idx ]#, 'Expected %s, got %s' % (
            else:
                assert result is None
                app.ExitMainLoop()
            expect.idx += 1
        
        dr.startWorker(consumer, worker, cargs=(1,), ckwargs={'a':'a'}, 
            wargs=(2,), wkwargs={'b':'b'}, senderArg='sender')

    app = wx.PySimpleApp()
    frame = wx.Frame(None) # need this otherwise MainLoop() returns immediately
    # pretend user has clicked: 
    import thread
    thread.start_new_thread( wx.CallAfter, (handleButtonClick,))
    app.MainLoop()


def testStartWorkerEvent():
    print 'Doing same with events'
    import time
    produce = [123, 456, 789, 012, 345, 678, 901]
    expect = dr.Struct(idx=0)
    def worker(a, b=None, sender=None):
        assert a == 2
        assert b == 'b'
        for val in produce:
            time.sleep(0.5)
            sender.sendResult(val)
    def consumer(event):
        assert event.a=='a'
        result = event.result.get()
        print 'got result', result
        if expect.idx < len(produce):
            assert result == produce[ expect.idx ]#, 'Expected %s, got %s' % (
        else:
            assert result is None
            app.ExitMainLoop()
        expect.idx += 1
    def handleButtonClick():
        dr.startWorker(frame, worker, 
            cargs=(eventClass,), ckwargs={'a':'a','resultAttr':'result'}, 
            wargs=(2,), wkwargs={'b':'b'}, senderArg='sender')

    app = wx.PySimpleApp()
    frame = wx.Frame(None) # need this otherwise MainLoop() returns immediately
    from wx.lib.newevent import NewEvent as wxNewEvent
    eventClass, eventBinder = wxNewEvent()
    frame.Bind(eventBinder, consumer)
    # pretend user has clicked: 
    import thread
    thread.start_new_thread( wx.CallAfter, (handleButtonClick,))
    app.MainLoop()


def testAbort():
    import threading
    abort = dr.AbortEvent()
    
    # create a wx app and a function that will cause 
    # app to close when abort occurs
    app = wx.PySimpleApp()
    frame = wx.Frame(None) # need this otherwise MainLoop() returns immediately
    def exiter():
        abort.wait()
        # make sure any events have time to be processed before exit
        wx.FutureCall(2000, app.ExitMainLoop) 
    threading.Thread(target=exiter).start()

    # now do the delayed result computation:
    def worker():
        count = 0
        while not abort(1):
            print 'Result computation not done, not aborted'
        return 'Result computed'
    def consumer(dr): # never gets called but as example
        print 'Got dr=', dr.get()
        app.ExitMainLoop()
    dr.startWorker(consumer, worker)
    
    # pretend user doing other stuff
    import time
    time.sleep(5)
    # pretend user aborts now:
    print 'Setting abort event'
    abort.set()
    app.MainLoop()


def testPreProcChain():
    # test when no chain
    def handler(dr):
        assert dr.getJobID() == 123
        assert dr.get() == 321
    pp=dr.PreProcessChain( handler )
    pp( dr.DelayedResult(321, jobID=123) )
    
    # test with chaining
    def handlerPP(chainTrav, n, a=None):
        print 'In handlerPP'
        assert n==1
        assert a=='a'
        assert chainTrav.getJobID() == 321
        res = chainTrav.get()
        assert res == 135
        print 'Done handlerPP'
        
    def subStart1(handler):
        pp=dr.PreProcessChain(handler)
        pp.addSub(subEnd1, 1, b='b')
        subStart2(pp.clone())
    def subEnd1(chainTrav, aa, b=None):
        print 'In subEnd1'
        assert aa==1
        assert b=='b'
        assert chainTrav.getJobID() == 321
        res = chainTrav.get()
        assert res == 246, 'res=%s' % res
        print 'Returning from subEnd1'
        return res - 111

    def subStart2(preProc):
        preProc.addSub(subEnd2, 3, c='c')
        ss = dr.SenderNoWx(preProc, jobID=321)
        ss.sendResult(123)
    def subEnd2(chainTrav, a, c=None):
        print 'In subEnd2'
        assert a==3
        assert c=='c'
        assert chainTrav.getJobID() == 321
        res = chainTrav.get()
        assert res == 123
        print 'Returning from subEnd2'
        return 123*2

    subStart1( dr.Handler(handlerPP, 1, a='a') )

testStruct()
testHandler()
testSender()
testSendExcept()
testThread()
testStartWorker()
testStartWorkerEvent()
testAbort()
testPreProcChain()

