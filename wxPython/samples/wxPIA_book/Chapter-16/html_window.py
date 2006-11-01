import wx
import wx.html

class MyHtmlFrame(wx.Frame):
    def __init__(self, parent, title):
        wx.Frame.__init__(self, parent, -1, title)
        html = wx.html.HtmlWindow(self)
        if "gtk2" in wx.PlatformInfo:
            html.SetStandardFonts()

        html.SetPage(
            "Here is some <b>formatted</b> <i><u>text</u></i> "
            "loaded from a <font color=\"red\">string</font>.")
                

app = wx.PySimpleApp()
frm = MyHtmlFrame(None, "Simple HTML")
frm.Show()
app.MainLoop()
