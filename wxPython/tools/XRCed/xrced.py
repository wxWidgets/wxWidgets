# Name:         xrced.py
# Purpose:      XRC editor, main module
# Author:       Roman Rolinsky <rolinsky@mema.ucl.ac.be>
# Created:      20.08.2001
# RCS-ID:       $Id$

from wxPython.wx import *
from wxPython.xrc import *
from wxPython.html import wxHtmlWindow
from xml.dom import minidom
import os
import getopt

# Icons
import images

# Constants

# Return code from wxGetOsVersion
wxGTK = 9

if wxGetOsVersion()[0] == wxGTK:
    labelFont = wxFont(12, wxDEFAULT, wxNORMAL, wxBOLD)
    modernFont = wxFont(12, wxMODERN, wxNORMAL, wxNORMAL)
else:
    labelFont = wxFont(10, wxDEFAULT, wxNORMAL, wxBOLD)
    modernFont = wxFont(10, wxMODERN, wxNORMAL, wxNORMAL)

progname = 'XRCed'
version = '0.0.7-3'

# Local modules
from xxx import *

# Globals
testWin = None
testWinPos = wxDefaultPosition

# 1 adds CMD command to Help menu
debug = 0

helpText = """\
<HTML><H2>Welcome to XRCed!</H2><H3><font color="green">DON'T PANIC :)</font></H3>
To start select tree root, then popup menu with your right mouse button,
select "Append Child", and then any command.<P>
Enter XML ID, change properties, create children.<P>
To test your interface select Test command (View menu).<P>
Consult README file for the details.</HTML>
"""

defaultIDs = {xxxPanel:'PANEL', xxxDialog:'DIALOG', xxxFrame:'FRAME',
              xxxMenuBar:'MENUBAR', xxxMenu:'MENU', xxxToolBar:'TOOLBAR'}

# Set menu to list items.
# Each menu command is a tuple (id, label, help)
# submenus are lists [id, label, help, submenu]
# and separators are any other type
def SetMenu(m, list):
    for l in list:
        if type(l) == types.TupleType:
            apply(m.Append, l)
        elif type(l) == types.ListType:
            subMenu = wxMenu()
            SetMenu(subMenu, l[2:])
            m.AppendMenu(wxNewId(), l[0], subMenu, l[1])
        else:                           # separator
            m.AppendSeparator()

################################################################################

# Properties panel containing notebook
class Panel(wxNotebook):
    def __init__(self, parent, id = -1):
        wxNotebook.__init__(self, parent, id, style=wxNB_BOTTOM)
        sys.modules['params'].panel = self
        # List of child windows
        self.pages = []
        # Create scrolled windows for pages
        self.page1 = wxScrolledWindow(self, -1)
        sizer = wxBoxSizer()
        sizer.Add(wxBoxSizer())         # dummy sizer
        self.page1.SetAutoLayout(true)
        self.page1.SetSizer(sizer)
        self.AddPage(self.page1, 'Properties')
        # Second page
        self.page2 = wxScrolledWindow(self, -1)
        sizer = wxBoxSizer()
        sizer.Add(wxBoxSizer())         # dummy sizer
        self.page2.SetAutoLayout(true)
        self.page2.SetSizer(sizer)
        # Cache for already used panels
        self.pageCache = {}             # cached property panels
        self.stylePageCache = {}        # cached style panels
        # Dummy parent window for cache pages
        self.cacheParent = wxFrame(None, -1, 'non visible')
    # Delete child windows and recreate page sizer
    def ResetPage(self, page):
        topSizer = page.GetSizer()
        sizer = topSizer.GetChildren()[0].GetSizer()
        for w in page.GetChildren():
            sizer.RemoveWindow(w)
            if isinstance(w, ParamPage):
                # With SetParent, we wouldn't need this
                w.Reparent(self.cacheParent)
            else:
                w.Destroy()
        topSizer.RemoveSizer(sizer)
        # Create new windows
        sizer = wxBoxSizer(wxVERTICAL)
        # Special case - resize html window
        if conf.panic:
            topSizer.Add(sizer, 1, wxEXPAND)
        else:
            topSizer.Add(sizer, 0, wxALL, 5)
        return sizer
    def SetData(self, xxx):
        self.pages = []
        # First page
        # Set cached or new page
        # Remove current objects and sizer
        sizer = self.ResetPage(self.page1)
        if not xxx or (not xxx.allParams and not xxx.hasName):
            if tree.selection:
                sizer.Add(wxStaticText(self.page1, -1, 'This item has no properties.'))
            else:                       # nothing selected
                # If first time, show some help
                if conf.panic:
                    html = wxHtmlWindow(self.page1, -1, wxDefaultPosition,
                                        wxDefaultSize, wxSUNKEN_BORDER)
                    html.SetPage(helpText)
                    sizer.Add(html, 1, wxEXPAND)
                    conf.panic = false
                else:
                    sizer.Add(wxStaticText(self.page1, -1, 'Select a tree item.'))
        else:
            SetCurrentXXX(xxx.treeObject())
            try:
                page = self.pageCache[xxx.__class__]
                page.Reparent(self.page1)
            except KeyError:
                page = PropPage(self.page1, xxx.className, xxx)
                self.pageCache[xxx.__class__] = page
            page.SetValues(xxx)
            self.pages.append(page)
            sizer.Add(page, 1, wxEXPAND)
            if xxx.hasChild:
                # Special label for child objects - they may have different GUI
                cacheID = (xxx.child.__class__, xxx.__class__)
                try:
                    page = self.pageCache[cacheID]
                    page.Reparent(self.page1)
                except KeyError:
                    page = PropPage(self.page1, xxx.child.className, xxx.child)
                    self.pageCache[cacheID] = page
                page.SetValues(xxx.child)
                self.pages.append(page)
                sizer.Add(page, 0, wxEXPAND | wxTOP, 5)
        self.page1.Layout()
        size = self.page1.GetSizer().GetMinSize()
        self.page1.SetScrollbars(1, 1, size.x, size.y, 0, 0, true)

        # Second page
        # Create if does not exist
        if xxx and xxx.treeObject().hasStyle:
            xxx = xxx.treeObject()
            # Simplest case: set data if class is the same
            sizer = self.ResetPage(self.page2)
            try:
                page = self.stylePageCache[xxx.__class__]
                page.Reparent(self.page2)
            except KeyError:
                page = StylePage(self.page2, xxx.className + ' style', xxx)
                self.stylePageCache[xxx.__class__] = page
            page.SetValues(xxx)
            self.pages.append(page)
            sizer.Add(page, 0, wxEXPAND)
            # Add page if not exists
            if not self.GetPageCount() == 2:
                self.AddPage(self.page2, 'Style')
            self.page2.Layout()
            size = self.page2.GetSizer().GetMinSize()
            self.page2.SetScrollbars(1, 1, size.x, size.y, 0, 0, true)
        else:
            # Remove page if exists
            if self.GetPageCount() == 2:
                self.SetSelection(0)
                self.page1.Refresh()
                self.RemovePage(1)
    def Clear(self):
        self.SetData(None)
    # Check if some parameter on some page has changed
    def IsModified(self):
        for p in self.pages:
            if p.IsModified(): return true
        return false
    # Reset changed state
    def SetModified(self, value):
        for p in self.pages: p.SetModified(value)
    def Apply(self):
        for p in self.pages: p.Apply()

################################################################################

# General class for notebook pages
class ParamPage(wxPanel):
    def __init__(self, parent, xxx):
        wxPanel.__init__(self, parent, -1)
        self.xxx = xxx
        # Register event handlers
        for id in paramIDs.values():
            EVT_CHECKBOX(self, id, self.OnCheckParams)
        self.modified = false
        self.checks = {}
        self.controls = {}              # save python objects
        self.controlName = None
    def OnCheckParams(self, evt):
        xxx = self.xxx
        param = evt.GetEventObject().GetName()
        w = self.controls[param]
        objElem = xxx.element
        if evt.IsChecked():
            # Ad  new text node in order of allParams
            w.SetValue('')              # set empty (default) value
            w.SetModified()             # mark as changed
            elem = tree.dom.createElement(param)
            # Some classes are special
            if param == 'font':
                xxx.params[param] = xxxParamFont(xxx.element, elem)
            else:
                xxx.params[param] = xxxParam(elem)
            # Find place to put new element: first present element after param
            found = false
            paramStyles = xxx.allParams + xxx.styles
            for p in paramStyles[paramStyles.index(param) + 1:]:
                # Content params don't have same type
                if xxx.params.has_key(p) and p != 'content':
                    found = true
                    break
            if found:
                nextTextElem = xxx.params[p].node
                objElem.insertBefore(elem, nextTextElem)
            else:
                objElem.appendChild(elem)
        else:
            # Remove parameter
            xxx.params[param].remove()
            del xxx.params[param]
            w.SetValue('')
            w.modified = false          # mark as not changed
            # Set modified flas
            self.SetModified(true)
        w.Enable(evt.IsChecked())
    # If some parameter has changed
    def IsModified(self):
        return self.modified
    def SetModified(self, value):
        self.modified = value
    def Apply(self):
        xxx = self.xxx
        # !!! Save undo info
#        if xxx.undo: xxx.undo.unlink()
#        xxx.undo = xxx.element.cloneNode(false)
        if self.controlName:
            name = self.controlName.GetValue()
            if xxx.name != name:
                xxx.name = name
                xxx.element.setAttribute('name', name)
        for param, w in self.controls.items():
            if w.modified:
                paramObj = xxx.params[param]
                value = w.GetValue()
                if param in xxx.specials:
                    xxx.setSpecial(param, value)
                else:
                    paramObj.update(value)

################################################################################

