# This file was created automatically by SWIG.
import gdic

from misc import *
class wxGDIObjectPtr(wxObjectPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __del__(self,gdic=gdic):
        if self.thisown == 1 :
            gdic.delete_wxGDIObject(self)
    def GetVisible(self, *_args, **_kwargs):
        val = apply(gdic.wxGDIObject_GetVisible,(self,) + _args, _kwargs)
        return val
    def SetVisible(self, *_args, **_kwargs):
        val = apply(gdic.wxGDIObject_SetVisible,(self,) + _args, _kwargs)
        return val
    def IsNull(self, *_args, **_kwargs):
        val = apply(gdic.wxGDIObject_IsNull,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxGDIObject instance at %s>" % (self.this,)
class wxGDIObject(wxGDIObjectPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(gdic.new_wxGDIObject,_args,_kwargs)
        self.thisown = 1




class wxBitmapPtr(wxGDIObjectPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __del__(self,gdic=gdic):
        if self.thisown == 1 :
            gdic.delete_wxBitmap(self)
    def GetPalette(self, *_args, **_kwargs):
        val = apply(gdic.wxBitmap_GetPalette,(self,) + _args, _kwargs)
        if val: val = wxPalettePtr(val) 
        return val
    def GetMask(self, *_args, **_kwargs):
        val = apply(gdic.wxBitmap_GetMask,(self,) + _args, _kwargs)
        if val: val = wxMaskPtr(val) 
        return val
    def LoadFile(self, *_args, **_kwargs):
        val = apply(gdic.wxBitmap_LoadFile,(self,) + _args, _kwargs)
        return val
    def SaveFile(self, *_args, **_kwargs):
        val = apply(gdic.wxBitmap_SaveFile,(self,) + _args, _kwargs)
        return val
    def SetMask(self, *_args, **_kwargs):
        val = apply(gdic.wxBitmap_SetMask,(self,) + _args, _kwargs)
        return val
    def Ok(self, *_args, **_kwargs):
        val = apply(gdic.wxBitmap_Ok,(self,) + _args, _kwargs)
        return val
    def GetWidth(self, *_args, **_kwargs):
        val = apply(gdic.wxBitmap_GetWidth,(self,) + _args, _kwargs)
        return val
    def GetHeight(self, *_args, **_kwargs):
        val = apply(gdic.wxBitmap_GetHeight,(self,) + _args, _kwargs)
        return val
    def GetDepth(self, *_args, **_kwargs):
        val = apply(gdic.wxBitmap_GetDepth,(self,) + _args, _kwargs)
        return val
    def SetWidth(self, *_args, **_kwargs):
        val = apply(gdic.wxBitmap_SetWidth,(self,) + _args, _kwargs)
        return val
    def SetHeight(self, *_args, **_kwargs):
        val = apply(gdic.wxBitmap_SetHeight,(self,) + _args, _kwargs)
        return val
    def SetDepth(self, *_args, **_kwargs):
        val = apply(gdic.wxBitmap_SetDepth,(self,) + _args, _kwargs)
        return val
    def GetSubBitmap(self, *_args, **_kwargs):
        val = apply(gdic.wxBitmap_GetSubBitmap,(self,) + _args, _kwargs)
        if val: val = wxBitmapPtr(val) ; val.thisown = 1
        return val
    def CopyFromIcon(self, *_args, **_kwargs):
        val = apply(gdic.wxBitmap_CopyFromIcon,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxBitmap instance at %s>" % (self.this,)
    
    def __del__(self,gdic=gdic):
        try:
            if self.thisown == 1 :
                gdic.delete_wxBitmap(self)
        except:
            pass

class wxBitmap(wxBitmapPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(gdic.new_wxBitmap,_args,_kwargs)
        self.thisown = 1




class wxMaskPtr(wxObjectPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def Destroy(self, *_args, **_kwargs):
        val = apply(gdic.wxMask_Destroy,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxMask instance at %s>" % (self.this,)
class wxMask(wxMaskPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(gdic.new_wxMask,_args,_kwargs)
        self.thisown = 1




class wxIconPtr(wxGDIObjectPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __del__(self,gdic=gdic):
        if self.thisown == 1 :
            gdic.delete_wxIcon(self)
    def LoadFile(self, *_args, **_kwargs):
        val = apply(gdic.wxIcon_LoadFile,(self,) + _args, _kwargs)
        return val
    def Ok(self, *_args, **_kwargs):
        val = apply(gdic.wxIcon_Ok,(self,) + _args, _kwargs)
        return val
    def GetWidth(self, *_args, **_kwargs):
        val = apply(gdic.wxIcon_GetWidth,(self,) + _args, _kwargs)
        return val
    def GetHeight(self, *_args, **_kwargs):
        val = apply(gdic.wxIcon_GetHeight,(self,) + _args, _kwargs)
        return val
    def GetDepth(self, *_args, **_kwargs):
        val = apply(gdic.wxIcon_GetDepth,(self,) + _args, _kwargs)
        return val
    def SetWidth(self, *_args, **_kwargs):
        val = apply(gdic.wxIcon_SetWidth,(self,) + _args, _kwargs)
        return val
    def SetHeight(self, *_args, **_kwargs):
        val = apply(gdic.wxIcon_SetHeight,(self,) + _args, _kwargs)
        return val
    def SetDepth(self, *_args, **_kwargs):
        val = apply(gdic.wxIcon_SetDepth,(self,) + _args, _kwargs)
        return val
    def CopyFromBitmap(self, *_args, **_kwargs):
        val = apply(gdic.wxIcon_CopyFromBitmap,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxIcon instance at %s>" % (self.this,)
    
    def __del__(self,gdic=gdic):
        try:
            if self.thisown == 1 :
                gdic.delete_wxIcon(self)
        except:
            pass

class wxIcon(wxIconPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(gdic.new_wxIcon,_args,_kwargs)
        self.thisown = 1




class wxCursorPtr(wxGDIObjectPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __del__(self,gdic=gdic):
        if self.thisown == 1 :
            gdic.delete_wxCursor(self)
    def Ok(self, *_args, **_kwargs):
        val = apply(gdic.wxCursor_Ok,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxCursor instance at %s>" % (self.this,)
class wxCursor(wxCursorPtr):
    def __init__(self,this):
        self.this = this




class wxNativeFontInfoPtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def FromXFontName(self, *_args, **_kwargs):
        val = apply(gdic.wxNativeFontInfo_FromXFontName,(self,) + _args, _kwargs)
        return val
    def GetXFontName(self, *_args, **_kwargs):
        val = apply(gdic.wxNativeFontInfo_GetXFontName,(self,) + _args, _kwargs)
        return val
    def Init(self, *_args, **_kwargs):
        val = apply(gdic.wxNativeFontInfo_Init,(self,) + _args, _kwargs)
        return val
    def FromString(self, *_args, **_kwargs):
        val = apply(gdic.wxNativeFontInfo_FromString,(self,) + _args, _kwargs)
        return val
    def ToString(self, *_args, **_kwargs):
        val = apply(gdic.wxNativeFontInfo_ToString,(self,) + _args, _kwargs)
        return val
    def __str__(self, *_args, **_kwargs):
        val = apply(gdic.wxNativeFontInfo___str__,(self,) + _args, _kwargs)
        return val
    def FromUserString(self, *_args, **_kwargs):
        val = apply(gdic.wxNativeFontInfo_FromUserString,(self,) + _args, _kwargs)
        return val
    def ToUserString(self, *_args, **_kwargs):
        val = apply(gdic.wxNativeFontInfo_ToUserString,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxNativeFontInfo instance at %s>" % (self.this,)
class wxNativeFontInfo(wxNativeFontInfoPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(gdic.new_wxNativeFontInfo,_args,_kwargs)
        self.thisown = 1




class wxFontMapperPtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __del__(self,gdic=gdic):
        if self.thisown == 1 :
            gdic.delete_wxFontMapper(self)
    def GetAltForEncoding(self, *_args, **_kwargs):
        val = apply(gdic.wxFontMapper_GetAltForEncoding,(self,) + _args, _kwargs)
        return val
    def IsEncodingAvailable(self, *_args, **_kwargs):
        val = apply(gdic.wxFontMapper_IsEncodingAvailable,(self,) + _args, _kwargs)
        return val
    def CharsetToEncoding(self, *_args, **_kwargs):
        val = apply(gdic.wxFontMapper_CharsetToEncoding,(self,) + _args, _kwargs)
        return val
    def SetDialogParent(self, *_args, **_kwargs):
        val = apply(gdic.wxFontMapper_SetDialogParent,(self,) + _args, _kwargs)
        return val
    def SetDialogTitle(self, *_args, **_kwargs):
        val = apply(gdic.wxFontMapper_SetDialogTitle,(self,) + _args, _kwargs)
        return val
    def SetConfig(self, *_args, **_kwargs):
        val = apply(gdic.wxFontMapper_SetConfig,(self,) + _args, _kwargs)
        return val
    def SetConfigPath(self, *_args, **_kwargs):
        val = apply(gdic.wxFontMapper_SetConfigPath,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxFontMapper instance at %s>" % (self.this,)
class wxFontMapper(wxFontMapperPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(gdic.new_wxFontMapper,_args,_kwargs)
        self.thisown = 1




class wxFontPtr(wxGDIObjectPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __del__(self,gdic=gdic):
        if self.thisown == 1 :
            gdic.delete_wxFont(self)
    def Ok(self, *_args, **_kwargs):
        val = apply(gdic.wxFont_Ok,(self,) + _args, _kwargs)
        return val
    def GetPointSize(self, *_args, **_kwargs):
        val = apply(gdic.wxFont_GetPointSize,(self,) + _args, _kwargs)
        return val
    def GetFamily(self, *_args, **_kwargs):
        val = apply(gdic.wxFont_GetFamily,(self,) + _args, _kwargs)
        return val
    def GetStyle(self, *_args, **_kwargs):
        val = apply(gdic.wxFont_GetStyle,(self,) + _args, _kwargs)
        return val
    def GetWeight(self, *_args, **_kwargs):
        val = apply(gdic.wxFont_GetWeight,(self,) + _args, _kwargs)
        return val
    def GetUnderlined(self, *_args, **_kwargs):
        val = apply(gdic.wxFont_GetUnderlined,(self,) + _args, _kwargs)
        return val
    def GetFaceName(self, *_args, **_kwargs):
        val = apply(gdic.wxFont_GetFaceName,(self,) + _args, _kwargs)
        return val
    def GetEncoding(self, *_args, **_kwargs):
        val = apply(gdic.wxFont_GetEncoding,(self,) + _args, _kwargs)
        return val
    def GetNativeFontInfo(self, *_args, **_kwargs):
        val = apply(gdic.wxFont_GetNativeFontInfo,(self,) + _args, _kwargs)
        if val: val = wxNativeFontInfoPtr(val) 
        return val
    def GetNativeFontInfoDesc(self, *_args, **_kwargs):
        val = apply(gdic.wxFont_GetNativeFontInfoDesc,(self,) + _args, _kwargs)
        return val
    def GetNativeFontInfoUserDesc(self, *_args, **_kwargs):
        val = apply(gdic.wxFont_GetNativeFontInfoUserDesc,(self,) + _args, _kwargs)
        return val
    def SetPointSize(self, *_args, **_kwargs):
        val = apply(gdic.wxFont_SetPointSize,(self,) + _args, _kwargs)
        return val
    def SetFamily(self, *_args, **_kwargs):
        val = apply(gdic.wxFont_SetFamily,(self,) + _args, _kwargs)
        return val
    def SetStyle(self, *_args, **_kwargs):
        val = apply(gdic.wxFont_SetStyle,(self,) + _args, _kwargs)
        return val
    def SetWeight(self, *_args, **_kwargs):
        val = apply(gdic.wxFont_SetWeight,(self,) + _args, _kwargs)
        return val
    def SetFaceName(self, *_args, **_kwargs):
        val = apply(gdic.wxFont_SetFaceName,(self,) + _args, _kwargs)
        return val
    def SetUnderlined(self, *_args, **_kwargs):
        val = apply(gdic.wxFont_SetUnderlined,(self,) + _args, _kwargs)
        return val
    def SetEncoding(self, *_args, **_kwargs):
        val = apply(gdic.wxFont_SetEncoding,(self,) + _args, _kwargs)
        return val
    def SetNativeFontInfo(self, *_args, **_kwargs):
        val = apply(gdic.wxFont_SetNativeFontInfo,(self,) + _args, _kwargs)
        return val
    def SetNativeFontInfoUserDesc(self, *_args, **_kwargs):
        val = apply(gdic.wxFont_SetNativeFontInfoUserDesc,(self,) + _args, _kwargs)
        return val
    def GetFamilyString(self, *_args, **_kwargs):
        val = apply(gdic.wxFont_GetFamilyString,(self,) + _args, _kwargs)
        return val
    def GetStyleString(self, *_args, **_kwargs):
        val = apply(gdic.wxFont_GetStyleString,(self,) + _args, _kwargs)
        return val
    def GetWeightString(self, *_args, **_kwargs):
        val = apply(gdic.wxFont_GetWeightString,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxFont instance at %s>" % (self.this,)
class wxFont(wxFontPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(gdic.new_wxFont,_args,_kwargs)
        self.thisown = 1



def wxFontFromNativeInfo(*_args,**_kwargs):
    val = wxFontPtr(apply(gdic.new_wxFontFromNativeInfo,_args,_kwargs))
    val.thisown = 1
    return val


class wxFontListPtr(wxObjectPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def AddFont(self, *_args, **_kwargs):
        val = apply(gdic.wxFontList_AddFont,(self,) + _args, _kwargs)
        return val
    def FindOrCreateFont(self, *_args, **_kwargs):
        val = apply(gdic.wxFontList_FindOrCreateFont,(self,) + _args, _kwargs)
        if val: val = wxFontPtr(val) 
        return val
    def RemoveFont(self, *_args, **_kwargs):
        val = apply(gdic.wxFontList_RemoveFont,(self,) + _args, _kwargs)
        return val
    def GetCount(self, *_args, **_kwargs):
        val = apply(gdic.wxFontList_GetCount,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxFontList instance at %s>" % (self.this,)
class wxFontList(wxFontListPtr):
    def __init__(self,this):
        self.this = this




class wxColourPtr(wxObjectPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __del__(self,gdic=gdic):
        if self.thisown == 1 :
            gdic.delete_wxColour(self)
    def Red(self, *_args, **_kwargs):
        val = apply(gdic.wxColour_Red,(self,) + _args, _kwargs)
        return val
    def Green(self, *_args, **_kwargs):
        val = apply(gdic.wxColour_Green,(self,) + _args, _kwargs)
        return val
    def Blue(self, *_args, **_kwargs):
        val = apply(gdic.wxColour_Blue,(self,) + _args, _kwargs)
        return val
    def Ok(self, *_args, **_kwargs):
        val = apply(gdic.wxColour_Ok,(self,) + _args, _kwargs)
        return val
    def Set(self, *_args, **_kwargs):
        val = apply(gdic.wxColour_Set,(self,) + _args, _kwargs)
        return val
    def Get(self, *_args, **_kwargs):
        val = apply(gdic.wxColour_Get,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxColour instance at %s>" % (self.this,)
    asTuple = Get
    def __str__(self): return str(self.asTuple())
    def __repr__(self): return str(self.asTuple())
class wxColour(wxColourPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(gdic.new_wxColour,_args,_kwargs)
        self.thisown = 1




class wxColourDatabasePtr(wxObjectPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def FindColour(self, *_args, **_kwargs):
        val = apply(gdic.wxColourDatabase_FindColour,(self,) + _args, _kwargs)
        if val: val = wxColourPtr(val) 
        return val
    def FindName(self, *_args, **_kwargs):
        val = apply(gdic.wxColourDatabase_FindName,(self,) + _args, _kwargs)
        return val
    def Append(self, *_args, **_kwargs):
        val = apply(gdic.wxColourDatabase_Append,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxColourDatabase instance at %s>" % (self.this,)
class wxColourDatabase(wxColourDatabasePtr):
    def __init__(self,this):
        self.this = this




class wxPenPtr(wxGDIObjectPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __del__(self,gdic=gdic):
        if self.thisown == 1 :
            gdic.delete_wxPen(self)
    def GetCap(self, *_args, **_kwargs):
        val = apply(gdic.wxPen_GetCap,(self,) + _args, _kwargs)
        return val
    def GetColour(self, *_args, **_kwargs):
        val = apply(gdic.wxPen_GetColour,(self,) + _args, _kwargs)
        if val: val = wxColourPtr(val) ; val.thisown = 1
        return val
    def GetJoin(self, *_args, **_kwargs):
        val = apply(gdic.wxPen_GetJoin,(self,) + _args, _kwargs)
        return val
    def GetStyle(self, *_args, **_kwargs):
        val = apply(gdic.wxPen_GetStyle,(self,) + _args, _kwargs)
        return val
    def GetWidth(self, *_args, **_kwargs):
        val = apply(gdic.wxPen_GetWidth,(self,) + _args, _kwargs)
        return val
    def Ok(self, *_args, **_kwargs):
        val = apply(gdic.wxPen_Ok,(self,) + _args, _kwargs)
        return val
    def SetCap(self, *_args, **_kwargs):
        val = apply(gdic.wxPen_SetCap,(self,) + _args, _kwargs)
        return val
    def SetColour(self, *_args, **_kwargs):
        val = apply(gdic.wxPen_SetColour,(self,) + _args, _kwargs)
        return val
    def SetJoin(self, *_args, **_kwargs):
        val = apply(gdic.wxPen_SetJoin,(self,) + _args, _kwargs)
        return val
    def SetStyle(self, *_args, **_kwargs):
        val = apply(gdic.wxPen_SetStyle,(self,) + _args, _kwargs)
        return val
    def SetWidth(self, *_args, **_kwargs):
        val = apply(gdic.wxPen_SetWidth,(self,) + _args, _kwargs)
        return val
    def SetDashes(self, *_args, **_kwargs):
        val = apply(gdic.wxPen_SetDashes,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxPen instance at %s>" % (self.this,)
class wxPen(wxPenPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(gdic.new_wxPen,_args,_kwargs)
        self.thisown = 1




class wxPyPenPtr(wxPenPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __del__(self,gdic=gdic):
        if self.thisown == 1 :
            gdic.delete_wxPyPen(self)
    def SetDashes(self, *_args, **_kwargs):
        val = apply(gdic.wxPyPen_SetDashes,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxPyPen instance at %s>" % (self.this,)
class wxPyPen(wxPyPenPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(gdic.new_wxPyPen,_args,_kwargs)
        self.thisown = 1




class wxPenListPtr(wxObjectPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def AddPen(self, *_args, **_kwargs):
        val = apply(gdic.wxPenList_AddPen,(self,) + _args, _kwargs)
        return val
    def FindOrCreatePen(self, *_args, **_kwargs):
        val = apply(gdic.wxPenList_FindOrCreatePen,(self,) + _args, _kwargs)
        if val: val = wxPenPtr(val) 
        return val
    def RemovePen(self, *_args, **_kwargs):
        val = apply(gdic.wxPenList_RemovePen,(self,) + _args, _kwargs)
        return val
    def GetCount(self, *_args, **_kwargs):
        val = apply(gdic.wxPenList_GetCount,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxPenList instance at %s>" % (self.this,)
class wxPenList(wxPenListPtr):
    def __init__(self,this):
        self.this = this




class wxBrushPtr(wxGDIObjectPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __del__(self,gdic=gdic):
        if self.thisown == 1 :
            gdic.delete_wxBrush(self)
    def GetColour(self, *_args, **_kwargs):
        val = apply(gdic.wxBrush_GetColour,(self,) + _args, _kwargs)
        if val: val = wxColourPtr(val) ; val.thisown = 1
        return val
    def GetStipple(self, *_args, **_kwargs):
        val = apply(gdic.wxBrush_GetStipple,(self,) + _args, _kwargs)
        if val: val = wxBitmapPtr(val) 
        return val
    def GetStyle(self, *_args, **_kwargs):
        val = apply(gdic.wxBrush_GetStyle,(self,) + _args, _kwargs)
        return val
    def Ok(self, *_args, **_kwargs):
        val = apply(gdic.wxBrush_Ok,(self,) + _args, _kwargs)
        return val
    def SetColour(self, *_args, **_kwargs):
        val = apply(gdic.wxBrush_SetColour,(self,) + _args, _kwargs)
        return val
    def SetStipple(self, *_args, **_kwargs):
        val = apply(gdic.wxBrush_SetStipple,(self,) + _args, _kwargs)
        return val
    def SetStyle(self, *_args, **_kwargs):
        val = apply(gdic.wxBrush_SetStyle,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxBrush instance at %s>" % (self.this,)
class wxBrush(wxBrushPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(gdic.new_wxBrush,_args,_kwargs)
        self.thisown = 1




class wxBrushListPtr(wxObjectPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def AddBrush(self, *_args, **_kwargs):
        val = apply(gdic.wxBrushList_AddBrush,(self,) + _args, _kwargs)
        return val
    def FindOrCreateBrush(self, *_args, **_kwargs):
        val = apply(gdic.wxBrushList_FindOrCreateBrush,(self,) + _args, _kwargs)
        if val: val = wxBrushPtr(val) 
        return val
    def RemoveBrush(self, *_args, **_kwargs):
        val = apply(gdic.wxBrushList_RemoveBrush,(self,) + _args, _kwargs)
        return val
    def GetCount(self, *_args, **_kwargs):
        val = apply(gdic.wxBrushList_GetCount,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxBrushList instance at %s>" % (self.this,)
class wxBrushList(wxBrushListPtr):
    def __init__(self,this):
        self.this = this




class wxDCPtr(wxObjectPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __del__(self,gdic=gdic):
        if self.thisown == 1 :
            gdic.delete_wxDC(self)
    def BeginDrawing(self, *_args, **_kwargs):
        val = apply(gdic.wxDC_BeginDrawing,(self,) + _args, _kwargs)
        return val
    def Blit(self, *_args, **_kwargs):
        val = apply(gdic.wxDC_Blit,(self,) + _args, _kwargs)
        return val
    def Clear(self, *_args, **_kwargs):
        val = apply(gdic.wxDC_Clear,(self,) + _args, _kwargs)
        return val
    def CrossHair(self, *_args, **_kwargs):
        val = apply(gdic.wxDC_CrossHair,(self,) + _args, _kwargs)
        return val
    def DestroyClippingRegion(self, *_args, **_kwargs):
        val = apply(gdic.wxDC_DestroyClippingRegion,(self,) + _args, _kwargs)
        return val
    def DeviceToLogicalX(self, *_args, **_kwargs):
        val = apply(gdic.wxDC_DeviceToLogicalX,(self,) + _args, _kwargs)
        return val
    def DeviceToLogicalXRel(self, *_args, **_kwargs):
        val = apply(gdic.wxDC_DeviceToLogicalXRel,(self,) + _args, _kwargs)
        return val
    def DeviceToLogicalY(self, *_args, **_kwargs):
        val = apply(gdic.wxDC_DeviceToLogicalY,(self,) + _args, _kwargs)
        return val
    def DeviceToLogicalYRel(self, *_args, **_kwargs):
        val = apply(gdic.wxDC_DeviceToLogicalYRel,(self,) + _args, _kwargs)
        return val
    def DrawArc(self, *_args, **_kwargs):
        val = apply(gdic.wxDC_DrawArc,(self,) + _args, _kwargs)
        return val
    def DrawCircle(self, *_args, **_kwargs):
        val = apply(gdic.wxDC_DrawCircle,(self,) + _args, _kwargs)
        return val
    def DrawEllipse(self, *_args, **_kwargs):
        val = apply(gdic.wxDC_DrawEllipse,(self,) + _args, _kwargs)
        return val
    def DrawEllipticArc(self, *_args, **_kwargs):
        val = apply(gdic.wxDC_DrawEllipticArc,(self,) + _args, _kwargs)
        return val
    def DrawIcon(self, *_args, **_kwargs):
        val = apply(gdic.wxDC_DrawIcon,(self,) + _args, _kwargs)
        return val
    def DrawLine(self, *_args, **_kwargs):
        val = apply(gdic.wxDC_DrawLine,(self,) + _args, _kwargs)
        return val
    def DrawLines(self, *_args, **_kwargs):
        val = apply(gdic.wxDC_DrawLines,(self,) + _args, _kwargs)
        return val
    def DrawPolygon(self, *_args, **_kwargs):
        val = apply(gdic.wxDC_DrawPolygon,(self,) + _args, _kwargs)
        return val
    def DrawPoint(self, *_args, **_kwargs):
        val = apply(gdic.wxDC_DrawPoint,(self,) + _args, _kwargs)
        return val
    def DrawRectangle(self, *_args, **_kwargs):
        val = apply(gdic.wxDC_DrawRectangle,(self,) + _args, _kwargs)
        return val
    def DrawRotatedText(self, *_args, **_kwargs):
        val = apply(gdic.wxDC_DrawRotatedText,(self,) + _args, _kwargs)
        return val
    def DrawRoundedRectangle(self, *_args, **_kwargs):
        val = apply(gdic.wxDC_DrawRoundedRectangle,(self,) + _args, _kwargs)
        return val
    def DrawSpline(self, *_args, **_kwargs):
        val = apply(gdic.wxDC_DrawSpline,(self,) + _args, _kwargs)
        return val
    def DrawText(self, *_args, **_kwargs):
        val = apply(gdic.wxDC_DrawText,(self,) + _args, _kwargs)
        return val
    def EndDoc(self, *_args, **_kwargs):
        val = apply(gdic.wxDC_EndDoc,(self,) + _args, _kwargs)
        return val
    def EndDrawing(self, *_args, **_kwargs):
        val = apply(gdic.wxDC_EndDrawing,(self,) + _args, _kwargs)
        return val
    def EndPage(self, *_args, **_kwargs):
        val = apply(gdic.wxDC_EndPage,(self,) + _args, _kwargs)
        return val
    def FloodFill(self, *_args, **_kwargs):
        val = apply(gdic.wxDC_FloodFill,(self,) + _args, _kwargs)
        return val
    def GetBackground(self, *_args, **_kwargs):
        val = apply(gdic.wxDC_GetBackground,(self,) + _args, _kwargs)
        if val: val = wxBrushPtr(val) ; val.thisown = 1
        return val
    def GetBrush(self, *_args, **_kwargs):
        val = apply(gdic.wxDC_GetBrush,(self,) + _args, _kwargs)
        if val: val = wxBrushPtr(val) ; val.thisown = 1
        return val
    def GetCharHeight(self, *_args, **_kwargs):
        val = apply(gdic.wxDC_GetCharHeight,(self,) + _args, _kwargs)
        return val
    def GetCharWidth(self, *_args, **_kwargs):
        val = apply(gdic.wxDC_GetCharWidth,(self,) + _args, _kwargs)
        return val
    def GetClippingBox(self, *_args, **_kwargs):
        val = apply(gdic.wxDC_GetClippingBox,(self,) + _args, _kwargs)
        return val
    def GetFont(self, *_args, **_kwargs):
        val = apply(gdic.wxDC_GetFont,(self,) + _args, _kwargs)
        if val: val = wxFontPtr(val) ; val.thisown = 1
        return val
    def GetLogicalFunction(self, *_args, **_kwargs):
        val = apply(gdic.wxDC_GetLogicalFunction,(self,) + _args, _kwargs)
        return val
    def GetLogicalScale(self, *_args, **_kwargs):
        val = apply(gdic.wxDC_GetLogicalScale,(self,) + _args, _kwargs)
        return val
    def GetMapMode(self, *_args, **_kwargs):
        val = apply(gdic.wxDC_GetMapMode,(self,) + _args, _kwargs)
        return val
    def GetOptimization(self, *_args, **_kwargs):
        val = apply(gdic.wxDC_GetOptimization,(self,) + _args, _kwargs)
        return val
    def GetPen(self, *_args, **_kwargs):
        val = apply(gdic.wxDC_GetPen,(self,) + _args, _kwargs)
        if val: val = wxPenPtr(val) ; val.thisown = 1
        return val
    def GetPixel(self, *_args, **_kwargs):
        val = apply(gdic.wxDC_GetPixel,(self,) + _args, _kwargs)
        if val: val = wxColourPtr(val) ; val.thisown = 1
        return val
    def GetSizeTuple(self, *_args, **_kwargs):
        val = apply(gdic.wxDC_GetSizeTuple,(self,) + _args, _kwargs)
        return val
    def GetSize(self, *_args, **_kwargs):
        val = apply(gdic.wxDC_GetSize,(self,) + _args, _kwargs)
        if val: val = wxSizePtr(val) ; val.thisown = 1
        return val
    def GetSizeMM(self, *_args, **_kwargs):
        val = apply(gdic.wxDC_GetSizeMM,(self,) + _args, _kwargs)
        if val: val = wxSizePtr(val) ; val.thisown = 1
        return val
    def GetTextBackground(self, *_args, **_kwargs):
        val = apply(gdic.wxDC_GetTextBackground,(self,) + _args, _kwargs)
        if val: val = wxColourPtr(val) ; val.thisown = 1
        return val
    def GetTextExtent(self, *_args, **_kwargs):
        val = apply(gdic.wxDC_GetTextExtent,(self,) + _args, _kwargs)
        return val
    def GetFullTextExtent(self, *_args, **_kwargs):
        val = apply(gdic.wxDC_GetFullTextExtent,(self,) + _args, _kwargs)
        return val
    def GetTextForeground(self, *_args, **_kwargs):
        val = apply(gdic.wxDC_GetTextForeground,(self,) + _args, _kwargs)
        if val: val = wxColourPtr(val) ; val.thisown = 1
        return val
    def GetUserScale(self, *_args, **_kwargs):
        val = apply(gdic.wxDC_GetUserScale,(self,) + _args, _kwargs)
        return val
    def LogicalToDeviceX(self, *_args, **_kwargs):
        val = apply(gdic.wxDC_LogicalToDeviceX,(self,) + _args, _kwargs)
        return val
    def LogicalToDeviceXRel(self, *_args, **_kwargs):
        val = apply(gdic.wxDC_LogicalToDeviceXRel,(self,) + _args, _kwargs)
        return val
    def LogicalToDeviceY(self, *_args, **_kwargs):
        val = apply(gdic.wxDC_LogicalToDeviceY,(self,) + _args, _kwargs)
        return val
    def LogicalToDeviceYRel(self, *_args, **_kwargs):
        val = apply(gdic.wxDC_LogicalToDeviceYRel,(self,) + _args, _kwargs)
        return val
    def MaxX(self, *_args, **_kwargs):
        val = apply(gdic.wxDC_MaxX,(self,) + _args, _kwargs)
        return val
    def MaxY(self, *_args, **_kwargs):
        val = apply(gdic.wxDC_MaxY,(self,) + _args, _kwargs)
        return val
    def MinX(self, *_args, **_kwargs):
        val = apply(gdic.wxDC_MinX,(self,) + _args, _kwargs)
        return val
    def MinY(self, *_args, **_kwargs):
        val = apply(gdic.wxDC_MinY,(self,) + _args, _kwargs)
        return val
    def Ok(self, *_args, **_kwargs):
        val = apply(gdic.wxDC_Ok,(self,) + _args, _kwargs)
        return val
    def SetDeviceOrigin(self, *_args, **_kwargs):
        val = apply(gdic.wxDC_SetDeviceOrigin,(self,) + _args, _kwargs)
        return val
    def SetBackground(self, *_args, **_kwargs):
        val = apply(gdic.wxDC_SetBackground,(self,) + _args, _kwargs)
        return val
    def SetBackgroundMode(self, *_args, **_kwargs):
        val = apply(gdic.wxDC_SetBackgroundMode,(self,) + _args, _kwargs)
        return val
    def SetClippingRegion(self, *_args, **_kwargs):
        val = apply(gdic.wxDC_SetClippingRegion,(self,) + _args, _kwargs)
        return val
    def SetClippingRegionAsRegion(self, *_args, **_kwargs):
        val = apply(gdic.wxDC_SetClippingRegionAsRegion,(self,) + _args, _kwargs)
        return val
    def SetPalette(self, *_args, **_kwargs):
        val = apply(gdic.wxDC_SetPalette,(self,) + _args, _kwargs)
        return val
    def SetBrush(self, *_args, **_kwargs):
        val = apply(gdic.wxDC_SetBrush,(self,) + _args, _kwargs)
        return val
    def SetFont(self, *_args, **_kwargs):
        val = apply(gdic.wxDC_SetFont,(self,) + _args, _kwargs)
        return val
    def SetLogicalFunction(self, *_args, **_kwargs):
        val = apply(gdic.wxDC_SetLogicalFunction,(self,) + _args, _kwargs)
        return val
    def SetLogicalScale(self, *_args, **_kwargs):
        val = apply(gdic.wxDC_SetLogicalScale,(self,) + _args, _kwargs)
        return val
    def SetMapMode(self, *_args, **_kwargs):
        val = apply(gdic.wxDC_SetMapMode,(self,) + _args, _kwargs)
        return val
    def SetOptimization(self, *_args, **_kwargs):
        val = apply(gdic.wxDC_SetOptimization,(self,) + _args, _kwargs)
        return val
    def SetPen(self, *_args, **_kwargs):
        val = apply(gdic.wxDC_SetPen,(self,) + _args, _kwargs)
        return val
    def SetTextBackground(self, *_args, **_kwargs):
        val = apply(gdic.wxDC_SetTextBackground,(self,) + _args, _kwargs)
        return val
    def SetTextForeground(self, *_args, **_kwargs):
        val = apply(gdic.wxDC_SetTextForeground,(self,) + _args, _kwargs)
        return val
    def SetUserScale(self, *_args, **_kwargs):
        val = apply(gdic.wxDC_SetUserScale,(self,) + _args, _kwargs)
        return val
    def StartDoc(self, *_args, **_kwargs):
        val = apply(gdic.wxDC_StartDoc,(self,) + _args, _kwargs)
        return val
    def StartPage(self, *_args, **_kwargs):
        val = apply(gdic.wxDC_StartPage,(self,) + _args, _kwargs)
        return val
    def DrawBitmap(self, *_args, **_kwargs):
        val = apply(gdic.wxDC_DrawBitmap,(self,) + _args, _kwargs)
        return val
    def CanDrawBitmap(self, *_args, **_kwargs):
        val = apply(gdic.wxDC_CanDrawBitmap,(self,) + _args, _kwargs)
        return val
    def CanGetTextExtent(self, *_args, **_kwargs):
        val = apply(gdic.wxDC_CanGetTextExtent,(self,) + _args, _kwargs)
        return val
    def GetDepth(self, *_args, **_kwargs):
        val = apply(gdic.wxDC_GetDepth,(self,) + _args, _kwargs)
        return val
    def GetPPI(self, *_args, **_kwargs):
        val = apply(gdic.wxDC_GetPPI,(self,) + _args, _kwargs)
        if val: val = wxSizePtr(val) ; val.thisown = 1
        return val
    def GetLogicalOrigin(self, *_args, **_kwargs):
        val = apply(gdic.wxDC_GetLogicalOrigin,(self,) + _args, _kwargs)
        return val
    def SetLogicalOrigin(self, *_args, **_kwargs):
        val = apply(gdic.wxDC_SetLogicalOrigin,(self,) + _args, _kwargs)
        return val
    def GetDeviceOrigin(self, *_args, **_kwargs):
        val = apply(gdic.wxDC_GetDeviceOrigin,(self,) + _args, _kwargs)
        return val
    def SetAxisOrientation(self, *_args, **_kwargs):
        val = apply(gdic.wxDC_SetAxisOrientation,(self,) + _args, _kwargs)
        return val
    def CalcBoundingBox(self, *_args, **_kwargs):
        val = apply(gdic.wxDC_CalcBoundingBox,(self,) + _args, _kwargs)
        return val
    def ResetBoundingBox(self, *_args, **_kwargs):
        val = apply(gdic.wxDC_ResetBoundingBox,(self,) + _args, _kwargs)
        return val
    def GetBoundingBox(self, *_args, **_kwargs):
        val = apply(gdic.wxDC_GetBoundingBox,(self,) + _args, _kwargs)
        return val
    def _DrawPointList(self, *_args, **_kwargs):
        val = apply(gdic.wxDC__DrawPointList,(self,) + _args, _kwargs)
        return val
    def _DrawLineList(self, *_args, **_kwargs):
        val = apply(gdic.wxDC__DrawLineList,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxDC instance at %s>" % (self.this,)
    
    def DrawPointList(self, points, pens=None):
        if pens is None:
           pens = []
        elif isinstance(pens, wxPenPtr):
           pens = [pens]
        elif len(pens) != len(points):
           raise ValueError('points and pens must have same length')
        return self._DrawPointList(points, pens)

    def DrawLineList(self, lines, pens=None):
        if pens is None:
           pens = []
        elif isinstance(pens, wxPenPtr):
           pens = [pens]
        elif len(pens) != len(lines):
           raise ValueError('lines and pens must have same length')
        return self._DrawLineList(lines, pens)

class wxDC(wxDCPtr):
    def __init__(self,this):
        self.this = this




class wxMemoryDCPtr(wxDCPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def SelectObject(self, *_args, **_kwargs):
        val = apply(gdic.wxMemoryDC_SelectObject,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxMemoryDC instance at %s>" % (self.this,)
class wxMemoryDC(wxMemoryDCPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(gdic.new_wxMemoryDC,_args,_kwargs)
        self.thisown = 1




class wxScreenDCPtr(wxDCPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def StartDrawingOnTopWin(self, *_args, **_kwargs):
        val = apply(gdic.wxScreenDC_StartDrawingOnTopWin,(self,) + _args, _kwargs)
        return val
    def StartDrawingOnTop(self, *_args, **_kwargs):
        val = apply(gdic.wxScreenDC_StartDrawingOnTop,(self,) + _args, _kwargs)
        return val
    def EndDrawingOnTop(self, *_args, **_kwargs):
        val = apply(gdic.wxScreenDC_EndDrawingOnTop,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxScreenDC instance at %s>" % (self.this,)
class wxScreenDC(wxScreenDCPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(gdic.new_wxScreenDC,_args,_kwargs)
        self.thisown = 1




class wxClientDCPtr(wxDCPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __repr__(self):
        return "<C wxClientDC instance at %s>" % (self.this,)
class wxClientDC(wxClientDCPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(gdic.new_wxClientDC,_args,_kwargs)
        self.thisown = 1




class wxPaintDCPtr(wxDCPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __repr__(self):
        return "<C wxPaintDC instance at %s>" % (self.this,)
class wxPaintDC(wxPaintDCPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(gdic.new_wxPaintDC,_args,_kwargs)
        self.thisown = 1




class wxWindowDCPtr(wxDCPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __repr__(self):
        return "<C wxWindowDC instance at %s>" % (self.this,)
class wxWindowDC(wxWindowDCPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(gdic.new_wxWindowDC,_args,_kwargs)
        self.thisown = 1




class wxPalettePtr(wxGDIObjectPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __del__(self,gdic=gdic):
        if self.thisown == 1 :
            gdic.delete_wxPalette(self)
    def GetPixel(self, *_args, **_kwargs):
        val = apply(gdic.wxPalette_GetPixel,(self,) + _args, _kwargs)
        return val
    def GetRGB(self, *_args, **_kwargs):
        val = apply(gdic.wxPalette_GetRGB,(self,) + _args, _kwargs)
        return val
    def Ok(self, *_args, **_kwargs):
        val = apply(gdic.wxPalette_Ok,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxPalette instance at %s>" % (self.this,)
class wxPalette(wxPalettePtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(gdic.new_wxPalette,_args,_kwargs)
        self.thisown = 1




class wxImageListPtr(wxObjectPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __del__(self,gdic=gdic):
        if self.thisown == 1 :
            gdic.delete_wxImageList(self)
    def Add(self, *_args, **_kwargs):
        val = apply(gdic.wxImageList_Add,(self,) + _args, _kwargs)
        return val
    def AddWithColourMask(self, *_args, **_kwargs):
        val = apply(gdic.wxImageList_AddWithColourMask,(self,) + _args, _kwargs)
        return val
    def AddIcon(self, *_args, **_kwargs):
        val = apply(gdic.wxImageList_AddIcon,(self,) + _args, _kwargs)
        return val
    def Replace(self, *_args, **_kwargs):
        val = apply(gdic.wxImageList_Replace,(self,) + _args, _kwargs)
        return val
    def Draw(self, *_args, **_kwargs):
        val = apply(gdic.wxImageList_Draw,(self,) + _args, _kwargs)
        return val
    def GetImageCount(self, *_args, **_kwargs):
        val = apply(gdic.wxImageList_GetImageCount,(self,) + _args, _kwargs)
        return val
    def Remove(self, *_args, **_kwargs):
        val = apply(gdic.wxImageList_Remove,(self,) + _args, _kwargs)
        return val
    def RemoveAll(self, *_args, **_kwargs):
        val = apply(gdic.wxImageList_RemoveAll,(self,) + _args, _kwargs)
        return val
    def GetSize(self, *_args, **_kwargs):
        val = apply(gdic.wxImageList_GetSize,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxImageList instance at %s>" % (self.this,)
class wxImageList(wxImageListPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(gdic.new_wxImageList,_args,_kwargs)
        self.thisown = 1




class wxRegionPtr(wxGDIObjectPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __del__(self,gdic=gdic):
        if self.thisown == 1 :
            gdic.delete_wxRegion(self)
    def Clear(self, *_args, **_kwargs):
        val = apply(gdic.wxRegion_Clear,(self,) + _args, _kwargs)
        return val
    def Contains(self, *_args, **_kwargs):
        val = apply(gdic.wxRegion_Contains,(self,) + _args, _kwargs)
        return val
    def ContainsPoint(self, *_args, **_kwargs):
        val = apply(gdic.wxRegion_ContainsPoint,(self,) + _args, _kwargs)
        return val
    def ContainsRect(self, *_args, **_kwargs):
        val = apply(gdic.wxRegion_ContainsRect,(self,) + _args, _kwargs)
        return val
    def ContainsRectDim(self, *_args, **_kwargs):
        val = apply(gdic.wxRegion_ContainsRectDim,(self,) + _args, _kwargs)
        return val
    def GetBox(self, *_args, **_kwargs):
        val = apply(gdic.wxRegion_GetBox,(self,) + _args, _kwargs)
        if val: val = wxRectPtr(val) ; val.thisown = 1
        return val
    def Intersect(self, *_args, **_kwargs):
        val = apply(gdic.wxRegion_Intersect,(self,) + _args, _kwargs)
        return val
    def IntersectRect(self, *_args, **_kwargs):
        val = apply(gdic.wxRegion_IntersectRect,(self,) + _args, _kwargs)
        return val
    def IntersectRegion(self, *_args, **_kwargs):
        val = apply(gdic.wxRegion_IntersectRegion,(self,) + _args, _kwargs)
        return val
    def IsEmpty(self, *_args, **_kwargs):
        val = apply(gdic.wxRegion_IsEmpty,(self,) + _args, _kwargs)
        return val
    def Union(self, *_args, **_kwargs):
        val = apply(gdic.wxRegion_Union,(self,) + _args, _kwargs)
        return val
    def UnionRect(self, *_args, **_kwargs):
        val = apply(gdic.wxRegion_UnionRect,(self,) + _args, _kwargs)
        return val
    def UnionRegion(self, *_args, **_kwargs):
        val = apply(gdic.wxRegion_UnionRegion,(self,) + _args, _kwargs)
        return val
    def Subtract(self, *_args, **_kwargs):
        val = apply(gdic.wxRegion_Subtract,(self,) + _args, _kwargs)
        return val
    def SubtractRect(self, *_args, **_kwargs):
        val = apply(gdic.wxRegion_SubtractRect,(self,) + _args, _kwargs)
        return val
    def SubtractRegion(self, *_args, **_kwargs):
        val = apply(gdic.wxRegion_SubtractRegion,(self,) + _args, _kwargs)
        return val
    def Xor(self, *_args, **_kwargs):
        val = apply(gdic.wxRegion_Xor,(self,) + _args, _kwargs)
        return val
    def XorRect(self, *_args, **_kwargs):
        val = apply(gdic.wxRegion_XorRect,(self,) + _args, _kwargs)
        return val
    def XorRegion(self, *_args, **_kwargs):
        val = apply(gdic.wxRegion_XorRegion,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxRegion instance at %s>" % (self.this,)
class wxRegion(wxRegionPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(gdic.new_wxRegion,_args,_kwargs)
        self.thisown = 1




class wxRegionIteratorPtr(wxObjectPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __del__(self,gdic=gdic):
        if self.thisown == 1 :
            gdic.delete_wxRegionIterator(self)
    def GetX(self, *_args, **_kwargs):
        val = apply(gdic.wxRegionIterator_GetX,(self,) + _args, _kwargs)
        return val
    def GetY(self, *_args, **_kwargs):
        val = apply(gdic.wxRegionIterator_GetY,(self,) + _args, _kwargs)
        return val
    def GetW(self, *_args, **_kwargs):
        val = apply(gdic.wxRegionIterator_GetW,(self,) + _args, _kwargs)
        return val
    def GetWidth(self, *_args, **_kwargs):
        val = apply(gdic.wxRegionIterator_GetWidth,(self,) + _args, _kwargs)
        return val
    def GetH(self, *_args, **_kwargs):
        val = apply(gdic.wxRegionIterator_GetH,(self,) + _args, _kwargs)
        return val
    def GetHeight(self, *_args, **_kwargs):
        val = apply(gdic.wxRegionIterator_GetHeight,(self,) + _args, _kwargs)
        return val
    def GetRect(self, *_args, **_kwargs):
        val = apply(gdic.wxRegionIterator_GetRect,(self,) + _args, _kwargs)
        if val: val = wxRectPtr(val) ; val.thisown = 1
        return val
    def HaveRects(self, *_args, **_kwargs):
        val = apply(gdic.wxRegionIterator_HaveRects,(self,) + _args, _kwargs)
        return val
    def Reset(self, *_args, **_kwargs):
        val = apply(gdic.wxRegionIterator_Reset,(self,) + _args, _kwargs)
        return val
    def Next(self, *_args, **_kwargs):
        val = apply(gdic.wxRegionIterator_Next,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxRegionIterator instance at %s>" % (self.this,)
class wxRegionIterator(wxRegionIteratorPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(gdic.new_wxRegionIterator,_args,_kwargs)
        self.thisown = 1






#-------------- FUNCTION WRAPPERS ------------------

def wxEmptyBitmap(*_args, **_kwargs):
    val = apply(gdic.wxEmptyBitmap,_args,_kwargs)
    if val: val = wxBitmapPtr(val); val.thisown = 1
    return val

def wxBitmapFromXPMData(*_args, **_kwargs):
    val = apply(gdic.wxBitmapFromXPMData,_args,_kwargs)
    if val: val = wxBitmapPtr(val); val.thisown = 1
    return val

def wxBitmapFromIcon(*_args, **_kwargs):
    val = apply(gdic.wxBitmapFromIcon,_args,_kwargs)
    if val: val = wxBitmapPtr(val); val.thisown = 1
    return val

def wxBitmapFromBits(*_args, **_kwargs):
    val = apply(gdic.wxBitmapFromBits,_args,_kwargs)
    if val: val = wxBitmapPtr(val); val.thisown = 1
    return val

def wxMaskColour(*_args, **_kwargs):
    val = apply(gdic.wxMaskColour,_args,_kwargs)
    if val: val = wxMaskPtr(val); val.thisown = 1
    return val

def wxEmptyIcon(*_args, **_kwargs):
    val = apply(gdic.wxEmptyIcon,_args,_kwargs)
    if val: val = wxIconPtr(val); val.thisown = 1
    return val

def wxIconFromXPMData(*_args, **_kwargs):
    val = apply(gdic.wxIconFromXPMData,_args,_kwargs)
    if val: val = wxIconPtr(val); val.thisown = 1
    return val

def wxStockCursor(*_args, **_kwargs):
    val = apply(gdic.wxStockCursor,_args,_kwargs)
    if val: val = wxCursorPtr(val); val.thisown = 1
    return val

def wxNamedColour(*_args, **_kwargs):
    val = apply(gdic.wxNamedColour,_args,_kwargs)
    if val: val = wxColourPtr(val); val.thisown = 1
    return val

def wxMemoryDCFromDC(*_args, **_kwargs):
    val = apply(gdic.wxMemoryDCFromDC,_args,_kwargs)
    if val: val = wxMemoryDCPtr(val); val.thisown = 1
    return val

wxFontMapper_GetEncodingName = gdic.wxFontMapper_GetEncodingName

wxFontMapper_GetEncodingDescription = gdic.wxFontMapper_GetEncodingDescription

wxFontMapper_GetDefaultConfigPath = gdic.wxFontMapper_GetDefaultConfigPath

wxFont_GetDefaultEncoding = gdic.wxFont_GetDefaultEncoding

wxFont_SetDefaultEncoding = gdic.wxFont_SetDefaultEncoding



#-------------- VARIABLE WRAPPERS ------------------

wxFONTFAMILY_DEFAULT = gdic.wxFONTFAMILY_DEFAULT
wxFONTFAMILY_DECORATIVE = gdic.wxFONTFAMILY_DECORATIVE
wxFONTFAMILY_ROMAN = gdic.wxFONTFAMILY_ROMAN
wxFONTFAMILY_SCRIPT = gdic.wxFONTFAMILY_SCRIPT
wxFONTFAMILY_SWISS = gdic.wxFONTFAMILY_SWISS
wxFONTFAMILY_MODERN = gdic.wxFONTFAMILY_MODERN
wxFONTFAMILY_TELETYPE = gdic.wxFONTFAMILY_TELETYPE
wxFONTFAMILY_MAX = gdic.wxFONTFAMILY_MAX
wxFONTSTYLE_NORMAL = gdic.wxFONTSTYLE_NORMAL
wxFONTSTYLE_ITALIC = gdic.wxFONTSTYLE_ITALIC
wxFONTSTYLE_SLANT = gdic.wxFONTSTYLE_SLANT
wxFONTSTYLE_MAX = gdic.wxFONTSTYLE_MAX
wxFONTWEIGHT_NORMAL = gdic.wxFONTWEIGHT_NORMAL
wxFONTWEIGHT_LIGHT = gdic.wxFONTWEIGHT_LIGHT
wxFONTWEIGHT_BOLD = gdic.wxFONTWEIGHT_BOLD
wxFONTWEIGHT_MAX = gdic.wxFONTWEIGHT_MAX
wxFONTENCODING_SYSTEM = gdic.wxFONTENCODING_SYSTEM
wxFONTENCODING_DEFAULT = gdic.wxFONTENCODING_DEFAULT
wxFONTENCODING_ISO8859_1 = gdic.wxFONTENCODING_ISO8859_1
wxFONTENCODING_ISO8859_2 = gdic.wxFONTENCODING_ISO8859_2
wxFONTENCODING_ISO8859_3 = gdic.wxFONTENCODING_ISO8859_3
wxFONTENCODING_ISO8859_4 = gdic.wxFONTENCODING_ISO8859_4
wxFONTENCODING_ISO8859_5 = gdic.wxFONTENCODING_ISO8859_5
wxFONTENCODING_ISO8859_6 = gdic.wxFONTENCODING_ISO8859_6
wxFONTENCODING_ISO8859_7 = gdic.wxFONTENCODING_ISO8859_7
wxFONTENCODING_ISO8859_8 = gdic.wxFONTENCODING_ISO8859_8
wxFONTENCODING_ISO8859_9 = gdic.wxFONTENCODING_ISO8859_9
wxFONTENCODING_ISO8859_10 = gdic.wxFONTENCODING_ISO8859_10
wxFONTENCODING_ISO8859_11 = gdic.wxFONTENCODING_ISO8859_11
wxFONTENCODING_ISO8859_12 = gdic.wxFONTENCODING_ISO8859_12
wxFONTENCODING_ISO8859_13 = gdic.wxFONTENCODING_ISO8859_13
wxFONTENCODING_ISO8859_14 = gdic.wxFONTENCODING_ISO8859_14
wxFONTENCODING_ISO8859_15 = gdic.wxFONTENCODING_ISO8859_15
wxFONTENCODING_ISO8859_MAX = gdic.wxFONTENCODING_ISO8859_MAX
wxFONTENCODING_KOI8 = gdic.wxFONTENCODING_KOI8
wxFONTENCODING_ALTERNATIVE = gdic.wxFONTENCODING_ALTERNATIVE
wxFONTENCODING_BULGARIAN = gdic.wxFONTENCODING_BULGARIAN
wxFONTENCODING_CP437 = gdic.wxFONTENCODING_CP437
wxFONTENCODING_CP850 = gdic.wxFONTENCODING_CP850
wxFONTENCODING_CP852 = gdic.wxFONTENCODING_CP852
wxFONTENCODING_CP855 = gdic.wxFONTENCODING_CP855
wxFONTENCODING_CP866 = gdic.wxFONTENCODING_CP866
wxFONTENCODING_CP874 = gdic.wxFONTENCODING_CP874
wxFONTENCODING_CP932 = gdic.wxFONTENCODING_CP932
wxFONTENCODING_CP936 = gdic.wxFONTENCODING_CP936
wxFONTENCODING_CP949 = gdic.wxFONTENCODING_CP949
wxFONTENCODING_CP950 = gdic.wxFONTENCODING_CP950
wxFONTENCODING_CP1250 = gdic.wxFONTENCODING_CP1250
wxFONTENCODING_CP1251 = gdic.wxFONTENCODING_CP1251
wxFONTENCODING_CP1252 = gdic.wxFONTENCODING_CP1252
wxFONTENCODING_CP1253 = gdic.wxFONTENCODING_CP1253
wxFONTENCODING_CP1254 = gdic.wxFONTENCODING_CP1254
wxFONTENCODING_CP1255 = gdic.wxFONTENCODING_CP1255
wxFONTENCODING_CP1256 = gdic.wxFONTENCODING_CP1256
wxFONTENCODING_CP1257 = gdic.wxFONTENCODING_CP1257
wxFONTENCODING_CP12_MAX = gdic.wxFONTENCODING_CP12_MAX
wxFONTENCODING_UTF7 = gdic.wxFONTENCODING_UTF7
wxFONTENCODING_UTF8 = gdic.wxFONTENCODING_UTF8
wxFONTENCODING_UNICODE = gdic.wxFONTENCODING_UNICODE
wxFONTENCODING_MAX = gdic.wxFONTENCODING_MAX
wxIMAGELIST_DRAW_NORMAL = gdic.wxIMAGELIST_DRAW_NORMAL
wxIMAGELIST_DRAW_TRANSPARENT = gdic.wxIMAGELIST_DRAW_TRANSPARENT
wxIMAGELIST_DRAW_SELECTED = gdic.wxIMAGELIST_DRAW_SELECTED
wxIMAGELIST_DRAW_FOCUSED = gdic.wxIMAGELIST_DRAW_FOCUSED
wxIMAGE_LIST_NORMAL = gdic.wxIMAGE_LIST_NORMAL
wxIMAGE_LIST_SMALL = gdic.wxIMAGE_LIST_SMALL
wxIMAGE_LIST_STATE = gdic.wxIMAGE_LIST_STATE
wxOutRegion = gdic.wxOutRegion
wxPartRegion = gdic.wxPartRegion
wxInRegion = gdic.wxInRegion
cvar = gdic.cvar
wxNORMAL_FONT = wxFontPtr(gdic.cvar.wxNORMAL_FONT)
wxSMALL_FONT = wxFontPtr(gdic.cvar.wxSMALL_FONT)
wxITALIC_FONT = wxFontPtr(gdic.cvar.wxITALIC_FONT)
wxSWISS_FONT = wxFontPtr(gdic.cvar.wxSWISS_FONT)
wxRED_PEN = wxPenPtr(gdic.cvar.wxRED_PEN)
wxCYAN_PEN = wxPenPtr(gdic.cvar.wxCYAN_PEN)
wxGREEN_PEN = wxPenPtr(gdic.cvar.wxGREEN_PEN)
wxBLACK_PEN = wxPenPtr(gdic.cvar.wxBLACK_PEN)
wxWHITE_PEN = wxPenPtr(gdic.cvar.wxWHITE_PEN)
wxTRANSPARENT_PEN = wxPenPtr(gdic.cvar.wxTRANSPARENT_PEN)
wxBLACK_DASHED_PEN = wxPenPtr(gdic.cvar.wxBLACK_DASHED_PEN)
wxGREY_PEN = wxPenPtr(gdic.cvar.wxGREY_PEN)
wxMEDIUM_GREY_PEN = wxPenPtr(gdic.cvar.wxMEDIUM_GREY_PEN)
wxLIGHT_GREY_PEN = wxPenPtr(gdic.cvar.wxLIGHT_GREY_PEN)
wxBLUE_BRUSH = wxBrushPtr(gdic.cvar.wxBLUE_BRUSH)
wxGREEN_BRUSH = wxBrushPtr(gdic.cvar.wxGREEN_BRUSH)
wxWHITE_BRUSH = wxBrushPtr(gdic.cvar.wxWHITE_BRUSH)
wxBLACK_BRUSH = wxBrushPtr(gdic.cvar.wxBLACK_BRUSH)
wxTRANSPARENT_BRUSH = wxBrushPtr(gdic.cvar.wxTRANSPARENT_BRUSH)
wxCYAN_BRUSH = wxBrushPtr(gdic.cvar.wxCYAN_BRUSH)
wxRED_BRUSH = wxBrushPtr(gdic.cvar.wxRED_BRUSH)
wxGREY_BRUSH = wxBrushPtr(gdic.cvar.wxGREY_BRUSH)
wxMEDIUM_GREY_BRUSH = wxBrushPtr(gdic.cvar.wxMEDIUM_GREY_BRUSH)
wxLIGHT_GREY_BRUSH = wxBrushPtr(gdic.cvar.wxLIGHT_GREY_BRUSH)
wxBLACK = wxColourPtr(gdic.cvar.wxBLACK)
wxWHITE = wxColourPtr(gdic.cvar.wxWHITE)
wxRED = wxColourPtr(gdic.cvar.wxRED)
wxBLUE = wxColourPtr(gdic.cvar.wxBLUE)
wxGREEN = wxColourPtr(gdic.cvar.wxGREEN)
wxCYAN = wxColourPtr(gdic.cvar.wxCYAN)
wxLIGHT_GREY = wxColourPtr(gdic.cvar.wxLIGHT_GREY)
wxSTANDARD_CURSOR = wxCursorPtr(gdic.cvar.wxSTANDARD_CURSOR)
wxHOURGLASS_CURSOR = wxCursorPtr(gdic.cvar.wxHOURGLASS_CURSOR)
wxCROSS_CURSOR = wxCursorPtr(gdic.cvar.wxCROSS_CURSOR)
wxNullBitmap = wxBitmapPtr(gdic.cvar.wxNullBitmap)
wxNullIcon = wxIconPtr(gdic.cvar.wxNullIcon)
wxNullCursor = wxCursorPtr(gdic.cvar.wxNullCursor)
wxNullPen = wxPenPtr(gdic.cvar.wxNullPen)
wxNullBrush = wxBrushPtr(gdic.cvar.wxNullBrush)
wxNullPalette = wxPalettePtr(gdic.cvar.wxNullPalette)
wxNullFont = wxFontPtr(gdic.cvar.wxNullFont)
wxNullColour = wxColourPtr(gdic.cvar.wxNullColour)
wxTheFontList = wxFontListPtr(gdic.cvar.wxTheFontList)
wxThePenList = wxPenListPtr(gdic.cvar.wxThePenList)
wxTheBrushList = wxBrushListPtr(gdic.cvar.wxTheBrushList)
wxTheColourDatabase = wxColourDatabasePtr(gdic.cvar.wxTheColourDatabase)
