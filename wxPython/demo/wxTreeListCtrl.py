
from wxPython.wx import *
from wxPython.gizmos import wxTreeListCtrl

import images

#----------------------------------------------------------------------

class TestPanel(wxPanel):
    def __init__(self, parent, log):
        self.log = log
        wxPanel.__init__(self, parent, -1)
        EVT_SIZE(self, self.OnSize)

        self.tree = wxTreeListCtrl(self, -1, style = wxTR_DEFAULT_STYLE
                                   #| wxTR_ROW_LINES
                                   #| wxTR_NO_LINES | wxTR_TWIST_BUTTONS
                                   )
        isz = (16,16)
        il = wxImageList(isz[0], isz[1])
        fldridx     = il.Add(wxArtProvider_GetBitmap(wxART_FOLDER,      wxART_OTHER, isz))
        fldropenidx = il.Add(wxArtProvider_GetBitmap(wxART_FILE_OPEN,   wxART_OTHER, isz))
        fileidx     = il.Add(wxArtProvider_GetBitmap(wxART_REPORT_VIEW, wxART_OTHER, isz))
        smileidx    = il.Add(images.getSmilesBitmap())

        self.tree.SetImageList(il)
        self.il = il

        # create some columns
        self.tree.AddColumn("Main column")
        self.tree.AddColumn("Column 1")
        self.tree.AddColumn("Column 2")
        self.tree.SetMainColumn(0) # the one with the tree in it...
        self.tree.SetColumnWidth(0, 175)


        self.root = self.tree.AddRoot("The Root Item")
        self.tree.SetItemText(self.root, "col 1 root", 1)
        self.tree.SetItemText(self.root, "col 2 root", 2)
        self.tree.SetItemImage(self.root, fldridx, which = wxTreeItemIcon_Normal)
        self.tree.SetItemImage(self.root, fldropenidx, which = wxTreeItemIcon_Expanded)


        for x in range(15):
            txt = "Item %d" % x
            child = self.tree.AppendItem(self.root, txt)
            self.tree.SetItemText(child, txt + "(c1)", 1)
            self.tree.SetItemText(child, txt + "(c2)", 2)
            self.tree.SetItemImage(child, fldridx, which = wxTreeItemIcon_Normal)
            self.tree.SetItemImage(child, fldropenidx, which = wxTreeItemIcon_Expanded)

            for y in range(5):
                txt = "item %d-%s" % (x, chr(ord("a")+y))
                last = self.tree.AppendItem(child, txt)
                self.tree.SetItemText(last, txt + "(c1)", 1)
                self.tree.SetItemText(last, txt + "(c2)", 2)
                self.tree.SetItemImage(last, fldridx, which = wxTreeItemIcon_Normal)
                self.tree.SetItemImage(last, fldropenidx, which = wxTreeItemIcon_Expanded)

                for z in range(5):
                    txt = "item %d-%s-%d" % (x, chr(ord("a")+y), z)
                    item = self.tree.AppendItem(last,  txt)
                    self.tree.SetItemText(item, txt + "(c1)", 1)
                    self.tree.SetItemText(item, txt + "(c2)", 2)
                    self.tree.SetItemImage(item, fileidx, which = wxTreeItemIcon_Normal)
                    self.tree.SetItemImage(item, smileidx, which = wxTreeItemIcon_Selected)


        self.tree.Expand(self.root)


    def OnSize(self, evt):
        self.tree.SetSize(self.GetSize())


#----------------------------------------------------------------------

def runTest(frame, nb, log):
    win = TestPanel(nb, log)
    return win

#----------------------------------------------------------------------



overview = """<html><body>
<h2><center>wxTreeListCtrl</center></h2>

The wxTreeListCtrl is essentially a wxTreeCtrl with extra columns,
such that the look is similar to a wxListCtrl.

</body></html>
"""




if __name__ == '__main__':
    #raw_input("Press enter...")
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])])

