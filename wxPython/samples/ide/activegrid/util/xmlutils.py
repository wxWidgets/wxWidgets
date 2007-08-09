#----------------------------------------------------------------------------
# Name:         xmlutils.py
# Purpose:      XML and Marshaller Utilities
#
# Author:       Jeff Norton
#
# Created:      6/2/05
# CVS-ID:       $Id$
# Copyright:    (c) 2004-2005 ActiveGrid, Inc.
# License:      wxWindows License
#----------------------------------------------------------------------------

from activegrid.util.lang import *
import os
import time
import urllib
import logging
from activegrid.util.lang import *
import activegrid.util.objutils as objutils
import activegrid.util.xmlmarshaller as xmlmarshaller
import activegrid.util.aglogging as aglogging

xmlLogger = logging.getLogger("activegrid.util.xml")
    
def load(fileName, knownTypes=None, knownNamespaces=None, createGenerics=False):
    loadedObject = None
    fileObject = file(fileName)
    timeStart = time.time()
    xml = ""
    try:
        xml = fileObject.read()
        loadedObject = unmarshal(xml, knownTypes=knownTypes, knownNamespaces=knownNamespaces, xmlSource=fileName, createGenerics=createGenerics)
        loadedObject.fileName = os.path.abspath(fileName)
        if hasattr(loadedObject, 'initialize'):
            loadedObject.initialize()
    finally:
        fileObject.close()
        if xmlLogger.isEnabledFor(aglogging.LEVEL_INFO):
            timeDone = time.time()
            aglogging.info(xmlLogger, ('Load statistics for file %s (%d bytes): elapsed time = %f secs' % (fileName, len(xml), timeDone-timeStart)))
    return loadedObject

def loadURI(uri, knownTypes=None, knownNamespaces=None, xmlSource=None, createGenerics=False):
    loadedObject = None
    timeStart = time.time()
    xml = ""
    try:
        xml = urllib.urlopen(uri).read()
        loadedObject = unmarshal(xml, knownTypes=knownTypes, knownNamespaces=knownNamespaces, xmlSource=xmlSource, createGenerics=createGenerics)
        loadedObject.fileName = uri
        if hasattr(loadedObject, 'initialize'):
            loadedObject.initialize()
    finally:
        if xmlLogger.isEnabledFor(aglogging.LEVEL_INFO):
            timeDone = time.time()
            aglogging.info(xmlLogger, ('Load statistics for URI %s (%d bytes): elapsed time = %f secs' % (uri, len(xml), timeDone-timeStart)))
    return loadedObject

def unmarshal(xml, knownTypes=None, knownNamespaces=None, xmlSource=None, createGenerics=False):
    if (knownTypes == None): 
        knownTypes, knownNamespaces = getAgKnownTypes()
    return xmlmarshaller.unmarshal(xml, knownTypes=knownTypes, knownNamespaces=knownNamespaces, xmlSource=xmlSource, createGenerics=createGenerics)    

def save(fileName, objectToSave, prettyPrint=True, marshalType=True, knownTypes=None, knownNamespaces=None, encoding='utf-8'):
    if hasattr(objectToSave, '_xmlReadOnly') and objectToSave._xmlReadOnly == True:
        raise xmlmarshaller.MarshallerException('Error marshalling object to file "%s": object is marked "readOnly" and cannot be written' % (fileName))        
    timeStart = time.time()
    xml = marshal(objectToSave, prettyPrint=prettyPrint, marshalType=marshalType, knownTypes=knownTypes, knownNamespaces=knownNamespaces, encoding=encoding)
    fileObject = file(fileName, 'w')
    try:
        fileObject.write(xml)
        fileObject.flush()
    except Exception, errorData:
        fileObject.close()
        raise xmlmarshaller.MarshallerException('Error marshalling object to file "%s": %s' % (fileName, str(errorData)))
    fileObject.close()
    timeDone = time.time()
    aglogging.info(xmlLogger, ('Save statistics for file %s: elapsed time = %f secs' % (fileName, timeDone-timeStart)))
    
