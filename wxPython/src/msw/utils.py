# This file was created automatically by SWIG.
import utilsc
import string
class wxConfigBasePtr :
    Type_Unknown = utilsc.wxConfigBase_Type_Unknown
    Type_String = utilsc.wxConfigBase_Type_String
    Type_Boolean = utilsc.wxConfigBase_Type_Boolean
    Type_Integer = utilsc.wxConfigBase_Type_Integer
    Type_Float = utilsc.wxConfigBase_Type_Float
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __del__(self,utilsc=utilsc):
        if self.thisown == 1 :
            utilsc.delete_wxConfigBase(self)
    def DeleteAll(self, *_args, **_kwargs):
        val = apply(utilsc.wxConfigBase_DeleteAll,(self,) + _args, _kwargs)
        return val
    def DeleteEntry(self, *_args, **_kwargs):
        val = apply(utilsc.wxConfigBase_DeleteEntry,(self,) + _args, _kwargs)
        return val
    def DeleteGroup(self, *_args, **_kwargs):
        val = apply(utilsc.wxConfigBase_DeleteGroup,(self,) + _args, _kwargs)
        return val
    def Exists(self, *_args, **_kwargs):
        val = apply(utilsc.wxConfigBase_Exists,(self,) + _args, _kwargs)
        return val
    def Flush(self, *_args, **_kwargs):
        val = apply(utilsc.wxConfigBase_Flush,(self,) + _args, _kwargs)
        return val
    def GetAppName(self, *_args, **_kwargs):
        val = apply(utilsc.wxConfigBase_GetAppName,(self,) + _args, _kwargs)
        return val
    def GetFirstGroup(self, *_args, **_kwargs):
        val = apply(utilsc.wxConfigBase_GetFirstGroup,(self,) + _args, _kwargs)
        return val
    def GetFirstEntry(self, *_args, **_kwargs):
        val = apply(utilsc.wxConfigBase_GetFirstEntry,(self,) + _args, _kwargs)
        return val
    def GetNextGroup(self, *_args, **_kwargs):
        val = apply(utilsc.wxConfigBase_GetNextGroup,(self,) + _args, _kwargs)
        return val
    def GetNextEntry(self, *_args, **_kwargs):
        val = apply(utilsc.wxConfigBase_GetNextEntry,(self,) + _args, _kwargs)
        return val
    def GetNumberOfEntries(self, *_args, **_kwargs):
        val = apply(utilsc.wxConfigBase_GetNumberOfEntries,(self,) + _args, _kwargs)
        return val
    def GetNumberOfGroups(self, *_args, **_kwargs):
        val = apply(utilsc.wxConfigBase_GetNumberOfGroups,(self,) + _args, _kwargs)
        return val
    def GetPath(self, *_args, **_kwargs):
        val = apply(utilsc.wxConfigBase_GetPath,(self,) + _args, _kwargs)
        return val
    def GetVendorName(self, *_args, **_kwargs):
        val = apply(utilsc.wxConfigBase_GetVendorName,(self,) + _args, _kwargs)
        return val
    def HasEntry(self, *_args, **_kwargs):
        val = apply(utilsc.wxConfigBase_HasEntry,(self,) + _args, _kwargs)
        return val
    def HasGroup(self, *_args, **_kwargs):
        val = apply(utilsc.wxConfigBase_HasGroup,(self,) + _args, _kwargs)
        return val
    def IsExpandingEnvVars(self, *_args, **_kwargs):
        val = apply(utilsc.wxConfigBase_IsExpandingEnvVars,(self,) + _args, _kwargs)
        return val
    def IsRecordingDefaults(self, *_args, **_kwargs):
        val = apply(utilsc.wxConfigBase_IsRecordingDefaults,(self,) + _args, _kwargs)
        return val
    def Read(self, *_args, **_kwargs):
        val = apply(utilsc.wxConfigBase_Read,(self,) + _args, _kwargs)
        return val
    def ReadInt(self, *_args, **_kwargs):
        val = apply(utilsc.wxConfigBase_ReadInt,(self,) + _args, _kwargs)
        return val
    def ReadFloat(self, *_args, **_kwargs):
        val = apply(utilsc.wxConfigBase_ReadFloat,(self,) + _args, _kwargs)
        return val
    def SetExpandEnvVars(self, *_args, **_kwargs):
        val = apply(utilsc.wxConfigBase_SetExpandEnvVars,(self,) + _args, _kwargs)
        return val
    def SetPath(self, *_args, **_kwargs):
        val = apply(utilsc.wxConfigBase_SetPath,(self,) + _args, _kwargs)
        return val
    def SetRecordDefaults(self, *_args, **_kwargs):
        val = apply(utilsc.wxConfigBase_SetRecordDefaults,(self,) + _args, _kwargs)
        return val
    def SetAppName(self, *_args, **_kwargs):
        val = apply(utilsc.wxConfigBase_SetAppName,(self,) + _args, _kwargs)
        return val
    def SetVendorName(self, *_args, **_kwargs):
        val = apply(utilsc.wxConfigBase_SetVendorName,(self,) + _args, _kwargs)
        return val
    def SetStyle(self, *_args, **_kwargs):
        val = apply(utilsc.wxConfigBase_SetStyle,(self,) + _args, _kwargs)
        return val
    def GetStyle(self, *_args, **_kwargs):
        val = apply(utilsc.wxConfigBase_GetStyle,(self,) + _args, _kwargs)
        return val
    def Write(self, *_args, **_kwargs):
        val = apply(utilsc.wxConfigBase_Write,(self,) + _args, _kwargs)
        return val
    def WriteInt(self, *_args, **_kwargs):
        val = apply(utilsc.wxConfigBase_WriteInt,(self,) + _args, _kwargs)
        return val
    def WriteFloat(self, *_args, **_kwargs):
        val = apply(utilsc.wxConfigBase_WriteFloat,(self,) + _args, _kwargs)
        return val
    def GetEntryType(self, *_args, **_kwargs):
        val = apply(utilsc.wxConfigBase_GetEntryType,(self,) + _args, _kwargs)
        return val
    def RenameEntry(self, *_args, **_kwargs):
        val = apply(utilsc.wxConfigBase_RenameEntry,(self,) + _args, _kwargs)
        return val
    def RenameGroup(self, *_args, **_kwargs):
        val = apply(utilsc.wxConfigBase_RenameGroup,(self,) + _args, _kwargs)
        return val
    def ExpandEnvVars(self, *_args, **_kwargs):
        val = apply(utilsc.wxConfigBase_ExpandEnvVars,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxConfigBase instance at %s>" % (self.this,)
class wxConfigBase(wxConfigBasePtr):
    def __init__(self,this):
        self.this = this




class wxConfigPtr(wxConfigBasePtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __del__(self,utilsc=utilsc):
        if self.thisown == 1 :
            utilsc.delete_wxConfig(self)
    def __repr__(self):
        return "<C wxConfig instance at %s>" % (self.this,)
class wxConfig(wxConfigPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(utilsc.new_wxConfig,_args,_kwargs)
        self.thisown = 1




class wxFileConfigPtr(wxConfigBasePtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __del__(self,utilsc=utilsc):
        if self.thisown == 1 :
            utilsc.delete_wxFileConfig(self)
    def __repr__(self):
        return "<C wxFileConfig instance at %s>" % (self.this,)
class wxFileConfig(wxFileConfigPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(utilsc.new_wxFileConfig,_args,_kwargs)
        self.thisown = 1




class wxDateTimePtr :
    Local = utilsc.wxDateTime_Local
    GMT_12 = utilsc.wxDateTime_GMT_12
    GMT_11 = utilsc.wxDateTime_GMT_11
    GMT_10 = utilsc.wxDateTime_GMT_10
    GMT_9 = utilsc.wxDateTime_GMT_9
    GMT_8 = utilsc.wxDateTime_GMT_8
    GMT_7 = utilsc.wxDateTime_GMT_7
    GMT_6 = utilsc.wxDateTime_GMT_6
    GMT_5 = utilsc.wxDateTime_GMT_5
    GMT_4 = utilsc.wxDateTime_GMT_4
    GMT_3 = utilsc.wxDateTime_GMT_3
    GMT_2 = utilsc.wxDateTime_GMT_2
    GMT_1 = utilsc.wxDateTime_GMT_1
    GMT0 = utilsc.wxDateTime_GMT0
    GMT1 = utilsc.wxDateTime_GMT1
    GMT2 = utilsc.wxDateTime_GMT2
    GMT3 = utilsc.wxDateTime_GMT3
    GMT4 = utilsc.wxDateTime_GMT4
    GMT5 = utilsc.wxDateTime_GMT5
    GMT6 = utilsc.wxDateTime_GMT6
    GMT7 = utilsc.wxDateTime_GMT7
    GMT8 = utilsc.wxDateTime_GMT8
    GMT9 = utilsc.wxDateTime_GMT9
    GMT10 = utilsc.wxDateTime_GMT10
    GMT11 = utilsc.wxDateTime_GMT11
    GMT12 = utilsc.wxDateTime_GMT12
    WET = utilsc.wxDateTime_WET
    WEST = utilsc.wxDateTime_WEST
    CET = utilsc.wxDateTime_CET
    CEST = utilsc.wxDateTime_CEST
    EET = utilsc.wxDateTime_EET
    EEST = utilsc.wxDateTime_EEST
    MSK = utilsc.wxDateTime_MSK
    MSD = utilsc.wxDateTime_MSD
    AST = utilsc.wxDateTime_AST
    ADT = utilsc.wxDateTime_ADT
    EST = utilsc.wxDateTime_EST
    EDT = utilsc.wxDateTime_EDT
    CST = utilsc.wxDateTime_CST
    CDT = utilsc.wxDateTime_CDT
    MST = utilsc.wxDateTime_MST
    MDT = utilsc.wxDateTime_MDT
    PST = utilsc.wxDateTime_PST
    PDT = utilsc.wxDateTime_PDT
    HST = utilsc.wxDateTime_HST
    AKST = utilsc.wxDateTime_AKST
    AKDT = utilsc.wxDateTime_AKDT
    A_WST = utilsc.wxDateTime_A_WST
    A_CST = utilsc.wxDateTime_A_CST
    A_EST = utilsc.wxDateTime_A_EST
    A_ESST = utilsc.wxDateTime_A_ESST
    UTC = utilsc.wxDateTime_UTC
    Gregorian = utilsc.wxDateTime_Gregorian
    Julian = utilsc.wxDateTime_Julian
    Country_Unknown = utilsc.wxDateTime_Country_Unknown
    Country_Default = utilsc.wxDateTime_Country_Default
    Country_WesternEurope_Start = utilsc.wxDateTime_Country_WesternEurope_Start
    Country_EEC = utilsc.wxDateTime_Country_EEC
    France = utilsc.wxDateTime_France
    Germany = utilsc.wxDateTime_Germany
    UK = utilsc.wxDateTime_UK
    Country_WesternEurope_End = utilsc.wxDateTime_Country_WesternEurope_End
    Russia = utilsc.wxDateTime_Russia
    USA = utilsc.wxDateTime_USA
    Jan = utilsc.wxDateTime_Jan
    Feb = utilsc.wxDateTime_Feb
    Mar = utilsc.wxDateTime_Mar
    Apr = utilsc.wxDateTime_Apr
    May = utilsc.wxDateTime_May
    Jun = utilsc.wxDateTime_Jun
    Jul = utilsc.wxDateTime_Jul
    Aug = utilsc.wxDateTime_Aug
    Sep = utilsc.wxDateTime_Sep
    Oct = utilsc.wxDateTime_Oct
    Nov = utilsc.wxDateTime_Nov
    Dec = utilsc.wxDateTime_Dec
    Inv_Month = utilsc.wxDateTime_Inv_Month
    Sun = utilsc.wxDateTime_Sun
    Mon = utilsc.wxDateTime_Mon
    Tue = utilsc.wxDateTime_Tue
    Wed = utilsc.wxDateTime_Wed
    Thu = utilsc.wxDateTime_Thu
    Fri = utilsc.wxDateTime_Fri
    Sat = utilsc.wxDateTime_Sat
    Inv_WeekDay = utilsc.wxDateTime_Inv_WeekDay
    Inv_Year = utilsc.wxDateTime_Inv_Year
    Name_Full = utilsc.wxDateTime_Name_Full
    Name_Abbr = utilsc.wxDateTime_Name_Abbr
    Default_First = utilsc.wxDateTime_Default_First
    Monday_First = utilsc.wxDateTime_Monday_First
    Sunday_First = utilsc.wxDateTime_Sunday_First
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __del__(self,utilsc=utilsc):
        if self.thisown == 1 :
            utilsc.delete_wxDateTime(self)
    def SetToCurrent(self, *_args, **_kwargs):
        val = apply(utilsc.wxDateTime_SetToCurrent,(self,) + _args, _kwargs)
        if val: val = wxDateTimePtr(val) 
        return val
    def SetTimeT(self, *_args, **_kwargs):
        val = apply(utilsc.wxDateTime_SetTimeT,(self,) + _args, _kwargs)
        if val: val = wxDateTimePtr(val) 
        return val
    def SetJDN(self, *_args, **_kwargs):
        val = apply(utilsc.wxDateTime_SetJDN,(self,) + _args, _kwargs)
        if val: val = wxDateTimePtr(val) 
        return val
    def SetHMS(self, *_args, **_kwargs):
        val = apply(utilsc.wxDateTime_SetHMS,(self,) + _args, _kwargs)
        if val: val = wxDateTimePtr(val) 
        return val
    def Set(self, *_args, **_kwargs):
        val = apply(utilsc.wxDateTime_Set,(self,) + _args, _kwargs)
        if val: val = wxDateTimePtr(val) 
        return val
    def ResetTime(self, *_args, **_kwargs):
        val = apply(utilsc.wxDateTime_ResetTime,(self,) + _args, _kwargs)
        if val: val = wxDateTimePtr(val) 
        return val
    def SetYear(self, *_args, **_kwargs):
        val = apply(utilsc.wxDateTime_SetYear,(self,) + _args, _kwargs)
        if val: val = wxDateTimePtr(val) 
        return val
    def SetMonth(self, *_args, **_kwargs):
        val = apply(utilsc.wxDateTime_SetMonth,(self,) + _args, _kwargs)
        if val: val = wxDateTimePtr(val) 
        return val
    def SetDay(self, *_args, **_kwargs):
        val = apply(utilsc.wxDateTime_SetDay,(self,) + _args, _kwargs)
        if val: val = wxDateTimePtr(val) 
        return val
    def SetHour(self, *_args, **_kwargs):
        val = apply(utilsc.wxDateTime_SetHour,(self,) + _args, _kwargs)
        if val: val = wxDateTimePtr(val) 
        return val
    def SetMinute(self, *_args, **_kwargs):
        val = apply(utilsc.wxDateTime_SetMinute,(self,) + _args, _kwargs)
        if val: val = wxDateTimePtr(val) 
        return val
    def SetSecond(self, *_args, **_kwargs):
        val = apply(utilsc.wxDateTime_SetSecond,(self,) + _args, _kwargs)
        if val: val = wxDateTimePtr(val) 
        return val
    def SetMillisecond(self, *_args, **_kwargs):
        val = apply(utilsc.wxDateTime_SetMillisecond,(self,) + _args, _kwargs)
        if val: val = wxDateTimePtr(val) 
        return val
    def SetToWeekDayInSameWeek(self, *_args, **_kwargs):
        val = apply(utilsc.wxDateTime_SetToWeekDayInSameWeek,(self,) + _args, _kwargs)
        if val: val = wxDateTimePtr(val) 
        return val
    def GetWeekDayInSameWeek(self, *_args, **_kwargs):
        val = apply(utilsc.wxDateTime_GetWeekDayInSameWeek,(self,) + _args, _kwargs)
        if val: val = wxDateTimePtr(val) ; val.thisown = 1
        return val
    def SetToNextWeekDay(self, *_args, **_kwargs):
        val = apply(utilsc.wxDateTime_SetToNextWeekDay,(self,) + _args, _kwargs)
        if val: val = wxDateTimePtr(val) 
        return val
    def GetNextWeekDay(self, *_args, **_kwargs):
        val = apply(utilsc.wxDateTime_GetNextWeekDay,(self,) + _args, _kwargs)
        if val: val = wxDateTimePtr(val) ; val.thisown = 1
        return val
    def SetToPrevWeekDay(self, *_args, **_kwargs):
        val = apply(utilsc.wxDateTime_SetToPrevWeekDay,(self,) + _args, _kwargs)
        if val: val = wxDateTimePtr(val) 
        return val
    def GetPrevWeekDay(self, *_args, **_kwargs):
        val = apply(utilsc.wxDateTime_GetPrevWeekDay,(self,) + _args, _kwargs)
        if val: val = wxDateTimePtr(val) ; val.thisown = 1
        return val
    def SetToWeekDay(self, *_args, **_kwargs):
        val = apply(utilsc.wxDateTime_SetToWeekDay,(self,) + _args, _kwargs)
        return val
    def GetWeekDay(self, *_args, **_kwargs):
        val = apply(utilsc.wxDateTime_GetWeekDay,(self,) + _args, _kwargs)
        if val: val = wxDateTimePtr(val) ; val.thisown = 1
        return val
    def SetToLastWeekDay(self, *_args, **_kwargs):
        val = apply(utilsc.wxDateTime_SetToLastWeekDay,(self,) + _args, _kwargs)
        return val
    def GetLastWeekDay(self, *_args, **_kwargs):
        val = apply(utilsc.wxDateTime_GetLastWeekDay,(self,) + _args, _kwargs)
        if val: val = wxDateTimePtr(val) ; val.thisown = 1
        return val
    def SetToTheWeek(self, *_args, **_kwargs):
        val = apply(utilsc.wxDateTime_SetToTheWeek,(self,) + _args, _kwargs)
        return val
    def GetWeek(self, *_args, **_kwargs):
        val = apply(utilsc.wxDateTime_GetWeek,(self,) + _args, _kwargs)
        if val: val = wxDateTimePtr(val) ; val.thisown = 1
        return val
    def SetToLastMonthDay(self, *_args, **_kwargs):
        val = apply(utilsc.wxDateTime_SetToLastMonthDay,(self,) + _args, _kwargs)
        if val: val = wxDateTimePtr(val) 
        return val
    def GetLastMonthDay(self, *_args, **_kwargs):
        val = apply(utilsc.wxDateTime_GetLastMonthDay,(self,) + _args, _kwargs)
        if val: val = wxDateTimePtr(val) ; val.thisown = 1
        return val
    def SetToYearDay(self, *_args, **_kwargs):
        val = apply(utilsc.wxDateTime_SetToYearDay,(self,) + _args, _kwargs)
        if val: val = wxDateTimePtr(val) 
        return val
    def GetYearDay(self, *_args, **_kwargs):
        val = apply(utilsc.wxDateTime_GetYearDay,(self,) + _args, _kwargs)
        if val: val = wxDateTimePtr(val) ; val.thisown = 1
        return val
    def GetJulianDayNumber(self, *_args, **_kwargs):
        val = apply(utilsc.wxDateTime_GetJulianDayNumber,(self,) + _args, _kwargs)
        return val
    def GetJDN(self, *_args, **_kwargs):
        val = apply(utilsc.wxDateTime_GetJDN,(self,) + _args, _kwargs)
        return val
    def GetModifiedJulianDayNumber(self, *_args, **_kwargs):
        val = apply(utilsc.wxDateTime_GetModifiedJulianDayNumber,(self,) + _args, _kwargs)
        return val
    def GetMJD(self, *_args, **_kwargs):
        val = apply(utilsc.wxDateTime_GetMJD,(self,) + _args, _kwargs)
        return val
    def GetRataDie(self, *_args, **_kwargs):
        val = apply(utilsc.wxDateTime_GetRataDie,(self,) + _args, _kwargs)
        return val
    def ToTimezone(self, *_args, **_kwargs):
        val = apply(utilsc.wxDateTime_ToTimezone,(self,) + _args, _kwargs)
        if val: val = wxDateTimePtr(val) ; val.thisown = 1
        return val
    def MakeTimezone(self, *_args, **_kwargs):
        val = apply(utilsc.wxDateTime_MakeTimezone,(self,) + _args, _kwargs)
        if val: val = wxDateTimePtr(val) 
        return val
    def ToGMT(self, *_args, **_kwargs):
        val = apply(utilsc.wxDateTime_ToGMT,(self,) + _args, _kwargs)
        if val: val = wxDateTimePtr(val) ; val.thisown = 1
        return val
    def MakeGMT(self, *_args, **_kwargs):
        val = apply(utilsc.wxDateTime_MakeGMT,(self,) + _args, _kwargs)
        if val: val = wxDateTimePtr(val) 
        return val
    def IsDST(self, *_args, **_kwargs):
        val = apply(utilsc.wxDateTime_IsDST,(self,) + _args, _kwargs)
        return val
    def IsValid(self, *_args, **_kwargs):
        val = apply(utilsc.wxDateTime_IsValid,(self,) + _args, _kwargs)
        return val
    def GetTicks(self, *_args, **_kwargs):
        val = apply(utilsc.wxDateTime_GetTicks,(self,) + _args, _kwargs)
        return val
    def GetYear(self, *_args, **_kwargs):
        val = apply(utilsc.wxDateTime_GetYear,(self,) + _args, _kwargs)
        return val
    def GetMonth(self, *_args, **_kwargs):
        val = apply(utilsc.wxDateTime_GetMonth,(self,) + _args, _kwargs)
        return val
    def GetDay(self, *_args, **_kwargs):
        val = apply(utilsc.wxDateTime_GetDay,(self,) + _args, _kwargs)
        return val
    def GetHour(self, *_args, **_kwargs):
        val = apply(utilsc.wxDateTime_GetHour,(self,) + _args, _kwargs)
        return val
    def GetMinute(self, *_args, **_kwargs):
        val = apply(utilsc.wxDateTime_GetMinute,(self,) + _args, _kwargs)
        return val
    def GetSecond(self, *_args, **_kwargs):
        val = apply(utilsc.wxDateTime_GetSecond,(self,) + _args, _kwargs)
        return val
    def GetMillisecond(self, *_args, **_kwargs):
        val = apply(utilsc.wxDateTime_GetMillisecond,(self,) + _args, _kwargs)
        return val
    def GetDayOfYear(self, *_args, **_kwargs):
        val = apply(utilsc.wxDateTime_GetDayOfYear,(self,) + _args, _kwargs)
        return val
    def GetWeekOfYear(self, *_args, **_kwargs):
        val = apply(utilsc.wxDateTime_GetWeekOfYear,(self,) + _args, _kwargs)
        return val
    def GetWeekOfMonth(self, *_args, **_kwargs):
        val = apply(utilsc.wxDateTime_GetWeekOfMonth,(self,) + _args, _kwargs)
        return val
    def IsWorkDay(self, *_args, **_kwargs):
        val = apply(utilsc.wxDateTime_IsWorkDay,(self,) + _args, _kwargs)
        return val
    def IsEqualTo(self, *_args, **_kwargs):
        val = apply(utilsc.wxDateTime_IsEqualTo,(self,) + _args, _kwargs)
        return val
    def IsEarlierThan(self, *_args, **_kwargs):
        val = apply(utilsc.wxDateTime_IsEarlierThan,(self,) + _args, _kwargs)
        return val
    def IsLaterThan(self, *_args, **_kwargs):
        val = apply(utilsc.wxDateTime_IsLaterThan,(self,) + _args, _kwargs)
        return val
    def IsStrictlyBetween(self, *_args, **_kwargs):
        val = apply(utilsc.wxDateTime_IsStrictlyBetween,(self,) + _args, _kwargs)
        return val
    def IsBetween(self, *_args, **_kwargs):
        val = apply(utilsc.wxDateTime_IsBetween,(self,) + _args, _kwargs)
        return val
    def IsSameDate(self, *_args, **_kwargs):
        val = apply(utilsc.wxDateTime_IsSameDate,(self,) + _args, _kwargs)
        return val
    def IsSameTime(self, *_args, **_kwargs):
        val = apply(utilsc.wxDateTime_IsSameTime,(self,) + _args, _kwargs)
        return val
    def IsEqualUpTo(self, *_args, **_kwargs):
        val = apply(utilsc.wxDateTime_IsEqualUpTo,(self,) + _args, _kwargs)
        return val
    def AddTS(self, *_args, **_kwargs):
        val = apply(utilsc.wxDateTime_AddTS,(self,) + _args, _kwargs)
        if val: val = wxDateTimePtr(val) 
        return val
    def AddDS(self, *_args, **_kwargs):
        val = apply(utilsc.wxDateTime_AddDS,(self,) + _args, _kwargs)
        if val: val = wxDateTimePtr(val) 
        return val
    def SubtractTS(self, *_args, **_kwargs):
        val = apply(utilsc.wxDateTime_SubtractTS,(self,) + _args, _kwargs)
        if val: val = wxDateTimePtr(val) 
        return val
    def SubtractDS(self, *_args, **_kwargs):
        val = apply(utilsc.wxDateTime_SubtractDS,(self,) + _args, _kwargs)
        if val: val = wxDateTimePtr(val) 
        return val
    def Subtract(self, *_args, **_kwargs):
        val = apply(utilsc.wxDateTime_Subtract,(self,) + _args, _kwargs)
        if val: val = wxTimeSpanPtr(val) ; val.thisown = 1
        return val
    def __add__TS(self, *_args, **_kwargs):
        val = apply(utilsc.wxDateTime___add__TS,(self,) + _args, _kwargs)
        if val: val = wxDateTimePtr(val) ; val.thisown = 1
        return val
    def __add__DS(self, *_args, **_kwargs):
        val = apply(utilsc.wxDateTime___add__DS,(self,) + _args, _kwargs)
        if val: val = wxDateTimePtr(val) ; val.thisown = 1
        return val
    def __sub__DT(self, *_args, **_kwargs):
        val = apply(utilsc.wxDateTime___sub__DT,(self,) + _args, _kwargs)
        if val: val = wxTimeSpanPtr(val) ; val.thisown = 1
        return val
    def __sub__TS(self, *_args, **_kwargs):
        val = apply(utilsc.wxDateTime___sub__TS,(self,) + _args, _kwargs)
        if val: val = wxDateTimePtr(val) ; val.thisown = 1
        return val
    def __sub__DS(self, *_args, **_kwargs):
        val = apply(utilsc.wxDateTime___sub__DS,(self,) + _args, _kwargs)
        if val: val = wxDateTimePtr(val) ; val.thisown = 1
        return val
    def __cmp__(self, *_args, **_kwargs):
        val = apply(utilsc.wxDateTime___cmp__,(self,) + _args, _kwargs)
        return val
    def ParseRfc822Date(self, *_args, **_kwargs):
        val = apply(utilsc.wxDateTime_ParseRfc822Date,(self,) + _args, _kwargs)
        return val
    def ParseFormat(self, *_args, **_kwargs):
        val = apply(utilsc.wxDateTime_ParseFormat,(self,) + _args, _kwargs)
        return val
    def ParseDateTime(self, *_args, **_kwargs):
        val = apply(utilsc.wxDateTime_ParseDateTime,(self,) + _args, _kwargs)
        return val
    def ParseDate(self, *_args, **_kwargs):
        val = apply(utilsc.wxDateTime_ParseDate,(self,) + _args, _kwargs)
        return val
    def ParseTime(self, *_args, **_kwargs):
        val = apply(utilsc.wxDateTime_ParseTime,(self,) + _args, _kwargs)
        return val
    def Format(self, *_args, **_kwargs):
        val = apply(utilsc.wxDateTime_Format,(self,) + _args, _kwargs)
        return val
    def FormatDate(self, *_args, **_kwargs):
        val = apply(utilsc.wxDateTime_FormatDate,(self,) + _args, _kwargs)
        return val
    def FormatTime(self, *_args, **_kwargs):
        val = apply(utilsc.wxDateTime_FormatTime,(self,) + _args, _kwargs)
        return val
    def FormatISODate(self, *_args, **_kwargs):
        val = apply(utilsc.wxDateTime_FormatISODate,(self,) + _args, _kwargs)
        return val
    def FormatISOTime(self, *_args, **_kwargs):
        val = apply(utilsc.wxDateTime_FormatISOTime,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxDateTime instance at %s>" % (self.this,)
    
    def __add__(self, other):
        if string.find(other.this, 'wxTimeSpan') != -1:
            return self.__add__TS(other)
        if string.find(other.this, 'wxDateSpan') != -1:
            return self.__add__DS(other)
        raise TypeError, 'Invalid r.h.s. type for __add__'
    def __sub__(self, other):
        if string.find(other.this, 'wxDateTime') != -1:
            return self.__sub__DT(other)
        if string.find(other.this, 'wxTimeSpan') != -1:
            return self.__sub__TS(other)
        if string.find(other.this, 'wxDateSpan') != -1:
            return self.__sub__DS(other)
        raise TypeError, 'Invalid r.h.s. type for __sub__'

    
    def __repr__(self):
        return '<wxDateTime: \"%s\" at %s>' % ( self.Format(), self.this)
    def __str__(self):
        return self.Format()

class wxDateTime(wxDateTimePtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(utilsc.new_wxDateTime,_args,_kwargs)
        self.thisown = 1



def wxDateTimeFromTimeT(*_args,**_kwargs):
    val = wxDateTimePtr(apply(utilsc.new_wxDateTimeFromTimeT,_args,_kwargs))
    val.thisown = 1
    return val

def wxDateTimeFromJDN(*_args,**_kwargs):
    val = wxDateTimePtr(apply(utilsc.new_wxDateTimeFromJDN,_args,_kwargs))
    val.thisown = 1
    return val

def wxDateTimeFromHMS(*_args,**_kwargs):
    val = wxDateTimePtr(apply(utilsc.new_wxDateTimeFromHMS,_args,_kwargs))
    val.thisown = 1
    return val

def wxDateTimeFromDMY(*_args,**_kwargs):
    val = wxDateTimePtr(apply(utilsc.new_wxDateTimeFromDMY,_args,_kwargs))
    val.thisown = 1
    return val


class wxTimeSpanPtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __del__(self,utilsc=utilsc):
        if self.thisown == 1 :
            utilsc.delete_wxTimeSpan(self)
    def Add(self, *_args, **_kwargs):
        val = apply(utilsc.wxTimeSpan_Add,(self,) + _args, _kwargs)
        if val: val = wxTimeSpanPtr(val) 
        return val
    def Subtract(self, *_args, **_kwargs):
        val = apply(utilsc.wxTimeSpan_Subtract,(self,) + _args, _kwargs)
        if val: val = wxTimeSpanPtr(val) 
        return val
    def Multiply(self, *_args, **_kwargs):
        val = apply(utilsc.wxTimeSpan_Multiply,(self,) + _args, _kwargs)
        if val: val = wxTimeSpanPtr(val) 
        return val
    def Neg(self, *_args, **_kwargs):
        val = apply(utilsc.wxTimeSpan_Neg,(self,) + _args, _kwargs)
        if val: val = wxTimeSpanPtr(val) 
        return val
    def Abs(self, *_args, **_kwargs):
        val = apply(utilsc.wxTimeSpan_Abs,(self,) + _args, _kwargs)
        if val: val = wxTimeSpanPtr(val) ; val.thisown = 1
        return val
    def __add__(self, *_args, **_kwargs):
        val = apply(utilsc.wxTimeSpan___add__,(self,) + _args, _kwargs)
        if val: val = wxTimeSpanPtr(val) ; val.thisown = 1
        return val
    def __sub__(self, *_args, **_kwargs):
        val = apply(utilsc.wxTimeSpan___sub__,(self,) + _args, _kwargs)
        if val: val = wxTimeSpanPtr(val) ; val.thisown = 1
        return val
    def __mul__(self, *_args, **_kwargs):
        val = apply(utilsc.wxTimeSpan___mul__,(self,) + _args, _kwargs)
        if val: val = wxTimeSpanPtr(val) ; val.thisown = 1
        return val
    def __rmul__(self, *_args, **_kwargs):
        val = apply(utilsc.wxTimeSpan___rmul__,(self,) + _args, _kwargs)
        if val: val = wxTimeSpanPtr(val) ; val.thisown = 1
        return val
    def __neg__(self, *_args, **_kwargs):
        val = apply(utilsc.wxTimeSpan___neg__,(self,) + _args, _kwargs)
        if val: val = wxTimeSpanPtr(val) ; val.thisown = 1
        return val
    def __cmp__(self, *_args, **_kwargs):
        val = apply(utilsc.wxTimeSpan___cmp__,(self,) + _args, _kwargs)
        return val
    def IsNull(self, *_args, **_kwargs):
        val = apply(utilsc.wxTimeSpan_IsNull,(self,) + _args, _kwargs)
        return val
    def IsPositive(self, *_args, **_kwargs):
        val = apply(utilsc.wxTimeSpan_IsPositive,(self,) + _args, _kwargs)
        return val
    def IsNegative(self, *_args, **_kwargs):
        val = apply(utilsc.wxTimeSpan_IsNegative,(self,) + _args, _kwargs)
        return val
    def IsEqualTo(self, *_args, **_kwargs):
        val = apply(utilsc.wxTimeSpan_IsEqualTo,(self,) + _args, _kwargs)
        return val
    def IsLongerThan(self, *_args, **_kwargs):
        val = apply(utilsc.wxTimeSpan_IsLongerThan,(self,) + _args, _kwargs)
        return val
    def IsShorterThan(self, *_args, **_kwargs):
        val = apply(utilsc.wxTimeSpan_IsShorterThan,(self,) + _args, _kwargs)
        return val
    def GetWeeks(self, *_args, **_kwargs):
        val = apply(utilsc.wxTimeSpan_GetWeeks,(self,) + _args, _kwargs)
        return val
    def GetDays(self, *_args, **_kwargs):
        val = apply(utilsc.wxTimeSpan_GetDays,(self,) + _args, _kwargs)
        return val
    def GetHours(self, *_args, **_kwargs):
        val = apply(utilsc.wxTimeSpan_GetHours,(self,) + _args, _kwargs)
        return val
    def GetMinutes(self, *_args, **_kwargs):
        val = apply(utilsc.wxTimeSpan_GetMinutes,(self,) + _args, _kwargs)
        return val
    def GetSeconds(self, *_args, **_kwargs):
        val = apply(utilsc.wxTimeSpan_GetSeconds,(self,) + _args, _kwargs)
        return val
    def GetMilliseconds(self, *_args, **_kwargs):
        val = apply(utilsc.wxTimeSpan_GetMilliseconds,(self,) + _args, _kwargs)
        return val
    def Format(self, *_args, **_kwargs):
        val = apply(utilsc.wxTimeSpan_Format,(self,) + _args, _kwargs)
        return val
    def FormatDate(self, *_args, **_kwargs):
        val = apply(utilsc.wxTimeSpan_FormatDate,(self,) + _args, _kwargs)
        return val
    def FormatTime(self, *_args, **_kwargs):
        val = apply(utilsc.wxTimeSpan_FormatTime,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxTimeSpan instance at %s>" % (self.this,)
class wxTimeSpan(wxTimeSpanPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(utilsc.new_wxTimeSpan,_args,_kwargs)
        self.thisown = 1




class wxDateSpanPtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __del__(self,utilsc=utilsc):
        if self.thisown == 1 :
            utilsc.delete_wxDateSpan(self)
    def SetYears(self, *_args, **_kwargs):
        val = apply(utilsc.wxDateSpan_SetYears,(self,) + _args, _kwargs)
        if val: val = wxDateSpanPtr(val) 
        return val
    def SetMonths(self, *_args, **_kwargs):
        val = apply(utilsc.wxDateSpan_SetMonths,(self,) + _args, _kwargs)
        if val: val = wxDateSpanPtr(val) 
        return val
    def SetWeeks(self, *_args, **_kwargs):
        val = apply(utilsc.wxDateSpan_SetWeeks,(self,) + _args, _kwargs)
        if val: val = wxDateSpanPtr(val) 
        return val
    def SetDays(self, *_args, **_kwargs):
        val = apply(utilsc.wxDateSpan_SetDays,(self,) + _args, _kwargs)
        if val: val = wxDateSpanPtr(val) 
        return val
    def GetYears(self, *_args, **_kwargs):
        val = apply(utilsc.wxDateSpan_GetYears,(self,) + _args, _kwargs)
        return val
    def GetMonths(self, *_args, **_kwargs):
        val = apply(utilsc.wxDateSpan_GetMonths,(self,) + _args, _kwargs)
        return val
    def GetWeeks(self, *_args, **_kwargs):
        val = apply(utilsc.wxDateSpan_GetWeeks,(self,) + _args, _kwargs)
        return val
    def GetDays(self, *_args, **_kwargs):
        val = apply(utilsc.wxDateSpan_GetDays,(self,) + _args, _kwargs)
        return val
    def GetTotalDays(self, *_args, **_kwargs):
        val = apply(utilsc.wxDateSpan_GetTotalDays,(self,) + _args, _kwargs)
        return val
    def Neg(self, *_args, **_kwargs):
        val = apply(utilsc.wxDateSpan_Neg,(self,) + _args, _kwargs)
        if val: val = wxDateSpanPtr(val) 
        return val
    def Multiply(self, *_args, **_kwargs):
        val = apply(utilsc.wxDateSpan_Multiply,(self,) + _args, _kwargs)
        if val: val = wxDateSpanPtr(val) 
        return val
    def __add__(self, *_args, **_kwargs):
        val = apply(utilsc.wxDateSpan___add__,(self,) + _args, _kwargs)
        if val: val = wxDateSpanPtr(val) ; val.thisown = 1
        return val
    def __sub__(self, *_args, **_kwargs):
        val = apply(utilsc.wxDateSpan___sub__,(self,) + _args, _kwargs)
        if val: val = wxDateSpanPtr(val) ; val.thisown = 1
        return val
    def __mul__(self, *_args, **_kwargs):
        val = apply(utilsc.wxDateSpan___mul__,(self,) + _args, _kwargs)
        if val: val = wxDateSpanPtr(val) ; val.thisown = 1
        return val
    def __rmul__(self, *_args, **_kwargs):
        val = apply(utilsc.wxDateSpan___rmul__,(self,) + _args, _kwargs)
        if val: val = wxDateSpanPtr(val) ; val.thisown = 1
        return val
    def __neg__(self, *_args, **_kwargs):
        val = apply(utilsc.wxDateSpan___neg__,(self,) + _args, _kwargs)
        if val: val = wxDateSpanPtr(val) ; val.thisown = 1
        return val
    def __repr__(self):
        return "<C wxDateSpan instance at %s>" % (self.this,)
class wxDateSpan(wxDateSpanPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(utilsc.new_wxDateSpan,_args,_kwargs)
        self.thisown = 1






#-------------- FUNCTION WRAPPERS ------------------

def wxConfigBase_Set(*_args, **_kwargs):
    val = apply(utilsc.wxConfigBase_Set,_args,_kwargs)
    if val: val = wxConfigBasePtr(val)
    return val

def wxConfigBase_Get(*_args, **_kwargs):
    val = apply(utilsc.wxConfigBase_Get,_args,_kwargs)
    if val: val = wxConfigBasePtr(val)
    return val

def wxConfigBase_Create(*_args, **_kwargs):
    val = apply(utilsc.wxConfigBase_Create,_args,_kwargs)
    if val: val = wxConfigBasePtr(val)
    return val

wxConfigBase_DontCreateOnDemand = utilsc.wxConfigBase_DontCreateOnDemand

wxDateTime_SetCountry = utilsc.wxDateTime_SetCountry

wxDateTime_GetCountry = utilsc.wxDateTime_GetCountry

wxDateTime_IsWestEuropeanCountry = utilsc.wxDateTime_IsWestEuropeanCountry

wxDateTime_GetCurrentYear = utilsc.wxDateTime_GetCurrentYear

wxDateTime_ConvertYearToBC = utilsc.wxDateTime_ConvertYearToBC

wxDateTime_GetCurrentMonth = utilsc.wxDateTime_GetCurrentMonth

wxDateTime_IsLeapYear = utilsc.wxDateTime_IsLeapYear

wxDateTime_GetCentury = utilsc.wxDateTime_GetCentury

wxDateTime_GetNumberOfDaysinYear = utilsc.wxDateTime_GetNumberOfDaysinYear

wxDateTime_GetNumberOfDaysInMonth = utilsc.wxDateTime_GetNumberOfDaysInMonth

wxDateTime_GetMonthName = utilsc.wxDateTime_GetMonthName

wxDateTime_GetWeekDayName = utilsc.wxDateTime_GetWeekDayName

wxDateTime_GetAmPmStrings = utilsc.wxDateTime_GetAmPmStrings

wxDateTime_IsDSTApplicable = utilsc.wxDateTime_IsDSTApplicable

def wxDateTime_GetBeginDST(*_args, **_kwargs):
    val = apply(utilsc.wxDateTime_GetBeginDST,_args,_kwargs)
    if val: val = wxDateTimePtr(val); val.thisown = 1
    return val

def wxDateTime_GetEndDST(*_args, **_kwargs):
    val = apply(utilsc.wxDateTime_GetEndDST,_args,_kwargs)
    if val: val = wxDateTimePtr(val); val.thisown = 1
    return val

def wxDateTime_Now(*_args, **_kwargs):
    val = apply(utilsc.wxDateTime_Now,_args,_kwargs)
    if val: val = wxDateTimePtr(val); val.thisown = 1
    return val

def wxDateTime_Today(*_args, **_kwargs):
    val = apply(utilsc.wxDateTime_Today,_args,_kwargs)
    if val: val = wxDateTimePtr(val); val.thisown = 1
    return val

def wxTimeSpan_Seconds(*_args, **_kwargs):
    val = apply(utilsc.wxTimeSpan_Seconds,_args,_kwargs)
    if val: val = wxTimeSpanPtr(val); val.thisown = 1
    return val

def wxTimeSpan_Second(*_args, **_kwargs):
    val = apply(utilsc.wxTimeSpan_Second,_args,_kwargs)
    if val: val = wxTimeSpanPtr(val); val.thisown = 1
    return val

def wxTimeSpan_Minutes(*_args, **_kwargs):
    val = apply(utilsc.wxTimeSpan_Minutes,_args,_kwargs)
    if val: val = wxTimeSpanPtr(val); val.thisown = 1
    return val

def wxTimeSpan_Minute(*_args, **_kwargs):
    val = apply(utilsc.wxTimeSpan_Minute,_args,_kwargs)
    if val: val = wxTimeSpanPtr(val); val.thisown = 1
    return val

def wxTimeSpan_Hours(*_args, **_kwargs):
    val = apply(utilsc.wxTimeSpan_Hours,_args,_kwargs)
    if val: val = wxTimeSpanPtr(val); val.thisown = 1
    return val

def wxTimeSpan_Hour(*_args, **_kwargs):
    val = apply(utilsc.wxTimeSpan_Hour,_args,_kwargs)
    if val: val = wxTimeSpanPtr(val); val.thisown = 1
    return val

def wxTimeSpan_Days(*_args, **_kwargs):
    val = apply(utilsc.wxTimeSpan_Days,_args,_kwargs)
    if val: val = wxTimeSpanPtr(val); val.thisown = 1
    return val

def wxTimeSpan_Day(*_args, **_kwargs):
    val = apply(utilsc.wxTimeSpan_Day,_args,_kwargs)
    if val: val = wxTimeSpanPtr(val); val.thisown = 1
    return val

def wxTimeSpan_Weeks(*_args, **_kwargs):
    val = apply(utilsc.wxTimeSpan_Weeks,_args,_kwargs)
    if val: val = wxTimeSpanPtr(val); val.thisown = 1
    return val

def wxTimeSpan_Week(*_args, **_kwargs):
    val = apply(utilsc.wxTimeSpan_Week,_args,_kwargs)
    if val: val = wxTimeSpanPtr(val); val.thisown = 1
    return val

def wxDateSpan_Days(*_args, **_kwargs):
    val = apply(utilsc.wxDateSpan_Days,_args,_kwargs)
    if val: val = wxDateSpanPtr(val); val.thisown = 1
    return val

def wxDateSpan_Day(*_args, **_kwargs):
    val = apply(utilsc.wxDateSpan_Day,_args,_kwargs)
    if val: val = wxDateSpanPtr(val); val.thisown = 1
    return val

def wxDateSpan_Weeks(*_args, **_kwargs):
    val = apply(utilsc.wxDateSpan_Weeks,_args,_kwargs)
    if val: val = wxDateSpanPtr(val); val.thisown = 1
    return val

def wxDateSpan_Week(*_args, **_kwargs):
    val = apply(utilsc.wxDateSpan_Week,_args,_kwargs)
    if val: val = wxDateSpanPtr(val); val.thisown = 1
    return val

def wxDateSpan_Months(*_args, **_kwargs):
    val = apply(utilsc.wxDateSpan_Months,_args,_kwargs)
    if val: val = wxDateSpanPtr(val); val.thisown = 1
    return val

def wxDateSpan_Month(*_args, **_kwargs):
    val = apply(utilsc.wxDateSpan_Month,_args,_kwargs)
    if val: val = wxDateSpanPtr(val); val.thisown = 1
    return val

def wxDateSpan_Years(*_args, **_kwargs):
    val = apply(utilsc.wxDateSpan_Years,_args,_kwargs)
    if val: val = wxDateSpanPtr(val); val.thisown = 1
    return val

def wxDateSpan_Year(*_args, **_kwargs):
    val = apply(utilsc.wxDateSpan_Year,_args,_kwargs)
    if val: val = wxDateSpanPtr(val); val.thisown = 1
    return val



#-------------- VARIABLE WRAPPERS ------------------

wxCONFIG_USE_LOCAL_FILE = utilsc.wxCONFIG_USE_LOCAL_FILE
wxCONFIG_USE_GLOBAL_FILE = utilsc.wxCONFIG_USE_GLOBAL_FILE
wxCONFIG_USE_RELATIVE_PATH = utilsc.wxCONFIG_USE_RELATIVE_PATH
