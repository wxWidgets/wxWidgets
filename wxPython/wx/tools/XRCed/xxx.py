# Name:         xxx.py ('xxx' is easy to distinguish from 'wx' :) )
# Purpose:      XML interface classes
# Author:       Roman Rolinsky <rolinsky@mema.ucl.ac.be>
# Created:      22.08.2001
# RCS-ID:       $Id$

from xml.dom import minidom
from globals import *
from params import *
import traceback, types

# Base class for interface parameter classes
class xxxNode:
    def __init__(self, node):
        self.node = node
    def remove(self):
        self.node.parentNode.removeChild(self.node)
        self.node.unlink()

# Generic (text) parameter class
class xxxParam(xxxNode):
    # Standard use: for text nodes
    def __init__(self, node):
        xxxNode.__init__(self, node)
        if not node.hasChildNodes():
            # If does not have child nodes, create empty text node
            text = g.tree.dom.createTextNode('')
            node.appendChild(text)
        else:
            text = node.childNodes[0] # first child must be text node
            assert text.nodeType == minidom.Node.TEXT_NODE
            # Append other text nodes if present and delete them
            extraText = ''
            for n in node.childNodes[1:]:
                if n.nodeType == minidom.Node.TEXT_NODE:
                    extraText += n.data
                    node.removeChild(n)
                    n.unlink()
                else: break
            if extraText: text.data = text.data + extraText
        # Use convertion from unicode to current encoding
        self.textNode = text
    # Value returns string
    if wx.USE_UNICODE:   # no conversion is needed
        def value(self):
            return self.textNode.data
        def update(self, value):
            self.textNode.data = value
    else:
        def value(self):
            try:
                return self.textNode.data.encode(g.currentEncoding)
            except LookupError:
                return self.textNode.data.encode()
        def update(self, value):
            try: # handle exception if encoding is wrong
                self.textNode.data = unicode(value, g.currentEncoding)
            except UnicodeDecodeError:
                self.textNode.data = unicode(value)
                #wx.LogMessage("Unicode error: set encoding in file\nglobals.py to something appropriate")

# Integer parameter
class xxxParamInt(xxxParam):
    # Standard use: for text nodes
    def __init__(self, node):
        xxxParam.__init__(self, node)
    # Value returns string
    def value(self):
        try:
            return int(self.textNode.data)
        except ValueError:
            return -1                   # invalid value
    def update(self, value):
        self.textNode.data = str(value)

# Content parameter
class xxxParamContent(xxxNode):
    def __init__(self, node):
        xxxNode.__init__(self, node)
        data, l = [], []                # data is needed to quicker value retrieval
        nodes = node.childNodes[:]      # make a copy of the child list
        for n in nodes:
            if n.nodeType == minidom.Node.ELEMENT_NODE:
                assert n.tagName == 'item', 'bad content content'
                if not n.hasChildNodes():
                    # If does not have child nodes, create empty text node
                    text = g.tree.dom.createTextNode('')
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
            childNodes = self.node.childNodes[:]
            for n in childNodes:
                self.node.removeChild(n)
            l = []
            for str in value:
                itemElem = g.tree.dom.createElement('item')
                itemText = g.tree.dom.createTextNode(str)
                itemElem.appendChild(itemText)
                self.node.appendChild(itemElem)
                l.append(itemText)
            self.l = l
        else:
            for i in range(len(value)):
                self.l[i].data = value[i]
        self.data = value

# Content parameter for checklist
class xxxParamContentCheckList(xxxNode):
    def __init__(self, node):
        xxxNode.__init__(self, node)
        data, l = [], []                # data is needed to quicker value retrieval
        nodes = node.childNodes[:]      # make a copy of the child list
        for n in nodes:
            if n.nodeType == minidom.Node.ELEMENT_NODE:
                assert n.tagName == 'item', 'bad content content'
                checked = n.getAttribute('checked')
                if not checked: checked = 0
                if not n.hasChildNodes():
                    # If does not have child nodes, create empty text node
                    text = g.tree.dom.createTextNode('')
                    node.appendChild(text)
                else:
                    # !!! normalize?
                    text = n.childNodes[0] # first child must be text node
                    assert text.nodeType == minidom.Node.TEXT_NODE
                l.append((text, n))
                data.append((str(text.data), int(checked)))
            else:                       # remove other
                node.removeChild(n)
                n.unlink()
        self.l, self.data = l, data
    def value(self):
        return self.data
    def update(self, value):
        # If number if items is not the same, recreate children
        if len(value) != len(self.l):   # remove first if number of items has changed
            childNodes = self.node.childNodes[:]
            for n in childNodes:
                self.node.removeChild(n)
            l = []
            for s,ch in value:
                itemElem = g.tree.dom.createElement('item')
                # Add checked only if True
                if ch: itemElem.setAttribute('checked', '1')
                itemText = g.tree.dom.createTextNode(s)
                itemElem.appendChild(itemText)
                self.node.appendChild(itemElem)
                l.append((itemText, itemElem))
            self.l = l
        else:
            for i in range(len(value)):
                self.l[i][0].data = value[i][0]
                self.l[i][1].setAttribute('checked', str(value[i][1]))
        self.data = value

# Bitmap parameter
class xxxParamBitmap(xxxParam):
    def __init__(self, node):
        xxxParam.__init__(self, node)
        self.stock_id = node.getAttribute('stock_id')
    def value(self):
        return [self.stock_id, xxxParam.value(self)]
    def update(self, value):
        self.stock_id = value[0]
        if self.stock_id:
            self.node.setAttribute('stock_id', self.stock_id)
        elif self.node.hasAttribute('stock_id'):
            self.node.removeAttribute('stock_id')
        xxxParam.update(self, value[1])

################################################################################

