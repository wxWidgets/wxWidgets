/////////////////////////////////////////////////////////////////////////////
// Name:        wx/android/setup.h
// Purpose:     Configuration for the android build of the library
// Author:      Julian Smart
// Created:     01/02/97
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_SETUP_H_
#define _WX_SETUP_H_

/* --- start common options --- */
// ----------------------------------------------------------------------------
// global settings
// ----------------------------------------------------------------------------

// define this to 0 when building wxBase library - this can also be done from
// makefile/project file overriding the value here
#ifndef wxUSE_GUI
    #define wxUSE_GUI            1
#endif // wxUSE_GUI

// ----------------------------------------------------------------------------
// compatibility settings
// ----------------------------------------------------------------------------

// This setting determines the compatibility with 2.8 API: set it to 0 to
// flag all cases of using deprecated functions.
//
// Default is 1 but please try building your code with 0 as the default will
// change to 0 in the next version and the deprecated functions will disappear
// in the version after it completely.
//
// Recommended setting: 0 (please update your code)
#define WXWIN_COMPATIBILITY_2_8 0

// This setting determines the compatibility with 3.0 API: set it to 0 to
// flag all cases of using deprecated functions.
//
// Default is 1 but please try building your code with 0 as the default will
// change to 0 in the next version and the deprecated functions will disappear
// in the version after it completely.
//
// Recommended setting: 0 (please update your code)
#define WXWIN_COMPATIBILITY_3_0 1

// MSW-only: Set to 0 for accurate dialog units, else 1 for old behaviour when
// default system font is used for wxWindow::GetCharWidth/Height() instead of
// the current font.
//
// Default is 0
//
// Recommended setting: 0
#define wxDIALOG_UNIT_COMPATIBILITY   0

// Provide unsafe implicit conversions in wxString to "const char*" or
// "std::string" (depending on wxUSE_STD_STRING_CONV_IN_WXSTRING value).
//
// Default is 1 but only for compatibility reasons, it is recommended to set
// this to 0 because converting wxString to a narrow (non-Unicode) string may
// fail unless a locale using UTF-8 encoding is used, which is never the case
// under MSW, for example, hence such conversions can result in silent data
// loss.
//
// Recommended setting: 0
#define wxUSE_UNSAFE_WXSTRING_CONV 1

// If set to 1, enables "reproducible builds", i.e. build output should be
// exactly the same if the same build is redone again. As using __DATE__ and
// __TIME__ macros clearly makes the build irreproducible, setting this option
// to 1 disables their use in the library code.
//
// Default is 0
//
// Recommended setting: 0
#define wxUSE_REPRODUCIBLE_BUILD 0

// ----------------------------------------------------------------------------
// debugging settings
// ----------------------------------------------------------------------------

// wxDEBUG_LEVEL will be defined as 1 in wx/debug.h so normally there is no
// need to define it here. You may do it for two reasons: either completely
// disable/compile out the asserts in release version (then do it inside #ifdef
// NDEBUG) or, on the contrary, enable more asserts, including the usually
// disabled ones, in the debug build (then do it inside #ifndef NDEBUG)
//
// #ifdef NDEBUG
//  #define wxDEBUG_LEVEL 0
// #else
//  #define wxDEBUG_LEVEL 2
// #endif

// wxHandleFatalExceptions() may be used to catch the program faults at run
// time and, instead of terminating the program with a usual GPF message box,
// call the user-defined wxApp::OnFatalException() function. If you set
// wxUSE_ON_FATAL_EXCEPTION to 0, wxHandleFatalExceptions() will not work.
//
// This setting is for Win32 only and can only be enabled if your compiler
// supports Win32 structured exception handling (currently only VC++ does)
//
// Default is 1
//
// Recommended setting: 1 if your compiler supports it.
#define wxUSE_ON_FATAL_EXCEPTION 1

// Set this to 1 to be able to generate a human-readable (unlike
// machine-readable minidump created by wxCrashReport::Generate()) stack back
// trace when your program crashes using wxStackWalker
//
// Default is 1 if supported by the compiler.
//
// Recommended setting: 1, set to 0 if your programs never crash
#define wxUSE_STACKWALKER 1

// Set this to 1 to compile in wxDebugReport class which allows you to create
// and optionally upload to your web site a debug report consisting of back
// trace of the crash (if wxUSE_STACKWALKER == 1) and other information.
//
// Default is 1 if supported by the compiler.
//
// Recommended setting: 1, it is compiled into a separate library so there
//                         is no overhead if you don't use it
#define wxUSE_DEBUGREPORT 1

// Generic comment about debugging settings: they are very useful if you don't
// use any other memory leak detection tools such as Purify/BoundsChecker, but
// are probably redundant otherwise. Also, Visual C++ CRT has the same features
// as wxWidgets memory debugging subsystem built in since version 5.0 and you
// may prefer to use it instead of built in memory debugging code because it is
// faster and more fool proof.
//
// Using VC++ CRT memory debugging is enabled by default in debug build (_DEBUG
// is defined) if wxUSE_GLOBAL_MEMORY_OPERATORS is *not* enabled (i.e. is 0)
// and if __NO_VC_CRTDBG__ is not defined.

// The rest of the options in this section are obsolete and not supported,
// enable them at your own risk.

// If 1, enables wxDebugContext, for writing error messages to file, etc. If
// __WXDEBUG__ is not defined, will still use the normal memory operators.
//
// Default is 0
//
// Recommended setting: 0
#define wxUSE_DEBUG_CONTEXT 0

// If 1, enables debugging versions of wxObject::new and wxObject::delete *IF*
// __WXDEBUG__ is also defined.
//
// WARNING: this code may not work with all architectures, especially if
// alignment is an issue. This switch is currently ignored for mingw / cygwin
//
// Default is 0
//
// Recommended setting: 1 if you are not using a memory debugging tool, else 0
#define wxUSE_MEMORY_TRACING 0

// In debug mode, cause new and delete to be redefined globally.
// If this causes problems (e.g. link errors which is a common problem
// especially if you use another library which also redefines the global new
// and delete), set this to 0.
// This switch is currently ignored for mingw / cygwin
//
// Default is 0
//
// Recommended setting: 0
#define wxUSE_GLOBAL_MEMORY_OPERATORS 0

// In debug mode, causes new to be defined to be WXDEBUG_NEW (see object.h). If
// this causes problems (e.g. link errors), set this to 0. You may need to set
// this to 0 if using templates (at least for VC++). This switch is currently
// ignored for MinGW/Cygwin.
//
// Default is 0
//
// Recommended setting: 0
#define wxUSE_DEBUG_NEW_ALWAYS 0


// ----------------------------------------------------------------------------
// Unicode support
// ----------------------------------------------------------------------------

// This option is deprecated: the library should be always built in Unicode mode
// now, only set wxUSE_UNICODE to 0 to compile legacy code in ANSI mode if
// absolutely necessary -- updating it is strongly recommended as the ANSI mode
// will disappear completely in future wxWidgets releases.
#ifndef wxUSE_UNICODE
    #define wxUSE_UNICODE 1
#endif

// ----------------------------------------------------------------------------
// global features
// ----------------------------------------------------------------------------

