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
@itemdef{\__WXBASE__, Only wxBase, no GUI features (same as @c wxUSE_GUI == 0)}
@itemdef{\__WXDFB__, wxUniversal using DirectFB}
@itemdef{\__WXGTK__, GTK+}
@itemdef{\__WXGTK127__, GTK+ 1.2.7 or higher (not used any longer)}
@itemdef{\__WXGTK20__, GTK+ 2.0 (2.6) or higher}
@itemdef{\__WXGTK210__, GTK+ 2.10 or higher}
@itemdef{\__WXGTK218__, GTK+ 2.18 or higher}
@itemdef{\__WXGTK220__, GTK+ 2.20 or higher}
@itemdef{\__WXMAC__, old define, same as <tt>\__WXOSX__</tt>}
@itemdef{\__WXMOTIF__, Motif (not used any longer).}
@itemdef{\__WXMOTIF20__, Motif 2.0 or higher (not used any longer).}
@itemdef{\__WXMSW__, GUI using <a href="http://en.wikipedia.org/wiki/Windows_User">Windows Controls</a>.
Notice that for compatibility reasons, this symbol is defined for console
applications under Windows as well, but it should only be used in the GUI code
while <tt>\__WINDOWS__</tt> should be used for the platform tests.}
@itemdef{\__WXOSX__, OS X GUI using any Apple widget framework (AppKit or UIKit)}
@itemdef{\__WXOSX_IPHONE__, iPhone (UIKit)}
@itemdef{\__WXOSX_COCOA__, macOS using Cocoa (AppKit)}
@itemdef{\__WXOSX_MAC__, macOS (Cocoa)}
@itemdef{\__WXPM__, OS/2 native Presentation Manager (not used any longer).}
@itemdef{\__WXXT__, Xt; mutually exclusive with WX_MOTIF, not implemented in wxWidgets 2.x}
@itemdef{\__WXX11__, wxX11 (<tt>\__WXUNIVERSAL__</tt> will be also defined)}
@itemdef{\__WXWINE__, WINE (i.e. WIN32 on Unix)}
@itemdef{\__WXUNIVERSAL__, wxUniversal port, always defined in addition
                        to one of the symbols above so this should be tested first.}
@itemdef{\__X__, any X11-based GUI toolkit except GTK+}
@endDefList

wxOSX is the successor of the venerable wxMac, it currently exists in two
versions: Cocoa for the desktop and a very early iPhone port. To summarize:

@li If you want to test for wxOSX on the desktop, use
    <tt>\__WXOSX_MAC__</tt>.
@li If you want to test for wxOSX on the iPhone, use
    <tt>\__WXOSX_IPHONE__</tt>.
@li If you want to test for any port under macOS, including, for
    example, wxGTK and also wxBase, use <tt>\__DARWIN__</tt> (see below).

The convention is to use the <tt>__WX</tt> prefix for these
symbols, although this has not always been followed.



@section page_cppconst_os Operating Systems

@beginDefList
@itemdef{\__APPLE__, any Mac OS version}
@itemdef{\__AIX__, AIX}
@itemdef{\__BSD__, Any *BSD system}
@itemdef{\__CYGWIN__, Cygwin: Unix on Win32}
@itemdef{\__DARWIN__, OS X (with BSD C library), using any port (see also <tt>\__WXOSX__</tt>)}
@itemdef{\__DATA_GENERAL__, DG-UX}
@itemdef{\__FREEBSD__, FreeBSD}
@itemdef{\__HPUX__, HP-UX (Unix)}
@itemdef{\__GNU__, GNU Hurd}
@itemdef{\__LINUX__, Linux}
@itemdef{\__MACH__, Mach-O Architecture (OS X only builds)}
@itemdef{\__OSF__, OSF/1}
@itemdef{\__QNX__, QNX Neutrino RTOS}
@itemdef{\__SGI__, IRIX}
@itemdef{\__SOLARIS__, Solaris}
@itemdef{\__SUN__, Any Sun}
@itemdef{\__SUNOS__, Sun OS}
@itemdef{\__SVR4__, SystemV R4}
@itemdef{\__SYSV__, SystemV generic}
@itemdef{\__ULTRIX__, Ultrix}
@itemdef{\__UNIX__, any Unix}
@itemdef{\__UNIX_LIKE__, Unix, BeOS or VMS}
@itemdef{\__VMS__, VMS}
@itemdef{\__WINDOWS__, Any Windows platform, using any port (see also <tt>\__WXMSW__</tt>)}
@itemdef{\__WIN16__, Win16 API (not supported since wxWidgets 2.6)}
@itemdef{\__WIN32__, Win32 API}
@itemdef{\__WIN64__, Win64 (mostly same as Win32 but data type sizes are different)}
@itemdef{\__WINE__, Wine}
@endDefList



