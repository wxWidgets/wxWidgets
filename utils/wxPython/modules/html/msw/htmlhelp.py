# This file was created automatically by SWIG.
import htmlhelpc

from misc import *

from misc2 import *

from windows import *

from gdi import *

from events import *

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
class wxHtmlHelpControllerPtr(wxEvtHandlerPtr):
    IMG_Book = htmlhelpc.wxHtmlHelpController_IMG_Book
    IMG_Folder = htmlhelpc.wxHtmlHelpController_IMG_Folder
    IMG_Page = htmlhelpc.wxHtmlHelpController_IMG_Page
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
    def __repr__(self):
        return "<C wxHtmlHelpController instance at %s>" % (self.this,)
class wxHtmlHelpController(wxHtmlHelpControllerPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(htmlhelpc.new_wxHtmlHelpController,_args,_kwargs)
        self.thisown = 1




class wxHtmlHelpSystemPtr(wxHtmlHelpControllerPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __del__(self,htmlhelpc=htmlhelpc):
        if self.thisown == 1 :
            htmlhelpc.delete_wxHtmlHelpSystem(self)
    def AddBookParam(self, *_args, **_kwargs):
        val = apply(htmlhelpc.wxHtmlHelpSystem_AddBookParam,(self,) + _args, _kwargs)
        return val
    def CreateToolBar(self, *_args, **_kwargs):
        val = apply(htmlhelpc.wxHtmlHelpSystem_CreateToolBar,(self,) + _args, _kwargs)
        if val: val = wxToolBarPtr(val) 
        return val
    def CreateContentsTree(self, *_args, **_kwargs):
        val = apply(htmlhelpc.wxHtmlHelpSystem_CreateContentsTree,(self,) + _args, _kwargs)
        if val: val = wxTreeCtrlPtr(val) 
        return val
    def CreateIndexList(self, *_args, **_kwargs):
        val = apply(htmlhelpc.wxHtmlHelpSystem_CreateIndexList,(self,) + _args, _kwargs)
        if val: val = wxListBoxPtr(val) 
        return val
    def CreateHelpWindow(self, *_args, **_kwargs):
        val = apply(htmlhelpc.wxHtmlHelpSystem_CreateHelpWindow,(self,) + _args, _kwargs)
        return val
    def SetControls(self, *_args, **_kwargs):
        val = apply(htmlhelpc.wxHtmlHelpSystem_SetControls,(self,) + _args, _kwargs)
        return val
    def GetFrame(self, *_args, **_kwargs):
        val = apply(htmlhelpc.wxHtmlHelpSystem_GetFrame,(self,) + _args, _kwargs)
        if val: val = wxFramePtr(val) 
        return val
    def GetHtmlWindow(self, *_args, **_kwargs):
        val = apply(htmlhelpc.wxHtmlHelpSystem_GetHtmlWindow,(self,) + _args, _kwargs)
        return val
    def GetContentsTree(self, *_args, **_kwargs):
        val = apply(htmlhelpc.wxHtmlHelpSystem_GetContentsTree,(self,) + _args, _kwargs)
        if val: val = wxTreeCtrlPtr(val) 
        return val
    def GetIndexList(self, *_args, **_kwargs):
        val = apply(htmlhelpc.wxHtmlHelpSystem_GetIndexList,(self,) + _args, _kwargs)
        if val: val = wxListBoxPtr(val) 
        return val
    def GetSearchList(self, *_args, **_kwargs):
        val = apply(htmlhelpc.wxHtmlHelpSystem_GetSearchList,(self,) + _args, _kwargs)
        if val: val = wxListBoxPtr(val) 
        return val
    def GetContentsImageList(self, *_args, **_kwargs):
        val = apply(htmlhelpc.wxHtmlHelpSystem_GetContentsImageList,(self,) + _args, _kwargs)
        if val: val = wxImageListPtr(val) 
        return val
    def OnToolbar(self, *_args, **_kwargs):
        val = apply(htmlhelpc.wxHtmlHelpSystem_OnToolbar,(self,) + _args, _kwargs)
        return val
    def OnContentsSel(self, *_args, **_kwargs):
        val = apply(htmlhelpc.wxHtmlHelpSystem_OnContentsSel,(self,) + _args, _kwargs)
        return val
    def OnIndexSel(self, *_args, **_kwargs):
        val = apply(htmlhelpc.wxHtmlHelpSystem_OnIndexSel,(self,) + _args, _kwargs)
        return val
    def OnSearchSel(self, *_args, **_kwargs):
        val = apply(htmlhelpc.wxHtmlHelpSystem_OnSearchSel,(self,) + _args, _kwargs)
        return val
    def OnSearch(self, *_args, **_kwargs):
        val = apply(htmlhelpc.wxHtmlHelpSystem_OnSearch,(self,) + _args, _kwargs)
        return val
    def OnCloseWindow(self, *_args, **_kwargs):
        val = apply(htmlhelpc.wxHtmlHelpSystem_OnCloseWindow,(self,) + _args, _kwargs)
        return val
    def RefreshLists(self, *_args, **_kwargs):
        val = apply(htmlhelpc.wxHtmlHelpSystem_RefreshLists,(self,) + _args, _kwargs)
        return val
    def CreateContents(self, *_args, **_kwargs):
        val = apply(htmlhelpc.wxHtmlHelpSystem_CreateContents,(self,) + _args, _kwargs)
        return val
    def CreateIndex(self, *_args, **_kwargs):
        val = apply(htmlhelpc.wxHtmlHelpSystem_CreateIndex,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxHtmlHelpSystem instance at %s>" % (self.this,)
class wxHtmlHelpSystem(wxHtmlHelpSystemPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(htmlhelpc.new_wxHtmlHelpSystem,_args,_kwargs)
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
