"""
wxMVCTree is a control which handles hierarchical data. It is constructed
in model-view-controller architecture, so the display of that data, and
the content of the data can be changed greatly without affecting the other parts.
This module contains the wxMVCTree class (the 'controller' of the MVC trio)
and PathfinderNode, which it uses internally to manage its info.

Pathfinder actually is even more configurable than MVC normally implies, because
almost every aspect of it is pluggable:
    wxMVCTree - Overall controller, and the window that actually gets placed
    in the GUI.
        Painter - Paints the control. The 'view' part of MVC.
           NodePainter - Paints just the nodes
           LinePainter - Paints just the lines between the nodes
           TextConverter - Figures out what text to print for each node
        Editor - Edits the contents of a node, if the model is editable.
        LayoutEngine - Determines initial placement of nodes
        Transform - Adjusts positions of nodes for movement or special effects.
        TreeModel - Contains the data which the rest of the control acts
        on. The 'model' part of MVC.

Author/Maintainer - Bryn Keller <xoltar@starship.python.net>
"""

#------------------------------------------------------------------------
from wxPython.wx import *
import os, sys
#------------------------------------------------------------------------

class MVCTreeNode:
    """
    Used internally by wxMVCTree to manage its data. Contains information about
    screen placement, the actual data associated with it, and more. These are
    the nodes passed to all the other helper parts to do their work with.
    """
    def __init__(self, data=None, parent = None, kids = [], x = 0, y = 0):
        self.x = 0
        self.y = 0
        self.projx = 0
        self.projy = 0
        self.parent = parent
        self.kids = kids
        self.data = data
        self.expanded = false
        self.selected = false
        self.built = false
        self.scale = 0

    def GetChildren(self):
        return self.kids

    def GetParent(self):
        return self.parent

    def Remove(self, node):
        try:
            self.kids.remove(node)
        except:
            pass
    def Add(self, node):
        self.kids.append(node)
        node.SetParent(self)

    def SetParent(self, parent):
        if self.parent and not (self.parent is parent):
            self.parent.Remove(self)
        self.parent = parent
    def __str__(self):
        return "Node: "  + str(self.data) + " (" + str(self.x) + ", " + str(self.y) + ")"
    def __repr__(self):
        return str(self.data)
    def GetTreeString(self, tabs=0):
        s = tabs * '\t' + str(self) + '\n'
        for kid in self.kids:
            s = s + kid.GetTreeString(tabs + 1)
        return s


class Editor:
    def __init__(self, tree):
        self.tree = tree
    def Edit(self, node):
        raise NotImplementedError
    def EndEdit(self, node, commit):
        raise NotImplementedError
    def CanEdit(self, node):
        raise NotImplementedError

class LayoutEngine:
    """
    Interface for layout engines.
    """
    def __init__(self, tree):
        self.tree = tree
    def layout(self, node):
        raise NotImplementedError

class Transform:
    """
    Transform interface.
    """
    def __init__(self, tree):
        self.tree = tree
    def transform(self, node, offset, rotation):
        """
        This method should only change the projx and projy attributes of
        the node. These represent the position of the node as it should
        be drawn on screen. Adjusting the x and y attributes can and
        should cause havoc.
        """
        raise NotImplementedError

class Painter:
    """
    This is the interface that wxMVCTree expects from painters. All painters should
    be Painter subclasses.
    """
    def __init__(self, tree):
        self.tree = tree
        self.textcolor = wxNamedColour("BLACK")
        self.bgcolor = wxNamedColour("WHITE")
        self.fgcolor = wxNamedColour("BLUE")
        self.linecolor = wxNamedColour("GREY")
        self.font = wxFont(9, wxDEFAULT, wxNORMAL, wxNORMAL, false)
        self.knobs = []
        self.rectangles = []
        self.minx = self.maxx = self.miny = self.maxy = 0

    def GetFont(self):
        return self.font

    def SetFont(self, font):
        self.font = font
        self.tree.Refresh()

    def paint(self, dc, node):
        raise NotImplementedError
    def GetTextColour(self):
        return self.textcolor
    def SetTextColour(self, color):
        self.textcolor = color
        self.textbrush = wxBrush(color)
        self.textpen = wxPen(color, 1, wxSOLID)
    def GetBackgroundColour(self):
        return self.bgcolor
    def SetBackgroundColour(self, color):
        self.bgcolor = color
        self.bgbrush = wxBrush(color)
        self.bgpen = wxPen(color, 1, wxSOLID)
    def GetForegroundColour(self):
        return self.fgcolor
    def SetForegroundColour(self, color):
        self.fgcolor = color
        self.fgbrush = wxBrush(color)
        self.fgpen = wxPen(color, 1, wxSOLID)
    def GetLineColour(self):
        return self.linecolor
    def SetLineColour(self, color):
        self.linecolor = color
        self.linebrush = wxBrush(color)
        self.linepen = wxPen( color, 1, wxSOLID)
    def GetForegroundPen(self):
        return self.fgpen
    def GetBackgroundPen(self):
        return self.bgpen
    def GetTextPen(self):
        return self.textpen
    def GetForegroundBrush(self):
        return self.fgbrush
    def GetBackgroundBrush(self):
        return self.bgbrush
    def GetTextBrush(self):
        return self.textbrush
    def GetLinePen(self):
        return self.linepen
    def GetLineBrush(self):
        return self.linebrush
    def OnMouse(self, evt):
        if evt.LeftDClick():
            for item in self.rectangles:
                if item[1].contains((evt.GetX(), evt.GetY())):
                    self.tree.Edit(item[0].data)
                    self.tree.OnNodeClick(item[0], evt)
                    return
        elif evt.ButtonDown():
            #self.oldpos = (evt.GetX(), evt.GetY())
            for item in self.rectangles:
                if item[1].contains((evt.GetX(), evt.GetY())):
                    self.tree.OnNodeClick(item[0], evt)
                    return
            for item in self.knobs:
                if item[1].contains((evt.GetX(), evt.GetY())):
                    self.tree.OnKnobClick(item[0])
                    return
        evt.Skip()


