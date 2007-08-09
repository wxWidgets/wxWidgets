# This file was created automatically by SWIG.
import dllwidget_c

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
class wxDllWidgetPtr(wxPanelPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def Ok(self, *_args, **_kwargs):
        val = dllwidget_c.wxDllWidget_Ok(self, *_args, **_kwargs)
        return val
    def SendCommand(self, *_args, **_kwargs):
        val = dllwidget_c.wxDllWidget_SendCommand(self, *_args, **_kwargs)
        return val
    def GetWidgetWindow(self, *_args, **_kwargs):
        val = dllwidget_c.wxDllWidget_GetWidgetWindow(self, *_args, **_kwargs)
        return val
    def __repr__(self):
        return "<%s.%s instance; proxy of C++ wxDllWidget instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this)
class wxDllWidget(wxDllWidgetPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = dllwidget_c.new_wxDllWidget(*_args,**_kwargs)
        self.thisown = 1
        self._setOORInfo(self)






#-------------- FUNCTION WRAPPERS ------------------

wxDllWidget_GetDllExt = dllwidget_c.wxDllWidget_GetDllExt



#-------------- VARIABLE WRAPPERS ------------------