// Compile library in exception-safe mode? If set to 1, the library will try to
// behave correctly in presence of exceptions (even though it still will not
// use the exceptions itself) and notify the user code about any unhandled
// exceptions. If set to 0, propagation of the exceptions through the library
// code will lead to undefined behaviour -- but the code itself will be
// slightly smaller and faster.
//
// Note that like wxUSE_THREADS this option is automatically set to 0 if
// wxNO_EXCEPTIONS is defined.
//
// Default is 1
//
// Recommended setting: depends on whether you intend to use C++ exceptions
//                      in your own code (1 if you do, 0 if you don't)
#define wxUSE_EXCEPTIONS    1

// Set wxUSE_EXTENDED_RTTI to 1 to use extended RTTI
//
// Default is 0
//
// Recommended setting: 0 (this is still work in progress...)
#define wxUSE_EXTENDED_RTTI 0

// Support for message/error logging. This includes wxLogXXX() functions and
// wxLog and derived classes. Don't set this to 0 unless you really know what
// you are doing.
//
// Default is 1
//
// Recommended setting: 1 (always)
#define wxUSE_LOG 1

// Recommended setting: 1
#define wxUSE_LOGWINDOW 1

// Recommended setting: 1
#define wxUSE_LOGGUI 1

// Recommended setting: 1
#define wxUSE_LOG_DIALOG 1

// Support for command line parsing using wxCmdLineParser class.
//
// Default is 1
//
// Recommended setting: 1 (can be set to 0 if you don't use the cmd line)
#define wxUSE_CMDLINE_PARSER 1

// Support for multithreaded applications: if 1, compile in thread classes
// (thread.h) and make the library a bit more thread safe. Although thread
// support is quite stable by now, you may still consider recompiling the
// library without it if you have no use for it - this will result in a
// somewhat smaller and faster operation.
//
// Notice that if wxNO_THREADS is defined, wxUSE_THREADS is automatically reset
// to 0 in wx/chkconf.h, so, for example, if you set USE_THREADS to 0 in
// build/msw/config.* file this value will have no effect.
//
// Default is 1
//
// Recommended setting: 0 unless you do plan to develop MT applications
#define wxUSE_THREADS 1

// If enabled, compiles wxWidgets streams classes
//
// wx stream classes are used for image IO, process IO redirection, network
// protocols implementation and much more and so disabling this results in a
// lot of other functionality being lost.
//
// Default is 1
//
// Recommended setting: 1 as setting it to 0 disables many other things
#define wxUSE_STREAMS       1

// Support for positional parameters (e.g. %1$d, %2$s ...) in wxVsnprintf.
// Note that if the system's implementation does not support positional
// parameters, setting this to 1 forces the use of the wxWidgets implementation
// of wxVsnprintf. The standard vsnprintf() supports positional parameters on
// many Unix systems but usually doesn't under Windows.
//
// Positional parameters are very useful when translating a program since using
// them in formatting strings allow translators to correctly reorder the
// translated sentences.
//
// Default is 1
//
// Recommended setting: 1 if you want to support multiple languages
#define wxUSE_PRINTF_POS_PARAMS      1

// Enable the use of compiler-specific thread local storage keyword, if any.
// This is used for wxTLS_XXX() macros implementation and normally should use
// the compiler-provided support as it's simpler and more efficient, but is
// disabled under Windows in wx/msw/chkconf.h as it can't be used if wxWidgets
// is used in a dynamically loaded Win32 DLL (i.e. using LoadLibrary()) under
// XP as this triggers a bug in compiler TLS support that results in crashes
// when any TLS variables are used.
//
// If you're absolutely sure that your build of wxWidgets is never going to be
// used in such situation, either because it's not going to be linked from any
// kind of plugin or because you only target Vista or later systems, you can
// set this to 2 to force the use of compiler TLS even under MSW.
//
// Default is 1 meaning that compiler TLS is used only if it's 100% safe.
//
// Recommended setting: 2 if you want to have maximal performance and don't
// care about the scenario described above.
#define wxUSE_COMPILER_TLS 1

// ----------------------------------------------------------------------------
// Interoperability with the standard library.
// ----------------------------------------------------------------------------

// Set wxUSE_STL to 1 to enable maximal interoperability with the standard
// library, even at the cost of backwards compatibility.
//
// Default is 0
//
// Recommended setting: 0 as the options below already provide a relatively
// good level of interoperability and changing this option arguably isn't worth
// diverging from the official builds of the library.
#define wxUSE_STL 0

// This is not a real option but is used as the default value for
// wxUSE_STD_IOSTREAM, wxUSE_STD_STRING and wxUSE_STD_CONTAINERS_COMPATIBLY.
//
// Set it to 0 if you want to disable the use of all standard classes
// completely for some reason.
#define wxUSE_STD_DEFAULT  1

// Use standard C++ containers where it can be done without breaking backwards
// compatibility.
//
// This provides better interoperability with the standard library, e.g. with
// this option on it's possible to insert std::vector<> into many wxWidgets
// containers directly.
//
// Default is 1.
//
// Recommended setting is 1 unless you want to avoid all dependencies on the
// standard library.
#define wxUSE_STD_CONTAINERS_COMPATIBLY wxUSE_STD_DEFAULT

// Use standard C++ containers to implement wxVector<>, wxStack<>, wxDList<>
// and wxHashXXX<> classes. If disabled, wxWidgets own (mostly compatible but
// usually more limited) implementations are used which allows to avoid the
// dependency on the C++ run-time library.
//
// Default is 0 for compatibility reasons.
//
// Recommended setting: 1 unless compatibility with the official wxWidgets
// build and/or the existing code is a concern.
#define wxUSE_STD_CONTAINERS 0

// Use standard C++ streams if 1 instead of wx streams in some places. If
// disabled, wx streams are used everywhere and wxWidgets doesn't depend on the
// standard streams library.
//
// Notice that enabling this does not replace wx streams with std streams
// everywhere, in a lot of places wx streams are used no matter what.
//
// Default is 1 if compiler supports it.
//
// Recommended setting: 1 if you use the standard streams anyhow and so
//                      dependency on the standard streams library is not a
//                      problem
#define wxUSE_STD_IOSTREAM  wxUSE_STD_DEFAULT

// Enable minimal interoperability with the standard C++ string class if 1.
// "Minimal" means that wxString can be constructed from std::string or
// std::wstring but can't be implicitly converted to them. You need to enable
// the option below for the latter.
//
// Default is 1 for most compilers.
//
// Recommended setting: 1 unless you want to ensure your program doesn't use
//                      the standard C++ library at all.
#define wxUSE_STD_STRING  wxUSE_STD_DEFAULT

// Make wxString as much interchangeable with std::[w]string as possible, in
// particular allow implicit conversion of wxString to either of these classes.
// This comes at a price (or a benefit, depending on your point of view) of not
// allowing implicit conversion to "const char *" and "const wchar_t *".
//
// Because a lot of existing code relies on these conversions, this option is
// disabled by default but can be enabled for your build if you don't care
// about compatibility.
//
// Default is 0 if wxUSE_STL has its default value or 1 if it is enabled.
//
// Recommended setting: 0 to remain compatible with the official builds of
// wxWidgets.
#define wxUSE_STD_STRING_CONV_IN_WXSTRING wxUSE_STL

// ----------------------------------------------------------------------------
// non GUI features selection
// ----------------------------------------------------------------------------

