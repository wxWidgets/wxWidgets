import  sys
import  wx

if wx.Platform == '__WXMSW__':
    from wx.lib.pdfwin import PDFWindow


#----------------------------------------------------------------------

class TestPanel(wx.Panel):
    def __init__(self, parent, log):
        wx.Panel.__init__(self, parent, -1)
        self.pdf = None

        sizer = wx.BoxSizer(wx.VERTICAL)
        btnSizer = wx.BoxSizer(wx.HORIZONTAL)

        self.pdf = PDFWindow(self, style=wx.SUNKEN_BORDER)

        sizer.Add(self.pdf, proportion=1, flag=wx.EXPAND)

        btn = wx.Button(self, wx.NewId(), "Open PDF File")
        self.Bind(wx.EVT_BUTTON, self.OnOpenButton, btn)
        btnSizer.Add(btn, proportion=1, flag=wx.EXPAND|wx.ALL, border=5)

        btn = wx.Button(self, wx.NewId(), "<-- Previous Page")
        self.Bind(wx.EVT_BUTTON, self.OnPrevPageButton, btn)
        btnSizer.Add(btn, proportion=1, flag=wx.EXPAND|wx.ALL, border=5)

        btn = wx.Button(self, wx.NewId(), "Next Page -->")
        self.Bind(wx.EVT_BUTTON, self.OnNextPageButton, btn)
        btnSizer.Add(btn, proportion=1, flag=wx.EXPAND|wx.ALL, border=5)


        btnSizer.Add((50,-1), proportion=2, flag=wx.EXPAND)
        sizer.Add(btnSizer, proportion=0, flag=wx.EXPAND)

        self.SetSizer(sizer)
        self.SetAutoLayout(True)



    def OnOpenButton(self, event):
        dlg = wx.FileDialog(self, wildcard="*.pdf")

        if dlg.ShowModal() == wx.ID_OK:
            wx.BeginBusyCursor()
            self.pdf.LoadFile(dlg.GetPath())
            wx.EndBusyCursor()

        dlg.Destroy()


    def OnPrevPageButton(self, event):
        self.pdf.gotoPreviousPage()


    def OnNextPageButton(self, event):
        self.pdf.gotoNextPage()



#----------------------------------------------------------------------

def runTest(frame, nb, log):
    if wx.Platform == '__WXMSW__':
        win = TestPanel(nb, log)
        return win
    else:
        dlg = wx.MessageDialog(frame, 'This demo only works on MSW.',
                          'Sorry', wx.OK | wx.ICON_INFORMATION)
        dlg.ShowModal()
        dlg.Destroy()


overview = """\
<html><body>
<h2>wx.lib.pdfwin.PDFWindow</h2>

The wx.lib.pdfwin.PDFWindow class is another example of using ActiveX
controls from wxPython using the new wx.activex module.  This allows
you to use an ActiveX control as if it is a wx.Window, you can call
its methods, set/get properties, and receive events from the ActiveX
control in a very intuitive way.

<p> Using this class is simpler than ActiveXWrapper, doesn't rely on
the win32all extensions, and is more "wx\'ish", meaning that it uses
events and etc. as would be expected from any other wx window.

<p> This demo embeds the Adobe Acrobat Reader, and gives you some
buttons for opening a PDF file, changing pages, etc. that show how to
call methods on the COM object.  If you don't have Acrobat Reader 4.0
(or greater) installed it won't work.

</body></html>
"""

#----------------------------------------------------------------------



if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])] + sys.argv[1:])


