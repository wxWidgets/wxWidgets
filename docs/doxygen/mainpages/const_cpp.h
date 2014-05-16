/////////////////////////////////////////////////////////////////////////////
// Name:        const_cpp.h
// Purpose:     Preprocessor symbols
// Author:      Vadim Zeitlin
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////


/**

@page page_cppconst Preprocessor Symbols

@tableofcontents

These are preprocessor symbols used in the wxWidgets source, grouped by
category (and sorted by alphabetical order inside each category). All of these
macros except for the @c wxUSE_XXX variety is defined if the corresponding
condition is @true and undefined if it isn't, so they should be always tested
using @ifdef_ and not @if_.



@section page_cppconst_guisystem GUI system

@beginDefList
@itemdef{__WXBASE__, Only wxBase, no GUI features (same as @c wxUSE_GUI == 0)}
@itemdef{__WXCOCOA__, OS X using wxCocoa Port}
@itemdef{__WXDFB__, wxUniversal using DirectFB}
@itemdef{__WXWINCE__, Windows CE}
@itemdef{__WXGTK__, GTK+}
@itemdef{__WXGTK12__, GTK+ 1.2 or higher}
@itemdef{__WXGTK20__, GTK+ 2.0 or higher}
@itemdef{__WXGTK24__, GTK+ 2.4 or higher}
@itemdef{__WXGTK26__, GTK+ 2.6 or higher}
@itemdef{__WXGTK210__, GTK+ 2.10 or higher}
@itemdef{__WXMAC__, old define, same as @c __WXOSX__}
@itemdef{__WXMOTIF__, Motif}
@itemdef{__WXMOTIF20__, Motif 2.0 or higher}
@itemdef{__WXMSW__, GUI using <a href="http://en.wikipedia.org/wiki/Windows_User">Windows Controls</a>.
Notice that for compatibility reasons, this symbol is defined for console
applications under Windows as well, but it should only be used in the GUI code
while @c __WINDOWS__ should be used for the platform tests.}
@itemdef{__WXOSX__, OS X GUI using any Apple widget framework (Carbon, AppKit or UIKit)}
@itemdef{__WXOSX_IPHONE__, OS X iPhone (UIKit)}
@itemdef{__WXOSX_CARBON__, Mac OS X using Carbon}
@itemdef{__WXOSX_COCOA__, Mac OS X using Cocoa (AppKit)}
@itemdef{__WXOSX_MAC__, Mac OS X (Carbon or Cocoa)}
@itemdef{__WXOSX_OR_COCOA__, Any Mac OS X port (wxOSX-Carbon, wxOSX-Cocoa or 
                        wxCocoa)}
@itemdef{__WXPM__, OS/2 native Presentation Manager (not used any longer).}
@itemdef{__WXSTUBS__, Stubbed version ('template' wxWin implementation)}
@itemdef{__WXXT__, Xt; mutually exclusive with WX_MOTIF, not implemented in wxWidgets 2.x}
@itemdef{__WXX11__, wxX11 (__WXUNIVERSAL__ will be also defined)}
@itemdef{__WXWINE__, WINE (i.e. WIN32 on Unix)}
@itemdef{__WXUNIVERSAL__, wxUniversal port, always defined in addition
                        to one of the symbols above so this should be tested first.}
@itemdef{__X__, any X11-based GUI toolkit except GTK+}
@endDefList

There are two wxWidgets ports to Mac OS X. One of them, wxOSX is the successor of the
venerable wxMac, it currently exists in three versions: Carbon and Cocoa for the
desktop and a very early iPhone port. And there is the Cocoa port named wxCocoa 
which has not been updated very actively since beginning 2008. To summarize:

@li If you want to test for wxOSX on the desktop, use
    @c __WXOSX_MAC__.
@li If you want to test for wxOSX on the iPhone, use
    @c __WXOSX_IPHONE__.
@li If you want to test for any GUI Mac port under OS X, use
    @c __WXOSX_OR_COCOA__.
@li If you want to test for any port under Mac OS X, including, for
    example, wxGTK and also wxBase, use @c __DARWIN__ (see below).

The convention is to use the @c __WX prefix for these
symbols, although this has not always been followed.



@section page_cppconst_os Operating Systems

@beginDefList
@itemdef{__APPLE__, any Mac OS version}
@itemdef{__AIX__, AIX}
@itemdef{__BSD__, Any *BSD system}
@itemdef{__CYGWIN__, Cygwin: Unix on Win32}
@itemdef{__DARWIN__, Mac OS X (with BSD C library), using any port (see also @c __WXOSX__)}
@itemdef{__DATA_GENERAL__, DG-UX}
@itemdef{__FREEBSD__, FreeBSD}
@itemdef{__HPUX__, HP-UX (Unix)}
@itemdef{__GNU__, GNU Hurd}
@itemdef{__LINUX__, Linux}
@itemdef{__MACH__, Mach-O Architecture (Mac OS X only builds)}
@itemdef{__OSF__, OSF/1}
@itemdef{__QNX__, QNX Neutrino RTOS}
@itemdef{__SGI__, IRIX}
@itemdef{__SOLARIS__, Solaris}
@itemdef{__SUN__, Any Sun}
@itemdef{__SUNOS__, Sun OS}
@itemdef{__SVR4__, SystemV R4}
@itemdef{__SYSV__, SystemV generic}
@itemdef{__ULTRIX__, Ultrix}
@itemdef{__UNIX__, any Unix}
@itemdef{__UNIX_LIKE__, Unix, BeOS or VMS}
@itemdef{__VMS__, VMS}
@itemdef{__WINDOWS__, Any Windows platform, using any port (see also @c __WXMSW__)}
@itemdef{__WIN16__, Win16 API (not supported since wxWidgets 2.6)}
@itemdef{__WIN32__, Win32 API}
@itemdef{__WIN64__, Win64 (mostly same as Win32 but data type sizes are different)}
@itemdef{__WINE__, Wine}
@itemdef{_WIN32_WCE, Windows CE version}
@endDefList



@section page_cppconst_cpu Hardware Architectures (CPU)

Note that not all of these symbols are always defined, it depends on the
compiler used.

@beginDefList
@itemdef{__ALPHA__, DEC Alpha architecture}
@itemdef{__INTEL__, Intel i386 or compatible}
@itemdef{__IA64__, Intel 64 bit architecture}
@itemdef{__POWERPC__, Motorola Power PC}
@endDefList



@section page_cppconst_hardware Hardware Type

@beginDefList
@itemdef{__SMARTPHONE__, Generic mobile devices with phone buttons and a small display}
@itemdef{__PDA__, Personal digital assistant, usually with touch screen}
@itemdef{__HANDHELD__, Small but powerful computer, usually with a keyboard}
@itemdef{__POCKETPC__, Microsoft-powered PocketPC devices with touch-screen}
@itemdef{__WINCE_STANDARDSDK__, Microsoft-powered Windows CE devices, for generic Windows CE applications}
@itemdef{__WINCE_NET__, Microsoft-powered Windows CE .NET devices (_WIN32_WCE is 400 or greater)}
@itemdef{WIN32_PLATFORM_WFSP, Microsoft-powered smartphone}
@endDefList



@section page_cppconst_compiler Compilers

@beginDefList
@itemdef{__BORLANDC__, Borland C++. The value of the macro corresponds
                        to the compiler version: 500 is 5.0.}
@itemdef{__DJGPP__, DJGPP}
@itemdef{__DIGITALMARS__, Digital Mars (not used any more).}
@itemdef{__EVC4__, Embedded Visual C++ 4 (can be only used for building wxWinCE)}
@itemdef{__GNUG__, Gnu C++ on any platform, see also wxCHECK_GCC_VERSION}
@itemdef{__GNUWIN32__, Gnu-Win32 compiler, see also wxCHECK_W32API_VERSION}
@itemdef{__INTELC__, Intel C++ compiler}
@itemdef{__MINGW32__, Either MinGW32 or MinGW-w64 in either 32 or 64 bits}
@itemdef{__MINGW32_TOOLCHAIN, MinGW32 only (32 bits only right now)}
@itemdef{__MINGW64__, MinGW-w64 in 64 bit builds}
@itemdef{__MINGW64_TOOLCHAIN__, MinGW-w64 in either 32 or 64 bit builds}
@itemdef{__SUNCC__, Sun CC, see also wxCHECK_SUNCC_VERSION}
@itemdef{__SYMANTECC__, Symantec C++ (not used any more).}
@itemdef{__VISAGECPP__, IBM Visual Age (OS/2) (not used any more).}
@itemdef{__VISUALC__, Microsoft Visual C++, see also ::wxCHECK_VISUALC_VERSION.
                    The value of this macro corresponds to the compiler version:
                    @c 1020 for @c 4.2 (the first supported version), @c 1100 for
                    @c 5.0, @c 1200 for @c 6.0 and so on. For convenience, the symbols
                    __VISUALCn__ are also defined for each major compiler version from
                    5 to 12, i.e. you can use tests such @ifdef_ __VISUALC7__ to test
                    for compiler version being precisely 7.}
@itemdef{__XLC__, AIX compiler}
@itemdef{__WATCOMC__, Watcom C++. The value of this macro corresponds to
                    the compiler version, @c 1100 is @c 11.0 and @c 1200 is OpenWatcom
                    (not used any more).}
@endDefList



@section page_cppconst_featuretests Feature Tests

Some library features may not be always available even if they were selected
by the user. To make it possible to check if this is the case, the library
predefines the symbols in the form @c wxHAS_FEATURE. Unlike
@c wxUSE_FEATURE symbols which are defined by the library user (directly
in @c setup.h or by running configure script) and which must be always
defined as either 0 or 1, the @c wxHAS symbols are only defined if
the corresponding feature is available and not defined at all otherwise.

Currently the following symbols exist:

@beginDefList
@itemdef{wxHAS_3STATE_CHECKBOX, Defined if wxCheckBox supports wxCHK_3STATE
    flag, i.e. is capable of showing three states and not only the usual two.
    Currently defined for almost all ports.}
@itemdef{wxHAS_ATOMIC_OPS, Defined if wxAtomicInc() and wxAtomicDec() functions
    have an efficient (CPU-specific) implementation. Notice that the functions
    themselves are always available but can be prohibitively slow to use when
    implemented in a generic way, using a critical section.}
@itemdef{wxHAS_BITMAPTOGGLEBUTTON, Defined in @c wx/tglbtn.h if
    wxBitmapToggleButton class is available in addition to wxToggleButton.}
@itemdef{wxHAS_CONFIG_TEMPLATE_RW, Defined if the currently used compiler
    supports template Read() and Write() methods in wxConfig.}
@itemdef{wxHAS_LARGE_FILES, Defined if wxFile supports files more than 4GB in
    size (notice that you must include @c wx/filefn.h before testing for this
    symbol).}
@itemdef{wxHAS_LARGE_FFILES, Defined if wxFFile supports files more than 4GB in
    size (notice that you must include @c wx/filefn.h before testing for this
    symbol).}
@itemdef{wxHAS_LONG_LONG_T_DIFFERENT_FROM_LONG, Defined if compiler supports a
    64 bit integer type (available as @c wxLongLong_t) and this type is
    different from long. Notice that, provided wxUSE_LONGLONG is not turned
    off, some 64 bit type is always available to wxWidgets programs and this
    symbol only indicates a presence of such primitive type. It is useful to
    decide whether some function should be overloaded for both
    <code>long</code> and <code>long long</code> types.}
@itemdef{wxHAS_MULTIPLE_FILEDLG_FILTERS, Defined if wxFileDialog supports multiple ('|'-separated) filters.}
@itemdef{wxHAS_IMAGES_IN_RESOURCES, Defined if <a href="http://en.wikipedia.org/wiki/Resource_(Windows)">
    Windows resource files</a> or OS/2 resource files are available on the current platform.}
@itemdef{wxHAS_POWER_EVENTS, Defined if wxPowerEvent are ever generated on the current platform.}
@itemdef{wxHAS_RADIO_MENU_ITEMS,
        Defined if the current port supports radio menu items (see wxMenu::AppendRadioItem).}
@itemdef{wxHAS_RAW_BITMAP, Defined if direct access to bitmap data using the classes in @c wx/rawbmp.h is supported.}
@itemdef{wxHAS_RAW_KEY_CODES, Defined if raw key codes (see wxKeyEvent::GetRawKeyCode are supported.}
@itemdef{wxHAS_REGEX_ADVANCED, Defined if advanced syntax is available in wxRegEx.}
@itemdef{wxHAS_TASK_BAR_ICON, Defined if wxTaskBarIcon is available on the current platform.}
@endDefList



@section page_cppconst_msvc_setup_h Library Selection for MSVC

Microsoft Visual C++ users may use the special @c wx/setup.h file for this
compiler in @c include/msvc subdirectory. This file implicitly links in all the
wxWidgets libraries using MSVC-specific pragmas which usually is much more
convenient than manually specifying the libraries list in all of the project
configurations. However sometimes linking with all the libraries is not
desirable, for example because some of them were not built and this is where
the symbols in this section can be helpful: defining them allows to not link
with the corresponding library. The following symbols are honoured:

    - wxNO_ADV_LIB
    - wxNO_AUI_LIB
    - wxNO_HTML_LIB
    - wxNO_MEDIA_LIB
    - wxNO_NET_LIB
    - wxNO_PROPGRID_LIB
    - wxNO_QA_LIB
    - wxNO_RICHTEXT_LIB
    - wxNO_WEBVIEW_LIB
    - wxNO_XML_LIB
    - wxNO_REGEX_LIB
    - wxNO_EXPAT_LIB
    - wxNO_JPEG_LIB
    - wxNO_PNG_LIB
    - wxNO_TIFF_LIB
    - wxNO_ZLIB_LIB

Notice that the base library is always included and the core is always included
for the GUI applications (i.e. those which don't define @c wxUSE_GUI as 0).

If the makefiles have been used to build the libraries from source and the @c CFG
variable has been set to specify a different output path for that particular
configuration of build then the @c wxCFG preprocessor symbol should be set in
the project that uses wxWidgets to the same value as the @c CFG variable in
order for the correct @c wx/setup.h file to automatically be included for that
configuration.

@section page_cppconst_miscellaneous Miscellaneous

@beginDefList
@itemdef{__WXWINDOWS__,
        always defined in wxWidgets applications, see also wxCHECK_VERSION}
@itemdef{wxDEBUG_LEVEL, defined as 1 by default, may be pre-defined as 0 before
        including wxWidgets headers to disable generation of any code at all
        for the assertion macros, see @ref overview_debugging}
@itemdef{__WXDEBUG__, defined if wxDEBUG_LEVEL is 1 or more, undefined otherwise}
@itemdef{wxUSE_XXX,
        if defined as 1, feature XXX is active, see the
        @ref page_wxusedef (the symbols of this form are always defined,
        use @if_ and not @ifdef_ to test for them)}
@itemdef{WX_PRECOMP,
        is defined if precompiled headers (PCH) are in use. In
        this case, @c wx/wxprec.h includes @c wx/wx.h which, in turn,
        includes a number of wxWidgets headers thus making it unnecessary to include
        them explicitly. However if this is not defined, you do need to include them
        and so the usual idiom which allows to support both cases is to first include
        @c wx/wxprec.h} and then, inside @ifndef_ WX_PRECOMP, individual
        headers you need.}
@itemdef{_UNICODE and UNICODE, both are defined if wxUSE_UNICODE is set to @c 1}
@itemdef{wxUSE_GUI,
        this particular feature test macro is defined to 1
        when compiling or using the library with the GUI features activated,
        if it is defined as @c 0, only wxBase is available.}
@itemdef{wxUSE_BASE,
        only used by wxWidgets internally (defined as 1 when
        building wxBase code, either as a standalone library or as part of the
        monolithic wxWidgets library, defined as 0 when building GUI library only)}
@itemdef{wxNO_RTTI, is defined if the compiler RTTI support has been switched off}
@itemdef{wxNO_EXCEPTIONS,
        is defined if the compiler support for C++ exceptions has been switched off}
@itemdef{wxNO_THREADS,
        if this macro is defined, the compilation options
        don't include compiler flags needed for multithreaded code generation. This
        implies that wxUSE_THREADS is 0 and also that other (non-wx-based) threading
        packages cannot be used neither.}
@itemdef{WXMAKINGDLL_XXX,
        used internally and defined when building the
        library @c XXX as a DLL; when a monolithic wxWidgets build is used only a
        single @c WXMAKINGDLL symbol is defined}
@itemdef{WXUSINGDLL,
        defined when compiling code which uses wxWidgets as a DLL/shared library}
@itemdef{WXBUILDING,
        defined when building wxWidgets itself, whether as a static or shared library}
@endDefList

*/
