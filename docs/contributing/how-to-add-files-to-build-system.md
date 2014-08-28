How to add new files and libraries to wxWidgets build system
============================================================


Regenerating makefiles
----------------------

wxWidgets now uses [Bakefile](http://bakefile.sourceforge.net) to generate
native makefiles. You must have bakefile installed if you want to regenerate
the makefiles. Bakefile currently runs on Unix and Windows systems. You will
need Python 2.x installed on Unix and either use Bakefile installer or have
Python on Windows.

Once you have installed Bakefile, you can easily regenerate the makefiles using
the bakefile_gen tool. Run it from $(wx)/build/bakefiles directory and it will
regenerate all outdated makefiles. See $(wx)/build/bakefiles/README for more
details.

Note that it generates makefiles for samples, too.

IMPORTANT NOTE: Don't forget to run autoconf in wxWidgets root directory
                (after running Bakefile) if you changed any conditional
                variable or target condition in .bkl files! You will know that
                this happened if $(wx)/autoconf_inc.m4 content changed.

You can use Bakefile to generate makefiles or projects customized to your
needs, too. See Bakefiles.bkgen for details on bakefile commands used to
generate makefiles. For example, you can use this command to generate
VC++ project files without wxUniversal configurations:

    bakefile -v -fmsvc6prj -o../wxmy.dsw -DRUNTIME_LIBS=dynamic
             -DDEBUG_INFO=default -DDEBUG_FLAG=default
             -DOFFICIAL_BUILD=0 -DUSE_HTML=1 -DUSE_OPENGL=1
             -DMONOLITHIC=0 -DUSE_GUI=1 -DWXUNIV=0 wx.bkl

Or monolithic instead of multilib (the default):

    bakefile -v -fmsvc6prj -o../wxmono.dsw -DRUNTIME_LIBS=dynamic
             -DDEBUG_INFO=default -DDEBUG_FLAG=default
             -DOFFICIAL_BUILD=0 -DUSE_HTML=1 -DUSE_OPENGL=1
             -DMONOLITHIC=1 -DUSE_GUI=1 wx.bkl

Or monolithic wxBase:

    bakefile -v -fmsvc6prj -o../wxBase.dsw -DRUNTIME_LIBS=dynamic
             -DDEBUG_INFO=default -DDEBUG_FLAG=default
             -DOFFICIAL_BUILD=0 -DUSE_HTML=0 -DUSE_OPENGL=0
             -DMONOLITHIC=1 -DUSE_GUI=0 wx.bkl

It is, however, recommended to modify Bakefiles.bkgen (or
Bakefiles.local.bkgen) by means of `<add-flags>` and `<del-flags>` directives
and use bakefile_gen instead of running bakefile directly.


Bakefile files organization
---------------------------

Makefile are generated from .bkl files ("bakefiles") from two places:

  - $(wx)/build/bakefiles directory
  - samples directories

$(wx)/build/bakefiles contains bakefiles for main library and support files
that simplify writing bakefiles for samples.

Support files are:

  - wxwin.py                - helper functions
  - common.bkl
  - common_samples.bkl
  - config.bkl              - user-configurable build options
  - make_dist.mk            - implementation of "make dist" on Unix

Files used to build the library are:

  - wx.bkl                  - main file
  - files.bkl               - lists of source files
  - monolithic.bkl          - targets for wxWin built as single big library
  - multilib.bkl            - targets for multilib build
  - opengl.bkl              - GL library with wxGLCanvas (this one is not
                            included in monolithic library for historical
                            reasons, so "monolithic" really means "two libs")
  - {expat,jpeg,png,tiff, regex,zlib,odbc}.bkl - 3rd party libraries makefiles


Adding files to existing library
--------------------------------

**UPDATE:** files.bkl is now itself partially generated from the master file
        build/files. If the variable which you need to modify, according to the
        instructions below, is already defined in build/files, update it there
        and run build/upmake to update files.bkl.


All files used by main libraries are listed in files.bkl. The file is
organized into variables for toolkits, platforms and libraries. The variables
come in pairs: there's always `FOO_SRC` for source files and `FOO_HDR` for header
files. Platform or toolkit specific files are grouped together in variable
with platform or toolkit name in them, e.g. `BASE_WIN32_SRC`, `BASE_UNIX_SRC`,
`GTK_SRC`, `MOTIF_SRC`.

Note: A side effect of this toolkit-centric organization is that one file may
      be present several times in files.bkl in different contexts.

When you are adding a file, you must put it into appropriate variable. This is
easy if you are adding the file to library that is always built from same
sources on all platforms (e.g. wxXml or wxXML) -- simply add the file to e.g.
`HTML_SRC` or `HTML_HDR`.

If the file is used only on one platform and is part of wxBase, add it to
`BASE_{platform}_SRC/HDR`. If it is used on all platforms, add it to `BASE_CMN`.
If it is built on more than one platform but not on all of them, add the file
to *all* platforms that use it!

If a file is not wxBase file, but GUI file, then the variables are named after
toolkits/ports, not platforms. Same rules as for wxBase files apply
(substitute "platform" with "toolkit"). Make sure you correctly choose between
`{port}_LOWLEVEL_SRC` and `{port}_SRC` -- the former is for files used by
wxUniversal, e.g. GDI classes. Files shared by all X Window System ports
should be put into `XWIN_LOWLEVEL_SRC`.


Adding sample
-------------

Copy the bakefile from another sample, change the ID and files accordingly.
If the sample uses some data files, make sure to have `<wx-data>` node
in the sample's bakefile (see e.g. samples/image/image.bkl for an example).
Make sure to add `<wx-lib>` statements for all libraries from multilib build
that are required by the sample.

The Windows resource specification should use the central .rc file:

        <win32-res>../sample.rc</win32-res>

Run bakefile_gen in $(wx)/build/bakefiles to regenerate the bakefiles.

Finally commit $(wx)/build/bakefiles/make_dist.mk and all the other modified files.

Currently we commit the generated makefiles except .dms, .vcp.


Adding new core library
-----------------------

When adding new library to the core set of libraries, the files must be
added to both a newly added library in multilib build and into the single
library built in monolithic mode. We will assume that the new library is
called wxFoo.

a) Add files to files.bkl:
   * If wxFoo builds from same files on all platforms (e.g. wxNet),
     add `FOO_SRC` and `FOO_HDR` variables with lists of sources and headers.
   * If wxFoo have no files in common (e.g. wxGL), add `FOO_SRC` and `FOO_HDR`
     with toolkit or platform conditions. Have a look at `OPENGL_SRC` for an
     example.
   * Otherwise add `FOO_CMN_SRC` and `FOO_CMN_HDR` for common files and
     `FOO_{platform}_{SRC,HDR}` or `FOO_{toolkit}_{SRC,HDR}` as appropriate. Add
     `FOO_PLATFORM_{SRC,HDR}` into "Define sources for specific libraries"
     section that is conditionally set to one of `FOO_xxx_{SRC,HDR}` based on
     target platform/toolkit (see `NET_PLATFORM_SRC` definition for an example).
     Finally, define `FOO_SRC` and `FOO_HDR` to contain both
     `FOO_PLATFORM_{SRC,HDR}` and `FOO_{SRC,HDR}` (see `NET_SRC` definition for an
     example).
   * Add `FOO_HDR` to `ALL_GUI_HEADERS` or `ALL_BASE_HEADERS`.
   * If wxFoo is wxBase library (doesn't use GUI), add `FOO_SRC` to
     `ALL_BASE_SOURCES`.

   (You can apply different approaches to HDR and SRC variables, if e.g.
   headers are all common but sources are not.)

   Note that the conditions can only test for equality, due to limitations of
   native make tools.

b) Modify bakefile system in build/bakefiles/ to recognize wxFoo:
   * Add 'foo' to `MAIN_LIBS` and `LIBS_NOGUI` or `LIBS_GUI` (depending on whether
     the library depends on wxCore or not) to wxwin.py file.
   * Add extra libraries needed by wxFoo (if any) to EXTRALIBS in wxwin.py
   * Add `WXLIB_FOO` definition to common.bkl (into the "Names of component
     libraries" section). It looks like this:

       <set var="WXLIB_FOO">
          <if cond="MONOLITHIC=='0'">$(mk.evalExpr(wxwin.mkLibName('foo')))</if>
       </set>

c) Modify monolithic.bkl to add files to monolithic build: it's enough to add
   `FOO_SRC` to `MONOLIB_GUI_SRC` or `MONOLIB_SRC`, depending on whether wxFoo uses
   GUI or not.

