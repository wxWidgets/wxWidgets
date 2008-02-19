/////////////////////////////////////////////////////////////////////////////
// Name:        const_cpp.h
// Purpose:     Preprocessor symbols
// Author:      Vadim Zeitlin
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////


/*!

 @page cppconst Preprocessor symbols defined by wxWidgets

 These are preprocessor symbols used in the wxWidgets source, grouped
 by category (and sorted by alphabetical order inside each category).
 All of these macros except for the @c wxUSE_XXX variety is defined if the
 corresponding condition is @true and undefined if it isn't, so they should be
 always tested using @ifdef and not @if.

 TODO: what can we use here instead of \twocolitem to keep text readable??

 @li @ref guisystemconst
 @li @ref osconst
 @li @ref cpuconst
 @li @ref hardwareconst
 @li @ref compilerconst
 @li @ref featuretests
 @li @ref miscellaneousconst

 <hr>


 @section guisystemconst GUI system
 
 \twocolitem{__WINDOWS__}{any Windows, you may also use __WXMSW__}
 \twocolitem{__WIN16__}{Win16 API (not supported since wxWidgets 2.6)}
 \twocolitem{__WIN32__}{Win32 API}
 \twocolitem{__WXBASE__}{Only wxBase, no GUI features (same as @c wxUSE_GUI} $== 0$)}
 \twocolitem{__WXCOCOA__}{OS X using Cocoa API}
 \twocolitem{__WXDFB__}{wxUniversal using DirectFB}
 \twocolitem{__WXWINCE__}{Windows CE}
 \twocolitem{__WXGTK__}{GTK+}
 \twocolitem{__WXGTK12__}{GTK+ 1.2 or higher}
 \twocolitem{__WXGTK20__}{GTK+ 2.0 or higher}
 \twocolitem{__WXGTK24__}{GTK+ 2.4 or higher}
 \twocolitem{__WXGTK26__}{GTK+ 2.6 or higher}
 \twocolitem{__WXGTK210__}{GTK+ 2.10 or higher}
 \twocolitem{__WXMOTIF__}{Motif}
 \twocolitem{__WXMOTIF20__}{Motif 2.0 or higher}
 \twocolitem{__WXMAC__}{Mac OS all targets}
 \twocolitem{__WXMAC_CLASSIC__}{MacOS for Classic}
 \twocolitem{__WXMAC_CARBON__}{MacOS for Carbon CFM (running under Classic or OSX) or true OS X Mach-O Builds}
 \twocolitem{__WXMAC_OSX__}{MacOS X Carbon Mach-O Builds}
 \twocolitem{__WXMGL__}{SciTech Soft MGL (__WXUNIVERSAL__ will be also
 defined)}
 \twocolitem{__WXMSW__}{Any Windows}
 \twocolitem{__WXOSX__}{Any Mac OS X port (either Carbon or Cocoa)}
 \twocolitem{__WXPALMOS__}{PalmOS}
 \twocolitem{__WXPM__}{OS/2 native Presentation Manager}
 \twocolitem{__WXSTUBS__}{Stubbed version ('template' wxWin implementation)}
 \twocolitem{__WXXT__}{Xt; mutually exclusive with WX_MOTIF, not
 implemented in wxWidgets 2.x}
 \twocolitem{__WXX11__}{wxX11 (__WXUNIVERSAL__ will be also defined)}
 \twocolitem{__WXWINE__}{WINE (i.e. WIN32 on Unix)}
 \twocolitem{__WXUNIVERSAL__}{wxUniversal port, always defined in addition
 to one of the symbols above so this should be tested first.}
 \twocolitem{__X__}{any X11-based GUI toolkit except GTK+}
 
 There are two wxWidgets ports to Mac OS. One of them, wxMac, exists in two versions: 
 Classic and Carbon. The Classic version is the only one to work on Mac OS version 8. 
 The Carbon version may be built either as CFM or Mach-O (binary format, like ELF)
 and the former may run under OS 9 while the latter only runs under OS X.
 Finally, there is a new Cocoa port which can only be used under OS X. To
 summarize:
 
 @li If you want to test for all Mac platforms, classic and OS X, you
     should test both @c __WXMAC__ and @c __WXCOCOA__.
 @li If you want to test for any GUI Mac port under OS X, use
     @c __WXOSX__.
 @li If you want to test for any port under Mac OS X, including, for
     example, wxGTK and also wxBase, use @c __DARWIN__ (see below).
 
 The convention is to use the @c __WX prefix for these
 symbols, although this has not always been followed.
 
 @section osconst Operating systems
 
 \twocolitem{__APPLE__}{any Mac OS version}
 \twocolitem{__AIX__}{AIX}
 \twocolitem{__BSD__}{Any *BSD system}
 \twocolitem{__CYGWIN__}{Cygwin: Unix on Win32}
 \twocolitem{__DARWIN__}{Mac OS X using the BSD Unix C library (as opposed to using the Metrowerks MSL C/C++ library)}
 \twocolitem{__DATA_GENERAL__}{DG-UX}
 \twocolitem{__DOS_GENERAL__}{DOS (used with wxMGL only)}
 \twocolitem{__FREEBSD__}{FreeBSD}
 \twocolitem{__HPUX__}{HP-UX (Unix)}
 \twocolitem{__GNU__}{GNU Hurd}
 \twocolitem{__LINUX__}{Linux}
 \twocolitem{__MACH__}{Mach-O Architecture (Mac OS X only builds)}
 \twocolitem{__OSF__}{OSF/1}
 \twocolitem{__PALMOS__}{PalmOS}
 \twocolitem{__SGI__}{IRIX}
 \twocolitem{__SOLARIS__}{Solaris}
 \twocolitem{__SUN__}{Any Sun}
 \twocolitem{__SUNOS__}{Sun OS}
 \twocolitem{__SVR4__}{SystemV R4}
 \twocolitem{__SYSV__}{SystemV generic}
 \twocolitem{__ULTRIX__}{Ultrix}
 \twocolitem{__UNIX__}{any Unix}
 \twocolitem{__UNIX_LIKE__}{Unix, BeOS or VMS}
 \twocolitem{__VMS__}{VMS}
 \twocolitem{__WINDOWS__}{any Windows}
 \twocolitem{__WINE__}{Wine}
 
 
 @section cpuconst Hardware architectures (CPU)
 
 Note that not all of these symbols are always defined, it depends on the
 compiler used.
 
 \twocolitem{__ALPHA__}{DEC Alpha architecture}
 \twocolitem{__INTEL__}{Intel i386 or compatible}
 \twocolitem{__IA64__}{Intel 64 bit architecture}
 \twocolitem{__POWERPC__}{Motorola Power PC}
 
 
 @section hardwareconst Hardware type
 
 \twocolitem{__SMARTPHONE__}{Generic mobile devices with phone buttons and a small display}
 \twocolitem{__PDA__}{Personal digital assistant, usually with touch screen}
 \twocolitem{__HANDHELD__}{Small but powerful computer, usually with a keyboard}
 \twocolitem{__POCKETPC__}{Microsoft-powered PocketPC devices with touch-screen}
 \twocolitem{__WINCE_STANDARDSDK__}{Microsoft-powered Windows CE devices, for generic Windows CE applications}
 \twocolitem{__WINCE_NET__}{Microsoft-powered Windows CE .NET devices (_WIN32_WCE is 400 or greater)}
 \twocolitem{WIN32_PLATFORM_WFSP}{Microsoft-powered smartphone}
 
 
 @section compilerconst Compilers
 
 \twocolitem{__BORLANDC__}{Borland C++. The value of the macro corresponds
 to the compiler version: $500$ is $5.0$.}
 \twocolitem{__DJGPP__}{DJGPP}
 \twocolitem{__DIGITALMARS__}{Digital Mars}
 \twocolitem{__GNUG__}{Gnu C++ on any platform, see also
 \helpref{wxCHECK_GCC_VERSION}{wxcheckgccversion}}
 \twocolitem{__GNUWIN32__}{Gnu-Win32 compiler, see also
 \helpref{wxCHECK_W32API_VERSION}{wxcheckw32apiversion}}
 \twocolitem{__MINGW32__}{MinGW}
 \twocolitem{__MWERKS__}{CodeWarrior MetroWerks compiler}
 \twocolitem{__SUNCC__}{Sun CC, see also \helpref{wxCHECK_SUNCC_VERSION}{wxchecksunccversion}}
 \twocolitem{__SYMANTECC__}{Symantec C++}
 \twocolitem{__VISAGECPP__}{IBM Visual Age (OS/2)}
 \twocolitem{__VISUALC__}{Microsoft Visual C++, see also
 \helpref{wxCHECK_VISUALC_VERSION}{wxcheckvisualcversion}. The value of this
 macro corresponds to the compiler version: $1020$ for $4.2$ (the first supported
 version), $1100$ for $5.0$, $1200$ for $6.0$ and so on. For convenience, the
 symbols __VISUALCn__ are also defined for each major compiler version from
 5 to 9, i.e. you can use tests such @ifdef __VISUALC7__} to test
 for compiler version being precisely 7.}
 \twocolitem{__XLC__}{AIX compiler}
 \twocolitem{__WATCOMC__}{Watcom C++. The value of this macro corresponds to
 the compiler version, $1100$ is $11.0$ and $1200$ is OpenWatcom.}
 \twocolitem{_WIN32_WCE}{Windows CE version}
 
 
 @section featuretests Feature tests
 
 Some library features may not be always available even if they were selected by
 the user. To make it possible to check if this is the case, the library
 predefines the symbols in the form @c wxHAS_FEATURE. Unlike 
 @c wxUSE_FEATURE symbols which are defined by the library user (directly
 in @c setup.h or by running configure script) and which must be always
 defined as either $0$ or $1$, the @c wxHAS symbols are only defined if
 the corresponding feature is available and not defined at all otherwise.
 
 Currently the following symbols exist:

 \twocolitem{wxHAS_LARGE_FILES}{Defined if \helpref{wxFile}{wxfile} supports
 files more than 4GB in size.}
 \twocolitem{wxHAS_LARGE_FFILES}{Defined if \helpref{wxFFile}{wxffile}
 supports files more than 4GB in size.}
 \twocolitem{wxHAS_POWER_EVENTS}{Defined if 
 \helpref{wxPowerEvent}{wxpowerevent} are ever generated on the current platform.}
 \twocolitem{wxHAS_RADIO_MENU_ITEMS}{Defined if the current port supports 
 \helpref{radio menu items}{wxmenuappendradioitem}.}
 \twocolitem{wxHAS_RAW_KEY_CODES}{Defined if 
 \helpref{raw key codes}{wxkeyeventgetrawkeycode} are supported.}
 \twocolitem{wxHAS_REGEX_ADVANCED}{Defined if advanced syntax is available in 
 \helpref{wxRegEx}{wxregex}.}
 \twocolitem{wxHAS_TASK_BAR_ICON}{Defined if 
 \helpref{wxTaskBarIcon}{wxtaskbaricon} is available on the current platform.}
 
 
 @section miscellaneousconst Miscellaneous
 
 \twocolitem{__WXWINDOWS__}{always defined in wxWidgets applications, see
 also \helpref{wxCHECK_VERSION}{wxcheckversion}}
 \twocolitem{__WXDEBUG__}{defined in debug mode, undefined in release mode}
 \twocolitem{wxUSE_XXX}{if defined as $1$, feature XXX is active, see the 
 \helpref{complete list}{wxusedef} (the symbols of this form are always defined,
 use \#if and not \#ifdef to test for them)}
 \twocolitem{WX_PRECOMP}{is defined if precompiled headers (PCH) are in use. In
 this case, @c wx/wxprec.h} includes @c wx/wx.h} which, in turn,
 includes a number of wxWidgets headers thus making it unnecessary to include
 them explicitly. However if this is not defined, you do need to include them
 and so the usual idiom which allows to support both cases is to first include
 @c wx/wxprec.h} and then, inside @ifndef WX_PRECOMP}, individual
 headers you need.}
 \twocolitem{_UNICODE and UNICODE}{both are defined if wxUSE_UNICODE is set to $1$}
 \twocolitem{wxUSE_GUI}{this particular feature test macro is defined to $1$
 when compiling or using the library with the GUI features activated, if it is
 defined as $0$, only wxBase is available.}
 \twocolitem{wxUSE_BASE}{only used by wxWidgets internally (defined as $1$ when
 building wxBase code, either as a standalone library or as part of the
 monolithic wxWidgets library, defined as $0$ when building GUI library only)}
 \twocolitem{wxNO_RTTI}{is defined if the compiler RTTI support has been switched off}
 \twocolitem{wxNO_EXCEPTIONS}{is defined if the compiler support for C++
 exceptions has been switched off}
 \twocolitem{wxNO_THREADS}{if this macro is defined, the compilation options
 don't include compiler flags needed for multithreaded code generation. This
 implies that wxUSE_THREADS is $0$ and also that other (non-wx-based) threading
 packages cannot be used neither.}
 \twocolitem{WXMAKINGDLL_XXX}{used internally and defined when building the
 library @c XXX} as a DLL; when a monolithic wxWidgets build is used only a
 single @c WXMAKINGDLL} symbol is defined}
 \twocolitem{WXUSINGDLL}{defined when compiling code which uses wxWidgets as a
 DLL/shared library}
 \twocolitem{WXBUILDING}{defined when building wxWidgets itself, whether as a
 static or shared library}

*/
