# This file was created automatically by SWIG.
# Don't modify this file, modify the SWIG interface instead.

"""
Classes for a media player control
"""

import _media

import _core
wx = _core 
__docfilter__ = wx.__DocFilter(globals()) 
MEDIASTATE_STOPPED = _media.MEDIASTATE_STOPPED
MEDIASTATE_PAUSED = _media.MEDIASTATE_PAUSED
MEDIASTATE_PLAYING = _media.MEDIASTATE_PLAYING
class MediaEvent(_core.NotifyEvent):
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxMediaEvent instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self, wxEventType commandType=wxEVT_NULL, int id=0) -> MediaEvent"""
        newobj = _media.new_MediaEvent(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown

class MediaEventPtr(MediaEvent):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = MediaEvent
_media.MediaEvent_swigregister(MediaEventPtr)

class MediaCtrl(_core.Control):
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxMediaCtrl instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """
        __init__(self, Window parent, int id=-1, String fileName=EmptyString, 
            Point pos=DefaultPosition, Size size=DefaultSize, 
            long style=0, String szBackend=EmptyString, 
            Validator validator=DefaultValidator, 
            String name=MediaCtrlNameStr) -> MediaCtrl
        """
        newobj = _media.new_MediaCtrl(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
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

    def Load(*args, **kwargs):
        """Load(self, String fileName) -> bool"""
        return _media.MediaCtrl_Load(*args, **kwargs)

    def LoadFromURI(*args, **kwargs):
        """LoadFromURI(self, String location) -> bool"""
        return _media.MediaCtrl_LoadFromURI(*args, **kwargs)

    def Loop(*args, **kwargs):
        """Loop(self, bool bLoop=True)"""
        return _media.MediaCtrl_Loop(*args, **kwargs)

    def IsLooped(*args, **kwargs):
        """IsLooped(self) -> bool"""
        return _media.MediaCtrl_IsLooped(*args, **kwargs)

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


class MediaCtrlPtr(MediaCtrl):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = MediaCtrl
_media.MediaCtrl_swigregister(MediaCtrlPtr)
cvar = _media.cvar
MediaCtrlNameStr = cvar.MediaCtrlNameStr

def PreMediaCtrl(*args, **kwargs):
    """PreMediaCtrl() -> MediaCtrl"""
    val = _media.new_PreMediaCtrl(*args, **kwargs)
    val.thisown = 1
    return val

wxEVT_MEDIA_FINISHED = _media.wxEVT_MEDIA_FINISHED
wxEVT_MEDIA_STOP = _media.wxEVT_MEDIA_STOP
EVT_MEDIA_FINISHED = wx.PyEventBinder( wxEVT_MEDIA_FINISHED, 1)
EVT_MEDIA_STOP     = wx.PyEventBinder( wxEVT_MEDIA_STOP, 1)


