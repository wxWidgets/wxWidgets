
import  wx

#----------------------------------------------------------------------


class TestPanel(wx.Panel):
    def __init__(self, parent, log):
        wx.Panel.__init__(self, parent, -1)

        e = wx.FontEnumerator()
        e.EnumerateFacenames()
        list = e.GetFacenames()

        list.sort()

        s1 = wx.StaticText(self, -1, "Face names:")

        self.lb1 = wx.ListBox(self, -1, wx.DefaultPosition, (200, 250),
                             list, wx.LB_SINGLE)

        self.Bind(wx.EVT_LISTBOX, self.OnSelect, id=self.lb1.GetId())

        self.txt = wx.StaticText(self, -1, "Sample text...", (285, 50))

        row = wx.BoxSizer(wx.HORIZONTAL)
        row.Add(s1, 0, wx.ALL, 5)
        row.Add(self.lb1, 0, wx.ALL, 5)
        row.Add(self.txt, 0, wx.ALL|wx.ADJUST_MINSIZE, 5)

        sizer = wx.BoxSizer(wx.VERTICAL)
        sizer.Add(row, 0, wx.ALL, 30)
        self.SetSizer(sizer)
        self.Layout()

        self.lb1.SetSelection(0)
        self.OnSelect(None)
        wx.FutureCall(300, self.SetTextSize)


    def SetTextSize(self):
        self.txt.SetSize(self.txt.GetBestSize())


    def OnSelect(self, evt):
        face = self.lb1.GetStringSelection()
        font = wx.Font(28, wx.DEFAULT, wx.NORMAL, wx.NORMAL, False, face)
        self.txt.SetLabel(face)
        self.txt.SetFont(font)
        if wx.Platform == "__WXMAC__": self.Refresh()

##         st = font.GetNativeFontInfo().ToString()
##         ni2 = wx.NativeFontInfo()
##         ni2.FromString(st)
##         font2 = wx.FontFromNativeInfo(ni2)

#----------------------------------------------------------------------

def runTest(frame, nb, log):
    win = TestPanel(nb, log)
    return win

#----------------------------------------------------------------------



overview = """<html><body>
wxFontEnumerator enumerates either all available fonts on the system or only
the ones with given attributes - either only fixed-width (suited for use in
programs such as terminal emulators and the like) or the fonts available in
the given encoding.
</body></html>
"""


if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])] + sys.argv[1:])

