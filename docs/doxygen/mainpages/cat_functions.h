/////////////////////////////////////////////////////////////////////////////
// Name:        cat_functions.h
// Purpose:     Functions-by-category page of the Doxygen manual
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////


/*!

 @page page_func_cat Functions by category

 A classification of wxWidgets functions by category.

 @li @ref page_func_cat_appinitterm
 @li @ref page_func_cat_procctrl
 @li @ref page_func_cat_thread
 @li @ref page_func_cat_file
 @li @ref page_func_cat_networkuseros
 @li @ref page_func_cat_string
 @li @ref page_func_cat_dialog
 @li @ref page_func_cat_math 
 @li @ref page_func_cat_gdi
 @li @ref page_func_cat_printer
 @li @ref page_func_cat_clipboard
 @li @ref page_func_cat_misc
 @li @ref page_func_cat_log
 @li @ref page_func_cat_time
 @li @ref page_func_cat_debug
 @li @ref page_func_cat_env
 @li @ref page_func_cat_atomic


 <hr>



 @section page_func_cat_appinitterm Application initialization and termination

The functions in this section are used on application startup/shutdown and also to control the behaviour of the main event loop of the GUI programs.
::wxEntry
::wxEntryCleanup
::wxEntryStart
::wxGetApp
::wxHandleFatalExceptions
::wxInitAllImageHandlers
::wxInitialize
::wxSafeYield
::wxUninitialize
::wxYield
::wxWakeUpIdle


 @section page_func_cat_procctrl Process control functions
The functions in this section are used to launch or terminate the other processes.
::wxExecute
::wxExit
::wxJoin
::wxKill
::wxGetProcessId
::wxShell
::wxShutdown

 @section page_func_cat_thread Thread functions
The functions and macros here mainly exist to make it writing the code which may be compiled in multi thread build (wxUSE_THREADS = 1) as well as in single thread configuration (wxUSE_THREADS = 0).
For example, a static variable must be protected against simultaneous access by multiple threads in the former configuration but in the latter the extra overhead of using the critical section is not needed. To solve this problem, the wxCRITICAL_SECTION macro may be used to create and use the critical section only when needed.
Include files
<wx/thread.h>
See also
wxThread, wxMutex, Multithreading overview

::wxIsMainThread
::wxMutexGuiEnter
::wxMutexGuiLeave

 @section page_func_cat_file File functions
Include files
<wx/filefn.h>
See also
wxPathList
wxDir
wxFile
wxFileName

::wxDos2UnixFilename
::wxFileExists
::wxFileModificationTime
::wxFileNameFromPath
::wxFindFirstFile
::wxFindNextFile
::wxGetDiskSpace
::wxGetFileKind
::wxGetOSDirectory
::wxIsAbsolutePath
::wxDirExists
::wxPathOnly
::wxUnix2DosFilename
::wxConcatFiles
::wxCopyFile
::wxGetCwd
::wxGetWorkingDirectory
::wxGetTempFileName
::wxIsWild
::wxMatchWild
::wxMkdir
::wxParseCommonDialogsFilter
::wxRemoveFile
::wxRenameFile
::wxRmdir
::wxSetWorkingDirectory
::wxSplit
::wxSplitPath
::wxTransferFileToStream
::wxTransferStreamToFile

 @section page_func_cat_networkuseros Network, user and OS functions
The functions in this section are used to retrieve information about the current computer and/or user characteristics.
::wxGetEmailAddress
::wxGetFreeMemory
::wxGetFullHostName
::wxGetHomeDir
::wxGetHostName
::wxGetOsDescription
::wxGetOsVersion
::wxIsPlatformLittleEndian
::wxIsPlatform64Bit
::wxGetUserHome
::wxGetUserId
::wxGetUserName


 @section page_func_cat_string String functions
::wxGetTranslation
::wxIsEmpty
wxS
::wxStrcmp
::wxStricmp
::wxStringEq
::wxStringMatch
::wxStringTokenize
::wxStrlen
::wxSnprintf
wxT
wxTRANSLATE
::wxVsnprintf
_
wxPLURAL
_T

 @section page_func_cat_dialog Dialog functions
Below are a number of convenience functions for getting input from the user or displaying messages. Note that in these functions the last three parameters are optional. However, it is recommended to pass a parent frame parameter, or (in MS Windows or Motif) the wrong window frame may be brought to the front when the dialog box is popped up.
::wxAboutBox
::wxBeginBusyCursor
::wxBell
::wxCreateFileTipProvider
::wxDirSelector
::wxFileSelector
::wxEndBusyCursor
::wxGenericAboutBox
::wxGetColourFromUser
::wxGetFontFromUser
::wxGetMultipleChoices
::wxGetNumberFromUser
::wxGetPasswordFromUser
::wxGetTextFromUser
::wxGetSingleChoice
::wxGetSingleChoiceIndex
::wxGetSingleChoiceData
::wxInfoMessageBox
::wxIsBusy
::wxMessageBox
::wxShowTip

 @section page_func_cat_math Math functions
Include files
<wx/math.h>
wxFinite
wxIsNaN

 @section page_func_cat_gdi GDI functions
The following are relevant to the GDI (Graphics Device Interface).
Include files
<wx/gdicmn.h>
wxBITMAP
::wxClientDisplayRect
::wxColourDisplay
::wxDisplayDepth
::wxDisplaySize
::wxDisplaySizeMM
::wxDROP_ICON
wxICON
::wxMakeMetafilePlaceable
::wxSetCursor

 @section page_func_cat_printer Printer settings
NB: These routines are obsolete and should no longer be used!
The following functions are used to control PostScript printing. Under Windows, PostScript output can only be sent to a file.
Include files
<wx/dcps.h>
::wxGetPrinterCommand
::wxGetPrinterFile
::wxGetPrinterMode
::wxGetPrinterOptions
::wxGetPrinterOrientation
::wxGetPrinterPreviewCommand
::wxGetPrinterScaling
::wxGetPrinterTranslation
::wxSetPrinterCommand
::wxSetPrinterFile
::wxSetPrinterMode
::wxSetPrinterOptions
::wxSetPrinterOrientation
::wxSetPrinterPreviewCommand
::wxSetPrinterScaling
::wxSetPrinterTranslation

 @section page_func_cat_clipboard Clipboard functions
These clipboard functions are implemented for Windows only. The use of these functions is deprecated and the code is no longer maintained. Use the wxClipboard class instead.
Include files
<wx/clipbrd.h>
::wxClipboardOpen
::wxCloseClipboard
::wxEmptyClipboard
::wxEnumClipboardFormats
::wxGetClipboardData
::wxGetClipboardFormatName
::wxIsClipboardFormatAvailable
::wxOpenClipboard
::wxRegisterClipboardFormat
::wxSetClipboardData

 @section page_func_cat_misc Miscellaneous functions and macros
::wxBase64Decode
::wxBase64DecodedSize
::wxBase64Encode
::wxBase64EncodedSize
::wxGetKeyState
::wxNewId
::wxRegisterId
::wxDDECleanUp
::wxDDEInitialize
::wxEnableTopLevelWindows
::wxFindMenuItemId
::wxFindWindowByLabel
::wxFindWindowByName
::wxFindWindowAtPoint
::wxFindWindowAtPointer
::wxGetActiveWindow
::wxGetBatteryState
::wxGetDisplayName
::wxGetPowerType
::wxGetMousePosition
::wxGetMouseState
::wxGetStockLabel
::wxGetTopLevelParent
::wxLaunchDefaultBrowser
::wxLoadUserResource
::wxPostDelete
::wxPostEvent
::wxSetDisplayName
::wxStripMenuCodes


 @section page_func_cat_log Log functions
These functions provide a variety of logging functions: see Log classes overview for further information. The functions use (implicitly) the currently active log target, so their descriptions here may not apply if the log target is not the standard one (installed by wxWidgets in the beginning of the program).
Include files
<wx/log.h>
::wxDebugMsg
::wxError
::wxFatalError
::wxLogError
::wxLogFatalError
::wxLogWarning
::wxLogMessage
::wxLogVerbose
::wxLogStatus
::wxLogSysError
::wxLogDebug
::wxLogTrace
::wxSafeShowMessage
::wxSysErrorCode
::wxSysErrorMsg
WXTRACE
WXTRACELEVEL
::wxTrace
::wxTraceLevel

 @section page_func_cat_time Time functions
The functions in this section deal with getting the current time and sleeping for the specified time interval.
::wxGetLocalTime
::wxGetLocalTimeMillis
::wxGetUTCTime
::wxMicroSleep
::wxMilliSleep
::wxNow
::wxSleep
::wxUsleep


 @section page_func_cat_debug Debugging macros and functions
Useful macros and functions for error checking and defensive programming. wxWidgets defines three families of the assert-like macros: the wxASSERT and wxFAIL macros only do anything if __WXDEBUG__ is defined (in other words, in the debug build) but disappear completely in the release build. On the other hand, the wxCHECK macros stay event in release builds but a check failure doesn't generate any user-visible effects then. Finally, the compile time assertions don't happen during the run-time but result in the compilation error messages if the condition they check fail.
Include files
<wx/debug.h>
::wxOnAssert
::wxTrap
::wxIsDebuggerRunning

 @section page_func_cat_env Environment access functions
The functions in this section allow to access (get) or change value of environment variables in a portable way. They are currently implemented under Win32 and POSIX-like systems (Unix).
Include files
<wx/utils.h>
wxGetenv
wxGetEnv
wxSetEnv
wxUnsetEnv

 @section page_func_cat_atomic Atomic operation functions
When using multi-threaded applications, it is often required to access or modify memory which is shared between threads. Atomic integer and pointer operations are an efficient way to handle this issue (another, less efficient, way is to use a mutex or critical section). A native implementation exists for Windows, Linux, Solaris and Mac OS X, for other OS, a wxCriticalSection is used to protect the data.
One particular application is reference counting (used by so-called smart pointers).
You should define your variable with the type wxAtomicInt in order to apply atomic operations to it.
Include files
<wx/atomic.h>
::wxAtomicInc
::wxAtomicDec

*/