/////////////////////////////////////////////////////////////////////////////
// Name:        cat_functions.h
// Purpose:     Functions-by-category page of the Doxygen manual
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**

@page page_func_cat Functions by Category

@li @ref page_func_cat_appinitterm
@li @ref page_func_cat_procctrl
@li @ref page_func_cat_thread
@li @ref page_func_cat_file
@li @ref page_func_cat_networkuseros
@li @ref page_func_cat_string
@li @ref page_func_cat_dialog
@li @ref page_func_cat_math
@li @ref page_func_cat_gdi
@li @ref page_func_cat_clipboard
@li @ref page_func_cat_log
@li @ref page_func_cat_time
@li @ref page_func_cat_debug
@li @ref page_func_cat_env
@li @ref page_func_cat_atomic
@li @ref page_func_cat_misc


<hr>



@section page_func_cat_appinitterm Application Initialization and Termination

The functions in this section are used on application startup/shutdown and also
to control the behaviour of the main event loop of the GUI programs.

@li wxEntry()
@li wxEntryCleanup()
@li wxEntryStart()
@li wxGetApp()
@li wxHandleFatalExceptions()
@li wxInitAllImageHandlers()
@li wxInitialize()
@li wxSafeYield()
@li wxUninitialize()
@li wxYield()
@li wxWakeUpIdle()


@section page_func_cat_procctrl Process Control

The functions in this section are used to launch or terminate the other
processes.

@li wxExecute()
@li wxExit()
@li wxJoin()
@li wxKill()
@li wxGetProcessId()
@li wxShell()
@li wxShutdown()


@section page_func_cat_thread Threads

The functions and macros here mainly exist to make it writing the code which
may be compiled in multi thread build (wxUSE_THREADS = 1) as well as in single
thread configuration (wxUSE_THREADS = 0).

For example, a static variable must be protected against simultaneous access by
multiple threads in the former configuration but in the latter the extra
overhead of using the critical section is not needed. To solve this problem,
the wxCRITICAL_SECTION macro may be used to create and use the critical section
only when needed.

@header{wx/thread.h}

@sa wxThread, wxMutex, @ref overview_thread

@li wxIsMainThread()
@li wxMutexGuiEnter()
@li wxMutexGuiLeave()


@section page_func_cat_file Files and Directories

@header{wx/filefn.h}

@sa wxPathList, wxDir, wxFile, wxFileName

@li wxDos2UnixFilename()
@li wxFileExists()
@li wxFileModificationTime()
@li wxFileNameFromPath()
@li wxFindFirstFile()
@li wxFindNextFile()
@li wxGetDiskSpace()
@li wxGetFileKind()
@li wxGetOSDirectory()
@li wxIsAbsolutePath()
@li wxDirExists()
@li wxPathOnly()
@li wxUnix2DosFilename()
@li wxConcatFiles()
@li wxCopyFile()
@li wxGetCwd()
@li wxGetWorkingDirectory()
@li wxGetTempFileName()
@li wxIsWild()
@li wxMatchWild()
@li wxMkdir()
@li wxParseCommonDialogsFilter()
@li wxRemoveFile()
@li wxRenameFile()
@li wxRmdir()
@li wxSetWorkingDirectory()
@li wxSplit()
@li wxSplitPath()
@li wxTransferFileToStream()
@li wxTransferStreamToFile()


@section page_func_cat_networkuseros Network, User and OS

The functions in this section are used to retrieve information about the
current computer and/or user characteristics.

@li wxGetEmailAddress()
@li wxGetFreeMemory()
@li wxGetFullHostName()
@li wxGetHomeDir()
@li wxGetHostName()
@li wxGetOsDescription()
@li wxGetOsVersion()
@li wxIsPlatformLittleEndian()
@li wxIsPlatform64Bit()
@li wxGetUserHome()
@li wxGetUserId()
@li wxGetUserName()


@section page_func_cat_string Strings

@li wxGetTranslation()
@li wxIsEmpty()
@li wxStrcmp()
@li wxStricmp()
@li wxStringEq()
@li wxStringMatch()
@li wxStringTokenize()
@li wxStrlen()
@li wxSnprintf()
@li wxVsnprintf()


@section page_func_cat_dialog Dialogs

Below are a number of convenience functions for getting input from the user
or displaying messages. Note that in these functions the last three parameters
are optional. However, it is recommended to pass a parent frame parameter, or
(in MS Windows or Motif) the wrong window frame may be brought to the front when
the dialog box is popped up.

@li wxAboutBox()
@li wxBeginBusyCursor()
@li wxBell()
@li wxCreateFileTipProvider()
@li wxDirSelector()
@li wxFileSelector()
@li wxEndBusyCursor()
@li wxGenericAboutBox()
@li wxGetColourFromUser()
@li wxGetFontFromUser()
@li wxGetMultipleChoices()
@li wxGetNumberFromUser()
@li wxGetPasswordFromUser()
@li wxGetTextFromUser()
@li wxGetSingleChoice()
@li wxGetSingleChoiceIndex()
@li wxGetSingleChoiceData()
@li wxInfoMessageBox()
@li wxIsBusy()
@li wxMessageBox()
@li wxShowTip()


