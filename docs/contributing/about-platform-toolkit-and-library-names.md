## wxWidgets naming conventions

Being a cross platform development library, it is naturally desirable
(at least to me ;) for wxWidgets to be exploited in a fully cross
platform development environment -- a single invocation of make should
be sufficient to build target executables for a variety of host platforms
when desired.

Since this is now in fact possible for at least the most commonly used
platforms, wxWidgets has been structured to allow multiple, simultaneous
installations of the library. Common files are shared, platform and port
specific files and libraries are arranged so as to be unambiguous when
installed together.

To manage this sanely we need a sufficiently descriptive and logical
labelling convention for file and install path names -- this document (at
least at its time of writing) describes the system we have adopted.

It is not fine grained enough to include every possible build configuration
for wxWidgets, but is encompassing enough to maintain a relatively complete
set of cross platform build tools on a single machine and to provide an
obvious slot for new ports to slip into.

The libraries use the following naming convention.
When an item of the name is empty, `'_'` or `'-'` are omitted.

*UNIX libraries* (configure, CMake on Linux/macOS/Cygwin):

    libwx_$(toolkit)$(widgetset)$(unicode)_$(flavour)_$(name)-$(version)-$(host).$(lib_extension)

*Windows libraries* (VS solution, makefile.gcc/vc, CMake on Windows/MinGW, shared libraries on Windows):

    wx$(toolkit)$(widgetset)$(version)$(unicode)$(debug)_$(flavour)_$(name)_$(compiler)_$(arch)_$(vendor).$(lib_extension)


Where:

--------------------------------------------------------------------

`$toolkit` can currently be one of the following:

 - `base`
 - `msw`
 - `gtk`, `gtk2`, `gtk3`, `gtk4`
 - `osx_cocoa`, `osx_iphone`
 - `motif`
 - `x11`
 - `dfb`
 - `qt`

--------------------------------------------------------------------

`$widgetset` may be one of:

 - `univ`

or empty if the widget set is the same as the toolkit.

--------------------------------------------------------------------

`$version` is a string encoding the major and minor version number,
separated by a dot on UNIX and without separator on Windows.
Windows shared libraries of development versions (odd minor releases)
contain the full version (major, minor, release).
On UNIX, the `$so_version` contains the release number.

Eg. for wxWidgets 3.1.5, `$version` is `315` for Windows shared libraries,
`31` for Windows static libraries, and `3.1` for UNIX libraries. And the
`$so_version` for UNIX libraries is `.5` and `.5.0.0`.

The rationale for this is that under UNIX-like systems it is desirable
that differently 'minor numbered' releases can be installed together,
meaning your old 2.2 apps can continue to work even if you migrate
development to the next stable or unstable release (eg. 2.3, 2.4),
but binary compatibility is maintained between point releases (those
with the same major.minor number).

A known break in binary compatibility should be addressed by updating
the library soname (see the notes in configure.in for details on this).

--------------------------------------------------------------------

`$unicode` is set to `'u'` when Unicode is enabled (default on), and is empty
when disabled. In the `setup.h` and `wx-config` names, the full `'unicode'`
name is used.

--------------------------------------------------------------------

`$debug` is set to `'d'` for the libraries using debug version of CRT and is empty
for release libraries. It is only really useful for the libraries created with MSVC
projects and makefiles, as MSVC debug and release CRT are not ABI-compatible,
but is also used by `makefile.gcc` under MSW for consistency with `makefile.vc`.
When using configure under MSW or UNIX, it is always empty.

--------------------------------------------------------------------

`$flavour` is an optional name to identify the build. It is empty by default.

--------------------------------------------------------------------

`$name` is the name of the library. It is empty for the `'base'` library.

--------------------------------------------------------------------

`$compiler` is the used compiler, for example `'vc'` or `'gcc'`.
It is only added to shared libraries on Windows.

--------------------------------------------------------------------

`$arch` is used by MSVC solutions. It is empty for 32-bit builds and
`'x64'` for 64-bit builds. It is only added to shared libraries.

--------------------------------------------------------------------

`$vendor` is an optional name appended to the library name. It is only
added to shared libraries on Windows. It defaults to `'custom'`.

--------------------------------------------------------------------

`$host` is empty for a 'native' library, (that is one where the host
system is the same as the build system) or set to the value returned
by the autoconf ${host_alias} variable in configure for libraries
that are cross compiled.

--------------------------------------------------------------------

`$lib_extension` is system specific and most usually set to `'.a'` for
a static UNIX library, `'.so.$so_version'` for a shared UNIX library,
`'.lib'` for a static MSVC library or `'.dll'` for a shared MSVC library.

--------------------------------------------------------------------

`type` is used to indicate a shared or static build. For MSVC, type is
`'lib'` for shared libraries and `'dll'` for static libraries.
On UNIX, type is empty for shared libraries and `'static'` for static libraries.

--------------------------------------------------------------------

## setup.h

The installed location of the library specific setup.h is also
determined by the values of these items. On UNIX they will be found in:

    $(prefix)/lib/wx/include/$(host)-$(toolkit)$(widgetset)-$(unicode)-$(type)-$(version)-$(flavour)/wx/setup.h

which will be in the include search path returned by the relevant
wx-config for that library (or presumably set in the relevant
make/project files for platforms that do not use wx-config).

For MSVC and gcc/vc makefile, the file is found in:

    $(prefix)/lib/$(compiler)_$(arch)_$(type)/$(toolkit)$(widgetset)$(unicode)$(debug)/wx/setup.h

--------------------------------------------------------------------

## wx-config

The port specific wx-config file for each library shall be named:

    $(prefix)/lib/wx/config/$(host)-$(toolkit)$(widgetset)-$(unicode)-$(type)-$(version)-$(flavour)

${prefix}/bin/wx-config shall exist as a link to (or copy of) one of
these port specific files (on platforms which support it) and as such
it defines the default build configuration for wxApps on the system.
It may be modified by the system user at any time.
