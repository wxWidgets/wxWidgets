
from wxPython.wx import *

import string

#---------------------------------------------------------------------------

class MyTreeCtrl(wxTreeCtrl):
    def __init__(self, parent, id, pos, size, style):
        wxTreeCtrl.__init__(self, parent, id, pos, size, style)


    def OnCompareItems(self, item1, item2):
        t1 = self.GetItemText(item1)
        t2 = self.GetItemText(item2)
        if t1 < t2: return -1
        if t1 == t2: return 0
        return 1

#---------------------------------------------------------------------------

class TestTreeCtrlPanel(wxPanel):
    def __init__(self, parent, log):
        # Use the WANTS_CHARS style so the panel doesn't eat the Return key.
        wxPanel.__init__(self, parent, -1, style=wxWANTS_CHARS)
        EVT_SIZE(self, self.OnSize)

        self.log = log
        tID = NewId()

        self.tree = MyTreeCtrl(self, tID, wxDefaultPosition, wxDefaultSize,
                               wxTR_HAS_BUTTONS | wxTR_EDIT_LABELS)# | wxTR_MULTIPLE)

        #il = wxImageList(16, 16)
        #idx1 = il.Add(wxBitmap('bitmaps/smiles.bmp', wxBITMAP_TYPE_BMP))
        #idx2 = il.Add(wxBitmap('bitmaps/open.bmp', wxBITMAP_TYPE_BMP))
        #idx3 = il.Add(wxBitmap('bitmaps/new.bmp', wxBITMAP_TYPE_BMP))
        #idx4 = il.Add(wxBitmap('bitmaps/copy.bmp', wxBITMAP_TYPE_BMP))
        #idx5 = il.Add(wxBitmap('bitmaps/paste.bmp', wxBITMAP_TYPE_BMP))

        #self.tree.SetImageList(il)
        #self.il = il

        # NOTE:  For some reason tree items have to have a data object in
        #        order to be sorted.  Since our compare just uses the labels
        #        we don't need any real data, so we'll just use None.

        self.root = self.tree.AddRoot("The Root Item")
        self.tree.SetPyData(self.root, None)
        #self.tree.SetItemImage(self.root, idx1)

        for x in range(15):
            child = self.tree.AppendItem(self.root, "Item %d" % x)
            self.tree.SetPyData(child, None)
            #self.tree.SetItemImage(child, idx2)
            #self.tree.SetItemSelectedImage(child, idx3)
            for y in range(5):
                last = self.tree.AppendItem(child, "item %d-%s" % (x, chr(ord("a")+y)))
                self.tree.SetPyData(last, None)
                #self.tree.SetItemImage(last, idx4)
                #self.tree.SetItemSelectedImage(last, idx5)
                for z in range(5):
                    item = self.tree.AppendItem(last,  "item %d-%s-%d" % (x, chr(ord("a")+y), z))
                    self.tree.SetPyData(item, None)

        self.tree.Expand(self.root)
        EVT_TREE_ITEM_EXPANDED  (self, tID, self.OnItemExpanded)
        EVT_TREE_ITEM_COLLAPSED (self, tID, self.OnItemCollapsed)
        EVT_TREE_SEL_CHANGED    (self, tID, self.OnSelChanged)
        EVT_TREE_BEGIN_LABEL_EDIT(self, tID, self.OnBeginEdit)
        EVT_TREE_END_LABEL_EDIT (self, tID, self.OnEndEdit)
        EVT_TREE_ITEM_ACTIVATED (self, tID, self.OnActivate)

        EVT_LEFT_DCLICK(self.tree, self.OnLeftDClick)
        EVT_RIGHT_DOWN(self.tree, self.OnRightClick)
        EVT_RIGHT_UP(self.tree, self.OnRightUp)


    def OnRightClick(self, event):
        pt = event.GetPosition();
        item, flags = self.tree.HitTest(pt)
        self.log.WriteText("OnRightClick: %s, %s\n" % (self.tree.GetItemText(item),
                                                       type(item)))
        self.tree.SelectItem(item)



    def OnRightUp(self, event):
        pt = event.GetPosition();
        item, flags = self.tree.HitTest(pt)
        self.log.WriteText("OnRightUp: %s (manually starting label edit)\n"
                           % self.tree.GetItemText(item))
        self.tree.EditLabel(item)



    def OnBeginEdit(self, event):
        self.log.WriteText("OnBeginEdit\n")
        # show how to prevent edit...
        if self.tree.GetItemText(event.GetItem()) == "The Root Item":
            wxBell()
            self.log.WriteText("You can't edit this one...\n")
            event.Veto()

    def OnEndEdit(self, event):
        self.log.WriteText("OnEndEdit\n")
        # show how to reject edit, we'll not allow any digits
        for x in event.GetLabel():
            if x in string.digits:
                self.log.WriteText("You can't enter digits...\n")
                event.Veto()
                return


    def OnLeftDClick(self, event):
        pt = event.GetPosition();
        item, flags = self.tree.HitTest(pt)
        self.log.WriteText("OnLeftDClick: %s\n" % self.tree.GetItemText(item))
        parent = self.tree.GetItemParent(item)
        self.tree.SortChildren(parent)
        event.Skip()


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
        self.item = event.GetItem()
        self.log.WriteText("OnSelChanged: %s\n" % self.tree.GetItemText(self.item))
        if wxPlatform == '__WXMSW__':
            self.log.WriteText("BoundingRect: %s\n" %
                               self.tree.GetBoundingRect(self.item, true))
        #items = self.tree.GetSelections()
        #print map(self.tree.GetItemText, items)
        event.Skip()


    def OnActivate(self, evt):
        self.log.WriteText("OnActivate: %s\n" % self.tree.GetItemText(self.item))


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
