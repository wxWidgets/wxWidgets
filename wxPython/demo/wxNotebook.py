
from   wxPython.wx import *

import ColorPanel
import GridSimple
import wxListCtrl
import wxScrolledWindow
import images

import sys

#----------------------------------------------------------------------------

class TestNB(wxNotebook):
    def __init__(self, parent, id, log):
        wxNotebook.__init__(self, parent, id, style=
                            #0
                            wxNB_BOTTOM
                            #wxNB_LEFT
                            #wxNB_RIGHT
                            )
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

        # Show how to put an image on one of the notebook tabs,
        # first make the image list:
        il = wxImageList(16, 16)
        idx1 = il.Add(images.getSmilesBitmap())
        self.AssignImageList(il)

        # now put an image on the first tab we just created:
        self.SetPageImage(0, idx1)


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
        sel = self.GetSelection()
        self.log.write('OnPageChanged,  old:%d, new:%d, sel:%d\n' % (old, new, sel))
        event.Skip()

    def OnPageChanging(self, event):
        old = event.GetOldSelection()
        new = event.GetSelection()
        sel = self.GetSelection()
        self.log.write('OnPageChanging, old:%d, new:%d, sel:%d\n' % (old, new, sel))
        event.Skip()

#----------------------------------------------------------------------------

def runTest(frame, nb, log):
    testWin = TestNB(nb, -1, log)
    return testWin

#----------------------------------------------------------------------------




overview = """\
<html><body>
<h2>wxNotebook</h2>
<p>
This class represents a notebook control, which manages multiple
windows with associated tabs.
<p>
To use the class, create a wxNotebook object and call AddPage or
InsertPage, passing a window to be used as the page. Do not explicitly
delete the window for a page that is currently managed by wxNotebook.

"""



if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])])