class wxTreeModel:
    """
    Interface for tree models
    """
    def GetRoot(self):
        raise NotImplementedError
    def SetRoot(self, root):
        raise NotImplementedError
    def GetChildCount(self, node):
        raise NotImplementedError
    def GetChildAt(self, node, index):
        raise NotImplementedError
    def GetParent(self, node):
        raise NotImplementedError
    def AddChild(self, parent, child):
        if hasattr(self, 'tree') and self.tree:
            self.tree.NodeAdded(parent, child)
    def RemoveNode(self, child):
        if hasattr(self, 'tree') and self.tree:
            self.tree.NodeRemoved(child)
    def InsertChild(self, parent, child, index):
        if hasattr(self, 'tree') and self.tree:
            self.tree.NodeInserted(parent, child, index)
    def IsLeaf(self, node):
        raise NotImplementedError

    def IsEditable(self, node):
        return false

    def SetEditable(self, node):
        return false

class NodePainter:
    """
    This is the interface expected of a nodepainter.
    """
    def __init__(self, painter):
        self.painter = painter
    def paint(self, node, dc, location = None):
        """
        location should be provided only to draw in an unusual position
        (not the node's normal position), otherwise the node's projected x and y
        coordinates will be used.
        """
        raise NotImplementedError

class LinePainter:
    """
    The linepainter interface.
    """
    def __init__(self, painter):
        self.painter = painter
    def paint(self, parent, child, dc):
        raise NotImplementedError

class TextConverter:
    """
    TextConverter interface.
    """
    def __init__(self, painter):
        self.painter = painter
    def convert(node):
        """
        Should return a string. The node argument will be an
        MVCTreeNode.
        """
        raise NotImplementedError


class BasicTreeModel(wxTreeModel):
    """
    A very simple treemodel implementation, but flexible enough for many needs.
    """
    def __init__(self):
        self.children = {}
        self.parents = {}
        self.root = None
    def GetRoot(self):
        return self.root
    def SetRoot(self, root):
        self.root = root
    def GetChildCount(self, node):
        if self.children.has_key(node):
            return len(self.children[node])
        else:
            return 0
    def GetChildAt(self, node, index):
        return self.children[node][index]

    def GetParent(self, node):
        return self.parents[node]

    def AddChild(self, parent, child):
        self.parents[child]=parent
        if not self.children.has_key(parent):
            self.children[parent]=[]
        self.children[parent].append(child)
        wxTreeModel.AddChild(self, parent, child)
        return child

    def RemoveNode(self, node):
        parent = self.parents[node]
        del self.parents[node]
        self.children[parent].remove(node)
        wxTreeModel.RemoveNode(self, node)

    def InsertChild(self, parent, child, index):
        self.parents[child]=parent
        if not self.children.has_key(parent):
            self.children[parent]=[]
        self.children[parent].insert(child, index)
        wxTreeModel.InsertChild(self, parent, child, index)
        return child

    def IsLeaf(self, node):
        return not self.children.has_key(node)

    def IsEditable(self, node):
        return false

    def SetEditable(self, node, bool):
        return false


