# This file was created automatically by SWIG.
import imagec

from misc import *

from gdi import *

from fonts import *

from streams import *
class wxImageHandlerPtr(wxObjectPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def GetName(self, *_args, **_kwargs):
        val = imagec.wxImageHandler_GetName(self, *_args, **_kwargs)
        return val
    def GetExtension(self, *_args, **_kwargs):
        val = imagec.wxImageHandler_GetExtension(self, *_args, **_kwargs)
        return val
    def GetType(self, *_args, **_kwargs):
        val = imagec.wxImageHandler_GetType(self, *_args, **_kwargs)
        return val
    def GetMimeType(self, *_args, **_kwargs):
        val = imagec.wxImageHandler_GetMimeType(self, *_args, **_kwargs)
        return val
    def CanRead(self, *_args, **_kwargs):
        val = imagec.wxImageHandler_CanRead(self, *_args, **_kwargs)
        return val
    def SetName(self, *_args, **_kwargs):
        val = imagec.wxImageHandler_SetName(self, *_args, **_kwargs)
        return val
    def SetExtension(self, *_args, **_kwargs):
        val = imagec.wxImageHandler_SetExtension(self, *_args, **_kwargs)
        return val
    def SetType(self, *_args, **_kwargs):
        val = imagec.wxImageHandler_SetType(self, *_args, **_kwargs)
        return val
    def SetMimeType(self, *_args, **_kwargs):
        val = imagec.wxImageHandler_SetMimeType(self, *_args, **_kwargs)
        return val
    def __repr__(self):
        return "<%s.%s instance; proxy of C++ wxImageHandler instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this)
class wxImageHandler(wxImageHandlerPtr):
    def __init__(self,this):
        self.this = this




class wxPNGHandlerPtr(wxImageHandlerPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __repr__(self):
        return "<%s.%s instance; proxy of C++ wxPNGHandler instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this)
class wxPNGHandler(wxPNGHandlerPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = imagec.new_wxPNGHandler(*_args,**_kwargs)
        self.thisown = 1




class wxJPEGHandlerPtr(wxImageHandlerPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __repr__(self):
        return "<%s.%s instance; proxy of C++ wxJPEGHandler instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this)
class wxJPEGHandler(wxJPEGHandlerPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = imagec.new_wxJPEGHandler(*_args,**_kwargs)
        self.thisown = 1




class wxBMPHandlerPtr(wxImageHandlerPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __repr__(self):
        return "<%s.%s instance; proxy of C++ wxBMPHandler instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this)
class wxBMPHandler(wxBMPHandlerPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = imagec.new_wxBMPHandler(*_args,**_kwargs)
        self.thisown = 1




class wxICOHandlerPtr(wxBMPHandlerPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __repr__(self):
        return "<%s.%s instance; proxy of C++ wxICOHandler instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this)
class wxICOHandler(wxICOHandlerPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = imagec.new_wxICOHandler(*_args,**_kwargs)
        self.thisown = 1




class wxCURHandlerPtr(wxICOHandlerPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __repr__(self):
        return "<%s.%s instance; proxy of C++ wxCURHandler instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this)
class wxCURHandler(wxCURHandlerPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = imagec.new_wxCURHandler(*_args,**_kwargs)
        self.thisown = 1




class wxANIHandlerPtr(wxCURHandlerPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __repr__(self):
        return "<%s.%s instance; proxy of C++ wxANIHandler instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this)
class wxANIHandler(wxANIHandlerPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = imagec.new_wxANIHandler(*_args,**_kwargs)
        self.thisown = 1




class wxGIFHandlerPtr(wxImageHandlerPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __repr__(self):
        return "<%s.%s instance; proxy of C++ wxGIFHandler instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this)
class wxGIFHandler(wxGIFHandlerPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = imagec.new_wxGIFHandler(*_args,**_kwargs)
        self.thisown = 1




class wxPNMHandlerPtr(wxImageHandlerPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __repr__(self):
        return "<%s.%s instance; proxy of C++ wxPNMHandler instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this)
class wxPNMHandler(wxPNMHandlerPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = imagec.new_wxPNMHandler(*_args,**_kwargs)
        self.thisown = 1




class wxPCXHandlerPtr(wxImageHandlerPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __repr__(self):
        return "<%s.%s instance; proxy of C++ wxPCXHandler instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this)
class wxPCXHandler(wxPCXHandlerPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = imagec.new_wxPCXHandler(*_args,**_kwargs)
        self.thisown = 1




class wxTIFFHandlerPtr(wxImageHandlerPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __repr__(self):
        return "<%s.%s instance; proxy of C++ wxTIFFHandler instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this)
class wxTIFFHandler(wxTIFFHandlerPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = imagec.new_wxTIFFHandler(*_args,**_kwargs)
        self.thisown = 1




class wxImagePtr(wxObjectPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __del__(self, delfunc=imagec.delete_wxImage):
        if self.thisown == 1:
            try:
                delfunc(self)
            except:
                pass
    def Create(self, *_args, **_kwargs):
        val = imagec.wxImage_Create(self, *_args, **_kwargs)
        return val
    def Destroy(self, *_args, **_kwargs):
        val = imagec.wxImage_Destroy(self, *_args, **_kwargs)
        return val
    def Scale(self, *_args, **_kwargs):
        val = imagec.wxImage_Scale(self, *_args, **_kwargs)
        if val: val = wxImagePtr(val) ; val.thisown = 1
        return val
    def Rescale(self, *_args, **_kwargs):
        val = imagec.wxImage_Rescale(self, *_args, **_kwargs)
        if val: val = wxImagePtr(val) 
        return val
    def SetRGB(self, *_args, **_kwargs):
        val = imagec.wxImage_SetRGB(self, *_args, **_kwargs)
        return val
    def GetRed(self, *_args, **_kwargs):
        val = imagec.wxImage_GetRed(self, *_args, **_kwargs)
        return val
    def GetGreen(self, *_args, **_kwargs):
        val = imagec.wxImage_GetGreen(self, *_args, **_kwargs)
        return val
    def GetBlue(self, *_args, **_kwargs):
        val = imagec.wxImage_GetBlue(self, *_args, **_kwargs)
        return val
    def FindFirstUnusedColour(self, *_args, **_kwargs):
        val = imagec.wxImage_FindFirstUnusedColour(self, *_args, **_kwargs)
        return val
    def SetMaskFromImage(self, *_args, **_kwargs):
        val = imagec.wxImage_SetMaskFromImage(self, *_args, **_kwargs)
        return val
    def LoadFile(self, *_args, **_kwargs):
        val = imagec.wxImage_LoadFile(self, *_args, **_kwargs)
        return val
    def LoadMimeFile(self, *_args, **_kwargs):
        val = imagec.wxImage_LoadMimeFile(self, *_args, **_kwargs)
        return val
    def SaveFile(self, *_args, **_kwargs):
        val = imagec.wxImage_SaveFile(self, *_args, **_kwargs)
        return val
    def SaveMimeFile(self, *_args, **_kwargs):
        val = imagec.wxImage_SaveMimeFile(self, *_args, **_kwargs)
        return val
    def LoadStream(self, *_args, **_kwargs):
        val = imagec.wxImage_LoadStream(self, *_args, **_kwargs)
        return val
    def LoadMimeStream(self, *_args, **_kwargs):
        val = imagec.wxImage_LoadMimeStream(self, *_args, **_kwargs)
        return val
    def Ok(self, *_args, **_kwargs):
        val = imagec.wxImage_Ok(self, *_args, **_kwargs)
        return val
    def GetWidth(self, *_args, **_kwargs):
        val = imagec.wxImage_GetWidth(self, *_args, **_kwargs)
        return val
    def GetHeight(self, *_args, **_kwargs):
        val = imagec.wxImage_GetHeight(self, *_args, **_kwargs)
        return val
    def GetSubImage(self, *_args, **_kwargs):
        val = imagec.wxImage_GetSubImage(self, *_args, **_kwargs)
        if val: val = wxImagePtr(val) ; val.thisown = 1
        return val
    def Copy(self, *_args, **_kwargs):
        val = imagec.wxImage_Copy(self, *_args, **_kwargs)
        if val: val = wxImagePtr(val) ; val.thisown = 1
        return val
    def Paste(self, *_args, **_kwargs):
        val = imagec.wxImage_Paste(self, *_args, **_kwargs)
        return val
    def GetDataBuffer(self, *_args, **_kwargs):
        val = imagec.wxImage_GetDataBuffer(self, *_args, **_kwargs)
        return val
    def GetData(self, *_args, **_kwargs):
        val = imagec.wxImage_GetData(self, *_args, **_kwargs)
        return val
    def SetDataBuffer(self, *_args, **_kwargs):
        val = imagec.wxImage_SetDataBuffer(self, *_args, **_kwargs)
        return val
    def SetData(self, *_args, **_kwargs):
        val = imagec.wxImage_SetData(self, *_args, **_kwargs)
        return val
    def SetMaskColour(self, *_args, **_kwargs):
        val = imagec.wxImage_SetMaskColour(self, *_args, **_kwargs)
        return val
    def GetMaskRed(self, *_args, **_kwargs):
        val = imagec.wxImage_GetMaskRed(self, *_args, **_kwargs)
        return val
    def GetMaskGreen(self, *_args, **_kwargs):
        val = imagec.wxImage_GetMaskGreen(self, *_args, **_kwargs)
        return val
    def GetMaskBlue(self, *_args, **_kwargs):
        val = imagec.wxImage_GetMaskBlue(self, *_args, **_kwargs)
        return val
    def SetMask(self, *_args, **_kwargs):
        val = imagec.wxImage_SetMask(self, *_args, **_kwargs)
        return val
    def HasMask(self, *_args, **_kwargs):
        val = imagec.wxImage_HasMask(self, *_args, **_kwargs)
        return val
    def Rotate(self, *_args, **_kwargs):
        val = imagec.wxImage_Rotate(self, *_args, **_kwargs)
        if val: val = wxImagePtr(val) ; val.thisown = 1
        return val
    def Rotate90(self, *_args, **_kwargs):
        val = imagec.wxImage_Rotate90(self, *_args, **_kwargs)
        if val: val = wxImagePtr(val) ; val.thisown = 1
        return val
    def Mirror(self, *_args, **_kwargs):
        val = imagec.wxImage_Mirror(self, *_args, **_kwargs)
        if val: val = wxImagePtr(val) ; val.thisown = 1
        return val
    def Replace(self, *_args, **_kwargs):
        val = imagec.wxImage_Replace(self, *_args, **_kwargs)
        return val
    def ConvertToMono(self, *_args, **_kwargs):
        val = imagec.wxImage_ConvertToMono(self, *_args, **_kwargs)
        if val: val = wxImagePtr(val) ; val.thisown = 1
        return val
    def SetOption(self, *_args, **_kwargs):
        val = imagec.wxImage_SetOption(self, *_args, **_kwargs)
        return val
    def SetOptionInt(self, *_args, **_kwargs):
        val = imagec.wxImage_SetOptionInt(self, *_args, **_kwargs)
        return val
    def GetOption(self, *_args, **_kwargs):
        val = imagec.wxImage_GetOption(self, *_args, **_kwargs)
        return val
    def GetOptionInt(self, *_args, **_kwargs):
        val = imagec.wxImage_GetOptionInt(self, *_args, **_kwargs)
        return val
    def HasOption(self, *_args, **_kwargs):
        val = imagec.wxImage_HasOption(self, *_args, **_kwargs)
        return val
    def CountColours(self, *_args, **_kwargs):
        val = imagec.wxImage_CountColours(self, *_args, **_kwargs)
        return val
    def ConvertToBitmap(self, *_args, **_kwargs):
        val = imagec.wxImage_ConvertToBitmap(self, *_args, **_kwargs)
        if val: val = wxBitmapPtr(val) ; val.thisown = 1
        return val
    def ConvertToMonoBitmap(self, *_args, **_kwargs):
        val = imagec.wxImage_ConvertToMonoBitmap(self, *_args, **_kwargs)
        if val: val = wxBitmapPtr(val) ; val.thisown = 1
        return val
    def __repr__(self):
        return "<%s.%s instance; proxy of C++ wxImage instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this)
    def __nonzero__(self): return self.Ok()
class wxImage(wxImagePtr):
    def __init__(self,*_args,**_kwargs):
        self.this = imagec.new_wxImage(*_args,**_kwargs)
        self.thisown = 1






#-------------- FUNCTION WRAPPERS ------------------

def wxEmptyImage(*_args, **_kwargs):
    val = imagec.wxEmptyImage(*_args,**_kwargs)
    if val: val = wxImagePtr(val); val.thisown = 1
    return val

def wxImageFromMime(*_args, **_kwargs):
    val = imagec.wxImageFromMime(*_args,**_kwargs)
    if val: val = wxImagePtr(val); val.thisown = 1
    return val

def wxImageFromBitmap(*_args, **_kwargs):
    val = imagec.wxImageFromBitmap(*_args,**_kwargs)
    if val: val = wxImagePtr(val); val.thisown = 1
    return val

def wxImageFromData(*_args, **_kwargs):
    val = imagec.wxImageFromData(*_args,**_kwargs)
    if val: val = wxImagePtr(val); val.thisown = 1
    return val

def wxImageFromStream(*_args, **_kwargs):
    val = imagec.wxImageFromStream(*_args,**_kwargs)
    if val: val = wxImagePtr(val); val.thisown = 1
    return val

def wxImageFromStreamMime(*_args, **_kwargs):
    val = imagec.wxImageFromStreamMime(*_args,**_kwargs)
    if val: val = wxImagePtr(val); val.thisown = 1
    return val

wxInitAllImageHandlers = imagec.wxInitAllImageHandlers

def wxBitmapFromImage(*_args, **_kwargs):
    val = imagec.wxBitmapFromImage(*_args,**_kwargs)
    if val: val = wxBitmapPtr(val); val.thisown = 1
    return val

wxImage_CanRead = imagec.wxImage_CanRead

wxImage_GetImageCount = imagec.wxImage_GetImageCount

wxImage_CanReadStream = imagec.wxImage_CanReadStream

wxImage_AddHandler = imagec.wxImage_AddHandler

wxImage_InsertHandler = imagec.wxImage_InsertHandler

wxImage_RemoveHandler = imagec.wxImage_RemoveHandler



#-------------- VARIABLE WRAPPERS ------------------

cvar = imagec.cvar
wxNullImage = wxImagePtr(imagec.cvar.wxNullImage)