// Set wxUSE_LONGLONG to 1 to compile the wxLongLong class. This is a 64 bit
// integer which is implemented in terms of native 64 bit integers if any or
// uses emulation otherwise.
//
// This class is required by wxDateTime and so you should enable it if you want
// to use wxDateTime. For most modern platforms, it will use the native 64 bit
// integers in which case (almost) all of its functions are inline and it
// almost does not take any space, so there should be no reason to switch it
// off.
//
// Recommended setting: 1
#define wxUSE_LONGLONG      1

// Set wxUSE_BASE64 to 1, to compile in Base64 support. This is required for
// storing binary data in wxConfig on most platforms.
//
// Default is 1.
//
// Recommended setting: 1 (but can be safely disabled if you don't use it)
#define wxUSE_BASE64        1

// Set this to 1 to be able to use wxEventLoop even in console applications
// (i.e. using base library only, without GUI). This is mostly useful for
// processing socket events but is also necessary to use timers in console
// applications
//
// Default is 1.
//
// Recommended setting: 1 (but can be safely disabled if you don't use it)
#define wxUSE_CONSOLE_EVENTLOOP 1

// Set wxUSE_(F)FILE to 1 to compile wx(F)File classes. wxFile uses low level
// POSIX functions for file access, wxFFile uses ANSI C stdio.h functions.
//
// Default is 1
//
// Recommended setting: 1 (wxFile is highly recommended as it is required by
// i18n code, wxFileConfig and others)
#define wxUSE_FILE          1
#define wxUSE_FFILE         1

// Use wxFSVolume class providing access to the configured/active mount points
//
// Default is 1
//
// Recommended setting: 1 (but may be safely disabled if you don't use it)
#define wxUSE_FSVOLUME      1

// Use wxSecretStore class for storing passwords using OS-specific facilities.
//
// Default is 1
//
// Recommended setting: 1 (but may be safely disabled if you don't use it)
#define wxUSE_SECRETSTORE   1

// Use wxStandardPaths class which allows to retrieve some standard locations
// in the file system
//
// Default is 1
//
// Recommended setting: 1 (may be disabled to save space, but not much)
#define wxUSE_STDPATHS      1

// use wxTextBuffer class: required by wxTextFile
#define wxUSE_TEXTBUFFER    1

// use wxTextFile class: requires wxFile and wxTextBuffer, required by
// wxFileConfig
#define wxUSE_TEXTFILE      1

// i18n support: _() macro, wxLocale class. Requires wxTextFile.
#define wxUSE_INTL          1

// Provide wxFoo_l() functions similar to standard foo() functions but taking
// an extra locale parameter.
//
// Notice that this is fully implemented only for the systems providing POSIX
// xlocale support or Microsoft Visual C++ >= 8 (which provides proprietary
// almost-equivalent of xlocale functions), otherwise wxFoo_l() functions will
// only work for the current user locale and "C" locale. You can use
// wxHAS_XLOCALE_SUPPORT to test whether the full support is available.
//
// Default is 1
//
// Recommended setting: 1 but may be disabled if you are writing programs
// running only in C locale anyhow
#define wxUSE_XLOCALE       1

// Set wxUSE_DATETIME to 1 to compile the wxDateTime and related classes which
// allow to manipulate dates, times and time intervals.
//
// Requires: wxUSE_LONGLONG
//
// Default is 1
//
// Recommended setting: 1
#define wxUSE_DATETIME      1

// Set wxUSE_TIMER to 1 to compile wxTimer class
//
// Default is 1
//
// Recommended setting: 1
#define wxUSE_TIMER         1

// Use wxStopWatch clas.
//
// Default is 1
//
// Recommended setting: 1 (needed by wxSocket)
#define wxUSE_STOPWATCH     1

// Set wxUSE_FSWATCHER to 1 if you want to enable wxFileSystemWatcher
//
// Default is 1
//
// Recommended setting: 1
#define wxUSE_FSWATCHER     1

// Setting wxUSE_CONFIG to 1 enables the use of wxConfig and related classes
// which allow the application to store its settings in the persistent
// storage. Setting this to 1 will also enable on-demand creation of the
// global config object in wxApp.
//
// See also wxUSE_CONFIG_NATIVE below.
//
// Recommended setting: 1
#define wxUSE_CONFIG        1

// If wxUSE_CONFIG is 1, you may choose to use either the native config
// classes under Windows (using .INI files under Win16 and the registry under
// Win32) or the portable text file format used by the config classes under
// Unix.
//
// Default is 1 to use native classes. Note that you may still use
// wxFileConfig even if you set this to 1 - just the config object created by
// default for the applications needs will be a wxRegConfig or wxIniConfig and
// not wxFileConfig.
//
// Recommended setting: 1
#define wxUSE_CONFIG_NATIVE   1

// If wxUSE_DIALUP_MANAGER is 1, compile in wxDialUpManager class which allows
// to connect/disconnect from the network and be notified whenever the dial-up
// network connection is established/terminated. Requires wxUSE_DYNAMIC_LOADER.
//
// Default is 1.
//
// Recommended setting: 1
#define wxUSE_DIALUP_MANAGER   1

// Compile in classes for run-time DLL loading and function calling.
// Required by wxUSE_DIALUP_MANAGER.
//
// This setting is for Win32 only
//
// Default is 1.
//
// Recommended setting: 1
#define wxUSE_DYNLIB_CLASS    1

// experimental, don't use for now
#define wxUSE_DYNAMIC_LOADER  1

// Set to 1 to use socket classes
#define wxUSE_SOCKETS       1

// Set to 1 to use ipv6 socket classes (requires wxUSE_SOCKETS)
//
// Notice that currently setting this option under Windows will result in
// programs which can only run on recent OS versions (with ws2_32.dll
// installed) which is why it is disabled by default.
//
// Default is 1.
//
// Recommended setting: 1 if you need IPv6 support
#define wxUSE_IPV6          0

// Set to 1 to enable virtual file systems (required by wxHTML)
#define wxUSE_FILESYSTEM    1

// Set to 1 to enable virtual ZIP filesystem (requires wxUSE_FILESYSTEM)
#define wxUSE_FS_ZIP        1

// Set to 1 to enable virtual archive filesystem (requires wxUSE_FILESYSTEM)
#define wxUSE_FS_ARCHIVE    1

// Set to 1 to enable virtual Internet filesystem (requires wxUSE_FILESYSTEM)
#define wxUSE_FS_INET       1

// wxArchive classes for accessing archives such as zip and tar
#define wxUSE_ARCHIVE_STREAMS     1

// Set to 1 to compile wxZipInput/OutputStream classes.
#define wxUSE_ZIPSTREAM     1

// Set to 1 to compile wxTarInput/OutputStream classes.
#define wxUSE_TARSTREAM     1

// Set to 1 to compile wxZlibInput/OutputStream classes. Also required by
// wxUSE_LIBPNG
#define wxUSE_ZLIB          1

