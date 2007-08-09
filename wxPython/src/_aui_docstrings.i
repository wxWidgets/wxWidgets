/////////////////////////////////////////////////////////////////////////////
// Name:        _aui_docstrings.i
// Purpose:     Docstrings for the wxAUI classes.  These are in a separate
//              file because we have SWIG scan the .h files directly.
//
// Author:      Robin Dunn
//
// Created:     7-July-2006
// RCS-ID:      $Id$
// Copyright:   (c) 2006 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////


DocStr(wxFrameManager,
"FrameManager manages the panes associated with it for a particular
`wx.Frame`, using a pane's `PaneInfo` information to determine each
pane's docking and floating behavior. FrameManager uses wxWidgets'
sizer mechanism to plan the layout of each frame. It uses a
replaceable `DockArt` class to do all drawing, so all drawing is
localized in one area, and may be customized depending on an
application's specific needs.
",

"
FrameManager works as follows: The programmer adds panes to the
class, or makes changes to existing pane properties (dock position,
floating state, show state, etc.). To apply these changes,
FrameManager's `Update` function is called. This batch processing
can be used to avoid flicker, by modifying more than one pane at a
time, and then \"committing\" all of the changes at once by calling
`Update`.

Panes can be added quite easily::

    text1 = wx.TextCtrl(self, -1)
    text2 = wx.TextCtrl(self, -1)
    self._mgr.AddPane(text1, wx.LEFT, \"Pane Caption\")
    self._mgr.AddPane(text2, wx.BOTTOM, \"Pane Caption\")
    self._mgr.Update()


Later on, the positions can be modified easily. The following will
float an existing pane in a tool window::

    self._mgr.GetPane(text1).Float();


**Layers, Rows and Directions, Positions**

Inside wx.aui the docking layout is figured out by checking several
pane parameters. Four of these are important for determining where a
pane will end up:

    * **Direction**: Each docked pane has a direction, Top, Bottom,
      Left, Right, or Center. This is fairly self-explanatory. The
      pane will be placed in the location specified by this variable.

    * **Position**: More than one pane can be placed inside of a
      \"dock.\"  Imagine two panes being docked on the left side of a
      window. One pane can be placed over another.  In proportionally
      managed docks, the pane position indicates it's sequential
      position, starting with zero. So, in our scenario with two panes
      docked on the left side, the top pane in the dock would have
      position 0, and the second one would occupy position 1.

    * **Row**: A row can allow for two docks to be placed next to each
      other. One of the most common places for this to happen is in
      the toolbar. Multiple toolbar rows are allowed, the first row
      being in row 0, and the second in row 1. Rows can also be used
      on vertically docked panes.

    * **Layer**: A layer is akin to an onion. Layer 0 is the very
      center of the managed pane. Thus, if a pane is in layer 0, it
      will be closest to the center window (also sometimes known as
      the \"content window\"). Increasing layers \"swallow up\" all
      layers of a lower value. This can look very similar to multiple
      rows, but is different because all panes in a lower level yield
      to panes in higher levels. The best way to understand layers is
      by running the AUI sample in the wxPython demo.
");

DocStr(wxFrameManager::wxFrameManager,
"Constructor.

    :param managed_wnd: Specifies the `wx.Window` which should be
        managed.  If not set in the call to this constructor then
        `SetManagedWindow` should be called later.

    :param flags: Specifies options which allow the frame management
        behavior to be modified.
",
"
Valid flags are:

    ==============================  =================================
    AUI_MGR_ALLOW_FLOATING          Panes can be undocked and floated
    AUI_MGR_ALLOW_ACTIVE_PANE       The last pane clicked on will be
                                    considered the active pane and will
                                    be highlighted.
    AUI_MGR_TRANSPARENT_DRAG        If the platform supports it the panes
                                    will be partially transparent while
                                    dragging.
    AUI_MGR_TRANSPARENT_HINT        If the platform supports it the
                                    hint used to show where the pane can
                                    be docked will be partially transparent.
    AUI_MGR_TRANSPARENT_HINT_FADE   Should the transparent hint be faded
                                    into view.
    AUI_MGR_DEFAULT                 The default flags.
    ==============================  =================================
");

DocStr(wxFrameManager::UnInit,
"UnInit uninitializes the framework and should be called before a
managed frame is destroyed. UnInit is usually called in the managed
window's destructor.
", "");

DocStr(wxFrameManager::SetFlags,
"SetFlags is used to specify the FrameManager's behavioral
settings. The flags parameter is described in the docs for `__init__`
", "");

DocStr(wxFrameManager::GetFlags,
"GetFlags returns the current FrameManager's flags.
", "");

DocStr(wxFrameManager::SetManagedWindow,
"SetManagedWindow is called to specify the window which is to be
managed by the FrameManager.  It is normally a `wx.Frame` but it is
possible to also allow docking within any container window.  This only
needs to be called if the window was not given to the manager in the
constructor.
", "");

DocStr(wxFrameManager::GetManagedWindow,
"GetManagedWindow returns the window currently being managed by the
FrameManager.
", "");

DocStr(wxFrameManager::SetArtProvider,
"SetArtProvider instructs FrameManager to use the art provider
specified for all drawing calls. This allows plugable look-and-feel
features. The previous art provider object, if any, will be destroyed
by FrameManager.

:note: If you wish to use a custom `DockArt` class to override drawing
    or metrics then you shoudl derive your class from the `PyDockArt`
    class, which has been instrumented for reflecting virtual calls to
    Python methods.
", "");

DocStr(wxFrameManager::GetArtProvider,
"GetArtProvider returns the current art provider being used.
", "");


DocAStr(wxFrameManager::GetAllPanes,
"GetAllPanes(self) -> list",        
"GetAllPanes returns a list of `PaneInfo` objects for all panes managed
by the frame manager.
", "");


DocStr(wxFrameManager::InsertPane,
"InsertPane is used to insert either a previously unmanaged pane window
into the frame manager, or to insert a currently managed pane
somewhere else. InsertPane will push all panes, rows, or docks aside
and insert the window into the position specified by
``insert_location``.  Because ``insert_location`` can specify either a pane,
dock row, or dock layer, the ``insert_level`` parameter is used to
disambiguate this. The parameter ``insert_level`` can take a value of
``AUI_INSERT_PANE``, ``AUI_INSERT_ROW`` or ``AUI_INSERT_DOCK``.
", "");

DocStr(wxFrameManager::DetachPane,
"DetachPane tells the FrameManager to stop managing the pane specified
by window. The window, if in a floated frame, is reparented to the
frame managed by FrameManager.
", "");

DocStr(wxFrameManager::SavePerspective,
"SavePerspective saves the entire user interface layout into an encoded
string, which can then be stored someplace by the application.  When a
perspective is restored using `LoadPerspective`, the entire user
interface will return to the state it was when the perspective was
saved.
", "");

DocStr(wxFrameManager::LoadPerspective,
"LoadPerspective loads a saved perspective. If ``update`` is ``True``,
`Update` is automatically invoked, thus realizing the saved
perspective on screen.
", "");

DocStr(wxFrameManager::Update,
"Update shoudl be called called after any number of changes are made to
any of the managed panes.  Update must be invoked after `AddPane` or
`InsertPane` are called in order to \"realize\" or \"commit\" the
changes. In addition, any number of changes may be made to `PaneInfo`
structures (retrieved with `GetPane` or `GetAllPanes`), but to realize
the changes, Update must be called. This construction allows pane
flicker to be avoided by updating the whole layout at one time.
", "");


//----------------------------------------------------------------------

DocStr(wxPaneInfo,
"PaneInfo specifies all the parameters for a pane for the
`FrameManager`. These parameters specify where the pane is on the
screen, whether it is docked or floating, or hidden. In addition,
these parameters specify the pane's docked position, floating
position, preferred size, minimum size, caption text among many other
parameters.
", "");

DocStr(wxPaneInfo::IsOk,
"IsOk returns ``True`` if the PaneInfo structure is valid.
", "");

DocStr(wxPaneInfo::IsFixed,
"IsFixed returns ``True`` if the pane cannot be resized.
", "");

DocStr(wxPaneInfo::IsResizable,
"IsResizeable returns ``True`` if the pane can be resized.
", "");

DocStr(wxPaneInfo::IsShown,
"IsShown returns ``True`` if the pane should be drawn on the screen.
", "");

DocStr(wxPaneInfo::IsFloating,
"IsFloating returns ``True`` if the pane is floating.
", "");

DocStr(wxPaneInfo::IsDocked,
"IsDocked returns ``True`` if the pane is docked.
", "");

DocStr(wxPaneInfo::IsToolbar,
"IsToolbar returns ``True`` if the pane contains a toolbar.
", "");

DocStr(wxPaneInfo::IsTopDockable,
"IsTopDockable returns ``True`` if the pane can be docked at the top of
the managed frame.
", "");

DocStr(wxPaneInfo::IsBottomDockable,
"IsBottomDockable returns ``True`` if the pane can be docked at the
bottom of the managed frame.
", "");

DocStr(wxPaneInfo::IsLeftDockable,
"IsLeftDockable returns ``True`` if the pane can be docked on the left
of the managed frame.
", "");

DocStr(wxPaneInfo::IsRightDockable,
"IsRightDockable returns ``True`` if the pane can be docked on the
right of the managed frame.
", "");

DocStr(wxPaneInfo::IsFloatable,
"IsFloatable returns ``True`` if the pane can be undocked and displayed
as a floating window.
", "");

DocStr(wxPaneInfo::IsMovable,
"IsMoveable returns ``True`` if the docked frame can be undocked or moved
to another dock position.
", "");

DocStr(wxPaneInfo::HasCaption,
"HasCaption returns ``True`` if the pane displays a caption.
", "");

DocStr(wxPaneInfo::HasGripper,
"HasGripper returns ``True`` if the pane displays a gripper.
", "");

DocStr(wxPaneInfo::HasBorder,
"HasBorder returns ``True`` if the pane displays a border.
", "");

DocStr(wxPaneInfo::HasCloseButton,
"HasCloseButton returns ``True`` if the pane displays a button to close
the pane.
", "");

DocStr(wxPaneInfo::HasMaximizeButton,
"HasMaximizeButton returns ``True`` if the pane displays a button to
maximize the pane.
", "");

DocStr(wxPaneInfo::HasMinimizeButton,
"HasMinimizeButton returns ``True`` if the pane displays a button to
minimize the pane.
", "");

DocStr(wxPaneInfo::HasPinButton,
"HasPinButton returns ``True`` if the pane displays a button to float
the pane.
", "");

DocStr(wxPaneInfo::Name,
"Name sets the name of the pane so it can be referenced in lookup
functions.
", "");

DocStr(wxPaneInfo::Caption,
"Caption sets the caption of the pane.
", "");

DocStr(wxPaneInfo::Left,
"Left sets the pane dock position to the left side of the frame.
", "");

DocStr(wxPaneInfo::Right,
"Right sets the pane dock position to the right side of the frame.
", "");

DocStr(wxPaneInfo::Top,
"Top sets the pane dock position to the top of the frame.
", "");

DocStr(wxPaneInfo::Bottom,
"Bottom sets the pane dock position to the bottom of the frame.
", "");

DocStr(wxPaneInfo::Centre,
"Centre sets the pane to the center position of the frame.
", "");

DocStr(wxPaneInfo::Center,
"Center sets the pane to the center position of the frame.
", "");

DocStr(wxPaneInfo::Direction,
"Direction determines the direction of the docked pane.
", "");

DocStr(wxPaneInfo::Layer,
"Layer determines the layer of the docked pane.
", "");

DocStr(wxPaneInfo::Row,
"Row determines the row of the docked pane.
", "");

DocStr(wxPaneInfo::Position,
"Position determines the position of the docked pane.
", "");



DocStr(wxPaneInfo::MaxSize,
"MaxSize sets the maximum size of the pane.
", "");

DocStr(wxPaneInfo::BestSize,
"BestSize sets the ideal size for the pane.
", "");

DocStr(wxPaneInfo::MinSize,
"MinSize sets the minimum size of the pane.
", "");

DocStr(wxPaneInfo::FloatingPosition,
"FloatingPosition sets the position of the floating pane.
", "");

DocStr(wxPaneInfo::FloatingSize,
"FloatingSize sets the size of the floating pane.
", "");



DocStr(wxPaneInfo::Fixed,
"Fixed forces a pane to be fixed size so that it cannot be resized.
", "");

DocStr(wxPaneInfo::Resizable,
"Resized allows a pane to be resized if resizable is true, and forces
it to be a fixed size if resizeable is false.
", "");

DocStr(wxPaneInfo::Dock,
"Dock indicates that a pane should be docked.
", "");

DocStr(wxPaneInfo::Float,
"Float indicates that a pane should be floated.
", "");

DocStr(wxPaneInfo::Hide,
"Hide indicates that a pane should be hidden.
", "");

DocStr(wxPaneInfo::Show,
"Show indicates that a pane should be shown.
", "");

DocStr(wxPaneInfo::CaptionVisible,
"CaptionVisible indicates that a pane caption should be visible.
", "");

DocStr(wxPaneInfo::PaneBorder,
"PaneBorder indicates that a border should be drawn for the pane.
", "");

DocStr(wxPaneInfo::Gripper,
"Gripper indicates that a gripper should be drawn for the pane..
", "");

DocStr(wxPaneInfo::CloseButton,
"CloseButton indicates that a close button should be drawn for the
pane.
", "");

DocStr(wxPaneInfo::MaximizeButton,
"MaximizeButton indicates that a maximize button should be drawn for
the pane.
", "");

DocStr(wxPaneInfo::MinimizeButton,
"MinimizeButton indicates that a minimize button should be drawn for
the pane.
", "");

DocStr(wxPaneInfo::PinButton,
"PinButton indicates that a pin button should be drawn for the pane.
", "");

DocStr(wxPaneInfo::DestroyOnClose,
"DestroyOnClose indicates whether a pane should be detroyed when it is
closed.
", "");

DocStr(wxPaneInfo::TopDockable,
"TopDockable indicates whether a pane can be docked at the top of the
frame.
", "");

DocStr(wxPaneInfo::BottomDockable,
"BottomDockable indicates whether a pane can be docked at the bottom of
the frame.
", "");

DocStr(wxPaneInfo::LeftDockable,
"LeftDockable indicates whether a pane can be docked on the left of the
frame.
", "");

DocStr(wxPaneInfo::RightDockable,
"RightDockable indicates whether a pane can be docked on the right of
the frame.
", "");

DocStr(wxPaneInfo::Floatable,
"Floatable indicates whether a frame can be floated.
", "");

DocStr(wxPaneInfo::Movable,
"Movable indicates whether a frame can be moved.
", "");

DocStr(wxPaneInfo::Dockable,
"Dockable indicates whether a pane can be docked at any position of the
frame.
", "");

DocStr(wxPaneInfo::DefaultPane,
"DefaultPane specifies that the pane should adopt the default pane
settings.
", "");

DocStr(wxPaneInfo::CentrePane,
"CentrePane specifies that the pane should adopt the default center
pane settings.
", "");

DocStr(wxPaneInfo::CenterPane,
"CenterPane specifies that the pane should adopt the default center
pane settings.
", "");

DocStr(wxPaneInfo::ToolbarPane,
"ToolbarPane specifies that the pane should adopt the default toolbar
pane settings.
", "");

DocStr(wxPaneInfo::SetFlag,
"SetFlag turns the property given by flag on or off with the
option_state parameter.
", "");

DocStr(wxPaneInfo::HasFlag,
"HasFlag returns ``True`` if the the property specified by flag is
active for the pane.
", "");


//---------------------------------------------------------------------------

DocStr(wxDockArt,
"DockArt is an art provider class which does all of the drawing for
`FrameManager`.  This allows the library caller to customize or replace the
dock art and drawing routines by deriving a new class from `PyDockArt`. The
active dock art class can be set via `FrameManager.SetArtProvider`.
", "");

DocStr(wxDefaultDockArt,
"DefaultDockArt is the type of art class constructed by default for the
`FrameManager`.","");
      
