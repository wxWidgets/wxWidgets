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
    """"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxXmlResource instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(wxString filemask, int flags=XRC_USE_LOCALE) -> XmlResource"""
        newobj = _xrc.new_XmlResource(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self.InitAllHandlers()

    def __del__(self, destroy=_xrc.delete_XmlResource):
        """__del__()"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def Load(*args, **kwargs):
        """Load(wxString filemask) -> bool"""
        return _xrc.XmlResource_Load(*args, **kwargs)

    def LoadFromString(*args, **kwargs):
        """LoadFromString(wxString data) -> bool"""
        return _xrc.XmlResource_LoadFromString(*args, **kwargs)

    def InitAllHandlers(*args, **kwargs):
        """InitAllHandlers()"""
        return _xrc.XmlResource_InitAllHandlers(*args, **kwargs)

    def AddHandler(*args, **kwargs):
        """AddHandler(XmlResourceHandler handler)"""
        return _xrc.XmlResource_AddHandler(*args, **kwargs)

    def InsertHandler(*args, **kwargs):
        """InsertHandler(XmlResourceHandler handler)"""
        return _xrc.XmlResource_InsertHandler(*args, **kwargs)

    def ClearHandlers(*args, **kwargs):
        """ClearHandlers()"""
        return _xrc.XmlResource_ClearHandlers(*args, **kwargs)

    def AddSubclassFactory(*args, **kwargs):
        """XmlResource.AddSubclassFactory(XmlSubclassFactory factory)"""
        return _xrc.XmlResource_AddSubclassFactory(*args, **kwargs)

    AddSubclassFactory = staticmethod(AddSubclassFactory)
    def LoadMenu(*args, **kwargs):
        """LoadMenu(wxString name) -> Menu"""
        return _xrc.XmlResource_LoadMenu(*args, **kwargs)

    def LoadMenuBar(*args, **kwargs):
        """LoadMenuBar(wxString name) -> MenuBar"""
        return _xrc.XmlResource_LoadMenuBar(*args, **kwargs)

    def LoadMenuBarOnFrame(*args, **kwargs):
        """LoadMenuBarOnFrame(Window parent, wxString name) -> MenuBar"""
        return _xrc.XmlResource_LoadMenuBarOnFrame(*args, **kwargs)

    def LoadToolBar(*args, **kwargs):
        """LoadToolBar(Window parent, wxString name) -> wxToolBar"""
        return _xrc.XmlResource_LoadToolBar(*args, **kwargs)

    def LoadDialog(*args, **kwargs):
        """LoadDialog(Window parent, wxString name) -> wxDialog"""
        return _xrc.XmlResource_LoadDialog(*args, **kwargs)

    def LoadOnDialog(*args, **kwargs):
        """LoadOnDialog(wxDialog dlg, Window parent, wxString name) -> bool"""
        return _xrc.XmlResource_LoadOnDialog(*args, **kwargs)

    def LoadPanel(*args, **kwargs):
        """LoadPanel(Window parent, wxString name) -> wxPanel"""
        return _xrc.XmlResource_LoadPanel(*args, **kwargs)

    def LoadOnPanel(*args, **kwargs):
        """LoadOnPanel(wxPanel panel, Window parent, wxString name) -> bool"""
        return _xrc.XmlResource_LoadOnPanel(*args, **kwargs)

    def LoadFrame(*args, **kwargs):
        """LoadFrame(Window parent, wxString name) -> wxFrame"""
        return _xrc.XmlResource_LoadFrame(*args, **kwargs)

    def LoadOnFrame(*args, **kwargs):
        """LoadOnFrame(wxFrame frame, Window parent, wxString name) -> bool"""
        return _xrc.XmlResource_LoadOnFrame(*args, **kwargs)

    def LoadObject(*args, **kwargs):
        """LoadObject(Window parent, wxString name, wxString classname) -> Object"""
        return _xrc.XmlResource_LoadObject(*args, **kwargs)

    def LoadOnObject(*args, **kwargs):
        """LoadOnObject(Object instance, Window parent, wxString name, wxString classname) -> bool"""
        return _xrc.XmlResource_LoadOnObject(*args, **kwargs)

    def LoadBitmap(*args, **kwargs):
        """LoadBitmap(wxString name) -> wxBitmap"""
        return _xrc.XmlResource_LoadBitmap(*args, **kwargs)

    def LoadIcon(*args, **kwargs):
        """LoadIcon(wxString name) -> wxIcon"""
        return _xrc.XmlResource_LoadIcon(*args, **kwargs)

    def AttachUnknownControl(*args, **kwargs):
        """AttachUnknownControl(wxString name, Window control, Window parent=None) -> bool"""
        return _xrc.XmlResource_AttachUnknownControl(*args, **kwargs)

    def GetXRCID(*args, **kwargs):
        """XmlResource.GetXRCID(wxString str_id) -> int"""
        return _xrc.XmlResource_GetXRCID(*args, **kwargs)

    GetXRCID = staticmethod(GetXRCID)
    def GetVersion(*args, **kwargs):
        """GetVersion() -> long"""
        return _xrc.XmlResource_GetVersion(*args, **kwargs)

    def CompareVersion(*args, **kwargs):
        """CompareVersion(int major, int minor, int release, int revision) -> int"""
        return _xrc.XmlResource_CompareVersion(*args, **kwargs)

    def Get(*args, **kwargs):
        """XmlResource.Get() -> XmlResource"""
        return _xrc.XmlResource_Get(*args, **kwargs)

    Get = staticmethod(Get)
    def Set(*args, **kwargs):
        """XmlResource.Set(XmlResource res) -> XmlResource"""
        return _xrc.XmlResource_Set(*args, **kwargs)

    Set = staticmethod(Set)
    def GetFlags(*args, **kwargs):
        """GetFlags() -> int"""
        return _xrc.XmlResource_GetFlags(*args, **kwargs)

    def SetFlags(*args, **kwargs):
        """SetFlags(int flags)"""
        return _xrc.XmlResource_SetFlags(*args, **kwargs)


class XmlResourcePtr(XmlResource):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = XmlResource
_xrc.XmlResource_swigregister(XmlResourcePtr)

def EmptyXmlResource(*args, **kwargs):
    """EmptyXmlResource(int flags=XRC_USE_LOCALE) -> XmlResource"""
    val = _xrc.new_EmptyXmlResource(*args, **kwargs)
    val.thisown = 1
    val.InitAllHandlers()
    return val

def XmlResource_AddSubclassFactory(*args, **kwargs):
    """XmlResource_AddSubclassFactory(XmlSubclassFactory factory)"""
    return _xrc.XmlResource_AddSubclassFactory(*args, **kwargs)

def XmlResource_GetXRCID(*args, **kwargs):
    """XmlResource_GetXRCID(wxString str_id) -> int"""
    return _xrc.XmlResource_GetXRCID(*args, **kwargs)

def XmlResource_Get(*args, **kwargs):
    """XmlResource_Get() -> XmlResource"""
    return _xrc.XmlResource_Get(*args, **kwargs)

def XmlResource_Set(*args, **kwargs):
    """XmlResource_Set(XmlResource res) -> XmlResource"""
    return _xrc.XmlResource_Set(*args, **kwargs)

def XRCID(str_id):
    return XmlResource_GetXRCID(str_id)

def XRCCTRL(window, str_id, *ignoreargs):
    return window.FindWindowById(XRCID(str_id))

#---------------------------------------------------------------------------

class XmlSubclassFactory(object):
    """"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPyXmlSubclassFactory instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__() -> XmlSubclassFactory"""
        newobj = _xrc.new_XmlSubclassFactory(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setCallbackInfo(self, XmlSubclassFactory)

    def _setCallbackInfo(*args, **kwargs):
        """_setCallbackInfo(PyObject self, PyObject _class)"""
        return _xrc.XmlSubclassFactory__setCallbackInfo(*args, **kwargs)


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
    """"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxXmlProperty instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(wxString name=wxPyEmptyString, wxString value=wxPyEmptyString, 
    XmlProperty next=None) -> XmlProperty"""
        newobj = _xrc.new_XmlProperty(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def GetName(*args, **kwargs):
        """GetName() -> wxString"""
        return _xrc.XmlProperty_GetName(*args, **kwargs)

    def GetValue(*args, **kwargs):
        """GetValue() -> wxString"""
        return _xrc.XmlProperty_GetValue(*args, **kwargs)

    def GetNext(*args, **kwargs):
        """GetNext() -> XmlProperty"""
        return _xrc.XmlProperty_GetNext(*args, **kwargs)

    def SetName(*args, **kwargs):
        """SetName(wxString name)"""
        return _xrc.XmlProperty_SetName(*args, **kwargs)

    def SetValue(*args, **kwargs):
        """SetValue(wxString value)"""
        return _xrc.XmlProperty_SetValue(*args, **kwargs)

    def SetNext(*args, **kwargs):
        """SetNext(XmlProperty next)"""
        return _xrc.XmlProperty_SetNext(*args, **kwargs)


class XmlPropertyPtr(XmlProperty):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = XmlProperty
_xrc.XmlProperty_swigregister(XmlPropertyPtr)

class XmlNode(object):
    """"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxXmlNode instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(XmlNode parent=None, wxXmlNodeType type=0, wxString name=wxPyEmptyString, 
    wxString content=wxPyEmptyString, 
    XmlProperty props=None, XmlNode next=None) -> XmlNode"""
        newobj = _xrc.new_XmlNode(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __del__(self, destroy=_xrc.delete_XmlNode):
        """__del__()"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def AddChild(*args, **kwargs):
        """AddChild(XmlNode child)"""
        return _xrc.XmlNode_AddChild(*args, **kwargs)

    def InsertChild(*args, **kwargs):
        """InsertChild(XmlNode child, XmlNode before_node)"""
        return _xrc.XmlNode_InsertChild(*args, **kwargs)

    def RemoveChild(*args, **kwargs):
        """RemoveChild(XmlNode child) -> bool"""
        return _xrc.XmlNode_RemoveChild(*args, **kwargs)

    def AddProperty(*args, **kwargs):
        """AddProperty(XmlProperty prop)"""
        return _xrc.XmlNode_AddProperty(*args, **kwargs)

    def AddPropertyName(*args, **kwargs):
        """AddPropertyName(wxString name, wxString value)"""
        return _xrc.XmlNode_AddPropertyName(*args, **kwargs)

    def DeleteProperty(*args, **kwargs):
        """DeleteProperty(wxString name) -> bool"""
        return _xrc.XmlNode_DeleteProperty(*args, **kwargs)

    def GetType(*args, **kwargs):
        """GetType() -> wxXmlNodeType"""
        return _xrc.XmlNode_GetType(*args, **kwargs)

    def GetName(*args, **kwargs):
        """GetName() -> wxString"""
        return _xrc.XmlNode_GetName(*args, **kwargs)

    def GetContent(*args, **kwargs):
        """GetContent() -> wxString"""
        return _xrc.XmlNode_GetContent(*args, **kwargs)

    def GetParent(*args, **kwargs):
        """GetParent() -> XmlNode"""
        return _xrc.XmlNode_GetParent(*args, **kwargs)

    def GetNext(*args, **kwargs):
        """GetNext() -> XmlNode"""
        return _xrc.XmlNode_GetNext(*args, **kwargs)

    def GetChildren(*args, **kwargs):
        """GetChildren() -> XmlNode"""
        return _xrc.XmlNode_GetChildren(*args, **kwargs)

    def GetProperties(*args, **kwargs):
        """GetProperties() -> XmlProperty"""
        return _xrc.XmlNode_GetProperties(*args, **kwargs)

    def GetPropVal(*args, **kwargs):
        """GetPropVal(wxString propName, wxString defaultVal) -> wxString"""
        return _xrc.XmlNode_GetPropVal(*args, **kwargs)

    def HasProp(*args, **kwargs):
        """HasProp(wxString propName) -> bool"""
        return _xrc.XmlNode_HasProp(*args, **kwargs)

    def SetType(*args, **kwargs):
        """SetType(wxXmlNodeType type)"""
        return _xrc.XmlNode_SetType(*args, **kwargs)

    def SetName(*args, **kwargs):
        """SetName(wxString name)"""
        return _xrc.XmlNode_SetName(*args, **kwargs)

    def SetContent(*args, **kwargs):
        """SetContent(wxString con)"""
        return _xrc.XmlNode_SetContent(*args, **kwargs)

    def SetParent(*args, **kwargs):
        """SetParent(XmlNode parent)"""
        return _xrc.XmlNode_SetParent(*args, **kwargs)

    def SetNext(*args, **kwargs):
        """SetNext(XmlNode next)"""
        return _xrc.XmlNode_SetNext(*args, **kwargs)

    def SetChildren(*args, **kwargs):
        """SetChildren(XmlNode child)"""
        return _xrc.XmlNode_SetChildren(*args, **kwargs)

    def SetProperties(*args, **kwargs):
        """SetProperties(XmlProperty prop)"""
        return _xrc.XmlNode_SetProperties(*args, **kwargs)


class XmlNodePtr(XmlNode):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = XmlNode
_xrc.XmlNode_swigregister(XmlNodePtr)

def XmlNodeEasy(*args, **kwargs):
    """XmlNodeEasy(wxXmlNodeType type, wxString name, wxString content=wxPyEmptyString) -> XmlNode"""
    val = _xrc.new_XmlNodeEasy(*args, **kwargs)
    val.thisown = 1
    return val

class XmlDocument(core.Object):
    """"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxXmlDocument instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(wxString filename, wxString encoding=wxPyUTF8String) -> XmlDocument"""
        newobj = _xrc.new_XmlDocument(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __del__(self, destroy=_xrc.delete_XmlDocument):
        """__del__()"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def Load(*args, **kwargs):
        """Load(wxString filename, wxString encoding=wxPyUTF8String) -> bool"""
        return _xrc.XmlDocument_Load(*args, **kwargs)

    def LoadFromStream(*args, **kwargs):
        """LoadFromStream(wxInputStream stream, wxString encoding=wxPyUTF8String) -> bool"""
        return _xrc.XmlDocument_LoadFromStream(*args, **kwargs)

    def Save(*args, **kwargs):
        """Save(wxString filename) -> bool"""
        return _xrc.XmlDocument_Save(*args, **kwargs)

    def SaveToStream(*args, **kwargs):
        """SaveToStream(OutputStream stream) -> bool"""
        return _xrc.XmlDocument_SaveToStream(*args, **kwargs)

    def IsOk(*args, **kwargs):
        """IsOk() -> bool"""
        return _xrc.XmlDocument_IsOk(*args, **kwargs)

    def GetRoot(*args, **kwargs):
        """GetRoot() -> XmlNode"""
        return _xrc.XmlDocument_GetRoot(*args, **kwargs)

    def GetVersion(*args, **kwargs):
        """GetVersion() -> wxString"""
        return _xrc.XmlDocument_GetVersion(*args, **kwargs)

    def GetFileEncoding(*args, **kwargs):
        """GetFileEncoding() -> wxString"""
        return _xrc.XmlDocument_GetFileEncoding(*args, **kwargs)

    def SetRoot(*args, **kwargs):
        """SetRoot(XmlNode node)"""
        return _xrc.XmlDocument_SetRoot(*args, **kwargs)

    def SetVersion(*args, **kwargs):
        """SetVersion(wxString version)"""
        return _xrc.XmlDocument_SetVersion(*args, **kwargs)

    def SetFileEncoding(*args, **kwargs):
        """SetFileEncoding(wxString encoding)"""
        return _xrc.XmlDocument_SetFileEncoding(*args, **kwargs)


class XmlDocumentPtr(XmlDocument):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = XmlDocument
_xrc.XmlDocument_swigregister(XmlDocumentPtr)

def XmlDocumentFromStream(*args, **kwargs):
    """XmlDocumentFromStream(wxInputStream stream, wxString encoding=wxPyUTF8String) -> XmlDocument"""
    val = _xrc.new_XmlDocumentFromStream(*args, **kwargs)
    val.thisown = 1
    return val

def EmptyXmlDocument(*args, **kwargs):
    """EmptyXmlDocument() -> XmlDocument"""
    val = _xrc.new_EmptyXmlDocument(*args, **kwargs)
    val.thisown = 1
    return val

#---------------------------------------------------------------------------

class XmlResourceHandler(core.Object):
    """"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPyXmlResourceHandler instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__() -> XmlResourceHandler"""
        newobj = _xrc.new_XmlResourceHandler(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setCallbackInfo(self, XmlResourceHandler)

    def _setCallbackInfo(*args, **kwargs):
        """_setCallbackInfo(PyObject self, PyObject _class)"""
        return _xrc.XmlResourceHandler__setCallbackInfo(*args, **kwargs)

    def CreateResource(*args, **kwargs):
        """CreateResource(XmlNode node, Object parent, Object instance) -> Object"""
        return _xrc.XmlResourceHandler_CreateResource(*args, **kwargs)

    def SetParentResource(*args, **kwargs):
        """SetParentResource(XmlResource res)"""
        return _xrc.XmlResourceHandler_SetParentResource(*args, **kwargs)

    def GetResource(*args, **kwargs):
        """GetResource() -> XmlResource"""
        return _xrc.XmlResourceHandler_GetResource(*args, **kwargs)

    def GetNode(*args, **kwargs):
        """GetNode() -> XmlNode"""
        return _xrc.XmlResourceHandler_GetNode(*args, **kwargs)

    def GetClass(*args, **kwargs):
        """GetClass() -> wxString"""
        return _xrc.XmlResourceHandler_GetClass(*args, **kwargs)

    def GetParent(*args, **kwargs):
        """GetParent() -> Object"""
        return _xrc.XmlResourceHandler_GetParent(*args, **kwargs)

    def GetInstance(*args, **kwargs):
        """GetInstance() -> Object"""
        return _xrc.XmlResourceHandler_GetInstance(*args, **kwargs)

    def GetParentAsWindow(*args, **kwargs):
        """GetParentAsWindow() -> Window"""
        return _xrc.XmlResourceHandler_GetParentAsWindow(*args, **kwargs)

    def GetInstanceAsWindow(*args, **kwargs):
        """GetInstanceAsWindow() -> Window"""
        return _xrc.XmlResourceHandler_GetInstanceAsWindow(*args, **kwargs)

    def IsOfClass(*args, **kwargs):
        """IsOfClass(XmlNode node, wxString classname) -> bool"""
        return _xrc.XmlResourceHandler_IsOfClass(*args, **kwargs)

    def GetNodeContent(*args, **kwargs):
        """GetNodeContent(XmlNode node) -> wxString"""
        return _xrc.XmlResourceHandler_GetNodeContent(*args, **kwargs)

    def HasParam(*args, **kwargs):
        """HasParam(wxString param) -> bool"""
        return _xrc.XmlResourceHandler_HasParam(*args, **kwargs)

    def GetParamNode(*args, **kwargs):
        """GetParamNode(wxString param) -> XmlNode"""
        return _xrc.XmlResourceHandler_GetParamNode(*args, **kwargs)

    def GetParamValue(*args, **kwargs):
        """GetParamValue(wxString param) -> wxString"""
        return _xrc.XmlResourceHandler_GetParamValue(*args, **kwargs)

    def AddStyle(*args, **kwargs):
        """AddStyle(wxString name, int value)"""
        return _xrc.XmlResourceHandler_AddStyle(*args, **kwargs)

    def AddWindowStyles(*args, **kwargs):
        """AddWindowStyles()"""
        return _xrc.XmlResourceHandler_AddWindowStyles(*args, **kwargs)

    def GetStyle(*args, **kwargs):
        """GetStyle(wxString param=wxPyStyleString, int defaults=0) -> int"""
        return _xrc.XmlResourceHandler_GetStyle(*args, **kwargs)

    def GetText(*args, **kwargs):
        """GetText(wxString param, bool translate=True) -> wxString"""
        return _xrc.XmlResourceHandler_GetText(*args, **kwargs)

    def GetID(*args, **kwargs):
        """GetID() -> int"""
        return _xrc.XmlResourceHandler_GetID(*args, **kwargs)

    def GetName(*args, **kwargs):
        """GetName() -> wxString"""
        return _xrc.XmlResourceHandler_GetName(*args, **kwargs)

    def GetBool(*args, **kwargs):
        """GetBool(wxString param, bool defaultv=False) -> bool"""
        return _xrc.XmlResourceHandler_GetBool(*args, **kwargs)

    def GetLong(*args, **kwargs):
        """GetLong(wxString param, long defaultv=0) -> long"""
        return _xrc.XmlResourceHandler_GetLong(*args, **kwargs)

    def GetColour(*args, **kwargs):
        """GetColour(wxString param) -> wxColour"""
        return _xrc.XmlResourceHandler_GetColour(*args, **kwargs)

    def GetSize(*args, **kwargs):
        """GetSize(wxString param=wxPySizeString) -> Size"""
        return _xrc.XmlResourceHandler_GetSize(*args, **kwargs)

    def GetPosition(*args, **kwargs):
        """GetPosition(wxString param=wxPyPosString) -> Point"""
        return _xrc.XmlResourceHandler_GetPosition(*args, **kwargs)

    def GetDimension(*args, **kwargs):
        """GetDimension(wxString param, int defaultv=0) -> int"""
        return _xrc.XmlResourceHandler_GetDimension(*args, **kwargs)

    def GetBitmap(*args, **kwargs):
        """GetBitmap(wxString param=wxPyBitmapString, wxArtClient defaultArtClient=wxART_OTHER, 
    Size size=DefaultSize) -> wxBitmap"""
        return _xrc.XmlResourceHandler_GetBitmap(*args, **kwargs)

    def GetIcon(*args, **kwargs):
        """GetIcon(wxString param=wxPyIconString, wxArtClient defaultArtClient=wxART_OTHER, 
    Size size=DefaultSize) -> wxIcon"""
        return _xrc.XmlResourceHandler_GetIcon(*args, **kwargs)

    def GetFont(*args, **kwargs):
        """GetFont(wxString param=wxPyFontString) -> wxFont"""
        return _xrc.XmlResourceHandler_GetFont(*args, **kwargs)

    def SetupWindow(*args, **kwargs):
        """SetupWindow(Window wnd)"""
        return _xrc.XmlResourceHandler_SetupWindow(*args, **kwargs)

    def CreateChildren(*args, **kwargs):
        """CreateChildren(Object parent, bool this_hnd_only=False)"""
        return _xrc.XmlResourceHandler_CreateChildren(*args, **kwargs)

    def CreateChildrenPrivately(*args, **kwargs):
        """CreateChildrenPrivately(Object parent, XmlNode rootnode=None)"""
        return _xrc.XmlResourceHandler_CreateChildrenPrivately(*args, **kwargs)

    def CreateResFromNode(*args, **kwargs):
        """CreateResFromNode(XmlNode node, Object parent, Object instance=None) -> Object"""
        return _xrc.XmlResourceHandler_CreateResFromNode(*args, **kwargs)

    def GetCurFileSystem(*args, **kwargs):
        """GetCurFileSystem() -> FileSystem"""
        return _xrc.XmlResourceHandler_GetCurFileSystem(*args, **kwargs)


class XmlResourceHandlerPtr(XmlResourceHandler):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = XmlResourceHandler
_xrc.XmlResourceHandler_swigregister(XmlResourceHandlerPtr)

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


