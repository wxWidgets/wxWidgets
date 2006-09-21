"""
This demonstrates a simple use of delayedresult: get/compute 
something that takes a long time, without hanging the GUI while this
is taking place. 

The top button runs a small GUI that uses wx.lib.delayedresult.startWorker
to wrap a long-running function into a separate thread. Just click
Get, and move the slider, and click Get and Abort a few times, and
observe that GUI responds. The key functions to look for in the code
are startWorker() and __handleResult().

The second button runs the same GUI, but without delayedresult.  Click
Get: now the get/compute is taking place in main thread, so the GUI
does not respond to user actions until worker function returns, it's
not even possible to Abort.
"""

import wx
import wx.lib.delayedresult as delayedresult


class FrameSimpleDelayedBase(wx.Frame):
    def __init__(self, *args, **kwds):
        wx.Frame.__init__(self, *args, **kwds)
        pnl = wx.Panel(self)
        self.checkboxUseDelayed = wx.CheckBox(pnl, -1, "Using delayedresult")
        self.buttonGet = wx.Button(pnl, -1, "Get")
        self.buttonAbort = wx.Button(pnl, -1, "Abort")
        self.slider = wx.Slider(pnl, -1, 0, 0, 10, size=(100,-1),
                                style=wx.SL_HORIZONTAL|wx.SL_AUTOTICKS)
        self.textCtrlResult = wx.TextCtrl(pnl, -1, "", style=wx.TE_READONLY)

        self.checkboxUseDelayed.SetValue(1)
        self.checkboxUseDelayed.Enable(False)
        self.buttonAbort.Enable(False)

        vsizer = wx.BoxSizer(wx.VERTICAL)
        hsizer = wx.BoxSizer(wx.HORIZONTAL)
        vsizer.Add(self.checkboxUseDelayed, 0, wx.ALL, 10)
        hsizer.Add(self.buttonGet, 0, wx.ALL, 5)
        hsizer.Add(self.buttonAbort, 0, wx.ALL, 5)
        hsizer.Add(self.slider, 0, wx.ALL, 5)
        hsizer.Add(self.textCtrlResult, 0, wx.ALL, 5)
        vsizer.Add(hsizer, 0, wx.ALL, 5)
        pnl.SetSizer(vsizer)
        vsizer.SetSizeHints(self)
        
        self.Bind(wx.EVT_BUTTON, self.handleGet, self.buttonGet)
        self.Bind(wx.EVT_BUTTON, self.handleAbort, self.buttonAbort)




class FrameSimpleDelayed(FrameSimpleDelayedBase):
    """This demos simplistic use of delayedresult module."""
    
    def __init__(self, *args, **kwargs):
        FrameSimpleDelayedBase.__init__(self, *args, **kwargs)
        self.jobID = 0
        self.abortEvent = delayedresult.AbortEvent()
        self.Bind(wx.EVT_CLOSE, self.handleClose)

    def setLog(self, log):
        self.log = log

    def handleClose(self, event):
        """Only needed because in demo, closing the window does not kill the 
        app, so worker thread continues and sends result to dead frame; normally
        your app would exit so this would not happen."""
        if self.buttonAbort.IsEnabled():
            self.log( "Exiting: Aborting job %s" % self.jobID )
            self.abortEvent.set()
        self.Destroy()
            
    def handleGet(self, event): 
        """Compute result in separate thread, doesn't affect GUI response."""
        self.buttonGet.Enable(False)
        self.buttonAbort.Enable(True)
        self.abortEvent.clear()
        self.jobID += 1
        
        self.log( "Starting job %s in producer thread: GUI remains responsive"
                  % self.jobID )
        delayedresult.startWorker(self._resultConsumer, self._resultProducer, 
                                  wargs=(self.jobID,self.abortEvent), jobID=self.jobID)

                        
    def _resultProducer(self, jobID, abortEvent):
        """Pretend to be a complex worker function or something that takes 
        long time to run due to network access etc. GUI will freeze if this 
        method is not called in separate thread."""
        import time
        count = 0
        while not abortEvent() and count < 50:
            time.sleep(0.1)
            count += 1
        return jobID


    def handleAbort(self, event): 
        """Abort the result computation."""
        self.log( "Aborting result for job %s" % self.jobID )
        self.buttonGet.Enable(True)
        self.buttonAbort.Enable(False)
        self.abortEvent.set()

        
    def _resultConsumer(self, delayedResult):
        jobID = delayedResult.getJobID()
        assert jobID == self.jobID
        try:
            result = delayedResult.get()
        except Exception, exc:
            self.log( "Result for job %s raised exception: %s" % (jobID, exc) )
            return
        
        # output result
        self.log( "Got result for job %s: %s" % (jobID, result) )
        self.textCtrlResult.SetValue(str(result))
        
        # get ready for next job:
        self.buttonGet.Enable(True)
        self.buttonAbort.Enable(False)


