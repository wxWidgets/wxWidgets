"""Decorator classes for documentation and shell scripting.
"""

__author__ = "Patrick K. O'Brien <pobrien@orbtech.com>"
__cvsid__ = "$Id$"
__revision__ = "$Revision$"[11:-2]


# These are not the real wxPython classes. These are Python versions
# for documentation purposes. They are also used to apply docstrings
# to the real wxPython classes, which are SWIG-generated wrappers for
# C-language classes.


import wxParameters as wx


def NewId():
    """"""
    pass

def RegisterId():
    """"""
    pass

def wxArtProvider_GetBitmap():
    """"""
    pass

def wxArtProvider_GetIcon():
    """"""
    pass

def wxArtProvider_PopProvider():
    """"""
    pass

def wxArtProvider_PushProvider():
    """"""
    pass

def wxArtProvider_RemoveProvider():
    """"""
    pass

def wxBeginBusyCursor():
    """"""
    pass

def wxBell():
    """"""
    pass

def wxBitmapFromBits():
    """"""
    pass

def wxBitmapFromIcon():
    """"""
    pass

def wxBitmapFromImage():
    """"""
    pass

def wxBitmapFromXPMData():
    """"""
    pass

def wxBufferedDCInternalBuffer():
    """"""
    pass

def wxButton_GetDefaultSize():
    """"""
    pass

## def wxCallAfter():
##     """"""
##     pass

from wxPython import wx as wxpy
wxCallAfter = wxpy.wxCallAfter
del wxpy

def wxCaret_GetBlinkTime():
    """"""
    pass

def wxCaret_SetBlinkTime():
    """"""
    pass

def wxClientDisplayRect():
    """"""
    pass

def wxColourDisplay():
    """"""
    pass

def wxConfigBase_Create():
    """"""
    pass

def wxConfigBase_DontCreateOnDemand():
    """"""
    pass

def wxConfigBase_Get():
    """"""
    pass

def wxConfigBase_Set():
    """"""
    pass

def wxCreateFileTipProvider():
    """"""
    pass

def wxCustomDataFormat():
    """"""
    pass

def wxDLG_PNT():
    """"""
    pass

def wxDLG_SZE():
    """"""
    pass

def wxDateSpan_Day():
    """"""
    pass

def wxDateSpan_Days():
    """"""
    pass

def wxDateSpan_Month():
    """"""
    pass

def wxDateSpan_Months():
    """"""
    pass

def wxDateSpan_Week():
    """"""
    pass

def wxDateSpan_Weeks():
    """"""
    pass

def wxDateSpan_Year():
    """"""
    pass

def wxDateSpan_Years():
    """"""
    pass

def wxDateTimeFromDMY():
    """"""
    pass

def wxDateTimeFromHMS():
    """"""
    pass

def wxDateTimeFromJDN():
    """"""
    pass

def wxDateTimeFromTimeT():
    """"""
    pass

def wxDateTime_ConvertYearToBC():
    """"""
    pass

def wxDateTime_GetAmPmStrings():
    """"""
    pass

def wxDateTime_GetBeginDST():
    """"""
    pass

def wxDateTime_GetCentury():
    """"""
    pass

def wxDateTime_GetCountry():
    """"""
    pass

def wxDateTime_GetCurrentMonth():
    """"""
    pass

def wxDateTime_GetCurrentYear():
    """"""
    pass

def wxDateTime_GetEndDST():
    """"""
    pass

def wxDateTime_GetMonthName():
    """"""
    pass

def wxDateTime_GetNumberOfDaysInMonth():
    """"""
    pass

def wxDateTime_GetNumberOfDaysinYear():
    """"""
    pass

def wxDateTime_GetWeekDayName():
    """"""
    pass

def wxDateTime_IsDSTApplicable():
    """"""
    pass

def wxDateTime_IsLeapYear():
    """"""
    pass

def wxDateTime_IsWestEuropeanCountry():
    """"""
    pass

def wxDateTime_Now():
    """"""
    pass

def wxDateTime_SetCountry():
    """"""
    pass

def wxDateTime_Today():
    """"""
    pass

def wxDateTime_UNow():
    """"""
    pass

def wxDirSelector():
    """"""
    pass

def wxDisplayDepth():
    """"""
    pass

def wxDisplaySize():
    """"""
    pass

def wxDisplaySizeMM():
    """"""
    pass

def wxDragIcon():
    """"""
    pass

def wxDragListItem():
    """"""
    pass

def wxDragString():
    """"""
    pass

def wxDragTreeItem():
    """"""
    pass

def wxEmptyBitmap():
    """"""
    pass

def wxEmptyIcon():
    """"""
    pass

def wxEmptyImage():
    """"""
    pass

def wxEnableTopLevelWindows():
    """"""
    pass

def wxEncodingConverter_GetAllEquivalents():
    """"""
    pass

def wxEncodingConverter_GetPlatformEquivalents():
    """"""
    pass

def wxEndBusyCursor():
    """"""
    pass

def wxExecute():
    """"""
    pass

def wxFileSelector():
    """"""
    pass

def wxFileSystem_AddHandler():
    """"""
    pass

def wxFileSystem_CleanUpHandlers():
    """"""
    pass

def wxFileTypeInfoSequence():
    """"""
    pass

def wxFileType_ExpandCommand():
    """"""
    pass

def wxFindWindowAtPoint():
    """"""
    pass

def wxFindWindowById(id, parent=wx.NULL):
    """"""
    pass

def wxFindWindowByLabel(label, parent=wx.NULL):
    """"""
    pass

def wxFindWindowByName(name, parent=wx.NULL):
    """"""
    pass

def wxFontFromNativeInfo():
    """"""
    pass

def wxFontMapper_Get():
    """"""
    pass

def wxFontMapper_GetDefaultConfigPath():
    """"""
    pass

def wxFontMapper_GetEncodingDescription():
    """"""
    pass

def wxFontMapper_GetEncodingName():
    """"""
    pass

def wxFontMapper_Set():
    """"""
    pass

def wxFont_GetDefaultEncoding():
    """"""
    pass

def wxFont_SetDefaultEncoding():
    """"""
    pass

def wxGenericFindWindowAtPoint():
    """"""
    pass

def wxGetAccelFromString():
    """"""
    pass

def wxGetActiveWindow():
    """"""
    pass

def wxGetApp():
    """"""
    pass

def wxGetClientDisplayRect():
    """"""
    pass

def wxGetCurrentId():
    """"""
    pass

def wxGetCurrentTime():
    """"""
    pass

def wxGetDisplayDepth():
    """"""
    pass

def wxGetDisplaySize():
    """"""
    pass

def wxGetDisplaySizeMM():
    """"""
    pass

def wxGetElapsedTime():
    """"""
    pass

def wxGetEmailAddress():
    """"""
    pass

def wxGetFullHostName():
    """"""
    pass

def wxGetHomeDir():
    """"""
    pass

def wxGetHostName():
    """"""
    pass

def wxGetLocalTime():
    """"""
    pass

def wxGetLocalTimeMillis():
    """"""
    pass

def wxGetLocale():
    """"""
    pass

def wxGetMousePosition():
    """"""
    pass

def wxGetNumberFromUser():
    """"""
    pass

def wxGetOsDescription():
    """"""
    pass

def wxGetOsVersion():
    """"""
    pass

def wxGetPasswordFromUser():
    """"""
    pass

def wxGetProcessId():
    """"""
    pass

def wxGetSingleChoice():
    """"""
    pass

def wxGetSingleChoiceIndex():
    """"""
    pass

def wxGetTextFromUser():
    """"""
    pass

def wxGetTopLevelParent():
    """"""
    pass

def wxGetTranslation():
    """"""
    pass

def wxGetUTCTime():
    """"""
    pass

def wxGetUserHome():
    """"""
    pass

def wxGetUserId():
    """"""
    pass

def wxGetUserName():
    """"""
    pass

def wxIconBundleFromFile():
    """"""
    pass

def wxIconBundleFromIcon():
    """"""
    pass

def wxIconFromBitmap():
    """"""
    pass

def wxIconFromXPMData():
    """"""
    pass

def wxImageFromBitmap():
    """"""
    pass

def wxImageFromData():
    """"""
    pass

def wxImageFromMime():
    """"""
    pass

def wxImageFromStream():
    """"""
    pass

def wxImageFromStreamMime():
    """"""
    pass

def wxImage_AddHandler():
    """"""
    pass

def wxImage_CanRead():
    """"""
    pass

def wxImage_CanReadStream():
    """"""
    pass

def wxImage_GetImageCount():
    """"""
    pass

def wxImage_InsertHandler():
    """"""
    pass

def wxImage_RemoveHandler():
    """"""
    pass

def wxInitAllImageHandlers():
    """"""
    pass

def wxIntersectRect():
    """"""
    pass

def wxIsBusy():
    """"""
    pass

def wxIsDragResultOk():
    """"""
    pass

def wxLoadFileSelector():
    """"""
    pass

def wxLocale_AddCatalogLookupPathPrefix():
    """"""
    pass

def wxLocale_AddLanguage():
    """"""
    pass

def wxLocale_GetLanguageInfo():
    """"""
    pass

def wxLocale_GetSystemEncoding():
    """"""
    pass

def wxLocale_GetSystemEncodingName():
    """"""
    pass

def wxLocale_GetSystemLanguage():
    """"""
    pass

def wxLogDebug():
    """"""
    pass

def wxLogError():
    """"""
    pass

