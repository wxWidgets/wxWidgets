
from wxPython.wx import *
from wxPython.lib.grids import wxFlexGridSizer

import time
from threading import Thread
def foo():
    for x in range(20):
        print x, "Fooing!"
        time.sleep(0.5)
Thread(target=foo).start()

class MyFrame(wxFrame):
    def __init__(self, parent, ID, title, pos, size):
        wxFrame.__init__(self, parent, ID, title, pos, size)
        panel = wxPanel(self, -1)
        self.panel=panel
        box = wxFlexGridSizer(1, 2, 10, 10)
        box.Add(wxButton(panel, 11211, "Foo"), 0, wxCENTER)
        box.Add(wxButton(panel, 11211, "Bar"), 0, wxCENTER)
        box.AddGrowableCol(1)
        panel.SetSizer(box)
        panel.SetAutoLayout(true)
        #EVT_SIZE(panel, lambda e, p=panel: p.Layout())
        EVT_BUTTON(self, 11211, self.Click)

    def Click(self, event):
        print "Click"


class MyApp(wxApp):
    def OnInit(self):
        win = MyFrame(None, -1, "Bummer", wxDefaultPosition, (200, 100))
        win.Show(true)
        self.SetTopWindow(win)
        return true

MyApp(0).MainLoop()

