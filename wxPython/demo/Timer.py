import  time
import  wx
import  wx.lib.scrolledpanel as sp

#----------------------------------------------------------------------


header = """\
This demo shows the various ways that wx.Timers can be used in your code.  Just
select one of the buttons in the left column to start a timer in the indicated way,
and watch the log window below for messages printed when the timer event or callback
happens.  Clicking the corresponding button on the right will stop that timer.  Since
timers are not owned by any other wx object you should hold on to a reference to the
timer until you are completely finished with it. """

doc1 = """\
Binding an event handler to the wx.EVT_TIMER event is the
prefered way to use the wx.Timer class directly.  It makes
handling timer events work just like normal window events.  You
just need to specify the window that is to receive the event in
the wx.Timer constructor.  If that window needs to be able to
receive events from more than one timer then you can optionally
specify an ID for the timer and the event binding.
"""


doc2 = """\
wx.CallLater is a convenience class for wx.Timer.  You just
specify the timeout in milliseconds and a callable object, along
with any args or keyword args you would like to be passed to your
callable, and wx.CallLater takes care of the rest.  If you don't
need to get the return value of the callable or to restart the
timer then there is no need to hold a reference to this object.
"""


doc3 = """\
If you derive a class from wx.Timer and give it a Notify method
then it will be called when the timer expires.
"""


doc4 = """\
wx.PyTimer is the old way (a kludge that goes back all the way to
the first version of wxPython) to bind a timer directly to a
callable.  You should migrate any code that uses this method to
use EVT_TIMER instead as this may be deprecated in the future.
"""