@section page_cppconst_cpu Hardware Architectures (CPU)

Note that not all of these symbols are always defined, it depends on the
compiler used.

@beginDefList
@itemdef{\__ALPHA__, DEC Alpha architecture}
@itemdef{\__INTEL__, Intel i386 or compatible}
@itemdef{\__IA64__, Intel 64 bit architecture}
@itemdef{\__POWERPC__, Motorola Power PC}
@endDefList



@section page_cppconst_compiler Compilers

@beginDefList
@itemdef{\__BORLANDC__, Borland C++. The value of the macro corresponds
                        to the compiler version: 500 is 5.0 (not used any more).}
@itemdef{\__DIGITALMARS__, Digital Mars (not used any more).}
@itemdef{\__GNUG__, Gnu C++ on any platform, see also wxCHECK_GCC_VERSION}
@itemdef{\__GNUWIN32__, Gnu-Win32 compiler, see also wxCHECK_W32API_VERSION}
@itemdef{\__INTELC__, Intel C++ compiler}
@itemdef{\__MINGW32__, Either MinGW32 or MinGW-w64 in either 32 or 64 bits}
@itemdef{__MINGW32_TOOLCHAIN, MinGW32 only (32 bits only right now)}
@itemdef{\__MINGW64__, MinGW-w64 in 64 bit builds}
@itemdef{\__MINGW64_TOOLCHAIN__, MinGW-w64 in either 32 or 64 bit builds}
@itemdef{\__SUNCC__, Sun CC, see also wxCHECK_SUNCC_VERSION}
@itemdef{\__SYMANTECC__, Symantec C++ (not used any more).}
@itemdef{\__VISAGECPP__, IBM Visual Age (OS/2) (not used any more).}
@itemdef{\__VISUALC__, Microsoft Visual C++, see also ::wxCHECK_VISUALC_VERSION.
                    The value of this macro corresponds to the compiler version:
                    @c 1020 for @c 4.2 (the first supported version), @c 1100 for
                    @c 5.0, @c 1200 for @c 6.0 and so on. For convenience, the symbols
                    \__VISUALCn__ are also defined for each major compiler version from
                    5 to 12, i.e. you can use tests such as <tt>\#ifdef \_\_VISUALC7\_\_</tt>
                    to test for compiler version being precisely 7.}
