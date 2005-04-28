#----------------------------------------------------------------------------
# Name:         xmlmarshaller.py
# Purpose:
#
# Author:       John Spurling
#
# Created:      7/28/04
# CVS-ID:       $Id$
# Copyright:    (c) 2004-2005 ActiveGrid, Inc.
# License:      wxWindows License
#----------------------------------------------------------------------------
from activegrid import util
import inspect
from types import *
import xml.sax
import xml.sax.handler
import __builtin__
from xml.sax import saxutils

### ToDO remove maxOccurs "unbounded" resolves to -1 hacks after bug 177 is fixed

"""

More documentation later, but here are some special Python attributes
that McLane recognizes:

name: __xmlname__
type: string
description: the name of the xml element for the marshalled object

name: __xmlattributes__
type: tuple or list
description: the name(s) of the Python string attribute(s) to be
marshalled as xml attributes instead of nested xml elements. currently
these can only be strings since there's not a way to get the type
information back when unmarshalling.

name: __xmlexclude__
type: tuple or list
description: the name(s) of the python attribute(s) to skip when
marshalling.

name: __xmlrename__
type: dict
description: describes an alternate Python <-> XML name mapping.  
Normally the name mapping is the identity function.  __xmlrename__
overrides that.  The keys are the Python names, the values are their
associated XML names.

name: __xmlflattensequence__
type: dict, tuple, or list
description: the name(s) of the Python sequence attribute(s) whose
items are to be marshalled as a series of xml elements (with an
optional keyword argument that specifies the element name to use) as
opposed to containing them in a separate sequence element, e.g.:

myseq = (1, 2)
<!-- normal way of marshalling -->
<myseq>
  <item objtype='int'>1</item>
  <item objtype='int'>2</item>
</myseq>
<!-- with __xmlflattensequence__ set to {'myseq': 'squish'} -->
<squish objtype='int'>1</squish>
<squish objtype='int'>2</squish>

name: __xmlnamespaces__
type: dict
description: a dict of the namespaces that the object uses.  Each item
in the dict should consist of a prefix,url combination where the key is
the prefix and url is the value, e.g.:

__xmlnamespaces__ = { "xsd":"http://www.w3c.org/foo.xsd" }

name: __xmldefaultnamespace__
type: String
description: the prefix of a namespace defined in __xmlnamespaces__ that
should be used as the default namespace for the object.

name: __xmlattrnamespaces__
type: dict
description: a dict assigning the Python object's attributes to the namespaces
defined in __xmlnamespaces__.  Each item in the dict should consist of a
prefix,attributeList combination where the key is the prefix and the value is
a list of the Python attribute names.  e.g.:

__xmlattrnamespaces__ = { "ag":["firstName", "lastName", "addressLine1", "city"] }

name: __xmlattrgroups__
type: dict
description: a dict specifying groups of attributes to be wrapped in an enclosing tag.
The key is the name of the enclosing tag; the value is a list of attributes to include
within it. e.g.

__xmlattrgroups__ = {"name": ["firstName", "lastName"], "address": ["addressLine1", "city", "state", "zip"]}

"""

################################################################################
#
# module exceptions
#
################################################################################

class Error(Exception):
    """Base class for errors in this module."""
    pass

class UnhandledTypeException(Error):
    """Exception raised when attempting to marshal an unsupported
    type.
    """
    def __init__(self, typename):
        self.typename = typename
    def __str__(self):
        return "%s is not supported for marshalling." % str(self.typename)

class XMLAttributeIsNotStringType(Error):
    """Exception raised when an object's attribute is specified to be
    marshalled as an XML attribute of the enclosing object instead of
    a nested element.
    """
    def __init__(self, attrname, typename):
        self.attrname = attrname
        self.typename = typename
    def __str__(self):
        return """%s was set to be marshalled as an XML attribute
        instead of a nested element, but the object's type is %s, not
        string.""" % (self.attrname, self.typename)

################################################################################
#
# constants and such
#
################################################################################

XMLNS = 'xmlns'
XMLNS_PREFIX = XMLNS + ':'
XMLNS_PREFIX_LENGTH = len(XMLNS_PREFIX)

BASETYPE_ELEMENT_NAME = 'item'

# This list doesn't seem to be used.
#   Internal documentation or useless? You make the call!
MEMBERS_TO_SKIP = ('__module__', '__doc__', '__xmlname__', '__xmlattributes__',
                   '__xmlexclude__', '__xmlflattensequence__', '__xmlnamespaces__',
                   '__xmldefaultnamespace__', '__xmlattrnamespaces__',
                   '__xmlattrgroups__')

