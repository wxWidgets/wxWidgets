import wx
import threading
import random

class WorkerThread(threading.Thread):
    """
    This just simulates some long-running task that periodically sends
    a message to the GUI thread.
    """
    def __init__(self, threadNum, window):
        threading.Thread.__init__(self)
        self.threadNum = threadNum
        self.window = window
        self.timeToQuit = threading.Event()
        self.timeToQuit.clear()
        self.messageCount = random.randint(10,20)
        self.messageDelay = 0.1 + 2.0 * random.random()

    def stop(self):
        self.timeToQuit.set()

    def run(self):
        msg = "Thread %d iterating %d times with a delay of %1.4f\n" \
              % (self.threadNum, self.messageCount, self.messageDelay)
        wx.CallAfter(self.window.LogMessage, msg)

        for i in range(1, self.messageCount+1):
            self.timeToQuit.wait(self.messageDelay)
            if self.timeToQuit.isSet():
                break
            msg = "Message %d from thread %d\n" % (i, self.threadNum)
            wx.CallAfter(self.window.LogMessage, msg)
        else:
            wx.CallAfter(self.window.ThreadFinished, self)
            
            

class MyFrame(wx.Frame):
    def __init__(self):
        wx.Frame.__init__(self, None, title="Multi-threaded GUI")
        self.threads = []
        self.count = 0
        
        panel = wx.Panel(self)
        startBtn = wx.Button(panel, -1, "Start a thread")
        stopBtn  = wx.Button(panel, -1, "Stop all threads")
        self.tc = wx.StaticText(panel, -1, "Worker Threads: 00")
        self.log = wx.TextCtrl(panel, -1, "",
                               style=wx.TE_RICH|wx.TE_MULTILINE)

        inner = wx.BoxSizer(wx.HORIZONTAL)
        inner.Add(startBtn, 0, wx.RIGHT, 15)
        inner.Add(stopBtn, 0, wx.RIGHT, 15)
        inner.Add(self.tc, 0, wx.ALIGN_CENTER_VERTICAL)
        main = wx.BoxSizer(wx.VERTICAL)
        main.Add(inner, 0, wx.ALL, 5)
        main.Add(self.log, 1, wx.EXPAND|wx.ALL, 5)
        panel.SetSizer(main)

        self.Bind(wx.EVT_BUTTON, self.OnStartButton, startBtn)
        self.Bind(wx.EVT_BUTTON, self.OnStopButton, stopBtn)
        self.Bind(wx.EVT_CLOSE,  self.OnCloseWindow)

        self.UpdateCount()

    def OnStartButton(self, evt):
        self.count += 1
        thread = WorkerThread(self.count, self)
        self.threads.append(thread)
        self.UpdateCount()
        thread.start()
    
    def OnStopButton(self, evt):
        self.StopThreads()
        self.UpdateCount()
        
    def OnCloseWindow(self, evt):
        self.StopThreads()
        self.Destroy()

    def StopThreads(self):
        while self.threads:
            thread = self.threads[0]
            thread.stop()
            self.threads.remove(thread)
            
    def UpdateCount(self):
        self.tc.SetLabel("Worker Threads: %d" % len(self.threads))
        
    def LogMessage(self, msg):
        self.log.AppendText(msg)
        
    def ThreadFinished(self, thread):
        self.threads.remove(thread)
        self.UpdateCount()
        

app = wx.PySimpleApp()
frm = MyFrame()
frm.Show()
app.MainLoop()