# Classes to interface DOM objects
class xxxObject:
    # Default behavior
    hasChildren = False                 # has children elements?
    hasStyle = True                     # almost everyone
    hasName = True                      # has name attribute?
    isSizer = hasChild = False
    isElement = True
    allParams = []                     # Some nodes have no parameters
    # Style parameters (all optional)
    styles = ['fg', 'bg', 'font', 'enabled', 'focused', 'hidden', 'tooltip']
    # Special parameters
    specials = []
    # Bitmap tags
    bitmapTags = ['bitmap', 'bitmap2', 'icon']
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
    def __init__(self, parent, element, refElem=None):
        self.parent = parent
        self.node = element
        self.refElem = refElem
        self.undo = None
        # Reference are dereferenced
        if element.tagName == 'object_ref':
            # Find original object
            self.ref = element.getAttribute('ref')
            if refElem:
                self.className = self.refElem.getAttribute('class')
            else:
                self.className = 'xxxUnknown'
            self.required = []
        else:
            # Get attributes
            self.ref = None
            self.className = element.getAttribute('class')
        self.subclass = element.getAttribute('subclass')
        if self.hasName: self.name = element.getAttribute('name')
        # Set parameters (text element children)
        self.params = {}
        for n in element.childNodes[:]:
            if n.nodeType == minidom.Node.ELEMENT_NODE:
                tag = n.tagName
                if tag in ['object', 'object_ref']:
                    continue            # do nothing for object children here
                elif tag not in self.allParams and tag not in self.styles:
                    print 'WARNING: unknown parameter for %s: %s' % \
                          (self.className, tag)
                elif tag in self.specials:
                    self.special(tag, n)
                elif tag == 'content':
                    if self.className == 'wxCheckListBox':
                        self.params[tag] = xxxParamContentCheckList(n)
                    else:
                        self.params[tag] = xxxParamContent(n)
                elif tag == 'font': # has children
                    self.params[tag] = xxxParamFont(element, n)
                elif tag in self.bitmapTags:
                    # Can have attributes
                    self.params[tag] = xxxParamBitmap(n)
                else:                   # simple parameter
                    self.params[tag] = xxxParam(n)
            elif n.nodeType == minidom.Node.TEXT_NODE and n.data.isspace():
                # Remove empty text nodes
                element.removeChild(n)
                n.unlink()

        # Check that all required params are set
        for param in self.required:
            if not self.params.has_key(param):
                # If default is specified, set it
                if self.default.has_key(param):
                    elem = g.tree.dom.createElement(param)
                    if param == 'content':
                        if self.className == 'wxCheckListBox':
                            self.params[param] = xxxParamContentCheckList(elem)
                        else:
                            self.params[param] = xxxParamContent(elem)
                    else:
                        self.params[param] = xxxParam(elem)
                    # Find place to put new element: first present element after param
                    found = False
                    paramStyles = self.allParams + self.styles
                    for p in paramStyles[paramStyles.index(param) + 1:]:
                        # Content params don't have same type
                        if self.params.has_key(p) and p != 'content':
                            found = True
                            break
                    if found:
                        nextTextElem = self.params[p].node
                        self.node.insertBefore(elem, nextTextElem)
                    else:
                        self.node.appendChild(elem)
                else:
                    wx.LogWarning('Required parameter %s of %s missing' %
                                 (param, self.className))
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
        if self.subclass: className = self.subclass
        else: className = self.className
        if self.hasName and self.name: return className + ' "' + self.name + '"'
        return className
    # Class name or subclass
    def panelName(self):
        if self.subclass: name = self.subclass + '(' + self.className + ')'
        name = self.className
        if self.ref: name = 'ref: ' + self.ref + ', ' + name
        return name
    # Sets name of tree object
    def setTreeName(self, name):
        if self.hasChild: obj = self.child
        else: obj = self
        obj.name = name
        obj.node.setAttribute('name', name)
    # Set normal (text) params
    def set(self, param, value):
        try:
            self.params[param].update(value)
        except KeyError:
            elem = g.tree.dom.createElement(param)
            p = xxxParam(elem)
            p.update(value)
            self.params[param] = p
            self.node.appendChild(elem)
    # Special processing for growablecols-like parameters
    # represented by several nodes
    def special(self, tag, node):
        if not self.params.has_key(tag):
            # Create new multi-group
            self.params[tag] = xxxParamMulti(node)
        self.params[tag].append(xxxParamInt(node))
    def setSpecial(self, param, value):
        # Straightforward implementation: remove, add again
        self.params[param].remove()
        del self.params[param]
        for i in value:
            node = g.tree.dom.createElement(param)
            text = g.tree.dom.createTextNode(str(i))
            node.appendChild(text)
            self.node.appendChild(node)
            self.special(param, node)

# Imitation of FindResource/DoFindResource from xmlres.cpp
def DoFindResource(parent, name, classname, recursive):
    for n in parent.childNodes:
        if n.nodeType == minidom.Node.ELEMENT_NODE and \
               n.tagName in ['object', 'object_ref'] and \
               n.getAttribute('name') == name:
            cls = n.getAttribute('class')
            if not classname or cls == classname:  return n
            if not cls or n.tagName == 'object_ref':
                refName = n.getAttribute('ref')
                if not refName:  continue
                refNode = FindResource(refName)
                if refName and refNode.getAttribute('class') == classname:
                    return n
    if recursive:
        for n in parent.childNodes:
            if n.nodeType == minidom.Node.ELEMENT_NODE and \
                   n.tagName in ['object', 'object_ref']:
                found = DoFindResource(n, name, classname, True)
                if found:  return found
def FindResource(name, classname='', recursive=True):
    found = DoFindResource(g.tree.mainNode, name, classname, recursive)
    if found:  return found
    wx.LogError('XRC resource "%s" not found!' % name)
                

################################################################################

# This is a little special: it is both xxxObject and xxxNode
class xxxParamFont(xxxObject, xxxNode):
    allParams = ['size', 'family', 'style', 'weight', 'underlined',
                 'face', 'encoding']
    def __init__(self, parent, element):
        xxxObject.__init__(self, parent, element)
        xxxNode.__init__(self, element)
        self.parentNode = parent       # required to behave similar to DOM node
        v = []
        for p in self.allParams:
            try:
                v.append(str(self.params[p].value()))
            except KeyError:
                v.append('')
        self.data = v
    def update(self, value):
        # `value' is a list of strings corresponding to all parameters
        elem = self.node
        # Remove old elements first
        childNodes = elem.childNodes[:]
        for node in childNodes: elem.removeChild(node)
        i = 0
        self.params.clear()
        v = []
        for param in self.allParams:
            if value[i]:
                fontElem = g.tree.dom.createElement(param)
                textNode = g.tree.dom.createTextNode(value[i])
                self.params[param] = textNode
                fontElem.appendChild(textNode)
                elem.appendChild(fontElem)
            v.append(value[i])
            i += 1
        self.data = v
    def value(self):
        return self.data

