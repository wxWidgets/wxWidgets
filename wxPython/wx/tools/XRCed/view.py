# Name:         view.py
# Purpose:      View classes
# Author:       Roman Rolinsky <rolinsky@femagsoft.com>
# Created:      07.06.2007
# RCS-ID:       $Id$

import os
from globals import *
from XMLTree import XMLTree
from XMLTreeMenu import XMLTreeMenu
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

    # Load resources
    res = xrc.EmptyXmlResource()
    # !!! Blocking of assert failure occurring in older unicode builds
    try:
        quietlog = wx.LogNull()
        res.Load(os.path.join(g.basePath, 'xrced.xrc'))
    except wx._core.PyAssertionError:
        print 'PyAssertionError was ignored'
    g.res = res

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
        g.fileHistory.UseMenu(self.recentMenu)
        g.fileHistory.AddFilesToMenu()

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

        # Create toolbar
        tb = self.CreateToolBar(wx.TB_HORIZONTAL | wx.NO_BORDER | wx.TB_FLAT)
        if wx.Platform != '__WXMAC__':
            # Redefine AddSeparator on wxGTK and wxMSW to add vertical line
            def _AddSeparator():
                tb.AddControl(wx.StaticLine(tb, -1, size=(-1,23), 
                                            style=wx.LI_VERTICAL))
            tb.AddSeparator = _AddSeparator
        
        # Use tango icons and slightly wider bitmap size on Mac
        if wx.Platform in ['__WXMAC__', '__WXMSW__']:
            tb.SetToolBitmapSize((26,26))
        else:
            tb.SetToolBitmapSize((24,24))
        new_bmp  = wx.ArtProvider.GetBitmap(wx.ART_NORMAL_FILE, wx.ART_TOOLBAR)
        open_bmp = wx.ArtProvider.GetBitmap(wx.ART_FILE_OPEN, wx.ART_TOOLBAR)
        save_bmp = wx.ArtProvider.GetBitmap(wx.ART_FILE_SAVE, wx.ART_TOOLBAR)
        undo_bmp = wx.ArtProvider.GetBitmap(wx.ART_UNDO, wx.ART_TOOLBAR)
        redo_bmp = wx.ArtProvider.GetBitmap(wx.ART_REDO, wx.ART_TOOLBAR)
        cut_bmp  = wx.ArtProvider.GetBitmap(wx.ART_CUT, wx.ART_TOOLBAR)
        copy_bmp = wx.ArtProvider.GetBitmap(wx.ART_COPY, wx.ART_TOOLBAR)
        paste_bmp= wx.ArtProvider.GetBitmap(wx.ART_PASTE, wx.ART_TOOLBAR)
        tb.AddSimpleTool(wx.ID_NEW, new_bmp, 'New', 'New file')
        tb.AddSimpleTool(wx.ID_OPEN, open_bmp, 'Open', 'Open file')
        tb.AddSimpleTool(wx.ID_SAVE, save_bmp, 'Save', 'Save file')
        tb.AddSeparator()
        tb.AddSimpleTool(wx.ID_UNDO, undo_bmp, 'Undo', 'Undo')
        tb.AddSimpleTool(wx.ID_REDO, redo_bmp, 'Redo', 'Redo')
        tb.AddSeparator()
        tb.AddSimpleTool(wx.ID_CUT, cut_bmp, 'Cut', 'Cut')
        tb.AddSimpleTool(wx.ID_COPY, copy_bmp, 'Copy', 'Copy')
        tb.AddSimpleTool(self.ID_TOOL_PASTE, paste_bmp, 'Paste', 'Paste')
        tb.AddSeparator()
        bmp = wx.ArtProvider.GetBitmap(self.ART_LOCATE, wx.ART_TOOLBAR)
        tb.AddSimpleTool(self.ID_TOOL_LOCATE, bmp,
                         'Locate', 'Locate control in test window and select it', True)
        bmp = wx.ArtProvider.GetBitmap(self.ART_TEST, wx.ART_TOOLBAR)
        tb.AddSimpleTool(self.ID_TEST, bmp, 'Test', 'Test window')
        bmp = wx.ArtProvider.GetBitmap(self.ART_REFRESH, wx.ART_TOOLBAR)
        tb.AddSimpleTool(self.ID_REFRESH, bmp, 'Refresh', 'Refresh view')
        bmp = wx.ArtProvider.GetBitmap(self.ART_AUTO_REFRESH, wx.ART_TOOLBAR)
        tb.AddSimpleTool(self.ID_AUTO_REFRESH, bmp,
                         'Auto-refresh', 'Toggle auto-refresh mode', True)
        tb.AddSeparator()
        bmp = wx.ArtProvider.GetBitmap(self.ART_MOVEUP, wx.ART_TOOLBAR)
        tb.AddSimpleTool(self.ID_MOVEUP, bmp,
                         'Up', 'Move before previous sibling')
        bmp = wx.ArtProvider.GetBitmap(self.ART_MOVEDOWN, wx.ART_TOOLBAR)
        tb.AddSimpleTool(self.ID_MOVEDOWN, bmp,
                         'Down', 'Move after next sibling')
        bmp = wx.ArtProvider.GetBitmap(self.ART_MOVELEFT, wx.ART_TOOLBAR)
        tb.AddSimpleTool(self.ID_MOVELEFT, bmp,
                         'Make Sibling', 'Make sibling of parent')
        bmp = wx.ArtProvider.GetBitmap(self.ART_MOVERIGHT, wx.ART_TOOLBAR)
        tb.AddSimpleTool(self.ID_MOVERIGHT, bmp,
                         'Make Child', 'Make child of previous sibling')
        tb.ToggleTool(self.ID_AUTO_REFRESH, g.conf.autoRefresh)
        tb.Realize()
 
        self.tb = tb
        self.minWidth = tb.GetSize()[0] # minimal width is the size of toolbar

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
        miniFrame = wx.MiniFrame(self, -1, 'Attributes',
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
        pass

    def EmbedUnembed(self, embedPanel):
        conf = g.conf
        conf.embedPanel = embedPanel
        if conf.embedPanel:
            # Remember last dimentions
            conf.panelX, conf.panelY = self.miniFrame.GetPosition()
            conf.panelWidth, conf.panelHeight = self.miniFrame.GetSize()
            size = self.GetSize()
            pos = self.GetPosition()
            sizePanel = panel.GetSize()
            panel.Reparent(self.splitter)
            self.miniFrame.GetSizer().Remove(panel)
            # Widen
            self.SetDimensions(pos.x, pos.y, size.width + sizePanel.width, size.height)
            self.splitter.SplitVertically(tree, panel, conf.sashPos)
            self.miniFrame.Show(False)
        else:
            conf.sashPos = self.splitter.GetSashPosition()
            pos = self.GetPosition()
            size = self.GetSize()
            sizePanel = panel.GetSize()
            self.splitter.Unsplit(panel)
            sizer = self.miniFrame.GetSizer()
            panel.Reparent(self.miniFrame)
            panel.Show(True)
            sizer.Add(panel, 1, wx.EXPAND)
            self.miniFrame.Show(True)
            self.miniFrame.SetDimensions(conf.panelX, conf.panelY,
                                         conf.panelWidth, conf.panelHeight)
            self.miniFrame.Layout()
            # Reduce width
            self.SetDimensions(pos.x, pos.y,
                               max(size.width - sizePanel.width, self.minWidth), size.height)
        

# ScrolledMessageDialog - modified from wxPython lib to set fixed-width font
class ScrolledMessageDialog(wx.Dialog):
    def __init__(self, parent, msg, caption, pos = wx.DefaultPosition, size = (500,300)):
        from wx.lib.layoutf import Layoutf
        wx.Dialog.__init__(self, parent, -1, caption, pos, size)
        text = wx.TextCtrl(self, -1, msg, wx.DefaultPosition,
                             wx.DefaultSize, wx.TE_MULTILINE | wx.TE_READONLY)
        text.SetFont(g.modernFont())
        dc = wx.WindowDC(text)
        w, h = dc.GetFullTextExtent(' ', g.modernFont())[:2]
        ok = wx.Button(self, wx.ID_OK, "OK")
        ok.SetDefault()
        text.SetConstraints(Layoutf('t=t5#1;b=t5#2;l=l5#1;r=r5#1', (self,ok)))
        text.SetSize((w * 80 + 30, h * 40))
        text.ShowPosition(1)            # scroll to the first line
        ok.SetConstraints(Layoutf('b=b5#1;x%w50#1;w!80;h!35', (self,)))
        self.SetAutoLayout(True)
        self.Fit()
        self.CenterOnScreen(wx.BOTH)

# ArtProvider for toolbar icons
class ToolArtProvider(wx.ArtProvider):
    def __init__(self):
        wx.ArtProvider.__init__(self)
        self.images = {
            'ART_LOCATE': images.getLocateImage(),
            'ART_TEST': images.getTestImage(),
            'ART_REFRESH': images.getRefreshImage(),
            'ART_AUTO_REFRESH': images.getAutoRefreshImage(),
            'ART_MOVEUP': images.getMoveUpImage(),
            'ART_MOVEDOWN': images.getMoveDownImage(),
            'ART_MOVELEFT': images.getMoveLeftImage(),
            'ART_MOVERIGHT': images.getMoveRightImage()
            }
        if wx.Platform in ['__WXMAC__', '__WXMSW__']:
            self.images.update({
                    wx.ART_NORMAL_FILE: images.getNewImage(),
                    wx.ART_FILE_OPEN: images.getOpenImage(),
                    wx.ART_FILE_SAVE: images.getSaveImage(),
                    wx.ART_UNDO: images.getUndoImage(),
                    wx.ART_REDO: images.getRedoImage(),
                    wx.ART_CUT: images.getCutImage(),
                    wx.ART_COPY: images.getCopyImage(),
                    wx.ART_PASTE: images.getPasteImage()
                    })
    def CreateBitmap(self, id, client, size):
        bmp = wx.NullBitmap
        if id in self.images:
            img = self.images[id]
            # Alpha not implemented completely there
            if wx.Platform in ['__WXMAC__', '__WXMSW__']:
                img.ConvertAlphaToMask()
            bmp = wx.BitmapFromImage(img)
        return bmp

