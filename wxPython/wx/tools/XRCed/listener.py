# Name:         listener.py
# Purpose:      Listener for dispatching events from view to presenter
# Author:       Roman Rolinsky <rolinsky@femagsoft.com>
# Created:      07.06.2007
# RCS-ID:       $Id$

import wx
import os,sys,shutil,tempfile
from globals import *
from presenter import Presenter
from component import Manager

class _Listener:
    '''
    Installs event handlers to view objects and delegates some events
    to Presenter.
    '''
    def Install(self, frame, tree, panel):
        '''Set event handlers.'''
        self.frame = frame
        self.tree = tree
        self.panel = panel

        # Some local members
        self.inUpdateUI = self.inIdle = False
        
        # Component events
        wx.EVT_MENU_RANGE(frame, Manager.firstId, Manager.lastId,
                          self.OnComponent)

        # Other events
        wx.EVT_IDLE(frame, self.OnIdle)
        wx.EVT_CLOSE(frame, self.OnCloseWindow)
#        wx.EVT_KEY_DOWN(frame, tools.OnKeyDown)
#        wx.EVT_KEY_UP(frame, tools.OnKeyUp)
#        wx.EVT_ICONIZE(frame, self.OnIconize)

        # Menubar events
        # File
        frame.Bind(wx.EVT_MENU, self.OnRecentFile, id=wx.ID_FILE1, id2=wx.ID_FILE9)
        wx.EVT_MENU(frame, wx.ID_NEW, self.OnNew)
        wx.EVT_MENU(frame, wx.ID_OPEN, self.OnOpen)
        wx.EVT_MENU(frame, wx.ID_SAVE, self.OnSaveOrSaveAs)
        wx.EVT_MENU(frame, wx.ID_SAVEAS, self.OnSaveOrSaveAs)
#        wx.EVT_MENU(frame, self.ID_GENERATE_PYTHON, self.OnGeneratePython)
#        wx.EVT_MENU(frame, self.ID_PREFS, self.OnPrefs)
        wx.EVT_MENU(frame, wx.ID_EXIT, self.OnExit)

        # Edit
        wx.EVT_MENU(frame, wx.ID_UNDO, self.OnUndo)
        wx.EVT_MENU(frame, wx.ID_REDO, self.OnRedo)
        wx.EVT_MENU(frame, wx.ID_CUT, self.OnCut)
        wx.EVT_MENU(frame, wx.ID_COPY, self.OnCopy)
        wx.EVT_MENU(frame, wx.ID_PASTE, self.OnPaste)
        wx.EVT_MENU(frame, ID.PASTE_SIBLING, self.OnPasteSibling)
        wx.EVT_MENU(frame, wx.ID_DELETE, self.OnDelete)
        wx.EVT_MENU(frame, frame.ID_TOOL_PASTE, self.OnPaste)
        wx.EVT_MENU(frame, frame.ID_LOCATE, self.OnLocate)
        wx.EVT_MENU(frame, frame.ID_TOOL_LOCATE, self.OnLocate)
        # View
        wx.EVT_MENU(frame, frame.ID_EMBED_PANEL, self.OnEmbedPanel)
        wx.EVT_MENU(frame, frame.ID_SHOW_TOOLS, self.OnShowTools)
        wx.EVT_MENU(frame, frame.ID_TEST, self.OnTest)
        wx.EVT_MENU(frame, frame.ID_REFRESH, self.OnRefresh)
        wx.EVT_MENU(frame, frame.ID_AUTO_REFRESH, self.OnAutoRefresh)
        wx.EVT_MENU(frame, frame.ID_TEST_HIDE, self.OnTestHide)
        # Move
        wx.EVT_MENU(frame, frame.ID_MOVEUP, self.OnMoveUp)
        wx.EVT_MENU(frame, frame.ID_MOVEDOWN, self.OnMoveDown)
        wx.EVT_MENU(frame, frame.ID_MOVELEFT, self.OnMoveLeft)
        wx.EVT_MENU(frame, frame.ID_MOVERIGHT, self.OnMoveRight)        
        # Help
        wx.EVT_MENU(frame, wx.ID_ABOUT, self.OnAbout)
        wx.EVT_MENU(frame, frame.ID_README, self.OnReadme)
        if debug:
            wx.EVT_MENU(frame, frame.ID_DEBUG_CMD, self.OnDebugCMD)

        # Update events
        wx.EVT_UPDATE_UI(frame, wx.ID_SAVE, self.OnUpdateUI)
        wx.EVT_UPDATE_UI(frame, wx.ID_CUT, self.OnUpdateUI)
        wx.EVT_UPDATE_UI(frame, wx.ID_COPY, self.OnUpdateUI)
        wx.EVT_UPDATE_UI(frame, wx.ID_PASTE, self.OnUpdateUI)
        wx.EVT_UPDATE_UI(frame, wx.ID_DELETE, self.OnUpdateUI)
        wx.EVT_UPDATE_UI(frame, frame.ID_LOCATE, self.OnUpdateUI)
        wx.EVT_UPDATE_UI(frame, frame.ID_TOOL_LOCATE, self.OnUpdateUI)
        wx.EVT_UPDATE_UI(frame, frame.ID_TOOL_PASTE, self.OnUpdateUI)
        wx.EVT_UPDATE_UI(frame, wx.ID_UNDO, self.OnUpdateUI)
        wx.EVT_UPDATE_UI(frame, wx.ID_REDO, self.OnUpdateUI)
        wx.EVT_UPDATE_UI(frame, frame.ID_TEST, self.OnUpdateUI)
        wx.EVT_UPDATE_UI(frame, frame.ID_MOVEUP, self.OnUpdateUI)
        wx.EVT_UPDATE_UI(frame, frame.ID_MOVEDOWN, self.OnUpdateUI)
        wx.EVT_UPDATE_UI(frame, frame.ID_MOVELEFT, self.OnUpdateUI)
        wx.EVT_UPDATE_UI(frame, frame.ID_MOVERIGHT, self.OnUpdateUI)
        wx.EVT_UPDATE_UI(frame, frame.ID_REFRESH, self.OnUpdateUI)

        wx.EVT_MENU(frame, ID.COLLAPSE, self.OnCollapse)
        wx.EVT_MENU(frame, ID.EXPAND, self.OnExpand)

        # XMLTree events
        # Register events
