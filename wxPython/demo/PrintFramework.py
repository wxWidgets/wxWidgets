
from wxPython.wx         import *
from wxScrolledWindow    import MyCanvas

#----------------------------------------------------------------------

class MyPrintout(wxPrintout):
    def __init__(self, canvas, log):
        wxPrintout.__init__(self)
        self.canvas = canvas
        self.log = log

    def OnBeginDocument(self, start, end):
        self.log.WriteText("wxPrintout.OnBeginDocument\n")
        return self.base_OnBeginDocument(start, end)

    def OnEndDocument(self):
        self.log.WriteText("wxPrintout.OnEndDocument\n")
        self.base_OnEndDocument()

    def OnBeginPrinting(self):
        self.log.WriteText("wxPrintout.OnBeginPrinting\n")
        self.base_OnBeginPrinting()

    def OnEndPrinting(self):
        self.log.WriteText("wxPrintout.OnEndPrinting\n")
        self.base_OnEndPrinting()

    def OnPreparePrinting(self):
        self.log.WriteText("wxPrintout.OnPreparePrinting\n")
        self.base_OnPreparePrinting()

    def HasPage(self, page):
        self.log.WriteText("wxPrintout.HasPage\n")
        if page == 1:
            return true
        else:
            return false

    def GetPageInfo(self):
        self.log.WriteText("wxPrintout.GetPageInfo\n")
        return (1, 1, 1, 1)

    def OnPrintPage(self, page):
        self.log.WriteText("wxPrintout.OnPrintPage\n")
        dc = self.GetDC()

        #-------------------------------------------
        # One possible method of setting scaling factors...

        maxX = self.canvas.getWidth()
        maxY = self.canvas.getHeight()

        # Let's have at least 50 device units margin
        marginX = 50
        marginY = 50

        # Add the margin to the graphic size
        maxX = maxX + (2 * marginX)
        maxY = maxY + (2 * marginY)

        # Get the size of the DC in pixels
        (w, h) = dc.GetSizeTuple()

        # Calculate a suitable scaling factor
        scaleX = float(w) / maxX
        scaleY = float(h) / maxY

        # Use x or y scaling factor, whichever fits on the DC
        actualScale = min(scaleX, scaleY)

        # Calculate the position on the DC for centring the graphic
        posX = (w - (self.canvas.getWidth() * actualScale)) / 2.0
        posY = (h - (self.canvas.getHeight() * actualScale)) / 2.0

        # Set the scale and origin
        dc.SetUserScale(actualScale, actualScale)
        dc.SetDeviceOrigin(int(posX), int(posY))

        #-------------------------------------------

        self.canvas.DoDrawing(dc)
        return true


#----------------------------------------------------------------------


class TestPrintPanel(wxPanel):
    def __init__(self, parent, frame, log):
        wxPanel.__init__(self, parent, -1)
        self.log = log
        self.frame = frame


        self.printData = wxPrintData()
        self.printData.SetPaperId(wxPAPER_LETTER)

        self.box = wxBoxSizer(wxVERTICAL)
        self.canvas = MyCanvas(self)
        self.box.Add(self.canvas, 1, wxGROW)

        subbox = wxBoxSizer(wxHORIZONTAL)
        btn = wxButton(self, 1201, "Print Setup")
        EVT_BUTTON(self, 1201, self.OnPrintSetup)
        subbox.Add(btn, 1, wxGROW | wxALL, 2)

        btn = wxButton(self, 1202, "Print Preview")
        EVT_BUTTON(self, 1202, self.OnPrintPreview)
        subbox.Add(btn, 1, wxGROW | wxALL, 2)

        btn = wxButton(self, 1203, "Print")
        EVT_BUTTON(self, 1203, self.OnDoPrint)
        subbox.Add(btn, 1, wxGROW | wxALL, 2)

        self.box.Add(subbox, 0, wxGROW)

        self.SetAutoLayout(true)
        self.SetSizer(self.box)


    def OnPrintSetup(self, event):
        printerDialog = wxPrintDialog(self)
        printerDialog.GetPrintDialogData().SetPrintData(self.printData)
        printerDialog.GetPrintDialogData().SetSetupDialog(true)
        printerDialog.ShowModal();
        self.printData = printerDialog.GetPrintDialogData().GetPrintData()
        printerDialog.Destroy()


    def OnPrintPreview(self, event):
        self.log.WriteText("OnPrintPreview\n")
        printout = MyPrintout(self.canvas, self.log)
        printout2 = MyPrintout(self.canvas, self.log)
        self.preview = wxPrintPreview(printout, printout2, self.printData)
        if not self.preview.Ok():
            self.log.WriteText("Houston, we have a problem...\n")
            return

        frame = wxPreviewFrame(self.preview, self.frame, "This is a print preview")

        frame.Initialize()
        frame.SetPosition(self.frame.GetPosition())
        frame.SetSize(self.frame.GetSize())
        frame.Show(true)



    def OnDoPrint(self, event):
        pdd = wxPrintDialogData()
        pdd.SetPrintData(self.printData)
        printer = wxPrinter(pdd)
        printout = MyPrintout(self.canvas, self.log)
        if not printer.Print(self.frame, printout):
            wxMessageBox("There was a problem printing.\nPerhaps your current printer is not set correctly?", "Printing", wxOK)
        else:
            self.printData = printer.GetPrintDialogData().GetPrintData()
        printout.Destroy()


#----------------------------------------------------------------------

def runTest(frame, nb, log):
    win = TestPrintPanel(nb, frame, log)
    return win


#----------------------------------------------------------------------





overview = """\
"""

