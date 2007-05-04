# Name:         tools.py
# Purpose:      XRC editor, toolbar
# Author:       Roman Rolinsky <rolinsky@mema.ucl.ac.be>
# Created:      19.03.2003
# RCS-ID:       $Id$

from xxx import *                       # xxx imports globals and params
from tree import ID_NEW

# Icons
import images

# Groups of controls
GROUPNUM = 4
GROUP_WINDOWS, GROUP_MENUS, GROUP_SIZERS, GROUP_CONTROLS = range(GROUPNUM)

# States depending on current selection and Control/Shift keys
STATE_ROOT, STATE_MENUBAR, STATE_TOOLBAR, STATE_MENU, STATE_STDDLGBTN, STATE_ELSE = range(6)
# Left toolbar for GUI elements
class Tools(wx.Panel):
    TOOL_SIZE = (30, 30)
    def __init__(self, parent):
        if wx.Platform == '__WXGTK__':
            wx.Panel.__init__(self, parent, -1,
                             style=wx.RAISED_BORDER|wx.WANTS_CHARS)
        else:
            wx.Panel.__init__(self, parent, -1, style=wx.WANTS_CHARS)
        # Create sizer for groups
        self.sizer = wx.BoxSizer(wx.VERTICAL)
        # Data to create buttons
        pullDownMenu = g.pullDownMenu
        self.groups = []
        self.ctrl = self.shift = False
        # Current state (what to enable/disable)
        self.state = None
        groups = [
            ["Windows",
             (ID_NEW.FRAME, images.getToolFrameBitmap()),
             (ID_NEW.DIALOG, images.getToolDialogBitmap()),
             (ID_NEW.PANEL, images.getToolPanelBitmap())],
            ["Menus",
             (ID_NEW.TOOL_BAR, images.getToolToolBarBitmap()),
             (ID_NEW.MENU_BAR, images.getToolMenuBarBitmap()),
             (ID_NEW.MENU, images.getToolMenuBitmap()),
             (ID_NEW.TOOL, images.getToolToolBitmap()),
             (ID_NEW.MENU_ITEM, images.getToolMenuItemBitmap()),
             (ID_NEW.SEPARATOR, images.getToolSeparatorBitmap())],
            ["Sizers",
             (ID_NEW.BOX_SIZER, images.getToolBoxSizerBitmap()),
             (ID_NEW.STATIC_BOX_SIZER, images.getToolStaticBoxSizerBitmap()),
             (ID_NEW.GRID_SIZER, images.getToolGridSizerBitmap()),
             (ID_NEW.FLEX_GRID_SIZER, images.getToolFlexGridSizerBitmap()),
             (ID_NEW.GRID_BAG_SIZER, images.getToolGridBagSizerBitmap()),
             (ID_NEW.SPACER, images.getToolSpacerBitmap())],
            ["Controls",
             (ID_NEW.STATIC_TEXT, images.getToolStaticTextBitmap()),
             (ID_NEW.STATIC_BITMAP, images.getToolStaticBitmapBitmap()),
             (ID_NEW.STATIC_LINE, images.getToolStaticLineBitmap()),
             
             (ID_NEW.BUTTON, images.getToolButtonBitmap()),
             (ID_NEW.BITMAP_BUTTON, images.getToolBitmapButtonBitmap()),
             (ID_NEW.STATIC_BOX, images.getToolStaticBoxBitmap()),
             
             (ID_NEW.TEXT_CTRL, images.getToolTextCtrlBitmap()),
             (ID_NEW.COMBO_BOX, images.getToolComboBoxBitmap()),
             (ID_NEW.CHOICE, images.getToolChoiceBitmap()),
             
             (ID_NEW.RADIO_BUTTON, images.getToolRadioButtonBitmap()),
             (ID_NEW.CHECK_BOX, images.getToolCheckBoxBitmap()),
             (ID_NEW.RADIO_BOX, images.getToolRadioBoxBitmap()),
             
             (ID_NEW.SPIN_CTRL, images.getToolSpinCtrlBitmap()),
             (ID_NEW.SPIN_BUTTON, images.getToolSpinButtonBitmap()),
             (ID_NEW.SCROLL_BAR, images.getToolScrollBarBitmap()),

             (ID_NEW.SLIDER, images.getToolSliderBitmap()),
             (ID_NEW.GAUGE, images.getToolGaugeBitmap()),
             (ID_NEW.TREE_CTRL, images.getToolTreeCtrlBitmap()),
             
             (ID_NEW.LIST_BOX, images.getToolListBoxBitmap()),
             (ID_NEW.CHECK_LIST, images.getToolCheckListBitmap()),
             (ID_NEW.LIST_CTRL, images.getToolListCtrlBitmap()),
             
             (ID_NEW.NOTEBOOK, images.getToolNotebookBitmap()),
             (ID_NEW.SPLITTER_WINDOW, images.getToolSplitterWindowBitmap()),

             (ID_NEW.UNKNOWN, images.getToolUnknownBitmap())]
            ]
        self.boxes = {}
        for grp in groups:
            self.AddGroup(grp[0])
            for b in grp[1:]:
                self.AddButton(b[0], b[1], g.pullDownMenu.createMap[b[0]])
        self.SetSizerAndFit(self.sizer)
        # Allow to be resized in vertical direction only
        self.SetSizeHints(self.GetSize()[0], -1)
        # Events
        wx.EVT_COMMAND_RANGE(self, ID_NEW.PANEL, ID_NEW.LAST,
                             wx.wxEVT_COMMAND_BUTTON_CLICKED, g.frame.OnCreate)
        wx.EVT_KEY_DOWN(self, self.OnKeyDown)
        wx.EVT_KEY_UP(self, self.OnKeyUp)
        self.Bind(wx.EVT_LEFT_DOWN, self.OnClickBox)

        self.drag = None

    def AddButton(self, id, image, text):
        from wx.lib import buttons
        button = buttons.GenBitmapButton(self, id, image, size=self.TOOL_SIZE,
                                         style=wx.NO_BORDER|wx.WANTS_CHARS)
        button.SetBezelWidth(0)
        wx.EVT_KEY_DOWN(button, self.OnKeyDown)
        wx.EVT_KEY_UP(button, self.OnKeyUp)
        wx.EVT_LEFT_DOWN(button, self.OnLeftDownOnButton)
        wx.EVT_MOTION(button, self.OnMotionOnButton)
        button.SetToolTipString(text)
        self.curSizer.Add(button)
        self.groups[-1][1][id] = button

    def AddGroup(self, name):
        # Each group is inside box
        id = wx.NewId()
        box = wx.StaticBox(self, id, '[+] '+name, style=wx.WANTS_CHARS)
        box.SetForegroundColour(wx.Colour(64, 64, 64))
