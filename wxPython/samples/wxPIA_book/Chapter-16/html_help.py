"""
wxPython can use html files for online help or other forms of
documentation for your application.  The help can be organized as a
collection of books, and there is a help viewer available that enables
the user to browse by book, via an index or full text search.  The
format of the contents and index files are similar to Microsoft
HtmlHelp.
"""

import wx
import wx.html

class MyHtmlFrame(wx.Frame):
    def __init__(self, parent, title):
        wx.Frame.__init__(self, parent, -1, title)
        p = wx.Panel(self)
        b1 = wx.Button(p, -1, "Show Help Contents")
        b2 = wx.Button(p, -1, "Show Help Index")
        b3 = wx.Button(p, -1, "Show Specific Help")
        self.Bind(wx.EVT_BUTTON, self.OnShowHelpContents, b1)
        self.Bind(wx.EVT_BUTTON, self.OnShowHelpIndex, b2)
        self.Bind(wx.EVT_BUTTON, self.OnShowSpecificHelp, b3)

        sizer = wx.BoxSizer(wx.VERTICAL)
        sizer.Add((10,10))
        sizer.Add(b1, 0, wx.ALL, 10)
        sizer.Add(b2, 0, wx.ALL, 10)
        sizer.Add(b3, 0, wx.ALL, 10)
        p.SetSizer(sizer)
        
        self.InitHelp()


    def InitHelp(self):
        def _addBook(filename):
            if not self.help.AddBook(filename):
                wx.MessageBox("Unable to open: " + filename,
                              "Error", wx.OK|wx.ICON_EXCLAMATION)

        self.help = wx.html.HtmlHelpController()

        _addBook("helpfiles/testing.hhp")
        _addBook("helpfiles/another.hhp")


    def OnShowHelpContents(self, evt):
        self.help.DisplayContents()

    def OnShowHelpIndex(self, evt):
        self.help.DisplayIndex()

    def OnShowSpecificHelp(self, evt):
        self.help.Display("sub book")


app = wx.PySimpleApp()
frm = MyHtmlFrame(None, "HTML Help")
frm.Show()
app.MainLoop()