class FileEditor(Editor):
    def Edit(self, node):
        treenode = self.tree.nodemap[node]
        self.editcomp = wxTextCtrl(self.tree, -1)
        for rect in self.tree.painter.rectangles:
            if rect[0] == treenode:
                self.editcomp.SetPosition((rect[1][0], rect[1][1]))
                break
        self.editcomp.SetValue(node.fileName)
        self.editcomp.SetSelection(0, len(node.fileName))
        self.editcomp.SetFocus()
        self.treenode = treenode
        EVT_KEY_DOWN(self.editcomp, self._key)
        EVT_LEFT_DOWN(self.editcomp, self._mdown)
        self.editcomp.CaptureMouse()

    def CanEdit(self, node):
        return isinstance(node, FileWrapper)

    def EndEdit(self, commit):
        if not self.tree._EditEnding(self.treenode.data):
            return
        if commit:
            node = self.treenode.data
            try:
                os.rename(node.path + os.sep + node.fileName, node.path + os.sep + self.editcomp.GetValue())
                node.fileName = self.editcomp.GetValue()
            except:
                import traceback;traceback.print_exc()
        self.editcomp.ReleaseMouse()
        self.editcomp.Destroy()
        del self.editcomp


    def _key(self, evt):
        if evt.KeyCode() == WXK_RETURN:
            self.EndEdit(true)
        elif evt.KeyCode() == WXK_ESCAPE:
            self.EndEdit(false)
        else:
            evt.Skip()

    def _mdown(self, evt):
        if evt.IsButton():
            pos = evt.GetPosition()
            print pos.x, pos.y
            edsize = self.editcomp.GetSize()
            if pos.x < 0 or pos.y < 0 or pos.x > edsize.width or pos.y > edsize.height:
                self.EndEdit(false)


class FileWrapper:
    """
    Node class for FSTreeModel.
    """
    def __init__(self, path, fileName):
        self.path = path
        self.fileName = fileName

    def __str__(self):
        return self.fileName

class FSTreeModel(BasicTreeModel):
    """
    This treemodel models the filesystem starting from a given path.
    """
    def __init__(self, path):
        BasicTreeModel.__init__(self)
        import string
        fw = FileWrapper(path, string.split(path, os.sep)[-1])
        self._Build(path, fw)
        self.SetRoot(fw)
        self._editable = true
    def _Build(self, path, fileWrapper):
        for name in os.listdir(path):
            fw = FileWrapper(path, name)
            self.AddChild(fileWrapper, fw)
            childName = path + os.sep + name
            if os.path.isdir(childName):
                self._Build(childName, fw)

    def IsEditable(self, node):
        return self._editable

    def SetEditable(self, node, bool):
        self._editable = bool

class LateFSTreeModel(FSTreeModel):
    """
    This treemodel models the filesystem starting from a given path.
    It retrieves the directory list as requested.
    """
    def __init__(self, path):
        BasicTreeModel.__init__(self)
        import string
        name = string.split(path, os.sep)[-1]
        pathpart = path[:-len(name)]
        print pathpart
        fw = FileWrapper(pathpart, name)
        self._Build(path, fw)
        self.SetRoot(fw)
        self._editable = true
        self.children = {}
        self.parents = {}
    def _Build(self, path, parent):
        ppath = parent.path + os.sep + parent.fileName
        if not os.path.isdir(ppath):
            return
        for name in os.listdir(ppath):
            fw = FileWrapper(ppath, name)
            self.AddChild(parent, fw)
    def GetChildCount(self, node):
        if self.children.has_key(node):
            return FSTreeModel.GetChildCount(self, node)
        else:
            self._Build(node.path, node)
            return FSTreeModel.GetChildCount(self, node)

    def IsLeaf(self, node):
        return not os.path.isdir(node.path + os.sep + node.fileName)

class StrTextConverter(TextConverter):
    def convert(self, node):
        return str(node.data)

class NullTransform(Transform):
    def transform(self, node, offset, rotation):
        node.projx = node.x + offset[0]
        node.projy = node.y + offset[1]
        for kid in node.kids:
            self.transform(kid, offset, rotation)

class Rect:
    def __init__(self, x, y, width, height):
        self.x = x
        self.y = y
        self.width = width
        self.height = height
    def __getitem__(self, index):
        return (self.x, self.y, self.width, self.height)[index]

    def __setitem__(self, index, value):
        name = ['x', 'y', 'width', 'height'][index]
        setattr(self, name, value)

    def contains(self, other):
        if type(other) == type(()):
            other = Rect(other[0], other[1], 0, 0)
        if other.x >= self.x:
            if other.y >= self.y:
                if other.width + other.x <= self.width + self.x:
                    if other.height + other.y  <= self.height + self.y:
                        return true
        return false

    def __str__(self):
        return "Rect: " + str([self.x, self.y, self.width, self.height])

class TreeLayout(LayoutEngine):
    def SetHeight(self, num):
        self.NODE_HEIGHT = num

    def __init__(self, tree):
        LayoutEngine.__init__(self, tree)
        self.NODE_STEP = 20
        self.NODE_HEIGHT = 20
    def layout(self, node):
        if node == self.tree.currentRoot:
            node.level = 1
            self.lastY = (-self.NODE_HEIGHT)
        node.x = self.NODE_STEP * node.level
        node.y = self.lastY + self.NODE_HEIGHT
        self.lastY = node.y
        if node.expanded:
            for kid in node.kids:
                kid.level = node.level + 1
                self.layout(kid)