# Panel for displaying properties
class PropPage(ParamPage):
    def __init__(self, parent, label, xxx):
        ParamPage.__init__(self, parent, xxx)
        box = wxStaticBox(self, -1, label)
        box.SetFont(labelFont)
        topSizer = wxStaticBoxSizer(box, wxVERTICAL)
        sizer = wxFlexGridSizer(len(xxx.allParams), 2, 1, 1)
        if xxx.hasName:
            label = wxStaticText(self, -1, 'XML ID:', size=(100,-1))
            control = ParamText(self, name='XML_name')
            sizer.AddMany([ (label, 0, wxALIGN_CENTER_VERTICAL),
                            (control, 0, wxALIGN_CENTER_VERTICAL) ])
            self.controlName = control
        for param in xxx.allParams:
            present = param in xxx.params
            if param in xxx.required:
                label = wxStaticText(self, paramIDs[param], param + ':',
                                     size = (100,-1), name = param)
            else:
                # Notebook has one very loooooong parameter
                if param == 'usenotebooksizer': sParam = 'usesizer:'
                else: sParam = param + ':'
                label = wxCheckBox(self, paramIDs[param], sParam,
                                   size = (100,-1), name = param)
                self.checks[param] = label
            try:
                typeClass = xxx.paramDict[param]
            except KeyError:
                try:
                    # Standart type
                    typeClass = paramDict[param]
                except KeyError:
                    # Default
                    typeClass = ParamText
            control = typeClass(self, param)
            control.Enable(present)
            sizer.AddMany([ (label, 0, wxALIGN_CENTER_VERTICAL),
                            (control, 0, wxALIGN_CENTER_VERTICAL) ])
            self.controls[param] = control
        topSizer.Add(sizer, 1, wxALL | wxEXPAND, 3)
        self.SetAutoLayout(true)
        self.SetSizer(topSizer)
        topSizer.Fit(self)
    def SetValues(self, xxx):
        self.xxx = xxx
        # Set values, checkboxes to false, disable defaults
        if xxx.hasName:
            self.controlName.SetValue(xxx.name)
        for param in xxx.allParams:
            w = self.controls[param]
            w.modified = false
            try:
                value = xxx.params[param].value()
                w.Enable(true)
                w.SetValue(value)
                if not param in xxx.required:
                    self.checks[param].SetValue(true)
            except KeyError:
                self.checks[param].SetValue(false)
                w.SetValue('')
                w.Enable(false)
        self.SetModified(false)

################################################################################

# Style notebook page
class StylePage(ParamPage):
    def __init__(self, parent, label, xxx):
        ParamPage.__init__(self, parent, xxx)
        box = wxStaticBox(self, -1, label)
        box.SetFont(labelFont)
        topSizer = wxStaticBoxSizer(box, wxVERTICAL)
        sizer = wxFlexGridSizer(len(xxx.styles), 2, 1, 1)
        for param in xxx.styles:
            present = param in xxx.params.keys()
            check = wxCheckBox(self, paramIDs[param],
                               param + ':', size = (100,-1), name = param)
            check.SetValue(present)
            control = paramDict[param](self, name = param)
            control.Enable(present)
            sizer.AddMany([ (check, 0, wxALIGN_CENTER_VERTICAL),
                            (control, 0, wxALIGN_CENTER_VERTICAL) ])
            self.checks[param] = check
            self.controls[param] = control
        topSizer.Add(sizer, 1, wxALL | wxEXPAND, 3)
        self.SetAutoLayout(true)
        self.SetSizer(topSizer)
        topSizer.Fit(self)
    # Set data for a cahced page
    def SetValues(self, xxx):
        self.xxx = xxx
        for param in xxx.styles:
            present = param in xxx.params.keys()
            check = self.checks[param]
            check.SetValue(present)
            w = self.controls[param]
            w.modified = false
            if present:
                w.SetValue(xxx.params[param].value())
            else:
                w.SetValue('')
            w.Enable(present)
        self.SetModified(false)

################################################################################

class HightLightBox:
    def __init__(self, pos, size):
        w = testWin.panel
        l1 = wxWindow(w, -1, pos, wxSize(size.x, 2))
        l1.SetBackgroundColour(wxRED)
        l2 = wxWindow(w, -1, pos, wxSize(2, size.y))
        l2.SetBackgroundColour(wxRED)
        l3 = wxWindow(w, -1, wxPoint(pos.x + size.x - 2, pos.y), wxSize(2, size.y))
        l3.SetBackgroundColour(wxRED)
        l4 = wxWindow(w, -1, wxPoint(pos.x, pos.y + size.y - 2), wxSize(size.x, 2))
        l4.SetBackgroundColour(wxRED)
        self.lines = [l1, l2, l3, l4]
    # Move highlight to a new position
    def Replace(self, pos, size):
        self.lines[0].SetDimensions(pos.x, pos.y, size.x, 2, wxSIZE_ALLOW_MINUS_ONE)
        self.lines[1].SetDimensions(pos.x, pos.y, 2, size.y, wxSIZE_ALLOW_MINUS_ONE)
        self.lines[2].SetDimensions(pos.x + size.x - 2, pos.y, 2, size.y,
                                    wxSIZE_ALLOW_MINUS_ONE)
        self.lines[3].SetDimensions(pos.x, pos.y + size.y - 2, size.x, 2,
                                    wxSIZE_ALLOW_MINUS_ONE)
    # Remove it
    def Remove(self):
        map(wxWindow.Destroy, self.lines)
        testWin.highLight = None

################################################################################

class MemoryFile:
    def __init__(self, name):
        self.name = name
        self.buffer = ''
    def write(self, data):
        self.buffer += data.encode()
    def close(self):
        wxMemoryFSHandler_AddFile(self.name, self.buffer)

