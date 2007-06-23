# Name:         presenter.py
# Purpose:      Presenter part
# Author:       Roman Rolinsky <rolinsky@femagsoft.com>
# Created:      07.06.2007
# RCS-ID:       $Id$

import os
from globals import *
import view
from model import Model
from component import Manager

# Presenter class linking model to view objects
class _Presenter:
    def init(self):
        Model.init()
        self.setModified(False)
        view.frame.Clear()
        view.tree.Clear()
        view.tree.SetPyData(view.tree.root, Model.mainNode)
        self.panels = []
        self.comp = None                # component shown in panel
        self.container = None           # current container (None if root)

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
        try:
            self.saveXML(path)
        except:
            inf = sys.exc_info()
            wx.LogError('Error writing file: %s' % path)
            if debug: raise
            raise

    def setModified(self, state=True):
        '''set global modified state.'''
        self.modified = state
        name = os.path.basename(Model.path)
        if not name: name = 'UNTITLED'
        # Update GUI
        if state:
            view.frame.SetTitle(progname + ': ' + name + ' *')
        else:
            view.frame.SetTitle(progname + ': ' + name)

    def setData(self, item):
        '''Set data and view for current tree item.'''
        if item == view.tree.root:
            print 'NYI'
            self.comp = self.container = None
            view.panel.Clear()
            self.panels = []
        else:
            node = view.tree.GetPyData(item)
            className = node.getAttribute('class')
            self.comp = Manager.components[className]
            print self.comp
            parentItem = view.tree.GetItemParent(item)
            parentNode = view.tree.GetPyData(parentItem)
            if parentNode == Model.mainNode:
                self.container = None
            else:
                parentClass = parentNode.getAttribute('class')
                self.container = Manager.components[parentClass]
            self.panels = view.panel.SetData(self.container, self.comp, node)

    def create(self, comp):
        # Add dom node
        node = Model.createObjectNode(comp.name)
        item = view.tree.GetSelection()
        parentNode = view.tree.GetPyData(item)
        if self.comp:
            self.comp.appendChild(parentNode, node)
        else:
            Model.mainNode.appendChild(node)
        view.tree.AppendItem(item, comp.name, comp.imageId, data=wx.TreeItemData(node))
        view.tree.Expand(item)
        # Notify Presenter
        self.setModified()

    def update(self):
        '''update DOM with new attribute values'''
        if self.comp and self.comp.hasName:
            view.panel.node.setAttribute('name', view.panel.controlName.GetValue())
        for panel in self.panels:
            # Replace node contents except object children
            for n in panel.node.childNodes[:]:
                if not is_object(n):
                    panel.node.removeChild(n)
                    n.unlink()
        for panel in self.panels:
            for a,w in panel.controls:
                value = w.GetValue()
                print a,value
                if value: 
                    elem = Model.dom.createElement(a)
                    text = Model.dom.createTextNode(w.GetValue())
                    elem.appendChild(text)
                    panel.node.appendChild(elem)
                    panel.node.appendChild(Model.dom.createTextNode('\n'))

    def delete(self):
        '''delete selected objects'''
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

    def createLocalConf(self, path):
        name = os.path.splitext(path)[0]
        name += '.xcfg'
        return wx.FileConfig(localFilename=name)


# Singleton class
Presenter = _Presenter()
