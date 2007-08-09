
import wx


class TestNotebook(wx.Notebook):
    def __init__(self, parent, ID=-1):
        wx.Notebook.__init__(self, parent, ID)

        # page 1
        # just a panel with a small fixed size
        p = wx.Panel(self, size=(50,50))
        self.AddPage(p, "page 1")

        # page 2
        # a medium sized panel with manually layed out controls
        p = wx.Panel(self)
        b = wx.Button(p, -1, "a button", (20,20))
        b = wx.Button(p, -1, "another button", (80,80))
        b = wx.Button(p, -1, "and yet another button", (140,140))
        b.Bind(wx.EVT_BUTTON, self.ShowBestSizes)
        self.AddPage(p, "page 2")

        # page 3
        # a larger panel with lots of controls in a sizer.
        text = "one two buckle my shoe three four shut the door "\
               "five six pick up sticks seven eight lay them straight "\
               "nine ten big fat hen"
        p = wx.Panel(self)
        fgs = wx.FlexGridSizer(cols=4, vgap=5, hgap=5)
        for word in text.split():
            label = wx.StaticText(p, -1, word+":")
            tc = wx.TextCtrl(p, -1, "", size=(120,-1))
            fgs.Add(label, flag=wx.ALIGN_RIGHT | wx.ALIGN_CENTER_VERTICAL | wx.LEFT, border=10)
            fgs.Add(tc, flag=wx.RIGHT, border=10)
        box = wx.BoxSizer()
        box.Add(fgs, 1, wx.EXPAND|wx.ALL, 10)
        p.SetSizer(box)
        self.AddPage(p, "page 3")


    # show the best size of each page
    def ShowBestSizes(self, evt):
        for num in range(self.GetPageCount()):
            page = self.GetPage(num)
            print page.GetBestSize()
            

if __name__ == '__main__':
    app = wx.PySimpleApp()
    f = wx.Frame(None, -1, "Notebook Test")
    nb = TestNotebook(f)
    s = wx.BoxSizer()
    s.Add(nb)           # notebook is added directly to the sizer
    f.SetSizer(s)
    s.Fit(f)            # sizer calculates layout to set frame size
    f.Show()
    app.MainLoop()

