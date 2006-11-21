# Name:         tree.py
# Purpose:      XRC editor, XML_tree class
# Author:       Roman Rolinsky <rolinsky@mema.ucl.ac.be>
# Created:      02.12.2002
# RCS-ID:       $Id$

from xxx import *                       # xxx imports globals and params
import types
import traceback

# Constant to define standart window name
STD_NAME = '_XRCED_T_W'

# Icons
import images

class MemoryFile:
    def __init__(self, name):
        self.name = name
        self.buffer = ''
    def write(self, data):
        if g.currentEncoding:
            encoding = g.currentEncoding
        else:
            encoding = wx.GetDefaultPyEncoding()
        try:
            self.buffer += data.encode(encoding)
        except UnicodeEncodeError:
            self.buffer += data.encode(encoding, 'xmlcharrefreplace')
            
    def close(self):
        wx.MemoryFSHandler.AddFile(self.name, self.buffer)

################################################################################

# Redefine writing to include encoding
class MyDocument(minidom.Document):
    def __init__(self):
        minidom.Document.__init__(self)
        self.encoding = ''
    def writexml(self, writer, indent="", addindent="", newl="", encoding=""):
        if encoding: encdstr = 'encoding="%s"' % encoding
        else: encdstr = ''
        writer.write('<?xml version="1.0" %s?>\n' % encdstr)
        for node in self.childNodes:
            node.writexml(writer, indent, addindent, newl)

################################################################################

# Ids for menu commands
class ID_NEW:
    PANEL = wx.NewId()
    DIALOG = wx.NewId()
    FRAME = wx.NewId()
    TOOL_BAR = wx.NewId()
    TOOL = wx.NewId()
    MENU_BAR = wx.NewId()
    MENU = wx.NewId()
    STATUS_BAR = wx.NewId()

    STATIC_TEXT = wx.NewId()
    TEXT_CTRL = wx.NewId()

    BUTTON = wx.NewId()
    BITMAP_BUTTON = wx.NewId()
    RADIO_BUTTON = wx.NewId()
    SPIN_BUTTON = wx.NewId()
    TOGGLE_BUTTON = wx.NewId()

    STATIC_BOX = wx.NewId()
    CHECK_BOX = wx.NewId()
    RADIO_BOX = wx.NewId()
    COMBO_BOX = wx.NewId()
    LIST_BOX = wx.NewId()

    STATIC_LINE = wx.NewId()
    STATIC_BITMAP = wx.NewId()
    CHOICE = wx.NewId()
    SLIDER = wx.NewId()
    GAUGE = wx.NewId()
    SCROLL_BAR = wx.NewId()
    TREE_CTRL = wx.NewId()
    LIST_CTRL = wx.NewId()
    CHECK_LIST = wx.NewId()
    NOTEBOOK = wx.NewId()
    CHOICEBOOK = wx.NewId()
    LISTBOOK = wx.NewId()
    SPLITTER_WINDOW = wx.NewId()
    SCROLLED_WINDOW = wx.NewId()
    HTML_WINDOW = wx.NewId()
    CALENDAR_CTRL = wx.NewId()
    DATE_CTRL = wx.NewId()
    GENERIC_DIR_CTRL = wx.NewId()
    SPIN_CTRL = wx.NewId()
    UNKNOWN = wx.NewId()
    WIZARD = wx.NewId()
    WIZARD_PAGE = wx.NewId()
    WIZARD_PAGE_SIMPLE = wx.NewId()
    BITMAP = wx.NewId()
    ICON = wx.NewId()
    STATUS_BAR = wx.NewId()

    BOX_SIZER = wx.NewId()
    STATIC_BOX_SIZER = wx.NewId()
    GRID_SIZER = wx.NewId()
    FLEX_GRID_SIZER = wx.NewId()
    GRID_BAG_SIZER = wx.NewId()
    STD_DIALOG_BUTTON_SIZER = wx.NewId()
    SPACER = wx.NewId()
    
    TOOL_BAR = wx.NewId()
    TOOL = wx.NewId()
    MENU = wx.NewId()
    MENU_ITEM = wx.NewId()
    SEPARATOR = wx.NewId()

    OK_BUTTON = wx.NewId()
    YES_BUTTON = wx.NewId()
    SAVE_BUTTON = wx.NewId()
    APPLY_BUTTON = wx.NewId()
    NO_BUTTON = wx.NewId()
    CANCEL_BUTTON = wx.NewId()
    HELP_BUTTON = wx.NewId()
    CONTEXT_HELP_BUTTON = wx.NewId()

    REF = wx.NewId()

    LAST = wx.NewId()

    

