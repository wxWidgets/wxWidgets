"""Decorator classes for documentation and shell scripting.
"""

__author__ = "Patrick K. O'Brien <pobrien@orbtech.com>"
__cvsid__ = "$Id$"
__revision__ = "$Revision$"[11:-2]


# These are not the real wxPython classes. These are Python versions
# for documentation purposes. They are also used to apply docstrings
# to the real wxPython classes, which are SWIG-generated wrappers for
# C-language classes.


from wxBase import wxObject
import wxParameters as wx


class wxClipboard(wxObject):
    """"""

    def AddData(self):
        """"""
        pass

    def Clear(self):
        """"""
        pass

    def Close(self):
        """"""
        pass

    def Flush(self):
        """"""
        pass

    def GetData(self):
        """"""
        pass

    def IsOpened(self):
        """"""
        pass

    def IsSupported(self):
        """"""
        pass

    def Open(self):
        """"""
        pass

    def SetData(self):
        """"""
        pass

    def UsePrimarySelection(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


class wxDataFormat:
    """"""

    def GetId(self):
        """"""
        pass

    def GetType(self):
        """"""
        pass

    def SetId(self):
        """"""
        pass

    def SetType(self):
        """"""
        pass

    def __del__(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


class wxDataObject:
    """"""

    def GetAllFormats(self):
        """"""
        pass

    def GetDataHere(self):
        """"""
        pass

    def GetDataSize(self):
        """"""
        pass

    def GetFormatCount(self):
        """"""
        pass

    def GetPreferredFormat(self):
        """"""
        pass

    def IsSupportedFormat(self):
        """"""
        pass

    def SetData(self):
        """"""
        pass

    def __del__(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


class wxDataObjectComposite(wxDataObject):
    """"""

    def Add(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


class wxDataObjectSimple(wxDataObject):
    """"""

    def GetFormat(self):
        """"""
        pass

    def SetFormat(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


class wxPyDataObjectSimple(wxDataObjectSimple):
    """"""

    def __init__(self):
        """"""
        pass

    def _setCallbackInfo(self):
        """"""
        pass


class wxBitmapDataObject(wxDataObjectSimple):
    """"""

    def GetBitmap(self):
        """"""
        pass

    def SetBitmap(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


class wxPyBitmapDataObject(wxBitmapDataObject):
    """"""

    def __init__(self):
        """"""
        pass

    def _setCallbackInfo(self):
        """"""
        pass


class wxCustomDataObject(wxDataObjectSimple):
    """"""

    def GetData(self):
        """"""
        pass

    def GetSize(self):
        """"""
        pass

    def SetData(self):
        """"""
        pass

    def TakeData(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


class wxDragImage(wxObject):
    """"""

    def BeginDrag(self):
        """"""
        pass

    def BeginDrag2(self):
        """"""
        pass

    def EndDrag(self):
        """"""
        pass

    def GetImageRect(self):
        """"""
        pass

    def Hide(self):
        """"""
        pass

    def Move(self):
        """"""
        pass

    def RedrawImage(self):
        """"""
        pass

    def SetBackingBitmap(self):
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


class wxDropSource:
    """"""

    def DoDragDrop(self):
        """"""
        pass

    def GetDataObject(self):
        """"""
        pass

    def SetCursor(self):
        """"""
        pass

    def SetData(self):
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

    def base_GiveFeedback(self):
        """"""
        pass


class wxDropTarget:
    """"""

    def __init__(self):
        """"""
        pass


class wxPyDropTarget(wxDropTarget):
    """"""

    def GetData(self):
        """"""
        pass

    def GetDataObject(self):
        """"""
        pass

    def SetDataObject(self):
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

    def base_OnDragOver(self):
        """"""
        pass

    def base_OnDrop(self):
        """"""
        pass

    def base_OnEnter(self):
        """"""
        pass

    def base_OnLeave(self):
        """"""
        pass


class wxFileDataObject(wxDataObjectSimple):
    """"""

    def GetFilenames(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


class wxFileDropTarget(wxPyDropTarget):
    """"""

    def __init__(self):
        """"""
        pass

    def _setCallbackInfo(self):
        """"""
        pass

    def base_OnData(self):
        """"""
        pass

    def base_OnDragOver(self):
        """"""
        pass

    def base_OnDrop(self):
        """"""
        pass

    def base_OnEnter(self):
        """"""
        pass

    def base_OnLeave(self):
        """"""
        pass


class wxTextDataObject(wxDataObjectSimple):
    """"""

    def GetText(self):
        """"""
        pass

    def GetTextLength(self):
        """"""
        pass

    def SetText(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


class wxPyTextDataObject(wxTextDataObject):
    """"""

    def __init__(self):
        """"""
        pass

    def _setCallbackInfo(self):
        """"""
        pass


class wxTextDropTarget(wxPyDropTarget):
    """"""

    def __init__(self):
        """"""
        pass

    def _setCallbackInfo(self):
        """"""
        pass

    def base_OnData(self):
        """"""
        pass

    def base_OnDragOver(self):
        """"""
        pass

    def base_OnDrop(self):
        """"""
        pass

    def base_OnEnter(self):
        """"""
        pass

    def base_OnLeave(self):
        """"""
        pass


class wxURLDataObject(wxDataObjectComposite):
    """"""

    def GetURL(self):
        """"""
        pass

    def SetURL(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass

