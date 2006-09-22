# This file was created automatically by SWIG 1.3.29.
# Don't modify this file, modify the SWIG interface instead.

"""
The wx.aui moduleis an Advanced User Interface library that aims to
implement "cutting-edge" interface usability and design features so
developers can quickly and easily create beautiful and usable
application interfaces.

**Vision and Design Principles**

wx.aui attempts to encapsulate the following aspects of the user
interface:

  * Frame Management: Frame management provides the means to open,
    move and hide common controls that are needed to interact with the
    document, and allow these configurations to be saved into
    different perspectives and loaded at a later time.

  * Toolbars: Toolbars are a specialized subset of the frame
    management system and should behave similarly to other docked
    components. However, they also require additional functionality,
    such as "spring-loaded" rebar support, "chevron" buttons and
    end-user customizability.

  * Modeless Controls: Modeless controls expose a tool palette or set
    of options that float above the application content while allowing
    it to be accessed. Usually accessed by the toolbar, these controls
    disappear when an option is selected, but may also be "torn off"
    the toolbar into a floating frame of their own.

  * Look and Feel: Look and feel encompasses the way controls are
    drawn, both when shown statically as well as when they are being
    moved. This aspect of user interface design incorporates "special
    effects" such as transparent window dragging as well as frame
    animation.

**PyAUI adheres to the following principles**

  - Use native floating frames to obtain a native look and feel for
    all platforms;

  - Use existing wxPython code where possible, such as sizer
    implementation for frame management;

  - Use standard wxPython coding conventions.


**Usage**

The following example shows a simple implementation that utilizes
`wx.aui.FrameManager` to manage three text controls in a frame window::

    import wx
    import wx.aui

    class MyFrame(wx.Frame):

        def __init__(self, parent, id=-1, title='wx.aui Test',
                     size=(800, 600), style=wx.DEFAULT_FRAME_STYLE):
            wx.Frame.__init__(self, parent, id, title, pos, size, style)

            self._mgr = wx.aui.FrameManager(self)

            # create several text controls
            text1 = wx.TextCtrl(self, -1, 'Pane 1 - sample text',
                                wx.DefaultPosition, wx.Size(200,150),
                                wx.NO_BORDER | wx.TE_MULTILINE)

            text2 = wx.TextCtrl(self, -1, 'Pane 2 - sample text',
                                wx.DefaultPosition, wx.Size(200,150),
                                wx.NO_BORDER | wx.TE_MULTILINE)

            text3 = wx.TextCtrl(self, -1, 'Main content window',
                                wx.DefaultPosition, wx.Size(200,150),
                                wx.NO_BORDER | wx.TE_MULTILINE)

            # add the panes to the manager
            self._mgr.AddPane(text1, wx.LEFT, 'Pane Number One')
            self._mgr.AddPane(text2, wx.BOTTOM, 'Pane Number Two')
            self._mgr.AddPane(text3, wx.CENTER)

            # tell the manager to 'commit' all the changes just made
            self._mgr.Update()

            self.Bind(wx.EVT_CLOSE, self.OnClose)


        def OnClose(self, event):
            # deinitialize the frame manager
            self._mgr.UnInit()
            # delete the frame
            self.Destroy()


    app = wx.App()
    frame = MyFrame(None)
    frame.Show()
    app.MainLoop()

"""

import _aui
import new
new_instancemethod = new.instancemethod
def _swig_setattr_nondynamic(self,class_type,name,value,static=1):
    if (name == "thisown"): return self.this.own(value)
    if (name == "this"):
        if type(value).__name__ == 'PySwigObject':
            self.__dict__[name] = value
            return
    method = class_type.__swig_setmethods__.get(name,None)
    if method: return method(self,value)
    if (not static) or hasattr(self,name):
        self.__dict__[name] = value
    else:
        raise AttributeError("You cannot add attributes to %s" % self)

def _swig_setattr(self,class_type,name,value):
    return _swig_setattr_nondynamic(self,class_type,name,value,0)

def _swig_getattr(self,class_type,name):
    if (name == "thisown"): return self.this.own()
    method = class_type.__swig_getmethods__.get(name,None)
    if method: return method(self)
    raise AttributeError,name

def _swig_repr(self):
    try: strthis = "proxy of " + self.this.__repr__()
    except: strthis = ""
    return "<%s.%s; %s >" % (self.__class__.__module__, self.__class__.__name__, strthis,)

import types
try:
    _object = types.ObjectType
    _newclass = 1
except AttributeError:
    class _object : pass
    _newclass = 0
del types


def _swig_setattr_nondynamic_method(set):
    def set_attr(self,name,value):
        if (name == "thisown"): return self.this.own(value)
        if hasattr(self,name) or (name == "this"):
            set(self,name,value)
        else:
            raise AttributeError("You cannot add attributes to %s" % self)
    return set_attr


import _core
import _windows
wx = _core 
__docfilter__ = wx.__DocFilter(globals()) 
USE_AUI = _aui.USE_AUI
AUI_DOCK_NONE = _aui.AUI_DOCK_NONE
AUI_DOCK_TOP = _aui.AUI_DOCK_TOP
AUI_DOCK_RIGHT = _aui.AUI_DOCK_RIGHT
AUI_DOCK_BOTTOM = _aui.AUI_DOCK_BOTTOM
AUI_DOCK_LEFT = _aui.AUI_DOCK_LEFT
AUI_DOCK_CENTER = _aui.AUI_DOCK_CENTER
AUI_DOCK_CENTRE = _aui.AUI_DOCK_CENTRE
AUI_MGR_ALLOW_FLOATING = _aui.AUI_MGR_ALLOW_FLOATING
AUI_MGR_ALLOW_ACTIVE_PANE = _aui.AUI_MGR_ALLOW_ACTIVE_PANE
AUI_MGR_TRANSPARENT_DRAG = _aui.AUI_MGR_TRANSPARENT_DRAG
AUI_MGR_TRANSPARENT_HINT = _aui.AUI_MGR_TRANSPARENT_HINT
AUI_MGR_TRANSPARENT_HINT_FADE = _aui.AUI_MGR_TRANSPARENT_HINT_FADE
AUI_MGR_DISABLE_VENETIAN_BLINDS = _aui.AUI_MGR_DISABLE_VENETIAN_BLINDS
AUI_MGR_DISABLE_VENETIAN_BLINDS_FADE = _aui.AUI_MGR_DISABLE_VENETIAN_BLINDS_FADE
AUI_MGR_DEFAULT = _aui.AUI_MGR_DEFAULT
AUI_ART_SASH_SIZE = _aui.AUI_ART_SASH_SIZE
AUI_ART_CAPTION_SIZE = _aui.AUI_ART_CAPTION_SIZE
AUI_ART_GRIPPER_SIZE = _aui.AUI_ART_GRIPPER_SIZE
AUI_ART_PANE_BORDER_SIZE = _aui.AUI_ART_PANE_BORDER_SIZE
AUI_ART_PANE_BUTTON_SIZE = _aui.AUI_ART_PANE_BUTTON_SIZE
AUI_ART_BACKGROUND_COLOUR = _aui.AUI_ART_BACKGROUND_COLOUR
AUI_ART_SASH_COLOUR = _aui.AUI_ART_SASH_COLOUR
AUI_ART_ACTIVE_CAPTION_COLOUR = _aui.AUI_ART_ACTIVE_CAPTION_COLOUR
AUI_ART_ACTIVE_CAPTION_GRADIENT_COLOUR = _aui.AUI_ART_ACTIVE_CAPTION_GRADIENT_COLOUR
AUI_ART_INACTIVE_CAPTION_COLOUR = _aui.AUI_ART_INACTIVE_CAPTION_COLOUR
AUI_ART_INACTIVE_CAPTION_GRADIENT_COLOUR = _aui.AUI_ART_INACTIVE_CAPTION_GRADIENT_COLOUR
AUI_ART_ACTIVE_CAPTION_TEXT_COLOUR = _aui.AUI_ART_ACTIVE_CAPTION_TEXT_COLOUR
AUI_ART_INACTIVE_CAPTION_TEXT_COLOUR = _aui.AUI_ART_INACTIVE_CAPTION_TEXT_COLOUR
AUI_ART_BORDER_COLOUR = _aui.AUI_ART_BORDER_COLOUR
AUI_ART_GRIPPER_COLOUR = _aui.AUI_ART_GRIPPER_COLOUR
AUI_ART_CAPTION_FONT = _aui.AUI_ART_CAPTION_FONT
AUI_ART_GRADIENT_TYPE = _aui.AUI_ART_GRADIENT_TYPE
AUI_GRADIENT_NONE = _aui.AUI_GRADIENT_NONE
AUI_GRADIENT_VERTICAL = _aui.AUI_GRADIENT_VERTICAL
AUI_GRADIENT_HORIZONTAL = _aui.AUI_GRADIENT_HORIZONTAL
AUI_BUTTON_STATE_NORMAL = _aui.AUI_BUTTON_STATE_NORMAL
AUI_BUTTON_STATE_HOVER = _aui.AUI_BUTTON_STATE_HOVER
AUI_BUTTON_STATE_PRESSED = _aui.AUI_BUTTON_STATE_PRESSED
AUI_INSERT_PANE = _aui.AUI_INSERT_PANE
AUI_INSERT_ROW = _aui.AUI_INSERT_ROW
AUI_INSERT_DOCK = _aui.AUI_INSERT_DOCK
class PaneInfo(object):
    """
    PaneInfo specifies all the parameters for a pane for the
    `FrameManager`. These parameters specify where the pane is on the
    screen, whether it is docked or floating, or hidden. In addition,
    these parameters specify the pane's docked position, floating
    position, preferred size, minimum size, caption text among many other
    parameters.

    """
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self) -> PaneInfo

        PaneInfo specifies all the parameters for a pane for the
        `FrameManager`. These parameters specify where the pane is on the
        screen, whether it is docked or floating, or hidden. In addition,
        these parameters specify the pane's docked position, floating
        position, preferred size, minimum size, caption text among many other
        parameters.

        """
        _aui.PaneInfo_swiginit(self,_aui.new_PaneInfo(*args, **kwargs))
    __swig_destroy__ = _aui.delete_PaneInfo
    __del__ = lambda self : None;
    def SafeSet(*args, **kwargs):
        """SafeSet(self, PaneInfo source)"""
        return _aui.PaneInfo_SafeSet(*args, **kwargs)

    def IsOk(*args, **kwargs):
        """
        IsOk(self) -> bool

        IsOk returns ``True`` if the PaneInfo structure is valid.

        """
        return _aui.PaneInfo_IsOk(*args, **kwargs)

    def IsFixed(*args, **kwargs):
        """
        IsFixed(self) -> bool

        IsFixed returns ``True`` if the pane cannot be resized.

        """
        return _aui.PaneInfo_IsFixed(*args, **kwargs)

    def IsResizable(*args, **kwargs):
        """
        IsResizable(self) -> bool

        IsResizeable returns ``True`` if the pane can be resized.

        """
        return _aui.PaneInfo_IsResizable(*args, **kwargs)

    def IsShown(*args, **kwargs):
        """
        IsShown(self) -> bool

        IsShown returns ``True`` if the pane should be drawn on the screen.

        """
        return _aui.PaneInfo_IsShown(*args, **kwargs)

    def IsFloating(*args, **kwargs):
        """
        IsFloating(self) -> bool

        IsFloating returns ``True`` if the pane is floating.

        """
        return _aui.PaneInfo_IsFloating(*args, **kwargs)

    def IsDocked(*args, **kwargs):
        """
        IsDocked(self) -> bool

        IsDocked returns ``True`` if the pane is docked.

        """
        return _aui.PaneInfo_IsDocked(*args, **kwargs)

    def IsToolbar(*args, **kwargs):
        """
        IsToolbar(self) -> bool

        IsToolbar returns ``True`` if the pane contains a toolbar.

        """
        return _aui.PaneInfo_IsToolbar(*args, **kwargs)

    def IsTopDockable(*args, **kwargs):
        """
        IsTopDockable(self) -> bool

        IsTopDockable returns ``True`` if the pane can be docked at the top of
        the managed frame.

        """
        return _aui.PaneInfo_IsTopDockable(*args, **kwargs)

    def IsBottomDockable(*args, **kwargs):
        """
        IsBottomDockable(self) -> bool

        IsBottomDockable returns ``True`` if the pane can be docked at the
        bottom of the managed frame.

        """
        return _aui.PaneInfo_IsBottomDockable(*args, **kwargs)

    def IsLeftDockable(*args, **kwargs):
        """
        IsLeftDockable(self) -> bool

        IsLeftDockable returns ``True`` if the pane can be docked on the left
        of the managed frame.

        """
        return _aui.PaneInfo_IsLeftDockable(*args, **kwargs)

    def IsRightDockable(*args, **kwargs):
        """
        IsRightDockable(self) -> bool

        IsRightDockable returns ``True`` if the pane can be docked on the
        right of the managed frame.

        """
        return _aui.PaneInfo_IsRightDockable(*args, **kwargs)

    def IsFloatable(*args, **kwargs):
        """
        IsFloatable(self) -> bool

        IsFloatable returns ``True`` if the pane can be undocked and displayed
        as a floating window.

        """
        return _aui.PaneInfo_IsFloatable(*args, **kwargs)

    def IsMovable(*args, **kwargs):
        """
        IsMovable(self) -> bool

        IsMoveable returns ``True`` if the docked frame can be undocked or moved
        to another dock position.

        """
        return _aui.PaneInfo_IsMovable(*args, **kwargs)

    def HasCaption(*args, **kwargs):
        """
        HasCaption(self) -> bool

        HasCaption returns ``True`` if the pane displays a caption.

        """
        return _aui.PaneInfo_HasCaption(*args, **kwargs)

    def HasGripper(*args, **kwargs):
        """
        HasGripper(self) -> bool

        HasGripper returns ``True`` if the pane displays a gripper.

        """
        return _aui.PaneInfo_HasGripper(*args, **kwargs)

    def HasBorder(*args, **kwargs):
        """
        HasBorder(self) -> bool

        HasBorder returns ``True`` if the pane displays a border.

        """
        return _aui.PaneInfo_HasBorder(*args, **kwargs)

    def HasCloseButton(*args, **kwargs):
        """
        HasCloseButton(self) -> bool

        HasCloseButton returns ``True`` if the pane displays a button to close
        the pane.

        """
        return _aui.PaneInfo_HasCloseButton(*args, **kwargs)

    def HasMaximizeButton(*args, **kwargs):
        """
        HasMaximizeButton(self) -> bool

        HasMaximizeButton returns ``True`` if the pane displays a button to
        maximize the pane.

        """
        return _aui.PaneInfo_HasMaximizeButton(*args, **kwargs)

    def HasMinimizeButton(*args, **kwargs):
        """
        HasMinimizeButton(self) -> bool

        HasMinimizeButton returns ``True`` if the pane displays a button to
        minimize the pane.

        """
        return _aui.PaneInfo_HasMinimizeButton(*args, **kwargs)

    def HasPinButton(*args, **kwargs):
        """
        HasPinButton(self) -> bool

        HasPinButton returns ``True`` if the pane displays a button to float
        the pane.

        """
        return _aui.PaneInfo_HasPinButton(*args, **kwargs)

    def HasGripperTop(*args, **kwargs):
        """HasGripperTop(self) -> bool"""
        return _aui.PaneInfo_HasGripperTop(*args, **kwargs)

    def Window(*args, **kwargs):
        """Window(self, Window w) -> PaneInfo"""
        return _aui.PaneInfo_Window(*args, **kwargs)

    def Name(*args, **kwargs):
        """
        Name(self, String n) -> PaneInfo

        Name sets the name of the pane so it can be referenced in lookup
        functions.

        """
        return _aui.PaneInfo_Name(*args, **kwargs)

    def Caption(*args, **kwargs):
        """
        Caption(self, String c) -> PaneInfo

        Caption sets the caption of the pane.

        """
        return _aui.PaneInfo_Caption(*args, **kwargs)

    def Left(*args, **kwargs):
        """
        Left(self) -> PaneInfo

        Left sets the pane dock position to the left side of the frame.

        """
        return _aui.PaneInfo_Left(*args, **kwargs)

    def Right(*args, **kwargs):
        """
        Right(self) -> PaneInfo

        Right sets the pane dock position to the right side of the frame.

        """
        return _aui.PaneInfo_Right(*args, **kwargs)

    def Top(*args, **kwargs):
        """
        Top(self) -> PaneInfo

        Top sets the pane dock position to the top of the frame.

        """
        return _aui.PaneInfo_Top(*args, **kwargs)

    def Bottom(*args, **kwargs):
        """
        Bottom(self) -> PaneInfo

        Bottom sets the pane dock position to the bottom of the frame.

        """
        return _aui.PaneInfo_Bottom(*args, **kwargs)

    def Center(*args, **kwargs):
        """
        Center(self) -> PaneInfo

        Center sets the pane to the center position of the frame.

        """
        return _aui.PaneInfo_Center(*args, **kwargs)

    def Centre(*args, **kwargs):
        """
        Centre(self) -> PaneInfo

        Centre sets the pane to the center position of the frame.

        """
        return _aui.PaneInfo_Centre(*args, **kwargs)

    def Direction(*args, **kwargs):
        """
        Direction(self, int direction) -> PaneInfo

        Direction determines the direction of the docked pane.

        """
        return _aui.PaneInfo_Direction(*args, **kwargs)

    def Layer(*args, **kwargs):
        """
        Layer(self, int layer) -> PaneInfo

        Layer determines the layer of the docked pane.

        """
        return _aui.PaneInfo_Layer(*args, **kwargs)

    def Row(*args, **kwargs):
        """
        Row(self, int row) -> PaneInfo

        Row determines the row of the docked pane.

        """
        return _aui.PaneInfo_Row(*args, **kwargs)

    def Position(*args, **kwargs):
        """
        Position(self, int pos) -> PaneInfo

        Position determines the position of the docked pane.

        """
        return _aui.PaneInfo_Position(*args, **kwargs)

    def BestSize(*args, **kwargs):
        """
        BestSize(self, Size size) -> PaneInfo

        BestSize sets the ideal size for the pane.

        """
        return _aui.PaneInfo_BestSize(*args, **kwargs)

    def MinSize(*args, **kwargs):
        """
        MinSize(self, Size size) -> PaneInfo

        MinSize sets the minimum size of the pane.

        """
        return _aui.PaneInfo_MinSize(*args, **kwargs)

    def MaxSize(*args, **kwargs):
        """
        MaxSize(self, Size size) -> PaneInfo

        MaxSize sets the maximum size of the pane.

        """
        return _aui.PaneInfo_MaxSize(*args, **kwargs)

    def FloatingPosition(*args, **kwargs):
        """
        FloatingPosition(self, Point pos) -> PaneInfo

        FloatingPosition sets the position of the floating pane.

        """
        return _aui.PaneInfo_FloatingPosition(*args, **kwargs)

    def FloatingSize(*args, **kwargs):
        """
        FloatingSize(self, Size size) -> PaneInfo

        FloatingSize sets the size of the floating pane.

        """
        return _aui.PaneInfo_FloatingSize(*args, **kwargs)

    def Fixed(*args, **kwargs):
        """
        Fixed(self) -> PaneInfo

        Fixed forces a pane to be fixed size so that it cannot be resized.

        """
        return _aui.PaneInfo_Fixed(*args, **kwargs)

    def Resizable(*args, **kwargs):
        """
        Resizable(self, bool resizable=True) -> PaneInfo

        Resized allows a pane to be resized if resizable is true, and forces
        it to be a fixed size if resizeable is false.

        """
        return _aui.PaneInfo_Resizable(*args, **kwargs)

    def Dock(*args, **kwargs):
        """
        Dock(self) -> PaneInfo

        Dock indicates that a pane should be docked.

        """
        return _aui.PaneInfo_Dock(*args, **kwargs)

    def Float(*args, **kwargs):
        """
        Float(self) -> PaneInfo

        Float indicates that a pane should be floated.

        """
        return _aui.PaneInfo_Float(*args, **kwargs)

    def Hide(*args, **kwargs):
        """
        Hide(self) -> PaneInfo

        Hide indicates that a pane should be hidden.

        """
        return _aui.PaneInfo_Hide(*args, **kwargs)

    def Show(*args, **kwargs):
        """
        Show(self, bool show=True) -> PaneInfo

        Show indicates that a pane should be shown.

        """
        return _aui.PaneInfo_Show(*args, **kwargs)

    def CaptionVisible(*args, **kwargs):
        """
        CaptionVisible(self, bool visible=True) -> PaneInfo

        CaptionVisible indicates that a pane caption should be visible.

        """
        return _aui.PaneInfo_CaptionVisible(*args, **kwargs)

    def PaneBorder(*args, **kwargs):
        """
        PaneBorder(self, bool visible=True) -> PaneInfo

        PaneBorder indicates that a border should be drawn for the pane.

        """
        return _aui.PaneInfo_PaneBorder(*args, **kwargs)

    def Gripper(*args, **kwargs):
        """
        Gripper(self, bool visible=True) -> PaneInfo

        Gripper indicates that a gripper should be drawn for the pane..

        """
        return _aui.PaneInfo_Gripper(*args, **kwargs)

    def GripperTop(*args, **kwargs):
        """GripperTop(self, bool attop=True) -> PaneInfo"""
        return _aui.PaneInfo_GripperTop(*args, **kwargs)

    def CloseButton(*args, **kwargs):
        """
        CloseButton(self, bool visible=True) -> PaneInfo

        CloseButton indicates that a close button should be drawn for the
        pane.

        """
        return _aui.PaneInfo_CloseButton(*args, **kwargs)

    def MaximizeButton(*args, **kwargs):
        """
        MaximizeButton(self, bool visible=True) -> PaneInfo

        MaximizeButton indicates that a maximize button should be drawn for
        the pane.

        """
        return _aui.PaneInfo_MaximizeButton(*args, **kwargs)

    def MinimizeButton(*args, **kwargs):
        """
        MinimizeButton(self, bool visible=True) -> PaneInfo

        MinimizeButton indicates that a minimize button should be drawn for
        the pane.

        """
        return _aui.PaneInfo_MinimizeButton(*args, **kwargs)

    def PinButton(*args, **kwargs):
        """
        PinButton(self, bool visible=True) -> PaneInfo

        PinButton indicates that a pin button should be drawn for the pane.

        """
        return _aui.PaneInfo_PinButton(*args, **kwargs)

    def DestroyOnClose(*args, **kwargs):
        """
        DestroyOnClose(self, bool b=True) -> PaneInfo

        DestroyOnClose indicates whether a pane should be detroyed when it is
        closed.

        """
        return _aui.PaneInfo_DestroyOnClose(*args, **kwargs)

    def TopDockable(*args, **kwargs):
        """
        TopDockable(self, bool b=True) -> PaneInfo

        TopDockable indicates whether a pane can be docked at the top of the
        frame.

        """
        return _aui.PaneInfo_TopDockable(*args, **kwargs)

    def BottomDockable(*args, **kwargs):
        """
        BottomDockable(self, bool b=True) -> PaneInfo

        BottomDockable indicates whether a pane can be docked at the bottom of
        the frame.

        """
        return _aui.PaneInfo_BottomDockable(*args, **kwargs)

    def LeftDockable(*args, **kwargs):
        """
        LeftDockable(self, bool b=True) -> PaneInfo

        LeftDockable indicates whether a pane can be docked on the left of the
        frame.

        """
        return _aui.PaneInfo_LeftDockable(*args, **kwargs)

    def RightDockable(*args, **kwargs):
        """
        RightDockable(self, bool b=True) -> PaneInfo

        RightDockable indicates whether a pane can be docked on the right of
        the frame.

        """
        return _aui.PaneInfo_RightDockable(*args, **kwargs)

    def Floatable(*args, **kwargs):
        """
        Floatable(self, bool b=True) -> PaneInfo

        Floatable indicates whether a frame can be floated.

        """
        return _aui.PaneInfo_Floatable(*args, **kwargs)

    def Movable(*args, **kwargs):
        """
        Movable(self, bool b=True) -> PaneInfo

        Movable indicates whether a frame can be moved.

        """
        return _aui.PaneInfo_Movable(*args, **kwargs)

    def Dockable(*args, **kwargs):
        """
        Dockable(self, bool b=True) -> PaneInfo

        Dockable indicates whether a pane can be docked at any position of the
        frame.

        """
        return _aui.PaneInfo_Dockable(*args, **kwargs)

    def DefaultPane(*args, **kwargs):
        """
        DefaultPane(self) -> PaneInfo

        DefaultPane specifies that the pane should adopt the default pane
        settings.

        """
        return _aui.PaneInfo_DefaultPane(*args, **kwargs)

    def CentrePane(*args, **kwargs):
        """
        CentrePane(self) -> PaneInfo

        CentrePane specifies that the pane should adopt the default center
        pane settings.

        """
        return _aui.PaneInfo_CentrePane(*args, **kwargs)

    def CenterPane(*args, **kwargs):
        """
        CenterPane(self) -> PaneInfo

        CenterPane specifies that the pane should adopt the default center
        pane settings.

        """
        return _aui.PaneInfo_CenterPane(*args, **kwargs)

    def ToolbarPane(*args, **kwargs):
        """
        ToolbarPane(self) -> PaneInfo

        ToolbarPane specifies that the pane should adopt the default toolbar
        pane settings.

        """
        return _aui.PaneInfo_ToolbarPane(*args, **kwargs)

    def SetFlag(*args, **kwargs):
        """
        SetFlag(self, int flag, bool option_state) -> PaneInfo

        SetFlag turns the property given by flag on or off with the
        option_state parameter.

        """
        return _aui.PaneInfo_SetFlag(*args, **kwargs)

    def HasFlag(*args, **kwargs):
        """
        HasFlag(self, int flag) -> bool

        HasFlag returns ``True`` if the the property specified by flag is
        active for the pane.

        """
        return _aui.PaneInfo_HasFlag(*args, **kwargs)

    optionFloating = _aui.PaneInfo_optionFloating
    optionHidden = _aui.PaneInfo_optionHidden
    optionLeftDockable = _aui.PaneInfo_optionLeftDockable
    optionRightDockable = _aui.PaneInfo_optionRightDockable
    optionTopDockable = _aui.PaneInfo_optionTopDockable
    optionBottomDockable = _aui.PaneInfo_optionBottomDockable
    optionFloatable = _aui.PaneInfo_optionFloatable
    optionMovable = _aui.PaneInfo_optionMovable
    optionResizable = _aui.PaneInfo_optionResizable
    optionPaneBorder = _aui.PaneInfo_optionPaneBorder
    optionCaption = _aui.PaneInfo_optionCaption
    optionGripper = _aui.PaneInfo_optionGripper
    optionDestroyOnClose = _aui.PaneInfo_optionDestroyOnClose
    optionToolbar = _aui.PaneInfo_optionToolbar
    optionActive = _aui.PaneInfo_optionActive
    optionGripperTop = _aui.PaneInfo_optionGripperTop
    buttonClose = _aui.PaneInfo_buttonClose
    buttonMaximize = _aui.PaneInfo_buttonMaximize
    buttonMinimize = _aui.PaneInfo_buttonMinimize
    buttonPin = _aui.PaneInfo_buttonPin
    buttonCustom1 = _aui.PaneInfo_buttonCustom1
    buttonCustom2 = _aui.PaneInfo_buttonCustom2
    buttonCustom3 = _aui.PaneInfo_buttonCustom3
    actionPane = _aui.PaneInfo_actionPane
    name = property(_aui.PaneInfo_name_get, _aui.PaneInfo_name_set)
    caption = property(_aui.PaneInfo_caption_get, _aui.PaneInfo_caption_set)
    window = property(_aui.PaneInfo_window_get, _aui.PaneInfo_window_set)
    frame = property(_aui.PaneInfo_frame_get, _aui.PaneInfo_frame_set)
    state = property(_aui.PaneInfo_state_get, _aui.PaneInfo_state_set)
    dock_direction = property(_aui.PaneInfo_dock_direction_get, _aui.PaneInfo_dock_direction_set)
    dock_layer = property(_aui.PaneInfo_dock_layer_get, _aui.PaneInfo_dock_layer_set)
    dock_row = property(_aui.PaneInfo_dock_row_get, _aui.PaneInfo_dock_row_set)
    dock_pos = property(_aui.PaneInfo_dock_pos_get, _aui.PaneInfo_dock_pos_set)
    best_size = property(_aui.PaneInfo_best_size_get, _aui.PaneInfo_best_size_set)
    min_size = property(_aui.PaneInfo_min_size_get, _aui.PaneInfo_min_size_set)
    max_size = property(_aui.PaneInfo_max_size_get, _aui.PaneInfo_max_size_set)
    floating_pos = property(_aui.PaneInfo_floating_pos_get, _aui.PaneInfo_floating_pos_set)
    floating_size = property(_aui.PaneInfo_floating_size_get, _aui.PaneInfo_floating_size_set)
    dock_proportion = property(_aui.PaneInfo_dock_proportion_get, _aui.PaneInfo_dock_proportion_set)
    buttons = property(_aui.PaneInfo_buttons_get, _aui.PaneInfo_buttons_set)
    rect = property(_aui.PaneInfo_rect_get, _aui.PaneInfo_rect_set)
_aui.PaneInfo_swigregister(PaneInfo)
cvar = _aui.cvar

class FrameManager(_core.EvtHandler):
    """
    FrameManager manages the panes associated with it for a particular
    `wx.Frame`, using a pane's `PaneInfo` information to determine each
    pane's docking and floating behavior. FrameManager uses wxWidgets'
    sizer mechanism to plan the layout of each frame. It uses a
    replaceable `DockArt` class to do all drawing, so all drawing is
    localized in one area, and may be customized depending on an
    application's specific needs.

    """
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, Window managed_wnd=None, int flags=AUI_MGR_DEFAULT) -> FrameManager

        Constructor.

            :param managed_wnd: Specifies the `wx.Window` which should be
                managed.  If not set in the call to this constructor then
                `SetManagedWindow` should be called later.

            :param flags: Specifies options which allow the frame management
                behavior to be modified.

        """
        _aui.FrameManager_swiginit(self,_aui.new_FrameManager(*args, **kwargs))
    __swig_destroy__ = _aui.delete_FrameManager
    __del__ = lambda self : None;
    def UnInit(*args, **kwargs):
        """
        UnInit(self)

        UnInit uninitializes the framework and should be called before a
        managed frame is destroyed. UnInit is usually called in the managed
        window's destructor.

        """
        return _aui.FrameManager_UnInit(*args, **kwargs)

    def SetFlags(*args, **kwargs):
        """
        SetFlags(self, int flags)

        SetFlags is used to specify the FrameManager's behavioral
        settings. The flags parameter is described in the docs for `__init__`

        """
        return _aui.FrameManager_SetFlags(*args, **kwargs)

    def GetFlags(*args, **kwargs):
        """
        GetFlags(self) -> int

        GetFlags returns the current FrameManager's flags.

        """
        return _aui.FrameManager_GetFlags(*args, **kwargs)

    def SetManagedWindow(*args, **kwargs):
        """
        SetManagedWindow(self, Window managed_wnd)

        SetManagedWindow is called to specify the window which is to be
        managed by the FrameManager.  It is normally a `wx.Frame` but it is
        possible to also allow docking within any container window.  This only
        needs to be called if the window was not given to the manager in the
        constructor.

        """
        return _aui.FrameManager_SetManagedWindow(*args, **kwargs)

    def GetManagedWindow(*args, **kwargs):
        """
        GetManagedWindow(self) -> Window

        GetManagedWindow returns the window currently being managed by the
        FrameManager.

        """
        return _aui.FrameManager_GetManagedWindow(*args, **kwargs)

    def SetArtProvider(*args, **kwargs):
        """
        SetArtProvider(self, DockArt art_provider)

        SetArtProvider instructs FrameManager to use the art provider
        specified for all drawing calls. This allows plugable look-and-feel
        features. The previous art provider object, if any, will be destroyed
        by FrameManager.

        :note: If you wish to use a custom `DockArt` class to override drawing
            or metrics then you shoudl derive your class from the `PyDockArt`
            class, which has been instrumented for reflecting virtual calls to
            Python methods.

        """
        return _aui.FrameManager_SetArtProvider(*args, **kwargs)

    def GetArtProvider(*args, **kwargs):
        """
        GetArtProvider(self) -> DockArt

        GetArtProvider returns the current art provider being used.

        """
        return _aui.FrameManager_GetArtProvider(*args, **kwargs)

    def _GetPaneByWidget(*args, **kwargs):
        """_GetPaneByWidget(self, Window window) -> PaneInfo"""
        return _aui.FrameManager__GetPaneByWidget(*args, **kwargs)

    def _GetPaneByName(*args, **kwargs):
        """_GetPaneByName(self, String name) -> PaneInfo"""
        return _aui.FrameManager__GetPaneByName(*args, **kwargs)

    def GetAllPanes(*args, **kwargs):
        """
        GetAllPanes(self) -> list

        GetAllPanes returns a list of `PaneInfo` objects for all panes managed
        by the frame manager.

        """
        return _aui.FrameManager_GetAllPanes(*args, **kwargs)

    def _AddPane1(*args, **kwargs):
        """_AddPane1(self, Window window, PaneInfo pane_info) -> bool"""
        return _aui.FrameManager__AddPane1(*args, **kwargs)

    def AddPaneAtPos(*args, **kwargs):
        """AddPaneAtPos(self, Window window, PaneInfo pane_info, Point drop_pos) -> bool"""
        return _aui.FrameManager_AddPaneAtPos(*args, **kwargs)

    def _AddPane2(*args, **kwargs):
        """_AddPane2(self, Window window, int direction=LEFT, String caption=wxEmptyString) -> bool"""
        return _aui.FrameManager__AddPane2(*args, **kwargs)

    def InsertPane(*args, **kwargs):
        """
        InsertPane(self, Window window, PaneInfo insert_location, int insert_level=AUI_INSERT_PANE) -> bool

        InsertPane is used to insert either a previously unmanaged pane window
        into the frame manager, or to insert a currently managed pane
        somewhere else. InsertPane will push all panes, rows, or docks aside
        and insert the window into the position specified by
        ``insert_location``.  Because ``insert_location`` can specify either a pane,
        dock row, or dock layer, the ``insert_level`` parameter is used to
        disambiguate this. The parameter ``insert_level`` can take a value of
        ``AUI_INSERT_PANE``, ``AUI_INSERT_ROW`` or ``AUI_INSERT_DOCK``.

        """
        return _aui.FrameManager_InsertPane(*args, **kwargs)

    def DetachPane(*args, **kwargs):
        """
        DetachPane(self, Window window) -> bool

        DetachPane tells the FrameManager to stop managing the pane specified
        by window. The window, if in a floated frame, is reparented to the
        frame managed by FrameManager.

        """
        return _aui.FrameManager_DetachPane(*args, **kwargs)

    def SavePaneInfo(*args, **kwargs):
        """SavePaneInfo(self, PaneInfo pane) -> String"""
        return _aui.FrameManager_SavePaneInfo(*args, **kwargs)

    def LoadPaneInfo(*args, **kwargs):
        """LoadPaneInfo(self, String pane_part, PaneInfo pane)"""
        return _aui.FrameManager_LoadPaneInfo(*args, **kwargs)

    def SavePerspective(*args, **kwargs):
        """
        SavePerspective(self) -> String

        SavePerspective saves the entire user interface layout into an encoded
        string, which can then be stored someplace by the application.  When a
        perspective is restored using `LoadPerspective`, the entire user
        interface will return to the state it was when the perspective was
        saved.

        """
        return _aui.FrameManager_SavePerspective(*args, **kwargs)

    def LoadPerspective(*args, **kwargs):
        """
        LoadPerspective(self, String perspective, bool update=True) -> bool

        LoadPerspective loads a saved perspective. If ``update`` is ``True``,
        `Update` is automatically invoked, thus realizing the saved
        perspective on screen.

        """
        return _aui.FrameManager_LoadPerspective(*args, **kwargs)

    def Update(*args, **kwargs):
        """
        Update(self)

        Update shoudl be called called after any number of changes are made to
        any of the managed panes.  Update must be invoked after `AddPane` or
        `InsertPane` are called in order to "realize" or "commit" the
        changes. In addition, any number of changes may be made to `PaneInfo`
        structures (retrieved with `GetPane` or `GetAllPanes`), but to realize
        the changes, Update must be called. This construction allows pane
        flicker to be avoided by updating the whole layout at one time.

        """
        return _aui.FrameManager_Update(*args, **kwargs)

    def DrawHintRect(*args, **kwargs):
        """DrawHintRect(self, Window pane_window, Point pt, Point offset)"""
        return _aui.FrameManager_DrawHintRect(*args, **kwargs)

    def ShowHint(*args, **kwargs):
        """ShowHint(self, Rect rect)"""
        return _aui.FrameManager_ShowHint(*args, **kwargs)

    def HideHint(*args, **kwargs):
        """HideHint(self)"""
        return _aui.FrameManager_HideHint(*args, **kwargs)

    def OnRender(*args, **kwargs):
        """OnRender(self, FrameManagerEvent evt)"""
        return _aui.FrameManager_OnRender(*args, **kwargs)

    def OnPaneButton(*args, **kwargs):
        """OnPaneButton(self, FrameManagerEvent evt)"""
        return _aui.FrameManager_OnPaneButton(*args, **kwargs)

    def GetPane(self, item):
        """
        GetPane(self, window_or_info item) -> PaneInfo

        GetPane is used to search for a `PaneInfo` object either by
        widget reference or by pane name, which acts as a unique id
        for a window pane. The returned `PaneInfo` object may then be
        modified to change a pane's look, state or position. After one
        or more modifications to the `PaneInfo`, `FrameManager.Update`
        should be called to realize the changes to the user interface.

        If the lookup failed (meaning the pane could not be found in
        the manager) GetPane returns an empty `PaneInfo`, a condition
        which can be checked by calling `PaneInfo.IsOk`.
        """
        if isinstance(item, wx.Window):
            return self._GetPaneByWidget(item)
        else:
            return self._GetPaneByName(item)

    def AddPane(self, window, info=None, caption=None):
        """
        AddPane(self, window, info=None, caption=None) -> bool

        AddPane tells the frame manager to start managing a child
        window. There are two versions of this function. The first
        verison accepts a `PaneInfo` object for the ``info`` parameter
        and allows the full spectrum of pane parameter
        possibilities. (Say that 3 times fast!)

        The second version is used for simpler user interfaces which
        do not require as much configuration.  In this case the
        ``info`` parameter specifies the direction property of the
        pane info, and defaults to ``wx.LEFT``.  The pane caption may
        also be specified as an extra parameter in this form.
        """
        if type(info) == PaneInfo:
            return self._AddPane1(window, info)
        else:
            
            if info is None:
                info = wx.LEFT
            if caption is None:
                caption = ""
            return self._AddPane2(window, info, caption)

    SetFrame = wx._deprecated(SetManagedWindow,
                              "SetFrame is deprecated, use `SetManagedWindow` instead.")
    GetFrame = wx._deprecated(GetManagedWindow,
                              "GetFrame is deprecated, use `GetManagedWindow` instead.")

    AllPanes = property(GetAllPanes,doc="See `GetAllPanes`") 
    ArtProvider = property(GetArtProvider,SetArtProvider,doc="See `GetArtProvider` and `SetArtProvider`") 
    Flags = property(GetFlags,SetFlags,doc="See `GetFlags` and `SetFlags`") 
    ManagedWindow = property(GetManagedWindow,SetManagedWindow,doc="See `GetManagedWindow` and `SetManagedWindow`") 