#WELL_KNOWN_OBJECTS = { #"xs:element"     : "activegrid.model.schema.XsdElement",
                       #"xs:complexType" : "activegrid.model.schema.XsdComplexType",
                       #"xs:sequence"    : "activegrid.model.schema.XsdSequence",
                       #"xs:element"     : "activegrid.model.schema.XsdElement",
                       #"xs:key"         : "activegrid.model.schema.XsdKey",
                       #"xs:field"       : "activegrid.model.schema.XsdKeyField",
                       #"xs:keyref"      : "activegrid.model.schema.XsdKeyRef",
                       #"xs:selector"    : "activegrid.model.schema.XsdKeySelector",              
                       #"xs:schema"      : "activegrid.model.schema.Schema",
                       #"ag:schemaOptions":"activegrid.model.schema.SchemaOptions",
                       #"ag:debug"       : "activegrid.model.processmodel.DebugOperation",
                       #"ag:body"        : "activegrid.model.processmodel.Body",         # alan (start)
                       #"ag:cssRule"     : "activegrid.model.processmodel.CssRule",
                       #"ag:datasource"  : "activegrid.data.dataservice.DataSource",
                       #"ag:deployment"  : "activegrid.server.deployment.Deployment",
                       #"ag:glue"        : "activegrid.model.processmodel.Glue",
                       #"ag:hr"          : "activegrid.model.processmodel.HorizontalRow",
                       #"ag:image"       : "activegrid.model.processmodel.Image",
                       #"ag:inputs"      : "activegrid.model.processmodel.Inputs",
                       #"ag:label"       : "activegrid.model.processmodel.Label",
                       #"ag:processmodel"    : "activegrid.model.processmodel.ProcessModel",
                       #"ag:processmodelref" : "activegrid.server.deployment.ProcessModelRef",
                       #"ag:query"       : "activegrid.model.processmodel.Query",
                       #"ag:schemaref"   : "activegrid.server.deployment.SchemaRef",
                       #"ag:set"         : "activegrid.model.processmodel.SetOperation",
                       #"ag:text"        : "activegrid.model.processmodel.Text",
                       #"ag:title"       : "activegrid.model.processmodel.Title",
                       #"ag:view"        : "activegrid.model.processmodel.View",
                       #"bpws:case"      : "activegrid.model.processmodel.BPELCase",
                       #"bpws:invoke"    : "activegrid.model.processmodel.BPELInvoke",
                       #"bpws:otherwise" : "activegrid.model.processmodel.BPELOtherwise",
                       #"bpws:process"   : "activegrid.model.processmodel.BPELProcess",
                       #"bpws:reply"     : "activegrid.model.processmodel.BPELReply",
                       #"bpws:switch"    : "activegrid.model.processmodel.BPELSwitch",
                       #"bpws:variable"  : "activegrid.model.processmodel.BPELVariable",
                       #"projectmodel"   : "activegrid.tool.ProjectEditor.ProjectModel",
                       #"wsdl:message"   : "activegrid.model.processmodel.WSDLMessage",
                       #"wsdl:part"      : "activegrid.model.processmodel.WSDLPart",
                       #"xforms:group"   : "activegrid.model.processmodel.XFormsGroup",
                       #"xforms:input"   : "activegrid.model.processmodel.XFormsInput",
                       #"xforms:label"   : "activegrid.model.processmodel.XFormsLabel",
                       #"xforms:output"  : "activegrid.model.processmodel.XFormsOutput",
                       #"xforms:secret"  : "activegrid.model.processmodel.XFormsSecret",
                       #"xforms:submit"  : "activegrid.model.processmodel.XFormsSubmit"}   # alan(end)


################################################################################
#
# classes and functions
#
################################################################################

def _objectfactory(objname, objargs=None, xsname=None):
    try:
        '''dynamically create an object based on the objname and return
        it. look it up in the BASETYPE_ELEMENT_MAP first.
        '''
        # split the objname into the typename and module path,
        # importing the module if need be.
        if not isinstance(objargs, list):
            objargs = [objargs]
            
        if (xsname):
            try:
                objname = knownGlobalTypes[xsname]
            except KeyError:
                pass
        
