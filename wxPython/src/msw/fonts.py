# This file was created automatically by SWIG.
import fontsc

from misc import *
class wxNativeFontInfoPtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def Init(self, *_args, **_kwargs):
        val = apply(fontsc.wxNativeFontInfo_Init,(self,) + _args, _kwargs)
        return val
    def GetPointSize(self, *_args, **_kwargs):
        val = apply(fontsc.wxNativeFontInfo_GetPointSize,(self,) + _args, _kwargs)
        return val
    def GetStyle(self, *_args, **_kwargs):
        val = apply(fontsc.wxNativeFontInfo_GetStyle,(self,) + _args, _kwargs)
        return val
    def GetWeight(self, *_args, **_kwargs):
        val = apply(fontsc.wxNativeFontInfo_GetWeight,(self,) + _args, _kwargs)
        return val
    def GetUnderlined(self, *_args, **_kwargs):
        val = apply(fontsc.wxNativeFontInfo_GetUnderlined,(self,) + _args, _kwargs)
        return val
    def GetFaceName(self, *_args, **_kwargs):
        val = apply(fontsc.wxNativeFontInfo_GetFaceName,(self,) + _args, _kwargs)
        return val
    def GetFamily(self, *_args, **_kwargs):
        val = apply(fontsc.wxNativeFontInfo_GetFamily,(self,) + _args, _kwargs)
        return val
    def GetEncoding(self, *_args, **_kwargs):
        val = apply(fontsc.wxNativeFontInfo_GetEncoding,(self,) + _args, _kwargs)
        return val
    def SetPointSize(self, *_args, **_kwargs):
        val = apply(fontsc.wxNativeFontInfo_SetPointSize,(self,) + _args, _kwargs)
        return val
    def SetStyle(self, *_args, **_kwargs):
        val = apply(fontsc.wxNativeFontInfo_SetStyle,(self,) + _args, _kwargs)
        return val
    def SetWeight(self, *_args, **_kwargs):
        val = apply(fontsc.wxNativeFontInfo_SetWeight,(self,) + _args, _kwargs)
        return val
    def SetUnderlined(self, *_args, **_kwargs):
        val = apply(fontsc.wxNativeFontInfo_SetUnderlined,(self,) + _args, _kwargs)
        return val
    def SetFaceName(self, *_args, **_kwargs):
        val = apply(fontsc.wxNativeFontInfo_SetFaceName,(self,) + _args, _kwargs)
        return val
    def SetFamily(self, *_args, **_kwargs):
        val = apply(fontsc.wxNativeFontInfo_SetFamily,(self,) + _args, _kwargs)
        return val
    def SetEncoding(self, *_args, **_kwargs):
        val = apply(fontsc.wxNativeFontInfo_SetEncoding,(self,) + _args, _kwargs)
        return val
    def FromString(self, *_args, **_kwargs):
        val = apply(fontsc.wxNativeFontInfo_FromString,(self,) + _args, _kwargs)
        return val
    def ToString(self, *_args, **_kwargs):
        val = apply(fontsc.wxNativeFontInfo_ToString,(self,) + _args, _kwargs)
        return val
    def __str__(self, *_args, **_kwargs):
        val = apply(fontsc.wxNativeFontInfo___str__,(self,) + _args, _kwargs)
        return val
    def FromUserString(self, *_args, **_kwargs):
        val = apply(fontsc.wxNativeFontInfo_FromUserString,(self,) + _args, _kwargs)
        return val
    def ToUserString(self, *_args, **_kwargs):
        val = apply(fontsc.wxNativeFontInfo_ToUserString,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxNativeFontInfo instance at %s>" % (self.this,)
class wxNativeFontInfo(wxNativeFontInfoPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(fontsc.new_wxNativeFontInfo,_args,_kwargs)
        self.thisown = 1




class wxFontMapperPtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __del__(self, delfunc=fontsc.delete_wxFontMapper):
        if self.thisown == 1:
            try:
                delfunc(self)
            except:
                pass
    def GetAltForEncoding(self, *_args, **_kwargs):
        val = apply(fontsc.wxFontMapper_GetAltForEncoding,(self,) + _args, _kwargs)
        return val
    def IsEncodingAvailable(self, *_args, **_kwargs):
        val = apply(fontsc.wxFontMapper_IsEncodingAvailable,(self,) + _args, _kwargs)
        return val
    def CharsetToEncoding(self, *_args, **_kwargs):
        val = apply(fontsc.wxFontMapper_CharsetToEncoding,(self,) + _args, _kwargs)
        return val
    def SetDialogParent(self, *_args, **_kwargs):
        val = apply(fontsc.wxFontMapper_SetDialogParent,(self,) + _args, _kwargs)
        return val
    def SetDialogTitle(self, *_args, **_kwargs):
        val = apply(fontsc.wxFontMapper_SetDialogTitle,(self,) + _args, _kwargs)
        return val
    def SetConfig(self, *_args, **_kwargs):
        val = apply(fontsc.wxFontMapper_SetConfig,(self,) + _args, _kwargs)
        return val
    def SetConfigPath(self, *_args, **_kwargs):
        val = apply(fontsc.wxFontMapper_SetConfigPath,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxFontMapper instance at %s>" % (self.this,)
class wxFontMapper(wxFontMapperPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(fontsc.new_wxFontMapper,_args,_kwargs)
        self.thisown = 1




class wxFontPtr(wxObjectPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def wxFontFromNativeInfoString(self, *_args, **_kwargs):
        val = apply(fontsc.wxFont_wxFontFromNativeInfoString,(self,) + _args, _kwargs)
        if val: val = wxFontPtr(val) ; val.thisown = 1
        return val
    def __del__(self, delfunc=fontsc.delete_wxFont):
        if self.thisown == 1:
            try:
                delfunc(self)
            except:
                pass
    def Ok(self, *_args, **_kwargs):
        val = apply(fontsc.wxFont_Ok,(self,) + _args, _kwargs)
        return val
    def GetPointSize(self, *_args, **_kwargs):
        val = apply(fontsc.wxFont_GetPointSize,(self,) + _args, _kwargs)
        return val
    def GetFamily(self, *_args, **_kwargs):
        val = apply(fontsc.wxFont_GetFamily,(self,) + _args, _kwargs)
        return val
    def GetStyle(self, *_args, **_kwargs):
        val = apply(fontsc.wxFont_GetStyle,(self,) + _args, _kwargs)
        return val
    def GetWeight(self, *_args, **_kwargs):
        val = apply(fontsc.wxFont_GetWeight,(self,) + _args, _kwargs)
        return val
    def GetUnderlined(self, *_args, **_kwargs):
        val = apply(fontsc.wxFont_GetUnderlined,(self,) + _args, _kwargs)
        return val
    def GetFaceName(self, *_args, **_kwargs):
        val = apply(fontsc.wxFont_GetFaceName,(self,) + _args, _kwargs)
        return val
    def GetEncoding(self, *_args, **_kwargs):
        val = apply(fontsc.wxFont_GetEncoding,(self,) + _args, _kwargs)
        return val
    def IsFixedWidth(self, *_args, **_kwargs):
        val = apply(fontsc.wxFont_IsFixedWidth,(self,) + _args, _kwargs)
        return val
    def GetNativeFontInfo(self, *_args, **_kwargs):
        val = apply(fontsc.wxFont_GetNativeFontInfo,(self,) + _args, _kwargs)
        if val: val = wxNativeFontInfoPtr(val) 
        return val
    def GetNativeFontInfoDesc(self, *_args, **_kwargs):
        val = apply(fontsc.wxFont_GetNativeFontInfoDesc,(self,) + _args, _kwargs)
        return val
    def GetNativeFontInfoUserDesc(self, *_args, **_kwargs):
        val = apply(fontsc.wxFont_GetNativeFontInfoUserDesc,(self,) + _args, _kwargs)
        return val
    def SetPointSize(self, *_args, **_kwargs):
        val = apply(fontsc.wxFont_SetPointSize,(self,) + _args, _kwargs)
        return val
    def SetFamily(self, *_args, **_kwargs):
        val = apply(fontsc.wxFont_SetFamily,(self,) + _args, _kwargs)
        return val
    def SetStyle(self, *_args, **_kwargs):
        val = apply(fontsc.wxFont_SetStyle,(self,) + _args, _kwargs)
        return val
    def SetWeight(self, *_args, **_kwargs):
        val = apply(fontsc.wxFont_SetWeight,(self,) + _args, _kwargs)
        return val
    def SetFaceName(self, *_args, **_kwargs):
        val = apply(fontsc.wxFont_SetFaceName,(self,) + _args, _kwargs)
        return val
    def SetUnderlined(self, *_args, **_kwargs):
        val = apply(fontsc.wxFont_SetUnderlined,(self,) + _args, _kwargs)
        return val
    def SetEncoding(self, *_args, **_kwargs):
        val = apply(fontsc.wxFont_SetEncoding,(self,) + _args, _kwargs)
        return val
    def SetNativeFontInfo(self, *_args, **_kwargs):
        val = apply(fontsc.wxFont_SetNativeFontInfo,(self,) + _args, _kwargs)
        return val
    def SetNativeFontInfoUserDesc(self, *_args, **_kwargs):
        val = apply(fontsc.wxFont_SetNativeFontInfoUserDesc,(self,) + _args, _kwargs)
        return val
    def GetFamilyString(self, *_args, **_kwargs):
        val = apply(fontsc.wxFont_GetFamilyString,(self,) + _args, _kwargs)
        return val
    def GetStyleString(self, *_args, **_kwargs):
        val = apply(fontsc.wxFont_GetStyleString,(self,) + _args, _kwargs)
        return val
    def GetWeightString(self, *_args, **_kwargs):
        val = apply(fontsc.wxFont_GetWeightString,(self,) + _args, _kwargs)
        return val
    def SetNoAntiAliasing(self, *_args, **_kwargs):
        val = apply(fontsc.wxFont_SetNoAntiAliasing,(self,) + _args, _kwargs)
        return val
    def GetNoAntiAliasing(self, *_args, **_kwargs):
        val = apply(fontsc.wxFont_GetNoAntiAliasing,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxFont instance at %s>" % (self.this,)
class wxFont(wxFontPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(fontsc.new_wxFont,_args,_kwargs)
        self.thisown = 1



def wxFontFromNativeInfo(*_args,**_kwargs):
    val = wxFontPtr(apply(fontsc.new_wxFontFromNativeInfo,_args,_kwargs))
    val.thisown = 1
    return val


class wxFontListPtr(wxObjectPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def AddFont(self, *_args, **_kwargs):
        val = apply(fontsc.wxFontList_AddFont,(self,) + _args, _kwargs)
        return val
    def FindOrCreateFont(self, *_args, **_kwargs):
        val = apply(fontsc.wxFontList_FindOrCreateFont,(self,) + _args, _kwargs)
        if val: val = wxFontPtr(val) 
        return val
    def RemoveFont(self, *_args, **_kwargs):
        val = apply(fontsc.wxFontList_RemoveFont,(self,) + _args, _kwargs)
        return val
    def GetCount(self, *_args, **_kwargs):
        val = apply(fontsc.wxFontList_GetCount,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxFontList instance at %s>" % (self.this,)
class wxFontList(wxFontListPtr):
    def __init__(self,this):
        self.this = this




class wxFontEnumeratorPtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __del__(self, delfunc=fontsc.delete_wxFontEnumerator):
        if self.thisown == 1:
            try:
                delfunc(self)
            except:
                pass
    def _setCallbackInfo(self, *_args, **_kwargs):
        val = apply(fontsc.wxFontEnumerator__setCallbackInfo,(self,) + _args, _kwargs)
        return val
    def EnumerateFacenames(self, *_args, **_kwargs):
        val = apply(fontsc.wxFontEnumerator_EnumerateFacenames,(self,) + _args, _kwargs)
        return val
    def EnumerateEncodings(self, *_args, **_kwargs):
        val = apply(fontsc.wxFontEnumerator_EnumerateEncodings,(self,) + _args, _kwargs)
        return val
    def GetEncodings(self, *_args, **_kwargs):
        val = apply(fontsc.wxFontEnumerator_GetEncodings,(self,) + _args, _kwargs)
        return val
    def GetFacenames(self, *_args, **_kwargs):
        val = apply(fontsc.wxFontEnumerator_GetFacenames,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxFontEnumerator instance at %s>" % (self.this,)
class wxFontEnumerator(wxFontEnumeratorPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(fontsc.new_wxFontEnumerator,_args,_kwargs)
        self.thisown = 1
        self._setCallbackInfo(self, wxFontEnumerator, 0)




class wxLanguageInfoPtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __setattr__(self,name,value):
        if name == "Language" :
            fontsc.wxLanguageInfo_Language_set(self,value)
            return
        if name == "CanonicalName" :
            fontsc.wxLanguageInfo_CanonicalName_set(self,value)
            return
        if name == "Description" :
            fontsc.wxLanguageInfo_Description_set(self,value)
            return
        self.__dict__[name] = value
    def __getattr__(self,name):
        if name == "Language" : 
            return fontsc.wxLanguageInfo_Language_get(self)
        if name == "CanonicalName" : 
            return fontsc.wxLanguageInfo_CanonicalName_get(self)
        if name == "Description" : 
            return fontsc.wxLanguageInfo_Description_get(self)
        raise AttributeError,name
    def __repr__(self):
        return "<C wxLanguageInfo instance at %s>" % (self.this,)
class wxLanguageInfo(wxLanguageInfoPtr):
    def __init__(self,this):
        self.this = this




class wxLocalePtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __del__(self, delfunc=fontsc.delete_wxLocale):
        if self.thisown == 1:
            try:
                delfunc(self)
            except:
                pass
    def Init1(self, *_args, **_kwargs):
        val = apply(fontsc.wxLocale_Init1,(self,) + _args, _kwargs)
        return val
    def Init2(self, *_args, **_kwargs):
        val = apply(fontsc.wxLocale_Init2,(self,) + _args, _kwargs)
        return val
    def IsOk(self, *_args, **_kwargs):
        val = apply(fontsc.wxLocale_IsOk,(self,) + _args, _kwargs)
        return val
    def GetLocale(self, *_args, **_kwargs):
        val = apply(fontsc.wxLocale_GetLocale,(self,) + _args, _kwargs)
        return val
    def GetLanguage(self, *_args, **_kwargs):
        val = apply(fontsc.wxLocale_GetLanguage,(self,) + _args, _kwargs)
        return val
    def GetSysName(self, *_args, **_kwargs):
        val = apply(fontsc.wxLocale_GetSysName,(self,) + _args, _kwargs)
        return val
    def GetCanonicalName(self, *_args, **_kwargs):
        val = apply(fontsc.wxLocale_GetCanonicalName,(self,) + _args, _kwargs)
        return val
    def AddCatalog(self, *_args, **_kwargs):
        val = apply(fontsc.wxLocale_AddCatalog,(self,) + _args, _kwargs)
        return val
    def IsLoaded(self, *_args, **_kwargs):
        val = apply(fontsc.wxLocale_IsLoaded,(self,) + _args, _kwargs)
        return val
    def GetString(self, *_args, **_kwargs):
        val = apply(fontsc.wxLocale_GetString,(self,) + _args, _kwargs)
        return val
    def GetName(self, *_args, **_kwargs):
        val = apply(fontsc.wxLocale_GetName,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxLocale instance at %s>" % (self.this,)
    
    def Init(self, *_args, **_kwargs):
        if type(_args[0]) in [type(''), type(u'')]:
            val = apply(self.Init1, _args, _kwargs)
        else:
            val = apply(self.Init2, _args, _kwargs)
        return val
    
class wxLocale(wxLocalePtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(fontsc.new_wxLocale,_args,_kwargs)
        self.thisown = 1




class wxEncodingConverterPtr(wxObjectPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __del__(self, delfunc=fontsc.delete_wxEncodingConverter):
        if self.thisown == 1:
            try:
                delfunc(self)
            except:
                pass
    def Init(self, *_args, **_kwargs):
        val = apply(fontsc.wxEncodingConverter_Init,(self,) + _args, _kwargs)
        return val
    def Convert(self, *_args, **_kwargs):
        val = apply(fontsc.wxEncodingConverter_Convert,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxEncodingConverter instance at %s>" % (self.this,)
class wxEncodingConverter(wxEncodingConverterPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(fontsc.new_wxEncodingConverter,_args,_kwargs)
        self.thisown = 1






#-------------- FUNCTION WRAPPERS ------------------

def wxGetLocale(*_args, **_kwargs):
    val = apply(fontsc.wxGetLocale,_args,_kwargs)
    if val: val = wxLocalePtr(val)
    return val

wxGetTranslation = fontsc.wxGetTranslation

def wxFontMapper_Get(*_args, **_kwargs):
    val = apply(fontsc.wxFontMapper_Get,_args,_kwargs)
    if val: val = wxFontMapperPtr(val)
    return val

def wxFontMapper_Set(*_args, **_kwargs):
    val = apply(fontsc.wxFontMapper_Set,_args,_kwargs)
    if val: val = wxFontMapperPtr(val)
    return val

wxFontMapper_GetEncodingName = fontsc.wxFontMapper_GetEncodingName

wxFontMapper_GetEncodingDescription = fontsc.wxFontMapper_GetEncodingDescription

wxFontMapper_GetDefaultConfigPath = fontsc.wxFontMapper_GetDefaultConfigPath

wxFont_GetDefaultEncoding = fontsc.wxFont_GetDefaultEncoding

wxFont_SetDefaultEncoding = fontsc.wxFont_SetDefaultEncoding

wxLocale_GetSystemLanguage = fontsc.wxLocale_GetSystemLanguage

wxLocale_GetSystemEncoding = fontsc.wxLocale_GetSystemEncoding

wxLocale_GetSystemEncodingName = fontsc.wxLocale_GetSystemEncodingName

wxLocale_AddCatalogLookupPathPrefix = fontsc.wxLocale_AddCatalogLookupPathPrefix

def wxLocale_GetLanguageInfo(*_args, **_kwargs):
    val = apply(fontsc.wxLocale_GetLanguageInfo,_args,_kwargs)
    if val: val = wxLanguageInfoPtr(val)
    return val

def wxLocale_FindLanguageInfo(*_args, **_kwargs):
    val = apply(fontsc.wxLocale_FindLanguageInfo,_args,_kwargs)
    if val: val = wxLanguageInfoPtr(val)
    return val

wxLocale_AddLanguage = fontsc.wxLocale_AddLanguage

wxEncodingConverter_GetPlatformEquivalents = fontsc.wxEncodingConverter_GetPlatformEquivalents

wxEncodingConverter_GetAllEquivalents = fontsc.wxEncodingConverter_GetAllEquivalents



#-------------- VARIABLE WRAPPERS ------------------

wxFONTFAMILY_DEFAULT = fontsc.wxFONTFAMILY_DEFAULT
wxFONTFAMILY_DECORATIVE = fontsc.wxFONTFAMILY_DECORATIVE
wxFONTFAMILY_ROMAN = fontsc.wxFONTFAMILY_ROMAN
wxFONTFAMILY_SCRIPT = fontsc.wxFONTFAMILY_SCRIPT
wxFONTFAMILY_SWISS = fontsc.wxFONTFAMILY_SWISS
wxFONTFAMILY_MODERN = fontsc.wxFONTFAMILY_MODERN
wxFONTFAMILY_TELETYPE = fontsc.wxFONTFAMILY_TELETYPE
wxFONTFAMILY_MAX = fontsc.wxFONTFAMILY_MAX
wxFONTFAMILY_UNKNOWN = fontsc.wxFONTFAMILY_UNKNOWN
wxFONTSTYLE_NORMAL = fontsc.wxFONTSTYLE_NORMAL
wxFONTSTYLE_ITALIC = fontsc.wxFONTSTYLE_ITALIC
wxFONTSTYLE_SLANT = fontsc.wxFONTSTYLE_SLANT
wxFONTSTYLE_MAX = fontsc.wxFONTSTYLE_MAX
wxFONTWEIGHT_NORMAL = fontsc.wxFONTWEIGHT_NORMAL
wxFONTWEIGHT_LIGHT = fontsc.wxFONTWEIGHT_LIGHT
wxFONTWEIGHT_BOLD = fontsc.wxFONTWEIGHT_BOLD
wxFONTWEIGHT_MAX = fontsc.wxFONTWEIGHT_MAX
wxFONTENCODING_SYSTEM = fontsc.wxFONTENCODING_SYSTEM
wxFONTENCODING_DEFAULT = fontsc.wxFONTENCODING_DEFAULT
wxFONTENCODING_ISO8859_1 = fontsc.wxFONTENCODING_ISO8859_1
wxFONTENCODING_ISO8859_2 = fontsc.wxFONTENCODING_ISO8859_2
wxFONTENCODING_ISO8859_3 = fontsc.wxFONTENCODING_ISO8859_3
wxFONTENCODING_ISO8859_4 = fontsc.wxFONTENCODING_ISO8859_4
wxFONTENCODING_ISO8859_5 = fontsc.wxFONTENCODING_ISO8859_5
wxFONTENCODING_ISO8859_6 = fontsc.wxFONTENCODING_ISO8859_6
wxFONTENCODING_ISO8859_7 = fontsc.wxFONTENCODING_ISO8859_7
wxFONTENCODING_ISO8859_8 = fontsc.wxFONTENCODING_ISO8859_8
wxFONTENCODING_ISO8859_9 = fontsc.wxFONTENCODING_ISO8859_9
wxFONTENCODING_ISO8859_10 = fontsc.wxFONTENCODING_ISO8859_10
wxFONTENCODING_ISO8859_11 = fontsc.wxFONTENCODING_ISO8859_11
wxFONTENCODING_ISO8859_12 = fontsc.wxFONTENCODING_ISO8859_12
wxFONTENCODING_ISO8859_13 = fontsc.wxFONTENCODING_ISO8859_13
wxFONTENCODING_ISO8859_14 = fontsc.wxFONTENCODING_ISO8859_14
wxFONTENCODING_ISO8859_15 = fontsc.wxFONTENCODING_ISO8859_15
wxFONTENCODING_ISO8859_MAX = fontsc.wxFONTENCODING_ISO8859_MAX
wxFONTENCODING_KOI8 = fontsc.wxFONTENCODING_KOI8
wxFONTENCODING_ALTERNATIVE = fontsc.wxFONTENCODING_ALTERNATIVE
wxFONTENCODING_BULGARIAN = fontsc.wxFONTENCODING_BULGARIAN
wxFONTENCODING_CP437 = fontsc.wxFONTENCODING_CP437
wxFONTENCODING_CP850 = fontsc.wxFONTENCODING_CP850
wxFONTENCODING_CP852 = fontsc.wxFONTENCODING_CP852
wxFONTENCODING_CP855 = fontsc.wxFONTENCODING_CP855
wxFONTENCODING_CP866 = fontsc.wxFONTENCODING_CP866
wxFONTENCODING_CP874 = fontsc.wxFONTENCODING_CP874
wxFONTENCODING_CP932 = fontsc.wxFONTENCODING_CP932
wxFONTENCODING_CP936 = fontsc.wxFONTENCODING_CP936
wxFONTENCODING_CP949 = fontsc.wxFONTENCODING_CP949
wxFONTENCODING_CP950 = fontsc.wxFONTENCODING_CP950
wxFONTENCODING_CP1250 = fontsc.wxFONTENCODING_CP1250
wxFONTENCODING_CP1251 = fontsc.wxFONTENCODING_CP1251
wxFONTENCODING_CP1252 = fontsc.wxFONTENCODING_CP1252
wxFONTENCODING_CP1253 = fontsc.wxFONTENCODING_CP1253
wxFONTENCODING_CP1254 = fontsc.wxFONTENCODING_CP1254
wxFONTENCODING_CP1255 = fontsc.wxFONTENCODING_CP1255
wxFONTENCODING_CP1256 = fontsc.wxFONTENCODING_CP1256
wxFONTENCODING_CP1257 = fontsc.wxFONTENCODING_CP1257
wxFONTENCODING_CP12_MAX = fontsc.wxFONTENCODING_CP12_MAX
wxFONTENCODING_UTF7 = fontsc.wxFONTENCODING_UTF7
wxFONTENCODING_UTF8 = fontsc.wxFONTENCODING_UTF8
wxFONTENCODING_GB2312 = fontsc.wxFONTENCODING_GB2312
wxFONTENCODING_BIG5 = fontsc.wxFONTENCODING_BIG5
wxFONTENCODING_SHIFT_JIS = fontsc.wxFONTENCODING_SHIFT_JIS
wxFONTENCODING_EUC_JP = fontsc.wxFONTENCODING_EUC_JP
wxFONTENCODING_UNICODE = fontsc.wxFONTENCODING_UNICODE
wxFONTENCODING_MAX = fontsc.wxFONTENCODING_MAX
wxLANGUAGE_DEFAULT = fontsc.wxLANGUAGE_DEFAULT
wxLANGUAGE_UNKNOWN = fontsc.wxLANGUAGE_UNKNOWN
wxLANGUAGE_ABKHAZIAN = fontsc.wxLANGUAGE_ABKHAZIAN
wxLANGUAGE_AFAR = fontsc.wxLANGUAGE_AFAR
wxLANGUAGE_AFRIKAANS = fontsc.wxLANGUAGE_AFRIKAANS
wxLANGUAGE_ALBANIAN = fontsc.wxLANGUAGE_ALBANIAN
wxLANGUAGE_AMHARIC = fontsc.wxLANGUAGE_AMHARIC
wxLANGUAGE_ARABIC = fontsc.wxLANGUAGE_ARABIC
wxLANGUAGE_ARABIC_ALGERIA = fontsc.wxLANGUAGE_ARABIC_ALGERIA
wxLANGUAGE_ARABIC_BAHRAIN = fontsc.wxLANGUAGE_ARABIC_BAHRAIN
wxLANGUAGE_ARABIC_EGYPT = fontsc.wxLANGUAGE_ARABIC_EGYPT
wxLANGUAGE_ARABIC_IRAQ = fontsc.wxLANGUAGE_ARABIC_IRAQ
wxLANGUAGE_ARABIC_JORDAN = fontsc.wxLANGUAGE_ARABIC_JORDAN
wxLANGUAGE_ARABIC_KUWAIT = fontsc.wxLANGUAGE_ARABIC_KUWAIT
wxLANGUAGE_ARABIC_LEBANON = fontsc.wxLANGUAGE_ARABIC_LEBANON
wxLANGUAGE_ARABIC_LIBYA = fontsc.wxLANGUAGE_ARABIC_LIBYA
wxLANGUAGE_ARABIC_MOROCCO = fontsc.wxLANGUAGE_ARABIC_MOROCCO
wxLANGUAGE_ARABIC_OMAN = fontsc.wxLANGUAGE_ARABIC_OMAN
wxLANGUAGE_ARABIC_QATAR = fontsc.wxLANGUAGE_ARABIC_QATAR
wxLANGUAGE_ARABIC_SAUDI_ARABIA = fontsc.wxLANGUAGE_ARABIC_SAUDI_ARABIA
wxLANGUAGE_ARABIC_SUDAN = fontsc.wxLANGUAGE_ARABIC_SUDAN
wxLANGUAGE_ARABIC_SYRIA = fontsc.wxLANGUAGE_ARABIC_SYRIA
wxLANGUAGE_ARABIC_TUNISIA = fontsc.wxLANGUAGE_ARABIC_TUNISIA
wxLANGUAGE_ARABIC_UAE = fontsc.wxLANGUAGE_ARABIC_UAE
wxLANGUAGE_ARABIC_YEMEN = fontsc.wxLANGUAGE_ARABIC_YEMEN
wxLANGUAGE_ARMENIAN = fontsc.wxLANGUAGE_ARMENIAN
wxLANGUAGE_ASSAMESE = fontsc.wxLANGUAGE_ASSAMESE
wxLANGUAGE_AYMARA = fontsc.wxLANGUAGE_AYMARA
wxLANGUAGE_AZERI = fontsc.wxLANGUAGE_AZERI
wxLANGUAGE_AZERI_CYRILLIC = fontsc.wxLANGUAGE_AZERI_CYRILLIC
wxLANGUAGE_AZERI_LATIN = fontsc.wxLANGUAGE_AZERI_LATIN
wxLANGUAGE_BASHKIR = fontsc.wxLANGUAGE_BASHKIR
wxLANGUAGE_BASQUE = fontsc.wxLANGUAGE_BASQUE
wxLANGUAGE_BELARUSIAN = fontsc.wxLANGUAGE_BELARUSIAN
wxLANGUAGE_BENGALI = fontsc.wxLANGUAGE_BENGALI
wxLANGUAGE_BHUTANI = fontsc.wxLANGUAGE_BHUTANI
wxLANGUAGE_BIHARI = fontsc.wxLANGUAGE_BIHARI
wxLANGUAGE_BISLAMA = fontsc.wxLANGUAGE_BISLAMA
wxLANGUAGE_BRETON = fontsc.wxLANGUAGE_BRETON
wxLANGUAGE_BULGARIAN = fontsc.wxLANGUAGE_BULGARIAN
wxLANGUAGE_BURMESE = fontsc.wxLANGUAGE_BURMESE
wxLANGUAGE_CAMBODIAN = fontsc.wxLANGUAGE_CAMBODIAN
wxLANGUAGE_CATALAN = fontsc.wxLANGUAGE_CATALAN
wxLANGUAGE_CHINESE = fontsc.wxLANGUAGE_CHINESE
wxLANGUAGE_CHINESE_SIMPLIFIED = fontsc.wxLANGUAGE_CHINESE_SIMPLIFIED
wxLANGUAGE_CHINESE_TRADITIONAL = fontsc.wxLANGUAGE_CHINESE_TRADITIONAL
wxLANGUAGE_CHINESE_HONGKONG = fontsc.wxLANGUAGE_CHINESE_HONGKONG
wxLANGUAGE_CHINESE_MACAU = fontsc.wxLANGUAGE_CHINESE_MACAU
wxLANGUAGE_CHINESE_SINGAPORE = fontsc.wxLANGUAGE_CHINESE_SINGAPORE
wxLANGUAGE_CHINESE_TAIWAN = fontsc.wxLANGUAGE_CHINESE_TAIWAN
wxLANGUAGE_CORSICAN = fontsc.wxLANGUAGE_CORSICAN
wxLANGUAGE_CROATIAN = fontsc.wxLANGUAGE_CROATIAN
wxLANGUAGE_CZECH = fontsc.wxLANGUAGE_CZECH
wxLANGUAGE_DANISH = fontsc.wxLANGUAGE_DANISH
wxLANGUAGE_DUTCH = fontsc.wxLANGUAGE_DUTCH
wxLANGUAGE_DUTCH_BELGIAN = fontsc.wxLANGUAGE_DUTCH_BELGIAN
wxLANGUAGE_ENGLISH = fontsc.wxLANGUAGE_ENGLISH
wxLANGUAGE_ENGLISH_UK = fontsc.wxLANGUAGE_ENGLISH_UK
wxLANGUAGE_ENGLISH_US = fontsc.wxLANGUAGE_ENGLISH_US
wxLANGUAGE_ENGLISH_AUSTRALIA = fontsc.wxLANGUAGE_ENGLISH_AUSTRALIA
wxLANGUAGE_ENGLISH_BELIZE = fontsc.wxLANGUAGE_ENGLISH_BELIZE
wxLANGUAGE_ENGLISH_BOTSWANA = fontsc.wxLANGUAGE_ENGLISH_BOTSWANA
wxLANGUAGE_ENGLISH_CANADA = fontsc.wxLANGUAGE_ENGLISH_CANADA
wxLANGUAGE_ENGLISH_CARIBBEAN = fontsc.wxLANGUAGE_ENGLISH_CARIBBEAN
wxLANGUAGE_ENGLISH_DENMARK = fontsc.wxLANGUAGE_ENGLISH_DENMARK
wxLANGUAGE_ENGLISH_EIRE = fontsc.wxLANGUAGE_ENGLISH_EIRE
wxLANGUAGE_ENGLISH_JAMAICA = fontsc.wxLANGUAGE_ENGLISH_JAMAICA
wxLANGUAGE_ENGLISH_NEW_ZEALAND = fontsc.wxLANGUAGE_ENGLISH_NEW_ZEALAND
wxLANGUAGE_ENGLISH_PHILIPPINES = fontsc.wxLANGUAGE_ENGLISH_PHILIPPINES
wxLANGUAGE_ENGLISH_SOUTH_AFRICA = fontsc.wxLANGUAGE_ENGLISH_SOUTH_AFRICA
wxLANGUAGE_ENGLISH_TRINIDAD = fontsc.wxLANGUAGE_ENGLISH_TRINIDAD
wxLANGUAGE_ENGLISH_ZIMBABWE = fontsc.wxLANGUAGE_ENGLISH_ZIMBABWE
wxLANGUAGE_ESPERANTO = fontsc.wxLANGUAGE_ESPERANTO
wxLANGUAGE_ESTONIAN = fontsc.wxLANGUAGE_ESTONIAN
wxLANGUAGE_FAEROESE = fontsc.wxLANGUAGE_FAEROESE
wxLANGUAGE_FARSI = fontsc.wxLANGUAGE_FARSI
wxLANGUAGE_FIJI = fontsc.wxLANGUAGE_FIJI
wxLANGUAGE_FINNISH = fontsc.wxLANGUAGE_FINNISH
wxLANGUAGE_FRENCH = fontsc.wxLANGUAGE_FRENCH
wxLANGUAGE_FRENCH_BELGIAN = fontsc.wxLANGUAGE_FRENCH_BELGIAN
wxLANGUAGE_FRENCH_CANADIAN = fontsc.wxLANGUAGE_FRENCH_CANADIAN
wxLANGUAGE_FRENCH_LUXEMBOURG = fontsc.wxLANGUAGE_FRENCH_LUXEMBOURG
wxLANGUAGE_FRENCH_MONACO = fontsc.wxLANGUAGE_FRENCH_MONACO
wxLANGUAGE_FRENCH_SWISS = fontsc.wxLANGUAGE_FRENCH_SWISS
wxLANGUAGE_FRISIAN = fontsc.wxLANGUAGE_FRISIAN
wxLANGUAGE_GALICIAN = fontsc.wxLANGUAGE_GALICIAN
wxLANGUAGE_GEORGIAN = fontsc.wxLANGUAGE_GEORGIAN
wxLANGUAGE_GERMAN = fontsc.wxLANGUAGE_GERMAN
wxLANGUAGE_GERMAN_AUSTRIAN = fontsc.wxLANGUAGE_GERMAN_AUSTRIAN
wxLANGUAGE_GERMAN_BELGIUM = fontsc.wxLANGUAGE_GERMAN_BELGIUM
wxLANGUAGE_GERMAN_LIECHTENSTEIN = fontsc.wxLANGUAGE_GERMAN_LIECHTENSTEIN
wxLANGUAGE_GERMAN_LUXEMBOURG = fontsc.wxLANGUAGE_GERMAN_LUXEMBOURG
wxLANGUAGE_GERMAN_SWISS = fontsc.wxLANGUAGE_GERMAN_SWISS
wxLANGUAGE_GREEK = fontsc.wxLANGUAGE_GREEK
wxLANGUAGE_GREENLANDIC = fontsc.wxLANGUAGE_GREENLANDIC
wxLANGUAGE_GUARANI = fontsc.wxLANGUAGE_GUARANI
wxLANGUAGE_GUJARATI = fontsc.wxLANGUAGE_GUJARATI
wxLANGUAGE_HAUSA = fontsc.wxLANGUAGE_HAUSA
wxLANGUAGE_HEBREW = fontsc.wxLANGUAGE_HEBREW
wxLANGUAGE_HINDI = fontsc.wxLANGUAGE_HINDI
wxLANGUAGE_HUNGARIAN = fontsc.wxLANGUAGE_HUNGARIAN
wxLANGUAGE_ICELANDIC = fontsc.wxLANGUAGE_ICELANDIC
wxLANGUAGE_INDONESIAN = fontsc.wxLANGUAGE_INDONESIAN
wxLANGUAGE_INTERLINGUA = fontsc.wxLANGUAGE_INTERLINGUA
wxLANGUAGE_INTERLINGUE = fontsc.wxLANGUAGE_INTERLINGUE
wxLANGUAGE_INUKTITUT = fontsc.wxLANGUAGE_INUKTITUT
wxLANGUAGE_INUPIAK = fontsc.wxLANGUAGE_INUPIAK
wxLANGUAGE_IRISH = fontsc.wxLANGUAGE_IRISH
wxLANGUAGE_ITALIAN = fontsc.wxLANGUAGE_ITALIAN
wxLANGUAGE_ITALIAN_SWISS = fontsc.wxLANGUAGE_ITALIAN_SWISS
wxLANGUAGE_JAPANESE = fontsc.wxLANGUAGE_JAPANESE
wxLANGUAGE_JAVANESE = fontsc.wxLANGUAGE_JAVANESE
wxLANGUAGE_KANNADA = fontsc.wxLANGUAGE_KANNADA
wxLANGUAGE_KASHMIRI = fontsc.wxLANGUAGE_KASHMIRI
wxLANGUAGE_KASHMIRI_INDIA = fontsc.wxLANGUAGE_KASHMIRI_INDIA
wxLANGUAGE_KAZAKH = fontsc.wxLANGUAGE_KAZAKH
wxLANGUAGE_KERNEWEK = fontsc.wxLANGUAGE_KERNEWEK
wxLANGUAGE_KINYARWANDA = fontsc.wxLANGUAGE_KINYARWANDA
wxLANGUAGE_KIRGHIZ = fontsc.wxLANGUAGE_KIRGHIZ
wxLANGUAGE_KIRUNDI = fontsc.wxLANGUAGE_KIRUNDI
wxLANGUAGE_KONKANI = fontsc.wxLANGUAGE_KONKANI
wxLANGUAGE_KOREAN = fontsc.wxLANGUAGE_KOREAN
wxLANGUAGE_KURDISH = fontsc.wxLANGUAGE_KURDISH
wxLANGUAGE_LAOTHIAN = fontsc.wxLANGUAGE_LAOTHIAN
wxLANGUAGE_LATIN = fontsc.wxLANGUAGE_LATIN
wxLANGUAGE_LATVIAN = fontsc.wxLANGUAGE_LATVIAN
wxLANGUAGE_LINGALA = fontsc.wxLANGUAGE_LINGALA
wxLANGUAGE_LITHUANIAN = fontsc.wxLANGUAGE_LITHUANIAN
wxLANGUAGE_MACEDONIAN = fontsc.wxLANGUAGE_MACEDONIAN
wxLANGUAGE_MALAGASY = fontsc.wxLANGUAGE_MALAGASY
wxLANGUAGE_MALAY = fontsc.wxLANGUAGE_MALAY
wxLANGUAGE_MALAYALAM = fontsc.wxLANGUAGE_MALAYALAM
wxLANGUAGE_MALAY_BRUNEI_DARUSSALAM = fontsc.wxLANGUAGE_MALAY_BRUNEI_DARUSSALAM
wxLANGUAGE_MALAY_MALAYSIA = fontsc.wxLANGUAGE_MALAY_MALAYSIA
wxLANGUAGE_MALTESE = fontsc.wxLANGUAGE_MALTESE
wxLANGUAGE_MANIPURI = fontsc.wxLANGUAGE_MANIPURI
wxLANGUAGE_MAORI = fontsc.wxLANGUAGE_MAORI
wxLANGUAGE_MARATHI = fontsc.wxLANGUAGE_MARATHI
wxLANGUAGE_MOLDAVIAN = fontsc.wxLANGUAGE_MOLDAVIAN
wxLANGUAGE_MONGOLIAN = fontsc.wxLANGUAGE_MONGOLIAN
wxLANGUAGE_NAURU = fontsc.wxLANGUAGE_NAURU
wxLANGUAGE_NEPALI = fontsc.wxLANGUAGE_NEPALI
wxLANGUAGE_NEPALI_INDIA = fontsc.wxLANGUAGE_NEPALI_INDIA
wxLANGUAGE_NORWEGIAN_BOKMAL = fontsc.wxLANGUAGE_NORWEGIAN_BOKMAL
wxLANGUAGE_NORWEGIAN_NYNORSK = fontsc.wxLANGUAGE_NORWEGIAN_NYNORSK
wxLANGUAGE_OCCITAN = fontsc.wxLANGUAGE_OCCITAN
wxLANGUAGE_ORIYA = fontsc.wxLANGUAGE_ORIYA
wxLANGUAGE_OROMO = fontsc.wxLANGUAGE_OROMO
wxLANGUAGE_PASHTO = fontsc.wxLANGUAGE_PASHTO
wxLANGUAGE_POLISH = fontsc.wxLANGUAGE_POLISH
wxLANGUAGE_PORTUGUESE = fontsc.wxLANGUAGE_PORTUGUESE
wxLANGUAGE_PORTUGUESE_BRAZILIAN = fontsc.wxLANGUAGE_PORTUGUESE_BRAZILIAN
wxLANGUAGE_PUNJABI = fontsc.wxLANGUAGE_PUNJABI
wxLANGUAGE_QUECHUA = fontsc.wxLANGUAGE_QUECHUA
wxLANGUAGE_RHAETO_ROMANCE = fontsc.wxLANGUAGE_RHAETO_ROMANCE
wxLANGUAGE_ROMANIAN = fontsc.wxLANGUAGE_ROMANIAN
wxLANGUAGE_RUSSIAN = fontsc.wxLANGUAGE_RUSSIAN
wxLANGUAGE_RUSSIAN_UKRAINE = fontsc.wxLANGUAGE_RUSSIAN_UKRAINE
wxLANGUAGE_SAMOAN = fontsc.wxLANGUAGE_SAMOAN
wxLANGUAGE_SANGHO = fontsc.wxLANGUAGE_SANGHO
wxLANGUAGE_SANSKRIT = fontsc.wxLANGUAGE_SANSKRIT
wxLANGUAGE_SCOTS_GAELIC = fontsc.wxLANGUAGE_SCOTS_GAELIC
wxLANGUAGE_SERBIAN = fontsc.wxLANGUAGE_SERBIAN
wxLANGUAGE_SERBIAN_CYRILLIC = fontsc.wxLANGUAGE_SERBIAN_CYRILLIC
wxLANGUAGE_SERBIAN_LATIN = fontsc.wxLANGUAGE_SERBIAN_LATIN
wxLANGUAGE_SERBO_CROATIAN = fontsc.wxLANGUAGE_SERBO_CROATIAN
wxLANGUAGE_SESOTHO = fontsc.wxLANGUAGE_SESOTHO
wxLANGUAGE_SETSWANA = fontsc.wxLANGUAGE_SETSWANA
wxLANGUAGE_SHONA = fontsc.wxLANGUAGE_SHONA
wxLANGUAGE_SINDHI = fontsc.wxLANGUAGE_SINDHI
wxLANGUAGE_SINHALESE = fontsc.wxLANGUAGE_SINHALESE
wxLANGUAGE_SISWATI = fontsc.wxLANGUAGE_SISWATI
wxLANGUAGE_SLOVAK = fontsc.wxLANGUAGE_SLOVAK
wxLANGUAGE_SLOVENIAN = fontsc.wxLANGUAGE_SLOVENIAN
wxLANGUAGE_SOMALI = fontsc.wxLANGUAGE_SOMALI
wxLANGUAGE_SPANISH = fontsc.wxLANGUAGE_SPANISH
wxLANGUAGE_SPANISH_ARGENTINA = fontsc.wxLANGUAGE_SPANISH_ARGENTINA
wxLANGUAGE_SPANISH_BOLIVIA = fontsc.wxLANGUAGE_SPANISH_BOLIVIA
wxLANGUAGE_SPANISH_CHILE = fontsc.wxLANGUAGE_SPANISH_CHILE
wxLANGUAGE_SPANISH_COLOMBIA = fontsc.wxLANGUAGE_SPANISH_COLOMBIA
wxLANGUAGE_SPANISH_COSTA_RICA = fontsc.wxLANGUAGE_SPANISH_COSTA_RICA
wxLANGUAGE_SPANISH_DOMINICAN_REPUBLIC = fontsc.wxLANGUAGE_SPANISH_DOMINICAN_REPUBLIC
wxLANGUAGE_SPANISH_ECUADOR = fontsc.wxLANGUAGE_SPANISH_ECUADOR
wxLANGUAGE_SPANISH_EL_SALVADOR = fontsc.wxLANGUAGE_SPANISH_EL_SALVADOR
wxLANGUAGE_SPANISH_GUATEMALA = fontsc.wxLANGUAGE_SPANISH_GUATEMALA
wxLANGUAGE_SPANISH_HONDURAS = fontsc.wxLANGUAGE_SPANISH_HONDURAS
wxLANGUAGE_SPANISH_MEXICAN = fontsc.wxLANGUAGE_SPANISH_MEXICAN
wxLANGUAGE_SPANISH_MODERN = fontsc.wxLANGUAGE_SPANISH_MODERN
wxLANGUAGE_SPANISH_NICARAGUA = fontsc.wxLANGUAGE_SPANISH_NICARAGUA
wxLANGUAGE_SPANISH_PANAMA = fontsc.wxLANGUAGE_SPANISH_PANAMA
wxLANGUAGE_SPANISH_PARAGUAY = fontsc.wxLANGUAGE_SPANISH_PARAGUAY
wxLANGUAGE_SPANISH_PERU = fontsc.wxLANGUAGE_SPANISH_PERU
wxLANGUAGE_SPANISH_PUERTO_RICO = fontsc.wxLANGUAGE_SPANISH_PUERTO_RICO
wxLANGUAGE_SPANISH_URUGUAY = fontsc.wxLANGUAGE_SPANISH_URUGUAY
wxLANGUAGE_SPANISH_US = fontsc.wxLANGUAGE_SPANISH_US
wxLANGUAGE_SPANISH_VENEZUELA = fontsc.wxLANGUAGE_SPANISH_VENEZUELA
wxLANGUAGE_SUNDANESE = fontsc.wxLANGUAGE_SUNDANESE
wxLANGUAGE_SWAHILI = fontsc.wxLANGUAGE_SWAHILI
wxLANGUAGE_SWEDISH = fontsc.wxLANGUAGE_SWEDISH
wxLANGUAGE_SWEDISH_FINLAND = fontsc.wxLANGUAGE_SWEDISH_FINLAND
wxLANGUAGE_TAGALOG = fontsc.wxLANGUAGE_TAGALOG
wxLANGUAGE_TAJIK = fontsc.wxLANGUAGE_TAJIK
wxLANGUAGE_TAMIL = fontsc.wxLANGUAGE_TAMIL
wxLANGUAGE_TATAR = fontsc.wxLANGUAGE_TATAR
wxLANGUAGE_TELUGU = fontsc.wxLANGUAGE_TELUGU
wxLANGUAGE_THAI = fontsc.wxLANGUAGE_THAI
wxLANGUAGE_TIBETAN = fontsc.wxLANGUAGE_TIBETAN
wxLANGUAGE_TIGRINYA = fontsc.wxLANGUAGE_TIGRINYA
wxLANGUAGE_TONGA = fontsc.wxLANGUAGE_TONGA
wxLANGUAGE_TSONGA = fontsc.wxLANGUAGE_TSONGA
wxLANGUAGE_TURKISH = fontsc.wxLANGUAGE_TURKISH
wxLANGUAGE_TURKMEN = fontsc.wxLANGUAGE_TURKMEN
wxLANGUAGE_TWI = fontsc.wxLANGUAGE_TWI
wxLANGUAGE_UIGHUR = fontsc.wxLANGUAGE_UIGHUR
wxLANGUAGE_UKRAINIAN = fontsc.wxLANGUAGE_UKRAINIAN
wxLANGUAGE_URDU = fontsc.wxLANGUAGE_URDU
wxLANGUAGE_URDU_INDIA = fontsc.wxLANGUAGE_URDU_INDIA
wxLANGUAGE_URDU_PAKISTAN = fontsc.wxLANGUAGE_URDU_PAKISTAN
wxLANGUAGE_UZBEK = fontsc.wxLANGUAGE_UZBEK
wxLANGUAGE_UZBEK_CYRILLIC = fontsc.wxLANGUAGE_UZBEK_CYRILLIC
wxLANGUAGE_UZBEK_LATIN = fontsc.wxLANGUAGE_UZBEK_LATIN
wxLANGUAGE_VIETNAMESE = fontsc.wxLANGUAGE_VIETNAMESE
wxLANGUAGE_VOLAPUK = fontsc.wxLANGUAGE_VOLAPUK
wxLANGUAGE_WELSH = fontsc.wxLANGUAGE_WELSH
wxLANGUAGE_WOLOF = fontsc.wxLANGUAGE_WOLOF
wxLANGUAGE_XHOSA = fontsc.wxLANGUAGE_XHOSA
wxLANGUAGE_YIDDISH = fontsc.wxLANGUAGE_YIDDISH
wxLANGUAGE_YORUBA = fontsc.wxLANGUAGE_YORUBA
wxLANGUAGE_ZHUANG = fontsc.wxLANGUAGE_ZHUANG
wxLANGUAGE_ZULU = fontsc.wxLANGUAGE_ZULU
wxLANGUAGE_USER_DEFINED = fontsc.wxLANGUAGE_USER_DEFINED
wxLOCALE_CAT_NUMBER = fontsc.wxLOCALE_CAT_NUMBER
wxLOCALE_CAT_DATE = fontsc.wxLOCALE_CAT_DATE
wxLOCALE_CAT_MONEY = fontsc.wxLOCALE_CAT_MONEY
wxLOCALE_CAT_MAX = fontsc.wxLOCALE_CAT_MAX
wxLOCALE_THOUSANDS_SEP = fontsc.wxLOCALE_THOUSANDS_SEP
wxLOCALE_DECIMAL_POINT = fontsc.wxLOCALE_DECIMAL_POINT
wxLOCALE_LOAD_DEFAULT = fontsc.wxLOCALE_LOAD_DEFAULT
wxLOCALE_CONV_ENCODING = fontsc.wxLOCALE_CONV_ENCODING
wxCONVERT_STRICT = fontsc.wxCONVERT_STRICT
wxCONVERT_SUBSTITUTE = fontsc.wxCONVERT_SUBSTITUTE
wxPLATFORM_CURRENT = fontsc.wxPLATFORM_CURRENT
wxPLATFORM_UNIX = fontsc.wxPLATFORM_UNIX
wxPLATFORM_WINDOWS = fontsc.wxPLATFORM_WINDOWS
wxPLATFORM_OS2 = fontsc.wxPLATFORM_OS2
wxPLATFORM_MAC = fontsc.wxPLATFORM_MAC
