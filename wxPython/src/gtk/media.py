# This file was created automatically by SWIG 1.3.29.
# Don't modify this file, modify the SWIG interface instead.

"""
Classes for a media player control
"""

import _media
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
MEDIASTATE_STOPPED = _media.MEDIASTATE_STOPPED
MEDIASTATE_PAUSED = _media.MEDIASTATE_PAUSED
MEDIASTATE_PLAYING = _media.MEDIASTATE_PLAYING
MEDIACTRLPLAYERCONTROLS_NONE = _media.MEDIACTRLPLAYERCONTROLS_NONE
MEDIACTRLPLAYERCONTROLS_STEP = _media.MEDIACTRLPLAYERCONTROLS_STEP
MEDIACTRLPLAYERCONTROLS_VOLUME = _media.MEDIACTRLPLAYERCONTROLS_VOLUME
MEDIACTRLPLAYERCONTROLS_DEFAULT = _media.MEDIACTRLPLAYERCONTROLS_DEFAULT
class MediaEvent(_core.NotifyEvent):
    """Proxy of C++ MediaEvent class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """__init__(self, EventType commandType=wxEVT_NULL, int id=0) -> MediaEvent"""
        _media.MediaEvent_swiginit(self,_media.new_MediaEvent(*args, **kwargs))
_media.MediaEvent_swigregister(MediaEvent)
cvar = _media.cvar
MEDIABACKEND_DIRECTSHOW = cvar.MEDIABACKEND_DIRECTSHOW
MEDIABACKEND_MCI = cvar.MEDIABACKEND_MCI
MEDIABACKEND_QUICKTIME = cvar.MEDIABACKEND_QUICKTIME
MEDIABACKEND_GSTREAMER = cvar.MEDIABACKEND_GSTREAMER
MEDIABACKEND_REALPLAYER = cvar.MEDIABACKEND_REALPLAYER
MEDIABACKEND_WMP10 = cvar.MEDIABACKEND_WMP10

class MediaCtrl(_core.Control):
    """Proxy of C++ MediaCtrl class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, Window parent, int id=-1, String fileName=EmptyString, 
            Point pos=DefaultPosition, Size size=DefaultSize, 
            long style=0, String szBackend=EmptyString, 
            Validator validator=DefaultValidator, 
            String name=MediaCtrlNameStr) -> MediaCtrl
        """
        _media.MediaCtrl_swiginit(self,_media.new_MediaCtrl(*args, **kwargs))
        self._setOORInfo(self)

    def Create(*args, **kwargs):
        """
        Create(self, Window parent, int id=-1, String fileName=EmptyString, 
            Point pos=DefaultPosition, Size size=DefaultSize, 
            long style=0, String szBackend=EmptyString, 
            Validator validator=DefaultValidator, 
            String name=MediaCtrlNameStr) -> bool
        """
        return _media.MediaCtrl_Create(*args, **kwargs)

    def Play(*args, **kwargs):
        """Play(self) -> bool"""
        return _media.MediaCtrl_Play(*args, **kwargs)

    def Pause(*args, **kwargs):
        """Pause(self) -> bool"""
        return _media.MediaCtrl_Pause(*args, **kwargs)

    def Stop(*args, **kwargs):
        """Stop(self) -> bool"""
        return _media.MediaCtrl_Stop(*args, **kwargs)

    def GetState(*args, **kwargs):
        """GetState(self) -> int"""
        return _media.MediaCtrl_GetState(*args, **kwargs)

    def GetPlaybackRate(*args, **kwargs):
        """GetPlaybackRate(self) -> double"""
        return _media.MediaCtrl_GetPlaybackRate(*args, **kwargs)

    def SetPlaybackRate(*args, **kwargs):
        """SetPlaybackRate(self, double dRate) -> bool"""
        return _media.MediaCtrl_SetPlaybackRate(*args, **kwargs)

    def Seek(*args, **kwargs):
        """Seek(self, wxFileOffset where, int mode=FromStart) -> wxFileOffset"""
        return _media.MediaCtrl_Seek(*args, **kwargs)

    def Tell(*args, **kwargs):
        """Tell(self) -> wxFileOffset"""
        return _media.MediaCtrl_Tell(*args, **kwargs)

    def Length(*args, **kwargs):
        """Length(self) -> wxFileOffset"""
        return _media.MediaCtrl_Length(*args, **kwargs)

    def GetVolume(*args, **kwargs):
        """GetVolume(self) -> double"""
        return _media.MediaCtrl_GetVolume(*args, **kwargs)

    def SetVolume(*args, **kwargs):
        """SetVolume(self, double dVolume) -> bool"""
        return _media.MediaCtrl_SetVolume(*args, **kwargs)

    def ShowPlayerControls(*args, **kwargs):
        """ShowPlayerControls(self, int flags=MEDIACTRLPLAYERCONTROLS_DEFAULT) -> bool"""
        return _media.MediaCtrl_ShowPlayerControls(*args, **kwargs)

    def Load(*args, **kwargs):
        """Load(self, String fileName) -> bool"""
        return _media.MediaCtrl_Load(*args, **kwargs)

    def LoadURI(*args, **kwargs):
        """LoadURI(self, String fileName) -> bool"""
        return _media.MediaCtrl_LoadURI(*args, **kwargs)

    def LoadURIWithProxy(*args, **kwargs):
        """LoadURIWithProxy(self, String fileName, String proxy) -> bool"""
        return _media.MediaCtrl_LoadURIWithProxy(*args, **kwargs)

    LoadFromURI = LoadURI 
    def GetDownloadProgress(*args, **kwargs):
        """GetDownloadProgress(self) -> wxFileOffset"""
        return _media.MediaCtrl_GetDownloadProgress(*args, **kwargs)

    def GetDownloadTotal(*args, **kwargs):
        """GetDownloadTotal(self) -> wxFileOffset"""
        return _media.MediaCtrl_GetDownloadTotal(*args, **kwargs)

    DownloadProgress = property(GetDownloadProgress,doc="See `GetDownloadProgress`") 
    DownloadTotal = property(GetDownloadTotal,doc="See `GetDownloadTotal`") 
    PlaybackRate = property(GetPlaybackRate,SetPlaybackRate,doc="See `GetPlaybackRate` and `SetPlaybackRate`") 
    State = property(GetState,doc="See `GetState`") 
    Volume = property(GetVolume,SetVolume,doc="See `GetVolume` and `SetVolume`") 
_media.MediaCtrl_swigregister(MediaCtrl)
MediaCtrlNameStr = cvar.MediaCtrlNameStr

def PreMediaCtrl(*args, **kwargs):
    """PreMediaCtrl() -> MediaCtrl"""
    val = _media.new_PreMediaCtrl(*args, **kwargs)
    return val

wxEVT_MEDIA_FINISHED = _media.wxEVT_MEDIA_FINISHED
wxEVT_MEDIA_STOP = _media.wxEVT_MEDIA_STOP
wxEVT_MEDIA_LOADED = _media.wxEVT_MEDIA_LOADED
wxEVT_MEDIA_STATECHANGED = _media.wxEVT_MEDIA_STATECHANGED
wxEVT_MEDIA_PLAY = _media.wxEVT_MEDIA_PLAY
wxEVT_MEDIA_PAUSE = _media.wxEVT_MEDIA_PAUSE
EVT_MEDIA_FINISHED       = wx.PyEventBinder( wxEVT_MEDIA_FINISHED, 1)
EVT_MEDIA_STOP           = wx.PyEventBinder( wxEVT_MEDIA_STOP, 1)
EVT_MEDIA_LOADED         = wx.PyEventBinder( wxEVT_MEDIA_LOADED, 1)
EVT_MEDIA_STATECHANGED   = wx.PyEventBinder( wxEVT_MEDIA_STATECHANGED, 1)
EVT_MEDIA_PLAY           = wx.PyEventBinder( wxEVT_MEDIA_PLAY, 1)
EVT_MEDIA_PAUSE          = wx.PyEventBinder( wxEVT_MEDIA_PAUSE, 1)



