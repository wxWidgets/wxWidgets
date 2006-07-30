"""PyCrust Filling is the gui tree control through which a user can navigate
the local namespace or any object."""

__author__ = "Patrick K. O'Brien <pobrien@orbtech.com>"
__cvsid__ = "$Id$"
__version__ = "$Revision$"[11:-2]

from wxPython.wx import *
from wxPython.stc import *
from version import VERSION
import inspect
import introspect
import keyword
import sys
import types


class FillingTree(wxTreeCtrl):
    """PyCrust FillingTree based on wxTreeCtrl."""
    
    name = 'PyCrust Filling Tree'
    revision = __version__

    def __init__(self, parent, id=-1, pos=wxDefaultPosition, \
                 size=wxDefaultSize, style=wxTR_HAS_BUTTONS, \
                 rootObject=None, rootLabel=None, rootIsNamespace=0):
        """Create a PyCrust FillingTree instance."""
        wxTreeCtrl.__init__(self, parent, id, pos, size)
        self.rootIsNamespace = rootIsNamespace
        if not rootObject:
            import __main__
            rootObject = __main__
            self.rootIsNamespace = 1
        if not rootLabel: rootLabel = 'Ingredients'
        rootData = wxTreeItemData(rootObject)
        self.root = self.AddRoot(rootLabel, -1, -1, rootData)
        self.SetItemHasChildren(self.root, self.hasChildren(self.root))
        EVT_TREE_ITEM_EXPANDING(self, self.GetId(), self.OnItemExpanding)
        EVT_TREE_ITEM_COLLAPSED(self, self.GetId(), self.OnItemCollapsed)
        EVT_TREE_SEL_CHANGED(self, self.GetId(), self.OnSelChanged)

    def hasChildren(self, object):
        """Return true if object has children."""
        if self.getChildren(object):
            return true
        else:
            return false

    def getChildren(self, object):
        """Return a dictionary with the attributes or contents of object."""
        dict = {}
        objtype = type(object)
        if objtype is types.DictType:
            dict = object
        elif (objtype in (types.ClassType, types.InstanceType, \
                          types.ModuleType)) \
        or hasattr(object, '__class__'):
            for key in introspect.getAttributeNames(object):
                # Believe it or not, some attributes can disappear, such as
                # the exc_traceback attribute of the sys module. So this is
                # nested in a try block.
                try:
                    dict[key] = getattr(object, key)
                except:
                    pass
        return dict

    def OnItemExpanding(self, event):
        selection = event.GetItem()
        if self.IsExpanded(selection):
            return
        object = self.GetPyData(selection)
        children = self.getChildren(object)
        if not children:
            return
        list = children.keys()
        list.sort()
        for item in list:
            itemtext = str(item)
            # Show string dictionary items with single quotes, except for
            # the first level of items, if they represent a namespace.
            if type(object) is types.DictType \
            and type(item) is types.StringType \
            and (selection != self.root \
                 or (selection == self.root and not self.rootIsNamespace)):
                itemtext = repr(item)
            child = self.AppendItem(selection, itemtext, -1, -1, \
                                    wxTreeItemData(children[item]))
            self.SetItemHasChildren(child, self.hasChildren(children[item]))

    def OnItemCollapsed(self, event):
        """Remove all children from the item."""
        item = event.GetItem()
        self.DeleteChildren(item)

    def OnSelChanged(self, event):
        item = event.GetItem()
        if item == self.root:
            self.setText('')
            return
        object = self.GetPyData(item)
        text = ''
        text += self.getFullName(item)
        text += '\n\nType: ' + str(type(object))[7:-2]
        value = str(object)
        if type(object) is types.StringType:
            value = repr(value)
        text += '\n\nValue: ' + value
        if type(object) is types.InstanceType:
            try:
                text += '\n\nClass Definition:\n\n' + \
                        inspect.getsource(object.__class__)
            except:
                try:
                    text += '\n\n"""' + inspect.getdoc(object).strip() + '"""'
                except:
                    pass
        else:
            try:
                text += '\n\nSource Code:\n\n' + \
                        inspect.getsource(object)
            except:
                try:
                    text += '\n\n"""' + inspect.getdoc(object).strip() + '"""'
                except:
                    pass
        self.setText(text)

    def getFullName(self, item, partial=''):
        """Return a syntactically proper name for item."""
        parent = self.GetItemParent(item)
        parentobject = self.GetPyData(parent)
        name = self.GetItemText(item)
        # Apply dictionary syntax to dictionary items, except the root
        # and first level children of a namepace.
        if type(parentobject) is types.DictType \
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


if wxPlatform == '__WXMSW__':
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
    if ((wxMAJOR_VERSION, wxMINOR_VERSION) == (2, 3) and wxRELEASE_NUMBER < 2) \
    or (wxMAJOR_VERSION <= 2 and wxMINOR_VERSION <= 2):
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


