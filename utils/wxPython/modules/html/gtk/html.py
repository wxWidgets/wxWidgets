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






#-------------- FUNCTION WRAPPERS ------------------



#-------------- VARIABLE WRAPPERS ------------------

