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
from Window import Window

try:
    True
except NameError:
    True = 1==1
    False = 1==0


class Control(Window):
    """Base class for a control or 'widget'.

    A control is generally a small window which processes user input
    and/or displays one or more item of data."""

    def __init__(self, parent, id, pos=wx.DefaultPosition,
                 size=wx.DefaultSize, style=0,
                 validator=wx.DefaultValidator, name='control'):
        """Create a Control instance."""
        pass

    def Command(self, event):
        """Simulates the effect of the user issuing a command to the
        item.  See CommandEvent."""
        pass

    def Create(self, parent, id, pos=wx.DefaultPosition,
               size=wx.DefaultSize, style=0,
               validator=wx.DefaultValidator, name='control'):
        """Create a Control instance."""
        pass

    def GetLabel(self):
        """Return the string label for the control."""
        pass

    def SetLabel(self, label):
        """Set the string label for the control."""
        pass


class PyControl(Control):
    """"""

    def __init__(self):
        """"""
        pass

    def _setCallbackInfo(self):
        """"""
        pass

    def base_AcceptsFocus(self):
        """"""
        pass

    def base_AcceptsFocusFromKeyboard(self):
        """"""
        pass

    def base_AddChild(self):
        """"""
        pass

    def base_DoGetBestSize(self):
        """"""
        pass

    def base_DoGetClientSize(self):
        """"""
        pass

    def base_DoGetPosition(self):
        """"""
        pass

    def base_DoGetSize(self):
        """"""
        pass

    def base_DoGetVirtualSize(self):
        """"""
        pass

    def base_DoMoveWindow(self):
        """"""
        pass

    def base_DoSetClientSize(self):
        """"""
        pass

    def base_DoSetSize(self):
        """"""
        pass

    def base_DoSetVirtualSize(self):
        """"""
        pass

    def base_GetMaxSize(self):
        """"""
        pass

    def base_InitDialog(self):
        """"""
        pass

    def base_RemoveChild(self):
        """"""
        pass

    def base_TransferDataFromWindow(self):
        """"""
        pass

    def base_TransferDataToWindow(self):
        """"""
        pass

    def base_Validate(self):
        """"""
        pass


