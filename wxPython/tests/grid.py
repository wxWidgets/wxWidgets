from wxPython.wx import *
from wxPython.lib.grids import wxFlexGridSizer

class aFrame(wxFrame):
    def __init__(self, parent=NULL, id=-1, title="test"):
        wxFrame.__init__(self, parent, id, title)
        s =wxBoxSizer(wxVERTICAL)
        gs =wxFlexGridSizer(2, 2, 2, 2)
        for label in ('one', 'two', 'tree', 'four'):
            gs.Add(wxButton(self, -1, label, size=(100,100)), 1, wxEXPAND)
        s.Add(gs, 1, wxEXPAND|wxALL, 50)
        self.SetSizer(s)
        self.SetAutoLayout(TRUE)
        s.Fit(self)

class MyApp(wxApp):
    def OnInit(self):
      frame =aFrame()
      self.SetTopWindow(frame)
      frame.Show(TRUE)
      return TRUE

app=MyApp(0)
app.MainLoop()
