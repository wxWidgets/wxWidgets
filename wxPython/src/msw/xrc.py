# This file was created automatically by SWIG 1.3.29.
# Don't modify this file, modify the SWIG interface instead.

"""
The `XmlResource` class allows program resources defining menus, layout of
controls on a panel, etc. to be loaded from an XML file.
"""

import _xrc
import new
new_instancemethod = new.instancemethod
def _swig_setattr_nondynamic(self,class_type,name,value,static=1):
    if (name == "thisown"): return self.this.own(value)
    if (name == "this"):
        if type(value).__name__ == 'PySwigObject':
            self.__dict__[name] = value
            return
    method = class_type.__swig_setmethods__.get(name,None)
    if method: return method(self,value)
    if (not static) or hasattr(self,name):
        self.__dict__[name] = value
    else:
        raise AttributeError("You cannot add attributes to %s" % self)

def _swig_setattr(self,class_type,name,value):
    return _swig_setattr_nondynamic(self,class_type,name,value,0)

def _swig_getattr(self,class_type,name):
    if (name == "thisown"): return self.this.own()
    method = class_type.__swig_getmethods__.get(name,None)
    if method: return method(self)
    raise AttributeError,name

def _swig_repr(self):
    try: strthis = "proxy of " + self.this.__repr__()
    except: strthis = ""
    return "<%s.%s; %s >" % (self.__class__.__module__, self.__class__.__name__, strthis,)

import types
try:
    _object = types.ObjectType
    _newclass = 1
except AttributeError:
    class _object : pass
    _newclass = 0
del types


def _swig_setattr_nondynamic_method(set):
    def set_attr(self,name,value):
        if (name == "thisown"): return self.this.own(value)
        if hasattr(self,name) or (name == "this"):
            set(self,name,value)
        else:
            raise AttributeError("You cannot add attributes to %s" % self)
    return set_attr


import _core
wx = _core 
__docfilter__ = wx.__DocFilter(globals()) 
#---------------------------------------------------------------------------

