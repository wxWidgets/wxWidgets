# This file was created automatically by SWIG.
import htmlc

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
import wx,htmlhelper
widget = htmlc
class HtmlHistoryItemPtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def GetPos(self, *_args, **_kwargs):
        val = apply(htmlc.HtmlHistoryItem_GetPos,(self,) + _args, _kwargs)
        return val
    def SetPos(self, *_args, **_kwargs):
        val = apply(htmlc.HtmlHistoryItem_SetPos,(self,) + _args, _kwargs)
        return val
    def GetPage(self, *_args, **_kwargs):
        val = apply(htmlc.HtmlHistoryItem_GetPage,(self,) + _args, _kwargs)
        return val
    def GetAnchor(self, *_args, **_kwargs):
        val = apply(htmlc.HtmlHistoryItem_GetAnchor,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C HtmlHistoryItem instance at %s>" % (self.this,)
class HtmlHistoryItem(HtmlHistoryItemPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(htmlc.new_HtmlHistoryItem,_args,_kwargs)
        self.thisown = 1




class wxHtmlWindowPtr(wxScrolledWindowPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def SetPage(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlWindow_SetPage,(self,) + _args, _kwargs)
        return val
    def LoadPage(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlWindow_LoadPage,(self,) + _args, _kwargs)
        return val
    def GetOpenedPage(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlWindow_GetOpenedPage,(self,) + _args, _kwargs)
        return val
    def SetRelatedFrame(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlWindow_SetRelatedFrame,(self,) + _args, _kwargs)
        return val
    def GetRelatedFrame(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlWindow_GetRelatedFrame,(self,) + _args, _kwargs)
        if val: val = wxFramePtr(val) 
        return val
    def SetRelatedStatusBar(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlWindow_SetRelatedStatusBar,(self,) + _args, _kwargs)
        return val
    def SetFonts(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlWindow_SetFonts,(self,) + _args, _kwargs)
        return val
    def SetTitle(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlWindow_SetTitle,(self,) + _args, _kwargs)
        return val
    def SetBorders(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlWindow_SetBorders,(self,) + _args, _kwargs)
        return val
    def HistoryBack(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlWindow_HistoryBack,(self,) + _args, _kwargs)
        return val
    def HistoryForward(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlWindow_HistoryForward,(self,) + _args, _kwargs)
        return val
    def HistoryClear(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlWindow_HistoryClear,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxHtmlWindow instance at %s>" % (self.this,)
class wxHtmlWindow(wxHtmlWindowPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(htmlc.new_wxHtmlWindow,_args,_kwargs)
        self.thisown = 1
        wx._StdWindowCallbacks(self)
        wx._StdOnScrollCallbacks(self)




class wxHtmlHelpControllerPtr(wxEvtHandlerPtr):
    IMG_Book = htmlc.wxHtmlHelpController_IMG_Book
    IMG_Folder = htmlc.wxHtmlHelpController_IMG_Folder
    IMG_Page = htmlc.wxHtmlHelpController_IMG_Page
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __del__(self,htmlc=htmlc):
        if self.thisown == 1 :
            htmlc.delete_wxHtmlHelpController(self)
    def SetTitleFormat(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlHelpController_SetTitleFormat,(self,) + _args, _kwargs)
        return val
    def SetTempDir(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlHelpController_SetTempDir,(self,) + _args, _kwargs)
        return val
    def AddBook(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlHelpController_AddBook,(self,) + _args, _kwargs)
        return val
    def Display(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlHelpController_Display,(self,) + _args, _kwargs)
        return val
    def DisplayID(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlHelpController_DisplayID,(self,) + _args, _kwargs)
        return val
    def DisplayContents(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlHelpController_DisplayContents,(self,) + _args, _kwargs)
        return val
    def DisplayIndex(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlHelpController_DisplayIndex,(self,) + _args, _kwargs)
        return val
    def KeywordSearch(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlHelpController_KeywordSearch,(self,) + _args, _kwargs)
        return val
    def UseConfig(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlHelpController_UseConfig,(self,) + _args, _kwargs)
        return val
    def ReadCustomization(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlHelpController_ReadCustomization,(self,) + _args, _kwargs)
        return val
    def WriteCustomization(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlHelpController_WriteCustomization,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxHtmlHelpController instance at %s>" % (self.this,)
class wxHtmlHelpController(wxHtmlHelpControllerPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(htmlc.new_wxHtmlHelpController,_args,_kwargs)
        self.thisown = 1




class wxHtmlHelpSystemPtr(wxHtmlHelpControllerPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __del__(self,htmlc=htmlc):
        if self.thisown == 1 :
            htmlc.delete_wxHtmlHelpSystem(self)
    def AddBookParam(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlHelpSystem_AddBookParam,(self,) + _args, _kwargs)
        return val
    def CreateToolBar(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlHelpSystem_CreateToolBar,(self,) + _args, _kwargs)
        if val: val = wxToolBarPtr(val) 
        return val
    def CreateContentsTree(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlHelpSystem_CreateContentsTree,(self,) + _args, _kwargs)
        if val: val = wxTreeCtrlPtr(val) 
        return val
    def CreateIndexList(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlHelpSystem_CreateIndexList,(self,) + _args, _kwargs)
        if val: val = wxListBoxPtr(val) 
        return val
    def CreateHelpWindow(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlHelpSystem_CreateHelpWindow,(self,) + _args, _kwargs)
        return val
    def SetControls(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlHelpSystem_SetControls,(self,) + _args, _kwargs)
        return val
    def GetFrame(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlHelpSystem_GetFrame,(self,) + _args, _kwargs)
        if val: val = wxFramePtr(val) 
        return val
    def GetHtmlWindow(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlHelpSystem_GetHtmlWindow,(self,) + _args, _kwargs)
        if val: val = wxHtmlWindowPtr(val) 
        return val
    def GetContentsTree(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlHelpSystem_GetContentsTree,(self,) + _args, _kwargs)
        if val: val = wxTreeCtrlPtr(val) 
        return val
    def GetIndexList(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlHelpSystem_GetIndexList,(self,) + _args, _kwargs)
        if val: val = wxListBoxPtr(val) 
        return val
    def GetSearchList(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlHelpSystem_GetSearchList,(self,) + _args, _kwargs)
        if val: val = wxListBoxPtr(val) 
        return val
    def GetContentsImageList(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlHelpSystem_GetContentsImageList,(self,) + _args, _kwargs)
        if val: val = wxImageListPtr(val) 
        return val
    def OnToolbar(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlHelpSystem_OnToolbar,(self,) + _args, _kwargs)
        return val
    def OnContentsSel(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlHelpSystem_OnContentsSel,(self,) + _args, _kwargs)
        return val
    def OnIndexSel(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlHelpSystem_OnIndexSel,(self,) + _args, _kwargs)
        return val
    def OnSearchSel(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlHelpSystem_OnSearchSel,(self,) + _args, _kwargs)
        return val
    def OnSearch(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlHelpSystem_OnSearch,(self,) + _args, _kwargs)
        return val
    def OnCloseWindow(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlHelpSystem_OnCloseWindow,(self,) + _args, _kwargs)
        return val
    def RefreshLists(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlHelpSystem_RefreshLists,(self,) + _args, _kwargs)
        return val
    def CreateContents(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlHelpSystem_CreateContents,(self,) + _args, _kwargs)
        return val
    def CreateIndex(self, *_args, **_kwargs):
        val = apply(htmlc.wxHtmlHelpSystem_CreateIndex,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxHtmlHelpSystem instance at %s>" % (self.this,)
class wxHtmlHelpSystem(wxHtmlHelpSystemPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(htmlc.new_wxHtmlHelpSystem,_args,_kwargs)
        self.thisown = 1






#-------------- FUNCTION WRAPPERS ------------------



#-------------- VARIABLE WRAPPERS ------------------

wxID_HTML_PANEL = htmlc.wxID_HTML_PANEL
wxID_HTML_BACK = htmlc.wxID_HTML_BACK
wxID_HTML_FORWARD = htmlc.wxID_HTML_FORWARD
wxID_HTML_TREECTRL = htmlc.wxID_HTML_TREECTRL
wxID_HTML_INDEXPAGE = htmlc.wxID_HTML_INDEXPAGE
wxID_HTML_INDEXLIST = htmlc.wxID_HTML_INDEXLIST
wxID_HTML_NOTEBOOK = htmlc.wxID_HTML_NOTEBOOK
wxID_HTML_SEARCHPAGE = htmlc.wxID_HTML_SEARCHPAGE
wxID_HTML_SEARCHTEXT = htmlc.wxID_HTML_SEARCHTEXT
wxID_HTML_SEARCHLIST = htmlc.wxID_HTML_SEARCHLIST
wxID_HTML_SEARCHBUTTON = htmlc.wxID_HTML_SEARCHBUTTON
