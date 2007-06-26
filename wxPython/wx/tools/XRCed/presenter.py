# Name:         presenter.py
# Purpose:      Presenter part
# Author:       Roman Rolinsky <rolinsky@femagsoft.com>
# Created:      07.06.2007
# RCS-ID:       $Id$

import os,tempfile,shutil
from globals import *
import view
from model import Model
from component import Manager

# Presenter class linking model to view objects
class _Presenter:
    def init(self):
        Model.init()
        # Global modified state
        self.setModified(False)
        view.frame.Clear()
        view.tree.Clear()
        view.tree.SetPyData(view.tree.root, Model.mainNode)
        view.panel.Clear()
        self.panels = []
        self.comp = Manager.rootComponent # component shown in panel (or the root component)
        self.container = None           # current container (None if root)
        # Insert/append mode flags
        self.forceSibling = self.forceInsert = False

    def getPath(self):
        return Model.path

    def loadXML(self, path):
        Model.loadXML(path)
        view.tree.Flush()
        view.tree.SetPyData(view.tree.root, Model.mainNode)

    def saveXML(self, path):
        Model.saveXML(path)

    def open(self, path):
        if not os.path.exists(path):
            wx.LogError('File does not exists: %s' % path)
            return False
        try:
            self.loadXML(path)
            g.conf.localconf = self.createLocalConf(path)
        except:
            inf = sys.exc_info()
            wx.LogError('Error reading file: %s' % path)
            if debug: raise
            return False
        return True
            
    def save(self, path):
        # Apply changes if needed
        if not self.applied:
            self.update(view.tree.GetSelection())
        try:
            tmpFile,tmpName = tempfile.mkstemp(prefix='xrced-')
            os.close(tmpFile)
            self.saveXML(tmpName)
            shutil.move(tmpName, path)
            Model.path = path
            self.setModified(False)
        except:
            inf = sys.exc_info()
            wx.LogError('Error writing file: %s' % path)
            if debug: raise
            raise

    def setModified(self, state=True):
        '''Set global modified state.'''
        self.modified = state
        # Panel applied flag
        self.applied = not state
        name = os.path.basename(Model.path)
        if not name: name = 'UNTITLED'
        # Update GUI
        if state:
            view.frame.SetTitle(progname + ': ' + name + ' *')
        else:
            view.frame.SetTitle(progname + ': ' + name)

    def setApplied(self, state=True):
        '''Set panel state.'''
        self.applied = state
        if not state and not self.modified: 
            self.setModified()  # toggle global state

    def setData(self, item):
        '''Set data and view for current tree item.'''
        if not item or item == view.tree.root:
            self.container = None
            self.comp = Manager.rootComponent
            self.panels = view.panel.SetData(None, self.comp, None)
        else:
            node = view.tree.GetPyData(item)
            className = node.getAttribute('class')
            self.comp = Manager.components[className]
            parentItem = view.tree.GetItemParent(item)
            parentNode = view.tree.GetPyData(parentItem)
            if parentNode == Model.mainNode:
                self.container = Manager.rootComponent
            else:
                parentClass = parentNode.getAttribute('class')
                self.container = Manager.components[parentClass]
            self.panels = view.panel.SetData(self.container, self.comp, node)

    def popupMenu(self, forceSibling, forceInsert, pos):
        '''Show popup menu and set sibling/insert flags.'''
        if not self.comp.isContainer():
            self.createSibling = True
        else:
            self.createSibling = forceSibling
        self.insertBefore = forceInsert
        menu = view.XMLTreeMenu(view.tree, self.createSibling, self.insertBefore)
        view.tree.PopupMenu(menu, pos)
        menu.Destroy()        

    # !!! NOT USED AT THE MOMENT
    def needInsert(self, item):
        '''Return True if item must be inserted after current vs. appending'''
        if item == self.GetRootItem(): return False
#        isContainer = self.GetPyData(item).hasChildren
        isContainer = True      # DEBUG
        # If leaf item or collapsed container, then insert mode
        return not isContainer or \
            self.GetChildrenCount(item, False) and not self.IsExpanded(item)

    def create(self, comp):
        # Add DOM node as child or sibling depending on flags
        child = Model.createObjectNode(comp.name)
        data = wx.TreeItemData(child)
        item = view.tree.GetSelection()
        root = view.tree.GetRootItem()
        if not item: 
            item = root
        if item == root:
            self.createSibling = False # can't create sibling of root
        if self.createSibling:
            parentItem = view.tree.GetItemParent(item)
            parentNode = view.tree.GetPyData(parentItem)
        else:
            parentNode = view.tree.GetPyData(item)
        if self.createSibling:
            node = view.tree.GetPyData(item)
            if self.insertBefore:
                self.container.insertBefore(parentNode, child, node)
                view.tree.InsertItemBefore(parentItem, item, comp.name, 
                                           comp.getTreeImageId(child), data=data)
            else:
                self.container.insertAfter(parentNode, child, node)
                view.tree.InsertItem(parentItem, item, comp.name, 
                                     comp.getTreeImageId(child), data=data)
            if parentItem != root:
                view.tree.Expand(parentItem)
        else:
            if self.insertBefore and parentNode.firstChild:
                nextNode = self.comp.getTreeNode(parentNode.firstChild)
                self.comp.insertBefore(parentNode, child, nextNode)
                view.tree.PrependItem(item, comp.name, comp.getTreeImageId(child), data=data)
            else:
                self.comp.appendChild(parentNode, child)
                view.tree.AppendItem(item, comp.name, comp.getTreeImageId(child), data=data)
            if item != root:
                view.tree.Expand(item)
        # Notify Presenter
        self.setModified()

    def update(self, item):
        '''Update DOM with new attribute values. Update tree if necessary.'''
        node = view.tree.GetPyData(item)
        if self.comp and self.comp.hasName:
            node.setAttribute('name', view.panel.controlName.GetValue())
        for panel in self.panels:
            # Replace node contents except object children
            for n in panel.node.childNodes[:]:
                if not is_object(n):
                    panel.node.removeChild(n)
                    n.unlink()
        for panel in self.panels:
            for a,w in panel.controls:
                value = w.GetValue()
                #print a,value
                if value: 
                    self.comp.addAttribute(panel.node, a, value)
        view.tree.SetItemImage(item, self.comp.getTreeImageId(node))
        self.setApplied()

    def delete(self):
        '''Delete selected object(s).'''
        item = view.tree.GetSelection()
        parentItem = view.tree.GetItemParent(item)
        parentNode = view.tree.GetPyData(parentItem)
        for item in view.tree.GetSelections():
            node = view.tree.GetPyData(item)
            if parentItem == view.tree.root:
                Model.mainNode.removeChild(node)
            else:
                self.container.removeChild(parentNode, node)
            node.unlink()
            view.tree.Delete(item)
        view.panel.Clear()

    def cut(self):
        raise NotImplementedError

    def createLocalConf(self, path):
        name = os.path.splitext(path)[0]
        name += '.xcfg'
        return wx.FileConfig(localFilename=name)


# Singleton class
Presenter = _Presenter()