##        print "[objectfactory] creating an object of type %s and value %s, xsname=%s" % (objname, objargs, xsname)
        objtype = objname.split('.')[-1]
        pathlist = objname.split('.')
        modulename = '.'.join(pathlist[0:-1])

##        print "[objectfactory] objtype is %s" % objtype
##        print "[objectfactory] objargs is %s" % `objargs`

        ## since the bool constructor will turn a string of non-zero
        ## length into True, we call it with no argument (yielding a
        ## False) if the string contains 'false'
        if objtype == 'bool' and objargs[0].lower() == 'false':
            objargs = None

##        if objtype == 'str':
##            print type(objargs)
##            print "string we're unescaping: '%s'" % objargs[0]
##            objargs = saxutils.unescape(objargs[0])
        if objtype in ('float', 'int', 'str', 'long'):
            objargs = [x.strip() for x in objargs]

        if objtype == 'str':
            objargs = [saxutils.unescape(x) for x in objargs]

        if __builtin__.__dict__.has_key(objname):
            module = __builtin__
        else:
            if modulename:
                module = __import__(modulename)
            for name in pathlist[1:-1]:
                module = module.__dict__[name]
        if objargs:
            return module.__dict__[objtype](*objargs)
        else:
            if objtype == 'None':
                return None
            return module.__dict__[objtype]()
    except KeyError:
        raise KeyError("Could not find class %s" % objname)

class Element:
    def __init__(self, name, attrs=None):
        self.name = name
        self.attrs = attrs
        self.content = ''
        self.children = []
    def getobjtype(self):
        if self.attrs.has_key('objtype'):
            return self.attrs.getValue('objtype')
        else:
            return 'str'
    def toString(self):
        print "    name = ", self.name, "; attrs = ", self.attrs, "number of children = ", len(self.children)
        i = -1
        for child in self.children:
            i = i + 1
            childClass = child.__class__.__name__
            print "    Child ", i, " class: ",childClass


class XMLObjectFactory(xml.sax.ContentHandler):
    def __init__(self):
        self.rootelement = None
        self.elementstack = []
        xml.sax.handler.ContentHandler.__init__(self)

    def toString(self):
        print "-----XMLObjectFactory Dump-------------------------------"
        if (self.rootelement == None):
            print "rootelement is None"
        else:
            print "rootelement is an object"
        i = -1
        print "length of elementstack is: ", len(self.elementstack)
        for e in self.elementstack:
            i = i + 1
            print "elementstack[", i, "]: "
            e.toString()
        print "-----end XMLObjectFactory--------------------------------"
        
    ## ContentHandler methods
    def startElement(self, name, attrs):
##        print "startElement for name: ", name
        if name.find(':') > -1:  # Strip namespace prefixes for now until actually looking them up in xsd
            name = name[name.index(':') + 1:]
##        for attrname in attrs.getNames():
##            print "%s: %s" % (attrname, attrs.getValue(attrname))
        element = Element(name, attrs.copy())
        self.elementstack.append(element)
##        print self.elementstack

    def characters(self, content):
##        print "got content: %s" % content
        if content:
            self.elementstack[-1].content += content

    def endElement(self, name):
##        print "[endElement] name of element we're at the end of: %s" % name
        xsname = name
        if name.find(':') > -1:  # Strip namespace prefixes for now until actually looking them up in xsd
            name = name[name.index(':') + 1:]
        oldChildren = self.elementstack[-1].children
        element = self.elementstack.pop()
        if ((len(self.elementstack) > 1) and (self.elementstack[-1].getobjtype() == "None")):
            parentElement = self.elementstack[-2]
##            print "[endElement] %s: found parent with objtype==None: using its grandparent" % name
        elif (len(self.elementstack) > 0):
            parentElement = self.elementstack[-1]
        objtype = element.getobjtype()
##        print "element objtype is: ", objtype
        if (objtype == "None"):
##            print "[endElement] %s: skipping a (objtype==None) end tag" % name
            return
        constructorarglist = []
        if element.content:
            strippedElementContent = element.content.strip()
            if strippedElementContent:
                constructorarglist.append(element.content)
##        print "[endElement] calling objectfactory"
        obj = _objectfactory(objtype, constructorarglist, xsname)
        complexType = None
        if hasattr(obj, '__xsdcomplextype__'):
            complexType = getattr(obj, '__xsdcomplextype__')
            if (hasattr(obj, '__xmlname__') and getattr(obj, '__xmlname__') == "sequence"):
##                print "[endElement] sequence found"
##                self.toString()
                self.elementstack[-1].children = oldChildren