#        box.SetFont(g.smallerFont())
        box.show = True
        box.name = name
        box.gnum = len(self.groups)
        box.Bind(wx.EVT_LEFT_DOWN, self.OnClickBox)
        boxSizer = wx.StaticBoxSizer(box, wx.VERTICAL)
        boxSizer.Add((0, 4))
        self.boxes[id] = box
        self.curSizer = wx.GridSizer(0, 3)
        boxSizer.Add(self.curSizer)
        self.sizer.Add(boxSizer, 0, wx.TOP | wx.LEFT | wx.RIGHT | wx.EXPAND, 4)
        self.groups.append((box,{}))

    # Enable/disable group
    def EnableGroup(self, gnum, enable = True):
        grp = self.groups[gnum]
        #grp[0].Enable(enable)
        for b in grp[1].values(): b.Enable(enable)

    # Show/hide group
    def ShowGroup(self, gnum, show = True):
        grp = self.groups[gnum]
        grp[0].show = show
        for b in grp[1].values(): b.Show(show)

    # Enable/disable group item
    def EnableGroupItem(self, gnum, id, enable = True):
        grp = self.groups[gnum]
        grp[1][id].Enable(enable)

    # Enable/disable group items
    def EnableGroupItems(self, gnum, ids, enable = True):
        grp = self.groups[gnum]
        for id in ids:
            grp[1][id].Enable(enable)

    def OnClickBox(self, evt):
        if wx.Platform == '__WXMSW__':
            box = None
            for id,b in self.boxes.items():
                # Detect click on label
                if b.GetRect().Inside(evt.GetPosition()):
                    box = b
                    break
            if not box: return
        else:
            box = self.boxes[evt.GetId()]
        # Collapse/restore static box, change label
        self.ShowGroup(box.gnum, not box.show)
        if box.show: box.SetLabel('[+] ' + box.name)
        else: box.SetLabel('[-] ' + box.name)
        self.Layout()
        self.Refresh()
        #for b in self.boxes.items():

    # DaD
    def OnLeftDownOnButton(self, evt):
        self.posDown = evt.GetPosition()
        self.idDown = evt.GetId()
        self.btnDown = evt.GetEventObject()
        evt.Skip()

    def OnMotionOnButton(self, evt):
        # Detect dragging
        if evt.Dragging() and evt.LeftIsDown():
            d = evt.GetPosition() - self.posDown
            if max(abs(d[0]), abs(d[1])) >= 5:
                if self.btnDown.HasCapture(): 
                    # Generate up event to release mouse
                    evt = wx.MouseEvent(wx.EVT_LEFT_UP.typeId)
                    evt.SetId(self.idDown)
                    # Set flag to prevent normal button operation this time
                    self.drag = True
                    self.btnDown.ProcessEvent(evt)
                self.StartDrag()
        evt.Skip()

    def StartDrag(self):
        do = MyDataObject()
        do.SetData(str(self.idDown))
        bm = self.btnDown.GetBitmapLabel()
        # wxGTK requires wxIcon cursor, wxWIN and wxMAC require wxCursor
        if wx.Platform == '__WXGTK__':
            icon = wx.EmptyIcon()
            icon.CopyFromBitmap(bm)
            dragSource = wx.DropSource(self, icon)
        else:
            curs = wx.CursorFromImage(wx.ImageFromBitmap(bm))
            dragSource = wx.DropSource(self, curs)
        dragSource.SetData(do)
        g.frame.SetStatusText('Release the mouse button over the test window')
        dragSource.DoDragDrop()

    # Process key events
    def OnKeyDown(self, evt):
        if evt.GetKeyCode() == wx.WXK_CONTROL:
            g.tree.ctrl = True
        elif evt.GetKeyCode() == wx.WXK_SHIFT:
            g.tree.shift = True
        self.UpdateIfNeeded()
        evt.Skip()

    def OnKeyUp(self, evt):
        if evt.GetKeyCode() == wx.WXK_CONTROL:
            g.tree.ctrl = False
        elif evt.GetKeyCode() == wx.WXK_SHIFT:
            g.tree.shift = False
        self.UpdateIfNeeded()
        evt.Skip()

    def OnMouse(self, evt):
        # Update control and shift states
        g.tree.ctrl = evt.ControlDown()
        g.tree.shift = evt.ShiftDown()
        self.UpdateIfNeeded()
        evt.Skip()

    # Update UI after key presses, if necessary
    def UpdateIfNeeded(self):
        tree = g.tree
        if self.ctrl != tree.ctrl or self.shift != tree.shift:
            # Enabling is needed only for ctrl
            if self.ctrl != tree.ctrl: self.UpdateUI()
            self.ctrl = tree.ctrl
            self.shift = tree.shift
            if tree.ctrl:
                status = 'SBL'
            elif tree.shift:
                status = 'INS'
            else:
                status = ''
            g.frame.SetStatusText(status, 1)

    # Update interface
    def UpdateUI(self):
        if not self.IsShown(): return
        # Update status bar
        pullDownMenu = g.pullDownMenu
        tree = g.tree
        item = tree.selection
        # If nothing selected, disable everything and return
        if not item:
            # Disable everything
            for grp in range(GROUPNUM):
                self.EnableGroup(grp, False)
            self.state = None
            return
        if tree.ctrl: needInsert = True
        else: needInsert = tree.NeedInsert(item)
        # Enable depending on selection
        if item == tree.root or needInsert and tree.GetItemParent(item) == tree.root:
            state = STATE_ROOT
        else:            
            xxx = tree.GetPyData(item).treeObject()
            # Check parent for possible child nodes if inserting sibling
            if needInsert: xxx = xxx.parent
            if xxx.__class__ == xxxMenuBar:
                state = STATE_MENUBAR
            elif xxx.__class__ in [xxxToolBar, xxxTool] or \
                 xxx.__class__ == xxxSeparator and xxx.parent.__class__ == xxxToolBar:
                state = STATE_TOOLBAR
            elif xxx.__class__ in [xxxMenu, xxxMenuItem]:
                state = STATE_MENU
            elif xxx.__class__ == xxxStdDialogButtonSizer:
                state = STATE_STDDLGBTN
            else:
                state = STATE_ELSE

        # Enable depending on selection
        if state != self.state:
            # Disable everything
            for grp in range(GROUPNUM):
                self.EnableGroup(grp, False)
            # Enable some
            if state == STATE_ROOT:
                self.EnableGroup(GROUP_WINDOWS, True)
                self.EnableGroup(GROUP_MENUS, True)
                # But disable items
                self.EnableGroupItems(GROUP_MENUS,
                                      [ ID_NEW.TOOL,
                                        ID_NEW.MENU_ITEM,
                                        ID_NEW.SEPARATOR ],
                                      False)
            elif state == STATE_STDDLGBTN:
                pass                    # nothing can be added from toolbar
            elif state == STATE_MENUBAR:
                self.EnableGroup(GROUP_MENUS)
                self.EnableGroupItems(GROUP_MENUS,
                                      [ ID_NEW.TOOL_BAR,
                                        ID_NEW.MENU_BAR,
                                        ID_NEW.TOOL ],
                                      False)
            elif state == STATE_TOOLBAR:
                self.EnableGroup(GROUP_MENUS)
                self.EnableGroupItems(GROUP_MENUS,
                                      [ ID_NEW.TOOL_BAR,
                                        ID_NEW.MENU,
                                        ID_NEW.MENU_BAR,
                                        ID_NEW.MENU_ITEM ],
                                      False)
                self.EnableGroup(GROUP_CONTROLS)
                self.EnableGroupItems(GROUP_CONTROLS,
                                      [ ID_NEW.TREE_CTRL,
                                        ID_NEW.NOTEBOOK,
                                        ID_NEW.SPLITTER_WINDOW ],
                                      False)
            elif state == STATE_MENU:
                self.EnableGroup(GROUP_MENUS)
                self.EnableGroupItems(GROUP_MENUS,
                                      [ ID_NEW.TOOL_BAR,
                                        ID_NEW.MENU_BAR,
                                        ID_NEW.TOOL ],
                                      False)
            else:
                self.EnableGroup(GROUP_WINDOWS)
                self.EnableGroupItems(GROUP_WINDOWS,
                                      [ ID_NEW.FRAME,
                                        ID_NEW.DIALOG ],
                                      False)
                self.EnableGroup(GROUP_MENUS)
                self.EnableGroupItems(GROUP_MENUS,
                                      [ ID_NEW.MENU_BAR,
                                        ID_NEW.MENU_BAR,
                                        ID_NEW.MENU,
                                        ID_NEW.MENU_ITEM,
                                        ID_NEW.TOOL,
                                        ID_NEW.SEPARATOR ],
                                      False)
                self.EnableGroup(GROUP_SIZERS)
                self.EnableGroup(GROUP_CONTROLS)
        # Special case for *book (always executed)
        if state == STATE_ELSE:
            if xxx.__class__ in [xxxNotebook, xxxChoicebook, xxxListbook]:
                self.EnableGroup(GROUP_SIZERS, False)
            else:
                self.EnableGroup(GROUP_SIZERS)
                if not (xxx.isSizer or xxx.parent and xxx.parent.isSizer):
                    self.EnableGroupItem(GROUP_SIZERS, ID_NEW.SPACER, False)
            if xxx.__class__ == xxxFrame:
                self.EnableGroupItem(GROUP_MENUS, ID_NEW.MENU_BAR)
        # Save state
        self.state = state
