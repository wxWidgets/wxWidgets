
import sys, os

from   wxPython.wx         import *
from   wxPython.html       import *
import wxPython.lib.wxpTag

from Main import opj

#----------------------------------------------------------------------

# This shows how to catch the OnLinkClicked non-event.  (It's a virtual
# method in the C++ code...)
class MyHtmlWindow(wxHtmlWindow):
    def __init__(self, parent, id, log):
        wxHtmlWindow.__init__(self, parent, id, style=wxNO_FULL_REPAINT_ON_RESIZE)
        self.log = log
        EVT_SCROLLWIN( self, self.OnScroll )

    def OnScroll( self, event ):
        #print 'event.GetOrientation()',event.GetOrientation()
        #print 'event.GetPosition()',event.GetPosition()
        event.Skip()


    def OnLinkClicked(self, linkinfo):
        self.log.WriteText('OnLinkClicked: %s\n' % linkinfo.GetHref())

        # Virtuals in the base class have been renamed with base_ on the front.
        self.base_OnLinkClicked(linkinfo)


    def OnSetTitle(self, title):
        self.log.WriteText('OnSetTitle: %s\n' % title)
        self.base_OnSetTitle(title)

    def OnCellMouseHover(self, cell, x, y):
        self.log.WriteText('OnCellMouseHover: %s, (%d %d)\n' % (cell, x, y))
        self.base_OnCellMouseHover(cell, x, y)

    def OnCellClicked(self, cell, x, y, evt):
        self.log.WriteText('OnCellClicked: %s, (%d %d)\n' % (cell, x, y))
        self.base_OnCellClicked(cell, x, y, evt)


# This filter doesn't really do anything but show how to use filters
class MyHtmlFilter(wxHtmlFilter):
    def __init__(self, log):
        wxHtmlFilter.__init__(self)
        self.log = log

    # This method decides if this filter is able to read the file
    def CanRead(self, fsfile):
        self.log.write("CanRead: %s\n" % fsfile.GetMimeType())
        return False

    # If CanRead returns True then this method is called to actually
    # read the file and return the contents.
    def ReadFile(self, fsfile):
        return ""


class TestHtmlPanel(wxPanel):
    def __init__(self, parent, frame, log):
        wxPanel.__init__(self, parent, -1, style=wxNO_FULL_REPAINT_ON_RESIZE)
        self.log = log
        self.frame = frame
        self.cwd = os.path.split(sys.argv[0])[0]
        if not self.cwd:
            self.cwd = os.getcwd()
        if frame:
            self.titleBase = frame.GetTitle()

        wxHtmlWindow_AddFilter(MyHtmlFilter(log))

        self.html = MyHtmlWindow(self, -1, log)
        self.html.SetRelatedFrame(frame, self.titleBase + " -- %s")
        self.html.SetRelatedStatusBar(0)

        self.printer = wxHtmlEasyPrinting()

        self.box = wxBoxSizer(wxVERTICAL)
        self.box.Add(self.html, 1, wxGROW)

        subbox = wxBoxSizer(wxHORIZONTAL)

        btn = wxButton(self, -1, "Load File")
        EVT_BUTTON(self, btn.GetId(), self.OnLoadFile)
        subbox.Add(btn, 1, wxGROW | wxALL, 2)

        btn = wxButton(self, -1, "Load URL")
        EVT_BUTTON(self, btn.GetId(), self.OnLoadURL)
        subbox.Add(btn, 1, wxGROW | wxALL, 2)

        btn = wxButton(self, -1, "With Widgets")
        EVT_BUTTON(self, btn.GetId(), self.OnWithWidgets)
        subbox.Add(btn, 1, wxGROW | wxALL, 2)

        btn = wxButton(self, -1, "Back")
        EVT_BUTTON(self, btn.GetId(), self.OnBack)
        subbox.Add(btn, 1, wxGROW | wxALL, 2)

        btn = wxButton(self, -1, "Forward")
        EVT_BUTTON(self, btn.GetId(), self.OnForward)
        subbox.Add(btn, 1, wxGROW | wxALL, 2)

        btn = wxButton(self, -1, "Print")
        EVT_BUTTON(self, btn.GetId(), self.OnPrint)
        subbox.Add(btn, 1, wxGROW | wxALL, 2)

        btn = wxButton(self, -1, "View Source")
        EVT_BUTTON(self, btn.GetId(), self.OnViewSource)
        subbox.Add(btn, 1, wxGROW | wxALL, 2)

        self.box.Add(subbox, 0, wxGROW)
        self.SetSizer(self.box)
        self.SetAutoLayout(True)

        # A button with this ID is created on the widget test page.
        EVT_BUTTON(self, wxID_OK, self.OnOk)

        self.OnShowDefault(None)


    def ShutdownDemo(self):
        # put the frame title back
        if self.frame:
            self.frame.SetTitle(self.titleBase)


    def OnShowDefault(self, event):
        name = os.path.join(self.cwd, opj('data/test.htm'))
        self.html.LoadPage(name)


    def OnLoadFile(self, event):
        dlg = wxFileDialog(self, wildcard = '*.htm*', style=wxOPEN)
        if dlg.ShowModal():
            path = dlg.GetPath()
            self.html.LoadPage(path)
        dlg.Destroy()


    def OnLoadURL(self, event):
        dlg = wxTextEntryDialog(self, "Enter a URL")
        if dlg.ShowModal():
            url = dlg.GetValue()
            self.html.LoadPage(url)
        dlg.Destroy()


    def OnWithWidgets(self, event):
        os.chdir(self.cwd)
        name = os.path.join(self.cwd, opj('data/widgetTest.htm'))
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
        ##self.printer.GetPageSetupData().SetMarginTopLeft((100,100))
        self.printer.PrintFile(self.html.GetOpenedPage())

#----------------------------------------------------------------------

def runTest(frame, nb, log):
    win = TestHtmlPanel(nb, frame, log)
    print wxWindow_FindFocus()
    return win


#----------------------------------------------------------------------





overview = """<html><body>
<h2>wxHtmlWindow</h2>

<p>wxHtmlWindow is capable of parsing and rendering most
simple HTML tags.

<p>It is not intended to be a high-end HTML browser.  If you're
looking for something like that try http://www.mozilla.org - there's a
chance you'll be able to make their widget wxWindows-compatible. I'm
sure everyone will enjoy your work in that case...

</body></html>
"""



if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])])








