# Name:         XMLTree.py
# Purpose:      XMLTree class
# Author:       Roman Rolinsky <rolinsky@femagsoft.com>
# Created:      31.05.2007
# RCS-ID:       $Id$

from xml.dom import minidom
from XMLTreeMenu import *
from model import Model
from presenter import Presenter
import images

# Test for object elements (!!! move somewhere?)
def is_object(node):
    return node.nodeType == minidom.Node.ELEMENT_NODE and \
           node.tagName in ['object', 'object_ref'] or \
           node.nodeType == minidom.Node.COMMENT_NODE

# Test for object elements (!!! move somewhere?)
def is_object(node):
    return node.nodeType == minidom.Node.ELEMENT_NODE and \
           node.tagName in ['object', 'object_ref'] or \
           node.nodeType == minidom.Node.COMMENT_NODE


class XMLTree(wx.TreeCtrl):
    def __init__(self, parent):
        style = wx.TR_HAS_BUTTONS | wx.TR_MULTIPLE | wx.TR_EDIT_LABELS
        wx.TreeCtrl.__init__(self, parent, style=style)

        # Color scheme
        self.SetBackgroundColour(wx.Colour(224, 248, 224))
        self.COLOUR_COMMENT  = wx.Colour(0, 0, 255)
        self.COLOUR_REF      = wx.Colour(0, 0, 128)
        self.COLOUR_HIDDEN   = wx.Colour(128, 128, 128)
        self.COLOUR_HL       = wx.Colour(255, 0, 0)
        self.COLOUR_DT       = wx.Colour(0, 64, 0)

        # Comments use italic font
        self.fontComment = wx.FFont(self.GetFont().GetPointSize(),
                                    self.GetFont().GetFamily(),
                                    wx.FONTFLAG_ITALIC)

        # Create image list
        il = wx.ImageList(16, 16, True)
        # 0 is the default image index
        il.Add(images.getTreeDefaultImage().Scale(16,16).ConvertToBitmap())
        # 1 is root
        self.rootImage = il.Add(images.getTreeRootImage().Scale(16,16).ConvertToBitmap())
        # Loop through registered components which have images
        for component in Manager.components.values():
            if component.image:
                component.imageId = il.Add(component.image.Scale(16,16).ConvertToBitmap())
        self.il = il
        self.SetImageList(il)

        self.root = self.AddRoot('XML tree', self.rootImage)
        self.SetItemHasChildren(self.root)
        self.Expand(self.root)

        # Register events
        self.Bind(wx.EVT_RIGHT_DOWN, self.OnRightDown)
        self.Bind(wx.EVT_TREE_SEL_CHANGED, self.OnSelChanged)
        
        # Insert/append mode flags
        self.forceSibling = self.forceInsert = False

    def OnRightDown(self, evt):
        menu = XMLTreeMenu(self)
        self.PopupMenu(menu, evt.GetPosition())
        menu.Destroy()

    def OnSelChanged(self, evt):
        if evt.GetOldItem(): print 'old:',self.GetItemText(evt.GetOldItem())
        if evt.GetItem(): print 'new:',self.GetItemText(evt.GetItem())
        # Tell presenter to update current data and view
        Presenter.setData(evt.GetItem())
        evt.Skip()

    def NeedInsert(self, item):
        '''return true if item must be inserted after current vs. appending'''
        if item == self.GetRootItem(): return False
#        isContainer = self.GetPyData(item).hasChildren
        isContainer = True      # DEBUG
        # If leaf item or collapsed container, then insert mode
        return not isContainer or \
            self.GetChildrenCount(item, False) and not self.IsExpanded(item)

    def Clear(self):
        '''Clear everything except the root item.'''
        self.UnselectAll()
        self.DeleteChildren(self.root)
        self.Expand(self.root)
        # Reset item data after possible model change
        self.SetPyData(self.root, Model.mainNode)

    # Add tree item for given parent item if node is DOM element node with
    # object/object_ref tag. xxxParent is parent xxx object
    def AddNode(self, parent, node):
        # Append tree item
        className = node.getAttribute('class')
        comp = Manager.components[className]
        item = self.AppendItem(parent, className, image=comp.imageId,
                               data=wx.TreeItemData(node))
        # Different color for comments and references
        if className == 'comment':
            self.SetItemTextColour(item, self.COLOUR_COMMENT)
            self.SetItemFont(item, self.fontComment)
#        elif treeObj.ref:
#            self.SetItemTextColour(item, self.COLOUR_REF)
#        elif treeObj.hasStyle and treeObj.params.get('hidden', False):
#            self.SetItemTextColour(item, self.COLOUR_HIDDEN)
        # Try to find children objects
        for n in filter(is_object, node.childNodes):
            self.AddNode(item, n)

    def Flush(self):
        '''Update all items after changes in model.'''
        self.Clear()
        for n in filter(is_object, Model.mainNode.childNodes):
            self.AddNode(self.root, n)
