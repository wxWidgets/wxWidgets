# This file was created automatically by SWIG.
# Don't modify this file, modify the SWIG interface instead.

import _gizmos

import windows
import core
import controls
wx = core 
wxEVT_DYNAMIC_SASH_SPLIT = _gizmos.wxEVT_DYNAMIC_SASH_SPLIT
wxEVT_DYNAMIC_SASH_UNIFY = _gizmos.wxEVT_DYNAMIC_SASH_UNIFY
DS_MANAGE_SCROLLBARS = _gizmos.DS_MANAGE_SCROLLBARS
DS_DRAG_CORNER = _gizmos.DS_DRAG_CORNER
class DynamicSashSplitEvent(core.CommandEvent):
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxDynamicSashSplitEvent instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(Object target) -> DynamicSashSplitEvent"""
        newobj = _gizmos.new_DynamicSashSplitEvent(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown

class DynamicSashSplitEventPtr(DynamicSashSplitEvent):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = DynamicSashSplitEvent
_gizmos.DynamicSashSplitEvent_swigregister(DynamicSashSplitEventPtr)

class DynamicSashUnifyEvent(core.CommandEvent):
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxDynamicSashUnifyEvent instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(Object target) -> DynamicSashUnifyEvent"""
        newobj = _gizmos.new_DynamicSashUnifyEvent(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown

class DynamicSashUnifyEventPtr(DynamicSashUnifyEvent):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = DynamicSashUnifyEvent
_gizmos.DynamicSashUnifyEvent_swigregister(DynamicSashUnifyEventPtr)

class DynamicSashWindow(core.Window):
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxDynamicSashWindow instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """
        __init__(Window parent, int id, Point pos=DefaultPosition, Size size=DefaultSize, 
            long style=wxCLIP_CHILDREN|wxDS_MANAGE_SCROLLBARS|wxDS_DRAG_CORNER, 
            wxString name=wxPyDynamicSashNameStr) -> DynamicSashWindow
        """
        newobj = _gizmos.new_DynamicSashWindow(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)

    def Create(*args, **kwargs):
        """
        Create(Window parent, int id, Point pos=DefaultPosition, Size size=DefaultSize, 
            long style=wxCLIP_CHILDREN|wxDS_MANAGE_SCROLLBARS|wxDS_DRAG_CORNER, 
            wxString name=wxPyDynamicSashNameStr) -> bool
        """
        return _gizmos.DynamicSashWindow_Create(*args, **kwargs)

    def GetHScrollBar(*args, **kwargs):
        """GetHScrollBar(Window child) -> ScrollBar"""
        return _gizmos.DynamicSashWindow_GetHScrollBar(*args, **kwargs)

    def GetVScrollBar(*args, **kwargs):
        """GetVScrollBar(Window child) -> ScrollBar"""
        return _gizmos.DynamicSashWindow_GetVScrollBar(*args, **kwargs)


class DynamicSashWindowPtr(DynamicSashWindow):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = DynamicSashWindow
_gizmos.DynamicSashWindow_swigregister(DynamicSashWindowPtr)

def PreDynamicSashWindow(*args, **kwargs):
    """PreDynamicSashWindow() -> DynamicSashWindow"""
    val = _gizmos.new_PreDynamicSashWindow(*args, **kwargs)
    val.thisown = 1
    return val

EVT_DYNAMIC_SASH_SPLIT = wx.PyEventBinder( wxEVT_DYNAMIC_SASH_SPLIT, 1 )
EVT_DYNAMIC_SASH_UNIFY = wx.PyEventBinder( wxEVT_DYNAMIC_SASH_UNIFY, 1 )

EL_ALLOW_NEW = _gizmos.EL_ALLOW_NEW
EL_ALLOW_EDIT = _gizmos.EL_ALLOW_EDIT
EL_ALLOW_DELETE = _gizmos.EL_ALLOW_DELETE
class EditableListBox(windows.Panel):
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxEditableListBox instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """
        __init__(Window parent, int id, wxString label, Point pos=DefaultPosition, 
            Size size=DefaultSize, long style=wxEL_ALLOW_NEW|wxEL_ALLOW_EDIT|wxEL_ALLOW_DELETE, 
            wxString name=wxPyEditableListBoxNameStr) -> EditableListBox
        """
        newobj = _gizmos.new_EditableListBox(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)

    def SetStrings(*args, **kwargs):
        """SetStrings(wxArrayString strings)"""
        return _gizmos.EditableListBox_SetStrings(*args, **kwargs)

    def GetStrings(*args, **kwargs):
        """GetStrings() -> PyObject"""
        return _gizmos.EditableListBox_GetStrings(*args, **kwargs)

    def GetListCtrl(*args, **kwargs):
        """GetListCtrl() -> wxListCtrl"""
        return _gizmos.EditableListBox_GetListCtrl(*args, **kwargs)

    def GetDelButton(*args, **kwargs):
        """GetDelButton() -> BitmapButton"""
        return _gizmos.EditableListBox_GetDelButton(*args, **kwargs)

    def GetNewButton(*args, **kwargs):
        """GetNewButton() -> BitmapButton"""
        return _gizmos.EditableListBox_GetNewButton(*args, **kwargs)

    def GetUpButton(*args, **kwargs):
        """GetUpButton() -> BitmapButton"""
        return _gizmos.EditableListBox_GetUpButton(*args, **kwargs)

    def GetDownButton(*args, **kwargs):
        """GetDownButton() -> BitmapButton"""
        return _gizmos.EditableListBox_GetDownButton(*args, **kwargs)

    def GetEditButton(*args, **kwargs):
        """GetEditButton() -> BitmapButton"""
        return _gizmos.EditableListBox_GetEditButton(*args, **kwargs)


class EditableListBoxPtr(EditableListBox):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = EditableListBox
_gizmos.EditableListBox_swigregister(EditableListBoxPtr)

class RemotelyScrolledTreeCtrl(controls.TreeCtrl):
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxRemotelyScrolledTreeCtrl instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """
        __init__(Window parent, int id, Point pos=DefaultPosition, Size size=DefaultSize, 
            long style=TR_HAS_BUTTONS) -> RemotelyScrolledTreeCtrl
        """
        newobj = _gizmos.new_RemotelyScrolledTreeCtrl(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)

    def HideVScrollbar(*args, **kwargs):
        """HideVScrollbar()"""
        return _gizmos.RemotelyScrolledTreeCtrl_HideVScrollbar(*args, **kwargs)

    def AdjustRemoteScrollbars(*args, **kwargs):
        """AdjustRemoteScrollbars()"""
        return _gizmos.RemotelyScrolledTreeCtrl_AdjustRemoteScrollbars(*args, **kwargs)

    def GetScrolledWindow(*args, **kwargs):
        """GetScrolledWindow() -> ScrolledWindow"""
        return _gizmos.RemotelyScrolledTreeCtrl_GetScrolledWindow(*args, **kwargs)

    def ScrollToLine(*args, **kwargs):
        """ScrollToLine(int posHoriz, int posVert)"""
        return _gizmos.RemotelyScrolledTreeCtrl_ScrollToLine(*args, **kwargs)

    def SetCompanionWindow(*args, **kwargs):
        """SetCompanionWindow(Window companion)"""
        return _gizmos.RemotelyScrolledTreeCtrl_SetCompanionWindow(*args, **kwargs)

    def GetCompanionWindow(*args, **kwargs):
        """GetCompanionWindow() -> Window"""
        return _gizmos.RemotelyScrolledTreeCtrl_GetCompanionWindow(*args, **kwargs)


class RemotelyScrolledTreeCtrlPtr(RemotelyScrolledTreeCtrl):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = RemotelyScrolledTreeCtrl
_gizmos.RemotelyScrolledTreeCtrl_swigregister(RemotelyScrolledTreeCtrlPtr)

class TreeCompanionWindow(core.Window):
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPyTreeCompanionWindow instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """
        __init__(Window parent, int id=-1, Point pos=DefaultPosition, 
            Size size=DefaultSize, long style=0) -> TreeCompanionWindow
        """
        newobj = _gizmos.new_TreeCompanionWindow(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self);self._setCallbackInfo(self, TreeCompanionWindow)

    def _setCallbackInfo(*args, **kwargs):
        """_setCallbackInfo(PyObject self, PyObject _class)"""
        return _gizmos.TreeCompanionWindow__setCallbackInfo(*args, **kwargs)

    def GetTreeCtrl(*args, **kwargs):
        """GetTreeCtrl() -> RemotelyScrolledTreeCtrl"""
        return _gizmos.TreeCompanionWindow_GetTreeCtrl(*args, **kwargs)

    def SetTreeCtrl(*args, **kwargs):
        """SetTreeCtrl(RemotelyScrolledTreeCtrl treeCtrl)"""
        return _gizmos.TreeCompanionWindow_SetTreeCtrl(*args, **kwargs)


class TreeCompanionWindowPtr(TreeCompanionWindow):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = TreeCompanionWindow
_gizmos.TreeCompanionWindow_swigregister(TreeCompanionWindowPtr)

class ThinSplitterWindow(windows.SplitterWindow):
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxThinSplitterWindow instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """
        __init__(Window parent, int id=-1, Point pos=DefaultPosition, 
            Size size=DefaultSize, long style=wxSP_3D|wxCLIP_CHILDREN) -> ThinSplitterWindow
        """
        newobj = _gizmos.new_ThinSplitterWindow(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)


class ThinSplitterWindowPtr(ThinSplitterWindow):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = ThinSplitterWindow
_gizmos.ThinSplitterWindow_swigregister(ThinSplitterWindowPtr)

class SplitterScrolledWindow(windows.ScrolledWindow):
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxSplitterScrolledWindow instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """
        __init__(Window parent, int id=-1, Point pos=DefaultPosition, 
            Size size=DefaultSize, long style=0) -> SplitterScrolledWindow
        """
        newobj = _gizmos.new_SplitterScrolledWindow(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)


class SplitterScrolledWindowPtr(SplitterScrolledWindow):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = SplitterScrolledWindow
_gizmos.SplitterScrolledWindow_swigregister(SplitterScrolledWindowPtr)

LED_ALIGN_LEFT = _gizmos.LED_ALIGN_LEFT
LED_ALIGN_RIGHT = _gizmos.LED_ALIGN_RIGHT
LED_ALIGN_CENTER = _gizmos.LED_ALIGN_CENTER
LED_ALIGN_MASK = _gizmos.LED_ALIGN_MASK
LED_DRAW_FADED = _gizmos.LED_DRAW_FADED
class LEDNumberCtrl(core.Control):
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxLEDNumberCtrl instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """
        __init__(Window parent, int id=-1, Point pos=DefaultPosition, 
            Size size=DefaultSize, long style=wxLED_ALIGN_LEFT|wxLED_DRAW_FADED) -> LEDNumberCtrl
        """
        newobj = _gizmos.new_LEDNumberCtrl(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)

    def Create(*args, **kwargs):
        """
        Create(Window parent, int id=-1, Point pos=DefaultPosition, 
            Size size=DefaultSize, long style=wxLED_ALIGN_LEFT|wxLED_DRAW_FADED) -> bool
        """
        return _gizmos.LEDNumberCtrl_Create(*args, **kwargs)

    def GetAlignment(*args, **kwargs):
        """GetAlignment() -> wxLEDValueAlign"""
        return _gizmos.LEDNumberCtrl_GetAlignment(*args, **kwargs)

    def GetDrawFaded(*args, **kwargs):
        """GetDrawFaded() -> bool"""
        return _gizmos.LEDNumberCtrl_GetDrawFaded(*args, **kwargs)

    def GetValue(*args, **kwargs):
        """GetValue() -> wxString"""
        return _gizmos.LEDNumberCtrl_GetValue(*args, **kwargs)

    def SetAlignment(*args, **kwargs):
        """SetAlignment(wxLEDValueAlign Alignment, bool Redraw=true)"""
        return _gizmos.LEDNumberCtrl_SetAlignment(*args, **kwargs)

    def SetDrawFaded(*args, **kwargs):
        """SetDrawFaded(bool DrawFaded, bool Redraw=true)"""
        return _gizmos.LEDNumberCtrl_SetDrawFaded(*args, **kwargs)

    def SetValue(*args, **kwargs):
        """SetValue(wxString Value, bool Redraw=true)"""
        return _gizmos.LEDNumberCtrl_SetValue(*args, **kwargs)


class LEDNumberCtrlPtr(LEDNumberCtrl):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = LEDNumberCtrl
_gizmos.LEDNumberCtrl_swigregister(LEDNumberCtrlPtr)

def PreLEDNumberCtrl(*args, **kwargs):
    """PreLEDNumberCtrl() -> LEDNumberCtrl"""
    val = _gizmos.new_PreLEDNumberCtrl(*args, **kwargs)
    val.thisown = 1
    return val

TL_ALIGN_LEFT = _gizmos.TL_ALIGN_LEFT
TL_ALIGN_RIGHT = _gizmos.TL_ALIGN_RIGHT
TL_ALIGN_CENTER = _gizmos.TL_ALIGN_CENTER
TREE_HITTEST_ONITEMCOLUMN = _gizmos.TREE_HITTEST_ONITEMCOLUMN
class TreeListColumnInfo(core.Object):
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxTreeListColumnInfo instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """
        __init__(wxString text=wxPyEmptyString, int image=-1, size_t width=100, 
            wxTreeListColumnAlign alignment=TL_ALIGN_LEFT) -> TreeListColumnInfo
        """
        newobj = _gizmos.new_TreeListColumnInfo(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def GetAlignment(*args, **kwargs):
        """GetAlignment() -> wxTreeListColumnAlign"""
        return _gizmos.TreeListColumnInfo_GetAlignment(*args, **kwargs)

    def GetText(*args, **kwargs):
        """GetText() -> wxString"""
        return _gizmos.TreeListColumnInfo_GetText(*args, **kwargs)

    def GetImage(*args, **kwargs):
        """GetImage() -> int"""
        return _gizmos.TreeListColumnInfo_GetImage(*args, **kwargs)

    def GetSelectedImage(*args, **kwargs):
        """GetSelectedImage() -> int"""
        return _gizmos.TreeListColumnInfo_GetSelectedImage(*args, **kwargs)

    def GetWidth(*args, **kwargs):
        """GetWidth() -> size_t"""
        return _gizmos.TreeListColumnInfo_GetWidth(*args, **kwargs)

    def SetAlignment(*args, **kwargs):
        """SetAlignment(wxTreeListColumnAlign alignment)"""
        return _gizmos.TreeListColumnInfo_SetAlignment(*args, **kwargs)

    def SetText(*args, **kwargs):
        """SetText(wxString text)"""
        return _gizmos.TreeListColumnInfo_SetText(*args, **kwargs)

    def SetImage(*args, **kwargs):
        """SetImage(int image)"""
        return _gizmos.TreeListColumnInfo_SetImage(*args, **kwargs)

    def SetSelectedImage(*args, **kwargs):
        """SetSelectedImage(int image)"""
        return _gizmos.TreeListColumnInfo_SetSelectedImage(*args, **kwargs)

    def SetWidth(*args, **kwargs):
        """SetWidth(size_t with)"""
        return _gizmos.TreeListColumnInfo_SetWidth(*args, **kwargs)


class TreeListColumnInfoPtr(TreeListColumnInfo):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = TreeListColumnInfo
_gizmos.TreeListColumnInfo_swigregister(TreeListColumnInfoPtr)

class TreeListCtrl(core.Control):
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPyTreeListCtrl instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """
        __init__(Window parent, int id=-1, Point pos=DefaultPosition, 
            Size size=DefaultSize, long style=TR_DEFAULT_STYLE, 
            Validator validator=DefaultValidator, 
            wxString name=wxPyTreeListCtrlNameStr) -> TreeListCtrl
        """
        newobj = _gizmos.new_TreeListCtrl(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self);self._setCallbackInfo(self, TreeListCtrl)

    def Create(*args, **kwargs):
        """
        Create(Window parent, int id=-1, Point pos=DefaultPosition, 
            Size size=DefaultSize, long style=TR_DEFAULT_STYLE, 
            Validator validator=DefaultValidator, 
            wxString name=wxPyTreeListCtrlNameStr) -> bool
        """
        return _gizmos.TreeListCtrl_Create(*args, **kwargs)

    def _setCallbackInfo(*args, **kwargs):
        """_setCallbackInfo(PyObject self, PyObject _class)"""
        return _gizmos.TreeListCtrl__setCallbackInfo(*args, **kwargs)

    def GetCount(*args, **kwargs):
        """GetCount() -> size_t"""
        return _gizmos.TreeListCtrl_GetCount(*args, **kwargs)

    def GetIndent(*args, **kwargs):
        """GetIndent() -> unsigned int"""
        return _gizmos.TreeListCtrl_GetIndent(*args, **kwargs)

    def SetIndent(*args, **kwargs):
        """SetIndent(unsigned int indent)"""
        return _gizmos.TreeListCtrl_SetIndent(*args, **kwargs)

    def GetSpacing(*args, **kwargs):
        """GetSpacing() -> unsigned int"""
        return _gizmos.TreeListCtrl_GetSpacing(*args, **kwargs)

    def SetSpacing(*args, **kwargs):
        """SetSpacing(unsigned int spacing)"""
        return _gizmos.TreeListCtrl_SetSpacing(*args, **kwargs)

    def GetLineSpacing(*args, **kwargs):
        """GetLineSpacing() -> unsigned int"""
        return _gizmos.TreeListCtrl_GetLineSpacing(*args, **kwargs)

    def SetLineSpacing(*args, **kwargs):
        """SetLineSpacing(unsigned int spacing)"""
        return _gizmos.TreeListCtrl_SetLineSpacing(*args, **kwargs)

    def GetImageList(*args, **kwargs):
        """GetImageList() -> wxImageList"""
        return _gizmos.TreeListCtrl_GetImageList(*args, **kwargs)

    def GetStateImageList(*args, **kwargs):
        """GetStateImageList() -> wxImageList"""
        return _gizmos.TreeListCtrl_GetStateImageList(*args, **kwargs)

    def GetButtonsImageList(*args, **kwargs):
        """GetButtonsImageList() -> wxImageList"""
        return _gizmos.TreeListCtrl_GetButtonsImageList(*args, **kwargs)

    def SetImageList(*args, **kwargs):
        """SetImageList(wxImageList imageList)"""
        return _gizmos.TreeListCtrl_SetImageList(*args, **kwargs)

    def SetStateImageList(*args, **kwargs):
        """SetStateImageList(wxImageList imageList)"""
        return _gizmos.TreeListCtrl_SetStateImageList(*args, **kwargs)

    def SetButtonsImageList(*args, **kwargs):
        """SetButtonsImageList(wxImageList imageList)"""
        return _gizmos.TreeListCtrl_SetButtonsImageList(*args, **kwargs)

    def AssignImageList(*args, **kwargs):
        """AssignImageList(wxImageList imageList)"""
        val = _gizmos.TreeListCtrl_AssignImageList(*args, **kwargs)
        args[1].thisown = 0
        return val

    def AssignStateImageList(*args, **kwargs):
        """AssignStateImageList(wxImageList imageList)"""
        val = _gizmos.TreeListCtrl_AssignStateImageList(*args, **kwargs)
        args[1].thisown = 0
        return val

    def AssignButtonsImageList(*args, **kwargs):
        """AssignButtonsImageList(wxImageList imageList)"""
        val = _gizmos.TreeListCtrl_AssignButtonsImageList(*args, **kwargs)
        args[1].thisown = 0
        return val

    def AddColumn(*args, **kwargs):
        """AddColumn(wxString text)"""
        return _gizmos.TreeListCtrl_AddColumn(*args, **kwargs)

    def AddColumnInfo(*args, **kwargs):
        """AddColumnInfo(TreeListColumnInfo col)"""
        return _gizmos.TreeListCtrl_AddColumnInfo(*args, **kwargs)

    def InsertColumn(*args, **kwargs):
        """InsertColumn(size_t before, wxString text)"""
        return _gizmos.TreeListCtrl_InsertColumn(*args, **kwargs)

    def InsertColumnInfo(*args, **kwargs):
        """InsertColumnInfo(size_t before, TreeListColumnInfo col)"""
        return _gizmos.TreeListCtrl_InsertColumnInfo(*args, **kwargs)

    def RemoveColumn(*args, **kwargs):
        """RemoveColumn(size_t column)"""
        return _gizmos.TreeListCtrl_RemoveColumn(*args, **kwargs)

    def GetColumnCount(*args, **kwargs):
        """GetColumnCount() -> size_t"""
        return _gizmos.TreeListCtrl_GetColumnCount(*args, **kwargs)

    def SetColumnWidth(*args, **kwargs):
        """SetColumnWidth(size_t column, size_t width)"""
        return _gizmos.TreeListCtrl_SetColumnWidth(*args, **kwargs)

    def GetColumnWidth(*args, **kwargs):
        """GetColumnWidth(size_t column) -> int"""
        return _gizmos.TreeListCtrl_GetColumnWidth(*args, **kwargs)

    def SetMainColumn(*args, **kwargs):
        """SetMainColumn(size_t column)"""
        return _gizmos.TreeListCtrl_SetMainColumn(*args, **kwargs)

    def GetMainColumn(*args, **kwargs):
        """GetMainColumn() -> size_t"""
        return _gizmos.TreeListCtrl_GetMainColumn(*args, **kwargs)

    def SetColumnText(*args, **kwargs):
        """SetColumnText(size_t column, wxString text)"""
        return _gizmos.TreeListCtrl_SetColumnText(*args, **kwargs)

    def GetColumnText(*args, **kwargs):
        """GetColumnText(size_t column) -> wxString"""
        return _gizmos.TreeListCtrl_GetColumnText(*args, **kwargs)

    def SetColumn(*args, **kwargs):
        """SetColumn(size_t column, TreeListColumnInfo info)"""
        return _gizmos.TreeListCtrl_SetColumn(*args, **kwargs)

    def GetColumn(*args, **kwargs):
        """GetColumn(size_t column) -> TreeListColumnInfo"""
        return _gizmos.TreeListCtrl_GetColumn(*args, **kwargs)

    def SetColumnAlignment(*args, **kwargs):
        """SetColumnAlignment(size_t column, wxTreeListColumnAlign align)"""
        return _gizmos.TreeListCtrl_SetColumnAlignment(*args, **kwargs)

    def GetColumnAlignment(*args, **kwargs):
        """GetColumnAlignment(size_t column) -> wxTreeListColumnAlign"""
        return _gizmos.TreeListCtrl_GetColumnAlignment(*args, **kwargs)

    def SetColumnImage(*args, **kwargs):
        """SetColumnImage(size_t column, int image)"""
        return _gizmos.TreeListCtrl_SetColumnImage(*args, **kwargs)

    def GetColumnImage(*args, **kwargs):
        """GetColumnImage(size_t column) -> int"""
        return _gizmos.TreeListCtrl_GetColumnImage(*args, **kwargs)

    def GetItemText(*args, **kwargs):
        """GetItemText(TreeItemId item, int column=-1) -> wxString"""
        return _gizmos.TreeListCtrl_GetItemText(*args, **kwargs)

    def GetItemImage(*args, **kwargs):
        """GetItemImage(TreeItemId item, int column=-1, wxTreeItemIcon which=TreeItemIcon_Normal) -> int"""
        return _gizmos.TreeListCtrl_GetItemImage(*args, **kwargs)

    def SetItemText(*args, **kwargs):
        """SetItemText(TreeItemId item, wxString text, int column=-1)"""
        return _gizmos.TreeListCtrl_SetItemText(*args, **kwargs)

    def SetItemImage(*args, **kwargs):
        """SetItemImage(TreeItemId item, int image, int column=-1, wxTreeItemIcon which=TreeItemIcon_Normal)"""
        return _gizmos.TreeListCtrl_SetItemImage(*args, **kwargs)

    def GetItemData(*args, **kwargs):
        """GetItemData(TreeItemId item) -> TreeItemData"""
        return _gizmos.TreeListCtrl_GetItemData(*args, **kwargs)

    def SetItemData(*args, **kwargs):
        """SetItemData(TreeItemId item, TreeItemData data)"""
        return _gizmos.TreeListCtrl_SetItemData(*args, **kwargs)

    def GetItemPyData(*args, **kwargs):
        """GetItemPyData(TreeItemId item) -> PyObject"""
        return _gizmos.TreeListCtrl_GetItemPyData(*args, **kwargs)

    def SetItemPyData(*args, **kwargs):
        """SetItemPyData(TreeItemId item, PyObject obj)"""
        return _gizmos.TreeListCtrl_SetItemPyData(*args, **kwargs)

    def SetItemHasChildren(*args, **kwargs):
        """SetItemHasChildren(TreeItemId item, bool has=True)"""
        return _gizmos.TreeListCtrl_SetItemHasChildren(*args, **kwargs)

    def SetItemBold(*args, **kwargs):
        """SetItemBold(TreeItemId item, bool bold=True)"""
        return _gizmos.TreeListCtrl_SetItemBold(*args, **kwargs)

    def SetItemTextColour(*args, **kwargs):
        """SetItemTextColour(TreeItemId item, wxColour col)"""
        return _gizmos.TreeListCtrl_SetItemTextColour(*args, **kwargs)

    def SetItemBackgroundColour(*args, **kwargs):
        """SetItemBackgroundColour(TreeItemId item, wxColour col)"""
        return _gizmos.TreeListCtrl_SetItemBackgroundColour(*args, **kwargs)

    def SetItemFont(*args, **kwargs):
        """SetItemFont(TreeItemId item, wxFont font)"""
        return _gizmos.TreeListCtrl_SetItemFont(*args, **kwargs)

    def GetItemBold(*args, **kwargs):
        """GetItemBold(TreeItemId item) -> bool"""
        return _gizmos.TreeListCtrl_GetItemBold(*args, **kwargs)

    def GetItemTextColour(*args, **kwargs):
        """GetItemTextColour(TreeItemId item) -> wxColour"""
        return _gizmos.TreeListCtrl_GetItemTextColour(*args, **kwargs)

    def GetItemBackgroundColour(*args, **kwargs):
        """GetItemBackgroundColour(TreeItemId item) -> wxColour"""
        return _gizmos.TreeListCtrl_GetItemBackgroundColour(*args, **kwargs)

    def GetItemFont(*args, **kwargs):
        """GetItemFont(TreeItemId item) -> wxFont"""
        return _gizmos.TreeListCtrl_GetItemFont(*args, **kwargs)

    def IsVisible(*args, **kwargs):
        """IsVisible(TreeItemId item) -> bool"""
        return _gizmos.TreeListCtrl_IsVisible(*args, **kwargs)

    def ItemHasChildren(*args, **kwargs):
        """ItemHasChildren(TreeItemId item) -> bool"""
        return _gizmos.TreeListCtrl_ItemHasChildren(*args, **kwargs)

    def IsExpanded(*args, **kwargs):
        """IsExpanded(TreeItemId item) -> bool"""
        return _gizmos.TreeListCtrl_IsExpanded(*args, **kwargs)

    def IsSelected(*args, **kwargs):
        """IsSelected(TreeItemId item) -> bool"""
        return _gizmos.TreeListCtrl_IsSelected(*args, **kwargs)

    def IsBold(*args, **kwargs):
        """IsBold(TreeItemId item) -> bool"""
        return _gizmos.TreeListCtrl_IsBold(*args, **kwargs)

    def GetChildrenCount(*args, **kwargs):
        """GetChildrenCount(TreeItemId item, bool recursively=True) -> size_t"""
        return _gizmos.TreeListCtrl_GetChildrenCount(*args, **kwargs)

    def GetRootItem(*args, **kwargs):
        """GetRootItem() -> TreeItemId"""
        return _gizmos.TreeListCtrl_GetRootItem(*args, **kwargs)

    def GetSelection(*args, **kwargs):
        """GetSelection() -> TreeItemId"""
        return _gizmos.TreeListCtrl_GetSelection(*args, **kwargs)

    def GetSelections(*args, **kwargs):
        """GetSelections() -> PyObject"""
        return _gizmos.TreeListCtrl_GetSelections(*args, **kwargs)

    def GetItemParent(*args, **kwargs):
        """GetItemParent(TreeItemId item) -> TreeItemId"""
        return _gizmos.TreeListCtrl_GetItemParent(*args, **kwargs)

    def GetFirstChild(*args, **kwargs):
        """GetFirstChild(TreeItemId item) -> PyObject"""
        return _gizmos.TreeListCtrl_GetFirstChild(*args, **kwargs)

    def GetNextChild(*args, **kwargs):
        """GetNextChild(TreeItemId item, long cookie) -> PyObject"""
        return _gizmos.TreeListCtrl_GetNextChild(*args, **kwargs)

    def GetLastChild(*args, **kwargs):
        """GetLastChild(TreeItemId item) -> TreeItemId"""
        return _gizmos.TreeListCtrl_GetLastChild(*args, **kwargs)

    def GetNextSibling(*args, **kwargs):
        """GetNextSibling(TreeItemId item) -> TreeItemId"""
        return _gizmos.TreeListCtrl_GetNextSibling(*args, **kwargs)

    def GetPrevSibling(*args, **kwargs):
        """GetPrevSibling(TreeItemId item) -> TreeItemId"""
        return _gizmos.TreeListCtrl_GetPrevSibling(*args, **kwargs)

    def GetFirstVisibleItem(*args, **kwargs):
        """GetFirstVisibleItem() -> TreeItemId"""
        return _gizmos.TreeListCtrl_GetFirstVisibleItem(*args, **kwargs)

    def GetNextVisible(*args, **kwargs):
        """GetNextVisible(TreeItemId item) -> TreeItemId"""
        return _gizmos.TreeListCtrl_GetNextVisible(*args, **kwargs)

    def GetPrevVisible(*args, **kwargs):
        """GetPrevVisible(TreeItemId item) -> TreeItemId"""
        return _gizmos.TreeListCtrl_GetPrevVisible(*args, **kwargs)

    def GetNext(*args, **kwargs):
        """GetNext(TreeItemId item) -> TreeItemId"""
        return _gizmos.TreeListCtrl_GetNext(*args, **kwargs)

    def AddRoot(*args, **kwargs):
        """
        AddRoot(wxString text, int image=-1, int selectedImage=-1, 
            TreeItemData data=None) -> TreeItemId
        """
        return _gizmos.TreeListCtrl_AddRoot(*args, **kwargs)

    def PrependItem(*args, **kwargs):
        """
        PrependItem(TreeItemId parent, wxString text, int image=-1, int selectedImage=-1, 
            TreeItemData data=None) -> TreeItemId
        """
        return _gizmos.TreeListCtrl_PrependItem(*args, **kwargs)

    def InsertItem(*args, **kwargs):
        """
        InsertItem(TreeItemId parent, TreeItemId idPrevious, wxString text, 
            int image=-1, int selectedImage=-1, TreeItemData data=None) -> TreeItemId
        """
        return _gizmos.TreeListCtrl_InsertItem(*args, **kwargs)

    def InsertItemBefore(*args, **kwargs):
        """
        InsertItemBefore(TreeItemId parent, size_t index, wxString text, int image=-1, 
            int selectedImage=-1, TreeItemData data=None) -> TreeItemId
        """
        return _gizmos.TreeListCtrl_InsertItemBefore(*args, **kwargs)

    def AppendItem(*args, **kwargs):
        """
        AppendItem(TreeItemId parent, wxString text, int image=-1, int selectedImage=-1, 
            TreeItemData data=None) -> TreeItemId
        """
        return _gizmos.TreeListCtrl_AppendItem(*args, **kwargs)

    def Delete(*args, **kwargs):
        """Delete(TreeItemId item)"""
        return _gizmos.TreeListCtrl_Delete(*args, **kwargs)

    def DeleteChildren(*args, **kwargs):
        """DeleteChildren(TreeItemId item)"""
        return _gizmos.TreeListCtrl_DeleteChildren(*args, **kwargs)

    def DeleteAllItems(*args, **kwargs):
        """DeleteAllItems()"""
        return _gizmos.TreeListCtrl_DeleteAllItems(*args, **kwargs)

    def Expand(*args, **kwargs):
        """Expand(TreeItemId item)"""
        return _gizmos.TreeListCtrl_Expand(*args, **kwargs)

    def ExpandAll(*args, **kwargs):
        """ExpandAll(TreeItemId item)"""
        return _gizmos.TreeListCtrl_ExpandAll(*args, **kwargs)

    def Collapse(*args, **kwargs):
        """Collapse(TreeItemId item)"""
        return _gizmos.TreeListCtrl_Collapse(*args, **kwargs)

    def CollapseAndReset(*args, **kwargs):
        """CollapseAndReset(TreeItemId item)"""
        return _gizmos.TreeListCtrl_CollapseAndReset(*args, **kwargs)

    def Toggle(*args, **kwargs):
        """Toggle(TreeItemId item)"""
        return _gizmos.TreeListCtrl_Toggle(*args, **kwargs)

    def Unselect(*args, **kwargs):
        """Unselect()"""
        return _gizmos.TreeListCtrl_Unselect(*args, **kwargs)

    def UnselectAll(*args, **kwargs):
        """UnselectAll()"""
        return _gizmos.TreeListCtrl_UnselectAll(*args, **kwargs)

    def SelectItem(*args, **kwargs):
        """SelectItem(TreeItemId item, bool unselect_others=True, bool extended_select=False)"""
        return _gizmos.TreeListCtrl_SelectItem(*args, **kwargs)

    def EnsureVisible(*args, **kwargs):
        """EnsureVisible(TreeItemId item)"""
        return _gizmos.TreeListCtrl_EnsureVisible(*args, **kwargs)

    def ScrollTo(*args, **kwargs):
        """ScrollTo(TreeItemId item)"""
        return _gizmos.TreeListCtrl_ScrollTo(*args, **kwargs)

    def HitTest(*args, **kwargs):
        """HitTest(Point point, int OUTPUT, int OUTPUT) -> TreeItemId"""
        return _gizmos.TreeListCtrl_HitTest(*args, **kwargs)

    def GetBoundingRect(*args, **kwargs):
        """GetBoundingRect(TreeItemId item, bool textOnly=False) -> PyObject"""
        return _gizmos.TreeListCtrl_GetBoundingRect(*args, **kwargs)

    def EditLabel(*args, **kwargs):
        """EditLabel(TreeItemId item)"""
        return _gizmos.TreeListCtrl_EditLabel(*args, **kwargs)

    def Edit(*args, **kwargs):
        """Edit(TreeItemId item)"""
        return _gizmos.TreeListCtrl_Edit(*args, **kwargs)

    def SortChildren(*args, **kwargs):
        """SortChildren(TreeItemId item)"""
        return _gizmos.TreeListCtrl_SortChildren(*args, **kwargs)

    def GetItemSelectedImage(*args, **kwargs):
        """GetItemSelectedImage(TreeItemId item) -> int"""
        return _gizmos.TreeListCtrl_GetItemSelectedImage(*args, **kwargs)

    def SetItemSelectedImage(*args, **kwargs):
        """SetItemSelectedImage(TreeItemId item, int image)"""
        return _gizmos.TreeListCtrl_SetItemSelectedImage(*args, **kwargs)

    def GetHeaderWindow(*args, **kwargs):
        """GetHeaderWindow() -> Window"""
        return _gizmos.TreeListCtrl_GetHeaderWindow(*args, **kwargs)

    def GetMainWindow(*args, **kwargs):
        """GetMainWindow() -> Window"""
        return _gizmos.TreeListCtrl_GetMainWindow(*args, **kwargs)


class TreeListCtrlPtr(TreeListCtrl):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = TreeListCtrl
_gizmos.TreeListCtrl_swigregister(TreeListCtrlPtr)

def PreTreeListCtrl(*args, **kwargs):
    """PreTreeListCtrl() -> TreeListCtrl"""
    val = _gizmos.new_PreTreeListCtrl(*args, **kwargs)
    val.thisown = 1
    return val