##                self.toString()
##                print "done moving sequence stuff; returning"
                return
        if len(self.elementstack) > 0:
##            print "[endElement] appending child with name: ", name, "; objtype: ", objtype
            parentElement.children.append((name, obj))
##            print "parentElement now has ", len(parentElement.children), " children"
        else:
            self.rootelement = obj
        if element.attrs and not isinstance(obj, list):
##            print "[endElement] %s: element has attrs and the obj is not a list" % name
            for attrname, attr in element.attrs.items():
                if attrname == XMLNS or attrname.startswith(XMLNS_PREFIX):
                    if attrname.startswith(XMLNS_PREFIX):
                        ns = attrname[XMLNS_PREFIX_LENGTH:]
                    else:
                        ns = ""
                    if not hasattr(obj, '__xmlnamespaces__'):
                        obj.__xmlnamespaces__ = {ns:attr}
                    elif ns not in obj.__xmlnamespaces__:
                        if (hasattr(obj.__class__, '__xmlnamespaces__') 
                           and obj.__xmlnamespaces__ is obj.__class__.__xmlnamespaces__):
                                obj.__xmlnamespaces__ = dict(obj.__xmlnamespaces__)
                        obj.__xmlnamespaces__[ns] = attr
                elif not attrname == 'objtype':
                    if attrname.find(':') > -1:  # Strip namespace prefixes for now until actually looking them up in xsd
                        attrname = attrname[attrname.index(':') + 1:]
                    if complexType:
                        xsdElement = complexType.findElement(attrname)
                        if xsdElement:
                            type = xsdElement.type
                            if type:
                                type = xsdToPythonType(type)
                                ### ToDO remove maxOccurs hack after bug 177 is fixed
                                if attrname == "maxOccurs" and attr == "unbounded":
                                    attr = "-1"
                                attr = _objectfactory(type, attr)
                    util.setattrignorecase(obj, _toAttrName(obj, attrname), attr)
##                    obj.__dict__[_toAttrName(obj, attrname)] = attr
        # stuff any child attributes meant to be in a sequence via the __xmlflattensequence__
        flattenDict = {}
        if hasattr(obj, '__xmlflattensequence__'):
##            print "[endElement] %s: obj has __xmlflattensequence__" % name
            if (isinstance(obj.__xmlflattensequence__,dict)):
##                print "[endElement]  dict with obj.__xmlflattensequence__.items: ", obj.__xmlflattensequence__.items()
                for sequencename, xmlnametuple in obj.__xmlflattensequence__.items():
                    for xmlname in xmlnametuple:
##                        print "[endElement]: adding flattenDict[%s] = %s" % (xmlname, sequencename)
                        flattenDict[xmlname] = sequencename
            # handle __xmlflattensequence__ list/tuple (i.e. no element rename)
            elif (isinstance(obj.__xmlflattensequence__,list) or isinstance(obj.__xmlflattensequence__,tuple)):
                for sequencename in obj.__xmlflattensequence__:
                    flattenDict[sequencename] = sequencename
            else:
                raise "Invalid type for __xmlflattensequence___ : it must be a dict, list, or tuple"

        # reattach an object's attributes to it
        for childname, child in element.children:
##            print "[endElement] childname is: ", childname, "; child is: ", child
            if flattenDict.has_key(childname):
                sequencename = _toAttrName(obj, flattenDict[childname])
##                print "[endElement] sequencename is: ", sequencename
                try:
##                    print "[endElement] obj.__dict__ is: ", obj.__dict__
                    sequencevalue = obj.__dict__[sequencename]
                except AttributeError:
                    sequencevalue = None
                except KeyError:
                    sequencevalue = None
                if sequencevalue == None:
                    sequencevalue = []
                    obj.__dict__[sequencename] = sequencevalue
                sequencevalue.append(child)
            elif isinstance(obj, list):
##                print "appended childname = ", childname
                obj.append(child)
            else:
##                print "childname = %s, obj = %s, child = %s" % (childname, repr(obj), repr(child))
                util.setattrignorecase(obj, _toAttrName(obj, childname), child)
                obj.__dict__[_toAttrName(obj, childname)] = child

        if complexType:
            for element in complexType.elements:
                if element.default:
                    elementName = _toAttrName(obj, element.name)
                    if ((elementName not in obj.__dict__) or (obj.__dict__[elementName] == None)):
                        pythonType = xsdToPythonType(element.type)
                        defaultValue = _objectfactory(pythonType, element.default)
                        obj.__dict__[elementName] = defaultValue

    def getRootObject(self):
        return self.rootelement

