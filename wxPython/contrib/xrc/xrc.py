# This file was created automatically by SWIG.
import xrcc

from misc import *

from misc2 import *

from windows import *

from gdi import *

from fonts import *

from clip_dnd import *

from events import *

from streams import *

from utils import *

from mdi import *

from frames import *

from stattool import *

from controls import *

from controls2 import *

from windows2 import *

from cmndlgs import *

from windows3 import *

from image import *

from printfw import *

from sizers import *

from filesys import *

def XRCID(str_id):
    return wxXmlResource_GetXRCID(str_id)

def XRCCTRL(window, str_id, *ignoreargs):
    return window.FindWindowById(XRCID(str_id))

XMLID = XRCID
XMLCTRL = XRCCTRL

class wxXmlResourcePtr(wxObjectPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __del__(self, delfunc=xrcc.delete_wxXmlResource):
        if self.thisown == 1:
            try:
                delfunc(self)
            except:
                pass
    def Load(self, *_args, **_kwargs):
        val = apply(xrcc.wxXmlResource_Load,(self,) + _args, _kwargs)
        return val
    def LoadFromString(self, *_args, **_kwargs):
        val = apply(xrcc.wxXmlResource_LoadFromString,(self,) + _args, _kwargs)
        return val
    def InitAllHandlers(self, *_args, **_kwargs):
        val = apply(xrcc.wxXmlResource_InitAllHandlers,(self,) + _args, _kwargs)
        return val
    def AddHandler(self, *_args, **_kwargs):
        val = apply(xrcc.wxXmlResource_AddHandler,(self,) + _args, _kwargs)
        return val
    def InsertHandler(self, *_args, **_kwargs):
        val = apply(xrcc.wxXmlResource_InsertHandler,(self,) + _args, _kwargs)
        return val
    def ClearHandlers(self, *_args, **_kwargs):
        val = apply(xrcc.wxXmlResource_ClearHandlers,(self,) + _args, _kwargs)
        return val
    def LoadMenu(self, *_args, **_kwargs):
        val = apply(xrcc.wxXmlResource_LoadMenu,(self,) + _args, _kwargs)
        return val
    def LoadMenuBar(self, *_args, **_kwargs):
        val = apply(xrcc.wxXmlResource_LoadMenuBar,(self,) + _args, _kwargs)
        return val
    def LoadMenuBarOnFrame(self, *_args, **_kwargs):
        val = apply(xrcc.wxXmlResource_LoadMenuBarOnFrame,(self,) + _args, _kwargs)
        return val
    def LoadToolBar(self, *_args, **_kwargs):
        val = apply(xrcc.wxXmlResource_LoadToolBar,(self,) + _args, _kwargs)
        return val
    def LoadDialog(self, *_args, **_kwargs):
        val = apply(xrcc.wxXmlResource_LoadDialog,(self,) + _args, _kwargs)
        if val: val = wxDialogPtr(val) 
        return val
    def LoadOnDialog(self, *_args, **_kwargs):
        val = apply(xrcc.wxXmlResource_LoadOnDialog,(self,) + _args, _kwargs)
        return val
    def LoadPanel(self, *_args, **_kwargs):
        val = apply(xrcc.wxXmlResource_LoadPanel,(self,) + _args, _kwargs)
        if val: val = wxPanelPtr(val) 
        return val
    def LoadOnPanel(self, *_args, **_kwargs):
        val = apply(xrcc.wxXmlResource_LoadOnPanel,(self,) + _args, _kwargs)
        return val
    def LoadFrame(self, *_args, **_kwargs):
        val = apply(xrcc.wxXmlResource_LoadFrame,(self,) + _args, _kwargs)
        return val
    def LoadOnFrame(self, *_args, **_kwargs):
        val = apply(xrcc.wxXmlResource_LoadOnFrame,(self,) + _args, _kwargs)
        return val
    def LoadObject(self, *_args, **_kwargs):
        val = apply(xrcc.wxXmlResource_LoadObject,(self,) + _args, _kwargs)
        return val
    def LoadOnObject(self, *_args, **_kwargs):
        val = apply(xrcc.wxXmlResource_LoadOnObject,(self,) + _args, _kwargs)
        return val
    def LoadBitmap(self, *_args, **_kwargs):
        val = apply(xrcc.wxXmlResource_LoadBitmap,(self,) + _args, _kwargs)
        if val: val = wxBitmapPtr(val) ; val.thisown = 1
        return val
    def LoadIcon(self, *_args, **_kwargs):
        val = apply(xrcc.wxXmlResource_LoadIcon,(self,) + _args, _kwargs)
        if val: val = wxIconPtr(val) ; val.thisown = 1
        return val
    def AttachUnknownControl(self, *_args, **_kwargs):
        val = apply(xrcc.wxXmlResource_AttachUnknownControl,(self,) + _args, _kwargs)
        return val
    def GetVersion(self, *_args, **_kwargs):
        val = apply(xrcc.wxXmlResource_GetVersion,(self,) + _args, _kwargs)
        return val
    def CompareVersion(self, *_args, **_kwargs):
        val = apply(xrcc.wxXmlResource_CompareVersion,(self,) + _args, _kwargs)
        return val
    def GetFlags(self, *_args, **_kwargs):
        val = apply(xrcc.wxXmlResource_GetFlags,(self,) + _args, _kwargs)
        return val
    def SetFlags(self, *_args, **_kwargs):
        val = apply(xrcc.wxXmlResource_SetFlags,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxXmlResource instance at %s>" % (self.this,)
class wxXmlResource(wxXmlResourcePtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(xrcc.new_wxXmlResource,_args,_kwargs)
        self.thisown = 1
        self.InitAllHandlers()



def wxEmptyXmlResource(*_args,**_kwargs):
    val = wxXmlResourcePtr(apply(xrcc.new_wxEmptyXmlResource,_args,_kwargs))
    val.thisown = 1
    val.InitAllHandlers()
    return val


class wxXmlSubclassFactoryPtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def _setCallbackInfo(self, *_args, **_kwargs):
        val = apply(xrcc.wxXmlSubclassFactory__setCallbackInfo,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxXmlSubclassFactory instance at %s>" % (self.this,)
class wxXmlSubclassFactory(wxXmlSubclassFactoryPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(xrcc.new_wxXmlSubclassFactory,_args,_kwargs)
        self.thisown = 1
        self._setCallbackInfo(self, wxXmlSubclassFactory)




class wxXmlPropertyPtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def GetName(self, *_args, **_kwargs):
        val = apply(xrcc.wxXmlProperty_GetName,(self,) + _args, _kwargs)
        return val
    def GetValue(self, *_args, **_kwargs):
        val = apply(xrcc.wxXmlProperty_GetValue,(self,) + _args, _kwargs)
        return val
    def GetNext(self, *_args, **_kwargs):
        val = apply(xrcc.wxXmlProperty_GetNext,(self,) + _args, _kwargs)
        if val: val = wxXmlPropertyPtr(val) 
        return val
    def SetName(self, *_args, **_kwargs):
        val = apply(xrcc.wxXmlProperty_SetName,(self,) + _args, _kwargs)
        return val
    def SetValue(self, *_args, **_kwargs):
        val = apply(xrcc.wxXmlProperty_SetValue,(self,) + _args, _kwargs)
        return val
    def SetNext(self, *_args, **_kwargs):
        val = apply(xrcc.wxXmlProperty_SetNext,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxXmlProperty instance at %s>" % (self.this,)
class wxXmlProperty(wxXmlPropertyPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(xrcc.new_wxXmlProperty,_args,_kwargs)
        self.thisown = 1




class wxXmlNodePtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __del__(self, delfunc=xrcc.delete_wxXmlNode):
        if self.thisown == 1:
            try:
                delfunc(self)
            except:
                pass
    def AddChild(self, *_args, **_kwargs):
        val = apply(xrcc.wxXmlNode_AddChild,(self,) + _args, _kwargs)
        return val
    def InsertChild(self, *_args, **_kwargs):
        val = apply(xrcc.wxXmlNode_InsertChild,(self,) + _args, _kwargs)
        return val
    def RemoveChild(self, *_args, **_kwargs):
        val = apply(xrcc.wxXmlNode_RemoveChild,(self,) + _args, _kwargs)
        return val
    def AddProperty(self, *_args, **_kwargs):
        val = apply(xrcc.wxXmlNode_AddProperty,(self,) + _args, _kwargs)
        return val
    def AddPropertyName(self, *_args, **_kwargs):
        val = apply(xrcc.wxXmlNode_AddPropertyName,(self,) + _args, _kwargs)
        return val
    def DeleteProperty(self, *_args, **_kwargs):
        val = apply(xrcc.wxXmlNode_DeleteProperty,(self,) + _args, _kwargs)
        return val
    def GetType(self, *_args, **_kwargs):
        val = apply(xrcc.wxXmlNode_GetType,(self,) + _args, _kwargs)
        return val
    def GetName(self, *_args, **_kwargs):
        val = apply(xrcc.wxXmlNode_GetName,(self,) + _args, _kwargs)
        return val
    def GetContent(self, *_args, **_kwargs):
        val = apply(xrcc.wxXmlNode_GetContent,(self,) + _args, _kwargs)
        return val
    def GetParent(self, *_args, **_kwargs):
        val = apply(xrcc.wxXmlNode_GetParent,(self,) + _args, _kwargs)
        if val: val = wxXmlNodePtr(val) 
        return val
    def GetNext(self, *_args, **_kwargs):
        val = apply(xrcc.wxXmlNode_GetNext,(self,) + _args, _kwargs)
        if val: val = wxXmlNodePtr(val) 
        return val
    def GetChildren(self, *_args, **_kwargs):
        val = apply(xrcc.wxXmlNode_GetChildren,(self,) + _args, _kwargs)
        if val: val = wxXmlNodePtr(val) 
        return val
    def GetProperties(self, *_args, **_kwargs):
        val = apply(xrcc.wxXmlNode_GetProperties,(self,) + _args, _kwargs)
        if val: val = wxXmlPropertyPtr(val) 
        return val
    def GetPropVal(self, *_args, **_kwargs):
        val = apply(xrcc.wxXmlNode_GetPropVal,(self,) + _args, _kwargs)
        return val
    def HasProp(self, *_args, **_kwargs):
        val = apply(xrcc.wxXmlNode_HasProp,(self,) + _args, _kwargs)
        return val
    def SetType(self, *_args, **_kwargs):
        val = apply(xrcc.wxXmlNode_SetType,(self,) + _args, _kwargs)
        return val
    def SetName(self, *_args, **_kwargs):
        val = apply(xrcc.wxXmlNode_SetName,(self,) + _args, _kwargs)
        return val
    def SetContent(self, *_args, **_kwargs):
        val = apply(xrcc.wxXmlNode_SetContent,(self,) + _args, _kwargs)
        return val
    def SetParent(self, *_args, **_kwargs):
        val = apply(xrcc.wxXmlNode_SetParent,(self,) + _args, _kwargs)
        return val
    def SetNext(self, *_args, **_kwargs):
        val = apply(xrcc.wxXmlNode_SetNext,(self,) + _args, _kwargs)
        return val
    def SetChildren(self, *_args, **_kwargs):
        val = apply(xrcc.wxXmlNode_SetChildren,(self,) + _args, _kwargs)
        return val
    def SetProperties(self, *_args, **_kwargs):
        val = apply(xrcc.wxXmlNode_SetProperties,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxXmlNode instance at %s>" % (self.this,)
class wxXmlNode(wxXmlNodePtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(xrcc.new_wxXmlNode,_args,_kwargs)
        self.thisown = 1



def wxXmlNodeEasy(*_args,**_kwargs):
    val = wxXmlNodePtr(apply(xrcc.new_wxXmlNodeEasy,_args,_kwargs))
    val.thisown = 1
    return val


class wxXmlDocumentPtr(wxObjectPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __del__(self, delfunc=xrcc.delete_wxXmlDocument):
        if self.thisown == 1:
            try:
                delfunc(self)
            except:
                pass
    def Load(self, *_args, **_kwargs):
        val = apply(xrcc.wxXmlDocument_Load,(self,) + _args, _kwargs)
        return val
    def LoadFromStream(self, *_args, **_kwargs):
        val = apply(xrcc.wxXmlDocument_LoadFromStream,(self,) + _args, _kwargs)
        return val
    def Save(self, *_args, **_kwargs):
        val = apply(xrcc.wxXmlDocument_Save,(self,) + _args, _kwargs)
        return val
    def SaveToStream(self, *_args, **_kwargs):
        val = apply(xrcc.wxXmlDocument_SaveToStream,(self,) + _args, _kwargs)
        return val
    def IsOk(self, *_args, **_kwargs):
        val = apply(xrcc.wxXmlDocument_IsOk,(self,) + _args, _kwargs)
        return val
    def GetRoot(self, *_args, **_kwargs):
        val = apply(xrcc.wxXmlDocument_GetRoot,(self,) + _args, _kwargs)
        if val: val = wxXmlNodePtr(val) 
        return val
    def GetVersion(self, *_args, **_kwargs):
        val = apply(xrcc.wxXmlDocument_GetVersion,(self,) + _args, _kwargs)
        return val
    def GetFileEncoding(self, *_args, **_kwargs):
        val = apply(xrcc.wxXmlDocument_GetFileEncoding,(self,) + _args, _kwargs)
        return val
    def SetRoot(self, *_args, **_kwargs):
        val = apply(xrcc.wxXmlDocument_SetRoot,(self,) + _args, _kwargs)
        return val
    def SetVersion(self, *_args, **_kwargs):
        val = apply(xrcc.wxXmlDocument_SetVersion,(self,) + _args, _kwargs)
        return val
    def SetFileEncoding(self, *_args, **_kwargs):
        val = apply(xrcc.wxXmlDocument_SetFileEncoding,(self,) + _args, _kwargs)
        return val
    def GetEncoding(self, *_args, **_kwargs):
        val = apply(xrcc.wxXmlDocument_GetEncoding,(self,) + _args, _kwargs)
        return val
    def SetEncoding(self, *_args, **_kwargs):
        val = apply(xrcc.wxXmlDocument_SetEncoding,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxXmlDocument instance at %s>" % (self.this,)
class wxXmlDocument(wxXmlDocumentPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(xrcc.new_wxXmlDocument,_args,_kwargs)
        self.thisown = 1



def wxXmlDocumentFromStream(*_args,**_kwargs):
    val = wxXmlDocumentPtr(apply(xrcc.new_wxXmlDocumentFromStream,_args,_kwargs))
    val.thisown = 1
    return val

def wxEmptyXmlDocument(*_args,**_kwargs):
    val = wxXmlDocumentPtr(apply(xrcc.new_wxEmptyXmlDocument,_args,_kwargs))
    val.thisown = 1
    return val


class wxXmlResourceHandlerPtr(wxObjectPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def _setCallbackInfo(self, *_args, **_kwargs):
        val = apply(xrcc.wxXmlResourceHandler__setCallbackInfo,(self,) + _args, _kwargs)
        return val
    def CreateResource(self, *_args, **_kwargs):
        val = apply(xrcc.wxXmlResourceHandler_CreateResource,(self,) + _args, _kwargs)
        return val
    def SetParentResource(self, *_args, **_kwargs):
        val = apply(xrcc.wxXmlResourceHandler_SetParentResource,(self,) + _args, _kwargs)
        return val
    def GetResource(self, *_args, **_kwargs):
        val = apply(xrcc.wxXmlResourceHandler_GetResource,(self,) + _args, _kwargs)
        if val: val = wxXmlResourcePtr(val) 
        return val
    def GetNode(self, *_args, **_kwargs):
        val = apply(xrcc.wxXmlResourceHandler_GetNode,(self,) + _args, _kwargs)
        if val: val = wxXmlNodePtr(val) 
        return val
    def GetClass(self, *_args, **_kwargs):
        val = apply(xrcc.wxXmlResourceHandler_GetClass,(self,) + _args, _kwargs)
        return val
    def GetParent(self, *_args, **_kwargs):
        val = apply(xrcc.wxXmlResourceHandler_GetParent,(self,) + _args, _kwargs)
        return val
    def GetInstance(self, *_args, **_kwargs):
        val = apply(xrcc.wxXmlResourceHandler_GetInstance,(self,) + _args, _kwargs)
        return val
    def GetParentAsWindow(self, *_args, **_kwargs):
        val = apply(xrcc.wxXmlResourceHandler_GetParentAsWindow,(self,) + _args, _kwargs)
        return val
    def GetInstanceAsWindow(self, *_args, **_kwargs):
        val = apply(xrcc.wxXmlResourceHandler_GetInstanceAsWindow,(self,) + _args, _kwargs)
        return val
    def IsOfClass(self, *_args, **_kwargs):
        val = apply(xrcc.wxXmlResourceHandler_IsOfClass,(self,) + _args, _kwargs)
        return val
    def GetNodeContent(self, *_args, **_kwargs):
        val = apply(xrcc.wxXmlResourceHandler_GetNodeContent,(self,) + _args, _kwargs)
        return val
    def HasParam(self, *_args, **_kwargs):
        val = apply(xrcc.wxXmlResourceHandler_HasParam,(self,) + _args, _kwargs)
        return val
    def GetParamNode(self, *_args, **_kwargs):
        val = apply(xrcc.wxXmlResourceHandler_GetParamNode,(self,) + _args, _kwargs)
        if val: val = wxXmlNodePtr(val) 
        return val
    def GetParamValue(self, *_args, **_kwargs):
        val = apply(xrcc.wxXmlResourceHandler_GetParamValue,(self,) + _args, _kwargs)
        return val
    def AddStyle(self, *_args, **_kwargs):
        val = apply(xrcc.wxXmlResourceHandler_AddStyle,(self,) + _args, _kwargs)
        return val
    def AddWindowStyles(self, *_args, **_kwargs):
        val = apply(xrcc.wxXmlResourceHandler_AddWindowStyles,(self,) + _args, _kwargs)
        return val
    def GetStyle(self, *_args, **_kwargs):
        val = apply(xrcc.wxXmlResourceHandler_GetStyle,(self,) + _args, _kwargs)
        return val
    def GetText(self, *_args, **_kwargs):
        val = apply(xrcc.wxXmlResourceHandler_GetText,(self,) + _args, _kwargs)
        return val
    def GetID(self, *_args, **_kwargs):
        val = apply(xrcc.wxXmlResourceHandler_GetID,(self,) + _args, _kwargs)
        return val
    def GetName(self, *_args, **_kwargs):
        val = apply(xrcc.wxXmlResourceHandler_GetName,(self,) + _args, _kwargs)
        return val
    def GetBool(self, *_args, **_kwargs):
        val = apply(xrcc.wxXmlResourceHandler_GetBool,(self,) + _args, _kwargs)
        return val
    def GetLong(self, *_args, **_kwargs):
        val = apply(xrcc.wxXmlResourceHandler_GetLong,(self,) + _args, _kwargs)
        return val
    def GetColour(self, *_args, **_kwargs):
        val = apply(xrcc.wxXmlResourceHandler_GetColour,(self,) + _args, _kwargs)
        if val: val = wxColourPtr(val) ; val.thisown = 1
        return val
    def GetSize(self, *_args, **_kwargs):
        val = apply(xrcc.wxXmlResourceHandler_GetSize,(self,) + _args, _kwargs)
        if val: val = wxSizePtr(val) ; val.thisown = 1
        return val
    def GetPosition(self, *_args, **_kwargs):
        val = apply(xrcc.wxXmlResourceHandler_GetPosition,(self,) + _args, _kwargs)
        if val: val = wxPointPtr(val) ; val.thisown = 1
        return val
    def GetDimension(self, *_args, **_kwargs):
        val = apply(xrcc.wxXmlResourceHandler_GetDimension,(self,) + _args, _kwargs)
        return val
    def GetBitmap(self, *_args, **_kwargs):
        val = apply(xrcc.wxXmlResourceHandler_GetBitmap,(self,) + _args, _kwargs)
        if val: val = wxBitmapPtr(val) ; val.thisown = 1
        return val
    def GetIcon(self, *_args, **_kwargs):
        val = apply(xrcc.wxXmlResourceHandler_GetIcon,(self,) + _args, _kwargs)
        if val: val = wxIconPtr(val) ; val.thisown = 1
        return val
    def GetFont(self, *_args, **_kwargs):
        val = apply(xrcc.wxXmlResourceHandler_GetFont,(self,) + _args, _kwargs)
        if val: val = wxFontPtr(val) ; val.thisown = 1
        return val
    def SetupWindow(self, *_args, **_kwargs):
        val = apply(xrcc.wxXmlResourceHandler_SetupWindow,(self,) + _args, _kwargs)
        return val
    def CreateChildren(self, *_args, **_kwargs):
        val = apply(xrcc.wxXmlResourceHandler_CreateChildren,(self,) + _args, _kwargs)
        return val
    def CreateChildrenPrivately(self, *_args, **_kwargs):
        val = apply(xrcc.wxXmlResourceHandler_CreateChildrenPrivately,(self,) + _args, _kwargs)
        return val
    def CreateResFromNode(self, *_args, **_kwargs):
        val = apply(xrcc.wxXmlResourceHandler_CreateResFromNode,(self,) + _args, _kwargs)
        return val
    def GetCurFileSystem(self, *_args, **_kwargs):
        val = apply(xrcc.wxXmlResourceHandler_GetCurFileSystem,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxXmlResourceHandler instance at %s>" % (self.this,)
class wxXmlResourceHandler(wxXmlResourceHandlerPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(xrcc.new_wxXmlResourceHandler,_args,_kwargs)
        self.thisown = 1
        self._setCallbackInfo(self, wxXmlResourceHandler)






#-------------- FUNCTION WRAPPERS ------------------

wxXmlResource_AddSubclassFactory = xrcc.wxXmlResource_AddSubclassFactory

wxXmlResource_GetXRCID = xrcc.wxXmlResource_GetXRCID

def wxXmlResource_Get(*_args, **_kwargs):
    val = apply(xrcc.wxXmlResource_Get,_args,_kwargs)
    if val: val = wxXmlResourcePtr(val)
    return val

def wxXmlResource_Set(*_args, **_kwargs):
    val = apply(xrcc.wxXmlResource_Set,_args,_kwargs)
    if val: val = wxXmlResourcePtr(val)
    return val



#-------------- VARIABLE WRAPPERS ------------------

wxXRC_USE_LOCALE = xrcc.wxXRC_USE_LOCALE
wxXRC_NO_SUBCLASSING = xrcc.wxXRC_NO_SUBCLASSING
wxXML_ELEMENT_NODE = xrcc.wxXML_ELEMENT_NODE
wxXML_ATTRIBUTE_NODE = xrcc.wxXML_ATTRIBUTE_NODE
wxXML_TEXT_NODE = xrcc.wxXML_TEXT_NODE
wxXML_CDATA_SECTION_NODE = xrcc.wxXML_CDATA_SECTION_NODE
wxXML_ENTITY_REF_NODE = xrcc.wxXML_ENTITY_REF_NODE
wxXML_ENTITY_NODE = xrcc.wxXML_ENTITY_NODE
wxXML_PI_NODE = xrcc.wxXML_PI_NODE
wxXML_COMMENT_NODE = xrcc.wxXML_COMMENT_NODE
wxXML_DOCUMENT_NODE = xrcc.wxXML_DOCUMENT_NODE
wxXML_DOCUMENT_TYPE_NODE = xrcc.wxXML_DOCUMENT_TYPE_NODE
wxXML_DOCUMENT_FRAG_NODE = xrcc.wxXML_DOCUMENT_FRAG_NODE
wxXML_NOTATION_NODE = xrcc.wxXML_NOTATION_NODE
wxXML_HTML_DOCUMENT_NODE = xrcc.wxXML_HTML_DOCUMENT_NODE


#-------------- USER INCLUDE -----------------------


# The global was removed  in favor of static accessor functions.  This is for
# backwards compatibility:
wxTheXmlResource = wxXmlResource_Get()

wx.wxXmlNodePtr = wxXmlNodePtr




#----------------------------------------------------------------------
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