class XML_Tree(wxTreeCtrl):
    def __init__(self, parent, id):
        wxTreeCtrl.__init__(self, parent, id, style = wxTR_HAS_BUTTONS)
        self.SetBackgroundColour(wxColour(224, 248, 224))
        EVT_TREE_SEL_CHANGED(self, self.GetId(), self.OnSelChanged)
        # One works on Linux, another on Windows
        if wxGetOsVersion()[0] == wxGTK:
            EVT_TREE_ITEM_ACTIVATED(self, self.GetId(), self.OnItemActivated)
        else:
            EVT_LEFT_DCLICK(self, self.OnDClick)
        EVT_RIGHT_DOWN(self, self.OnRightDown)

        self.needUpdate = false
        self.pendingHighLight = None
        self.ctrl = self.shift = false
        self.dom = None
        # Create image list
        il = wxImageList(16, 16, true)
        self.rootImage = il.AddIcon(wxIconFromXPMData(images.getTreeRootData()))
        xxxObject.image = il.AddIcon(wxIconFromXPMData(images.getTreeDefaultData()))
        xxxPanel.image = il.AddIcon(wxIconFromXPMData(images.getTreePanelData()))
        xxxDialog.image = il.AddIcon(wxIconFromXPMData(images.getTreeDialogData()))
        xxxFrame.image = il.AddIcon(wxIconFromXPMData(images.getTreeFrameData()))
        xxxMenuBar.image = il.AddIcon(wxIconFromXPMData(images.getTreeMenuBarData()))
        xxxToolBar.image = il.AddIcon(wxIconFromXPMData(images.getTreeToolBarData()))
        xxxMenu.image = il.AddIcon(wxIconFromXPMData(images.getTreeMenuData()))
        xxxSizer.imageH = il.AddIcon(wxIconFromXPMData(images.getTreeSizerHData()))
        xxxSizer.imageV = il.AddIcon(wxIconFromXPMData(images.getTreeSizerVData()))
        xxxStaticBoxSizer.imageH = il.AddIcon(wxIconFromXPMData(images.getTreeStaticBoxSizerHData()))
        xxxStaticBoxSizer.imageV = il.AddIcon(wxIconFromXPMData(images.getTreeStaticBoxSizerVData()))
        xxxGridSizer.image = il.AddIcon(wxIconFromXPMData(images.getTreeSizerGridData()))
        xxxFlexGridSizer.image = il.AddIcon(wxIconFromXPMData(images.getTreeSizerFlexGridData()))
        self.il = il
        self.SetImageList(il)

    def Unselect(self):
        self.selection = None
        wxTreeCtrl.Unselect(self)

    def ExpandAll(self, item):
        if self.ItemHasChildren(item):
            self.Expand(item)
            i, cookie = self.GetFirstChild(item, 0)
            children = []
            while i.IsOk():
                children.append(i)
                i, cookie = self.GetNextChild(item, cookie)
            for i in children:
                self.ExpandAll(i)
    def CollapseAll(self, item):
        if self.ItemHasChildren(item):
            i, cookie = self.GetFirstChild(item, 0)
            children = []
            while i.IsOk():
                children.append(i)
                i, cookie = self.GetNextChild(item, cookie)
            for i in children:
                self.CollapseAll(i)
            self.Collapse(item)

    # Clear tree
    def Clear(self):
        self.DeleteAllItems()
        # Add minimal structure
        if self.dom: self.dom.unlink()
        self.dom = minidom.Document()
        self.dummyNode = self.dom.createComment('dummy node')
        # Create main node
        self.mainNode = self.dom.createElement('resource')
        self.dom.appendChild(self.mainNode)
        xxx = xxxMainNode(None, self.mainNode)
        self.root = self.AddRoot('XML tree', self.rootImage, data=wxTreeItemData(xxx))
        self.SetItemHasChildren(self.root)
        self.Expand(self.root)
        self.Unselect()

    # Clear old data and set new
    def SetData(self, dom):
        self.DeleteAllItems()
        # Add minimal structure
        if self.dom: self.dom.unlink()
        self.dom = dom
        self.dummyNode = self.dom.createComment('dummy node')
        # Find 'resource' child, add it's children
        self.mainNode = dom.getElementsByTagName('resource')[0]
        xxx = xxxMainNode(None, self.mainNode)
        self.root = self.AddRoot('XML tree', self.rootImage, data=wxTreeItemData(xxx))
        self.SetItemHasChildren(self.root)
        nodes = self.mainNode.childNodes[:]
        for node in nodes:
            if IsObject(node):
                self.AddNode(self.root, None, node)
            else:
                self.mainNode.removeChild(node)
                node.unlink()
        self.Expand(self.root)
        self.Unselect()

    # Add tree item for given parent item if node is DOM element node with
    # 'object' tag. xxxParent is parent xxx object
    def AddNode(self, itemParent, xxxParent, node):
        # Set item data to current node
        try:
            xxx = MakeXXXFromDOM(xxxParent, node)
        except:
            print 'ERROR: MakeXXXFromDom(%s, %s)' % (xxxParent, node)
            raise
        treeObj = xxx.treeObject()
        # Append tree item
        item = self.AppendItem(itemParent, treeObj.treeName(),
                               image=treeObj.treeImage(),
                               data=wxTreeItemData(xxx))
        # Try to find children objects
        if treeObj.hasChildren:
            nodes = treeObj.element.childNodes[:]
            for n in nodes:
                if IsObject(n):
                    self.AddNode(item, treeObj, n)
                elif n.nodeType != minidom.Node.ELEMENT_NODE:
                    treeObj.element.removeChild(n)
                    n.unlink()
    # Remove leaf of tree, return it's data object
    def RemoveLeaf(self, leaf):
        xxx = self.GetPyData(leaf)
        node = xxx.element
        parent = node.parentNode
        parent.removeChild(node)
        self.Delete(leaf)
        # Reset selection object
        self.selection = None
        return node
    # Find position relative to the top-level window
    def FindNodePos(self, item):
        # Root at (0,0)
        if item == testWin.item: return wxPoint(0, 0)
        itemParent = self.GetItemParent(item)
        # Select NB page
        obj = self.FindNodeObject(item)
        if self.GetPyData(itemParent).treeObject().__class__ == xxxNotebook:
            notebook = self.FindNodeObject(itemParent)
            # Find position
            for i in range(notebook.GetPageCount()):
                if notebook.GetPage(i) == obj:
                    if notebook.GetSelection() != i: notebook.SetSelection(i)
                    break
        # Find first ancestor which is a wxWindow (not a sizer)
        winParent = itemParent
        while self.GetPyData(winParent).isSizer:
            winParent = self.GetItemParent(winParent)
        parentPos = self.FindNodePos(winParent)
        # Position (-1,-1) is really (0,0)
        pos = obj.GetPosition()
        if pos == (-1,-1): pos = (0,0)
        return parentPos + pos
    # Find window (or sizer) corresponding to a tree item.
    def FindNodeObject(self, item):
        if item == testWin.item: return testWin.panel
        itemParent = self.GetItemParent(item)
        # If top-level, return testWin (or panel if wxFrame)
        xxx = self.GetPyData(item).treeObject()
        parentWin = self.FindNodeObject(itemParent)
        # Top-level sizer? return window's sizer
        if xxx.isSizer and isinstance(parentWin, wxWindowPtr):
            return parentWin.GetSizer()
        # Otherwise get parent's object and it's child
        n = 0                           # index of sibling
        prev = self.GetPrevSibling(item)
        while prev.IsOk():
            prev = self.GetPrevSibling(prev)
            n += 1
        child = parentWin.GetChildren()[n]
        # Return window or sizer for sizer items
        if child.GetClassName() == 'wxSizerItem':
            if child.IsWindow(): child = child.GetWindow()
            elif child.IsSizer():
                child = child.GetSizer()
                # Test for notebook sizers
                if isinstance(child, wxNotebookSizerPtr):
                    child = child.GetNotebook()
        return child
    def OnSelChanged(self, evt):
        # Apply changes
        # !!! problem with wxGTK - GetOldItem is Ok if nothing selected
        #oldItem = evt.GetOldItem()
        status = ''
        oldItem = self.selection
        if oldItem:
            xxx = self.GetPyData(oldItem)
            # If some data was modified, apply changes
            if panel.IsModified():
                self.Apply(xxx, oldItem)
                #if conf.autoRefresh:
                if testWin:
                    if testWin.highLight and not tree.IsHighlatable(oldItem):
                        testWin.highLight.Remove()
                    self.needUpdate = true
                status = 'Changes were applied'
        frame.SetStatusText(status)
        # Generate view
        self.selection = evt.GetItem()
        if not self.selection.IsOk():
            self.selection = None
            return
        xxx = self.GetPyData(self.selection)
        # Update panel
        panel.SetData(xxx)
        # Clear flag
        panel.SetModified(false)
        # Hightlighting is done in OnIdle
        tree.pendingHighLight = self.selection
    # Check if item is in testWin subtree
    def IsHighlatable(self, item):
        if item == testWin.item: return false
        while item != self.root:
            item = self.GetItemParent(item)
            if item == testWin.item: return true
        return false
    # Highlight selected item
    def HighLight(self, item):
        self.pendingHighLight = None
        if not testWin or self.GetPyData(testWin.item).className \
            not in ['wxDialog', 'wxPanel', 'wxFrame']:
            return
        # Top-level does not have highlight
        if item == testWin.item or item == tree.root:
            if testWin.highLight: testWin.highLight.Remove()
            return
        # If a control from another window is selected, remove highlight
        if not self.IsHighlatable(item):
            if testWin.highLight: testWin.highLight.Remove()
            return
        # Get window/sizer object
        obj, pos = self.FindNodeObject(item), self.FindNodePos(item)
        size = obj.GetSize()
        # Highlight
        # Nagative positions are not working wuite well
        if testWin.highLight:
            testWin.highLight.Replace(pos, size)
        else:
            testWin.highLight = HightLightBox(pos, size)
        testWin.highLight.item = item
    def ShowTestWindow(self, item):
        global testWin
        xxx = self.GetPyData(item)
        if panel.IsModified():
            self.Apply(xxx, item)       # apply changes
        treeObj = xxx.treeObject()
        if treeObj.className not in ['wxFrame', 'wxPanel', 'wxDialog',
                                     'wxMenuBar', 'wxToolBar']:
            wxLogMessage('No view for this element (yet)')
            return
        if not treeObj.name:
            wxLogError("Can't display a noname element!")
            return
        # Show item in bold
        if testWin:
            self.SetItemBold(testWin.item, false)
        self.SetItemBold(item)
        self.CreateTestWin(item)
    # Double-click on Linux
    def OnItemActivated(self, evt):
        if evt.GetItem() != self.root:
            self.ShowTestWindow(evt.GetItem())
    # Double-click on Windows
    def OnDClick(self, evt):
        item, flags = self.HitTest(evt.GetPosition())
        if flags in [wxTREE_HITTEST_ONITEMBUTTON, wxTREE_HITTEST_ONITEMLABEL]:
            if item != self.root: self.ShowTestWindow(item)
        else:
            evt.Skip()
    # (re)create test window
    def CreateTestWin(self, item):
        global testWin
        wxBeginBusyCursor()
        # Create a window with this resource
        xxx = self.GetPyData(item).treeObject()
        # Close old window, remember where it was
        highLight = None
        if testWin:
            pos = testWin.GetPosition()
            if item == testWin.item:
                # Remember highlight if same top-level window
                if testWin.highLight:
                    highLight = testWin.highLight.item
                # !!! if 0 is removed, refresh is broken (notebook not deleted?)
                if xxx.className == 'wxPanel':
                    if testWin.highLight:
                        testWin.pendingHighLight = highLight
                        testWin.highLight.Remove()
                    testWin.panel.Destroy()
                    testWin.panel = None
                else:
                    testWin.Destroy()
                    testWin = None
            else:
                testWin.Destroy()
                testWin = None
        else:
            pos = testWinPos
        # Save in memory FS
        memFile = MemoryFile('xxx.xrc')
        # Create partial XML file - faster for big files

        dom = minidom.Document()
        mainNode = dom.createElement('resource')
        dom.appendChild(mainNode)

        # Remove temporarily from old parent
        elem = xxx.element
        parent = elem.parentNode
        next = elem.nextSibling
        parent.replaceChild(self.dummyNode, elem)
        # Append to new DOM, write it
        mainNode.appendChild(elem)
        dom.writexml(memFile)
        # Put back in place
        mainNode.removeChild(elem)
        dom.unlink()
        parent.replaceChild(elem, self.dummyNode)
        memFile.close()                 # write to wxMemoryFS
        res = wxXmlResource('')
        res.Load('memory:xxx.xrc')
        if xxx.className == 'wxFrame':
            # Create new frame
            testWin = wxPreFrame()
            res.LoadFrame(testWin, frame, xxx.name)
            # Create status bar
            testWin.CreateStatusBar()
            testWin.panel = testWin
            testWin.SetPosition(pos)
            testWin.Show(true)
        elif xxx.className == 'wxPanel':
            # Create new frame
            if not testWin:
                testWin = wxFrame(frame, -1, 'Panel: ' + xxx.name, pos=pos)
            testWin.panel = res.LoadPanel(testWin, xxx.name)
            testWin.SetClientSize(testWin.panel.GetSize())
            testWin.Show(true)
        elif xxx.className == 'wxDialog':
            # Create new frame
            testWin = res.LoadDialog(None, xxx.name)
            testWin.panel = testWin
            testWin.Layout()
            testWin.SetPosition(pos)
            testWin.Show(true)
        elif xxx.className == 'wxMenuBar':
            testWin = wxFrame(frame, -1, 'MenuBar: ' + xxx.name, pos=pos)
            testWin.panel = None
            # Set status bar to display help
            testWin.CreateStatusBar()
            testWin.menuBar = res.LoadMenuBar(xxx.name)
            testWin.SetMenuBar(testWin.menuBar)
            testWin.Show(true)
        elif xxx.className == 'wxToolBar':
            testWin = wxFrame(frame, -1, 'ToolBar: ' + xxx.name, pos=pos)
            testWin.panel = None
            # Set status bar to display help
            testWin.CreateStatusBar()
            testWin.toolBar = res.LoadToolBar(testWin, xxx.name)
            testWin.SetToolBar(testWin.toolBar)
            testWin.Show(true)
        wxMemoryFSHandler_RemoveFile('xxx.xrc')
        testWin.item = item
        EVT_CLOSE(testWin, self.OnCloseTestWin)
        EVT_BUTTON(testWin, wxID_OK, self.OnCloseTestWin)
        EVT_BUTTON(testWin, wxID_CANCEL, self.OnCloseTestWin)
        testWin.highLight = None
        if highLight and not tree.pendingHighLight:
            self.HighLight(highLight)
        wxEndBusyCursor()

    def OnCloseTestWin(self, evt):
        global testWin, testWinPos
        self.SetItemBold(testWin.item, false)
        testWinPos = testWin.GetPosition()
        testWin.Destroy()
        testWin = None

    # Return item index in parent
    def ItemIndex(self, parent, item):
        i = 0
        it, cookie = self.GetFirstChild(parent, 0)
        while it != item:
            i += 1
            it, cookie = self.GetNextChild(parent, cookie)
        return i

    # True if next item should be inserted after current (vs. appended to it)
    def NeedInsert(self, item):
        xxx = self.GetPyData(item)
        if item == self.root: return false        # root item
        if xxx.hasChildren and not self.GetChildrenCount(item, false):
            return false
        return not (self.IsExpanded(item) and self.GetChildrenCount(item, false))

    # Pull-down
    def OnRightDown(self, evt):
        # select this item
        pt = evt.GetPosition();
        item, flags = self.HitTest(pt)
        if item.Ok() and flags & wxTREE_HITTEST_ONITEM:
            self.SelectItem(item)

        # Setup menu
        menu = wxMenu()

        item = self.selection
        if not item:
            menu.Append(pullDownMenu.ID_EXPAND, 'Expand', 'Expand tree')
            menu.Append(pullDownMenu.ID_COLLAPSE, 'Collapse', 'Collapse tree')
        else:
            self.ctrl = evt.ControlDown() # save Ctrl state
            self.shift = evt.ShiftDown()  # and Shift too
            m = wxMenu()                  # create menu
            if self.ctrl:
                needInsert = true
            else:
                needInsert = self.NeedInsert(item)
            if item == self.root or needInsert and self.GetItemParent(item) == self.root:
                m.Append(pullDownMenu.ID_NEW_PANEL, 'Panel', 'Create panel')
                m.Append(pullDownMenu.ID_NEW_DIALOG, 'Dialog', 'Create dialog')
                m.Append(pullDownMenu.ID_NEW_FRAME, 'Frame', 'Create frame')
                m.AppendSeparator()
                m.Append(pullDownMenu.ID_NEW_TOOL_BAR, 'ToolBar', 'Create toolbar')
                m.Append(pullDownMenu.ID_NEW_MENU_BAR, 'MenuBar', 'Create menubar')
                m.Append(pullDownMenu.ID_NEW_MENU, 'Menu', 'Create menu')
            else:
                xxx = self.GetPyData(item).treeObject()
                # Check parent for possible child nodes if inserting sibling
                if needInsert: xxx = xxx.parent
                if xxx.__class__ == xxxMenuBar:
                    m.Append(pullDownMenu.ID_NEW_MENU, 'Menu', 'Create menu')
                elif xxx.__class__ in [xxxToolBar, xxxTool] or \
                     xxx.__class__ == xxxSeparator and xxx.parent.__class__ == xxxToolBar:
                    SetMenu(m, pullDownMenu.toolBarControls)
                elif xxx.__class__ in [xxxMenu, xxxMenuItem]:
                    SetMenu(m, pullDownMenu.menuControls)
                else:
                    SetMenu(m, pullDownMenu.controls)
                    if xxx.__class__ == xxxNotebook:
                        m.Enable(m.FindItem('sizer'), false)
                    elif not (xxx.isSizer or xxx.parent and xxx.parent.isSizer):
                        m.Enable(pullDownMenu.ID_NEW_SPACER, false)
            # Select correct label for create menu
            if not needInsert:
                if self.shift:
                    menu.AppendMenu(wxNewId(), 'Insert Child', m,
                                    'Create child object as the first child')
                else:
                    menu.AppendMenu(wxNewId(), 'Append Child', m,
                                    'Create child object as the last child')
            else:
                if self.shift:
                    menu.AppendMenu(wxNewId(), 'Create Sibling', m,
                                    'Create sibling before selected object')
                else:
                    menu.AppendMenu(wxNewId(), 'Create Sibling', m,
                                    'Create sibling after selected object')
            menu.AppendSeparator()
            # Not using standart IDs because we don't want to show shortcuts
            menu.Append(wxID_CUT, 'Cut', 'Cut to the clipboard')
            menu.Append(wxID_COPY, 'Copy', 'Copy to the clipboard')
            if self.ctrl and item != tree.root:
                menu.Append(pullDownMenu.ID_PASTE_SIBLING, 'Paste Sibling',
                            'Paste from the clipboard as a sibling')
            else:
                menu.Append(wxID_PASTE, 'Paste', 'Paste from the clipboard')
            menu.Append(pullDownMenu.ID_DELETE,
                                'Delete', 'Delete object')
            if self.ItemHasChildren(item):
                menu.AppendSeparator()
                menu.Append(pullDownMenu.ID_EXPAND, 'Expand', 'Expand subtree')
                menu.Append(pullDownMenu.ID_COLLAPSE, 'Collapse', 'Collapse subtree')
        self.PopupMenu(menu, evt.GetPosition())
        menu.Destroy()

    # Apply changes
    def Apply(self, xxx, item):
        panel.Apply()
        # Update tree view
        xxx = xxx.treeObject()
        if xxx.hasName and self.GetItemText(item) != xxx.name:
            self.SetItemText(item, xxx.treeName())
        # Change tree icon for sizers
        if isinstance(xxx, xxxBoxSizer):
            self.SetItemImage(item, xxx.treeImage())
        # Set global modified state
        frame.modified = true

