#!/usr/bin/env python

"""
This demo attempts to override the C++ MainLoop and implement it
in Python.
"""

import time
import wx                  

#---------------------------------------------------------------------------

class MyFrame(wx.Frame):

    def __init__(self, parent, id, title):
        wx.Frame.__init__(self, parent, id, title,
                         (100, 100), (160, 150))

        self.Bind(wx.EVT_SIZE, self.OnSize)
        self.Bind(wx.EVT_MOVE, self.OnMove)
        self.Bind(wx.EVT_CLOSE, self.OnCloseWindow)
        self.Bind(wx.EVT_IDLE, self.OnIdle)

        self.count = 0

        panel = wx.Panel(self)
        sizer = wx.FlexGridSizer(cols=2, hgap=5, vgap=5)
        
        self.sizeCtrl = wx.TextCtrl(panel, -1, "", style=wx.TE_READONLY)
        sizer.Add(wx.StaticText(panel, -1, "Size:"))
        sizer.Add(self.sizeCtrl)

        self.posCtrl = wx.TextCtrl(panel, -1, "", style=wx.TE_READONLY)
        sizer.Add(wx.StaticText(panel, -1, "Pos:"))
        sizer.Add(self.posCtrl)

        self.idleCtrl = wx.TextCtrl(panel, -1, "", style=wx.TE_READONLY)
        sizer.Add(wx.StaticText(panel, -1, "Idle:"))
        sizer.Add(self.idleCtrl)

        border = wx.BoxSizer()
        border.Add(sizer, 0, wx.ALL, 20)
        panel.SetSizer(border)
        

    def OnCloseWindow(self, event):
        app.keepGoing = False
        self.Destroy()

    def OnIdle(self, event):
        self.idleCtrl.SetValue(str(self.count))
        self.count = self.count + 1

    def OnSize(self, event):
        size = event.GetSize()
        self.sizeCtrl.SetValue("%s, %s" % (size.width, size.height))
        event.Skip()

    def OnMove(self, event):
        pos = event.GetPosition()
        self.posCtrl.SetValue("%s, %s" % (pos.x, pos.y))



#---------------------------------------------------------------------------

class MyApp(wx.App):
    def MainLoop(self):

        # Create an event loop and make it active.  If you are
        # only going to temporarily have a nested event loop then
        # you should get a reference to the old one and set it as
        # the active event loop when you are done with this one...
        evtloop = wx.EventLoop()
        old = wx.EventLoop.GetActive()
        wx.EventLoop.SetActive(evtloop)

        # This outer loop determines when to exit the application,
        # for this example we let the main frame reset this flag
        # when it closes.
        while self.keepGoing:
            # At this point in the outer loop you could do
            # whatever you implemented your own MainLoop for.  It
            # should be quick and non-blocking, otherwise your GUI
            # will freeze.  

            # call_your_code_here()


            # This inner loop will process any GUI events
            # until there are no more waiting.
            while evtloop.Pending():
                evtloop.Dispatch()

            # Send idle events to idle handlers.  You may want to
            # throttle this back a bit somehow so there is not too
            # much CPU time spent in the idle handlers.  For this
            # example, I'll just snooze a little...
            time.sleep(0.10)
            self.ProcessIdle()

        wx.EventLoop.SetActive(old)



    def OnInit(self):
        frame = MyFrame(None, -1, "This is a test")
        frame.Show(True)
        self.SetTopWindow(frame)

        self.keepGoing = True
        return True


app = MyApp(False)
app.MainLoop()





