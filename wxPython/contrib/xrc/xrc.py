# This file was created automatically by SWIG.
import xrcc

from misc import *

from misc2 import *

from windows import *

from gdi import *

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

def XMLID(str_id):
    return wxXmlResource_GetXMLID(str_id)

def XMLCTRL(window, str_id, *args):
    return window.FindWindowById(XMLID(str_id))


class wxXmlResourcePtr(wxObjectPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __del__(self,xrcc=xrcc):
        if self.thisown == 1 :
            xrcc.delete_wxXmlResource(self)
    def Load(self, *_args, **_kwargs):
        val = apply(xrcc.wxXmlResource_Load,(self,) + _args, _kwargs)
        return val
    def InitAllHandlers(self, *_args, **_kwargs):
        val = apply(xrcc.wxXmlResource_InitAllHandlers,(self,) + _args, _kwargs)
        return val
    def AddHandler(self, *_args, **_kwargs):
        val = apply(xrcc.wxXmlResource_AddHandler,(self,) + _args, _kwargs)
        return val
    def ClearHandlers(self, *_args, **_kwargs):
        val = apply(xrcc.wxXmlResource_ClearHandlers,(self,) + _args, _kwargs)
        return val
    def LoadMenu(self, *_args, **_kwargs):
        val = apply(xrcc.wxXmlResource_LoadMenu,(self,) + _args, _kwargs)
        return val
    def LoadMenuBar(self, *_args, **_kwargs):
        val = apply(xrcc.wxXmlResource_LoadMenuBar,(self,) + _args, _kwargs)
        return val
    def LoadToolBar(self, *_args, **_kwargs):
        val = apply(xrcc.wxXmlResource_LoadToolBar,(self,) + _args, _kwargs)
        return val
    def LoadDialog(self, *_args, **_kwargs):
        val = apply(xrcc.wxXmlResource_LoadDialog,(self,) + _args, _kwargs)
        if val: val = wxDialogPtr(val) 
        return val
    def LoadOnDialog(self, *_args, **_kwargs):
        val = apply(xrcc.wxXmlResource_LoadOnDialog,(self,) + _args, _kwargs)
        return val
    def LoadPanel(self, *_args, **_kwargs):
        val = apply(xrcc.wxXmlResource_LoadPanel,(self,) + _args, _kwargs)
        if val: val = wxPanelPtr(val) 
        return val
    def LoadOnPanel(self, *_args, **_kwargs):
        val = apply(xrcc.wxXmlResource_LoadOnPanel,(self,) + _args, _kwargs)
        return val
    def LoadFrame(self, *_args, **_kwargs):
        val = apply(xrcc.wxXmlResource_LoadFrame,(self,) + _args, _kwargs)
        return val
    def LoadBitmap(self, *_args, **_kwargs):
        val = apply(xrcc.wxXmlResource_LoadBitmap,(self,) + _args, _kwargs)
        if val: val = wxBitmapPtr(val) ; val.thisown = 1
        return val
    def LoadIcon(self, *_args, **_kwargs):
        val = apply(xrcc.wxXmlResource_LoadIcon,(self,) + _args, _kwargs)
        if val: val = wxIconPtr(val) ; val.thisown = 1
        return val
    def AttachUnknownControl(self, *_args, **_kwargs):
        val = apply(xrcc.wxXmlResource_AttachUnknownControl,(self,) + _args, _kwargs)
        return val
    def GetVersion(self, *_args, **_kwargs):
        val = apply(xrcc.wxXmlResource_GetVersion,(self,) + _args, _kwargs)
        return val
    def CompareVersion(self, *_args, **_kwargs):
        val = apply(xrcc.wxXmlResource_CompareVersion,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxXmlResource instance at %s>" % (self.this,)
class wxXmlResource(wxXmlResourcePtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(xrcc.new_wxXmlResourceEmpty,_args,_kwargs)
        self.thisown = 1



def wxXmlResource(*_args,**_kwargs):
    val = wxXmlResourcePtr(apply(xrcc.new_wxXmlResource,_args,_kwargs))
    val.thisown = 1
    return val




#-------------- FUNCTION WRAPPERS ------------------

wxXmlResource_GetXMLID = xrcc.wxXmlResource_GetXMLID



#-------------- VARIABLE WRAPPERS ------------------

wxXRC_USE_LOCALE = xrcc.wxXRC_USE_LOCALE
wxXRC_NO_SUBCLASSING = xrcc.wxXRC_NO_SUBCLASSING
cvar = xrcc.cvar
wxTheXmlResource = wxXmlResourcePtr(xrcc.cvar.wxTheXmlResource)