@itemdef{\__XLC__, AIX compiler}
@itemdef{\__WATCOMC__, Watcom C++. The value of this macro corresponds to
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
@itemdef{wxHAS_DEPRECATED_ATTR, Defined if C++14 @c [[deprecated]] attribute is
    supported (this symbol only exists in wxWidgets 3.1.6 or later).}
@itemdef{wxHAS_DPI_INDEPENDENT_PIXELS, Defined if pixel coordinates on the
    current platform scale with DPI, i.e. if the given length in pixels has the
    same apparent size on the display independently of the DPI (this symbol
    only exists in wxWidgets 3.1.6 or later). Note that it should rarely, if
    ever, be necessary to use this symbol directly, functions such as
    wxWindow::FromDIP() and wxBitmap::GetLogicalSize() exist to hide the
    differences between the platforms with and without DPI-independent pixels.}
@itemdef{wxHAS_IMAGE_RESOURCES, Defined if wxICON() and wxBITMAP() macros use
    images from (Windows) resources. Otherwise, these macros use XPMs.}
@itemdef{wxHAS_MEMBER_DEFAULT, Defined if the currently used compiler supports
    C++11 @c =default.}
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
@itemdef{wxHAS_NATIVE_ANIMATIONCTRL, Defined if native wxAnimationCtrl class is being used (this symbol only exists in wxWidgets 3.1.4 and later).}
@itemdef{wxHAS_NATIVE_DATAVIEWCTRL, Defined if native wxDataViewCtrl class is being used (this symbol only exists in wxWidgets 3.1.4 and later).}
@itemdef{wxHAS_NATIVE_WINDOW, Defined if wxNativeWindow class is available.}
@itemdef{wxHAS_NOEXCEPT, This symbol exists only for compatibility and is always defined now.}
@itemdef{wxHAS_NULLPTR_T, This symbol exists only for compatibility and is always defined now.}
@itemdef{wxHAS_IMAGE_RESOURCES, Defined if images can be embedded into the
    program as resources, i.e. without being defined in the program text
    itself. This is currently the case for MSW and Mac platforms. This constant
    is available since wxWidgets 3.1.6.}
@itemdef{wxHAS_IMAGES_IN_RESOURCES, Defined if <a href="http://en.wikipedia.org/wiki/Resource_(Windows)">
    Windows resource files</a> resource files are available on the current platform.
    Usually wxHAS_IMAGE_RESOURCES should be used instead.}
@itemdef{wxHAS_POWER_EVENTS, Defined if wxPowerEvent are ever generated on the current platform.}
@itemdef{wxHAS_RADIO_MENU_ITEMS,
        Defined if the current port supports radio menu items (see wxMenu::AppendRadioItem).}
@itemdef{wxHAS_RAW_BITMAP, Defined if direct access to bitmap data using the classes in @c wx/rawbmp.h is supported.}
@itemdef{wxHAS_RAW_KEY_CODES, Defined if raw key codes (see wxKeyEvent::GetRawKeyCode are supported.}
@itemdef{wxHAS_REGEX_ADVANCED, Defined if advanced syntax is available in
    wxRegEx. This is always the case in wxWidgets 3.1.6 and later, so this
    symbol doesn't need to be tested any more.}
@itemdef{wxHAS_SVG, Defined if SVG support (currently only via wxBitmapBundle::FromSVG()) is available.}
@itemdef{wxHAS_TASK_BAR_ICON, Defined if wxTaskBarIcon is available on the current platform.}
@itemdef{wxHAS_WINDOW_LABEL_IN_STATIC_BOX, Defined if wxStaticBox::Create()
    overload taking @c wxWindow* instead of the text label is available on the current platform.}
@itemdef{wxHAS_MODE_T, Defined when wxWidgets defines @c mode_t typedef for the
    compilers not providing it. If another library used in a wxWidgets
    application, such as ACE (http://www.cs.wustl.edu/~schmidt/ACE.html), also
    defines @c mode_t, this symbol can be predefined after including the other
    library header, such as @c "ace/os_include/sys/os_types.h" in ACE case,
    but before including any wxWidgets headers, to prevent a definition
    conflict.}
@endDefList



@section page_cppconst_msvc_setup_h MSVC-specific Symbols

Microsoft Visual C++ users may use the special @c wx/setup.h file for this
compiler in @c include/msvc subdirectory. This file implicitly links in all the
wxWidgets libraries using MSVC-specific pragmas which usually is much more
convenient than manually specifying the libraries list in all of the project
configurations.

By default, the pragmas used in this file to actually link with wxWidgets
libraries suppose that the libraries are located in @c vc_lib or @c vc_dll
directory which is used by default. However when using multiple MSVC versions,
or when using the @ref plat_msw_binaries "official binaries", the libraries are
in a directory containing the compiler version number, e.g. @c vc140_dll. To
make linking work in this case, you must predefine @c wxMSVC_VERSION as @c
vc140 <em>before</em> include @c wx/setup.h file, i.e. typically in the MSVS
project options. Alternatively, you can predefine @c wxMSVC_VERSION_AUTO symbol
(without any value), which means that the appropriate compiler version should
be used automatically, e.g. "vc140" for MSVC 14.0 (MSVS 2015), "vc141" for MSVC
14.1 (MSVS 2017), "vc142" for MSVC 14.2 (MSVS 2019) and "vc143" for MSVC 14.3
(MSVS 2022).

Finally, there is also @c wxMSVC_VERSION_ABI_COMPAT symbol which can be
predefined to use the "vc14x" prefix ("x" is the literal letter "x" here and
not just a placeholder). This allows building the libraries with any of MSVC
14.x versions, that are ABI-compatible with each other, and using them when
using any later version.

If the makefiles have been used to build the libraries from source and the @c CFG
variable has been set to specify a different output path for that particular
configuration of build then the @c wxCFG preprocessor symbol should be set in
the project that uses wxWidgets to the same value as the @c CFG variable in
order for the correct @c wx/setup.h file to automatically be included for that
configuration.


@subsection page_cppconst_msvc_setup_h_no_libs Library Selection for MSVC

As explained above, MSVC users don't need to explicitly specify wxWidgets
libraries to link with, as this is done by @c wx/setup.h. However sometimes
linking with all the libraries, as is done by default, is not desirable, for
example because some of them were not built and this is where the symbols in
this section can be helpful: defining them allows to not link with the
corresponding library. The following symbols are honoured:

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


@section page_cppconst_compatibility Compatibility Macros

wxWidgets always tries to preserve source backwards compatibility, however
sometimes existing symbols may need to be removed. Except in exceedingly rare
cases, this happens in several steps: first, the symbol is marked as
deprecated, so that using it results in a warning when using the supported
compilers in some wxWidgets release @c x.y. It can still be used, however
the warnings indicate all the places in your code which will need to be updated
in the future. If your code doesn't use any deprecated symbols or you have already
fixed all their occurrences, you may change @c WXWIN_COMPATIBILITY_x_y to 0
to ensure they can't be used -- however its default value is still 1 at this time.

At some point in the future, the next stable wxWidgets release @c x.y+2 changes
the default @c WXWIN_COMPATIBILITY_x_y value to 0, meaning that now the symbol
becomes unavailable by default and if you still want to be able to compile the
code using it, you need to explicitly change @c WXWIN_COMPATIBILITY_x_y to 1
when building the library.

And, finally, the symbol is completely removed from the library in the next
stable version after this, i.e. @c x.y+4. @c WXWIN_COMPATIBILITY_x_y itself is
removed as well at this time, as it is not useful any longer.

According to this general rule, currently, i.e. in wxWidgets 3.4, the following
two symbols are defined: @c WXWIN_COMPATIBILITY_3_0, as 0, and @c
WXWIN_COMPATIBILITY_3_2, as 1. Please see @ref overview_backwardcompat for even
more details.

@beginDefList
@itemdef{WXWIN_COMPATIBILITY_3_0,
         defined as 0 by default meaning that symbols existing in wxWidgets 3.0
         but deprecated in 3.2 release are not available by default. It can be
         changed to 1 to make them available, but it is strongly recommended to
         update the code using them instead.}
@itemdef{WXWIN_COMPATIBILITY_3_2,
         defined as 1 by default meaning that symbols existing in wxWidgets 3.2
         but deprecated since then are still available. It can be changed to 0
         to ensure that no deprecated symbols are used accidentally.}
@itemdef{wxDIALOG_UNIT_COMPATIBILITY,
         wxMSW-specific setting which can be set to 1 to make
         wxWindow::GetCharWidth() and wxWindow::GetCharHeight() more compatible
         with old wxWidgets versions. Changing it is not recommended.}
@itemdef{wxUSE_UNSAFE_WXSTRING_CONV,
         this option determines if unsafe implicit conversions of wxString to
         @c char* or @c std::string (depending on whether @c wxUSE_STL is 0 or
         1) are defined. It is set to 1 by default for compatibility reasons,
         however it is recommended to set it to 0 for the new projects. See
         also @c wxNO_UNSAFE_WXSTRING_CONV below for an alternative way of
         disabling these unsafe conversions not requiring rebuilding the
         library.}
@endDefList

@section page_cppconst_miscellaneous Miscellaneous

@beginDefList
@itemdef{\__WXWINDOWS__,
        always defined in wxWidgets applications, see also wxCHECK_VERSION}
@itemdef{wxDEBUG_LEVEL, defined as 1 by default, may be pre-defined as 0 before
        including wxWidgets headers to disable generation of any code at all
        for the assertion macros, see @ref overview_debugging}
@itemdef{\__WXDEBUG__, defined if wxDEBUG_LEVEL is 1 or more, undefined otherwise}
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
        @c wx/wxprec.h} and then, inside <tt>\#ifndef WX_PRECOMP</tt>, individual
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
        packages cannot be used either.}
@itemdef{wxNO_UNSAFE_WXSTRING_CONV,
        this symbol is not defined by wxWidgets itself, but can be defined by
        the applications using the library to disable unsafe implicit
        conversions in wxString class. This is especially useful when using
        standard build of the library, e.g. installed by the system package
        manager under Unix, which is compiled with @c wxUSE_UNSAFE_WXSTRING_CONV
        set to 1 for compatibility reasons as @c -DwxNO_UNSAFE_WXSTRING_CONV
        can be used only compiling the application code, without rebuilding the
        library. Support for this option appeared in wxWidgets 3.1.1.}
@itemdef{wxNO_IMPLICIT_WXSTRING_ENCODING,
        this symbol is not defined by wxWidgets itself, but can be defined by
        the applications using the library to disable implicit
        conversions from and to <tt>const char*</tt> in wxString class.
        Support for this option appeared in wxWidgets 3.1.4.}
@itemdef{WXMAKINGDLL_XXX,
        used internally and defined when building the
        library @c XXX as a DLL; when a monolithic wxWidgets build is used only a
        single @c WXMAKINGDLL symbol is defined}
@itemdef{WXUSINGDLL,
        defined when compiling code which uses wxWidgets as a DLL/shared library}
@itemdef{WXBUILDING,
        defined when building wxWidgets itself, whether as a static or shared library}
@itemdef{wxICON_IS_BITMAP,
         defined in the ports where wxIcon inherits from wxBitmap (all but
         wxMSW currently)}
@endDefList

*/
