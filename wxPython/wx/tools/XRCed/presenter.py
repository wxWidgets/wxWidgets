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
        self.panels = []

    def getPath(self):
        return Model.path

    def loadXML(self, path):
        Model.loadXML(path)
        view.tree.Flush()

    def saveXML(self, path):
        Model.saveXML(path)

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
            self.panels = []
        else:
            node = view.tree.GetPyData(item)
            className = node.getAttribute('class')
            comp = Manager.components[className]
            print comp
            self.panels = view.panel.SetData(comp, node)

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

    def update(self):
        '''update DOM with new attribute values'''
        for panel in self.panels:
            # Replace node contents except object children
            for n in panel.node.childNodes[:]:
                if not is_object(n):
                    panel.node.removeChild(n)
                    n.unlink()
        print view.panel.controls
        for a,w in view.panel.controls.items():
            value = w.GetValue()
            if not value: continue
            print value
            elem = Model.dom.createElement(a)
            text = Model.dom.createTextNode(w.GetValue())
            print a,w.GetValue()
            elem.appendChild(text)
            view.panel.node.appendChild(elem)
            view.panel.node.appendChild(Model.dom.createTextNode('\n'))

# Singleton class
Presenter = _Presenter()
