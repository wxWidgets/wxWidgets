# 12/01/2003 - Jeff Grimmett (grimmtooth@softhome.net)
#
# o Updated for wx namespace. Not tested though.
#
# 12/17/2003 - Jeff Grimmett (grimmtooth@softhome.net)
#
# o Removed wx prefix from class name, 
#   updated reverse renamer
#

"""
sorry no documentation...
Christopher J. Fama
"""


import  wx
import  wx.html as  html

class PyClickableHtmlWindow(html.HtmlWindow):
    """
    Class for a wxHtmlWindow which responds to clicks on links by opening a
    browser pointed at that link, and to shift-clicks by copying the link
    to the clipboard.
    """
    def __init__(self,parent,ID,**kw):
        apply(html.HtmlWindow.__init__,(self,parent,ID),kw)

    def OnLinkClicked(self,link):
        self.link = wx.TextDataObject(link.GetHref())
        if link.GetEvent().ShiftDown():
            if wx.TheClipboard.Open():
                wx.TheClipboard.SetData(self.link)
                wx.TheClipboard.Close()
            else:
                dlg = wx.MessageDialog(self,"Couldn't open clipboard!\n",wx.OK)
                wx.Bell()
                dlg.ShowModal()
                dlg.Destroy()
        else:
            if 0:  # Chris's original code...
                if sys.platform not in ["windows",'nt']           :
                    #TODO: A MORE APPROPRIATE COMMAND LINE FOR Linux
                    #[or rather, non-Windows platforms... as of writing,
                    #this MEANS  Linux, until wxPython for wxMac comes along...]
                    command = "/usr/bin/netscape"
                else:
                    command = "start"
                command = "%s \"%s\"" % (command,
                                         self.link.GetText ())
                os.system (command)

            else:  # My alternative
                import webbrowser
                webbrowser.open(link.GetHref())