class PullDownMenu:
    ID_NEW_PANEL = wxNewId()
    ID_NEW_DIALOG = wxNewId()
    ID_NEW_FRAME = wxNewId()
    ID_NEW_TOOL_BAR = wxNewId()
    ID_NEW_TOOL = wxNewId()
    ID_NEW_MENU_BAR = wxNewId()
    ID_NEW_MENU = wxNewId()

    ID_NEW_STATIC_TEXT = wxNewId()
    ID_NEW_TEXT_CTRL = wxNewId()

    ID_NEW_BUTTON = wxNewId()
    ID_NEW_BITMAP_BUTTON = wxNewId()
    ID_NEW_RADIO_BUTTON = wxNewId()
    ID_NEW_SPIN_BUTTON = wxNewId()

    ID_NEW_STATIC_BOX = wxNewId()
    ID_NEW_CHECK_BOX = wxNewId()
    ID_NEW_RADIO_BOX = wxNewId()
    ID_NEW_COMBO_BOX = wxNewId()
    ID_NEW_LIST_BOX = wxNewId()

    ID_NEW_STATIC_LINE = wxNewId()
    ID_NEW_STATIC_BITMAP = wxNewId()
    ID_NEW_CHOICE = wxNewId()
    ID_NEW_SLIDER = wxNewId()
    ID_NEW_GAUGE = wxNewId()
    ID_NEW_SCROLL_BAR = wxNewId()
    ID_NEW_TREE_CTRL = wxNewId()
    ID_NEW_LIST_CTRL = wxNewId()
    ID_NEW_CHECK_LIST = wxNewId()
    ID_NEW_NOTEBOOK = wxNewId()
    ID_NEW_HTML_WINDOW = wxNewId()
    ID_NEW_CALENDAR = wxNewId()

    ID_NEW_BOX_SIZER = wxNewId()
    ID_NEW_STATIC_BOX_SIZER = wxNewId()
    ID_NEW_GRID_SIZER = wxNewId()
    ID_NEW_FLEX_GRID_SIZER = wxNewId()
    ID_NEW_SPACER = wxNewId()
    ID_NEW_TOOL_BAR = wxNewId()
    ID_NEW_TOOL = wxNewId()
    ID_NEW_MENU = wxNewId()
    ID_NEW_MENU_ITEM = wxNewId()
    ID_NEW_SEPARATOR = wxNewId()
    ID_NEW_LAST = wxNewId()
    ID_EXPAND = wxNewId()
    ID_COLLAPSE = wxNewId()
    ID_PASTE_SIBLING = wxNewId()

    def __init__(self, parent):
        self.ID_DELETE = parent.ID_DELETE
        EVT_MENU_RANGE(parent, self.ID_NEW_PANEL,
                       self.ID_NEW_LAST, parent.OnCreate)
        EVT_MENU(parent, self.ID_COLLAPSE, parent.OnCollapse)
        EVT_MENU(parent, self.ID_EXPAND, parent.OnExpand)
        EVT_MENU(parent, self.ID_PASTE_SIBLING, parent.OnPaste)
        # We connect to tree, but process in frame
        EVT_MENU_HIGHLIGHT_ALL(tree, parent.OnPullDownHighlight)

################################################################################

# ScrolledMessageDialog - modified from wxPython lib to set fixed-width font
class ScrolledMessageDialog(wxDialog):
    def __init__(self, parent, msg, caption, pos = wxDefaultPosition, size = (500,300)):
        from wxPython.lib.layoutf import Layoutf
        wxDialog.__init__(self, parent, -1, caption, pos, size)
        text = wxTextCtrl(self, -1, msg, wxDefaultPosition,
                             wxDefaultSize, wxTE_MULTILINE | wxTE_READONLY)
        text.SetFont(modernFont)
        dc = wxWindowDC(text)
        w, h = dc.GetTextExtent(' ')
        ok = wxButton(self, wxID_OK, "OK")
        text.SetConstraints(Layoutf('t=t5#1;b=t5#2;l=l5#1;r=r5#1', (self,ok)))
        text.SetSize((w * 80 + 30, h * 40))
        ok.SetConstraints(Layoutf('b=b5#1;x%w50#1;w!80;h!25', (self,)))
        self.SetAutoLayout(TRUE)
        self.Fit()
        self.CenterOnScreen(wxBOTH)

################################################################################