class PullDownMenu:
    ID_EXPAND = wx.NewId()
    ID_COLLAPSE = wx.NewId()
    ID_PASTE_SIBLING = wx.NewId()
    ID_TOOL_PASTE = wx.NewId()
    ID_SUBCLASS = wx.NewId()

    def __init__(self, parent):
        self.ID_DELETE = parent.ID_DELETE
        wx.EVT_MENU_RANGE(parent, ID_NEW.PANEL, ID_NEW.LAST, parent.OnCreate)
        wx.EVT_MENU_RANGE(parent, 1000 + ID_NEW.PANEL, 1000 + ID_NEW.LAST, parent.OnReplace)
        wx.EVT_MENU(parent, self.ID_COLLAPSE, parent.OnCollapse)
        wx.EVT_MENU(parent, self.ID_EXPAND, parent.OnExpand)
        wx.EVT_MENU(parent, self.ID_PASTE_SIBLING, parent.OnPaste)
        wx.EVT_MENU(parent, self.ID_SUBCLASS, parent.OnSubclass)
        # We connect to tree, but process in frame
        wx.EVT_MENU_HIGHLIGHT_ALL(g.tree, parent.OnPullDownHighlight)

        # Mapping from IDs to element names
        self.createMap = {
            ID_NEW.PANEL: 'wxPanel',
            ID_NEW.DIALOG: 'wxDialog',
            ID_NEW.FRAME: 'wxFrame',
            ID_NEW.WIZARD: 'wxWizard',
            ID_NEW.WIZARD_PAGE: 'wxWizardPage',
            ID_NEW.WIZARD_PAGE_SIMPLE: 'wxWizardPageSimple',
            ID_NEW.TOOL_BAR: 'wxToolBar',
            ID_NEW.TOOL: 'tool',
            ID_NEW.STATUS_BAR: 'wxStatusBar',
            ID_NEW.MENU_BAR: 'wxMenuBar',
            ID_NEW.MENU: 'wxMenu',
            ID_NEW.MENU_ITEM: 'wxMenuItem',
            ID_NEW.BITMAP: 'wxBitmap',
            ID_NEW.ICON: 'wxIcon',
            ID_NEW.SEPARATOR: 'separator',

            ID_NEW.STATIC_TEXT: 'wxStaticText',
            ID_NEW.TEXT_CTRL: 'wxTextCtrl',

            ID_NEW.BUTTON: 'wxButton',
            ID_NEW.BITMAP_BUTTON: 'wxBitmapButton',
            ID_NEW.RADIO_BUTTON: 'wxRadioButton',
            ID_NEW.SPIN_BUTTON: 'wxSpinButton',
            ID_NEW.TOGGLE_BUTTON: 'wxToggleButton',

            ID_NEW.STATIC_BOX: 'wxStaticBox',
            ID_NEW.CHECK_BOX: 'wxCheckBox',
            ID_NEW.RADIO_BOX: 'wxRadioBox',
            ID_NEW.COMBO_BOX: 'wxComboBox',
            ID_NEW.LIST_BOX: 'wxListBox',
            ID_NEW.CHECK_LIST: 'wxCheckListBox',

            ID_NEW.STATIC_LINE: 'wxStaticLine',
            ID_NEW.STATIC_BITMAP: 'wxStaticBitmap',
            ID_NEW.CHOICE: 'wxChoice',
            ID_NEW.SLIDER: 'wxSlider',
            ID_NEW.GAUGE: 'wxGauge',
            ID_NEW.SCROLL_BAR: 'wxScrollBar',
            ID_NEW.TREE_CTRL: 'wxTreeCtrl',
            ID_NEW.LIST_CTRL: 'wxListCtrl',
            ID_NEW.NOTEBOOK: 'wxNotebook',
            ID_NEW.CHOICEBOOK: 'wxChoicebook',
            ID_NEW.LISTBOOK: 'wxListbook',
            ID_NEW.SPLITTER_WINDOW: 'wxSplitterWindow',
            ID_NEW.SCROLLED_WINDOW: 'wxScrolledWindow',
            ID_NEW.HTML_WINDOW: 'wxHtmlWindow',
            ID_NEW.CALENDAR_CTRL: 'wxCalendarCtrl',
            ID_NEW.DATE_CTRL: 'wxDatePickerCtrl',
            ID_NEW.GENERIC_DIR_CTRL: 'wxGenericDirCtrl',
            ID_NEW.SPIN_CTRL: 'wxSpinCtrl',

            ID_NEW.BOX_SIZER: 'wxBoxSizer',
            ID_NEW.STATIC_BOX_SIZER: 'wxStaticBoxSizer',
            ID_NEW.GRID_SIZER: 'wxGridSizer',
            ID_NEW.FLEX_GRID_SIZER: 'wxFlexGridSizer',
            ID_NEW.GRID_BAG_SIZER: 'wxGridBagSizer',
            ID_NEW.STD_DIALOG_BUTTON_SIZER: 'wxStdDialogButtonSizer',
            ID_NEW.SPACER: 'spacer',
            ID_NEW.UNKNOWN: 'unknown',

            ID_NEW.OK_BUTTON: 'wxButton',
            ID_NEW.YES_BUTTON: 'wxButton',
            ID_NEW.SAVE_BUTTON: 'wxButton',
            ID_NEW.APPLY_BUTTON: 'wxButton',
            ID_NEW.NO_BUTTON: 'wxButton',
            ID_NEW.CANCEL_BUTTON: 'wxButton',
            ID_NEW.HELP_BUTTON: 'wxButton',
            ID_NEW.CONTEXT_HELP_BUTTON: 'wxButton',
            }
        self.topLevel = [
            (ID_NEW.PANEL, 'Panel', 'Create panel'),
            (ID_NEW.DIALOG, 'Dialog', 'Create dialog'),
            (ID_NEW.FRAME, 'Frame', 'Create frame'),
            (ID_NEW.WIZARD, 'Wizard', 'Create wizard'),
            None,
            (ID_NEW.TOOL_BAR, 'ToolBar', 'Create toolbar'),
            (ID_NEW.MENU_BAR, 'MenuBar', 'Create menubar'),
            (ID_NEW.MENU, 'Menu', 'Create menu'),
            None,
            (ID_NEW.BITMAP, 'Bitmap', 'Create bitmap'),
            (ID_NEW.ICON, 'Icon', 'Create icon'),
            ]
        self.containers = [
             (ID_NEW.PANEL, 'Panel', 'Create panel'),
             (ID_NEW.NOTEBOOK, 'Notebook', 'Create notebook control'),
             (ID_NEW.CHOICEBOOK, 'Choicebook', 'Create choicebook control'),
             (ID_NEW.LISTBOOK, 'Listbook', 'Create listbook control'),
             (ID_NEW.SPLITTER_WINDOW, 'SplitterWindow', 'Create splitter window'),
             (ID_NEW.TOOL_BAR, 'ToolBar', 'Create toolbar'),
             (ID_NEW.STATUS_BAR, 'StatusBar', 'Create status bar'),
#             (ID_NEW.WIZARD_PAGE, 'WizardPage', 'Create wizard page'),
             (ID_NEW.WIZARD_PAGE_SIMPLE, 'WizardPageSimple', 'Create simple wizard page'),
            ]
        self.sizers = [
             (ID_NEW.BOX_SIZER, 'BoxSizer', 'Create box sizer'),
             (ID_NEW.STATIC_BOX_SIZER, 'StaticBoxSizer',
              'Create static box sizer'),
             (ID_NEW.GRID_SIZER, 'GridSizer', 'Create grid sizer'),
             (ID_NEW.FLEX_GRID_SIZER, 'FlexGridSizer',
              'Create flexgrid sizer'),
             (ID_NEW.GRID_BAG_SIZER, 'GridBagSizer',
              'Create gridbag sizer'),
#             (ID_NEW.STD_DIALOG_BUTTON_SIZER, 'StdDialogButtonSizer',
#              'Create standard button sizer'),
             (ID_NEW.SPACER, 'Spacer', 'Create spacer'),
             ]
        self.controls = [
            ['control', 'Various controls',
             (ID_NEW.STATIC_TEXT, 'Label', 'Create label'),
             (ID_NEW.STATIC_BITMAP, 'Bitmap', 'Create bitmap'),
             (ID_NEW.STATIC_LINE, 'Line', 'Create line'),
             (ID_NEW.TEXT_CTRL, 'TextBox', 'Create text box'),
             (ID_NEW.CHOICE, 'Choice', 'Create choice'),
             (ID_NEW.SLIDER, 'Slider', 'Create slider'),
             (ID_NEW.GAUGE, 'Gauge', 'Create gauge'),
             (ID_NEW.SPIN_CTRL, 'SpinCtrl', 'Create spin'),
             (ID_NEW.SCROLL_BAR, 'ScrollBar', 'Create scroll bar'),
             (ID_NEW.TREE_CTRL, 'TreeCtrl', 'Create tree'),
             (ID_NEW.LIST_CTRL, 'ListCtrl', 'Create list'),
             (ID_NEW.SCROLLED_WINDOW, 'ScrolledWindow', 'Create scrolled window'),
             (ID_NEW.HTML_WINDOW, 'HtmlWindow', 'Create HTML window'),
             (ID_NEW.CALENDAR_CTRL, 'CalendarCtrl', 'Create calendar control'),
             (ID_NEW.DATE_CTRL, 'DatePickerCtrl', 'Create date picker control'),
             (ID_NEW.GENERIC_DIR_CTRL, 'GenericDirCtrl', 'Create generic dir control'),
             (ID_NEW.UNKNOWN, 'Unknown', 'Create custom control placeholder'),
             ],
            ['button', 'Buttons',
             (ID_NEW.BUTTON, 'Button', 'Create button'),
             (ID_NEW.BITMAP_BUTTON, 'BitmapButton', 'Create bitmap button'),
             (ID_NEW.RADIO_BUTTON, 'RadioButton', 'Create radio button'),
             (ID_NEW.SPIN_BUTTON, 'SpinButton', 'Create spin button'),
             (ID_NEW.TOGGLE_BUTTON, 'ToggleButton', 'Create toggle button'),
             ],
            ['box', 'Boxes',
             (ID_NEW.STATIC_BOX, 'StaticBox', 'Create static box'),
             (ID_NEW.CHECK_BOX, 'CheckBox', 'Create check box'),
             (ID_NEW.RADIO_BOX, 'RadioBox', 'Create radio box'),
             (ID_NEW.COMBO_BOX, 'ComboBox', 'Create combo box'),
             (ID_NEW.LIST_BOX, 'ListBox', 'Create list box'),
             (ID_NEW.CHECK_LIST, 'CheckListBox', 'Create checklist box'),
             ],
            ['container', 'Containers',
             (ID_NEW.PANEL, 'Panel', 'Create panel'),
             (ID_NEW.NOTEBOOK, 'Notebook', 'Create notebook control'),
             (ID_NEW.CHOICEBOOK, 'Choicebook', 'Create choicebook control'),
             (ID_NEW.LISTBOOK, 'Listbook', 'Create listbook control'),
             (ID_NEW.SPLITTER_WINDOW, 'SplitterWindow', 'Create splitter window'),
             (ID_NEW.TOOL_BAR, 'ToolBar', 'Create toolbar'),
             (ID_NEW.STATUS_BAR, 'StatusBar', 'Create status bar'),
             (ID_NEW.MENU_BAR, 'MenuBar', 'Create menubar'),
#             (ID_NEW.WIZARD_PAGE, 'Wizard Page', 'Create wizard page'),
             (ID_NEW.WIZARD_PAGE_SIMPLE, 'WizardPageSimple', 'Create simple wizard page'),
             ],
            ['sizer', 'Sizers',
             (ID_NEW.BOX_SIZER, 'BoxSizer', 'Create box sizer'),
             (ID_NEW.STATIC_BOX_SIZER, 'StaticBoxSizer',
              'Create static box sizer'),
             (ID_NEW.GRID_SIZER, 'GridSizer', 'Create grid sizer'),
             (ID_NEW.FLEX_GRID_SIZER, 'FlexGridSizer',
              'Create flexgrid sizer'),
             (ID_NEW.GRID_BAG_SIZER, 'GridBagSizer',
              'Create gridbag sizer'),
             (ID_NEW.SPACER, 'Spacer', 'Create spacer'),
             (ID_NEW.STD_DIALOG_BUTTON_SIZER, 'StdDialogButtonSizer',
              'Create standard button sizer'),
             ]
            ]
        self.menuControls = [
            (ID_NEW.MENU, 'Menu', 'Create menu'),
            (ID_NEW.MENU_ITEM, 'MenuItem', 'Create menu item'),
            (ID_NEW.SEPARATOR, 'Separator', 'Create separator'),
            ]
        self.toolBarControls = [
            (ID_NEW.TOOL, 'Tool', 'Create tool'),
            (ID_NEW.SEPARATOR, 'Separator', 'Create separator'),
            ['control', 'Various controls',
             (ID_NEW.STATIC_TEXT, 'Label', 'Create label'),
             (ID_NEW.STATIC_BITMAP, 'Bitmap', 'Create bitmap'),
             (ID_NEW.STATIC_LINE, 'Line', 'Create line'),
             (ID_NEW.TEXT_CTRL, 'TextBox', 'Create text box'),
             (ID_NEW.CHOICE, 'Choice', 'Create choice'),
             (ID_NEW.SLIDER, 'Slider', 'Create slider'),
             (ID_NEW.GAUGE, 'Gauge', 'Create gauge'),
             (ID_NEW.SCROLL_BAR, 'ScrollBar', 'Create scroll bar'),
             (ID_NEW.LIST_CTRL, 'ListCtrl', 'Create list control'),
             ],
            ['button', 'Buttons',
             (ID_NEW.BUTTON, 'Button', 'Create button'),
             (ID_NEW.BITMAP_BUTTON, 'BitmapButton', 'Create bitmap button'),
             (ID_NEW.RADIO_BUTTON, 'RadioButton', 'Create radio button'),
             (ID_NEW.SPIN_BUTTON, 'SpinButton', 'Create spin button'),
             ],
            ['box', 'Boxes',
             (ID_NEW.STATIC_BOX, 'StaticBox', 'Create static box'),
             (ID_NEW.CHECK_BOX, 'CheckBox', 'Create check box'),
             (ID_NEW.RADIO_BOX, 'RadioBox', 'Create radio box'),
             (ID_NEW.COMBO_BOX, 'ComboBox', 'Create combo box'),
             (ID_NEW.LIST_BOX, 'ListBox', 'Create list box'),
             (ID_NEW.CHECK_LIST, 'CheckListBox', 'Create checklist box'),
             ],
            ]
        self.stdButtons = [
            (ID_NEW.OK_BUTTON, 'OK Button', 'Create standard button'),
            (ID_NEW.YES_BUTTON, 'YES Button', 'Create standard button'),
            (ID_NEW.SAVE_BUTTON, 'SAVE Button',  'Create standard button'),
            (ID_NEW.APPLY_BUTTON, 'APPLY Button',  'Create standard button'),
            (ID_NEW.NO_BUTTON, 'NO Button',  'Create standard button'),
            (ID_NEW.CANCEL_BUTTON, 'CANCEL Button',  'Create standard button'),
            (ID_NEW.HELP_BUTTON, 'HELP Button',  'Create standard button'),
            (ID_NEW.CONTEXT_HELP_BUTTON, 'CONTEXT HELP Button', 'Create standard button'),
            ]
        self.stdButtonIDs = {
            ID_NEW.OK_BUTTON: ('wxID_OK', '&Ok'),
            ID_NEW.YES_BUTTON: ('wxID_YES', '&Yes'),
            ID_NEW.SAVE_BUTTON: ('wxID_SAVE', '&Save'),
            ID_NEW.APPLY_BUTTON: ('wxID_APPLY', '&Apply'),
            ID_NEW.NO_BUTTON: ('wxID_NO', '&No'),
            ID_NEW.CANCEL_BUTTON: ('wxID_CANCEL', '&Cancel'),
            ID_NEW.HELP_BUTTON: ('wxID_HELP', '&Help'),
            ID_NEW.CONTEXT_HELP_BUTTON: ('wxID_CONTEXT_HELP', '&Help'),
            }
            


