
from   wxPython.wx import *

import ColorPanel
import GridSimple
import wxListCtrl
import wxScrolledWindow

import sys

#----------------------------------------------------------------------------

class TestNB(wxNotebook):
    def __init__(self, parent, id, log):
        wxNotebook.__init__(self, parent, id, style=wxNB_BOTTOM)
        self.log = log


        win = self.makeColorPanel(wxBLUE)
        self.AddPage(win, "Blue")
        st = wxStaticText(win.win, -1,
                          "You can put nearly any type of window here,\n"
                          "and if the platform supports it then the\n"
                          "tabs can be on any side of the notebook.",
                          wxPoint(10, 10))
        st.SetForegroundColour(wxWHITE)
        st.SetBackgroundColour(wxBLUE)

        win = self.makeColorPanel(wxRED)
        self.AddPage(win, "Red")

        win = wxScrolledWindow.MyCanvas(self)
        self.AddPage(win, 'ScrolledWindow')

        win = self.makeColorPanel(wxGREEN)
        self.AddPage(win, "Green")

        win = GridSimple.SimpleGrid(self, log)
        self.AddPage(win, "Grid")

        win = wxListCtrl.TestListCtrlPanel(self, log)
        self.AddPage(win, 'List')

        win = self.makeColorPanel(wxCYAN)
        self.AddPage(win, "Cyan")

        win = self.makeColorPanel(wxWHITE)
        self.AddPage(win, "White")

        win = self.makeColorPanel(wxBLACK)
        self.AddPage(win, "Black")

        win = self.makeColorPanel(wxNamedColour('MIDNIGHT BLUE'))
        self.AddPage(win, "MIDNIGHT BLUE")

        win = self.makeColorPanel(wxNamedColour('INDIAN RED'))
        self.AddPage(win, "INDIAN RED")

        EVT_NOTEBOOK_PAGE_CHANGED(self, self.GetId(), self.OnPageChanged)
        EVT_NOTEBOOK_PAGE_CHANGING(self, self.GetId(), self.OnPageChanging)


    def makeColorPanel(self, color):
        p = wxPanel(self, -1)
        win = ColorPanel.ColoredPanel(p, color)
        p.win = win
        def OnCPSize(evt, win=win):
            win.SetSize(evt.GetSize())
        EVT_SIZE(p, OnCPSize)
        return p


    def OnPageChanged(self, event):
        old = event.GetOldSelection()
        new = event.GetSelection()
        self.log.write('OnPageChanged, old:%d, new:%d\n' % (old, new))
        event.Skip()

    def OnPageChanging(self, event):
        old = event.GetOldSelection()
        new = event.GetSelection()
        self.log.write('OnPageChanging, old:%d, new:%d\n' % (old, new))
        event.Skip()


#----------------------------------------------------------------------------

def runTest(frame, nb, log):
    testWin = TestNB(nb, -1, log)
    return testWin

#----------------------------------------------------------------------------












overview = """\
This class represents a notebook control, which manages multiple windows with associated tabs.

To use the class, create a wxNotebook object and call AddPage or InsertPage, passing a window to be used as the page. Do not explicitly delete the window for a page that is currently managed by wxNotebook.

"""


if __name__ == "__main__":
    app = wxPySimpleApp()
    frame = wxFrame(None, -1, "Test Notebook", size=(600, 400))
    win = TestNB(frame, -1, sys.stdout)
    frame.Show(true)
    app.MainLoop()



