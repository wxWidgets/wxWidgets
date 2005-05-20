# This file was created automatically by SWIG.
import iewinc

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
import wx

def EVT_MSHTML_BEFORENAVIGATE2(win, id, func):
    win.Connect(id, -1, wxEVT_COMMAND_MSHTML_BEFORENAVIGATE2, func)

def EVT_MSHTML_NEWWINDOW2(win, id, func):
    win.Connect(id, -1, wxEVT_COMMAND_MSHTML_NEWWINDOW2, func)

def EVT_MSHTML_DOCUMENTCOMPLETE(win, id, func):
    win.Connect(id, -1, wxEVT_COMMAND_MSHTML_DOCUMENTCOMPLETE, func)

def EVT_MSHTML_PROGRESSCHANGE(win, id, func):
    win.Connect(id, -1, wxEVT_COMMAND_MSHTML_PROGRESSCHANGE, func)

def EVT_MSHTML_STATUSTEXTCHANGE(win, id, func):
    win.Connect(id, -1, wxEVT_COMMAND_MSHTML_STATUSTEXTCHANGE, func)

def EVT_MSHTML_TITLECHANGE(win, id, func):
    win.Connect(id, -1, wxEVT_COMMAND_MSHTML_TITLECHANGE, func)

class wxMSHTMLEventPtr(wxNotifyEventPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def GetText1(self, *_args, **_kwargs):
        val = iewinc.wxMSHTMLEvent_GetText1(self, *_args, **_kwargs)
        return val
    def GetLong1(self, *_args, **_kwargs):
        val = iewinc.wxMSHTMLEvent_GetLong1(self, *_args, **_kwargs)
        return val
    def GetLong2(self, *_args, **_kwargs):
        val = iewinc.wxMSHTMLEvent_GetLong2(self, *_args, **_kwargs)
        return val
    def __repr__(self):
        return "<%s.%s instance; proxy of C++ wxMSHTMLEvent instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this)
class wxMSHTMLEvent(wxMSHTMLEventPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = iewinc.new_wxMSHTMLEvent(*_args,**_kwargs)
        self.thisown = 1




class wxIEHtmlWinPtr(wxWindowPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def LoadUrl(self, *_args, **_kwargs):
        val = iewinc.wxIEHtmlWin_LoadUrl(self, *_args, **_kwargs)
        return val
    def LoadString(self, *_args, **_kwargs):
        val = iewinc.wxIEHtmlWin_LoadString(self, *_args, **_kwargs)
        return val
    def LoadStream(self, *_args, **_kwargs):
        val = iewinc.wxIEHtmlWin_LoadStream(self, *_args, **_kwargs)
        return val
    def SetCharset(self, *_args, **_kwargs):
        val = iewinc.wxIEHtmlWin_SetCharset(self, *_args, **_kwargs)
        return val
    def SetEditMode(self, *_args, **_kwargs):
        val = iewinc.wxIEHtmlWin_SetEditMode(self, *_args, **_kwargs)
        return val
    def GetEditMode(self, *_args, **_kwargs):
        val = iewinc.wxIEHtmlWin_GetEditMode(self, *_args, **_kwargs)
        return val
    def GetStringSelection(self, *_args, **_kwargs):
        val = iewinc.wxIEHtmlWin_GetStringSelection(self, *_args, **_kwargs)
        return val
    def GetText(self, *_args, **_kwargs):
        val = iewinc.wxIEHtmlWin_GetText(self, *_args, **_kwargs)
        return val
    def GoBack(self, *_args, **_kwargs):
        val = iewinc.wxIEHtmlWin_GoBack(self, *_args, **_kwargs)
        return val
    def GoForward(self, *_args, **_kwargs):
        val = iewinc.wxIEHtmlWin_GoForward(self, *_args, **_kwargs)
        return val
    def GoHome(self, *_args, **_kwargs):
        val = iewinc.wxIEHtmlWin_GoHome(self, *_args, **_kwargs)
        return val
    def GoSearch(self, *_args, **_kwargs):
        val = iewinc.wxIEHtmlWin_GoSearch(self, *_args, **_kwargs)
        return val
    def RefreshPage(self, *_args, **_kwargs):
        val = iewinc.wxIEHtmlWin_RefreshPage(self, *_args, **_kwargs)
        return val
    def Stop(self, *_args, **_kwargs):
        val = iewinc.wxIEHtmlWin_Stop(self, *_args, **_kwargs)
        return val
    def __repr__(self):
        return "<%s.%s instance; proxy of C++ wxIEHtmlWin instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this)
    Navigate = LoadUrl
class wxIEHtmlWin(wxIEHtmlWinPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = iewinc.new_wxIEHtmlWin(*_args,**_kwargs)
        self.thisown = 1






#-------------- FUNCTION WRAPPERS ------------------



#-------------- VARIABLE WRAPPERS ------------------

wxEVT_COMMAND_MSHTML_BEFORENAVIGATE2 = iewinc.wxEVT_COMMAND_MSHTML_BEFORENAVIGATE2
wxEVT_COMMAND_MSHTML_NEWWINDOW2 = iewinc.wxEVT_COMMAND_MSHTML_NEWWINDOW2
wxEVT_COMMAND_MSHTML_DOCUMENTCOMPLETE = iewinc.wxEVT_COMMAND_MSHTML_DOCUMENTCOMPLETE
wxEVT_COMMAND_MSHTML_PROGRESSCHANGE = iewinc.wxEVT_COMMAND_MSHTML_PROGRESSCHANGE
wxEVT_COMMAND_MSHTML_STATUSTEXTCHANGE = iewinc.wxEVT_COMMAND_MSHTML_STATUSTEXTCHANGE
wxEVT_COMMAND_MSHTML_TITLECHANGE = iewinc.wxEVT_COMMAND_MSHTML_TITLECHANGE
wxIEHTML_REFRESH_NORMAL = iewinc.wxIEHTML_REFRESH_NORMAL
wxIEHTML_REFRESH_IFEXPIRED = iewinc.wxIEHTML_REFRESH_IFEXPIRED
wxIEHTML_REFRESH_CONTINUE = iewinc.wxIEHTML_REFRESH_CONTINUE
wxIEHTML_REFRESH_COMPLETELY = iewinc.wxIEHTML_REFRESH_COMPLETELY


#-------------- USER INCLUDE -----------------------

# Stuff these names into the wx namespace so wxPyConstructObject can find them

wx.wxMSHTMLEventPtr  = wxMSHTMLEventPtr
wx.wxIEHtmlWinPtr    = wxIEHtmlWinPtr