def marshal(objectToSave, prettyPrint=True, marshalType=True, knownTypes=None, knownNamespaces=None, encoding='utf-8'):
    if (knownTypes == None): 
        knownTypes, knownNamespaces = getAgKnownTypes()
    return xmlmarshaller.marshal(objectToSave, prettyPrint=prettyPrint, marshalType=marshalType, knownTypes=knownTypes, knownNamespaces=knownNamespaces, encoding=encoding)
    
def addNSAttribute(xmlDoc, shortNamespace, longNamespace):
    if not hasattr(xmlDoc, "__xmlnamespaces__"):
        xmlDoc.__xmlnamespaces__ = {shortNamespace:longNamespace}
    elif shortNamespace not in xmlDoc.__xmlnamespaces__:
        if (hasattr(xmlDoc.__class__, "__xmlnamespaces__") 
            and (xmlDoc.__xmlnamespaces__ is xmlDoc.__class__.__xmlnamespaces__)):
            xmlDoc.__xmlnamespaces__ = dict(xmlDoc.__xmlnamespaces__)
        xmlDoc.__xmlnamespaces__[shortNamespace] = longNamespace

def genShortNS(xmlDoc, longNamespace=None):
    if not hasattr(xmlDoc, "__xmlnamespaces__"):
        return "ns1"
    elif longNamespace != None and longNamespace in xmlDoc.__xmlnamespaces__.items():
        for key, value in xmlDoc.__xmlnamespaces__.iteritems():
            if value == longNamespace:
                return key
    i = 1
    while ("ns%d" % i) in xmlDoc.__xmlnamespaces__:
        i += 1
    return ("ns%d" % i)
    
def genTargetNS(fileName, applicationName=None, type=None):
    if (applicationName != None):
        if (type != None):
            tns = "urn:%s:%s:%s" % (applicationName, type, fileName)
        else:
            tns = "urn:%s:%s" % (applicationName, fileName)
    else:
        tns = "urn:%s" % fileName
    return tns
    
def splitType(typeName):
    index = typeName.rfind(':')
    if index != -1:
        ns = typeName[:index]
        complexTypeName = typeName[index+1:]
    else:
        ns = None
        complexTypeName = typeName
    return (ns, complexTypeName)
        
def cloneObject(objectToClone, knownTypes=None, marshalType=True, knownNamespaces=None, encoding='utf-8'):
    if (knownTypes == None): 
        knownTypes, knownNamespaces = getAgKnownTypes()
    xml = xmlmarshaller.marshal(objectToClone, prettyPrint=True, marshalType=marshalType, knownTypes=knownTypes, knownNamespaces=knownNamespaces, encoding=encoding)
    clonedObject = xmlmarshaller.unmarshal(xml, knownTypes=knownTypes, knownNamespaces=knownNamespaces)
    if hasattr(objectToClone, 'fileName'):
        clonedObject.fileName = objectToClone.fileName
    if hasattr(objectToClone, "_parentDoc"):
        clonedObject._parentDoc = objectToClone._parentDoc
    try:
        clonedObject.initialize()
    except AttributeError:
        pass
    return clonedObject

def getAgVersion(fileName):
    fileObject = file(fileName)
    try:
        xml = fileObject.read()
    finally:
        fileObject.close()
    i = xml.find(' ag:version=')
    if i >= 0:
        i += 12
    else:
        i2 = xml.find('<ag:')
        if i2 >= 0:
            i = xml.find(' version=', i2)
            if i > 0:
                i += 9
        elif xml.find('<project version="10"') >= 0:
            return "10"
        else:
            return None
    version = None
    if xml[i:i+1] == '"':
        j = xml.find('"', i+1)
        if (j > i+1):
            version = xml[i+1:j]
    return version

    
