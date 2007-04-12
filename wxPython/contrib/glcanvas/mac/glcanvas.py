# This file was created automatically by SWIG 1.3.29.
# Don't modify this file, modify the SWIG interface instead.

"""
`GLCanvas` provides an OpenGL Context on a `wx.Window`.
"""

import _glcanvas
import new
new_instancemethod = new.instancemethod
def _swig_setattr_nondynamic(self,class_type,name,value,static=1):
    if (name == "thisown"): return self.this.own(value)
    if (name == "this"):
        if type(value).__name__ == 'PySwigObject':
            self.__dict__[name] = value
            return
    method = class_type.__swig_setmethods__.get(name,None)
    if method: return method(self,value)
    if (not static) or hasattr(self,name):
        self.__dict__[name] = value
    else:
        raise AttributeError("You cannot add attributes to %s" % self)

def _swig_setattr(self,class_type,name,value):
    return _swig_setattr_nondynamic(self,class_type,name,value,0)

def _swig_getattr(self,class_type,name):
    if (name == "thisown"): return self.this.own()
    method = class_type.__swig_getmethods__.get(name,None)
    if method: return method(self)
    raise AttributeError,name

def _swig_repr(self):
    try: strthis = "proxy of " + self.this.__repr__()
    except: strthis = ""
    return "<%s.%s; %s >" % (self.__class__.__module__, self.__class__.__name__, strthis,)

import types
try:
    _object = types.ObjectType
    _newclass = 1
except AttributeError:
    class _object : pass
    _newclass = 0
del types


def _swig_setattr_nondynamic_method(set):
    def set_attr(self,name,value):
        if (name == "thisown"): return self.this.own(value)
        if hasattr(self,name) or (name == "this"):
            set(self,name,value)
        else:
            raise AttributeError("You cannot add attributes to %s" % self)
    return set_attr


import _core
wx = _core 
__docfilter__ = wx.__DocFilter(globals()) 
class GLContext(_core.Object):
    """Proxy of C++ GLContext class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """__init__(self, GLCanvas win, GLContext other=None) -> GLContext"""
        _glcanvas.GLContext_swiginit(self,_glcanvas.new_GLContext(*args, **kwargs))
    __swig_destroy__ = _glcanvas.delete_GLContext
    __del__ = lambda self : None;
_glcanvas.GLContext_swigregister(GLContext)
cvar = _glcanvas.cvar
GLCanvasNameStr = cvar.GLCanvasNameStr

WX_GL_RGBA = _glcanvas.WX_GL_RGBA
WX_GL_BUFFER_SIZE = _glcanvas.WX_GL_BUFFER_SIZE
WX_GL_LEVEL = _glcanvas.WX_GL_LEVEL
WX_GL_DOUBLEBUFFER = _glcanvas.WX_GL_DOUBLEBUFFER
WX_GL_STEREO = _glcanvas.WX_GL_STEREO
WX_GL_AUX_BUFFERS = _glcanvas.WX_GL_AUX_BUFFERS
WX_GL_MIN_RED = _glcanvas.WX_GL_MIN_RED
WX_GL_MIN_GREEN = _glcanvas.WX_GL_MIN_GREEN
WX_GL_MIN_BLUE = _glcanvas.WX_GL_MIN_BLUE
WX_GL_MIN_ALPHA = _glcanvas.WX_GL_MIN_ALPHA
WX_GL_DEPTH_SIZE = _glcanvas.WX_GL_DEPTH_SIZE
WX_GL_STENCIL_SIZE = _glcanvas.WX_GL_STENCIL_SIZE
WX_GL_MIN_ACCUM_RED = _glcanvas.WX_GL_MIN_ACCUM_RED
WX_GL_MIN_ACCUM_GREEN = _glcanvas.WX_GL_MIN_ACCUM_GREEN
WX_GL_MIN_ACCUM_BLUE = _glcanvas.WX_GL_MIN_ACCUM_BLUE
WX_GL_MIN_ACCUM_ALPHA = _glcanvas.WX_GL_MIN_ACCUM_ALPHA
class GLCanvas(_core.Window):
    """Proxy of C++ GLCanvas class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, Window parent, int id=-1, int attribList=None, Point pos=DefaultPosition, 
            Size size=DefaultSize, 
            long style=0, String name=GLCanvasNameStr, Palette palette=wxNullPalette) -> GLCanvas
        """
        _glcanvas.GLCanvas_swiginit(self,_glcanvas.new_GLCanvas(*args, **kwargs))
        self._setOORInfo(self)

    def Create(*args, **kwargs):
        """
        Create(self, Window parent, int id=ID_ANY, Point pos=DefaultPosition, 
            Size size=DefaultSize, long style=0, String name=wxGLCanvasName, 
            int attribList=None, 
            Palette palette=wxNullPalette) -> bool
        """
        return _glcanvas.GLCanvas_Create(*args, **kwargs)

    def SetColour(*args, **kwargs):
        """SetColour(self, String colour)"""
        return _glcanvas.GLCanvas_SetColour(*args, **kwargs)

    def SwapBuffers(*args, **kwargs):
        """SwapBuffers(self)"""
        return _glcanvas.GLCanvas_SwapBuffers(*args, **kwargs)

    def GetContext(*args, **kwargs):
        """GetContext(self) -> GLContext"""
        return _glcanvas.GLCanvas_GetContext(*args, **kwargs)

    def SetCurrent(*args):
        """
        SetCurrent(self, GLContext context)
        SetCurrent(self)
        """
        return _glcanvas.GLCanvas_SetCurrent(*args)

    Context = property(GetContext,doc="See `GetContext`") 
_glcanvas.GLCanvas_swigregister(GLCanvas)

def GLCanvasWithContext(*args, **kwargs):
    """
    GLCanvasWithContext(Window parent, GLContext shared=None, int id=-1, Point pos=DefaultPosition, 
        Size size=DefaultSize, 
        long style=0, String name=GLCanvasNameStr, 
        int attribList=None, Palette palette=wxNullPalette) -> GLCanvas
    """
    val = _glcanvas.new_GLCanvasWithContext(*args, **kwargs)
    val._setOORInfo(val)
    return val



