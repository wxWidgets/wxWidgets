# Name:         undo.py
# Purpose:      XRC editor, undo/redo module
# Author:       Roman Rolinsky <rolinsky@mema.ucl.ac.be>
# Created:      01.12.2002
# RCS-ID:       $Id$

from globals import *
#from panel import *

# Undo/redo classes
class UndoManager:
    # Undo/redo stacks
    undo = []
    redo = []
    def RegisterUndo(self, undoObj):
        self.undo.append(undoObj)
        for i in self.redo: i.destroy()
        self.redo = []
    def Undo(self):
        undoObj = self.undo.pop()
        undoObj.undo()
        self.redo.append(undoObj)
        g.frame.modified = True
        g.frame.SetStatusText('Undone')
    def Redo(self):
        undoObj = self.redo.pop()
        undoObj.redo()
        self.undo.append(undoObj)
        g.frame.modified = True
        g.frame.SetStatusText('Redone')
    def Clear(self):
        for i in self.undo: i.destroy()
        self.undo = []
        for i in self.redo: i.destroy()
        self.redo = []
    def CanUndo(self):
        return not not self.undo
    def CanRedo(self):
        return not not self.redo

class UndoCutDelete:
    def __init__(self, itemIndex, parent, elem):
        self.itemIndex = itemIndex
        self.parent = parent
        self.elem = elem
    def destroy(self):
        if self.elem: self.elem.unlink()
    def undo(self):
        item = g.tree.InsertNode(g.tree.ItemAtFullIndex(self.itemIndex[:-1]),
                                 self.parent, self.elem,
                                 g.tree.ItemAtFullIndex(self.itemIndex))
        # Scroll to show new item (!!! redundant?)
        g.tree.EnsureVisible(item)
        g.tree.SelectItem(item)
        self.elem = None
        # Update testWin if needed
        if g.testWin and g.tree.IsHighlatable(item):
            if g.conf.autoRefresh:
                g.tree.needUpdate = True
                g.tree.pendingHighLight = item
            else:
                g.tree.pendingHighLight = None
    def redo(self):
        item = g.tree.ItemAtFullIndex(self.itemIndex)
        # Delete testWin?
        if g.testWin:
            # If deleting top-level item, delete testWin
            if item == g.testWin.item:
                g.testWin.Destroy()
                g.testWin = None
            else:
                # Remove highlight, update testWin
                if g.testWin.highLight:
                    g.testWin.highLight.Remove()
                g.tree.needUpdate = True
        self.elem = g.tree.RemoveLeaf(item)
        g.tree.Unselect()
        g.panel.Clear()

class UndoPasteCreate:
    def __init__(self, itemParent, parent, item, selected):
        self.itemParentIndex = g.tree.ItemFullIndex(itemParent)
        self.parent = parent
        self.itemIndex = g.tree.ItemFullIndex(item) # pasted item
        self.selectedIndex = g.tree.ItemFullIndex(selected) # maybe different from item
        self.elem = None
    def destroy(self):
        if self.elem: self.elem.unlink()
    def undo(self):
        self.elem = g.tree.RemoveLeaf(g.tree.ItemAtFullIndex(self.itemIndex))
        # Restore old selection
        selected = g.tree.ItemAtFullIndex(self.selectedIndex)
        g.tree.EnsureVisible(selected)
        g.tree.SelectItem(selected)
        # Delete testWin?
        if g.testWin:
            # If deleting top-level item, delete testWin
            if selected == g.testWin.item:
                g.testWin.Destroy()
                g.testWin = None
            else:
                # Remove highlight, update testWin
                if g.testWin.highLight:
                    g.testWin.highLight.Remove()
                g.tree.needUpdate = True
    def redo(self):
        item = g.tree.InsertNode(g.tree.ItemAtFullIndex(self.itemParentIndex),
                                 self.parent, self.elem,
                                 g.tree.ItemAtFullIndex(self.itemIndex))
        # Scroll to show new item
        g.tree.EnsureVisible(item)
        g.tree.SelectItem(item)
        self.elem = None
        # Update testWin if needed
        if g.testWin and g.tree.IsHighlatable(item):
            if g.conf.autoRefresh:
                g.tree.needUpdate = True
                g.tree.pendingHighLight = item
            else:
                g.tree.pendingHighLight = None

class UndoEdit:
    def __init__(self):
        self.pages = map(ParamPage.GetState, g.panel.pages)
        self.selectedIndex = g.tree.ItemFullIndex(g.tree.GetSelection())
    def destroy(self):
        pass
    # Update test view
    def update(self, selected):
        g.tree.Apply(g.tree.GetPyData(selected), selected)
        # Update view
        if g.testWin:
            if g.testWin.highLight:
                g.testWin.highLight.Remove()
            g.tree.pendingHighLight = selected
        if g.testWin:
            g.tree.needUpdate = True
    def undo(self):
        # Restore selection
        selected = g.tree.ItemAtFullIndex(self.selectedIndex)
        if selected != g.tree.GetSelection():
            g.tree.SelectItem(selected)
        # Save current state for redo
        map(ParamPage.SaveState, g.panel.pages)
        pages = map(ParamPage.GetState, g.panel.pages)
        map(ParamPage.SetState, g.panel.pages, self.pages)
        self.pages = pages
        self.update(selected)
    def redo(self):
        self.undo()
        self.update(g.tree.GetSelection())
