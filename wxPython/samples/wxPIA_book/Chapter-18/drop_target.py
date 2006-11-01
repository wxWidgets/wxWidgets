import wx

class MyFileDropTarget(wx.FileDropTarget):
    def __init__(self, window):
        wx.FileDropTarget.__init__(self)
        self.window = window

    def OnDropFiles(self, x, y, filenames):
        self.window.AppendText("\n%d file(s) dropped at (%d,%d):\n" %
                               (len(filenames), x, y))
        for file in filenames:
            self.window.AppendText("\t%s\n" % file)
            

class MyFrame(wx.Frame):
    def __init__(self):
        wx.Frame.__init__(self, None, title="Drop Target",
                          size=(500,300))
        p = wx.Panel(self)

        # create the controls
        label = wx.StaticText(p, -1, "Drop some files here:")
        text = wx.TextCtrl(p, -1, "",
                           style=wx.TE_MULTILINE|wx.HSCROLL)

        # setup the layout with sizers
        sizer = wx.BoxSizer(wx.VERTICAL)
        sizer.Add(label, 0, wx.ALL, 5)
        sizer.Add(text, 1, wx.EXPAND|wx.ALL, 5)
        p.SetSizer(sizer)

        # make the text control be a drop target
        dt = MyFileDropTarget(text)
        text.SetDropTarget(dt)
        

app = wx.PySimpleApp()
frm = MyFrame()
frm.Show()
app.MainLoop()
