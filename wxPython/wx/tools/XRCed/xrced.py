# Name:         xrced.py
# Purpose:      XRC editor, main module
# Author:       Roman Rolinsky <rolinsky@mema.ucl.ac.be>
# Created:      20.08.2001
# RCS-ID:       $Id$

"""

xrced -- Simple resource editor for XRC format used by wxWidgets/wxPython
         GUI toolkit.

Usage:

    xrced [ -h ] [ -v ] [ XRC-file ]

Options:

    -h          output short usage info and exit

    -v          output version info and exit
"""

from globals import *
import os, sys, getopt, re, traceback, tempfile, shutil, cPickle
from xml.parsers import expat

# Local modules
from tree import *                      # imports xxx which imports params
from panel import *
from tools import *
from params import genericStyles
# Cleanup recursive import sideeffects, otherwise we can't create undoMan
import undo
undo.ParamPage = ParamPage
undoMan = g.undoMan = UndoManager()

# Set application path for loading resources
if __name__ == '__main__':
    basePath = os.path.dirname(sys.argv[0])
else:
    basePath = os.path.dirname(__file__)

# Remember system path
sys_path = sys.path

# 1 adds CMD command to Help menu
debug = 0

g.helpText = """\
<HTML><H2>Welcome to XRC<font color="blue">ed</font></H2><H3><font color="green">DON'T PANIC :)</font></H3>
Read this note before clicking on anything!<P>
To start select tree root, then popup menu with your right mouse button,
select "Append Child", and then any command.<P>
Or just press one of the buttons on the tools palette.<P>
Enter XML ID, change properties, create children.<P>
To test your interface select Test command (View menu).<P>
Consult README.txt file for the details.</HTML>
"""

defaultIDs = {xxxPanel:'PANEL', xxxDialog:'DIALOG', xxxFrame:'FRAME',
              xxxMenuBar:'MENUBAR', xxxMenu:'MENU', xxxToolBar:'TOOLBAR',
              xxxWizard:'WIZARD', xxxBitmap:'BITMAP', xxxIcon:'ICON'}

defaultName = 'UNTITLED.xrc'

################################################################################

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

################################################################################

# Event handler for using during location
class Locator(wx.EvtHandler):
    def ProcessEvent(self, evt):
        print evt

class Frame(wx.Frame):
    def __init__(self, pos, size):
        wx.Frame.__init__(self, None, -1, '', pos, size)
        global frame
        frame = g.frame = self
        bar = self.CreateStatusBar(2)
        bar.SetStatusWidths([-1, 40])
        self.SetIcon(images.getIconIcon())

        # Idle flag
        self.inIdle = False

        # Load our own resources
        self.res = xrc.XmlResource('')
        # !!! Blocking of assert failure occurring in older unicode builds
        try:
            quietlog = wx.LogNull()
            self.res.Load(os.path.join(basePath, 'xrced.xrc'))
        except wx._core.PyAssertionError:
            print 'PyAssertionError was ignored'

        # Make menus
        menuBar = wx.MenuBar()

        menu = wx.Menu()
        menu.Append(wx.ID_NEW, '&New\tCtrl-N', 'New file')
        menu.AppendSeparator()
        menu.Append(wx.ID_OPEN, '&Open...\tCtrl-O', 'Open XRC file')
        self.recentMenu = wx.Menu()
        self.AppendRecent(self.recentMenu)
        menu.AppendMenu(-1, 'Open Recent', self.recentMenu, 'Open a recent file')
        menu.AppendSeparator()
        menu.Append(wx.ID_SAVE, '&Save\tCtrl-S', 'Save XRC file')
        menu.Append(wx.ID_SAVEAS, 'Save &As...', 'Save XRC file under different name')
        self.ID_GENERATE_PYTHON = wx.NewId()
        menu.Append(self.ID_GENERATE_PYTHON, '&Generate Python...', 
                    'Generate a Python module that uses this XRC')
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
        self.ID_DELETE = wx.NewId()
        menu.Append(self.ID_DELETE, '&Delete\tCtrl-D', 'Delete object')
        menu.AppendSeparator()
        self.ID_LOCATE = wx.NewId()
        self.ID_TOOL_LOCATE = wx.NewId()
        self.ID_TOOL_PASTE = wx.NewId()
        menu.Append(self.ID_LOCATE, '&Locate\tCtrl-L', 'Locate control in test window and select it')
        menuBar.Append(menu, '&Edit')

        menu = wx.Menu()
        self.ID_EMBED_PANEL = wx.NewId()
        menu.Append(self.ID_EMBED_PANEL, '&Embed Panel',
                    'Toggle embedding properties panel in the main window', True)
        menu.Check(self.ID_EMBED_PANEL, conf.embedPanel)
        self.ID_SHOW_TOOLS = wx.NewId()
        menu.Append(self.ID_SHOW_TOOLS, 'Show &Tools', 'Toggle tools', True)
        menu.Check(self.ID_SHOW_TOOLS, conf.showTools)
        menu.AppendSeparator()
        self.ID_TEST = wx.NewId()
        menu.Append(self.ID_TEST, '&Test\tF5', 'Show test window')
        self.ID_REFRESH = wx.NewId()
        menu.Append(self.ID_REFRESH, '&Refresh\tCtrl-R', 'Refresh test window')
        self.ID_AUTO_REFRESH = wx.NewId()
        menu.Append(self.ID_AUTO_REFRESH, '&Auto-refresh\tCtrl-A',
                    'Toggle auto-refresh mode', True)
        menu.Check(self.ID_AUTO_REFRESH, conf.autoRefresh)
        self.ID_TEST_HIDE = wx.NewId()
        menu.Append(self.ID_TEST_HIDE, '&Hide\tF6', 'Close test window')
        menuBar.Append(menu, '&View')

        menu = wx.Menu()
        self.ID_MOVEUP = wx.NewId()
        menu.Append(self.ID_MOVEUP, '&Up', 'Move before previous sibling')
        self.ID_MOVEDOWN = wx.NewId()
        menu.Append(self.ID_MOVEDOWN, '&Down', 'Move after next sibling')
        self.ID_MOVELEFT = wx.NewId()
        menu.Append(self.ID_MOVELEFT, '&Make sibling', 'Make sibling of parent')
        self.ID_MOVERIGHT = wx.NewId()
        menu.Append(self.ID_MOVERIGHT, '&Make child', 'Make child of previous sibling')
        menuBar.Append(menu, '&Move')

        menu = wx.Menu()
        menu.Append(wx.ID_ABOUT, '&About...', 'About XCRed')
        self.ID_README = wx.NewId()
        menu.Append(self.ID_README, '&Readme...\tF1', 'View the README file')
        if debug:
            self.ID_DEBUG_CMD = wx.NewId()
            menu.Append(self.ID_DEBUG_CMD, 'CMD', 'Python command line')
            wx.EVT_MENU(self, self.ID_DEBUG_CMD, self.OnDebugCMD)
        menuBar.Append(menu, '&Help')

        self.menuBar = menuBar
        self.SetMenuBar(menuBar)

        # Create toolbar
        tb = self.CreateToolBar(wx.TB_HORIZONTAL | wx.NO_BORDER | wx.TB_FLAT)
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
        tb.AddControl(wx.StaticLine(tb, -1, size=(-1,23), style=wx.LI_VERTICAL))
        tb.AddSimpleTool(wx.ID_UNDO, undo_bmp, 'Undo', 'Undo')
        tb.AddSimpleTool(wx.ID_REDO, redo_bmp, 'Redo', 'Redo')
        tb.AddControl(wx.StaticLine(tb, -1, size=(-1,23), style=wx.LI_VERTICAL))
        tb.AddSimpleTool(wx.ID_CUT, cut_bmp, 'Cut', 'Cut')
        tb.AddSimpleTool(wx.ID_COPY, copy_bmp, 'Copy', 'Copy')
        tb.AddSimpleTool(self.ID_TOOL_PASTE, paste_bmp, 'Paste', 'Paste')
        tb.AddControl(wx.StaticLine(tb, -1, size=(-1,23), style=wx.LI_VERTICAL))
        tb.AddSimpleTool(self.ID_TOOL_LOCATE,
                        images.getLocateBitmap(), #images.getLocateArmedBitmap(),
                        'Locate', 'Locate control in test window and select it', True)
        tb.AddControl(wx.StaticLine(tb, -1, size=(-1,23), style=wx.LI_VERTICAL))
        tb.AddSimpleTool(self.ID_TEST, images.getTestBitmap(), 'Test', 'Test window')
        tb.AddSimpleTool(self.ID_REFRESH, images.getRefreshBitmap(),
                         'Refresh', 'Refresh view')
        tb.AddSimpleTool(self.ID_AUTO_REFRESH, images.getAutoRefreshBitmap(),
                         'Auto-refresh', 'Toggle auto-refresh mode', True)
        tb.AddControl(wx.StaticLine(tb, -1, size=(-1,23), style=wx.LI_VERTICAL))
        tb.AddSimpleTool(self.ID_MOVEUP, images.getToolMoveUpBitmap(),
                         'Up', 'Move before previous sibling')
        tb.AddSimpleTool(self.ID_MOVEDOWN, images.getToolMoveDownBitmap(),
                         'Down', 'Move after next sibling')
        tb.AddSimpleTool(self.ID_MOVELEFT, images.getToolMoveLeftBitmap(),
                         'Make Sibling', 'Make sibling of parent')
        tb.AddSimpleTool(self.ID_MOVERIGHT, images.getToolMoveRightBitmap(),
                         'Make Child', 'Make child of previous sibling')
