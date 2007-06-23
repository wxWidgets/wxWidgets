# Name:         view.py
# Purpose:      View classes
# Author:       Roman Rolinsky <rolinsky@femagsoft.com>
# Created:      07.06.2007
# RCS-ID:       $Id$

from globals import *
from XMLTree import XMLTree
from AttributePanel import Panel
import images

class TaskBarIcon(wx.TaskBarIcon):
    def __init__(self, frame):
        wx.TaskBarIcon.__init__(self)
        self.frame = frame
        # Set the image
        self.SetIcon(images.getIconIcon(), "XRCed")

def CreateView():
    '''
    Create all necessary view objects. Some of them are set as module
    global variables for convenience.
    '''
    global frame
    frame = Frame()                     # frame creates other

class Frame(wx.Frame):
    def __init__(self):
        wx.Frame.__init__(self, None, -1, '', size=(600,400))
        bar = self.CreateStatusBar(2)
        bar.SetStatusWidths([-1, 40])
        self.SetIcon(images.getIconIcon())
        try:
            self.tbicon = TaskBarIcon(self)
        except:
            self.tbicon = None

        # Make menus
        menuBar = wx.MenuBar()

        menu = wx.Menu()
        menu.Append(wx.ID_NEW, '&New\tCtrl-N', 'New file')
        menu.AppendSeparator()
        menu.Append(wx.ID_OPEN, '&Open...\tCtrl-O', 'Open XRC file')
        
        self.recentMenu = wx.Menu()