class Frame(wxFrame):
    def __init__(self, pos, size):
        global frame
        frame = self
        wxFrame.__init__(self, None, -1, '', pos, size)
        self.CreateStatusBar()
        icon = wxIcon(os.path.join(sys.path[0], 'xrced.ico'), wxBITMAP_TYPE_ICO)
        self.SetIcon(icon)

        # Idle flag
        self.inIdle = false

        # Make menus
        menuBar = wxMenuBar()

        menu = wxMenu()
        menu.Append(wxID_NEW, '&New\tCtrl-N', 'New file')
        menu.Append(wxID_OPEN, '&Open...\tCtrl-O', 'Open XRC file')
        menu.Append(wxID_SAVE, '&Save\tCtrl-S', 'Save XRC file')
        menu.Append(wxID_SAVEAS, 'Save &As...', 'Save XRC file under different name')
        menu.AppendSeparator()
        menu.Append(wxID_EXIT, '&Quit\tCtrl-Q', 'Exit application')
        menuBar.Append(menu, '&File')

        menu = wxMenu()
        menu.Append(wxID_UNDO, '&Undo\tCtrl-Z', 'Undo')
        menu.Append(wxID_REDO, '&Redo\tCtrl-Y', 'Redo')
        menu.AppendSeparator()
        menu.Append(wxID_CUT, 'Cut\tCtrl-X', 'Cut to the clipboard')
        menu.Append(wxID_COPY, '&Copy\tCtrl-C', 'Copy to the clipboard')
        menu.Append(wxID_PASTE, '&Paste\tCtrl-V', 'Paste from the clipboard')
        self.ID_DELETE = wxNewId()
        menu.Append(self.ID_DELETE, '&Delete\tCtrl-D', 'Delete object')
        menuBar.Append(menu, '&Edit')

        menu = wxMenu()
        self.ID_EMBED_PANEL = wxNewId()
        menu.Append(self.ID_EMBED_PANEL, '&Embed Panel',
                    'Toggle embedding properties panel in the main window', true)
        menu.Check(self.ID_EMBED_PANEL, conf.embedPanel)
        menu.AppendSeparator()
        self.ID_TEST = wxNewId()
        menu.Append(self.ID_TEST, '&Test\tF5', 'Test window')
        self.ID_REFRESH = wxNewId()
        menu.Append(self.ID_REFRESH, '&Refresh\tCtrl-R', 'Refresh test window')
        self.ID_AUTO_REFRESH = wxNewId()
        menu.Append(self.ID_AUTO_REFRESH, '&Auto-refresh\tCtrl-A',
                    'Toggle auto-refresh mode', true)
        menu.Check(self.ID_AUTO_REFRESH, conf.autoRefresh)
        menuBar.Append(menu, '&View')

        menu = wxMenu()
        menu.Append(wxID_ABOUT, '&About...', 'About XCRed')
        self.ID_README = wxNewId()
        menu.Append(self.ID_README, '&Readme...', 'View the README file')
        if debug:
            self.ID_DEBUG_CMD = wxNewId()
            menu.Append(self.ID_DEBUG_CMD, 'CMD', 'Python command line')
            EVT_MENU(self, self.ID_DEBUG_CMD, self.OnDebugCMD)
        menuBar.Append(menu, '&Help')

        self.menuBar = menuBar
        self.SetMenuBar(menuBar)

        # Create toolbar
        tb = self.CreateToolBar(wxTB_HORIZONTAL | wxNO_BORDER | wxTB_FLAT)
        tb.SetToolBitmapSize((24, 23))
        tb.AddSimpleTool(wxID_NEW, images.getNewBitmap(), 'New', 'New file')
        tb.AddSimpleTool(wxID_OPEN, images.getOpenBitmap(), 'Open', 'Open file')
        tb.AddSimpleTool(wxID_SAVE, images.getSaveBitmap(), 'Save', 'Save file')
        tb.AddControl(wxStaticLine(tb, -1, size=(-1,23), style=wxLI_VERTICAL))
        tb.AddSimpleTool(wxID_CUT, images.getCutBitmap(), 'Cut', 'Cut')
        tb.AddSimpleTool(wxID_COPY, images.getCopyBitmap(), 'Copy', 'Copy')
        tb.AddSimpleTool(wxID_PASTE, images.getPasteBitmap(), 'Paste', 'Paste')
        tb.AddControl(wxStaticLine(tb, -1, size=(-1,23), style=wxLI_VERTICAL))
        tb.AddSimpleTool(self.ID_TEST, images.getTestBitmap(), 'Test', 'Test window')
        tb.AddSimpleTool(self.ID_REFRESH, images.getRefreshBitmap(),
                         'Refresh', 'Refresh view')
        tb.AddSimpleTool(self.ID_AUTO_REFRESH, images.getAutoRefreshBitmap(),
                         'Auto-refresh', 'Toggle auto-refresh mode', true)
        if wxGetOsVersion()[0] == wxGTK:
            tb.AddSeparator()   # otherwise auto-refresh sticks in status line
        tb.ToggleTool(self.ID_AUTO_REFRESH, conf.autoRefresh)
        tb.Realize()
        self.tb = tb
        self.minWidth = tb.GetSize()[0] # minimal width is the size of toolbar

        # File
        EVT_MENU(self, wxID_NEW, self.OnNew)
        EVT_MENU(self, wxID_OPEN, self.OnOpen)
        EVT_MENU(self, wxID_SAVE, self.OnSaveOrSaveAs)
        EVT_MENU(self, wxID_SAVEAS, self.OnSaveOrSaveAs)
        EVT_MENU(self, wxID_EXIT, self.OnExit)
        # Edit
        EVT_MENU(self, wxID_UNDO, self.OnUndo)
        EVT_MENU(self, wxID_REDO, self.OnRedo)
        EVT_MENU(self, wxID_CUT, self.OnCut)
        EVT_MENU(self, wxID_COPY, self.OnCopy)
        EVT_MENU(self, wxID_PASTE, self.OnPaste)
        EVT_MENU(self, self.ID_DELETE, self.OnDelete)
        # View
        EVT_MENU(self, self.ID_EMBED_PANEL, self.OnEmbedPanel)
        EVT_MENU(self, self.ID_TEST, self.OnTest)
        EVT_MENU(self, self.ID_REFRESH, self.OnRefresh)
        EVT_MENU(self, self.ID_AUTO_REFRESH, self.OnAutoRefresh)
        # Help
        EVT_MENU(self, wxID_ABOUT, self.OnAbout)
        EVT_MENU(self, self.ID_README, self.OnReadme)

        # Update events
        EVT_UPDATE_UI(self, wxID_CUT, self.OnUpdateUI)
        EVT_UPDATE_UI(self, wxID_COPY, self.OnUpdateUI)
        EVT_UPDATE_UI(self, wxID_PASTE, self.OnUpdateUI)
        EVT_UPDATE_UI(self, self.ID_DELETE, self.OnUpdateUI)
        EVT_UPDATE_UI(self, self.ID_TEST, self.OnUpdateUI)
        EVT_UPDATE_UI(self, self.ID_REFRESH, self.OnUpdateUI)

        # Build interface
        sizer = wxBoxSizer(wxVERTICAL)
        sizer.Add(wxStaticLine(self, -1), 0, wxEXPAND)
        splitter = wxSplitterWindow(self, -1, style=wxSP_3DSASH)
        self.splitter = splitter
        splitter.SetMinimumPaneSize(100)
        # Create tree
        global tree
        tree = XML_Tree(splitter, -1)
        sys.modules['xxx'].tree = tree
        # !!! frame styles are broken
        # Miniframe for not embedded mode
        miniFrame = wxFrame(self, -1, 'Properties Panel',
                            (conf.panelX, conf.panelY),
                            (conf.panelWidth, conf.panelHeight))
        self.miniFrame = miniFrame
        sizer2 = wxBoxSizer()
        miniFrame.SetAutoLayout(true)
        miniFrame.SetSizer(sizer2)
        EVT_CLOSE(self.miniFrame, self.OnCloseMiniFrame)
        # Create panel for parameters
        global panel
        if conf.embedPanel:
            panel = Panel(splitter)
            # Set plitter windows
            splitter.SplitVertically(tree, panel, conf.sashPos)
        else:
            panel = Panel(miniFrame)
            sizer2.Add(panel, 1, wxEXPAND)
            miniFrame.Show(true)
            splitter.Initialize(tree)
        sizer.Add(splitter, 1, wxEXPAND)
        self.SetAutoLayout(true)
        self.SetSizer(sizer)

        # Init pull-down menu data
        global pullDownMenu
        pullDownMenu = PullDownMenu(self)
        # Mapping from IDs to element names
        self.createMap = {
            pullDownMenu.ID_NEW_PANEL: 'wxPanel',
            pullDownMenu.ID_NEW_DIALOG: 'wxDialog',
            pullDownMenu.ID_NEW_FRAME: 'wxFrame',
            pullDownMenu.ID_NEW_TOOL_BAR: 'wxToolBar',
            pullDownMenu.ID_NEW_TOOL: 'tool',
            pullDownMenu.ID_NEW_MENU_BAR: 'wxMenuBar',
            pullDownMenu.ID_NEW_MENU: 'wxMenu',
            pullDownMenu.ID_NEW_MENU_ITEM: 'wxMenuItem',
            pullDownMenu.ID_NEW_SEPARATOR: 'separator',

            pullDownMenu.ID_NEW_STATIC_TEXT: 'wxStaticText',
            pullDownMenu.ID_NEW_TEXT_CTRL: 'wxTextCtrl',

            pullDownMenu.ID_NEW_BUTTON: 'wxButton',
            pullDownMenu.ID_NEW_BITMAP_BUTTON: 'wxBitmapButton',
            pullDownMenu.ID_NEW_RADIO_BUTTON: 'wxRadioButton',
            pullDownMenu.ID_NEW_SPIN_BUTTON: 'wxSpinButton',

            pullDownMenu.ID_NEW_STATIC_BOX: 'wxStaticBox',
            pullDownMenu.ID_NEW_CHECK_BOX: 'wxCheckBox',
            pullDownMenu.ID_NEW_RADIO_BOX: 'wxRadioBox',
            pullDownMenu.ID_NEW_COMBO_BOX: 'wxComboBox',
            pullDownMenu.ID_NEW_LIST_BOX: 'wxListBox',

            pullDownMenu.ID_NEW_STATIC_LINE: 'wxStaticLine',
            pullDownMenu.ID_NEW_STATIC_BITMAP: 'wxStaticBitmap',
            pullDownMenu.ID_NEW_CHOICE: 'wxChoice',
            pullDownMenu.ID_NEW_SLIDER: 'wxSlider',
            pullDownMenu.ID_NEW_GAUGE: 'wxGauge',
            pullDownMenu.ID_NEW_SCROLL_BAR: 'wxScrollBar',
            pullDownMenu.ID_NEW_TREE_CTRL: 'wxTreeCtrl',
            pullDownMenu.ID_NEW_LIST_CTRL: 'wxListCtrl',
            pullDownMenu.ID_NEW_CHECK_LIST: 'wxCheckList',
            pullDownMenu.ID_NEW_NOTEBOOK: 'wxNotebook',
            pullDownMenu.ID_NEW_HTML_WINDOW: 'wxHtmlWindow',
            pullDownMenu.ID_NEW_CALENDAR: 'wxCalendar',

            pullDownMenu.ID_NEW_BOX_SIZER: 'wxBoxSizer',
            pullDownMenu.ID_NEW_STATIC_BOX_SIZER: 'wxStaticBoxSizer',
            pullDownMenu.ID_NEW_GRID_SIZER: 'wxGridSizer',
            pullDownMenu.ID_NEW_FLEX_GRID_SIZER: 'wxFlexGridSizer',
            pullDownMenu.ID_NEW_SPACER: 'spacer',
            }
        pullDownMenu.controls = [
            ['control', 'Various controls',
             (pullDownMenu.ID_NEW_STATIC_TEXT, 'Label', 'Create static label'),
             (pullDownMenu.ID_NEW_STATIC_LINE, 'Line', 'Create static line'),
             (pullDownMenu.ID_NEW_TEXT_CTRL, 'TextBox', 'Create text box control'),
             (pullDownMenu.ID_NEW_CHOICE, 'Choice', 'Create choice control'),
             (pullDownMenu.ID_NEW_SLIDER, 'Slider', 'Create slider control'),
             (pullDownMenu.ID_NEW_GAUGE, 'Gauge', 'Create gauge control'),
             (pullDownMenu.ID_NEW_SCROLL_BAR, 'ScrollBar', 'Create scroll bar'),
             (pullDownMenu.ID_NEW_TREE_CTRL, 'TreeCtrl', 'Create tree control'),
             (pullDownMenu.ID_NEW_LIST_CTRL, 'ListCtrl', 'Create list control'),
             (pullDownMenu.ID_NEW_HTML_WINDOW, 'HtmlWindow', 'Create HTML window'),
             (pullDownMenu.ID_NEW_CALENDAR, 'Calendar', 'Create calendar control'),
             (pullDownMenu.ID_NEW_PANEL, 'Panel', 'Create panel'),
             (pullDownMenu.ID_NEW_NOTEBOOK, 'Notebook', 'Create notebook control'),
             ],
            ['button', 'Buttons',
             (pullDownMenu.ID_NEW_BUTTON, 'Button', 'Create button'),
             (pullDownMenu.ID_NEW_BITMAP_BUTTON, 'BitmapButton', 'Create bitmap button'),
             (pullDownMenu.ID_NEW_RADIO_BUTTON, 'RadioButton', 'Create radio button'),
             (pullDownMenu.ID_NEW_SPIN_BUTTON, 'SpinButton', 'Create spin button'),
             ],
            ['box', 'Boxes',
             (pullDownMenu.ID_NEW_STATIC_BOX, 'StaticBox', 'Create static box'),
             (pullDownMenu.ID_NEW_CHECK_BOX, 'CheckBox', 'Create check box'),
             (pullDownMenu.ID_NEW_RADIO_BOX, 'RadioBox', 'Create radio box'),
             (pullDownMenu.ID_NEW_COMBO_BOX, 'ComboBox', 'Create combo box'),
             (pullDownMenu.ID_NEW_LIST_BOX, 'ListBox', 'Create list box'),
             (pullDownMenu.ID_NEW_CHECK_LIST, 'CheckListBox',
              'Create check list control'),
             ],
            ['sizer', 'Sizers',
             (pullDownMenu.ID_NEW_BOX_SIZER, 'BoxSizer', 'Create box sizer'),
             (pullDownMenu.ID_NEW_STATIC_BOX_SIZER, 'StaticBoxSizer',
              'Create static box sizer'),
             (pullDownMenu.ID_NEW_GRID_SIZER, 'GridSizer', 'Create grid sizer'),
             (pullDownMenu.ID_NEW_FLEX_GRID_SIZER, 'FlexGridSizer',
              'Create flexgrid sizer'),
             (pullDownMenu.ID_NEW_SPACER, 'Spacer', 'Create spacer'),
             ]
            ]
        pullDownMenu.menuControls = [
            (pullDownMenu.ID_NEW_MENU, 'Menu', 'Create menu'),
            (pullDownMenu.ID_NEW_MENU_ITEM, 'MenuItem', 'Create menu item'),
            (pullDownMenu.ID_NEW_SEPARATOR, 'Separator', 'Create separator'),
            ]
        pullDownMenu.toolBarControls = [
            (pullDownMenu.ID_NEW_TOOL, 'Tool', 'Create tool'),
            (pullDownMenu.ID_NEW_SEPARATOR, 'Separator', 'Create separator'),
            ['control', 'Various controls',
             (pullDownMenu.ID_NEW_STATIC_TEXT, 'Label', 'Create static label'),
             (pullDownMenu.ID_NEW_STATIC_LINE, 'Line', 'Create static line'),
             (pullDownMenu.ID_NEW_TEXT_CTRL, 'TextBox', 'Create text box control'),
             (pullDownMenu.ID_NEW_CHOICE, 'Choice', 'Create choice control'),
             (pullDownMenu.ID_NEW_SLIDER, 'Slider', 'Create slider control'),
             (pullDownMenu.ID_NEW_GAUGE, 'Gauge', 'Create gauge control'),
             (pullDownMenu.ID_NEW_SCROLL_BAR, 'ScrollBar', 'Create scroll bar'),
             (pullDownMenu.ID_NEW_LIST_CTRL, 'ListCtrl', 'Create list control'),
             ],
            ['button', 'Buttons',
             (pullDownMenu.ID_NEW_BUTTON, 'Button', 'Create button'),
             (pullDownMenu.ID_NEW_BITMAP_BUTTON, 'BitmapButton', 'Create bitmap button'),
             (pullDownMenu.ID_NEW_RADIO_BUTTON, 'RadioButton', 'Create radio button'),
             (pullDownMenu.ID_NEW_SPIN_BUTTON, 'SpinButton', 'Create spin button'),
             ],
            ['box', 'Boxes',
             (pullDownMenu.ID_NEW_STATIC_BOX, 'StaticBox', 'Create static box'),
             (pullDownMenu.ID_NEW_CHECK_BOX, 'CheckBox', 'Create check box'),
             (pullDownMenu.ID_NEW_RADIO_BOX, 'RadioBox', 'Create radio box'),
             (pullDownMenu.ID_NEW_COMBO_BOX, 'ComboBox', 'Create combo box'),
             (pullDownMenu.ID_NEW_LIST_BOX, 'ListBox', 'Create list box'),
             (pullDownMenu.ID_NEW_CHECK_LIST, 'CheckListBox',
              'Create check list control'),
             ],
            ]

        # Initialize
        self.Clear()

        # Other events
        EVT_IDLE(self, self.OnIdle)
        EVT_CLOSE(self, self.OnCloseWindow)

    def OnNew(self, evt):
        self.Clear()

    def OnOpen(self, evt):
        if not self.AskSave(): return
        dlg = wxFileDialog(self, 'Open', os.path.dirname(self.dataFile),
                           '', '*.xrc', wxOPEN | wxCHANGE_DIR)
        if dlg.ShowModal() == wxID_OK:
            path = dlg.GetPath()
            self.SetStatusText('Loading...')
            wxYield()
            wxBeginBusyCursor()
            try:
                self.Open(path)
                self.SetStatusText('Data loaded')
            except:
                self.SetStatusText('Failed')
                raise
            wxEndBusyCursor()
        dlg.Destroy()

    def OnSaveOrSaveAs(self, evt):
        if evt.GetId() == wxID_SAVEAS or not self.dataFile:
            if self.dataFile: defaultName = ''
            else: defaultName = 'UNTITLED.xrc'
            dlg = wxFileDialog(self, 'Save As', os.path.dirname(self.dataFile),
                               defaultName, '*.xrc',
                               wxSAVE | wxOVERWRITE_PROMPT | wxCHANGE_DIR)
            if dlg.ShowModal() == wxID_OK:
                path = dlg.GetPath()
                dlg.Destroy()
            else:
                dlg.Destroy()
                return
        else:
            path = self.dataFile
        self.SetStatusText('Saving...')
        wxYield()
        wxBeginBusyCursor()
        try:
            self.Save(path)
            self.dataFile = path
            self.SetStatusText('Data saved')
        except IOError:
            self.SetStatusText('Failed')
        wxEndBusyCursor()

    def OnExit(self, evt):
        self.Close()

    def OnUndo(self, evt):
        print '*** being implemented'
        return
        print self.lastOp, self.undo
        if self.lastOp == 'DELETE':
            parent, prev, elem = self.undo
            if prev.IsOk():
                xxx = MakeXXXFromDOM(tree.GetPyData(parent).treeObject(), elem)
                item = tree.InsertItem( parent, prev, xxx.treeObject().className,
                                        data=wxTreeItemData(xxx) )

    def OnRedo(self, evt):
        print '*** being implemented'

    def OnCut(self, evt):
        selected = tree.selection
        if not selected: return         # key pressed event
        # Undo info
        self.lastOp = 'CUT'
        self.undo = [tree.GetItemParent(selected), tree.GetPrevSibling(selected)]
        # Delete testWin?
        global testWin
        if testWin:
            # If deleting top-level item, delete testWin
            if selected == testWin.item:
                testWin.Destroy()
                testWin = None
            else:
                # Remove highlight, update testWin
                if not tree.IsHighlatable(selected):
                    if testWin.highLight: testWin.highLight.Remove()
                    tree.needUpdate = true
        self.clipboard = tree.RemoveLeaf(selected)
        tree.pendingHighLight = None
        tree.Unselect()
        panel.Clear()
        self.modified = true
        self.SetStatusText('Removed to clipboard')

    def OnCopy(self, evt):
        selected = tree.selection
        if not selected: return         # key pressed event
        xxx = tree.GetPyData(selected)
        self.clipboard = xxx.element.cloneNode(true)
        self.SetStatusText('Copied')

    def OnPaste(self, evt):
        selected = tree.selection
        if not selected: return         # key pressed event
        # For pasting with Ctrl pressed
        if evt.GetId() == pullDownMenu.ID_PASTE_SIBLING: appendChild = false
        else: appendChild = not tree.NeedInsert(selected)
        xxx = tree.GetPyData(selected)
        if not appendChild:
            # If has next item, insert, else append to parent
            nextItem = tree.GetNextSibling(selected)
            if nextItem.IsOk():
                # Insert before nextItem
                parentLeaf = tree.GetItemParent(selected)
            else:                   # last child: change selected to parent
                appendChild = true
                selected = tree.GetItemParent(selected)
        # Expanded container (must have children)
        elif tree.IsExpanded(selected) and tree.GetChildrenCount(selected, false):
            appendChild = false
            nextItem = tree.GetFirstChild(selected, 0)[0]
            parentLeaf = selected
        # Parent should be tree element or None
        if appendChild:
            parent = tree.GetPyData(selected)
        else:
            parent = tree.GetPyData(parentLeaf)
        if parent.hasChild: parent = parent.child

        # Create a copy of clipboard element
        elem = self.clipboard.cloneNode(true)
        # Tempopary xxx object to test things
        xxx = MakeXXXFromDOM(parent, elem)

        # Check compatibility
        error = false
        # Top-level
        x = xxx.treeObject()
        if x.__class__ in [xxxDialog, xxxFrame, xxxMenuBar, xxxToolBar]:
            if parent.__class__ != xxxMainNode: error = true
        elif x.__class__ == xxxPanel and parent.__class__ == xxxMainNode:
            pass
        elif x.__class__ == xxxSpacer:
            if not parent.isSizer: error = true
        elif x.__class__ == xxxSeparator:
            if not parent.__class__ in [xxxMenu, xxxToolBar]: error = true
        elif x.__class__ == xxxTool:
            if parent.__class__ != xxxToolBar: error = true
        elif x.__class__ == xxxMenuItem:
            if not parent.__class__ in [xxxMenuBar, xxxMenu]: error = true
        elif x.isSizer and parent.__class__ == xxxNotebook: error = true
        else:                           # normal controls can be almost anywhere
            if parent.__class__ == xxxMainNode or \
               parent.__class__ in [xxxMenuBar, xxxMenu]: error = true
        if error:
            if parent.__class__ == xxxMainNode: parentClass = 'root'
            else: parentClass = parent.className
            wxLogError('Incompatible parent/child: parent is %s, child is %s!' %
                       (parentClass, x.className))
            return

        # Check parent and child relationships.
        # If parent is sizer or notebook, child is of wrong class or
        # parent is normal window, child is child container then detach child.
        isChildContainer = isinstance(xxx, xxxChildContainer)
        if isChildContainer and \
           ((parent.isSizer and not isinstance(xxx, xxxSizerItem)) or \
           (isinstance(parent, xxxNotebook) and not isinstance(xxx, xxxNotebookPage)) or \
           not (parent.isSizer or isinstance(parent, xxxNotebook))):
            elem.removeChild(xxx.child.element) # detach child
            elem.unlink()           # delete child container
            elem = xxx.child.element # replace
            # This may help garbage collection
            xxx.child.parent = None
            isChildContainer = false
        # Parent is sizer or notebook, child is not child container
        if parent.isSizer and not isChildContainer and not isinstance(xxx, xxxSpacer):
            # Create sizer item element
            sizerItemElem = MakeEmptyDOM('sizeritem')
            sizerItemElem.appendChild(elem)
            elem = sizerItemElem
        elif isinstance(parent, xxxNotebook) and not isChildContainer:
            pageElem = MakeEmptyDOM('notebookpage')
            pageElem.appendChild(elem)
            elem = pageElem
        xxx = MakeXXXFromDOM(parent, elem)
        # Figure out if we must append a new child or sibling
        if appendChild:
            parent.element.appendChild(elem)
            newItem = tree.AppendItem(selected, xxx.treeName(), image=xxx.treeImage(),
                                      data=wxTreeItemData(xxx))
        else:
            node = tree.GetPyData(nextItem).element
            parent.element.insertBefore(elem, node)
            # Inserting before is difficult, se we insert after or first child
            index = tree.ItemIndex(parentLeaf, nextItem)
            newItem = tree.InsertItemBefore(parentLeaf, index,
                        xxx.treeName(), image=xxx.treeImage())
            tree.SetPyData(newItem, xxx)
