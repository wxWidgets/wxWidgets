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
from XMLTreeMenu import *

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
        
        # Component events
        wx.EVT_MENU_RANGE(frame, Manager.firstId, Manager.lastId,
                          self.OnComponent)

        # Other events
#        wx.EVT_IDLE(frame, self.OnIdle)
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
        wx.EVT_MENU(frame, wx.ID_CUT, self.OnCutDelete)
        wx.EVT_MENU(frame, wx.ID_COPY, self.OnCopy)
        wx.EVT_MENU(frame, wx.ID_PASTE, self.OnPaste)
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
        wx.EVT_UPDATE_UI(frame, frame.ID_LOCATE, self.OnUpdateUI)
        wx.EVT_UPDATE_UI(frame, frame.ID_TOOL_LOCATE, self.OnUpdateUI)
        wx.EVT_UPDATE_UI(frame, frame.ID_TOOL_PASTE, self.OnUpdateUI)
        wx.EVT_UPDATE_UI(frame, wx.ID_UNDO, self.OnUpdateUI)
        wx.EVT_UPDATE_UI(frame, wx.ID_REDO, self.OnUpdateUI)
        wx.EVT_UPDATE_UI(frame, wx.ID_DELETE, self.OnUpdateUI)
        wx.EVT_UPDATE_UI(frame, frame.ID_TEST, self.OnUpdateUI)
        wx.EVT_UPDATE_UI(frame, frame.ID_MOVEUP, self.OnUpdateUI)
        wx.EVT_UPDATE_UI(frame, frame.ID_MOVEDOWN, self.OnUpdateUI)
        wx.EVT_UPDATE_UI(frame, frame.ID_MOVELEFT, self.OnUpdateUI)
        wx.EVT_UPDATE_UI(frame, frame.ID_MOVERIGHT, self.OnUpdateUI)
        wx.EVT_UPDATE_UI(frame, frame.ID_REFRESH, self.OnUpdateUI)

        # XMLTree events
        # Register events
        tree.Bind(wx.EVT_RIGHT_DOWN, self.OnTreeRightDown)
        tree.Bind(wx.EVT_TREE_SEL_CHANGING, self.OnTreeSelChanging)
        tree.Bind(wx.EVT_TREE_SEL_CHANGED, self.OnTreeSelChanged)        
        
    def OnComponent(self, evt):
        '''Hadnler for creating new elements.'''
        comp = Manager.findById(evt.GetId())
        print comp
        Presenter.create(comp)

    def OnCutDelete(self, evt):
        '''wx.ID_CUT and wx.ID_DELETE hadndler.'''
        Presenter.delete()

    def OnNew(self, evt):
        '''wx.ID_NEW hadndler.'''
        if not self.AskSave(): return
        Presenter.init()

    def OnOpen(self, evt):
        '''wx.ID_OPEN handler.'''
        if not self.AskSave(): return
        dlg = wx.FileDialog(self.frame, 'Open', os.path.dirname(Presenter.getPath()),
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
        path = Presenter.getPath()
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
                tmpFile,tmpName = tempfile.mkstemp(prefix='xrced-')
                os.close(tmpFile)
                Presenter.save(tmpName) # save temporary file first
                shutil.move(tmpName, path)
                print path
                self.dataFile = path
                Presenter.setModified(False)
                if g.conf.localconf.ReadBool("autogenerate", False):
                    pypath = g.conf.localconf.Read("filename")
                    embed = g.conf.localconf.ReadBool("embedResource", False)
                    genGettext = g.conf.localconf.ReadBool("genGettext", False)
                    self.GeneratePython(self.dataFile, pypath, embed, genGettext)
                    
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
#        if not panel.GetPageCount() == 2:
#            panel.page2.Destroy()
#        else:
            # If we don't do this, page does not get destroyed (a bug?)
#            panel.RemovePage(1)
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
        # Extra check to not mess with idle updating
        if undoMan.CanUndo():
            undoMan.Undo()
            g.panel.SetModified(False)
            if not undoMan.CanUndo():
                self.SetModified(False)

    def OnRedo(self, evt):
        if undoMan.CanRedo():
            undoMan.Redo()
            self.SetModified(True)

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
        elif not parent.hasChildren:
            error = True
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

        # Remove highlight, update testWin
        if g.testWin and g.testWin.highLight:
            g.testWin.highLight.Remove()
            tree.needUpdate = True

        # Undo info
        self.lastOp = 'MOVEUP'
        status = 'Moved before previous sibling'

        # Prepare undo data
        panel.Apply()
        tree.UnselectAll()

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

        # Remove highlight, update testWin
        if g.testWin and g.testWin.highLight:
            g.testWin.highLight.Remove()
            tree.needUpdate = True

        # Undo info
        self.lastOp = 'MOVEDOWN'
        status = 'Moved after next sibling'

        # Prepare undo data
        panel.Apply()
        tree.UnselectAll()

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

        if g.testWin and g.testWin.highLight:
            g.testWin.highLight.Remove()
            tree.needUpdate = True

        # Undo info
        self.lastOp = 'MOVELEFT'
        status = 'Made next sibling of parent'

        # Prepare undo data
        panel.Apply()
        tree.UnselectAll()

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

        # Remove highlight, update testWin
        if g.testWin and g.testWin.highLight:
            g.testWin.highLight.Remove()
            tree.needUpdate = True

        # Prepare undo data
        panel.Apply()
        tree.UnselectAll()

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
        tree.Expand(selected)
        tree.SelectItem(selected)

        undoMan.RegisterUndo(UndoMove(oldParent, oldIndex, newParent, newIndex))

        self.modified = True
        self.SetStatusText(status)

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

    def OnUpdateUI(self, evt):
        if evt.GetId() in [wx.ID_CUT, wx.ID_COPY, wx.ID_DELETE]:
            evt.Enable(bool(self.tree.GetSelection()))
        elif evt.GetId() == wx.ID_SAVE:
            evt.Enable(Presenter.modified)
        elif evt.GetId() in [wx.ID_PASTE, self.frame.ID_TOOL_PASTE]:
            evt.Enable(bool(self.tree.GetSelection()))
        elif evt.GetId() in [self.frame.ID_TEST,
                             self.frame.ID_MOVEUP, self.frame.ID_MOVEDOWN,
                             self.frame.ID_MOVELEFT, self.frame.ID_MOVERIGHT]:
            evt.Enable(bool(self.tree.GetSelection()))
        elif evt.GetId() in [self.frame.ID_LOCATE, self.frame.ID_TOOL_LOCATE,
                             self.frame.ID_REFRESH]:
            evt.Enable(g.testWin is not None)
        elif evt.GetId() == wx.ID_UNDO:  evt.Enable(g.undoMan.CanUndo())
        elif evt.GetId() == wx.ID_REDO:  evt.Enable(g.undoMan.CanRedo())

    def OnIdle(self, evt):
        if self.inIdle: return          # Recursive call protection
        self.inIdle = True
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



    #
    # XMLTree event handlers
    #
    
    def OnTreeRightDown(self, evt):
        menu = XMLTreeMenu(self.tree)
        self.tree.PopupMenu(menu, evt.GetPosition())
        menu.Destroy()

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
            print 'old:', self.tree.GetItemText(evt.GetOldItem())
            Presenter.update()
        if evt.GetItem(): print 'new:',self.tree.GetItemText(evt.GetItem())
        # Tell presenter to update current data and view
        Presenter.setData(evt.GetItem())
        evt.Skip()



# Singleton class
Listener = _Listener()
