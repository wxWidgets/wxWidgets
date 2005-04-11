# This file was created automatically by SWIG.
# Don't modify this file, modify the SWIG interface instead.

"""
Simple animation player classes, including `GIFAnimationCtrl` for displaying
animated GIF files

"""

import _animate

def _swig_setattr_nondynamic(self,class_type,name,value,static=1):
    if (name == "this"):
        if isinstance(value, class_type):
            self.__dict__[name] = value.this
            if hasattr(value,"thisown"): self.__dict__["thisown"] = value.thisown
            del value.thisown
            return
    method = class_type.__swig_setmethods__.get(name,None)
    if method: return method(self,value)
    if (not static) or hasattr(self,name) or (name == "thisown"):
        self.__dict__[name] = value
    else:
        raise AttributeError("You cannot add attributes to %s" % self)

def _swig_setattr(self,class_type,name,value):
    return _swig_setattr_nondynamic(self,class_type,name,value,0)

def _swig_getattr(self,class_type,name):
    method = class_type.__swig_getmethods__.get(name,None)
    if method: return method(self)
    raise AttributeError,name

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
        if hasattr(self,name) or (name in ("this", "thisown")):
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
class AnimationPlayer(_core.Object):
    """Proxy of C++ AnimationPlayer class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxAnimationPlayer instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self, AnimationBase animation=None, bool destroyAnimation=False) -> AnimationPlayer"""
        newobj = _animate.new_AnimationPlayer(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __del__(self, destroy=_animate.delete_AnimationPlayer):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def SetAnimation(*args, **kwargs):
        """SetAnimation(self, AnimationBase animation, bool destroyAnimation=False)"""
        return _animate.AnimationPlayer_SetAnimation(*args, **kwargs)

    def GetAnimation(*args, **kwargs):
        """GetAnimation(self) -> AnimationBase"""
        return _animate.AnimationPlayer_GetAnimation(*args, **kwargs)

    def SetDestroyAnimation(*args, **kwargs):
        """SetDestroyAnimation(self, bool destroyAnimation)"""
        return _animate.AnimationPlayer_SetDestroyAnimation(*args, **kwargs)

    def GetDestroyAnimation(*args, **kwargs):
        """GetDestroyAnimation(self) -> bool"""
        return _animate.AnimationPlayer_GetDestroyAnimation(*args, **kwargs)

    def SetCurrentFrame(*args, **kwargs):
        """SetCurrentFrame(self, int currentFrame)"""
        return _animate.AnimationPlayer_SetCurrentFrame(*args, **kwargs)

    def GetCurrentFrame(*args, **kwargs):
        """GetCurrentFrame(self) -> int"""
        return _animate.AnimationPlayer_GetCurrentFrame(*args, **kwargs)

    def SetWindow(*args, **kwargs):
        """SetWindow(self, Window window)"""
        return _animate.AnimationPlayer_SetWindow(*args, **kwargs)

    def GetWindow(*args, **kwargs):
        """GetWindow(self) -> Window"""
        return _animate.AnimationPlayer_GetWindow(*args, **kwargs)

    def SetPosition(*args, **kwargs):
        """SetPosition(self, Point pos)"""
        return _animate.AnimationPlayer_SetPosition(*args, **kwargs)

    def GetPosition(*args, **kwargs):
        """GetPosition(self) -> Point"""
        return _animate.AnimationPlayer_GetPosition(*args, **kwargs)

    def SetLooped(*args, **kwargs):
        """SetLooped(self, bool looped)"""
        return _animate.AnimationPlayer_SetLooped(*args, **kwargs)

    def GetLooped(*args, **kwargs):
        """GetLooped(self) -> bool"""
        return _animate.AnimationPlayer_GetLooped(*args, **kwargs)

    def HasAnimation(*args, **kwargs):
        """HasAnimation(self) -> bool"""
        return _animate.AnimationPlayer_HasAnimation(*args, **kwargs)

    def IsPlaying(*args, **kwargs):
        """IsPlaying(self) -> bool"""
        return _animate.AnimationPlayer_IsPlaying(*args, **kwargs)

    def UseBackgroundColour(*args, **kwargs):
        """UseBackgroundColour(self, bool useBackground)"""
        return _animate.AnimationPlayer_UseBackgroundColour(*args, **kwargs)

    def UsingBackgroundColour(*args, **kwargs):
        """UsingBackgroundColour(self) -> bool"""
        return _animate.AnimationPlayer_UsingBackgroundColour(*args, **kwargs)

    def SetCustomBackgroundColour(*args, **kwargs):
        """SetCustomBackgroundColour(self, Colour col, bool useCustomBackgroundColour=True)"""
        return _animate.AnimationPlayer_SetCustomBackgroundColour(*args, **kwargs)

    def UsingCustomBackgroundColour(*args, **kwargs):
        """UsingCustomBackgroundColour(self) -> bool"""
        return _animate.AnimationPlayer_UsingCustomBackgroundColour(*args, **kwargs)

    def GetCustomBackgroundColour(*args, **kwargs):
        """GetCustomBackgroundColour(self) -> Colour"""
        return _animate.AnimationPlayer_GetCustomBackgroundColour(*args, **kwargs)

    def UseParentBackground(*args, **kwargs):
        """UseParentBackground(self, bool useParent)"""
        return _animate.AnimationPlayer_UseParentBackground(*args, **kwargs)

    def UsingParentBackground(*args, **kwargs):
        """UsingParentBackground(self) -> bool"""
        return _animate.AnimationPlayer_UsingParentBackground(*args, **kwargs)

    def Play(*args, **kwargs):
        """Play(self, Window window, Point pos=wxPoint(0, 0), bool looped=True) -> bool"""
        return _animate.AnimationPlayer_Play(*args, **kwargs)

    def Build(*args, **kwargs):
        """Build(self) -> bool"""
        return _animate.AnimationPlayer_Build(*args, **kwargs)

    def Stop(*args, **kwargs):
        """Stop(self)"""
        return _animate.AnimationPlayer_Stop(*args, **kwargs)

    def Draw(*args, **kwargs):
        """Draw(self, DC dc)"""
        return _animate.AnimationPlayer_Draw(*args, **kwargs)

    def GetFrameCount(*args, **kwargs):
        """GetFrameCount(self) -> int"""
        return _animate.AnimationPlayer_GetFrameCount(*args, **kwargs)

    def GetFrame(*args, **kwargs):
        """GetFrame(self, int i) -> Image"""
        return _animate.AnimationPlayer_GetFrame(*args, **kwargs)

    def GetDisposalMethod(*args, **kwargs):
        """GetDisposalMethod(self, int i) -> int"""
        return _animate.AnimationPlayer_GetDisposalMethod(*args, **kwargs)

    def GetFrameRect(*args, **kwargs):
        """GetFrameRect(self, int i) -> Rect"""
        return _animate.AnimationPlayer_GetFrameRect(*args, **kwargs)

    def GetDelay(*args, **kwargs):
        """GetDelay(self, int i) -> int"""
        return _animate.AnimationPlayer_GetDelay(*args, **kwargs)

    def GetLogicalScreenSize(*args, **kwargs):
        """GetLogicalScreenSize(self) -> Size"""
        return _animate.AnimationPlayer_GetLogicalScreenSize(*args, **kwargs)

    def GetBackgroundColour(*args, **kwargs):
        """GetBackgroundColour(self, Colour col) -> bool"""
        return _animate.AnimationPlayer_GetBackgroundColour(*args, **kwargs)

    def GetTransparentColour(*args, **kwargs):
        """GetTransparentColour(self, Colour col) -> bool"""
        return _animate.AnimationPlayer_GetTransparentColour(*args, **kwargs)

    def PlayFrame(*args, **kwargs):
        """PlayFrame(self, int frame, Window window, Point pos) -> bool"""
        return _animate.AnimationPlayer_PlayFrame(*args, **kwargs)

    def PlayNextFrame(*args, **kwargs):
        """PlayNextFrame(self) -> bool"""
        return _animate.AnimationPlayer_PlayNextFrame(*args, **kwargs)

    def DrawFrame(*args, **kwargs):
        """DrawFrame(self, int frame, DC dc, Point pos)"""
        return _animate.AnimationPlayer_DrawFrame(*args, **kwargs)

    def DrawBackground(*args, **kwargs):
        """DrawBackground(self, DC dc, Point pos, Colour colour)"""
        return _animate.AnimationPlayer_DrawBackground(*args, **kwargs)

    def ClearCache(*args, **kwargs):
        """ClearCache(self)"""
        return _animate.AnimationPlayer_ClearCache(*args, **kwargs)

    def SaveBackground(*args, **kwargs):
        """SaveBackground(self, Rect rect)"""
        return _animate.AnimationPlayer_SaveBackground(*args, **kwargs)

    def GetBackingStore(*args, **kwargs):
        """GetBackingStore(self) -> Bitmap"""
        return _animate.AnimationPlayer_GetBackingStore(*args, **kwargs)


class AnimationPlayerPtr(AnimationPlayer):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = AnimationPlayer
_animate.AnimationPlayer_swigregister(AnimationPlayerPtr)
cvar = _animate.cvar
AnimationControlNameStr = cvar.AnimationControlNameStr

class AnimationBase(_core.Object):
    """Proxy of C++ AnimationBase class"""
    def __init__(self): raise RuntimeError, "No constructor defined"
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxAnimationBase instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __del__(self, destroy=_animate.delete_AnimationBase):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def GetFrameCount(*args, **kwargs):
        """GetFrameCount(self) -> int"""
        return _animate.AnimationBase_GetFrameCount(*args, **kwargs)

    def GetFrame(*args, **kwargs):
        """GetFrame(self, int i) -> Image"""
        return _animate.AnimationBase_GetFrame(*args, **kwargs)

    def GetDisposalMethod(*args, **kwargs):
        """GetDisposalMethod(self, int i) -> int"""
        return _animate.AnimationBase_GetDisposalMethod(*args, **kwargs)

    def GetFrameRect(*args, **kwargs):
        """GetFrameRect(self, int i) -> Rect"""
        return _animate.AnimationBase_GetFrameRect(*args, **kwargs)

    def GetDelay(*args, **kwargs):
        """GetDelay(self, int i) -> int"""
        return _animate.AnimationBase_GetDelay(*args, **kwargs)

    def GetLogicalScreenSize(*args, **kwargs):
        """GetLogicalScreenSize(self) -> Size"""
        return _animate.AnimationBase_GetLogicalScreenSize(*args, **kwargs)

    def GetBackgroundColour(*args, **kwargs):
        """GetBackgroundColour(self, Colour col) -> bool"""
        return _animate.AnimationBase_GetBackgroundColour(*args, **kwargs)

    def GetTransparentColour(*args, **kwargs):
        """GetTransparentColour(self, Colour col) -> bool"""
        return _animate.AnimationBase_GetTransparentColour(*args, **kwargs)

    def IsValid(*args, **kwargs):
        """IsValid(self) -> bool"""
        return _animate.AnimationBase_IsValid(*args, **kwargs)

    def LoadFile(*args, **kwargs):
        """LoadFile(self, String filename) -> bool"""
        return _animate.AnimationBase_LoadFile(*args, **kwargs)


class AnimationBasePtr(AnimationBase):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = AnimationBase
_animate.AnimationBase_swigregister(AnimationBasePtr)

class GIFAnimation(AnimationBase):
    """Proxy of C++ GIFAnimation class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxGIFAnimation instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self) -> GIFAnimation"""
        newobj = _animate.new_GIFAnimation(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __del__(self, destroy=_animate.delete_GIFAnimation):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def GetFrameCount(*args, **kwargs):
        """GetFrameCount(self) -> int"""
        return _animate.GIFAnimation_GetFrameCount(*args, **kwargs)

    def GetFrame(*args, **kwargs):
        """GetFrame(self, int i) -> Image"""
        return _animate.GIFAnimation_GetFrame(*args, **kwargs)

    def GetDisposalMethod(*args, **kwargs):
        """GetDisposalMethod(self, int i) -> int"""
        return _animate.GIFAnimation_GetDisposalMethod(*args, **kwargs)

    def GetFrameRect(*args, **kwargs):
        """GetFrameRect(self, int i) -> Rect"""
        return _animate.GIFAnimation_GetFrameRect(*args, **kwargs)

    def GetDelay(*args, **kwargs):
        """GetDelay(self, int i) -> int"""
        return _animate.GIFAnimation_GetDelay(*args, **kwargs)

    def GetLogicalScreenSize(*args, **kwargs):
        """GetLogicalScreenSize(self) -> Size"""
        return _animate.GIFAnimation_GetLogicalScreenSize(*args, **kwargs)

    def GetBackgroundColour(*args, **kwargs):
        """GetBackgroundColour(self, Colour col) -> bool"""
        return _animate.GIFAnimation_GetBackgroundColour(*args, **kwargs)

    def GetTransparentColour(*args, **kwargs):
        """GetTransparentColour(self, Colour col) -> bool"""
        return _animate.GIFAnimation_GetTransparentColour(*args, **kwargs)

    def IsValid(*args, **kwargs):
        """IsValid(self) -> bool"""
        return _animate.GIFAnimation_IsValid(*args, **kwargs)

    def LoadFile(*args, **kwargs):
        """LoadFile(self, String filename) -> bool"""
        return _animate.GIFAnimation_LoadFile(*args, **kwargs)


class GIFAnimationPtr(GIFAnimation):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = GIFAnimation
_animate.GIFAnimation_swigregister(GIFAnimationPtr)

AN_FIT_ANIMATION = _animate.AN_FIT_ANIMATION
class GIFAnimationCtrl(_core.Control):
    """Proxy of C++ GIFAnimationCtrl class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxGIFAnimationCtrl instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """
        __init__(self, Window parent, int id=-1, String filename=EmptyString, 
            Point pos=DefaultPosition, Size size=DefaultSize, 
            long style=wxAN_FIT_ANIMATION|wxNO_BORDER, 
            String name=AnimationControlNameStr) -> GIFAnimationCtrl
        """
        newobj = _animate.new_GIFAnimationCtrl(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)

    def Create(*args, **kwargs):
        """
        Create(self, Window parent, int id=-1, String filename=EmptyString, 
            Point pos=DefaultPosition, Size size=DefaultSize, 
            long style=wxAN_FIT_ANIMATION|wxNO_BORDER, 
            String name=AnimationControlNameStr) -> bool
        """
        return _animate.GIFAnimationCtrl_Create(*args, **kwargs)

    def LoadFile(*args, **kwargs):
        """LoadFile(self, String filename=EmptyString) -> bool"""
        return _animate.GIFAnimationCtrl_LoadFile(*args, **kwargs)

    def Play(*args, **kwargs):
        """Play(self, bool looped=True) -> bool"""
        return _animate.GIFAnimationCtrl_Play(*args, **kwargs)

    def Stop(*args, **kwargs):
        """Stop(self)"""
        return _animate.GIFAnimationCtrl_Stop(*args, **kwargs)

    def FitToAnimation(*args, **kwargs):
        """FitToAnimation(self)"""
        return _animate.GIFAnimationCtrl_FitToAnimation(*args, **kwargs)

    def IsPlaying(*args, **kwargs):
        """IsPlaying(self) -> bool"""
        return _animate.GIFAnimationCtrl_IsPlaying(*args, **kwargs)

    def GetPlayer(*args, **kwargs):
        """GetPlayer(self) -> AnimationPlayer"""
        return _animate.GIFAnimationCtrl_GetPlayer(*args, **kwargs)

    def GetAnimation(*args, **kwargs):
        """GetAnimation(self) -> AnimationBase"""
        return _animate.GIFAnimationCtrl_GetAnimation(*args, **kwargs)

    def GetFilename(*args, **kwargs):
        """GetFilename(self) -> String"""
        return _animate.GIFAnimationCtrl_GetFilename(*args, **kwargs)

    def SetFilename(*args, **kwargs):
        """SetFilename(self, String filename)"""
        return _animate.GIFAnimationCtrl_SetFilename(*args, **kwargs)


class GIFAnimationCtrlPtr(GIFAnimationCtrl):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = GIFAnimationCtrl
_animate.GIFAnimationCtrl_swigregister(GIFAnimationCtrlPtr)

def PreGIFAnimationCtrl(*args, **kwargs):
    """PreGIFAnimationCtrl() -> GIFAnimationCtrl"""
    val = _animate.new_PreGIFAnimationCtrl(*args, **kwargs)
    val.thisown = 1
    return val