#            newItem = tree.InsertItem(parentLeaf, selected, xxx.treeName(),
#                                      image=xxx.treeImage(), data=wxTreeItemData(xxx))
        # Add children items
        if xxx.hasChildren:
            treeObj = xxx.treeObject()
            for n in treeObj.element.childNodes:
                if IsObject(n):
                    tree.AddNode(newItem, treeObj, n)
        # Scroll to show new item
        tree.EnsureVisible(newItem)
        tree.SelectItem(newItem)
        if not tree.IsVisible(newItem):
            tree.ScrollTo(newItem)
            tree.Refresh()
        # Update view?
        if testWin and tree.IsHighlatable(newItem):
            if conf.autoRefresh:
                tree.needUpdate = true
                tree.pendingHighLight = newItem
            else:
                tree.pendingHighLight = None
        self.modified = true
        self.SetStatusText('Pasted')

    def OnDelete(self, evt):
        selected = tree.selection
        if not selected: return         # key pressed event
        # Undo info
        self.lastOp = 'DELETE'
        self.undo = [tree.GetItemParent(selected), tree.GetPrevSibling(selected)]
        # Delete testWin?
        global testWin
        if testWin:
            # If deleting top-level item, delete testWin
            if selected == testWin.item:
                testWin.Destroy()
                testWin = None
            else:
                # Remove highlight, update testWin
                if not tree.IsHighlatable(selected):
                    if testWin.highLight: testWin.highLight.Remove()
                    tree.needUpdate = true
        xnode = tree.RemoveLeaf(selected)
        # !!! cloneNode is broken, or something is wrong