// Set to 1 if liblzma is available to enable wxLZMA{Input,Output}Stream
// classes.
//
// Notice that if you enable this build option when not using configure or
// CMake, you need to ensure that liblzma headers and libraries are available
// (i.e. by building the library yourself or downloading its binaries) and can
// be found, either by copying them to one of the locations searched by the
// compiler/linker by default (e.g. any of the directories in the INCLUDE or
// LIB environment variables, respectively, when using MSVC) or modify the
// make- or project files to add references to these directories.
//
// Default is 0 under MSW, auto-detected by configure.
//
// Recommended setting: 1 if you need LZMA compression.
#define wxUSE_LIBLZMA       0

// If enabled, the code written by Apple will be used to write, in a portable
// way, float on the disk. See extended.c for the license which is different
// from wxWidgets one.
//
// Default is 1.
//
// Recommended setting: 1 unless you don't like the license terms (unlikely)
#define wxUSE_APPLE_IEEE          1

// Joystick support class
#define wxUSE_JOYSTICK            1

// wxFontEnumerator class
#define wxUSE_FONTENUM 1

// wxFontMapper class
#define wxUSE_FONTMAP 1

// wxMimeTypesManager class
#define wxUSE_MIMETYPE 1

// wxProtocol and related classes: if you want to use either of wxFTP, wxHTTP
// or wxURL you need to set this to 1.
//
// Default is 1.
//
// Recommended setting: 1
#define wxUSE_PROTOCOL 1

// The settings for the individual URL schemes
#define wxUSE_PROTOCOL_FILE 1
#define wxUSE_PROTOCOL_FTP 1
#define wxUSE_PROTOCOL_HTTP 1

// Define this to use wxURL class.
#define wxUSE_URL 1

// Define this to use native platform url and protocol support.
// Currently valid only for MS-Windows.
// Note: if you set this to 1, you can open ftp/http/gopher sites
// and obtain a valid input stream for these sites
// even when you set wxUSE_PROTOCOL_FTP/HTTP to 0.
// Doing so reduces the code size.
//
// This code is experimental and subject to change.
#define wxUSE_URL_NATIVE 0

// Support for wxVariant class used in several places throughout the library,
// notably in wxDataViewCtrl API.
//
// Default is 1.
//
// Recommended setting: 1 unless you want to reduce the library size as much as
// possible in which case setting this to 0 can gain up to 100KB.
#define wxUSE_VARIANT 1

// Support for wxAny class, the successor for wxVariant.
//
// Default is 1.
//
// Recommended setting: 1 unless you want to reduce the library size by a small amount,
// or your compiler cannot for some reason cope with complexity of templates used.
#define wxUSE_ANY 1

// Support for regular expression matching via wxRegEx class: enable this to
// use POSIX regular expressions in your code. You need to compile regex
// library from src/regex to use it under Windows.
//
// Default is 0
//
// Recommended setting: 1 if your compiler supports it, if it doesn't please
// contribute us a makefile for src/regex for it
#define wxUSE_REGEX       1

// wxSystemOptions class
#define wxUSE_SYSTEM_OPTIONS 1

// wxSound class
#define wxUSE_SOUND      1

// Use wxMediaCtrl
//
// Default is 1.
//
// Recommended setting: 1
#define wxUSE_MEDIACTRL     1

// Use wxWidget's XRC XML-based resource system.  Recommended.
//
// Default is 1
//
// Recommended setting: 1 (requires wxUSE_XML)
#define wxUSE_XRC       1

// XML parsing classes. Note that their API will change in the future, so
// using wxXmlDocument and wxXmlNode in your app is not recommended.
//
// Default is the same as wxUSE_XRC, i.e. 1 by default.
//
// Recommended setting: 1 (required by XRC)
#define wxUSE_XML       wxUSE_XRC

// Use wxWidget's AUI docking system
//
// Default is 1
//
// Recommended setting: 1
#define wxUSE_AUI       1

// Use wxWidget's Ribbon classes for interfaces
//
// Default is 1
//
// Recommended setting: 1
#define wxUSE_RIBBON    1

// Use wxPropertyGrid.
//
// Default is 1
//
// Recommended setting: 1
#define wxUSE_PROPGRID  1

// Use wxStyledTextCtrl, a wxWidgets implementation of Scintilla.
//
// Default is 1
//
// Recommended setting: 1
#define wxUSE_STC 1

// Use wxWidget's web viewing classes
//
// Default is 1
//
// Recommended setting: 1
#define wxUSE_WEBVIEW 1

// Use the IE wxWebView backend
//
// Default is 1 on MSW
//
// Recommended setting: 1
#ifdef __WXMSW__
#define wxUSE_WEBVIEW_IE 1
#else
#define wxUSE_WEBVIEW_IE 0
#endif

// Use the WebKit wxWebView backend
//
// Default is 1 on GTK and OSX
//
// Recommended setting: 1
#if (defined(__WXGTK__) && !defined(__WXGTK3__)) || defined(__WXOSX__)
#define wxUSE_WEBVIEW_WEBKIT 1
#else
#define wxUSE_WEBVIEW_WEBKIT 0
#endif

// Use the WebKit2 wxWebView backend
//
// Default is 1 on GTK3
//
// Recommended setting: 1
#if defined(__WXGTK3__)
#define wxUSE_WEBVIEW_WEBKIT2 1
#else
#define wxUSE_WEBVIEW_WEBKIT2 0
#endif

// Enable wxGraphicsContext and related classes for a modern 2D drawing API.
//
// Default is 1 except if you're using a compiler without support for GDI+
// under MSW, i.e. gdiplus.h and related headers (MSVC and MinGW >= 4.8 are
// known to have them). For other compilers (e.g. older mingw32) you may need
// to install the headers (and just the headers) yourself. If you do, change
// the setting below manually.
//
// Recommended setting: 1 if supported by the compilation environment

// Notice that we can't use wxCHECK_VISUALC_VERSION() nor wxCHECK_GCC_VERSION()
// here as this file is included from wx/platform.h before they're defined.
#if defined(_MSC_VER) || \
    (defined(__MINGW32__) && (__GNUC__ > 4 || __GNUC_MINOR__ >= 8))
#define wxUSE_GRAPHICS_CONTEXT 1
#else
// Disable support for other Windows compilers, enable it if your compiler
// comes with new enough SDK or you installed the headers manually.
//
// Notice that this will be set by configure under non-Windows platforms
// anyhow so the value there is not important.
#define wxUSE_GRAPHICS_CONTEXT 0
#endif

// Enable wxGraphicsContext implementation using Cairo library.
//
// This is not needed under Windows and detected automatically by configure
// under other systems, however you may set this to 1 manually if you installed
// Cairo under Windows yourself and prefer to use it instead the native GDI+
// implementation.
//
// Default is 0
//
// Recommended setting: 0
#define wxUSE_CAIRO 0


// ----------------------------------------------------------------------------
// Individual GUI controls
// ----------------------------------------------------------------------------

// You must set wxUSE_CONTROLS to 1 if you are using any controls at all
// (without it, wxControl class is not compiled)
//
// Default is 1
//
// Recommended setting: 1 (don't change except for very special programs)
#define wxUSE_CONTROLS     1

// Support markup in control labels, i.e. provide wxControl::SetLabelMarkup().
// Currently markup is supported only by a few controls and only some ports but
// their number will increase with time.
//
// Default is 1
//
// Recommended setting: 1 (may be set to 0 if you want to save on code size)
#define wxUSE_MARKUP       1

