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


class Sizer(Object):
    """"""

    def Add(self):
        """"""
        pass

    def AddMany(self):
        """"""
        pass

    def AddSizer(self):
        """"""
        pass

    def AddSpacer(self):
        """"""
        pass

    def AddWindow(self):
        """"""
        pass

    def Clear(self):
        """"""
        pass

    def DeleteWindows(self):
        """"""
        pass

    def Destroy(self):
        """"""
        pass

    def Fit(self):
        """"""
        pass

    def FitInside(self):
        """"""
        pass

    def GetChildren(self):
        """"""
        pass

    def GetMinSize(self):
        """"""
        pass

    def GetMinSizeTuple(self):
        """"""
        pass

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

    def Hide(self):
        """"""
        pass

    def HideSizer(self):
        """"""
        pass

    def HideWindow(self):
        """"""
        pass

    def Insert(self):
        """"""
        pass

    def InsertSizer(self):
        """"""
        pass

    def InsertSpacer(self):
        """"""
        pass

    def InsertWindow(self):
        """"""
        pass

    def IsShown(self):
        """"""
        pass

    def IsShownSizer(self):
        """"""
        pass

    def IsShownWindow(self):
        """"""
        pass

    def Layout(self):
        """"""
        pass

    def Prepend(self):
        """"""
        pass

    def PrependSizer(self):
        """"""
        pass

    def PrependSpacer(self):
        """"""
        pass

    def PrependWindow(self):
        """"""
        pass

    def Remove(self):
        """"""
        pass

    def RemovePos(self):
        """"""
        pass

    def RemoveSizer(self):
        """"""
        pass

    def RemoveWindow(self):
        """"""
        pass

    def SetDimension(self):
        """"""
        pass

    def SetItemMinSize(self):
        """"""
        pass

    def SetItemMinSizePos(self):
        """"""
        pass

    def SetItemMinSizeSizer(self):
        """"""
        pass

    def SetItemMinSizeWindow(self):
        """"""
        pass

    def SetMinSize(self):
        """"""
        pass

    def SetSizeHints(self):
        """"""
        pass

    def SetVirtualSizeHints(self):
        """"""
        pass

    def Show(self):
        """"""
        pass

    def ShowItems(self):
        """"""
        pass

    def ShowSizer(self):
        """"""
        pass

    def ShowWindow(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass

    def _setOORInfo(self):
        """"""
        pass


class SizerItem(Object):
    """"""

    def CalcMin(self):
        """"""
        pass

    def DeleteWindows(self):
        """"""
        pass

    def GetBorder(self):
        """"""
        pass

    def GetFlag(self):
        """"""
        pass

    def GetOption(self):
        """"""
        pass

    def GetPosition(self):
        """"""
        pass

    def GetRatio(self):
        """"""
        pass

    def GetSize(self):
        """"""
        pass

    def GetSizer(self):
        """"""
        pass

    def GetUserData(self):
        """"""
        pass

    def GetWindow(self):
        """"""
        pass

    def IsShown(self):
        """"""
        pass

    def IsSizer(self):
        """"""
        pass

    def IsSpacer(self):
        """"""
        pass

    def IsWindow(self):
        """"""
        pass

    def SetBorder(self):
        """"""
        pass

    def SetDimension(self):
        """"""
        pass

    def SetFlag(self):
        """"""
        pass

    def SetInitSize(self):
        """"""
        pass

    def SetOption(self):
        """"""
        pass

    def SetRatio(self):
        """"""
        pass

    def SetRatioSize(self):
        """"""
        pass

    def SetRatioWH(self):
        """"""
        pass

    def SetSizer(self):
        """"""
        pass

    def SetWindow(self):
        """"""
        pass

    def Show(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


class BoxSizer(Sizer):
    """"""

    def CalcMin(self):
        """"""
        pass

    def GetOrientation(self):
        """"""
        pass

    def RecalcSizes(self):
        """"""
        pass

    def SetOrientation(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


class GridSizer(Sizer):
    """"""

    def CalcMin(self):
        """"""
        pass

    def GetCols(self):
        """"""
        pass

    def GetHGap(self):
        """"""
        pass

    def GetRows(self):
        """"""
        pass

    def GetVGap(self):
        """"""
        pass

    def RecalcSizes(self):
        """"""
        pass

    def SetCols(self):
        """"""
        pass

    def SetHGap(self):
        """"""
        pass

    def SetRows(self):
        """"""
        pass

    def SetVGap(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


class FlexGridSizer(GridSizer):
    """"""

    def AddGrowableCol(self):
        """"""
        pass

    def AddGrowableRow(self):
        """"""
        pass

    def CalcMin(self):
        """"""
        pass

    def RecalcSizes(self):
        """"""
        pass

    def RemoveGrowableCol(self):
        """"""
        pass

    def RemoveGrowableRow(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


class NotebookSizer(Sizer):
    """"""

    def CalcMin(self):
        """"""
        pass

    def GetNotebook(self):
        """"""
        pass

    def RecalcSizes(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


class PySizer(Sizer):
    """"""

    def __init__(self):
        """"""
        pass

    def _setCallbackInfo(self):
        """"""
        pass


class StaticBoxSizer(BoxSizer):
    """"""

    def CalcMin(self):
        """"""
        pass

    def GetStaticBox(self):
        """"""
        pass

    def RecalcSizes(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass
