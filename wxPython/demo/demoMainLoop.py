#!/usr/bin/env python
#---------------------------------------------------------------------------
"""
This demo attempts to override the C++ MainLoop and implement it
in Python.  This is not part of the demo framework.


                THIS FEATURE IS STILL EXPERIMENTAL...
"""


import wx
import time


#---------------------------------------------------------------------------

class MyFrame(wx.Frame):

    def __init__(self, parent, id, title):
        wx.Frame.__init__(self, parent, id, title,
                         wx.Point(100, 100), wx.Size(160, 150))

        wx.EVT_SIZE(self, self.OnSize)
        wx.EVT_MOVE(self, self.OnMove)
        wx.EVT_CLOSE(self, self.OnCloseWindow)
        wx.EVT_IDLE(self, self.OnIdle)

        self.count = 0

        panel = wx.Panel(self, -1)
        wx.StaticText(panel, -1, "Size:",
                     wx.DLG_PNT(panel, wx.Point(4, 4)),  wx.DefaultSize)
        wx.StaticText(panel, -1, "Pos:",
                     wx.DLG_PNT(panel, wx.Point(4, 16)), wx.DefaultSize)

        wx.StaticText(panel, -1, "Idle:",
                     wx.DLG_PNT(panel, wx.Point(4, 28)), wx.DefaultSize)

        self.sizeCtrl = wx.TextCtrl(panel, -1, "",
                                   wx.DLG_PNT(panel, wx.Point(24, 4)),
                                   wx.DLG_SZE(panel, wx.Size(36, -1)),
                                   wx.TE_READONLY)

        self.posCtrl = wx.TextCtrl(panel, -1, "",
                                  wx.DLG_PNT(panel, wx.Point(24, 16)),
                                  wx.DLG_SZE(panel, wx.Size(36, -1)),
                                  wx.TE_READONLY)

        self.idleCtrl = wx.TextCtrl(panel, -1, "",
                                   wx.DLG_PNT(panel, wx.Point(24, 28)),
                                   wx.DLG_SZE(panel, wx.Size(36, -1)),
                                   wx.TE_READONLY)


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
        # This outer loop determines when to exit the application,  for
        # this example we let the main frame reset this flag when it
        # closes.
        while self.keepGoing:
            # At this point in the outer loop you could do whatever you
            # implemented your own MainLoop for.  It should be quick and
            # non-blocking, otherwise your GUI will freeze.  For example,
            # call Fnorb's reactor.do_one_event(0), etc.

            # call_your_code_here()


            # This inner loop will process any GUI events until there
            # are no more waiting.
            while self.Pending():
                self.Dispatch()

            # Send idle events to idle handlers.  You may want to throtle
            # this back a bit so there is not too much CPU time spent in
            # the idle handlers.  For this example, I'll just snooze a
            # little...
            time.sleep(0.25)
            self.ProcessIdle()



    def OnInit(self):
        frame = MyFrame(None, -1, "This is a test")
        frame.Show(True)
        self.SetTopWindow(frame)

        self.keepGoing = True

        return True


app = MyApp(0)
app.MainLoop()





