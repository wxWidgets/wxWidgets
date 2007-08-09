# This file was created automatically by SWIG 1.3.29.
# Don't modify this file, modify the SWIG interface instead.

"""
Simple animation player classes, including `GIFAnimationCtrl` for displaying
animated GIF files

"""

import _animate
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
import wx 
__docfilter__ = wx._core.__DocFilter(globals()) 
ANIM_UNSPECIFIED = _animate.ANIM_UNSPECIFIED
ANIM_DONOTREMOVE = _animate.ANIM_DONOTREMOVE
ANIM_TOBACKGROUND = _animate.ANIM_TOBACKGROUND
ANIM_TOPREVIOUS = _animate.ANIM_TOPREVIOUS
ANIMATION_TYPE_INVALID = _animate.ANIMATION_TYPE_INVALID
ANIMATION_TYPE_GIF = _animate.ANIMATION_TYPE_GIF
ANIMATION_TYPE_ANI = _animate.ANIMATION_TYPE_ANI
ANIMATION_TYPE_ANY = _animate.ANIMATION_TYPE_ANY
class AnimationBase(_core.Object):
    """Proxy of C++ AnimationBase class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    __swig_destroy__ = _animate.delete_AnimationBase
    __del__ = lambda self : None;
    def IsOk(*args, **kwargs):
        """IsOk(self) -> bool"""
        return _animate.AnimationBase_IsOk(*args, **kwargs)

    def GetDelay(*args, **kwargs):
        """GetDelay(self, int i) -> int"""
        return _animate.AnimationBase_GetDelay(*args, **kwargs)

    def GetFrameCount(*args, **kwargs):
        """GetFrameCount(self) -> int"""
        return _animate.AnimationBase_GetFrameCount(*args, **kwargs)

    def GetFrame(*args, **kwargs):
        """GetFrame(self, int i) -> Image"""
        return _animate.AnimationBase_GetFrame(*args, **kwargs)

    def GetSize(*args, **kwargs):
        """GetSize(self) -> Size"""
        return _animate.AnimationBase_GetSize(*args, **kwargs)

    def LoadFile(*args, **kwargs):
        """LoadFile(self, String name, int type=ANIMATION_TYPE_ANY) -> bool"""
        return _animate.AnimationBase_LoadFile(*args, **kwargs)

    def Load(*args, **kwargs):
        """Load(self, InputStream stream, int type=ANIMATION_TYPE_ANY) -> bool"""
        return _animate.AnimationBase_Load(*args, **kwargs)

_animate.AnimationBase_swigregister(AnimationBase)
cvar = _animate.cvar
AnimationCtrlNameStr = cvar.AnimationCtrlNameStr

class Animation(AnimationBase):
    """Proxy of C++ Animation class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args): 
        """
        __init__(self) -> Animation
        __init__(self, String name, int type=ANIMATION_TYPE_ANY) -> Animation
        """
        _animate.Animation_swiginit(self,_animate.new_Animation(*args))
    __swig_destroy__ = _animate.delete_Animation
    __del__ = lambda self : None;
    def GetFramePosition(*args, **kwargs):
        """GetFramePosition(self, int frame) -> Point"""
        return _animate.Animation_GetFramePosition(*args, **kwargs)

    def GetFrameSize(*args, **kwargs):
        """GetFrameSize(self, int frame) -> Size"""
        return _animate.Animation_GetFrameSize(*args, **kwargs)

    def GetDisposalMethod(*args, **kwargs):
        """GetDisposalMethod(self, int frame) -> int"""
        return _animate.Animation_GetDisposalMethod(*args, **kwargs)

    def GetTransparentColour(*args, **kwargs):
        """GetTransparentColour(self, int frame) -> Colour"""
        return _animate.Animation_GetTransparentColour(*args, **kwargs)

    def GetBackgroundColour(*args, **kwargs):
        """GetBackgroundColour(self) -> Colour"""
        return _animate.Animation_GetBackgroundColour(*args, **kwargs)

_animate.Animation_swigregister(Animation)

