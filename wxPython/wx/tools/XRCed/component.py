# Name:         component.py
# Purpose:      base component classes
# Author:       Roman Rolinsky <rolinsky@femagsoft.com>
# Created:      31.05.2007
# RCS-ID:       $Id$

import wx,sys,bisect
from sets import Set
from globals import *
from model import Model
from attribute import *
import params

# Group compatibility specifications. 
# Key is the parent group, value is the list of child groups.
# !value means named main group is excluded from possible children.
# "root" is a special group for the tree root
parentChildGroups = {
    'root': ['top_level'],      # top-level objects
    'frame': ['toolbar','menubar'],
    'window': ['control', 'window', '!frame'],
    'sizer': ['control', 'sizer','spacer'],
    'toolbar': ['tool','separator'],
    'menu': ['menu', 'menu_item','separator']
}

class Component(object):
    '''Base component class.'''
    # Common window attributes
    windowAttributes = ['fg', 'bg', 'font', 'enabled', 
                        'focused', 'hidden', 'tooltip', 'help']
    hasName = True                      # most elements have XRC IDs
    def __init__(self, name, groups, attributes, **kargs):
        self.name = name
        self.groups = groups
        self.attributes = attributes
        self.styles = []
        self.exStyles = []
        self.defaults = kargs.get('defaults', {})
        # Special Attribute classes if required
        self.specials = kargs.get('specials', {'font': FontAttribute})
        # Special Param classes if required
        self.params = kargs.get('params', {})
        # Tree image
        if 'images' in kargs:
            self.images = kargs['images']
        elif 'image' in kargs:
            self.images = [kargs['image']]
        elif not 'image' in self.__dict__:
            self.images = []

    def addStyles(self, *styles):
        self.styles.extend(styles)

    def addExStyles(self, *styles):
        self.exStyles.extend(styles)

    def setSpecial(self, attrName, attrClass):
        '''Set special Attribute class.'''
        self.specials[attrName] = attrClass

    def setParamClass(self, attrName, paramClass):
        '''Set special Param class.'''
        self.params[attrName] = paramClass

    def getTreeImageId(self, node):
        try:
            return self.images[0].Id
        except IndexError:
            return 0

    # Order components having same index by group and name
    def __cmp__(self, other):
        if self.groups < other.groups: return -1
        elif self.groups == other.groups: 
            if self.name < other.name: return -1
            elif self.name == other.name: return 0
            else: return 1
        else: return 1

    def __repr__(self):
        return "Component('%s', %s)" % (self.name, self.attributes)

    def canHaveChild(self, component):
        '''True if the current component can have child of given type.

        This function is redefined by container classes.'''
        return False

    def canBeReplaced(self, component):
        '''True if the current component can be replaced by component.

        This function can be redefined by derived classes.'''
        return component.groups == groups

    def isContainer(self):
        return isinstance(self, Container)

    def getAttribute(self, node, attribute):
        for n in node.childNodes:
            if n.nodeType == node.ELEMENT_NODE and n.tagName == attribute:
                attrClass = self.specials.get(attribute, Attribute)
                return attrClass.get(n)
        return ''

    def addAttribute(self, node, attribute, value):
        '''Add attribute element.'''
        attrClass = self.specials.get(attribute, Attribute)
        attrClass.add(node, attribute, value)

class Container(Component):
    '''Base class for containers.'''
    def canHaveChild(self, component):
        # Test exclusion first
        for g in self.groups:
            if '!'+component.groups[0] in parentChildGroups.get(g, []): return False
        # Test for any possible parent-child
        groups = Set(component.groups)
        for g in self.groups: 
            if groups.intersection(parentChildGroups.get(g, [])):
                return True
        return False

    def requireImplicit(self, node):
        '''If there are implicit nodes for this particular node.'''
        return False

    def getTreeNode(self, node):
        '''Some containers may hide some internal elements.'''
        return node

    def appendChild(self, parentNode, node):
        '''Append child node. Can be overriden to create implicit nodes.'''
        parentNode.appendChild(node)

    def insertBefore(self, parentNode, node, nextNode):
        '''Insert node before nextNode. Can be overriden to create implicit nodes.'''
        parentNode.insertBefore(node, nextNode)

    def insertAfter(self, parentNode, node, prevNode):
        '''Insert node after prevNode. Can be overriden to create implicit nodes.'''
        parentNode.insertBefore(node, prevNode.nextSibling)

    def removeChild(self, parentNode, node):
        '''Some containers may remove additional elements.'''
        parentNode.removeChild(node)

class RootComponent(Container):    
    '''Special root component.'''
    windowAttributes = []
    hasName = False