_aui.FrameManager_swigregister(FrameManager)

class FrameManagerEvent(_core.Event):
    """Proxy of C++ FrameManagerEvent class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """__init__(self, EventType type=wxEVT_NULL) -> FrameManagerEvent"""
        _aui.FrameManagerEvent_swiginit(self,_aui.new_FrameManagerEvent(*args, **kwargs))
    def Clone(*args, **kwargs):
        """Clone(self) -> Event"""
        return _aui.FrameManagerEvent_Clone(*args, **kwargs)

    def SetPane(*args, **kwargs):
        """SetPane(self, PaneInfo p)"""
        return _aui.FrameManagerEvent_SetPane(*args, **kwargs)

    def SetButton(*args, **kwargs):
        """SetButton(self, int b)"""
        return _aui.FrameManagerEvent_SetButton(*args, **kwargs)

    def SetDC(*args, **kwargs):
        """SetDC(self, DC pdc)"""
        return _aui.FrameManagerEvent_SetDC(*args, **kwargs)

    def GetPane(*args, **kwargs):
        """GetPane(self) -> PaneInfo"""
        return _aui.FrameManagerEvent_GetPane(*args, **kwargs)

    def GetButton(*args, **kwargs):
        """GetButton(self) -> int"""
        return _aui.FrameManagerEvent_GetButton(*args, **kwargs)

    def GetDC(*args, **kwargs):
        """GetDC(self) -> DC"""
        return _aui.FrameManagerEvent_GetDC(*args, **kwargs)

    def Veto(*args, **kwargs):
        """Veto(self, bool veto=True)"""
        return _aui.FrameManagerEvent_Veto(*args, **kwargs)

    def GetVeto(*args, **kwargs):
        """GetVeto(self) -> bool"""
        return _aui.FrameManagerEvent_GetVeto(*args, **kwargs)

    def SetCanVeto(*args, **kwargs):
        """SetCanVeto(self, bool can_veto)"""
        return _aui.FrameManagerEvent_SetCanVeto(*args, **kwargs)

    def CanVeto(*args, **kwargs):
        """CanVeto(self) -> bool"""
        return _aui.FrameManagerEvent_CanVeto(*args, **kwargs)

    pane = property(_aui.FrameManagerEvent_pane_get, _aui.FrameManagerEvent_pane_set)
    button = property(_aui.FrameManagerEvent_button_get, _aui.FrameManagerEvent_button_set)
    veto_flag = property(_aui.FrameManagerEvent_veto_flag_get, _aui.FrameManagerEvent_veto_flag_set)
    canveto_flag = property(_aui.FrameManagerEvent_canveto_flag_get, _aui.FrameManagerEvent_canveto_flag_set)
    dc = property(_aui.FrameManagerEvent_dc_get, _aui.FrameManagerEvent_dc_set)
    Button = property(GetButton,SetButton,doc="See `GetButton` and `SetButton`") 
    DC = property(GetDC,SetDC,doc="See `GetDC` and `SetDC`") 
    Pane = property(GetPane,SetPane,doc="See `GetPane` and `SetPane`") 
_aui.FrameManagerEvent_swigregister(FrameManagerEvent)

class DockInfo(object):
    """Proxy of C++ DockInfo class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """__init__(self) -> DockInfo"""
        _aui.DockInfo_swiginit(self,_aui.new_DockInfo(*args, **kwargs))
    def IsOk(*args, **kwargs):
        """IsOk(self) -> bool"""
        return _aui.DockInfo_IsOk(*args, **kwargs)

    def IsHorizontal(*args, **kwargs):
        """IsHorizontal(self) -> bool"""
        return _aui.DockInfo_IsHorizontal(*args, **kwargs)

    def IsVertical(*args, **kwargs):
        """IsVertical(self) -> bool"""
        return _aui.DockInfo_IsVertical(*args, **kwargs)

    panes = property(_aui.DockInfo_panes_get, _aui.DockInfo_panes_set)
    rect = property(_aui.DockInfo_rect_get, _aui.DockInfo_rect_set)
    dock_direction = property(_aui.DockInfo_dock_direction_get, _aui.DockInfo_dock_direction_set)
    dock_layer = property(_aui.DockInfo_dock_layer_get, _aui.DockInfo_dock_layer_set)
    dock_row = property(_aui.DockInfo_dock_row_get, _aui.DockInfo_dock_row_set)
    size = property(_aui.DockInfo_size_get, _aui.DockInfo_size_set)
    min_size = property(_aui.DockInfo_min_size_get, _aui.DockInfo_min_size_set)
    resizable = property(_aui.DockInfo_resizable_get, _aui.DockInfo_resizable_set)
    toolbar = property(_aui.DockInfo_toolbar_get, _aui.DockInfo_toolbar_set)
    fixed = property(_aui.DockInfo_fixed_get, _aui.DockInfo_fixed_set)