class FrameSimpleDirect(FrameSimpleDelayedBase):
    """This does not use delayedresult so the GUI will freeze while
    the GET is taking place."""
    
    def __init__(self, *args, **kwargs):
        self.jobID = 1
        FrameSimpleDelayedBase.__init__(self, *args, **kwargs)
        self.checkboxUseDelayed.SetValue(False)
                
    def setLog(self, log):
        self.log = log
        
    def handleGet(self, event): 
        """Use delayedresult, this will compute result in separate
        thread, and will affect GUI response because a thread is not
        used."""
        self.buttonGet.Enable(False)
        self.buttonAbort.Enable(True)

        self.log( "Doing job %s without delayedresult (same as GUI thread): GUI hangs (for a while)" % self.jobID )
        result = self._resultProducer(self.jobID)
        self._resultConsumer( result )

    def _resultProducer(self, jobID):
        """Pretend to be a complex worker function or something that takes 
        long time to run due to network access etc. GUI will freeze if this 
        method is not called in separate thread."""
        import time
        time.sleep(5)
        return jobID

    def handleAbort(self, event):
        """can never be called"""
        pass
        
    def _resultConsumer(self, result):
        # output result
        self.log( "Got result for job %s: %s" % (self.jobID, result) )
        self.textCtrlResult.SetValue(str(result))
        
        # get ready for next job:
        self.buttonGet.Enable(True)
        self.buttonAbort.Enable(False)
        self.jobID += 1


#---------------------------------------------------------------------------
#---------------------------------------------------------------------------

class TestPanel(wx.Panel):
    def __init__(self, parent, log):
        self.log = log
        wx.Panel.__init__(self, parent, -1)

        vsizer = wx.BoxSizer(wx.VERTICAL)
        b = wx.Button(self, -1, "Long-running function in separate thread")
        vsizer.Add(b, 0, wx.ALL, 5)
        self.Bind(wx.EVT_BUTTON, self.OnButton1, b)

        b = wx.Button(self, -1, "Long-running function in GUI thread")
        vsizer.Add(b, 0, wx.ALL, 5)
        self.Bind(wx.EVT_BUTTON, self.OnButton2, b)

        bdr = wx.BoxSizer()
        bdr.Add(vsizer, 0, wx.ALL, 50)
        self.SetSizer(bdr)
        self.Layout()

    def OnButton1(self, evt):
        frame = FrameSimpleDelayed(self, title="Long-running function in separate thread")
        frame.setLog(self.log.WriteText)
        frame.Show()

    def OnButton2(self, evt):
        frame = FrameSimpleDirect(self, title="Long-running function in GUI thread")
        frame.setLog(self.log.WriteText)
        frame.Show()
    

#---------------------------------------------------------------------------


def runTest(frame, nb, log):
    win = TestPanel(nb, log)
    return win


#---------------------------------------------------------------------------


overview = __doc__


if __name__ == '__main__':
   import sys,os
   import run
   run.main(['', os.path.basename(sys.argv[0])] + sys.argv[1:])