class TreePainter(Painter):
    """
    The default painter class. Uses double-buffering, delegates the painting of nodes and
    lines to helper classes deriving from NodePainter and LinePainter.
    """
    def __init__(self, tree, nodePainter = None, linePainter = None, textConverter = None):
        Painter.__init__(self, tree)
        if not nodePainter:
            nodePainter = TreeNodePainter(self)
        self.nodePainter = nodePainter
        if not linePainter:
            linePainter = TreeLinePainter(self)
        self.linePainter = linePainter
        if not textConverter:
            textConverter = StrTextConverter(self)
        self.textConverter = textConverter
        self.charWidths = []

    def paint(self, dc, node):
        if not self.charWidths:
            self.charWidths = []
            for i in range(25):
                self.charWidths.append(dc.GetTextExtent("D")[0] * i)
            self.charHeight = dc.GetTextExtent("D")[1]
        self.textpen = wxPen(self.GetTextColour(), 1, wxSOLID)
        self.fgpen = wxPen(self.GetForegroundColour(), 1, wxSOLID)
        self.bgpen = wxPen(self.GetBackgroundColour(), 1, wxSOLID)
        self.linepen = wxPen(self.GetLineColour(), 1, wxSOLID)
        self.dashpen = wxPen(self.GetLineColour(), 1, wxDOT)
        self.textbrush = wxBrush(self.GetTextColour(), wxSOLID)
        self.fgbrush = wxBrush(self.GetForegroundColour(), wxSOLID)
        self.bgbrush = wxBrush(self.GetBackgroundColour(), wxSOLID)
        self.linebrush = wxPen(self.GetLineColour(), 1, wxSOLID)
        self.rectangles = []
        self.knobs = []
        dc.BeginDrawing()
        dc.SetPen(self.GetBackgroundPen())
        dc.SetBrush(self.GetBackgroundBrush())
        size = self.tree.GetSize()
        dc.DrawRectangle(0, 0, size.width, size.height)
        if node:
            self.paintWalk(node, dc)
        dc.EndDrawing()

    def GetDashPen(self):
        return self.dashpen

    def SetLinePen(self, pen):
        Painter.SetLinePen(self, pen)
        self.dashpen = wxPen(pen.GetColour(), 1, wxDOT)

    def drawBox(self, px, py, node, dc):
        if self.tree.model.IsLeaf(node.data) or ((node.expanded or not self.tree._assumeChildren) and not len(node.kids)):
            return
        dc.SetPen(self.linepen)
        dc.SetBrush(self.bgbrush)
        dc.DrawRectangle(px -4, py-4, 9, 9)
        self.knobs.append(node, Rect(px -4, py -4, 9, 9))
        dc.SetPen(self.textpen)
        if not node.expanded:
            dc.DrawLine(px, py -2, px, py + 3)
        dc.DrawLine(px -2, py, px + 3, py)

    def paintWalk(self, node, dc):
        self.linePainter.paint(node.parent, node, dc)
        self.nodePainter.paint(node, dc)
        if node.expanded:
            for kid in node.kids:
                if not self.paintWalk(kid, dc):
                    return false
            for kid in node.kids:
                px = (kid.projx - self.tree.layout.NODE_STEP) + 5
                py = kid.projy + kid.height/2
                self.drawBox(px, py, kid, dc)
        if node == self.tree.currentRoot:
            px = (node.projx - self.tree.layout.NODE_STEP) + 5
            py = node.projy + node.height/2
            self.drawBox(px, py, node, dc)
        return true

    def OnMouse(self, evt):
        Painter.OnMouse(self, evt)

class TreeNodePainter(NodePainter):
    def paint(self, node, dc, location = None):
        text = self.painter.textConverter.convert(node)
        extent = dc.GetTextExtent(text)
        node.width = extent[0]
        node.height = extent[1]
        if node == self.painter.tree.currentRoot:
            self.painter.minx = self.painter.maxx = self.painter.miny = self.painter.maxy = 0
        if node.projx < self.painter.minx:
            self.painter.minx = node.projx
        elif node.projx + node.width > self.painter.maxx:
            self.painter.maxx = node.projx + node.width
        if node.projy < self.painter.miny:
            self.painter.miny = node.projy
        elif node.projy + node.height > self.painter.maxy:
            self.painter.maxy = node.projy + node.height
        if node.selected:
            dc.SetPen(self.painter.GetLinePen())
            dc.SetBrush(self.painter.GetForegroundBrush())
            dc.SetTextForeground(wxNamedColour("WHITE"))
            dc.DrawRectangle(node.projx -1, node.projy -1, node.width + 3, node.height + 3)
        else:
            dc.SetTextForeground(self.painter.GetTextColour())
        dc.DrawText(text, node.projx, node.projy)
        self.painter.rectangles.append((node, Rect(node.projx, node.projy, node.width, node.height)))

