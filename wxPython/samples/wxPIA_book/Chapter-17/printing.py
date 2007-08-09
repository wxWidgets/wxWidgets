import wx
import os

FONTSIZE = 10

class TextDocPrintout(wx.Printout):
    """
    A printout class that is able to print simple text documents.
    Does not handle page numbers or titles, and it assumes that no
    lines are longer than what will fit within the page width.  Those
    features are left as an exercise for the reader. ;-)
    """
    def __init__(self, text, title, margins):
        wx.Printout.__init__(self, title)
        self.lines = text.split('\n')
        self.margins = margins


    def HasPage(self, page):
        return page <= self.numPages

    def GetPageInfo(self):
        return (1, self.numPages, 1, self.numPages)


    def CalculateScale(self, dc):
        # Scale the DC such that the printout is roughly the same as
        # the screen scaling.
        ppiPrinterX, ppiPrinterY = self.GetPPIPrinter()
        ppiScreenX, ppiScreenY = self.GetPPIScreen()
        logScale = float(ppiPrinterX)/float(ppiScreenX)

        # Now adjust if the real page size is reduced (such as when
        # drawing on a scaled wx.MemoryDC in the Print Preview.)  If
        # page width == DC width then nothing changes, otherwise we
        # scale down for the DC.
        pw, ph = self.GetPageSizePixels()
        dw, dh = dc.GetSize()
        scale = logScale * float(dw)/float(pw)

        # Set the DC's scale.
        dc.SetUserScale(scale, scale)

        # Find the logical units per millimeter (for calculating the
        # margins)
        self.logUnitsMM = float(ppiPrinterX)/(logScale*25.4)


    def CalculateLayout(self, dc):
        # Determine the position of the margins and the
        # page/line height
        topLeft, bottomRight = self.margins
        dw, dh = dc.GetSize()
        self.x1 = topLeft.x * self.logUnitsMM
        self.y1 = topLeft.y * self.logUnitsMM
        self.x2 = dc.DeviceToLogicalXRel(dw) - bottomRight.x * self.logUnitsMM 
        self.y2 = dc.DeviceToLogicalYRel(dh) - bottomRight.y * self.logUnitsMM 

        # use a 1mm buffer around the inside of the box, and a few
        # pixels between each line
        self.pageHeight = self.y2 - self.y1 - 2*self.logUnitsMM
        font = wx.Font(FONTSIZE, wx.TELETYPE, wx.NORMAL, wx.NORMAL)
        dc.SetFont(font)
        self.lineHeight = dc.GetCharHeight() 
        self.linesPerPage = int(self.pageHeight/self.lineHeight)


    def OnPreparePrinting(self):
        # calculate the number of pages
        dc = self.GetDC()
        self.CalculateScale(dc)
        self.CalculateLayout(dc)
        self.numPages = len(self.lines) / self.linesPerPage
        if len(self.lines) % self.linesPerPage != 0:
            self.numPages += 1


    def OnPrintPage(self, page):
        dc = self.GetDC()
        self.CalculateScale(dc)
        self.CalculateLayout(dc)

        # draw a page outline at the margin points
        dc.SetPen(wx.Pen("black", 0))
        dc.SetBrush(wx.TRANSPARENT_BRUSH)
        r = wx.RectPP((self.x1, self.y1),
                      (self.x2, self.y2))
        dc.DrawRectangleRect(r)
        dc.SetClippingRect(r)

        # Draw the text lines for this page
        line = (page-1) * self.linesPerPage
        x = self.x1 + self.logUnitsMM
        y = self.y1 + self.logUnitsMM
        while line < (page * self.linesPerPage):
            dc.DrawText(self.lines[line], x, y)
            y += self.lineHeight
            line += 1
            if line >= len(self.lines):
                break
        return True


class PrintFrameworkSample(wx.Frame):
    def __init__(self):
        wx.Frame.__init__(self, None, size=(640, 480),
                          title="Print Framework Sample")
        self.CreateStatusBar()

        # A text widget to display the doc and let it be edited
        self.tc = wx.TextCtrl(self, -1, "",
                              style=wx.TE_MULTILINE|wx.TE_DONTWRAP)
        self.tc.SetFont(wx.Font(FONTSIZE, wx.TELETYPE, wx.NORMAL, wx.NORMAL))
        filename = os.path.join(os.path.dirname(__file__), "sample-text.txt")
        self.tc.SetValue(open(filename).read())
        self.tc.Bind(wx.EVT_SET_FOCUS, self.OnClearSelection)
        wx.CallAfter(self.tc.SetInsertionPoint, 0)

        # Create the menu and menubar
        menu = wx.Menu()
        item = menu.Append(-1, "Page Setup...\tF5",
                           "Set up page margins and etc.")
        self.Bind(wx.EVT_MENU, self.OnPageSetup, item)
        item = menu.Append(-1, "Print Preview...\tF6",
                           "View the printout on-screen")
        self.Bind(wx.EVT_MENU, self.OnPrintPreview, item)
        item = menu.Append(-1, "Print...\tF7", "Print the document")
        self.Bind(wx.EVT_MENU, self.OnPrint, item)
        menu.AppendSeparator()
