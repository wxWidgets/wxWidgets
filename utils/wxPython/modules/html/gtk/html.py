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
    def GetPos(self):
        val = htmlc.HtmlHistoryItem_GetPos(self.this)
        return val
    def SetPos(self,arg0):
        val = htmlc.HtmlHistoryItem_SetPos(self.this,arg0)
        return val
    def GetPage(self):
        val = htmlc.HtmlHistoryItem_GetPage(self.this)
        return val
    def GetAnchor(self):
        val = htmlc.HtmlHistoryItem_GetAnchor(self.this)
        return val
    def __repr__(self):
        return "<C HtmlHistoryItem instance>"
class HtmlHistoryItem(HtmlHistoryItemPtr):
    def __init__(self,arg0,arg1) :
        self.this = htmlc.new_HtmlHistoryItem(arg0,arg1)
        self.thisown = 1




class wxHtmlWindowPtr(wxScrolledWindowPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def SetPage(self,arg0):
        val = htmlc.wxHtmlWindow_SetPage(self.this,arg0)
        return val
    def LoadPage(self,arg0):
        val = htmlc.wxHtmlWindow_LoadPage(self.this,arg0)
        return val
    def GetOpenedPage(self):
        val = htmlc.wxHtmlWindow_GetOpenedPage(self.this)
        return val
    def SetRelatedFrame(self,arg0,arg1):
        val = htmlc.wxHtmlWindow_SetRelatedFrame(self.this,arg0.this,arg1)
        return val
    def GetRelatedFrame(self):
        val = htmlc.wxHtmlWindow_GetRelatedFrame(self.this)
        val = wxFramePtr(val)
        return val
    def SetRelatedStatusBar(self,arg0):
        val = htmlc.wxHtmlWindow_SetRelatedStatusBar(self.this,arg0)
        return val
    def SetFonts(self,arg0,arg1,arg2,arg3,arg4):
        val = htmlc.wxHtmlWindow_SetFonts(self.this,arg0,arg1,arg2,arg3,arg4)
        return val
    def SetTitle(self,arg0):
        val = htmlc.wxHtmlWindow_SetTitle(self.this,arg0)
        return val
    def SetBorders(self,arg0):
        val = htmlc.wxHtmlWindow_SetBorders(self.this,arg0)
        return val
    def HistoryBack(self):
        val = htmlc.wxHtmlWindow_HistoryBack(self.this)
        return val
    def HistoryForward(self):
        val = htmlc.wxHtmlWindow_HistoryForward(self.this)
        return val
    def HistoryClear(self):
        val = htmlc.wxHtmlWindow_HistoryClear(self.this)
        return val
    def __repr__(self):
        return "<C wxHtmlWindow instance>"
class wxHtmlWindow(wxHtmlWindowPtr):
    def __init__(self,arg0,*args) :
        argl = map(None,args)
        try: argl[1] = argl[1].this
        except: pass
        try: argl[2] = argl[2].this
        except: pass
        args = tuple(argl)
        self.this = apply(htmlc.new_wxHtmlWindow,(arg0.this,)+args)
        self.thisown = 1
        wx._StdWindowCallbacks(self)
        wx._StdOnScrollCallbacks(self)






#-------------- FUNCTION WRAPPERS ------------------



#-------------- VARIABLE WRAPPERS ------------------

