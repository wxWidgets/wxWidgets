# This file was created automatically by SWIG.
import htmlhelpc

from misc import *

from misc2 import *

from windows import *

from gdi import *

from fonts import *

from clip_dnd import *

from events import *

from streams import *

from utils import *

from mdi import *

from frames import *

from stattool import *

from controls import *

from controls2 import *

from windows2 import *

from cmndlgs import *

from windows3 import *

from image import *

from printfw import *

from sizers import *

from filesys import *
class wxHtmlBookRecordPtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def GetBookFile(self, *_args, **_kwargs):
        val = apply(htmlhelpc.wxHtmlBookRecord_GetBookFile,(self,) + _args, _kwargs)
        return val
    def GetTitle(self, *_args, **_kwargs):
        val = apply(htmlhelpc.wxHtmlBookRecord_GetTitle,(self,) + _args, _kwargs)
        return val
    def GetStart(self, *_args, **_kwargs):
        val = apply(htmlhelpc.wxHtmlBookRecord_GetStart,(self,) + _args, _kwargs)
        return val
    def GetBasePath(self, *_args, **_kwargs):
        val = apply(htmlhelpc.wxHtmlBookRecord_GetBasePath,(self,) + _args, _kwargs)
        return val
    def SetContentsRange(self, *_args, **_kwargs):
        val = apply(htmlhelpc.wxHtmlBookRecord_SetContentsRange,(self,) + _args, _kwargs)
        return val
    def GetContentsStart(self, *_args, **_kwargs):
        val = apply(htmlhelpc.wxHtmlBookRecord_GetContentsStart,(self,) + _args, _kwargs)
        return val
    def GetContentsEnd(self, *_args, **_kwargs):
        val = apply(htmlhelpc.wxHtmlBookRecord_GetContentsEnd,(self,) + _args, _kwargs)
        return val
    def SetTitle(self, *_args, **_kwargs):
        val = apply(htmlhelpc.wxHtmlBookRecord_SetTitle,(self,) + _args, _kwargs)
        return val
    def SetBasePath(self, *_args, **_kwargs):
        val = apply(htmlhelpc.wxHtmlBookRecord_SetBasePath,(self,) + _args, _kwargs)
        return val
    def SetStart(self, *_args, **_kwargs):
        val = apply(htmlhelpc.wxHtmlBookRecord_SetStart,(self,) + _args, _kwargs)
        return val
    def GetFullPath(self, *_args, **_kwargs):
        val = apply(htmlhelpc.wxHtmlBookRecord_GetFullPath,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxHtmlBookRecord instance at %s>" % (self.this,)
class wxHtmlBookRecord(wxHtmlBookRecordPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(htmlhelpc.new_wxHtmlBookRecord,_args,_kwargs)
        self.thisown = 1




class wxHtmlContentsItemPtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def GetLevel(self, *_args, **_kwargs):
        val = apply(htmlhelpc.wxHtmlContentsItem_GetLevel,(self,) + _args, _kwargs)
        return val
    def GetID(self, *_args, **_kwargs):
        val = apply(htmlhelpc.wxHtmlContentsItem_GetID,(self,) + _args, _kwargs)
        return val
    def GetName(self, *_args, **_kwargs):
        val = apply(htmlhelpc.wxHtmlContentsItem_GetName,(self,) + _args, _kwargs)
        return val
    def GetPage(self, *_args, **_kwargs):
        val = apply(htmlhelpc.wxHtmlContentsItem_GetPage,(self,) + _args, _kwargs)
        return val
    def GetBook(self, *_args, **_kwargs):
        val = apply(htmlhelpc.wxHtmlContentsItem_GetBook,(self,) + _args, _kwargs)
        if val: val = wxHtmlBookRecordPtr(val) 
        return val
    def __repr__(self):
        return "<C wxHtmlContentsItem instance at %s>" % (self.this,)
class wxHtmlContentsItem(wxHtmlContentsItemPtr):
    def __init__(self,this):
        self.this = this




class wxHtmlSearchStatusPtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def Search(self, *_args, **_kwargs):
        val = apply(htmlhelpc.wxHtmlSearchStatus_Search,(self,) + _args, _kwargs)
        return val
    def IsActive(self, *_args, **_kwargs):
        val = apply(htmlhelpc.wxHtmlSearchStatus_IsActive,(self,) + _args, _kwargs)
        return val
    def GetCurIndex(self, *_args, **_kwargs):
        val = apply(htmlhelpc.wxHtmlSearchStatus_GetCurIndex,(self,) + _args, _kwargs)
        return val
    def GetMaxIndex(self, *_args, **_kwargs):
        val = apply(htmlhelpc.wxHtmlSearchStatus_GetMaxIndex,(self,) + _args, _kwargs)
        return val
    def GetName(self, *_args, **_kwargs):
        val = apply(htmlhelpc.wxHtmlSearchStatus_GetName,(self,) + _args, _kwargs)
        return val
    def GetContentsItem(self, *_args, **_kwargs):
        val = apply(htmlhelpc.wxHtmlSearchStatus_GetContentsItem,(self,) + _args, _kwargs)
        if val: val = wxHtmlContentsItemPtr(val) 
        return val
    def __repr__(self):
        return "<C wxHtmlSearchStatus instance at %s>" % (self.this,)
class wxHtmlSearchStatus(wxHtmlSearchStatusPtr):
    def __init__(self,this):
        self.this = this




class wxHtmlHelpDataPtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __del__(self, delfunc=htmlhelpc.delete_wxHtmlHelpData):
        if self.thisown == 1:
            try:
                delfunc(self)
            except:
                pass
    def SetTempDir(self, *_args, **_kwargs):
        val = apply(htmlhelpc.wxHtmlHelpData_SetTempDir,(self,) + _args, _kwargs)
        return val
    def AddBook(self, *_args, **_kwargs):
        val = apply(htmlhelpc.wxHtmlHelpData_AddBook,(self,) + _args, _kwargs)
        return val
    def FindPageByName(self, *_args, **_kwargs):
        val = apply(htmlhelpc.wxHtmlHelpData_FindPageByName,(self,) + _args, _kwargs)
        return val
    def FindPageById(self, *_args, **_kwargs):
        val = apply(htmlhelpc.wxHtmlHelpData_FindPageById,(self,) + _args, _kwargs)
        return val
    def GetBookRecArray(self, *_args, **_kwargs):
        val = apply(htmlhelpc.wxHtmlHelpData_GetBookRecArray,(self,) + _args, _kwargs)
        return val
    def GetContents(self, *_args, **_kwargs):
        val = apply(htmlhelpc.wxHtmlHelpData_GetContents,(self,) + _args, _kwargs)
        if val: val = wxHtmlContentsItemPtr(val) 
        return val
    def GetContentsCnt(self, *_args, **_kwargs):
        val = apply(htmlhelpc.wxHtmlHelpData_GetContentsCnt,(self,) + _args, _kwargs)
        return val
    def GetIndex(self, *_args, **_kwargs):
        val = apply(htmlhelpc.wxHtmlHelpData_GetIndex,(self,) + _args, _kwargs)
        if val: val = wxHtmlContentsItemPtr(val) 
        return val
    def GetIndexCnt(self, *_args, **_kwargs):
        val = apply(htmlhelpc.wxHtmlHelpData_GetIndexCnt,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxHtmlHelpData instance at %s>" % (self.this,)
class wxHtmlHelpData(wxHtmlHelpDataPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(htmlhelpc.new_wxHtmlHelpData,_args,_kwargs)
        self.thisown = 1




class wxHtmlHelpFramePtr(wxFramePtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def GetData(self, *_args, **_kwargs):
        val = apply(htmlhelpc.wxHtmlHelpFrame_GetData,(self,) + _args, _kwargs)
        if val: val = wxHtmlHelpDataPtr(val) 
        return val
    def SetTitleFormat(self, *_args, **_kwargs):
        val = apply(htmlhelpc.wxHtmlHelpFrame_SetTitleFormat,(self,) + _args, _kwargs)
        return val
    def Display(self, *_args, **_kwargs):
        val = apply(htmlhelpc.wxHtmlHelpFrame_Display,(self,) + _args, _kwargs)
        return val
    def DisplayID(self, *_args, **_kwargs):
        val = apply(htmlhelpc.wxHtmlHelpFrame_DisplayID,(self,) + _args, _kwargs)
        return val
    def DisplayContents(self, *_args, **_kwargs):
        val = apply(htmlhelpc.wxHtmlHelpFrame_DisplayContents,(self,) + _args, _kwargs)
        return val
    def DisplayIndex(self, *_args, **_kwargs):
        val = apply(htmlhelpc.wxHtmlHelpFrame_DisplayIndex,(self,) + _args, _kwargs)
        return val
    def KeywordSearch(self, *_args, **_kwargs):
        val = apply(htmlhelpc.wxHtmlHelpFrame_KeywordSearch,(self,) + _args, _kwargs)
        return val
    def UseConfig(self, *_args, **_kwargs):
        val = apply(htmlhelpc.wxHtmlHelpFrame_UseConfig,(self,) + _args, _kwargs)
        return val
    def ReadCustomization(self, *_args, **_kwargs):
        val = apply(htmlhelpc.wxHtmlHelpFrame_ReadCustomization,(self,) + _args, _kwargs)
        return val
    def WriteCustomization(self, *_args, **_kwargs):
        val = apply(htmlhelpc.wxHtmlHelpFrame_WriteCustomization,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxHtmlHelpFrame instance at %s>" % (self.this,)
class wxHtmlHelpFrame(wxHtmlHelpFramePtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(htmlhelpc.new_wxHtmlHelpFrame,_args,_kwargs)
        self.thisown = 1
        self._setOORInfo(self)




class wxHtmlHelpControllerPtr(wxEvtHandlerPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __del__(self, delfunc=htmlhelpc.delete_wxHtmlHelpController):
        if self.thisown == 1:
            try:
                delfunc(self)
            except:
                pass
    def SetTitleFormat(self, *_args, **_kwargs):
        val = apply(htmlhelpc.wxHtmlHelpController_SetTitleFormat,(self,) + _args, _kwargs)
        return val
    def SetTempDir(self, *_args, **_kwargs):
        val = apply(htmlhelpc.wxHtmlHelpController_SetTempDir,(self,) + _args, _kwargs)
        return val
    def AddBook(self, *_args, **_kwargs):
        val = apply(htmlhelpc.wxHtmlHelpController_AddBook,(self,) + _args, _kwargs)
        return val
    def Display(self, *_args, **_kwargs):
        val = apply(htmlhelpc.wxHtmlHelpController_Display,(self,) + _args, _kwargs)
        return val
    def DisplayID(self, *_args, **_kwargs):
        val = apply(htmlhelpc.wxHtmlHelpController_DisplayID,(self,) + _args, _kwargs)
        return val
    def DisplayContents(self, *_args, **_kwargs):
        val = apply(htmlhelpc.wxHtmlHelpController_DisplayContents,(self,) + _args, _kwargs)
        return val
    def DisplayIndex(self, *_args, **_kwargs):
        val = apply(htmlhelpc.wxHtmlHelpController_DisplayIndex,(self,) + _args, _kwargs)
        return val
    def KeywordSearch(self, *_args, **_kwargs):
        val = apply(htmlhelpc.wxHtmlHelpController_KeywordSearch,(self,) + _args, _kwargs)
        return val
    def UseConfig(self, *_args, **_kwargs):
        val = apply(htmlhelpc.wxHtmlHelpController_UseConfig,(self,) + _args, _kwargs)
        return val
    def ReadCustomization(self, *_args, **_kwargs):
        val = apply(htmlhelpc.wxHtmlHelpController_ReadCustomization,(self,) + _args, _kwargs)
        return val
    def WriteCustomization(self, *_args, **_kwargs):
        val = apply(htmlhelpc.wxHtmlHelpController_WriteCustomization,(self,) + _args, _kwargs)
        return val
    def GetFrame(self, *_args, **_kwargs):
        val = apply(htmlhelpc.wxHtmlHelpController_GetFrame,(self,) + _args, _kwargs)
        if val: val = wxHtmlHelpFramePtr(val) 
        return val
    def __repr__(self):
        return "<C wxHtmlHelpController instance at %s>" % (self.this,)
class wxHtmlHelpController(wxHtmlHelpControllerPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(htmlhelpc.new_wxHtmlHelpController,_args,_kwargs)
        self.thisown = 1
        self._setOORInfo(self)






#-------------- FUNCTION WRAPPERS ------------------



#-------------- VARIABLE WRAPPERS ------------------

wxHF_TOOLBAR = htmlhelpc.wxHF_TOOLBAR
wxHF_FLATTOOLBAR = htmlhelpc.wxHF_FLATTOOLBAR
wxHF_CONTENTS = htmlhelpc.wxHF_CONTENTS
wxHF_INDEX = htmlhelpc.wxHF_INDEX
wxHF_SEARCH = htmlhelpc.wxHF_SEARCH
wxHF_BOOKMARKS = htmlhelpc.wxHF_BOOKMARKS
wxHF_OPENFILES = htmlhelpc.wxHF_OPENFILES
wxHF_PRINT = htmlhelpc.wxHF_PRINT
wxHF_DEFAULTSTYLE = htmlhelpc.wxHF_DEFAULTSTYLE
