# This file was created automatically by SWIG.
import framesc

from misc import *

from gdi import *

from fonts import *

from windows import *

from clip_dnd import *

from stattool import *

from controls import *

from events import *
import wx
class wxTopLevelWindowPtr(wxWindowPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def Create(self, *_args, **_kwargs):
        val = framesc.wxTopLevelWindow_Create(self, *_args, **_kwargs)
        return val
    def Maximize(self, *_args, **_kwargs):
        val = framesc.wxTopLevelWindow_Maximize(self, *_args, **_kwargs)
        return val
    def Restore(self, *_args, **_kwargs):
        val = framesc.wxTopLevelWindow_Restore(self, *_args, **_kwargs)
        return val
    def Iconize(self, *_args, **_kwargs):
        val = framesc.wxTopLevelWindow_Iconize(self, *_args, **_kwargs)
        return val
    def IsMaximized(self, *_args, **_kwargs):
        val = framesc.wxTopLevelWindow_IsMaximized(self, *_args, **_kwargs)
        return val
    def IsIconized(self, *_args, **_kwargs):
        val = framesc.wxTopLevelWindow_IsIconized(self, *_args, **_kwargs)
        return val
    def GetIcon(self, *_args, **_kwargs):
        val = framesc.wxTopLevelWindow_GetIcon(self, *_args, **_kwargs)
        if val: val = wxIconPtr(val) ; val.thisown = 1
        return val
    def SetIcon(self, *_args, **_kwargs):
        val = framesc.wxTopLevelWindow_SetIcon(self, *_args, **_kwargs)
        return val
    def SetIcons(self, *_args, **_kwargs):
        val = framesc.wxTopLevelWindow_SetIcons(self, *_args, **_kwargs)
        return val
    def ShowFullScreen(self, *_args, **_kwargs):
        val = framesc.wxTopLevelWindow_ShowFullScreen(self, *_args, **_kwargs)
        return val
    def IsFullScreen(self, *_args, **_kwargs):
        val = framesc.wxTopLevelWindow_IsFullScreen(self, *_args, **_kwargs)
        return val
    def SetTitle(self, *_args, **_kwargs):
        val = framesc.wxTopLevelWindow_SetTitle(self, *_args, **_kwargs)
        return val
    def GetTitle(self, *_args, **_kwargs):
        val = framesc.wxTopLevelWindow_GetTitle(self, *_args, **_kwargs)
        return val
    def SetShape(self, *_args, **_kwargs):
        val = framesc.wxTopLevelWindow_SetShape(self, *_args, **_kwargs)
        return val
    def __repr__(self):
        return "<%s.%s instance; proxy of C++ wxTopLevelWindow instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this)
class wxTopLevelWindow(wxTopLevelWindowPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = framesc.new_wxTopLevelWindow(*_args,**_kwargs)
        self.thisown = 1
        self._setOORInfo(self)



def wxPreTopLevelWindow(*_args,**_kwargs):
    val = wxTopLevelWindowPtr(framesc.new_wxPreTopLevelWindow(*_args,**_kwargs))
    val.thisown = 1
    val._setOORInfo(val)
    return val


class wxFramePtr(wxTopLevelWindowPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def Create(self, *_args, **_kwargs):
        val = framesc.wxFrame_Create(self, *_args, **_kwargs)
        return val
    def GetClientAreaOrigin(self, *_args, **_kwargs):
        val = framesc.wxFrame_GetClientAreaOrigin(self, *_args, **_kwargs)
        if val: val = wxPointPtr(val) ; val.thisown = 1
        return val
    def SetMenuBar(self, *_args, **_kwargs):
        val = framesc.wxFrame_SetMenuBar(self, *_args, **_kwargs)
        return val
    def GetMenuBar(self, *_args, **_kwargs):
        val = framesc.wxFrame_GetMenuBar(self, *_args, **_kwargs)
        return val
    def ProcessCommand(self, *_args, **_kwargs):
        val = framesc.wxFrame_ProcessCommand(self, *_args, **_kwargs)
        return val
    def CreateStatusBar(self, *_args, **_kwargs):
        val = framesc.wxFrame_CreateStatusBar(self, *_args, **_kwargs)
        return val
    def GetStatusBar(self, *_args, **_kwargs):
        val = framesc.wxFrame_GetStatusBar(self, *_args, **_kwargs)
        return val
    def SetStatusBar(self, *_args, **_kwargs):
        val = framesc.wxFrame_SetStatusBar(self, *_args, **_kwargs)
        return val
    def SetStatusText(self, *_args, **_kwargs):
        val = framesc.wxFrame_SetStatusText(self, *_args, **_kwargs)
        return val
    def SetStatusWidths(self, *_args, **_kwargs):
        val = framesc.wxFrame_SetStatusWidths(self, *_args, **_kwargs)
        return val
    def PushStatusText(self, *_args, **_kwargs):
        val = framesc.wxFrame_PushStatusText(self, *_args, **_kwargs)
        return val
    def PopStatusText(self, *_args, **_kwargs):
        val = framesc.wxFrame_PopStatusText(self, *_args, **_kwargs)
        return val
    def DoGiveHelp(self, *_args, **_kwargs):
        val = framesc.wxFrame_DoGiveHelp(self, *_args, **_kwargs)
        return val
    def SetStatusBarPane(self, *_args, **_kwargs):
        val = framesc.wxFrame_SetStatusBarPane(self, *_args, **_kwargs)
        return val
    def GetStatusBarPane(self, *_args, **_kwargs):
        val = framesc.wxFrame_GetStatusBarPane(self, *_args, **_kwargs)
        return val
    def CreateToolBar(self, *_args, **_kwargs):
        val = framesc.wxFrame_CreateToolBar(self, *_args, **_kwargs)
        return val
    def GetToolBar(self, *_args, **_kwargs):
        val = framesc.wxFrame_GetToolBar(self, *_args, **_kwargs)
        return val
    def SetToolBar(self, *_args, **_kwargs):
        val = framesc.wxFrame_SetToolBar(self, *_args, **_kwargs)
        return val
    def SendSizeEvent(self, *_args, **_kwargs):
        val = framesc.wxFrame_SendSizeEvent(self, *_args, **_kwargs)
        return val
    def __repr__(self):
        return "<%s.%s instance; proxy of C++ wxFrame instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this)
    Command = ProcessCommand
class wxFrame(wxFramePtr):
    def __init__(self,*_args,**_kwargs):
        self.this = framesc.new_wxFrame(*_args,**_kwargs)
        self.thisown = 1
        self._setOORInfo(self)



def wxPreFrame(*_args,**_kwargs):
    val = wxFramePtr(framesc.new_wxPreFrame(*_args,**_kwargs))
    val.thisown = 1
    val._setOORInfo(val)
    return val


class wxDialogPtr(wxTopLevelWindowPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def Create(self, *_args, **_kwargs):
        val = framesc.wxDialog_Create(self, *_args, **_kwargs)
        return val
    def Centre(self, *_args, **_kwargs):
        val = framesc.wxDialog_Centre(self, *_args, **_kwargs)
        return val
    def EndModal(self, *_args, **_kwargs):
        val = framesc.wxDialog_EndModal(self, *_args, **_kwargs)
        return val
    def SetModal(self, *_args, **_kwargs):
        val = framesc.wxDialog_SetModal(self, *_args, **_kwargs)
        return val
    def IsModal(self, *_args, **_kwargs):
        val = framesc.wxDialog_IsModal(self, *_args, **_kwargs)
        return val
    def ShowModal(self, *_args, **_kwargs):
        val = framesc.wxDialog_ShowModal(self, *_args, **_kwargs)
        return val
    def GetReturnCode(self, *_args, **_kwargs):
        val = framesc.wxDialog_GetReturnCode(self, *_args, **_kwargs)
        return val
    def SetReturnCode(self, *_args, **_kwargs):
        val = framesc.wxDialog_SetReturnCode(self, *_args, **_kwargs)
        return val
    def CreateTextSizer(self, *_args, **_kwargs):
        val = framesc.wxDialog_CreateTextSizer(self, *_args, **_kwargs)
        return val
    def CreateButtonSizer(self, *_args, **_kwargs):
        val = framesc.wxDialog_CreateButtonSizer(self, *_args, **_kwargs)
        return val
    def __repr__(self):
        return "<%s.%s instance; proxy of C++ wxDialog instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this)
class wxDialog(wxDialogPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = framesc.new_wxDialog(*_args,**_kwargs)
        self.thisown = 1
        self._setOORInfo(self)



def wxPreDialog(*_args,**_kwargs):
    val = wxDialogPtr(framesc.new_wxPreDialog(*_args,**_kwargs))
    val.thisown = 1
    val._setOORInfo(val)
    return val


class wxMiniFramePtr(wxFramePtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def Create(self, *_args, **_kwargs):
        val = framesc.wxMiniFrame_Create(self, *_args, **_kwargs)
        return val
    def __repr__(self):
        return "<%s.%s instance; proxy of C++ wxMiniFrame instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this)
class wxMiniFrame(wxMiniFramePtr):
    def __init__(self,*_args,**_kwargs):
        self.this = framesc.new_wxMiniFrame(*_args,**_kwargs)
        self.thisown = 1
        self._setOORInfo(self)



def wxPreMiniFrame(*_args,**_kwargs):
    val = wxMiniFramePtr(framesc.new_wxPreMiniFrame(*_args,**_kwargs))
    val.thisown = 1
    val._setOORInfo(val)
    return val


class wxSplashScreenWindowPtr(wxWindowPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def SetBitmap(self, *_args, **_kwargs):
        val = framesc.wxSplashScreenWindow_SetBitmap(self, *_args, **_kwargs)
        return val
    def GetBitmap(self, *_args, **_kwargs):
        val = framesc.wxSplashScreenWindow_GetBitmap(self, *_args, **_kwargs)
        if val: val = wxBitmapPtr(val) 
        return val
    def __repr__(self):
        return "<%s.%s instance; proxy of C++ wxSplashScreenWindow instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this)
class wxSplashScreenWindow(wxSplashScreenWindowPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = framesc.new_wxSplashScreenWindow(*_args,**_kwargs)
        self.thisown = 1
        self._setOORInfo(self)




class wxSplashScreenPtr(wxFramePtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def GetSplashStyle(self, *_args, **_kwargs):
        val = framesc.wxSplashScreen_GetSplashStyle(self, *_args, **_kwargs)
        return val
    def GetSplashWindow(self, *_args, **_kwargs):
        val = framesc.wxSplashScreen_GetSplashWindow(self, *_args, **_kwargs)
        if val: val = wxSplashScreenWindowPtr(val) 
        return val
    def GetTimeout(self, *_args, **_kwargs):
        val = framesc.wxSplashScreen_GetTimeout(self, *_args, **_kwargs)
        return val
    def __repr__(self):
        return "<%s.%s instance; proxy of C++ wxSplashScreen instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this)
class wxSplashScreen(wxSplashScreenPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = framesc.new_wxSplashScreen(*_args,**_kwargs)
        self.thisown = 1
        self._setOORInfo(self)






#-------------- FUNCTION WRAPPERS ------------------



#-------------- VARIABLE WRAPPERS ------------------

wxFULLSCREEN_NOMENUBAR = framesc.wxFULLSCREEN_NOMENUBAR
wxFULLSCREEN_NOTOOLBAR = framesc.wxFULLSCREEN_NOTOOLBAR
wxFULLSCREEN_NOSTATUSBAR = framesc.wxFULLSCREEN_NOSTATUSBAR
wxFULLSCREEN_NOBORDER = framesc.wxFULLSCREEN_NOBORDER
wxFULLSCREEN_NOCAPTION = framesc.wxFULLSCREEN_NOCAPTION
wxFULLSCREEN_ALL = framesc.wxFULLSCREEN_ALL
wxTOPLEVEL_EX_DIALOG = framesc.wxTOPLEVEL_EX_DIALOG
wxSPLASH_CENTRE_ON_PARENT = framesc.wxSPLASH_CENTRE_ON_PARENT
wxSPLASH_CENTRE_ON_SCREEN = framesc.wxSPLASH_CENTRE_ON_SCREEN
wxSPLASH_NO_CENTRE = framesc.wxSPLASH_NO_CENTRE
wxSPLASH_TIMEOUT = framesc.wxSPLASH_TIMEOUT
wxSPLASH_NO_TIMEOUT = framesc.wxSPLASH_NO_TIMEOUT
