
import sys, os

from   wxPython.wx         import *
from   wxPython.html       import *
import wxPython.lib.wxpTag

#----------------------------------------------------------------------

# This shows how to catch the OnLinkClicked non-event.  (It's a virtual
# method in the C++ code...)
class MyHtmlWindow(wxHtmlWindow):
    def __init__(self, parent, id, log):
        wxHtmlWindow.__init__(self, parent, id)
        self.log = log


    def OnLinkClicked(self, linkinfo):
        self.log.WriteText('OnLinkClicked: %s\n' % linkinfo.GetHref())

        # Virtuals in the base class have been renamed with base_ on the front.
        self.base_OnLinkClicked(linkinfo)


    def OnSetTitle(self, title):
        self.log.WriteText('OnSetTitle: %s\n' % title)
        self.base_OnSetTitle(title)

    def __del__(self):
        print 'MyHtmlWindow.__del__'


class TestHtmlPanel(wxPanel):
    def __init__(self, parent, frame, log):
        wxPanel.__init__(self, parent, -1)
        self.log = log
        self.frame = frame
        self.cwd = os.path.split(sys.argv[0])[0]
        if not self.cwd:
            self.cwd = os.getcwd()

        self.html = MyHtmlWindow(self, -1, log)
        self.html.SetRelatedFrame(frame, "wxPython: (A Demonstration) -- %s")
        self.html.SetRelatedStatusBar(0)

        self.printer = wxHtmlEasyPrinting()

        self.box = wxBoxSizer(wxVERTICAL)
        self.box.Add(self.html, 1, wxGROW)

        subbox = wxBoxSizer(wxHORIZONTAL)
        btn = wxButton(self, 1201, "Show Default")
        EVT_BUTTON(self, 1201, self.OnShowDefault)
        subbox.Add(btn, 1, wxGROW | wxALL, 2)

        btn = wxButton(self, 1202, "Load File")
        EVT_BUTTON(self, 1202, self.OnLoadFile)
        subbox.Add(btn, 1, wxGROW | wxALL, 2)

        btn = wxButton(self, 1203, "With Widgets")
        EVT_BUTTON(self, 1203, self.OnWithWidgets)
        subbox.Add(btn, 1, wxGROW | wxALL, 2)

        btn = wxButton(self, 1204, "Back")
        EVT_BUTTON(self, 1204, self.OnBack)
        subbox.Add(btn, 1, wxGROW | wxALL, 2)

        btn = wxButton(self, 1205, "Forward")
        EVT_BUTTON(self, 1205, self.OnForward)
        subbox.Add(btn, 1, wxGROW | wxALL, 2)

        btn = wxButton(self, 1207, "Print")
        EVT_BUTTON(self, 1207, self.OnPrint)
        subbox.Add(btn, 1, wxGROW | wxALL, 2)

        btn = wxButton(self, 1206, "View Source")
        EVT_BUTTON(self, 1206, self.OnViewSource)
        subbox.Add(btn, 1, wxGROW | wxALL, 2)

        self.box.Add(subbox, 0, wxGROW)
        self.SetSizer(self.box)
        self.SetAutoLayout(true)

        # A button with this ID is created on the widget test page.
        EVT_BUTTON(self, wxID_OK, self.OnOk)

        self.OnShowDefault(None)


    def __del__(self):
        print 'TestHtmlPanel.__del__'



    def OnShowDefault(self, event):
        name = os.path.join(self.cwd, 'data/test.htm')
        self.html.LoadPage(name)


    def OnLoadFile(self, event):
        dlg = wxFileDialog(self, wildcard = '*.htm*', style=wxOPEN)
        if dlg.ShowModal():
            path = dlg.GetPath()
            self.html.LoadPage(path)
        dlg.Destroy()


    def OnWithWidgets(self, event):
        os.chdir(self.cwd)
        name = os.path.join(self.cwd, 'data/widgetTest.htm')
        self.html.LoadPage(name)


    def OnOk(self, event):
        self.log.WriteText("It works!\n")

    def OnBack(self, event):
        if not self.html.HistoryBack():
            wxMessageBox("No more items in history!")


    def OnForward(self, event):
        if not self.html.HistoryForward():
            wxMessageBox("No more items in history!")


    def OnViewSource(self, event):
        from wxPython.lib.dialogs import wxScrolledMessageDialog
        source = self.html.GetParser().GetSource()
        dlg = wxScrolledMessageDialog(self, source, 'HTML Source')
        dlg.ShowModal()
        dlg.Destroy()


    def OnPrint(self, event):
        self.printer.PrintFile(self.html.GetOpenedPage())

#----------------------------------------------------------------------

def runTest(frame, nb, log):
    win = TestHtmlPanel(nb, frame, log)
    return win


#----------------------------------------------------------------------





overview = """\
wxHtmlWindow is capable of parsing and rendering most simple HTML tags.

It is not intended to be a high-end HTML browser.  If you're looking for something like that try http://www.mozilla.org - there's a chance you'll be able to make their widget wxWindows-compatible. I'm sure everyone will enjoy your work in that case...

"""










