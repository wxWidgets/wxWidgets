# This file was created automatically by SWIG.
# Don't modify this file, modify the SWIG interface instead.

import _xrc

import core
wx = core 
#---------------------------------------------------------------------------

WX_XMLRES_CURRENT_VERSION_MAJOR = _xrc.WX_XMLRES_CURRENT_VERSION_MAJOR
WX_XMLRES_CURRENT_VERSION_MINOR = _xrc.WX_XMLRES_CURRENT_VERSION_MINOR
WX_XMLRES_CURRENT_VERSION_RELEASE = _xrc.WX_XMLRES_CURRENT_VERSION_RELEASE
WX_XMLRES_CURRENT_VERSION_REVISION = _xrc.WX_XMLRES_CURRENT_VERSION_REVISION
XRC_USE_LOCALE = _xrc.XRC_USE_LOCALE
XRC_NO_SUBCLASSING = _xrc.XRC_NO_SUBCLASSING
class XmlResource(core.Object):
    def __init__(self, *args, **kwargs):
        newobj = _xrc.new_XmlResource(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self.InitAllHandlers()
    def __del__(self, destroy=_xrc.delete_XmlResource):
        try:
            if self.thisown: destroy(self)
        except: pass
    def Load(*args, **kwargs): return _xrc.XmlResource_Load(*args, **kwargs)
    def LoadFromString(*args, **kwargs): return _xrc.XmlResource_LoadFromString(*args, **kwargs)
    def InitAllHandlers(*args, **kwargs): return _xrc.XmlResource_InitAllHandlers(*args, **kwargs)
    def AddHandler(*args, **kwargs): return _xrc.XmlResource_AddHandler(*args, **kwargs)
    def InsertHandler(*args, **kwargs): return _xrc.XmlResource_InsertHandler(*args, **kwargs)
    def ClearHandlers(*args, **kwargs): return _xrc.XmlResource_ClearHandlers(*args, **kwargs)
    AddSubclassFactory = staticmethod(_xrc.XmlResource_AddSubclassFactory)
    def LoadMenu(*args, **kwargs): return _xrc.XmlResource_LoadMenu(*args, **kwargs)
    def LoadMenuBar(*args, **kwargs): return _xrc.XmlResource_LoadMenuBar(*args, **kwargs)
    def LoadMenuBarOnFrame(*args, **kwargs): return _xrc.XmlResource_LoadMenuBarOnFrame(*args, **kwargs)
    def LoadToolBar(*args, **kwargs): return _xrc.XmlResource_LoadToolBar(*args, **kwargs)
    def LoadDialog(*args, **kwargs): return _xrc.XmlResource_LoadDialog(*args, **kwargs)
    def LoadOnDialog(*args, **kwargs): return _xrc.XmlResource_LoadOnDialog(*args, **kwargs)
    def LoadPanel(*args, **kwargs): return _xrc.XmlResource_LoadPanel(*args, **kwargs)
    def LoadOnPanel(*args, **kwargs): return _xrc.XmlResource_LoadOnPanel(*args, **kwargs)
    def LoadFrame(*args, **kwargs): return _xrc.XmlResource_LoadFrame(*args, **kwargs)
    def LoadOnFrame(*args, **kwargs): return _xrc.XmlResource_LoadOnFrame(*args, **kwargs)
    def LoadObject(*args, **kwargs): return _xrc.XmlResource_LoadObject(*args, **kwargs)
    def LoadOnObject(*args, **kwargs): return _xrc.XmlResource_LoadOnObject(*args, **kwargs)
    def LoadBitmap(*args, **kwargs): return _xrc.XmlResource_LoadBitmap(*args, **kwargs)
    def LoadIcon(*args, **kwargs): return _xrc.XmlResource_LoadIcon(*args, **kwargs)
    def AttachUnknownControl(*args, **kwargs): return _xrc.XmlResource_AttachUnknownControl(*args, **kwargs)
    GetXRCID = staticmethod(_xrc.XmlResource_GetXRCID)
    def GetVersion(*args, **kwargs): return _xrc.XmlResource_GetVersion(*args, **kwargs)
    def CompareVersion(*args, **kwargs): return _xrc.XmlResource_CompareVersion(*args, **kwargs)
    Get = staticmethod(_xrc.XmlResource_Get)
    Set = staticmethod(_xrc.XmlResource_Set)
    def GetFlags(*args, **kwargs): return _xrc.XmlResource_GetFlags(*args, **kwargs)
    def SetFlags(*args, **kwargs): return _xrc.XmlResource_SetFlags(*args, **kwargs)
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxXmlResource instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class XmlResourcePtr(XmlResource):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = XmlResource
_xrc.XmlResource_swigregister(XmlResourcePtr)

def EmptyXmlResource(*args, **kwargs):
    val = _xrc.new_EmptyXmlResource(*args, **kwargs)
    val.thisown = 1
    val.InitAllHandlers()
    return val

XmlResource_AddSubclassFactory = _xrc.XmlResource_AddSubclassFactory

XmlResource_GetXRCID = _xrc.XmlResource_GetXRCID

XmlResource_Get = _xrc.XmlResource_Get

XmlResource_Set = _xrc.XmlResource_Set

def XRCID(str_id):
    return wxXmlResource_GetXRCID(str_id)

def XRCCTRL(window, str_id, *ignoreargs):
    return window.FindWindowById(XRCID(str_id))

#---------------------------------------------------------------------------

class XmlSubclassFactory(object):
    def __init__(self, *args, **kwargs):
        newobj = _xrc.new_XmlSubclassFactory(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setCallbackInfo(self, XmlSubclassFactory)
    def _setCallbackInfo(*args, **kwargs): return _xrc.XmlSubclassFactory__setCallbackInfo(*args, **kwargs)
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPyXmlSubclassFactory instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class XmlSubclassFactoryPtr(XmlSubclassFactory):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = XmlSubclassFactory
_xrc.XmlSubclassFactory_swigregister(XmlSubclassFactoryPtr)

#---------------------------------------------------------------------------

XML_ELEMENT_NODE = _xrc.XML_ELEMENT_NODE
XML_ATTRIBUTE_NODE = _xrc.XML_ATTRIBUTE_NODE
XML_TEXT_NODE = _xrc.XML_TEXT_NODE
XML_CDATA_SECTION_NODE = _xrc.XML_CDATA_SECTION_NODE
XML_ENTITY_REF_NODE = _xrc.XML_ENTITY_REF_NODE
XML_ENTITY_NODE = _xrc.XML_ENTITY_NODE
XML_PI_NODE = _xrc.XML_PI_NODE
XML_COMMENT_NODE = _xrc.XML_COMMENT_NODE
XML_DOCUMENT_NODE = _xrc.XML_DOCUMENT_NODE
XML_DOCUMENT_TYPE_NODE = _xrc.XML_DOCUMENT_TYPE_NODE
XML_DOCUMENT_FRAG_NODE = _xrc.XML_DOCUMENT_FRAG_NODE
XML_NOTATION_NODE = _xrc.XML_NOTATION_NODE
XML_HTML_DOCUMENT_NODE = _xrc.XML_HTML_DOCUMENT_NODE
class XmlProperty(object):
    def __init__(self, *args, **kwargs):
        newobj = _xrc.new_XmlProperty(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def GetName(*args, **kwargs): return _xrc.XmlProperty_GetName(*args, **kwargs)
    def GetValue(*args, **kwargs): return _xrc.XmlProperty_GetValue(*args, **kwargs)
    def GetNext(*args, **kwargs): return _xrc.XmlProperty_GetNext(*args, **kwargs)
    def SetName(*args, **kwargs): return _xrc.XmlProperty_SetName(*args, **kwargs)
    def SetValue(*args, **kwargs): return _xrc.XmlProperty_SetValue(*args, **kwargs)
    def SetNext(*args, **kwargs): return _xrc.XmlProperty_SetNext(*args, **kwargs)
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxXmlProperty instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class XmlPropertyPtr(XmlProperty):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = XmlProperty
_xrc.XmlProperty_swigregister(XmlPropertyPtr)

class XmlNode(object):
    def __init__(self, *args, **kwargs):
        newobj = _xrc.new_XmlNode(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __del__(self, destroy=_xrc.delete_XmlNode):
        try:
            if self.thisown: destroy(self)
        except: pass
    def AddChild(*args, **kwargs): return _xrc.XmlNode_AddChild(*args, **kwargs)
    def InsertChild(*args, **kwargs): return _xrc.XmlNode_InsertChild(*args, **kwargs)
    def RemoveChild(*args, **kwargs): return _xrc.XmlNode_RemoveChild(*args, **kwargs)
    def AddProperty(*args, **kwargs): return _xrc.XmlNode_AddProperty(*args, **kwargs)
    def AddPropertyName(*args, **kwargs): return _xrc.XmlNode_AddPropertyName(*args, **kwargs)
    def DeleteProperty(*args, **kwargs): return _xrc.XmlNode_DeleteProperty(*args, **kwargs)
    def GetType(*args, **kwargs): return _xrc.XmlNode_GetType(*args, **kwargs)
    def GetName(*args, **kwargs): return _xrc.XmlNode_GetName(*args, **kwargs)
    def GetContent(*args, **kwargs): return _xrc.XmlNode_GetContent(*args, **kwargs)
    def GetParent(*args, **kwargs): return _xrc.XmlNode_GetParent(*args, **kwargs)
    def GetNext(*args, **kwargs): return _xrc.XmlNode_GetNext(*args, **kwargs)
    def GetChildren(*args, **kwargs): return _xrc.XmlNode_GetChildren(*args, **kwargs)
    def GetProperties(*args, **kwargs): return _xrc.XmlNode_GetProperties(*args, **kwargs)
    def GetPropVal(*args, **kwargs): return _xrc.XmlNode_GetPropVal(*args, **kwargs)
    def HasProp(*args, **kwargs): return _xrc.XmlNode_HasProp(*args, **kwargs)
    def SetType(*args, **kwargs): return _xrc.XmlNode_SetType(*args, **kwargs)
    def SetName(*args, **kwargs): return _xrc.XmlNode_SetName(*args, **kwargs)
    def SetContent(*args, **kwargs): return _xrc.XmlNode_SetContent(*args, **kwargs)
    def SetParent(*args, **kwargs): return _xrc.XmlNode_SetParent(*args, **kwargs)
    def SetNext(*args, **kwargs): return _xrc.XmlNode_SetNext(*args, **kwargs)
    def SetChildren(*args, **kwargs): return _xrc.XmlNode_SetChildren(*args, **kwargs)
    def SetProperties(*args, **kwargs): return _xrc.XmlNode_SetProperties(*args, **kwargs)
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxXmlNode instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class XmlNodePtr(XmlNode):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = XmlNode
_xrc.XmlNode_swigregister(XmlNodePtr)

def XmlNodeEasy(*args, **kwargs):
    val = _xrc.new_XmlNodeEasy(*args, **kwargs)
    val.thisown = 1
    return val

class XmlDocument(core.Object):
    def __init__(self, *args, **kwargs):
        newobj = _xrc.new_XmlDocument(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __del__(self, destroy=_xrc.delete_XmlDocument):
        try:
            if self.thisown: destroy(self)
        except: pass
    def Load(*args, **kwargs): return _xrc.XmlDocument_Load(*args, **kwargs)
    def LoadFromStream(*args, **kwargs): return _xrc.XmlDocument_LoadFromStream(*args, **kwargs)
    def Save(*args, **kwargs): return _xrc.XmlDocument_Save(*args, **kwargs)
    def SaveToStream(*args, **kwargs): return _xrc.XmlDocument_SaveToStream(*args, **kwargs)
    def IsOk(*args, **kwargs): return _xrc.XmlDocument_IsOk(*args, **kwargs)
    def GetRoot(*args, **kwargs): return _xrc.XmlDocument_GetRoot(*args, **kwargs)
    def GetVersion(*args, **kwargs): return _xrc.XmlDocument_GetVersion(*args, **kwargs)
    def GetFileEncoding(*args, **kwargs): return _xrc.XmlDocument_GetFileEncoding(*args, **kwargs)
    def SetRoot(*args, **kwargs): return _xrc.XmlDocument_SetRoot(*args, **kwargs)
    def SetVersion(*args, **kwargs): return _xrc.XmlDocument_SetVersion(*args, **kwargs)
    def SetFileEncoding(*args, **kwargs): return _xrc.XmlDocument_SetFileEncoding(*args, **kwargs)
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxXmlDocument instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class XmlDocumentPtr(XmlDocument):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = XmlDocument
_xrc.XmlDocument_swigregister(XmlDocumentPtr)

def XmlDocumentFromStream(*args, **kwargs):
    val = _xrc.new_XmlDocumentFromStream(*args, **kwargs)
    val.thisown = 1
    return val

def EmptyXmlDocument(*args, **kwargs):
    val = _xrc.new_EmptyXmlDocument(*args, **kwargs)
    val.thisown = 1
    return val

#---------------------------------------------------------------------------

class XmlResourceHandler(core.Object):
    def __init__(self, *args, **kwargs):
        newobj = _xrc.new_XmlResourceHandler(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setCallbackInfo(self, XmlResourceHandler)
    def _setCallbackInfo(*args, **kwargs): return _xrc.XmlResourceHandler__setCallbackInfo(*args, **kwargs)
    def CreateResource(*args, **kwargs): return _xrc.XmlResourceHandler_CreateResource(*args, **kwargs)
    def SetParentResource(*args, **kwargs): return _xrc.XmlResourceHandler_SetParentResource(*args, **kwargs)
    def GetResource(*args, **kwargs): return _xrc.XmlResourceHandler_GetResource(*args, **kwargs)
    def GetNode(*args, **kwargs): return _xrc.XmlResourceHandler_GetNode(*args, **kwargs)
    def GetClass(*args, **kwargs): return _xrc.XmlResourceHandler_GetClass(*args, **kwargs)
    def GetParent(*args, **kwargs): return _xrc.XmlResourceHandler_GetParent(*args, **kwargs)
    def GetInstance(*args, **kwargs): return _xrc.XmlResourceHandler_GetInstance(*args, **kwargs)
    def GetParentAsWindow(*args, **kwargs): return _xrc.XmlResourceHandler_GetParentAsWindow(*args, **kwargs)
    def GetInstanceAsWindow(*args, **kwargs): return _xrc.XmlResourceHandler_GetInstanceAsWindow(*args, **kwargs)
    def IsOfClass(*args, **kwargs): return _xrc.XmlResourceHandler_IsOfClass(*args, **kwargs)
    def GetNodeContent(*args, **kwargs): return _xrc.XmlResourceHandler_GetNodeContent(*args, **kwargs)
    def HasParam(*args, **kwargs): return _xrc.XmlResourceHandler_HasParam(*args, **kwargs)
    def GetParamNode(*args, **kwargs): return _xrc.XmlResourceHandler_GetParamNode(*args, **kwargs)
    def GetParamValue(*args, **kwargs): return _xrc.XmlResourceHandler_GetParamValue(*args, **kwargs)
    def AddStyle(*args, **kwargs): return _xrc.XmlResourceHandler_AddStyle(*args, **kwargs)
    def AddWindowStyles(*args, **kwargs): return _xrc.XmlResourceHandler_AddWindowStyles(*args, **kwargs)
    def GetStyle(*args, **kwargs): return _xrc.XmlResourceHandler_GetStyle(*args, **kwargs)
    def GetText(*args, **kwargs): return _xrc.XmlResourceHandler_GetText(*args, **kwargs)
    def GetID(*args, **kwargs): return _xrc.XmlResourceHandler_GetID(*args, **kwargs)
    def GetName(*args, **kwargs): return _xrc.XmlResourceHandler_GetName(*args, **kwargs)
    def GetBool(*args, **kwargs): return _xrc.XmlResourceHandler_GetBool(*args, **kwargs)
    def GetLong(*args, **kwargs): return _xrc.XmlResourceHandler_GetLong(*args, **kwargs)
    def GetColour(*args, **kwargs): return _xrc.XmlResourceHandler_GetColour(*args, **kwargs)
    def GetSize(*args, **kwargs): return _xrc.XmlResourceHandler_GetSize(*args, **kwargs)
    def GetPosition(*args, **kwargs): return _xrc.XmlResourceHandler_GetPosition(*args, **kwargs)
    def GetDimension(*args, **kwargs): return _xrc.XmlResourceHandler_GetDimension(*args, **kwargs)
    def GetBitmap(*args, **kwargs): return _xrc.XmlResourceHandler_GetBitmap(*args, **kwargs)
    def GetIcon(*args, **kwargs): return _xrc.XmlResourceHandler_GetIcon(*args, **kwargs)
    def GetFont(*args, **kwargs): return _xrc.XmlResourceHandler_GetFont(*args, **kwargs)
    def SetupWindow(*args, **kwargs): return _xrc.XmlResourceHandler_SetupWindow(*args, **kwargs)
    def CreateChildren(*args, **kwargs): return _xrc.XmlResourceHandler_CreateChildren(*args, **kwargs)
    def CreateChildrenPrivately(*args, **kwargs): return _xrc.XmlResourceHandler_CreateChildrenPrivately(*args, **kwargs)
    def CreateResFromNode(*args, **kwargs): return _xrc.XmlResourceHandler_CreateResFromNode(*args, **kwargs)
    def GetCurFileSystem(*args, **kwargs): return _xrc.XmlResourceHandler_GetCurFileSystem(*args, **kwargs)
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPyXmlResourceHandler instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class XmlResourceHandlerPtr(XmlResourceHandler):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = XmlResourceHandler
_xrc.XmlResourceHandler_swigregister(XmlResourceHandlerPtr)

#----------------------------------------------------------------------------
# The global was removed  in favor of static accessor functions.  This is for
# backwards compatibility:

wxTheXmlResource = wxXmlResource_Get()


#----------------------------------------------------------------------------
#  Create a factory for handling the subclass property of the object tag.


def _my_import(name):
    mod = __import__(name)
    components = name.split('.')
    for comp in components[1:]:
        mod = getattr(mod, comp)
    return mod


class wxXmlSubclassFactory_Python(wxXmlSubclassFactory):
    def __init__(self):
        wxXmlSubclassFactory.__init__(self)

    def Create(self, className):
        assert className.find('.') != -1, "Module name must be specified!"
        mname = className[:className.rfind('.')]
        cname = className[className.rfind('.')+1:]
        module = _my_import(mname)
        klass = getattr(module, cname)
        inst = klass()
        return inst


wxXmlResource_AddSubclassFactory(wxXmlSubclassFactory_Python())

#----------------------------------------------------------------------------


