
import  sys

import  wx

import  ColorPanel
import  GridSimple
import  ListCtrl
import  ScrolledWindow
import  images

#----------------------------------------------------------------------------

class TestNB(wx.Notebook):
    def __init__(self, parent, id, log):
        wx.Notebook.__init__(self, parent, id, size=(21,21),
                             #style=
                             #wx.NB_TOP # | wx.NB_MULTILINE
                             #wx.NB_BOTTOM
                             #wx.NB_LEFT
                             #wx.NB_RIGHT
                             )
        self.log = log

        win = self.makeColorPanel(wx.BLUE)
        self.AddPage(win, "Blue")
        st = wx.StaticText(win.win, -1,
                          "You can put nearly any type of window here,\n"
                          "and if the platform supports it then the\n"
                          "tabs can be on any side of the notebook.",
                          (10, 10))

        st.SetForegroundColour(wx.WHITE)
        st.SetBackgroundColour(wx.BLUE)

        # Show how to put an image on one of the notebook tabs,
        # first make the image list:
        il = wx.ImageList(16, 16)
        idx1 = il.Add(images.getSmilesBitmap())
        self.AssignImageList(il)

        # now put an image on the first tab we just created:
        self.SetPageImage(0, idx1)


        win = self.makeColorPanel(wx.RED)
        self.AddPage(win, "Red")

        win = ScrolledWindow.MyCanvas(self)
        self.AddPage(win, 'ScrolledWindow')

        win = self.makeColorPanel(wx.GREEN)
        self.AddPage(win, "Green")

        win = GridSimple.SimpleGrid(self, log)
        self.AddPage(win, "Grid")

        win = ListCtrl.TestListCtrlPanel(self, log)
        self.AddPage(win, 'List')

        win = self.makeColorPanel(wx.CYAN)
        self.AddPage(win, "Cyan")

#         win = self.makeColorPanel(wxWHITE)
#         self.AddPage(win, "White")

#         win = self.makeColorPanel(wxBLACK)
#         self.AddPage(win, "Black")

        win = self.makeColorPanel(wx.NamedColour('MIDNIGHT BLUE'))
        self.AddPage(win, "MIDNIGHT BLUE")

        win = self.makeColorPanel(wx.NamedColour('INDIAN RED'))
        self.AddPage(win, "INDIAN RED")

        self.Bind(wx.EVT_NOTEBOOK_PAGE_CHANGED, self.OnPageChanged)
        self.Bind(wx.EVT_NOTEBOOK_PAGE_CHANGING, self.OnPageChanging)


    def makeColorPanel(self, color):
        p = wx.Panel(self, -1)
        win = ColorPanel.ColoredPanel(p, color)
        p.win = win

        def OnCPSize(evt, win=win):
            win.SetSize(evt.GetSize())

        p.Bind(wx.EVT_SIZE, OnCPSize)
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
<h2>wx.Notebook</h2>
<p>
This class represents a notebook control, which manages multiple
windows with associated tabs.
<p>
To use the class, create a wx.Notebook object and call AddPage or
InsertPage, passing a window to be used as the page. Do not explicitly
delete the window for a page that is currently managed by wx.Notebook.

"""


if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])] + sys.argv[1:])