################################################################################

# Set menu to list items.
# Each menu command is a tuple (id, label, help)
# submenus are lists [id, label, help, submenu]
# and separators are any other type. Shift is for making
# alternative sets of IDs. (+1000).
def SetMenu(m, list, shift=False):
    for l in list:
        if type(l) == types.TupleType:
            # Shift ID
            if shift:  l = (1000 + l[0],) + l[1:]
            apply(m.Append, l)
        elif type(l) == types.ListType:
            subMenu = wx.Menu()
            SetMenu(subMenu, l[2:], shift)
            m.AppendMenu(wx.NewId(), l[0], subMenu, l[1])
        else:                           # separator
            m.AppendSeparator()

################################################################################

class HighLightBox:
    def __init__(self, pos, size):
        if size.width == -1: size.width = 0
        if size.height == -1: size.height = 0
        w = g.testWin.panel
        l1 = wx.Window(w, -1, pos, wx.Size(size.width, 2))
        l1.SetBackgroundColour(wx.RED)
        l2 = wx.Window(w, -1, pos, wx.Size(2, size.height))
        l2.SetBackgroundColour(wx.RED)
        l3 = wx.Window(w, -1, wx.Point(pos.x + size.width - 2, pos.y), wx.Size(2, size.height))
        l3.SetBackgroundColour(wx.RED)
        l4 = wx.Window(w, -1, wx.Point(pos.x, pos.y + size.height - 2), wx.Size(size.width, 2))
        l4.SetBackgroundColour(wx.RED)
        self.lines = [l1, l2, l3, l4]
    # Move highlight to a new position
    def Replace(self, pos, size):
        if size.width == -1: size.width = 0
        if size.height == -1: size.height = 0
        self.lines[0].SetDimensions(pos.x, pos.y, size.width, 2)
        self.lines[1].SetDimensions(pos.x, pos.y, 2, size.height)
        self.lines[2].SetDimensions(pos.x + size.width - 2, pos.y, 2, size.height)
        self.lines[3].SetDimensions(pos.x, pos.y + size.height - 2, size.width, 2)
    # Remove it
    def Remove(self):
        map(wx.Window.Destroy, self.lines)
        g.testWin.highLight = None
    def Refresh(self):
        map(wx.Window.Refresh, self.lines)

