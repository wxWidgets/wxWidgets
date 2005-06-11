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

import os
import activegrid.util.objutils as objutils
import activegrid.util.xmlmarshaller as xmlmarshaller

agKnownTypes = None

def defaultLoad(fileObject, knownTypes=None):
    xml = fileObject.read()
    loadedObject = unmarshal(xml, knownTypes=knownTypes)
    if hasattr(fileObject, 'name'):
        loadedObject.fileName = os.path.abspath(fileObject.name)
    loadedObject.initialize()
    return loadedObject

def unmarshal(xml, knownTypes=None):
    if not knownTypes: knownTypes = getAgKnownTypes()
    return xmlmarshaller.unmarshal(xml, knownTypes=knownTypes)    

def defaultSave(fileObject, objectToSave, prettyPrint=True, knownTypes=None, encoding='utf-8'):
    xml = marshal(objectToSave, prettyPrint=prettyPrint, knownTypes=knownTypes, encoding=encoding)
    fileObject.write(xml)
    fileObject.flush()

def marshal(objectToSave, prettyPrint=True, knownTypes=None, encoding='utf-8'):
    if not knownTypes: knownTypes = getAgKnownTypes()
    return xmlmarshaller.marshal(objectToSave, prettyPrint=prettyPrint, knownTypes=knownTypes, encoding=encoding)
    
def cloneObject(objectToClone, knownTypes=None, encoding='utf-8'):
    if not knownTypes: knownTypes = getAgKnownTypes()
    xml = xmlmarshaller.marshal(objectToClone, prettyPrint=True, knownTypes=knownTypes, encoding=encoding)
    clonedObject = xmlmarshaller.unmarshal(xml, knownTypes=knownTypes)
    if hasattr(objectToClone, 'fileName'):
        clonedObject.fileName = objectToClone.fileName
    try:
        clonedObject.initialize()
    except AttributeError:
        pass
    return clonedObject

def getAgKnownTypes():
    import activegrid.model.processmodel
    import activegrid.model.schema
    import activegrid.data.dataservice
    import activegrid.server.deployment
    global agKnownTypes
    if agKnownTypes == None:
        tmpAgKnownTypes = {} 
        AG_TYPE_MAPPING = { 
            "ag:append"          : "activegrid.model.processmodel.AppendOperation",
            "ag:body"            : "activegrid.model.processmodel.Body",
            "ag:cssRule"         : "activegrid.model.processmodel.CssRule",
            "ag:datasource"      : "activegrid.data.dataservice.DataSource",
            "ag:debug"           : "activegrid.model.processmodel.DebugOperation",
            "ag:deployment"      : "activegrid.server.deployment.Deployment",
            "ag:glue"            : "activegrid.model.processmodel.Glue",
            "ag:hr"              : "activegrid.model.processmodel.HorizontalRow",
            "ag:image"           : "activegrid.model.processmodel.Image",
            "ag:inputs"          : "activegrid.model.processmodel.Inputs",
            "ag:label"           : "activegrid.model.processmodel.Label",
            "ag:processmodel"    : "activegrid.model.processmodel.ProcessModel",
            "ag:processmodelref" : "activegrid.server.deployment.ProcessModelRef",
            "ag:query"           : "activegrid.model.processmodel.Query",
            "ag:restParameter"   : "activegrid.server.deployment.RestParameter",
            "ag:restService"     : "activegrid.server.deployment.RestService",
            "ag:schemaOptions"   : "activegrid.model.schema.SchemaOptions",
            "ag:schemaref"       : "activegrid.server.deployment.SchemaRef",
            "ag:serviceref"      : "activegrid.server.deployment.ServiceRef",
            "ag:set"             : "activegrid.model.processmodel.SetOperation",
            "ag:text"            : "activegrid.model.processmodel.Text",
            "ag:title"           : "activegrid.model.processmodel.Title",
            "ag:view"            : "activegrid.model.processmodel.View",
            "bpws:case"          : "activegrid.model.processmodel.BPELCase",
            "bpws:catch"         : "activegrid.model.processmodel.BPELCatch",
            "bpws:faultHandlers" : "activegrid.model.processmodel.BPELFaultHandlers",
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
            "wsdl:message"       : "activegrid.model.processmodel.WSDLMessage",
            "wsdl:part"          : "activegrid.model.processmodel.WSDLPart",
            "xforms:group"       : "activegrid.model.processmodel.XFormsGroup",
            "xforms:input"       : "activegrid.model.processmodel.XFormsInput",
            "xforms:label"       : "activegrid.model.processmodel.XFormsLabel",
            "xforms:output"      : "activegrid.model.processmodel.XFormsOutput",
            "xforms:secret"      : "activegrid.model.processmodel.XFormsSecret",
            "xforms:submit"      : "activegrid.model.processmodel.XFormsSubmit",
            "xs:all"             : "activegrid.model.schema.XsdSequence",
            "xs:complexType"     : "activegrid.model.schema.XsdComplexType",
            "xs:element"         : "activegrid.model.schema.XsdElement",
            "xs:field"           : "activegrid.model.schema.XsdKeyField",
            "xs:key"             : "activegrid.model.schema.XsdKey",
            "xs:keyref"          : "activegrid.model.schema.XsdKeyRef",
            "xs:schema"          : "activegrid.model.schema.Schema",
            "xs:selector"        : "activegrid.model.schema.XsdKeySelector",              
            "xs:sequence"        : "activegrid.model.schema.XsdSequence",
          }
                   
        for keyName, className in AG_TYPE_MAPPING.iteritems():
            try:
                tmpAgKnownTypes[keyName] = objutils.classForName(className)
            except KeyError:
                print "Error mapping knownType", className
                pass
        if len(tmpAgKnownTypes) > 0:
            agKnownTypes = tmpAgKnownTypes
    return agKnownTypes