#        tree.Bind(wx.EVT_LEFT_DOWN, self.OnTreeLeftDown)
        tree.Bind(wx.EVT_RIGHT_DOWN, self.OnTreeRightDown)
        tree.Bind(wx.EVT_TREE_SEL_CHANGING, self.OnTreeSelChanging)
        tree.Bind(wx.EVT_TREE_SEL_CHANGED, self.OnTreeSelChanged)
        tree.Bind(wx.EVT_TREE_ITEM_COLLAPSED, self.OnTreeItemCollapsed)

    def OnComponent(self, evt):
        '''Hadnler for creating new elements.'''
        comp = Manager.findById(evt.GetId())
        Presenter.create(comp)

    def OnNew(self, evt):
        '''wx.ID_NEW hadndler.'''
        if not self.AskSave(): return
        Presenter.init()

    def OnOpen(self, evt):
        '''wx.ID_OPEN handler.'''
        if not self.AskSave(): return
        dlg = wx.FileDialog(self.frame, 'Open', os.path.dirname(Presenter.path),
                           '', '*.xrc', wx.OPEN | wx.CHANGE_DIR)
        if dlg.ShowModal() == wx.ID_OK:
            path = dlg.GetPath()
            self.frame.SetStatusText('Loading...')
            wx.BeginBusyCursor()
            try:
                if Presenter.open(path):
                    self.frame.SetStatusText('Data loaded')
                    self.SaveRecent(path)
                else:
                    self.frame.SetStatusText('Failed')
            finally:
                wx.EndBusyCursor()
        dlg.Destroy()

    def OnRecentFile(self, evt):
        '''wx.ID_FILE<n> handler.'''
        if not self.AskSave(): return
        wx.BeginBusyCursor()

        # get the pathname based on the menu ID
        fileNum = evt.GetId() - wx.ID_FILE1
        path = g.fileHistory.GetHistoryFile(fileNum)
            
        if Presenter.open(path):
            self.frame.SetStatusText('Data loaded')
            # add it back to the history so it will be moved up the list
            self.SaveRecent(path)
        else:
            self.frame.SetStatusText('Failed')

        wx.EndBusyCursor()

    def OnSaveOrSaveAs(self, evt):
        '''wx.ID_SAVE and wx.ID_SAVEAS handler'''
        path = Presenter.path
        if evt.GetId() == wx.ID_SAVEAS or not path:
            dirname = os.path.abspath(os.path.dirname(path))
            dlg = wx.FileDialog(self.frame, 'Save As', dirname, '', '*.xrc',
                               wx.SAVE | wx.OVERWRITE_PROMPT | wx.CHANGE_DIR)
            if dlg.ShowModal() == wx.ID_OK:
                path = dlg.GetPath()
                if isinstance(path, unicode):
                    path = path.encode(sys.getfilesystemencoding())
                dlg.Destroy()
            else:
                dlg.Destroy()
                return

            if g.conf.localconf:
                # if we already have a localconf then it needs to be
                # copied to a new config with the new name
                lc = g.conf.localconf
                nc = Presenter.createLocalConf(path)
                flag, key, idx = lc.GetFirstEntry()
                while flag:
                    nc.Write(key, lc.Read(key))
                    flag, key, idx = lc.GetNextEntry(idx)
                g.conf.localconf = nc
            else:
                # otherwise create a new one
                g.conf.localconf = Presenter.createLocalConf(path)
        self.frame.SetStatusText('Saving...')
        wx.BeginBusyCursor()
        try:
            try:
                Presenter.save(path) # save temporary file first
                if g.conf.localconf.ReadBool("autogenerate", False):
                    pypath = g.conf.localconf.Read("filename")
                    embed = g.conf.localconf.ReadBool("embedResource", False)
                    genGettext = g.conf.localconf.ReadBool("genGettext", False)
                    self.GeneratePython(path, pypath, embed, genGettext)
                    
                self.frame.SetStatusText('Data saved')
                self.SaveRecent(path)
            except IOError:
                self.frame.SetStatusText('Failed')
        finally:
            wx.EndBusyCursor()        

    def OnExit(self, evt):
        '''wx.ID_EXIT handler'''
        self.frame.Close()
        
    def SaveRecent(self, path):
        '''Append path to recently used files.'''
        g.fileHistory.AddFileToHistory(path)

    def AskSave(self):
        '''Show confirmation dialog.'''
        if not Presenter.modified: return True
        flags = wx.ICON_EXCLAMATION | wx.YES_NO | wx.CANCEL | wx.CENTRE
        dlg = wx.MessageDialog(self.frame, 'File is modified. Save before exit?',
                               'Save before too late?', flags)
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

    def OnCloseWindow(self, evt):
        '''wx.EVT_CLOSE handler'''
        if not self.AskSave(): return
        if g.testWin: g.testWin.Destroy()
        if not self.frame.IsIconized():
            g.conf.x, g.conf.y = self.frame.GetPosition()
            if wx.Platform == '__WXMAC__':
                g.conf.width, g.conf.height = self.frame.GetClientSize()
            else:
                g.conf.width, g.conf.height = self.frame.GetSize()
