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
STATE_ROOT, STATE_MENUBAR, STATE_TOOLBAR, STATE_MENU, STATE_ELSE = range(5)

# Left toolbar for GUI elements
class Tools(wxPanel):
    TOOL_SIZE = (30, 30)
    def __init__(self, parent):
        if wxPlatform == '__WXGTK__':
            wxPanel.__init__(self, parent, -1,
                             style=wxRAISED_BORDER|wxWANTS_CHARS)
        else:
            wxPanel.__init__(self, parent, -1, style=wxWANTS_CHARS)
        # Create sizer for groups
        self.sizer = wxBoxSizer(wxVERTICAL)
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
        for grp in groups:
            self.AddGroup(grp[0])
            for b in grp[1:]:
                self.AddButton(b[0], b[1], g.pullDownMenu.createMap[b[0]])
        self.SetAutoLayout(True)
        self.SetSizerAndFit(self.sizer)
        # Allow to be resized in vertical direction only
        self.SetSizeHints(self.GetSize()[0], -1)
        # Events
        EVT_COMMAND_RANGE(self, ID_NEW.PANEL, ID_NEW.LAST,
                          wxEVT_COMMAND_BUTTON_CLICKED, g.frame.OnCreate)
        EVT_KEY_DOWN(self, self.OnKeyDown)
        EVT_KEY_UP(self, self.OnKeyUp)

    def AddButton(self, id, image, text):
        from wxPython.lib import buttons
        button = buttons.wxGenBitmapButton(self, id, image, size=self.TOOL_SIZE,
                                           style=wxNO_BORDER|wxWANTS_CHARS)
        button.SetBezelWidth(0)
        EVT_KEY_DOWN(button, self.OnKeyDown)
        EVT_KEY_UP(button, self.OnKeyUp)
        button.SetToolTipString(text)
        self.curSizer.Add(button)
        self.groups[-1][1][id] = button

    def AddGroup(self, name):
        # Each group is inside box
        box = wxStaticBox(self, -1, name, style=wxWANTS_CHARS)
        box.SetFont(g.smallerFont())
        boxSizer = wxStaticBoxSizer(box, wxVERTICAL)
        boxSizer.Add((0, 4))
        self.curSizer = wxGridSizer(0, 3)
        boxSizer.Add(self.curSizer)
        self.sizer.Add(boxSizer, 0, wxTOP | wxLEFT | wxRIGHT, 4)
        self.groups.append((box,{}))

    # Enable/disable group
    def EnableGroup(self, gnum, enable = True):
        grp = self.groups[gnum]
        grp[0].Enable(enable)
        for b in grp[1].values(): b.Enable(enable)

    # Enable/disable group item
    def EnableGroupItem(self, gnum, id, enable = True):
        grp = self.groups[gnum]
        grp[1][id].Enable(enable)

    # Enable/disable group items
    def EnableGroupItems(self, gnum, ids, enable = True):
        grp = self.groups[gnum]
        for id in ids:
            grp[1][id].Enable(enable)

    # Process key events
    def OnKeyDown(self, evt):
        if evt.GetKeyCode() == WXK_CONTROL:
            g.tree.ctrl = True
        elif evt.GetKeyCode() == WXK_SHIFT:
            g.tree.shift = True
        self.UpdateIfNeeded()
        evt.Skip()

    def OnKeyUp(self, evt):
        if evt.GetKeyCode() == WXK_CONTROL:
            g.tree.ctrl = False
        elif evt.GetKeyCode() == WXK_SHIFT:
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
        # Special case for notebook (always executed)
        if state == STATE_ELSE:
            if xxx.__class__ == xxxNotebook:
                self.EnableGroup(GROUP_SIZERS, False)
            else:
                self.EnableGroup(GROUP_SIZERS)
                if not (xxx.isSizer or xxx.parent and xxx.parent.isSizer):
                    self.EnableGroupItem(GROUP_SIZERS, ID_NEW.SPACER, False)
        # Save state
        self.state = state
