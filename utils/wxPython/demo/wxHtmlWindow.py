
from wxPython.wx         import *
from wxPython.html       import *
from wxPython.lib.sizers import *

#----------------------------------------------------------------------

# This shows how to catch the OnLinkClicked non-event.  (It's a virtual
# method in the C++ code...)
class MyHtmlWindow(wxHtmlWindow):
    def __init__(self, parent, id, log):
        wxHtmlWindow.__init__(self, parent, id)
        self.log = log

    def OnLinkClicked(self, link):
        self.log.WriteText('OnLinkClicked: %s\n' % link)

        # Virtuals in the base class have been renamed with base_ on the font.
        self.base_OnLinkClicked(link)



class TestHtmlPanel(wxPanel):
    def __init__(self, parent, frame, log):
        wxPanel.__init__(self, parent, -1)
        self.log = log
        self.frame = frame


        self.html = MyHtmlWindow(self, -1, log)
        self.html.SetRelatedFrame(frame, "wxPython: (A Demonstration) -- %s")
        self.html.SetRelatedStatusBar(0)

        self.box = box.wxBoxSizer(wxVERTICAL)
        self.box.Add(self.html, 1)

        subbox = wxBoxSizer(wxHORIZONTAL)
        btn = wxButton(self, 1201, "Show Default")
        EVT_BUTTON(self, 1201, self.OnShowDefault)
        subbox.Add(btn, 1)

        btn = wxButton(self, 1202, "Load File")
        EVT_BUTTON(self, 1202, self.OnLoadFile)
        subbox.Add(btn, 1)

        btn = wxButton(self, 1203, "With Widgets")
        EVT_BUTTON(self, 1203, self.OnWithWidgets)
        subbox.Add(btn, 1)

        btn = wxButton(self, 1204, "Back")
        EVT_BUTTON(self, 1204, self.OnBack)
        subbox.Add(btn, 1)

        btn = wxButton(self, 1205, "Forward")
        EVT_BUTTON(self, 1205, self.OnForward)
        subbox.Add(btn, 1)

        self.box.Add(subbox)
        self.OnShowDefault(None)


    def OnSize(self, event):
        size = self.GetClientSize()
        self.box.Layout(size)


    def OnShowDefault(self, event):
        self.html.LoadPage("data/test.htm")


    def OnLoadFile(self, event):
        pass


    def OnWithWidgets(self, event):
        pass

    def OnBack(self, event):
        if not self.html.HistoryBack():
            wxMessageBox("No more items in history!")

    def OnForward(self, event):
        if not self.html.HistoryForward():
            wxMessageBox("No more items in history!")


#----------------------------------------------------------------------

def runTest(frame, nb, log):
    win = TestHtmlPanel(nb, frame, log)
    return win


#----------------------------------------------------------------------





overview = """\
wxHtmlWindow is capable of parsing and rendering most simple HTML tags.

It is not intended to be a high-end HTML browser.  If you're looking for something like that try http://www.mozilla.org - there's a chance you'll be able to make their widget wxWindows-compatible. I'm sure everyone will enjoy your work in that case...

"""