AG_NS_URL = "http://www.activegrid.com/ag.xsd"
BPEL_NS_URL = "http://schemas.xmlsoap.org/ws/2003/03/business-process"
HTTP_WSDL_NS_URL = "http://schemas.xmlsoap.org/wsdl/http/"
MIME_WSDL_NS_URL = "http://schemas.xmlsoap.org/wsdl/mime/"
SOAP_NS_URL = "http://schemas.xmlsoap.org/wsdl/soap/"
SOAP12_NS_URL = "http://schemas.xmlsoap.org/wsdl/soap12/"
SOAP_NS_ENCODING = "http://schemas.xmlsoap.org/soap/encoding/"
WSDL_NS_URL = "http://schemas.xmlsoap.org/wsdl/"
WSSE_NS_URL = "http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-secext-1.0.xsd"
XFORMS_NS_URL = "http://www.w3c.org/xform.xsd"
XMLSCHEMA_NS_URL = "http://www.w3.org/2001/XMLSchema"
XSI_NS_URL = "http://www.w3.org/2001/XMLSchema-instance"
XACML_NS_URL = "urn:oasis:names:tc:xacml:2.0:policy:schema:os"

KNOWN_NAMESPACES = { AG_NS_URL          :  "ag",
                     BPEL_NS_URL        :  "bpws",
                     HTTP_WSDL_NS_URL   :  "http",
                     MIME_WSDL_NS_URL   :  "mime",
                     SOAP_NS_URL        :  "soap",
                     SOAP12_NS_URL      :  "soap12",
                     WSDL_NS_URL        :  "wsdl",
                     WSSE_NS_URL        :  "wsse", 
                     XFORMS_NS_URL      :  "xforms",                             
                     XMLSCHEMA_NS_URL   :  "xs",
                     XACML_NS_URL       :  "xacml",
                   }
    
