# This file was created automatically by SWIG.
import printfwc

from misc import *

from windows import *

from gdi import *

from cmndlgs import *

from frames import *

from stattool import *

from controls import *

from events import *
import wx
class wxPrintDataPtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __del__(self):
        if self.thisown == 1 :
            printfwc.delete_wxPrintData(self.this)
    def GetNoCopies(self):
        val = printfwc.wxPrintData_GetNoCopies(self.this)
        return val
    def GetCollate(self):
        val = printfwc.wxPrintData_GetCollate(self.this)
        return val
    def GetOrientation(self):
        val = printfwc.wxPrintData_GetOrientation(self.this)
        return val
    def GetPrinterName(self):
        val = printfwc.wxPrintData_GetPrinterName(self.this)
        return val
    def GetColour(self):
        val = printfwc.wxPrintData_GetColour(self.this)
        return val
    def GetDuplex(self):
        val = printfwc.wxPrintData_GetDuplex(self.this)
        return val
    def GetPaperId(self):
        val = printfwc.wxPrintData_GetPaperId(self.this)
        return val
    def GetPaperSize(self):
        val = printfwc.wxPrintData_GetPaperSize(self.this)
        val = wxSizePtr(val)
        return val
    def GetQuality(self):
        val = printfwc.wxPrintData_GetQuality(self.this)
        return val
    def SetNoCopies(self,arg0):
        val = printfwc.wxPrintData_SetNoCopies(self.this,arg0)
        return val
    def SetCollate(self,arg0):
        val = printfwc.wxPrintData_SetCollate(self.this,arg0)
        return val
    def SetOrientation(self,arg0):
        val = printfwc.wxPrintData_SetOrientation(self.this,arg0)
        return val
    def SetPrinterName(self,arg0):
        val = printfwc.wxPrintData_SetPrinterName(self.this,arg0)
        return val
    def SetColour(self,arg0):
        val = printfwc.wxPrintData_SetColour(self.this,arg0)
        return val
    def SetDuplex(self,arg0):
        val = printfwc.wxPrintData_SetDuplex(self.this,arg0)
        return val
    def SetPaperId(self,arg0):
        val = printfwc.wxPrintData_SetPaperId(self.this,arg0)
        return val
    def SetPaperSize(self,arg0):
        val = printfwc.wxPrintData_SetPaperSize(self.this,arg0.this)
        return val
    def SetQuality(self,arg0):
        val = printfwc.wxPrintData_SetQuality(self.this,arg0)
        return val
    def __repr__(self):
        return "<C wxPrintData instance>"
class wxPrintData(wxPrintDataPtr):
    def __init__(self) :
        self.this = printfwc.new_wxPrintData()
        self.thisown = 1




class wxPageSetupDialogDataPtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __del__(self):
        if self.thisown == 1 :
            printfwc.delete_wxPageSetupDialogData(self.this)
    def EnableHelp(self,arg0):
        val = printfwc.wxPageSetupDialogData_EnableHelp(self.this,arg0)
        return val
    def EnableMargins(self,arg0):
        val = printfwc.wxPageSetupDialogData_EnableMargins(self.this,arg0)
        return val
    def EnableOrientation(self,arg0):
        val = printfwc.wxPageSetupDialogData_EnableOrientation(self.this,arg0)
        return val
    def EnablePaper(self,arg0):
        val = printfwc.wxPageSetupDialogData_EnablePaper(self.this,arg0)
        return val
    def EnablePrinter(self,arg0):
        val = printfwc.wxPageSetupDialogData_EnablePrinter(self.this,arg0)
        return val
    def GetDefaultMinMargins(self):
        val = printfwc.wxPageSetupDialogData_GetDefaultMinMargins(self.this)
        return val
    def GetEnableMargins(self):
        val = printfwc.wxPageSetupDialogData_GetEnableMargins(self.this)
        return val
    def GetEnableOrientation(self):
        val = printfwc.wxPageSetupDialogData_GetEnableOrientation(self.this)
        return val
    def GetEnablePaper(self):
        val = printfwc.wxPageSetupDialogData_GetEnablePaper(self.this)
        return val
    def GetEnablePrinter(self):
        val = printfwc.wxPageSetupDialogData_GetEnablePrinter(self.this)
        return val
    def GetEnableHelp(self):
        val = printfwc.wxPageSetupDialogData_GetEnableHelp(self.this)
        return val
    def GetDefaultInfo(self):
        val = printfwc.wxPageSetupDialogData_GetDefaultInfo(self.this)
        return val
    def GetMarginTopLeft(self):
        val = printfwc.wxPageSetupDialogData_GetMarginTopLeft(self.this)
        val = wxPointPtr(val)
        val.thisown = 1
        return val
    def GetMarginBottomRight(self):
        val = printfwc.wxPageSetupDialogData_GetMarginBottomRight(self.this)
        val = wxPointPtr(val)
        val.thisown = 1
        return val
    def GetMinMarginTopLeft(self):
        val = printfwc.wxPageSetupDialogData_GetMinMarginTopLeft(self.this)
        val = wxPointPtr(val)
        val.thisown = 1
        return val
    def GetMinMarginBottomRight(self):
        val = printfwc.wxPageSetupDialogData_GetMinMarginBottomRight(self.this)
        val = wxPointPtr(val)
        val.thisown = 1
        return val
    def GetPaperId(self):
        val = printfwc.wxPageSetupDialogData_GetPaperId(self.this)
        return val
    def GetPaperSize(self):
        val = printfwc.wxPageSetupDialogData_GetPaperSize(self.this)
        val = wxSizePtr(val)
        val.thisown = 1
        return val
    def GetPrintData(self):
        val = printfwc.wxPageSetupDialogData_GetPrintData(self.this)
        val = wxPrintDataPtr(val)
        val.thisown = 1
        return val
    def SetDefaultInfo(self,arg0):
        val = printfwc.wxPageSetupDialogData_SetDefaultInfo(self.this,arg0)
        return val
    def SetDefaultMinMargins(self,arg0):
        val = printfwc.wxPageSetupDialogData_SetDefaultMinMargins(self.this,arg0)
        return val
    def SetMarginTopLeft(self,arg0):
        val = printfwc.wxPageSetupDialogData_SetMarginTopLeft(self.this,arg0.this)
        return val
    def SetMarginBottomRight(self,arg0):
        val = printfwc.wxPageSetupDialogData_SetMarginBottomRight(self.this,arg0.this)
        return val
    def SetMinMarginTopLeft(self,arg0):
        val = printfwc.wxPageSetupDialogData_SetMinMarginTopLeft(self.this,arg0.this)
        return val
    def SetMinMarginBottomRight(self,arg0):
        val = printfwc.wxPageSetupDialogData_SetMinMarginBottomRight(self.this,arg0.this)
        return val
    def SetPaperId(self,arg0):
        val = printfwc.wxPageSetupDialogData_SetPaperId(self.this,arg0)
        return val
    def SetPaperSize(self,arg0):
        val = printfwc.wxPageSetupDialogData_SetPaperSize(self.this,arg0.this)
        return val
    def SetPrintData(self,arg0):
        val = printfwc.wxPageSetupDialogData_SetPrintData(self.this,arg0.this)
        return val
    def __repr__(self):
        return "<C wxPageSetupDialogData instance>"
class wxPageSetupDialogData(wxPageSetupDialogDataPtr):
    def __init__(self) :
        self.this = printfwc.new_wxPageSetupDialogData()
        self.thisown = 1