_aui.DockInfo_swigregister(DockInfo)

class DockUIPart(object):
    """Proxy of C++ DockUIPart class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    typeCaption = _aui.DockUIPart_typeCaption
    typeGripper = _aui.DockUIPart_typeGripper
    typeDock = _aui.DockUIPart_typeDock
    typeDockSizer = _aui.DockUIPart_typeDockSizer
    typePane = _aui.DockUIPart_typePane
    typePaneSizer = _aui.DockUIPart_typePaneSizer
    typeBackground = _aui.DockUIPart_typeBackground
    typePaneBorder = _aui.DockUIPart_typePaneBorder
    typePaneButton = _aui.DockUIPart_typePaneButton
    type = property(_aui.DockUIPart_type_get, _aui.DockUIPart_type_set)
    orientation = property(_aui.DockUIPart_orientation_get, _aui.DockUIPart_orientation_set)
    dock = property(_aui.DockUIPart_dock_get, _aui.DockUIPart_dock_set)
    pane = property(_aui.DockUIPart_pane_get, _aui.DockUIPart_pane_set)
    button = property(_aui.DockUIPart_button_get, _aui.DockUIPart_button_set)
    cont_sizer = property(_aui.DockUIPart_cont_sizer_get, _aui.DockUIPart_cont_sizer_set)
    sizer_item = property(_aui.DockUIPart_sizer_item_get, _aui.DockUIPart_sizer_item_set)
    rect = property(_aui.DockUIPart_rect_get, _aui.DockUIPart_rect_set)
_aui.DockUIPart_swigregister(DockUIPart)

class PaneButton(object):
    """Proxy of C++ PaneButton class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    button_id = property(_aui.PaneButton_button_id_get, _aui.PaneButton_button_id_set)
