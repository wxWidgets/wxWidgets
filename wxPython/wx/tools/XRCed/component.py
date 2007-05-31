# Name:         component.py
# Purpose:      base component classes
# Author:       Roman Rolinsky <rolinsky@femagsoft.com>
# Created:      31.05.2007
# RCS-ID:       $Id$

import bisect
import sys
import wx

class Component(object):
    '''base component class'''
    def __init__(self, name, group, attributes):
        self.name = name
        self.group = group
        self.attributes = attributes
        # Tree image
        self.image = None
        self.imageId = 0

    def setData(self, node):
        self.node = node

    # Order components having same index by group and name
    def __cmp__(self, other):
        if self.group < other.group: return -1
        elif self.group == other.group: 
            if self.name < other.name: return -1
            elif self.name == other.name: return 0
            else: return 1
        else: return 1

    def __repr__(self):
        return "Component('%s', %s)" % (self.name, self.attributes)

    def canHaveChild(self, component):
        '''true if the current component can have child of given type'''
        return False

    def canBeReplaced(self, component):
        '''true if the current component can be replaced by component'''
        return component.group == group


class ContainerComponent(Component):
    '''base class for containers'''
    def canHaveChild(self, component):
        return True


class ComponentManager:
    '''manager instance collects information from component plugins.'''
    def __init__(self):
        self.components = {}
        self.ids = {}
        self.menus = {}
        self.panels = {}
        # None reserved for top-level menu
        self.menuNames = [None, 'control', 'button', 'box', 
                          'container', 'sizer', 'custom']
        self.panelNames = ['Windows', 'Menus', 'Sizers', 
                           'Controls', 'Custom']

    def register(self, component):
        self.components[component.name] = component
        component.id = wx.NewId()
        self.ids[component.id] = component

    def setMenu(self, component, menu, label, help, index=sys.maxint):
        if menu not in self.menuNames: self.menuNames.append(menu)
        if menu not in self.menus: self.menus[menu] = []
        bisect.insort_left(self.menus[menu], (index, component, label, help))

    def setTool(self, component, panel, bitmap, index=sys.maxint):
        if panel not in self.panelNames: self.panelNames.append(panel)
        if panel not in self.panels: self.panels[panel] = []
        bisect.insort_left(self.panels[panel], (index, component, bitmap))
        
    def findById(self, id):
        return self.ids[id]

manager = ComponentManager()