class FillingText(wxStyledTextCtrl):
    """PyCrust FillingText based on wxStyledTextCtrl."""
    
    name = 'PyCrust Filling Text'
    revision = __version__

    def __init__(self, parent, id=-1, pos=wxDefaultPosition, \
                 size=wxDefaultSize, style=wxCLIP_CHILDREN):
        """Create a PyCrust FillingText instance."""
        wxStyledTextCtrl.__init__(self, parent, id, pos, size, style)
        # Configure various defaults and user preferences.
        self.config()

    def config(self):
        """Configure shell based on user preferences."""
        self.SetMarginWidth(1, 0)
        
        self.SetLexer(wxSTC_LEX_PYTHON)
        self.SetKeyWords(0, ' '.join(keyword.kwlist))

        self.setStyles(faces)
        self.SetViewWhiteSpace(0)
        self.SetTabWidth(4)
        self.SetUseTabs(0)
        self.SetReadOnly(1)

    def setStyles(self, faces):
        """Configure font size, typeface and color for lexer."""
        
        # Default style
        self.StyleSetSpec(wxSTC_STYLE_DEFAULT, "face:%(mono)s,size:%(size)d" % faces)

        self.StyleClearAll()

        # Built in styles
        self.StyleSetSpec(wxSTC_STYLE_LINENUMBER, "back:#C0C0C0,face:%(mono)s,size:%(lnsize)d" % faces)
        self.StyleSetSpec(wxSTC_STYLE_CONTROLCHAR, "face:%(mono)s" % faces)
        self.StyleSetSpec(wxSTC_STYLE_BRACELIGHT, "fore:#0000FF,back:#FFFF88")
        self.StyleSetSpec(wxSTC_STYLE_BRACEBAD, "fore:#FF0000,back:#FFFF88")

        # Python styles
        self.StyleSetSpec(wxSTC_P_DEFAULT, "face:%(mono)s" % faces)
        self.StyleSetSpec(wxSTC_P_COMMENTLINE, "fore:#007F00,face:%(mono)s" % faces)
        self.StyleSetSpec(wxSTC_P_NUMBER, "")
        self.StyleSetSpec(wxSTC_P_STRING, "fore:#7F007F,face:%(mono)s" % faces)
        self.StyleSetSpec(wxSTC_P_CHARACTER, "fore:#7F007F,face:%(mono)s" % faces)
        self.StyleSetSpec(wxSTC_P_WORD, "fore:#00007F,bold")
        self.StyleSetSpec(wxSTC_P_TRIPLE, "fore:#7F0000")
        self.StyleSetSpec(wxSTC_P_TRIPLEDOUBLE, "fore:#000033,back:#FFFFE8")
        self.StyleSetSpec(wxSTC_P_CLASSNAME, "fore:#0000FF,bold")
        self.StyleSetSpec(wxSTC_P_DEFNAME, "fore:#007F7F,bold")
        self.StyleSetSpec(wxSTC_P_OPERATOR, "")
        self.StyleSetSpec(wxSTC_P_IDENTIFIER, "")
        self.StyleSetSpec(wxSTC_P_COMMENTBLOCK, "fore:#7F7F7F")
        self.StyleSetSpec(wxSTC_P_STRINGEOL, "fore:#000000,face:%(mono)s,back:#E0C0E0,eolfilled" % faces)

    def SetText(self, *args, **kwds):
        self.SetReadOnly(0)
        wxStyledTextCtrl.SetText(self, *args, **kwds)
        self.SetReadOnly(1)


class Filling(wxSplitterWindow):
    """PyCrust Filling based on wxSplitterWindow."""
    
    name = 'PyCrust Filling'
    revision = __version__
    
    def __init__(self, parent, id=-1, pos=wxDefaultPosition, \
                 size=wxDefaultSize, style=wxSP_3D, name='Filling Window', \
                 rootObject=None, rootLabel=None, rootIsNamespace=0):
        """Create a PyCrust Filling instance."""
        wxSplitterWindow.__init__(self, parent, id, pos, size, style, name)
        self.fillingTree = FillingTree(parent=self, rootObject=rootObject, \
                                       rootLabel=rootLabel, \
                                       rootIsNamespace=rootIsNamespace)
        self.fillingText = FillingText(parent=self)
        self.SplitVertically(self.fillingTree, self.fillingText, 200)
        self.SetMinimumPaneSize(1)
        # Override the filling so that descriptions go to fillingText.
        self.fillingTree.setText = self.fillingText.SetText
        # Select the root item.
        self.fillingTree.SelectItem(self.fillingTree.root)


class FillingFrame(wxFrame):
    """Frame containing the PyCrust filling, or namespace tree component."""
    
    name = 'PyCrust Filling Frame'
    revision = __version__
    
    def __init__(self, parent=None, id=-1, title='PyFilling', \
                 pos=wxDefaultPosition, size=wxDefaultSize, \
                 style=wxDEFAULT_FRAME_STYLE, rootObject=None, \
                 rootLabel=None, rootIsNamespace=0):
        """Create a PyCrust FillingFrame instance."""
        wxFrame.__init__(self, parent, id, title, pos, size, style)
        intro = 'Welcome To PyFilling - The Tastiest Namespace Inspector'
        self.CreateStatusBar()
        self.SetStatusText(intro)
        if wxPlatform == '__WXMSW__':
            import os
            filename = os.path.join(os.path.dirname(__file__), 'PyCrust.ico')
            icon = wxIcon(filename, wxBITMAP_TYPE_ICO)
            self.SetIcon(icon)
        self.filling = Filling(parent=self, rootObject=rootObject, \
                               rootLabel=rootLabel, \
                               rootIsNamespace=rootIsNamespace)
        # Override the filling so that status messages go to the status bar.
        self.filling.fillingTree.setStatusText = self.SetStatusText


class App(wxApp):
    """PyFilling standalone application."""
    
    def OnInit(self):
        self.fillingFrame = FillingFrame()
        self.fillingFrame.Show(true)
        self.SetTopWindow(self.fillingFrame)
        return true


  
   