#        if wx.Platform == '__WXGTK__':
#            tb.AddSeparator()   # otherwise auto-refresh sticks in status line
        tb.ToggleTool(self.ID_AUTO_REFRESH, conf.autoRefresh)
        tb.Realize()
 
        self.tb = tb
        self.minWidth = tb.GetSize()[0] # minimal width is the size of toolbar

        # File
        wx.EVT_MENU(self, wx.ID_NEW, self.OnNew)
        wx.EVT_MENU(self, wx.ID_OPEN, self.OnOpen)
        wx.EVT_MENU(self, wx.ID_SAVE, self.OnSaveOrSaveAs)
        wx.EVT_MENU(self, wx.ID_SAVEAS, self.OnSaveOrSaveAs)
        wx.EVT_MENU(self, self.ID_GENERATE_PYTHON, self.OnGeneratePython)
        wx.EVT_MENU(self, wx.ID_EXIT, self.OnExit)
        # Edit
        wx.EVT_MENU(self, wx.ID_UNDO, self.OnUndo)
        wx.EVT_MENU(self, wx.ID_REDO, self.OnRedo)
        wx.EVT_MENU(self, wx.ID_CUT, self.OnCutDelete)
        wx.EVT_MENU(self, wx.ID_COPY, self.OnCopy)
        wx.EVT_MENU(self, wx.ID_PASTE, self.OnPaste)
        wx.EVT_MENU(self, self.ID_TOOL_PASTE, self.OnPaste)
        wx.EVT_MENU(self, self.ID_DELETE, self.OnCutDelete)
        wx.EVT_MENU(self, self.ID_LOCATE, self.OnLocate)
        wx.EVT_MENU(self, self.ID_TOOL_LOCATE, self.OnLocate)
        # View
        wx.EVT_MENU(self, self.ID_EMBED_PANEL, self.OnEmbedPanel)
        wx.EVT_MENU(self, self.ID_SHOW_TOOLS, self.OnShowTools)
        wx.EVT_MENU(self, self.ID_TEST, self.OnTest)
        wx.EVT_MENU(self, self.ID_REFRESH, self.OnRefresh)
        wx.EVT_MENU(self, self.ID_AUTO_REFRESH, self.OnAutoRefresh)
        wx.EVT_MENU(self, self.ID_TEST_HIDE, self.OnTestHide)
        # Move
        wx.EVT_MENU(self, self.ID_MOVEUP, self.OnMoveUp)
        wx.EVT_MENU(self, self.ID_MOVEDOWN, self.OnMoveDown)
        wx.EVT_MENU(self, self.ID_MOVELEFT, self.OnMoveLeft)
        wx.EVT_MENU(self, self.ID_MOVERIGHT, self.OnMoveRight)        
        # Help
        wx.EVT_MENU(self, wx.ID_ABOUT, self.OnAbout)
        wx.EVT_MENU(self, self.ID_README, self.OnReadme)

        # Update events
        wx.EVT_UPDATE_UI(self, wx.ID_SAVE, self.OnUpdateUI)
        wx.EVT_UPDATE_UI(self, wx.ID_CUT, self.OnUpdateUI)
        wx.EVT_UPDATE_UI(self, wx.ID_COPY, self.OnUpdateUI)
        wx.EVT_UPDATE_UI(self, wx.ID_PASTE, self.OnUpdateUI)
        wx.EVT_UPDATE_UI(self, self.ID_LOCATE, self.OnUpdateUI)
        wx.EVT_UPDATE_UI(self, self.ID_TOOL_LOCATE, self.OnUpdateUI)
        wx.EVT_UPDATE_UI(self, self.ID_TOOL_PASTE, self.OnUpdateUI)
        wx.EVT_UPDATE_UI(self, wx.ID_UNDO, self.OnUpdateUI)
        wx.EVT_UPDATE_UI(self, wx.ID_REDO, self.OnUpdateUI)
        wx.EVT_UPDATE_UI(self, self.ID_DELETE, self.OnUpdateUI)
        wx.EVT_UPDATE_UI(self, self.ID_TEST, self.OnUpdateUI)
        wx.EVT_UPDATE_UI(self, self.ID_REFRESH, self.OnUpdateUI)

        # Build interface
        sizer = wx.BoxSizer(wx.VERTICAL)
        sizer.Add(wx.StaticLine(self, -1), 0, wx.EXPAND)
        # Horizontal sizer for toolbar and splitter
        self.toolsSizer = sizer1 = wx.BoxSizer()
        splitter = wx.SplitterWindow(self, -1, style=wx.SP_3DSASH)
        self.splitter = splitter
        splitter.SetMinimumPaneSize(100)
        # Create tree
        global tree
        g.tree = tree = XML_Tree(splitter, -1)

        # Init pull-down menu data
        global pullDownMenu
        g.pullDownMenu = pullDownMenu = PullDownMenu(self)

        # Vertical toolbar for GUI buttons
        g.tools = tools = Tools(self)
        tools.Show(conf.showTools)
        if conf.showTools: sizer1.Add(tools, 0, wx.EXPAND)

        tree.RegisterKeyEvents()

        # Miniframe for split mode
        miniFrame = wx.MiniFrame(self, -1, 'Properties & Style',
                                 (conf.panelX, conf.panelY),
                                 (conf.panelWidth, conf.panelHeight))
        self.miniFrame = miniFrame
        sizer2 = wx.BoxSizer()
        miniFrame.SetAutoLayout(True)
        miniFrame.SetSizer(sizer2)
        wx.EVT_CLOSE(self.miniFrame, self.OnCloseMiniFrame)
        # Create panel for parameters
        global panel
        if conf.embedPanel:
            panel = Panel(splitter)
            # Set plitter windows
            splitter.SplitVertically(tree, panel, conf.sashPos)
        else:
            panel = Panel(miniFrame)
            sizer2.Add(panel, 1, wx.EXPAND)
            miniFrame.Show(True)
            splitter.Initialize(tree)
        sizer1.Add(splitter, 1, wx.EXPAND)
        sizer.Add(sizer1, 1, wx.EXPAND)
        self.SetAutoLayout(True)
        self.SetSizer(sizer)

        # Save sys.modules names
        self.modules = set(sys.modules.keys())
        
        # Initialize
        self.Clear()

        # Other events
        wx.EVT_IDLE(self, self.OnIdle)
        wx.EVT_CLOSE(self, self.OnCloseWindow)
        wx.EVT_KEY_DOWN(self, tools.OnKeyDown)
        wx.EVT_KEY_UP(self, tools.OnKeyUp)
        wx.EVT_ICONIZE(self, self.OnIconize)
    
    def AppendRecent(self, menu):
        # add recently used files to the menu
        for id,name in conf.recentfiles.iteritems():
            menu.Append(id,name)
            wx.EVT_MENU(self,id,self.OnRecentFile)
        return 
        
    def OnRecentFile(self,evt):
        # open recently used file
        if not self.AskSave(): return
        wx.BeginBusyCursor()
        try:
            path=conf.recentfiles[evt.GetId()]
            if self.Open(path):
                self.SetStatusText('Data loaded')
            else:
                self.SetStatusText('Failed')
        except KeyError:
            self.SetStatusText('No such file')
        wx.EndBusyCursor()

    def OnNew(self, evt):
        if not self.AskSave(): return
        self.Clear()

    def OnOpen(self, evt):
        if not self.AskSave(): return
        dlg = wx.FileDialog(self, 'Open', os.path.dirname(self.dataFile),
                           '', '*.xrc', wx.OPEN | wx.CHANGE_DIR)
        if dlg.ShowModal() == wx.ID_OK:
            path = dlg.GetPath()
            self.SetStatusText('Loading...')
            wx.BeginBusyCursor()
            try:
                if self.Open(path):
                    self.SetStatusText('Data loaded')
                else:
                    self.SetStatusText('Failed')
                self.SaveRecent(path)
            finally:
                wx.EndBusyCursor()
        dlg.Destroy()

    def OnSaveOrSaveAs(self, evt):
        if evt.GetId() == wx.ID_SAVEAS or not self.dataFile:
            if self.dataFile: name = ''
            else: name = defaultName
            dirname = os.path.abspath(os.path.dirname(self.dataFile))
            dlg = wx.FileDialog(self, 'Save As', dirname, name, '*.xrc',
                               wx.SAVE | wx.OVERWRITE_PROMPT | wx.CHANGE_DIR)
            if dlg.ShowModal() == wx.ID_OK:
                path = dlg.GetPath()
                if isinstance(path, unicode):
                    path = path.encode(sys.getfilesystemencoding())
                dlg.Destroy()
            else:
                dlg.Destroy()
                return

            if conf.localconf:
                # if we already have a localconf then it needs to be
                # copied to a new config with the new name
                lc = conf.localconf
                nc = self.CreateLocalConf(path)
                flag, key, idx = lc.GetFirstEntry()
                while flag:
                    nc.Write(key, lc.Read(key))
                    flag, key, idx = lc.GetNextEntry(idx)
                conf.localconf = nc
            else:
                # otherwise create a new one
                conf.localconf = self.CreateLocalConf(path)
        else:
            path = self.dataFile
        self.SetStatusText('Saving...')
        wx.BeginBusyCursor()
        try:
            try:
                tmpFile,tmpName = tempfile.mkstemp(prefix='xrced-')
                os.close(tmpFile)
                self.Save(tmpName) # save temporary file first
                shutil.move(tmpName, path)
                self.dataFile = path
                if conf.localconf.ReadBool("autogenerate", False):
                    pypath = conf.localconf.Read("filename")
                    embed = conf.localconf.ReadBool("embedResource", False)
                    genGettext = conf.localconf.ReadBool("genGettext", False)
                    self.GeneratePython(self.dataFile, pypath, embed, genGettext)
                    
                self.SetStatusText('Data saved')
                self.SaveRecent(path)
            except IOError:
                self.SetStatusText('Failed')
        finally:
            wx.EndBusyCursor()        

    def SaveRecent(self,path):
        # append to recently used files
        if path not in conf.recentfiles.values():
            newid = wx.NewId()
            self.recentMenu.Append(newid, path)
            wx.EVT_MENU(self, newid, self.OnRecentFile)
            conf.recentfiles[newid] = path

    def GeneratePython(self, dataFile, pypath, embed, genGettext):
        try:
            import wx.tools.pywxrc
            rescomp = wx.tools.pywxrc.XmlResourceCompiler()
            rescomp.MakePythonModule([dataFile], pypath, embed, genGettext)
        except:
            inf = sys.exc_info()
            wx.LogError(traceback.format_exception(inf[0], inf[1], None)[-1])
            wx.LogError('Error generating python code : %s' % pypath)
            raise
        

    def OnGeneratePython(self, evt):
        if self.modified or not conf.localconf:
            wx.MessageBox("Save the XRC file first!", "Error")
            return
        
        dlg = PythonOptions(self, conf.localconf, self.dataFile)
        dlg.ShowModal()
        dlg.Destroy()

        
    def OnExit(self, evt):
        self.Close()

    def OnUndo(self, evt):
        # Extra check to not mess with idle updating
        if undoMan.CanUndo():
            undoMan.Undo()

    def OnRedo(self, evt):
        if undoMan.CanRedo():
            undoMan.Redo()

    def OnCopy(self, evt):
        selected = tree.selection
        if not selected: return         # key pressed event
        xxx = tree.GetPyData(selected)
        if wx.TheClipboard.Open():
            if xxx.isElement:
                data = wx.CustomDataObject('XRCED')
                # Set encoding in header
                # (False,True)
                s = xxx.node.toxml(encoding=expat.native_encoding)
            else:
                data = wx.CustomDataObject('XRCED_node')
                s = xxx.node.data
            data.SetData(cPickle.dumps(s))
            wx.TheClipboard.SetData(data)
            wx.TheClipboard.Close()
            self.SetStatusText('Copied')
        else:
            wx.MessageBox("Unable to open the clipboard", "Error")

    def OnPaste(self, evt):
        selected = tree.selection
        if not selected: return         # key pressed event
        # For pasting with Ctrl pressed
        appendChild = True
        if evt.GetId() == pullDownMenu.ID_PASTE_SIBLING: appendChild = False
        elif evt.GetId() == self.ID_TOOL_PASTE:
            if g.tree.ctrl: appendChild = False
            else: appendChild = not tree.NeedInsert(selected)
        else: appendChild = not tree.NeedInsert(selected)
        xxx = tree.GetPyData(selected)
        if not appendChild:
            # If has next item, insert, else append to parent
            nextItem = tree.GetNextSibling(selected)
            parentLeaf = tree.GetItemParent(selected)
        # Expanded container (must have children)
        elif tree.IsExpanded(selected) and tree.GetChildrenCount(selected, False):
            # Insert as first child
            nextItem = tree.GetFirstChild(selected)[0]
            parentLeaf = selected
        else:
            # No children or unexpanded item - appendChild stays True
            nextItem = wx.TreeItemId()   # no next item
            parentLeaf = selected
        parent = tree.GetPyData(parentLeaf).treeObject()

        # Create a copy of clipboard pickled element
        success = success_node = False
        if wx.TheClipboard.Open():
            try:
                data = wx.CustomDataObject('XRCED')
                if wx.TheClipboard.IsSupported(data.GetFormat()):
                    try:
                        success = wx.TheClipboard.GetData(data)
                    except:
                        # there is a problem if XRCED_node is in clipboard
                        # but previous SetData was for XRCED
                        pass
                if not success:             # try other format
                    data = wx.CustomDataObject('XRCED_node')
                    if wx.TheClipboard.IsSupported(data.GetFormat()):
                        success_node = wx.TheClipboard.GetData(data)
            finally:
                wx.TheClipboard.Close()

        if not success and not success_node:
            wx.MessageBox(
                "There is no data in the clipboard in the required format",
                "Error")
            return

        xml = cPickle.loads(data.GetData()) # xml representation of element
        if success:
            elem = minidom.parseString(xml).childNodes[0]
        else:
            elem = g.tree.dom.createComment(xml)
        
        # Tempopary xxx object to test things
        xxx = MakeXXXFromDOM(parent, elem)
        
        # Check compatibility
        if not self.ItemsAreCompatible(parent, xxx.treeObject()): return

        # Check parent and child relationships.
        # If parent is sizer or notebook, child is of wrong class or
        # parent is normal window, child is child container then detach child.
        isChildContainer = isinstance(xxx, xxxChildContainer)
        parentIsBook = parent.__class__ in [xxxNotebook, xxxChoicebook, xxxListbook]
        if isChildContainer and \
           ((parent.isSizer and not isinstance(xxx, xxxSizerItem)) or \
            (parentIsBook and not isinstance(xxx, xxxPage)) or \
           not (parent.isSizer or parentIsBook)):
            elem.removeChild(xxx.child.node) # detach child
            elem.unlink()           # delete child container
            elem = xxx.child.node # replace
            # This may help garbage collection
            xxx.child.parent = None
            isChildContainer = False
        # Parent is sizer or notebook, child is not child container
        if parent.isSizer and not isChildContainer and not isinstance(xxx, xxxSpacer):
            # Create sizer item element
            sizerItemElem = MakeEmptyDOM(parent.itemTag)
            sizerItemElem.appendChild(elem)
            elem = sizerItemElem
        elif isinstance(parent, xxxNotebook) and not isChildContainer:
            pageElem = MakeEmptyDOM('notebookpage')
            pageElem.appendChild(elem)
            elem = pageElem
        elif isinstance(parent, xxxChoicebook) and not isChildContainer:
            pageElem = MakeEmptyDOM('choicebookpage')
            pageElem.appendChild(elem)
            elem = pageElem
        elif isinstance(parent, xxxListbook) and not isChildContainer:
            pageElem = MakeEmptyDOM('listbookpage')
            pageElem.appendChild(elem)
            elem = pageElem
        # Insert new node, register undo
        newItem = tree.InsertNode(parentLeaf, parent, elem, nextItem)
        undoMan.RegisterUndo(UndoPasteCreate(parentLeaf, parent, newItem, selected))
        # Scroll to show new item (!!! redundant?)
        tree.EnsureVisible(newItem)
        tree.SelectItem(newItem)
        if not tree.IsVisible(newItem):
            tree.ScrollTo(newItem)
            tree.Refresh()
        # Update view?
        if g.testWin and tree.IsHighlatable(newItem):
            if conf.autoRefresh:
                tree.needUpdate = True
                tree.pendingHighLight = newItem
            else:
                tree.pendingHighLight = None
        self.SetModified()
        self.SetStatusText('Pasted')


    def ItemsAreCompatible(self, parent, child):
        # Check compatibility
        error = False
        # Comments are always compatible
        if child.__class__ == xxxComment:
            return True
        # Top-level
        if child.__class__ in [xxxDialog, xxxFrame, xxxWizard]:
            # Top-level classes
            if parent.__class__ != xxxMainNode: error = True
        elif child.__class__ == xxxMenuBar:
            # Menubar can be put in frame or dialog
            if parent.__class__ not in [xxxMainNode, xxxFrame, xxxDialog]: error = True
        elif child.__class__ == xxxToolBar:
            # Toolbar can be top-level of child of panel or frame
            if parent.__class__ not in [xxxMainNode, xxxPanel, xxxFrame] and \
               not parent.isSizer: error = True
        elif child.__class__ == xxxPanel and parent.__class__ == xxxMainNode:
            pass
        elif child.__class__ == xxxSpacer:
            if not parent.isSizer: error = True
        elif child.__class__ == xxxSeparator:
            if not parent.__class__ in [xxxMenu, xxxToolBar]: error = True
        elif child.__class__ == xxxTool:
            if parent.__class__ != xxxToolBar: error = True
        elif child.__class__ == xxxMenu:
            if not parent.__class__ in [xxxMainNode, xxxMenuBar, xxxMenu]: error = True
        elif child.__class__ == xxxMenuItem:
            if not parent.__class__ in [xxxMenuBar, xxxMenu]: error = True
        elif child.isSizer and parent.__class__ in [xxxNotebook, xxxChoicebook, xxxListbook]:
            error = True
        else:                           # normal controls can be almost anywhere
            if parent.__class__ == xxxMainNode or \
               parent.__class__ in [xxxMenuBar, xxxMenu]: error = True
        if error:
            if parent.__class__ == xxxMainNode: parentClass = 'root'
            else: parentClass = parent.className
            wx.LogError('Incompatible parent/child: parent is %s, child is %s!' %
                       (parentClass, child.className))
            return False
        return True

    def OnMoveUp(self, evt):
        selected = tree.selection
        if not selected: return

        index = tree.ItemIndex(selected)
        if index == 0: return # No previous sibling found

        # Undo info
        self.lastOp = 'MOVEUP'
        status = 'Moved before previous sibling'

        # Prepare undo data
        panel.Apply()

        parent = tree.GetItemParent(selected)
        elem = tree.RemoveLeaf(selected)
        nextItem = tree.GetFirstChild(parent)[0]
        for i in range(index - 1): nextItem = tree.GetNextSibling(nextItem)
        selected = tree.InsertNode(parent, tree.GetPyData(parent).treeObject(), elem, nextItem)
        newIndex = tree.ItemIndex(selected)
        tree.SelectItem(selected)

        undoMan.RegisterUndo(UndoMove(parent, index, parent, newIndex))

        self.modified = True
        self.SetStatusText(status)

        return

    def OnMoveDown(self, evt):
        selected = tree.selection
        if not selected: return

        index = tree.ItemIndex(selected)
        next = tree.GetNextSibling(selected)
        if not next: return

        # Undo info
        self.lastOp = 'MOVEDOWN'
        status = 'Moved after next sibling'

        # Prepare undo data
        panel.Apply()

        parent = tree.GetItemParent(selected)
        elem = tree.RemoveLeaf(selected)
        nextItem = tree.GetFirstChild(parent)[0]
        for i in range(index + 1): nextItem = tree.GetNextSibling(nextItem)
        selected = tree.InsertNode(parent, tree.GetPyData(parent).treeObject(), elem, nextItem)
        newIndex = tree.ItemIndex(selected)
        tree.SelectItem(selected)

        undoMan.RegisterUndo(UndoMove(parent, index, parent, newIndex))

        self.modified = True
        self.SetStatusText(status)

        return
    
    def OnMoveLeft(self, evt):
        selected = tree.selection
        if not selected: return

        oldParent = tree.GetItemParent(selected)
        if not oldParent: return
        pparent = tree.GetItemParent(oldParent)
        if not pparent: return

        # Check compatibility
        if not self.ItemsAreCompatible(tree.GetPyData(pparent).treeObject(), tree.GetPyData(selected).treeObject()): return

        # Undo info
        self.lastOp = 'MOVELEFT'
        status = 'Made next sibling of parent'

        oldIndex = tree.ItemIndex(selected)
        elem = tree.RemoveLeaf(selected)
        nextItem = tree.GetFirstChild(pparent)[0]
        parentIndex = tree.ItemIndex(oldParent)
        for i in range(parentIndex + 1): nextItem = tree.GetNextSibling(nextItem)

        # Check parent and child relationships.
        # If parent is sizer or notebook, child is of wrong class or
        # parent is normal window, child is child container then detach child.
        parent = tree.GetPyData(pparent).treeObject()
        xxx = MakeXXXFromDOM(parent, elem)
        isChildContainer = isinstance(xxx, xxxChildContainer)
        if isChildContainer and \
           ((parent.isSizer and not isinstance(xxx, xxxSizerItem)) or \
            (isinstance(parent, xxxNotebook) and not isinstance(xxx, xxxNotebookPage)) or \
           not (parent.isSizer or isinstance(parent, xxxNotebook))):
            elem.removeChild(xxx.child.node) # detach child
            elem.unlink()           # delete child container
            elem = xxx.child.node # replace
            # This may help garbage collection
            xxx.child.parent = None
            isChildContainer = False
        # Parent is sizer or notebook, child is not child container
        if parent.isSizer and not isChildContainer and not isinstance(xxx, xxxSpacer):
            # Create sizer item element
            sizerItemElem = MakeEmptyDOM('sizeritem')
            sizerItemElem.appendChild(elem)
            elem = sizerItemElem
        elif isinstance(parent, xxxNotebook) and not isChildContainer:
            pageElem = MakeEmptyDOM('notebookpage')
            pageElem.appendChild(elem)
            elem = pageElem

        selected = tree.InsertNode(pparent, tree.GetPyData(pparent).treeObject(), elem, nextItem)
        newIndex = tree.ItemIndex(selected)
        tree.SelectItem(selected)

        undoMan.RegisterUndo(UndoMove(oldParent, oldIndex, pparent, newIndex))
        
        self.modified = True
        self.SetStatusText(status)

    def OnMoveRight(self, evt):
        selected = tree.selection
        if not selected: return

        oldParent = tree.GetItemParent(selected)
        if not oldParent: return
        
        newParent = tree.GetPrevSibling(selected)
        if not newParent: return

        parent = tree.GetPyData(newParent).treeObject()

        # Check compatibility
        if not self.ItemsAreCompatible(parent, tree.GetPyData(selected).treeObject()): return

        # Undo info
        self.lastOp = 'MOVERIGHT'
        status = 'Made last child of previous sibling'

        oldIndex = tree.ItemIndex(selected)
        elem = tree.RemoveLeaf(selected)

        # Check parent and child relationships.
        # If parent is sizer or notebook, child is of wrong class or
        # parent is normal window, child is child container then detach child.
        xxx = MakeXXXFromDOM(parent, elem)
        isChildContainer = isinstance(xxx, xxxChildContainer)
        if isChildContainer and \
           ((parent.isSizer and not isinstance(xxx, xxxSizerItem)) or \
            (isinstance(parent, xxxNotebook) and not isinstance(xxx, xxxNotebookPage)) or \
           not (parent.isSizer or isinstance(parent, xxxNotebook))):
            elem.removeChild(xxx.child.node) # detach child
            elem.unlink()           # delete child container
            elem = xxx.child.node # replace
            # This may help garbage collection
            xxx.child.parent = None
            isChildContainer = False
        # Parent is sizer or notebook, child is not child container
        if parent.isSizer and not isChildContainer and not isinstance(xxx, xxxSpacer):
            # Create sizer item element
            sizerItemElem = MakeEmptyDOM('sizeritem')
            sizerItemElem.appendChild(elem)
            elem = sizerItemElem
        elif isinstance(parent, xxxNotebook) and not isChildContainer:
            pageElem = MakeEmptyDOM('notebookpage')
            pageElem.appendChild(elem)
            elem = pageElem

        selected = tree.InsertNode(newParent, tree.GetPyData(newParent).treeObject(), elem, wx.TreeItemId())

        newIndex = tree.ItemIndex(selected)
        tree.SelectItem(selected)

        undoMan.RegisterUndo(UndoMove(oldParent, oldIndex, newParent, newIndex))
        
        self.modified = True
        self.SetStatusText(status)

        
    def OnCutDelete(self, evt):
        selected = tree.selection
        if not selected: return         # key pressed event
        # Undo info
        if evt.GetId() == wx.ID_CUT:
            self.lastOp = 'CUT'
            status = 'Removed to clipboard'
        else:
            self.lastOp = 'DELETE'
            status = 'Deleted'
        # Delete testWin?
        if g.testWin:
            # If deleting top-level item, delete testWin
            if selected == g.testWin.item:
                g.testWin.Destroy()
                g.testWin = None
            else:
                # Remove highlight, update testWin
                if g.testWin.highLight:
                    g.testWin.highLight.Remove()
                tree.needUpdate = True
        # Prepare undo data
        panel.Apply()
        index = tree.ItemFullIndex(selected)
        xxx = tree.GetPyData(selected)
        parent = tree.GetPyData(tree.GetItemParent(selected)).treeObject()
        elem = tree.RemoveLeaf(selected)
        undoMan.RegisterUndo(UndoCutDelete(index, parent, elem))
        if evt.GetId() == wx.ID_CUT:
            if wx.TheClipboard.Open():
                if xxx.isElement:
                    data = wx.CustomDataObject('XRCED')
                    # (False, True)
                    s = elem.toxml(encoding=expat.native_encoding)
                else:
                    data = wx.CustomDataObject('XRCED_node')
                    s = xxx.node.data
                data.SetData(cPickle.dumps(s))
                wx.TheClipboard.SetData(data)
                wx.TheClipboard.Close()
            else:
                wx.MessageBox("Unable to open the clipboard", "Error")
        tree.pendingHighLight = None
        tree.UnselectAll()
        tree.selection = None
        # Update tools
        g.tools.UpdateUI()
        panel.Clear()
        self.SetModified()
        self.SetStatusText(status)

    def OnSubclass(self, evt):
        selected = tree.selection
        xxx = tree.GetPyData(selected).treeObject()
        elem = xxx.node
        subclass = xxx.subclass
        dlg = wx.TextEntryDialog(self, 'Subclass:', defaultValue=subclass)
        if dlg.ShowModal() == wx.ID_OK:
            subclass = dlg.GetValue()
            if subclass:
                elem.setAttribute('subclass', subclass)
            elif elem.hasAttribute('subclass'):
                elem.removeAttribute('subclass')
            self.SetModified()
            xxx.subclass = elem.getAttribute('subclass')
            tree.SetItemText(selected, xxx.treeName())
            panel.pages[0].box.SetLabel(xxx.panelName())
        dlg.Destroy()

    def OnEmbedPanel(self, evt):
        conf.embedPanel = evt.IsChecked()
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

    def OnShowTools(self, evt):
        conf.showTools = evt.IsChecked()
        g.tools.Show(conf.showTools)
        if conf.showTools:
            self.toolsSizer.Prepend(g.tools, 0, wx.EXPAND)
        else:
            self.toolsSizer.Remove(g.tools)
        self.toolsSizer.Layout()
        
    def OnTest(self, evt):
        if not tree.selection: return   # key pressed event
        tree.ShowTestWindow(tree.selection)

    def OnTestHide(self, evt):
        tree.CloseTestWindow()

    # Find object by relative position
    def FindObject(self, item, obj):
        # We simply perform depth-first traversal, sinse it's too much
        # hassle to deal with all sizer/window combinations
        w = tree.FindNodeObject(item)
        if w == obj or isinstance(w, wx.GBSizerItem) and w.GetWindow() == obj:
            return item
        if tree.ItemHasChildren(item):
            child = tree.GetFirstChild(item)[0]
            while child:
                found = self.FindObject(child, obj)
                if found: return found
                child = tree.GetNextSibling(child)
        return None

    def OnTestWinLeftDown(self, evt):
        pos = evt.GetPosition()
        self.SetHandler(g.testWin)
        g.testWin.Disconnect(wx.ID_ANY, wx.ID_ANY, wx.wxEVT_LEFT_DOWN)
        item = self.FindObject(g.testWin.item, evt.GetEventObject())
        if item:
            tree.EnsureVisible(item)
            tree.SelectItem(item)
        self.tb.ToggleTool(self.ID_TOOL_LOCATE, False)
        if item:
            self.SetStatusText('Selected %s' % tree.GetItemText(item))
        else:
            self.SetStatusText('Locate failed!')

    def SetHandler(self, w, h=None):
        if h:
            w.SetEventHandler(h)
            w.SetCursor(wx.CROSS_CURSOR)
        else:
            w.SetEventHandler(w)
            w.SetCursor(wx.NullCursor)
        for ch in w.GetChildren():
            self.SetHandler(ch, h)

    def OnLocate(self, evt):
        if g.testWin:
            if evt.GetId() == self.ID_LOCATE or \
               evt.GetId() == self.ID_TOOL_LOCATE and evt.IsChecked():
                self.SetHandler(g.testWin, g.testWin)
                g.testWin.Connect(wx.ID_ANY, wx.ID_ANY, wx.wxEVT_LEFT_DOWN, self.OnTestWinLeftDown)
                if evt.GetId() == self.ID_LOCATE:
                    self.tb.ToggleTool(self.ID_TOOL_LOCATE, True)
            elif evt.GetId() == self.ID_TOOL_LOCATE and not evt.IsChecked():
                self.SetHandler(g.testWin, None)
                g.testWin.Disconnect(wx.ID_ANY, wx.ID_ANY, wx.wxEVT_LEFT_DOWN)
            self.SetStatusText('Click somewhere in your test window now')

    def OnRefresh(self, evt):
        # If modified, apply first
        selection = tree.selection
        if selection:
            xxx = tree.GetPyData(selection)
            if xxx and panel.IsModified():
                tree.Apply(xxx, selection)
        if g.testWin:
            # (re)create
            tree.CreateTestWin(g.testWin.item)
        panel.modified = False
        tree.needUpdate = False

    def OnAutoRefresh(self, evt):
        conf.autoRefresh = evt.IsChecked()
        self.menuBar.Check(self.ID_AUTO_REFRESH, conf.autoRefresh)
        self.tb.ToggleTool(self.ID_AUTO_REFRESH, conf.autoRefresh)

    def OnAbout(self, evt):
        str = '''\
XRCed version %s

(c) Roman Rolinsky <rollrom@users.sourceforge.net>
Homepage: http://xrced.sourceforge.net\
''' % version
        dlg = wx.MessageDialog(self, str, 'About XRCed', wx.OK | wx.CENTRE)
        dlg.ShowModal()
        dlg.Destroy()

    def OnReadme(self, evt):
        text = open(os.path.join(basePath, 'README.txt'), 'r').read()
        dlg = ScrolledMessageDialog(self, text, "XRCed README")
        dlg.ShowModal()
        dlg.Destroy()

    # Simple emulation of python command line
    def OnDebugCMD(self, evt):
        while 1:
            try:
                exec raw_input('C:\> ')
            except EOFError:
                print '^D'
                break
            except:
                (etype, value, tb) =sys.exc_info()
                tblist =traceback.extract_tb(tb)[1:]
                msg =' '.join(traceback.format_exception_only(etype, value)
                        +traceback.format_list(tblist))
                print msg

    def OnCreate(self, evt):
        selected = tree.selection
        if tree.ctrl: appendChild = False
        else: appendChild = not tree.NeedInsert(selected)
        xxx = tree.GetPyData(selected)
        if not appendChild:
            # If insert before
            if tree.shift:
                # If has previous item, insert after it, else append to parent
                nextItem = selected
                parentLeaf = tree.GetItemParent(selected)
            else:
                # If has next item, insert, else append to parent
                nextItem = tree.GetNextSibling(selected)
                parentLeaf = tree.GetItemParent(selected)
        # Expanded container (must have children)
        elif tree.shift and tree.IsExpanded(selected) \
           and tree.GetChildrenCount(selected, False):
            nextItem = tree.GetFirstChild(selected)[0]
            parentLeaf = selected
        else:
            nextItem = wx.TreeItemId()
            parentLeaf = selected
        parent = tree.GetPyData(parentLeaf)
        if parent.hasChild: parent = parent.child

        # Create object_ref?
        if evt.GetId() == ID_NEW.REF:
            ref = wx.GetTextFromUser('Create reference to:', 'Create reference')
            if not ref: return
            xxx = MakeEmptyRefXXX(parent, ref)
        elif evt.GetId() == ID_NEW.COMMENT:
            xxx = MakeEmptyCommentXXX(parent)
        else:
            # Create empty element
            if evt.GetId() >= ID_NEW.CUSTOM:
                className = pullDownMenu.customMap[evt.GetId()]
            else:
                className = pullDownMenu.createMap[evt.GetId()]
            xxx = MakeEmptyXXX(parent, className)

        # Insert new node, register undo
        if xxx.isElement:                 # true object
            # Set default name for top-level windows
            if parent.__class__ == xxxMainNode:
                cl = xxx.treeObject().__class__
                frame.maxIDs[cl] += 1
                xxx.setTreeName('%s%d' % (defaultIDs[cl], frame.maxIDs[cl]))
            # And for some other standard controls
            elif parent.__class__ == xxxStdDialogButtonSizer:
                xxx.setTreeName(pullDownMenu.stdButtonIDs[evt.GetId()][0])
                # We can even set label
                obj = xxx.treeObject()
                elem = g.tree.dom.createElement('label')
                elem.appendChild(g.tree.dom.createTextNode(pullDownMenu.stdButtonIDs[evt.GetId()][1]))
                obj.params['label'] = xxxParam(elem)
                xxx.treeObject().node.appendChild(elem)

            newItem = tree.InsertNode(parentLeaf, parent, xxx.node, nextItem)
        else:                           # comment node
            newItem = tree.InsertNode(parentLeaf, parent, xxx.node, nextItem)
        undoMan.RegisterUndo(UndoPasteCreate(parentLeaf, parent, newItem, selected))
        tree.EnsureVisible(newItem)
        tree.SelectItem(newItem)
        if not tree.IsVisible(newItem):
            tree.ScrollTo(newItem)
            tree.Refresh()
        # Update view?
        if xxx.isElement and g.testWin and tree.IsHighlatable(newItem):
            if conf.autoRefresh:
                tree.needUpdate = True
                tree.pendingHighLight = newItem
            else:
                tree.pendingHighLight = None
        tree.SetFocus()
        if not xxx.isElement:
            tree.EditLabel(newItem)
        self.SetModified()

    # Replace one object with another
    def OnReplace(self, evt):
        selected = tree.selection
        xxx = tree.GetPyData(selected).treeObject()
        elem = xxx.node
        parent = elem.parentNode
        undoMan.RegisterUndo(UndoReplace(selected))
        # New class
        className = pullDownMenu.createMap[evt.GetId() - 1000]
        
        # Create temporary empty node (with default values)
        dummy = MakeEmptyDOM(className)
        if className == 'spacer' and xxx.className != 'spacer':
            klass = xxxSpacer
        elif xxx.className == 'spacer' and className != 'spacer':
            klass = xxxSizerItem
        else:
            klass = xxxDict[className]
        # Remove non-compatible children
        if tree.ItemHasChildren(selected) and not klass.hasChildren:
            tree.DeleteChildren(selected)
        nodes = elem.childNodes[:]
        tags = []
        for node in nodes:
            if node.nodeType != minidom.Node.ELEMENT_NODE: continue
            remove = False
            tag = node.tagName
            if tag == 'object':
                if not klass.hasChildren:  remove = True
            elif tag not in klass.allParams and \
                     (not klass.hasStyle or tag not in klass.styles):
                remove = True
            else:
                tags.append(tag)
            if remove:
                elem.removeChild(node)
                node.unlink()
        
        # Remove sizeritem child if spacer
        if className == 'spacer' and xxx.className != 'spacer':
            sizeritem = elem.parentNode
            assert sizeritem.getAttribute('class') == 'sizeritem'
            sizeritem.removeChild(elem)
            elem.unlink()
            elem = sizeritem
            tree.GetPyData(selected).hasChild = False
        elif xxx.className == 'spacer' and className != 'spacer':
            # Create sizeritem element
            assert xxx.parent.isSizer
            elem.setAttribute('class', 'sizeritem')
            node = MakeEmptyDOM(className)
            elem.appendChild(node)
            # Replace to point to new object
            xxx = xxxSizerItem(xxx.parent, elem)
            elem = node
            tree.SetPyData(selected, xxx)
            xxx = xxx.child
        else:
            # Copy parameters present in dummy but not in elem
            for node in dummy.childNodes:
                if node.tagName not in tags:  elem.appendChild(node.cloneNode(True))
        dummy.unlink()
        
        # Change class name
        elem.setAttribute('class', className)
        if elem.hasAttribute('subclass'):
            elem.removeAttribute('subclass') # clear subclassing
        # Re-create xxx element
        xxx = MakeXXXFromDOM(xxx.parent, elem)
        # Remove incompatible style flags
        if 'style' in xxx.params:
            styles = map(string.strip, xxx.params['style'].value().split('|'))
            newStyles = [s for s in styles if s in klass.winStyles or s in genericStyles]
            if newStyles != styles:
                if newStyles:
                    value = reduce(lambda a,b: a+'|'+b, newStyles)
                else:
                    value = ''
                xxx.params['style'].update(value)

        # Update parent in child objects
        if tree.ItemHasChildren(selected):
            i, cookie = tree.GetFirstChild(selected)
            while i.IsOk():
                x = tree.GetPyData(i)
                x.parent = xxx
                if x.hasChild: x.child.parent = xxx
                i, cookie = tree.GetNextChild(selected, cookie)

        # Update tree
        if tree.GetPyData(selected).hasChild: # child container
            container = tree.GetPyData(selected)
            container.resetChild(xxx)
            xxx = container
        else:
            tree.SetPyData(selected, xxx)
        tree.SetItemText(selected, xxx.treeName())
        tree.SetItemImage(selected, xxx.treeImage())

        # Set default name for top-level windows
        if parent.__class__ == xxxMainNode:
            cl = xxx.treeObject().__class__
            frame.maxIDs[cl] += 1
            xxx.setTreeName('%s%d' % (defaultIDs[cl], frame.maxIDs[cl]))

        # Update panel
        g.panel.SetData(xxx)
        # Update tools
        g.tools.UpdateUI()

        #undoMan.RegisterUndo(UndoPasteCreate(parentLeaf, parent, newItem, selected))
        # Update view?
        if g.testWin and tree.IsHighlatable(selected):
            if conf.autoRefresh:
                tree.needUpdate = True
                tree.pendingHighLight = selected
            else:
                tree.pendingHighLight = None
        tree.SetFocus()
        self.SetModified()

    # Expand/collapse subtree
    def OnExpand(self, evt):
        if tree.selection: tree.ExpandAll(tree.selection)
        else: tree.ExpandAll(tree.root)
    def OnCollapse(self, evt):
        if tree.selection: tree.CollapseAll(tree.selection)
        else: tree.CollapseAll(tree.root)

    def OnPullDownHighlight(self, evt):
        menuId = evt.GetMenuId()
        if menuId != -1:
            menu = evt.GetEventObject()
            help = menu.GetHelpString(menuId)
            self.SetStatusText(help)
        else:
            self.SetStatusText('')

    def OnUpdateUI(self, evt):
        if evt.GetId() in [wx.ID_CUT, wx.ID_COPY, self.ID_DELETE]:
            evt.Enable(tree.selection is not None and tree.selection != tree.root)
        elif evt.GetId() == wx.ID_SAVE:
            evt.Enable(self.modified)
        elif evt.GetId() in [wx.ID_PASTE, self.ID_TOOL_PASTE]:
            evt.Enable(tree.selection is not None)
        elif evt.GetId() == self.ID_TEST:
            evt.Enable(tree.selection is not None and tree.selection != tree.root)
        elif evt.GetId() in [self.ID_LOCATE, self.ID_TOOL_LOCATE]:
            evt.Enable(g.testWin is not None)
        elif evt.GetId() == wx.ID_UNDO:  evt.Enable(undoMan.CanUndo())
        elif evt.GetId() == wx.ID_REDO:  evt.Enable(undoMan.CanRedo())

    def OnIdle(self, evt):
        if self.inIdle: return          # Recursive call protection
        self.inIdle = True
        try:
            if tree.needUpdate:
                if conf.autoRefresh:
                    if g.testWin:
                        self.SetStatusText('Refreshing test window...')
                        # (re)create
                        tree.CreateTestWin(g.testWin.item)
                        self.SetStatusText('')
                    tree.needUpdate = False
            elif tree.pendingHighLight:
                try:
                    tree.HighLight(tree.pendingHighLight)
                except:
                    # Remove highlight if any problem
                    if g.testWin.highLight:
                        g.testWin.highLight.Remove()
                    tree.pendingHighLight = None
                    raise
            else:
                evt.Skip()
        finally:
            self.inIdle = False

    # We don't let close panel window
    def OnCloseMiniFrame(self, evt):
        return

    def OnIconize(self, evt):
        if evt.Iconized():
            conf.x, conf.y = self.GetPosition()
            conf.width, conf.height = self.GetSize()
            if conf.embedPanel:
                conf.sashPos = self.splitter.GetSashPosition()
            else:
                conf.panelX, conf.panelY = self.miniFrame.GetPosition()
                conf.panelWidth, conf.panelHeight = self.miniFrame.GetSize()
                self.miniFrame.Iconize()
        else:
            if not conf.embedPanel:
                self.miniFrame.Iconize(False)
        evt.Skip()

    def OnCloseWindow(self, evt):
        if not self.AskSave(): return
        if g.testWin: g.testWin.Destroy()
        if not panel.GetPageCount() == 2:
            panel.page2.Destroy()
        else:
            # If we don't do this, page does not get destroyed (a bug?)
            panel.RemovePage(1)
        if not self.IsIconized():
            conf.x, conf.y = self.GetPosition()
            conf.width, conf.height = self.GetSize()
            if conf.embedPanel:
                conf.sashPos = self.splitter.GetSashPosition()
            else:
                conf.panelX, conf.panelY = self.miniFrame.GetPosition()
                conf.panelWidth, conf.panelHeight = self.miniFrame.GetSize()
        evt.Skip()


    def CreateLocalConf(self, path):
        name = os.path.splitext(path)[0]
        name += '.xcfg'
        return wx.FileConfig(localFilename=name)


    def Clear(self):
        self.dataFile = ''
        conf.localconf = None
        undoMan.Clear()
        self.SetModified(False)
        tree.Clear()
        panel.Clear()
        if g.testWin:
            g.testWin.Destroy()
            g.testWin = None
        # Numbers for new controls
        self.maxIDs = {}
        for cl in [xxxPanel, xxxDialog, xxxFrame,
                   xxxMenuBar, xxxMenu, xxxToolBar,
                   xxxWizard, xxxBitmap, xxxIcon]:
            self.maxIDs[cl] = 0
        # Handlers
        clearHandlers()
        g.pullDownMenu.clearCustom()
        # Delete modules imported from comment directives
        map(sys.modules.pop, [m for m in sys.modules if m not in self.modules])

    def SetModified(self, state=True):
        self.modified = state
        name = os.path.basename(self.dataFile)
        if not name: name = defaultName
        if state:
            self.SetTitle(progname + ': ' + name + ' *')
        else:
            self.SetTitle(progname + ': ' + name)

    def Open(self, path):
        if not os.path.exists(path):
            wx.LogError('File does not exists: %s' % path)
            return False
        # Try to read the file
        try:
            f = open(path)
            self.Clear()
            dom = minidom.parse(f)
            f.close()
            # Set encoding global variable and default encoding
            if dom.encoding:
                g.currentEncoding = dom.encoding
                wx.SetDefaultPyEncoding(g.currentEncoding.encode())
            else:
                g.currentEncoding = ''
            # Change dir
            self.dataFile = path = os.path.abspath(path)
            dir = os.path.dirname(path)
            if dir: os.chdir(dir)
            # Allow importing modules from the same directory
            sys.path = sys_path + [dir]
            tree.SetData(dom)
            self.SetTitle(progname + ': ' + os.path.basename(path))
            conf.localconf = self.CreateLocalConf(self.dataFile)
        except:
            # Nice exception printing
            inf = sys.exc_info()
            wx.LogError(traceback.format_exception(inf[0], inf[1], None)[-1])
            wx.LogError('Error reading file: %s' % path)
            if debug: raise
            return False
        return True

    def Indent(self, node, indent = 0):
        if node.nodeType == minidom.Node.COMMENT_NODE:
            text = self.domCopy.createTextNode('\n' + ' ' * indent)
            node.parentNode.insertBefore(text, node)
            return                      # no children
        # Copy child list because it will change soon
        children = node.childNodes[:]
        # Main node doesn't need to be indented
        if indent:
            text = self.domCopy.createTextNode('\n' + ' ' * indent)
            node.parentNode.insertBefore(text, node)
        if children:
            # Append newline after last child, except for text nodes
            if children[-1].nodeType == minidom.Node.ELEMENT_NODE:
                text = self.domCopy.createTextNode('\n' + ' ' * indent)
                node.appendChild(text)
            # Indent children which are elements
            for n in children:
                if n.nodeType == minidom.Node.ELEMENT_NODE or \
                       n.nodeType == minidom.Node.COMMENT_NODE:
                    self.Indent(n, indent + 2)

    def Save(self, path):
        try:
            import codecs
            # Apply changes
            if tree.selection and panel.IsModified():
                self.OnRefresh(wx.CommandEvent())
            if g.currentEncoding:
                f = codecs.open(path, 'wt', g.currentEncoding)
            else:
                f = codecs.open(path, 'wt')
            # Make temporary copy for formatting it
            # !!! We can't clone dom node, it works only once
            #self.domCopy = tree.dom.cloneNode(True)
            self.domCopy = MyDocument()
            mainNode = self.domCopy.appendChild(tree.mainNode.cloneNode(True))
            # Remove first child (test element)
            testElem = mainNode.firstChild
            mainNode.removeChild(testElem)
            testElem.unlink()
            self.Indent(mainNode)
            self.domCopy.writexml(f, encoding = g.currentEncoding)
            f.close()
            self.domCopy.unlink()
            self.domCopy = None
            self.SetModified(False)
            panel.SetModified(False)
            conf.localconf.Flush()
        except:
            inf = sys.exc_info()
            wx.LogError(traceback.format_exception(inf[0], inf[1], None)[-1])
            wx.LogError('Error writing file: %s' % path)
            raise
            
    def AskSave(self):
        if not (self.modified or panel.IsModified()): return True
        flags = wx.ICON_EXCLAMATION | wx.YES_NO | wx.CANCEL | wx.CENTRE
        dlg = wx.MessageDialog( self, 'File is modified. Save before exit?',
                               'Save before too late?', flags )
        say = dlg.ShowModal()
        dlg.Destroy()
        wx.Yield()
        if say == wx.ID_YES:
            self.OnSaveOrSaveAs(wx.CommandEvent(wx.ID_SAVE))
            # If save was successful, modified flag is unset
            if not self.modified: return True
        elif say == wx.ID_NO:
            self.SetModified(False)
            panel.SetModified(False)
            return True
        return False

    def SaveUndo(self):
        pass                            # !!!

