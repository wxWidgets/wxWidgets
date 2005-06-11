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
import __builtin__
import sys
from types import *
import logging
import xml.sax
import xml.sax.handler
import xml.sax.saxutils as saxutils
from activegrid.util.lang import *
import activegrid.util.aglogging as aglogging

MODULE_PATH = "__main__"

## ToDO remove maxOccurs "unbounded" resolves to -1 hacks after bug 177 is fixed

"""
Special attributes that we recognize:

name: __xmlname__
type: string
description: the name of the xml element for the marshalled object

name: __xmlattributes__
type: tuple or list
description: the name(s) of the Lang string attribute(s) to be
marshalled as xml attributes instead of nested xml elements. currently
these can only be strings since there"s not a way to get the type
information back when unmarshalling.

name: __xmlexclude__
type: tuple or list
description: the name(s) of the lang attribute(s) to skip when
marshalling.

name: __xmlrename__
type: dict
description: describes an alternate Lang <-> XML name mapping.  
Normally the name mapping is the identity function.  __xmlrename__
overrides that.  The keys are the Lang names, the values are their
associated XML names.

name: __xmlflattensequence__
type: dict, tuple, or list
description: the name(s) of the Lang sequence attribute(s) whose
items are to be marshalled as a series of xml elements (with an
optional keyword argument that specifies the element name to use) as
opposed to containing them in a separate sequence element, e.g.:

myseq = (1, 2)
<!-- normal way of marshalling -->
<myseq>
  <item objtype="int">1</item>
  <item objtype="int">2</item>
</myseq>
<!-- with __xmlflattensequence__ set to {"myseq": "squish"} -->
<squish objtype="int">1</squish>
<squish objtype="int">2</squish>

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
description: a dict assigning the Lang object"s attributes to the namespaces
defined in __xmlnamespaces__.  Each item in the dict should consist of a
prefix,attributeList combination where the key is the prefix and the value is
a list of the Lang attribute names.  e.g.:

__xmlattrnamespaces__ = { "ag":["firstName", "lastName", "addressLine1", "city"] }

name: __xmlattrgroups__
type: dict
description: a dict specifying groups of attributes to be wrapped in an enclosing tag.
The key is the name of the enclosing tag; the value is a list of attributes to include
within it. e.g.

__xmlattrgroups__ = {"name": ["firstName", "lastName"], "address": ["addressLine1", "city", "state", "zip"]}

"""

global xmlMarshallerLogger
xmlMarshallerLogger = logging.getLogger("activegrid.util.xmlmarshaller.marshal")
xmlMarshallerLogger.setLevel(aglogging.LEVEL_WARN)
# INFO  : low-level info
# DEBUG : debugging info

global knownGlobalTypes

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
    """Exception raised when an object"s attribute is specified to be
    marshalled as an XML attribute of the enclosing object instead of
    a nested element.
    """
    def __init__(self, attrname, typename):
        self.attrname = attrname
        self.typename = typename
    def __str__(self):
        return """%s was set to be marshalled as an XML attribute
        instead of a nested element, but the object"s type is %s, not
        string.""" % (self.attrname, self.typename)

class MarshallerException(Exception):
    pass 

################################################################################
#
# constants and such
#
################################################################################

XMLNS = "xmlns"
XMLNS_PREFIX = XMLNS + ":"
XMLNS_PREFIX_LENGTH = len(XMLNS_PREFIX)

BASETYPE_ELEMENT_NAME = "item"
DICT_ITEM_NAME = "qqDictItem"
DICT_ITEM_KEY_NAME = "key"
DICT_ITEM_VALUE_NAME = "value"

# This list doesn"t seem to be used.
#   Internal documentation or useless? You make the call!
##MEMBERS_TO_SKIP = ("__module__", "__doc__", "__xmlname__", "__xmlattributes__",
##                   "__xmlexclude__", "__xmlflattensequence__", "__xmlnamespaces__",
##                   "__xmldefaultnamespace__", "__xmlattrnamespaces__",
##                   "__xmlattrgroups__")

################################################################################
#
# classes and functions
#
################################################################################