################################################################################

class xxxContainer(xxxObject):
    hasChildren = True
    exStyles = []

# Simulate normal parameter for encoding
class xxxEncoding:
    def value(self):
        return g.currentEncoding
    def update(self, val):
        g.currentEncoding = val

# Special class for root node
class xxxMainNode(xxxContainer):
    allParams = ['encoding']
    hasStyle = hasName = False
    def __init__(self, dom):
        xxxContainer.__init__(self, None, dom.documentElement)
        self.className = 'XML tree'
        # Reset required parameters after processing XML, because encoding is
        # a little special
        self.required = ['encoding']
        self.params['encoding'] = xxxEncoding()

################################################################################
# Top-level windwows

class xxxPanel(xxxContainer):
    allParams = ['pos', 'size', 'style']
    winStyles = ['wxNO_3D', 'wxTAB_TRAVERSAL']
    styles = ['fg', 'bg', 'font', 'enabled', 'focused', 'hidden', 'exstyle',
              'tooltip']

class xxxDialog(xxxContainer):
    allParams = ['title', 'centered', 'pos', 'size', 'style']
    paramDict = {'centered': ParamBool}
    required = ['title']
    default = {'title': ''}
    winStyles = ['wxDEFAULT_DIALOG_STYLE', 'wxCAPTION', 
                 'wxSTAY_ON_TOP', 'wxSYSTEM_MENU', 'wxTHICK_FRAME',
                 'wxRESIZE_BORDER', 'wxRESIZE_BOX', 'wxCLOSE_BOX',
                 'wxMAXIMIZE_BOX', 'wxMINIMIZE_BOX',
                 'wxDIALOG_MODAL', 'wxDIALOG_MODELESS', 'wxDIALOG_NO_PARENT'
                 'wxNO_3D', 'wxTAB_TRAVERSAL']                 
    exStyles = ['wxWS_EX_VALIDATE_RECURSIVELY', 'wxDIALOG_EX_METAL']
    styles = ['fg', 'bg', 'font', 'enabled', 'focused', 'hidden', 'exstyle',
              'tooltip']

class xxxFrame(xxxContainer):
    allParams = ['title', 'centered', 'pos', 'size', 'style']
    paramDict = {'centered': ParamBool}
    required = ['title']
    default = {'title': ''}
    winStyles = ['wxDEFAULT_FRAME_STYLE', 'wxDEFAULT_DIALOG_STYLE', 'wxCAPTION', 
                 'wxSTAY_ON_TOP', 'wxSYSTEM_MENU', 'wxTHICK_FRAME',
                 'wxRESIZE_BORDER', 'wxRESIZE_BOX', 'wxCLOSE_BOX',
                 'wxMAXIMIZE_BOX', 'wxMINIMIZE_BOX',
                 'wxFRAME_NO_TASKBAR', 'wxFRAME_SHAPED', 'wxFRAME_TOOL_WINDOW',
                 'wxFRAME_FLOAT_ON_PARENT',
                 'wxNO_3D', 'wxTAB_TRAVERSAL']                 
    exStyles = ['wxWS_EX_VALIDATE_RECURSIVELY', 'wxFRAME_EX_METAL']
    styles = ['fg', 'bg', 'font', 'enabled', 'focused', 'hidden', 'exstyle',
              'tooltip']

class xxxTool(xxxObject):
    allParams = ['bitmap', 'bitmap2', 'radio', 'toggle', 'tooltip', 'longhelp', 'label']
    required = ['bitmap']
    paramDict = {'bitmap2': ParamBitmap, 'radio': ParamBool, 'toggle': ParamBool}
    hasStyle = False

class xxxToolBar(xxxContainer):
    allParams = ['bitmapsize', 'margins', 'packing', 'separation', 'dontattachtoframe',
                 'pos', 'size', 'style']
    hasStyle = False
    paramDict = {'bitmapsize': ParamPosSize, 'margins': ParamPosSize,
                 'packing': ParamUnit, 'separation': ParamUnit,
                 'dontattachtoframe': ParamBool, 'style': ParamNonGenericStyle}
    winStyles = ['wxTB_FLAT', 'wxTB_DOCKABLE', 'wxTB_VERTICAL', 'wxTB_HORIZONTAL',
                 'wxTB_3DBUTTONS','wxTB_TEXT', 'wxTB_NOICONS', 'wxTB_NODIVIDER',
                 'wxTB_NOALIGN', 'wxTB_HORZ_LAYOUT', 'wxTB_HORZ_TEXT']

class xxxStatusBar(xxxObject):
    hasStyle = False
    allParams = ['fields', 'widths', 'styles', 'style']
    paramDict = {'fields': ParamIntNN, 'widths': ParamText, 'styles': ParamText,
                 'style': ParamNonGenericStyle}
    winStyles = ['wxST_SIZEGRIP']

class xxxWizard(xxxContainer):
    allParams = ['title', 'bitmap', 'pos']
    required = ['title']
    default = {'title': ''}
    winStyles = []
    exStyles = ['wxWIZARD_EX_HELPBUTTON']
    styles = ['fg', 'bg', 'font', 'exstyle']

class xxxWizardPage(xxxContainer):
    allParams = ['bitmap']
    winStyles = []
    exStyles = []

class xxxWizardPageSimple(xxxContainer):
    allParams = ['bitmap']
    winStyles = []
    exStyles = []

################################################################################
# Bitmap, Icon

class xxxBitmap(xxxObject):
    allParams = ['bitmap']
    required = ['bitmap']

# Just like bitmap
class xxxIcon(xxxObject):
    allParams = []

################################################################################
# Controls

