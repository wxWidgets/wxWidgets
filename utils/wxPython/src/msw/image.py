# This file was created automatically by SWIG.
import imagec

from misc import *

from gdi import *
class wxImageHandlerPtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def GetName(self):
        val = imagec.wxImageHandler_GetName(self.this)
        return val
    def GetExtension(self):
        val = imagec.wxImageHandler_GetExtension(self.this)
        return val
    def GetType(self):
        val = imagec.wxImageHandler_GetType(self.this)
        return val
    def GetMimeType(self):
        val = imagec.wxImageHandler_GetMimeType(self.this)
        return val
    def SetName(self,arg0):
        val = imagec.wxImageHandler_SetName(self.this,arg0)
        return val
    def SetExtension(self,arg0):
        val = imagec.wxImageHandler_SetExtension(self.this,arg0)
        return val
    def SetType(self,arg0):
        val = imagec.wxImageHandler_SetType(self.this,arg0)
        return val
    def SetMimeType(self,arg0):
        val = imagec.wxImageHandler_SetMimeType(self.this,arg0)
        return val
    def __repr__(self):
        return "<C wxImageHandler instance>"
class wxImageHandler(wxImageHandlerPtr):
    def __init__(self) :
        self.this = imagec.new_wxImageHandler()
        self.thisown = 1




class wxPNGHandlerPtr(wxImageHandlerPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __repr__(self):
        return "<C wxPNGHandler instance>"
class wxPNGHandler(wxPNGHandlerPtr):
    def __init__(self) :
        self.this = imagec.new_wxPNGHandler()
        self.thisown = 1




class wxJPEGHandlerPtr(wxImageHandlerPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __repr__(self):
        return "<C wxJPEGHandler instance>"
class wxJPEGHandler(wxJPEGHandlerPtr):
    def __init__(self) :
        self.this = imagec.new_wxJPEGHandler()
        self.thisown = 1




class wxBMPHandlerPtr(wxImageHandlerPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __repr__(self):
        return "<C wxBMPHandler instance>"
class wxBMPHandler(wxBMPHandlerPtr):
    def __init__(self) :
        self.this = imagec.new_wxBMPHandler()
        self.thisown = 1




class wxGIFHandlerPtr(wxImageHandlerPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __repr__(self):
        return "<C wxGIFHandler instance>"
class wxGIFHandler(wxGIFHandlerPtr):
    def __init__(self) :
        self.this = imagec.new_wxGIFHandler()
        self.thisown = 1




class wxImagePtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __del__(self, imagec=imagec):
        if self.thisown == 1 :
            imagec.delete_wxImage(self.this)
    def ConvertToBitmap(self):
        val = imagec.wxImage_ConvertToBitmap(self.this)
        val = wxBitmapPtr(val)
        val.thisown = 1
        return val
    def Create(self,arg0,arg1):
        val = imagec.wxImage_Create(self.this,arg0,arg1)
        return val
    def Destroy(self):
        val = imagec.wxImage_Destroy(self.this)
        return val
    def Scale(self,arg0,arg1):
        val = imagec.wxImage_Scale(self.this,arg0,arg1)
        val = wxImagePtr(val)
        val.thisown = 1
        return val
    def Rescale(self,arg0,arg1):
        val = imagec.wxImage_Rescale(self.this,arg0,arg1)
        return val
    def SetRGB(self,arg0,arg1,arg2,arg3,arg4):
        val = imagec.wxImage_SetRGB(self.this,arg0,arg1,arg2,arg3,arg4)
        return val
    def GetRed(self,arg0,arg1):
        val = imagec.wxImage_GetRed(self.this,arg0,arg1)
        return val
    def GetGreen(self,arg0,arg1):
        val = imagec.wxImage_GetGreen(self.this,arg0,arg1)
        return val
    def GetBlue(self,arg0,arg1):
        val = imagec.wxImage_GetBlue(self.this,arg0,arg1)
        return val
    def LoadFile(self,arg0,*args):
        val = apply(imagec.wxImage_LoadFile,(self.this,arg0,)+args)
        return val
    def LoadMimeFile(self,arg0,arg1):
        val = imagec.wxImage_LoadMimeFile(self.this,arg0,arg1)
        return val
    def SaveFile(self,arg0,arg1):
        val = imagec.wxImage_SaveFile(self.this,arg0,arg1)
        return val
    def SaveMimeFile(self,arg0,arg1):
        val = imagec.wxImage_SaveMimeFile(self.this,arg0,arg1)
        return val
    def Ok(self):
        val = imagec.wxImage_Ok(self.this)
        return val
    def GetWidth(self):
        val = imagec.wxImage_GetWidth(self.this)
        return val
    def GetHeight(self):
        val = imagec.wxImage_GetHeight(self.this)
        return val
    def GetData(self):
        val = imagec.wxImage_GetData(self.this)
        return val
    def SetData(self,arg0):
        val = imagec.wxImage_SetData(self.this,arg0)
        return val
    def SetMaskColour(self,arg0,arg1,arg2):
        val = imagec.wxImage_SetMaskColour(self.this,arg0,arg1,arg2)
        return val
    def GetMaskRed(self):
        val = imagec.wxImage_GetMaskRed(self.this)
        return val
    def GetMaskGreen(self):
        val = imagec.wxImage_GetMaskGreen(self.this)
        return val
    def GetMaskBlue(self):
        val = imagec.wxImage_GetMaskBlue(self.this)
        return val
    def SetMask(self,*args):
        val = apply(imagec.wxImage_SetMask,(self.this,)+args)
        return val
    def HasMask(self):
        val = imagec.wxImage_HasMask(self.this)
        return val
    def __repr__(self):
        return "<C wxImage instance>"
class wxImage(wxImagePtr):
    def __init__(self,arg0,*args) :
        self.this = apply(imagec.new_wxImage,(arg0,)+args)
        self.thisown = 1






#-------------- FUNCTION WRAPPERS ------------------

def wxNullImage():
    val = imagec.wxNullImage()
    val = wxImagePtr(val)
    val.thisown = 1
    return val

def wxEmptyImage(arg0,arg1):
    val = imagec.wxEmptyImage(arg0,arg1)
    val = wxImagePtr(val)
    val.thisown = 1
    return val

def wxImageFromMime(arg0,arg1):
    val = imagec.wxImageFromMime(arg0,arg1)
    val = wxImagePtr(val)
    val.thisown = 1
    return val

def wxImageFromBitmap(arg0):
    val = imagec.wxImageFromBitmap(arg0.this)
    val = wxImagePtr(val)
    val.thisown = 1
    return val

def wxImage_AddHandler(arg0):
    val = imagec.wxImage_AddHandler(arg0.this)
    return val



#-------------- VARIABLE WRAPPERS ------------------

