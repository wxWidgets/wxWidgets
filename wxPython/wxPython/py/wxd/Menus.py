"""Decorator classes for documentation and shell scripting.
"""

__author__ = "Patrick K. O'Brien <pobrien@orbtech.com>"
__cvsid__ = "$Id$"
__revision__ = "$Revision$"[11:-2]


# These are not the real wxPython classes. These are Python versions
# for documentation purposes. They are also used to apply docstrings
# to the real wxPython classes, which are SWIG-generated wrappers for
# C-language classes.


from Base import Object, EvtHandler
import Parameters as wx
from Window import Window


class FileHistory(Object):
    """"""

    def AddFileToHistory(self):
        """"""
        pass

    def AddFilesToMenu(self):
        """"""
        pass

    def AddFilesToThisMenu(self):
        """"""
        pass

    def GetCount(self):
        """"""
        pass

    def GetHistoryFile(self):
        """"""
        pass

    def GetMaxFiles(self):
        """"""
        pass

    def GetNoHistoryFiles(self):
        """"""
        pass

    def Load(self):
        """"""
        pass

    def RemoveFileFromHistory(self):
        """"""
        pass

    def RemoveMenu(self):
        """"""
        pass

    def Save(self):
        """"""
        pass

    def UseMenu(self):
        """"""
        pass

    def __del__(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


class Menu(EvtHandler):
    """"""

    def Append(self):
        """"""
        pass

    def AppendCheckItem(self):
        """"""
        pass

    def AppendItem(self):
        """"""
        pass

    def AppendMenu(self):
        """"""
        pass

    def AppendRadioItem(self):
        """"""
        pass

    def AppendSeparator(self):
        """"""
        pass

    def Break(self):
        """"""
        pass

    def Check(self):
        """"""
        pass

    def Delete(self):
        """"""
        pass

    def DeleteItem(self):
        """"""
        pass

    def Destroy(self):
        """"""
        pass

    def DestroyId(self):
        """"""
        pass

    def DestroyItem(self):
        """"""
        pass

    def Enable(self):
        """"""
        pass

    def FindItem(self):
        """"""
        pass

    def FindItemById(self):
        """"""
        pass

    def GetEventHandler(self):
        """"""
        pass

    def GetHelpString(self):
        """"""
        pass

    def GetInvokingWindow(self):
        """"""
        pass

    def GetLabel(self):
        """"""
        pass

    def GetMenuItemCount(self):
        """"""
        pass

    def GetMenuItems(self):
        """"""
        pass

    def GetParent(self):
        """"""
        pass

    def GetStyle(self):
        """"""
        pass

    def GetTitle(self):
        """"""
        pass

    def Insert(self):
        """"""
        pass

    def InsertCheckItem(self):
        """"""
        pass

    def InsertItem(self):
        """"""
        pass

    def InsertMenu(self):
        """"""
        pass

    def InsertRadioItem(self):
        """"""
        pass

    def InsertSeparator(self):
        """"""
        pass

    def IsAttached(self):
        """"""
        pass

    def IsChecked(self):
        """"""
        pass

    def IsEnabled(self):
        """"""
        pass

    def Prepend(self):
        """"""
        pass

    def PrependCheckItem(self):
        """"""
        pass

    def PrependItem(self):
        """"""
        pass

    def PrependMenu(self):
        """"""
        pass

    def PrependRadioItem(self):
        """"""
        pass

    def PrependSeparator(self):
        """"""
        pass

    def Remove(self):
        """"""
        pass

    def RemoveItem(self):
        """"""
        pass

    def SetEventHandler(self):
        """"""
        pass

    def SetHelpString(self):
        """"""
        pass

    def SetInvokingWindow(self):
        """"""
        pass

    def SetLabel(self):
        """"""
        pass

    def SetParent(self):
        """"""
        pass

    def SetTitle(self):
        """"""
        pass

    def UpdateUI(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


class MenuBar(Window):
    """"""

    def Append(self):
        """"""
        pass

    def Check(self):
        """"""
        pass

    def Enable(self):
        """"""
        pass

    def EnableTop(self):
        """"""
        pass

    def FindItemById(self):
        """"""
        pass

    def FindMenu(self):
        """"""
        pass

    def FindMenuItem(self):
        """"""
        pass

    def GetHelpString(self):
        """"""
        pass

    def GetLabel(self):
        """"""
        pass

    def GetLabelTop(self):
        """"""
        pass

    def GetMenu(self):
        """"""
        pass

    def GetMenuCount(self):
        """"""
        pass

    def Insert(self):
        """"""
        pass

    def IsChecked(self):
        """"""
        pass

    def IsEnabled(self):
        """"""
        pass

    def IsEnabledTop(self):
        """"""
        pass

    def Remove(self):
        """"""
        pass

    def Replace(self):
        """"""
        pass

    def SetHelpString(self):
        """"""
        pass

    def SetLabel(self):
        """"""
        pass

    def SetLabelTop(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass

class MenuItem(Object):
    """"""

    def Check(self):
        """"""
        pass

    def Enable(self):
        """"""
        pass

    def GetAccel(self):
        """"""
        pass

    def GetBitmap(self):
        """"""
        pass

    def GetHelp(self):
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

    def GetMenu(self):
        """"""
        pass

    def GetSubMenu(self):
        """"""
        pass

    def GetText(self):
        """"""
        pass

    def IsCheckable(self):
        """"""
        pass

    def IsChecked(self):
        """"""
        pass

    def IsEnabled(self):
        """"""
        pass

    def IsSeparator(self):
        """"""
        pass

    def IsSubMenu(self):
        """"""
        pass

    def SetAccel(self):
        """"""
        pass

    def SetBitmap(self):
        """"""
        pass

    def SetCheckable(self):
        """"""
        pass

    def SetHelp(self):
        """"""
        pass

    def SetId(self):
        """"""
        pass

    def SetSubMenu(self):
        """"""
        pass

    def SetText(self):
        """"""
        pass

    def Toggle(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass

