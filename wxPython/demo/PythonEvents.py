
import  sys

import  wx

#----------------------------------------------------------------------

# This shows the new 'official' way to do custom events as derived
# from the wxPython 2.5 migration guide.

#######################################################\
#  *** Old and busted ***                              |
#                                                      |
# myEVT_BUTTON_CLICKPOS = wx.NewEventType()            |
#                                                      |
# def EVT_BUTTON_CLICKPOS(win, id, func):              |
#     win.Connect(id, -1, myEVT_BUTTON_CLICKPOS, func) |
#######################################################/

#############################\
#   *** The new Hottness *** |
#############################/
myEVT_BUTTON_CLICKPOS = wx.NewEventType()
EVT_BUTTON_CLICKPOS = wx.PyEventBinder(myEVT_BUTTON_CLICKPOS, 1)

#----------------------------------------------------------------------


class MyEvent(wx.PyCommandEvent):
    def __init__(self, evtType, id):
        wx.PyCommandEvent.__init__(self, evtType, id)
        self.myVal = None

    #def __del__(self):
    #    print '__del__'
    #    wx.PyCommandEvent.__del__(self)

    def SetMyVal(self, val):
        self.myVal = val

    def GetMyVal(self):
        return self.myVal


class MyButton(wx.Button):
    def __init__(self, parent, id, txt, pos):
        wx.Button.__init__(self, parent, id, txt, pos)
        self.Bind(wx.EVT_LEFT_DOWN, self.OnLeftDown)

    def OnLeftDown(self, event):
        pt = event.GetPosition()
        evt = MyEvent(myEVT_BUTTON_CLICKPOS, self.GetId())
        evt.SetMyVal(pt)
        #print id(evt), sys.getrefcount(evt)
        self.GetEventHandler().ProcessEvent(evt)
        #print id(evt), sys.getrefcount(evt)
        event.Skip()



class TestPanel(wx.Panel):
    def __init__(self, parent, log):
        wx.Panel.__init__(self, parent, -1)
        self.log = log

        b = MyButton(self, -1, " Click me ", (30,30))
        self.Bind(wx.EVT_BUTTON, self.OnClick, id=b.GetId())
        
        # This is our custom event binder created above.
        self.Bind(EVT_BUTTON_CLICKPOS, self.OnMyEvent, id=b.GetId())

        wx.StaticText(
            self, -1, "Please see the Overview and Demo Code for details...",
            (30, 80)
            )


    def OnClick(self, event):
        self.log.WriteText("OnClick\n")

    def OnMyEvent(self, event):
        #print id(event), sys.getrefcount(event)
        self.log.WriteText("MyEvent: %s\n" % (event.GetMyVal(), ) )


#----------------------------------------------------------------------

def runTest(frame, nb, log):
    win = TestPanel(nb, log)
    return win

#----------------------------------------------------------------------


overview = """\
This demo is a contrived example of defining an event class in wxPython and 
sending it up the containment hierarchy for processing.

V2.5 note: this demo also shows the new style of creating event binders that
is required if you used the *.Bind() method of setting up event handlers.
"""


if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])] + sys.argv[1:])

