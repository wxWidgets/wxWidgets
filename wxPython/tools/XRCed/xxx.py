# Name:         xxx.py ('xxx' is easy to distinguish from 'wx' :) )
# Purpose:      XML interface classes
# Author:       Roman Rolinsky <rolinsky@mema.ucl.ac.be>
# Created:      22.08.2001
# RCS-ID:       $Id$

from wxPython.wx import *
from wxPython.xrc import *
from xml.dom import minidom
import wxPython.lib.wxpTag

from params import *

# Parameter value class
class xxxParam:
    # Standard use: for text nodes
    def __init__(self, node):
        self.node = node
        if not node.hasChildNodes():
            # If does not have child nodes, create empty text node
            text = tree.dom.createTextNode('')
            node.appendChild(text)
        else:
            text = node.childNodes[0] # first child must be text node
            assert text.nodeType == minidom.Node.TEXT_NODE
        self.textNode = text
    # Value returns string
    def value(self):
        return str(self.textNode.data)
    def update(self, value):
        self.textNode.data = value
    def remove(self):
        self.node.parentNode.removeChild(self.node)
        self.node.unlink()

# Content parameter
class xxxParamContent:
    def __init__(self, node):
        self.node = node
        data, l = [], []                # data is needed to quicker value retrieval
        nodes = node.childNodes[:]      # make a copy of the child list
        for n in nodes:
            if n.nodeType == minidom.Node.ELEMENT_NODE:
                assert n.tagName == 'item', 'bad content content'
                if not n.hasChildNodes():
                    # If does not have child nodes, create empty text node
                    text = tree.dom.createTextNode('')
                    node.appendChild(text)
                else:
                    # !!! normalize?
                    text = n.childNodes[0] # first child must be text node
                    assert text.nodeType == minidom.Node.TEXT_NODE
                l.append(text)
                data.append(text.data)
            else:                       # remove other
                node.removeChild(n)
                n.unlink()
        self.l, self.data = l, data
    def value(self):
        return self.data
    def update(self, value):
        # If number if items is not the same, recreate children
        if len(value) != len(self.l):   # remove first if number of items has changed
            for n in self.node.childNodes:
                self.node.removeChild(n)
            l = []
            for str in value:
                itemElem = tree.dom.createElement('item')
                itemText = tree.dom.createTextNode(str)
                itemElem.appendChild(itemText)
                self.node.appendChild(itemElem)
                l.append(itemText)
        else:
            for i in range(len(value)):
                self.l[i].data = value[i]
        self.data = value

################################################################################

# Classes to interface DOM objects
class xxxObject:
    # Default behavior
    hasChildren = false                 # has children elements?
    hasStyle = true                     # almost everyone
    hasName = true                      # has name attribute?
    isSizer = hasChild = false
    # Style parameters (all optional)
    styles = ['fg', 'bg', 'font', 'enabled', 'focused', 'hidden', 'tooltip']
    # Special parameters
    specials = []
    # Required paremeters: none by default
    required = []
    # Default parameters with default values
    default = {}
    # Parameter types
    paramDict = {}
    # Window styles and extended styles
    winStyles = []
    # Tree icon index
    #image = -1
    # Construct a new xxx object from DOM element
    # parent is parent xxx object (or None if none), element is DOM element object
    def __init__(self, parent, element):
        self.parent = parent
        self.element = element
        self.undo = None
        # Get attributes
        self.className = element.getAttribute('class')
        if self.hasName: self.name = element.getAttribute('name')
        # Set parameters (text element children)
        self.params = {}
        nodes = element.childNodes[:]
        for node in nodes:
            if node.nodeType == minidom.Node.ELEMENT_NODE:
                tag = node.tagName
                if tag == 'object':
                    continue            # do nothing for object children here
                if not tag in self.allParams and not tag in self.styles:
                    print 'WARNING: unknown parameter for %s: %s' % \
                          (self.className, tag)
                elif tag in self.specials:
                    self.special(tag, node)
                elif tag == 'content':
                    self.params[tag] = xxxParamContent(node)
                elif tag == 'font': # has children
                    self.params[tag] = xxxParamFont(self, node)
                else:                   # simple parameter
                    self.params[tag] = xxxParam(node)
            else:
                # Remove all other nodes
                element.removeChild(node)
                node.unlink()
    # Generate HTML
    def generateHtml(self, prefix=''):
        SetCurrentXXX(self)
        html = '<table cellspacing=0 cellpadding=0><tr><td width=120>\
<font size="+1"><b>%s</b></font></td>' % self.className
        # Has id (name) attribute
        if self.hasName:
            html += """\
<td><wxp module="xxx" class="ParamText" width=150>
<param name="name" value="data_name">
</wxp></td>"""
        html += '</table><p>'
        html += '<table cellspacing=0 cellpadding=0>\n'
        # Add required parameters
        for param in self.allParams:
            # Add checkbox or just text
            if param in self.required:
                html += '<tr><td width=20></td><td width=100>%s: </td>' % param
            else:                       # optional parameter
                html += """\
<tr><td width=20><wxp class="wxCheckBox">
<param name="id" value="%d">
<param name="size" value="(20,-1)">
<param name="name" value="%s">
<param name="label" value=("")>
</wxp></td><td width=100>%s: </td>
""" % (paramIDs[param], prefix + 'check_' + param, param)
            # Get parameter type
            try:
                # Local or overriden type
                typeClass = self.paramDict[param].__name__
            except KeyError:
                try:
                    # Standart type
                    typeClass = paramDict[param].__name__
                except KeyError:
                    # Default
                    typeClass = 'ParamText'
            html += """\
<td><wxp module="xxx" class="%s">
<param name="id" value="%d">
<param name="name" value="%s">
</wxp></td>
""" % (typeClass, -1, prefix + 'data_' + param)
        html += '</table>\n'
        return html
    # Returns real tree object
    def treeObject(self):
        if self.hasChild: return self.child
        return self
    # Returns tree image index
    def treeImage(self):
        if self.hasChild: return self.child.treeImage()
        return self.image
    # Class name plus wx name
    def treeName(self):
        if self.hasChild: return self.child.treeName()
        if self.hasName and self.name: return self.className + ' "' + self.name + '"'
        return self.className