// wxPopupWindow class is a top level transient window. It is currently used
// to implement wxTipWindow
//
// Default is 1
//
// Recommended setting: 1 (may be set to 0 if you don't wxUSE_TIPWINDOW)
#define wxUSE_POPUPWIN     1

// wxTipWindow allows to implement the custom tooltips, it is used by the
// context help classes. Requires wxUSE_POPUPWIN.
//
// Default is 1
//
// Recommended setting: 1 (may be set to 0)
#define wxUSE_TIPWINDOW    1

// Each of the settings below corresponds to one wxWidgets control. They are
// all switched on by default but may be disabled if you are sure that your
// program (including any standard dialogs it can show!) doesn't need them and
// if you desperately want to save some space. If you use any of these you must
// set wxUSE_CONTROLS as well.
//
// Default is 1
//
// Recommended setting: 1
#define wxUSE_ACTIVITYINDICATOR 1 // wxActivityIndicator
#define wxUSE_ANIMATIONCTRL 1   // wxAnimationCtrl
#define wxUSE_BANNERWINDOW  1   // wxBannerWindow
#define wxUSE_BUTTON        1   // wxButton
#define wxUSE_BMPBUTTON     1   // wxBitmapButton
#define wxUSE_CALENDARCTRL  1   // wxCalendarCtrl
#define wxUSE_CHECKBOX      1   // wxCheckBox
#define wxUSE_CHECKLISTBOX  1   // wxCheckListBox (requires wxUSE_OWNER_DRAWN)
#define wxUSE_CHOICE        1   // wxChoice
#define wxUSE_COLLPANE      1   // wxCollapsiblePane
#define wxUSE_COLOURPICKERCTRL 1    // wxColourPickerCtrl
#define wxUSE_COMBOBOX      1   // wxComboBox
#define wxUSE_COMMANDLINKBUTTON 1   // wxCommandLinkButton
#define wxUSE_DATAVIEWCTRL  1   // wxDataViewCtrl
#define wxUSE_DATEPICKCTRL  1   // wxDatePickerCtrl
#define wxUSE_DIRPICKERCTRL 1   // wxDirPickerCtrl
#define wxUSE_EDITABLELISTBOX 1 // wxEditableListBox
#define wxUSE_FILECTRL      1   // wxFileCtrl
#define wxUSE_FILEPICKERCTRL 1  // wxFilePickerCtrl
#define wxUSE_FONTPICKERCTRL 1  // wxFontPickerCtrl
#define wxUSE_GAUGE         1   // wxGauge
#define wxUSE_HEADERCTRL    1   // wxHeaderCtrl
#define wxUSE_HYPERLINKCTRL 1   // wxHyperlinkCtrl
#define wxUSE_LISTBOX       1   // wxListBox
#define wxUSE_LISTCTRL      1   // wxListCtrl
#define wxUSE_RADIOBOX      1   // wxRadioBox
#define wxUSE_RADIOBTN      1   // wxRadioButton
#define wxUSE_RICHMSGDLG    1   // wxRichMessageDialog
#define wxUSE_SCROLLBAR     1   // wxScrollBar
#define wxUSE_SEARCHCTRL    1   // wxSearchCtrl
#define wxUSE_SLIDER        1   // wxSlider
#define wxUSE_SPINBTN       1   // wxSpinButton
#define wxUSE_SPINCTRL      1   // wxSpinCtrl
#define wxUSE_STATBOX       1   // wxStaticBox
#define wxUSE_STATLINE      1   // wxStaticLine
#define wxUSE_STATTEXT      1   // wxStaticText
#define wxUSE_STATBMP       1   // wxStaticBitmap
#define wxUSE_TEXTCTRL      1   // wxTextCtrl
#define wxUSE_TIMEPICKCTRL  1   // wxTimePickerCtrl
#define wxUSE_TOGGLEBTN     1   // requires wxButton
#define wxUSE_TREECTRL      1   // wxTreeCtrl
#define wxUSE_TREELISTCTRL  1   // wxTreeListCtrl

// Use a status bar class? Depending on the value of wxUSE_NATIVE_STATUSBAR
// below either wxStatusBar95 or a generic wxStatusBar will be used.
//
// Default is 1
//
// Recommended setting: 1
#define wxUSE_STATUSBAR    1

// Two status bar implementations are available under Win32: the generic one
// or the wrapper around native control. For native look and feel the native
// version should be used.
//
// Default is 1 for the platforms where native status bar is supported.
//
// Recommended setting: 1 (there is no advantage in using the generic one)
#define wxUSE_NATIVE_STATUSBAR        1

// wxToolBar related settings: if wxUSE_TOOLBAR is 0, don't compile any toolbar
// classes at all. Otherwise, use the native toolbar class unless
// wxUSE_TOOLBAR_NATIVE is 0.
//
// Default is 1 for all settings.
//
// Recommended setting: 1 for wxUSE_TOOLBAR and wxUSE_TOOLBAR_NATIVE.
#define wxUSE_TOOLBAR 1
#define wxUSE_TOOLBAR_NATIVE 1

// wxNotebook is a control with several "tabs" located on one of its sides. It
// may be used to logically organise the data presented to the user instead of
// putting everything in one huge dialog. It replaces wxTabControl and related
// classes of wxWin 1.6x.
//
// Default is 1.
//
// Recommended setting: 1
#define wxUSE_NOTEBOOK 1

// wxListbook control is similar to wxNotebook but uses wxListCtrl instead of
// the tabs
//
// Default is 1.
//
// Recommended setting: 1
#define wxUSE_LISTBOOK 1

// wxChoicebook control is similar to wxNotebook but uses wxChoice instead of
// the tabs
//
// Default is 1.
//
// Recommended setting: 1
#define wxUSE_CHOICEBOOK 1

// wxTreebook control is similar to wxNotebook but uses wxTreeCtrl instead of
// the tabs
//
// Default is 1.
//
// Recommended setting: 1
#define wxUSE_TREEBOOK 1

// wxToolbook control is similar to wxNotebook but uses wxToolBar instead of
// tabs
//
// Default is 1.
//
// Recommended setting: 1
#define wxUSE_TOOLBOOK 1

// wxTaskBarIcon is a small notification icon shown in the system toolbar or
// dock.
//
// Default is 1.
//
// Recommended setting: 1 (but can be set to 0 if you don't need it)
#define wxUSE_TASKBARICON 1

// wxGrid class
//
// Default is 1, set to 0 to cut down compilation time and binaries size if you
// don't use it.
//
// Recommended setting: 1
//
#define wxUSE_GRID         1

// wxMiniFrame class: a frame with narrow title bar
//
// Default is 1.
//
// Recommended setting: 1 (it doesn't cost almost anything)
#define wxUSE_MINIFRAME 1

// wxComboCtrl and related classes: combobox with custom popup window and
// not necessarily a listbox.
//
// Default is 1.
//
// Recommended setting: 1 but can be safely set to 0 except for wxUniv where it
//                      it used by wxComboBox
#define wxUSE_COMBOCTRL 1

// wxOwnerDrawnComboBox is a custom combobox allowing to paint the combobox
// items.
//
// Default is 1.
//
// Recommended setting: 1 but can be safely set to 0, except where it is
//                      needed as a base class for generic wxBitmapComboBox.
#define wxUSE_ODCOMBOBOX 1