class ControlWithItems(Control):
    """"""

    def Append(self):
        """"""
        pass

    def AppendItems(self):
        """"""
        pass

    def Delete(self):
        """"""
        pass

    def FindString(self):
        """"""
        pass

    def GetClientData(self):
        """"""
        pass

    def GetCount(self):
        """"""
        pass

    def GetSelection(self):
        """"""
        pass

    def GetString(self):
        """"""
        pass

    def GetStringSelection(self):
        """"""
        pass

    def Number(self):
        """"""
        pass

    def Select(self):
        """"""
        pass

    def SetClientData(self):
        """"""
        pass

    def SetString(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


class Button(Control):
    """A button is a control that contains a text string, and is one
    of the most common elements of a GUI.  It may be placed on a
    dialog box or panel, or indeed almost any other window.

    Styles
    ------

    BU_LEFT: Left-justifies the label.  WIN32 only.

    BU_TOP: Aligns the label to the top of the button.  WIN32 only.

    BU_RIGHT: Right-justifies the bitmap label.  WIN32 only.

    BU_BOTTOM: Aligns the label to the bottom of the button.  WIN32
    only.

    BU_EXACTFIT: Creates the button as small as possible instead of
    making it of the standard size (which is the default behaviour.)

    Events
    ------
    
    EVT_BUTTON(win,id,func): Sent when the button is clicked."""

    def __init__(self, parent, id, label, pos=wx.DefaultPosition, 
                 size=wx.DefaultSize, style=0,
                 validator=wx.DefaultValidator, name='button'):
        """Create and show a button.

        parent:    Parent window.  Must not be None.
        id:        Button identifier.  A value of -1 indicates a default value.
        label:     The text to be displayed on the button.
        pos:       The button position on it's parent.
        size:      Button size.  If the default size (-1, -1) is specified
                   then the button is sized appropriately for the text.
        style:     Window style.  See Button.
        validator: Window validator.
        name:      Window name."""
        pass

    def Create(self, parent, id, label, pos=wx.DefaultPosition, 
               size=wx.DefaultSize, style=0,
               validator=wx.DefaultValidator, name='button'):
        """Create and show a button."""
        pass

    def SetBackgroundColour(self):
        """"""
        pass

    def SetDefault(self):
        """Set the button to be the default item for the panel or
        dialog box.

        Under Windows, only dialog box buttons respond to this
        function. As normal under Windows and Motif, pressing return
        causes the default button to be depressed when the return key
        is pressed. See also Window.SetFocus which sets the keyboard
        focus for windows and text panel items, and
        Panel.SetDefaultItem."""
        pass

    def SetForegroundColour(self):
        """"""
        pass


class BitmapButton(Button):
    """"""

    def Create(self):
        """"""
        pass

    def GetBitmapDisabled(self):
        """"""
        pass

    def GetBitmapFocus(self):
        """"""
        pass

    def GetBitmapLabel(self):
        """"""
        pass

    def GetBitmapSelected(self):
        """"""
        pass

    def GetMarginX(self):
        """"""
        pass

    def GetMarginY(self):
        """"""
        pass

    def SetBitmapDisabled(self):
        """"""
        pass

    def SetBitmapFocus(self):
        """"""
        pass

    def SetBitmapLabel(self):
        """"""
        pass

    def SetBitmapSelected(self):
        """"""
        pass

    def SetMargins(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


class CheckBox(Control):
    """"""

    def __init__(self):
        """"""
        pass

    def Create(self):
        """"""
        pass

    def GetValue(self):
        """"""
        pass

    def IsChecked(self):
        """"""
        pass

    def SetValue(self):
        """"""
        pass


class Choice(ControlWithItems):
    """"""

    def __init__(self):
        """"""
        pass

    def Clear(self):
        """"""
        pass

    def Create(self):
        """"""
        pass

    def GetColumns(self):
        """"""
        pass

    def Select(self):
        """"""
        pass

    def SetColumns(self):
        """"""
        pass

    def SetSelection(self):
        """"""
        pass

    def SetString(self):
        """"""
        pass

    def SetStringSelection(self):
        """"""
        pass


class Gauge(Control):
    """"""

    def Create(self):
        """"""
        pass

    def GetBezelFace(self):
        """"""
        pass

    def GetRange(self):
        """"""
        pass

    def GetShadowWidth(self):
        """"""
        pass

    def GetValue(self):
        """"""
        pass

    def SetBezelFace(self):
        """"""
        pass

    def SetRange(self):
        """"""
        pass

    def SetShadowWidth(self):
        """"""
        pass

    def SetValue(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


class GenericDirCtrl(Control):
    """"""

    def Create(self):
        """"""
        pass

    def ExpandPath(self):
        """"""
        pass

    def GetDefaultPath(self):
        """"""
        pass

    def GetFilePath(self):
        """"""
        pass

    def GetFilter(self):
        """"""
        pass

    def GetFilterIndex(self):
        """"""
        pass

    def GetFilterListCtrl(self):
        """"""
        pass

    def GetPath(self):
        """"""
        pass

    def GetRootId(self):
        """"""
        pass

    def GetShowHidden(self):
        """"""
        pass

    def GetTreeCtrl(self):
        """"""
        pass

    def SetDefaultPath(self):
        """"""
        pass

    def SetFilter(self):
        """"""
        pass

    def SetFilterIndex(self):
        """"""
        pass

    def SetPath(self):
        """"""
        pass

    def ShowHidden(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


class ListBox(ControlWithItems):
    """"""

    def Clear(self):
        """"""
        pass

    def Create(self):
        """"""
        pass

    def Deselect(self):
        """"""
        pass

    def GetSelections(self):
        """"""
        pass

    def InsertItems(self):
        """"""
        pass

    def IsSelected(self):
        """"""
        pass

    def Selected(self):
        """"""
        pass

    def Set(self):
        """"""
        pass

    def SetFirstItem(self):
        """"""
        pass

    def SetFirstItemStr(self):
        """"""
        pass

    def SetSelection(self):
        """"""
        pass

    def SetString(self):
        """"""
        pass

    def SetStringSelection(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


class CheckListBox(ListBox):
    """"""

    def __init__(self):
        """"""
        pass

    def Check(self):
        """"""
        pass

    def Create(self):
        """"""
        pass

    def GetItemHeight(self):
        """"""
        pass

    def HitTest(self):
        """"""
        pass

    def HitTestXY(self):
        """"""
        pass

    def InsertItems(self):
        """"""
        pass

    def IsChecked(self):
        """"""
        pass


class ListCtrl(Control):
    """"""

    def Append(self):
        """"""
        pass

    def Arrange(self):
        """"""
        pass

    def AssignImageList(self):
        """"""
        pass

    def ClearAll(self):
        """"""
        pass

    def ClearColumnImage(self):
        """"""
        pass

    def Create(self):
        """"""
        pass

    def DeleteAllColumns(self):
        """"""
        pass

    def DeleteAllItems(self):
        """"""
        pass

    def DeleteColumn(self):
        """"""
        pass

    def DeleteItem(self):
        """"""
        pass

    def EnsureVisible(self):
        """"""
        pass

    def FindItem(self):
        """"""
        pass

    def FindItemAtPos(self):
        """"""
        pass

    def FindItemData(self):
        """"""
        pass

    def Focus(self):
        """"""
        pass

    def GetColumn(self):
        """"""
        pass

    def GetColumnCount(self):
        """"""
        pass

    def GetColumnWidth(self):
        """"""
        pass

    def GetCountPerPage(self):
        """"""
        pass

    def GetFirstSelected(self):
        """"""
        pass

    def GetFocusedItem(self):
        """"""
        pass

    def GetImageList(self):
        """"""
        pass

    def GetItem(self):
        """"""
        pass

    def GetItemBackgroundColour(self):
        """"""
        pass

    def GetItemCount(self):
        """"""
        pass

    def GetItemData(self):
        """"""
        pass

    def GetItemPosition(self):
        """"""
        pass

    def GetItemRect(self):
        """"""
        pass

    def GetItemSpacing(self):
        """"""
        pass

    def GetItemState(self):
        """"""
        pass

    def GetItemText(self):
        """"""
        pass

    def GetItemTextColour(self):
        """"""
        pass

    def GetMainWindow(self):
        """"""
        pass

    def GetNextItem(self):
        """"""
        pass

    def GetNextSelected(self):
        """"""
        pass

    def GetSelectedItemCount(self):
        """"""
        pass

    def GetTextColour(self):
        """"""
        pass

    def GetTopItem(self):
        """"""
        pass

    def HitTest(self):
        """"""
        pass

    def InsertColumn(self):
        """"""
        pass

    def InsertColumnInfo(self):
        """"""
        pass

    def InsertImageItem(self):
        """"""
        pass

    def InsertImageStringItem(self):
        """"""
        pass

    def InsertItem(self):
        """"""
        pass

    def InsertStringItem(self):
        """"""
        pass

    def IsSelected(self):
        """"""
        pass

    def IsVirtual(self):
        """"""
        pass

    def RefreshItem(self):
        """"""
        pass

    def RefreshItems(self):
        """"""
        pass

    def ScrollList(self):
        """"""
        pass

    def Select(self):
        """"""
        pass

    def SetBackgroundColour(self):
        """"""
        pass

    def SetColumn(self):
        """"""
        pass

    def SetColumnImage(self):
        """"""
        pass

    def SetColumnWidth(self):
        """"""
        pass

    def SetForegroundColour(self):
        """"""
        pass

    def SetImageList(self):
        """"""
        pass

    def SetItem(self):
        """"""
        pass

    def SetItemBackgroundColour(self):
        """"""
        pass

    def SetItemCount(self):
        """"""
        pass

    def SetItemData(self):
        """"""
        pass

    def SetItemImage(self):
        """"""
        pass

    def SetItemPosition(self):
        """"""
        pass

    def SetItemState(self):
        """"""
        pass

    def SetItemText(self):
        """"""
        pass

    def SetItemTextColour(self):
        """"""
        pass

    def SetSingleStyle(self):
        """"""
        pass

    def SetStringItem(self):
        """"""
        pass

    def SetTextColour(self):
        """"""
        pass

    def SetWindowStyleFlag(self):
        """"""
        pass

    def SortItems(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass

    def _setCallbackInfo(self):
        """"""
        pass


class ListItem(Object):
    """"""

    def Clear(self):
        """"""
        pass

    def ClearAttributes(self):
        """"""
        pass

    def GetAlign(self):
        """"""
        pass

    def GetAttributes(self):
        """"""
        pass

    def GetBackgroundColour(self):
        """"""
        pass

    def GetColumn(self):
        """"""
        pass

    def GetData(self):
        """"""
        pass

    def GetFont(self):
        """"""
        pass

    def GetId(self):
        """"""
        pass

    def GetImage(self):
        """"""
        pass

    def GetMask(self):
        """"""
        pass

    def GetState(self):
        """"""
        pass

    def GetText(self):
        """"""
        pass

    def GetTextColour(self):
        """"""
        pass

    def GetWidth(self):
        """"""
        pass

    def HasAttributes(self):
        """"""
        pass

    def SetAlign(self):
        """"""
        pass

    def SetBackgroundColour(self):
        """"""
        pass

    def SetColumn(self):
        """"""
        pass

    def SetData(self):
        """"""
        pass

    def SetFont(self):
        """"""
        pass

    def SetId(self):
        """"""
        pass

    def SetImage(self):
        """"""
        pass

    def SetMask(self):
        """"""
        pass

    def SetState(self):
        """"""
        pass

    def SetStateMask(self):
        """"""
        pass

    def SetText(self):
        """"""
        pass

    def SetTextColour(self):
        """"""
        pass

    def SetWidth(self):
        """"""
        pass

    def __del__(self):
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


class ListItemAttr:
    """"""

    def GetBackgroundColour(self):
        """"""
        pass

    def GetFont(self):
        """"""
        pass

    def GetTextColour(self):
        """"""
        pass

    def HasBackgroundColour(self):
        """"""
        pass

    def HasFont(self):
        """"""
        pass

    def HasTextColour(self):
        """"""
        pass

    def SetBackgroundColour(self):
        """"""
        pass

    def SetFont(self):
        """"""
        pass

    def SetTextColour(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


class ListView(ListCtrl):
    """"""

    def ClearColumnImage(self):
        """"""
        pass

    def Create(self):
        """"""
        pass

    def Focus(self):
        """"""
        pass

    def GetFirstSelected(self):
        """"""
        pass

    def GetFocusedItem(self):
        """"""
        pass

    def GetNextSelected(self):
        """"""
        pass

    def IsSelected(self):
        """"""
        pass

    def Select(self):
        """"""
        pass

    def SetColumnImage(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


class Notebook(Control):

    def __init__(self, parent, id, pos=wx.DefaultPosition,
                 size=wx.DefaultSize, style=0, name=wx.PyNOTEBOOK_NAME):
        """"""
        pass

    def AddPage(self, page, text, select=False, imageId=-1):
        """"""
        pass

    def AdvanceSelection(self, forward=True):
        """"""
        pass

    def AssignImageList(self, imageList) :
        """"""
        pass

    def Create(self, parent, id, pos=wx.DefaultPosition,
               size=wx.DefaultSize, style=0, name=wx.PyNOTEBOOK_NAME):
        """"""
        pass

    def DeleteAllPages(self):
        """"""
        pass

    def DeletePage(self, page):
        """"""
        pass

    def GetImageList(self):
        """"""
        pass

    def GetPage(self, page):
        """"""
        pass

    def GetPageCount(self):
        """"""
        pass

    def GetPageImage(self, page):
        """"""
        pass

    def GetPageText(self, page):
        """"""
        pass

    def GetRowCount(self):
        """"""
        pass

    def GetSelection(self):
        """"""
        pass

    def InsertPage(self, index, page, text, select=False, imageId=-1):
        """"""
        pass

    def RemovePage(self, page):
        """"""
        pass

    def ResizeChildren(self):
        """"""
        pass

    def SetImageList(self, imageList):
        """"""
        pass

    def SetPadding(self, padding):
        """"""
        pass

    def SetPageImage(self, page, image):
        """"""
        pass

    def SetPageSize(self, size):
        """"""
        pass

    def SetPageText(self, page, text):
        """"""
        pass

    def SetSelection(self, page):
        """"""
        pass


class RadioBox(Control):
    """"""

    def Create(self):
        """"""
        pass

    def Enable(self):
        """"""
        pass

    def EnableItem(self):
        """"""
        pass

    def FindString(self):
        """"""
        pass

    def GetCount(self):
        """"""
        pass

    def GetItemLabel(self):
        """"""
        pass

    def GetSelection(self):
        """"""
        pass

    def GetString(self):
        """"""
        pass

    def GetStringSelection(self):
        """"""
        pass

    def Number(self):
        """"""
        pass

    def SetItemLabel(self):
        """"""
        pass

    def SetSelection(self):
        """"""
        pass

    def SetStringSelection(self):
        """"""
        pass

    def Show(self):
        """"""
        pass

    def ShowItem(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


class RadioButton(Control):
    """"""

    def Create(self):
        """"""
        pass

    def GetValue(self):
        """"""
        pass

    def SetValue(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


class ScrollBar(Control):
    """"""

    def Create(self):
        """"""
        pass

    def GetPageSize(self):
        """"""
        pass

    def GetRange(self):
        """"""
        pass

    def GetThumbLength(self):
        """"""
        pass

    def GetThumbPosition(self):
        """"""
        pass

    def GetThumbSize(self):
        """"""
        pass

    def IsVertical(self):
        """"""
        pass

    def SetScrollbar(self):
        """"""
        pass

    def SetThumbPosition(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


class Slider(Control):
    """"""

    def ClearSel(self):
        """"""
        pass

    def ClearTicks(self):
        """"""
        pass

    def Create(self):
        """"""
        pass

    def GetLineSize(self):
        """"""
        pass

    def GetMax(self):
        """"""
        pass

    def GetMin(self):
        """"""
        pass

    def GetPageSize(self):
        """"""
        pass

    def GetSelEnd(self):
        """"""
        pass

    def GetSelStart(self):
        """"""
        pass

    def GetThumbLength(self):
        """"""
        pass

    def GetTickFreq(self):
        """"""
        pass

    def GetValue(self):
        """"""
        pass

    def SetLineSize(self):
        """"""
        pass

    def SetPageSize(self):
        """"""
        pass

    def SetRange(self):
        """"""
        pass

    def SetSelection(self):
        """"""
        pass

    def SetThumbLength(self):
        """"""
        pass

    def SetTick(self):
        """"""
        pass

    def SetTickFreq(self):
        """"""
        pass

    def SetValue(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


class SpinButton(Control):
    """"""

    def Create(self):
        """"""
        pass

    def GetMax(self):
        """"""
        pass

    def GetMin(self):
        """"""
        pass

    def GetValue(self):
        """"""
        pass

    def SetRange(self):
        """"""
        pass

    def SetValue(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


class SpinCtrl(SpinButton):
    """"""

    def Create(self):
        """"""
        pass

    def GetMax(self):
        """"""
        pass

    def GetMin(self):
        """"""
        pass

    def GetValue(self):
        """"""
        pass

    def SetRange(self):
        """"""
        pass

    def SetValue(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


class StaticBitmap(Control):
    """"""

    def Create(self):
        """"""
        pass

    def GetBitmap(self):
        """"""
        pass

    def SetBitmap(self):
        """"""
        pass

    def SetIcon(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


class StaticBox(Control):
    """"""

    def Create(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


class StaticLine(Control):
    """"""

    def Create(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


class StaticText(Control):
    """"""

    def Create(self):
        """"""
        pass

    def GetLabel(self):
        """"""
        pass

    def SetLabel(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


class TextAttr:
    """"""

    def GetBackgroundColour(self):
        """"""
        pass

    def GetFont(self):
        """"""
        pass

    def GetTextColour(self):
        """"""
        pass

    def HasBackgroundColour(self):
        """"""
        pass

    def HasFont(self):
        """"""
        pass

    def HasTextColour(self):
        """"""
        pass

    def IsDefault(self):
        """"""
        pass

    def SetBackgroundColour(self):
        """"""
        pass

    def SetFont(self):
        """"""
        pass

    def SetTextColour(self):
        """"""
        pass

    def __del__(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


class TextCtrl(Control):
    """"""

    def AppendText(self):
        """"""
        pass

    def CanCopy(self):
        """"""
        pass

    def CanCut(self):
        """"""
        pass

    def CanPaste(self):
        """"""
        pass

    def CanRedo(self):
        """"""
        pass

    def CanUndo(self):
        """"""
        pass

    def Clear(self):
        """"""
        pass

    def Copy(self):
        """"""
        pass

    def Create(self):
        """"""
        pass

    def Cut(self):
        """"""
        pass

    def DiscardEdits(self):
        """"""
        pass

    def EmulateKeyPress(self):
        """"""
        pass

    def GetDefaultStyle(self):
        """"""
        pass

    def GetInsertionPoint(self):
        """"""
        pass

    def GetLastPosition(self):
        """"""
        pass

    def GetLineLength(self):
        """"""
        pass

    def GetLineText(self):
        """"""
        pass

    def GetNumberOfLines(self):
        """"""
        pass

    def GetRange(self):
        """"""
        pass

    def GetSelection(self):
        """"""
        pass

    def GetString(self):
        """"""
        pass

    def GetStringSelection(self):
        """"""
        pass

    def GetValue(self):
        """"""
        pass

    def IsEditable(self):
        """"""
        pass

    def IsModified(self):
        """"""
        pass

    def IsMultiLine(self):
        """"""
        pass

    def IsSingleLine(self):
        """"""
        pass

    def LoadFile(self):
        """"""
        pass

    def Paste(self):
        """"""
        pass

    def PositionToXY(self):
        """"""
        pass

    def Redo(self):
        """"""
        pass

    def Remove(self):
        """"""
        pass

    def Replace(self):
        """"""
        pass

    def SaveFile(self):
        """"""
        pass

    def SelectAll(self):
        """"""
        pass

    def SetDefaultStyle(self):
        """"""
        pass

    def SetEditable(self):
        """"""
        pass

    def SetInsertionPoint(self):
        """"""
        pass

    def SetInsertionPointEnd(self):
        """"""
        pass

    def SetMaxLength(self):
        """"""
        pass

    def SetSelection(self):
        """"""
        pass

    def SetStyle(self):
        """"""
        pass

    def SetValue(self):
        """"""
        pass

    def ShowPosition(self):
        """"""
        pass

    def Undo(self):
        """"""
        pass

    def WriteText(self):
        """"""
        pass

    def XYToPosition(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass

    def write(self):
        """"""
        pass


class ToggleButton(Control):
    """"""

    def Create(self):
        """"""
        pass

    def GetValue(self):
        """"""
        pass

    def SetLabel(self):
        """"""
        pass

    def SetValue(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