################################################################################

class XML_Tree(wx.TreeCtrl):
    def __init__(self, parent, id):
        wx.TreeCtrl.__init__(self, parent, id, style = wx.TR_HAS_BUTTONS | wx.TR_MULTIPLE)
        self.SetBackgroundColour(wx.Colour(224, 248, 224))
        # Register events
        wx.EVT_TREE_SEL_CHANGED(self, self.GetId(), self.OnSelChanged)
        # One works on Linux, another on Windows
        if wx.Platform == '__WXGTK__':
            wx.EVT_TREE_ITEM_ACTIVATED(self, self.GetId(), self.OnItemActivated)
        else:
            wx.EVT_LEFT_DCLICK(self, self.OnDClick)
        wx.EVT_RIGHT_DOWN(self, self.OnRightDown)
        wx.EVT_TREE_ITEM_EXPANDED(self, self.GetId(), self.OnItemExpandedCollapsed)
        wx.EVT_TREE_ITEM_COLLAPSED(self, self.GetId(), self.OnItemExpandedCollapsed)

        self.selection = None
        self.selectionChanging = False
        self.needUpdate = False
        self.pendingHighLight = None
        self.ctrl = self.shift = False
        self.dom = None
        # Create image list
        il = wx.ImageList(16, 16, True)
        self.rootImage = il.Add(images.getTreeRootImage().Scale(16,16).ConvertToBitmap())
        xxxObject.image = il.Add(images.getTreeDefaultImage().Scale(16,16).ConvertToBitmap())
        xxxPanel.image = il.Add(images.getTreePanelImage().Scale(16,16).ConvertToBitmap())
        xxxDialog.image = il.Add(images.getTreeDialogImage().Scale(16,16).ConvertToBitmap())
        xxxFrame.image = il.Add(images.getTreeFrameImage().Scale(16,16).ConvertToBitmap())
        xxxMenuBar.image = il.Add(images.getTreeMenuBarImage().Scale(16,16).ConvertToBitmap())
        xxxMenu.image = il.Add(images.getTreeMenuImage().Scale(16,16).ConvertToBitmap())
        xxxMenuItem.image = il.Add(images.getTreeMenuItemImage().Scale(16,16).ConvertToBitmap())
        xxxToolBar.image = il.Add(images.getTreeToolBarImage().Scale(16,16).ConvertToBitmap())
        xxxTool.image = il.Add(images.getTreeToolImage().Scale(16,16).ConvertToBitmap())
        xxxSeparator.image = il.Add(images.getTreeSeparatorImage().Scale(16,16).ConvertToBitmap())
        xxxSizer.imageH = il.Add(images.getTreeSizerHImage().Scale(16,16).ConvertToBitmap())
        xxxSizer.imageV = il.Add(images.getTreeSizerVImage().Scale(16,16).ConvertToBitmap())
        xxxStaticBoxSizer.imageH = il.Add(images.getTreeStaticBoxSizerHImage().Scale(16,16).ConvertToBitmap())
        xxxStaticBoxSizer.imageV = il.Add(images.getTreeStaticBoxSizerVImage().Scale(16,16).ConvertToBitmap())
        xxxGridSizer.image = il.Add(images.getTreeSizerGridImage().Scale(16,16).ConvertToBitmap())
        xxxFlexGridSizer.image = il.Add(images.getTreeSizerFlexGridImage().Scale(16,16).ConvertToBitmap())
        self.il = il
        self.SetImageList(il)

    def RegisterKeyEvents(self):
        wx.EVT_KEY_DOWN(self, g.tools.OnKeyDown)
        wx.EVT_KEY_UP(self, g.tools.OnKeyUp)
        wx.EVT_ENTER_WINDOW(self, g.tools.OnMouse)
        wx.EVT_LEAVE_WINDOW(self, g.tools.OnMouse)

    def ExpandAll(self, item):
        if self.ItemHasChildren(item):
            self.Expand(item)
            i, cookie = self.GetFirstChild(item)
            children = []
            while i.IsOk():
                children.append(i)
                i, cookie = self.GetNextChild(item, cookie)
            for i in children:
                self.ExpandAll(i)
    def CollapseAll(self, item):
        if self.ItemHasChildren(item):
            i, cookie = self.GetFirstChild(item)
            children = []
            while i.IsOk():
                children.append(i)
                i, cookie = self.GetNextChild(item, cookie)
            for i in children:
                self.CollapseAll(i)
            self.Collapse(item)

    # Clear tree
    def Clear(self):
        self.selection = None
        self.UnselectAll()
        self.DeleteAllItems()
        # Add minimal structure
        if self.dom: self.dom.unlink()
        self.dom = MyDocument()
        self.dummyNode = self.dom.createComment('dummy node')
        # Create main node
        self.mainNode = self.dom.createElement('resource')
        self.dom.appendChild(self.mainNode)
        self.rootObj = xxxMainNode(self.dom)
        self.root = self.AddRoot('XML tree', self.rootImage,
                                 data=wx.TreeItemData(self.rootObj))
        self.SetItemHasChildren(self.root)
        self.testElem = self.dom.createElement('dummy')
        self.mainNode.appendChild(self.testElem)
        self.Expand(self.root)

    # Clear old data and set new
    def SetData(self, dom):
        self.selection = None
        self.UnselectAll()
        self.DeleteAllItems()
        # Add minimal structure
        if self.dom: self.dom.unlink()
        self.dom = dom
        self.dummyNode = self.dom.createComment('dummy node')
        # Find 'resource' child, add it's children
        self.mainNode = dom.documentElement
        self.rootObj = xxxMainNode(self.dom)
        self.root = self.AddRoot('XML tree', self.rootImage,
                                 data=wx.TreeItemData(self.rootObj))
        self.SetItemHasChildren(self.root)
        nodes = self.mainNode.childNodes[:]
        for node in nodes:
            if IsObject(node):
                self.AddNode(self.root, None, node)
            else:
                self.mainNode.removeChild(node)
                node.unlink()
        if self.mainNode.firstChild:
            self.testElem = self.dom.createElement('dummy')
            self.mainNode.insertBefore(self.testElem, self.mainNode.firstChild)
        else:
            self.testElem = self.dom.createElement('dummy')
            self.mainNode.appendChild(self.testElem)
        self.Expand(self.root)

    # Add tree item for given parent item if node is DOM element node with
    # object/object_ref tag. xxxParent is parent xxx object
    def AddNode(self, itemParent, xxxParent, node):
        # Set item data to current node
        try:
            xxx = MakeXXXFromDOM(xxxParent, node)
        except:
            print 'ERROR: MakeXXXFromDom(%s, %s)' % (xxxParent, node)
            raise
        treeObj = xxx.treeObject()
        # Append tree item
        item = self.AppendItem(itemParent, treeObj.treeName(),
                               image=treeObj.treeImage(),
                               data=wx.TreeItemData(xxx))
        # Different color for references
        if treeObj.ref:
            self.SetItemTextColour(item, 'DarkGreen')
        # Try to find children objects
        if treeObj.hasChildren:
            nodes = treeObj.element.childNodes[:]
            for n in nodes:
                if IsObject(n):
                    self.AddNode(item, treeObj, n)
                elif n.nodeType != minidom.Node.ELEMENT_NODE:
                    treeObj.element.removeChild(n)
                    n.unlink()

    # Insert new item at specific position
    def InsertNode(self, itemParent, parent, elem, nextItem):
        # Insert in XML tree and wxWin
        xxx = MakeXXXFromDOM(parent, elem)
        # If nextItem is None, we append to parent, otherwise insert before it
        if nextItem.IsOk():
            node = self.GetPyData(nextItem).element
            parent.element.insertBefore(elem, node)
            # Inserting before is difficult, se we insert after or first child
            index = self.ItemIndex(nextItem)
            newItem = self.InsertItemBefore(itemParent, index,
                        xxx.treeName(), image=xxx.treeImage())
            self.SetPyData(newItem, xxx)
        else:
            parent.element.appendChild(elem)
            newItem = self.AppendItem(itemParent, xxx.treeName(), image=xxx.treeImage(),
                                      data=wx.TreeItemData(xxx))
        # Different color for references
        if xxx.treeObject().ref:  self.SetItemTextColour(newItem, 'DarkGreen')
        # Add children items
        if xxx.hasChildren:
            treeObj = xxx.treeObject()
            for n in treeObj.element.childNodes:
                if IsObject(n):
                    self.AddNode(newItem, treeObj, n)
        return newItem

    # Remove leaf of tree, return it's data object
    def RemoveLeaf(self, leaf):
        xxx = self.GetPyData(leaf)
        node = xxx.element
        parent = node.parentNode
        parent.removeChild(node)
        self.Delete(leaf)
        # Reset selection object
        self.selection = None
        return node
    # Find position relative to the top-level window
    def FindNodePos(self, item, obj=None):
        # Root at (0,0)
        if item == g.testWin.item: return wx.Point(0, 0)
        itemParent = self.GetItemParent(item)
        # Select book page
        if not obj: obj = self.FindNodeObject(item)
        if self.GetPyData(itemParent).treeObject().__class__ in \
               [xxxNotebook, xxxChoicebook, xxxListbook]:
            book = self.FindNodeObject(itemParent)
            # Find position
            for i in range(book.GetPageCount()):
                if book.GetPage(i) == obj:
                    if book.GetSelection() != i:
                        book.SetSelection(i)
                        # Remove highlight - otherwise highlight window won't be visible
                        if g.testWin.highLight:
                            g.testWin.highLight.Remove()
                    break
        # Find first ancestor which is a wxWindow (not a sizer)
        winParent = itemParent
        while self.GetPyData(winParent).isSizer:
            winParent = self.GetItemParent(winParent)
        # Notebook children are layed out in a little strange way
        if self.GetPyData(itemParent).treeObject().__class__ == xxxNotebook:
            parentPos = wx.Point(0,0)
        else:
            parentPos = self.FindNodePos(winParent)
        # Position (-1,-1) is really (0,0)
        pos = obj.GetPosition()
        if pos == (-1,-1): pos = (0,0)
        return parentPos + pos

    # Find window (or sizer) corresponding to a tree item.
    def FindNodeObject(self, item):
        testWin = g.testWin
        # If top-level, return testWin (or panel its panel)
        if item == testWin.item: return testWin.panel
        itemParent = self.GetItemParent(item)
        xxx = self.GetPyData(item).treeObject()
        parentWin = self.FindNodeObject(itemParent)
        # Top-level sizer? return window's sizer
        if xxx.isSizer and isinstance(parentWin, wx.Window):
            return parentWin.GetSizer()
        elif xxx.__class__ in [xxxMenu, xxxMenuItem, xxxSeparator]:  return None
        elif xxx.__class__ in [xxxToolBar, xxxMenuBar]:
            # If it's the main toolbar or menubar, we can't really select it
            if xxx.parent.__class__ == xxxFrame:  return None
        elif isinstance(xxx.parent, xxxToolBar):
            # Select complete toolbar
            return parentWin
        elif isinstance(xxx.parent, xxxStdDialogButtonSizer):
            # This sizer returns non-existing children
            for ch in parentWin.GetChildren():
                if ch.GetWindow() and ch.GetWindow().GetName() == xxx.name:
                    return ch.GetWindow()
            return None
        elif xxx.parent.__class__ in [xxxChoicebook, xxxListbook]:
            # First window is controld
            return parentWin.GetChildren()[self.ItemIndex(item)+1]
        # Otherwise get parent's object and it's child
        child = parentWin.GetChildren()[self.WindowIndex(item)]
        # Return window or sizer for sizer items
        if child.GetClassName() == 'wxSizerItem':
            if child.IsWindow(): child = child.GetWindow()
            elif child.IsSizer():
                child = child.GetSizer()
        return child

    def OnSelChanged(self, evt):
        if self.selectionChanging: return
        self.selectionChanging = True
        self.UnselectAll()
        self.SelectItem(evt.GetItem())
        self.selectionChanging = False

    def ChangeSelection(self, item):
        # Apply changes
        # !!! problem with wxGTK - GetOldItem is Ok if nothing selected
        #oldItem = evt.GetOldItem()
        status = ''
        oldItem = self.selection
        # use GetItemParent as a way to determine if the itemId is still valid
        if oldItem and self.GetItemParent(oldItem):
            xxx = self.GetPyData(oldItem)
            # If some data was modified, apply changes
            if g.panel.IsModified():
                self.Apply(xxx, oldItem)
                #if conf.autoRefresh:
                if g.testWin:
                    if g.testWin.highLight:
                        g.testWin.highLight.Remove()
                    self.needUpdate = True
                status = 'Changes were applied'
        g.frame.SetStatusText(status)
        # Generate view
        self.selection = item
        if not self.selection.IsOk():
            self.selection = None
            return
        xxx = self.GetPyData(self.selection)
        # Update panel
        g.panel.SetData(xxx)
        # Update tools
        g.tools.UpdateUI()
        # Highlighting is done in OnIdle
        self.pendingHighLight = self.selection

    # Check if item is in testWin subtree
    def IsHighlatable(self, item):
        if item == g.testWin.item: return False
        while item != self.root:
            item = self.GetItemParent(item)
            if item == g.testWin.item: return True
        return False

    # Highlight selected item
    def HighLight(self, item):
        self.pendingHighLight = None
        # Can highlight only with some top-level windows
        if not g.testWin or self.GetPyData(g.testWin.item).treeObject().__class__ \
            not in [xxxDialog, xxxPanel, xxxFrame]:
            return
        # If a control from another window is selected, remove highlight
        if not self.IsHighlatable(item):
            if g.testWin.highLight: g.testWin.highLight.Remove()
            return
        # Get window/sizer object
        obj = self.FindNodeObject(item)
        if not obj:
            if g.testWin.highLight: g.testWin.highLight.Remove()
            return
        pos = self.FindNodePos(item, obj)
        size = obj.GetSize()
        # Highlight
        # Negative positions are not working quite well
        if g.testWin.highLight:
            g.testWin.highLight.Replace(pos, size)
        else:
            g.testWin.highLight = HighLightBox(pos, size)
        g.testWin.highLight.Refresh()
        g.testWin.highLight.item = item

    def ShowTestWindow(self, item):
        xxx = self.GetPyData(item)
        if g.panel.IsModified():
            self.Apply(xxx, item)       # apply changes
        availableViews = ['wxFrame', 'wxPanel', 'wxDialog',  
                          'wxMenuBar', 'wxToolBar', 'wxWizard',  
                          'wxWizardPageSimple']
        originalItem = item
        # Walk up the tree until we find an item that has a view
        while item and self.GetPyData(item).treeObject().className not in availableViews:
            item = self.GetItemParent(item)
        if not item or not item.IsOk():
            wx.LogMessage('No view for this element (yet)')
            return
        # Show item in bold
        if g.testWin:     # Reset old
            self.SetItemBold(g.testWin.item, False)
        try:
            wx.BeginBusyCursor()
            self.CreateTestWin(item)
        finally:
            wx.EndBusyCursor()
        # Maybe an error occurred, so we need to test
        if g.testWin:
            self.SetItemBold(g.testWin.item)
            # Select original item
            self.ChangeSelection(originalItem)

    # Double-click on Linux
    def OnItemActivated(self, evt):
        if evt.GetItem() != self.root:
            self.ShowTestWindow(evt.GetItem())

    # Double-click on Windows
    def OnDClick(self, evt):
        item, flags = self.HitTest(evt.GetPosition())
        if flags in [wx.TREE_HITTEST_ONITEMBUTTON, wx.TREE_HITTEST_ONITEMLABEL]:
            if item != self.root: self.ShowTestWindow(item)
        else:
            evt.Skip()

    def OnItemExpandedCollapsed(self, evt):
        # Update tool palette
        g.tools.UpdateUI()
        evt.Skip()

    # (re)create test window
    def CreateTestWin(self, item):
        testWin = g.testWin
        # Create a window with this resource
        xxx = self.GetPyData(item).treeObject()

        # If frame