def setattrignorecase(object, name, value):
    if (name not in object.__dict__):
        namelow = name.lower()
        for attr in object.__dict__:
            if attr.lower() == namelow:
                object.__dict__[attr] = value
                return
    object.__dict__[name] = value

def getComplexType(obj):
    if (hasattr(obj, "__xsdcomplextype__")):
        return obj.__xsdcomplextype__
    return None

def _objectfactory(objname, objargs=None, xsname=None):
    "dynamically create an object based on the objname and return it."

    if not isinstance(objargs, list):
        objargs = [objargs]

##    print "[objectfactory] xsname [%s]; objname [%s]" % (xsname, objname)

    # (a) deal with tagName:knownTypes mappings
    if (xsname != None):
        objclass = knownGlobalTypes.get(xsname)
        if (objclass != None):
            if (objargs != None):
                return objclass(*objargs)
            else:
                return objclass()                

    # (b) next with intrinisic types
    if objname == "str" or objname == "unicode": # don"t strip: blanks are significant
        if len(objargs) > 0:
            return saxutils.unescape(objargs[0]).encode()
        else:
            return ""
    elif objname == "bool":
        return not objargs[0].lower() == "false"
    elif objname in ("float", "int", "long"):
##        objargs = [x.strip() for x in objargs]
        return __builtin__.__dict__[objname](*objargs)
    elif objname == "None":
        return None

    # (c) objtype=path...module.class   
    # split the objname into the typename and module path,
    # importing the module if need be.
##    print "[objectfactory] creating an object of type %s and value %s, xsname=%s" % (objname, objargs, xsname)
    objtype = objname.split(".")[-1]
    pathlist = objname.split(".")
    modulename = ".".join(pathlist[0:-1])
##    print "[objectfactory] object [%s] %s(%r)" % (objname, objtype, objargs)

    try:
        if modulename:
            module = __import__(modulename)
            for name in pathlist[1:-1]:
                module = module.__dict__[name]
        elif __builtin__.__dict__.has_key(objname):
            module = __builtin__
        else:
            raise MarshallerException("Could not find class %s" % objname)
        if objargs:
            return module.__dict__[objtype](*objargs)
        else:
            return module.__dict__[objtype]()
    except KeyError:
        raise MarshallerException("Could not find class %s" % objname)

class Element:
    
    def __init__(self, name, attrs=None):
        self.name = name
        self.attrs = attrs
        self.content = ""
        self.children = []
        
    def getobjtype(self):
        objtype = self.attrs.get("objtype")
        if (objtype == None):
            if (len(self.children) > 0):
                objtype = "dict"
            else:
                objtype = "str"
        return objtype
            
    def __str__(self):
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

    def __str__(self):
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
            str(e)
        print "-----end XMLObjectFactory--------------------------------"
        
    ## ContentHandler methods
    def startElement(self, name, attrs):
##        print "startElement for name: ", name
        if name.find(":") > -1:  # Strip namespace prefixes for now until actually looking them up in xsd
            name = name[name.find(":") + 1:]
##        for attrname in attrs.getNames():
##            print "%s: %s" % (attrname, attrs.getValue(attrname))
        element = Element(name, attrs.copy())
        self.elementstack.append(element)
##        print self.elementstack

    def characters(self, content):
##        print "got content: %s (%s)" % (content, type(content))
        if (content != None):
            self.elementstack[-1].content += content

    def endElement(self, name):
##        print "[endElement] name of element we"re at the end of: %s" % name
        xsname = name
        if name.find(":") > -1:  # Strip namespace prefixes for now until actually looking them up in xsd
            name = name[name.find(":") + 1:]
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
        if (len(element.content) > 0):
            strippedElementContent = element.content.strip()
            if (len(strippedElementContent) > 0):
                constructorarglist.append(element.content)
##        print "[endElement] calling objectfactory"
        obj = _objectfactory(objtype, constructorarglist, xsname)
        complexType = getComplexType(obj)
        if (obj != None):
            if (hasattr(obj, "__xmlname__") and getattr(obj, "__xmlname__") == "sequence"):
                self.elementstack[-1].children = oldChildren
                return
        if (len(element.attrs) > 0) and not isinstance(obj, list):
##            print "[endElement] %s: element has attrs and the obj is not a list" % name
            for attrname, attr in element.attrs.items():
                if attrname == XMLNS or attrname.startswith(XMLNS_PREFIX):
                    if attrname.startswith(XMLNS_PREFIX):
                        ns = attrname[XMLNS_PREFIX_LENGTH:]
                    else:
                        ns = ""
                    if not hasattr(obj, "__xmlnamespaces__"):
                        obj.__xmlnamespaces__ = {ns:attr}
                    elif ns not in obj.__xmlnamespaces__:
                        if (hasattr(obj.__class__, "__xmlnamespaces__") 
                            and (obj.__xmlnamespaces__ is obj.__class__.__xmlnamespaces__)):
                            obj.__xmlnamespaces__ = dict(obj.__xmlnamespaces__)
                        obj.__xmlnamespaces__[ns] = attr
                elif not attrname == "objtype":
                    if attrname.find(":") > -1:  # Strip namespace prefixes for now until actually looking them up in xsd
                        attrname = attrname[attrname.find(":") + 1:]
                    if (complexType != None):
                        xsdElement = complexType.findElement(attrname)
                        if (xsdElement != None):
                            type = xsdElement.type
                            if (type != None):
                                type = xsdToLangType(type)
                                ### ToDO remove maxOccurs hack after bug 177 is fixed
                                if attrname == "maxOccurs" and attr == "unbounded":
                                    attr = "-1"
                                attr = _objectfactory(type, attr)
                    try:
                        setattrignorecase(obj, _toAttrName(obj, attrname), attr)
                    except AttributeError:
                        raise MarshallerException("Error unmarshalling attribute \"%s\" of XML element \"%s\": object type not specified or known" % (attrname, name))
##                    obj.__dict__[_toAttrName(obj, attrname)] = attr
        # stuff any child attributes meant to be in a sequence via the __xmlflattensequence__
        flattenDict = {}
        if hasattr(obj, "__xmlflattensequence__"):
            flatten = obj.__xmlflattensequence__
##            print "[endElement] %s: obj has __xmlflattensequence__" % name
            if (isinstance(flatten, dict)):
##                print "[endElement]  dict with flatten.items: ", flatten.items()
                for sequencename, xmlnametuple in flatten.items():
                    if (xmlnametuple == None):
                        flattenDict[sequencename] = sequencename
                    elif (not isinstance(xmlnametuple, (tuple, list))):
                        flattenDict[str(xmlnametuple)] = sequencename
                    else:
                        for xmlname in xmlnametuple:
    ##                        print "[endElement]: adding flattenDict[%s] = %s" % (xmlname, sequencename)
                            flattenDict[xmlname] = sequencename
            else:
                raise "Invalid type for __xmlflattensequence___ : it must be a dict"

        # reattach an object"s attributes to it
        for childname, child in element.children:
##            print "[endElement] childname is: ", childname, "; child is: ", child
            if (childname in flattenDict):
                sequencename = _toAttrName(obj, flattenDict[childname])
##                print "[endElement] sequencename is: ", sequencename
                if (not hasattr(obj, sequencename)):
##                    print "[endElement] obj.__dict__ is: ", obj.__dict__
                    obj.__dict__[sequencename] = []
                sequencevalue = getattr(obj, sequencename)
                if (sequencevalue == None):
                    obj.__dict__[sequencename] = []
                    sequencevalue = getattr(obj, sequencename)
                sequencevalue.append(child)
            elif (objtype == "list"):
                obj.append(child)
            elif isinstance(obj, dict):
                if (childname == DICT_ITEM_NAME):
                    obj[child[DICT_ITEM_KEY_NAME]] = child[DICT_ITEM_VALUE_NAME]
                else:
                    obj[childname] = child
            else:
##                print "childname = %s, obj = %s, child = %s" % (childname, repr(obj), repr(child))
                try:
                    setattrignorecase(obj, _toAttrName(obj, childname), child)
                except AttributeError:
                    raise MarshallerException("Error unmarshalling child element \"%s\" of XML element \"%s\": object type not specified or known" % (childname, name))