#        g.fileHistory.UseMenu(self.recentMenu)
#        g.fileHistory.AddFilesToMenu()

        menu.AppendMenu(-1, 'Open &Recent', self.recentMenu, 'Open a recent file')
        
        menu.AppendSeparator()
        menu.Append(wx.ID_SAVE, '&Save\tCtrl-S', 'Save XRC file')
        menu.Append(wx.ID_SAVEAS, 'Save &As...', 'Save XRC file under different name')
        self.ID_GENERATE_PYTHON = wx.NewId()
        menu.Append(self.ID_GENERATE_PYTHON, '&Generate Python...', 
                    'Generate a Python module that uses this XRC')
        menu.AppendSeparator()
        self.ID_PREFS = wx.NewId()
        menu.Append(self.ID_PREFS, 'Preferences...', 'Change XRCed settings')
        menu.AppendSeparator()
        menu.Append(wx.ID_EXIT, '&Quit\tCtrl-Q', 'Exit application')

        menuBar.Append(menu, '&File')

        menu = wx.Menu()
        menu.Append(wx.ID_UNDO, '&Undo\tCtrl-Z', 'Undo')
        menu.Append(wx.ID_REDO, '&Redo\tCtrl-Y', 'Redo')
        menu.AppendSeparator()
        menu.Append(wx.ID_CUT, 'Cut\tCtrl-X', 'Cut to the clipboard')
        menu.Append(wx.ID_COPY, '&Copy\tCtrl-C', 'Copy to the clipboard')
        menu.Append(wx.ID_PASTE, '&Paste\tCtrl-V', 'Paste from the clipboard')
        menu.Append(wx.ID_DELETE, '&Delete\tCtrl-D', 'Delete object')
        menu.AppendSeparator()
        self.ID_LOCATE = wx.NewId()
        self.ID_TOOL_LOCATE = wx.NewId()
        self.ART_LOCATE = 'ART_LOCATE'
        self.ID_TOOL_PASTE = wx.NewId()
        menu.Append(self.ID_LOCATE, '&Locate\tCtrl-L', 'Locate control in test window and select it')
        menuBar.Append(menu, '&Edit')
        menu = wx.Menu()
        self.ID_EMBED_PANEL = wx.NewId()
        menu.Append(self.ID_EMBED_PANEL, '&Embed Panel',
                    'Toggle embedding properties panel in the main window', True)
        menu.Check(self.ID_EMBED_PANEL, g.conf.embedPanel)
        self.ID_SHOW_TOOLS = wx.NewId()
        menu.Append(self.ID_SHOW_TOOLS, 'Show &Tools', 'Toggle tools', True)
        menu.Check(self.ID_SHOW_TOOLS, g.conf.showTools)
        menu.AppendSeparator()
        self.ID_TEST = wx.NewId()
        self.ART_TEST = 'ART_TEST'
        menu.Append(self.ID_TEST, '&Test\tF5', 'Show test window')
        self.ID_REFRESH = wx.NewId()
        self.ART_REFRESH = 'ART_REFRESH'
        menu.Append(self.ID_REFRESH, '&Refresh\tCtrl-R', 'Refresh test window')
        self.ID_AUTO_REFRESH = wx.NewId()
        self.ART_AUTO_REFRESH = 'ART_AUTO_REFRESH'
        menu.Append(self.ID_AUTO_REFRESH, '&Auto-refresh\tAlt-A',
                    'Toggle auto-refresh mode', True)
        menu.Check(self.ID_AUTO_REFRESH, g.conf.autoRefresh)
        self.ID_TEST_HIDE = wx.NewId()
        menu.Append(self.ID_TEST_HIDE, '&Hide\tF6', 'Close test window')
        menuBar.Append(menu, '&View')

        menu = wx.Menu()
        self.ID_MOVEUP = wx.NewId()
        self.ART_MOVEUP = 'ART_MOVEUP'
        menu.Append(self.ID_MOVEUP, '&Up', 'Move before previous sibling')
        self.ID_MOVEDOWN = wx.NewId()
        self.ART_MOVEDOWN = 'ART_MOVEDOWN'
        menu.Append(self.ID_MOVEDOWN, '&Down', 'Move after next sibling')
        self.ID_MOVELEFT = wx.NewId()
        self.ART_MOVELEFT = 'ART_MOVELEFT'
        menu.Append(self.ID_MOVELEFT, '&Make sibling', 'Make sibling of parent')
        self.ID_MOVERIGHT = wx.NewId()
        self.ART_MOVERIGHT = 'ART_MOVERIGHT'
        menu.Append(self.ID_MOVERIGHT, '&Make child', 'Make child of previous sibling')
        menuBar.Append(menu, '&Move')

        menu = wx.Menu()
        menu.Append(wx.ID_ABOUT, '&About...', 'About XCRed')
        self.ID_README = wx.NewId()
        menu.Append(self.ID_README, '&Readme...\tF1', 'View the README file')
        if debug:
            self.ID_DEBUG_CMD = wx.NewId()
            menu.Append(self.ID_DEBUG_CMD, 'CMD', 'Python command line')
        menuBar.Append(menu, '&Help')

        self.menuBar = menuBar
        self.SetMenuBar(menuBar)

        # Build interface
        sizer = wx.BoxSizer(wx.VERTICAL)
        #sizer.Add(wx.StaticLine(self, -1), 0, wx.EXPAND)
        # Horizontal sizer for toolbar and splitter
        self.toolsSizer = sizer1 = wx.BoxSizer()
        splitter = wx.SplitterWindow(self, -1, style=wx.SP_3DSASH)
        self.splitter = splitter
        splitter.SetMinimumPaneSize(100)

        global tree
        tree = XMLTree(splitter)

        # Miniframe for split mode
        miniFrame = wx.MiniFrame(self, -1, 'Properties & Style',
                                 (g.conf.panelX, g.conf.panelY),
                                 (g.conf.panelWidth, g.conf.panelHeight))
        self.miniFrame = miniFrame
        sizer2 = wx.BoxSizer()
        miniFrame.SetSizer(sizer2)
        # Create panel for parameters
        global panel
        if g.conf.embedPanel:
            panel = Panel(splitter)
            # Set plitter windows
            splitter.SplitVertically(tree, panel, g.conf.sashPos)
        else:
            panel = Panel(miniFrame)
            sizer2.Add(panel, 1, wx.EXPAND)
            miniFrame.Show(True)
            splitter.Initialize(tree)
        if wx.Platform == '__WXMAC__':
            sizer1.Add(splitter, 1, wx.EXPAND|wx.RIGHT, 5)
        else:
            sizer1.Add(splitter, 1, wx.EXPAND)
        sizer.Add(sizer1, 1, wx.EXPAND)
        self.SetAutoLayout(True)
        self.SetSizer(sizer)

    def Clear(self):
        tree.Clear()
