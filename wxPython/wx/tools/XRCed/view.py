# Name:         view.py
# Purpose:      View classes
# Author:       Roman Rolinsky <rolinsky@femagsoft.com>
# Created:      07.06.2007
# RCS-ID:       $Id$

import os,sys,shutil,tempfile,traceback
from globals import *
from XMLTree import XMLTree
from AttributePanel import Panel
from component import Manager
from presenter import Presenter
import images

class TaskBarIcon(wx.TaskBarIcon):
    def __init__(self, frame):
        wx.TaskBarIcon.__init__(self)
        self.frame = frame
        # Set the image
        self.SetIcon(images.getIconIcon(), "XRCed")


class Frame(wx.Frame):
    def __init__(self):
        wx.Frame.__init__(self, None, -1, '', size=(600,400))
        global frame
        frame = self
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
        self.Bind(wx.EVT_MENU, self.OnRecentFile, id=wx.ID_FILE1, id2=wx.ID_FILE9)
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
        menu.Check(self.ID_EMBED_PANEL, conf.embedPanel)
        self.ID_SHOW_TOOLS = wx.NewId()
        menu.Append(self.ID_SHOW_TOOLS, 'Show &Tools', 'Toggle tools', True)
        menu.Check(self.ID_SHOW_TOOLS, conf.showTools)
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
        menu.Check(self.ID_AUTO_REFRESH, conf.autoRefresh)
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
#         if debug:
#             self.ID_DEBUG_CMD = wx.NewId()
#             menu.Append(self.ID_DEBUG_CMD, 'CMD', 'Python command line')
#             wx.EVT_MENU(self, self.ID_DEBUG_CMD, self.OnDebugCMD)
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
                                 (conf.panelX, conf.panelY),
                                 (conf.panelWidth, conf.panelHeight))
        self.miniFrame = miniFrame
        sizer2 = wx.BoxSizer()
        miniFrame.SetSizer(sizer2)
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
        if wx.Platform == '__WXMAC__':
            sizer1.Add(splitter, 1, wx.EXPAND|wx.RIGHT, 5)
        else:
            sizer1.Add(splitter, 1, wx.EXPAND)
        sizer.Add(sizer1, 1, wx.EXPAND)
        self.SetAutoLayout(True)
        self.SetSizer(sizer)

        # Component events
        wx.EVT_MENU_RANGE(self, Manager.firstId, Manager.lastId,
                          self.OnComponent)

        # Menubar events
        # File
        wx.EVT_MENU(self, wx.ID_NEW, self.OnNew)
        wx.EVT_MENU(self, wx.ID_OPEN, self.OnOpen)
        wx.EVT_MENU(self, wx.ID_SAVE, self.OnSaveOrSaveAs)
        wx.EVT_MENU(self, wx.ID_SAVEAS, self.OnSaveOrSaveAs)
#        wx.EVT_MENU(self, self.ID_GENERATE_PYTHON, self.OnGeneratePython)
#        wx.EVT_MENU(self, self.ID_PREFS, self.OnPrefs)
        wx.EVT_MENU(self, wx.ID_EXIT, self.OnExit)

        # Other events
#        wx.EVT_IDLE(self, self.OnIdle)
        wx.EVT_CLOSE(self, self.OnCloseWindow)
