
"""
sorry no documentation...
Christopher J. Fama
"""



from wxPython.wx import *
from wxPython.html import *

class wxPyClickableHtmlWindow(wxHtmlWindow):
    """
    Class for a wxHtmlWindow which responds to clicks on links by opening a
    browser pointed at that link, and to shift-clicks by copying the link
    to the clipboard.
    """
    def __init__(self,parent,ID,**kw):
        apply(wxHtmlWindow.__init__,(self,parent,ID),kw)

    def OnLinkClicked(self,link):
        self.link = wxTextDataObject(link.GetHref())
        if link.GetEvent().ShiftDown():
            if wxTheClipboard.Open():
                wxTheClipboard.SetData(self.link)
                wxTheClipboard.Close()
            else:
                dlg = wxMessageDialog(self,"Couldn't open clipboard!\n",wxOK)
                wxBell()
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