d) Modify multilib.bkl to add files to multilib build: add foolib and foodll
   targets. Don't use wxBase targets as the template, use e.g. wxXML or wxHTML.
   Make sure `WXMAKINGDLL_FOO` is defined in foodll.

e) Regenerate all makefiles (don't forget to run autoconf)

f) Update configure.in and wx-config.in to contain information about
   the library and needed linker flags:
   * Add "foo" to `BUILT_WX_LIBS` in configure.in.
   * If appropriate, but it rarely is, so normally this should _not_ be done,
     add "foo" to either `STD_BASE_LIBS` or `STD_GUI_LIBS` in configure.in.
   * If wxFoo links against additional libraries, add necessary linker
     flags and libraries to ldflags_foo and ldlibs_foo variables in
     wx-config.in (both are optional).

g) Update dlimpexp.h to define `WXMAKINGDLL_FOO` if `WXMAKINGDLL` is defined (add
   `#define WXMAKINGDLL_FOO` inside first `#ifdef WXMAKINGDLL` block in
   dlimpexp.h) and to define `WXDLLIMPEXP_FOO` and `WXDLLIMPEXP_DATA_FOO`. You
   can copy e.g. `WXDLLIMPEXP_NET` definition, it is something like this:

        #ifdef WXMAKINGDLL_NET
            #define WXDLLIMPEXP_NET WXEXPORT
            #define WXDLLIMPEXP_DATA_NET(type) WXEXPORT type
        #elif defined(WXUSINGDLL)
            #define WXDLLIMPEXP_NET WXIMPORT
            #define WXDLLIMPEXP_DATA_NET(type) WXIMPORT type
        #else // not making nor using DLL
            #define WXDLLIMPEXP_NET
            #define WXDLLIMPEXP_DATA_NET(type) type
        #endif

   Don't forget to add `WXDLLIMPEXP_FWD_FOO` definitions too.
   Use `WXDLLIMPEXP_FOO` when declaring wxFoo classes and functions.

h) Add this code to one of wxFoo's files (the more often used, the better):

      // DLL options compatibility check:
      #include "wx/app.h"
      WX_CHECK_BUILD_OPTIONS("wxFoo")

i) Add information about wxFoo to the manual ("Libraries list" section
   in libs.tex) and update docs/latex/wx/libs.dia (you need Dia for this)
   to show the dependencies of the new library.

j) Also please add `#pragma comment(lib, "foo")` (for all build configuration
   combinations) to the file include/msvc/wx/setup.h and
   add a check for `WXMAKINGDLL_FOO` to the test whether we're building a DLL at
   the end of include/wx/msw/chkconf.h.
