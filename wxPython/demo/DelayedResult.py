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
from wx.lib.delayedresult import startWorker

class FrameSimpleDelayedGlade(wx.Frame):
    def __init__(self, *args, **kwds):
        # begin wxGlade: FrameSimpleDelayed.__init__
        kwds["style"] = wx.DEFAULT_FRAME_STYLE
        wx.Frame.__init__(self, *args, **kwds)
        self.checkboxUseDelayed = wx.CheckBox(self, -1, "Use delayedresult")
        self.buttonGet = wx.Button(self, -1, "Get")
        self.buttonAbort = wx.Button(self, -1, "Abort")
        self.slider = wx.Slider(self, -1, 0, 0, 10, size=(100,-1), style=wx.SL_HORIZONTAL|wx.SL_AUTOTICKS)
        self.textCtrlResult = wx.TextCtrl(self, -1, "", style=wx.TE_READONLY)

        self.__set_properties()
        self.__do_layout()

        self.Bind(wx.EVT_BUTTON, self.handleGet, self.buttonGet)
        self.Bind(wx.EVT_BUTTON, self.handleAbort, self.buttonAbort)
        # end wxGlade

    def __set_properties(self):
        # begin wxGlade: FrameSimpleDelayed.__set_properties
        self.SetTitle("Simple Examle of Delayed Result")
        self.checkboxUseDelayed.SetValue(1)
        self.checkboxUseDelayed.Enable(False)
        self.buttonAbort.Enable(False)
        # end wxGlade

    def __do_layout(self):
        # begin wxGlade: FrameSimpleDelayed.__do_layout
        sizerFrame = wx.BoxSizer(wx.VERTICAL)
        sizerGetResult = wx.BoxSizer(wx.HORIZONTAL)
        sizerUseDelayed = wx.BoxSizer(wx.HORIZONTAL)
        sizerUseDelayed.Add(self.checkboxUseDelayed, 0, wx.LEFT|wx.ALIGN_CENTER_VERTICAL|wx.ADJUST_MINSIZE, 5)
        sizerFrame.Add(sizerUseDelayed, 1, wx.EXPAND, 0)
        sizerGetResult.Add(self.buttonGet, 0, wx.ADJUST_MINSIZE, 0)
        sizerGetResult.Add(self.buttonAbort, 0, wx.ADJUST_MINSIZE, 0)
        sizerGetResult.Add(self.slider, 0, wx.ADJUST_MINSIZE, 0)
        sizerGetResult.Add(self.textCtrlResult, 0, wx.ADJUST_MINSIZE, 0)
        sizerFrame.Add(sizerGetResult, 1, wx.ALL|wx.EXPAND, 5)
        self.SetAutoLayout(True)
        self.SetSizer(sizerFrame)
        sizerFrame.Fit(self)
        sizerFrame.SetSizeHints(self)
        self.Layout()
        # end wxGlade


class FrameSimpleDelayed(FrameSimpleDelayedGlade):
    """This demos simplistic use of delayedresult module."""
    
    def __init__(self, *args, **kwargs):
        self.jobID = 1
        FrameSimpleDelayedGlade.__init__(self, *args, **kwargs)
        self.Bind(wx.EVT_CLOSE, self.handleClose)
    
    def setLog(self, log):
        self.log = log

    def handleClose(self, event):
        """Only needed because in demo, closing the window does not kill the 
        app, so worker thread continues and sends result to dead frame; normally
        your app would exit so this would not happen."""
        if self.buttonAbort.IsEnabled():
            self.Hide()
            import time
            time.sleep(5)
        self.Destroy()
            
    def handleGet(self, event): 
        """Compute result in separate thread, doesn't affect GUI response."""
        self.buttonGet.Enable(False)
        self.buttonAbort.Enable(True)

        self.log( "Starting job %s in producer thread: GUI remains responsive" % self.jobID )
        startWorker(self.__handleResult, self.__resultCreator, 
                    wargs=(self.jobID,), jobID=self.jobID)
                        
    def __resultCreator(self, jobID):
        """Pretend to be a complex worker function or something that takes 
        long time to run due to network access etc. GUI will freeze if this 
        method is not called in separate thread."""
        import time
        time.sleep(5)
        return jobID

    def handleAbort(self, event): 
        """Abort actually just means 'ignore the result when it gets to 
        handler, it is no longer relevant'. We just increase the job ID, 
        this will let handler know that the result has been cancelled."""
        self.log( "Aborting result for job %s" % self.jobID )
        self.buttonGet.Enable(True)
        self.buttonAbort.Enable(False)
        self.jobID += 1
        
    def __handleResult(self, delayedResult):
        # See if we still want the result for last job started
        jobID = delayedResult.getJobID()
        if jobID != self.jobID:
            self.log( "Got obsolete result for job %s, ignored" % jobID )
            return

        # we do, get result:
        try:
            result = delayedResult.get()
        except Exception, exc:
            self.log( "Result for job %s raised exception: %s" % (jobID, exc) )
            self.jobID += 1
            return
        
        # output result
        self.log( "Got result for job %s: %s" % (jobID, result) )
        self.textCtrlResult.SetValue(str(result))
        
        # get ready for next job:
        self.buttonGet.Enable(True)
        self.buttonAbort.Enable(False)
        self.jobID += 1


class FrameSimpleDirect(FrameSimpleDelayedGlade):
    """This does not use delayedresult so the GUI will freeze while
    the GET is taking place."""
    
    def __init__(self, *args, **kwargs):
        self.jobID = 1
        FrameSimpleDelayedGlade.__init__(self, *args, **kwargs)
        self.checkboxUseDelayed.SetValue(False)
                
    def setLog(self, log):
        self.log = log
        
    def handleGet(self, event): 
        """Use delayedresult, this will compute
        result in separate thread, and won't affect GUI response. """
        self.buttonGet.Enable(False)
        self.buttonAbort.Enable(True)

        self.log( "Doing job %s without delayedresult (same as GUI thread): GUI hangs (for a while)" % self.jobID )
        result = self.__resultCreator(self.jobID)
        self.__handleResult( result )

    def __resultCreator(self, jobID):
        """Pretend to be a complex worker function or something that takes 
        long time to run due to network access etc. GUI will freeze if this 
        method is not called in separate thread."""
        import time
        time.sleep(5)
        return jobID

    def handleAbort(self, event):
        """can never be called"""
        pass
        
    def __handleResult(self, result):
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