def wxLogFatalError():
    """"""
    pass

def wxLogGeneric():
    """"""
    pass

def wxLogInfo():
    """"""
    pass

def wxLogMessage():
    """"""
    pass

def wxLogStatus():
    """"""
    pass

def wxLogStatusFrame():
    """"""
    pass

def wxLogSysError():
    """"""
    pass

def wxLogTrace():
    """"""
    pass

def wxLogTraceMask():
    """"""
    pass

def wxLogVerbose():
    """"""
    pass

def wxLogWarning():
    """"""
    pass

def wxLog_AddTraceMask():
    """"""
    pass

def wxLog_ClearTraceMasks():
    """"""
    pass

def wxLog_DontCreateOnDemand():
    """"""
    pass

def wxLog_EnableLogging():
    """"""
    pass

def wxLog_FlushActive():
    """"""
    pass

def wxLog_GetActiveTarget():
    """"""
    pass

def wxLog_GetLogLevel():
    """"""
    pass

def wxLog_GetTimestamp():
    """"""
    pass

def wxLog_GetTraceMask():
    """"""
    pass

def wxLog_GetTraceMasks():
    """"""
    pass

def wxLog_IsAllowedTraceMask():
    """"""
    pass

def wxLog_IsEnabled():
    """"""
    pass

def wxLog_OnLog():
    """"""
    pass

def wxLog_RemoveTraceMask():
    """"""
    pass

def wxLog_Resume():
    """"""
    pass

def wxLog_SetActiveTarget():
    """"""
    pass

def wxLog_SetLogLevel():
    """"""
    pass

def wxLog_SetTimestamp():
    """"""
    pass

def wxLog_SetTraceMask():
    """"""
    pass

def wxLog_SetVerbose():
    """"""
    pass

def wxLog_Suspend():
    """"""
    pass

def wxMaskColour():
    """"""
    pass

def wxMemoryDCFromDC():
    """"""
    pass

def wxMemoryFSHandler_AddFile():
    """"""
    pass

def wxMemoryFSHandler_RemoveFile():
    """"""
    pass

def wxMenuItem_GetLabelFromText():
    """"""
    pass

def wxMessageBox():
    """"""
    pass

def wxMimeTypesManager_IsOfType():
    """"""
    pass

def wxMutexGuiEnter():
    """"""
    pass

def wxMutexGuiLeave():
    """"""
    pass

def wxNamedColour():
    """"""
    pass

def wxNamedColour():
    """"""
    pass

def wxNewEventType():
    """"""
    pass

def wxNewId():
    """"""
    pass

def wxNow():
    """"""
    pass

def wxNullFileTypeInfo():
    """"""
    pass

def wxPoint2DDoubleCopy():
    """"""
    pass

def wxPoint2DDoubleFromPoint():
    """"""
    pass

def wxPostEvent():
    """"""
    pass

def wxPostScriptDC_GetResolution():
    """"""
    pass

def wxPostScriptDC_SetResolution():
    """"""
    pass

def wxPreBitmapButton():
    """"""
    pass

def wxPreButton():
    """"""
    pass

def wxPreCheckBox():
    """"""
    pass

def wxPreCheckListBox():
    """"""
    pass

def wxPreChoice():
    """"""
    pass

def wxPreComboBox():
    """"""
    pass

def wxPreControl():
    """"""
    pass

def wxPreDialog():
    """"""
    pass

def wxPreDirFilterListCtrl():
    """"""
    pass

def wxPreFindReplaceDialog():
    """"""
    pass

def wxPreFrame():
    """"""
    pass

def wxPreGauge():
    """"""
    pass

def wxPreGenericDirCtrl():
    """"""
    pass

def wxPreListBox():
    """"""
    pass

def wxPreListCtrl():
    """"""
    pass

def wxPreListView():
    """"""
    pass

def wxPreMDIChildFrame():
    """"""
    pass

def wxPreMDIClientWindow():
    """"""
    pass

def wxPreMDIParentFrame():
    """"""
    pass

def wxPreMiniFrame():
    """"""
    pass

def wxPreNotebook():
    """"""
    pass

def wxPrePanel():
    """"""
    pass

def wxPrePopupTransientWindow():
    """"""
    pass

def wxPrePopupWindow():
    """"""
    pass

def wxPreRadioBox():
    """"""
    pass

def wxPreRadioButton():
    """"""
    pass

def wxPreSashLayoutWindow():
    """"""
    pass

def wxPreSashWindow():
    """"""
    pass

def wxPreScrollBar():
    """"""
    pass

def wxPreScrolledWindow():
    """"""
    pass

def wxPreSingleInstanceChecker():
    """"""
    pass

def wxPreSlider():
    """"""
    pass

def wxPreSpinButton():
    """"""
    pass

def wxPreSpinCtrl():
    """"""
    pass

