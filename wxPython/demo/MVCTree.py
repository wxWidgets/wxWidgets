
import  os
import  sys

import  wx
import  wx.lib.mvctree  as  tree

logger = None
def selchanging(evt):
    logger.write("SelChanging!\n")

def selchanged(evt):
    logger.write("SelChange!\n")
    logger.write(str(evt.node))
def expanded(evt):
    logger.write("Expanded\n")
def closed(evt):
    logger.write("Closed!\n")
def key(evt):
    logger.write("Key\n")
def add(evt):
    logger.write("Add\n")
def delitem(evt):
    logger.write("Delete\n")

def runTest(frame, nb, log):
    #f = wx.Frame(frame, -1, "MVCTree", (0,0), (200,500))
    global logger
    logger = log
    p = tree.MVCTree(nb, -1)
    #f = wx.Frame(frame, -1, "MVCTree")
    #p = tree.MVCTree(f, -1)
    p.SetAssumeChildren(True)
    p.SetModel(tree.LateFSTreeModel(os.path.normpath(os.getcwd() + os.sep +'..')))

    #Uncomment this to enable live filename editing!
#    p.AddEditor(FileEditor(p))

    p.SetMultiSelect(True)
    tree.EVT_MVCTREE_SEL_CHANGING(p, p.GetId(), selchanging)
    tree.EVT_MVCTREE_SEL_CHANGED(p, p.GetId(), selchanged)
    tree.EVT_MVCTREE_ITEM_EXPANDED(p, p.GetId(), expanded)
    tree.EVT_MVCTREE_ITEM_COLLAPSED(p, p.GetId(), closed)
    tree.EVT_MVCTREE_ADD_ITEM(p, p.GetId(), add)
    tree.EVT_MVCTREE_DELETE_ITEM(p, p.GetId(), delitem)
    tree.EVT_MVCTREE_KEY_DOWN(p, p.GetId(), key)

    return p
    #frame.otherWin = f
    #f.Show(True)
    #return None


overview = """\

MVCTree is a control which handles hierarchical data. It is
constructed in model-view-controller architecture, so the display of
that data, and the content of the data can be changed greatly without
affecting the other parts.

Multiple selections are possible by holding down the Ctrl key.

This demo shows the wxPython directory structure. The interesting part
is that the tree model is late-bound to the filesystem, so the
filenames are not retrieved until the directory is expanded. In
mvctree.py are models for generic data, and both the early and
late-bound filesystem models.

There is also support for editing, though it's not enabled in this
demo, to avoid accidentally renaming files!

"""


if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])] + sys.argv[1:])
