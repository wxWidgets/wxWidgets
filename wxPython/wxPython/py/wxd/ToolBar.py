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
from Controls import Control
import Parameters as wx


class ToolBarBase(Control):
    """"""

    def AddCheckLabelTool(self):
        """"""
        pass

    def AddCheckTool(self):
        """"""
        pass

    def AddControl(self):
        """"""
        pass

    def AddLabelTool(self):
        """"""
        pass

    def AddRadioLabelTool(self):
        """"""
        pass

    def AddRadioTool(self):
        """"""
        pass

    def AddSeparator(self):
        """"""
        pass

    def AddSimpleTool(self):
        """"""
        pass

    def AddTool(self):
        """"""
        pass

    def ClearTools(self):
        """"""
        pass

    def DeleteTool(self):
        """"""
        pass

    def DeleteToolByPos(self):
        """"""
        pass

    def DoAddTool(self):
        """"""
        pass

    def DoInsertTool(self):
        """"""
        pass

    def EnableTool(self):
        """"""
        pass

    def FindControl(self):
        """"""
        pass

    def FindToolForPosition(self):
        """"""
        pass

    def GetMargins(self):
        """"""
        pass

    def GetMaxCols(self):
        """"""
        pass

    def GetMaxRows(self):
        """"""
        pass

    def GetToolBitmapSize(self):
        """"""
        pass

    def GetToolClientData(self):
        """"""
        pass

    def GetToolEnabled(self):
        """"""
        pass

    def GetToolLongHelp(self):
        """"""
        pass

    def GetToolMargins(self):
        """"""
        pass

    def GetToolPacking(self):
        """"""
        pass

    def GetToolSeparation(self):
        """"""
        pass

    def GetToolShortHelp(self):
        """"""
        pass

    def GetToolSize(self):
        """"""
        pass

    def GetToolState(self):
        """"""
        pass

    def InsertControl(self):
        """"""
        pass

    def InsertLabelTool(self):
        """"""
        pass

    def InsertSeparator(self):
        """"""
        pass

    def InsertSimpleTool(self):
        """"""
        pass

    def InsertTool(self):
        """"""
        pass

    def IsVertical(self):
        """"""
        pass

    def Realize(self):
        """"""
        pass

    def RemoveTool(self):
        """"""
        pass

    def SetMargins(self):
        """"""
        pass

    def SetMarginsXY(self):
        """"""
        pass

    def SetMaxRowsCols(self):
        """"""
        pass

    def SetRows(self):
        """"""
        pass

    def SetToggle(self):
        """"""
        pass

    def SetToolBitmapSize(self):
        """"""
        pass

    def SetToolClientData(self):
        """"""
        pass

    def SetToolLongHelp(self):
        """"""
        pass

    def SetToolPacking(self):
        """"""
        pass

    def SetToolSeparation(self):
        """"""
        pass

    def SetToolShortHelp(self):
        """"""
        pass

    def ToggleTool(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


class ToolBar(ToolBarBase):
    """"""

    def Create(self):
        """"""
        pass

    def FindToolForPosition(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


class ToolBarSimple(ToolBarBase):
    """"""

    def Create(self):
        """"""
        pass

    def FindToolForPosition(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


class ToolBarToolBase(Object):
    """"""

    def Attach(self):
        """"""
        pass

    def CanBeToggled(self):
        """"""
        pass

    def Destroy(self):
        """"""
        pass

    def Detach(self):
        """"""
        pass

    def Enable(self):
        """"""
        pass

    def GetBitmap(self):
        """"""
        pass

    def GetBitmap1(self):
        """"""
        pass

    def GetBitmap2(self):
        """"""
        pass

    def GetClientData(self):
        """"""
        pass

    def GetControl(self):
        """"""
        pass

    def GetDisabledBitmap(self):
        """"""
        pass

    def GetId(self):
        """"""
        pass

    def GetKind(self):
        """"""
        pass

    def GetLabel(self):
        """"""
        pass

    def GetLongHelp(self):
        """"""
        pass

    def GetNormalBitmap(self):
        """"""
        pass

    def GetShortHelp(self):
        """"""
        pass

    def GetStyle(self):
        """"""
        pass

    def GetToolBar(self):
        """"""
        pass

    def IsButton(self):
        """"""
        pass

    def IsControl(self):
        """"""
        pass

    def IsEnabled(self):
        """"""
        pass

    def IsSeparator(self):
        """"""
        pass

    def IsToggled(self):
        """"""
        pass

    def SetBitmap1(self):
        """"""
        pass

    def SetBitmap2(self):
        """"""
        pass

    def SetClientData(self):
        """"""
        pass

    def SetDisabledBitmap(self):
        """"""
        pass

    def SetLabel(self):
        """"""
        pass

    def SetLongHelp(self):
        """"""
        pass

    def SetNormalBitmap(self):
        """"""
        pass

    def SetShortHelp(self):
        """"""
        pass

    def SetToggle(self):
        """"""
        pass

    def Toggle(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