def wxPreSplitterWindow():
    """"""
    pass

def wxPreStaticBitmap():
    """"""
    pass

def wxPreStaticBox():
    """"""
    pass

def wxPreStaticLine():
    """"""
    pass

def wxPreStaticText():
    """"""
    pass

def wxPreStatusBar():
    """"""
    pass

def wxPreTextCtrl():
    """"""
    pass

def wxPreToggleButton():
    """"""
    pass

def wxPreToolBar():
    """"""
    pass

def wxPreToolBarSimple():
    """"""
    pass

def wxPreTopLevelWindow():
    """"""
    pass

def wxPreTreeCtrl():
    """"""
    pass

def wxPreWindow():
    """"""
    pass

def wxProcess_Exists():
    """"""
    pass

def wxProcess_Kill():
    """"""
    pass

def wxProcess_Open():
    """"""
    pass

def wxPyTypeCast(obj, typeStr):
    """This helper function will take a wxPython object and convert it
    to another wxPython object type.  This will not be able to create
    objects that are derived from wxPython classes by the user, only
    those that are actually part of wxPython and directly corespond to
    C++ objects.

    This is useful in situations where some method returns a generic
    type such as wxWindow, but you know that it is actually some
    derived type such as a wxTextCtrl.  You can't call wxTextCtrl
    specific methods on a wxWindow object, but you can use this
    function to create a wxTextCtrl object that will pass the same
    pointer to the C++ code.  You use it like this:

       textCtrl = wxPyTypeCast(window, "wxTextCtrl")

    WARNING:  Using this function to type cast objects into types that
              they are not is not recommended and is likely to cause
              your program to crash...  Hard."""
    pass

def wxPy_isinstance(obj, klasses):
    """An isinstance for Python < 2.2 that can check a sequence of
    class objects like the one in 2.2 can."""
    pass

def wxRegionFromPoints():
    """"""
    pass

def wxRegisterId():
    """"""
    pass

## def wxRunLater(callable, *args, **kw):
##     """An alias for wxCallAfter."""
##     pass

wxRunLater = wxCallAfter

def wxSafeShowMessage():
    """"""
    pass

def wxSafeYield():
    """"""
    pass

def wxSaveFileSelector():
    """"""
    pass

def wxSetCursor():
    """"""
    pass

def wxShell():
    """"""
    pass

def wxShowTip():
    """"""
    pass

def wxShutdown():
    """"""
    pass

def wxSleep():
    """"""
    pass

def wxStartTimer():
    """"""
    pass

def wxStockCursor():
    """"""
    pass

def wxStripMenuCodes():
    """"""
    pass

def wxSysErrorCode():
    """"""
    pass

def wxSysErrorMsg():
    """"""
    pass

def wxSystemSettings_GetColour():
    """"""
    pass

def wxSystemSettings_GetFont():
    """"""
    pass

def wxSystemSettings_GetMetric():
    """"""
    pass

def wxSystemSettings_GetScreenType():
    """"""
    pass

def wxSystemSettings_GetColour():
    """"""
    pass

def wxSystemSettings_GetFont():
    """"""
    pass

def wxSystemSettings_GetMetric():
    """"""
    pass

def wxSystemSettings_HasFeature():
    """"""
    pass

def wxSystemSettings_SetScreenType():
    """"""
    pass

def wxTextAttr_Combine():
    """"""
    pass

def wxThread_IsMain():
    """"""
    pass

def wxTimeSpan_Day():
    """"""
    pass

def wxTimeSpan_Days():
    """"""
    pass

def wxTimeSpan_Hour():
    """"""
    pass

def wxTimeSpan_Hours():
    """"""
    pass

def wxTimeSpan_Minute():
    """"""
    pass

def wxTimeSpan_Minutes():
    """"""
    pass

def wxTimeSpan_Second():
    """"""
    pass

def wxTimeSpan_Seconds():
    """"""
    pass

def wxTimeSpan_Week():
    """"""
    pass

def wxTimeSpan_Weeks():
    """"""
    pass

def wxToolTip_Enable():
    """"""
    pass

def wxToolTip_SetDelay():
    """"""
    pass

def wxUsleep():
    """"""
    pass

def wxValidator_IsSilent():
    """"""
    pass

def wxValidator_SetBellOnError():
    """"""
    pass

def wxWakeUpIdle():
    """"""
    pass

def wxWaveData():
    """"""
    pass

def wxWindow_FindFocus():
    """"""
    pass

def wxWindow_GetCapture():
    """"""
    pass

def wxWindow_NewControlId():
    """"""
    pass

def wxWindow_NextControlId():
    """"""
    pass

def wxWindow_PrevControlId():
    """"""
    pass

def wxYield():
    """"""
    pass

def wxYieldIfNeeded():
    """"""
    pass
