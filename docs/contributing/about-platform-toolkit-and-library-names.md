wxWidgets naming conventions
============================

Being a cross platform development library, it is naturally desirable
(at least to me ;) for wxWidgets to be exploited in a fully cross
platform development environment -- a single invocation of make should
be sufficient to build target executables for a variety of host platforms
when desired.

Since this is now in fact possible for at least the most commonly used
platforms, wxWidgets has been structured to allow multiple, simultaneous
installations of the library.  Common files are shared, platform and port
specific files and libraries are arranged so as to be unambiguous when
installed together.

To manage this sanely we need a sufficiently descriptive and logical
labelling convention for file and install path names -- this document (at
least at its time of writing) describes the system we have adopted.

It is not fine grained enough to include every possible build configuration
for wxWidgets, but is encompassing enough to maintain a relatively complete
set of cross platform build tools on a single machine and to provide an
obvious slot for new ports to slip into.


For UNIX libraries, the canonical library name shall be of the form:

	libwx_$(toolkit)$(widgetset)$(debug)-$(version)-$(host).$(lib_extension)

For MSW (native hosted only) libraries the library name should be of
the form:

	wx$(toolkit)$(widgetset)$(version)$(unicode)$(debug).$(lib_extension)


Where:

--------------------------------------------------------------------

`$toolkit` must currently be one of the following:

	msw
	gtk
	base
	mac
	motif

--------------------------------------------------------------------

`$widgetset` may be one of:

	univ

or empty if the widget set is the same as the toolkit.

--------------------------------------------------------------------

`$version` is a string encoding the full version (major, minor, release)
for MSW, or just the major and minor number for UNIX.

eg. for wxWidgets 2.3.2, `$version` = 232 for MSW or 2.3 for UNIX.

The rationale for this is that under UNIX-like systems it is desirable
that differently 'minor numbered' releases can be installed together,
meaning your old 2.2 apps can continue to work even if you migrate
development to the next stable or unstable release (eg.  2.3, 2.4),
but binary compatibility is maintained between point releases (those
with the same major.minor number)

A known break in binary compatibility should be addressed by updating
the library soname (see the notes in configure.in for details on this)

I do not know why MSW should not also omit the release number from
`$version`. (maybe that will change by the time this document is ratified)

--------------------------------------------------------------------

`$unicode` and `$debug` are either empty or set to `'u'` and `'d'`
respectively when enabled.

--------------------------------------------------------------------

`$host` is empty for a 'native' library, (that is one where the host
system is the same as the build system) or set to the value returned
by the autoconf ${host_alias} variable in configure for libraries
that are cross compiled.

--------------------------------------------------------------------

`$lib_extension` is system specific and most usually set to `.a` for
a static library, `.dll` for a MSW shared library, or `.so.$so_version`
for a shared UNIX library.

====================================================================


The installed location of the library specific setup.h is also
determined by the values of these items.  On UNIX systems they
will be found in:

	$(prefix)/lib/wx/include/$(toolkit)$(widgetset)$(debug)-$(version)-$(host)/wx/

which will be in the include search path returned by the relevant
wx-config for that library.  (or presumably set in the relevant
make/project files for platforms that do not use wx-config)

====================================================================


The port specific wx-config file for each library shall be named:

	wx-$(toolkit)$(widgetset)$(debug)-$(version)-$(host)-config

${prefix}/bin/wx-config shall exist as a link to (or copy of) one of
these port specific files (on platforms which support it) and as such
it defines the default build configuration for wxApps on the system.
It may be modified by the system user at any time.