#            if conf.embedPanel:
#                conf.sashPos = self.splitter.GetSashPosition()
#            else:
#                conf.panelX, conf.panelY = self.miniframe.GetPosition()
#                conf.panelWidth, conf.panelHeight = self.miniframe.GetSize()
        evt.Skip()

    def OnUndo(self, evt):
        raise NotImplementedError # !!!

        # Extra check to not mess with idle updating
        if undoMan.CanUndo():
            undoMan.Undo()
            g.panel.SetModified(False)
            if not undoMan.CanUndo():
                self.SetModified(False)

    def OnRedo(self, evt):
        raise NotImplementedError # !!!

        if undoMan.CanRedo():
            undoMan.Redo()
            self.SetModified(True)

    def OnCut(self, evt):
        '''wx.ID_CUT handler.'''
        Presenter.cut()

    def OnDelete(self, evt):
        '''wx.ID_DELETE handler.'''
        Presenter.delete()

    def OnCopy(self, evt):
        '''wx.ID_COPY handler.'''
        Presenter.copy()

    def OnPaste(self, evt):
        '''wx.ID_PASTE handler.'''
        Presenter.paste()

    def OnPasteSibling(self, evt):
        '''ID.PASTE_SIBLING handler.'''
        Presenter.paste()

    def ItemsAreCompatible(self, parent, child):
        raise NotImplementedError

    def OnMoveUp(self, evt):
        raise NotImplementedError

    def OnMoveDown(self, evt):
        raise NotImplementedError
    
    def OnMoveLeft(self, evt):
        raise NotImplementedError

    def OnMoveRight(self, evt):
        raise NotImplementedError

    def OnLocate(self, evt):
        raise NotImplementedError

    def OnRefresh(self, evt):
        raise NotImplementedError

    def OnAutoRefresh(self, evt):
        conf.autoRefresh = evt.IsChecked()
        self.menuBar.Check(ID.AUTO_REFRESH, conf.autoRefresh)
        self.tb.ToggleTool(ID.AUTO_REFRESH, conf.autoRefresh)

    def OnAbout(self, evt):
        str = '''\
XRCed version %s

(c) Roman Rolinsky <rollrom@users.sourceforge.net>
Homepage: http://xrced.sourceforge.net\
''' % version
        dlg = wx.MessageDialog(self.frame, str, 'About XRCed', wx.OK | wx.CENTRE)
        dlg.ShowModal()
        dlg.Destroy()

    def OnReadme(self, evt):
        text = open(os.path.join(basePath, 'README.txt'), 'r').read()
        dlg = ScrolledMessageDialog(self.frame, text, "XRCed README")
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
                import traceback
                (etype, value, tb) =sys.exc_info()
                tblist =traceback.extract_tb(tb)[1:]
                msg =' '.join(traceback.format_exception_only(etype, value)
                        +traceback.format_list(tblist))
                print msg


    def OnEmbedPanel(self, evt):
        self.frame.EmbedUnembed(evt.IsChecked())

    def OnShowTools(self, evt):
        raise NotImplementedError # !!!

        conf.showTools = evt.IsChecked()
        g.tools.Show(conf.showTools)
        if conf.showTools:
            self.toolsSizer.Prepend(g.tools, 0, wx.EXPAND)
        else:
            self.toolsSizer.Remove(g.tools)
        self.toolsSizer.Layout()
        
    def OnTest(self, evt):
        raise NotImplementedError # !!!

        if not tree.selection: return   # key pressed event
        tree.ShowTestWindow(tree.selection)

    def OnTestHide(self, evt):
        raise NotImplementedError # !!!

        tree.CloseTestWindow()

    def OnUpdateUI(self, evt):
        if self.inUpdateUI: return          # Recursive call protection
        self.inUpdateUI = True
        if evt.GetId() in [wx.ID_CUT, wx.ID_COPY, wx.ID_DELETE]:
            evt.Enable(bool(self.tree.GetSelection()))
        elif evt.GetId() == wx.ID_SAVE:
            evt.Enable(Presenter.modified)
        elif evt.GetId() in [wx.ID_PASTE, self.frame.ID_TOOL_PASTE]:
