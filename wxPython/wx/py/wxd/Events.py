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


class Event(Object):
    """"""

    def __init__(self):
        """"""
        pass

    def Clone(self):
        """"""
        pass

    def GetEventObject(self):
        """"""
        pass

    def GetEventType(self):
        """"""
        pass

    def GetId(self):
        """"""
        pass

    def GetSkipped(self):
        """"""
        pass

    def GetTimestamp(self):
        """"""
        pass

    def SetEventObject(self):
        """"""
        pass

    def SetEventType(self):
        """"""
        pass

    def SetId(self):
        """"""
        pass

    def SetTimestamp(self):
        """"""
        pass

    def Skip(self):
        """"""
        pass


class PyEvent(Event):
    """"""

    def GetSelf(self):
        """"""
        pass

    def SetSelf(self):
        """"""
        pass

    def __del__(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


class ActivateEvent(Event):
    """"""

    def GetActive(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


class CalculateLayoutEvent(Event):
    """"""

    def GetFlags(self):
        """"""
        pass

    def GetRect(self):
        """"""
        pass

    def SetFlags(self):
        """"""
        pass

    def SetRect(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


class CloseEvent(Event):
    """"""

    def CanVeto(self):
        """"""
        pass

    def GetLoggingOff(self):
        """"""
        pass

    def GetVeto(self):
        """"""
        pass

    def SetCanVeto(self):
        """"""
        pass

    def SetLoggingOff(self):
        """"""
        pass

    def Veto(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


class CommandEvent(Event):
    """"""

    def __init__(self):
        """"""
        pass

    def Checked(self):
        """"""
        pass

    def GetClientData(self):
        """"""
        pass

    def GetExtraLong(self):
        """"""
        pass

    def GetInt(self):
        """"""
        pass

    def GetSelection(self):
        """"""
        pass

    def GetString(self):
        """"""
        pass

    def IsChecked(self):
        """"""
        pass

    def IsSelection(self):
        """"""
        pass

    def SetExtraLong(self):
        """"""
        pass

    def SetInt(self):
        """"""
        pass

    def SetString(self):
        """"""
        pass


class ChildFocusEvent(CommandEvent):
    """"""

    def GetWindow(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


class ContextMenuEvent(CommandEvent):
    """"""

    def GetPosition(self):
        """"""
        pass

    def SetPosition(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


class DisplayChangedEvent(Event):
    """"""

    def __init__(self):
        """"""
        pass


class DropFilesEvent(Event):
    """"""

    def GetFiles(self):
        """"""
        pass

    def GetNumberOfFiles(self):
        """"""
        pass

    def GetPosition(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


class EraseEvent(Event):
    """"""

    def GetDC(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


class FindDialogEvent(CommandEvent):
    """"""

    def GetDialog(self):
        """"""
        pass

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

    def __init__(self):
        """"""
        pass


class FocusEvent(Event):
    """"""

    def __init__(self):
        """"""
        pass


class IconizeEvent(Event):
    """"""

    def Iconized(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


class IdleEvent(Event):
    """"""

    def MoreRequested(self):
        """"""
        pass

    def RequestMore(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


class InitDialogEvent(Event):
    """"""

    def __init__(self):
        """"""
        pass


class JoystickEvent(Event):
    """"""

    def ButtonDown(self):
        """"""
        pass

    def ButtonIsDown(self):
        """"""
        pass

    def ButtonUp(self):
        """"""
        pass

    def GetButtonChange(self):
        """"""
        pass

    def GetButtonState(self):
        """"""
        pass

    def GetJoystick(self):
        """"""
        pass

    def GetPosition(self):
        """"""
        pass

    def GetZPosition(self):
        """"""
        pass

    def IsButton(self):
        """"""
        pass

    def IsMove(self):
        """"""
        pass

    def IsZMove(self):
        """"""
        pass

    def SetButtonChange(self):
        """"""
        pass

    def SetButtonState(self):
        """"""
        pass

    def SetJoystick(self):
        """"""
        pass

    def SetPosition(self):
        """"""
        pass

    def SetZPosition(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


class KeyEvent(Event):
    """"""

    def AltDown(self):
        """"""
        pass

    def ControlDown(self):
        """"""
        pass

    def GetKeyCode(self):
        """"""
        pass

    def GetPosition(self):
        """"""
        pass

    def GetPositionTuple(self):
        """"""
        pass

    def GetRawKeyCode(self):
        """"""
        pass

    def GetRawKeyFlags(self):
        """"""
        pass

    def GetX(self):
        """"""
        pass

    def GetY(self):
        """"""
        pass

    def HasModifiers(self):
        """"""
        pass

    def KeyCode(self):
        """"""
        pass

    def MetaDown(self):
        """"""
        pass

    def ShiftDown(self):
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


class MaximizeEvent(Event):
    """"""

    def __init__(self):
        """"""
        pass


class MenuEvent(Event):
    """"""

    def GetMenuId(self):
        """"""
        pass

    def IsPopup(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


class MouseCaptureChangedEvent(Event):
    """"""

    def GetCapturedWindow(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


class MouseEvent(Event):
    """"""

    def AltDown(self):
        """"""
        pass

    def Button(self):
        """"""
        pass

    def ButtonDClick(self):
        """"""
        pass

    def ButtonDown(self):
        """"""
        pass

    def ButtonIsDown(self):
        """"""
        pass

    def ButtonUp(self):
        """"""
        pass

    def ControlDown(self):
        """"""
        pass

    def Dragging(self):
        """"""
        pass

    def Entering(self):
        """"""
        pass

    def GetLinesPerAction(self):
        """"""
        pass

    def GetLogicalPosition(self):
        """"""
        pass

    def GetPosition(self):
        """"""
        pass

    def GetPositionTuple(self):
        """"""
        pass

    def GetWheelDelta(self):
        """"""
        pass

    def GetWheelRotation(self):
        """"""
        pass

    def GetX(self):
        """"""
        pass

    def GetY(self):
        """"""
        pass

    def IsButton(self):
        """"""
        pass

    def Leaving(self):
        """"""
        pass

    def LeftDClick(self):
        """"""
        pass

    def LeftDown(self):
        """"""
        pass

    def LeftIsDown(self):
        """"""
        pass

    def LeftUp(self):
        """"""
        pass

    def MetaDown(self):
        """"""
        pass

    def MiddleDClick(self):
        """"""
        pass

    def MiddleDown(self):
        """"""
        pass

    def MiddleIsDown(self):
        """"""
        pass

    def MiddleUp(self):
        """"""
        pass

    def Moving(self):
        """"""
        pass

    def RightDClick(self):
        """"""
        pass

    def RightDown(self):
        """"""
        pass

    def RightIsDown(self):
        """"""
        pass

    def RightUp(self):
        """"""
        pass

    def ShiftDown(self):
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


class MoveEvent(Event):
    """"""

    def GetPosition(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


class NavigationKeyEvent(Event):
    """"""

    def GetCurrentFocus(self):
        """"""
        pass

    def GetDirection(self):
        """"""
        pass

    def IsWindowChange(self):
        """"""
        pass

    def SetCurrentFocus(self):
        """"""
        pass

    def SetDirection(self):
        """"""
        pass

    def SetWindowChange(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


class NotifyEvent(CommandEvent):
    """"""

    def __init__(self):
        """"""
        pass

    def Allow(self):
        """"""
        pass

    def IsAllowed(self):
        """"""
        pass

    def Veto(self):
        """"""
        pass


class ListEvent(NotifyEvent):
    """"""

    def GetCacheFrom(self):
        """"""
        pass

    def GetCacheTo(self):
        """"""
        pass

    def GetCode(self):
        """"""
        pass

    def GetColumn(self):
        """"""
        pass

    def GetData(self):
        """"""
        pass

    def GetImage(self):
        """"""
        pass

    def GetIndex(self):
        """"""
        pass

    def GetItem(self):
        """"""
        pass

    def GetKeyCode(self):
        """"""
        pass

    def GetLabel(self):
        """"""
        pass

    def GetMask(self):
        """"""
        pass

    def GetPoint(self):
        """"""
        pass

    def GetText(self):
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


class NotebookEvent(NotifyEvent):

    def __init__(self, commandType=wx.EVT_NULL, id=0, nSel=-1, nOldSel=-1):
        """"""
        pass

    def GetSelection(self):
        """"""
        pass
    
    def GetOldSelection(self):
        """"""
        pass
    
    def SetOldSelection(self, page):
        """"""
        pass

    def SetSelection(self, page):
        """"""
        pass


class PaintEvent(Event):
    """"""

    def __init__(self):
        """"""
        pass


class PaletteChangedEvent(Event):
    """"""

    def GetChangedWindow(self):
        """"""
        pass

    def SetChangedWindow(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


class ProcessEvent(Event):
    """"""

    def GetExitCode(self):
        """"""
        pass

    def GetPid(self):
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


class PyCommandEvent(CommandEvent):
    """"""

    def GetSelf(self):
        """"""
        pass

    def SetSelf(self):
        """"""
        pass

    def __del__(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


class QueryLayoutInfoEvent(Event):
    """"""

    def GetAlignment(self):
        """"""
        pass

    def GetFlags(self):
        """"""
        pass

    def GetOrientation(self):
        """"""
        pass

    def GetRequestedLength(self):
        """"""
        pass

    def GetSize(self):
        """"""
        pass

    def SetAlignment(self):
        """"""
        pass

    def SetFlags(self):
        """"""
        pass

    def SetOrientation(self):
        """"""
        pass

    def SetRequestedLength(self):
        """"""
        pass

    def SetSize(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


class QueryNewPaletteEvent(Event):
    """"""

    def GetPaletteRealized(self):
        """"""
        pass

    def SetPaletteRealized(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


class SashEvent(CommandEvent):
    """"""

    def GetDragRect(self):
        """"""
        pass

    def GetDragStatus(self):
        """"""
        pass

    def GetEdge(self):
        """"""
        pass

    def SetDragRect(self):
        """"""
        pass

    def SetDragStatus(self):
        """"""
        pass

    def SetEdge(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


class ScrollEvent(CommandEvent):
    """"""

    def GetOrientation(self):
        """"""
        pass

    def GetPosition(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


class ScrollWinEvent(Event):
    """"""

    def GetOrientation(self):
        """"""
        pass

    def GetPosition(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


class SetCursorEvent(Event):
    """"""

    def GetCursor(self):
        """"""
        pass

    def GetX(self):
        """"""
        pass

    def GetY(self):
        """"""
        pass

    def HasCursor(self):
        """"""
        pass

    def SetCursor(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


class ShowEvent(Event):
    """"""

    def GetShow(self):
        """"""
        pass

    def SetShow(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


class SizeEvent(Event):
    """"""

    def GetSize(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


class SpinEvent(ScrollEvent):
    """"""

    def __init__(self):
        """"""
        pass


class SplitterEvent(NotifyEvent):
    """"""

    def GetSashPosition(self):
        """"""
        pass

    def GetWindowBeingRemoved(self):
        """"""
        pass

    def GetX(self):
        """"""
        pass

    def GetY(self):
        """"""
        pass

    def SetSashPosition(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


class SysColourChangedEvent(Event):
    """"""

    def __init__(self):
        """"""
        pass


class TextUrlEvent(CommandEvent):
    """"""

    def GetMouseEvent(self):
        """"""
        pass

    def GetURLEnd(self):
        """"""
        pass

    def GetURLStart(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


class TimerEvent(Event):
    """"""

    def GetInterval(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


class TreeEvent(NotifyEvent):
    """"""

    def GetCode(self):
        """"""
        pass

    def GetItem(self):
        """"""
        pass

    def GetKeyCode(self):
        """"""
        pass

    def GetKeyEvent(self):
        """"""
        pass

    def GetLabel(self):
        """"""
        pass

    def GetOldItem(self):
        """"""
        pass

    def GetPoint(self):
        """"""
        pass

    def IsEditCancelled(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


class UpdateUIEvent(Event):
    """"""

    def Check(self):
        """"""
        pass

    def Enable(self):
        """"""
        pass

    def GetChecked(self):
        """"""
        pass

    def GetEnabled(self):
        """"""
        pass

    def GetSetChecked(self):
        """"""
        pass

    def GetSetEnabled(self):
        """"""
        pass

    def GetSetText(self):
        """"""
        pass

    def GetText(self):
        """"""
        pass

    def SetText(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


class WindowCreateEvent(CommandEvent):
    """"""

    def GetWindow(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


class WindowDestroyEvent(CommandEvent):
    """"""

    def GetWindow(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass
