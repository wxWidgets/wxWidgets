"""PyCrust Filling is the gui tree control through which a user can navigate
the local namespace or any object."""

__author__ = "Patrick K. O'Brien <pobrien@orbtech.com>"
__cvsid__ = "$Id$"
__revision__ = "$Revision$"[11:-2]

from wxPython import wx
from wxPython import stc
from version import VERSION
import inspect
import introspect
import keyword
import sys
import types

True, False = 1, 0

COMMONTYPES = [getattr(types, t) for t in dir(types) \
               if not t.startswith('_') \
               and t not in ('ClassType', 'InstanceType', 'ModuleType')]
try:
    COMMONTYPES.append(type(''.__repr__))  # Method-wrapper in version 2.2.x.
except AttributeError:
    pass


class FillingTree(wx.wxTreeCtrl):
    """PyCrust FillingTree based on wxTreeCtrl."""
    
    name = 'PyCrust Filling Tree'
    revision = __revision__

    def __init__(self, parent, id=-1, pos=wx.wxDefaultPosition, 
                 size=wx.wxDefaultSize, style=wx.wxTR_HAS_BUTTONS, 
                 rootObject=None, rootLabel=None, rootIsNamespace=0):
        """Create a PyCrust FillingTree instance."""
        wx.wxTreeCtrl.__init__(self, parent, id, pos, size)
        self.rootIsNamespace = rootIsNamespace
        if not rootObject:
            import __main__
            rootObject = __main__
            self.rootIsNamespace = 1
        if not rootLabel: rootLabel = 'Ingredients'
        rootData = wx.wxTreeItemData(rootObject)
        self.root = self.AddRoot(rootLabel, -1, -1, rootData)
        self.SetItemHasChildren(self.root, self.hasChildren(self.root))
        wx.EVT_TREE_ITEM_EXPANDING(self, self.GetId(), self.OnItemExpanding)
        wx.EVT_TREE_ITEM_COLLAPSED(self, self.GetId(), self.OnItemCollapsed)
        wx.EVT_TREE_SEL_CHANGED(self, self.GetId(), self.OnSelChanged)

    def hasChildren(self, o):
        """Return true if object has children."""
        if self.getChildren(o):
            return True
        else:
            return False

    def getChildren(self, o):
        """Return a dictionary with the attributes or contents of object."""
        busy = wx.wxBusyCursor()
        otype = type(o)
        if (otype is types.DictType) \
        or str(otype)[17:23] == 'BTrees' and hasattr(o, 'keys'):
            return o
        d = {}
        if otype is types.ListType:
            for n in range(len(o)):
                key = '[' + str(n) + ']'
                d[key] = o[n]
        if otype not in COMMONTYPES:
            for key in introspect.getAttributeNames(o):
                # Believe it or not, some attributes can disappear, such as
                # the exc_traceback attribute of the sys module. So this is
                # nested in a try block.
                try:
                    d[key] = getattr(o, key)
                except:
                    pass
        return d

    def OnItemExpanding(self, event):
        busy = wx.wxBusyCursor()
        selection = event.GetItem()
        if self.IsExpanded(selection):
            return
        o = self.GetPyData(selection)
        children = self.getChildren(o)
        if not children:
            return
        list = children.keys()
        try:
            list.sort(lambda x, y: cmp(x.lower(), y.lower()))
        except:
            pass
        for item in list:
            itemtext = str(item)
            # Show string dictionary items with single quotes, except for
            # the first level of items, if they represent a namespace.
            if type(o) is types.DictType \
            and type(item) is types.StringType \
            and (selection != self.root \
                 or (selection == self.root and not self.rootIsNamespace)):
                itemtext = repr(item)
            child = self.AppendItem(selection, itemtext, -1, -1, 
                                    wx.wxTreeItemData(children[item]))
            self.SetItemHasChildren(child, self.hasChildren(children[item]))

    def OnItemCollapsed(self, event):
        """Remove all children from the item."""
        busy = wx.wxBusyCursor()
        item = event.GetItem()
        self.DeleteChildren(item)

    def OnSelChanged(self, event):
        busy = wx.wxBusyCursor()
        self.setText('')
        item = event.GetItem()
        if item == self.root:
            del busy
            return
        o = self.GetPyData(item)
        if o is None: # Windows bug fix.
            del busy
            return
        otype = type(o)
        text = ''
        text += self.getFullName(item)
        text += '\n\nType: ' + str(otype)
        try:
            value = str(o)
        except:
            value = ''
        if otype is types.StringType or otype is types.UnicodeType:
            value = repr(o)
        text += '\n\nValue: ' + value
        try:
            text += '\n\nDocstring:\n\n"""\n' + inspect.getdoc(o).strip() + '\n"""'
        except:
            pass
        if otype is types.InstanceType:
            try:
                text += '\n\nClass Definition:\n\n' + \
                        inspect.getsource(o.__class__)
            except:
                pass
        else:
            try:
                text += '\n\nSource Code:\n\n' + \
                        inspect.getsource(o)
            except:
                pass
        self.setText(text)
        del busy

    def getFullName(self, item, partial=''):
        """Return a syntactically proper name for item."""
        parent = self.GetItemParent(item)
        parento = self.GetPyData(parent)
        name = self.GetItemText(item)
        # Apply dictionary syntax to dictionary items, except the root
        # and first level children of a namepace.
        if (type(parento) is types.DictType \
            or str(type(parento))[17:23] == 'BTrees' \
            and hasattr(parento, 'keys')) \
        and ((item != self.root and parent != self.root) \
            or (parent == self.root and not self.rootIsNamespace)):
            name = '[' + name + ']'
        # Apply dot syntax to multipart names.
        if partial:
            if partial[0] == '[':
                name += partial
            else:
                name += '.' + partial
        # Repeat for everything but the root item
        # and first level children of a namespace.
        if (item != self.root and parent != self.root) \
        or (parent == self.root and not self.rootIsNamespace):
            name = self.getFullName(parent, partial=name)
        return name

    def setText(self, text):
        """Display information about the current selection."""

        # This method will most likely be replaced by the enclosing app
        # to do something more interesting, like write to a text control.
        print text

    def setStatusText(self, text):
        """Display status information."""
        
        # This method will most likely be replaced by the enclosing app
        # to do something more interesting, like write to a status bar.
        print text


if wx.wxPlatform == '__WXMSW__':
    faces = { 'times'  : 'Times New Roman',
              'mono'   : 'Courier New',
              'helv'   : 'Lucida Console',
              'lucida' : 'Lucida Console',
              'other'  : 'Comic Sans MS',
              'size'   : 10,
              'lnsize' : 9,
              'backcol': '#FFFFFF',
            }
    # Versions of wxPython prior to 2.3.2 had a sizing bug on Win platform.
    # The font was 2 points too large. So we need to reduce the font size.
    if ((wx.wxMAJOR_VERSION, wx.wxMINOR_VERSION) == (2, 3) and wx.wxRELEASE_NUMBER < 2) \
    or (wx.wxMAJOR_VERSION <= 2 and wx.wxMINOR_VERSION <= 2):
        faces['size'] -= 2
        faces['lnsize'] -= 2
else:  # GTK
    faces = { 'times'  : 'Times',
              'mono'   : 'Courier',
              'helv'   : 'Helvetica',
              'other'  : 'new century schoolbook',
              'size'   : 12,
              'lnsize' : 10,
              'backcol': '#FFFFFF',
            }


class FillingText(stc.wxStyledTextCtrl):
    """PyCrust FillingText based on wxStyledTextCtrl."""
    
    name = 'PyCrust Filling Text'
    revision = __revision__

    def __init__(self, parent, id=-1, pos=wx.wxDefaultPosition, 
                 size=wx.wxDefaultSize, style=wx.wxCLIP_CHILDREN):
        """Create a PyCrust FillingText instance."""
        stc.wxStyledTextCtrl.__init__(self, parent, id, pos, size, style)
        # Configure various defaults and user preferences.
        self.config()

    def config(self):
        """Configure shell based on user preferences."""
        self.SetMarginWidth(1, 0)
        
        self.SetLexer(stc.wxSTC_LEX_PYTHON)
        self.SetKeyWords(0, ' '.join(keyword.kwlist))

        self.setStyles(faces)
        self.SetViewWhiteSpace(0)
        self.SetTabWidth(4)
        self.SetUseTabs(0)
        self.SetReadOnly(1)
        try:
            self.SetWrapMode(1)
        except AttributeError:
            pass

    def setStyles(self, faces):
        """Configure font size, typeface and color for lexer."""
        
        # Default style
        self.StyleSetSpec(stc.wxSTC_STYLE_DEFAULT, "face:%(mono)s,size:%(size)d" % faces)

        self.StyleClearAll()

        # Built in styles
        self.StyleSetSpec(stc.wxSTC_STYLE_LINENUMBER, "back:#C0C0C0,face:%(mono)s,size:%(lnsize)d" % faces)
        self.StyleSetSpec(stc.wxSTC_STYLE_CONTROLCHAR, "face:%(mono)s" % faces)
        self.StyleSetSpec(stc.wxSTC_STYLE_BRACELIGHT, "fore:#0000FF,back:#FFFF88")
        self.StyleSetSpec(stc.wxSTC_STYLE_BRACEBAD, "fore:#FF0000,back:#FFFF88")

        # Python styles
        self.StyleSetSpec(stc.wxSTC_P_DEFAULT, "face:%(mono)s" % faces)
        self.StyleSetSpec(stc.wxSTC_P_COMMENTLINE, "fore:#007F00,face:%(mono)s" % faces)
        self.StyleSetSpec(stc.wxSTC_P_NUMBER, "")
        self.StyleSetSpec(stc.wxSTC_P_STRING, "fore:#7F007F,face:%(mono)s" % faces)
        self.StyleSetSpec(stc.wxSTC_P_CHARACTER, "fore:#7F007F,face:%(mono)s" % faces)
        self.StyleSetSpec(stc.wxSTC_P_WORD, "fore:#00007F,bold")
        self.StyleSetSpec(stc.wxSTC_P_TRIPLE, "fore:#7F0000")
        self.StyleSetSpec(stc.wxSTC_P_TRIPLEDOUBLE, "fore:#000033,back:#FFFFE8")
        self.StyleSetSpec(stc.wxSTC_P_CLASSNAME, "fore:#0000FF,bold")
        self.StyleSetSpec(stc.wxSTC_P_DEFNAME, "fore:#007F7F,bold")
        self.StyleSetSpec(stc.wxSTC_P_OPERATOR, "")
        self.StyleSetSpec(stc.wxSTC_P_IDENTIFIER, "")
        self.StyleSetSpec(stc.wxSTC_P_COMMENTBLOCK, "fore:#7F7F7F")
        self.StyleSetSpec(stc.wxSTC_P_STRINGEOL, "fore:#000000,face:%(mono)s,back:#E0C0E0,eolfilled" % faces)

    def SetText(self, *args, **kwds):
        self.SetReadOnly(0)
        stc.wxStyledTextCtrl.SetText(self, *args, **kwds)
        self.SetReadOnly(1)


class Filling(wx.wxSplitterWindow):
    """PyCrust Filling based on wxSplitterWindow."""
    
    name = 'PyCrust Filling'
    revision = __revision__
    
    def __init__(self, parent, id=-1, pos=wx.wxDefaultPosition, 
                 size=wx.wxDefaultSize, style=wx.wxSP_3D, name='Filling Window', 
                 rootObject=None, rootLabel=None, rootIsNamespace=0):
        """Create a PyCrust Filling instance."""
        wx.wxSplitterWindow.__init__(self, parent, id, pos, size, style, name)
        self.fillingTree = FillingTree(parent=self, rootObject=rootObject, 
                                       rootLabel=rootLabel, 
                                       rootIsNamespace=rootIsNamespace)
        self.fillingText = FillingText(parent=self)
        self.SplitVertically(self.fillingTree, self.fillingText, 200)
        self.SetMinimumPaneSize(1)
        # Override the filling so that descriptions go to fillingText.
        self.fillingTree.setText = self.fillingText.SetText
        # Select the root item.
        self.fillingTree.SelectItem(self.fillingTree.root)


class FillingFrame(wx.wxFrame):
    """Frame containing the PyCrust filling, or namespace tree component."""
    
    name = 'PyCrust Filling Frame'
    revision = __revision__
    
    def __init__(self, parent=None, id=-1, title='PyFilling', 
                 pos=wx.wxDefaultPosition, size=wx.wxDefaultSize, 
                 style=wx.wxDEFAULT_FRAME_STYLE, rootObject=None, 
                 rootLabel=None, rootIsNamespace=0):
        """Create a PyCrust FillingFrame instance."""
        wx.wxFrame.__init__(self, parent, id, title, pos, size, style)
        intro = 'Welcome To PyFilling - The Tastiest Namespace Inspector'
        self.CreateStatusBar()
        self.SetStatusText(intro)
        import images
        self.SetIcon(images.getPyCrustIcon())
        self.filling = Filling(parent=self, rootObject=rootObject, 
                               rootLabel=rootLabel, 
                               rootIsNamespace=rootIsNamespace)
        # Override the filling so that status messages go to the status bar.
        self.filling.fillingTree.setStatusText = self.SetStatusText


class App(wx.wxApp):
    """PyFilling standalone application."""
    
    def OnInit(self):
        wx.wxInitAllImageHandlers()
        self.fillingFrame = FillingFrame()
        self.fillingFrame.Show(True)
        self.SetTopWindow(self.fillingFrame)
        return True


  
   