// wxBitmapComboBox is a combobox that can have images in front of text items.
//
// Default is 1.
//
// Recommended setting: 1 but can be safely set to 0
#define wxUSE_BITMAPCOMBOBOX 1

// wxRearrangeCtrl is a wxCheckListBox with two buttons allowing to move items
// up and down in it. It is also used as part of wxRearrangeDialog.
//
// Default is 1.
//
// Recommended setting: 1 but can be safely set to 0 (currently used only by
// wxHeaderCtrl)
#define wxUSE_REARRANGECTRL 1

// wxAddRemoveCtrl is a composite control containing a control showing some
// items (e.g. wxListBox, wxListCtrl, wxTreeCtrl, wxDataViewCtrl, ...) and "+"/
// "-" buttons allowing to add and remove items to/from the control.
//
// Default is 1.
//
// Recommended setting: 1 but can be safely set to 0 if you don't need it (not
// used by the library itself).
#define wxUSE_ADDREMOVECTRL 1

// ----------------------------------------------------------------------------
// Miscellaneous GUI stuff
// ----------------------------------------------------------------------------

// wxAcceleratorTable/Entry classes and support for them in wxMenu(Bar)
#define wxUSE_ACCEL 1

// Use the standard art provider. The icons returned by this provider are
// embedded into the library as XPMs so disabling it reduces the library size
// somewhat but this should only be done if you use your own custom art
// provider returning the icons or never use any icons not provided by the
// native art provider (which might not be implemented at all for some
// platforms) or by the Tango icons provider (if it's not itself disabled
// below).
//
// Default is 1.
//
// Recommended setting: 1 unless you use your own custom art provider.
#define wxUSE_ARTPROVIDER_STD 1

// Use art provider providing Tango icons: this art provider has higher quality
// icons than the default ones using smaller size XPM icons without
// transparency but the embedded PNG icons add to the library size.
//
// Default is 1 under non-GTK ports. Under wxGTK the native art provider using
// the GTK+ stock icons replaces it so it is normally not necessary.
//
// Recommended setting: 1 but can be turned off to reduce the library size.
#define wxUSE_ARTPROVIDER_TANGO 1

// Hotkey support (currently Windows only)
#define wxUSE_HOTKEY 1

// Use wxCaret: a class implementing a "cursor" in a text control (called caret
// under Windows).
//
// Default is 1.
//
// Recommended setting: 1 (can be safely set to 0, not used by the library)
#define wxUSE_CARET         1

// Use wxDisplay class: it allows enumerating all displays on a system and
// their geometries as well as finding the display on which the given point or
// window lies.
//
// Default is 1.
//
// Recommended setting: 1 if you need it, can be safely set to 0 otherwise
#define wxUSE_DISPLAY       1

// Miscellaneous geometry code: needed for Canvas library
#define wxUSE_GEOMETRY            1

// Use wxImageList. This class is needed by wxNotebook, wxTreeCtrl and
// wxListCtrl.
//
// Default is 1.
//
// Recommended setting: 1 (set it to 0 if you don't use any of the controls
// enumerated above, then this class is mostly useless too)
#define wxUSE_IMAGLIST      1

// Use wxInfoBar class.
//
// Default is 1.
//
// Recommended setting: 1 (but can be disabled without problems as nothing
// depends on it)
#define wxUSE_INFOBAR       1

// Use wxMenu, wxMenuBar, wxMenuItem.
//
// Default is 1.
//
// Recommended setting: 1 (can't be disabled under MSW)
#define wxUSE_MENUS         1

// Use wxNotificationMessage.
//
// wxNotificationMessage allows to show non-intrusive messages to the user
// using balloons, banners, popups or whatever is the appropriate method for
// the current platform.
//
// Default is 1.
//
// Recommended setting: 1
#define wxUSE_NOTIFICATION_MESSAGE 1

// wxPreferencesEditor provides a common API for different ways of presenting
// the standard "Preferences" or "Properties" dialog under different platforms
// (e.g. some use modal dialogs, some use modeless ones; some apply the changes
// immediately while others require an explicit "Apply" button).
//
// Default is 1.
//
// Recommended setting: 1 (but can be safely disabled if you don't use it)
#define wxUSE_PREFERENCES_EDITOR 1

// wxFont::AddPrivateFont() allows to use fonts not installed on the system by
// loading them from font files during run-time.
//
// Default is 1 except under Unix where it will be turned off by configure if
// the required libraries are not available or not new enough.
//
// Recommended setting: 1 (but can be safely disabled if you don't use it and
// want to avoid extra dependencies under Linux, for example).
#define wxUSE_PRIVATE_FONTS 1

// wxRichToolTip is a customizable tooltip class which has more functionality
// than the stock (but native, unlike this class) wxToolTip.
//
// Default is 1.
//
// Recommended setting: 1 (but can be safely set to 0 if you don't need it)
#define wxUSE_RICHTOOLTIP 1

// Use wxSashWindow class.
//
// Default is 1.
//
// Recommended setting: 1
#define wxUSE_SASH          1

// Use wxSplitterWindow class.
//
// Default is 1.
//
// Recommended setting: 1
#define wxUSE_SPLITTER      1

// Use wxToolTip and wxWindow::Set/GetToolTip() methods.
//
// Default is 1.
//
// Recommended setting: 1
#define wxUSE_TOOLTIPS      1

// wxValidator class and related methods
#define wxUSE_VALIDATORS 1

// Use reference counted ID management: this means that wxWidgets will track
// the automatically allocated ids (those used when you use wxID_ANY when
// creating a window, menu or toolbar item &c) instead of just supposing that
// the program never runs out of them. This is mostly useful only under wxMSW
// where the total ids range is limited to SHRT_MIN..SHRT_MAX and where
// long-running programs can run into problems with ids reuse without this. On
// the other platforms, where the ids have the full int range, this shouldn't
// be necessary.
#ifdef __WXMSW__
#define wxUSE_AUTOID_MANAGEMENT 1
#else
#define wxUSE_AUTOID_MANAGEMENT 0
#endif

// ----------------------------------------------------------------------------
// common dialogs
// ----------------------------------------------------------------------------

// On rare occasions (e.g. using DJGPP) may want to omit common dialogs (e.g.
// file selector, printer dialog). Switching this off also switches off the
// printing architecture and interactive wxPrinterDC.
//
// Default is 1
//
// Recommended setting: 1 (unless it really doesn't work)
#define wxUSE_COMMON_DIALOGS 1

// wxBusyInfo displays window with message when app is busy. Works in same way
// as wxBusyCursor
#define wxUSE_BUSYINFO      1

// Use single/multiple choice dialogs.
//
// Default is 1
//
// Recommended setting: 1 (used in the library itself)
#define wxUSE_CHOICEDLG     1

// Use colour picker dialog
//
// Default is 1
//
// Recommended setting: 1
#define wxUSE_COLOURDLG     1

// wxDirDlg class for getting a directory name from user
#define wxUSE_DIRDLG 1

// TODO: setting to choose the generic or native one

// Use file open/save dialogs.
//
// Default is 1
//
// Recommended setting: 1 (used in many places in the library itself)
#define wxUSE_FILEDLG       1

// Use find/replace dialogs.
//
// Default is 1
//
// Recommended setting: 1 (but may be safely set to 0)
#define wxUSE_FINDREPLDLG       1