AC_NO_AUTORESIZE = _animate.AC_NO_AUTORESIZE
AC_DEFAULT_STYLE = _animate.AC_DEFAULT_STYLE
AN_FIT_ANIMATION = _animate.AN_FIT_ANIMATION
class AnimationCtrlBase(_core.Control):
    """Proxy of C++ AnimationCtrlBase class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def LoadFile(*args, **kwargs):
        """LoadFile(self, String filename, int type=ANIMATION_TYPE_ANY) -> bool"""
        return _animate.AnimationCtrlBase_LoadFile(*args, **kwargs)

    def SetAnimation(*args, **kwargs):
        """SetAnimation(self, Animation anim)"""
        return _animate.AnimationCtrlBase_SetAnimation(*args, **kwargs)

    def GetAnimation(*args, **kwargs):
        """GetAnimation(self) -> Animation"""
        return _animate.AnimationCtrlBase_GetAnimation(*args, **kwargs)

    Animation = property(GetAnimation,SetAnimation) 
    def Play(*args, **kwargs):
        """Play(self) -> bool"""
        return _animate.AnimationCtrlBase_Play(*args, **kwargs)

    def Stop(*args, **kwargs):
        """Stop(self)"""
        return _animate.AnimationCtrlBase_Stop(*args, **kwargs)

    def IsPlaying(*args, **kwargs):
        """IsPlaying(self) -> bool"""
        return _animate.AnimationCtrlBase_IsPlaying(*args, **kwargs)

    def SetInactiveBitmap(*args, **kwargs):
        """SetInactiveBitmap(self, Bitmap bmp)"""
        return _animate.AnimationCtrlBase_SetInactiveBitmap(*args, **kwargs)

    def GetInactiveBitmap(*args, **kwargs):
        """GetInactiveBitmap(self) -> Bitmap"""
        return _animate.AnimationCtrlBase_GetInactiveBitmap(*args, **kwargs)

    InactiveBitmap = property(GetInactiveBitmap,SetInactiveBitmap) 
_animate.AnimationCtrlBase_swigregister(AnimationCtrlBase)
NullAnimation = cvar.NullAnimation

class AnimationCtrl(AnimationCtrlBase):
    """Proxy of C++ AnimationCtrl class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, Window parent, int id=-1, Animation anim=NullAnimation, 
            Point pos=DefaultPosition, Size size=DefaultSize, 
            long style=AC_DEFAULT_STYLE, String name=AnimationCtrlNameStr) -> AnimationCtrl
        """
        _animate.AnimationCtrl_swiginit(self,_animate.new_AnimationCtrl(*args, **kwargs))
        self._setOORInfo(self)

    def Create(*args, **kwargs):
        """
        Create(self, Window parent, int id, Animation anim=NullAnimation, 
            Point pos=DefaultPosition, Size size=DefaultSize, 
            long style=AC_DEFAULT_STYLE, String name=AnimationCtrlNameStr) -> bool
        """
        return _animate.AnimationCtrl_Create(*args, **kwargs)

    def SetUseWindowBackgroundColour(*args, **kwargs):
        """SetUseWindowBackgroundColour(self, bool useWinBackground=True)"""
        return _animate.AnimationCtrl_SetUseWindowBackgroundColour(*args, **kwargs)

    def IsUsingWindowBackgroundColour(*args, **kwargs):
        """IsUsingWindowBackgroundColour(self) -> bool"""
        return _animate.AnimationCtrl_IsUsingWindowBackgroundColour(*args, **kwargs)

    def DrawCurrentFrame(*args, **kwargs):
        """DrawCurrentFrame(self, DC dc)"""
        return _animate.AnimationCtrl_DrawCurrentFrame(*args, **kwargs)

    def GetBackingStore(*args, **kwargs):
        """GetBackingStore(self) -> Bitmap"""
        return _animate.AnimationCtrl_GetBackingStore(*args, **kwargs)

_animate.AnimationCtrl_swigregister(AnimationCtrl)

def PreAnimationCtrl(*args, **kwargs):
    """PreAnimationCtrl() -> AnimationCtrl"""
    val = _animate.new_PreAnimationCtrl(*args, **kwargs)
    return val

class GIFAnimationCtrl(AnimationCtrl):
    """
    Backwards compatibility class for AnimationCtrl.
    """
    def __init__(self, parent, id=-1, filename="",
                 pos=wx.DefaultPosition, size=wx.DefaultSize,
                 style=AC_DEFAULT_STYLE,
                 name="gifAnimation"):
        AnimationCtrl.__init__(self, parent, id, NullAnimation, pos, size, style, name)
        self.LoadFile(filename)

    def GetPlayer(self):
        return self

    def UseBackgroundColour(self, useBackground=True):
        self.SetUseWindowBackgroundColour(useBackground)