class TreeLinePainter(LinePainter):
    def paint(self, parent, child, dc):
        dc.SetPen(self.painter.GetDashPen())
        px = py = cx = cy = 0
        if parent is None or child == self.painter.tree.currentRoot:
            px = (child.projx - self.painter.tree.layout.NODE_STEP) + 5
            py = child.projy + self.painter.tree.layout.NODE_HEIGHT/2
            cx = child.projx
            cy = py
            dc.DrawLine(px, py, cx, cy)
        else:
            px = parent.projx + 5
            py = parent.projy + parent.height
            cx = child.projx -5
            cy = child.projy + self.painter.tree.layout.NODE_HEIGHT/2
            dc.DrawLine(px, py, px, cy)
            dc.DrawLine(px, cy, cx, cy)


wxEVT_MVCTREE_BEGIN_EDIT = 20204 #Start editing. Vetoable.
wxEVT_MVCTREE_END_EDIT = 20205 #Stop editing. Vetoable.
wxEVT_MVCTREE_DELETE_ITEM = 20206 #Item removed from model.
wxEVT_MVCTREE_ITEM_EXPANDED = 20209
wxEVT_MVCTREE_ITEM_EXPANDING = 20210
wxEVT_MVCTREE_ITEM_COLLAPSED = 20211
wxEVT_MVCTREE_ITEM_COLLAPSING = 20212
wxEVT_MVCTREE_SEL_CHANGED = 20213
wxEVT_MVCTREE_SEL_CHANGING = 20214 #Vetoable.
wxEVT_MVCTREE_KEY_DOWN = 20215
wxEVT_MVCTREE_ADD_ITEM = 20216 #Item added to model.

def EVT_MVCTREE_SEL_CHANGED(win, id, func):
    win.Connect(id, -1, wxEVT_MVCTREE_SEL_CHANGED, func)

def EVT_MVCTREE_SEL_CHANGING(win, id, func):
    win.Connect(id, -1, wxEVT_MVCTREE_SEL_CHANGING, func)

def EVT_MVCTREE_ITEM_EXPANDED(win, id, func):
    win.Connect(id, -1, wxEVT_MVCTREE_ITEM_EXPANDED, func)

def EVT_MVCTREE_ITEM_EXPANDING(win, id, func):
    win.Connect(id, -1, wxEVT_MVCTREE_ITEM_EXPANDING, func)

def EVT_MVCTREE_ITEM_COLLAPSED(win, id, func):
    win.Connect(id, -1, wxEVT_MVCTREE_ITEM_COLLAPSED, func)

def EVT_MVCTREE_ITEM_COLLAPSING(win, id, func):
    win.Connect(id, -1, wxEVT_MVCTREE_ITEM_COLLAPSING, func)

def EVT_MVCTREE_ADD_ITEM(win, id, func):
    win.Connect(id, -1, wxEVT_MVCTREE_ADD_ITEM, func)

def EVT_MVCTREE_DELETE_ITEM(win, id, func):
    win.Connect(id, -1, wxEVT_MVCTREE_DELETE_ITEM, func)

def EVT_MVCTREE_KEY_DOWN(win, id, func):
    win.Connect(id, -1, wxEVT_MVCTREE_KEY_DOWN, func)


class wxMVCTreeEvent(wxPyCommandEvent):
    def __init__(self, type, id, node = None, nodes = None, keyEvent = None, **kwargs):
        apply(wxPyCommandEvent.__init__, (self, type, id), kwargs)
        self.node = node
        self.nodes = nodes
        self.keyEvent = keyEvent


class wxMVCTreeNotifyEvent(wxMVCTreeEvent):
    def __init__(self, type, id, node = None, nodes = None, **kwargs):
        apply(wxMVCTreeEvent.__init__, (self, type, id), kwargs)
        self.notify = wxNotifyEvent(type, id)