WX_XMLRES_CURRENT_VERSION_MAJOR = _xrc.WX_XMLRES_CURRENT_VERSION_MAJOR
WX_XMLRES_CURRENT_VERSION_MINOR = _xrc.WX_XMLRES_CURRENT_VERSION_MINOR
WX_XMLRES_CURRENT_VERSION_RELEASE = _xrc.WX_XMLRES_CURRENT_VERSION_RELEASE
WX_XMLRES_CURRENT_VERSION_REVISION = _xrc.WX_XMLRES_CURRENT_VERSION_REVISION
XRC_USE_LOCALE = _xrc.XRC_USE_LOCALE
XRC_NO_SUBCLASSING = _xrc.XRC_NO_SUBCLASSING
XRC_NO_RELOADING = _xrc.XRC_NO_RELOADING
class XmlResource(_core.Object):
    """Proxy of C++ XmlResource class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """__init__(self, String filemask, int flags=XRC_USE_LOCALE, String domain=wxEmptyString) -> XmlResource"""
        _xrc.XmlResource_swiginit(self,_xrc.new_XmlResource(*args, **kwargs))
        self.InitAllHandlers()

    __swig_destroy__ = _xrc.delete_XmlResource
    __del__ = lambda self : None;
    def GetFirstRoot(*args, **kwargs):
        """GetFirstRoot(self) -> XmlNode"""
        return _xrc.XmlResource_GetFirstRoot(*args, **kwargs)

    def Load(*args, **kwargs):
        """Load(self, String filemask) -> bool"""
        return _xrc.XmlResource_Load(*args, **kwargs)

    def LoadFromString(*args, **kwargs):
        """LoadFromString(self, String data) -> bool"""
        return _xrc.XmlResource_LoadFromString(*args, **kwargs)

    def Unload(*args, **kwargs):
        """Unload(self, String filename) -> bool"""
        return _xrc.XmlResource_Unload(*args, **kwargs)

    def InitAllHandlers(*args, **kwargs):
        """InitAllHandlers(self)"""
        return _xrc.XmlResource_InitAllHandlers(*args, **kwargs)

    def AddHandler(*args, **kwargs):
        """AddHandler(self, XmlResourceHandler handler)"""
        return _xrc.XmlResource_AddHandler(*args, **kwargs)

    def InsertHandler(*args, **kwargs):
        """InsertHandler(self, XmlResourceHandler handler)"""
        return _xrc.XmlResource_InsertHandler(*args, **kwargs)

    def ClearHandlers(*args, **kwargs):
        """ClearHandlers(self)"""
        return _xrc.XmlResource_ClearHandlers(*args, **kwargs)

    def AddSubclassFactory(*args, **kwargs):
        """AddSubclassFactory(XmlSubclassFactory factory)"""
        return _xrc.XmlResource_AddSubclassFactory(*args, **kwargs)

    AddSubclassFactory = staticmethod(AddSubclassFactory)
    def LoadMenu(*args, **kwargs):
        """LoadMenu(self, String name) -> Menu"""
        return _xrc.XmlResource_LoadMenu(*args, **kwargs)

    def LoadMenuBar(*args, **kwargs):
        """LoadMenuBar(self, String name) -> MenuBar"""
        return _xrc.XmlResource_LoadMenuBar(*args, **kwargs)

    def LoadMenuBarOnFrame(*args, **kwargs):
        """LoadMenuBarOnFrame(self, Window parent, String name) -> MenuBar"""
        return _xrc.XmlResource_LoadMenuBarOnFrame(*args, **kwargs)

    def LoadToolBar(*args, **kwargs):
        """LoadToolBar(self, Window parent, String name) -> wxToolBar"""
        return _xrc.XmlResource_LoadToolBar(*args, **kwargs)

    def LoadDialog(*args, **kwargs):
        """LoadDialog(self, Window parent, String name) -> wxDialog"""
        return _xrc.XmlResource_LoadDialog(*args, **kwargs)

    def LoadOnDialog(*args, **kwargs):
        """LoadOnDialog(self, wxDialog dlg, Window parent, String name) -> bool"""
        return _xrc.XmlResource_LoadOnDialog(*args, **kwargs)

    def LoadPanel(*args, **kwargs):
        """LoadPanel(self, Window parent, String name) -> wxPanel"""
        return _xrc.XmlResource_LoadPanel(*args, **kwargs)

    def LoadOnPanel(*args, **kwargs):
        """LoadOnPanel(self, wxPanel panel, Window parent, String name) -> bool"""
        return _xrc.XmlResource_LoadOnPanel(*args, **kwargs)

    def LoadFrame(*args, **kwargs):
        """LoadFrame(self, Window parent, String name) -> wxFrame"""
        return _xrc.XmlResource_LoadFrame(*args, **kwargs)

    def LoadOnFrame(*args, **kwargs):
        """LoadOnFrame(self, wxFrame frame, Window parent, String name) -> bool"""
        return _xrc.XmlResource_LoadOnFrame(*args, **kwargs)

    def LoadObject(*args, **kwargs):
        """LoadObject(self, Window parent, String name, String classname) -> Object"""
        return _xrc.XmlResource_LoadObject(*args, **kwargs)

    def LoadOnObject(*args, **kwargs):
        """LoadOnObject(self, Object instance, Window parent, String name, String classname) -> bool"""
        return _xrc.XmlResource_LoadOnObject(*args, **kwargs)

    def LoadBitmap(*args, **kwargs):
        """LoadBitmap(self, String name) -> Bitmap"""
        return _xrc.XmlResource_LoadBitmap(*args, **kwargs)

    def LoadIcon(*args, **kwargs):
        """LoadIcon(self, String name) -> Icon"""
        return _xrc.XmlResource_LoadIcon(*args, **kwargs)

    def AttachUnknownControl(*args, **kwargs):
        """AttachUnknownControl(self, String name, Window control, Window parent=None) -> bool"""
        return _xrc.XmlResource_AttachUnknownControl(*args, **kwargs)

    def GetXRCID(*args, **kwargs):
        """GetXRCID(String str_id, int value_if_not_found=ID_NONE) -> int"""
        return _xrc.XmlResource_GetXRCID(*args, **kwargs)

    GetXRCID = staticmethod(GetXRCID)
    def GetVersion(*args, **kwargs):
        """GetVersion(self) -> long"""
        return _xrc.XmlResource_GetVersion(*args, **kwargs)

    def CompareVersion(*args, **kwargs):
        """CompareVersion(self, int major, int minor, int release, int revision) -> int"""
        return _xrc.XmlResource_CompareVersion(*args, **kwargs)

    def Get(*args, **kwargs):
        """Get() -> XmlResource"""
        return _xrc.XmlResource_Get(*args, **kwargs)

    Get = staticmethod(Get)
    def Set(*args, **kwargs):
        """Set(XmlResource res) -> XmlResource"""
        return _xrc.XmlResource_Set(*args, **kwargs)

    Set = staticmethod(Set)
    def GetFlags(*args, **kwargs):
        """GetFlags(self) -> int"""
        return _xrc.XmlResource_GetFlags(*args, **kwargs)

    def SetFlags(*args, **kwargs):
        """SetFlags(self, int flags)"""
        return _xrc.XmlResource_SetFlags(*args, **kwargs)

    def GetDomain(*args, **kwargs):
        """GetDomain(self) -> String"""
        return _xrc.XmlResource_GetDomain(*args, **kwargs)

    def SetDomain(*args, **kwargs):
        """SetDomain(self, String domain)"""
        return _xrc.XmlResource_SetDomain(*args, **kwargs)

    Domain = property(GetDomain,SetDomain,doc="See `GetDomain` and `SetDomain`") 
    Flags = property(GetFlags,SetFlags,doc="See `GetFlags` and `SetFlags`") 
    Version = property(GetVersion,doc="See `GetVersion`") 
_xrc.XmlResource_swigregister(XmlResource)
cvar = _xrc.cvar
UTF8String = cvar.UTF8String
StyleString = cvar.StyleString
SizeString = cvar.SizeString
PosString = cvar.PosString
BitmapString = cvar.BitmapString
IconString = cvar.IconString
FontString = cvar.FontString
AnimationString = cvar.AnimationString

def EmptyXmlResource(*args, **kwargs):
    """EmptyXmlResource(int flags=XRC_USE_LOCALE, String domain=wxEmptyString) -> XmlResource"""
    val = _xrc.new_EmptyXmlResource(*args, **kwargs)
    val.InitAllHandlers()
    return val

def XmlResource_AddSubclassFactory(*args, **kwargs):
  """XmlResource_AddSubclassFactory(XmlSubclassFactory factory)"""
  return _xrc.XmlResource_AddSubclassFactory(*args, **kwargs)

def XmlResource_GetXRCID(*args, **kwargs):
  """XmlResource_GetXRCID(String str_id, int value_if_not_found=ID_NONE) -> int"""
  return _xrc.XmlResource_GetXRCID(*args, **kwargs)

def XmlResource_Get(*args):
  """XmlResource_Get() -> XmlResource"""
  return _xrc.XmlResource_Get(*args)

def XmlResource_Set(*args, **kwargs):
  """XmlResource_Set(XmlResource res) -> XmlResource"""
  return _xrc.XmlResource_Set(*args, **kwargs)

def XRCID(str_id, value_if_not_found = wx.ID_NONE):
    return XmlResource_GetXRCID(str_id, value_if_not_found)

def XRCCTRL(window, str_id, *ignoreargs):
    return window.FindWindowById(XRCID(str_id))

#---------------------------------------------------------------------------

class XmlSubclassFactory(object):
    """Proxy of C++ XmlSubclassFactory class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """__init__(self) -> XmlSubclassFactory"""
        _xrc.XmlSubclassFactory_swiginit(self,_xrc.new_XmlSubclassFactory(*args, **kwargs))
        self._setCallbackInfo(self, XmlSubclassFactory)

    def _setCallbackInfo(*args, **kwargs):
        """_setCallbackInfo(self, PyObject self, PyObject _class)"""
        return _xrc.XmlSubclassFactory__setCallbackInfo(*args, **kwargs)