#        wx.EVT_KEY_DOWN(self, tools.OnKeyDown)
#        wx.EVT_KEY_UP(self, tools.OnKeyUp)
#        wx.EVT_ICONIZE(self, self.OnIconize)

        wx.EVT_MENU(self, wx.ID_DELETE, self.OnCutDelete)

        return

        # Edit
        wx.EVT_MENU(self, wx.ID_UNDO, self.OnUndo)
        wx.EVT_MENU(self, wx.ID_REDO, self.OnRedo)
        wx.EVT_MENU(self, wx.ID_CUT, self.OnCutDelete)
        wx.EVT_MENU(self, wx.ID_COPY, self.OnCopy)
        wx.EVT_MENU(self, wx.ID_PASTE, self.OnPaste)
        wx.EVT_MENU(self, self.ID_TOOL_PASTE, self.OnPaste)
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
        wx.EVT_UPDATE_UI(self, wx.ID_DELETE, self.OnUpdateUI)
        wx.EVT_UPDATE_UI(self, self.ID_TEST, self.OnUpdateUI)
        wx.EVT_UPDATE_UI(self, self.ID_MOVEUP, self.OnUpdateUI)
        wx.EVT_UPDATE_UI(self, self.ID_MOVEDOWN, self.OnUpdateUI)
        wx.EVT_UPDATE_UI(self, self.ID_MOVELEFT, self.OnUpdateUI)
        wx.EVT_UPDATE_UI(self, self.ID_MOVERIGHT, self.OnUpdateUI)
        wx.EVT_UPDATE_UI(self, self.ID_REFRESH, self.OnUpdateUI)

    def OnComponent(self, evt):
        '''Hadnler for creating new elements.'''
        comp = Manager.findById(evt.GetId())
        print comp
        Presenter.create(comp)

    def OnCutDelete(self, evt):
        Presenter.delete()

    def OnNew(self, evt):
        if not self.AskSave(): return
        self.Clear()

    def OnOpen(self, evt):
        if not self.AskSave(): return
        dlg = wx.FileDialog(self, 'Open', os.path.dirname(Presenter.getPath()),
                           '', '*.xrc', wx.OPEN | wx.CHANGE_DIR)
        if dlg.ShowModal() == wx.ID_OK:
            path = dlg.GetPath()
            self.SetStatusText('Loading...')
            wx.BeginBusyCursor()
            try:
                if self.Open(path):
                    self.SetStatusText('Data loaded')
                    self.SaveRecent(path)
                else:
                    self.SetStatusText('Failed')
            finally:
                wx.EndBusyCursor()
        dlg.Destroy()

    def OnRecentFile(self,evt):
        # open recently used file
        if not self.AskSave(): return
        wx.BeginBusyCursor()

        # get the pathname based on the menu ID
        fileNum = evt.GetId() - wx.ID_FILE1
        path = g.fileHistory.GetHistoryFile(fileNum)
            
        if self.Open(path):
            self.SetStatusText('Data loaded')
            # add it back to the history so it will be moved up the list
            self.SaveRecent(path)
        else:
            self.SetStatusText('Failed')

        wx.EndBusyCursor()

    def OnSaveOrSaveAs(self, evt):
        path = Presenter.getPath()
        if evt.GetId() == wx.ID_SAVEAS or not path:
            dirname = os.path.abspath(os.path.dirname(path))
            dlg = wx.FileDialog(self, 'Save As', dirname, '', '*.xrc',
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
        self.SetStatusText('Saving...')
        wx.BeginBusyCursor()
        try:
            try:
                tmpFile,tmpName = tempfile.mkstemp(prefix='xrced-')
                os.close(tmpFile)
                self.Save(tmpName) # save temporary file first
                shutil.move(tmpName, path)
                self.dataFile = path
                Presenter.setModified(False)
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

    def OnCloseWindow(self, evt):
        if not self.AskSave(): return
        if g.testWin: g.testWin.Destroy()
#        if not panel.GetPageCount() == 2:
#            panel.page2.Destroy()
#        else:
            # If we don't do this, page does not get destroyed (a bug?)
#            panel.RemovePage(1)
        if not self.IsIconized():
            conf.x, conf.y = self.GetPosition()
            if wx.Platform == '__WXMAC__':
                conf.width, conf.height = self.GetClientSize()
            else:
                conf.width, conf.height = self.GetSize()
#            if conf.embedPanel:
#                conf.sashPos = self.splitter.GetSashPosition()
#            else:
#                conf.panelX, conf.panelY = self.miniFrame.GetPosition()
#                conf.panelWidth, conf.panelHeight = self.miniFrame.GetSize()
        evt.Skip()

    def SaveRecent(self,path):
        # append to recently used files
        g.fileHistory.AddFileToHistory(path)

    def AskSave(self):
        if not Presenter.modified: return True
        flags = wx.ICON_EXCLAMATION | wx.YES_NO | wx.CANCEL | wx.CENTRE
        dlg = wx.MessageDialog( self, 'File is modified. Save before exit?',
                               'Save before too late?', flags )
        say = dlg.ShowModal()
        dlg.Destroy()
        wx.Yield()
        if say == wx.ID_YES:
            self.OnSaveOrSaveAs(wx.CommandEvent(wx.ID_SAVE))
            # If save was successful, modified flag is unset
            if not Presenter.modified: return True
        elif say == wx.ID_NO:
            Presenter.setModified(False)
            return True
        return False

    def CreateLocalConf(self, path):
        name = os.path.splitext(path)[0]
        name += '.xcfg'
        return wx.FileConfig(localFilename=name)

    def Clear(self):
        print 'NYI:', self

    def Open(self, path):
        if not os.path.exists(path):
            wx.LogError('File does not exists: %s' % path)
            return False
        try:
            Presenter.loadXML(path)
        except:
            inf = sys.exc_info()
            wx.LogError(traceback.format_exception(inf[0], inf[1], None)[-1])
            wx.LogError('Error reading file: %s' % path)
            if debug: raise
            return False
        return True
            
    def Save(self, path):
        try:
            Presenter.saveXML(path)
        except:
            inf = sys.exc_info()
            wx.LogError(traceback.format_exception(inf[0], inf[1], None)[-1])
            wx.LogError('Error writing file: %s' % path)
            if debug: raise
            raise
        
    def OnExit(self, evt):
        self.Close()

