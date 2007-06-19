# Name:         XMLTreeMenu.py
# Purpose:      dynamic pulldown menu for XMLTree
# Author:       Roman Rolinsky <rolinsky@femagsoft.com>
# Created:      31.05.2007
# RCS-ID:       $Id$

import wx
from component import Manager

# Global id constants
class ID:
    MENU = wx.NewId()
    EXPAND = wx.NewId()
    COLLAPSE = wx.NewId()
    PASTE_SIBLING = wx.NewId()
    TOOL_PASTE = wx.NewId()
    INSERT = wx.NewId()
    APPEND = wx.NewId()
    SIBLING = wx.NewId()
    SUBCLASS = wx.NewId()
    REF = wx.NewId()
    COMMENT = wx.NewId()


class XMLTreeMenu(wx.Menu):
    '''dynamic pulldown menu for XMLTree'''
    def __init__(self, tree):
        wx.Menu.__init__(self)
        item = tree.GetSelection()
        if not item:
            self.Append(ID.EXPAND, 'Expand', 'Expand tree')
            self.Append(ID.COLLAPSE, 'Collapse', 'Collapse tree')
            return              # no commands if no selection
        # Populate create menu
        if tree.forceSibling:
            needInsert = True
        else:
            needInsert = tree.NeedInsert(item)
        root = tree.GetRootItem()
        if item == root or needInsert and tree.GetItemParent(item) == root:
            menu = self.CreateTopLevelMenu()
        else:
            menu = self.CreateSubMenus()
        # Select correct label for create menu
        if not needInsert:
            if tree.forceInsert:
                self.AppendMenu(ID.INSERT, 'Insert Child', menu,
                                'Create child object as the first child')
            else:
                self.AppendMenu(ID.APPEND, 'Append Child', menu,
                                'Create child object as the last child')
        else:
            if tree.forceInsert:
                self.AppendMenu(ID.SIBLING, 'Create Sibling', menu,
                                'Create sibling before selected object')
            else:
                self.AppendMenu(ID.SIBLING, 'Create Sibling', menu,
                                'Create sibling after selected object')

        self.AppendSeparator()
        self.Append(wx.ID_DELETE, 'Delete', 'Delete object')

    def CreateTopLevelMenu(self):
        m = wx.Menu()
        for index,component,label,help in Manager.menus['root']:
            m.Append(component.id, label, help)
        m.AppendSeparator()
        m.Append(ID.REF, 'reference...', 'Create object_ref node')
        m.Append(ID.COMMENT, 'comment', 'Create comment node')        
        return m

    def CreateSubMenus(self):
        menu = wx.Menu()
        for name in Manager.menuNames[1:]:
            # Skip empty menu groups
            if not Manager.menus.get(name, []): continue
            m = wx.Menu()
            for index,component,label,help in Manager.menus[name]:
                m.Append(component.id, label, help)
            menu.AppendMenu(ID.MENU, name, m)
            menu.AppendSeparator()
        menu.Append(ID.REF, 'reference...', 'Create object_ref node')
        menu.Append(ID.COMMENT, 'comment', 'Create comment node')
        return menu
