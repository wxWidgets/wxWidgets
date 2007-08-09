"""
Hello, and welcome to this test of the wxTreeItemData
class.

The wxTreeItemData class can be used to associate a python
object with a wxTreeCtrl item. In this sample, its use is
demonstrated via a tree control that shows the contents of a
python namespace according to the standard dir()
command. Every item in the tree has its label taken from the
dir() output, and 'behind it' a reference to the python
object is stored in a wxTreeItemData object.

As you may have guessed by now, this sample automatically
displays '__doc__' strings if the selected python object
happens to have one. Please expand the pyTree object to
learn more about the implementation.

Version 1.0, April 4 1999.
Harm van der Heijden (H.v.d.Heijden@phys.tue.nl)

P.S. Check out the string module. It's imported in this
sample not because it's used, but because it's so
beautifully documented...
"""

import  string  # Used for demo purposes, nothing more. :-)
import  sys

import  wx

#----------------------------------------------------------------------

def _getindent(line):
    """Returns the indentation level of the given line."""
    indent = 0
    for c in line:
        if c == ' ': indent = indent + 1
        elif c == '\t': indent = indent + 8
        else: break
    return indent

def _sourcefinder(func):
    """Given a func_code object, this function tries to find and return
    the python source code of the function."""
    try:
        f = open(func.co_filename,"r")
    except:
        return "(could not open file %s)" % (func.co_filename,)

    for i in range(func.co_firstlineno):
        line = f.readline()

    ind = _getindent(line)
    msg = ""

    while line:
        msg = msg + line
        line = f.readline()
        # the following should be <= ind, but then we get
        # confused by multiline docstrings. Using == works most of
        # the time... but not always!
        if _getindent(line) == ind: break

    return msg

#----------------------------------------------------------------------

class pyTree(wx.TreeCtrl):
    """
    This wx.TreeCtrl derivative displays a tree view of a Python namespace.
    Anything from which the dir() command returns a non-empty list is a branch
    in this tree.
    """

    def __init__(self, parent, id, root):
        """
        Initialize function; because we insert branches into the tree
        as needed, we use the ITEM_EXPANDING event handler. The
        ITEM_COLLAPSED handler removes the stuff afterwards. The
        SEL_CHANGED handler attempts to display interesting
        information about the selected object.
        """
        wx.TreeCtrl.__init__(self, parent, id)
        self.root = self.AddRoot(str(root), -1, -1, wx.TreeItemData(root))

        if dir(root):
            self.SetItemHasChildren(self.root, True)

        self.Bind(wx.EVT_TREE_ITEM_EXPANDING, self.OnItemExpanding, id=self.GetId())
        self.Bind(wx.EVT_TREE_ITEM_COLLAPSED, self.OnItemCollapsed, id=self.GetId())
        self.Bind(wx.EVT_TREE_SEL_CHANGED, self.OnSelChanged, id=self.GetId())

        self.output = None
        self.Expand(self.root)


    def SetOutput(self, output):
        """
        Set output function (accepts single string). Used to display string
        representation of the selected object by OnSelChanged.
        """
        self.output = output


    def OnItemExpanding(self,event):
        """
        The real workhorse of this class. First we retrieve the object
        (parent) belonging to the branch that is to be expanded. This
        is done by calling GetPyData(parent), which is a short-cut for
        GetPyItemData(parent).Get().

        Then we get the dir() list of that object. For each item in
        this list, a tree item is created with associated
        wxTreeItemData referencing the child object. We get this
        object using child = getattr(parent, item).

        Finally, we check wether the child returns a non-empty dir()
        list. If so, it is labeled as 'having children', so that it
        may be expanded. When it actually is expanded, this function
        will again figure out what the offspring is.
        """
        item = event.GetItem()

        if self.IsExpanded(item):  # This event can happen twice in the self.Expand call
            return
            
        obj = self.GetPyData( item )
        lst = dir(obj)

        for key in lst:
            new_obj = getattr(obj,key)
            new_item = self.AppendItem( item, key, -1, -1,
                                        wx.TreeItemData(new_obj) )

            if dir(new_obj):
                self.SetItemHasChildren(new_item, True)

    def OnItemCollapsed(self, event):
        """
        We need to remove all children here, otherwise we'll see all
        that old rubbish again after the next expansion.
        """
        item = event.GetItem()
        self.DeleteChildren(item)

    def OnSelChanged(self, event):
        """
        If an output function is defined, we try to print some
        informative, interesting and thought-provoking stuff to it.
        If it has a __doc__ string, we print it. If it's a function or
        unbound class method, we attempt to find the python source.
        """
        if not self.output:
            return

        obj = self.GetPyData( event.GetItem() )
        msg = str(obj)

        if hasattr(obj, '__doc__'):
            msg = msg+"\n\nDocumentation string:\n\n%s" % ( getattr(obj, '__doc__'),)

        # Is it a function?
        func = None

        if hasattr(obj, "func_code"): # normal function
            func = getattr(obj, "func_code")

        elif hasattr(obj, "im_func"): # unbound class method
            func = getattr(getattr(obj, "im_func"), "func_code")

        if func: # if we found one, let's try to print the source
            msg = msg+"\n\nFunction source:\n\n" + _sourcefinder(func)

        apply(self.output, (msg,))

#----------------------------------------------------------------------

overview = __doc__

def runTest(frame, nb, log):
    """
    This method is used by the wxPython Demo Framework for integrating
    this demo with the rest.
    """
    thisModule = sys.modules[__name__]
    win = wx.Frame(frame, -1, "PyTreeItemData Test")
    split = wx.SplitterWindow(win, -1)
    tree = pyTree(split, -1, thisModule)
    text = wx.TextCtrl(split, -1, "", style=wx.TE_MULTILINE)
    split.SplitVertically(tree, text, 200)
    tree.SetOutput(text.SetValue)
    tree.SelectItem(tree.root)
    win.SetSize((800,500))
    frame.otherWin = win
    win.Show(1)



#----------------------------------------------------------------------
if __name__ == '__main__':

    class MyFrame(wx.Frame):
        """Very standard Frame class. Nothing special here!"""

        def __init__(self):
            """Make a splitter window; left a tree, right a textctrl. Wow."""
            import __main__
            wx.Frame.__init__(self, None, -1, "PyTreeItemData Test", size=(800,500))
            split = wx.SplitterWindow(self, -1)
            tree = pyTree(split, -1, __main__)
            text = wx.TextCtrl(split, -1, "", style=wx.TE_MULTILINE)
            split.SplitVertically(tree, text, 200)
            tree.SetOutput(text.SetValue)
            tree.SelectItem(tree.root)

    class MyApp(wx.App):
        """This class is even less interesting than MyFrame."""

        def OnInit(self):
            """OnInit. Boring, boring, boring!"""
            frame = MyFrame()
            frame.Show(True)
            self.SetTopWindow(frame)
            return True

    app = MyApp(False)
    app.MainLoop()