def _toAttrName(obj, name):
    if (hasattr(obj, "__xmlrename__")):
        for key, val in obj.__xmlrename__.iteritems():
            if (name == val):
                name = key
                break
##    if (name.startswith("__") and not name.endswith("__")):
##        name = "_%s%s" % (obj.__class__.__name__, name)
    return name

__typeMappingXsdToPython = {
    "string": "str",
    "char": "str",
    "varchar": "str",
    "date": "str", # ToDO Need to work out how to create python date types
    "boolean": "bool",
    "decimal": "float", # ToDO Does python have a better fixed point type?
    "int": "int",
    "long": "long",
    "float": "float",
    "bool": "bool",
    "str": "str",
    "unicode": "unicode",
    "short": "int",
    "duration": "str", # see above (date)
    "datetime": "str", # see above (date)
    "time": "str", # see above (date)
    "double": "float",
    }    

def xsdToPythonType(xsdType):
    try:
        return __typeMappingXsdToPython[xsdType]
    except KeyError:
        raise Exception("Unknown xsd type %s" % xsdType)

def _getXmlValue(pythonValue):
    if (isinstance(pythonValue, bool)):
        return str(pythonValue).lower()
    else:
        return str(pythonValue)

def unmarshal(xmlstr, knownTypes=None):
    global knownGlobalTypes
    if (knownTypes == None):
        knownGlobalTypes = {}
    else:
        knownGlobalTypes = knownTypes
    objectfactory = XMLObjectFactory()
    xml.sax.parseString(xmlstr, objectfactory)
    return objectfactory.getRootObject()


def marshal(obj, elementName=None, nameSpacePrefix='', nameSpaces=None, prettyPrint=False, indent=0, knownTypes=None):
    if prettyPrint or indent:
        prefix = ' '*indent
        newline = '\n'
        increment = 4
    else:
        prefix = ''
        newline = ''
        increment = 0

    ## Determine the XML element name. If it isn't specified in the
    ## parameter list, look for it in the __xmlname__ Python
    ## attribute, else use the default generic BASETYPE_ELEMENT_NAME.
    if not nameSpaces: nameSpaces = {}  # Need to do this since if the {} is a default parameter it gets shared by all calls into the function
    nameSpaceAttrs = ''
    if knownTypes == None:
        knownTypes = {}
    if hasattr(obj, '__xmlnamespaces__'):
        for nameSpaceKey, nameSpaceUrl in getattr(obj, '__xmlnamespaces__').items():
            if nameSpaceUrl in nameSpaces:
                nameSpaceKey = nameSpaces[nameSpaceUrl]
            else:
##                # TODO: Wait to do this until there is shared state for use when going through the object graph
##                origNameSpaceKey = nameSpaceKey  # Make sure there is no key collision, ie: same key referencing two different URL's
##                i = 1
##                while nameSpaceKey in nameSpaces.values():
##                    nameSpaceKey = origNameSpaceKey + str(i)
##                    i += 1
                nameSpaces[nameSpaceUrl] = nameSpaceKey
                if nameSpaceKey == '':
                    nameSpaceAttrs += ' xmlns="%s" ' % (nameSpaceUrl)
                else:
                    nameSpaceAttrs += ' xmlns:%s="%s" ' % (nameSpaceKey, nameSpaceUrl)
        nameSpaceAttrs = nameSpaceAttrs.rstrip()
    if hasattr(obj, '__xmldefaultnamespace__'):
        nameSpacePrefix = getattr(obj, '__xmldefaultnamespace__') + ':'        
    if not elementName:
        if hasattr(obj, '__xmlname__'):
            elementName = nameSpacePrefix + obj.__xmlname__
        else:
            elementName = nameSpacePrefix + BASETYPE_ELEMENT_NAME
    else:
        elementName = nameSpacePrefix + elementName
    if hasattr(obj, '__xmlsequencer__'):
        elementAdd = obj.__xmlsequencer__
    else:
        elementAdd = None
               
##    print "marshal: entered with elementName: ", elementName
    members_to_skip = []
    ## Add more members_to_skip based on ones the user has selected
    ## via the __xmlexclude__ attribute.
    if hasattr(obj, '__xmlexclude__'):