################################################################################

class xxxParamFont(xxxParam):
    allParams = ['size', 'style', 'weight', 'family', 'underlined',
                 'face', 'encoding']
    def __init__(self, parent, element):
        xxxObject.__init__(self, parent, element)
        self.parentNode = element       # required to behave similar to DOM node
        v = []
        for p in self.allParams:
            try:
                v.append(str(self.params[p].data))
            except KeyError:
                v.append('')
        self.data = v
    def update(self, value):
        # `value' is a list of strings corresponding to all parameters
        elem = self.element
        for node in elem.childNodes:
            elem.removeChild(node)
        i = 0
        self.params.clear()
        v = []
        for param in self.allParams:
            if value[i]:
                fontElem = tree.dom.createElement(param)
                textNode = tree.dom.createTextNode(value[i])
                self.params[param] = textNode
                fontElem.appendChild(textNode)
                elem.appendChild(fontElem)
            v.append(value[i])
            i += 1
        self.data = v

################################################################################

class xxxContainer(xxxObject):
    hasChildren = true

################################################################################
# Top-level windwos

class xxxPanel(xxxContainer):
    allParams = ['pos', 'size', 'style']
    styles = ['fg', 'bg', 'font', 'enabled', 'focused', 'hidden', 'exstyle',
              'tooltip']
    winStyles = ['wxNO_3D', 'wxTAB_TRAVERSAL', 'wxCLIP_CHILDREN']
    exStyles = ['wxWS_EX_VALIDATE_RECURSIVELY']

class xxxDialog(xxxContainer):
    allParams = ['title', 'pos', 'size', 'style']
    required = ['title']
    winStyles = ['wxDEFAULT_DIALOG_STYLE', 'wxSTAY_ON_TOP',
                 'wxDIALOG_MODAL', 'wxDIALOG_MODELESS',
                 'wxCAPTION', 'wxSYSTEM_MENU', 'wxRESIZE_BORDER', 'wxRESIZE_BOX',
                 'wxTHICK_FRAME', 
                 'wxNO_3D', 'wxTAB_TRAVERSAL', 'wxCLIP_CHILDREN']
    styles = ['fg', 'bg', 'font', 'enabled', 'focused', 'hidden', 'exstyle',
              'tooltip']
    exStyles = ['wxWS_EX_VALIDATE_RECURSIVELY']

