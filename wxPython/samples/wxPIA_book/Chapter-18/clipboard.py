import wx

t1_text = """\
The whole contents of this control
will be placed in the system's
clipboard when you click the copy
button below.
"""

t2_text = """\
If the clipboard contains a text
data object then it will be placed
in this control when you click
the paste button below.  Try
copying to and pasting from
other applications too!
"""

class MyFrame(wx.Frame):
    def __init__(self):
        wx.Frame.__init__(self, None, title="Clipboard",
                          size=(500,300))
        p = wx.Panel(self)

        # create the controls
        self.t1 = wx.TextCtrl(p, -1, t1_text,
                              style=wx.TE_MULTILINE|wx.HSCROLL)
        self.t2 = wx.TextCtrl(p, -1, t2_text,
                              style=wx.TE_MULTILINE|wx.HSCROLL)
        copy = wx.Button(p, -1, "Copy")
        paste = wx.Button(p, -1, "Paste")

        # setup the layout with sizers
        fgs = wx.FlexGridSizer(2, 2, 5, 5)
        fgs.AddGrowableRow(0)
        fgs.AddGrowableCol(0)
        fgs.AddGrowableCol(1)
        fgs.Add(self.t1, 0, wx.EXPAND)
        fgs.Add(self.t2, 0, wx.EXPAND)
        fgs.Add(copy, 0, wx.EXPAND)
        fgs.Add(paste, 0, wx.EXPAND)
        border = wx.BoxSizer()
        border.Add(fgs, 1, wx.EXPAND|wx.ALL, 5)
        p.SetSizer(border)

        # Bind events
        self.Bind(wx.EVT_BUTTON, self.OnDoCopy, copy)
        self.Bind(wx.EVT_BUTTON, self.OnDoPaste, paste)

    def OnDoCopy(self, evt):
        data = wx.TextDataObject()
        data.SetText(self.t1.GetValue())
        if wx.TheClipboard.Open():
            wx.TheClipboard.SetData(data)
            wx.TheClipboard.Close()
        else:
            wx.MessageBox("Unable to open the clipboard", "Error")

    def OnDoPaste(self, evt):
        success = False
        data = wx.TextDataObject()
        if wx.TheClipboard.Open():
            success = wx.TheClipboard.GetData(data)
            wx.TheClipboard.Close()

        if success:
            self.t2.SetValue(data.GetText())
        else:
            wx.MessageBox(
                "There is no data in the clipboard in the required format",
                "Error")
    

app = wx.PySimpleApp()
frm = MyFrame()
frm.Show()
app.MainLoop()