class wxMVCTree(wxWindow):
    """
    The main mvcTree class.
    """
    def __init__(self, parent, id, model = None, layout = None, transform = None,
                 painter = None, *args, **kwargs):
        apply(wxWindow.__init__, (self, parent, id), kwargs)
        self.nodemap = {}
        self._multiselect = false
        self._selections = []
        self._assumeChildren = false
        self._scrollx = false
        self._scrolly = false
        self.doubleBuffered = true
        self._editors = []
        if not model:
            model = BasicTreeModel()
            model.SetRoot("Root")
        self.SetModel(model)
        if not layout:
            layout = TreeLayout(self)
        self.layout = layout
        if not transform:
           transform = NullTransform(self)
        self.transform = transform
        if not painter:
            painter = TreePainter(self)
        self.painter = painter
        self.SetFont(wxFont(9, wxDEFAULT, wxNORMAL, wxNORMAL, false))
        EVT_MOUSE_EVENTS(self, self.OnMouse)
        EVT_SCROLLWIN(self, self.OnScroll)
        EVT_KEY_DOWN(self, self.OnKeyDown)

    def __repr__(self):
        return "<wxMVCTree instance at %s>" % str(hex(id(self)))

    def __str__(self):
        return self.__repr__()

    def NodeAdded(self, parent, child):
        e = wxMVCTreeEvent(wxEVT_MVCTREE_ADD_ITEM, self.GetId(), node = child, nodes = [parent, child])
        self.GetEventHandler().ProcessEvent(e)

    def NodeInserted(self, parent, child, index):
        e = wxMVCTreeEvent(wxEVT_MVCTREE_ADD_ITEM, self.GetId(), node = child, nodes = [parent, child])
        self.GetEventHandler().ProcessEvent(e)

    def NodeRemoved(self, node):
        e = wxMVCTreeEvent(wxEVT_MVCTREE_DELETE_ITEM, self.GetId(), node = child, nodes = [parent, child])
        self.GetEventHandler().ProcessEvent(e)

    def OnKeyDown(self, evt):
        e = wxMVCTreeEvent(wxEVT_MVCTREE_KEY_DOWN, self.GetId(), keyEvent = evt)
        self.GetEventHandler().ProcessEvent(e)

    def SetFont(self, font):
        self.painter.SetFont(font)
        dc = wxClientDC(self)
        dc.SetFont(font)
        self.layout.SetHeight(dc.GetTextExtent("")[1] + 18)

    def GetFont(self):
        return self.painter.GetFont()

    def AddEditor(self, editor):
        self._editors.append(editor)

    def RemoveEditor(self, editor):
        self._editors.remove(editor)

    def OnMouse(self, evt):
        self.painter.OnMouse(evt)

    def OnNodeClick(self, node, mouseEvent):
        if node.selected:
            self.RemoveFromSelection(node.data)
        else:
            self.AddToSelection(node.data, mouseEvent.ControlDown())
        self.Refresh()

    def OnKnobClick(self, node):
        self.SetExpanded(node.data, not node.expanded)

    def GetDisplayText(self, node):
        treenode = self.nodemap[node]
        return self.painter.textConverter.convert(treenode)

    def IsDoubleBuffered(self):
        return self.doubleBuffered

    def SetDoubleBuffered(self, bool):
        """
        By default wxMVCTree is double-buffered.
        """
        self.doubleBuffered = bool

    def GetModel(self):
        return self.model

    def SetModel(self, model):
        """
        Completely change the data to be displayed.
        """
        self.model = model
        model.tree = self
        self.laidOut = 0
        self.transformed = 0
        self._selections = []
        self.layoutRoot = MVCTreeNode()
        self.layoutRoot.data = self.model.GetRoot()
        self.layoutRoot.expanded = true
        self.LoadChildren(self.layoutRoot)
        self.currentRoot = self.layoutRoot
        self.offset = [0,0]
        self.rotation = 0
        self.Refresh()

    def GetCurrentRoot(self):
        return self.currentRoot

    def LoadChildren(self, layoutNode):
        if layoutNode.built:
            return
        else:
            self.nodemap[layoutNode.data]=layoutNode
            for i in range(self.GetModel().GetChildCount(layoutNode.data)):
                p = MVCTreeNode("RAW", layoutNode, [])
                layoutNode.Add(p)
                p.data = self.GetModel().GetChildAt(layoutNode.data, i)
                self.nodemap[p.data]=p
            layoutNode.built = true
            if not self._assumeChildren:
                for kid in layoutNode.kids:
                    self.LoadChildren(kid)

    def OnEraseBackground(self, evt):
        pass

    def OnSize(self, evt):
        try:
            size = self.GetSizeTuple()
            self.center = (size[0]/2, size[1]/2)
            del self.bmp
        except:
            pass

    def GetSelection(self):
        "Returns a tuple of selected nodes."
        return tuple(self._selections)

    def SetSelection(self, nodeTuple):
        if type(nodeTuple) != type(()):
            nodeTuple = (nodeTuple,)
        e = wxMVCTreeNotifyEvent(wxEVT_MVCTREE_SEL_CHANGING, self.GetId(), nodeTuple[0], nodes = nodeTuple)
        self.GetEventHandler().ProcessEvent(e)
        if not e.notify.IsAllowed():
            return
        for node in nodeTuple:
            treenode = self.nodemap[node]
            treenode.selected = true
        for node in self._selections:
            treenode = self.nodemap[node]
            node.selected = false
        self._selections = list(nodeTuple)
        e = wxMVCTreeEvent(wxEVT_MVCTREE_SEL_CHANGED, self.GetId(), nodeTuple[0], nodes = nodeTuple)
        self.GetEventHandler().ProcessEvent(e)

    def IsMultiSelect(self):
        return self._multiselect

    def SetMultiSelect(self, bool):
        self._multiselect = bool

    def IsSelected(self, node):
        return self.nodemap[node].selected

    def Edit(self, node):
        if not self.model.IsEditable(node):
            return
        for ed in self._editors:
            if ed.CanEdit(node):
                e = wxMVCTreeNotifyEvent(wxEVT_MVCTREE_BEGIN_EDIT, self.GetId(), node)
                self.GetEventHandler().ProcessEvent(e)
                if not e.notify.IsAllowed():
                    return
                ed.Edit(node)
                self._currentEditor = ed
                break

    def EndEdit(self):
        if self._currentEditor:
            self._currentEditor.EndEdit
            self._currentEditor = None

    def _EditEnding(self, node):
        e = wxMVCTreeNotifyEvent(wxEVT_MVCTREE_END_EDIT, self.GetId(), node)
        self.GetEventHandler().ProcessEvent(e)
        if not e.notify.IsAllowed():
            return false
        self._currentEditor = None
        return true


    def SetExpanded(self, node, bool):
        treenode = self.nodemap[node]
        if bool:
            e = wxMVCTreeNotifyEvent(wxEVT_MVCTREE_ITEM_EXPANDING, self.GetId(), node)
            self.GetEventHandler().ProcessEvent(e)
            if not e.notify.IsAllowed():
                return
            if not treenode.built:
                self.LoadChildren(treenode)
        else:
            e = wxMVCTreeNotifyEvent(wxEVT_MVCTREE_ITEM_COLLAPSING, self.GetId(), node)
            self.GetEventHandler().ProcessEvent(e)
            if not e.notify.IsAllowed():
                return
        treenode.expanded = bool
        e = None
        if treenode.expanded:
            e = wxMVCTreeEvent(wxEVT_MVCTREE_ITEM_EXPANDED, self.GetId(), node)
        else:
            e = wxMVCTreeEvent(wxEVT_MVCTREE_ITEM_COLLAPSED, self.GetId(), node)
        self.GetEventHandler().ProcessEvent(e)
        self.layout.layout(self.currentRoot)
        self.transform.transform(self.currentRoot, self.offset, self.rotation)
        self.Refresh()

    def IsExpanded(self, node):
        return self.nodemap[node].expanded

    def AddToSelection(self, nodeOrTuple, enableMulti = true):
        nodeTuple = nodeOrTuple
        if type(nodeOrTuple)!= type(()):
            nodeTuple = (nodeOrTuple,)
        e = wxMVCTreeNotifyEvent(wxEVT_MVCTREE_SEL_CHANGING, self.GetId(), nodeTuple[0], nodes = nodeTuple)
        self.GetEventHandler().ProcessEvent(e)
        if not e.notify.IsAllowed():
            return
        if not self.IsMultiSelect() or not enableMulti:
            for node in self._selections:
                treenode = self.nodemap[node]
                treenode.selected = false
            node = nodeTuple[0]
            self._selections = [node]
            treenode = self.nodemap[node]
            treenode.selected = true
        else:
            for node in nodeTuple:
                try:
                    self._selections.index(node)
                except ValueError:
                    self._selections.append(node)
                    treenode = self.nodemap[node]
                    treenode.selected = true
        e = wxMVCTreeEvent(wxEVT_MVCTREE_SEL_CHANGED, self.GetId(), nodeTuple[0], nodes = nodeTuple)
        self.GetEventHandler().ProcessEvent(e)

    def RemoveFromSelection(self, nodeTuple):
        if type(nodeTuple) != type(()):
            nodeTuple = (nodeTuple,)
        for node in nodeTuple:
            try:
                self._selections.index(node)
            except IndexError:
                self._selections.remove(node)
                treenode = self.nodemap[node]
                node.selected = false
        e = wxMVCTreeEvent(wxEVT_MVCTREE_SEL_CHANGED, self.GetId(), node, nodes = nodeTuple)
        self.GetEventHandler().ProcessEvent(e)



    def GetBackgroundColour(self):
        if hasattr(self, 'painter') and self.painter:
            return self.painter.GetBackgroundColour()
        else:
            return wxWindow.GetBackgroundColour(self)
    def SetBackgroundColour(self, color):
        if hasattr(self, 'painter') and self.painter:
            self.painter.SetBackgroundColour(color)
        else:
            wxWindow.SetBackgroundColour(self, color)
    def GetForegroundColour(self):
        if hasattr(self, 'painter') and self.painter:
            return self.painter.GetForegroundColour()
        else:
            return wxWindow.GetBackgroundColour(self)
    def SetForegroundColour(self, color):
        if hasattr(self, 'painter') and self.painter:
            self.painter.SetForegroundColour(color)
        else:
            wxWindow.SetBackgroundColour(self, color)

    def SetAssumeChildren(self, bool):
        self._assumeChildren = bool

    def GetAssumeChildren(self):
        return self._assumeChildren

    def OnScroll(self, evt):
        type = evt.GetEventType()
        field = [self.painter.maxx - self.painter.minx, self.painter.maxy - self.painter.miny]
        size = self.GetSizeTuple()
        index = 1
        if evt.GetOrientation() == wxHORIZONTAL:
            index = 0
            self._scrollx = true
        else:
            self._scrolly = true
            index = 1
        if type ==  wxEVT_SCROLLWIN_TOP:
            self.offset[index] = 0
        elif type == wxEVT_SCROLLWIN_LINEUP:
            self.offset[index] = self.offset[index] + 1
        elif type == wxEVT_SCROLLWIN_LINEDOWN:
            self.offset[index] = self.offset[index] - 1
        elif type == wxEVT_SCROLLWIN_PAGEUP:
            self.offset[index] = self.offset[index] + int(20 * float(field[index])/float(size[index]))
        elif type == wxEVT_SCROLLWIN_PAGEDOWN:
            self.offset[index] = self.offset[index] - int(20 * float(field[index])/float(size[index]))
        elif type == wxEVT_SCROLLWIN_THUMBTRACK:
            self.offset[index] = -(evt.GetPosition())
        elif type == wxEVT_SCROLLWIN_BOTTOM:
            self.offset[index] = field[index]
        self.transformed = false
        self.Refresh()

    def OnPaint(self, evt):
        """
        Ensures that the tree has been laid out and transformed, then calls the painter
        to paint the control.
        """
        try:
            if not self.laidOut:
                self.layout.layout(self.currentRoot)
                self.laidOut = true
            if not self.transformed:
                self.transform.transform(self.currentRoot, self.offset, self.rotation)
                self.transformed = true
            dc = wxPaintDC(self)
            dc.SetFont(self.GetFont())
            if self.doubleBuffered:
                size = self.GetSize()
                if not hasattr(self, 'bmp'):
                    self.bmp = bmp =wxEmptyBitmap(size.width, size.height)
                else:
                    bmp = self.bmp
                mem_dc = wxMemoryDC()
                mem_dc.SetFont(self.GetFont())
                mem_dc.SelectObject(bmp)
                self.painter.paint(mem_dc, self.currentRoot)
                dc.Blit(0, 0, size.width, size.height, mem_dc, 0, 0);
            else:
                self.painter.paint(dc, self.currentRoot)
            size = self.GetSizeTuple()
            if self._scrollx or self.painter.minx < 0 or self.painter.maxx > size[0]:
                field = self.painter.maxx - self.painter.minx
                self.SetScrollbar(wxHORIZONTAL, -self.offset[0], size[0]/field, field, true)
                self._scrollx = false
            if self._scrolly or self.painter.miny < 0 or self.painter.maxy > size[1]:
                field = self.painter.maxy - self.painter.miny
                self.SetScrollbar(wxVERTICAL, -self.offset[1], size[1]/field, field, true)
                self._scrolly = false
        except:
            import traceback;traceback.print_exc()

if __name__ == '__main__':
    def exit(evt):
        import sys;sys.exit()

    block = 0

    def selchanging(evt):
        print "SelChanging!"
        print evt.node
        global block
        if block:
            evt.notify.Veto()
        block = not block

    def selchanged(evt):
        print "SelChange!"
        print evt.node
    def expanded(evt):
        print "Expanded!"
    def closed(evt):
        print "Closed!"
    def key(evt):
        print "Key"
    def add(evt):
        print "Add"
    def delitem(evt):
        print "Delete"

    class MyApp(wxApp):
        def OnInit(self):
            f = wxFrame(NULL, -1, "wxMVCTree")
            p = None
            p = wxMVCTree(f, -1)
            p.SetAssumeChildren(true)
            if len(sys.argv) > 1:
                p.SetModel(LateFSTreeModel(sys.argv[1]))
                p.AddEditor(FileEditor(p))
            p.SetMultiSelect(true)
            f.Show(true)
            EVT_CLOSE(f, exit)
            EVT_MVCTREE_SEL_CHANGED(p, p.GetId(), selchanged)
            EVT_MVCTREE_SEL_CHANGING(p, p.GetId(), selchanging)
            EVT_MVCTREE_ITEM_EXPANDED(p, p.GetId(), expanded)
            EVT_MVCTREE_ITEM_COLLAPSED(p, p.GetId(), closed)
            EVT_MVCTREE_ADD_ITEM(p, p.GetId(), add)
            EVT_MVCTREE_DELETE_ITEM(p, p.GetId(), delitem)
            EVT_MVCTREE_KEY_DOWN(p, p.GetId(), key)
            p.SetForegroundColour(wxNamedColour("GREEN"))
            self.SetTopWindow(f)
            return true

    app = MyApp(false)
    app.MainLoop()