#        self.undo.append(xnode.cloneNode(true))
        xnode.unlink()
        tree.pendingHighLight = None
        tree.Unselect()
        panel.Clear()
        self.modified = true
        self.SetStatusText('Deleted')

    def OnEmbedPanel(self, evt):
        conf.embedPanel = evt.IsChecked()
        if conf.embedPanel:
            # Remember last dimentions
            conf.panelX, conf.panelY = self.miniFrame.GetPosition()
            conf.panelWidth, conf.panelHeight = self.miniFrame.GetSize()
            size = self.GetSize()
            pos = self.GetPosition()
            sizePanel = panel.GetSize()
            panel.Reparent(self.splitter)
            self.miniFrame.GetSizer().RemoveWindow(panel)
            wxYield()
            # Widen
            self.SetDimensions(pos.x, pos.y, size.x + sizePanel.x, size.y)
            self.splitter.SplitVertically(tree, panel, conf.sashPos)
            self.miniFrame.Show(false)
        else:
            conf.sashPos = self.splitter.GetSashPosition()
            pos = self.GetPosition()
            size = self.GetSize()
            sizePanel = panel.GetSize()
            self.splitter.Unsplit(panel)
            sizer = self.miniFrame.GetSizer()
            panel.Reparent(self.miniFrame)
            panel.Show(true)
            sizer.Add(panel, 1, wxEXPAND)
            self.miniFrame.Show(true)
            self.miniFrame.SetDimensions(conf.panelX, conf.panelY,
                                         conf.panelWidth, conf.panelHeight)
            wxYield()
            # Reduce width
            self.SetDimensions(pos.x, pos.y,
                               max(size.x - sizePanel.x, self.minWidth), size.y)

    def OnTest(self, evt):
        if not tree.selection: return   # key pressed event
        tree.ShowTestWindow(tree.selection)

    def OnRefresh(self, evt):
        # If modified, apply first
        selection = tree.selection
        if selection:
            xxx = tree.GetPyData(selection)
            if xxx and panel.IsModified():
                tree.Apply(xxx, selection)
        if testWin:
            # (re)create
            tree.CreateTestWin(testWin.item)
        tree.needUpdate = false

    def OnAutoRefresh(self, evt):
        conf.autoRefresh = evt.IsChecked()
        self.menuBar.Check(self.ID_AUTO_REFRESH, conf.autoRefresh)
        self.tb.ToggleTool(self.ID_AUTO_REFRESH, conf.autoRefresh)

    def OnAbout(self, evt):
        str = '%s %s\n\nRoman Rolinsky <rolinsky@mema.ucl.ac.be>' % \
              (progname, version)
        dlg = wxMessageDialog(self, str, 'About ' + progname, wxOK | wxCENTRE)
        dlg.ShowModal()
        dlg.Destroy()

    def OnReadme(self, evt):
        text = open(os.path.join(sys.path[0], 'README'), 'r').read()
        dlg = ScrolledMessageDialog(self, text, "XRCed README")
        dlg.ShowModal()
        dlg.Destroy()


    # Simple emulation of python command line
    def OnDebugCMD(self, evt):
        import traceback
        while 1:
            try:
                exec raw_input('C:\> ')
            except EOFError:
                print '^D'
                break
            except:
                (etype, value, tb) =sys.exc_info()
                tblist =traceback.extract_tb(tb)[1:]
                msg =string.join(traceback.format_exception_only(etype, value)
                        +traceback.format_list(tblist))
                print msg

    def OnCreate(self, evt):
        selected = tree.selection
        if tree.ctrl: appendChild = false
        else: appendChild = not tree.NeedInsert(selected)
        xxx = tree.GetPyData(selected)
        if not appendChild:
            # If insert before
            if tree.shift:
                # If has previous item, insert after it, else append to parent
                nextItem = selected
                parentLeaf = tree.GetItemParent(selected)
            else:
                # If has next item, insert, else append to parent
                nextItem = tree.GetNextSibling(selected)
                if nextItem.IsOk():
                    # Insert before nextItem
                    parentLeaf = tree.GetItemParent(selected)
                else:                   # last child: change selected to parent
                    appendChild = true
                    selected = tree.GetItemParent(selected)
        # Expanded container (must have children)
        elif tree.shift and tree.IsExpanded(selected) \
           and tree.GetChildrenCount(selected, false):
            appendChild = false
            nextItem = tree.GetFirstChild(selected, 0)[0]
            parentLeaf = selected
        # Parent should be tree element or None
        if appendChild:
            parent = tree.GetPyData(selected)
        else:
            parent = tree.GetPyData(parentLeaf)
        if parent.hasChild: parent = parent.child

        # Create element
        className = self.createMap[evt.GetId()]
        xxx = MakeEmptyXXX(parent, className)

        # Set default name for top-level windows
        if parent.__class__ == xxxMainNode:
            cl = xxx.treeObject().__class__
            frame.maxIDs[cl] += 1
            xxx.treeObject().name = '%s%d' % (defaultIDs[cl], frame.maxIDs[cl])
            xxx.treeObject().element.setAttribute('name', xxx.treeObject().name)

        # Figure out if we must append a new child or sibling
        elem = xxx.element
        if appendChild:
            # Insert newline for debug purposes
            parent.element.appendChild(elem)
            newItem = tree.AppendItem(selected, xxx.treeName(), image=xxx.treeImage(),
                                      data=wxTreeItemData(xxx))
        else:
            node = tree.GetPyData(nextItem).element
            parent.element.insertBefore(elem, node)
            # !!! There is a different behavious on Win and GTK
            # !!! On Win InsertItem(parent, parent, ...) inserts at the end.
            index = tree.ItemIndex(parentLeaf, nextItem)
            newItem = tree.InsertItemBefore(parentLeaf, index,
                        xxx.treeName(), image=xxx.treeImage())
