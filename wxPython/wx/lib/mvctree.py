# 12/09/2003 - Jeff Grimmett (grimmtooth@softhome.net)
#
# o 2.5 compatability update.
# o I'm a little nervous about some of it though.
#
# 12/20/2003 - Jeff Grimmett (grimmtooth@softhome.net)
#
# o wxTreeModel -> TreeModel
# o wxMVCTree -> MVCTree
# o wxMVCTreeEvent -> MVCTreeEvent
# o wxMVCTreeNotifyEvent -> MVCTreeNotifyEvent
#

"""
MVCTree is a control which handles hierarchical data. It is constructed
in model-view-controller architecture, so the display of that data, and
the content of the data can be changed greatly without affecting the other parts.

MVCTree actually is even more configurable than MVC normally implies, because
almost every aspect of it is pluggable:
    MVCTree - Overall controller, and the window that actually gets placed
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


NOTE: This module is *not* supported in any way.  Use it however you
      wish, but be warned that dealing with any consequences is
      entirly up to you.
      --Robin
"""

#------------------------------------------------------------------------
import  os
import  sys
import  traceback
import  warnings

import  wx
#------------------------------------------------------------------------

warningmsg = r"""\

################################################\
# This module is not supported in any way!      |
#                                               |
# See cource code for wx.lib.mvctree for more   |
# information.                                  |
################################################/

"""

warnings.warn(warningmsg, DeprecationWarning, stacklevel=2)
#------------------------------------------------------------------------

class MVCTreeNode:
    """
    Used internally by MVCTree to manage its data. Contains information about
    screen placement, the actual data associated with it, and more. These are
    the nodes passed to all the other helper parts to do their work with.
    """
    def __init__(self, data=None, parent = None, kids = None, x = 0, y = 0):
        self.x = 0
        self.y = 0
        self.projx = 0
        self.projy = 0
        self.parent = parent
        self.kids = kids
        if self.kids is None:
            self.kids = []
        self.data = data
        self.expanded = False
        self.selected = False
        self.built = False
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
    def Layout(self, node):
        raise NotImplementedError
    def GetNodeList(self):
        raise NotImplementedError

class Transform:
    """
    Transform interface.
    """
    def __init__(self, tree):
        self.tree = tree
    def Transform(self, node, offset, rotation):
        """
        This method should only change the projx and projy attributes of
        the node. These represent the position of the node as it should
        be drawn on screen. Adjusting the x and y attributes can and
        should cause havoc.
        """
        raise NotImplementedError

    def GetSize(self):
        """
        Returns the size of the entire tree as laid out and transformed
        as a tuple
        """
        raise NotImplementedError

class Painter:
    """
    This is the interface that MVCTree expects from painters. All painters should
    be Painter subclasses.
    """
    def __init__(self, tree):
        self.tree = tree
        self.textcolor = wx.NamedColour("BLACK")
        self.bgcolor = wx.NamedColour("WHITE")
        self.fgcolor = wx.NamedColour("BLUE")
        self.linecolor = wx.NamedColour("GREY")
        self.font = wx.Font(9, wx.DEFAULT, wx.NORMAL, wx.NORMAL, False)
        self.bmp = None

    def GetFont(self):
        return self.font

    def SetFont(self, font):
        self.font = font
        self.tree.Refresh()
    def GetBuffer(self):
        return self.bmp
    def ClearBuffer(self):
        self.bmp = None
    def Paint(self, dc, node, doubleBuffered=1, paintBackground=1):
        raise NotImplementedError
    def GetTextColour(self):
        return self.textcolor
    def SetTextColour(self, color):
        self.textcolor = color
        self.textbrush = wx.Brush(color)
        self.textpen = wx.Pen(color, 1, wx.SOLID)
    def GetBackgroundColour(self):
        return self.bgcolor
    def SetBackgroundColour(self, color):
        self.bgcolor = color
        self.bgbrush = wx.Brush(color)
        self.bgpen = wx.Pen(color, 1, wx.SOLID)
    def GetForegroundColour(self):
        return self.fgcolor
    def SetForegroundColour(self, color):
        self.fgcolor = color
        self.fgbrush = wx.Brush(color)
        self.fgpen = wx.Pen(color, 1, wx.SOLID)
    def GetLineColour(self):
        return self.linecolor
    def SetLineColour(self, color):
        self.linecolor = color
        self.linebrush = wx.Brush(color)
        self.linepen = wx.Pen( color, 1, wx.SOLID)
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
            x, y = self.tree.CalcUnscrolledPosition(evt.GetX(), evt.GetY())
            for item in self.rectangles:
                if item[1].Contains((x,y)):
                    self.tree.Edit(item[0].data)
                    self.tree.OnNodeClick(item[0], evt)
                    return
        elif evt.ButtonDown():
            x, y = self.tree.CalcUnscrolledPosition(evt.GetX(), evt.GetY())
            for item in self.rectangles:
                if item[1].Contains((x, y)):
                    self.tree.OnNodeClick(item[0], evt)
                    return
            for item in self.knobs:
                if item[1].Contains((x, y)):
                    self.tree.OnKnobClick(item[0])
                    return
        evt.Skip()


class TreeModel:
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
        return False

    def SetEditable(self, node):
        return False

class NodePainter:
    """
    This is the interface expected of a nodepainter.
    """
    def __init__(self, painter):
        self.painter = painter
    def Paint(self, node, dc, location = None):
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
    def Paint(self, parent, child, dc):
        raise NotImplementedError

class TextConverter:
    """
    TextConverter interface.
    """
    def __init__(self, painter):
        self.painter = painter
    def Convert(node):
        """
        Should return a string. The node argument will be an
        MVCTreeNode.
        """
        raise NotImplementedError


class BasicTreeModel(TreeModel):
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
        TreeModel.AddChild(self, parent, child)
        return child

    def RemoveNode(self, node):
        parent = self.parents[node]
        del self.parents[node]
        self.children[parent].remove(node)
        TreeModel.RemoveNode(self, node)

    def InsertChild(self, parent, child, index):
        self.parents[child]=parent
        if not self.children.has_key(parent):
            self.children[parent]=[]
        self.children[parent].insert(child, index)
        TreeModel.InsertChild(self, parent, child, index)
        return child

    def IsLeaf(self, node):
        return not self.children.has_key(node)

    def IsEditable(self, node):
        return False

    def SetEditable(self, node, bool):
        return False


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
#        self.editcomp.Bind(wx.EVT_KEY_DOWN, self._key)
        self.editcomp.Bind(wx.EVT_KEY_UP, self._key)
        self.editcomp.Bind(wx.EVT_LEFT_DOWN, self._mdown)
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
                traceback.print_exc()
        self.editcomp.ReleaseMouse()
        self.editcomp.Destroy()
        del self.editcomp
        self.tree.Refresh()


    def _key(self, evt):
        if evt.GetKeyCode() == wx.WXK_RETURN:
            self.EndEdit(True)
        elif evt.GetKeyCode() == wx.WXK_ESCAPE:
            self.EndEdit(False)
        else:
            evt.Skip()

    def _mdown(self, evt):
        if evt.IsButton():
            x, y = evt.GetPosition()
            w, h = self.editcomp.GetSize()
            if x < 0 or y < 0 or x > w or y > h:
                self.EndEdit(False)


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
        fw = FileWrapper(path, path.split(os.sep)[-1])
        self._Build(path, fw)
        self.SetRoot(fw)
        self._editable = True
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
        name = path.split(os.sep)[-1]
        pathpart = path[:-len(name)]
        fw = FileWrapper(pathpart, name)
        self._Build(path, fw)
        self.SetRoot(fw)
        self._editable = True
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
    def Convert(self, node):
        return str(node.data)

class NullTransform(Transform):
    def GetSize(self):
        return tuple(self.size)

    def Transform(self, node, offset, rotation):
        self.size = [0,0]
        list = self.tree.GetLayoutEngine().GetNodeList()
        for node in list:
            node.projx = node.x + offset[0]
            node.projy = node.y + offset[1]
            if node.projx > self.size[0]:
                self.size[0] = node.projx
            if node.projy > self.size[1]:
                self.size[1] = node.projy

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

    def Contains(self, other):
        if type(other) == type(()):
            other = Rect(other[0], other[1], 0, 0)
        if other.x >= self.x:
            if other.y >= self.y:
                if other.width + other.x <= self.width + self.x:
                    if other.height + other.y  <= self.height + self.y:
                        return True
        return False

    def __str__(self):
        return "Rect: " + str([self.x, self.y, self.width, self.height])

class TreeLayout(LayoutEngine):
    def SetHeight(self, num):
        self.NODE_HEIGHT = num

    def __init__(self, tree):
        LayoutEngine.__init__(self, tree)
        self.NODE_STEP = 20
        self.NODE_HEIGHT = 20
        self.nodelist = []

    def Layout(self, node):
        self.nodelist = []
        self.NODE_HEIGHT = self.tree.GetFont().GetPointSize() * 2
        self.layoutwalk(node)

    def GetNodeList(self):
        return self.nodelist

    def layoutwalk(self, node):
        if node == self.tree.currentRoot:
            node.level = 1
            self.lastY = (-self.NODE_HEIGHT)
        node.x = self.NODE_STEP * node.level
        node.y = self.lastY + self.NODE_HEIGHT
        self.lastY = node.y
        self.nodelist.append(node)
        if node.expanded:
            for kid in node.kids:
                kid.level = node.level + 1
                self.layoutwalk(kid)

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

    def Paint(self, dc, node, doubleBuffered=1, paintBackground=1):
        if not self.charWidths:
            self.charWidths = []
            for i in range(25):
                self.charWidths.append(dc.GetTextExtent("D")[0] * i)
            self.charHeight = dc.GetTextExtent("D")[1]
        self.textpen = wx.Pen(self.GetTextColour(), 1, wx.SOLID)
        self.fgpen = wx.Pen(self.GetForegroundColour(), 1, wx.SOLID)
        self.bgpen = wx.Pen(self.GetBackgroundColour(), 1, wx.SOLID)
        self.linepen = wx.Pen(self.GetLineColour(), 1, wx.SOLID)
        self.dashpen = wx.Pen(self.GetLineColour(), 1, wx.DOT)
        self.textbrush = wx.Brush(self.GetTextColour(), wx.SOLID)
        self.fgbrush = wx.Brush(self.GetForegroundColour(), wx.SOLID)
        self.bgbrush = wx.Brush(self.GetBackgroundColour(), wx.SOLID)
        self.linebrush = wx.Pen(self.GetLineColour(), 1, wx.SOLID)
        treesize = self.tree.GetSize()
        size = self.tree.transform.GetSize()
        size = (max(treesize.width, size[0]+50), max(treesize.height, size[1]+50))
        dc.BeginDrawing()
        if doubleBuffered:
            mem_dc = wx.MemoryDC()
            if not self.GetBuffer():
                self.knobs = []
                self.rectangles = []
                self.bmp = wx.EmptyBitmap(size[0], size[1])
                mem_dc.SelectObject(self.GetBuffer())
                mem_dc.SetPen(self.GetBackgroundPen())
                mem_dc.SetBrush(self.GetBackgroundBrush())
                mem_dc.DrawRectangle(0, 0, size[0], size[1])
                mem_dc.SetFont(self.tree.GetFont())
                self.paintWalk(node, mem_dc)
            else:
                mem_dc.SelectObject(self.GetBuffer())
            xstart, ystart = self.tree.CalcUnscrolledPosition(0,0)
            size = self.tree.GetClientSizeTuple()
            dc.Blit(xstart, ystart, size[0], size[1], mem_dc, xstart, ystart)
        else:
            if node == self.tree.currentRoot:
                self.knobs = []
                self.rectangles = []
            dc.SetPen(self.GetBackgroundPen())
            dc.SetBrush(self.GetBackgroundBrush())
            dc.SetFont(self.tree.GetFont())
            if paintBackground:
                dc.DrawRectangle(0, 0, size[0], size[1])
            if node:
                #Call with not paintBackground because if we are told not to paint the
                #whole background, we have to paint in parts to undo selection coloring.
                pb = paintBackground
                self.paintWalk(node, dc, not pb)
        dc.EndDrawing()

    def GetDashPen(self):
        return self.dashpen

    def SetLinePen(self, pen):
        Painter.SetLinePen(self, pen)
        self.dashpen = wx.Pen(pen.GetColour(), 1, wx.DOT)

    def paintWalk(self, node, dc, paintRects=0):
        self.linePainter.Paint(node.parent, node, dc)
        self.nodePainter.Paint(node, dc, drawRects = paintRects)
        if node.expanded:
            for kid in node.kids:
                if not self.paintWalk(kid, dc, paintRects):
                    return False
            for kid in node.kids:
                px = (kid.projx - self.tree.layout.NODE_STEP) + 5
                py = kid.projy + kid.height/2
                if (not self.tree.model.IsLeaf(kid.data)) or ((kid.expanded or self.tree._assumeChildren) and len(kid.kids)):
                    dc.SetPen(self.linepen)
                    dc.SetBrush(self.bgbrush)
                    dc.DrawRectangle(px -4, py-4, 9, 9)
                    self.knobs.append( (kid, Rect(px -4, py -4, 9, 9)) )
                    dc.SetPen(self.textpen)
                    if not kid.expanded:
                        dc.DrawLine(px, py -2, px, py + 3)
                    dc.DrawLine(px -2, py, px + 3, py)
        if node == self.tree.currentRoot:
            px = (node.projx - self.tree.layout.NODE_STEP) + 5
            py = node.projy + node.height/2
            dc.SetPen(self.linepen)
            dc.SetBrush(self.bgbrush)
            dc.DrawRectangle(px -4, py-4, 9, 9)
            self.knobs.append( (node, Rect(px -4, py -4, 9, 9)) )
            dc.SetPen(self.textpen)
            if not node.expanded:
                dc.DrawLine(px, py -2, px, py + 3)
            dc.DrawLine(px -2, py, px + 3, py)
        return True

    def OnMouse(self, evt):
        Painter.OnMouse(self, evt)

class TreeNodePainter(NodePainter):
    def Paint(self, node, dc, location = None, drawRects = 0):
        text = self.painter.textConverter.Convert(node)
        extent = dc.GetTextExtent(text)
        node.width = extent[0]
        node.height = extent[1]
        if node.selected:
            dc.SetPen(self.painter.GetLinePen())
            dc.SetBrush(self.painter.GetForegroundBrush())
            dc.SetTextForeground(wx.NamedColour("WHITE"))
            dc.DrawRectangle(node.projx -1, node.projy -1, node.width + 3, node.height + 3)
        else:
            if drawRects:
                dc.SetBrush(self.painter.GetBackgroundBrush())
                dc.SetPen(self.painter.GetBackgroundPen())
                dc.DrawRectangle(node.projx -1, node.projy -1, node.width + 3, node.height + 3)
            dc.SetTextForeground(self.painter.GetTextColour())
        dc.DrawText(text, node.projx, node.projy)
        self.painter.rectangles.append((node, Rect(node.projx, node.projy, node.width, node.height)))

class TreeLinePainter(LinePainter):
    def Paint(self, parent, child, dc):
        dc.SetPen(self.painter.GetDashPen())
        px = py = cx = cy = 0
        if parent is None or child == self.painter.tree.currentRoot:
            px = (child.projx - self.painter.tree.layout.NODE_STEP) + 5
            py = child.projy + self.painter.tree.layout.NODE_HEIGHT/2 -2
            cx = child.projx
            cy = py
            dc.DrawLine(px, py, cx, cy)
        else:
            px = parent.projx + 5
            py = parent.projy + parent.height
            cx = child.projx -5
            cy = child.projy + self.painter.tree.layout.NODE_HEIGHT/2 -3
            dc.DrawLine(px, py, px, cy)
            dc.DrawLine(px, cy, cx, cy)

#>> Event defs
wxEVT_MVCTREE_BEGIN_EDIT = wx.NewEventType() #Start editing. Vetoable.
wxEVT_MVCTREE_END_EDIT = wx.NewEventType() #Stop editing. Vetoable.
wxEVT_MVCTREE_DELETE_ITEM = wx.NewEventType() #Item removed from model.
wxEVT_MVCTREE_ITEM_EXPANDED = wx.NewEventType()
wxEVT_MVCTREE_ITEM_EXPANDING = wx.NewEventType()
wxEVT_MVCTREE_ITEM_COLLAPSED = wx.NewEventType()
wxEVT_MVCTREE_ITEM_COLLAPSING = wx.NewEventType()
wxEVT_MVCTREE_SEL_CHANGED = wx.NewEventType()
wxEVT_MVCTREE_SEL_CHANGING = wx.NewEventType() #Vetoable.
wxEVT_MVCTREE_KEY_DOWN = wx.NewEventType()
wxEVT_MVCTREE_ADD_ITEM = wx.NewEventType() #Item added to model.

EVT_MVCTREE_SEL_CHANGED = wx.PyEventBinder(wxEVT_MVCTREE_SEL_CHANGED, 1)
EVT_MVCTREE_SEL_CHANGING = wx.PyEventBinder(wxEVT_MVCTREE_SEL_CHANGING, 1)
EVT_MVCTREE_ITEM_EXPANDED = wx.PyEventBinder(wxEVT_MVCTREE_ITEM_EXPANDED, 1)
EVT_MVCTREE_ITEM_EXPANDING = wx.PyEventBinder(wxEVT_MVCTREE_ITEM_EXPANDING, 1)
EVT_MVCTREE_ITEM_COLLAPSED = wx.PyEventBinder(wxEVT_MVCTREE_ITEM_COLLAPSED, 1)
EVT_MVCTREE_ITEM_COLLAPSING = wx.PyEventBinder(wxEVT_MVCTREE_ITEM_COLLAPSING, 1)
EVT_MVCTREE_ADD_ITEM = wx.PyEventBinder(wxEVT_MVCTREE_ADD_ITEM, 1)
EVT_MVCTREE_DELETE_ITEM = wx.PyEventBinder(wxEVT_MVCTREE_DELETE_ITEM, 1)
EVT_MVCTREE_KEY_DOWN = wx.PyEventBinder(wxEVT_MVCTREE_KEY_DOWN, 1)

class MVCTreeEvent(wx.PyCommandEvent):
    def __init__(self, type, id, node = None, nodes = None, keyEvent = None, **kwargs):
        apply(wx.PyCommandEvent.__init__, (self, type, id), kwargs)
        self.node = node
        self.nodes = nodes
        self.keyEvent = keyEvent
    def GetNode(self):
        return self.node
    def GetNodes(self):
        return self.nodes
    def getKeyEvent(self):
        return self.keyEvent

class MVCTreeNotifyEvent(MVCTreeEvent):
    def __init__(self, type, id, node = None, nodes = None, **kwargs):
        apply(MVCTreeEvent.__init__, (self, type, id, node, nodes), kwargs)
        self.notify = wx.NotifyEvent(type, id)
    def getNotifyEvent(self):
        return self.notify

class MVCTree(wx.ScrolledWindow):
    """
    The main mvc tree class.
    """
    def __init__(self, parent, id, model = None, layout = None, transform = None,
                 painter = None, *args, **kwargs):
        apply(wx.ScrolledWindow.__init__, (self, parent, id), kwargs)
        self.nodemap = {}
        self._multiselect = False
        self._selections = []
        self._assumeChildren = False
        self._scrollx = False
        self._scrolly = False
        self.doubleBuffered = False
        self._lastPhysicalSize = self.GetSize()
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
        self.SetFont(wx.Font(9, wx.DEFAULT, wx.NORMAL, wx.NORMAL, False))
        self.Bind(wx.EVT_MOUSE_EVENTS, self.OnMouse)
        self.Bind(wx.EVT_KEY_DOWN, self.OnKeyDown)
        self.doubleBuffered = True
        self.Bind(wx.EVT_SIZE, self.OnSize)
        self.Bind(wx.EVT_ERASE_BACKGROUND, self.OnEraseBackground)
        self.Bind(wx.EVT_PAINT, self.OnPaint)


    def Refresh(self):
        if self.doubleBuffered:
            self.painter.ClearBuffer()
        wx.ScrolledWindow.Refresh(self, False)

    def GetPainter(self):
        return self.painter

    def GetLayoutEngine(self):
        return self.layout

    def GetTransform(self):
        return self.transform

    def __repr__(self):
        return "<MVCTree instance at %s>" % str(hex(id(self)))

    def __str__(self):
        return self.__repr__()

    def NodeAdded(self, parent, child):
        e = MVCTreeEvent(wxEVT_MVCTREE_ADD_ITEM, self.GetId(), node = child, nodes = [parent, child])
        self.GetEventHandler().ProcessEvent(e)
        self.painter.ClearBuffer()

    def NodeInserted(self, parent, child, index):
        e = MVCTreeEvent(wxEVT_MVCTREE_ADD_ITEM, self.GetId(), node = child, nodes = [parent, child])
        self.GetEventHandler().ProcessEvent(e)
        self.painter.ClearBuffer()

    def NodeRemoved(self, node):
        e = MVCTreeEvent(wxEVT_MVCTREE_DELETE_ITEM, self.GetId(), node = child, nodes = [parent, child])
        self.GetEventHandler().ProcessEvent(e)
        self.painter.ClearBuffer()

    def OnKeyDown(self, evt):
        e = MVCTreeEvent(wxEVT_MVCTREE_KEY_DOWN, self.GetId(), keyEvent = evt)
        self.GetEventHandler().ProcessEvent(e)

    def SetFont(self, font):
        self.painter.SetFont(font)
        dc = wx.ClientDC(self)
        dc.SetFont(font)
        self.layout.SetHeight(dc.GetTextExtent("")[1] + 18)
        self.painter.ClearBuffer()

    def GetFont(self):
        return self.painter.GetFont()

    def AddEditor(self, editor):
        self._editors.append(editor)

    def RemoveEditor(self, editor):
        self._editors.remove(editor)

    def OnMouse(self, evt):
        self.painter.OnMouse(evt)

    def OnNodeClick(self, node, mouseEvent):
        if node.selected and (self.IsMultiSelect() and mouseEvent.ControlDown()):
            self.RemoveFromSelection(node.data)
        else:
            self.AddToSelection(node.data, mouseEvent.ControlDown(), mouseEvent.ShiftDown())

    def OnKnobClick(self, node):
        self.SetExpanded(node.data, not node.expanded)

    def GetDisplayText(self, node):
        treenode = self.nodemap[node]
        return self.painter.textConverter.Convert(treenode)

    def IsDoubleBuffered(self):
        return self.doubleBuffered

    def SetDoubleBuffered(self, bool):
        """
        By default MVCTree is double-buffered.
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
        self.layoutRoot.expanded = True
        self.LoadChildren(self.layoutRoot)
        self.currentRoot = self.layoutRoot
        self.offset = [0,0]
        self.rotation = 0
        self._scrollset = None
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
            layoutNode.built = True
            if not self._assumeChildren:
                for kid in layoutNode.kids:
                    self.LoadChildren(kid)

    def OnEraseBackground(self, evt):
        pass

    def OnSize(self, evt):
        size = self.GetSize()
        self.center = (size.width/2, size.height/2)
        if self._lastPhysicalSize.width < size.width or self._lastPhysicalSize.height < size.height:
            self.painter.ClearBuffer()
        self._lastPhysicalSize = size

    def GetSelection(self):
        "Returns a tuple of selected nodes."
        return tuple(self._selections)

    def SetSelection(self, nodeTuple):
        if type(nodeTuple) != type(()):
            nodeTuple = (nodeTuple,)
        e = MVCTreeNotifyEvent(wxEVT_MVCTREE_SEL_CHANGING, self.GetId(), nodeTuple[0], nodes = nodeTuple)
        self.GetEventHandler().ProcessEvent(e)
        if not e.notify.IsAllowed():
            return
        for node in nodeTuple:
            treenode = self.nodemap[node]
            treenode.selected = True
        for node in self._selections:
            treenode = self.nodemap[node]
            node.selected = False
        self._selections = list(nodeTuple)
        e = MVCTreeEvent(wxEVT_MVCTREE_SEL_CHANGED, self.GetId(), nodeTuple[0], nodes = nodeTuple)
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
                e = MVCTreeNotifyEvent(wxEVT_MVCTREE_BEGIN_EDIT, self.GetId(), node)
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
        e = MVCTreeNotifyEvent(wxEVT_MVCTREE_END_EDIT, self.GetId(), node)
        self.GetEventHandler().ProcessEvent(e)
        if not e.notify.IsAllowed():
            return False
        self._currentEditor = None
        return True


    def SetExpanded(self, node, bool):
        treenode = self.nodemap[node]
        if bool:
            e = MVCTreeNotifyEvent(wxEVT_MVCTREE_ITEM_EXPANDING, self.GetId(), node)
            self.GetEventHandler().ProcessEvent(e)
            if not e.notify.IsAllowed():
                return
            if not treenode.built:
                self.LoadChildren(treenode)
        else:
            e = MVCTreeNotifyEvent(wxEVT_MVCTREE_ITEM_COLLAPSING, self.GetId(), node)
            self.GetEventHandler().ProcessEvent(e)
            if not e.notify.IsAllowed():
                return
        treenode.expanded = bool
        e = None
        if treenode.expanded:
            e = MVCTreeEvent(wxEVT_MVCTREE_ITEM_EXPANDED, self.GetId(), node)
        else:
            e = MVCTreeEvent(wxEVT_MVCTREE_ITEM_COLLAPSED, self.GetId(), node)
        self.GetEventHandler().ProcessEvent(e)
        self.layout.Layout(self.currentRoot)
        self.transform.Transform(self.currentRoot, self.offset, self.rotation)
        self.Refresh()

    def IsExpanded(self, node):
        return self.nodemap[node].expanded

    def AddToSelection(self, nodeOrTuple, enableMulti = True, shiftMulti = False):
        nodeTuple = nodeOrTuple
        if type(nodeOrTuple)!= type(()):
            nodeTuple = (nodeOrTuple,)
        e = MVCTreeNotifyEvent(wxEVT_MVCTREE_SEL_CHANGING, self.GetId(), nodeTuple[0], nodes = nodeTuple)
        self.GetEventHandler().ProcessEvent(e)
        if not e.notify.IsAllowed():
            return
        changeparents = []
        if not (self.IsMultiSelect() and (enableMulti or shiftMulti)):
            for node in self._selections:
                treenode = self.nodemap[node]
                treenode.selected = False
                changeparents.append(treenode)
            node = nodeTuple[0]
            self._selections = [node]
            treenode = self.nodemap[node]
            changeparents.append(treenode)
            treenode.selected = True
        else:
            if shiftMulti:
                for node in nodeTuple:
                    treenode = self.nodemap[node]
                    oldtreenode = self.nodemap[self._selections[0]]
                    if treenode.parent == oldtreenode.parent:
                        found = 0
                        for kid in oldtreenode.parent.kids:
                            if kid == treenode or kid == oldtreenode:
                                found = not found
                                kid.selected = True
                                self._selections.append(kid.data)
                                changeparents.append(kid)
                            elif found:
                                kid.selected = True
                                self._selections.append(kid.data)
                                changeparents.append(kid)
            else:
                for node in nodeTuple:
                    try:
                        self._selections.index(node)
                    except ValueError:
                        self._selections.append(node)
                        treenode = self.nodemap[node]
                        treenode.selected = True
                        changeparents.append(treenode)
        e = MVCTreeEvent(wxEVT_MVCTREE_SEL_CHANGED, self.GetId(), nodeTuple[0], nodes = nodeTuple)
        self.GetEventHandler().ProcessEvent(e)
        dc = wx.ClientDC(self)
        self.PrepareDC(dc)
        for node in changeparents:
            if node:
                self.painter.Paint(dc, node, doubleBuffered = 0, paintBackground = 0)
        self.painter.ClearBuffer()

    def RemoveFromSelection(self, nodeTuple):
        if type(nodeTuple) != type(()):
            nodeTuple = (nodeTuple,)
        changeparents = []
        for node in nodeTuple:
            self._selections.remove(node)
            treenode = self.nodemap[node]
            changeparents.append(treenode)
            treenode.selected = False
        e = MVCTreeEvent(wxEVT_MVCTREE_SEL_CHANGED, self.GetId(), node, nodes = nodeTuple)
        self.GetEventHandler().ProcessEvent(e)
        dc = wx.ClientDC(self)
        self.PrepareDC(dc)
        for node in changeparents:
            if node:
                self.painter.Paint(dc, node, doubleBuffered = 0, paintBackground = 0)
        self.painter.ClearBuffer()


    def GetBackgroundColour(self):
        if hasattr(self, 'painter') and self.painter:
            return self.painter.GetBackgroundColour()
        else:
            return wx.Window.GetBackgroundColour(self)
    def SetBackgroundColour(self, color):
        if hasattr(self, 'painter') and self.painter:
            self.painter.SetBackgroundColour(color)
        else:
            wx.Window.SetBackgroundColour(self, color)
    def GetForegroundColour(self):
        if hasattr(self, 'painter') and self.painter:
            return self.painter.GetForegroundColour()
        else:
            return wx.Window.GetBackgroundColour(self)
    def SetForegroundColour(self, color):
        if hasattr(self, 'painter') and self.painter:
            self.painter.SetForegroundColour(color)
        else:
            wx.Window.SetBackgroundColour(self, color)

    def SetAssumeChildren(self, bool):
        self._assumeChildren = bool

    def GetAssumeChildren(self):
        return self._assumeChildren

    def OnPaint(self, evt):
        """
        Ensures that the tree has been laid out and transformed, then calls the painter
        to paint the control.
        """
        try:
            self.EnableScrolling(False, False)
            if not self.laidOut:
                self.layout.Layout(self.currentRoot)
                self.laidOut = True
                self.transformed = False
            if not self.transformed:
                self.transform.Transform(self.currentRoot, self.offset, self.rotation)
                self.transformed = True
            tsize = None
            tsize = list(self.transform.GetSize())
            tsize[0] = tsize[0] + 50
            tsize[1] = tsize[1] + 50
            w, h = self.GetSize()
            if tsize[0] > w or tsize[1] > h:
                if not hasattr(self, '_oldsize') or (tsize[0] > self._oldsize[0] or tsize[1] > self._oldsize[1]):
                    self._oldsize = tsize
                    oldstart = self.GetViewStart()
                    self._lastPhysicalSize = self.GetSize()
                    self.SetScrollbars(10, 10, tsize[0]/10, tsize[1]/10)
                    self.Scroll(oldstart[0], oldstart[1])
            dc = wx.PaintDC(self)
            self.PrepareDC(dc)
            dc.SetFont(self.GetFont())
            self.painter.Paint(dc, self.currentRoot, self.doubleBuffered)
        except:
            traceback.print_exc()



