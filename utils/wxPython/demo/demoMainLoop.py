#!/usr/bin/env python
#---------------------------------------------------------------------------
"""
This demo attempts to override the C++ MainLoop and implement it
in Python.  This is not part of the demo framework.


                THIS FEATURE IS STILL EXPERIMENTAL...
"""


from wxPython.wx import *
import time


#---------------------------------------------------------------------------

class MyFrame(wxFrame):

    def __init__(self, parent, id, title):
        wxFrame.__init__(self, parent, id, title,
                         wxPoint(100, 100), wxSize(160, 150))

        EVT_SIZE(self, self.OnSize)
        EVT_MOVE(self, self.OnMove)
        EVT_CLOSE(self, self.OnCloseWindow)
        EVT_IDLE(self, self.OnIdle)

        self.count = 0

        panel = wxPanel(self, -1)
        wxStaticText(panel, -1, "Size:",
                     wxDLG_PNT(panel, wxPoint(4, 4)),  wxDefaultSize)
        wxStaticText(panel, -1, "Pos:",
                     wxDLG_PNT(panel, wxPoint(4, 16)), wxDefaultSize)

        wxStaticText(panel, -1, "Idle:",
                     wxDLG_PNT(panel, wxPoint(4, 28)), wxDefaultSize)

        self.sizeCtrl = wxTextCtrl(panel, -1, "",
                                   wxDLG_PNT(panel, wxPoint(24, 4)),
                                   wxDLG_SZE(panel, wxSize(36, -1)),
                                   wxTE_READONLY)

        self.posCtrl = wxTextCtrl(panel, -1, "",
                                  wxDLG_PNT(panel, wxPoint(24, 16)),
                                  wxDLG_SZE(panel, wxSize(36, -1)),
                                  wxTE_READONLY)

        self.idleCtrl = wxTextCtrl(panel, -1, "",
                                   wxDLG_PNT(panel, wxPoint(24, 28)),
                                   wxDLG_SZE(panel, wxSize(36, -1)),
                                   wxTE_READONLY)


    def OnCloseWindow(self, event):
        app.keepGoing = false
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

class MyApp(wxApp):
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
        frame = MyFrame(NULL, -1, "This is a test")
        frame.Show(true)
        self.SetTopWindow(frame)

        self.keepGoing = true

        return true


app = MyApp(0)
app.MainLoop()





