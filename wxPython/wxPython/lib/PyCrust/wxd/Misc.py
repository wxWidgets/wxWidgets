"""Decorator classes for documentation and shell scripting.
"""

__author__ = "Patrick K. O'Brien <pobrien@orbtech.com>"
__cvsid__ = "$Id$"
__revision__ = "$Revision$"[11:-2]


# These are not the real wxPython classes. These are Python versions
# for documentation purposes. They are also used to apply docstrings
# to the real wxPython classes, which are SWIG-generated wrappers for
# C-language classes.


from Base import Object
import Parameters as wx


class ArtProvider(Object):
    """"""

    def __init__(self):
        """"""
        pass

    def _setCallbackInfo(self):
        """"""
        pass


class BusyCursor:
    """"""

    def __del__(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


class BusyInfo(Object):
    """"""

    def __del__(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


class Caret:
    """"""

    def GetPosition(self):
        """"""
        pass

    def GetPositionTuple(self):
        """"""
        pass

    def GetSize(self):
        """"""
        pass

    def GetSizeTuple(self):
        """"""
        pass

    def GetWindow(self):
        """"""
        pass

    def Hide(self):
        """"""
        pass

    def IsOk(self):
        """"""
        pass

    def IsVisible(self):
        """"""
        pass

    def Move(self):
        """"""
        pass

    def MoveXY(self):
        """"""
        pass

    def SetSize(self):
        """"""
        pass

    def SetSizeWH(self):
        """"""
        pass

    def Show(self):
        """"""
        pass

    def __del__(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


class EncodingConverter(Object):
    """"""

    def Convert(self):
        """"""
        pass

    def Init(self):
        """"""
        pass

    def __del__(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


class DirItemData(Object):
    """"""

    def SetNewDirName(self):
        """"""
        pass

    def __getattr__(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass

    def __setattr__(self):
        """"""
        pass


class Effects(Object):
    """"""

    def DrawSunkenEdge(self):
        """"""
        pass

    def GetDarkShadow(self):
        """"""
        pass

    def GetFaceColour(self):
        """"""
        pass

    def GetHighlightColour(self):
        """"""
        pass

    def GetLightShadow(self):
        """"""
        pass

    def GetMediumShadow(self):
        """"""
        pass

    def Set(self):
        """"""
        pass

    def SetDarkShadow(self):
        """"""
        pass

    def SetFaceColour(self):
        """"""
        pass

    def SetHighlightColour(self):
        """"""
        pass

    def SetLightShadow(self):
        """"""
        pass

    def SetMediumShadow(self):
        """"""
        pass

    def TileBitmap(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


class FontEnumerator:
    """"""

    def EnumerateEncodings(self):
        """"""
        pass

    def EnumerateFacenames(self):
        """"""
        pass

    def GetEncodings(self):
        """"""
        pass

    def GetFacenames(self):
        """"""
        pass

    def __del__(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass

    def _setCallbackInfo(self):
        """"""
        pass


class FontMapper:
    """"""

    def CharsetToEncoding(self):
        """"""
        pass

    def GetAltForEncoding(self):
        """"""
        pass

    def IsEncodingAvailable(self):
        """"""
        pass

    def SetConfig(self):
        """"""
        pass

    def SetConfigPath(self):
        """"""
        pass

    def SetDialogParent(self):
        """"""
        pass

    def SetDialogTitle(self):
        """"""
        pass

    def __del__(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


class LanguageInfo:
    """"""

    def __getattr__(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass

    def __setattr__(self):
        """"""
        pass


class Locale:
    """"""

    def AddCatalog(self):
        """"""
        pass

    def GetCanonicalName(self):
        """"""
        pass

    def GetLanguage(self):
        """"""
        pass

    def GetLocale(self):
        """"""
        pass

    def GetName(self):
        """"""
        pass

    def GetString(self):
        """"""
        pass

    def GetSysName(self):
        """"""
        pass

    def Init(self):
        """"""
        pass

    def IsLoaded(self):
        """"""
        pass

    def IsOk(self):
        """"""
        pass

    def __del__(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


class NativeFontInfo:
    """"""

    def FromString(self):
        """"""
        pass

    def FromUserString(self):
        """"""
        pass

    def GetEncoding(self):
        """"""
        pass

    def GetFaceName(self):
        """"""
        pass

    def GetFamily(self):
        """"""
        pass

    def GetPointSize(self):
        """"""
        pass

    def GetStyle(self):
        """"""
        pass

    def GetUnderlined(self):
        """"""
        pass

    def GetWeight(self):
        """"""
        pass

    def Init(self):
        """"""
        pass

    def SetEncoding(self):
        """"""
        pass

    def SetFaceName(self):
        """"""
        pass

    def SetFamily(self):
        """"""
        pass

    def SetPointSize(self):
        """"""
        pass

    def SetStyle(self):
        """"""
        pass

    def SetUnderlined(self):
        """"""
        pass

    def SetWeight(self):
        """"""
        pass

    def ToString(self):
        """"""
        pass

    def ToUserString(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass

    def __str__(self):
        """"""
        pass


class PyTimer(Object):
    """"""

    def GetInterval(self):
        """"""
        pass

    def IsOneShot(self):
        """"""
        pass

    def IsRunning(self):
        """"""
        pass

    def SetOwner(self):
        """"""
        pass

    def Start(self):
        """"""
        pass

    def Stop(self):
        """"""
        pass

    def __del__(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


class StopWatch:
    """"""

    def Pause(self):
        """"""
        pass

    def Resume(self):
        """"""
        pass

    def Start(self):
        """"""
        pass

    def Time(self):
        """"""
        pass

    def __del__(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


class SystemSettings:
    """"""

    def __init__(self):
        """"""
        pass


class Timer(PyTimer):
    """"""

    def __init__(self, evtHandler=None, id=-1):
        """Create a Timer instance."""
        pass


class Wave(Object):
    """"""

    def IsOk(self):
        """"""
        pass

    def Play(self):
        """"""
        pass

    def __del__(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


class WindowDisabler:
    """"""

    def __del__(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