class TestPanel(sp.ScrolledPanel):
    def __init__(self, parent, log):
        self.log = log
        sp.ScrolledPanel.__init__(self, parent, -1)

        outsideSizer = wx.BoxSizer(wx.VERTICAL)

        text = wx.StaticText(self, -1, "wx.Timer", style=wx.ALIGN_CENTRE)
        text.SetFont(wx.Font(24, wx.SWISS, wx.NORMAL, wx.BOLD, False))
        text.SetSize(text.GetBestSize())
        text.SetForegroundColour(wx.BLUE)
        outsideSizer.Add(text, 0, wx.EXPAND|wx.ALL, 5)
        outsideSizer.Add(wx.StaticText(self, -1, header), 0, wx.ALIGN_CENTER|wx.ALL, 5)
        outsideSizer.Add(wx.StaticLine(self, -1), 0, wx.EXPAND)
        outsideSizer.Add((20,20))


        t1b1 = wx.Button(self, -1, "EVT_TIMER")
        t1b2 = wx.Button(self, -1, "stop timer")
        t1st = wx.StaticText(self, -1, doc1)
        t1b2.Disable()
        self.Bind(wx.EVT_BUTTON, self.OnTest1Start, t1b1)
        self.Bind(wx.EVT_BUTTON, self.OnTest1Stop,  t1b2)

        # Bind all EVT_TIMER events to self.OnTest1Timer
        self.Bind(wx.EVT_TIMER, self.OnTest1Timer)

        
        t2b1 = wx.Button(self, -1, "wx.CallLater")
        t2b2 = wx.Button(self, -1, "stop timer")
        t2st = wx.StaticText(self, -1, doc2)
        t2b2.Disable()
        self.Bind(wx.EVT_BUTTON, self.OnTest2Start, t2b1)
        self.Bind(wx.EVT_BUTTON, self.OnTest2Stop,  t2b2)

        t3b1 = wx.Button(self, -1, "self.Notify")
        t3b2 = wx.Button(self, -1, "stop timer")
        t3st = wx.StaticText(self, -1, doc3)
        t3b2.Disable()
        self.Bind(wx.EVT_BUTTON, self.OnTest3Start, t3b1)
        self.Bind(wx.EVT_BUTTON, self.OnTest3Stop,  t3b2)

        t4b1 = wx.Button(self, -1, "wx.PyTimer")
        t4b2 = wx.Button(self, -1, "stop timer")
        t4st = wx.StaticText(self, -1, doc4)
        t4b2.Disable()
        self.Bind(wx.EVT_BUTTON, self.OnTest4Start, t4b1)
        self.Bind(wx.EVT_BUTTON, self.OnTest4Stop,  t4b2)


        self.t1b2 = t1b2
        self.t2b2 = t2b2
        self.t3b2 = t3b2
        self.t4b2 = t4b2

        fgs = wx.FlexGridSizer(cols=3, hgap=10, vgap=10)
        fgs.Add(t1b1)
        fgs.Add(t1b2)
        fgs.Add(t1st)

        fgs.Add(t2b1)
        fgs.Add(t2b2)
        fgs.Add(t2st)

        fgs.Add(t3b1)
        fgs.Add(t3b2)
        fgs.Add(t3st)

        fgs.Add(t4b1)
        fgs.Add(t4b2)
        fgs.Add(t4st)
        
        outsideSizer.Add(fgs, 0, wx.ALIGN_CENTER|wx.ALL, 10)
        self.SetSizer(outsideSizer)
        self.SetupScrolling()
        

    # Test 1 shows how to use a timer to generate EVT_TIMER
    # events, by passing self to the wx.Timer constructor.  The
    # event is bound above to the OnTest1Timer method.
    
    def OnTest1Start(self, evt):
        self.t1 = wx.Timer(self)
        self.t1.Start(1000)
        self.log.write("EVT_TIMER timer started\n")
        self.t1b2.Enable()

    def OnTest1Stop(self, evt):
        self.t1.Stop()
        self.log.write("EVT_TIMER timer stoped\n")
        del self.t1
        self.t1b2.Disable()

    def OnTest1Timer(self, evt):
        self.log.write("got EVT_TIMER event\n")
    


    # Test 2 shows how to use the wx.CallLater class.
    
    def OnTest2Start(self, evt):
        # Call OnTest2Timer one second in the future, passing some
        # optional arbitrary args.  There is no need to hold a
        # reference to this one, unless we want to manipulate or query
        # it later like we do in the two methods below
        self.t2 = wx.CallLater(1000, self.OnTest2Timer,
                                'a', 'b', 'c', one=1, two=2)
        self.log.write("CallLater scheduled\n")
        self.t2b2.Enable()
        
    def OnTest2Stop(self, evt):
        self.t2.Stop()
        self.log.write("CallLater stopped, last return value was: %s\n" %
                       repr(self.t2.GetResult()))
        del self.t2
        self.t2b2.Disable()
           
    def OnTest2Timer(self, *args, **kw):
        self.log.write("CallLater called with args=%s, kwargs=%s\n" % (args, kw))

        # Normally a FutureCall is one-shot, but we can make it
        # recurring just by calling Restart.  We can even use a
        # different timeout or pass differnt args this time.
        self.t2.Restart(1500, "restarted")

        # The return value of this function is saved and can be
        # retrived later.  See OnTest2Stop above.
        return "This is my return value"



    # Test 3 shows how to use a class derived from wx.Timer.  See
    # also the NotifyTimer class below.
    
    def OnTest3Start(self, evt):
        self.t3 = NotifyTimer(self.log)
        self.t3.Start(1000)
        self.log.write("NotifyTimer timer started\n")
        self.t3b2.Enable()

    def OnTest3Stop(self, evt):
        self.t3.Stop()
        self.log.write("NotifyTimer timer stoped\n")
        del self.t3
        self.t3b2.Disable()



    # Test 4 shows the old way (a kludge that goes back all the
    # way to the first version of wxPython) to bind a timer
    # directly to a callable.  You should migrate any code that
    # uses this method to use EVT_TIMER instead as this may be
    # deprecated in the future.
    def OnTest4Start(self, evt):
        self.t4 = wx.PyTimer(self.OnTest4Timer)
        self.t4.Start(1000)
        self.log.write("wx.PyTimer timer started\n")
        self.t4b2.Enable()

    def OnTest4Stop(self, evt):
        self.t4.Stop()
        self.log.write("wx.PyTimer timer stoped\n")
        del self.t4
        self.t4b2.Disable()

    def OnTest4Timer(self):
        self.log.write("got wx.PyTimer event\n")
    


#----------------------------------------------------------------------


# When deriving from wx.Timer you must provide a Notify method
# that will be called when the timer expires.
class NotifyTimer(wx.Timer):
    def __init__(self, log):
        wx.Timer.__init__(self)
        self.log = log
        
    def Notify(self):
        self.log.write("got NotifyTimer event\n")



#----------------------------------------------------------------------

def runTest(frame, nb, log):
    win = TestPanel(nb, log)
    return win


#----------------------------------------------------------------------

overview = """<html><body>
<h2><center>wx.Timer</center></h2>

The wx.Timer class allows you to execute code at specified intervals
from within the wxPython event loop. Timers can be one-shot or
repeating.  This demo shows the principle method of using a timer
(with events) as well as the convenient wx.FutureCall class.  Also
there are two other usage patterns shown here that have been preserved
for backwards compatibility.

</body></html>
"""



if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])] + sys.argv[1:])