##         item = menu.Append(-1, "Test other stuff...\tF9", "")
##         self.Bind(wx.EVT_MENU, self.OnPrintTest, item)
##         menu.AppendSeparator()
        item = menu.Append(wx.ID_EXIT, "E&xit\tCtrl-Q", "Close this application")
        self.Bind(wx.EVT_MENU, self.OnExit, item)
        
        menubar = wx.MenuBar()
        menubar.Append(menu, "&File")
        self.SetMenuBar(menubar)

        # initialize the print data and set some default values
        self.pdata = wx.PrintData()
        self.pdata.SetPaperId(wx.PAPER_LETTER)
        self.pdata.SetOrientation(wx.PORTRAIT)
        self.margins = (wx.Point(15,15), wx.Point(15,15))


    def OnExit(self, evt):
        self.Close()


    def OnClearSelection(self, evt):
        evt.Skip()
        wx.CallAfter(self.tc.SetInsertionPoint,
                     self.tc.GetInsertionPoint())


    def OnPageSetup(self, evt):
        data = wx.PageSetupDialogData()
        data.SetPrintData(self.pdata)

        data.SetDefaultMinMargins(True)
        data.SetMarginTopLeft(self.margins[0])
        data.SetMarginBottomRight(self.margins[1])

        dlg = wx.PageSetupDialog(self, data)
        if dlg.ShowModal() == wx.ID_OK:
            data = dlg.GetPageSetupData()
            self.pdata = wx.PrintData(data.GetPrintData()) # force a copy
            self.pdata.SetPaperId(data.GetPaperId())
            self.margins = (data.GetMarginTopLeft(),
                            data.GetMarginBottomRight())
        dlg.Destroy()


    def OnPrintPreview(self, evt):
        data = wx.PrintDialogData(self.pdata)
        text = self.tc.GetValue() 
        printout1 = TextDocPrintout(text, "title", self.margins)
        printout2 = None #TextDocPrintout(text, "title", self.margins)
        preview = wx.PrintPreview(printout1, printout2, data)
        if not preview.Ok():
            wx.MessageBox("Unable to create PrintPreview!", "Error")
        else:
            # create the preview frame such that it overlays the app frame
            frame = wx.PreviewFrame(preview, self, "Print Preview",
                                    pos=self.GetPosition(),
                                    size=self.GetSize())
            frame.Initialize()
            frame.Show()


    def OnPrint(self, evt):
        data = wx.PrintDialogData(self.pdata)
        printer = wx.Printer(data)
        text = self.tc.GetValue() 
        printout = TextDocPrintout(text, "title", self.margins)
        useSetupDialog = True
        if not printer.Print(self, printout, useSetupDialog) \
           and printer.GetLastError() == wx.PRINTER_ERROR:
            wx.MessageBox(
                "There was a problem printing.\n"
                "Perhaps your current printer is not set correctly?",
                "Printing Error", wx.OK)
        else:
            data = printer.GetPrintDialogData()
            self.pdata = wx.PrintData(data.GetPrintData()) # force a copy
        printout.Destroy()


    def OnPrintTest(self, evt):
        data = wx.PrintDialogData(self.pdata)
        dlg = wx.PrintDialog(self, data)
        if dlg.ShowModal() == wx.ID_OK:
            data = dlg.GetPrintDialogData()
            print
            print "GetFromPage:", data.GetFromPage()
            print "GetToPage:", data.GetToPage()
            print "GetMinPage:", data.GetMinPage()
            print "GetMaxPage:", data.GetMaxPage()
            print "GetNoCopies:", data.GetNoCopies()
            print "GetAllPages:", data.GetAllPages()
            print "GetSelection:", data.GetSelection()
            print "GetCollate:", data.GetCollate()
            print "GetPrintToFile:", data.GetPrintToFile()

            self.pdata = wx.PrintData(data.GetPrintData())
            print
            print "GetPrinterName:", self.pdata.GetPrinterName()

        dlg.Destroy()
        
    
app = wx.PySimpleApp()
frm = PrintFrameworkSample()
frm.Show()
app.MainLoop()
