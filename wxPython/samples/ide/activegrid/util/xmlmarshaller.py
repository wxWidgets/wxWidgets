#----------------------------------------------------------------------------
# Name:         xmlmarshaller.py
# Purpose:
#
# Authors:       John Spurling, Joel Hare, Alan Mullendore
#
# Created:      7/28/04
# CVS-ID:       $Id$
# Copyright:    (c) 2004-2005 ActiveGrid, Inc.
# License:      wxWindows License
#----------------------------------------------------------------------------
import sys
from types import *
from activegrid.util.lang import *
import logging
ifDefPy()
import xml.sax
import xml.sax.handler
endIfDef()
import xml.sax.saxutils as saxutils
import activegrid.util.objutils as objutils
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

name: __xmlcdatacontent__
type: string
description: value is the name of a string attribute that should be assigned CDATA content from the
source document and that should be marshalled as CDATA.

__xmlcdatacontent__ = "messyContent"

"""

global xmlMarshallerLogger
xmlMarshallerLogger = logging.getLogger("activegrid.util.xmlmarshaller.marshal")
xmlMarshallerLogger.setLevel(aglogging.LEVEL_WARN)
# INFO  : low-level info
# DEBUG : debugging info

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

class UnmarshallerException(Exception):
    pass 

################################################################################
#
# constants and such
#
################################################################################

XMLNS = "xmlns"
XMLNS_PREFIX = XMLNS + ":"
XMLNS_PREFIX_LENGTH = len(XMLNS_PREFIX)
DEFAULT_NAMESPACE_KEY = "__DEFAULTNS__"
TYPE_QNAME = "QName"
XMLSCHEMA_XSD_URL = "http://www.w3.org/2001/XMLSchema"
AG_URL = "http://www.activegrid.com/ag.xsd"

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

def _objectfactory(objname, objargs=None, objclass=None):
    "dynamically create an object based on the objname and return it."
##    print "[objectfactory] objname [%s]" % (objname)
    if not isinstance(objargs, list):
        objargs = [objargs]
    if (objclass != None):
        if (len(objargs) > 0):
            if (hasattr(objclass, "__xmlcdatacontent__")):
                obj = objclass()
                contentAttr = obj.__xmlcdatacontent__
                obj.__dict__[contentAttr] = str(objargs[0])
                return obj
            return objclass(*objargs)
        else:
            return objclass()
    return objutils.newInstance(objname, objargs)

class Element:
    def __init__(self, name, attrs=None, xsname=None):
        self.name = name
        self.attrs = attrs
        self.content = ""
        self.children = []
        self.objclass = None
        self.xsname = xsname
        
    def getobjtype(self):
        objtype = self.attrs.get("objtype")
        if (objtype == None):
            if (len(self.children) > 0):
                objtype = "dict"
            else:
                objtype = "str"
        return objtype
            
class NsElement(object):
    def __init__(self):
        self.nsMap = {}
        self.targetNS = None
        self.defaultNS = None
        self.prefix = None
        
    def isEmpty(self):
        return ((self.nsMap == {}) and (self.targetNS == None) and (self.defaultNS == None))

    def setKnownTypes(self, masterKnownTypes, masterKnownNamespaces, parentNSE):
        # if we're a nested element, extend our parent element's mapping
        if parentNSE != None:
            self.knownTypes = parentNSE.knownTypes.copy()
            # but if we have a different default namespace, replace the parent's default mappings
            if parentNSE.defaultNS != self.defaultNS:
                newKT = self.knownTypes.copy()
                for tag in newKT:
                    if tag.find(':') < 0:
                        del self.knownTypes[tag]
            newMap = parentNSE.nsMap.copy()
            if self.nsMap != {}:
                for k, v in self.nsMap.iteritems():
                    newMap[k] = v
            self.nsMap = newMap
        else:
            self.knownTypes = {}
        reversedKNS = {}
        # TODO: instead of starting with the knownNamespaces, start with the "xmlms" mappings
        # for this element. Then we'd only process the namespaces and tags we need to.
        # But for now, this works.
        for long, short in masterKnownNamespaces.iteritems():
            reversedKNS[short] = long
        mapLongs = self.nsMap.values()
        for tag, mapClass in masterKnownTypes.iteritems():
            i = tag.rfind(':')
            if i >= 0:                                      # e.g. "wsdl:description"
                knownTagShort = tag[:i]                     # "wsdl"
                knownTagName = tag[i+1:]                    # "description"
                knownTagLong  = reversedKNS[knownTagShort]  # e.g. "http://schemas.xmlsoap.org/wsdl"
                if (knownTagLong in mapLongs):
                    for mShort, mLong in self.nsMap.iteritems():
                        if mLong == knownTagLong:
                            actualShort = mShort  # e.g. "ws"
                            actualTag = '%s:%s' % (actualShort, knownTagName)
                            self.knownTypes[actualTag] = mapClass
                            break
                if self.defaultNS == knownTagLong:
                    self.knownTypes[knownTagName] = mapClass
            else:                                           # e.g. "ItemSearchRequest"
                self.knownTypes[tag] = mapClass
##                print 'mapping <%s> to class "%s"' % (tag, mapClass.__name__)

    def expandQName(self, eName, attrName, attrValue):
        bigValue = attrValue
        i = attrValue.rfind(':')
        if (i < 0):
            if self.defaultNS != None:
                bigValue = '%s:%s' % (self.defaultNS, attrValue)
        else:
            attrNS = attrValue[:i]
            attrNCName = attrValue[i+1:]
            for shortNs, longNs in self.nsMap.iteritems():
                if shortNs == attrNS:
                    bigValue = '%s:%s' % (longNs, attrNCName)
                    break
##        print '[expandQName] input attrName = "%s" and attrValue "%s"; output = "%s"' % (attrName, attrValue, bigValue)
        return bigValue

class XMLObjectFactory(xml.sax.ContentHandler):
    def __init__(self, knownTypes=None, knownNamespaces=None):
        self.rootelement = None
        if (knownTypes == None):
            self.knownTypes = {}
        else:
            self.knownTypes = knownTypes
        if (knownNamespaces == None):
            self.knownNamespaces = {}
        else:
            self.knownNamespaces = knownNamespaces
        self.skipper = False
        self.elementstack = []
        self.nsstack = []
        self.collectContent = None
        xml.sax.handler.ContentHandler.__init__(self)

    def appendElementStack(self, newElement, newNS):
        self.elementstack.append(newElement)
        if (newNS.isEmpty()):
            if (len(self.nsstack) > 0):
                newNS = self.nsstack[-1]
            else:
                newNS.knownTypes = self.knownTypes.copy()
        else:
            if (len(self.nsstack) > 0):
                newNS.setKnownTypes(self.knownTypes, self.knownNamespaces, self.nsstack[-1])
            else:
                newNS.setKnownTypes(self.knownTypes, self.knownNamespaces, None)
        self.nsstack.append(newNS)
        return newNS
    
    def popElementStack(self):
        element = self.elementstack.pop()
        nse = self.nsstack.pop()
        return element, nse
        
    ## ContentHandler methods
    def startElement(self, name, attrs):
##        print '[startElement] <%s>' % (name)
        if name == 'xs:annotation' or name == 'xsd:annotation': # should use namespace mapping here
            self.skipper = True
            self.appendElementStack(Element(name, attrs.copy()), NsElement())
        if self.skipper:
            return
        if self.collectContent != None:
            strVal = '<%s' % (name)
            for aKey, aVal in attrs.items():
                strVal += (' %s="%s"' % (aKey, aVal))
            strVal += '>'
            self.collectContent.content += strVal
        xsname = name
        if name.find(':') > -1:  # Strip namespace prefixes for now until actually looking them up in xsd
            name = name[name.rfind(":") + 1:]
        element = Element(name, attrs.copy(), xsname=xsname)
        # if the element has namespace attributes, process them and add them to our stack
        nse = NsElement()
        for k in attrs.getNames():
            if k.startswith('xmlns'):
                longNs = attrs[k]
                eLongNs = longNs + '/'
                if str(eLongNs) in asDict(self.knownNamespaces):
                    longNs = eLongNs
                if k == 'xmlns':
                    nse.defaultNS = longNs
                else:
                    shortNs = k[6:]
                    nse.nsMap[shortNs] = longNs
            elif k == 'targetNamespace':
                nse.targetNS = attrs.getValue(k)
        nse = self.appendElementStack(element, nse)
        element.objclass = nse.knownTypes.get(xsname)
        if (hasattr(element.objclass, "__xmlcontent__")):
            self.collectContent = element

    def characters(self, content):
##        print '[characters] "%s" (%s)' % (content, type(content))
        if (content != None):
            if self.collectContent != None:
                self.collectContent.content += content
            else:
                self.elementstack[-1].content += content

    def endElement(self, name):
##        print "[endElement] </%s>" % name
        xsname = name
        if name.find(":") > -1:  # Strip namespace prefixes for now until actually looking them up in xsd
            name = name[name.find(":") + 1:]
        if self.skipper:
            if xsname == "xs:annotation" or xsname == "xsd:annotation":     # here too
                self.skipper = False
                self.popElementStack()
            return
        if self.collectContent != None:
            if xsname != self.collectContent.xsname:
                self.collectContent.content += ('</%s>' % (xsname))
                self.popElementStack()
                return
            else:
                self.collectContent = None
        oldChildren = self.elementstack[-1].children
        element, nse = self.popElementStack()
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
        obj = _objectfactory(objtype, constructorarglist, element.objclass)
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
                    if complexType != None:
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
##                            print 'Unmarshalling element "%s", attribute "%s" with type "%s"' % (name, xsdElement.name, type)
                            if (type != None):
                                if (type == TYPE_QNAME):
                                    attr = nse.expandQName(name, attrname, attr)
                                type = xsdToLangType(type)
                                ### ToDO remove maxOccurs hack after bug 177 is fixed
                                if attrname == "maxOccurs" and attr == "unbounded":
                                    attr = "-1"
                                attr = _objectfactory(type, attr)
                    try:
                        setattrignorecase(obj, _toAttrName(obj, attrname), attr)
                    except AttributeError:
                        errorString = 'Error unmarshalling XML document at line %i, column %i: The object type of attribute "%s" of XML element "%s": not specified or known' % (self._locator.getLineNumber(), self._locator.getColumnNumber(), attrname, name)
                        raise UnmarshallerException(errorString)
##                    obj.__dict__[_toAttrName(obj, attrname)] = attr
        # stuff any child attributes meant to be in a sequence via the __xmlflattensequence__
        flattenDict = {}
        if hasattr(obj, "__xmlflattensequence__"):
            flatten = obj.__xmlflattensequence__
            if (isinstance(flatten, dict)):
                for sequencename, xmlnametuple in flatten.items():
                    if (xmlnametuple == None):
                        flattenDict[sequencename] = sequencename
                    elif (not isinstance(xmlnametuple, (tuple, list))):
                        flattenDict[str(xmlnametuple)] = sequencename
                    else:
                        for xmlname in xmlnametuple:
##                            print "[endElement]: adding flattenDict[%s] = %s" % (xmlname, sequencename)
                            flattenDict[xmlname] = sequencename
            else:
                raise Exception("Invalid type for __xmlflattensequence___ : it must be a dict")

        # reattach an object"s attributes to it
        for childname, child in element.children:
##            print "[endElement] childname is: ", childname, "; child is: ", child
            if (childname in flattenDict):
                sequencename = _toAttrName(obj, flattenDict[childname])
                if (not hasattr(obj, sequencename)):
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
    "integer":"int",
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
    "QName" : "str",
    "blob" : "str",         # ag:blob
    "currency" : "str",     # ag:currency
    }    

def xsdToLangType(xsdType):
    if xsdType.startswith(XMLSCHEMA_XSD_URL):
        xsdType = xsdType[len(XMLSCHEMA_XSD_URL)+1:]
    elif xsdType.startswith(AG_URL):
        xsdType = xsdType[len(AG_URL)+1:]        
    langType = __typeMappingXsdToLang.get(xsdType)
    if (langType == None):
        raise Exception("Unknown xsd type %s" % xsdType)
    return langType
    
def langToXsdType(langType):
    if langType in asDict(__typeMappingXsdToLang):
        return '%s:%s' % (XMLSCHEMA_XSD_URL, langType)
    return langType

def _getXmlValue(langValue):
    if (isinstance(langValue, bool)):
        return str(langValue).lower()
    elif (isinstance(langValue, unicode)):
        return langValue.encode()
    else:
        return str(langValue)

def unmarshal(xmlstr, knownTypes=None, knownNamespaces=None, xmlSource=None):
    objectfactory = XMLObjectFactory(knownTypes, knownNamespaces)
    try:
        xml.sax.parseString(xmlstr, objectfactory)
    except xml.sax.SAXParseException, errorData:
        if xmlSource == None:
            xmlSource = 'unknown'
        errorString = 'SAXParseException ("%s") detected at line %d, column %d in XML document from source "%s" ' % (errorData.getMessage(), errorData.getLineNumber(), errorData.getColumnNumber(), xmlSource)
        raise UnmarshallerException(errorString)
    return objectfactory.getRootObject()

def marshal(obj, elementName=None, prettyPrint=False, marshalType=True, indent=0, knownTypes=None, knownNamespaces=None, encoding=-1):
##    print '[marshal] entered with elementName = "%s"' % (elementName)
    worker = XMLMarshalWorker(prettyPrint=prettyPrint, marshalType=marshalType, knownTypes=knownTypes, knownNamespaces=knownNamespaces)    
    if obj != None and hasattr(obj, '__xmldeepexclude__'):
        worker.xmldeepexclude = obj.__xmldeepexclude__
    xmlstr = "".join(worker._marshal(obj, elementName, indent=indent))
    if (isinstance(encoding, basestring)):
        return '<?xml version="1.0" encoding="%s"?>\n%s' % (encoding, xmlstr.encode(encoding))
    elif (encoding == None):
        return xmlstr
    else:
        return '<?xml version="1.0" encoding="%s"?>\n%s' % (sys.getdefaultencoding(), xmlstr)

class XMLMarshalWorker(object):
    def __init__(self, marshalType=True, prettyPrint=False, knownTypes=None, knownNamespaces=None):
        if knownTypes == None:
            self.knownTypes = {}
        else:
            self.knownTypes = knownTypes
        if knownNamespaces == None:
            self.knownNamespaces = {}
        else:
            self.knownNamespaces = knownNamespaces
        self.prettyPrint = prettyPrint
        self.marshalType = marshalType
        self.xmldeepexclude = []
        self.nsstack = []

    def getNSPrefix(self):
        if len(self.nsstack) > 0:
            return self.nsstack[-1].prefix
        return ''

    def isKnownType(self, elementName):
        tagLongNs = None
        nse = self.nsstack[-1]
        i = elementName.rfind(':')
        if i > 0:
            prefix = elementName[:i]
            name = elementName[i+1:]
        else:
            prefix = DEFAULT_NAMESPACE_KEY
            name = elementName
        for shortNs, longNs in nse.nameSpaces.iteritems():
            if shortNs == prefix:
                tagLongNs = longNs
                break
        if tagLongNs == None:
            knownTagName = elementName
        else:
            knownShortNs = self.knownNamespaces[tagLongNs]
            knownTagName = knownShortNs + ':' + name
        if (knownTagName in asDict(self.knownTypes)):
            knownClass = self.knownTypes[knownTagName]
            return True
        return False
                      
    def popNSStack(self):
        self.nsstack.pop()

    def appendNSStack(self, obj):
        nameSpaces = {}
        defaultLongNS = None
        for nse in self.nsstack:
            for k, v in nse.nsMap.iteritems():
                nameSpaces[k] = v
                if k == DEFAULT_NAMESPACE_KEY:
                    defaultLongNS = v
        newNS = NsElement()
        nameSpaceAttrs = ""
        if hasattr(obj, "__xmlnamespaces__"):
            ns = getattr(obj, "__xmlnamespaces__")
            keys = ns.keys()
            keys.sort()
            for nameSpaceKey in keys:
                nameSpaceUrl = ns[nameSpaceKey]
                if nameSpaceUrl in nameSpaces.values():
                    for k, v in nameSpaces.iteritems():
                        if v == nameSpaceUrl:
                            nameSpaceKey = k
                            break
                else:
                    if nameSpaceKey == "":
                        defaultLongNS = nameSpaceUrl
                        nameSpaces[DEFAULT_NAMESPACE_KEY] = nameSpaceUrl
                        newNS.nsMap[DEFAULT_NAMESPACE_KEY] = nameSpaceUrl
                        nameSpaceAttrs += ' xmlns="%s" ' % (nameSpaceUrl)
                    else:
                        nameSpaces[nameSpaceKey] = nameSpaceUrl
                        newNS.nsMap[nameSpaceKey] = nameSpaceUrl
                        nameSpaceAttrs += ' xmlns:%s="%s" ' % (nameSpaceKey, nameSpaceUrl)
            nameSpaceAttrs = nameSpaceAttrs.rstrip()
        if len(self.nsstack) > 0:
            newNS.prefix = self.nsstack[-1].prefix
        else:
            newNS.prefix = ''
        if hasattr(obj, "__xmldefaultnamespace__"):
            longPrefixNS = getattr(obj, "__xmldefaultnamespace__")
            if longPrefixNS == defaultLongNS:
                newNS.prefix = ''
            else:
                try:
                    for k, v in nameSpaces.iteritems():
                        if v == longPrefixNS:
                            newNS.prefix = k + ':'
                            break;
##                    print '[appendNSStack] found longPrefixNS in nameSpaces = "%s"' % (newNS.prefix)
                except:
                    if (longPrefixNS in asDict(self.knownNamespaces)):
                        newNS.prefix = self.knownNamespaces[longPrefixNS] + ':'
                    else:
                        raise MarshallerException('Error marshalling __xmldefaultnamespace__ ("%s") not defined in namespace stack' % (longPrefixNS))
        if hasattr(obj, "targetNamespace"):
            newNS.targetNS = obj.targetNamespace
        elif len(self.nsstack) > 0:
            newNS.targetNS = self.nsstack[-1].targetNS
        newNS.nameSpaces = nameSpaces
        self.nsstack.append(newNS)
        return nameSpaceAttrs       

    def contractQName(self, value, obj, attr):
        value = langToXsdType(value)
        i = value.rfind(':')
        if i >= 0:
            longNS = value[:i]
        else:
            # the value doesn't have a namespace and we couldn't map it to an XSD type...what to do?
            # (a) just write it, as is, and hope it's in the default namespace (for now)
            # (b) throw an exception so we can track down the bad code (later)
            return value
        if (longNS in self.nsstack[-1].nameSpaces.values()):
            for kShort, vLong in self.nsstack[-1].nameSpaces.iteritems():
                if vLong == longNS:
                    shortNS = kShort
                    break
        else:
            shortNS = longNS    # if we can't find the long->short mappping, just use longNS
        if shortNS == DEFAULT_NAMESPACE_KEY:
            value = value[i+1:]
        else:
            value = shortNS + ':' + value[i+1:]
        return value

    def _genObjTypeStr(self, typeString):
        if self.marshalType:
            return ' objtype="%s"' % typeString
        return ""
            
    def _marshal(self, obj, elementName=None, nameSpacePrefix="", indent=0):
        if (obj != None):
            xmlMarshallerLogger.debug("--> _marshal: elementName=%s%s, type=%s, obj=%s, indent=%d" % (nameSpacePrefix, elementName, type(obj), str(obj), indent))
        else:
            xmlMarshallerLogger.debug("--> _marshal: elementName=%s%s, obj is None, indent=%d" % (nameSpacePrefix, elementName, indent))
        excludeAttrs = []
        excludeAttrs.extend(self.xmldeepexclude)
        if hasattr(obj, "__xmlexclude__"):
            excludeAttrs.extend(obj.__xmlexclude__)
        prettyPrint = self.prettyPrint
        knownTypes = self.knownTypes
        xmlString = None
        if self.prettyPrint or indent:
            prefix = " "*indent
            newline = "\n"
            increment = 2
        else:
            prefix = ""
            newline = ""
            increment = 0
        ## Determine the XML element name. If it isn"t specified in the
        ## parameter list, look for it in the __xmlname__ Lang
        ## attribute, else use the default generic BASETYPE_ELEMENT_NAME.
        nameSpaceAttrs = self.appendNSStack(obj)
        nameSpacePrefix = self.getNSPrefix()       
        if not elementName:
            if hasattr(obj, "__xmlname__"):
                elementName = nameSpacePrefix + obj.__xmlname__
            else:
                elementName = nameSpacePrefix + BASETYPE_ELEMENT_NAME
        else:
            elementName = nameSpacePrefix + elementName
##        print '[XMLMarshalWorker._marshal] elementName "%s"; nameSpaceAttrs is "%s"' % (elementName, nameSpaceAttrs)
    
        if (hasattr(obj, "__xmlsequencer__")) and (obj.__xmlsequencer__ != None):
            if (XMLSCHEMA_XSD_URL in self.nsstack[-1].nameSpaces.values()):
                for kShort, vLong in self.nsstack[-1].nameSpaces.iteritems():
                    if vLong == XMLSCHEMA_XSD_URL:
                        xsdPrefix = kShort + ':'
                        break
            else:
                xsdPrefix = 'xs:'
            elementAdd = xsdPrefix + obj.__xmlsequencer__
        else:
            elementAdd = None
                   
    ##    print "marshal: entered with elementName: ", elementName
        members_to_skip = []
        ## Add more members_to_skip based on ones the user has selected
        ## via the __xmlexclude__ and __xmldeepexclude__ attributes.
        members_to_skip.extend(excludeAttrs)
        # Marshal the attributes that are selected to be XML attributes.
        objattrs = ""
        className = ag_className(obj)
        classNamePrefix = "_" + className
        if hasattr(obj, "__xmlattributes__"):
            xmlattributes = obj.__xmlattributes__
            members_to_skip.extend(xmlattributes)
            for attr in xmlattributes:
##                print 'Processing element "%s"; attribute "%s"' % (elementName, attr)
                internalAttrName = attr
                ifDefPy()
                if (attr.startswith("__") and not attr.endswith("__")): 
                    internalAttrName = classNamePrefix + attr
                endIfDef()
                # Fail silently if a python attribute is specified to be
                # an XML attribute but is missing.
##            print "marshal:   processing attribute ", internalAttrName
                attrNameSpacePrefix = ""
                if hasattr(obj, "__xmlattrnamespaces__"):
                    for nameSpaceKey, nameSpaceAttributes in getattr(obj, "__xmlattrnamespaces__").iteritems():
                        if nameSpaceKey == nameSpacePrefix[:-1]: # Don't need to specify attribute namespace if it is the same as its element
                            continue
                        if attr in nameSpaceAttributes:
                            attrNameSpacePrefix = nameSpaceKey + ":"
                            break
                attrs = obj.__dict__
                value = attrs.get(internalAttrName)
                if (hasattr(obj, "__xmlrename__") and attr in asDict(obj.__xmlrename__)):
                    attr = obj.__xmlrename__[attr]
                xsdElement = None
                complexType = getComplexType(obj)
                if (complexType != None):
                    xsdElement = complexType.findElement(attr)
                if (xsdElement != None):
                    default = xsdElement.default
                    if (default != None):
                        if ((default == value) or (default == _getXmlValue(value))):
                            continue
                    else:
                        if (value == None):
                            continue
                        elif xsdElement.type == TYPE_QNAME:
                            value = self.contractQName(value, obj, attr)
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
                else:
                    value = objutils.toDiffableRepr(value)
    
                objattrs += ' %s%s="%s"' % (attrNameSpacePrefix, attr, saxutils.escape(value))
    ##            print "marshal:   new objattrs is: ", objattrs
        if (obj == None):
            xmlString = [""]
        elif isinstance(obj, bool):
            objTypeStr = self._genObjTypeStr("bool")
            xmlString = ['%s<%s%s>%s</%s>%s' % (prefix, elementName, objTypeStr, obj, elementName, newline)]
        elif isinstance(obj, int):
            objTypeStr = self._genObjTypeStr("int")
            xmlString = ['%s<%s%s>%s</%s>%s' % (prefix, elementName, objTypeStr, str(obj), elementName, newline)]
        elif isinstance(obj, long):
            objTypeStr = self._genObjTypeStr("long")
            xmlString = ['%s<%s%s>%s</%s>%s' % (prefix, elementName, objTypeStr, str(obj), elementName, newline)]
        elif isinstance(obj, float):
            objTypeStr = self._genObjTypeStr("float")
            xmlString = ['%s<%s%s>%s</%s>%s' % (prefix, elementName, objTypeStr, str(obj), elementName, newline)]
        elif isinstance(obj, unicode): # have to check before basestring - unicode is instance of base string
            xmlString = ['%s<%s>%s</%s>%s' % (prefix, elementName, saxutils.escape(obj.encode()), elementName, newline)]
        elif isinstance(obj, basestring):
            xmlString = ['%s<%s>%s</%s>%s' % (prefix, elementName, saxutils.escape(obj), elementName, newline)]
        elif isinstance(obj, list):
            if len(obj) < 1:
                xmlString = ""
            else:
                objTypeStr = self._genObjTypeStr("list")
                xmlString = ['%s<%s%s>%s' % (prefix, elementName, objTypeStr, newline)]
                for item in obj:
                    xmlString.extend(self._marshal(item, indent=indent+increment))
                xmlString.append("%s</%s>%s" % (prefix, elementName, newline))
        elif isinstance(obj, tuple):
            if len(obj) < 1:
                xmlString = ""
            else:
                objTypeStr = self._genObjTypeStr("list")
                xmlString = ['%s<%s%s mutable="false">%s' % (prefix, elementName, objTypeStr, newline)]
                for item in obj:
                    xmlString.extend(self._marshal(item, indent=indent+increment))
                xmlString.append("%s</%s>%s" % (prefix, elementName, newline))
        elif isinstance(obj, dict):
            objTypeStr = self._genObjTypeStr("dict")
            xmlString = ['%s<%s%s>%s' % (prefix, elementName, objTypeStr, newline)]
            subprefix = prefix + " "*increment
            subindent = indent + 2*increment
            keys = obj.keys()
            keys.sort()
            for key in keys:
                xmlString.append("%s<%s>%s" % (subprefix, DICT_ITEM_NAME, newline))
                xmlString.extend(self._marshal(key, elementName=DICT_ITEM_KEY_NAME, indent=subindent))
                xmlString.extend(self._marshal(obj[key], elementName=DICT_ITEM_VALUE_NAME, indent=subindent))
                xmlString.append("%s</%s>%s" % (subprefix, DICT_ITEM_NAME, newline))
            xmlString.append("%s</%s>%s" % (prefix, elementName, newline))
        elif hasattr(obj, "__xmlcontent__"):
            contentValue = getattr(obj, obj.__xmlcontent__)
            if contentValue == None: 
                contentValue = ''
            else:
                contentValue = saxutils.escape(contentValue)
            xmlString = ["%s<%s%s%s>%s</%s>%s" % (prefix, elementName, nameSpaceAttrs, objattrs, contentValue, elementName, newline)]        
        else:
            # Only add the objtype if the element tag is unknown to us.
            if (self.isKnownType(elementName) == True): 
                objTypeStr = ""
            else:
                objTypeStr = self._genObjTypeStr("%s.%s" % (obj.__class__.__module__, className))
            xmlString = ['%s<%s%s%s%s' % (prefix, elementName, nameSpaceAttrs, objattrs, objTypeStr)]
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
                        objTypeStr = self._genObjTypeStr("None")
                        xmlMemberString.append('%s<%s%s>%s' % (prefix, eName, objTypeStr, newline))
                    for name in eList:
                        value = obj.__dict__[name]
                        if eName == "__nogroup__" and name in members_to_skip: continue
                        if name.startswith("__") and name.endswith("__"): continue
                        if (hasattr(obj, "__xmlcdatacontent__") and (obj.__xmlcdatacontent__ == name)):
                            continue
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
                            if not isinstance(value, (list, tuple)):
                              value = [value]
                            for seqitem in value:
    ##                            xmlname = xmlnametuple[ix]
    ##                            ix += 1
    ##                            if (ix >= len(xmlnametuple)):
    ##                                ix = 0
                                xmlMemberString.extend(self._marshal(seqitem, xmlname, subElementNameSpacePrefix, indent=indent+increment))
                        else:
                            if (hasattr(obj, "__xmlrename__") and name in asDict(obj.__xmlrename__)):
                                xmlname = obj.__xmlrename__[name]
                            else:
                                xmlname = name
                            xmlMemberString.extend(self._marshal(value, xmlname, subElementNameSpacePrefix, indent=indent+increment))
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
                if hasattr(obj, "__xmlcdatacontent__"):
                    cdataAttr = obj.__xmlcdatacontent__
                    cdataContent = obj.__dict__[cdataAttr]
                    xmlString.append("><![CDATA[%s]]></%s>%s" % (cdataContent, elementName, newline))
                else:
                    xmlString.append("/>%s" % newline)
    ##        return xmlString
        xmlMarshallerLogger.debug("<-- _marshal: %s" % str(xmlString))
        #print "<-- _marshal: %s" % str(xmlString)
        self.popNSStack()
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