_aui.PaneButton_swigregister(PaneButton)

wxEVT_AUI_PANEBUTTON = _aui.wxEVT_AUI_PANEBUTTON
wxEVT_AUI_PANECLOSE = _aui.wxEVT_AUI_PANECLOSE
wxEVT_AUI_RENDER = _aui.wxEVT_AUI_RENDER
EVT_AUI_PANEBUTTON = wx.PyEventBinder( wxEVT_AUI_PANEBUTTON )
EVT_AUI_PANECLOSE = wx.PyEventBinder( wxEVT_AUI_PANECLOSE )
EVT_AUI_RENDER = wx.PyEventBinder( wxEVT_AUI_RENDER )

class DockArt(object):
    """
    DockArt is an art provider class which does all of the drawing for
    `FrameManager`.  This allows the library caller to customize or replace the
    dock art and drawing routines by deriving a new class from `PyDockArt`. The
    active dock art class can be set via `FrameManager.SetArtProvider`.

    """
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    __swig_destroy__ = _aui.delete_DockArt
    __del__ = lambda self : None;
    def GetMetric(*args, **kwargs):
        """GetMetric(self, int id) -> int"""
        return _aui.DockArt_GetMetric(*args, **kwargs)

    def SetMetric(*args, **kwargs):
        """SetMetric(self, int id, int new_val)"""
        return _aui.DockArt_SetMetric(*args, **kwargs)

    def SetFont(*args, **kwargs):
        """SetFont(self, int id, Font font)"""
        return _aui.DockArt_SetFont(*args, **kwargs)

    def GetFont(*args, **kwargs):
        """GetFont(self, int id) -> Font"""
        return _aui.DockArt_GetFont(*args, **kwargs)

    def GetColour(*args, **kwargs):
        """GetColour(self, int id) -> Colour"""
        return _aui.DockArt_GetColour(*args, **kwargs)

    def SetColour(*args, **kwargs):
        """SetColour(self, int id, wxColor colour)"""
        return _aui.DockArt_SetColour(*args, **kwargs)

    def GetColor(*args, **kwargs):
        """GetColor(self, int id) -> Colour"""
        return _aui.DockArt_GetColor(*args, **kwargs)

    def SetColor(*args, **kwargs):
        """SetColor(self, int id, Colour color)"""
        return _aui.DockArt_SetColor(*args, **kwargs)

    def DrawSash(*args, **kwargs):
        """DrawSash(self, DC dc, Window window, int orientation, Rect rect)"""
        return _aui.DockArt_DrawSash(*args, **kwargs)

    def DrawBackground(*args, **kwargs):
        """DrawBackground(self, DC dc, Window window, int orientation, Rect rect)"""
        return _aui.DockArt_DrawBackground(*args, **kwargs)

    def DrawCaption(*args, **kwargs):
        """DrawCaption(self, DC dc, Window window, String text, Rect rect, PaneInfo pane)"""
        return _aui.DockArt_DrawCaption(*args, **kwargs)

    def DrawGripper(*args, **kwargs):
        """DrawGripper(self, DC dc, Window window, Rect rect, PaneInfo pane)"""
        return _aui.DockArt_DrawGripper(*args, **kwargs)

    def DrawBorder(*args, **kwargs):
        """DrawBorder(self, DC dc, Window window, Rect rect, PaneInfo pane)"""
        return _aui.DockArt_DrawBorder(*args, **kwargs)

    def DrawPaneButton(*args, **kwargs):
        """
        DrawPaneButton(self, DC dc, Window window, int button, int button_state, 
            Rect rect, PaneInfo pane)
        """
        return _aui.DockArt_DrawPaneButton(*args, **kwargs)