class xxxFrame(xxxContainer):
    allParams = ['title', 'centered', 'pos', 'size', 'style']
    paramDict = {'centered': ParamBool}
    required = ['title']
    winStyles = ['wxDEFAULT_FRAME_STYLE', 'wxDEFAULT_DIALOG_STYLE',
                 'wxSTAY_ON_TOP', 
                 'wxCAPTION', 'wxSYSTEM_MENU', 'wxRESIZE_BORDER',
                 'wxRESIZE_BOX', 'wxMINIMIZE_BOX', 'wxMAXIMIZE_BOX',
                 'wxFRAME_FLOAT_ON_PARENT', 'wxFRAME_TOOL_WINDOW',
                 'wxNO_3D', 'wxTAB_TRAVERSAL', 'wxCLIP_CHILDREN']
    styles = ['fg', 'bg', 'font', 'enabled', 'focused', 'hidden', 'exstyle',
              'tooltip']
    exStyles = ['wxWS_EX_VALIDATE_RECURSIVELY']

class xxxTool(xxxObject):
    allParams = ['bitmap', 'bitmap2', 'toggle', 'tooltip', 'longhelp']
    paramDict = {'bitmap2': ParamFile}
    hasStyle = false

class xxxToolBar(xxxContainer):
    allParams = ['bitmapsize', 'margins', 'packing', 'separation', 
                 'pos', 'size', 'style']
    hasStyle = false
    paramDict = {'bitmapsize': ParamPosSize, 'margins': ParamPosSize,
                 'packing': ParamInt, 'separation': ParamInt,
                 'style': ParamNonGenericStyle}
    winStyles = ['wxTB_FLAT', 'wxTB_DOCKABLE', 'wxTB_VERTICAL', 'wxTB_HORIZONTAL']

################################################################################
# Bitmap, Icon

class xxxBitmap(xxxObject):
    allParams = ['bitmap']
    required = ['bitmap']

class xxxIcon(xxxObject):
    allParams = ['icon']
    required = ['icon']

################################################################################
# Controls

class xxxStaticText(xxxObject):
    allParams = ['label', 'pos', 'size', 'style']
    required = ['label']
    winStyles = ['wxALIGN_LEFT', 'wxALIGN_RIGHT', 'wxALIGN_CENTRE', 'wxST_NO_AUTORESIZE']

class xxxStaticLine(xxxObject):
    allParams = ['pos', 'size', 'style']
    winStyles = ['wxLI_HORIZONTAL', 'wxLI_VERTICAL']

class xxxStaticBitmap(xxxObject):
    allParams = ['bitmap', 'pos', 'size', 'style']
    required = ['bitmap']

class xxxTextCtrl(xxxObject):
    allParams = ['value', 'pos', 'size', 'style']
    winStyles = ['wxTE_PROCESS_ENTER', 'wxTE_PROCESS_TAB', 'wxTE_MULTILINE',
              'wxTE_PASSWORD', 'wxTE_READONLY', 'wxHSCROLL']

class xxxChoice(xxxObject):
    allParams = ['content', 'selection', 'pos', 'size', 'style']
    required = ['content']
    winStyles = ['wxCB_SORT']

class xxxSlider(xxxObject):
    allParams = ['value', 'min', 'max', 'pos', 'size', 'style',
                 'tickfreq', 'pagesize', 'linesize', 'thumb', 'tick',
                 'selmin', 'selmax']
    paramDict = {'value': ParamInt, 'tickfreq': ParamInt, 'pagesize': ParamInt,
                 'linesize': ParamInt, 'thumb': ParamInt, 'thumb': ParamInt,
                 'tick': ParamInt, 'selmin': ParamInt, 'selmax': ParamInt}
    required = ['value', 'min', 'max']
    winStyles = ['wxSL_HORIZONTAL', 'wxSL_VERTICAL', 'wxSL_AUTOTICKS', 'wxSL_LABELS',
                 'wxSL_LEFT', 'wxSL_RIGHT', 'wxSL_TOP', 'wxSL_BOTTOM',
                 'wxSL_BOTH', 'wxSL_SELRANGE']

class xxxGauge(xxxObject):
    allParams = ['range', 'pos', 'size', 'style', 'value', 'shadow', 'bezel']
    paramDict = {'range': ParamInt, 'value': ParamInt,
                 'shadow': ParamInt, 'bezel': ParamInt}
    winStyles = ['wxGA_HORIZONTAL', 'wxGA_VERTICAL', 'wxGA_PROGRESSBAR', 'wxGA_SMOOTH']

class xxxScrollBar(xxxObject):
    allParams = ['pos', 'size', 'style', 'value', 'thumbsize', 'range', 'pagesize']
    paramDict = {'value': ParamInt, 'range': ParamInt, 'thumbsize': ParamInt,
                 'pagesize': ParamInt}
    winStyles = ['wxSB_HORIZONTAL', 'wxSB_VERTICAL']