_xrc.XmlSubclassFactory_swigregister(XmlSubclassFactory)

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
    """Proxy of C++ XmlProperty class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, String name=EmptyString, String value=EmptyString, 
            XmlProperty next=None) -> XmlProperty
        """
        _xrc.XmlProperty_swiginit(self,_xrc.new_XmlProperty(*args, **kwargs))
    def GetName(*args, **kwargs):
        """GetName(self) -> String"""
        return _xrc.XmlProperty_GetName(*args, **kwargs)

    def GetValue(*args, **kwargs):
        """GetValue(self) -> String"""
        return _xrc.XmlProperty_GetValue(*args, **kwargs)

    def GetNext(*args, **kwargs):
        """GetNext(self) -> XmlProperty"""
        return _xrc.XmlProperty_GetNext(*args, **kwargs)

    def SetName(*args, **kwargs):
        """SetName(self, String name)"""
        return _xrc.XmlProperty_SetName(*args, **kwargs)

    def SetValue(*args, **kwargs):
        """SetValue(self, String value)"""
        return _xrc.XmlProperty_SetValue(*args, **kwargs)

    def SetNext(*args, **kwargs):
        """SetNext(self, XmlProperty next)"""
        return _xrc.XmlProperty_SetNext(*args, **kwargs)

    Name = property(GetName,SetName,doc="See `GetName` and `SetName`") 
    Next = property(GetNext,SetNext,doc="See `GetNext` and `SetNext`") 
    Value = property(GetValue,SetValue,doc="See `GetValue` and `SetValue`") 
