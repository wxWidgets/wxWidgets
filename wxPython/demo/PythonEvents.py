
from wxPython.wx import *
import sys

#----------------------------------------------------------------------

myEVT_BUTTON_CLICKPOS = 5015

def EVT_BUTTON_CLICKPOS(win, id, func):
    win.Connect(id, -1, myEVT_BUTTON_CLICKPOS, func)



class MyEvent(wxPyCommandEvent):
    def __init__(self, evtType, id):
        wxPyCommandEvent.__init__(self, evtType, id)
        self.myVal = None

    #def __del__(self):
    #    print '__del__'
    #    wxPyCommandEvent.__del__(self)

    def SetMyVal(self, val):
        self.myVal = val

    def GetMyVal(self):
        return self.myVal



class MyButton(wxButton):
    def __init__(self, parent, id, txt, pos):
        wxButton.__init__(self, parent, id, txt, pos)
        EVT_LEFT_DOWN(self, self.OnLeftDown)

    def OnLeftDown(self, event):
        pt = event.GetPosition()
        evt = MyEvent(myEVT_BUTTON_CLICKPOS, self.GetId())
        evt.SetMyVal(pt)
        #print id(evt), sys.getrefcount(evt)
        self.GetEventHandler().ProcessEvent(evt)
        #print id(evt), sys.getrefcount(evt)
        event.Skip()



class TestPanel(wxPanel):
    def __init__(self, parent, log):
        wxPanel.__init__(self, parent, -1)
        self.log = log

        b = MyButton(self, -1, " Click me ", wxPoint(30,30))
        EVT_BUTTON(self, b.GetId(), self.OnClick)
        EVT_BUTTON_CLICKPOS(self, b.GetId(), self.OnMyEvent)

        wxStaticText(self, -1, "Please see the Overview and Demo Code for details...",
                     wxPoint(30, 80))


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
This demo is a contrived example of defining an event class in wxPython and sending it up the containment heirachy for processing.
"""