// Use font picker dialog
//
// Default is 1
//
// Recommended setting: 1 (used in the library itself)
#define wxUSE_FONTDLG       1

// Use wxMessageDialog and wxMessageBox.
//
// Default is 1
//
// Recommended setting: 1 (used in the library itself)
#define wxUSE_MSGDLG        1

// progress dialog class for lengthy operations
#define wxUSE_PROGRESSDLG 1

// Set to 0 to disable the use of the native progress dialog (currently only
// available under MSW and suffering from some bugs there, hence this option).
#define wxUSE_NATIVE_PROGRESSDLG 1

// support for startup tips (wxShowTip &c)
#define wxUSE_STARTUP_TIPS 1

// text entry dialog and wxGetTextFromUser function
#define wxUSE_TEXTDLG 1

// number entry dialog
#define wxUSE_NUMBERDLG 1

// splash screen class
#define wxUSE_SPLASH 1

// wizards
#define wxUSE_WIZARDDLG 1

// Compile in wxAboutBox() function showing the standard "About" dialog.
//
// Default is 1
//
// Recommended setting: 1 but can be set to 0 to save some space if you don't
//                      use this function
#define wxUSE_ABOUTDLG 1

// wxFileHistory class
//
// Default is 1
//
// Recommended setting: 1
#define wxUSE_FILE_HISTORY 1

// ----------------------------------------------------------------------------
// Metafiles support
// ----------------------------------------------------------------------------

// Windows supports the graphics format known as metafile which, though not
// portable, is widely used under Windows and so is supported by wxWidgets
// (under Windows only, of course). Both the so-called "Window MetaFiles" or
// WMFs, and "Enhanced MetaFiles" or EMFs are supported in wxWin and, by
// default, EMFs will be used. This may be changed by setting
// wxUSE_WIN_METAFILES_ALWAYS to 1 and/or setting wxUSE_ENH_METAFILE to 0.
// You may also set wxUSE_METAFILE to 0 to not compile in any metafile
// related classes at all.
//
// Default is 1 for wxUSE_ENH_METAFILE and 0 for wxUSE_WIN_METAFILES_ALWAYS.
//
// Recommended setting: default or 0 for everything for portable programs.
#define wxUSE_METAFILE              1
#define wxUSE_ENH_METAFILE          1
#define wxUSE_WIN_METAFILES_ALWAYS  0

// ----------------------------------------------------------------------------
// Big GUI components
// ----------------------------------------------------------------------------

// Set to 0 to disable MDI support.
//
// Requires wxUSE_NOTEBOOK under platforms other than MSW.
//
// Default is 1.
//
// Recommended setting: 1, can be safely set to 0.
#define wxUSE_MDI 1

// Set to 0 to disable document/view architecture
#define wxUSE_DOC_VIEW_ARCHITECTURE 1

// Set to 0 to disable MDI document/view architecture
//
// Requires wxUSE_MDI && wxUSE_DOC_VIEW_ARCHITECTURE
#define wxUSE_MDI_ARCHITECTURE    1

// Set to 0 to disable print/preview architecture code
#define wxUSE_PRINTING_ARCHITECTURE  1

// wxHTML sublibrary allows to display HTML in wxWindow programs and much,
// much more.
//
// Default is 1.
//
// Recommended setting: 1 (wxHTML is great!), set to 0 if you want compile a
// smaller library.
#define wxUSE_HTML          1

// Setting wxUSE_GLCANVAS to 1 enables OpenGL support. You need to have OpenGL
// headers and libraries to be able to compile the library with wxUSE_GLCANVAS
// set to 1 and, under Windows, also to add opengl32.lib and glu32.lib to the
// list of libraries used to link your application (although this is done
// implicitly for Microsoft Visual C++ users).
//
// Default is 1.
//
// Recommended setting: 1 if you intend to use OpenGL, can be safely set to 0
// otherwise.
#define wxUSE_GLCANVAS       1

// wxRichTextCtrl allows editing of styled text.
//
// Default is 1.
//
// Recommended setting: 1, set to 0 if you want compile a
// smaller library.
#define wxUSE_RICHTEXT       1

// ----------------------------------------------------------------------------
// Data transfer
// ----------------------------------------------------------------------------

// Use wxClipboard class for clipboard copy/paste.
//
// Default is 1.
//
// Recommended setting: 1
#define wxUSE_CLIPBOARD     1

// Use wxDataObject and related classes. Needed for clipboard and OLE drag and
// drop
//
// Default is 1.
//
// Recommended setting: 1
#define wxUSE_DATAOBJ       1

// Use wxDropTarget and wxDropSource classes for drag and drop (this is
// different from "built in" drag and drop in wxTreeCtrl which is always
// available). Requires wxUSE_DATAOBJ.
//
// Default is 1.
//
// Recommended setting: 1
#define wxUSE_DRAG_AND_DROP 1

// Use wxAccessible for enhanced and customisable accessibility.
// Depends on wxUSE_OLE on MSW.
//
// Default is 1 on MSW, 0 elsewhere.
//
// Recommended setting (at present): 1 (MSW-only)
#ifdef __WXMSW__
#define wxUSE_ACCESSIBILITY 1
#else
#define wxUSE_ACCESSIBILITY 0
#endif

// ----------------------------------------------------------------------------
// miscellaneous settings
// ----------------------------------------------------------------------------

// wxSingleInstanceChecker class allows to verify at startup if another program
// instance is running.
//
// Default is 1
//
// Recommended setting: 1 (the class is tiny, disabling it won't save much
// space)
#define wxUSE_SNGLINST_CHECKER  1

#define wxUSE_DRAGIMAGE 1

#define wxUSE_IPC         1
                                // 0 for no interprocess comms
#define wxUSE_HELP        1
                                // 0 for no help facility

// Should we use MS HTML help for wxHelpController? If disabled, neither
// wxCHMHelpController nor wxBestHelpController are available.
//
// Default is 1 under MSW, 0 is always used for the other platforms.
//
// Recommended setting: 1, only set to 0 if you have trouble compiling
// wxCHMHelpController (could be a problem with really ancient compilers)
#define wxUSE_MS_HTML_HELP 1


// Use wxHTML-based help controller?
#define wxUSE_WXHTML_HELP 1

#define wxUSE_CONSTRAINTS 1
                                // 0 for no window layout constraint system

#define wxUSE_SPLINES     1
                                // 0 for no splines

#define wxUSE_MOUSEWHEEL        1
                                // Include mouse wheel support

// Compile wxUIActionSimulator class?
#define wxUSE_UIACTIONSIMULATOR 1

// ----------------------------------------------------------------------------
// wxDC classes for various output formats
// ----------------------------------------------------------------------------

// Set to 1 for PostScript device context.
#define wxUSE_POSTSCRIPT  0

// Set to 1 to use font metric files in GetTextExtent
#define wxUSE_AFM_FOR_POSTSCRIPT 1

// Set to 1 to compile in support for wxSVGFileDC, a wxDC subclass which allows
// to create files in SVG (Scalable Vector Graphics) format.
#define wxUSE_SVG 1