class xxxStaticText(xxxObject):
    allParams = ['label', 'pos', 'size', 'style']
    required = ['label']
    default = {'label': ''}
    winStyles = ['wxALIGN_LEFT', 'wxALIGN_RIGHT', 'wxALIGN_CENTRE', 'wxST_NO_AUTORESIZE']

class xxxStaticLine(xxxObject):
    allParams = ['pos', 'size', 'style']
    winStyles = ['wxLI_HORIZONTAL', 'wxLI_VERTICAL']

class xxxStaticBitmap(xxxObject):
    allParams = ['bitmap', 'pos', 'size', 'style']
    required = ['bitmap']

class xxxTextCtrl(xxxObject):
    allParams = ['value', 'pos', 'size', 'style']
    winStyles = ['wxTE_NO_VSCROLL',
                 'wxTE_AUTO_SCROLL',
                 'wxTE_PROCESS_ENTER',
                 'wxTE_PROCESS_TAB',
                 'wxTE_MULTILINE',
                 'wxTE_PASSWORD',
                 'wxTE_READONLY',
                 'wxHSCROLL',
                 'wxTE_RICH',
                 'wxTE_RICH2',
                 'wxTE_AUTO_URL',
                 'wxTE_NOHIDESEL',
                 'wxTE_LEFT',
                 'wxTE_CENTRE',
                 'wxTE_RIGHT',
                 'wxTE_DONTWRAP',
                 'wxTE_LINEWRAP',
                 'wxTE_WORDWRAP']
    paramDict = {'value': ParamMultilineText}

class xxxChoice(xxxObject):
    allParams = ['content', 'selection', 'pos', 'size', 'style']
    required = ['content']
    default = {'content': '[]'}
    winStyles = ['wxCB_SORT']

class xxxSlider(xxxObject):
    allParams = ['value', 'min', 'max', 'pos', 'size', 'style',
                 'tickfreq', 'pagesize', 'linesize', 'thumb', 'tick',
                 'selmin', 'selmax']
    paramDict = {'value': ParamInt, 'tickfreq': ParamIntNN, 'pagesize': ParamIntNN,
                 'linesize': ParamIntNN, 'thumb': ParamUnit, 
                 'tick': ParamInt, 'selmin': ParamInt, 'selmax': ParamInt}
    required = ['value', 'min', 'max']
    winStyles = ['wxSL_HORIZONTAL', 'wxSL_VERTICAL', 'wxSL_AUTOTICKS', 'wxSL_LABELS',
                 'wxSL_LEFT', 'wxSL_RIGHT', 'wxSL_TOP', 'wxSL_BOTTOM',
                 'wxSL_BOTH', 'wxSL_SELRANGE', 'wxSL_INVERSE']

class xxxGauge(xxxObject):
    allParams = ['range', 'pos', 'size', 'style', 'value', 'shadow', 'bezel']
    paramDict = {'range': ParamIntNN, 'value': ParamIntNN,
                 'shadow': ParamIntNN, 'bezel': ParamIntNN}
    winStyles = ['wxGA_HORIZONTAL', 'wxGA_VERTICAL', 'wxGA_PROGRESSBAR', 'wxGA_SMOOTH']

class xxxScrollBar(xxxObject):
    allParams = ['pos', 'size', 'style', 'value', 'thumbsize', 'range', 'pagesize']
    paramDict = {'value': ParamIntNN, 'range': ParamIntNN, 'thumbsize': ParamIntNN,
                 'pagesize': ParamIntNN}
    winStyles = ['wxSB_HORIZONTAL', 'wxSB_VERTICAL']

class xxxListCtrl(xxxObject):
    allParams = ['pos', 'size', 'style']
    winStyles = ['wxLC_LIST', 'wxLC_REPORT', 'wxLC_ICON', 'wxLC_SMALL_ICON',
                 'wxLC_ALIGN_TOP', 'wxLC_ALIGN_LEFT', 'wxLC_AUTOARRANGE',
                 'wxLC_USER_TEXT', 'wxLC_EDIT_LABELS', 'wxLC_NO_HEADER',
                 'wxLC_SINGLE_SEL', 'wxLC_SORT_ASCENDING', 'wxLC_SORT_DESCENDING',
                 'wxLC_VIRTUAL', 'wxLC_HRULES', 'wxLC_VRULES', 'wxLC_NO_SORT_HEADER']

class xxxTreeCtrl(xxxObject):
    allParams = ['pos', 'size', 'style']
    winStyles = ['wxTR_EDIT_LABELS',
                 'wxTR_NO_BUTTONS',
                 'wxTR_HAS_BUTTONS',
                 'wxTR_TWIST_BUTTONS',
                 'wxTR_NO_LINES',
                 'wxTR_FULL_ROW_HIGHLIGHT',
                 'wxTR_LINES_AT_ROOT',
                 'wxTR_HIDE_ROOT',
                 'wxTR_ROW_LINES',
                 'wxTR_HAS_VARIABLE_ROW_HEIGHT',
                 'wxTR_SINGLE',
                 'wxTR_MULTIPLE',
                 'wxTR_EXTENDED',
                 'wxTR_DEFAULT_STYLE']

class xxxHtmlWindow(xxxObject):
    allParams = ['pos', 'size', 'style', 'borders', 'url', 'htmlcode']
    paramDict = {'htmlcode':ParamMultilineText}
    winStyles = ['wxHW_SCROLLBAR_NEVER', 'wxHW_SCROLLBAR_AUTO', 'wxHW_NO_SELECTION']

class xxxCalendarCtrl(xxxObject):
    allParams = ['pos', 'size', 'style']
    winStyles = ['wxCAL_SUNDAY_FIRST', 'wxCAL_MONDAY_FIRST', 'wxCAL_SHOW_HOLIDAYS',
                 'wxCAL_NO_YEAR_CHANGE', 'wxCAL_NO_MONTH_CHANGE',
                 'wxCAL_SEQUENTIAL_MONTH_SELECTION', 'wxCAL_SHOW_SURROUNDING_WEEKS']

