# This file was created automatically by SWIG.
import glcanvasc

from misc import *

from misc2 import *

from windows import *

from gdi import *

from clip_dnd import *

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

from sizers import *
import wx
class wxGLContextPtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __del__(self,glcanvasc=glcanvasc):
        if self.thisown == 1 :
            glcanvasc.delete_wxGLContext(self)
    def SetCurrent(self, *_args, **_kwargs):
        val = apply(glcanvasc.wxGLContext_SetCurrent,(self,) + _args, _kwargs)
        return val
    def SetColour(self, *_args, **_kwargs):
        val = apply(glcanvasc.wxGLContext_SetColour,(self,) + _args, _kwargs)
        return val
    def SwapBuffers(self, *_args, **_kwargs):
        val = apply(glcanvasc.wxGLContext_SwapBuffers,(self,) + _args, _kwargs)
        return val
    def GetWindow(self, *_args, **_kwargs):
        val = apply(glcanvasc.wxGLContext_GetWindow,(self,) + _args, _kwargs)
        if val: val = wxWindowPtr(val) 
        return val
    def __repr__(self):
        return "<C wxGLContext instance at %s>" % (self.this,)
class wxGLContext(wxGLContextPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(glcanvasc.new_wxGLContext,_args,_kwargs)
        self.thisown = 1




class wxGLCanvasPtr(wxScrolledWindowPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def SetCurrent(self, *_args, **_kwargs):
        val = apply(glcanvasc.wxGLCanvas_SetCurrent,(self,) + _args, _kwargs)
        return val
    def SetColour(self, *_args, **_kwargs):
        val = apply(glcanvasc.wxGLCanvas_SetColour,(self,) + _args, _kwargs)
        return val
    def SwapBuffers(self, *_args, **_kwargs):
        val = apply(glcanvasc.wxGLCanvas_SwapBuffers,(self,) + _args, _kwargs)
        return val
    def GetContext(self, *_args, **_kwargs):
        val = apply(glcanvasc.wxGLCanvas_GetContext,(self,) + _args, _kwargs)
        if val: val = wxGLContextPtr(val) 
        return val
    def __repr__(self):
        return "<C wxGLCanvas instance at %s>" % (self.this,)
class wxGLCanvas(wxGLCanvasPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(glcanvasc.new_wxGLCanvas,_args,_kwargs)
        self.thisown = 1
        #wx._StdWindowCallbacks(self)






#-------------- FUNCTION WRAPPERS ------------------



#-------------- VARIABLE WRAPPERS ------------------