class xxxListCtrl(xxxObject):
    allParams = ['pos', 'size', 'style']
    winStyles = ['wxLC_LIST', 'wxLC_REPORT', 'wxLC_ICON', 'wxLC_SMALL_ICON',
              'wxLC_ALIGN_TOP', 'wxLC_ALIGN_LEFT', 'wxLC_AUTOARRANGE',
              'wxLC_USER_TEXT', 'wxLC_EDIT_LABELS', 'wxLC_NO_HEADER',
              'wxLC_SINGLE_SEL', 'wxLC_SORT_ASCENDING', 'wxLC_SORT_DESCENDING']

# !!! temporary
xxxCheckList = xxxListCtrl

class xxxTreeCtrl(xxxObject):
    allParams = ['pos', 'size', 'style']
    winStyles = ['wxTR_HAS_BUTTONS', 'wxTR_NO_LINES', 'wxTR_LINES_AT_ROOT',
              'wxTR_EDIT_LABELS', 'wxTR_MULTIPLE']

class xxxHtmlWindow(xxxObject):
    allParams = ['pos', 'size', 'style', 'borders', 'url', 'htmlcode']
    paramDict = {'borders': ParamInt}
    winStyles = ['wxHW_SCROLLBAR_NEVER', 'wxHW_SCROLLBAR_AUTO']

class xxxCalendar(xxxObject):
    allParams = ['pos', 'size', 'style']

class xxxNotebook(xxxContainer):
    allParams = ['usenotebooksizer', 'pos', 'size', 'style']
    paramDict = {'usenotebooksizer': ParamBool}
    winStyles = ['wxNB_FIXEDWIDTH', 'wxNB_LEFT', 'wxNB_RIGHT', 'wxNB_BOTTOM']

################################################################################
# Buttons

class xxxButton(xxxObject):
    allParams = ['label', 'default', 'pos', 'size', 'style']
    paramDict = {'default': ParamBool}
    required = ['label']
    winStyles = ['wxBU_LEFT', 'wxBU_TOP', 'wxBU_RIGHT', 'wxBU_BOTTOM']

class xxxBitmapButton(xxxObject):
    allParams = ['bitmap', 'selected', 'focus', 'disabled', 'default',
                 'pos', 'size', 'style']
    required = ['bitmap']
    winStyles = ['wxBU_AUTODRAW', 'wxBU_LEFT', 'wxBU_TOP',
                 'wxBU_RIGHT', 'wxBU_BOTTOM']

class xxxRadioButton(xxxObject):
    allParams = ['label', 'value', 'pos', 'size', 'style']
    paramDict = {'value': ParamBool}
    required = ['label']
    winStyles = ['wxRB_GROUP']

class xxxSpinButton(xxxObject):
    allParams = ['pos', 'size', 'style']
    winStyles = ['wxSP_HORIZONTAL', 'wxSP_VERTICAL', 'wxSP_ARROW_KEYS', 'wxSP_WRAP']

################################################################################
# Boxes

class xxxStaticBox(xxxObject):
    allParams = ['label', 'pos', 'size', 'style']
    required = ['label']

class xxxRadioBox(xxxObject):
    allParams = ['label', 'content', 'selection', 'dimension', 'pos', 'size', 'style']
    paramDict = {'dimension': ParamInt}
    required = ['label', 'content']
    winStyles = ['wxRA_SPECIFY_ROWS', 'wxRA_SPECIFY_COLS']

class xxxCheckBox(xxxObject):
    allParams = ['label', 'pos', 'size', 'style']
    required = ['label']

class xxxComboBox(xxxObject):
    allParams = ['content', 'selection', 'value', 'pos', 'size', 'style']
    required = ['content']
    winStyles = ['wxCB_SIMPLE', 'wxCB_SORT', 'wxCB_READONLY', 'wxCB_DROPDOWN']

class xxxListBox(xxxObject):
    allParams = ['content', 'selection', 'pos', 'size', 'style']
    required = ['content']
    winStyles = ['wxLB_SINGLE', 'wxLB_MULTIPLE', 'wxLB_EXTENDED', 'wxLB_HSCROLL',
              'wxLB_ALWAYS_SB', 'wxLB_NEEDED_SB', 'wxLB_SORT']

################################################################################
# Sizers

class xxxSizer(xxxContainer):
    hasName = hasStyle = false
    paramDict = {'orient': ParamOrient}
    isSizer = true

class xxxBoxSizer(xxxSizer):
    allParams = ['orient']
    required = ['orient']
    default = {'orient': 'wxVERTICAL'}
    # Tree icon depends on orientation
    def treeImage(self):
        if self.params['orient'].value() == 'wxHORIZONTAL': return self.imageH
        else: return self.imageV

class xxxStaticBoxSizer(xxxBoxSizer):
    allParams = ['label', 'orient']
    required = ['label', 'orient']
    default = {'orient': 'wxVERTICAL'}

class xxxGridSizer(xxxSizer):
    allParams = ['cols', 'rows', 'vgap', 'hgap']
    required = ['cols']
    default = {'cols': '2', 'rows': '2'}

# For repeated parameters
class xxxParamMulti:
    def __init__(self):
        self.l, self.data = [], []
    def append(self, param):
        self.l.append(param)
        self.data.append(param.value())
    def value(self):
        return self.data
    def remove(self):
        for param in self.l:
            param.remove()
        self.l, self.data = [], []

class xxxFlexGridSizer(xxxGridSizer):
    specials = ['growablecols', 'growablerows']
    allParams = ['cols', 'rows', 'vgap', 'hgap'] + specials
    paramDict = {'growablecols':ParamContent, 'growablerows':ParamContent}
    # Special processing for growable* parameters
    # (they are represented by several nodes)
    def special(self, tag, node):
        if tag not in self.params.keys():
            self.params[tag] = xxxParamMulti()
        self.params[tag].append(xxxParam(node))
    def setSpecial(self, param, value):
        # Straightforward implementation: remove, add again
        self.params[param].remove()
        del self.params[param]
        for str in value:
            node = tree.dom.createElement(param)
            text = tree.dom.createTextNode(str)
            node.appendChild(text)
            self.element.appendChild(node)
            self.special(param, node)

# Container with only one child.
# Not shown in tree.
class xxxChildContainer(xxxObject):
    hasName = hasStyle = false
    hasChild = true
    def __init__(self, parent, element):
        xxxObject.__init__(self, parent, element)
        # Must have one child with 'object' tag, but we don't check it
        nodes = element.childNodes[:]   # create copy
        for node in nodes:
            if node.nodeType == minidom.Node.ELEMENT_NODE:
                if node.tagName == 'object':
                    # Create new xxx object for child node
                    self.child = MakeXXXFromDOM(self, node)
                    self.child.parent = parent
                    # Copy hasChildren and isSizer attributes
                    self.hasChildren = self.child.hasChildren
                    self.isSizer = self.child.isSizer
                    return              # success
            else:
                element.removeChild(node)
                node.unlink()
        assert 0, 'no child found'
    def generateHtml(self):
        return xxxObject.generateHtml(self, '_') + '<hr>\n' + \
               self.child.generateHtml()

class xxxSizerItem(xxxChildContainer):
    allParams = ['option', 'flag', 'border']
    paramDict = {'option': ParamInt}
    def __init__(self, parent, element):
        xxxChildContainer.__init__(self, parent, element)
        # Remove pos parameter - unnecessary for sizeritems
        if 'pos' in self.child.allParams:
            self.child.allParams = self.child.allParams[:]
            self.child.allParams.remove('pos')

class xxxNotebookPage(xxxChildContainer):
    allParams = ['label', 'selected']
    paramDict = {'selected': ParamBool}
    required = ['label']
    def __init__(self, parent, element):
        xxxChildContainer.__init__(self, parent, element)
        # pos and size dont matter for notebookpages
        if 'pos' in self.child.allParams:
            self.child.allParams = self.child.allParams[:]
            self.child.allParams.remove('pos')
        if 'size' in self.child.allParams:
            self.child.allParams = self.child.allParams[:]
            self.child.allParams.remove('size')

class xxxSpacer(xxxObject):
    hasName = hasStyle = false
    allParams = ['size', 'option', 'flag', 'border']
    paramDict = {'option': ParamInt}
    default = {'size': '0,0'}

class xxxMenuBar(xxxContainer):
    allParams = []

class xxxMenu(xxxContainer):
    allParams = ['label']
    default = {'label': ''}
    paramDict = {'style': ParamNonGenericStyle}    # no generic styles
    winStyles = ['wxMENU_TEAROFF']

class xxxMenuItem(xxxObject):
    allParams = ['checkable', 'label', 'accel', 'help']
    default = {'label': ''}