class xxxNotebook(xxxContainer):
    allParams = ['pos', 'size', 'style']
    winStyles = ['wxNB_TOP', 'wxNB_LEFT', 'wxNB_RIGHT', 'wxNB_BOTTOM',
                 'wxNB_FIXEDWIDTH', 'wxNB_MULTILINE', 'wxNB_NOPAGETHEME', 'wxNB_FLAT']

class xxxChoicebook(xxxContainer):
    allParams = ['pos', 'size', 'style']
    winStyles = ['wxCHB_DEFAULT', 'wxCHB_LEFT', 'wxCHB_RIGHT', 'wxCHB_TOP', 'wxCHB_BOTTOM']
    
class xxxListbook(xxxContainer):
    allParams = ['pos', 'size', 'style']
    winStyles = ['wxLB_DEFAULT', 'wxLB_LEFT', 'wxLB_RIGHT', 'wxLB_TOP', 'wxLB_BOTTOM']
    
class xxxSplitterWindow(xxxContainer):
    allParams = ['orientation', 'sashpos', 'minsize', 'pos', 'size', 'style']
    paramDict = {'orientation': ParamOrientation, 'sashpos': ParamUnit, 'minsize': ParamUnit }
    winStyles = ['wxSP_3D', 'wxSP_3DSASH', 'wxSP_3DBORDER', 
                 'wxSP_FULLSASH', 'wxSP_NOBORDER', 'wxSP_PERMIT_UNSPLIT', 'wxSP_LIVE_UPDATE',
                 'wxSP_NO_XP_THEME' ]

class xxxGenericDirCtrl(xxxObject):
    allParams = ['defaultfolder', 'filter', 'defaultfilter', 'pos', 'size', 'style']
    paramDict = {'defaultfilter': ParamIntNN}
    winStyles = ['wxDIRCTRL_DIR_ONLY', 'wxDIRCTRL_3D_INTERNAL', 'wxDIRCTRL_SELECT_FIRST',
                 'wxDIRCTRL_SHOW_FILTERS']

class xxxScrolledWindow(xxxContainer):
    allParams = ['pos', 'size', 'style']
    winStyles = ['wxHSCROLL', 'wxVSCROLL', 'wxNO_3D', 'wxTAB_TRAVERSAL']

class xxxDateCtrl(xxxObject):
    allParams = ['pos', 'size', 'style', 'borders']
    winStyles = ['wxDP_DEFAULT', 'wxDP_SPIN', 'wxDP_DROPDOWN',
                 'wxDP_ALLOWNONE', 'wxDP_SHOWCENTURY']

class xxxGrid(xxxObject):
    allParams = ['pos', 'size', 'style']

class xxxFilePickerCtrl(xxxObject):
    allParams = ['value', 'message', 'wildcard', 'pos', 'size', 'style']
    winStyles = ['wxFLP_OPEN', 'wxFLP_SAVE', 'wxFLP_OVERWRITE_PROMPT',
                 'wxFLP_FILE_MUST_EXIST', 'wxFLP_CHANGE_DIR',
                 'wxFLP_DEFAULT_STYLE']


################################################################################
# Buttons

class xxxButton(xxxObject):
    allParams = ['label', 'default', 'pos', 'size', 'style']
    paramDict = {'default': ParamBool}
    required = ['label']
    winStyles = ['wxBU_LEFT', 'wxBU_TOP', 'wxBU_RIGHT', 'wxBU_BOTTOM', 'wxBU_EXACTFIT',
                 'wxNO_BORDER']

class xxxBitmapButton(xxxObject):
    allParams = ['bitmap', 'selected', 'focus', 'disabled', 'default',
                 'pos', 'size', 'style']
    paramDict = {'selected': ParamBitmap, 'focus': ParamBitmap, 'disabled': ParamBitmap,
                 'default': ParamBool}
    required = ['bitmap']
    winStyles = ['wxBU_AUTODRAW', 'wxBU_LEFT', 'wxBU_RIGHT',
                 'wxBU_TOP', 'wxBU_BOTTOM']

class xxxRadioButton(xxxObject):
    allParams = ['label', 'value', 'pos', 'size', 'style']
    paramDict = {'value': ParamBool}
    required = ['label']
    winStyles = ['wxRB_GROUP', 'wxRB_SINGLE']

class xxxSpinButton(xxxObject):
    allParams = ['value', 'min', 'max', 'pos', 'size', 'style']
    paramDict = {'value': ParamInt}
    winStyles = ['wxSP_HORIZONTAL', 'wxSP_VERTICAL', 'wxSP_ARROW_KEYS', 'wxSP_WRAP']

class xxxSpinCtrl(xxxObject):
    allParams = ['value', 'min', 'max', 'pos', 'size', 'style']
    paramDict = {'value': ParamInt}
    winStyles = ['wxSP_HORIZONTAL', 'wxSP_VERTICAL', 'wxSP_ARROW_KEYS', 'wxSP_WRAP']

class xxxToggleButton(xxxObject):
    allParams = ['label', 'checked', 'pos', 'size', 'style']
    paramDict = {'checked': ParamBool}
    required = ['label']

################################################################################
# Boxes

class xxxStaticBox(xxxObject):
    allParams = ['label', 'pos', 'size', 'style']
    required = ['label']

class xxxRadioBox(xxxObject):
    allParams = ['label', 'content', 'selection', 'dimension', 'pos', 'size', 'style']
    paramDict = {'dimension': ParamIntNN}
    required = ['label', 'content']
    default = {'content': '[]'}
    winStyles = ['wxRA_SPECIFY_ROWS', 'wxRA_SPECIFY_COLS', 'wxRA_HORIZONTAL',
                 'wxRA_VERTICAL']

class xxxCheckBox(xxxObject):
    allParams = ['label', 'checked', 'pos', 'size', 'style']
    paramDict = {'checked': ParamBool}
    winStyles = ['wxCHK_2STATE', 'wxCHK_3STATE', 'wxCHK_ALLOW_3RD_STATE_FOR_USER',
                 'wxALIGN_RIGHT']
    required = ['label']

class xxxComboBox(xxxObject):
    allParams = ['content', 'selection', 'value', 'pos', 'size', 'style']
    required = ['content']
    default = {'content': '[]'}
    winStyles = ['wxCB_SIMPLE', 'wxCB_SORT', 'wxCB_READONLY', 'wxCB_DROPDOWN']