#        if xxx.__class__ == xxxFrame:
            # Frame can't have many children,
            # but it's first child possibly can...
#            child = self.GetFirstChild(item)[0]
#            if child.IsOk() and self.GetPyData(child).__class__ == xxxPanel:
#                # Clean-up before recursive call or error
#                wx.MemoryFSHandler.RemoveFile('xxx.xrc')
#                wx.EndBusyCursor()
#                self.CreateTestWin(child)
#                return

        # Close old window, remember where it was
        highLight = None
        if testWin:
            pos = testWin.GetPosition()
            if item == testWin.item:
                # Remember highlight if same top-level window
                if testWin.highLight:
                    highLight = testWin.highLight.item
                if xxx.className == 'wxPanel':
                    if testWin.highLight:
                        testWin.pendingHighLight = highLight
                        testWin.highLight.Remove()
                    testWin.panel.Destroy()
                    testWin.panel = None
                else:
                    testWin.Destroy()
                    testWin = g.testWin = None
            else:
                testWin.Destroy()
                testWin = g.testWin = None
        else:
            pos = g.testWinPos
        # Save in memory FS
        memFile = MemoryFile('xxx.xrc')
        # Create memory XML file
        elem = xxx.element.cloneNode(True)
        if not xxx.name:
            name = 'noname'
        else:
            name = xxx.name
        elem.setAttribute('name', STD_NAME)
        oldTestNode = self.testElem
        self.testElem = elem
        self.mainNode.replaceChild(elem, oldTestNode)
        oldTestNode.unlink()
        # Replace wizard page class temporarily
        if xxx.__class__ in [xxxWizardPage, xxxWizardPageSimple]:
            oldCl = elem.getAttribute('class')
            elem.setAttribute('class', 'wxPanel')
        parent = elem.parentNode
        encd = self.rootObj.params['encoding'].value()
        if not encd: encd = None
        try:
            self.dom.writexml(memFile, encoding=encd)
        except:
            inf = sys.exc_info()
            wx.LogError(traceback.format_exception(inf[0], inf[1], None)[-1])
            wx.LogError('Error writing temporary file')
        memFile.close()                 # write to wxMemoryFS
        xmlFlags = xrc.XRC_NO_SUBCLASSING
        # Use translations if encoding is not specified
        if not g.currentEncoding:
            xmlFlags != xrc.XRC_USE_LOCALE
        res = xrc.XmlResource('', xmlFlags)
        res.Load('memory:xxx.xrc')
        try:
            if xxx.__class__ == xxxFrame:
                # Frame can't have many children,
                # but it's first child possibly can...
    #            child = self.GetFirstChild(item)[0]
    #            if child.IsOk() and self.GetPyData(child).__class__ == xxxPanel:
    #                # Clean-up before recursive call or error
    #                wx.MemoryFSHandler.RemoveFile('xxx.xrc')
    #                wx.EndBusyCursor()
    #                self.CreateTestWin(child)
    #                return
                # This currently works under GTK, but not under MSW
                testWin = g.testWin = wx.PreFrame()
                res.LoadOnFrame(testWin, g.frame, STD_NAME)
                # Create status bar
                testWin.panel = testWin
                #testWin.CreateStatusBar()
                testWin.SetClientSize(testWin.GetBestSize())
                testWin.SetPosition(pos)
                testWin.Show(True)
            elif xxx.__class__ == xxxPanel:
                # Create new frame
                if not testWin:
                    testWin = g.testWin = wx.Frame(g.frame, -1, 'Panel: ' + name,
                                                  pos=pos, name=STD_NAME)
                testWin.panel = res.LoadPanel(testWin, STD_NAME)
                testWin.SetClientSize(testWin.GetBestSize())
                testWin.Show(True)
            elif xxx.__class__ == xxxDialog:
                testWin = g.testWin = res.LoadDialog(None, STD_NAME)
                testWin.panel = testWin
                testWin.Layout()
                testWin.SetPosition(pos)
                testWin.Show(True)
                # Dialog's default code does not produce wx.EVT_CLOSE
                wx.EVT_BUTTON(testWin, wx.ID_OK, self.OnCloseTestWin)
                wx.EVT_BUTTON(testWin, wx.ID_CANCEL, self.OnCloseTestWin)
            elif xxx.__class__ == xxxWizard:
                wiz = wx.wizard.PreWizard()
                res.LoadOnObject(wiz, None, STD_NAME, 'wxWizard')
                # Find first page (don't know better way)
                firstPage = None
                for w in wiz.GetChildren():
                    if isinstance(w, wx.wizard.WizardPage):
                        firstPage = w
                        break
                if not firstPage:
                    wx.LogError('Wizard is empty')
                else:
                    # Wizard should be modal
                    self.SetItemBold(item)
                    wiz.RunWizard(w)
                    self.SetItemBold(item, False)
                    wiz.Destroy()
            elif xxx.__class__ in [xxxWizardPage, xxxWizardPageSimple]:
                # Create new frame
                if not testWin:
                    testWin = g.testWin = wx.Frame(g.frame, -1, 'Wizard page: ' + name,
                                                  pos=pos, name=STD_NAME)
                testWin.panel = wx.PrePanel()
                res.LoadOnObject(testWin.panel, testWin, STD_NAME, 'wxPanel')
                testWin.SetClientSize(testWin.GetBestSize())
                testWin.Show(True)
            elif xxx.__class__ == xxxMenuBar:
                testWin = g.testWin = wx.Frame(g.frame, -1, 'MenuBar: ' + name,
                                              pos=pos, name=STD_NAME)
                testWin.panel = None
                # Set status bar to display help
                testWin.CreateStatusBar()
                testWin.menuBar = res.LoadMenuBar(STD_NAME)
                testWin.SetMenuBar(testWin.menuBar)
                testWin.Show(True)
            elif xxx.__class__ == xxxToolBar:
                testWin = g.testWin = wx.Frame(g.frame, -1, 'ToolBar: ' + name,
                                              pos=pos, name=STD_NAME)
                testWin.panel = None
                # Set status bar to display help
                testWin.CreateStatusBar()
                testWin.toolBar = res.LoadToolBar(testWin, STD_NAME)
                testWin.SetToolBar(testWin.toolBar)
                testWin.Show(True)
            if testWin:
                testWin.item = item
                wx.EVT_CLOSE(testWin, self.OnCloseTestWin)
                testWin.highLight = None
                if highLight and not self.pendingHighLight:
                    self.HighLight(highLight)
        except:
            if g.testWin:
                self.SetItemBold(item, False)
                g.testWinPos = g.testWin.GetPosition()
                g.testWin.Destroy()
                g.testWin = None
            inf = sys.exc_info()
            wx.LogError(traceback.format_exception(inf[0], inf[1], None)[-1])
            wx.LogError('Error loading resource')
        wx.MemoryFSHandler.RemoveFile('xxx.xrc')

    def CloseTestWindow(self):
        if not g.testWin: return
        self.SetItemBold(g.testWin.item, False)
        g.frame.tb.ToggleTool(g.frame.ID_TOOL_LOCATE, False)
        g.testWinPos = g.testWin.GetPosition()
        g.testWin.Destroy()
        g.testWin = None

    def OnCloseTestWin(self, evt):
        self.CloseTestWindow()

    # Return index in parent, for real window children
    def WindowIndex(self, item):
        n = 0                           # index of sibling
        prev = self.GetPrevSibling(item)
        while prev.IsOk():
            # MenuBar is not a child
            if not isinstance(self.GetPyData(prev), xxxMenuBar):
                n += 1
            prev = self.GetPrevSibling(prev)
        return n

    # Return item index in parent
    def ItemIndex(self, item):
        n = 0                           # index of sibling
        prev = self.GetPrevSibling(item)
        while prev.IsOk():
            prev = self.GetPrevSibling(prev)
            n += 1
        return n

    # Full tree index of an item - list of positions
    def ItemFullIndex(self, item):
        if not item.IsOk(): return None
        l = []
        while item != self.root:
            l.insert(0, self.ItemIndex(item))
            item = self.GetItemParent(item)
        return l
    # Get item position from full index
    def ItemAtFullIndex(self, index):
        if index is None: return wx.TreeItemId()
        item = self.root
        for i in index:
            item = self.GetFirstChild(item)[0]
            for k in range(i): item = self.GetNextSibling(item)
        return item

    # True if next item should be inserted after current (vs. appended to it)
    def NeedInsert(self, item):
        xxx = self.GetPyData(item)
        if item == self.root: return False        # root item
        if xxx.hasChildren and not self.GetChildrenCount(item, False):
            return False
        return not (self.IsExpanded(item) and self.GetChildrenCount(item, False))

    # Override to use like single-selection tree
    def GetSelection(self):
        return self.selection
    def SelectItem(self, item):
        self.UnselectAll()
        self.ChangeSelection(item)
        wx.TreeCtrl.SelectItem(self, item)

    # Pull-down
    def OnRightDown(self, evt):
        pullDownMenu = g.pullDownMenu
        # select this item
        pt = evt.GetPosition();
        item, flags = self.HitTest(pt)
        if item.Ok() and flags & wx.TREE_HITTEST_ONITEM:
            self.SelectItem(item)

        # Setup menu
        menu = wx.Menu()

        item = self.selection
        if not item:
            menu.Append(g.pullDownMenu.ID_EXPAND, 'Expand', 'Expand tree')
            menu.Append(g.pullDownMenu.ID_COLLAPSE, 'Collapse', 'Collapse tree')
        else:
