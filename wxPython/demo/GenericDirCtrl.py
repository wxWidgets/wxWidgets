
import  wx

#----------------------------------------------------------------------

class TestPanel(wx.Panel):
    def __init__(self, parent, log):
        wx.Panel.__init__(self, parent, -1)
        self.log = log

        txt1 = wx.StaticText(self, -1, "style=0")
        dir1 = wx.GenericDirCtrl(self, -1, size=(200,225), style=0)

        txt2 = wx.StaticText(self, -1, "wx.DIRCTRL_DIR_ONLY")
        dir2 = wx.GenericDirCtrl(self, -1, size=(200,225), style=wx.DIRCTRL_DIR_ONLY)

        txt3 = wx.StaticText(self, -1, "wx.DIRCTRL_SHOW_FILTERS")
        dir3 = wx.GenericDirCtrl(self, -1, size=(200,225), style=wx.DIRCTRL_SHOW_FILTERS,
                                filter="All files (*.*)|*.*|Python files (*.py)|*.py")

        sz = wx.FlexGridSizer(cols=3, hgap=5, vgap=5)
        sz.Add((35, 35))  # some space above
        sz.Add((35, 35))
        sz.Add((35, 35))

        sz.Add(txt1)
        sz.Add(txt2)
        sz.Add(txt3)

        sz.Add(dir1, 0, wx.EXPAND)
        sz.Add(dir2, 0, wx.EXPAND)
        sz.Add(dir3, 0, wx.EXPAND)

        sz.Add((35,35))  # some space below

        sz.AddGrowableRow(2)
        sz.AddGrowableCol(0)
        sz.AddGrowableCol(1)
        sz.AddGrowableCol(2)

        self.SetSizer(sz)
        self.SetAutoLayout(True)


#----------------------------------------------------------------------

def runTest(frame, nb, log):
    win = TestPanel(nb, log)
    return win

#----------------------------------------------------------------------


overview = """\
This control can be used to place a directory listing (with optional files)
on an arbitrary window. The control contains a TreeCtrl window representing 
the directory hierarchy, and optionally, a Choice window containing a list 
of filters.

The filters work in the same manner as in FileDialog.

"""


if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])] + sys.argv[1:])