# !!! Does not work on wxGTK
#             enabled = False
#             if not wx.TheClipboard.IsOpened() and wx.TheClipboard.Open():
#                 data = wx.CustomDataObject('XRCED_elem')
#                 if wx.TheClipboard.IsSupported(data.GetFormat()):
#                     enabled = True
#                 else:
#                     data = wx.CustomDataObject('XRCED_node')
#                     if wx.TheClipboard.IsSupported(data.GetFormat()):
#                         enabled = True
#                 wx.TheClipboard.Close()
#             evt.Enable(enabled)
            evt.Enable(True)
        elif evt.GetId() in [self.frame.ID_TEST,
                             self.frame.ID_MOVEUP, self.frame.ID_MOVEDOWN,
                             self.frame.ID_MOVELEFT, self.frame.ID_MOVERIGHT]:
            evt.Enable(bool(self.tree.GetSelection()))
        elif evt.GetId() in [self.frame.ID_LOCATE, self.frame.ID_TOOL_LOCATE,
                             self.frame.ID_REFRESH]:
            evt.Enable(g.testWin is not None)
        elif evt.GetId() == wx.ID_UNDO:  evt.Enable(g.undoMan.CanUndo())
        elif evt.GetId() == wx.ID_REDO:  evt.Enable(g.undoMan.CanRedo())
        self.inUpdateUI = False

    def OnIdle(self, evt):
        if self.inIdle: return          # Recursive call protection
        self.inIdle = True
        if not Presenter.applied:
            item = self.tree.GetSelection()
            if item: Presenter.update(item)
        self.inIdle = False
        return

        #print 'onidle',tree.needUpdate,tree.pendingHighLight
        try:
            if tree.needUpdate:
                if conf.autoRefresh:
                    if g.testWin:
                        #self.SetStatusText('Refreshing test window...')
                        # (re)create
                        tree.CreateTestWin(g.testWin.item)
                        #self.SetStatusText('')
                    tree.needUpdate = False
            elif tree.pendingHighLight:
                try:
                    tree.HighLight(tree.pendingHighLight)
                except:
                    # Remove highlight if any problem
                    if g.testWin and g.testWin.highLight:
                        g.testWin.highLight.Remove()
                    tree.pendingHighLight = None
                    raise
            else:
                evt.Skip()
        finally:
            self.inIdle = False

    def OnIconize(self, evt):
        raise NotImplementedError # !!!

        if evt.Iconized():
            conf.x, conf.y = self.GetPosition()
            conf.width, conf.height = self.GetSize()
            if conf.embedPanel:
                conf.sashPos = self.splitter.GetSashPosition()
            else:
                conf.panelX, conf.panelY = self.miniFrame.GetPosition()
                conf.panelWidth, conf.panelHeight = self.miniFrame.GetSize()
                self.miniFrame.Show(False)
        else:
            if not conf.embedPanel:
                self.miniFrame.Show(True)
        evt.Skip()

    # Expand/collapse subtree
    def OnExpand(self, evt):
        if self.tree.GetSelection(): 
            map(self.tree.ExpandAll, self.tree.GetSelections())
        else: self.tree.ExpandAll(self.tree.root)

    def OnCollapse(self, evt):
        if self.tree.GetSelection(): 
            map(self.tree.CollapseAll, self.tree.GetSelections())
        else: self.tree.CollapseAll(self.tree.root)


    #
    # XMLTree event handlers
    #
    
    def OnTreeLeftDown(self, evt):
        pt = evt.GetPosition();
        item, flags = self.tree.HitTest(pt)
