import wx
import wx.html

page = """<html><body>

This silly example shows how custom tags can be defined and used in a
wx.HtmlWindow.  We've defined a new tag, &lt;blue&gt; that will change
the <blue>foreground color</blue> of the portions of the document that
it encloses to some shade of blue.  The tag handler can also use
parameters specifed in the tag, for example:

<ul>
<li> <blue shade='sky'>Sky Blue</blue>
<li> <blue shade='midnight'>Midnight Blue</blue>
<li> <blue shade='dark'>Dark Blue</blue>
<li> <blue shade='navy'>Navy Blue</blue>
</ul>

</body></html>
"""


class BlueTagHandler(wx.html.HtmlWinTagHandler):
    def __init__(self):
        wx.html.HtmlWinTagHandler.__init__(self)

    def GetSupportedTags(self):
        return "BLUE"

    def HandleTag(self, tag):
        old = self.GetParser().GetActualColor()
        clr = "#0000FF"
        if tag.HasParam("SHADE"):
            shade = tag.GetParam("SHADE")
            if shade.upper() == "SKY":
                clr = "#3299CC"
            if shade.upper() == "MIDNIGHT":
                clr = "#2F2F4F"
            elif shade.upper() == "DARK":
                clr = "#00008B"
            elif shade.upper == "NAVY":
                clr = "#23238E"

        self.GetParser().SetActualColor(clr)
        self.GetParser().GetContainer().InsertCell(wx.html.HtmlColourCell(clr))
        
        self.ParseInner(tag)

        self.GetParser().SetActualColor(old)
        self.GetParser().GetContainer().InsertCell(wx.html.HtmlColourCell(old))

        return True
    

wx.html.HtmlWinParser_AddTagHandler(BlueTagHandler)



class MyHtmlFrame(wx.Frame):
    def __init__(self, parent, title):
        wx.Frame.__init__(self, parent, -1, title)
        html = wx.html.HtmlWindow(self)
        if "gtk2" in wx.PlatformInfo:
            html.SetStandardFonts()
        html.SetPage(page)
                

app = wx.PySimpleApp()
frm = MyHtmlFrame(None, "Custom HTML Tag Handler")
frm.Show()
app.MainLoop()
