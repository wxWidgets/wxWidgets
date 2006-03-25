# This file was created automatically by SWIG.
import htmlhelpc

from misc import *

from misc2 import *

from windows import *

from gdi import *

from clip_dnd import *

from events import *

from streams import *

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

from utils import *
class wxHtmlHelpFrameCfgPtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __setattr__(self,name,value):
        if name == "x" :
            htmlhelpc.wxHtmlHelpFrameCfg_x_set(self,value)
            return
        if name == "y" :
            htmlhelpc.wxHtmlHelpFrameCfg_y_set(self,value)
            return
        if name == "w" :
            htmlhelpc.wxHtmlHelpFrameCfg_w_set(self,value)
            return
        if name == "h" :
            htmlhelpc.wxHtmlHelpFrameCfg_h_set(self,value)
            return
        if name == "sashpos" :
            htmlhelpc.wxHtmlHelpFrameCfg_sashpos_set(self,value)
            return
        if name == "navig_on" :
            htmlhelpc.wxHtmlHelpFrameCfg_navig_on_set(self,value)
            return
        self.__dict__[name] = value
    def __getattr__(self,name):
        if name == "x" : 
            return htmlhelpc.wxHtmlHelpFrameCfg_x_get(self)
        if name == "y" : 
            return htmlhelpc.wxHtmlHelpFrameCfg_y_get(self)
        if name == "w" : 
            return htmlhelpc.wxHtmlHelpFrameCfg_w_get(self)
        if name == "h" : 
            return htmlhelpc.wxHtmlHelpFrameCfg_h_get(self)
        if name == "sashpos" : 
            return htmlhelpc.wxHtmlHelpFrameCfg_sashpos_get(self)
        if name == "navig_on" : 
            return htmlhelpc.wxHtmlHelpFrameCfg_navig_on_get(self)
        raise AttributeError,name
    def __repr__(self):
        return "<C wxHtmlHelpFrameCfg instance at %s>" % (self.this,)
class wxHtmlHelpFrameCfg(wxHtmlHelpFrameCfgPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(htmlhelpc.new_wxHtmlHelpFrameCfg,_args,_kwargs)
        self.thisown = 1




class wxHtmlBookRecordPtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
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
    def __setattr__(self,name,value):
        if name == "m_Level" :
            htmlhelpc.wxHtmlContentsItem_m_Level_set(self,value)
            return
        if name == "m_ID" :
            htmlhelpc.wxHtmlContentsItem_m_ID_set(self,value)
            return
        if name == "m_Name" :
            htmlhelpc.wxHtmlContentsItem_m_Name_set(self,value)
            return
        if name == "m_Page" :
            htmlhelpc.wxHtmlContentsItem_m_Page_set(self,value)
            return
        if name == "m_Book" :
            htmlhelpc.wxHtmlContentsItem_m_Book_set(self,value.this)
            return
        self.__dict__[name] = value
    def __getattr__(self,name):
        if name == "m_Level" : 
            return htmlhelpc.wxHtmlContentsItem_m_Level_get(self)
        if name == "m_ID" : 
            return htmlhelpc.wxHtmlContentsItem_m_ID_get(self)
        if name == "m_Name" : 
            return htmlhelpc.wxHtmlContentsItem_m_Name_get(self)
        if name == "m_Page" : 
            return htmlhelpc.wxHtmlContentsItem_m_Page_get(self)
        if name == "m_Book" : 
            return wxHtmlBookRecordPtr(htmlhelpc.wxHtmlContentsItem_m_Book_get(self))
        raise AttributeError,name
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
    def __del__(self,htmlhelpc=htmlhelpc):
        if self.thisown == 1 :
            htmlhelpc.delete_wxHtmlHelpData(self)
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




class wxHtmlHelpControllerPtr(wxEvtHandlerPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __del__(self,htmlhelpc=htmlhelpc):
        if self.thisown == 1 :
            htmlhelpc.delete_wxHtmlHelpController(self)
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






#-------------- FUNCTION WRAPPERS ------------------



#-------------- VARIABLE WRAPPERS ------------------

wxID_HTML_PANEL = htmlhelpc.wxID_HTML_PANEL
wxID_HTML_BACK = htmlhelpc.wxID_HTML_BACK
wxID_HTML_FORWARD = htmlhelpc.wxID_HTML_FORWARD
wxID_HTML_TREECTRL = htmlhelpc.wxID_HTML_TREECTRL
wxID_HTML_INDEXPAGE = htmlhelpc.wxID_HTML_INDEXPAGE
wxID_HTML_INDEXLIST = htmlhelpc.wxID_HTML_INDEXLIST
wxID_HTML_NOTEBOOK = htmlhelpc.wxID_HTML_NOTEBOOK
wxID_HTML_SEARCHPAGE = htmlhelpc.wxID_HTML_SEARCHPAGE
wxID_HTML_SEARCHTEXT = htmlhelpc.wxID_HTML_SEARCHTEXT
wxID_HTML_SEARCHLIST = htmlhelpc.wxID_HTML_SEARCHLIST
wxID_HTML_SEARCHBUTTON = htmlhelpc.wxID_HTML_SEARCHBUTTON
wxID_HTML_SEARCHCHOICE = htmlhelpc.wxID_HTML_SEARCHCHOICE
wxID_HTML_HELPFRAME = htmlhelpc.wxID_HTML_HELPFRAME