class wxPageSetupDialogPtr(wxDialogPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def GetPageSetupData(self):
        val = printfwc.wxPageSetupDialog_GetPageSetupData(self.this)
        val = wxPageSetupDialogDataPtr(val)
        return val
    def ShowModal(self):
        val = printfwc.wxPageSetupDialog_ShowModal(self.this)
        return val
    def __repr__(self):
        return "<C wxPageSetupDialog instance>"
class wxPageSetupDialog(wxPageSetupDialogPtr):
    def __init__(self,arg0,*args) :
        argl = map(None,args)
        try: argl[0] = argl[0].this
        except: pass
        args = tuple(argl)
        self.this = apply(printfwc.new_wxPageSetupDialog,(arg0.this,)+args)
        self.thisown = 1
        wx._StdDialogCallbacks(self)




class wxPrintDialogDataPtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __del__(self):
        if self.thisown == 1 :
            printfwc.delete_wxPrintDialogData(self.this)
    def EnableHelp(self,arg0):
        val = printfwc.wxPrintDialogData_EnableHelp(self.this,arg0)
        return val
    def EnablePageNumbers(self,arg0):
        val = printfwc.wxPrintDialogData_EnablePageNumbers(self.this,arg0)
        return val
    def EnablePrintToFile(self,arg0):
        val = printfwc.wxPrintDialogData_EnablePrintToFile(self.this,arg0)
        return val
    def EnableSelection(self,arg0):
        val = printfwc.wxPrintDialogData_EnableSelection(self.this,arg0)
        return val
    def GetAllPages(self):
        val = printfwc.wxPrintDialogData_GetAllPages(self.this)
        return val
    def GetCollate(self):
        val = printfwc.wxPrintDialogData_GetCollate(self.this)
        return val
    def GetFromPage(self):
        val = printfwc.wxPrintDialogData_GetFromPage(self.this)
        return val
    def GetMaxPage(self):
        val = printfwc.wxPrintDialogData_GetMaxPage(self.this)
        return val
    def GetMinPage(self):
        val = printfwc.wxPrintDialogData_GetMinPage(self.this)
        return val
    def GetNoCopies(self):
        val = printfwc.wxPrintDialogData_GetNoCopies(self.this)
        return val
    def GetPrintData(self):
        val = printfwc.wxPrintDialogData_GetPrintData(self.this)
        val = wxPrintDataPtr(val)
        val.thisown = 1
        return val
    def GetPrintToFile(self):
        val = printfwc.wxPrintDialogData_GetPrintToFile(self.this)
        return val
    def GetToPage(self):
        val = printfwc.wxPrintDialogData_GetToPage(self.this)
        return val
    def SetCollate(self,arg0):
        val = printfwc.wxPrintDialogData_SetCollate(self.this,arg0)
        return val
    def SetFromPage(self,arg0):
        val = printfwc.wxPrintDialogData_SetFromPage(self.this,arg0)
        return val
    def SetMaxPage(self,arg0):
        val = printfwc.wxPrintDialogData_SetMaxPage(self.this,arg0)
        return val
    def SetMinPage(self,arg0):
        val = printfwc.wxPrintDialogData_SetMinPage(self.this,arg0)
        return val
    def SetNoCopies(self,arg0):
        val = printfwc.wxPrintDialogData_SetNoCopies(self.this,arg0)
        return val
    def SetPrintData(self,arg0):
        val = printfwc.wxPrintDialogData_SetPrintData(self.this,arg0.this)
        return val
    def SetPrintToFile(self,arg0):
        val = printfwc.wxPrintDialogData_SetPrintToFile(self.this,arg0)
        return val
    def SetSetupDialog(self,arg0):
        val = printfwc.wxPrintDialogData_SetSetupDialog(self.this,arg0)
        return val
    def SetToPage(self,arg0):
        val = printfwc.wxPrintDialogData_SetToPage(self.this,arg0)
        return val
    def __repr__(self):
        return "<C wxPrintDialogData instance>"
class wxPrintDialogData(wxPrintDialogDataPtr):
    def __init__(self) :
        self.this = printfwc.new_wxPrintDialogData()
        self.thisown = 1




class wxPrintDialogPtr(wxDialogPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def GetPrintDialogData(self):
        val = printfwc.wxPrintDialog_GetPrintDialogData(self.this)
        val = wxPrintDialogDataPtr(val)
        return val
    def GetPrintDC(self):
        val = printfwc.wxPrintDialog_GetPrintDC(self.this)
        val = wxDCPtr(val)
        val.thisown = 1
        return val
    def ShowModal(self):
        val = printfwc.wxPrintDialog_ShowModal(self.this)
        return val
    def __repr__(self):
        return "<C wxPrintDialog instance>"
class wxPrintDialog(wxPrintDialogPtr):
    def __init__(self,arg0,*args) :
        argl = map(None,args)
        try: argl[0] = argl[0].this
        except: pass
        args = tuple(argl)
        self.this = apply(printfwc.new_wxPrintDialog,(arg0.this,)+args)
        self.thisown = 1
        wx._StdDialogCallbacks(self)




class wxPrintoutPtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def _setSelf(self,arg0):
        val = printfwc.wxPrintout__setSelf(self.this,arg0)
        return val
    def Destroy(self):
        val = printfwc.wxPrintout_Destroy(self.this)
        return val
    def GetDC(self):
        val = printfwc.wxPrintout_GetDC(self.this)
        val = wxDCPtr(val)
        return val
    def GetPageSizeMM(self):
        val = printfwc.wxPrintout_GetPageSizeMM(self.this)
        return val
    def GetPageSizePixels(self):
        val = printfwc.wxPrintout_GetPageSizePixels(self.this)
        return val
    def GetPPIPrinter(self):
        val = printfwc.wxPrintout_GetPPIPrinter(self.this)
        return val
    def GetPPIScreen(self):
        val = printfwc.wxPrintout_GetPPIScreen(self.this)
        return val
    def IsPreview(self):
        val = printfwc.wxPrintout_IsPreview(self.this)
        return val
    def base_OnBeginDocument(self,arg0,arg1):
        val = printfwc.wxPrintout_base_OnBeginDocument(self.this,arg0,arg1)
        return val
    def base_OnEndDocument(self):
        val = printfwc.wxPrintout_base_OnEndDocument(self.this)
        return val
    def base_OnBeginPrinting(self):
        val = printfwc.wxPrintout_base_OnBeginPrinting(self.this)
        return val
    def base_OnEndPrinting(self):
        val = printfwc.wxPrintout_base_OnEndPrinting(self.this)
        return val
    def base_OnPreparePrinting(self):
        val = printfwc.wxPrintout_base_OnPreparePrinting(self.this)
        return val
    def base_GetPageInfo(self):
        val = printfwc.wxPrintout_base_GetPageInfo(self.this)
        return val
    def base_HasPage(self,arg0):
        val = printfwc.wxPrintout_base_HasPage(self.this,arg0)
        return val
    def __repr__(self):
        return "<C wxPrintout instance>"
class wxPrintout(wxPrintoutPtr):
    def __init__(self,*args) :
        self.this = apply(printfwc.new_wxPrintout,()+args)
        self.thisown = 1
        self._setSelf(self)




class wxPrinterPtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __del__(self):
        if self.thisown == 1 :
            printfwc.delete_wxPrinter(self.this)
    def CreateAbortWindow(self,arg0,arg1):
        val = printfwc.wxPrinter_CreateAbortWindow(self.this,arg0.this,arg1.this)
        return val
    def GetPrintDialogData(self):
        val = printfwc.wxPrinter_GetPrintDialogData(self.this)
        val = wxPrintDialogDataPtr(val)
        return val
    def Print(self,arg0,arg1,*args):
        val = apply(printfwc.wxPrinter_Print,(self.this,arg0.this,arg1.this,)+args)
        return val
    def PrintDialog(self,arg0):
        val = printfwc.wxPrinter_PrintDialog(self.this,arg0.this)
        val = wxDCPtr(val)
        return val
    def ReportError(self,arg0,arg1,arg2):
        val = printfwc.wxPrinter_ReportError(self.this,arg0.this,arg1.this,arg2)
        return val
    def Setup(self,arg0):
        val = printfwc.wxPrinter_Setup(self.this,arg0.this)
        return val
    def __repr__(self):
        return "<C wxPrinter instance>"
class wxPrinter(wxPrinterPtr):
    def __init__(self,*args) :
        argl = map(None,args)
        try: argl[0] = argl[0].this
        except: pass
        args = tuple(argl)
        self.this = apply(printfwc.new_wxPrinter,()+args)
        self.thisown = 1




class wxPrintPreviewPtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def GetCanvas(self):
        val = printfwc.wxPrintPreview_GetCanvas(self.this)
        val = wxWindowPtr(val)
        return val
    def GetCurrentPage(self):
        val = printfwc.wxPrintPreview_GetCurrentPage(self.this)
        return val
    def GetFrame(self):
        val = printfwc.wxPrintPreview_GetFrame(self.this)
        val = wxFramePtr(val)
        return val
    def GetMaxPage(self):
        val = printfwc.wxPrintPreview_GetMaxPage(self.this)
        return val
    def GetMinPage(self):
        val = printfwc.wxPrintPreview_GetMinPage(self.this)
        return val
    def GetPrintDialogData(self):
        val = printfwc.wxPrintPreview_GetPrintDialogData(self.this)
        val = wxPrintDialogDataPtr(val)
        return val
    def GetPrintout(self):
        val = printfwc.wxPrintPreview_GetPrintout(self.this)
        val = wxPrintoutPtr(val)
        return val
    def GetPrintoutForPrinting(self):
        val = printfwc.wxPrintPreview_GetPrintoutForPrinting(self.this)
        val = wxPrintoutPtr(val)
        return val
    def GetZoom(self):
        val = printfwc.wxPrintPreview_GetZoom(self.this)
        return val
    def Ok(self):
        val = printfwc.wxPrintPreview_Ok(self.this)
        return val
    def Print(self,arg0):
        val = printfwc.wxPrintPreview_Print(self.this,arg0)
        return val
    def SetCanvas(self,arg0):
        val = printfwc.wxPrintPreview_SetCanvas(self.this,arg0.this)
        return val
    def SetCurrentPage(self,arg0):
        val = printfwc.wxPrintPreview_SetCurrentPage(self.this,arg0)
        return val
    def SetFrame(self,arg0):
        val = printfwc.wxPrintPreview_SetFrame(self.this,arg0.this)
        return val
    def SetPrintout(self,arg0):
        val = printfwc.wxPrintPreview_SetPrintout(self.this,arg0.this)
        return val
    def SetZoom(self,arg0):
        val = printfwc.wxPrintPreview_SetZoom(self.this,arg0)
        return val
    def __repr__(self):
        return "<C wxPrintPreview instance>"
class wxPrintPreview(wxPrintPreviewPtr):
    def __init__(self,arg0,arg1,*args) :
        argl = map(None,args)
        try: argl[0] = argl[0].this
        except: pass
        args = tuple(argl)
        self.this = apply(printfwc.new_wxPrintPreview,(arg0.this,arg1.this,)+args)
        self.thisown = 1




class wxPreviewFramePtr(wxFramePtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def Initialize(self):
        val = printfwc.wxPreviewFrame_Initialize(self.this)
        return val
    def __repr__(self):
        return "<C wxPreviewFrame instance>"
class wxPreviewFrame(wxPreviewFramePtr):
    def __init__(self,arg0,arg1,arg2,*args) :
        argl = map(None,args)
        try: argl[0] = argl[0].this
        except: pass
        try: argl[1] = argl[1].this
        except: pass
        args = tuple(argl)
        self.this = apply(printfwc.new_wxPreviewFrame,(arg0.this,arg1.this,arg2,)+args)
        self.thisown = 1
        wx._StdFrameCallbacks(self)






#-------------- FUNCTION WRAPPERS ------------------



#-------------- VARIABLE WRAPPERS ------------------