class xxxListBox(xxxObject):
    allParams = ['content', 'selection', 'pos', 'size', 'style']
    required = ['content']
    default = {'content': '[]'}
    winStyles = ['wxLB_SINGLE', 'wxLB_MULTIPLE', 'wxLB_EXTENDED', 'wxLB_HSCROLL',
                 'wxLB_ALWAYS_SB', 'wxLB_NEEDED_SB', 'wxLB_SORT']

class xxxCheckList(xxxObject):
    allParams = ['content', 'pos', 'size', 'style']
    required = ['content']
    default = {'content': '[]'}
    winStyles = ['wxLB_SINGLE', 'wxLB_MULTIPLE', 'wxLB_EXTENDED', 'wxLB_HSCROLL',
                 'wxLB_ALWAYS_SB', 'wxLB_NEEDED_SB', 'wxLB_SORT']
    paramDict = {'content': ParamContentCheckList}

################################################################################
# Sizers

class xxxSizer(xxxContainer):
    hasName = hasStyle = False
    paramDict = {'orient': ParamOrient}
    isSizer = True
    itemTag = 'sizeritem'               # different for some sizers

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

class xxxGridSizer(xxxSizer):
    allParams = ['cols', 'rows', 'vgap', 'hgap']
    required = ['cols']
    default = {'cols': '2', 'rows': '2'}

class xxxStdDialogButtonSizer(xxxSizer):
    allParams = []
    itemTag = 'button'

# For repeated parameters
class xxxParamMulti:
    def __init__(self, node):
        self.node = node
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
    paramDict = {'growablecols': ParamIntList, 'growablerows': ParamIntList}

class xxxGridBagSizer(xxxSizer):
    specials = ['growablecols', 'growablerows']
    allParams = ['vgap', 'hgap'] + specials
    paramDict = {'growablecols': ParamIntList, 'growablerows': ParamIntList}

# Container with only one child.
# Not shown in tree.
class xxxChildContainer(xxxObject):
    hasName = hasStyle = False
    hasChild = True
    def __init__(self, parent, element, refElem=None):
        xxxObject.__init__(self, parent, element, refElem)
        # Must have one child with 'object' tag, but we don't check it
        nodes = element.childNodes[:]   # create copy
        for node in nodes:
            if node.nodeType == minidom.Node.ELEMENT_NODE:
                if node.tagName in ['object', 'object_ref']:
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
    def resetChild(self, xxx):
        '''Reset child info (for replacing with another class).'''
        self.child = xxx
        self.hasChildren = xxx.hasChildren
        self.isSizer = xxx.isSizer        

class xxxSizerItem(xxxChildContainer):
    allParams = ['option', 'flag', 'border', 'minsize', 'ratio']
    paramDict = {'option': ParamInt, 'minsize': ParamPosSize, 'ratio': ParamPosSize}
    defaults_panel = {}
    defaults_control = {}
    def __init__(self, parent, element, refElem=None):
        # For GridBag sizer items, extra parameters added
        if isinstance(parent, xxxGridBagSizer):
            self.allParams = self.allParams + ['cellpos', 'cellspan']
        xxxChildContainer.__init__(self, parent, element, refElem)
        # Remove pos parameter - not needed for sizeritems
        if 'pos' in self.child.allParams:
            self.child.allParams = self.child.allParams[:]
            self.child.allParams.remove('pos')
    def resetChild(self, xxx):
        xxxChildContainer.resetChild(self, xxx)
        # Remove pos parameter - not needed for sizeritems
        if 'pos' in self.child.allParams:
            self.child.allParams = self.child.allParams[:]
            self.child.allParams.remove('pos')        

class xxxSizerItemButton(xxxSizerItem):
    allParams = []
    paramDict = {}
    def __init__(self, parent, element, refElem=None):
        xxxChildContainer.__init__(self, parent, element, refElem=None)
        # Remove pos parameter - not needed for sizeritems
        if 'pos' in self.child.allParams:
            self.child.allParams = self.child.allParams[:]
            self.child.allParams.remove('pos')

class xxxPage(xxxChildContainer):
    allParams = ['label', 'selected']
    paramDict = {'selected': ParamBool}
    required = ['label']
    def __init__(self, parent, element, refElem=None):
        xxxChildContainer.__init__(self, parent, element, refElem)
        # pos and size dont matter for notebookpages
        if 'pos' in self.child.allParams:
            self.child.allParams = self.child.allParams[:]
            self.child.allParams.remove('pos')
        if 'size' in self.child.allParams:
            self.child.allParams = self.child.allParams[:]
            self.child.allParams.remove('size')

class xxxSpacer(xxxObject):
    hasName = hasStyle = False
    allParams = ['size', 'option', 'flag', 'border']
    paramDict = {'option': ParamInt}
    default = {'size': '0,0'}
    def __init__(self, parent, element, refElem=None):
        # For GridBag sizer items, extra parameters added
        if isinstance(parent, xxxGridBagSizer):
            self.allParams = self.allParams + ['cellpos', 'cellspan']
        xxxObject.__init__(self, parent, element, refElem)

class xxxMenuBar(xxxContainer):
    allParams = ['style']
    paramDict = {'style': ParamNonGenericStyle}    # no generic styles
    winStyles = ['wxMB_DOCKABLE']

class xxxMenu(xxxContainer):
    allParams = ['label', 'help', 'style']
    default = {'label': ''}
    paramDict = {'style': ParamNonGenericStyle}    # no generic styles
    winStyles = ['wxMENU_TEAROFF']

class xxxMenuItem(xxxObject):
    allParams = ['label', 'bitmap', 'accel', 'help',
                 'checkable', 'radio', 'enabled', 'checked']
    default = {'label': ''}
    hasStyle = False

class xxxSeparator(xxxObject):
    hasName = hasStyle = False

################################################################################
# Unknown control

class xxxUnknown(xxxObject):
    allParams = ['pos', 'size', 'style']
    winStyles = ['wxNO_FULL_REPAINT_ON_RESIZE']

################################################################################
# Comment