##        print "marshal: found __xmlexclude__"
        members_to_skip += list(obj.__xmlexclude__)
    # Marshal the attributes that are selected to be XML attributes.
    objattrs = ''
    className = obj.__class__.__name__
    classNamePrefix = "_" + className
    if hasattr(obj, '__xmlattributes__'):
##        print "marshal: found __xmlattributes__"
        xmlattributes = obj.__xmlattributes__
        members_to_skip += xmlattributes
        for attr in xmlattributes:
            internalAttrName = attr
            if (attr.startswith("__") and not attr.endswith("__")): 
                internalAttrName = classNamePrefix + attr
            # Fail silently if a python attribute is specified to be
            # an XML attribute but is missing.
##            print "marshal:   processing attribute ", internalAttrName
            try:
                value = obj.__dict__[internalAttrName]
            except KeyError:
                continue
##                # But, check and see if it is a property first:
##                if (hasPropertyValue(obj, attr)):
##                    value = getattr(obj, attr)
##                else:
##                    continue
            xsdElement = None
            if hasattr(obj, '__xsdcomplextype__'):
##                print "marshal: found __xsdcomplextype__"
                complexType = getattr(obj, '__xsdcomplextype__')
                xsdElement = complexType.findElement(attr)
            if xsdElement:
                default = xsdElement.default
                if default == value or default == _getXmlValue(value):
                    continue
            elif value == None:
                continue

            # ToDO remove maxOccurs hack after bug 177 is fixed
            if attr == "maxOccurs" and value == -1:
                value = "unbounded"

            if isinstance(value, bool):
               if value == True:
                   value = "true"
               else:
                   value = "false"

            attrNameSpacePrefix = ''
            if hasattr(obj, '__xmlattrnamespaces__'):
##                print "marshal: found __xmlattrnamespaces__"
                for nameSpaceKey, nameSpaceAttributes in getattr(obj, '__xmlattrnamespaces__').items():
                    if nameSpaceKey == nameSpacePrefix[:-1]: # Don't need to specify attribute namespace if it is the same as it selement
                        continue
                    if attr in nameSpaceAttributes:
                        attrNameSpacePrefix = nameSpaceKey + ':'
                        break
##            if attr.startswith('_'):
##                attr = attr[1:]
            if (hasattr(obj, "__xmlrename__") and attr in obj.__xmlrename__):
##                print "marshal: found __xmlrename__ (and its attribute)"
                attr = obj.__xmlrename__[attr]

            objattrs += ' %s%s="%s"' % (attrNameSpacePrefix, attr, value)
##            print "marshal:   new objattrs is: ", objattrs

    objtype = type(obj)
    if isinstance(obj, NoneType):
        #print "marshal:   skipping an element with no type"
        return ''
#        return '%s<%s objtype="None"/>%s' % (prefix, elementName, newline)
    elif isinstance(obj, bool):
        xmlString = '%s<%s objtype="bool">%s</%s>%s' % (prefix, elementName, obj, elementName, newline)
        #print "marshal:   returning a bool element: \n", xmlString
        return xmlString
    elif isinstance(obj, int):
        xmlString = '''%s<%s objtype="int">%s</%s>%s''' % (prefix, elementName, str(obj), elementName, newline)
        #print "marshal:   returning a int element: \n", xmlString
        return xmlString
    elif isinstance(obj, long):
        xmlString = '%s<%s objtype="long">%s</%s>%s' % (prefix, elementName, str(obj), elementName, newline)
        #print "marshal:   returning a long element: \n", xmlString
        return xmlString
    elif isinstance(obj, float):
        xmlString = '%s<%s objtype="float">%s</%s>%s' % (prefix, elementName, str(obj), elementName, newline)
        #print "marshal:   returning a float element: \n", xmlString
        return xmlString
    elif isinstance(obj, basestring):
        xmlString = '''%s<%s>%s</%s>%s''' % (prefix, elementName, saxutils.escape(obj), elementName, newline)
        #print "marshal:   returning a str element: \n", xmlString
        return xmlString
