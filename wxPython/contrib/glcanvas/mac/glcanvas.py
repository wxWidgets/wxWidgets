# This file was created automatically by SWIG.
import glcanvasc

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
class wxGLContextPtr(wxObjectPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __del__(self, delfunc=glcanvasc.delete_wxGLContext):
        if self.thisown == 1:
            try:
                delfunc(self)
            except:
                pass
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
        return val
    def __repr__(self):
        return "<C wxGLContext instance at %s>" % (self.this,)
class wxGLContext(wxGLContextPtr):
    def __init__(self,this):
        self.this = this




class wxGLCanvasPtr(wxWindowPtr):
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
        self._setOORInfo(self)



def wxGLCanvasWithContext(*_args,**_kwargs):
    val = wxGLCanvasPtr(apply(glcanvasc.new_wxGLCanvasWithContext,_args,_kwargs))
    val.thisown = 1
    val._setOORInfo(self)
    return val




#-------------- FUNCTION WRAPPERS ------------------



#-------------- VARIABLE WRAPPERS ------------------

WX_GL_RGBA = glcanvasc.WX_GL_RGBA
WX_GL_BUFFER_SIZE = glcanvasc.WX_GL_BUFFER_SIZE
WX_GL_LEVEL = glcanvasc.WX_GL_LEVEL
WX_GL_DOUBLEBUFFER = glcanvasc.WX_GL_DOUBLEBUFFER
WX_GL_STEREO = glcanvasc.WX_GL_STEREO
WX_GL_AUX_BUFFERS = glcanvasc.WX_GL_AUX_BUFFERS
WX_GL_MIN_RED = glcanvasc.WX_GL_MIN_RED
WX_GL_MIN_GREEN = glcanvasc.WX_GL_MIN_GREEN
WX_GL_MIN_BLUE = glcanvasc.WX_GL_MIN_BLUE
WX_GL_MIN_ALPHA = glcanvasc.WX_GL_MIN_ALPHA
WX_GL_DEPTH_SIZE = glcanvasc.WX_GL_DEPTH_SIZE
WX_GL_STENCIL_SIZE = glcanvasc.WX_GL_STENCIL_SIZE
WX_GL_MIN_ACCUM_RED = glcanvasc.WX_GL_MIN_ACCUM_RED
WX_GL_MIN_ACCUM_GREEN = glcanvasc.WX_GL_MIN_ACCUM_GREEN
WX_GL_MIN_ACCUM_BLUE = glcanvasc.WX_GL_MIN_ACCUM_BLUE
WX_GL_MIN_ACCUM_ALPHA = glcanvasc.WX_GL_MIN_ACCUM_ALPHA
