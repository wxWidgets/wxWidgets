
import  wx
import  wx.gizmos   as  gizmos

import  images

#----------------------------------------------------------------------

class TestPanel(wx.Panel):
    def __init__(self, parent, log):
        self.log = log
        wx.Panel.__init__(self, parent, -1)
        self.Bind(wx.EVT_SIZE, self.OnSize)

        self.tree = gizmos.TreeListCtrl(self, -1, style =
                                        wx.TR_DEFAULT_STYLE
                                        #| wx.TR_HAS_BUTTONS
                                        #| wx.TR_TWIST_BUTTONS
                                        #| wx.TR_ROW_LINES
                                        #| wx.TR_COLUMN_LINES
                                        #| wx.TR_NO_LINES 
                                        | wx.TR_FULL_ROW_HIGHLIGHT
                                   )

        isz = (16,16)
        il = wx.ImageList(isz[0], isz[1])
        fldridx     = il.Add(wx.ArtProvider_GetBitmap(wx.ART_FOLDER,      wx.ART_OTHER, isz))
        fldropenidx = il.Add(wx.ArtProvider_GetBitmap(wx.ART_FILE_OPEN,   wx.ART_OTHER, isz))
        fileidx     = il.Add(wx.ArtProvider_GetBitmap(wx.ART_NORMAL_FILE, wx.ART_OTHER, isz))
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
        self.tree.SetItemImage(self.root, fldridx, which = wx.TreeItemIcon_Normal)
        self.tree.SetItemImage(self.root, fldropenidx, which = wx.TreeItemIcon_Expanded)


        for x in range(15):
            txt = "Item %d" % x
            child = self.tree.AppendItem(self.root, txt)
            self.tree.SetItemText(child, txt + "(c1)", 1)
            self.tree.SetItemText(child, txt + "(c2)", 2)
            self.tree.SetItemImage(child, fldridx, which = wx.TreeItemIcon_Normal)
            self.tree.SetItemImage(child, fldropenidx, which = wx.TreeItemIcon_Expanded)

            for y in range(5):
                txt = "item %d-%s" % (x, chr(ord("a")+y))
                last = self.tree.AppendItem(child, txt)
                self.tree.SetItemText(last, txt + "(c1)", 1)
                self.tree.SetItemText(last, txt + "(c2)", 2)
                self.tree.SetItemImage(last, fldridx, which = wx.TreeItemIcon_Normal)
                self.tree.SetItemImage(last, fldropenidx, which = wx.TreeItemIcon_Expanded)

                for z in range(5):
                    txt = "item %d-%s-%d" % (x, chr(ord("a")+y), z)
                    item = self.tree.AppendItem(last,  txt)
                    self.tree.SetItemText(item, txt + "(c1)", 1)
                    self.tree.SetItemText(item, txt + "(c2)", 2)
                    self.tree.SetItemImage(item, fileidx, which = wx.TreeItemIcon_Normal)
                    self.tree.SetItemImage(item, smileidx, which = wx.TreeItemIcon_Selected)


        self.tree.Expand(self.root)

        self.tree.GetMainWindow().Bind(wx.EVT_RIGHT_UP, self.OnRightUp)


    def OnRightUp(self, evt):
        pos = evt.GetPosition()
        item, flags, col = self.tree.HitTest(pos)
        if item:
            self.log.write('Flags: %s, Col:%s, Text: %s' %
                           (flags, col, self.tree.GetItemText(item, col)))

    def OnSize(self, evt):
        self.tree.SetSize(self.GetSize())


#----------------------------------------------------------------------

def runTest(frame, nb, log):
    win = TestPanel(nb, log)
    return win

#----------------------------------------------------------------------



overview = """<html><body>
<h2><center>TreeListCtrl</center></h2>

The TreeListCtrl is essentially a wx.TreeCtrl with extra columns,
such that the look is similar to a wx.ListCtrl.

</body></html>
"""


if __name__ == '__main__':
    #raw_input("Press enter...")
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])] + sys.argv[1:])