class xxxSeparator(xxxObject):
    hasName = hasStyle = false
    allParams = []

################################################################################

xxxDict = {
    'wxPanel': xxxPanel,
    'wxDialog': xxxDialog,
    'wxFrame': xxxFrame,
    'tool': xxxTool,
    'wxToolBar': xxxToolBar,
    
    'wxBitmap': xxxBitmap,
    'wxIcon': xxxIcon,

    'wxButton': xxxButton,
    'wxBitmapButton': xxxBitmapButton,
    'wxRadioButton': xxxRadioButton,
    'wxSpinButton': xxxSpinButton,

    'wxStaticBox': xxxStaticBox,
    'wxStaticBitmap': xxxStaticBitmap,
    'wxRadioBox': xxxRadioBox,
    'wxComboBox': xxxComboBox,
    'wxCheckBox': xxxCheckBox,
    'wxListBox': xxxListBox,

    'wxStaticText': xxxStaticText,
    'wxStaticLine': xxxStaticLine,
    'wxTextCtrl': xxxTextCtrl,
    'wxChoice': xxxChoice,
    'wxSlider': xxxSlider,
    'wxGauge': xxxGauge,
    'wxScrollBar': xxxScrollBar,
    'wxTreeCtrl': xxxTreeCtrl,
    'wxListCtrl': xxxListCtrl,
    'wxCheckList': xxxCheckList,
    'wxNotebook': xxxNotebook,
    'notebookpage': xxxNotebookPage,
    'wxHtmlWindow': xxxHtmlWindow,
    'wxCalendar': xxxCalendar,
    
    'wxBoxSizer': xxxBoxSizer,
    'wxStaticBoxSizer': xxxStaticBoxSizer,
    'wxGridSizer': xxxGridSizer,
    'wxFlexGridSizer': xxxFlexGridSizer,
    'sizeritem': xxxSizerItem,
    'spacer': xxxSpacer,

    'wxMenuBar': xxxMenuBar,
    'wxMenu': xxxMenu,
    'wxMenuItem': xxxMenuItem,
    'separator': xxxSeparator,
    }

# Create IDs for all parameters of all classes
paramIDs = {'fg': wxNewId(), 'bg': wxNewId(), 'exstyle': wxNewId(), 'font': wxNewId(),
            'enabled': wxNewId(), 'focused': wxNewId(), 'hidden': wxNewId(),
            'tooltip': wxNewId()
            }
for cl in xxxDict.values():
    for param in cl.allParams + cl.paramDict.keys():
        if not paramIDs.has_key(param):
            paramIDs[param] = wxNewId()

################################################################################
# Helper functions

# Test for object elements
def IsObject(node):
    return node.nodeType == minidom.Node.ELEMENT_NODE and node.tagName == 'object'

# Make XXX object from some DOM object, selecting correct class
def MakeXXXFromDOM(parent, element):
    try:
        return xxxDict[element.getAttribute('class')](parent, element)
    except KeyError:
        # Verify that it's not recursive exception
        if element.getAttribute('class') not in xxxDict.keys():
            print 'ERROR: unknown class:', element.getAttribute('class')
        raise

# Make empty DOM element
def MakeEmptyDOM(className): 
    elem = tree.dom.createElement('object')
    elem.setAttribute('class', className)
    # Set required and default parameters
    xxxClass = xxxDict[className]
    defaultNotRequired = filter(lambda x, l=xxxClass.required: x not in l,
                                xxxClass.default.keys())
    for param in xxxClass.required + defaultNotRequired:
        textElem = tree.dom.createElement(param)
        try:
            textNode = tree.dom.createTextNode(xxxClass.default[param])
        except KeyError:
            textNode = tree.dom.createTextNode('')
        textElem.appendChild(textNode)
        elem.appendChild(textElem)
    return elem

# Make empty XXX object
def MakeEmptyXXX(parent, className):
    # Make corresponding DOM object first
    elem = MakeEmptyDOM(className)
    # If parent is a sizer, we should create sizeritem object, except for spacers
    if parent:
        if parent.isSizer and className != 'spacer':
            sizerItemElem = MakeEmptyDOM('sizeritem')
            sizerItemElem.appendChild(elem)
            elem = sizerItemElem
        elif isinstance(parent, xxxNotebook):
            pageElem = MakeEmptyDOM('notebookpage')
            pageElem.appendChild(elem)
            elem = pageElem
    # Now just make object
    return MakeXXXFromDOM(parent, elem)