################################################################################

class PythonOptions(wx.Dialog):

    def __init__(self, parent, cfg, dataFile):
        pre = wx.PreDialog()
        g.frame.res.LoadOnDialog(pre, parent, "PYTHON_OPTIONS")
        self.PostCreate(pre)

        self.cfg = cfg
        self.dataFile = dataFile

        self.AutoGenerateCB = xrc.XRCCTRL(self, "AutoGenerateCB")
        self.EmbedCB = xrc.XRCCTRL(self, "EmbedCB")
        self.GettextCB = xrc.XRCCTRL(self, "GettextCB")
        self.MakeXRSFileCB = xrc.XRCCTRL(self, "MakeXRSFileCB")
        self.FileNameTC = xrc.XRCCTRL(self, "FileNameTC")
        self.BrowseBtn = xrc.XRCCTRL(self, "BrowseBtn")
        self.GenerateBtn = xrc.XRCCTRL(self, "GenerateBtn")
        self.SaveOptsBtn = xrc.XRCCTRL(self, "SaveOptsBtn")

        self.Bind(wx.EVT_BUTTON, self.OnBrowse, self.BrowseBtn)
        self.Bind(wx.EVT_BUTTON, self.OnGenerate, self.GenerateBtn)
        self.Bind(wx.EVT_BUTTON, self.OnSaveOpts, self.SaveOptsBtn)

        if self.cfg.Read("filename", "") != "":
            self.FileNameTC.SetValue(self.cfg.Read("filename"))
        else:
            name = os.path.splitext(os.path.split(dataFile)[1])[0]
            name += '_xrc.py'
            self.FileNameTC.SetValue(name)
        self.AutoGenerateCB.SetValue(self.cfg.ReadBool("autogenerate", False))
        self.EmbedCB.SetValue(self.cfg.ReadBool("embedResource", False))
        self.MakeXRSFileCB.SetValue(self.cfg.ReadBool("makeXRS", False))
        self.GettextCB.SetValue(self.cfg.ReadBool("genGettext", False))
        
                  
    def OnBrowse(self, evt):
        path = self.FileNameTC.GetValue()
        dirname = os.path.abspath(os.path.dirname(path))
        name = os.path.split(path)[1]
        dlg = wx.FileDialog(self, 'Save As', dirname, name, '*.py',
                               wx.SAVE | wx.OVERWRITE_PROMPT)
        if dlg.ShowModal() == wx.ID_OK:
            path = dlg.GetPath()
            self.FileNameTC.SetValue(path)
        dlg.Destroy()
    

    def OnGenerate(self, evt):
        pypath = self.FileNameTC.GetValue()
        embed = self.EmbedCB.GetValue()
        genGettext = self.GettextCB.GetValue()
        frame.GeneratePython(self.dataFile, pypath, embed, genGettext)
        self.OnSaveOpts()

    
    def OnSaveOpts(self, evt=None):
        self.cfg.Write("filename", self.FileNameTC.GetValue())
        self.cfg.WriteBool("autogenerate", self.AutoGenerateCB.GetValue())
        self.cfg.WriteBool("embedResource", self.EmbedCB.GetValue())
        self.cfg.WriteBool("makeXRS", self.MakeXRSFileCB.GetValue())
        self.cfg.WriteBool("genGettext", self.GettextCB.GetValue())

        self.EndModal(wx.ID_OK)
    
        
