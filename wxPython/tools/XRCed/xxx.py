# Name:         xxx.py (xxx is easy to distinguish from 'wx' :) )
# Purpose:      XML interface classes
# Author:       Roman Rolinsky <rolinsky@mema.ucl.ac.be>
# Created:      22.08.2001

from wxPython.wx import *
from wxPython.xrc import *
from xml.dom import minidom
import wxPython.lib.wxpTag

from params import *

# Classes to interface DOM objects
class xxxObject:
    # Param ids for controls
    ID_CHECK_PARAMS = wxNewId()
    ID_TEXT_PARAMS = wxNewId()
    # Default behavior
    hasChildren = false                 # has children elements?
    hasName = true                      # has name attribute?
    isSizer = hasChild = false
    # Required paremeters: none by default
    required = []
    # Default parameters with default values
    default = {}
    # Parameter types
    paramDict = {}
    # Additional styles
    styles = []
    # Tree icon index
    image = -1
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
                if node.tagName == 'object':
                    continue            # do nothing for object children here
                if not node.tagName in self.allParams:
                    print 'WARNING: unknown parameter for %s: %s' % \
                          (self.className, node.tagName)
                if node.tagName == 'content': # has items
                    # Param value is a list of text nodes
                    l = []
                    nodes = node.childNodes[:]
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
                        else:
                            node.removeChild(n)
                            n.unlink()
                    self.params[node.tagName] = l
                else:                   # simple parameter
                    if not node.hasChildNodes():
                        # If does not have child nodes, create empty text node
                        text = tree.dom.createTextNode('')
                        node.appendChild(text)
                    else:
                        text = node.childNodes[0] # first child must be text node
                        assert text.nodeType == minidom.Node.TEXT_NODE
                    self.params[node.tagName] = text
            else:
                # Remove all other nodes
                element.removeChild(node)
                node.unlink()
                
    # Generate HTML
    def generateHtml(self, prefix=''):
        SetCurrentXXX(self)
        html = '<table cellspacing=0 cellpadding=0><tr><td width=130>\
<font size="+1"><b>%s</b></font></td>' % self.className
        # Has id (name) attribute
        if self.hasName:
            html += """\
<td><wxp module="xxx" class="ParamText" width=150>
<param name="id" value="%d">
<param name="name" value="text_name">
<param name="value" value='("%s")'>
<param name="param" value="name">
</wxp></td>""" % (self.ID_TEXT_PARAMS, self.name)
        html += '</table><p>'
        html += '<table cellspacing=0 cellpadding=0>\n'
        # Add required parameters
        for param in self.allParams:
            # Add checkbox or just text
            if param in self.required:
                html += '<tr><td width=25></td><td width=110>%s: </td>' % param
            else:                       # optional parameter
                html += """\
<tr><td width=20><wxp class="wxCheckBox">
<param name="id" value="%d">
<param name="size" value="(20,-1)">
<param name="name" value="check_%s">
<param name="label" value=("")>
</wxp></td><td width=110>%s: </td>
""" % (self.ID_CHECK_PARAMS, param, param + '&nbsp;')
            # Add value part
            if self.params.has_key(param):
                if param == 'content':
                    l = []
                    for text in self.params[param]:
                        l.append(str(text.data)) # convert from unicode
                    value = str(l)
                else:
                    value = "('" + self.params[param].data + "')"
            else:
                value = "('')"
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
<param name="name" value="text_%s">
<param name="value" value="%s">
<param name="param" value="%s">
</wxp></td>
""" % (typeClass, self.ID_TEXT_PARAMS,
       prefix + param, value, prefix + param)
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

class xxxContainer(xxxObject):
    hasChildren = true

################################################################################
# Top-level windwos

class xxxPanel(xxxContainer):
    allParams = ['pos', 'size', 'style']

class xxxDialog(xxxContainer):
    allParams = ['title', 'pos', 'size', 'style']
    required = ['title']
    styles = ['wxDIALOG_MODAL', 'wxCAPTION', 'wxDEFAULT_DIALOG_STYLE',
              'wxRESIZE_BORDER', 'wxSYSTEM_MENU',  'wxTHICK_FRAME', 'wxSTAY_ON_TOP']

class xxxFrame(xxxContainer):
    allParams = ['title', 'centered', 'pos', 'size', 'style']
    paramDict = {'centered': ParamBool}
    required = ['title']
    styles = ['wxDEFAULT_FRAME_STYLE', 'wxICONIZE', 'wxCAPTION', 'wxMINIMIZE',
              'wxICONIZE', 'wxMINIMIZE_BOX', 'wxMAXIMIZE', 'wxMAXIMIZE_BOX',
              'wxSTAY_ON_TOP', 'wxSYSTEM_MENU', 'wxRESIZE_BORDER',
              'wxFRAME_FLOAT_ON_PARENT', 'wxFRAME_TOOL_WINDOW']

################################################################################
# Controls

class xxxStaticText(xxxObject):
    allParams = ['label', 'pos', 'size', 'style']
    required = ['label']
    styles = ['wxALIGN_LEFT', 'wxALIGN_RIGHT', 'wxALIGN_CENTRE', 'wxST_NO_AUTORESIZE']

class xxxStaticLine(xxxObject):
    allParams = ['pos', 'size', 'style']
    styles = ['wxLI_HORIZONTAL', 'wxLI_VERTICAL']

class xxxTextCtrl(xxxObject):
    allParams = ['value', 'pos', 'size', 'style']
    styles = ['wxTE_PROCESS_ENTER', 'wxTE_PROCESS_TAB', 'wxTE_MULTILINE',
              'wxTE_PASSWORD', 'wxTE_READONLY']

class xxxChoice(xxxObject):
    allParams = ['content', 'selection', 'pos', 'size', 'style']
    required = ['content']

class xxxSlider(xxxObject):
    allParams = ['value', 'min', 'max', 'pos', 'size', 'style',
                 'tickfreq', 'pagesize', 'linesize', 'thumb', 'tick',
                 'selmin', 'selmax']
    paramDict = {'value': ParamInt, 'tickfreq': ParamInt, 'pagesize': ParamInt,
                 'linesize': ParamInt, 'thumb': ParamInt, 'thumb': ParamInt,
                 'tick': ParamInt, 'selmin': ParamInt, 'selmax': ParamInt}
    required = ['value', 'min', 'max']
    styles = ['wxSL_HORIZONTAL', 'wxSL_VERTICAL', 'wxSL_AUTOTICKS', 'wxSL_LABELS',
              'wxSL_LEFT', 'wxSL_RIGHT', 'wxSL_TOP', 'wxSL_SELRANGE']

class xxxGauge(xxxObject):
    allParams = ['range', 'pos', 'size', 'style', 'value', 'shadow', 'bezel']
    paramDict = {'range': ParamInt, 'value': ParamInt,
                 'shadow': ParamInt, 'bezel': ParamInt}
    styles = ['wxGA_HORIZONTAL', 'wxGA_VERTICAL', 'wxGA_PROGRESSBAR', 'wxGA_SMOOTH']

class xxxScrollBar(xxxObject):
    allParams = ['pos', 'size', 'style', 'value', 'thumbsize', 'range', 'pagesize']
    paramDict = {'value': ParamInt, 'range': ParamInt, 'thumbsize': ParamInt,
                 'pagesize': ParamInt}
    styles = ['wxSB_HORIZONTAL', 'wxSB_VERTICAL']

class xxxListCtrl(xxxObject):
    allParams = ['pos', 'size', 'style']
    styles = ['wxLC_LIST', 'wxLC_REPORT', 'wxLC_ICON', 'wxLC_SMALL_ICON',
              'wxLC_ALIGN_TOP', 'wxLC_ALIGN_LEFT', 'wxLC_AUTOARRANGE',
              'wxLC_USER_TEXT', 'wxLC_EDIT_LABELS', 'wxLC_NO_HEADER',
              'wxLC_SINGLE_SEL', 'wxLC_SORT_ASCENDING', 'wxLC_SORT_DESCENDING']

# !!! temporary
xxxCheckList = xxxListCtrl

class xxxTreeCtrl(xxxObject):
    allParams = ['pos', 'size', 'style']
    styles = ['wxTR_HAS_BUTTONS', 'wxTR_NO_LINES', 'wxTR_LINES_AT_ROOT',
              'wxTR_EDIT_LABELS', 'wxTR_MULTIPLE']

class xxxHtmlWindow(xxxObject):
    allParams = ['pos', 'size', 'style', 'borders', 'url', 'htmlcode']
    paramDict = {'borders': ParamInt}
    styles = ['wxHW_SCROLLBAR_NEVER', 'wxHW_SCROLLBAR_AUTO']

class xxxCalendar(xxxObject):
    allParams = ['pos', 'size', 'style']

class xxxNotebook(xxxContainer):
    allParams = ['usenotebooksizer', 'pos', 'size', 'style']
    paramDict = {'usenotebooksizer': ParamBool}
    styles = ['wxNB_FIXEDWIDTH', 'wxNB_LEFT', 'wxNB_RIGHT', 'wxNB_BOTTOM']

################################################################################
# Buttons

class xxxButton(xxxObject):
    allParams = ['label', 'default', 'pos', 'size', 'style']
    paramDict = {'default': ParamBool}
    required = ['label']
    styles = ['wxBU_LEFT', 'wxBU_TOP', 'wxBU_RIGHT', 'wxBU_BOTTOM']

class xxxBitmapButton(xxxObject):
    allParams = ['bitmap', 'selected', 'focus', 'disabled', 'default',
                 'pos', 'size', 'style']
    required = ['bitmap']
    styles = ['wxBU_LEFT', 'wxBU_TOP', 'wxBU_RIGHT', 'wxBU_BOTTOM']

class xxxRadioButton(xxxObject):
    allParams = ['label', 'value', 'pos', 'size', 'style']
    paramDict = {'value': ParamBool}
    required = ['label']
    styles = ['wxRB_GROUP']

class xxxSpinButton(xxxObject):
    allParams = ['pos', 'size', 'style']
    styles = ['wxSP_HORIZONTAL', 'wxSP_VERTICAL', 'wxSP_ARROW_KEYS', 'wxSP_WRAP']

################################################################################
# Boxes

class xxxStaticBox(xxxObject):
    allParams = ['label', 'pos', 'size', 'style']
    required = ['label']

class xxxRadioBox(xxxObject):
    allParams = ['label', 'content', 'selection', 'dimension', 'pos', 'size', 'style']
    paramDict = {'dimension': ParamInt}
    required = ['label', 'content']
    styles = ['wxRA_SPECIFY_ROWS', 'wxRA_SPECIFY_COLS']

class xxxCheckBox(xxxObject):
    allParams = ['label', 'pos', 'size', 'style']
    required = ['label']

class xxxComboBox(xxxObject):
    allParams = ['content', 'selection', 'value', 'pos', 'size', 'style']
    required = ['content']
    styles = ['wxCB_SIMPLE', 'wxCB_DROPDOWN', 'wxCB_READONLY', 'wxCB_DROPDOWN',
              'wxCB_SORT']

class xxxListBox(xxxObject):
    allParams = ['content', 'selection', 'pos', 'size', 'style']
    required = ['content']
    styles = ['wxLB_SINGLE', 'wxLB_MULTIPLE', 'wxLB_EXTENDED', 'wxLB_HSCROLL',
              'wxLB_ALWAYS_SB', 'wxLB_NEEDED_SB', 'wxLB_SORT']

################################################################################
# Sizers

class xxxSizer(xxxContainer):
    hasName = false
    paramDict = {'orient': ParamOrient}
    isSizer = true

class xxxBoxSizer(xxxSizer):
    allParams = ['orient']
    required = ['orient']
    default = {'orient': 'wxVERTICAL'}
    # Tree icon depends on orientation
    def treeImage(self):
        if self.params['orient'].data == 'wxHORIZONTAL': return self.imageH
        else: return self.imageV

class xxxStaticBoxSizer(xxxBoxSizer):
    allParams = ['label', 'orient']
    required = ['label', 'orient']
    default = {'orient': 'wxVERTICAL'}

class xxxGridSizer(xxxSizer):
    allParams = ['cols', 'rows', 'vgap', 'hgap']
    required = ['cols']
    default = {'cols': '2', 'rows': '2'}

class xxxFlexGridSizer(xxxGridSizer):
    pass

# Container with only one child.
# Not shown in tree.
class xxxChildContainer(xxxObject):
    # Special param ids
    ID_CHECK_PARAMS = wxNewId()
    ID_TEXT_PARAMS = wxNewId()
    hasName = false
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
    hasName = false
    allParams = ['size', 'option', 'flag', 'border']
    paramDict = {'option': ParamInt}
    default = {'size': '0,0'}

class xxxMenuBar(xxxContainer):
    allParams = []

class xxxMenu(xxxContainer):
    allParams = ['label']
    default = {'label': ''}

class xxxMenuItem(xxxObject):
    allParams = ['checkable', 'label', 'accel', 'help']
    default = {'label': ''}

class xxxSeparator(xxxObject):
    hasName = false
    allParams = []

xxxDict = {
    'wxPanel': xxxPanel,
    'wxDialog': xxxDialog,
    'wxFrame': xxxFrame,
    
    'wxButton': xxxButton,
    'wxBitmapButton': xxxBitmapButton,
    'wxRadioButton': xxxRadioButton,
    'wxSpinButton': xxxSpinButton,

    'wxStaticBox': xxxStaticBox,
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

# Helper functions

# Test for object elements
def IsObject(node):
    return node.nodeType == minidom.Node.ELEMENT_NODE and node.tagName == 'object'

# Make XXX object from some DOM object, selecting correct class
def MakeXXXFromDOM(parent, element):
    return xxxDict[element.getAttribute('class')](parent, element)

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

