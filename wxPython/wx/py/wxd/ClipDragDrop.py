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


class Clipboard(Object):
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


class DataFormat:
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


class DataObject:
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


class DataObjectComposite(DataObject):
    """"""

    def Add(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


class DataObjectSimple(DataObject):
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


class PyDataObjectSimple(DataObjectSimple):
    """"""

    def __init__(self):
        """"""
        pass

    def _setCallbackInfo(self):
        """"""
        pass


class BitmapDataObject(DataObjectSimple):
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


class PyBitmapDataObject(BitmapDataObject):
    """"""

    def __init__(self):
        """"""
        pass

    def _setCallbackInfo(self):
        """"""
        pass


class CustomDataObject(DataObjectSimple):
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


class DragImage(Object):
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


class DropSource:
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


class DropTarget:
    """"""

    def __init__(self):
        """"""
        pass


class PyDropTarget(DropTarget):
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


class FileDataObject(DataObjectSimple):
    """"""

    def GetFilenames(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


class FileDropTarget(PyDropTarget):
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


class TextDataObject(DataObjectSimple):
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


class PyTextDataObject(TextDataObject):
    """"""

    def __init__(self):
        """"""
        pass

    def _setCallbackInfo(self):
        """"""
        pass


class TextDropTarget(PyDropTarget):
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


class URLDataObject(DataObjectComposite):
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