_handlers = []                          # custom handler classes/funcs
def getHandlers():
    return _handlers
def setHandlers(handlers):
    global _handlers
    _handlers = handlers
_CFuncPtr = None                        # ctypes function type

def register(hndlr):
    """Register hndlr function or XmlResourceHandler class."""
    if _CFuncPtr and isinstance(hndlr, _CFuncPtr):
        _handlers.append(hndlr)
        return
    if not isinstance(hndlr, type):
        wx.LogError('handler is not a type: %s' % hndlr)
    elif not issubclass(hndlr, wx.xrc.XmlResourceHandler):
        wx.LogError('handler is not a XmlResourceHandler: %s' % hndlr)
    else:
        _handlers.append(hndlr)

def load_dl(path, localname=''):
    """Load shared/dynamic library into xxx namespace.

    If path is not absolute or relative, try to find in the module's directory.
    """
    if not localname:
        localname = os.path.basename(os.path.splitext(path)[0])
    try:
        import ctypes
        global _CFuncPtr
        _CFuncPtr = ctypes._CFuncPtr    # use as a flag of loaded ctypes
        #if not os.path.dirname(path) and os.path.isfile(path):
            
    except ImportError:
        wx.LogError('ctypes module not found')
        globals()[localname] = None
        return
    try:
        dl = ctypes.CDLL(path)
        globals()[localname] = dl
        # Register AddXmlHandlers() if exists
        try:
            register(dl.AddXmlHandlers)
        except:
            pass
    except:
        wx.LogError('error loading dynamic library: %s' % path)
        print traceback.print_exc()

# Called when creating test window
def addHandlers():
    for h in _handlers:
        if _CFuncPtr and isinstance(h, _CFuncPtr):
            try:
                apply(h, ())
            except:
                wx.LogError('error calling DL func: "%s"' % h)
                print traceback.print_exc()
        else:
            try:
                xrc.XmlResource.Get().AddHandler(apply(h, ()))
            except:
                wx.LogError('error adding XmlHandler: "%s"' % h)
                print traceback.print_exc()

def custom(klassName, klass='unknown'):
    """Define custom control based on xrcClass.

    klass: new object name
    xrcClass: name of an existing XRC object class or
              a class object defining class parameters.
    """
    if type(klass) is str:
        # Copy correct xxx class under new name
        kl = xxxDict[klass]
        xxxClass = types.ClassType('xxx' + klassName, kl.__bases__, kl.__dict__)
    else:
        xxxClass = klass
        # Register param IDs
        for param in klass.allParams + klass.paramDict.keys():
            if not paramIDs.has_key(param):
                paramIDs[param] = wx.NewId()
    # Insert in dictionaty
    xxxDict[klassName] = xxxClass
    # Add to menu
    g.pullDownMenu.addCustom(klassName)

class xxxParamComment(xxxParam):
    locals = {}                         # namespace for comment directives
    allow = None                        # undefined initial state for current file
    def __init__(self, node):
        xxxNode.__init__(self, node)
        self.textNode = node
        # Parse "pragma" comments if enabled
        if node.data and node.data[0] == '%' and g.conf.allowExec != 'no' and \
               xxxParamComment.allow is not False:
            # Show warning
            if g.conf.allowExec == 'ask' and xxxParamComment.allow is None:
                flags = wx.ICON_EXCLAMATION | wx.YES_NO | wx.CENTRE
                dlg = wx.MessageDialog(g.frame, '''
This file contains executable %comment directives. Allow to execute?''',
                                       'Warning', flags)
                say = dlg.ShowModal()
                dlg.Destroy()
                if say == wx.ID_YES:
                    xxxParamComment.allow = True
                else:
                    xxxParamComment.allow = False
            try:
                code = node.data[1:]
                exec code in globals(), self.locals
            except:
                wx.LogError('exec error: "%s"' % code)
                print traceback.print_exc()

class xxxComment(xxxObject):
    hasStyle = hasName = False
    allParams = required = ['comment']
    
    def __init__(self, parent, node):
        self.parent = parent
        self.node = node
        self.isElement = False
        self.undo = None
        self.className = 'comment'
        self.ref = self.subclass = None
        self.params = {'comment': xxxParamComment(node)}
        
    def treeName(self):
        # Replace newlines by \n to avoid tree item resizing
        return self.params['comment'].value().replace('\n', r'\n')

################################################################################

# Mapping of XRC names to xxx classes
xxxDict = {
    'wxPanel': xxxPanel,
    'wxDialog': xxxDialog,
    'wxFrame': xxxFrame,
    'tool': xxxTool,
    'wxToolBar': xxxToolBar,
    'wxStatusBar': xxxStatusBar,
    'wxWizard': xxxWizard,
    'wxWizardPage': xxxWizardPage,
    'wxWizardPageSimple': xxxWizardPageSimple,

    'wxBitmap': xxxBitmap,
    'wxIcon': xxxIcon,

    'wxButton': xxxButton,
    'wxBitmapButton': xxxBitmapButton,
    'wxRadioButton': xxxRadioButton,
    'wxSpinButton': xxxSpinButton,
    'wxToggleButton' : xxxToggleButton,

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
    'wxCheckListBox': xxxCheckList,
    'notebookpage': xxxPage,
    'choicebookpage': xxxPage,
    'listbookpage': xxxPage,
    'wxNotebook': xxxNotebook,
    'wxChoicebook': xxxChoicebook,
    'wxListbook': xxxListbook,
    'wxSplitterWindow': xxxSplitterWindow,
    'wxHtmlWindow': xxxHtmlWindow,
    'wxCalendarCtrl': xxxCalendarCtrl,
    'wxGenericDirCtrl': xxxGenericDirCtrl,
    'wxSpinCtrl': xxxSpinCtrl,
    'wxScrolledWindow': xxxScrolledWindow,
    'wxGrid': xxxGrid,
    'wxFilePickerCtrl': xxxFilePickerCtrl,
    'wxDatePickerCtrl': xxxDateCtrl,

    'wxBoxSizer': xxxBoxSizer,
    'wxStaticBoxSizer': xxxStaticBoxSizer,
    'wxGridSizer': xxxGridSizer,
    'wxFlexGridSizer': xxxFlexGridSizer,
    'wxGridBagSizer': xxxGridBagSizer,
    'wxStdDialogButtonSizer': xxxStdDialogButtonSizer,
    'sizeritem': xxxSizerItem, 'button': xxxSizerItemButton,
    'spacer': xxxSpacer,

    'wxMenuBar': xxxMenuBar,
    'wxMenu': xxxMenu,
    'wxMenuItem': xxxMenuItem,
    'separator': xxxSeparator,

    'unknown': xxxUnknown,
    'comment': xxxComment,
    }

