
from   wxPython.wx import *

import ColorPanel
import GridSimple
import wxListCtrl
import wxScrolledWindow

#----------------------------------------------------------------------------

class TestNB(wxNotebook):
    def __init__(self, parent, id, log):
        wxNotebook.__init__(self, parent, id, style=wxNB_BOTTOM)
        self.log = log

        win = ColorPanel.ColoredPanel(self, wxBLUE)
        self.AddPage(win, "Blue")
        st = wxStaticText(win, -1,
                          "You can put nearly any type of window here,\n"
                          "and the tabs can be on any side... (look below.)",
                          wxPoint(10, 10))
        st.SetForegroundColour(wxWHITE)
        st.SetBackgroundColour(wxBLUE)

        win = ColorPanel.ColoredPanel(self, wxRED)
        self.AddPage(win, "Red")

        win = wxScrolledWindow.MyCanvas(self)
        self.AddPage(win, 'ScrolledWindow')

        win = ColorPanel.ColoredPanel(self, wxGREEN)
        self.AddPage(win, "Green")

        win = GridSimple.SimpleGrid(self, log)
        self.AddPage(win, "Grid")

        win = wxListCtrl.TestListCtrlPanel(self, log)
        self.AddPage(win, 'List')

        win = ColorPanel.ColoredPanel(self, wxCYAN)
        self.AddPage(win, "Cyan")

        win = ColorPanel.ColoredPanel(self, wxWHITE)
        self.AddPage(win, "White")

        win = ColorPanel.ColoredPanel(self, wxBLACK)
        self.AddPage(win, "Black")

        win = ColorPanel.ColoredPanel(self, wxNamedColour('MIDNIGHT BLUE'))
        self.AddPage(win, "MIDNIGHT BLUE")

        win = ColorPanel.ColoredPanel(self, wxNamedColour('INDIAN RED'))
        self.AddPage(win, "INDIAN RED")

        EVT_NOTEBOOK_PAGE_CHANGED(self, self.GetId(), self.OnPageChanged)


    def OnPageChanged(self, event):
        self.log.write('OnPageChanged\n')
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
