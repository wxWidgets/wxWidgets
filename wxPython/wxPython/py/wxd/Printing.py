"""Decorator classes for documentation and shell scripting.
"""

__author__ = "Patrick K. O'Brien <pobrien@orbtech.com>"
__cvsid__ = "$Id$"
__revision__ = "$Revision$"[11:-2]


# These are not the real wxPython classes. These are Python versions
# for documentation purposes. They are also used to apply docstrings
# to the real wxPython classes, which are SWIG-generated wrappers for
# C-language classes.


from Base import Object
from Dialogs import Dialog
from Frames import Frame
import Parameters as wx


class PageSetupDialog(Dialog):
    """"""

    def GetPageSetupData(self):
        """"""
        pass

    def ShowModal(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


class PageSetupDialogData(Object):
    """"""

    def EnableHelp(self):
        """"""
        pass

    def EnableMargins(self):
        """"""
        pass

    def EnableOrientation(self):
        """"""
        pass

    def EnablePaper(self):
        """"""
        pass

    def EnablePrinter(self):
        """"""
        pass

    def GetDefaultInfo(self):
        """"""
        pass

    def GetDefaultMinMargins(self):
        """"""
        pass

    def GetEnableHelp(self):
        """"""
        pass

    def GetEnableMargins(self):
        """"""
        pass

    def GetEnableOrientation(self):
        """"""
        pass

    def GetEnablePaper(self):
        """"""
        pass

    def GetEnablePrinter(self):
        """"""
        pass

    def GetMarginBottomRight(self):
        """"""
        pass

    def GetMarginTopLeft(self):
        """"""
        pass

    def GetMinMarginBottomRight(self):
        """"""
        pass

    def GetMinMarginTopLeft(self):
        """"""
        pass

    def GetPaperId(self):
        """"""
        pass

    def GetPaperSize(self):
        """"""
        pass

    def GetPrintData(self):
        """"""
        pass

    def Ok(self):
        """"""
        pass

    def SetDefaultInfo(self):
        """"""
        pass

    def SetDefaultMinMargins(self):
        """"""
        pass

    def SetMarginBottomRight(self):
        """"""
        pass

    def SetMarginTopLeft(self):
        """"""
        pass

    def SetMinMarginBottomRight(self):
        """"""
        pass

    def SetMinMarginTopLeft(self):
        """"""
        pass

    def SetPaperId(self):
        """"""
        pass

    def SetPaperSize(self):
        """"""
        pass

    def SetPrintData(self):
        """"""
        pass

    def __del__(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


class PrintDialog(Dialog):
    """"""

    def GetPrintDC(self):
        """"""
        pass

    def GetPrintDialogData(self):
        """"""
        pass

    def ShowModal(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


class PrintDialogData(Object):
    """"""

    def EnableHelp(self):
        """"""
        pass

    def EnablePageNumbers(self):
        """"""
        pass

    def EnablePrintToFile(self):
        """"""
        pass

    def EnableSelection(self):
        """"""
        pass

    def GetAllPages(self):
        """"""
        pass

    def GetCollate(self):
        """"""
        pass

    def GetFromPage(self):
        """"""
        pass

    def GetMaxPage(self):
        """"""
        pass

    def GetMinPage(self):
        """"""
        pass

    def GetNoCopies(self):
        """"""
        pass

    def GetPrintData(self):
        """"""
        pass

    def GetPrintToFile(self):
        """"""
        pass

    def GetToPage(self):
        """"""
        pass

    def Ok(self):
        """"""
        pass

    def SetCollate(self):
        """"""
        pass

    def SetFromPage(self):
        """"""
        pass

    def SetMaxPage(self):
        """"""
        pass

    def SetMinPage(self):
        """"""
        pass

    def SetNoCopies(self):
        """"""
        pass

    def SetPrintData(self):
        """"""
        pass

    def SetPrintToFile(self):
        """"""
        pass

    def SetSetupDialog(self):
        """"""
        pass

    def SetToPage(self):
        """"""
        pass

    def __del__(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


class PreviewFrame(Frame):
    """"""

    def Initialize(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


class PrintData(Object):
    """"""

    def GetCollate(self):
        """"""
        pass

    def GetColour(self):
        """"""
        pass

    def GetDuplex(self):
        """"""
        pass

    def GetFilename(self):
        """"""
        pass

    def GetFontMetricPath(self):
        """"""
        pass

    def GetNoCopies(self):
        """"""
        pass

    def GetOrientation(self):
        """"""
        pass

    def GetPaperId(self):
        """"""
        pass

    def GetPaperSize(self):
        """"""
        pass

    def GetPreviewCommand(self):
        """"""
        pass

    def GetPrintMode(self):
        """"""
        pass

    def GetPrinterCommand(self):
        """"""
        pass

    def GetPrinterName(self):
        """"""
        pass

    def GetPrinterOptions(self):
        """"""
        pass

    def GetPrinterScaleX(self):
        """"""
        pass

    def GetPrinterScaleY(self):
        """"""
        pass

    def GetPrinterTranslateX(self):
        """"""
        pass

    def GetPrinterTranslateY(self):
        """"""
        pass

    def GetQuality(self):
        """"""
        pass

    def Ok(self):
        """"""
        pass

    def SetCollate(self):
        """"""
        pass

    def SetColour(self):
        """"""
        pass

    def SetDuplex(self):
        """"""
        pass

    def SetFilename(self):
        """"""
        pass

    def SetFontMetricPath(self):
        """"""
        pass

    def SetNoCopies(self):
        """"""
        pass

    def SetOrientation(self):
        """"""
        pass

    def SetPaperId(self):
        """"""
        pass

    def SetPaperSize(self):
        """"""
        pass

    def SetPreviewCommand(self):
        """"""
        pass

    def SetPrintMode(self):
        """"""
        pass

    def SetPrinterCommand(self):
        """"""
        pass

    def SetPrinterName(self):
        """"""
        pass

    def SetPrinterOptions(self):
        """"""
        pass

    def SetPrinterScaleX(self):
        """"""
        pass

    def SetPrinterScaleY(self):
        """"""
        pass

    def SetPrinterScaling(self):
        """"""
        pass

    def SetPrinterTranslateX(self):
        """"""
        pass

    def SetPrinterTranslateY(self):
        """"""
        pass

    def SetPrinterTranslation(self):
        """"""
        pass

    def SetQuality(self):
        """"""
        pass

    def __del__(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


class PrintPreview(Object):
    """"""

    def GetCanvas(self):
        """"""
        pass

    def GetCurrentPage(self):
        """"""
        pass

    def GetFrame(self):
        """"""
        pass

    def GetMaxPage(self):
        """"""
        pass

    def GetMinPage(self):
        """"""
        pass

    def GetPrintDialogData(self):
        """"""
        pass

    def GetPrintout(self):
        """"""
        pass

    def GetPrintoutForPrinting(self):
        """"""
        pass

    def GetZoom(self):
        """"""
        pass

    def Ok(self):
        """"""
        pass

    def Print(self):
        """"""
        pass

    def SetCanvas(self):
        """"""
        pass

    def SetCurrentPage(self):
        """"""
        pass

    def SetFrame(self):
        """"""
        pass

    def SetPrintout(self):
        """"""
        pass

    def SetZoom(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


class Printer(Object):
    """"""

    def CreateAbortWindow(self):
        """"""
        pass

    def GetPrintDialogData(self):
        """"""
        pass

    def Print(self):
        """"""
        pass

    def PrintDialog(self):
        """"""
        pass

    def ReportError(self):
        """"""
        pass

    def Setup(self):
        """"""
        pass

    def __del__(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


class Printout(Object):
    """"""

    def Destroy(self):
        """"""
        pass

    def GetDC(self):
        """"""
        pass

    def GetPPIPrinter(self):
        """"""
        pass

    def GetPPIScreen(self):
        """"""
        pass

    def GetPageSizeMM(self):
        """"""
        pass

    def GetPageSizePixels(self):
        """"""
        pass

    def IsPreview(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass

    def _setCallbackInfo(self):
        """"""
        pass

    def base_GetPageInfo(self):
        """"""
        pass

    def base_HasPage(self):
        """"""
        pass

    def base_OnBeginDocument(self):
        """"""
        pass

    def base_OnBeginPrinting(self):
        """"""
        pass

    def base_OnEndDocument(self):
        """"""
        pass

    def base_OnEndPrinting(self):
        """"""
        pass

    def base_OnPreparePrinting(self):
        """"""
        pass