_aui.DockArt_swigregister(DockArt)

class DefaultDockArt(DockArt):
    """
    DefaultDockArt is the type of art class constructed by default for the
    `FrameManager`.
    """
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self) -> DefaultDockArt

        DefaultDockArt is the type of art class constructed by default for the
        `FrameManager`.
        """
        _aui.DefaultDockArt_swiginit(self,_aui.new_DefaultDockArt(*args, **kwargs))
_aui.DefaultDockArt_swigregister(DefaultDockArt)

class FloatingPane(_windows.MiniFrame):
    """Proxy of C++ FloatingPane class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, Window parent, FrameManager owner_mgr, PaneInfo pane, 
            int id=ID_ANY) -> FloatingPane
        """
        _aui.FloatingPane_swiginit(self,_aui.new_FloatingPane(*args, **kwargs))
    __swig_destroy__ = _aui.delete_FloatingPane
    __del__ = lambda self : None;
    def SetPaneWindow(*args, **kwargs):
        """SetPaneWindow(self, PaneInfo pane)"""
        return _aui.FloatingPane_SetPaneWindow(*args, **kwargs)

_aui.FloatingPane_swigregister(FloatingPane)

class AuiNotebookEvent(_core.NotifyEvent):
    """Proxy of C++ AuiNotebookEvent class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """__init__(self, EventType command_type=wxEVT_NULL, int win_id=0) -> AuiNotebookEvent"""
        _aui.AuiNotebookEvent_swiginit(self,_aui.new_AuiNotebookEvent(*args, **kwargs))
    def SetSelection(*args, **kwargs):
        """SetSelection(self, int s)"""
        return _aui.AuiNotebookEvent_SetSelection(*args, **kwargs)

    def SetOldSelection(*args, **kwargs):
        """SetOldSelection(self, int s)"""
        return _aui.AuiNotebookEvent_SetOldSelection(*args, **kwargs)

    def GetSelection(*args, **kwargs):
        """
        GetSelection(self) -> int

        Returns item index for a listbox or choice selection event (not valid
        for a deselection).
        """
        return _aui.AuiNotebookEvent_GetSelection(*args, **kwargs)

    def GetOldSelection(*args, **kwargs):
        """GetOldSelection(self) -> int"""
        return _aui.AuiNotebookEvent_GetOldSelection(*args, **kwargs)

    old_selection = property(_aui.AuiNotebookEvent_old_selection_get, _aui.AuiNotebookEvent_old_selection_set)
    selection = property(_aui.AuiNotebookEvent_selection_get, _aui.AuiNotebookEvent_selection_set)
    OldSelection = property(GetOldSelection,SetOldSelection,doc="See `GetOldSelection` and `SetOldSelection`") 
    Selection = property(GetSelection,SetSelection,doc="See `GetSelection` and `SetSelection`") 
