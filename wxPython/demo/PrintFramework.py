
import  wx
import  ScrolledWindow

#----------------------------------------------------------------------

ID_Setup    =   wx.NewId()
ID_Preview  =   wx.NewId()
ID_Print    =   wx.NewId()

class MyPrintout(wx.Printout):
    def __init__(self, canvas, log):
        wx.Printout.__init__(self)
        self.canvas = canvas
        self.log = log

    def OnBeginDocument(self, start, end):
        self.log.WriteText("wx.Printout.OnBeginDocument\n")
        return self.base_OnBeginDocument(start, end)

    def OnEndDocument(self):
        self.log.WriteText("wx.Printout.OnEndDocument\n")
        self.base_OnEndDocument()

    def OnBeginPrinting(self):
        self.log.WriteText("wx.Printout.OnBeginPrinting\n")
        self.base_OnBeginPrinting()

    def OnEndPrinting(self):
        self.log.WriteText("wx.Printout.OnEndPrinting\n")
        self.base_OnEndPrinting()

    def OnPreparePrinting(self):
        self.log.WriteText("wx.Printout.OnPreparePrinting\n")
        self.base_OnPreparePrinting()

    def HasPage(self, page):
        self.log.WriteText("wx.Printout.HasPage: %d\n" % page)
        if page <= 2:
            return True
        else:
            return False

    def GetPageInfo(self):
        self.log.WriteText("wx.Printout.GetPageInfo\n")
        return (1, 2, 1, 2)

    def OnPrintPage(self, page):
        self.log.WriteText("wx.Printout.OnPrintPage: %d\n" % page)
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

        # Calculate the position on the DC for centering the graphic
        posX = (w - (self.canvas.getWidth() * actualScale)) / 2.0
        posY = (h - (self.canvas.getHeight() * actualScale)) / 2.0

        # Set the scale and origin
        dc.SetUserScale(actualScale, actualScale)
        dc.SetDeviceOrigin(int(posX), int(posY))

        #-------------------------------------------

        self.canvas.DoDrawing(dc, True)
        dc.DrawText("Page: %d" % page, marginX/2, maxY-marginY)

        return True


#----------------------------------------------------------------------


class TestPrintPanel(wx.Panel):
    def __init__(self, parent, frame, log):
        wx.Panel.__init__(self, parent, -1)
        self.log = log
        self.frame = frame

        self.printData = wx.PrintData()
        self.printData.SetPaperId(wx.PAPER_LETTER)
        self.printData.SetPrintMode(wx.PRINT_MODE_PRINTER)
        
        self.box = wx.BoxSizer(wx.VERTICAL)
        self.canvas = ScrolledWindow.MyCanvas(self)
        self.box.Add(self.canvas, 1, wx.GROW)

        subbox = wx.BoxSizer(wx.HORIZONTAL)
        btn = wx.Button(self, ID_Setup, "Print Setup")
        self.Bind(wx.EVT_BUTTON, self.OnPrintSetup, btn)
        subbox.Add(btn, 1, wx.GROW | wx.ALL, 2)

        btn = wx.Button(self, ID_Preview, "Print Preview")
        self.Bind(wx.EVT_BUTTON, self.OnPrintPreview, btn)
        subbox.Add(btn, 1, wx.GROW | wx.ALL, 2)

        btn = wx.Button(self, ID_Print, "Print")
        self.Bind(wx.EVT_BUTTON, self.OnDoPrint, btn)
        subbox.Add(btn, 1, wx.GROW | wx.ALL, 2)

        self.box.Add(subbox, 0, wx.GROW)

        self.SetAutoLayout(True)
        self.SetSizer(self.box)


    def OnPrintSetup(self, event):
        data = wx.PrintDialogData(self.printData)
        printerDialog = wx.PrintDialog(self, data)
        printerDialog.GetPrintDialogData().SetSetupDialog(True)
        printerDialog.ShowModal();

        # this makes a copy of the wx.PrintData instead of just saving
        # a reference to the one inside the printDialogData that will
        # be destroyed
        self.printData = wx.PrintData( printerDialog.GetPrintDialogData().GetPrintData() )
        
        printerDialog.Destroy()


    def OnPrintPreview(self, event):
        self.log.WriteText("OnPrintPreview\n")
        data = wx.PrintDialogData(self.printData)
        printout = MyPrintout(self.canvas, self.log)
        printout2 = MyPrintout(self.canvas, self.log)
        self.preview = wx.PrintPreview(printout, printout2, data)

        if not self.preview.Ok():
            self.log.WriteText("Houston, we have a problem...\n")
            return

        frame = wx.PreviewFrame(self.preview, self.frame, "This is a print preview")

        frame.Initialize()
        frame.SetPosition(self.frame.GetPosition())
        frame.SetSize(self.frame.GetSize())
        frame.Show(True)



    def OnDoPrint(self, event):
        pdd = wx.PrintDialogData(self.printData)
        pdd.SetToPage(2)
        printer = wx.Printer(pdd)
        printout = MyPrintout(self.canvas, self.log)

        if not printer.Print(self.frame, printout, True):
            wx.MessageBox("There was a problem printing.\nPerhaps your current printer is not set correctly?", "Printing", wx.OK)
        else:
            self.printData = wx.PrintData( printer.GetPrintDialogData().GetPrintData() )
        printout.Destroy()


#----------------------------------------------------------------------

def runTest(frame, nb, log):
    win = TestPrintPanel(nb, frame, log)
    return win


#----------------------------------------------------------------------





overview = """\
<html>
<body>
<h1>PrintFramework</h1>

This is an overview of the classes and methods used to print documents.
It also demonstrates how to do print previews and invoke the printer
setup dialog.

<p>Classes demonstrated here:<P>
<ul>
    <li><b>wx.Printout()</b> - This class encapsulates the functionality of printing out 
        an application document. A new class must be derived and members overridden 
        to respond to calls such as OnPrintPage and HasPage. Instances of this class 
        are passed to wx.Printer.Print() or a wx.PrintPreview object to initiate 
        printing or previewing.<P><p>
        
    <li><b>wx.PrintData()</b> - This class holds a variety of information related to 
        printers and printer device contexts. This class is used to create a 
        wx.PrinterDC and a wx.PostScriptDC. It is also used as a data member of 
        wx.PrintDialogData and wx.PageSetupDialogData, as part of the mechanism for 
        transferring data between the print dialogs and the application.<p><p>

    <li><b>wx.PrintDialog()</b> - This class represents the print and print setup 
        common dialogs. You may obtain a wx.PrinterDC device context from a 
        successfully dismissed print dialog.<p><p>
        
    <li><b>wx.PrintPreview()</b> - Objects of this class manage the print preview 
        process. The object is passed a wx.Printout object, and the wx.PrintPreview 
        object itself is passed to a wx.PreviewFrame object. Previewing is started by 
        initializing and showing the preview frame. Unlike wxPrinter.Print, flow of 
        control returns to the application immediately after the frame is shown.<p><p>
</ul>

<p>Other classes are also demonstrated, but this is the gist of the printer interface
framework in wxPython.

</body>
</html>

"""


if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])] + sys.argv[1:])