global agXsdToClassName
agXsdToClassName = None
def getAgXsdToClassName():
    global agXsdToClassName
    if (agXsdToClassName == None):
        agXsdToClassName = {
            "ag:append"          : "activegrid.model.processmodel.AppendOperation",
            "ag:attribute"       : "activegrid.model.identitymodel.Attribute",
            "ag:body"            : "activegrid.model.processmodel.Body",
            "ag:category_substitutions"    : "activegrid.server.layoutrenderer.CategorySubstitutions",
            "ag:command"         : "activegrid.model.wsdl.Command",
            "ag:setElement"      : "activegrid.model.processmodel.SetElementOperation",
            "ag:css"             : "activegrid.server.layoutrenderer.CSS", 
            "ag:databaseService" : "activegrid.server.deployment.DatabaseService",
            "ag:datasource"      : "activegrid.data.dataservice.DataSource",
            "ag:dataObjectList"  : "activegrid.data.datalang.DataObjectList",
            "ag:debug"           : "activegrid.model.processmodel.DebugOperation",
            "ag:deployment"      : "activegrid.server.deployment.Deployment",
            "ag:formData"        : "activegrid.model.processmodel.FormData",
            "ag:formVar"         : "activegrid.model.processmodel.FormVar",
            "ag:generator"       : "activegrid.server.layoutrenderer.SerializableGenerator", 
            "ag:head"            : "activegrid.server.layoutrenderer.Head", 
            "ag:hr"              : "activegrid.model.processmodel.HorizontalRow",
            "ag:identity"        : "activegrid.model.identitymodel.Identity",
            "ag:identityref"     : "activegrid.server.deployment.IdentityRef",
            "ag:image"           : "activegrid.model.processmodel.Image",
            "ag:inputPart"       : "activegrid.model.processmodel.InputPart",
            "ag:keystore"        : "activegrid.model.identitymodel.KeyStore",
            "ag:label"           : "activegrid.model.processmodel.Label",
            "ag:layout"          : "activegrid.server.layoutrenderer.Layout", 
            "ag:layouts"         : "activegrid.server.layoutrenderer.Layouts", 
            "ag:ldapsource"      : "activegrid.model.identitymodel.LDAPSource",
            "ag:localService"    : "activegrid.server.deployment.LocalService",
            "ag:parameter"       : "activegrid.server.layoutrenderer.Parameter",
            "ag:parameters"      : "activegrid.server.layoutrenderer.Parameters",
            "ag:postInitialize"  : "activegrid.model.processmodel.PostInitialize",
            "ag:processref"      : "activegrid.server.deployment.ProcessRef",
            "ag:query"           : "activegrid.model.processmodel.Query",
            "ag:soapService"     : "activegrid.server.deployment.SoapService",
            "ag:redirect"        : "activegrid.server.layoutrenderer.Redirect", 
            "ag:requiredFile"    : "activegrid.server.layoutrenderer.RequiredFile", 
            "ag:resource"        : "activegrid.model.identitymodel.IDResource",
            "ag:restService"     : "activegrid.server.deployment.RestService",
            "ag:rewrite"         : "activegrid.model.wsdl.Rewrite",
            "ag:role"            : "activegrid.model.identitymodel.IDRole",
            "ag:roledefn"        : "activegrid.model.identitymodel.RoleDefn",
            "ag:rssService"      : "activegrid.server.deployment.RssService",
            "ag:rule"            : "activegrid.model.identitymodel.IDRule",
            "ag:schemaOptions"   : "activegrid.model.schema.SchemaOptions",
            "ag:schemaref"       : "activegrid.server.deployment.SchemaRef",
            "ag:serviceCache"    : "activegrid.server.deployment.ServiceCache",
            "ag:serviceExtension": "activegrid.model.wsdl.ServiceExtension",
            "ag:serviceExtensions": "activegrid.model.wsdl.ServiceExtensions",
            "ag:serviceParameter": "activegrid.server.deployment.ServiceParameter",
            "ag:serviceref"      : "activegrid.server.deployment.ServiceRef",
            "ag:set"             : "activegrid.model.processmodel.SetOperation",
            "ag:skinref"         : "activegrid.server.deployment.SkinRef",
            "ag:skin"            : "activegrid.server.layoutrenderer.Skin",
            "ag:skin_element_ref": "activegrid.server.layoutrenderer.SkinElementRef",
            "ag:skin_element"    : "activegrid.server.layoutrenderer.SkinElement",
            "ag:skins"           : "activegrid.server.layoutrenderer.Skins",
            "ag:substitution"    : "activegrid.server.layoutrenderer.Substitution", 
            "ag:text"            : "activegrid.model.processmodel.Text",
            "ag:title"           : "activegrid.model.processmodel.Title",
            "ag:usertemplate"    : "activegrid.model.identitymodel.UserTemplate",
            "ag:xformref"        : "activegrid.server.deployment.XFormRef",
            "bpws:case"          : "activegrid.model.processmodel.BPELCase",
            "bpws:catch"         : "activegrid.model.processmodel.BPELCatch",
            "bpws:faultHandlers" : "activegrid.model.processmodel.BPELFaultHandlers",
            "bpws:flow"          : "activegrid.model.processmodel.BPELFlow",
            "bpws:invoke"        : "activegrid.model.processmodel.BPELInvoke",
            "bpws:onMessage"     : "activegrid.model.processmodel.BPELOnMessage",
            "bpws:otherwise"     : "activegrid.model.processmodel.BPELOtherwise",
            "bpws:pick"          : "activegrid.model.processmodel.BPELPick",
            "bpws:process"       : "activegrid.model.processmodel.BPELProcess",
            "bpws:receive"       : "activegrid.model.processmodel.BPELReceive",
            "bpws:reply"         : "activegrid.model.processmodel.BPELReply",
            "bpws:scope"         : "activegrid.model.processmodel.BPELScope",
            "bpws:sequence"      : "activegrid.model.processmodel.BPELSequence",
            "bpws:switch"        : "activegrid.model.processmodel.BPELSwitch",
            "bpws:terminate"     : "activegrid.model.processmodel.BPELTerminate",
            "bpws:variable"      : "activegrid.model.processmodel.BPELVariable",
            "bpws:variables"     : "activegrid.model.processmodel.BPELVariables",
            "bpws:while"         : "activegrid.model.processmodel.BPELWhile",
            "http:address"       : "activegrid.model.wsdl.HttpAddress",
            "http:binding"       : "activegrid.model.wsdl.HttpBinding",
            "http:operation"     : "activegrid.model.wsdl.HttpOperation",
            "http:urlEncoded"    : "activegrid.model.wsdl.HttpUrlEncoded",
            "mime:content"       : "activegrid.model.wsdl.MimeContent",
            "mime:mimeXml"       : "activegrid.model.wsdl.MimeMimeXml",
            "soap:address"       : "activegrid.model.wsdl.SoapAddress",
            "soap:binding"       : "activegrid.model.wsdl.SoapBinding",
            "soap:body"          : "activegrid.model.wsdl.SoapBody",
            "soap:fault"         : "activegrid.model.wsdl.SoapFault",
            "soap:header"        : "activegrid.model.wsdl.SoapHeader",
            "soap:operation"     : "activegrid.model.wsdl.SoapOperation",
            "soap12:address"     : "activegrid.model.wsdl.Soap12Address",
            "soap12:binding"     : "activegrid.model.wsdl.Soap12Binding",
            "soap12:body"        : "activegrid.model.wsdl.Soap12Body",
            "soap12:fault"       : "activegrid.model.wsdl.Soap12Fault",
            "soap12:header"      : "activegrid.model.wsdl.Soap12Header",
            "soap12:operation"   : "activegrid.model.wsdl.Soap12Operation",
            "wsdl:binding"       : "activegrid.model.wsdl.WsdlBinding",
            "wsdl:definitions"   : "activegrid.model.wsdl.WsdlDocument",
            "wsdl:documentation" : "activegrid.model.wsdl.WsdlDocumentation",
            "wsdl:fault"         : "activegrid.model.wsdl.WsdlFault",
            "wsdl:import"        : "activegrid.model.wsdl.WsdlImport",
            "wsdl:input"         : "activegrid.model.wsdl.WsdlInput",
            "wsdl:message"       : "activegrid.model.wsdl.WsdlMessage",
            "wsdl:operation"     : "activegrid.model.wsdl.WsdlOperation",
            "wsdl:output"        : "activegrid.model.wsdl.WsdlOutput",
            "wsdl:part"          : "activegrid.model.wsdl.WsdlPart",
            "wsdl:port"          : "activegrid.model.wsdl.WsdlPort",
            "wsdl:portType"      : "activegrid.model.wsdl.WsdlPortType",
            "wsdl:service"       : "activegrid.model.wsdl.WsdlService",
            "wsdl:types"         : "activegrid.model.wsdl.WsdlTypes",
            "xacml:Action"       : "activegrid.model.identitymodel.XACMLAction",
            "xacml:ActionAttributeDesignator" : "activegrid.model.identitymodel.XACMLActionAttributeDesignator",
            "xacml:ActionMatch"  : "activegrid.model.identitymodel.XACMLActionMatch",
            "xacml:Actions"      : "activegrid.model.identitymodel.XACMLActions",
            "xacml:AttributeValue" : "activegrid.model.identitymodel.XACMLAttributeValue",
            "xacml:Policy"       : "activegrid.model.identitymodel.XACMLPolicy",
            "xacml:Resource"     : "activegrid.model.identitymodel.XACMLResource",
            "xacml:ResourceAttributeDesignator" : "activegrid.model.identitymodel.XACMLResourceAttributeDesignator",
            "xacml:ResourceMatch" : "activegrid.model.identitymodel.XACMLResourceMatch",
            "xacml:Resources"    : "activegrid.model.identitymodel.XACMLResources",
            "xacml:Rule"         : "activegrid.model.identitymodel.XACMLRule",
            "xacml:Target"       : "activegrid.model.identitymodel.XACMLTarget",
            "xforms:copy"        : "activegrid.model.processmodel.XFormsCopy",
            "xforms:group"       : "activegrid.model.processmodel.XFormsGroup",
            "xforms:include"     : "activegrid.model.processmodel.XFormsInclude",
            "xforms:input"       : "activegrid.model.processmodel.XFormsInput",
            "xforms:item"        : "activegrid.model.processmodel.XFormsItem",
            "xforms:itemset"     : "activegrid.model.processmodel.XFormsItemset",
            "xforms:label"       : "activegrid.model.processmodel.XFormsLabel",
            "xforms:model"       : "activegrid.model.processmodel.XFormsModel",
            "xforms:output"      : "activegrid.model.processmodel.XFormsOutput",
            "xforms:secret"      : "activegrid.model.processmodel.XFormsSecret",
            "xforms:select1"     : "activegrid.model.processmodel.XFormsSelect1",
            "xforms:submission"  : "activegrid.model.processmodel.XFormsSubmission",
            "xforms:submit"      : "activegrid.model.processmodel.XFormsSubmit",
            "xforms:value"       : "activegrid.model.processmodel.XFormsValue",
            "xforms:xform"       : "activegrid.model.processmodel.View",
            "xforms:xforms"      : "activegrid.model.processmodel.XFormsRoot",
            "xs:all"             : "activegrid.model.schema.XsdSequence",
            "xs:any"             : "activegrid.model.schema.XsdAny",
            "xs:anyAttribute"    : "activegrid.model.schema.XsdAnyAttribute",
            "xs:attribute"       : "activegrid.model.schema.XsdAttribute",
            "xs:choice"          : "activegrid.model.schema.XsdChoice",
            "xs:complexContent"  : "activegrid.model.schema.XsdComplexContent",
            "xs:complexType"     : "activegrid.model.schema.XsdComplexType",
            "xs:documentation"   : "activegrid.model.schema.XsdDocumentation",
            "xs:element"         : "activegrid.model.schema.XsdElement",
            "xs:enumeration"     : "activegrid.model.schema.XsdFacetEnumeration",
            "xs:extension"       : "activegrid.model.schema.XsdExtension",
            "xs:fractionDigits"  : "activegrid.model.schema.XsdFacetFractionDigits",
            "xs:field"           : "activegrid.model.schema.XsdKeyField",
            "xs:import"          : "activegrid.model.schema.XsdInclude",
            "xs:include"         : "activegrid.model.schema.XsdInclude",
            "xs:key"             : "activegrid.model.schema.XsdKey",
            "xs:keyref"          : "activegrid.model.schema.XsdKeyRef",
            "xs:length"          : "activegrid.model.schema.XsdFacetLength",
            "xs:list"            : "activegrid.model.schema.XsdList",
            "xs:maxExclusive"    : "activegrid.model.schema.XsdFacetMaxExclusive",
            "xs:maxInclusive"    : "activegrid.model.schema.XsdFacetMaxInclusive",
            "xs:maxLength"       : "activegrid.model.schema.XsdFacetMaxLength",
            "xs:minExclusive"    : "activegrid.model.schema.XsdFacetMinExclusive",
            "xs:minInclusive"    : "activegrid.model.schema.XsdFacetMinInclusive",
            "xs:minLength"       : "activegrid.model.schema.XsdFacetMinLength",
            "xs:pattern"         : "activegrid.model.schema.XsdFacetPattern",
            "xs:restriction"     : "activegrid.model.schema.XsdRestriction",
            "xs:schema"          : "activegrid.model.schema.Schema",
            "xs:selector"        : "activegrid.model.schema.XsdKeySelector",              
            "xs:sequence"        : "activegrid.model.schema.XsdSequence",
            "xs:simpleContent"   : "activegrid.model.schema.XsdSimpleContent",
            "xs:simpleType"      : "activegrid.model.schema.XsdSimpleType",
            "xs:totalDigits"     : "activegrid.model.schema.XsdFacetTotalDigits",
            "xs:whiteSpace"      : "activegrid.model.schema.XsdFacetWhiteSpace",
        }
    return agXsdToClassName
    
global agKnownTypes
agKnownTypes = None
def getAgKnownTypes():
    global agKnownTypes
    if agKnownTypes == None:
        try:
            tmpAgKnownTypes = {}
            import activegrid.model.processmodel
            import activegrid.model.schema
            import activegrid.server.deployment
            import activegrid.model.wsdl
            ifDefPy()
            import activegrid.data.dataservice
            endIfDef()
            for keyName, className in getAgXsdToClassName().iteritems():
                classType = objutils.classForName(className)
                if (classType == None):
                    raise Exception("Cannot get class type for %s" % className)
                else:
                    tmpAgKnownTypes[keyName] = classType
            if len(tmpAgKnownTypes) > 0:
                agKnownTypes = tmpAgKnownTypes
        except ImportError:
            agKnownTypes = {}
    if len(agKnownTypes) == 0:     # standalone IDE and XmlMarshaller don't contain known AG types
        noKnownNamespaces = {}
        return agKnownTypes, noKnownNamespaces            
    return agKnownTypes, KNOWN_NAMESPACES