_xrc.XmlProperty_swigregister(XmlProperty)

class XmlNode(object):
    """Proxy of C++ XmlNode class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, XmlNode parent=None, int type=0, String name=EmptyString, 
            String content=EmptyString, XmlProperty props=None, 
            XmlNode next=None) -> XmlNode
        """
        _xrc.XmlNode_swiginit(self,_xrc.new_XmlNode(*args, **kwargs))
    __swig_destroy__ = _xrc.delete_XmlNode
    __del__ = lambda self : None;
    def AddChild(*args, **kwargs):
        """AddChild(self, XmlNode child)"""
        return _xrc.XmlNode_AddChild(*args, **kwargs)

    def InsertChild(*args, **kwargs):
        """InsertChild(self, XmlNode child, XmlNode before_node) -> bool"""
        return _xrc.XmlNode_InsertChild(*args, **kwargs)

    def RemoveChild(*args, **kwargs):
        """RemoveChild(self, XmlNode child) -> bool"""
        return _xrc.XmlNode_RemoveChild(*args, **kwargs)

    def AddProperty(*args, **kwargs):
        """AddProperty(self, XmlProperty prop)"""
        return _xrc.XmlNode_AddProperty(*args, **kwargs)

    def AddPropertyName(*args, **kwargs):
        """AddPropertyName(self, String name, String value)"""
        return _xrc.XmlNode_AddPropertyName(*args, **kwargs)

    def DeleteProperty(*args, **kwargs):
        """DeleteProperty(self, String name) -> bool"""
        return _xrc.XmlNode_DeleteProperty(*args, **kwargs)

    def GetType(*args, **kwargs):
        """GetType(self) -> int"""
        return _xrc.XmlNode_GetType(*args, **kwargs)

    def GetName(*args, **kwargs):
        """GetName(self) -> String"""
        return _xrc.XmlNode_GetName(*args, **kwargs)

    def GetContent(*args, **kwargs):
        """GetContent(self) -> String"""
        return _xrc.XmlNode_GetContent(*args, **kwargs)

    def IsWhitespaceOnly(*args, **kwargs):
        """IsWhitespaceOnly(self) -> bool"""
        return _xrc.XmlNode_IsWhitespaceOnly(*args, **kwargs)

    def GetDepth(*args, **kwargs):
        """GetDepth(self, XmlNode grandparent=None) -> int"""
        return _xrc.XmlNode_GetDepth(*args, **kwargs)

    def GetNodeContent(*args, **kwargs):
        """GetNodeContent(self) -> String"""
        return _xrc.XmlNode_GetNodeContent(*args, **kwargs)

    def GetParent(*args, **kwargs):
        """GetParent(self) -> XmlNode"""
        return _xrc.XmlNode_GetParent(*args, **kwargs)

    def GetNext(*args, **kwargs):
        """GetNext(self) -> XmlNode"""
        return _xrc.XmlNode_GetNext(*args, **kwargs)

    def GetChildren(*args, **kwargs):
        """GetChildren(self) -> XmlNode"""
        return _xrc.XmlNode_GetChildren(*args, **kwargs)

    def GetProperties(*args, **kwargs):
        """GetProperties(self) -> XmlProperty"""
        return _xrc.XmlNode_GetProperties(*args, **kwargs)

    def GetPropVal(*args, **kwargs):
        """GetPropVal(self, String propName, String defaultVal) -> String"""
        return _xrc.XmlNode_GetPropVal(*args, **kwargs)

    def HasProp(*args, **kwargs):
        """HasProp(self, String propName) -> bool"""
        return _xrc.XmlNode_HasProp(*args, **kwargs)

    def SetType(*args, **kwargs):
        """SetType(self, int type)"""
        return _xrc.XmlNode_SetType(*args, **kwargs)

    def SetName(*args, **kwargs):
        """SetName(self, String name)"""
        return _xrc.XmlNode_SetName(*args, **kwargs)

    def SetContent(*args, **kwargs):
        """SetContent(self, String con)"""
        return _xrc.XmlNode_SetContent(*args, **kwargs)

    def SetParent(*args, **kwargs):
        """SetParent(self, XmlNode parent)"""
        return _xrc.XmlNode_SetParent(*args, **kwargs)

    def SetNext(*args, **kwargs):
        """SetNext(self, XmlNode next)"""
        return _xrc.XmlNode_SetNext(*args, **kwargs)

    def SetChildren(*args, **kwargs):
        """SetChildren(self, XmlNode child)"""
        return _xrc.XmlNode_SetChildren(*args, **kwargs)

    def SetProperties(*args, **kwargs):
        """SetProperties(self, XmlProperty prop)"""
        return _xrc.XmlNode_SetProperties(*args, **kwargs)

    Children = property(GetChildren,SetChildren,doc="See `GetChildren` and `SetChildren`") 
    Content = property(GetContent,SetContent,doc="See `GetContent` and `SetContent`") 
    Name = property(GetName,SetName,doc="See `GetName` and `SetName`") 
    Next = property(GetNext,SetNext,doc="See `GetNext` and `SetNext`") 
    Parent = property(GetParent,SetParent,doc="See `GetParent` and `SetParent`") 
    Properties = property(GetProperties,SetProperties,doc="See `GetProperties` and `SetProperties`") 
    Type = property(GetType,SetType,doc="See `GetType` and `SetType`") 