@section page_func_cat_math Math

@header{wx/math.h}

@li wxFinite()
@li wxIsNaN()


@section page_func_cat_gdi Graphics Device Interface

The following are relevant to the GDI (Graphics Device Interface).

@header{wx/gdicmn.h}

@li wxBITMAP()
@li wxClientDisplayRect()
@li wxColourDisplay()
@li wxDisplayDepth()
@li wxDisplaySize()
@li wxDisplaySizeMM()
@li wxDROP_ICON()
@li wxICON()
@li wxMakeMetafilePlaceable()
@li wxSetCursor()


@section page_func_cat_clipboard Clipboard

@warning These functions are deprecated, use the wxClipboard class instead.

These clipboard functions are implemented for Windows only. 

@header{wx/clipbrd.h}

@li wxClipboardOpen()
@li wxCloseClipboard()
@li wxEmptyClipboard()
@li wxEnumClipboardFormats()
@li wxGetClipboardData()
@li wxGetClipboardFormatName()
@li wxIsClipboardFormatAvailable()
@li wxOpenClipboard()
@li wxRegisterClipboardFormat()
@li wxSetClipboardData()


@section page_func_cat_log Logging

These functions provide a variety of logging functions. The functions use
(implicitly) the currently active log target, so their descriptions here may
not apply if the log target is not the standard one (installed by wxWidgets in
the beginning of the program).

Related Overviews: @ref overview_log

@header{wx/log.h}

@li wxDebugMsg()
@li wxError()
@li wxFatalError()
@li wxLogError()
@li wxLogFatalError()
@li wxLogWarning()
@li wxLogMessage()
@li wxLogVerbose()
@li wxLogStatus()
@li wxLogSysError()
@li wxLogDebug()
@li wxLogTrace()
@li wxSafeShowMessage()
@li wxSysErrorCode()
@li wxSysErrorMsg()
@li wxTrace()
@li wxTraceLevel()


@section page_func_cat_time Time

The functions in this section deal with getting the current time and sleeping
for the specified time interval.

@li wxGetLocalTime()
@li wxGetLocalTimeMillis()
@li wxGetUTCTime()
@li wxMicroSleep()
@li wxMilliSleep()
@li wxNow()
@li wxSleep()
@li wxUsleep()


@section page_func_cat_debug Debugging

Useful macros and functions for error checking and defensive programming.
wxWidgets defines three families of the assert-like macros: the wxASSERT and
wxFAIL macros only do anything if __WXDEBUG__ is defined (in other words, in
the debug build) but disappear completely in the release build. On the other
hand, the wxCHECK macros stay event in release builds but a check failure
doesn't generate any user-visible effects then. Finally, the compile time
assertions don't happen during the run-time but result in the compilation error
messages if the condition they check fail.

@header{wx/debug.h}

@li wxOnAssert()
@li wxTrap()
@li wxIsDebuggerRunning()


@section page_func_cat_env Environment

The functions in this section allow to access (get) or change value of
environment variables in a portable way. They are currently implemented under
Win32 and POSIX-like systems (Unix).

@header{wx/utils.h}

@li wxGetenv()
@li wxGetEnv()
@li wxSetEnv()
@li wxUnsetEnv()


@section page_func_cat_atomic Atomic Operations

When using multi-threaded applications, it is often required to access or
modify memory which is shared between threads. Atomic integer and pointer
operations are an efficient way to handle this issue (another, less efficient,
way is to use a mutex or critical section). A native implementation exists for
Windows, Linux, Solaris and Mac OS X, for other OS, a wxCriticalSection is used
to protect the data.

One particular application is reference counting (used by so-called smart
pointers).

You should define your variable with the type wxAtomicInt in order to apply
atomic operations to it.

@header{wx/atomic.h}

@li wxAtomicInc()
@li wxAtomicDec()


@section page_func_cat_misc Miscellaneous

@li wxBase64Decode()
@li wxBase64DecodedSize()
@li wxBase64Encode()
@li wxBase64EncodedSize()
@li wxGetKeyState()
@li wxNewId()
@li wxRegisterId()
@li wxDDECleanUp()
@li wxDDEInitialize()
@li wxEnableTopLevelWindows()
@li wxFindMenuItemId()
@li wxFindWindowByLabel()
@li wxFindWindowByName()
@li wxFindWindowAtPoint()
@li wxFindWindowAtPointer()
@li wxGetActiveWindow()
@li wxGetBatteryState()
@li wxGetDisplayName()
@li wxGetPowerType()
@li wxGetMousePosition()
@li wxGetMouseState()
@li wxGetStockLabel()
@li wxGetTopLevelParent()
@li wxLaunchDefaultBrowser()
@li wxLoadUserResource()
@li wxPostDelete()
@li wxPostEvent()
@li wxSetDisplayName()
@li wxStripMenuCodes()

*/

