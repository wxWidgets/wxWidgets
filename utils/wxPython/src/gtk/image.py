# This file was created automatically by SWIG.
import imagec

from misc import *

from gdi import *
class wxImageHandlerPtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def GetName(self, *_args, **_kwargs):
        val = apply(imagec.wxImageHandler_GetName,(self,) + _args, _kwargs)
        return val
    def GetExtension(self, *_args, **_kwargs):
        val = apply(imagec.wxImageHandler_GetExtension,(self,) + _args, _kwargs)
        return val
    def GetType(self, *_args, **_kwargs):
        val = apply(imagec.wxImageHandler_GetType,(self,) + _args, _kwargs)
        return val
    def GetMimeType(self, *_args, **_kwargs):
        val = apply(imagec.wxImageHandler_GetMimeType,(self,) + _args, _kwargs)
        return val
    def SetName(self, *_args, **_kwargs):
        val = apply(imagec.wxImageHandler_SetName,(self,) + _args, _kwargs)
        return val
    def SetExtension(self, *_args, **_kwargs):
        val = apply(imagec.wxImageHandler_SetExtension,(self,) + _args, _kwargs)
        return val
    def SetType(self, *_args, **_kwargs):
        val = apply(imagec.wxImageHandler_SetType,(self,) + _args, _kwargs)
        return val
    def SetMimeType(self, *_args, **_kwargs):
        val = apply(imagec.wxImageHandler_SetMimeType,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxImageHandler instance at %s>" % (self.this,)
class wxImageHandler(wxImageHandlerPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(imagec.new_wxImageHandler,_args,_kwargs)
        self.thisown = 1




class wxPNGHandlerPtr(wxImageHandlerPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __repr__(self):
        return "<C wxPNGHandler instance at %s>" % (self.this,)
class wxPNGHandler(wxPNGHandlerPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(imagec.new_wxPNGHandler,_args,_kwargs)
        self.thisown = 1




class wxJPEGHandlerPtr(wxImageHandlerPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __repr__(self):
        return "<C wxJPEGHandler instance at %s>" % (self.this,)
class wxJPEGHandler(wxJPEGHandlerPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(imagec.new_wxJPEGHandler,_args,_kwargs)
        self.thisown = 1




class wxBMPHandlerPtr(wxImageHandlerPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __repr__(self):
        return "<C wxBMPHandler instance at %s>" % (self.this,)
class wxBMPHandler(wxBMPHandlerPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(imagec.new_wxBMPHandler,_args,_kwargs)
        self.thisown = 1




class wxGIFHandlerPtr(wxImageHandlerPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __repr__(self):
        return "<C wxGIFHandler instance at %s>" % (self.this,)
class wxGIFHandler(wxGIFHandlerPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(imagec.new_wxGIFHandler,_args,_kwargs)
        self.thisown = 1




class wxImagePtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __del__(self,imagec=imagec):
        if self.thisown == 1 :
            imagec.delete_wxImage(self)
    def ConvertToBitmap(self, *_args, **_kwargs):
        val = apply(imagec.wxImage_ConvertToBitmap,(self,) + _args, _kwargs)
        if val: val = wxBitmapPtr(val) ; val.thisown = 1
        return val
    def Create(self, *_args, **_kwargs):
        val = apply(imagec.wxImage_Create,(self,) + _args, _kwargs)
        return val
    def Destroy(self, *_args, **_kwargs):
        val = apply(imagec.wxImage_Destroy,(self,) + _args, _kwargs)
        return val
    def Scale(self, *_args, **_kwargs):
        val = apply(imagec.wxImage_Scale,(self,) + _args, _kwargs)
        if val: val = wxImagePtr(val) ; val.thisown = 1
        return val
    def Rescale(self, *_args, **_kwargs):
        val = apply(imagec.wxImage_Rescale,(self,) + _args, _kwargs)
        return val
    def SetRGB(self, *_args, **_kwargs):
        val = apply(imagec.wxImage_SetRGB,(self,) + _args, _kwargs)
        return val
    def GetRed(self, *_args, **_kwargs):
        val = apply(imagec.wxImage_GetRed,(self,) + _args, _kwargs)
        return val
    def GetGreen(self, *_args, **_kwargs):
        val = apply(imagec.wxImage_GetGreen,(self,) + _args, _kwargs)
        return val
    def GetBlue(self, *_args, **_kwargs):
        val = apply(imagec.wxImage_GetBlue,(self,) + _args, _kwargs)
        return val
    def LoadFile(self, *_args, **_kwargs):
        val = apply(imagec.wxImage_LoadFile,(self,) + _args, _kwargs)
        return val
    def LoadMimeFile(self, *_args, **_kwargs):
        val = apply(imagec.wxImage_LoadMimeFile,(self,) + _args, _kwargs)
        return val
    def SaveFile(self, *_args, **_kwargs):
        val = apply(imagec.wxImage_SaveFile,(self,) + _args, _kwargs)
        return val
    def SaveMimeFile(self, *_args, **_kwargs):
        val = apply(imagec.wxImage_SaveMimeFile,(self,) + _args, _kwargs)
        return val
    def Ok(self, *_args, **_kwargs):
        val = apply(imagec.wxImage_Ok,(self,) + _args, _kwargs)
        return val
    def GetWidth(self, *_args, **_kwargs):
        val = apply(imagec.wxImage_GetWidth,(self,) + _args, _kwargs)
        return val
    def GetHeight(self, *_args, **_kwargs):
        val = apply(imagec.wxImage_GetHeight,(self,) + _args, _kwargs)
        return val
    def GetData(self, *_args, **_kwargs):
        val = apply(imagec.wxImage_GetData,(self,) + _args, _kwargs)
        return val
    def SetData(self, *_args, **_kwargs):
        val = apply(imagec.wxImage_SetData,(self,) + _args, _kwargs)
        return val
    def SetMaskColour(self, *_args, **_kwargs):
        val = apply(imagec.wxImage_SetMaskColour,(self,) + _args, _kwargs)
        return val
    def GetMaskRed(self, *_args, **_kwargs):
        val = apply(imagec.wxImage_GetMaskRed,(self,) + _args, _kwargs)
        return val
    def GetMaskGreen(self, *_args, **_kwargs):
        val = apply(imagec.wxImage_GetMaskGreen,(self,) + _args, _kwargs)
        return val
    def GetMaskBlue(self, *_args, **_kwargs):
        val = apply(imagec.wxImage_GetMaskBlue,(self,) + _args, _kwargs)
        return val
    def SetMask(self, *_args, **_kwargs):
        val = apply(imagec.wxImage_SetMask,(self,) + _args, _kwargs)
        return val
    def HasMask(self, *_args, **_kwargs):
        val = apply(imagec.wxImage_HasMask,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxImage instance at %s>" % (self.this,)
class wxImage(wxImagePtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(imagec.new_wxImage,_args,_kwargs)
        self.thisown = 1






#-------------- FUNCTION WRAPPERS ------------------

def wxNullImage(*_args, **_kwargs):
    val = apply(imagec.wxNullImage,_args,_kwargs)
    if val: val = wxImagePtr(val); val.thisown = 1
    return val

def wxEmptyImage(*_args, **_kwargs):
    val = apply(imagec.wxEmptyImage,_args,_kwargs)
    if val: val = wxImagePtr(val); val.thisown = 1
    return val

def wxImageFromMime(*_args, **_kwargs):
    val = apply(imagec.wxImageFromMime,_args,_kwargs)
    if val: val = wxImagePtr(val); val.thisown = 1
    return val

def wxImageFromBitmap(*_args, **_kwargs):
    val = apply(imagec.wxImageFromBitmap,_args,_kwargs)
    if val: val = wxImagePtr(val); val.thisown = 1
    return val

wxImage_AddHandler = imagec.wxImage_AddHandler



#-------------- VARIABLE WRAPPERS ------------------

