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
from wxFrames import wxFrame
import wxParameters as wx
from wxWindow import wxTopLevelWindow


class wxDialog(wxTopLevelWindow):
    """"""

    def Centre(self):
        """"""
        pass

    def Create(self):
        """"""
        pass

    def CreateButtonSizer(self):
        """"""
        pass

    def CreateTextSizer(self):
        """"""
        pass

    def EndModal(self):
        """"""
        pass

    def GetReturnCode(self):
        """"""
        pass

    def IsModal(self):
        """"""
        pass

    def SetModal(self):
        """"""
        pass

    def SetReturnCode(self):
        """"""
        pass

    def ShowModal(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


class wxColourDialog(wxDialog):
    """"""

    def GetColourData(self):
        """"""
        pass

    def ShowModal(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


class wxColourData(wxObject):
    """"""

    def GetChooseFull(self):
        """"""
        pass

    def GetColour(self):
        """"""
        pass

    def GetCustomColour(self):
        """"""
        pass

    def SetChooseFull(self):
        """"""
        pass

    def SetColour(self):
        """"""
        pass

    def SetCustomColour(self):
        """"""
        pass

    def __del__(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


class wxColourDatabase(wxObject):
    """"""

    def Append(self):
        """"""
        pass

    def FindColour(self):
        """"""
        pass

    def FindName(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


class wxDirDialog(wxDialog):
    """"""

    def GetMessage(self):
        """"""
        pass

    def GetPath(self):
        """"""
        pass

    def GetStyle(self):
        """"""
        pass

    def SetMessage(self):
        """"""
        pass

    def SetPath(self):
        """"""
        pass

    def ShowModal(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


class wxFileDialog(wxDialog):
    """"""

    def GetDirectory(self):
        """"""
        pass

    def GetFilename(self):
        """"""
        pass

    def GetFilenames(self):
        """"""
        pass

    def GetFilterIndex(self):
        """"""
        pass

    def GetMessage(self):
        """"""
        pass

    def GetPath(self):
        """"""
        pass

    def GetPaths(self):
        """"""
        pass

    def GetStyle(self):
        """"""
        pass

    def GetWildcard(self):
        """"""
        pass

    def SetDirectory(self):
        """"""
        pass

    def SetFilename(self):
        """"""
        pass

    def SetFilterIndex(self):
        """"""
        pass

    def SetMessage(self):
        """"""
        pass

    def SetPath(self):
        """"""
        pass

    def SetStyle(self):
        """"""
        pass

    def SetWildcard(self):
        """"""
        pass

    def ShowModal(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


class wxFindReplaceDialog(wxDialog):
    """"""

    def Create(self):
        """"""
        pass

    def GetData(self):
        """"""
        pass

    def SetData(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


class wxFindReplaceData(wxObject):
    """"""

    def GetFindString(self):
        """"""
        pass

    def GetFlags(self):
        """"""
        pass

    def GetReplaceString(self):
        """"""
        pass

    def SetFindString(self):
        """"""
        pass

    def SetFlags(self):
        """"""
        pass

    def SetReplaceString(self):
        """"""
        pass

    def __del__(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


class wxFontDialog(wxDialog):
    """"""

    def GetFontData(self):
        """"""
        pass

    def ShowModal(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


class wxFontData(wxObject):
    """"""

    def EnableEffects(self):
        """"""
        pass

    def GetAllowSymbols(self):
        """"""
        pass

    def GetChosenFont(self):
        """"""
        pass

    def GetColour(self):
        """"""
        pass

    def GetEnableEffects(self):
        """"""
        pass

    def GetInitialFont(self):
        """"""
        pass

    def GetShowHelp(self):
        """"""
        pass

    def SetAllowSymbols(self):
        """"""
        pass

    def SetChosenFont(self):
        """"""
        pass

    def SetColour(self):
        """"""
        pass

    def SetInitialFont(self):
        """"""
        pass

    def SetRange(self):
        """"""
        pass

    def SetShowHelp(self):
        """"""
        pass

    def __del__(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


class wxMessageDialog(wxDialog):
    """"""

    def ShowModal(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass



class wxMultiChoiceDialog(wxDialog):
    """"""

    def GetSelections(self):
        """"""
        pass

    def SetSelections(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


class wxProgressDialog(wxFrame):
    """"""

    def Resume(self):
        """"""
        pass

    def Update(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


class wxSingleChoiceDialog(wxDialog):
    """"""

    def GetSelection(self):
        """"""
        pass

    def GetStringSelection(self):
        """"""
        pass

    def SetSelection(self):
        """"""
        pass

    def ShowModal(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


class wxTextEntryDialog(wxDialog):
    """"""

    def GetValue(self):
        """"""
        pass

    def SetValue(self):
        """"""
        pass

    def ShowModal(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass
