
from wxPython.wx import *

#---------------------------------------------------------------------------

class TestTreeCtrlPanel(wxPanel):
    def __init__(self, parent, log):
        wxPanel.__init__(self, parent, -1)

        self.log = log
        tID = NewId()

        self.tree = wxTreeCtrl(self, tID)
        root = self.tree.AddRoot("The Root Item")
        for x in range(15):
            child = self.tree.AppendItem(root, "Item %d" % x)
            for y in range(5):
                last = self.tree.AppendItem(child, "item %d-%s" % (x, chr(ord("a")+y)))
                for z in range(5):
                    self.tree.AppendItem(last,  "item %d-%s-%d" % (x, chr(ord("a")+y), z))

        self.tree.Expand(root)
        EVT_TREE_ITEM_EXPANDED  (self, tID, self.OnItemExpanded)
        EVT_TREE_ITEM_COLLAPSED (self, tID, self.OnItemCollapsed)
        EVT_TREE_SEL_CHANGED    (self, tID, self.OnSelChanged)

        EVT_LEFT_DCLICK(self.tree, self.OnLeftDClick)
        EVT_RIGHT_DOWN(self.tree, self.OnRightClick)
        EVT_RIGHT_UP(self.tree, self.OnRightUp)

    def OnRightClick(self, event):
        (x,y) = event.Position();
        item = self.tree.HitTest(wxPoint(x,y))
        self.log.WriteText("OnRightClick: %s\n" % self.tree.GetItemText(item))
        self.tree.SelectItem(item)

    def OnRightUp(self, event):
        (x,y) = event.Position();
        item = self.tree.HitTest(wxPoint(x,y))
        self.log.WriteText("OnRightUp: %s\n" % self.tree.GetItemText(item))

    def OnLeftDClick(self, event):
        (x,y) = event.Position();
        item = self.tree.HitTest(wxPoint(x,y))
        self.log.WriteText("OnLeftDClick: %s\n" % self.tree.GetItemText(item))


    def OnSize(self, event):
        w,h = self.GetClientSizeTuple()
        self.tree.SetDimensions(0, 0, w, h)


    def OnItemExpanded(self, event):
        item = event.GetItem()
        self.log.WriteText("OnItemExpanded: %s\n" % self.tree.GetItemText(item))

    def OnItemCollapsed(self, event):
        item = event.GetItem()
        self.log.WriteText("OnItemCollapsed: %s\n" % self.tree.GetItemText(item))

    def OnSelChanged(self, event):
        item = event.GetItem()
        self.log.WriteText("OnSelChanged: %s\n" % self.tree.GetItemText(item))

#---------------------------------------------------------------------------

def runTest(frame, nb, log):
    win = TestTreeCtrlPanel(nb, log)
    return win

#---------------------------------------------------------------------------


















overview = """\
A tree control presents information as a hierarchy, with items that may be expanded to show further items. Items in a tree control are referenced by wxTreeItemId handles.

wxTreeCtrl()
-------------------------

Default constructor.

wxTreeCtrl(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxTR_HAS_BUTTONS, const wxValidator& validator = wxDefaultValidator, const wxString& name = "listCtrl")

Constructor, creating and showing a tree control.

Parameters
-------------------

parent = Parent window. Must not be NULL.

id = Window identifier. A value of -1 indicates a default value.

pos = Window position.

size = Window size. If the default size (-1, -1) is specified then the window is sized appropriately.

style = Window style. See wxTreeCtrl.

validator = Window validator.

name = Window name.
"""