#            self.ctrl = evt.ControlDown() # save Ctrl state
#            self.shift = evt.ShiftDown()  # and Shift too
            m = wx.Menu()                  # create menu
            if self.ctrl:
                needInsert = True
            else:
                needInsert = self.NeedInsert(item)
            if item == self.root or needInsert and self.GetItemParent(item) == self.root:
                SetMenu(m, pullDownMenu.topLevel)
                m.AppendSeparator()
                m.Append(ID_NEW.REF, 'reference...', 'Create object_ref node')
            else:
                xxx = self.GetPyData(item).treeObject()
                # Check parent for possible child nodes if inserting sibling
                if needInsert: xxx = xxx.parent
                if xxx.__class__ == xxxMenuBar:
                    m.Append(ID_NEW.MENU, 'Menu', 'Create menu')
                elif xxx.__class__ in [xxxToolBar, xxxTool] or \
                     xxx.__class__ == xxxSeparator and xxx.parent.__class__ == xxxToolBar:
                    SetMenu(m, pullDownMenu.toolBarControls)
                elif xxx.__class__ in [xxxMenu, xxxMenuItem]:
                    SetMenu(m, pullDownMenu.menuControls)
                elif xxx.__class__ == xxxStdDialogButtonSizer:
                    SetMenu(m, pullDownMenu.stdButtons)
                else:
                    SetMenu(m, pullDownMenu.controls)
                    if xxx.__class__ in [xxxNotebook, xxxChoicebook, xxxListbook]:
                        m.Enable(m.FindItem('sizer'), False)
                    elif not (xxx.isSizer or xxx.parent and xxx.parent.isSizer):
                        m.Enable(ID_NEW.SPACER, False)
                    if xxx.__class__ is not xxxFrame:
                        m.Enable(ID_NEW.MENU_BAR, False)
                m.AppendSeparator()
                m.Append(ID_NEW.REF, 'reference...', 'Create object_ref node')
            # Select correct label for create menu
            if not needInsert:
                if self.shift:
                    menu.AppendMenu(wx.NewId(), 'Insert Child', m,
                                    'Create child object as the first child')
                else:
                    menu.AppendMenu(wx.NewId(), 'Append Child', m,
                                    'Create child object as the last child')
            else:
                if self.shift:
                    menu.AppendMenu(wx.NewId(), 'Create Sibling', m,
                                    'Create sibling before selected object')
                else:
                    menu.AppendMenu(wx.NewId(), 'Create Sibling', m,
                                    'Create sibling after selected object')
            # Build replace menu
            if item != self.root:
                xxx = self.GetPyData(item).treeObject()
                m = wx.Menu()                  # create replace menu
                if xxx.__class__ == xxxMenuBar:
                    m.Append(1000 + ID_NEW.MENU, 'Menu', 'Create menu')
                elif xxx.__class__ in [xxxMenu, xxxMenuItem]:
                    SetMenu(m, pullDownMenu.menuControls, shift=True)
                elif xxx.__class__ == xxxToolBar and \
                         self.GetItemParent(item) == self.root:
                    SetMenu(m, [], shift=True)
                elif xxx.__class__ in [xxxFrame, xxxDialog, xxxPanel]:
                    SetMenu(m, [
                        (ID_NEW.PANEL, 'Panel', 'Create panel'),
                        (ID_NEW.DIALOG, 'Dialog', 'Create dialog'),
                        (ID_NEW.FRAME, 'Frame', 'Create frame')], shift=True)
                elif xxx.isSizer:
                    SetMenu(m, pullDownMenu.sizers, shift=True)
                else:
                    SetMenu(m, pullDownMenu.controls, shift=True)
                id = wx.NewId()
                menu.AppendMenu(id, 'Replace With', m)
                if not m.GetMenuItemCount(): menu.Enable(id, False)
                menu.Append(pullDownMenu.ID_SUBCLASS, 'Subclass...',
                            'Set "subclass" property')
            menu.AppendSeparator()
            # Not using standart IDs because we don't want to show shortcuts
            menu.Append(wx.ID_CUT, 'Cut', 'Cut to the clipboard')
            menu.Append(wx.ID_COPY, 'Copy', 'Copy to the clipboard')
            if self.ctrl and item != self.root:
                menu.Append(pullDownMenu.ID_PASTE_SIBLING, 'Paste Sibling',
                            'Paste from the clipboard as a sibling')
            else:
                menu.Append(wx.ID_PASTE, 'Paste', 'Paste from the clipboard')
            menu.Append(pullDownMenu.ID_DELETE,
                                'Delete', 'Delete object')
            if self.ItemHasChildren(item):
                menu.AppendSeparator()
                menu.Append(pullDownMenu.ID_EXPAND, 'Expand', 'Expand subtree')
                menu.Append(pullDownMenu.ID_COLLAPSE, 'Collapse', 'Collapse subtree')
        self.PopupMenu(menu, evt.GetPosition())
        menu.Destroy()

    # Apply changes
    def Apply(self, xxx, item):
        g.panel.Apply()
        # Update tree view
        xxx = xxx.treeObject()
        if xxx.hasName and self.GetItemText(item) != xxx.name:
            self.SetItemText(item, xxx.treeName())
            # Item width may have changed
            # !!! Tric to update tree width (wxGTK, ??)
            self.SetIndent(self.GetIndent())
        # Change tree icon for sizers
        if isinstance(xxx, xxxBoxSizer):
            self.SetItemImage(item, xxx.treeImage())
        # Set global modified state
        g.frame.SetModified()