_aui.AuiNotebookEvent_swigregister(AuiNotebookEvent)

class AuiNotebookPage(object):
    """Proxy of C++ AuiNotebookPage class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    window = property(_aui.AuiNotebookPage_window_get, _aui.AuiNotebookPage_window_set)
    caption = property(_aui.AuiNotebookPage_caption_get, _aui.AuiNotebookPage_caption_set)
    bitmap = property(_aui.AuiNotebookPage_bitmap_get, _aui.AuiNotebookPage_bitmap_set)
    rect = property(_aui.AuiNotebookPage_rect_get, _aui.AuiNotebookPage_rect_set)
    active = property(_aui.AuiNotebookPage_active_get, _aui.AuiNotebookPage_active_set)
_aui.AuiNotebookPage_swigregister(AuiNotebookPage)

class AuiTabContainerButton(object):
    """Proxy of C++ AuiTabContainerButton class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    id = property(_aui.AuiTabContainerButton_id_get, _aui.AuiTabContainerButton_id_set)
    cur_state = property(_aui.AuiTabContainerButton_cur_state_get, _aui.AuiTabContainerButton_cur_state_set)
    bitmap = property(_aui.AuiTabContainerButton_bitmap_get, _aui.AuiTabContainerButton_bitmap_set)
    rect = property(_aui.AuiTabContainerButton_rect_get, _aui.AuiTabContainerButton_rect_set)
_aui.AuiTabContainerButton_swigregister(AuiTabContainerButton)

