
import sys, os
from wxPython.wx import *
from wxPython.lib.mvctree import *


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
    #f = wxFrame(frame, -1, "wxMVCTree", wxPoint(0,0), wxSize(200,500))
    global logger
    logger = log
    p = wxMVCTree(nb, -1)
    p.SetAssumeChildren(true)
    p.SetModel(LateFSTreeModel(os.path.normpath(os.getcwd() + os.sep +'..')))
    #Uncomment this to enable live filename editing!
#    p.AddEditor(FileEditor(p))
    p.SetMultiSelect(true)
    EVT_MVCTREE_SEL_CHANGING(p, p.GetId(), selchanging)
    EVT_MVCTREE_SEL_CHANGED(p, p.GetId(), selchanged)
    EVT_MVCTREE_ITEM_EXPANDED(p, p.GetId(), expanded)
    EVT_MVCTREE_ITEM_COLLAPSED(p, p.GetId(), closed)
    EVT_MVCTREE_ADD_ITEM(p, p.GetId(), add)
    EVT_MVCTREE_DELETE_ITEM(p, p.GetId(), delitem)
    EVT_MVCTREE_KEY_DOWN(p, p.GetId(), key)
    return p

overview = """\
wxMVCTree is a control which handles hierarchical data. It is constructed in model-view-controller architecture, so the display of that data, and the content of the data can be changed greatly without affecting the other parts.

Multiple selections are possible by holding down the Ctrl key.

This demo shows the wxPython directory structure. The interesting part is that the tree model is late-bound to the filesystem, so the filenames are not retrieved until the directory is expanded. In mvctree.py are models for generic data, and both the early and late-bound filesystem models.

There is also support for editing, though it's not enabled in this demo, to avoid accidentally renaming files!

"""








