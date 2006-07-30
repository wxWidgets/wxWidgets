# This file was created automatically by SWIG.
import printfwc

from misc import *

from windows import *

from gdi import *

from clip_dnd import *

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
    def __del__(self,printfwc=printfwc):
        if self.thisown == 1 :
            printfwc.delete_wxPrintData(self)
    def GetNoCopies(self, *_args, **_kwargs):
        val = apply(printfwc.wxPrintData_GetNoCopies,(self,) + _args, _kwargs)
        return val
    def GetCollate(self, *_args, **_kwargs):
        val = apply(printfwc.wxPrintData_GetCollate,(self,) + _args, _kwargs)
        return val
    def GetOrientation(self, *_args, **_kwargs):
        val = apply(printfwc.wxPrintData_GetOrientation,(self,) + _args, _kwargs)
        return val
    def GetPrinterName(self, *_args, **_kwargs):
        val = apply(printfwc.wxPrintData_GetPrinterName,(self,) + _args, _kwargs)
        return val
    def GetColour(self, *_args, **_kwargs):
        val = apply(printfwc.wxPrintData_GetColour,(self,) + _args, _kwargs)
        return val
    def GetDuplex(self, *_args, **_kwargs):
        val = apply(printfwc.wxPrintData_GetDuplex,(self,) + _args, _kwargs)
        return val
    def GetPaperId(self, *_args, **_kwargs):
        val = apply(printfwc.wxPrintData_GetPaperId,(self,) + _args, _kwargs)
        return val
    def GetPaperSize(self, *_args, **_kwargs):
        val = apply(printfwc.wxPrintData_GetPaperSize,(self,) + _args, _kwargs)
        if val: val = wxSizePtr(val) 
        return val
    def GetQuality(self, *_args, **_kwargs):
        val = apply(printfwc.wxPrintData_GetQuality,(self,) + _args, _kwargs)
        return val
    def SetNoCopies(self, *_args, **_kwargs):
        val = apply(printfwc.wxPrintData_SetNoCopies,(self,) + _args, _kwargs)
        return val
    def SetCollate(self, *_args, **_kwargs):
        val = apply(printfwc.wxPrintData_SetCollate,(self,) + _args, _kwargs)
        return val
    def SetOrientation(self, *_args, **_kwargs):
        val = apply(printfwc.wxPrintData_SetOrientation,(self,) + _args, _kwargs)
        return val
    def SetPrinterName(self, *_args, **_kwargs):
        val = apply(printfwc.wxPrintData_SetPrinterName,(self,) + _args, _kwargs)
        return val
    def SetColour(self, *_args, **_kwargs):
        val = apply(printfwc.wxPrintData_SetColour,(self,) + _args, _kwargs)
        return val
    def SetDuplex(self, *_args, **_kwargs):
        val = apply(printfwc.wxPrintData_SetDuplex,(self,) + _args, _kwargs)
        return val
    def SetPaperId(self, *_args, **_kwargs):
        val = apply(printfwc.wxPrintData_SetPaperId,(self,) + _args, _kwargs)
        return val
    def SetPaperSize(self, *_args, **_kwargs):
        val = apply(printfwc.wxPrintData_SetPaperSize,(self,) + _args, _kwargs)
        return val
    def SetQuality(self, *_args, **_kwargs):
        val = apply(printfwc.wxPrintData_SetQuality,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxPrintData instance at %s>" % (self.this,)
class wxPrintData(wxPrintDataPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(printfwc.new_wxPrintData,_args,_kwargs)
        self.thisown = 1




class wxPageSetupDialogDataPtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __del__(self,printfwc=printfwc):
        if self.thisown == 1 :
            printfwc.delete_wxPageSetupDialogData(self)
    def EnableHelp(self, *_args, **_kwargs):
        val = apply(printfwc.wxPageSetupDialogData_EnableHelp,(self,) + _args, _kwargs)
        return val
    def EnableMargins(self, *_args, **_kwargs):
        val = apply(printfwc.wxPageSetupDialogData_EnableMargins,(self,) + _args, _kwargs)
        return val
    def EnableOrientation(self, *_args, **_kwargs):
        val = apply(printfwc.wxPageSetupDialogData_EnableOrientation,(self,) + _args, _kwargs)
        return val
    def EnablePaper(self, *_args, **_kwargs):
        val = apply(printfwc.wxPageSetupDialogData_EnablePaper,(self,) + _args, _kwargs)
        return val
    def EnablePrinter(self, *_args, **_kwargs):
        val = apply(printfwc.wxPageSetupDialogData_EnablePrinter,(self,) + _args, _kwargs)
        return val
    def GetDefaultMinMargins(self, *_args, **_kwargs):
        val = apply(printfwc.wxPageSetupDialogData_GetDefaultMinMargins,(self,) + _args, _kwargs)
        return val
    def GetEnableMargins(self, *_args, **_kwargs):
        val = apply(printfwc.wxPageSetupDialogData_GetEnableMargins,(self,) + _args, _kwargs)
        return val
    def GetEnableOrientation(self, *_args, **_kwargs):
        val = apply(printfwc.wxPageSetupDialogData_GetEnableOrientation,(self,) + _args, _kwargs)
        return val
    def GetEnablePaper(self, *_args, **_kwargs):
        val = apply(printfwc.wxPageSetupDialogData_GetEnablePaper,(self,) + _args, _kwargs)
        return val
    def GetEnablePrinter(self, *_args, **_kwargs):
        val = apply(printfwc.wxPageSetupDialogData_GetEnablePrinter,(self,) + _args, _kwargs)
        return val
    def GetEnableHelp(self, *_args, **_kwargs):
        val = apply(printfwc.wxPageSetupDialogData_GetEnableHelp,(self,) + _args, _kwargs)
        return val
    def GetDefaultInfo(self, *_args, **_kwargs):
        val = apply(printfwc.wxPageSetupDialogData_GetDefaultInfo,(self,) + _args, _kwargs)
        return val
    def GetMarginTopLeft(self, *_args, **_kwargs):
        val = apply(printfwc.wxPageSetupDialogData_GetMarginTopLeft,(self,) + _args, _kwargs)
        if val: val = wxPointPtr(val) ; val.thisown = 1
        return val
    def GetMarginBottomRight(self, *_args, **_kwargs):
        val = apply(printfwc.wxPageSetupDialogData_GetMarginBottomRight,(self,) + _args, _kwargs)
        if val: val = wxPointPtr(val) ; val.thisown = 1
        return val
    def GetMinMarginTopLeft(self, *_args, **_kwargs):
        val = apply(printfwc.wxPageSetupDialogData_GetMinMarginTopLeft,(self,) + _args, _kwargs)
        if val: val = wxPointPtr(val) ; val.thisown = 1
        return val
    def GetMinMarginBottomRight(self, *_args, **_kwargs):
        val = apply(printfwc.wxPageSetupDialogData_GetMinMarginBottomRight,(self,) + _args, _kwargs)
        if val: val = wxPointPtr(val) ; val.thisown = 1
        return val
    def GetPaperId(self, *_args, **_kwargs):
        val = apply(printfwc.wxPageSetupDialogData_GetPaperId,(self,) + _args, _kwargs)
        return val
    def GetPaperSize(self, *_args, **_kwargs):
        val = apply(printfwc.wxPageSetupDialogData_GetPaperSize,(self,) + _args, _kwargs)
        if val: val = wxSizePtr(val) ; val.thisown = 1
        return val
    def GetPrintData(self, *_args, **_kwargs):
        val = apply(printfwc.wxPageSetupDialogData_GetPrintData,(self,) + _args, _kwargs)
        if val: val = wxPrintDataPtr(val) ; val.thisown = 1
        return val
    def SetDefaultInfo(self, *_args, **_kwargs):
        val = apply(printfwc.wxPageSetupDialogData_SetDefaultInfo,(self,) + _args, _kwargs)
        return val
    def SetDefaultMinMargins(self, *_args, **_kwargs):
        val = apply(printfwc.wxPageSetupDialogData_SetDefaultMinMargins,(self,) + _args, _kwargs)
        return val
    def SetMarginTopLeft(self, *_args, **_kwargs):
        val = apply(printfwc.wxPageSetupDialogData_SetMarginTopLeft,(self,) + _args, _kwargs)
        return val
    def SetMarginBottomRight(self, *_args, **_kwargs):
        val = apply(printfwc.wxPageSetupDialogData_SetMarginBottomRight,(self,) + _args, _kwargs)
        return val
    def SetMinMarginTopLeft(self, *_args, **_kwargs):
        val = apply(printfwc.wxPageSetupDialogData_SetMinMarginTopLeft,(self,) + _args, _kwargs)
        return val
    def SetMinMarginBottomRight(self, *_args, **_kwargs):
        val = apply(printfwc.wxPageSetupDialogData_SetMinMarginBottomRight,(self,) + _args, _kwargs)
        return val
    def SetPaperId(self, *_args, **_kwargs):
        val = apply(printfwc.wxPageSetupDialogData_SetPaperId,(self,) + _args, _kwargs)
        return val
    def SetPaperSize(self, *_args, **_kwargs):
        val = apply(printfwc.wxPageSetupDialogData_SetPaperSize,(self,) + _args, _kwargs)
        return val
    def SetPrintData(self, *_args, **_kwargs):
        val = apply(printfwc.wxPageSetupDialogData_SetPrintData,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxPageSetupDialogData instance at %s>" % (self.this,)
class wxPageSetupDialogData(wxPageSetupDialogDataPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(printfwc.new_wxPageSetupDialogData,_args,_kwargs)
        self.thisown = 1




class wxPageSetupDialogPtr(wxDialogPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def GetPageSetupData(self, *_args, **_kwargs):
        val = apply(printfwc.wxPageSetupDialog_GetPageSetupData,(self,) + _args, _kwargs)
        if val: val = wxPageSetupDialogDataPtr(val) 
        return val
    def ShowModal(self, *_args, **_kwargs):
        val = apply(printfwc.wxPageSetupDialog_ShowModal,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxPageSetupDialog instance at %s>" % (self.this,)
class wxPageSetupDialog(wxPageSetupDialogPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(printfwc.new_wxPageSetupDialog,_args,_kwargs)
        self.thisown = 1
        wx._StdDialogCallbacks(self)




class wxPrintDialogDataPtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __del__(self,printfwc=printfwc):
        if self.thisown == 1 :
            printfwc.delete_wxPrintDialogData(self)
    def EnableHelp(self, *_args, **_kwargs):
        val = apply(printfwc.wxPrintDialogData_EnableHelp,(self,) + _args, _kwargs)
        return val
    def EnablePageNumbers(self, *_args, **_kwargs):
        val = apply(printfwc.wxPrintDialogData_EnablePageNumbers,(self,) + _args, _kwargs)
        return val
    def EnablePrintToFile(self, *_args, **_kwargs):
        val = apply(printfwc.wxPrintDialogData_EnablePrintToFile,(self,) + _args, _kwargs)
        return val
    def EnableSelection(self, *_args, **_kwargs):
        val = apply(printfwc.wxPrintDialogData_EnableSelection,(self,) + _args, _kwargs)
        return val
    def GetAllPages(self, *_args, **_kwargs):
        val = apply(printfwc.wxPrintDialogData_GetAllPages,(self,) + _args, _kwargs)
        return val
    def GetCollate(self, *_args, **_kwargs):
        val = apply(printfwc.wxPrintDialogData_GetCollate,(self,) + _args, _kwargs)
        return val
    def GetFromPage(self, *_args, **_kwargs):
        val = apply(printfwc.wxPrintDialogData_GetFromPage,(self,) + _args, _kwargs)
        return val
    def GetMaxPage(self, *_args, **_kwargs):
        val = apply(printfwc.wxPrintDialogData_GetMaxPage,(self,) + _args, _kwargs)
        return val
    def GetMinPage(self, *_args, **_kwargs):
        val = apply(printfwc.wxPrintDialogData_GetMinPage,(self,) + _args, _kwargs)
        return val
    def GetNoCopies(self, *_args, **_kwargs):
        val = apply(printfwc.wxPrintDialogData_GetNoCopies,(self,) + _args, _kwargs)
        return val
    def GetPrintData(self, *_args, **_kwargs):
        val = apply(printfwc.wxPrintDialogData_GetPrintData,(self,) + _args, _kwargs)
        if val: val = wxPrintDataPtr(val) ; val.thisown = 1
        return val
    def GetPrintToFile(self, *_args, **_kwargs):
        val = apply(printfwc.wxPrintDialogData_GetPrintToFile,(self,) + _args, _kwargs)
        return val
    def GetToPage(self, *_args, **_kwargs):
        val = apply(printfwc.wxPrintDialogData_GetToPage,(self,) + _args, _kwargs)
        return val
    def SetCollate(self, *_args, **_kwargs):
        val = apply(printfwc.wxPrintDialogData_SetCollate,(self,) + _args, _kwargs)
        return val
    def SetFromPage(self, *_args, **_kwargs):
        val = apply(printfwc.wxPrintDialogData_SetFromPage,(self,) + _args, _kwargs)
        return val
    def SetMaxPage(self, *_args, **_kwargs):
        val = apply(printfwc.wxPrintDialogData_SetMaxPage,(self,) + _args, _kwargs)
        return val
    def SetMinPage(self, *_args, **_kwargs):
        val = apply(printfwc.wxPrintDialogData_SetMinPage,(self,) + _args, _kwargs)
        return val
    def SetNoCopies(self, *_args, **_kwargs):
        val = apply(printfwc.wxPrintDialogData_SetNoCopies,(self,) + _args, _kwargs)
        return val
    def SetPrintData(self, *_args, **_kwargs):
        val = apply(printfwc.wxPrintDialogData_SetPrintData,(self,) + _args, _kwargs)
        return val
    def SetPrintToFile(self, *_args, **_kwargs):
        val = apply(printfwc.wxPrintDialogData_SetPrintToFile,(self,) + _args, _kwargs)
        return val
    def SetSetupDialog(self, *_args, **_kwargs):
        val = apply(printfwc.wxPrintDialogData_SetSetupDialog,(self,) + _args, _kwargs)
        return val
    def SetToPage(self, *_args, **_kwargs):
        val = apply(printfwc.wxPrintDialogData_SetToPage,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxPrintDialogData instance at %s>" % (self.this,)
class wxPrintDialogData(wxPrintDialogDataPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(printfwc.new_wxPrintDialogData,_args,_kwargs)
        self.thisown = 1




class wxPrintDialogPtr(wxDialogPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def GetPrintDialogData(self, *_args, **_kwargs):
        val = apply(printfwc.wxPrintDialog_GetPrintDialogData,(self,) + _args, _kwargs)
        if val: val = wxPrintDialogDataPtr(val) 
        return val
    def GetPrintDC(self, *_args, **_kwargs):
        val = apply(printfwc.wxPrintDialog_GetPrintDC,(self,) + _args, _kwargs)
        if val: val = wxDCPtr(val) ; val.thisown = 1
        return val
    def ShowModal(self, *_args, **_kwargs):
        val = apply(printfwc.wxPrintDialog_ShowModal,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxPrintDialog instance at %s>" % (self.this,)
class wxPrintDialog(wxPrintDialogPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(printfwc.new_wxPrintDialog,_args,_kwargs)
        self.thisown = 1
        wx._StdDialogCallbacks(self)




class wxPrintoutPtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def _setSelf(self, *_args, **_kwargs):
        val = apply(printfwc.wxPrintout__setSelf,(self,) + _args, _kwargs)
        return val
    def Destroy(self, *_args, **_kwargs):
        val = apply(printfwc.wxPrintout_Destroy,(self,) + _args, _kwargs)
        return val
    def GetDC(self, *_args, **_kwargs):
        val = apply(printfwc.wxPrintout_GetDC,(self,) + _args, _kwargs)
        if val: val = wxDCPtr(val) 
        return val
    def GetPageSizeMM(self, *_args, **_kwargs):
        val = apply(printfwc.wxPrintout_GetPageSizeMM,(self,) + _args, _kwargs)
        return val
    def GetPageSizePixels(self, *_args, **_kwargs):
        val = apply(printfwc.wxPrintout_GetPageSizePixels,(self,) + _args, _kwargs)
        return val
    def GetPPIPrinter(self, *_args, **_kwargs):
        val = apply(printfwc.wxPrintout_GetPPIPrinter,(self,) + _args, _kwargs)
        return val
    def GetPPIScreen(self, *_args, **_kwargs):
        val = apply(printfwc.wxPrintout_GetPPIScreen,(self,) + _args, _kwargs)
        return val
    def IsPreview(self, *_args, **_kwargs):
        val = apply(printfwc.wxPrintout_IsPreview,(self,) + _args, _kwargs)
        return val
    def base_OnBeginDocument(self, *_args, **_kwargs):
        val = apply(printfwc.wxPrintout_base_OnBeginDocument,(self,) + _args, _kwargs)
        return val
    def base_OnEndDocument(self, *_args, **_kwargs):
        val = apply(printfwc.wxPrintout_base_OnEndDocument,(self,) + _args, _kwargs)
        return val
    def base_OnBeginPrinting(self, *_args, **_kwargs):
        val = apply(printfwc.wxPrintout_base_OnBeginPrinting,(self,) + _args, _kwargs)
        return val
    def base_OnEndPrinting(self, *_args, **_kwargs):
        val = apply(printfwc.wxPrintout_base_OnEndPrinting,(self,) + _args, _kwargs)
        return val
    def base_OnPreparePrinting(self, *_args, **_kwargs):
        val = apply(printfwc.wxPrintout_base_OnPreparePrinting,(self,) + _args, _kwargs)
        return val
    def base_GetPageInfo(self, *_args, **_kwargs):
        val = apply(printfwc.wxPrintout_base_GetPageInfo,(self,) + _args, _kwargs)
        return val
    def base_HasPage(self, *_args, **_kwargs):
        val = apply(printfwc.wxPrintout_base_HasPage,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxPrintout instance at %s>" % (self.this,)
class wxPrintout(wxPrintoutPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(printfwc.new_wxPrintout,_args,_kwargs)
        self.thisown = 1
        self._setSelf(self)




class wxPrinterPtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __del__(self,printfwc=printfwc):
        if self.thisown == 1 :
            printfwc.delete_wxPrinter(self)
    def CreateAbortWindow(self, *_args, **_kwargs):
        val = apply(printfwc.wxPrinter_CreateAbortWindow,(self,) + _args, _kwargs)
        return val
    def GetPrintDialogData(self, *_args, **_kwargs):
        val = apply(printfwc.wxPrinter_GetPrintDialogData,(self,) + _args, _kwargs)
        if val: val = wxPrintDialogDataPtr(val) 
        return val
    def Print(self, *_args, **_kwargs):
        val = apply(printfwc.wxPrinter_Print,(self,) + _args, _kwargs)
        return val
    def PrintDialog(self, *_args, **_kwargs):
        val = apply(printfwc.wxPrinter_PrintDialog,(self,) + _args, _kwargs)
        if val: val = wxDCPtr(val) 
        return val
    def ReportError(self, *_args, **_kwargs):
        val = apply(printfwc.wxPrinter_ReportError,(self,) + _args, _kwargs)
        return val
    def Setup(self, *_args, **_kwargs):
        val = apply(printfwc.wxPrinter_Setup,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxPrinter instance at %s>" % (self.this,)
class wxPrinter(wxPrinterPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(printfwc.new_wxPrinter,_args,_kwargs)
        self.thisown = 1




class wxPrintPreviewPtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def GetCanvas(self, *_args, **_kwargs):
        val = apply(printfwc.wxPrintPreview_GetCanvas,(self,) + _args, _kwargs)
        if val: val = wxWindowPtr(val) 
        return val
    def GetCurrentPage(self, *_args, **_kwargs):
        val = apply(printfwc.wxPrintPreview_GetCurrentPage,(self,) + _args, _kwargs)
        return val
    def GetFrame(self, *_args, **_kwargs):
        val = apply(printfwc.wxPrintPreview_GetFrame,(self,) + _args, _kwargs)
        if val: val = wxFramePtr(val) 
        return val
    def GetMaxPage(self, *_args, **_kwargs):
        val = apply(printfwc.wxPrintPreview_GetMaxPage,(self,) + _args, _kwargs)
        return val
    def GetMinPage(self, *_args, **_kwargs):
        val = apply(printfwc.wxPrintPreview_GetMinPage,(self,) + _args, _kwargs)
        return val
    def GetPrintDialogData(self, *_args, **_kwargs):
        val = apply(printfwc.wxPrintPreview_GetPrintDialogData,(self,) + _args, _kwargs)
        if val: val = wxPrintDialogDataPtr(val) 
        return val
    def GetPrintout(self, *_args, **_kwargs):
        val = apply(printfwc.wxPrintPreview_GetPrintout,(self,) + _args, _kwargs)
        if val: val = wxPrintoutPtr(val) 
        return val
    def GetPrintoutForPrinting(self, *_args, **_kwargs):
        val = apply(printfwc.wxPrintPreview_GetPrintoutForPrinting,(self,) + _args, _kwargs)
        if val: val = wxPrintoutPtr(val) 
        return val
    def GetZoom(self, *_args, **_kwargs):
        val = apply(printfwc.wxPrintPreview_GetZoom,(self,) + _args, _kwargs)
        return val
    def Ok(self, *_args, **_kwargs):
        val = apply(printfwc.wxPrintPreview_Ok,(self,) + _args, _kwargs)
        return val
    def Print(self, *_args, **_kwargs):
        val = apply(printfwc.wxPrintPreview_Print,(self,) + _args, _kwargs)
        return val
    def SetCanvas(self, *_args, **_kwargs):
        val = apply(printfwc.wxPrintPreview_SetCanvas,(self,) + _args, _kwargs)
        return val
    def SetCurrentPage(self, *_args, **_kwargs):
        val = apply(printfwc.wxPrintPreview_SetCurrentPage,(self,) + _args, _kwargs)
        return val
    def SetFrame(self, *_args, **_kwargs):
        val = apply(printfwc.wxPrintPreview_SetFrame,(self,) + _args, _kwargs)
        return val
    def SetPrintout(self, *_args, **_kwargs):
        val = apply(printfwc.wxPrintPreview_SetPrintout,(self,) + _args, _kwargs)
        return val
    def SetZoom(self, *_args, **_kwargs):
        val = apply(printfwc.wxPrintPreview_SetZoom,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxPrintPreview instance at %s>" % (self.this,)
class wxPrintPreview(wxPrintPreviewPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(printfwc.new_wxPrintPreview,_args,_kwargs)
        self.thisown = 1




class wxPreviewFramePtr(wxFramePtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def Initialize(self, *_args, **_kwargs):
        val = apply(printfwc.wxPreviewFrame_Initialize,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxPreviewFrame instance at %s>" % (self.this,)
class wxPreviewFrame(wxPreviewFramePtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(printfwc.new_wxPreviewFrame,_args,_kwargs)
        self.thisown = 1
        wx._StdFrameCallbacks(self)






#-------------- FUNCTION WRAPPERS ------------------



#-------------- VARIABLE WRAPPERS ------------------