class AuiTabContainer(object):
    """Proxy of C++ AuiTabContainer class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """__init__(self) -> AuiTabContainer"""
        _aui.AuiTabContainer_swiginit(self,_aui.new_AuiTabContainer(*args, **kwargs))
    __swig_destroy__ = _aui.delete_AuiTabContainer
    __del__ = lambda self : None;
    def AddPage(*args, **kwargs):
        """AddPage(self, Window page, AuiNotebookPage info) -> bool"""
        return _aui.AuiTabContainer_AddPage(*args, **kwargs)

    def InsertPage(*args, **kwargs):
        """InsertPage(self, Window page, AuiNotebookPage info, size_t idx) -> bool"""
        return _aui.AuiTabContainer_InsertPage(*args, **kwargs)

    def RemovePage(*args, **kwargs):
        """RemovePage(self, Window page) -> bool"""
        return _aui.AuiTabContainer_RemovePage(*args, **kwargs)

    def SetActivePage(*args):
        """
        SetActivePage(self, Window page) -> bool
        SetActivePage(self, size_t page) -> bool
        """
        return _aui.AuiTabContainer_SetActivePage(*args)

    def SetNoneActive(*args, **kwargs):
        """SetNoneActive(self)"""
        return _aui.AuiTabContainer_SetNoneActive(*args, **kwargs)

    def GetActivePage(*args, **kwargs):
        """GetActivePage(self) -> int"""
        return _aui.AuiTabContainer_GetActivePage(*args, **kwargs)

    def TabHitTest(*args, **kwargs):
        """TabHitTest(self, int x, int y, Window hit) -> bool"""
        return _aui.AuiTabContainer_TabHitTest(*args, **kwargs)

    def ButtonHitTest(*args, **kwargs):
        """ButtonHitTest(self, int x, int y, AuiTabContainerButton hit) -> bool"""
        return _aui.AuiTabContainer_ButtonHitTest(*args, **kwargs)

    def GetWindowFromIdx(*args, **kwargs):
        """GetWindowFromIdx(self, size_t idx) -> Window"""
        return _aui.AuiTabContainer_GetWindowFromIdx(*args, **kwargs)

    def GetIdxFromWindow(*args, **kwargs):
        """GetIdxFromWindow(self, Window page) -> int"""
        return _aui.AuiTabContainer_GetIdxFromWindow(*args, **kwargs)

    def GetPageCount(*args, **kwargs):
        """GetPageCount(self) -> size_t"""
        return _aui.AuiTabContainer_GetPageCount(*args, **kwargs)

    def GetPage(*args, **kwargs):
        """GetPage(self, size_t idx) -> AuiNotebookPage"""
        return _aui.AuiTabContainer_GetPage(*args, **kwargs)

    def GetPages(*args, **kwargs):
        """GetPages(self) -> wxAuiNotebookPageArray"""
        return _aui.AuiTabContainer_GetPages(*args, **kwargs)

    def SetNormalFont(*args, **kwargs):
        """SetNormalFont(self, Font normal_font)"""
        return _aui.AuiTabContainer_SetNormalFont(*args, **kwargs)

    def SetSelectedFont(*args, **kwargs):
        """SetSelectedFont(self, Font selected_font)"""
        return _aui.AuiTabContainer_SetSelectedFont(*args, **kwargs)

    def SetMeasuringFont(*args, **kwargs):
        """SetMeasuringFont(self, Font measuring_font)"""
        return _aui.AuiTabContainer_SetMeasuringFont(*args, **kwargs)

    def DoShowHide(*args, **kwargs):
        """DoShowHide(self)"""
        return _aui.AuiTabContainer_DoShowHide(*args, **kwargs)

    def SetRect(*args, **kwargs):
        """SetRect(self, Rect rect)"""
        return _aui.AuiTabContainer_SetRect(*args, **kwargs)

    def AddButton(*args, **kwargs):
        """AddButton(self, int id, Bitmap bmp)"""
        return _aui.AuiTabContainer_AddButton(*args, **kwargs)

    ActivePage = property(GetActivePage,SetActivePage,doc="See `GetActivePage` and `SetActivePage`") 
    PageCount = property(GetPageCount,doc="See `GetPageCount`") 
    Pages = property(GetPages,doc="See `GetPages`") 
_aui.AuiTabContainer_swigregister(AuiTabContainer)

class AuiTabCtrl(_core.Control,AuiTabContainer):
    """Proxy of C++ AuiTabCtrl class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, Window parent, int id=ID_ANY, Point pos=DefaultPosition, 
            Size size=DefaultSize, long style=0) -> AuiTabCtrl
        """
        _aui.AuiTabCtrl_swiginit(self,_aui.new_AuiTabCtrl(*args, **kwargs))
        self._setOORInfo(self)

_aui.AuiTabCtrl_swigregister(AuiTabCtrl)

class AuiMultiNotebook(_core.Control):
    """Proxy of C++ AuiMultiNotebook class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, Window parent, int id=ID_ANY, Point pos=DefaultPosition, 
            Size size=DefaultSize, long style=0) -> AuiMultiNotebook
        """
        _aui.AuiMultiNotebook_swiginit(self,_aui.new_AuiMultiNotebook(*args, **kwargs))
        self._setOORInfo(self)

    def Create(*args, **kwargs):
        """
        Create(self, Window parent, int id=ID_ANY, Point pos=DefaultPosition, 
            Size size=DefaultSize, long style=0) -> bool

        Do the 2nd phase and create the GUI control.
        """
        return _aui.AuiMultiNotebook_Create(*args, **kwargs)

    def AddPage(*args, **kwargs):
        """AddPage(self, Window page, String caption, bool select=False, Bitmap bitmap=wxNullBitmap) -> bool"""
        return _aui.AuiMultiNotebook_AddPage(*args, **kwargs)

    def InsertPage(*args, **kwargs):
        """
        InsertPage(self, size_t page_idx, Window page, String caption, bool select=False, 
            Bitmap bitmap=wxNullBitmap) -> bool
        """
        return _aui.AuiMultiNotebook_InsertPage(*args, **kwargs)

    def DeletePage(*args, **kwargs):
        """DeletePage(self, size_t page) -> bool"""
        return _aui.AuiMultiNotebook_DeletePage(*args, **kwargs)

    def RemovePage(*args, **kwargs):
        """RemovePage(self, size_t page) -> bool"""
        return _aui.AuiMultiNotebook_RemovePage(*args, **kwargs)

    def SetPageText(*args, **kwargs):
        """SetPageText(self, size_t page, String text) -> bool"""
        return _aui.AuiMultiNotebook_SetPageText(*args, **kwargs)

    def SetSelection(*args, **kwargs):
        """SetSelection(self, size_t new_page) -> size_t"""
        return _aui.AuiMultiNotebook_SetSelection(*args, **kwargs)

    def GetSelection(*args, **kwargs):
        """GetSelection(self) -> int"""
        return _aui.AuiMultiNotebook_GetSelection(*args, **kwargs)

    def GetPageCount(*args, **kwargs):
        """GetPageCount(self) -> size_t"""
        return _aui.AuiMultiNotebook_GetPageCount(*args, **kwargs)

    def GetPage(*args, **kwargs):
        """GetPage(self, size_t page_idx) -> Window"""
        return _aui.AuiMultiNotebook_GetPage(*args, **kwargs)

    PageCount = property(GetPageCount,doc="See `GetPageCount`") 
    Selection = property(GetSelection,SetSelection,doc="See `GetSelection` and `SetSelection`") 
_aui.AuiMultiNotebook_swigregister(AuiMultiNotebook)

def PreAuiMultiNotebook(*args, **kwargs):
    """PreAuiMultiNotebook() -> AuiMultiNotebook"""
    val = _aui.new_PreAuiMultiNotebook(*args, **kwargs)
    self._setOORInfo(self)
    return val

wxEVT_COMMAND_AUINOTEBOOK_PAGE_CHANGED = _aui.wxEVT_COMMAND_AUINOTEBOOK_PAGE_CHANGED
wxEVT_COMMAND_AUINOTEBOOK_PAGE_CHANGING = _aui.wxEVT_COMMAND_AUINOTEBOOK_PAGE_CHANGING
wxEVT_COMMAND_AUINOTEBOOK_BUTTON = _aui.wxEVT_COMMAND_AUINOTEBOOK_BUTTON
wxEVT_COMMAND_AUINOTEBOOK_BEGIN_DRAG = _aui.wxEVT_COMMAND_AUINOTEBOOK_BEGIN_DRAG
wxEVT_COMMAND_AUINOTEBOOK_END_DRAG = _aui.wxEVT_COMMAND_AUINOTEBOOK_END_DRAG
wxEVT_COMMAND_AUINOTEBOOK_DRAG_MOTION = _aui.wxEVT_COMMAND_AUINOTEBOOK_DRAG_MOTION
EVT_AUINOTEBOOK_PAGE_CHANGED = wx.PyEventBinder( wxEVT_COMMAND_AUINOTEBOOK_PAGE_CHANGED, 1 )
EVT_AUINOTEBOOK_PAGE_CHANGING = wx.PyEventBinder( wxEVT_COMMAND_AUINOTEBOOK_PAGE_CHANGING, 1 )
EVT_AUINOTEBOOK_BUTTON = wx.PyEventBinder( wxEVT_COMMAND_AUINOTEBOOK_BUTTON, 1 )
EVT_AUINOTEBOOK_BEGIN_DRAG = wx.PyEventBinder( wxEVT_COMMAND_AUINOTEBOOK_BEGIN_DRAG, 1 )
EVT_AUINOTEBOOK_END_DRAG = wx.PyEventBinder( wxEVT_COMMAND_AUINOTEBOOK_END_DRAG, 1 )
EVT_AUINOTEBOOK_DRAG_MOTION = wx.PyEventBinder( wxEVT_COMMAND_AUINOTEBOOK_DRAG_MOTION, 1 )

class PyDockArt(DefaultDockArt):
    """
    This version of the `DockArt` class has been instrumented to be
    subclassable in Python and to reflect all calls to the C++ base class
    methods to the Python methods implemented in the derived class.
    """
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
_aui.PyDockArt_swigregister(PyDockArt)