################################################################################

def usage():
    print >> sys.stderr, 'usage: xrced [-dhiv] [file]'

class App(wx.App):
    def OnInit(self):
        # Check version
        if wx.VERSION[:3] < MinWxVersion:
            wx.LogWarning('''\
This version of XRCed may not work correctly on your version of wxWidgets. \
Please upgrade wxWidgets to %d.%d.%d or higher.''' % MinWxVersion)
        global debug
        # Process comand-line
        opts = args = None
        try:
            opts, args = getopt.getopt(sys.argv[1:], 'dhiv')
            for o,a in opts:
                if o == '-h':
                    usage()
                    sys.exit(0)
                elif o == '-d':
                    debug = True
                elif o == '-v':
                    print 'XRCed version', version
                    sys.exit(0)
            
        except getopt.GetoptError:
            if wx.Platform != '__WXMAC__': # macs have some extra parameters
                print >> sys.stderr, 'Unknown option'
                usage()
                sys.exit(1)

        self.SetAppName('xrced')
        # Settings
        global conf
        conf = g.conf = wx.Config(style = wx.CONFIG_USE_LOCAL_FILE)
        conf.localconf = None
        conf.autoRefresh = conf.ReadInt('autorefresh', True)
        pos = conf.ReadInt('x', -1), conf.ReadInt('y', -1)
        size = conf.ReadInt('width', 800), conf.ReadInt('height', 600)
        conf.embedPanel = conf.ReadInt('embedPanel', True)
        conf.showTools = conf.ReadInt('showTools', True)
        conf.sashPos = conf.ReadInt('sashPos', 200)
        # read recently used files
        recentfiles=conf.Read('recentFiles','')
        conf.recentfiles={}
        if recentfiles:
            for fil in recentfiles.split('|'):
                conf.recentfiles[wx.NewId()]=fil
        if not conf.embedPanel:
            conf.panelX = conf.ReadInt('panelX', -1)
            conf.panelY = conf.ReadInt('panelY', -1)
        else:
            conf.panelX = conf.panelY = -1
        conf.panelWidth = conf.ReadInt('panelWidth', 200)
        conf.panelHeight = conf.ReadInt('panelHeight', 200)
        conf.panic = not conf.HasEntry('nopanic')
        # Add handlers
        wx.FileSystem.AddHandler(wx.MemoryFSHandler())
        # Create main frame
        frame = Frame(pos, size)
        frame.Show(True)

        # Load file after showing
        if args:
            conf.panic = False
            frame.open = frame.Open(args[0])

        return True

    def OnExit(self):
        # Write config
        global conf
        wc = conf
        wc.WriteInt('autorefresh', conf.autoRefresh)
        wc.WriteInt('x', conf.x)
        wc.WriteInt('y', conf.y)
        wc.WriteInt('width', conf.width)
        wc.WriteInt('height', conf.height)
        wc.WriteInt('embedPanel', conf.embedPanel)
        wc.WriteInt('showTools', conf.showTools)
        if not conf.embedPanel:
            wc.WriteInt('panelX', conf.panelX)
            wc.WriteInt('panelY', conf.panelY)
        wc.WriteInt('sashPos', conf.sashPos)
        wc.WriteInt('panelWidth', conf.panelWidth)
        wc.WriteInt('panelHeight', conf.panelHeight)
        wc.WriteInt('nopanic', True)
        wc.Write('recentFiles', '|'.join(conf.recentfiles.values()[-5:]))
        wc.Flush()

def main():
    app = App(0, useBestVisual=False)
    #app.SetAssertMode(wx.PYAPP_ASSERT_LOG)
    app.MainLoop()
    app.OnExit()
    global conf
    del conf

if __name__ == '__main__':
    main()
