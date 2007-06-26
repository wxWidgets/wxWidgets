# Name:         XMLTreeMenu.py
# Purpose:      dynamic pulldown menu for XMLTree
# Author:       Roman Rolinsky <rolinsky@femagsoft.com>
# Created:      31.05.2007
# RCS-ID:       $Id$

import wx
from globals import ID
from component import Manager

class XMLTreeMenu(wx.Menu):
    '''dynamic pulldown menu for XMLTree'''
    def __init__(self, tree, createSibling, insertBefore):
        '''
        Create tree pull-down menu. createSibling flag is set if the
        child must be a sibling of the selected item, insertBefore
        flag is set if the child should be put before selected item in
        sibling mode or as the first child in non-sibling mode.
        '''
        wx.Menu.__init__(self)
        items = tree.GetSelections()
        if len(items) <= 1:
            item = tree.GetSelection()
            if not item: item = tree.root
            if item == tree.root or tree.GetItemParent(item) == tree.root and createSibling:
                menu = self.CreateTopLevelMenu()
            else:
                menu = self.CreateSubMenus()
            # Select correct label for submenu
            if createSibling:
                if insertBefore:
                    self.AppendMenu(ID.SIBLING, 'Create Sibling', menu,
                                    'Create sibling before selected object')
                else:
                    self.AppendMenu(ID.SIBLING, 'Create Sibling', menu,
                                    'Create sibling after selected object')
            else:
                if insertBefore:
                    self.AppendMenu(ID.INSERT, 'Insert', menu,
                                    'Create object as the first child')
                else:
                    self.AppendMenu(ID.APPEND, 'Append', menu,
                                    'Create object as the last child')

            self.AppendSeparator()
            self.Append(wx.ID_CUT, 'Cut', 'Cut to the clipboard')
            self.Append(wx.ID_COPY, 'Copy', 'Copy to the clipboard')
            if createSibling and item != tree.root:
                self.Append(ID.PASTE_SIBLING, 'Paste Sibling',
                            'Paste from the clipboard as a sibling')
            else:
                self.Append(wx.ID_PASTE, 'Paste', 'Paste from the clipboard')
        if items:
            self.Append(wx.ID_DELETE, 'Delete', 'Delete selected objects')
            self.AppendSeparator()
        self.Append(ID.EXPAND, 'Expand', 'Expand tree')
        self.Append(ID.COLLAPSE, 'Collapse', 'Collapse tree')

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