##    elif isinstance(obj, unicode):
##        return '''%s<%s>%s</%s>%s''' % (prefix, elementName, obj, elementName, newline)
    elif isinstance(obj, list):
        if len(obj) < 1:
            #print "marshal:   skipping an empty list"
            return ''
        xmlString = '%s<%s objtype="list">%s' % (prefix, elementName, newline)
        for item in obj:
            xmlString += marshal(item, nameSpaces=nameSpaces, indent=indent+increment, knownTypes=knownTypes)
        xmlString += '%s</%s>%s' % (prefix, elementName, newline)
        #print "marshal:    returning a list element: \n", xmlString
        return xmlString
    elif isinstance(obj, tuple):
        if len(obj) < 1:
            #print "marshal:   skipping an empty tuple"
            return ''
        xmlString = '%s<%s objtype="list" mutable="false">%s' % (prefix, elementName, newline)
        for item in obj:
            xmlString += marshal(item, nameSpaces=nameSpaces, indent=indent+increment, knownTypes=knownTypes)
        xmlString += '%s</%s>%s' % (prefix, elementName, newline)
        #print "marshal:    returning a tuple element: \n", xmlString
        return xmlString
    elif isinstance(obj, dict):
        xmlString = '%s<%s objtype="dict">%s' % (prefix, elementName, newline)
        subprefix = prefix + ' '*increment
        subindent = indent + 2*increment
        for key, val in obj.iteritems():
            xmlString += "%s<key>%s%s%s</key>%s%s<value>%s%s%s</value>%s" \
                         % (subprefix, newline, marshal(key, indent=subindent, knownTypes=knownTypes), subprefix, newline, subprefix, newline, marshal(val, nameSpaces=nameSpaces, indent=subindent, knownTypes=knownTypes), subprefix, newline)
        xmlString += '%s</%s>%s' % (prefix, elementName, newline)
        #print "marshal:    returning a dict element: \n", xmlString
        return xmlString
    else:
        moduleName = obj.__class__.__module__
        if (moduleName == "activegrid.model.schema"):
##            print "marshal:     found an activegrid.model.schema class element"
            xmlString = '%s<%s%s%s' % (prefix, elementName, nameSpaceAttrs, objattrs)
        else:
##            print "marshal:     found a ", moduleName, " class element"
            # Only add the objtype if the element tag is unknown to us.
            try:
                objname = knownTypes[elementName]
##                print "successfully mapped ", elementName, " to known-objtype ", objname
                xmlString = '%s<%s%s%s' % (prefix, elementName, nameSpaceAttrs, objattrs)
            except KeyError:
##                print "failed to map elementName: ", elementName, "; knownTypes: ", knownTypes
                xmlString = '%s<%s%s%s objtype="%s.%s"' % (prefix, elementName, nameSpaceAttrs, objattrs, moduleName, className)
##                print "UnknownTypeException: Unknown type (%s.%s) passed to marshaller" % (moduleName, className)
        # get the member, value pairs for the object, filtering out
        # the types we don't support.
##        print "marshal:    elementString: \n", xmlString
        if (elementAdd != None):
            prefix += increment*' '
            indent += increment
            
        xmlMemberString = ''
        if hasattr(obj, '__xmlbody__'):
            xmlMemberString = getattr(obj, obj.__xmlbody__)
        else:
            entryList = obj.__dict__.items()
##            # Add in properties
##            for key in obj.__class__.__dict__.iterkeys():
##                if (key not in members_to_skip and key not in obj.__dict__
##                    and hasPropertyValue(obj, key)):
##                    value = getattr(obj, key)
##                    entryList.append((key, value))
            entryList.sort()
            if hasattr(obj, '__xmlattrgroups__'):
                attrGroups = obj.__xmlattrgroups__
                if (not isinstance(attrGroups,dict)):
                    raise "__xmlattrgroups__ is not a dict, but must be"
                for n in attrGroups:
                    v = attrGroups[n]
                    members_to_skip += v
            else:
                attrGroups = {}
            # add the list of all attributes to attrGroups
            eList = []
            for x, z in entryList:
                eList.append(x)
            attrGroups['__nogroup__'] = eList
            
            for eName in attrGroups:
                eList = attrGroups[eName]
                if (eName != '__nogroup__'):
                    prefix += increment*' '
                    indent += increment
                    xmlMemberString += '%s<%s objtype="None">%s' % (prefix, eName, newline)
                for name in eList:
                    value = obj.__dict__[name]
##                    print " ", name, " = ", value
##                    # special name handling for private "__*" attributes:
##                    # remove the _<class-name> added by Python
##                    if name.startswith(classNamePrefix): name = name[len(classNamePrefix):]
                    if eName == '__nogroup__' and name in members_to_skip: continue
                    if name.startswith('__') and name.endswith('__'): continue