#        print item,flags
        if flags & wx.TREE_HITTEST_NOWHERE or not item:
            self.tree.UnselectAll()
        evt.Skip()

    def OnTreeRightDown(self, evt):
        forceSibling = evt.ControlDown()
        forceInsert = evt.ShiftDown()
        Presenter.popupMenu(forceSibling, forceInsert, evt.GetPosition())

    def OnTreeSelChanging(self, evt):
        # Permit multiple selection for same level only
        state = wx.GetMouseState()
        oldItem = evt.GetOldItem()
        if oldItem and (state.ShiftDown() or state.ControlDown()) and \
           self.tree.GetItemParent(oldItem) != self.tree.GetItemParent(evt.GetItem()):
            evt.Veto()
            self.frame.SetStatusText('Veto selection (not same level)')
            return
        evt.Skip()

    def OnTreeSelChanged(self, evt):
        if evt.GetOldItem(): 
            Presenter.update(evt.GetOldItem())
        # Tell presenter to update current data and view
        Presenter.setData(evt.GetItem())
        # Set initial sibling/insert modes
        Presenter.createSibling = not Presenter.comp.isContainer()
        Presenter.insertBefore = False
        evt.Skip()

    def OnTreeItemCollapsed(self, evt):
        # If no selection, reset panel
        if not self.tree.GetSelection():
            if not Presenter.applied: Presenter.update()
            Presenter.setData(None)
        evt.Skip()

# Singleton class
Listener = _Listener()