#                       data=wxTreeItemData(xxx)) # does not work
            tree.SetPyData(newItem, xxx)
#            newItem = tree.InsertItem(parentLeaf, selected,
#                                      xxx.treeName(), image=xxx.treeImage(),
#                                      data=wxTreeItemData(xxx))
        tree.EnsureVisible(newItem)
        tree.SelectItem(newItem)
        if not tree.IsVisible(newItem):
            tree.ScrollTo(newItem)
            tree.Refresh()
        # Update view?
        if testWin and tree.IsHighlatable(newItem):
            if conf.autoRefresh:
                tree.needUpdate = true
                tree.pendingHighLight = newItem
            else:
                tree.pendingHighLight = None

    # Expand/collapse subtree
    def OnExpand(self, evt):
        if tree.selection: tree.ExpandAll(tree.selection)
        else: tree.ExpandAll(tree.root)
    def OnCollapse(self, evt):
        if tree.selection: tree.CollapseAll(tree.selection)
        else: tree.CollapseAll(tree.root)

    def OnPullDownHighlight(self, evt):
        menuId = evt.GetMenuId()
        if menuId != -1:
            menu = evt.GetEventObject()
            help = menu.GetHelpString(menuId)
            self.SetStatusText(help)
        else:
            self.SetStatusText('')

    def OnUpdateUI(self, evt):
        if evt.GetId() in [wxID_CUT, wxID_COPY, self.ID_DELETE]:
            evt.Enable(tree.selection != tree.root)
        elif evt.GetId() == wxID_PASTE:
            evt.Enable((self.clipboard and tree.selection) != None)
        elif evt.GetId() == self.ID_TEST:
            evt.Enable(tree.selection != tree.root)

    def OnIdle(self, evt):
        if self.inIdle: return          # Recursive call protection
        self.inIdle = true
        if tree.needUpdate:
            if conf.autoRefresh:
                if testWin:
                    self.SetStatusText('Refreshing test window...')
                    # (re)create
                    tree.CreateTestWin(testWin.item)
                    wxYield()
                    self.SetStatusText('')
                tree.needUpdate = false
        elif tree.pendingHighLight:
            tree.HighLight(tree.pendingHighLight)
        else:
            evt.Skip()
        self.inIdle = false

    # We don't let close panel window
    def OnCloseMiniFrame(self, evt):
        return

    def OnCloseWindow(self, evt):
        if not self.AskSave(): return
        if testWin: testWin.Destroy()
        # Destroy cached windows
        panel.cacheParent.Destroy()
        if not panel.GetPageCount() == 2:
            panel.page2.Destroy()
        conf.x, conf.y = self.GetPosition()
        conf.width, conf.height = self.GetSize()
        if conf.embedPanel:
            conf.sashPos = self.splitter.GetSashPosition()
        else:
            conf.panelX, conf.panelY = self.miniFrame.GetPosition()
            conf.panelWidth, conf.panelHeight = self.miniFrame.GetSize()
        evt.Skip()

    def Clear(self):
        self.dataFile = ''
        self.clipboard = None
        self.modified = false
        panel.SetModified(false)
        tree.Clear()
        panel.Clear()
        global testWin
        if testWin:
            testWin.Destroy()
            testWin = None
        self.SetTitle(progname)
        # Numbers for new controls
        self.maxIDs = {}
        self.maxIDs[xxxPanel] = self.maxIDs[xxxDialog] = self.maxIDs[xxxFrame] = \
        self.maxIDs[xxxMenuBar] = self.maxIDs[xxxMenu] = self.maxIDs[xxxToolBar] = 0

    def Open(self, path):
        # Try to read the file
        try:
            open(path)
            self.Clear()
            # Build wx tree
            dom = minidom.parse(path)
            tree.SetData(dom)
            self.dataFile = path
            self.SetTitle(progname + ': ' + os.path.basename(path))
        except:
            wxLogError('Error reading file: %s' % path)
            raise

    def Indent(self, node, indent = 0):
        # Copy child list because it will change soon
        children = node.childNodes[:]
        # Main node doesn't need to be indented
        if indent:
            text = self.domCopy.createTextNode('\n' + ' ' * indent)
            node.parentNode.insertBefore(text, node)
        if children:
            # Append newline after last child, except for text nodes
            if children[-1].nodeType == minidom.Node.ELEMENT_NODE:
                text = self.domCopy.createTextNode('\n' + ' ' * indent)
                node.appendChild(text)
            # Indent children which are elements
            for n in children:
                if n.nodeType == minidom.Node.ELEMENT_NODE:
                    self.Indent(n, indent + 2)

    def Save(self, path):
        try:
            # Apply changes
            self.OnRefresh(wxCommandEvent())
            f = open(path, 'w')
            # Make temporary copy
            # !!! We can't clone dom node, it works only once
            #self.domCopy = tree.dom.cloneNode(true)
            self.domCopy = minidom.Document()
            mainNode = self.domCopy.appendChild(tree.mainNode.cloneNode(true))
            self.Indent(mainNode)
            self.domCopy.writexml(f)
            f.close()
            self.domCopy.unlink()
            self.domCopy = None
            self.modified = false
            panel.SetModified(false)
        except:
            wxLogError('Error writing file: %s' % path)
            raise

    def AskSave(self):
        if not (self.modified or panel.IsModified()): return true
        flags = wxICON_EXCLAMATION | wxYES_NO | wxCANCEL | wxCENTRE
        dlg = wxMessageDialog( self, 'File is modified. Save before exit?',
                               'Save before too late?', flags )
        say = dlg.ShowModal()
        dlg.Destroy()
        if say == wxID_YES:
            self.OnSaveOrSaveAs(wxCommandEvent(wxID_SAVE))
            # If save was successful, modified flag is unset
            if not self.modified: return true
        elif say == wxID_NO:
            self.modified = false
            panel.SetModified(false)
            return true
        return false

################################################################################

def usage():
    print >> sys.stderr, 'usage: xrced [-dvh] [file]'

class App(wxApp):
    def OnInit(self):
        global debug, verbose
        # Process comand-line
        try:
            opts, args = getopt.getopt(sys.argv[1:], 'dvh')
        except getopt.GetoptError:
            print >> sys.stderr, 'Unknown option'
            usage()
            sys.exit(1)
        for o,a in opts:
            if o == '-h':
                usage()
                sys.exit(0)
            elif o == '-d':
                debug = true
            elif o == '-v':
                print 'XRCed version', version
                sys.exit(0)

        self.SetAppName('xrced')
        # Settings
        global conf
        conf = wxConfig(style = wxCONFIG_USE_LOCAL_FILE)
        conf.autoRefresh = conf.ReadInt('autorefresh', true)
        pos = conf.ReadInt('x', -1), conf.ReadInt('y', -1)
        size = conf.ReadInt('width', 800), conf.ReadInt('height', 600)
        conf.embedPanel = conf.ReadInt('embedPanel', true)
        conf.sashPos = conf.ReadInt('sashPos', 200)
        if not conf.embedPanel:
            conf.panelX = conf.ReadInt('panelX', -1)
            conf.panelY = conf.ReadInt('panelY', -1)
        else:
            conf.panelX = conf.panelY = -1
        conf.panelWidth = conf.ReadInt('panelWidth', 200)
        conf.panelHeight = conf.ReadInt('panelHeight', 200)
        conf.panic = not conf.HasEntry('nopanic')
        # Add handlers
        wxFileSystem_AddHandler(wxMemoryFSHandler())
        wxInitAllImageHandlers()
        # Create main frame
        frame = Frame(pos, size)
        frame.Show(true)
        # Load resources from XRC file (!!! should be transformed to .py later?)
        sys.modules['params'].frame = frame
        frame.res = wxXmlResource('')
        frame.res.Load(os.path.join(sys.path[0], 'xrced.xrc'))

        # Load file after showing
        if args:
            conf.panic = false
            frame.open = frame.Open(args[0])

        return true

    def OnExit(self):
        # Write config
        wc = wxConfigBase_Get()
        wc.WriteInt('autorefresh', conf.autoRefresh)
        wc.WriteInt('x', conf.x)
        wc.WriteInt('y', conf.y)
        wc.WriteInt('width', conf.width)
        wc.WriteInt('height', conf.height)
        wc.WriteInt('embedPanel', conf.embedPanel)
        if not conf.embedPanel:
            wc.WriteInt('panelX', conf.panelX)
            wc.WriteInt('panelY', conf.panelY)
        wc.WriteInt('sashPos', conf.sashPos)
        wc.WriteInt('panelWidth', conf.panelWidth)
        wc.WriteInt('panelHeight', conf.panelHeight)
        wc.WriteInt('nopanic', 1)
        wc.Flush()

def main():
    app = App()
    app.MainLoop()
    app.OnExit()

if __name__ == '__main__':
    main()