_xrc.XmlNode_swigregister(XmlNode)

def XmlNodeEasy(*args, **kwargs):
    """XmlNodeEasy(int type, String name, String content=EmptyString) -> XmlNode"""
    val = _xrc.new_XmlNodeEasy(*args, **kwargs)
    return val

XML_NO_INDENTATION = _xrc.XML_NO_INDENTATION
XMLDOC_NONE = _xrc.XMLDOC_NONE
XMLDOC_KEEP_WHITESPACE_NODES = _xrc.XMLDOC_KEEP_WHITESPACE_NODES
class XmlDocument(_core.Object):
    """Proxy of C++ XmlDocument class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """__init__(self, String filename, String encoding=UTF8String) -> XmlDocument"""
        _xrc.XmlDocument_swiginit(self,_xrc.new_XmlDocument(*args, **kwargs))
    __swig_destroy__ = _xrc.delete_XmlDocument
    __del__ = lambda self : None;
    def Load(*args, **kwargs):
        """Load(self, String filename, String encoding=UTF8String, int flags=XMLDOC_NONE) -> bool"""
        return _xrc.XmlDocument_Load(*args, **kwargs)

    def LoadFromStream(*args, **kwargs):
        """LoadFromStream(self, InputStream stream, String encoding=UTF8String, int flags=XMLDOC_NONE) -> bool"""
        return _xrc.XmlDocument_LoadFromStream(*args, **kwargs)

    def Save(*args, **kwargs):
        """Save(self, String filename, int indentstep=1) -> bool"""
        return _xrc.XmlDocument_Save(*args, **kwargs)

    def SaveToStream(*args, **kwargs):
        """SaveToStream(self, OutputStream stream, int indentstep=1) -> bool"""
        return _xrc.XmlDocument_SaveToStream(*args, **kwargs)

    def IsOk(*args, **kwargs):
        """IsOk(self) -> bool"""
        return _xrc.XmlDocument_IsOk(*args, **kwargs)

    def GetRoot(*args, **kwargs):
        """GetRoot(self) -> XmlNode"""
        return _xrc.XmlDocument_GetRoot(*args, **kwargs)

    def GetVersion(*args, **kwargs):
        """GetVersion(self) -> String"""
        return _xrc.XmlDocument_GetVersion(*args, **kwargs)

    def GetFileEncoding(*args, **kwargs):
        """GetFileEncoding(self) -> String"""
        return _xrc.XmlDocument_GetFileEncoding(*args, **kwargs)

    def DetachRoot(*args, **kwargs):
        """DetachRoot(self) -> XmlNode"""
        return _xrc.XmlDocument_DetachRoot(*args, **kwargs)

    def SetRoot(*args, **kwargs):
        """SetRoot(self, XmlNode node)"""
        return _xrc.XmlDocument_SetRoot(*args, **kwargs)

    def SetVersion(*args, **kwargs):
        """SetVersion(self, String version)"""
        return _xrc.XmlDocument_SetVersion(*args, **kwargs)

    def SetFileEncoding(*args, **kwargs):
        """SetFileEncoding(self, String encoding)"""
        return _xrc.XmlDocument_SetFileEncoding(*args, **kwargs)

    FileEncoding = property(GetFileEncoding,SetFileEncoding,doc="See `GetFileEncoding` and `SetFileEncoding`") 
    Root = property(GetRoot,SetRoot,doc="See `GetRoot` and `SetRoot`") 
    Version = property(GetVersion,SetVersion,doc="See `GetVersion` and `SetVersion`") 
_xrc.XmlDocument_swigregister(XmlDocument)

def XmlDocumentFromStream(*args, **kwargs):
    """XmlDocumentFromStream(InputStream stream, String encoding=UTF8String) -> XmlDocument"""
    val = _xrc.new_XmlDocumentFromStream(*args, **kwargs)
    return val

def EmptyXmlDocument(*args, **kwargs):
    """EmptyXmlDocument() -> XmlDocument"""
    val = _xrc.new_EmptyXmlDocument(*args, **kwargs)
    return val

#---------------------------------------------------------------------------

class XmlResourceHandler(_core.Object):
    """Proxy of C++ XmlResourceHandler class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """__init__(self) -> XmlResourceHandler"""
        _xrc.XmlResourceHandler_swiginit(self,_xrc.new_XmlResourceHandler(*args, **kwargs))
        self._setCallbackInfo(self, XmlResourceHandler)

    __swig_destroy__ = _xrc.delete_XmlResourceHandler
    __del__ = lambda self : None;
    def _setCallbackInfo(*args, **kwargs):
        """_setCallbackInfo(self, PyObject self, PyObject _class)"""
        return _xrc.XmlResourceHandler__setCallbackInfo(*args, **kwargs)

    def CreateResource(*args, **kwargs):
        """CreateResource(self, XmlNode node, Object parent, Object instance) -> Object"""
        return _xrc.XmlResourceHandler_CreateResource(*args, **kwargs)

    def SetParentResource(*args, **kwargs):
        """SetParentResource(self, XmlResource res)"""
        return _xrc.XmlResourceHandler_SetParentResource(*args, **kwargs)

    def GetResource(*args, **kwargs):
        """GetResource(self) -> XmlResource"""
        return _xrc.XmlResourceHandler_GetResource(*args, **kwargs)

    def GetNode(*args, **kwargs):
        """GetNode(self) -> XmlNode"""
        return _xrc.XmlResourceHandler_GetNode(*args, **kwargs)

    def GetClass(*args, **kwargs):
        """GetClass(self) -> String"""
        return _xrc.XmlResourceHandler_GetClass(*args, **kwargs)

    def GetParent(*args, **kwargs):
        """GetParent(self) -> Object"""
        return _xrc.XmlResourceHandler_GetParent(*args, **kwargs)

    def GetInstance(*args, **kwargs):
        """GetInstance(self) -> Object"""
        return _xrc.XmlResourceHandler_GetInstance(*args, **kwargs)

    def GetParentAsWindow(*args, **kwargs):
        """GetParentAsWindow(self) -> Window"""
        return _xrc.XmlResourceHandler_GetParentAsWindow(*args, **kwargs)

    def IsOfClass(*args, **kwargs):
        """IsOfClass(self, XmlNode node, String classname) -> bool"""
        return _xrc.XmlResourceHandler_IsOfClass(*args, **kwargs)

    def GetNodeContent(*args, **kwargs):
        """GetNodeContent(self, XmlNode node) -> String"""
        return _xrc.XmlResourceHandler_GetNodeContent(*args, **kwargs)

    def HasParam(*args, **kwargs):
        """HasParam(self, String param) -> bool"""
        return _xrc.XmlResourceHandler_HasParam(*args, **kwargs)

    def GetParamNode(*args, **kwargs):
        """GetParamNode(self, String param) -> XmlNode"""
        return _xrc.XmlResourceHandler_GetParamNode(*args, **kwargs)

    def GetParamValue(*args, **kwargs):
        """GetParamValue(self, String param) -> String"""
        return _xrc.XmlResourceHandler_GetParamValue(*args, **kwargs)

    def AddStyle(*args, **kwargs):
        """AddStyle(self, String name, int value)"""
        return _xrc.XmlResourceHandler_AddStyle(*args, **kwargs)

    def AddWindowStyles(*args, **kwargs):
        """AddWindowStyles(self)"""
        return _xrc.XmlResourceHandler_AddWindowStyles(*args, **kwargs)

    def GetStyle(*args, **kwargs):
        """GetStyle(self, String param=StyleString, int defaults=0) -> int"""
        return _xrc.XmlResourceHandler_GetStyle(*args, **kwargs)

    def GetText(*args, **kwargs):
        """GetText(self, String param, bool translate=True) -> String"""
        return _xrc.XmlResourceHandler_GetText(*args, **kwargs)

    def GetID(*args, **kwargs):
        """GetID(self) -> int"""
        return _xrc.XmlResourceHandler_GetID(*args, **kwargs)

    def GetName(*args, **kwargs):
        """GetName(self) -> String"""
        return _xrc.XmlResourceHandler_GetName(*args, **kwargs)

    def GetBool(*args, **kwargs):
        """GetBool(self, String param, bool defaultv=False) -> bool"""
        return _xrc.XmlResourceHandler_GetBool(*args, **kwargs)

    def GetLong(*args, **kwargs):
        """GetLong(self, String param, long defaultv=0) -> long"""
        return _xrc.XmlResourceHandler_GetLong(*args, **kwargs)

    def GetColour(*args, **kwargs):
        """GetColour(self, String param) -> Colour"""
        return _xrc.XmlResourceHandler_GetColour(*args, **kwargs)

    def GetSize(*args, **kwargs):
        """GetSize(self, String param=SizeString) -> Size"""
        return _xrc.XmlResourceHandler_GetSize(*args, **kwargs)

    def GetPosition(*args, **kwargs):
        """GetPosition(self, String param=PosString) -> Point"""
        return _xrc.XmlResourceHandler_GetPosition(*args, **kwargs)

    def GetDimension(*args, **kwargs):
        """GetDimension(self, String param, int defaultv=0) -> int"""
        return _xrc.XmlResourceHandler_GetDimension(*args, **kwargs)

    def GetBitmap(*args, **kwargs):
        """
        GetBitmap(self, String param=BitmapString, wxArtClient defaultArtClient=wxART_OTHER, 
            Size size=DefaultSize) -> Bitmap
        """
        return _xrc.XmlResourceHandler_GetBitmap(*args, **kwargs)

    def GetIcon(*args, **kwargs):
        """
        GetIcon(self, String param=IconString, wxArtClient defaultArtClient=wxART_OTHER, 
            Size size=DefaultSize) -> Icon
        """
        return _xrc.XmlResourceHandler_GetIcon(*args, **kwargs)

    def GetFont(*args, **kwargs):
        """GetFont(self, String param=FontString) -> Font"""
        return _xrc.XmlResourceHandler_GetFont(*args, **kwargs)

    def GetAnimation(*args, **kwargs):
        """GetAnimation(self, String param=AnimationString) -> wxAnimation"""
        return _xrc.XmlResourceHandler_GetAnimation(*args, **kwargs)

    def SetupWindow(*args, **kwargs):
        """SetupWindow(self, Window wnd)"""
        return _xrc.XmlResourceHandler_SetupWindow(*args, **kwargs)

    def CreateChildren(*args, **kwargs):
        """CreateChildren(self, Object parent, bool this_hnd_only=False)"""
        return _xrc.XmlResourceHandler_CreateChildren(*args, **kwargs)

    def CreateChildrenPrivately(*args, **kwargs):
        """CreateChildrenPrivately(self, Object parent, XmlNode rootnode=None)"""
        return _xrc.XmlResourceHandler_CreateChildrenPrivately(*args, **kwargs)

    def CreateResFromNode(*args, **kwargs):
        """CreateResFromNode(self, XmlNode node, Object parent, Object instance=None) -> Object"""
        return _xrc.XmlResourceHandler_CreateResFromNode(*args, **kwargs)

    def GetCurFileSystem(*args, **kwargs):
        """GetCurFileSystem(self) -> FileSystem"""
        return _xrc.XmlResourceHandler_GetCurFileSystem(*args, **kwargs)

    Class = property(GetClass,doc="See `GetClass`") 
    CurFileSystem = property(GetCurFileSystem,doc="See `GetCurFileSystem`") 
    ID = property(GetID,doc="See `GetID`") 
    Instance = property(GetInstance,doc="See `GetInstance`") 
    Name = property(GetName,doc="See `GetName`") 
    Node = property(GetNode,doc="See `GetNode`") 
    Parent = property(GetParent,doc="See `GetParent`") 
    ParentAsWindow = property(GetParentAsWindow,doc="See `GetParentAsWindow`") 
    Resource = property(GetResource,doc="See `GetResource`") 
_xrc.XmlResourceHandler_swigregister(XmlResourceHandler)

#----------------------------------------------------------------------------
# The global was removed  in favor of static accessor functions.  This is for
# backwards compatibility:

TheXmlResource = XmlResource_Get()


#----------------------------------------------------------------------------
#  Create a factory for handling the subclass property of the object tag.


def _my_import(name):
    mod = __import__(name)
    components = name.split('.')
    for comp in components[1:]:
        mod = getattr(mod, comp)
    return mod


class XmlSubclassFactory_Python(XmlSubclassFactory):
    def __init__(self):
        XmlSubclassFactory.__init__(self)

    def Create(self, className):
        assert className.find('.') != -1, "Module name must be specified!"
        mname = className[:className.rfind('.')]
        cname = className[className.rfind('.')+1:]
        module = _my_import(mname)
        klass = getattr(module, cname)
        inst = klass()
        return inst


XmlResource_AddSubclassFactory(XmlSubclassFactory_Python())

#----------------------------------------------------------------------------



