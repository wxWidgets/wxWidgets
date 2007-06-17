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

    def getPath(self):
        return Model.path

    def loadXML(self, path):
        Model.loadXML(path)
        view.tree.Flush()

    def saveXML(self, path):
        Model.saveXML(path)

    def setModified(self, state=True):
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
        else:
            node = view.tree.GetPyData(item)
            className = node.getAttribute('class')
            comp = Manager.components[className]
            print comp
            view.panel.SetData(comp, node)

    def create(self, comp):
        # Add dom node
        node = Model.createObjectNode(comp.name)
        item = view.tree.GetSelection()
        parentNode = view.tree.GetPyData(item)
        parentNode.appendChild(node)
        view.tree.AppendItem(item, comp.name, comp.imageId, data=wx.TreeItemData(node))
        view.tree.Expand(item)
        # Notify Presenter
        self.setModified()


# Singleton class
Presenter = _Presenter()