// Should wxDC provide SetTransformMatrix() and related methods?
//
// Default is 1 but can be set to 0 if this functionality is not used. Notice
// that currently wxMSW, wxGTK3 support this for wxDC and all platforms support
// this for wxGCDC so setting this to 0 doesn't change much if neither of these
// is used (although it will still save a few bytes probably).
//
// Recommended setting: 1.
#define wxUSE_DC_TRANSFORM_MATRIX 1

// ----------------------------------------------------------------------------
// image format support
// ----------------------------------------------------------------------------

// wxImage supports many different image formats which can be configured at
// compile-time. BMP is always supported, others are optional and can be safely
// disabled if you don't plan to use images in such format sometimes saving
// substantial amount of code in the final library.
//
// Some formats require an extra library which is included in wxWin sources
// which is mentioned if it is the case.

// Set to 1 for wxImage support (recommended).
#define wxUSE_IMAGE         1

// Set to 1 for PNG format support (requires libpng). Also requires wxUSE_ZLIB.
#define wxUSE_LIBPNG        1

// Set to 1 for JPEG format support (requires libjpeg)
#define wxUSE_LIBJPEG       1

// Set to 1 for TIFF format support (requires libtiff)
#define wxUSE_LIBTIFF       1

// Set to 1 for TGA format support (loading only)
#define wxUSE_TGA           1

// Set to 1 for GIF format support
#define wxUSE_GIF           1

// Set to 1 for PNM format support
#define wxUSE_PNM           1

// Set to 1 for PCX format support
#define wxUSE_PCX           1

// Set to 1 for IFF format support (Amiga format)
#define wxUSE_IFF           0

// Set to 1 for XPM format support
#define wxUSE_XPM           1

// Set to 1 for MS Icons and Cursors format support
#define wxUSE_ICO_CUR       1

// Set to 1 to compile in wxPalette class
#define wxUSE_PALETTE       1

// ----------------------------------------------------------------------------
// wxUniversal-only options
// ----------------------------------------------------------------------------

// Set to 1 to enable compilation of all themes, this is the default
#define wxUSE_ALL_THEMES    1

// Set to 1 to enable the compilation of individual theme if wxUSE_ALL_THEMES
// is unset, if it is set these options are not used; notice that metal theme
// uses Win32 one
#define wxUSE_THEME_GTK     0
#define wxUSE_THEME_METAL   0
#define wxUSE_THEME_MONO    0
#define wxUSE_THEME_WIN32   0


/* --- end common options --- */

/* --- start MSW options --- */
// ----------------------------------------------------------------------------
// Windows-only settings
// ----------------------------------------------------------------------------

// Set this to 1 for generic OLE support: this is required for drag-and-drop,
// clipboard, OLE Automation. Only set it to 0 if your compiler is very old and
// can't compile/doesn't have the OLE headers.
//
// Default is 1.
//
// Recommended setting: 1
#define wxUSE_OLE           1

// Set this to 1 to enable wxAutomationObject class.
//
// Default is 1.
//
// Recommended setting: 1 if you need to control other applications via OLE
// Automation, can be safely set to 0 otherwise
#define wxUSE_OLE_AUTOMATION 1

// Set this to 1 to enable wxActiveXContainer class allowing to embed OLE
// controls in wx.
//
// Default is 1.
//
// Recommended setting: 1, required by wxMediaCtrl
#define wxUSE_ACTIVEX 1

// wxDC caching implementation
#define wxUSE_DC_CACHEING 1

// Set this to 1 to enable wxDIB class used internally for manipulating
// wxBitmap data.
//
// Default is 1, set it to 0 only if you don't use wxImage neither
//
// Recommended setting: 1 (without it conversion to/from wxImage won't work)
#define wxUSE_WXDIB 1

// Set to 0 to disable PostScript print/preview architecture code under Windows
// (just use Windows printing).
#define wxUSE_POSTSCRIPT_ARCHITECTURE_IN_MSW 1

// Set this to 1 to compile in wxRegKey class.
//
// Default is 1
//
// Recommended setting: 1, this is used internally by wx in a few places
#define wxUSE_REGKEY 1

// Set this to 1 to use RICHEDIT controls for wxTextCtrl with style wxTE_RICH
// which allows to put more than ~32Kb of text in it even under Win9x (NT
// doesn't have such limitation).
//
// Default is 1 for compilers which support it
//
// Recommended setting: 1, only set it to 0 if your compiler doesn't have
//                      or can't compile <richedit.h>
#define wxUSE_RICHEDIT  1

// Set this to 1 to use extra features of richedit v2 and later controls
//
// Default is 1 for compilers which support it
//
// Recommended setting: 1
#define wxUSE_RICHEDIT2 1

// Set this to 1 to enable support for the owner-drawn menu and listboxes. This
// is required by wxUSE_CHECKLISTBOX.
//
// Default is 1.
//
// Recommended setting: 1, set to 0 for a small library size reduction
#define wxUSE_OWNER_DRAWN 1

// Set this to 1 to enable MSW-specific wxTaskBarIcon::ShowBalloon() method. It
// is required by native wxNotificationMessage implementation.
//
// Default is 1 but disabled in wx/msw/chkconf.h if SDK is too old to contain
// the necessary declarations.
//
// Recommended setting: 1, set to 0 for a tiny library size reduction
#define wxUSE_TASKBARICON_BALLOONS 1

// Set to 1 to compile MS Windows XP theme engine support
#define wxUSE_UXTHEME           1

// Set to 1 to use InkEdit control (Tablet PC), if available
#define wxUSE_INKEDIT  0

// Set to 1 to enable .INI files based wxConfig implementation (wxIniConfig)
//
// Default is 0.
//
// Recommended setting: 0, nobody uses .INI files any more
#define wxUSE_INICONF 0

// Set to 1 if you need to include <winsock2.h> over <winsock.h>
//
// Default is 0.
//
// Recommended setting: 0
#define wxUSE_WINSOCK2 0

// ----------------------------------------------------------------------------
// Generic versions of native controls
// ----------------------------------------------------------------------------

// Set this to 1 to be able to use wxDatePickerCtrlGeneric in addition to the
// native wxDatePickerCtrl
//
// Default is 0.
//
// Recommended setting: 0, this is mainly used for testing
#define wxUSE_DATEPICKCTRL_GENERIC 0

// ----------------------------------------------------------------------------
// Crash debugging helpers
// ----------------------------------------------------------------------------

// Set this to 1 to be able to use wxCrashReport::Generate() to create mini
// dumps of your program when it crashes (or at any other moment)
//
// Default is 1 if supported by the compiler (VC++ and recent BC++ only).
//
// Recommended setting: 1, set to 0 if your programs never crash
#define wxUSE_CRASHREPORT 1
/* --- end MSW options --- */

/* --- start wxUniv options --- */
// ----------------------------------------------------------------------------
// wxUniversal-only options
// ----------------------------------------------------------------------------

// Set to 1 to enable compilation of all themes, this is the default
#define wxUSE_ALL_THEMES    1

// Set to 1 to enable the compilation of individual theme if wxUSE_ALL_THEMES
// is unset, if it is set these options are not used; notice that metal theme
// uses Win32 one
#define wxUSE_THEME_GTK     0
#define wxUSE_THEME_METAL   0
#define wxUSE_THEME_MONO    0
#define wxUSE_THEME_WIN32   0
/* --- end wxUniv options --- */

#endif // _WX_SETUP_H_
