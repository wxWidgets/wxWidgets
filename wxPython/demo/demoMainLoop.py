#!/usr/bin/env python
#---------------------------------------------------------------------------
# 11/9/2003 - Jeff Grimmett (grimmtooth@softhome.net
#
# o Updated for V2.5
# o Mainloop is freezing up app.
#

"""
This demo attempts to override the C++ MainLoop and implement it
in Python.  This is not part of the demo framework.


                THIS FEATURE IS STILL EXPERIMENTAL...
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

        panel = wx.Panel(self, -1)
        wx.StaticText(panel, -1, "Size:",
                     wx.DLG_PNT(panel, (4, 4)),  wx.DefaultSize)
        wx.StaticText(panel, -1, "Pos:",
                     wx.DLG_PNT(panel, (4, 16)), wx.DefaultSize)

        wx.StaticText(panel, -1, "Idle:",
                     wx.DLG_PNT(panel, (4, 28)), wx.DefaultSize)

        self.sizeCtrl = wx.TextCtrl(panel, -1, "",
                                   wx.DLG_PNT(panel, (24, 4)),
                                   wx.DLG_SZE(panel, (36, -1)),
                                   wx.TE_READONLY)

        self.posCtrl = wx.TextCtrl(panel, -1, "",
                                  wx.DLG_PNT(panel, (24, 16)),
                                  wx.DLG_SZE(panel, (36, -1)),
                                  wx.TE_READONLY)

        self.idleCtrl = wx.TextCtrl(panel, -1, "",
                                   wx.DLG_PNT(panel, (24, 28)),
                                   wx.DLG_SZE(panel, (36, -1)),
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

            # Send idle events to idle handlers.  You may want to throttle
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


app = MyApp(False)
app.MainLoop()





