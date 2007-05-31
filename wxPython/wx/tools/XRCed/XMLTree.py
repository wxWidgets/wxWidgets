# Name:         XMLTree.py
# Purpose:      XMLTree class
# Author:       Roman Rolinsky <rolinsky@femagsoft.com>
# Created:      31.05.2007
# RCS-ID:       $Id$

from XMLTreeMenu import *
import images

class XMLTree(wx.TreeCtrl):
    def __init__(self, parent):
        style = wx.TR_HAS_BUTTONS | wx.TR_MULTIPLE | wx.TR_EDIT_LABELS
        wx.TreeCtrl.__init__(self, parent, style=style)

        # Item colour
        self.COLOUR_COMMENT  = wx.Colour(0, 0, 255)
        self.COLOUR_REF      = wx.Colour(0, 0, 128)
        self.COLOUR_HIDDEN   = wx.Colour(128, 128, 128)
        self.COLOUR_HL       = wx.Colour(255, 0, 0)
        self.COLOUR_DT       = wx.Colour(0, 64, 0)

        self.SetBackgroundColour(wx.Colour(224, 248, 224))
        self.fontComment = wx.FFont(self.GetFont().GetPointSize(),
                                    self.GetFont().GetFamily(),
                                    wx.FONTFLAG_ITALIC)

        # Create image list
        il = wx.ImageList(16, 16, True)
        # 0 index is the default image
        il.Add(images.getTreeDefaultImage().Scale(16,16).ConvertToBitmap())
        self.rootImage = il.Add(images.getTreeRootImage().Scale(16,16).ConvertToBitmap())
        for component in manager.components.values():
            if component.image:
                component.imageId = il.Add(component.image.Scale(16,16).ConvertToBitmap())
        self.il = il
        self.SetImageList(il)
        self.AddRoot('XML tree', self.rootImage)

        # Register events
        wx.EVT_RIGHT_DOWN(self, self.OnRightDown)

        self.ctrl = self.shift = False

    def OnRightDown(self, evt):
        menu = XMLTreeMenu(self)
        self.PopupMenu(menu, evt.GetPosition())
        menu.Destroy()

    def NeedInsert(self, item):
#        isContainer = self.GetPyData(item).hasChildren
        isContainer = True      # DEBUG
        if item == self.GetRootItem(): return False
        # If leaf item or collapsed container, then insert mode
        return not isContainer or \
            self.GetChildrenCount(item, False) and not self.IsExpanded(item)
