import  sys
import  wx

if wx.Platform == '__WXMSW__':
    from wx.lib.pdfwin import PDFWindow


#----------------------------------------------------------------------

class TestPanel(wx.Panel):
    def __init__(self, parent, log):
        wx.Panel.__init__(self, parent, -1)

        mainsizer = wx.BoxSizer(wx.HORIZONTAL)
        leftsizer = wx.BoxSizer(wx.VERTICAL)
        self.pdf = PDFWindow(self, style=wx.SUNKEN_BORDER)
        leftsizer.Add(self.pdf, proportion=1, flag=wx.EXPAND)

        box = wx.StaticBox(self, wx.NewId(), "" )
        buttonsizer = wx.StaticBoxSizer(box, wx.HORIZONTAL )
    
        b1 = wx.Button(self, wx.NewId(), "First")
        buttonsizer.Add(b1, proportion=0, flag=wx.ALIGN_CENTER|wx.ALL, border=5)
        self.Bind(wx.EVT_BUTTON, self.OnFirstPageButton, b1)

        b2 = wx.Button(self,  wx.NewId(), "Previous")
        buttonsizer.Add(b2, proportion=0, flag=wx.ALIGN_CENTER|wx.ALL, border=5)
        self.Bind(wx.EVT_BUTTON, self.OnPreviousPageButton, b2)
        
        tx1 = wx.StaticText(self, wx.NewId(), "   Go to page" )
        buttonsizer.Add(tx1, proportion=0, flag=wx.ALIGN_CENTER|wx.ALL, border=5)
        tc1 = wx.TextCtrl(self, wx.NewId(), "0", size=[30,-1])
        buttonsizer.Add( tc1, proportion=0, flag=wx.ALIGN_CENTER|wx.ALL, border=5)
        self.Bind(wx.EVT_TEXT, self.OnGotoPage, tc1)
    
        b3 = wx.Button(self, wx.NewId(), "Next")
        buttonsizer.Add(b3, proportion=0, flag=wx.ALIGN_CENTER|wx.ALL, border=5)
        self.Bind(wx.EVT_BUTTON, self.OnNextPageButton, b3)

        b4 = wx.Button(self, wx.NewId(), "Last")
        buttonsizer.Add(b4, proportion=0, flag=wx.ALIGN_CENTER|wx.ALL, border=5)
        self.Bind(wx.EVT_BUTTON, self.OnLastPageButton, b4)

        tx2 = wx.StaticText(self, wx.NewId(), "     Zoom")
        buttonsizer.Add(tx2, proportion=0, flag=wx.ALIGN_CENTER|wx.ALL, border=5)
 
        ch1 = wx.Choice(self, wx.NewId(),
                        choices=["Default", "Fit", "FitH", "FitV",
                                 "25%", "50%", "75%", "100%", "125%", "200%", "400%"])
        ch1.SetSelection(0)
        buttonsizer.Add(ch1, proportion=0, flag=wx.ALIGN_CENTER|wx.ALL, border=5)
        self.Bind(wx.EVT_CHOICE, self.OnZoom, ch1)

        leftsizer.Add(buttonsizer, proportion=0)
        mainsizer.Add(leftsizer, proportion=1, flag=wx.GROW|wx.ALIGN_CENTER_HORIZONTAL|wx.ALL, border=5)

        box = wx.StaticBox(self, wx.NewId(), "" )
        rightsizer = wx.StaticBoxSizer(box, wx.VERTICAL)
    
        b5 = wx.Button(self, wx.NewId(), "Load PDF")
        rightsizer.Add(b5, proportion=0, flag=wx.ALIGN_CENTER_VERTICAL|wx.ALL, border=5)
        self.Bind(wx.EVT_BUTTON, self.OnLoadButton, b5)

        b6 = wx.Button(self, wx.NewId(), "Print")
        rightsizer.Add(b6, proportion=0, flag=wx.ALIGN_CENTER_VERTICAL|wx.ALL, border=5)
        self.Bind(wx.EVT_BUTTON, self.OnPrintButton, b6)

        tx3 = wx.StaticText(self, wx.NewId(), "Page mode:")
        rightsizer.Add(tx3, proportion=0, flag=wx.ALIGN_CENTER_VERTICAL|wx.ALL, border=5)

        ch2 = wx.Choice(self, wx.NewId(),size=[100,-1],
                        choices=["None", "Bookmarks", "Thumbs"])
        ch2.SetSelection(0)
        rightsizer.Add(ch2, proportion=0, flag=wx.ALIGN_CENTER_VERTICAL|wx.ALL, border=5)
        self.Bind(wx.EVT_CHOICE, self.OnPageMode, ch2)

        tx4 = wx.StaticText(self, wx.NewId(), "Layout mode:")
        rightsizer.Add(tx4, proportion=0, flag=wx.ALIGN_CENTER_VERTICAL|wx.ALL, border=5)

        ch3 = wx.Choice(self, wx.NewId(),size=[100,-1],
                        choices=["DontCare", "SinglePage",
                                 "OneColumn", "TwoColumnLeft", "TwoColumnRight" ])
        ch3.SetSelection(0)
        rightsizer.Add(ch3, proportion=0, flag=wx.ALIGN_CENTER_VERTICAL|wx.ALL, border=5)
        self.Bind(wx.EVT_CHOICE, self.OnLayoutMode, ch3)

        cx1 = wx.CheckBox(self, wx.NewId(), "Toolbar")
        cx1.SetValue( True )
        rightsizer.Add( cx1,proportion=0, flag=wx.ALIGN_CENTER_VERTICAL|wx.ALL, border=5)
        self.Bind(wx.EVT_CHECKBOX, self.OnToolbar, cx1)

        cx2 = wx.CheckBox(self, wx.NewId(), "Scrollbars")
        cx2.SetValue( True )
        rightsizer.Add( cx2,proportion=0, flag=wx.ALIGN_CENTER_VERTICAL|wx.ALL, border=5)
        self.Bind(wx.EVT_CHECKBOX, self.OnScrollbars, cx2)

        mainsizer.Add( rightsizer, proportion=0, flag=wx.ALL, border=15)
        self.SetSizer(mainsizer)
        self.SetAutoLayout(True)

    def OnFirstPageButton(self, event):
        self.pdf.gotoFirstPage()

    def OnPreviousPageButton(self, event):
        self.pdf.gotoPreviousPage()

    def OnNextPageButton(self, event):
        self.pdf.gotoNextPage()

    def OnLastPageButton(self, event):
        self.pdf.gotoLastPage()

    def OnGotoPage(self, event):
        npage = event.GetEventObject().GetValue()
        try:
            self.pdf.setCurrentPage(int(npage))
        except ValueError:
            pass

    def OnZoom(self, event):
        astring = event.GetEventObject().GetStringSelection()
        if astring.startswith('Fit'):
            self.pdf.setView(astring)
        else:
            try:
                percent = float(astring.replace('%',''))
                self.pdf.setZoom(percent)
            except ValueError:
                pass
        
    def OnLoadButton(self, event):
        dlg = wx.FileDialog(self, wildcard="*.pdf")
        if dlg.ShowModal() == wx.ID_OK:
            wx.BeginBusyCursor()
            self.pdf.LoadFile(dlg.GetPath())
            wx.EndBusyCursor()
        dlg.Destroy()

    def OnPrintButton(self, event):
        self.pdf.Print()

    def OnPageMode(self, event):
        astring = event.GetEventObject().GetStringSelection()
        self.pdf.setPageMode(astring.lower())

    def OnLayoutMode(self, event):
        astring = event.GetEventObject().GetStringSelection()
        self.pdf.setLayoutMode(astring)

    def OnToolbar(self, event):
        on = event.GetEventObject().GetValue()
        self.pdf.setShowToolbar(on)

    def OnScrollbars(self, event):
        on = event.GetEventObject().GetValue()
        self.pdf.setShowScrollbars(on)

#----------------------------------------------------------------------

def runTest(frame, nb, log):
    if wx.Platform == '__WXMSW__':
        win = TestPanel(nb, log)
        return win
    else:
        from Main import MessagePanel
        win = MessagePanel(nb, 'This demo only works on Microsoft Windows.',
                           'Sorry', wx.ICON_WARNING)
        return win


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