# Create IDs for all parameters of all classes
paramIDs = {'fg': wx.NewId(), 'bg': wx.NewId(), 'exstyle': wx.NewId(), 'font': wx.NewId(),
            'enabled': wx.NewId(), 'focused': wx.NewId(), 'hidden': wx.NewId(),
            'tooltip': wx.NewId(), 'encoding': wx.NewId(),
            'cellpos': wx.NewId(), 'cellspan': wx.NewId(),
            'text': wx.NewId()
            }
for cl in xxxDict.values():
    if cl.allParams:
        for param in cl.allParams + cl.paramDict.keys():
            if not paramIDs.has_key(param):
                paramIDs[param] = wx.NewId()

################################################################################
# Helper functions

# Test for object elements
def IsObject(node):
    return node.nodeType == minidom.Node.ELEMENT_NODE and \
           node.tagName in ['object', 'object_ref'] or \
           node.nodeType == minidom.Node.COMMENT_NODE

# Make XXX object from some DOM object, selecting correct class
def MakeXXXFromDOM(parent, node):
    if node.nodeType == minidom.Node.COMMENT_NODE:
        return xxxComment(parent, node)
    if node.tagName == 'object_ref':
        ref = node.getAttribute('ref')
        refElem = FindResource(ref)
        if refElem: cls = refElem.getAttribute('class')
        else: return xxxUnknown(parent, node)
    else:
        refElem = None
        cls = node.getAttribute('class')
    try:
        klass = xxxDict[cls]
    except KeyError:
        # If we encounter a weird class, use unknown template
        print 'WARNING: unsupported class:', node.getAttribute('class')
        klass = xxxUnknown
    return klass(parent, node, refElem)

# Make empty DOM element
def MakeEmptyDOM(className):
    elem = g.tree.dom.createElement('object')
    elem.setAttribute('class', className)
    # Set required and default parameters
    xxxClass = xxxDict[className]
    defaultNotRequired = filter(lambda x, l=xxxClass.required: x not in l,
                                xxxClass.default.keys())
    for param in xxxClass.required + defaultNotRequired:
        textElem = g.tree.dom.createElement(param)
        try:
            textNode = g.tree.dom.createTextNode(xxxClass.default[param])
        except KeyError:
            textNode = g.tree.dom.createTextNode('')
        textElem.appendChild(textNode)
        elem.appendChild(textElem)
    return elem

# Make empty XXX object
def MakeEmptyXXX(parent, className):
    # Make corresponding DOM object first
    elem = MakeEmptyDOM(className)
    # Special handling, e.g. if parent is a sizer, we should create
    # sizeritem object, except for spacers, etc.
    if parent:
        if parent.isSizer and className != 'spacer':
            sizerItemElem = MakeEmptyDOM(parent.itemTag)
            sizerItemElem.appendChild(elem)
            elem = sizerItemElem
        elif isinstance(parent, xxxNotebook):
            pageElem = MakeEmptyDOM('notebookpage')
            pageElem.appendChild(elem)
            elem = pageElem
        elif isinstance(parent, xxxChoicebook):
            pageElem = MakeEmptyDOM('choicebookpage')
            pageElem.appendChild(elem)
            elem = pageElem
        elif isinstance(parent, xxxListbook):
            pageElem = MakeEmptyDOM('listbookpage')
            pageElem.appendChild(elem)
            elem = pageElem
    # Now just make object
    xxx = MakeXXXFromDOM(parent, elem)
    # Special defaults for new panels and controls
    if isinstance(xxx, xxxSizerItem):
        if isinstance(xxx.child, xxxContainer) and not xxx.child.isSizer:
            for param,v in xxxSizerItem.defaults_panel.items():
                xxx.set(param, v)
        elif isinstance(xxx.child, xxxObject):
            for param,v in xxxSizerItem.defaults_control.items():
                xxx.set(param, v)
    return xxx            

# Make empty DOM element for reference
def MakeEmptyRefDOM(ref):
    elem = g.tree.dom.createElement('object_ref')
    elem.setAttribute('ref', ref)
    return elem

# Make empty XXX object
def MakeEmptyRefXXX(parent, ref):
    # Make corresponding DOM object first
    elem = MakeEmptyRefDOM(ref)
    # If parent is a sizer, we should create sizeritem object, except for spacers
    if parent:
        if parent.isSizer:
            sizerItemElem = MakeEmptyDOM(parent.itemTag)
            sizerItemElem.appendChild(elem)
            elem = sizerItemElem
        elif isinstance(parent, xxxNotebook):
            pageElem = MakeEmptyDOM('notebookpage')
            pageElem.appendChild(elem)
            elem = pageElem
        elif isinstance(parent, xxxChoicebook):
            pageElem = MakeEmptyDOM('choicebookpage')
            pageElem.appendChild(elem)
            elem = pageElem
        elif isinstance(parent, xxxListbook):
            pageElem = MakeEmptyDOM('listbookpage')
            pageElem.appendChild(elem)
            elem = pageElem
    # Now just make object
    xxx = MakeXXXFromDOM(parent, elem)
    # Label is not used for references
    xxx.allParams = xxx.allParams[:]
    #xxx.allParams.remove('label')
    return xxx

# Make empty comment node
def MakeEmptyCommentDOM():
    node = g.tree.dom.createComment('')
    return node

# Make empty xxxComment
def MakeEmptyCommentXXX(parent):
    node = MakeEmptyCommentDOM()
    # Now just make object
    xxx = MakeXXXFromDOM(parent, node)
    return xxx