##                obj.__dict__[_toAttrName(obj, childname)] = child

        if (complexType != None):
            for element in complexType.elements:
                if element.default:
                    elementName = _toAttrName(obj, element.name)
                    if ((elementName not in obj.__dict__) or (obj.__dict__[elementName] == None)):
                        langType = xsdToLangType(element.type)
                        defaultValue = _objectfactory(langType, element.default)
                        obj.__dict__[elementName] = defaultValue

        ifDefPy()
        if (isinstance(obj, list)):
            if ((element.attrs.has_key("mutable")) and (element.attrs.getValue("mutable") == "false")):
                obj = tuple(obj)
        endIfDef()
            
        if (len(self.elementstack) > 0):
##            print "[endElement] appending child with name: ", name, "; objtype: ", objtype
            parentElement.children.append((name, obj))
##            print "parentElement now has ", len(parentElement.children), " children"
        else:
            self.rootelement = obj
            
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

__typeMappingXsdToLang = {
    "string": "str",
    "char": "str",
    "varchar": "str",
    "date": "str", # ToDO Need to work out how to create lang date types
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

def xsdToLangType(xsdType):
    langType = __typeMappingXsdToLang.get(xsdType)
    if (langType == None):
        raise Exception("Unknown xsd type %s" % xsdType)
    return langType

def _getXmlValue(langValue):
    if (isinstance(langValue, bool)):
        return str(langValue).lower()
    elif (isinstance(langValue, unicode)):
        return langValue.encode()
    else:
        return str(langValue)

def unmarshal(xmlstr, knownTypes=None):
    global knownGlobalTypes
    if (knownTypes == None):
        knownGlobalTypes = {}
    else:
        knownGlobalTypes = knownTypes
    objectfactory = XMLObjectFactory()
    xml.sax.parseString(xmlstr, objectfactory)
    return objectfactory.getRootObject()


def marshal(obj, elementName=None, prettyPrint=False, indent=0, knownTypes=None, encoding=-1):
    xmlstr = "".join(_marshal(obj, elementName, prettyPrint=prettyPrint, indent=indent, knownTypes=knownTypes))
    if (isinstance(encoding, basestring)):
        return '<?xml version="1.0" encoding="%s"?>\n%s' % (encoding, xmlstr.encode(encoding))
    elif (encoding == None):
        return xmlstr
    else:
        return '<?xml version="1.0" encoding="%s"?>\n%s' % (sys.getdefaultencoding(), xmlstr)

def _marshal(obj, elementName=None, nameSpacePrefix="", nameSpaces=None, prettyPrint=False, indent=0, knownTypes=None):
    xmlMarshallerLogger.debug("--> _marshal: elementName=%s, type=%s, obj=%s" % (elementName, type(obj), str(obj)))
    xmlString = None
    if prettyPrint or indent:
        prefix = " "*indent
        newline = "\n"
        increment = 4
    else:
        prefix = ""
        newline = ""
        increment = 0

    ## Determine the XML element name. If it isn"t specified in the
    ## parameter list, look for it in the __xmlname__ Lang
    ## attribute, else use the default generic BASETYPE_ELEMENT_NAME.
    if not nameSpaces: nameSpaces = {}  # Need to do this since if the {} is a default parameter it gets shared by all calls into the function
    nameSpaceAttrs = ""
    if knownTypes == None:
        knownTypes = {}
    if hasattr(obj, "__xmlnamespaces__"):
        for nameSpaceKey, nameSpaceUrl in getattr(obj, "__xmlnamespaces__").items():
            if nameSpaceUrl in asDict(nameSpaces):
                nameSpaceKey = nameSpaces[nameSpaceUrl]
            else:
##                # TODO: Wait to do this until there is shared for use when going through the object graph
##                origNameSpaceKey = nameSpaceKey  # Make sure there is no key collision, ie: same key referencing two different URL"s
##                i = 1
##                while nameSpaceKey in nameSpaces.values():
##                    nameSpaceKey = origNameSpaceKey + str(i)
##                    i += 1
                nameSpaces[nameSpaceUrl] = nameSpaceKey
                if nameSpaceKey == "":
                    nameSpaceAttrs += ' xmlns="%s" ' % (nameSpaceUrl)
                else:
                    nameSpaceAttrs += ' xmlns:%s="%s" ' % (nameSpaceKey, nameSpaceUrl)
        nameSpaceAttrs = nameSpaceAttrs.rstrip()
    if hasattr(obj, "__xmldefaultnamespace__"):
        nameSpacePrefix = getattr(obj, "__xmldefaultnamespace__") + ":"        
    if not elementName:
        if hasattr(obj, "__xmlname__"):
            elementName = nameSpacePrefix + obj.__xmlname__
        else:
            elementName = nameSpacePrefix + BASETYPE_ELEMENT_NAME
    else:
        elementName = nameSpacePrefix + elementName
    if hasattr(obj, "__xmlsequencer__"):
        elementAdd = obj.__xmlsequencer__
    else:
        elementAdd = None
               
##    print "marshal: entered with elementName: ", elementName
    members_to_skip = []
    ## Add more members_to_skip based on ones the user has selected
    ## via the __xmlexclude__ attribute.
    if hasattr(obj, "__xmlexclude__"):
##        print "marshal: found __xmlexclude__"
        members_to_skip.extend(obj.__xmlexclude__)
    # Marshal the attributes that are selected to be XML attributes.
    objattrs = ""
    className = ag_className(obj)
    classNamePrefix = "_" + className
    if hasattr(obj, "__xmlattributes__"):
##        print "marshal: found __xmlattributes__"
        xmlattributes = obj.__xmlattributes__
        members_to_skip.extend(xmlattributes)
        for attr in xmlattributes:
            internalAttrName = attr
            ifDefPy()
            if (attr.startswith("__") and not attr.endswith("__")): 
                internalAttrName = classNamePrefix + attr
            endIfDef()
            # Fail silently if a python attribute is specified to be
            # an XML attribute but is missing.
##            print "marshal:   processing attribute ", internalAttrName
            attrs = obj.__dict__
            value = attrs.get(internalAttrName)
            xsdElement = None
            complexType = getComplexType(obj)
            if (complexType != None):
##                print "marshal: found __xsdcomplextype__"
                xsdElement = complexType.findElement(attr)
            if (xsdElement != None):
                default = xsdElement.default
                if (default != None):
                    if ((default == value) or (default == _getXmlValue(value))):
                        continue
                else:
                    if (value == None):
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

            attrNameSpacePrefix = ""
            if hasattr(obj, "__xmlattrnamespaces__"):
##                print "marshal: found __xmlattrnamespaces__"
                for nameSpaceKey, nameSpaceAttributes in getattr(obj, "__xmlattrnamespaces__").iteritems():
                    if nameSpaceKey == nameSpacePrefix[:-1]: # Don't need to specify attribute namespace if it is the same as its element
                        continue
                    if attr in nameSpaceAttributes:
                        attrNameSpacePrefix = nameSpaceKey + ":"
                        break
##            if attr.startswith("_"):
##                attr = attr[1:]
            if (hasattr(obj, "__xmlrename__") and attr in asDict(obj.__xmlrename__)):
##                print "marshal: found __xmlrename__ (and its attribute)"
                attr = obj.__xmlrename__[attr]

            objattrs += ' %s%s="%s"' % (attrNameSpacePrefix, attr, str(value))
##            print "marshal:   new objattrs is: ", objattrs

    if (obj == None):
        xmlString = [""]
    elif isinstance(obj, bool):
        xmlString = ['%s<%s objtype="bool">%s</%s>%s' % (prefix, elementName, obj, elementName, newline)]
    elif isinstance(obj, int):
        xmlString = ['%s<%s objtype="int">%s</%s>%s' % (prefix, elementName, str(obj), elementName, newline)]
    elif isinstance(obj, long):
        xmlString = ['%s<%s objtype="long">%s</%s>%s' % (prefix, elementName, str(obj), elementName, newline)]
    elif isinstance(obj, float):
        xmlString = ['%s<%s objtype="float">%s</%s>%s' % (prefix, elementName, str(obj), elementName, newline)]
    elif isinstance(obj, unicode): # have to check before basestring - unicode is instance of base string
        xmlString = ['%s<%s>%s</%s>%s' % (prefix, elementName, saxutils.escape(obj.encode()), elementName, newline)]
    elif isinstance(obj, basestring):
        xmlString = ['%s<%s>%s</%s>%s' % (prefix, elementName, saxutils.escape(obj), elementName, newline)]
    elif isinstance(obj, list):
        if len(obj) < 1:
            xmlString = ""
        else:
            xmlString = ['%s<%s objtype="list">%s' % (prefix, elementName, newline)]
            for item in obj:
                xmlString.extend(_marshal(item, nameSpaces=nameSpaces, indent=indent+increment, knownTypes=knownTypes))
            xmlString.append("%s</%s>%s" % (prefix, elementName, newline))
    elif isinstance(obj, tuple):
        if len(obj) < 1:
            xmlString = ""
        else:
            xmlString = ['%s<%s objtype="list" mutable="false">%s' % (prefix, elementName, newline)]
            for item in obj:
                xmlString.extend(_marshal(item, nameSpaces=nameSpaces, indent=indent+increment, knownTypes=knownTypes))
            xmlString.append("%s</%s>%s" % (prefix, elementName, newline))
    elif isinstance(obj, dict):
        xmlString = ['%s<%s objtype="dict">%s' % (prefix, elementName, newline)]
        subprefix = prefix + " "*increment
        subindent = indent + 2*increment
        for key, val in obj.iteritems():
##            if (isinstance(key, basestring) and key is legal identifier):
##                xmlString.extend(_marshal(val, elementName=key, nameSpaces=nameSpaces, indent=subindent, knownTypes=knownTypes))
##            else:
            xmlString.append("%s<%s>%s" % (subprefix, DICT_ITEM_NAME, newline))
            xmlString.extend(_marshal(key, elementName=DICT_ITEM_KEY_NAME, indent=subindent, knownTypes=knownTypes))
            xmlString.extend(_marshal(val, elementName=DICT_ITEM_VALUE_NAME, nameSpaces=nameSpaces, indent=subindent, knownTypes=knownTypes))
            xmlString.append("%s</%s>%s" % (subprefix, DICT_ITEM_NAME, newline))
        xmlString.append("%s</%s>%s" % (prefix, elementName, newline))
    else:
        # Only add the objtype if the element tag is unknown to us.
        objname = knownTypes.get(elementName)
        if (objname != None):
            xmlString = ["%s<%s%s%s" % (prefix, elementName, nameSpaceAttrs, objattrs)]
        else:
            xmlString = ['%s<%s%s%s objtype="%s.%s"' % (prefix, elementName, nameSpaceAttrs, objattrs, obj.__class__.__module__, className)]
        # get the member, value pairs for the object, filtering out the types we don"t support
        if (elementAdd != None):
            prefix += increment*" "
            indent += increment
            
        xmlMemberString = []
        if hasattr(obj, "__xmlbody__"):
            xmlbody = getattr(obj, obj.__xmlbody__)
            if xmlbody != None:
                xmlMemberString.append(xmlbody)           
        else:
            if hasattr(obj, "__xmlattrgroups__"):
                attrGroups = obj.__xmlattrgroups__.copy()
                if (not isinstance(attrGroups, dict)):
                    raise "__xmlattrgroups__ is not a dict, but must be"
                for n in attrGroups.iterkeys():
                    members_to_skip.extend(attrGroups[n])
            else:
                attrGroups = {}
            # add the list of all attributes to attrGroups
            eList = obj.__dict__.keys()
            eList.sort()
            attrGroups["__nogroup__"] = eList
            
            for eName, eList in attrGroups.iteritems():
                if (eName != "__nogroup__"):
                    prefix += increment*" "
                    indent += increment
                    xmlMemberString.append('%s<%s objtype="None">%s' % (prefix, eName, newline))
                for name in eList:
                    value = obj.__dict__[name]
                    if eName == "__nogroup__" and name in members_to_skip: continue
                    if name.startswith("__") and name.endswith("__"): continue
                    subElementNameSpacePrefix = nameSpacePrefix
                    if hasattr(obj, "__xmlattrnamespaces__"):
                        for nameSpaceKey, nameSpaceValues in getattr(obj, "__xmlattrnamespaces__").iteritems():
                            if name in nameSpaceValues:
                                subElementNameSpacePrefix = nameSpaceKey + ":"
                                break
                    # handle sequences listed in __xmlflattensequence__
                    # specially: instead of listing the contained items inside
                    # of a separate list, as God intended, list them inside
                    # the object containing the sequence.
                    if (hasattr(obj, "__xmlflattensequence__") and (value != None) and (name in asDict(obj.__xmlflattensequence__))):
                        xmlnametuple = obj.__xmlflattensequence__[name]
                        if (xmlnametuple == None):
                            xmlnametuple = [name]
                        elif (not isinstance(xmlnametuple, (tuple,list))):
                            xmlnametuple = [str(xmlnametuple)]
                        xmlname = None
                        if (len(xmlnametuple) == 1):
                            xmlname = xmlnametuple[0]
##                        ix = 0
                        for seqitem in value:
##                            xmlname = xmlnametuple[ix]
##                            ix += 1
##                            if (ix >= len(xmlnametuple)):
##                                ix = 0
                            xmlMemberString.extend(_marshal(seqitem, xmlname, subElementNameSpacePrefix, nameSpaces=nameSpaces, indent=indent+increment, knownTypes=knownTypes))
                    else:
                        if (hasattr(obj, "__xmlrename__") and name in asDict(obj.__xmlrename__)):
                            xmlname = obj.__xmlrename__[name]
                        else:
                            xmlname = name
                        xmlMemberString.extend(_marshal(value, xmlname, subElementNameSpacePrefix, nameSpaces=nameSpaces, indent=indent+increment, knownTypes=knownTypes))
                if (eName != "__nogroup__"):
                    xmlMemberString.append("%s</%s>%s" % (prefix, eName, newline))
                    prefix = prefix[:-increment]
                    indent -= increment

        # if we have nested elements, add them here, otherwise close the element tag immediately.
        newList = []
        for s in xmlMemberString:
            if (len(s) > 0): newList.append(s)
        xmlMemberString = newList
        if len(xmlMemberString) > 0:
            xmlString.append(">")
            if hasattr(obj, "__xmlbody__"):
                xmlString.extend(xmlMemberString)
                xmlString.append("</%s>%s" % (elementName, newline))
            else:
                xmlString.append(newline)
                if (elementAdd != None):
                    xmlString.append("%s<%s>%s" % (prefix, elementAdd, newline))
                xmlString.extend(xmlMemberString)
                if (elementAdd != None):
                    xmlString.append("%s</%s>%s" % (prefix, elementAdd, newline))
                    prefix = prefix[:-increment]
                    indent -= increment
                xmlString.append("%s</%s>%s" % (prefix, elementName, newline))
        else:
            xmlString.append("/>%s" % newline)
##        return xmlString
    xmlMarshallerLogger.debug("<-- _marshal: %s" % str(xmlString))
    return xmlString

# A simple test, to be executed when the xmlmarshaller is run standalone
class MarshallerPerson:
    __xmlname__ = "person"
    __xmlexclude__ = ["fabulousness",]
    __xmlattributes__ = ("nonSmoker",)
    __xmlrename__ = {"_phoneNumber": "telephone"}
    __xmlflattensequence__ = {"favoriteWords": ("vocabulary",)}
    __xmlattrgroups__ = {"name": ["firstName", "lastName"], "address": ["addressLine1", "city", "state", "zip"]}

    def setPerson(self):
        self.firstName = "Albert"
        self.lastName = "Camus"
        self.addressLine1 = "23 Absurd St."
        self.city = "Ennui"
        self.state = "MO"
        self.zip = "54321"
        self._phoneNumber = "808-303-2323"
        self.favoriteWords = ["angst", "ennui", "existence"]
        self.phobias = ["war", "tuberculosis", "cars"]
        self.weight = 150
        self.fabulousness = "tres tres"
        self.nonSmoker = False

if isMain(__name__):
    p1 = MarshallerPerson()
    p1.setPerson() 
    xmlP1 = marshal(p1, prettyPrint=True, encoding="utf-8")        
    print "\n########################"
    print   "# testPerson test case #"
    print   "########################"
    print xmlP1
    p2 = unmarshal(xmlP1)
    xmlP2 = marshal(p2, prettyPrint=True, encoding="utf-8")
    if xmlP1 == xmlP2:
        print "Success: repeated marshalling yields identical results"
    else:
        print "Failure: repeated marshalling yields different results"
        print xmlP2