class SmartContainer(Container):
    def __init__(self, name, groups, attributes, **kargs):
        Container.__init__(self, name, groups, attributes, **kargs)
        self.implicitName = kargs['implicit_name']
        self.implicitPageName = kargs['implicit_page']
        self.implicitAttributes = kargs['implicit_attributes']
        # This is optional
        self.implicitParams = kargs.get('implicit_params', {})

    '''Base class for containers with implicit nodes.'''
    def getTreeNode(self, node):
        if node.getAttribute('class') == self.implicitName:
            for n in node.childNodes:
                if is_object(n): return n
        # Maybe some children are not implicit
        return node

    def appendChild(self, parentNode, node):
        if self.requireImplicit(node):
            elem = Model.createObjectNode(self.implicitName)
            elem.appendChild(node)
            parentNode.appendChild(elem)
        else:
            parentNode.appendChild(node)

    def insertBefore(self, parentNode, node, nextNode):
        if self.requireImplicit(nextNode):
            nextNode = nextNode.parentNode
        if self.requireImplicit(node):
            elem = Model.createObjectNode(self.implicitName)
            elem.appendChild(node)
            parentNode.insertBefore(elem, nextNode)
        else:
            parentNode.insertBefore(node, nextNode)

    def insertAfter(self, parentNode, node, prevNode):
        if self.requireImplicit(prevNode):
            nextNode = prevNode.parentNode.nextSibling
        else:
            nextNode = prevNode.nextSibling
        if self.requireImplicit(node):
            elem = Model.createObjectNode(self.implicitName)
            elem.appendChild(node)
            parentNode.insertBefore(elem, nextNode)
        else:
            parentNode.insertBefore(node, nextNode)

    def removeChild(self, parentNode, node):
        if self.requireImplicit(node):
            implicitNode = node.parentNode
            implicitNode.removeChild(node)
            parentNode.removeChild(implicitNode)
            implicitNode.unlink()
        else:
            parentNode.removeChild(node)

    def requireImplicit(self, node):
        # SmartContainer by default requires implicit
        return True

    def setImplicitParamClass(self, attrName, paramClass):
        '''Set special Param class.'''
        self.implicitParams[attrName] = paramClass


class Sizer(SmartContainer):
    '''Sizers are not windows and have common implicit node.'''
    windowAttributes = []
    hasName = False

    def __init__(self, name, groups, attributes, **kargs):
        kargs.setdefault('implicit_name', 'sizeritem')
        kargs.setdefault('implicit_page', 'SizeItem Attributes')
        kargs.setdefault('implicit_attributes', ['option', 'flag', 'border', 'minsize', 'ratio'])
        kargs.setdefault('implicit_params', {'option': params.ParamInt, 
                                             'minsize': params.ParamPosSize, 
                                             'ratio': params.ParamPosSize})
        SmartContainer.__init__(self, name, groups, attributes, **kargs)

    def requireImplicit(self, node):
        '''if there are implicit nodes for this particular component'''
        return node.getAttribute('class') != 'spacer'


class BoxSizer(Sizer):
    '''Sizers are not windows and have common implicit node.'''

    def __init__(self, name, groups, attributes, **kargs):
        self.images = kargs.get('images', None)
        Sizer.__init__(self, name, groups, attributes, **kargs)

    def getTreeImageId(self, node):
        if self.getAttribute(node, 'orient') == 'wxVERTICAL':
            return self.images[0].Id
        else:
            return self.images[1].Id
    
class _ComponentManager:
    '''Manager instance collects information from component plugins.'''
    def __init__(self):
        self.rootComponent = RootComponent('root', ['root'], ['encoding'])
        self.components = {}
        self.ids = {}
        self.firstId = self.lastId = -1
        self.menus = {}
        self.panels = {}
        self.menuNames = ['root', 'control', 'button', 'box', 
                          'container', 'sizer', 'custom']
        self.panelNames = ['Windows', 'Panels', 'Menus', 'Sizers', 
                           'Controls', 'Custom']

    def init(self):
        self.firstId = self.lastId = wx.NewId()

    def register(self, component):
        '''Register component object.'''
        self.components[component.name] = component
        # unique wx ID for event handling
        component.id = self.lastId = wx.NewId()
        self.ids[component.id] = component

    def forget(self, name):
        '''Remove registered component.'''
        del self.components[name]
        for menu,iclh in self.menus.items():
            if iclh[1].name == name:
                self.menus[menu].remove(iclh)
        for panel,icb in self.panels.items():
            if icb[1].name == name:
                self.panels[panel].remove(icb)

    def setMenu(self, component, menu, label, help, index=sys.maxint):
        '''Set pulldown menu data.'''
        if menu not in self.menuNames: self.menuNames.append(menu)
        if menu not in self.menus: self.menus[menu] = []
        bisect.insort_left(self.menus[menu], (index, component, label, help))

    def setTool(self, component, panel, bitmap, index=sys.maxint):
        '''Set toolpanel data.'''
        if panel not in self.panelNames: self.panelNames.append(panel)
        if panel not in self.panels: self.panels[panel] = []
        bisect.insort_left(self.panels[panel], (index, component, bitmap))
        
    def findById(self, id):
        return self.ids[id]

# Singleton object
Manager = _ComponentManager()

