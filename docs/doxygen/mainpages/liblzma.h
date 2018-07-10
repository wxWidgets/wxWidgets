/////////////////////////////////////////////////////////////////////////////
// Name:        liblzma.h
// Purpose:     Documentation of the use of liblzma with wxWidgets
// Author:      Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

/**

@page page_build_liblzma Using LZMA compression with liblzma

To use wxLZMAInputStream and wxLZMAOutputStream classes, a public domain
liblzma library is required when building wxWidgets. This guide explains how to
make liblzma available and let wxWidgets build system use it. The exact steps
depend on the operating system and compiler used, please refer to the
section appropriate for your environment below.

@section page_build_liblzma_unix Unix-like operating system (including Cygwin)

Under many Unix or Unix-like (including Cygwin) systems, liblzma is available
as a system package and the simplest thing to do is to just install it using
the system-specific tool (apt, yum, ...). Just note that you need to install
the liblzma-dev or liblzma-devel package in order to obtain the library headers
and not just the library itself.

If your system doesn't provide such package or you need to use a newer version
than is available in the system repositories, it is also simple to build
liblzma from sources: basically, just download the library sources from
https://tukaani.org/xz/ or clone its repository and perform the usual

@code
    $ ./configure
    $ make
    $ make install
@endcode

commands sequence.

As wxWidgets is built using either configure or CMake under Unix-like systems,
there is nothing special to do to let wxWidgets use liblzma, as it will be
detected automatically -- just check the end of configure or CMake output to
confirm that it was found. If this is not the case because the library was
installed in some non-default location, you need to provide @c CPPFLAGS and @c
LDFLAGS on configure command line, e.g.:

@code
    $ ../configure CPPFLAGS=-I/opt/liblzma/include LDFLAGS=-L/opt/liblzma/lib
@endcode


@section page_build_liblzma_msw MSW not using configure

When not using configure, you must edit @c wx/msw/setup.h file and change the
value of @c wxUSE_LIBLZMA option to @c 1 manually in order to enable LZMA
support, which is disabled by default.

Next, you need to actually ensure that the library is available to be able to
build wxWidgets:

@subsection page_build_liblzma_msw_vcpkg MSW using MSVS 2015 with Vcpkg

If you build wxWidgets with Microsoft Visual Studio 2015.3 or later, you can
use Microsoft vcpkg tool (see https://github.com/Microsoft/vcpkg) to install
liblzma as any other library and integrate it with Microsoft Visual Studio
automatically. Please see vcpkg documentation for details, but, basically, it's
as simple as executing the following commands:

@code
    > git clone https://github.com/Microsoft/vcpkg.git
    > cd vcpkg
    > .\bootstrap-vcpkg.bat
    > .\vcpkg integrate install
    > .\vcpkg install liblzma
@endcode

Afterwards, liblzma headers and libraries (in DLL form) will be available to
all C++ Visual Studio projects, including wxWidgets, so you just need to build
it as usual, i.e. by opening @c build/msw/wx_vcXX.sln file and building it.


@section page_build_liblzma_msw_manual MSW manual setup

First of all, you need to either build or download liblzma binaries. Building
the library from source with gcc requires configure, and as you don't use an
environment capable of running it (if you're, you're reading the wrong section
of this guide, please go to the first one instead), it might be simpler to just
download the binaries from https://tukaani.org/xz/

However these binaries don't provide import libraries for MSVC, so you may
prefer building from source when using this compiler, provided it is at least
2013.2 or later (otherwise, you must use the binaries and create the import
library from the DLL). To do it, simply open the MSVS solution provided under
@c windows directory of the source archive corresponding to your compiler
version and build all the relevant configurations. Notice that you may build it
as either a static or a dynamic library, but if you choose the former, you must
also predefine @c LZMA_API_STATIC when building wxWidgets.


Second, you need to let wxWidgets build system know about the liblzma headers
and libraries location. When using makefiles, with either gcc or MSVC compiler,
this can be done simply by specifying @c CPPFLAGS and @c LDFLAGS on make
command line, e.g.

@code
    > make -f makefile.gcc CPPFLAGS=-Ic:/dev/include LDFLAGS=-Lc:/dev/lib
@endcode

If you built liblzma as a static library, change @c CPPFLAGS to also include @c
-DLZMA_API_STATIC.


When building wxWidgets with MSVC project files, you must update them to use
liblzma. For this, start by copying build\\msw\\wx_setup.props file to
build\\msw\\wx_local.props, if you don't have the latter file yet. Then open
this file in your favourite editor and locate the @c Link tag inside an @c
ItemDefinitionGroup tag and add a new @c AdditionalIncludeDirectories line
inside the @c ClCompile tag as well as add @c liblzma.lib to the @c
AdditionalDependencies tag under @c Link itself. The result should look like
the following, assuming liblzma sources are in c:\\src\\liblzma:

@code
  <ItemDefinitionGroup>
    <ClCompile>
      ... any existing lines here ...
      <AdditionalIncludeDirectories>c:\src\liblzma\src\liblzma\api;%(AdditionalIncludeDirectories)</AdditionalIncludeDirectories>
      <!--
        Uncomment this line if, and only if, you link with liblzma statically.
      <PreprocessorDefinitions>LZMA_API_STATIC;%(PreprocessorDefinitions)</PreprocessorDefinitions>
        -->
    </ClCompile>
    <Link>
      <AdditionalDependencies>... all the previously listed libraries;c:\src\liblzma\windows\vs2017\$(Configuration)\$(Platform)\liblzma_dll\liblzma.lib;%(AdditionalDependencies)</AdditionalDependencies>
    </Link>
  </ItemDefinitionGroup>
@endcode

Afterwards, build wxWidgets as usual and don't forget to provide @c liblzma.dll
when distributing your application unless you have decided to link it
statically.
 */