##                    idx = name.find('__')
##                    if idx > 0:
##                        newName = name[idx+2:]
##                        if newName:
##                            name = newName
##                    print "marshal:   processing subElement ", name
                    subElementNameSpacePrefix = nameSpacePrefix
                    if hasattr(obj, '__xmlattrnamespaces__'):
                        for nameSpaceKey, nameSpaceValues in getattr(obj, '__xmlattrnamespaces__').items():
                            if name in nameSpaceValues:
                                subElementNameSpacePrefix = nameSpaceKey + ':'
                                break
                    # handle sequences listed in __xmlflattensequence__
                    # specially: instead of listing the contained items inside
                    # of a separate list, as god intended, list them inside
                    # the object containing the sequence.
                    if hasattr(obj, '__xmlflattensequence__') and name in obj.__xmlflattensequence__ and value:
                        try:
                            xmlnametuple = obj.__xmlflattensequence__[name]
                            xmlname = None
                            if len(xmlnametuple) == 1:
                                xmlname = xmlnametuple[0]
                        except:
                            xmlname = name
##                            xmlname = name.lower()
                        for seqitem in value:
                            xmlMemberString += marshal(seqitem, xmlname, subElementNameSpacePrefix, nameSpaces=nameSpaces, indent=indent+increment, knownTypes=knownTypes)
                    else:
                        if (hasattr(obj, "__xmlrename__") and name in obj.__xmlrename__):
                            xmlname = obj.__xmlrename__[name]
                        else:
                            xmlname = name
##                            xmlname = name.lower()
##                            # skip
##                            if xmlname.startswith('_') and not xmlname.startswith('__'):
##                                xmlname = xmlname[1:]
##                        if (indent > 30):
##                            print "getting pretty deep, xmlname = ", xmlname
##                        print "marshal:   marshalling ", xmlname
                        xmlMemberString += marshal(value, xmlname, subElementNameSpacePrefix, nameSpaces=nameSpaces, indent=indent+increment, knownTypes=knownTypes)
##                        print "marshal:   back with new xmlMemberString: \n", xmlMemberString
                if (eName != '__nogroup__'):
##                    print "marshal: Completing attrGroup ", eName
                    xmlMemberString += '%s</%s>%s' % (prefix, eName, newline)
                    prefix = prefix[:-increment]
                    indent -= increment

        # if we have nested elements, add them here, otherwise close the element tag immediately.
        if xmlMemberString:
            xmlString += '>'
            if hasattr(obj, '__xmlbody__'):
                xmlString += xmlMemberString
                xmlString += '</%s>%s' % (elementName, newline)
            else:
                xmlString += newline
                if (elementAdd != None):
                    xmlString += '%s<%s>%s' % (prefix, elementAdd, newline)
                xmlString += xmlMemberString
                if (elementAdd != None):
                    xmlString += '%s</%s>%s' % (prefix, elementAdd, newline)
                    prefix = prefix[:-increment]
                    indent -= increment
                xmlString += '%s</%s>%s' % (prefix, elementName, newline)
        else:
            xmlString = xmlString + '/>%s' % newline
        return xmlString

# We don't use this anymore but in case we want to get properties this is how
# you do it
def hasPropertyValue(obj, attr):
    hasProp = False
    try:
        prop = obj.__class__.__dict__[attr]
        if (isinstance(prop, property)):
            hasProp = hasattr(obj, attr)
            if (hasProp):
                # It's a property and it has a value but sometimes we don't want it.
                # If there is a _hasattr method execute it and the
                # result will tell us whether to include this value
                try:
                    hasProp = obj._hasattr(attr)
                except:
                    pass
    except KeyError:
        pass
    return hasProp

if __name__ == '__main__':
    from xmlmarshallertests import Person, marshalledint, marshalledlist

    l = [1, 2, 3]
    d = {'1': 1, '2': 2}
    outerlist = [l]
    xmlstr = marshal(d, "d", prettyPrint=True)
    print xmlstr

    person = Person()
    person.firstName = "Albert"
    person.lastName = "Camus"
    person.addressLine1 = "23 Absurd St."
    person.city = "Ennui"
    person.state = "MO"
    person.zip = "54321"
    person._phoneNumber = "808-303-2323"
    person.favoriteWords = ['angst', 'ennui', 'existence']
    person.weight = 150

    xmlstring = marshal(person, 'person', prettyPrint=True)
    print xmlstring

    obj = unmarshal(marshalledlist)
    print "obj has type %s and value %s" % (type(obj), str(obj))
    for item in obj:
        print "item: %s" % str(item)
